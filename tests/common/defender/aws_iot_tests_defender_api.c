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

#include "unity_fixture.h"

#include "FreeRTOS_POSIX/unistd.h"

#include "aws_iot_serializer.h"

#define _WAIT_STATE_INTERVAL_SECONDS          1

#define _DEFENDER_PUBLISH_INTERVAL_SECONDS    8

#if AWS_IOT_DEFENDER_FORMAT == AWS_IOT_DEFENDER_FORMAT_CBOR

    #define _Decoder    _AwsIotSerializerCborDecoder /**< Global defined in aws_iot_serializer.h . */

#elif AWS_IOT_DEFENDER_FORMAT == AWS_IOT_DEFENDER_FORMAT_JSON

    #define _Decoder    _AwsIotSerializerJsonDecoder /**< Global defined in aws_iot_serializer.h . */

#endif

#define _WAIT_TIME_SECONDS             5

#define _CALLBACK_PARAM_INITIALIZER    { 0 }

static const AwsIotDefenderCallback_t _EMPTY_CALLBACK = { .function = NULL, .param1 = NULL };

typedef struct _testCallbackParam
{
    uint32_t metricsFlag[ _DEFENDER_METRICS_GROUP_COUNT ];
} _testCallbackParam_t;

static AwsIotDefenderCallback_t _testCallback;

static AwsIotDefenderStartInfo_t _startInfo = AWS_IOT_DEFENDER_START_INFO_INITIALIZER;

static bool _reportAccepted;
static bool _reportRejected;

static bool _defenderStarted;

static void _verifyCallbackFunction( void * param1,
                                     AwsIotDefenderCallbackInfo_t * const pCallbackInfo );

/* Wait a state to be true for certain timeout. Return true if the wait succeeds. */
static bool _waitForState( bool * pState,
                           uint32_t timeoutSec );

static void _verifyAcceptedMessage( AwsIotDefenderCallbackInfo_t * const pCallbackInfo );

static void _verifyMetricsReport( AwsIotDefenderCallbackInfo_t * const pCallbackInfo );

static void _verifyTcpConections( AwsIotSerializerDecoderObject_t * pMetricsObject );

/* Indicate this test doesn't actually publish report. */
static void _publishMetricsNotNeeded();

TEST_GROUP( Full_DEFENDER_API );

TEST_SETUP( Full_DEFENDER_API )
{
    _reportAccepted = false;
    _reportRejected = false;

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

TEST_TEAR_DOWN( Full_DEFENDER_API )
{
    AwsIotDefender_Stop();

    if( _reportAccepted || _reportRejected )
    {
        sleep( _DEFENDER_PUBLISH_INTERVAL_SECONDS );
    }
}

TEST_GROUP_RUNNER( Full_DEFENDER_API )
{
    /* These tests do not require any network connectivity. */

    RUN_TEST_CASE( Full_DEFENDER_API, Stop_should_return_err_when_not_started );

    RUN_TEST_CASE( Full_DEFENDER_API, SetMetrics_with_invalid_metrics_group );
    RUN_TEST_CASE( Full_DEFENDER_API, SetMetrics_with_TCP_connections_all );

    /* These tests do not publish metrics report. */

    RUN_TEST_CASE( Full_DEFENDER_API, Start_should_return_success );
    RUN_TEST_CASE( Full_DEFENDER_API, Start_should_return_err_if_already_started );

    RUN_TEST_CASE( Full_DEFENDER_API, Stop_should_return_success_when_started );

    /* This tests that the agent successfully reports to the service */
    RUN_TEST_CASE( Full_DEFENDER_API, Metrics_empty_are_published );
    RUN_TEST_CASE( Full_DEFENDER_API, Metrics_TCP_connections_all_are_published );
    RUN_TEST_CASE( Full_DEFENDER_API, Metrics_TCP_connections_total_are_published );
    RUN_TEST_CASE( Full_DEFENDER_API, Metrics_TCP_connections_remote_addr_are_published );
}

TEST( Full_DEFENDER_API, SetMetrics_with_invalid_metrics_group )
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

TEST( Full_DEFENDER_API, SetMetrics_with_TCP_connections_all )
{
    /* Set "all metrics" for TCP connections metrics group. */
    AwsIotDefenderError_t error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                                             AWS_IOT_DEFENDER_METRICS_ALL );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_METRICS_ALL, _AwsIotDefenderMetrics.metricsFlag[ AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS ] );
}

