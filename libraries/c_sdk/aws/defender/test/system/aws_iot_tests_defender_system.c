/*
 * FreeRTOS Defender V3.0.2
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

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "platform/iot_clock.h"

/* Platform metrics include. */
#include "platform/iot_metrics.h"
#include "iot_secure_sockets.h"

/* Platform network include. */
#include "platform/iot_network.h"

/* Defender internal includes. */
#include "private/aws_iot_defender_internal.h"

#include "aws_test_tcp.h"

#include "iot_init.h"

/* Test network header include. */
#include IOT_TEST_NETWORK_HEADER

/* Serializer includes. */
#include "iot_serializer.h"

#include "cbor.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "aws_test_utils.h"


/* Total time to wait for a state to be true. */
#define WAIT_STATE_TOTAL_SECONDS    10

/* Time interval for defender agent to publish metrics. It will be throttled if too frequent. */
/* TODO: if we can change "thingname" in each test, this can be lowered. */
#define DEFENDER_PUBLISH_INTERVAL_SECONDS    20

/* Estimated max size of message payload received in MQTT callback. */
#define PAYLOAD_MAX_SIZE                     200

/* Estimated max size of metrics report defender published. */
#define METRICS_MAX_SIZE                     200

/* Max size of address: IP + port. */
#define MAX_ADDRESS_LENGTH                   25

/* Use a big number to represent no event happened in defender. */
#define NO_EVENT                             255

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default values of test configuration constants.
 */
#ifndef IOT_TEST_MQTT_CONNECT_INIT_RETRY_DELAY_MS
    #define IOT_TEST_MQTT_CONNECT_INIT_RETRY_DELAY_MS    ( 1100 )
#endif
#ifndef IOT_TEST_MQTT_CONNECT_RETRY_COUNT
    #define IOT_TEST_MQTT_CONNECT_RETRY_COUNT            ( 6 )
#endif
#if IOT_TEST_MQTT_CONNECT_RETRY_COUNT < 1
    #error "IOT_TEST_MQTT_CONNECT_RETRY_COUNT must be at least 1."
#endif
/** @endcond */

static const uint32_t _ECHO_SERVER_IP = SOCKETS_inet_addr_quick( tcptestECHO_SERVER_ADDR0,
                                                                 tcptestECHO_SERVER_ADDR1,
                                                                 tcptestECHO_SERVER_ADDR2,
                                                                 tcptestECHO_SERVER_ADDR3 );

/* Empty callback structure passed to startInfo. */
static const AwsIotDefenderCallback_t _emptyCallback = { .function = NULL, .pCallbackContext = NULL };

static struct IotNetworkServerInfo _serverInfo = IOT_TEST_NETWORK_SERVER_INFO_INITIALIZER;
static struct IotNetworkCredentials _credential = IOT_TEST_NETWORK_CREDENTIALS_INITIALIZER;
static IotMqttConnection_t _mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

/*------------------ global variables -----------------------------*/

static uint8_t _payloadBuffer[ PAYLOAD_MAX_SIZE ];
static uint8_t _metricsBuffer[ METRICS_MAX_SIZE ];

static AwsIotDefenderCallback_t _testCallback;

static AwsIotDefenderStartInfo_t _startInfo = AWS_IOT_DEFENDER_START_INFO_INITIALIZER;

/*
 * Waiting for it indicates waiting for any event to happen
 * Posting it indicates any event happened
 */
static IotSemaphore_t _callbackInfoSem;

static AwsIotDefenderCallbackInfo_t _callbackInfo;

static IotSerializerDecoderObject_t _decoderObject;
static IotSerializerDecoderObject_t _metricsObject;

static bool _mqttConnectionStarted = false;
/*------------------ Functions -----------------------------*/

/* Copy data from MQTT callback to local buffer. */
static void _copyDataCallbackFunction( void * param1,
                                       AwsIotDefenderCallbackInfo_t * const pCallbackInfo );

static bool _waitForAnyEvent( uint32_t timeoutSec );

/* Wait for metrics to be accepted by defender service, for maximum timeout. */
static void _waitForMetricsAccepted( uint32_t timeoutSec );

/* Verify common section of metrics report. */
static void _verifyMetricsCommon( void );

