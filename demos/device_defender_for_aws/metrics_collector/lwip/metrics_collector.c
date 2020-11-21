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

/* Standard includes. */
#include <stdint.h>

/* Lwip includes. */
#include "lwip/arch.h"
#include "lwip/stats.h"
#include "lwip/tcpip.h"         /* #define LOCK_TCPIP_CORE()     sys_mutex_lock(&lock_tcpip_core) */
#include "lwip/ip_addr.h"       /* ip_addr_t, ipaddr_ntoa, ip_addr_copy */
#include "lwip/tcp.h"           /* struct tcp_pcb */
#include "lwip/udp.h"           /* struct udp_pcb */
#include "lwip/priv/tcp_priv.h" /* tcp_listen_pcbs_t */

/* Demo config. */
#include "defender_demo_config.h"

/* Interface includes. */
#include "metrics_collector.h"

#if ( FREERTOS_LWIP_METRICS_ENABLE == 1 )

    extern struct tcp_pcb * tcp_active_pcbs;        /* List of all TCP PCBs that are in a state in which they accept or send data. */
    extern union tcp_listen_pcbs_t tcp_listen_pcbs; /* List of all TCP PCBs in LISTEN state */
    extern struct udp_pcb * udp_pcbs;               /* List of UDP PCBs */

/**
 * @brief Get a list of the listening TCP ports.
 *
 * This function finds the listening TCP ports by traversing LWIP TCP PCBs list
 * for ports that are in listen state. The head of the list is "tcp_listen_pcbs.listen_pcbs struture".
 * This function can be called with @p pOutTcpPortsArray NULL to get the number of the open TCP ports.
 *
 * @param[in] pOutTcpPortsArray The array to write the open TCP ports into. This
 * can be NULL, if only the number of open ports is needed.
 * @param[in] tcpPortsArrayLength Length of the pOutTcpPortsArray, if it is not
 * NULL.
 * @param[out] pOutNumTcpOpenPorts Number of the open TCP ports.
 *
 * @return #MetricsCollectorSuccess if open TCP ports are successfully obtained;
 * #MetricsCollectorBadParameter if invalid parameters are passed;
 */
    MetricsCollectorStatus_t GetOpenTcpPorts( uint16_t * pOutTcpPortsArray,
                                              uint32_t portsArrayLength,
                                              uint32_t * pOutNumTcpOpenPorts )
    {
        MetricsCollectorStatus_t status = MetricsCollectorSuccess;
        struct tcp_pcb_listen * pCurrPcb = tcp_listen_pcbs.listen_pcbs;
        uint16_t pcbCnt = 0;

        if( ( ( pOutTcpPortsArray != NULL ) && ( portsArrayLength == 0 ) ) ||
            ( pOutNumTcpOpenPorts == NULL ) )
        {
            LogError( ( "Invalid parameters. pOutTcpPortsArray: 0x%08x,"
                        "portsArrayLength: %u, pOutNumTcpOpenPorts: 0x%08x.",
                        pOutTcpPortsArray,
                        portsArrayLength,
                        pOutNumTcpOpenPorts ) );
            status = MetricsCollectorBadParameter;
        }

        if( status == MetricsCollectorSuccess )
        {
            LOCK_TCPIP_CORE();

            while( pCurrPcb )
            {
                /*  Write the ports into pOutTcpPortsArray, if not NULL */
                if( ( pOutTcpPortsArray != NULL ) && ( pcbCnt < portsArrayLength ) )
                {
                    pOutTcpPortsArray[ pcbCnt ] = pCurrPcb->local_port;
                }

                ++pcbCnt;
                pCurrPcb = pCurrPcb->next;
            }

            UNLOCK_TCPIP_CORE();
        }

        if( ( pOutTcpPortsArray != NULL ) && ( pcbCnt > portsArrayLength ) )
        {
            LogWarn( ( "The portsArrayLength is not long enough to store all the listening ports" ) );
        }

        if( status == MetricsCollectorSuccess )
        {
            *pOutNumTcpOpenPorts = pcbCnt;
        }

        return status;
    }


    MetricsCollectorStatus_t GetOpenUdpPorts( uint16_t * pOutUdpPortsArray,
                                              uint32_t portsArrayLength,
                                              uint32_t * pOutNumUdpOpenPorts )
    {
        MetricsCollectorStatus_t status = MetricsCollectorSuccess;
        struct udp_pcb * pCurrPcb = udp_pcbs;
        uint16_t pcbCnt = 0;

        if( ( ( pOutUdpPortsArray != NULL ) && ( portsArrayLength == 0 ) ) ||
            ( pOutNumUdpOpenPorts == NULL ) )
        {
            LogError( ( "Invalid parameters. pOutUdpPortsArray: 0x%08x,"
                        "portsArrayLength: %u, pOutNumUdpOpenPorts: 0x%08x.",
                        pOutUdpPortsArray,
                        portsArrayLength,
                        pOutNumUdpOpenPorts ) );
            status = MetricsCollectorBadParameter;
        }

        if( status == MetricsCollectorSuccess )
        {
            LOCK_TCPIP_CORE();

            while( pCurrPcb )
            {
                /*  Write the ports into pOutUdpPortsArray, if not NULL */
                if( ( pOutUdpPortsArray != NULL ) && ( pcbCnt < portsArrayLength ) )
                {
                    pOutUdpPortsArray[ pcbCnt ] = pCurrPcb->local_port;
                }

                ++pcbCnt;
                pCurrPcb = pCurrPcb->next;
            }

            UNLOCK_TCPIP_CORE();
        }

        if( ( pOutUdpPortsArray != NULL ) && ( pcbCnt > portsArrayLength ) )
        {
            LogWarn( ( "The portsArrayLength is not long enough to store all the listening ports" ) );
        }

        if( status == MetricsCollectorSuccess )
        {
            *pOutNumUdpOpenPorts = pcbCnt;
        }

        return status;
    }



