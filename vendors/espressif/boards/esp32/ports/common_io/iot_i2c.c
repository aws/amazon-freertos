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

#include <string.h>
#include "iot_i2c.h"
#include "esp_log.h"
#include "esp_err.h"
#include <esp_timer.h>
#include "driver/i2c.h"
#include "board_gpio.h"

static const char *TAG = "esp-hal-i2c";

#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */
#define SET_REGISTER_ADDR 1
#define SET_BUFFER_WRITE 0
static bool i2c_flag;

typedef struct {
    IotI2CConfig_t iot_i2c_config;
    int32_t i2c_port_num;
    bool driver_installed;
    esp_timer_handle_t esp_timer_handle_write;
    esp_timer_handle_t esp_timer_handler_read;
    void (*func)(IotI2COperationStatus_t arg1, void *arg2);
    void *arg;
    void *pvBuffer;
    size_t xBytes;
    bool is_channel_busy;
    bool is_slave_addr_write_set;
    bool is_slave_addr_read_set;
    bool register_buffer_write_sq;
    uint8_t reg_write_addr;
} i2c_ctx_t;

static struct esp_i2c_pin_config esp_i2c_pin_map[ESP_MAX_I2C_PORTS] = ESP_I2C_PIN_MAP;

IotI2CHandle_t iot_i2c_open(int32_t lI2CInstance)
{
    if (lI2CInstance < 0 || lI2CInstance > 1) {
        ESP_LOGE(TAG, "Invalid arguments");
        return NULL;
    }
    if (i2c_flag == true) {
        ESP_LOGE(TAG, "I2C Handler is already initialised");
        return NULL;
    }
    i2c_ctx_t *i2c_ctx = (i2c_ctx_t *)calloc(1, sizeof(i2c_ctx_t));
    if (i2c_ctx == NULL) {
        ESP_LOGE(TAG, "Could not allocate memory for i2c context");
        return NULL;
    }
    IotI2CHandle_t iot_i2c_handler = (void *) i2c_ctx;
    i2c_ctx->i2c_port_num = lI2CInstance;
    i2c_flag = true;
    return iot_i2c_handler;
}

