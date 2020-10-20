/*
 * Amazon FreeRTOS Cellular Preview Release
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#include "iot_config.h"

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "event_groups.h"

/* Define _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE to prevent secure sockets functions
 * from redefining in iot_secure_sockets_wrapper_metrics.h. */
#define _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE

/* Amazon FreeRTOS network include. */
#include "iot_secure_sockets.h"

/* Cellular HAL api includes. */
#include "aws_cellular_config.h"
#include "cellular_config_defaults.h"
#include "cellular_api.h"

/* TLS includes. */
#include "iot_tls.h"

/* Clock includes. */
#include "platform/iot_clock.h"

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_NETWORK
    #define LIBRARY_LOG_LEVEL        IOT_LOG_LEVEL_NETWORK
#else
    #ifdef IOT_LOG_LEVEL_GLOBAL
        #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
    #else
        #define LIBRARY_LOG_LEVEL    IOT_LOG_ERROR
    #endif
#endif
#define LIBRARY_LOG_NAME             "SECURE_SOCKETS_CELLULAR"

/* logging includes. */
#include "iot_logging_setup.h"

/*-----------------------------------------------------------*/

/* Secure socket needs application provide the cellular handle and pdn context id. */
/* User of secure sockets cellular should provide this variable. */
/* coverity[misra_c_2012_rule_8_6_violation] */
extern CellularHandle_t CellularHandle;
/* User of secure sockets cellular should provide this variable. */
/* coverity[misra_c_2012_rule_8_6_violation] */
extern uint8_t CellularSocketPdnContextId;

/*-----------------------------------------------------------*/

/* Windows simulator implementation. */
#if defined( _WIN32 ) || defined( _WIN64 )
    #define strtok_r    strtok_s
#endif

/* Cellular socket access mode. */
#define CELLULAR_SOCKET_ACCESS_MODE           CELLULAR_ACCESSMODE_BUFFER

#define CELLULAR_SOCKET_OPEN_FLAG             ( 1UL << 0 )
#define CELLULAR_SOCKET_CONNECT_FLAG          ( 1UL << 1 )
#define CELLULAR_SOCKET_SECURE_FLAG           ( 1UL << 2 )
#define CELLULAR_SOCKET_READ_CLOSED_FLAG      ( 1UL << 3 )
#define CELLULAR_SOCKET_WRITE_CLOSED_FLAG     ( 1UL << 4 )

#define SOCKET_DATA_RECEIVED_CALLBACK_BIT     ( 0x00000001U )
#define SOCKET_OPEN_CALLBACK_BIT              ( 0x00000002U )
#define SOCKET_OPEN_FAILED_CALLBACK_BIT       ( 0x00000004U )
#define SOCKET_CLOSE_CALLBACK_BIT             ( 0x00000008U )

#define CELLULAR_SOCKET_OPEN_TIMEOUT_TICKS    ( portMAX_DELAY )

#define TICKS_TO_MS( xTicks )    ( ( ( xTicks ) * 1000U ) / ( ( uint32_t ) configTICK_RATE_HZ ) )
#define UINT32_MAX_DELAY_MS                   ( 0xFFFFFFFFUL )
#define UINT32_MAX_MS_TICKS                   ( UINT32_MAX_DELAY_MS / ( TICKS_TO_MS( 1U ) ) )

#define CELLULAR_PDN_CONTEXT_ID_SOCKETS       ( CellularSocketPdnContextId )

#define CELLULAR_SOCKET_RECV_TIMEOUT_MS       ( 1000UL )   /* Cellular recv command timeout. */

#ifndef SOCKETS_ntohs
    #define SOCKETS_ntohs                     SOCKETS_htons
#endif

#if ( CELLULAR_SUPPORT_GETHOSTBYNAME == 0 )
    #define     GETHOSTBYNAME_CACHE_SIZE    ( 10U )
#endif /* if ( CELLULAR_SUPPORT_GETHOSTBYNAME == 0 ) */

/*-----------------------------------------------------------*/

typedef struct xSOCKET
{
    CellularSocketHandle_t cellularSocketHandle;
    uint32_t ulFlags;
    uint32_t xSendFlags;
    uint32_t xRecvFlags;

    TickType_t receiveTimeout;
    TickType_t sendTimeout;

    char * pcDestination;
    void * pvTLSContext;
    char * pcServerCertificate;
    uint32_t ulServerCertificateLength;

    EventGroupHandle_t socketEventGroupHandle;
} _cellularSecureSocket_t;

/*-----------------------------------------------------------*/

static BaseType_t prvNetworkSend( void * ctx,
                                  const uint8_t * buf,
                                  size_t len );
static BaseType_t prvNetworkRecvCellular( const _cellularSecureSocket_t * pCellularSocketContext,
                                          uint8_t * buf,
                                          size_t len );
static BaseType_t prvNetworkRecv( void * ctx,
                                  uint8_t * buf,
                                  size_t len );
static void prvCellularSocketOpenCallback( CellularUrcEvent_t urcEvent,
                                           CellularSocketHandle_t socketHandle,
                                           void * pCallbackContext );
static void prvCellularSocketDataReadyCallback( CellularSocketHandle_t socketHandle,
                                                void * pCallbackContext );
static void prvCellularSocketClosedCallback( CellularSocketHandle_t socketHandle,
                                             void * pCallbackContext );
static int32_t prvSetupSocketNonblock( _cellularSecureSocket_t * pCellularSocketContext );
static int32_t prvSetupSocketRecvTimeout( _cellularSecureSocket_t * pCellularSocketContext,
                                          TickType_t receiveTimeout );
static int32_t prvSetupSocketSendTimeout( _cellularSecureSocket_t * pCellularSocketContext,
                                          TickType_t sendTimeout );
static int32_t prvSetupServerNameIndication( _cellularSecureSocket_t * pCellularSocketContext,
                                             const void * pvOptionValue,
                                             size_t xOptionLength );
static int32_t prvSetupTrustedServerCertificate( _cellularSecureSocket_t * pCellularSocketContext,
                                                 const void * pvOptionValue,
                                                 size_t xOptionLength );
static int32_t prvCellularSocketRegisterCallback( CellularSocketHandle_t tcpSocket,
                                                  _cellularSecureSocket_t * pCellularSocketContext );
static int32_t prvCellularSetupTLS( _cellularSecureSocket_t * pCellularSocketContext );
static bool prvGetNextTok( char ** pDnsResultStr,
                           char delimiter,
                           char ** pToken );
static bool prvConvertStrToNumber( const char * pToken,
                                   uint32_t * pRetValue );
static int32_t prvSocketsAton( char * cp,
                               uint32_t * pIpAddress );
static bool _calculateElapsedTime( uint64_t entryTimeMs,
                                   uint32_t timeoutValueMs,
                                   uint64_t * pElapsedTimeMs );
static int32_t prvCheckSetSockOptParams( Socket_t xSocket,
                                         int32_t lOptionName,
                                         const void * pvOptionValue,
                                         TickType_t * pSockTimeout );

#if ( CELLULAR_SUPPORT_GETHOSTBYNAME == 0 )
    static const char * prvReverseLookup( uint32_t ipAddress );
    static uint32_t prvLookup( const char * pcHostName );
#endif /* if ( CELLULAR_SUPPORT_GETHOSTBYNAME == 0 ) */

/*-----------------------------------------------------------*/

#if ( CELLULAR_SUPPORT_GETHOSTBYNAME == 0 )
    static char _dnsCache[ GETHOSTBYNAME_CACHE_SIZE ][ CELLULAR_IP_ADDRESS_MAX_SIZE + 1 ];
#endif /* if ( CELLULAR_SUPPORT_GETHOSTBYNAME == 0 ) */

/*-----------------------------------------------------------*/

/* This function sends the data until timeout or data is completely sent to server.
 * Send timeout unit is TickType_t. Any timeout value greater than UINT32_MAX_MS_TICKS
 * or portMAX_DELAY will be regarded as MAX deley. In this case, this function
 * will not return until all bytes of data are sent successfully or until an error occurs. */
