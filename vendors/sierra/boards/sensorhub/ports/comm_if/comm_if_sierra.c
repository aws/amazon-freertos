/*
 * Amazon FreeRTOS Cellular Preview Release
 * Copyright (C) 20 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/*-----------------------------------------------------------*/
/* The config header is always included first. */
#include "aws_cellular_config.h"
#include "cellular_config_defaults.h"

/* Cellular comm interface include file. */
#include "cellular_comm_interface.h"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#define COMM_IF_DEBUG 0

#if COMM_IF_DEBUG

#include <stdarg.h>
#include "esp_log.h"
#define ESP_LOGE(tag, format, ...)      ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR, tag, format, ##__VA_ARGS__)
#define COMM_IF_LOGE(tag, format, ...)  ESP_LOGE(tag, format, ##__VA_ARGS__)
#define COMM_IF_LOGW(tag, format, ...)  ESP_LOGW(tag, format, ##__VA_ARGS__)
#define COMM_IF_LOGI(tag, format, ...)  ESP_LOGI(tag, format, ##__VA_ARGS__)
#define COMM_IF_LOGD(tag, format, ...)  ESP_LOGD(tag, format, ##__VA_ARGS__)
#define COMM_IF_LOGV(tag, format, ...)  ESP_LOGV(tag, format, ##__VA_ARGS__)
#define COMM_IF_LOG_BUFFER_HEX(tag, buffer, buff_len) ESP_LOG_BUFFER_HEX(tag, buffer, buff_len)

static const char *COMM_IF_TAG = "COMM_IF_SIERRA";

#else /* COMM_IF_DEBUG */

#define COMM_IF_LOGE(tag, format, ...)
#define COMM_IF_LOGW(tag, format, ...)
#define COMM_IF_LOGI(tag, format, ...)
#define COMM_IF_LOGD(tag, format, ...)
#define COMM_IF_LOGV(tag, format, ...)
#define COMM_IF_LOG_BUFFER_HEX(tag, buffer, buff_len)

#endif /* COMM_IF_DEBUG */

#define BX310X_HL78_TXD         (GPIO_NUM_12)
#define BX310X_HL78_RXD         (GPIO_NUM_13)
#define BX310X_HL78_RTS         (GPIO_NUM_14)
#define BX310X_HL78_CTS         (GPIO_NUM_15)

#define BX310X_BUS_EN_PIN   (GPIO_NUM_27)

#define BX310X_HL78_UART (UART_NUM_1)

#define EVENTS_QUEUE_SIZE       (64)
#define BX310X_UART_BUF_SIZE    (2048)

/**
 * @brief A context of the communication interface.
 */
struct CommIfContext
{
    // static portMUX_TYPE lock;
    CellularCommInterfaceReceiveCallback_t pRecvCB;
    void *pUserData;
    bool isOpen;
};

static struct CommIfContext _CommIfContext = {0};
static QueueHandle_t _CommIfQueue;

// this function should be called at startup
static void hw_lowlevel_init(void)
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1ULL<<BX310X_BUS_EN_PIN);
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    gpio_set_level(BX310X_BUS_EN_PIN, 0);
}

static void uart_event_task(void *pvParameters)
{
    CellularCommInterfaceError_t retComm = IOT_COMM_INTERFACE_SUCCESS;
    uart_event_t event;
    for (;;)
    {
        //Waiting for UART event.
        if (xQueueReceive(_CommIfQueue, (void *)&event, (portTickType)portMAX_DELAY))
        {
            switch (event.type)
            {
            //Event of UART receving data
            /*We'd better handler data event fast, there would be much more data events than
                other types of events. If we take too much time on data event, the queue might
                be full.*/
            case UART_DATA:
                COMM_IF_LOGI(COMM_IF_TAG, "[UART DATA]: %d", event.size);
                if (_CommIfContext.pRecvCB != NULL)
                {
                    retComm = _CommIfContext.pRecvCB(_CommIfContext.pUserData,
                                           (CellularCommInterfaceHandle_t)&_CommIfContext);
                    if( retComm == IOT_COMM_INTERFACE_SUCCESS )
                    {
                        portYIELD_FROM_ISR();
                    }
                }
                else
                {
                    COMM_IF_LOGE(COMM_IF_TAG, "Callback is null");
                }
                break;
            //Event of HW FIFO overflow detected
            case UART_FIFO_OVF:
                COMM_IF_LOGE(COMM_IF_TAG, "hw fifo overflow");
                // If fifo overflow happened, you should consider adding flow control for your application.
                // The ISR has already reset the rx FIFO,
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(BX310X_HL78_UART);
                xQueueReset(_CommIfQueue);
                break;
            //Event of UART ring buffer full
            case UART_BUFFER_FULL:
                COMM_IF_LOGE(COMM_IF_TAG, "ring buffer full");
                // If buffer full happened, you should consider encreasing your buffer size
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(BX310X_HL78_UART);
                xQueueReset(_CommIfQueue);
                break;
            //Event of UART RX break detected
            case UART_BREAK:
                COMM_IF_LOGE(COMM_IF_TAG, "uart rx break");
                break;
            //Event of UART parity check error
            case UART_PARITY_ERR:
                COMM_IF_LOGE(COMM_IF_TAG, "uart parity error");
                break;
            //Event of UART frame error
            case UART_FRAME_ERR:
                COMM_IF_LOGE(COMM_IF_TAG, "uart frame error");
                break;
            //UART_PATTERN_DET
            case UART_PATTERN_DET:
                COMM_IF_LOGI(COMM_IF_TAG, "[UART_PATTERN_DET]: size %d", event.size);
                if (_CommIfContext.pRecvCB != NULL)
                {
                    COMM_IF_LOGI(COMM_IF_TAG, "[UART_PATTERN_DET]: Invoke receive callback");
                    retComm = _CommIfContext.pRecvCB(_CommIfContext.pUserData,
                                           (CellularCommInterfaceHandle_t)&_CommIfContext);
                    if( retComm == IOT_COMM_INTERFACE_SUCCESS )
                    {
                        portYIELD_FROM_ISR();
                    }
                }
                else
                {
                    COMM_IF_LOGE(COMM_IF_TAG, "Callback is null");
                }
                break;
            //Others
            default:
                COMM_IF_LOGE(COMM_IF_TAG, "uart event type: %d", event.type);
                break;
            }
        }
    }
    vTaskDelete(NULL);
}

