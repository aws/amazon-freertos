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

#include "iot_uart.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "soc/uart_reg.h"
#include "string.h"
#include "iot_board_gpio.h"
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
    char *write_buf;
    size_t bytes_to_read;
    size_t bytes_to_write;
    size_t async_bytes_written;
    size_t async_bytes_read;
    int32_t uart_port_num;
    esp_timer_handle_t uart_timer_wr_hdl;
    esp_timer_handle_t uart_timer_rd_hdl;
    SemaphoreHandle_t uart_rd_cb_wait;
    SemaphoreHandle_t uart_wr_cb_wait;
    unsigned wr_op_in_progress: 1;
    unsigned rd_op_in_progress: 1;
    unsigned uart_rd_op_cancel_req: 1;
    unsigned uart_wr_op_cancel_req: 1;
} uart_ctx_t;

static struct esp_uart_pin_config esp_uart_pin_map[ESP_MAX_UART_PORTS] = ESP_UART_PIN_MAP;
static volatile uint8_t uart_bit_mask, uart_is_installed_bit_mask;

IRAM_ATTR static void esp32_uart_write_cb(void *param, unsigned int ulparam)
{
    ESP_EARLY_LOGD(TAG, "%s", __func__);
    uart_ctx_t *uart_ctx = (uart_ctx_t *) param;
    int bytes_write_tmp = 0;
    uint8_t *write_buffer = uart_ctx->write_buf;
    uint8_t op_status = eUartWriteCompleted;
    int actual_bytes_to_write = 0;
    if (uart_ctx->uart_wr_op_cancel_req) {
        ESP_EARLY_LOGD(TAG, "cancel operation recieved");
        op_status = eUartLastWriteFailed;
        if (uart_ctx->func) {
            ESP_EARLY_LOGD(TAG, "%s invoking callback %p", __func__, uart_ctx->func);
            uart_ctx->func(op_status, uart_ctx->arg);
        }
        uart_ctx->bytes_to_write = 0;
        uart_ctx->wr_op_in_progress = false;
        uart_ctx->uart_wr_op_cancel_req = false;
        xSemaphoreGive(uart_ctx->uart_wr_cb_wait);
        return;
    }

    if (uart_ctx->bytes_to_write == 0) {
        if (uart_ctx->func) {
            ESP_EARLY_LOGD(TAG, "%s invoking callback %p", __func__, uart_ctx->func);
            uart_ctx->func(op_status, uart_ctx->arg);
        }
        uart_ctx->bytes_to_write = 0;
        uart_ctx->wr_op_in_progress = false;
        xSemaphoreGive(uart_ctx->uart_wr_cb_wait);
        return;
    }

    actual_bytes_to_write = uart_ctx->bytes_to_write < UART_FIFO_LEN ? uart_ctx->bytes_to_write : UART_FIFO_LEN;
    uart_enable_intr_mask(uart_ctx->uart_port_num, UART_TX_DONE_INT_ENA_M);
    bytes_write_tmp = uart_write_bytes(uart_ctx->uart_port_num, write_buffer, actual_bytes_to_write);
    uart_ctx->write_buf += bytes_write_tmp;
    uart_ctx->bytes_to_write -= bytes_write_tmp;
    uart_ctx->async_bytes_written += bytes_write_tmp;
}

