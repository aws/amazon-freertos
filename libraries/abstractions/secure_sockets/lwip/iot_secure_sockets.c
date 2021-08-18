/*
 * FreeRTOS Secure Sockets V1.3.1
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file iot_secure_sockets.c
 * @brief Secure Socket interface implementation.
 */

/* Define _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE to prevent secure sockets functions
 * from redefining in iot_secure_sockets_wrapper_metrics.h */
#define _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE

/* Secure Socket interface includes. */
#include "iot_secure_sockets.h"


#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/err.h"

#include "iot_tls.h"

#include "iot_atomic.h"

#include "FreeRTOSConfig.h"

#include "task.h"

#include <stdbool.h>

#undef _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE

/*
 * The loop delay used while waiting for DNS resolution
 * to complete.
 */

#define lwip_dns_resolver_LOOP_DELAY_MS       ( 250 )
#define lwip_dns_resolver_LOOP_DELAY_TICKS    ( ( TickType_t ) lwip_dns_resolver_LOOP_DELAY_MS / portTICK_PERIOD_MS )

/*
 * The maximum time to wait for DNS resolution
 * to complete.
 */
#define lwip_dns_resolver_MAX_WAIT_SECONDS    ( 20 )

/*
 * The maximum number of loop iterations to wait for DNS
 * resolution to complete.
 */
#define lwip_dns_resolver_MAX_WAIT_CYCLES                 \
    ( ( ( lwip_dns_resolver_MAX_WAIT_SECONDS ) * 1000 ) / \
      ( lwip_dns_resolver_LOOP_DELAY_MS ) )

/*-----------------------------------------------------------*/

#define SS_STATUS_CONNECTED    ( 1 )
#define SS_STATUS_SECURED      ( 2 )

/*
 * secure socket context.
 */
typedef enum E_AWS_SOCkET_RX_STATE
{
    SST_RX_IDLE,
    SST_RX_READY,
    SST_RX_CLOSING,
    SST_RX_CLOSED,
} T_AWS_SOCKET_RX_STATE;

typedef struct _ss_ctx_t
{
    int ip_socket;

    int state;
    unsigned int status;
    int send_flag;
    int recv_flag;

    TaskHandle_t rx_handle;
    void ( * rx_callback )( Socket_t pxSocket );

    bool enforce_tls;
    void * tls_ctx;
    char * destination;

    char * server_cert;
    int server_cert_len;

    char ** ppcAlpnProtocols;
    uint32_t ulAlpnProtocolsCount;
    uint32_t ulRefcount;
} ss_ctx_t;

/*-----------------------------------------------------------*/

/*#define SUPPORTED_DESCRIPTORS  (2) */

/*-----------------------------------------------------------*/

/*static int8_t sockets_allocated = SUPPORTED_DESCRIPTORS; */
static int8_t sockets_allocated = socketsconfigDEFAULT_MAX_NUM_SECURE_SOCKETS;


/*-----------------------------------------------------------*/

/*
 * convert from system ticks to seconds.
 */
#define TICK_TO_S( _t_ )     ( ( _t_ ) / configTICK_RATE_HZ )

/*
 * convert from system ticks to micro seconds.
 */
#define TICK_TO_US( _t_ )    ( ( _t_ ) * 1000 / configTICK_RATE_HZ * 1000 )

/*-----------------------------------------------------------*/

/*
 * @brief Sockets close
 */
static void prvSocketsClose( ss_ctx_t * ctx )
{
    uint32_t ulProtocol;

    sockets_allocated++;

    /* Clean-up application protocol array. */
    if( NULL != ctx->ppcAlpnProtocols )
    {
        for( ulProtocol = 0;
             ulProtocol < ctx->ulAlpnProtocolsCount;
             ulProtocol++ )
        {
            if( NULL != ctx->ppcAlpnProtocols[ ulProtocol ] )
            {
                vPortFree( ctx->ppcAlpnProtocols[ ulProtocol ] );
            }
        }

        vPortFree( ctx->ppcAlpnProtocols );
    }

    if( true == ctx->enforce_tls )
    {
        TLS_Cleanup( ctx->tls_ctx );
    }

    if( ctx->server_cert )
    {
        vPortFree( ctx->server_cert );
    }

    if( ctx->destination )
    {
        vPortFree( ctx->destination );
    }

    vPortFree( ctx );
}

