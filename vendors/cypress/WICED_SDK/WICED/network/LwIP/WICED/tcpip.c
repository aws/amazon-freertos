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
 * LwIP TCP/IP library
 */

#include "wiced_network.h"
#include "wiced_ping.h"
#include "lwip/api.h"
#include "lwip/netbuf.h"
#include "lwip/igmp.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/prot/tcp.h"
#include "lwip/ip.h"
#include "lwip/mld6.h"
#include "lwipopts.h"
#include "dns.h"
#include "RTOS/wwd_rtos_interface.h"
#include "wiced_tcpip.h"
#ifndef WICED_DISABLE_TLS
#include "wiced_tls.h"
#include "wiced_tcpip_tls_api.h"
#endif
#ifndef WICED_CONFIG_DISABLE_DTLS
#include "wiced_udpip_dtls_api.h"
#endif
#include "wwd_assert.h"
#include "wiced_crypto.h"
#include "internal/wiced_internal_api.h"

#ifndef WICED_CONFIG_DISABLE_DTLS
#include "wiced_udpip_dtls_api.h"
#include "wiced_dtls.h"
#endif /* ifndef WICED_CONFIG_DISABLE_DTLS */

/******************************************************
 *                      Macros
 ******************************************************/

#define CONVERT_TO_LWIP_TIMEOUT(timeout)   do { if ( timeout == 0 ){ timeout = 1; }} while(0)

#define WICED_LWIP_CONNECTION_TIMEOUT_MAX   0xFFFF
/******************************************************
 *                    Constants
 ******************************************************/

#ifndef WICED_MAXIMUM_NUMBER_OF_SOCKETS_WITH_CALLBACKS
#define WICED_MAXIMUM_NUMBER_OF_SOCKETS_WITH_CALLBACKS    (5)
#endif

#ifndef WICED_MAXIMUM_NUMBER_OF_ACCEPT_SOCKETS
#define WICED_MAXIMUM_NUMBER_OF_ACCEPT_SOCKETS    (5)
#endif
#define WICED_MAXIMUM_NUMBER_OF_SERVERS (WICED_MAXIMUM_NUMBER_OF_SOCKETS_WITH_CALLBACKS)
#define SERVER_LISTEN_QUEUE_SIZE 0
/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static wiced_result_t       internal_udp_send                              ( struct netconn* handler, wiced_packet_t* packet, wiced_interface_t interface );
static wiced_result_t       internal_packet_create                         ( wiced_packet_t** packet, uint16_t content_length, uint8_t** data, uint16_t* available_space );
static wiced_tcp_socket_t*  internal_netconn_to_wiced_async_socket         ( struct netconn *conn );

#ifndef WICED_AMAZON_FREERTOS_SDK
static wiced_tcp_server_t*  internal_netconn_to_wiced_async_server_socket  ( struct netconn *conn );
static wiced_udp_socket_t*  internal_netconn_to_wiced_udp_socket           ( struct netconn *conn );
static void                 internal_netconn_tcp_server_socket_callback    ( struct netconn *conn, enum netconn_evt event, u16_t length );
static wiced_result_t       internal_tcp_server_find_free_socket           ( wiced_tcp_server_t* tcp_server, int* index );
static wiced_result_t       internal_wiced_tcp_server_listen               ( wiced_tcp_server_t* tcp_server);
static int                  internal_wiced_get_socket_index_for_server     ( wiced_tcp_server_t* tcp_server, wiced_tcp_socket_t* socket);
static void                 internal_netconn_udp_socket_callback           ( struct netconn* conn, enum netconn_evt event, u16_t length );
#endif

static void                 internal_netconn_tcp_socket_callback           ( struct netconn* conn, enum netconn_evt event, u16_t length );
/******************************************************
 *               Variable Definitions
 ******************************************************/

static wiced_tcp_socket_t* tcp_sockets_with_callbacks[WICED_MAXIMUM_NUMBER_OF_SOCKETS_WITH_CALLBACKS];
static wiced_udp_socket_t* udp_sockets_with_callbacks[WICED_MAXIMUM_NUMBER_OF_SOCKETS_WITH_CALLBACKS];
#ifndef WICED_AMAZON_FREERTOS_SDK
static wiced_tcp_server_t* server_list[WICED_MAXIMUM_NUMBER_OF_SERVERS] = { NULL };
#endif
wiced_result_t lwip_to_wiced_result[] =
{
    [-(ERR_OK        )] = WICED_SUCCESS,
    [-(ERR_MEM       )] = WICED_OUT_OF_HEAP_SPACE,
    [-(ERR_BUF       )] = WICED_PACKET_BUFFER_CORRUPT,
    [-(ERR_TIMEOUT   )] = WICED_TIMEOUT,
    [-(ERR_RTE       )] = WICED_ROUTING_ERROR,
    [-(ERR_INPROGRESS)] = WICED_PENDING,
    [-(ERR_VAL       )] = WICED_BADVALUE,
    [-(ERR_WOULDBLOCK)] = WICED_WOULD_BLOCK,
    [-(ERR_USE       )] = WICED_ADDRESS_IN_USE,
    [-(ERR_ALREADY   )] = WICED_UNFINISHED,
    [-(ERR_ISCONN    )] = WICED_ALREADY_CONNECTED,
    [-(ERR_CONN      )] = WICED_NOT_CONNECTED,
    [-(ERR_IF        )] = WICED_NETWORK_INTERFACE_ERROR,
    [-(ERR_ABRT      )] = WICED_ABORTED,
    [-(ERR_RST       )] = WICED_CONNECTION_RESET,
    [-(ERR_CLSD      )] = WICED_CONNECTION_CLOSED,
    [-(ERR_ARG       )] = WICED_BADARG,
};


/******************************************************
 *               Function Definitions
 ******************************************************/

/*
Packet-Queue ( Case #1 )
=============
Two TCP segments(payload-length 1200 bytes and 800 bytes respectively) are received out of order.
LwIP Buffer size(pbuf) is greater or equal to MSS/MTU size(1500 bytes).
LwIP will fetch two pbufs for each of the segment received out of order, holds them until they are in sequence,
links them and put the queue in a single message box entry.

|-----------------------------|             |---------------------------|
| TCP1(tot: 2000, len: 1200)  | --(next)--> | TCP2(tot: 800, len: 800)  |--(next)--> NULL
|-----------------------------|             |---------------------------|

Packet-chain ( Case #2 )
=============
A single TCP segment of size 1000 bytes is received.
LwIP buffer size(pbuf) is 512 bytes. However, MTU is still 1500 bytes.
LwIP will fetch two pbufs(512 bytes each) to handle incoming 1000-byte TCP segment and queue them together.
LwIP will put the received TCP segment in a single message box entry.

|------------------------------|             |-----------------------------|
| TCP1-B1(tot: 1000, len: 512) |--(next)-->  | TCP1-B2(tot: 488, len: 488) |--(next)--> NULL
|------------------------------|             |-----------------------------|

Structurally, both case #1 and case #2 are similar. With current configuration, Wiced TCP LwIP framework should NEVER receive TCP segments under Case #2.

Note: For UDP datagram payload larger than WICED_LINK_MTU, IP fragmentation/reassembly logic will kick-in - it will fall into Case #2 automatically.
*/

/* Creates a new netbuf holding the first TCP packet from a cached packet-queue.
 * @note  This function need to be enhanced when LwIP can mix Packet-queues with Packet-chains
 */
static struct netbuf* fetch_new_tcp_packet( struct netbuf* cached_netbuf )
{
    struct netbuf* new = NULL;

    new = netbuf_new();
    if( !new )
        return NULL;

    memcpy(new, cached_netbuf, sizeof(struct netbuf) );

    new->p->tot_len = (u16_t)(cached_netbuf->p->tot_len - cached_netbuf->p->next->tot_len );

    /* move cached_pkt_queue pointer */
    cached_netbuf->p = cached_netbuf->p->next;

    /* break the chain */
    new->p->next = NULL;

    return new;
}