static BaseType_t prvNetworkSend( void * ctx,
                                  const uint8_t * buf,
                                  size_t len )
{
    CellularSocketHandle_t tcpSocket = NULL;
    BaseType_t retSendLength = 0;
    uint32_t sentLength = 0;
    CellularError_t socketStatus = CELLULAR_SUCCESS;
    _cellularSecureSocket_t * pCellularSocketContext = ( _cellularSecureSocket_t * ) ctx;
    uint32_t bytesToSend = len;
    uint64_t entryTimeMs = IotClock_GetTimeMs();
    uint64_t elapsedTimeMs = 0;
    uint32_t sendTimeoutMs = 0;

    if( pCellularSocketContext == NULL )
    {
        IotLogError( "Cellular prvNetworkSend Invalid xSocket %p", pCellularSocketContext );
        retSendLength = ( BaseType_t ) SOCKETS_SOCKET_ERROR;
    }
    else if( ( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_OPEN_FLAG ) == 0U ) ||
             ( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_CONNECT_FLAG ) == 0U ) )
    {
        /* Don't send the data if write closed is set. Otherwise return error. */
        if( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_WRITE_CLOSED_FLAG ) == 0U )
        {
            IotLogError( "Cellular prvNetworkSend Invalid xSocket flag %p 0x%08x",
                         pCellularSocketContext, pCellularSocketContext->ulFlags );
            retSendLength = ( BaseType_t ) SOCKETS_SOCKET_ERROR;
        }
    }
    else
    {
        tcpSocket = pCellularSocketContext->cellularSocketHandle;

        /* Convert ticks to ms delay. */
        if( ( pCellularSocketContext->sendTimeout >= UINT32_MAX_MS_TICKS ) || ( pCellularSocketContext->sendTimeout >= portMAX_DELAY ) )
        {
            /* Check if the ticks cause overflow. */
            sendTimeoutMs = UINT32_MAX_DELAY_MS;
        }
        else
        {
            sendTimeoutMs = TICKS_TO_MS( pCellularSocketContext->sendTimeout );
        }

        /* Loop sending data until data is sent completly or timeout. */
        while( bytesToSend > 0U )
        {
            socketStatus = Cellular_SocketSend( CellularHandle,
                                                tcpSocket,
                                                &buf[ retSendLength ],
                                                bytesToSend,
                                                &sentLength );

            if( socketStatus == CELLULAR_SUCCESS )
            {
                retSendLength = retSendLength + ( BaseType_t ) sentLength;
                bytesToSend = bytesToSend - sentLength;
            }

            /* Check socket status or timeout break. */
            if( ( socketStatus != CELLULAR_SUCCESS ) ||
                ( _calculateElapsedTime( entryTimeMs, sendTimeoutMs, &elapsedTimeMs ) ) )
            {
                if( socketStatus != CELLULAR_SUCCESS )
                {
                    retSendLength = ( BaseType_t ) SOCKETS_SOCKET_ERROR;
                }

                break;
            }
        }

        IotLogDebug( "prvNetworkSend expect %d write %d", len, sentLength );
    }

    return retSendLength;
}

/*-----------------------------------------------------------*/

/* This function receives data. It returns when non-zero bytes of data is received,
 * when an error occurs, or when timeout occurs. Receive timeout unit is TickType_t.
 * Any timeout value bigger than portMAX_DELAY will be regarded as max delay.
 * In this case, this function will not return until non-zero bytes of data is received
 * or until an error occurs. */
static BaseType_t prvNetworkRecvCellular( const _cellularSecureSocket_t * pCellularSocketContext,
                                          uint8_t * buf,
                                          size_t len )
{
    CellularSocketHandle_t tcpSocket = NULL;
    BaseType_t retRecvLength = 0;
    uint32_t recvLength = 0;
    TickType_t recvTimeout = 0;
    TickType_t recvStartTime = 0;
    CellularError_t socketStatus = CELLULAR_SUCCESS;
    EventBits_t waitEventBits = 0;

    tcpSocket = pCellularSocketContext->cellularSocketHandle;

    if( pCellularSocketContext->receiveTimeout >= portMAX_DELAY )
    {
        recvTimeout = portMAX_DELAY;
    }
    else
    {
        recvTimeout = pCellularSocketContext->receiveTimeout;
    }

    recvStartTime = xTaskGetTickCount();

    ( void ) xEventGroupClearBits( pCellularSocketContext->socketEventGroupHandle,
                                   SOCKET_DATA_RECEIVED_CALLBACK_BIT );
    socketStatus = Cellular_SocketRecv( CellularHandle, tcpSocket, buf, len, &recvLength );

    /* Calculate remain recvTimeout. */
    if( recvTimeout != portMAX_DELAY )
    {
        if( ( recvStartTime + recvTimeout ) > xTaskGetTickCount() )
        {
            recvTimeout = recvTimeout - ( xTaskGetTickCount() - recvStartTime );
        }
        else
        {
            recvTimeout = 0;
        }
    }

    if( ( socketStatus == CELLULAR_SUCCESS ) && ( recvLength == 0U ) &&
        ( recvTimeout != 0U ) )
    {
        waitEventBits = xEventGroupWaitBits( pCellularSocketContext->socketEventGroupHandle,
                                             SOCKET_DATA_RECEIVED_CALLBACK_BIT | SOCKET_CLOSE_CALLBACK_BIT,
                                             pdTRUE,
                                             pdFALSE,
                                             recvTimeout );

        if( ( waitEventBits & SOCKET_CLOSE_CALLBACK_BIT ) != 0U )
        {
            socketStatus = CELLULAR_SOCKET_CLOSED;
        }
        else if( ( waitEventBits & SOCKET_DATA_RECEIVED_CALLBACK_BIT ) != 0U )
        {
            socketStatus = Cellular_SocketRecv( CellularHandle, tcpSocket, buf, len, &recvLength );
        }
        else
        {
            IotLogInfo( "prvNetworkRecv timeout" );
            socketStatus = CELLULAR_SUCCESS;
            recvLength = 0;
        }
    }

    if( socketStatus == CELLULAR_SUCCESS )
    {
        retRecvLength = ( BaseType_t ) recvLength;
    }
    else
    {
        IotLogError( "prvNetworkRecv failed %d", socketStatus );
        retRecvLength = SOCKETS_SOCKET_ERROR;
    }

    IotLogDebug( "prvNetworkRecv expect %d read %d", len, recvLength );
    return retRecvLength;
}

/*-----------------------------------------------------------*/

static BaseType_t prvNetworkRecv( void * ctx,
                                  uint8_t * buf,
                                  size_t len )
{
    _cellularSecureSocket_t * pCellularSocketContext = ( _cellularSecureSocket_t * ) ctx;
    BaseType_t retRecvLength = 0;

    if( pCellularSocketContext == NULL )
    {
        IotLogError( "Cellular prvNetworkRecv Invalid xSocket %p", pCellularSocketContext );
        retRecvLength = SOCKETS_EINVAL;
    }
    else if( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_READ_CLOSED_FLAG ) != 0U )
    {
        IotLogDebug( "Cellular prvNetworkRecv shutdown %p", pCellularSocketContext );
        retRecvLength = 0;
    }
    else if( ( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_OPEN_FLAG ) == 0U ) ||
             ( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_CONNECT_FLAG ) == 0U ) )
    {
        IotLogError( "Cellular prvNetworkRecv Invalid xSocket flag %p",
                     pCellularSocketContext, pCellularSocketContext->ulFlags );
        retRecvLength = SOCKETS_ENOTCONN;
    }
    else
    {
        retRecvLength = prvNetworkRecvCellular( pCellularSocketContext, buf, len );
    }

    return retRecvLength;
}

/*-----------------------------------------------------------*/

