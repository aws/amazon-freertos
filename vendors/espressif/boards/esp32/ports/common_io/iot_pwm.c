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
#include "iot_pwm.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "board_gpio.h"

#define IOT_PWM_DEFAULT_FREQ (5000)
#define IOT_PWM_DC_RESOLUTION (LEDC_TIMER_10_BIT)

static const char *TAG = "esp-hal-pwm";

typedef struct {
    IotPwmConfig_t config;
    bool is_pwm_configured;
    uint8_t iot_pwm_instance;
} pwm_ctx_t;

static struct esp_pwm_pin_config esp_pwm_pin_map[ESP_MAX_PWM_CHANNELS] = ESP_PWM_PIN_MAP;
static volatile bool is_pwm_init_done = false;

IotPwmHandle_t iot_pwm_open( int32_t lPwmInstance )
{
    if (is_pwm_init_done) {
        ESP_LOGD(TAG, "pwm instance already initialized");
        return NULL;
    }
    if (lPwmInstance != 0 && lPwmInstance != 1) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return NULL;
    }

    pwm_ctx_t *pwm_ctx = (pwm_ctx_t *) calloc(1, sizeof(pwm_ctx_t));
    if (pwm_ctx == NULL) {
        ESP_LOGE(TAG, "Could not allocate memory for pwm context");
        return NULL;
    }
    pwm_ctx->iot_pwm_instance = lPwmInstance;
    ledc_timer_config_t ledc_timer = {0};
    ledc_timer.duty_resolution = IOT_PWM_DC_RESOLUTION;
    ledc_timer.freq_hz = IOT_PWM_DEFAULT_FREQ;
    if (lPwmInstance == 0) {
        ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
        ledc_timer.timer_num = LEDC_TIMER_0;
    } else {
        ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
        ledc_timer.timer_num = LEDC_TIMER_1;
    }
    ledc_timer_config(&ledc_timer);

    IotPwmHandle_t iot_pwn_handler = (void *)pwm_ctx;
    is_pwm_init_done = true;
    return iot_pwn_handler;
}

int32_t iot_pwm_set_config( IotPwmHandle_t const pxPwmHandle,
                            const IotPwmConfig_t xConfig )
{
    esp_err_t ret;
    if (pxPwmHandle == NULL || xConfig.ucPwmChannel > ESP_MAX_PWM_CHANNELS) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_PWM_INVALID_VALUE;
    }
    pwm_ctx_t *pwm_ctx = (pwm_ctx_t *) pxPwmHandle;
    ledc_channel_config_t ledc_channel;

    if (pwm_ctx->iot_pwm_instance == 0) {
        ret = ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, xConfig.ulPwmFrequency);
        ledc_channel.channel    = xConfig.ucPwmChannel;
        ledc_channel.duty       = ((xConfig.ucPwmDutyCycle * 2 * IOT_PWM_DC_RESOLUTION) / 100);
        ledc_channel.gpio_num   = esp_pwm_pin_map[xConfig.ucPwmChannel].pwm_num;
        ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
        ledc_channel.hpoint     = 0;
        ledc_channel.timer_sel  = LEDC_TIMER_0;
        ret |= ledc_channel_config(&ledc_channel);
        ret |= ledc_timer_pause(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
    } else if (pwm_ctx->iot_pwm_instance == 1) {
        ret = ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_1, xConfig.ulPwmFrequency);
        ledc_channel.channel    = xConfig.ucPwmChannel;
        ledc_channel.duty       = ((xConfig.ucPwmDutyCycle * 2 * IOT_PWM_DC_RESOLUTION) / 100);
        ledc_channel.gpio_num   = esp_pwm_pin_map[xConfig.ucPwmChannel].pwm_num;
        ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
        ledc_channel.hpoint     = 0;
        ledc_channel.timer_sel  = LEDC_TIMER_1;
        ret |= ledc_channel_config(&ledc_channel);
        ret |= ledc_timer_pause(LEDC_LOW_SPEED_MODE, LEDC_TIMER_1);
    } else {
        ESP_LOGE(TAG, "Invalid pwm instance %d %s", pwm_ctx->iot_pwm_instance, __func__);
        return IOT_PWM_INVALID_VALUE;
    }
    pwm_ctx->config = xConfig;
    pwm_ctx->is_pwm_configured = true;
    return (ret == ESP_OK) ? IOT_PWM_SUCCESS : IOT_PWM_INVALID_VALUE;
}

