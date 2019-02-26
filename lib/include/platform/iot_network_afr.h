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
 * @file iot_network_afr.h
 * @brief Declares the network stack functions specified in aws_iot_network.h for
 * Amazon FreeRTOS Secure Sockets.
 */

#ifndef _IOT_NETWORK_AFR_H_
#define _IOT_NETWORK_AFR_H_

/* Standard includes. */
#include <stdbool.h>

/* Platform network include. */
#include "platform/iot_network.h"

/* Amazon FreeRTOS Secure Sockets include. */
#include "aws_secure_sockets.h"

/* Credentials include. */
#include "aws_clientcredential.h"

/**
 * @brief Represents a network connection that uses Amazon FreeRTOS Secure Sockets.
 *
 * All instances of #IotNetworkConnectionAfr_t should be initialized with
 * #IOT_NETWORK_CONNECTION_AFR_INITIALIZER.
 *
 * @attention The members of this struct are intended to be opaque and may change
 * at any time. This struct should only be passed as the connection handle to the
 * functions declared in this file. Do not directly modify its members!
 */
typedef struct IotNetworkConnectionAfr
{
    Socket_t socket;                             /**< @brief Amazon FreeRTOS Secure Sockets handle. */
    StaticSemaphore_t socketMutex;               /**< @brief Prevents concurrent threads from sending on a socket. */
    StaticEventGroup_t connectionFlags;          /**< @brief Synchronizes with the receive task. */

    uint8_t * pReceiveBuffer;                    /**< @brief Buffer for incoming data. */
    IotNetworkReceiveCallback_t receiveCallback; /**< @brief Network receive callback, if any. */
    void * pReceiveContext;                      /**< @brief The context for the receive callback. */
} IotNetworkConnectionAfr_t;

/**
 * @brief Information on the remote server for connection setup.
 *
 * Passed to #IotNetworkAfr_Create as `pConnectionInfo`.
 *
 * All instances of #IotNetworkServerInfoAfr_t should be initialized with
 * #IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER or #AWS_IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER.
 */
typedef struct IotNetworkServerInfoAfr
{
    const char * pHostName; /**< @brief Server host name. Must be NULL-terminated. */
    uint16_t port;          /**< @brief Server port in host-order. */
} IotNetworkServerInfoAfr_t;

/**
 * @brief Contains the credentials necessary for connection setup with Amazon
 * FreeRTOS Secure Sockets.
 *
 * Passed to #IotNetworkAfr_Create as `pCredentialInfo`.
 *
 * All instances of #IotNetworkCredentialsAfr_t should be initialized with either
 * #IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER or #AWS_IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER.
 */
typedef struct IotNetworkCredentialsAfr
{
    /**
     * @brief Set this to a non-NULL value to use ALPN.
     *
     * This string must be NULL-terminated.
     *
     * See [this link]
     * (https://aws.amazon.com/blogs/iot/mqtt-with-tls-client-authentication-on-port-443-why-it-is-useful-and-how-it-works/)
     * for more information.
     */
    const char * pAlpnProtos;

    /**
     * @brief Disable server name indication (SNI) for a TLS session.
     */
    bool disableSni;

    /* PEM-encoded credentials. */
    const char * pRootCa;     /**< @brief PEM-encoded root certicate, `NULL` to use system defaults. */
    const char * pClientCert; /**< @brief PEM-encoded client certificate. */
    const char * pPrivateKey; /**< @brief PEM-encoded client certicate private key. */

    /* Sizes of credentials. */
    size_t rootCaSize;     /**< @brief Size of root certicate. Ignored if `pRootCa` is `NULL`. */
    size_t clientCertSize; /**< @brief Size of `pClientCert`. */
    size_t privateKeySize; /**< @brief Size of `pPrivateKey`. */
} IotNetworkCredentialsAfr_t;

/**
 * @brief Provides a default value for an #IotNetworkConnectionAfr_t.
 *
 * All instances of #IotNetworkConnectionAfr_t should be initialized with
 * this constant.
 *
 * @warning Failing to initialize an #IotNetworkConnectionAfr_t with this
 * initializer may result in undefined behavior!
 * @note This initializer may change at any time in future versions, but its
 * name will remain the same.
 */