#ifndef WICED_AMAZON_FREERTOS_SDK
wiced_result_t wiced_tcp_server_start( wiced_tcp_server_t* tcp_server, wiced_interface_t interface, uint16_t port, uint16_t max_sockets, wiced_tcp_socket_callback_t connect_callback, wiced_tcp_socket_callback_t receive_callback, wiced_tcp_socket_callback_t disconnect_callback, void* arg )
{
    int i;

    wiced_assert("Bad args", (connect_callback != NULL) && (receive_callback != NULL) && (disconnect_callback != NULL));

    tcp_server->port = port;

    if( max_sockets > WICED_MAXIMUM_NUMBER_OF_ACCEPT_SOCKETS )
    {
        return WICED_BADARG;
    }

    /* register server */
    for ( i = 0; i < WICED_MAXIMUM_NUMBER_OF_SERVERS; ++i )
    {
        if ( server_list[ i ] == NULL )
        {
            server_list[ i ] = tcp_server;
            break;
        }
    }
    /* too many server sockets */
    if ( i == WICED_MAXIMUM_NUMBER_OF_SERVERS )
    {
        return WICED_ERROR;
    }

    /* Check whether the interface is a valid one and it was attached to an IP address */
    if ( ip_addr_isany(&IP_HANDLE(interface).ip_addr) )
    {
        return WICED_INVALID_INTERFACE;
    }

    /* register callbacks */
    tcp_server->listen_socket.callbacks.connect    = connect_callback;
    tcp_server->listen_socket.callbacks.receive    = receive_callback;
    tcp_server->listen_socket.callbacks.disconnect = disconnect_callback;
    tcp_server->listen_socket.callback_arg         = arg;

    WICED_VERIFY( wiced_tcp_create_socket( &tcp_server->listen_socket, interface ) );

    netconn_set_async_callback( tcp_server->listen_socket.conn_handler, internal_netconn_tcp_server_socket_callback );

    /* create accept sockets */
    for ( i = 0; i < WICED_MAXIMUM_NUMBER_OF_ACCEPT_SOCKETS; i++ )
    {
        tcp_server->accept_socket[ i ].socket_state         = WICED_SOCKET_CLOSED;
        tcp_server->accept_socket[ i ].is_bound             = WICED_FALSE;
        tcp_server->accept_socket[ i ].interface            = interface;
        tcp_server->accept_socket[ i ].conn_handler         = tcp_server->listen_socket.conn_handler;
        tcp_server->accept_socket[ i ].accept_handler       = (struct netconn*) 0;
        tcp_server->accept_socket[ i ].callbacks.connect    = connect_callback;
        tcp_server->accept_socket[ i ].callbacks.receive    = receive_callback;
        tcp_server->accept_socket[ i ].callbacks.disconnect = disconnect_callback;
        tcp_server->accept_socket[ i ].callback_arg         = arg;
        tcp_server->accept_socket[ i ].cached_pkt_queue     = NULL;

        /* Netconn cannot handle calls from multiple threads on the same socket */
        wiced_rtos_init_mutex( &tcp_server->accept_socket[ i ].netconn_mutex );

        /* Ensure connect and disconnect do not execute concurrently */
        wiced_rtos_init_mutex( &tcp_server->accept_socket[ i ].conn_mutex );
    }
    tcp_server->data_pending_on_socket = -1;

    /* start server listen*/
    WICED_VERIFY( internal_wiced_tcp_server_listen( tcp_server ) );

    return WICED_SUCCESS;
}

wiced_result_t wiced_tcp_server_accept( wiced_tcp_server_t* tcp_server, wiced_tcp_socket_t* socket )
{
    WICED_VERIFY( wiced_tcp_accept( socket ) );

    return internal_wiced_tcp_server_listen( tcp_server );
}

wiced_result_t wiced_tcp_server_disconnect_socket( wiced_tcp_server_t* tcp_server, wiced_tcp_socket_t* socket )
{
    int index;

    index = internal_wiced_get_socket_index_for_server( tcp_server, socket );

    if ( ( index < 0 ) || ( index >= WICED_MAXIMUM_NUMBER_OF_ACCEPT_SOCKETS ) )
    {
        return WICED_ERROR;
    }

    wiced_rtos_lock_mutex( &socket->conn_mutex );
#ifndef WICED_DISABLE_TLS
    /* Free all buffers held onto by TLS context */
    if ( socket->tls_context != NULL )
    {
        wiced_tls_deinit_context( socket->tls_context );
    }
#endif
    /* first free any cached netbufs , if any */
    if( socket->cached_pkt_queue )
    {
        netbuf_delete( socket->cached_pkt_queue );
        socket->cached_pkt_queue = NULL;
    }

    if ( socket->accept_handler != NULL )
    {
        err_t res = netconn_delete( socket->accept_handler );
        if ( res != ERR_OK )
        {
            wiced_rtos_unlock_mutex( &socket->conn_mutex );
            return LWIP_TO_WICED_ERR( res );
        }
        socket->accept_handler = NULL;
        socket->is_bound       = WICED_FALSE;
        socket->conn_handler   = tcp_server->listen_socket.conn_handler;
    }

    tcp_server->accept_socket[ index ].socket_state = WICED_SOCKET_CLOSED;

    wiced_rtos_unlock_mutex( &socket->conn_mutex );

    return WICED_SUCCESS;
}

wiced_result_t wiced_tcp_server_disconnect_socket_with_timeout( wiced_tcp_server_t* tcp_server, wiced_tcp_socket_t* socket, uint32_t timeout_ms )
{
    (void)timeout_ms;

    /*
     * lwIP doesn't support using a timeout on the disconnect operation.
     */

    return wiced_tcp_server_disconnect_socket( tcp_server, socket );
}

wiced_result_t wiced_tcp_server_stop( wiced_tcp_server_t* tcp_server )
{
    err_t res = ERR_OK;
    int i;

    wiced_assert("Bad args", tcp_server != NULL);

    /* create accept sockets */
    for ( i = 0; i < WICED_MAXIMUM_NUMBER_OF_ACCEPT_SOCKETS; i++ )
    {
#ifndef WICED_DISABLE_TLS
        if ( tcp_server->accept_socket[ i ].tls_context != NULL )
        {
            /* Check if context is of an advanced variety. Note that the server and advanced client context are exactly the same */
            wiced_tls_deinit_context( tcp_server->accept_socket[ i ].tls_context );
            if ( tcp_server->accept_socket[ i ].context_malloced == WICED_TRUE )
            {
                free( tcp_server->accept_socket[ i ].tls_context );
                tcp_server->accept_socket[ i ].tls_context = NULL;
                tcp_server->accept_socket[ i ].context_malloced = WICED_FALSE;
            }
        }
#endif
        /* check for cached netbufs on tcp-server sockets */
        if( tcp_server->accept_socket[ i ].cached_pkt_queue )
        {
            netbuf_delete( tcp_server->accept_socket[ i ].cached_pkt_queue );
            tcp_server->accept_socket[ i ].cached_pkt_queue = NULL;
        }

        netconn_delete( tcp_server->accept_socket[ i ].accept_handler );

        tcp_server->accept_socket[ i ].accept_handler = NULL;
        tcp_server->accept_socket[ i ].conn_handler = NULL;
        tcp_server->accept_socket[ i ].is_bound = WICED_FALSE;
        wiced_rtos_deinit_mutex( &tcp_server->accept_socket[ i ].netconn_mutex );
        wiced_rtos_deinit_mutex( &tcp_server->accept_socket[ i ].conn_mutex );
    }

    netconn_disconnect( tcp_server->listen_socket.conn_handler );
    netconn_delete( tcp_server->listen_socket.conn_handler );

    for ( i = 0; i < WICED_MAXIMUM_NUMBER_OF_SERVERS; i++ )
    {
        server_list[ i ] = NULL;
    }

    return LWIP_TO_WICED_ERR( res );
}

wiced_result_t wiced_tcp_server_get_socket_at_index( wiced_tcp_server_t* tcp_server, wiced_tcp_socket_t** socket, uint8_t index )
{
    wiced_assert("Error fetching Server's socket", (tcp_server != NULL) && (socket != NULL) );

    if( (index >= WICED_MAXIMUM_NUMBER_OF_ACCEPT_SOCKETS) )
    {
        *socket = NULL;
        return WICED_BADARG;
    }

    *socket = &tcp_server->accept_socket[ index ];

    return WICED_SUCCESS;
}
#endif
wiced_result_t wiced_tcp_create_socket( wiced_tcp_socket_t* socket, wiced_interface_t interface )
{
    /* Check whether the interface is a valid one and it was attached to an IP address */
    if ( ip_addr_isany(&IP_HANDLE(interface).ip_addr) )
    {
        return WICED_INVALID_INTERFACE;
    }

    memset( socket, 0, sizeof(wiced_tcp_socket_t) );

    /* LwIP async callback SHOULD be independent of application's callbacks */
    socket->conn_handler = netconn_new_with_callback( NETCONN_TCP, internal_netconn_tcp_socket_callback );
    if( !socket->conn_handler )
    {
        return WICED_SOCKET_CREATE_FAIL;
    }

    /* get the local ip addr of the given network interface and keep it inside */
    /* socket structure since we will need it for future uses when we are going to perform a bind or connect */
    ip_addr_copy( socket->local_ip_addr, IP_HANDLE(interface).ip_addr );
    socket->is_bound = WICED_FALSE;
    socket->accept_handler = ( struct netconn* )0;
    socket->interface = interface;
    socket->cached_pkt_queue = NULL;
    /* Netconn cannot handle calls from multiple threads on the same socket */
    wiced_rtos_init_mutex( &socket->netconn_mutex );

    /* Ensure connect and disconnect do not execute concurrently */
    wiced_rtos_init_mutex( &socket->conn_mutex );

    return WICED_SUCCESS;
}