IRAM_ATTR static void esp32_uart_read_cb(void *param, unsigned int ulparam)
{
    ESP_EARLY_LOGD(TAG, "%s", __func__);
    uart_ctx_t *uart_ctx = (uart_ctx_t *) param;
    int bytes_read_tmp = 0;
    uint8_t op_status = eUartReadCompleted;
    uint8_t *read_buffer = uart_ctx->read_buf;
    int actual_bytes_to_read = UART_FIFO_LEN * 2;

    if (uart_ctx->uart_rd_op_cancel_req) {
        ESP_EARLY_LOGD(TAG, "cancel operation recieved");
        op_status = eUartLastReadFailed;
        if (uart_ctx->func) {
            ESP_EARLY_LOGD(TAG, "%s invoking callback %p", __func__, uart_ctx->func);
            uart_ctx->func(op_status, uart_ctx->arg);
        }
        uart_ctx->bytes_to_read = 0;
        uart_ctx->rd_op_in_progress = false;
        uart_ctx->uart_rd_op_cancel_req = false;
        xSemaphoreGive(uart_ctx->uart_rd_cb_wait);
        return;
    }

    if (actual_bytes_to_read > uart_ctx->bytes_to_read) {
        actual_bytes_to_read = uart_ctx->bytes_to_read;
    }
    uart_enable_rx_intr(uart_ctx->uart_port_num);
    bytes_read_tmp = uart_read_bytes(uart_ctx->uart_port_num, read_buffer, actual_bytes_to_read, 0);
    uart_ctx->read_buf += bytes_read_tmp;
    uart_ctx->bytes_to_read -= bytes_read_tmp;
    uart_ctx->async_bytes_read += bytes_read_tmp;
    if (uart_ctx->bytes_to_read <= 0) {
        op_status = eUartReadCompleted;
        if (uart_ctx->func) {
            ESP_EARLY_LOGD(TAG, "%s invoking callback %p", __func__, uart_ctx->func);
            uart_ctx->func(op_status, uart_ctx->arg);
        }
        uart_ctx->bytes_to_read = 0;
        uart_ctx->rd_op_in_progress = false;
        uart_ctx->uart_rd_op_cancel_req = false;
        xSemaphoreGive(uart_ctx->uart_rd_cb_wait);
        return;
    }
}

