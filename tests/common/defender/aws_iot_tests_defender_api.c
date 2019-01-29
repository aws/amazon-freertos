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

#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

#include <string.h>
#include "FreeRTOS.h"

#include "projdefs.h"
#include "task.h"

#include "aws_clientcredential.h"

/* Defender internal includes. */
#include "aws_iot_defender_internal.h"

#include "unity_fixture.h"

#include "FreeRTOS_POSIX/unistd.h"

#include "aws_iot_serializer.h"

/* Time interval to wait for a state to be true. */
#define _WAIT_STATE_INTERVAL_SECONDS          1

/* Total time to wait for a state to be true. */
#define _WAIT_STATE_TOTAL_SECONDS             5

/* Time interval for defender agent to publish metrics. It will be throttled if too frequent.
 */
#define _DEFENDER_PUBLISH_INTERVAL_SECONDS    20

/* Define a decoder based on chosen format. */
#if AWS_IOT_DEFENDER_FORMAT == AWS_IOT_DEFENDER_FORMAT_CBOR

    #define _Decoder    _AwsIotSerializerCborDecoder /**< Global defined in aws_iot_serializer.h . */

#elif AWS_IOT_DEFENDER_FORMAT == AWS_IOT_DEFENDER_FORMAT_JSON

    #define _Decoder    _AwsIotSerializerJsonDecoder /**< Global defined in aws_iot_serializer.h . */

#endif

#define _CALLBACK_PARAM_INITIALIZER    { 0 }

static const AwsIotDefenderCallback_t _EMPTY_CALLBACK = { .function = NULL, .param1 = NULL };

/*------------------ global variables -----------------------------*/

static AwsIotDefenderCallback_t _testCallback;

static AwsIotDefenderStartInfo_t _startInfo = AWS_IOT_DEFENDER_START_INFO_INITIALIZER;

static AwsIotDefenderCallbackInfo_t _callbackInfo =
{
    0,
    .eventType = -1 /* Initialize with an invalid event type. */
};

/*------------------ functions -----------------------------*/

/* Indicate this test doesn't actually publish report. */
static void _publishMetricsNotNeeded();

static void _waitForMetricsAccepted( uint32_t timeoutSec );

static void _verification();

static void _verifyAcceptedMessage( AwsIotDefenderCallbackInfo_t * const pCallbackInfo );

static void _verifyMetricsReport( AwsIotDefenderCallbackInfo_t * const pCallbackInfo );

static void _verifyTcpConections( AwsIotSerializerDecoderObject_t * pMetricsObject );

static void _verifyCallbackFunction( void * param1,
                                     AwsIotDefenderCallbackInfo_t * const pCallbackInfo );

static void _resetCalbackInfo();

TEST_GROUP( Full_OTA_DEFENDER );

