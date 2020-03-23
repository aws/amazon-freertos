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
#include "esp_log.h"
#include "iot_gpio.h"
#include "driver/gpio.h"

#define ESP_INTR_FLAG_DEFAULT 0

static const char *TAG = "esp-hal-gpio";

typedef struct {
    void (*func)(uint8_t arg1, void *arg2);
    void *arg2;
} gpio_cb_func_t;

typedef struct {
    int32_t gpio_num;
    gpio_config_t iot_gpio_conf;
    gpio_cb_func_t gpio_cb_func[GPIO_NUM_MAX];
    uint8_t ucPinState;
    uint32_t set_function_val;
} gpio_ctx_t;

static volatile uint64_t gpio_status_bitmask; //BIT Mask for GPIO
static volatile uint64_t gpio_int_status_bitmask; //BIT Mask for GPIO

IotGpioHandle_t iot_gpio_open( int32_t lGpioNumber )
{
    if (lGpioNumber < 0 || lGpioNumber > 40) {
        ESP_LOGE(TAG, "Invalid  arguments %s", __func__);
        return NULL;
    }
    if (!(0x01 & gpio_status_bitmask >> lGpioNumber)) {
        gpio_ctx_t *gpio_ctx = (gpio_ctx_t *) calloc(1, sizeof(gpio_ctx_t));
        if (gpio_ctx == NULL) {
            ESP_LOGE(TAG, "Could not allocate memory for gpio context");
            return NULL;
        }
        gpio_ctx->gpio_num = lGpioNumber;
        gpio_ctx->iot_gpio_conf.pin_bit_mask = BIT64(gpio_ctx->gpio_num);
        gpio_config(&gpio_ctx->iot_gpio_conf);
        gpio_status_bitmask = gpio_status_bitmask | BIT64(gpio_ctx->gpio_num);
        IotGpioHandle_t iot_gpio_handler = (void *)gpio_ctx;
        ESP_LOGD(TAG, "%s GPIO Number %d, GPIO bitmask %d", __func__, lGpioNumber, gpio_status_bitmask);
        return iot_gpio_handler;
    }
    return NULL;
}

static void iot_hal_gpio_set_output_mode(IotGpioOutputMode_t *gpio_set_output_mode, gpio_config_t *gpio_conf)
{
    switch (*gpio_set_output_mode) {
    ESP_LOGD(TAG, "gpio output mode: %d", *gpio_set_output_mode);
    case eGpioOpenDrain :
        gpio_conf->mode = GPIO_MODE_OUTPUT_OD;
        break;
    case eGpioPushPull :
        gpio_conf->mode = GPIO_MODE_DEF_OUTPUT;
        break;
    default :
        ESP_LOGD(TAG, "gpio output mode not supported");
        break;
    }
}

static void iot_hal_gpio_get_output_mode(IotGpioOutputMode_t *gpio_get_output_mode, gpio_config_t *gpio_conf)
{
    switch (gpio_conf->mode) {
    case GPIO_MODE_OUTPUT_OD :
        *gpio_get_output_mode = eGpioOpenDrain;
        break;
    case GPIO_MODE_DEF_OUTPUT :
        *gpio_get_output_mode = eGpioPushPull;
        break;
    default :
        ESP_LOGD(TAG, "gpio output mode not supported");
        break;
    }
}

static void iot_hal_gpio_set_pullup_pulldown(IotGpioPull_t *gpio_set_pullup_pulldown, gpio_config_t *gpio_conf)
{
    switch (*gpio_set_pullup_pulldown) {
    ESP_LOGD(TAG, "gpio pullup/pulldown mode: %d", *gpio_set_pullup_pulldown);
    case eGpioPullNone :
        gpio_conf->pull_down_en = GPIO_PULLDOWN_DISABLE;
        gpio_conf->pull_up_en = GPIO_PULLUP_DISABLE;
        break;
    case eGpioPullUp :
        gpio_conf->pull_down_en = GPIO_PULLDOWN_DISABLE;
        gpio_conf->pull_up_en = GPIO_PULLUP_ENABLE;
        break;
    case eGpioPullDown :
        gpio_conf->pull_down_en = GPIO_PULLDOWN_ENABLE;
        gpio_conf->pull_up_en =  GPIO_PULLUP_DISABLE;
        break;
    default :
        ESP_LOGD(TAG, "gpio set pullup/pulldown not supported");
        break;
    }
}