TEST( Full_DEFENDER_API, Start_should_return_success )
{
    _publishMetricsNotNeeded();

    AwsIotDefenderError_t error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );
}

TEST( Full_DEFENDER_API, Start_should_return_err_if_already_started )
{
    _publishMetricsNotNeeded();

    AwsIotDefenderError_t error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Start defender for a second time. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_ALREADY_STARTED, error );
}

TEST( Full_DEFENDER_API, Stop_should_return_success_when_started )
{
    _publishMetricsNotNeeded();

    AwsIotDefenderError_t error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    error = AwsIotDefender_Stop();

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );
}

TEST( Full_DEFENDER_API, Stop_should_return_err_when_not_started )
{
    AwsIotDefenderError_t error = AwsIotDefender_Stop();

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_NOT_STARTED, error );
}

TEST( Full_DEFENDER_API, Metrics_empty_are_published )
{
    AwsIotDefenderError_t error;

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    /* Start defender. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Wait certain time for _reportAccepted to be true. */
    TEST_ASSERT_TRUE( _waitForState( &_reportAccepted, _WAIT_TIME_SECONDS ) );
}

TEST( Full_DEFENDER_API, Metrics_TCP_connections_all_are_published )
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
    TEST_ASSERT_TRUE( _waitForState( &_reportAccepted, _WAIT_TIME_SECONDS ) );
}

TEST( Full_DEFENDER_API, Metrics_TCP_connections_total_are_published )
{
    AwsIotDefenderError_t error;

    /* Set "all metrics" for TCP connections metrics group. */
    error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                       AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    /* Start defender. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Wait certain time for _reportAccepted to be true. */
    TEST_ASSERT_TRUE( _waitForState( &_reportAccepted, _WAIT_TIME_SECONDS ) );
}

TEST( Full_DEFENDER_API, Metrics_TCP_connections_remote_addr_are_published )
{
    AwsIotDefenderError_t error;

    /* Set "all metrics" for TCP connections metrics group. */
    error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                       AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_REMOTE_ADDR );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Set test callback to verify report. */
    _startInfo.callback = _testCallback;

    /* Start defender. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Wait certain time for _reportAccepted to be true. */
    TEST_ASSERT_TRUE( _waitForState( &_reportAccepted, _WAIT_TIME_SECONDS ) );
}

/*-----------------------------------------------------------*/

static void _verifyCallbackFunction( void * param1,
                                     AwsIotDefenderCallbackInfo_t * const pCallbackInfo )
{
    TEST_ASSERT_NOT_NULL( pCallbackInfo );

    _reportRejected = pCallbackInfo->eventType == AWS_IOT_DEFENDER_METRICS_REJECTED;

    if( pCallbackInfo->eventType == AWS_IOT_DEFENDER_METRICS_ACCEPTED )
    {
        _verifyAcceptedMessage( pCallbackInfo );
        _verifyMetricsReport( pCallbackInfo );

        _reportAccepted = true;
    }
}

/*-----------------------------------------------------------*/

static void _publishMetricsNotNeeded()
{
    _startInfo.pThingName = "dummy-thing-for-test";
    _startInfo.thingNameLength = ( uint16_t ) strlen( "dummy-thing-for-test" );
}

/*-----------------------------------------------------------*/

static bool _waitForState( bool * pState,
                           uint32_t timeoutSec )
{
    uint32_t totalTime = 0;

    while( !( *pState ) )
    {
        if( totalTime >= timeoutSec )
        {
            return false;
        }

        totalTime += _WAIT_STATE_INTERVAL_SECONDS;
        sleep( _WAIT_STATE_INTERVAL_SECONDS );
    }

    return true;
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

    TEST_ASSERT_EQUAL( 0, strcmp( statusObject.value.pString, "ACCEPTED" ) );

    /*AwsIotSerializerDecoderObject_t thingNameObject = AWS_IOT_SERIALIZER_DECODER_INITIALIZER;
     *
     *  char thingName[10] = "";
     *  thingNameObject.value.pString = (uint8_t *)thingName;
     *  thingNameObject.value.stringLength = 10;
     *
     *  error = _Decoder.find(&decoderObject, "status", &statusObject);
     *
     *  TEST_ASSERT_EQUAL(AWS_IOT_SERIALIZER_SUCCESS, error);
     *
     *  TEST_ASSERT_EQUAL(AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING, statusObject.type);
     *
     *  TEST_ASSERT_EQUAL(0, strcmp(statusObject.value.pString, clientcredentialIOT_THING_NAME));*/
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
