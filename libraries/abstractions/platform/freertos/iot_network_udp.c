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
 * @file iot_network_udp.c
 * @brief Implementation of the network-related functions from iot_network_udp.h
 * for Amazon FreeRTOS secure sockets.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* FreeRTOS includes. */
#include "semphr.h"
#include "event_groups.h"

/* Error handling include. */
#include "private/iot_error.h"

/* Amazon FreeRTOS network include. */
#include "platform/iot_network_udp.h"

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_NETWORK
    #define LIBRARY_LOG_LEVEL        IOT_LOG_LEVEL_NETWORK
#else
    #ifdef IOT_LOG_LEVEL_GLOBAL
        #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
    #else
        #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
    #endif
#endif

#define LIBRARY_LOG_NAME    ( "NET" )
#include "iot_logging_setup.h"

/* Provide a default value for the number of milliseconds for a socket poll.
 * This is a temporary workaround to deal with the lack of poll(). */
#ifndef IOT_NETWORK_SOCKET_POLL_MS
    #define IOT_NETWORK_SOCKET_POLL_MS    ( 1000 )
#endif

/**
 * @brief The event group bit to set when a connection's socket is shut down.
 */
#define _FLAG_SHUTDOWN                ( 1 )

/**
 * @brief The event group bit to set when a connection's receive task exits.
 */
#define _FLAG_RECEIVE_TASK_EXITED     ( 2 )

/**
 * @brief The event group bit to set when the connection is destroyed from the
 * receive task.
 */
#define _FLAG_CONNECTION_DESTROYED    ( 4 )
/*-----------------------------------------------------------*/

typedef struct _networkConnection
{
    Socket_t socket;                             /**< @brief Amazon FreeRTOS Secure Sockets handle. */
    SocketsSockaddr_t serverAddr;                /**< @brief Server address. */
    bool strictServerAddrCheck;                  /**< @brief Verify if server address matches recvFrom address */
    StaticSemaphore_t socketMutex;               /**< @brief Prevents concurrent threads from sending on a socket. */
    StaticEventGroup_t connectionFlags;          /**< @brief Synchronizes with the receive task. */
    TaskHandle_t receiveTask;                    /**< @brief Handle of the receive task, if any. */
    IotNetworkReceiveCallback_t receiveCallback; /**< @brief Network receive callback, if any. */
    void * pReceiveContext;                      /**< @brief The context for the receive callback. */
    bool bufferedByteValid;                      /**< @brief Used to determine if the buffered byte is valid. */
    uint8_t bufferedByte;                        /**< @brief A single byte buffered from a receive, since AFR Secure Sockets does not have poll(). */
} _networkConnection_t;

/*-----------------------------------------------------------*/

/**
 * @brief An #IotNetworkInterface_t that uses the functions in this file.
 */
const IotNetworkInterface_t IotNetworkUdp =
{
    .create             = IotNetworkUdp_Create,
    .setReceiveCallback = IotNetworkUdp_SetReceiveCallback,
    .send               = IotNetworkUdp_SendTo,
    .receive            = IotNetworkUdp_ReceiveFrom,
    .close              = IotNetworkUdp_Close,
    .destroy            = IotNetworkUdp_Destroy
};
/*-----------------------------------------------------------*/

/**
 * @brief Destroys a network connection.
 *
 * @param[in] pNetworkConnection The connection to destroy.
 */
static void _destroyConnection( _networkConnection_t * pNetworkConnection )
{
    /* Call Secure Sockets close function to free resources. */
    int32_t socketStatus = SOCKETS_Close( pNetworkConnection->socket );

    if( socketStatus != SOCKETS_ERROR_NONE )
    {
        IotLogWarn( "Failed to destroy connection." );
    }

    /* Free the network connection. */
    vPortFree( pNetworkConnection );
}
/*-----------------------------------------------------------*/

/**
 * @brief Task routine that waits on incoming network data.
 * @Note This is designed for async read for clients.
 *
 * @param[in] pArgument The network connection.
 */
