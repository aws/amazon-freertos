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

/* Cellular include. */
#include "aws_cellular_config.h"
#include "cellular_config_defaults.h"
#include "cellular_api.h"
#include "cellular_types.h"
#include "cellular_comm_interface.h"
#include "iot_secure_sockets.h"

/* Testing variable includes. */
#include "aws_test_runner.h"
#include "aws_test_tcp.h"
#include "aws_test_utils.h"
#include "aws_test_cellular.h"
#include "iot_config.h"
#include "aws_clientcredential.h"

/*-----------------------------------------------------------*/

/* Testing configurations definitions. */

/* Retry until SIM is ready. */
#ifndef CELLULAR_MAX_SIM_RETRY
    #define CELLULAR_MAX_SIM_RETRY    ( 5U )
#endif

#ifndef testCELLULAR_SOCKET_CONNECTION_TIMEOUT_MS
    #define testCELLULAR_SOCKET_CONNECTION_TIMEOUT_MS    ( 150000U )
#endif

#ifndef testCELLULAR_SOCKET_SEND_TIMEOUT_MS
    #define testCELLULAR_SOCKET_SEND_TIMEOUT_MS    ( 60000U )
#endif

#ifndef testCELLULAR_MAX_NETWORK_REGISTER_RETRY
    #define testCELLULAR_MAX_NETWORK_REGISTER_RETRY    ( 40U )
#endif

#ifndef testCELLULAR_NETWORK_REGISTER_RETRY_INTERVAL_MS
    #define testCELLULAR_NETWORK_REGISTER_RETRY_INTERVAL_MS    ( 500U )
#endif

/* Retry until SIM is ready. */
#ifndef testCELLULAR_MAX_SIM_RETRY
    #define testCELLULAR_MAX_SIM_RETRY    ( 5U )
#endif

#ifndef testCELLULAR_SIM_RETRY_INTERVAL_MS
    #define testCELLULAR_SIM_RETRY_INTERVAL_MS    ( 500U )
#endif

#ifndef testCELLULAR_MAX_GET_PSM_RETRY
    #define testCELLULAR_MAX_GET_PSM_RETRY    ( 5U )
#endif

#ifndef testCELLULAR_GET_PSM_RETRY_INTERVAL_MS
    #define testCELLULAR_GET_PSM_RETRY_INTERVAL_MS    ( 500U )
#endif

#ifndef testCELLULAR_SOCKET_WAIT_INTERVAL_MS
    #define testCELLULAR_SOCKET_WAIT_INTERVAL_MS    ( 2000UL )
#endif

#ifndef testCELLULAR_GET_RAT_RETRY
    #define testCELLULAR_GET_RAT_RETRY    ( 5UL )
#endif

#ifndef testCELLULAR_GET_RAT_RETRY_INTERVAL_MS
    #define testCELLULAR_GET_RAT_RETRY_INTERVAL_MS    ( 200U )
#endif

#ifndef testCELLULAR_WAIT_PSM_ENTER_EVENT_RETRY
    #define testCELLULAR_WAIT_PSM_ENTER_EVENT_RETRY    ( 2U )
#endif

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
#define TEST_ECHO_SERVER_ADDRESS                                                 \
    SOCKETS_inet_addr_quick( tcptestECHO_SERVER_ADDR0, tcptestECHO_SERVER_ADDR1, \
                             tcptestECHO_SERVER_ADDR2, tcptestECHO_SERVER_ADDR3 )

#define TEST_INVALID_CELLULAR_APN            "VZWINTERNETVZWINTERNETVZWINTERNETVZWINTERNETVZWINTERNETVZWINTERN"

#define SOCKET_DATA_RECEIVED_CALLBACK_BIT    ( 0x00000001U )
#define SOCKET_OPEN_CALLBACK_BIT             ( 0x00000002U )
#define SOCKET_OPEN_FAILED_CALLBACK_BIT      ( 0x00000004U )

#define ECHO_SERVER_DATA_SEND_INTERVAL_MS    ( 30000UL )

#define MODEM_EVENT_BOOTUP_OR_REBOOT_BIT     ( 0x00000001U )
#define MODEM_EVENT_POWERED_DOWN_BIT         ( 0x00000002U )
#define MODEM_EVENT_PSM_ENTER_BIT            ( 0x00000004U )

#define SOCKET_OPEN_STATUS_UNKNOWN           ( 0U )
#define SOCKET_OPEN_STATUS_OPENED            ( 1U )
#define SOCKET_OPEN_STATUS_FAILED            ( 2U )

#define SOCKET_OPERATION_POLLING_TIMES       ( 4U )

#define MESSAGE_BUFFER_LENGTH                ( 256U )

/* RAT priority count for testing. This value should larger or equal to
 * CELLULAR_MAX_RAT_PRIORITY_COUNT. */
#define TEST_MAX_RAT_PRIORITY_COUNT          ( 3U )
#if CELLULAR_MAX_RAT_PRIORITY_COUNT > TEST_MAX_RAT_PRIORITY_COUNT
    #error "TEST_MAX_RAT_PRIORITY_COUNT should not larger or equal to CELLULAR_MAX_RAT_PRIORITY_COUNT"
#endif

/*-----------------------------------------------------------*/

/**
 * @brief the default Cellular comm interface in system.
 */
extern CellularCommInterface_t CellularCommInterface;
extern CellularHandle_t CellularHandle;

/*-----------------------------------------------------------*/

/* Test state variables. */
static uint8_t _dataReady = 0;
static CellularHandle_t _cellularHandle = NULL;
static bool _genericUrcCalled = false;
static EventGroupHandle_t _socketEventGroup = NULL;
static EventGroupHandle_t _modemEventGroup = NULL;

/* The callback context to check. */
static void * _socketDataReadyContext = NULL;
static void * _socketOpenContext = NULL;
static void * _socketClosedContext = NULL;

/* Socket data send pattern. */
static const char _socketDataSend[] = "hello from SJC31";

/*-----------------------------------------------------------*/

/* Netwrok registeration callback function. */
static void prvNetworkRegistrationCallback( CellularUrcEvent_t urcEvent,
                                            const CellularServiceStatus_t * pServiceStatus,
                                            void * pCallbackContext )
{
    if( pServiceStatus != NULL )
    {
        if( ( urcEvent == CELLULAR_URC_EVENT_NETWORK_CS_REGISTRATION ) ||
            ( urcEvent == CELLULAR_URC_EVENT_NETWORK_PS_REGISTRATION ) )
        {
            configPRINTF( ( "Network CS registration status received: %d. \r\n", pServiceStatus->csRegistrationStatus ) );
            configPRINTF( ( "Network PS registration status received: %d. \r\n", pServiceStatus->psRegistrationStatus ) );
        }
    }
}

/*-----------------------------------------------------------*/

/* Signal strength changed callback function. */
static void prvSignalStrengthChangedCallback( CellularUrcEvent_t urcEvent,
                                              const CellularSignalInfo_t * pSignalInfo,
                                              void * pCallbackContext )
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

/*-----------------------------------------------------------*/

/* Generic callback function to test Cellular_RegisterUrcGenericCallback API. */
static void prvGenericCallback( const char * pRawData,
                                void * pCallbackContext )
{
    configPRINTF( ( "prvGenericCallback : %s \r\n", pRawData ) );
    _genericUrcCalled = true;
}

/*-----------------------------------------------------------*/

/* PDN event callback function. */
static void prvPdnEventCallback( CellularUrcEvent_t urcEvent,
                                 uint8_t contextId,
                                 void * pCallbackContext )
{
    if( ( urcEvent == CELLULAR_URC_EVENT_PDN_ACTIVATED ) || ( urcEvent == CELLULAR_URC_EVENT_PDN_DEACTIVATED ) )
    {
        configPRINTF( ( "PDP Status changed. %d\r\n", urcEvent ) );
    }
}

/*-----------------------------------------------------------*/

/* Callback functions for testing. */
static void prvCellularSocketDataReadyCallback( CellularSocketHandle_t socketHandle,
                                                void * pCallbackContext )
{
    configPRINTF( ( "Data Ready on Socket\r\n" ) );
    _dataReady = 1;

    if( _socketEventGroup != NULL )
    {
        ( void ) xEventGroupSetBits( _socketEventGroup, SOCKET_DATA_RECEIVED_CALLBACK_BIT );
    }

    _socketDataReadyContext = pCallbackContext;
}

/*-----------------------------------------------------------*/

