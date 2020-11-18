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

#define FREE_AT_RESPONSE_AND_SET_NULL( pResp )    { ( _Cellular_AtResponseFree( ( pResp ) ) ); ( ( pResp ) = NULL ); }

#define PKTIO_SHUTDOWN_WAIT_INTERVAL_MS    ( 10U )

/*-----------------------------------------------------------*/

static void _saveData( char * pLine,
                       CellularATCommandResponse_t * pResp,
                       uint32_t dataLen );
static void _saveRawData( char * pLine,
                          CellularATCommandResponse_t * pResp,
                          uint32_t dataLen );
static void _saveATData( char * pLine,
                         CellularATCommandResponse_t * pResp );
static CellularPktStatus_t _processIntermediateResponse( char * pLine,
                                                         CellularATCommandResponse_t * pResp,
                                                         CellularATCommandType_t atType,
                                                         const char * pRespPrefix );
static CellularATCommandResponse_t * _Cellular_AtResponseNew( void );
static void _Cellular_AtResponseFree( CellularATCommandResponse_t * pResp );
static CellularPktStatus_t _Cellular_ProcessLine( const CellularContext_t * pContext,
                                                  char * pLine,
                                                  CellularATCommandResponse_t * pResp,
                                                  CellularATCommandType_t atType,
                                                  const char * pRespPrefix );
static bool urcTokenWoPrefix( const CellularContext_t * pContext,
                              const char * pLine );
static _atRespType_t _getMsgType( const CellularContext_t * pContext,
                                  const char * pLine,
                                  const char * pRespPrefix );
static CellularCommInterfaceError_t _Cellular_PktRxCallBack( void * pUserData,
                                                             CellularCommInterfaceHandle_t commInterfaceHandle );
static char * _handleLeftoverBuffer( CellularContext_t * pContext );
static char * _Cellular_ReadLine( CellularContext_t * pContext,
                                  uint32_t * pBytesRead,
                                  const CellularATCommandResponse_t * pAtResp );
static CellularPktStatus_t _handleData( char * pStartOfData,
                                        CellularContext_t * pContext,
                                        CellularATCommandResponse_t * pAtResp,
                                        char ** ppLine,
                                        uint32_t bytesRead,
                                        uint32_t * pBytesLeft );
static CellularPktStatus_t _handleMsgType( CellularContext_t * pContext,
                                           CellularATCommandResponse_t ** ppAtResp,
                                           char * pLine );
static void _handleAllReceived( CellularContext_t * pContext,
                                CellularATCommandResponse_t ** ppAtResp,
                                char * pData,
                                uint32_t bytesInBuffer );
static uint32_t _handleRxDataEvent( CellularContext_t * pContext,
                                    CellularATCommandResponse_t ** ppAtResp );
static void _pktioReadThread( void * pUserData );
static void _PktioInitProcessReadThreadStatus( CellularContext_t * pContext );

/*-----------------------------------------------------------*/

static uint32_t _convertCharPtrDistance( const char * pEndPtr,
                                         const char * pStartPtr )
{
    int32_t ptrDistance = ( pEndPtr - pStartPtr );
    uint32_t retValue = 0;

    if( ( ptrDistance >= 0 ) && ( ptrDistance < INT32_MAX ) )
    {
        retValue = ( uint32_t ) ptrDistance;
    }
    else
    {
        IotLogError( "pStartPtr is bigger then pEndPtr." );
        retValue = 0U;
    }

    return retValue;
}

/*-----------------------------------------------------------*/