IRAM_ATTR void uart_event_cb(uart_event_t uart_event, void *param)
{
    uart_ctx_t *uart_ctx = (uart_ctx_t *) param;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (uart_event.type == UART_TX_DONE && uart_ctx->wr_op_in_progress) {
        if (esp_timer_start_once(uart_ctx->uart_timer_wr_hdl, 0)!= ESP_OK) {
            ESP_EARLY_LOGE(TAG, "%s: failed to create timer for write async", __func__);
            xSemaphoreGiveFromISR(uart_ctx->uart_wr_cb_wait, &xHigherPriorityTaskWoken);
            return;
        }
    } else if (uart_event.type == UART_DATA && uart_ctx->rd_op_in_progress) {
        if (esp_timer_start_once(uart_ctx->uart_timer_rd_hdl, 0)!= ESP_OK) {
            ESP_EARLY_LOGE(TAG, "%s: failed to create timer for read async", __func__);
            xSemaphoreGiveFromISR(uart_ctx->uart_rd_cb_wait, &xHigherPriorityTaskWoken);
            return;
        }
    }
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
                            | UART_PARITY_ERR_INT_ENA_M
                            | UART_TX_DONE_INT_ENA_M,
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
    return ret;
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
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to install uart driver");
            free(uart_ctx);
            return NULL;
        } else if (ret == ESP_ERR_INVALID_ARG) {
            uart_is_installed_bit_mask |= BIT(lUartInstance);
            ESP_LOGD(TAG, "uart bit already installed mask: %d\n", uart_is_installed_bit_mask);
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
        //Create a callback function to handle UART event from ISR
        ret = uart_register_callback_with_isr(lUartInstance, uart_event_cb, (void *)uart_ctx);

        /* Ensure first semaphore take could succeed */
        xSemaphoreGive(uart_ctx->uart_rd_cb_wait);
        xSemaphoreGive(uart_ctx->uart_wr_cb_wait);

        //Create timer for write async
        esp_timer_create_args_t write_timer = {
            .callback = (esp_timer_cb_t)esp32_uart_write_cb,
            .arg = (void *) uart_ctx,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "uart wr timer",
        };
        ret = esp_timer_create(&write_timer, &uart_ctx->uart_timer_wr_hdl);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create uart write timer %d", ret);
            return NULL;
        }

        //Create timer for read async
        esp_timer_create_args_t read_timer = {
            .callback = (esp_timer_cb_t)esp32_uart_read_cb,
            .arg = (void *) uart_ctx,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "uart rd timer",
        };
        ret = esp_timer_create(&read_timer, &uart_ctx->uart_timer_rd_hdl);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create uart read timer %d", ret);
            return NULL;
        }

        /* Set default config in UART context before returning */
        uart_ctx->iot_uart_conf.ulBaudrate = IOT_UART_BAUD_RATE_DEFAULT;
        uart_ctx->iot_uart_conf.xParity = eUartParityNone;
        uart_ctx->iot_uart_conf.xStopbits = eUartStopBitsOne;
        uart_ctx->iot_uart_conf.ucWordlength = 8;
        uart_ctx->iot_uart_conf.ucFlowControl = 0;

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
        size_t *tx_bytes_write = (size_t *)pvBuffer;
        *tx_bytes_write = uart_ctx->async_bytes_written;
        return IOT_UART_SUCCESS;
    } else if (xUartRequest == eGetRxNoOfbytes) {
        size_t *rx_bytes_read = (size_t *)pvBuffer;
        *rx_bytes_read = uart_ctx->async_bytes_read;
        return IOT_UART_SUCCESS;
    }
    if (!iot_uart_handler->wr_op_in_progress && !iot_uart_handler->rd_op_in_progress) {
        switch(xUartRequest) {
            case eUartSetConfig : {
                int32_t uart_port_num = uart_ctx->uart_port_num;
                IotUARTConfig_t *iot_uart_config = (IotUARTConfig_t *) pvBuffer;
                uart_config_t uart_config = {0};
                uart_config.baud_rate = iot_uart_config->ulBaudrate;
                if (iot_uart_config->ucWordlength == 5) {
                    uart_config.data_bits = UART_DATA_5_BITS;
                } else if (iot_uart_config->ucWordlength == 6) {
                    uart_config.data_bits = UART_DATA_6_BITS;
                } else if (iot_uart_config->ucWordlength == 7) {
                    uart_config.data_bits = UART_DATA_7_BITS;
                } else if (iot_uart_config->ucWordlength == 8) {
                    uart_config.data_bits = UART_DATA_8_BITS;
                } else {
                    return IOT_UART_INVALID_VALUE;
                }

                if (iot_uart_config->ucFlowControl == true) {
                    uart_config.flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS;
                    uart_config.rx_flow_ctrl_thresh = (UART_FIFO_LEN / 2);
                } else {
                    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
                }
                switch (iot_uart_config->xParity) {
                    case eUartParityNone:
                    uart_config.parity = UART_PARITY_DISABLE;
                    break;
                    case eUartParityOdd:
                    uart_config.parity = UART_PARITY_ODD;
                    break;
                    case eUartParityEven:
                    uart_config.parity = UART_PARITY_EVEN;
                    break;
                    default:
                    uart_config.parity = UART_PARITY_DISABLE;
                }
                switch (iot_uart_config->xStopbits) {
                    case eUartStopBitsOne:
                    uart_config.stop_bits = UART_STOP_BITS_1;
                    break;
                    case eUartStopBitsTwo:
                    uart_config.stop_bits = UART_STOP_BITS_2;
                    break;
                    default:
                    uart_config.stop_bits = UART_STOP_BITS_1;
                }
                if (iot_uart_config->ulBaudrate == 0) {
                    uart_config.baud_rate = IOT_UART_BAUD_RATE_DEFAULT;
                }
                ret = uart_driver_delete(uart_port_num);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to delete UART driver");
                    return IOT_UART_INVALID_VALUE;
                }
                ret = iot_uart_driver_install(uart_port_num, uart_config);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to install UART driver");
                    return IOT_UART_INVALID_VALUE;
                }
                //Create a callback function to handle UART event from ISR
                ret = uart_register_callback_with_isr(uart_port_num, uart_event_cb, (void *)iot_uart_handler);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to register callback");
                    return IOT_UART_INVALID_VALUE;
                }

                memcpy(&uart_ctx->iot_uart_conf, iot_uart_config, sizeof(IotUARTConfig_t));
                return IOT_UART_SUCCESS;
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
    uart_ctx_t *uart_ctx = (uart_ctx_t *) pxUartPeripheral;

    uart_ctx->arg = pvUserContext;
    uart_ctx->func = (void *)xCallback;
}

