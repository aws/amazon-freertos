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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Cellular includes. */
#include "cellular_types.h"
#include "cellular_api.h"
#include "cellular_common_api.h"
#include "cellular_common.h"
#include "cellular_at_core.h"

/* Cellular module incliudes. */
#include "cellular_r4.h"

/*-----------------------------------------------------------*/

/* TODO : confirm the value. */
#define PDN_ACT_PACKET_REQ_TIMEOUT_MS           ( 150000UL )

#define PDN_DEACT_PACKET_REQ_TIMEOUT_MS         ( 40000UL )

#define GPRS_ATTACH_REQ_TIMEOUT_MS              ( 180000UL )

#define DNS_QUERY_REQ_TIMEOUT_MS                ( 120000UL )

#define SOCKET_CLOSE_PACKET_REQ_TIMEOUT_MS      ( 120000U )

#define SOCKET_CONNECT_PACKET_REQ_TIMEOUT_MS    ( 120000U )

#define CELLULAR_AT_CMD_TYPICAL_MAX_SIZE        ( 32U )

#define DATA_SEND_TIMEOUT_MS                    ( 10000U )

#define PACKET_REQ_TIMEOUT_MS                   ( 10000U )

#define DATA_READ_TIMEOUT_MS                    ( 50000UL )

#define SOCKET_DATA_PREFIX_TOKEN                "+USORD: "
#define SOCKET_DATA_PREFIX_TOKEN_LEN            ( 8U )
#define SOCKET_DATA_PREFIX_STRING_LENGTH        ( SOCKET_DATA_PREFIX_TOKEN_LEN + 9U )       /* +USORD: <socket_number>,<length>,\" ./
                                                                                             *
                                                                                             #define USOCR_PROTOCOL_TCP                            ( 6U )
                                                                                             #define USOCR_PROTOCOL_UDP                            ( 17U )
                                                                                             *
                                                                                             * /*-----------------------------------------------------------*/
#define RAT_PRIOIRTY_LIST_LENGTH                ( 3U )

/**
 * @brief Parameters involved in receiving data through sockets
 */
typedef struct _socketDataRecv
{
    uint32_t * pDataLen;
    uint8_t * pData;
} _socketDataRecv_t;

/*-----------------------------------------------------------*/

static CellularPktStatus_t socketRecvDataPrefix( void * pCallbackContext,
                                                 char * pLine,
                                                 uint32_t lineLength,
                                                 char ** ppDataStart,
                                                 uint32_t * pDataLength );
static CellularATError_t getDataFromResp( const CellularATCommandResponse_t * pAtResp,
                                          const _socketDataRecv_t * pDataRecv,
                                          uint32_t outBufSize );
static CellularPktStatus_t _Cellular_RecvFuncData( CellularContext_t * pContext,
                                                   const CellularATCommandResponse_t * pAtResp,
                                                   void * pData,
                                                   uint16_t dataLen );
static CellularError_t buildSocketConfig( CellularSocketHandle_t socketHandle,
                                          char * pCmdBuf );
static CellularError_t storeAccessModeAndAddress( CellularContext_t * pContext,
                                                  CellularSocketHandle_t socketHandle,
                                                  CellularSocketAccessMode_t dataAccessMode,
                                                  const CellularSocketAddress_t * pRemoteSocketAddress );
static CellularPktStatus_t _Cellular_RecvFuncGetSocketId( CellularContext_t * pContext,
                                                          const CellularATCommandResponse_t * pAtResp,
                                                          void * pData,
                                                          uint16_t dataLen );
static CellularError_t _Cellular_GetSocketNumber( CellularHandle_t cellularHandle,
                                                  CellularSocketHandle_t socketHandle,
                                                  uint8_t * pSessionId );

CellularError_t Cellular_SetPdnConfig( CellularHandle_t cellularHandle,
                                       uint8_t contextId,
                                       const CellularPdnConfig_t * pPdnConfig );

static CellularError_t _Cellular_GetPacketSwitchStatus( CellularHandle_t cellularHandle,
                                                        bool * pPacketSwitchStatus );

static CellularError_t _Cellular_GetContextActivationStatus( CellularHandle_t cellularHandle,
                                                             CellularPdnContextActInfo_t * pPdpContextsActInfo );

static bool _parseExtendedSignalQuality( char * pQcsqPayload,
                                         CellularSignalInfo_t * pSignalInfo );
static bool _parseSignalQuality( char * pQcsqPayload,
                                 CellularSignalInfo_t * pSignalInfo );
static CellularPktStatus_t _Cellular_RecvFuncGetSignalInfo( CellularContext_t * pContext,
                                                            const CellularATCommandResponse_t * pAtResp,
                                                            void * pData,
                                                            uint16_t dataLen );
static CellularError_t controlSignalStrengthIndication( CellularContext_t * pContext,
                                                        bool enable );

CellularError_t Cellular_SetPsmSettings( CellularHandle_t cellularHandle,
                                         const CellularPsmSettings_t * pPsmSettings );

/*-----------------------------------------------------------*/