/*
 * @brief Decrement ctx refcount and call release function if the count is 1 (
 *        last user of the ctx)
 */
static void prvDecrementRefCount( ss_ctx_t * ctx )
{
    if( Atomic_Decrement_u32( &ctx->ulRefcount ) == 1 )
    {
        prvSocketsClose( ctx );
    }
}

/*
 * @brief Increment ctx refcount
 */
static void prvIncrementRefCount( ss_ctx_t * ctx )
{
    Atomic_Increment_u32( &ctx->ulRefcount );
}

/*
 * @brief Network send callback.
 */
static BaseType_t prvNetworkSend( void * pvContext,
                                  const unsigned char * pucData,
                                  size_t xDataLength )
{
    ss_ctx_t * ctx = ( ss_ctx_t * ) pvContext;

    int ret = lwip_send( ctx->ip_socket,
                         pucData,
                         xDataLength,
                         ctx->send_flag );

    return ( BaseType_t ) ret;
}

/*-----------------------------------------------------------*/

/*
 * @brief Network receive callback.
 */
static BaseType_t prvNetworkRecv( void * pvContext,
                                  unsigned char * pucReceiveBuffer,
                                  size_t xReceiveLength )
{
    ss_ctx_t * ctx;

    ctx = ( ss_ctx_t * ) pvContext;

    configASSERT( ctx->ip_socket >= 0 );

    int ret = lwip_recv( ctx->ip_socket,
                         pucReceiveBuffer,
                         xReceiveLength,
                         ctx->recv_flag );

    if( -1 == ret )
    {
        /*
         * 1. EWOULDBLOCK if the socket is NON-blocking, but there is no data
         *    when recv is called.
         * 2. EAGAIN if the socket would block and have waited long enough but
         *    packet is not received.
         */
        if( ( errno == EWOULDBLOCK ) || ( errno == EAGAIN ) )
        {
            return SOCKETS_ERROR_NONE; /* timeout or would block */
        }

        /*
         * socket is not connected.
         */
        if( errno == EBADF )
        {
            return SOCKETS_ECLOSED;
        }
    }

    if( ( 0 == ret ) && ( errno == ENOTCONN ) )
    {
        ret = SOCKETS_ECLOSED;
    }

    return ( BaseType_t ) ret;
}

/*-----------------------------------------------------------*/

static void vTaskRxSelect( void * param )
{
    ss_ctx_t * ctx = ( ss_ctx_t * ) param;
    int s = ctx->ip_socket;

    fd_set read_fds;
    fd_set write_fds;
    fd_set err_fds;

    FD_ZERO( &read_fds );
    FD_ZERO( &write_fds );
    FD_ZERO( &err_fds );

    FD_SET( s, &read_fds );
    FD_SET( s, &err_fds );

    ctx->state = SST_RX_READY;

    while( 1 )
    {
        if( ctx->state == SST_RX_CLOSING )
        {
            ctx->state = SST_RX_CLOSED;
            break;
        }

        if( lwip_select( s + 1, &read_fds, &write_fds, &err_fds, NULL ) == -1 )
        {
            break;
        }

        if( FD_ISSET( s, &read_fds ) )
        {
            ctx->rx_callback( ( Socket_t ) ctx );
        }
    }

    prvDecrementRefCount( ctx );
    vTaskDelete( NULL );
}


/*-----------------------------------------------------------*/

static void prvRxSelectSet( ss_ctx_t * ctx,
                            const void * pvOptionValue )
{
    BaseType_t xReturned;
    TaskHandle_t xHandle = NULL;
    configSTACK_DEPTH_TYPE xStackDepth = socketsconfigRECEIVE_CALLBACK_TASK_STACK_DEPTH;

    ctx->rx_callback = ( void ( * )( Socket_t ) )pvOptionValue;

    prvIncrementRefCount( ctx );
    xReturned = xTaskCreate( vTaskRxSelect, /* pvTaskCode */
                             "rxs",         /* pcName */
                             xStackDepth,   /* usStackDepth */
                             ctx,           /* pvParameters */
                             1,             /* uxPriority */
                             &xHandle );    /* pxCreatedTask */

    configASSERT( xReturned == pdPASS );
    configASSERT( xHandle != NULL );

    ctx->rx_handle = xHandle;
}

