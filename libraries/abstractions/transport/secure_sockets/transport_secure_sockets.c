/*
 * FreeRTOS V202002.00
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
 * @file transport_secure_sockets.c
 * @brief Implementation of transport interface and connect/disconnect API with Secure Sockets.
 */

/* Standard includes. */
#include <stdlib.h>
#include <string.h>

/* TCP/IP abstraction includes. */
#include "iot_secure_sockets.h"
#include "transport_secure_sockets.h"


/*-----------------------------------------------------------*/

/**
 * @brief Set up the TCP context send and receive timeouts.
 *
 * @param[in] tcpSocket The context to be set.
 * @param[in] sendTimeoutMs Timeout for transport send.
 * @param[in] recvTimeoutMs Timeout for transport recv.
 *
 * @return #SOCKETS_ERROR_NONE or error code from Secure Sockets.
 */
static int32_t transportTimeoutSetup( Socket_t tcpSocket,
                                      uint32_t sendTimeoutMs,
                                      uint32_t recvTimeoutMs );

/**
 * @brief Sets up a TCP only connection or a TLS session on top of a TCP connection with Secure Sockets API.
 *
 * @param[out] pNetworkContext The output parameter to return the created network context.
 * @param[in] pServerInfo Server connection info.
 * @param[in] pSocketsConfig socket configs for the connection.
 *
 * @return #TRANSPORT_SOCKET_STATUS_SUCCESS on success;
 *         #TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER, #TRANSPORT_SOCKET_STATUS_INSUFFICIENT_MEMORY,
 *         #TRANSPORT_SOCKET_STATUS_INVALID_CREDENTIALS, #TRANSPORT_SOCKET_STATUS_INTERNAL_ERROR,
 *         #TRANSPORT_SOCKET_STATUS_DNS_FAILURE, #TRANSPORT_SOCKET_STATUS_CONNECT_FAILURE on failure.
 */
static TransportSocketStatus_t establishConnect( NetworkContext_t * pNetworkContext,
                                                 const ServerInfo_t * pServerInfo,
                                                 const SocketsConfig_t * pSocketsConfig );

/**
 * @brief Set up TLS configurations for the socket.
 *
 * @param[in] pSocketsConfig The session configuration.
 * @param[in] tcpSocket An initialized socket to secure.
 * @param[in] pHostName Remote server name for SNI.
 * @param[in] hostnameLength The length of `pHostName`.
 *
 * @return #SOCKETS_ERROR_NONE or #SOCKETS_SOCKET_ERROR.
 */
static int32_t tlsSetup( const SocketsConfig_t * pSocketsConfig,
                         Socket_t tcpSocket,
                         const char * pHostName,
                         size_t hostnameLength );


/*-----------------------------------------------------------*/

int32_t SecureSocketsTransport_Send( const NetworkContext_t * pNetworkContext,
                                     const void * pMessage,
                                     size_t bytesToSend )
{
    int32_t bytesSent = 0;
    int32_t transportSocketStatus = SOCKETS_ERROR_NONE;

    if( ( pMessage == NULL ) ||
        ( bytesToSend == 0UL ) ||
        ( pNetworkContext == NULL ) )
    {
        LogError( ( "Invalid parameter: pMessage=%p, bytesToSend=%lu, pNetworkContext=%p",
                    pMessage, bytesToSend, pNetworkContext ) );
        bytesSent = SOCKETS_EINVAL;
    }
    else if( ( pNetworkContext != NULL ) && ( pNetworkContext->pContext == NULL ) )
    {
        LogError( ( "Invalid parameter: pNetworkContext->pContext cannot be NULL." ) );
        bytesSent = SOCKETS_EINVAL;
    }
    else
    {
        transportSocketStatus = SOCKETS_Send( ( Socket_t ) pNetworkContext->pContext,
                                              pMessage,
                                              bytesToSend,
                                              0 );

        /* If an error occurred, a negative value is returned. @ref SocketsErrors. */
        if( transportSocketStatus >= 0 )
        {
            bytesSent = transportSocketStatus;
            LogInfo( ( "Successfully sent %ld bytes over network.", transportSocketStatus ) );
        }
        else
        {
            LogError( ( "Failed to send data over network. BytesRequested=%ld.", transportSocketStatus ) );
            bytesSent = SOCKETS_SOCKET_ERROR;
        }
    }

    return bytesSent;
}

/*-----------------------------------------------------------*/

