/*
 * Copyright 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * FreeRTOS OTA PAL for ESP32-DevKitC ESP-WROVER-KIT V1.0.4
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.*/

/* OTA PAL implementation for Espressif esp32_devkitc_esp_wrover_kit platform */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ota.h"
#include "ota_pal.h"
#include "ota_interface_private.h"

/* OTA_DO_NOT_USE_CUSTOM_CONFIG allows building the OTA library
 * without a custom config. If a custom config is provided, the
 * OTA_DO_NOT_USE_CUSTOM_CONFIG macro should not be defined. */
#ifndef OTA_DO_NOT_USE_CUSTOM_CONFIG
    #include "ota_config.h"
#endif

/* Include config defaults header to get default values of configs not defined
 * in ota_config.h file. */
#include "ota_config_defaults.h"

#include "types/iot_network_types.h"
#include "aws_iot_network_config.h"
#include "iot_crypto.h"
#include "core_pkcs11.h"
#include "esp_system.h"
#include "esp_log.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_wdt.h"

#include "esp_partition.h"
#include "esp_spi_flash.h"
#include "esp_image_format.h"
#include "esp_ota_ops.h"
#include "aws_esp_ota_ops.h"
#include "mbedtls/asn1.h"
#include "mbedtls/bignum.h"
#include "mbedtls/base64.h"
#include "task.h"

#define kOTA_HalfSecondDelay    pdMS_TO_TICKS( 500UL )
#define ECDSA_INTEGER_LEN       32

/* Check configuration for memory constraints provided SPIRAM is not enabled */
#if !CONFIG_SPIRAM_SUPPORT
    #if ( configENABLED_DATA_PROTOCOLS & OTA_DATA_OVER_HTTP ) && ( configENABLED_NETWORKS & AWSIOT_NETWORK_TYPE_BLE )
        #error "Cannot enable OTA data over HTTP together with BLE because of not enough heap."
    #endif
#endif /* !CONFIG_SPIRAM_SUPPORT */

/*
 * Includes 4 bytes of version field, followed by 64 bytes of signature
 * (Rest 12 bytes for padding to make it 16 byte aligned for flash encryption)
 */
#define ECDSA_SIG_SIZE    80

typedef struct
{
    const esp_partition_t * update_partition;
    const OtaFileContext_t * cur_ota;
    esp_ota_handle_t update_handle;
    uint32_t data_write_len;
    bool valid_image;
} esp_ota_context_t;

typedef struct
{
    uint8_t sec_ver[ 4 ];
    uint8_t raw_ecdsa_sig[ 64 ];
    uint8_t pad[ 12 ];
} esp_sec_boot_sig_t;

static esp_ota_context_t ota_ctx;
static const char * TAG = "ota_pal";

/* Specify the OTA signature algorithm we support on this platform. */
const char OTA_JsonFileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";

static CK_RV prvGetCertificateHandle( CK_FUNCTION_LIST_PTR pxFunctionList,
                                      CK_SESSION_HANDLE xSession,
                                      const char * pcLabelName,
                                      CK_OBJECT_HANDLE_PTR pxCertHandle );
static CK_RV prvGetCertificate( const char * pcLabelName,
                                uint8_t ** ppucData,
                                uint32_t * pulDataSize );

static OtaErr_t asn1_to_raw_ecdsa( uint8_t * signature,
                                   uint16_t sig_len,
                                   uint8_t * out_signature )
{
    int ret = 0;
    const unsigned char * end = signature + sig_len;
    size_t len;
    mbedtls_mpi r = { 0 };
    mbedtls_mpi s = { 0 };

    if( out_signature == NULL )
    {
        ESP_LOGE( TAG, "ASN1 invalid argument !" );
        goto cleanup;
    }

    mbedtls_mpi_init( &r );
    mbedtls_mpi_init( &s );

    if( ( ret = mbedtls_asn1_get_tag( &signature, end, &len,
                                      MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE ) ) != 0 )
    {
        ESP_LOGE( TAG, "Bad Input Signature" );
        goto cleanup;
    }

    if( signature + len != end )
    {
        ESP_LOGE( TAG, "Incorrect ASN1 Signature Length" );
        goto cleanup;
    }

    if( ( ( ret = mbedtls_asn1_get_mpi( &signature, end, &r ) ) != 0 ) ||
        ( ( ret = mbedtls_asn1_get_mpi( &signature, end, &s ) ) != 0 ) )
    {
        ESP_LOGE( TAG, "ASN1 parsing failed" );
        goto cleanup;
    }

    ret = mbedtls_mpi_write_binary( &r, out_signature, ECDSA_INTEGER_LEN );
    ret = mbedtls_mpi_write_binary( &s, out_signature + ECDSA_INTEGER_LEN, ECDSA_INTEGER_LEN );

cleanup:
    mbedtls_mpi_free( &r );
    mbedtls_mpi_free( &s );

    if( ret == 0 )
    {
        return OtaErrNone;
    }
    else
    {
        return OtaPalBadSignerCert;
    }
}

