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

#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

#include "iot_linear_containers.h"

/**
 * @def IotMetrics_Assert( expression )
 * @brief Assertion macro for the Metrics library.
 *
 * @param[in] expression Expression to be evaluated.
 */
#if IOT_METRICS_ENABLE_ASSERTS == 1
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
#if IOT_STATIC_MEMORY_ONLY == 1
    #include "private/iot_static_memory.h"

/**
 * @brief Allocate an array of uint8_t. This function should have the same
 * signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    #ifndef IotMetrics_MallocTcpConnection
        #define IotMetrics_MallocTcpConnection    Iot_MallocMetricsTcpConnection
    #endif

/**
 * @brief Free an array of uint8_t. This function should have the same
 * signature as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    #ifndef IotMetrics_FreeTcpConnection
        #define IotMetrics_FreeTcpConnection    Iot_FreeMetricsTcpConnection
    #endif

#else /* if IOT_STATIC_MEMORY_ONLY */
    #include <stdlib.h>

    #ifndef IotMetrics_MallocTcpConnection
        #define IotMetrics_MallocTcpConnection    malloc
    #endif

    #ifndef IotMetrics_FreeTcpConnection
        #define IotMetrics_FreeTcpConnection    free
    #endif

#endif /* if IOT_STATIC_MEMORY_ONLY */

#ifndef IotMetricsConnectionId_t
    #define IotMetricsConnectionId_t    uint32_t
#endif

/**
 * Data handle of TCP connection
 */
typedef struct IotMetricsTcpConnection
{
    IotLink_t link;
    IotMetricsConnectionId_t id;
    char * pRemoteIP;    /* This is limited to IPv4. */
    uint16_t remotePort; /* In host order. */
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
bool IotMetrics_Init();

/**
 * Record one TCP connection metric.
 */
void IotMetrics_AddTcpConnection( IotMetricsTcpConnection_t * pTcpConnection );

/**
 * Remove one TCP connection metric.
 */
void IotMetrics_RemoveTcpConnection( IotMetricsConnectionId_t tcpConnectionId );

/**
 * Use a callback to process a list of TCP connections
 */
void IotMetrics_ProcessTcpConnections( IotMetricsListCallback_t tcpConnectionsCallback );

#endif /* ifndef _IOT_METRICS_H_ */