int32_t SecureSocketsTransport_Recv( const NetworkContext_t * pNetworkContext,
                                     void * pBuffer,
                                     size_t bytesToRecv )
{
    int32_t transportSocketStatus = 0;
    int32_t bytesReceived = SOCKETS_SOCKET_ERROR;
    uint8_t * pRecvBuffer = ( uint8_t * ) pBuffer;

    if( ( pBuffer == NULL ) ||
        ( bytesToRecv == 0UL ) ||
        ( pNetworkContext == NULL ) )
    {
        LogError( ( "Invalid parameter: pBuffer=%p, bytesToRecv=%lu, pNetworkContext=%p",
                    pBuffer, bytesToRecv, pNetworkContext ) );
        bytesReceived = SOCKETS_EINVAL;
    }
    else if( ( pNetworkContext != NULL ) && ( pNetworkContext->pContext == NULL ) )
    {
        LogError( ( "Invalid parameter: pNetworkContext->pContext cannot be NULL." ) );
        bytesReceived = SOCKETS_EINVAL;
    }
    else
    {
        transportSocketStatus = SOCKETS_Recv( ( Socket_t ) pNetworkContext->pContext,
                                              pRecvBuffer,
                                              bytesToRecv,
                                              0 );

        if( transportSocketStatus == SOCKETS_EWOULDBLOCK )
        {
            /* The return value EWOULDBLOCK means no data was received within
             * the receive timeout. */
            bytesReceived = 0;
        }
        else if( transportSocketStatus < 0 )
        {
            LogError( ( "Failed to receive data over network. SocketsStatus=%d", transportSocketStatus ) );
        }
        else
        {
            bytesReceived = transportSocketStatus;
        }

        if( bytesReceived < bytesToRecv )
        {
            LogInfo( ( "Receive requested %lu bytes, but %lu bytes received instead.",
                       bytesToRecv,
                       bytesReceived ) );
        }
        else
        {
            LogInfo( ( "Successfully received %lu bytes.",
                       bytesReceived ) );
        }
    }

    return bytesReceived;
}

/*-----------------------------------------------------------*/

static int32_t tlsSetup( const SocketsConfig_t * pSocketsConfig,
                         Socket_t tcpSocket,
                         const char * pHostName,
                         size_t hostnameLength )
{
    int32_t transportSocketStatus = SOCKETS_ERROR_NONE;

    /* ALPN options for AWS IoT. */
    /* ppcALPNProtos is unused. putting here to align behavior in IotNetworkAfr_Create. */
    /* coverity[misra_c_2012_rule_4_6_violation] */
    /* coverity[misra_c_2012_rule_8_13_violation] */
    const char * ppcALPNProtos[] = { socketsAWS_IOT_ALPN_MQTT };

    /* Set secured option. */
    transportSocketStatus = SOCKETS_SetSockOpt( tcpSocket,
                                                0,
                                                SOCKETS_SO_REQUIRE_TLS,
                                                NULL,
                                                0 );

    if( transportSocketStatus != ( int32_t ) SOCKETS_ERROR_NONE )
    {
        LogError( ( "Failed to set secured option for socket." ) );
    }

    /* Set ALPN option. */
    if( ( transportSocketStatus == SOCKETS_ERROR_NONE ) && ( pSocketsConfig->pAlpnProtos != NULL ) )
    {
        transportSocketStatus = SOCKETS_SetSockOpt( tcpSocket,
                                                    0,
                                                    SOCKETS_SO_ALPN_PROTOCOLS,
                                                    ppcALPNProtos,
                                                    sizeof( ppcALPNProtos ) / sizeof( ppcALPNProtos[ 0 ] ) );

        if( transportSocketStatus != ( int32_t ) SOCKETS_ERROR_NONE )
        {
            LogError( ( "Failed to set ALPN option socket." ) );
        }
    }

    /* Set SNI option. */
    if( ( transportSocketStatus == SOCKETS_ERROR_NONE ) && ( pSocketsConfig->disableSni == false ) )
    {
        transportSocketStatus = SOCKETS_SetSockOpt( tcpSocket,
                                                    0,
                                                    SOCKETS_SO_SERVER_NAME_INDICATION,
                                                    pHostName,
                                                    hostnameLength + 1 );

        if( transportSocketStatus != ( int32_t ) SOCKETS_ERROR_NONE )
        {
            LogError( ( "Failed to set SNI option for socket. transportSocketStatus=%d, HostNameLength=%lu",
                        transportSocketStatus, ( hostnameLength + 1UL ) ) );
        }
    }

    /* Set custom server certificate. */
    if( ( transportSocketStatus == SOCKETS_ERROR_NONE ) && ( pSocketsConfig->pRootCa != NULL ) )
    {
        transportSocketStatus = SOCKETS_SetSockOpt( tcpSocket,
                                                    0,
                                                    SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE,
                                                    pSocketsConfig->pRootCa,
                                                    pSocketsConfig->rootCaSize );

        if( transportSocketStatus != ( int32_t ) SOCKETS_ERROR_NONE )
        {
            LogError( ( "Failed to set server certificate option for socket." ) );
        }
    }

    return transportSocketStatus;
}

