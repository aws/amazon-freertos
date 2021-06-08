/*
 * FreeRTOS WiFi V2.0.0
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

/* Wi-Fi include. */
#include "iot_wifi.h"
#include "aws_clientcredential.h"

/* Testing variable includes. */
#include "aws_test_runner.h"
#include "aws_test_tcp.h"
#include "aws_test_utils.h"
#include "iot_test_wifi.h"
#include "aws_test_wifi_config.h"

/* Testing configurations defintions. */

/* The number of times to retry a WiFi Connect if it fails. */
#define testwifiCONNECTION_RETRY_TIMES    3
/* The initial delay between connection attempts that multiplies with each attempt. */
#define testwifiCONNECTION_RETRY_DELAY    250

/* The number of times to loop in the WiFiConnectionLoop test. */
#define testwifiCONNECTION_LOOP_TIMES     3

/* The delay in ms between connection and disconnection. This can be configured
 * in aws_test_wifi_config.h for your specific platform. */
#ifndef testwifiCONNECTION_DELAY
    #define testwifiCONNECTION_DELAY    pdMS_TO_TICKS( 2000 )
#endif

/* The maximum number of networks that can be scanned. For your specific
 * platform, this can be configured in aws_test_wifi_config.h. */
#ifndef testwifiMAX_SCAN_NUMBER
    #define testwifiMAX_SCAN_NUMBER    10
#endif

/* WIFI_Ping() test configurations. */
#define testwifiPING_COUNT          3
#define testwifiPING_INTERVAL_MS    10

/* The WIFF_Ping() testing address is currently set to the echo server see
 * aws_test_tcp.h. */
#define testwifiECHO_SERVER_ADDRESS                                              \
    SOCKETS_inet_addr_quick( tcptestECHO_SERVER_ADDR0, tcptestECHO_SERVER_ADDR1, \
                             tcptestECHO_SERVER_ADDR2, tcptestECHO_SERVER_ADDR3 )
#define testwifiPING_ADDRESS                testwifiECHO_SERVER_ADDRESS

/* WIFI_GetHostIP() test configurations. */
#define testwifiTEST_DOMAIN_NAME            "amazon.com" /* Valid domain name. */
#define testwifiTEST_INVALID_DOMAIN_NAME    "invalid"    /* Invalid domain name. */

/* Invalid credentials for WIFI_ConnectAP() tests. Valid credentials used are
 * defined in aws_clientcredentials.h. */
#define testwifiINVALID_WIFI_SSID \
    "invalid ssid"     /* This must be a SSID that does not exist in your range. */
#define testwifiINVALID_WIFI_PASSWORD \
    "invalid password" /* This must be a password that is mismatched from the \
                        * valid AP configured in aws_clientcredentials.h. */
#define testwifiINVALID_WIFI_SECURITY \
    eWiFiSecurityWEP   /* This must be a security type that is mismatched from the \
                        * valid AP configured in aws_clientcredentials.h. */

/* 14 total channels in the 2.4 GHz band. Set to the number of channels
 * available in your region. This can be configured in aws_test_wifi_config.h.
 */
#ifndef testwifiMAX_CHANNEL_NUMBER
    #define testwifiMAX_CHANNEL_NUMBER    14
#endif

/* 6 total bytes in a MAC address. */
#define testwifiMAC_ADDRESS_LENGTH    6

/* Arbitrary maximum number of networks to save for WIFI_NetworkGet(),
 * WIFI_NetworkSet() and WIFI_NetworkAdd() tests. This can be configured in
 * aws_test_wifi_config.h. */
#ifndef testwifiMAX_NETWORK_SAVE_NUMBER
    #define testwifiMAX_NETWORK_SAVE_NUMBER    100
#endif

/* String for round-trip testing. */
#define testwifiROUND_TRIP_TEST_STRING                    "abcd"
/* Arbitrary max length for the round trip string. */
#define testwifiROUND_TRIP_TEST_STRING_MAX_LENGTH         32
/* The amount of times to retry the socket connect in the round trip test. */
#define testwifiROUND_TRIP_TEST_RETRY_CONNECTION_TIMES    6

/* The delay in milliseconds that is multiplied during exponential retires of the socket
 * connection in the round trip test. This is to prevent network congestion. */
#define testwifiROUND_TRIP_TEST_RETRY_DELAY_MS            150

/* Socket receive and send timeouts for the loopback test. These can be
 * configured in aws_test_wifi_config.h*/
#ifndef testwifiLOOPBACK_TEST_SOCKETS_RECEIVE_TIMEOUT
    #define testwifiLOOPBACK_TEST_SOCKETS_RECEIVE_TIMEOUT    10000
#endif
#ifndef testwifiLOOPBACK_TEST_SOCKETS_SEND_TIMEOUT
    #define testwifiLOOPBACK_TEST_SOCKETS_SEND_TIMEOUT       10000
#endif

/* Wi-Fi connection multi-task test parameters. */

/* The number of tasks to create for the multi-task test. */
#define testwifiNUM_TASKS           2

/* The mask used in the task finish event group to signal that all tasks have
 * finished. */
#define testwifiTASK_FINISH_MASK    ( ( 1 << testwifiNUM_TASKS ) - 1 )

/* Task parameters for the Wi-Fi multi-task tests. These can be configured in
 * aws_test_wifi_config.h. */
#ifndef testwifiTASK_STACK_SIZE
    #define testwifiTASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 4 )
#endif
#ifndef testwifiTASK_PRIORITY
    #define testwifiTASK_PRIORITY      ( tskIDLE_PRIORITY )
#endif

/* Timeout for all of the tasks to finish the test. This can be configured in
 * aws_test_wifi_config.h. */
#ifndef testwifiMULTITASK_TEST_TIMEOUT
    #define testwifiMULTITASK_TEST_TIMEOUT    pdMS_TO_TICKS( 120000 )
#endif

/* Timeout when one task waits for the other to synchronize with it. This can be
 * configured in aws_test_wifi_config.h. */
#ifndef testwifiTASK_SYNC_TIMEOUT
    #define testwifiTASK_SYNC_TIMEOUT    pdMS_TO_TICKS( 30000 )
#endif

/* The event group used to synchronize the extended Wi-Fi multi-task tests. */
#define EXT_WIFI_EVENT_SCAN_DONE       ( 1 << 0 )
#define EXT_WIFI_EVENT_CONNECTED       ( 1 << 1 )
#define EXT_WIFI_EVENT_DISCONNECTED    ( 1 << 2 )
#define EXT_WIFI_EVENT_IP_READY        ( 1 << 3 )
static EventGroupHandle_t xExtWifiEventHandle;

/* Define to test extended Wifi API tests. */
#define testwifiENABLE_EXT_WIFI_TESTS    0

/* Maximum loop count waiting client connection */
#define MAX_LOOP_COUNT                   60

/* The event group used to synchronize the Wi-Fi multi-task tests. */
static EventGroupHandle_t xTaskFinishEventGroupHandle;

/* The event group to synchronize task connection and disconnection in the Wi-Fi
 * multi-task test. */
static EventGroupHandle_t xTaskConnectDisconnectSyncEventGroupHandle;

/* Task status for WiFiSeperateTasksConnectingAndDisconnectingAtOnce test. */
typedef struct
{
    uint16_t usTaskId;
    WIFIReturnCode_t xWiFiStatus;
    char cStatusMsg[ 100 ]; /* A status message associated with the WiFi return code. This is needed until there are more error return codes. */
    TaskHandle_t xTaskHandle;
} testwifiTaskParams_t;

/* Test WIFI_ConfigureAP(). Beware that enabling these tests might affect
 * WIFI_ConnectAP()
 * if in WIFI_ConnectAP() the mode is not switched from SoftAP to Station. */
#define testwifiENABLE_CONFIGURE_AP_TESTS    0

/* Functions that are required to be implemented for DQP_FR are:
 * - WIFI_On()
 * - WIFI_ConnectAP()
 * - WIFI_Disconnect()
 * - WIFI_Scan() - Microchip did not implement
 * - WIFI_GetIP()
 * - WIFI_GetMAC()
 * - WIFI_GetHostIP()
 */

/* Custom WIFI Test asserts. */
#define TEST_WIFI_ASSERT_REQUIRED_API( condition, result )             \
    if( result == eWiFiNotSupported )                                  \
    {                                                                  \
        TEST_FAIL_MESSAGE( "Required Wi-Fi API is not implemented." ); \
    }                                                                  \
    else                                                               \
    {                                                                  \
        TEST_ASSERT( condition );                                      \
    }

#define TEST_WIFI_ASSERT_REQUIRED_API_MSG( condition, result, message ) \
    if( result == eWiFiNotSupported )                                   \
    {                                                                   \
        TEST_FAIL_MESSAGE( "Required Wi-Fi API is not implemented." );  \
    }                                                                   \
    else                                                                \
    {                                                                   \
        TEST_ASSERT_MESSAGE( condition, message );                      \
    }

#define TEST_WIFI_ASSERT_OPTIONAL_API( condition, result ) \
    if( result == eWiFiNotSupported )                      \
    {                                                      \
        TEST_ASSERT( 1 );                                  \
    }                                                      \
    else                                                   \
    {                                                      \
        TEST_ASSERT( condition );                          \
    }

#define TEST_WIFI_ASSERT_OPTIONAL_API_MSG( condition, result, message ) \
    if( result == eWiFiNotSupported )                                   \
    {                                                                   \
        TEST_ASSERT( 1 );                                               \
    }                                                                   \
    else                                                                \
    {                                                                   \
        TEST_ASSERT_MESSAGE( condition, message );                      \
    }

/* Helper functions. */

/* Set the network parameters for this aws_clientcredential.h defined
 * parameters. */
static inline void prvSetClientNetworkParameters( WIFINetworkParams_t * pxClientNetworkParams )
{
    const char * pcSSID = clientcredentialWIFI_SSID;
    size_t xSSIDLength = strlen( pcSSID );
    const char * pcPassword = clientcredentialWIFI_PASSWORD;
    size_t xPasswordLength = strlen( pcPassword );

    memcpy( pxClientNetworkParams->ucSSID, pcSSID, xSSIDLength );
    pxClientNetworkParams->ucSSIDLength = xSSIDLength;
    pxClientNetworkParams->xSecurity = clientcredentialWIFI_SECURITY;

    if( ( pxClientNetworkParams->xSecurity == eWiFiSecurityWPA2 ) ||
        ( pxClientNetworkParams->xSecurity == eWiFiSecurityWPA ) )
    {
        memcpy( pxClientNetworkParams->xPassword.xWPA.cPassphrase, pcPassword, xPasswordLength );
        pxClientNetworkParams->xPassword.xWPA.ucLength = xPasswordLength;
    }
}

/* Set network parameters for this test's defined parameters. */
static inline void prvSetTestNetworkParameters( WIFINetworkParams_t * pxTestNetworkParams )
{
    const char * pcSSID = testwifiWIFI_SSID;
    size_t xSSIDLength = strlen( pcSSID );
    const char * pcPassword = testwifiWIFI_PASSWORD;
    size_t xPasswordLength = strlen( pcPassword );

    memcpy( pxTestNetworkParams->ucSSID, pcSSID, xSSIDLength );
    pxTestNetworkParams->ucSSIDLength = xSSIDLength;
    pxTestNetworkParams->xSecurity = testwifiWIFI_SECURITY;

    if( ( pxTestNetworkParams->xSecurity == eWiFiSecurityWPA2 ) ||
        ( pxTestNetworkParams->xSecurity == eWiFiSecurityWPA ) )
    {
        memcpy( pxTestNetworkParams->xPassword.xWPA.cPassphrase, pcPassword, xPasswordLength );
        pxTestNetworkParams->xPassword.xWPA.ucLength = xPasswordLength;
    }
}