/* socketHandle is function prototype. Not used in this callback function. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static void prvCellularSocketOpenCallback( CellularUrcEvent_t urcEvent,
                                           CellularSocketHandle_t socketHandle,
                                           void * pCallbackContext )
{
    _cellularSecureSocket_t * pCellularSocketContext = ( _cellularSecureSocket_t * ) pCallbackContext;

    ( void ) socketHandle;

    if( pCellularSocketContext != NULL )
    {
        IotLogDebug( "Socket open callback on Socket %p %d %d.",
                     pCellularSocketContext, socketHandle, urcEvent );

        if( urcEvent == CELLULAR_URC_SOCKET_OPENED )
        {
            pCellularSocketContext->ulFlags = pCellularSocketContext->ulFlags | CELLULAR_SOCKET_CONNECT_FLAG;
            ( void ) xEventGroupSetBits( pCellularSocketContext->socketEventGroupHandle,
                                         SOCKET_OPEN_CALLBACK_BIT );
        }
        else
        {
            /* Socket open failed. */
            ( void ) xEventGroupSetBits( pCellularSocketContext->socketEventGroupHandle,
                                         SOCKET_OPEN_FAILED_CALLBACK_BIT );
        }
    }
    else
    {
        IotLogError( "Spurious socket open callback." );
    }
}

/*-----------------------------------------------------------*/

/* socketHandle is function prototype. Not used in this callback function. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static void prvCellularSocketDataReadyCallback( CellularSocketHandle_t socketHandle,
                                                void * pCallbackContext )
{
    _cellularSecureSocket_t * pCellularSocketContext = ( _cellularSecureSocket_t * ) pCallbackContext;

    ( void ) socketHandle;

    if( pCellularSocketContext != NULL )
    {
        IotLogDebug( "Data ready on Socket %p", pCellularSocketContext );
        ( void ) xEventGroupSetBits( pCellularSocketContext->socketEventGroupHandle,
                                     SOCKET_DATA_RECEIVED_CALLBACK_BIT );
    }
    else
    {
        IotLogError( "spurious data callback" );
    }
}

/*-----------------------------------------------------------*/

/* socketHandle is function prototype. Not used in this callback function. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static void prvCellularSocketClosedCallback( CellularSocketHandle_t socketHandle,
                                             void * pCallbackContext )
{
    _cellularSecureSocket_t * pCellularSocketContext = ( _cellularSecureSocket_t * ) pCallbackContext;

    ( void ) socketHandle;

    if( pCellularSocketContext != NULL )
    {
        IotLogInfo( "Socket Close on Socket %p", pCellularSocketContext );
        pCellularSocketContext->ulFlags = pCellularSocketContext->ulFlags & ( ~CELLULAR_SOCKET_CONNECT_FLAG );
        ( void ) xEventGroupSetBits( pCellularSocketContext->socketEventGroupHandle,
                                     SOCKET_CLOSE_CALLBACK_BIT );
    }
    else
    {
        IotLogError( "spurious socket close callback" );
    }
}

/*-----------------------------------------------------------*/

static int32_t prvSetupSocketNonblock( _cellularSecureSocket_t * pCellularSocketContext )
{
    CellularError_t socketStatus = CELLULAR_SUCCESS;
    int32_t retSetSockOpt = SOCKETS_ERROR_NONE;
    uint32_t sendTimeoutMs = 0;
    CellularSocketHandle_t tcpSocket = NULL;

    if( pCellularSocketContext == NULL )
    {
        retSetSockOpt = SOCKETS_EINVAL;
    }
    else
    {
        tcpSocket = pCellularSocketContext->cellularSocketHandle;

        /* Setup send timeout. */
        socketStatus = Cellular_SocketSetSockOpt( CellularHandle,
                                                  tcpSocket,
                                                  CELLULAR_SOCKET_OPTION_LEVEL_TRANSPORT,
                                                  CELLULAR_SOCKET_OPTION_SEND_TIMEOUT,
                                                  ( const uint8_t * ) &sendTimeoutMs,
                                                  sizeof( uint32_t ) );

        if( socketStatus != CELLULAR_SUCCESS )
        {
            retSetSockOpt = SOCKETS_EINVAL;
        }
        else
        {
            /* Setup receive timeout. */
            pCellularSocketContext->receiveTimeout = 0;
        }
    }

    return retSetSockOpt;
}

/*-----------------------------------------------------------*/

static int32_t prvSetupSocketRecvTimeout( _cellularSecureSocket_t * pCellularSocketContext,
                                          TickType_t receiveTimeout )
{
    int32_t retSetSockOpt = SOCKETS_ERROR_NONE;

    if( pCellularSocketContext == NULL )
    {
        retSetSockOpt = SOCKETS_EINVAL;
    }
    else
    {
        if( receiveTimeout >= portMAX_DELAY )
        {
            pCellularSocketContext->receiveTimeout = portMAX_DELAY;
        }
        else
        {
            pCellularSocketContext->receiveTimeout = receiveTimeout;
        }
    }

    return retSetSockOpt;
}

/*-----------------------------------------------------------*/

/* Send timeout unit is TickType_t. The underlying cellular API uses miliseconds for timeout.
 * Any send timeout greater than UINT32_MAX_MS_TICKS( UINT32_MAX_DELAY_MS/MS_PER_TICKS ) or
 * portMAX_DELAY is regarded as UINT32_MAX_DELAY_MS for cellular API. */
static int32_t prvSetupSocketSendTimeout( _cellularSecureSocket_t * pCellularSocketContext,
                                          TickType_t sendTimeout )
{
    CellularError_t socketStatus = CELLULAR_SUCCESS;
    int32_t retSetSockOpt = SOCKETS_ERROR_NONE;
    uint32_t sendTimeoutMs = 0;
    CellularSocketHandle_t tcpSocket = NULL;

    if( pCellularSocketContext == NULL )
    {
        retSetSockOpt = SOCKETS_EINVAL;
    }
    else
    {
        tcpSocket = pCellularSocketContext->cellularSocketHandle;

        if( sendTimeout >= UINT32_MAX_MS_TICKS )
        {
            /* Check if the ticks cause overflow. */
            pCellularSocketContext->sendTimeout = portMAX_DELAY;
            sendTimeoutMs = UINT32_MAX_DELAY_MS;
        }
        else if( sendTimeout >= portMAX_DELAY )
        {
            IotLogWarn( "Sendtimeout %d longer than portMAX_DELAY, %d ms is used instead",
                        sendTimeout, UINT32_MAX_DELAY_MS );
            pCellularSocketContext->sendTimeout = portMAX_DELAY;
            sendTimeoutMs = UINT32_MAX_DELAY_MS;
        }
        else
        {
            pCellularSocketContext->sendTimeout = sendTimeout;
            sendTimeoutMs = TICKS_TO_MS( sendTimeout );
        }

        socketStatus = Cellular_SocketSetSockOpt( CellularHandle,
                                                  tcpSocket,
                                                  CELLULAR_SOCKET_OPTION_LEVEL_TRANSPORT,
                                                  CELLULAR_SOCKET_OPTION_SEND_TIMEOUT,
                                                  ( const uint8_t * ) &sendTimeoutMs,
                                                  sizeof( uint32_t ) );

        if( socketStatus != CELLULAR_SUCCESS )
        {
            retSetSockOpt = SOCKETS_EINVAL;
        }
    }

    return retSetSockOpt;
}

/*-----------------------------------------------------------*/

static int32_t prvSetupServerNameIndication( _cellularSecureSocket_t * pCellularSocketContext,
                                             const void * pvOptionValue,
                                             size_t xOptionLength )
{
    int32_t retSetSockOpt = SOCKETS_ERROR_NONE;

    if( ( pCellularSocketContext == NULL ) || ( pvOptionValue == NULL ) )
    {
        retSetSockOpt = SOCKETS_EINVAL;
    }
    else
    {
        if( pCellularSocketContext->pcDestination != NULL )
        {
            IotLogWarn( "Server name indication is already set. Previous setting is freed and set the new setting." );
            vPortFree( pCellularSocketContext->pcDestination );
            pCellularSocketContext->pcDestination = NULL;
        }

        pCellularSocketContext->pcDestination = ( char * ) pvPortMalloc( 1U + xOptionLength );

        if( pCellularSocketContext->pcDestination == NULL )
        {
            retSetSockOpt = SOCKETS_ENOMEM;
        }
        else
        {
            ( void ) memcpy( ( void * ) pCellularSocketContext->pcDestination, pvOptionValue, xOptionLength );
            pCellularSocketContext->pcDestination[ xOptionLength ] = '\0';
        }
    }

    return retSetSockOpt;
}

