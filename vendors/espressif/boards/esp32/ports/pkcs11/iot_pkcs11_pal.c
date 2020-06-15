// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// FreeRTOS PKCS #11 PAL for ESP32-DevKitC ESP-WROVER-KIT V1.0.3
// Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file iot_pkcs11_pal.c
 * @brief PKCS11 Interface.
 */

/* PKCS#11 Interface Include. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "iot_crypto.h"
#include "iot_pkcs11.h"
#include "iot_pkcs11_config.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_flash_encrypt.h"
#include "nvs_flash.h"

#define NVS_PART_NAME                             pkcs11configSTORAGE_PARTITION
#define NAMESPACE                                 pkcs11configSTORAGE_NS
static const char *TAG = "PKCS11";

#define pkcs11palFILE_NAME_CLIENT_CERTIFICATE    "P11_Cert"
#define pkcs11palFILE_NAME_KEY                   "P11_Key"
#define pkcs11palFILE_CODE_SIGN_PUBLIC_KEY       "P11_CSK"
#define pkcs11palFILE_JITP_CERTIFICATE           "P11_JITP"

enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey,
    eAwsJITPCertificate
};
/*-----------------------------------------------------------*/

static void initialize_nvs_partition()
{
    static bool nvs_inited;

    portENTER_CRITICAL();
    if (nvs_inited == true) {
        portEXIT_CRITICAL();
        return;
    }

    ESP_LOGI(TAG, "Initializing NVS partition: \"%s\"", NVS_PART_NAME);



#if CONFIG_NVS_ENCRYPTION
    if (esp_flash_encryption_enabled()) {
        const esp_partition_t *key_part = esp_partition_find_first(
                ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS_KEYS, NULL);
        assert(key_part && "NVS key partition not found");

        nvs_sec_cfg_t cfg;
        esp_err_t err = nvs_flash_read_security_cfg(key_part, &cfg);
        if (err == ESP_ERR_NVS_KEYS_NOT_INITIALIZED) {
            ESP_LOGI(TAG, "NVS key partition empty, generating keys");
            nvs_flash_generate_keys(key_part, &cfg);
        } else {
            ESP_ERROR_CHECK(err);
        }

        esp_err_t ret = nvs_flash_secure_init_partition(NVS_PART_NAME, &cfg);
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_LOGW(TAG, "Error initialising the NVS partition [%d]. Erasing the partition.", ret);
            ESP_ERROR_CHECK(nvs_flash_erase_partition(NVS_PART_NAME));
            ret = nvs_flash_secure_init_partition(NVS_PART_NAME, &cfg);
        }
        ESP_ERROR_CHECK(ret);
    } else {
#endif // CONFIG_NVS_ENCRYPTION
        esp_err_t ret = nvs_flash_init_partition(NVS_PART_NAME);
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_LOGW(TAG, "Error initialising the NVS partition [%d]. Erasing the partition.", ret);
            ESP_ERROR_CHECK(nvs_flash_erase_partition(NVS_PART_NAME));
            ret = nvs_flash_init_partition(NVS_PART_NAME);
        }
        ESP_ERROR_CHECK(ret);
#if CONFIG_NVS_ENCRYPTION
    }