static void _esp_ota_ctx_clear( esp_ota_context_t * ota_ctx )
{
    if( ota_ctx != NULL )
    {
        memset( ota_ctx, 0, sizeof( esp_ota_context_t ) );
    }
}

static bool _esp_ota_ctx_validate( OtaFileContext_t * C )
{
    return( C != NULL && ota_ctx.cur_ota == C && C->pFile == ( uint8_t * ) &ota_ctx );
}

static void _esp_ota_ctx_close( OtaFileContext_t * C )
{
    if( C != NULL )
    {
        C->pFile = 0;
    }

    /*memset(&ota_ctx, 0, sizeof(esp_ota_context_t)); */
    ota_ctx.cur_ota = 0;
}

/* Abort receiving the specified OTA update by closing the file. */
OtaPalStatus_t otaPal_Abort( OtaFileContext_t * const C )
{
    OtaPalStatus_t ota_ret = OTA_PAL_COMBINE_ERR( OtaPalAbortFailed, 0 );

    if( _esp_ota_ctx_validate( C ) )
    {
        _esp_ota_ctx_close( C );
        ota_ret = OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
    }
    else if( C && ( C->pFile == NULL ) )
    {
        ota_ret = OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
    }

    return ota_ret;
}

/* Attempt to create a new receive file for the file chunks as they come in. */
OtaPalStatus_t otaPal_CreateFileForRx( OtaFileContext_t * const C )
{
    if( ( NULL == C ) || ( NULL == C->pFilePath ) )
    {
        return OTA_PAL_COMBINE_ERR( OtaPalRxFileCreateFailed, 0 );
    }

    const esp_partition_t * update_partition = aws_esp_ota_get_next_update_partition( NULL );

    if( update_partition == NULL )
    {
        ESP_LOGE( TAG, "failed to find update partition" );
        return OTA_PAL_COMBINE_ERR( OtaPalRxFileCreateFailed, 0 );
    }

    ESP_LOGI( TAG, "Writing to partition subtype %d at offset 0x%x",
              update_partition->subtype, update_partition->address );

    esp_ota_handle_t update_handle;
    esp_err_t err = aws_esp_ota_begin( update_partition, OTA_SIZE_UNKNOWN, &update_handle );

    if( err != ESP_OK )
    {
        ESP_LOGE( TAG, "aws_esp_ota_begin failed (%d)", err );
        return OTA_PAL_COMBINE_ERR( OtaPalRxFileCreateFailed, 0 );
    }

    ota_ctx.cur_ota = C;
    ota_ctx.update_partition = update_partition;
    ota_ctx.update_handle = update_handle;

    C->pFile = ( uint8_t * ) &ota_ctx;
    ota_ctx.data_write_len = 0;
    ota_ctx.valid_image = false;

    ESP_LOGI( TAG, "aws_esp_ota_begin succeeded" );

    return OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
}


static CK_RV prvGetCertificateHandle( CK_FUNCTION_LIST_PTR pxFunctionList,
                                      CK_SESSION_HANDLE xSession,
                                      const char * pcLabelName,
                                      CK_OBJECT_HANDLE_PTR pxCertHandle )
{
    CK_ATTRIBUTE xTemplate;
    CK_RV xResult = CKR_OK;
    CK_ULONG ulCount = 0;
    CK_BBOOL xFindInit = CK_FALSE;

    /* Get the certificate handle. */
    if( 0 == xResult )
    {
        xTemplate.type = CKA_LABEL;
        xTemplate.ulValueLen = strlen( pcLabelName ) + 1;
        xTemplate.pValue = ( char * ) pcLabelName;
        xResult = pxFunctionList->C_FindObjectsInit( xSession, &xTemplate, 1 );
    }

    if( 0 == xResult )
    {
        xFindInit = CK_TRUE;
        xResult = pxFunctionList->C_FindObjects( xSession,
                                                 ( CK_OBJECT_HANDLE_PTR ) pxCertHandle,
                                                 1,
                                                 &ulCount );
    }

    if( CK_TRUE == xFindInit )
    {
        xResult = pxFunctionList->C_FindObjectsFinal( xSession );
    }

    return xResult;
}

