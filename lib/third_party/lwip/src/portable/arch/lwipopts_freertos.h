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

#ifndef __LWIPOPTS_FREERTOS_H__
#define __LWIPOPTS_FREERTOS_H__

#include "FreeRTOSConfig.h"

/* tcpip_thread should run on HIGH priority */
#define TCPIP_THREAD_NAME              "lwIP"
#define TCPIP_THREAD_STACKSIZE         (512 * 4)
#define TCPIP_THREAD_PRIO              tskIDLE_PRIORITY + 5


#define TCPIP_MBOX_SIZE                 16
#define DEFAULT_RAW_RECVMBOX_SIZE       16
#define DEFAULT_UDP_RECVMBOX_SIZE       16
#define DEFAULT_TCP_RECVMBOX_SIZE       16
#define DEFAULT_ACCEPTMBOX_SIZE         16

//fix http IOT issue
#define LWIP_WND_SCALE                  1
#define TCP_RCV_SCALE                   1
#define MEMP_NUM_NETDB                  4

/*
 *-----------------------------
 * Socket options
 * ----------------------------
 */
#define LWIP_NETBUF_RECVINFO            1
#define LWIP_SOCKET                     1

//fix reuse address issue
#define SO_REUSE                        1
#define LWIP_SO_SNDTIMEO                1
#define LWIP_SO_RCVTIMEO                1
//for ip display
#define LWIP_NETIF_STATUS_CALLBACK      1

#define ETH_PAD_SIZE                    0


/**
 * LWIP_EVENT_API and LWIP_CALLBACK_API: Only one of these should be set to 1.
 *     LWIP_EVENT_API==1: The user defines lwip_tcp_event() to receive all
 *         events (accept, sent, etc) that happen in the system.
 *     LWIP_CALLBACK_API==1: The PCB callback function is called directly
 *         for the event. This is the default.
 */
#define LWIP_EVENT_API                  0
#define LWIP_CALLBACK_API               1

/*
   ------------------------------------
   ---------- Memory options ----------
   ------------------------------------
 */

/* MEM_USE_EXTERNAL_MGMT: set to 1 when using FreeRTOS
 * memory management. The size of the memory should be set
 * in FreeRTOSConfig.h
 */
#define MEM_USE_EXTERNAL_MGMT 1

/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
#define MEM_ALIGNMENT           4

/* MEMP_MEM_MALLOC: 1 to use mem_malloc/mem_free instead of the lwip pool
   allocator. Especially useful with MEM_LIBC_MALLOC but handle with care
   regarding execution speed and usage from interrupts! */
#define MEMP_MEM_MALLOC         1


/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT    1

/*
   ------------------------------------------------
   ---------- Internal Memory Pool Sizes ----------
   ------------------------------------------------
*/

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#define MEMP_NUM_PBUF           8

/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#define MEMP_NUM_UDP_PCB        8

/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. */
#define MEMP_NUM_TCP_PCB        32

/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#define MEMP_NUM_TCP_PCB_LISTEN 32  //16 original

/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#define MEMP_NUM_TCP_SEG        255

/* MEMP_NUM_ARP_QUEUE: the number of simulateously queued outgoing
   packets (pbufs) that are waiting for an ARP request (to resolve
   their destination address) to finish. (requires the ARP_QUEUEING option) */
#define MEMP_NUM_ARP_QUEUE      8

/**
 * MEMP_NUM_NETCONN: the number of struct netconns.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#define MEMP_NUM_NETCONN        32


/*
   ----------------------------------
   ---------- Pbuf options ----------
   ----------------------------------
 */


/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#define PBUF_POOL_SIZE          10

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool.
   MT7687 packets have extra TXD header and packet offset. */
#define PBUF_POOL_BUFSIZE       (1536 + 128)

#define RESERVED_HLEN            0    //depend on CFG_CONNSYS_TXD_PAD_SIZE

#define PBUF_LINK_ENCAPSULATION_HLEN 0


/* PBUF_LINK_HLEN: the number of bytes that should be allocated for a
   link level header. The default is 14, the standard value for
   Ethernet. */
#define PBUF_LINK_HLEN           (RESERVED_HLEN + 14 + ETH_PAD_SIZE)


/*
   ---------------------------------
   ---------- TCP options ----------
   ---------------------------------
 */


/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ         1

/* TCP Maximum segment size. */
#define TCP_MSS                 1476

/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF             (24 * 1024) //(12 * 1024)

