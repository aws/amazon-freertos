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

/* stdlib includes. */
#include "string.h"
#include "stdio.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "iot_secure_sockets.h"

/* Unity framework includes. */
#include "unity_fixture.h"

/* CELLULAR include. */
#include "aws_cellular_config.h"
#include "cellular_config_defaults.h"
#include "cellular_api.h"
#include "test_cellular_api.h"
#include "cellular_types.h"
#include "cellular_comm_interface.h"
#include "iot_secure_sockets.h"

/* Testing variable includes. */
#include "aws_test_runner.h"
#include "aws_test_tcp.h"
#include "aws_test_utils.h"
#include "iot_config.h"
#include "aws_clientcredential.h"

/* Testing configurations definitions. */

/* Custom CELLULAR Test asserts. */
#define TEST_CELLULAR_ASSERT_REQUIRED_API( condition, result )            \
    if( result == CELLULAR_UNSUPPORTED )                                  \
    {                                                                     \
        TEST_FAIL_MESSAGE( "Required CELLULAR API is not implemented." ); \
    }                                                                     \
    else                                                                  \
    {                                                                     \
        TEST_ASSERT( condition );                                         \
    }

#define TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( condition, result, message ) \
    if( result == CELLULAR_UNSUPPORTED )                                    \
    {                                                                       \
        TEST_FAIL_MESSAGE( "Required CELLULAR API is not implemented." );   \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        TEST_ASSERT_MESSAGE( condition, message );                          \
    }

#define TEST_CELLULAR_ASSERT_OPTIONAL_API( condition, result ) \
    if( result == CELLULAR_UNSUPPORTED )                       \
    {                                                          \
        TEST_ASSERT( 1 );                                      \
    }                                                          \
    else                                                       \
    {                                                          \
        TEST_ASSERT( condition );                              \
    }

#define TEST_CELLULAR_ASSERT_OPTIONAL_API_MSG( condition, result, message ) \
    if( result == CELLULAR_UNSUPPORTED )                                    \
    {                                                                       \
        TEST_ASSERT( 1 );                                                   \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        TEST_ASSERT_MESSAGE( condition, message );                          \
    }

/* Test TCP echo server address. */
#define testtcpECHO_SERVER_ADDRESS                                               \
    SOCKETS_inet_addr_quick( tcptestECHO_SERVER_ADDR0, tcptestECHO_SERVER_ADDR1, \
                             tcptestECHO_SERVER_ADDR2, tcptestECHO_SERVER_ADDR3 )

/* Test state variables. */
static CellularSocketHandle_t _socketHandleSet;
static uint8_t _simReady = 0;
static uint8_t _dataReady = 0;
static uint8_t _AtResp = 0;
static char _AtRespTxt[ 100 ] = { 0 };
static uint8_t _socketOpened = 0;
static CellularCommInterface_t * _pCommIntf = NULL;
static CellularHandle_t _cellularHandle = NULL;

/* Callback functions for testing */
static void prvCellularSocketDataReadyCallback( CellularSocketHandle_t socketHandle,
                                                void * pCallbackContext )
{
    configPRINTF( ( "Data Ready on Socket" ) );
    _dataReady = 1;
}

static void prvnetworkRegistrationCallback( CellularUrcEvent_t urcEvent,
                                            const CellularServiceStatus_t * pServiceStatus )
{
    if( !urcEvent || !pServiceStatus )
    {
        return;
    }
    else
    {
        if( ( urcEvent == CELLULAR_URC_EVENT_NETWORK_CS_REGISTRATION ) ||
            ( urcEvent == CELLULAR_URC_EVENT_NETWORK_PS_REGISTRATION ) )
        {
            configPRINTF( ( "URC received: %d. \r\n", pServiceStatus->csRegistrationStatus ) );
            configPRINTF( ( "URC received: %d. \r\n", pServiceStatus->psRegistrationStatus ) );
        }
    }
}

static CellularPktStatus_t prvresponseReceivedCallback( CellularHandle_t cellularHandle,
                                                        const CellularATCommandResponse_t * pAtResp,
                                                        void * pData,
                                                        uint16_t dataLen )
{
    if( !pAtResp || !pAtResp->status )
    {
        configPRINTF( ( "AT command error. \r\n" ) );
        return CELLULAR_BAD_PARAMETER;
    }
    else
    {
        configPRINTF( ( "AT command final response: %d. \r\n", pAtResp->status ) );
    }

    if( pAtResp->pItm )
    {
        CellularATCommandLine_t * pCurr = pAtResp->pItm;

        while( pCurr )
        {
            if( pCurr->pLine )
            {
                configPRINTF( ( "AT command Intermediate response: %s. \r\n", pAtResp->pItm->pLine ) );
            }

            _AtResp = 1;
            strncpy( _AtRespTxt, pAtResp->pItm->pLine, 100 );
            pCurr = pCurr->pNext;
        }
    }
    else
    {
        configPRINTF( ( "AT command No intermediate response. \r\n" ) );
    }

    configPRINTF( ( "AT command set and ready. \r\n" ) );
    return CELLULAR_SUCCESS;
}

static void prvsignalStrengthChangedCallback( CellularUrcEvent_t urcEvent,
                                              const CellularSignalInfo_t * pSignalInfo )
{
    if( ( pSignalInfo != NULL ) && ( urcEvent == CELLULAR_URC_EVENT_SIGNAL_CHANGED ) )
    {
        if( pSignalInfo->rssi != CELLULAR_INVALID_SIGNAL_VALUE )
        {
            configPRINTF( ( "RSSI received: %d. \r\n", pSignalInfo->rssi ) );
        }
        else
        {
            configPRINTF( ( "RSSI received: UNKNOWN. \r\n" ) );
        }

        if( pSignalInfo->rsrp != CELLULAR_INVALID_SIGNAL_VALUE )
        {
            configPRINTF( ( "RSRP received: %d. \r\n", pSignalInfo->rsrp ) );
        }
        else
        {
            configPRINTF( ( "RSRP received: UNKNOWN. \r\n" ) );
        }

        if( pSignalInfo->rsrq != CELLULAR_INVALID_SIGNAL_VALUE )
        {
            configPRINTF( ( "RSRQ received: %d. \r\n", pSignalInfo->rsrq ) );
        }
        else
        {
            configPRINTF( ( "RSRQ received: UNKNOWN. \r\n" ) );
        }

        if( pSignalInfo->ber != CELLULAR_INVALID_SIGNAL_VALUE )
        {
            configPRINTF( ( "BER received: %d. \r\n", pSignalInfo->ber ) );
        }
        else
        {
            configPRINTF( ( "BER received: UNKNOWN. \r\n" ) );
        }

        if( pSignalInfo->bars != CELLULAR_INVALID_SIGNAL_BAR_VALUE )
        {
            configPRINTF( ( "BARS received: %u. \r\n", pSignalInfo->bars ) );
        }
        else
        {
            configPRINTF( ( "BARS received: UNKNOWN. \r\n" ) );
        }
    }
}

