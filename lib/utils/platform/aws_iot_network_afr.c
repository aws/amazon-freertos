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
 * @file aws_iot_network_afr.c
 * @brief Implementation of the network-related functions from aws_iot_network.h
 * for Amazon FreeRTOS secure sockets.
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>

/* Platform layer includes. */
#include "platform/aws_iot_network.h"

/* Amazon FreeRTOS secure sockets include. */
#include "aws_secure_sockets.h"

/* Configure logs for the functions in this file. */
#ifdef AWS_IOT_NETWORK_LOG_LEVEL
    #define _LIBRARY_LOG_LEVEL        AWS_IOT_NETWORK_LOG_LEVEL
#else
    #ifdef AWS_IOT_GLOBAL_LOG_LEVEL
        #define _LIBRARY_LOG_LEVEL    AWS_IOT_GLOBAL_LOG_LEVEL
    #else
        #define _LIBRARY_LOG_LEVEL    AWS_IOT_LOG_NONE
    #endif
#endif

#define _LIBRARY_LOG_NAME    ( "NET" )
#include "aws_iot_logging_setup.h"

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default implementations of the memory allocation functions and
 * default values for undefined configuration constants.
 */
#ifndef AwsIotNetwork_Malloc
    #define AwsIotNetwork_Malloc    pvPortMalloc
#endif
#ifndef AwsIotNetwork_Free
    #define AwsIotNetwork_Free      vPortFree
#endif

/* Provide default values for undefined configuration constants. */
#ifndef AWS_IOT_NETWORK_RECEIVE_BUFFER_SIZE
    #define AWS_IOT_NETWORK_RECEIVE_BUFFER_SIZE    ( 1024 )
#endif
#ifndef AWS_IOT_NETWORK_SHORT_TIMEOUT_MS
    #define AWS_IOT_NETWORK_SHORT_TIMEOUT_MS       ( 100 )
#endif
#ifndef AWS_IOT_NETWORK_SHUTDOWN_POLL_MS
    #define AWS_IOT_NETWORK_SHUTDOWN_POLL_MS       ( 1000 )
#endif
/** @endcond */

/**
 * @brief The bit to set when a connection's socket is shut down.
 */
#define _FLAG_SHUTDOWN               ( 1 )

/**
 * @brief The bit to set when a connection's receive task exits.
 */
#define _FLAG_RECEIVE_TASK_EXITED    ( 2 )

/*-----------------------------------------------------------*/

/**
 * @brief Connection info, which is pointed to by the connection handle.
 *
 * Holds data on a single connection.
 */
typedef struct NetworkConnection
{
    Socket_t xSocket;                                 /**< @brief Socket associated with connection. */
    StaticEventGroup_t xConnectionFlags;              /**< @brief Tracks whether the connection has been shut down or the receive task has exited. */
    AwsIotMqttConnection_t * pxMqttConnection;        /**< @brief MQTT connection handle associated with network connection. */
    uint8_t * pucReceiveBuffer;                       /**< @brief Buffer to hold incoming network data. */
    AwsIotMqttReceiveCallback_t xMqttReceiveCallback; /**< @brief MQTT receive callback function, if any. */
} NetworkConnection_t;

/*-----------------------------------------------------------*/

/**
 * @brief Reallocate a network receive buffer; data is also copied from the old
 * buffer into the new buffer.
 *
 * @param[in] pucOldBuffer The current network receive buffer.
 * @param[in] xOldBufferSize The size (in bytes) of pucOldBuffer.
 * @param[in] xNewBufferSize Requested size of the reallocated buffer.
 * @param[in] xCopyOffset The offset in pucOldBuffer from which data should be
 * copied into the new buffer.
 *
 * @return Pointer to a new, reallocated buffer; NULL if buffer reallocation failed.
 */