/*-----------------------------------------------------------*/

static int32_t prvSetupTrustedServerCertificate( _cellularSecureSocket_t * pCellularSocketContext,
                                                 const void * pvOptionValue,
                                                 size_t xOptionLength )
{
    int32_t retSetSockOpt = SOCKETS_ERROR_NONE;

    if( ( pCellularSocketContext == NULL ) || ( pvOptionValue == NULL ) )
    {
        retSetSockOpt = SOCKETS_EINVAL;
    }
    else
    {
        if( pCellularSocketContext->pcServerCertificate != NULL )
        {
            IotLogWarn( "Server certificate is already set. Previous setting is freed and set the new setting." );
            vPortFree( pCellularSocketContext->pcServerCertificate );
            pCellularSocketContext->pcServerCertificate = NULL;
        }

        pCellularSocketContext->pcServerCertificate = ( char * ) pvPortMalloc( xOptionLength );

        if( pCellularSocketContext->pcServerCertificate == NULL )
        {
            retSetSockOpt = SOCKETS_ENOMEM;
        }
        else
        {
            ( void ) memcpy( ( void * ) pCellularSocketContext->pcServerCertificate, pvOptionValue, xOptionLength );
            pCellularSocketContext->ulServerCertificateLength = xOptionLength;
        }
    }

    return retSetSockOpt;
}

/*-----------------------------------------------------------*/

static int32_t prvCellularSocketRegisterCallback( CellularSocketHandle_t tcpSocket,
                                                  _cellularSecureSocket_t * pCellularSocketContext )
{
    int32_t retRegCallback = SOCKETS_ERROR_NONE;
    CellularError_t socketStatus = CELLULAR_SUCCESS;

    if( tcpSocket == NULL )
    {
        retRegCallback = SOCKETS_EINVAL;
    }

    if( retRegCallback == SOCKETS_ERROR_NONE )
    {
        socketStatus = Cellular_SocketRegisterDataReadyCallback( CellularHandle, tcpSocket,
                                                                 prvCellularSocketDataReadyCallback, ( void * ) pCellularSocketContext );

        if( socketStatus != CELLULAR_SUCCESS )
        {
            IotLogError( "Failed to SocketRegisterDataReadyCallback. Socket status %d.", socketStatus );
            retRegCallback = SOCKETS_SOCKET_ERROR;
        }
    }

    if( retRegCallback == SOCKETS_ERROR_NONE )
    {
        socketStatus = Cellular_SocketRegisterSocketOpenCallback( CellularHandle, tcpSocket,
                                                                  prvCellularSocketOpenCallback, ( void * ) pCellularSocketContext );

        if( socketStatus != CELLULAR_SUCCESS )
        {
            IotLogError( "Failed to SocketRegisterSocketOpenCallbac. Socket status %d.", socketStatus );
            retRegCallback = SOCKETS_SOCKET_ERROR;
        }
    }

    if( retRegCallback == SOCKETS_ERROR_NONE )
    {
        socketStatus = Cellular_SocketRegisterClosedCallback( CellularHandle, tcpSocket,
                                                              prvCellularSocketClosedCallback, ( void * ) pCellularSocketContext );

        if( socketStatus != CELLULAR_SUCCESS )
        {
            IotLogError( "Failed to SocketRegisterClosedCallback. Socket status %d.", socketStatus );
            retRegCallback = SOCKETS_SOCKET_ERROR;
        }
    }

    return retRegCallback;
}

/*-----------------------------------------------------------*/

static int32_t prvCellularSetupTLS( _cellularSecureSocket_t * pCellularSocketContext )
{
    int32_t retSetupTLS = SOCKETS_ERROR_NONE;
    TLSParams_t xTLSParams = { 0 };
    BaseType_t tlsRet = pdFREERTOS_ERRNO_NONE;

    if( pCellularSocketContext == NULL )
    {
        retSetupTLS = SOCKETS_EINVAL;
    }

    if( retSetupTLS == SOCKETS_ERROR_NONE )
    {
        xTLSParams.ulSize = sizeof( xTLSParams );
        xTLSParams.pcDestination = pCellularSocketContext->pcDestination;
        xTLSParams.pcServerCertificate = pCellularSocketContext->pcServerCertificate;
        xTLSParams.ulServerCertificateLength = pCellularSocketContext->ulServerCertificateLength;
        xTLSParams.pvCallerContext = ( void * ) pCellularSocketContext;
        xTLSParams.pxNetworkRecv = prvNetworkRecv;
        xTLSParams.pxNetworkSend = prvNetworkSend;

        /* Initialize TLS. */
        tlsRet = TLS_Init( &pCellularSocketContext->pvTLSContext, &xTLSParams );

        if( tlsRet != pdFREERTOS_ERRNO_NONE )
        {
            IotLogError( "Failed to TLS_Init. tls status %d.", tlsRet );
            retSetupTLS = SOCKETS_TLS_INIT_ERROR;
        }

        /* Initiate TLS handshake. */
        if( retSetupTLS == SOCKETS_ERROR_NONE )
        {
            tlsRet = TLS_Connect( pCellularSocketContext->pvTLSContext );

            if( tlsRet != pdFREERTOS_ERRNO_NONE )
            {
                IotLogError( "Failed to TLS_Connect. tls status %d.", tlsRet );
                retSetupTLS = SOCKETS_TLS_HANDSHAKE_ERROR;
            }
        }
    }

    return retSetupTLS;
}

/*-----------------------------------------------------------*/

static bool prvGetNextTok( char ** pDnsResultStr,
                           char delimiter,
                           char ** pToken )
{
    bool ret = true;
    uint32_t strIndex = 0;
    char * pDnsResultStrStart = *pDnsResultStr;

    if( ( **pDnsResultStr == delimiter ) || ( **pDnsResultStr == '\0' ) )
    {
        ret = false;
    }
    else
    {
        *pToken = ( char * ) *pDnsResultStr;

        while( ( **pDnsResultStr != delimiter ) && ( **pDnsResultStr != '\0' ) )
        {
            strIndex = strIndex + 1U;
            *pDnsResultStr = &( pDnsResultStrStart )[ strIndex ];
        }

        /* Move 1 char ahead after the delimiter. */
        if( **pDnsResultStr == delimiter )
        {
            **pDnsResultStr = '\0';
            strIndex = strIndex + 1U;
            *pDnsResultStr = &( pDnsResultStrStart )[ strIndex ];
        }
    }

    return ret;
}


/*-----------------------------------------------------------*/

/* Internal function of prvSocketsAton to reduce complexity. */
static bool prvConvertStrToNumber( const char * pToken,
                                   uint32_t * pRetValue )
{
    bool retStatus = true;
    int32_t retStrtol = 0;
    char * pEndStr = NULL;

    if( ( pToken == NULL ) || ( pRetValue == NULL ) )
    {
        retStatus = false;
    }
    else
    {
        /* MISRA C 2012 Directive 4.7 – If a function returns error information, then
         * that error information shall be tested.
         * MISRA C 2012 Rule 22.8 – The errno variable must be "zero" before calling
         * strtol function.
         * MISRA C 2012 Rule 22.9 – The errno must be tested after strtol function is
         * called.
         *
         * The following line violates MISRA rule 4.7 because return value of strtol()
         * is not checked for error. Also, it violates MISRA rule 22.8 because variable
         * "errno" is neither used nor initialized to zero before strtol() is called.
         * It violates MISRA Rule 22.9 because Variable "errno" is not checked for error
         * after strtol() is called.
         *
         * The above three violations are justified because error checking by "errno"
         * for any POSIX API is not thread safe in FreeRTOS unless "configUSE_POSIX_ERRNO"
         * is enabled. In order to avoid the dependency on this feature, errno variable
         * is not used. The function strtol returns LONG_MIN and LONG_MAX in case of
         * underflow and overflow respectively and sets the errno to ERANGE. It is not
         * possible to distinguish between valid LONG_MIN and LONG_MAX return values
         * and underflow and overflow scenarios without checking errno. Therefore, we
         * cannot check return value of strtol for errors. We ensure that strtol performed
         * a valid conversion by checking that *pEndPtr is '\0'. strtol stores the address
         * of the first invalid character in *pEndPtr and therefore, '\0' value of *pEndPtr
         * means that the complete pToken string passed for conversion was valid and a valid
         * conversion was performed. */
        /* coverity[misra_c_2012_directive_4_7_violation] */
        /* coverity[misra_c_2012_rule_22_8_violation] */
        /* coverity[misra_c_2012_rule_22_9_violation] */
        retStrtol = strtol( pToken, &pEndStr, 10 );

        if( ( *pEndStr == '\0' ) && ( retStrtol >= 0 ) )
        {
            *pRetValue = ( uint32_t ) retStrtol;
        }
        else
        {
            retStatus = false;
        }
    }

    return retStatus;
}