/* Socket close event callback function. */
static void prvSocketClosedCallback( CellularSocketHandle_t socketHandle,
                                     void * pCallbackContext )
{
    if( socketHandle != NULL )
    {
        configPRINTF( ( "Socket is closed. \r\n" ) );
    }

    _socketClosedContext = pCallbackContext;
}

/*-----------------------------------------------------------*/

/* Socket open event callback function. */
static void prvCellularSocketOpenCallback( CellularUrcEvent_t urcEvent,
                                           CellularSocketHandle_t socketHandle,
                                           void * pCallbackContext )
{
    if( urcEvent == CELLULAR_URC_SOCKET_OPENED )
    {
        configPRINTF( ( "Socket open callback, Success\r\n" ) );

        if( _socketEventGroup != NULL )
        {
            ( void ) xEventGroupSetBits( _socketEventGroup, SOCKET_OPEN_CALLBACK_BIT );
        }
    }
    else
    {
        configPRINTF( ( "Socket open callback, Failure\r\n" ) );

        if( _socketEventGroup != NULL )
        {
            ( void ) xEventGroupSetBits( _socketEventGroup, SOCKET_OPEN_FAILED_CALLBACK_BIT );
        }
    }

    _socketOpenContext = pCallbackContext;
}

/*-----------------------------------------------------------*/

/* Modem event callback function. */
static void prvCellularModemEventCallback( CellularModemEvent_t modemEvent,
                                           void * pCallbackContext )
{
    if( _modemEventGroup != NULL )
    {
        switch( modemEvent )
        {
            case CELLULAR_MODEM_EVENT_BOOTUP_OR_REBOOT:
                ( void ) xEventGroupSetBits( _modemEventGroup, MODEM_EVENT_BOOTUP_OR_REBOOT_BIT );
                break;

            case CELLULAR_MODEM_EVENT_POWERED_DOWN:
                ( void ) xEventGroupSetBits( _modemEventGroup, MODEM_EVENT_POWERED_DOWN_BIT );
                break;

            case CELLULAR_MODEM_EVENT_PSM_ENTER:
                ( void ) xEventGroupSetBits( _modemEventGroup, MODEM_EVENT_PSM_ENTER_BIT );
                break;

            default:
                break;
        }
    }
}

/*-----------------------------------------------------------*/

/* Helper function to check sim card ready. */
static bool prvWaitSimCardReady( void )
{
    bool simReady = false;
    uint32_t tries = 0;
    CellularSimCardStatus_t simStatus = { 0 };
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;

    for( tries = 0; tries < testCELLULAR_MAX_SIM_RETRY; tries++ )
    {
        xCellularStatus = Cellular_GetSimCardStatus( _cellularHandle, &simStatus );

        if( ( CELLULAR_SUCCESS == xCellularStatus ) &&
            ( simStatus.simCardState == CELLULAR_SIM_CARD_INSERTED ) &&
            ( simStatus.simCardLockState == CELLULAR_SIM_CARD_READY ) )
        {
            simReady = true;
            break;
        }

        vTaskDelay( pdMS_TO_TICKS( testCELLULAR_SIM_RETRY_INTERVAL_MS ) );
    }

    return simReady;
}

/*-----------------------------------------------------------*/

/**
 * @brief Connect to the CELLULAR and verify success.
 */