/* Note that this function mallocs a buffer for the certificate to reside in,
 * and it is the responsibility of the caller to free the buffer. */
static CK_RV prvGetCertificate( const char * pcLabelName,
                                uint8_t ** ppucData,
                                uint32_t * pulDataSize )
{
    /* Find the certificate */
    CK_OBJECT_HANDLE xHandle = 0;
    CK_RV xResult;
    CK_FUNCTION_LIST_PTR xFunctionList;
    CK_SLOT_ID xSlotId;
    CK_ULONG xCount = 1;
    CK_SESSION_HANDLE xSession;
    CK_ATTRIBUTE xTemplate = { 0 };
    uint8_t * pucCert = NULL;
    CK_BBOOL xSessionOpen = CK_FALSE;

    xResult = C_GetFunctionList( &xFunctionList );

    if( CKR_OK == xResult )
    {
        xResult = xFunctionList->C_Initialize( NULL );
    }

    if( ( CKR_OK == xResult ) || ( CKR_CRYPTOKI_ALREADY_INITIALIZED == xResult ) )
    {
        xResult = xFunctionList->C_GetSlotList( CK_TRUE, &xSlotId, &xCount );
    }

    if( CKR_OK == xResult )
    {
        xResult = xFunctionList->C_OpenSession( xSlotId, CKF_SERIAL_SESSION, NULL, NULL, &xSession );
    }

    if( CKR_OK == xResult )
    {
        xSessionOpen = CK_TRUE;
        xResult = prvGetCertificateHandle( xFunctionList, xSession, pcLabelName, &xHandle );
    }

    if( ( xHandle != 0 ) && ( xResult == CKR_OK ) ) /* 0 is an invalid handle */
    {
        /* Get the length of the certificate */
        xTemplate.type = CKA_VALUE;
        xTemplate.pValue = NULL;
        xResult = xFunctionList->C_GetAttributeValue( xSession, xHandle, &xTemplate, xCount );

        if( xResult == CKR_OK )
        {
            pucCert = pvPortMalloc( xTemplate.ulValueLen );
        }

        if( ( xResult == CKR_OK ) && ( pucCert == NULL ) )
        {
            xResult = CKR_HOST_MEMORY;
        }

        if( xResult == CKR_OK )
        {
            xTemplate.pValue = pucCert;
            xResult = xFunctionList->C_GetAttributeValue( xSession, xHandle, &xTemplate, xCount );

            if( xResult == CKR_OK )
            {
                *ppucData = pucCert;
                *pulDataSize = xTemplate.ulValueLen;
            }
            else
            {
                vPortFree( pucCert );
            }
        }
    }
    else /* Certificate was not found. */
    {
        *ppucData = NULL;
        *pulDataSize = 0;
    }

    if( xSessionOpen == CK_TRUE )
    {
        ( void ) xFunctionList->C_CloseSession( xSession );
    }

    return xResult;
}

uint8_t * prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                      uint32_t * const ulSignerCertSize )
{
    uint8_t * pucCertData;
    uint32_t ulCertSize;
    uint8_t * pucSignerCert = NULL;
    CK_RV xResult;

    xResult = prvGetCertificate( ( const char * ) pucCertName, &pucSignerCert, ulSignerCertSize );

    if( ( xResult == CKR_OK ) && ( pucSignerCert != NULL ) )
    {
        ESP_LOGI( TAG, "Using cert with label: %s OK\r\n", ( const char * ) pucCertName );
    }
    else
    {
        ESP_LOGI( TAG, "No such certificate file: %s. Using aws_ota_codesigner_certificate.h.\r\n",
                  ( const char * ) pucCertName );

        /* Allocate memory for the signer certificate plus a terminating zero so we can copy it and return to the caller. */
        ulCertSize = sizeof( signingcredentialSIGNING_CERTIFICATE_PEM );
        pucSignerCert = pvPortMalloc( ulCertSize );                           /*lint !e9029 !e9079 !e838 malloc proto requires void*. */
        pucCertData = ( uint8_t * ) signingcredentialSIGNING_CERTIFICATE_PEM; /*lint !e9005 we don't modify the cert but it could be set by PKCS11 so it's not const. */

        if( pucSignerCert != NULL )
        {
            memcpy( pucSignerCert, pucCertData, ulCertSize );
            *ulSignerCertSize = ulCertSize;
        }
        else
        {
            ESP_LOGE( TAG, "Error: No memory for certificate in prvPAL_ReadAndAssumeCertificate!\r\n" );
        }
    }

    return pucSignerCert;
}