static void prvpdnEventCallback( CellularUrcEvent_t urcEvent,
                                 uint8_t contextId )
{
    if( !urcEvent )
    {
        return;
    }
    else
    {
        if( ( urcEvent == CELLULAR_URC_EVENT_PDN_ACTIVATED ) || ( urcEvent == CELLULAR_URC_EVENT_PDN_DEACTIVATED ) )
        {
            configPRINTF( ( "PDP Status changed. \r\n" ) );
        }
    }
}

static void prvdclosedCallback( CellularSocketHandle_t socketHandle,
                                void * pCallbackContext )
{
    if( !socketHandle )
    {
        return;
    }
    else
    {
        configPRINTF( ( "Socket is closed. \r\n" ) );
    }
}

static void prvCellularSocketOpenCallback( CellularUrcEvent_t urcEvent,
                                           CellularSocketHandle_t socketHandle,
                                           void * pCallbackContext )
{
    if( urcEvent == CELLULAR_URC_SOCKET_OPENED )
    {
        configPRINTF( ( "Socket open callback, Success" ) );
        _socketOpened = 1;
    }
    else
    {
        _socketOpened = 2;
        configPRINTF( ( "Socket open callback, Failure" ) );
    }
}

/**
 * @brief Connect to the CELLULAR and verify success.
 */
static BaseType_t prvConnectCellular( void )
{
    BaseType_t xResult = pdPASS;
    CellularError_t xCellularStatus;
    CellularSimCardStatus_t simStatus;
    CellularServiceStatus_t serviceStatus;
    CellularCommInterface_t * pCommIntf = &CellularCommInterface;
    const uint32_t waitInterval = 500UL;
    uint8_t tries = 0;
    CellularPdnConfig_t pdnConfig = { CELLULAR_PDN_CONTEXT_IPV4, CELLULAR_PDN_AUTH_NONE, testCELLULAR_APN, "", "" };
    CellularPdnStatus_t PdnStatusBuffers;
    char localIP[ 32 ] = { '\0' };
    uint32_t timeoutMilliseconds = 20000UL;
    uint32_t timeoutCountLimit = ( timeoutMilliseconds / waitInterval ) + 1U;
    uint32_t timeoutCount = 0;
    uint8_t NumStatus = 1;
    uint8_t pdnContextId = 1;

    /* Initialize Cellular Comm Interface */
    _pCommIntf = pCommIntf;
    xCellularStatus = Cellular_Init( &_cellularHandle, pCommIntf );

    if( xCellularStatus != CELLULAR_SUCCESS )
    {
        configPRINTF( ( ">>>  Cellular module can't initialized  <<<" ) );
        xResult = pdFAIL;
    }
    else
    {
        xResult = pdPASS;
    }

    if( ( xCellularStatus == CELLULAR_SUCCESS ) && ( xResult == pdPASS ) )
    {
        /* wait until SIM is ready */
        while( tries < CELLULAR_MAX_SIM_RETRY )
        {
            xCellularStatus = Cellular_GetSimCardStatus( _cellularHandle, &simStatus );

            if( xCellularStatus == CELLULAR_SUCCESS )
            {
                if( ( simStatus.simCardState == CELLULAR_SIM_CARD_INSERTED ) &&
                    ( simStatus.simCardLockState == CELLULAR_SIM_CARD_READY ) )
                {
                    _simReady = 1;
                    xResult = pdPASS;
                    break;
                }
            }
            else
            {
                configPRINTF( ( ">>>  Cellular SIM failure  <<<" ) );
                xResult = pdFAIL;
                break;
            }

            vTaskDelay( pdMS_TO_TICKS( waitInterval ) );
            tries++;
        }
    }

    /* Activate Cellular */
    while( timeoutCount < timeoutCountLimit )
    {
        xCellularStatus = Cellular_GetServiceStatus( _cellularHandle, &serviceStatus );

        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            if( ( serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME ) ||
                ( serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING ) )
            {
                break;
            }
            else
            {
                timeoutCount++;

                if( timeoutCount >= timeoutCountLimit )
                {
                    configPRINTF( ( ">>>  Cellular module can't be registered1  <<<" ) );
                }

                vTaskDelay( pdMS_TO_TICKS( waitInterval * 2 ) );
            }
        }
        else
        {
            configPRINTF( ( ">>>  Cellular module can't be registered2  <<<" ) );
        }
    }

    if( ( xCellularStatus == CELLULAR_SUCCESS ) && ( xResult == pdPASS ) )
    {
        xCellularStatus = Cellular_RegisterUrcSignalStrengthChangedCallback( _cellularHandle, &prvsignalStrengthChangedCallback );
        xCellularStatus = Cellular_RegisterUrcNetworkRegistrationEventCallback( _cellularHandle, &prvnetworkRegistrationCallback );
        xCellularStatus = Cellular_RegisterUrcPdnEventCallback( _cellularHandle, &prvpdnEventCallback );
        xCellularStatus = Cellular_SetPdnConfig( _cellularHandle, pdnContextId, &pdnConfig );
        xCellularStatus = Cellular_ActivatePdn( _cellularHandle, pdnContextId );
        xCellularStatus = Cellular_GetIPAddress( _cellularHandle, pdnContextId, localIP, sizeof( localIP ) );
        xCellularStatus = Cellular_GetPdnStatus( _cellularHandle, &PdnStatusBuffers, pdnContextId, &NumStatus );
        xCellularStatus = Cellular_SetDns( _cellularHandle, pdnContextId, "8.8.8.8" );
    }

    if( ( xCellularStatus == CELLULAR_SUCCESS ) && ( PdnStatusBuffers.state == 1 ) && ( sizeof( localIP ) == 32 ) )
    {
        xResult = pdPASS;
    }
    else
    {
        xResult = pdFAIL;
    }

    return xResult;
}

static BaseType_t prvIsConnectedCellular( void )
{
    CellularError_t xCellularStatus;
    CellularPdnStatus_t PdnStatusBuffers;
    uint8_t NumStatus = 1;
    BaseType_t xResult = pdPASS;

    xCellularStatus = Cellular_GetPdnStatus( _cellularHandle, &PdnStatusBuffers, 1, &NumStatus );

    if( ( xCellularStatus == CELLULAR_SUCCESS ) && ( PdnStatusBuffers.state == 1 ) )
    {
        xResult = pdPASS;
    }
    else
    {
        xResult = pdFAIL;
    }

    return xResult;
}

