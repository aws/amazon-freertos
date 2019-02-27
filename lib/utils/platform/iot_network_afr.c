/*
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
 */

/**
 * @file iot_network_afr.c
 * @brief Implementation of the network-related functions from iot_network_afr.h
 * for Amazon FreeRTOS secure sockets.
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>

/* Error handling include. */
#include "private/iot_error.h"

/* Amazon FreeRTOS network include. */
#include "platform/iot_network_afr.h"

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_NETWORK
    #define _LIBRARY_LOG_LEVEL        IOT_LOG_LEVEL_NETWORK
#else
    #ifdef IOT_LOG_LEVEL_GLOBAL
        #define _LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
    #else
        #define _LIBRARY_LOG_LEVEL    IOT_LOG_NONE
    #endif
#endif

#define _LIBRARY_LOG_NAME    ( "NET" )
#include "iot_logging_setup.h"

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default implementations of the memory allocation functions and
 * default values for undefined configuration constants.
 */
#ifndef IotNetwork_Malloc
    #define IotNetwork_Malloc    pvPortMalloc
#endif
#ifndef IotNetwork_Free
    #define IotNetwork_Free      vPortFree
#endif

/* Provide default values for undefined configuration constants. */
#ifndef IOT_NETWORK_RECEIVE_BUFFER_SIZE
    #define IOT_NETWORK_RECEIVE_BUFFER_SIZE    ( 1024 )
#endif
#ifndef IOT_NETWORK_SHORT_TIMEOUT_MS
    #define IOT_NETWORK_SHORT_TIMEOUT_MS       ( 100 )
#endif
#ifndef IOT_NETWORK_SHUTDOWN_POLL_MS
    #define IOT_NETWORK_SHUTDOWN_POLL_MS       ( 1000 )
#endif
/** @endcond */

/**
 * @brief The event group bit to set when a connection's socket is shut down.
 */
#define _FLAG_SHUTDOWN               ( 1 )

/**
 * @brief The event group bit to set when a connection's receive task exits.
 */
#define _FLAG_RECEIVE_TASK_EXITED    ( 2 )

/*-----------------------------------------------------------*/

/**
 * @brief An #IotNetworkInterface_t that uses the functions in this file.
 */
const IotNetworkInterface_t _IotNetworkAfr =
{
    .create             = IotNetworkAfr_Create,
    .setReceiveCallback = IotNetworkAfr_SetReceiveCallback,
    .send               = IotNetworkAfr_Send,
    .receive            = IotNetworkAfr_Receive,
    .close              = IotNetworkAfr_Close,
    .destroy            = IotNetworkAfr_Destroy
};

/*-----------------------------------------------------------*/

/**
 * @brief Reallocate a network receive buffer; data is also copied from the old
 * buffer into the new buffer.
 *
 * @param[in] pOldBuffer The current network receive buffer.
 * @param[in] oldBufferSize The size (in bytes) of `pOldBuffer`.
 * @param[in] newBufferSize Requested size of the reallocated buffer.
 * @param[in] copyOffset The offset in `pOldBuffer` from which data should be
 * copied into the new buffer.
 *
 * @return Pointer to a new, reallocated buffer; `NULL` if buffer reallocation
 * failed.
 */
static uint8_t * _reallocReceiveBuffer( uint8_t * pOldBuffer,
                                        size_t oldBufferSize,
                                        size_t newBufferSize,
                                        size_t copyOffset )
{
    uint8_t * pNewBuffer = NULL;

    /* This function should not be called with a smaller new buffer size or a
     * copy offset greater than the old buffer size. */
    configASSERT( newBufferSize >= oldBufferSize );
    configASSERT( copyOffset < oldBufferSize );

    /* Allocate a larger receive buffer. */
    pNewBuffer = IotNetwork_Malloc( newBufferSize );

    /* Copy data into the new buffer. */
    if( pNewBuffer != NULL )
    {
        ( void ) memcpy( pNewBuffer,
                         pOldBuffer + copyOffset,
                         oldBufferSize - copyOffset );
    }

    /* Free the old buffer. */
    IotNetwork_Free( pOldBuffer );

    return pNewBuffer;
}

