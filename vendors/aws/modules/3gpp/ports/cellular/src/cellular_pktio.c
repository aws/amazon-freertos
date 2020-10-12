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
 * @brief cellular HAL common packet I/O functions to assemble packet from comm interface.
 */

#include "iot_config.h"
#include "aws_cellular_config.h"
#include "cellular_config_defaults.h"
#include "cellular_types.h"

/* Standard includes. */
#include <string.h>
#include <stdlib.h>

/* Platform layer includes. */
#include "platform/iot_threads.h"
#include "event_groups.h"

#include "cellular_internal.h"
#include "cellular_pktio_internal.h"
#include "cellular_common_internal.h"

/*-----------------------------------------------------------*/

#define PKTIO_EVT_MASK_STARTED    ( 0x0001UL )
#define PKTIO_EVT_MASK_ABORT      ( 0x0002UL )
#define PKTIO_EVT_MASK_ABORTED    ( 0x0004UL )
#define PKTIO_EVT_MASK_RX_DATA    ( 0x0008UL )
#define PKTIO_EVT_MASK_ALL_EVENTS \
    ( PKTIO_EVT_MASK_STARTED      \
      | PKTIO_EVT_MASK_ABORT      \
      | PKTIO_EVT_MASK_ABORTED    \
      | PKTIO_EVT_MASK_RX_DATA )

#define FREE_AT_RESPONSE_AND_SET_NULL( pResp )    { ( _Cellular_AtResponseFree( ( pResp ) ) ); ( pResp = NULL ); }

#define PKTIO_COMMIF_RECV_TIMEOUT    ( 100UL )

/*-----------------------------------------------------------*/

static void _saveData( const char * pLine,
                       CellularATCommandResponse_t * pResp,
                       uint32_t dataLen );
static void _saveRawData( const char * pLine,
                          CellularATCommandResponse_t * pResp,
                          uint32_t dataLen );
static void _saveATData( const char * pLine,
                         CellularATCommandResponse_t * pResp );
static CellularPktStatus_t _processIntermediateResponse( const char * pLine,
                                                         CellularATCommandResponse_t * pResp,
                                                         CellularATCommandType_t atType,
                                                         const char * pRespPrefix );
static CellularATCommandResponse_t * _Cellular_AtResponseNew( void );
static void _Cellular_AtResponseFree( CellularATCommandResponse_t * pResp );
static CellularPktStatus_t _Cellular_ProcessData( const char * pData,
                                                  CellularATCommandResponse_t * pAtResp,
                                                  uint32_t dataLen );
static CellularPktStatus_t _Cellular_ProcessLine( const CellularContext_t * pContext,
                                                  const char * pLine,
                                                  CellularATCommandResponse_t * pResp,
                                                  CellularATCommandType_t atType,
                                                  const char * pRespPrefix );
static bool urcTokenWoPrefix( const CellularContext_t * pContext,
                              const char * pLine );
static _atRespType_t _getMsgType( const CellularContext_t * pContext,
                                  const char * pLine,
                                  const char * pRespPrefix );
static void _Cellular_PktRxCallBack( void * pUserData,
                                     CellularCommInterfaceHandle_t commInterfaceHandle );
static char * _handleLeftoverBuffer( CellularContext_t * pContext,
                                     uint32_t * pBytesRead,
                                     uint32_t partialData );
static char * _Cellular_ReadLine( CellularContext_t * pContext,
                                  uint32_t * pBytesRead,
                                  uint32_t partialData );
static CellularPktStatus_t _handleData( char * pStartOfData,
                                        CellularContext_t * pContext,
                                        CellularATCommandResponse_t * pAtResp,
                                        char ** ppLine,
                                        uint32_t bytesRead,
                                        uint32_t * pBytesLeft );
static CellularPktStatus_t _handleReceivedMsg( CellularContext_t * pContext,
                                               CellularATCommandResponse_t ** ppAtResp,
                                               char * pLine,
                                               char ** ppStartOfData );
static char * _handleAllReceivedAtResponse( CellularContext_t * pContext,
                                            char * pTempLine,
                                            _atRespType_t * pRecvdMsgType );
static void _handleAllReceived( CellularContext_t * pContext,
                                CellularATCommandResponse_t ** ppAtResp,
                                char * pLine,
                                uint32_t bytesRead );
static uint32_t _handleRxDataEvent( CellularContext_t * pContext,
                                    CellularATCommandResponse_t ** ppAtResp );
static void _pktioReadThread( void * pUserData );
static void _PktioInitProcessReadThreadStatus( CellularContext_t * pContext );

/*-----------------------------------------------------------*/

static void _saveData( const char * pLine,
                       CellularATCommandResponse_t * pResp,
                       uint32_t dataLen )
{
    CellularATCommandLine_t * pNew, * pTemp;

    /* coverity[misra_c_2012_rule_10_5_violation] */
    configASSERT( ( int32_t ) ( ( pLine != NULL ) && ( pResp != NULL ) ) );
    pNew = ( CellularATCommandLine_t * ) pvPortMalloc( sizeof( CellularATCommandLine_t ) );
    /* coverity[misra_c_2012_rule_10_5_violation] */
    configASSERT( ( int32_t ) ( pNew != NULL ) );

    if( dataLen != 0U )
    {
        pNew->pLine = ( char * ) pvPortMalloc( dataLen );
        /* coverity[misra_c_2012_rule_10_5_violation] */
        configASSERT( ( int32_t ) ( pNew->pLine != NULL ) );
        ( void ) memcpy( pNew->pLine, pLine, dataLen );
        IotLogDebug( "Adding data [%p] to loc [%p]", pNew->pLine, pNew->pLine );
    }
    else
    {
        ( void ) Cellular_ATStrDup( &pNew->pLine, pLine );
        IotLogDebug( "Adding [%s] to loc [%p]", pNew->pLine, pNew->pLine );
    }

    pNew->pNext = NULL;

    if( pResp->pItm == NULL )
    {
        pResp->pItm = pNew;
    }
    else
    {
        pTemp = pResp->pItm;

        while( pTemp->pNext != NULL )
        {
            pTemp = pTemp->pNext;
        }

        pTemp->pNext = pNew;
    }
}

