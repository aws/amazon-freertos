/*
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 */

/**
 * @file iot_network_udp.h
 * @brief Declares the network stack functions specified in platform/iot_network.h for
 * Amazon FreeRTOS UDP Sockets.
 */

#ifndef _IOT_NETWORK_UDP_H_
#define _IOT_NETWORK_UDP_H_

/* Standard includes. */
#include <stdbool.h>

/* Platform network include. */
#include "platform/iot_network.h"

/* Amazon FreeRTOS Secure Sockets include. */
#include "iot_secure_sockets.h"

/**
 * @brief Represents a network connection that uses Amazon FreeRTOS UDP Sockets.
 *
 * This is an incomplete type. In application code, only pointers to this type
 * should be used.
 */
typedef struct _networkConnection IotNetworkConnectionUdp_t;

/**
 * @brief Information on the remote server for connection setup.
 *
 * Passed to #IotNetworkUdp_Create as `pConnectionInfo`.
 *
 * All instances of #IotNetworkServerInfoUdp_t should be initialized with
 * #IOT_NETWORK_SERVER_INFO_UDP_INITIALIZER.
 * @Note If noBind is set to true, underlying stack must support bind in send
 */
typedef struct IotNetworkServerInfoUdp
{
    const char * pHostName;     /**< @brief Server host name. Must be NULL-terminated. */
    uint16_t port;              /**< @brief Server port in host-order. */
    uint32_t socketTimeout;     /**< @brief Socket timeout in Millisecods. */
    bool strictServerAddrCheck; /**< @brief Verify server address matches receive address. */
    bool noBind;                /**< @brief Do not bind socket. */
} IotNetworkServerInfoUdp_t;

/**
 * @brief Provides a default value for an #IotNetworkConnectionUdp_t.
 *
 * All instances of #IotNetworkConnectionUdp_t should be initialized with
 * this constant.
 *
 * @warning Failing to initialize an #IotNetworkConnectionUdp_t with this
 * initializer may result in undefined behavior!
 * @note This initializer may change at any time in future versions, but its
 * name will remain the same.
 */
#define IOT_NETWORK_CONNECTION_UDP_INITIALIZER     { 0 }

/**
 * @brief Provides a initializer value for networkConnection pointer.
 *
 * All instances of #IotNetworkConnectionUdp_t pointer should be initialized with
 * this constant.
 *
 * @warning Failing to initialize an #IotNetworkConnectionUdp_t with this
 * initializer may result in undefined behavior!
 * @note This initializer may change at any time in future versions, but its
 * name will remain the same.
 */
#define IOT_NETWORK_INVALID_CONNECTION     ( ( IotNetworkConnectionUdp_t* ) NULL )

/**
 * @brief Generic initializer for an #IotNetworkServerInfoUdp_t.
 *
 * @note This initializer may change at any time in future versions, but its
 * name will remain the same.
 */
#define IOT_NETWORK_SERVER_INFO_UDP_INITIALIZER    { 0 }

/**
 * @brief Provides a pointer to an #IotNetworkInterface_t that uses the functions
 * declared in this file.
 */
#define IOT_NETWORK_INTERFACE_UDP    ( &( IotNetworkUdp ) )

/**
 * @brief An implementation of #IotNetworkInterface_t::create for Amazon FreeRTOS
 * UDP Sockets.
 */
IotNetworkError_t IotNetworkUdp_Create( void * pConnectionInfo,
                                        void * pCredentialInfo,
                                        void ** const pConnection );

/**
 * @brief An implementation of #IotNetworkInterface_t::setReceiveCallback for
 * Amazon FreeRTOS UDP Sockets.
 */
IotNetworkError_t IotNetworkUdp_SetReceiveCallback( void * pConnection,
                                                    IotNetworkReceiveCallback_t receiveCallback,
                                                    void * pContext );

/**
 * @brief An implementation of #IotNetworkInterface_t::send for Amazon FreeRTOS
 * UDP Sockets.
 */
size_t IotNetworkUdp_SendTo( void * pConnection,
                           const uint8_t * pMessage,
                           size_t messageLength );

/**
 * @brief An implementation of #IotNetworkInterface_t::receive for Amazon FreeRTOS
 * UDP Sockets.
 */
size_t IotNetworkUdp_ReceiveFrom( void * pConnection,
                              uint8_t * pBuffer,
                              size_t bytesRequested );

/**
 * @brief An implementation of #IotNetworkInterface_t::close for Amazon FreeRTOS
 * UDP Sockets.
 */
IotNetworkError_t IotNetworkUdp_Close( void * pConnection );

/**
 * @brief An implementation of #IotNetworkInterface_t::destroy for Amazon FreeRTOS
 * UDP Sockets.
 */
IotNetworkError_t IotNetworkUdp_Destroy( void * pConnection );

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Declaration of a network interface struct using the functions in this file.
 */
extern const IotNetworkInterface_t IotNetworkUdp;
/** @endcond */
#endif