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

/* The config header is always included first. */
#include "iot_config.h"

/* This file should only be compiled if dynamic memory allocation is forbidden. */
#if IOT_STATIC_MEMORY_ONLY == 1

/* Standard includes. */
    #include <stdbool.h>
    #include <stddef.h>
    #include <string.h>

/* Static memory include. */
    #include "private/iot_static_memory.h"

/* Metrics include. */
    #include "platform/iot_metrics.h"

    #ifndef IOT_METRICS_TCP_CONNECTIONS
        #define IOT_METRICS_TCP_CONNECTIONS    ( 10 )
    #endif

    #ifndef IOT_METRICS_IP_ADDRESSES
        #define IOT_METRICS_IP_ADDRESSES    ( 10 )
    #endif

/* Validate static memory configuration settings. */
    #if IOT_METRICS_TCP_CONNECTIONS <= 0
        #error "IOT_METRICS_TCP_CONNECTIONS cannot be 0 or negative."
    #endif

    #if IOT_METRICS_IP_ADDRESSES <= 0
        #error "IOT_METRICS_IP_ADDRESSES cannot be 0 or negative."
    #endif

    #define _METRICS_TCP_CONNECTION_SIZE    ( sizeof( IotMetricsTcpConnection_t ) * IOT_METRICS_TCP_CONNECTIONS )

/*-----------------------------------------------------------*/

/* Extern declarations of common static memory functions in iot_static_memory_common.c
 * Because these functions are specific to this static memory implementation, they are
 * not placed in the common static memory header file. */
    extern int IotStaticMemory_FindFree( bool * const pInUse,
                                         int limit );
    extern void IotStaticMemory_ReturnInUse( void * ptr,
                                             void * const pTcpConnection,
                                             bool * const pInUse,
                                             int limit,
                                             size_t elementSize );

/*-----------------------------------------------------------*/

/*
 * Static memory buffers and flags, allocated and zeroed at compile-time.
 */
    static bool _inUseTcpConnections[ IOT_METRICS_TCP_CONNECTIONS ] = { 0 };
    static IotMetricsTcpConnection_t _tcpConnections[ IOT_METRICS_TCP_CONNECTIONS ][ _METRICS_TCP_CONNECTION_SIZE ] = { { { .link = { 0 } } } };

    static bool _inUseIpAddresses[ IOT_METRICS_IP_ADDRESSES ] = { 0 };
    static char _ipAddresses[ IOT_METRICS_IP_ADDRESSES ][ IOT_METRICS_MAX_IP_STRING_LENGTH ] = { { 0 } };

/*-----------------------------------------------------------*/

    void * Iot_MallocMetricsTcpConnection( size_t size )
    {
        int freeIndex = -1;
        void * pNewTcpConnection = NULL;

        if( size <= _METRICS_TCP_CONNECTION_SIZE )
        {
            freeIndex = IotStaticMemory_FindFree( _inUseTcpConnections,
                                                  IOT_METRICS_TCP_CONNECTIONS );

            if( freeIndex != -1 )
            {
                pNewTcpConnection = &( _tcpConnections[ freeIndex ][ 0 ] );
            }
        }

        return pNewTcpConnection;
    }

/*-----------------------------------------------------------*/

    void Iot_FreeMetricsTcpConnection( void * ptr )
    {
        IotStaticMemory_ReturnInUse( ptr,
                                     _tcpConnections,
                                     _inUseTcpConnections,
                                     IOT_METRICS_TCP_CONNECTIONS,
                                     _METRICS_TCP_CONNECTION_SIZE );
    }


/*-----------------------------------------------------------*/

    void * Iot_MallocMetricsIpAddress( size_t size )
    {
        int freeIndex = -1;
        void * pNewIpAddress = NULL;

        if( size <= IOT_METRICS_MAX_IP_STRING_LENGTH )
        {
            /* Get the index of a free Shadow subscription. */
            freeIndex = IotStaticMemory_FindFree( _inUseIpAddresses,
                                                  IOT_METRICS_IP_ADDRESSES );

            if( freeIndex != -1 )
            {
                pNewIpAddress = &( _ipAddresses[ freeIndex ][ 0 ] );
            }
        }

        return pNewIpAddress;
    }

/*-----------------------------------------------------------*/

    void Iot_FreeMetricsIpAddress( void * ptr )
    {
        /* Return the in-use Shadow subscription. */
        IotStaticMemory_ReturnInUse( ptr,
                                     _ipAddresses,
                                     _inUseIpAddresses,
                                     IOT_METRICS_IP_ADDRESSES,
                                     IOT_METRICS_MAX_IP_STRING_LENGTH );
    }

#endif /* if IOT_STATIC_MEMORY_ONLY == 1 */
