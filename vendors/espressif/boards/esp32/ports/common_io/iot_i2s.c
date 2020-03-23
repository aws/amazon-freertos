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

#include "stdint.h"
#include <string.h>
#include <FreeRTOS.h>
#include <freertos/timers.h>
#include <esp_err.h>
#include "esp_log.h"
#include "driver/i2s.h"
#include <iot_i2s.h>

#define SEM_WAIT_TIME (60000 / portTICK_RATE_MS)

const char *TAG = "esp-iot-i2s";

typedef struct {
    uint8_t i2s_port_num;
    void (*func)(IotI2SOperationStatus_t arg1, void *arg2);
    void *arg;
    char *read_buf;
    char *write_buf;
    size_t bytes_to_read;
    size_t bytes_to_write;
    SemaphoreHandle_t i2s_rd_cb_wait;
    SemaphoreHandle_t i2s_wr_cb_wait;
    unsigned is_channel_write_busy: 1;
    unsigned is_channel_read_busy: 1;
    unsigned i2s_op_cancel_req: 1;
} i2s_ctx_t;

IotI2SHandle_t iot_i2s_open(int32_t lI2SInstance)
{
    i2s_ctx_t *i2s_ctx = (i2s_ctx_t *) calloc(1, sizeof(i2s_ctx_t));
    if (i2s_ctx == NULL) {
        ESP_LOGE(TAG, "Could not allocate memory for i2s context");
        return NULL;
    }
    if (lI2SInstance == -1) {
        free(i2s_ctx);
        return NULL;
    }

    i2s_ctx->i2s_rd_cb_wait = xSemaphoreCreateBinary();
    if (i2s_ctx->i2s_rd_cb_wait == NULL) {
        ESP_LOGE(TAG, "Failed to create read binary semaphore");
        free(i2s_ctx);
        return NULL;
    }

    i2s_ctx->i2s_wr_cb_wait = xSemaphoreCreateBinary();
    if (i2s_ctx->i2s_wr_cb_wait == NULL) {
        ESP_LOGE(TAG, "Failed to create write binary semaphore");
        vSemaphoreDelete(i2s_ctx->i2s_rd_cb_wait);
        free(i2s_ctx);
        return NULL;
    }

    /* Ensure first semaphore take could succeed */
    xSemaphoreGive(i2s_ctx->i2s_rd_cb_wait);
    xSemaphoreGive(i2s_ctx->i2s_wr_cb_wait);

    IotI2SHandle_t iot_i2s_handler = (void *) i2s_ctx;
    return iot_i2s_handler;
}

void iot_i2s_set_callback(IotI2SHandle_t const pxI2SPeripheral, IotI2SCallback_t xCallback, void *pvUserContext)
{
    if (pxI2SPeripheral == NULL || xCallback == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
    }
    i2s_ctx_t *i2s_ctx = (i2s_ctx_t *) pxI2SPeripheral;

    i2s_ctx->arg = pvUserContext;
    i2s_ctx->func = (void *)xCallback;

}

static void read_async_cb(void *param, unsigned int ulparam)
{
    uint8_t op_status = 0;
    size_t bytes_read_tmp = 0;
    i2s_ctx_t *i2s_ctx = (i2s_ctx_t *) param;
    int32_t i2s_port_num = i2s_ctx->i2s_port_num;
    char *read_buffer = i2s_ctx->read_buf;
    size_t bytes_read = i2s_ctx->bytes_to_read;

    ESP_LOGD(TAG, "Read async callback invoked %s", __func__);
    while (bytes_read > 0) {
        /* Non blocking i2s read */
        i2s_read(i2s_port_num, read_buffer, bytes_read, &bytes_read_tmp, 0);
        if (i2s_ctx->i2s_op_cancel_req) {
            ESP_LOGD(TAG, "i2s read cancel operation recieved");
            break;
        }
        bytes_read  -= bytes_read_tmp;
        read_buffer += bytes_read_tmp;
    }
    if (i2s_ctx->i2s_op_cancel_req) {
        op_status = eI2SLastReadFailed;
    } else {
        op_status = eI2SCompleted;
    }

    if (i2s_ctx->func) {
        ESP_LOGD(TAG, "%s invoking callback %p", __func__, i2s_ctx->func);
        i2s_ctx->func(op_status, i2s_ctx->arg);
    }

    i2s_ctx->is_channel_read_busy = false;
    i2s_ctx->i2s_op_cancel_req = false;
    xSemaphoreGive(i2s_ctx->i2s_rd_cb_wait);
}