static BaseType_t prvConnectCellular( void )
{
    BaseType_t xResult = pdPASS;
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularServiceStatus_t serviceStatus = { 0 };
    CellularCommInterface_t * pCommIntf = &CellularCommInterface;
    CellularPdnConfig_t pdnConfig = { CELLULAR_PDN_CONTEXT_IPV4, CELLULAR_PDN_AUTH_NONE, testCELLULAR_APN, "", "" };
    CellularPdnStatus_t PdnStatusBuffers = { 0 };
    char localIP[ CELLULAR_IP_ADDRESS_MAX_SIZE ] = { '\0' };
    uint32_t timeoutCount = 0;
    uint8_t NumStatus = 1;
    bool simReady = false;

    /* Clean up the cellular handle before init. */
    if( _cellularHandle != NULL )
    {
        ( void ) Cellular_Cleanup( _cellularHandle );
        _cellularHandle = NULL;
    }

    /* Initialize Cellular Comm Interface. */
    xCellularStatus = Cellular_Init( &_cellularHandle, pCommIntf );

    if( xCellularStatus != CELLULAR_SUCCESS )
    {
        configPRINTF( ( ">>>  Cellular module can't initialized  <<<\r\n" ) );
        xResult = pdFAIL;
    }
    else
    {
        xResult = pdPASS;
    }

    if( ( xCellularStatus == CELLULAR_SUCCESS ) && ( xResult == pdPASS ) )
    {
        /* Wait until SIM is ready. */
        simReady = prvWaitSimCardReady();

        if( simReady == false )
        {
            xResult = pdFAIL;
        }
    }

    if( ( xCellularStatus == CELLULAR_SUCCESS ) && ( xResult == pdPASS ) )
    {
        /* Setup PDN for EPS Network Registration. */
        xCellularStatus = Cellular_SetPdnConfig( _cellularHandle, testCELLULAR_PDN_CONTEXT_ID, &pdnConfig );

        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            xResult = pdPASS;
        }
        else
        {
            xResult = pdFAIL;
        }
    }

    /* Rescan network. */
    if( xCellularStatus == CELLULAR_SUCCESS )
    {
        xCellularStatus = Cellular_RfOff( _cellularHandle );
    }

    if( xCellularStatus == CELLULAR_SUCCESS )
    {
        xCellularStatus = Cellular_RfOn( _cellularHandle );
    }

    if( ( xCellularStatus == CELLULAR_SUCCESS ) && ( xResult == pdPASS ) )
    {
        /* Check network register status. */
        xResult = pdFAIL;

        for( timeoutCount = 0; timeoutCount < testCELLULAR_MAX_NETWORK_REGISTER_RETRY; timeoutCount++ )
        {
            xCellularStatus = Cellular_GetServiceStatus( _cellularHandle, &serviceStatus );

            if( ( xCellularStatus == CELLULAR_SUCCESS ) &&
                ( ( serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME ) ||
                  ( serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING ) ) )
            {
                xResult = pdPASS;
                break;
            }

            vTaskDelay( pdMS_TO_TICKS( testCELLULAR_NETWORK_REGISTER_RETRY_INTERVAL_MS ) );
        }

        if( xResult == pdFAIL )
        {
            configPRINTF( ( ">>>  Cellular module can't be registered  <<<\r\n" ) );
        }
    }

    if( ( xCellularStatus == CELLULAR_SUCCESS ) && ( xResult == pdPASS ) )
    {
        xCellularStatus = Cellular_RegisterUrcNetworkRegistrationEventCallback( _cellularHandle, &prvNetworkRegistrationCallback, NULL );

        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            xCellularStatus = Cellular_RegisterUrcPdnEventCallback( _cellularHandle, &prvPdnEventCallback, NULL );
        }

        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            xCellularStatus = Cellular_SetPdnConfig( _cellularHandle, testCELLULAR_PDN_CONTEXT_ID, &pdnConfig );
        }

        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            xCellularStatus = Cellular_ActivatePdn( _cellularHandle, testCELLULAR_PDN_CONTEXT_ID );
        }

        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            xCellularStatus = Cellular_GetIPAddress( _cellularHandle, testCELLULAR_PDN_CONTEXT_ID, localIP, sizeof( localIP ) );
        }

        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            xCellularStatus = Cellular_GetPdnStatus( _cellularHandle, &PdnStatusBuffers, testCELLULAR_PDN_CONTEXT_ID, &NumStatus );
        }

        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            xCellularStatus = Cellular_SetDns( _cellularHandle, testCELLULAR_PDN_CONTEXT_ID, testCELLULAR_DNS_SERVER_ADDRESS );

            /* Modem use dynamic DNS. */
            if( xCellularStatus == CELLULAR_UNSUPPORTED )
            {
                xCellularStatus = CELLULAR_SUCCESS;
            }
        }
    }

    if( ( xCellularStatus == CELLULAR_SUCCESS ) && ( xResult == pdPASS ) && ( PdnStatusBuffers.state == 1 ) )
    {
        xResult = pdPASS;
    }
    else
    {
        xResult = pdFAIL;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

/* Helper function to check if cellular network connected. */
static BaseType_t prvIsConnectedCellular( void )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularPdnStatus_t PdnStatusBuffers = { 0 };
    uint8_t NumStatus = 1;
    BaseType_t xResult = pdPASS;

    if( _cellularHandle != NULL )
    {
        xCellularStatus = Cellular_GetPdnStatus( _cellularHandle, &PdnStatusBuffers, 1, &NumStatus );

        /* State 0 = Deactivated, 1 = Activated. */
        if( ( xCellularStatus == CELLULAR_SUCCESS ) && ( PdnStatusBuffers.state == 1 ) )
        {
            xResult = pdPASS;
        }
        else
        {
            xResult = pdFAIL;
        }
    }
    else
    {
        xResult = pdFAIL;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

/* Finish test help function. */
static BaseType_t prvFinishCellularTesting( void )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    BaseType_t xResult = pdPASS;

    xCellularStatus = Cellular_Cleanup( _cellularHandle );

    if( xCellularStatus != CELLULAR_SUCCESS )
    {
        configPRINTF( ( ">>>  Cellular module cleanup failed  <<<\r\n" ) );
        xResult = pdFAIL;
    }
    else
    {
        _cellularHandle = NULL;
        xResult = pdPASS;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

/* Setup socket connection. */
static CellularSocketHandle_t prvSocketConnectionSetup( uint16_t serverPort,
                                                        char * pServerAddress )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularSocketAddress_t remoteSocketAddress = { 0 };
    CellularSocketHandle_t socketHandle = NULL;
    uint32_t sendTimeout = testCELLULAR_SOCKET_SEND_TIMEOUT_MS;
    EventBits_t waitEventBits = 0;

    /* Setup the event group. */
    _socketEventGroup = xEventGroupCreate();
    TEST_ASSERT_MESSAGE( _socketEventGroup != NULL, "event group create failed" );
    xEventGroupClearBits( _socketEventGroup,
                          SOCKET_OPEN_CALLBACK_BIT | SOCKET_OPEN_FAILED_CALLBACK_BIT | SOCKET_DATA_RECEIVED_CALLBACK_BIT );

    /* Setup the tcp connection. */
    /* Create Socket. */
    xCellularStatus = Cellular_CreateSocket( _cellularHandle,
                                             testCELLULAR_PDN_CONTEXT_ID,
                                             CELLULAR_SOCKET_DOMAIN_AF_INET,
                                             CELLULAR_SOCKET_TYPE_DGRAM,
                                             CELLULAR_SOCKET_PROTOCOL_TCP,
                                             &socketHandle );
    TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

    /* Modify Socket. */
    xCellularStatus = Cellular_SocketSetSockOpt( _cellularHandle,
                                                 socketHandle,
                                                 CELLULAR_SOCKET_OPTION_LEVEL_TRANSPORT,
                                                 CELLULAR_SOCKET_OPTION_SEND_TIMEOUT,
                                                 ( uint8_t * ) &sendTimeout,
                                                 sizeof( sendTimeout ) );
    TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

    /* Data and Socket Event call back enabled. */
    xCellularStatus = Cellular_SocketRegisterDataReadyCallback( _cellularHandle,
                                                                socketHandle,
                                                                &prvCellularSocketDataReadyCallback,
                                                                socketHandle );
    TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
    xCellularStatus = Cellular_SocketRegisterSocketOpenCallback( _cellularHandle,
                                                                 socketHandle,
                                                                 &prvCellularSocketOpenCallback,
                                                                 socketHandle );
    TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
    xCellularStatus = Cellular_SocketRegisterClosedCallback( _cellularHandle,
                                                             socketHandle,
                                                             &prvSocketClosedCallback,
                                                             socketHandle );
    TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

    /* Connect Socket. */
    remoteSocketAddress.port = serverPort;
    remoteSocketAddress.ipAddress.ipAddressType = CELLULAR_IP_ADDRESS_V4;
    strncpy( remoteSocketAddress.ipAddress.ipAddress, pServerAddress, CELLULAR_IP_ADDRESS_MAX_SIZE );
    xCellularStatus = Cellular_SocketConnect( _cellularHandle,
                                              socketHandle,
                                              CELLULAR_ACCESSMODE_BUFFER,
                                              &remoteSocketAddress );
    TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
    waitEventBits = xEventGroupWaitBits( _socketEventGroup,
                                         SOCKET_OPEN_CALLBACK_BIT | SOCKET_OPEN_FAILED_CALLBACK_BIT,
                                         pdTRUE,
                                         pdFALSE,
                                         pdMS_TO_TICKS( testCELLULAR_SOCKET_CONNECTION_TIMEOUT_MS ) );
    TEST_ASSERT_MESSAGE( ( waitEventBits & SOCKET_OPEN_CALLBACK_BIT ) != 0, "Socket connection timeout or failed" );

    return socketHandle;
}


/*-----------------------------------------------------------*/

/* Close socket connection. */
static void prvSocketConnectionClose( CellularSocketHandle_t socketHandle )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;

    /* Close the socket. */
    xCellularStatus = Cellular_SocketRegisterDataReadyCallback( _cellularHandle,
                                                                socketHandle,
                                                                NULL,
                                                                NULL );
    TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

    xCellularStatus = Cellular_SocketRegisterSocketOpenCallback( _cellularHandle,
                                                                 socketHandle,
                                                                 NULL,
                                                                 NULL );
    TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

    xCellularStatus = Cellular_SocketClose( _cellularHandle, socketHandle );
    TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

    if( _socketEventGroup != NULL )
    {
        vEventGroupDelete( _socketEventGroup );
        _socketEventGroup = NULL;
    }
}

/*-----------------------------------------------------------*/

/* EDRX receive count test function. */
static uint32_t prvTestSocketReceiveCount( const uint32_t testTimeMs,
                                           const uint32_t dataReceiveIntervalMs )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularSocketHandle_t socketHandle = NULL;
    uint32_t dataReceivedCount = 0;
    uint32_t sentDataLen = 0;
    uint8_t receiveBuff[ 100 ] = { 0 };
    uint32_t receivedDataLen = 0;
    uint32_t totalReceivedDataLen = 0;
    TickType_t recvStartTime = 0;

    /* Setup the socket connection. */
    socketHandle = prvSocketConnectionSetup( testCELLULAR_EDRX_ECHO_SERVER_PORT,
                                             testCELLULAR_EDRX_ECHO_SERVER_ADDRESS );

    /* Send a byte to the server to start echo in time interval. */
    xCellularStatus = Cellular_SocketSend( _cellularHandle,
                                           socketHandle,
                                           ( const uint8_t * ) _socketDataSend,
                                           strlen( _socketDataSend ),
                                           &sentDataLen );
    TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

    recvStartTime = xTaskGetTickCount();
    /* Echo server will send data after received data. Wait 5 seconds for the first data. */
    configPRINTF( ( "start receive time %d, test time ms %d\r\n", recvStartTime, testTimeMs ) );
    vTaskDelay( pdMS_TO_TICKS( 5000UL ) );

    while( 1 )
    {
        totalReceivedDataLen = 0;

        while( 1 )
        {
            xCellularStatus = Cellular_SocketRecv( _cellularHandle,
                                                   socketHandle,
                                                   receiveBuff,
                                                   sizeof( receiveBuff ),
                                                   &receivedDataLen );
            TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

            if( receivedDataLen == 0 )
            {
                break;
            }

            totalReceivedDataLen = totalReceivedDataLen + receivedDataLen;
        }

        if( totalReceivedDataLen != 0 )
        {
            configPRINTF( ( "Bytes received %d\r\n", totalReceivedDataLen ) );
            dataReceivedCount = dataReceivedCount + 1;
        }

        if( ( xTaskGetTickCount() - recvStartTime ) > pdMS_TO_TICKS( testTimeMs ) )
        {
            break;
        }

        vTaskDelay( pdMS_TO_TICKS( dataReceiveIntervalMs ) );
    }

    prvSocketConnectionClose( socketHandle );

    return dataReceivedCount;
}

/*-----------------------------------------------------------*/