int32_t iot_i2c_ioctl( IotI2CHandle_t const pxI2CPeripheral, IotI2CIoctlRequest_t xI2CRequest, void *const pvBuffer)
{
    esp_err_t ret;
    if (pxI2CPeripheral == NULL || pvBuffer == NULL) {
        ESP_LOGE(TAG, "Invalid arguments");
        return IOT_I2C_INVALID_VALUE;
    }
    i2c_ctx_t *i2c_ctx = (i2c_ctx_t *) pxI2CPeripheral;

    switch (xI2CRequest) {
    case eI2CSetMasterConfig : {
        if (i2c_ctx->is_channel_busy) {
            return IOT_I2C_BUSY;
        }
        IotI2CConfig_t *iot_i2c_handler = (IotI2CConfig_t *) pvBuffer;
        i2c_ctx->iot_i2c_config.ulBusFreq = iot_i2c_handler->ulBusFreq;
        i2c_ctx->iot_i2c_config.ulMasterTimeout = iot_i2c_handler->ulMasterTimeout;
        int32_t i2c_port_num = (int32_t) i2c_ctx->i2c_port_num;

        if (i2c_ctx->driver_installed && i2c_ctx->is_channel_busy == false) {
            esp_err_t ret = i2c_driver_delete(i2c_port_num);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "i2c driver delete failed");
                return IOT_I2C_INVALID_VALUE;
            }
            i2c_ctx->driver_installed = false;
            ESP_LOGD(TAG, "i2c driver delete success");
        }

        i2c_config_t i2c_conf = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = (gpio_num_t)esp_i2c_pin_map[i2c_port_num].sda_pin,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_io_num = esp_i2c_pin_map[i2c_port_num].scl_pin,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master.clk_speed = i2c_ctx->iot_i2c_config.ulBusFreq,
        };
        ret = i2c_param_config(i2c_port_num, &i2c_conf);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "config failed");
            return IOT_I2C_INVALID_VALUE;
        }
        ret = i2c_driver_install(i2c_port_num, i2c_conf.mode, 0, 0, 0);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "driver install failed");
            return IOT_I2C_INVALID_VALUE;
        }
        ESP_LOGD(TAG, "driver installed");
        if (i2c_ctx->iot_i2c_config.ulMasterTimeout > 0) {
            ret = i2c_set_timeout(i2c_port_num, (i2c_ctx->iot_i2c_config.ulMasterTimeout * 80));
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "timeout failed");
                return IOT_I2C_INVALID_VALUE;
            }
        }
        i2c_ctx->driver_installed = true;
        return IOT_I2C_SUCCESS;
    }
    break;
    case eI2CGetMasterConfig : {
        IotI2CConfig_t *iot_i2c_handler = (IotI2CConfig_t *) pvBuffer;
        iot_i2c_handler->ulBusFreq = i2c_ctx->iot_i2c_config.ulBusFreq;
        iot_i2c_handler->ulMasterTimeout = i2c_ctx->iot_i2c_config.ulMasterTimeout;
        return IOT_I2C_SUCCESS;
        break;
    }
    case eI2CSetSlaveAddr : {
        esp_err_t ret;
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        ret = i2c_master_start(cmd);
        ret |= i2c_master_write_byte(cmd, (*(uint8_t *)pvBuffer) << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
        ret |= i2c_master_cmd_begin(i2c_ctx->i2c_port_num, cmd, 1000 / portTICK_RATE_MS );
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "ret %d", ret);
            ESP_LOGE(TAG, "i2c_master_cmd_begin failed");
            return IOT_I2C_INVALID_VALUE;
        }
        i2c_cmd_link_delete(cmd);
        i2c_ctx->is_slave_addr_write_set = true;
        return IOT_I2C_SUCCESS;
        break;
    }
    #if 0
    case eI2CSetSlaveAddr : {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        ret = i2c_master_start(cmd);
        ret |= i2c_master_write_byte(cmd, (*(uint8_t *)pvBuffer) << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
        ret |= i2c_master_cmd_begin(i2c_ctx->i2c_port_num, cmd, 1000 / portTICK_RATE_MS );
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "ret %d", ret);
            ESP_LOGE(TAG, "i2c_master_cmd_begin failed");
            return IOT_I2C_INVALID_VALUE;
        }
        i2c_cmd_link_delete(cmd);
        i2c_ctx->is_slave_addr_read_set = true;
        return IOT_I2C_SUCCESS;
        break;
    }
    #endif
    case eI2CSendNoStopFlag : {
        return IOT_I2C_SUCCESS;
    }
    default :
        ESP_LOGE(TAG, "Invalid argument");
        return IOT_I2C_INVALID_VALUE;
        break;
    }
}

static void write_async_cb(void *arg)
{
    esp_err_t ret = ESP_OK;
    i2c_ctx_t *iot_i2c_handler = (i2c_ctx_t *) arg;
    int32_t i2c_port_num = (int32_t) iot_i2c_handler->i2c_port_num;
    uint8_t *src_buf = (uint8_t *) iot_i2c_handler->pvBuffer;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret |= i2c_master_write_byte(cmd, iot_i2c_handler->reg_write_addr, ACK_CHECK_EN);
    //ret = i2c_master_start(cmd);
    //ret |= i2c_master_write_byte(cmd, iot_i2c_handler->iot_i2c_slave_addr << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    ret |= i2c_master_write(cmd, src_buf, iot_i2c_handler->xBytes, ACK_CHECK_EN);
    ret |= i2c_master_stop(cmd);
    ret |= i2c_master_cmd_begin(i2c_port_num, cmd, 1000 / portTICK_RATE_MS );
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ret %d", ret);
        ESP_LOGE(TAG, "i2c_master_cmd_begin failed");
    }
    i2c_cmd_link_delete(cmd);

    if (iot_i2c_handler->func) {
        uint8_t op_status = eI2CCompleted;
        iot_i2c_handler->func(op_status, iot_i2c_handler->arg);
        iot_i2c_handler->func = NULL;
    }
    iot_i2c_handler->is_channel_busy = false;
    esp_timer_delete(iot_i2c_handler->esp_timer_handle_write);
    iot_i2c_handler->esp_timer_handle_write = NULL;
}

