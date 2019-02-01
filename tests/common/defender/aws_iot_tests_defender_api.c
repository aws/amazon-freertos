/*
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 */

#include <string.h>
#include "FreeRTOS.h"

#include "projdefs.h"
#include "task.h"

#include "aws_clientcredential.h"

/* Defender internal includes. */
#include "aws_iot_defender_internal.h"

#include "aws_secure_sockets.h"

#include "unity_fixture.h"

#include "FreeRTOS_POSIX/unistd.h"

#include "aws_iot_serializer.h"

/* Time interval to wait for a state to be true. */
#define _WAIT_STATE_INTERVAL_SECONDS    1

/* Total time to wait for a state to be true. */
#define _WAIT_STATE_TOTAL_SECONDS       5

/* Time interval for defender agent to publish metrics. It will be throttled if too frequent. */
/* TODO: if we can change "thingname" in each test, this can be lowered. */
#define _DEFENDER_PUBLISH_INTERVAL_SECONDS    30

/* Estimated max size of message payload received in MQTT callback. */
#define _PAYLOAD_MAX_SIZE                     100

/* Estimated max size of metrics report defender published. */
#define _METRICS_MAX_SIZE                     200

/* Max size of address: IP + port. */
#define _MAX_ADDRESS_LENGTH                   25

/* Define a decoder based on chosen format. */
#if AWS_IOT_DEFENDER_FORMAT == AWS_IOT_DEFENDER_FORMAT_CBOR

    #define _Decoder    _AwsIotSerializerCborDecoder /**< Global defined in aws_iot_serializer.h . */

#elif AWS_IOT_DEFENDER_FORMAT == AWS_IOT_DEFENDER_FORMAT_JSON

    #define _Decoder    _AwsIotSerializerJsonDecoder /**< Global defined in aws_iot_serializer.h . */

#endif

static const uint32_t _ECHO_SERVER_IP = SOCKETS_inet_addr_quick( configECHO_SERVER_ADDR0,
                                                                 configECHO_SERVER_ADDR1,
                                                                 configECHO_SERVER_ADDR2,
                                                                 configECHO_SERVER_ADDR3 );

static char _ECHO_SERVER_ADDRESS[ _MAX_ADDRESS_LENGTH ];

static const AwsIotDefenderCallback_t _EMPTY_CALLBACK = { .function = NULL, .param1 = NULL };

/*------------------ global variables -----------------------------*/

static uint8_t _payloadBuffer[ _PAYLOAD_MAX_SIZE ];
static uint8_t _metricsBuffer[ _METRICS_MAX_SIZE ];

static AwsIotDefenderCallback_t _testCallback;

static AwsIotDefenderStartInfo_t _startInfo = AWS_IOT_DEFENDER_START_INFO_INITIALIZER;

static AwsIotDefenderCallbackInfo_t _callbackInfo;

static AwsIotSerializerDecoderObject_t _decoderObject;
static AwsIotSerializerDecoderObject_t _metricsObject;

/*------------------ Functions -----------------------------*/

/* Copy data from MQTT callback to local buffer. */
static void _copyDataCallbackFunction( void * param1,
                                       AwsIotDefenderCallbackInfo_t * const pCallbackInfo );

/* Wait for metrics to be accepted by defender service, for maxinum timeout. */
static void _waitForMetricsAccepted( uint32_t timeoutSec );

/* Verify common section of metrics report. */
static void _verifyMetricsCommon();

/* Verify tcp connections in metrics report. */
static void _verifyTcpConections( int total,
                                  ... );

/* Indicate this test doesn't actually publish report. */
static void _publishMetricsNotNeeded();

static void _resetCalbackInfo();

static char * _getIotAddress();

static Socket_t _createSocketToEchoServer();

TEST_GROUP( Full_DEFENDER );