IotPwmConfig_t *iot_pwm_get_config( IotPwmHandle_t const pxPwmHandle )
{
    if (pxPwmHandle == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return NULL;
    }

    pwm_ctx_t *pwm_ctx = (pwm_ctx_t *) pxPwmHandle;
    return (IotPwmConfig_t *)&pwm_ctx->config;
}

int32_t iot_pwm_start( IotPwmHandle_t const pxPwmHandle )
{
    esp_err_t ret;
    if (pxPwmHandle == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_PWM_INVALID_VALUE;
    }
    pwm_ctx_t *pwm_ctx = (pwm_ctx_t *) pxPwmHandle;
    if (!pwm_ctx->is_pwm_configured) {
        return IOT_PWM_NOT_CONFIGURED;
    }
    if (pwm_ctx->iot_pwm_instance == 0) {
        ret = ledc_timer_resume(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
    } else if (pwm_ctx->iot_pwm_instance == 1) {
        ret = ledc_timer_resume(LEDC_LOW_SPEED_MODE, LEDC_TIMER_1);
    } else {
        ESP_LOGE(TAG, "Invalid pwm instance %d %s", pwm_ctx->iot_pwm_instance, __func__);
        return IOT_PWM_INVALID_VALUE;
    }
    return (ret == ESP_OK) ? IOT_PWM_SUCCESS : IOT_PWM_INVALID_VALUE;
}

int32_t iot_pwm_stop( IotPwmHandle_t const pxPwmHandle )
{
    esp_err_t ret;
    if (pxPwmHandle == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_PWM_INVALID_VALUE;
    }
    pwm_ctx_t *pwm_ctx = (pwm_ctx_t *) pxPwmHandle;
    if (!pwm_ctx->is_pwm_configured) {
        return IOT_PWM_NOT_CONFIGURED;
    }
    if (pwm_ctx->iot_pwm_instance == 0) {
        ret = ledc_timer_pause(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
    } else if (pwm_ctx->iot_pwm_instance == 1) {
        ret = ledc_timer_pause(LEDC_LOW_SPEED_MODE, LEDC_TIMER_1);
    } else {
        ESP_LOGE(TAG, "Invalid pwm instance %d %s", pwm_ctx->iot_pwm_instance, __func__);
        return IOT_PWM_INVALID_VALUE;
    }
    return (ret == ESP_OK) ? IOT_PWM_SUCCESS : IOT_PWM_INVALID_VALUE;
}

int32_t iot_pwm_close( IotPwmHandle_t const pxPwmHandle )
{
    esp_err_t ret;
    if (pxPwmHandle == NULL) {
        ESP_LOGE(TAG, "Invalid arguments %s", __func__);
        return IOT_PWM_INVALID_VALUE;
    }
    if (!is_pwm_init_done) {
        ESP_LOGE(TAG, "pwm instance already closed");
        return IOT_PWM_INVALID_VALUE;
    }
    pwm_ctx_t *pwm_ctx = (pwm_ctx_t *) pxPwmHandle;
    if (pwm_ctx->iot_pwm_instance == 0) {
        ret = ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, 0);
    } else if (pwm_ctx->iot_pwm_instance == 1) {
        ret = ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_TIMER_1, 0);
    } else {
        ESP_LOGE(TAG, "Invalid pwm instance %d %s", pwm_ctx->iot_pwm_instance, __func__);
        return IOT_PWM_INVALID_VALUE;
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to close iot_pwm %d", ret);
    }
    free(pxPwmHandle);
    is_pwm_init_done = false;
    return IOT_PWM_SUCCESS;
}
