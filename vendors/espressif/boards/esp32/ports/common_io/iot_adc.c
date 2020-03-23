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
#include <FreeRTOS.h>
#include <freertos/timers.h>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/adc.h"
#include <iot_adc.h>

static const char *TAG = "esp-iot-adc";
#define ADC_CHANNEL 8
#define ADC_WIDTH_9 9
#define ADC_WIDTH_10 10
#define ADC_WIDTH_11 11
#define ADC_WIDTH_12 12
#define ADC_MAX_SAMPLE_TIME 1000
#define ADC_PORT_NUM1 0
#define ADC_PORT_NUM2 1

/*
channel accessed for testing are 3,8,11
*/
static bool is_adc_open = false;

typedef struct {
    IotAdcConfig_t config;
    IotAdcChStatus_t status[ADC_CHANNEL];
    IotAdcChBuffer_t buffer[ADC_CHANNEL];
    IotAdcChain_t chain;
} channel_info;

typedef struct {
    channel_info channel_info_t;
    bool callback_flag;
    void (*func)(uint16_t *arg1, void *arg2);
    uint16_t XConverted_Data[10];
    void *arg2;
    uint8_t ucAdcChannel;
    uint8_t adc_width;
    uint8_t adc_port_num
} adc_cxt_t;

int32_t iot_adc_ioctl( IotAdcHandle_t const pxAdc,
                       IotAdcIoctlRequest_t lRequest,
                       void *const pvBuffer )
{
    if (pxAdc == NULL || pvBuffer == NULL) {
        ESP_LOGE(TAG, "%s Invalid Arguments", __func__);
        return IOT_ADC_INVALID_VALUE;
    }

    esp_err_t ret;
    adc_cxt_t *adc_instance = (adc_cxt_t *)pxAdc;

    switch (lRequest) {
    case eGetAdcConfig: {
        IotAdcConfig_t *getchconfig = (IotAdcConfig_t *) pvBuffer;

        switch (adc_instance->adc_width) {
        case ADC_WIDTH_BIT_9: {
            getchconfig->ucAdcResolution = ADC_WIDTH_9;
            break;
        }
        case ADC_WIDTH_BIT_10: {
            getchconfig->ucAdcResolution = ADC_WIDTH_10;
            break;
        }
        case ADC_WIDTH_BIT_11: {
            getchconfig->ucAdcResolution = ADC_WIDTH_11;
            break;
        }
        case ADC_WIDTH_BIT_12: {
            getchconfig->ucAdcResolution = ADC_WIDTH_12;
            break;
        }
        default:
            ESP_LOGD(TAG, "%s Invalid ADC resolution", __func__);
            return IOT_ADC_INVALID_VALUE;
        }

        getchconfig->ulAdcSampleTime = adc_instance->channel_info_t.config.ulAdcSampleTime;
        return IOT_ADC_SUCCESS;
        break;
    }
    case eSetAdcConfig: {
        IotAdcConfig_t *setchconfig = (IotAdcConfig_t *) pvBuffer;

        if (setchconfig->ulAdcSampleTime > ADC_MAX_SAMPLE_TIME) {
            ESP_LOGD(TAG, "%s Invalid ADC sample time %d", __func__, setchconfig->ulAdcSampleTime);
            return IOT_ADC_INVALID_VALUE;
        }

        switch (setchconfig->ucAdcResolution) {
        ESP_LOGD(TAG, "%s setting ADC resolution to %d", setchconfig->ucAdcResolution);
        case ADC_WIDTH_9: {
            adc_instance->adc_width = ADC_WIDTH_BIT_9;
            break;
        }
        case ADC_WIDTH_10: {
            adc_instance->adc_width = ADC_WIDTH_BIT_10;
            break;
        }
        case ADC_WIDTH_11: {
            adc_instance->adc_width = ADC_WIDTH_BIT_11;
            break;
        }
        case ADC_WIDTH_12: {
            adc_instance->adc_width = ADC_WIDTH_BIT_12;
            break;
        }
        default:
            ESP_LOGD(TAG, "%s Invalid setting for ADC resolution %d", __func__, setchconfig->ucAdcResolution);
            return IOT_ADC_INVALID_VALUE;
        }

        ret = adc_set_data_width(ADC_UNIT_1, adc_instance->adc_width);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s failed to set data width", __func__);
            return IOT_ADC_FAILED;
        }

        adc_instance->channel_info_t.config.ucAdcResolution = setchconfig->ucAdcResolution;
        adc_instance->channel_info_t.config.ulAdcSampleTime = setchconfig->ulAdcSampleTime;
        return IOT_ADC_SUCCESS;
        break;
    }
    case eGetChStatus: {
        IotAdcChStatus_t *channel_status = (IotAdcChStatus_t *) pvBuffer;

        if (adc_instance->adc_port_num == ADC_PORT_NUM1) {
            if (!(channel_status->ucAdcChannel < ADC1_CHANNEL_MAX)) {
                ESP_LOGE(TAG, "%s Invalid ADC1 channel", __func__);
                return IOT_ADC_INVALID_VALUE;
            }
        }
        if (adc_instance->adc_port_num == ADC_PORT_NUM2) {
             if (!(channel_status->ucAdcChannel < ADC2_CHANNEL_MAX)) {
                ESP_LOGE(TAG, "%s Invalid ADC2 channel", __func__);
                return IOT_ADC_INVALID_VALUE;
            }
        }

        channel_status->xAdcChState = adc_instance->channel_info_t.status[channel_status->ucAdcChannel].xAdcChState;
        return IOT_ADC_SUCCESS;
        break;
    }
    case eSetChBuffer: {
        IotAdcChBuffer_t *buffer_config = (IotAdcChBuffer_t *) pvBuffer;

        if (adc_instance->adc_port_num == ADC_PORT_NUM1) {
            if (!(buffer_config->ucAdcChannel < ADC1_CHANNEL_MAX)) {
                ESP_LOGE(TAG, "%s Invalid ADC1 channel", __func__);
                return IOT_ADC_INVALID_VALUE;
            }
        }
        if (adc_instance->adc_port_num == ADC_PORT_NUM2) {
             if (!(buffer_config->ucAdcChannel < ADC2_CHANNEL_MAX)) {
                ESP_LOGE(TAG, "%s Invalid ADC2 channel", __func__);
                return IOT_ADC_INVALID_VALUE;
            }
        }

        if (buffer_config->pvBuffer == NULL) {
            ESP_LOGD(TAG, "%s Invalid Buffer Handler", __func__);
            return IOT_ADC_INVALID_VALUE;
        }

        if (!buffer_config->ucBufLen) {
            ESP_LOGD(TAG, "%s Invalid Buffer Length", __func__);
            return IOT_ADC_INVALID_VALUE;
        }

        adc_instance->channel_info_t.buffer[buffer_config->ucAdcChannel].ucAdcChannel = buffer_config->ucAdcChannel;
        adc_instance->channel_info_t.buffer[buffer_config->ucAdcChannel].pvBuffer = buffer_config->pvBuffer;
        adc_instance->channel_info_t.buffer[buffer_config->ucAdcChannel].ucBufLen = buffer_config->ucBufLen;

        return IOT_ADC_SUCCESS;
        break;
    }
    case eSetAdcChain: {
        IotAdcChain_t *chain_config = (IotAdcChain_t *) pvBuffer;

        if (chain_config->pvBuffer == NULL) {
            ESP_LOGD(TAG, "%s Invalid Buffer Handler", __func__);
            return IOT_ADC_INVALID_VALUE;
        }

        if (adc_instance->adc_port_num == ADC_PORT_NUM1) {
            if (!(chain_config->ucAdcChannel < ADC1_CHANNEL_MAX)) {
                ESP_LOGE(TAG, "%s Invalid ADC channel", __func__);
                return IOT_ADC_INVALID_VALUE;
            }
        }
        if (adc_instance->adc_port_num == ADC_PORT_NUM2) {
             if (!(chain_config->ucAdcChannel < ADC2_CHANNEL_MAX)) {
                ESP_LOGE(TAG, "%s Invalid ADC channel", __func__);
                return IOT_ADC_INVALID_VALUE;
            }
        }

        if (!chain_config->ucBufLen) {
            ESP_LOGD(TAG, "%s Invalid Buffer Length", __func__);
            return IOT_ADC_INVALID_VALUE;
        }

        return IOT_ADC_FUNCTION_NOT_SUPPORTED;
        break;
    }
    default: {
        ESP_LOGD(TAG, "%s Invalid Arguments", __func__);
        return IOT_ADC_INVALID_VALUE;
        break;
    }
    }
}