TEST_SETUP( Full_DEFENDER )
{
    SOCKETS_inet_ntoa( _ECHO_SERVER_IP, _ECHO_SERVER_ADDRESS );
    sprintf( _ECHO_SERVER_ADDRESS, "%s:%d", _ECHO_SERVER_ADDRESS, configTCP_ECHO_CLIENT_PORT );

    _resetCalbackInfo();

    _decoderObject = ( AwsIotSerializerDecoderObject_t ) AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
    _metricsObject = ( AwsIotSerializerDecoderObject_t ) AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

    /* Reset test callback. */
    _testCallback = ( AwsIotDefenderCallback_t ) {
        .function = _copyDataCallbackFunction, .param1 = NULL
    };

    /* Setup startInfo. */
    _startInfo.pAwsIotEndpoint = clientcredentialMQTT_BROKER_ENDPOINT;
    _startInfo.port = clientcredentialMQTT_BROKER_PORT;
    _startInfo.pThingName = clientcredentialIOT_THING_NAME;
    _startInfo.thingNameLength = ( uint16_t ) strlen( clientcredentialIOT_THING_NAME );
    _startInfo.callback = _EMPTY_CALLBACK;

    /* Setup TLS information. */
    _startInfo.tlsInfo = ( AwsIotNetworkTlsInfo_t ) AWS_IOT_NETWORK_TLS_INFO_INITIALIZER;

    /* Use the default root CA provided with Amazon FreeRTOS. */
    _startInfo.tlsInfo.pRootCa = NULL;
    _startInfo.tlsInfo.rootCaLength = 0;

    /* Set client credentials. */
    _startInfo.tlsInfo.pClientCert = clientcredentialCLIENT_CERTIFICATE_PEM;
    _startInfo.tlsInfo.clientCertLength = ( size_t ) clientcredentialCLIENT_CERTIFICATE_LENGTH;
    _startInfo.tlsInfo.pPrivateKey = clientcredentialCLIENT_PRIVATE_KEY_PEM;
    _startInfo.tlsInfo.privateKeyLength = ( size_t ) clientcredentialCLIENT_PRIVATE_KEY_LENGTH;

    /* If not connecting over port 443, disable ALPN. */
    if( clientcredentialMQTT_BROKER_PORT != 443 )
    {
        _startInfo.tlsInfo.pAlpnProtos = NULL;
    }
}

TEST_TEAR_DOWN( Full_DEFENDER )
{
    AwsIotDefender_Stop();

    /* Actually get defender callback. */
    if( _callbackInfo.eventType != -1 )
    {
        sleep( _DEFENDER_PUBLISH_INTERVAL_SECONDS );
    }
}

TEST_GROUP_RUNNER( Full_DEFENDER )
{
    /*
     * Setup: none
     * Action: call Start API with invliad IoT endpoint
     * Expectation: Start API returns network connection failure
     */
    RUN_TEST_CASE( Full_DEFENDER, Start_with_wrong_network_information );

    /*
     * Setup: defender not started yet
     * Action: call SetMetrics API with an invalid big integer as metrics group
     * Expectation:
     * - SetMetrics API return invalid input
     * - global metrics flag array are untouched
     */
    RUN_TEST_CASE( Full_DEFENDER, SetMetrics_with_invalid_metrics_group );

    /*
     * Setup: defender not started yet
     * Action: call SetMetrics API with Tcp connections group and "All Metrics" flag value
     * Expectation:
     * - SetMetrics API return success
     * - global metrics flag array are updated correctly
     */
    RUN_TEST_CASE( Full_DEFENDER, SetMetrics_with_TCP_connections_all );

    /*
     * Setup: defender is started
     * Action: call SetMetrics API with Tcp connections group and "All Metrics" flag value
     * Expectation:
     * - SetMetrics API return success
     * - global metrics flag array are updated correctly
     */
    RUN_TEST_CASE( Full_DEFENDER, SetMetrics_after_defender_started );

    /*
     * Setup: defender not started yet
     * Action: call SetPeriod API with small value less than 300
     * Expectation:
     * - SetPeriod API return "period too short" error
     */
    RUN_TEST_CASE( Full_DEFENDER, SetPeriod_too_short );

    /*
     * Setup: defender not started yet
     * Action: call SetPeriod API with 301
     * Expectation:
     * - SetPeriod API return success
     */
    RUN_TEST_CASE( Full_DEFENDER, SetPeriod_with_proper_value );

    /*
     * Setup: defender is started
     * Action: call SetPeriod API with 600
     * Expectation:
     * - SetPeriod API return success
     */
    RUN_TEST_CASE( Full_DEFENDER, SetPeriod_after_started );

    /*
     * Setup: kept from publishing metrics report
     * Action: call Start API with correct network information
     * Expectation: Start API return success
     */
    RUN_TEST_CASE( Full_DEFENDER, Start_should_return_success );

    /*
     * Setup: call Start API the first time; kept from publishing metrics report
     * Action: call Start API second time
     * Expectation: Start API return "already started" error
     */
    RUN_TEST_CASE( Full_DEFENDER, Start_should_return_err_if_already_started );

    /*
     * Setup: not set any metrics; register test callback
     * Action: call Start API
     * Expectation: metrics are accepted by defender service
     */
    RUN_TEST_CASE( Full_DEFENDER, Metrics_empty_are_published );

    /*
     * Setup: set "tcp connections" with "all metrics"; register test callback
     * Action: call Start API
     * Expectation:
     * - metrics are accepted by defender service
     * - verify metrics report has correct content
     */
    RUN_TEST_CASE( Full_DEFENDER, Metrics_TCP_connections_all_are_published );

    RUN_TEST_CASE( Full_DEFENDER, Metrics_TCP_connections_all_are_published_multiple_sockets );

    /*
     * Setup: set "tcp connections" with "total count"; register test callback
     * Action: call Start API
     * Expectation:
     * - metrics are accepted by defender service
     * - verify metrics report has correct content
     */
    RUN_TEST_CASE( Full_DEFENDER, Metrics_TCP_connections_total_are_published );

    /*
     * Setup: set "tcp connections" with "remote address"; register test callback
     * Action: call Start API
     * Expectation:
     * - metrics are accepted by defender service
     * - verify metrics report has correct content
     */
    RUN_TEST_CASE( Full_DEFENDER, Metrics_TCP_connections_remote_addr_are_published );

    /*
     * Setup: set "tcp connections" with "total count"; register test callback; call Start API
     * Action: call Stop API; set "tcp connections" with "all metrics"; call Start again
     * Expectation:
     * - metrics are accepted by defender service in both times
     * - verify metrics report has correct content respectively in both times
     */
    RUN_TEST_CASE( Full_DEFENDER, Restart_and_updated_metrics_are_published );
}