/* Verify the signature of the specified file. */
OtaPalStatus_t prvPAL_CheckFileSignature( OtaFileContext_t * const C )
{
    OtaPalStatus_t result;
    uint32_t ulSignerCertSize;
    void * pvSigVerifyContext;
    uint8_t * pucSignerCert = 0;
    static spi_flash_mmap_memory_t ota_data_map;
    const void * buf = NULL;

    /* Verify an ECDSA-SHA256 signature. */
    if( CRYPTO_SignatureVerificationStart( &pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA,
                                           cryptoHASH_ALGORITHM_SHA256 ) == pdFALSE )
    {
        ESP_LOGE( TAG, "signature verification start failed" );
        return OTA_PAL_COMBINE_ERR( OtaPalRxFileCreateFailed, 0 );
    }

    pucSignerCert = prvPAL_ReadAndAssumeCertificate( ( const uint8_t * const ) C->pCertFilepath, &ulSignerCertSize );

    if( pucSignerCert == NULL )
    {
        ESP_LOGE( TAG, "cert read failed" );
        return OTA_PAL_COMBINE_ERR( OtaPalBadSignerCert, 0 );
    }

    esp_err_t ret = esp_partition_mmap( ota_ctx.update_partition, 0, ota_ctx.data_write_len,
                                        SPI_FLASH_MMAP_DATA, &buf, &ota_data_map );

    if( ret != ESP_OK )
    {
        ESP_LOGE( TAG, "partition mmap failed %d", ret );
        result = OTA_PAL_COMBINE_ERR( OtaPalRxFileCreateFailed, 0 );
        goto end;
    }

    CRYPTO_SignatureVerificationUpdate( pvSigVerifyContext, buf, ota_ctx.data_write_len );
    spi_flash_munmap( ota_data_map );

    if( CRYPTO_SignatureVerificationFinal( pvSigVerifyContext, ( char * ) pucSignerCert, ulSignerCertSize,
                                           C->pSignature->data, C->pSignature->size ) == pdFALSE )
    {
        ESP_LOGE( TAG, "signature verification failed" );
        result = OTA_PAL_COMBINE_ERR( OtaPalRxFileCreateFailed, 0 );
    }
    else
    {
        result = OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
    }

end:

    /* Free the signer certificate that we now own after prvReadAndAssumeCertificate(). */
    if( pucSignerCert != NULL )
    {
        vPortFree( pucSignerCert );
    }

    return result;
}