static void _networkReceiveTask( void * pArgument )
{
    bool destroyConnection = false;
    int32_t socketStatus = 0;
    EventBits_t connectionFlags = 0;
    SocketsSockaddr_t serverAddress = { 0 };
    size_t addrLength = 0;

    /* Cast network connection to the correct type. */
    _networkConnection_t * pNetworkConnection = pArgument;

    while( true )
    {
        /* No buffered byte should be in the connection. */
        configASSERT( pNetworkConnection->bufferedByteValid == false );

        /* UDP will discard the buffer once it is read, therefore, PEEK flag is needed.
         * recvFrom() will try to read 1 byte, and will return when data is avaialble.
         * For asynchronous UDP read, socket timeout will trigger failure as the task should not block for ever.
         */
        do
        {
            socketStatus = SOCKETS_RecvFrom( pNetworkConnection->socket,
                                             &( pNetworkConnection->bufferedByte ),
                                             1,
                                             SOCKETS_MSG_PEEK,
                                             &serverAddress,
                                             &addrLength );

            connectionFlags = xEventGroupGetBits( ( EventGroupHandle_t ) &( pNetworkConnection->connectionFlags ) );

            if( ( connectionFlags & _FLAG_SHUTDOWN ) == _FLAG_SHUTDOWN )
            {
                socketStatus = SOCKETS_ECLOSED;
            }

            /* Check for timeout. for all error including EWOULDBLOCK break */
        } while( ( socketStatus == 0 ) );

        if( socketStatus <= 0 )
        {
            IotLogWarn( " _networkReceiveTask failed with error code: %d ", socketStatus );
            break;
        }
        else
        {
            pNetworkConnection->bufferedByteValid = true;
        }

        /* Invoke the network callback. */
        pNetworkConnection->receiveCallback( pNetworkConnection,
                                             pNetworkConnection->pReceiveContext );

        /* Check if the connection was destroyed by the receive callback. This
         * does not need to be thread-safe because the destroy connection function
         * may only be called once (per its API doc). */
        connectionFlags = xEventGroupGetBits( ( EventGroupHandle_t ) &( pNetworkConnection->connectionFlags ) );

        if( ( connectionFlags & _FLAG_CONNECTION_DESTROYED ) == _FLAG_CONNECTION_DESTROYED )
        {
            destroyConnection = true;
            break;
        }
    }

    IotLogDebug( "Network receive task terminating." );

    /* If necessary, destroy the network connection before exiting. */
    if( destroyConnection == true )
    {
        _destroyConnection( pNetworkConnection );
    }
    else
    {
        IotLogDebug( "destroyConnection False: set _FLAG_RECEIVE_TASK_EXITED" );
        /* Set the flag to indicate that the receive task has exited. */
        ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &( pNetworkConnection->connectionFlags ),
                                     _FLAG_RECEIVE_TASK_EXITED );
    }

    IotLogDebug( "Network receive task: before vTaskDelete" );

    vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/