TEST( Full_DEFENDER, SetMetrics_with_invalid_metrics_group )
{
    /* Input a dummy, invalid metrics group. */
    AwsIotDefenderError_t error = AwsIotDefender_SetMetrics( 10000,
                                                             AWS_IOT_DEFENDER_METRICS_ALL );

    /* SetMetrics should return "invalid input". */
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_INVALID_INPUT, error );

    /* Assert metrics flag in each metrics group remains 0. */
    for( uint8_t i = 0; i < _DEFENDER_METRICS_GROUP_COUNT; i++ )
    {
        TEST_ASSERT_EQUAL( 0, _AwsIotDefenderMetrics.metricsFlag[ i ] );
    }
}

TEST( Full_DEFENDER, SetMetrics_with_TCP_connections_all )
{
    /* Set "all metrics" for TCP connections metrics group. */
    AwsIotDefenderError_t error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                                             AWS_IOT_DEFENDER_METRICS_ALL );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_METRICS_ALL, _AwsIotDefenderMetrics.metricsFlag[ AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS ] );
}

TEST( Full_DEFENDER, SetMetrics_after_defender_started )
{
    _publishMetricsNotNeeded();

    AwsIotDefenderError_t error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Set "all metrics" for TCP connections metrics group. */
    error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                       AWS_IOT_DEFENDER_METRICS_ALL );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_METRICS_ALL, _AwsIotDefenderMetrics.metricsFlag[ AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS ] );
}

TEST( Full_DEFENDER, Start_with_wrong_network_information )
{
    /* Given a dummy IoT endpoint to fail network connection. */
    _startInfo.pAwsIotEndpoint = "dummy endpoint";

    AwsIotDefenderError_t error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_CONNECTION_FAILURE, error );
}

TEST( Full_DEFENDER, Start_should_return_success )
{
    _publishMetricsNotNeeded();

    AwsIotDefenderError_t error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );
}

TEST( Full_DEFENDER, Start_should_return_err_if_already_started )
{
    _publishMetricsNotNeeded();

    AwsIotDefenderError_t error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Start defender for a second time. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_ALREADY_STARTED, error );
}

TEST( Full_DEFENDER, Metrics_empty_are_published )
{
    AwsIotDefenderError_t error;

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    /* Start defender. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    _waitForMetricsAccepted( _WAIT_STATE_TOTAL_SECONDS );

    _verifyMetricsCommon();
    _verifyTcpConections( 0 );
}

TEST( Full_DEFENDER, Metrics_TCP_connections_all_are_published )
{
    AwsIotDefenderError_t error;

    /* Set "all metrics" for TCP connections metrics group. */
    error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                       AWS_IOT_DEFENDER_METRICS_ALL );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    /* Get Iot address from DNS. */
    char * pIotAddress = _getIotAddress();

    /* Start defender. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Wait certain time for _reportAccepted to be true. */
    _waitForMetricsAccepted( _WAIT_STATE_TOTAL_SECONDS );

    _verifyMetricsCommon();
    _verifyTcpConections( 1, pIotAddress );
}