wiced_result_t wiced_tcp_get_socket_state( wiced_tcp_socket_t* socket, wiced_socket_state_t* socket_state )
{

    if( socket == NULL )
    {
        *socket_state = WICED_SOCKET_ERROR;
        return WICED_BADARG;
    }

    *socket_state = socket->socket_state;

    return WICED_SUCCESS;

}

void wiced_tcp_set_type_of_service( wiced_tcp_socket_t* socket, uint32_t tos )
{
    socket->conn_handler->pcb.ip->tos = (uint8_t)tos;
}

wiced_result_t wiced_tcp_accept( wiced_tcp_socket_t* socket )
{
    err_t status;
    wiced_result_t result = WICED_TCPIP_SUCCESS;

    wiced_assert("Bad args", socket != NULL);

    WICED_LINK_CHECK( socket->interface );

    wiced_rtos_lock_mutex( &socket->conn_mutex );

    if ( socket->accept_handler != NULL )
    {
        /* delete previous connection and use the same connection handler to accept a new one */
        status = netconn_delete( socket->accept_handler );
        socket->accept_handler = NULL;
        if ( status != ERR_OK )
        {
            result = LWIP_TO_WICED_ERR( status );
            goto wiced_tcp_accept_exit;
        }
    }

    if ( ( status =  netconn_accept( socket->conn_handler, &socket->accept_handler ) ) != ERR_OK )
    {
        netconn_delete( socket->accept_handler );
        socket->accept_handler = NULL;
        result = LWIP_TO_WICED_ERR( status );
        goto wiced_tcp_accept_exit;
    }
#ifndef WICED_DISABLE_TLS
    if ( socket->tls_context != NULL )
    {
        result = wiced_tcp_start_tls( socket, WICED_TLS_AS_SERVER, WICED_TLS_DEFAULT_VERIFICATION );
        if ( result != WICED_SUCCESS )
        {
            WPRINT_LIB_INFO( ( "Error starting TLS connection\n" ) );
            goto wiced_tcp_accept_exit;
        }
    }
#endif
wiced_tcp_accept_exit:
    wiced_rtos_unlock_mutex( &socket->conn_mutex );

    return result;
}

wiced_result_t wiced_tcp_bind( wiced_tcp_socket_t* socket, uint16_t port )
{
    err_t lwip_error;
    wiced_assert("Bad args", socket != NULL);

    WICED_LINK_CHECK( socket->interface );

    if ( socket->conn_handler != NULL )
    {
        lwip_error = netconn_bind( socket->conn_handler, &socket->local_ip_addr, port );
        if ( lwip_error == ERR_OK)
        {
            socket->is_bound = WICED_TRUE;
            return WICED_SUCCESS;
        }
        return LWIP_TO_WICED_ERR( lwip_error );
    }

    return WICED_INVALID_SOCKET;
}

wiced_result_t wiced_tcp_register_callbacks( wiced_tcp_socket_t* socket, wiced_tcp_socket_callback_t connect_callback, wiced_tcp_socket_callback_t receive_callback, wiced_tcp_socket_callback_t disconnect_callback, void* arg )
{
    int a;

    /* Add the socket to the list of sockets with callbacks */
    for ( a = 0; a < WICED_MAXIMUM_NUMBER_OF_SOCKETS_WITH_CALLBACKS; ++a )
    {
        if ( tcp_sockets_with_callbacks[a] == socket || tcp_sockets_with_callbacks[a] == NULL )
        {
            tcp_sockets_with_callbacks[a] = socket;
            break;
        }
    }

    if ( a == WICED_MAXIMUM_NUMBER_OF_SOCKETS_WITH_CALLBACKS )
    {
        return WICED_ERROR;
    }

    if ( disconnect_callback != NULL )
    {
        socket->callbacks.disconnect = disconnect_callback;
    }

    if ( receive_callback != NULL )
    {
        socket->callbacks.receive = receive_callback;
    }

    if ( connect_callback != NULL )
    {
        socket->callbacks.connect = connect_callback;
    }

    socket->callback_arg = arg;

    return WICED_SUCCESS;
}

wiced_result_t wiced_tcp_unregister_callbacks( wiced_tcp_socket_t* socket )
{
    int a;
    for ( a = 0; a < WICED_MAXIMUM_NUMBER_OF_SOCKETS_WITH_CALLBACKS; ++a )
    {
        if ( tcp_sockets_with_callbacks[ a ] == socket )
        {
            tcp_sockets_with_callbacks[ a ] = NULL;
        }
    }
    return WICED_TCPIP_SUCCESS;
}

wiced_result_t wiced_tcp_delete_socket( wiced_tcp_socket_t* socket )
{
    err_t res;

    wiced_assert("Bad args", socket != NULL);

#ifndef WICED_DISABLE_TLS
    if ( socket->tls_context != NULL )
    {
        /* Make sure that we use tls_close_notify only when netconn is still valid */
        if( socket->conn_handler )
        {
            wiced_tls_close_notify( socket );
        }

        wiced_tls_deinit_context( socket->tls_context );

        if ( socket->context_malloced == WICED_TRUE )
        {
            free( socket->tls_context );
            socket->tls_context = NULL;
            socket->context_malloced = WICED_FALSE;
        }
    }
#endif /* ifndef WICED_DISABLE_TLS */

    if ( socket->cached_pkt_queue )
    {
        netbuf_delete( socket->cached_pkt_queue );
        socket->cached_pkt_queue = NULL;
    }

    /* if  conn-handler is already NULL( as result of tcp_disconnect ) */
    if( socket->conn_handler )
    {
        res = netconn_delete( socket->conn_handler );
        if ( res != ERR_OK )
        {
            socket->conn_handler = NULL;
            return LWIP_TO_WICED_ERR( res );
        }
    }

    if ( socket->accept_handler != NULL )
    {
        netconn_delete( socket->accept_handler );
    }

    socket->accept_handler      = NULL;
    socket->conn_handler        = NULL;
    socket->is_bound            = WICED_FALSE;
    socket->keep_alive_enabled  = 0;

    wiced_rtos_deinit_mutex( &socket->netconn_mutex );
    wiced_rtos_deinit_mutex( &socket->conn_mutex );

    return WICED_TCPIP_SUCCESS;
}

wiced_result_t wiced_tcp_listen( wiced_tcp_socket_t* socket, uint16_t port )
{
    err_t res;
    wiced_assert("Bad args", socket != NULL);

    WICED_LINK_CHECK( socket->interface );

    if ( socket->conn_handler == NULL)
    {
        return WICED_INVALID_SOCKET;
    }

    /* Check if this socket is already listening */
    if (socket->conn_handler->state == NETCONN_LISTEN)
    {
        socket->socket_state = WICED_SOCKET_LISTEN;
        return WICED_SUCCESS;
    }

    /* Ensure the socket is bound to a port */
    if ( socket->is_bound == WICED_FALSE )
    {
        res = netconn_bind( socket->conn_handler, &socket->local_ip_addr, port );
        if ( res != ERR_OK )
        {
            return LWIP_TO_WICED_ERR( res );
        }
        socket->is_bound = WICED_TRUE;
    }

    res = netconn_listen_with_backlog( socket->conn_handler, WICED_DEFAULT_TCP_LISTEN_QUEUE_SIZE );
    if( res != ERR_OK )
    {
        return LWIP_TO_WICED_ERR( res );
    }

    socket->socket_state = WICED_SOCKET_LISTEN;

    return WICED_TCPIP_SUCCESS;
}

wiced_result_t network_tcp_send_packet( wiced_tcp_socket_t* socket, wiced_packet_t* packet )
{
    err_t    status;
    uint16_t length;
    void*    data;
    uint16_t available;
    wiced_result_t result;

    wiced_assert("Bad args", socket != NULL);

    WICED_LINK_CHECK( socket->interface );

    /* Get data pointer of the payload from the allocated packet */
    packet->p->len = packet->p->tot_len;
    result = wiced_packet_get_data( packet, 0, (uint8_t**) &data, &length, &available );
    if ( result != WICED_SUCCESS )
    {
        return result;
    }

    wiced_rtos_lock_mutex( &socket->netconn_mutex );

    wiced_rtos_lock_mutex( &lwip_send_interface_mutex );

    /* Set to default the requested interface */
    netif_set_default( &IP_HANDLE(socket->interface));

    if ( socket->accept_handler == NULL )
    {
        if ( socket->conn_handler != NULL )
        {
            status = netconn_write( socket->conn_handler, data, length, NETCONN_COPY );
        }
        else
        {
            status = ERR_CLSD;
        }
    }
    else
    {
        status = netconn_write( socket->accept_handler, data, length, NETCONN_COPY );
    }

    wiced_rtos_unlock_mutex( &lwip_send_interface_mutex );

    wiced_rtos_unlock_mutex( &socket->netconn_mutex );

    if ( status != ERR_OK )
    {
        return LWIP_TO_WICED_ERR( status );
    }

    /* Release the use provided packet as the contents have been copied */
    netbuf_delete( packet );

    return WICED_SUCCESS;
}

