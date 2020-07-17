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

/* Defender internal includes. */
#include "private/aws_iot_defender_internal.h"

/* MQTT Mock include */
#include "iot_tests_mqtt_mock.h"

/* Platform metrics include. */
#include "platform/iot_metrics.h"

/* Platform network include. */
#include "platform/iot_network.h"

#include "iot_init.h"
#include "unity_fixture.h"

/* Thing Name must be defined for these tests. */
#ifndef AWS_IOT_TEST_DEFENDER_THING_NAME
    #error "Please define AWS_IOT_TEST_DEFENDER_THING_NAME."
#endif

/**
 * @brief The length of @ref AWS_IOT_TEST_DEFENDER_THING_NAME.
 */
#define THING_NAME_LENGTH                                 ( sizeof( AWS_IOT_TEST_DEFENDER_THING_NAME ) - 1 )

/**
 * @brief Default Invalid Metrics Group.
 * Used by the SetMetrics_with_invalid_metrics_group unit test.
 */
#define AWS_IOT_DEFENDER_DEFAULT_INVALID_METRICS_GROUP    ( 10 )

/* Empty callback structure passed to startInfo. */
static const AwsIotDefenderCallback_t _emptyCallback = { .function = NULL, .pCallbackContext = NULL };

static IotMqttConnection_t _mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

/*------------------ global variables -----------------------------*/

static AwsIotDefenderStartInfo_t _startInfo = AWS_IOT_DEFENDER_START_INFO_INITIALIZER;

static bool _mockedMqttConnection = false;
/*------------------ Functions -----------------------------*/

TEST_GROUP( Defender_Unit );

TEST_SETUP( Defender_Unit )
{
    if( IotSdk_Init() == false )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize SDK." );
    }

    if( IotMqtt_Init() != IOT_MQTT_SUCCESS )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize MQTT." );
    }

    if( IotMetrics_Init() == false )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize metrics." );
    }

    /* Set fields of start info. */
    _startInfo.pClientIdentifier = AWS_IOT_TEST_DEFENDER_THING_NAME;
    _startInfo.clientIdentifierLength = ( uint16_t ) strlen( AWS_IOT_TEST_DEFENDER_THING_NAME );
    _startInfo.callback = _emptyCallback;
}

TEST_TEAR_DOWN( Defender_Unit )
{
    AwsIotDefender_Stop();

    if( _mockedMqttConnection )
    {
        IotTest_MqttMockCleanup();
        _mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;
        _mockedMqttConnection = false;
    }

    IotMetrics_Cleanup();
    IotMqtt_Cleanup();
    IotSdk_Cleanup();
}

TEST_GROUP_RUNNER( Defender_Unit )
{
    /*
     * Setup: none
     * Action: call Start API with invalid MQTT connection
     * Expectation: Start API returns failure
     */
    RUN_TEST_CASE( Defender_Unit, Start_with_invalid_mqtt_connection );

    /*
     * Setup: defender not started yet
     * Action: call SetMetrics API with an invalid big integer as metrics group
     * Expectation:
     * - SetMetrics API return invalid input
     * - global metrics flag array are untouched
     */
    RUN_TEST_CASE( Defender_Unit, SetMetrics_with_invalid_metrics_group );

    /*
     * Setup: defender not started yet
     * Action: call SetMetrics API with Tcp connections group and "All Metrics" flag value
     * Expectation:
     * - SetMetrics API return success
     * - global metrics flag array are updated correctly
     */
    RUN_TEST_CASE( Defender_Unit, SetMetrics_with_TCP_connections_all );

    /*
     * Setup: defender is started
     * Action: call SetMetrics API with Tcp connections group and "All Metrics" flag value
     * Expectation:
     * - SetMetrics API return success
     * - global metrics flag array are updated correctly
     */
    RUN_TEST_CASE( Defender_Unit, SetMetrics_after_defender_started );

    /*
     * Setup: defender not started yet
     * Action: call SetPeriod API with small value less than AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS
     * Expectation:
     * - SetPeriod API return "period too short" error
     */
    RUN_TEST_CASE( Defender_Unit, SetPeriod_too_short );

    /*
     * Setup: defender not started yet
     * Action: call SetPeriod API with AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS + 1
     * Expectation:
     * - SetPeriod API return success
     */
    RUN_TEST_CASE( Defender_Unit, SetPeriod_with_proper_value );

    /*
     * Setup: defender is started
     * Action: call SetPeriod API with 2 * AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS
     * Expectation:
     * - SetPeriod API return success
     */
    RUN_TEST_CASE( Defender_Unit, SetPeriod_after_started );

    /*
     * Setup: kept from publishing metrics report
     * Action: call Start API with correct network information
     * Expectation: Start API return success
     */
    RUN_TEST_CASE( Defender_Unit, Start_should_return_success );

    /*
     * Setup: call Start API the first time; kept from publishing metrics report
     * Action: call Start API second time
     * Expectation: Start API return "already started" error
     */
    RUN_TEST_CASE( Defender_Unit, Start_should_return_err_if_already_started );
}