TEST( Full_DEFENDER, Metrics_TCP_connections_all_are_published_multiple_sockets )
{
    AwsIotDefenderError_t error;

    Socket_t socket = _createSocketToEchoServer();

    if( TEST_PROTECT() )
    {
        /* Set "all metrics" for TCP connections metrics group. */
        error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                           AWS_IOT_DEFENDER_METRICS_ALL );

        TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

        /* Set test callback to verify report. */
        _startInfo.callback = _testCallback;

        /* Get Iot address from DNS. */
        char * pIotAddress = _getIotAddress();

        /* Start defender. */
        error = AwsIotDefender_Start( &_startInfo );

        TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

        /* Wait certain time for _reportAccepted to be true. */
        _waitForMetricsAccepted( _WAIT_STATE_TOTAL_SECONDS );

        _verifyMetricsCommon();
        _verifyTcpConections( 2, _ECHO_SERVER_ADDRESS, pIotAddress );
    }

    SOCKETS_Shutdown( socket, SOCKETS_SHUT_RDWR );
    SOCKETS_Close( socket );
}

TEST( Full_DEFENDER, Metrics_TCP_connections_total_are_published )
{
    AwsIotDefenderError_t error;

    /* Set "total count" for TCP connections metrics group. */
    error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                       AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    /* Get Iot address from DNS. */
    char * pIotAddress = _getIotAddress();

    /* Start defender. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Wait certain time for _reportAccepted to be true. */
    _waitForMetricsAccepted( _WAIT_STATE_TOTAL_SECONDS );

    _verifyMetricsCommon();
    _verifyTcpConections( 1, pIotAddress );
}

TEST( Full_DEFENDER, Metrics_TCP_connections_remote_addr_are_published )
{
    AwsIotDefenderError_t error;

    /* Set "remote address" for TCP connections metrics group. */
    error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                       AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_REMOTE_ADDR );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    /* Get Iot address from DNS. */
    char * pIotAddress = _getIotAddress();

    /* Start defender. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Wait certain time for _reportAccepted to be true. */
    _waitForMetricsAccepted( _WAIT_STATE_TOTAL_SECONDS );

    _verifyMetricsCommon();
    _verifyTcpConections( 1, pIotAddress );
}

TEST( Full_DEFENDER, Restart_and_updated_metrics_are_published )
{
    char * pIotAddress = NULL;

    /* Set "total count" for TCP connections metrics group. */
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS,
                       AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS, AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL ) );

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    pIotAddress = _getIotAddress();

    /* Start defender. */
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, AwsIotDefender_Start( &_startInfo ) );

    /* Wait certain time for _reportAccepted to be true. */
    _waitForMetricsAccepted( _WAIT_STATE_TOTAL_SECONDS );

    _verifyMetricsCommon();
    _verifyTcpConections( 1, pIotAddress );

    AwsIotDefender_Stop();

    /* Reset _callbackInfo before restarting. */
    _resetCalbackInfo();

    sleep( _DEFENDER_PUBLISH_INTERVAL_SECONDS );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS,
                       AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS, AWS_IOT_DEFENDER_METRICS_ALL ) );

    pIotAddress = _getIotAddress();

    /* Restart defender. */
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, AwsIotDefender_Start( &_startInfo ) );

    /* Wait certain time for _reportAccepted to be true. */
    _waitForMetricsAccepted( _WAIT_STATE_TOTAL_SECONDS );

    _verifyMetricsCommon();
    _verifyTcpConections( 1, pIotAddress );
}

TEST( Full_DEFENDER, SetPeriod_too_short )
{
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_PERIOD_TOO_SHORT, AwsIotDefender_SetPeriod( 299 ) );
}

TEST( Full_DEFENDER, SetPeriod_with_proper_value )
{
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, AwsIotDefender_SetPeriod( 301 ) );

    TEST_ASSERT_EQUAL( 301, AwsIotDefender_GetPeriod() );
}

TEST( Full_DEFENDER, SetPeriod_after_started )
{
    _publishMetricsNotNeeded();

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS,
                       AwsIotDefender_Start( &_startInfo ) );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, AwsIotDefender_SetPeriod( 600 ) );

    TEST_ASSERT_EQUAL( 600, AwsIotDefender_GetPeriod() );
}

/*-----------------------------------------------------------*/

