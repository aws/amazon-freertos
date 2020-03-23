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
#include <string.h>
#include "iot_watchdog.h"
#include "esp_log.h"
#include "soc/rtc_wdt.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/rtc_cntl.h"

static const char *TAG = "esp-hal-wdt";

typedef struct {
    uint32_t wdt_bark_time;
    uint32_t wdt_bite_time;
    void (*func)(void *arg);
    void *arg;
    uint8_t wdt_instance_num;
    IotWatchdogStatus_t status;
} wdt_ctx_t;

static volatile uint8_t wdt_status_bitmask;

IotWatchdogHandle_t iot_watchdog_open( int32_t lWatchdogInstance )
{
    IotWatchdogHandle_t awswdtPerripheralHandle = NULL;
    if (lWatchdogInstance == -1) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return NULL;
    }
    if (!(0x01 & wdt_status_bitmask >> lWatchdogInstance)) {
        wdt_ctx_t *wdt_ctx = (wdt_ctx_t *) calloc(1, sizeof(wdt_ctx_t));
        if (wdt_ctx == NULL) {
            ESP_LOGE(TAG, "Could not allocate memory for WDT context");
            return NULL;
        }
        wdt_ctx->status = eWatchdogTimerStopped;
        wdt_ctx->wdt_instance_num = lWatchdogInstance;
        awswdtPerripheralHandle = (void *) wdt_ctx;
        wdt_status_bitmask = wdt_status_bitmask | BIT(lWatchdogInstance);
        return awswdtPerripheralHandle;
    }
    return NULL;
}

int32_t iot_watchdog_ioctl( IotWatchdogHandle_t const pxWatchdogHandle,
                            IotWatchdogIoctlRequest_t xRequest,
                            void * const pvBuffer )
{
    if (pxWatchdogHandle == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_WATCHDOG_INVALID_VALUE;
    }
    if (pvBuffer == NULL) {
        ESP_LOGE(TAG, "Invalid buffer %s", __func__);
        return IOT_WATCHDOG_INVALID_VALUE;
    }

    wdt_ctx_t *wdt_ctx = (wdt_ctx_t *) pxWatchdogHandle;
    uint32_t *tmp_buf = (uint32_t *) pvBuffer;
    //Bite is stage 1 and bark is stage 0
    switch (xRequest) {
    case eSetWatchdogBarkTime :
        if (wdt_ctx->wdt_bite_time && wdt_ctx->wdt_bite_time < *tmp_buf) {
            ESP_LOGE(TAG, "bark time %d should be less then bite time %d", *tmp_buf, wdt_ctx->wdt_bite_time);
            return IOT_WATCHDOG_INVALID_VALUE;
        }
        wdt_ctx->wdt_bark_time = *tmp_buf;
        rtc_wdt_protect_off();
        rtc_wdt_set_length_of_reset_signal(RTC_WDT_SYS_RESET_SIG, RTC_WDT_LENGTH_1_6us);
        rtc_wdt_set_time(RTC_WDT_STAGE0, wdt_ctx->wdt_bark_time);
        rtc_wdt_set_stage(RTC_WDT_STAGE0, RTC_WDT_STAGE_ACTION_INTERRUPT);
        rtc_wdt_protect_on();
        ESP_LOGD(TAG, "request: %d, bark time %d bite time %d", wdt_ctx->wdt_bark_time, wdt_ctx->wdt_bite_time);
        break;
    case eSetWatchdogBiteTime :
        if (wdt_ctx->wdt_bark_time && wdt_ctx->wdt_bark_time > *tmp_buf) {
            ESP_LOGE(TAG, "bite time %d should be greater then bite time %d", *tmp_buf, wdt_ctx->wdt_bite_time);
            return IOT_WATCHDOG_INVALID_VALUE;
        }
        wdt_ctx->wdt_bite_time = *tmp_buf;
        rtc_wdt_protect_off();
        rtc_wdt_set_length_of_reset_signal(RTC_WDT_SYS_RESET_SIG, RTC_WDT_LENGTH_1_6us);
        rtc_wdt_set_time(RTC_WDT_STAGE1, wdt_ctx->wdt_bite_time);
        rtc_wdt_set_stage(RTC_WDT_STAGE1, RTC_WDT_STAGE_ACTION_RESET_RTC);
        rtc_wdt_protect_on();
        ESP_LOGD(TAG, "request: %d, bark time %d bite time %d", wdt_ctx->wdt_bark_time, wdt_ctx->wdt_bite_time);
        break;
    case eGetWatchdogBarkTime :
        *tmp_buf = wdt_ctx->wdt_bark_time;
        break;
    case eGetWatchdogBiteTime :
        *tmp_buf = wdt_ctx->wdt_bite_time;
        break;
    case eGetWatchdogStatus :
        *tmp_buf = wdt_ctx->status;
        ESP_LOGD(TAG, "request: %d, bark time %d bite time %d status %d",
            wdt_ctx->wdt_bark_time, wdt_ctx->wdt_bite_time, wdt_ctx->status);
        break;
    case eSetWatchdogBiteBehaviour :
        if (*tmp_buf == eWatchdogBiteTimerReset) {
            rtc_wdt_set_stage(RTC_WDT_STAGE1, RTC_WDT_STAGE_ACTION_RESET_RTC);
        }
        if (*tmp_buf == eWatchdogBiteTimerInterrupt) {
            return IOT_WATCHDOG_FUNCTION_NOT_SUPPORTED;
        }
        break;
    default :
        ESP_LOGD(TAG, "Expected request is not correct %d", xRequest);
        return IOT_WATCHDOG_INVALID_VALUE;
        break;
    }
    return IOT_WATCHDOG_SUCCESS;
}