/* Close the specified file. This shall authenticate the file if it is marked as secure. */
OtaPalStatus_t otaPal_CloseFile( OtaFileContext_t * const C )
{
    OtaPalStatus_t result = OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );

    if( !_esp_ota_ctx_validate( C ) )
    {
        return OTA_PAL_COMBINE_ERR( OtaPalFileClose, 0 );
    }

    if( C->pSignature == NULL )
    {
        ESP_LOGE( TAG, "Image Signature not found" );
        _esp_ota_ctx_clear( &ota_ctx );
        result = OTA_PAL_COMBINE_ERR( OtaPalRxFileCreateFailed, 0 );
    }
    else if( ota_ctx.data_write_len == 0 )
    {
        ESP_LOGE( TAG, "No data written to partition" );
        result = OTA_PAL_COMBINE_ERR( OtaPalRxFileCreateFailed, 0 );
    }
    else
    {
        /* Verify the file signature, close the file and return the signature verification result. */
        result = prvPAL_CheckFileSignature( C );

        if( result != OtaErrNone )
        {
            esp_partition_erase_range( ota_ctx.update_partition, 0, ota_ctx.update_partition->size );
        }
        else
        {
            /* Write ASN1 decoded signature at the end of firmware image for bootloader to validate during bootup */
            esp_sec_boot_sig_t * sec_boot_sig = ( esp_sec_boot_sig_t * ) malloc( sizeof( esp_sec_boot_sig_t ) );

            if( sec_boot_sig != NULL )
            {
                memset( sec_boot_sig->sec_ver, 0x00, sizeof( sec_boot_sig->sec_ver ) );
                memset( sec_boot_sig->pad, 0xFF, sizeof( sec_boot_sig->pad ) );
                result = asn1_to_raw_ecdsa( C->pSignature->data, C->pSignature->size, sec_boot_sig->raw_ecdsa_sig );

                if( result == OTA_PAL_COMBINE_ERR( OtaErrNone, 0 ) )
                {
                    esp_err_t ret = aws_esp_ota_write( ota_ctx.update_handle, sec_boot_sig, ota_ctx.data_write_len, ECDSA_SIG_SIZE );

                    if( ret != ESP_OK )
                    {
                        return OTA_PAL_COMBINE_ERR( OtaPalFileClose, 0 );
                    }

                    ota_ctx.data_write_len += ECDSA_SIG_SIZE;
                }

                free( sec_boot_sig );
                ota_ctx.valid_image = true;
            }
            else
            {
                result = OTA_PAL_COMBINE_ERR( OtaPalRxFileCreateFailed, 0 );
            }
        }
    }

    return result;
}

OtaPalStatus_t IRAM_ATTR otaPal_ResetDevice( OtaFileContext_t * const C )
{
    ( void ) C;

    /* Short delay for debug log output before reset. */
    vTaskDelay( kOTA_HalfSecondDelay );
    esp_restart();
    return OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
}

OtaPalStatus_t otaPal_ActivateNewImage( OtaFileContext_t * const C )
{
    ( void ) C;

    if( ota_ctx.cur_ota != NULL )
    {
        if( aws_esp_ota_end( ota_ctx.update_handle ) != ESP_OK )
        {
            ESP_LOGE( TAG, "aws_esp_ota_end failed!" );
            esp_partition_erase_range( ota_ctx.update_partition, 0, ota_ctx.update_partition->size );
            otaPal_ResetDevice( C );
        }

        esp_err_t err = aws_esp_ota_set_boot_partition( ota_ctx.update_partition );

        if( err != ESP_OK )
        {
            ESP_LOGE( TAG, "aws_esp_ota_set_boot_partition failed (%d)!", err );
            esp_partition_erase_range( ota_ctx.update_partition, 0, ota_ctx.update_partition->size );
            _esp_ota_ctx_clear( &ota_ctx );
        }

        otaPal_ResetDevice( C );
    }

    _esp_ota_ctx_clear( &ota_ctx );
    otaPal_ResetDevice( C );
    return OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
}

/* Write a block of data to the specified file. */
int16_t otaPal_WriteBlock( OtaFileContext_t * const C,
                           uint32_t iOffset,
                           uint8_t * const pacData,
                           uint32_t iBlockSize )
{
    if( _esp_ota_ctx_validate( C ) )
    {
        esp_err_t ret = aws_esp_ota_write( ota_ctx.update_handle, pacData, iOffset, iBlockSize );

        if( ret != ESP_OK )
        {
            ESP_LOGE( TAG, "Couldn't flash at the offset %d", iOffset );
            return -1;
        }

        ota_ctx.data_write_len += iBlockSize;
    }
    else
    {
        ESP_LOGI( TAG, "Invalid OTA Context" );
        return -1;
    }

    return iBlockSize;
}

OtaPalImageState_t otaPal_GetPlatformImageState( OtaFileContext_t * const C )
{
    OtaPalImageState_t eImageState = OtaPalImageStateUnknown;
    uint32_t ota_flags;

    ( void ) C;

    ESP_LOGI( TAG, "%s", __func__ );

    if( ( ota_ctx.cur_ota != NULL ) && ( ota_ctx.data_write_len != 0 ) )
    {
        /* Firmware update is complete or on-going, retrieve its status */
        ota_flags = ota_ctx.valid_image == true ? ESP_OTA_IMG_NEW : ESP_OTA_IMG_INVALID;
    }
    else
    {
        esp_err_t ret = aws_esp_ota_get_boot_flags( &ota_flags, true );

        if( ret != ESP_OK )
        {
            ESP_LOGE( TAG, "failed to get ota flags %d", ret );
            return eImageState;
        }
    }

    switch( ota_flags )
    {
        case ESP_OTA_IMG_PENDING_VERIFY:
            /* Pending Commit means we're in the Self Test phase. */
            eImageState = OtaPalImageStatePendingCommit;
            break;

        case ESP_OTA_IMG_VALID:
        case ESP_OTA_IMG_NEW:
            eImageState = OtaPalImageStateValid;
            break;

        default:
            eImageState = OtaPalImageStateInvalid;
            break;
    }

    return eImageState;
}

