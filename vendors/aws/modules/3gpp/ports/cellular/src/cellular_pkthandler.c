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
 * @brief cellular HAL common packet handler functions to dispatch packet.
 */

#include "iot_config.h"
#include "aws_cellular_config.h"
#include "cellular_config_defaults.h"

/* Standard includes. */
#include <stdlib.h>
#include <string.h>

#include "platform/iot_threads.h"

#include "queue.h"
#include "cellular_internal.h"
#include "cellular_pkthandler_internal.h"
#include "cellular_pktio_internal.h"
#include "cellular_types.h"
#include "cellular_common_internal.h"

/*-----------------------------------------------------------*/

#define MAX( a, b )    ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )

/* Windows simulator implementation. */
#if defined( _WIN32 ) || defined( _WIN64 )
    #define strtok_r             strtok_s
#endif

#define SORT_MODULE_TOKEN_MAP    ( 0U )

/*-----------------------------------------------------------*/

static void _convertAndQueueRespPacket( CellularContext_t * pContext,
                                        const void * pBuf );
static CellularPktStatus_t urcParseToken( CellularContext_t * pContext,
                                          char * pInputLine );
static void _processUrcPacket( CellularContext_t * pContext,
                               const char * pBuf );
static CellularPktStatus_t _Cellular_TimeoutAtcmdRequestWithCallbackRaw( CellularContext_t * pContext,
                                                                         CellularAtReq_t atReq,
                                                                         uint32_t timeoutMS );
static CellularPktStatus_t _Cellular_DataSendWithTimeoutDelayRaw( CellularContext_t * pContext,
                                                                  CellularAtDataReq_t dataReq,
                                                                  uint32_t timeoutMs,
                                                                  uint32_t interDelayMS );
static void _Cellular_PktHandlerAcquirePktRequestMutex( CellularContext_t * pContext );
static void _Cellular_PktHandlerReleasePktRequestMutex( CellularContext_t * pContext );
static int _searchCompareFunc( const void * pInputToken,
                               const void * pBase );
static int _sortCompareFunc( const void * pElem1Ptr,
                             const void * pElem2Ptr );
static void _Cellular_ProcessGenericUrc( const CellularContext_t * pContext,
                                         const char * pInputLine );
static CellularPktStatus_t _atParseGetHandler( CellularContext_t * pContext,
                                               char * pTokenPtr,
                                               char * pSavePtr );

/*-----------------------------------------------------------*/