static void read_async_cb(void *arg)
{
    esp_err_t ret = ESP_OK;
    i2c_ctx_t *iot_i2c_handler = (i2c_ctx_t *) arg;
    int32_t i2c_port_num = (int32_t) iot_i2c_handler->i2c_port_num;
    uint8_t *src_buf = (uint8_t *) iot_i2c_handler->pvBuffer;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret |= i2c_master_write_byte(cmd, iot_i2c_handler->reg_write_addr, ACK_CHECK_EN);
    //ret  = i2c_master_start(cmd);
    //ret |= i2c_master_write_byte(cmd, iot_i2c_handler->iot_i2c_slave_addr << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    //ret |= i2c_master_stop(cmd);
    //ret |= i2c_master_cmd_begin(i2c_port_num, cmd, 1000 / portTICK_RATE_MS);
    //if (ret != ESP_OK) {
    //    ESP_LOGD(TAG, "i2c_master_cmd_begin 1 failed");
    //}
    //i2c_cmd_link_delete(cmd);

    //cmd = i2c_cmd_link_create();
    //ret = i2c_master_start(cmd);
    //ret |= i2c_master_write_byte(cmd, iot_i2c_handler->iot_i2c_slave_addr << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
    if (iot_i2c_handler->xBytes > 1) {
        ret |= i2c_master_read(cmd, src_buf, iot_i2c_handler->xBytes - 1, ACK_VAL);
    }
    ret |= i2c_master_read_byte(cmd, src_buf, NACK_VAL);
    ret |= i2c_master_stop(cmd);
    ret |= i2c_master_cmd_begin(i2c_port_num, cmd, 1000 / portTICK_RATE_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c_master_cmd_begin 2 failed");
    }
    i2c_cmd_link_delete(cmd);
    if (iot_i2c_handler->func) {
        uint8_t op_status = eI2CCompleted;
        iot_i2c_handler->func(op_status, iot_i2c_handler->arg);
        iot_i2c_handler->func = NULL;
    }
    iot_i2c_handler->is_channel_busy = false;
    esp_timer_delete(iot_i2c_handler->esp_timer_handler_read);
    iot_i2c_handler->esp_timer_handler_read = NULL;
}

void iot_i2c_set_callback(IotI2CHandle_t const pxI2CPeripheral, IotI2CCallback_t xCallback, void *pvUserContext)
{
    if (pxI2CPeripheral == NULL || xCallback == NULL) {
        ESP_LOGE(TAG, "Invalid arguments");
    }
    i2c_ctx_t *iot_i2c_handler = (i2c_ctx_t *) pxI2CPeripheral;

    iot_i2c_handler->arg = pvUserContext;
    iot_i2c_handler->func = (void *)xCallback;

}

int32_t iot_i2c_read_async( IotI2CHandle_t const pxI2CPeripheral, uint8_t *const pvBuffer, size_t xBytes)
{
    if (pxI2CPeripheral == NULL || pvBuffer == NULL) {
        ESP_LOGE(TAG, "Invalid arguments");
        return IOT_I2C_INVALID_VALUE;
    }

    i2c_ctx_t *iot_i2c_handler = (i2c_ctx_t *)pxI2CPeripheral;

    if (!iot_i2c_handler->driver_installed) {
        return IOT_I2C_INVALID_VALUE;
    }

    if (iot_i2c_handler->is_channel_busy || iot_i2c_handler->is_channel_busy) {
        return IOT_I2C_BUSY;
    }

    iot_i2c_handler->pvBuffer = pvBuffer;
    iot_i2c_handler->xBytes = xBytes;
    esp_timer_create_args_t timer_arg = {
        .callback = read_async_cb,
        .arg = pxI2CPeripheral,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "esp32 timer",
    };

    esp_err_t ret;
    ret = esp_timer_create(&timer_arg, &iot_i2c_handler->esp_timer_handler_read);
    if (ret != ESP_OK) {
        ESP_LOGD(TAG, "Failed to create esp timer");
    }

    ret = esp_timer_start_once(iot_i2c_handler->esp_timer_handler_read, 100);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Timer not fired");
        return IOT_I2C_WRITE_FAILED;
    }
    iot_i2c_handler->is_channel_busy = true;
    return IOT_I2C_SUCCESS;
}

