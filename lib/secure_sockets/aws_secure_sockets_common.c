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

/* Secure Sockets includes. */
#include "aws_secure_sockets.h"
#include "aws_secure_sockets_internal.h"

#include "aws_iot_metrics.h"

/*-----------------------------------------------------------*/

/*
 * Interface routines.
 */

int32_t SOCKETS_Close( Socket_t xSocket )
{
    return SocketsInternal_Close( xSocket );
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Connect( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t xAddressLength )
{
    int32_t result = SocketsInternal_Connect( xSocket, pxAddress, xAddressLength );

    if( result == SOCKETS_ERROR_NONE )
    {
        IotMetricsTcpConnection_t connection;
        /* Cast the Socket_t to void pointer as handle. */
        connection.pHandle = ( void * ) xSocket;
        connection.remotePort = SOCKETS_ntohs(pxAddress->usPort);
        connection.remoteIP = pxAddress->ulAddress;

        IotMetrics_AddTcpConnection( &connection );
    }

    return result;
}
/*-----------------------------------------------------------*/

uint32_t SOCKETS_GetHostByName( const char * pcHostName )
{
    return SocketsInternal_GetHostByName( pcHostName );
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Recv( Socket_t xSocket,
                      void * pvBuffer,
                      size_t xBufferLength,
                      uint32_t ulFlags )
{
    return SocketsInternal_Recv( xSocket, pvBuffer, xBufferLength, ulFlags );
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Send( Socket_t xSocket,
                      const void * pvBuffer,
                      size_t xDataLength,
                      uint32_t ulFlags )
{
    return SocketsInternal_Send( xSocket, pvBuffer, xDataLength, ulFlags );
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_SetSockOpt( Socket_t xSocket,
                            int32_t lLevel,
                            int32_t lOptionName,
                            const void * pvOptionValue,
                            size_t xOptionLength )
{
    return SocketsInternal_SetSockOpt( xSocket, lLevel, lOptionName, pvOptionValue, xOptionLength );
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Shutdown( Socket_t xSocket,
                          uint32_t ulHow )
{
    int32_t result = SocketsInternal_Shutdown( xSocket, ulHow );

    if( result == SOCKETS_ERROR_NONE )
    {
        IotMetrics_RemoveTcpConnection( (void *)xSocket );
    }

    return result;
}
/*-----------------------------------------------------------*/

Socket_t SOCKETS_Socket( int32_t lDomain,
                         int32_t lType,
                         int32_t lProtocol )
{
    return SocketsInternal_Socket( lDomain, lType, lProtocol );
}

/*-----------------------------------------------------------*/

BaseType_t SOCKETS_Init( void )
{
    IotMetrics_Init();

    return SocketsInternal_Init();
}
