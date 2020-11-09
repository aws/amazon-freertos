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

#include "iot_config.h"
#include "aws_cellular_config.h"
#include "cellular_config_defaults.h"

/* Standard includes. */
#include <stdlib.h>
#include <string.h>

#include "cellular_types.h"
#include "cellular_common.h"
#include "cellular_common_api.h"
#include "cellular_common_portable.h"
#include "limits.h"

/* Cellular module includes. */
#include "cellular_hl7802.h"

/*-----------------------------------------------------------*/

static void _cellular_UrcProcessKtcpInd( CellularContext_t * pContext,
                                         char * pInputLine );
static void handleTcpNotif( CellularSocketContext_t * pSocketData,
                            uint8_t tcpNotif );
static void _cellular_UrcProcessKtcpNotif( CellularContext_t * pContext,
                                           char * pInputLine );
static void _cellular_UrcProcessKtcpData( CellularContext_t * pContext,
                                          char * pInputLine );

/*-----------------------------------------------------------*/

/* Try to Keep this map in Alphabetical order. */
/* Cellular HAL common porting interface. */
/* coverity[misra_c_2012_rule_8_7_violation] */
CellularAtParseTokenMap_t CellularUrcHandlerTable[] =
{
    { "CEREG",      Cellular_CommonUrcProcessCereg },
    { "CREG",       Cellular_CommonUrcProcessCreg  },
    { "KTCP_DATA",  _cellular_UrcProcessKtcpData   },         /* TCP data URC. */
    { "KTCP_IND",   _cellular_UrcProcessKtcpInd    },         /* TCP status URC. */
    { "KTCP_NOTIF", _cellular_UrcProcessKtcpNotif  }          /* TCP connection failure. */
};

/* Cellular HAL common porting interface. */
/* coverity[misra_c_2012_rule_8_7_violation] */
uint32_t CellularUrcHandlerTableSize = sizeof( CellularUrcHandlerTable ) / sizeof( CellularAtParseTokenMap_t );

/*-----------------------------------------------------------*/

static void _cellular_UrcProcessKtcpInd( CellularContext_t * pContext,
                                         char * pInputLine )
{
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    char * pLocalInputLine = pInputLine;
    char * pToken = NULL;
    CellularSocketContext_t * pSocketData = NULL;
    uint8_t sessionId = 0;
    uint32_t socketIndex = 0;
    int32_t tempValue = 0;

    if( ( pContext != NULL ) && ( pInputLine != NULL ) )
    {
        /* The inputline is in this format +KTCP_IND: <session_id>, 1
         * This URC indicate connection success. */
        atCoreStatus = Cellular_ATGetNextTok( &pLocalInputLine, &pToken );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

            if( atCoreStatus == CELLULAR_AT_SUCCESS )
            {
                if( ( tempValue >= MIN_TCP_SESSION_ID ) && ( tempValue <= MAX_TCP_SESSION_ID ) )
                {
                    sessionId = ( uint8_t ) tempValue;
                    socketIndex = _Cellular_GetSocketId( pContext, sessionId );
                }
                else
                {
                    IotLogError( "error parsing _cellular_UrcProcessKtcpInd session ID" );
                    atCoreStatus = CELLULAR_AT_ERROR;
                }
            }
        }

        /* Call the callback function of this session. */
        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            pSocketData = _Cellular_GetSocketData( pContext, socketIndex );

            if( pSocketData == NULL )
            {
                IotLogError( "_cellular_UrcProcessKtcpInd : invalid socket index %d", socketIndex );
            }
            else if( pSocketData->pModemData != ( void * ) ( ( uint32_t ) sessionId ) )
            {
                IotLogError( "_cellular_UrcProcessKtcpInd : session not match %d socket index %d",
                             ( uint32_t ) pSocketData->pModemData, socketIndex );
            }
            else if( pSocketData->openCallback == NULL )
            {
                IotLogDebug( "_cellular_UrcProcessKtcpInd : Open callback not set!!" );
            }
            else
            {
                IotLogDebug( "Notify session %d with socket opened\r\n", sessionId );
                pSocketData->socketState = SOCKETSTATE_CONNECTED;
                pSocketData->openCallback( CELLULAR_URC_SOCKET_OPENED,
                                           pSocketData, pSocketData->pOpenCallbackContext );
            }
        }
    }
}

/*-----------------------------------------------------------*/

static void handleTcpNotif( CellularSocketContext_t * pSocketData,
                            uint8_t tcpNotif )
{
    switch( tcpNotif )
    {
        case TCP_NOTIF_TCP_DISCONNECTION: /* TCP disconnection by the server or remote client. */

            pSocketData->socketState = SOCKETSTATE_DISCONNECTED;

            if( pSocketData->closedCallback != NULL )
            {
                IotLogDebug( "Notify session %d with socket disconnected\r\n", pSocketData->pModemData );
                pSocketData->closedCallback( pSocketData, pSocketData->pClosedCallbackContext );
            }

            break;

        case TCP_NOTIF_TCP_CONNECTION_ERROR: /* TCP connection error. */

            pSocketData->socketState = SOCKETSTATE_DISCONNECTED;

            if( pSocketData->openCallback != NULL )
            {
                IotLogDebug( "Notify session %d with socket open failed\r\n", pSocketData->pModemData );
                pSocketData->openCallback( CELLULAR_URC_SOCKET_OPEN_FAILED,
                                           pSocketData, pSocketData->pOpenCallbackContext );
            }

            break;

        default:
            break;
    }
}

