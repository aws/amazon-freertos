/*
 * Copyright 2019-2020 Cypress Semiconductor Corporation
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
 * Network helper library
 */
#include "mbed.h"

#include "lwip/dhcp.h"
#include "lwip/ip4_addr.h"      // NOTE: LwIP specific - ip4_addr
#include "lwip/netif.h"         // NOTE: LwIP specific - for etharp_cleanup_netif()
#include "lwip/etharp.h"        // NOTE: LwIP specific - for netif_list for use in etharp_cleanup_netif() call

#include "cy_nw_helper.h"
#include "NetworkInterface.h"
#include "WhdSTAInterface.h"
#include "ip4string.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * NOTE:
 * =====
 * This implementation is specific to mbed-os flavor.
 * The APIs defined in this file should be implemented for other OS/RTOS/Frameworks when it's getting ported on other frameworks.
 *
 */
static struct netif *nw_get_our_netif( WhdSTAInterface *iface )
{
    char iface_name[4];     /* typically "st0" */
    struct netif *netif;


    if ( iface->get_interface_name(iface_name) != NULL)
    {
        netif = netif_list;                     // NOTE: This is an LwIP global - refer but do not change
        while( netif != NULL)
        {
            char *net_iface_name = netif->name;
            uint8_t index = netif->num;
            if ( (iface_name[0] == net_iface_name[0]) &&
                 (iface_name[1] == net_iface_name[1]) &&
                 (iface_name[2] == ( index + '0') ) )
            {

                return netif;
            }
            netif = netif->next;
        }
    }
    return NULL;
}

/*
 * The only event we care about is
 *   NSAPI_EVENT_CONNECTION_STATUS_CHANGE
 *
 * These are the values given to us in the "val" argument:
 *  NSAPI_STATUS_LOCAL_UP           = 0,        local IP address set
 *  NSAPI_STATUS_GLOBAL_UP          = 1,        global IP address set
 *  NSAPI_STATUS_DISCONNECTED       = 2,        no connection to network
 *  NSAPI_STATUS_CONNECTING         = 3,        connecting to network
 *      We notice that this is what is set for ARP OL Functional test F5.
 *      When we see this, call dhcp_network_changed to get the dhcp to reconnect.
 */
static void nw_ip_status_change_handler(cy_nw_ip_status_change_callback_t *cb, nsapi_event_t event, intptr_t val)
{
    if (event == NSAPI_EVENT_CONNECTION_STATUS_CHANGE)
    {
        /* We tell the dhcp server that the ip address is NULL to force DHCP to renew */
        if (val == NSAPI_STATUS_CONNECTING)
        {
            struct netif *netiface = nw_get_our_netif( (WhdSTAInterface *)cb->priv );
            netif_set_addr( netiface, NULL, NULL, NULL);
        }
        if (cb->cb_func != NULL )
        {
            (*cb->cb_func)(reinterpret_cast<cy_nw_ip_interface_t>(cb->priv), cb->arg);
        }
    }
}

void cy_nw_ip_initialize_status_change_callback(cy_nw_ip_status_change_callback_t *cb, cy_nw_ip_status_change_callback_func_t *cb_func, void *arg)
{
    cb->cb_func = cb_func;
    cb->arg = arg;
    cb->priv = 0;
}

bool cy_nw_ip_get_ipv4_address(cy_nw_ip_interface_t nw_interface, cy_nw_ip_address_t *ip_addr)
{
    WhdSTAInterface *iface =  (WhdSTAInterface *)nw_interface;
    if (iface != NULL)
    {
        const char *addr;
        SocketAddress address;
        uint32_t ipv4 = 0UL;
        nsapi_error_t rc = NSAPI_ERROR_OK;
        if (iface->is_interface_connected() != WHD_SUCCESS)
        {
            return false;
        }

        rc = iface->get_ip_address(&address);
        if (rc == NSAPI_ERROR_OK)
        {
            addr = (const char *) address.get_ip_address();
        }
        else
        {
            return false;
        }

        if ( (stoip4(addr, strlen(addr), &ipv4) != 0) && (ipv4 != 0UL) )
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
    if (ip_str == NULL || address == NULL)
    {
        return -1;
    }

    if (stoip4(ip_str, strlen(ip_str), &address->ip.v4))
    {
        address->version = NW_IP_IPV4;
        return 0;
    }

    return -1;
}

void cy_nw_ip_register_status_change_callback(cy_nw_ip_interface_t nw_interface, cy_nw_ip_status_change_callback_t *cb)
{
    NetworkInterface *iface = reinterpret_cast<NetworkInterface*>(nw_interface);

    cb->priv = iface;
    iface->add_event_listener(mbed::callback(nw_ip_status_change_handler, cb));
}

void cy_nw_ip_unregister_status_change_callback(cy_nw_ip_interface_t nw_interface, cy_nw_ip_status_change_callback_t *cb)
{
    NetworkInterface *iface = reinterpret_cast<NetworkInterface*>(nw_interface);

    cb->priv = 0;
    iface->remove_event_listener(mbed::callback(nw_ip_status_change_handler, cb));
}

int cy_nw_host_arp_cache_clear( cy_nw_ip_interface_t iface )
{
    WhdSTAInterface *ifp =  (WhdSTAInterface *)iface;
    struct netif *netif;

    netif = nw_get_our_netif( ifp );
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
    WhdSTAInterface *ifp =  (WhdSTAInterface *)iface;

    if ((ifp == NULL) || (list == NULL) || (count == 0) || (filled == NULL))
    {
        return 1;
    }

    *filled = 0;

    netif = nw_get_our_netif( ifp );
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

int cy_nw_host_send_arp_request( cy_nw_ip_interface_t iface, const char *ip_string)
{
    ip4_addr_t ipv4addr;
    struct netif *netif;
    WhdSTAInterface *ifp =  (WhdSTAInterface *)iface;

    stoip4(ip_string, strlen(ip_string), &ipv4addr.addr);

    netif = nw_get_our_netif( ifp );
    if( netif != NULL)
    {
        err_t err = etharp_request(netif, &ipv4addr);
        return err;
    }
    return 1;
}

uint32_t cy_nw_get_time (void)
{
    /* Get Number of ticks per second */
    uint32_t tick_freq = osKernelGetTickFreq();

    /* Convert ticks count to time in milliseconds */
    return (osKernelGetTickCount() * (1000 / tick_freq) );
}

#ifdef __cplusplus
}
#endif