static void write_async_cb(void *param, unsigned int ulparam)
{
    uint8_t op_status = 0;
    size_t bytes_write_tmp = 0;
    i2s_ctx_t *i2s_ctx = (i2s_ctx_t *) param;
    int32_t i2s_port_num = i2s_ctx->i2s_port_num;
    char *write_buffer = i2s_ctx->write_buf;
    size_t bytes_write = i2s_ctx->bytes_to_write;

    ESP_LOGD(TAG, "Write aync callback invoked %s", __func__);
    while (bytes_write > 0) {
        /* Non blocking i2s write */
        i2s_write(i2s_port_num, write_buffer, bytes_write, &bytes_write_tmp, 0);
        if (i2s_ctx->i2s_op_cancel_req) {
            ESP_LOGD(TAG, "i2s write cancel operation recieved");
            break;
        }
        bytes_write  -= bytes_write_tmp;
        write_buffer += bytes_write_tmp;
    }
    if (i2s_ctx->i2s_op_cancel_req) {
        op_status = eI2SLastWriteFailed;
    } else {
        op_status = eI2SCompleted;
    }

    if (i2s_ctx->func) {
        ESP_LOGD(TAG, "%s invoking callback %p", __func__, i2s_ctx->func);
        i2s_ctx->func(op_status, i2s_ctx->arg);
    }

    i2s_ctx->is_channel_write_busy = false;
    i2s_ctx->i2s_op_cancel_req = false;
    xSemaphoreGive(i2s_ctx->i2s_wr_cb_wait);
}

int32_t iot_i2s_read_async(IotI2SHandle_t const pxI2SPeripheral, uint8_t *const pvBuffer, size_t xBytes)
{
    if (pxI2SPeripheral == NULL || pvBuffer == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_I2S_INVALID_VALUE;
    }
    i2s_ctx_t *i2s_ctx = (i2s_ctx_t *) pxI2SPeripheral;
    if (i2s_ctx->is_channel_read_busy) {
        ESP_LOGD(TAG, "I2S bus busy");
        return IOT_I2S_BUSY;
    }
    i2s_ctx->read_buf = (char *)pvBuffer;
    i2s_ctx->bytes_to_read = xBytes;
    //Read from another task to make async
    if (!xSemaphoreTake(i2s_ctx->i2s_rd_cb_wait, SEM_WAIT_TIME)) {
        ESP_LOGE(TAG, "%s: failed to acquire read sem", __func__);
        return IOT_I2S_READ_FAILED;
    }
    if (xTimerPendFunctionCall(read_async_cb, (void *)i2s_ctx, 0, 0) != pdTRUE) {
        ESP_LOGE(TAG, "%s: failed to create timer for read async", __func__);
        xSemaphoreGive(i2s_ctx->i2s_rd_cb_wait);
        return IOT_I2S_READ_FAILED;
    }
    i2s_ctx->is_channel_read_busy = true;
    return IOT_I2S_SUCCESS;
}

int32_t iot_i2s_write_async(IotI2SHandle_t const pxI2SPeripheral, uint8_t *const pvBuffer, size_t xBytes)
{

    if (pxI2SPeripheral == NULL || pvBuffer == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_I2S_INVALID_VALUE;
    }
    i2s_ctx_t *i2s_ctx = (i2s_ctx_t *) pxI2SPeripheral;
    if (i2s_ctx->is_channel_write_busy) {
        ESP_LOGD(TAG, "I2S bus busy");
        return IOT_I2S_BUSY;
    }
    i2s_ctx->write_buf = (char *)pvBuffer;
    i2s_ctx->bytes_to_write = xBytes;
    //Write from another task to make async
    if (!xSemaphoreTake(i2s_ctx->i2s_wr_cb_wait, SEM_WAIT_TIME)) {
        ESP_LOGE(TAG, "%s: failed to acquire write sem", __func__);
        return IOT_I2S_WRITE_FAILED;
    }
    if (xTimerPendFunctionCall(write_async_cb, (void *)i2s_ctx, 0, 0) != pdTRUE) {
        ESP_LOGE(TAG, "%s: failed to create timer for write async", __func__);
        xSemaphoreGive(i2s_ctx->i2s_wr_cb_wait);
        return IOT_I2S_WRITE_FAILED;
    }
    i2s_ctx->is_channel_write_busy = true;
    return IOT_I2S_SUCCESS;
}