IotAdcHandle_t iot_adc_open( int32_t lAdc )
{
    if (lAdc < 0 || lAdc > 1) {
        ESP_LOGE(TAG, "%s Invalid Arguments", __func__);
        return NULL;
    }

    if (is_adc_open == true) {
        ESP_LOGD(TAG, "%s ADC already initialized ?", __func__);
        return NULL;
    }

    is_adc_open = true;

    adc_cxt_t *adc_instance = (adc_cxt_t *) calloc(1, sizeof(adc_cxt_t));
    if (adc_instance == NULL) {
        ESP_LOGE(TAG, "%s Could not allocate memory for the adc", __func__);
        return NULL;
    }
    adc_instance->adc_port_num = lAdc;
    return (IotAdcHandle_t)adc_instance;
}

int32_t iot_adc_close( IotAdcHandle_t const pxAdc )
{
    if (pxAdc == NULL) {
        ESP_LOGE(TAG, "%s Invalid Arguments", __func__);
        return IOT_ADC_INVALID_VALUE;
    }
    if (is_adc_open == false) {
        ESP_LOGD(TAG, "%s ADC not open ?", __func__);
        return IOT_ADC_NOT_OPEN;
    }

    is_adc_open = false;
    adc_cxt_t *adc_instance = (adc_cxt_t *)pxAdc;
    adc_instance->func = NULL;
    free(pxAdc);
    return IOT_ADC_SUCCESS;
}