TEST( Defender_Unit, SetMetrics_with_invalid_metrics_group )
{
    uint8_t i = 0;

    /* Input a dummy, invalid metrics group. */
    AwsIotDefenderError_t error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_DEFAULT_INVALID_METRICS_GROUP,
                                                             AWS_IOT_DEFENDER_METRICS_ALL );

    /* SetMetrics should return "invalid input". */
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_INVALID_INPUT, error );

    /* Assert metrics flag in each metrics group remains 0. */
    for( i = 0; i < DEFENDER_METRICS_GROUP_COUNT; i++ )
    {
        TEST_ASSERT_EQUAL( 0, _AwsIotDefenderMetrics.metricsFlag[ i ] );
    }
}

TEST( Defender_Unit, SetMetrics_with_TCP_connections_all )
{
    /* Set "all metrics" for TCP connections metrics group. */
    AwsIotDefenderError_t error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                                             AWS_IOT_DEFENDER_METRICS_ALL );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_METRICS_ALL, _AwsIotDefenderMetrics.metricsFlag[ AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS ] );
}

TEST( Defender_Unit, SetMetrics_after_defender_started )
{
    /* Set up a mocked MQTT connection. */
    TEST_ASSERT_EQUAL_INT( true, IotTest_MqttMockInit( &_mqttConnection ) );
    _mockedMqttConnection = true;
    _startInfo.mqttConnection = _mqttConnection;

    AwsIotDefenderError_t error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Set "all metrics" for TCP connections metrics group. */
    error = AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
                                       AWS_IOT_DEFENDER_METRICS_ALL );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_METRICS_ALL, _AwsIotDefenderMetrics.metricsFlag[ AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS ] );
}

TEST( Defender_Unit, Start_with_invalid_mqtt_connection )
{
    /* Set uninitialized MQTT connection */
    _startInfo.mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

    AwsIotDefenderError_t error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_INVALID_INPUT, error );
}

TEST( Defender_Unit, Start_should_return_success )
{
    /* Set up a mocked MQTT connection. */
    TEST_ASSERT_EQUAL_INT( true, IotTest_MqttMockInit( &_mqttConnection ) );
    _mockedMqttConnection = true;
    _startInfo.mqttConnection = _mqttConnection;

    AwsIotDefenderError_t error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );
}

TEST( Defender_Unit, Start_should_return_err_if_already_started )
{
    /* Set up a mocked MQTT connection. */
    TEST_ASSERT_EQUAL_INT( true, IotTest_MqttMockInit( &_mqttConnection ) );
    _mockedMqttConnection = true;
    _startInfo.mqttConnection = _mqttConnection;

    AwsIotDefenderError_t error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, error );

    /* Start defender for a second time. */
    error = AwsIotDefender_Start( &_startInfo );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_ALREADY_STARTED, error );
}

TEST( Defender_Unit, SetPeriod_too_short )
{
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_PERIOD_TOO_SHORT, AwsIotDefender_SetPeriod( AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS - 1 ) );
}

TEST( Defender_Unit, SetPeriod_with_proper_value )
{
    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, AwsIotDefender_SetPeriod( AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS + 1 ) );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS + 1, AwsIotDefender_GetPeriod() );
}

TEST( Defender_Unit, SetPeriod_after_started )
{
    /* Set up a mocked MQTT connection. */
    TEST_ASSERT_EQUAL_INT( true, IotTest_MqttMockInit( &_mqttConnection ) );
    _mockedMqttConnection = true;
    _startInfo.mqttConnection = _mqttConnection;

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS,
                       AwsIotDefender_Start( &_startInfo ) );

    TEST_ASSERT_EQUAL( AWS_IOT_DEFENDER_SUCCESS, AwsIotDefender_SetPeriod( 2 * AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS ) );

    TEST_ASSERT_EQUAL( 2 * AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS, AwsIotDefender_GetPeriod() );
}