static BaseType_t prvFinishCellularTesting( void )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    BaseType_t xResult = pdPASS;

    xCellularStatus = Cellular_Cleanup( _cellularHandle );

    if( xCellularStatus != CELLULAR_SUCCESS )
    {
        configPRINTF( ( ">>>  Cellular module cleanup failed  <<<" ) );
        xResult = pdFAIL;
    }
    else
    {
        _cellularHandle = NULL;
        xResult = pdPASS;
        _simReady = 0;
    }

    return xResult;
}


/* Unity TEST initializations. */
TEST_GROUP( Full_CELLULAR_API );

TEST_SETUP( Full_CELLULAR_API )
{
    CellularError_t xCellularStatus;
    CellularModemInfo_t modemInfo;
    CellularSimCardInfo_t simCardInfo;
    CellularSimCardStatus_t simStatus;
    CellularSignalInfo_t signalInfo;
    char localIP[ 32 ] = { '\0' };
    CellularPlmnInfo_t networkInfo;
    CellularServiceStatus_t serviceStatus;
    CellularTime_t networkTime;
    CellularPsmSettings_t psmSettings;
    CellularEidrxSettingsList_t eidrxSettingsList;
    uint8_t pdnContextId = 1;

    vTaskDelay( pdMS_TO_TICKS( 2000 ) );

    configPRINTF( ( "\r\n==================================================================================\r\n" ) );

    xCellularStatus = Cellular_GetModemInfo( _cellularHandle, &modemInfo );

    if( xCellularStatus == CELLULAR_SUCCESS )
    {
        configPRINTF(
            ( " FW: %s \r\n IMEI: %s \r\n MfrID/ModId: %s/%s \r\n", modemInfo.firmwareVersion, modemInfo.imei, modemInfo.manufactureId, modemInfo.modelId ) );
    }
    else
    {
        configPRINTF( ( " FW: \r\n IMEI: \r\n MfrID/ModId: \r\n" ) );
    }

    xCellularStatus = Cellular_GetSimCardInfo( _cellularHandle, &simCardInfo );

    if( xCellularStatus == CELLULAR_SUCCESS )
    {
        configPRINTF(
            ( " ICCID: %s \r\n IMSI: %s \r\n HPLMN: %s-%s \r\n", simCardInfo.iccid, simCardInfo.imsi, simCardInfo.plmn.mcc, simCardInfo.plmn.mnc ) );
    }
    else
    {
        configPRINTF( ( " ICCID: \r\n IMSI: \r\n HPLMN: \r\n" ) );
    }

    xCellularStatus = Cellular_GetSimCardStatus( _cellularHandle, &simStatus );

    if( xCellularStatus == CELLULAR_SUCCESS )
    {
        configPRINTF( ( " SIM Status: %d \r\n SIM Lock: %d \r\n", simStatus.simCardState, simStatus.simCardLockState ) );
    }
    else
    {
        configPRINTF( ( " SIM Status: \r\n SIM Lock: \r\n" ) );
    }

    xCellularStatus = Cellular_GetSignalInfo( _cellularHandle, &signalInfo );

    if( xCellularStatus == CELLULAR_SUCCESS )
    {
        configPRINTF(
            ( " Signal Bars: %d \r\n Signal RSSI: %d \r\n Signal RSRP: %d \r\n Signal RSRQ: %d \r\n", signalInfo.bars, signalInfo.rssi, signalInfo.rsrp, signalInfo.rsrq ) );
    }
    else
    {
        configPRINTF(
            ( " Signal Bars: \r\n Signal RSSI: \r\n Signal RSRP: \r\n Signal RSRQ: \r\n", signalInfo.bars, signalInfo.rssi, signalInfo.rsrp, signalInfo.rsrq ) );
    }

    xCellularStatus = Cellular_GetServiceStatus( _cellularHandle, &serviceStatus );

    if( xCellularStatus == CELLULAR_SUCCESS )
    {
        configPRINTF(
            ( " rat: %d \r\n cs: %d \r\n ps: %d \r\n mode: %d \r\n csRej: %d \r\n psRej: %d \r\n plmn: %s%s \r\n", serviceStatus.rat, serviceStatus.csRegistrationStatus, serviceStatus.psRegistrationStatus, serviceStatus.networkRegistrationMode, serviceStatus.csRejectionCause, serviceStatus.psRejectionCause, serviceStatus.plmnInfo.mcc, serviceStatus.plmnInfo.mnc ) );
    }
    else
    {
        configPRINTF( ( " rat: \r\n cs: \r\n ps: \r\n mode: \r\n csRej: \r\n psRej: \r\n plmn: \r\n" ) );
    }

    xCellularStatus = Cellular_GetRegisteredNetwork( _cellularHandle, &networkInfo );

    if( xCellularStatus == CELLULAR_SUCCESS )
    {
        configPRINTF( ( " Network: %s-%s \r\n", networkInfo.mcc, networkInfo.mnc ) );
    }
    else
    {
        configPRINTF( ( " Network: \r\n" ) );
    }

    xCellularStatus = Cellular_GetNetworkTime( _cellularHandle, &networkTime );

    if( xCellularStatus == CELLULAR_SUCCESS )
    {
        configPRINTF(
            ( " Network time: %d/%d/%d %d:%d:%d \r\n", networkTime.month, networkTime.day, networkTime.year, networkTime.hour, networkTime.minute, networkTime.second ) );
    }
    else
    {
        configPRINTF( ( " Network time: \r\n" ) );
    }

    xCellularStatus = Cellular_ATCommandRaw( _cellularHandle, "+QCFG", "AT+QCFG=\"nwscanseq\"", CELLULAR_AT_WITH_PREFIX,
                                             &prvresponseReceivedCallback, NULL, 0 );

    if( xCellularStatus == CELLULAR_SUCCESS )
    {
        char * AtRespRatPriority = NULL;
        char * token = strtok( _AtRespTxt, "," );

        while( token != NULL )
        {
            AtRespRatPriority = token;
            token = strtok( NULL, "," );
        }

        snprintf( _AtRespTxt, 100, AtRespRatPriority );
        configPRINTF( ( " RAT Priority:%s \r\n", _AtRespTxt ) );
    }
    else
    {
        configPRINTF( ( " RAT Priority: \r\n" ) );
    }

    xCellularStatus = Cellular_GetIPAddress( _cellularHandle, pdnContextId, localIP, sizeof( localIP ) );

    if( xCellularStatus == CELLULAR_SUCCESS )
    {
        configPRINTF( ( " IP address: %s \r\n", localIP ) );
    }
    else
    {
        configPRINTF( ( " IP address: \r\n" ) );
    }

    xCellularStatus = Cellular_GetPsmSettings( _cellularHandle, &psmSettings );

    if( xCellularStatus == CELLULAR_SUCCESS )
    {
        configPRINTF(
            ( " PSM mode: %d \r\n PSM TAU Value: %d \r\n PSM RAU Value: %d \r\n PSM GPRS Timer: %d \r\n PSM Active Value: %d \r\n", psmSettings.mode, psmSettings.periodicTauValue, psmSettings.periodicRauValue, psmSettings.gprsReadyTimer, psmSettings.activeTimeValue ) );
    }
    else
    {
        configPRINTF(
            ( " PSM mode: \r\n PSM TAU Value: \r\n PSM RAU Value: \r\n PSM GPRS Timer: \r\n PSM Active Value: \r\n" ) );
    }

    xCellularStatus = Cellular_GetEidrxSettings( _cellularHandle, &eidrxSettingsList );

    if( xCellularStatus == CELLULAR_SUCCESS )
    {
        for( int i = 0; i < eidrxSettingsList.count; i++ )
        {
            configPRINTF(
                ( " eDRX index: %d eDRX mode: %d eDRX rat:%d eDRX UE Value:%d eDRX NW value:%d \r\n", i, eidrxSettingsList.eidrxList[ i ].mode, eidrxSettingsList.eidrxList[ i ].rat, eidrxSettingsList.eidrxList[ i ].requestedEdrxVaue, eidrxSettingsList.eidrxList[ i ].nwProvidedEdrxVaue ) );
        }
    }
    else
    {
        configPRINTF( ( " eDRX index: eDRX mode: eDRX rat: eDRX UE Value: eDRX NW value: \r\n" ) );
    }

    configPRINTF( ( "\r\n==================================================================================\r\n" ) );
}