static void adc_buffer_cb(void *arg, unsigned int ulparam)
{
    adc_cxt_t *adc_instance = (adc_cxt_t *)arg;
    adc_instance->channel_info_t.status[adc_instance->ucAdcChannel].xAdcChState = eChStateBusy;
    if (adc_instance->func) {
        ESP_LOGD(TAG,"buff len %d", adc_instance->channel_info_t.buffer[adc_instance->ucAdcChannel].ucBufLen);
        for (int i = 0 ; i < adc_instance->channel_info_t.buffer[adc_instance->ucAdcChannel].ucBufLen; i++) {
            if (adc_instance->adc_port_num == ADC_PORT_NUM1) {
                adc_instance->XConverted_Data[i] = (uint16_t)adc1_get_raw(adc_instance->ucAdcChannel);
            } else  if (adc_instance->adc_port_num == ADC_PORT_NUM2) {
                adc2_get_raw(adc_instance->ucAdcChannel, adc_instance->adc_width, &adc_instance->XConverted_Data[i]);
            }
            adc_instance->channel_info_t.buffer[adc_instance->ucAdcChannel].pvBuffer = &adc_instance->XConverted_Data[i];
            adc_instance->channel_info_t.buffer[adc_instance->ucAdcChannel].pvBuffer++;
         }
        adc_instance->func(adc_instance->XConverted_Data, adc_instance->arg2);
    }
}

static void adc_cb(void *arg, unsigned int ulparam)
{
    adc_cxt_t *adc_instance = (adc_cxt_t *)arg;
    adc_instance->channel_info_t.status[adc_instance->ucAdcChannel].xAdcChState = eChStateBusy;
    if (adc_instance->func) {
    if (adc_instance->adc_port_num == ADC_PORT_NUM1) {
            adc_instance->XConverted_Data[0] = (uint16_t)adc1_get_raw(adc_instance->ucAdcChannel);
        } else  if (adc_instance->adc_port_num == ADC_PORT_NUM2) {
            adc2_get_raw(adc_instance->ucAdcChannel, adc_instance->adc_width, &adc_instance->XConverted_Data[0]);
        }
        adc_instance->func(adc_instance->XConverted_Data, adc_instance->arg2);
        adc_instance->func = NULL;
    }
}

void iot_adc_set_callback( IotAdcHandle_t const pxAdc,
                           uint8_t ucAdcChannel,
                           IotAdcCallback_t xAdcCallback,
                           void *pvUserContext )
{
    if (pxAdc == NULL) {
        ESP_LOGD(TAG, "%s Invalid ADC H", __func__);
        return;
    }

    adc_cxt_t *adc_instance = (adc_cxt_t *)pxAdc;
    if (adc_instance->adc_port_num == ADC_PORT_NUM1) {
        if (!(ucAdcChannel < ADC1_CHANNEL_MAX)) {
            ESP_LOGE(TAG, "%s Invalid ADC channel", __func__);
            return;
        }
    }
    if (adc_instance->adc_port_num == ADC_PORT_NUM2) {
         if (!(ucAdcChannel < ADC2_CHANNEL_MAX)) {
            ESP_LOGE(TAG, "%s Invalid ADC channel", __func__);
            return;
        }
    }
    if (pvUserContext == NULL) {
        ESP_LOGD(TAG, "%s Invalid User context", __func__);
        return;
    }

    adc_instance->func = xAdcCallback;
    adc_instance->arg2 = pvUserContext;
    adc_instance->callback_flag = true;
}