IotNetworkError_t IotNetworkUdp_Create( void * pConnectionInfo,
                                        void * pCredentialInfo,
                                        void ** pConnection )
{
    IOT_FUNCTION_ENTRY( IotNetworkError_t, IOT_NETWORK_SUCCESS );
    Socket_t udpSocket = SOCKETS_INVALID_SOCKET;
    int32_t udpServerAddr = 0;
    int32_t socketStatus = SOCKETS_ERROR_NONE;
    EventGroupHandle_t pConnectionFlags = NULL;
    const IotNetworkServerInfoUdp_t * pServerInfo = NULL;
    _networkConnection_t ** pNetworkConnection = NULL;
    TickType_t socketTimeout = 0;
    size_t hostnameLength = 0;


    /* Credentials are not used  for UDP for now */
    ( void ) pCredentialInfo;

    /* Make sure all params are valid */
    if( ( pConnectionInfo == NULL ) ||
        ( pConnection == NULL ) ||
        ( *pConnection != IOT_NETWORK_INVALID_CONNECTION ) )
    {
        IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_BAD_PARAMETER );
    }

    pServerInfo = ( IotNetworkServerInfoUdp_t * ) pConnectionInfo;

    if( pServerInfo->pHostName == NULL )
    {
        IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_BAD_PARAMETER );
    }

    pNetworkConnection = ( _networkConnection_t ** ) pConnection;
    socketTimeout = pdMS_TO_TICKS( pServerInfo->socketTimeout );
    hostnameLength = strlen( pServerInfo->pHostName );

    /* Check host name length against the maximum length allowed by Secure
     * Sockets. */
    if( hostnameLength > ( size_t ) securesocketsMAX_DNS_NAME_LENGTH )
    {
        IotLogError( "Host name length exceeds %d, which is the maximum allowed.",
                     securesocketsMAX_DNS_NAME_LENGTH );
        IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_BAD_PARAMETER );
    }

    udpSocket = SOCKETS_Socket( SOCKETS_AF_INET,
                                SOCKETS_SOCK_DGRAM,
                                SOCKETS_IPPROTO_UDP );

    if( udpSocket == SOCKETS_INVALID_SOCKET )
    {
        IotLogError( "Failed to create new socket." );
        IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_SYSTEM_ERROR );
    }

    /* Bind socket */
    if( pServerInfo->noBind != true )
    {
        SocketsSockaddr_t sockAddr = { 0 };
        socketStatus = SOCKETS_Bind( udpSocket, &sockAddr, sizeof( SocketsSockaddr_t ) );

        if( socketStatus != SOCKETS_ERROR_NONE )
        {
            IotLogError( " Failed to bind UDP socket status = %d ", socketStatus );
            IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_FAILURE );
        }
    }

    /* Set receive timeout for the socket. */
    socketStatus = SOCKETS_SetSockOpt( udpSocket,
                                       0,
                                       SOCKETS_SO_RCVTIMEO,
                                       &socketTimeout,
                                       sizeof( socketTimeout ) );

    if( socketStatus != SOCKETS_ERROR_NONE )
    {
        IotLogError( " Failed to set UDP socket timeout status = %d ", socketStatus );
        IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_FAILURE );
    }

    udpServerAddr = SOCKETS_GetHostByName( pServerInfo->pHostName );

    if( udpServerAddr == 0 )
    {
        IotLogError( "Failed to resolve DNS address %s.",
                     pServerInfo->pHostName );
        IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_FAILURE );
    }

    *pNetworkConnection = pvPortMalloc( sizeof( _networkConnection_t ) );

    if( *pNetworkConnection == NULL )
    {
        IotLogError( "Failed to allocate memory for new network connection." );
        IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_NO_MEMORY );
    }

    /* Clear the connection information. */
    ( void ) memset( *pNetworkConnection, 0x00, sizeof( _networkConnection_t ) );

    ( *pNetworkConnection )->socket = udpSocket;
    ( *pNetworkConnection )->serverAddr.ulAddress = udpServerAddr;
    ( *pNetworkConnection )->serverAddr.usPort = SOCKETS_htons( pServerInfo->port );
    ( *pNetworkConnection )->serverAddr.ucSocketDomain = SOCKETS_AF_INET;
    ( *pNetworkConnection )->strictServerAddrCheck = pServerInfo->strictServerAddrCheck;

    /* Create the connection event flags and mutex. */
    pConnectionFlags = xEventGroupCreateStatic( &( ( *pNetworkConnection )->connectionFlags ) );

    /* Static event flags and mutex creation should never fail. The handles
     * should point inside the connection object. */
    configASSERT( pConnectionFlags == ( EventGroupHandle_t ) &( ( *pNetworkConnection )->connectionFlags ) );


    IOT_FUNCTION_CLEANUP_BEGIN();

    if( status != IOT_NETWORK_SUCCESS )
    {
        if( udpSocket != SOCKETS_INVALID_SOCKET )
        {
            SOCKETS_Close( udpSocket );
        }

        /* Clear the connection information. */
        if( ( pNetworkConnection != NULL ) && ( ( *pNetworkConnection ) != NULL ) )
        {
            vPortFree( *pNetworkConnection );
        }
    }

    IOT_FUNCTION_CLEANUP_END();
}
/*-----------------------------------------------------------*/

IotNetworkError_t IotNetworkUdp_SetReceiveCallback( void * pConnection,
                                                    IotNetworkReceiveCallback_t receiveCallback,
                                                    void * pContext )
{
    IOT_FUNCTION_ENTRY( IotNetworkError_t, IOT_NETWORK_SUCCESS );

    /* Cast network connection to the correct type. */
    _networkConnection_t * pNetworkConnection = ( _networkConnection_t * ) pConnection;

    if( pConnection == NULL )
    {
        IotLogWarn( "Invalid connection parameter" );
        IOT_SET_AND_GOTO_CLEANUP( IOT_NETWORK_BAD_PARAMETER );
    }

    /* Set the receive callback and context. */
    pNetworkConnection->receiveCallback = receiveCallback;
    pNetworkConnection->pReceiveContext = pContext;

    /* No flags should be set. */
    configASSERT( xEventGroupGetBits( ( EventGroupHandle_t ) &( pNetworkConnection->connectionFlags ) ) == 0 );

    /* Create task that waits for incoming data. */
    if( xTaskCreate( _networkReceiveTask,
                     "NetUdpRecv",
                     IOT_NETWORK_RECEIVE_TASK_STACK_SIZE,
                     pNetworkConnection,
                     IOT_NETWORK_RECEIVE_TASK_PRIORITY,
                     &( pNetworkConnection->receiveTask ) ) != pdPASS )
    {
        configASSERT( pNetworkConnection->receiveTask );
        IotLogError( "Failed to create network receive task." );

        status = IOT_NETWORK_SYSTEM_ERROR;
    }

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}
/*-----------------------------------------------------------*/

