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

#ifndef _AWS_IOT_METRICS_H_
#define _AWS_IOT_METRICS_H_

#include <stdint.h>
#include "aws_iot_queue.h"

#define IOT_METRICS_TCP_CONNECTION_OFFSET    AwsIotLink_Offset( IotMetricsTcpConnection_t, link )

#ifndef IOT_METRICS_ENABLED
    #define IOT_METRICS_ENABLED              ( 1 )
#endif

/**
 * Data handle of TCP connection
 */
typedef struct IotMetricsTcpConnection
{
    void * pHandle;
    uint16_t remotePort;
    uint32_t remoteIP;
    AwsIotLink_t link;
} IotMetricsTcpConnection_t;

/**
 * Callback data handle to process specific metrics.
 */
typedef struct IotMetricsListCallback
{
    void * param1;
    void * ( *function )( void * param1, AwsIotList_t * pMetricsList );
} IotMetricsListCallback_t;

/**
 * This function must be called before any other functions.
 */
BaseType_t IotMetrics_Init();

/**
 * Record one TCP connection metric.
 */
void IotMetrics_AddTcpConnection( IotMetricsTcpConnection_t * pTcpConnection );

/**
 * Remove one TCP connection metric.
 */
void IotMetrics_RemoveTcpConnection( void * pTcpConnectionHandle );

/**
 * Use a callback to process a list of TCP connections
 */
void * IotMetrics_ProcessTcpConnections( IotMetricsListCallback_t tcpConnectionsCallback );

#endif /* ifndef _AWS_IOT_DEFENDER_METRICS_SOCKET_H_ */
