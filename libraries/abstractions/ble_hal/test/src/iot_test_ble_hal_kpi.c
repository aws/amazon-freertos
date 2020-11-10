/*
 * FreeRTOS BLE HAL V5.1.0
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
 * @file aws_test_ble_kpi.c
 * @brief Tests for ble.
 */

#include "iot_test_ble_hal_kpi.h"

extern BTInterface_t * _pxBTInterface;

/*-----------------------------------------------------------*/

TEST_GROUP( Full_BLE_KPI_Test );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_BLE_KPI_Test )
{
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_BLE_KPI_Test )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_BLE_KPI_Test )
{
    RUN_TEST_CASE( Full_BLE, BLE_Setup );
    RUN_TEST_CASE( Full_BLE, BLE_Initialize_common_GAP );
    RUN_TEST_CASE( Full_BLE, BLE_Initialize_BLE_GAP );
    RUN_TEST_CASE( Full_BLE, BLE_Initialize_BLE_GATT );

    RUN_TEST_CASE( Full_BLE, BLE_Advertising_SetProperties ); /*@TOTO, incomplete */
    RUN_TEST_CASE( Full_BLE, BLE_Connection_RemoveAllBonds );

    RUN_TEST_CASE( Full_BLE, BLE_Advertising_SetAvertisementData ); /*@TOTO, incomplete */
    RUN_TEST_CASE( Full_BLE_KPI_Test, BLE_KPI_ReConnect );

    RUN_TEST_CASE( Full_BLE_KPI_Test, BLE_KPI_Teardown );
    RUN_TEST_CASE( Full_BLE, BLE_Free );
}

TEST( Full_BLE_KPI_Test, BLE_KPI_ReConnect )
{
    uint32_t loop;

    for( loop = 0; loop < TOTAL_NUMBER_RECONNECT; loop++ )
    {
        IotTestBleHal_StartAdvertisement();
        IotTestBleHal_WaitConnection( true );
        IotTestBleHal_WaitConnection( false );
    }
}

TEST( Full_BLE_KPI_Test, BLE_KPI_Teardown )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    IotTestBleHal_BTUnregister();
    IotTestBleHal_BLEEnable( false );

    xStatus = _pxBTInterface->pxBtManagerCleanup();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}