TEST_SETUP( Full_OTA_DEFENDER )
{
    /* Reset test callback. */
    _testCallback = ( AwsIotDefenderCallback_t ) {
        .function = _verifyCallbackFunction, .param1 = NULL
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

TEST_TEAR_DOWN( Full_OTA_DEFENDER )
{
    AwsIotDefender_Stop();

    /* Actually get defender callback. */
    if( _callbackInfo.eventType != -1 )
    {
        sleep( _DEFENDER_PUBLISH_INTERVAL_SECONDS );
    }

    _resetCalbackInfo();
}

TEST_GROUP_RUNNER( Full_OTA_DEFENDER )
{
    /*
     * Setup: none
     * Action: call Start API with invliad IoT endpoint
     * Expectation: Start API returns network connection failure
     */
    RUN_TEST_CASE( Full_OTA_DEFENDER, Start_with_wrong_network_information );

    /*
     * Setup: defender not started yet
     * Action: call SetMetrics API with an invalid big integer as metrics group
     * Expectation:
     * - SetMetrics API return invalid input
     * - global metrics flag array are untouched
     */
    RUN_TEST_CASE( Full_OTA_DEFENDER, SetMetrics_with_invalid_metrics_group );

    /*
     * Setup: defender not started yet
     * Action: call SetMetrics API with Tcp connections group and "All Metrics" flag value
     * Expectation:
     * - SetMetrics API return success
     * - global metrics flag array are updated correctly
     */
    RUN_TEST_CASE( Full_OTA_DEFENDER, SetMetrics_with_TCP_connections_all );

    /*
     * Setup: defender is started
     * Action: call SetMetrics API with Tcp connections group and "All Metrics" flag value
     * Expectation:
     * - SetMetrics API return success
     * - global metrics flag array are updated correctly
     */
    RUN_TEST_CASE( Full_OTA_DEFENDER, SetMetrics_after_defender_started );

    /*
     * Setup: defender not started yet
     * Action: call SetPeriod API with small value less than 300
     * Expectation:
     * - SetPeriod API return "period too short" error
     */
    RUN_TEST_CASE( Full_OTA_DEFENDER, SetPeriod_too_short );

    /*
     * Setup: defender not started yet
     * Action: call SetPeriod API with 301
     * Expectation:
     * - SetPeriod API return success
     */
    RUN_TEST_CASE( Full_OTA_DEFENDER, SetPeriod_with_proper_value );

    /*
     * Setup: defender is started
     * Action: call SetPeriod API with 600
     * Expectation:
     * - SetPeriod API return success
     */
    RUN_TEST_CASE( Full_OTA_DEFENDER, SetPeriod_after_started );

    /*
     * Setup: kept from publishing metrics report
     * Action: call Start API with correct network information
     * Expectation: Start API return success
     */
    RUN_TEST_CASE( Full_OTA_DEFENDER, Start_should_return_success );

    /*
     * Setup: call Start API the first time; kept from publishing metrics report
     * Action: call Start API second time
     * Expectation: Start API return "already started" error
     */
    RUN_TEST_CASE( Full_OTA_DEFENDER, Start_should_return_err_if_already_started );

    /*
     * Setup: not set any metrics; register test callback
     * Action: call Start API
     * Expectation: metrics are accepted by defender service
     */
    RUN_TEST_CASE( Full_OTA_DEFENDER, Metrics_empty_are_published );

    /*
     * Setup: set "tcp connections" with "all metrics"; register test callback
     * Action: call Start API
     * Expectation:
     * - metrics are accepted by defender service
     * - verify metrics report has correct content
     */
    RUN_TEST_CASE( Full_OTA_DEFENDER, Metrics_TCP_connections_all_are_published );

    /*
     * Setup: set "tcp connections" with "total count"; register test callback
     * Action: call Start API
     * Expectation:
     * - metrics are accepted by defender service
     * - verify metrics report has correct content
     */
    RUN_TEST_CASE( Full_OTA_DEFENDER, Metrics_TCP_connections_total_are_published );

    /*
     * Setup: set "tcp connections" with "remote address"; register test callback
     * Action: call Start API
     * Expectation:
     * - metrics are accepted by defender service
     * - verify metrics report has correct content
     */
    RUN_TEST_CASE( Full_OTA_DEFENDER, Metrics_TCP_connections_remote_addr_are_published );

    /*
     * Setup: set "tcp connections" with "total count"; register test callback; call Start API
     * Action: call Stop API; set "tcp connections" with "all metrics"; call Start again
     * Expectation:
     * - metrics are accepted by defender service in both times
     * - verify metrics report has correct content respectively in both times
     */
    RUN_TEST_CASE( Full_OTA_DEFENDER, Restart_and_updated_metrics_are_published );
}

TEST( Full_OTA_DEFENDER, SetMetrics_with_invalid_metrics_group )
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

TEST( Full_OTA_DEFENDER, SetMetrics_with_TCP_connections_all )
{
    /* Set "all metrics" for TCP connections metrics group. */
    AwsIotDefenderError_t error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                                             AWS_IOT_DEFENDER_METRICS_ALL );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_METRICS_ALL, _AwsIotDefenderMetrics.metricsFlag[ AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS ] );
}

TEST( Full_OTA_DEFENDER, SetMetrics_after_defender_started )
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

TEST( Full_OTA_DEFENDER, Start_with_wrong_network_information )
{
    /* Given a dummy IoT endpoint to fail network connection. */
    _startInfo.pAwsIotEndpoint = "dummy endpoint";

    AwsIotDefenderError_t error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_CONNECTION_FAILURE, error );
}

TEST( Full_OTA_DEFENDER, Start_should_return_success )
{
    _publishMetricsNotNeeded();

    AwsIotDefenderError_t error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );
}

TEST( Full_OTA_DEFENDER, Start_should_return_err_if_already_started )
{
    _publishMetricsNotNeeded();

    AwsIotDefenderError_t error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Start defender for a second time. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_ALREADY_STARTED, error );
}

