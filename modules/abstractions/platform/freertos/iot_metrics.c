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

/*
 * The implementation is Amazon Freertos specific. It depends on aws_secure_sockets library.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdio.h>

/* Metrics include. */
#include "platform/iot_metrics.h"

/* Platform threads include. */
#include "platform/iot_threads.h"

/* Secure sockets include. */
#include "aws_secure_sockets.h"

/* Compare function to identify the TCP connection id. */
static bool _tcpConnectionMatch( const IotLink_t * pLink,
                                 void * pId );

/*------------------- Global Variables ------------------------*/

static IotListDouble_t _connectionsList = IOT_LIST_DOUBLE_INITIALIZER;
static IotMutex_t _mutex;

/*-----------------------------------------------------------*/

static bool _tcpConnectionMatch( const IotLink_t * pLink,
                                 void * pId )
{
    return false;
}

/*-----------------------------------------------------------*/

bool IotMetrics_Init()
{
    bool result = false;

    if( IotMutex_Create( &_mutex, false ) )
    {
        IotListDouble_Create( &_connectionsList );
        result = true;
    }

    return result;
}

/*-----------------------------------------------------------*/

void IotMetrics_AddTcpConnection( IotMetricsTcpConnection_t * pTcpConnection )
{
}

/*-----------------------------------------------------------*/

void IotMetrics_RemoveTcpConnection( IotMetricsConnectionId_t tcpConnectionId )
{
}

/*-----------------------------------------------------------*/

void IotMetrics_GetTcpConnections( void * pContext,
                                   void ( * metricsCallback )( void *, const IotListDouble_t * ) )
{
}
