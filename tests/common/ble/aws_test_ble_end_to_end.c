/*
 * Amazon FreeRTOS
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
 * @file aws_test_ble_end_to_end.c
 * @brief Tests for ble.
 */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* C standard library includes. */
#include <stdbool.h>

/* Network interface includes. */
#include "platform/iot_network_afr.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"

/*-----------------------------------------------------------*/

/**
 * @brief Declares and runs an MQTT system test.
 */
#define RUN_MQTT_SYSTEM_TEST( name )                    \
    extern void TEST_MQTT_System_ ## name ## _( void ); \
    TEST_MQTT_System_ ## name ## _();

/*-----------------------------------------------------------*/

/**
 * @brief Switches the network interface to use.
 */
static bool _useBle = false;

/*-----------------------------------------------------------*/

const IotNetworkInterface_t * IotTest_GetNetworkInterface( void )
{
    /* Default to using the sockets network interface. */
    const IotNetworkInterface_t * pNetworkInterface = IOT_NETWORK_INTERFACE_AFR;

    if( _useBle == true )
    {
        /* Use the BLE network interface. */
    }

    return pNetworkInterface;
}

/*-----------------------------------------------------------*/

TEST_GROUP( Full_BLE_END_TO_END );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_BLE_END_TO_END )
{
    /* Run test set up from MQTT system tests. */
    extern void TEST_MQTT_System_SETUP( void );
    TEST_MQTT_System_SETUP();
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_BLE_END_TO_END )
{
    /* Run test tear down from MQTT system tests. */
    extern void TEST_MQTT_System_TEAR_DOWN( void );
    TEST_MQTT_System_TEAR_DOWN();
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_BLE_END_TO_END )
{
    /* For these tests, use the BLE network interface. */
    _useBle = true;

    RUN_TEST_CASE( Full_BLE_END_TO_END, SubscribePublishWaitQoS1 );

    /* Revert to sockets network interface after this test is finished. */
    _useBle = false;
}

/*-----------------------------------------------------------*/

TEST( Full_BLE_END_TO_END, SubscribePublishWaitQoS1 )
{
    RUN_MQTT_SYSTEM_TEST( SubscribePublishWaitQoS1 );
}

/*-----------------------------------------------------------*/
