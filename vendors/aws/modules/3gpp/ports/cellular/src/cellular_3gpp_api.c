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
 * @brief cellular HAL API implemenation with 3GPP AT command.
 */

#include "iot_config.h"

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "queue.h"
#include "aws_cellular_config.h"
#include "cellular_config_defaults.h"
#include "cellular_internal.h"
#include "cellular_common_internal.h"
#include "cellular_common_api.h"
#include "cellular_at_core.h"
#include "cellular_types.h"

/*-----------------------------------------------------------*/

#define CELLULAR_CEDRXS_POS_ACT             ( 0U )
#define CELLULAR_CEDRXS_POS_VALUE           ( 1U )
#define CELLULAR_CEDRXS_MAX_ENTRY           ( 4U )

#define CELLULAR_AT_CMD_TYPICAL_MAX_SIZE    ( 32U )

#define PRINTF_BINARY_PATTERN_INT4          "%c%c%c%c"
#define PRINTF_BYTE_TO_BINARY_INT4( i )            \
    ( ( ( ( i ) & 0x08UL ) != 0UL ) ? '1' : '0' ), \
    ( ( ( ( i ) & 0x04UL ) != 0UL ) ? '1' : '0' ), \
    ( ( ( ( i ) & 0x02UL ) != 0UL ) ? '1' : '0' ), \
    ( ( ( ( i ) & 0x01UL ) != 0UL ) ? '1' : '0' )

/*-----------------------------------------------------------*/

/**
 * @brief operator information.
 */
typedef struct cellularOperatorInfo
{
    CellularPlmnInfo_t plmnInfo;                             /* Device registered PLMN info (MCC and MNC).  */
    CellularRat_t rat;                                       /* Device registered Radio Access Technology (Cat-M, Cat-NB, GPRS etc).  */
    CellularNetworkRegistrationMode_t networkRegMode;        /* Network Registered mode of the device (Manual, Auto etc).   */
    CellularOperatorNameFormat_t operatorNameFormat;         /* Format of registered network operator name. */
    char operatorName[ CELLULAR_NETWORK_NAME_MAX_SIZE + 1 ]; /* Registered network operator name. */
} cellularOperatorInfo_t;

/*-----------------------------------------------------------*/

static CellularPktStatus_t _parseTimeZoneInCCLKResponse( char ** ppToken,
                                                         bool * pTimeZoneSignNegative,
                                                         const char * pTimeZoneResp,
                                                         CellularTime_t * pTimeInfo );
static CellularPktStatus_t _parseYearMonthDayInCCLKResponse( char ** ppToken,
                                                             char ** ppTimeZoneResp,
                                                             CellularTime_t * pTimeInfo );
static CellularPktStatus_t _parseTimeInCCLKResponse( char ** ppToken,
                                                     bool timeZoneSignNegative,
                                                     char ** ppTimeZoneResp,
                                                     CellularTime_t * pTimeInfo );
static CellularPktStatus_t _parseTimeZoneInfo( char * pTimeZoneResp,
                                               CellularTime_t * pTimeInfo );
static CellularPktStatus_t _Cellular_RecvFuncGetNetworkTime( CellularContext_t * pContext,
                                                             const CellularATCommandResponse_t * pAtResp,
                                                             void * pData,
                                                             uint16_t dataLen );
static CellularPktStatus_t _Cellular_RecvFuncGetFirmwareVersion( CellularContext_t * pContext,
                                                                 const CellularATCommandResponse_t * pAtResp,
                                                                 void * pData,
                                                                 uint16_t dataLen );
static CellularPktStatus_t _Cellular_RecvFuncGetImei( CellularContext_t * pContext,
                                                      const CellularATCommandResponse_t * pAtResp,
                                                      void * pData,
                                                      uint16_t dataLen );
static CellularPktStatus_t _Cellular_RecvFuncGetModelId( CellularContext_t * pContext,
                                                         const CellularATCommandResponse_t * pAtResp,
                                                         void * pData,
                                                         uint16_t dataLen );
static CellularPktStatus_t _Cellular_RecvFuncGetManufactureId( CellularContext_t * pContext,
                                                               const CellularATCommandResponse_t * pAtResp,
                                                               void * pData,
                                                               uint16_t dataLen );
static CellularPktStatus_t _Cellular_RecvFuncGetNetworkReg( CellularContext_t * pContext,
                                                            const CellularATCommandResponse_t * pAtResp,
                                                            void * pData,
                                                            uint16_t dataLen );
static CellularError_t queryNetworkStatus( CellularContext_t * pContext,
                                           const char * pCommand,
                                           const char * pPrefix,
                                           CellularNetworkRegType_t regType );
static bool _parseCopsRegModeToken( char * pToken,
                                    cellularOperatorInfo_t * pOperatorInfo );
static bool _parseCopsNetworkNameFormatToken( const char * pToken,
                                              cellularOperatorInfo_t * pOperatorInfo );
static bool _parseCopsNetworkNameToken( const char * pToken,
                                        cellularOperatorInfo_t * pOperatorInfo );
static bool _parseCopsRatToken( const char * pToken,
                                cellularOperatorInfo_t * pOperatorInfo );
static CellularATError_t _parseCops( char * pCopsResponse,
                                     cellularOperatorInfo_t * pOperatorInfo );
static CellularPktStatus_t _Cellular_RecvFuncUpdateMccMnc( CellularContext_t * pContext,
                                                           const CellularATCommandResponse_t * pAtResp,
                                                           void * pData,
                                                           uint16_t dataLen );
static CellularPktStatus_t _Cellular_RecvFuncIpAddress( CellularContext_t * pContext,
                                                        const CellularATCommandResponse_t * pAtResp,
                                                        void * pData,
                                                        uint16_t dataLen );
static CellularATError_t parseEidrxToken( char * pToken,
                                          uint8_t tokenIndex,
                                          CellularEidrxSettingsList_t * pEidrxSettingsList,
                                          uint8_t count );
static CellularATError_t parseEidrxLine( char * pInputLine,
                                         uint8_t count,
                                         CellularEidrxSettingsList_t * pEidrxSettingsList );
static CellularPktStatus_t _Cellular_RecvFuncGetEidrxSettings( CellularContext_t * pContext,
                                                               const CellularATCommandResponse_t * pAtResp,
                                                               void * pData,
                                                               uint16_t dataLen );
static CellularError_t atcmdUpdateMccMnc( CellularContext_t * pContext,
                                          cellularOperatorInfo_t * pOperatorInfo );
