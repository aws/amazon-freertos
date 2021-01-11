/*
 * FreeRTOS V202012.00
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

/* Standard includes. */
#include <stdint.h>

/* Demo config. */
#include "defender_demo_config.h"

/* Interface includes. */
#include "metrics_collector.h"

/* Lwip includes. */
#include "lwip/arch.h"
#include "lwip/stats.h"
#include "lwip/tcpip.h"         /* #define LOCK_TCPIP_CORE()     sys_mutex_lock(&lock_tcpip_core) */
#include "lwip/ip_addr.h"       /* ip_addr_t, ipaddr_ntoa, ip_addr_copy */
#include "lwip/tcp.h"           /* struct tcp_pcb */
#include "lwip/udp.h"           /* struct udp_pcb */
#include "lwip/priv/tcp_priv.h" /* tcp_listen_pcbs_t */

/* Lwip configuration includes. */
#include "lwipopts.h"
#include "netif_port.h"

#if !defined( LWIP_TCPIP_CORE_LOCKING ) || ( LWIP_TCPIP_CORE_LOCKING == 0 )
    #error "Network metrics are only supported in core locking mode. Please define LWIP_TCPIP_CORE_LOCKING to 1 in lwipopts.h."
#endif

/* Helper macros to get bytes in/out and packets in/out. */
#define LWIP_GET_PACKETS_IN()       ( lwip_stats.mib2.ipinreceives )
#define LWIP_GET_PACKETS_OUT()      ( lwip_stats.mib2.ipoutrequests )
#if ( LWIP_BYTES_IN_OUT_UNSUPPORTED == 0 )
    #define LWIP_GET_BYTES_IN()     ( LWIP_NET_IF.mib2_counters.ifinoctets )
    #define LWIP_GET_BYTES_OUT()    ( LWIP_NET_IF.mib2_counters.ifoutoctets )
#else
    #define LWIP_GET_BYTES_IN()     ( 0 )
    #define LWIP_GET_BYTES_OUT()    ( 0 )
#endif /* LWIP_BYTES_IN_OUT_UNSUPPORTED == 1 */

/* Variables defined in the LWIP source code. */
extern struct tcp_pcb * tcp_active_pcbs;        /* List of all TCP PCBs that are in a state in which they accept or send data. */
extern union tcp_listen_pcbs_t tcp_listen_pcbs; /* List of all TCP PCBs in LISTEN state. */
extern struct udp_pcb * udp_pcbs;               /* List of UDP PCBs. */
/*-----------------------------------------------------------*/

MetricsCollectorStatus_t GetNetworkStats( NetworkStats_t * pOutNetworkStats )
{
    MetricsCollectorStatus_t status = MetricsCollectorSuccess;

    if( pOutNetworkStats == NULL )
    {
        LogError( ( "Invalid parameters. pOutNetworkStats: 0x%08x", pOutNetworkStats ) );
        status = MetricsCollectorBadParameter;
    }

    if( status == MetricsCollectorSuccess )
    {
        LOCK_TCPIP_CORE();

        pOutNetworkStats->bytesReceived = LWIP_GET_BYTES_IN();
        pOutNetworkStats->bytesSent = LWIP_GET_BYTES_OUT();
        pOutNetworkStats->packetsReceived = LWIP_GET_PACKETS_IN();
        pOutNetworkStats->packetsSent = LWIP_GET_PACKETS_OUT();

        UNLOCK_TCPIP_CORE();
    }

    return status;
}
/*-----------------------------------------------------------*/

MetricsCollectorStatus_t GetOpenTcpPorts( uint16_t * pOutTcpPortsArray,
                                          uint32_t tcpPortsArrayLength,
                                          uint32_t * pOutNumTcpOpenPorts )
{
    MetricsCollectorStatus_t status = MetricsCollectorSuccess;
    struct tcp_pcb_listen * pCurrPcb;
    uint16_t pcbCnt = 0;

    if( ( ( pOutTcpPortsArray != NULL ) && ( tcpPortsArrayLength == 0 ) ) ||
        ( pOutNumTcpOpenPorts == NULL ) )
    {
        LogError( ( "Invalid parameters. pOutTcpPortsArray: 0x%08x,"
                    "tcpPortsArrayLength: %u, pOutNumTcpOpenPorts: 0x%08x.",
                    pOutTcpPortsArray,
                    tcpPortsArrayLength,
                    pOutNumTcpOpenPorts ) );
        status = MetricsCollectorBadParameter;
    }

    if( status == MetricsCollectorSuccess )
    {
        LOCK_TCPIP_CORE();

        for( pCurrPcb = tcp_listen_pcbs.listen_pcbs; pCurrPcb != NULL; pCurrPcb = pCurrPcb->next )
        {
            if( pOutTcpPortsArray != NULL )
            {
                if( pcbCnt < tcpPortsArrayLength )
                {
                    pOutTcpPortsArray[ pcbCnt ] = pCurrPcb->local_port;
                    ++pcbCnt;
                }
                else
                {
                    /* Break if the output array is full. */
                    break;
                }
            }
            else
            {
                ++pcbCnt;
            }
        }

        UNLOCK_TCPIP_CORE();
    }

    if( ( pOutTcpPortsArray != NULL ) && ( pcbCnt == tcpPortsArrayLength ) && ( pCurrPcb != NULL ) )
    {
        LogWarn( ( "The pOutTcpPortsArray is not large enough to store all of the open TCP ports." ) );
    }

    if( status == MetricsCollectorSuccess )
    {
        *pOutNumTcpOpenPorts = pcbCnt;
    }

    return status;
}
/*-----------------------------------------------------------*/

