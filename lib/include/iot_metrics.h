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
 * @file iot_metrics.h
 * @brief User-facing functions and structs of Metrics library
 *
 */

#ifndef _IOT_METRICS_H_
#define _IOT_METRICS_H_

#include <stdint.h>
#include "iot_linear_containers.h"

/**
 * @def IotMetrics_Assert( expression )
 * @brief Assertion macro for the Metrics library.
 *
 * @param[in] expression Expression to be evaluated.
 */
#if AWS_IOT_METRICS_ENABLE_ASSERTS == 1
    #ifndef IotMetrics_Assert
        #include <assert.h>
        #define IotMetrics_Assert( expression )    assert( expression )
    #endif
#else
    #define IotMetrics_Assert( expression )
#endif

/*
 * Provide default values for undefined memory allocation functions based on
 * the usage of dynamic memory allocation.
 */
#if AWS_IOT_STATIC_MEMORY_ONLY == 1
    #include "platform/aws_iot_static_memory.h"

/**
 * @brief Allocate an array of uint8_t. This function should have the same
 * signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    #ifndef AwsIotMetrics_MallocTcpConnection
        #define AwsIotMetrics_MallocTcpConnection    AwsIot_MallocMetricsTcpConnection
    #endif

/**
 * @brief Free an array of uint8_t. This function should have the same
 * signature as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    #ifndef AwsIotMetrics_FreeTcpConnection
        #define AwsIotMetrics_FreeTcpConnection    AwsIot_FreeMetricsTcpConnection
    #endif

#else /* if AWS_IOT_STATIC_MEMORY_ONLY */
    #include <stdlib.h>

    #ifndef AwsIotMetrics_MallocTcpConnection
        #define AwsIotMetrics_MallocTcpConnection    malloc
    #endif

    #ifndef AwsIotMetrics_FreeTcpConnection
        #define AwsIotMetrics_FreeTcpConnection    free
    #endif

#endif /* if AWS_IOT_STATIC_MEMORY_ONLY */

/**
 * @page Metrics_constants Constants
 * @brief Defined constants of the Metrics library.
 * - @ref MetricsTcpConnectionOffset "TCP connections offset"
 */

/**
 * @anchor MetricsTcpConnectionOffset
 * @name TCP connections offset
 *
 * @brief Offset constant used for list operations.
 */
#define IOT_METRICS_TCP_CONNECTION_OFFSET    AwsIotLink_Offset( IotMetricsTcpConnection_t, link )

/**
 * Data handle of TCP connection
 */
typedef struct IotMetricsTcpConnection
{
    void * pHandle;
    uint16_t remotePort;
    uint32_t remoteIP;
    IotLink_t link;
} IotMetricsTcpConnection_t;

/**
 * Callback data handle to process specific metrics.
 */
typedef struct IotMetricsListCallback
{
    void * param1;
    void ( * function )( void * param1,
                         IotListDouble_t * pMetricsList ); /* pMetricsList is guaranteed a valid metrics list. */
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
void IotMetrics_ProcessTcpConnections( IotMetricsListCallback_t tcpConnectionsCallback );

#endif /* ifndef _IOT_METRICS_H_ */