/* Set the SoftAP network parameters for WIFI_ConfigureAP() test. */
static inline void prvSetSoftAPNetworkParameters( WIFINetworkParams_t * pxSoftAPNetworkParams )
{
    const char * pcSSID = wificonfigACCESS_POINT_SSID_PREFIX;
    size_t xSSIDLength = strlen( pcSSID );
    const char * pcPassword = wificonfigACCESS_POINT_PASSKEY;
    size_t xPasswordLength = strlen( pcPassword );

    memcpy( pxSoftAPNetworkParams->ucSSID, pcSSID, xSSIDLength );
    pxSoftAPNetworkParams->ucSSIDLength = xSSIDLength;
    pxSoftAPNetworkParams->xSecurity = wificonfigACCESS_POINT_SECURITY;

    if( ( pxSoftAPNetworkParams->xSecurity == eWiFiSecurityWPA2 ) ||
        ( pxSoftAPNetworkParams->xSecurity == eWiFiSecurityWPA ) )
    {
        memcpy( pxSoftAPNetworkParams->xPassword.xWPA.cPassphrase, pcPassword, xPasswordLength );
        pxSoftAPNetworkParams->xPassword.xWPA.ucLength = xPasswordLength;
    }

    pxSoftAPNetworkParams->ucChannel = wificonfigACCESS_POINT_CHANNEL;
}

/* Set the network profile for WIFI_NetworkGet() and WIFI_NetworkAdd() tests. */
static inline void prvSetNetworkProfile( WIFINetworkProfile_t * pxNetworkProfile )
{
    pxNetworkProfile->ucSSIDLength = strlen( clientcredentialWIFI_SSID );
    memcpy( pxNetworkProfile->ucSSID, clientcredentialWIFI_SSID,
            pxNetworkProfile->ucSSIDLength );
    pxNetworkProfile->ucPasswordLength = strlen( clientcredentialWIFI_PASSWORD );
    memcpy( pxNetworkProfile->cPassword, clientcredentialWIFI_PASSWORD,
            pxNetworkProfile->ucPasswordLength );
    pxNetworkProfile->xSecurity = clientcredentialWIFI_SECURITY;
}

/**
 * @brief Connect to the Wi-Fi access point and verify success.
 */
static BaseType_t prvConnectAPTest( void )
{
    WIFIReturnCode_t xWiFiStatus;
    WIFINetworkParams_t xNetworkParams = { 0 };
    BaseType_t xResult = pdPASS;
    uint32_t ulInitialRetryPeriodMs = testwifiCONNECTION_RETRY_DELAY;
    BaseType_t xMaxRetries = testwifiCONNECTION_RETRY_TIMES;

    prvSetClientNetworkParameters( &xNetworkParams );

    RETRY_EXPONENTIAL( xWiFiStatus = WIFI_ConnectAP( &xNetworkParams ),
                       eWiFiSuccess, ulInitialRetryPeriodMs, xMaxRetries );

    if( eWiFiSuccess != xWiFiStatus )
    {
        configPRINTF( ( "Could not connect to the access point.\r\n" ) );
        xResult = pdFAIL;
    }

    if( eWiFiSuccess == xWiFiStatus )
    {
        vTaskDelay( testwifiCONNECTION_DELAY );

        if( pdTRUE != WIFI_IsConnected( NULL ) )
        {
            configPRINTF( ( "Wi-Fi is not connected.\r\n" ) );
            xResult = pdFAIL;
        }
    }

    return xResult;
}

/**
 * @brief Send and receive a string to/from the Echo server defined in
 * aws_test_tcp.h.
 */
static BaseType_t prvRoundTripTest( void )
{
    Socket_t xSocket;
    SocketsSockaddr_t xEchoServerSockaddr;
    BaseType_t xNumBytesSentTotal = 0;
    BaseType_t xNumBytesSent = 0;
    BaseType_t xNumBytesRecvTotal = 0;
    BaseType_t xNumBytesRecv = 0;
    BaseType_t xSocketResult;
    TickType_t xTimeOnEnteringShutdown;
    char * pcRoundTripTestString = testwifiROUND_TRIP_TEST_STRING;
    size_t xRoundTripTestStringLength = sizeof( testwifiROUND_TRIP_TEST_STRING );
    char cRecvBuffer[ testwifiROUND_TRIP_TEST_STRING_MAX_LENGTH + 1 ];
    BaseType_t xResult = pdPASS;
    TickType_t xRxTimeOut =
        pdMS_TO_TICKS( testwifiLOOPBACK_TEST_SOCKETS_RECEIVE_TIMEOUT );
    TickType_t xTxTimeOut =
        pdMS_TO_TICKS( testwifiLOOPBACK_TEST_SOCKETS_SEND_TIMEOUT );
    uint32_t ulInitialRetryPeriodMs = testwifiROUND_TRIP_TEST_RETRY_DELAY_MS;
    BaseType_t xMaxRetries = testwifiROUND_TRIP_TEST_RETRY_CONNECTION_TIMES;

    configPRINTF(
        ( "%s:%d %s\r\n", __FILENAME__, __LINE__, "Starting round trip test." ) );

    /* Make socket. */
    xSocket =
        SOCKETS_Socket( SOCKETS_AF_INET, SOCKETS_SOCK_STREAM, SOCKETS_IPPROTO_TCP );

    if( xSocket == SOCKETS_INVALID_SOCKET )
    {
        xResult = pdFAIL;
        configPRINTF( ( "%s:%d %s\r\n", __FILENAME__, __LINE__,
                        "Error creating the socket." ) );
    }
    else
    {
        xSocketResult = SOCKETS_SetSockOpt(
            xSocket, 0, SOCKETS_SO_RCVTIMEO, &xRxTimeOut, sizeof( xRxTimeOut ) );

        if( xSocketResult != SOCKETS_ERROR_NONE )
        {
            xResult = pdFAIL;
            configPRINTF( ( "%s:%d %s %d\r\n", __FILENAME__, __LINE__,
                            "Failed to set the socket receive timeout.",
                            xSocketResult ) );
        }

        xSocketResult = SOCKETS_SetSockOpt(
            xSocket, 0, SOCKETS_SO_SNDTIMEO, &xTxTimeOut, sizeof( xTxTimeOut ) );

        if( xSocketResult != SOCKETS_ERROR_NONE )
        {
            xResult = pdFAIL;
            configPRINTF( ( "%s:%d %s %d\r\n", __FILENAME__, __LINE__,
                            "Failed to set the socket send timeout.",
                            xSocketResult ) );
        }
    }

    if( xResult == pdPASS )
    {
        /* Connect the socket to the echo server. If the Wi-Fi is not connected we
         * may fail here. */
        xEchoServerSockaddr.ulAddress = testwifiECHO_SERVER_ADDRESS;
        xEchoServerSockaddr.usPort = SOCKETS_htons( tcptestECHO_PORT );
        xEchoServerSockaddr.ucLength = sizeof( SocketsSockaddr_t );
        xEchoServerSockaddr.ucSocketDomain = SOCKETS_AF_INET;

        RETRY_EXPONENTIAL( xSocketResult = SOCKETS_Connect( xSocket, &xEchoServerSockaddr,
                                                            sizeof( xEchoServerSockaddr ) ),
                           SOCKETS_ERROR_NONE,
                           ulInitialRetryPeriodMs,
                           xMaxRetries );

        if( xSocketResult != SOCKETS_ERROR_NONE )
        {
            xResult = pdFAIL;
            configPRINTF( ( "%s:%d %s %d\r\n", __FILENAME__, __LINE__,
                            "Error connecting on the socket.",
                            xSocketResult ) );
        }
    }

    if( xResult == pdPASS )
    {
        /* Send the data. */
        while( ( size_t ) xNumBytesSentTotal < xRoundTripTestStringLength )
        {
            xNumBytesSent =
                SOCKETS_Send( xSocket, &pcRoundTripTestString[ xNumBytesSentTotal ],
                              xRoundTripTestStringLength - xNumBytesSentTotal, 0 );

            if( xNumBytesSent <= 0 )
            {
                xResult = pdFAIL;
                configPRINTF( ( "%s:%d %s %d\r\n", __FILENAME__, __LINE__,
                                "Error in SOCKETS_Send.  Return value:", xNumBytesSent ) );
                break;
            }
            else
            {
                xNumBytesSentTotal += xNumBytesSent;
            }
        }
    }

    if( xResult == pdPASS )
    {
        /* Clear the receive buffer. */
        memset( cRecvBuffer, 0, sizeof( cRecvBuffer ) );

        /* Receive the data. */
        while( ( size_t ) xNumBytesRecvTotal < xRoundTripTestStringLength )
        {
            xNumBytesRecv =
                SOCKETS_Recv( xSocket, &cRecvBuffer[ xNumBytesRecvTotal ],
                              xRoundTripTestStringLength - xNumBytesRecvTotal, 0 );

            if( xNumBytesRecv == 0 )
            {
                xResult = pdFAIL;
                configPRINTF( ( "%s:%d %s\r\n", __FILENAME__, __LINE__,
                                "Timed out receiving from echo server" ) );
                break;
            }
            else if( xNumBytesRecv < 0 )
            {
                xResult = pdFAIL;
                configPRINTF( ( "%s:%d %s %d\r\n", __FILENAME__, __LINE__,
                                "Error while receiving from echo server:",
                                xNumBytesRecv ) );
                break;
            }
            else
            {
                xNumBytesRecvTotal += xNumBytesRecv;
            }
        }

        if( ( size_t ) xNumBytesRecvTotal != xRoundTripTestStringLength )
        {
            xResult = pdFAIL;
            configPRINTF(
                ( "Mismatch in rx and tx; bytes sent: %d, bytes received: %d.\r\n",
                  xRoundTripTestStringLength, xNumBytesRecvTotal ) );
            configPRINTF( ( "String sent: %s, String received: %s\r\n",
                            pcRoundTripTestString, cRecvBuffer ) );
        }

        cRecvBuffer[ xRoundTripTestStringLength ] = '\0';
    }

    if( xResult == pdPASS )
    {
        /* Compare the received string with the test string. */
        if( strncmp( cRecvBuffer, pcRoundTripTestString,
                     xRoundTripTestStringLength ) != 0 )
        {
            xResult = pdFAIL;
            configPRINTF( ( "Test String: %s, Received String: %s\r\n",
                            testwifiROUND_TRIP_TEST_STRING, cRecvBuffer ) );
        }
    }

    if( xSocket != SOCKETS_INVALID_SOCKET )
    {
        /* Finished using the connected socket, initiate a graceful close:
         * FIN, FIN+ACK, ACK. */
        xSocketResult = SOCKETS_Shutdown( xSocket, SOCKETS_SHUT_RDWR );

        /* Expect SOCKETS_Recv() to return an error once the shutdown is
         * complete. */
        xTimeOnEnteringShutdown = xTaskGetTickCount();

        do
        {
            xNumBytesRecv =
                SOCKETS_Recv( xSocket,               /* The socket being received from. */
                              cRecvBuffer,           /* The buffer into which the received data
                                                      * will be written. */
                              sizeof( cRecvBuffer ), /* The size of the buffer provided
                                                      * to receive the data. */
                              0 );

            if( xNumBytesRecv < 0 )
            {
                break;
            }

            /* Wait for shutdown to complete.  If a receive block time is used then
             * this delay will not be necessary as FreeRTOS_recv() will place the RTOS
             * task into the Blocked state anyway. */
            vTaskDelay( 250 );
        } while( ( xTaskGetTickCount() - xTimeOnEnteringShutdown ) < xRxTimeOut );

        if( xSocketResult != SOCKETS_ERROR_NONE )
        {
            /* The loop back succeeded, which shows connectivity, so failing to
             * close the socket does not indicate failure. */
            configPRINTF( ( "%s:%d %s %d\r\n", __FILENAME__, __LINE__,
                            "Failed to shutdown the socket:",
                            ( int ) xSocketResult ) );
        }

        /* Close the socket. */
        xSocketResult = SOCKETS_Close( xSocket );

        if( SOCKETS_ERROR_NONE != xSocketResult )
        {
            /* The loop back succeeded, which shows connectivity, so failing to
             * close the socket does not indicate failure. */
            configPRINTF( ( "%s:%d %s %d\r\n", __FILENAME__, __LINE__,
                            "Failed to close the socket:",
                            ( int ) xSocketResult ) );
        }
    }

    configPRINTF( ( "%s:%d %s %d\r\n", __FILENAME__, __LINE__,
                    "Exiting round trip test with status:", xResult ) );
    return xResult;
}