/*-----------------------------------------------------------*/

static void prvRxSelectClear( ss_ctx_t * ctx )
{
    /* TODO */
}

/*-----------------------------------------------------------*/

Socket_t SOCKETS_Socket( int32_t lDomain,
                         int32_t lType,
                         int32_t lProtocol )
{
    ss_ctx_t * ctx;

    configASSERT( lDomain == SOCKETS_AF_INET );
    configASSERT( lType == SOCKETS_SOCK_STREAM );
    configASSERT( lProtocol == SOCKETS_IPPROTO_TCP );

    if( ( lDomain != SOCKETS_AF_INET ) ||
        ( lType != SOCKETS_SOCK_STREAM ) ||
        ( lProtocol != SOCKETS_IPPROTO_TCP ) ||
        ( sockets_allocated <= 0 )
        )
    {
        return SOCKETS_INVALID_SOCKET;
    }

    ctx = ( ss_ctx_t * ) pvPortMalloc( sizeof( *ctx ) );

    if( ctx )
    {
        memset( ctx, 0, sizeof( *ctx ) );

        ctx->ip_socket = lwip_socket( lDomain, lType, lProtocol );

        if( ctx->ip_socket >= 0 )
        {
            ctx->ulRefcount = 1;
            sockets_allocated--;
            return ( Socket_t ) ctx;
        }

        vPortFree( ctx );
    }

    return ( Socket_t ) SOCKETS_INVALID_SOCKET;
}

/*-----------------------------------------------------------*/

int32_t SOCKETS_Bind( Socket_t xSocket,
                      SocketsSockaddr_t * pxAddress,
                      Socklen_t xAddressLength )
{
    ss_ctx_t * ctx;
    int32_t ret;
    struct sockaddr_in sa_addr = { 0 };

    if( SOCKETS_INVALID_SOCKET == xSocket )
    {
        configPRINTF( ( "TCP socket Invalid\n" ) );
        return SOCKETS_EINVAL;
    }

    if( pxAddress == NULL )
    {
        configPRINTF( ( "TCP socket Invalid Address\n" ) );
        return SOCKETS_EINVAL;
    }

    ctx = ( ss_ctx_t * ) xSocket;

    if( NULL == ctx )
    {
        configPRINTF( ( "Invalid secure socket passed: Socket=%p\n", ctx ) );
        return SOCKETS_EINVAL;
    }

    if( 0 > ctx->ip_socket )
    {
        configPRINTF( ( "TCP socket Invalid index\n" ) );
        return SOCKETS_EINVAL;
    }

    /*
     * Setting SO_REUSEADDR socket option in order to be able to bind to the same ip:port again
     * without netconn_bind failing.
     */
    #if SO_REUSE
        ret = lwip_setsockopt( ctx->ip_socket, SOL_SOCKET, SO_REUSEADDR, &( uint32_t ) { 1 }, sizeof( uint32_t ) );

        if( 0 > ret )
        {
            return SOCKETS_SOCKET_ERROR;
        }
    #endif /* SO_REUSE */

    sa_addr.sin_family = pxAddress->ucSocketDomain ? pxAddress->ucSocketDomain : AF_INET;
    sa_addr.sin_addr.s_addr = pxAddress->ulAddress;
    sa_addr.sin_port = pxAddress->usPort;

    ret = lwip_bind( ctx->ip_socket, ( struct sockaddr * ) &sa_addr, sizeof( sa_addr ) );

    if( 0 > ret )
    {
        configPRINTF( ( "lwip_bind fail :%d\n", ret ) );
        return SOCKETS_SOCKET_ERROR;
    }

    return SOCKETS_ERROR_NONE;
}

/*-----------------------------------------------------------*/