static void iot_hal_gpio_get_pullup_pulldown(IotGpioPull_t *gpio_get_pullup_pulldown, gpio_config_t *gpio_conf)
{
    if (gpio_conf->pull_down_en == GPIO_PULLDOWN_ENABLE) {
        *gpio_get_pullup_pulldown = eGpioPullDown;
    } else if (gpio_conf->pull_up_en == GPIO_PULLUP_ENABLE) {
        *gpio_get_pullup_pulldown = eGpioPullUp;
    } else {
        *gpio_get_pullup_pulldown = eGpioPullNone;
    }
}

static void iot_hal_gpio_set_interrupt(IotGpioInterrupt_t *gpio_set_interrupt, gpio_config_t *gpio_conf)
{
    switch (*gpio_set_interrupt) {
    ESP_LOGD(TAG, "gpio interrupt mode: %d", *gpio_set_interrupt);
    case eGpioInterruptNone :
        gpio_conf->intr_type = GPIO_PIN_INTR_DISABLE;
        break;
    case eGpioInterruptRising :
        gpio_conf->intr_type = GPIO_INTR_POSEDGE;
        break;
    case eGpioInterruptFalling :
        gpio_conf->intr_type = GPIO_INTR_NEGEDGE;
        break;
    case eGpioInterruptEdge :
        gpio_conf->intr_type = GPIO_INTR_ANYEDGE;
        break;
    case eGpioInterruptLow :
        gpio_conf->intr_type = GPIO_INTR_LOW_LEVEL;
        break;
    case eGpioInterruptHigh :
        gpio_conf->intr_type = GPIO_INTR_HIGH_LEVEL;
        break;
    default :
        ESP_LOGD(TAG, "gpio set interrupt not supported");
        break;
    }
}

static void iot_hal_gpio_get_interrupt(IotGpioInterrupt_t *gpio_get_interrupt, gpio_config_t *gpio_conf)
{
    switch (gpio_conf->intr_type) {
    case GPIO_PIN_INTR_DISABLE :
        *gpio_get_interrupt = GPIO_PIN_INTR_DISABLE;
        break;
    case GPIO_INTR_POSEDGE :
        *gpio_get_interrupt = GPIO_INTR_POSEDGE;
        break;
    case GPIO_INTR_NEGEDGE :
        *gpio_get_interrupt = GPIO_INTR_NEGEDGE;
        break;
    case GPIO_INTR_ANYEDGE :
        *gpio_get_interrupt = GPIO_INTR_ANYEDGE;
        break;
    case GPIO_INTR_LOW_LEVEL :
        *gpio_get_interrupt = GPIO_INTR_LOW_LEVEL;
        break;
    case GPIO_INTR_HIGH_LEVEL :
        *gpio_get_interrupt = GPIO_INTR_HIGH_LEVEL;
        break;
    default :
        ESP_LOGD(TAG, "gpio set interrupt not supported");
        break;
    }
}

static void iot_hal_gpio_set_direction(IotGpioDirection_t *gpio_set_direction, gpio_config_t *gpio_conf)
{
    switch (*gpio_set_direction) {
    ESP_LOGD(TAG, "gpio direction: %d", *gpio_set_direction);
    case eGpioDirectionInput:
        gpio_conf->mode = GPIO_MODE_DEF_INPUT;
        break;
    case eGpioDirectionOutput:
        gpio_conf->mode = GPIO_MODE_OUTPUT;
        break;
    default :
        ESP_LOGD(TAG, "gpio set direction not supported");
        break;
    }
}

static void iot_hal_gpio_get_direction(IotGpioDirection_t *gpio_get_direction, gpio_config_t *gpio_conf)
{
    switch (gpio_conf->mode) {
    case GPIO_MODE_DEF_INPUT:
        *gpio_get_direction = eGpioDirectionInput;
        break;
    case GPIO_MODE_OUTPUT:
        *gpio_get_direction = eGpioDirectionOutput;
        break;
    default :
        ESP_LOGD(TAG, "gpio get direction not supported");
        break;
    }
}