/* Verify tcp connections in metrics report. */
static void _verifyTcpConnections( int total );

static void _resetCalbackInfo( void );

static IotMqttError_t _startMqttConnection( void );
static void _stopMqttConnection( void );

static Socket_t _createSocketToEchoServer();

TEST_GROUP( Defender_System );

TEST_SETUP( Defender_System )
{
    if( IotSdk_Init() == false )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize SDK." );
    }

    if( IotTestNetwork_Init() != IOT_NETWORK_SUCCESS )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize network stack." );
    }

    if( IotMqtt_Init() != IOT_MQTT_SUCCESS )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize MQTT." );
    }

    if( IotMetrics_Init() == false )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize metrics." );
    }

    /* Create a binary semaphore with initial value 0. */
    if( !IotSemaphore_Create( &_callbackInfoSem, 0, 1 ) )
    {
        TEST_FAIL_MESSAGE( "Fail to create semaphore for callback info." );
    }

    _resetCalbackInfo();

    _mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

    _decoderObject = ( IotSerializerDecoderObject_t ) IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
    _metricsObject = ( IotSerializerDecoderObject_t ) IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

    /* Reset test callback. */
    _testCallback = ( AwsIotDefenderCallback_t ) {
        .function = _copyDataCallbackFunction, .pCallbackContext = NULL
    };

    /* By default IOT_TEST_NETWORK_CREDENTIALS_INITIALIZER enables ALPN. ALPN
     * must be used with port 443; disable ALPN if another port is being used. */
    if( _serverInfo.port != 443 )
    {
        _credential.pAlpnProtos = NULL;
    }

    /* Reset server info. */
    _serverInfo = ( struct IotNetworkServerInfo ) IOT_TEST_NETWORK_SERVER_INFO_INITIALIZER;

    /* Set fields of start info. */
    _startInfo.pClientIdentifier = AWS_IOT_TEST_DEFENDER_THING_NAME;
    _startInfo.clientIdentifierLength = ( uint16_t ) strlen( AWS_IOT_TEST_DEFENDER_THING_NAME );
    _startInfo.callback = _emptyCallback;
}

TEST_TEAR_DOWN( Defender_System )
{
    AwsIotDefender_Stop();

    /* Actually get defender callback. */
    if( ( _callbackInfo.eventType == AWS_IOT_DEFENDER_METRICS_ACCEPTED ) ||
        ( _callbackInfo.eventType == AWS_IOT_DEFENDER_METRICS_REJECTED ) )
    {
        IotClock_SleepMs( DEFENDER_PUBLISH_INTERVAL_SECONDS * 1000 );
    }

    IotSemaphore_Destroy( &_callbackInfoSem );
    _stopMqttConnection();
    IotMetrics_Cleanup();
    IotMqtt_Cleanup();
    IotTestNetwork_Cleanup();
    IotSdk_Cleanup();
}

TEST_GROUP_RUNNER( Defender_System )
{
    /*
     * Setup: not set any metrics; register test callback
     * Action: call Start API
     * Expectation: metrics are accepted by defender service
     */
    RUN_TEST_CASE( Defender_System, Metrics_empty_are_published );

    /*
     * Setup: set "tcp connections" with "all metrics"; register test callback
     * Action: call Start API
     * Expectation:
     * - metrics are accepted by defender service
     * - verify metrics report has correct content
     */
    RUN_TEST_CASE( Defender_System, Metrics_TCP_connections_all_are_published );

    RUN_TEST_CASE( Defender_System, Metrics_TCP_connections_all_are_published_multiple_sockets );

    /*
     * Setup: set "tcp connections" with "total count"; register test callback
     * Action: call Start API
     * Expectation:
     * - metrics are accepted by defender service
     * - verify metrics report has correct content
     */
    RUN_TEST_CASE( Defender_System, Metrics_TCP_connections_total_are_published );

    /*
     * Setup: set "tcp connections" with "remote address"; register test callback
     * Action: call Start API
     * Expectation:
     * - metrics are accepted by defender service
     * - verify metrics report has correct content
     */
    RUN_TEST_CASE( Defender_System, Metrics_TCP_connections_remote_addr_are_published );

    /*
     * Setup: set "tcp connections" with "total count"; register test callback; call Start API
     * Action: call Stop API; set "tcp connections" with "all metrics"; call Start again
     * Expectation:
     * - metrics are accepted by defender service in both times
     * - verify metrics report has correct content respectively in both times
     */
    RUN_TEST_CASE( Defender_System, Restart_and_updated_metrics_are_published );
}