/*-----------------------------------------------------------*/

static void prvFinishWiFiTesting( void )
{
    /* Disconnect first. */
    WIFI_Disconnect();

    /* Wait for disconnection. */
    vTaskDelay( testwifiCONNECTION_DELAY );

    /* Turn off the Wi-Fi. */
    WIFI_Off();

    /* Turn on the Wi-Fi. */
    WIFI_On();

    /* Turn on the Wi-Fi. */
    if( prvConnectAPTest() != pdPASS )
    {
        configPRINTF( ( "Could not reconnect the Wi-Fi following testing.\r\n" ) );
    }
    else
    {
        configPRINTF( ( "Wi-Fi reconnected following tests finished. \r\n" ) );
    }
}

/* Unity TEST initializations. */
TEST_GROUP( Full_WiFi );

TEST_SETUP( Full_WiFi )
{
}

TEST_TEAR_DOWN( Full_WiFi )
{
    WIFI_Disconnect();
}

TEST_GROUP_RUNNER( Full_WiFi )
{
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_Scan );
    /* Null parameter tests. */
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_GetMode_NullParameters );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_GetIP_NullParameters );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_GetMAC_NullParameters );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_GetHostIP_NullParameters );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_Scan_NullParameters );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_NetworkAdd_NullParameters );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_NetworkGet_NullParameters );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_SetPMMode_NullParameters );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_GetPMMode_NullParameters );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_Ping_NullParameters );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_ConnectAP_NullParameters );

    /* Invalid parameters tests. */
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_SetMode_InvalidMode );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_GetHostIP_InvalidDomainName );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_GetHostIP_DomainNameLengthExceeded );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_NetworkDelete_DeleteNonExistingNetwork );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_NetworkGetNonExistingNetwork );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_SetPMMode_InvalidPMMode );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_ConnectAP_InvalidSSID );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_ConnectAP_InvalidPassword );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_ConnectAP_MaxSSIDLengthExceeded );
    RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_ConnectAP_MaxPasswordLengthExceeded );

    /* Stability tests. */
    #if ( testwifiENABLE_CONFIGURE_AP_TESTS == 1 )
        RUN_TEST_CASE( Full_WiFi, AFQP_WiFiConfigureAP );
        RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_ConfigureAP_NullParameters );
        RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_ConfigureAP_InvalidSecurityType );
        RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_ConfigureAP_NullParameters );
        RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_ConfigureAP_MaxSSIDLengthExceeded );
        RUN_TEST_CASE( Full_WiFi, AFQP_WIFI_ConfigureAP_MaxPasswordLengthExceeded );
    #endif

    #if ( testwifiENABLE_EXT_WIFI_TESTS == 1 )
        RUN_TEST_CASE( Full_WiFi, AFQP_WifiScanAndGetResult );
        RUN_TEST_CASE( Full_WiFi, AFQP_WifiSetModeGetModeExt );
        RUN_TEST_CASE( Full_WiFi, AFQP_WifiStartConnectAPDisconnect );
        RUN_TEST_CASE( Full_WiFi, AFQP_CountryCode );
        RUN_TEST_CASE( Full_WiFi, AFQP_GetCapability );
        #if ( testwifiENABLE_CONFIGURE_AP_TESTS == 1 )
            RUN_TEST_CASE( Full_WiFi, AFQP_WiFiConfigureAPManual );
        #endif
    #endif /* testwifiENABLE_EXT_WIFI_TESTS */
    prvFinishWiFiTesting();
}

/**
 * Test group for quarantined WiFi tests.
 */
TEST_GROUP( Quarantine_WiFi );


TEST_SETUP( Quarantine_WiFi )
{
}

TEST_TEAR_DOWN( Quarantine_WiFi )
{
}

TEST_GROUP_RUNNER( Quarantine_WiFi )
{
    /* Happy path tests. */
    RUN_TEST_CASE( Quarantine_WiFi, AFQP_WiFiOnOff );
    RUN_TEST_CASE( Quarantine_WiFi, AFQP_WiFiMode );
    RUN_TEST_CASE( Quarantine_WiFi, AFQP_WiFiConnectionLoop );
    RUN_TEST_CASE( Quarantine_WiFi, AFQP_WiFiNetworkAddGetDelete );
    RUN_TEST_CASE( Quarantine_WiFi, AFQP_WiFiPowerManagementMode )
    RUN_TEST_CASE( Quarantine_WiFi, AFQP_WiFiGetIP );
    RUN_TEST_CASE( Quarantine_WiFi, AFQP_WiFiGetMAC );
    RUN_TEST_CASE( Quarantine_WiFi, AFQP_WiFiGetHostIP );
    RUN_TEST_CASE( Quarantine_WiFi, AFQP_WiFiScan );
    RUN_TEST_CASE( Quarantine_WiFi, AFQP_WiFiReset );
    RUN_TEST_CASE( Quarantine_WiFi, AFQP_WiFiPing );
    RUN_TEST_CASE( Quarantine_WiFi, AFQP_WiFiIsConnected );
    RUN_TEST_CASE( Quarantine_WiFi, AFQP_WiFiConnectMultipleAP );
    RUN_TEST_CASE( Quarantine_WiFi, AFQP_WiFiOnOffLoop );

    /* Stability tests. */
    RUN_TEST_CASE( Quarantine_WiFi, AFQP_WIFI_NetworkGet_GetManyNetworks );
    RUN_TEST_CASE( Quarantine_WiFi, AFQP_WIFI_NetworkAdd_AddManyNetworks );
}

/**
 * @brief Turn the Wi-Fi off then on in a loop and verify success. The Wi-Fi
 * should be ON after this test is finished.
 */