int32_t SOCKETS_Connect( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t xAddressLength )
{
    ss_ctx_t * ctx;

    if( SOCKETS_INVALID_SOCKET == xSocket )
    {
        return SOCKETS_EINVAL;
    }

    /* removed because qualification program wants invalid length to go through */
    #if 0
        if( ( NULL == pxAddress ) || ( 0 == xAddressLength ) )
        {
            return SOCKETS_EINVAL;
        }
    #endif

    if( pxAddress == NULL )
    {
        return SOCKETS_EINVAL;
    }

    /* support only SOCKETS_AF_INET for now */
    pxAddress->ucSocketDomain = SOCKETS_AF_INET;

    ctx = ( ss_ctx_t * ) xSocket;
    configASSERT( ctx->ip_socket >= 0 );

    struct sockaddr_in sa_addr = { 0 };
    int ret;
    sa_addr.sin_family = pxAddress->ucSocketDomain;
    sa_addr.sin_addr.s_addr = pxAddress->ulAddress;
    sa_addr.sin_port = pxAddress->usPort;

    ret = lwip_connect( ctx->ip_socket,
                        ( struct sockaddr * ) &sa_addr,
                        sizeof( sa_addr ) );

    if( 0 == ret )
    {
        TLSParams_t tls_params = { 0 };
        BaseType_t status;

        ctx->status |= SS_STATUS_CONNECTED;

        if( !ctx->enforce_tls )
        {
            return SOCKETS_ERROR_NONE;
        }

        tls_params.ulSize = sizeof( tls_params );
        tls_params.pcDestination = ctx->destination;
        tls_params.pcServerCertificate = ctx->server_cert;
        tls_params.ulServerCertificateLength = ctx->server_cert_len;
        tls_params.pvCallerContext = ctx;
        tls_params.pxNetworkRecv = prvNetworkRecv;
        tls_params.pxNetworkSend = prvNetworkSend;
        tls_params.ppcAlpnProtocols = ( const char ** ) ctx->ppcAlpnProtocols;
        tls_params.ulAlpnProtocolsCount = ctx->ulAlpnProtocolsCount;

        status = TLS_Init( &ctx->tls_ctx, &tls_params );

        if( pdFREERTOS_ERRNO_NONE != status )
        {
            configPRINTF( ( "TLS_Init fail\n" ) );
            return SOCKETS_SOCKET_ERROR;
        }

        status = TLS_Connect( ctx->tls_ctx );

        if( pdFREERTOS_ERRNO_NONE == status )
        {
            ctx->status |= SS_STATUS_SECURED;
            return SOCKETS_ERROR_NONE;
        }
        else
        {
            configPRINTF( ( "TLS_Connect fail (0x%x, %s)\n",
                            ( unsigned int ) -status,
                            ctx->destination ? ctx->destination : "NULL" ) );
        }
    }
    else
    {
        configPRINTF( ( "LwIP connect fail %d %d\n", ret, errno ) );
    }

    return SOCKETS_SOCKET_ERROR;
}

/*-----------------------------------------------------------*/

int32_t SOCKETS_Recv( Socket_t xSocket,
                      void * pvBuffer,
                      size_t xBufferLength,
                      uint32_t ulFlags )
{
    ss_ctx_t * ctx = ( ss_ctx_t * ) xSocket;

    if( SOCKETS_INVALID_SOCKET == xSocket )
    {
        return SOCKETS_SOCKET_ERROR;
    }

    if( ( NULL == pvBuffer ) || ( 0 == xBufferLength ) )
    {
        return SOCKETS_EINVAL;
    }

    if( ( ctx->status & SS_STATUS_CONNECTED ) != SS_STATUS_CONNECTED )
    {
        return SOCKETS_ENOTCONN;
    }

    ctx->recv_flag = ulFlags;

    configASSERT( ctx->ip_socket >= 0 );

    if( ctx->enforce_tls )
    {
        /* Receive through TLS pipe, if negotiated. */
        return TLS_Recv( ctx->tls_ctx, pvBuffer, xBufferLength );
    }
    else
    {
        return prvNetworkRecv( ( void * ) ctx, pvBuffer, xBufferLength );
    }
}

/*-----------------------------------------------------------*/

