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

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#ifndef WICED_AMAZON_FREERTOS_SDK
#include "tls_types.h"
#include "dtls_types.h"
#endif


#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/netbuf.h"
#include "lwip/prot/ip4.h"
#include "lwip/prot/tcp.h"
#include "arch/sys_arch.h"
#include "wiced_result.h"
#include "rtos.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#define LWIP_TO_WICED_ERR( lwip_err )  ((-lwip_err < -ERR_ARG) ? lwip_to_wiced_result[ -lwip_err ] : WICED_UNKNOWN_NETWORK_STACK_ERROR )

#define IP_HANDLE(interface)   (*wiced_ip_handle[(interface)&3])

/******************************************************
 *                    Constants
 ******************************************************/

#define MAX_TCP_PAYLOAD_SIZE    ( WICED_PAYLOAD_MTU - TCP_HLEN - IP_HLEN - WICED_PHYSICAL_HEADER )
#define MAX_UDP_PAYLOAD_SIZE    ( WICED_PAYLOAD_MTU - UDP_HLEN - IP_HLEN - WICED_PHYSICAL_HEADER )
#define MAX_IP_PAYLOAD_SIZE     ( WICED_PAYLOAD_MTU - IP_HLEN - WICED_PHYSICAL_HEADER )


#define IP_STACK_SIZE               (4*1024)
#define DHCP_STACK_SIZE             (1280)

#define WICED_ANY_PORT              (0)

#define wiced_packet_pools          (NULL)

#define WICED_NETWORK_MTU_SIZE      (WICED_LINK_MTU)

#define WICED_MAXIMUM_NUMBER_OF_SOCKETS_WITH_CALLBACKS    (5)
#define WICED_MAXIMUM_NUMBER_OF_ACCEPT_SOCKETS            (MEMP_NUM_NETCONN)

#define WICED_LINK_CHECK( interface )      do { if ( netif_is_up( &IP_HANDLE(interface) ) != 1){ return WICED_NOTUP; }} while(0)
#define WICED_LINK_CHECK_TCP_SOCKET( socket_in )   WICED_LINK_CHECK((socket_in)->interface)
#define WICED_LINK_CHECK_UDP_SOCKET( socket_in )   WICED_LINK_CHECK((socket_in)->interface)
#define WICED_MAXIMUM_SEGMENT_SIZE( socket_in )    MAX_TCP_PAYLOAD_SIZE

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    WICED_TCP_STANDARD_SOCKET,
    WICED_TCP_SECURE_SOCKET,
} wiced_tcp_socket_type_t;

typedef enum
{
    WICED_SOCKET_CLOSED,
    WICED_SOCKET_CLOSING,
    WICED_SOCKET_CONNECTING,
    WICED_SOCKET_CONNECTED,
    WICED_SOCKET_DATA_PENDING,
    WICED_SOCKET_LISTEN,
    WICED_SOCKET_ERROR
} wiced_socket_state_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct netbuf wiced_packet_t;

/* Forward declaration so we can use the type in the function declarations below */
typedef struct wiced_ip_address_s wiced_ip_address_t;

/******************************************************
 *                    Structures
 ******************************************************/
/* These should be in wiced_tcpip.h but are needed by wiced_tcp_socket_t, which would cause a circular include chain */
typedef struct wiced_tcp_socket_struct wiced_tcp_socket_t;
typedef struct wiced_udp_socket_struct wiced_udp_socket_t;

typedef wiced_result_t (*wiced_tcp_socket_callback_t)( wiced_tcp_socket_t* socket, void* arg );
typedef wiced_result_t (*wiced_udp_socket_callback_t)( wiced_udp_socket_t* socket, void* arg );

struct wiced_tcp_socket_struct
{
    wiced_tcp_socket_type_t     type;
    int                         socket;
    struct netconn*             conn_handler;
    struct netconn*             accept_handler;
    ip_addr_t                   local_ip_addr;
    wiced_bool_t                is_bound;
    int                         interface;
#ifndef WICED_DISABLE_TLS
    wiced_tls_context_t*        tls_context;
#endif
    wiced_bool_t                context_malloced;
    struct
    {
        wiced_tcp_socket_callback_t disconnect;
        wiced_tcp_socket_callback_t receive;
        wiced_tcp_socket_callback_t connect;
    } callbacks;
    void*                       callback_arg;
    wiced_socket_state_t        socket_state;       /* internal LwIP socket states do not seem to work correctly */
    wiced_mutex_t               netconn_mutex;      /* Serializes netconn API calls for a socket */
    wiced_mutex_t               conn_mutex;         /* Serializes connect and disconnect calls */
    wiced_packet_t*             cached_pkt_queue;   /* Required for caching the packet pointer when two or more TCP packets are queued together */

    uint8_t                     keep_alive_enabled;
    struct
    {
        uint16_t                time;
        uint16_t                probes;
        uint16_t                interval;
    } keep_alive_params;
};

typedef struct
{
    wiced_tcp_socket_t   listen_socket;
    wiced_tcp_socket_t   accept_socket       [WICED_MAXIMUM_NUMBER_OF_ACCEPT_SOCKETS];
    int                  data_pending_on_socket;
    uint16_t             port;
#ifndef WICED_DISABLE_TLS
    wiced_tls_identity_t* tls_identity;
#endif
} wiced_tcp_server_t;

struct wiced_udp_socket_struct
{
    int                         socket;
    struct netconn*             conn_handler;
    struct netconn*             accept_handler;
    ip_addr_t                   local_ip_addr;
    wiced_bool_t                is_bound;
    int                         interface;
#ifndef WICED_DISABLE_TLS
    wiced_dtls_context_t*       dtls_context;
#endif
    wiced_bool_t                context_malloced;
    wiced_udp_socket_callback_t receive_callback;
    void*                       callback_arg;
};

typedef struct
{
    wiced_tcp_socket_type_t  type;
    int                      socket;
#ifndef WICED_DISABLE_TLS
    wiced_tls_context_t      context;
    wiced_tls_session_t      session;
    wiced_tls_certificate_t* certificate;
    wiced_tls_key_t*         key;
#endif
} wiced_tls_socket_t;



/******************************************************
 *                 Global Variables
 ******************************************************/

/* Note: These objects are for internal use only! */
extern sys_thread_t    wiced_thread_handle;
extern struct netif*   wiced_ip_handle[3];
extern struct dhcp     wiced_dhcp_handle;

extern wiced_result_t  lwip_to_wiced_result[];
extern wiced_mutex_t   lwip_send_interface_mutex;

/******************************************************
 *               Function Declarations
 ******************************************************/
wiced_result_t wiced_init_autoipv6( wiced_interface_t interface );

extern void lwip_ip_to_wiced( wiced_ip_address_t *dest, const ip_addr_t *src );
extern void wiced_ip_to_lwip( ip_addr_t *dest, const wiced_ip_address_t *src );

#ifdef __cplusplus
} /*extern "C" */
#endif
