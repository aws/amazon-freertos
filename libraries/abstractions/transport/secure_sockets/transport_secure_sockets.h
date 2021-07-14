/*
 * FreeRTOS Transport Secure Sockets V1.0.1
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file transport_secure_sockets.h
 * @brief Secure Sockets based API for transport interface implementation.
 */

#ifndef TRANSPORT_SECURE_SOCKETS_H
#define TRANSPORT_SECURE_SOCKETS_H

/* bool is defined in only C99+. */
#if defined( __cplusplus ) || ( defined( __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901L ) )
    #include <stdbool.h>
#elif !defined( bool ) && !defined( false ) && !defined( true )
    #define bool     int8_t
    #define false    ( int8_t ) 0
    #define true     ( int8_t ) 1
#endif
/** @endcond */

/* Transport interface include. */
#include "transport_interface.h"
#include "iot_secure_sockets.h"

/* Kernel include. */
#include "FreeRTOS.h"
#include "task.h"

/* Include header that defines log levels. */
#include "logging_levels.h"

/* Logging configuration for the transport interface implementation which uses
 * Secure Sockets. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME     "Transport_Secure_Sockets"
#endif
#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_ERROR
#endif

/* Logging implementation header include. */
#include "logging_stack.h"

/**
 * @brief Definition of the network context for the transport interface
 * implementation that uses Secure Sockets API.
 */
typedef struct SecureSocketsTransportParams
{
    Socket_t tcpSocket;
} SecureSocketsTransportParams_t;

/**
 * @brief TCP, TLS Connect / Disconnect return status.
 */
typedef enum TransportSocketStatus
{
    TRANSPORT_SOCKET_STATUS_SUCCESS = 0,         /**< Function successfully completed. */
    TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER,   /**< At least one parameter was invalid. */
    TRANSPORT_SOCKET_STATUS_INSUFFICIENT_MEMORY, /**< Insufficient memory required to establish connection. */
    TRANSPORT_SOCKET_STATUS_CREDENTIALS_INVALID, /**< Provided credentials were invalid. */
    TRANSPORT_SOCKET_STATUS_INTERNAL_ERROR,      /**< A call to a system API resulted in an internal error. */
    TRANSPORT_SOCKET_STATUS_DNS_FAILURE,         /**< Resolving hostname of the server failed. */
    TRANSPORT_SOCKET_STATUS_CONNECT_FAILURE      /**< Initial connection to the server failed. */
} TransportSocketStatus_t;


/**
 * @brief Information on the remote server for connection setup.
 */
typedef struct ServerInfo
{
    const char * pHostName; /**< @brief Server host name. Must be NULL-terminated. */
    size_t hostNameLength;  /**< @brief Length of the server host name. */
    uint16_t port;          /**< @brief Server port in host-order. */
} ServerInfo_t;


/**
 * @brief Contains the credentials necessary for connection setup.
 *
 */
typedef struct SocketsConfig
{
    bool enableTls;         /**< @brief Whether require TLS for the transport. */
    uint32_t sendTimeoutMs; /**< @brief Timeout for transport send. */
    uint32_t recvTimeoutMs; /**< @brief Timeout for transport recv. */

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

    /**
     * @brief Set this to a non-zero value to use TLS max fragment length
     * negotiation (TLS MFLN).
     *
     * @note The network stack may have a minimum value for this parameter and
     * may return an error if this parameter is too small.
     */
    size_t maxFragmentLength;

    const char * pRootCa; /**< @brief String representing a trusted server Root CA certificate. */
    size_t rootCaSize;    /**< @brief Size associated with #IotNetworkCredentials_t.pRootCa. */
} SocketsConfig_t;


/**
 * @brief Sets up a TCP only connection or a TLS session on top of a TCP connection with Secure Sockets API.
 *
 * @param[out] pNetworkContext The output parameter to return the created network context.
 * @param[in] pServerInfo Server connection info.
 * @param[in] pSocketsConfig socket configs for the connection.
 *
 * @return #TRANSPORT_SOCKET_STATUS_SUCCESS on success;
 *         #TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER, #TRANSPORT_SOCKET_STATUS_INSUFFICIENT_MEMORY,
 *         #TRANSPORT_SOCKET_STATUS_CREDENTIALS_INVALID, #TRANSPORT_SOCKET_STATUS_INTERNAL_ERROR,
 *         #TRANSPORT_SOCKET_STATUS_DNS_FAILURE, #TRANSPORT_SOCKET_STATUS_CONNECT_FAILURE on failure.
 */
TransportSocketStatus_t SecureSocketsTransport_Connect( NetworkContext_t * pNetworkContext,
                                                        const ServerInfo_t * pServerInfo,
                                                        const SocketsConfig_t * pSocketsConfig );

/**
 * @brief Closes a TLS session on top of a TCP connection using the Secure Sockets API.
 *
 * @param[out] pNetworkContext The output parameter to end the TLS session and
 *             clean the created network context.
 *
 * @return #TRANSPORT_SOCKET_STATUS_SUCCESS on success;
 *         #TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER, #TRANSPORT_SOCKET_STATUS_INTERNAL_ERROR on failure.
 */
TransportSocketStatus_t SecureSocketsTransport_Disconnect( const NetworkContext_t * pNetworkContext );


/**
 * @brief Receives data over an established TLS session using the Secure Sockets API.
 *
 * This can be used as #TransportInterface.recv function for receiving data
 * from the network.
 *
 * @param[in] pNetworkContext The network context created using Socket_Connect API.
 * @param[out] pBuffer Buffer to receive network data into.
 * @param[in] bytesToRecv Number of bytes requested from the network.
 *
 * @return Number of bytes (> 0) received if successful;
 *         0 if the socket times out without reading any bytes;
 *         negative value on error.
 */
int32_t SecureSocketsTransport_Recv( NetworkContext_t * pNetworkContext,
                                     void * pBuffer,
                                     size_t bytesToRecv );

/**
 * @brief Sends data over an established TLS session using the Secure Sockets API.
 *
 * This can be used as the #TransportInterface.send function to send data
 * over the network.
 *
 * @param[in] pNetworkContext The network context created using Secure Sockets API.
 * @param[in] pBuffer Buffer containing the bytes to send over the network stack.
 * @param[in] bytesToSend Number of bytes to send over the network.
 *
 * @return Number of bytes sent if successful; negative value on error.
 */
int32_t SecureSocketsTransport_Send( NetworkContext_t * pNetworkContext,
                                     const void * pMessage,
                                     size_t bytesToSend );

#endif /* TRANSPORT_SECURE_SOCKETS_H */