static CellularError_t atcmdQueryRegStatus( CellularContext_t * pContext,
                                            CellularServiceStatus_t * pServiceStatus );

/*-----------------------------------------------------------*/

static CellularPktStatus_t _parseTimeZoneInCCLKResponse( char ** ppToken,
                                                         bool * pTimeZoneSignNegative,
                                                         const char * pTimeZoneResp,
                                                         CellularTime_t * pTimeInfo )
{
    int32_t tempValue = 0;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    /* Get Time Zone info. */
    *ppToken = strstr( pTimeZoneResp, "+" );

    if( *ppToken == NULL )
    {
        *ppToken = strstr( pTimeZoneResp, "-" );

        if( *ppToken != NULL )
        {
            /* Setting the timeZoneNegative value to 1 for processing seconds later. */
            *pTimeZoneSignNegative = true;
        }
    }

    if( *ppToken != NULL )
    {
        atCoreStatus = Cellular_ATStrtoi( *ppToken, 10, &tempValue );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            pTimeInfo->timeZone = tempValue;
        }
        else
        {
            IotLogError( "Error in Processing TimeZone Information. Token %s", *ppToken );
        }
    }

    pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );

    return pktStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _parseYearMonthDayInCCLKResponse( char ** ppToken,
                                                             char ** ppTimeZoneResp,
                                                             CellularTime_t * pTimeInfo )
{
    int32_t tempValue = 0;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    atCoreStatus = Cellular_ATStrtoi( *ppToken, 10, &tempValue );

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        if( ( tempValue >= 0 ) && ( tempValue <= ( int32_t ) UINT16_MAX ) )
        {
            pTimeInfo->year = ( uint16_t ) tempValue;
        }
        else
        {
            IotLogError( "Error in Processing Year. Token %s", *ppToken );
            atCoreStatus = CELLULAR_AT_ERROR;
        }
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        /* Getting the next token to process month in the CCLK AT response. */
        atCoreStatus = Cellular_ATGetSpecificNextTok( ppTimeZoneResp, "/", ppToken );
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATStrtoi( *ppToken, 10, &tempValue );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( ( tempValue >= 0 ) &&
                ( tempValue <= ( int32_t ) UINT8_MAX ) )
            {
                pTimeInfo->month = ( uint8_t ) tempValue;
            }
            else
            {
                IotLogError( "Error in Processing month. Token %s", *ppToken );
                atCoreStatus = CELLULAR_AT_ERROR;
            }
        }
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        /* Getting the next token to process date in the CCLK AT response. */
        atCoreStatus = Cellular_ATGetSpecificNextTok( ppTimeZoneResp, ",", ppToken );
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATStrtoi( *ppToken, 10, &tempValue );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( ( tempValue >= 0 ) && ( tempValue <= ( int32_t ) UINT8_MAX ) )
            {
                pTimeInfo->day = ( uint8_t ) tempValue;
            }
            else
            {
                IotLogError( "Error in Processing Day. token %s", *ppToken );
                atCoreStatus = CELLULAR_AT_ERROR;
            }
        }
    }

    pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    return pktStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _parseTimeInCCLKResponse( char ** ppToken,
                                                     bool timeZoneSignNegative,
                                                     char ** ppTimeZoneResp,
                                                     CellularTime_t * pTimeInfo )
{
    int32_t tempValue = 0;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

    atCoreStatus = Cellular_ATStrtoi( *ppToken, 10, &tempValue );

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        if( ( tempValue >= 0 ) && ( tempValue <= ( int32_t ) UINT8_MAX ) )
        {
            pTimeInfo->hour = ( uint8_t ) tempValue;
        }
        else
        {
            IotLogError( "Error in Processing Hour. token %s", *ppToken );
            atCoreStatus = CELLULAR_AT_ERROR;
        }
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        /* Getting the next Token to process Minute in the CCLK AT Response. */
        atCoreStatus = Cellular_ATGetSpecificNextTok( ppTimeZoneResp, ":", ppToken );
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATStrtoi( *ppToken, 10, &tempValue );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( ( tempValue >= 0 ) && ( tempValue <= ( int32_t ) UINT8_MAX ) )
            {
                pTimeInfo->minute = ( uint8_t ) tempValue;
            }
            else
            {
                IotLogError( "Error in Processing minute. Token %s", *ppToken );
                atCoreStatus = CELLULAR_AT_ERROR;
            }
        }
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        /* Getting the next token to process Second in the CCLK AT Response.
         * Get the next token based on the signedness of the Timezone. */
        if( !timeZoneSignNegative )
        {
            atCoreStatus = Cellular_ATGetSpecificNextTok( ppTimeZoneResp, "+", ppToken );
        }
        else
        {
            atCoreStatus = Cellular_ATGetSpecificNextTok( ppTimeZoneResp, "-", ppToken );
        }
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATStrtoi( *ppToken, 10, &tempValue );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( ( tempValue >= 0 ) && ( tempValue <= ( int32_t ) UINT8_MAX ) )
            {
                pTimeInfo->second = ( uint8_t ) tempValue;
            }
            else
            {
                IotLogError( "Error in Processing Second. Token %s", *ppToken );
                atCoreStatus = CELLULAR_AT_ERROR;
            }
        }
    }

    pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    return pktStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _parseTimeZoneInfo( char * pTimeZoneResp,
                                               CellularTime_t * pTimeInfo )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    char * pToken = NULL;
    bool timeZoneSignNegative = false;
    char * pTempTimeZoneResp = NULL;

    if( ( pTimeZoneResp == NULL ) || ( pTimeInfo == NULL ) )
    {
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
        pTempTimeZoneResp = pTimeZoneResp;
        ( void ) memset( pTimeInfo, 0, sizeof( CellularTime_t ) );
        atCoreStatus = Cellular_ATRemoveOutermostDoubleQuote( &pTempTimeZoneResp );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            pktStatus = _parseTimeZoneInCCLKResponse( &pToken, &timeZoneSignNegative,
                                                      pTempTimeZoneResp, pTimeInfo );
        }

        if( pktStatus == CELLULAR_PKT_STATUS_OK )
        {
            /* Getting the next token to process year in the CCLK AT response. */
            atCoreStatus = Cellular_ATGetSpecificNextTok( &pTempTimeZoneResp, "/", &pToken );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            pktStatus = _parseYearMonthDayInCCLKResponse( &pToken,
                                                          &pTempTimeZoneResp, pTimeInfo );
        }

        if( pktStatus == CELLULAR_PKT_STATUS_OK )
        {
            /* Getting the next token to process hour in the CCLK AT response. */
            atCoreStatus = Cellular_ATGetSpecificNextTok( &pTempTimeZoneResp, ":", &pToken );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            pktStatus = _parseTimeInCCLKResponse( &pToken, timeZoneSignNegative,
                                                  &pTempTimeZoneResp, pTimeInfo );
        }
        else
        {
            pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
        }
    }

    if( pktStatus == CELLULAR_PKT_STATUS_OK )
    {
        IotLogDebug( "\n TimeZoneInfo: Timezone %d Year %d Month %d day %d  Hour %d Minute %d Second %d ",
                     pTimeInfo->timeZone, pTimeInfo->year,
                     pTimeInfo->month, pTimeInfo->day,
                     pTimeInfo->hour, pTimeInfo->minute,
                     pTimeInfo->second );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL prototype. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static CellularPktStatus_t _Cellular_RecvFuncGetNetworkTime( CellularContext_t * pContext,
                                                             const CellularATCommandResponse_t * pAtResp,
                                                             void * pData,
                                                             uint16_t dataLen )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    char * pRespLine = NULL;

    if( pContext == NULL )
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if( ( pAtResp == NULL ) || ( pAtResp->pItm == NULL ) ||
             ( pAtResp->pItm->pLine == NULL ) || ( pData == NULL ) || ( dataLen == 0u ) )
    {
        IotLogError( "GetNetworkTime: Response is invalid " );
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
        pRespLine = pAtResp->pItm->pLine;
        atCoreStatus = Cellular_ATRemovePrefix( &pRespLine );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces( pRespLine );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            pktStatus = _parseTimeZoneInfo( pRespLine, ( CellularTime_t * ) pData );
        }
        else
        {
            pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
        }
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL prototype. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static CellularPktStatus_t _Cellular_RecvFuncGetFirmwareVersion( CellularContext_t * pContext,
                                                                 const CellularATCommandResponse_t * pAtResp,
                                                                 void * pData,
                                                                 uint16_t dataLen )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    char * pRespLine = NULL;

    if( pContext == NULL )
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if( ( pAtResp == NULL ) || ( pAtResp->pItm == NULL ) || ( pAtResp->pItm->pLine == NULL ) ||
             ( pData == NULL ) || ( dataLen != ( CELLULAR_FW_VERSION_MAX_SIZE + 1U ) ) )
    {
        IotLogError( "GetFirmwareVersion: Response is invalid " );
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
        pRespLine = pAtResp->pItm->pLine;
        atCoreStatus = Cellular_ATRemoveLeadingWhiteSpaces( &pRespLine );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATRemoveTrailingWhiteSpaces( pRespLine );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            ( void ) strncpy( ( char * ) pData, pRespLine, CELLULAR_FW_VERSION_MAX_SIZE );
        }

        pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL prototype. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static CellularPktStatus_t _Cellular_RecvFuncGetImei( CellularContext_t * pContext,
                                                      const CellularATCommandResponse_t * pAtResp,
                                                      void * pData,
                                                      uint16_t dataLen )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    char * pRespLine = NULL;

    if( pContext == NULL )
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if( ( pAtResp == NULL ) || ( pAtResp->pItm == NULL ) || ( pAtResp->pItm->pLine == NULL ) ||
             ( pData == NULL ) || ( dataLen != ( CELLULAR_IMEI_MAX_SIZE + 1U ) ) )
    {
        IotLogError( "GetImei: Response is invalid " );
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
        pRespLine = pAtResp->pItm->pLine;
        atCoreStatus = Cellular_ATRemoveLeadingWhiteSpaces( &pRespLine );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces( pRespLine );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            ( void ) strncpy( ( char * ) pData, pRespLine, CELLULAR_IMEI_MAX_SIZE );
        }

        pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL prototype. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static CellularPktStatus_t _Cellular_RecvFuncGetModelId( CellularContext_t * pContext,
                                                         const CellularATCommandResponse_t * pAtResp,
                                                         void * pData,
                                                         uint16_t dataLen )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    char * pRespLine = NULL;

    if( pContext == NULL )
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if( ( pAtResp == NULL ) || ( pAtResp->pItm == NULL ) || ( pAtResp->pItm->pLine == NULL ) ||
             ( pData == NULL ) || ( dataLen != ( CELLULAR_MODEL_ID_MAX_SIZE + 1U ) ) )
    {
        IotLogError( "GetModelId: Response is invalid " );
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
        pRespLine = pAtResp->pItm->pLine;
        atCoreStatus = Cellular_ATRemoveLeadingWhiteSpaces( &pRespLine );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATRemoveTrailingWhiteSpaces( pRespLine );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            ( void ) strncpy( ( char * ) pData, pRespLine, CELLULAR_MODEL_ID_MAX_SIZE );
        }

        pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL prototype. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static CellularPktStatus_t _Cellular_RecvFuncGetManufactureId( CellularContext_t * pContext,
                                                               const CellularATCommandResponse_t * pAtResp,
                                                               void * pData,
                                                               uint16_t dataLen )
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    char * pRespLine = NULL;

    if( pContext == NULL )
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if( ( pAtResp == NULL ) || ( pAtResp->pItm == NULL ) || ( pAtResp->pItm->pLine == NULL ) ||
             ( pData == NULL ) || ( dataLen != ( CELLULAR_MANUFACTURE_ID_MAX_SIZE + 1U ) ) )
    {
        IotLogError( "GetManufactureId: Response is invalid " );
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
        pRespLine = pAtResp->pItm->pLine;
        atCoreStatus = Cellular_ATRemoveLeadingWhiteSpaces( &pRespLine );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATRemoveTrailingWhiteSpaces( pRespLine );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            ( void ) strncpy( ( char * ) pData, pRespLine, CELLULAR_MANUFACTURE_ID_MAX_SIZE );
        }

        pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL prototype. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static CellularPktStatus_t _Cellular_RecvFuncGetNetworkReg( CellularContext_t * pContext,
                                                            const CellularATCommandResponse_t * pAtResp,
                                                            void * pData,
                                                            uint16_t dataLen )
{
    char * pPregLine = NULL;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularNetworkRegType_t regType = CELLULAR_REG_TYPE_UNKNOWN;

    if( pContext == NULL )
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if( ( pAtResp == NULL ) || ( pAtResp->pItm == NULL ) || ( pAtResp->pItm->pLine == NULL ) ||
             ( pData == NULL ) || ( dataLen != sizeof( CellularNetworkRegType_t ) ) )
    {
        IotLogError( "_Cellular_RecvFuncGetPsreg: response is invalid" );
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
        regType = *( ( CellularNetworkRegType_t * ) pData );
        pPregLine = pAtResp->pItm->pLine;
        atCoreStatus = Cellular_ATRemoveLeadingWhiteSpaces( &pPregLine );
        pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );

        if( pktStatus == CELLULAR_PKT_STATUS_OK )
        {
            /* Assumption is that the data is null terminated so we don't need the dataLen. */
            _Cellular_LockAtDataMutex( pContext );
            pktStatus = _Cellular_ParseRegStatus( pContext, pPregLine, false, regType );
            _Cellular_UnlockAtDataMutex( pContext );
        }

        IotLogDebug( "atcmd network register status %d pktStatus:%d", regType, pktStatus );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

static CellularError_t queryNetworkStatus( CellularContext_t * pContext,
                                           const char * pCommand,
                                           const char * pPrefix,
                                           CellularNetworkRegType_t regType )
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularNetworkRegType_t recvRegType = regType;

    CellularAtReq_t atReqGetResult =
    {
        pCommand,
        CELLULAR_AT_WITH_PREFIX,
        pPrefix,
        _Cellular_RecvFuncGetNetworkReg,
        &recvRegType,
        sizeof( CellularNetworkRegType_t )
    };

    if( pContext == NULL )
    {
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else
    {
        pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqGetResult );
        cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

static bool _parseCopsRegModeToken( char * pToken,
                                    cellularOperatorInfo_t * pOperatorInfo )
{
    bool parseStatus = true;
    int32_t var = 0;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;

    if( ( pOperatorInfo == NULL ) || ( pToken == NULL ) )
    {
        IotLogError( "_parseCopsRegMode: Input Parameter NULL" );
        parseStatus = false;
    }
    else
    {
        atCoreStatus = Cellular_ATStrtoi( pToken, 10, &var );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            if( ( var >= 0 ) && ( var < ( int32_t ) CELLULAR_NETWORK_REGISTRATION_MODE_MAX ) )
            {
                /* Variable "var" is ensured that it is valid and within
                 * a valid range. Hence, assigning the value of the variable to
                 * networkRegMode with a enum cast. */
                /* coverity[misra_c_2012_rule_10_5_violation] */
                pOperatorInfo->networkRegMode = ( CellularNetworkRegistrationMode_t ) var;
            }
            else
            {
                IotLogError( "_parseCopsRegMode: Error in processing Network Registration mode. Token %s", pToken );
                parseStatus = false;
            }
        }
    }

    return parseStatus;
}

/*-----------------------------------------------------------*/

static bool _parseCopsNetworkNameFormatToken( const char * pToken,
                                              cellularOperatorInfo_t * pOperatorInfo )
{
    bool parseStatus = true;
    int32_t var = 0;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;

    if( ( pOperatorInfo == NULL ) || ( pToken == NULL ) )
    {
        IotLogError( "_parseCopsNetworkNameFormat: Input Parameter NULL" );
        parseStatus = false;
    }
    else
    {
        atCoreStatus = Cellular_ATStrtoi( pToken, 10, &var );

        if( ( atCoreStatus == CELLULAR_AT_SUCCESS ) && ( var >= 0 ) &&
            ( var < ( int32_t ) CELLULAR_OPERATOR_NAME_FORMAT_MAX ) )
        {
            /* Variable "var" is ensured that it is valid and within
             * a valid range. Hence, assigning the value of the variable to
             * operatorNameFormat with a enum cast. */
            /* coverity[misra_c_2012_rule_10_5_violation] */
            pOperatorInfo->operatorNameFormat = ( CellularOperatorNameFormat_t ) var;
        }
        else
        {
            IotLogError( "_parseCopsNetworkNameFormat: Error in processing Network Registration mode. Token %s", pToken );
            parseStatus = false;
        }
    }

    return parseStatus;
}

/*-----------------------------------------------------------*/

static bool _parseCopsNetworkNameToken( const char * pToken,
                                        cellularOperatorInfo_t * pOperatorInfo )
{
    bool parseStatus = true;
    uint32_t mccMncLen = 0U;

    if( ( pOperatorInfo == NULL ) || ( pToken == NULL ) )
    {
        IotLogError( "_parseCopsNetworkName: Input Parameter NULL" );
        parseStatus = false;
    }
    else
    {
        if( ( pOperatorInfo->operatorNameFormat == CELLULAR_OPERATOR_NAME_FORMAT_LONG ) ||
            ( pOperatorInfo->operatorNameFormat == CELLULAR_OPERATOR_NAME_FORMAT_SHORT ) )
        {
            ( void ) strncpy( pOperatorInfo->operatorName, pToken, CELLULAR_NETWORK_NAME_MAX_SIZE );
        }
        else if( pOperatorInfo->operatorNameFormat == CELLULAR_OPERATOR_NAME_FORMAT_NUMERIC )
        {
            mccMncLen = ( uint32_t ) strlen( pToken );

            if( ( mccMncLen == ( CELLULAR_MCC_MAX_SIZE + CELLULAR_MNC_MAX_SIZE ) ) ||
                ( mccMncLen == ( CELLULAR_MCC_MAX_SIZE + CELLULAR_MNC_MAX_SIZE - 1U ) ) )
            {
                ( void ) strncpy( pOperatorInfo->plmnInfo.mcc, pToken, CELLULAR_MCC_MAX_SIZE );
                pOperatorInfo->plmnInfo.mcc[ CELLULAR_MCC_MAX_SIZE ] = '\0';
                ( void ) strncpy( pOperatorInfo->plmnInfo.mnc, &pToken[ CELLULAR_MCC_MAX_SIZE ],
                                  ( mccMncLen - CELLULAR_MCC_MAX_SIZE + 1u ) );
                pOperatorInfo->plmnInfo.mnc[ CELLULAR_MNC_MAX_SIZE ] = '\0';
            }
            else
            {
                IotLogError( "_parseCopsNetworkName: Error in processing Network MCC MNC: Length not Valid" );
                parseStatus = false;
            }
        }
        else
        {
            IotLogError( "Error in processing Operator Name: Format Unknown" );
            parseStatus = false;
        }
    }

    return parseStatus;
}

/*-----------------------------------------------------------*/

static bool _parseCopsRatToken( const char * pToken,
                                cellularOperatorInfo_t * pOperatorInfo )
{
    bool parseStatus = true;
    int32_t var = 0;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;

    if( ( pOperatorInfo == NULL ) || ( pToken == NULL ) )
    {
        IotLogError( "_parseCopsNetworkName:Input Parameter NULL" );
        parseStatus = false;
    }
    else
    {
        atCoreStatus = Cellular_ATStrtoi( pToken, 10, &var );

        if( ( atCoreStatus == CELLULAR_AT_SUCCESS ) &&
            ( var < ( int32_t ) CELLULAR_RAT_MAX ) && ( var >= 0 ) )
        {
            /* Variable "var" is ensured that it is valid and within
             * a valid range. Hence, assigning the value of the variable to
             * rat with a enum cast. */
            /* coverity[misra_c_2012_rule_10_5_violation] */
            pOperatorInfo->rat = ( CellularRat_t ) var;
        }
        else
        {
            IotLogError( "_parseCopsNetworkName: Error in processing RAT. Token %s", pToken );
            parseStatus = false;
        }
    }

    return parseStatus;
}

/*-----------------------------------------------------------*/

/* internal function of _Cellular_RecvFuncUpdateMccMnc to reduce complexity. */
/* Cellular HAL prototype. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static CellularATError_t _parseCops( char * pCopsResponse,
                                     cellularOperatorInfo_t * pOperatorInfo )
{
    char * pToken = NULL;
    char * pTempCopsResponse = pCopsResponse;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    bool parseStatus = false;

    /* Getting next token from COPS response. */
    atCoreStatus = Cellular_ATGetNextTok( &pTempCopsResponse, &pToken );

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        parseStatus = _parseCopsRegModeToken( pToken, pOperatorInfo );

        if( parseStatus == false )
        {
            atCoreStatus = CELLULAR_AT_ERROR;
        }
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATGetNextTok( &pTempCopsResponse, &pToken );
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        parseStatus = _parseCopsNetworkNameFormatToken( pToken, pOperatorInfo );

        if( parseStatus == false )
        {
            atCoreStatus = CELLULAR_AT_ERROR;
        }
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATGetNextTok( &pTempCopsResponse, &pToken );
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        parseStatus = _parseCopsNetworkNameToken( pToken, pOperatorInfo );

        if( parseStatus == false )
        {
            atCoreStatus = CELLULAR_AT_ERROR;
        }
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATGetNextTok( &pTempCopsResponse, &pToken );
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        parseStatus = _parseCopsRatToken( pToken, pOperatorInfo );

        if( parseStatus == false )
        {
            atCoreStatus = CELLULAR_AT_ERROR;
        }
    }

    return atCoreStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL prototype. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static CellularPktStatus_t _Cellular_RecvFuncUpdateMccMnc( CellularContext_t * pContext,
                                                           const CellularATCommandResponse_t * pAtResp,
                                                           void * pData,
                                                           uint16_t dataLen )
{
    char * pCopsResponse = NULL;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    cellularOperatorInfo_t * pOperatorInfo = NULL;

    if( pContext == NULL )
    {
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if( ( pAtResp == NULL ) || ( pAtResp->pItm == NULL ) || ( pAtResp->pItm->pLine == NULL ) ||
             ( pData == NULL ) || ( dataLen != sizeof( cellularOperatorInfo_t ) ) )
    {
        IotLogError( "UpdateMccMnc: input_line passed is NULL" );
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
        pCopsResponse = pAtResp->pItm->pLine;
        pOperatorInfo = ( cellularOperatorInfo_t * ) pData;

        /* Remove COPS Prefix. */
        atCoreStatus = Cellular_ATRemovePrefix( &pCopsResponse );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            /* Removing all the Quotes from the COPS response. */
            atCoreStatus = Cellular_ATRemoveAllDoubleQuote( pCopsResponse );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            /* Removing all Space from the COPS response. */
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces( pCopsResponse );
        }

        /* parse all the data from cops. */
        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = _parseCops( pCopsResponse, pOperatorInfo );
        }

        if( atCoreStatus == CELLULAR_AT_ERROR )
        {
            IotLogError( "ERROR: COPS %s", pCopsResponse );
            pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
        }
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL prototype. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static CellularPktStatus_t _Cellular_RecvFuncIpAddress( CellularContext_t * pContext,
                                                        const CellularATCommandResponse_t * pAtResp,
                                                        void * pData,
                                                        uint16_t dataLen )
{
    char * pInputLine = NULL, * pToken = NULL;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;

    if( pContext == NULL )
    {
        IotLogError( "Recv IP address: Invalid context" );
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if( ( pAtResp == NULL ) || ( pAtResp->pItm == NULL ) || ( pAtResp->pItm->pLine == NULL ) )
    {
        IotLogError( "Recv IP address: response is invalid" );
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if( ( pData == NULL ) || ( dataLen == 0U ) )
    {
        IotLogError( "Recv IP address: Invalid param" );
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
        pInputLine = pAtResp->pItm->pLine;
        atCoreStatus = Cellular_ATRemovePrefix( &pInputLine );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATGetNextTok( &pInputLine, &pToken );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            IotLogDebug( "Recv IP address: Context id: %s", pToken );

            if( pInputLine[ 0 ] != '\0' )
            {
                atCoreStatus = Cellular_ATGetNextTok( &pInputLine, &pToken );
            }
            else
            {
                /* This is the case "+CGPADDR: 1". Return "0.0.0.0" in this case.*/
                strncpy( pData, "0,0,0,0", dataLen );
            }
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            IotLogDebug( "Recv IP address: Ip Addr: %s", pToken );
            ( void ) strncpy( pData, pToken, dataLen );
        }

        pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

static CellularATError_t parseEidrxToken( char * pToken,
                                          uint8_t tokenIndex,
                                          CellularEidrxSettingsList_t * pEidrxSettingsList,
                                          uint8_t count )
{
    int32_t tempValue = 0;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;

    switch( tokenIndex )
    {
        case CELLULAR_CEDRXS_POS_ACT:
            /* coverity[misra_c_2012_rule_22_9_violation] */
            atCoreStatus = Cellular_ATStrtoi( pToken, 10, &tempValue );

            if( atCoreStatus == CELLULAR_AT_SUCCESS )
            {
                if( ( tempValue >= 0 ) &&
                    ( tempValue <= ( int32_t ) UINT8_MAX ) )
                {
                    pEidrxSettingsList->eidrxList[ count ].rat = ( uint8_t ) tempValue;
                }
                else
                {
                    IotLogError( "Error in processing RAT value. Token %s", pToken );
                    atCoreStatus = CELLULAR_AT_ERROR;
                }
            }

            break;

        case CELLULAR_CEDRXS_POS_VALUE:
            atCoreStatus = Cellular_ATStrtoi( pToken, 2, &tempValue );

            if( atCoreStatus == CELLULAR_AT_SUCCESS )
            {
                if( ( tempValue >= 0 ) &&
                    ( tempValue <= ( int32_t ) UINT8_MAX ) )
                {
                    pEidrxSettingsList->eidrxList[ count ].requestedEdrxVaue = ( uint8_t ) tempValue;
                }
                else
                {
                    IotLogError( "Error in processing Requested Edrx value. Token %s", pToken );
                    atCoreStatus = CELLULAR_AT_ERROR;
                }
            }

            break;

        default:
            IotLogDebug( "Unknown Parameter Position in AT+CEDRXS Response" );
            atCoreStatus = CELLULAR_AT_ERROR;
            break;
    }

    return atCoreStatus;
}

/*-----------------------------------------------------------*/

static CellularATError_t parseEidrxLine( char * pInputLine,
                                         uint8_t count,
                                         CellularEidrxSettingsList_t * pEidrxSettingsList )
{
    char * pToken = NULL;
    char * pLocalInputLine = pInputLine;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    uint8_t tokenIndex = 0;

    atCoreStatus = Cellular_ATRemovePrefix( &pLocalInputLine );

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATRemoveAllDoubleQuote( pLocalInputLine );
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        atCoreStatus = Cellular_ATGetNextTok( &pLocalInputLine, &pToken );
    }

    if( atCoreStatus == CELLULAR_AT_SUCCESS )
    {
        tokenIndex = 0;

        while( pToken != NULL )
        {
            if( parseEidrxToken( pToken, tokenIndex, pEidrxSettingsList, count ) != CELLULAR_AT_SUCCESS )
            {
                IotLogInfo( "parseEidrxToken %s index %d failed", pToken, tokenIndex );
            }

            tokenIndex++;

            if( Cellular_ATGetNextTok( &pLocalInputLine, &pToken ) != CELLULAR_AT_SUCCESS )
            {
                break;
            }
        }
    }

    return atCoreStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL prototype. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static CellularPktStatus_t _Cellular_RecvFuncGetEidrxSettings( CellularContext_t * pContext,
                                                               const CellularATCommandResponse_t * pAtResp,
                                                               void * pData,
                                                               uint16_t dataLen )
{
    char * pInputLine = NULL;
    uint8_t count = 0;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularEidrxSettingsList_t * pEidrxSettingsList = NULL;
    const CellularATCommandLine_t * pCommnadItem = NULL;

    if( pContext == NULL )
    {
        IotLogError( "GetEidrxSettings: Invalid context" );
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if( ( pAtResp == NULL ) || ( pAtResp->pItm == NULL ) ||
             ( pAtResp->pItm->pLine == NULL ) || ( pData == NULL ) || ( dataLen != CELLULAR_EDRX_LIST_MAX_SIZE ) )
    {
        IotLogError( "GetEidrxSettings: Invalid param" );
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
        pEidrxSettingsList = ( CellularEidrxSettingsList_t * ) pData;
        pCommnadItem = pAtResp->pItm;

        while( pCommnadItem != NULL )
        {
            pInputLine = pCommnadItem->pLine;
            atCoreStatus = parseEidrxLine( pInputLine, count, pEidrxSettingsList );
            pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );

            if( pktStatus != CELLULAR_PKT_STATUS_OK )
            {
                IotLogError( "GetEidrx: Parsing Error encountered, atCoreStatus: %d", atCoreStatus );
            }
            else
            {
                IotLogDebug( "GetEidrx setting[%d]: RAT: %d, Value: 0x%x",
                             count, pEidrxSettingsList->eidrxList[ count ].rat, pEidrxSettingsList->eidrxList[ count ].requestedEdrxVaue );
            }

            pCommnadItem = pCommnadItem->pNext;
            count++;
            pEidrxSettingsList->count = count;
        }
    }

    return pktStatus;
}

/*-----------------------------------------------------------*/

/* Cellular common prototype. */
/* coverity[misra_c_2012_rule_8_13_violation] */
static CellularError_t atcmdUpdateMccMnc( CellularContext_t * pContext,
                                          cellularOperatorInfo_t * pOperatorInfo )
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus;
    CellularAtReq_t atReqGetMccMnc =
    {
        "AT+COPS?",
        CELLULAR_AT_WITH_PREFIX,
        "+COPS",
        _Cellular_RecvFuncUpdateMccMnc,
        pOperatorInfo,
        sizeof( cellularOperatorInfo_t ),
    };

    pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqGetMccMnc );
    cellularStatus = _Cellular_TranslatePktStatus( pktStatus );

    return cellularStatus;
}

/*-----------------------------------------------------------*/

static CellularError_t atcmdQueryRegStatus( CellularContext_t * pContext,
                                            CellularServiceStatus_t * pServiceStatus )
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularNetworkRegistrationStatus_t psRegStatus = CELLULAR_NETWORK_REGISTRATION_STATUS_UNKNOWN;
    const cellularAtData_t * pLibAtData = NULL;

    if( pContext == NULL )
    {
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else
    {
        cellularStatus = queryNetworkStatus( pContext, "AT+CREG?", "+CREG", CELLULAR_REG_TYPE_CREG );

        if( cellularStatus == CELLULAR_SUCCESS )
        {
            cellularStatus = queryNetworkStatus( pContext, "AT+CGREG?", "+CGREG", CELLULAR_REG_TYPE_CGREG );
        }

        /* Check if modem acquired GPRS Registration. */
        /* Query CEREG only if the modem did not already acquire PS registration. */
        _Cellular_LockAtDataMutex( pContext );
        psRegStatus = pContext->libAtData.psRegStatus;
        _Cellular_UnlockAtDataMutex( pContext );

        if( ( cellularStatus == CELLULAR_SUCCESS ) &&
            ( psRegStatus != CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME ) &&
            ( psRegStatus != CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING ) )
        {
            cellularStatus = queryNetworkStatus( pContext, "AT+CEREG?", "+CEREG", CELLULAR_REG_TYPE_CEREG );
        }
    }

    /* Get the service status from lib AT data. */
    if( ( cellularStatus == CELLULAR_SUCCESS ) && ( pServiceStatus != NULL ) )
    {
        pLibAtData = &pContext->libAtData;
        _Cellular_LockAtDataMutex( pContext );
        pServiceStatus->rat = pLibAtData->rat;
        pServiceStatus->csRegistrationStatus = pLibAtData->csRegStatus;
        pServiceStatus->psRegistrationStatus = pLibAtData->psRegStatus;
        pServiceStatus->csRejectionCause = pLibAtData->csRejCause;
        pServiceStatus->csRejectionType = pLibAtData->csRejectType;
        pServiceStatus->psRejectionCause = pLibAtData->psRejCause;
        pServiceStatus->psRejectionType = pLibAtData->psRejectType;
        _Cellular_UnlockAtDataMutex( pContext );
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonGetEidrxSettings( CellularHandle_t cellularHandle,
                                                 CellularEidrxSettingsList_t * pEidrxSettingsList )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularAtReq_t atReqGetEidrx =
    {
        "AT+CEDRXS?",
        CELLULAR_AT_MULTI_WITH_PREFIX,
        "+CEDRXS",
        _Cellular_RecvFuncGetEidrxSettings,
        pEidrxSettingsList,
        CELLULAR_EDRX_LIST_MAX_SIZE,
    };

    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else if( pEidrxSettingsList == NULL )
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        ( void ) memset( pEidrxSettingsList, 0, sizeof( CellularEidrxSettingsList_t ) );
        /* we should always query the pEidrxSettings from the network. */
        pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqGetEidrx );

        if( pktStatus != CELLULAR_PKT_STATUS_OK )
        {
            IotLogError( "Cellular_GetEidrxSettings: couldn't retrieve Eidrx settings" );
            cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonSetEidrxSettings( CellularHandle_t cellularHandle,
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
        0,
    };

    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else if( pEidrxSettings == NULL )
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* Form the AT command. */

        /* The return value of snprintf is not used.
         * The max length of the string is fixed and checked offline. */
        /* coverity[misra_c_2012_rule_21_6_violation]. */
        ( void ) snprintf( cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "%s%d,%d,\"" PRINTF_BINARY_PATTERN_INT4 "\"",
                           "AT+CEDRXS=",
                           pEidrxSettings->mode,
                           pEidrxSettings->rat,
                           PRINTF_BYTE_TO_BINARY_INT4( pEidrxSettings->requestedEdrxVaue ) );
        IotLogDebug( "Eidrx setting: %s ", cmdBuf );
        /* we should always query the PSMsettings from the network*/
        pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqSetEidrx );

        if( pktStatus != CELLULAR_PKT_STATUS_OK )
        {
            IotLogError( "_Cellular_SetEidrxSettings: couldn't set Eidrx settings" );
            cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonRfOn( CellularHandle_t cellularHandle )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus;
    CellularPktStatus_t pktStatus;
    CellularAtReq_t atReq =
    {
        "AT+CFUN=1",
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };

    /* Make sure library is open. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReq );
        cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonRfOff( CellularHandle_t cellularHandle )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus;
    CellularPktStatus_t pktStatus;
    CellularAtReq_t atReq =
    {
        "AT+CFUN=4",
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };

    /* Make sure library is open. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReq );
        cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonGetRegisteredNetwork( CellularHandle_t cellularHandle,
                                                     CellularPlmnInfo_t * pNetworkInfo )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    cellularOperatorInfo_t operatorInfo = { 0 };

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else if( pNetworkInfo == NULL )
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        cellularStatus = atcmdUpdateMccMnc( pContext, &operatorInfo );
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        if( operatorInfo.rat != CELLULAR_RAT_INVALID )
        {
            ( void ) memcpy( pNetworkInfo->mcc, operatorInfo.plmnInfo.mcc, CELLULAR_MCC_MAX_SIZE );
            pNetworkInfo->mcc[ CELLULAR_MCC_MAX_SIZE ] = '\0';
            ( void ) memcpy( pNetworkInfo->mnc, operatorInfo.plmnInfo.mnc, CELLULAR_MNC_MAX_SIZE );
            pNetworkInfo->mnc[ CELLULAR_MNC_MAX_SIZE ] = '\0';
        }
        else
        {
            cellularStatus = CELLULAR_UNKNOWN;
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonGetServiceStatus( CellularHandle_t cellularHandle,
                                                 CellularServiceStatus_t * pServiceStatus )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    cellularOperatorInfo_t operatorInfo = { 0 };

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else if( pServiceStatus == NULL )
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* Always query and update the cellular Lib AT data. */
        ( void ) atcmdQueryRegStatus( pContext, pServiceStatus );
        ( void ) atcmdUpdateMccMnc( pContext, &operatorInfo );

        /* Service status data from operator info. */
        pServiceStatus->networkRegistrationMode = operatorInfo.networkRegMode;
        pServiceStatus->plmnInfo = operatorInfo.plmnInfo;
        ( void ) strncpy( pServiceStatus->operatorName, operatorInfo.operatorName, CELLULAR_NETWORK_NAME_MAX_SIZE );
        pServiceStatus->operatorNameFormat = operatorInfo.operatorNameFormat;

        IotLogDebug( "SrvStatus: rat %d cs %d, ps %d, mode %d, csRejType %d, csRej %d, psRejType %d, psRej %d, plmn %s%s",
                     pServiceStatus->rat,
                     pServiceStatus->csRegistrationStatus, pServiceStatus->psRegistrationStatus,
                     pServiceStatus->networkRegistrationMode,
                     pServiceStatus->csRejectionType, pServiceStatus->csRejectionCause,
                     pServiceStatus->psRejectionType, pServiceStatus->psRejectionCause,
                     pServiceStatus->plmnInfo.mcc, pServiceStatus->plmnInfo.mnc );
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL prototype. */
/* coverity[misra_c_2012_rule_8_13_violation] */
CellularError_t Cellular_CommonGetNetworkTime( CellularHandle_t cellularHandle,
                                               CellularTime_t * pNetworkTime )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularAtReq_t atReqGetNetworkTime =
    {
        "AT+CCLK?",
        CELLULAR_AT_WITH_PREFIX,
        "+CCLK",
        _Cellular_RecvFuncGetNetworkTime,
        pNetworkTime,
        sizeof( CellularTime_t )
    };

    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else if( pNetworkTime == NULL )
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqGetNetworkTime );

        if( pktStatus != CELLULAR_PKT_STATUS_OK )
        {
            IotLogError( "Cellular_GetNetworkTime: couldn't retrieve Network Time" );
            cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

CellularError_t Cellular_CommonGetModemInfo( CellularHandle_t cellularHandle,
                                             CellularModemInfo_t * pModemInfo )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularAtReq_t atReqGetFirmwareVersion =
    {
        "AT+CGMR",
        CELLULAR_AT_WO_PREFIX,
        NULL,
        _Cellular_RecvFuncGetFirmwareVersion,
        pModemInfo->firmwareVersion,
        CELLULAR_FW_VERSION_MAX_SIZE + 1U,
    };
    CellularAtReq_t atReqGetImei =
    {
        "AT+CGSN",
        CELLULAR_AT_WO_PREFIX,
        NULL,
        _Cellular_RecvFuncGetImei,
        pModemInfo->imei,
        CELLULAR_IMEI_MAX_SIZE + 1U,
    };
    CellularAtReq_t atReqGetModelId =
    {
        "AT+CGMM",
        CELLULAR_AT_WO_PREFIX,
        NULL,
        _Cellular_RecvFuncGetModelId,
        pModemInfo->modelId,
        CELLULAR_MODEL_ID_MAX_SIZE + 1U,
    };
    CellularAtReq_t atReqGetManufactureId =
    {
        "AT+CGMI",
        CELLULAR_AT_WO_PREFIX,
        NULL,
        _Cellular_RecvFuncGetManufactureId,
        pModemInfo->manufactureId,
        CELLULAR_MANUFACTURE_ID_MAX_SIZE + 1U,
    };

    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else if( pModemInfo == NULL )
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        ( void ) memset( pModemInfo, 0, sizeof( CellularModemInfo_t ) );
        pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqGetFirmwareVersion );

        if( pktStatus != CELLULAR_PKT_STATUS_OK )
        {
            cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
        }

        pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqGetImei );

        if( pktStatus != CELLULAR_PKT_STATUS_OK )
        {
            cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
        }

        pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqGetModelId );

        if( pktStatus != CELLULAR_PKT_STATUS_OK )
        {
            cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
        }

        pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqGetManufactureId );

        if( pktStatus != CELLULAR_PKT_STATUS_OK )
        {
            cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
        }

        if( cellularStatus == CELLULAR_SUCCESS )
        {
            IotLogDebug( "ModemInfo: hwVer:%s, fwVer:%s, serialNum:%s, IMEI:%s, manufactureId:%s, modelId:%s ",
                         pModemInfo->hardwareVersion, pModemInfo->firmwareVersion, pModemInfo->serialNumber, pModemInfo->imei,
                         pModemInfo->manufactureId, pModemInfo->modelId );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Cellular HAL prototype. */
/* coverity[misra_c_2012_rule_8_13_violation] */
CellularError_t Cellular_CommonGetIPAddress( CellularHandle_t cellularHandle,
                                             uint8_t contextId,
                                             char * pBuffer,
                                             uint32_t bufferLength )
{
    CellularContext_t * pContext = ( CellularContext_t * ) cellularHandle;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    char cmdBuf[ CELLULAR_AT_CMD_TYPICAL_MAX_SIZE ] = { '\0' };
    CellularAtReq_t atReqGetIp =
    {
        cmdBuf,
        CELLULAR_AT_WITH_PREFIX,
        "+CGPADDR",
        _Cellular_RecvFuncIpAddress,
        pBuffer,
        bufferLength
    };

    /* Make sure the library is open. */
    cellularStatus = _Cellular_CheckLibraryStatus( pContext );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        IotLogDebug( "_Cellular_CheckLibraryStatus failed" );
    }
    else if( ( pBuffer == NULL ) || ( bufferLength == 0U ) )
    {
        IotLogError( "_Cellular_GetIPAddress: Invalid parameter" );
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        cellularStatus = _Cellular_IsValidPdn( contextId );
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        /* Form the AT command. */

        /* The return value of snprintf is not used.
         * The max length of the string is fixed and checked offline. */
        /* coverity[misra_c_2012_rule_21_6_violation]. */
        ( void ) snprintf( cmdBuf, CELLULAR_AT_CMD_TYPICAL_MAX_SIZE, "%s%d", "AT+CGPADDR=", contextId );
        pktStatus = _Cellular_AtcmdRequestWithCallback( pContext, atReqGetIp );

        if( pktStatus != CELLULAR_PKT_STATUS_OK )
        {
            IotLogError( "_Cellular_GetIPAddress: couldn't retrieve the IP, cmdBuf:%s, pktStatus: %d", cmdBuf, pktStatus );
            cellularStatus = _Cellular_TranslatePktStatus( pktStatus );
        }
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

void _Cellular_DestroyAtDataMutex( CellularContext_t * pContext )
{
    if( pContext != NULL )
    {
        IotMutex_Destroy( &pContext->libAtDataMutex );
    }
}

/*-----------------------------------------------------------*/

bool _Cellular_CreateAtDataMutex( CellularContext_t * pContext )
{
    bool status = false;

    if( pContext != NULL )
    {
        status = IotMutex_Create( &pContext->libAtDataMutex, false );
    }

    return status;
}

/*-----------------------------------------------------------*/

void _Cellular_LockAtDataMutex( CellularContext_t * pContext )
{
    if( pContext != NULL )
    {
        IotMutex_Lock( &pContext->libAtDataMutex );
    }
}

/*-----------------------------------------------------------*/

void _Cellular_UnlockAtDataMutex( CellularContext_t * pContext )
{
    if( pContext != NULL )
    {
        IotMutex_Unlock( &pContext->libAtDataMutex );
    }
}

/*-----------------------------------------------------------*/

/* mode 0: Clean everything.
 * mode 1: Only clean the fields for creg/cgreg=0 URC. */
void _Cellular_InitAtData( CellularContext_t * pContext,
                           uint32_t mode )
{
    cellularAtData_t * pLibAtData = NULL;

    if( pContext != NULL )
    {
        pLibAtData = &pContext->libAtData;

        if( mode == 0u )
        {
            ( void ) memset( pLibAtData, 0, sizeof( cellularAtData_t ) );
            pLibAtData->csRegStatus = CELLULAR_NETWORK_REGISTRATION_STATUS_NOT_REGISTERED_SEARCHING;
            pLibAtData->psRegStatus = CELLULAR_NETWORK_REGISTRATION_STATUS_NOT_REGISTERED_SEARCHING;
        }

        pLibAtData->lac = 0xFFFFU;
        pLibAtData->cellId = 0xFFFFFFFFU;
        pLibAtData->rat = CELLULAR_RAT_INVALID;
        pLibAtData->rac = 0xFF;
    }
}

/*-----------------------------------------------------------*/
