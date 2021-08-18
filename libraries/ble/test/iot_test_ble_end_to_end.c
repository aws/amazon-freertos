/*
 * FreeRTOS BLE V2.2.0
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
/* Platform abstraction includes. */
#include "platform/iot_threads.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"


#define BLE_TEST_CONNECTION_TIMEOUT_MS    ( 15000 )

/*-----------------------------------------------------------*/

static bool bBLEInitialized = pdFALSE;
static bool bBLEEnabled = false;
static bool bBLEConnected = false;

static IotSemaphore_t bleConnectionSem;


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

/*-----------------------------------------------------------*/
