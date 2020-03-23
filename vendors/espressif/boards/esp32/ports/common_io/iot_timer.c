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
#include "stdlib.h"
#include <iot_timer.h>
#include <esp_timer.h>
#include <esp_err.h>
#include "esp_log.h"

#define MS_TO_US    1000

static const char *TAG = "esp-iot-timer";
static volatile bool is_timer_open = false;

typedef struct {
    uint64_t timeout_us;
    uint64_t start_time;
    esp_timer_handle_t esp_timer_handle;
    bool status;
    void (*func)(void *arg);
    void *arg;
} timer_ctx_t;

IotTimerHandle_t iot_timer_open( int32_t lTimerInstance )
{
    if (lTimerInstance != 0) {
        ESP_LOGD(TAG, "Invalid instance of timer");
        return NULL;
    }
    timer_ctx_t *tm_ctx = (timer_ctx_t *) calloc(1, sizeof(timer_ctx_t));
    if (tm_ctx == NULL) {
        ESP_LOGD(TAG, "Could not allocate memory for the timer");
        return NULL;
    }
    is_timer_open = true;
    return (IotTimerHandle_t) tm_ctx;
}

static void timer_cb(void *arg)
{
    timer_ctx_t *tm_ctx = (timer_ctx_t *) arg;
    if (tm_ctx->func) {
        tm_ctx->func(tm_ctx->arg);
        tm_ctx->func = NULL;
    }
}

void iot_timer_set_callback( IotTimerHandle_t const pxTimerHandle,
                             IotTimerCallback_t xCallback,
                             void *pvUserContext )
{
    if (pxTimerHandle == NULL) {
        ESP_LOGD(TAG, "Invalid Timer Handler %s", __func__);
        return;
    }
    esp_err_t ret;
    timer_ctx_t *tm_ctx = (timer_ctx_t *) pxTimerHandle;

    tm_ctx->func = xCallback;
    tm_ctx->arg = pvUserContext;

    esp_timer_create_args_t timer_arg = {
        .callback = (esp_timer_cb_t)timer_cb,
        .arg = pxTimerHandle,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "esp32 timer",
    };

    if (tm_ctx->esp_timer_handle != NULL) {
        ESP_LOGD(TAG, "Callback has initialised");
        return;
    }

    ret = esp_timer_create(&timer_arg, &tm_ctx->esp_timer_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create esp timer %d", ret);
        return;
    }
}

int32_t iot_timer_start( IotTimerHandle_t const pxTimerHandle )
{
    if (pxTimerHandle == NULL) {
        ESP_LOGD(TAG, "Invalid Timer Handler %s", __func__);
        return IOT_TIMER_INVALID_VALUE;
    }

    esp_err_t ret;
    timer_ctx_t *tm_ctx = (timer_ctx_t *) pxTimerHandle;

    if (tm_ctx->status == true) {
        return IOT_TIMER_SUCCESS;
    }
    if (tm_ctx->esp_timer_handle && tm_ctx->timeout_us) {
        ret = esp_timer_start_once(tm_ctx->esp_timer_handle, tm_ctx->timeout_us);
        if (ret != ESP_OK) {
            ESP_LOGD(TAG, "Failed to start esp timer %d", ret);
            return IOT_TIMER_SET_FAILED;
        }
    }
    tm_ctx->start_time = esp_timer_get_time();
    tm_ctx->status = true;
    return IOT_TIMER_SUCCESS;
}