int32_t iot_uart_read_async(IotUARTHandle_t const pxUartPeripheral, uint8_t * const pvBuffer, size_t xBytes)
{
    ESP_LOGD(TAG, "%s: %p %p %d", __func__, pxUartPeripheral, pvBuffer, xBytes);

    if (pxUartPeripheral == NULL || pvBuffer == NULL || xBytes == 0) {
        ESP_LOGE(TAG, "Invalid arguments");
        return IOT_UART_INVALID_VALUE;
    }

    uart_ctx_t *uart_ctx = (uart_ctx_t *) pxUartPeripheral;
    //Read from another task to make async
    if (!xSemaphoreTake(uart_ctx->uart_rd_cb_wait, SEM_WAIT_TIME)) {
        ESP_LOGE(TAG, "%s: failed to acquire read sem", __func__);
        return IOT_UART_READ_FAILED;
    }

    uart_ctx->read_buf = (char *) pvBuffer;
    uart_ctx->bytes_to_read = xBytes;
    uart_ctx->async_bytes_read = 0;

    esp_timer_start_once(uart_ctx->uart_timer_rd_hdl, 0);
    uart_ctx->rd_op_in_progress = true;
    return IOT_UART_SUCCESS;
}

int32_t iot_uart_write_async(IotUARTHandle_t const pxUartPeripheral, uint8_t * const pvBuffer, size_t xBytes)
{
    ESP_LOGD(TAG, "%s: %p %p %d", __func__, pxUartPeripheral, pvBuffer, xBytes);

    if (pxUartPeripheral == NULL || pvBuffer == NULL || xBytes == 0) {
        ESP_LOGD(TAG, "Invalid arguments");
        return IOT_UART_INVALID_VALUE;
    }

    uart_ctx_t *uart_ctx = (uart_ctx_t *) pxUartPeripheral;
    if (!xSemaphoreTake(uart_ctx->uart_wr_cb_wait, SEM_WAIT_TIME)) {
        ESP_LOGE(TAG, "%s: failed to acquire write sem", __func__);
        return IOT_UART_WRITE_FAILED;
    }
    uart_ctx->wr_op_in_progress = true;
    uart_ctx->write_buf = (char *)pvBuffer;
    uart_ctx->bytes_to_write = xBytes;
    uart_ctx->async_bytes_written = 0;
    esp_timer_start_once(uart_ctx->uart_timer_wr_hdl, 0);
    return IOT_UART_SUCCESS;
}