TEST( Defender_System, Metrics_empty_are_published )
{
    AwsIotDefenderError_t error = AWS_IOT_DEFENDER_SUCCESS;
    IotMqttError_t mqttError = IOT_MQTT_SUCCESS;

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    /* Start actual MQTT connection. */
    mqttError = _startMqttConnection();
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, mqttError );
    _startInfo.mqttConnection = _mqttConnection;

    /* Start defender. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    _waitForMetricsAccepted( WAIT_STATE_TOTAL_SECONDS );

    _verifyMetricsCommon();
    _verifyTcpConnections( 0 );
}

TEST( Defender_System, Metrics_TCP_connections_all_are_published )
{
    AwsIotDefenderError_t error = AWS_IOT_DEFENDER_SUCCESS;
    IotMqttError_t mqttError = IOT_MQTT_SUCCESS;

    /* start actual MQTT connection */
    mqttError = _startMqttConnection();
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, mqttError );
    _startInfo.mqttConnection = _mqttConnection;

    /* Set "all metrics" for TCP connections metrics group. */
    error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                       AWS_IOT_DEFENDER_METRICS_ALL );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    /* Start defender. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Wait certain time for _reportAccepted to be true. */
    _waitForMetricsAccepted( WAIT_STATE_TOTAL_SECONDS );

    _verifyMetricsCommon();
    _verifyTcpConnections( 1 );
}


TEST( Defender_System, Metrics_TCP_connections_all_are_published_multiple_sockets )
{
    AwsIotDefenderError_t error = AWS_IOT_DEFENDER_SUCCESS;
    IotMqttError_t mqttError = IOT_MQTT_SUCCESS;

    /* start actual MQTT connection */
    mqttError = _startMqttConnection();
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, mqttError );
    _startInfo.mqttConnection = _mqttConnection;

    Socket_t socket = _createSocketToEchoServer();

    if( TEST_PROTECT() )
    {
        /* Set "all metrics" for TCP connections metrics group. */
        error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                           AWS_IOT_DEFENDER_METRICS_ALL );
        TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );
        /* Set test callback to verify report. */
        _startInfo.callback = _testCallback;
        /* Start defender. */
        error = AwsIotDefender_Start( &_startInfo );
        TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );
        /* Wait certain time for _reportAccepted to be true. */
        _waitForMetricsAccepted( WAIT_STATE_TOTAL_SECONDS );
        _verifyMetricsCommon();
        _verifyTcpConnections( 2 );
    }

    SOCKETS_Shutdown( socket, SOCKETS_SHUT_RDWR );
    SOCKETS_Close( socket );
}

TEST( Defender_System, Metrics_TCP_connections_total_are_published )
{
    AwsIotDefenderError_t error = AWS_IOT_DEFENDER_SUCCESS;
    IotMqttError_t mqttError = IOT_MQTT_SUCCESS;

    /* Set "total count" for TCP connections metrics group. */
    error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                       AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* start actual MQTT connection */
    mqttError = _startMqttConnection();
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, mqttError );
    _startInfo.mqttConnection = _mqttConnection;

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    /* Start defender. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Wait certain time for _reportAccepted to be true. */
    _waitForMetricsAccepted( WAIT_STATE_TOTAL_SECONDS );

    _verifyMetricsCommon();
    _verifyTcpConnections( 1 );
}

TEST( Defender_System, Metrics_TCP_connections_remote_addr_are_published )
{
    AwsIotDefenderError_t error = AWS_IOT_DEFENDER_SUCCESS;
    IotMqttError_t mqttError = IOT_MQTT_SUCCESS;

    /* Set "remote address" for TCP connections metrics group. */
    error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                       AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_REMOTE_ADDR );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* start actual MQTT connection */
    mqttError = _startMqttConnection();
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, mqttError );
    _startInfo.mqttConnection = _mqttConnection;

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    /* Start defender. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Wait certain time for _reportAccepted to be true. */
    _waitForMetricsAccepted( WAIT_STATE_TOTAL_SECONDS );

    _verifyMetricsCommon();
    _verifyTcpConnections( 1 );
}