/*-----------------------------------------------------------*/

static CellularCommInterfaceError_t _prvCommIntfOpen(CellularCommInterfaceReceiveCallback_t receiveCallback,
                                                     void *pUserData,
                                                     CellularCommInterfaceHandle_t *pCommInterfaceHandle);
static CellularCommInterfaceError_t _prvCommIntfSend(CellularCommInterfaceHandle_t commInterfaceHandle,
                                                     const uint8_t *pData,
                                                     uint32_t dataLength,
                                                     uint32_t timeoutMilliseconds,
                                                     uint32_t *pDataSentLength);
static CellularCommInterfaceError_t _prvCommIntfReceive(CellularCommInterfaceHandle_t commInterfaceHandle,
                                                        uint8_t *pBuffer,
                                                        uint32_t bufferLength,
                                                        uint32_t timeoutMilliseconds,
                                                        uint32_t *pDataReceivedLength);
static CellularCommInterfaceError_t _prvCommIntfClose(CellularCommInterfaceHandle_t commInterfaceHandle);

/*-----------------------------------------------------------*/

CellularCommInterface_t CellularCommInterface =
    {
        .open = _prvCommIntfOpen,
        .send = _prvCommIntfSend,
        .recv = _prvCommIntfReceive,
        .close = _prvCommIntfClose};

/*-----------------------------------------------------------*/

static CellularCommInterfaceError_t _prvCommIntfOpen(CellularCommInterfaceReceiveCallback_t receiveCallback,
                                                     void *pUserData,
                                                     CellularCommInterfaceHandle_t *pCommInterfaceHandle)
{
    // COMM_IF_LOGI(COMM_IF_TAG, "Request to open\n");
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 120,
        .use_ref_tick = 0x00,
    };

    hw_lowlevel_init();

    if (_CommIfContext.isOpen == true)
    {
        COMM_IF_LOGE(COMM_IF_TAG, "[CommIntfOpen]: Driver failure");
        return IOT_COMM_INTERFACE_FAILURE;
    }
    else
    {
        memset(&_CommIfContext, 0, sizeof(struct CommIfContext));
    }

    if (pCommInterfaceHandle == NULL)
    {
        COMM_IF_LOGE(COMM_IF_TAG, "[CommIntfOpen]: Interface handler is NULL");
        return IOT_COMM_INTERFACE_BAD_PARAMETER;
    }
    _CommIfContext.pRecvCB = receiveCallback;
    _CommIfContext.pUserData = pUserData;
    *pCommInterfaceHandle = (CellularCommInterfaceHandle_t)&_CommIfContext;

    if (uart_driver_install(BX310X_HL78_UART, BX310X_UART_BUF_SIZE, 0, EVENTS_QUEUE_SIZE, &_CommIfQueue, 0) != ESP_OK)
    {
        COMM_IF_LOGE(COMM_IF_TAG, "[CommIntfOpen]: Failed to install driver");
        return IOT_COMM_INTERFACE_DRIVER_ERROR;
    }
    if (uart_param_config(BX310X_HL78_UART, &uart_config) != ESP_OK)
    {
        COMM_IF_LOGE(COMM_IF_TAG, "[CommIntfOpen]: Failed to configure UART");
        return IOT_COMM_INTERFACE_DRIVER_ERROR;
    }
    if (uart_set_pin(BX310X_HL78_UART, BX310X_HL78_TXD, BX310X_HL78_RXD, BX310X_HL78_RTS, BX310X_HL78_CTS) != ESP_OK)
    {
        COMM_IF_LOGE(COMM_IF_TAG, "[CommIntfOpen]: Failed to change UART's pins");
        return IOT_COMM_INTERFACE_DRIVER_ERROR;
    }
    // Work around: CTS pin may not be asertted if a pull up is set
    ESP_ERROR_CHECK(gpio_set_pull_mode(BX310X_HL78_CTS, GPIO_FLOATING));

    _CommIfContext.isOpen = true;

    ESP_ERROR_CHECK(uart_enable_rx_intr(BX310X_HL78_UART));
    ESP_ERROR_CHECK(uart_set_rx_timeout(BX310X_HL78_UART, 9));

    //Create a task to handler UART events from ISR
    xTaskCreate(uart_event_task, "uart_event_task", 2048, &_CommIfContext, tskIDLE_PRIORITY + 5, NULL);

    return IOT_COMM_INTERFACE_SUCCESS;
}

