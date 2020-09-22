// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include "iot_i2c.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c.h"
#include "iot_board_gpio.h"
#include "freertos/timers.h"

static const char *TAG = "esp-hal-i2c";

#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */
#define SET_REGISTER_ADDR 1
#define SET_BUFFER_WRITE 0
#define BYTES_TO_WRITE_AT_ONE_TIME 20
#define BYTES_TO_READ_AT_ONE_TIME 20
#define SEM_WAIT_TICKS 0
static volatile uint8_t i2c_bit_mask;

typedef struct {
    IotI2CConfig_t iot_i2c_config;
    int32_t i2c_port_num;
    bool driver_installed;
    void (*func)(IotI2COperationStatus_t arg1, void *arg2);
    void *arg;
    void *pvBuffer;
    size_t xBytes;
    bool is_slave_addr_set;
    bool is_send_no_stop_flag_set;
    uint32_t bytes_to_read;
    uint32_t bytes_to_write;
    uint8_t slave_addr;
    unsigned async_op: 1;
    i2c_cmd_handle_t cmd;
    SemaphoreHandle_t i2c_semph;
} i2c_ctx_t;

static struct esp_i2c_pin_config esp_i2c_pin_map[ESP_MAX_I2C_PORTS] = ESP_I2C_PIN_MAP;

