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


#include "FreeRTOS.h"
#include "list.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#include <wlan.h>
#include <wm_net.h>

extern void vNetworkNotifyIFUp();
extern void vNetworkNotifyIFDown();

#define MLAN_HANDLE 1
#define UAP_HANDLE 2
#define netif_e(...)                             \
	        wmlog_e("net", ##__VA_ARGS__)

#define netif_w(...)      \
	        wmlog("net", ##__VA_ARGS__)

#ifdef CONFIG_NET_DEBUG
#define netif_d(...)                              \
	        wmlog("net", ##__VA_ARGS__)
#else
#define netif_d(...)
#endif /* ! CONFIG_NET_DEBUG */

void handle_data_packet(const t_u8 interface, const t_u8 *rcvdata,
		                        const t_u16 datalen);

void net_wlan_init(void)
{
	static int net_wlan_init_done;
	
	if (!net_wlan_init_done) {
		wifi_register_data_input_callback(&handle_data_packet);
		wlan_wlcmgr_send_msg(WIFI_EVENT_NET_INTERFACE_CONFIG,WIFI_EVENT_REASON_SUCCESS, NULL);
		net_wlan_init_done = 1;
		netif_d("Initialized TCP/IP networking stack");
	}
}

#if 0
void net_inet_ntoa(unsigned long addr, char *cp)
{
        struct in_addr saddr;
        saddr.s_addr = addr;
        /* No length, sigh! */
        strcpy(cp, inet_ntoa(saddr));
	FreeRTOS_inet_ntoa( (uint32_t)addr, cp );
}

#endif

//char *inet_ntoa(ip_addr_t *ip_addr)
void ipaddr_ntoa(ip_addr_t *ip_addr)
{
	static char cbuffer[16];
	FreeRTOS_inet_ntoa( (uint32_t)(ip_addr->addr), cbuffer);
}


int net_get_if_addr(struct wlan_ip_config *addr, void *intrfc_handle)
{
        addr->ipv4.address = FreeRTOS_GetIPAddress();
        addr->ipv4.netmask = FreeRTOS_GetNetmask();
        addr->ipv4.gw = FreeRTOS_GetGatewayAddress();
        addr->ipv4.dns1 = FreeRTOS_GetDNSServerAddress();

        return WM_SUCCESS;
}


void net_configure_dns(struct wlan_ip_config *ip, enum wlan_bss_role role)
{
        /*
         * Skip this?
         */
}


void net_interface_down(void *intrfc_handle)
{
        vNetworkNotifyIFDown();
}

void net_interface_dhcp_stop(void *intrfc_handle)
{
        /*
         * Before this call to net_interface_down is made
         * which stop dhcp
         */
}

#include <FreeRTOS_IP_Private.h>

int net_configure_address(struct wlan_ip_config *addr, void *intrfc_handle)
{

        if (!addr)
                return -WM_E_INVAL;
        if (!intrfc_handle)
                return -WM_E_INVAL;

        wmprintf("configuring interface (with %s)",
                (addr->ipv4.addr_type == ADDR_TYPE_DHCP)
                ? "DHCP client" : "Static IP");

        switch (addr->ipv4.addr_type){
        case ADDR_TYPE_STATIC:
	        /* Finally this should send the following event. */
		wlan_wlcmgr_send_msg(WIFI_EVENT_NET_STA_ADDR_CONFIG,
			        WIFI_EVENT_REASON_SUCCESS, NULL);
                vNetworkNotifyIFUp();
                break;

        case ADDR_TYPE_DHCP:
		wlan_wlcmgr_send_msg(WIFI_EVENT_NET_STA_ADDR_CONFIG,
			        WIFI_EVENT_REASON_SUCCESS, NULL);
		*ipLOCAL_IP_ADDRESS_POINTER = 0x00UL;
		/* The network is not up until DHCP has completed. */
		xSendEventToIPTask( eDHCPEvent );

		while (*ipLOCAL_IP_ADDRESS_POINTER == 0)
			os_thread_sleep(10);
	        /* Finally this should send the following event. */
		wlan_wlcmgr_send_msg(WIFI_EVENT_NET_DHCP_CONFIG,
			        WIFI_EVENT_REASON_SUCCESS, NULL);
		vNetworkNotifyIFUp();
                break;
        default:
                break;
	}



        return 0;
}


void *net_get_mlan_handle()
{
        //return &g_mlan;
/*
 * return of net_get_mlan_handle() needs to ne not null.
 * it is not processed by wlcmgr
 */
        return (void *)MLAN_HANDLE;
}

void *net_get_uap_handle(void)
{
        //return &g_uap;
        return (void *)UAP_HANDLE;
}