/* Unity TEST initializations. */
TEST_GROUP( Full_CELLULAR_API );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_CELLULAR_API )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularModemInfo_t modemInfo = { 0 };
    CellularSimCardInfo_t simCardInfo = { 0 };
    CellularSimCardStatus_t simStatus = { 0 };
    CellularSignalInfo_t signalInfo = { 0 };
    char localIP[ CELLULAR_IP_ADDRESS_MAX_SIZE ] = { '\0' };
    CellularPlmnInfo_t networkInfo = { 0 };
    CellularServiceStatus_t serviceStatus = { 0 };
    CellularTime_t networkTime = { 0 };
    CellularPsmSettings_t psmSettings = { 0 };
    CellularEidrxSettingsList_t eidrxSettingsList = { 0 };
    CellularRat_t pRatPriorities[ CELLULAR_MAX_RAT_PRIORITY_COUNT ] = { CELLULAR_RAT_INVALID };
    uint8_t receivedRatPrioritiesLength = 0;
    uint32_t ratIndex = 0;

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

    /* Cellular_GetSignalInfo should be called after Cellular_GetServiceStatus to set libAtData.rat to get correct bar level. */
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

    xCellularStatus = Cellular_GetRatPriority( _cellularHandle,
                                               pRatPriorities, CELLULAR_MAX_RAT_PRIORITY_COUNT, &receivedRatPrioritiesLength );

    if( xCellularStatus == CELLULAR_SUCCESS )
    {
        for( ratIndex = 0; ratIndex < receivedRatPrioritiesLength; ratIndex++ )
        {
            configPRINTF( ( " RAT Priority: %u %u\r\n", ratIndex, pRatPriorities[ ratIndex ] ) );
        }
    }

    xCellularStatus = Cellular_GetIPAddress( _cellularHandle, testCELLULAR_PDN_CONTEXT_ID, localIP, sizeof( localIP ) );

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
        configPRINTF( ( " PSM mode: %d \r\n PSM TAU Value: %d \r\n PSM RAU Value: %d \r\n PSM GPRS Timer: %d \r\n PSM Active Value: %d \r\n",
                        psmSettings.mode,
                        psmSettings.periodicTauValue,
                        psmSettings.periodicRauValue,
                        psmSettings.gprsReadyTimer,
                        psmSettings.activeTimeValue ) );
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
            configPRINTF( ( " eDRX index: %d eDRX mode: %d eDRX rat:%d eDRX UE Value:%d eDRX NW value:%d \r\n",
                            i,
                            eidrxSettingsList.eidrxList[ i ].mode,
                            eidrxSettingsList.eidrxList[ i ].rat,
                            eidrxSettingsList.eidrxList[ i ].requestedEdrxVaue,
                            eidrxSettingsList.eidrxList[ i ].nwProvidedEdrxVaue ) );
        }
    }
    else
    {
        configPRINTF( ( " eDRX index: eDRX mode: eDRX rat: eDRX UE Value: eDRX NW value: \r\n" ) );
    }

    configPRINTF( ( "\r\n==================================================================================\r\n" ) );
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_CELLULAR_API )
{
    configPRINTF( ( "\r\n==================================================================================\r\n" ) );
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_CELLULAR_API )
{
    CellularPsmSettings_t psmSettings = { 0 };

    /* Clean up the cellular context. */
    if( CellularHandle != NULL )
    {
        configPRINTF( ( "Clean up the cellular\r\n" ) );
        psmSettings.mode = 0;
        psmSettings.periodicTauValue = 0;
        psmSettings.periodicRauValue = 0;
        psmSettings.gprsReadyTimer = 0;
        psmSettings.activeTimeValue = 0;
        Cellular_SetPsmSettings( CellularHandle, &psmSettings );
        Cellular_DeactivatePdn( CellularHandle, testCELLULAR_PDN_CONTEXT_ID );
        Cellular_Cleanup( CellularHandle );
        CellularHandle = NULL;
    }

    /* List of all tests under this group */
    /* In sequence tests. */
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_Configure );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_Activate );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_GetNetworkTime );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_GetHostByName );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_TCPDataTransfer );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_EidrxSettings );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_PsmSettings );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_RatPriority );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_AtCommandRawAndGenericUrc );
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

    /* PSM and eDRX tests. */
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_EidrxEchoTimes );
    RUN_TEST_CASE( Full_CELLULAR_API, AFQP_Cellular_PsmStatus );

    prvFinishCellularTesting();
}

/*-----------------------------------------------------------*/