int32_t iot_watchdog_start( IotWatchdogHandle_t const pxWatchdogHandle )
{
    if (pxWatchdogHandle == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_WATCHDOG_INVALID_VALUE;
    }
    wdt_ctx_t *wdt_ctx = (wdt_ctx_t *) pxWatchdogHandle;
    if (wdt_ctx->wdt_bark_time == 0 && wdt_ctx->wdt_bite_time == 0) {
        ESP_LOGE(TAG, "bark time/bite time not set %s", __func__);
        return IOT_WATCHDOG_TIME_NOT_SET;
    }
    rtc_wdt_protect_off();
    rtc_wdt_enable();
    rtc_wdt_protect_on();
    wdt_ctx->status = eWatchdogTimerRunning;
    return IOT_WATCHDOG_SUCCESS;
}

int32_t iot_watchdog_stop( IotWatchdogHandle_t const pxWatchdogHandle )
{
    if (pxWatchdogHandle == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_WATCHDOG_INVALID_VALUE;
    }
    wdt_ctx_t *wdt_ctx = (wdt_ctx_t *) pxWatchdogHandle;
    wdt_ctx->wdt_bark_time = 0;
    wdt_ctx->wdt_bite_time = 0;
    wdt_ctx->status = eWatchdogTimerStopped;
    rtc_wdt_disable();
    return IOT_WATCHDOG_SUCCESS;
}

int32_t iot_watchdog_restart( IotWatchdogHandle_t const pxWatchdogHandle )
{
    if (pxWatchdogHandle == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_WATCHDOG_INVALID_VALUE;
    }
    wdt_ctx_t *wdt_ctx = (wdt_ctx_t *) pxWatchdogHandle;
    if (wdt_ctx->wdt_bark_time == 0 || wdt_ctx->wdt_bite_time == 0) {
        ESP_LOGE(TAG, "bark time/bite time not set %s", __func__);
        return IOT_WATCHDOG_TIME_NOT_SET;
    }
    rtc_wdt_feed();
    return IOT_WATCHDOG_SUCCESS;
}

void esp_rtc_wdt_cb(void *arg)
{
    wdt_ctx_t *wdt_ctx = (wdt_ctx_t *) arg;
    if (wdt_ctx->func) {
        wdt_ctx->func(wdt_ctx->arg);
        wdt_ctx->func = NULL;
        wdt_ctx->status = eWatchdogTimerBarkExpired;
        ESP_LOGD(TAG, "Callback invoked %s", __func__);
    }
    REG_CLR_BIT(RTC_CNTL_INT_ENA_REG, RTC_CNTL_WDT_INT_ENA_M);
}

void iot_watchdog_set_callback( IotWatchdogHandle_t const pxWatchdogHandle,
                                IotWatchdogCallback_t xCallback,
                                void * pvUserContext )
{
    if (pxWatchdogHandle == NULL || xCallback == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return;
    }

    wdt_ctx_t *wdt_ctx = (wdt_ctx_t *) pxWatchdogHandle;
    wdt_ctx->func = xCallback;
    wdt_ctx->arg = pvUserContext;

    rtc_isr_register(esp_rtc_wdt_cb, (void *) wdt_ctx, RTC_CNTL_WDT_INT_ENA_M);
    REG_SET_BIT(RTC_CNTL_INT_ENA_REG, RTC_CNTL_WDT_INT_ENA_M);
}

int32_t iot_watchdog_close( IotWatchdogHandle_t const pxWatchdogHandle )
{
    if (pxWatchdogHandle == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_WATCHDOG_INVALID_VALUE;
    }
    wdt_ctx_t *wdt_ctx = (wdt_ctx_t *) pxWatchdogHandle;
    if (0x01 & wdt_status_bitmask >> wdt_ctx->wdt_instance_num) {
        wdt_status_bitmask = wdt_status_bitmask & ~(BIT(wdt_ctx->wdt_instance_num));
        rtc_wdt_disable();
        free(pxWatchdogHandle);
        return IOT_WATCHDOG_SUCCESS;
    }
    return IOT_WATCHDOG_INVALID_VALUE;
}