/*
 * Copyright 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * Amazon FreeRTOS OTA PAL for ESP32-DevKitC ESP-WROVER-KIT V1.0.2
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
#include "aws_ota_agent.h"
#include "aws_ota_pal.h"
#include "aws_crypto.h"
#include "aws_pkcs11.h"
#include "esp_system.h"
#include "esp_log.h"
#include "soc/rtc_cntl_reg.h"
#include "aws_ota_codesigner_certificate.h"

#include "esp_partition.h"
#include "esp_spi_flash.h"
#include "esp_image_format.h"
#include "esp_ota_ops.h"
#include "aws_esp_ota_ops.h"
#include "mbedtls/asn1.h"
#include "mbedtls/bignum.h"
#include "mbedtls/base64.h"

#define kOTA_HalfSecondDelay    pdMS_TO_TICKS( 500UL )
#define ECDSA_INTEGER_LEN       32

/*
 * Includes 4 bytes of version field, followed by 64 bytes of signature
 * (Rest 12 bytes for padding to make it 16 byte aligned for flash encryption)
 */
#define ECDSA_SIG_SIZE          80

typedef struct
{
    const esp_partition_t * update_partition;
    const OTA_FileContext_t * cur_ota;
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
const char pcOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";

static CK_RV prvGetCertificateHandle( CK_FUNCTION_LIST_PTR pxFunctionList,
                                      CK_SESSION_HANDLE xSession,
                                      const char * pcLabelName,
                                      CK_OBJECT_HANDLE_PTR pxCertHandle );
static CK_RV prvGetCertificate( const char * pcLabelName,
                                uint8_t ** ppucData,
                                uint32_t * pulDataSize );

static OTA_Err_t asn1_to_raw_ecdsa( uint8_t * signature,
                                    uint16_t sig_len,
                                    uint8_t * out_signature )
{
    int ret = 0;
    const unsigned char * end = signature + sig_len;
    size_t len;
    mbedtls_mpi r, s;

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
        return kOTA_Err_None;
    }
    else
    {
        return kOTA_Err_BadSignerCert;
    }
}

static void _esp_ota_ctx_clear( esp_ota_context_t * ota_ctx )
{
    if( ota_ctx != NULL )
    {
        memset( ota_ctx, 0, sizeof( esp_ota_context_t ) );
    }
}

static bool _esp_ota_ctx_validate( OTA_FileContext_t * C )
{
    return( C != NULL && ota_ctx.cur_ota == C && C->pucFile == ( uint8_t * ) &ota_ctx );
}

static void _esp_ota_ctx_close( OTA_FileContext_t * C )
{
    if( C != NULL )
    {
        C->pucFile = 0;
    }

    /*memset(&ota_ctx, 0, sizeof(esp_ota_context_t)); */
    ota_ctx.cur_ota = 0;
}

/* Abort receiving the specified OTA update by closing the file. */
OTA_Err_t prvPAL_Abort( OTA_FileContext_t * const C )
{
    OTA_Err_t ota_ret = kOTA_Err_FileAbort;

    if( _esp_ota_ctx_validate( C ) )
    {
        _esp_ota_ctx_close( C );
        ota_ret = kOTA_Err_None;
    }
    else if( C && ( C->pucFile == NULL ) )
    {
        ota_ret = kOTA_Err_None;
    }

    return ota_ret;
}