TEST( Quarantine_WiFi, AFQP_WiFiOnOffLoop )
{
    char cBuffer[ 256 ];
    int16_t sBufferLength = 256;
    uint32_t ulIndex;
    WIFIReturnCode_t xWiFiStatus;

    if( TEST_PROTECT() )
    {
        for( ulIndex = 0; ulIndex < testwifiCONNECTION_LOOP_TIMES; ++ulIndex )
        {
            xWiFiStatus = WIFI_Off();
            snprintf( cBuffer, sBufferLength,
                      "Failed WIFI_Off() in iteration %lu", ( long unsigned int ) ulIndex );
            TEST_WIFI_ASSERT_OPTIONAL_API_MSG(
                eWiFiSuccess == xWiFiStatus, xWiFiStatus, cBuffer );

            xWiFiStatus = WIFI_On();
            snprintf( cBuffer, sBufferLength, "Failed WIFI_On() in iteration %lu",
                      ( long unsigned int ) ulIndex );
            TEST_WIFI_ASSERT_REQUIRED_API_MSG(
                eWiFiSuccess == xWiFiStatus, xWiFiStatus, cBuffer );
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief A single happy path case of turning the Wi-Fi off, on, then connecting
 * to the AP and disconnecting. Verify API results.
 */
TEST( Quarantine_WiFi, AFQP_WiFiOnOff )
{
    WIFINetworkParams_t xNetworkParams = { 0 };
    WIFIReturnCode_t xWiFiStatus;

    prvSetClientNetworkParameters( &xNetworkParams );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_Off();
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        if( eWiFiSuccess == xWiFiStatus )
        {
            xWiFiStatus = WIFI_On();
            TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

            xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
            TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

            vTaskDelay( testwifiCONNECTION_DELAY );

            xWiFiStatus = WIFI_Disconnect();
            TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call WIFI_GetMode() and WIFI_SetMode() with each of the available
 * Wi-Fi modes; verify the API return status.
 */
TEST( Quarantine_WiFi, AFQP_WiFiMode )
{
    WIFIDeviceMode_t xWiFiDeviceMode;
    WIFIReturnCode_t xWiFiStatus;

    /* Valid Modes
     * ----------------------------------------------------------------- */

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetMode( &xWiFiDeviceMode );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        xWiFiStatus = WIFI_SetMode( eWiFiModeAP );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        xWiFiStatus = WIFI_SetMode( eWiFiModeP2P );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        xWiFiStatus = WIFI_SetMode( eWiFiModeStation );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        xWiFiStatus = WIFI_SetMode( xWiFiDeviceMode );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call WIFI_GetMode() with Null parameterrs and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_Scan )
{
    int32_t lI = 0;
    int8_t cScanSize = 10;
    WIFIReturnCode_t xWiFiStatus;
    WIFIScanResult_t xScanResults[ 10 ] = { 0 };

    /* Disconnect first before running any Wi-Fi test. */
    xWiFiStatus = WIFI_Disconnect();
    TEST_ASSERT_EQUAL_INT( eWiFiSuccess, xWiFiStatus );

    xWiFiStatus = WIFI_Scan( xScanResults, cScanSize );

    TEST_ASSERT_EQUAL_INT( eWiFiSuccess, xWiFiStatus );

    configPRINTF(
        ( "WiFi Networks and strength: \r\n" ) );

    for( lI = 0; lI < cScanSize; lI++ )
    {
        configPRINTF( ( "    %.*s: %d\r\n",
                        xScanResults[ lI ].ucSSIDLength, xScanResults[ lI ].ucSSID, xScanResults[ lI ].cRSSI ) );
    }

    configPRINTF(
        ( "End of WiFi Networks\r\n" ) );

    vTaskDelay( testwifiCONNECTION_DELAY );
}

/**
 * @brief Call WIFI_GetMode() with Null parameterrs and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_GetMode_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetMode( NULL );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Call WIFI_SetMode() with an invalid mode and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_SetMode_InvalidMode )
{
    WIFIReturnCode_t xWiFiStatus;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_SetMode( eWiFiModeNotSupported );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }

    /* Some ports will use an assert for invalid parameters instead of returning a failure code.
     * Because of this there is no else { TEST_FAIL(); } appended to this if( TEST_PROTECT() ) block.
     * This applies to all invalid parameter tests in this file. */
}

/**
 * @brief Connect and disconnect the wireless AP testwifiCONNECTION_LOOP_TIMES
 * and verify the return status each time.
 */
TEST( Quarantine_WiFi, AFQP_WiFiConnectionLoop )
{
    WIFINetworkParams_t xNetworkParams = { 0 };
    WIFIReturnCode_t xWiFiStatus;
    int16_t sConnectLoop;

    prvSetClientNetworkParameters( &xNetworkParams );

    /* Try connect and disconnect several times. Reconnect after the test. */
    if( TEST_PROTECT() )
    {
        for( sConnectLoop = 0; sConnectLoop < testwifiCONNECTION_LOOP_TIMES;
             ++sConnectLoop )
        {
            xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
            TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

            vTaskDelay( testwifiCONNECTION_DELAY );

            TEST_ASSERT( prvRoundTripTest() == pdPASS );

            xWiFiStatus = WIFI_Disconnect();
            TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

            vTaskDelay( testwifiCONNECTION_DELAY );
        }

        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
        TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Exercise WIFI_GetIP() and verify the return status.
 */
TEST( Quarantine_WiFi, AFQP_WiFiGetIP )
{
    WIFIReturnCode_t xWiFiStatus;
    WIFIIPConfiguration_t xIPConfiguration;

    memset( &xIPConfiguration, 0, sizeof( xIPConfiguration ) );

    /* Connect to the AP. */
    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    if( TEST_PROTECT() )
    {
        /* Acquire the IP address. */
        xWiFiStatus = WIFI_GetIPInfo( &xIPConfiguration );

        TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        /* Assert that the IP address is found. */
        TEST_ASSERT( xIPConfiguration.xIPAddress.ulAddress[ 0 ] != 0 );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call WIFI_GetIP() with NULL parameters and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_GetIP_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;

    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetIPInfo( NULL );

        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Exercise WIFI_GetMAC and verify the return status.
 *
 * The MAC address returned is checked to be non-zero. Checking for manufacturer
 * addresses given the unknown set of network interface vendors is infeasible.
 */
TEST( Quarantine_WiFi, AFQP_WiFiGetMAC )
{
    uint8_t ucMacAddressVal[ testwifiMAC_ADDRESS_LENGTH ];
    WIFIReturnCode_t xWiFiStatus;
    uint32_t ulIndex;
    uint32_t ulSum = 0;

    memset( ucMacAddressVal, 0, sizeof( ucMacAddressVal ) );

    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetMAC( ucMacAddressVal );

        TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        for( ulIndex = 0; ulIndex < testwifiMAC_ADDRESS_LENGTH; ulIndex++ )
        {
            ulSum += ucMacAddressVal[ ulIndex ];
        }

        TEST_ASSERT( ulSum != 0 );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call WIFI_GetMAC() with NULL parameters and verify the return status.
 */
TEST( Full_WiFi, AFQP_WIFI_GetMAC_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;

    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetMAC( NULL );
        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Exercise WIFI_GetHostIP and verify the return status.
 */
TEST( Quarantine_WiFi, AFQP_WiFiGetHostIP )
{
    uint8_t ucIPAddr[ 4 ];
    WIFIReturnCode_t xWiFiStatus;

    memset( ucIPAddr, 0, sizeof( ucIPAddr ) );

    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetHostIP( testwifiTEST_DOMAIN_NAME, ucIPAddr );

        TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        /* Assert that the IP address is found. */
        TEST_ASSERT( *( ( uint32_t * ) ucIPAddr ) != 0 );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call WIFI_GetHOSTIP with NULL parameters and verify the return status.
 */
TEST( Full_WiFi, AFQP_WIFI_GetHostIP_NullParameters )
{
    uint8_t ucIPAddr[ 4 ];
    WIFIReturnCode_t xWiFiStatus;

    memset( ucIPAddr, 0, sizeof( ucIPAddr ) );

    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetHostIP( testwifiTEST_DOMAIN_NAME, NULL );
        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetHostIP( NULL, ucIPAddr );
        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Call WIFI_GetHostIP with the invalid domain name defined in
 * testwifiTEST_INVALID_DOMAIN_NAME and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_GetHostIP_InvalidDomainName )
{
    uint8_t ucIPAddr[ 4 ];
    WIFIReturnCode_t xWiFiStatus;

    memset( ucIPAddr, 0, sizeof( ucIPAddr ) );

    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetHostIP( testwifiTEST_INVALID_DOMAIN_NAME, ucIPAddr );

        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );

        /* Assert that the IP address is NOT found. */
        TEST_ASSERT_EQUAL_INT32( *( ( uint32_t * ) ucIPAddr ), 0 );
    }
}

/**
 * @brief Call WIFI_GetHostIP with a domain name that exceeds the standard
 * maximum domain name length of 253 and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_GetHostIP_DomainNameLengthExceeded )
{
    uint8_t ucIPAddr[ 4 ];
    WIFIReturnCode_t xWiFiStatus;
    /* 256 slots: 0 .. 255 */
    char cDomainNameLengthExceeded[ securesocketsMAX_DNS_NAME_LENGTH + 3 ];

    memset( ucIPAddr, 0, sizeof( ucIPAddr ) );

    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    /* Setting 0 .. 254 to 'x'. */
    memset( cDomainNameLengthExceeded, 'x', sizeof( cDomainNameLengthExceeded ) );

    /* Setting 255 to null termination. */
    cDomainNameLengthExceeded[ securesocketsMAX_DNS_NAME_LENGTH + 2 ] = '\0';

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetHostIP( cDomainNameLengthExceeded, ucIPAddr );

        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );

        /* Assert that the IP address is NOT found. */
        TEST_ASSERT_EQUAL_INT32( *( ( uint32_t * ) ucIPAddr ), 0 );
    }
}

/**
 * @brief Exercise WIFI_Scan() and verify the return status.
 */
TEST( Quarantine_WiFi, AFQP_WiFiScan )
{
    WIFIScanResult_t xScanResults[ testwifiMAX_SCAN_NUMBER ];
    WIFIReturnCode_t xWiFiStatus;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_Scan( xScanResults, testwifiMAX_SCAN_NUMBER );
        TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call WIFI_Scan() with NULL parameters and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_Scan_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_Scan( NULL, testwifiMAX_SCAN_NUMBER );
        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Single test of adding a Wi-Fi network, getting it, then delete it;
 * verify the return status.
 */
TEST( Quarantine_WiFi, AFQP_WiFiNetworkAddGetDelete )
{
    WIFINetworkProfile_t xNetworkProfile;
    WIFIReturnCode_t xWiFiStatus;
    uint16_t usIndex;

    prvSetNetworkProfile( &xNetworkProfile );

    if( TEST_PROTECT() )
    {
        /* Delete all the networks first. */
        for( usIndex = 0; usIndex < testwifiMAX_NETWORK_SAVE_NUMBER; usIndex++ )
        {
            WIFI_NetworkDelete( usIndex );
        }

        /* Add a network. */
        xWiFiStatus = WIFI_NetworkAdd( &xNetworkProfile, &usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        /* Reset the network profile variable so that we can verify if we get the
         * one that was just added. */
        memset( &xNetworkProfile, 0, sizeof( WIFINetworkProfile_t ) );

        /* Get a network. */
        xWiFiStatus = WIFI_NetworkGet( &xNetworkProfile, usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        /* If successful, then implemented. So check network parameters are what was
         * just added. */
        if( xWiFiStatus == eWiFiSuccess )
        {
            TEST_ASSERT_EQUAL_INT32( strlen( clientcredentialWIFI_SSID ),
                                     xNetworkProfile.ucSSIDLength );
            TEST_ASSERT_EQUAL_INT32(
                0, strncmp( ( char * ) xNetworkProfile.ucSSID, clientcredentialWIFI_SSID,
                            strlen( clientcredentialWIFI_SSID ) ) );
            TEST_ASSERT_EQUAL_INT32( clientcredentialWIFI_SECURITY,
                                     xNetworkProfile.xSecurity );
        }

        /* Delete a network. */
        xWiFiStatus = WIFI_NetworkDelete( usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call WIFI_NetworkAdd() with NULL parameters and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_NetworkAdd_NullParameters )
{
    WIFINetworkProfile_t xNetworkProfile;
    WIFIReturnCode_t xWiFiStatus;
    uint16_t usIndex;

    prvSetNetworkProfile( &xNetworkProfile );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_NetworkAdd( &xNetworkProfile, NULL );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_NetworkAdd( NULL, &usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Call WIFI_NetworkGet() with NULL parameters and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_NetworkGet_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_NetworkGet( NULL, 0 );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Call WIFI_NetworkDelete() on a network that doesn't exist and verify
 * success.
 */
TEST( Full_WiFi, AFQP_WIFI_NetworkDelete_DeleteNonExistingNetwork )
{
    WIFINetworkProfile_t xNetworkProfile;
    WIFIReturnCode_t xWiFiStatus;
    uint16_t usIndex;

    prvSetNetworkProfile( &xNetworkProfile );

    if( TEST_PROTECT() )
    {
        /* Delete all the networks first. */
        for( usIndex = 0; usIndex < testwifiMAX_NETWORK_SAVE_NUMBER; usIndex++ )
        {
            WIFI_NetworkDelete( usIndex );
        }

        /* Add a network. */
        xWiFiStatus = WIFI_NetworkAdd( &xNetworkProfile, &usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        /* Delete the network. */
        xWiFiStatus = WIFI_NetworkDelete( usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        /* Delete non-existing network. */
        xWiFiStatus = WIFI_NetworkDelete( usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
}

/**
 * @brief Call WIFI_NetworkGet() on a non-existing network and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_NetworkGetNonExistingNetwork )
{
    WIFINetworkProfile_t xNetworkProfile;
    WIFIReturnCode_t xWiFiStatus;
    uint16_t usIndex;

    prvSetNetworkProfile( &xNetworkProfile );

    if( TEST_PROTECT() )
    {
        /* Delete all the networks first. */
        for( usIndex = 0; usIndex < testwifiMAX_NETWORK_SAVE_NUMBER; usIndex++ )
        {
            WIFI_NetworkDelete( usIndex );
        }

        /* Add a network. */
        xWiFiStatus = WIFI_NetworkAdd( &xNetworkProfile, &usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        /* Delete the network. */
        xWiFiStatus = WIFI_NetworkDelete( usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        /* Get non-existing network. */
        xWiFiStatus = WIFI_NetworkGet( &xNetworkProfile, usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess != xWiFiStatus, xWiFiStatus );
    }
}

/**
 * @brief Call WIFI_NetworkGet() over the maximum network save number.
 */
TEST( Quarantine_WiFi, AFQP_WIFI_NetworkGet_GetManyNetworks )
{
    WIFIReturnCode_t xWiFiStatus;
    WIFINetworkProfile_t xNetworkProfile;
    uint16_t usIndex;

    if( TEST_PROTECT() )
    {
        /* Delete all the networks first. */
        for( usIndex = 0; usIndex < testwifiMAX_NETWORK_SAVE_NUMBER; usIndex++ )
        {
            WIFI_NetworkDelete( usIndex );
        }

        for( usIndex = 0; usIndex < testwifiMAX_NETWORK_SAVE_NUMBER; usIndex++ )
        {
            xWiFiStatus = WIFI_NetworkGet( &xNetworkProfile, usIndex );
            TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess != xWiFiStatus, xWiFiStatus );
        }
    }
}

/**
 * @brief Call WIFI_NetworkAdd() over the maximum network save number.
 *
 * The number of networks allowed to be saved or if at all vary by platform.
 * Therefore, the success of WIFI_NetworkGet() and WIFI_NetworkAdd() is not
 * checked. Instead only the network returned on success is checked to be
 * correct.
 */
TEST( Quarantine_WiFi, AFQP_WIFI_NetworkAdd_AddManyNetworks )
{
    WIFIReturnCode_t xWiFiStatus;
    WIFINetworkProfile_t xNetworkProfile;
    uint16_t usNetworkIndex;
    uint16_t usSavedNetworkIndex;
    char cNetworkSSID[ wificonfigMAX_SSID_LEN + 1 ];
    int lCompResults;
    int lNetworkSSIDLength;

    /* Set some network profile fields. */
    prvSetNetworkProfile( &xNetworkProfile );

    if( TEST_PROTECT() )
    {
        /* Delete all the networks first. */
        for( usNetworkIndex = 0; usNetworkIndex < testwifiMAX_NETWORK_SAVE_NUMBER;
             usNetworkIndex++ )
        {
            WIFI_NetworkDelete( usNetworkIndex );
        }

        /* Add networks in increasing post-fix order in the SSID string. */
        for( usNetworkIndex = 0; usNetworkIndex < testwifiMAX_NETWORK_SAVE_NUMBER;
             usNetworkIndex++ )
        {
            /* Create the network SSID to verify was added. */
            lNetworkSSIDLength = sprintf( cNetworkSSID, "network%d", usNetworkIndex );
            cNetworkSSID[ lNetworkSSIDLength ] = '\0';
            strcpy( ( char * ) xNetworkProfile.ucSSID, cNetworkSSID );
            xNetworkProfile.ucSSIDLength = lNetworkSSIDLength;

            /* Add the network profile. */
            xWiFiStatus = WIFI_NetworkAdd( &xNetworkProfile, &usSavedNetworkIndex );
        }

        /* Verify added networks */
        for( usNetworkIndex = 0; usNetworkIndex < testwifiMAX_NETWORK_SAVE_NUMBER;
             usNetworkIndex++ )
        {
            /* Create the network SSID to verify was added. */
            lNetworkSSIDLength = sprintf( cNetworkSSID, "network%d", usNetworkIndex );
            cNetworkSSID[ lNetworkSSIDLength ] = '\0';

            /* Clear the network profile from the last round. */
            memset( &xNetworkProfile, 0, sizeof( WIFINetworkProfile_t ) );

            /* Get the network profile. */
            xWiFiStatus = WIFI_NetworkGet( &xNetworkProfile, usNetworkIndex );

            /* The board may not support the defined maximum networks saved so we need
             * to check if success
             * was returned from the last function before we check the saved network.
             */
            if( eWiFiSuccess == xWiFiStatus )
            {
                /* Verify the network profile SSID. The returned network SSID may not be
                 * null terminated. */
                lCompResults =
                    strncmp( ( char * ) xNetworkProfile.ucSSID, cNetworkSSID, lNetworkSSIDLength );
                TEST_ASSERT_EQUAL_INT32( 0, lCompResults );
            }
        }
    }
}

/**
 * @brief Exercise the WIFI_SetPMMode() and WIFI_GetPMMode() APIs for each of
 * the available modes and verify the return status.
 */
TEST( Quarantine_WiFi, AFQP_WiFiPowerManagementMode )
{
    WIFIPMMode_t xPMMode;
    WIFIReturnCode_t xWiFiStatus;
    uint32_t ulOptionValue = 0;

    if( TEST_PROTECT() )
    {
        xPMMode = eWiFiPMLowPower;
        xWiFiStatus = WIFI_SetPMMode( xPMMode, ( void * ) &ulOptionValue );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
        xWiFiStatus = WIFI_GetPMMode( &xPMMode, ( void * ) &ulOptionValue );
        TEST_WIFI_ASSERT_OPTIONAL_API(
            ( eWiFiSuccess == xWiFiStatus && eWiFiPMLowPower == xPMMode ),
            xWiFiStatus );

        xPMMode = eWiFiPMAlwaysOn;
        xWiFiStatus = WIFI_SetPMMode( xPMMode, ( void * ) &ulOptionValue );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
        xWiFiStatus = WIFI_GetPMMode( &xPMMode, ( void * ) &ulOptionValue );
        TEST_WIFI_ASSERT_OPTIONAL_API(
            ( eWiFiSuccess == xWiFiStatus && eWiFiPMAlwaysOn == xPMMode ),
            xWiFiStatus );

        xPMMode = eWiFiPMNormal;
        xWiFiStatus = WIFI_SetPMMode( xPMMode, ( void * ) &ulOptionValue );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
        xWiFiStatus = WIFI_GetPMMode( &xPMMode, ( void * ) &ulOptionValue );
        TEST_WIFI_ASSERT_OPTIONAL_API(
            ( eWiFiSuccess == xWiFiStatus && eWiFiPMNormal == xPMMode ), xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call WIFI_SetPMMode() with NULL parameters.
 */
TEST( Full_WiFi, AFQP_WIFI_SetPMMode_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_SetPMMode( eWiFiPMNormal, NULL );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief call WIFI_GetPMMode() with NULL parameters.
 */
TEST( Full_WiFi, AFQP_WIFI_GetPMMode_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;
    WIFIPMMode_t xPMMode;
    uint32_t ulOptionValue = 0;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetPMMode( &xPMMode, NULL );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetPMMode( NULL, ( void * ) &ulOptionValue );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief call WIFI_SetPMMode() with an invalid power mode and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_SetPMMode_InvalidPMMode )
{
    WIFIReturnCode_t xWiFiStatus;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_SetPMMode( eWiFiPMNotSupported, NULL );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Test to configure the Software Access Point and verify the return
 * status of the API.
 *
 * @note This test can only be verified manually. Set a breakpoint after
 * WIFI_ConfigureAP, which will block for some time. Connect to the Wi-Fi
 * network created by it on your phone and configure. You should expect it to
 * return eWiFiSuccess.
 */
TEST( Full_WiFi, AFQP_WiFiConfigureAP )
{
    WIFINetworkParams_t xNetworkParams = { 0 };
    WIFIReturnCode_t xWiFiStatus;

    /* Set the network parameters with valid parameters */
    prvSetSoftAPNetworkParameters( &xNetworkParams );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConfigureAP( &xNetworkParams );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call WIFI_ConfigureAP with NULL parameters and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_ConfigureAP_NullParameters )
{
    WIFINetworkParams_t xNetworkParams = { 0 };
    WIFIReturnCode_t xWiFiStatus;

    /* Null Network Parameters */
    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConfigureAP( NULL );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }

    /* Set the network parameters with valid parameters */
    prvSetSoftAPNetworkParameters( &xNetworkParams );

    /* Null SSID */

    memset( xNetworkParams.ucSSID, 0x00, sizeof( xNetworkParams.ucSSID ) );
    xNetworkParams.ucSSIDLength = 0;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConfigureAP( &xNetworkParams );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }

    /* Null password. */
    prvSetSoftAPNetworkParameters( &xNetworkParams );
    memset( xNetworkParams.xPassword.xWPA.cPassphrase, 0x00, sizeof( xNetworkParams.xPassword.xWPA.cPassphrase ) );
    xNetworkParams.xPassword.xWPA.ucLength = 0;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConfigureAP( &xNetworkParams );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }

    /* Null password for open security network. */

    xNetworkParams.xSecurity = eWiFiSecurityOpen;

    if( TEST_PROTECT() )
    {
        /* Some ports allow this some don't. */
        xWiFiStatus = WIFI_ConfigureAP( &xNetworkParams );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Configure the AP with an invalid security type an verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_ConfigureAP_InvalidSecurityType )
{
    WIFINetworkParams_t xNetworkParams = { 0 };
    WIFIReturnCode_t xWiFiStatus;

    /* Set the network parameters with valid parameters */
    prvSetSoftAPNetworkParameters( &xNetworkParams );

    xNetworkParams.xSecurity = eWiFiSecurityNotSupported;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConfigureAP( &xNetworkParams );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Configure the SoftAP with a SSID exceeding the maximum SSID length and
 * verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_ConfigureAP_MaxSSIDLengthExceeded )
{
    WIFINetworkParams_t xNetworkParams = { 0 };
    WIFIReturnCode_t xWiFiStatus;

    /* Set the network parameters with valid parameters */
    prvSetSoftAPNetworkParameters( &xNetworkParams );

    /* Make sure ssid is not null terminated and set the length to greater than max length. */
    memset( xNetworkParams.ucSSID, 'x', sizeof( xNetworkParams.ucSSID ) );
    xNetworkParams.ucSSIDLength = wificonfigMAX_SSID_LEN + 1;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConfigureAP( &xNetworkParams );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Configure the SoftAP with a password exceeding the maximum password
 * length and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_ConfigureAP_MaxPasswordLengthExceeded )
{
    WIFINetworkParams_t xNetworkParams = { 0 };
    WIFIReturnCode_t xWiFiStatus;

    /* Set the network parameters with valid parameters */
    prvSetSoftAPNetworkParameters( &xNetworkParams );

    /* Make sure password is not null terminated and set the length to greater than max length. */
    memset( xNetworkParams.xPassword.xWPA.cPassphrase, 'x', sizeof( xNetworkParams.xPassword.xWPA.cPassphrase ) );
    xNetworkParams.xPassword.xWPA.ucLength = wificonfigMAX_PASSPHRASE_LEN + 1;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConfigureAP( &xNetworkParams );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Exercise WIFI_Reset and verify a success.
 */
TEST( Quarantine_WiFi, AFQP_WiFiReset )
{
    WIFIReturnCode_t xWiFiStatus;

    xWiFiStatus = WIFI_Reset();
    TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
}

/**
 * @brief Exercise WIFI_Ping() and verify a success.
 */
TEST( Quarantine_WiFi, AFQP_WiFiPing )
{
    WIFIReturnCode_t xWiFiStatus;
    uint32_t ulPingAddress = testwifiPING_ADDRESS;

    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    xWiFiStatus = WIFI_Ping( ( uint8_t * ) &ulPingAddress, testwifiPING_COUNT,
                             testwifiPING_INTERVAL_MS );
    TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
}

/**
 * @brief Call WIFI_Ping() with NULL parameters and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_Ping_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;

    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_Ping( NULL, testwifiPING_COUNT, testwifiPING_INTERVAL_MS );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Test WIFI_IsConnected() after calling WIFI_ConnectAP() and
 * WIFI_DisconnectP() and verify success.
 */
TEST( Quarantine_WiFi, AFQP_WiFiIsConnected )
{
    WIFINetworkParams_t xNetworkParams = { 0 };
    BaseType_t xIsConnected;
    WIFIReturnCode_t xWiFiStatus;

    prvSetClientNetworkParameters( &xNetworkParams );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );

        vTaskDelay( testwifiCONNECTION_DELAY );

        xIsConnected = WIFI_IsConnected( NULL );
        TEST_WIFI_ASSERT_REQUIRED_API( pdTRUE == xIsConnected, xWiFiStatus );

        /* Confirm if we are truly connected with the round-trip test. */
        TEST_ASSERT( prvRoundTripTest() == pdPASS );

        xWiFiStatus = WIFI_Disconnect();
        TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        vTaskDelay( testwifiCONNECTION_DELAY );

        xIsConnected = WIFI_IsConnected( NULL );
        TEST_WIFI_ASSERT_REQUIRED_API( pdFALSE == xIsConnected, xIsConnected );

        TEST_ASSERT( prvRoundTripTest() != pdPASS );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Test WIFI_ConnectAP() with null parameters. It is expected that null
 * parameters will go into an assert or fail.
 */
TEST( Full_WiFi, AFQP_WIFI_ConnectAP_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;
    WIFINetworkParams_t xNetworkParams = { 0 };

    /* Null network params structure. */
    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConnectAP( NULL );
        TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess != xWiFiStatus, xWiFiStatus );
    }

    /* Null SSID */
    prvSetClientNetworkParameters( &xNetworkParams );

    /* Set SSID is to all zeroed buffer with a zero length. */
    memset( xNetworkParams.ucSSID, 0x00, sizeof( xNetworkParams.ucSSID ) );
    xNetworkParams.ucSSIDLength = 0;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }

    /* Test a null password when it is needed. */
    prvSetClientNetworkParameters( &xNetworkParams );
    memset( xNetworkParams.xPassword.xWPA.cPassphrase, 0x00, sizeof( xNetworkParams.xPassword.xWPA.cPassphrase ) );
    xNetworkParams.xPassword.xWPA.ucLength = 0;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }

    /* Test a null password when it is not needed. */
    xNetworkParams.xSecurity = eWiFiSecurityOpen;

    if( TEST_PROTECT() )
    {
        /* Every port will implement this differently. */
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
    }
}

/**
 * @brief Test WIFI_ConnectAP() with an invalid SSID.
 *
 * @note This test may fail for an valid SSID but invalid/valid password if
 * there exists a Access point, in range, with the same SSID but an Open
 * security type.
 */
TEST( Full_WiFi, AFQP_WIFI_ConnectAP_InvalidPassword )
{
    WIFINetworkParams_t xNetworkParams = { 0 };
    WIFIReturnCode_t xWiFiStatus = eWiFiFailure;
    BaseType_t xIsConnected;

    /* Set the valid client parameters. */
    prvSetClientNetworkParameters( &xNetworkParams );

    /* Set the invalid password. */
    strncpy( xNetworkParams.xPassword.xWPA.cPassphrase, testwifiINVALID_WIFI_PASSWORD, sizeof( xNetworkParams.xPassword.xWPA.cPassphrase ) );
    xNetworkParams.xPassword.xWPA.ucLength = strlen( testwifiINVALID_WIFI_PASSWORD );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
    }

    xIsConnected = WIFI_IsConnected( NULL );

    if( ( xIsConnected == pdFALSE ) && ( xWiFiStatus == eWiFiSuccess ) )
    {
        TEST_FAIL_MESSAGE(
            "WIFI_ConnectAP() returned success, but failed to connect." );
    }

    TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    TEST_WIFI_ASSERT_REQUIRED_API( pdFALSE == xIsConnected, xIsConnected );
}

/**
 * @brief Call WIFI_ConnectAP() with an invalid SSID and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_ConnectAP_InvalidSSID )
{
    WIFINetworkParams_t xNetworkParams = { 0 };
    WIFIReturnCode_t xWiFiStatus = eWiFiFailure;
    BaseType_t xIsConnected;

    /* Set the valid client parameters. */
    prvSetClientNetworkParameters( &xNetworkParams );

    /* Set the invalid SSID. */
    strncpy( ( char * ) xNetworkParams.ucSSID, testwifiINVALID_WIFI_SSID, sizeof( xNetworkParams.ucSSID ) );
    xNetworkParams.ucSSIDLength = strlen( testwifiINVALID_WIFI_SSID );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
    }

    xIsConnected = WIFI_IsConnected( NULL );

    if( ( xIsConnected == pdFALSE ) && ( xWiFiStatus == eWiFiSuccess ) )
    {
        TEST_FAIL_MESSAGE(
            "WIFI_ConnectAP() returned success, but failed to connect." );
    }

    TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    TEST_WIFI_ASSERT_REQUIRED_API( pdFALSE == xIsConnected, xIsConnected );
}

/**
 * @brief Call WIFI_ConnectAP() with an SSID that exceeds the maximum SSID
 * length and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_ConnectAP_MaxSSIDLengthExceeded )
{
    WIFINetworkParams_t xNetworkParams = { 0 };
    WIFIReturnCode_t xWiFiStatus = eWiFiFailure;
    BaseType_t xIsConnected;

    /* Set the valid client parameters. */
    prvSetClientNetworkParameters( &xNetworkParams );

    /* Ensure SSID is not null terminated and set the length to greater than max length */
    memset( xNetworkParams.ucSSID, 'x', sizeof( xNetworkParams.ucSSID ) );
    xNetworkParams.ucSSIDLength = wificonfigMAX_SSID_LEN + 1;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
    }

    xIsConnected = WIFI_IsConnected( NULL );

    if( ( xIsConnected == pdFALSE ) && ( xWiFiStatus == eWiFiSuccess ) )
    {
        TEST_FAIL_MESSAGE(
            "WIFI_ConnectAP() returned success, but failed to connect." );
    }

    TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    TEST_WIFI_ASSERT_REQUIRED_API( pdFALSE == xIsConnected, xIsConnected );
}

/**
 * @brief Call WIFI_ConnectAP() with a password that exceeds the maximum
 * password length and verify failure.
 */
TEST( Full_WiFi, AFQP_WIFI_ConnectAP_MaxPasswordLengthExceeded )
{
    WIFINetworkParams_t xNetworkParams = { 0 };
    WIFIReturnCode_t xWiFiStatus = eWiFiFailure;
    BaseType_t xIsConnected;

    /* Set the valid client parameters. */
    prvSetClientNetworkParameters( &xNetworkParams );

    /* Ensure password is not null terminated and set the length to greater than max length */
    memset( xNetworkParams.xPassword.xWPA.cPassphrase, 'x', sizeof( xNetworkParams.xPassword.xWPA.cPassphrase ) );
    xNetworkParams.xPassword.xWPA.ucLength = wificonfigMAX_PASSPHRASE_LEN + 1;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
    }

    xIsConnected = WIFI_IsConnected( NULL );

    if( ( xIsConnected == pdFALSE ) && ( xWiFiStatus == eWiFiSuccess ) )
    {
        TEST_FAIL_MESSAGE(
            "WIFI_ConnectAP() returned success, but failed to connect." );
    }

    TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    TEST_WIFI_ASSERT_REQUIRED_API( pdFALSE == xIsConnected, xIsConnected );
}

/**
 * @brief Connect to the Wi-Fi credentials in aws_clientcredential.h and the
 * credentials defined in this test over and over and verify we are still
 * connected.
 */
TEST( Quarantine_WiFi, AFQP_WiFiConnectMultipleAP )
{
    BaseType_t xIsConnected;
    BaseType_t xMaxRetries = 6;
    BaseType_t xMessageStringLength = 256;
    BaseType_t xRoundTripResults;
    char cMessageString[ 256 ];
    uint32_t ulIndex;
    uint32_t ulInitialRetryPeriodMs = 500;
    WIFINetworkParams_t xClientNetworkParams = { 0 };
    WIFINetworkParams_t xTestNetworkParams = { 0 };
    WIFIReturnCode_t eWiFiStatus = eWiFiFailure;

    configPRINTF( ( "%s:%d %s\r\n", __FILENAME__, __LINE__,
                    "AFQP_WiFiConnectMultipleAP Started" ) );

    /* Set the valid client parameters. */
    prvSetClientNetworkParameters( &xClientNetworkParams );

    /* Set the valid client parameters defined in this test. */
    prvSetTestNetworkParameters( &xTestNetworkParams );

    if( TEST_PROTECT() )
    {
        for( ulIndex = 0; ulIndex < testwifiCONNECTION_LOOP_TIMES; ulIndex++ )
        {
            RETRY_EXPONENTIAL( eWiFiStatus = WIFI_ConnectAP( &xTestNetworkParams ),
                               eWiFiSuccess, ulInitialRetryPeriodMs, xMaxRetries );

            /* TODO: Check for could not connect error code. This error code does not
             * exist yet. */
            snprintf( cMessageString, xMessageStringLength,
                      "Could not connect to %.*s on iteration %d after %d "
                      "retries. Status was %d.",
                      xTestNetworkParams.ucSSIDLength, ( char * ) xTestNetworkParams.ucSSID, ( int ) ulIndex, ( int ) xMaxRetries,
                      eWiFiStatus );
            TEST_WIFI_ASSERT_REQUIRED_API_MSG(
                eWiFiStatus == eWiFiSuccess, eWiFiStatus, cMessageString );

            vTaskDelay( testwifiCONNECTION_DELAY );

            xIsConnected = WIFI_IsConnected( NULL );
            TEST_WIFI_ASSERT_REQUIRED_API_MSG(
                pdTRUE == xIsConnected, xIsConnected,
                ( "API is violated, we must connect to the new network." ) );

            /* Perform the round-trip test to verify we are actually connected.
             * The client credential network is the network expected to have the
             * echo server, since the tcp tests use that network. */
            xRoundTripResults = prvRoundTripTest();

            if( xRoundTripResults == pdFAIL )
            {
                snprintf( cMessageString, xMessageStringLength,
                          "Wi-Fi API claims to be connected to %.*s, but round "
                          "trip test on iteration %d failed.\r\n",
                          xTestNetworkParams.ucSSIDLength, ( char * ) xTestNetworkParams.ucSSID, ( int ) ulIndex );
                TEST_FAIL_MESSAGE( cMessageString );
            }

            RETRY_EXPONENTIAL( eWiFiStatus = WIFI_ConnectAP( &xClientNetworkParams ),
                               eWiFiSuccess, ulInitialRetryPeriodMs, xMaxRetries );

            snprintf( cMessageString, xMessageStringLength,
                      "Could not connect to %.*s on iteration %d after %d "
                      "retries. Status was %d.",
                      xTestNetworkParams.ucSSIDLength, ( char * ) xTestNetworkParams.ucSSID,
                      ( int ) ulIndex, ( int ) xMaxRetries,
                      eWiFiStatus );
            TEST_WIFI_ASSERT_REQUIRED_API_MSG(
                eWiFiStatus == eWiFiSuccess, eWiFiStatus, cMessageString );

            vTaskDelay( testwifiCONNECTION_DELAY );

            xIsConnected = WIFI_IsConnected( NULL );
            TEST_WIFI_ASSERT_REQUIRED_API_MSG(
                pdTRUE == xIsConnected, xIsConnected,
                ( "API is violated, we must connect to the new network." ) );

            /* Perform the round-trip test to verify we are actually connected. The
             * client credential network is the network expected to have the echo
             * server, since the tcp tests use that network. */
            xRoundTripResults = prvRoundTripTest();

            if( xRoundTripResults == pdFAIL )
            {
                snprintf( cMessageString, xMessageStringLength,
                          "Wi-Fi API claims to be connected to %.*s, but round "
                          "trip test on iteration %lu failed.\r\n",
                          xTestNetworkParams.ucSSIDLength, ( char * ) xTestNetworkParams.ucSSID,
                          ( long unsigned int ) ulIndex );
                TEST_FAIL_MESSAGE( cMessageString );
            }
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Connection loop task. This function will connect then disconnect
 * checking for the thread-safety of WIFI_ConnectAP() and WIFI_Disconnect().
 */
static void prvConnectionTask( void * pvParameters )
{
    WIFIReturnCode_t xWiFiConnectStatus;
    uint32_t ulIndex;
    BaseType_t xIsConnected;
    BaseType_t xRoundTripResults;
    WIFINetworkParams_t xClientNetworkParams = { 0 };
    WIFINetworkParams_t xTestNetworkParams = { 0 };
    testwifiTaskParams_t * pxTaskParams;

    /* Set the network parameters. */
    prvSetClientNetworkParameters( &xClientNetworkParams );
    prvSetTestNetworkParameters( &xTestNetworkParams );

    /* Initialize to failure and set to success after the connection loop is
     * complete. */
    pxTaskParams = ( testwifiTaskParams_t * ) ( pvParameters );
    pxTaskParams->xWiFiStatus = eWiFiFailure;
    memset( pxTaskParams->cStatusMsg, 0, sizeof( pxTaskParams->cStatusMsg ) );

    for( ulIndex = 0; ulIndex < testwifiCONNECTION_LOOP_TIMES; ulIndex++ )
    {
        /* Connect to the test network. */
        xWiFiConnectStatus = WIFI_ConnectAP( &xTestNetworkParams );

        if( xWiFiConnectStatus != eWiFiSuccess )
        {
            snprintf( pxTaskParams->cStatusMsg,
                      sizeof( pxTaskParams->cStatusMsg ),
                      "Task %d failed to connect to the AP %.*s with error code %d.\r\n",
                      pxTaskParams->usTaskId,
                      xTestNetworkParams.ucSSIDLength, ( char * ) xTestNetworkParams.ucSSID,
                      xWiFiConnectStatus );
            configPRINTF( ( pxTaskParams->cStatusMsg ) );
            break;
        }

        /* Delay. */
        vTaskDelay( testwifiCONNECTION_DELAY );

        /* Connect to the client credential network, where the echo server is
         * attached. */
        xWiFiConnectStatus = WIFI_ConnectAP( &xClientNetworkParams );

        if( xWiFiConnectStatus != eWiFiSuccess )
        {
            snprintf( pxTaskParams->cStatusMsg,
                      sizeof( pxTaskParams->cStatusMsg ),
                      "Task %d failed to connect to the AP %.*s with error code %d.\r\n",
                      pxTaskParams->usTaskId,
                      xTestNetworkParams.ucSSIDLength, ( char * ) xTestNetworkParams.ucSSID,
                      xWiFiConnectStatus );
            configPRINTF( ( pxTaskParams->cStatusMsg ) );
            break;
        }

        /* Delay. */
        vTaskDelay( testwifiCONNECTION_DELAY );

        /* Wait for the other tasks to finish connecting. */
        if( ( xTaskConnectDisconnectSyncEventGroupHandle != NULL ) &&
            ( xEventGroupSync(
                  xTaskConnectDisconnectSyncEventGroupHandle,
                  ( 0x1
                      << pxTaskParams->usTaskId ), /* Set our task ID when we are done. */
                  testwifiTASK_FINISH_MASK,        /* Wait for both our task and the other
                                                    * task. */
                  testwifiTASK_SYNC_TIMEOUT ) != testwifiTASK_FINISH_MASK ) )
        {
            snprintf( pxTaskParams->cStatusMsg,
                      sizeof( pxTaskParams->cStatusMsg ),
                      "Task %d timeout waiting for the other task to finish "
                      "connection.\r\n",
                      pxTaskParams->usTaskId );

            configPRINTF( ( pxTaskParams->cStatusMsg ) );
            break;
        }

        /* Check that we are indicated as connected. */
        xIsConnected = WIFI_IsConnected( NULL );

        if( xIsConnected == pdFALSE )
        {
            configPRINTF(
                ( "Task %d indicates from WIFI_IsConnected() that it is not "
                  "connected.\r\n",
                  pxTaskParams->usTaskId ) );
            break;
        }

        /* Attempt a round trip test to verify connection. */
        xRoundTripResults = prvRoundTripTest();

        if( xRoundTripResults == pdFALSE )
        {
            snprintf( pxTaskParams->cStatusMsg,
                      sizeof( pxTaskParams->cStatusMsg ),
                      "Task %d failed the round-trip test after connect.\r\n",
                      pxTaskParams->usTaskId );
            configPRINTF( ( pxTaskParams->cStatusMsg ) );
            break;
        }

        /* Wait for the other tasks before moving on to disconnecting. */
        if( ( xTaskConnectDisconnectSyncEventGroupHandle != NULL ) &&
            ( xEventGroupSync(
                  xTaskConnectDisconnectSyncEventGroupHandle, /* The event group used
                                                               * for the rendezvous.
                                                               */
                  ( 0x1
                      << pxTaskParams->usTaskId ),            /* Set our task ID when we are done. */
                  testwifiTASK_FINISH_MASK,                   /* Wait for both our task and the other
                                                               * task. */
                  testwifiTASK_SYNC_TIMEOUT ) != testwifiTASK_FINISH_MASK ) )
        {
            snprintf( pxTaskParams->cStatusMsg,
                      sizeof( pxTaskParams->cStatusMsg ),
                      "Task %d timeout waiting for the other task to finish round-trip "
                      "test after connect.\r\n",
                      pxTaskParams->usTaskId );
            configPRINTF( ( pxTaskParams->cStatusMsg ) );
            break;
        }

        /* ---------------------------------------------------------------------------------------
         */

        /* Disconnect. */
        xWiFiConnectStatus = WIFI_Disconnect();

        if( xWiFiConnectStatus != eWiFiSuccess )
        {
            configPRINTF( ( "Task %d failed to disconnect with Wi-Fi error code %d\r\n",
                            pxTaskParams->usTaskId, xWiFiConnectStatus ) );
            break;
        }

        /* Delay. */
        vTaskDelay( testwifiCONNECTION_DELAY );

        /* Wait for the other tasks. */
        if( ( xTaskConnectDisconnectSyncEventGroupHandle != NULL ) &&
            ( xEventGroupSync(
                  xTaskConnectDisconnectSyncEventGroupHandle,
                  ( 0x1
                      << pxTaskParams->usTaskId ), /* Set our task ID when we are done. */
                  testwifiTASK_FINISH_MASK,        /* Wait for both our task and the other
                                                    * task. */
                  testwifiTASK_SYNC_TIMEOUT ) != testwifiTASK_FINISH_MASK ) )
        {
            snprintf( pxTaskParams->cStatusMsg,
                      sizeof( pxTaskParams->cStatusMsg ),
                      "Task %d timeout waiting for the other task to finish "
                      "disconnection.\r\n",
                      pxTaskParams->usTaskId );
            configPRINTF( ( pxTaskParams->cStatusMsg ) );
            break;
        }

        /* Are we disconnected? */
        xIsConnected = WIFI_IsConnected( NULL );

        if( xIsConnected == pdTRUE )
        {
            snprintf( pxTaskParams->cStatusMsg,
                      sizeof( pxTaskParams->cStatusMsg ),
                      "Task %d returned eWiFiSuccess from WIFI_Disconnect(), but is not "
                      "disconnected.\r\n",
                      pxTaskParams->usTaskId );
            configPRINTF( ( pxTaskParams->cStatusMsg ) );
            break;
        }

        /* Perform a round-trip test to ensure we are NOT connected. */
        xRoundTripResults = prvRoundTripTest();

        if( xRoundTripResults == pdPASS )
        {
            snprintf( pxTaskParams->cStatusMsg,
                      sizeof( pxTaskParams->cStatusMsg ),
                      "Task %d completed the round-trip test after supposedly "
                      "disconnecting.\r\n",
                      pxTaskParams->usTaskId );
            configPRINTF( ( pxTaskParams->cStatusMsg ) );
            break;
        }

        /* Wait for the other tasks. */
        if( ( xTaskConnectDisconnectSyncEventGroupHandle != NULL ) &&
            ( xEventGroupSync(
                  xTaskConnectDisconnectSyncEventGroupHandle,
                  ( 0x1
                      << pxTaskParams->usTaskId ), /* Set our task ID when we are done. */
                  testwifiTASK_FINISH_MASK,        /* Wait for both our task and the other
                                                    * task. */
                  testwifiTASK_SYNC_TIMEOUT ) != testwifiTASK_FINISH_MASK ) )
        {
            snprintf( pxTaskParams->cStatusMsg,
                      sizeof( pxTaskParams->cStatusMsg ),
                      "Task %d timeout waiting for the other task to finish the round "
                      "trip after disconnect.\r\n",
                      pxTaskParams->usTaskId );
            configPRINTF( ( pxTaskParams->cStatusMsg ) );
            break;
        }
    }

    /* If the loop was completed without breaking, the task succeeded. */
    if( ulIndex >= testwifiCONNECTION_LOOP_TIMES )
    {
        pxTaskParams->xWiFiStatus = eWiFiSuccess;
    }

    /* Flag that the task is done. */
    if( xTaskFinishEventGroupHandle != NULL )
    {
        xEventGroupSetBits( xTaskFinishEventGroupHandle,
                            ( 1 << pxTaskParams->usTaskId ) );
    }

    vTaskDelete( NULL ); /* Delete this task. */
}

/**
 * @brief Spawn two threads to connect and disconnect at the same time. This is
 * to verify thread safety in the Wi-Fi driver API.
 */
TEST( Full_WiFi, AFQP_WiFiSeperateTasksConnectingAndDisconnectingAtOnce )
{
    BaseType_t xTaskCreateResult;
    char cBuffer[ 256 ];
    int16_t sBufferLength = 256;
    testwifiTaskParams_t xTaskParams[ testwifiNUM_TASKS ];
    uint16_t usIndex;
    uint32_t ulResultEventMask;

    if( TEST_PROTECT() )
    {
        /* Create an event group for tasks finishing testing. */
        xTaskFinishEventGroupHandle = xEventGroupCreate();
        configASSERT( xTaskFinishEventGroupHandle != NULL );

        /* Even group for task connection and disconnection synchronization. */
        xTaskConnectDisconnectSyncEventGroupHandle = xEventGroupCreate();
        configASSERT( xTaskConnectDisconnectSyncEventGroupHandle != NULL );

        /* Create the task that connect at the same time. */
        for( usIndex = 0; usIndex < testwifiNUM_TASKS; usIndex++ )
        {
            xTaskParams[ usIndex ].usTaskId = usIndex;
            xTaskParams[ usIndex ].xWiFiStatus = eWiFiFailure;
            xTaskCreateResult = xTaskCreate(
                prvConnectionTask, "WiFiConnectionTask", testwifiTASK_STACK_SIZE,
                &xTaskParams[ usIndex ], testwifiTASK_PRIORITY,
                &( xTaskParams[ usIndex ].xTaskHandle ) );

            if( xTaskCreateResult != pdPASS )
            {
                configPRINTF(
                    ( "Wi-Fi connection task %d creation failed.\r\n", usIndex ) );
            }

            TEST_ASSERT_EQUAL_INT32( pdPASS, xTaskCreateResult );
        }

        /* Wait for all of the tasks to finish. */
        ulResultEventMask = xEventGroupWaitBits(
            xTaskFinishEventGroupHandle, testwifiTASK_FINISH_MASK, pdTRUE, pdTRUE,
            testwifiMULTITASK_TEST_TIMEOUT );

        if( ulResultEventMask != testwifiTASK_FINISH_MASK )
        {
            configPRINTF(
                ( "Timed out waiting for all connection/disconnection tasks to "
                  "finish.\r\n" ) );
        }

        /* Check the task status and assert test results. */
        for( usIndex = 0; usIndex < testwifiNUM_TASKS; usIndex++ )
        {
            snprintf( cBuffer, sBufferLength, "Task %d failed.\r\n", usIndex );
            TEST_WIFI_ASSERT_REQUIRED_API_MSG(
                xTaskParams[ usIndex ].xWiFiStatus == eWiFiSuccess,
                xTaskParams[ usIndex ].xWiFiStatus, cBuffer );
        }
    }
    else
    {
        TEST_FAIL();
    }

    /* Clean up the task finish event group. */
    if( xTaskFinishEventGroupHandle != NULL )
    {
        vEventGroupDelete( xTaskFinishEventGroupHandle );
        xTaskFinishEventGroupHandle = NULL;
    }

    /* Clean up the connection and disconnection sync event groups. */
    if( xTaskConnectDisconnectSyncEventGroupHandle != NULL )
    {
        vEventGroupDelete( xTaskConnectDisconnectSyncEventGroupHandle );
        xTaskConnectDisconnectSyncEventGroupHandle = NULL;
    }
}

/* --------------------------------------------------------------------- */
static void extWifiEventCb( WIFIEvent_t * xEvent )
{
    if( xExtWifiEventHandle && xEvent )
    {
        switch( xEvent->xEventType )
        {
            case eWiFiEventScanDone:
                xEventGroupSetBits( xExtWifiEventHandle, EXT_WIFI_EVENT_SCAN_DONE );
                break;

            case eWiFiEventConnected:
                xEventGroupSetBits( xExtWifiEventHandle, EXT_WIFI_EVENT_CONNECTED );
                break;

            case eWiFiEventDisconnected:
                xEventGroupSetBits( xExtWifiEventHandle, EXT_WIFI_EVENT_DISCONNECTED );
                break;

            case eWiFiEventIPReady:
                xEventGroupSetBits( xExtWifiEventHandle, EXT_WIFI_EVENT_IP_READY );
                break;

            default:
                configPRINTF( ( "Unsupported event=%d\n", xEvent->xEventType ) );
                break;
        }
    }
}

/**
 * @brief Scan and get result task. This function will do a sccan. Wait for the scan done
 * event and get the result.
 */
TEST( Full_WiFi, AFQP_WifiScanAndGetResult )
{
    WIFIReturnCode_t rtnCode;
    WIFIScanConfig_t scanConfig;
    uint16_t ucNumNetworks;
    const WIFIScanResult_t * pScanResult;
    EventBits_t eventBits;
    int i;

    xExtWifiEventHandle = xEventGroupCreate();
    configASSERT( xExtWifiEventHandle != NULL );

    rtnCode = WIFI_RegisterEvent( eWiFiEventScanDone, extWifiEventCb );
    TEST_WIFI_ASSERT_OPTIONAL_API( rtnCode == eWiFiSuccess, rtnCode );

    memset( &scanConfig, 0x0, sizeof( WIFIScanConfig_t ) );
    scanConfig.ucSSIDLength = strlen( clientcredentialWIFI_SSID );
    memcpy( scanConfig.ucSSID, clientcredentialWIFI_SSID, scanConfig.ucSSIDLength );

    xEventGroupClearBits( xExtWifiEventHandle, EXT_WIFI_EVENT_SCAN_DONE );

    rtnCode = WIFI_StartScan( &scanConfig );
    TEST_WIFI_ASSERT_OPTIONAL_API( rtnCode == eWiFiSuccess, rtnCode );

    /* Wait for result */
    eventBits = xEventGroupWaitBits( xExtWifiEventHandle, EXT_WIFI_EVENT_SCAN_DONE, pdTRUE, pdTRUE, testwifiTASK_SYNC_TIMEOUT );
    configASSERT( eventBits == EXT_WIFI_EVENT_SCAN_DONE );

    rtnCode = WIFI_GetScanResults( &pScanResult, &ucNumNetworks );
    TEST_WIFI_ASSERT_OPTIONAL_API( rtnCode == eWiFiSuccess, rtnCode );

    /* Print the result */
    for( i = 0; i < ucNumNetworks; ++i, ++pScanResult )
    {
        char ssid[ 33 ];

        memset( ssid, 0, sizeof( ssid ) );
        memcpy( ssid, pScanResult->ucSSID, pScanResult->ucSSIDLength );
        configPRINTF( ( "[%2d]: Channel=%2u, BSSID=%02X:%02X:%02X:%02X:%02X:%02X, "
                        "RSSI=%d, Security=%u, Len=%2u, SSID=%s\n",
                        i,
                        pScanResult->ucChannel,
                        pScanResult->ucBSSID[ 0 ],
                        pScanResult->ucBSSID[ 1 ],
                        pScanResult->ucBSSID[ 2 ],
                        pScanResult->ucBSSID[ 3 ],
                        pScanResult->ucBSSID[ 4 ],
                        pScanResult->ucBSSID[ 5 ],
                        pScanResult->cRSSI,
                        pScanResult->xSecurity,
                        pScanResult->ucSSIDLength,
                        ssid ) );
    }

    /* Clean up */
    vEventGroupDelete( xExtWifiEventHandle );
    xExtWifiEventHandle = NULL;
}

/**
 * @brief Set and get mode ext. This function will call WIFI_SetModeExt, then call WIFI_GetModeExt
 * to verify the result
 */
TEST( Full_WiFi, AFQP_WifiSetModeGetModeExt )
{
    WIFIReturnCode_t retCode;
    WIFIDeviceMode_t orgMode, deviceMode;

    /* Get the original mode, to be restored later */
    retCode = WIFI_GetMode( &orgMode );
    TEST_ASSERT( retCode == eWiFiSuccess );

    retCode = WIFI_SetMode( eWiFiModeStation );
    TEST_WIFI_ASSERT_OPTIONAL_API( retCode == eWiFiSuccess, retCode );

    retCode = WIFI_GetMode( &deviceMode );
    TEST_ASSERT( retCode == eWiFiSuccess );
    TEST_ASSERT( deviceMode == eWiFiModeStation );

    retCode = WIFI_SetMode( eWiFiModeAP );
    TEST_WIFI_ASSERT_OPTIONAL_API( retCode == eWiFiSuccess, retCode );

    retCode = WIFI_GetMode( &deviceMode );
    TEST_ASSERT( retCode == eWiFiSuccess );
    TEST_ASSERT( deviceMode == eWiFiModeAP );

    /* Restore the mode */
    retCode = WIFI_SetMode( orgMode );
    TEST_WIFI_ASSERT_OPTIONAL_API( retCode == eWiFiSuccess, retCode );
}

/**
 * @brief Connect related task. This function will do a connection. Then it will call
 * WIFI_GetRSSI, WIFI_GetStatistic WIFI_GetIPInfo and finally WIFI_StartDisconnect
 */
TEST( Full_WiFi, AFQP_WifiStartConnectAPDisconnect )
{
    EventBits_t eventBits;
    WIFIReturnCode_t retCode;
    WIFINetworkParams_t networkParams;
    WIFIIPConfiguration_t ipConfig;
    int8_t rssi;
    WIFIStatisticInfo_t xStatistics;

    retCode = WIFI_RegisterEvent( eWiFiEventConnected, extWifiEventCb );
    TEST_ASSERT( retCode == eWiFiSuccess );

    retCode = WIFI_RegisterEvent( eWiFiEventDisconnected, extWifiEventCb );
    TEST_ASSERT( retCode == eWiFiSuccess );

    retCode = WIFI_RegisterEvent( eWiFiEventIPReady, extWifiEventCb );
    TEST_ASSERT( retCode == eWiFiSuccess );

    xExtWifiEventHandle = xEventGroupCreate();
    configASSERT( xExtWifiEventHandle != NULL );

    xEventGroupClearBits( xExtWifiEventHandle, EXT_WIFI_EVENT_CONNECTED | EXT_WIFI_EVENT_IP_READY );

    memset( &networkParams, 0x0, sizeof( WIFINetworkParams_t ) );
    networkParams.ucSSIDLength = strlen( clientcredentialWIFI_SSID );
    memcpy( networkParams.ucSSID, clientcredentialWIFI_SSID, networkParams.ucSSIDLength );

    networkParams.xSecurity = clientcredentialWIFI_SECURITY;
    networkParams.xPassword.xWPA.ucLength = sizeof( clientcredentialWIFI_PASSWORD );
    memcpy( networkParams.xPassword.xWPA.cPassphrase, clientcredentialWIFI_PASSWORD,
            networkParams.xPassword.xWPA.ucLength );
    retCode = WIFI_StartConnectAP( &networkParams );
    TEST_ASSERT( retCode == eWiFiSuccess );

    /* Wait for connected and IP ready event */
    eventBits = xEventGroupWaitBits( xExtWifiEventHandle, EXT_WIFI_EVENT_CONNECTED | EXT_WIFI_EVENT_IP_READY, pdFALSE, pdTRUE, testwifiTASK_SYNC_TIMEOUT );
    TEST_ASSERT( eventBits == ( EXT_WIFI_EVENT_CONNECTED | EXT_WIFI_EVENT_IP_READY ) );

    retCode = WIFI_GetRSSI( &rssi );
    TEST_ASSERT( retCode == eWiFiSuccess );
    configPRINTF( ( "RSSI =%d\n", rssi ) );

    retCode = WIFI_GetStatistic( &xStatistics );
    TEST_ASSERT( retCode == eWiFiSuccess );

    configPRINTF( ( "ulTxSuccessCount=%d, ulTxRetryCount=%d, ulTxFailCount=%d, ulRxSuccessCount=%d, ulRxCRCErrorCount=%d, ulMICErrorCount=%d, ",
                    xStatistics.ulTxSuccessCount, xStatistics.ulTxRetryCount, xStatistics.ulTxFailCount, xStatistics.ulRxSuccessCount,
                    xStatistics.ulRxCRCErrorCount, xStatistics.ulMICErrorCount ) );
    configPRINTF( ( "cNoise=%d, usPhyRate=%d, usTxRate=%d, usRxRate=%d, cRssi=%d, ucBandwidth=%d, ucIdleTimePer=%d\n",
                    xStatistics.cNoise, xStatistics.usPhyRate, xStatistics.usTxRate, xStatistics.usRxRate, xStatistics.cRssi,
                    xStatistics.ucBandwidth, xStatistics.ucIdleTimePer ) );

    retCode = WIFI_GetIPInfo( &ipConfig );
    TEST_ASSERT( retCode == eWiFiSuccess );

    if( ipConfig.xIPAddress.xType == eWiFiIPAddressTypeV4 ) /* IPv4 */
    {
        TEST_ASSERT_NOT_EQUAL( 0, ipConfig.xIPAddress.ulAddress[ 0 ] );
    }

    /* Try disconnect */
    xEventGroupClearBits( xExtWifiEventHandle, EXT_WIFI_EVENT_DISCONNECTED );

    retCode = WIFI_StartDisconnect();
    TEST_ASSERT( retCode == eWiFiSuccess );

    /* Wait for result */
    eventBits = xEventGroupWaitBits( xExtWifiEventHandle, EXT_WIFI_EVENT_DISCONNECTED, pdTRUE, pdTRUE, testwifiTASK_SYNC_TIMEOUT );
    configASSERT( eventBits == EXT_WIFI_EVENT_DISCONNECTED );

    /* Clean up */
    vEventGroupDelete( xExtWifiEventHandle );
    xExtWifiEventHandle = NULL;
}

/**
 * @brief Country code related task. This function will call WIFI_SetCountryCode, then it will call
 * WIFI_GetCountryCode to verify the result
 */
TEST( Full_WiFi, AFQP_CountryCode )
{
    char orgCountryCode[ 4 ], curCountryCode[ 4 ];
    const char * pcCountryCode = "US";
    WIFIReturnCode_t retCode;

    /* Read original value */
    retCode = WIFI_GetCountryCode( orgCountryCode );
    TEST_ASSERT( retCode == eWiFiSuccess );

    retCode = WIFI_SetCountryCode( pcCountryCode );
    TEST_ASSERT( retCode == eWiFiSuccess );

    retCode = WIFI_GetCountryCode( curCountryCode );
    TEST_ASSERT( retCode == eWiFiSuccess );
    TEST_ASSERT( strncmp( curCountryCode, pcCountryCode, sizeof( orgCountryCode ) ) == 0 );

    /* Try an invalid value */
    retCode = WIFI_SetCountryCode( "FAN" );
    TEST_ASSERT( retCode == eWiFiFailure );

    /* Restore back */
    retCode = WIFI_SetCountryCode( orgCountryCode );
    TEST_ASSERT( retCode == eWiFiSuccess );
}

/**
 * @brief Get capability related tast. This function will call AFQP_GetCapability
 */
TEST( Full_WiFi, AFQP_GetCapability )
{
    WIFIReturnCode_t retCode;
    WIFICapabilityInfo_t xCapabilities;

    retCode = WIFI_GetCapability( &xCapabilities );
    TEST_ASSERT( retCode == eWiFiSuccess );
    TEST_ASSERT( xCapabilities.xBand < eWiFiBandMax );
    TEST_ASSERT( xCapabilities.xPhyMode < eWiFiPhyMax );
    TEST_ASSERT( xCapabilities.xBandwidth < eWiFiBWMax );
}

/**
 * @brief Test to configure the Software Access Point and verify the return
 * status of the API.
 *
 * @note This test can only be verified manually. Set a breakpoint after
 * WIFI_ConfigureAPManual, which will block for some time. Connect to the Wi-Fi
 * network created by it on your phone and configure. You should expect it to
 * return eWiFiSuccess.
 */
TEST( Full_WiFi, AFQP_WiFiConfigureAPManual )
{
    WIFINetworkParams_t xNetworkParams = { 0 };
    WIFIReturnCode_t xWiFiStatus;
    WIFIStationInfo_t xStationList;
    uint8_t uStationListSize, loopTime;

    xNetworkParams.ucSSIDLength =
        strlen( wificonfigACCESS_POINT_SSID_PREFIX );
    memcpy( xNetworkParams.ucSSID, wificonfigACCESS_POINT_SSID_PREFIX, xNetworkParams.ucSSIDLength );

    xNetworkParams.xPassword.xWPA.ucLength = sizeof( wificonfigACCESS_POINT_PASSKEY );
    memcpy( xNetworkParams.xPassword.xWPA.cPassphrase, wificonfigACCESS_POINT_PASSKEY, xNetworkParams.xPassword.xWPA.ucLength );
    xNetworkParams.xSecurity = wificonfigACCESS_POINT_SECURITY;
    xNetworkParams.ucChannel = wificonfigACCESS_POINT_CHANNEL;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConfigureAP( &xNetworkParams );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }

    WIFI_StartAP();

    /* Wait for a client to connect */
    loopTime = 0;

    do
    {
        uStationListSize = 1;
        WIFI_GetStationList( &xStationList, &uStationListSize );
        vTaskDelay( pdMS_TO_TICKS( 1000 ) ); /* Try evey second, until 1 min is gone */
    } while( uStationListSize == 0 && ++loopTime < MAX_LOOP_COUNT );

    TEST_ASSERT( loopTime < MAX_LOOP_COUNT );
}