static void disable_rtc_wdt()
{
    ESP_LOGI( TAG, "Disabling RTC hardware watchdog timer" );
    rtc_wdt_disable();
}

OtaPalStatus_t otaPal_SetPlatformImageState( OtaFileContext_t * const C,
                                       OtaImageState_t eState )
{
    OtaPalMainStatus_t mainErr = OtaPalSuccess;
    int state;

    ( void ) C;

    ESP_LOGI( TAG, "%s, %d", __func__, eState );

    switch( eState )
    {
        case OtaImageStateAccepted:
            ESP_LOGI( TAG, "Set image as valid one!" );
            state = ESP_OTA_IMG_VALID;
            break;

        case OtaImageStateRejected:
            ESP_LOGW( TAG, "Set image as invalid!" );
            state = ESP_OTA_IMG_INVALID;
            break;

        case OtaImageStateAborted:
            ESP_LOGW( TAG, "Set image as aborted!" );
            state = ESP_OTA_IMG_ABORTED;
            break;

        case OtaImageStateTesting:
            ESP_LOGW( TAG, "Set image as testing!" );
            return OTA_PAL_COMBINE_ERR(OtaPalSuccess,0);

        default:
            ESP_LOGW( TAG, "Set image invalid state!" );
            return OTA_PAL_COMBINE_ERR(OtaPalBadImageState,0);
    }

    uint32_t ota_flags;
    /* Get current active (running) firmware image flags */
    esp_err_t ret = aws_esp_ota_get_boot_flags( &ota_flags, true );

    if( ret != ESP_OK )
    {
        ESP_LOGE( TAG, "failed to get ota flags %d", ret );
        return OTA_PAL_COMBINE_ERR(OtaPalCommitFailed,0);
    }

    /* If this is first request to set platform state, post bootup and there is not OTA being
     * triggered yet, then operate on active image flags, else use passive image flags */
    if( ( ota_ctx.cur_ota == NULL ) && ( ota_ctx.data_write_len == 0 ) )
    {
        if( ota_flags == ESP_OTA_IMG_PENDING_VERIFY )
        {
            ret = aws_esp_ota_set_boot_flags( state, true );

            if( ret != ESP_OK )
            {
                ESP_LOGE( TAG, "failed to set ota flags %d", ret );
                return OTA_PAL_COMBINE_ERR(OtaPalCommitFailed,0);
            }
            else
            {
                /* RTC watchdog timer can now be stopped */
                disable_rtc_wdt();
            }
        }
        else
        {
            ESP_LOGW( TAG, "Image not in self test mode %d", ota_flags );
            mainErr = ota_flags == ESP_OTA_IMG_VALID ? OtaPalSuccess : OtaPalCommitFailed;
        }

        /* For debug purpose only, get current flags */
        aws_esp_ota_get_boot_flags( &ota_flags, true );
    }
    else
    {
        if( ( eState == OtaImageStateAccepted ) && ( ota_ctx.valid_image == false ) )
        {
            /* Incorrect update image or not yet validated */
            return OTA_PAL_COMBINE_ERR( OtaPalCommitFailed, 0 );
        }

        if( ota_flags != ESP_OTA_IMG_VALID )
        {
            ESP_LOGE( TAG, "currently executing firmware not marked as valid, abort" );
            return OTA_PAL_COMBINE_ERR( OtaPalCommitFailed, 0 );
        }

        ret = aws_esp_ota_set_boot_flags( state, false );

        if( ret != ESP_OK )
        {
            ESP_LOGE( TAG, "failed to set ota flags %d", ret );
            return OTA_PAL_COMBINE_ERR( OtaPalCommitFailed, 0 );
        }
    }

    return OTA_PAL_COMBINE_ERR(mainErr,0);
}