/*-----------------------------------------------------------*/

static int32_t transportTimeoutSetup( Socket_t tcpSocket,
                                      uint32_t sendTimeoutMs,
                                      uint32_t recvTimeoutMs )
{
    TickType_t receiveTimeout = 0, sendTimeout = 0;
    int32_t transportSocketStatus = ( int32_t ) SOCKETS_ERROR_NONE;

    if( tcpSocket == NULL )
    {
        transportSocketStatus = SOCKETS_EINVAL;
    }

    if( transportSocketStatus == SOCKETS_ERROR_NONE )
    {
        /* Secure Sockets uses TickType_t therefore replace timeout vale with portMAX_DELAY if it exceeds. */
        receiveTimeout = pdMS_TO_TICKS( recvTimeoutMs );

        if( receiveTimeout > portMAX_DELAY )
        {
            receiveTimeout = portMAX_DELAY;
        }

        transportSocketStatus = SOCKETS_SetSockOpt( tcpSocket,
                                                    0,
                                                    SOCKETS_SO_RCVTIMEO,
                                                    &receiveTimeout,
                                                    sizeof( TickType_t ) );

        if( transportSocketStatus != ( int32_t ) SOCKETS_ERROR_NONE )
        {
            LogError( ( "Failed to set socket receive timeout. SocketStatus=%d.", transportSocketStatus ) );
        }
    }

    if( transportSocketStatus == SOCKETS_ERROR_NONE )
    {
        /* Secure Sockets uses TickType_t therefore replace timeout vale with portMAX_DELAY if it exceeds. */
        if( pdMS_TO_TICKS( sendTimeoutMs ) > portMAX_DELAY )
        {
            sendTimeout = portMAX_DELAY;
        }
        else
        {
            sendTimeout = pdMS_TO_TICKS( sendTimeoutMs );
        }

        transportSocketStatus = SOCKETS_SetSockOpt( tcpSocket,
                                                    0,
                                                    SOCKETS_SO_SNDTIMEO,
                                                    &sendTimeout,
                                                    sizeof( TickType_t ) );

        if( transportSocketStatus != ( int32_t ) SOCKETS_ERROR_NONE )
        {
            LogError( ( "Failed to set socket send timeout. Socket status %d.", transportSocketStatus ) );
        }
    }

    return transportSocketStatus;
}

/*-----------------------------------------------------------*/

static TransportSocketStatus_t establishConnect( NetworkContext_t * pNetworkContext,
                                                 const ServerInfo_t * pServerInfo,
                                                 const SocketsConfig_t * pSocketsConfig )
{
    Socket_t tcpSocket = ( Socket_t ) SOCKETS_INVALID_SOCKET;
    TransportSocketStatus_t returnStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;
    int32_t transportSocketStatus = ( int32_t ) SOCKETS_ERROR_NONE;
    SocketsSockaddr_t serverAddress = { 0 };
    const size_t hostnameLength = pServerInfo->hostNameLength;

    if( ( hostnameLength > ( size_t ) securesocketsMAX_DNS_NAME_LENGTH ) )
    {
        LogError( ( "Host name length %d exceeds max length %d",
                    hostnameLength, securesocketsMAX_DNS_NAME_LENGTH ) );
        returnStatus = TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER;
    }

    if( returnStatus == TRANSPORT_SOCKET_STATUS_SUCCESS )
    {
        /* Create a new TCP socket. */
        tcpSocket = SOCKETS_Socket( SOCKETS_AF_INET,
                                    SOCKETS_SOCK_STREAM,
                                    SOCKETS_IPPROTO_TCP );

        if( tcpSocket == ( Socket_t ) SOCKETS_INVALID_SOCKET )
        {
            LogError( ( "Failed to create new socket.\n" ) );
            returnStatus = TRANSPORT_SOCKET_STATUS_INSUFFICIENT_MEMORY;
        }
    }

    if( returnStatus == TRANSPORT_SOCKET_STATUS_SUCCESS )
    {
        /* Set up connection encryption if credentials are provided. */
        if( pSocketsConfig->enableTls == true )
        {
            if( ( int32_t ) SOCKETS_ERROR_NONE != tlsSetup( pSocketsConfig,
                                                            tcpSocket,
                                                            pServerInfo->pHostName,
                                                            hostnameLength ) )
            {
                returnStatus = TRANSPORT_SOCKET_STATUS_INVALID_CREDENTIALS;
            }
        }
    }

    if( returnStatus == TRANSPORT_SOCKET_STATUS_SUCCESS )
    {
        /* Establish connection. */
        serverAddress.ucSocketDomain = SOCKETS_AF_INET;
        serverAddress.usPort = SOCKETS_htons( pServerInfo->port );
        serverAddress.ulAddress = SOCKETS_GetHostByName( pServerInfo->pHostName );

        /* Check for errors from DNS lookup. */
        if( serverAddress.ulAddress == ( uint32_t ) 0 )
        {
            LogError( ( "Failed to connect to server: DNS resolution failed: Server=%s.", pServerInfo->pHostName ) );
            returnStatus = TRANSPORT_SOCKET_STATUS_DNS_FAILURE;
        }
    }

    if( returnStatus == TRANSPORT_SOCKET_STATUS_SUCCESS )
    {
        transportSocketStatus = SOCKETS_Connect( tcpSocket,
                                                 &serverAddress,
                                                 sizeof( SocketsSockaddr_t ) );

        if( transportSocketStatus != ( int32_t ) SOCKETS_ERROR_NONE )
        {
            LogError( ( "Failed to establish new connection. Socket status: %d.", transportSocketStatus ) );
            returnStatus = TRANSPORT_SOCKET_STATUS_CONNECT_FAILURE;
        }
    }

    if( returnStatus == TRANSPORT_SOCKET_STATUS_SUCCESS )
    {
        /* Configure send and receive timeouts for the socket. */
        transportSocketStatus = transportTimeoutSetup( tcpSocket, pSocketsConfig->sendTimeoutMs, pSocketsConfig->recvTimeoutMs );

        if( transportSocketStatus != ( int32_t ) SOCKETS_ERROR_NONE )
        {
            LogError( ( "Failed to configure send and receive timeouts for socket: Status=%d.", transportSocketStatus ) );
            returnStatus = TRANSPORT_SOCKET_STATUS_INTERNAL_ERROR;
        }
    }

    if( returnStatus == TRANSPORT_SOCKET_STATUS_SUCCESS )
    {
        /* Set the socket in the network context. */
        pNetworkContext->pContext = tcpSocket;
    }
    else
    {
        /* Clean up socket on failure. */
        ( void ) SOCKETS_Close( tcpSocket );
    }

    return returnStatus;
}