TEST( Defender_System, Restart_and_updated_metrics_are_published )
{
    IotMqttError_t mqttError = IOT_MQTT_SUCCESS;

    /* Set "total count" for TCP connections metrics group. */
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS,
                       AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS, AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL ) );

    /* start actual MQTT connection */
    mqttError = _startMqttConnection();
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, mqttError );
    _startInfo.mqttConnection = _mqttConnection;

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    /* Start defender. */
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, AwsIotDefender_Start( &_startInfo ) );

    /* Wait certain time for _reportAccepted to be true. */
    _waitForMetricsAccepted( WAIT_STATE_TOTAL_SECONDS );

    _verifyMetricsCommon();
    _verifyTcpConnections( 1 );

    AwsIotDefender_Stop();

    /* Reset _callbackInfo before restarting. */
    _resetCalbackInfo();

    IotClock_SleepMs( DEFENDER_PUBLISH_INTERVAL_SECONDS );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS,
                       AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS, AWS_IOT_DEFENDER_METRICS_ALL ) );

    /* Restart defender. */
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, AwsIotDefender_Start( &_startInfo ) );

    /* Wait certain time for _reportAccepted to be true. */
    _waitForMetricsAccepted( WAIT_STATE_TOTAL_SECONDS );

    _verifyMetricsCommon();
    _verifyTcpConnections( 1 );
}

/*-----------------------------------------------------------*/

static void _copyDataCallbackFunction( void * param1,
                                       AwsIotDefenderCallbackInfo_t * const pCallbackInfo )
{
    /* Silence the compiler. */
    ( void ) param1;

    /* Print out rejected message to stdout. The function used for printing
     * i.e. cbor_value_to_pretty is available only in the hosted environment.
     * The following #if guard is needed because some of the platforms
     * (eg. Marvell) pass -ffreestanding flag to the compiler which results in
     * __STDC_HOSTED__ being defined to 0 and therefore, cbor_value_to_pretty
     * not being available. */
    #if !defined( __STDC_HOSTED__ ) || __STDC_HOSTED__ - 0 == 1
        {
            if( pCallbackInfo->eventType == AWS_IOT_DEFENDER_METRICS_REJECTED )
            {
                CborParser cborParser;
                CborValue cborValue;
                cbor_parser_init( pCallbackInfo->pPayload, pCallbackInfo->payloadLength, 0, &cborParser, &cborValue );
                cbor_value_to_pretty( stdout, &cborValue );
            }
        }
    #endif /* __STDC_HOSTED__ check */

    /* Copy data from pCallbackInfo to _callbackInfo. */
    if( pCallbackInfo != NULL )
    {
        _callbackInfo.eventType = pCallbackInfo->eventType;
        _callbackInfo.metricsReportLength = pCallbackInfo->metricsReportLength;
        _callbackInfo.payloadLength = pCallbackInfo->payloadLength;

        if( _callbackInfo.payloadLength > 0 )
        {
            memcpy( ( uint8_t * ) _callbackInfo.pPayload, pCallbackInfo->pPayload, _callbackInfo.payloadLength );
        }

        if( _callbackInfo.metricsReportLength > 0 )
        {
            memcpy( ( uint8_t * ) _callbackInfo.pMetricsReport, pCallbackInfo->pMetricsReport, _callbackInfo.metricsReportLength );
        }
    }

    IotSemaphore_Post( &_callbackInfoSem );
}

/*-----------------------------------------------------------*/