static void _copyDataCallbackFunction( void * param1,
                                       AwsIotDefenderCallbackInfo_t * const pCallbackInfo )
{
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
}

/*-----------------------------------------------------------*/

static void _publishMetricsNotNeeded()
{
    _startInfo.pThingName = "dummy-thing-for-test";
    _startInfo.thingNameLength = ( uint16_t ) strlen( "dummy-thing-for-test" );
}

/*-----------------------------------------------------------*/

static void _resetCalbackInfo()
{
    /* Clean data buffer. */
    memset( _payloadBuffer, 0, _PAYLOAD_MAX_SIZE );
    memset( _metricsBuffer, 0, _METRICS_MAX_SIZE );

    /* Reset callback info. */
    _callbackInfo = ( AwsIotDefenderCallbackInfo_t ) {
        .pMetricsReport = _metricsBuffer,
        .metricsReportLength = 0,
        .pPayload = _payloadBuffer,
        .payloadLength = 0,
        .eventType = -1 /* Initialize to -1 to indicate there is no event. */
    };
}

/*-----------------------------------------------------------*/

static void _waitForMetricsAccepted( uint32_t timeoutSec )
{
    uint32_t maxIterations = timeoutSec / _WAIT_STATE_INTERVAL_SECONDS;
    uint32_t iter = 1;

    /* Wait for an event type to be set. */
    while( _callbackInfo.eventType == -1 )
    {
        if( iter > maxIterations )
        {
            /* Timeout. */
            TEST_FAIL_MESSAGE( "Metrics are still not accepted after max timeout." );
        }

        sleep( _WAIT_STATE_INTERVAL_SECONDS );

        iter++;
    }

    /* Assert metrics is accepted. */
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_METRICS_ACCEPTED, _callbackInfo.eventType );

    TEST_ASSERT_NOT_NULL( _callbackInfo.pPayload );
    TEST_ASSERT_GREATER_THAN( 0, _callbackInfo.payloadLength );

    AwsIotSerializerDecoderObject_t decoderObject = AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

    AwsIotSerializerError_t error = _Decoder.init( &decoderObject, _callbackInfo.pPayload, _callbackInfo.payloadLength );

    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_CONTAINER_MAP, decoderObject.type );

    AwsIotSerializerDecoderObject_t statusObject = AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

    char status[ 10 ] = "";
    statusObject.value.pString = ( uint8_t * ) status;
    statusObject.value.stringLength = 10;

    error = _Decoder.find( &decoderObject, "status", &statusObject );

    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING, statusObject.type );

    TEST_ASSERT_EQUAL( 0, strncmp( ( const char * ) statusObject.value.pString, "ACCEPTED", statusObject.value.stringLength ) );
}

/*-----------------------------------------------------------*/

static void _verifyMetricsCommon()
{
    TEST_ASSERT_NOT_NULL( _callbackInfo.pMetricsReport );
    TEST_ASSERT_GREATER_THAN( 0, _callbackInfo.metricsReportLength );

    AwsIotSerializerError_t error = _Decoder.init( &_decoderObject, _callbackInfo.pMetricsReport, _callbackInfo.metricsReportLength );

    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_CONTAINER_MAP, _decoderObject.type );

    error = _Decoder.find( &_decoderObject, "metrics", &_metricsObject );

    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_CONTAINER_MAP, _metricsObject.type );
}

/*-----------------------------------------------------------*/