TEST( Full_OTA_DEFENDER, Metrics_empty_are_published )
{
    AwsIotDefenderError_t error;

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    /* Start defender. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    _waitForMetricsAccepted( _WAIT_STATE_TOTAL_SECONDS );

    _verification();
}

TEST( Full_OTA_DEFENDER, Metrics_TCP_connections_all_are_published )
{
    AwsIotDefenderError_t error;

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
    _waitForMetricsAccepted( _WAIT_STATE_TOTAL_SECONDS );

    _verification();
}

TEST( Full_OTA_DEFENDER, Metrics_TCP_connections_total_are_published )
{
    AwsIotDefenderError_t error;

    /* Set "total count" for TCP connections metrics group. */
    error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                       AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    /* Start defender. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Wait certain time for _reportAccepted to be true. */
    _waitForMetricsAccepted( _WAIT_STATE_TOTAL_SECONDS );

    _verification();
}

TEST( Full_OTA_DEFENDER, Metrics_TCP_connections_remote_addr_are_published )
{
    AwsIotDefenderError_t error;

    /* Set "remote address" for TCP connections metrics group. */
    error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                       AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_REMOTE_ADDR );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    /* Start defender. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Wait certain time for _reportAccepted to be true. */
    _waitForMetricsAccepted( _WAIT_STATE_TOTAL_SECONDS );

    _verification();
}

TEST( Full_OTA_DEFENDER, Restart_and_updated_metrics_are_published )
{
    /* Set "total count" for TCP connections metrics group. */
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS,
                       AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS, AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL ) );

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    /* Start defender. */
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, AwsIotDefender_Start( &_startInfo ) );

    /* Wait certain time for _reportAccepted to be true. */
    _waitForMetricsAccepted( _WAIT_STATE_TOTAL_SECONDS );

    _verification();

    AwsIotDefender_Stop();

    /* Reset _callbackInfo before restarting. */
    _resetCalbackInfo();

    sleep( _DEFENDER_PUBLISH_INTERVAL_SECONDS );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS,
                       AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS, AWS_IOT_DEFENDER_METRICS_ALL ) );

    /* Restart defender. */
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, AwsIotDefender_Start( &_startInfo ) );

    /* Wait certain time for _reportAccepted to be true. */
    _waitForMetricsAccepted( _WAIT_STATE_TOTAL_SECONDS );

    _verification();
}

TEST( Full_OTA_DEFENDER, SetPeriod_too_short )
{
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_PERIOD_TOO_SHORT, AwsIotDefender_SetPeriod( 299 ) );
}

TEST( Full_OTA_DEFENDER, SetPeriod_with_proper_value )
{
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, AwsIotDefender_SetPeriod( 301 ) );

    TEST_ASSERT_EQUAL( 301, AwsIotDefender_GetPeriod() );
}

TEST( Full_OTA_DEFENDER, SetPeriod_after_started )
{
    _publishMetricsNotNeeded();

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS,
                       AwsIotDefender_Start( &_startInfo ) );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, AwsIotDefender_SetPeriod( 600 ) );

    TEST_ASSERT_EQUAL( 600, AwsIotDefender_GetPeriod() );
}

/*-----------------------------------------------------------*/

