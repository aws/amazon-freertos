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

/*
 * @brief Various events supported by a ble data transfer channel.
 */
typedef enum IotBleDataTransferChannelEvent
{
    IOT_BLE_DATA_TRANSFER_CHANNEL_OPENED = 0,            /**< Indicates if the channel is opened and ready to read or write data. */
    IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVE_START,    /**< Event invoked when the first chunk of a data stream is received on the channel. */
    IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVE_COMPLETE, /**< Event invoked when the last chunk of a data stream is received on the channel. */
    IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_SENT,             /**< Event invoked after the last chunk of data stream is sent over the channel. */
    IOT_BLE_DATA_TRANSFER_CHANNEL_CLOSED                 /**< Event invoked when the channel is closed. */

} IotBleDataTransferChannelEvent_t;

/**
 * @brief Forward declaration of Data transfer channel structure.
 */
typedef struct IotBleDataTransferChannel IotBleDataTransferChannel_t;

/**
 * @brief Callback invoked whenever an event occurs on the data transfer channel.
 */
typedef void ( * IotBleDataTransferChannelCallback_t ) ( IotBleDataTransferChannelEvent_t event, IotBleDataTransferChannel_t *pChannel, void *pContext );

/**
 * @brief Structure used to represent a data transfer channel. 
 */
struct IotBleDataTransferChannel
{
    uint8_t * pReceiveBuffer;                          /**< Pointer to the receiver buffer. */
    size_t    length;                                  /**< Length of the receive buffer in bytes. */
    size_t    head;                                    /**< Offset into the buffer to write data. */
    size_t    tail;                                    /**< Offset into the buffer to read data. */
    IotBleDataTransferChannelCallback_t callback;      /**< Callback invoked on various events on the channel. */
    void                *pContext;                     /**< Callback context. */
    uint32_t             timeout;                      /**< Timeout value in milliseconds for the sending/receiving data. */
    bool                 isUsed;                       /**< Flag to indicate if the channel is used. */
    bool                 isOpen;                       /**< Flag to indicate if the channel is ready to send/receive data. */
    bool                 pendingRead;                  /**< Flag to indicate if data is pending to be read */
    StreamBufferHandle_t sendBuffer;                   /**< Buffer to chache the data while sending out through BLE network. */
    IotSemaphore_t       sendLock;                     /**< Lock to protect access to the send buffer. */
    IotMutex_t           receiveLock;                  /**< Lock to protect access to the receive buffer. */
};


/**
 * @brief Initializes the data transfer services.
 * Data transfer services are statically configured as the attribute table are written in flash.
 * 
 * @return True if all data transfer services are intialized successfully.
 * 
 */
bool IotBleDataTransfer_Init( void );

/**
 * @brief Open Channel for sending or receiving BLE data.
 * Channel need not be opened immediately, in which case user can set a callback to be notified of when the channel is opened.
 * 
 * @param[in] channelIdentifier Identifier used to uniquely identify a channel.
 * @return Pointer to the data transfer channel structure. 
 */
IotBleDataTransferChannel_t * IotBleDataTransfer_Open( uint8_t channelIdentifier );

/**
 * @brief Set callbacks to be notified of events received on a data transfer channel.
 * 
 * @param[in] pChannel Channel on which the callback is set
 * @param[in] callback The callback.
 * @param[in] pContext Context associated with callback.
 * 
 * @return True if callback is set succesfully, false if callback is already set or channel is not opened.
 */
bool IotBleDataTransfer_SetCallback( IotBleDataTransferChannel_t* pChannel, const IotBleDataTransferChannelCallback_t callback, void *pContext );

/**
 * @brief Sent data over a ble data transfer channel.
 * 
 * @param[in] pChannel Pointer to data transfer channel.
 * @param[in] pMessage Pointer to the message to be sent.
 * @param[in] messageLength Length in bytes of the message to be sent.
 * 
 * @return Number of bytes of message actually sent.  
 */
size_t IotBleDataTransfer_Send( IotBleDataTransferChannel_t * pChannel, const uint8_t * const pMessage, size_t messageLength );

/**
 * @brief Receive over a ble data transfer channel.
 * 
 * @param[in] pChannel Pointer to data transfer channel.
 * @param[in] pBuffer Pointer to the buffer where the data will be copied.
 * @param[in] bytesRequested Number of bytes of data requested.
 * 
 * @return Number of bytes of data returned ( should be <= Number of bytes requested ).
 */
size_t IotBleDataTransfer_Receive( IotBleDataTransferChannel_t * pChannel, uint8_t * pBuffer, size_t bytesRequested );

/**
 * @brief Close a ble data tranfer channel.
 * Waits for any ongoing send operation to be complete or the timeout is reached and resets the send buffer.
 * Waits for any pending read operation to be complete and deletes the buffer. Future read will return 0 bytes.
 * 
 * @param[in] pChannel Pointer to data transfer channel.
 * @return Number of bytes of data returned ( should be <= Number of bytes requested ).
 */
void IotBleDataTransfer_Close( IotBleDataTransferChannel_t * pChannel );


/**
 * @brief Resets a ble data transfer channel.
 * Makes the channel unused by removing the callbacks
 * 
 * @param[in] pChannel Pointer to data transfer channel.
 * 
 * @return Number of bytes of data returned ( should be <= Number of bytes requested ).
 */
void IotBleDataTransfer_Reset( IotBleDataTransferChannel_t * pChannel );

#endif /* IOT_BLE_MQTT_H */