int32_t iot_adc_start( IotAdcHandle_t const pxAdc,
                       uint8_t ucAdcChannel )
{
    if (pxAdc == NULL) {
        ESP_LOGD(TAG, "%s Invalid ADC Handler", __func__);
        return IOT_ADC_INVALID_VALUE;
    }

    adc_cxt_t *adc_instance = (adc_cxt_t *) pxAdc;
    if (adc_instance->adc_port_num == ADC_PORT_NUM1) {
        if (!(ucAdcChannel < ADC1_CHANNEL_MAX)) {
            ESP_LOGE(TAG, "%s Invalid ADC channel", __func__);
            return IOT_ADC_INVALID_VALUE;
        }
    }
    if (adc_instance->adc_port_num == ADC_PORT_NUM2) {
         if (!(ucAdcChannel < ADC2_CHANNEL_MAX)) {
            ESP_LOGE(TAG, "%s Invalid ADC channel", __func__);
            return IOT_ADC_INVALID_VALUE;
        }
    }

   if (!adc_instance->callback_flag) {
        ESP_LOGD(TAG, "%s ADC callback has not set", __func__);
        return IOT_ADC_FAILED;
    } 

    adc_instance->ucAdcChannel = ucAdcChannel;
    adc_instance->channel_info_t.status[ucAdcChannel].ucAdcChannel = ucAdcChannel;

    if (adc_instance->callback_flag && adc_instance->channel_info_t.buffer[ucAdcChannel].pvBuffer != NULL) {
        ESP_LOGD(TAG, "%s In periodic timer callback", __func__);
        if (xTimerPendFunctionCall(adc_buffer_cb, (void *)pxAdc, 0, 0) != pdTRUE) {
            ESP_LOGE(TAG, "%s: failed to create timer for adc start", __func__);
            return IOT_ADC_FAILED;
        }
    }

    if (adc_instance->callback_flag && adc_instance->channel_info_t.buffer[ucAdcChannel].pvBuffer == NULL) {
        ESP_LOGD(TAG, "%s In timer callback", __func__);
        //default 1sec check this case
        if (xTimerPendFunctionCall(adc_cb, (void *)pxAdc, 0, 0) != pdTRUE) {
            ESP_LOGE(TAG, "%s: failed to create timer for adc callback", __func__);
            return IOT_ADC_FAILED;
        }
    }
    adc_instance->channel_info_t.status[ucAdcChannel].xAdcChState = eChStateBusy;
    return IOT_ADC_SUCCESS;
}

int32_t iot_adc_stop( IotAdcHandle_t const pxAdc,
                      uint8_t ucAdcChannel )
{
    if (pxAdc == NULL) {
        ESP_LOGD(TAG, "%s Invalid ADC Handler", __func__);
        return IOT_ADC_INVALID_VALUE;
    }

    adc_cxt_t *adc_instance = (adc_cxt_t *) pxAdc;
    if (adc_instance->adc_port_num == ADC_PORT_NUM1) {
        if (!(ucAdcChannel < ADC1_CHANNEL_MAX)) {
            ESP_LOGE(TAG, "%s Invalid ADC channel", __func__);
            return IOT_ADC_INVALID_VALUE;
        }
    }
    if (adc_instance->adc_port_num == ADC_PORT_NUM2) {
         if (!(ucAdcChannel < ADC2_CHANNEL_MAX)) {
            ESP_LOGE(TAG, "%s Invalid ADC channel", __func__);
            return IOT_ADC_INVALID_VALUE;
        }
    }

    adc_instance->channel_info_t.status[adc_instance->ucAdcChannel].xAdcChState = eChStateIdle;
    return IOT_ADC_SUCCESS;
}


int32_t iot_adc_read_sample(IotAdcHandle_t const pxAdc, uint8_t ucAdcChannel, uint16_t * pusAdcSample)
{
    if (pxAdc == NULL) {
        ESP_LOGD(TAG, "%s Invalid ADC Handler", __func__);
        return IOT_ADC_INVALID_VALUE;
    }

    adc_cxt_t *adc_instance = (adc_cxt_t *) pxAdc;
    if (adc_instance->adc_port_num == ADC_PORT_NUM1) {
        if (!(ucAdcChannel < ADC1_CHANNEL_MAX)) {
            ESP_LOGE(TAG, "%s Invalid ADC channel", __func__);
            return IOT_ADC_INVALID_VALUE;
        }
    }
    if (adc_instance->adc_port_num == ADC_PORT_NUM2) {
         if (!(ucAdcChannel < ADC2_CHANNEL_MAX)) {
            ESP_LOGE(TAG, "%s Invalid ADC channel", __func__);
            return IOT_ADC_INVALID_VALUE;
        }
    }

    if (adc_instance->adc_port_num == ADC_PORT_NUM1) {
        *pusAdcSample = adc1_get_raw(ucAdcChannel);
    } else if (adc_instance->adc_port_num == ADC_PORT_NUM2) {
        adc2_get_raw(ucAdcChannel, adc_instance->adc_width, pusAdcSample);
    }

    ESP_LOGD(TAG, "%s ADC values is: %d", __func__, *pusAdcSample);
    return IOT_ADC_SUCCESS;
}