static void _verifyCallbackFunction( void * param1,
                                     AwsIotDefenderCallbackInfo_t * const pCallbackInfo )
{
    /* Copy and malloc data from pCallbackInfo to _callbackInfo. */
    if( pCallbackInfo != NULL )
    {
        _callbackInfo.eventType = pCallbackInfo->eventType;
        _callbackInfo.metricsReportLength = pCallbackInfo->metricsReportLength;
        _callbackInfo.payloadLength = pCallbackInfo->payloadLength;

        if( _callbackInfo.payloadLength > 0 )
        {
            _callbackInfo.pPayload = AwsIotTest_Malloc( _callbackInfo.payloadLength );

            TEST_ASSERT_NOT_NULL( _callbackInfo.pPayload );

            memcpy( ( uint8_t * ) _callbackInfo.pPayload, pCallbackInfo->pPayload, _callbackInfo.payloadLength );
        }

        if( _callbackInfo.metricsReportLength > 0 )
        {
            _callbackInfo.pMetricsReport = AwsIotTest_Malloc( _callbackInfo.metricsReportLength );

            TEST_ASSERT_NOT_NULL( _callbackInfo.pMetricsReport );

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
    AwsIotTest_Free( ( uint8_t * ) _callbackInfo.pPayload );
    AwsIotTest_Free( ( uint8_t * ) _callbackInfo.pMetricsReport );

    _callbackInfo = ( AwsIotDefenderCallbackInfo_t ) {
        0,
        .eventType = -1
    };
}

/*-----------------------------------------------------------*/

static void _waitForMetricsAccepted( uint32_t timeoutSec )
{
    uint32_t maxIterations = timeoutSec / _WAIT_STATE_INTERVAL_SECONDS;
    uint32_t iter = 1;

    /* Wait for an valid event type to be set. */
    while( _callbackInfo.eventType != AWS_IOT_DEFENDER_METRICS_ACCEPTED )
    {
        if( iter > maxIterations )
        {
            /* Timeout. */
            break;
        }

        /* Event type is not set yet. */
        if( _callbackInfo.eventType == -1 )
        {
            sleep( _WAIT_STATE_INTERVAL_SECONDS );
        }
        /* An unexpected error event happened. */
        else
        {
            break;
        }
    }
}

/*-----------------------------------------------------------*/

static void _verification()
{
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_METRICS_ACCEPTED, _callbackInfo.eventType );

    _verifyAcceptedMessage( &_callbackInfo );
    _verifyMetricsReport( &_callbackInfo );
}

/*-----------------------------------------------------------*/

static void _verifyAcceptedMessage( AwsIotDefenderCallbackInfo_t * const pCallbackInfo )
{
    TEST_ASSERT_NOT_NULL( pCallbackInfo->pPayload );
    TEST_ASSERT_GREATER_THAN( 0, pCallbackInfo->payloadLength );

    AwsIotSerializerDecoderObject_t decoderObject = AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

    AwsIotSerializerError_t error = _Decoder.init( &decoderObject, pCallbackInfo->pPayload, pCallbackInfo->payloadLength );

    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_CONTAINER_MAP, decoderObject.type );

    AwsIotSerializerDecoderObject_t statusObject = AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

    char status[ 10 ] = "";
    statusObject.value.pString = ( uint8_t * ) status;
    statusObject.value.stringLength = 10;

    error = _Decoder.find( &decoderObject, "status", &statusObject );

    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING, statusObject.type );

    TEST_ASSERT_EQUAL( 0, strncmp( statusObject.value.pString, "ACCEPTED", statusObject.value.stringLength ) );
}

/*-----------------------------------------------------------*/

static void _verifyMetricsReport( AwsIotDefenderCallbackInfo_t * const pCallbackInfo )
{
    TEST_ASSERT_NOT_NULL( pCallbackInfo->pMetricsReport );
    TEST_ASSERT_GREATER_THAN( 0, pCallbackInfo->metricsReportLength );

    AwsIotSerializerDecoderObject_t decoderObject = AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

    AwsIotSerializerError_t error = _Decoder.init( &decoderObject, pCallbackInfo->pMetricsReport, pCallbackInfo->metricsReportLength );

    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_CONTAINER_MAP, decoderObject.type );

    AwsIotSerializerDecoderObject_t metricsObject = AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

    error = _Decoder.find( &decoderObject, "metrics", &metricsObject );

    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_CONTAINER_MAP, metricsObject.type );

    _verifyTcpConections( &metricsObject );
}

/*-----------------------------------------------------------*/

static void _verifyTcpConections( AwsIotSerializerDecoderObject_t * pMetricsObject )
{
    uint32_t tcpConnFlag = _AwsIotDefenderMetrics.metricsFlag[ AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS ];

    /* Assert find a "tcp_connections" map in "metrics" */
    AwsIotSerializerDecoderObject_t tcpConnObject = AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

    AwsIotSerializerError_t error = _Decoder.find( pMetricsObject, "tcp_connections", &tcpConnObject );

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

                TEST_ASSERT_EQUAL( 1, totalObject.value.signedInt );
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

                /* Assert find one "connection" map in "connections" */
                error = _Decoder.stepIn( &connsObject, &connIterator );

                TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

                AwsIotSerializerDecoderObject_t connMap = AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
                error = _Decoder.get( connIterator, &connMap );

                TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

                TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_CONTAINER_MAP, connMap.type );

                AwsIotSerializerDecoderObject_t remoteAddrObject = AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

                char remoteAddrStr[ 22 ] = "";
                remoteAddrObject.value.pString = ( uint8_t * ) remoteAddrStr;
                remoteAddrObject.value.stringLength = 22;

                error = _Decoder.find( &connMap, "remote_addr", &remoteAddrObject );

                if( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_REMOTE_ADDR )
                {
                    /* Assert find a "remote_addr" string in "connection" */
                    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

                    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING, remoteAddrObject.type );
                    /* TODO: verify content or IP and port. */
                }
                else
                {
                    /* Assert not found the "remote_addr". */
                    TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_NOT_FOUND, error );
                }

                error = _Decoder.next( connIterator );

                TEST_ASSERT_EQUAL( AWS_IOT_SERIALIZER_SUCCESS, error );

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