/*-----------------------------------------------------------*/

static CellularCommInterfaceError_t _prvCommIntfClose(CellularCommInterfaceHandle_t commInterfaceHandle)
{
    if (uart_driver_delete(BX310X_HL78_UART) != ESP_OK)
    {
        COMM_IF_LOGE(COMM_IF_TAG, "[CommIntfClose]: Failed to uninstall UART driver");
        return IOT_COMM_INTERFACE_DRIVER_ERROR;
    }

    _CommIfContext.isOpen = false;

    return IOT_COMM_INTERFACE_SUCCESS;
}

/*-----------------------------------------------------------*/

static CellularCommInterfaceError_t _prvCommIntfSend(CellularCommInterfaceHandle_t commInterfaceHandle,
                                                     const uint8_t *pData,
                                                     uint32_t dataLength,
                                                     uint32_t timeoutMilliseconds,
                                                     uint32_t *pDataSentLength)
{
    COMM_IF_LOGI(COMM_IF_TAG, "[CommIntfSend]: Request to send\n");
    uint32_t len = 0;
    len = uart_write_bytes(BX310X_HL78_UART, (const char *)pData, dataLength);
    if (len != dataLength)
    {
        COMM_IF_LOGE(COMM_IF_TAG, "[CommIntfSend]: Buffer error");
        return IOT_COMM_INTERFACE_NO_MEMORY;
    }
    if (pDataSentLength != NULL)
    {
        *pDataSentLength = len;
    }
    if (uart_wait_tx_done(BX310X_HL78_UART, timeoutMilliseconds) != ESP_OK)
    {
        COMM_IF_LOGE(COMM_IF_TAG, "[CommIntfSend]: Timeout");
        return IOT_COMM_INTERFACE_TIMEOUT;
    }
    COMM_IF_LOGI(COMM_IF_TAG, "[CommIntfSend]: Sent: %d\n", len);

    return IOT_COMM_INTERFACE_SUCCESS;
}

/*-----------------------------------------------------------*/

static CellularCommInterfaceError_t _prvCommIntfReceive(CellularCommInterfaceHandle_t commInterfaceHandle,
                                                        uint8_t *pBuffer,
                                                        uint32_t bufferLength,
                                                        uint32_t timeoutMilliseconds,
                                                        uint32_t *pDataReceivedLength)
{
    COMM_IF_LOGI(COMM_IF_TAG, "[CommIntfReceive]: Request to receive\n");
    COMM_IF_LOGI(COMM_IF_TAG, "[CommIntfReceive]: Buffer available %d\n", bufferLength);

    int len = 0;
    size_t avail;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(BX310X_HL78_UART, &avail));
    avail = avail > bufferLength ? bufferLength : avail;
    len = uart_read_bytes(BX310X_HL78_UART, pBuffer, avail, timeoutMilliseconds);
    if (len < 0)
    {
        COMM_IF_LOGE(COMM_IF_TAG, "[CommIntfReceive]: Timeout");
        return IOT_COMM_INTERFACE_DRIVER_ERROR;
    }
    if (pDataReceivedLength != NULL)
    {
        *pDataReceivedLength = len;
    }
    pBuffer[len] = 0;

    COMM_IF_LOGI(COMM_IF_TAG, "\n================\n%s\n", (char *)pBuffer);
//     COMM_IF_LOG_BUFFER_HEX(COMM_IF_TAG, pBuffer, *pDataReceivedLength);
    COMM_IF_LOGI(COMM_IF_TAG, "\n================\n");
    COMM_IF_LOGI(COMM_IF_TAG, "[CommIntfReceive]: Received %d\n", *pDataReceivedLength);

    return IOT_COMM_INTERFACE_SUCCESS;
}

/*-----------------------------------------------------------*/