/*-----------------------------------------------------------*/

/**
 * @brief Block on a socket and wait for incoming data.
 *
 * This function is simulates the use of `poll` on a socket, then reads the data
 * available.
 *
 * @param[in] pNetworkConnection The connection to receive data from.
 * @param[out] pReceiveBuffer The buffer into which the received data will be placed.
 * @param[in] receiveBufferSize The size of `pReceiveBuffer`.
 *
 * @return The number of bytes read into `pReceiveBuffer`; a negative value on error.
 */
static int32_t _readSocket( IotNetworkConnectionAfr_t * pNetworkConnection,
                            uint8_t * pReceiveBuffer,
                            size_t receiveBufferSize )
{
    _IOT_FUNCTION_ENTRY( int32_t, 0 );
    TickType_t receiveTimeout = 0;
    EventBits_t connectionFlags = 0;

    /* Set a long timeout to wait for either a byte to be received or for
     * a socket shutdown. */
    receiveTimeout = pdMS_TO_TICKS( IOT_NETWORK_SHUTDOWN_POLL_MS );

    status = SOCKETS_SetSockOpt( pNetworkConnection->socket,
                                 0,
                                 SOCKETS_SO_RCVTIMEO,
                                 &receiveTimeout,
                                 sizeof( TickType_t ) );

    if( status != SOCKETS_ERROR_NONE )
    {
        _IOT_GOTO_CLEANUP();
    }

    /* Block until a single byte is received from the socket or the socket
     * is shut down. */
    while( ( status = SOCKETS_Recv( pNetworkConnection->socket,
                                    pReceiveBuffer,
                                    1,
                                    0 ) ) == 0 )
    {
        /* Read the current connection flags. */
        connectionFlags = xEventGroupGetBits( ( EventGroupHandle_t ) &( pNetworkConnection->connectionFlags ) );

        /* Check the shutdown flag. */
        if( ( connectionFlags & _FLAG_SHUTDOWN ) == _FLAG_SHUTDOWN )
        {
            _IOT_SET_AND_GOTO_CLEANUP( SOCKETS_ECLOSED );
        }
    }

    /* Check for a socket error from the receive of 1 byte. */
    if( status < 0 )
    {
        _IOT_GOTO_CLEANUP();
    }

    /* A single byte should have been read. */
    configASSERT( status == 1 );

    /* Once a single byte is successfully read from the socket, set a short timeout
     * to read the rest of the data. */
    receiveTimeout = pdMS_TO_TICKS( IOT_NETWORK_SHORT_TIMEOUT_MS );

    status = SOCKETS_SetSockOpt( pNetworkConnection->socket,
                                 0,
                                 SOCKETS_SO_RCVTIMEO,
                                 &receiveTimeout,
                                 sizeof( TickType_t ) );

    if( status != SOCKETS_ERROR_NONE )
    {
        _IOT_GOTO_CLEANUP();
    }

    /* Read all data currently available on the socket. */
    status = SOCKETS_Recv( pNetworkConnection->socket,
                           pReceiveBuffer + 1,
                           receiveBufferSize - 1,
                           0 );

    /* Add the length of the single byte initially read when successful. */
    if( status >= 0 )
    {
        status++;
    }

    _IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

/**
 * @brief Task routine that waits on incoming network data.
 *
 * @param[in] pArgument The network connection.
 */
static void _networkReceiveTask( void * pArgument )
{
    /* Cast network connection to the correct type. */
    IotNetworkConnectionAfr_t * pNetworkConnection = pArgument;
    int32_t socketStatus = SOCKETS_ERROR_NONE, callbackStatus = 0;
    size_t processOffset = 0, receiveBufferSize = IOT_NETWORK_RECEIVE_BUFFER_SIZE;

    /* A receive buffer should have been allocated. */
    configASSERT( pNetworkConnection->pReceiveBuffer != NULL );

    while( true )
    {
        /* An offset larger than the buffer size should not have been calculated. */
        configASSERT( processOffset < receiveBufferSize );

        /* Block and wait for data from the socket. */
        socketStatus = _readSocket( pNetworkConnection,
                                    pNetworkConnection->pReceiveBuffer + processOffset,
                                    receiveBufferSize - processOffset );

        if( socketStatus <= 0 )
        {
            IotLogWarn( "No data could be received. Socket read returned %ld.",
                        socketStatus );

            break;
        }

        /* Invoke the network callback. */
        callbackStatus = pNetworkConnection->receiveCallback( pNetworkConnection->pReceiveContext,
                                                              pNetworkConnection,
                                                              pNetworkConnection->pReceiveBuffer,
                                                              ( size_t ) socketStatus + processOffset,
                                                              0,
                                                              IotNetwork_Free );

        /* Check the receive callback return value. */
        if( callbackStatus == -1 )
        {
            /* Error in callback; exit receive task. */
            break;
        }
        else if( callbackStatus == 0 )
        {
            IotLogDebug( "Receive buffer too small. Allocating larger buffer." );

            /* Check that it's possible to allocate a larger buffer (overflow check). */
            if( receiveBufferSize * 2 < receiveBufferSize )
            {
                IotLogError( "No larger receive buffer can be allocated." );

                break;
            }

            /* Allocate a larger receive buffer. */
            pNetworkConnection->pReceiveBuffer = _reallocReceiveBuffer( pNetworkConnection->pReceiveBuffer,
                                                                        receiveBufferSize,
                                                                        receiveBufferSize * 2,
                                                                        0 );

            /* Update buffer offset and size. */
            if( pNetworkConnection->pReceiveBuffer != NULL )
            {
                processOffset = receiveBufferSize;
                receiveBufferSize *= 2;
            }
            else
            {
                IotLogError( "No larger receive buffer can be allocated." );

                break;
            }
        }
        else if( callbackStatus < socketStatus + ( int32_t ) processOffset )
        {
            IotLogDebug( "Network receive callback processed %ld of %ld bytes.",
                         ( long int ) callbackStatus,
                         ( long int ) socketStatus + ( long int ) processOffset );

            /* Allocate a new buffer and copy the unprocessed data to the beginning. */
            pNetworkConnection->pReceiveBuffer = _reallocReceiveBuffer( pNetworkConnection->pReceiveBuffer,
                                                                        receiveBufferSize,
                                                                        receiveBufferSize,
                                                                        ( size_t ) callbackStatus );

            if( pNetworkConnection->pReceiveBuffer == NULL )
            {
                break;
            }

            processOffset = ( size_t ) socketStatus + processOffset - ( size_t ) callbackStatus;
        }
        else
        {
            /* If this block is reached, then the entire buffer was successfully processed. */
            pNetworkConnection->pReceiveBuffer = IotNetwork_Malloc( receiveBufferSize );

            if( pNetworkConnection->pReceiveBuffer == NULL )
            {
                break;
            }

            processOffset = 0;
        }
    }

    IotLogDebug( "Network receive task terminating." );

    /* If a receive buffer is allocated, free it. */
    if( pNetworkConnection->pReceiveBuffer != NULL )
    {
        IotNetwork_Free( pNetworkConnection->pReceiveBuffer );
    }

    /* Set the flag to indicate that the receive task has exited. */
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &( pNetworkConnection->connectionFlags ),
                                 _FLAG_RECEIVE_TASK_EXITED );

    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/

/**
 * @brief Set up a secured TLS connection.
 *
 * @param[in] pAfrCredentials Credentials for the secured connection.
 * @param[in] tcpSocket An initialized socket to secure.
 * @param[in] pHostName Remote server name for SNI.
 * @param[in] hostnameLength The length of `pHostName`.
 *
 * @return #IOT_NETWORK_SUCCESS or #IOT_NETWORK_SYSTEM_ERROR.
 */
static IotNetworkError_t _tlsSetup( const IotNetworkCredentialsAfr_t * pAfrCredentials,
                                    Socket_t tcpSocket,
                                    const char * pHostName,
                                    size_t hostnameLength )
{
    _IOT_FUNCTION_ENTRY( IotNetworkError_t, IOT_NETWORK_SUCCESS );
    int32_t socketStatus = SOCKETS_ERROR_NONE;

    /* ALPN options for AWS IoT. */
    const char * ppcALPNProtos[] = { socketsAWS_IOT_ALPN_MQTT };

    /* Set secured option. */
    socketStatus = SOCKETS_SetSockOpt( tcpSocket,
                                       0,
                                       SOCKETS_SO_REQUIRE_TLS,
                                       NULL,
                                       0 );

    if( socketStatus != SOCKETS_ERROR_NONE )
    {
        IotLogError( "Failed to set secured option for new connection." );
        _IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_SYSTEM_ERROR );
    }

    /* Set ALPN option. */
    if( pAfrCredentials->pAlpnProtos != NULL )
    {
        socketStatus = SOCKETS_SetSockOpt( tcpSocket,
                                           0,
                                           SOCKETS_SO_ALPN_PROTOCOLS,
                                           ppcALPNProtos,
                                           sizeof( ppcALPNProtos ) / sizeof( ppcALPNProtos[ 0 ] ) );

        if( socketStatus != SOCKETS_ERROR_NONE )
        {
            IotLogError( "Failed to set ALPN option for new connection." );
            _IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_SYSTEM_ERROR );
        }
    }

    /* Set SNI option. */
    if( pAfrCredentials->disableSni == false )
    {
        socketStatus = SOCKETS_SetSockOpt( tcpSocket,
                                           0,
                                           SOCKETS_SO_SERVER_NAME_INDICATION,
                                           pHostName,
                                           hostnameLength + 1 );

        if( socketStatus != SOCKETS_ERROR_NONE )
        {
            IotLogError( "Failed to set SNI option for new connection." );
            _IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_SYSTEM_ERROR );
        }
    }

    /* Set custom server certificate. */
    if( pAfrCredentials->pRootCa != NULL )
    {
        socketStatus = SOCKETS_SetSockOpt( tcpSocket,
                                           0,
                                           SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE,
                                           pAfrCredentials->pRootCa,
                                           pAfrCredentials->rootCaSize );

        if( socketStatus != SOCKETS_ERROR_NONE )
        {
            IotLogError( "Failed to set server certificate option for new connection." );
            _IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_SYSTEM_ERROR );
        }
    }

    _IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotNetworkError_t IotNetworkAfr_Create( void * pConnectionInfo,
                                        void * pCredentialInfo,
                                        void * pConnection )
{
    _IOT_FUNCTION_ENTRY( IotNetworkError_t, IOT_NETWORK_SUCCESS );
    Socket_t tcpSocket = SOCKETS_INVALID_SOCKET;
    int32_t connectStatus = SOCKETS_ERROR_NONE;
    SocketsSockaddr_t serverAddress = { 0 };
    EventGroupHandle_t pConnectionFlags = NULL;
    SemaphoreHandle_t pConnectionMutex = NULL;

    /* Cast function parameters to correct types. */
    const IotNetworkServerInfoAfr_t * pServerInfo = pConnectionInfo;
    const IotNetworkCredentialsAfr_t * pAfrCredentials = pCredentialInfo;
    IotNetworkConnectionAfr_t * pNetworkConnection = pConnection;

    /* Check host name length against the maximum length allowed by Secure
     * Sockets. */
    const size_t hostnameLength = strlen( pServerInfo->pHostName );

    if( hostnameLength > ( size_t ) securesocketsMAX_DNS_NAME_LENGTH )
    {
        IotLogError( "Host name length exceeds %d, which is the maximum allowed.",
                     securesocketsMAX_DNS_NAME_LENGTH );
        _IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_BAD_PARAMETER );
    }

    /* Clear the connection information. */
    ( void ) memset( pNetworkConnection, 0x00, sizeof( IotNetworkConnectionAfr_t ) );

    /* Create a new TCP socket. */
    tcpSocket = SOCKETS_Socket( SOCKETS_AF_INET,
                                SOCKETS_SOCK_STREAM,
                                SOCKETS_IPPROTO_TCP );

    if( tcpSocket == SOCKETS_INVALID_SOCKET )
    {
        IotLogError( "Failed to create new socket." );
        _IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_SYSTEM_ERROR );
    }

    /* Set up connection encryption if credentials are provided. */
    if( pAfrCredentials != NULL )
    {
        status = _tlsSetup( pAfrCredentials, tcpSocket, pServerInfo->pHostName, hostnameLength );

        if( status != IOT_NETWORK_SUCCESS )
        {
            _IOT_GOTO_CLEANUP();
        }
    }

    /* Establish connection. */
    serverAddress.ucSocketDomain = SOCKETS_AF_INET;
    serverAddress.usPort = SOCKETS_htons( pServerInfo->port );
    serverAddress.ulAddress = SOCKETS_GetHostByName( pServerInfo->pHostName );

    connectStatus = SOCKETS_Connect( tcpSocket,
                                     &serverAddress,
                                     sizeof( SocketsSockaddr_t ) );

    if( connectStatus != SOCKETS_ERROR_NONE )
    {
        IotLogError( "Failed to establish new connection." );
        _IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_SYSTEM_ERROR );
    }

    _IOT_FUNCTION_CLEANUP_BEGIN();

    /* Clean up on failure. */
    if( status != IOT_NETWORK_SUCCESS )
    {
        if( tcpSocket != SOCKETS_INVALID_SOCKET )
        {
            SOCKETS_Close( tcpSocket );
        }

        /* Clear the connection information. */
        ( void ) memset( pNetworkConnection, 0x00, sizeof( IotNetworkConnectionAfr_t ) );
    }
    else
    {
        /* Set the output parameter. */
        pNetworkConnection->socket = tcpSocket;

        /* Create the connection event flags and mutex. */
        pConnectionFlags = xEventGroupCreateStatic( &( pNetworkConnection->connectionFlags ) );
        pConnectionMutex = xSemaphoreCreateMutexStatic( &( pNetworkConnection->socketMutex ) );

        /* Static event flags and mutex creation should never fail. The handles
         * should point inside the connection object. */
        configASSERT( pConnectionFlags == ( EventGroupHandle_t ) &( pNetworkConnection->connectionFlags ) );
        configASSERT( pConnectionMutex == ( SemaphoreHandle_t ) &( pNetworkConnection->socketMutex ) );
    }

    _IOT_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