int32_t SOCKETS_Send( Socket_t xSocket,
                      const void * pvBuffer,
                      size_t xDataLength,
                      uint32_t ulFlags )
{
    ss_ctx_t * ctx;

    if( SOCKETS_INVALID_SOCKET == xSocket )
    {
        return SOCKETS_SOCKET_ERROR;
    }

    if( ( NULL == pvBuffer ) || ( 0 == xDataLength ) )
    {
        return SOCKETS_EINVAL;
    }

    ctx = ( ss_ctx_t * ) xSocket;

    if( ( ctx->status & SS_STATUS_CONNECTED ) != SS_STATUS_CONNECTED )
    {
        return SOCKETS_ENOTCONN;
    }

    configASSERT( ctx->ip_socket >= 0 );
    ctx->send_flag = ulFlags;

    if( ctx->enforce_tls )
    {
        /* Send through TLS pipe, if negotiated. */
        return TLS_Send( ctx->tls_ctx, pvBuffer, xDataLength );
    }
    else
    {
        return prvNetworkSend( ( void * ) ctx, pvBuffer, xDataLength );
    }
}

/*-----------------------------------------------------------*/

int32_t SOCKETS_Shutdown( Socket_t xSocket,
                          uint32_t ulHow )
{
    ss_ctx_t * ctx;
    int ret;

    if( SOCKETS_INVALID_SOCKET == xSocket )
    {
        return SOCKETS_EINVAL;
    }

    ctx = ( ss_ctx_t * ) xSocket;

    configASSERT( ctx->ip_socket >= 0 );
    ret = lwip_shutdown( ctx->ip_socket, ( int ) ulHow );

    if( 0 > ret )
    {
        return SOCKETS_SOCKET_ERROR;
    }

    return SOCKETS_ERROR_NONE;
}

/*-----------------------------------------------------------*/

int32_t SOCKETS_Close( Socket_t xSocket )
{
    ss_ctx_t * ctx;

    if( SOCKETS_INVALID_SOCKET == xSocket )
    {
        return SOCKETS_EINVAL;
    }

    ctx = ( ss_ctx_t * ) xSocket;
    ctx->state = SST_RX_CLOSING;

    lwip_close( ctx->ip_socket );
    prvDecrementRefCount( ctx );

    return SOCKETS_ERROR_NONE;
}


/*-----------------------------------------------------------*/