size_t IotNetworkUdp_SendTo( void * pConnection,
                           const uint8_t * pMessage,
                           size_t messageLength )
{
    size_t bytesSent = 0;
    _networkConnection_t * pNetworkConnection = ( _networkConnection_t * ) pConnection;

    if( pConnection == NULL )
    {
        IotLogWarn( "Invalid connection parameter" );
    }
    else
    {
        bytesSent = SOCKETS_SendTo( pNetworkConnection->socket,
                                    ( void * ) pMessage,
                                    messageLength,
                                    0,
                                    &pNetworkConnection->serverAddr,
                                    sizeof( SocketsSockaddr_t ) );
    }

    return bytesSent;
}
/*-----------------------------------------------------------*/

size_t IotNetworkUdp_ReceiveFrom( void * pConnection,
                              uint8_t * pBuffer,
                              size_t bytesRequested )
{
    int32_t socketStatus = 0;
    _networkConnection_t * pNetworkConnection = ( _networkConnection_t * ) pConnection;
    SocketsSockaddr_t serverAddress = { 0 };
    size_t addrLength = 0;

    if( pConnection == NULL )
    {
        IotLogWarn( "Invalid connection parameter" );
    }
    /* if Receive was called as a result of asynch callback, check if receive had failed.*/
    else if( ( xTaskGetCurrentTaskHandle() == pNetworkConnection->receiveTask ) && ( pNetworkConnection->bufferedByteValid == false ) )
    {
        IotLogWarn( " Async receive indicated failure\n" );
    }
    else
    {
        socketStatus = SOCKETS_RecvFrom( pNetworkConnection->socket,
                                         pBuffer,
                                         bytesRequested,
                                         0,
                                         &serverAddress,
                                         &addrLength );

        if( socketStatus <= 0 )
        {
            IotLogError( "Failed to receive requested bytes: %d, received: %d socketStatus: %d", bytesRequested, bytesReceived,
                         socketStatus );
        }

        configASSERT( socketStatus == bytesRequested );
    }

    if( ( socketStatus > 0 ) && ( pNetworkConnection->strictServerAddrCheck ) )
    {
        /* if strict server address check is enabled, only accept packets from the server that was used for Send */
        if( serverAddress.ulAddress != pNetworkConnection->serverAddr.ulAddress )
        {
            IotLogError( "Server address does not match and strict server address check enabled." );
            socketStatus = 0;
        }
    }

    /* if Receive was called as a result of asynch callback, set bufferbytes valid to false.*/
    if( ( xTaskGetCurrentTaskHandle() == pNetworkConnection->receiveTask ) )
    {
        pNetworkConnection->bufferedByteValid = false;
    }

    return socketStatus;
}
/*-----------------------------------------------------------*/

IotNetworkError_t IotNetworkUdp_Close( void * pConnection )
{
    IOT_FUNCTION_ENTRY( IotNetworkError_t, IOT_NETWORK_SUCCESS );

    /* Cast network connection to the correct type. */
    _networkConnection_t * pNetworkConnection = ( _networkConnection_t * ) pConnection;

    if( pConnection == NULL )
    {
        IotLogWarn( "Invalid connection parameter" );
    }
    else if( pNetworkConnection->socket == SOCKETS_INVALID_SOCKET )
    {
        IotLogWarn( " Close failed: Invalid Socket " );
    }
    else
    {
        /* If receive task was started , indicate SHUT_DOWN */
        if( pNetworkConnection->receiveCallback != NULL )
        {
            /* Set the shutdown flag. */
            ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &( pNetworkConnection->connectionFlags ),
                                         _FLAG_SHUTDOWN );
        }
    }

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}
/*-----------------------------------------------------------*/

IotNetworkError_t IotNetworkUdp_Destroy( void * pConnection )
{
    IOT_FUNCTION_ENTRY( IotNetworkError_t, IOT_NETWORK_SUCCESS );

    /* Cast network connection to the correct type. */
    _networkConnection_t * pNetworkConnection = ( _networkConnection_t * ) pConnection;

    /* Check if this function is being called from the receive task. */
    if( xTaskGetCurrentTaskHandle() == pNetworkConnection->receiveTask )
    {
        /* Set the flag specifying that the connection is destroyed. */
        ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &( pNetworkConnection->connectionFlags ),
                                     _FLAG_CONNECTION_DESTROYED );
    }
    else
    {
        /* if receive task was created, wait for it to exit */
        if( pNetworkConnection->receiveCallback != NULL )
        {
            IotLogWarn( "Waiting for receive task to exist " );
            ( void ) xEventGroupWaitBits( ( EventGroupHandle_t ) &( pNetworkConnection->connectionFlags ),
                                          _FLAG_RECEIVE_TASK_EXITED,
                                          pdTRUE,
                                          pdTRUE,
                                          portMAX_DELAY );
        }

        _destroyConnection( pNetworkConnection );
    }

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}
/*-----------------------------------------------------------*/
