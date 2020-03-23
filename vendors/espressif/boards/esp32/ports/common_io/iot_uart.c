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

#include "iot_uart.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "board_gpio.h"
#include "freertos/timers.h"

static const char *TAG = "esp-hal-uart";
/**/
#define BUF_SIZE (1024) //This needs to be more then the UART FIFO Length(>128)
#define UART_READ_CB_STACK (512 * 4)
#define SEM_WAIT_TIME (60000 / portTICK_RATE_MS)
#define UART_TX_WAIT_TIME (60000 / portTICK_RATE_MS)
/* UART hardware FIFO is of 128 bytes and default threshold value for RX full FIFO interupt is 120 bytes.
 * When crital sections of other peripherals(for example WiFi or Bluetooth in power save) run for longer
 * time(sometimes interupts are masked), there is a possibility of UART RX FIFO to overflow.
 * Hence reducing RX full FIFO threshold to 60 bytes, gives us time to handle uart interupt
 * since there is more room in hardware FIFO(128 - 60 = 68 bytes) i.e. making FIFO empty more frequently.
 */
#define UART_RX_FULL_THRESH  (60)
#define UART_TIMEOUT_THRESH  (10)
#define UART_TX_EMPTY_THRESH (10)

typedef struct {
    IotUARTConfig_t iot_uart_conf;
    void (*func)(IotUARTOperationStatus_t arg1, void *arg2);
    void *arg;
    char *read_buf;
    size_t bytes_to_read;
    size_t bytes_to_write;
    int32_t uart_port_num;
    SemaphoreHandle_t uart_rd_cb_wait;
    SemaphoreHandle_t uart_wr_cb_wait;
    unsigned uart_write_op_in_progress: 1;
    unsigned uart_read_op_in_progress: 1;
    unsigned uart_op_cancel_req: 1;
} uart_ctx_t;

static struct esp_uart_pin_config esp_uart_pin_map[ESP_MAX_UART_PORTS] = ESP_UART_PIN_MAP;
static volatile uint8_t uart_bit_mask;

static void esp32_uart_write_cb(void *param, unsigned int ulparam)
{
    uart_ctx_t *iot_uart_handler = (uart_ctx_t *) param;
    uint8_t op_status = 0;

    ESP_LOGD(TAG, "%s", __func__);
    if (uart_wait_tx_done(iot_uart_handler->uart_port_num, UART_TX_WAIT_TIME) != ESP_OK) {
        ESP_LOGE(TAG, "uart write async failed");
        op_status = eUartLastWriteFailed;
    } else {
        op_status = eUartWriteCompleted;
    }
    if (iot_uart_handler->func) {
        ESP_LOGD(TAG, "%s invoking callback %p", __func__, iot_uart_handler->func);
        iot_uart_handler->func(op_status, iot_uart_handler->arg);
    }
    iot_uart_handler->bytes_to_write = 0;
    iot_uart_handler->uart_write_op_in_progress = false;
    xSemaphoreGive(iot_uart_handler->uart_wr_cb_wait);
}

static void esp32_uart_read_cb(void *param, unsigned int ulparam)
{
    uart_ctx_t *iot_uart_handler = (uart_ctx_t *) param;
    int bytes_read_tmp = 0;
    int bytes_read = iot_uart_handler->bytes_to_read;
    uint8_t op_status = 0;
    uint8_t *read_buffer = iot_uart_handler->read_buf;

    ESP_LOGD(TAG, "%s", __func__);

    while (bytes_read > 0) {
        /* Non blocking uart read */
        bytes_read_tmp = uart_read_bytes(iot_uart_handler->uart_port_num, read_buffer, bytes_read, 0);
        if (iot_uart_handler->uart_op_cancel_req) {
            ESP_LOGD(TAG, "cancel operation recieved");
            break;
        }
        if (bytes_read_tmp == 0) {
            vTaskDelay(5);  //wait for 5 ticks since there is nothing on the uart line
        } else {
            bytes_read -= bytes_read_tmp;
            read_buffer += bytes_read_tmp;
            iot_uart_handler->bytes_to_read = bytes_read;
        }
    }
    if (iot_uart_handler->uart_op_cancel_req) {
        op_status = eUartLastReadFailed;
    } else {
        op_status = eUartReadCompleted;
    }

    if (iot_uart_handler->func) {
        ESP_LOGD(TAG, "%s invoking callback %p", __func__, iot_uart_handler->func);
        iot_uart_handler->func(op_status, iot_uart_handler->arg);
    }

    iot_uart_handler->bytes_to_read = 0;
    iot_uart_handler->uart_read_op_in_progress = false;
    iot_uart_handler->uart_op_cancel_req = false;
    xSemaphoreGive(iot_uart_handler->uart_rd_cb_wait);
}