int32_t SOCKETS_SetSockOpt( Socket_t xSocket,
                            int32_t lLevel,
                            int32_t lOptionName,
                            const void * pvOptionValue,
                            size_t xOptionLength )
{
    ss_ctx_t * ctx;
    int ret = 0;
    char ** ppcAlpnIn = ( char ** ) pvOptionValue;
    size_t xLength = 0;
    uint32_t ulProtocol;

    if( SOCKETS_INVALID_SOCKET == xSocket )
    {
        return SOCKETS_EINVAL;
    }

    ctx = ( ss_ctx_t * ) xSocket;

    configASSERT( ctx->ip_socket >= 0 );

    switch( lOptionName )
    {
        case SOCKETS_SO_RCVTIMEO:
        case SOCKETS_SO_SNDTIMEO:
           {
               TickType_t ticks;
               struct timeval tv;

               ticks = *( ( const TickType_t * ) pvOptionValue );

               tv.tv_sec = TICK_TO_S( ticks );
               tv.tv_usec = TICK_TO_US( ticks % configTICK_RATE_HZ );

               ret = lwip_setsockopt( ctx->ip_socket,
                                      SOL_SOCKET,
                                      lOptionName == SOCKETS_SO_RCVTIMEO ?
                                      SO_RCVTIMEO : SO_SNDTIMEO,
                                      ( struct timeval * ) &tv,
                                      sizeof( tv ) );

               if( 0 != ret )
               {
                   return SOCKETS_EINVAL;
               }

               break;
           }

        case SOCKETS_SO_NONBLOCK:
           {
               int opt;

               if( ( ctx->status & SS_STATUS_CONNECTED ) != SS_STATUS_CONNECTED )
               {
                   return SOCKETS_ENOTCONN;
               }

               opt = 1;

               ret = lwip_ioctl( ctx->ip_socket, FIONBIO, &opt );

               if( 0 != ret )
               {
                   return SOCKETS_EINVAL;
               }

               break;
           }

        case SOCKETS_SO_REQUIRE_TLS:

            if( ctx->status & SS_STATUS_CONNECTED )
            {
                return SOCKETS_EISCONN;
            }

            ctx->enforce_tls = true;
            break;

        case SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE:

            if( ctx->status & SS_STATUS_CONNECTED )
            {
                return SOCKETS_EISCONN;
            }

            if( ( NULL == pvOptionValue ) || ( 0 == xOptionLength ) )
            {
                return SOCKETS_EINVAL;
            }

            if( ctx->server_cert )
            {
                vPortFree( ctx->server_cert );
            }

            ctx->server_cert = pvPortMalloc( xOptionLength + 1 );

            if( NULL == ctx->server_cert )
            {
                return SOCKETS_ENOMEM;
            }

            memset( ctx->server_cert, 0, xOptionLength + 1 );
            memcpy( ctx->server_cert, pvOptionValue, xOptionLength );
            ctx->server_cert_len = xOptionLength;

            break;

        case SOCKETS_SO_SERVER_NAME_INDICATION:

            if( ctx->status & SS_STATUS_CONNECTED )
            {
                return SOCKETS_EISCONN;
            }

            if( ( NULL == pvOptionValue ) || ( 0 == xOptionLength ) )
            {
                return SOCKETS_EINVAL;
            }

            if( ctx->destination )
            {
                vPortFree( ctx->destination );
            }

            ctx->destination = pvPortMalloc( xOptionLength + 1 );

            if( NULL == ctx->destination )
            {
                return SOCKETS_ENOMEM;
            }

            memcpy( ctx->destination, pvOptionValue, xOptionLength );
            ctx->destination[ xOptionLength ] = '\0';

            break;

        case SOCKETS_SO_WAKEUP_CALLBACK:

            if( ( xOptionLength == sizeof( void * ) ) &&
                ( pvOptionValue != NULL ) )
            {
                prvRxSelectSet( ctx, pvOptionValue );
            }
            else
            {
                prvRxSelectClear( ctx );
            }

            break;

        case SOCKETS_SO_ALPN_PROTOCOLS:

            /* Do not set the ALPN option if the socket is already connected. */
            if( ctx->status & SS_STATUS_CONNECTED )
            {
                return SOCKETS_EISCONN;
            }

            /* Allocate a sufficiently long array of pointers. */
            ctx->ulAlpnProtocolsCount = 1 + xOptionLength;

            if( NULL == ( ctx->ppcAlpnProtocols =
                              ( char ** ) pvPortMalloc( ctx->ulAlpnProtocolsCount *
                                                        sizeof( char * ) ) ) )
            {
                return SOCKETS_ENOMEM;
            }
            else
            {
                memset( ctx->ppcAlpnProtocols,
                        0x00,
                        ctx->ulAlpnProtocolsCount * sizeof( char * ) );
            }

            /* Copy each protocol string. */
            for( ulProtocol = 0; ( ulProtocol < ctx->ulAlpnProtocolsCount - 1 );
                 ulProtocol++ )
            {
                xLength = strlen( ppcAlpnIn[ ulProtocol ] );

                if( NULL == ( ctx->ppcAlpnProtocols[ ulProtocol ] =
                                  ( char * ) pvPortMalloc( 1 + xLength ) ) )
                {
                    ctx->ppcAlpnProtocols[ ulProtocol ] = NULL;
                    return SOCKETS_ENOMEM;
                }
                else
                {
                    memcpy( ctx->ppcAlpnProtocols[ ulProtocol ],
                            ppcAlpnIn[ ulProtocol ],
                            xLength );
                    ctx->ppcAlpnProtocols[ ulProtocol ][ xLength ] = '\0';
                }
            }

            break;

        case SOCKETS_SO_TCPKEEPALIVE:

            ret = lwip_setsockopt( ctx->ip_socket,
                                   SOL_SOCKET,
                                   SO_KEEPALIVE,
                                   pvOptionValue,
                                   sizeof( int ) );

            break;

            #if LWIP_TCP_KEEPALIVE
                case SOCKETS_SO_TCPKEEPALIVE_INTERVAL:

                    ret = lwip_setsockopt( ctx->ip_socket,
                                           IPPROTO_TCP,
                                           TCP_KEEPINTVL,
                                           pvOptionValue,
                                           sizeof( int ) );

                    break;

                case SOCKETS_SO_TCPKEEPALIVE_COUNT:

                    ret = lwip_setsockopt( ctx->ip_socket,
                                           IPPROTO_TCP,
                                           TCP_KEEPCNT,
                                           pvOptionValue,
                                           sizeof( int ) );

                    break;

                case SOCKETS_SO_TCPKEEPALIVE_IDLE_TIME:

                    ret = lwip_setsockopt( ctx->ip_socket,
                                           IPPROTO_TCP,
                                           TCP_KEEPIDLE,
                                           pvOptionValue,
                                           sizeof( int ) );

                    break;
            #endif /* if LWIP_TCP_KEEPALIVE */


        default:
            return SOCKETS_ENOPROTOOPT;
    }

    if( 0 > ret )
    {
        return SOCKETS_SOCKET_ERROR;
    }

    return ret;
}