int32_t iot_uart_read_sync(IotUARTHandle_t const pxUartPeripheral, uint8_t * const pvBuffer, size_t xBytes)
{
    if (pxUartPeripheral == NULL || pvBuffer == NULL || xBytes == 0) {
        ESP_LOGD(TAG, "Invalid arguments");
        return IOT_UART_INVALID_VALUE;
    }
    uint8_t *src_buf = (uint8_t *) pvBuffer;
    int32_t uart_port_num = ((uart_ctx_t *)pxUartPeripheral)->uart_port_num;
    int bytes_read = uart_read_bytes(uart_port_num, src_buf, xBytes, portMAX_DELAY);
    return (bytes_read == xBytes) ? IOT_UART_SUCCESS : IOT_UART_READ_FAILED;
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
    uart_enable_intr_mask(uart_port_num, UART_TX_DONE_INT_ENA_M);
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
        ESP_LOGE(TAG, "Invalid arguments");
        return IOT_UART_INVALID_VALUE;
    }
    uart_ctx_t *uart_ctx = (uart_ctx_t *) pxUartPeripheral;
    int32_t uart_port_num = uart_ctx->uart_port_num;
    if (0x01 & uart_bit_mask >> uart_port_num) {
        if (uart_ctx->rd_op_in_progress) {
            uart_ctx->uart_rd_op_cancel_req = true;
            /* Ensure no active operations on UART before flusing all data */
            if (!xSemaphoreTake(uart_ctx->uart_rd_cb_wait, SEM_WAIT_TIME)) {
                ESP_LOGE(TAG, "%s: failed to acquire read sem", __func__);
                return IOT_UART_INVALID_VALUE;
            }
            xSemaphoreGive(uart_ctx->uart_rd_cb_wait);
        }
        if (uart_ctx->wr_op_in_progress) {
            uart_ctx->uart_wr_op_cancel_req = true;
            if (!xSemaphoreTake(uart_ctx->uart_wr_cb_wait, SEM_WAIT_TIME)) {
                ESP_LOGE(TAG, "%s: failed to acquire write sem", __func__);
                return IOT_UART_INVALID_VALUE;
            }
            xSemaphoreGive(uart_ctx->uart_wr_cb_wait);
        }
        if (0x01 & !(uart_is_installed_bit_mask >> uart_port_num)) {
            ret |= uart_flush_input(uart_port_num);
            ret |= uart_driver_delete(uart_port_num);
            uart_is_installed_bit_mask &= ~(BIT(uart_port_num));
        }
        esp_timer_stop(uart_ctx->uart_timer_wr_hdl);
        esp_timer_stop(uart_ctx->uart_timer_rd_hdl);
        esp_timer_delete(uart_ctx->uart_timer_wr_hdl);
        esp_timer_delete(uart_ctx->uart_timer_rd_hdl);
        if (uart_ctx->uart_rd_cb_wait) {
            vSemaphoreDelete(uart_ctx->uart_rd_cb_wait);
        }
        if (uart_ctx->uart_wr_cb_wait) {
            vSemaphoreDelete(uart_ctx->uart_wr_cb_wait);
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
    uart_ctx_t *uart_ctx = (uart_ctx_t *) pxUartPeripheral;
    int32_t uart_port_num = uart_ctx->uart_port_num;

    if (!uart_ctx->rd_op_in_progress && !uart_ctx->wr_op_in_progress) {
        return IOT_UART_NOTHING_TO_CANCEL;
    } else if (uart_ctx->rd_op_in_progress) {
        uart_ctx->uart_rd_op_cancel_req = true;
        ESP_LOGD(TAG, "operation cancel request: %d\n", uart_ctx->uart_rd_op_cancel_req);

        /* Start a timer to trigger the cancel operation. */
        if (esp_timer_start_once(uart_ctx->uart_timer_rd_hdl, 0)!= ESP_OK) {
            ESP_EARLY_LOGE(TAG, "%s: failed to create timer for cancel request", __func__);
            return IOT_UART_INVALID_VALUE;
        }

        /* Ensure no active operations on UART before flusing all data */
        if (!xSemaphoreTake(uart_ctx->uart_rd_cb_wait, SEM_WAIT_TIME)) {
            ESP_LOGE(TAG, "%s: failed to acquire read sem", __func__);
            uart_ctx->uart_rd_op_cancel_req = false;
            return IOT_UART_INVALID_VALUE;
        }
        xSemaphoreGive(uart_ctx->uart_rd_cb_wait);

        ret |= uart_flush_input(uart_port_num);
        return (ret == ESP_OK) ? IOT_UART_SUCCESS : IOT_UART_INVALID_VALUE;
    } else {
        ESP_LOGE(TAG, "write cancel not supported");
        return IOT_UART_FUNCTION_NOT_SUPPORTED;
    }
}
