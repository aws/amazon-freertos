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

/**
 * @brief cellular HAL APIs implementation without AT commands.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdio.h>
#include <string.h>

#include "aws_cellular_config.h"
#include "cellular_config_defaults.h"
#include "cellular_types.h"
#include "cellular_api.h"
#include "cellular_internal.h"
#include "cellular_common_internal.h"
#include "cellular_pkthandler_internal.h"
#include "cellular_pktio_internal.h"
#include "cellular_common_portable.h"
#include "cellular_common_api.h"

/*-----------------------------------------------------------*/

static CellularError_t _socketSetSockOptLevelTransport( CellularSocketOption_t option,
                                                        CellularSocketHandle_t socketHandle,
                                                        const uint8_t * pOptionValue,
                                                        uint32_t optionValueLength );

/*-----------------------------------------------------------*/

/* Internal function of Cellular_SocketSetSockOpt to reduce complexity. */
static CellularError_t _socketSetSockOptLevelTransport( CellularSocketOption_t option,
                                                        CellularSocketHandle_t socketHandle,
                                                        const uint8_t * pOptionValue,
                                                        uint32_t optionValueLength )
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    const uint32_t * pTimeoutMs = NULL;

    if( option == CELLULAR_SOCKET_OPTION_SEND_TIMEOUT )
    {
        if( optionValueLength == sizeof( uint32_t ) )
        {
            /* The variable length is checked. */
            /* coverity[misra_c_2012_rule_11_3_violation] */
            pTimeoutMs = ( const uint32_t * ) pOptionValue;
            socketHandle->sendTimeoutMs = *pTimeoutMs;
        }
        else
        {
            cellularStatus = CELLULAR_INTERNAL_FAILURE;
        }
    }
    else if( option == CELLULAR_SOCKET_OPTION_RECV_TIMEOUT )
    {
        if( optionValueLength == sizeof( uint32_t ) )
        {
            /* The variable length is checked. */
            /* coverity[misra_c_2012_rule_11_3_violation] */
            pTimeoutMs = ( const uint32_t * ) pOptionValue;
            socketHandle->recvTimeoutMs = *pTimeoutMs;
        }
        else
        {
            cellularStatus = CELLULAR_INTERNAL_FAILURE;
        }
    }
    else if( option == CELLULAR_SOCKET_OPTION_PDN_CONTEXT_ID )
    {
        if( socketHandle->socketState == SOCKETSTATE_ALLOCATED )
        {
            socketHandle->contextId = *pOptionValue;
        }
        else
        {
            IotLogError( "Cellular_SocketSetSockOpt: Cannot change the contextID in this state %d",
                         socketHandle->socketState );
            cellularStatus = CELLULAR_INTERNAL_FAILURE;
        }
    }
    else
    {
        IotLogError( "Cellular_SocketSetSockOpt: Option not supported" );
        cellularStatus = CELLULAR_UNSUPPORTED;
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonInit( CellularHandle_t * pCellularHandle,
                                     const CellularCommInterface_t * pCommInterface )
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularContext_t * pContext = NULL;
    CellularTokenTable_t tokenTable =
    {
        .pCellularUrcHandlerTable              = CellularUrcHandlerTable,
        .cellularPrefixToParserMapSize         = CellularUrcHandlerTableSize,
        .pCellularSrcTokenErrorTable           = CellularSrcTokenErrorTable,
        .cellularSrcTokenErrorTableSize        = CellularSrcTokenErrorTableSize,
        .pCellularSrcTokenSuccessTable         = CellularSrcTokenSuccessTable,
        .cellularSrcTokenSuccessTableSize      = CellularSrcTokenSuccessTableSize,
        .pCellularUrcTokenWoPrefixTable        = CellularUrcTokenWoPrefixTable,
        .cellularUrcTokenWoPrefixTableSize     = CellularUrcTokenWoPrefixTableSize,
        .pCellularSrcExtraTokenSuccessTable    = NULL,
        .cellularSrcExtraTokenSuccessTableSize = 0
    };

    /* Init the Cellular HAL common. */
    cellularStatus = _Cellular_LibInit( pCellularHandle, pCommInterface, &tokenTable );

    /* Init the module. */
    if( cellularStatus == CELLULAR_SUCCESS )
    {
        pContext = *pCellularHandle;
        cellularStatus = Cellular_ModuleInit( pContext, &pContext->pModueContext );
    }

    /* Setup UE, URC and query register status. */
    if( cellularStatus == CELLULAR_SUCCESS )
    {
        cellularStatus = Cellular_ModuleEnableUE( pContext );
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        cellularStatus = Cellular_ModuleEnableUrc( pContext );
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL prototype. */
/* coverity[misra_c_2012_rule_8_13_violation] */
CellularError_t Cellular_CommonCleanup( CellularHandle_t cellularHandle )
{
    /* Functions called this function modify the pContext data. */
    /* coverity[misra_c_2012_rule_8_13_violation] */
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    if( pContext == NULL )
    {
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else
    {
        ( void ) Cellular_ModuleCleanUp( pContext );
        ( void ) _Cellular_LibCleanup( cellularHandle );
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonRegisterUrcNetworkRegistrationEventCallback( CellularHandle_t cellularHandle,
                                                                            CellularUrcNetworkRegistrationCallback_t networkRegistrationCallback,
                                                                            void * pCallbackContext )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else
    {
        pContext->cbEvents.networkRegistrationCallback = networkRegistrationCallback;
        pContext->cbEvents.pNetworkRegistrationCallbackContext = pCallbackContext;
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonRegisterUrcPdnEventCallback( CellularHandle_t cellularHandle,
                                                            CellularUrcPdnEventCallback_t pdnEventCallback,
                                                            void * pCallbackContext )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else
    {
        pContext->cbEvents.pdnEventCallback = pdnEventCallback;
        pContext->cbEvents.pPdnEventCallbackContext = pCallbackContext;
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* This function is provided as common code to cellular module porting.
 * Vendor may choose to use this function or use their implementation. */
/* coverity[misra_c_2012_rule_8_7_violation]. */
CellularError_t Cellular_CommonRegisterUrcSignalStrengthChangedCallback( CellularHandle_t cellularHandle,
                                                                         CellularUrcSignalStrengthChangedCallback_t signalStrengthChangedCallback,
                                                                         void * pCallbackContext )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else
    {
        pContext->cbEvents.signalStrengthChangedCallback = signalStrengthChangedCallback;
        pContext->cbEvents.pSignalStrengthChangedCallbackContext = pCallbackContext;
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonRegisterUrcGenericCallback( CellularHandle_t cellularHandle,
                                                           CellularUrcGenericCallback_t genericCallback,
                                                           void * pCallbackContext )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else
    {
        pContext->cbEvents.genericCallback = genericCallback;
        pContext->cbEvents.pGenericCallbackContext = pCallbackContext;
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonRegisterModemEventCallback( CellularHandle_t cellularHandle,
                                                           CellularModemEventCallback_t modemEventCallback,
                                                           void * pCallbackContext )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else
    {
        pContext->cbEvents.modemEventCallback = modemEventCallback;
        pContext->cbEvents.pModemEventCallbackContext = pCallbackContext;
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonATCommandRaw( CellularHandle_t cellularHandle,
                                             const char * pATCommandPrefix,
                                             const char * pATCommandPayload,
                                             CellularATCommandType_t atCommandType,
                                             CellularATCommandResponseReceivedCallback_t responseReceivedCallback,
                                             void * pData,
                                             uint16_t dataLen )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularAtReq_t atReqGetResult = { 0 };

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else if( pATCommandPayload == NULL )
    {
        IotLogError( "Cellular_ATCommandRaw: Input parameter is NULL" );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        atReqGetResult.atCmdType = atCommandType;
        atReqGetResult.pAtRspPrefix = ( const char * ) pATCommandPrefix;
        atReqGetResult.pAtCmd = ( const char * ) pATCommandPayload;
        atReqGetResult.pData = pData;
        atReqGetResult.dataLen = dataLen;
        atReqGetResult.respCallback = responseReceivedCallback;

        pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqGetResult );
        cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonCreateSocket( CellularHandle_t cellularHandle,
                                             uint8_t pdnContextId,
                                             CellularSocketDomain_t socketDomain,
                                             CellularSocketType_t socketType,
                                             CellularSocketProtocol_t socketProtocol,
                                             CellularSocketHandle_t * pSocketHandle )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else if( pSocketHandle == NULL )
    {
        IotLogError( "pSocketHandle is NULL" );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else if( _Cellular_IsValidPdn( pdnContextId ) != CELLULAR_SUCCESS )
    {
        IotLogError( "_Cellular_IsValidPdn failed" );
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else
    {
        cellularStatus = _Cellular_CreateSocketData( pContext, pdnContextId,
                                                     socketDomain, socketType, socketProtocol, pSocketHandle );
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonSocketSetSockOpt( CellularHandle_t cellularHandle,
                                                 CellularSocketHandle_t socketHandle,
                                                 CellularSocketOptionLevel_t optionLevel,
                                                 CellularSocketOption_t option,
                                                 const uint8_t * pOptionValue,
                                                 uint32_t optionValueLength )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else if( socketHandle == NULL )
    {
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else if( ( pOptionValue == NULL ) || ( optionValueLength == 0U ) )
    {
        IotLogError( "Cellular_SocketSetSockOpt: Invalid parameter" );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        if( optionLevel == CELLULAR_SOCKET_OPTION_LEVEL_IP )
        {
            if( option == CELLULAR_SOCKET_OPTION_MAX_IP_PACKET_SIZE )
            {
                /*TBD */
            }
            else
            {
                IotLogError( "Cellular_SocketSetSockOpt: Option not supported" );
                cellularStatus = CELLULAR_UNSUPPORTED;
            }
        }
        else /* optionLevel CELLULAR_SOCKET_OPTION_LEVEL_TRANSPORT. */
        {
            cellularStatus = _socketSetSockOptLevelTransport( option, socketHandle, pOptionValue, optionValueLength );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonSocketRegisterDataReadyCallback( CellularHandle_t cellularHandle,
                                                                CellularSocketHandle_t socketHandle,
                                                                CellularSocketDataReadyCallback_t dataReadyCallback,
                                                                void * pCallbackContext )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else if( socketHandle == NULL )
    {
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else
    {
        socketHandle->dataReadyCallback = dataReadyCallback;
        socketHandle->pDataReadyCallbackContext = pCallbackContext;
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonSocketRegisterSocketOpenCallback( CellularHandle_t cellularHandle,
                                                                 CellularSocketHandle_t socketHandle,
                                                                 CellularSocketOpenCallback_t socketOpenCallback,
                                                                 void * pCallbackContext )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else if( socketHandle == NULL )
    {
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else
    {
        socketHandle->openCallback = socketOpenCallback;
        socketHandle->pOpenCallbackContext = pCallbackContext;
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonSocketRegisterClosedCallback( CellularHandle_t cellularHandle,
                                                             CellularSocketHandle_t socketHandle,
                                                             CellularSocketClosedCallback_t closedCallback,
                                                             void * pCallbackContext )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else if( socketHandle == NULL )
    {
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else
    {
        socketHandle->closedCallback = closedCallback;
        socketHandle->pClosedCallbackContext = pCallbackContext;
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/
