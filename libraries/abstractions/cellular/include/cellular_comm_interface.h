/*
 * Amazon FreeRTOS CELLULAR Preview Release
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

#ifndef __CELLULAR_COMM_INTERFACE_H__
#define __CELLULAR_COMM_INTERFACE_H__

#include "iot_config.h"
#include "cellular_types.h"

/* Standard includes. */
#include <stdint.h>

/**
 * @brief Return codes from various APIs.
 */
typedef enum CellularCommInterfaceError
{
    IOT_COMM_INTERFACE_SUCCESS = 0,   /**< Function successfully completed. */
    IOT_COMM_INTERFACE_FAILURE,       /**< Generic failure not covered by other values. */
    IOT_COMM_INTERFACE_BAD_PARAMETER, /**< At least one parameter was invalid. */
    IOT_COMM_INTERFACE_NO_MEMORY,     /**< Memory allocation failed. */
    IOT_COMM_INTERFACE_TIMEOUT,       /**< Operation timed out. */
    IOT_COMM_INTERFACE_DRIVER_ERROR,  /**< An error occurred when calling a low level driver API. */
    IOT_COMM_INTERFACE_BUSY           /**< The interface is currently busy. */
} CellularCommInterfaceError_t;

/**
 * @brief Opaque handle to comm interface.
 */
struct CellularCommInterfaceContext;
typedef struct CellularCommInterfaceContext * CellularCommInterfaceHandle_t;

/**
 * @brief Provide an asynchronous notification of incoming data.
 *
 * A function of this signature is supplied in CellularCommInterfaceOpen_t and is
 * used to notify whenever data is available for reading on the comm interface.
 *
 * @param[in] pUserData Userdata to be provided in the callback.
 * @param[in] commInterfaceHandle Handle corresponding to the comm interface.
 *
 * @return IOT_COMM_INTERFACE_SUCCESS if the operation is successful, and need to yield from ISR
 * IOT_COMM_INTERFACE_BUSY if the operation is successful,
 * otherwise an error code indicating the cause of the error.
 */
typedef CellularCommInterfaceError_t ( * CellularCommInterfaceReceiveCallback_t )( void * pUserData,
                                                                                   CellularCommInterfaceHandle_t commInterfaceHandle );

/**
 * @brief Open a connection to the comm interface.
 *
 * @param[in] receiveCallback Callback to be invoked whenever there is data
 * available for reading from the comm interface.
 * @param[in] pUserData Userdata to be provided in the callback.
 * @param[out] pCommInterfaceHandle Out parameter to provide the comm interface
 * handle.
 *
 * @return IOT_COMM_INTERFACE_SUCCESS if the operation is successful, otherwise
 * an error code indicating the cause of the error.
 */
typedef CellularCommInterfaceError_t ( * CellularCommInterfaceOpen_t )( CellularCommInterfaceReceiveCallback_t receiveCallback,
                                                                        void * pUserData,
                                                                        CellularCommInterfaceHandle_t * pCommInterfaceHandle );

/**
 * @brief Send data to the comm interface.
 *
 * @param[in] commInterfaceHandle Comm interface handle as returned from the
 * CellularCommInterfaceOpen_t call.
 * @param[in] pData The data to send.
 * @param[in] dataLength Length of the data to send.
 * @param[in] timeoutMilliseconds Timeout in milliseconds for the send operation.
 * @param[out] pDataSentLength Out parameter to provide the length of the actual
 * data sent. Note that it may be less than the dataLength in case complete data
 * could not be sent.
 *
 * @return IOT_COMM_INTERFACE_SUCCESS if the operation is successful, otherwise
 * an error code indicating the cause of the error.
 */
typedef CellularCommInterfaceError_t ( * CellularCommInterfaceSend_t )( CellularCommInterfaceHandle_t commInterfaceHandle,
                                                                        const uint8_t * pData,
                                                                        uint32_t dataLength,
                                                                        uint32_t timeoutMilliseconds,
                                                                        uint32_t * pDataSentLength );

/**
 * @brief Receive data from the comm interface.
 *
 * @param[in] commInterfaceHandle Comm interface handle as returned from the
 * CellularCommInterfaceOpen_t call.
 * @param[in] pBuffer The buffer to receive the data into.
 * @param[in] bufferLength The length of the buffer pBuffer.
 * @param[in] timeoutMilliseconds Timeout in milliseconds for the receive
 * operation.
 * @param[out] pDataReceivedLength Out parameter to provide the length of the
 * actual data received in the buffer pBuffer. Note that it may be less than
 * the bufferLength.
 *
 * @return IOT_COMM_INTERFACE_SUCCESS if the operation is successful, otherwise
 * an error code indicating the cause of the error.
 */
typedef CellularCommInterfaceError_t ( * CellularCommInterfaceRecv_t )( CellularCommInterfaceHandle_t commInterfaceHandle,
                                                                        uint8_t * pBuffer,
                                                                        uint32_t bufferLength,
                                                                        uint32_t timeoutMilliseconds,
                                                                        uint32_t * pDataReceivedLength );

/**
 * @brief Close the connection to the comm interface.
 *
 * @param[in] commInterfaceHandle Comm interface handle as returned from the
 * CellularCommInterfaceOpen_t call.
 *
 * @return IOT_COMM_INTERFACE_SUCCESS if the operation is successful, otherwise
 * an error code indicating the cause of the error.
 */
typedef CellularCommInterfaceError_t ( * CellularCommInterfaceClose_t )( CellularCommInterfaceHandle_t commInterfaceHandle );

/**
 * @brief Represents the functions of a comm interface.
 *
 * Functions of these signature should be implemented against a comm interface
 * (like UART, SPI etc.).
 */
typedef struct CellularCommInterface
{
    CellularCommInterfaceOpen_t open;
    CellularCommInterfaceSend_t send;
    CellularCommInterfaceRecv_t recv;
    CellularCommInterfaceClose_t close;
} CellularCommInterface_t;

#endif /* __CELLULAR_COMM_INTERFACE_H__ */
