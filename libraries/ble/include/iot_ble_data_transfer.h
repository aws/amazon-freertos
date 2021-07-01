/*
 * FreeRTOS BLE V2.2.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_iot_data_transfer.h
 * @brief Header file contains the API for a generic BLE data transfer channel for sending/receiving data over BLE.
 *        APIs could be implemented using GATT, L2CAP etc..
 */

#ifndef IOT_BLE_DATA_TRANSFER_H
#define IOT_BLE_DATA_TRANSFER_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
 * @brief Events related to a ble data transfer channel.
 */
typedef enum IotBleDataTransferChannelEvent
{
    IOT_BLE_DATA_TRANSFER_CHANNEL_OPENED = 0,    /**< Indicates if the channel is opened and ready to read or write data. */
    IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVED, /**< Event invoked when the last chunk of a large object or a small packet is received on the channel. */
    IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_SENT,     /**< Event invoked after the last chunk of data stream is sent over the channel. */
    IOT_BLE_DATA_TRANSFER_CHANNEL_CLOSED         /**< Event invoked when the channel is closed. */
} IotBleDataTransferChannelEvent_t;

/**
 * @brief Forward declaration of Data transfer channel structure.
 */
typedef struct IotBleDataTransferChannel IotBleDataTransferChannel_t;

/**
 * @brief Callback invoked whenever an event occurs on the data transfer channel.
 */
typedef void ( * IotBleDataTransferChannelCallback_t ) ( IotBleDataTransferChannelEvent_t event,
                                                         IotBleDataTransferChannel_t * pChannel,
                                                         void * pContext );

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
 * @return True if callback is set successfully, false if callback is already set or channel is not opened.
 */
bool IotBleDataTransfer_SetCallback( IotBleDataTransferChannel_t * pChannel,
                                     const IotBleDataTransferChannelCallback_t callback,
                                     void * pContext );

/**
 * @brief Sent data over a ble data transfer channel.
 *
 * @param[in] pChannel Pointer to data transfer channel.
 * @param[in] pMessage Pointer to the message to be sent.
 * @param[in] messageLength Length in bytes of the message to be sent.
 *
 * @return Number of bytes of message actually sent.
 */
size_t IotBleDataTransfer_Send( IotBleDataTransferChannel_t * pChannel,
                                const uint8_t * const pMessage,
                                size_t messageLength );

/**
 * @brief Function copies the requested bytes of data from the receive buffer to the user provided buffer.
 * This should always be called in the context of a IotBleDataTransferChannelCallback_t IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVED event.
 *
 * @param[in] pChannel Pointer to data transfer channel.
 * @param[in] pBuffer Pointer to the buffer where the data will be copied. Pass NULL to do an empty read to flush the data.
 * @param[in] bytesRequested Number of bytes of data requested.
 *
 * @return Number of bytes of data returned. Should be less than or equal to the number of bytes requested.
 */
size_t IotBleDataTransfer_Receive( IotBleDataTransferChannel_t * pChannel,
                                   uint8_t * pBuffer,
                                   size_t bytesRequested );

/**
 * @brief Returns a pointer to the received buffer and length of the received data.
 * Function should always be called in the context of a IotBleDataTransferChannelCallback_t IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVED event.
 *
 * @param[in] pChannel Channel on which the callback is fired.
 * @param[out] pBuffer Pointer to the received buffer.
 * @param[out] Length of the received buffer.
 *
 */
void IotBleDataTransfer_PeekReceiveBuffer( IotBleDataTransferChannel_t * pChannel,
                                           const uint8_t ** pBuffer,
                                           size_t * pBufferLength );

/**
 * @brief Close a ble data transfer channel.
 * Waits for any ongoing send operation to be complete or the timeout is reached and resets the send buffer.
 * Waits for any pending read operation to be complete and deletes the buffer. Future read will return 0 bytes.
 *
 * @param[in] pChannel Pointer to data transfer channel.
 */
void IotBleDataTransfer_Close( IotBleDataTransferChannel_t * pChannel );


/**
 * @brief Resets a ble data transfer channel.
 * Makes the channel free to use and removes any callbacks assigned.
 *
 * @param[in] pChannel Pointer to data transfer channel.
 *
 * @return Number of bytes of data returned ( should be <= Number of bytes requested ).
 */
void IotBleDataTransfer_Reset( IotBleDataTransferChannel_t * pChannel );


/**
 * @brief Cleanup the BLE data transfer services.
 *
 * @return true/false if the cleanup was successful or not.
 */
bool IotBleDataTransfer_Cleanup( void );

#endif /* IOT_BLE_DATA_TRANSFER_H */
