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
#include "driver/i2c.h"
#include "board_gpio.h"

static const char *TAG = "esp-hal-i2c";

#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */
#define SET_REGISTER_ADDR 1
#define SET_BUFFER_WRITE 0
#define BYTES_TO_WRITE_AT_ONE_TIME 20
#define BYTES_TO_READ_AT_ONE_TIME 20
#define TICKS_TO_WAIT 10
static volatile uint8_t i2c_bit_mask;

typedef struct {
    IotI2CConfig_t iot_i2c_config;
    int32_t i2c_port_num;
    bool driver_installed;
    void (*func)(IotI2COperationStatus_t arg1, void *arg2);
    void *arg;
    void *pvBuffer;
    size_t xBytes;
    bool is_channel_busy;
    bool is_slave_addr_set;
    bool is_send_no_stop_flag_set;
    uint32_t bytes_to_read;
    uint32_t bytes_to_write;
    uint8_t slave_addr;
} i2c_ctx_t;

static struct esp_i2c_pin_config esp_i2c_pin_map[ESP_MAX_I2C_PORTS] = ESP_I2C_PIN_MAP;

IotI2CHandle_t iot_i2c_open(int32_t lI2CInstance)
{
    if (lI2CInstance < 0 || lI2CInstance > 1) {
        ESP_LOGE(TAG, "Invalid arguments");
        return NULL;
    }
    if (0x01 & (i2c_bit_mask >> lI2CInstance)) {
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
    i2c_bit_mask |= BIT(lI2CInstance);
    return iot_i2c_handler;
}

int32_t iot_i2c_ioctl( IotI2CHandle_t const pxI2CPeripheral, IotI2CIoctlRequest_t xI2CRequest, void *const pvBuffer)
{
    esp_err_t ret;
    if (xI2CRequest != eI2CSendNoStopFlag) {
        if (pxI2CPeripheral == NULL || pvBuffer == NULL) {
            ESP_LOGE(TAG, "Invalid arguments");
            return IOT_I2C_INVALID_VALUE;
        }
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

        if (i2c_ctx->driver_installed) {
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
    }
    case eI2CSetSlaveAddr : {
        i2c_ctx->slave_addr = (*(uint8_t *)pvBuffer);
        i2c_ctx->is_slave_addr_set = true;
        return IOT_I2C_SUCCESS;
    }
    case eI2CGetBusState : {
        IotI2CBusStatus_t *bus_state = (IotI2CBusStatus_t *) pvBuffer;
        *bus_state = (i2c_ctx->is_channel_busy == true) ? eI2cBusBusy : eI2CBusIdle;
        return IOT_I2C_SUCCESS;
    }
    case eI2CGetTxNoOfbytes : {
        uint16_t *tx_bytes = (uint16_t *) pvBuffer;
        *tx_bytes = i2c_ctx->bytes_to_write;
        return IOT_I2C_SUCCESS;
    }
    case eI2CGetRxNoOfbytes : {
        uint16_t *rx_bytes = (uint16_t *) pvBuffer;
        *rx_bytes = i2c_ctx->bytes_to_read;
        return IOT_I2C_SUCCESS;
    }
    case eI2CSendNoStopFlag : {
        i2c_ctx->is_send_no_stop_flag_set = true;
        return IOT_I2C_SUCCESS;
    }
    default :
        ESP_LOGE(TAG, "Invalid argument");
        return IOT_I2C_INVALID_VALUE;
    }
}

static void write_async_cb(void *arg)
{
    esp_err_t ret = ESP_OK;
    uint8_t bytes_to_write_tmp = 0;
    i2c_ctx_t *iot_i2c_handler = (i2c_ctx_t *) arg;
    int32_t i2c_port_num = (int32_t) iot_i2c_handler->i2c_port_num;
    uint8_t *src_buf = (uint8_t *) iot_i2c_handler->pvBuffer;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret = i2c_master_start(cmd);
    ret |= i2c_master_write_byte(cmd, iot_i2c_handler->slave_addr << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    while (iot_i2c_handler->xBytes > 0) {
        bytes_to_write_tmp = (iot_i2c_handler->xBytes > BYTES_TO_WRITE_AT_ONE_TIME) ? BYTES_TO_WRITE_AT_ONE_TIME : iot_i2c_handler->xBytes;
        ret |= i2c_master_write(cmd, src_buf, bytes_to_write_tmp, ACK_CHECK_EN);
        ret |= i2c_master_cmd_begin(i2c_port_num, cmd, 1000 / portTICK_RATE_MS );
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "ret %d", ret);
            ESP_LOGE(TAG, "i2c_master_cmd_begin failed");
            break;
        }
        iot_i2c_handler->xBytes -= bytes_to_write_tmp;
        src_buf += bytes_to_write_tmp;
        iot_i2c_handler->bytes_to_write += bytes_to_write_tmp;
    }
    if (iot_i2c_handler->is_send_no_stop_flag_set == false) {
        ret |= i2c_master_stop(cmd);
    }
    i2c_cmd_link_delete(cmd);

    iot_i2c_handler->is_channel_busy = false;
    iot_i2c_handler->is_send_no_stop_flag_set = false;
    if (iot_i2c_handler->func) {
        uint8_t op_status = (iot_i2c_handler->xBytes) ? eI2CDriverFailed : eI2CCompleted;
        iot_i2c_handler->func(op_status, iot_i2c_handler->arg);
    }
}

