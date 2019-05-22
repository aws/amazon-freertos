/*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
*
*  MARVELL CONFIDENTIAL
*  The source code contained or described herein and all documents related to
*  the source code ("Material") are owned by Marvell International Ltd or its
*  suppliers or licensors. Title to the Material remains with Marvell
*  International Ltd or its suppliers and licensors. The Material contains
*  trade secrets and proprietary and confidential information of Marvell or its
*  suppliers and licensors. The Material is protected by worldwide copyright
*  and trade secret laws and treaty provisions. No part of the Material may be
*  used, copied, reproduced, modified, published, uploaded, posted,
*  transmitted, distributed, or disclosed in any way without Marvell's prior
*  express written permission.
*
*  No license under any patent, copyright, trade secret or other intellectual
*  property right is granted to or conferred upon you by disclosure or delivery
*  of the Materials, either expressly, by implication, inducement, estoppel or
*  otherwise. Any license under such intellectual property rights must be
*  express and approved by Marvell in writing.
*
*/

#ifndef _WM_NET_H_
#define _WM_NET_H_

#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include <wlan.h>

#define htonl(x) FreeRTOS_htonl(x)
#define  EINVAL       22

/* If your port already typedef's in_addr_t, define IN_ADDR_T_DEFINED
   to prevent this code from redefining it. */
//#if !defined(in_addr_t) && !defined(IN_ADDR_T_DEFINED)
//typedef u32_t in_addr_t;

typedef uint32_t in_addr_t;
//#endif
struct in_addr {
  in_addr_t s_addr;
};// check this


/* This is the aligned version of ip_addr_t,
   used as local variable, on the stack, etc. */
struct ip_addr {
  u32_t addr;
};

typedef struct ip_addr ip_addr_t;

void net_wlan_init(void);

int net_get_if_addr(struct wlan_ip_config *addr, void *intrfc_handle);

void net_configure_dns(struct wlan_ip_config *ip, enum wlan_bss_role role);

void net_interface_down(void *intrfc_handle);

void net_interface_dhcp_stop(void *intrfc_handle);

int net_configure_address(struct wlan_ip_config *addr, void *intrfc_handle);

void *net_get_mlan_handle();

void *net_get_uap_handle(void);

//char *inet_ntoa(ip_addr_t *addr);
void ipaddr_ntoa(ip_addr_t *ip_addr);
#define inet_ntoa(addr) ipaddr_ntoa((ip_addr_t*)&(addr))

static inline void net_inet_ntoa(unsigned long addr, char *cp)
{
        FreeRTOS_inet_ntoa( (uint32_t)addr, cp );
}

static inline uint32_t net_inet_aton(const char *cp)
{
	uint32_t dummy = 0;
#if 0
        struct in_addr addr;
        inet_aton(cp, &addr);
        return addr.s_addr;
#endif
	return dummy;
}








//// pbuff used only for wrapper_wlan_handle_rx_packet ///////////

struct pbuf {
  /** next pbuf in singly linked pbuf chain */
  struct pbuf *next;

  /** pointer to the actual data in the buffer */
  void *payload;

  /**
   * total length of this buffer and all next buffers in chain
   * belonging to the same packet.
   *
   * For non-queue packet chains this is the invariant:
   * p->tot_len == p->len + (p->next? p->next->tot_len: 0)
   */
  uint16_t tot_len;

  /** length of this buffer */
  uint16_t len;

  /** pbuf_type as u8_t instead of enum to save space */
  uint8_t /*pbuf_type*/ type;

  /** misc flags */
  uint8_t flags;

  /**
   * the reference count always equals the number of pointers
   * that refer to this pbuf. This can be pointers from an application,
   * the stack itself, or pbuf->next pointers from a chain.
   */
  uint16_t ref;
};
#endif // _WM_NET_H_