/* Attempt to create a new receive file for the file chunks as they come in. */
OTA_Err_t prvPAL_CreateFileForRx( OTA_FileContext_t * const C )
{
    if( ( NULL == C ) || ( NULL == C->pacFilepath ) )
    {
        return kOTA_Err_RxFileCreateFailed;
    }

    const esp_partition_t * update_partition = aws_esp_ota_get_next_update_partition( NULL );

    if( update_partition == NULL )
    {
        ESP_LOGE( TAG, "failed to find update partition" );
        return kOTA_Err_RxFileCreateFailed;
    }

    ESP_LOGI( TAG, "Writing to partition subtype %d at offset 0x%x",
              update_partition->subtype, update_partition->address );

    esp_ota_handle_t update_handle;
    esp_err_t err = aws_esp_ota_begin( update_partition, OTA_SIZE_UNKNOWN, &update_handle );

    if( err != ESP_OK )
    {
        ESP_LOGE( TAG, "aws_esp_ota_begin failed (%d)", err );
        return kOTA_Err_RxFileCreateFailed;
    }

    ota_ctx.cur_ota = C;
    ota_ctx.update_partition = update_partition;
    ota_ctx.update_handle = update_handle;

    C->pucFile = ( uint8_t * ) &ota_ctx;
    ota_ctx.data_write_len = 0;
    ota_ctx.valid_image = false;

    ESP_LOGI( TAG, "aws_esp_ota_begin succeeded" );

    return kOTA_Err_None;
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
    CK_OBJECT_HANDLE xHandle;
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

u8 * prvPAL_ReadAndAssumeCertificate( const u8 * const pucCertName,
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
OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C )
{
    OTA_Err_t result;
    uint32_t ulSignerCertSize;
    void * pvSigVerifyContext;
    u8 * pucSignerCert = 0;
    static spi_flash_mmap_memory_t ota_data_map;
    const void * buf = NULL;

    /* Verify an ECDSA-SHA256 signature. */
    if( CRYPTO_SignatureVerificationStart( &pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA,
                                           cryptoHASH_ALGORITHM_SHA256 ) == pdFALSE )
    {
        ESP_LOGE( TAG, "signature verification start failed" );
        return kOTA_Err_SignatureCheckFailed;
    }

    pucSignerCert = prvPAL_ReadAndAssumeCertificate( ( const u8 * const ) C->pacCertFilepath, &ulSignerCertSize );

    if( pucSignerCert == NULL )
    {
        ESP_LOGE( TAG, "cert read failed" );
        return kOTA_Err_BadSignerCert;
    }

    esp_err_t ret = esp_partition_mmap( ota_ctx.update_partition, 0, ota_ctx.data_write_len,
                                        SPI_FLASH_MMAP_DATA, &buf, &ota_data_map );

    if( ret != ESP_OK )
    {
        ESP_LOGE( TAG, "partition mmap failed %d", ret );
        result = kOTA_Err_SignatureCheckFailed;
        goto end;
    }

    CRYPTO_SignatureVerificationUpdate( pvSigVerifyContext, buf, ota_ctx.data_write_len );
    spi_flash_munmap( ota_data_map );

    if( CRYPTO_SignatureVerificationFinal( pvSigVerifyContext, ( char * ) pucSignerCert, ulSignerCertSize,
                                           C->pxSignature->ucData, C->pxSignature->usSize ) == pdFALSE )
    {
        ESP_LOGE( TAG, "signature verification failed" );
        result = kOTA_Err_SignatureCheckFailed;
    }
    else
    {
        result = kOTA_Err_None;
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
OTA_Err_t prvPAL_CloseFile( OTA_FileContext_t * const C )
{
    OTA_Err_t result = kOTA_Err_None;

    if( !_esp_ota_ctx_validate( C ) )
    {
        return kOTA_Err_FileClose;
    }

    if( C->pxSignature == NULL )
    {
        ESP_LOGE( TAG, "Image Signature not found" );
        _esp_ota_ctx_clear( &ota_ctx );
        result = kOTA_Err_SignatureCheckFailed;
    }
    else if( ota_ctx.data_write_len == 0 )
    {
        ESP_LOGE( TAG, "No data written to partition" );
        result = kOTA_Err_SignatureCheckFailed;
    }
    else
    {
        /* Verify the file signature, close the file and return the signature verification result. */
        result = prvPAL_CheckFileSignature( C );

        if( result != kOTA_Err_None )
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
                result = asn1_to_raw_ecdsa( C->pxSignature->ucData, C->pxSignature->usSize, sec_boot_sig->raw_ecdsa_sig );

                if( result == kOTA_Err_None )
                {
                    esp_err_t ret = aws_esp_ota_write( ota_ctx.update_handle, sec_boot_sig, ota_ctx.data_write_len, ECDSA_SIG_SIZE );

                    if( ret != ESP_OK )
                    {
                        return kOTA_Err_FileClose;
                    }

                    ota_ctx.data_write_len += ECDSA_SIG_SIZE;
                }

                free( sec_boot_sig );
                ota_ctx.valid_image = true;
            }
            else
            {
                result = kOTA_Err_SignatureCheckFailed;
            }
        }
    }

    return result;
}

OTA_Err_t IRAM_ATTR prvPAL_ResetDevice( void )
{
    /* Short delay for debug log output before reset. */
    vTaskDelay( kOTA_HalfSecondDelay );
    esp_restart();
    return kOTA_Err_None;
}

OTA_Err_t prvPAL_ActivateNewImage( void )
{
    if( ota_ctx.cur_ota != NULL )
    {
        if( aws_esp_ota_end( ota_ctx.update_handle ) != ESP_OK )
        {
            ESP_LOGE( TAG, "aws_esp_ota_end failed!" );
            esp_partition_erase_range( ota_ctx.update_partition, 0, ota_ctx.update_partition->size );
            prvPAL_ResetDevice();
        }

        esp_err_t err = aws_esp_ota_set_boot_partition( ota_ctx.update_partition );

        if( err != ESP_OK )
        {
            ESP_LOGE( TAG, "aws_esp_ota_set_boot_partition failed (%d)!", err );
            esp_partition_erase_range( ota_ctx.update_partition, 0, ota_ctx.update_partition->size );
            _esp_ota_ctx_clear( &ota_ctx );
        }

        prvPAL_ResetDevice();
    }

    _esp_ota_ctx_clear( &ota_ctx );
    prvPAL_ResetDevice();
    return kOTA_Err_None;
}

/* Write a block of data to the specified file. */
int16_t prvPAL_WriteBlock( OTA_FileContext_t * const C,
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

OTA_PAL_ImageState_t prvPAL_GetPlatformImageState()
{
    OTA_PAL_ImageState_t eImageState = eOTA_PAL_ImageState_Unknown;
    uint32_t ota_flags;

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
            eImageState = eOTA_PAL_ImageState_PendingCommit;
            break;

        case ESP_OTA_IMG_VALID:
        case ESP_OTA_IMG_NEW:
            eImageState = eOTA_PAL_ImageState_Valid;
            break;

        default:
            eImageState = eOTA_PAL_ImageState_Invalid;
            break;
    }

    return eImageState;
}

static void disable_rtc_wdt()
{
    ESP_LOGI( TAG, "Disabling RTC hardware watchdog timer" );
    WRITE_PERI_REG( RTC_CNTL_WDTWPROTECT_REG, RTC_CNTL_WDT_WKEY_VALUE );
    WRITE_PERI_REG( RTC_CNTL_WDTFEED_REG, 1 );
    REG_SET_FIELD( RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_STG0, RTC_WDT_STG_SEL_OFF );
    REG_CLR_BIT( RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_EN );
    WRITE_PERI_REG( RTC_CNTL_WDTWPROTECT_REG, 0 );
}

OTA_Err_t prvPAL_SetPlatformImageState( OTA_ImageState_t eState )
{
    OTA_Err_t eResult = kOTA_Err_None;
    int state;

    ESP_LOGI( TAG, "%s, %d", __func__, eState );

    switch( eState )
    {
        case eOTA_ImageState_Accepted:
            ESP_LOGI( TAG, "Set image as valid one!" );
            state = ESP_OTA_IMG_VALID;
            break;

        case eOTA_ImageState_Rejected:
            ESP_LOGW( TAG, "Set image as invalid!" );
            state = ESP_OTA_IMG_INVALID;
            break;

        case eOTA_ImageState_Aborted:
            ESP_LOGW( TAG, "Set image as aborted!" );
            state = ESP_OTA_IMG_ABORTED;
            break;

        case eOTA_ImageState_Testing:
            ESP_LOGW( TAG, "Set image as testing!" );
            return kOTA_Err_None;

        default:
            ESP_LOGW( TAG, "Set image invalid state!" );
            return kOTA_Err_BadImageState;
    }

    uint32_t ota_flags;
    /* Get current active (running) firmware image flags */
    esp_err_t ret = aws_esp_ota_get_boot_flags( &ota_flags, true );

    if( ret != ESP_OK )
    {
        ESP_LOGE( TAG, "failed to get ota flags %d", ret );
        return kOTA_Err_CommitFailed;
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
                return kOTA_Err_CommitFailed;
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
            eResult = ota_flags == ESP_OTA_IMG_VALID ? kOTA_Err_None : kOTA_Err_CommitFailed;
        }

        /* For debug purpose only, get current flags */
        aws_esp_ota_get_boot_flags( &ota_flags, true );
    }
    else
    {
        if( ( eState == eOTA_ImageState_Accepted ) && ( ota_ctx.valid_image == false ) )
        {
            /* Incorrect update image or not yet validated */
            return kOTA_Err_CommitFailed;
        }

        if( ota_flags != ESP_OTA_IMG_VALID )
        {
            ESP_LOGE( TAG, "currently executing firmware not marked as valid, abort" );
            return kOTA_Err_CommitFailed;
        }

        ret = aws_esp_ota_set_boot_flags( state, false );

        if( ret != ESP_OK )
        {
            ESP_LOGE( TAG, "failed to set ota flags %d", ret );
            return kOTA_Err_CommitFailed;
        }
    }

    return eResult;
}

#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_ota_pal_test_access_define.h"
#endif