int32_t iot_i2s_read_sync(IotI2SHandle_t const pxI2SPeripheral, uint8_t *const pvBuffer, size_t xBytes)
{
    if (pxI2SPeripheral == NULL || pvBuffer == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_I2S_INVALID_VALUE;
    }
    size_t bytes_written = 0;
    char *src_buf = (char *) pvBuffer;
    i2s_ctx_t *i2s_ctx = (i2s_ctx_t *) pxI2SPeripheral;
    int32_t i2s_port_num = i2s_ctx->i2s_port_num;
    esp_err_t ret = i2s_read(i2s_port_num, src_buf, xBytes, &bytes_written, portMAX_DELAY);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2s read sync fail %d", ret);
        return IOT_I2S_READ_FAILED;
    }
    return (bytes_written == xBytes) ? IOT_I2S_SUCCESS : IOT_I2S_READ_FAILED;
}

int32_t iot_i2s_write_sync(IotI2SHandle_t const pxI2SPeripheral, uint8_t *const pvBuffer, size_t xBytes)
{
    if (pxI2SPeripheral == NULL || pvBuffer == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_I2S_INVALID_VALUE;
    }
    size_t bytes_written = 0;
    char *src_buf = (char *) pvBuffer;
    i2s_ctx_t *i2s_ctx = (i2s_ctx_t *) pxI2SPeripheral;
    int32_t i2s_port_num = i2s_ctx->i2s_port_num;
    esp_err_t ret = i2s_write(i2s_port_num, src_buf, xBytes, &bytes_written, portMAX_DELAY);
    return (bytes_written == xBytes) ? IOT_I2S_SUCCESS : IOT_I2S_WRITE_FAILED;
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2s write sync fail %d", ret);
        return IOT_I2S_WRITE_FAILED;
    }
}

int32_t iot_i2s_close( IotI2SHandle_t const pxI2SPeripheral)
{
    esp_err_t ret;
    if (pxI2SPeripheral == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_I2S_INVALID_VALUE;
    }
    i2s_ctx_t *i2s_ctx = (i2s_ctx_t *) pxI2SPeripheral;
    int32_t i2s_port_num = i2s_ctx->i2s_port_num;
    ret = i2s_zero_dma_buffer(i2s_port_num);
    ret |= i2s_driver_uninstall(i2s_port_num);
    free(pxI2SPeripheral);
    return (ret == ESP_OK) ? IOT_I2S_SUCCESS : IOT_I2S_INVALID_VALUE;

}

