/*
 * Copyright 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * FreeRTOS OTA PAL for ESP32-DevKitC ESP-WROVER-KIT V2.0.0
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
#include <sys/param.h>
#include "ota.h"
#include "ota_pal.h"
#include "ota_interface_private.h"
#include "ota_config.h"

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

#define OTA_HALF_SECOND_DELAY    pdMS_TO_TICKS( 500UL )
#define ECDSA_INTEGER_LEN        32

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

static const char codeSigningCertificatePEM[] = otapalconfigCODE_SIGNING_CERTIFICATE;

/* Specify the OTA signature algorithm we support on this platform. */
const char OTA_JsonFileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";

static CK_RV prvGetCertificateHandle( CK_FUNCTION_LIST_PTR pxFunctionList,
                                      CK_SESSION_HANDLE xSession,
                                      const char * pcLabelName,
                                      CK_OBJECT_HANDLE_PTR pxCertHandle );
static CK_RV prvGetCertificate( const char * pcLabelName,
                                uint8_t ** ppucData,
                                uint32_t * pulDataSize );

static OtaPalMainStatus_t asn1_to_raw_ecdsa( uint8_t * signature,
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
        LogError( ( "ASN1 invalid argument !" ) );
        goto cleanup;
    }

    mbedtls_mpi_init( &r );
    mbedtls_mpi_init( &s );

    if( ( ret = mbedtls_asn1_get_tag( &signature, end, &len,
                                      MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE ) ) != 0 )
    {
        LogError( ( "Bad Input Signature" ) );
        goto cleanup;
    }

    if( signature + len != end )
    {
        LogError( ( "Incorrect ASN1 Signature Length" ) );
        goto cleanup;
    }

    if( ( ( ret = mbedtls_asn1_get_mpi( &signature, end, &r ) ) != 0 ) ||
        ( ( ret = mbedtls_asn1_get_mpi( &signature, end, &s ) ) != 0 ) )
    {
        LogError( ( "ASN1 parsing failed" ) );
        goto cleanup;
    }

    ret = mbedtls_mpi_write_binary( &r, out_signature, ECDSA_INTEGER_LEN );
    ret = mbedtls_mpi_write_binary( &s, out_signature + ECDSA_INTEGER_LEN, ECDSA_INTEGER_LEN );