static void IRAM_ATTR async_cb(i2c_trans_status_t status, void *arg);

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
    i2c_ctx->i2c_semph = xSemaphoreCreateBinary();
    if (i2c_ctx->i2c_semph == NULL) {
        ESP_LOGE(TAG, "Failed to create binary semaphore");
        free(i2c_ctx);
        return NULL;
    }
    xSemaphoreGive(i2c_ctx->i2c_semph);
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
        if (xSemaphoreTake(i2c_ctx->i2c_semph, SEM_WAIT_TICKS) == pdFALSE) {
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
                xSemaphoreGive(i2c_ctx->i2c_semph);
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
            xSemaphoreGive(i2c_ctx->i2c_semph);
            return IOT_I2C_INVALID_VALUE;
        }
        ret = i2c_driver_install(i2c_port_num, i2c_conf.mode, 0, 0, 0);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "driver install failed");
            xSemaphoreGive(i2c_ctx->i2c_semph);
            return IOT_I2C_INVALID_VALUE;
        }
        ESP_LOGD(TAG, "driver installed");
        if (i2c_ctx->iot_i2c_config.ulMasterTimeout > 0) {
            ret = i2c_set_timeout(i2c_port_num, (i2c_ctx->iot_i2c_config.ulMasterTimeout * 80));
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "timeout failed");
                xSemaphoreGive(i2c_ctx->i2c_semph);
                return IOT_I2C_INVALID_VALUE;
            }
        }
        i2c_ctx->driver_installed = true;
        i2c_master_register_callback_with_isr(i2c_ctx->i2c_port_num, async_cb, (void *) i2c_ctx);
        xSemaphoreGive(i2c_ctx->i2c_semph);
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
        if (xSemaphoreTake(i2c_ctx->i2c_semph, SEM_WAIT_TICKS) == pdFALSE) {
            return IOT_I2C_BUSY;
        }
        i2c_ctx->slave_addr = (*(uint8_t *)pvBuffer);
        i2c_ctx->is_slave_addr_set = true;
        xSemaphoreGive(i2c_ctx->i2c_semph);
        return IOT_I2C_SUCCESS;
    }
    case eI2CGetBusState : {
        IotI2CBusStatus_t *bus_state = (IotI2CBusStatus_t *) pvBuffer;
        *bus_state = (xSemaphoreTake(i2c_ctx->i2c_semph, SEM_WAIT_TICKS) == pdFALSE) ? eI2cBusBusy : eI2CBusIdle;
        if (*bus_state == eI2CBusIdle) {
            xSemaphoreGive(i2c_ctx->i2c_semph);
        }
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

static void i2c_callback_dispatch(void *arg, i2c_trans_status_t status)
{
    i2c_ctx_t *iot_i2c_handler = (i2c_ctx_t *) arg;
    uint8_t op_status = (status != I2C_TRANS_STATUS_DONE) ? eI2CDriverFailed : eI2CCompleted;
    i2c_cmd_link_delete(iot_i2c_handler->cmd);
    xSemaphoreGive(iot_i2c_handler->i2c_semph);
    if (iot_i2c_handler->func) {
        iot_i2c_handler->func(op_status, iot_i2c_handler->arg);
    }
}

static void IRAM_ATTR async_cb(i2c_trans_status_t status, void *arg)
{
    i2c_ctx_t *iot_i2c_handler = (i2c_ctx_t *) arg;
    if (iot_i2c_handler->async_op) {
        if (xTimerPendFunctionCallFromISR(i2c_callback_dispatch, arg, status, NULL) == pdFALSE) {
            abort();
        }
        iot_i2c_handler->async_op = false;
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
    if (!iot_i2c_handler->driver_installed) {
        return IOT_I2C_INVALID_VALUE;
    }
    if (xSemaphoreTake(iot_i2c_handler->i2c_semph, SEM_WAIT_TICKS) == pdFALSE) {
        return IOT_I2C_BUSY;
    }

    iot_i2c_handler->async_op = true;
    iot_i2c_handler->cmd = i2c_cmd_link_create();
    ret  = i2c_master_start(iot_i2c_handler->cmd);
    ret |= i2c_master_write_byte(iot_i2c_handler->cmd, iot_i2c_handler->slave_addr << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
    if (xBytes > 1) {
        ret |= i2c_master_read(iot_i2c_handler->cmd, src_buf, xBytes - 1, ACK_VAL);
    }
    ret |= i2c_master_read_byte(iot_i2c_handler->cmd, (uint8_t *)(src_buf + xBytes - 1), NACK_VAL);
    if (iot_i2c_handler->is_send_no_stop_flag_set == false) {
        ret |= i2c_master_stop(iot_i2c_handler->cmd);
    }
    ret |= i2c_master_cmd_begin_async(i2c_port_num, iot_i2c_handler->cmd);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c master read failed");
        xSemaphoreGive(iot_i2c_handler->i2c_semph);
        return IOT_I2C_READ_FAILED;
    }
    iot_i2c_handler->is_send_no_stop_flag_set = false;
    iot_i2c_handler->bytes_to_read = xBytes;
    return (ret == ESP_OK) ?  IOT_I2C_SUCCESS : IOT_I2C_READ_FAILED;
}

int32_t iot_i2c_write_async( IotI2CHandle_t const pxI2CPeripheral, uint8_t *const pvBuffer, size_t xBytes)
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

    if (!iot_i2c_handler->driver_installed) {
        return IOT_I2C_INVALID_VALUE;
    }

    if (xSemaphoreTake(iot_i2c_handler->i2c_semph, SEM_WAIT_TICKS) == pdFALSE) {
        return IOT_I2C_BUSY;
    }

    iot_i2c_handler->async_op = true;
    iot_i2c_handler->cmd = i2c_cmd_link_create();
    ret  = i2c_master_start(iot_i2c_handler->cmd);
    ret |= i2c_master_write_byte(iot_i2c_handler->cmd, iot_i2c_handler->slave_addr << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    ret = i2c_master_write(iot_i2c_handler->cmd, src_buf, xBytes, ACK_CHECK_EN);
    if (iot_i2c_handler->is_send_no_stop_flag_set == false) {
        ret |= i2c_master_stop(iot_i2c_handler->cmd);
    }
    ret |= i2c_master_cmd_begin_async(i2c_port_num, iot_i2c_handler->cmd);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c master write failed");
        xSemaphoreGive(iot_i2c_handler->i2c_semph);
        return IOT_I2C_WRITE_FAILED;
    }
    iot_i2c_handler->bytes_to_write = xBytes;
    iot_i2c_handler->is_send_no_stop_flag_set = false;
    return (ret == ESP_OK) ?  IOT_I2C_SUCCESS : IOT_I2C_WRITE_FAILED;
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
    if (xSemaphoreTake(iot_i2c_handler->i2c_semph, SEM_WAIT_TICKS) == pdFALSE) {
        return IOT_I2C_BUSY;
    }

    iot_i2c_handler->async_op = false;
    iot_i2c_handler->cmd = i2c_cmd_link_create();
    ret  = i2c_master_start(iot_i2c_handler->cmd);
    ret |= i2c_master_write_byte(iot_i2c_handler->cmd, iot_i2c_handler->slave_addr << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
    if (xBytes > 1) {
        ret |= i2c_master_read(iot_i2c_handler->cmd, src_buf, xBytes - 1, ACK_VAL);
    }
    ret |= i2c_master_read_byte(iot_i2c_handler->cmd, (uint8_t *)(src_buf + xBytes - 1), NACK_VAL);
    if (iot_i2c_handler->is_send_no_stop_flag_set == false) {
        ret |= i2c_master_stop(iot_i2c_handler->cmd);
    }
    ret |= i2c_master_cmd_begin(i2c_port_num, iot_i2c_handler->cmd, 1000 / portTICK_RATE_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c master read failed");
        xSemaphoreGive(iot_i2c_handler->i2c_semph);
        return IOT_I2C_READ_FAILED;
    }
    i2c_cmd_link_delete(iot_i2c_handler->cmd);
    iot_i2c_handler->is_send_no_stop_flag_set = false;
    iot_i2c_handler->bytes_to_read = xBytes;
    xSemaphoreGive(iot_i2c_handler->i2c_semph);
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
    if (xSemaphoreTake(iot_i2c_handler->i2c_semph, SEM_WAIT_TICKS) == pdFALSE) {
        return IOT_I2C_BUSY;
    }

    iot_i2c_handler->async_op = false;
    iot_i2c_handler->cmd = i2c_cmd_link_create();
    ret  = i2c_master_start(iot_i2c_handler->cmd);
    ret |= i2c_master_write_byte(iot_i2c_handler->cmd, iot_i2c_handler->slave_addr << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    ret = i2c_master_write(iot_i2c_handler->cmd, src_buf, xBytes, ACK_CHECK_EN);
    if (iot_i2c_handler->is_send_no_stop_flag_set == false) {
        ret |= i2c_master_stop(iot_i2c_handler->cmd);
    }
    ret |= i2c_master_cmd_begin(i2c_port_num, iot_i2c_handler->cmd, 1000 / portTICK_RATE_MS );
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c master write failed");
        xSemaphoreGive(iot_i2c_handler->i2c_semph);
        return IOT_I2C_WRITE_FAILED;
    }
    i2c_cmd_link_delete(iot_i2c_handler->cmd);
    iot_i2c_handler->bytes_to_write = xBytes;
    iot_i2c_handler->is_send_no_stop_flag_set = false;
    xSemaphoreGive(iot_i2c_handler->i2c_semph);
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
    if (iot_i2c_handler->i2c_semph) {
        xSemaphoreTake(iot_i2c_handler->i2c_semph, portMAX_DELAY);
    }
    i2c_bit_mask = i2c_bit_mask & ~(BIT(iot_i2c_handler->i2c_port_num));
    int32_t i2c_port_num = (int32_t) iot_i2c_handler->i2c_port_num;
    if (iot_i2c_handler->driver_installed) {
        esp_err_t ret = i2c_driver_delete(i2c_port_num);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Invalid arguments");
            xSemaphoreGive(iot_i2c_handler->i2c_semph);
            return IOT_I2C_INVALID_VALUE;
        }
    }
    if (iot_i2c_handler->i2c_semph) {
        vSemaphoreDelete(iot_i2c_handler->i2c_semph);
        iot_i2c_handler->i2c_semph = NULL;
    }
    free(pxI2CPeripheral);
    return IOT_I2C_SUCCESS;
}

int32_t iot_i2c_cancel(IotI2CHandle_t const pxI2CPeripheral)

{
    return IOT_I2C_FUNCTION_NOT_SUPPORTED;
}