IotNetworkError_t IotNetworkAfr_SetReceiveCallback( void * pConnection,
                                                    IotNetworkReceiveCallback_t receiveCallback,
                                                    void * pContext )
{
    _IOT_FUNCTION_ENTRY( IotNetworkError_t, IOT_NETWORK_SUCCESS );

    /* Cast network connection to the correct type. */
    IotNetworkConnectionAfr_t * pNetworkConnection = pConnection;

    /* Set the receive callback and context. */
    pNetworkConnection->receiveCallback = receiveCallback;
    pNetworkConnection->pReceiveContext = pContext;

    /* No receive buffer should be allocated, and no flags should be set. */
    configASSERT( pNetworkConnection->pReceiveBuffer == NULL );
    configASSERT( xEventGroupGetBits( ( EventGroupHandle_t ) &( pNetworkConnection->connectionFlags ) ) == 0 );

    /* Allocate initial network receive buffer. */
    pNetworkConnection->pReceiveBuffer = IotNetwork_Malloc( IOT_NETWORK_RECEIVE_BUFFER_SIZE );

    if( pNetworkConnection->pReceiveBuffer == NULL )
    {
        IotLogError( "Failed to allocate network receive buffer." );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_NO_MEMORY );
    }

    /* Create task that waits for incoming data. */
    if( xTaskCreate( _networkReceiveTask,
                     "NetRecv",
                     IOT_NETWORK_RECEIVE_TASK_STACK_SIZE,
                     pNetworkConnection,
                     IOT_NETWORK_RECEIVE_TASK_PRIORITY,
                     NULL ) != pdPASS )
    {
        IotLogError( "Failed to create network receive task." );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_SYSTEM_ERROR );
    }

    /* Clean up on failure. */
    _IOT_FUNCTION_CLEANUP_BEGIN();

    if( status != IOT_NETWORK_SUCCESS )
    {
        if( pNetworkConnection->pReceiveBuffer != NULL )
        {
            IotNetwork_Free( pNetworkConnection->pReceiveBuffer );
        }
    }

    _IOT_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

