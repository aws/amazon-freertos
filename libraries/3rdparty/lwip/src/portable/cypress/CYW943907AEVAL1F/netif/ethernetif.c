/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
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

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include <string.h>
#include "lwip/tcpip.h"
#include "lwip/debug.h"
#include "lwip/netif.h"
#include "lwip/igmp.h"
#include "lwip/ethip6.h"
#include "netif/ppp/ppp_opts.h"
#include "netif/etharp.h"
#include "ethernetif.h"
#include "wwd_wifi.h"
#include "wwd_eapol.h"
#include "network/wwd_network_interface.h"
#include "network/wwd_buffer_interface.h"
#include "wwd_assert.h"
#include "wiced_constants.h"
#include <stdlib.h>
#include "wiced_low_power.h"
#include "wiced_management.h"

#define ETHTYPE_EAPOL    0x888E

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "wwd_bus_protocol.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'w'
#define IFNAME1 'l'

#define MULTICAST_IP_TO_MAC(ip)       { (uint8_t) 0x01,             \
                                        (uint8_t) 0x00,             \
                                        (uint8_t) 0x5e,             \
                                        (uint8_t) ((ip)[1] & 0x7F), \
                                        (uint8_t) (ip)[2],          \
                                        (uint8_t) (ip)[3]           \
                                      }


/* Forward declarations. */
#if LWIP_IGMP
static err_t lwip_igmp_mac_filter(struct netif *netif,
       const ip4_addr_t *group, enum netif_mac_filter_action action);
#endif

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init( /*@partial@*/ struct netif *netif )
{
    /* Set MAC hardware address length */
    netif->hwaddr_len = (u8_t) ETHARP_HWADDR_LEN;

    /* Setup the physical address of this IP instance. */
    if ( wwd_wifi_get_mac_address( (wiced_mac_t*) ( netif->hwaddr ), (wwd_interface_t) netif->state ) != WWD_SUCCESS )
    {
        WPRINT_NETWORK_DEBUG(("Couldn't get MAC address\n"));
        return;
    }

    /* Set Maximum Transfer Unit */
    netif->mtu = (u16_t) WICED_PAYLOAD_MTU;

    /* Set device capabilities. Don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags = (u8_t) ( NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP );

    /* Do whatever else is needed to initialize interface. */
#if LWIP_IGMP
    netif->flags |= NETIF_FLAG_IGMP;
    netif_set_igmp_mac_filter(netif, lwip_igmp_mac_filter);
#endif
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
    /*@-noeffect@*/
    UNUSED_PARAMETER( netif );
    /*@+noeffect@*/

    if ( ( (wiced_interface_t) netif->state  == WICED_ETHERNET_INTERFACE ) ||
         ( wwd_wifi_is_ready_to_transceive( (wwd_interface_t) netif->state ) == WWD_SUCCESS ) )
    {
        /* Take a reference to this packet */
        pbuf_ref( p );

        LWIP_ASSERT( "No chained buffers", ( ( p->next == NULL ) && ( ( p->tot_len == p->len ) ) ) );
        wwd_network_send_ethernet_data( p, (wwd_interface_t) netif->state );

        LINK_STATS_INC( link.xmit );

        return (err_t) ERR_OK;
    }
    else
    {
        /* Stop lint warning about packet not being freed - it is not being referenced */ /*@-mustfree@*/
        return (err_t) ERR_INPROGRESS; /* Note that signalling ERR_CLSD or ERR_CONN causes loss of connectivity on a roam */
        /*@+mustfree@*/
    }
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

    /* Verify the netif is a valid interface */
    if ( (wwd_interface_t) netif->state > WWD_ETHERNET_INTERFACE )
    {
        return ERR_ARG;
    }

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;

    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...)
     */
    netif->output = etharp_output;
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif
    netif->linkoutput = low_level_output;

    /* Initialize the hardware */
    low_level_init( netif );

    return ERR_OK;
}

#if LWIP_IGMP
/**
 * Interface between LwIP IGMP MAC filter and WICED MAC filter
 */
static err_t lwip_igmp_mac_filter(struct netif *netif,
       const ip4_addr_t *group, enum netif_mac_filter_action action)
{
    wiced_mac_t mac = { MULTICAST_IP_TO_MAC((uint8_t*)group) };
    /*@-noeffect@*/
    UNUSED_PARAMETER(netif);
    /*@+noeffect@*/

    switch ( action )
    {
        case IGMP_ADD_MAC_FILTER:
            if ( wwd_wifi_register_multicast_address( &mac ) != WWD_SUCCESS )
            {
                return ERR_VAL;
            }
            break;

        case IGMP_DEL_MAC_FILTER:
            if ( wwd_wifi_unregister_multicast_address( &mac ) != WWD_SUCCESS )
            {
                return ERR_VAL;
            }
            break;

        default:
            return ERR_VAL;
    }

    return ERR_OK;
}
#endif
/********************************** End of file ******************************************/