int32_t iot_i2s_ioctl(IotI2SHandle_t const pxI2SPeripheral, IotI2SIoctlRequest_t xI2SRequest, void *const pvBuffer)
{
    esp_err_t ret;
    if (pxI2SPeripheral == NULL || pvBuffer == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_I2S_INVALID_VALUE;
    }
    if (xI2SRequest != eI2SSetConfig && xI2SRequest != eI2SGetBusState) {
        ESP_LOGE(TAG, "Expected request not correct: %d", xI2SRequest);
        return IOT_I2S_INVALID_VALUE;
    }
    i2s_ctx_t *i2s_ctx = (i2s_ctx_t *) pxI2SPeripheral;
    if (xI2SRequest == eI2SSetConfig) {
        if (i2s_ctx->is_channel_write_busy || i2s_ctx->is_channel_read_busy) {
            ESP_LOGD(TAG, "I2S bus busy");
            return IOT_I2S_BUSY;
        }
        int32_t i2s_port_num = i2s_ctx->i2s_port_num;
        IotI2sIoctlConfig_t *iot_i2s_config = (IotI2sIoctlConfig_t *) pvBuffer;
        i2s_config_t i2s_config = {
            .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX,
            .sample_rate = iot_i2s_config->ulI2SFrequency,
            .bits_per_sample = iot_i2s_config->ulI2SFrameLength,
            .channel_format = iot_i2s_config->xI2SChannel,
            .communication_format = iot_i2s_config->xI2SMode,
            .dma_buf_count = 3,                   /*!< number of dma buffer */
            .dma_buf_len = 300,                   /*!< size of each dma buffer (Byte) */
            .intr_alloc_flags = 0,
            .use_apll = 0,
        };
        i2s_pin_config_t i2s_pin_config = {
            .bck_io_num = GPIO_NUM_5,
            .ws_io_num =  GPIO_NUM_25,
            .data_out_num = GPIO_NUM_26,
            .data_in_num = GPIO_NUM_35,
        };
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
        SET_PERI_REG_BITS(PIN_CTRL, CLK_OUT1, 0, CLK_OUT1_S);
        ret = i2s_driver_install(i2s_port_num, &i2s_config, 0, NULL);
        ret |= i2s_set_pin(i2s_port_num, &i2s_pin_config);
        ret |= i2s_zero_dma_buffer(i2s_port_num);
        ESP_LOGD(TAG, "Setting I2S configuration to frequency: %d, framelength: %d, mode: %d, channel: %d",
            iot_i2s_config->ulI2SFrequency, iot_i2s_config->ulI2SFrameLength, iot_i2s_config->xI2SChannel,
            iot_i2s_config->xI2SMode);
        return (ret == ESP_OK) ? IOT_I2S_SUCCESS : IOT_I2S_INVALID_VALUE;
    }
    if (xI2SRequest == eI2SGetBusState) {
        if (i2s_ctx->is_channel_write_busy || i2s_ctx->is_channel_read_busy) {
            ESP_LOGD(TAG, "I2S bus busy");
            return IOT_I2S_BUSY;
        } else {
            return IOT_I2S_SUCCESS;
        }
    }
    return IOT_I2S_NOT_INITIALIZED;
}

int32_t iot_i2s_cancel(IotI2SHandle_t const pxI2SPeripheral)
{
    esp_err_t ret;
    if (pxI2SPeripheral == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_I2S_INVALID_VALUE;
    }
    i2s_ctx_t *i2s_ctx = (i2s_ctx_t *) pxI2SPeripheral;
    int32_t i2s_port_num = i2s_ctx->i2s_port_num;
    if (!i2s_ctx->is_channel_read_busy && !i2s_ctx->is_channel_write_busy) {
        return IOT_I2S_NOTHING_TO_CANCEL;
    }
    if (i2s_ctx->is_channel_read_busy) {
        i2s_ctx->i2s_op_cancel_req = true;
        ESP_LOGD(TAG, "operation cancel request: %d\n", i2s_ctx->i2s_op_cancel_req);
        /* Ensure no active operations on i2s read before stoping i2s */
        if (!xSemaphoreTake(i2s_ctx->i2s_rd_cb_wait, SEM_WAIT_TIME)) {
            ESP_LOGE(TAG, "%s: failed to acquire read sem", __func__);
            return IOT_I2S_INVALID_VALUE;
        }
        xSemaphoreGive(i2s_ctx->i2s_rd_cb_wait);
        i2s_ctx->i2s_op_cancel_req = false;
    }
    if (i2s_ctx->is_channel_write_busy) {
        i2s_ctx->i2s_op_cancel_req = true;
        ESP_LOGD(TAG, "operation cancel request: %d\n", i2s_ctx->i2s_op_cancel_req);
        /* Ensure no active operations on i2s write before stoping i2s */
        if (!xSemaphoreTake(i2s_ctx->i2s_wr_cb_wait, SEM_WAIT_TIME)) {
            ESP_LOGE(TAG, "%s: failed to acquire read sem", __func__);
            return IOT_I2S_INVALID_VALUE;
        }
        xSemaphoreGive(i2s_ctx->i2s_wr_cb_wait);
        i2s_ctx->i2s_op_cancel_req = false;
    }

    ret = i2s_zero_dma_buffer(i2s_port_num);
    ret |= i2s_stop(i2s_port_num);
    return (ret == ESP_OK) ? IOT_I2S_SUCCESS : IOT_I2S_INVALID_VALUE;
}