#define IOT_NETWORK_CONNECTION_AFR_INITIALIZER     { 0 }

/**
 * @brief Generic initializer for an #IotNetworkServerInfoAfr_t.
 *
 * @note This initializer may change at any time in future versions, but its
 * name will remain the same.
 */
#define IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER    { 0 }

/**
 * @brief Initialize an #IotNetworkServerInfoAfr_t for use with AWS IoT.
 *
 * @note This initializer may change at any time in future versions, but its
 * name will remain the same.
 */
#define AWS_IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER        \
    {                                                      \
        .pHostName = clientcredentialMQTT_BROKER_ENDPOINT, \
        .port = clientcredentialMQTT_BROKER_PORT           \
    }

/**
 * @brief Generic initializer for an #IotNetworkCredentialsAfr_t.
 *
 * @note This initializer may change at any time in future versions, but its
 * name will remain the same.
 */
#define IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER    { 0 }

/**
 * @brief Initialize an #IotNetworkCredentialsAfr_t for use with AWS IoT.
 *
 * @note This initializer may change at any time in future versions, but its
 * name will remain the same.
 */
#define AWS_IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER                             \
    {                                                                           \
        .disableSni = false,                                                    \
        .pAlpnProtos = socketsAWS_IOT_ALPN_MQTT,                                \
        .pRootCa = NULL,                                                        \
        .pClientCert = clientcredentialCLIENT_CERTIFICATE_PEM,                  \
        .pPrivateKey = clientcredentialCLIENT_PRIVATE_KEY_PEM,                  \
        .rootCaSize = 0,                                                        \
        .clientCertSize = ( size_t ) clientcredentialCLIENT_CERTIFICATE_LENGTH, \
        .privateKeySize = ( size_t ) clientcredentialCLIENT_PRIVATE_KEY_LENGTH  \
    }

/**
 * @brief Provides a pointer to an #IotNetworkInterface_t that uses the functions
 * declared in this file.
 */
#define IOT_NETWORK_INTERFACE_AFR    ( &( _IotNetworkAfr ) )

/**
 * @brief An implementation of #IotNetworkInterface_t::create for Amazon FreeRTOS
 * Secure Sockets.
 */
IotNetworkError_t IotNetworkAfr_Create( void * pConnectionInfo,
                                        void * pCredentialInfo,
                                        void * const pConnection );

/**
 * @brief An implementation of #IotNetworkInterface_t::setReceiveCallback for
 * Amazon FreeRTOS Secure Sockets.
 */
IotNetworkError_t IotNetworkAfr_SetReceiveCallback( void * pConnection,
                                                    IotNetworkReceiveCallback_t receiveCallback,
                                                    void * pContext );

/**
 * @brief An implementation of #IotNetworkInterface_t::send for Amazon FreeRTOS
 * Secure Sockets.
 */
size_t IotNetworkAfr_Send( void * pConnection,
                           const uint8_t * pMessage,
                           size_t messageLength );

/**
 * @brief An implementation of #IotNetworkInterface_t::receive for Amazon FreeRTOS
 * Secure Sockets.
 */
size_t IotNetworkAfr_Receive( void * pConnection,
                              uint8_t * pBuffer,
                              size_t bytesRequested );

/**
 * @brief An implementation of #IotNetworkInterface_t::close for Amazon FreeRTOS
 * Secure Sockets.
 */
IotNetworkError_t IotNetworkAfr_Close( void * pConnection );

/**
 * @brief An implementation of #IotNetworkInterface_t::destroy for Amazon FreeRTOS
 * Secure Sockets.
 */
IotNetworkError_t IotNetworkAfr_Destroy( void * pConnection );

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Declaration of a network interface struct using the functions in this file.
 */
extern const IotNetworkInterface_t _IotNetworkAfr;
/** @endcond */

#endif /* ifndef _IOT_NETWORK_AFR_H_ */