/*-----------------------------------------------------------*/

static void _cellular_UrcProcessKtcpNotif( CellularContext_t * pContext,
                                           char * pInputLine )
{
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    char * pLocalInputLine = pInputLine;
    char * pToken = NULL;
    CellularSocketContext_t * pSocketData = NULL;
    uint8_t sessionId = 0;
    tcpConnectionFailure_t tcpNotif = TCP_NOTIF_OK;
    uint32_t socketIndex = 0;
    int32_t tempValue = 0;

    if( ( pContext != NULL ) && ( pInputLine != NULL ) )
    {
        /* The inputline is in this format +KTCP_NOTIF: <session_id>, <tcp_notif>
         * This URC indicate connection problem. */

        /* Remove leading space. */
        atCoreStatus = Cellular_ATRemoveLeadingWhiteSpaces( &pLocalInputLine );

        /* Parse the session ID. */
        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATGetNextTok( &pLocalInputLine, &pToken );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

            if( atCoreStatus == CELLULAR_AT_SUCCESS )
            {
                if( ( tempValue >= MIN_TCP_SESSION_ID ) && ( tempValue <= MAX_TCP_SESSION_ID ) )
                {
                    sessionId = ( uint8_t ) tempValue;
                    socketIndex = _Cellular_GetSocketId( pContext, sessionId );
                }
                else
                {
                    IotLogError( "error parsing _cellular_UrcProcessKtcpInd session ID" );
                    atCoreStatus = CELLULAR_AT_ERROR;
                }
            }
        }

        /* Parse the tcp notif. */
        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATGetNextTok( &pLocalInputLine, &pToken );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

            if( atCoreStatus == CELLULAR_AT_SUCCESS )
            {
                if( ( tempValue >= TCP_NOTIF_OK ) && ( tempValue <= TCP_NOTIF_MAX ) )
                {
                    tcpNotif = ( tcpConnectionFailure_t ) tempValue;
                }
                else
                {
                    IotLogError( "error parsing _cellular_UrcProcessKtcpInd session ID" );
                    atCoreStatus = CELLULAR_AT_ERROR;
                }
            }
        }

        /* Call the callback function of this session. */
        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            pSocketData = _Cellular_GetSocketData( pContext, socketIndex );

            if( pSocketData == NULL )
            {
                IotLogError( "_cellular_UrcProcessKtcpNotif : invalid socket index %d", socketIndex );
            }
            else if( pSocketData->pModemData != ( void * ) ( ( uint32_t ) sessionId ) )
            {
                IotLogError( "_cellular_UrcProcessKtcpNotif : session not match %d socket index %d",
                             ( uint32_t ) pSocketData->pModemData, socketIndex );
            }
            else
            {
                handleTcpNotif( pSocketData, tcpNotif );
            }
        }
    }
}

/*-----------------------------------------------------------*/

static void _cellular_UrcProcessKtcpData( CellularContext_t * pContext,
                                          char * pInputLine )
{
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    char * pLocalInputLine = pInputLine;
    char * pToken = NULL;
    CellularSocketContext_t * pSocketData = NULL;
    uint8_t sessionId = 0;
    uint32_t socketIndex = 0;
    int32_t tempValue = 0;

    if( ( pContext != NULL ) && ( pInputLine != NULL ) )
    {
        /* The inputline is in this format +KTCP_DATA: <session_id>, <bytes_received>
         * This URC indicate connection problem. */

        /* parse the session ID. */
        atCoreStatus = Cellular_ATGetNextTok( &pLocalInputLine, &pToken );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

            if( atCoreStatus == CELLULAR_AT_SUCCESS )
            {
                if( ( tempValue >= MIN_TCP_SESSION_ID ) && ( tempValue <= MAX_TCP_SESSION_ID ) )
                {
                    sessionId = ( uint8_t ) tempValue;
                    socketIndex = _Cellular_GetSocketId( pContext, sessionId );
                }
                else
                {
                    IotLogError( "error parsing _cellular_UrcProcessKtcpData session ID" );
                    atCoreStatus = CELLULAR_AT_ERROR;
                }
            }
        }

        /* Indicate the upper layer about the data reception. */
        /* Call the callback function of this session. */
        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            pSocketData = _Cellular_GetSocketData( pContext, socketIndex );

            if( pSocketData == NULL )
            {
                IotLogError( "_cellular_UrcProcessKtcpData : invalid socket index %d", socketIndex );
            }
            else if( pSocketData->pModemData != ( void * ) ( ( uint32_t ) sessionId ) )
            {
                IotLogError( "_cellular_UrcProcessKtcpData : session not match %d socket index %d",
                             ( uint32_t ) pSocketData->pModemData, socketIndex );
            }
            else if( pSocketData->dataReadyCallback == NULL )
            {
                IotLogDebug( "_cellular_UrcProcessKtcpData : Data ready callback not set!!" );
            }
            else
            {
                pSocketData->dataReadyCallback( pSocketData, pSocketData->pDataReadyCallbackContext );
            }
        }
    }
}

/*-----------------------------------------------------------*/
