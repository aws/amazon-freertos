// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Amazon FreeRTOS PKCS #11 PAL for ESP32-DevKitC ESP-WROVER-KIT V1.0.1
// Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file aws_pkcs11_pal.c
 * @brief PKCS11 Interface.
 */

/* PKCS#11 Interface Include. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "aws_crypto.h"
#include "aws_pkcs11.h"
#include "aws_pkcs11_config.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_spiffs.h"
#include "sys/stat.h"

static const char * TAG = "PKCS11";

#define pkcsFLASH_PARTITION                      "storage"
#define pkcs11palFILE_NAME_CLIENT_CERTIFICATE    "FreeRTOS_P11_Certificate.dat"
#define pkcs11palFILE_NAME_KEY                   "FreeRTOS_P11_Key.dat"
#define pkcs11palFILE_CODE_SIGN_PUBLIC_KEY       "FreeRTOS_P11_CodeSignKey.dat"

enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey
};

/*-----------------------------------------------------------*/

static void initialize_spiffs()
{
    static bool spiffs_inited = false;

    ESP_LOGI( TAG, "Initializing SPIFFS" );

    portENTER_CRITICAL();
    if ( spiffs_inited == true )
    {
        portEXIT_CRITICAL();
        return;
    }

    esp_vfs_spiffs_conf_t conf =
    {
        .base_path = "/spiffs",
        .partition_label = pkcsFLASH_PARTITION,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    /* Use settings defined above to initialize and mount SPIFFS filesystem. */
    /* Note: esp_vfs_spiffs_register is an all-in-one convenience function. */
    esp_err_t ret = esp_vfs_spiffs_register( &conf );

    if ( ret == ESP_OK )
    {
        spiffs_inited = true;
    }
    portEXIT_CRITICAL();

    if( ret != ESP_OK )
    {
        if( ret == ESP_FAIL )
        {
            ESP_LOGE( TAG, "Failed to mount or format filesystem" );
        }
        else if( ret == ESP_ERR_NOT_FOUND )
        {
            ESP_LOGE( TAG, "Failed to find SPIFFS partition" );
        }
        else
        {
            ESP_LOGE( TAG, "Failed to initialize SPIFFS (%d)", ret );
        }

        return;
    }


    size_t total = 0, used = 0;
    ret = esp_spiffs_info( pkcsFLASH_PARTITION, &total, &used );

    if( ret != ESP_OK )
    {
        ESP_LOGE( TAG, "Failed to get SPIFFS partition information" );
    }
    else
    {
        ESP_LOGI( TAG, "Partition size: total: %d, used: %d", total, used );
    }
}


/* Converts a label to its respective filename and handle. */
void prvLabelToFilenameHandle( uint8_t * pcLabel,
                               char ** pcFileName,
                               CK_OBJECT_HANDLE_PTR pHandle )
{
    if( pcLabel != NULL )
    {
        /* Translate from the PKCS#11 label to local storage file name. */
        if( 0 == memcmp( pcLabel,
                         &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                         sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
        {
            *pcFileName = pkcs11palFILE_NAME_CLIENT_CERTIFICATE;
            *pHandle = eAwsDeviceCertificate;
        }
        else if( 0 == memcmp( pcLabel,
                              &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
        {
            *pcFileName = pkcs11palFILE_NAME_KEY;
            *pHandle = eAwsDevicePrivateKey;
        }
        else if( 0 == memcmp( pcLabel,
                              &pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
        {
            *pcFileName = pkcs11palFILE_NAME_KEY;
            *pHandle = eAwsDevicePublicKey;
        }
        else if( 0 == memcmp( pcLabel,
                              &pkcs11configLABEL_CODE_VERIFICATION_KEY,
                              sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
        {
            *pcFileName = pkcs11palFILE_CODE_SIGN_PUBLIC_KEY;
            *pHandle = eAwsCodeSigningKey;
        }
        else
        {
            *pcFileName = NULL;
            *pHandle = eInvalidHandle;
        }
    }
}

/**
 * @brief Writes a file to local storage.
 *
 * Port-specific file write for crytographic information.
 *
 * @param[in] pxLabel       Label of the object to be saved.
 * @param[in] pucData       Data buffer to be written to file
 * @param[in] ulDataSize    Size (in bytes) of data to be saved.
 *
 * @return The file handle of the object that was stored.
 */
CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
                                        uint8_t * pucData,
                                        uint32_t ulDataSize )
{
    initialize_spiffs();

    FILE * fp;
    char fname[ 32 ];


    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    char * pcFileName = NULL;

    /* Translate from the PKCS#11 label to local storage file name. */
    prvLabelToFilenameHandle( pxLabel->pValue,
                              &pcFileName,
                              &xHandle );

    ESP_LOGD( TAG, "Writing file %s, %d bytes", pcFileName, ulDataSize );
    snprintf( fname, sizeof( fname ), "/spiffs/%s", pcFileName );
    fp = fopen( fname, "w+" );

    if( fp == NULL )
    {
        return eInvalidHandle;
    }

    int written_size = 0;
    int bytes = 256;

    while( written_size < ulDataSize )
    {
        if( bytes > ( ulDataSize - written_size ) )
        {
            bytes = ulDataSize - written_size;
        }

        int len = fwrite( pucData, 1, bytes, fp );
        pucData += len;
        written_size += len;
    }

    fclose( fp );

    return xHandle;
}

/**
 * @brief Translates a PKCS #11 label into an object handle.
 *
 * Port-specific object handle retrieval.
 *
 *
 * @param[in] pxLabel         Pointer to the label of the object
 *                           who's handle should be found.
 * @param[in] usLength       The length of the label, in bytes.
 *
 * @return The object handle if operation was successful.
 * Returns eInvalidHandle if unsuccessful.
 */
CK_OBJECT_HANDLE PKCS11_PAL_FindObject( uint8_t * pcLabel,
                                        uint8_t usLength )
{
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    char fname[ 32 ];
    char * pcFileName = NULL;

    initialize_spiffs();

    /* Translate from the PKCS#11 label to local storage file name. */
    prvLabelToFilenameHandle( pcLabel,
                              &pcFileName,
                              &xHandle );

    if( pcFileName != NULL )
    {
        ESP_LOGD( TAG, "Reading file %s", pcFileName );
        snprintf( fname, sizeof( fname ), "/spiffs/%s", pcFileName );

        /* Check if destination file exists. */
        struct stat st;

        if( stat( fname, &st ) != 0 )
        {
            xHandle = eInvalidHandle;
        }
    }

    return xHandle;
}

/*-----------------------------------------------------------*/

/**
 * @brief Gets the value of an object in storage, by handle.
 *
 * Port-specific file access for cryptographic information.
 *
 * This call dynamically allocates the buffer which object value
 * data is copied into.  PKCS11_PAL_GetObjectValueCleanup()
 * should be called after each use to free the dynamically allocated
 * buffer.
 *
 * @sa PKCS11_PAL_GetObjectValueCleanup
 *
 * @param[in] pcFileName    The name of the file to be read.
 * @param[out] ppucData     Pointer to buffer for file data.
 * @param[out] pulDataSize  Size (in bytes) of data located in file.
 * @param[out] pIsPrivate   Boolean indicating if value is private (CK_TRUE)
 *                          or exportable (CK_FALSE)
 *
 * @return CKR_OK if operation was successful.  CKR_KEY_HANDLE_INVALID if
 * no such object handle was found, CKR_DEVICE_MEMORY if memory for
 * buffer could not be allocated, CKR_FUNCTION_FAILED for device driver
 * error.
 */
CK_RV PKCS11_PAL_GetObjectValue( CK_OBJECT_HANDLE xHandle,
                                 uint8_t ** ppucData,
                                 uint32_t * pulDataSize,
                                 CK_BBOOL * pIsPrivate )
{
    initialize_spiffs();

    FILE * fp;
    char fname[ 32 ];
    char * pcFileName = NULL;
    CK_RV ulReturn = CKR_OK;

    if( xHandle == eAwsDeviceCertificate )
    {
        pcFileName = pkcs11palFILE_NAME_CLIENT_CERTIFICATE;
        *pIsPrivate = CK_FALSE;
    }
    else if( xHandle == eAwsDevicePrivateKey )
    {
        pcFileName = pkcs11palFILE_NAME_KEY;
        *pIsPrivate = CK_TRUE;
    }
    else if( xHandle == eAwsDevicePublicKey )
    {
        /* Public and private key are stored together in same file. */
        pcFileName = pkcs11palFILE_NAME_KEY;
        *pIsPrivate = CK_FALSE;
    }
    else if( xHandle == eAwsCodeSigningKey )
    {
        pcFileName = pkcs11palFILE_CODE_SIGN_PUBLIC_KEY;
        *pIsPrivate = CK_FALSE;
    }
    else
    {
        ulReturn = CKR_OBJECT_HANDLE_INVALID;
    }

    if( ulReturn == CKR_OK )
    {
        ESP_LOGD( TAG, "Reading file %s", pcFileName );
        snprintf( fname, sizeof( fname ), "/spiffs/%s", pcFileName );

        /* Check if destination file exists before renaming */
        struct stat st;

        if( stat( fname, &st ) != 0 )
        {
            return CKR_OBJECT_HANDLE_INVALID;
        }

        uint8_t * data = pvPortMalloc( st.st_size );

        if( data == NULL )
        {
            ESP_LOGE( TAG, "malloc failed" );

            return CKR_HOST_MEMORY;
        }

        *ppucData = data;

        fp = fopen( fname, "r" );

        if( fp == NULL )
        {
            vPortFree( data );
            return CKR_FUNCTION_FAILED;
        }

        int read_size = 0;
        int bytes = 256;

        while( read_size < st.st_size )
        {
            if( bytes > ( st.st_size - read_size ) )
            {
                bytes = st.st_size - read_size;
            }

            int len = fread( data, 1, bytes, fp );

            if( len == 0 )
            {
                break;
            }

            data += len;
            read_size += len;
        }

        *pulDataSize = st.st_size;
        fclose( fp );
    }

    return ulReturn;
}

/**
 * @brief Cleanup after PKCS11_GetObjectValue().
 *
 * @param[in] pucData       The buffer to free.
 *                          (*ppucData from PKCS11_PAL_GetObjectValue())
 * @param[in] ulDataSize    The length of the buffer to free.
 *                          (*pulDataSize from PKCS11_PAL_GetObjectValue())
 */
void PKCS11_PAL_GetObjectValueCleanup( uint8_t * pucData,
                                       uint32_t ulDataSize )
{
    /* Unused parameters. */
    ( void ) ulDataSize;

    if( pucData != NULL )
    {
        vPortFree( pucData );
    }
}