static IotMqttError_t _startMqttConnection( void )
{
    IotMqttError_t mqttError = IOT_MQTT_SUCCESS;
    IotMqttNetworkInfo_t mqttNetworkInfo = ( IotMqttNetworkInfo_t ) IOT_MQTT_NETWORK_INFO_INITIALIZER;
    IotMqttConnectInfo_t mqttConnectionInfo = ( IotMqttConnectInfo_t ) IOT_MQTT_CONNECT_INFO_INITIALIZER;

    if( !_mqttConnectionStarted )
    {
        mqttNetworkInfo = ( IotMqttNetworkInfo_t ) IOT_MQTT_NETWORK_INFO_INITIALIZER;
        mqttNetworkInfo.createNetworkConnection = true;
        mqttNetworkInfo.u.setup.pNetworkServerInfo = &_serverInfo;
        mqttNetworkInfo.u.setup.pNetworkCredentialInfo = &_credential;

        mqttNetworkInfo.pNetworkInterface = IOT_TEST_NETWORK_INTERFACE;

        mqttConnectionInfo = ( IotMqttConnectInfo_t ) IOT_MQTT_CONNECT_INFO_INITIALIZER;

        /* Set MQTT connection information. */
        mqttConnectionInfo.pClientIdentifier = AWS_IOT_TEST_DEFENDER_THING_NAME;
        mqttConnectionInfo.clientIdentifierLength = ( uint16_t ) strlen( AWS_IOT_TEST_DEFENDER_THING_NAME );

        RETRY_EXPONENTIAL( mqttError = IotMqtt_Connect( &mqttNetworkInfo,
                                                        &mqttConnectionInfo,
                                                        1000,
                                                        &_mqttConnection ),
                           IOT_MQTT_SUCCESS,
                           IOT_TEST_MQTT_CONNECT_INIT_RETRY_DELAY_MS,
                           IOT_TEST_MQTT_CONNECT_RETRY_COUNT );

        if( mqttError == IOT_MQTT_SUCCESS )
        {
            _mqttConnectionStarted = true;
        }
    }

    return mqttError;
}

/*-----------------------------------------------------------*/

static void _stopMqttConnection( void )
{
    if( _mqttConnectionStarted )
    {
        IotMqtt_Disconnect( _mqttConnection, false );
        _mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;
        _mqttConnectionStarted = false;
    }
}

/*-----------------------------------------------------------*/

static void _resetCalbackInfo( void )
{
    /* Clean data buffer. */
    memset( _payloadBuffer, 0, PAYLOAD_MAX_SIZE );
    memset( _metricsBuffer, 0, METRICS_MAX_SIZE );

    /* Reset callback info. */
    _callbackInfo = ( AwsIotDefenderCallbackInfo_t ) {
        .pMetricsReport = _metricsBuffer,
        .metricsReportLength = 0,
        .pPayload = _payloadBuffer,
        .payloadLength = 0,
        .eventType = NO_EVENT
    };
}


/*-----------------------------------------------------------*/

static bool _waitForAnyEvent( uint32_t timeoutSec )
{
    return IotSemaphore_TimedWait( &_callbackInfoSem, timeoutSec * 1000 );
}

/*-----------------------------------------------------------*/

/* Assert the cause of rejection is throttle. */
static void _assertRejectDueToThrottle( void )
{
    TEST_ASSERT_NOT_NULL( _callbackInfo.pPayload );
    TEST_ASSERT_GREATER_THAN( 0, _callbackInfo.payloadLength );

    IotSerializerDecoderObject_t decoderObject = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
    IotSerializerDecoderObject_t statusDetailsObject = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
    IotSerializerDecoderObject_t errorCodeObject = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

    char errorCode[ 12 ] = "";

    IotSerializerError_t error = _pAwsIotDefenderDecoder->init( &decoderObject, _callbackInfo.pPayload, _callbackInfo.payloadLength );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, error );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_MAP, decoderObject.type );

    error = _pAwsIotDefenderDecoder->find( &decoderObject, "statusDetails", &statusDetailsObject );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, error );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_MAP, statusDetailsObject.type );

    errorCodeObject.u.value.u.string.pString = ( uint8_t * ) errorCode;
    errorCodeObject.u.value.u.string.length = 12;

    error = _pAwsIotDefenderDecoder->find( &statusDetailsObject, "ErrorCode", &errorCodeObject );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, error );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_TEXT_STRING, errorCodeObject.type );

    TEST_ASSERT_EQUAL( 0, strncmp( ( const char * ) errorCodeObject.u.value.u.string.pString, "Throttled", errorCodeObject.u.value.u.string.length ) );

    _pAwsIotDefenderDecoder->destroy( &statusDetailsObject );
    _pAwsIotDefenderDecoder->destroy( &decoderObject );
}