size_t IotNetworkAfr_Send( void * pConnection,
                           const uint8_t * pMessage,
                           size_t messageLength )
{
    size_t bytesSent = 0;
    int32_t socketStatus = SOCKETS_ERROR_NONE;

    /* Cast network connection to the correct type. */
    IotNetworkConnectionAfr_t * pNetworkConnection = pConnection;

    /* Only one thread at a time may send on the connection. Lock the socket
     * mutex to prevent other threads from sending. */
    if( xSemaphoreTake( ( QueueHandle_t ) &( pNetworkConnection->socketMutex ),
                        portMAX_DELAY ) == pdTRUE )
    {
        socketStatus = SOCKETS_Send( pNetworkConnection->socket,
                                     pMessage,
                                     messageLength,
                                     0 );

        if( socketStatus > 0 )
        {
            bytesSent = ( size_t ) socketStatus;
        }

        xSemaphoreGive( ( QueueHandle_t ) &( pNetworkConnection->socketMutex ) );
    }

    return bytesSent;
}

/*-----------------------------------------------------------*/

size_t IotNetworkAfr_Receive( void * pConnection,
                              uint8_t * pBuffer,
                              size_t bytesRequested )
{
    /* Not implemented yet, return 0. */
    return 0;
}

/*-----------------------------------------------------------*/