cleanup:
    mbedtls_mpi_free( &r );
    mbedtls_mpi_free( &s );

    if( ret == 0 )
    {
        return OtaPalSuccess;
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

static bool _esp_ota_ctx_validate( OtaFileContext_t * pFileContext )
{
    return( pFileContext != NULL && ota_ctx.cur_ota == pFileContext && pFileContext->pFile == ( uint8_t * ) &ota_ctx );
}

static void _esp_ota_ctx_close( OtaFileContext_t * pFileContext )
{
    if( pFileContext != NULL )
    {
        pFileContext->pFile = 0;
    }

    /*memset(&ota_ctx, 0, sizeof(esp_ota_context_t)); */
    ota_ctx.cur_ota = 0;
}

/* Abort receiving the specified OTA update by closing the file. */
OtaPalStatus_t otaPal_Abort( OtaFileContext_t * const pFileContext )
{
    OtaPalStatus_t ota_ret = OTA_PAL_COMBINE_ERR( OtaPalAbortFailed, 0 );

    if( _esp_ota_ctx_validate( pFileContext ) )
    {
        _esp_ota_ctx_close( pFileContext );
        ota_ret = OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
    }
    else if( pFileContext && ( pFileContext->pFile == NULL ) )
    {
        ota_ret = OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
    }

    return ota_ret;
}

/* Attempt to create a new receive file for the file chunks as they come in. */
OtaPalStatus_t otaPal_CreateFileForRx( OtaFileContext_t * const pFileContext )
{
    if( ( NULL == pFileContext ) || ( NULL == pFileContext->pFilePath ) )
    {
        return OTA_PAL_COMBINE_ERR( OtaPalRxFileCreateFailed, 0 );
    }

    const esp_partition_t * update_partition = esp_ota_get_next_update_partition( NULL );

    if( update_partition == NULL )
    {
        LogError( ( "Failed to find update partition" ) );
        return OTA_PAL_COMBINE_ERR( OtaPalRxFileCreateFailed, 0 );
    }

    LogInfo( ( "Writing to partition subtype %d at offset 0x%x",
               update_partition->subtype, update_partition->address ) );

    esp_ota_handle_t update_handle;
    esp_err_t err = esp_ota_begin( update_partition, OTA_SIZE_UNKNOWN, &update_handle );

    if( err != ESP_OK )
    {
        LogError( ( "esp_ota_begin failed (%d)", err ) );
        return OTA_PAL_COMBINE_ERR( OtaPalRxFileCreateFailed, 0 );
    }

    ota_ctx.cur_ota = pFileContext;
    ota_ctx.update_partition = update_partition;
    ota_ctx.update_handle = update_handle;

    pFileContext->pFile = ( uint8_t * ) &ota_ctx;
    ota_ctx.data_write_len = 0;
    ota_ctx.valid_image = false;

    LogInfo( ( "esp_ota_begin succeeded" ) );

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

uint8_t * otaPal_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                           uint32_t * const ulSignerCertSize )
{
    uint8_t * pucCertData;
    uint32_t ulCertSize;
    uint8_t * pucSignerCert = NULL;
    CK_RV xResult;

    xResult = prvGetCertificate( ( const char * ) pucCertName, &pucSignerCert, ulSignerCertSize );

    if( ( xResult == CKR_OK ) && ( pucSignerCert != NULL ) )
    {
        LogInfo( ( "Using cert with label: %s OK", ( const char * ) pucCertName ) );
    }
    else
    {
        LogInfo( ( "No such certificate file: %s. Using certificate in ota_demo_config.h.",
                   ( const char * ) pucCertName ) );

        /* Allocate memory for the signer certificate plus a terminating zero so we can copy it and return to the caller. */
        ulCertSize = sizeof( codeSigningCertificatePEM );
        pucSignerCert = pvPortMalloc( ulCertSize );            /*lint !e9029 !e9079 !e838 malloc proto requires void*. */
        pucCertData = ( uint8_t * ) codeSigningCertificatePEM; /*lint !e9005 we don't modify the cert but it could be set by PKCS11 so it's not const. */

        if( pucSignerCert != NULL )
        {
            memcpy( pucSignerCert, pucCertData, ulCertSize );
            *ulSignerCertSize = ulCertSize;
        }
        else
        {
            LogError( ( "No memory for certificate in otaPal_ReadAndAssumeCertificate!" ) );
        }
    }

    return pucSignerCert;
}

/* Verify the signature of the specified file. */
OtaPalStatus_t otaPal_CheckFileSignature( OtaFileContext_t * const pFileContext )
{
    OtaPalStatus_t result;
    uint32_t ulSignerCertSize;
    void * pvSigVerifyContext;
    uint8_t * pucSignerCert = 0;
    static spi_flash_mmap_memory_t ota_data_map;
    uint32_t mmu_free_pages_count, len, flash_offset = 0;

    /* Verify an ECDSA-SHA256 signature. */
    if( CRYPTO_SignatureVerificationStart( &pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA,
                                           cryptoHASH_ALGORITHM_SHA256 ) == pdFALSE )
    {
        LogError( ( "Signature verification start failed" ) );
        return OTA_PAL_COMBINE_ERR( OtaPalSignatureCheckFailed, 0 );
    }

    pucSignerCert = otaPal_ReadAndAssumeCertificate( ( const uint8_t * const ) pFileContext->pCertFilepath, &ulSignerCertSize );

    if( pucSignerCert == NULL )
    {
        LogError( ( "Cert read failed" ) );
        return OTA_PAL_COMBINE_ERR( OtaPalBadSignerCert, 0 );
    }

    mmu_free_pages_count = spi_flash_mmap_get_free_pages( SPI_FLASH_MMAP_DATA );
    len = ota_ctx.data_write_len;

    while( len > 0 )
    {
        /* Data we could map in case we are not aligned to PAGE boundary is one page size lesser.
         * 0x0000FFFF is mmap aligned mask for 64K boundary */
        uint32_t mmu_page_offset = ( ( flash_offset & 0x0000FFFF ) != 0 ) ? 1 : 0;
        /* Read the image that fits in the free MMU pages */
        uint32_t partial_image_len = MIN( len, ( ( mmu_free_pages_count - mmu_page_offset ) * SPI_FLASH_MMU_PAGE_SIZE ) );
        const void * buf = NULL;

        esp_err_t ret = esp_partition_mmap( ota_ctx.update_partition, flash_offset, partial_image_len,
                                            SPI_FLASH_MMAP_DATA, &buf, &ota_data_map );

        if( ret != ESP_OK )
        {
            LogError( ( "Partition mmap failed %d", ret ) );
            result = OTA_PAL_COMBINE_ERR( OtaPalSignatureCheckFailed, 0 );
            goto end;
        }

        CRYPTO_SignatureVerificationUpdate( pvSigVerifyContext, buf, partial_image_len );
        spi_flash_munmap( ota_data_map );
        flash_offset += partial_image_len;
        len -= partial_image_len;
    }

    if( CRYPTO_SignatureVerificationFinal( pvSigVerifyContext, ( char * ) pucSignerCert, ulSignerCertSize,
                                           pFileContext->pSignature->data, pFileContext->pSignature->size ) == pdFALSE )
    {
        LogError( ( "Signature verification failed" ) );
        result = OTA_PAL_COMBINE_ERR( OtaPalSignatureCheckFailed, 0 );
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
OtaPalStatus_t otaPal_CloseFile( OtaFileContext_t * const pFileContext )
{
    OtaPalMainStatus_t mainErr = OtaPalSuccess;

    if( !_esp_ota_ctx_validate( pFileContext ) )
    {
        return OTA_PAL_COMBINE_ERR( OtaPalFileClose, 0 );
    }

    if( pFileContext->pSignature == NULL )
    {
        LogError( ( "Image Signature not found" ) );
        _esp_ota_ctx_clear( &ota_ctx );
        mainErr = OtaPalSignatureCheckFailed;
    }
    else if( ota_ctx.data_write_len == 0 )
    {
        LogError( ( "No data written to partition" ) );
        mainErr = OtaPalSignatureCheckFailed;
    }
    else
    {
        /* Verify the file signature, close the file and return the signature verification result. */
        mainErr = OTA_PAL_MAIN_ERR( otaPal_CheckFileSignature( pFileContext ) );

        if( mainErr != OtaPalSuccess )
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
                mainErr = asn1_to_raw_ecdsa( pFileContext->pSignature->data, pFileContext->pSignature->size, sec_boot_sig->raw_ecdsa_sig );

                if( mainErr == OtaPalSuccess )
                {
                    esp_err_t ret = esp_ota_write_with_offset( ota_ctx.update_handle, sec_boot_sig, ECDSA_SIG_SIZE, ota_ctx.data_write_len );

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
                mainErr = OtaPalFileClose;
            }
        }
    }

    return OTA_PAL_COMBINE_ERR( mainErr, 0 );
}

OtaPalStatus_t IRAM_ATTR otaPal_ResetDevice( OtaFileContext_t * const pFileContext )
{
    ( void ) pFileContext;

    /* Short delay for debug log output before reset. */
    vTaskDelay( OTA_HALF_SECOND_DELAY );
    esp_restart();
    return OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
}

OtaPalStatus_t otaPal_ActivateNewImage( OtaFileContext_t * const pFileContext )
{
    ( void ) pFileContext;

    if( ota_ctx.cur_ota != NULL )
    {
        if( esp_ota_end( ota_ctx.update_handle ) != ESP_OK )
        {
            LogError( ( "esp_ota_end failed!" ) );
            esp_partition_erase_range( ota_ctx.update_partition, 0, ota_ctx.update_partition->size );
            otaPal_ResetDevice( pFileContext );
        }

        esp_err_t err = esp_ota_set_boot_partition( ota_ctx.update_partition );

        if( err != ESP_OK )
        {
            LogError( ( "esp_ota_set_boot_partition failed (%d)!", err ) );
            esp_partition_erase_range( ota_ctx.update_partition, 0, ota_ctx.update_partition->size );
            _esp_ota_ctx_clear( &ota_ctx );
        }

        otaPal_ResetDevice( pFileContext );
    }

    _esp_ota_ctx_clear( &ota_ctx );
    otaPal_ResetDevice( pFileContext );
    return OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
}

/* Write a block of data to the specified file. */
int16_t otaPal_WriteBlock( OtaFileContext_t * const pFileContext,
                           uint32_t iOffset,
                           uint8_t * const pacData,
                           uint32_t iBlockSize )
{
    if( _esp_ota_ctx_validate( pFileContext ) )
    {
        esp_err_t ret = esp_ota_write_with_offset( ota_ctx.update_handle, pacData, iBlockSize, iOffset );

        if( ret != ESP_OK )
        {
            LogError( ( "Couldn't flash at the offset %d", iOffset ) );
            return -1;
        }

        ota_ctx.data_write_len += iBlockSize;
    }
    else
    {
        LogInfo( ( "Invalid OTA Context" ) );
        return -1;
    }

    return iBlockSize;
}

OtaPalImageState_t otaPal_GetPlatformImageState( OtaFileContext_t * const pFileContext )
{
    OtaPalImageState_t eImageState = OtaPalImageStateUnknown;
    uint32_t ota_flags;

    ( void ) pFileContext;

    LogInfo( ( "%s", __func__ ) );

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
            LogError( ( "Failed to get ota flags %d", ret ) );
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
    LogInfo( ( "Disabling RTC hardware watchdog timer" ) );
    rtc_wdt_disable();
}

OtaPalStatus_t otaPal_SetPlatformImageState( OtaFileContext_t * const pFileContext,
                                             OtaImageState_t eState )
{
    OtaPalMainStatus_t mainErr = OtaPalSuccess;
    int state;

    ( void ) pFileContext;

    LogInfo( ( "%s, %d", __func__, eState ) );

    switch( eState )
    {
        case OtaImageStateAccepted:
            LogInfo( ( "Set image as valid one!" ) );
            state = ESP_OTA_IMG_VALID;
            break;

        case OtaImageStateRejected:
            LogWarn( ( "Set image as invalid!" ) );
            state = ESP_OTA_IMG_INVALID;
            break;

        case OtaImageStateAborted:
            LogWarn( ( "Set image as aborted!" ) );
            state = ESP_OTA_IMG_ABORTED;
            break;

        case OtaImageStateTesting:
            LogWarn( ( "Set image as testing!" ) );
            return OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );

        default:
            LogWarn( ( "Set image invalid state!" ) );
            return OTA_PAL_COMBINE_ERR( OtaPalBadImageState, 0 );
    }

    uint32_t ota_flags;
    /* Get current active (running) firmware image flags */
    esp_err_t ret = aws_esp_ota_get_boot_flags( &ota_flags, true );

    if( ret != ESP_OK )
    {
        LogError( ( "Failed to get ota flags %d", ret ) );
        return OTA_PAL_COMBINE_ERR( OtaPalCommitFailed, 0 );
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
                LogError( ( "Failed to set ota flags %d", ret ) );
                return OTA_PAL_COMBINE_ERR( OtaPalCommitFailed, 0 );
            }
            else
            {
                /* RTC watchdog timer can now be stopped */
                disable_rtc_wdt();
            }
        }
        else
        {
            LogWarn( ( "Image not in self test mode %d", ota_flags ) );
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
            LogError( ( "Currently executing firmware not marked as valid, abort" ) );
            return OTA_PAL_COMBINE_ERR( OtaPalCommitFailed, 0 );
        }

        ret = aws_esp_ota_set_boot_flags( state, false );

        if( ret != ESP_OK )
        {
            LogError( ( "Failed to set ota flags %d", ret ) );
            return OTA_PAL_COMBINE_ERR( OtaPalCommitFailed, 0 );
        }
    }

    return OTA_PAL_COMBINE_ERR( mainErr, 0 );
}

/* Provide access to private members for testing. */
#ifdef FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_ota_pal_test_access_define.h"
#endif
