/*
 * Amazon FreeRTOS
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/**
 * @file iot_ble_mqtt.h
 * @brief GATT service for transferring MQTT packets over BLE
 */

#ifndef IOT_BLE_MQTT_H
#define IOT_BLE_MQTT_H

#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "stream_buffer.h"

#include "iot_ble.h"
#include "platform/iot_threads.h"


typedef enum IotBleDataTransferChannelStatus
{
    IOT_BLE_DATA_TRANSFER_CHANNEL_READY = 0,
    IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVE_START,
    IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVE_COMPLETE,
    IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_SENT,
    IOT_BLE_DATA_TRANSFER_CHANNEL_CLOSED

} IotBleDataTransferChannelStatus_t;


typedef struct IotBleDataTransferChannel IotBleDataTransferChannel_t;

typedef void ( * IotBleDataTransferChannelCallback_t ) ( IotBleDataTransferChannelStatus_t event, IotBleDataTransferChannel_t *pChannel, void *pContext );


struct IotBleDataTransferChannel
{
    uint8_t * pReceiveBuffer;                          /**< Pointer to the receiver buffer. */
    size_t    length;                                  /**< Length of the receive buffer in bytes. */
    size_t    head;                                    /**< Offset into the buffer to write data. */
    size_t    tail;                                    /**< Offset into the buffer to read data. */
    IotBleDataTransferChannelCallback_t callback;         /**< Callback invoked on various events on the channel. */
    void                *pContext;                     /**< Callback context. */
    uint32_t             timeout;                      /**< Timeout for the sending/receiving data. */
    bool                 isUsed;                       /**< Flag to indicate if the channel is used. */
    bool                 isReady;                      /**< Flag to indicate if the channel is ready to send/receive data. */
    bool                 pendingRead;                  /**< Flag to indicate if data is pending to be read */
    StreamBufferHandle_t sendBuffer;                   /**< Buffer to chache the data while sending out through BLE network. */
    IotSemaphore_t       sendLock;                     /**< Lock to protect access to the send buffer. */
    IotMutex_t           receiveLock;                  /**< Lock to protect access to the receive buffer. */

};


bool IotBleDataTransfer_Init( void );

IotBleDataTransferChannel_t * IotBleDataTransfer_Open( uint8_t serviceIdentifier );

bool IotBleDataTransfer_SetCallback( IotBleDataTransferChannel_t* pChannel, const IotBleDataTransferChannelCallback_t callback, void *pContext );

size_t IotBleDataTransfer_Send( IotBleDataTransferChannel_t * pChannel, const uint8_t * const pMessage, size_t messageLength );

size_t IotBleDataTransfer_Receive( IotBleDataTransferChannel_t * pChannel, uint8_t * pBuffer, size_t bytesRequested );

void IotBleDataTransfer_Close( IotBleDataTransferChannel_t * pChannel );

void IotBleDataTransfer_Reset( IotBleDataTransferChannel_t * pChannel );

#endif /* IOT_BLE_MQTT_H */
