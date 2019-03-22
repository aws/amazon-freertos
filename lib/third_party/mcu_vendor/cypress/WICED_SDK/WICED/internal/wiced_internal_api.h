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
#pragma once

#include "wiced_management.h"
#include "wiced_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#define WICED_TO_WWD_INTERFACE( interface )        ( wwd_interface_t )( (interface)&3 )
#define SET_IP_NETWORK_INITED(interface, status)   (ip_networking_inited[(interface)&3] = status)

#ifdef WICED_USE_ETHERNET_INTERFACE
#define LINK_UP_CALLBACKS_LIST( interface )        ( interface == WICED_STA_INTERFACE ? link_up_callbacks_wireless : link_up_callbacks_ethernet )
#define LINK_UP_CALLBACKS_ARG_LIST( interface )    ( interface == WICED_STA_INTERFACE ? link_up_callbacks_arg_wireless : link_up_callbacks_arg_ethernet )
#define LINK_DOWN_CALLBACKS_LIST( interface )      ( interface == WICED_STA_INTERFACE ? link_down_callbacks_wireless : link_down_callbacks_ethernet )
#define LINK_DOWN_CALLBACKS_ARG_LIST( interface )  ( interface == WICED_STA_INTERFACE ? link_down_callbacks_arg_wireless : link_down_callbacks_arg_ethernet )
#else
#define LINK_UP_CALLBACKS_LIST( interface )        ( interface == WICED_STA_INTERFACE ? link_up_callbacks_wireless : link_up_callbacks_wireless )
#define LINK_UP_CALLBACKS_ARG_LIST( interface )    ( interface == WICED_STA_INTERFACE ? link_up_callbacks_arg_wireless : link_up_callbacks_arg_wireless )
#define LINK_DOWN_CALLBACKS_LIST( interface )      ( interface == WICED_STA_INTERFACE ? link_down_callbacks_wireless : link_down_callbacks_wireless )
#define LINK_DOWN_CALLBACKS_ARG_LIST( interface )  ( interface == WICED_STA_INTERFACE ? link_down_callbacks_arg_wireless : link_down_callbacks_arg_wireless )
#endif


/******************************************************
 *                    Constants
 ******************************************************/

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
    uint16_t          received_byte_count;
    uint8_t           received_cooee_data[512];
    wiced_semaphore_t sniff_complete;
    wiced_mac_t         initiator_mac;
    wiced_mac_t         ap_bssid;
    wiced_bool_t      wiced_cooee_complete;
    uint16_t          size_of_zero_data_packet;
    uint32_t          received_segment_bitmap[32];
    wiced_bool_t      scan_complete;
    uint8_t*          user_processed_data;
} wiced_cooee_workspace_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

extern wiced_bool_t                  ip_networking_inited[];
extern wiced_mutex_t                 link_subscribe_mutex;

extern wiced_network_link_callback_t link_up_callbacks_wireless[WICED_MAXIMUM_LINK_CALLBACK_SUBSCRIPTIONS];
extern void*                         link_up_callbacks_arg_wireless[WICED_MAXIMUM_LINK_CALLBACK_SUBSCRIPTIONS];
extern wiced_network_link_callback_t link_down_callbacks_wireless[WICED_MAXIMUM_LINK_CALLBACK_SUBSCRIPTIONS];
extern void*                         link_down_callbacks_arg_wireless[WICED_MAXIMUM_LINK_CALLBACK_SUBSCRIPTIONS];
#ifdef WICED_USE_ETHERNET_INTERFACE
extern wiced_network_link_callback_t link_up_callbacks_ethernet[WICED_MAXIMUM_LINK_CALLBACK_SUBSCRIPTIONS];
extern void*                         link_up_callbacks_arg_ethernet[WICED_MAXIMUM_LINK_CALLBACK_SUBSCRIPTIONS];
extern wiced_network_link_callback_t link_down_callbacks_ethernet[WICED_MAXIMUM_LINK_CALLBACK_SUBSCRIPTIONS];
extern void*                         link_down_callbacks_arg_ethernet[WICED_MAXIMUM_LINK_CALLBACK_SUBSCRIPTIONS];
#endif

/******************************************************
 *               Function Declarations
 ******************************************************/

/* WICED <-> Platform API */
extern wiced_result_t wiced_platform_init( void );

/* WICED <-> Network API */
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
extern wiced_result_t wiced_join_ap       ( wiced_interface_t interface );
extern wiced_result_t wiced_join_ap_specific( wiced_interface_t interface, wiced_ap_info_t* details, uint8_t security_key_length, const char security_key[ 64 ] );
extern wiced_result_t wiced_start_sta     ( wiced_interface_t interface );
#else
extern wiced_result_t wiced_join_ap       ( void );
extern wiced_result_t wiced_join_ap_specific( wiced_ap_info_t* details, uint8_t security_key_length, const char security_key[ 64 ] );
#endif
extern wiced_result_t wiced_leave_ap      ( wiced_interface_t interface );
extern wiced_result_t wiced_start_ap      ( wiced_ssid_t* ssid, wiced_security_t security, const char* key, uint8_t channel);
extern wiced_result_t wiced_stop_ap       ( void );

extern wiced_result_t wiced_ip_up         ( wiced_interface_t interface, wiced_network_config_t config, const wiced_ip_setting_t* ip_settings );
extern wiced_result_t wiced_ip_down       ( wiced_interface_t interface );
extern wiced_bool_t IP_NETWORK_IS_INITED( wiced_interface_t interface );
extern wiced_result_t wiced_ip_get_clients_ip_address_list( wiced_interface_t interface, void* ip_address_list );

/* WICED <-> Network Link Management API */
/* NOTE:
 * The notify link functions below are called from within the context of the WICED thread
 * The link handler functions below are called from within the context of the Network Worker thread */
extern void           wiced_network_notify_link_up     ( wiced_interface_t interface );
extern void           wiced_network_notify_link_down   ( wiced_interface_t interface );
extern wiced_result_t wiced_wireless_link_down_handler ( void* arg );
extern wiced_result_t wiced_wireless_link_up_handler   ( void* arg );
extern wiced_result_t wiced_wireless_link_renew_handler( void* arg );
#ifdef WICED_USE_ETHERNET_INTERFACE
extern wiced_result_t wiced_ethernet_link_down_handler ( void );
extern wiced_result_t wiced_ethernet_link_up_handler   ( void );
#endif

/* Wiced Cooee API*/
extern wiced_result_t wiced_wifi_cooee( wiced_cooee_workspace_t* workspace );

/* TLS helper function to do TCP without involving TLS context */
wiced_result_t network_tcp_send_packet( wiced_tcp_socket_t* socket, wiced_packet_t*  packet );
wiced_result_t network_tcp_receive( wiced_tcp_socket_t* socket, wiced_packet_t** packet, uint32_t timeout );
wiced_result_t network_udp_receive( wiced_udp_socket_t* socket, wiced_packet_t** packet, uint32_t timeout );

void system_monitor_thread_main( wiced_thread_arg_t arg );

wiced_result_t internal_defer_tcp_callback_to_wiced_network_thread( wiced_tcp_socket_t* socket, wiced_tcp_socket_callback_t callback  );
wiced_result_t internal_defer_udp_callback_to_wiced_network_thread( wiced_udp_socket_t* socket );
wiced_result_t wiced_tcp_server_get_socket_at_index( wiced_tcp_server_t* tcp_server, wiced_tcp_socket_t** socket, uint8_t index );

#ifdef __cplusplus
} /*extern "C" */
#endif