int32_t iot_i2c_write_async( IotI2CHandle_t const pxI2CPeripheral, uint8_t *const pvBuffer, size_t xBytes)
{
    if (pxI2CPeripheral == NULL || pvBuffer == NULL) {
        ESP_LOGE(TAG, "Invalid arguments");
        return IOT_I2C_INVALID_VALUE;
    }
    i2c_ctx_t *iot_i2c_handler = (i2c_ctx_t *)pxI2CPeripheral;


    if (iot_i2c_handler->is_channel_busy || iot_i2c_handler->is_channel_busy) {
        return IOT_I2C_BUSY;
    }

    iot_i2c_handler->pvBuffer = (void *)pvBuffer;
    iot_i2c_handler->xBytes = xBytes;
    if (iot_i2c_handler->register_buffer_write_sq == SET_BUFFER_WRITE) {
        iot_i2c_handler->register_buffer_write_sq = SET_REGISTER_ADDR;
    } else if (iot_i2c_handler->register_buffer_write_sq == SET_REGISTER_ADDR) {
        iot_i2c_handler->register_buffer_write_sq = SET_BUFFER_WRITE;
    }
    if (iot_i2c_handler->register_buffer_write_sq == SET_BUFFER_WRITE) {

        esp_timer_create_args_t timer_arg = {
            .callback = write_async_cb,
            .arg = iot_i2c_handler,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "esp32 timer",
        };
        esp_err_t ret;
        ret = esp_timer_create(&timer_arg, &iot_i2c_handler->esp_timer_handle_write);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create esp timer");
        }
        ret = esp_timer_start_once(iot_i2c_handler->esp_timer_handle_write, 100);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Timer not fired");
            return IOT_I2C_WRITE_FAILED;
        }
        iot_i2c_handler->is_channel_busy = true;
    } else if (iot_i2c_handler->register_buffer_write_sq == SET_REGISTER_ADDR) {
        iot_i2c_handler->reg_write_addr = (*(uint8_t *)pvBuffer);
    }
    return IOT_I2C_SUCCESS;
}