wiced_result_t network_tcp_receive( wiced_tcp_socket_t* socket, wiced_packet_t** packet, uint32_t timeout )
{
    err_t status;
    struct netbuf* netbuf = NULL;
    struct netbuf* new = NULL;
    struct netbuf* cached = NULL;

    wiced_assert("Bad args", (packet != NULL) && (socket->conn_handler != NULL));

    if ( socket == NULL )
    {
        status = ERR_CLSD;
        return LWIP_TO_WICED_ERR( status );
    }

    /* If the socket is in listening state but the connection has not been
     * established, netconn_recv and subsequently this function cannot be
     * called on the socket. Hence, return an appropriate return code under
     * this condition. This condition can be checked by whether a valid
     * accept_handler exists (i.e. non-NULL) when in listening state.
     * A valid accept_handler indicates the connection is established.
     */
    if ( socket->conn_handler->state == NETCONN_LISTEN &&
         socket->accept_handler == NULL )
    {
        return WICED_TCPIP_INVALID_SOCKET;
    }

    wiced_rtos_lock_mutex( &socket->netconn_mutex );

    /* If we had any cached packet-queue, fetch the TCP packet from it */
    if( socket->cached_pkt_queue )
    {
        cached = socket->cached_pkt_queue;

        if( cached->p == NULL )
        {
            /* we should not reach here; it should have been caught earlier */
            status = ERR_CONN;
            netbuf_delete( cached );
            goto err_exit;
        }

        /* Check if the cached netbuf is still a queue */
        if( !cached->p->next )
        {
            /* Only one packet remaining in the queue. give it to the application - reuse the cached netbuf */
            *packet = cached;
            socket->cached_pkt_queue = NULL;
            status = ERR_OK;
            goto err_exit;
        }

        /* Get a new netbuf holding first TCP segment from the packet queue */
        new = fetch_new_tcp_packet( cached );
        if( new == NULL )
        {
            /* if we don't have memory, drop the whole packet and flag it to the application */
            netbuf_delete( cached );
            socket->cached_pkt_queue = NULL;
            status = ERR_MEM;
            goto err_exit;
        }
        /* give it to the application */
        *packet = new;
        status = ERR_OK;
        goto err_exit;
    }
    else
    {
        CONVERT_TO_LWIP_TIMEOUT(timeout);

        if ( socket->accept_handler == NULL )
        {
            netconn_set_recvtimeout( socket->conn_handler, timeout );
            status = netconn_recv( socket->conn_handler, &netbuf );
        }
        else
        {
            netconn_set_recvtimeout( socket->accept_handler, timeout );
            status = netconn_recv( socket->accept_handler, &netbuf );
        }

        if( netbuf == NULL || status != ERR_OK )
        {
            *packet = NULL;
            goto err_exit;
        }

        /* Check for packet-queue and cache it for next receive */
        if( netbuf->p->next )
        {
            socket->cached_pkt_queue = netbuf;
            new = fetch_new_tcp_packet( netbuf );
            if( new == NULL )
            {
                netbuf_delete( netbuf );
                socket->cached_pkt_queue = NULL;
                status = ERR_MEM;
                goto err_exit;
            }
            /* give it to the application */
            *packet = new;
        }
        else
        {
            *packet = netbuf;
        }
    }
err_exit:
    wiced_rtos_unlock_mutex( &socket->netconn_mutex );
    return LWIP_TO_WICED_ERR( status );
}

wiced_result_t network_udp_receive( wiced_udp_socket_t* socket, wiced_packet_t** packet, uint32_t timeout )
{
    err_t status;

    wiced_assert("Bad args", (packet != NULL) && (socket->conn_handler != NULL));

    if ( socket == NULL )
    {
        status = ERR_CLSD;
    }
    else
    {
        CONVERT_TO_LWIP_TIMEOUT(timeout);

        if ( socket->accept_handler == NULL )
        {
            netconn_set_recvtimeout( socket->conn_handler, timeout );
            status = netconn_recv( socket->conn_handler, packet );
        }
        else
        {
            netconn_set_recvtimeout( socket->accept_handler, timeout );
            status = netconn_recv( socket->accept_handler, packet );
        }
    }
    return LWIP_TO_WICED_ERR( status );
}

wiced_result_t wiced_packet_create_tcp( wiced_tcp_socket_t* socket, uint16_t content_length, wiced_packet_t** packet, uint8_t** data, uint16_t* available_space )
{
    wiced_result_t result;

    UNUSED_PARAMETER(content_length);
#ifdef WICED_DISABLE_TLS
    UNUSED_PARAMETER(socket);
#endif
    result = internal_packet_create( packet, MAX_TCP_PAYLOAD_SIZE, data, available_space );
    if ( result != WICED_SUCCESS )
    {
        return result;
    }
#ifndef WICED_DISABLE_TLS
    if ( socket->tls_context != NULL )
    {
        uint16_t header_space;
        uint16_t footer_pad_space;

        wiced_tls_calculate_overhead( &socket->tls_context->context, (*packet)->p->len, &header_space, &footer_pad_space );

        pbuf_header((*packet)->p, (s16_t)(-header_space));
        *data = (*packet)->p->payload;
        *available_space = (uint16_t)((*packet)->p->len - footer_pad_space);
    }
#endif
    return WICED_SUCCESS;
}

wiced_result_t wiced_tcp_enable_keepalive( wiced_tcp_socket_t* socket, uint16_t interval, uint16_t probes, uint16_t time )
{
    struct netconn*             conn;
    wiced_assert("Bad args", socket != NULL);

    /* first cache it for future usage */
    socket->keep_alive_enabled         = 1;
    socket->keep_alive_params.probes   = probes;
    socket->keep_alive_params.interval = interval;
    socket->keep_alive_params.time     = time;

    /* Just set a few options of the socket */
    if( socket->accept_handler )
    {
        /* if we are a server, then configure accept tcp pcb */
        conn = socket->accept_handler;
    }
    else
    {
        if( socket->conn_handler == NULL )
        {
            return WICED_TCPIP_INVALID_SOCKET;
        }
        conn = socket->conn_handler;
    }
    conn->pcb.tcp->so_options |= SOF_KEEPALIVE;
    conn->pcb.tcp->keep_idle  =  (u32_t)( 1000 * time );
    conn->pcb.tcp->keep_intvl =  (u32_t)( 1000 * interval );
    conn->pcb.tcp->keep_cnt   =  (u32_t)( probes );

    return WICED_TCPIP_SUCCESS;
}

wiced_result_t wiced_packet_create_udp( wiced_udp_socket_t* socket, uint16_t content_length, wiced_packet_t** packet, uint8_t** data, uint16_t* available_space )
{
    wiced_result_t result = WICED_SUCCESS;
    UNUSED_PARAMETER( socket );
    UNUSED_PARAMETER( content_length );

    result = internal_packet_create( packet, MAX_UDP_PAYLOAD_SIZE, data, available_space );
    if ( result != WICED_SUCCESS )
       {
           return result;
       }
#ifndef WICED_CONFIG_DISABLE_DTLS
       if ( socket->dtls_context != NULL )
       {
           uint16_t header_space;
           uint16_t footer_pad_space;

           wiced_dtls_calculate_overhead( &socket->dtls_context->context, ( *packet )->p->len, &header_space, &footer_pad_space );

           pbuf_header( ( *packet )->p, (s16_t) ( -header_space ) );
           *data = ( *packet )->p->payload;
           *available_space = (uint16_t) ( ( *packet )->p->len - footer_pad_space );
       }
#endif
       return result;
}

static wiced_result_t internal_packet_create_no_wait( wiced_packet_t** packet, uint16_t content_length, uint8_t** data, uint16_t* available_space )
{
    wiced_assert("Bad args", (packet != NULL) && (data != NULL) && (available_space != NULL));

    *packet = netbuf_new( );
    if ( *packet != NULL )
    {
        *data = netbuf_alloc( *packet, content_length );
        if ( *data != NULL )
        {
            *available_space = content_length;
            return WICED_TCPIP_SUCCESS;
        }
        netbuf_delete( *packet );
        *packet = NULL;
        *available_space = 0;
    }

    *available_space = 0;
    return WICED_TCPIP_ERROR;
}

wiced_result_t wiced_packet_create_udp_no_wait( wiced_udp_socket_t* socket, uint16_t content_length, wiced_packet_t** packet, uint8_t** data, uint16_t* available_space )
{
    wiced_result_t result = WICED_SUCCESS;
    UNUSED_PARAMETER( socket );
    UNUSED_PARAMETER( content_length );

    result = internal_packet_create_no_wait( packet, MAX_UDP_PAYLOAD_SIZE, data, available_space );
    if ( result != WICED_SUCCESS )
       {
           return result;
       }
#ifndef WICED_CONFIG_DISABLE_DTLS
       if ( socket->dtls_context != NULL )
       {
           uint16_t header_space;
           uint16_t footer_pad_space;

           wiced_dtls_calculate_overhead( &socket->dtls_context->context, ( *packet )->p->len, &header_space, &footer_pad_space );

           pbuf_header( ( *packet )->p, (s16_t) ( -header_space ) );
           *data = ( *packet )->p->payload;
           *available_space = (uint16_t) ( ( *packet )->p->len - footer_pad_space );
       }
#endif
       return result;
}