/**
 * @brief Get a list of established connections.
 *
 * This function finds the established connections by traversing LWIP TCP active PCBs list.
 * The head of LWIP TCP active PCBs list is "tcp_active_pcbs".
 * This function can be called with @p pOutConnectionsArray NULL to get the number of
 * established connections.
 *
 * @param[in] pOutConnectionsArray The array to write the established connections
 * into. This can be NULL, if only the number of established connections is
 * needed.
 * @param[in] connectionsArrayLength Length of the pOutConnectionsArray, if it
 * is not NULL.
 * @param[out] pOutNumEstablishedConnections Number of the established connections.
 *
 * @return #MetricsCollectorSuccess if open TCP ports are successfully obtained;
 * #MetricsCollectorBadParameter if invalid parameters are passed;
 */
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

            while( pCurrPcb )
            {
                /*  Write the ports into pOutConnectionsArray, if not NULL. */
                if( ( pOutConnectionsArray != NULL ) && ( pcbCnt < connectionsArrayLength ) )
                {
                    /* The output array member to fill. */
                    pEstablishedConnection = &( pOutConnectionsArray[ pcbCnt ] );

                    pEstablishedConnection->remoteIp = pCurrPcb->remote_ip.addr; /* network byte order */
                    pEstablishedConnection->localIp = pCurrPcb->local_ip.addr;   /* network byte order */
                    pEstablishedConnection->localPort = pCurrPcb->local_port;    /* host byte order */
                    pEstablishedConnection->remotePort = pCurrPcb->remote_port;  /* host byte order */
                }

                ++pcbCnt;
                pCurrPcb = pCurrPcb->next;
            }

            UNLOCK_TCPIP_CORE();
        }

        if( ( pOutConnectionsArray != NULL ) && ( pcbCnt > connectionsArrayLength ) )
        {
            LogWarn( ( "The connectionsArrayLength is not long enough to store all the established connections" ) );
        }

        if( status == MetricsCollectorSuccess )
        {
            *pOutNumEstablishedConnections = pcbCnt;
        }

        return status;
    }

/**
 * @brief Get network stats.
 *
 * This function finds the network stats by reading MIB-II structure.
 *
 * @param[out] pOutNetworkStats The network stats.
 *
 * @return #MetricsCollectorSuccess if the network stats are successfully obtained;
 * #MetricsCollectorBadParameter if invalid parameters are passed;
 */
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
            pOutNetworkStats->bytesReceived = LWIP_GET_NETIF_PACKET_IN_sta();
            pOutNetworkStats->bytesSent = LWIP_GET_NETIF_PACKET_OUT_sta();
            pOutNetworkStats->packetsReceived = LWIP_GET_TCP_PACKET_IN();
            pOutNetworkStats->packetsSent = LWIP_GET_TCP_PACKET_OUT();
            UNLOCK_TCPIP_CORE();
        }

        return status;
    }

#endif //FREERTOS_LWIP_METRICS_ENABLE