static void _verifyTcpConections( int total,
                                  ... )
{
    uint32_t tcpConnFlag = _AwsIotDefenderMetrics.metricsFlag[ AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS ];

    /* Assert find a "tcp_connections" map in "metrics" */
    AwsIotSerializerDecoderObject_t tcpConnObject = AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

    AwsIotSerializerError_t error = _Decoder.find( &_metricsObject, "tcp_connections", &tcpConnObject );

    /* If any TCP connections flag is specified. */
    if( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_ALL )
    {
        /* Assert found the "tcp_connections" map. */
        TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

        TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_CONTAINER_MAP, tcpConnObject.type );

        AwsIotSerializerDecoderObject_t estConnObject = AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

        error = _Decoder.find( &tcpConnObject, "established_connections", &estConnObject );

        if( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED )
        {
            /* Assert found a "established_connections" map in "tcp_connections" */
            TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

            TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_CONTAINER_MAP, estConnObject.type );

            AwsIotSerializerDecoderObject_t totalObject = AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

            error = _Decoder.find( &estConnObject, "total", &totalObject );

            if( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL )
            {
                /* Assert find a "total" integer with value 1 in "established_connections" */
                TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

                TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT, totalObject.type );

                TEST_ASSERT_EQUAL( total, totalObject.value.signedInt );
            }
            else
            {
                /* Assert not found the "total". */
                TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_NOT_FOUND, error );
            }

            AwsIotSerializerDecoderObject_t connsObject = AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
            AwsIotSerializerDecoderIterator_t connIterator = AWS_IOT_SERIALIZER_DECODER_ITERATOR_INITIALIZER;

            error = _Decoder.find( &estConnObject, "connections", &connsObject );

            if( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_CONNECTIONS )
            {
                /* Assert find a "connections" array in "established_connections" */
                TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );
                TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_CONTAINER_ARRAY, connsObject.type );

                error = _Decoder.stepIn( &connsObject, &connIterator );
                TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

                /* Create argument list for expected remote addresses. */
                va_list valist;
                va_start( valist, total );

                for( uint8_t i = 0; i < total; i++ )
                {
                    /* Assert find one "connection" map in "connections" */
                    AwsIotSerializerDecoderObject_t connMap = AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
                    error = _Decoder.get( connIterator, &connMap );
                    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );
                    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_CONTAINER_MAP, connMap.type );

                    AwsIotSerializerDecoderObject_t remoteAddrObject = AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

                    error = _Decoder.find( &connMap, "remote_addr", &remoteAddrObject );

                    if( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_REMOTE_ADDR )
                    {
                        /* Assert find a "remote_addr" string in "connection" */
                        TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

                        TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING, remoteAddrObject.type );

                        /* Verify the passed address matching. */
                        TEST_ASSERT_EQUAL_STRING_LEN( va_arg( valist, char * ),
                                                      remoteAddrObject.value.pString,
                                                      remoteAddrObject.value.stringLength );
                    }
                    else
                    {
                        /* Assert not found the "remote_addr". */
                        TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_NOT_FOUND, error );
                    }

                    error = _Decoder.next( connIterator );
                    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );
                }

                va_end( valist );

                TEST_ASSERT_TRUE( _Decoder.isEndOfContainer( connIterator ) );
            }
            else
            {
                /* Assert not found the "connections". */
                TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_NOT_FOUND, error );
            }
        }
        else
        {
            /* Assert not found the "established_connections" map. */
            TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_NOT_FOUND, error );
        }
    }
    else
    {
        /* Assert not found the "tcp_connections" map. */
        TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_NOT_FOUND, error );
    }
}

/*-----------------------------------------------------------*/

static char * _getIotAddress()
{
    uint32_t ip = SOCKETS_GetHostByName( clientcredentialMQTT_BROKER_ENDPOINT );
    static char address[ _MAX_ADDRESS_LENGTH ];

    SOCKETS_inet_ntoa( ip, address );
    sprintf( address, "%s:%d", address, clientcredentialMQTT_BROKER_PORT );

    return address;
}

/*-----------------------------------------------------------*/

static Socket_t _createSocketToEchoServer()
{
    static const TickType_t xReceiveTimeOut = pdMS_TO_TICKS( 2000 );
    static const TickType_t xSendTimeOut = pdMS_TO_TICKS( 2000 );

    Socket_t socket;
    SocketsSockaddr_t echoServerAddress;
    int32_t error = 0;

    /* Echo requests are sent to the echo server.  The address of the echo
     * server is configured by the constants configECHO_SERVER_ADDR0 to
     * configECHO_SERVER_ADDR3 in FreeRTOSConfig.h. */
    echoServerAddress.usPort = SOCKETS_htons( configTCP_ECHO_CLIENT_PORT );
    echoServerAddress.ulAddress = _ECHO_SERVER_IP;

    socket = SOCKETS_Socket( SOCKETS_AF_INET, SOCKETS_SOCK_STREAM, SOCKETS_IPPROTO_TCP );
    TEST_ASSERT_NOT_EQUAL( SOCKETS_INVALID_SOCKET, socket );

    /* Set a time out so a missing reply does not cause the task to block indefinitely. */
    SOCKETS_SetSockOpt( socket, 0, SOCKETS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );
    SOCKETS_SetSockOpt( socket, 0, SOCKETS_SO_SNDTIMEO, &xSendTimeOut, sizeof( xSendTimeOut ) );

    error = SOCKETS_Connect( socket, &echoServerAddress, sizeof( echoServerAddress ) );
    TEST_ASSERT_EQUAL( 0, error );

    return socket;
}