/*-----------------------------------------------------------*/

TransportSocketStatus_t SecureSocketsTransport_Connect( NetworkContext_t * pNetworkContext,
                                                        const ServerInfo_t * pServerInfo,
                                                        const SocketsConfig_t * pSocketsConfig )
{
    TransportSocketStatus_t returnStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;

    /* Sanity checks for input parameters. */
    if( pSocketsConfig == NULL )
    {
        LogError( ( "Parameter check failed: pSocketsConfig is NULL." ) );
        returnStatus = TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER;
    }
    else if( pServerInfo == NULL )
    {
        LogError( ( "Parameter check failed: pServerInfo is NULL." ) );
        returnStatus = TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER;
    }
    else if( pServerInfo->pHostName == NULL )
    {
        LogError( ( "Parameter check failed: pServerInfo->pHostName is NULL." ) );
        returnStatus = TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER;
    }
    else if( pServerInfo->hostNameLength == 0UL )
    {
        LogError( ( "Parameter check failed: hostNameLength must be greater than 0." ) );
        returnStatus = TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER;
    }
    else
    {
        /* Establish the TCP connection. */
        returnStatus = establishConnect( pNetworkContext,
                                         pServerInfo,
                                         pSocketsConfig );
    }

    return returnStatus;
}

/*-----------------------------------------------------------*/

int32_t SecureSocketsTransport_Disconnect( const NetworkContext_t * pNetworkContext )
{
    TransportSocketStatus_t returnStatus = TRANSPORT_SOCKET_STATUS_INVALID_PARAMETER;

    if( pNetworkContext != NULL )
    {
        /* Call Secure Sockets shutdown function to close connection. */
        if( SOCKETS_Shutdown( ( Socket_t ) pNetworkContext->pContext, SOCKETS_SHUT_RDWR ) != ( int32_t ) SOCKETS_ERROR_NONE )
        {
            LogError( ( "Failed to close connection: SOCKETS_Shutdown call failed." ) );
            returnStatus = TRANSPORT_SOCKET_STATUS_INTERNAL_ERROR;
        }
        else
        {
            /* Call Secure Sockets close function to close socket. */
            if( SOCKETS_Close( ( Socket_t ) pNetworkContext->pContext ) != ( int32_t ) SOCKETS_ERROR_NONE )
            {
                LogError( ( "Failed to close connection: SOCKETS_Close call failed." ) );

                returnStatus = TRANSPORT_SOCKET_STATUS_INTERNAL_ERROR;
            }
            else
            {
                returnStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;
            }
        }
    }
    else
    {
        LogError( ( "Failed to close connection: pTransportInterface is NULL." ) );
    }

    return returnStatus;
}

/*-----------------------------------------------------------*/