/*-----------------------------------------------------------*/

/* This function is a simplifed version of int inet_aton(const char *cp, struct in_addr *addr)
 * Only accept IPV4 "xxx.xxx.xxx.xxx" format.
 * return 1 for success, 0 for error. */
static int32_t prvSocketsAton( char * cp,
                               uint32_t * pIpAddress )
{
    int32_t ret = 1;
    uint32_t i = 0;
    uint32_t tempValue = 0;
    char * pToken = NULL;
    uint32_t ulIP[ 4 ] = { 0 };
    bool retGetToken = true;
    char * pLocatCp = cp;

    for( ; i < ( uint32_t ) 4; i++ )
    {
        /* Get next tok. */
        retGetToken = prvGetNextTok( &pLocatCp, '.', &pToken );

        /* Convert the pToken to int. */
        if( retGetToken == true )
        {
            if( prvConvertStrToNumber( pToken, &tempValue ) == false )
            {
                ret = 0;
            }
        }
        else
        {
            ret = 0;
        }

        if( ret == 1 )
        {
            ulIP[ i ] = tempValue;
        }
        else
        {
            break;
        }
    }

    /* store the data to output parameter. */
    if( ret == 1 )
    {
        *pIpAddress = SOCKETS_inet_addr_quick( ulIP[ 0 ], ulIP[ 1 ], ulIP[ 2 ], ulIP[ 3 ] );
    }

    return ret;
}

/*-----------------------------------------------------------*/

static bool _calculateElapsedTime( uint64_t entryTimeMs,
                                   uint32_t timeoutValueMs,
                                   uint64_t * pElapsedTimeMs )
{
    uint64_t currentTimeMs = IotClock_GetTimeMs();
    bool isExpired = false;

    /* timeoutValueMs with UINT32_MAX_DELAY_MS means wait for ever, same behavior as freertos_plus_tcp. */
    if( timeoutValueMs == UINT32_MAX_DELAY_MS )
    {
        isExpired = false;
    }

    /* timeoutValueMs = 0 means none blocking mode. */
    else if( timeoutValueMs == 0U )
    {
        isExpired = true;
    }
    else
    {
        *pElapsedTimeMs = currentTimeMs - entryTimeMs;

        if( ( currentTimeMs - entryTimeMs ) >= timeoutValueMs )
        {
            isExpired = true;
        }
        else
        {
            isExpired = false;
        }
    }

    return isExpired;
}

/*-----------------------------------------------------------*/

static int32_t prvCheckSetSockOptParams( Socket_t xSocket,
                                         int32_t lOptionName,
                                         const void * pvOptionValue,
                                         TickType_t * pSockTimeout )
{
    _cellularSecureSocket_t * pCellularSocketContext = ( _cellularSecureSocket_t * ) xSocket;
    int32_t retSetSockOpt = SOCKETS_ERROR_NONE;

    /* xSocket need to be check against SOCKET_INVALID_SOCKET. */
    /* coverity[misra_c_2012_rule_11_4_violation] */
    if( ( pCellularSocketContext == NULL ) || ( xSocket == SOCKETS_INVALID_SOCKET ) ||
        ( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_OPEN_FLAG ) == 0U ) )
    {
        IotLogError( "Invalid xSocket %p", pCellularSocketContext );
        retSetSockOpt = SOCKETS_SOCKET_ERROR;
    }
    else if( ( pvOptionValue == NULL ) && ( ( lOptionName == SOCKETS_SO_SERVER_NAME_INDICATION ) ||
                                            ( lOptionName == SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE ) ||
                                            ( lOptionName == SOCKETS_SO_SNDTIMEO ) ||
                                            ( lOptionName == SOCKETS_SO_RCVTIMEO ) ) )
    {
        /* These socket options require option value. Return error if option value is not
         * provided. */
        retSetSockOpt = SOCKETS_EINVAL;
    }
    else if( ( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_CONNECT_FLAG ) != 0U ) &&
             ( ( lOptionName == SOCKETS_SO_SERVER_NAME_INDICATION ) ||
               ( lOptionName == SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE ) ||
               ( lOptionName == SOCKETS_SO_REQUIRE_TLS ) ) )
    {
        /* TLS socket option can only be set before connection. */
        retSetSockOpt = SOCKETS_EISCONN;
    }
    else if( ( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_CONNECT_FLAG ) == 0U ) &&
             ( lOptionName == SOCKETS_SO_NONBLOCK ) )
    {
        /* Nonblock option can only be set after connection. */
        retSetSockOpt = SOCKETS_ENOTCONN;
    }
    else if( ( lOptionName == SOCKETS_SO_SNDTIMEO ) || ( lOptionName == SOCKETS_SO_RCVTIMEO ) )
    {
        *pSockTimeout = *( ( const TickType_t * ) pvOptionValue );

        /* Comply with Berkeley standard - a 0 timeout is wait forever. */
        if( *pSockTimeout == 0U )
        {
            *pSockTimeout = portMAX_DELAY;
        }
    }
    else
    {
        /* Empty Else MISRA 15.7 */
    }

    return retSetSockOpt;
}

/*-----------------------------------------------------------*/

#if ( ( CELLULAR_SUPPORT_GETHOSTBYNAME == 0 ) )

    static uint32_t prvLookup( const char * pcHostName )
    {
        uint32_t resolvedAddress = 0, i = 0, foundInCache = 0;
        uint32_t hostNameLength = strlen( pcHostName ); /* pcHostName is checked by caller. */
        uint32_t cacheEntryLength = 0;

        /* Ensure that the provided DNS name is of valid length. */
        if( hostNameLength <= CELLULAR_IP_ADDRESS_MAX_SIZE )
        {
            /* Check if the provided DNS name already exists in cache. */
            for( i = 0; i < GETHOSTBYNAME_CACHE_SIZE; i++ )
            {
                cacheEntryLength = strlen( _dnsCache[ i ] );

                /* The length check is necessary to avoid false substring matches. */
                if( ( cacheEntryLength == hostNameLength ) &&
                    ( strncmp( _dnsCache[ i ], pcHostName, hostNameLength ) == 0 ) )
                {
                    foundInCache = 1;
                    break;
                }
            }

            if( foundInCache == 1 )
            {
                /* We return i + 1 to avoid returing 0 which is used to indicate a
                 * lookup failure. */
                resolvedAddress = i + 1;
            }
            else
            {
                /* Find an empty place in the cache and copy the DNS name. */
                for( i = 0; i < GETHOSTBYNAME_CACHE_SIZE; i++ )
                {
                    if( _dnsCache[ i ][ 0 ] == '\0' )
                    {
                        strncpy( _dnsCache[ i ], pcHostName, CELLULAR_IP_ADDRESS_MAX_SIZE );

                        IotLogWarn( "Store %s in cache %d", pcHostName, i );

                        /* Ensure NULL termination. */
                        _dnsCache[ i ][ hostNameLength ] = '\0';

                        break;
                    }
                }

                /* Was an empty place found? */
                if( i < GETHOSTBYNAME_CACHE_SIZE )
                {
                    resolvedAddress = i + 1;
                }
            }
        }

        return resolvedAddress;
    }

/*-----------------------------------------------------------*/

    static const char * prvReverseLookup( uint32_t ipAddress )
    {
        char * hostName = NULL;

        /* Ensure that the provided IP address is a valid one resolved by us
         * earlier.*/
        if( ( ipAddress > 0 ) && ( ipAddress <= GETHOSTBYNAME_CACHE_SIZE ) )
        {
            hostName = _dnsCache[ ( ipAddress - 1 ) ];
        }

        return hostName;
    }
