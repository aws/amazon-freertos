// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Amazon FreeRTOS PKCS#11 for for ESP32-DevKitC ESP-WROVER-KIT V1.0.0
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

static const char *TAG = "PKCS11";

/*-----------------------------------------------------------*/

static void initialize_spiffs()
{
    static bool spiffs_inited;

    if (spiffs_inited == true) {
        return;
    }

    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%d)", ret);
        }
        return;
    }

    spiffs_inited = true;
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information");
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    return;
}

/**
 * @brief Writes a file to local storage.
 */
BaseType_t PKCS11_PAL_SaveFile( char * pcFileName,
        uint8_t * pucData,
        uint32_t ulDataSize )
{
    initialize_spiffs();

    FILE *fp;
    char fname[32];

    ESP_LOGD(TAG, "Writing file %s, %d bytes", pcFileName, ulDataSize);
    snprintf(fname, sizeof(fname), "/spiffs/%s", pcFileName);
    fp = fopen(fname, "w+");
    if (fp == NULL) {
        return pdFALSE;
    }

    int written_size = 0;
    int bytes = 256;
    while (written_size < ulDataSize) {
        if (bytes > (ulDataSize - written_size)) {
            bytes = ulDataSize - written_size;
        }

        int len = fwrite(pucData, 1, bytes, fp);
        pucData += len;
        written_size += len;
    }

    fclose(fp);
    return pdTRUE;
}

/*-----------------------------------------------------------*/

/**
 * @brief Reads a file from local storage.
 */
BaseType_t PKCS11_PAL_ReadFile( char * pcFileName,
        uint8_t ** ppucData,
        uint32_t * pulDataSize )
{
    initialize_spiffs();

    FILE *fp;
    char fname[32];

    ESP_LOGD(TAG, "Reading file %s", pcFileName);
    snprintf(fname, sizeof(fname), "/spiffs/%s", pcFileName);

    // Check if destination file exists before renaming
    struct stat st;
    if (stat(fname, &st) != 0) {
        return pdFALSE;
    }

    uint8_t *data =	pvPortMalloc(st.st_size);
    if (data == NULL) {
        ESP_LOGE(TAG, "malloc failed");
        return pdFALSE;
    }
    *ppucData = data;

    fp = fopen(fname, "r");
    if (fp == NULL) {
        return pdFALSE;
    }

    int read_size = 0;
    int bytes = 256;
    while (read_size < st.st_size) {
        if (bytes > (st.st_size - read_size)) {
            bytes = st.st_size - read_size;
        }

        int len = fread(data, 1, bytes, fp);
        data += len;
        read_size += len;
    }

    *pulDataSize = st.st_size;
    fclose(fp);
    return pdTRUE;
}

/**
 * @brief Cleanup after PKCS11_ReadFile().
 *
 * @param[in] pucBuffer The buffer to free.
 * @param[in] ulBufferSize The length of the above buffer.
 */
void PKCS11_PAL_ReleaseFileData( uint8_t * pucBuffer,
                                 uint32_t ulBufferSize )
{
    /* Unused parameters. */
    ( void ) ulBufferSize;

    vPortFree( pucBuffer );
}