/**
 * @brief Configure CELLULAR.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_Configure )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularSimCardStatus_t simStatus = { 0 };
    CellularCommInterface_t * pCommIntf = &CellularCommInterface;
    uint8_t tries = 0;
    uint8_t simReady = 0;

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_Init( &_cellularHandle, pCommIntf );
        TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                               ">>>  Cellular module can't be initialized  <<<" );

        /* Wait until SIM is ready. */
        for( tries = 0; tries < testCELLULAR_MAX_SIM_RETRY; tries++ )
        {
            xCellularStatus = Cellular_GetSimCardStatus( _cellularHandle, &simStatus );
            TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                                   ">>>  Cellular SIM failure  <<<" );

            if( ( simStatus.simCardState == CELLULAR_SIM_CARD_INSERTED ) &&
                ( simStatus.simCardLockState == CELLULAR_SIM_CARD_READY ) )
            {
                simReady = 1;
                break;
            }

            vTaskDelay( pdMS_TO_TICKS( testCELLULAR_SIM_RETRY_INTERVAL_MS ) );
        }

        TEST_ASSERT( simReady != 0 );

        /* Enable Callbacks. */
        xCellularStatus = Cellular_RegisterUrcSignalStrengthChangedCallback( _cellularHandle, &prvSignalStrengthChangedCallback, NULL );
        TEST_CELLULAR_ASSERT_OPTIONAL_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        xCellularStatus = Cellular_RegisterUrcNetworkRegistrationEventCallback( _cellularHandle, &prvNetworkRegistrationCallback, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        xCellularStatus = Cellular_RegisterUrcPdnEventCallback( _cellularHandle, &prvPdnEventCallback, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief CELLULAR Activate.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_Activate )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularServiceStatus_t serviceStatus = { 0 };
    CellularPdnConfig_t pdnConfig =
    { CELLULAR_PDN_CONTEXT_IPV4, CELLULAR_PDN_AUTH_NONE, testCELLULAR_APN, "", "" };
    CellularPdnStatus_t PdnStatusBuffers = { 0 };
    char localIP[ CELLULAR_IP_ADDRESS_MAX_SIZE ] = { '\0' };
    uint32_t timeoutCount = 0;
    uint8_t numStatus = 1;
    CellularPsmSettings_t psmSettings = { 0 };
    CellularEidrxSettings_t eidrxSettings = { 0 };

    if( TEST_PROTECT() )
    {
        /* Setup PDN for EPS Network Registration. */
        xCellularStatus = Cellular_SetPdnConfig( _cellularHandle, testCELLULAR_PDN_CONTEXT_ID, &pdnConfig );
        TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                               ">>>  PDN configuration failed  <<<" );

        /* Verify registration. */
        for( timeoutCount = 0; timeoutCount < testCELLULAR_MAX_NETWORK_REGISTER_RETRY; timeoutCount++ )
        {
            xCellularStatus = Cellular_GetServiceStatus( _cellularHandle, &serviceStatus );
            TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                                   ">>>  Cellular module can't be registered  <<<" );

            if( ( serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME ) ||
                ( serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING ) )
            {
                break;
            }

            vTaskDelay( pdMS_TO_TICKS( testCELLULAR_NETWORK_REGISTER_RETRY_INTERVAL_MS ) );
        }

        if( timeoutCount >= testCELLULAR_MAX_NETWORK_REGISTER_RETRY )
        {
            TEST_FAIL_MESSAGE( ">>>  Cellular module can't be registered  <<<" );
        }

        /* Configure and Activate PDN, set DNS and verify IP. */
        xCellularStatus = Cellular_ActivatePdn( _cellularHandle, testCELLULAR_PDN_CONTEXT_ID );
        TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                               ">>>  Cellular module can't be activated  <<<" );

        /* Get PDN & IP and verify. */
        xCellularStatus = Cellular_GetIPAddress( _cellularHandle, testCELLULAR_PDN_CONTEXT_ID, localIP, sizeof( localIP ) );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        xCellularStatus = Cellular_GetPdnStatus( _cellularHandle, &PdnStatusBuffers, testCELLULAR_PDN_CONTEXT_ID, &numStatus );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( 1, PdnStatusBuffers.state, ">>>  Cellular module failed to be activated  <<<" );

        /* Set DNS. */
        xCellularStatus = Cellular_SetDns( _cellularHandle, testCELLULAR_PDN_CONTEXT_ID, testCELLULAR_DNS_SERVER_ADDRESS );
        TEST_CELLULAR_ASSERT_OPTIONAL_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                               ">>>  DNS configuration failed  <<<" );

        /* Disable PSM and eDRX for the following tests. */
        psmSettings.mode = 0;
        psmSettings.periodicTauValue = 0;
        psmSettings.periodicRauValue = 0;
        psmSettings.gprsReadyTimer = 0;
        psmSettings.activeTimeValue = 0;
        xCellularStatus = Cellular_SetPsmSettings( _cellularHandle, &psmSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                               ">>>  Disable PSM failed  <<<" );

        eidrxSettings.mode = 0;
        eidrxSettings.rat = testCELLULAR_EDRX_RAT;
        eidrxSettings.requestedEdrxVaue = 0;
        xCellularStatus = Cellular_SetEidrxSettings( _cellularHandle, &eidrxSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                               ">>>  Disable EDRX failed  <<<" );
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Get network time.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetNetworkTime )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularTime_t networkTime = { 0 };

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_GetNetworkTime( _cellularHandle, &networkTime );
        TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                               ">>>  Get network time failed  <<<" );

        /* Verify the value range. */
        TEST_ASSERT_MESSAGE( ( ( networkTime.month >= 1 ) && ( networkTime.month <= 12 ) ),
                             ">>>  Get network time month value error  <<<" );
        TEST_ASSERT_MESSAGE( ( ( networkTime.day >= 1 ) && ( networkTime.day <= 31 ) ),
                             ">>>  Get network time day value error  <<<" );
        TEST_ASSERT_MESSAGE( ( ( networkTime.hour >= 0 ) && ( networkTime.hour <= 24 ) ),
                             ">>>  Get network time hour value error  <<<" );
        TEST_ASSERT_MESSAGE( ( ( networkTime.minute >= 0 ) && ( networkTime.minute <= 59 ) ),
                             ">>>  Get network time minute value error  <<<" );
        TEST_ASSERT_MESSAGE( ( ( networkTime.second >= 0 ) && ( networkTime.second <= 59 ) ),
                             ">>>  Get network time second value error  <<<" );
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Host name resolve test.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetHostByName )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    char pIpAddress[ CELLULAR_IP_ADDRESS_MAX_SIZE ] = { '\0' };

    if( TEST_PROTECT() )
    {
        /* DNS query IP. */
        xCellularStatus = Cellular_GetHostByName(
            _cellularHandle,
            testCELLULAR_PDN_CONTEXT_ID,
            testCELLULAR_HOST_NAME,
            pIpAddress );
        TEST_CELLULAR_ASSERT_OPTIONAL_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus,
                                               ">>>  DNS query IP failed  <<<" );

        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            TEST_ASSERT_MESSAGE( strncmp( pIpAddress, testCELLULAR_HOST_NAME_ADDRESS, CELLULAR_IP_ADDRESS_MAX_SIZE ) == 0,
                                 ">>>  DNS query IP incorrect  <<<" );
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief TCP Data Transfer.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_TCPDataTransfer )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularSocketHandle_t socketHandle = NULL;
    uint8_t tries = 0;
    uint32_t sentDataLen = 0;
    char receiveBuff[ 100 ] = { 0 };
    uint32_t receivedDataLen = 0;
    char pServerAddress[ CELLULAR_IP_ADDRESS_MAX_SIZE ] = { '\0' };

    if( TEST_PROTECT() )
    {
        /* Setup the test variable. */
        _dataReady = 0;
        _socketOpenContext = NULL;
        _socketDataReadyContext = NULL;

        /* Setup server connection. */
        SOCKETS_inet_ntoa( TEST_ECHO_SERVER_ADDRESS, pServerAddress );
        socketHandle = prvSocketConnectionSetup( tcptestECHO_PORT, pServerAddress );
        TEST_ASSERT_MESSAGE( _socketOpenContext == socketHandle, "Socket open context check failed" );

        /* Send Data on Socket. */
        for( tries = 0; tries < SOCKET_OPERATION_POLLING_TIMES; tries++ )
        {
            xCellularStatus = Cellular_SocketSend( _cellularHandle,
                                                   socketHandle,
                                                   ( const uint8_t * ) _socketDataSend,
                                                   strlen( _socketDataSend ),
                                                   &sentDataLen );

            if( xCellularStatus == CELLULAR_SUCCESS )
            {
                break;
            }
        }

        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        /* Receive Data on Socket in polling method. */
        for( tries = 0; tries < SOCKET_OPERATION_POLLING_TIMES; tries++ )
        {
            vTaskDelay( pdMS_TO_TICKS( testCELLULAR_SOCKET_WAIT_INTERVAL_MS ) );

            if( _dataReady == 1 )
            {
                xCellularStatus = Cellular_SocketRecv( _cellularHandle,
                                                       socketHandle,
                                                       ( uint8_t * ) receiveBuff,
                                                       sizeof( receiveBuff ),
                                                       &receivedDataLen );
                TEST_ASSERT_MESSAGE( _socketDataReadyContext == socketHandle, "Socket data ready context check failed" );
                break;
            }
        }

        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        /* Compare Data on Socket. */
        TEST_ASSERT_MESSAGE( strncmp( _socketDataSend, receiveBuff, strlen( _socketDataSend ) ) == 0,
                             "AFQP_Cellular_TCPDataTransfer received data compare failed" );

        /* Close Socket. */
        prvSocketConnectionClose( socketHandle );
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Enable CELLULAR Idle Discontinuous Reception.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_EidrxSettings )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularEidrxSettings_t eidrxSettings = { 0 };
    CellularEidrxSettingsList_t eidrxSettingsList = { 0 };
    uint8_t drxValue = 5; /* 5 = ( 0 1 0 1 )  81.92 seconds. */
    int i = 0;

    if( TEST_PROTECT() )
    {
        /* Disable the EDRX mode. */
        eidrxSettings.mode = 0;
        eidrxSettings.rat = testCELLULAR_EDRX_RAT;
        eidrxSettings.requestedEdrxVaue = 0;

        xCellularStatus = Cellular_SetEidrxSettings( _cellularHandle, &eidrxSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        /* Enabling the EDRX mode and verify. */
        eidrxSettings.mode = 1; /* Enable the use of e-I-DRX. */
        eidrxSettings.rat = testCELLULAR_EDRX_RAT;
        eidrxSettings.requestedEdrxVaue = drxValue;

        xCellularStatus = Cellular_SetEidrxSettings( _cellularHandle, &eidrxSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        xCellularStatus = Cellular_GetEidrxSettings( _cellularHandle, &eidrxSettingsList );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        TEST_ASSERT_MESSAGE( eidrxSettingsList.count > 0, "eidrxSettingsList count is 0" );

        for( i = 0; i < eidrxSettingsList.count; i++ )
        {
            if( eidrxSettingsList.eidrxList[ i ].rat == testCELLULAR_EDRX_RAT )
            {
                TEST_ASSERT_EQUAL_INT32( eidrxSettingsList.eidrxList[ i ].requestedEdrxVaue, drxValue );
            }
        }

        /* Disabling the EDRX mode and verify. */
        eidrxSettings.mode = 3; /* Disable the use of e-I-DRX and discard all parameters for e-I-DRX or,
                                 * if available, reset to the manufacturer specific default values. */
        eidrxSettings.rat = testCELLULAR_EDRX_RAT;
        eidrxSettings.requestedEdrxVaue = 0;

        xCellularStatus = Cellular_SetEidrxSettings( _cellularHandle, &eidrxSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        xCellularStatus = Cellular_GetEidrxSettings( _cellularHandle, &eidrxSettingsList );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Enable CELLULAR Power Saving Mode attributes.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_PsmSettings )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularPsmSettings_t psmSettings = { 0 };
    uint32_t psmTau = 4;    /* 4 * 10 minutes = 40 minutes. */
    uint32_t psmTimer = 14; /* 14 * 2 seconds = 28 Seconds. */
    uint32_t tries = 0;

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

        xCellularStatus = Cellular_SetPsmSettings( _cellularHandle, &psmSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        for( tries = 0; tries < testCELLULAR_MAX_GET_PSM_RETRY; tries++ )
        {
            xCellularStatus = Cellular_GetPsmSettings( _cellularHandle, &psmSettings );

            if( ( xCellularStatus == CELLULAR_SUCCESS ) && ( psmSettings.mode == 1 ) )
            {
                break;
            }

            vTaskDelay( pdMS_TO_TICKS( testCELLULAR_GET_PSM_RETRY_INTERVAL_MS ) );
        }

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

        for( tries = 0; tries < testCELLULAR_MAX_GET_PSM_RETRY; tries++ )
        {
            xCellularStatus = Cellular_GetPsmSettings( _cellularHandle, &psmSettings );

            if( ( xCellularStatus == CELLULAR_SUCCESS ) && ( psmSettings.mode == 0 ) )
            {
                break;
            }

            vTaskDelay( pdMS_TO_TICKS( testCELLULAR_GET_PSM_RETRY_INTERVAL_MS ) );
        }

        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        TEST_ASSERT_EQUAL_INT32( psmSettings.mode, 0 );
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Setting and checking CELLULAR RAT priority.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_RatPriority )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    const CellularRat_t pRatPriorities1[ TEST_MAX_RAT_PRIORITY_COUNT ] =
    { CELLULAR_RAT_NBIOT, CELLULAR_RAT_CATM1, CELLULAR_RAT_GSM };
    const CellularRat_t pRatPriorities2[ TEST_MAX_RAT_PRIORITY_COUNT ] =
    { CELLULAR_RAT_CATM1, CELLULAR_RAT_NBIOT, CELLULAR_RAT_GSM };
    CellularRat_t pRatPriorities[ TEST_MAX_RAT_PRIORITY_COUNT ] = { CELLULAR_RAT_INVALID };
    uint8_t receivedRatPrioritiesLength = 0;
    int i = 0;
    uint32_t tries = 0;
    bool ratFlag = true;

    if( TEST_PROTECT() )
    {
        /* Set the first priority and verify. */
        xCellularStatus = Cellular_SetRatPriority( _cellularHandle,
                                                   ( const CellularRat_t * ) pRatPriorities1,
                                                   CELLULAR_MAX_RAT_PRIORITY_COUNT );
        TEST_CELLULAR_ASSERT_OPTIONAL_API_MSG( ( CELLULAR_SUCCESS == xCellularStatus ) || ( CELLULAR_NOT_ALLOWED == xCellularStatus ),
                                               xCellularStatus,
                                               "Set RAT priority failed" );

        /* Set RAT priority may not be supported in the cellular module. */
        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            for( tries = 0; tries < testCELLULAR_GET_RAT_RETRY; tries++ )
            {
                xCellularStatus = Cellular_GetRatPriority( _cellularHandle,
                                                           pRatPriorities,
                                                           CELLULAR_MAX_RAT_PRIORITY_COUNT,
                                                           &receivedRatPrioritiesLength );
                TEST_ASSERT_MESSAGE( CELLULAR_SUCCESS == xCellularStatus, "Get RAT priority failed" );

                /* Check the return priority length if RAT priority is supported. */
                if( xCellularStatus == CELLULAR_SUCCESS )
                {
                    TEST_ASSERT_MESSAGE( receivedRatPrioritiesLength > 0, "Get RAT priority failed" );
                    ratFlag = true;

                    for( i = 0; i < receivedRatPrioritiesLength; i++ )
                    {
                        if( pRatPriorities1[ i ] != pRatPriorities[ i ] )
                        {
                            configPRINTF( ( "%d : Set RAT [%d] != Get RAT [ %d ]\r\n",
                                            i, pRatPriorities1[ i ], pRatPriorities[ i ] ) );
                            ratFlag = false;
                            break;
                        }
                    }

                    if( ratFlag == true )
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }

                vTaskDelay( pdMS_TO_TICKS( testCELLULAR_GET_RAT_RETRY_INTERVAL_MS ) );
            }

            TEST_ASSERT_MESSAGE( ratFlag == true, "RATs priority compare failed" );

            /* Restore the second priority. */
            xCellularStatus = Cellular_SetRatPriority( _cellularHandle,
                                                       ( const CellularRat_t * ) pRatPriorities2,
                                                       CELLULAR_MAX_RAT_PRIORITY_COUNT );
            TEST_ASSERT_MESSAGE( CELLULAR_SUCCESS == xCellularStatus, "Set RAT priority failed" );
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Send AT command with receive the generic URC.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_AtCommandRawAndGenericUrc )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;

    if( TEST_PROTECT() )
    {
        _genericUrcCalled = false;
        xCellularStatus = Cellular_RegisterUrcGenericCallback( _cellularHandle,
                                                               prvGenericCallback, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( xCellularStatus == CELLULAR_SUCCESS, xCellularStatus,
                                               "Register URC generic callback failed" );

        /* Send the 3GPP get network time AT command.
         * The returned network time string is handled in generic URC handler. */
        xCellularStatus = Cellular_ATCommandRaw( _cellularHandle,
                                                 NULL,
                                                 "AT+CCLK?",
                                                 CELLULAR_AT_NO_RESULT,
                                                 NULL,
                                                 NULL,
                                                 0U );
        TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( xCellularStatus == CELLULAR_SUCCESS, xCellularStatus,
                                               "Send AT command raw failed" );

        /* The maximum response time is 300ms. */
        vTaskDelay( pdMS_TO_TICKS( 300U ) );
        TEST_ASSERT_MESSAGE( _genericUrcCalled == true, "Generic URC is not called" );

        xCellularStatus = Cellular_RegisterUrcGenericCallback( _cellularHandle,
                                                               NULL,
                                                               NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( xCellularStatus == CELLULAR_SUCCESS, xCellularStatus,
                                               "Register URC generic callback failed" );
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Setting CELLULAR Airplane Mode On and off.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_AirplaneMode )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularServiceStatus_t serviceStatus = { 0 };
    bool simReady = false;
    uint32_t tries = 0;

    if( TEST_PROTECT() )
    {
        /* RF Off. */
        xCellularStatus = Cellular_RfOff( _cellularHandle );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        /* Wait until SIM is ready. */
        simReady = prvWaitSimCardReady();
        TEST_ASSERT( simReady == true );

        /* Check network registration status. Airplane mode the register status should be
         * CELLULAR_NETWORK_REGISTRATION_STATUS_NOT_REGISTERED_NOT_SEARCHING */
        for( tries = 0; tries < testCELLULAR_MAX_NETWORK_REGISTER_RETRY; tries++ )
        {
            xCellularStatus = Cellular_GetServiceStatus( _cellularHandle, &serviceStatus );
            TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

            if( ( serviceStatus.psRegistrationStatus != CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME ) &&
                ( serviceStatus.psRegistrationStatus != CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING ) )
            {
                break;
            }

            vTaskDelay( pdMS_TO_TICKS( testCELLULAR_NETWORK_REGISTER_RETRY_INTERVAL_MS ) );
        }

        configPRINTF( ( "serviceStatus.psRegistrationStatus %d\r\n", serviceStatus.psRegistrationStatus ) );

        /* Add also psRegistrationStatus=4 if +CGREG: 2,0 and +CEREG: 2,4. */
        TEST_ASSERT_MESSAGE( ( serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_NOT_REGISTERED_NOT_SEARCHING ) ||
                             ( serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_UNKNOWN ),
                             "Airplane mode network registration check failed" );

        /* RF On. */
        xCellularStatus = Cellular_RfOn( _cellularHandle );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        /* Wait until SIM is ready. */
        simReady = prvWaitSimCardReady();
        TEST_ASSERT( simReady == true );

        /* Check network registration status. Airplane mode the register status should be
         * CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME or
         * CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING */
        for( tries = 0; tries < testCELLULAR_MAX_NETWORK_REGISTER_RETRY; tries++ )
        {
            xCellularStatus = Cellular_GetServiceStatus( _cellularHandle, &serviceStatus );
            TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

            if( ( serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME ) ||
                ( serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING ) )
            {
                break;
            }

            vTaskDelay( pdMS_TO_TICKS( testCELLULAR_NETWORK_REGISTER_RETRY_INTERVAL_MS ) );
        }

        configPRINTF( ( "serviceStatus.psRegistrationStatus %d\r\n", serviceStatus.psRegistrationStatus ) );
        TEST_ASSERT_MESSAGE(
            ( serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME ) ||
            ( serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING ),
            "Airplane mode network registration check failed\r\n" );
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Deactivate CELLULAR.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_Deactivate )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularPdnStatus_t pdnStatusBuffers = { 0 };
    uint8_t numStatus = 1;

    if( TEST_PROTECT() )
    {
        /* Deactivate PDN and verify. */
        xCellularStatus = Cellular_DeactivatePdn( _cellularHandle, testCELLULAR_PDN_CONTEXT_ID );

        /* Check also if in LTE network, modem allows default bearer context to be deactivated. */
        TEST_CELLULAR_ASSERT_REQUIRED_API( ( CELLULAR_SUCCESS == xCellularStatus ) ||
                                           ( CELLULAR_NOT_ALLOWED == xCellularStatus ), xCellularStatus );

        if( xCellularStatus != CELLULAR_NOT_ALLOWED )
        {
            xCellularStatus = Cellular_GetPdnStatus( _cellularHandle, &pdnStatusBuffers, testCELLULAR_PDN_CONTEXT_ID, &numStatus );
            TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

            TEST_ASSERT_MESSAGE( ( numStatus == 0 ) ||
                                 ( ( numStatus == 1 ) && ( pdnStatusBuffers.state == 0 ) ), "Deactive PDN should return 0" );
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief CELLULAR unconfigure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_UnConfigure )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;

    if( TEST_PROTECT() )
    {
        /* Remove call backs. */
        xCellularStatus = Cellular_RegisterUrcSignalStrengthChangedCallback( _cellularHandle, NULL, NULL );
        TEST_CELLULAR_ASSERT_OPTIONAL_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        xCellularStatus = Cellular_RegisterUrcNetworkRegistrationEventCallback( _cellularHandle, NULL, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        xCellularStatus = Cellular_RegisterUrcPdnEventCallback( _cellularHandle, NULL, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        xCellularStatus = Cellular_RegisterModemEventCallback( _cellularHandle, NULL, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        /* Clean up. */
        xCellularStatus = Cellular_Cleanup( _cellularHandle );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            _cellularHandle = NULL;
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Call Cellular_GetModemInfo( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetModemInfo_NullParameters )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;

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

/*-----------------------------------------------------------*/

/**
 * @brief Call Cellular_Cellular_GetSimCardInfo( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetSimCardInfo_NullParameters )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;

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

/*-----------------------------------------------------------*/

/**
 * @brief Call Cellular_GetSimCardStatus( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetSimCardStatus_NullParameters )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;

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

/*-----------------------------------------------------------*/

/**
 * @brief Call Cellular_GetServiceStatus( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetServiceStatus_NullParameters )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;

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

/*-----------------------------------------------------------*/

/**
 * @brief Call Cellular_GetSignalInfo( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetSignalInfo_NullParameters )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;

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

/*-----------------------------------------------------------*/

/**
 * @brief Call Cellular_GetRegisteredNetwork( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetRegisteredNetwork_NullParameters )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;

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

/*-----------------------------------------------------------*/

/**
 * @brief Call Cellular_GetPsmSettings( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetPsmSettings_NullParameters )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;

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

/*-----------------------------------------------------------*/

/**
 * @brief Call Cellular_GetEidrxSettings( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetEidrxSettings_NullParameters )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;

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

/*-----------------------------------------------------------*/

/**
 * @brief Call Cellular_GetPdnStatus( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetPdnStatus_NullParameters )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    uint8_t numStatus = 1;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_GetPdnStatus( _cellularHandle, NULL, testCELLULAR_PDN_CONTEXT_ID, &numStatus );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Call Cellular_GetIPAddress( _cellularHandle ) with Null parameters and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_GetIPAddress_NullParameters )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_GetIPAddress( _cellularHandle,
                                                 testCELLULAR_PDN_CONTEXT_ID,
                                                 NULL,
                                                 CELLULAR_IP_ADDRESS_MAX_SIZE );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Call Cellular_SetRatPriority( _cellularHandle ) with an invalid mode and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_SetRatPriority_InvalidMode )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    const CellularRat_t ratPriorities[ TEST_MAX_RAT_PRIORITY_COUNT ] = { 9, 8, 1 }; /* Invalid value 1. */

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_SetRatPriority( _cellularHandle,
                                                   ( const CellularRat_t * ) &ratPriorities,
                                                   5 /* Invalid value. */ );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Call Cellular_SetPsmSettings( _cellularHandle ) with an invalid mode and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_SetPsmSettings_InvalidMode )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularPsmSettings_t psmSettings = { 0 };

    psmSettings.activeTimeValue = 28;
    psmSettings.gprsReadyTimer = 0;
    psmSettings.mode = 2; /* Invalid value. */
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

/*-----------------------------------------------------------*/

/**
 * @brief Call Cellular_SetEidrxSettings( _cellularHandle ) with an invalid mode and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_SetEidrxSettings_InvalidMode )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularEidrxSettings_t eidrxSettings = { 0 };

    eidrxSettings.mode = 1;
    eidrxSettings.rat = 6; /* invalid value. */
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

/*-----------------------------------------------------------*/

/**
 * @brief Call Cellular_SetPdnConfig( _cellularHandle ) with an invalid mode and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_SetPdnConfig_InvalidMode )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    /* Set the invalid PDN context type. */
    CellularPdnConfig_t pdnConfig =
    { CELLULAR_PDN_CONTEXT_TYPE_MAX, CELLULAR_PDN_AUTH_NONE, TEST_INVALID_CELLULAR_APN, "", "" };

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_SetPdnConfig( _cellularHandle, testCELLULAR_PDN_CONTEXT_ID, &pdnConfig );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Call Cellular_SetDns( _cellularHandle ) with an invalid mode and verify failure.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_SetDns_InvalidMode )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        xCellularStatus = Cellular_SetDns( _cellularHandle, testCELLULAR_PDN_CONTEXT_ID, "123" );
        TEST_CELLULAR_ASSERT_OPTIONAL_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief CELLULAR data transfer loop.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_Data_Loop )
{
    uint8_t index = 0;
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularSocketHandle_t socketHandle = NULL;
    uint8_t tries = 0;
    uint32_t sentDataLen = 0;
    char receiveBuff[ 100 ] = { 0 };
    uint32_t receivedDataLen = 0;
    char cBuffer[ MESSAGE_BUFFER_LENGTH ] = { '\0' };
    char pServerAddress[ CELLULAR_IP_ADDRESS_MAX_SIZE ] = { '\0' };

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        for( index = 0; index < testCELLULARCONNECTION_LOOP_TIMES; ++index )
        {
            SOCKETS_inet_ntoa( TEST_ECHO_SERVER_ADDRESS, pServerAddress );
            socketHandle = prvSocketConnectionSetup( tcptestECHO_PORT, pServerAddress );

            /* Send Data on Socket. */
            xCellularStatus = Cellular_SocketSend( _cellularHandle, socketHandle, ( const uint8_t * ) _socketDataSend, strlen( _socketDataSend ),
                                                   &sentDataLen );
            snprintf( cBuffer, sizeof( cBuffer ), "Failed Cellular_SocketSend( _cellularHandle ) in iteration %d", index );
            TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus, cBuffer );

            /* Receive Data on Socket. */
            for( tries = 0; tries < SOCKET_OPERATION_POLLING_TIMES; tries++ )
            {
                vTaskDelay( pdMS_TO_TICKS( testCELLULAR_SOCKET_WAIT_INTERVAL_MS ) );

                if( _dataReady )
                {
                    xCellularStatus = Cellular_SocketRecv( _cellularHandle, socketHandle, ( uint8_t * ) receiveBuff, sizeof( receiveBuff ),
                                                           &receivedDataLen );
                    _dataReady = 0;
                    break;
                }
            }

            snprintf( cBuffer, sizeof( cBuffer ), "Failed Cellular_SocketRecv( _cellularHandle ) in iteration %d", index );
            TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus, cBuffer );

            /* Compare Data on Socket. */
            TEST_ASSERT_MESSAGE( strncmp( _socketDataSend, receiveBuff, strlen( _socketDataSend ) ) == 0,
                                 "AFQP_Cellular_Data_Loop received data compare failed" );

            /* Close Socket. */
            prvSocketConnectionClose( socketHandle );
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief CELLULAR airplane mode loop.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_AirplaneMode_Loop )
{
    char cBuffer[ MESSAGE_BUFFER_LENGTH ] = { '\0' };
    uint8_t index = 0;
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    bool simReady = false;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        for( index = 0; index < testCELLULARCONNECTION_LOOP_TIMES; ++index )
        {
            /* RF Off. */
            xCellularStatus = Cellular_RfOff( _cellularHandle );
            snprintf( cBuffer, sizeof( cBuffer ), "Failed Cellular_RfOff( _cellularHandle ) in iteration %d", index );
            TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus, cBuffer );

            /* Wait until SIM is ready. */
            simReady = prvWaitSimCardReady();
            TEST_ASSERT( simReady == true );

            /* RF On. */
            xCellularStatus = Cellular_RfOn( _cellularHandle );
            snprintf( cBuffer, sizeof( cBuffer ), "Failed Cellular_RfOn( _cellularHandle ) in iteration %d", index );
            TEST_CELLULAR_ASSERT_REQUIRED_API_MSG( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus, cBuffer );

            /* Wait until SIM is ready. */
            simReady = prvWaitSimCardReady();
            TEST_ASSERT( simReady == true );
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief CELLULAR power cycle loop.
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_Power_Loop )
{
    uint8_t index = 0;
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularCommInterface_t * pCommIntf = &CellularCommInterface;

    if( TEST_PROTECT() )
    {
        /* Clean previous setting. */
        ( void ) Cellular_Cleanup( _cellularHandle );
        _cellularHandle = NULL;

        for( index = 0; index < testCELLULARCONNECTION_LOOP_TIMES; ++index )
        {
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

/*-----------------------------------------------------------*/

/**
 * @brief Test eDRX settings on echo server received times.
 *
 * ------------------------------|--------------------------------|-------------------------------
 *       t1                      |         t2                     |    t3
 *     EDRX = 0                  |       EDRX = 1                 |    EDRX = 0
 * ( RX is on )                  | ( RX is off periodically )     |  ( RX is on )
 * ( Data receiption is normal ) | ( Data receiption is delayed ) |  ( Data receiption is normal )
 * ------------------------------|--------------------------------|-------------------------------
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_EidrxEchoTimes )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularEidrxSettings_t eidrxSettings = { 0 };
    CellularEidrxSettingsList_t eidrxSettingsList = { 0 };
    uint8_t drxValue = 5;                 /* 5 = ( 0 1 0 1 )  81.92 seconds. */
    const uint32_t testTimoutMs = 80000U; /* Test waiting socket receive time. */
    uint32_t normalReceiveTimes = 0;
    uint32_t edrxReceiveTimes = 0;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        /* Disable the EDRX mode. */
        eidrxSettings.mode = 0;
        eidrxSettings.rat = testCELLULAR_EDRX_RAT;
        eidrxSettings.requestedEdrxVaue = 0;

        xCellularStatus = Cellular_SetEidrxSettings( _cellularHandle, &eidrxSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        /* Send the data to the server and wait for response. */
        normalReceiveTimes = prvTestSocketReceiveCount( testTimoutMs, testCELLULAR_EDRX_ECHO_SERVER_DATA_SEND_INTERVAL_MS );
        configPRINTF( ( "Normal echo test receive times %d\r\n", normalReceiveTimes ) );
        vTaskDelay( pdMS_TO_TICKS( testCELLULAR_EDRX_ECHO_SERVER_DATA_SEND_INTERVAL_MS ) );

        /* Enabling the EDRX mode and verify. */
        eidrxSettings.mode = 1;
        eidrxSettings.rat = testCELLULAR_EDRX_RAT;
        eidrxSettings.requestedEdrxVaue = drxValue;

        xCellularStatus = Cellular_SetEidrxSettings( _cellularHandle, &eidrxSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        xCellularStatus = Cellular_GetEidrxSettings( _cellularHandle, &eidrxSettingsList );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        /* Send the data to the server and wait for response.
         * Data receive times is less in eDRX mode. */
        edrxReceiveTimes = prvTestSocketReceiveCount( testTimoutMs, testCELLULAR_EDRX_ECHO_SERVER_DATA_SEND_INTERVAL_MS );
        configPRINTF( ( "EDRX echo test receive times %d\r\n", edrxReceiveTimes ) );
        TEST_ASSERT_MESSAGE( ( edrxReceiveTimes < normalReceiveTimes ),
                             "EDRX receive more times than normal" );

        /* Disabling the EDRX mode and verify. */
        eidrxSettings.mode = 3;
        eidrxSettings.rat = testCELLULAR_EDRX_RAT;
        eidrxSettings.requestedEdrxVaue = 0;

        xCellularStatus = Cellular_SetEidrxSettings( _cellularHandle, &eidrxSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
        xCellularStatus = Cellular_GetEidrxSettings( _cellularHandle, &eidrxSettingsList );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

/*
 * @brief Check cellular power saving mode status.
 *
 * --------------------|---------------------
 *      t1             |         t2
 *    PSM = 0          |       PSM = 1
 * (at cmd works)      |   (at cmd fails)
 * --------------------|---------------------
 */
TEST( Full_CELLULAR_API, AFQP_Cellular_PsmStatus )
{
    CellularError_t xCellularStatus = CELLULAR_SUCCESS;
    CellularPsmSettings_t psmSettings = { 0 };
    uint32_t psmTau = 4;    /* 4 * 10 minutes = 40 minutes. */
    uint32_t psmTimer = 14; /* 14 * 2 seconds = 28 Seconds. */
    uint32_t tries = 0;
    EventBits_t waitEventBits = 0;

    if( prvIsConnectedCellular() == pdFAIL )
    {
        TEST_ASSERT( prvConnectCellular() == pdPASS );
    }

    if( TEST_PROTECT() )
    {
        /* Setup the modem event. */
        _modemEventGroup = xEventGroupCreate();
        TEST_ASSERT_MESSAGE( _modemEventGroup != NULL, "Create event group fail" );
        xEventGroupClearBits( _modemEventGroup,
                              MODEM_EVENT_BOOTUP_OR_REBOOT_BIT | MODEM_EVENT_POWERED_DOWN_BIT | MODEM_EVENT_PSM_ENTER_BIT );

        xCellularStatus = Cellular_RegisterModemEventCallback( _cellularHandle, prvCellularModemEventCallback, NULL );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

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

        xCellularStatus = Cellular_SetPsmSettings( _cellularHandle, &psmSettings );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS == xCellularStatus, xCellularStatus );

        for( tries = 0; tries < testCELLULAR_MAX_GET_PSM_RETRY; tries++ )
        {
            xCellularStatus = Cellular_GetPsmSettings( _cellularHandle, &psmSettings );

            if( xCellularStatus == CELLULAR_SUCCESS )
            {
                configPRINTF( ( "PSM mode polling %u\r\n", psmSettings.mode ) );

                if( psmSettings.mode == 1 )
                {
                    break;
                }
            }

            vTaskDelay( pdMS_TO_TICKS( testCELLULAR_GET_PSM_RETRY_INTERVAL_MS ) );
        }

        if( xCellularStatus == CELLULAR_SUCCESS )
        {
            TEST_ASSERT_EQUAL_INT32( psmSettings.mode, 1 );
            configPRINTF( ( "PSM active time %u\r\n", psmSettings.activeTimeValue ) );
        }

        /* Wait until active timer expired. */
        for( tries = 0; tries < testCELLULAR_WAIT_PSM_ENTER_EVENT_RETRY; tries++ )
        {
            configPRINTF( ( "Waiting PSM enter event %u\r\n", tries ) );
            waitEventBits = xEventGroupWaitBits( _modemEventGroup,
                                                 MODEM_EVENT_PSM_ENTER_BIT,
                                                 pdTRUE,
                                                 pdFALSE,
                                                 pdMS_TO_TICKS( psmSettings.activeTimeValue * 1000UL ) );

            if( ( waitEventBits & MODEM_EVENT_PSM_ENTER_BIT ) != 0 )
            {
                break;
            }
        }

        /* Wait 5 seconds after PSM mode entered. */
        vTaskDelay( pdMS_TO_TICKS( 5000 ) );

        /* Send the AT command to cellular module should return error. */
        xCellularStatus = Cellular_ATCommandRaw( _cellularHandle,
                                                 NULL,
                                                 "AT",
                                                 CELLULAR_AT_NO_RESULT,
                                                 NULL,
                                                 NULL,
                                                 0U );
        TEST_CELLULAR_ASSERT_REQUIRED_API( CELLULAR_SUCCESS != xCellularStatus, xCellularStatus );
    }
    else
    {
        TEST_FAIL();
    }

    if( _modemEventGroup != NULL )
    {
        vEventGroupDelete( _modemEventGroup );
        _modemEventGroup = NULL;
    }
}

/*-----------------------------------------------------------*/
