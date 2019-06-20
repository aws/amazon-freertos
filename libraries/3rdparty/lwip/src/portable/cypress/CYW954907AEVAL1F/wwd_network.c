/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/******************************************************************************
 Wiced Includes
 ******************************************************************************/

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

/*****************************************************************************
 * The following is based on the skeleton Ethernet driver in LwIP            *
 *****************************************************************************/


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
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

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


/* Ethertype packet filtering support. */
static uint16_t filter_ethertype;
static wwd_interface_t filter_interface;
static wwd_network_filter_ethernet_packet_t filter_ethernet_packet_callback;
static void* filter_userdata;



/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param p : the incoming ethernet packet
 */
void host_network_process_ethernet_data( /*@only@*/ wiced_buffer_t buffer, wwd_interface_t interface )
{
    struct eth_hdr* ethernet_header;
    struct netif*   tmp_netif;
    u8_t            result;
    uint16_t        ethertype;

    if ( buffer == NULL )
        return;

    /* points to packet payload, which starts with an Ethernet header */
    ethernet_header = (struct eth_hdr *) buffer->payload;

    ethertype = htons( ethernet_header->type );

    if (filter_ethernet_packet_callback != NULL && filter_ethertype == ethertype && filter_interface == interface)
    {
        filter_ethernet_packet_callback(buffer->payload, filter_userdata);
    }

    /* Check if this is an 802.1Q VLAN tagged packet */
    if ( ethertype == WICED_ETHERTYPE_8021Q )
    {
        /* Need to remove the 4 octet VLAN Tag, by moving src and dest addresses 4 octets to the right,
         * and then read the actual ethertype. The VLAN ID and priority fields are currently ignored. */
        uint8_t temp_buffer[ 12 ];
        memcpy( temp_buffer, buffer->payload, 12 );
        memcpy( ( (uint8_t*) buffer->payload ) + 4, temp_buffer, 12 );

        buffer->payload = ( (uint8_t*) buffer->payload ) + 4;
        buffer->len = (u16_t) ( buffer->len - 4 );

        ethernet_header = (struct eth_hdr *) buffer->payload;
        ethertype = htons( ethernet_header->type );
    }

    if ( WICED_DEEP_SLEEP_IS_ENABLED() && ( WICED_DEEP_SLEEP_SAVE_PACKETS_NUM != 0 ) )
    {
        if ( wiced_deep_sleep_save_packet( buffer, interface ) )
        {
            return;
        }
    }

    switch ( ethertype )
    {
        case WICED_ETHERTYPE_IPv4:
        case WICED_ETHERTYPE_ARP:
#if PPPOE_SUPPORT
        /* PPPoE packet? */
        case ETHTYPE_PPPOEDISC:
        case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
            /* Find the netif object matching the provided interface */
            for ( tmp_netif = netif_list; ( tmp_netif != NULL ) && ( tmp_netif->state != (void*) interface ); tmp_netif = tmp_netif->next )
            {
            }

            if ( tmp_netif == NULL )
            {
                /* Received a packet for a network interface is not initialised Cannot do anything with packet - just drop it. */
                result = pbuf_free( buffer );
                LWIP_ASSERT("Failed to release packet buffer", ( result != (u8_t)0 ) );
                buffer = NULL;
                return;
            }

            /* Send to packet to tcpip_thread to process */
            if ( tcpip_input( buffer, tmp_netif ) != ERR_OK )
            {
                LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));

                /* Stop lint warning - packet has not been released in this case */ /*@-usereleased@*/
                result = pbuf_free( buffer );
                /*@+usereleased@*/
                LWIP_ASSERT("Failed to release packet buffer", ( result != (u8_t)0 ) );
                buffer = NULL;
            }
            break;

        case WICED_ETHERTYPE_EAPOL:
            wwd_eapol_receive_eapol_packet( buffer, interface );
            break;

        default:
            result = pbuf_free( buffer );
            LWIP_ASSERT("Failed to release packet buffer", ( result != (u8_t)0 ) );
            buffer = NULL;
            break;
    }
}

void host_network_set_ethertype_filter( uint16_t ethertype, wwd_interface_t interface, wwd_network_filter_ethernet_packet_t ethernet_packet_callback, void* userdata )
{
    filter_ethertype                = ethertype;
    filter_interface                = interface;
    filter_ethernet_packet_callback = ethernet_packet_callback;
    filter_userdata                 = userdata;
}
/********************************** End of file ******************************************/