TEST_TEAR_DOWN( Full_CELLULAR_API )
{
    configPRINTF( ( "\r\n==================================================================================\r\n" ) );
}

TEST_GROUP_RUNNER( Full_CELLULAR_API )
{
    /* List of all tests under this group */
    /* In sequence tests. */
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_Configure );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_Activate );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_TCPDataTransfer );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_EidrxSettings );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_PsmSettings );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_RatPriority );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_AirplaneMode );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_Deactivate );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_UnConfigure );

    /* Null parameter tests. */
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_GetModemInfo_NullParameters );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_GetSimCardInfo_NullParameters );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_GetSimCardStatus_NullParameters );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_GetServiceStatus_NullParameters );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_GetSignalInfo_NullParameters );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_GetRegisteredNetwork_NullParameters );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_GetPsmSettings_NullParameters );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_GetEidrxSettings_NullParameters );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_GetPdnStatus_NullParameters );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_GetIPAddress_NullParameters );

    /* Invalid parameters tests. */
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_SetRatPriority_InvalidMode );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_SetPsmSettings_InvalidMode );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_SetEidrxSettings_InvalidMode );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_SetPdnConfig_InvalidMode );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_SetDns_InvalidMode );

    /* Stability tests. */
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_Data_Loop );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_AirplaneMode_Loop );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_Power_Loop );

    /* HotSpot tests if supported*/
    #if ( testHotSpotENABLE_CONFIGURE_AP_TESTS == 1 )
        RUN_TEST_CASE( Full_CELLULAR_API, AFQP_WiFiConfigureAP );
    #endif
    prvFinishCellularTesting();
}