static wiced_result_t internal_packet_create( wiced_packet_t** packet, uint16_t content_length, uint8_t** data, uint16_t* available_space )
{
    int i = 0;
    wiced_assert("Bad args", (packet != NULL) && (data != NULL) && (available_space != NULL));

    /* Try to allocate a packet, waiting up to WICED_ALLOCATE_PACKET_TIMEOUT milliseconds */
    for ( i = 0; i < WICED_ALLOCATE_PACKET_TIMEOUT; ++i )
    {
        *packet = netbuf_new( );
        if ( *packet != NULL )
        {
            *data = netbuf_alloc( *packet, content_length );
            if ( *data != NULL )
            {
                *available_space = content_length;
                return WICED_TCPIP_SUCCESS;
            }
            netbuf_delete( *packet );
            *packet = NULL;
            *available_space = 0;
        }

        wiced_rtos_delay_milliseconds( 1 );
    }

    *available_space = 0;
    return WICED_TCPIP_TIMEOUT;
}

wiced_result_t wiced_packet_create( uint16_t content_length, wiced_packet_t** packet, uint8_t** data, uint16_t* available_space )
{
    UNUSED_PARAMETER( content_length );

    return internal_packet_create( packet, MAX_IP_PAYLOAD_SIZE, data, available_space );
}

wiced_result_t wiced_packet_delete( wiced_packet_t* packet )
{
    wiced_assert("Bad args", packet != NULL);
    netbuf_delete( packet );
    return WICED_TCPIP_SUCCESS;
}

#ifndef WICED_CONFIG_DISABLE_ENTERPRISE_SECURITY
wiced_result_t wiced_buffer_eap_set_data_end( void* ptr, uint8_t* data_end )
{
    wiced_buffer_t packet = (wiced_buffer_t) ptr;
    packet->len = (uint16_t) ( data_end - ( (uint8_t*) packet->payload ) );
    packet->tot_len = packet->len;

    return WICED_TCPIP_SUCCESS;
}

wiced_result_t wiced_buffer_eap_set_data_start( void* ptr, uint8_t* data_start )
{
    wiced_buffer_t packet = (wiced_buffer_t) ptr;

    pbuf_header( packet, (s16_t) ( (uint8_t*) packet->payload - data_start ) );

    /* Recalculate tot_len for p */
    packet->tot_len = 0;

    for ( struct pbuf* temp_pbuf = packet; temp_pbuf != NULL; temp_pbuf = temp_pbuf->next )
    {
        packet->tot_len = (u16_t) ( packet->tot_len + temp_pbuf->len );
    }

    return WICED_TCPIP_SUCCESS;
}
#endif

wiced_result_t wiced_packet_set_data_end( wiced_packet_t* packet, uint8_t* data_end )
{
    packet->ptr->len     = (uint16_t) ( data_end - ( (uint8_t*) packet->ptr->payload ) );
    packet->p->tot_len = packet->p->len;

    return WICED_TCPIP_SUCCESS;
}

wiced_result_t wiced_packet_set_data_start( wiced_packet_t* packet, uint8_t* data_start )
{
    pbuf_header( packet->ptr, (s16_t) ( (uint8_t*) packet->ptr->payload - data_start ) );

    if ( packet->p != packet->ptr )
    {
        /* Recalculate tot_len for p */
        packet->p->tot_len = 0;

        for ( struct pbuf* temp_pbuf = packet->p; temp_pbuf != NULL; temp_pbuf = temp_pbuf->next )
        {
            packet->p->tot_len = (u16_t) ( packet->p->tot_len + temp_pbuf->len );
        }
    }
    return WICED_TCPIP_SUCCESS;
}

wiced_result_t wiced_packet_get_data( wiced_packet_t* packet, uint16_t offset, uint8_t** data, uint16_t* fragment_available_data_length, uint16_t *total_available_data_length )
{
    s8_t get_next_result;

    wiced_assert( "Bad args", (packet != NULL) && (data != NULL) && (fragment_available_data_length != NULL) && (total_available_data_length != NULL) );

    netbuf_first( packet );
    *total_available_data_length = (uint16_t)( netbuf_len(packet) - offset );

    do
    {
        uint16_t frag_size = packet->ptr->len;
        *data        = packet->ptr->payload;

        if ( frag_size == 0 && *total_available_data_length == 0 )
        {
            *data                           = NULL;
            *fragment_available_data_length = 0;
            *total_available_data_length    = 0;
            return WICED_SUCCESS;
        }
        else if ( offset < frag_size )
        {
            *data += offset;
            *fragment_available_data_length = (uint16_t)(frag_size - offset);
            return WICED_SUCCESS;
        }
        else
        {
            offset = (uint16_t)(offset - frag_size);
            get_next_result = netbuf_next( packet );
        }
    } while ( get_next_result != -1 );

    return WICED_CORRUPT_PACKET_BUFFER;
}

wiced_result_t wiced_packet_pool_init( wiced_packet_pool_ref packet_pool, uint8_t* memory_pointer, uint32_t memory_size, char *pool_name )
{
    UNUSED_PARAMETER( packet_pool );
    UNUSED_PARAMETER( memory_pointer );
    UNUSED_PARAMETER( memory_size );
    UNUSED_PARAMETER( pool_name );
    return WICED_TCPIP_UNSUPPORTED;
}

wiced_result_t wiced_packet_pool_allocate_packet( wiced_packet_pool_ref packet_pool, wiced_packet_type_t packet_type, wiced_packet_t** packet, uint8_t** data, uint16_t* available_space, uint32_t timeout )
{
    UNUSED_PARAMETER( packet_pool );
    UNUSED_PARAMETER( packet_type );
    UNUSED_PARAMETER( packet );
    UNUSED_PARAMETER( data );
    UNUSED_PARAMETER( available_space );
    UNUSED_PARAMETER( timeout );
    return WICED_TCPIP_UNSUPPORTED;
}

wiced_result_t wiced_packet_pool_deinit( wiced_packet_pool_ref packet_pool )
{
    UNUSED_PARAMETER( packet_pool );
    return WICED_TCPIP_UNSUPPORTED;
}

wiced_result_t wiced_ip_get_ipv4_address( wiced_interface_t interface, wiced_ip_address_t* ipv4_address )
{
    SET_IPV4_ADDRESS( *ipv4_address, ntohl(ip_2_ip4(&IP_HANDLE(interface).ip_addr)->addr) );
    return WICED_TCPIP_SUCCESS;
}

wiced_result_t wiced_udp_create_socket( wiced_udp_socket_t* socket, uint16_t port, wiced_interface_t interface )
{
    err_t status;
    wiced_assert("Bad args", socket != NULL);

    /* Check link and return if there is no link */
    WICED_LINK_CHECK( interface );

    memset( socket, 0, sizeof(wiced_udp_socket_t) );

    socket->conn_handler = netconn_new( NETCONN_UDP );
    if( socket->conn_handler == NULL )
    {
        return WICED_SOCKET_CREATE_FAIL;
    }

    /* Bind it to designated port and an interface */
    status = netconn_bind( socket->conn_handler, IP_ANY_TYPE, port );
    if( status != ERR_OK )
    {
        netconn_delete( socket->conn_handler );
        socket->conn_handler = NULL;
        return LWIP_TO_WICED_ERR( status );
    }
    socket->is_bound  = WICED_TRUE;
    socket->interface = interface;

    return WICED_TCPIP_SUCCESS;
}

void wiced_udp_set_type_of_service( wiced_udp_socket_t* socket, uint32_t tos )
{
    socket->conn_handler->pcb.ip->tos = (uint8_t)tos;
}

wiced_result_t wiced_udp_reply( wiced_udp_socket_t* socket, wiced_packet_t* in_packet, wiced_packet_t* out_packet )
{
    err_t status;
    UNUSED_PARAMETER(in_packet);

    wiced_assert("Bad args", (socket != NULL) && (in_packet != NULL) && (out_packet != NULL));

    WICED_LINK_CHECK( socket->interface );

    /* Verify connection is valid */
    if ( socket->conn_handler == NULL )
    {
        return WICED_INVALID_SOCKET;
    }

    status = netconn_send( socket->conn_handler, out_packet );
    if ( status != ERR_OK )
    {
        return LWIP_TO_WICED_ERR( status );
    }

    /* Remove our reference to the sent packet */
    netbuf_delete( out_packet );

    return WICED_SUCCESS;
}