#endif // CONFIG_NVS_ENCRYPTION
    nvs_inited = true;
    portEXIT_CRITICAL();

    return;
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
                         pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                         strlen( (char*)pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
        {
            *pcFileName = pkcs11palFILE_NAME_CLIENT_CERTIFICATE;
            *pHandle = eAwsDeviceCertificate;
        }
        else if( 0 == memcmp( pcLabel,
                              pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                              strlen( (char*)pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
        {
            *pcFileName = pkcs11palFILE_NAME_KEY;
            *pHandle = eAwsDevicePrivateKey;
        }
        else if( 0 == memcmp( pcLabel,
                              pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                              strlen( (char*)pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
        {
            *pcFileName = pkcs11palFILE_NAME_KEY;
            *pHandle = eAwsDevicePublicKey;
        }
        else if( 0 == memcmp( pcLabel,
                              pkcs11configLABEL_CODE_VERIFICATION_KEY,
                              strlen( (char*)pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
        {
            *pcFileName = pkcs11palFILE_CODE_SIGN_PUBLIC_KEY;
            *pHandle = eAwsCodeSigningKey;
        }
        else if( 0 == memcmp( pcLabel,
                              pkcs11configLABEL_JITP_CERTIFICATE,
                              strlen( (char*)pkcs11configLABEL_JITP_CERTIFICATE ) ) )
        {
            *pcFileName = pkcs11palFILE_JITP_CERTIFICATE;
            *pHandle = eAwsJITPCertificate;
        }
        else
        {
            *pcFileName = NULL;
            *pHandle = eInvalidHandle;
        }
    }
}

CK_RV PKCS11_PAL_Initialize( void )
{
    return CKR_OK;
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
                                        CK_BYTE_PTR pucData,
                                        CK_ULONG ulDataSize )
{
    initialize_nvs_partition();

    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    char * pcFileName = NULL;

    /* Translate from the PKCS#11 label to local storage file name. */
    prvLabelToFilenameHandle( pxLabel->pValue,
                              &pcFileName,
                              &xHandle );

    ESP_LOGD(TAG, "Writing file %s, %d bytes", ( char * ) pcFileName, ( uint32_t ) ulDataSize);
    nvs_handle handle;
    esp_err_t err = nvs_open_from_partition(NVS_PART_NAME, NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed nvs open %d", err);
        return eInvalidHandle;
    }

    err = nvs_set_blob(handle, pcFileName, ( char * ) pucData, ( uint32_t ) ulDataSize);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed nvs set blob %d", err);
        nvs_close(handle);
        return eInvalidHandle;
    }

    nvs_close(handle);
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
CK_OBJECT_HANDLE PKCS11_PAL_FindObject( CK_BYTE_PTR pxLabel,
                                        CK_ULONG usLength )
{
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    char * pcFileName = NULL;

    initialize_nvs_partition();

    /* Translate from the PKCS#11 label to local storage file name. */
    prvLabelToFilenameHandle( pxLabel,
                              &pcFileName,
                              &xHandle );

    if( pcFileName != NULL )
    {
        ESP_LOGD( TAG, "Finding file %s", pcFileName );
        nvs_handle handle;
        esp_err_t err = nvs_open_from_partition(NVS_PART_NAME, NAMESPACE, NVS_READONLY, &handle);
        if (err != ESP_OK) {
            /* This can happen if namespace doesn't exist yet, so no files stored */
            ESP_LOGD(TAG, "failed nvs open %d", err);
            return eInvalidHandle;
        }

        size_t required_size = 0;
        err = nvs_get_blob(handle, pcFileName, NULL, &required_size);
        if (err != ESP_OK || required_size == 0) {
            ESP_LOGE(TAG, "failed nvs get file size %d %d", err, required_size);
            xHandle = eInvalidHandle;
        }
        nvs_close(handle);
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
                                      CK_BYTE_PTR * ppucData,
                                      CK_ULONG_PTR pulDataSize,
                                      CK_BBOOL * pIsPrivate )
{
    initialize_nvs_partition();

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
    else if( xHandle == eAwsJITPCertificate )
    {
        pcFileName = pkcs11palFILE_JITP_CERTIFICATE;
        *pIsPrivate = CK_FALSE;
    }
    else
    {
        ulReturn = CKR_OBJECT_HANDLE_INVALID;
    }

    if (ulReturn == CKR_OK)
    {
        ESP_LOGD(TAG, "Reading file %s", pcFileName);
        nvs_handle handle;
        esp_err_t err = nvs_open_from_partition(NVS_PART_NAME, NAMESPACE, NVS_READONLY, &handle);
        if (err != ESP_OK) {
            /* This can happen if namespace doesn't exist yet, so no files stored */
            ESP_LOGD(TAG, "failed nvs open %d", err);
            return CKR_OBJECT_HANDLE_INVALID;
        }

        size_t required_size = 0;
        err = nvs_get_blob(handle, pcFileName, NULL, &required_size);
        if (err != ESP_OK || required_size == 0) {
            ESP_LOGE(TAG, "failed nvs get file size %d %d", err, required_size);
            ulReturn = CKR_OBJECT_HANDLE_INVALID;
            goto done;
        }

        uint8_t *data = pvPortMalloc(required_size);
        if (data == NULL) {
            ESP_LOGE(TAG, "malloc failed");
            ulReturn = CKR_HOST_MEMORY;
            goto done;
        }
        *ppucData = data;

        err = nvs_get_blob(handle, pcFileName, data, &required_size);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "failed nvs get file %d", err);
            vPortFree(data);
            ulReturn = CKR_FUNCTION_FAILED;
            goto done;
        }

        *pulDataSize = required_size;
done:
        nvs_close(handle);
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
void PKCS11_PAL_GetObjectValueCleanup( CK_BYTE_PTR pucData,
                                       CK_ULONG ulDataSize )
{
    /* Unused parameters. */
    ( void ) ulDataSize;

    if( pucData != NULL )
    {
        vPortFree( pucData );
    }
}
