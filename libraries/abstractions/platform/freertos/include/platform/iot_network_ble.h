/*
 * FreeRTOS Platform V1.1.1
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
 * @file iot_network_BLE.h
 * @brief Declares the network stack functions specified in aws_iot_network.h for
 * Bluetooth Low Energy.
 */

#ifndef _IOT_NETWORK_BLE_H_
#define _IOT_NETWORK_BLE_H_

/* Standard includes. */
#include <stdbool.h>
#include "platform/iot_network.h"

/**
 * @brief Provides a pointer to an #IotNetworkInterface_t that uses the functions
 * declared in this file.
 */
#define IOT_NETWORK_INTERFACE_BLE    ( &( IotNetworkBle ) )

/**
 * @brief An implementation of #IotNetworkInterface_t::create for FreeRTOS
 * Secure Sockets.
 */
IotNetworkError_t IotNetworkBle_Create( void * pConnectionInfo,
                                        void * pCredentialInfo,
                                        void ** pConnection );

/**
 * @brief An implementation of #IotNetworkInterface_t::setReceiveCallback for
 * FreeRTOS Secure Sockets.
 */
IotNetworkError_t IotNetworkBle_SetReceiveCallback( void * pConnection,
                                                    IotNetworkReceiveCallback_t receiveCallback,
                                                    void * pContext );

/**
 * @brief An implementation of #IotNetworkInterface_t::send for FreeRTOS
 * Secure Sockets.
 */
size_t IotNetworkBle_Send( void * pConnection,
                           const uint8_t * pMessage,
                           size_t messageLength );

/**
 * @brief An implementation of #IotNetworkInterface_t::receive for FreeRTOS
 * Secure Sockets.
 */
size_t IotNetworkBle_Receive( void * pConnection,
                              uint8_t * pBuffer,
                              size_t bytesRequested );

/**
 * @brief An implementation of #IotNetworkInterface_t::close for FreeRTOS
 * Secure Sockets.
 */
IotNetworkError_t IotNetworkBle_Close( void * pConnection );

/**
 * @brief An implementation of #IotNetworkInterface_t::destroy for FreeRTOS
 * Secure Sockets.
 */
IotNetworkError_t IotNetworkBle_Destroy( void * pConnection );

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Declaration of a network interface struct using the functions in this file.
 */
extern const IotNetworkInterface_t IotNetworkBle;
/** @endcond */

#endif /* ifndef _IOT_NETWORK_BLE_H_ */