int32_t iot_gpio_ioctl( IotGpioHandle_t const pxGpio,
                        IotGpioIoctlRequest_t xRequest,
                        void *const pvBuffer )
{
    esp_err_t ret;
    if (pxGpio == NULL || pvBuffer == NULL) {
        ESP_LOGD(TAG, "Invalid  arguments %s", __func__);
        return IOT_GPIO_INVALID_VALUE;
    }
    gpio_ctx_t *gpio_ctx = (gpio_ctx_t *) pxGpio;
    //gpio_config_t io_conf = {0};
    switch (xRequest) {
    case eSetGpioFunction: {
        gpio_ctx->set_function_val = *(uint8_t *)pvBuffer;
        //FIXME what is this ?
        return IOT_GPIO_SUCCESS;
        //return IOT_GPIO_FUNCTION_NOT_SUPPORTED;
        break;
    }
    case eSetGpioDirection: {
        IotGpioDirection_t *set_direction = (IotGpioDirection_t *)pvBuffer;
        iot_hal_gpio_set_direction(set_direction, &gpio_ctx->iot_gpio_conf);
        gpio_set_direction(gpio_ctx->gpio_num, gpio_ctx->iot_gpio_conf.mode);
        return IOT_GPIO_SUCCESS;
        break;
    }
    case eSetGpioPull: {
        IotGpioPull_t *gpio_set_push_pull = (IotGpioPull_t *)pvBuffer;
        iot_hal_gpio_set_pullup_pulldown(gpio_set_push_pull, &gpio_ctx->iot_gpio_conf);
        if (gpio_ctx->iot_gpio_conf.pull_up_en) {
            gpio_pullup_en(gpio_ctx->gpio_num);
        } else {
            gpio_pullup_dis(gpio_ctx->gpio_num);
        }
        if (gpio_ctx->iot_gpio_conf.pull_down_en) {
            gpio_pulldown_en(gpio_ctx->gpio_num);
        } else {
            gpio_pulldown_dis(gpio_ctx->gpio_num);
        }
        return IOT_GPIO_SUCCESS;
        break;
    }
    case eSetGpioOutputMode: {
        IotGpioOutputMode_t *gpio_set_op_mode = (IotGpioOutputMode_t *)pvBuffer;
        iot_hal_gpio_set_output_mode(gpio_set_op_mode, &gpio_ctx->iot_gpio_conf);
        return IOT_GPIO_SUCCESS;
        break;
    }
    case eSetGpioInterrupt: {
        IotGpioInterrupt_t *gpio_set_interrupt = (IotGpioInterrupt_t *)pvBuffer;
        iot_hal_gpio_set_interrupt(gpio_set_interrupt, &gpio_ctx->iot_gpio_conf);
        gpio_set_intr_type(gpio_ctx->gpio_num, gpio_ctx->iot_gpio_conf.intr_type);
        return IOT_GPIO_SUCCESS;
        break;
    }
    case eSetGpioSpeed: {
        //need to check the existing APIS
        return IOT_GPIO_FUNCTION_NOT_SUPPORTED;
        break;
    }
    case eSetGpioDriveStrength: {
        gpio_drive_cap_t *gpio_drive_strength = (gpio_drive_cap_t *)pvBuffer;
        ret = gpio_set_drive_capability(gpio_ctx->gpio_num, *gpio_drive_strength);
        if (ret != ESP_OK) {
            ESP_LOGD(TAG, "Failed in setting drive capability %d", ret);
            return IOT_GPIO_WRITE_FAILED;
        }
        return IOT_GPIO_SUCCESS;
        break;
    }
    case eGetGpioFunction: {
        *(uint32_t *)pvBuffer = gpio_ctx->set_function_val;
        //FIXME What is this ?
        //return IOT_GPIO_FUNCTION_NOT_SUPPORTED;
        return IOT_GPIO_SUCCESS;
        break;
    }
    case eGetGpioDirection: {
        IotGpioDirection_t *get_direction = (IotGpioDirection_t *)pvBuffer;
        iot_hal_gpio_get_direction(get_direction, &gpio_ctx->iot_gpio_conf);
        return IOT_GPIO_SUCCESS;
        break;
    }
    case eGetGpioPull: {
        IotGpioPull_t *gpio_get_push_pull = (IotGpioPull_t *)pvBuffer;
        iot_hal_gpio_get_pullup_pulldown(gpio_get_push_pull, &gpio_ctx->iot_gpio_conf);
        return IOT_GPIO_SUCCESS;
        break;
    }
    case eGetGpioOutputType: {
        IotGpioOutputMode_t *gpio_get_op_mode = (IotGpioOutputMode_t *)pvBuffer;
        iot_hal_gpio_get_output_mode(gpio_get_op_mode, &gpio_ctx->iot_gpio_conf);
        return IOT_GPIO_SUCCESS;
        break;
    }
    case eGetGpioInterrupt: {
        IotGpioInterrupt_t *gpio_get_interrupt = (IotGpioInterrupt_t *)pvBuffer;
        iot_hal_gpio_get_interrupt(gpio_get_interrupt, &gpio_ctx->iot_gpio_conf);
        return IOT_GPIO_SUCCESS;
        break;
    }
    case eGetGpioSpeed: {

        return IOT_GPIO_FUNCTION_NOT_SUPPORTED;
        break;
    }
    case eGetGpioDriveStrength: {
        gpio_drive_cap_t *gpio_drive_strength = (gpio_drive_cap_t *)pvBuffer;
        ret = gpio_get_drive_capability(gpio_ctx->gpio_num, gpio_drive_strength);
        if (ret != ESP_OK) {
            ESP_LOGD(TAG, "Failed in getting drive capability %d", ret);
            return IOT_GPIO_READ_FAILED;
        }
        return IOT_GPIO_SUCCESS;
        break;
    }
    default: {
        return IOT_GPIO_INVALID_VALUE;
        break;
    }
    }
}