static uint8_t * prvReallocReceiveBuffer( uint8_t * pucOldBuffer,
                                          size_t xOldBufferSize,
                                          size_t xNewBufferSize,
                                          size_t xCopyOffset )
{
    uint8_t * pucNewBuffer = NULL;

    /* This function should not be called with a smaller new buffer size or a
     * copy offset greater than the old buffer size. */
    configASSERT( xNewBufferSize >= xOldBufferSize );
    configASSERT( xCopyOffset < xOldBufferSize );

    /* Allocate a larger receive buffer. */
    pucNewBuffer = AwsIotNetwork_Malloc( xNewBufferSize );

    /* Copy data into the new buffer. */
    if( pucNewBuffer != NULL )
    {
        ( void ) memcpy( pucNewBuffer,
                         pucOldBuffer + xCopyOffset,
                         xOldBufferSize - xCopyOffset );
    }

    /* Free the old buffer. */
    AwsIotNetwork_Free( pucOldBuffer );

    return pucNewBuffer;
}

/*-----------------------------------------------------------*/

/**
 * @brief Block on a socket and wait for incoming data.
 *
 * This function simulates the use of poll() on a socket, then reads the data
 * available.
 * @param[in] pxConnection The connection to receive data from.
 * @param[out] pucReceiveBuffer The buffer into which the received data will be placed.
 * @param[in] xReceiveBufferSize The size of pucReceiveBuffer.
 *
 * @return The number of bytes read into pucReceiveBuffer; a negative value on error.
 */
static int32_t prvReadSocketData( NetworkConnection_t * pxConnection,
                                  uint8_t * const pucReceiveBuffer,
                                  size_t xReceiveBufferSize )
{
    TickType_t xReceiveTimeout = 0;
    int32_t lSocketReturn = 0;

    /* Set a long timeout to wait for either a byte to be received or for
     * a socket shutdown. */
    xReceiveTimeout = pdMS_TO_TICKS( AWS_IOT_NETWORK_SHUTDOWN_POLL_MS );

    lSocketReturn = SOCKETS_SetSockOpt( pxConnection->xSocket,
                                        0,
                                        SOCKETS_SO_RCVTIMEO,
                                        &xReceiveTimeout,
                                        sizeof( TickType_t ) );

    if( lSocketReturn == SOCKETS_ERROR_NONE )
    {
        /* Block until a single byte is received from the socket or the socket
         * is shut down. */
        while( ( lSocketReturn = SOCKETS_Recv( pxConnection->xSocket,
                                               pucReceiveBuffer,
                                               1,
                                               0 ) ) == 0 )
        {
            /* Check the shutdown flag. */
            if( ( xEventGroupGetBits( &( pxConnection->xConnectionFlags ) )
                  & _FLAG_SHUTDOWN ) == _FLAG_SHUTDOWN )
            {
                /* Socket is shut down; this task will exit. */
                lSocketReturn = SOCKETS_ECLOSED;
                break;
            }
        }
    }

    /* Once a single byte is successfully read from the socket, set a short timeout
     * to read the rest of the data. */
    if( lSocketReturn == 1 )
    {
        xReceiveTimeout = pdMS_TO_TICKS( AWS_IOT_NETWORK_SHORT_TIMEOUT_MS );

        lSocketReturn = SOCKETS_SetSockOpt( pxConnection->xSocket,
                                            0,
                                            SOCKETS_SO_RCVTIMEO,
                                            &xReceiveTimeout,
                                            sizeof( TickType_t ) );
    }

    /* Read all data currently available on the socket. */
    if( lSocketReturn == SOCKETS_ERROR_NONE )
    {
        lSocketReturn = SOCKETS_Recv( pxConnection->xSocket,
                                      pucReceiveBuffer + 1,
                                      xReceiveBufferSize - 1,
                                      0 );

        /* Add the length of the single byte initially read with an infinite timeout. */
        if( lSocketReturn >= 0 )
        {
            lSocketReturn += 1;
        }
    }

    return lSocketReturn;
}

/*-----------------------------------------------------------*/

