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

#include <esp_spi_flash.h>
#include <iot_flash.h>

#define SPI_FLASH_BLOCK_SIZE 0x10000
static IotFlashInfo_t FL_Info;
static bool is_flash_drv_open;

IotFlashHandle_t iot_flash_open(int32_t lInstance)
{
    if (lInstance == -1) {
        return NULL;
    }
    // Since architecture is based on XIP, flash is already initialized
    // Underlying driver takes care of concurrency in flash usage
    is_flash_drv_open = true;
    return (IotFlashHandle_t) 1;
}

int32_t iot_flash_close(IotFlashHandle_t const pxFlash)
{
    if (is_flash_drv_open != true) {
        return IOT_FLASH_INVALID_VALUE;
    }
    is_flash_drv_open = false;
    return IOT_FLASH_SUCCESS;
}

IotFlashInfo_t *iot_flash_getinfo(IotFlashHandle_t const pxFlash)
{
    if (pxFlash == NULL) {
        return NULL;
    }

    FL_Info.ulFlashSize = spi_flash_get_chip_size();
    FL_Info.ulBlockSize = SPI_FLASH_BLOCK_SIZE;
    FL_Info.ulSectorSize = SPI_FLASH_SEC_SIZE;
    FL_Info.ulPageSize = 256;
    FL_Info.ulLockSupportSize = SPI_FLASH_BLOCK_SIZE;
    FL_Info.ucAsyncSupported = false;

    return &FL_Info;
}

void iot_flash_set_callback(IotFlashHandle_t const pxFlash, IotFlashCallback_t xCallback, void *pvUserContext)
{
    // Not supported for ESP32
    (void) pxFlash;
    (void) xCallback;
    (void) pvUserContext;
}

int32_t iot_flash_erase_sectors(IotFlashHandle_t const pxFlash, uint32_t ulAddress, size_t lSize)
{
    if (pxFlash == NULL) {
        return IOT_FLASH_INVALID_VALUE;
    }

    if ((ulAddress % SPI_FLASH_SEC_SIZE) != 0 || (lSize % SPI_FLASH_SEC_SIZE) != 0) {
        return IOT_FLASH_INVALID_VALUE;
    }

    esp_err_t ret = spi_flash_erase_range(ulAddress, lSize);
    return (ret == ESP_OK) ? IOT_FLASH_SUCCESS : IOT_FLASH_ERASE_FAILED;
}

int32_t iot_flash_erase_chip(IotFlashHandle_t const pxFlash)
{
    if (pxFlash == NULL) {
        return IOT_FLASH_INVALID_VALUE;
    }

    int flash_size = spi_flash_get_chip_size();
    esp_err_t ret = spi_flash_erase_range(0, flash_size);
    return (ret == ESP_OK) ? IOT_FLASH_SUCCESS : IOT_FLASH_ERASE_FAILED;
}

int32_t iot_flash_write_sync(IotFlashHandle_t const pxFlash, uint32_t ulAddress, uint8_t *const pvBuffer, size_t xBytes)
{
    if (pxFlash == NULL || (int) ulAddress < 0 || pvBuffer == NULL) {
        return IOT_FLASH_INVALID_VALUE;
    }

    esp_err_t ret = spi_flash_write(ulAddress, pvBuffer, xBytes);
    return (ret == ESP_OK) ? IOT_FLASH_SUCCESS : IOT_FLASH_WRITE_FAILED;
}

int32_t iot_flash_read_sync(IotFlashHandle_t const pxFlash, uint32_t ulAddress, uint8_t *const pvBuffer, size_t xBytes)
{
    if (pxFlash == NULL || (int) ulAddress < 0 || pvBuffer == NULL) {
        return IOT_FLASH_INVALID_VALUE;
    }

    esp_err_t ret = spi_flash_read(ulAddress, pvBuffer, xBytes);
    return (ret == ESP_OK) ? IOT_FLASH_SUCCESS : IOT_FLASH_READ_FAILED;
}

int32_t iot_flash_ioctl(IotFlashHandle_t const pxFlash, IotFlashIoctlRequest_t xRequest, void *const pvBuffer)
{
    (void) pvBuffer;

    if (pxFlash == NULL || (int) xRequest < 0) {
        return IOT_FLASH_INVALID_VALUE;
    }

    return IOT_FLASH_FUNCTION_NOT_SUPPORTED;
}