wiced_result_t wiced_udp_receive( wiced_udp_socket_t* socket, wiced_packet_t** packet, uint32_t timeout )
{
    err_t status;
    CONVERT_TO_LWIP_TIMEOUT(timeout);

    wiced_assert("Bad args", socket != NULL);

    if ( socket->conn_handler == NULL )
    {
        return WICED_ERROR;
    }

    WICED_LINK_CHECK( socket->interface );

    netconn_set_recvtimeout( socket->conn_handler, timeout );

    status = netconn_recv( socket->conn_handler, packet );
    if ( status != ERR_OK )
    {
        return LWIP_TO_WICED_ERR( status );
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_udp_delete_socket( wiced_udp_socket_t* socket )
{
    wiced_assert("Bad args", socket != NULL);

    if ( socket->conn_handler == NULL )
    {
        return WICED_INVALID_SOCKET;
    }

#ifndef WICED_CONFIG_DISABLE_DTLS
    if ( socket->dtls_context != NULL )
    {
        wiced_dtls_close_notify( socket );

        wiced_dtls_deinit_context( socket->dtls_context );
    }
#endif /* ifndef WICED_CONFIG_DISABLE_DTLS */

    /* Note: No need to check return value of netconn_delete. It only ever returns ERR_OK */
    netconn_delete( socket->conn_handler );

    socket->conn_handler = NULL;
    return WICED_SUCCESS;
}

wiced_result_t wiced_multicast_join( wiced_interface_t interface, const wiced_ip_address_t* address )
{
    wiced_result_t res = WICED_TCPIP_UNSUPPORTED;
    ip_addr_t temp;

    WICED_LINK_CHECK( interface );

    if ( address->version == WICED_IPV4 )
    {
        ip_addr_set_ip4_u32( &temp, htonl(GET_IPV4_ADDRESS(*address)) );
        res = LWIP_TO_WICED_ERR( igmp_joingroup( ip_2_ip4(&IP_HANDLE(interface).ip_addr), ip_2_ip4(&temp) ));
    }
#if LWIP_IPV6
    else if ( address->version == WICED_IPV6 )
    {
        IP_ADDR6_HOST( &temp, address->ip.v6[0], address->ip.v6[1], address->ip.v6[2], address->ip.v6[3] );
        res = LWIP_TO_WICED_ERR( mld6_joingroup( ip_2_ip6(&IP_HANDLE(interface).ip_addr), ip_2_ip6(&temp) ));
    }
#endif
    return res;
}

wiced_result_t wiced_multicast_leave( wiced_interface_t interface, const wiced_ip_address_t* address )
{
    wiced_result_t res = WICED_TCPIP_UNSUPPORTED;
    ip_addr_t temp;

    WICED_LINK_CHECK( interface );

    if ( address->version == WICED_IPV4 )
    {
        /* call lwip stack to leave a multicast group */
        ip_addr_set_ip4_u32( &temp, htonl(GET_IPV4_ADDRESS(*address)) );
        res = LWIP_TO_WICED_ERR( igmp_leavegroup( ip_2_ip4(&IP_HANDLE( interface ).ip_addr), ip_2_ip4(&temp) ));
    }
#if LWIP_IPV6
    else if ( address->version == WICED_IPV6 )
    {
        IP_ADDR6_HOST( &temp, address->ip.v6[0], address->ip.v6[1], address->ip.v6[2], address->ip.v6[3] );
        res = LWIP_TO_WICED_ERR( mld6_leavegroup( ip_2_ip6(&IP_HANDLE(interface).ip_addr), ip_2_ip6(&temp) ));
    }
#endif
    return res;
}

wiced_result_t wiced_ip_get_gateway_address( wiced_interface_t interface, wiced_ip_address_t* ipv4_address )
{
    SET_IPV4_ADDRESS( *ipv4_address, ntohl(ip_2_ip4(&IP_HANDLE(interface).gw)->addr));
    return WICED_TCPIP_SUCCESS;
}

wiced_result_t wiced_ip_get_netmask( wiced_interface_t interface, wiced_ip_address_t* ipv4_address )
{
    SET_IPV4_ADDRESS( *ipv4_address, ntohl(ip_2_ip4(&IP_HANDLE(interface).netmask)->addr));
    return WICED_TCPIP_SUCCESS;
}
#ifndef WICED_DISABLE_TLS
wiced_result_t wiced_tcp_server_enable_tls( wiced_tcp_server_t* tcp_server, wiced_tls_identity_t* identity )
{
    tcp_server->tls_identity = identity;
    return WICED_TCPIP_SUCCESS;
}
#endif
wiced_result_t wiced_ping( wiced_interface_t interface, const wiced_ip_address_t* address, uint32_t timeout_ms, uint32_t* elapsed_ms )
{
    WICED_LINK_CHECK( interface );

    if ( address->version != WICED_IPV4 )
    {
        wiced_assert("ping for ipv6 not implemented yet", 0!=0);
        return WICED_TCPIP_UNSUPPORTED;
    }

    return wiced_ping_send( interface, address, timeout_ms, elapsed_ms );
}

wiced_result_t wiced_tcp_server_peer( wiced_tcp_socket_t* socket, wiced_ip_address_t* address, uint16_t* port )
{
    ip_addr_t peer_address;
    err_t       res;

    wiced_assert("Bad args", (socket != NULL) && (address != NULL) && (port != NULL));

    if( ( socket == NULL ) || ( address == NULL ) || (port == NULL) )
    {
        return WICED_BADARG;
    }

    if ( ( res = netconn_peer( socket->accept_handler, &peer_address, port ) ) == ERR_OK )
    {
        lwip_ip_to_wiced( address, &peer_address );
        return WICED_SUCCESS;
    }

    return LWIP_TO_WICED_ERR( res );
}

wiced_result_t wiced_tcp_client_peer( wiced_tcp_socket_t* socket, wiced_ip_address_t* address, uint16_t* port )
{
    ip_addr_t peer_address;
    err_t       res;

    wiced_assert("Bad args", (socket != NULL) && (address != NULL) && (port != NULL));

    if( ( socket == NULL ) || ( address == NULL ) || (port == NULL) )
    {
        return WICED_BADARG;
    }

    if ( ( res = netconn_peer( socket->conn_handler, &peer_address, port ) ) == ERR_OK )
    {
        lwip_ip_to_wiced( address, &peer_address );
        return WICED_SUCCESS;
    }

    return LWIP_TO_WICED_ERR( res );
}

wiced_result_t wiced_tcp_connect( wiced_tcp_socket_t* socket, const wiced_ip_address_t* address, uint16_t port, uint32_t timeout )
{
    ip_addr_t temp;
    err_t    lwip_error;
    UNUSED_PARAMETER( timeout );

    wiced_assert("Bad args", (socket != NULL) && (address != NULL));

    if ( socket->conn_handler == NULL )
    {
        socket->conn_handler = netconn_new_with_callback( NETCONN_TCP, internal_netconn_tcp_socket_callback );
        if( !socket->conn_handler )
        {
            return WICED_INVALID_SOCKET;
        }

        if( socket->keep_alive_enabled )
        {
            struct netconn* conn = socket->conn_handler;

            conn->pcb.tcp->so_options |= SOF_KEEPALIVE;
            conn->pcb.tcp->keep_idle  =  (u32_t)( 1000 * (socket->keep_alive_params.time) );
            conn->pcb.tcp->keep_intvl =  (u32_t)( 1000 * (socket->keep_alive_params.interval) );
            conn->pcb.tcp->keep_cnt   =  (u32_t)( socket->keep_alive_params.probes );
        }
    }

    WICED_LINK_CHECK( socket->interface );

    wiced_ip_to_lwip( &temp, address );

    /* To avoid silent truncation of 32-bit timeout variable to 16-bit value
    saturating the timeout value to 65535(0xFFFF) */
    if ( timeout > WICED_LWIP_CONNECTION_TIMEOUT_MAX )
    {
        timeout = WICED_LWIP_CONNECTION_TIMEOUT_MAX;
        WPRINT_NETWORK_INFO(("Timeout 32-bit value has been truncated to a 16-bit value\n"));
    }
#ifdef WICED_AMAZON_FREERTOS_SDK
    lwip_error = netconn_connect( socket->conn_handler, &temp, port );
#else
    lwip_error = netconn_connect( socket->conn_handler, &temp, port, (uint16_t) timeout );
#endif
    if ( lwip_error != ERR_OK )
    {
        return LWIP_TO_WICED_ERR( lwip_error );
    }

    socket->conn_handler->pcb.tcp->flags &= (uint8_t) ( ~TF_NODELAY );
#ifndef WICED_DISABLE_TLS
    if ( socket->tls_context != NULL )
    {
        wiced_result_t result = wiced_tcp_start_tls( socket, WICED_TLS_AS_CLIENT, WICED_TLS_DEFAULT_VERIFICATION );
        if ( result != WICED_SUCCESS)
        {
            netconn_disconnect( socket->conn_handler );
            return result;
        }
    }
#endif
    socket->socket_state = WICED_SOCKET_CONNECTED;

    return WICED_SUCCESS;
}

wiced_result_t wiced_tcp_disconnect( wiced_tcp_socket_t* socket )
{
    wiced_assert("Bad args", socket != NULL);
#ifndef WICED_DISABLE_TLS
    if ( socket->tls_context != NULL )
    {
        wiced_tls_close_notify( socket );
    }
#endif
    if ( socket->cached_pkt_queue )
    {
        netbuf_delete( socket->cached_pkt_queue );
        socket->cached_pkt_queue = NULL;
    }

    if ( socket->accept_handler != NULL )
    {
        err_t res = netconn_delete( socket->accept_handler );
        if ( res != ERR_OK )
        {
            socket->socket_state = WICED_SOCKET_ERROR;
            return LWIP_TO_WICED_ERR( res );
        }

        socket->accept_handler = NULL;
    }

    else if( socket->conn_handler != NULL )
    {
        err_t res = netconn_delete( socket->conn_handler );
        if ( res != ERR_OK )
        {
            socket->socket_state = WICED_SOCKET_ERROR;
            return LWIP_TO_WICED_ERR( res );;
        }

        socket->conn_handler = NULL;
    }

    socket->socket_state = WICED_SOCKET_CLOSED;

    return WICED_SUCCESS;
}

wiced_result_t wiced_tcp_disconnect_with_timeout( wiced_tcp_socket_t* socket, uint32_t timeout_ms )
{
    (void)timeout_ms;

    /*
     * lwIP doesn't support using a timeout on the disconnect operation.
     */

    return wiced_tcp_disconnect(socket);
}

wiced_result_t wiced_udp_packet_get_info( wiced_packet_t* packet, wiced_ip_address_t* address, uint16_t* port )
{
    UNUSED_PARAMETER( address );

    lwip_ip_to_wiced( address, &packet->addr );
    *port            = netbuf_fromport( packet );

    return WICED_SUCCESS;
}

wiced_result_t wiced_udp_packet_get_info_ext( wiced_packet_t* packet, wiced_ip_address_t* address, uint16_t* port, wiced_ip_address_t* dst_address )
{
    UNUSED_PARAMETER( address );
    UNUSED_PARAMETER( dst_address );

    lwip_ip_to_wiced( address, &packet->addr );
    *port            = netbuf_fromport( packet );

    //lwip_ip_to_wiced( dst_address, &packet->toaddr );
    return WICED_SUCCESS;
}

wiced_result_t wiced_ip_get_ipv6_address( wiced_interface_t interface, wiced_ip_address_t* ipv6_address, wiced_ipv6_address_type_t address_type )
{
#if LWIP_IPV6
    int i;

    for ( i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++ )
    {
        if ( ip6_addr_isvalid( netif_ip6_addr_state( &IP_HANDLE(interface), i ) ) )
        {
            const ip_addr_t *ip6_addr = netif_ip_addr6( &IP_HANDLE(interface), i );

            switch (address_type )
            {
                case IPv6_GLOBAL_ADDRESS:
                    if ( ip6_addr_isglobal( ip_2_ip6(ip6_addr) ) )
                    {
                        lwip_ip_to_wiced( ipv6_address, ip6_addr );
                        return WICED_TCPIP_SUCCESS;
                    }
                    break;

                case IPv6_LINK_LOCAL_ADDRESS:
                    if ( ip_addr_islinklocal( ip6_addr ) )
                    {
                        lwip_ip_to_wiced( ipv6_address, ip6_addr );
                        return WICED_TCPIP_SUCCESS;
                    }
                    break;

                default:
                    wiced_assert("Wrong address type", 0!=0 );
                    return WICED_TCPIP_ERROR;
            }
        }
    }
    return WICED_TCPIP_ERROR;
#else
    UNUSED_PARAMETER( interface );
    UNUSED_PARAMETER( ipv6_address );
    UNUSED_PARAMETER( address_type );

    return WICED_UNSUPPORTED;
#endif
}

/* This function was firstly created for NetX/NetX_Duo (ThreadX)
 * LwIP doesn't seem to have UDP packet receiving limitation
 * Thus, nothing to do ...
 */
wiced_result_t wiced_udp_update_socket_backlog( wiced_udp_socket_t* socket, uint32_t backlog )
{
    UNUSED_PARAMETER( socket );
    UNUSED_PARAMETER( backlog );

    return WICED_SUCCESS;
}

wiced_result_t wiced_udp_send( wiced_udp_socket_t* socket, const wiced_ip_address_t* address, uint16_t port, wiced_packet_t* packet )
{
    ip_addr_t temp;
    err_t status;
    wiced_result_t result;

    wiced_assert("Bad args", (socket != NULL) && (address != NULL) && (packet != NULL));

    WICED_LINK_CHECK( socket->interface );

    /* Associate UDP socket with specific remote IP address and a port */
    wiced_ip_to_lwip( &temp, address );
    status = netconn_connect( socket->conn_handler, &temp, port );
    if ( status != ERR_OK )
    {
        return LWIP_TO_WICED_ERR( status );
    }

    /* Total length and a length must be equal for a packet to be valid */
    packet->p->len = packet->p->tot_len;

    /* Send the packet via UDP socket */
    result = internal_udp_send( socket->conn_handler, packet, (wiced_interface_t)socket->interface );
    if ( result != WICED_SUCCESS )
    {
        netconn_disconnect( socket->conn_handler );
        return result;
    }

    netbuf_delete( packet );

    /* Return back to disconnected state
     * Note: We are ignoring the return for this as we MUST return WICED_SUCCESS otherwise the caller may attempt to
     * free the packet a second time.
     */
    netconn_disconnect( socket->conn_handler );

    return WICED_SUCCESS;
}

wiced_result_t wiced_udp_register_callbacks( wiced_udp_socket_t* socket, wiced_udp_socket_callback_t receive_callback, void* arg )
{
    int a;

    /* Add the socket to the list of sockets with callbacks */
    for ( a = 0; a < WICED_MAXIMUM_NUMBER_OF_SOCKETS_WITH_CALLBACKS; ++a )
    {
        if ( udp_sockets_with_callbacks[a] == socket || udp_sockets_with_callbacks[a] == NULL )
        {
            udp_sockets_with_callbacks[a] = socket;
            break;
        }
    }

    if ( a == WICED_MAXIMUM_NUMBER_OF_SOCKETS_WITH_CALLBACKS )
    {
        return WICED_ERROR;
    }

    socket->receive_callback = receive_callback;
    socket->callback_arg = arg;
#ifndef WICED_AMAZON_FREERTOS_SDK
    netconn_set_async_callback( socket->conn_handler, internal_netconn_udp_socket_callback );
#endif
    return WICED_SUCCESS;
}

wiced_result_t wiced_udp_unregister_callbacks( wiced_udp_socket_t* socket )
{
    int a;
    for ( a = 0; a < WICED_MAXIMUM_NUMBER_OF_SOCKETS_WITH_CALLBACKS; ++a )
    {
        if ( udp_sockets_with_callbacks[ a ] == socket )
        {
            udp_sockets_with_callbacks[ a ] = NULL;
        }
    }
    return WICED_TCPIP_SUCCESS;
}

/******************************************************
 *            Static Function Definitions
 ******************************************************/

static wiced_result_t internal_udp_send( struct netconn* handler, wiced_packet_t* packet, wiced_interface_t interface )
{
    err_t status;
    wiced_rtos_lock_mutex( &lwip_send_interface_mutex );

    /* set to default the requested interface */
    netif_set_default( &IP_HANDLE(interface));

    /* send a packet */
    packet->p->len = packet->p->tot_len;
    status = netconn_send( handler, packet );
    wiced_rtos_unlock_mutex( &lwip_send_interface_mutex );
    netbuf_free(packet);
    return LWIP_TO_WICED_ERR( status );
}

static wiced_tcp_socket_t* internal_netconn_to_wiced_async_socket( struct netconn *conn )
{
    int i = 0;
    for ( i = 0; i < WICED_MAXIMUM_NUMBER_OF_SOCKETS_WITH_CALLBACKS; ++i )
    {
        if ( tcp_sockets_with_callbacks[i] != NULL )
        {
            if ( tcp_sockets_with_callbacks[i]->conn_handler == conn )
            {
                return tcp_sockets_with_callbacks[i];
            }
            else if ( tcp_sockets_with_callbacks[i]->conn_handler )
            {
                if ( (tcp_sockets_with_callbacks[i]->conn_handler->pcb.tcp->local_port == conn->pcb.tcp->local_port ) &&
                     (tcp_sockets_with_callbacks[i]->conn_handler->pcb.tcp->local_port) )
                {
                    return tcp_sockets_with_callbacks[i];
                }
            }
        }
    }
    return NULL;
}
#if 0
static wiced_tcp_server_t* internal_netconn_to_wiced_async_server_socket( struct netconn *conn )
{
    int i = 0;
    for ( i = 0; i < WICED_MAXIMUM_NUMBER_OF_SERVERS; ++i )
    {
        if ( server_list[i] != NULL )
        {
            if ( server_list[i]->listen_socket.conn_handler == conn )
            {
                return server_list[i];
            }
            else if ( server_list[i]->listen_socket.conn_handler )
            {
                if ( (server_list[i]->listen_socket.conn_handler->pcb.tcp->local_port == conn->pcb.tcp->local_port ) &&
                     (server_list[i]->listen_socket.conn_handler->pcb.tcp->local_port) )
                {
                    return server_list[i];
                }
            }
        }
    }
    return NULL;
}

static wiced_udp_socket_t* internal_netconn_to_wiced_udp_socket( struct netconn *conn )
{
    int i = 0;
    for ( i = 0; i < WICED_MAXIMUM_NUMBER_OF_SOCKETS_WITH_CALLBACKS; ++i )
    {
        if ( udp_sockets_with_callbacks[i] != NULL )
        {
            if ( udp_sockets_with_callbacks[i]->conn_handler == conn )
            {
                return udp_sockets_with_callbacks[i];
            }
        }
    }
    return NULL;
}
#endif
static void internal_netconn_tcp_socket_callback( struct netconn *conn, enum netconn_evt event, u16_t length )
{
    wiced_tcp_socket_t* socket;

    /* Verify that the socket is among the asynchronous callback sockets list */
    socket = internal_netconn_to_wiced_async_socket( conn );
    if (socket == NULL)
    {
        return;
    }

    switch ( event )
    {
        case NETCONN_EVT_RCVPLUS:
            if ( length > 0 )
            {
                socket->socket_state = WICED_SOCKET_CONNECTED;
                internal_defer_tcp_callback_to_wiced_network_thread( socket, socket->callbacks.receive );
            }
            else
            {
                /* Logic of events:
                 * Disconnect - if we receive length zero for a connection in other than closed state
                 * Connect    - if we receive length zero for a connection in listening state
                 */
                if( socket->socket_state == WICED_SOCKET_LISTEN )
                {
                    internal_defer_tcp_callback_to_wiced_network_thread( socket, socket->callbacks.connect );
                    socket->socket_state = WICED_SOCKET_CONNECTING;
                }
                else
                {
                    if( socket->socket_state != WICED_SOCKET_CLOSED )
                    {
                        internal_defer_tcp_callback_to_wiced_network_thread( socket, socket->callbacks.disconnect );
                        socket->socket_state = WICED_SOCKET_CLOSED;
                    }
                }
            }
            break;

        case NETCONN_EVT_ERROR:
            /*  Connection was reseted externally, inform about disconnect */
            internal_defer_tcp_callback_to_wiced_network_thread( socket, socket->callbacks.disconnect );
            socket->socket_state = WICED_SOCKET_CLOSED;
            break;

        case NETCONN_EVT_RCVMINUS:
        case NETCONN_EVT_SENDPLUS:
        case NETCONN_EVT_SENDMINUS:
        default:
            break;
    }
}
#ifndef WICED_AMAZON_FREERTOS_SDK
static void internal_netconn_udp_socket_callback( struct netconn *conn, enum netconn_evt event, u16_t length )
{
    wiced_udp_socket_t* socket;

    /* Verify that the socket is among the asynchronous callback sockets list */
    socket = internal_netconn_to_wiced_udp_socket( conn );
    if (socket == NULL)
    {
        return;
    }

    switch ( event )
    {
        case NETCONN_EVT_RCVPLUS:
            if ( length > 0 )
            {
                internal_defer_udp_callback_to_wiced_network_thread( socket );
            }
            break;

        case NETCONN_EVT_ERROR:
        case NETCONN_EVT_RCVMINUS:
        case NETCONN_EVT_SENDPLUS:
        case NETCONN_EVT_SENDMINUS:
        default:
            break;
    }
}

static void internal_netconn_tcp_server_socket_callback( struct netconn *conn, enum netconn_evt event, u16_t length )
{
    wiced_tcp_server_t* server;
    int                 socket_index;
    wiced_bool_t        new_peer = WICED_TRUE;
    server = internal_netconn_to_wiced_async_server_socket( conn );
    switch ( event )
    {
        case NETCONN_EVT_RCVPLUS:
            if (server != NULL)
            {
                for (socket_index = 0; socket_index < WICED_MAXIMUM_NUMBER_OF_ACCEPT_SOCKETS ; socket_index++)
                {
                    struct netconn *accept_handler = server->accept_socket[socket_index].accept_handler;

                    if ( accept_handler != NULL )
                    {
                        if ( ( ip_addr_cmp( &accept_handler->pcb.tcp->remote_ip, &conn->pcb.tcp->remote_ip ) ) &&
                             ( accept_handler->pcb.tcp->remote_port    ==  conn->pcb.tcp->remote_port    ) &&
                             ( accept_handler->pcb.tcp->remote_port != 0 ) &&
                             ( ! ip_addr_isany(&accept_handler->pcb.tcp->remote_ip) ) )
                        {
                            new_peer = WICED_FALSE;
                            break;
                        }
                    }
                 }

                if ( new_peer == WICED_TRUE )
                {
                    if  ( length > 0 )
                    {
                        if ( ( server->data_pending_on_socket >= 0  ) && ( server->data_pending_on_socket < WICED_MAXIMUM_NUMBER_OF_ACCEPT_SOCKETS ) )
                        {
                            server->accept_socket[server->data_pending_on_socket].socket_state = WICED_SOCKET_CONNECTED;
                            internal_defer_tcp_callback_to_wiced_network_thread( (void*)&server->accept_socket[server->data_pending_on_socket], server->accept_socket[server->data_pending_on_socket].callbacks.receive );
                            server->data_pending_on_socket = -1;
                        }
                    }
                    else
                    {
                        if ( internal_tcp_server_find_free_socket( server, &socket_index ) == WICED_SUCCESS )
                        {
                            server->accept_socket[socket_index].socket_state = WICED_SOCKET_CONNECTING;
                            server->data_pending_on_socket                   = socket_index;
                            internal_defer_tcp_callback_to_wiced_network_thread( &server->accept_socket[socket_index], server->accept_socket[socket_index].callbacks.connect );
                        }
                    }
                }
                else /*already connected peer */
                {
                    if  ( length > 0 )
                    {
                        server->accept_socket[socket_index].socket_state = WICED_SOCKET_CONNECTED;
                        internal_defer_tcp_callback_to_wiced_network_thread( &server->accept_socket[socket_index], server->accept_socket[socket_index].callbacks.receive );
                    }
                    else if ( ( server->accept_socket[socket_index].socket_state != WICED_SOCKET_CLOSED ) )
                    {
                        server->data_pending_on_socket            = -1;
                        server->accept_socket[socket_index].socket_state = WICED_SOCKET_CLOSING;
                        internal_defer_tcp_callback_to_wiced_network_thread( &server->accept_socket[socket_index], server->accept_socket[socket_index].callbacks.disconnect );
                    }
                }
            }
            break;
        case NETCONN_EVT_ERROR:
            break;
        case NETCONN_EVT_RCVMINUS:
        case NETCONN_EVT_SENDPLUS:
        case NETCONN_EVT_SENDMINUS:
        default:
            break;
    }
}

static wiced_result_t internal_tcp_server_find_free_socket (wiced_tcp_server_t* tcp_server, int* index )
{
    int i;
    for (i = 0; i < WICED_MAXIMUM_NUMBER_OF_ACCEPT_SOCKETS; i++)
    {
        if( tcp_server->accept_socket[i].socket_state == WICED_SOCKET_CLOSED )
        {
            *index = i;
            return WICED_SUCCESS;
        }
    }
    return WICED_ERROR;
}

static int internal_wiced_get_socket_index_for_server(wiced_tcp_server_t* tcp_server, wiced_tcp_socket_t* socket)
{
    int i;
    for (i = 0; i < WICED_MAXIMUM_NUMBER_OF_ACCEPT_SOCKETS; i++)
    {
        if ( &tcp_server->accept_socket[i] == socket )
        {
            return i;
        }
    }
    return -1;
}

static wiced_result_t internal_wiced_tcp_server_listen(wiced_tcp_server_t* tcp_server )
{
    err_t res = ERR_CONN;

    wiced_assert("Bad args", tcp_server != NULL && tcp_server->listen_socket.conn_handler != NULL);

    WICED_LINK_CHECK( tcp_server->listen_socket.interface );


    /* Check if this socket is already listening */
    if (tcp_server->listen_socket.conn_handler->state == NETCONN_LISTEN)
    {
        return WICED_SUCCESS;
    }

    /* Ensure the socket is bound to a port */
    if (tcp_server->listen_socket.is_bound == WICED_FALSE )
    {
        res = netconn_bind( tcp_server->listen_socket.conn_handler, &tcp_server->listen_socket.local_ip_addr, tcp_server->port );
        if ( res != ERR_OK )
        {
            return LWIP_TO_WICED_ERR( res );
        }
        tcp_server->listen_socket.is_bound = WICED_TRUE;
    }

    if ( tcp_server->listen_socket.conn_handler->pcb.tcp->state != LISTEN )
    {
        res = netconn_listen_with_backlog( tcp_server->listen_socket.conn_handler, SERVER_LISTEN_QUEUE_SIZE );
    }
    return LWIP_TO_WICED_ERR( res );
}
#endif
