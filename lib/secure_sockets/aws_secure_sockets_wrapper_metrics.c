/*
 * Amazon FreeRTOS Secure Socket for FreeRTOS+TCP V1.1.4
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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/*
 * This file implements common logic for portable secure sockets implemntations.
 * It updates metrics of sockets in relevant socket functions.
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Define _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE to prevent secure sockets functions
 * from redefining in aws_secure_sockets_wrapper_metrics.h */
#define _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE

/* Secure Sockets includes. */
#include "aws_secure_sockets.h"

/* Metrics includes. */
#include "iot_metrics.h"

#undef _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE

#ifdef AWS_IOT_SECURE_SOCKETS_METRICS_ENABLED

/*-----------------------------------------------------------*/

    int32_t Sockets_MetricsConnect( Socket_t xSocket,
                                    SocketsSockaddr_t * pxAddress,
                                    Socklen_t xAddressLength )
    {
        int32_t result = SOCKETS_Connect( xSocket, pxAddress, xAddressLength );

        if( result == SOCKETS_ERROR_NONE )
        {
            IotMetricsTcpConnection_t connection;
            /* Cast the Socket_t to void pointer as handle. */
            connection.pHandle = ( void * ) xSocket;

            /* The port passed to SocketsSockaddr_t is with network endian.
             * Therefor it must convert back for metrics: from network endian to host endian. */
            connection.remotePort = SOCKETS_ntohs( pxAddress->usPort );

            /* The IP passed to SocketsSockaddr_t is with network endian.
             * Therefor it must convert back for metrics: from network endian to host endian. */
            connection.remoteIP = SOCKETS_ntohl( pxAddress->ulAddress );

            IotMetrics_AddTcpConnection( &connection );
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
            IotMetrics_RemoveTcpConnection( ( void * ) xSocket );
        }

        return result;
    }

#endif /* ifdef AWS_IOT_SECURE_SOCKETS_METRICS_ENABLED */
