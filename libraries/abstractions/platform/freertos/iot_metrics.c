/*
 * FreeRTOS Platform V1.1.2
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

/*
 * The implementation is FreeRTOS specific. It depends on aws_secure_sockets library.
 */

/* Define _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE to prevent secure sockets functions
 * from redefining in iot_secure_sockets_wrapper_metrics.h */
#define _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* Metrics include. */
#include "platform/iot_metrics.h"

/* Platform threads include. */
#include "platform/iot_threads.h"

/* Secure sockets include. */
#include "iot_secure_sockets.h"

#undef _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE

#if AWS_IOT_SECURE_SOCKETS_METRICS_ENABLED == 1

/**
 * @brief Used to match metrics connection records by network connection.
 *
 * @param[in] pConnectionLink Pointer to a metrics connection record's link element.
 * @param[in] pContext The network connection to match.
 *
 * @return `true` if the given metrics connection record matches the given
 * network connection; `false` otherwise.
 */
    static bool _connectionMatch( const IotLink_t * pConnectionLink,
                                  void * pContext );

    static void _metricsAddTcpConnection( Socket_t xSocket,
                                          SocketsSockaddr_t * pxAddress );

/*------------------- Global Variables ------------------------*/

/**
 * @brief Holds a list of active TCP connections.
 */
    static IotListDouble_t _connectionList = IOT_LIST_DOUBLE_INITIALIZER;

/**
 * @brief Protects #_connectionList from concurrent access.
 */
    static IotMutex_t _connectionListMutex;

/*-----------------------------------------------------------*/

    static bool _connectionMatch( const IotLink_t * pConnectionLink,
                                  void * pContext )
    {
        /* Retrieve the pointer the the TCP connection record. The given link
         * pointer is never NULL. */
        IotMetricsTcpConnection_t * pTcpConnection = IotLink_Container( IotMetricsTcpConnection_t,
                                                                        pConnectionLink,
                                                                        link );

        return( pTcpConnection->pNetworkContext == pContext );
    }

/*-----------------------------------------------------------*/

    bool IotMetrics_Init( void )
    {
        IotListDouble_Create( &_connectionList );

        return IotMutex_Create( &_connectionListMutex, false );
    }

/*-----------------------------------------------------------*/

    void IotMetrics_Cleanup( void )
    {
        IotMutex_Destroy( &_connectionListMutex );
    }

/*-----------------------------------------------------------*/

    void IotMetrics_GetTcpConnections( void * pContext,
                                       void ( * metricsCallback )( void *, const IotListDouble_t * ) )
    {
        /* Provide the connection list. Ensure that it is not modified elsewhere by
         * locking the connection list mutex. */
        IotMutex_Lock( &_connectionListMutex );
        metricsCallback( pContext, &_connectionList );
        IotMutex_Unlock( &_connectionListMutex );
    }

/*-----------------------------------------------------------*/

    static void _metricsAddTcpConnection( Socket_t xSocket,
                                          SocketsSockaddr_t * pxAddress )
    {
        IotMetricsTcpConnection_t * pTcpConnection = NULL;
        void * pSocketContext = ( void * ) xSocket;

        IotMutex_Lock( &_connectionListMutex );

        /* Only add if it doesn't exist in the _connectionList. */
        if( IotListDouble_FindFirstMatch( &_connectionList,
                                          NULL,
                                          _connectionMatch,
                                          pSocketContext ) == NULL )
        {
            /* Allocate memory for a new metrics connection. */
            pTcpConnection = IotMetrics_MallocTcpConnection( sizeof( IotMetricsTcpConnection_t ) );

            if( pTcpConnection != NULL )
            {
                ( void ) memset( pTcpConnection, 0x00, sizeof( IotMetricsTcpConnection_t ) );

                pTcpConnection->pNetworkContext = pSocketContext;

                /* Convert IP to string. */
                SOCKETS_inet_ntoa( pxAddress->ulAddress, pTcpConnection->pRemoteAddress );

                /* Formatting. */
                sprintf( pTcpConnection->pRemoteAddress + strlen( pTcpConnection->pRemoteAddress ), ":%d", SOCKETS_ntohs( pxAddress->usPort ) );

                pTcpConnection->addressLength = strlen( pTcpConnection->pRemoteAddress );

                /* Insert to the list. */
                IotListDouble_InsertTail( &_connectionList, &( pTcpConnection->link ) );
            }
        }

        IotMutex_Unlock( &_connectionListMutex );
    }

/*-----------------------------------------------------------*/

    static void _metricsRemoveTcpConnection( Socket_t xSocket )
    {
        IotMutex_Lock( &_connectionListMutex );

        IotLink_t * pFoundConnectionLink = IotListDouble_RemoveFirstMatch( &_connectionList,
                                                                           NULL,
                                                                           _connectionMatch,
                                                                           ( void * ) xSocket );

        if( pFoundConnectionLink != NULL )
        {
            IotMetricsTcpConnection_t * pFoundTcpConnection = IotLink_Container( IotMetricsTcpConnection_t, pFoundConnectionLink, link );

            IotMetrics_FreeTcpConnection( pFoundTcpConnection );
        }

        IotMutex_Unlock( &_connectionListMutex );
    }

/*-----------------------------------------------------------*/

    BaseType_t Sockets_MetricsInit( void )
    {
        BaseType_t result = SOCKETS_Init();

        if( result == pdTRUE )
        {
            result = IotMetrics_Init() ? pdTRUE : pdFALSE;
        }

        return result;
    }

/*-----------------------------------------------------------*/

    int32_t Sockets_MetricsConnect( Socket_t xSocket,
                                    SocketsSockaddr_t * pxAddress,
                                    Socklen_t xAddressLength )
    {
        int32_t result = SOCKETS_Connect( xSocket, pxAddress, xAddressLength );

        if( result == SOCKETS_ERROR_NONE )
        {
            _metricsAddTcpConnection( xSocket, pxAddress );
        }

        return result;
    }

/*-----------------------------------------------------------*/

    int32_t Sockets_MetricsShutdown( Socket_t xSocket,
                                     uint32_t ulHow )
    {
        int32_t result = SOCKETS_Shutdown( xSocket, ulHow );

        if( result == SOCKETS_ERROR_NONE )
        {
            _metricsRemoveTcpConnection( xSocket );
        }

        return result;
    }

#endif /* ifdef AWS_IOT_SECURE_SOCKETS_METRICS_ENABLED */