/*-----------------------------------------------------------*/

/*
 * Lwip DNS Found callback, compatible with type "dns_found_callback"
 * declared in lwip/dns.h.
 *
 * NOTE: this resolves only ipv4 addresses; calls to dns_gethostbyname_addrtype()
 * must specify dns_addrtype == LWIP_DNS_ADDRTYPE_IPV4.
 */
static void lwip_dns_found_callback( const char * name,
                                     const ip_addr_t * ipaddr,
                                     void * callback_arg )
{
    uint32_t * addr = ( uint32_t * ) callback_arg;

    if( ipaddr != NULL )
    {
        *addr = *( ( uint32_t * ) ipaddr ); /* NOTE: IPv4 addresses only */
    }
    else
    {
        *addr = 0;
    }
}

/*-----------------------------------------------------------*/

uint32_t SOCKETS_GetHostByName( const char * pcHostName )
{
    uint32_t addr = 0; /* 0 indicates failure to caller */
    err_t xLwipError = ERR_OK;
    ip_addr_t xLwipIpv4Address;
    uint32_t ulDnsResolutionWaitCycles = 0;

    if( strlen( pcHostName ) <= ( size_t ) securesocketsMAX_DNS_NAME_LENGTH )
    {
        xLwipError = dns_gethostbyname_addrtype( pcHostName, &xLwipIpv4Address,
                                                 lwip_dns_found_callback, ( void * ) &addr,
                                                 LWIP_DNS_ADDRTYPE_IPV4 );

        switch( xLwipError )
        {
            case ERR_OK:
                addr = *( ( uint32_t * ) &xLwipIpv4Address ); /* NOTE: IPv4 addresses only */
                break;

            case ERR_INPROGRESS:

                /*
                 * The DNS resolver is working the request.  Wait for it to complete
                 * or time out; print a timeout error message if configured for debug
                 * printing.
                 */
                do
                {
                    vTaskDelay( lwip_dns_resolver_LOOP_DELAY_TICKS );
                }   while( ( ulDnsResolutionWaitCycles++ < lwip_dns_resolver_MAX_WAIT_CYCLES ) && addr == 0 );

                if( addr == 0 )
                {
                    configPRINTF( ( "Unable to resolve (%s) within (%ul) seconds",
                                    pcHostName, lwip_dns_resolver_MAX_WAIT_SECONDS ) );
                }

                break;

            default:
                configPRINTF( ( "Unexpected error (%lu) from dns_gethostbyname_addrtype() while resolving (%s)!",
                                ( uint32_t ) xLwipError, pcHostName ) );
                break;
        }
    }
    else
    {
        addr = 0;
        configPRINTF( ( "Host name (%s) too long!", pcHostName ) );
    }

    return addr;
}

/*-----------------------------------------------------------*/

BaseType_t SOCKETS_Init( void )
{
    BaseType_t xResult = pdPASS;

    dns_init();

    return xResult;
}

/*-----------------------------------------------------------*/
