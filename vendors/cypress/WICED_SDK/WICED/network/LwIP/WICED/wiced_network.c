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

/** @file
 */

#include <string.h>
#include "wiced.h"
#include "wiced_network.h"
#include "wiced_utilities.h"
#include "wwd_debug.h"
#include "wwd_assert.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "netif/etharp.h"
#include "lwip/netifapi.h"
#include "lwip/prot/dhcp.h"
#include "lwip/dhcp.h"
#include "lwip/prot/autoip.h"
#include "lwip/igmp.h"
#include "lwip/ip_addr.h"
#include "wwd_management.h"
#include "wwd_network.h"

#ifndef WICED_AMAZON_FREERTOS_SDK
#include "dhcp_server.h"
#endif

#include "dns.h"
#include "internal/wiced_internal_api.h"
#include "lwip/dns.h"
#include "lwip/nd6.h"
#ifndef WICED_DISABLE_TLS
#include "wiced_p2p.h"
#endif
#include "network/wwd_buffer_interface.h"
#include "wiced_low_power.h"

#if LWIP_NETIF_HOSTNAME
#include "platform_dct.h"

#ifdef NETWORK_CONFIG_APPLICATION_DEFINED
#include "network_config_dct.h"
#else/* #ifdef NETWORK_CONFIG_APPLICATION_DEFINED */
#include "default_network_config_dct.h"
#endif /* #ifdef NETWORK_CONFIG_APPLICATION_DEFINED */

#endif /* LWIP_NETIF_HOSTNAME */

/******************************************************
 *                      Macros
 ******************************************************/

#define CONFIG_GET_FOR_IF( interface )            ( wiced_config_cache[         ( interface ) & 3 ] )
#define CONFIG_SET_FOR_IF( interface, config )      wiced_config_cache[         ( interface ) & 3 ] = config;

/******************************************************
 *                    Constants
 ******************************************************/

#define TIME_WAIT_TCP_SOCKET_DELAY (400)
#define MAXIMUM_IP_ADDRESS_CHANGE_CALLBACKS           (2)

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    wiced_ip_address_change_callback_t callback;
    void*                              arg;
} ip_address_change_callback_t;

/******************************************************
 *                 Static Variables
 ******************************************************/

#ifdef WICED_USE_WIFI_STA_INTERFACE
    static struct netif       sta_ip_handle;
    #define STA_IP_HANDLE    &sta_ip_handle
#else
    #define STA_IP_HANDLE    NULL
#endif

#ifdef WICED_USE_WIFI_AP_INTERFACE
    static struct netif      ap_ip_handle;
    #define AP_IP_HANDLE    &ap_ip_handle
#else
    #define AP_IP_HANDLE    NULL
#endif

#ifdef WICED_USE_WIFI_P2P_INTERFACE
    static struct netif       p2p_ip_handle;
    #define P2P_IP_HANDLE    &p2p_ip_handle
#else
    #define P2P_IP_HANDLE    NULL
#endif

#if LWIP_NETIF_HOSTNAME
static wiced_hostname_t    hostname;
#endif /* LWIP_NETIF_HOSTNAME */

#if (LWIP_AUTOIP == 0)
    struct autoip
    {
        autoip_state_enum_t state;
    };
    #define autoip_set_struct(x, y)
    #define autoip_start(x) ERR_IF
    #define autoip_stop(x)
#endif

struct netif* wiced_ip_handle[ 3 ] =
{
    [WICED_STA_INTERFACE] = STA_IP_HANDLE,
    [WICED_AP_INTERFACE]  = AP_IP_HANDLE,
    [WICED_P2P_INTERFACE] = P2P_IP_HANDLE
};

static wiced_network_config_t wiced_config_cache[ WICED_INTERFACE_MAX ] =
{
    [WICED_STA_INTERFACE]      = WICED_USE_EXTERNAL_DHCP_SERVER,
    [WICED_AP_INTERFACE]       = WICED_USE_INTERNAL_DHCP_SERVER,
    [WICED_P2P_INTERFACE]      = WICED_USE_EXTERNAL_DHCP_SERVER,
    [WICED_ETHERNET_INTERFACE] = WICED_USE_EXTERNAL_DHCP_SERVER,
};

/* Network objects */
struct dhcp         wiced_dhcp_handle;
static wiced_bool_t        wiced_using_dhcp;

#ifndef WICED_AMAZON_FREERTOS_SDK
static wiced_dhcp_server_t internal_dhcp_server;
#endif

struct autoip              wiced_autoip_handle;
static wiced_bool_t        wiced_using_autoip;

