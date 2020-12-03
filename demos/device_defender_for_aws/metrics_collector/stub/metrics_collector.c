/*
 * FreeRTOS V202011.00
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
 * https://aws.amazon.com/freertos
 * https://www.FreeRTOS.org
 */

/**
 * @file metrics_collector.c
 *
 * @brief Functions used by the defender demo to collect metrics on the
 * device's open ports and sockets. This file is a stub which returns zero for
 * all metrics. In order to report correct metrics, the functions in this file
 * must be implemented for the target platform's network stack. For guidance,
 * see the provided implmentations for FreeRTOS+TCP and lwIP (with
 * LWIP_TCPIP_CORE_LOCKING enabled) available at the following link:
 * https://github.com/aws/amazon-freertos/tree/master/demos/device_defender_for_aws/metrics_collector
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdint.h>

/* Demo config. */
#include "defender_demo_config.h"

/* Interface include. */
#include "metrics_collector.h"
/*-----------------------------------------------------------*/

MetricsCollectorStatus_t GetNetworkStats( NetworkStats_t * pOutNetworkStats )
{
    MetricsCollectorStatus_t status = MetricsCollectorSuccess;

    configASSERT( pOutNetworkStats != NULL );

    /* Initialize everything to zero. */
    memset( pOutNetworkStats, 0, sizeof( NetworkStats_t ) );

    LogError( ( "GetNetworkStats called from stub metrics_collector! "
                "This will need to be implemented for your network stack for correct metrics" ) );

    pOutNetworkStats->bytesReceived = 0;
    pOutNetworkStats->packetsReceived = 0;
    pOutNetworkStats->bytesSent = 0;
    pOutNetworkStats->packetsSent = 0;

    return status;
}
/*-----------------------------------------------------------*/

MetricsCollectorStatus_t GetOpenTcpPorts( uint16_t * pOutTcpPortsArray,
                                          uint32_t tcpPortsArrayLength,
                                          uint32_t * pOutNumTcpOpenPorts )
{
    MetricsCollectorStatus_t status = MetricsCollectorSuccess;

    /* pOutTcpPortsArray can be NULL. */
    configASSERT( pOutNumTcpOpenPorts != NULL );

    LogError( ( "GetOpenTcpPorts called from stub metrics_collector! "
                "This will need to be implemented for your network stack for correct metrics" ) );

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

    return status;
}
/*-----------------------------------------------------------*/

MetricsCollectorStatus_t GetOpenUdpPorts( uint16_t * pOutUdpPortsArray,
                                          uint32_t udpPortsArrayLength,
                                          uint32_t * pOutNumUdpOpenPorts )
{
    MetricsCollectorStatus_t status = MetricsCollectorSuccess;

    /* pOutUdpPortsArray can be NULL. */
    configASSERT( pOutNumUdpOpenPorts != NULL );

    LogError( ( "GetOpenUdpPorts called from stub metrics_collector! "
                "This will need to be implemented for your network stack for correct metrics" ) );

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

    return status;
}

/*-----------------------------------------------------------*/

MetricsCollectorStatus_t GetEstablishedConnections( Connection_t * pOutConnectionsArray,
                                                    uint32_t connectionsArrayLength,
                                                    uint32_t * pOutNumEstablishedConnections )
{
    MetricsCollectorStatus_t status = MetricsCollectorSuccess;

    /* pOutConnectionsArray can be NULL. */
    configASSERT( pOutNumEstablishedConnections != NULL );

    LogError( ( "GetEstablishedConnections called from stub metrics_collector! "
                "This will need to be implemented for your network stack for correct metrics" ) );

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

    return status;
}
/*-----------------------------------------------------------*/
