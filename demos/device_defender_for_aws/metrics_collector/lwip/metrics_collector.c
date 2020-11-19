/* Standard includes. */
#include <stdint.h>
/* Lwip includes. */
#include "lwip/arch.h"
#include "lwip/stats.h"
#include "lwip/tcpip.h"         /* #define LOCK_TCPIP_CORE()     sys_mutex_lock(&lock_tcpip_core) */
#include "lwip/ip_addr.h"       /*ip_addr_t, ipaddr_ntoa, ip_addr_copy */
#include "lwip/tcp.h"           /* struct tcp_pcb */
#include "lwip/priv/tcp_priv.h" /*tcp_listen_pcbs_t */
/* Platform layer includes. */
#include "platform/iot_threads.h"
/* Demo logging include. */
#include "iot_demo_logging.h"
/* Interface includes. */
#include "metrics_collector.h"

#if ( FREERTOS_LWIP_METRICS_ENABLE == 1 )

    extern struct tcp_pcb * tcp_active_pcbs;        /* List of all TCP PCBs that are in a state in which they accept or send data. */
    extern union tcp_listen_pcbs_t tcp_listen_pcbs; /* List of all TCP PCBs in LISTEN state */

/**
 * @brief Get a list of the listening TCP ports.
 *
 * This function finds the listening TCP ports by traversing LWIP TCP PCBs list that
 * in listen state. The header of the list is "tcp_listen_pcbs.listen_pcbs struture".
 * It can be called with @p pOutTcpPortsArray NULL to get the number of the open TCP ports.
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
            IotLogError( "Invalid parameters. pOutTcpPortsArray: 0x%08x,"
                         "portsArrayLength: %u, pOutNumTcpOpenPorts: 0x%08x.",
                         pOutTcpPortsArray,
                         portsArrayLength,
                         pOutNumTcpOpenPorts );
            status = MetricsCollectorBadParameter;
        }

        if( status == MetricsCollectorSuccess )
        {
            LOCK_TCPIP_CORE();

            while( pCurrPcb )
            {
                /*  write the ports into pOutTcpPortsArray if not NULL */
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
            IotLogWarn( "The portsArrayLength is not long engough to store all the listening ports" );
        }

        if( status == MetricsCollectorSuccess )
        {
            *pOutNumTcpOpenPorts = pcbCnt;
        }

        return status;
    }


    MetricsCollectorStatus_t GetOpenUdpPorts( uint16_t * pOutTcpPortsArray,
                                            uint32_t portsArrayLength,
                                            uint32_t * pOutNumTcpOpenPorts )
    {
        //TODO
        return MetricsCollectorSuccess;
    }



/**
 * @brief Get a list of established connections.
 *
 * This function finds the established connections by traversing LWIP TCP active PCBs list.
 * The header of LWIP TCP active PCBs list is "tcp_active_pcbs".
 * It can be called with @p pOutConnectionsArray NULL to get the number of
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
            IotLogError( "Invalid parameters. pOutConnectionsArray: 0x%08x,"
                         " connectionsArrayLength: %u, pOutNumEstablishedConnections: 0x%08x.",
                         pOutConnectionsArray,
                         connectionsArrayLength,
                         pOutNumEstablishedConnections );
            status = MetricsCollectorBadParameter;
        }

        if( status == MetricsCollectorSuccess )
        {
            LOCK_TCPIP_CORE();

            while( pCurrPcb )
            {
                /*  write the ports into pOutConnectionsArray if not NULL */
                if( ( pOutConnectionsArray != NULL ) && ( pcbCnt < connectionsArrayLength ) )
                {
                    /* The output array member to fill. */
                    pEstablishedConnection = &( pOutConnectionsArray[ pcbCnt ] );

                    ip_addr_copy( pEstablishedConnection->remoteIp, pCurrPcb->remote_ip ); /* network byte order */
                    ip_addr_copy( pEstablishedConnection->localIp, pCurrPcb->local_ip );   /* network byte order */
                    pEstablishedConnection->localPort = pCurrPcb->local_port;              /* host byte order */
                    pEstablishedConnection->remotePort = pCurrPcb->remote_port;            /* host byte order */
                }

                ++pcbCnt;
                pCurrPcb = pCurrPcb->next;
            }

            UNLOCK_TCPIP_CORE();
        }

        if( ( pOutConnectionsArray != NULL ) && ( pcbCnt > connectionsArrayLength ) )
        {
            IotLogWarn( "The connectionsArrayLength is not long engough to store all the established connections" );
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
            IotLogError( "Invalid parameters. pOutNetworkStats: 0x%08x", pOutNetworkStats );
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
