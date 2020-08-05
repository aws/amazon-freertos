#pragma once

#include <whd_types.h>
#include "lwipopts_freertos.h"

#ifdef CUSTOM_LWIPOPTS
#include "custom_lwipopts.h"
#else /* ifdef CUSTOM_LWIPOPTS */

#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
#define LWIP_TIMEVAL_PRIVATE 0
#endif

/**
 * MEM_ALIGNMENT: should be set to the alignment of the CPU
 *    4 byte alignment -> #define MEM_ALIGNMENT 4
 *    2 byte alignment -> #define MEM_ALIGNMENT 2
 */
#define MEM_ALIGNMENT                  (4)

/**
 * Enable IPV4 networking
 */
#define LWIP_IPV4                       (1)
#define LWIP_ICMP                       (1)
#define LWIP_TCP                        (1)
#define LWIP_UDP                        (1)
#define LWIP_IGMP                       (1)

/**
 * Enable IPV6 networking
 */
#define LWIP_IPV6                       (1)
#define LWIP_IPV6_AUTOCONFIG            (1)
#define LWIP_IPV6_SEND_ROUTER_SOLICIT   (1)
#define LWIP_ICMP6                      (1)
#define LWIP_IPV6_MLD                   (1)

/**
 * MEM_LIBC_MALLOC==1: Use malloc/free/realloc provided by your C-library
 * instead of the lwip internal allocator. Can save code size if you
 * already use it.
 */
#define MEM_LIBC_MALLOC                (1)

/**
 * MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
 * per active UDP "connection".
 * (requires the LWIP_UDP option)
 */
#ifdef MEMP_NUM_UDP_PCB
#undef MEMP_NUM_UDP_PCB
#endif
#define MEMP_NUM_UDP_PCB               10

/**
 * MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments.
 * (requires the LWIP_TCP option)
 */
#ifdef MEMP_NUM_TCP_SEG
#undef MEMP_NUM_TCP_SEG
#endif
#define MEMP_NUM_TCP_SEG               (TCP_SND_QUEUELEN+1)

/**
 * PBUF_POOL_TX_SIZE: the number of buffers in the Tx pbuf pool.
 */
#ifndef PBUF_POOL_TX_SIZE
#ifdef TX_PACKET_POOL_SIZE
#define PBUF_POOL_TX_SIZE                 TX_PACKET_POOL_SIZE
#else
#define PBUF_POOL_TX_SIZE                 (7)
#endif
#endif

#ifdef PBUF_POOL_SIZE
#undef PBUF_POOL_SIZE
#endif
#define PBUF_POOL_SIZE 14
/**
 * PBUF_POOL_LARGE_SIZE is the memory pool for packets greater
 * than the set MTU size
 */
#ifndef PBUF_POOL_LARGE_SIZE
#ifdef  LARGE_PACKET_POOL_SIZE
#define PBUF_POOL_LARGE_SIZE             LARGE_PACKET_POOL_SIZE
#else
#define PBUF_POOL_LARGE_SIZE             (1)
#endif
#endif

/**
 * MEMP_NUM_REASSDATA: the number of IP packets simultaneously queued for
 * reassembly (whole packets, not fragments!)
 */
#if IP_REASS_MAX_PBUFS > 1
#ifndef MEMP_NUM_REASSDATA
#define MEMP_NUM_REASSDATA              (IP_REASS_MAX_PBUFS - 1)
#endif
#else
#define MEMP_NUM_REASSDATA              0
#endif

/**
 * PBUF_LINK_HLEN: the number of bytes that should be allocated for a
 * link level header. The default is 14, the standard value for
 * Ethernet.
 */
#ifdef PBUF_LINK_HLEN
#undef PBUF_LINK_HLEN
#endif
#define PBUF_LINK_HLEN                 (WHD_PHYSICAL_HEADER)

/**
 * PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. The default is
 * designed to accommodate single full size TCP frame in one pbuf, including
 * TCP_MSS, IP header, and link header.
 */
#ifdef PBUF_POOL_BUFSIZE
#undef PBUF_POOL_BUFSIZE
#endif
#define PBUF_OFFSET_HDRLEN  PBUF_LINK_ENCAPSULATION_HLEN + PBUF_LINK_HLEN + PBUF_IP_HLEN + PBUF_TRANSPORT_HLEN
#define PBUF_ALLOC_LENGTH   LWIP_MEM_ALIGN_BUFFER(LWIP_MEM_ALIGN_SIZE(16 + PBUF_OFFSET_HDRLEN) + LWIP_MEM_ALIGN_SIZE(WHD_PAYLOAD_MTU))
#define PBUF_POOL_BUFSIZE   PBUF_ALLOC_LENGTH
#define WHD_PING_PAYLOAD_MTU (10000)

/* This PBUF_POOL_LARGE_BUFSIZE is for ping with payload of
 * 10000 bytes + IP Header Length + Transport Header length (ICMP HDR) + sizeof struct pbuf
 */
#define PBUF_POOL_LARGE_BUFSIZE  LWIP_MEM_ALIGN_BUFFER(LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF + PBUF_OFFSET_HDRLEN) + LWIP_MEM_ALIGN_SIZE(WHD_PING_PAYLOAD_MTU))

/**
 * TCP_MSS: TCP Maximum segment size. (default is 536, a conservative default,
 * you might want to increase this.)
 * For the receive side, this MSS is advertised to the remote side
 * when opening a connection. For the transmit size, this MSS sets
 * an upper limit on the MSS advertised by the remote host.
 */
/* Ethernet MTU - IP header size - TCP header size */
#ifdef TCP_MSS
#undef TCP_MSS
#endif
#define TCP_MSS                        (WHD_PAYLOAD_MTU-20-20)