/* TCP sender buffer space (pbufs). This must be at least =
   2 * TCP_SND_BUF/TCP_MSS for things to work. */
#define TCP_SND_QUEUELEN        ((4 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS))

/* TCP receive window. */
#define TCP_WND                 (24 * 1024)

/* Maximum number of retransmissions of data segments. */
#define TCP_MAXRTX              12

/* Maximum number of retransmissions of SYN segments. */
#define TCP_SYNMAXRTX           4

/**
 * LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS==1: randomize the local port for the first
 * local TCP/UDP pcb (default==0). This can prevent creating predictable port
 * numbers after booting a device.
 */
#define LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS 1


/*
   ---------------------------------
   ---------- ARP options ----------
   ---------------------------------
 */


/**
 * ARP_QUEUEING==1: Multiple outgoing packets are queued during hardware address
 * resolution. By default, only the most recent packet is queued per IP address.
 * This is sufficient for most protocols and mainly reduces TCP connection
 * startup time. Set this to 1 if you know your application sends more than one
 * packet in a row to an IP address that is not in the ARP cache.
 */
#define ARP_QUEUEING            1


/*
   ---------------------------------
   ----------- IP options ----------
   ---------------------------------
 */


/**
 * IP_REASSEMBLY==1: Reassemble incoming fragmented IP packets. Note that
 * this option does not affect outgoing packet sizes, which can be controlled
 * via IP_FRAG.
 */
#define IP_REASSEMBLY           1

/**
 * IP_REASS_MAX_PBUFS: Total maximum amount of pbufs waiting to be reassembled.
 * Since the received pbufs are enqueued, be sure to configure
 * PBUF_POOL_SIZE > IP_REASS_MAX_PBUFS so that the stack is still able to receive
 * packets even if the maximum amount of fragments is enqueued for reassembly!
 */

#define IP_REASS_MAX_PBUFS      10

/*
   ---------------------------------
   ---------- DHCP options ---------
   ---------------------------------
 */


/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. */
#define LWIP_DHCP               1

/**
 * LWIP_DHCP_CHECK_LINK_UP==1: dhcp_start() only really starts if the netif has
 * NETIF_FLAG_LINK_UP set in its flags. As this is only an optimization and
 * netif drivers might not set this flag, the default is off. If enabled,
 * netif_set_link_up() must be called to continue dhcp starting.
 */
#define LWIP_DHCP_CHECK_LINK_UP 1

/* 1 if you want to do an ARP check on the offered address
   (recommended). */

/**
 * DHCP_DOES_ARP_CHECK==1: Do an ARP check on the offered address.
 */
#define DHCP_DOES_ARP_CHECK     0


/*
   ----------------------------------
   ---------- DNS options -----------
   ----------------------------------
*/


#define LWIP_DNS               1


/*
   ------------------------------------------------
   ---------- Network Interfaces options ----------
   ------------------------------------------------
 */


/**
 * LWIP_NETIF_LOOPBACK==1: Support sending packets with a destination IP
 * address equal to the netif IP address, looping them back up the stack.
 */
#define LWIP_NETIF_LOOPBACK     1


/**
 * LWIP_LOOPBACK_MAX_PBUFS: Maximum number of pbufs on queue for loopback
 * sending for each netif (0 = disabled)
 */
#define LWIP_LOOPBACK_MAX_PBUFS 12


/*
   ------------------------------------
   ---------- LOOPIF options ----------
   ------------------------------------
 */


/* LWIP_HAVE_LOOPIF==1: Support loop interface (127.0.0.1) and loopif.c */
#define LWIP_HAVE_LOOPIF        1


/*
   ------------------------------------
   ----------- Debug options ----------
   ------------------------------------
 */

//#define LWIP_DEBUG
//#define UDP_DEBUG LWIP_DBG_ON
//#define SOCKETS_DEBUG LWIP_DBG_ON
#define NETIF_DEBUG LWIP_DBG_ON
//#define ETHARP_DEBUG LWIP_DBG_ON
//#define DHCP_DEBUG LWIP_DBG_ON
//#define IP_DEBUG LWIP_DBG_ON


/*
 * Use the errno.h mechanism from FreeRTOS.
 */
#if ( configUSE_POSIX_ERRNO == 1 )
    /**
     * @brief System error variable, errno.
     */
    extern int FreeRTOS_errno;
    #define errno FreeRTOS_errno
#endif

#endif /* __LWIPOPTS_FREERTOS_H__ */