static void read_async_cb(void *arg)
{
    esp_err_t ret = ESP_OK;
    uint8_t bytes_to_read_tmp = 0;
    i2c_ctx_t *iot_i2c_handler = (i2c_ctx_t *) arg;
    int32_t i2c_port_num = (int32_t) iot_i2c_handler->i2c_port_num;
    uint8_t *src_buf = (uint8_t *) iot_i2c_handler->pvBuffer;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret = i2c_master_start(cmd);
    ret |= i2c_master_write_byte(cmd, iot_i2c_handler->slave_addr << 1 | I2C_MASTER_READ, ACK_CHECK_EN);

    while (iot_i2c_handler->xBytes > 1) {
        bytes_to_read_tmp = (iot_i2c_handler->xBytes > BYTES_TO_READ_AT_ONE_TIME) ? BYTES_TO_READ_AT_ONE_TIME : iot_i2c_handler->xBytes - 1;
        ret |= i2c_master_read(cmd, src_buf, bytes_to_read_tmp, ACK_VAL);
        ret |= i2c_master_cmd_begin(i2c_port_num, cmd, 1000 / portTICK_RATE_MS);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "i2c_master_cmd_begin 2 failed");
            break;
        }
        iot_i2c_handler->xBytes -= bytes_to_read_tmp;
        src_buf += bytes_to_read_tmp;
        iot_i2c_handler->bytes_to_read += bytes_to_read_tmp;
    }
    ret |= i2c_master_read_byte(cmd, src_buf, NACK_VAL);
    iot_i2c_handler->bytes_to_read++;
    iot_i2c_handler->xBytes--;
    if (iot_i2c_handler->is_send_no_stop_flag_set == false) {
        ret |= i2c_master_stop(cmd);
    }
    ret |= i2c_master_cmd_begin(i2c_port_num, cmd, 1000 / portTICK_RATE_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c_master_cmd_begin 2 failed");
    }
    i2c_cmd_link_delete(cmd);
    iot_i2c_handler->is_channel_busy = false;
    iot_i2c_handler->is_send_no_stop_flag_set = false;
    if (iot_i2c_handler->func) {
        uint8_t op_status = (iot_i2c_handler->xBytes) ? eI2CDriverFailed : eI2CCompleted;
        iot_i2c_handler->func(op_status, iot_i2c_handler->arg);
        iot_i2c_handler->func = NULL;
    }
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

    if (!iot_i2c_handler->is_slave_addr_set) {
        ESP_LOGE(TAG, "Slave address not set");
        return IOT_I2C_SLAVE_ADDRESS_NOT_SET;
    }

    if (!iot_i2c_handler->driver_installed) {
        return IOT_I2C_INVALID_VALUE;
    }

    if (iot_i2c_handler->is_channel_busy) {
        return IOT_I2C_BUSY;
    }

    iot_i2c_handler->pvBuffer = pvBuffer;
    iot_i2c_handler->xBytes = xBytes;
    iot_i2c_handler->bytes_to_read = 0;
    //Create timer with 0 wait time
    if (xTimerPendFunctionCall(read_async_cb, (void *)iot_i2c_handler, 0, TICKS_TO_WAIT) != pdTRUE) {
        ESP_LOGE(TAG, "%s: failed to queue pend function cb on timer for read async", __func__);
        return IOT_I2C_INVALID_VALUE;
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
    iot_i2c_handler->bytes_to_write = 0;
    //Create timer with 0 wait time
    if (xTimerPendFunctionCall(write_async_cb, (void *)iot_i2c_handler, 0, TICKS_TO_WAIT) != pdTRUE) {
        ESP_LOGE(TAG, "%s: failed to queue pend function cb on timer for write async", __func__);
        return IOT_I2C_INVALID_VALUE;
    }
    iot_i2c_handler->is_channel_busy = true;
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
    if (!iot_i2c_handler->is_slave_addr_set) {
        ESP_LOGE(TAG, "Slave address not set");
        return IOT_I2C_SLAVE_ADDRESS_NOT_SET;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret  = i2c_master_start(cmd);
    ret |= i2c_master_write_byte(cmd, iot_i2c_handler->slave_addr << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
    if (xBytes > 1) {
        ret |= i2c_master_read(cmd, src_buf, xBytes - 1, ACK_VAL);
    }
    ret |= i2c_master_read_byte(cmd, (uint8_t *)(src_buf + xBytes - 1), NACK_VAL);
    if (iot_i2c_handler->is_send_no_stop_flag_set == false) {
        ret |= i2c_master_stop(cmd);
    }
    ret |= i2c_master_cmd_begin(i2c_port_num, cmd, 1000 / portTICK_RATE_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c master read failed");
        return IOT_I2C_READ_FAILED;
    }
    i2c_cmd_link_delete(cmd);
    iot_i2c_handler->is_send_no_stop_flag_set = false;
    iot_i2c_handler->bytes_to_read = xBytes;
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
    if (!iot_i2c_handler->is_slave_addr_set) {
        ESP_LOGE(TAG, "Slave address not set");
        return IOT_I2C_SLAVE_ADDRESS_NOT_SET;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret  = i2c_master_start(cmd);
    ret |= i2c_master_write_byte(cmd, iot_i2c_handler->slave_addr << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    ret = i2c_master_write(cmd, src_buf, xBytes, ACK_CHECK_EN);
    if (iot_i2c_handler->is_send_no_stop_flag_set == false) {
        ret |= i2c_master_stop(cmd);
    }
    ret |= i2c_master_cmd_begin(i2c_port_num, cmd, 1000 / portTICK_RATE_MS );
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c master write failed");
        return IOT_I2C_WRITE_FAILED;
    }
    i2c_cmd_link_delete(cmd);
    iot_i2c_handler->bytes_to_write = xBytes;
    iot_i2c_handler->is_send_no_stop_flag_set = false;
    //After successful test make slave address zero.
    return (ret == ESP_OK) ?  IOT_I2C_SUCCESS : IOT_I2C_WRITE_FAILED;
}

int32_t iot_i2c_close(IotI2CHandle_t const pxI2CPeripheral)
{
    if (pxI2CPeripheral == NULL) {
        ESP_LOGE(TAG, "Invalid I2C Handler");
        return IOT_I2C_INVALID_VALUE;
    }
    i2c_ctx_t *iot_i2c_handler = (i2c_ctx_t *) pxI2CPeripheral;
    if (!(0x01 & (i2c_bit_mask >> iot_i2c_handler->i2c_port_num))) {
        ESP_LOGE(TAG, "I2C Handler is not initialised");
        return IOT_I2C_INVALID_VALUE;
    }
    i2c_bit_mask = i2c_bit_mask & ~(BIT(iot_i2c_handler->i2c_port_num));
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
    return IOT_I2C_FUNCTION_NOT_SUPPORTED;
}
