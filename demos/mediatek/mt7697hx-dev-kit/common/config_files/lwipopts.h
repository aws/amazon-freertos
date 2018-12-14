/*
 * Copyright (c) 2001, 2002 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/* for LwIP task name and stack size */
//#include "task_def.h"
//#include "syslog.h"

/* FreeRTOS lwipopts.h */
#include "arch/lwipopts_freertos.h"

#if defined(MTK_MDNS_ENABLE)
#define LWIP_IPV6                       1
#define IP_REASS_MAXAGE                 60 // 3
#define LWIP_IPV6_FRAG                  1
#define LWIP_MULTICAST_PING             1
#define LWIP_HAVE_LOOPIF                1
#define LWIP_IGMP                       1
#define LWIP_NETIF_HOSTNAME             1
#define LWIP_DHCP_AUTOIP_COOP           1
#define LWIP_DHCP_AUTOIP_COOP_TRIES     3

#define LWIP_NETBUF_RECVINFO            1
#define LWIP_SOCKET                     1
#define LWIP_COMPAT_SOCKETS             1
#define LWIP_POSIX_SOCKETS_IO_NAMES     1
#define MEMP_NUM_NETBUF                 16
#define MEMP_NUM_MLD6_GROUP             10

#define MEMP_NUM_ND6_QUEUE              30
#define LWIP_ND6_RETRANS_TIMER          200
#endif


/* MEM_SIZE: the size of the heap memory. If the application will send
   a lot of data that needs to be copied, this should be set high. */
#if defined(MTK_WIFI_TGN_VERIFY_ENABLE) && !defined(MTK_HOMEKIT_ENABLE)
#define MEM_SIZE                (100 * 1024)
#else
#define MEM_SIZE                (40 * 1024)
#endif

/* Declare the existence/type/size of LwIP's own heap. */
extern char                     ram_heap[ MEM_SIZE ];

/* Let LwIP use TCM memory for pbuf heap. */
#define LWIP_RAM_HEAP_POINTER   ram_heap


#ifdef MTK_HIF_GDMA_ENABLE
#define PBUF_LINK_ENCAPSULATION_HLEN 2 // for WiFi headroom (TX zero copy)
#else
#define PBUF_LINK_ENCAPSULATION_HLEN 0
#endif

/* PBUF_LINK_HLEN: the number of bytes that should be allocated for a
   link level header. The default is 14, the standard value for
   Ethernet. */
#define PBUF_LINK_HLEN           (RESERVED_HLEN + 14 + ETH_PAD_SIZE)

/*
   ---------------------------------
   ----------- IP options ----------
   ---------------------------------
 */

/**
 * IP_REASS_MAX_PBUFS: Total maximum amount of pbufs waiting to be reassembled.
 * Since the received pbufs are enqueued, be sure to configure
 * PBUF_POOL_SIZE > IP_REASS_MAX_PBUFS so that the stack is still able to receive
 * packets even if the maximum amount of fragments is enqueued for reassembly!
 */
#if defined(MTK_WIFI_TGN_VERIFY_ENABLE)
#define IP_REASS_MAX_PBUFS      25
#else
#define IP_REASS_MAX_PBUFS      10
#endif



/*
   ---------------------------------
   ----- Statistics options --------
   ---------------------------------
 */


#if defined(MTK_LWIP_STATISTICS_ENABLE)
#define LWIP_STATS              1
#define LWIP_STATS_DISPLAY      1
#endif


/*
   ------------------------------------
   ----------- Debug options ----------
   ------------------------------------
 */


#if defined(MTK_LWIP_DYNAMIC_DEBUG_ENABLE)

struct lwip_debug_flags {
    char        *debug_flag_name;
    uint32_t    debug_flag;
};

extern struct lwip_debug_flags  lwip_debug_flags[];

#define LWIP_DEBUG LWIP_DBG_ON
#define LWIP_DEBUG_IDX(idx)     ((idx) >> 8)

#undef ETHARP_DEBUG
#undef NETIF_DEBUG
#undef PBUF_DEBUG
#undef API_LIB_DEBUG
#undef API_MSG_DEBUG
#undef SOCKETS_DEBUG
#undef ICMP_DEBUG
#undef IGMP_DEBUG
#undef INET_DEBUG
#undef IP_DEBUG
#undef IP_REASS_DEBUG
#undef RAW_DEBUG
#undef MEM_DEBUG
#undef MEMP_DEBUG
#undef SYS_DEBUG
#undef TIMERS_DEBUG
#undef TCP_DEBUG
#undef TCP_INPUT_DEBUG
#undef TCP_FR_DEBUG
#undef TCP_RTO_DEBUG
#undef TCP_CWND_DEBUG
#undef TCP_WND_DEBUG
#undef TCP_OUTPUT_DEBUG
#undef TCP_RST_DEBUG
#undef TCP_QLEN_DEBUG
#undef UDP_DEBUG
#undef TCPIP_DEBUG
#undef PPP_DEBUG
#undef SLIP_DEBUG
#undef DHCP_DEBUG
#undef AUTOIP_DEBUG
#undef SNMP_MSG_DEBUG
#undef SNMP_MIB_DEBUG
#undef DNS_DEBUG

#define ETHARP_DEBUG                    0x0000U
#define NETIF_DEBUG                     0x0100U
#define PBUF_DEBUG                      0x0200U
#define API_LIB_DEBUG                   0x0300U
#define API_MSG_DEBUG                   0x0400U
#define SOCKETS_DEBUG                   0x0500U
#define ICMP_DEBUG                      0x0600U
#define IGMP_DEBUG                      0x0700U
#define INET_DEBUG                      0x0800U
#define IP_DEBUG                        0x0900U
#define IP_REASS_DEBUG                  0x0a00U
#define RAW_DEBUG                       0x0b00U
#define MEM_DEBUG                       0x0c00U
#define MEMP_DEBUG                      0x0d00U
#define SYS_DEBUG                       0x0e00U
#define TIMERS_DEBUG                    0x0f00U
#define TCP_DEBUG                       0x1000U
#define TCP_INPUT_DEBUG                 0x1100U
#define TCP_FR_DEBUG                    0x1200U
#define TCP_RTO_DEBUG                   0x1300U
#define TCP_CWND_DEBUG                  0x1400U
#define TCP_WND_DEBUG                   0x1500U
#define TCP_OUTPUT_DEBUG                0x1600U
#define TCP_RST_DEBUG                   0x1700U
#define TCP_QLEN_DEBUG                  0x1800U
#define UDP_DEBUG                       0x1900U
#define TCPIP_DEBUG                     0x1a00U
#define PPP_DEBUG                       0x1b00U
#define SLIP_DEBUG                      0x1c00U
#define DHCP_DEBUG                      0x1d00U
#define AUTOIP_DEBUG                    0x1e00U
#define SNMP_MSG_DEBUG                  0x1f00U
#define SNMP_MIB_DEBUG                  0x2000U
#define DNS_DEBUG                       0x2100U
#endif

/* enable netconn type raw for ping */
#define LWIP_RAW                        1

#endif /* __LWIPOPTS_H__ */