#endif /* ( ( CELLULAR_SUPPORT_GETHOSTBYNAME == 0 ) ) */

/*-----------------------------------------------------------*/

/* Standard secure socket api implementation. */
/* coverity[misra_c_2012_rule_8_7_violation] */
Socket_t SOCKETS_Socket( int32_t lDomain,
                         int32_t lType,
                         int32_t lProtocol )
{
    Socket_t retSocket = NULL;
    CellularSocketHandle_t tcpSocket = NULL;
    _cellularSecureSocket_t * pCellularSocketContext = NULL;
    CellularError_t socketStatus = CELLULAR_INVALID_HANDLE;

    /* Ensure that only supported values are supplied. */
    /* configASSET is used to check the function not supported in secure sockets. */
    /* coverity[misra_c_2012_rule_10_4_violation] */
    /* coverity[misra_c_2012_rule_10_5_violation] */
    configASSERT( lDomain == SOCKETS_AF_INET );
    /* coverity[misra_c_2012_rule_10_4_violation] */
    /* coverity[misra_c_2012_rule_10_5_violation] */
    configASSERT( lType == SOCKETS_SOCK_STREAM );
    /* coverity[misra_c_2012_rule_10_4_violation] */
    /* coverity[misra_c_2012_rule_10_5_violation] */
    configASSERT( lProtocol == SOCKETS_IPPROTO_TCP );

    /* Create a new TCP socket. */
    socketStatus = Cellular_CreateSocket( CellularHandle,
                                          CELLULAR_PDN_CONTEXT_ID_SOCKETS,
                                          CELLULAR_SOCKET_DOMAIN_AF_INET,
                                          CELLULAR_SOCKET_TYPE_STREAM,
                                          CELLULAR_SOCKET_PROTOCOL_TCP,
                                          &tcpSocket );

    if( socketStatus == CELLULAR_SUCCESS )
    {
        pCellularSocketContext = pvPortMalloc( sizeof( _cellularSecureSocket_t ) );

        if( pCellularSocketContext == NULL )
        {
            IotLogError( "Failed to allocate new socket context." );
            retSocket = NULL;
            ( void ) Cellular_SocketClose( CellularHandle, tcpSocket );
        }
        else
        {
            /* Initialize all the members to sane values. */
            IotLogDebug( "Created CELLULAR Socket %p.", pCellularSocketContext );
            ( void ) memset( pCellularSocketContext, 0, sizeof( _cellularSecureSocket_t ) );
            pCellularSocketContext->cellularSocketHandle = tcpSocket;
            pCellularSocketContext->ulFlags |= CELLULAR_SOCKET_OPEN_FLAG;
            pCellularSocketContext->xSendFlags = 0U;
            pCellularSocketContext->xRecvFlags = 0U;
            pCellularSocketContext->receiveTimeout = ( TickType_t ) socketsconfigDEFAULT_RECV_TIMEOUT;
            pCellularSocketContext->sendTimeout = ( TickType_t ) socketsconfigDEFAULT_SEND_TIMEOUT;
            pCellularSocketContext->pcDestination = NULL;
            pCellularSocketContext->pvTLSContext = NULL;
            pCellularSocketContext->pcServerCertificate = NULL;
            pCellularSocketContext->ulServerCertificateLength = 0U;
            pCellularSocketContext->socketEventGroupHandle = NULL;
            retSocket = ( Socket_t ) pCellularSocketContext;
        }
    }

    if( retSocket != NULL )
    {
        pCellularSocketContext->socketEventGroupHandle = xEventGroupCreate();

        if( pCellularSocketContext->socketEventGroupHandle == NULL )
        {
            IotLogError( "Failed create cellular socket eventGroupHandle %p.", pCellularSocketContext );
            ( void ) Cellular_SocketClose( CellularHandle, tcpSocket );
            vPortFree( pCellularSocketContext );
            retSocket = NULL;
        }
    }

    return retSocket;
}

/*-----------------------------------------------------------*/

/* Standard secure socket api implementation. */
/* coverity[misra_c_2012_rule_8_7_violation] */
int32_t SOCKETS_Connect( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t xAddressLength )
{
    /* Initialize the local variable to zero. */
    /* coverity[misra_c_2012_rule_10_3_violation] */
    CellularSocketAddress_t serverAddress = { 0 };
    CellularSocketHandle_t tcpSocket = NULL;
    CellularError_t socketStatus = CELLULAR_SUCCESS;
    EventBits_t waitEventBits = 0;
    _cellularSecureSocket_t * pCellularSocketContext = ( _cellularSecureSocket_t * ) xSocket;
    int32_t retConnect = SOCKETS_ERROR_NONE;
    uint32_t tlsFlag = 0;
    const uint32_t defaultReceiveTimeoutMs = CELLULAR_SOCKET_RECV_TIMEOUT_MS;

    #if ( CELLULAR_SUPPORT_GETHOSTBYNAME == 0 )
        const char * pHostname = NULL;
    #endif /* if ( CELLULAR_SUPPORT_GETHOSTBYNAME == 0 ) */

    ( void ) xAddressLength;

    /* xSocket need to be check against SOCKET_INVALID_SOCKET. */
    /* coverity[misra_c_2012_rule_11_4_violation] */
    if( ( pCellularSocketContext == NULL ) || ( xSocket == SOCKETS_INVALID_SOCKET ) ||
        ( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_OPEN_FLAG ) == 0U ) )
    {
        IotLogError( "Cellular connect Invalid xSocket %p", pCellularSocketContext );
        retConnect = SOCKETS_EINVAL;
    }
    else if( pxAddress == NULL )
    {
        IotLogError( "Cellular connect Invalid Address %p", pxAddress );
        retConnect = SOCKETS_EINVAL;
    }
    else
    {
        tcpSocket = pCellularSocketContext->cellularSocketHandle;
        tlsFlag = pCellularSocketContext->ulFlags & CELLULAR_SOCKET_SECURE_FLAG;
    }

    if( retConnect == SOCKETS_ERROR_NONE )
    {
        serverAddress.ipAddress.ipAddressType = CELLULAR_IP_ADDRESS_V4;

        #if ( CELLULAR_SUPPORT_GETHOSTBYNAME == 0 )
            pHostname = prvReverseLookup( pxAddress->ulAddress );

            if( pHostname != NULL )
            {
                strncpy( serverAddress.ipAddress.ipAddress, pHostname, CELLULAR_IP_ADDRESS_MAX_SIZE );
            }
            else
            {
                /* This macro is defined in secure sockets header file.
                 * It should be used during address convert in secure sockets implementation. */
                /* coverity[misra_c_2012_directive_4_6_violation] */
                /* coverity[misra_c_2012_rule_21_6_violation] */
                ( void ) SOCKETS_inet_ntoa( pxAddress->ulAddress, serverAddress.ipAddress.ipAddress );
            }
        #else /* if ( CELLULAR_SUPPORT_GETHOSTBYNAME == 0 ) */

            /* This macro is defined in secure sockets header file.
             * It should be used during address convert in secure sockets implementation. */
            /* coverity[misra_c_2012_directive_4_6_violation] */
            /* coverity[misra_c_2012_rule_21_6_violation] */
            ( void ) SOCKETS_inet_ntoa( pxAddress->ulAddress, serverAddress.ipAddress.ipAddress );
        #endif /* if ( CELLULAR_SUPPORT_GETHOSTBYNAME == 0 ) */

        /* Cellular socket use host endian. */
        serverAddress.port = SOCKETS_ntohs( pxAddress->usPort );
        IotLogDebug( "Ip address %s port %d\r\n", serverAddress.ipAddress.ipAddress, serverAddress.port );
        retConnect = prvCellularSocketRegisterCallback( tcpSocket, pCellularSocketContext );
    }

    if( retConnect == SOCKETS_ERROR_NONE )
    {
        ( void ) xEventGroupClearBits( pCellularSocketContext->socketEventGroupHandle,
                                       SOCKET_DATA_RECEIVED_CALLBACK_BIT | SOCKET_OPEN_FAILED_CALLBACK_BIT );
        socketStatus = Cellular_SocketConnect( CellularHandle, tcpSocket, CELLULAR_SOCKET_ACCESS_MODE, &serverAddress );

        if( socketStatus != CELLULAR_SUCCESS )
        {
            IotLogError( "Failed to establish new connection. Socket status %d.", socketStatus );
            retConnect = SOCKETS_SOCKET_ERROR;
        }
    }

    /* Set cellular socket recv AT command default timeout. */
    if( retConnect == SOCKETS_ERROR_NONE )
    {
        socketStatus = Cellular_SocketSetSockOpt( CellularHandle,
                                                  tcpSocket,
                                                  CELLULAR_SOCKET_OPTION_LEVEL_TRANSPORT,
                                                  CELLULAR_SOCKET_OPTION_RECV_TIMEOUT,
                                                  ( const uint8_t * ) &defaultReceiveTimeoutMs,
                                                  sizeof( uint32_t ) );
    }

    if( retConnect == SOCKETS_ERROR_NONE )
    {
        waitEventBits = xEventGroupWaitBits( pCellularSocketContext->socketEventGroupHandle,
                                             SOCKET_OPEN_CALLBACK_BIT | SOCKET_OPEN_FAILED_CALLBACK_BIT,
                                             pdTRUE,
                                             pdFALSE,
                                             CELLULAR_SOCKET_OPEN_TIMEOUT_TICKS );

        if( waitEventBits != SOCKET_OPEN_CALLBACK_BIT )
        {
            IotLogError( "Socket connect timeout." );
            retConnect = SOCKETS_ENOTCONN;
        }
    }

    /* Setup TLS parameters. */
    if( ( retConnect == SOCKETS_ERROR_NONE ) && ( tlsFlag != 0U ) )
    {
        retConnect = prvCellularSetupTLS( pCellularSocketContext );
    }

    return retConnect;
}