/*-----------------------------------------------------------*/

static void _waitForMetricsAccepted( uint32_t timeoutSec )
{
    /* If not event has occurred, simply fail the test. */
    if( !_waitForAnyEvent( timeoutSec ) )
    {
        TEST_FAIL_MESSAGE( "No event has occurred within timeout." );
    }

    if( _callbackInfo.eventType == AWS_IOT_DEFENDER_METRICS_REJECTED )
    {
        _assertRejectDueToThrottle();

        return;
    }

    /* Assert metrics is accepted. */
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_METRICS_ACCEPTED, _callbackInfo.eventType );

    TEST_ASSERT_NOT_NULL( _callbackInfo.pPayload );
    TEST_ASSERT_GREATER_THAN( 0, _callbackInfo.payloadLength );

    IotSerializerDecoderObject_t decoderObject = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

    IotSerializerError_t error = _pAwsIotDefenderDecoder->init( &decoderObject, _callbackInfo.pPayload, _callbackInfo.payloadLength );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, error );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_MAP, decoderObject.type );

    IotSerializerDecoderObject_t statusObject = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

    char status[ 10 ] = "";
    statusObject.u.value.u.string.pString = ( uint8_t * ) status;
    statusObject.u.value.u.string.length = 10;

    error = _pAwsIotDefenderDecoder->find( &decoderObject, "status", &statusObject );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, error );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_TEXT_STRING, statusObject.type );

    TEST_ASSERT_EQUAL( 0, strncmp( ( const char * ) statusObject.u.value.u.string.pString, "ACCEPTED", statusObject.u.value.u.string.length ) );

    _pAwsIotDefenderDecoder->destroy( &statusObject );
    _pAwsIotDefenderDecoder->destroy( &decoderObject );
}

/*-----------------------------------------------------------*/

static void _verifyMetricsCommon( void )
{
    TEST_ASSERT_NOT_NULL( _callbackInfo.pMetricsReport );
    TEST_ASSERT_GREATER_THAN( 0, _callbackInfo.metricsReportLength );

    IotSerializerError_t error = _pAwsIotDefenderDecoder->init( &_decoderObject, _callbackInfo.pMetricsReport, _callbackInfo.metricsReportLength );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, error );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_MAP, _decoderObject.type );

    error = _pAwsIotDefenderDecoder->find( &_decoderObject, "metrics", &_metricsObject );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, error );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_MAP, _metricsObject.type );
}

/*-----------------------------------------------------------*/

