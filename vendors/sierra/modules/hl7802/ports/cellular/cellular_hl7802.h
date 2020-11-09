/*
 * Amazon FreeRTOS CELLULAR Preview Release
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

#ifndef __CELLULAR_HL7802_H__
#define __CELLULAR_HL7802_H__

/* Configure logs for HL7802 functions. */
#ifdef IOT_LOG_LEVEL_CELLULAR
    #define LIBRARY_LOG_LEVEL        IOT_LOG_LEVEL_CELLULAR
#else
    #ifdef IOT_LOG_LEVEL_GLOBAL
        #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
    #else
        #define LIBRARY_LOG_LEVEL    IOT_LOG_INFO
    #endif
#endif

#define LIBRARY_LOG_NAME    ( "CELLULAR_HL7802" )
#include "iot_logging_setup.h"

/*-----------------------------------------------------------*/

#define MIN_TCP_SESSION_ID          ( 1U )
#define MAX_TCP_SESSION_ID          ( 6U )
#define TCP_SESSION_TABLE_LEGNTH    ( MAX_TCP_SESSION_ID + 1 )

#define INVALID_SOCKET_INDEX        ( UINT32_MAX )

/*-----------------------------------------------------------*/

typedef struct cellularModuleContext
{
    uint32_t placeholder;
    uint32_t pSessionMap[ TCP_SESSION_TABLE_LEGNTH ];
} cellularModuleContext_t;

typedef enum tcpSocketState
{
    TCP_SOCKET_STATE_NOT_DEFINED = 0,
    TCP_SOCKET_STATE_DEFINED_BUT_NOT_USED = 1,
    TCP_SOCKET_STATE_OPENING_AND_CONNECTING = 2,
    TCP_SOCKET_STATE_CONNECTION_UP = 3,
    TCP_SOCKET_STATE_CONNECTION_CLOSING = 4,
    TCP_SOCKET_STATE_CLOSED = 5,
    TCP_SOCKET_STATE_MAX
} tcpSocketState_t;

typedef enum tcpConnectionFailure
{
    TCP_NOTIF_OK = -1,
    TCP_NOTIF_NETWORK_ERROR = 0,
    TCP_NOTIF_NO_MORE_SOCKETS_AVAILABLE = 1,
    TCP_NOTIF_MEMORY_PROBLEM = 2,
    TCP_NOTIF_DNS_ERROR = 3,
    TCP_NOTIF_TCP_DISCONNECTION = 4,
    TCP_NOTIF_TCP_CONNECTION_ERROR = 5,
    TCP_NOTIF_GENERIC_ERROR = 6,
    TCP_NOTIF_FAIL_TO_ACCEPT_CLIENT_REQUEST = 7,
    TCP_NOTIF_KTCPSND_WAITING_FOR_MORE_OR_LESS_CHARACTERS = 8,
    TCP_NOTIF_BAD_SESSION_ID = 9,
    TCP_NOTIF_SESSION_IS_ALREADY_RUNNING = 10,
    TCP_NOTIF_ALL_SESSIONS_ARE_USED = 11,
    TCP_NOTIF_SOCKET_CONNECTION_TIMEOUT_ERROR = 12,
    TCP_NOTIF_SSL_CONNECTION_ERROR = 13,
    TCP_NOTIF_SSL_INITIALIZATION_ERROR = 14,
    TCP_NOTIF_MAX,
} tcpConnectionFailure_t;

/*-----------------------------------------------------------*/

uint32_t _Cellular_GetSocketId( CellularContext_t * pContext,
                                uint8_t sessionId );

/*-----------------------------------------------------------*/

#endif /* ifndef __CELLULAR_HL7802_H__ */