/* IP status callback variables */
static ip_address_change_callback_t wiced_ip_address_change_callbacks[MAXIMUM_IP_ADDRESS_CHANGE_CALLBACKS];

const wiced_ip_address_t INITIALISER_IPV4_ADDRESS( wiced_ip_broadcast, 0xFFFFFFFF );

wiced_mutex_t            lwip_send_interface_mutex;

/* Wi-Fi power save state */
static uint8_t wifi_powersave_mode = 0;
static uint16_t wifi_return_to_sleep_delay = 0;

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static void tcpip_init_done( void* arg );
static void ip_address_changed_handler( struct netif *netif );
static void invalidate_all_arp_entries( struct netif *netif );
static wiced_result_t wiced_ip_driver_notify( wiced_interface_t interface, wiced_bool_t up );

static void wiced_call_link_up_callbacks( wiced_interface_t interface );
static void wiced_call_link_down_callbacks( wiced_interface_t interface );

/* etharp_free_entry() is an LwIP function in etharp.c. It is used in invalidate_all_arp_entries() */
extern void etharp_free_entry( int i );

/******************************************************
 *               Function Definitions
 ******************************************************/

void lwip_ip_to_wiced( wiced_ip_address_t *dest, const ip_addr_t *src )
{
    if ( IP_IS_V4( src ) )
    {
        SET_IPV4_ADDRESS( *dest, ntohl( ip_2_ip4( src )->addr ) );
    }
#if LWIP_IPV6
    else if (IP_IS_V6( src ) )
    {
        dest->version = WICED_IPV6;
        dest->ip.v6[0] = ntohl(ip_2_ip6(src)->addr[0]);
        dest->ip.v6[1] = ntohl(ip_2_ip6(src)->addr[1]);
        dest->ip.v6[2] = ntohl(ip_2_ip6(src)->addr[2]);
        dest->ip.v6[3] = ntohl(ip_2_ip6(src)->addr[3]);
    }
#endif
    else
    {
        dest->version = WICED_INVALID_IP;
    }
}

void wiced_ip_to_lwip( ip_addr_t *dest, const wiced_ip_address_t *src )
{
    switch ( src->version )
    {
        case WICED_IPV4:
            ip_addr_set_ip4_u32( dest, htonl( GET_IPV4_ADDRESS(*src) ) );
            break;

        case WICED_IPV6:
#if LWIP_IPV6
            IP_ADDR6_HOST(dest, src->ip.v6[0], src->ip.v6[1], src->ip.v6[2], src->ip.v6[3]);
            break;
#endif
        case WICED_INVALID_IP:
        default:
            ip_addr_set_any(0, dest);
    }
}

wiced_result_t wiced_network_init( void )
{
    wiced_result_t result;
    host_semaphore_type_t lwip_done_sema;

    /* Initialize the LwIP system.  */
    WPRINT_NETWORK_INFO(("Initialising LwIP " LwIP_VERSION "\n"));

    ip_networking_inited[0] = WICED_FALSE;
    ip_networking_inited[1] = WICED_FALSE;
    ip_networking_inited[2] = WICED_FALSE;

    /* Create a semaphore to signal when LwIP has finished initialising */
    result = host_rtos_init_semaphore(  &lwip_done_sema );
    if ( result != WICED_SUCCESS )
    {
        /* Could not create semaphore */
        WPRINT_NETWORK_ERROR(("Could not create LwIP init semaphore"));
        return WICED_ERROR;
    }

    /* Initialise LwIP, providing the callback function and callback semaphore */
    tcpip_init( tcpip_init_done, (void*) &lwip_done_sema );
    host_rtos_get_semaphore( &lwip_done_sema, NEVER_TIMEOUT, WICED_FALSE );
    host_rtos_deinit_semaphore( &lwip_done_sema );

#ifndef WICED_AMAZON_FREERTOS_SDK
    memset(&internal_dhcp_server, 0, sizeof(internal_dhcp_server));
#endif

    /* create a mutex for link up and down registrations */
    wiced_rtos_init_mutex( &link_subscribe_mutex );
    /* Create a mutex for UDP and TCP sending with ability to swap a default interface */
    wiced_rtos_init_mutex( &lwip_send_interface_mutex );

    memset( link_up_callbacks_wireless,   0, sizeof( link_up_callbacks_wireless ) );
    memset( link_up_callbacks_arg_wireless,   0, sizeof( link_up_callbacks_arg_wireless ) );
    memset( link_down_callbacks_wireless, 0, sizeof( link_down_callbacks_wireless ) );
    memset( link_down_callbacks_arg_wireless,   0, sizeof( link_down_callbacks_arg_wireless ) );

#ifdef WICED_USE_ETHERNET_INTERFACE
    memset( link_up_callbacks_ethernet,   0, sizeof(link_up_callbacks_ethernet) );
    memset( link_up_callbacks_arg_ethernet,   0, sizeof( link_up_callbacks_arg_ethernet ) );
    memset( link_down_callbacks_ethernet, 0, sizeof(link_down_callbacks_ethernet) );
    memset( link_down_callbacks_arg_ethernet,   0, sizeof( link_down_callbacks_arg_ethernet ) );
#endif

#if LWIP_NETIF_HOSTNAME
    /* Get hostname */
    result = wiced_network_get_hostname( &hostname );
    if( result != WICED_SUCCESS )
    {
        return result;
    }
    WPRINT_NETWORK_INFO( ("DHCP CLIENT hostname %s\n", hostname.value) );
#endif /* LWIP_NETIF_HOSTNAME */

    wiced_using_dhcp = WICED_FALSE;
    wiced_using_autoip = WICED_FALSE;

    return WICED_SUCCESS;
}