/*-----------------------------------------------------------*/

/* Standard secure socket api implementation. */
/* coverity[misra_c_2012_rule_8_7_violation] */
int32_t SOCKETS_Recv( Socket_t xSocket,
                      void * pvBuffer,
                      size_t xBufferLength,
                      uint32_t ulFlags )
{
    _cellularSecureSocket_t * pCellularSocketContext = ( _cellularSecureSocket_t * ) xSocket;
    uint32_t tlsFlag = 0;
    int32_t retRecvLength = SOCKETS_ERROR_NONE;
    BaseType_t bytesRecv = 0;

    /* xSocket need to be check against SOCKET_INVALID_SOCKET. */
    /* coverity[misra_c_2012_rule_11_4_violation] */
    if( ( pCellularSocketContext == NULL ) || ( xSocket == SOCKETS_INVALID_SOCKET ) ||
        ( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_OPEN_FLAG ) == 0U ) )
    {
        IotLogDebug( "Cellular recv Invalid xSocket %p", pCellularSocketContext );
        retRecvLength = SOCKETS_EINVAL;
    }
    else if( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_CONNECT_FLAG ) == 0U )
    {
        IotLogDebug( "Cellular recv Invalid xSocket %p not conn", pCellularSocketContext );
        retRecvLength = SOCKETS_ENOTCONN;
    }
    else if( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_READ_CLOSED_FLAG ) != 0U )
    {
        IotLogDebug( "Cellular recv Invalid xSocket %p closed", pCellularSocketContext );
        retRecvLength = SOCKETS_ECLOSED;
    }
    else if( pvBuffer == NULL )
    {
        IotLogDebug( "Cellular recv Invalid parameter pvBuffer %p", pvBuffer );
        retRecvLength = SOCKETS_EINVAL;
    }
    else
    {
        pCellularSocketContext->xRecvFlags = ulFlags;
        tlsFlag = pCellularSocketContext->ulFlags & CELLULAR_SOCKET_SECURE_FLAG;
    }

    /* Socket Receive operation. */
    if( retRecvLength == SOCKETS_ERROR_NONE )
    {
        if( tlsFlag != 0U )
        {
            bytesRecv = TLS_Recv( pCellularSocketContext->pvTLSContext, pvBuffer, xBufferLength );
        }
        else
        {
            bytesRecv = prvNetworkRecv( pCellularSocketContext, pvBuffer, xBufferLength );
        }

        /* Check if gracefully shutdown again. */
        if( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_READ_CLOSED_FLAG ) != 0U )
        {
            IotLogInfo( "Cellular recv Invalid xSocket %p closed in receive", pCellularSocketContext );
            retRecvLength = SOCKETS_ECLOSED;
        }
        else
        {
            retRecvLength = ( int32_t ) bytesRecv;
        }
    }

    IotLogDebug( "(Network connection %p) Recv %d bytes.", pCellularSocketContext, retRecvLength );
    return retRecvLength;
}

/*-----------------------------------------------------------*/

/* Standard secure socket api implementation. */
/* coverity[misra_c_2012_rule_8_7_violation] */
int32_t SOCKETS_Send( Socket_t xSocket,
                      const void * pvBuffer,
                      size_t xDataLength,
                      uint32_t ulFlags )
{
    _cellularSecureSocket_t * pCellularSocketContext = ( _cellularSecureSocket_t * ) xSocket;
    uint32_t tlsFlag = 0;
    int32_t retSentLength = SOCKETS_ERROR_NONE;
    BaseType_t bytesSent = 0;

    /* xSocket need to be check against SOCKET_INVALID_SOCKET. */
    /* coverity[misra_c_2012_rule_11_4_violation] */
    if( ( pCellularSocketContext == NULL ) || ( xSocket == SOCKETS_INVALID_SOCKET ) ||
        ( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_OPEN_FLAG ) == 0U ) )
    {
        IotLogInfo( "Cellular send Invalid xSocket %p", pCellularSocketContext );
        retSentLength = SOCKETS_EINVAL;
    }
    else if( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_CONNECT_FLAG ) == 0U )
    {
        retSentLength = SOCKETS_ENOTCONN;
    }
    else if( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_WRITE_CLOSED_FLAG ) != 0U )
    {
        retSentLength = SOCKETS_ECLOSED;
    }
    else if( pvBuffer == NULL )
    {
        IotLogError( "Cellular send Invalid parameter pvBuffer %p", pvBuffer );
        retSentLength = SOCKETS_EINVAL;
    }
    else
    {
        pCellularSocketContext->xSendFlags = ulFlags;
        tlsFlag = pCellularSocketContext->ulFlags & CELLULAR_SOCKET_SECURE_FLAG;
    }

    if( retSentLength == SOCKETS_ERROR_NONE )
    {
        if( tlsFlag != 0U )
        {
            bytesSent = TLS_Send( pCellularSocketContext->pvTLSContext, pvBuffer, xDataLength );
        }
        else
        {
            bytesSent = prvNetworkSend( pCellularSocketContext, pvBuffer, xDataLength );
        }

        /* Check if gracefully shutdown. */
        if( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_WRITE_CLOSED_FLAG ) != 0U )
        {
            retSentLength = SOCKETS_ECLOSED;
        }
        else if( bytesSent < 0 )
        {
            if( tlsFlag != 0U )
            {
                IotLogError( "Error %d while TLS_Send data. %08x", bytesSent, pCellularSocketContext->ulFlags );
                retSentLength = SOCKETS_TLS_SEND_ERROR;
            }
            else
            {
                IotLogError( "Error %d while send data.", bytesSent );
                retSentLength = SOCKETS_SOCKET_ERROR;
            }
        }
        else
        {
            retSentLength = ( int32_t ) bytesSent;
        }
    }

    IotLogDebug( "(Network connection %p) Sent %d bytes.", pCellularSocketContext, retSentLength );
    return retSentLength;
}

/*-----------------------------------------------------------*/