static void _convertAndQueueRespPacket( CellularContext_t * pContext,
                                        const void * pBuf )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    const CellularATCommandResponse_t * pAtResp = NULL;

    if( ( pContext != NULL ) && ( pBuf != NULL ) )
    {
        pAtResp = ( const CellularATCommandResponse_t * ) pBuf;
        IotMutex_Lock( &pContext->PktRespMutex );

        if( pAtResp->status == false )
        {
            IotLogError( "_convertAndQueueRespPacket: AT response contains error" );
            pktStatus = CELLULAR_PKT_STATUS_FAILURE;
        }

        if( ( pContext->pktRespCB != NULL ) && ( pktStatus == CELLULAR_PKT_STATUS_OK ) )
        {
            pktStatus = pContext->pktRespCB( pContext,
                                             ( const CellularATCommandResponse_t * ) pBuf,
                                             pContext->pPktUsrData,
                                             pContext->PktUsrDataLen );
        }

        /* Notify calling thread, Not blocking immediately comes back if the queue is full. */
        if( xQueueSend( pContext->pktRespQueue, ( void * ) &pktStatus, ( TickType_t ) 0 ) != pdPASS )
        {
            IotLogError( "_convertAndQueueRespPacket: Got a response when the Resp Q is full!!" );
        }

        IotMutex_Unlock( &pContext->PktRespMutex );
    }
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t urcParseToken( CellularContext_t * pContext,
                                          char * pInputLine )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    /* pInputLine = "+" pTokenPtr + ":" + pSavePtr.
     * if string not start with "+", then pTokenPtr = pSavePtr = pInputPtr. */
    char * pSavePtr = pInputLine, * pTokenPtr = pInputLine;

    if( pContext == NULL )
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if( pInputLine == NULL )
    {
        pktStatus = CELLULAR_PKT_STATUS_BAD_REQUEST;
    }
    else
    {
        IotLogDebug( "Next URC token to parse [%s]", pInputLine );

        /* First check for + at the beginning and advance to point to the next
         * byte. Use that string to pass to strtok and retrieve the token. Once the
         * token use is retrieved, get the function handler map and call that
         * function. */
        if( *pSavePtr == '+' )
        {
            pSavePtr++;
            pTokenPtr = strtok_r( pSavePtr, ":", &pSavePtr );

            if( pTokenPtr == NULL )
            {
                IotLogError( "_Cellular_AtParse : input string error, start with \"+\" but no token %s", pInputLine );
                pktStatus = CELLULAR_PKT_STATUS_BAD_REQUEST;
            }
        }
    }

    if( pktStatus == CELLULAR_PKT_STATUS_OK )
    {
        /* Now get the handler function based on the token. */
        pktStatus = _atParseGetHandler( pContext, pTokenPtr, pSavePtr );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/**
 * @brief copy the URC log in the buffer to a heap memory and process it.
 */
static void _processUrcPacket( CellularContext_t * pContext,
                               const char * pBuf )
{
    char * payload = NULL;

    if( pBuf != NULL )
    {
        ( void ) Cellular_ATStrDup( &payload, pBuf );

        if( payload != NULL )
        {
            /* The payload is null terminated. */
            ( void ) urcParseToken( pContext, ( char * ) payload );
            vPortFree( payload );
        }
        else
        {
            IotLogWarn( "Couldn't Allocate memory of %d for urc", strlen( pBuf ) );
        }
    }
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _Cellular_TimeoutAtcmdRequestWithCallbackRaw( CellularContext_t * pContext,
                                                                         CellularAtReq_t atReq,
                                                                         uint32_t timeoutMS )
{
    CellularPktStatus_t respCode = CELLULAR_PKT_STATUS_OK;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    BaseType_t qRet = pdFALSE;

    if( pContext == NULL )
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if( atReq.pAtCmd == NULL )
    {
        IotLogError( "PKT_STATUS_BAD_REQUEST, null AT param" );
        pktStatus = CELLULAR_PKT_STATUS_BAD_REQUEST;
    }
    else
    {
        /* Fill in request info structure. */
        pContext->pktRespCB = atReq.respCallback;
        IotLogDebug( ">>>>>Start sending [%s]<<<<<", atReq.pAtCmd );
        pContext->pPktUsrData = atReq.pData;
        pContext->PktUsrDataLen = ( uint16_t ) atReq.dataLen;
        pContext->pCurrentCmd = atReq.pAtCmd;
        pktStatus = _Cellular_PktioSendAtCmd( pContext, atReq.pAtCmd, atReq.atCmdType, atReq.pAtRspPrefix );

        if( pktStatus != CELLULAR_PKT_STATUS_OK )
        {
            IotLogError( "Can't send req packet" );
        }
        else
        {
            /* Wait for a response. */
            qRet = xQueueReceive( pContext->pktRespQueue, &respCode, pdMS_TO_TICKS( timeoutMS ) );

            if( qRet == pdTRUE )
            {
                pktStatus = ( CellularPktStatus_t ) respCode;

                if( pktStatus != CELLULAR_PKT_STATUS_OK )
                {
                    IotLogError( "pkt_recv status=%d, error in AT cmd %s resp", pktStatus, atReq.pAtCmd );
                } /* Ignore errors from callbacks as they will be handled elsewhere. */
            }
            else
            {
                pktStatus = CELLULAR_PKT_STATUS_TIMED_OUT;
                IotLogError( "pkt_recv status=%d, AT cmd %s timed out", pktStatus, atReq.pAtCmd );
            }
        }

        /* No command is waiting response. */
        pContext->PktioAtCmdType = CELLULAR_AT_NO_COMMAND;
        pContext->pktRespCB = NULL;
        pContext->pCurrentCmd = NULL;
        IotLogDebug( "<<<<<Exit sending [%s] status[%d]<<<<<", atReq.pAtCmd, pktStatus );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _Cellular_DataSendWithTimeoutDelayRaw( CellularContext_t * pContext,
                                                                  CellularAtDataReq_t dataReq,
                                                                  uint32_t timeoutMs,
                                                                  uint32_t interDelayMS )
{
    CellularPktStatus_t respCode = CELLULAR_PKT_STATUS_OK;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    BaseType_t qStatus = pdFALSE;
    uint32_t sendEndPatternLen = 0U;

    /* Check the pContext. */
    if( pContext == NULL )
    {
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if( ( dataReq.pData == NULL ) || ( dataReq.pSentDataLength == NULL ) )
    {
        IotLogError( "_Cellular_DataSendWithTimeoutDelayRaw, null input" );
        pktStatus = CELLULAR_PKT_STATUS_BAD_REQUEST;
    }
    else
    {
        IotLogDebug( ">>>>>Start sending Data <<<<<" );
        pContext->PktioAtCmdType = CELLULAR_AT_NO_RESULT;
        /* Send the packet. */
        *dataReq.pSentDataLength = _Cellular_PktioSendData( pContext, dataReq.pData, dataReq.dataLen );

        if( *dataReq.pSentDataLength != dataReq.dataLen )
        {
            IotLogError( "_Cellular_DataSendWithTimeoutDelayRaw, incomplete data transfer" );
            pktStatus = CELLULAR_PKT_STATUS_SEND_ERROR;
        }
    }

    /* Some driver required wait for a minimum of delay before sending data. */
    vTaskDelay( pdMS_TO_TICKS( interDelayMS ) );

    /* End pattern for specific modem. */
    if( ( pktStatus == CELLULAR_PKT_STATUS_OK ) && ( dataReq.pEndPattern != NULL ) )
    {
        sendEndPatternLen = _Cellular_PktioSendData( pContext, dataReq.pEndPattern, dataReq.endPatternLen );

        if( sendEndPatternLen != dataReq.endPatternLen )
        {
            IotLogError( "_Cellular_DataSendWithTimeoutDelayRaw, incomplete endpattern transfer" );
            pktStatus = CELLULAR_PKT_STATUS_SEND_ERROR;
        }
    }

    /* Wait for a response. */
    if( pktStatus == CELLULAR_PKT_STATUS_OK )
    {
        qStatus = xQueueReceive( pContext->pktRespQueue, &respCode, pdMS_TO_TICKS( timeoutMs ) );

        if( qStatus == pdTRUE )
        {
            pktStatus = ( CellularPktStatus_t ) respCode;

            if( pktStatus == CELLULAR_PKT_STATUS_OK )
            {
                IotLogDebug( "Data Sent successfully!" );
            }
            else
            {
                IotLogError( "pkt_recv status=%d, error in sending data", pktStatus );
            }
        }
        else
        {
            pktStatus = CELLULAR_PKT_STATUS_TIMED_OUT;
            IotLogError( "pkt_recv status=%d, data sending timed out", pktStatus );
        }

        pContext->PktioAtCmdType = CELLULAR_AT_NO_COMMAND;
        IotLogDebug( "<<<<<Exit sending data ret[%d]>>>>>", pktStatus );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

static void _Cellular_PktHandlerAcquirePktRequestMutex( CellularContext_t * pContext )
{
    if( pContext != NULL )
    {
        IotMutex_Lock( &pContext->pktRequestMutex );
    }
}

/*-----------------------------------------------------------*/

static void _Cellular_PktHandlerReleasePktRequestMutex( CellularContext_t * pContext )
{
    if( pContext != NULL )
    {
        IotMutex_Unlock( &pContext->pktRequestMutex );
    }
}

/*-----------------------------------------------------------*/

/* _searchCompareFunc is returning a variable with "int" data type because
 * this is the Compare function used in bsearch() function.
 * bsearch function syntax mandates the compare function should be of type int.
 * Hence int data type is used instead of typedef datatype. */
/* coverity[misra_c_2012_directive_4_6_violation] */
static int _searchCompareFunc( const void * pInputToken,
                               const void * pBase )
{
    /* _searchCompareFunc is returning a variable with "int" data type because
     * this is the Compare function used in bsearch() function.
     * bsearch function syntax mandates the compare function should be of type int.
     * Hence int data type is used instead of typedef datatype. */
    /* coverity[misra_c_2012_directive_4_6_violation] */
    int compareValue = 0;
    const char * pToken = ( const char * ) pInputToken;
    const CellularAtParseTokenMap_t * pBasePtr = ( const CellularAtParseTokenMap_t * ) pBase;
    uint32_t tokenLen = strlen( pInputToken );
    uint32_t strLen = strlen( pBasePtr->pStrValue );

    compareValue = strncmp( pToken,
                            pBasePtr->pStrValue,
                            MAX( tokenLen, strLen ) );

    /* To avoid undefined behavior, the table should not contain duplicated item and
     * compareValue is 0 only if the string is exactly the same. */
    if( ( compareValue == 0 ) && ( tokenLen != strLen ) )
    {
        if( tokenLen > strLen )
        {
            compareValue = 1;
        }
        else
        {
            compareValue = -1;
        }
    }

    return compareValue;
}

/*-----------------------------------------------------------*/

/* _sortCompareFunc is returning a variable with "int" data type because
 * this is the Compare function used in qsort() function.
 * qsort function syntax mandates the compare function should be of type int.
 * Hence int data type is used instead of typedef datatype. */
/* coverity[misra_c_2012_directive_4_6_violation] */
static int _sortCompareFunc( const void * pElem1Ptr,
                             const void * pElem2Ptr )
{
    /* _sortCompareFunc is returning a variable with "int" data type because
     * this is the Compare function used in qsort() function.
     * qsort function syntax mandates the compare function should be of type int.
     * Hence int data type is used instead of typedef datatype. */
    /* coverity[misra_c_2012_directive_4_6_violation] */
    int compareValue = 0;
    const CellularAtParseTokenMap_t * pElement1Ptr = ( const CellularAtParseTokenMap_t * ) pElem1Ptr;
    const CellularAtParseTokenMap_t * pElement2Ptr = ( const CellularAtParseTokenMap_t * ) pElem2Ptr;
    uint32_t element1PtrLen = strlen( pElement1Ptr->pStrValue );
    uint32_t element2PtrLen = strlen( pElement2Ptr->pStrValue );

    compareValue = strncmp( pElement1Ptr->pStrValue,
                            pElement2Ptr->pStrValue,
                            MAX( element1PtrLen, element2PtrLen ) );

    /* To avoid undefined behavior, the table should not contain duplicated item and
     * compareValue is 0 only if the string is exactly the same. */
    if( ( compareValue == 0 ) && ( element1PtrLen != element2PtrLen ) )
    {
        if( element1PtrLen > element2PtrLen )
        {
            compareValue = 1;
        }
        else
        {
            compareValue = -1;
        }
    }

    return compareValue;
}

/*-----------------------------------------------------------*/

static void _Cellular_ProcessGenericUrc( const CellularContext_t * pContext,
                                         const char * pInputLine )
{
    if( pContext == NULL )
    {
        IotLogWarn( "_Cellular_ProcessGenericUrc: Context not set" );
    }
    else
    {
        _Cellular_GenericCallback( pContext, pInputLine );
    }
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _atParseGetHandler( CellularContext_t * pContext,
                                               char * pTokenPtr,
                                               char * pSavePtr )
{
    /* Now get the handler function based on the token. */
    const CellularAtParseTokenMap_t * pElementPtr = NULL;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    const CellularAtParseTokenMap_t * pTokenMap = pContext->tokenTable.pCellularUrcHandlerTable;
    uint32_t tokenMapSize = pContext->tokenTable.cellularPrefixToParserMapSize;

    /* the unspecified behavior, which relates to the treatment of elements that compare as equal,
     * can be avoided by ensuring that the comparison function never returns 0.
     * When two elements are otherwise equal, the comparison function could
     * return a value that indicates their relative order in the initial array.
     * This the token table must be checked without duplicated string. The return value
     * is 0 only if the string is exactly the same. */
    /* coverity[misra_c_2012_rule_21_9_violation] */
    pElementPtr = ( CellularAtParseTokenMap_t * ) bsearch( ( const void * ) pTokenPtr,
                                                           ( const void * ) pTokenMap,
                                                           tokenMapSize,
                                                           sizeof( CellularAtParseTokenMap_t ),
                                                           _searchCompareFunc );

    if( pElementPtr != NULL )
    {
        if( pElementPtr->parserFunc != NULL )
        {
            pElementPtr->parserFunc( pContext, pSavePtr );
        }
        else
        {
            IotLogWarn( "No URC Callback func avail %s", pTokenPtr );
            pktStatus = CELLULAR_PKT_STATUS_FAILURE;
        }
    }
    else
    {
        /* No URC callback function available, check for generic call back. */
        IotLogDebug( "No URC Callback func avail %s, now trying generic URC Callback", pTokenPtr );
        _Cellular_ProcessGenericUrc( pContext, pSavePtr );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

void _Cellular_PktHandlerCleanup( CellularContext_t * pContext )
{
    if( ( pContext != NULL ) && ( pContext->pktRespQueue != NULL ) )
    {
        /* Wait for response to finish. */
        _Cellular_PktHandlerAcquirePktRequestMutex( pContext );
        vQueueDelete( pContext->pktRespQueue );
        pContext->pktRespQueue = NULL;
        _Cellular_PktHandlerReleasePktRequestMutex( pContext );
    }
}

/*-----------------------------------------------------------*/

void _Cellular_HandlePacket( CellularContext_t * pContext,
                             _atRespType_t atRespType,
                             const void * pBuf )
{
    if( pContext == NULL )
    {
        IotLogError( "_Cellular_HandlePacket : Invalid cellular context" );
    }
    else
    {
        switch( atRespType )
        {
            case AT_SOLICITED:
                _convertAndQueueRespPacket( pContext, pBuf );
                break;

            case AT_UNSOLICITED:
                _processUrcPacket( pContext, pBuf );
                break;

            default:
                IotLogError( "_Cellular_HandlePacket Callback type (%d) error", atRespType );
                break;
        }
    }
}

/*-----------------------------------------------------------*/

/* This function is provided as common code to cellular module porting.
 * Vendor may choose to use this function or use their implementation. */
/* coverity[misra_c_2012_rule_8_7_violation]. */
CellularPktStatus_t _Cellular_AtcmdRequestWithCallback( CellularContext_t * pContext,
                                                        CellularAtReq_t atReq )
{
    /* Parameters are checked in this function. */
    return _Cellular_TimeoutAtcmdRequestWithCallback( pContext, atReq, PACKET_REQ_TIMEOUT_MS );
}

/*-----------------------------------------------------------*/

/* This function is provided as common code to cellular module porting.
 * Vendor may choose to use this function or use their implementation. */
/* coverity[misra_c_2012_rule_8_7_violation]. */
CellularPktStatus_t _Cellular_TimeoutAtcmdRequestWithCallback( CellularContext_t * pContext,
                                                               CellularAtReq_t atReq,
                                                               uint32_t timeoutMS )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    if( pContext == NULL )
    {
        IotLogError( "_Cellular_TimeoutAtcmdRequestWithCallback : Invalid cellular context" );
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else
    {
        _Cellular_PktHandlerAcquirePktRequestMutex( pContext );
        pktStatus = _Cellular_TimeoutAtcmdRequestWithCallbackRaw( pContext, atReq, timeoutMS );
        _Cellular_PktHandlerReleasePktRequestMutex( pContext );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* This function is provided as common code to cellular module porting.
 * Vendor may choose to use this function or use their implementation. */
/* coverity[misra_c_2012_rule_8_7_violation]. */
CellularPktStatus_t _Cellular_TimeoutAtcmdDataRecvRequestWithCallback( CellularContext_t * pContext,
                                                                       CellularAtReq_t atReq,
                                                                       uint32_t timeoutMS,
                                                                       CellularATCommandDataPrefixCallback_t pktDataPrefixCallback,
                                                                       void * pCallbackContext )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    if( pContext == NULL )
    {
        IotLogError( "_Cellular_TimeoutAtcmdDataRecvRequestWithCallback : Invalid cellular context" );
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else
    {
        _Cellular_PktHandlerAcquirePktRequestMutex( pContext );
        pContext->pktDataPrefixCB = pktDataPrefixCallback;
        pContext->pDataPrefixCBContext = pCallbackContext;
        pktStatus = _Cellular_TimeoutAtcmdRequestWithCallbackRaw( pContext, atReq, timeoutMS );
        pContext->pktDataPrefixCB = NULL;
        pContext->pDataPrefixCBContext = NULL;
        _Cellular_PktHandlerReleasePktRequestMutex( pContext );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* This function is provided as common code to cellular module porting.
 * Vendor may choose to use this function or use their implementation. */
/* coverity[misra_c_2012_rule_8_7_violation]. */
CellularPktStatus_t _Cellular_AtcmdDataSend( CellularContext_t * pContext,
                                             CellularAtReq_t atReq,
                                             CellularAtDataReq_t dataReq,
                                             CellularATCommandDataSendPrefixCallback_t pktDataSendPrefixCallback,
                                             void * pCallbackContext,
                                             uint32_t atTimeoutMS,
                                             uint32_t dataTimeoutMS,
                                             uint32_t interDelayMS )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    if( pContext == NULL )
    {
        IotLogError( "_Cellular_TimeoutAtcmdDataSendRequestWithCallback : Invalid cellular context" );
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else
    {
        _Cellular_PktHandlerAcquirePktRequestMutex( pContext );
        pContext->pktDataSendPrefixCB = pktDataSendPrefixCallback;
        pContext->pDataSendPrefixCBContext = pCallbackContext;
        pktStatus = _Cellular_TimeoutAtcmdRequestWithCallbackRaw( pContext, atReq, atTimeoutMS );
        pContext->pDataSendPrefixCBContext = NULL;
        pContext->pktDataSendPrefixCB = NULL;

        if( pktStatus == CELLULAR_PKT_STATUS_OK )
        {
            pktStatus = _Cellular_DataSendWithTimeoutDelayRaw( pContext, dataReq, dataTimeoutMS, interDelayMS );
        }

        _Cellular_PktHandlerReleasePktRequestMutex( pContext );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* This function is provided as common code to cellular module porting.
 * Vendor may choose to use this function or use their implementation. */
/* coverity[misra_c_2012_rule_8_7_violation]. */
CellularPktStatus_t _Cellular_TimeoutAtcmdDataSendRequestWithCallback( CellularContext_t * pContext,
                                                                       CellularAtReq_t atReq,
                                                                       CellularAtDataReq_t dataReq,
                                                                       uint32_t atTimeoutMS,
                                                                       uint32_t dataTimeoutMS )
{
    return _Cellular_AtcmdDataSend( pContext, atReq, dataReq, NULL, NULL, atTimeoutMS, dataTimeoutMS, 0U );
}

/*-----------------------------------------------------------*/

/* This function is provided as common code to cellular module porting.
 * Vendor may choose to use this function or use their implementation. */
/* coverity[misra_c_2012_rule_8_7_violation]. */
CellularPktStatus_t _Cellular_TimeoutAtcmdDataSendSuccessToken( CellularContext_t * pContext,
                                                                CellularAtReq_t atReq,
                                                                CellularAtDataReq_t dataReq,
                                                                uint32_t atTimeoutMS,
                                                                uint32_t dataTimeoutMS,
                                                                const char ** pCellularSrcTokenSuccessTable,
                                                                uint32_t cellularSrcTokenSuccessTableSize )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    if( pContext == NULL )
    {
        IotLogError( "_Cellular_TimeoutAtcmdDataSendSuccessToken : Invalid cellular context" );
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else
    {
        _Cellular_PktHandlerAcquirePktRequestMutex( pContext );
        pContext->tokenTable.pCellularSrcExtraTokenSuccessTable = pCellularSrcTokenSuccessTable;
        pContext->tokenTable.cellularSrcExtraTokenSuccessTableSize = cellularSrcTokenSuccessTableSize;
        pktStatus = _Cellular_TimeoutAtcmdRequestWithCallbackRaw( pContext, atReq, atTimeoutMS );
        pContext->tokenTable.cellularSrcExtraTokenSuccessTableSize = 0;
        pContext->tokenTable.pCellularSrcExtraTokenSuccessTable = NULL;

        if( pktStatus == CELLULAR_PKT_STATUS_OK )
        {
            pktStatus = _Cellular_DataSendWithTimeoutDelayRaw( pContext, dataReq, dataTimeoutMS, 0U );
        }

        _Cellular_PktHandlerReleasePktRequestMutex( pContext );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

CellularPktStatus_t _Cellular_PktHandlerInit( CellularContext_t * pContext )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    if( pContext != NULL )
    {
        /* Create the response queue which is used to post reponses to the sender. */
        pContext->pktRespQueue = xQueueCreate( 1, sizeof( CellularPktStatus_t ) );

        if( pContext->pktRespQueue == NULL )
        {
            pktStatus = CELLULAR_PKT_STATUS_FAILURE;
        }
    }
    else
    {
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

void _Cellular_AtParseInit( const CellularContext_t * pContext )
{
    uint32_t i = 0;
    bool finit = true;
    const CellularAtParseTokenMap_t * pTokenMap = NULL;
    uint32_t tokenMapSize = 0;
    int32_t result = 0;

    if( ( pContext != NULL ) && ( pContext->tokenTable.pCellularUrcHandlerTable != NULL ) &&
        ( pContext->tokenTable.cellularPrefixToParserMapSize > 0U ) )
    {
        pTokenMap = pContext->tokenTable.pCellularUrcHandlerTable;
        tokenMapSize = pContext->tokenTable.cellularPrefixToParserMapSize;

        /* Check order of the sorted Map. */
        for( i = 0; i < ( tokenMapSize - 1U ); i++ )
        {
            result = _sortCompareFunc( &pTokenMap[ i ], &pTokenMap[ i + 1U ] );

            if( result >= 0 )
            {
                IotLogError( "AtParseFail for %u: %d %s %s", i, result,
                             pTokenMap[ i ].pStrValue, pTokenMap[ i + 1U ].pStrValue );
                finit = false;
            }
        }

        /* The Prefix Map should be sorted. */
        #if ( SORT_MODULE_TOKEN_MAP == 1U )
            if( finit != true )
            {
                taskENTER_CRITICAL();

                /* Ensure that the prefix to parse map is sorted
                 * to rule out any manual insertion errors while introducing new AT elements. */
                /* coverity[misra_c_2012_rule_10_4_violation]. */
                qsort( pTokenMap,
                       tokenMapSize,
                       sizeof( CellularAtParseTokenMap_t ), _sortCompareFunc );
                finit = true;
                taskEXIT_CRITICAL();
            }
        #else /* if ( SORT_MODULE_TOKEN_MAP == 1U ) */
            if( finit != true )
            {
                IotLogError( "AtParseFail URC token table is not sorted" );
            }

            /* coverity[misra_c_2012_rule_10_4_violation] */
            /* coverity[misra_c_2012_rule_10_5_violation] */
            configASSERT( finit == true );
        #endif /* if ( SORT_MODULE_TOKEN_MAP == 1U ) */

        for( i = 0; i < tokenMapSize; i++ )
        {
            IotLogDebug( "Callbacks setup for %u : %s", i, pTokenMap[ i ].pStrValue );
        }
    }
}

/*-----------------------------------------------------------*/

bool _Cellular_CreatePktRequestMutex( CellularContext_t * pContext )
{
    bool status = false;

    if( pContext != NULL )
    {
        status = IotMutex_Create( &pContext->pktRequestMutex, false );
    }

    return status;
}

/*-----------------------------------------------------------*/

bool _Cellular_CreatePktResponseMutex( CellularContext_t * pContext )
{
    bool status = false;

    if( pContext != NULL )
    {
        status = IotMutex_Create( &pContext->PktRespMutex, false );
    }

    return status;
}

/*-----------------------------------------------------------*/

void _Cellular_DestroyPktRequestMutex( CellularContext_t * pContext )
{
    if( pContext != NULL )
    {
        IotMutex_Destroy( &pContext->pktRequestMutex );
    }
}

/*-----------------------------------------------------------*/

void _Cellular_DestroyPktResponseMutex( CellularContext_t * pContext )
{
    if( pContext != NULL )
    {
        IotMutex_Destroy( &pContext->PktRespMutex );
    }
}

/*-----------------------------------------------------------*/