/**
 * TCP_SND_BUF: TCP sender buffer space (bytes).
 * To achieve good performance, this should be at least 2 * TCP_MSS.
 */
#ifdef TCP_SND_BUF
#undef TCP_SND_BUF
#endif
#ifdef TX_PACKET_POOL_SIZE
#define TCP_SND_BUF                    LWIP_MAX((2 * TCP_MSS), ((TX_PACKET_POOL_SIZE/2) * TCP_MSS))
#else
#define TCP_SND_BUF                    (6 * TCP_MSS)
#endif

/**
 * TCP_WND: The size of a TCP window.  This must be at least
 * (2 * TCP_MSS) for things to work well.
 * ATTENTION: when using TCP_RCV_SCALE, TCP_WND is the total size
 * with scaling applied. Maximum window value in the TCP header
 * will be TCP_WND >> TCP_RCV_SCALE
 */
#ifdef TCP_WND
#undef TCP_WND
#endif
#define TCP_WND                        (2 * TCP_MSS)

/**
 * LWIP_NETIF_TX_SINGLE_PBUF: if this is set to 1, lwIP tries to put all data
 * to be sent into one single pbuf. This is for compatibility with DMA-enabled
 * MACs that do not support scatter-gather.
 * Beware that this might involve CPU-memcpy before transmitting that would not
 * be needed without this flag! Use this only if you need to!
 *
 * @todo: TCP and IP-frag do not work with this, yet:
 */
/* TODO: remove this option once buffer chaining has been implemented */
#define LWIP_NETIF_TX_SINGLE_PBUF      (1)

/** Define LWIP_COMPAT_MUTEX if the port has no mutexes and binary semaphores
 *  should be used instead
 */
#define LWIP_COMPAT_MUTEX              (0)
#define LWIP_COMPAT_MUTEX_ALLOWED

/**
 * LWIP_RAW==1: Enable application layer to hook into the IP layer itself.
 */
#define LWIP_RAW                       (1)

/**
 * TCPIP_THREAD_STACKSIZE: The stack size used by the main tcpip thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifdef TCPIP_THREAD_STACKSIZE
#undef TCPIP_THREAD_STACKSIZE
#endif
#define TCPIP_THREAD_STACKSIZE         (4 * 1024)

/**
 * TCP_LISTEN_BACKLOG: Enable the backlog option for tcp listen pcb.
 */
#define TCP_LISTEN_BACKLOG     (1)

/* WHD likes to enable AUTOIP */
#ifdef AUTO_IP_ENABLED
/**
 * LWIP_AUTOIP==1: Enable AutoIP module.
 */
#   define LWIP_AUTOIP                 (1)

/* Note: Not enabling LWIP_DHCP_AUTOIP_COOP on purpose */
#endif /* AUTO_IP_ENABLED */

/**
 * LWIP_PROVIDE_ERRNO: System does not have errno defines - force LwIP to create them
 */
//#define LWIP_PROVIDE_ERRNO             (1)

/** LWIP_TCPIP_TIMEOUT==1: Enable tcpip_timeout/tcpip_untimeout to create
 * timers running in tcpip_thread from another thread.
 */
#define LWIP_TCPIP_TIMEOUT             (0)

/**
 * MEMP_NUM_ARP_QUEUE: the number of simultaneously queued outgoing
 * packets (pbufs) that are waiting for an ARP request (to resolve
 * their destination address) to finish.
 * (requires the ARP_QUEUEING option)
 */
#ifdef MEMP_NUM_ARP_QUEUE
#undef MEMP_NUM_ARP_QUEUE
#endif
#define MEMP_NUM_ARP_QUEUE              5

/**
 * MEMP_NUM_NETCONN: the number of struct netconns.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#ifdef MEMP_NUM_NETCONN
#undef MEMP_NUM_NETCONN
#endif
#define MEMP_NUM_NETCONN               (18)

/**
 * LWIP_TCP_KEEPALIVE==1: Enable TCP_KEEPIDLE, TCP_KEEPINTVL and TCP_KEEPCNT
 * options processing. Note that TCP_KEEPIDLE and TCP_KEEPINTVL have to be set
 * in seconds. (does not require sockets.c, and will affect tcp.c)
 */
#define LWIP_TCP_KEEPALIVE             (1)

#ifdef LWIP_SO_RCVBUF
#if ( LWIP_SO_RCVBUF == 1 )
#include <limits.h>  /* Needed because RECV_BUFSIZE_DEFAULT is defined as INT_MAX */
#endif /* if ( LWIP_SO_RCVBUF == 1 ) */
#endif /* ifdef LWIP_SO_RCVBUF */

/**
 * LWIP_STATS==1: Enable statistics collection in lwip_stats.
 */
#ifdef CY_LWIP_DEBUG
#define LWIP_STATS                     (1)
#else
#define LWIP_STATS                     (0)
#endif /* ifdef CY_LWIP_DEBUG */

/**
 * LWIP_NETIF_API==1: Support netif api (in netifapi.c)
 */
#define LWIP_NETIF_API                  1

/**
 * LWIP_NETIF_HOSTNAME==1: use DHCP_OPTION_HOSTNAME with netif's hostname
 * field.
 */
#define LWIP_NETIF_HOSTNAME             1

/** LWIP_SUPPORT_CUSTOM_PBUF==1: Custom pbufs behave much like their pbuf type
 * but they are allocated by external code (initialised by calling
 * pbuf_alloced_custom()) and when pbuf_free gives up their last reference, they
 * are freed by calling pbuf_custom->custom_free_function().
 * Currently, the pbuf_custom code is only needed for one specific configuration
 * of IP_FRAG, unless required by external driver/application code. */
#define LWIP_SUPPORT_CUSTOM_PBUF        1

#endif /* ifdef CUSTOM_LWIPOPTS */