IRAM_ATTR static void gpio_cb (void *arg)
{
    gpio_ctx_t *gpio_ctx = (gpio_ctx_t *) arg;
    if (gpio_ctx->gpio_cb_func[gpio_ctx->gpio_num].func) {
        gpio_ctx->gpio_cb_func[gpio_ctx->gpio_num].func(gpio_ctx->ucPinState, gpio_ctx->gpio_cb_func[gpio_ctx->gpio_num].arg2);
    }
}

void iot_gpio_set_callback( IotGpioHandle_t const pxGpio,
                            IotGpioCallback_t xGpioCallback,
                            void *pvUserContext )
{
    esp_err_t ret = ESP_OK;
    if (pxGpio == NULL || xGpioCallback == NULL) {
        ESP_LOGD(TAG, "Invalid  arguments %s", __func__);
        return;
    }
    gpio_ctx_t *gpio_ctx = (gpio_ctx_t *) pxGpio;
    ret = gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    if (ret == ESP_ERR_NO_MEM) {
        ESP_LOGE(TAG, "Failed to install ISR service");
        return;
    }
    gpio_ctx->gpio_cb_func[gpio_ctx->gpio_num].func = xGpioCallback;
    gpio_ctx->gpio_cb_func[gpio_ctx->gpio_num].arg2 = pvUserContext;
    ret = gpio_isr_handler_add(gpio_ctx->gpio_num, gpio_cb, (void *)gpio_ctx);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add isr handler %d", ret);
        return;
    }
    gpio_int_status_bitmask = gpio_int_status_bitmask | BIT64(gpio_ctx->gpio_num);
    ESP_LOGD(TAG, "%s GPIO interrupt bitmask %d", __func__, gpio_int_status_bitmask);
}

int32_t iot_gpio_read_sync( IotGpioHandle_t const pxGpio,
                            uint8_t *pucPinState )
{
    if (pxGpio == NULL) {
        ESP_LOGD(TAG, "Invalid  arguments %s", __func__);
        return IOT_GPIO_INVALID_VALUE;
    }
    gpio_ctx_t *gpio_ctx = (gpio_ctx_t *) pxGpio;
    *pucPinState = gpio_get_level(gpio_ctx->gpio_num);

    return IOT_GPIO_SUCCESS;
}

int32_t iot_gpio_write_sync( IotGpioHandle_t const pxGpio,
                             uint8_t pucPinState )
{
    esp_err_t ret = ESP_OK;
    if (pxGpio == NULL) {
        ESP_LOGD(TAG, "Invalid  arguments %s", __func__);
        return IOT_GPIO_INVALID_VALUE;
    }
    gpio_ctx_t *gpio_ctx = (gpio_ctx_t *) pxGpio;
    ret = gpio_set_level(gpio_ctx->gpio_num, pucPinState);
    return (ret == ESP_OK) ? IOT_GPIO_SUCCESS : IOT_GPIO_WRITE_FAILED;
}

int32_t iot_gpio_close( IotGpioHandle_t const pxGpio )
{
    if (pxGpio == NULL) {
        ESP_LOGD(TAG, "Invalid  arguments %s", __func__);
        return IOT_GPIO_INVALID_VALUE;
    }
    gpio_ctx_t *gpio_ctx = (gpio_ctx_t *) pxGpio;
    if (gpio_ctx->gpio_num >= 0 && GPIO_IS_VALID_GPIO(gpio_ctx->gpio_num) && (0x01 & gpio_status_bitmask >> gpio_ctx->gpio_num)) {
        ESP_LOGD(TAG, "%s GPIO Number %d, GPIO bitmask %d, GPIO interrupt bitmask %d",
            __func__, gpio_ctx->gpio_num, gpio_status_bitmask, gpio_int_status_bitmask);
        if (0x01 & gpio_int_status_bitmask >> gpio_ctx->gpio_num) {
            gpio_isr_handler_remove(gpio_ctx->gpio_num);
            gpio_int_status_bitmask = gpio_int_status_bitmask & ~(BIT64(gpio_ctx->gpio_num));
        }
        gpio_status_bitmask = gpio_status_bitmask & ~(BIT64(gpio_ctx->gpio_num));
        free(pxGpio);
        return IOT_GPIO_SUCCESS;
    } else {
        return IOT_GPIO_INVALID_VALUE;
    }
}