int32_t iot_timer_stop( IotTimerHandle_t const pxTimerHandle )
{
    if (pxTimerHandle == NULL) {
        ESP_LOGD(TAG, "Invalid Timer Handler %s", __func__);
        return IOT_TIMER_INVALID_VALUE;
    }
    esp_err_t ret = ESP_OK;
    timer_ctx_t *tm_ctx = (timer_ctx_t *) pxTimerHandle;
    if (tm_ctx->esp_timer_handle != NULL) {
        ret = esp_timer_stop(tm_ctx->esp_timer_handle);
        if (ret != ESP_OK) {
            ESP_LOGD(TAG, "Failed to stop esp timer %d", ret);
            return IOT_TIMER_FUNCTION_NOT_SUPPORTED;
        }
    }
    tm_ctx->status = false;
    return (ret == ESP_OK) ? IOT_TIMER_SUCCESS : IOT_TIMER_FUNCTION_NOT_SUPPORTED;
}

int32_t iot_timer_get_value( IotTimerHandle_t const pxTimerHandle,
                             uint64_t *ulMicroSeconds )
{
    if (pxTimerHandle == NULL) {
        ESP_LOGD(TAG, "Invalid Timer Handler %s", __func__);
        return IOT_TIMER_INVALID_VALUE;
    }

    if (ulMicroSeconds == NULL) {
        ESP_LOGE(TAG, "NULL buffer");
        return IOT_TIMER_INVALID_VALUE;
    }
    timer_ctx_t *tm_ctx = (timer_ctx_t *) pxTimerHandle;
    if (tm_ctx->status == false) {
        return IOT_TIMER_NOT_RUNNING;
    }

    *ulMicroSeconds = esp_timer_get_time();
    return IOT_TIMER_SUCCESS;
}

int32_t iot_timer_delay( IotTimerHandle_t const pxTimerHandle,
                         uint32_t ulDelayMicroSeconds )
{
    if (pxTimerHandle == NULL) {
        ESP_LOGD(TAG, "Invalid Timer Handler %s", __func__);
        return IOT_TIMER_INVALID_VALUE;
    }

    timer_ctx_t *tm_ctx = (timer_ctx_t *) pxTimerHandle;
    tm_ctx->timeout_us = (uint64_t) ulDelayMicroSeconds;
    return IOT_TIMER_SUCCESS;
}

int32_t iot_timer_close( IotTimerHandle_t const pxTimerHandle )
{
    if (pxTimerHandle == NULL) {
        ESP_LOGD(TAG, "Invalid Timer Handler %s", __func__);
        return IOT_TIMER_INVALID_VALUE;
    }

    if (!is_timer_open) {
        ESP_LOGD(TAG, "Timer Handler is not open");
        return IOT_TIMER_INVALID_VALUE;
    }
    is_timer_open = false;

    esp_err_t ret = ESP_OK;
    timer_ctx_t *tm_ctx = (timer_ctx_t *) pxTimerHandle;
    if (tm_ctx->status) {
        if (tm_ctx->esp_timer_handle != NULL) {
            ret = esp_timer_stop(tm_ctx->esp_timer_handle);
            if (ret == ESP_ERR_INVALID_STATE) {
                ESP_LOGD(TAG, "Timer not armed ?");
            }
            ret = esp_timer_delete(tm_ctx->esp_timer_handle);
            if (ret != ESP_OK) {
                ESP_LOGD(TAG, "Failed to close esp timer %d", ret);
                return IOT_TIMER_INVALID_VALUE;
            }
        }
    }
    free(tm_ctx);
    return IOT_TIMER_SUCCESS;
}

int32_t iot_timer_cancel( IotTimerHandle_t const pxTimerHandle )
{
    if (pxTimerHandle == NULL) {
        ESP_LOGD(TAG, "Invalid Timer Handler %s", __func__);
        return IOT_TIMER_INVALID_VALUE;
    }
    esp_err_t ret = ESP_OK;
    timer_ctx_t *tm_ctx = (timer_ctx_t *) pxTimerHandle;
    if (tm_ctx->esp_timer_handle != NULL) {
        ret = esp_timer_stop(tm_ctx->esp_timer_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to cancel timer %d", ret);
        }
    }
    return (ret == ESP_OK) ? IOT_TIMER_SUCCESS : IOT_TIMER_INVALID_VALUE;
}