IotNetworkError_t IotNetworkAfr_Close( void * pConnection )
{
    int32_t socketStatus = SOCKETS_ERROR_NONE;

    /* Cast network connection to the correct type. */
    IotNetworkConnectionAfr_t * pNetworkConnection = pConnection;

    /* Call Secure Sockets shutdown function to close connection. */
    socketStatus = SOCKETS_Shutdown( pNetworkConnection->socket,
                                     SOCKETS_SHUT_RDWR );

    if( socketStatus != SOCKETS_ERROR_NONE )
    {
        IotLogWarn( "Failed to close connection." );
    }

    /* Set the shutdown flag. */
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &( pNetworkConnection->connectionFlags ),
                                 _FLAG_SHUTDOWN );

    return IOT_NETWORK_SUCCESS;
}

/*-----------------------------------------------------------*/

IotNetworkError_t IotNetworkAfr_Destroy( void * pConnection )
{
    int32_t socketStatus = SOCKETS_ERROR_NONE;

    /* Cast network connection to the correct type. */
    IotNetworkConnectionAfr_t * pNetworkConnection = pConnection;

    /* Wait for the receive task to exit. */
    ( void ) xEventGroupWaitBits( ( EventGroupHandle_t ) &( pNetworkConnection->connectionFlags ),
                                  _FLAG_RECEIVE_TASK_EXITED,
                                  pdTRUE,
                                  pdTRUE,
                                  portMAX_DELAY );

    /* Call Secure Sockets close function to free resources. */
    socketStatus = SOCKETS_Close( pNetworkConnection->socket );

    if( socketStatus != SOCKETS_ERROR_NONE )
    {
        IotLogWarn( "Failed to destroy connection." );
    }

    /* Clear the network connection. */
    ( void ) memset( pNetworkConnection, 0x00, sizeof( IotNetworkConnectionAfr_t ) );

    return IOT_NETWORK_SUCCESS;
}

/*-----------------------------------------------------------*/
