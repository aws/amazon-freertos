/*
 * Copyright 2020 Cypress Semiconductor Corporation
 * SPDX-License-Identifier: Apache-2.0
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file
 *
 * Network helper library for FreeRTOS
 */

#include "lwip/dhcp.h"
#include "lwip/ip4_addr.h"      // NOTE: LwIP specific - ip4_addr
#include "lwip/netif.h"         // NOTE: LwIP specific - for etharp_cleanup_netif()
#include "lwip/etharp.h"        // NOTE: LwIP specific - for netif_list for use in etharp_cleanup_netif() call
#include "cy_nw_helper.h"
#include "cyabs_rtos.h"
#include "iot_wifi_common.h"

#ifdef __cplusplus
extern "C" {
#endif

static struct netif *nw_get_our_netif(void)
{
    struct netif *net = cy_lwip_get_interface();
    return net;
}

void cy_nw_ip_initialize_status_change_callback(cy_nw_ip_status_change_callback_t *cb, cy_nw_ip_status_change_callback_func_t *cb_func, void *arg)
{
    /* This function is not implemented for COMPONENT_LWIP. Expectation is to use the Wi-Fi Connection Manager library (WCM) for IP status change callback */
    printf ("Not implemented for COMPONENT_LWIP. Use Wi-Fi Connection Manager WCM APIs instead\n");
    return;
}

bool cy_nw_ip_get_ipv4_address(cy_nw_ip_interface_t nw_interface, cy_nw_ip_address_t *ip_addr)
{
   struct netif *net = cy_lwip_get_interface() ;

   if (net != NULL)
   {
        uint32_t ipv4 = net->ip_addr.u_addr.ip4.addr;

        if (ipv4 != 0UL)
        {
            if (ip_addr != NULL)
            {
                ip_addr->ip.v4 = ipv4;
                ip_addr->version = NW_IP_IPV4;
                return true;
            }
        }
    }
    return false;
}

int cy_nw_str_to_ipv4(const char *ip_str, cy_nw_ip_address_t *address)
{
    ip4_addr_t addr;
    if (ip_str == NULL || address == NULL)
    {
        return -1;
    }
    
    if (ip4addr_aton(ip_str, &addr))
    {
        address->ip.v4 = addr.addr;
        address->version = NW_IP_IPV4;
        return 0;
    }
    return -1;
}

void cy_nw_ip_register_status_change_callback(cy_nw_ip_interface_t nw_interface, cy_nw_ip_status_change_callback_t *cb)
{
    /* This function is not implemented for COMPONENT_LWIP. Expectation is to use the Wi-Fi Connection Manager library (WCM) for IP status change callback */
    printf ("Not implemented for COMPONENT_LWIP. Use Wi-Fi Connection Manager WCM APIs instead\n");
    return;
}

void cy_nw_ip_unregister_status_change_callback(cy_nw_ip_interface_t nw_interface, cy_nw_ip_status_change_callback_t *cb)
{
    /* This function is not implemented for COMPONENT_LWIP. Expectation is to use the Wi-Fi Connection Manager library (WCM) for IP status change callback */
    printf ("Not implemented for COMPONENT_LWIP. Use Wi-Fi Connection Manager WCM APIs instead\n");
    return;
}

int cy_nw_host_arp_cache_clear( cy_nw_ip_interface_t iface )
{
    struct netif *netif;

    netif = nw_get_our_netif();
    if( netif != NULL)
    {
        etharp_cleanup_netif(netif);
    }
    return 0;
}

int cy_nw_host_arp_cache_get_list( cy_nw_ip_interface_t iface, cy_nw_arp_cache_entry_t *list, uint32_t count, uint32_t *filled)
{
    uint8_t index;
    struct ip4_addr *ipaddr_ptr;
    struct netif *netif_ptr;
    struct eth_addr *eth_ret_ptr;
    struct netif *netif;

    *filled = 0;

    netif = nw_get_our_netif();
    if( netif != NULL)
    {
        for (index =0 ; index < ARP_TABLE_SIZE; index++)
        {
            if (etharp_get_entry(index, &ipaddr_ptr, &netif_ptr, &eth_ret_ptr) == 1)
            {
                list[*filled].ip.version = NW_IP_IPV4;
                list[*filled].ip.ip.v4 = ipaddr_ptr->addr;
                memcpy(list[*filled].mac.mac, eth_ret_ptr->addr, sizeof(list->mac.mac));

                *filled = *filled + 1;
                if (*filled >= count)
                {
                    break;
                }
            }
            else
            {

            }
        }
        /* The query was successful */
        return 0;
    }

    return 1;
}

int cy_nw_host_send_arp_request( cy_nw_ip_interface_t ipiface, const char *ip_string)
{
    ip4_addr_t ipv4addr;
    struct netif *netif;

    ipv4addr.addr = ipaddr_addr(ip_string);
    netif = nw_get_our_netif();
    
    if( netif != NULL)
    {
        err_t err = etharp_request(netif, &ipv4addr);
        return err;
    }
    return 1;
}

uint32_t cy_nw_get_time (void)
{
    cy_time_t systime;
    cy_rtos_get_time(&systime);
    return systime;
}

#ifdef __cplusplus
}
#endif