/**
 * @brief Configure CELLULAR
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_Configure )
{
    CellularError_t xCellularStatus;
    CellularSimCardStatus_t simStatus;
    CellularCommInterface_t * pCommIntf = &CellularCommInterface;
    const uint32_t waitInterval = 500UL;
    uint8_t tries = 0;

    if( TEST_PROTECT() )
    {
        _pCommIntf = pCommIntf;
        xCellularStatus = Cellular_Init( &_cellularHandle, pCommIntf );
        TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                               ">>>  Cellular module can't be initialized  <<<" );

        /* wait until SIM is ready */
        while( tries < CELLULAR_MAX_SIM_RETRY )
        {
            xCellularStatus = Cellular_GetSimCardStatus( _cellularHandle, &simStatus );

            if( xCellularStatus == CELLULAR_SUCCESS )
            {
                if( ( simStatus.simCardState == CELLULAR_SIM_CARD_INSERTED ) &&
                    ( simStatus.simCardLockState == CELLULAR_SIM_CARD_READY ) )
                {
                    _simReady = 1;
                    break;
                }
            }
            else
            {
                TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                                       ">>>  Cellular SIM failure  <<<" );
                break;
            }

            vTaskDelay( pdMS_TO_TICKS( waitInterval ) );
            tries++;
        }

        TEST_ASSERT( _simReady != 0 );

        /* Enable Callbacks. */
        xCellularStatus = Cellular_RegisterUrcSignalStrengthChangedCallback( _cellularHandle, &prvsignalStrengthChangedCallback );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        xCellularStatus = Cellular_RegisterUrcNetworkRegistrationEventCallback( _cellularHandle, &prvnetworkRegistrationCallback );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        xCellularStatus = Cellular_RegisterUrcPdnEventCallback( _cellularHandle, &prvpdnEventCallback );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief CELLULAR Activate
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_Activate )
{
    CellularError_t xCellularStatus;
    CellularServiceStatus_t serviceStatus;
    CellularPdnConfig_t pdnConfig =
    { CELLULAR_PDN_CONTEXT_IPV4, CELLULAR_PDN_AUTH_NONE, testCELLULAR_APN, "", "" };
    CellularPdnStatus_t PdnStatusBuffers;
    char localIP[ 32 ] = { '\0' };
    const uint32_t waitInterval = 1000UL;
    uint32_t timeoutMilliseconds = 20000UL;
    uint32_t timeoutCountLimit = ( timeoutMilliseconds / waitInterval ) + 1U;
    uint32_t timeoutCount = 0;
    uint8_t numStatus = 1;
    uint8_t pdnContextId = 1;
    char pIpAddress[ CELLULAR_IP_ADDRESS_MAX_SIZE ] = { 0 };

    if( TEST_PROTECT() )
    {
        /* Verify registration. */
        while( timeoutCount < timeoutCountLimit )
        {
            xCellularStatus = Cellular_GetServiceStatus( _cellularHandle, &serviceStatus );

            if( xCellularStatus == CELLULAR_SUCCESS )
            {
                if( ( serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME ) ||
                    ( serviceStatus.psRegistrationStatus
                      == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING ) )
                {
                    break;
                }
                else
                {
                    timeoutCount++;

                    if( timeoutCount >= timeoutCountLimit )
                    {
                        TEST_FAIL_MESSAGE( ">>>  Cellular module can't be registered3  <<<" );
                    }

                    vTaskDelay( pdMS_TO_TICKS( waitInterval ) );
                }
            }
            else
            {
                TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                                       ">>>  Cellular module can't be registered4  <<<" );
            }
        }

        /* Configure and Activate PDN, set DNS and verify IP. */
        xCellularStatus = Cellular_SetPdnConfig( _cellularHandle, pdnContextId, &pdnConfig );
        TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                               ">>>  PDN configuration failed  <<<" );
        xCellularStatus = Cellular_ActivatePdn( _cellularHandle, pdnContextId );
        TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                               ">>>  Cellular module can't be activated  <<<" );

        /* Get PDN & IP and verify. */
        xCellularStatus = Cellular_GetIPAddress( _cellularHandle, pdnContextId, localIP, sizeof( localIP ) );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        xCellularStatus = Cellular_GetPdnStatus( _cellularHandle, &PdnStatusBuffers, pdnContextId, &numStatus );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( 1, PdnStatusBuffers.state, ">>>  Cellular module failed to be activated  <<<" );
        TEST_ASSERT_GREATER_OR_EQUAL_MESSAGE( 32, sizeof( localIP ),
                                              ">>>  Cellular module failed to obtain IP address  <<<" );

        /* Set DNS. */
        xCellularStatus = Cellular_SetDns( _cellularHandle, pdnContextId, "8.8.8.8" );
        TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                               ">>>  DNS configuration failed  <<<" );

        /* DNS query IP. */
        xCellularStatus = Cellular_GetHostByName( _cellularHandle, pdnContextId, clientcredentialMQTT_BROKER_ENDPOINT, pIpAddress );
        TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                               ">>>  DNS query IP failed  <<<" );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief TCP Data Transfer
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_TCPDataTransfer )
{
    CellularError_t xCellularStatus;
    CellularSocketHandle_t socketHandle;
    CellularSocketAddress_t pRemoteSocketAddress;
    uint32_t sendTimeout = 60000;
    char dataToSend[ 40 ] = "hello from SJC31";
    uint8_t tries = 0;
    uint32_t sentDataLen = 0;
    char receiveBuff[ 100 ] = { 0 };
    uint32_t receivedDataLen = 0;
    uint8_t pdnContextId = 1;
    uint8_t socketConnected = 0;
    int lCompResults;

    if( TEST_PROTECT() )
    {
        /* Create Socket. */
        xCellularStatus = Cellular_CreateSocket( _cellularHandle, pdnContextId, /* context id */
                                                 CELLULAR_SOCKET_DOMAIN_AF_INET, CELLULAR_SOCKET_TYPE_DGRAM, CELLULAR_SOCKET_PROTOCOL_TCP, &socketHandle );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        _socketHandleSet = socketHandle;

        /* Modify Socket. */
        xCellularStatus = Cellular_SocketSetSockOpt( _cellularHandle, socketHandle, CELLULAR_SOCKET_OPTION_LEVEL_TRANSPORT,
                                                     CELLULAR_SOCKET_OPTION_SEND_TIMEOUT, ( uint8_t * ) &sendTimeout, sizeof( sendTimeout ) );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        /* Data and Socket Event call back enabled. */
        xCellularStatus = Cellular_SocketRegisterDataReadyCallback( _cellularHandle,
                                                                    socketHandle, &prvCellularSocketDataReadyCallback, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        xCellularStatus = Cellular_SocketRegisterSocketOpenCallback( _cellularHandle,
                                                                     socketHandle, &prvCellularSocketOpenCallback, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        /* Connect Socket. */
        pRemoteSocketAddress.port = tcptestECHO_PORT;
        pRemoteSocketAddress.ipAddress.ipAddressType = CELLULAR_IP_ADDRESS_V4;
        SOCKETS_inet_ntoa( testtcpECHO_SERVER_ADDRESS, pRemoteSocketAddress.ipAddress.ipAddress );
        xCellularStatus = Cellular_SocketConnect( _cellularHandle, socketHandle, CELLULAR_ACCESSMODE_BUFFER, &pRemoteSocketAddress );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            while( tries < 75 )
            {
                /* it takes upto 150 seconds for socket connect */
                vTaskDelay( pdMS_TO_TICKS( 2000 ) );

                if( _socketOpened == 1 )
                {
                    socketConnected = 1;
                    tries = 0;
                    break;
                }
                else if( _socketOpened == 2 )
                {
                    break;
                }

                tries++;
            }

            if( socketConnected == 0 )
            {
                TEST_FAIL_MESSAGE( "Socket failed to connect" );
                configASSERT( socketConnected == 1 );
            }
        }

        /* Send Data on Socket. */
        vTaskDelay( pdMS_TO_TICKS( 2000 ) );

        while( tries < 4 )
        {
            xCellularStatus = Cellular_SocketSend( _cellularHandle, socketHandle, ( const uint8_t * ) dataToSend, strlen( dataToSend ),
                                                   &sentDataLen );

            if( xCellularStatus == CELLULAR_SUCCESS )
            {
                break;
            }

            vTaskDelay( pdMS_TO_TICKS( 2000 ) );
            tries++;
        }

        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        /* Receive Data on Socket. */
        while( tries < 4 )
        {
            vTaskDelay( pdMS_TO_TICKS( 2000 ) );

            if( _dataReady )
            {
                xCellularStatus = Cellular_SocketRecv( _cellularHandle, socketHandle, ( uint8_t * ) receiveBuff, sizeof( receiveBuff ),
                                                       &receivedDataLen );
                _dataReady = 0;
                break;
            }

            tries++;
        }

        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        /* Compare Data on Socket. */
        lCompResults = strncmp( dataToSend, receiveBuff, strlen( dataToSend ) );
        TEST_ASSERT_EQUAL_INT32( 0, lCompResults );

        /* Close Socket. */
        vTaskDelay( pdMS_TO_TICKS( 2000 ) );
        xCellularStatus = Cellular_SocketClose( _cellularHandle, socketHandle );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Enable CELLULAR Idle Discontinuous Reception
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_EidrxSettings )
{
    CellularError_t xCellularStatus;
    CellularEidrxSettings_t eidrxSettings;
    CellularEidrxSettingsList_t eidrxSettingsList;
    uint8_t drxRat = 4;
    uint8_t drxValue = 9;

    if( TEST_PROTECT() )
    {
        /* Disable the EDRX mode. */
        eidrxSettings.mode = 3;
        eidrxSettings.rat = drxRat;
        eidrxSettings.requestedEdrxVaue = 0;

        xCellularStatus = Cellular_SetEidrxSettings( _cellularHandle, &eidrxSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        /* Enabling the EDRX mode and verify. */
        eidrxSettings.mode = 1;
        eidrxSettings.rat = drxRat;
        eidrxSettings.requestedEdrxVaue = drxValue;

        vTaskDelay( pdMS_TO_TICKS( 2000 ) );
        xCellularStatus = Cellular_SetEidrxSettings( _cellularHandle, &eidrxSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
        xCellularStatus = Cellular_GetEidrxSettings( _cellularHandle, &eidrxSettingsList );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        for( int i = 0; i < eidrxSettingsList.count; i++ )
        {
            if( eidrxSettingsList.eidrxList[ i ].rat == drxRat )
            {
                TEST_ASSERT_EQUAL_INT32( eidrxSettingsList.eidrxList[ i ].requestedEdrxVaue, drxValue );
            }
            else
            {
                continue;
            }
        }

        /* Disabling the EDRX mode and verify. */
        eidrxSettings.mode = 3;
        eidrxSettings.rat = drxRat;
        eidrxSettings.requestedEdrxVaue = 0;

        xCellularStatus = Cellular_SetEidrxSettings( _cellularHandle, &eidrxSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
        xCellularStatus = Cellular_GetEidrxSettings( _cellularHandle, &eidrxSettingsList );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Enable CELLULAR Power Saving Mode attributes
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_PsmSettings )
{
    CellularError_t xCellularStatus;
    CellularPsmSettings_t psmSettings;
    uint32_t psmTau = 4;
    uint32_t psmTimer = 14; /* 28 Seconds. */

    if( TEST_PROTECT() )
    {
        /* Disabling the PSM mode if ON. */
        xCellularStatus = Cellular_GetPsmSettings( _cellularHandle, &psmSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        if( psmSettings.mode == 1 )
        {
            psmSettings.mode = 0;
            psmSettings.periodicTauValue = 0;
            psmSettings.periodicRauValue = 0;
            psmSettings.gprsReadyTimer = 0;
            psmSettings.activeTimeValue = 0;

            xCellularStatus = Cellular_SetPsmSettings( _cellularHandle, &psmSettings );
            TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        }

        /* Enabling the PSM mode and verify. */
        psmSettings.mode = 1;
        psmSettings.periodicTauValue = psmTau;
        psmSettings.periodicRauValue = 0;
        psmSettings.gprsReadyTimer = 0;
        psmSettings.activeTimeValue = psmTimer;

        vTaskDelay( pdMS_TO_TICKS( 2000 ) );
        xCellularStatus = Cellular_SetPsmSettings( _cellularHandle, &psmSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        vTaskDelay( pdMS_TO_TICKS( 2000 ) );
        xCellularStatus = Cellular_GetPsmSettings( _cellularHandle, &psmSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        TEST_ASSERT_EQUAL_INT32( psmSettings.mode, 1 );

        /* Disabling the PSM mode and verify. */
        psmSettings.mode = 0;
        psmSettings.periodicTauValue = 0;
        psmSettings.periodicRauValue = 0;
        psmSettings.gprsReadyTimer = 0;
        psmSettings.activeTimeValue = 0;

        xCellularStatus = Cellular_SetPsmSettings( _cellularHandle, &psmSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        vTaskDelay( pdMS_TO_TICKS( 2000 ) );
        xCellularStatus = Cellular_GetPsmSettings( _cellularHandle, &psmSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        TEST_ASSERT_EQUAL_INT32( psmSettings.mode, 0 );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Setting and checking CELLULAR RAT priority
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_RatPriority )
{
    CellularError_t xCellularStatus;
    const CellularRat_t ratPriorities1[] = { 9, 8, 0 };
    const CellularRat_t ratPriorities2[] = { 8, 9, 0 };
    int lCompResults;

    if( TEST_PROTECT() )
    {
        /* Set the first priority and verify. */
        xCellularStatus = Cellular_SetRatPriority( _cellularHandle, ( const CellularRat_t * ) &ratPriorities1, 3 );
        vTaskDelay( pdMS_TO_TICKS( 15000 ) );
        xCellularStatus = Cellular_ATCommandRaw( _cellularHandle, "+QCFG", "AT+QCFG=\"nwscanseq\"", CELLULAR_AT_WITH_PREFIX,
                                                 &prvresponseReceivedCallback, NULL, 0 );

        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            char * AtRespRatPriority = NULL;
            char * token = strtok( _AtRespTxt, "," );

            while( token != NULL )
            {
                AtRespRatPriority = token;
                token = strtok( NULL, "," );
            }

            lCompResults = strncmp( AtRespRatPriority, "030201", 6 );
            TEST_ASSERT_EQUAL_INT32( 0, lCompResults );

            /* Restore the second priority. */
            xCellularStatus = Cellular_SetRatPriority( _cellularHandle, ( const CellularRat_t * ) &ratPriorities2, 3 );
            vTaskDelay( pdMS_TO_TICKS( 15000 ) );
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Setting CELLULAR Airplane Mode On and off
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_AirplaneMode )
{
    CellularError_t xCellularStatus;

    if( TEST_PROTECT() )
    {
        CellularSimCardStatus_t simStatus;
        uint8_t tries = 0;

        /* RF Off. */
        xCellularStatus = Cellular_RfOff( _cellularHandle );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            /* wait until SIM is ready */
            while( tries < 4 )
            {
                xCellularStatus = Cellular_GetSimCardStatus( _cellularHandle, &simStatus );

                if( xCellularStatus != CELLULAR_SUCCESS )
                {
                    TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
                    break;
                }
                else if( simStatus.simCardState == CELLULAR_SIM_CARD_INSERTED )
                {
                    if( simStatus.simCardLockState == CELLULAR_SIM_CARD_READY )
                    {
                        _simReady = 0;
                        break;
                    }
                }

                vTaskDelay( pdMS_TO_TICKS( 5000 ) );
                tries++;
            }
        }

        /* RF On. */
        xCellularStatus = Cellular_RfOn( _cellularHandle );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            /* wait until SIM is ready */
            while( tries < 4 )
            {
                xCellularStatus = Cellular_GetSimCardStatus( _cellularHandle, &simStatus );

                if( xCellularStatus != CELLULAR_SUCCESS )
                {
                    TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
                    break;
                }
                else if( simStatus.simCardState == CELLULAR_SIM_CARD_INSERTED )
                {
                    if( simStatus.simCardLockState == CELLULAR_SIM_CARD_READY )
                    {
                        _simReady = 1;
                        break;
                    }
                }

                vTaskDelay( pdMS_TO_TICKS( 5000 ) );
                tries++;
            }
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Deactivate CELLULAR
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_Deactivate )
{
    CellularError_t xCellularStatus;
    CellularPdnStatus_t pdnStatusBuffers;
    uint8_t numStatus = 1;
    uint8_t pdnContextId = 1;

    if( TEST_PROTECT() )
    {
        /* Deactivate PDN and verify. */
        xCellularStatus = Cellular_DeactivatePdn( _cellularHandle, pdnContextId );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        vTaskDelay( pdMS_TO_TICKS( 2000 ) );
        xCellularStatus = Cellular_GetPdnStatus( _cellularHandle, &pdnStatusBuffers, pdnContextId, &numStatus );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief CELLULAR unconfigure
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_UnConfigure )
{
    CellularError_t xCellularStatus;

    if( TEST_PROTECT() )
    {
        /* Remove call backs. */
        xCellularStatus = Cellular_RegisterUrcSignalStrengthChangedCallback( _cellularHandle, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        xCellularStatus = Cellular_RegisterUrcNetworkRegistrationEventCallback( _cellularHandle, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        xCellularStatus = Cellular_RegisterUrcPdnEventCallback( _cellularHandle, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        xCellularStatus = Cellular_RegisterModemEventCallback( _cellularHandle, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        /* Clean up. */
        xCellularStatus = Cellular_Cleanup( _cellularHandle );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            _cellularHandle = NULL;
            _simReady = 0;
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call Cellular_GetModemInfo( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetModemInfo_NullParameters )
{
    CellularError_t xCellularStatus;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_GetModemInfo( _cellularHandle, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call Cellular_Cellular_GetSimCardInfo( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetSimCardInfo_NullParameters )
{
    CellularError_t xCellularStatus;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_GetSimCardInfo( _cellularHandle, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call Cellular_GetSimCardStatus( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetSimCardStatus_NullParameters )
{
    CellularError_t xCellularStatus;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_GetSimCardStatus( _cellularHandle, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call Cellular_GetServiceStatus( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetServiceStatus_NullParameters )
{
    CellularError_t xCellularStatus;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_GetServiceStatus( _cellularHandle, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call Cellular_GetSignalInfo( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetSignalInfo_NullParameters )
{
    CellularError_t xCellularStatus;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_GetSignalInfo( _cellularHandle, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call Cellular_GetRegisteredNetwork( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetRegisteredNetwork_NullParameters )
{
    CellularError_t xCellularStatus;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_GetRegisteredNetwork( _cellularHandle, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call Cellular_GetPsmSettings( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetPsmSettings_NullParameters )
{
    CellularError_t xCellularStatus;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_GetPsmSettings( _cellularHandle, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call Cellular_GetEidrxSettings( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetEidrxSettings_NullParameters )
{
    CellularError_t xCellularStatus;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_GetEidrxSettings( _cellularHandle, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call Cellular_GetPdnStatus( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetPdnStatus_NullParameters )
{
    CellularError_t xCellularStatus;
    uint8_t pdnContextId = 1;
    uint8_t numStatus = 1;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_GetPdnStatus( _cellularHandle, NULL, pdnContextId, &numStatus );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call Cellular_GetIPAddress( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetIPAddress_NullParameters )
{
    CellularError_t xCellularStatus;
    uint8_t pdnContextId = 1;
    char localIP[ 32 ] = { '\0' };

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_GetIPAddress( _cellularHandle, pdnContextId, NULL, sizeof( localIP ) );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call Cellular_SetRatPriority( _cellularHandle ) with an invalid mode and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_SetRatPriority_InvalidMode )
{
    CellularError_t xCellularStatus;
    const CellularRat_t ratPriorities[] = { 9, 8, 1 }; /*Invalid value 1 */

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_SetRatPriority( _cellularHandle, ( const CellularRat_t * ) &ratPriorities, 5 /*invalid value*/ );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call Cellular_SetPsmSettings( _cellularHandle ) with an invalid mode and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_SetPsmSettings_InvalidMode )
{
    CellularError_t xCellularStatus;
    CellularPsmSettings_t psmSettings;

    psmSettings.activeTimeValue = 28;
    psmSettings.gprsReadyTimer = 0;
    psmSettings.mode = 2; /*Invalid value */
    psmSettings.periodicRauValue = 0;
    psmSettings.periodicTauValue = 4;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_SetPsmSettings( _cellularHandle, &psmSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call Cellular_SetEidrxSettings( _cellularHandle ) with an invalid mode and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_SetEidrxSettings_InvalidMode )
{
    CellularError_t xCellularStatus;
    CellularEidrxSettings_t eidrxSettings;

    eidrxSettings.mode = 1;
    eidrxSettings.rat = 6; /*invalid value */
    eidrxSettings.requestedEdrxVaue = 1;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_SetEidrxSettings( _cellularHandle, &eidrxSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call Cellular_SetPdnConfig( _cellularHandle ) with an invalid mode and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_SetPdnConfig_InvalidMode )
{
    CellularError_t xCellularStatus;
    uint8_t pdnContextId = 1;
    CellularPdnConfig_t pdnConfig =
    { CELLULAR_PDN_CONTEXT_IPV4, CELLULAR_PDN_AUTH_NONE, testINVALID_CELLULAR_APN, "", "" };

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_SetPdnConfig( _cellularHandle, pdnContextId, &pdnConfig );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call Cellular_SetDns( _cellularHandle ) with an invalid mode and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_SetDns_InvalidMode )
{
    CellularError_t xCellularStatus;
    uint8_t pdnContextId = 1;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_SetDns( _cellularHandle, pdnContextId, "123" );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief CELLULAR data transfer loop
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_Data_Loop )
{
    uint8_t index;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        for( index = 0; index < testCELLULARCONNECTION_LOOP_TIMES; ++index )
        {
            CellularError_t xCellularStatus;
            CellularSocketHandle_t socketHandle;
            CellularSocketAddress_t pRemoteSocketAddress;
            uint32_t sendTimeout = 60000;
            char dataToSend[ 40 ] = "hello from SJC31";
            uint8_t tries = 0;
            uint32_t sentDataLen = 0;
            char receiveBuff[ 100 ] = { 0 };
            uint32_t receivedDataLen = 0;
            uint8_t pdnContextId = 1;
            uint8_t socketConnected = 0;
            int lCompResults;
            char cBuffer[ 256 ];
            uint16_t sBufferLength = 256;

            /* Create Socket. */
            xCellularStatus = Cellular_CreateSocket( _cellularHandle, pdnContextId, /* context id */
                                                     CELLULAR_SOCKET_DOMAIN_AF_INET, CELLULAR_SOCKET_TYPE_DGRAM, CELLULAR_SOCKET_PROTOCOL_TCP, &socketHandle );
            TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

            /* Modify Socket. */
            xCellularStatus = Cellular_SocketSetSockOpt( _cellularHandle, socketHandle, CELLULAR_SOCKET_OPTION_LEVEL_TRANSPORT,
                                                         CELLULAR_SOCKET_OPTION_SEND_TIMEOUT, ( uint8_t * ) &sendTimeout, sizeof( sendTimeout ) );
            TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

            /* Data and Socket Event call back enabled. */
            xCellularStatus = Cellular_SocketRegisterDataReadyCallback( _cellularHandle,
                                                                        socketHandle, &prvCellularSocketDataReadyCallback, NULL );
            TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
            xCellularStatus = Cellular_SocketRegisterSocketOpenCallback( _cellularHandle,
                                                                         socketHandle, &prvCellularSocketOpenCallback, NULL );
            TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
            xCellularStatus = Cellular_SocketRegisterClosedCallback( _cellularHandle,
                                                                     socketHandle, &prvdclosedCallback, NULL );
            TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

            /* Connect Socket. */
            pRemoteSocketAddress.port = tcptestECHO_PORT;
            pRemoteSocketAddress.ipAddress.ipAddressType = CELLULAR_IP_ADDRESS_V4;
            SOCKETS_inet_ntoa( testtcpECHO_SERVER_ADDRESS, pRemoteSocketAddress.ipAddress.ipAddress );
            xCellularStatus = Cellular_SocketConnect( _cellularHandle, socketHandle, CELLULAR_ACCESSMODE_BUFFER, &pRemoteSocketAddress );
            TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

            if( xCellularStatus == CELLULAR_SUCCESS )
            {
                tries = 0;

                while( tries < 75 )
                {
                    /* it takes upto 150 seconds for socket connect */
                    vTaskDelay( pdMS_TO_TICKS( 2000 ) );

                    if( _socketOpened == 1 )
                    {
                        socketConnected = 1;
                        tries = 0;
                        break;
                    }
                    else if( _socketOpened == 2 )
                    {
                        break;
                    }

                    tries++;
                }

                if( socketConnected == 0 )
                {
                    TEST_FAIL_MESSAGE( "Socket failed to connect" );
                    configASSERT( socketConnected == 1 );
                }
            }

            /* Send Data on Socket. */
            vTaskDelay( pdMS_TO_TICKS( 2000 ) );
            tries = 0;

            while( tries < 4 )
            {
                xCellularStatus = Cellular_SocketSend( _cellularHandle, socketHandle, ( const uint8_t * ) dataToSend, strlen( dataToSend ),
                                                       &sentDataLen );

                if( xCellularStatus == CELLULAR_SUCCESS )
                {
                    break;
                }

                vTaskDelay( pdMS_TO_TICKS( 2000 ) );
                tries++;
            }

            snprintf( cBuffer, sBufferLength, "Failed Cellular_SocketSend( _cellularHandle ) in iteration %d", index );
            TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus, cBuffer );

            /* Receive Data on Socket. */
            tries = 0;

            while( tries < 4 )
            {
                vTaskDelay( pdMS_TO_TICKS( 2000 ) );

                if( _dataReady )
                {
                    xCellularStatus = Cellular_SocketRecv( _cellularHandle, socketHandle, ( uint8_t * ) receiveBuff, sizeof( receiveBuff ),
                                                           &receivedDataLen );
                    _dataReady = 0;
                    break;
                }

                tries++;
            }

            snprintf( cBuffer, sBufferLength, "Failed Cellular_SocketRecv( _cellularHandle ) in iteration %d", index );
            TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus, cBuffer );

            /* Compare Data on Socket. */
            lCompResults = strncmp( dataToSend, receiveBuff, strlen( dataToSend ) );
            TEST_ASSERT_EQUAL_INT32( 0, lCompResults );

            /* Close Socket. */
            vTaskDelay( pdMS_TO_TICKS( 2000 ) );
            xCellularStatus = Cellular_SocketClose( _cellularHandle, socketHandle );
            TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

            if( xCellularStatus == CELLULAR_SUCCESS )
            {
                _socketOpened = 0;
                socketConnected = 0;
                vTaskDelay( pdMS_TO_TICKS( 5000 ) );
            }
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief CELLULAR airplane mode loop
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_AirplaneMode_Loop )
{
    char cBuffer[ 256 ];
    uint16_t sBufferLength = 256;
    uint8_t index;
    CellularError_t xCellularStatus;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        for( index = 0; index < testCELLULARCONNECTION_LOOP_TIMES; ++index )
        {
            CellularSimCardStatus_t simStatus;
            uint8_t tries = 0;

            /* RF Off. */
            xCellularStatus = Cellular_RfOff( _cellularHandle );
            snprintf( cBuffer, sBufferLength, "Failed Cellular_RfOff( _cellularHandle ) in iteration %d", index );
            TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus, cBuffer );

            if( xCellularStatus == CELLULAR_SUCCESS )
            {
                /* wait until SIM is ready */
                while( tries < 4 )
                {
                    xCellularStatus = Cellular_GetSimCardStatus( _cellularHandle, &simStatus );

                    if( xCellularStatus != CELLULAR_SUCCESS )
                    {
                        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
                        break;
                    }
                    else if( simStatus.simCardState == CELLULAR_SIM_CARD_INSERTED )
                    {
                        if( simStatus.simCardLockState == CELLULAR_SIM_CARD_READY )
                        {
                            _simReady = 0;
                            break;
                        }
                    }

                    vTaskDelay( pdMS_TO_TICKS( 5000 ) );
                    tries++;
                }
            }

            /* RF On. */
            xCellularStatus = Cellular_RfOn( _cellularHandle );
            snprintf( cBuffer, sBufferLength, "Failed Cellular_RfOn( _cellularHandle ) in iteration %d", index );
            TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus, cBuffer );

            if( xCellularStatus == CELLULAR_SUCCESS )
            {
                /* wait until SIM is ready */
                while( tries < 4 )
                {
                    xCellularStatus = Cellular_GetSimCardStatus( _cellularHandle, &simStatus );

                    if( xCellularStatus != CELLULAR_SUCCESS )
                    {
                        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
                        break;
                    }
                    else if( simStatus.simCardState == CELLULAR_SIM_CARD_INSERTED )
                    {
                        if( simStatus.simCardLockState == CELLULAR_SIM_CARD_READY )
                        {
                            _simReady = 1;
                            break;
                        }
                    }

                    vTaskDelay( pdMS_TO_TICKS( 5000 ) );
                    tries++;
                }
            }
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief CELLULAR power cycle loop
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_Power_Loop )
{
    uint8_t index = 0;
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularCommInterface_t * pCommIntf = &CellularCommInterface;

    if( TEST_PROTECT() )
    {
        /* clean previous setting. */
        ( void ) Cellular_Cleanup( _cellularHandle );
        _cellularHandle = NULL;

        for( index = 0; index < testCELLULARCONNECTION_LOOP_TIMES; ++index )
        {
            _pCommIntf = pCommIntf;
            vTaskDelay( pdMS_TO_TICKS( 1000 ) );
            xCellularStatus = Cellular_Init( &_cellularHandle, pCommIntf );
            TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                                   ">>>  Cellular module can't be initialized  <<<" );

            /* Clean up. */
            xCellularStatus = Cellular_Cleanup( _cellularHandle );
            _cellularHandle = NULL;
            TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                                   ">>>  Cellular module can't be cleanup  <<<" );
        }
    }
    else
    {
        TEST_FAIL();
    }
}
