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
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef __METRICS_H__
#define __METRICS_H__

/* Standard includes. */
#include <stdint.h>
/* Lwip includes. */
#include "lwipopts.h"
#include "lwip/ip_addr.h"
#include "netif_port.h"

#if !defined( FREERTOS_LWIP_METRICS_ENABLE ) || ( FREERTOS_LWIP_METRICS_ENABLE == 0 )
    #warning "FREERTOS_LWIP_METRICS_ENABLE is disabled."
#endif

#if !defined( LWIP_TCPIP_CORE_LOCKING_INPUT ) || ( LWIP_TCPIP_CORE_LOCKING_INPUT == 0 )
    #warning "The network metric will not be supported if LWIP_TCPIP_CORE_LOCKING_INPUT is disabled."
#endif


/**
 * @brief Return codes from metrics collector APIs.
 */
typedef enum
{
    MetricsCollectorSuccess = 0,
    MetricsCollectorBadParameter,
    MetricsCollectorFileOpenFailed,
    MetricsCollectorParsingFailed
} MetricsCollectorStatus_t;

/**
 * @brief Represents a network connection.
 */
typedef struct Connection
{
    uint32_t localIp;
    uint32_t remoteIp;
    uint16_t localPort;
    uint16_t remotePort;
} Connection_t;

/**
 * @brief Represents network stats.
 */
typedef struct NetworkStats
{
    uint32_t bytesReceived;   /**< Number of bytes received. */
    uint32_t bytesSent;       /**< Number of bytes sent. */
    uint32_t packetsReceived; /**< Number of TCP packets received. */
    uint32_t packetsSent;     /**< Number of TCP packets sent. */
} NetworkStats_t;


#define LWIP_GET_TCP_PACKET_IN()           ( lwip_stats.mib2.ipinreceives )
#define LWIP_GET_TCP_PACKET_OUT()          ( lwip_stats.mib2.ipoutrequests )

#define LWIP_GET_NETIF_PACKET_IN_sta()     ( LWIP_NET_IF.mib2_counters.ifinoctets )
#define LWIP_GET_NETIF_PACKET_OUT_sta()    ( LWIP_NET_IF.mib2_counters.ifoutoctets )

extern MetricsCollectorStatus_t GetNetworkStats( NetworkStats_t * pOutNetworkStats );
extern MetricsCollectorStatus_t GetEstablishedConnections( Connection_t * pOutConnectionsArray,
                                                           uint32_t connectionsArrayLength,
                                                           uint32_t * pOutNumEstablishedConnections );
extern MetricsCollectorStatus_t GetOpenTcpPorts( uint16_t * pOutPortsArray,
                                                 uint32_t portsArrayLength,
                                                 uint32_t * pOutNumOpenPorts );
extern MetricsCollectorStatus_t GetOpenUdpPorts( uint16_t * pOutPortsArray,
                                                 uint32_t portsArrayLength,
                                                 uint32_t * pOutNumOpenPorts );

#endif //__METRICS_H__
