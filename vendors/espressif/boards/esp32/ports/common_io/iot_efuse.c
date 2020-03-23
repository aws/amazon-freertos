/*
 * Copyright 2020 Espressif Systems (Shanghai) Co., Ltd.
 * All rights reserved.
 *
 * You may not use this file except in compliance with the terms and
 * conditions set forth in the accompanying LICENSE.TXT file.
 *
 * THESE MATERIALS ARE PROVIDED ON AN "AS IS" BASIS. ESPRESSIF/AMAZON SPECIFICALLY
 * DISCLAIMS, WITH RESPECT TO THESE MATERIALS, ALL WARRANTIES, EXPRESS,
 * IMPLIED, OR STATUTORY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <iot_efuse.h>
#include <esp_efuse.h>

#define VALUE_16    16
#define VALUE_32    32

static bool efuse_inited;
static esp_efuse_desc_t *efuse_ctx[] = {NULL, NULL};

IotEfuseHandle_t iot_efuse_open(void)
{
    if (efuse_inited) {
        goto end;
    }
    efuse_ctx[0] = (esp_efuse_desc_t *)calloc(1, sizeof(esp_efuse_desc_t));
    if (efuse_ctx[0] == NULL) {
        return NULL;
    }
    efuse_ctx[0]->efuse_block = EFUSE_BLK3;
    efuse_inited = true;
end:
    return (IotEfuseHandle_t)efuse_ctx[0];
}

int32_t iot_efuse_close(IotEfuseHandle_t const pxEfuseHandle)
{
    if (pxEfuseHandle == NULL || !efuse_inited) {
        return IOT_EFUSE_INVALID_VALUE;
    }
    efuse_inited = false;
    free(pxEfuseHandle);
    efuse_ctx[0] = NULL;
    return IOT_EFUSE_SUCCESS;
}

int32_t iot_efuse_read_32bit_word( IotEfuseHandle_t const pxEfuseHandle,
                                   uint32_t ulIndex,
                                   uint32_t *ulValue)
{
    if (pxEfuseHandle == NULL || ulIndex > 7 || ulValue == NULL) {
        return IOT_EFUSE_INVALID_VALUE;
    }

    esp_efuse_desc_t *read_instance = (esp_efuse_desc_t *)pxEfuseHandle;
    read_instance->bit_start = (ulIndex * VALUE_32);
    read_instance->bit_count = VALUE_32;
    if (esp_efuse_read_field_blob(efuse_ctx, ulValue, VALUE_32) != IOT_EFUSE_SUCCESS) {
        return IOT_EFUSE_READ_FAIL;
    }
    return IOT_EFUSE_SUCCESS;
}

int32_t iot_efuse_write_32bit_word( IotEfuseHandle_t const pxEfuseHandle,
                                    uint32_t ulIndex,
                                    uint32_t ulValue)
{
    if (pxEfuseHandle == NULL || ulIndex > 7) {
        return IOT_EFUSE_INVALID_VALUE;
    }

    esp_efuse_desc_t *write_instance = (esp_efuse_desc_t *)pxEfuseHandle;
    write_instance->bit_start = (ulIndex * VALUE_32);
    write_instance->bit_count = VALUE_32;
    if (esp_efuse_write_field_blob(efuse_ctx, &ulValue, VALUE_32) != IOT_EFUSE_SUCCESS) {
        return IOT_EFUSE_WRITE_FAIL;
    }

    return IOT_EFUSE_SUCCESS;
}

int32_t iot_efuse_read_16bit_word( IotEfuseHandle_t const pxEfuseHandle,
                                   uint32_t ulIndex,
                                   uint16_t *ulValue)
{
    if (pxEfuseHandle == NULL || ulIndex > 15 || ulValue == NULL) {
        return IOT_EFUSE_INVALID_VALUE;
    }

    esp_efuse_desc_t *read_instance = (esp_efuse_desc_t *)pxEfuseHandle;
    read_instance->bit_start = (ulIndex * VALUE_16);
    read_instance->bit_count = VALUE_16;
    if (esp_efuse_read_field_blob(efuse_ctx, ulValue, VALUE_16) != IOT_EFUSE_SUCCESS) {
        return IOT_EFUSE_READ_FAIL;
    }

    return IOT_EFUSE_SUCCESS;
}

int32_t iot_efuse_write_16bit_word( IotEfuseHandle_t const pxEfuseHandle,
                                    uint32_t ulIndex,
                                    uint16_t value)
{
    if (pxEfuseHandle == NULL || ulIndex > 15) {
        return IOT_EFUSE_INVALID_VALUE;
    }

    esp_efuse_desc_t *write_instance = (esp_efuse_desc_t *)pxEfuseHandle;
    write_instance->bit_start = (ulIndex * VALUE_16);
    write_instance->bit_count = VALUE_16;

    if (esp_efuse_write_field_blob(efuse_ctx, &value, VALUE_16) != IOT_EFUSE_SUCCESS) {
        return IOT_EFUSE_WRITE_FAIL;
    }
    return IOT_EFUSE_SUCCESS;
}
