/*
 * Amazon FreeRTOS BLE HAL V1.0.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_test_ble_stress_test.c
 * @brief Tests for ble.
 */

#include "iot_test_ble_hal_stress_test.h"

#define TOTAL_NUMBER_STRESS_TEST    10

static BTCallbacks_t xBTManagerCb =
{
    .pxDeviceStateChangedCb     = prvDeviceStateChangedCb,
    .pxAdapterPropertiesCb      = NULL,
    .pxRemoteDevicePropertiesCb = NULL,
    .pxSspRequestCb             = NULL,
    .pxPairingStateChangedCb    = NULL,
    .pxBondedCb                 = NULL,
    .pxDutModeRecvCb            = NULL,
    .pxleTestModeCb             = NULL,
    .pxEnergyInfoCb             = NULL,
    .pxReadRssiCb               = NULL,
    .pxTxPowerCb                = NULL,
    .pxSlaveSecurityRequestCb   = NULL,
};

/*-----------------------------------------------------------*/

TEST_GROUP( Full_BLE_Stress_Test );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_BLE_Stress_Test )
{
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_BLE_Stress_Test )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_BLE_Stress_Test )
{
    uint16_t loop;

    /* Initializations that need to be done once for all the tests. */
    RUN_TEST_CASE( Full_BLE, BLE_Setup );

    RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Stack_Init_DeInit_Setup );

    for( loop = 0; loop < TOTAL_NUMBER_STRESS_TEST; loop++ )
    {
        RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Stack_Init_DeInit );
    }

    RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Stack_Init_DeInit_Teardown );

    /* RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Simple_Connect_Setup ); */
    /* for ( loop = 0; loop < TOTAL_NUMBER_STRESS_TEST; loop++ ) */
    /* { */
    /*     RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Simple_Connect ); */
    /* } */
    /* RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Simple_Connect_Teardown ); */
}

TEST( Full_BLE_Stress_Test, BLE_Stack_Init_DeInit_Setup )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    /* Get BT interface */
    g_pxBTInterface = ( BTInterface_t * ) BTGetBluetoothInterface();
    TEST_ASSERT_NOT_EQUAL( NULL, g_pxBTInterface );

    /* Initialize callbacks */
    xStatus = g_pxBTInterface->pxBtManagerInit( &xBTManagerCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

TEST( Full_BLE_Stress_Test, BLE_Stack_Init_DeInit )
{
    BLETESTInitDeinitCallback_t xInitDeinitCb;
    BTStatus_t xStatus = eBTStatusSuccess;

    /* Enable RADIO and wait for callback. */
    xStatus = g_pxBTInterface->pxEnable( 0 );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventEnableDisableCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTstateOn, xInitDeinitCb.xBLEState );

    /* Enable RADIO and wait for callback. */
    xStatus = g_pxBTInterface->pxDisable( 0 );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventEnableDisableCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTstateOff, xInitDeinitCb.xBLEState );
}

TEST( Full_BLE_Stress_Test, BLE_Stack_Init_DeInit_Teardown )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    /* Initialize callbacks */
    xStatus = g_pxBTInterface->pxBtManagerCleanup();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

void prvDeviceStateChangedCb( BTState_t xState )
{
    BLETESTInitDeinitCallback_t * pxInitDeinitCb = IotTest_Malloc( sizeof( BLETESTInitDeinitCallback_t ) );

    pxInitDeinitCb->xBLEState = xState;
    pxInitDeinitCb->xEvent.lHandle = NO_HANDLE;
    pxInitDeinitCb->xEvent.xEventTypes = eBLEHALEventEnableDisableCb;
    pushToQueue( &pxInitDeinitCb->xEvent.eventList );
}

TEST( Full_BLE_Stress_Test, BLE_Simple_Connect_Setup )
{
}

TEST( Full_BLE_Stress_Test, BLE_Simple_Connect )
{
}

TEST( Full_BLE_Stress_Test, BLE_Simple_Connect_Teardown )
{
}