/* Standard secure socket api implementation. */
/* coverity[misra_c_2012_rule_8_7_violation] */
int32_t SOCKETS_Shutdown( Socket_t xSocket,
                          uint32_t ulHow )
{
    int32_t retShutdown = SOCKETS_ERROR_NONE;
    _cellularSecureSocket_t * pCellularSocketContext = ( _cellularSecureSocket_t * ) xSocket;

    /* xSocket need to be check against SOCKET_INVALID_SOCKET. */
    /* coverity[misra_c_2012_rule_11_4_violation] */
    if( ( pCellularSocketContext == NULL ) || ( xSocket == SOCKETS_INVALID_SOCKET ) ||
        ( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_OPEN_FLAG ) == 0U ) )
    {
        retShutdown = SOCKETS_EINVAL;
    }

    if( retShutdown == SOCKETS_ERROR_NONE )
    {
        switch( ulHow )
        {
            case SOCKETS_SHUT_RD:
                pCellularSocketContext->ulFlags |= CELLULAR_SOCKET_READ_CLOSED_FLAG;
                break;

            case SOCKETS_SHUT_WR:
                pCellularSocketContext->ulFlags |= CELLULAR_SOCKET_WRITE_CLOSED_FLAG;
                break;

            case SOCKETS_SHUT_RDWR:
                pCellularSocketContext->ulFlags |= CELLULAR_SOCKET_READ_CLOSED_FLAG;
                pCellularSocketContext->ulFlags |= CELLULAR_SOCKET_WRITE_CLOSED_FLAG;
                break;

            default:
                retShutdown = SOCKETS_EINVAL;
                break;
        }
    }

    return retShutdown;
}

/*-----------------------------------------------------------*/

/* Standard secure socket api implementation. */
/* coverity[misra_c_2012_rule_8_7_violation] */
int32_t SOCKETS_Close( Socket_t xSocket )
{
    int32_t retClose = SOCKETS_ERROR_NONE;
    _cellularSecureSocket_t * pCellularSocketContext = ( _cellularSecureSocket_t * ) xSocket;
    CellularSocketHandle_t tcpSocket = NULL;

    /* xSocket need to be check against SOCKET_INVALID_SOCKET. */
    /* coverity[misra_c_2012_rule_11_4_violation] */
    if( ( pCellularSocketContext == NULL ) || ( xSocket == SOCKETS_INVALID_SOCKET ) )
    {
        IotLogError( "Invalid xSocket %p", pCellularSocketContext );
        retClose = SOCKETS_EINVAL;
    }
    else
    {
        tcpSocket = pCellularSocketContext->cellularSocketHandle;
    }

    if( retClose == SOCKETS_ERROR_NONE )
    {
        if( tcpSocket != NULL )
        {
            if( Cellular_SocketClose( CellularHandle, tcpSocket ) != CELLULAR_SUCCESS )
            {
                IotLogWarn( "Failed to destroy connection." );
                retClose = SOCKETS_SOCKET_ERROR;
            }

            ( void ) Cellular_SocketRegisterDataReadyCallback( CellularHandle, tcpSocket, NULL, NULL );
            ( void ) Cellular_SocketRegisterSocketOpenCallback( CellularHandle, tcpSocket, NULL, NULL );
            ( void ) Cellular_SocketRegisterClosedCallback( CellularHandle, tcpSocket, NULL, NULL );
            pCellularSocketContext->cellularSocketHandle = NULL;
        }

        if( pCellularSocketContext->pcDestination != NULL )
        {
            vPortFree( pCellularSocketContext->pcDestination );
            pCellularSocketContext->pcDestination = NULL;
        }

        if( pCellularSocketContext->pcServerCertificate != NULL )
        {
            vPortFree( pCellularSocketContext->pcServerCertificate );
            pCellularSocketContext->pcServerCertificate = NULL;
        }

        if( pCellularSocketContext->pvTLSContext != NULL )
        {
            TLS_Cleanup( pCellularSocketContext->pvTLSContext );
            pCellularSocketContext->pvTLSContext = NULL;
        }

        if( pCellularSocketContext->socketEventGroupHandle != NULL )
        {
            vEventGroupDelete( pCellularSocketContext->socketEventGroupHandle );
            pCellularSocketContext->socketEventGroupHandle = NULL;
        }

        vPortFree( pCellularSocketContext );
    }

    return retClose;
}

/*-----------------------------------------------------------*/

/* Standard secure socket api implementation. */
/* coverity[misra_c_2012_rule_8_7_violation] */
int32_t SOCKETS_SetSockOpt( Socket_t xSocket,
                            int32_t lLevel,
                            int32_t lOptionName,
                            const void * pvOptionValue,
                            size_t xOptionLength )
{
    TickType_t sockTimeout = 0;
    _cellularSecureSocket_t * pCellularSocketContext = ( _cellularSecureSocket_t * ) xSocket;
    int32_t retSetSockOpt = SOCKETS_ERROR_NONE;

    ( void ) lLevel;

    retSetSockOpt = prvCheckSetSockOptParams( xSocket, lOptionName, pvOptionValue, &sockTimeout );

    if( retSetSockOpt == SOCKETS_ERROR_NONE )
    {
        switch( lOptionName )
        {
            case SOCKETS_SO_SERVER_NAME_INDICATION:
                retSetSockOpt = prvSetupServerNameIndication( pCellularSocketContext, pvOptionValue, xOptionLength );
                break;

            case SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE:
                retSetSockOpt = prvSetupTrustedServerCertificate( pCellularSocketContext, pvOptionValue, xOptionLength );
                break;

            case SOCKETS_SO_REQUIRE_TLS:
                pCellularSocketContext->ulFlags |= CELLULAR_SOCKET_SECURE_FLAG;
                break;

            case SOCKETS_SO_SNDTIMEO:
                retSetSockOpt = prvSetupSocketSendTimeout( pCellularSocketContext, sockTimeout );
                break;

            case SOCKETS_SO_RCVTIMEO:
                retSetSockOpt = prvSetupSocketRecvTimeout( pCellularSocketContext, sockTimeout );
                break;

            case SOCKETS_SO_NONBLOCK:
                retSetSockOpt = prvSetupSocketNonblock( pCellularSocketContext );
                break;

            default:
                retSetSockOpt = SOCKETS_ENOPROTOOPT;
                break;
        }
    }

    return retSetSockOpt;
}

/*-----------------------------------------------------------*/

#if ( CELLULAR_SUPPORT_GETHOSTBYNAME == 0 )

/* Standard secure socket api implementation. */
/* coverity[misra_c_2012_rule_8_7_violation] */
    uint32_t SOCKETS_GetHostByName( const char * pcHostName )
    {
        uint32_t ulIPAddress = 0;

        if( pcHostName != NULL )
        {
            ulIPAddress = prvLookup( pcHostName );
        }

        return ulIPAddress;
    }

/*-----------------------------------------------------------*/

#else /* if ( CELLULAR_SUPPORT_GETHOSTBYNAME == 0 ) */

/* Standard secure socket api implementation. */
/* coverity[misra_c_2012_rule_8_7_violation] */
    uint32_t SOCKETS_GetHostByName( const char * pcHostName )
    {
        uint32_t ulIPAddress = 0;
        char pIpAddress[ CELLULAR_IP_ADDRESS_MAX_SIZE ] = { 0 };
        CellularError_t socketStatus = CELLULAR_SUCCESS;

        if( pcHostName != NULL )
        {
            socketStatus = Cellular_GetHostByName( CellularHandle,
                                                   CELLULAR_PDN_CONTEXT_ID_SOCKETS, pcHostName, pIpAddress );

            if( socketStatus == CELLULAR_SUCCESS )
            {
                /* Convert the IP string to uIPAddress. */
                if( prvSocketsAton( pIpAddress, &ulIPAddress ) == 0 )
                {
                    ulIPAddress = 0;
                }
            }
            else
            {
                ulIPAddress = 0;
            }
        }

        return ulIPAddress;
    }
#endif /* if ( CELLULAR_SUPPORT_GETHOSTBYNAME == 0 ) */

/*-----------------------------------------------------------*/

/* Standard secure socket api implementation. */
/* coverity[misra_c_2012_rule_8_7_violation] */
BaseType_t SOCKETS_Init( void )
{
    /* Empty initialization for this port. */
    return pdPASS;
}

/*-----------------------------------------------------------*/