static CellularPktStatus_t socketRecvDataPrefix( void * pCallbackContext,
                                                 char * pLine,
                                                 uint32_t lineLength,
                                                 char ** ppDataStart,
                                                 uint32_t * pDataLength )
{
    char * pToken = NULL;
    uint32_t tempStrlen = 0;
    int32_t tempValue = 0;
    CellularATError_t atResult = CELLULAR_AT_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    char pLocalLine[ SOCKET_DATA_PREFIX_STRING_LENGTH + 1 ] = { '\0' };
    char * pDataStart = pLocalLine;

    /* pCallbackContext is not used in this function. It should be NULL. */
    if( ( pLine == NULL ) || ( ppDataStart == NULL ) || ( pDataLength == NULL ) || ( pCallbackContext != NULL ) )
    {
        IotLogError( "socketRecvData: Bad parameters" );
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if( strncmp( pLine, SOCKET_DATA_PREFIX_TOKEN, SOCKET_DATA_PREFIX_TOKEN_LEN ) != 0 )
    {
        /* Prefix string which is not SOCKET_DATA_PREFIX_TOKEN does't indicate data start.
         * Set data length to 0.*/
        *ppDataStart = NULL;
        *pDataLength = 0;
    }
    else
    {
        /* The string length of "+USORD: <socket>[,<length>],"<data in text mode>". */
        strncpy( pLocalLine, pLine, SOCKET_DATA_PREFIX_STRING_LENGTH );
        pDataStart = &pLocalLine[ SOCKET_DATA_PREFIX_TOKEN_LEN ];

        /* Skip the socket number. */
        atResult = Cellular_ATGetNextTok( &pDataStart, &pToken );

        /* Parse the receive data length. */
        if( ( atResult == CELLULAR_AT_SUCCESS ) && ( pDataStart[ 0 ] == '"' ) )
        {
            /* Peek the next symbol. If it is \", then length is ommited. Received byte is 0. */
            IotLogDebug( "socketRecvData: 0 received bytes." );
            *pDataLength = 0U;
            *ppDataStart = NULL;
        }
        else if( atResult == CELLULAR_AT_SUCCESS )
        {
            /* Parse the length. */
            atResult = Cellular_ATGetNextTok( &pDataStart, &pToken );

            if( atResult == CELLULAR_AT_SUCCESS )
            {
                atResult = Cellular_ATStrtoi( pToken, 10, &tempValue );

                if( ( tempValue < 0 ) && ( tempValue > CELLULAR_MAX_RECV_DATA_LEN ) )
                {
                    IotLogError( "socketRecvData: Bad parameters" );
                    atResult = CELLULAR_AT_ERROR;
                }
            }

            if( atResult == CELLULAR_AT_SUCCESS )
            {
                /* Change +USROD: <socket>,<length>," to +USROD: <socket>,<length>,\r. */
                pDataStart = &pLine[ pDataStart - pLocalLine ];

                if( pDataStart[ 0 ] == '\"' )
                {
                    *pDataStart = '\r';
                    *ppDataStart = ( char * ) &pDataStart[ 1 ];
                    *pDataLength = ( uint32_t ) tempValue;
                }
            }
        }
        else
        {
            IotLogDebug( "socketRecvDataPrefix : pLine [%s] with lineLength [%d] is not data prefix", pLine, lineLength );
        }
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

static CellularATError_t getDataFromResp( const CellularATCommandResponse_t * pAtResp,
                                          const _socketDataRecv_t * pDataRecv,
                                          uint32_t outBufSize )
{
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    const char * pInputLine = NULL;
    uint32_t dataLenToCopy = 0;

    /* Check if the received data size is greater than the output buffer size. */
    if( *pDataRecv->pDataLen > outBufSize )
    {
        IotLogError( "Data is turncated, received data length %d, out buffer size %d",
                     *pDataRecv->pDataLen, outBufSize );
        dataLenToCopy = outBufSize;
        *pDataRecv->pDataLen = outBufSize;
    }
    else
    {
        dataLenToCopy = *pDataRecv->pDataLen;
    }

    /* Data is stored in the next intermediate response. */
    if( pAtResp->pItm->pNext != NULL )
    {
        pInputLine = pAtResp->pItm->pNext->pLine;

        if( ( pInputLine != NULL ) && ( dataLenToCopy > 0U ) )
        {
            /* Copy the data to the out buffer. */
            ( void ) memcpy( ( void * ) pDataRecv->pData, ( const void * ) pInputLine, dataLenToCopy );
        }
        else
        {
            IotLogError( "Receive Data: paramerter error, data pointer %p, data to copy %d",
                         pInputLine, dataLenToCopy );
            atCoreStatus = CELLULAR_AT_BAD_PARAMETER;
        }
    }
    else if( *pDataRecv->pDataLen == 0U )
    {
        /* Receive command success but no data. */
        IotLogDebug( "Receive Data: no data" );
    }
    else
    {
        IotLogError( "Receive Data: Intermediate response empty" );
        atCoreStatus = CELLULAR_AT_BAD_PARAMETER;
    }

    return atCoreStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL types. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static CellularPktStatus_t _Cellular_RecvFuncData( CellularContext_t * pContext,
                                                   const CellularATCommandResponse_t * pAtResp,
                                                   void * pData,
                                                   uint16_t dataLen )
{
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    char * pInputLine = NULL, * pToken = NULL;
    const _socketDataRecv_t * pDataRecv = ( _socketDataRecv_t * ) pData;
    int32_t tempValue = 0;

    if( pContext == NULL )
    {
        IotLogError( "Receive Data: invalid context" );
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if( ( pAtResp == NULL ) || ( pAtResp->pItm == NULL ) || ( pAtResp->pItm->pLine == NULL ) )
    {
        IotLogError( "Receive Data: response is invalid" );
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if( ( pDataRecv == NULL ) || ( pDataRecv->pData == NULL ) || ( pDataRecv->pDataLen == NULL ) )
    {
        IotLogError( "Receive Data: Bad parameters" );
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
        pInputLine = pAtResp->pItm->pLine; /* The first item is the data prefix. */

        /* Check the data prefix token "+USORD: ". */
        if( strncmp( pInputLine, SOCKET_DATA_PREFIX_TOKEN, SOCKET_DATA_PREFIX_TOKEN_LEN ) != 0 )
        {
            IotLogError( "response item error in prefix CONNECT" );
            atCoreStatus = CELLULAR_AT_ERROR;
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( pAtResp->pItm->pNext == NULL )
            {
                /* No data response. */
                *pDataRecv->pDataLen = 0;
            }
            else
            {
                /* Parse the data length. */
                atCoreStatus = Cellular_ATGetNextTok( &pInputLine, &pToken );

                if( atCoreStatus == CELLULAR_AT_SUCCESS )
                {
                    atCoreStatus = Cellular_ATGetNextTok( &pInputLine, &pToken );
                }

                if( atCoreStatus == CELLULAR_AT_SUCCESS )
                {
                    atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

                    if( tempValue >= 0 )
                    {
                        *pDataRecv->pDataLen = ( uint32_t ) tempValue;
                    }
                    else
                    {
                        atCoreStatus = CELLULAR_AT_ERROR;
                    }
                }

                /* Process the data buffer. */
                if( atCoreStatus == CELLULAR_AT_SUCCESS )
                {
                    atCoreStatus = getDataFromResp( pAtResp, pDataRecv, *pDataRecv->pDataLen );
                }
            }
        }

        pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

static CellularError_t buildSocketConfig( CellularSocketHandle_t socketHandle,
                                          char * pCmdBuf )
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    if( pCmdBuf == NULL )
    {
        IotLogDebug( "buildSocketConfig: Invalid command buffer" );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else if( socketHandle->socketProtocol != CELLULAR_SOCKET_PROTOCOL_TCP )
    {
        IotLogError( "buildSocketConfig: socket protocol unsupported %d",
                     socketHandle->socketProtocol );
        cellularStatus = CELLULAR_UNSUPPORTED;
    }
    else
    {
        /* Form the AT command. The socket number is returned in response.
         * AT+USOCR=<protocol>[,<local_port>[,<IP_type>]]
         * protocol=6 => TCP, local_port=0 => random port. */

        /* The return value of snprintf is not used.
         * The max length of the string is fixed and checked offline. */
        /* coverity[misra_c_2012_rule_21_6_violation]. */
        ( void ) snprintf( pCmdBuf, CELLULAR_AT_CMD_MAX_SIZE,
                           "AT+USOCR=6,0" );
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

static CellularError_t storeAccessModeAndAddress( CellularContext_t * pContext,
                                                  CellularSocketHandle_t socketHandle,
                                                  CellularSocketAccessMode_t dataAccessMode,
                                                  const CellularSocketAddress_t * pRemoteSocketAddress )
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else if( ( pRemoteSocketAddress == NULL ) || ( socketHandle == NULL ) )
    {
        IotLogDebug( "storeAccessModeAndAddress: Invalid socket address" );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else if( socketHandle->socketState != SOCKETSTATE_ALLOCATED )
    {
        IotLogError( "storeAccessModeAndAddress, bad socket state %d",
                     socketHandle->socketState );
        cellularStatus = CELLULAR_INTERNAL_FAILURE;
    }
    else if( dataAccessMode != CELLULAR_ACCESSMODE_BUFFER )
    {
        IotLogError( "storeAccessModeAndAddress, Access mode not supported %d",
                     dataAccessMode );
        cellularStatus = CELLULAR_UNSUPPORTED;
    }
    else
    {
        socketHandle->remoteSocketAddress.port = pRemoteSocketAddress->port;
        socketHandle->dataMode = dataAccessMode;
        socketHandle->remoteSocketAddress.ipAddress.ipAddressType =
            pRemoteSocketAddress->ipAddress.ipAddressType;
        ( void ) strncpy( socketHandle->remoteSocketAddress.ipAddress.ipAddress,
                          pRemoteSocketAddress->ipAddress.ipAddress,
                          CELLULAR_IP_ADDRESS_MAX_SIZE + 1U );
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL types. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static CellularPktStatus_t _Cellular_RecvFuncGetSocketId( CellularContext_t * pContext,
                                                          const CellularATCommandResponse_t * pAtResp,
                                                          void * pData,
                                                          uint16_t dataLen )
{
    char * pInputLine = NULL;
    uint8_t * pSessionId = pData;
    bool parseStatus = true;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    int32_t tempValue = 0;

    if( pContext == NULL )
    {
        IotLogError( "GetSocketId: Invalid handle" );
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if( ( pSessionId == NULL ) || ( dataLen != sizeof( uint8_t ) ) )
    {
        IotLogError( "GetSocketId: Bad parameters" );
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if( ( pAtResp == NULL ) || ( pAtResp->pItm == NULL ) || ( pAtResp->pItm->pLine == NULL ) )
    {
        IotLogError( "GetSocketId: Input Line passed is NULL" );
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
        pInputLine = pAtResp->pItm->pLine;
        atCoreStatus = Cellular_ATRemovePrefix( &pInputLine );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces( pInputLine );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATStrtoi( pInputLine, 10, &tempValue );

            if( atCoreStatus == CELLULAR_AT_SUCCESS )
            {
                if( ( tempValue >= MIN_TCP_SESSION_ID ) && ( tempValue <= MAX_TCP_SESSION_ID ) )
                {
                    *pSessionId = ( uint8_t ) tempValue;
                }
                else
                {
                    atCoreStatus = CELLULAR_AT_ERROR;
                }
            }
        }

        pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

static CellularError_t _Cellular_GetSocketNumber( CellularHandle_t cellularHandle,
                                                  CellularSocketHandle_t socketHandle,
                                                  uint8_t * pSessionId )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularAtReq_t atReqSocketConnect =
    {
        "AT+USOCR=6,0",
        CELLULAR_AT_WITH_PREFIX,
        "+USOCR",
        _Cellular_RecvFuncGetSocketId,
        pSessionId,
        sizeof( uint8_t ),
    };

    /* Internal function. Caller checks parameters. */

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqSocketConnect );

        if( pktStatus != CELLULAR_PKT_STATUS_OK )
        {
            IotLogError( "_Cellular_GetSocketNumber: get socekt number failed PktRet: %d", pktStatus );
            cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
/* coverity[misra_c_2012_rule_8_13_violation] */
CellularError_t Cellular_SocketRecv( CellularHandle_t cellularHandle,
                                     CellularSocketHandle_t socketHandle,
                                     /* coverity[misra_c_2012_rule_8_13_violation] */
                                     uint8_t * pBuffer,
                                     uint32_t bufferLength,
                                     /* coverity[misra_c_2012_rule_8_13_violation] */
                                     uint32_t * pReceivedDataLength )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    char cmdBuf[ CELLULAR_AT_CMD_TYPICAL_MAX_SIZE ] = { '\0' };
    uint32_t recvTimeout = DATA_READ_TIMEOUT_MS;
    uint32_t recvLen = bufferLength;
    _socketDataRecv_t dataRecv =
    {
        pReceivedDataLength,
        pBuffer
    };
    CellularAtReq_t atReqSocketRecv =
    {
        cmdBuf,
        CELLULAR_AT_MULTI_DATA_WO_PREFIX,
        "+USORD",
        _Cellular_RecvFuncData,
        ( void * ) &dataRecv,
        bufferLength,
    };
    uint32_t sessionId = 0;

    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "Cellular_SocketRecv: _Cellular_CheckLibraryStatus failed" );
    }
    else if( socketHandle == NULL )
    {
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else if( ( pBuffer == NULL ) || ( pReceivedDataLength == NULL ) || ( bufferLength == 0U ) )
    {
        IotLogDebug( "Cellular_SocketRecv: Bad input Param" );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        sessionId = ( uint32_t ) socketHandle->pModemData;

        /* Update recvLen to maximum module length. */
        if( CELLULAR_MAX_RECV_DATA_LEN <= bufferLength )
        {
            recvLen = ( uint32_t ) CELLULAR_MAX_RECV_DATA_LEN;
        }

        /* Update receive timeout to default timeout if not set with setsocketopt. */
        if( recvLen > 0 )
        {
            if( socketHandle->recvTimeoutMs != 0U )
            {
                recvTimeout = socketHandle->recvTimeoutMs;
            }

            /* The return value of snprintf is not used.
             * The max length of the string is fixed and checked offline. */
            /* coverity[misra_c_2012_rule_21_6_violation]. */
            ( void ) snprintf( cmdBuf, CELLULAR_AT_CMD_TYPICAL_MAX_SIZE,
                               "%s%ld,%ld", "AT+USORD=", sessionId, recvLen );
            pktStatus = _Cellular_TimeoutAtcmdDataRecvRequestWithCallback( pContext,
                                                                           atReqSocketRecv,
                                                                           recvTimeout,
                                                                           socketRecvDataPrefix,
                                                                           NULL );

            if( pktStatus != CELLULAR_PKT_STATUS_OK )
            {
                /* Reset data handling parameters. */
                IotLogError( "Cellular_SocketRecv: Data Receive fail, pktStatus: %d", pktStatus );
                cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
            }
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t socketSendDataPrefix( void * pCallbackContext,
                                                 char * pLine,
                                                 uint32_t * pBytesRead )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    if( ( pLine == NULL ) || ( pBytesRead == NULL ) )
    {
        IotLogError( "socketSendDataPrefix: pLine is invalid or pBytesRead is invalid" );
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if( pCallbackContext != NULL )
    {
        IotLogError( "socketSendDataPrefix: pCallbackContext is not NULL" );
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if( *pBytesRead != 1U )
    {
        IotLogDebug( "socketSendDataPrefix: pBytesRead %u %s is not 1", *pBytesRead, pLine );
    }
    else
    {
        /* After the data prefix, there should not be any data in stream.
         * Cellular commmon processes AT command in lines. Add a '\0' after '@'. */
        if( strcmp( pLine, "@" ) == 0 )
        {
            pLine[ 1 ] = '\n';
            *pBytesRead = 2;
        }
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
/* coverity[misra_c_2012_rule_8_13_violation] */
CellularError_t Cellular_SocketSend( CellularHandle_t cellularHandle,
                                     CellularSocketHandle_t socketHandle,
                                     const uint8_t * pData,
                                     uint32_t dataLength,
                                     /* coverity[misra_c_2012_rule_8_13_violation] */
                                     uint32_t * pSentDataLength )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    uint32_t sendTimeout = DATA_SEND_TIMEOUT_MS;
    char cmdBuf[ CELLULAR_AT_CMD_TYPICAL_MAX_SIZE ] = { '\0' };
    CellularAtReq_t atReqSocketSend =
    {
        cmdBuf,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
    CellularAtDataReq_t atDataReqSocketSend =
    {
        pData,
        dataLength,
        pSentDataLength,
        NULL,
        0
    };
    uint32_t sessionId = 0;

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "Cellular_SocketSend: _Cellular_CheckLibraryStatus failed" );
    }
    else if( socketHandle == NULL )
    {
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else if( ( pData == NULL ) || ( pSentDataLength == NULL ) || ( dataLength == 0U ) )
    {
        IotLogDebug( "Cellular_SocketSend: Invalid parameter" );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else if( socketHandle->socketState != SOCKETSTATE_CONNECTED )
    {
        /* Check the socket connection state. */
        if( ( socketHandle->socketState == SOCKETSTATE_ALLOCATED ) || ( socketHandle->socketState == SOCKETSTATE_CONNECTING ) )
        {
            cellularStatus = CELLULAR_SOCKET_NOT_CONNECTED;
        }
        else
        {
            cellularStatus = CELLULAR_SOCKET_CLOSED;
        }
    }
    else
    {
        sessionId = ( uint32_t ) socketHandle->pModemData;

        /* Send data length check. */
        if( dataLength > ( uint32_t ) CELLULAR_MAX_SEND_DATA_LEN )
        {
            atDataReqSocketSend.dataLen = ( uint32_t ) CELLULAR_MAX_SEND_DATA_LEN;
        }

        /* Check send timeout. If not set by setsockopt, use default value. */
        if( socketHandle->sendTimeoutMs != 0U )
        {
            sendTimeout = socketHandle->sendTimeoutMs;
        }

        /* The return value of snprintf is not used.
         * The max length of the string is fixed and checked offline. */
        /* coverity[misra_c_2012_rule_21_6_violation]. */
        ( void ) snprintf( cmdBuf, CELLULAR_AT_CMD_TYPICAL_MAX_SIZE, "%s%lu,%ld",
                           "AT+USOWR=", sessionId, atDataReqSocketSend.dataLen );

        pktStatus = _Cellular_AtcmdDataSend( pContext, atReqSocketSend, atDataReqSocketSend,
                                             socketSendDataPrefix, NULL,
                                             PACKET_REQ_TIMEOUT_MS, sendTimeout, 200U );

        if( pktStatus != CELLULAR_PKT_STATUS_OK )
        {
            if( socketHandle->socketState == SOCKETSTATE_DISCONNECTED )
            {
                IotLogInfo( "Cellular_SocketSend: Data send fail, socket already closed" );
                cellularStatus = CELLULAR_SOCKET_CLOSED;
            }
            else
            {
                IotLogError( "Cellular_SocketSend: Data send fail, PktRet: %d", pktStatus );
                cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
            }
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
CellularError_t Cellular_SocketClose( CellularHandle_t cellularHandle,
                                      CellularSocketHandle_t socketHandle )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    char cmdBuf[ CELLULAR_AT_CMD_TYPICAL_MAX_SIZE ] = { '\0' };
    CellularAtReq_t atReqSocketClose =
    {
        cmdBuf,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
    uint32_t sessionId = ( uint32_t ) socketHandle->pModemData;
    cellularModuleContext_t * pModuleContext = NULL;

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "Cellular_SocketClose: _Cellular_CheckLibraryStatus failed" );
    }
    else if( socketHandle == NULL )
    {
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else
    {
        cellularStatus = _Cellular_GetModuleContext( pContext, ( void ** ) &pModuleContext );
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        /* Remove the mapping. */
        pModuleContext->pSessionMap[ sessionId ] = INVALID_SOCKET_INDEX;

        /* Close the socket. */
        if( socketHandle->socketState == SOCKETSTATE_CONNECTED )
        {
            ( void ) snprintf( cmdBuf, CELLULAR_AT_CMD_TYPICAL_MAX_SIZE, "%s%u,%d",
                               "AT+USOCL=", sessionId, CELLULAR_CONFIG_SET_SOCKET_CLOSE_ASYNC_MODE );
            pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback( pContext, atReqSocketClose, SOCKET_CLOSE_PACKET_REQ_TIMEOUT_MS );

            /* Delete the socket config. */
            if( pktStatus != CELLULAR_PKT_STATUS_OK )
            {
                IotLogWarn( "Cellular_SocketClose: AT+USOCL fail, PktRet: %d", pktStatus );
            }
        }

        /* Ignore the result from the info, and force to remove the socket. */
        cellularStatus = _Cellular_RemoveSocketData( pContext, socketHandle );
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
CellularError_t Cellular_SocketConnect( CellularHandle_t cellularHandle,
                                        CellularSocketHandle_t socketHandle,
                                        CellularSocketAccessMode_t dataAccessMode,
                                        const CellularSocketAddress_t * pRemoteSocketAddress )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    char cmdBuf[ CELLULAR_AT_CMD_MAX_SIZE ] = { '\0' };
    uint8_t sessionId = 0;
    CellularAtReq_t atReqSocketConnect =
    {
        cmdBuf,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
    cellularModuleContext_t * pModuleContext = NULL;

    /* Make sure the library is open. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "Cellular_SocketConnect: _Cellular_CheckLibraryStatus failed" );
    }
    else if( pRemoteSocketAddress == NULL )
    {
        IotLogDebug( "Cellular_SocketConnect: Invalid socket address" );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else if( socketHandle == NULL )
    {
        IotLogDebug( "Cellular_SocketConnect: Invalid socket handle" );
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else if( socketHandle->socketProtocol != CELLULAR_SOCKET_PROTOCOL_TCP )
    {
        IotLogDebug( "Cellular_SocketConnect: Invalid socket protocol" );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        cellularStatus = storeAccessModeAndAddress( pContext, socketHandle, dataAccessMode, pRemoteSocketAddress );
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        cellularStatus = _Cellular_GetModuleContext( pContext, ( void ** ) &pModuleContext );
    }

    /* Set socket config and get session id. The session id is defined by the modem. */
    if( cellularStatus == CELLULAR_SUCCESS )
    {
        /* Builds the Socket connect command. */
        cellularStatus = _Cellular_GetSocketNumber( pContext, socketHandle, &sessionId );

        if( cellularStatus == CELLULAR_SUCCESS )
        {
            /* Store the session ID in the pointer directly instead allocate extra memory. */
            socketHandle->pModemData = ( void * ) sessionId;

            /* Create the reverse table to store the socketIndex to sessionId. */
            pModuleContext->pSessionMap[ sessionId ] = socketHandle->socketId;
        }
    }

    /* Start the tcp connection. */
    if( cellularStatus == CELLULAR_SUCCESS )
    {
        /*
         * /* The return value of snprintf is not used.
         * The max length of the string is fixed and checked offline. */
        /* coverity[misra_c_2012_rule_21_6_violation]. */
        ( void ) snprintf( cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,
                           "AT+USOCO=%u,\"%s\",%d,1",
                           sessionId,
                           socketHandle->remoteSocketAddress.ipAddress.ipAddress,
                           socketHandle->remoteSocketAddress.port );

        pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback( pContext, atReqSocketConnect,
                                                               SOCKET_CONNECT_PACKET_REQ_TIMEOUT_MS );

        if( pktStatus != CELLULAR_PKT_STATUS_OK )
        {
            IotLogError( "Cellular_SocketConnect: Socket connect failed, cmdBuf:%s, PktRet: %d", cmdBuf, pktStatus );
            cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
        }
        else
        {
            socketHandle->socketState = SOCKETSTATE_CONNECTING;
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
CellularError_t Cellular_GetSimCardStatus( CellularHandle_t cellularHandle,
                                           CellularSimCardStatus_t * pSimCardStatus )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "Cellular_GetSimCardStatus: _Cellular_CheckLibraryStatus failed" );
    }
    else if( pSimCardStatus == NULL )
    {
        IotLogWarn( "Cellular_GetSimCardStatus: Bad input Parameter " );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* Parameters are checked in this API. */
        pSimCardStatus->simCardState = CELLULAR_SIM_CARD_UNKNOWN;
        pSimCardStatus->simCardLockState = CELLULAR_SIM_CARD_LOCK_UNKNOWN;

        cellularStatus = Cellular_CommonGetSimCardLockStatus( cellularHandle, pSimCardStatus );

        if( cellularStatus == CELLULAR_SUCCESS )
        {
            if( ( pSimCardStatus->simCardLockState != CELLULAR_SIM_CARD_INVALID ) &&
                ( pSimCardStatus->simCardLockState != CELLULAR_SIM_CARD_LOCK_UNKNOWN ) )
            {
                pSimCardStatus->simCardState = CELLULAR_SIM_CARD_INSERTED;
            }
            else
            {
                pSimCardStatus->simCardState = CELLULAR_SIM_CARD_UNKNOWN;
            }

            IotLogInfo( "Cellular_GetSimCardStatus, Sim Insert State[%d], Lock State[%d]",
                        pSimCardStatus->simCardState, pSimCardStatus->simCardLockState );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
CellularError_t Cellular_DeactivatePdn( CellularHandle_t cellularHandle,
                                        uint8_t contextId )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    char cmdBuf[ CELLULAR_AT_CMD_MAX_SIZE ] = { '\0' };
    bool packetSwitchStatus = false;

    CellularServiceStatus_t serviceStatus = { 0 };
    CellularPdnContextActInfo_t pdpContextsActInfo = { 0 };

    CellularAtReq_t atReqDeactPdn =
    {
        cmdBuf,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };

    cellularStatus = _Cellular_IsValidPdn( contextId );

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        /* Make sure the library is open. */
        cellularStatus = _Cellular_CheckLibraryStatus( pContext );
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        /* Get current network operator settings. */
        cellularStatus = Cellular_CommonGetServiceStatus( cellularHandle, &serviceStatus );

        if( cellularStatus == CELLULAR_SUCCESS )
        {
            /* Check the current <Act> status of context. */
            cellularStatus = _Cellular_GetContextActivationStatus( cellularHandle, &pdpContextsActInfo );
        }

        if( cellularStatus == CELLULAR_SUCCESS )
        {
            IotLogDebug( "Cellular_DeactivatePdn: Listing operator and context details below." );

            for( int i = 0; i < MAX_PDP_CONTEXTS - 1; i++ )
            {
                /* Print only those contexts that are present in +CGACT response */
                if( pdpContextsActInfo.contextsPresent[ i ] )
                {
                    IotLogDebug( "Context [%d], Act State [%d], Operator <Act> [%d]\r\n", i + 1,
                                 pdpContextsActInfo.contextActState[ i ], serviceStatus.rat );
                }
            }

            /* Deactivate context if active */
            if( pdpContextsActInfo.contextActState[ contextId - 1 ] == TRUE )
            {
                /* Don't deactivate LTE default bearer context */
                /* Otherwise sending AT command "+CGACT=0,1" for deactivation will result in ERROR */
                if( ( serviceStatus.rat >= CELLULAR_RAT_LTE ) && ( contextId == DEFAULT_BEARER_CONTEXT_ID ) )
                {
                    IotLogInfo( "Cellular_DeactivatePdn: Default Bearer context %d Active. Not allowed to deactivate.", contextId );
                    cellularStatus = CELLULAR_NOT_ALLOWED;
                }
                else
                {
                    ( void ) snprintf( cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "%s=0,%u", "AT+CGACT", contextId );
                    pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback( pContext, atReqDeactPdn, PDN_DEACT_PACKET_REQ_TIMEOUT_MS );
                    cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
                }

                if( ( cellularStatus != CELLULAR_SUCCESS ) && ( cellularStatus != CELLULAR_NOT_ALLOWED ) )
                {
                    IotLogError( "Cellular_DeactivatePdn: can't deactivate PDN, PktRet: %d", pktStatus );

                    /* Sometimes +CGACT deactivation fails in 2G. Then check packet switch attach. If attached, detach packet switch. */
                    if( ( serviceStatus.rat == CELLULAR_RAT_GSM ) || ( serviceStatus.rat == CELLULAR_RAT_EDGE ) )
                    {
                        cellularStatus = _Cellular_GetPacketSwitchStatus( cellularHandle, &packetSwitchStatus );

                        if( ( cellularStatus == CELLULAR_SUCCESS ) && ( packetSwitchStatus == true ) )
                        {
                            IotLogError( "Deactivate Packet switch" );
                            ( void ) snprintf( cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "%s", "AT+CGATT=0" );
                            pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback( pContext, atReqDeactPdn, GPRS_ATTACH_REQ_TIMEOUT_MS );
                            cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
                        }
                        else if( cellularStatus != CELLULAR_SUCCESS )
                        {
                            IotLogError( "Packet switch query failed" );
                            pktStatus = CELLULAR_PKT_STATUS_FAILURE;
                            cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
                        }
                        else
                        {
                            IotLogInfo( "Packet switch detached" );
                        }
                    }
                }
            }
            else
            {
                IotLogInfo( "Cellular_DeactivatePdn: Context id [%d] is already deactive", contextId );
            }
        }
        else
        {
            IotLogError( "Cellular_DeactivatePdn: Unable to list operator and context details." );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL prototype. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static CellularPktStatus_t _Cellular_RecvFuncPacketSwitchStatus( CellularContext_t * pContext,
                                                                 const CellularATCommandResponse_t * pAtResp,
                                                                 void * pData,
                                                                 uint16_t dataLen )
{
    char * pInputLine = NULL;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    bool * pPacketSwitchStatus = ( bool * ) pData;

    if( pContext == NULL )
    {
        IotLogError( "PacketSwitchStatus: Invalid handle" );
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if( ( pData == NULL ) || ( dataLen != sizeof( bool ) ) )
    {
        IotLogError( "GetPacketSwitchStatus: Invalid param" );
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if( ( pAtResp == NULL ) || ( pAtResp->pItm == NULL ) || ( pAtResp->pItm->pLine == NULL ) )
    {
        IotLogError( "GetPacketSwitchStatus: Input Line passed is NULL" );
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
        pInputLine = pAtResp->pItm->pLine;

        /* Remove prefix. */
        atCoreStatus = Cellular_ATRemovePrefix( &pInputLine );

        /* Remove leading space. */
        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATRemoveLeadingWhiteSpaces( &pInputLine );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( *pInputLine == '0' )
            {
                *pPacketSwitchStatus = false;
            }
            else if( *pInputLine == '1' )
            {
                *pPacketSwitchStatus = true;
            }
            else
            {
                atCoreStatus = CELLULAR_AT_ERROR;
            }
        }

        pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* Get PDN context Activation state */

static CellularPktStatus_t _Cellular_RecvFuncGetPdpContextActState( CellularContext_t * pContext,
                                                                    const CellularATCommandResponse_t * pAtResp,
                                                                    void * pData,
                                                                    uint16_t dataLen )
{
    char * pRespLine = NULL;
    CellularPdnContextActInfo_t * pPDPContextsActInfo = ( CellularPdnContextActInfo_t * ) pData;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    const CellularATCommandLine_t * pCommnadItem = NULL;
    uint8_t tokenIndex = 0;
    uint8_t contextId = 0;
    int32_t tempValue = 0;
    char * pToken = NULL;

    if( pContext == NULL )
    {
        IotLogError( "_Cellular_RecvFuncGetPdpContextActState: invalid context" );
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if( ( pPDPContextsActInfo == NULL ) || ( dataLen != sizeof( CellularPdnContextActInfo_t ) ) )
    {
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if( pAtResp == NULL )
    {
        IotLogError( "_Cellular_RecvFuncGetPdpContextActState: Response is invalid" );
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if( ( pAtResp->pItm == NULL ) || ( pAtResp->pItm->pLine == NULL ) )
    {
        IotLogError( "_Cellular_RecvFuncGetPdpContextActState: no PDN context available" );
        pktStatus = CELLULAR_PKT_STATUS_OK;
    }
    else
    {
        pRespLine = pAtResp->pItm->pLine;

        pCommnadItem = pAtResp->pItm;

        while( pCommnadItem != NULL )
        {
            pRespLine = pCommnadItem->pLine;
            IotLogDebug( "_Cellular_RecvFuncGetPdpContextActState: pRespLine [%s]", pRespLine );

            /* Removing all the Spaces in the AT Response. */
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces( pRespLine );

            if( atCoreStatus == CELLULAR_AT_SUCCESS )
            {
                atCoreStatus = Cellular_ATRemovePrefix( &pRespLine );

                if( atCoreStatus == CELLULAR_AT_SUCCESS )
                {
                    atCoreStatus = Cellular_ATRemoveAllDoubleQuote( pRespLine );
                }

                if( atCoreStatus == CELLULAR_AT_SUCCESS )
                {
                    atCoreStatus = Cellular_ATGetNextTok( &pRespLine, &pToken );
                }

                if( atCoreStatus == CELLULAR_AT_SUCCESS )
                {
                    tokenIndex = 0;

                    while( ( pToken != NULL ) && ( atCoreStatus == CELLULAR_AT_SUCCESS ) )
                    {
                        switch( tokenIndex )
                        {
                            case ( CELLULAR_PDN_ACT_STATUS_POS_CONTEXT_ID ):
                                IotLogDebug( "_Cellular_RecvFuncGetPdpContextActState: Context Id pToken: %s", pToken );
                                atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

                                if( atCoreStatus == CELLULAR_AT_SUCCESS )
                                {
                                    if( ( tempValue >= ( int32_t ) CELLULAR_PDN_CONTEXT_ID_MIN ) &&
                                        ( tempValue <= ( int32_t ) MAX_PDP_CONTEXTS ) )
                                    {
                                        contextId = ( uint8_t ) tempValue;
                                        pPDPContextsActInfo->contextsPresent[ contextId - 1 ] = TRUE;
                                        IotLogDebug( "_Cellular_RecvFuncGetPdpContextActState: Context Id: %d", contextId );
                                    }
                                    else
                                    {
                                        IotLogError( "_Cellular_RecvFuncGetPdpContextActState: Invalid Context Id. Token %s", pToken );
                                        atCoreStatus = CELLULAR_AT_ERROR;
                                    }
                                }

                                break;

                            case ( CELLULAR_PDN_ACT_STATUS_POS_CONTEXT_STATE ):
                                IotLogDebug( "_Cellular_RecvFuncGetPdpContextActState: Context <Act> pToken: %s", pToken );
                                atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

                                if( atCoreStatus == CELLULAR_AT_SUCCESS )
                                {
                                    pPDPContextsActInfo->contextActState[ contextId - 1 ] = ( bool ) tempValue;
                                    IotLogDebug( "_Cellular_RecvFuncGetPdpContextActState: Context <Act>: %d", pPDPContextsActInfo->contextActState[ contextId - 1 ] );
                                }

                                break;

                            default:
                                break;
                        }

                        tokenIndex++;

                        if( Cellular_ATGetNextTok( &pRespLine, &pToken ) != CELLULAR_AT_SUCCESS )
                        {
                            break;
                        }
                    }
                }
            }

            pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );

            if( pktStatus != CELLULAR_PKT_STATUS_OK )
            {
                IotLogError( "_Cellular_RecvFuncGetPdpContextActState: parse %s failed", pRespLine );
                break;
            }

            pCommnadItem = pCommnadItem->pNext;
        }
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* Check activation status of particular context. */

static CellularError_t _Cellular_GetContextActivationStatus( CellularHandle_t cellularHandle,
                                                             CellularPdnContextActInfo_t * pPdpContextsActInfo )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    CellularAtReq_t atReqPacketSwitchStatus =
    {
        "AT+CGACT?",
        CELLULAR_AT_MULTI_WITH_PREFIX,
        "+CGACT",
        _Cellular_RecvFuncGetPdpContextActState,
        pPdpContextsActInfo,
        sizeof( CellularPdnContextActInfo_t ),
    };

    /* Internal function. Callee check parameters. */
    pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqPacketSwitchStatus );
    cellularStatus = _Cellular_TranslatePktStatus( pktStatus );

    return cellularStatus;
}

/*-----------------------------------------------------------*/

static CellularError_t _Cellular_GetPacketSwitchStatus( CellularHandle_t cellularHandle,
                                                        bool * pPacketSwitchStatus )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularAtReq_t atReqPacketSwitchStatus =
    {
        "AT+CGATT?",
        CELLULAR_AT_WITH_PREFIX,
        "+CGATT",
        _Cellular_RecvFuncPacketSwitchStatus,
        pPacketSwitchStatus,
        sizeof( bool ),
    };

    /* Internal function. Callee check parameters. */
    pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback( pContext, atReqPacketSwitchStatus, PDN_ACT_PACKET_REQ_TIMEOUT_MS );
    cellularStatus = _Cellular_TranslatePktStatus( pktStatus );

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
CellularError_t Cellular_ActivatePdn( CellularHandle_t cellularHandle,
                                      uint8_t contextId )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    char cmdBuf[ CELLULAR_AT_CMD_MAX_SIZE ] = { '\0' };
    bool packetSwitchStatus = false;
    bool pdnStatus = false;

    CellularPdnContextActInfo_t pdpContextsActInfo = { 0 };

    CellularAtReq_t atReqActPdn =
    {
        cmdBuf,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };

    cellularStatus = _Cellular_IsValidPdn( contextId );

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        /* Make sure the library is open. */
        cellularStatus = _Cellular_CheckLibraryStatus( pContext );
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        /* Check the current <Act> status of context. If not activated, activate the PDN context ID. */
        cellularStatus = _Cellular_GetContextActivationStatus( cellularHandle, &pdpContextsActInfo );

        if( cellularStatus == CELLULAR_SUCCESS )
        {
            IotLogDebug( "Cellular_ActivatePdn: Listing operator and context details below." );

            for( int i = 0; i < MAX_PDP_CONTEXTS - 1; i++ )
            {
                /* Print only those contexts that are present in +CGACT response */
                if( pdpContextsActInfo.contextsPresent[ i ] )
                {
                    IotLogDebug( "Cellular_ActivatePdn: Context [%d], Act State [%d]\r\n", i + 1,
                                 pdpContextsActInfo.contextActState[ i ] );
                }
            }
        }

        /* Activate context if not already active */
        if( pdpContextsActInfo.contextActState[ contextId - 1 ] == FALSE )
        {
            if( pktStatus == CELLULAR_PKT_STATUS_OK )
            {
                ( void ) snprintf( cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "%s=1,%u", "AT+CGACT", contextId );
                pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback( pContext, atReqActPdn, PDN_ACT_PACKET_REQ_TIMEOUT_MS );
            }

            if( pktStatus != CELLULAR_PKT_STATUS_OK )
            {
                IotLogError( "Cellular_ActivatePdn: can't activate PDN, PktRet: %d", pktStatus );
                cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
            }
        }
        else
        {
            IotLogInfo( "Cellular_ActivatePdn: Context id [%d] is already active", contextId );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
/* coverity[misra_c_2012_rule_8_13_violation] */
CellularError_t Cellular_GetPdnStatus( CellularHandle_t cellularHandle,
                                       CellularPdnStatus_t * pPdnStatusBuffers,
                                       uint8_t numStatusBuffers,
                                       uint8_t * pNumStatus )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    CellularPdnContextActInfo_t pdpContextsActInfo = { 0 };

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        /* Make sure the library is open. */
        cellularStatus = _Cellular_CheckLibraryStatus( pContext );
    }

    if( ( pPdnStatusBuffers == NULL ) || ( pNumStatus == NULL ) || ( numStatusBuffers < 1u ) )
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
        IotLogWarn( "_Cellular_GetPdnStatus: Bad input Parameter " );
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        /* Check the current <Act> status of contexts. */
        cellularStatus = _Cellular_GetContextActivationStatus( cellularHandle, &pdpContextsActInfo );

        if( cellularStatus == CELLULAR_SUCCESS )
        {
            IotLogDebug( "Cellular_GetPdnStatus: Listing operator and context details below." );

            for( int i = 0; i < MAX_PDP_CONTEXTS - 1; i++ )
            {
                /* Print only those contexts that are present in +CGACT response */
                if( pdpContextsActInfo.contextsPresent[ i ] )
                {
                    IotLogDebug( "Context [%d], Act State [%d]\r\n", i + 1, pdpContextsActInfo.contextActState[ i ] );
                }
            }

            /* TODO: Currently only one context state can be saved in PdnStatusBuffers. */
            pPdnStatusBuffers->state = pdpContextsActInfo.contextActState[ 0 ];
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
CellularError_t Cellular_SetRatPriority( CellularHandle_t cellularHandle,
                                         const CellularRat_t * pRatPriorities,
                                         uint8_t ratPrioritiesLength )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    uint8_t i = 0;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    char cmdBuf[ CELLULAR_AT_CMD_MAX_SIZE ] = { '\0' };

    CellularAtReq_t atReqSetRatPriority =
    {
        cmdBuf,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };

    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "Cellular_SetRatPriority: _Cellular_CheckLibraryStatus failed" );
    }
    else if( ( pRatPriorities == NULL ) || ( ratPrioritiesLength == 0U ) ||
             ( ratPrioritiesLength > ( uint8_t ) CELLULAR_MAX_RAT_PRIORITY_COUNT ) )
    {
        IotLogWarn( "Cellular_SetRatPriority: Bad input Parameter " );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* In case of +UMNOPROF=0, AT+URAT set commad is not allowed */
        if( CELLULAR_CONFIG_SET_MNO_PROFILE != 0 )
        {
            ( void ) strcpy( cmdBuf, "AT+CFUN=4;+URAT=" );

            while( i < ratPrioritiesLength )
            {
                if( ( pRatPriorities[ i ] == CELLULAR_RAT_GSM ) || ( pRatPriorities[ i ] == CELLULAR_RAT_EDGE ) )
                {
                    ( void ) strcat( cmdBuf, "9" );
                }
                else if( pRatPriorities[ i ] == CELLULAR_RAT_CATM1 )
                {
                    ( void ) strcat( cmdBuf, "7" );
                }
                else if( pRatPriorities[ i ] == CELLULAR_RAT_NBIOT )
                {
                    ( void ) strcat( cmdBuf, "8" );
                }
                else
                {
                    cellularStatus = CELLULAR_BAD_PARAMETER;
                    break;
                }

                i++;

                if( i < ratPrioritiesLength )
                {
                    ( void ) strcat( cmdBuf, "," );
                }
            }

            ( void ) strcat( cmdBuf, ";+CFUN=1" );

            if( cellularStatus == CELLULAR_SUCCESS )
            {
                pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqSetRatPriority );
                cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
            }
        }
        else
        {
            IotLogDebug( "Cellular_SetRatPriority: Automatic selection as UMNOPROF profile is 0" );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Get modem RAT priority setting. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static CellularPktStatus_t _Cellular_RecvFuncGetRatPriority( CellularContext_t * pContext,
                                                             const CellularATCommandResponse_t * pAtResp,
                                                             void * pData,
                                                             uint16_t dataLen )
{
    char * pInputLine = NULL, * pToken = NULL;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularRat_t * pRatPriorities = NULL;
    uint8_t ratIndex = 0;
    uint32_t maxRatPriorityLength = ( dataLen > RAT_PRIOIRTY_LIST_LENGTH ? RAT_PRIOIRTY_LIST_LENGTH : dataLen );

    if( pContext == NULL )
    {
        IotLogError( "_Cellular_RecvFuncGetRatPriority: Invalid context" );
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if( ( pAtResp == NULL ) || ( pAtResp->pItm == NULL ) ||
             ( pAtResp->pItm->pLine == NULL ) || ( pData == NULL ) || ( dataLen == 0U ) )
    {
        IotLogError( "_Cellular_RecvFuncGetRatPriority: Invalid param" );
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
        pInputLine = pAtResp->pItm->pLine;
        pRatPriorities = ( CellularRat_t * ) pData;

        atCoreStatus = Cellular_ATRemovePrefix( &pInputLine );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces( pInputLine );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            memset( pRatPriorities, CELLULAR_RAT_INVALID, dataLen );

            /* pInputLine : 7,8,9. */
            atCoreStatus = Cellular_ATGetNextTok( &pInputLine, &pToken );

            if( atCoreStatus == CELLULAR_AT_SUCCESS )
            {
                while( pToken != NULL && ratIndex < maxRatPriorityLength )
                {
                    IotLogDebug( "_Cellular_RecvFuncGetRatPriority: pToken [%s]", pToken );

                    if( strcmp( pToken, "7" ) == 0 )
                    {
                        pRatPriorities[ ratIndex ] = CELLULAR_RAT_CATM1;
                        IotLogDebug( "_Cellular_RecvFuncGetRatPriority: CELLULAR_RAT_CATM1" );
                    }
                    else if( strcmp( pToken, "8" ) == 0 )
                    {
                        pRatPriorities[ ratIndex ] = CELLULAR_RAT_NBIOT;
                        IotLogDebug( "_Cellular_RecvFuncGetRatPriority: CELLULAR_RAT_NBIOT" );
                    }
                    else if( strcmp( pToken, "9" ) == 0 )
                    {
                        pRatPriorities[ ratIndex ] = CELLULAR_RAT_GSM; /* or CELLULAR_RAT_EDGE */
                        IotLogDebug( "_Cellular_RecvFuncGetRatPriority: CELLULAR_RAT_GSM" );
                    }
                    else
                    {
                        IotLogDebug( "_Cellular_RecvFuncGetRatPriority: Invalid RAT string [%s]", pToken );
                    }

                    ratIndex++;

                    if( Cellular_ATGetNextTok( &pInputLine, &pToken ) != CELLULAR_AT_SUCCESS )
                    {
                        break;
                    }
                }
            }
        }

        pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
CellularError_t Cellular_GetRatPriority( CellularHandle_t cellularHandle,
                                         CellularRat_t * pRatPriorities,
                                         uint8_t ratPrioritiesLength,
                                         uint8_t * pReceiveRatPrioritesLength )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    uint8_t ratIndex = 0;

    CellularAtReq_t atReqSetRatPriority =
    {
        "AT+URAT?",
        CELLULAR_AT_WITH_PREFIX,
        "+URAT",
        _Cellular_RecvFuncGetRatPriority,
        pRatPriorities,
        ( uint16_t ) ratPrioritiesLength,
    };

    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "Cellular_GetRatPriority: _Cellular_CheckLibraryStatus failed" );
    }
    else if( ( pRatPriorities == NULL ) || ( ratPrioritiesLength == 0U ) ||
             ( ratPrioritiesLength > ( uint8_t ) CELLULAR_MAX_RAT_PRIORITY_COUNT ) ||
             ( pReceiveRatPrioritesLength == NULL ) )
    {
        IotLogWarn( "Cellular_GetRatPriority: Bad input Parameter " );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* In case of +UMNOPROF=0, AT+URAT? read commad is not allowed */
        if( CELLULAR_CONFIG_SET_MNO_PROFILE != 0 )
        {
            pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqSetRatPriority );

            if( pktStatus == CELLULAR_PKT_STATUS_OK )
            {
                for( ratIndex = 0; ratIndex < ratPrioritiesLength; ratIndex++ )
                {
                    if( pRatPriorities[ ratIndex ] == CELLULAR_RAT_INVALID )
                    {
                        break;
                    }
                }

                *pReceiveRatPrioritesLength = ratIndex;
            }

            cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
        }
        else
        {
            IotLogDebug( "Cellular_GetRatPriority: Automatic selection as UMNOPROF profile is 0" );

            pRatPriorities[ ratIndex++ ] = CELLULAR_RAT_CATM1;
            pRatPriorities[ ratIndex++ ] = CELLULAR_RAT_NBIOT;
            pRatPriorities[ ratIndex++ ] = CELLULAR_RAT_GSM;
            *pReceiveRatPrioritesLength = RAT_PRIOIRTY_LIST_LENGTH;
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

static bool _parseExtendedSignalQuality( char * pQcsqPayload,
                                         CellularSignalInfo_t * pSignalInfo )
{
    char * pToken = NULL, * pTmpQcsqPayload = pQcsqPayload;
    int32_t tempValue = 0;
    int16_t berValue = 0;
    bool parseStatus = true;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    if( ( pSignalInfo == NULL ) || ( pQcsqPayload == NULL ) )
    {
        IotLogError( "_parseExtendedSignalQuality: Invalid Input Parameters" );
        parseStatus = false;
    }

    /*  +CESQ: <rxlev>,<ber>,<rscp>,<ecn0>,<rsrq>,<rsrp>. */

    /* Skip <rxlev>. */
    atCoreStatus = Cellular_ATGetNextTok( &pTmpQcsqPayload, &pToken );

    /* Parse <ber>. */
    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATGetNextTok( &pTmpQcsqPayload, &pToken );
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

        if( ( atCoreStatus == CELLULAR_AT_SUCCESS ) && ( tempValue <= INT16_MAX ) && ( tempValue >= INT16_MIN ) )
        {
            /*
             * Bit Error Rate (BER):
             *  • 0..7: as RXQUAL values in the table in 3GPP TS 45.008 [124], subclause 8.2.4
             *  • 99: not known or not detectable
             */
            if( ( tempValue >= 0 ) && ( tempValue <= 7 ) )
            {
                pSignalInfo->ber = ( int16_t ) tempValue;
            }
            else
            {
                pSignalInfo->ber = CELLULAR_INVALID_SIGNAL_VALUE;
            }
        }
        else
        {
            IotLogError( "_parseExtendedSignalQuality: Error in processing BER. Token %s", pToken );
            atCoreStatus = CELLULAR_AT_ERROR;
        }
    }

    /* Skip <rscp>. */
    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATGetNextTok( &pTmpQcsqPayload, &pToken );
    }

    /* Skip <ecno>. */
    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATGetNextTok( &pTmpQcsqPayload, &pToken );
    }

    /* Parse <rsrq>. */
    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATGetNextTok( &pTmpQcsqPayload, &pToken );
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            /*
             * Reference Signal Received Quality (RSRQ):
             *  • 0: less than -19.5 dB
             *  • 1..33: from -19.5 dB to -3.5 dB with 0.5 dB steps
             *  • 34: -3 dB or greater
             *  • 255: not known or not detectable
             */
            if( ( tempValue >= 0 ) && ( tempValue <= 34 ) )
            {
                pSignalInfo->rsrq = ( int16_t ) ( ( -20 ) + ( tempValue * 0.5 ) );
            }
            else
            {
                pSignalInfo->rsrq = CELLULAR_INVALID_SIGNAL_VALUE;
            }
        }
        else
        {
            IotLogError( "_parseExtendedSignalQuality: Error in processing RSRP. Token %s", pToken );
            parseStatus = false;
        }
    }

    /* Parse <rsrp>. */
    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATGetNextTok( &pTmpQcsqPayload, &pToken );
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            /*
             * Reference Signal Received Power(RSRP) :
             *  • 0 : less than - 140 dBm
             *  • 1..96 : from - 140 dBm to - 45 dBm with 1 dBm steps
             *  • 97 : -44 dBm or greater
             *  • 255 : not known or not detectable
             */
            if( ( tempValue >= 0 ) && ( tempValue <= 97 ) )
            {
                pSignalInfo->rsrp = ( int16_t ) ( ( -141 ) + ( tempValue ) );
            }
            else
            {
                pSignalInfo->rsrp = CELLULAR_INVALID_SIGNAL_VALUE;
            }
        }
        else
        {
            IotLogError( "_parseExtendedSignalQuality: Error in processing RSRP. Token %s", pToken );
            parseStatus = false;
        }
    }

    return parseStatus;
}

/*-----------------------------------------------------------*/

static bool _parseSignalQuality( char * pQcsqPayload,
                                 CellularSignalInfo_t * pSignalInfo )
{
    char * pToken = NULL, * pTmpQcsqPayload = pQcsqPayload;
    int32_t tempValue = 0;
    int16_t berValue = 0;
    bool parseStatus = true;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    if( ( pSignalInfo == NULL ) || ( pQcsqPayload == NULL ) )
    {
        IotLogError( "_parseSignalQuality: Invalid Input Parameters" );
        parseStatus = false;
    }

    /* +CSQ: <signal_power>,<qual>. */

    /* Parse <signal_power>. */
    atCoreStatus = Cellular_ATGetNextTok( &pTmpQcsqPayload, &pToken );

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            /*
             * The allowed range is 0-31 and 99.
             *  0           RSSI of the network <= -113 dBm
             *  1           -111 dBm
             *  2...30      -109 dBm <= RSSI of the network <= -53 dBm
             *  31          -51 dBm <= RSSI of the network
             *  99          Not known or not detectable
             */
            if( ( tempValue >= 0 ) && ( tempValue <= 31 ) )
            {
                pSignalInfo->rssi = ( int16_t ) ( ( -113 ) + ( tempValue * 2 ) );
            }
            else if( tempValue == 99 )
            {
                pSignalInfo->rssi = -113;
            }
            else
            {
                pSignalInfo->rssi = CELLULAR_INVALID_SIGNAL_VALUE;
            }
        }
        else
        {
            IotLogError( "_parseSignalQuality: Error in processing RSSI. Token %s", pToken );
            parseStatus = false;
        }
    }

    /* Parse <qual>. */
    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATGetNextTok( &pTmpQcsqPayload, &pToken );
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            /*
             * The allowed range is 0-7 and 99 :
             *  In 2G RAT CS dedicated and GPRS packet transfer mode indicates the Bit Error Rate (BER) as specified in 3GPP TS 45.008
             */
            if( ( tempValue >= 0 ) && ( tempValue <= 7 ) )
            {
                pSignalInfo->ber = ( int16_t ) tempValue;
            }
            else
            {
                pSignalInfo->ber = CELLULAR_INVALID_SIGNAL_VALUE;
            }
        }
        else
        {
            IotLogError( "_parseSignalQuality: Error in processing ber. Token %s", pToken );
            parseStatus = false;
        }
    }

    return parseStatus;
}

/*-----------------------------------------------------------*/

/* parse signal strength response */

static CellularPktStatus_t _Cellular_RecvFuncGetSignalInfo( CellularContext_t * pContext,
                                                            const CellularATCommandResponse_t * pAtResp,
                                                            void * pData,
                                                            uint16_t dataLen )
{
    char * pInputLine = NULL;
    CellularSignalInfo_t * pSignalInfo = ( CellularSignalInfo_t * ) pData;
    bool parseStatus = true;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    bool isExtendedResponse = false;

    if( pContext == NULL )
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if( ( pSignalInfo == NULL ) || ( dataLen != sizeof( CellularSignalInfo_t ) ) )
    {
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if( ( pAtResp == NULL ) || ( pAtResp->pItm == NULL ) || ( pAtResp->pItm->pLine == NULL ) )
    {
        IotLogError( "GetSignalInfo: Input Line passed is NULL" );
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
        pInputLine = pAtResp->pItm->pLine;

        if( strstr( pInputLine, "+CESQ" ) )
        {
            IotLogDebug( "GetSignalInfo: ExtendedResponse received." );
            isExtendedResponse = true;
        }

        atCoreStatus = Cellular_ATRemovePrefix( &pInputLine );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces( pInputLine );
        }

        if( atCoreStatus != CELLULAR_AT_SUCCESS )
        {
            pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
        }
    }

    if( pktStatus == CELLULAR_PKT_STATUS_OK )
    {
        if( isExtendedResponse )
        {
            parseStatus = _parseExtendedSignalQuality( pInputLine, pSignalInfo );
        }
        else
        {
            parseStatus = _parseSignalQuality( pInputLine, pSignalInfo );
        }

        if( parseStatus != true )
        {
            pSignalInfo->rssi = CELLULAR_INVALID_SIGNAL_VALUE;
            pSignalInfo->rsrp = CELLULAR_INVALID_SIGNAL_VALUE;
            pSignalInfo->rsrq = CELLULAR_INVALID_SIGNAL_VALUE;
            pSignalInfo->ber = CELLULAR_INVALID_SIGNAL_VALUE;
            pSignalInfo->bars = CELLULAR_INVALID_SIGNAL_BAR_VALUE;
            pktStatus = CELLULAR_PKT_STATUS_FAILURE;
        }
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_GetSignalInfo( CellularHandle_t cellularHandle,
                                        CellularSignalInfo_t * pSignalInfo )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularRat_t rat = CELLULAR_RAT_INVALID;
    CellularAtReq_t atReqQuerySignalInfo =
    {
        "AT+CSQ",
        CELLULAR_AT_WITH_PREFIX,
        "+CSQ",
        _Cellular_RecvFuncGetSignalInfo,
        pSignalInfo,
        sizeof( CellularSignalInfo_t ),
    };
    CellularAtReq_t atReqQueryExtendedSignalInfo =
    {
        "AT+CESQ",
        CELLULAR_AT_WITH_PREFIX,
        "+CESQ",
        _Cellular_RecvFuncGetSignalInfo,
        pSignalInfo,
        sizeof( CellularSignalInfo_t ),
    };

    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "Cellular_GetSignalInfo: _Cellular_CheckLibraryStatus failed" );
    }
    else if( pSignalInfo == NULL )
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        cellularStatus = _Cellular_GetCurrentRat( pContext, &rat );
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        /* Get +CSQ response */
        pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqQuerySignalInfo );

        if( pktStatus == CELLULAR_PKT_STATUS_OK )
        {
            /* Get +CESQ response */
            pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqQueryExtendedSignalInfo );

            if( pktStatus == CELLULAR_PKT_STATUS_OK )
            {
                /* If the convert failed, the API will return CELLULAR_INVALID_SIGNAL_BAR_VALUE in bars field. */
                ( void ) _Cellular_ComputeSignalBars( rat, pSignalInfo );
            }

            cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
CellularError_t Cellular_SetDns( CellularHandle_t cellularHandle,
                                 uint8_t contextId,
                                 const char * pDnsServerAddress )
{
    /* Modem use dynamic DNS addresses. Return unsupported. */
    return CELLULAR_UNSUPPORTED;
}

/*-----------------------------------------------------------*/

static CellularError_t controlSignalStrengthIndication( CellularContext_t * pContext,
                                                        bool enable )
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    char cmdBuf[ CELLULAR_AT_CMD_TYPICAL_MAX_SIZE ] = { '\0' };
    uint8_t enable_value = 0;
    CellularAtReq_t atReqControlSignalStrengthIndication =
    {
        cmdBuf,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };

    if( enable == true )
    {
        enable_value = 1;
    }
    else
    {
        enable_value = 0;
    }

    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        if( enable_value )
        {
            /* Enable signal level change indication via +CIEV URC.*/
            ( void ) snprintf( cmdBuf, CELLULAR_AT_CMD_TYPICAL_MAX_SIZE, "AT+UCIND=2" );
            pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqControlSignalStrengthIndication );
        }
        else
        {
            /* Disable signal level change indication via +CIEV URC.*/
            ( void ) snprintf( cmdBuf, CELLULAR_AT_CMD_TYPICAL_MAX_SIZE, "AT+UCIND=0" );
            pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqControlSignalStrengthIndication );
        }

        cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
CellularError_t Cellular_RegisterUrcSignalStrengthChangedCallback( CellularHandle_t cellularHandle,
                                                                   CellularUrcSignalStrengthChangedCallback_t signalStrengthChangedCallback,
                                                                   void * pCallbackContext )
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;

    /* pContext is checked in the common library. */
    cellularStatus = Cellular_CommonRegisterUrcSignalStrengthChangedCallback(
        cellularHandle, signalStrengthChangedCallback, pCallbackContext );

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        if( signalStrengthChangedCallback != NULL )
        {
            cellularStatus = controlSignalStrengthIndication( pContext, true );
        }
        else
        {
            cellularStatus = controlSignalStrengthIndication( pContext, false );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Resolve Domain name to IP address */

static CellularPktStatus_t _Cellular_RecvFuncResolveDomainToIpAddress( CellularContext_t * pContext,
                                                                       const CellularATCommandResponse_t * pAtResp,
                                                                       void * pData,
                                                                       uint16_t dataLen )
{
    char * pRespLine = NULL;
    char * pResolvedIpAddress = ( char * ) pData;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    const CellularATCommandLine_t * pCommnadItem = NULL;
    uint8_t tokenIndex = 0;
    char * pToken = NULL;

    if( pContext == NULL )
    {
        IotLogError( "_Cellular_RecvFuncResolveDomainToIpAddress: invalid context" );
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if( ( pResolvedIpAddress == NULL ) || ( dataLen != CELLULAR_IP_ADDRESS_MAX_SIZE ) )
    {
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if( pAtResp == NULL )
    {
        IotLogError( "_Cellular_RecvFuncResolveDomainToIpAddress: Response is invalid" );
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if( ( pAtResp->pItm == NULL ) || ( pAtResp->pItm->pLine == NULL ) )
    {
        IotLogError( "_Cellular_RecvFuncResolveDomainToIpAddress: Address not resolved" );
        pktStatus = CELLULAR_PKT_STATUS_OK;
    }
    else
    {
        pRespLine = pAtResp->pItm->pLine;

        pCommnadItem = pAtResp->pItm;

        while( pCommnadItem != NULL )
        {
            pRespLine = pCommnadItem->pLine;
            IotLogDebug( "_Cellular_RecvFuncResolveDomainToIpAddress: pRespLine [%s]", pRespLine );

            /* Removing all the Spaces in the AT Response. */
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces( pRespLine );

            if( atCoreStatus == CELLULAR_AT_SUCCESS )
            {
                atCoreStatus = Cellular_ATRemovePrefix( &pRespLine );

                if( atCoreStatus == CELLULAR_AT_SUCCESS )
                {
                    atCoreStatus = Cellular_ATRemoveAllDoubleQuote( pRespLine );
                }

                if( atCoreStatus == CELLULAR_AT_SUCCESS )
                {
                    atCoreStatus = Cellular_ATGetNextTok( &pRespLine, &pToken );
                }

                if( atCoreStatus == CELLULAR_AT_SUCCESS )
                {
                    ( void ) strncpy( pResolvedIpAddress, pToken, dataLen );

                    IotLogDebug( "_Cellular_RecvFuncResolveDomainToIpAddress: Resolved IP Address: [%s]", pResolvedIpAddress );
                }
            }

            pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );

            if( pktStatus != CELLULAR_PKT_STATUS_OK )
            {
                IotLogError( "_Cellular_RecvFuncResolveDomainToIpAddress: parse %s failed", pRespLine );
                break;
            }

            pCommnadItem = pCommnadItem->pNext;
        }
    }

    return pktStatus;
}



/*-----------------------------------------------------------*/

/* Cellular HAL API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
CellularError_t Cellular_GetHostByName( CellularHandle_t cellularHandle,
                                        uint8_t contextId,
                                        const char * pcHostName,
                                        char * pResolvedAddress )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    char cmdBuf[ CELLULAR_AT_CMD_MAX_SIZE ] = { '\0' };

    CellularAtReq_t atReqQueryDns =
    {
        cmdBuf,
        CELLULAR_AT_MULTI_WITH_PREFIX,
        "+UDNSRN",
        _Cellular_RecvFuncResolveDomainToIpAddress,
        pResolvedAddress,
        CELLULAR_IP_ADDRESS_MAX_SIZE,
    };

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "Cellular_GetHostByName: _Cellular_CheckLibraryStatus failed" );
    }
    else if( ( pcHostName == NULL ) || ( pResolvedAddress == NULL ) )
    {
        IotLogError( "Cellular_GetHostByName: Bad input Parameter " );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        ( void ) snprintf( cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,
                           "AT+UDNSRN=0,\"%s\"", pcHostName );

        pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback( pContext, atReqQueryDns, DNS_QUERY_REQ_TIMEOUT_MS );

        if( pktStatus != CELLULAR_PKT_STATUS_OK )
        {
            IotLogError( "Cellular_GetHostByName: couldn't resolve host name" );
            cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Get PDN context APN name*/

static CellularPktStatus_t _Cellular_RecvFuncGetPdpContextSettings( CellularContext_t * pContext,
                                                                    const CellularATCommandResponse_t * pAtResp,
                                                                    void * pData,
                                                                    uint16_t dataLen )
{
    char * pRespLine = NULL;
    CellularPdnContextInfo_t * pPDPContextsInfo = ( CellularPdnContextInfo_t * ) pData;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    const CellularATCommandLine_t * pCommnadItem = NULL;
    uint8_t tokenIndex = 0;
    uint8_t contextId = 0;
    int32_t tempValue = 0;
    char * pToken = NULL;

    if( pContext == NULL )
    {
        IotLogError( "_Cellular_RecvFuncGetPdpContextSettings: invalid context" );
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if( ( pPDPContextsInfo == NULL ) || ( dataLen != sizeof( CellularPdnContextInfo_t ) ) )
    {
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if( pAtResp == NULL )
    {
        IotLogError( "_Cellular_RecvFuncGetPdpContextSettings: Response is invalid" );
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if( ( pAtResp->pItm == NULL ) || ( pAtResp->pItm->pLine == NULL ) )
    {
        IotLogError( "_Cellular_RecvFuncGetPdpContextSettings: no PDN context available" );
        pktStatus = CELLULAR_PKT_STATUS_OK;
    }
    else
    {
        pRespLine = pAtResp->pItm->pLine;

        pCommnadItem = pAtResp->pItm;

        while( pCommnadItem != NULL )
        {
            pRespLine = pCommnadItem->pLine;
            IotLogDebug( "_Cellular_RecvFuncGetPdpContextSettings: pRespLine [%s]", pRespLine );

            /* Removing all the Spaces in the AT Response. */
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces( pRespLine );

            if( atCoreStatus == CELLULAR_AT_SUCCESS )
            {
                atCoreStatus = Cellular_ATRemovePrefix( &pRespLine );

                if( atCoreStatus == CELLULAR_AT_SUCCESS )
                {
                    atCoreStatus = Cellular_ATRemoveAllDoubleQuote( pRespLine );
                }

                if( atCoreStatus == CELLULAR_AT_SUCCESS )
                {
                    atCoreStatus = Cellular_ATGetNextTok( &pRespLine, &pToken );
                }

                if( atCoreStatus == CELLULAR_AT_SUCCESS )
                {
                    tokenIndex = 0;

                    while( ( pToken != NULL ) && ( atCoreStatus == CELLULAR_AT_SUCCESS ) )
                    {
                        switch( tokenIndex )
                        {
                            case ( CELLULAR_PDN_STATUS_POS_CONTEXT_ID ):
                                IotLogDebug( "_Cellular_RecvFuncGetPdpContextSettings: Context Id pToken: %s", pToken );
                                atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

                                if( atCoreStatus == CELLULAR_AT_SUCCESS )
                                {
                                    if( ( tempValue >= ( int32_t ) CELLULAR_PDN_CONTEXT_ID_MIN ) &&
                                        ( tempValue <= ( int32_t ) MAX_PDP_CONTEXTS ) )
                                    {
                                        contextId = ( uint8_t ) tempValue;
                                        pPDPContextsInfo->contextsPresent[ contextId - 1 ] = TRUE;
                                        IotLogDebug( "_Cellular_RecvFuncGetPdpContextSettings: Context Id: %d", contextId );
                                    }
                                    else
                                    {
                                        IotLogError( "_Cellular_RecvFuncGetPdpContextSettings: Invalid Context Id. Token %s", pToken );
                                        atCoreStatus = CELLULAR_AT_ERROR;
                                    }
                                }

                                break;

                            case ( CELLULAR_PDN_STATUS_POS_CONTEXT_TYPE ):
                                IotLogDebug( "_Cellular_RecvFuncGetPdpContextSettings: Context Type pToken: %s", pToken );

                                ( void ) memcpy( ( void * ) pPDPContextsInfo->ipType[ contextId - 1 ],
                                                 ( void * ) pToken, CELULAR_PDN_CONTEXT_TYPE_MAX_SIZE + 1U );
                                break;

                            case ( CELLULAR_PDN_STATUS_POS_APN_NAME ):
                                IotLogDebug( "_Cellular_RecvFuncGetPdpContextSettings: Context APN name pToken: %s", pToken );

                                ( void ) memcpy( ( void * ) pPDPContextsInfo->apnName[ contextId - 1 ],
                                                 ( void * ) pToken, CELLULAR_APN_MAX_SIZE + 1U );
                                break;

                            case ( CELLULAR_PDN_STATUS_POS_IP_ADDRESS ):
                                IotLogDebug( "_Cellular_RecvFuncGetPdpContextSettings: Context IP address pToken: %s", pToken );

                                ( void ) memcpy( ( void * ) pPDPContextsInfo->ipAddress[ contextId - 1 ],
                                                 ( void * ) pToken, CELLULAR_IP_ADDRESS_MAX_SIZE + 1U );
                                break;

                            default:
                                break;
                        }

                        tokenIndex++;

                        if( Cellular_ATGetNextTok( &pRespLine, &pToken ) != CELLULAR_AT_SUCCESS )
                        {
                            break;
                        }
                    }
                }
            }

            pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );

            if( pktStatus != CELLULAR_PKT_STATUS_OK )
            {
                IotLogError( "_Cellular_RecvFuncGetPdpContextSettings: parse %s failed", pRespLine );
                break;
            }

            pCommnadItem = pCommnadItem->pNext;
        }
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* Set PDN APN name and Authentication setting */

CellularError_t Cellular_SetPdnConfig( CellularHandle_t cellularHandle,
                                       uint8_t contextId,
                                       const CellularPdnConfig_t * pPdnConfig )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    char cmdBuf[ CELLULAR_AT_CMD_MAX_SIZE ] = { '\0' };
    char pPdpTypeStr[ CELULAR_PDN_CONTEXT_TYPE_MAX_SIZE ] = { '\0' };

    CellularPdnContextInfo_t pdpContextsInfo = { 0 };
    CellularPdnContextInfo_t * pPdpContextsInfo = &pdpContextsInfo;

    CellularAtReq_t atReqSetPdn =
    {
        cmdBuf,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };

    if( pPdnConfig == NULL )
    {
        IotLogDebug( "Cellular_SetPdnConfig: Input parameter is NULL" );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        switch( pPdnConfig->pdnContextType )
        {
            case CELLULAR_PDN_CONTEXT_IPV4:
                ( void ) strncpy( pPdpTypeStr, "IP", 3U ); /* 3U is the length of "IP" + '\0'. */
                break;

            case CELLULAR_PDN_CONTEXT_IPV6:
                ( void ) strncpy( pPdpTypeStr, "IPV6", 5U ); /* 5U is the length of "IPV6" + '\0'. */
                break;

            case CELLULAR_PDN_CONTEXT_IPV4V6:
                ( void ) strncpy( pPdpTypeStr, "IPV4V6", 7U ); /* 7U is the length of "IPV4V6" + '\0'. */
                break;

            default:
                IotLogDebug( "Cellular_SetPdnConfig: Invalid pdn context type %d",
                             CELLULAR_PDN_CONTEXT_IPV4V6 );
                cellularStatus = CELLULAR_BAD_PARAMETER;
                break;
        }
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        cellularStatus = _Cellular_IsValidPdn( contextId );
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        /* Make sure the library is open. */
        cellularStatus = _Cellular_CheckLibraryStatus( pContext );
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        /* Check current APN name and IP type of contextId first to avoid unneccessary network de-registration. */
        /* TODO: This implementation currently assumes only one context in list. Need to add complete contexts list parsing */

        CellularAtReq_t atReqGetCurrentApnName =
        {
            "AT+CGDCONT?",
            CELLULAR_AT_MULTI_WITH_PREFIX,
            "+CGDCONT",
            _Cellular_RecvFuncGetPdpContextSettings,
            pPdpContextsInfo,
            sizeof( CellularPdnContextInfo_t ),
        };

        pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqGetCurrentApnName );
        cellularStatus = _Cellular_TranslatePktStatus( pktStatus );

        if( cellularStatus == CELLULAR_SUCCESS )
        {
            IotLogDebug( "Cellular_SetPdnConfig: Listing operator and context details below." );

            for( int i = 0; i < MAX_PDP_CONTEXTS - 1; i++ )
            {
                /* Print only those contexts that are present in +CGDCONT response */
                if( pdpContextsInfo.contextsPresent[ i ] )
                {
                    IotLogDebug( "Context [%d], IP Type [%s], APN Name [%s], IP Address [%s]\r\n", i + 1,
                                 pdpContextsInfo.ipType[ i ], pdpContextsInfo.apnName, pdpContextsInfo.ipAddress );
                }
            }
        }
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        /* Form the AT command. */

        /* The return value of snprintf is not used.
         * The max length of the string is fixed and checked offline. */
        /* coverity[misra_c_2012_rule_21_6_violation]. */

        if( ( strstr( pdpContextsInfo.apnName[ contextId - 1 ], pPdnConfig->apnName ) == NULL ) || ( strcmp( pdpContextsInfo.ipType[ contextId - 1 ], pPdpTypeStr ) != 0 ) )
        {
            if( strcmp( pdpContextsInfo.ipType[ contextId - 1 ], pPdpTypeStr ) != 0 )
            {
                IotLogInfo( "Cellular_SetPdnConfig: Setting new IPv (Module IPv:%s != %s)\r\n", pdpContextsInfo.ipType[ contextId - 1 ], pPdpTypeStr );
            }

            if( strstr( pdpContextsInfo.apnName[ contextId - 1 ], pPdnConfig->apnName ) == NULL )
            {
                IotLogInfo( "Cellular_SetPdnConfig: Setting new APN (Module APN:%s != %s)\r\n", pdpContextsInfo.apnName[ contextId - 1 ], pPdnConfig->apnName );
            }

            /* TODO: Add support if + UAUTHREQ is PAP / CHAP */
            ( void ) snprintf( cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "%s%d,\"%s\",\"%s\",,0,0;%s%d,%d%s", /*,\"%s\",\"%s\"      TODO: add if +UAUTHREQ is PAP/CHAP */
                               "AT+COPS=2;+CGDCONT=",
                               contextId,
                               pPdpTypeStr,
                               pPdnConfig->apnName,
                               "+UAUTHREQ=",
                               contextId,
                               pPdnConfig->pdnAuthType,
                               ";+COPS=0" );
            /*pPdnConfig->username, */
            /*pPdnConfig->password); */

            pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqSetPdn );

            if( pktStatus != CELLULAR_PKT_STATUS_OK )
            {
                IotLogError( "Cellular_SetPdnConfig: can't set PDN, cmdBuf:%s, PktRet: %d", cmdBuf, pktStatus );
                cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
            }
        }
        else
        {
            IotLogInfo( "Cellular_SetPdnConfig: APN and IPv already set.\r\n" );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
CellularError_t Cellular_SetPsmSettings( CellularHandle_t cellularHandle,
                                         const CellularPsmSettings_t * pPsmSettings )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    char cmdBuf[ CELLULAR_AT_CMD_MAX_SIZE ] = { '\0' };

    CellularAtReq_t atReqSetPsm =
    {
        cmdBuf,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0
    };

    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogError( "Cellular_SetPsmSettings: _Cellular_CheckLibraryStatus failed" );
    }
    else if( pPsmSettings == NULL )
    {
        IotLogError( "Cellular_SetPsmSettings : Bad parameter" );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        if( pPsmSettings->mode == 1 )
        {
            /*
             * SARA-R4: To change the command setting issue AT+COPS=2 or AT+CFUN=0 to deregister the module from
             * network, issue the +CPSMS command and reboot the module in order to apply the new configuration. */
            /* After PSM mode active, press "PWR_ON" key to awake modem or T3412 timer is expired. */
            ( void ) snprintf( cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+CFUN=0" );
            pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqSetPsm );

            if( pktStatus == CELLULAR_PKT_STATUS_OK )
            {
                cellularStatus = Cellular_CommonSetPsmSettings( cellularHandle, pPsmSettings );

                if( cellularStatus == CELLULAR_SUCCESS )
                {
                    cellularStatus = rebootCellularModem( pContext, false, true );
                }
                else
                {
                    IotLogError( "Cellular_SetPsmSettings: Unable to set PSM settings." );
                }
            }
        }
        else
        {
            cellularStatus = Cellular_CommonSetPsmSettings( cellularHandle, pPsmSettings );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
CellularError_t Cellular_SetEidrxSettings( CellularHandle_t cellularHandle,
                                           const CellularEidrxSettings_t * pEidrxSettings )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    char cmdBuf[ CELLULAR_AT_CMD_MAX_SIZE ] = { '\0' };

    CellularAtReq_t atReqSetEidrx =
    {
        cmdBuf,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0
    };

    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogError( "Cellular_SetEidrxSettings: _Cellular_CheckLibraryStatus failed" );
    }
    else if( pEidrxSettings == NULL )
    {
        IotLogError( "Cellular_SetEidrxSettings : Bad parameter" );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        if( ( pEidrxSettings->mode == 1 ) || ( pEidrxSettings->mode == 2 ) )
        {
            /*
             * SARA-R4: To change the command setting issue AT+COPS=2 or AT+CFUN=0 to deregister the module from
             * network, issue the +CEDRXS command and reboot the module in order to apply the new configuration.
             */
            ( void ) snprintf( cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+CFUN=0" );
            pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqSetEidrx );

            if( pktStatus == CELLULAR_PKT_STATUS_OK )
            {
                cellularStatus = Cellular_CommonSetEidrxSettings( cellularHandle, pEidrxSettings );

                if( cellularStatus == CELLULAR_SUCCESS )
                {
                    cellularStatus = rebootCellularModem( pContext, true, false );
                }
                else
                {
                    IotLogError( "Cellular_SetEidrxSettings: Unable to set Eidrx settings." );
                }
            }
        }
        else
        {
            cellularStatus = Cellular_CommonSetEidrxSettings( cellularHandle, pEidrxSettings );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/
