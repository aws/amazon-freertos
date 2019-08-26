/*
 * Amazon FreeRTOS BLE V2.0.0
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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file iot_test_ble_end_to_end.c
 * @brief Tests for ble.
 */
/* The config header is always included first. */
#include "iot_config.h"

/* C standard library includes. */
#include <stdbool.h>

/* Network interface includes. */
#include "platform/iot_network_ble.h"
#include "iot_mqtt.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"

/*-----------------------------------------------------------*/

extern void IotTestNetwork_SelectNetworkType( uint16_t networkType );

/**
 * @brief Declares and runs an MQTT system test.
 */
#define RUN_MQTT_SYSTEM_TEST( name )                    \
    extern void TEST_MQTT_System_ ## name ## _( void ); \
    TEST_MQTT_System_ ## name ## _();

/**
 * @brief Declares and runs shadow system test.
 */
#define RUN_SHADOW_SYSTEM_TEST( name )                    \
    extern void TEST_Shadow_System_ ## name ## _( void ); \
    TEST_Shadow_System_ ## name ## _();
/*-----------------------------------------------------------*/

TEST_GROUP( Full_BLE_END_TO_END_MQTT );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_BLE_END_TO_END_MQTT )
{
    /* Run test set up from MQTT system tests. */
    extern void TEST_MQTT_System_SETUP( void );

    TEST_MQTT_System_SETUP();
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_BLE_END_TO_END_MQTT )
{
    /* Run test tear down from MQTT system tests. */
    extern void TEST_MQTT_System_TEAR_DOWN( void );

    TEST_MQTT_System_TEAR_DOWN();
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_BLE_END_TO_END_MQTT )
{
    /* For these tests, use the BLE network interface. */
    IotTestNetwork_SelectNetworkType( AWSIOT_NETWORK_TYPE_BLE );

    RUN_TEST_CASE( Full_BLE_END_TO_END_MQTT, SubscribePublishWaitQoS0 );
    RUN_TEST_CASE( Full_BLE_END_TO_END_MQTT, SubscribePublishWaitQoS1 );
    RUN_TEST_CASE( Full_BLE_END_TO_END_MQTT, SubscribePublishAsync );

    /* Those 2 tests are disabled. At the moment BLE doesn't support
     * Persistant sessions nor Last Will and Testament.
     */
    /*RUN_TEST_CASE( Full_BLE_END_TO_END_MQTT, LastWillAndTestament );*/
    /*RUN_TEST_CASE( Full_BLE_END_TO_END_MQTT, RestorePreviousSession );*/

    RUN_TEST_CASE( Full_BLE_END_TO_END_MQTT, WaitAfterDisconnect );

    RUN_TEST_CASE( Full_BLE_END_TO_END_MQTT, SubscribeCompleteReentrancy );
    RUN_TEST_CASE( Full_BLE_END_TO_END_MQTT, IncomingPublishReentrancy );

    /* Revert to sockets network interface after this test is finished. */
    IotTestNetwork_SelectNetworkType( DEFAULT_NETWORK );
}


/*-----------------------------------------------------------*/


TEST( Full_BLE_END_TO_END_MQTT, SubscribePublishWaitQoS0 )
{
    RUN_MQTT_SYSTEM_TEST( SubscribePublishWaitQoS0 );
}

TEST( Full_BLE_END_TO_END_MQTT, SubscribePublishWaitQoS1 )
{
    RUN_MQTT_SYSTEM_TEST( SubscribePublishWaitQoS1 );
}

TEST( Full_BLE_END_TO_END_MQTT, SubscribePublishAsync )
{
    RUN_MQTT_SYSTEM_TEST( SubscribePublishAsync );
}

TEST( Full_BLE_END_TO_END_MQTT, LastWillAndTestament )
{
    RUN_MQTT_SYSTEM_TEST( LastWillAndTestament );
}

TEST( Full_BLE_END_TO_END_MQTT, RestorePreviousSession )
{
    RUN_MQTT_SYSTEM_TEST( RestorePreviousSession );
}

TEST( Full_BLE_END_TO_END_MQTT, WaitAfterDisconnect )
{
    RUN_MQTT_SYSTEM_TEST( WaitAfterDisconnect );
}

TEST( Full_BLE_END_TO_END_MQTT, SubscribeCompleteReentrancy )
{
    RUN_MQTT_SYSTEM_TEST( SubscribeCompleteReentrancy );
}

TEST( Full_BLE_END_TO_END_MQTT, IncomingPublishReentrancy )
{
    RUN_MQTT_SYSTEM_TEST( IncomingPublishReentrancy );
}



TEST_GROUP( Full_BLE_END_TO_END_SHADOW );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_BLE_END_TO_END_SHADOW )
{
    /* Run test set up from MQTT system tests. */
    extern void TEST_Shadow_System_SETUP( void );

    TEST_Shadow_System_SETUP();
}

/*-----------------------------------------------------------*/
TEST_TEAR_DOWN( Full_BLE_END_TO_END_SHADOW )
{
    /* Run test tear down from MQTT system tests. */
    extern void TEST_Shadow_System_TEAR_DOWN( void );

    TEST_Shadow_System_TEAR_DOWN();
}

TEST_GROUP_RUNNER( Full_BLE_END_TO_END_SHADOW )
{
    /* For these tests, use the BLE network interface. */
    IotTestNetwork_SelectNetworkType( AWSIOT_NETWORK_TYPE_BLE );

    RUN_TEST_CASE( Full_BLE_END_TO_END_SHADOW, UpdateGetDeleteAsyncQoS0 );
    RUN_TEST_CASE( Full_BLE_END_TO_END_SHADOW, UpdateGetDeleteAsyncQoS1 );
    RUN_TEST_CASE( Full_BLE_END_TO_END_SHADOW, UpdateGetDeleteBlockingQoS0 );
    RUN_TEST_CASE( Full_BLE_END_TO_END_SHADOW, UpdateGetDeleteBlockingQoS1 );
    RUN_TEST_CASE( Full_BLE_END_TO_END_SHADOW, DeltaCallback );
    RUN_TEST_CASE( Full_BLE_END_TO_END_SHADOW, UpdatedCallback );

    /* Revert to sockets network interface after this test is finished. */
    IotTestNetwork_SelectNetworkType( DEFAULT_NETWORK );
}

TEST( Full_BLE_END_TO_END_SHADOW, UpdateGetDeleteAsyncQoS0 )
{
    RUN_SHADOW_SYSTEM_TEST( UpdateGetDeleteAsyncQoS0 );
}

TEST( Full_BLE_END_TO_END_SHADOW, UpdateGetDeleteAsyncQoS1 )
{
    RUN_SHADOW_SYSTEM_TEST( UpdateGetDeleteAsyncQoS1 );
}

TEST( Full_BLE_END_TO_END_SHADOW, UpdateGetDeleteBlockingQoS0 )
{
    RUN_SHADOW_SYSTEM_TEST( UpdateGetDeleteBlockingQoS0 );
}

TEST( Full_BLE_END_TO_END_SHADOW, UpdateGetDeleteBlockingQoS1 )
{
    RUN_SHADOW_SYSTEM_TEST( UpdateGetDeleteBlockingQoS1 );
}

TEST( Full_BLE_END_TO_END_SHADOW, DeltaCallback )
{
    RUN_SHADOW_SYSTEM_TEST( DeltaCallback );
}

TEST( Full_BLE_END_TO_END_SHADOW, UpdatedCallback )
{
    RUN_SHADOW_SYSTEM_TEST( UpdatedCallback );
}


/*-----------------------------------------------------------*/
