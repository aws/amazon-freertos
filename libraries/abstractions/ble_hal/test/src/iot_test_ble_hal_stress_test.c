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

extern BTService_t _xSrvcA;
extern BTService_t _xSrvcB;
extern BTInterface_t * _pxBTInterface;

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
    /* Initializations that need to be done once for all the tests. */
    RUN_TEST_CASE( Full_BLE, BLE_Setup );

    for( uint32_t init_loop = 0; init_loop < INIT_DEINIT_NUMBER_STRESS_TEST; init_loop++ )
    {
        RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Stack_Init );

        for( uint32_t enable_loop = 0; enable_loop < ENABLE_DISABLE_NUMBER_STRESS_TEST; enable_loop++ )
        {
            /*TODO: add some randomness to catch timing issues */

            RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Stack_Enable );

            RUN_TEST_CASE( Full_BLE, BLE_Initialize_BLE_GAP );
            RUN_TEST_CASE( Full_BLE, BLE_Initialize_BLE_GATT );

            RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Service_Create );
            RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Service_Restart );
            RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Service_Delete );

            RUN_TEST_CASE( Full_BLE, BLE_Advertising_SetProperties );       /*@TOTO, incomplete */
            RUN_TEST_CASE( Full_BLE, BLE_Connection_RemoveAllBonds );
            RUN_TEST_CASE( Full_BLE, BLE_Advertising_SetAvertisementData ); /*@TOTO, incomplete */

            RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Connection_ReConnect );

            RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Teardown );

            RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Stack_Disable );
        }

        RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Stack_Deinit );
    }

    RUN_TEST_CASE( Full_BLE, BLE_Free );
}

TEST( Full_BLE_Stress_Test, BLE_Stack_Init )
{
    IotTestBleHal_BLEManagerInit();
}

TEST( Full_BLE_Stress_Test, BLE_Stack_Enable )
{
    IotTestBleHal_BLEEnable( true );
}

TEST( Full_BLE_Stress_Test, BLE_Stack_Disable )
{
    IotTestBleHal_BLEEnable( false );
}

TEST( Full_BLE_Stress_Test, BLE_Stack_Deinit )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = _pxBTInterface->pxBtManagerCleanup();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

TEST( Full_BLE_Stress_Test, BLE_Service_Delete )
{
    IotTestBleHal_DeleteService( &_xSrvcA );
    IotTestBleHal_DeleteService( &_xSrvcB );
}

TEST( Full_BLE_Stress_Test, BLE_Teardown )
{
    IotTestBleHal_BTUnregister();
}

TEST( Full_BLE_Stress_Test, BLE_Service_Create )
{
    IotTestBleHal_CreateServiceA();
    IotTestBleHal_CreateServiceB();
}

TEST( Full_BLE_Stress_Test, BLE_Service_Restart )
{
    prvRestartService( &_xSrvcA );
    prvRestartService( &_xSrvcB );
}

TEST( Full_BLE_Stress_Test, BLE_Connection_ReConnect )
{
    uint32_t loop;

    for( loop = 0; loop < RECONNECT_NUMBER_STRESS_TEST; loop++ )
    {
        IotTestBleHal_StartAdvertisement();
        IotTestBleHal_WaitConnection( true );
        IotTestBleHal_WaitConnection( false );
    }
}

void prvRestartService( BTService_t * xRefSrvc )
{
    uint16_t loop;

    for( loop = 0; loop < RESTART_NUMBER_STRESS_TEST; loop++ )
    {
        IotTestBleHal_StartService( xRefSrvc );
        IotTestBleHal_StopService( xRefSrvc );
    }
}
