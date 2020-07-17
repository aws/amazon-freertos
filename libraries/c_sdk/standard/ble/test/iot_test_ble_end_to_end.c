/*
 * FreeRTOS BLE V2.1.0
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

/**
 * @file iot_test_ble_end_to_end.c
 * @brief Tests for ble.
 */

#include <string.h>

/* The config header is always included first. */
#include "iot_config.h"

/* C standard library includes. */
#include <stdbool.h>

#include "iot_ble.h"
/* Network interface includes. */
#include "platform/iot_network_ble.h"
#include "iot_mqtt.h"
#include "platform/iot_threads.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"


#define BLE_TEST_CONNECTION_TIMEOUT_MS    ( 15000 )

/*-----------------------------------------------------------*/

extern bool IotTestNetwork_SelectNetworkType( uint16_t networkType );
static bool bBLEInitialized = pdFALSE;
static bool bBLEEnabled = false;
static bool bBLEConnected = false;

static IotSemaphore_t bleConnectionSem;


/**
 * @brief Declares and runs an MQTT system test.
 */
#define RUN_MQTT_SYSTEM_TEST( name )                               \
    extern void TEST_MQTT_System_ ## name ## _( void );            \
    TEST_ASSERT_MESSAGE( bBLEInitialized, "BLE Not initialized" ); \
    TEST_MQTT_System_ ## name ## _();

/**
 * @brief Declares and runs shadow system test.
 */
#define RUN_SHADOW_SYSTEM_TEST( name )                             \
    extern void TEST_Shadow_System_ ## name ## _( void );          \
    TEST_ASSERT_MESSAGE( bBLEInitialized, "BLE Not initialized" ); \
    TEST_Shadow_System_ ## name ## _();
/*-----------------------------------------------------------*/

BTStatus_t bleStackInit( void );

static BTStatus_t _BLEInit( void )
{
    BTStatus_t status = eBTStatusSuccess;

    /** Only do one time initialization of stack and Middleware **/
    if( !bBLEInitialized )
    {
        status = bleStackInit();

        TEST_ASSERT_EQUAL( eBTStatusSuccess, status );

        status = IotBle_Init();

        TEST_ASSERT_EQUAL( eBTStatusSuccess, status );

        bBLEInitialized = true;
    }

    return status;
}

static void _BLEConnectionCallback( BTStatus_t status,
                                    uint16_t connId,
                                    bool connected,
                                    BTBdaddr_t * pBa )
{
    if( connected == true )
    {
        IotBle_StopAdv( NULL );
    }
    else
    {
        ( void ) IotBle_StartAdv( NULL );
    }

    bBLEConnected = connected;
    IotSemaphore_Post( &bleConnectionSem );
}


static BTStatus_t _BLEEnable( void )
{
    IotBleEventsCallbacks_t eventCb;
    BTStatus_t status = eBTStatusSuccess;

    if( !bBLEEnabled )
    {
        TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &bleConnectionSem, 0, 1 ) );

        eventCb.pConnectionCb = _BLEConnectionCallback;

        status = IotBle_RegisterEventCb( eBLEConnection, eventCb );

        TEST_ASSERT_EQUAL( eBTStatusSuccess, status );

        status = IotBle_On();

        TEST_ASSERT_EQUAL( eBTStatusSuccess, status );

        TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &bleConnectionSem, BLE_TEST_CONNECTION_TIMEOUT_MS ) );

        TEST_ASSERT_TRUE( bBLEConnected );

        bBLEEnabled = true;
    }

    return status;
}


static BTStatus_t _BLEDisable( void )
{
    BTStatus_t status;
    IotBleEventsCallbacks_t eventCb;

    if( bBLEEnabled )
    {
        status = IotBle_Off();

        TEST_ASSERT_EQUAL( eBTStatusSuccess, status );

        TEST_ASSERT_TRUE( IotSemaphore_TimedWait( &bleConnectionSem, BLE_TEST_CONNECTION_TIMEOUT_MS ) );

        TEST_ASSERT_FALSE( bBLEConnected );


        eventCb.pConnectionCb = _BLEConnectionCallback;
        status = IotBle_UnRegisterEventCb( eBLEConnection, eventCb );

        TEST_ASSERT_EQUAL( eBTStatusSuccess, status );

        IotSemaphore_Destroy( &bleConnectionSem );

        bBLEEnabled = false;
    }

    return status;
}

TEST_GROUP( Full_BLE_END_TO_END_CONNECTIVITY );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_BLE_END_TO_END_CONNECTIVITY )
{
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_BLE_END_TO_END_CONNECTIVITY )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_BLE_END_TO_END_CONNECTIVITY )
{
    RUN_TEST_CASE( Full_BLE_END_TO_END_CONNECTIVITY, InitializeBLE );
    RUN_TEST_CASE( Full_BLE_END_TO_END_CONNECTIVITY, SetDeviceName );
    RUN_TEST_CASE( Full_BLE_END_TO_END_CONNECTIVITY, EnableBLE );
}



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

    _BLEInit();
    _BLEEnable();

    bBLEInitialized = IotTestNetwork_SelectNetworkType( AWSIOT_NETWORK_TYPE_BLE );

    RUN_TEST_CASE( Full_BLE_END_TO_END_MQTT, SubscribePublishWaitQoS0 );
    RUN_TEST_CASE( Full_BLE_END_TO_END_MQTT, SubscribePublishWaitQoS1 );
    RUN_TEST_CASE( Full_BLE_END_TO_END_MQTT, SubscribePublishAsync );

    /* Those 2 tests are disabled. At the moment BLE doesn't support
     * Persistant sessions nor Last Will and Testament.
     */
    /*RUN_TEST_CASE( Full_BLE_END_TO_END_MQTT, LastWillAndTestament );*/
    /*RUN_TEST_CASE( Full_BLE_END_TO_END_MQTT, RestorePreviousSession );*/

    RUN_TEST_CASE( Full_BLE_END_TO_END_MQTT, SubscribeCompleteReentrancy );
    RUN_TEST_CASE( Full_BLE_END_TO_END_MQTT, IncomingPublishReentrancy );


    /* Revert to sockets network interface after this test is finished. */
    IotTestNetwork_SelectNetworkType( DEFAULT_NETWORK );
}


/*-----------------------------------------------------------*/

TEST( Full_BLE_END_TO_END_CONNECTIVITY, InitializeBLE )
{
    _BLEInit();
}

TEST( Full_BLE_END_TO_END_CONNECTIVITY, EnableBLE )
{
    _BLEEnable();
}

TEST( Full_BLE_END_TO_END_CONNECTIVITY, SetDeviceName )
{
    TEST_ASSERT_EQUAL( eBTStatusSuccess,
                       IotBle_SetDeviceName( IOT_BLE_DEVICE_COMPLETE_LOCAL_NAME, strlen( IOT_BLE_DEVICE_COMPLETE_LOCAL_NAME ) ) );
}

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
    _BLEInit();
    _BLEEnable();

    /* For these tests, use the BLE network interface. */
    bBLEInitialized = IotTestNetwork_SelectNetworkType( AWSIOT_NETWORK_TYPE_BLE );

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