static void _saveData( char * pLine,
                       CellularATCommandResponse_t * pResp,
                       uint32_t dataLen )
{
    CellularATCommandLine_t * pNew = NULL, * pTemp = NULL;

    IotLogDebug( "_saveData : Save data %p with length %d", pLine, dataLen );

    pNew = ( CellularATCommandLine_t * ) pvPortMalloc( sizeof( CellularATCommandLine_t ) );
    /* coverity[misra_c_2012_rule_10_5_violation] */
    configASSERT( ( int32_t ) ( pNew != NULL ) );

    /* Reuse the pktio buffer instead of allocate. */
    pNew->pLine = pLine;
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

static void _saveRawData( char * pLine,
                          CellularATCommandResponse_t * pResp,
                          uint32_t dataLen )
{
    IotLogDebug( "Save [%p] %d data to pResp", pLine, dataLen );
    _saveData( pLine, pResp, dataLen );
}

/*-----------------------------------------------------------*/

static void _saveATData( char * pLine,
                         CellularATCommandResponse_t * pResp )
{
    IotLogDebug( "Save [%s] %d AT data to pResp", pLine, strlen( pLine ) );
    _saveData( pLine, pResp, strlen( pLine ) + 1U );
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _processIntermediateResponse( char * pLine,
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
    CellularATCommandResponse_t * pNew = NULL;

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
    CellularATCommandLine_t * pCurrLine = NULL;
    CellularATCommandLine_t * pToFree = NULL;

    if( pResp != NULL )
    {
        pCurrLine = pResp->pItm;

        while( pCurrLine != NULL )
        {
            pToFree = pCurrLine;
            pCurrLine = pCurrLine->pNext;

            /* Ruese the pktiobuffer. No need to free pToFree->pLine here. */
            vPortFree( pToFree );
        }

        vPortFree( pResp );
    }
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _Cellular_ProcessLine( const CellularContext_t * pContext,
                                                  char * pLine,
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
            IotLogError( "Modem return ERROR: cmd: %s line: %s respPrefix: %s status: %d",
                         ( ( pContext->pCurrentCmd == NULL ) ? "NULL" : pContext->pCurrentCmd ),
                         pLine,
                         ( ( pRespPrefix == NULL ) ? "NULL" : pRespPrefix ),
                         pkStatus );
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
                ( pContext->PktioAtCmdType == CELLULAR_AT_WITH_PREFIX ) ||
                ( pContext->PktioAtCmdType == CELLULAR_AT_MULTI_WITH_PREFIX ) )
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
static CellularCommInterfaceError_t _Cellular_PktRxCallBack( void * pUserData,
                                                             CellularCommInterfaceHandle_t commInterfaceHandle )
{
    const CellularContext_t * pContext = ( CellularContext_t * ) pUserData;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult = pdFALSE;
    CellularCommInterfaceError_t retComm = IOT_COMM_INTERFACE_SUCCESS;

    ( void ) commInterfaceHandle; /* Comm if is not used in this function. */

    /* The context of this function is a ISR. */
    if( ( pContext == NULL ) || ( pContext->pPktioCommEvent == NULL ) )
    {
        retComm = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }
    else
    {
        xResult = xEventGroupSetBitsFromISR( pContext->pPktioCommEvent,
                                             PKTIO_EVT_MASK_RX_DATA,
                                             &xHigherPriorityTaskWoken );

        if( xResult == pdPASS )
        {
            if( xHigherPriorityTaskWoken == pdTRUE )
            {
                retComm = IOT_COMM_INTERFACE_SUCCESS;
            }
            else
            {
                retComm = IOT_COMM_INTERFACE_BUSY;
            }
        }
        else
        {
            retComm = IOT_COMM_INTERFACE_FAILURE;
        }
    }

    return retComm;
}

/*-----------------------------------------------------------*/

static char * _handleLeftoverBuffer( CellularContext_t * pContext )
{
    char * pRead = NULL; /* Pointer to first empty space in pContext->pktioReadBuf. */

    /* Move the leftover data or AT command response to the start of buffer.
     * Set the pRead pointer to the empty buffer space. */

    IotLogDebug( "moved the partial line/data from %p to %p %d",
                 pContext->pPktioReadPtr, pContext->pktioReadBuf, pContext->partialDataRcvdLen );

    ( void ) memmove( pContext->pktioReadBuf, pContext->pPktioReadPtr, pContext->partialDataRcvdLen );
    pContext->pktioReadBuf[ pContext->partialDataRcvdLen ] = '\0';

    pRead = &pContext->pktioReadBuf[ pContext->partialDataRcvdLen ];

    pContext->pPktioReadPtr = pContext->pktioReadBuf;

    return pRead;
}

/*-----------------------------------------------------------*/

/* pBytesRead : bytes read from comm interface. */
/* partialData : leftover bytes in the pktioreadbuf. Not enougth to be a command. */
static char * _Cellular_ReadLine( CellularContext_t * pContext,
                                  uint32_t * pBytesRead,
                                  const CellularATCommandResponse_t * pAtResp )
{
    char * pAtBuf = NULL; /* The returned start of data. */
    char * pRead = NULL;  /* pRead is the first empty ptr in the Buffer for comm intf to read. */
    uint32_t bytesRead = 0;
    uint32_t partialDataRead = pContext->partialDataRcvdLen;
    uint32_t bufferEmptyLength = PKTIO_READ_BUFFER_SIZE;

    pAtBuf = pContext->pktioReadBuf;
    pRead = pContext->pktioReadBuf;

    /* pContext->pPktioReadPtr is valid data start pointer.
     * pContext->partialDataRcvdLen is the valid data length need to be handled.
     * if pContext->pPktioReadPtr is NULL, valid data start from pContext->pktioReadBuf.
     * pAtResp equals NULL indicate that no data is buffered in AT command response and
     * data before pPktioReadPtr is invalid data can be recycled. */
    if( ( pContext->pPktioReadPtr != NULL ) && ( pContext->pPktioReadPtr != pContext->pktioReadBuf ) &&
        ( pContext->partialDataRcvdLen != 0U ) && ( pAtResp == NULL ) )
    {
        pRead = _handleLeftoverBuffer( pContext );
        bufferEmptyLength = PKTIO_READ_BUFFER_SIZE - pContext->partialDataRcvdLen;
    }
    else
    {
        if( pContext->pPktioReadPtr != NULL )
        {
            /* There are still valid data before pPktioReadPtr. */
            pRead = &pContext->pPktioReadPtr[ pContext->partialDataRcvdLen ];
            pAtBuf = pContext->pPktioReadPtr;
            bufferEmptyLength = PKTIO_READ_BUFFER_SIZE -
                                pContext->partialDataRcvdLen - _convertCharPtrDistance( pContext->pPktioReadPtr, pContext->pktioReadBuf );
        }
        else
        {
            /* There are valid data need to be handled with length pContext->partialDataRcvdLen. */
            pRead = &pContext->pktioReadBuf[ pContext->partialDataRcvdLen ];
            pAtBuf = pContext->pktioReadBuf;
            bufferEmptyLength = PKTIO_READ_BUFFER_SIZE - pContext->partialDataRcvdLen;
        }
    }

    if( bufferEmptyLength > 0U )
    {
        ( void ) pContext->pCommIntf->recv( pContext->hPktioCommIntf, ( uint8_t * ) pRead,
                                            bufferEmptyLength,
                                            CELLULAR_COMM_IF_RECV_TIMEOUT_MS, &bytesRead );

        if( bytesRead > 0U )
        {
            /* Add a NULL after the bytesRead. This is required for further processing. */
            pRead[ bytesRead ] = '\0';

            IotLogDebug( "AT Read %d bytes, data[%p]", bytesRead, pRead );
            /* Set the pBytesRead only when actual bytes read from comm interface. */
            *pBytesRead = bytesRead + partialDataRead;

            /* Clean the partial data and read pointer. */
            pContext->partialDataRcvdLen = 0;
        }
        else
        {
            pAtBuf = NULL;
            *pBytesRead = 0U;
        }
    }
    else
    {
        IotLogError( "No empty space for comm if to read. Try handle data again." );
        *pBytesRead = partialDataRead;
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
    /* Calculate the size of data received so far. */
    uint32_t bytesBeforeData = _convertCharPtrDistance( pStartOfData, *ppLine );
    CellularPktStatus_t pkStatus = CELLULAR_PKT_STATUS_OK;
    uint32_t bytesDataAndLeft = 0;

    /* Bytes before pStartOfData is not data, skip the bytes received. */
    /* bytesRead = bytesBeforeData( data prefix ) + bytesData + bytesLeft( other AT command response ). */
    bytesDataAndLeft = bytesRead - bytesBeforeData;

    if( bytesDataAndLeft >= pContext->dataLength )
    {
        /* Add data to the response linked list. */
        _saveRawData( pStartOfData, pAtResp, pContext->dataLength );

        /* Advance pLine to a point after data. */
        *ppLine = &pStartOfData[ pContext->dataLength ];

        /* There are more bytes after the data. */
        *pBytesLeft = ( bytesDataAndLeft - pContext->dataLength );

        IotLogDebug( "_handleData : read buffer buffer %p start %p prefix %d left %d, read total %d",
                     pContext->pktioReadBuf,
                     pStartOfData,
                     bytesBeforeData,
                     *pBytesLeft,
                     bytesRead );

        /* reset the data related variables. */
        pContext->dataLength = 0U;

        /* Set the pPktioReadPtr to indicate data already handled. */
        pContext->pPktioReadPtr = *ppLine;
        pContext->partialDataRcvdLen = *pBytesLeft;
    }
    else
    {
        /* The data received is partial. Store the start of data in read pointer. */
        pContext->pPktioReadPtr = pStartOfData;
        pContext->partialDataRcvdLen = bytesDataAndLeft;
        pkStatus = CELLULAR_PKT_STATUS_PENDING_DATA_BUFFER;
    }

    return pkStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _handleMsgType( CellularContext_t * pContext,
                                           CellularATCommandResponse_t ** ppAtResp,
                                           char * pLine )
{
    CellularPktStatus_t pkStatus = CELLULAR_PKT_STATUS_OK;

    if( pContext->recvdMsgType == AT_UNSOLICITED )
    {
        if( pContext->pPktioHandlepktCB != NULL )
        {
            pContext->pPktioHandlepktCB( pContext, AT_UNSOLICITED, pLine );
        }
    }
    else if( pContext->recvdMsgType == AT_SOLICITED )
    {
        if( *ppAtResp == NULL )
        {
            *ppAtResp = _Cellular_AtResponseNew();
            IotLogDebug( "Allocat at response %p", *ppAtResp );
        }

        IotLogDebug( "AT solicited Resp[%s]", pLine );

        /* Process Line will store the Line data in AT response. */
        pkStatus = _Cellular_ProcessLine( pContext, pLine, *ppAtResp, pContext->PktioAtCmdType, pContext->pRespPrefix );

        if( pkStatus == CELLULAR_PKT_STATUS_OK )
        {
            if( pContext->pPktioHandlepktCB != NULL )
            {
                pContext->pPktioHandlepktCB( pContext, AT_SOLICITED, *ppAtResp );
            }

            FREE_AT_RESPONSE_AND_SET_NULL( *ppAtResp );
        }
        else if( pkStatus == CELLULAR_PKT_STATUS_PENDING_DATA_BUFFER )
        {
            /* Check data prefix first then store the data if this command has data response. */
        }
        else
        {
            if( pkStatus != CELLULAR_PKT_STATUS_PENDING_DATA )
            {
                ( void ) memset( pContext->pktioReadBuf, 0, PKTIO_READ_BUFFER_SIZE + 1U );
                pContext->pPktioReadPtr = NULL;
                FREE_AT_RESPONSE_AND_SET_NULL( *ppAtResp );
                IotLogError( "processLine ERROR, cleaning up! Current command %s",
                             ( ( pContext->pCurrentCmd == NULL ) ? "NULL" : pContext->pCurrentCmd ) );
            }
        }
    }
    else
    {
        IotLogError( "recvdMsgType is AT_UNDEFINED for Message: line %s, cmd %s",
                     pLine, ( ( pContext->pCurrentCmd == NULL ) ? "NULL" : pContext->pCurrentCmd ) );
        ( void ) memset( pContext->pktioReadBuf, 0, PKTIO_READ_BUFFER_SIZE + 1U );
        pContext->pPktioReadPtr = NULL;
        pContext->partialDataRcvdLen = 0;
        FREE_AT_RESPONSE_AND_SET_NULL( *ppAtResp );
        pkStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }

    return pkStatus;
}

/*-----------------------------------------------------------*/

static bool _findLineInStream( CellularContext_t * pContext,
                               char * pLine,
                               uint32_t bytesRead )
{
    bool keepProcess = true;
    char * pTempLine = pLine;
    uint32_t i = 0;

    /* Handle the complete line here. GetMsgType needs a complete Line or longer then maximum prefix line. */
    for( i = 0; i < bytesRead; i++ )
    {
        if( ( pTempLine[ i ] == '\0' ) || ( pTempLine[ i ] == '\r' ) || ( pTempLine[ i ] == '\n' ) )
        {
            break;
        }
    }

    /* A complete Line is found. */
    if( i < bytesRead )
    {
        pTempLine[ i ] = '\0';
    }
    else
    {
        IotLogDebug( "%p is not a complete line", pTempLine );
        pContext->pPktioReadPtr = pTempLine;
        pContext->partialDataRcvdLen = bytesRead;
        keepProcess = false;
    }

    return keepProcess;
}

/*-----------------------------------------------------------*/

static bool _preprocessLine( CellularContext_t * pContext,
                             char * pLine,
                             uint32_t * pBytesRead,
                             char ** ppStartOfData )
{
    char * pTempLine = pLine;
    bool keepProcess = true;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    /* The line only has change line. */
    if( *pBytesRead <= 0U )
    {
        pContext->pPktioReadPtr = pTempLine;
        pContext->partialDataRcvdLen = 0;
        keepProcess = false;
    }
    else
    {
        if( pContext->pktDataSendPrefixCB != NULL )
        {
            /* Check if the AT command response is the data send prefix.
             * Data send prefix is an SRC success token for data send AT commmand.
             * It is used to indicate modem can receive data now. */
            /* This function may fix the data stream if the data send prefix is not a line. */
            pktStatus = pContext->pktDataSendPrefixCB( pContext->pDataSendPrefixCBContext, pTempLine, pBytesRead );

            if( pktStatus != CELLULAR_PKT_STATUS_OK )
            {
                IotLogError( "pktDataSendPrefixCB returns error %d", pktStatus );
                keepProcess = false;
            }
        }
        else if( pContext->pktDataPrefixCB != NULL )
        {
            /* Check if the AT command response is the data receive prefix.
             * Data receive prefix is an SRC success token for data receive AT commnad.
             * It is used to indicate modem will start to send data. Don't parse the content. */
            /* ppStartOfData and pDataLength are not set if this function failed. */

            /* This function may fix the data stream if the AT response and data
             * received are in the same line. */
            pktStatus = pContext->pktDataPrefixCB( pContext->pDataPrefixCBContext,
                                                   pTempLine, *pBytesRead,
                                                   ppStartOfData, &pContext->dataLength );

            if( pktStatus == CELLULAR_PKT_STATUS_SIZE_MISMATCH )
            {
                /* The modem driver is waiting for more data to decide. */
                IotLogDebug( "%p is not a complete line", pTempLine );
                pContext->pPktioReadPtr = pTempLine;
                pContext->partialDataRcvdLen = *pBytesRead;
                keepProcess = false;
            }
            else
            {
                if( pktStatus != CELLULAR_PKT_STATUS_OK )
                {
                    IotLogError( "pktDataPrefixCB returns error %d", pktStatus );
                    keepProcess = false;
                }
            }
        }
        else
        {
            /* This is the case AT command don't need data send or data receive prefix. */
            /* MISRA empty else. */
        }

        if( keepProcess == true )
        {
            keepProcess = _findLineInStream( pContext, pTempLine, *pBytesRead );
        }
    }

    return keepProcess;
}

/*-----------------------------------------------------------*/

static bool _handleDataResult( CellularContext_t * pContext,
                               CellularATCommandResponse_t * const * ppAtResp,
                               char * pStartOfData,
                               char ** ppLine,
                               uint32_t * pBytesRead )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    uint32_t bytesLeft = 0;
    bool keepProcess = true;

    /* The input line is a data recv command. Handle the data buffer. */
    pktStatus = _handleData( pStartOfData, pContext, *ppAtResp, ppLine, *pBytesRead, &bytesLeft );

    if( pktStatus == CELLULAR_PKT_STATUS_PENDING_DATA_BUFFER )
    {
        IotLogDebug( "Partial Data received %d, waiting for more data", *pBytesRead );
        keepProcess = false;
    }
    else if( bytesLeft == 0U )
    {
        IotLogDebug( "Complete Data received" );
        keepProcess = false;
    }
    else
    {
        *pBytesRead = bytesLeft;
        IotLogDebug( "_handleData okay, keep processing %u bytes %p", bytesLeft, *ppLine );
    }

    return keepProcess;
}

/*-----------------------------------------------------------*/

static bool _getNextLine( CellularContext_t * pContext,
                          char ** ppLine,
                          uint32_t * pBytesRead,
                          CellularPktStatus_t pktStatus )
{
    uint32_t stringLength = 0;
    bool keepProcess = true;

    /* Find other responses or urcs which need to be processed in this read buffer. */
    stringLength = strnlen( *ppLine, *pBytesRead );

    /* Advanced 1 bytes to read next Line. */
    if( *pBytesRead >= ( stringLength + 1U ) )
    {
        *ppLine = &( ( *ppLine )[ ( stringLength + 1U ) ] );
        *pBytesRead = *pBytesRead - ( stringLength + 1U );
        pContext->pPktioReadPtr = *ppLine;
        pContext->partialDataRcvdLen = *pBytesRead;

        if( ( pktStatus == CELLULAR_PKT_STATUS_OK ) && ( pContext->recvdMsgType == AT_SOLICITED ) )
        {
            /* Garbage collection. */
            IotLogDebug( "Garbage collection" );
            ( void ) memmove( pContext->pktioReadBuf, *ppLine, *pBytesRead );
            *ppLine = pContext->pktioReadBuf;
            pContext->pPktioReadPtr = pContext->pktioReadBuf;
            pContext->partialDataRcvdLen = *pBytesRead;
        }
    }
    else
    {
        /* No complete line for parsing. */
        pContext->pPktioReadPtr = *ppLine;
        pContext->partialDataRcvdLen = *pBytesRead;
        keepProcess = false;
    }

    return keepProcess;
}

/*-----------------------------------------------------------*/

/* This function handle message in line( string terminated with \r or \n ). */
static void _handleAllReceived( CellularContext_t * pContext,
                                CellularATCommandResponse_t ** ppAtResp,
                                char * pData,
                                uint32_t bytesInBuffer )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    char * pStartOfData = NULL, * pTempLine = pData;
    uint32_t bytesRead = bytesInBuffer;
    bool keepProcess = true;

    while( keepProcess == true )
    {
        /* Pktio is reading command. Skip over the change line. */
        while( ( ( *pTempLine == '\r' ) || ( *pTempLine == '\n' ) ) && ( bytesRead > 0U ) )
        {
            pTempLine++;
            bytesRead = bytesRead - 1U;
        }

        /* Preprocess line. */
        keepProcess = _preprocessLine( pContext, pTempLine, &bytesRead, &pStartOfData );

        if( keepProcess == true )
        {
            /* A complete Line received. Get the message type. */
            pContext->recvdMsgType = _getMsgType( pContext, pTempLine, pContext->pRespPrefix );

            /* Handle the message according the received message type. */
            pktStatus = _handleMsgType( pContext, ppAtResp, pTempLine );

            if( pktStatus == CELLULAR_PKT_STATUS_PENDING_DATA_BUFFER )
            {
                /* The input line is a data recv command. Handle the data buffer. */
                if( ( pContext->dataLength != 0U ) && ( pStartOfData != NULL ) )
                {
                    keepProcess = _handleDataResult( pContext, ppAtResp, pStartOfData, &pTempLine, &bytesRead );
                }
                else
                {
                    keepProcess = _getNextLine( pContext, &pTempLine, &bytesRead, pktStatus );
                }
            }
            else if( ( pktStatus == CELLULAR_PKT_STATUS_OK ) || ( pktStatus == CELLULAR_PKT_STATUS_PENDING_DATA ) )
            {
                /* Process AT reponse success. Get the next Line. */
                keepProcess = _getNextLine( pContext, &pTempLine, &bytesRead, pktStatus );
            }
            else
            {
                IotLogDebug( "_handleMsgType failed %d", pktStatus );
                keepProcess = false;
            }
        }
    }
}

/*-----------------------------------------------------------*/

static uint32_t _handleRxDataEvent( CellularContext_t * pContext,
                                    CellularATCommandResponse_t ** ppAtResp )
{
    char * pLine = NULL;
    uint32_t bytesRead = 0;
    uint32_t bytesLeft = 0;

    /* Return the first line, may be more lines in buffer. */
    /* Start from pLine there are bytesRead bytes. */
    pLine = _Cellular_ReadLine( pContext, &bytesRead, *ppAtResp );

    if( ( bytesRead > 0U ) && ( pLine != NULL ) )
    {
        if( pContext->dataLength != 0U )
        {
            ( void ) _handleData( pLine, pContext, *ppAtResp, &pLine, bytesRead, &bytesLeft );
        }
        else
        {
            bytesLeft = bytesRead;
        }

        /* If bytesRead in _Cellular_ReadLine is all for data, don't parse the AT command response. */
        if( bytesLeft > 0U )
        {
            /* Add the null terminated char to the end of pLine. */
            pLine[ bytesLeft ] = '\0';
            _handleAllReceived( pContext, ppAtResp, pLine, bytesLeft );
        }
    }

    /* Return the bytes read from comm interface to do more reading. */
    return bytesRead;
}

/*-----------------------------------------------------------*/

static void _pktioReadThread( void * pUserData )
{
    CellularContext_t * pContext = ( CellularContext_t * ) pUserData;
    CellularATCommandResponse_t * pAtResp = NULL;
    EventBits_t uxBits = 0;
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
                FREE_AT_RESPONSE_AND_SET_NULL( pAtResp );
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
    EventBits_t uxBits = 0;

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

    if( pContext == NULL )
    {
        IotLogError( "_Cellular_PktioSendAtCmd : invalid cellular context" );
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if( ( pContext->pCommIntf == NULL ) || ( pContext->hPktioCommIntf == NULL ) )
    {
        IotLogError( "_Cellular_PktioSendAtCmd : invalid comm interface handle" );
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if( pAtCmd == NULL )
    {
        IotLogError( "_Cellular_PktioSendAtCmd : invalid pAtCmd" );
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
        cmdLen = strlen( pAtCmd );

        if( cmdLen > PKTIO_WRITE_BUFFER_SIZE )
        {
            IotLogError( "_Cellular_PktioSendAtCmd : invalid pAtCmd" );
            pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
        }
        else
        {
            pContext->pRespPrefix = pAtRspPrefix;
            pContext->PktioAtCmdType = atType;
            newCmdLen = cmdLen;
            newCmdLen += 1U; /* Include space for \r. */

            ( void ) strncpy( pContext->pktioSendBuf, pAtCmd, cmdLen );
            pContext->pktioSendBuf[ cmdLen ] = '\r';

            ( void ) pContext->pCommIntf->send( pContext->hPktioCommIntf,
                                                ( const uint8_t * ) &pContext->pktioSendBuf, newCmdLen,
                                                CELLULAR_COMM_IF_SEND_TIMEOUT_MS, &sentLen );
        }
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

    if( pContext == NULL )
    {
        IotLogError( "_Cellular_PktioSendData : invalid cellular context" );
    }
    else if( ( pContext->pCommIntf == NULL ) || ( pContext->hPktioCommIntf == NULL ) )
    {
        IotLogError( "_Cellular_PktioSendData : invalid comm interface handle" );
    }
    else if( pData == NULL )
    {
        IotLogError( "_Cellular_PktioSendData : invalid pData" );
    }
    else
    {
        ( void ) pContext->pCommIntf->send( pContext->hPktioCommIntf, pData,
                                            dataLen, CELLULAR_COMM_IF_SEND_TIMEOUT_MS, &sentLen );
    }

    IotLogDebug( "PktioSendData sent %d bytes", sentLen );
    return sentLen;
}

/*-----------------------------------------------------------*/

void _Cellular_PktioShutdown( CellularContext_t * pContext )
{
    EventBits_t uxBits = 0;

    if( ( pContext != NULL ) && ( pContext->bPktioUp ) )
    {
        if( pContext->pPktioCommEvent != NULL )
        {
            ( void ) xEventGroupSetBits( pContext->pPktioCommEvent, PKTIO_EVT_MASK_ABORT );
            uxBits = xEventGroupGetBits( pContext->pPktioCommEvent );

            while( ( uxBits & ( EventBits_t ) PKTIO_EVT_MASK_ABORTED ) != ( ( EventBits_t ) PKTIO_EVT_MASK_ABORTED ) )
            {
                vTaskDelay( pdMS_TO_TICKS( PKTIO_SHUTDOWN_WAIT_INTERVAL_MS ) );
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
