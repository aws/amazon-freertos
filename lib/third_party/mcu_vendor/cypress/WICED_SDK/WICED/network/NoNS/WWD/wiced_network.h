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
#if 0
#include "nx_api.h"
#include "tx_port.h" /* Needed by nx_dhcp.h that follows */
#include "netx_applications/dhcp/nx_dhcp.h"
#endif
//#include "tls_types.h"
//#include "dtls_types.h"
#include "wiced_result.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#define IP_HANDLE(interface)   (wiced_ip_handle[(interface) & 3])

/******************************************************
 *                    Constants
 ******************************************************/

#define WICED_MAXIMUM_NUMBER_OF_SOCKETS_WITH_CALLBACKS    (1)
#define WICED_MAXIMUM_NUMBER_OF_SERVER_SOCKETS            (1)

#define SIZE_OF_ARP_ENTRY           sizeof(1)

#define IP_STACK_SIZE               (2 * 1024)
#define ARP_CACHE_SIZE              (6 * SIZE_OF_ARP_ENTRY)
#define DHCP_STACK_SIZE             (1024)

#define WICED_ANY_PORT              (0)

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    WICED_SOCKET_ERROR
} wiced_socket_state_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/
typedef struct
{
    int dummy;
}NOOS_DUMMY;
typedef NOOS_DUMMY    wiced_packet_t;

//typedef NOOS_DUMMY wiced_tls_context_type_t;
//typedef NOOS_DUMMY wiced_tls_context_t;
//typedef NOOS_DUMMY wiced_tls_session_t;
//typedef NOOS_DUMMY wiced_tls_certificate_t;
//typedef NOOS_DUMMY  wiced_tls_endpoint_type_t;
typedef NOOS_DUMMY NOOS_TCP_SOCKET;

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    //wiced_dtls_context_t*       dtls_context;
    int dummy;
} wiced_udp_socket_t;

typedef struct
{
    NOOS_TCP_SOCKET             socket;
    //wiced_tls_context_t*        tls_context;
    wiced_bool_t                context_malloced;
} wiced_tcp_socket_t;

typedef struct
{
    wiced_tcp_socket_t  socket[WICED_MAXIMUM_NUMBER_OF_SERVER_SOCKETS];
    int                 interface;
    uint16_t            port;
} wiced_tcp_server_t;

typedef wiced_result_t (*wiced_tcp_socket_callback_t)( wiced_tcp_socket_t* socket, void* arg );
typedef wiced_result_t (*wiced_udp_socket_callback_t)( wiced_udp_socket_t* socket, void* arg );
/******************************************************
 *                 Global Variables
 ******************************************************/
typedef struct
{
    int dummy;
}NOOS_IP;
typedef struct
{
    int dummy;
}NOOS_PACKET_POOL;
/*
 * Note: These objects are for internal use only!
 */
extern NOOS_IP          wiced_ip_handle       [3];
extern NOOS_PACKET_POOL wiced_packet_pools    [2]; /* 0=TX, 1=RX */

/******************************************************
 *               Function Declarations
 ******************************************************/


#ifdef __cplusplus
} /*extern "C" */
#endif