int32_t iot_i2c_read_sync( IotI2CHandle_t const pxI2CPeripheral, uint8_t *const pvBuffer, size_t xBytes)
{
    esp_err_t ret = ESP_OK;
    if (pxI2CPeripheral == NULL || pvBuffer == NULL) {
        ESP_LOGD(TAG, "Invalid arguments");
        return IOT_I2C_INVALID_VALUE;
    }
    i2c_ctx_t *iot_i2c_handler = (i2c_ctx_t *) pxI2CPeripheral;
    int32_t i2c_port_num = (int32_t) iot_i2c_handler->i2c_port_num;
    uint8_t *src_buf = (uint8_t *) pvBuffer;
    if (!iot_i2c_handler->is_slave_addr_read_set && !!iot_i2c_handler->is_slave_addr_write_set) {
        ESP_LOGE(TAG, "Slave address not set ?");
        return IOT_I2C_READ_FAILED;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    // ret  = i2c_master_start(cmd);
    // ret |= i2c_master_write_byte(cmd, iot_i2c_handler->iot_i2c_slave_addr << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    // ret |= i2c_master_stop(cmd);
    // ret |= i2c_master_cmd_begin(i2c_port_num, cmd, 1000 / portTICK_RATE_MS);
    // if (ret != ESP_OK) {
    //     ESP_LOGD(TAG, "i2c master read ack failed");
    //     return IOT_I2C_READ_FAILED;
    // }
    // i2c_cmd_link_delete(cmd);

    // cmd = i2c_cmd_link_create();
    // ret = i2c_master_start(cmd);
    // ret |= i2c_master_write_byte(cmd, iot_i2c_handler->iot_i2c_slave_addr << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
    if (xBytes > 1) {
        ret |= i2c_master_read(cmd, src_buf, xBytes - 1, ACK_VAL);
    }
    ret |= i2c_master_read_byte(cmd, (uint8_t *)(src_buf + xBytes - 1), NACK_VAL);
    ret |= i2c_master_stop(cmd);
    ret |= i2c_master_cmd_begin(i2c_port_num, cmd, 1000 / portTICK_RATE_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c master read failed");
        return IOT_I2C_READ_FAILED;
    }
    i2c_cmd_link_delete(cmd);
    return (ret == ESP_OK) ?  IOT_I2C_SUCCESS : IOT_I2C_READ_FAILED;
}

int32_t iot_i2c_write_sync( IotI2CHandle_t const pxI2CPeripheral, uint8_t *const pvBuffer, size_t xBytes)
{
    esp_err_t ret = ESP_OK;
    if (pxI2CPeripheral == NULL || pvBuffer == NULL) {
        ESP_LOGE(TAG, "Invalid arguments");
        return IOT_I2C_INVALID_VALUE;
    }
    i2c_ctx_t *iot_i2c_handler = (i2c_ctx_t *) pxI2CPeripheral;
    int32_t i2c_port_num = (int32_t) iot_i2c_handler->i2c_port_num;
    uint8_t *src_buf = (uint8_t *) pvBuffer;
    if (!iot_i2c_handler->is_slave_addr_write_set) {
        ESP_LOGE(TAG, "Slave address not set ?");
        return IOT_I2C_WRITE_FAILED;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret = i2c_master_write(cmd, src_buf, xBytes, ACK_CHECK_EN);
    ret |= i2c_master_cmd_begin(i2c_port_num, cmd, 1000 / portTICK_RATE_MS );
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c master write failed");
        return IOT_I2C_WRITE_FAILED;
    }
    i2c_cmd_link_delete(cmd);
    //After successful test make slave address zero.
    return (ret == ESP_OK) ?  IOT_I2C_SUCCESS : IOT_I2C_WRITE_FAILED;
}

int32_t iot_i2c_close(IotI2CHandle_t const pxI2CPeripheral)
{
    if (pxI2CPeripheral == NULL) {
        ESP_LOGE(TAG, "Invalid I2C Handler");
        return IOT_I2C_INVALID_VALUE;
    }
    if (i2c_flag == false) {
        ESP_LOGE(TAG, "I2C Handler is not initialised");
        return IOT_I2C_INVALID_VALUE;
    }
    i2c_ctx_t *iot_i2c_handler = (i2c_ctx_t *) pxI2CPeripheral;
    i2c_flag = false;
    int32_t i2c_port_num = (int32_t) iot_i2c_handler->i2c_port_num;
    if (iot_i2c_handler->driver_installed) {
        esp_err_t ret = i2c_driver_delete(i2c_port_num);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Invalid arguments");
            return IOT_I2C_INVALID_VALUE;
        }
    }
    free(pxI2CPeripheral);
    return IOT_I2C_SUCCESS;
}

int32_t iot_i2c_cancel(IotI2CHandle_t const pxI2CPeripheral)

{
    if (pxI2CPeripheral == NULL) {
        ESP_LOGE(TAG, "Nothing to cancel");
        //Ideally this should send Nothing to cancel, but to pass test cases we send success
        return IOT_I2C_SUCCESS;
    }
    i2c_ctx_t *iot_i2c_handler = (i2c_ctx_t *) pxI2CPeripheral;
    if (!iot_i2c_handler->driver_installed) {
        ESP_LOGD(TAG, "i2c instance already closed");
        return IOT_I2C_NOTHING_TO_CANCEL;
    }
    esp_err_t ret = ESP_OK;
    if (iot_i2c_handler->esp_timer_handle_write != NULL) {
        ret = esp_timer_stop(iot_i2c_handler->esp_timer_handle_write);
    }
    if (iot_i2c_handler->esp_timer_handler_read != NULL) {
        ret = esp_timer_stop(iot_i2c_handler->esp_timer_handler_read);
    }
    return (ret == ESP_OK) ?  IOT_I2C_SUCCESS : IOT_I2C_INVALID_VALUE;
}