static void _verifyTcpConnections( int total )
{
    uint8_t i = 0;

    uint32_t tcpConnFlag = _AwsIotDefenderMetrics.metricsFlag[ AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS ];

    /* Assert find a "tcp_connections" map in "metrics" */
    IotSerializerDecoderObject_t tcpConnObject = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

    IotSerializerError_t error = _pAwsIotDefenderDecoder->find( &_metricsObject, "tcp_connections", &tcpConnObject );

    /* If any TCP connections flag is specified. */
    if( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_ALL )
    {
        /* Assert found the "tcp_connections" map. */
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, error );

        TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_MAP, tcpConnObject.type );

        IotSerializerDecoderObject_t estConnObject = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

        error = _pAwsIotDefenderDecoder->find( &tcpConnObject, "established_connections", &estConnObject );

        if( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED )
        {
            /* Assert found a "established_connections" map in "tcp_connections" */
            TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, error );

            TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_MAP, estConnObject.type );

            IotSerializerDecoderObject_t totalObject = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

            error = _pAwsIotDefenderDecoder->find( &estConnObject, "total", &totalObject );

            if( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL )
            {
                /* Assert find a "total" integer with value 1 in "established_connections" */
                TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, error );

                TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_SIGNED_INT, totalObject.type );

                TEST_ASSERT_EQUAL( total, totalObject.u.value.u.signedInt );
            }
            else
            {
                /* Assert not found the "total". */
                TEST_ASSERT_EQUAL( IOT_SERIALIZER_NOT_FOUND, error );
            }

            IotSerializerDecoderObject_t connsObject = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
            IotSerializerDecoderIterator_t connIterator = IOT_SERIALIZER_DECODER_ITERATOR_INITIALIZER;

            error = _pAwsIotDefenderDecoder->find( &estConnObject, "connections", &connsObject );

            if( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_CONNECTIONS )
            {
                /* Assert find a "connections" array in "established_connections" */
                TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, error );
                TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_ARRAY, connsObject.type );

                error = _pAwsIotDefenderDecoder->stepIn( &connsObject, &connIterator );
                TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, error );

                for( i = 0; i < total; i++ )
                {
                    /* Assert find one "connection" map in "connections" */
                    IotSerializerDecoderObject_t connMap = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
                    error = _pAwsIotDefenderDecoder->get( connIterator, &connMap );
                    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, error );
                    TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_MAP, connMap.type );

                    IotSerializerDecoderObject_t remoteAddrObject = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

                    error = _pAwsIotDefenderDecoder->find( &connMap, "remote_addr", &remoteAddrObject );

                    if( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_REMOTE_ADDR )
                    {
                        /* Assert find a "remote_addr" string in "connection" */
                        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, error );

                        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SCALAR_TEXT_STRING, remoteAddrObject.type );
                    }
                    else
                    {
                        /* Assert not found the "remote_addr". */
                        TEST_ASSERT_EQUAL( IOT_SERIALIZER_NOT_FOUND, error );
                    }

                    error = _pAwsIotDefenderDecoder->next( connIterator );
                    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS, error );

                    _pAwsIotDefenderDecoder->destroy( &connMap );
                }

                TEST_ASSERT_TRUE( _pAwsIotDefenderDecoder->isEndOfContainer( connIterator ) );

                _pAwsIotDefenderDecoder->stepOut( connIterator, &connsObject );
            }
            else
            {
                /* Assert not found the "connections". */
                TEST_ASSERT_EQUAL( IOT_SERIALIZER_NOT_FOUND, error );
            }

            _pAwsIotDefenderDecoder->destroy( &connsObject );
        }
        else
        {
            /* Assert not found the "established_connections" map. */
            TEST_ASSERT_EQUAL( IOT_SERIALIZER_NOT_FOUND, error );
        }

        _pAwsIotDefenderDecoder->destroy( &estConnObject );
    }
    else
    {
        /* Assert not found the "tcp_connections" map. */
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_NOT_FOUND, error );
    }

    _pAwsIotDefenderDecoder->destroy( &tcpConnObject );
    _pAwsIotDefenderDecoder->destroy( &_metricsObject );
    _pAwsIotDefenderDecoder->destroy( &_decoderObject );
}

/*-----------------------------------------------------------*/

static Socket_t _createSocketToEchoServer()
{
    const TickType_t xReceiveTimeOut = pdMS_TO_TICKS( 2000 );
    const TickType_t xSendTimeOut = pdMS_TO_TICKS( 2000 );

    Socket_t socket;
    SocketsSockaddr_t echoServerAddress;
    int32_t error = 0;

    echoServerAddress.usPort = SOCKETS_htons( tcptestECHO_PORT );
    echoServerAddress.ulAddress = _ECHO_SERVER_IP;

    socket = SOCKETS_Socket( SOCKETS_AF_INET, SOCKETS_SOCK_STREAM, SOCKETS_IPPROTO_TCP );
    TEST_ASSERT_NOT_EQUAL( SOCKETS_INVALID_SOCKET, socket );

    /* Set a time out so a missing reply does not cause the task to block indefinitely. */
    error = SOCKETS_SetSockOpt( socket, 0, SOCKETS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );
    TEST_ASSERT_EQUAL( SOCKETS_ERROR_NONE, error );
    error = SOCKETS_SetSockOpt( socket, 0, SOCKETS_SO_SNDTIMEO, &xSendTimeOut, sizeof( xSendTimeOut ) );
    TEST_ASSERT_EQUAL( SOCKETS_ERROR_NONE, error );

    error = SOCKETS_Connect( socket, &echoServerAddress, sizeof( echoServerAddress ) );
    TEST_ASSERT_EQUAL( SOCKETS_ERROR_NONE, error );

    return socket;
}