/**
 * @brief The MQTT network receive task.
 *
 * Receives data on a socket, then calls the MQTT receive callback to process
 * the incoming data.
 * @param[in] pvArgument The network connection data associated with this task.
 */
void prvMqttReceiveTask( void * pvArgument )
{
    NetworkConnection_t * pxConnection = ( NetworkConnection_t * ) pvArgument;
    int32_t lSocketReturn = 0;
    ssize_t xMqttCallbackReturn = 0;
    size_t xProcessOffset = 0, xReceiveBufferSize = AWS_IOT_NETWORK_RECEIVE_BUFFER_SIZE;

    /* A receive buffer should have been allocated. */
    configASSERT( pxConnection->pucReceiveBuffer != NULL );

    while( true )
    {
        /* An offset larger than the buffer size should not have been calculated. */
        configASSERT( xProcessOffset < xReceiveBufferSize );

        /* Block and wait for data from the socket. */
        lSocketReturn = prvReadSocketData( pxConnection,
                                           pxConnection->pucReceiveBuffer + xProcessOffset,
                                           xReceiveBufferSize - xProcessOffset );

        if( lSocketReturn <= 0 )
        {
            AwsIotLogError( "No data could be read from the socket. Returned %ld.",
                            lSocketReturn );
            break;
        }

        /* Pass the received data to the MQTT callback. */
        xMqttCallbackReturn = pxConnection->xMqttReceiveCallback( pxConnection->pxMqttConnection,
                                                                  pxConnection->pucReceiveBuffer,
                                                                  0,
                                                                  ( size_t ) lSocketReturn + xProcessOffset,
                                                                  AwsIotNetwork_Free );

        /* Check for MQTT protocol violation. */
        if( xMqttCallbackReturn == -1 )
        {
            AwsIotLogError( "MQTT protocol violation encountered." );
            break;
        }
        /* If 0 bytes were processed, the receive buffer is too small for a complete MQTT packet. */
        else if( xMqttCallbackReturn == 0 )
        {
            AwsIotLogDebug( "Receive buffer too small. Allocating larger buffer." );

            /* Check that it's possible to allocate a larger buffer. */
            if( xReceiveBufferSize * 2 < xReceiveBufferSize )
            {
                AwsIotLogError( "No larger receive buffer can be allocated." );

                break;
            }

            /* Allocate a larger receive buffer. */
            pxConnection->pucReceiveBuffer = prvReallocReceiveBuffer( pxConnection->pucReceiveBuffer,
                                                                      xReceiveBufferSize,
                                                                      xReceiveBufferSize * 2,
                                                                      0 );

            /* Update buffer offset and size. */
            if( pxConnection->pucReceiveBuffer != NULL )
            {
                xProcessOffset = xReceiveBufferSize;
                xReceiveBufferSize *= 2;
            }
            else
            {
                /* Failed to reallocate receive buffer; exit task. */
                break;
            }
        }
        /* If the entire buffer wasn't processed, then it contains a partial packet at the end. */
        else if( xMqttCallbackReturn < ( ssize_t ) lSocketReturn + ( ssize_t ) xProcessOffset )
        {
            AwsIotLogDebug( "MQTT library processed %ld of %ld bytes.",
                            ( long int ) xMqttCallbackReturn,
                            ( long int ) lSocketReturn + ( long int ) xProcessOffset );

            /* Allocate a new buffer and copy the partial packet to the beginning. */
            pxConnection->pucReceiveBuffer = prvReallocReceiveBuffer( pxConnection->pucReceiveBuffer,
                                                                      xReceiveBufferSize,
                                                                      xReceiveBufferSize,
                                                                      ( size_t ) xMqttCallbackReturn );

            /* Check that reallocation succeeded. */
            if( pxConnection->pucReceiveBuffer == NULL )
            {
                break;
            }

            xProcessOffset = ( size_t ) lSocketReturn + xProcessOffset - ( size_t ) xMqttCallbackReturn;
        }
        /* If this block is reached, then the entire buffer was successfully processed. */
        else
        {
            /* Allocate a new receive buffer. The MQTT library will free the old one. */
            pxConnection->pucReceiveBuffer = AwsIotNetwork_Malloc( xReceiveBufferSize );

            /* Check that allocation succeeded. */
            if( pxConnection->pucReceiveBuffer == NULL )
            {
                break;
            }

            xProcessOffset = 0;
        }
    }

    AwsIotLogDebug( "MQTT receive task terminating." );

    /* If a receive buffer is allocated, free it. */
    if( pxConnection->pucReceiveBuffer != NULL )
    {
        AwsIotNetwork_Free( pxConnection->pucReceiveBuffer );
    }

    /* Set the receive task exited flag. */
    ( void ) xEventGroupSetBits( &( pxConnection->xConnectionFlags ),
                                 _FLAG_RECEIVE_TASK_EXITED );

    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/

AwsIotNetworkError_t AwsIotNetwork_Init( void )
{
    /* No additional initialization is needed on Amazon FreeRTOS. */
    return AWS_IOT_NETWORK_SUCCESS;
}

/*-----------------------------------------------------------*/

void AwsIotNetwork_Cleanup( void )
{
    /* No cleanup is needed on Amazon FreeRTOS. */
}

/*-----------------------------------------------------------*/

AwsIotNetworkError_t AwsIotNetwork_CreateConnection( AwsIotNetworkConnection_t * const pNetworkConnection,
                                                     const char * const pHostName,
                                                     uint16_t port,
                                                     const AwsIotNetworkTlsInfo_t * const pTlsInfo )
{
    AwsIotNetworkError_t xStatus = AWS_IOT_NETWORK_SUCCESS;
    NetworkConnection_t * pxConnection = NULL;
    Socket_t xSocket = SOCKETS_INVALID_SOCKET;
    SocketsSockaddr_t xServer = { 0 };
    size_t xHostNameLength = strlen( pHostName );
    const char * ppcALPNProtos[] = { socketsAWS_IOT_ALPN_MQTT };

    /* Check URL length. */
    if( xHostNameLength > ( size_t ) securesocketsMAX_DNS_NAME_LENGTH )
    {
        AwsIotLogError( "URL exceeds %d, which is the maximum allowed length.",
                        securesocketsMAX_DNS_NAME_LENGTH );
        xStatus = AWS_IOT_NETWORK_BAD_PARAMETER;
    }

    /* Allocate memory for new connection. */
    if( xStatus == AWS_IOT_NETWORK_SUCCESS )
    {
        pxConnection = AwsIotNetwork_Malloc( sizeof( NetworkConnection_t ) );

        if( pxConnection == NULL )
        {
            AwsIotLogError( "Failed to allocate memory for new network connection." );

            xStatus = AWS_IOT_NETWORK_NO_MEMORY;
        }
        else
        {
            ( void ) memset( pxConnection, 0x00, sizeof( NetworkConnection_t ) );
        }
    }

    /* Create new TCP socket. */
    if( xStatus == AWS_IOT_NETWORK_SUCCESS )
    {
        xSocket = SOCKETS_Socket( SOCKETS_AF_INET,
                                  SOCKETS_SOCK_STREAM,
                                  SOCKETS_IPPROTO_TCP );

        if( xSocket == SOCKETS_INVALID_SOCKET )
        {
            AwsIotLogError( "Failed to create new socket." );
            xStatus = AWS_IOT_NETWORK_NO_MEMORY;
        }
    }

    /* Set secured option. */
    if( xStatus == AWS_IOT_NETWORK_SUCCESS )
    {
        if( pTlsInfo != NULL )
        {
            if( SOCKETS_SetSockOpt( xSocket,
                                    0,
                                    SOCKETS_SO_REQUIRE_TLS,
                                    NULL,
                                    0 ) != SOCKETS_ERROR_NONE )
            {
                AwsIotLogError( "Failed to set secured option for new connection." );
                xStatus = AWS_IOT_NETWORK_SYSTEM_ERROR;
            }
        }
    }

    /* Set ALPN option. */
    if( ( xStatus == AWS_IOT_NETWORK_SUCCESS ) &&
        ( pTlsInfo != NULL ) )
    {
        if( pTlsInfo->pAlpnProtos != NULL )
        {
            if( SOCKETS_SetSockOpt( xSocket,
                                    0,
                                    SOCKETS_SO_ALPN_PROTOCOLS,
                                    ppcALPNProtos,
                                    sizeof( ppcALPNProtos ) / sizeof( ppcALPNProtos[ 0 ] ) ) != SOCKETS_ERROR_NONE )
            {
                AwsIotLogError( "Failed to set ALPN option for new connection." );
                xStatus = AWS_IOT_NETWORK_SYSTEM_ERROR;
            }
        }
    }

    /* Set SNI option. */
    if( ( xStatus == AWS_IOT_NETWORK_SUCCESS ) &&
        ( pTlsInfo != NULL ) )
    {
        if( pTlsInfo->disableSni == false )
        {
            if( SOCKETS_SetSockOpt( xSocket,
                                    0,
                                    SOCKETS_SO_SERVER_NAME_INDICATION,
                                    pHostName,
                                    xHostNameLength + 1 ) != SOCKETS_ERROR_NONE )
            {
                AwsIotLogError( "Failed to set SNI option for new connection." );
                xStatus = AWS_IOT_NETWORK_SYSTEM_ERROR;
            }
        }
    }

    /* Set custom server certificate. */
    if( ( xStatus == AWS_IOT_NETWORK_SUCCESS ) &&
        ( pTlsInfo != NULL ) )
    {
        if( pTlsInfo->pRootCa != NULL )
        {
            if( SOCKETS_SetSockOpt( xSocket,
                                    0,
                                    SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE,
                                    pTlsInfo->pRootCa,
                                    pTlsInfo->rootCaLength ) != SOCKETS_ERROR_NONE )
            {
                AwsIotLogError( "Failed to set server certificate option for new connection." );
                xStatus = AWS_IOT_NETWORK_SYSTEM_ERROR;
            }
        }
    }

    /* Establish connection. */
    if( xStatus == AWS_IOT_NETWORK_SUCCESS )
    {
        xServer.ucSocketDomain = SOCKETS_AF_INET;
        xServer.usPort = SOCKETS_htons( port );
        xServer.ulAddress = SOCKETS_GetHostByName( pHostName );

        if( SOCKETS_Connect( xSocket,
                             &xServer,
                             sizeof( SocketsSockaddr_t ) ) != SOCKETS_ERROR_NONE )
        {
            AwsIotLogError( "Failed to establish new connection." );
            xStatus = AWS_IOT_NETWORK_SYSTEM_ERROR;
        }
    }

    /* Set output parameter. */
    if( xStatus == AWS_IOT_NETWORK_SUCCESS )
    {
        /* Initialize connection flags. This function will not fail when its
         * parameter isn't NULL. */
        ( void ) xEventGroupCreateStatic( &( pxConnection->xConnectionFlags ) );

        AwsIotLogInfo( "New network connection established." );
        pxConnection->xSocket = xSocket;
        *pNetworkConnection = pxConnection;
    }
    /* Clean up on failure. */
    else
    {
        if( pxConnection != NULL )
        {
            AwsIotNetwork_Free( pxConnection );
        }

        if( xSocket != SOCKETS_INVALID_SOCKET )
        {
            SOCKETS_Close( xSocket );
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

AwsIotNetworkError_t AwsIotNetwork_SetMqttReceiveCallback( AwsIotNetworkConnection_t networkConnection,
                                                           AwsIotMqttConnection_t * pMqttConnection,
                                                           AwsIotMqttReceiveCallback_t receiveCallback )
{
    AwsIotNetworkError_t xStatus = AWS_IOT_NETWORK_SUCCESS;
    NetworkConnection_t * pxConnection = ( NetworkConnection_t * ) networkConnection;

    /* Set the parameters for the receive task. */
    pxConnection->pxMqttConnection = pMqttConnection;
    pxConnection->xMqttReceiveCallback = receiveCallback;

    /* No receive buffer should be allocated, and no flags should be set. */
    configASSERT( pxConnection->pucReceiveBuffer == NULL );
    configASSERT( xEventGroupGetBits( &( pxConnection->xConnectionFlags ) ) == 0 );

    /* Allocate initial network receive buffer. */
    pxConnection->pucReceiveBuffer = AwsIotNetwork_Malloc( AWS_IOT_NETWORK_RECEIVE_BUFFER_SIZE );

    if( pxConnection->pucReceiveBuffer == NULL )
    {
        xStatus = AWS_IOT_NETWORK_NO_MEMORY;
    }

    /* Create the receive task. */
    if( xStatus == AWS_IOT_NETWORK_SUCCESS )
    {
        if( xTaskCreate( prvMqttReceiveTask,
                         "MqttRecv",
                         AWS_IOT_MQTT_RECEIVE_TASK_STACK_SIZE,
                         pxConnection,
                         AWS_IOT_MQTT_RECEIVE_TASK_PRIORITY,
                         NULL ) != pdPASS )
        {
            AwsIotLogError( "Failed to create MQTT receive task." );
            AwsIotNetwork_Free( pxConnection->pucReceiveBuffer );
            xStatus = AWS_IOT_NETWORK_SYSTEM_ERROR;
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

void AwsIotNetwork_CloseConnection( AwsIotNetworkConnection_t networkConnection )
{
    NetworkConnection_t * pxConnection = ( NetworkConnection_t * ) networkConnection;

    /* Call Secure Sockets shutdown function to close connection. */
    if( SOCKETS_Shutdown( pxConnection->xSocket,
                          SOCKETS_SHUT_RDWR ) != SOCKETS_ERROR_NONE )
    {
        AwsIotLogWarn( "Failed to close connection." );
    }

    /* Set the shutdown flag. */
    ( void ) xEventGroupSetBits( &( pxConnection->xConnectionFlags ),
                                 _FLAG_SHUTDOWN );
}

/*-----------------------------------------------------------*/

void AwsIotNetwork_DestroyConnection( AwsIotNetworkConnection_t networkConnection )
{
    NetworkConnection_t * pxConnection = ( NetworkConnection_t * ) networkConnection;

    /* Wait for the receive task to exit. */
    ( void ) xEventGroupWaitBits( &( pxConnection->xConnectionFlags ),
                                  _FLAG_RECEIVE_TASK_EXITED,
                                  pdTRUE,
                                  pdTRUE,
                                  portMAX_DELAY );

    /* Call Secure Sockets close function to free resources. */
    if( SOCKETS_Close( pxConnection->xSocket ) != SOCKETS_ERROR_NONE )
    {
        AwsIotLogWarn( "Failed to destroy connection." );
    }

    AwsIotNetwork_Free( pxConnection );
}

/*-----------------------------------------------------------*/

size_t AwsIotNetwork_Send( void * networkConnection,
                           const void * const pMessage,
                           size_t messageLength )
{
    NetworkConnection_t * pxConnection = ( NetworkConnection_t * ) networkConnection;
    int32_t lSocketReturn = 0;
    size_t xReturn = 0;

    /* Send the message. */
    lSocketReturn = SOCKETS_Send( pxConnection->xSocket,
                                  pMessage,
                                  messageLength,
                                  0 );

    /* Return the number of bytes sent. */
    if( lSocketReturn > 0 )
    {
        xReturn = ( size_t ) lSocketReturn;
    }

    return xReturn;
}

/*-----------------------------------------------------------*/