/**
 *  LwIP init complete callback
 *
 *  This function is called by LwIP when initialisation has finished.
 *  A semaphore is posted to allow the startup thread to resume, and to run the app_main function
 *
 * @param arg : the handle for the semaphore to post (cast to a void pointer)
 */

static void tcpip_init_done( void* arg )
{
    host_semaphore_type_t* LwIP_done_sema = (host_semaphore_type_t*) arg;
    host_rtos_set_semaphore( LwIP_done_sema, WICED_FALSE );
}

/* Create placeholder for application supplied TX and RX PBUF buffer pools. These pools are
 * dynamically created during runtime as opposed to general LwIP pools that are statically
 * created.
 */
#define WICED_LWIP_MEMPOOL_DECLARE(name,size,desc) \
  LWIP_MEMPOOL_DECLARE_STATS_INSTANCE(memp_stats_ ## name) \
    \
  static struct memp *memp_tab_ ## name; \
    \
  struct memp_desc memp_ ## name = { \
    DECLARE_LWIP_MEMPOOL_DESC(desc) \
    LWIP_MEMPOOL_DECLARE_STATS_REFERENCE(memp_stats_ ## name) \
    LWIP_MEM_ALIGN_SIZE(size), \
    0, \
    NULL, \
    &memp_tab_ ## name \
  };

WICED_LWIP_MEMPOOL_DECLARE( PBUF_APP_POOL_TX, PBUF_POOL_BUFSIZE + LWIP_MEM_ALIGN_SIZE(sizeof(struct pbuf_custom)), "Application TX buffers" )
WICED_LWIP_MEMPOOL_DECLARE( PBUF_APP_POOL_RX, PBUF_POOL_BUFSIZE + LWIP_MEM_ALIGN_SIZE(sizeof(struct pbuf_custom)), "Application RX buffers" )

wiced_result_t wiced_network_create_packet_pool( uint8_t* memory_pointer, uint32_t memory_size, wiced_network_packet_dir_t direction )
{
    uint32_t adjusted_element_size, adjusted_pool_size;
    struct memp_desc *pool = (direction == WICED_NETWORK_PACKET_RX) ? &memp_PBUF_APP_POOL_RX : &memp_PBUF_APP_POOL_TX;

    pool->base = LWIP_MEM_ALIGN(memory_pointer);

    /* Calculate adjusted pool size since some memory can be lost due to memory alignment operation. */
    adjusted_pool_size = (uint32_t)((memory_pointer + memory_size) - pool->base);

    /* Adjusted element size to include space for MEMP structures and custom PBUF structure */
    adjusted_element_size = MEMP_ALIGN_SIZE((uint32_t)pool->size) + MEMP_SIZE;

    /* Calculate the number of buffers that the pool can support */
    pool->num = (uint16_t)(adjusted_pool_size/adjusted_element_size);

    memp_init_pool(pool);

    host_buffer_add_application_defined_pool( pool, direction );

    return WICED_SUCCESS;
}

wiced_result_t wiced_network_deinit( void )
{
#ifndef WICED_AMAZON_FREERTOS_SDK
    /* default LwIP code does not have deinit; deinit APIs are added in LwIP code by wiced */
    tcpip_deinit( );
#endif
    wiced_rtos_deinit_mutex( &link_subscribe_mutex );
    wiced_rtos_deinit_mutex( &lwip_send_interface_mutex );
    return WICED_SUCCESS;
}

wiced_result_t wiced_network_suspend(void)
{
#ifndef WICED_AMAZON_FREERTOS_SDK
    /* default LwIP code does not have tcpip_timeouts; tcpip_timeouts APIs are added in LwIP code by wiced */
    /* Stop all registered TCP/IP timers */
    if( tcpip_deactivate_tcpip_timeouts() != ERR_OK )
    {
        return WICED_ERROR;
    }
#endif
    return WICED_SUCCESS;
}

wiced_result_t wiced_network_resume(void)
{
#ifndef WICED_AMAZON_FREERTOS_SDK
    /* default LwIP code does not have tcpip_timeouts APIs; tcpip_timeouts APIs are added in LwIP code by wiced */
    /* Resume all TCP/IP timers again */
    if( tcpip_activate_tcpip_timeouts() != ERR_OK )
    {
        return WICED_ERROR;
    }
#endif
    return WICED_SUCCESS;
}

wiced_result_t wiced_ip_up( wiced_interface_t interface, wiced_network_config_t config, const wiced_ip_setting_t* ip_settings )
{
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;
    wiced_bool_t static_ip;
    wiced_result_t result = WICED_SUCCESS;

    if ( IP_NETWORK_IS_INITED( interface ) == WICED_TRUE )
    {
        return WICED_SUCCESS;
    }

    static_ip = ( ( config == WICED_USE_STATIC_IP || config == WICED_USE_INTERNAL_DHCP_SERVER) && ip_settings != NULL )? WICED_TRUE : WICED_FALSE;

    /* Enable the network interface */
    if ( static_ip == WICED_TRUE )
    {
        ipaddr.addr  = htonl(GET_IPV4_ADDRESS(ip_settings->ip_address));
        gw.addr      = htonl(GET_IPV4_ADDRESS(ip_settings->gateway));
        netmask.addr = htonl(GET_IPV4_ADDRESS(ip_settings->netmask));
    }
    else
    {
        /* make sure that ip address is zero in order to start dhcp client */
        ip4_addr_set_zero( &gw );
        ip4_addr_set_zero( &ipaddr );
        ip4_addr_set_zero( &netmask );
    }

    if ( NULL == netif_add( &IP_HANDLE(interface), &ipaddr, &netmask, &gw, (void*) (ptrdiff_t) WICED_TO_WWD_INTERFACE( interface ), ethernetif_init, ethernet_input ) )
    {
        WPRINT_NETWORK_ERROR(( "Could not add network interface\n" ));
        return WICED_ERROR;
    }

#if LWIP_IPV6
    /* Set the IPv6 linklocal address using our MAC */
    WPRINT_NETWORK_INFO( ("Setting IPv6 link-local address\n") );

    netif_create_ip6_linklocal_address( &IP_HANDLE(interface), 1 );
    IP_HANDLE(interface).ip6_autoconfig_enabled = 1;
#endif

    /* Register a handler for any address changes */
    /* Note that the "status" callback will also be called when the interface
       goes up or down */
    netif_set_status_callback( &IP_HANDLE(interface), ip_address_changed_handler );

    /* Obtain DHCP address through an external dhcp server (RESTORE is a no-op for LWIP as it has no API to support) */
    if ( ( static_ip == WICED_FALSE ) && ( ( config == WICED_USE_EXTERNAL_DHCP_SERVER ) || ( config == WICED_USE_EXTERNAL_DHCP_SERVER_RESTORE ) ) )
    {
#ifndef WICED_AMAZON_FREERTOS_SDK
        wiced_ip_address_t dns_server_address;
#endif
        uint32_t     address_resolution_timeout = WICED_DHCP_IP_ADDRESS_RESOLUTION_TIMEOUT;
        wiced_bool_t timeout_occured            = WICED_FALSE;

        /* Bring up the network interface */
        netif_set_up( &IP_HANDLE(interface) );
        netif_set_default( &IP_HANDLE(interface) );

        WPRINT_NETWORK_INFO(("Obtaining IPv4 address via DHCP\n"));
        dhcp_set_struct( &IP_HANDLE(interface), &wiced_dhcp_handle );

#if LWIP_NETIF_HOSTNAME
        IP_HANDLE(interface).hostname = hostname.value;
#endif

        dhcp_start( &IP_HANDLE(interface) );
        while ( wiced_dhcp_handle.state != DHCP_STATE_BOUND  && timeout_occured == WICED_FALSE )
        {
            sys_msleep( 10 );
            address_resolution_timeout -= 10;

            if ( address_resolution_timeout <= 0 )
            {
                /* timeout has occured */
                timeout_occured = WICED_TRUE;
            }
        }

        if ( timeout_occured == WICED_TRUE )
        {
            dhcp_stop( &IP_HANDLE(interface) );

            if (LWIP_AUTOIP == 1)
            {
                int   tries = 0;

                WPRINT_NETWORK_INFO(("Unable to obtain IP address via DHCP. Perform AUTO_IP\n"));
                address_resolution_timeout = WICED_AUTO_IP_ADDRESS_RESOLUTION_TIMEOUT;
                timeout_occured            = WICED_FALSE;

                autoip_set_struct( &IP_HANDLE(interface), &wiced_autoip_handle );
                if (autoip_start( &IP_HANDLE(interface) ) != ERR_OK )
                {   /* trick: skip the while-loop, do the cleaning up stuff */
                    timeout_occured = WICED_TRUE;
                }

                while ( (timeout_occured == WICED_FALSE)
                    &&  (wiced_autoip_handle.state != AUTOIP_STATE_BOUND) )
                {
                    sys_msleep( 10 );
                    address_resolution_timeout -= 10;

                    if ( address_resolution_timeout <= 0 )
                    {
                        if( tries++ < 5 )
                        {
                            address_resolution_timeout = WICED_AUTO_IP_ADDRESS_RESOLUTION_TIMEOUT;
                        }
                        else
                        {
                            timeout_occured = WICED_TRUE;
                        }
                    }
                }

                if ( timeout_occured == WICED_TRUE )
                {
                    WPRINT_NETWORK_INFO(("Unable to obtain IP address via AutoIP\n"));
                    autoip_stop( &IP_HANDLE(interface) );
                    netif_remove( &IP_HANDLE(interface) );
                    return WICED_ERROR;
                }
                else
                {
                    wiced_using_autoip = WICED_TRUE;
                }
            }
            else
            {
                WPRINT_NETWORK_INFO(("Unable to obtain IP address via DHCP\n"));
                netif_remove( &IP_HANDLE(interface) );
                return WICED_ERROR;
            }
        }
        else
        {
            wiced_using_dhcp = WICED_TRUE;
        }

        /* Check if DNS servers were supplied by the DHCP client */
#if LWIP_DHCP && LWIP_DNS
        if ( ! ip_addr_isany( dns_getserver( 0 ) ) )
        {
            u8_t i;
            const ip_addr_t *lwip_dns_server_addr;

            for (i = 0; i < DNS_MAX_SERVERS; i++)
            {
                lwip_dns_server_addr = dns_getserver(i);
                if ( ! ip_addr_isany( lwip_dns_server_addr ) )
                {
#ifndef WICED_AMAZON_FREERTOS_SDK
                    lwip_ip_to_wiced( &dns_server_address, lwip_dns_server_addr );
                    dns_client_add_server_address(dns_server_address);
#endif
                }
            }
        }
        else
#endif /* if LWIP_DHCP && LWIP_DNS */
        {
            /* DNS servers were not supplied by DHCP client... */
            /* Add gateway DNS server and Google public DNS server */
#ifndef WICED_AMAZON_FREERTOS_SDK
            wiced_ip_get_gateway_address( interface, &dns_server_address );
            dns_client_add_server_address( dns_server_address );


            /* Google DNS server is 8.8.8.8 */
            SET_IPV4_ADDRESS( dns_server_address, 0x08080808 );
            dns_client_add_server_address( dns_server_address );
#endif
        }

        /* Register for IP address change notification */
        /* TODO: Add support for IP address change notification */
    }
    else
    {
        netif_set_up( &IP_HANDLE(interface) );

        igmp_start(&IP_HANDLE(interface));

        /* Check if we should start the DHCP server */
        if ( config == WICED_USE_INTERNAL_DHCP_SERVER )
        {
#ifndef WICED_AMAZON_FREERTOS_SDK
            result = wiced_start_dhcp_server(&internal_dhcp_server, interface);

            if ( WICED_SUCCESS != result ) {
                WPRINT_NETWORK_ERROR( ( "Failed to initialise DHCP server\r\n" ) );
                goto leave_wifi_and_delete_ip;
            }
#endif
            goto leave_wifi_and_delete_ip;
        }
    }

#if LWIP_IPV6
    /*The code commented below causes reboot in the cypress tests. Further investigation
    is required for the root cause of the reboot.*/
    /* Wait until the address has been properly advertised */
    /*while ( ip6_addr_istentative( netif_ip6_addr_state( &IP_HANDLE(interface), 0 )))
    {*/
        /* Give LwIP time to change the state */
        /*WPRINT_NETWORK_DEBUG(( "Waiting for IPv6 address validation\n"));
        host_rtos_delay_milliseconds( ND6_TMR_INTERVAL );
    }*/

    if ( ip6_addr_isvalid( netif_ip6_addr_state( &IP_HANDLE(interface), 0 )))
    {
        WPRINT_NETWORK_INFO( ( "IPv6 Network ready IP: %s\n", ip6addr_ntoa(netif_ip6_addr(&IP_HANDLE(interface), 0))) );
    }
    else
    {
        WPRINT_NETWORK_INFO( ( "IPv6 Network is not ready\n" ));
    }
#endif

    CONFIG_SET_FOR_IF( interface, config );

    SET_IP_NETWORK_INITED( interface, WICED_TRUE );

    WPRINT_NETWORK_INFO( ( "IPv4 Network ready IP: %s\n", ip4addr_ntoa(netif_ip4_addr(&IP_HANDLE(interface)))) );

    /* 3. Replay buffered traffic: send up stack now that IP addresses are configured  */
    if ( WICED_DEEP_SLEEP_IS_ENABLED() )
    {
        wiced_deep_sleep_set_networking_ready();
    }

    return result;

leave_wifi_and_delete_ip:

    wiced_ip_driver_notify( interface, WICED_FALSE );

    if ( interface == WICED_STA_INTERFACE )
    {
        wiced_leave_ap( interface );
    }

    /* 3. Replay buffered traffic: the replay here may result in lost packets, which seems ok since IP addresses weren't obtained properly */
    if ( WICED_DEEP_SLEEP_IS_ENABLED() )
    {
        wiced_deep_sleep_set_networking_ready();
    }

    netif_remove( &IP_HANDLE(interface) );

    return result;
}

wiced_result_t wiced_ip_down( wiced_interface_t interface )
{
    if ( IP_NETWORK_IS_INITED( interface ) == WICED_TRUE )
    {
        /* Cleanup DHCP & DNS */
        if ( CONFIG_GET_FOR_IF( interface ) == WICED_USE_INTERNAL_DHCP_SERVER)
        {
#ifndef WICED_AMAZON_FREERTOS_SDK
            wiced_stop_dhcp_server( &internal_dhcp_server );

            /* Wait till the time wait sockets get closed */
            sys_msleep(TIME_WAIT_TCP_SOCKET_DELAY);
#endif
        }
        else /* External DHCP or AutoIP */
        {
            if ( wiced_using_dhcp == WICED_TRUE )
            {
                dhcp_stop( &IP_HANDLE(interface) );
                wiced_using_dhcp = WICED_FALSE;
            }
            else if ( wiced_using_autoip == WICED_TRUE )
            {
                autoip_stop( &IP_HANDLE(interface) );
                wiced_using_autoip = WICED_FALSE;
            }
#ifndef WICED_AMAZON_FREERTOS_SDK
            dns_client_remove_all_server_addresses();
#endif
        }

        /* Delete the network interface */
        netif_remove( &IP_HANDLE(interface) );

        SET_IP_NETWORK_INITED( interface, WICED_FALSE );
    }
    return WICED_SUCCESS;
}

void wiced_network_notify_link_up( wiced_interface_t interface )
{
    UNUSED_PARAMETER( interface );
    /* This function is called by the WWD thread.
     * Do nothing here. Instead, wait for the worker thread to call us.
     * This avoids race conditions if the link state changes rapidly. */
}

void wiced_network_notify_link_down( wiced_interface_t interface )
{
    UNUSED_PARAMETER( interface );
    /* This function is called by the WWD thread.
     * Do nothing here. Instead, wait for the worker thread to call us.
     * This avoids race conditions if the link state changes rapidly. */
}

static void wiced_call_link_down_callbacks( wiced_interface_t interface )
{
    int i;

    for ( i = 0; i < WICED_MAXIMUM_LINK_CALLBACK_SUBSCRIPTIONS; i++ )
    {
        if ( (LINK_DOWN_CALLBACKS_LIST( interface ))[i] != NULL )
        {
            (LINK_DOWN_CALLBACKS_LIST( interface ))[i]( (LINK_DOWN_CALLBACKS_ARG_LIST( interface ))[i] );
        }
    }
}

static void wiced_call_link_up_callbacks( wiced_interface_t interface )
{
    int i;

    for ( i = 0; i < WICED_MAXIMUM_LINK_CALLBACK_SUBSCRIPTIONS; i++ )
    {
        if ( (LINK_UP_CALLBACKS_LIST( interface ))[i] != NULL )
        {
            (LINK_UP_CALLBACKS_LIST( interface ))[i]( (LINK_UP_CALLBACKS_ARG_LIST( interface ))[i] );
        }
    }
}

wiced_result_t wiced_wireless_link_down_handler( void* arg )
{
    UNUSED_PARAMETER( arg );

    WPRINT_NETWORK_DEBUG( ("Wireless link DOWN!\n\r") );

    /* Notify LwIP that the link is down*/
    netif_set_down( &IP_HANDLE(WICED_STA_INTERFACE) );

    if ( wiced_using_dhcp == WICED_TRUE )
    {
        dhcp_stop( &IP_HANDLE(WICED_STA_INTERFACE) );
    }
    else if ( wiced_using_autoip == WICED_TRUE )
    {
        autoip_stop( &IP_HANDLE(WICED_STA_INTERFACE) );
    }
    /* Wait for a while before the time wait sockets get closed */
    sys_msleep( TIME_WAIT_TCP_SOCKET_DELAY );

    /* Add clearing of ARP cache */
    netifapi_netif_common( &IP_HANDLE(WICED_STA_INTERFACE), (netifapi_void_fn) invalidate_all_arp_entries, NULL );

    /* Inform all subscribers about the link down event */
    wiced_call_link_down_callbacks(WICED_STA_INTERFACE);

    /* Kick the radio chip if it's in power save mode in case the link down event is due to missing beacons. Setting the chip to the same power save mode is sufficient. */
    wifi_powersave_mode = wiced_wifi_get_powersave_mode();
    if ( wifi_powersave_mode == PM1_POWERSAVE_MODE )
    {
        wiced_wifi_enable_powersave();
    }
    else if ( wifi_powersave_mode == PM2_POWERSAVE_MODE )
    {
        wifi_return_to_sleep_delay = wiced_wifi_get_return_to_sleep_delay();
        wiced_wifi_enable_powersave_with_throughput( wifi_return_to_sleep_delay );
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_wireless_link_up_handler( void* arg )
{
    ip4_addr_t ip_addr;
    wiced_result_t result = WICED_SUCCESS;

    UNUSED_PARAMETER( arg );

    WPRINT_NETWORK_DEBUG(("Wireless link UP!\n\r"));

    /* Notify LwIP that the link is up */
    netif_set_up( &IP_HANDLE(WICED_STA_INTERFACE) );

    if ((wiced_using_dhcp == WICED_TRUE ) || (wiced_using_autoip == WICED_TRUE))
    {
        /* Save the current power save state */
        wifi_powersave_mode        = wiced_wifi_get_powersave_mode();
        wifi_return_to_sleep_delay = wiced_wifi_get_return_to_sleep_delay();

        /* Disable power save for the DHCP exchange */
        if ( wifi_powersave_mode != NO_POWERSAVE_MODE )
        {
            wiced_wifi_disable_powersave( );
        }

        if ( wiced_using_dhcp == WICED_TRUE )
        {
            /* For DHCP only, we should reset netif IP address.
             * We don't want to re-use previous netif IP address
             * given from previous DHCP session */
            ip4_addr_set_zero( &ip_addr );
            netif_set_ipaddr( &IP_HANDLE(WICED_STA_INTERFACE), &ip_addr );

            /* Restart DHCP */
            if ( dhcp_start( &IP_HANDLE(WICED_STA_INTERFACE)) != ERR_OK )
            {
                WPRINT_NETWORK_ERROR( ( "Failed to initiate DHCP transaction\n" ) );
                result = WICED_ERROR;
            }
        }
        else if ( wiced_using_autoip == WICED_TRUE )
        {
            if ( autoip_start( &IP_HANDLE(WICED_STA_INTERFACE) ) != ERR_OK )
            {
                WPRINT_NETWORK_ERROR( ("Failed to initiate AutoIP\n" ) );
                result = WICED_ERROR;
            }
        }

        /* Wait a little to allow DHCP/AutoIP a chance to complete, but we can't block here */
        host_rtos_delay_milliseconds( 10 );

        if ( wifi_powersave_mode == PM1_POWERSAVE_MODE )
        {
            wiced_wifi_enable_powersave( );
        }
        else if ( wifi_powersave_mode == PM2_POWERSAVE_MODE )
        {
            wiced_wifi_enable_powersave_with_throughput( wifi_return_to_sleep_delay );
        }
    }

    /* Inform all subscribers about an event */
    wiced_call_link_up_callbacks(WICED_STA_INTERFACE);

    return result;
}

wiced_result_t wiced_wireless_link_renew_handler( void* arg )
{
    UNUSED_PARAMETER( arg );

    /* TODO : need to invalidate only particular arp instead of all arp entries*/
    netifapi_netif_common( &IP_HANDLE(WICED_STA_INTERFACE), (netifapi_void_fn) invalidate_all_arp_entries, NULL );

    /* TODO: Do a DHCP renew if interface is using external DHCP server */

    if ( wiced_dhcp_handle.state == DHCP_STATE_BOUND )
    {
        netifapi_netif_common( &IP_HANDLE(WICED_STA_INTERFACE), (netifapi_void_fn)dhcp_renew, NULL);
    }

    return WICED_SUCCESS;
}

static void ip_address_changed_handler( struct netif *netif )
{
    uint8_t i;

    UNUSED_PARAMETER( netif );

    for ( i = 0; i < MAXIMUM_IP_ADDRESS_CHANGE_CALLBACKS; i++ )
    {
        if ( wiced_ip_address_change_callbacks[i].callback != NULL )
        {
            ( *wiced_ip_address_change_callbacks[i].callback )( wiced_ip_address_change_callbacks[i].arg );
        }
    }
}

wiced_bool_t wiced_ip_is_any_pending_packets( wiced_interface_t interface )
{
    UNUSED_PARAMETER( interface );
    /* LwIP has no APIs to query; assume none */
    return WICED_FALSE;
}

void host_buffer_init_fifo( wiced_buffer_fifo_t* fifo )
{
    memset( fifo->per_interface_fifos, 0, sizeof( fifo->per_interface_fifos ) );
}

wiced_result_t wiced_ip_register_address_change_callback( wiced_ip_address_change_callback_t callback, void* arg )
{
    uint8_t i;
    for ( i = 0; i < MAXIMUM_IP_ADDRESS_CHANGE_CALLBACKS; i++ )
    {
        if ( wiced_ip_address_change_callbacks[i].callback == NULL )
        {
            wiced_ip_address_change_callbacks[i].callback = callback;
            wiced_ip_address_change_callbacks[i].arg = arg;
            return WICED_SUCCESS;
        }
    }

    WPRINT_NETWORK_ERROR( ( "Out of callback storage space\n" ) );

    return WICED_ERROR;
}

wiced_result_t wiced_ip_deregister_address_change_callback( wiced_ip_address_change_callback_t callback )
{
    uint8_t i;
    for ( i = 0; i < MAXIMUM_IP_ADDRESS_CHANGE_CALLBACKS; i++ )
    {
        if ( wiced_ip_address_change_callbacks[i].callback == callback )
        {
            memset( &wiced_ip_address_change_callbacks[i], 0, sizeof( wiced_ip_address_change_callback_t ) );
            return WICED_SUCCESS;
        }
    }

    WPRINT_NETWORK_ERROR( ( "Unable to find callback to deregister\n" ) );

    return WICED_ERROR;
}

wiced_result_t wiced_ip_get_clients_ip_address_list( wiced_interface_t interface, void* ip_address_list )
{
    UNUSED_PARAMETER( interface );
    UNUSED_PARAMETER( ip_address_list );

#ifndef WICED_AMAZON_FREERTOS_SDK
    if ( CONFIG_GET_FOR_IF( interface ) != WICED_USE_INTERNAL_DHCP_SERVER )
    {
        return WICED_ERROR;
    }

    return wiced_get_clients_ip_address_list_dhcp_server( &internal_dhcp_server, ip_address_list );
#endif
    return WICED_ERROR;
}

/**
 * Remove all ARP table entries of the specified netif.
 * @param netif points to a network interface
 */
static void invalidate_all_arp_entries( struct netif *netif )
{
#ifndef WICED_AMAZON_FREERTOS_SDK
    u8_t i;
#endif
    LWIP_UNUSED_ARG(netif);

    /*free all the entries in arp list */
#ifndef WICED_AMAZON_FREERTOS_SDK
    /* default LwIP code does not have ARP APIs; arp  APIs are added in LwIP code by wiced */
    for ( i = 0; i < ARP_TABLE_SIZE; ++i )
    {
        etharp_free_entry( i );
    }
#endif
}

static wiced_result_t wiced_ip_driver_notify( wiced_interface_t interface, wiced_bool_t up )
{
    wiced_result_t result = WICED_SUCCESS;

#ifdef WICED_USE_ETHERNET_INTERFACE
    if ( interface == WICED_ETHERNET_INTERFACE )
    {
        if ( up )
        {
            result = ( platform_ethernet_start( ) == PLATFORM_SUCCESS ) ? WICED_SUCCESS : WICED_ERROR;
        }
        else
        {
            result = ( platform_ethernet_stop( ) == PLATFORM_SUCCESS ) ? WICED_SUCCESS : WICED_ERROR;
        }
    }
#else
    UNUSED_PARAMETER( interface );
    UNUSED_PARAMETER( up );
#endif

    return result;
}

wiced_result_t wiced_init_autoipv6( wiced_interface_t interface )
{
    /* Not implemented yet */
    UNUSED_PARAMETER( interface );
    return WICED_ERROR;
}