static esp_err_t iot_uart_driver_install(uint8_t uart_num, uart_config_t uart_conf)
{
    esp_err_t ret;
    uart_intr_config_t uart_intr = {
        .intr_enable_mask = UART_RXFIFO_FULL_INT_ENA_M
                            | UART_RXFIFO_TOUT_INT_ENA_M
                            | UART_FRM_ERR_INT_ENA_M
                            | UART_RXFIFO_OVF_INT_ENA_M
                            | UART_BRK_DET_INT_ENA_M
                            | UART_PARITY_ERR_INT_ENA_M,
        .rxfifo_full_thresh = UART_RX_FULL_THRESH,
        .rx_timeout_thresh = UART_TIMEOUT_THRESH,
        .txfifo_empty_intr_thresh = UART_TX_EMPTY_THRESH
    };
    ret = uart_param_config(uart_num, &uart_conf);
    ret |= uart_set_pin(uart_num, esp_uart_pin_map[uart_num].txd_pin, esp_uart_pin_map[uart_num].rxd_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    ret |= uart_driver_install(uart_num, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0);
    ret |= uart_intr_config(uart_num, &uart_intr);
    if (ret != ESP_ERR_INVALID_ARG && ret != ESP_OK) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

IotUARTHandle_t iot_uart_open(int32_t lUartInstance)
{
    if (lUartInstance < 0 || lUartInstance > ESP_MAX_UART_PORTS) {
        return NULL;
    }
    if (!(0x01 & uart_bit_mask >> lUartInstance)) {
        uart_ctx_t *uart_ctx = (uart_ctx_t *) calloc(1, sizeof(uart_ctx_t));
        if (uart_ctx == NULL) {
            ESP_LOGD(TAG, "Could not allocate memory for uart context");
            return NULL;
        }
        //Set default configuration
        uart_ctx->uart_port_num = lUartInstance;
        uart_config_t uart_config = {
            .baud_rate = IOT_UART_BAUD_RATE_DEFAULT,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
        };
        esp_err_t ret = iot_uart_driver_install(lUartInstance, uart_config);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to install uart driver");
            free(uart_ctx);
            return NULL;
        }

        uart_ctx->uart_rd_cb_wait = xSemaphoreCreateBinary();
        if (uart_ctx->uart_rd_cb_wait == NULL) {
            ESP_LOGD(TAG, "Failed to create read binary semaphore");
            free(uart_ctx);
            return NULL;
        }

        uart_ctx->uart_wr_cb_wait = xSemaphoreCreateBinary();
        if (uart_ctx->uart_wr_cb_wait == NULL) {
            ESP_LOGD(TAG, "Failed to create write binary semaphore");
            vSemaphoreDelete(uart_ctx->uart_rd_cb_wait);
            free(uart_ctx);
            return NULL;
        }

        /* Ensure first semaphore take could succeed */
        xSemaphoreGive(uart_ctx->uart_rd_cb_wait);
        xSemaphoreGive(uart_ctx->uart_wr_cb_wait);

        IotUARTHandle_t iot_uart_handler = (void *) uart_ctx;
        uart_bit_mask = uart_bit_mask | BIT(lUartInstance);
        return iot_uart_handler;
    } else {
        ESP_LOGD(TAG, "driver already open for the instance");
        return NULL;
    }
}

int32_t iot_uart_ioctl(IotUARTHandle_t const pxUartPeripheral, IotUARTIoctlRequest_t xUartRequest, void * const pvBuffer)
{
    esp_err_t ret;
    uart_ctx_t *iot_uart_handler = (uart_ctx_t *) pxUartPeripheral;

    if (pxUartPeripheral == NULL || pvBuffer == NULL) {
        ESP_LOGD(TAG, "Invalid arguments");
        return IOT_UART_INVALID_VALUE;
    }

    uart_ctx_t *uart_ctx = (uart_ctx_t *) pxUartPeripheral;
    if (xUartRequest == eGetTxNoOfbytes) {
        size_t tx_bytes_write = (size_t)pvBuffer;
        tx_bytes_write = uart_ctx->bytes_to_write;
        return IOT_UART_SUCCESS;
    } else if (xUartRequest == eGetRxNoOfbytes) {
        size_t rx_bytes_read = (size_t)pvBuffer;
        rx_bytes_read = uart_ctx->bytes_to_read;
        return IOT_UART_SUCCESS;
    }
    if (!iot_uart_handler->uart_write_op_in_progress && !iot_uart_handler->uart_read_op_in_progress) {
        switch(xUartRequest) {
            case eUartSetConfig : {
                int32_t uart_port_num = uart_ctx->uart_port_num;
                IotUARTConfig_t *iot_uart_config = (IotUARTConfig_t *) pvBuffer;
                memcpy(&uart_ctx->iot_uart_conf, iot_uart_config, sizeof(IotUARTConfig_t));
                uart_config_t uart_config;
                uart_config.baud_rate = iot_uart_config->ulBaudrate;
                uart_config.data_bits = iot_uart_config->ucWordlength;    
                uart_config.flow_ctrl = iot_uart_config->ucFlowControl;
                switch (iot_uart_config->xParity) {
                    case eUartParityNone:
                    uart_config.parity = UART_PARITY_DISABLE;
                    case eUartParityOdd:
                    uart_config.parity = UART_PARITY_ODD;
                    case eUartParityEven:
                    uart_config.parity = UART_PARITY_EVEN;
                    default:
                    uart_config.parity = UART_PARITY_DISABLE;
                }
                switch (iot_uart_config->xStopbits) {
                    case eUartStopBitsOne:
                    uart_config.stop_bits = UART_STOP_BITS_1;
                    case eUartStopBitsTwo:
                    uart_config.stop_bits = UART_STOP_BITS_2;
                    default:
                    uart_config.stop_bits = UART_STOP_BITS_1;
                }
                if (iot_uart_config->ulBaudrate == 0) {
                    uart_config.baud_rate = IOT_UART_BAUD_RATE_DEFAULT;
                }
                ret = uart_driver_delete(uart_port_num);
                ret |= iot_uart_driver_install(uart_port_num, uart_config);
                return (ret == ESP_OK) ? IOT_UART_SUCCESS : IOT_UART_INVALID_VALUE;
            }
            case eUartGetConfig : {
                IotUARTConfig_t *iot_uart_config = (IotUARTConfig_t *) pvBuffer;
                memcpy(iot_uart_config, &uart_ctx->iot_uart_conf, sizeof(IotUARTConfig_t));
                return IOT_UART_SUCCESS;
            }
            default :
            return IOT_UART_INVALID_VALUE;
        }
    } else {
        return IOT_UART_BUSY;
    }
}

void iot_uart_set_callback(IotUARTHandle_t const pxUartPeripheral, IotUARTCallback_t xCallback, void * pvUserContext)
{
    if (pxUartPeripheral == NULL || xCallback == NULL) {
        ESP_LOGD(TAG, "Invalid arguments");
    }
    uart_ctx_t *iot_uart_handler = (uart_ctx_t *) pxUartPeripheral;

    iot_uart_handler->arg = pvUserContext;
    iot_uart_handler->func = (void *)xCallback;
}

int32_t iot_uart_read_async(IotUARTHandle_t const pxUartPeripheral, uint8_t * const pvBuffer, size_t xBytes)
{
    ESP_LOGD(TAG, "%s: %p %p %d", __func__, pxUartPeripheral, pvBuffer, xBytes);

    if (pxUartPeripheral == NULL || pvBuffer == NULL || xBytes == 0) {
        ESP_LOGD(TAG, "Invalid arguments");
        return IOT_UART_INVALID_VALUE;
    }

    uart_ctx_t *uart_ctx = (uart_ctx_t *) pxUartPeripheral;
    uart_ctx->read_buf = (char *) pvBuffer;
    uart_ctx->bytes_to_read = xBytes;

    //Read from another task to make async
    if (!xSemaphoreTake(uart_ctx->uart_rd_cb_wait, SEM_WAIT_TIME)) {
        ESP_LOGE(TAG, "%s: failed to acquire read sem", __func__);
        return IOT_UART_READ_FAILED;
    }
    uart_ctx->uart_read_op_in_progress = true;
    //Create timer with 0 wait time
    if (xTimerPendFunctionCall(esp32_uart_read_cb, (void *)pxUartPeripheral, 0, 0) != pdTRUE) {
        ESP_LOGE(TAG, "%s: failed to create timer for read async", __func__);
        xSemaphoreGive(uart_ctx->uart_rd_cb_wait);
        return IOT_UART_READ_FAILED;
    }

    return IOT_UART_SUCCESS;
}

int32_t iot_uart_write_async(IotUARTHandle_t const pxUartPeripheral, uint8_t * const pvBuffer, size_t xBytes)
{
    ESP_LOGD(TAG, "%s: %p %p %d", __func__, pxUartPeripheral, pvBuffer, xBytes);

    if (pxUartPeripheral == NULL || pvBuffer == NULL || xBytes == 0) {
        ESP_LOGD(TAG, "Invalid arguments");
        return IOT_UART_INVALID_VALUE;
    }

    char *src_buf = (char *) pvBuffer;
    uart_ctx_t *uart_ctx = (uart_ctx_t *) pxUartPeripheral;
    int32_t uart_port_num = uart_ctx->uart_port_num;
    if (!xSemaphoreTake(uart_ctx->uart_wr_cb_wait, SEM_WAIT_TIME)) {
        ESP_LOGE(TAG, "%s: failed to acquire write sem", __func__);
        return IOT_UART_WRITE_FAILED;
    }
    uart_ctx->uart_write_op_in_progress = true;
    uart_ctx->bytes_to_write = xBytes;
    int bytes_written = uart_write_bytes(uart_port_num, src_buf, xBytes);
    //Create timer with 0 ticks wait time
    if (xTimerPendFunctionCall(esp32_uart_write_cb, (void *)pxUartPeripheral, 0, 1) != pdTRUE) {
        ESP_LOGE(TAG, "%s: failed to create timer for write async", __func__);
        xSemaphoreGive(uart_ctx->uart_wr_cb_wait);
        return IOT_UART_WRITE_FAILED;
    }
    return (bytes_written == xBytes) ? IOT_UART_SUCCESS : IOT_UART_WRITE_FAILED;
}

int32_t iot_uart_read_sync(IotUARTHandle_t const pxUartPeripheral, uint8_t * const pvBuffer, size_t xBytes)
{
    if (pxUartPeripheral == NULL || pvBuffer == NULL || xBytes == 0) {
        ESP_LOGD(TAG, "Invalid arguments");
        return IOT_UART_INVALID_VALUE;
    }
    uint8_t *src_buf = (uint8_t *) pvBuffer;
    int32_t uart_port_num = ((uart_ctx_t *)pxUartPeripheral)->uart_port_num;
    int bytes_written = uart_read_bytes(uart_port_num, src_buf, xBytes, portMAX_DELAY);
    return (bytes_written == xBytes) ? IOT_UART_SUCCESS : IOT_UART_READ_FAILED;
}

int32_t iot_uart_write_sync(IotUARTHandle_t const pxUartPeripheral, uint8_t * const pvBuffer, size_t xBytes)
{
    if (pxUartPeripheral == NULL || pvBuffer == NULL || xBytes == 0) {
        ESP_LOGD(TAG, "Invalid arguments");
        return IOT_UART_INVALID_VALUE;
    }
    char *src_buf = (char *) pvBuffer;
    uart_ctx_t *uart_ctx = (uart_ctx_t *) pxUartPeripheral;
    int32_t uart_port_num = uart_ctx->uart_port_num;
    int bytes_written = uart_write_bytes(uart_port_num, src_buf, xBytes);
    if (bytes_written == xBytes) {
        if (uart_wait_tx_done(uart_port_num, UART_TX_WAIT_TIME) != ESP_OK) {
            ESP_LOGE(TAG, "uart write sync failed");
            return IOT_UART_WRITE_FAILED;
        }
    }
    return IOT_UART_SUCCESS;
}

int32_t iot_uart_close(IotUARTHandle_t const pxUartPeripheral)
{
    esp_err_t ret = ESP_OK;
    if (pxUartPeripheral == NULL) {
        ESP_LOGD(TAG, "Invalid arguments");
        return IOT_UART_INVALID_VALUE;
    }
    uart_ctx_t *iot_uart_handler = (uart_ctx_t *) pxUartPeripheral;
    int32_t uart_port_num = iot_uart_handler->uart_port_num;
    if (0x01 & uart_bit_mask >> uart_port_num) {
        if (iot_uart_handler->uart_read_op_in_progress) {
            iot_uart_handler->uart_op_cancel_req = true;
            /* Ensure no active operations on UART before flusing all data */
            if (!xSemaphoreTake(iot_uart_handler->uart_rd_cb_wait, SEM_WAIT_TIME)) {
                ESP_LOGE(TAG, "%s: failed to acquire read sem", __func__);
                return IOT_UART_INVALID_VALUE;
            }
            xSemaphoreGive(iot_uart_handler->uart_rd_cb_wait);
        }
        if (iot_uart_handler->uart_write_op_in_progress) {
            if (!xSemaphoreTake(iot_uart_handler->uart_wr_cb_wait, SEM_WAIT_TIME)) {
                ESP_LOGE(TAG, "%s: failed to acquire write sem", __func__);
                return IOT_UART_INVALID_VALUE;
            }
            xSemaphoreGive(iot_uart_handler->uart_wr_cb_wait);
        }
        ret |= uart_flush_input(uart_port_num);
        ret |= uart_driver_delete(uart_port_num);
        if (iot_uart_handler->uart_rd_cb_wait) {
            vSemaphoreDelete(iot_uart_handler->uart_rd_cb_wait);
        }
        if (iot_uart_handler->uart_wr_cb_wait) {
            vSemaphoreDelete(iot_uart_handler->uart_wr_cb_wait);
        }
        uart_bit_mask = uart_bit_mask & ~(BIT(uart_port_num));
        free(pxUartPeripheral);
        return (ret == ESP_OK) ? IOT_UART_SUCCESS : IOT_UART_INVALID_VALUE;
    } else {
        return IOT_UART_INVALID_VALUE;
    }
}

int32_t iot_uart_cancel(IotUARTHandle_t const pxUartPeripheral)
{
    esp_err_t ret;
    if (pxUartPeripheral == NULL) {
        ESP_LOGD(TAG, "Invalid arguments");
        return IOT_UART_INVALID_VALUE;
    }
    uart_ctx_t *iot_uart_handler = (uart_ctx_t *) pxUartPeripheral;
    int32_t uart_port_num = iot_uart_handler->uart_port_num;

    if (!iot_uart_handler->uart_read_op_in_progress && !iot_uart_handler->uart_write_op_in_progress) {
        return IOT_UART_NOTHING_TO_CANCEL;
    } else if (iot_uart_handler->uart_read_op_in_progress) {
        iot_uart_handler->uart_op_cancel_req = true;
        ESP_LOGD(TAG, "operation cancel request: %d\n", iot_uart_handler->uart_op_cancel_req);
        /* Ensure no active operations on UART before flusing all data */
        if (!xSemaphoreTake(iot_uart_handler->uart_rd_cb_wait, SEM_WAIT_TIME)) {
            ESP_LOGE(TAG, "%s: failed to acquire read sem", __func__);
            return IOT_UART_INVALID_VALUE;
        }
        xSemaphoreGive(iot_uart_handler->uart_rd_cb_wait);

        iot_uart_handler->uart_op_cancel_req = false;

        ret |= uart_flush_input(uart_port_num);
        return (ret == ESP_OK) ? IOT_UART_SUCCESS : IOT_UART_INVALID_VALUE;
    } else {
        ESP_LOGE(TAG, "write cancel not supported");
        return IOT_UART_FUNCTION_NOT_SUPPORTED;
    }
}