MetricsCollectorStatus_t GetOpenUdpPorts( uint16_t * pOutUdpPortsArray,
                                          uint32_t udpPortsArrayLength,
                                          uint32_t * pOutNumUdpOpenPorts )
{
    MetricsCollectorStatus_t status = MetricsCollectorSuccess;
    struct udp_pcb * pCurrPcb;
    uint16_t pcbCnt = 0;

    if( ( ( pOutUdpPortsArray != NULL ) && ( udpPortsArrayLength == 0 ) ) ||
        ( pOutNumUdpOpenPorts == NULL ) )
    {
        LogError( ( "Invalid parameters. pOutUdpPortsArray: 0x%08x,"
                    "udpPortsArrayLength: %u, pOutNumUdpOpenPorts: 0x%08x.",
                    pOutUdpPortsArray,
                    udpPortsArrayLength,
                    pOutNumUdpOpenPorts ) );
        status = MetricsCollectorBadParameter;
    }

    if( status == MetricsCollectorSuccess )
    {
        LOCK_TCPIP_CORE();

        for( pCurrPcb = udp_pcbs; pCurrPcb != NULL; pCurrPcb = pCurrPcb->next )
        {
            if( pOutUdpPortsArray != NULL )
            {
                if( pcbCnt < udpPortsArrayLength )
                {
                    pOutUdpPortsArray[ pcbCnt ] = pCurrPcb->local_port;
                    ++pcbCnt;
                }
                else
                {
                    /* Break if the output array is full. */
                    break;
                }
            }
            else
            {
                ++pcbCnt;
            }
        }

        UNLOCK_TCPIP_CORE();
    }

    if( ( pOutUdpPortsArray != NULL ) && ( pcbCnt == udpPortsArrayLength ) && ( pCurrPcb != NULL ) )
    {
        LogWarn( ( "The pOutUdpPortsArray is not large enough to store all of the open UDP ports." ) );
    }

    if( status == MetricsCollectorSuccess )
    {
        *pOutNumUdpOpenPorts = pcbCnt;
    }

    return status;
}
/*-----------------------------------------------------------*/

MetricsCollectorStatus_t GetEstablishedConnections( Connection_t * pOutConnectionsArray,
                                                    uint32_t connectionsArrayLength,
                                                    uint32_t * pOutNumEstablishedConnections )
{
    MetricsCollectorStatus_t status = MetricsCollectorSuccess;
    struct tcp_pcb * pCurrPcb = tcp_active_pcbs;
    uint16_t pcbCnt = 0;
    Connection_t * pEstablishedConnection;

    if( ( ( pOutConnectionsArray != NULL ) && ( connectionsArrayLength == 0 ) ) ||
        ( pOutNumEstablishedConnections == NULL ) )
    {
        LogError( ( "Invalid parameters. pOutConnectionsArray: 0x%08x,"
                    " connectionsArrayLength: %u, pOutNumEstablishedConnections: 0x%08x.",
                    pOutConnectionsArray,
                    connectionsArrayLength,
                    pOutNumEstablishedConnections ) );
        status = MetricsCollectorBadParameter;
    }

    if( status == MetricsCollectorSuccess )
    {
        LOCK_TCPIP_CORE();

        for( pCurrPcb = tcp_active_pcbs; pCurrPcb != NULL; pCurrPcb = pCurrPcb->next )
        {
            if( pOutConnectionsArray != NULL )
            {
                if( pcbCnt < connectionsArrayLength )
                {
                    /* The output array member to fill. */
                    pEstablishedConnection = &( pOutConnectionsArray[ pcbCnt ] );

                    #if LWIP_IPV4 && LWIP_IPV6
                        pEstablishedConnection->remoteIp = pCurrPcb->remote_ip.u_addr.ip4.addr; /* Network byte order. */
                        pEstablishedConnection->localIp = pCurrPcb->local_ip.u_addr.ip4.addr;   /* Network byte order. */
                    #elif LWIP_IPV4
                        pEstablishedConnection->remoteIp = pCurrPcb->remote_ip.addr;            /* Network byte order. */
                        pEstablishedConnection->localIp = pCurrPcb->local_ip.addr;              /* Network byte order. */
                    #else
                    #error "IPV6 only is not supported."
                    #endif

                    pEstablishedConnection->localPort = pCurrPcb->local_port;   /* Host byte order. */
                    pEstablishedConnection->remotePort = pCurrPcb->remote_port; /* Host byte order. */

                    ++pcbCnt;
                }
                else
                {
                    /* Break if the output array is full. */
                    break;
                }
            }
            else
            {
                ++pcbCnt;
            }
        }

        UNLOCK_TCPIP_CORE();
    }

    if( ( pOutConnectionsArray != NULL ) && ( pcbCnt == connectionsArrayLength ) && ( pCurrPcb != NULL ) )
    {
        LogWarn( ( "The pOutConnectionsArray is not large enough to store all of the established connections." ) );
    }

    if( status == MetricsCollectorSuccess )
    {
        *pOutNumEstablishedConnections = pcbCnt;
    }

    return status;
}
/*-----------------------------------------------------------*/
