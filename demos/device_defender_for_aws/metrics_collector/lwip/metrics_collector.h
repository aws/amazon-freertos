#ifndef __METRICS_H__
#define __METRICS_H__

/* Standard includes. */
#include <stdint.h>
/* Lwip includes */
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
    ip_addr_t localIp;
    ip_addr_t remoteIp;
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
#define LWIP_GET_TCP_PACKET_OUT()          ( lwip_stats.mib2.ipoutrequests + lwip_stats.mib2.ipforwdatagrams ) /*need review */

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