/*-----------------------------------------------------------*/

static void _saveRawData( const char * pLine,
                          CellularATCommandResponse_t * pResp,
                          uint32_t dataLen )
{
    _saveData( pLine, pResp, dataLen );
}

/*-----------------------------------------------------------*/

static void _saveATData( const char * pLine,
                         CellularATCommandResponse_t * pResp )
{
    _saveData( pLine, pResp, 0 );
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _processIntermediateResponse( const char * pLine,
                                                         CellularATCommandResponse_t * pResp,
                                                         CellularATCommandType_t atType,
                                                         const char * pRespPrefix )
{
    CellularPktStatus_t pkStatus = CELLULAR_PKT_STATUS_PENDING_DATA;
    bool result = true;
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;

    switch( atType )
    {
        case CELLULAR_AT_WO_PREFIX:

            if( pResp->pItm == NULL )
            {
                _saveATData( pLine, pResp );
            }
            else
            {
                /* We already have an intermediate response. */
                pkStatus = CELLULAR_PKT_STATUS_INVALID_DATA;
                IotLogError( "CELLULAR_AT_WO_PREFIX AT process ERROR: %s, status: %d ", pLine, pkStatus );
            }

            break;

        case CELLULAR_AT_WITH_PREFIX:

            if( pResp->pItm == NULL )
            {
                atStatus = Cellular_ATStrStartWith( pLine, pRespPrefix, &result );

                if( ( atStatus == CELLULAR_AT_SUCCESS ) && ( result == true ) )
                {
                    _saveATData( pLine, pResp );
                }
                else
                {
                    /* Prefix mismatch at the beginning of response. */
                    pkStatus = CELLULAR_PKT_STATUS_PREFIX_MISMATCH;
                    IotLogError( "CELLULAR_AT_WITH_PREFIX AT process ERROR: %s respPrefix %s status: %d", pLine, pRespPrefix, pkStatus );
                }
            }
            else
            {
                /* We already have an intermediate response. */
                pkStatus = CELLULAR_PKT_STATUS_INVALID_DATA;
                IotLogError( "CELLULAR_AT_WITH_PREFIX AT process ERROR: %s, status: %d ", pLine, pkStatus );
            }

            break;

        case CELLULAR_AT_MULTI_WITH_PREFIX:

            if( Cellular_ATStrStartWith( pLine, pRespPrefix, &result ) == CELLULAR_AT_SUCCESS )
            {
                if( result == true )
                {
                    _saveATData( pLine, pResp );
                }
                else
                {
                    /* Prefix mismatch at the beginning of response. */
                    pkStatus = CELLULAR_PKT_STATUS_PREFIX_MISMATCH;
                    IotLogError( "CELLULAR_AT_MULTI_WITH_PREFIX AT process ERROR: %s respPrefix %s status: %d", pLine, pRespPrefix, pkStatus );
                }
            }

            break;

        case CELLULAR_AT_MULTI_WO_PREFIX:
            _saveATData( pLine, pResp );
            break;

        case CELLULAR_AT_MULTI_DATA_WO_PREFIX:
            _saveATData( pLine, pResp );
            pkStatus = CELLULAR_PKT_STATUS_PENDING_DATA_BUFFER;
            break;

        default:
            /* This should never be reached. */
            IotLogError( "FATAL:AT Unsupported AT command type %d", atType );
            pkStatus = CELLULAR_PKT_STATUS_FAILURE;
            break;
    }

    return pkStatus;
}

/*-----------------------------------------------------------*/

static CellularATCommandResponse_t * _Cellular_AtResponseNew( void )
{
    CellularATCommandResponse_t * pNew;

    pNew = ( CellularATCommandResponse_t * ) pvPortMalloc( sizeof( CellularATCommandResponse_t ) );
    /* coverity[misra_c_2012_rule_10_5_violation] */
    configASSERT( ( int32_t ) ( pNew != NULL ) );

    ( void ) memset( ( void * ) pNew, 0, sizeof( CellularATCommandResponse_t ) );

    return pNew;
}

/*-----------------------------------------------------------*/

/**
 * Returns a pointer to the end of the next line
 * special-cases the "> " SMS prompt.
 *
 * Returns NULL if there is no complete line.
 */
static void _Cellular_AtResponseFree( CellularATCommandResponse_t * pResp )
{
    CellularATCommandLine_t * pCurrLine;

    if( pResp != NULL )
    {
        pCurrLine = pResp->pItm;

        while( pCurrLine != NULL )
        {
            CellularATCommandLine_t * pToFree;
            pToFree = pCurrLine;
            pCurrLine = pCurrLine->pNext;

            if( pToFree->pLine != NULL )
            {
                vPortFree( pToFree->pLine );
            }

            vPortFree( pToFree );
        }

        vPortFree( pResp );
    }
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _Cellular_ProcessData( const char * pData,
                                                  CellularATCommandResponse_t * pAtResp,
                                                  uint32_t dataLen )
{
    CellularPktStatus_t pkStatus = CELLULAR_PKT_STATUS_OK;

    if( ( pData == NULL ) || ( pAtResp == NULL ) )
    {
        pkStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }

    _saveRawData( pData, pAtResp, dataLen );

    return pkStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _Cellular_ProcessLine( const CellularContext_t * pContext,
                                                  const char * pLine,
                                                  CellularATCommandResponse_t * pResp,
                                                  CellularATCommandType_t atType,
                                                  const char * pRespPrefix )
{
    CellularPktStatus_t pkStatus = CELLULAR_PKT_STATUS_FAILURE;
    bool result = true;
    const char * const * pTokenSuccessTable = NULL;
    const char * const * pTokenErrorTable = NULL;
    const char * const * pTokenExtraTable = NULL;
    uint32_t tokenSuccessTableSize = 0;
    uint32_t tokenErrorTableSize = 0;
    uint32_t tokenExtraTableSize = 0;


    if( ( pContext != NULL ) &&
        ( pContext->tokenTable.pCellularSrcTokenErrorTable != NULL ) &&
        ( pContext->tokenTable.pCellularSrcTokenSuccessTable != NULL ) )
    {
        pTokenSuccessTable = pContext->tokenTable.pCellularSrcTokenSuccessTable;
        tokenSuccessTableSize = pContext->tokenTable.cellularSrcTokenSuccessTableSize;
        pTokenErrorTable = pContext->tokenTable.pCellularSrcTokenErrorTable;
        tokenErrorTableSize = pContext->tokenTable.cellularSrcTokenErrorTableSize;
        pTokenExtraTable = pContext->tokenTable.pCellularSrcExtraTokenSuccessTable;
        tokenExtraTableSize = pContext->tokenTable.cellularSrcExtraTokenSuccessTableSize;

        if( pResp == NULL )
        {
            /* Error. It should never happen. */
            IotLogError( "FATAL ERROR: pResp is NULL" );
        }
        else if( ( pTokenExtraTable != NULL ) &&
                 ( Cellular_ATcheckErrorCode( pLine, pTokenExtraTable,
                                              tokenExtraTableSize, &result ) == CELLULAR_AT_SUCCESS ) &&
                 ( result == true ) )
        {
            pResp->status = true;
            pkStatus = CELLULAR_PKT_STATUS_OK;
            IotLogDebug( "Final AT response is SUCCESS [%s] in extra table", pLine );
        }
        else if( ( Cellular_ATcheckErrorCode( pLine, pTokenSuccessTable,
                                              tokenSuccessTableSize, &result ) == CELLULAR_AT_SUCCESS ) &&
                 ( result == true ) )
        {
            pResp->status = true;
            pkStatus = CELLULAR_PKT_STATUS_OK;
            IotLogDebug( "Final AT response is SUCCESS [%s]", pLine );
        }
        else if( ( Cellular_ATcheckErrorCode( pLine, pTokenErrorTable,
                                              tokenErrorTableSize, &result ) == CELLULAR_AT_SUCCESS ) &&
                 ( result == true ) )
        {
            pResp->status = false;
            pkStatus = CELLULAR_PKT_STATUS_OK;
            IotLogError( "Modem return ERROR: %s %s respPrefix: %s status: %d",
                         pContext->pCurrentCmd, pLine, pRespPrefix, pkStatus );
        }
        else
        {
            pkStatus = _processIntermediateResponse( pLine, pResp, atType, pRespPrefix );
        }
    }

    return pkStatus;
}

/*-----------------------------------------------------------*/

static bool urcTokenWoPrefix( const CellularContext_t * pContext,
                              const char * pLine )
{
    bool ret = false;
    uint32_t i = 0;
    uint32_t urcTokenTableSize = pContext->tokenTable.cellularUrcTokenWoPrefixTableSize;
    const char * const * const pUrcTokenTable = pContext->tokenTable.pCellularUrcTokenWoPrefixTable;

    for( i = 0; i < urcTokenTableSize; i++ )
    {
        if( strcmp( pLine, pUrcTokenTable[ i ] ) == 0 )
        {
            ret = true;
            break;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

static _atRespType_t _getMsgType( const CellularContext_t * pContext,
                                  const char * pLine,
                                  const char * pRespPrefix )
{
    _atRespType_t atRespType = AT_UNDEFINED;
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    bool inputWithPrefix = false;
    bool inputWithSrcPrefix = false;

    if( ( pContext == NULL ) || ( pLine == NULL ) || ( pContext->tokenTable.pCellularUrcTokenWoPrefixTable == NULL ) )
    {
        atStatus = CELLULAR_AT_ERROR;
        atRespType = AT_UNDEFINED;
    }
    else if( urcTokenWoPrefix( pContext, pLine ) == true )
    {
        atRespType = AT_UNSOLICITED;
    }
    else
    {
        /* Check if prefix exist in pLine. */
        atStatus = Cellular_ATIsPrefixPresent( pLine, &inputWithPrefix );

        if( ( atStatus == CELLULAR_AT_SUCCESS ) && ( inputWithPrefix == true ) && ( pRespPrefix != NULL ) )
        {
            /* Check if SRC prefix exist in pLine. */
            atStatus = Cellular_ATStrStartWith( pLine, pRespPrefix, &inputWithSrcPrefix );
        }
    }

    if( ( atStatus == CELLULAR_AT_SUCCESS ) && ( atRespType == AT_UNDEFINED ) )
    {
        if( inputWithPrefix == true )
        {
            if( ( pContext->PktioAtCmdType != CELLULAR_AT_NO_COMMAND ) && ( inputWithSrcPrefix == true ) )
            {
                atRespType = AT_SOLICITED;
            }
            else
            {
                atRespType = AT_UNSOLICITED;
            }
        }
        else
        {
            if( ( ( pContext->PktioAtCmdType != CELLULAR_AT_NO_COMMAND ) && ( pRespPrefix == NULL ) ) ||
                ( pContext->PktioAtCmdType == CELLULAR_AT_MULTI_WO_PREFIX ) ||
                ( pContext->PktioAtCmdType == CELLULAR_AT_MULTI_DATA_WO_PREFIX ) ||
                ( pContext->PktioAtCmdType == CELLULAR_AT_WITH_PREFIX ) )
            {
                atRespType = AT_SOLICITED;
            }
        }
    }

    return atRespType;
}

/*-----------------------------------------------------------*/
/* Cellular comm interface callback prototype. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static void _Cellular_PktRxCallBack( void * pUserData,
                                     CellularCommInterfaceHandle_t commInterfaceHandle )
{
    ( void ) commInterfaceHandle;

    /* The context of this function is a ISR. */
    const CellularContext_t * pContext = ( CellularContext_t * ) pUserData;
    BaseType_t xHigherPriorityTaskWoken, xResult;

    if( ( pContext != NULL ) && ( pContext->pPktioCommEvent != NULL ) )
    {
        xHigherPriorityTaskWoken = pdFALSE;
        xResult = xEventGroupSetBitsFromISR( pContext->pPktioCommEvent,
                                             PKTIO_EVT_MASK_RX_DATA,
                                             &xHigherPriorityTaskWoken );

        if( xResult == pdPASS )
        {
            /* portYIELD_FROM_ISR manipulates hardware registers by writing to them directly. */
            /* coverity[misra_c_2012_rule_11_4_violation] */
            /* coverity[misra_c_2012_rule_1_2_violation] */
            /* coverity[misra_c_2012_rule_20_7_violation] */
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        }
    }
}

/*-----------------------------------------------------------*/

static char * _handleLeftoverBuffer( CellularContext_t * pContext,
                                     uint32_t * pBytesRead,
                                     uint32_t partialData )
{
    char * pAtBuf = NULL;
    char ** ppAtBufLeftOver = NULL;
    char * pRead = NULL; /* Pointet to first empty buffer in pktioReadBuf. */
    char * pEndOfLine = NULL;
    uint32_t len = 0;

    pAtBuf = pContext->pktioReadBuf;
    ppAtBufLeftOver = &pContext->pPktioReadPtr;
    pRead = pAtBuf;

    IotLogDebug( "Non empty buffer at the beginning: %p", *ppAtBufLeftOver );

    if( partialData != 0U )
    {
        /* Data is received, but not complete.
         * Move it up and prepare to read more. */
        len = partialData;
        IotLogDebug( "leftover Data len=%d", len );
    }
    else
    {
        /* There is AT command in the buffer from last read.
         * Skip over leading newlines. */
        while( ( **ppAtBufLeftOver == '\r' ) || ( **ppAtBufLeftOver == '\n' ) )
        {
            ( *ppAtBufLeftOver )++;
        }

        ( void ) Cellular_ATFindNextEOL( &pEndOfLine, *ppAtBufLeftOver );

        if( pEndOfLine == NULL )
        {
            len = strnlen( *ppAtBufLeftOver, PKTIO_READ_BUFFER_SIZE );
            IotLogDebug( "leftover AT buffer len=%d", len );
        }
        else
        {
            /* full line found in the leftover buffer, this is not possible so print an error if you see it. */
            IotLogError( "full line found in leftover buffer" );
        }
    }

    if( len != 0U )
    {
        /* Move the leftover data or AT command response to the start of buffer.
         * Set the pRead pointer to the empty buffer space. */
        ( void ) memmove( pAtBuf, *ppAtBufLeftOver, len + 1U );
        IotLogDebug( "moved the partial line/data to starting of pAtBuf=%p", pAtBuf );
        pRead = &pAtBuf[ len ];
        *pBytesRead += len;
        IotLogDebug( "Advanced pRead after partial line=%s", pRead );
    }

    /* set it to NULL since we have accounted for the leftover data. */
    *ppAtBufLeftOver = NULL;

    return pRead;
}

/*-----------------------------------------------------------*/

/* pBytesRead : bytes read from comm interface. */
/* partialData : leftover bytes in the pktioreadbuf. Not enougth to be a command. */
static char * _Cellular_ReadLine( CellularContext_t * pContext,
                                  uint32_t * pBytesRead,
                                  uint32_t partialData )
{
    char * pAtBuf = NULL, * pRead = NULL;
    uint32_t bufferUsed = 0;
    uint32_t bytesRead = 0;

    if( ( pContext != NULL ) && ( pContext->pCommIntf != NULL ) && ( pContext->hPktioCommIntf != NULL ) && ( pBytesRead != NULL ) )
    {
        pAtBuf = pContext->pktioReadBuf;
        pRead = pAtBuf;

        if( pContext->pPktioReadPtr != NULL )
        {
            pRead = _handleLeftoverBuffer( pContext, pBytesRead, partialData );
        }

        /* pRead, returned from _handleLeftoverBuffer is pointing to the same array as pAtBuf. */
        /* coverity[misra_c_2012_rule_10_8_violation] */
        /* coverity[misra_c_2012_rule_18_2_violation] */
        bufferUsed = ( uint32_t ) ( pRead - pAtBuf );

        if( ( PKTIO_READ_BUFFER_SIZE - bufferUsed ) == 0U )
        {
            IotLogError( "Input line exceeded buffer", "error on read" );
            pRead = pAtBuf;
        }

        ( void ) pContext->pCommIntf->recv( pContext->hPktioCommIntf, ( uint8_t * ) pRead,
                                            PKTIO_READ_BUFFER_SIZE - bufferUsed,
                                            PKTIO_COMMIF_RECV_TIMEOUT, &bytesRead );

        if( bytesRead > 0U )
        {
            *pBytesRead += bytesRead;
            pRead[ bytesRead ] = '\0';
            IotLogDebug( "AT Read %d bytes, data[%p]", bytesRead, pRead );

            if( partialData == 0U )
            {
                /* skip over leading newlines. */
                while( ( *pAtBuf == '\r' ) || ( *pAtBuf == '\n' ) )
                {
                    pAtBuf++;
                }
            }
        }
    }

    return pAtBuf;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _handleData( char * pStartOfData,
                                        CellularContext_t * pContext,
                                        CellularATCommandResponse_t * pAtResp,
                                        char ** ppLine,
                                        uint32_t bytesRead,
                                        uint32_t * pBytesLeft )
{
    uint32_t * pDataLength = &pContext->dataLength;
    uint32_t * pPartialDataRcvdLen = &pContext->partialDataRcvdLen;
    /* Calculate the size of data received so far. */
    int32_t temp = ( pStartOfData - pContext->pktioReadBuf );
    CellularPktStatus_t pkStatus = CELLULAR_PKT_STATUS_OK;
    CellularATCommandResponse_t * pTempAtResp = pAtResp;

    /* Bytes before pStartOfData is not data, skip the bytes received. */
    *pPartialDataRcvdLen = bytesRead - ( uint32_t ) temp;

    if( *pPartialDataRcvdLen >= *pDataLength )
    {
        /* Add data to the response linked list. */
        pkStatus = _Cellular_ProcessData( pStartOfData, pTempAtResp, *pDataLength );
        /* Advance pLine to a point after data. */
        *ppLine = &pStartOfData[ *pDataLength ];

        if( *pPartialDataRcvdLen > *pDataLength )
        {
            /* There are more bytes after the data. */
            *pBytesLeft = ( *pPartialDataRcvdLen - *pDataLength );
        }
        else
        {
            *pBytesLeft = 0U;
        }

        /* reset the data related variables. */
        *pPartialDataRcvdLen = 0U;
        *pDataLength = 0U;

        if( pkStatus != CELLULAR_PKT_STATUS_OK )
        {
            FREE_AT_RESPONSE_AND_SET_NULL( ( pTempAtResp ) );
            IotLogError( "processData ERROR, cleaning up!" );
        }
    }
    else
    {
        /* The data received is partial. Store the start of data in read pointer. */
        IotLogDebug( "Partial data received, saving the start of data for further processing" );
        pContext->pPktioReadPtr = pStartOfData;
        pkStatus = CELLULAR_PKT_STATUS_PENDING_DATA_BUFFER;
    }

    return pkStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _handleReceivedMsg( CellularContext_t * pContext,
                                               CellularATCommandResponse_t ** ppAtResp,
                                               char * pLine,
                                               char ** ppStartOfData )
{
    const _atRespType_t * pRecvdMsgType = &pContext->recvdMsgType;
    uint32_t * pDataLength = &pContext->dataLength;
    CellularPktStatus_t pkStatus = CELLULAR_PKT_STATUS_OK;

    if( *pRecvdMsgType == AT_UNSOLICITED )
    {
        if( pContext->pPktioHandlepktCB != NULL )
        {
            pContext->pPktioHandlepktCB( pContext, AT_UNSOLICITED, pLine );
        }
    }
    else if( *pRecvdMsgType == AT_SOLICITED )
    {
        if( *ppAtResp == NULL )
        {
            *ppAtResp = _Cellular_AtResponseNew();
        }

        IotLogDebug( "AT Resp[%s]", pLine );
        pkStatus = _Cellular_ProcessLine( pContext, pLine, *ppAtResp, pContext->PktioAtCmdType, pContext->pRespPrefix );

        if( pkStatus == CELLULAR_PKT_STATUS_OK )
        {
            if( pContext->pPktioHandlepktCB != NULL )
            {
                pContext->pPktioHandlepktCB( pContext, AT_SOLICITED, *ppAtResp );
            }

            FREE_AT_RESPONSE_AND_SET_NULL( ( *ppAtResp ) );
        }
        else if( pkStatus == CELLULAR_PKT_STATUS_PENDING_DATA_BUFFER )
        {
            /* pStartOfData is NULL if not a data response. */
            if( pContext->pktDataPrefixCB != NULL )
            {
                /* ppStartOfData and pDataLength are not set if this function failed. */
                ( void ) pContext->pktDataPrefixCB( pContext->pDataPrefixCBContext,
                                                    pLine, ppStartOfData, pDataLength );
            }
        }
        else
        {
            if( pkStatus != CELLULAR_PKT_STATUS_PENDING_DATA )
            {
                ( void ) memset( pContext->pktioReadBuf, 0, PKTIO_READ_BUFFER_SIZE + 1U );
                pContext->pPktioReadPtr = NULL;
                FREE_AT_RESPONSE_AND_SET_NULL( ( *ppAtResp ) );
                IotLogError( "processLine ERROR, cleaning up! Current command %s", pContext->pCurrentCmd );
            }
        }
    }
    else
    {
        IotLogError( "recvdMsgType is AT_UNDEFINED for Message: %s %s", pLine, pContext->pCurrentCmd );
        ( void ) memset( pContext->pktioReadBuf, 0, PKTIO_READ_BUFFER_SIZE + 1U );
        pContext->pPktioReadPtr = NULL;
        FREE_AT_RESPONSE_AND_SET_NULL( ( *ppAtResp ) );
        pkStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }

    return pkStatus;
}

/*-----------------------------------------------------------*/

/* Internal function of _handleAllReceived to reduce complexity. */
static char * _handleAllReceivedAtResponse( CellularContext_t * pContext,
                                            char * pTempLine,
                                            _atRespType_t * pRecvdMsgType )
{
    _atRespType_t recvdMsgTypeNew = AT_UNDEFINED;
    char * pEol = NULL;

    ( void ) Cellular_ATFindNextEOL( &pEol, pTempLine );

    if( pEol != NULL )
    {
        *pEol = '\0';

        /* Some other response or URC needs to be processed.
         * Get the msgtype of the next msg in this read. */
        recvdMsgTypeNew = _getMsgType( pContext, pTempLine, pContext->pRespPrefix );

        if( ( recvdMsgTypeNew != AT_UNDEFINED ) && ( recvdMsgTypeNew != *pRecvdMsgType ) )
        {
            IotLogDebug( "Msg type changed from %d to %d", *pRecvdMsgType, recvdMsgTypeNew );
            *pRecvdMsgType = recvdMsgTypeNew;
        }
    }
    else
    {
        if( strlen( pTempLine ) > 0U )
        {
            IotLogDebug( "leftover buf str[%s]", pTempLine );
            pContext->pPktioReadPtr = pTempLine;
        }
    }

    return pEol;
}

/*-----------------------------------------------------------*/

static void _handleAllReceived( CellularContext_t * pContext,
                                CellularATCommandResponse_t ** ppAtResp,
                                char * pLine,
                                uint32_t bytesRead )
{
    _atRespType_t * pRecvdMsgType = &pContext->recvdMsgType;
    const uint32_t * pDataLength = &pContext->dataLength;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    char * pStartOfData = NULL, * pTempLine = pLine;
    const char * pEol = NULL;
    bool breakFlag = false;
    uint32_t bytesLeft = 0;

    *pRecvdMsgType = _getMsgType( pContext, pTempLine, pContext->pRespPrefix );

    do
    {
        /* parse the input line. */
        pktStatus = _handleReceivedMsg( pContext, ppAtResp, pTempLine, &pStartOfData );

        if( ( pktStatus != CELLULAR_PKT_STATUS_OK ) &&
            ( pktStatus != CELLULAR_PKT_STATUS_PENDING_DATA ) &&
            ( pktStatus != CELLULAR_PKT_STATUS_PENDING_DATA_BUFFER ) )
        {
            IotLogDebug( "_handleReceivedMsg failed %d", pktStatus );
            breakFlag = true;
        }
        else if( ( *pDataLength != 0U ) && ( pStartOfData != NULL ) )
        {
            /* The input line is a data recv command. Handle the data buffer. */
            pktStatus = _handleData( pStartOfData, pContext, *ppAtResp, &pTempLine, bytesRead, &bytesLeft );

            if( pktStatus == CELLULAR_PKT_STATUS_PENDING_DATA_BUFFER )
            {
                IotLogDebug( "Partial Data received" );
                breakFlag = true;
            }
            else if( ( pktStatus == CELLULAR_PKT_STATUS_OK ) && ( bytesLeft == 0U ) )
            {
                IotLogDebug( "Complete Data received" );
                breakFlag = true;
            }
            else if( pktStatus == CELLULAR_PKT_STATUS_OK )
            {
                IotLogDebug( "_handleData okay, keep processing %u bytes", bytesLeft );
            }
            else
            {
                IotLogError( "_handleData error %d.", pktStatus );
                breakFlag = true;
            }
        }
        else
        {
            /* Find other responses or urcs which need to be processed in this read buffer. */
            pTempLine = &pTempLine[ strnlen( pTempLine, PKTIO_READ_BUFFER_SIZE ) + 1U ];
        }

        if( breakFlag == true )
        {
            break;
        }

        /* Remove the leading change line. */
        while( ( *pTempLine == '\r' ) || ( *pTempLine == '\n' ) )
        {
            pTempLine++;
        }

        /* Parse the AT command response in the pTempLine. */
        pEol = _handleAllReceivedAtResponse( pContext, pTempLine, pRecvdMsgType );
    }
    while( pEol != NULL );
}

/*-----------------------------------------------------------*/

static uint32_t _handleRxDataEvent( CellularContext_t * pContext,
                                    CellularATCommandResponse_t ** ppAtResp )
{
    const uint32_t * pPartialDataRcvdLen = &pContext->partialDataRcvdLen;
    char * pLine, * pEol = NULL;
    uint32_t bytesRead = 0;
    uint32_t bytesLeft = 0;

    /* Return the first line, may be more lines in buffer. */
    pLine = _Cellular_ReadLine( pContext, &bytesRead, *pPartialDataRcvdLen );

    if( ( bytesRead > 0U ) && ( pLine != NULL ) )
    {
        if( ( *pPartialDataRcvdLen != 0U ) || ( pContext->dataLength != 0U ) )
        {
            ( void ) _handleData( pLine, pContext, *ppAtResp, &pLine, bytesRead, &bytesLeft );

            /* Remove the leading change line. */
            if( ( *pPartialDataRcvdLen == 0U ) && ( pContext->dataLength == 0U ) )
            {
                while( ( *pLine == '\r' ) || ( *pLine == '\n' ) )
                {
                    pLine++;
                }
            }
        }
        else
        {
            bytesLeft = bytesRead;
        }

        /* If bytesRead in _Cellular_ReadLine is all for data, don't parse the AT command response. */
        if( ( *pPartialDataRcvdLen == 0U ) && ( bytesLeft > 0U ) )
        {
            ( void ) Cellular_ATFindNextEOL( &pEol, pLine );

            if( pEol != NULL )
            {
                *pEol = '\0';
                _handleAllReceived( pContext, ppAtResp, pLine, bytesRead );
            }
            else
            {
                /* This means the line read is not a complete response,
                 * save it in the left over buffer and return. */
                IotLogDebug( "saving incomplete URC len[%d] Str[%s]", strlen( pLine ), pLine );
                pContext->pPktioReadPtr = pLine;
            }
        }
    }

    return bytesRead;
}

/*-----------------------------------------------------------*/

static void _pktioReadThread( void * pUserData )
{
    CellularContext_t * pContext = ( CellularContext_t * ) pUserData;
    CellularATCommandResponse_t * pAtResp = NULL;
    EventBits_t uxBits;
    uint32_t bytesRead = 0;

    /* Open main communication port. */
    if( ( pContext != NULL ) && ( pContext->pCommIntf != NULL ) &&
        ( pContext->pCommIntf->open( _Cellular_PktRxCallBack, ( void * ) pContext,
                                     &pContext->hPktioCommIntf ) == IOT_COMM_INTERFACE_SUCCESS ) )
    {
        /* Send thread started event. */
        ( void ) xEventGroupSetBits( pContext->pPktioCommEvent,
                                     PKTIO_EVT_MASK_STARTED );

        do
        {
            /* Wait events for abort thread or rx data available. */
            uxBits = xEventGroupWaitBits( ( pContext->pPktioCommEvent ),
                                          ( ( EventBits_t ) PKTIO_EVT_MASK_ABORT | ( EventBits_t ) PKTIO_EVT_MASK_RX_DATA ),
                                          pdTRUE,
                                          pdFALSE,
                                          portMAX_DELAY );

            if( ( uxBits & ( EventBits_t ) PKTIO_EVT_MASK_ABORT ) != 0U )
            {
                IotLogDebug( "Abort received, cleaning up!" );
                FREE_AT_RESPONSE_AND_SET_NULL( ( pAtResp ) );
                break;
            }
            else if( ( uxBits & ( EventBits_t ) PKTIO_EVT_MASK_RX_DATA ) != 0U )
            {
                /* Keep Reading until there is no more bytes in comm interface. */
                do
                {
                    bytesRead = _handleRxDataEvent( pContext, &pAtResp );
                } while( bytesRead != 0U );
            }
            else
            {
                /* Empty else to avoid MISRA violation */
            }
        }
        while( true );

        ( void ) pContext->pCommIntf->close( pContext->hPktioCommIntf );
        pContext->hPktioCommIntf = NULL;
        pContext->pPktioShutdownCB( pContext );
    }
    else
    {
        IotLogError( "Comm port open failed" );
    }

    if( pContext != NULL )
    {
        if( pContext->pPktioCommEvent != NULL )
        {
            ( void ) xEventGroupSetBits( pContext->pPktioCommEvent, PKTIO_EVT_MASK_ABORTED );
        }

        /* Call the shutdown callback if it is defined. */
        if( pContext->pPktioShutdownCB != NULL )
        {
            pContext->pPktioShutdownCB( pContext );
        }
    }
}

/*-----------------------------------------------------------*/

static void _PktioInitProcessReadThreadStatus( CellularContext_t * pContext )
{
    EventBits_t uxBits;

    uxBits = xEventGroupWaitBits( ( pContext->pPktioCommEvent ),
                                  ( ( EventBits_t ) PKTIO_EVT_MASK_STARTED | ( EventBits_t ) PKTIO_EVT_MASK_ABORTED ),
                                  pdTRUE,
                                  pdFALSE,
                                  ( ( TickType_t ) ~( 0UL ) ) );

    if( ( uxBits & ( EventBits_t ) PKTIO_EVT_MASK_ABORTED ) != PKTIO_EVT_MASK_ABORTED )
    {
        pContext->bPktioUp = true;
    }
}

/*-----------------------------------------------------------*/

void _Cellular_PktioSetShutdownCallback( CellularContext_t * pContext,
                                         _pPktioShutdownCallback_t shutdownCb )
{
    if( pContext != NULL )
    {
        pContext->pPktioShutdownCB = shutdownCb;
    }
}

/*-----------------------------------------------------------*/

CellularPktStatus_t _Cellular_PktioInit( CellularContext_t * pContext,
                                         _pPktioHandlePacketCallback_t handlePacketCb )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    bool status = false;

    if( pContext != NULL )
    {
        pContext->PktioAtCmdType = CELLULAR_AT_NO_COMMAND;
        pContext->pPktioCommEvent = xEventGroupCreate();

        if( pContext->pPktioCommEvent != NULL )
        {
            pContext->pPktioHandlepktCB = handlePacketCb;
            ( void ) xEventGroupClearBits( ( pContext->pPktioCommEvent ),
                                           ( ( EventBits_t ) PKTIO_EVT_MASK_ALL_EVENTS ) );

            /* Create the Read thread. */
            status = Iot_CreateDetachedThread( _pktioReadThread,
                                               ( void * ) pContext,
                                               IOT_THREAD_DEFAULT_PRIORITY,
                                               IOT_THREAD_DEFAULT_STACK_SIZE );

            if( status )
            {
                IotLogDebug( "Reader thread created" );
                _PktioInitProcessReadThreadStatus( pContext );
            }
            else
            {
                IotLogError( "Can't create reader thread" );
                pktStatus = CELLULAR_PKT_STATUS_CREATION_FAIL;
            }
        }

        else
        {
            IotLogError( "Can't create event group" );
            pktStatus = CELLULAR_PKT_STATUS_CREATION_FAIL;
        }

        if( pktStatus == CELLULAR_PKT_STATUS_OK )
        {
            IotLogDebug( "Thread create: read_thread status:%d", pktStatus );
        }
        else
        {
            pContext->pPktioHandlepktCB = NULL;

            if( pContext->pPktioCommEvent != NULL )
            {
                vEventGroupDelete( pContext->pPktioCommEvent );
                pContext->pPktioCommEvent = NULL;
            }
        }
    }
    else
    {
        pktStatus = ( CellularPktStatus_t ) CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* Sends the AT command to the modem. */
CellularPktStatus_t _Cellular_PktioSendAtCmd( CellularContext_t * pContext,
                                              const char * pAtCmd,
                                              CellularATCommandType_t atType,
                                              const char * pAtRspPrefix )
{
    uint32_t cmdLen = 0, newCmdLen = 0;
    uint32_t sentLen = 0;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    if( pContext != NULL )
    {
        pContext->pRespPrefix = pAtRspPrefix;
        pContext->PktioAtCmdType = atType;
        cmdLen = strnlen( pAtCmd, PKTIO_WRITE_BUFFER_SIZE - 1U );
        newCmdLen = cmdLen;
        newCmdLen += 1U; /* Include space for \r. */

        ( void ) strncpy( pContext->pktioSendBuf, pAtCmd, cmdLen );
        pContext->pktioSendBuf[ cmdLen ] = '\r';

        if( ( pContext->pCommIntf != NULL ) && ( pContext->hPktioCommIntf != NULL ) )
        {
            ( void ) pContext->pCommIntf->send( pContext->hPktioCommIntf, ( const uint8_t * ) &pContext->pktioSendBuf, newCmdLen, 1000UL, &sentLen );
        }
    }
    else
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* Sends data to the modem. */
uint32_t _Cellular_PktioSendData( CellularContext_t * pContext,
                                  const uint8_t * pData,
                                  uint32_t dataLen )
{
    uint32_t sentLen = 0;
    uint32_t sendDataLength = 0U;

    if( pContext != NULL )
    {
        if( dataLen < ( uint32_t ) PKTIO_WRITE_BUFFER_SIZE )
        {
            sendDataLength = dataLen;
        }
        else
        {
            sendDataLength = ( uint32_t ) ( ( uint32_t ) PKTIO_WRITE_BUFFER_SIZE - 1U );
        }

        ( void ) memcpy( ( void * ) pContext->pktioSendBuf, ( const void * ) pData, sendDataLength );

        if( ( pContext->pCommIntf != NULL ) && ( pContext->hPktioCommIntf != NULL ) )
        {
            ( void ) pContext->pCommIntf->send( pContext->hPktioCommIntf, ( const uint8_t * ) &pContext->pktioSendBuf,
                                                sendDataLength, 1000UL, &sentLen );
        }

        IotLogDebug( "PktioSendData sent %d bytes", sentLen );
    }

    return sentLen;
}

/*-----------------------------------------------------------*/

void _Cellular_PktioShutdown( CellularContext_t * pContext )
{
    EventBits_t uxBits;

    if( ( pContext != NULL ) && ( pContext->bPktioUp ) )
    {
        if( pContext->pPktioCommEvent != NULL )
        {
            ( void ) xEventGroupSetBits( pContext->pPktioCommEvent, PKTIO_EVT_MASK_ABORT );
            uxBits = xEventGroupGetBits( pContext->pPktioCommEvent );

            while( ( uxBits & ( EventBits_t ) PKTIO_EVT_MASK_ABORTED ) != ( ( EventBits_t ) PKTIO_EVT_MASK_ABORTED ) )
            {
                vTaskDelay( pdMS_TO_TICKS( 10 ) );
                uxBits = xEventGroupGetBits( pContext->pPktioCommEvent );
            }

            vEventGroupDelete( pContext->pPktioCommEvent );
            pContext->pPktioCommEvent = NULL;
        }

        pContext->pPktioHandlepktCB = NULL;
        pContext->bPktioUp = false;
    }
}

/*-----------------------------------------------------------*/
