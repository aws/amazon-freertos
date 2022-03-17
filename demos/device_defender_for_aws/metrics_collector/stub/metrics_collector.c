/*
 * FreeRTOS V202203.00
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * https://aws.amazon.com/freertos
 * https://www.FreeRTOS.org
 */

/**
 * @file metrics_collector.c
 *
 * @brief Functions used by the defender demo to collect networking metrics.
 * This file is a stub which returns zero for all metrics. In order to report
 * correct metrics, the functions in this file must be implemented for the
 * target platform's network stack.
 * Here are reference implementations for the FreeRTOS+TCP and LWIP network stacks:
 *
 * FreeRTOS+TCP:
 *      If you are using FreeRTOS+TCP, use the metrics_collector.c file
 *      available at
 *      [FreeRTOS+TCP metrics_collector implementation](https://github.com/aws/amazon-freertos/blob/master/demos/device_defender_for_aws/metrics_collector/freertos_plus_tcp/metrics_collector.c).
 *
 * LWIP:
 *      If you are using LWIP, use the metrics_collector.c available at
 *      [LWIP metrics_collector implementation](https://github.com/aws/amazon-freertos/blob/master/demos/device_defender_for_aws/metrics_collector/lwip/metrics_collector.c).
 *
 *      In addition, define the following macros in your lwipopts.h:
 *      #define LINK_SPEED_OF_YOUR_NETIF_IN_BPS 0
 *      #define LWIP_TCPIP_CORE_LOCKING         1
 *      #define LWIP_STATS                      1
 *      #define MIB2_STATS                      1
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo config. */
#include "defender_demo_config.h"

/* Interface include. */
#include "metrics_collector.h"
/*-----------------------------------------------------------*/

MetricsCollectorStatus_t GetNetworkStats( NetworkStats_t * pOutNetworkStats )
{
    MetricsCollectorStatus_t status = MetricsCollectorSuccess;

    if( pOutNetworkStats == NULL )
    {
        status = MetricsCollectorBadParameter;
    }

    if( status == MetricsCollectorSuccess )
    {
        /* Take a look at the comments at the top of this file. */
        LogError( ( "Using stub definition of GetNetworkStats! "
                    "Please implement for your network stack to get correct metrics." ) );

        pOutNetworkStats->bytesReceived = 0;
        pOutNetworkStats->packetsReceived = 0;
        pOutNetworkStats->bytesSent = 0;
        pOutNetworkStats->packetsSent = 0;
    }

    return status;
}
/*-----------------------------------------------------------*/

MetricsCollectorStatus_t GetOpenTcpPorts( uint16_t * pOutTcpPortsArray,
                                          size_t tcpPortsArrayLength,
                                          size_t * pOutNumTcpOpenPorts )
{
    MetricsCollectorStatus_t status = MetricsCollectorSuccess;

    /* pOutTcpPortsArray can be NULL. */
    if( pOutNumTcpOpenPorts == NULL )
    {
        status = MetricsCollectorBadParameter;
    }

    if( status == MetricsCollectorSuccess )
    {
        /* Take a look at the comments at the top of this file. */
        LogError( ( "Using stub definition of GetOpenTcpPorts! "
                    "Please implement for your network stack to get correct metrics." ) );

        if( pOutTcpPortsArray != NULL )
        {
            /* Fill the output array with as many TCP ports as will fit in the
             * given array. */

            /* Return the number of elements copied to the array. */
            *pOutNumTcpOpenPorts = 0;
        }
        else
        {
            /* Return the total number of open ports. */
            *pOutNumTcpOpenPorts = 0;
        }
    }

    return status;
}
/*-----------------------------------------------------------*/

MetricsCollectorStatus_t GetOpenUdpPorts( uint16_t * pOutUdpPortsArray,
                                          size_t udpPortsArrayLength,
                                          size_t * pOutNumUdpOpenPorts )
{
    MetricsCollectorStatus_t status = MetricsCollectorSuccess;

    if( pOutNumUdpOpenPorts == NULL )
    {
        status = MetricsCollectorBadParameter;
    }

    if( status == MetricsCollectorSuccess )
    {
        /* Take a look at the comments at the top of this file. */
        LogError( ( "Using stub definition of GetOpenUdpPorts! "
                    "Please implement for your network stack to get correct metrics." ) );

        if( pOutUdpPortsArray != NULL )
        {
            /* Fill the output array with as many UDP ports as will fit in the
             * given array. */

            /* Return the number of elements copied to the array. */
            *pOutNumUdpOpenPorts = 0;
        }
        else
        {
            /* Return the total number of open ports. */
            *pOutNumUdpOpenPorts = 0;
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

MetricsCollectorStatus_t GetEstablishedConnections( Connection_t * pOutConnectionsArray,
                                                    size_t connectionsArrayLength,
                                                    size_t * pOutNumEstablishedConnections )
{
    MetricsCollectorStatus_t status = MetricsCollectorSuccess;

    if( pOutNumEstablishedConnections == NULL )
    {
        status = MetricsCollectorBadParameter;
    }

    if( status == MetricsCollectorSuccess )
    {
        /* Take a look at the comments at the top of this file. */
        LogError( ( "Using stub definition of GetEstablishedConnections! "
                    "Please implement for your network stack to get correct metrics." ) );

        if( pOutConnectionsArray != NULL )
        {
            /* Fill the output array with as many TCP socket infos as will fit in
             * the given array. */

            /* Return the number of elements copied to the array. */
            *pOutNumEstablishedConnections = 0;
        }
        else
        {
            /* Return the total number of established connections. */
            *pOutNumEstablishedConnections = 0;
        }
    }

    return status;
}
/*-----------------------------------------------------------*/
