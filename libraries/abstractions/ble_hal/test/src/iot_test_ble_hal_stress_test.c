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
 * @file aws_test_ble_stress_test.c
 * @brief Tests for ble.
 */

#include "iot_test_ble_hal_stress_test.h"

extern BTService_t _xSrvcA;
extern BTService_t _xSrvcB;
extern BTInterface_t * _pxBTInterface;
extern uint8_t _ucBLEServerIf;
extern BTCallbacks_t _xBTManagerCb;
extern BTGattServerCallbacks_t _xBTGattServerCb;
extern BTBleAdapterCallbacks_t _xBTBleAdapterCb;
extern BTGattServerInterface_t * _pxGattServerInterface;

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

    for( uint32_t init_loop = 0; init_loop < STRESS_TEST_INIT_REPETITION; init_loop++ )
    {
        RUN_TEST_CASE( Full_BLE_Stress_Test, Initialize_common_GAP );

        for( uint32_t enable_loop = 0; enable_loop < STRESS_TEST_ENABLE_REPETITION; enable_loop++ )
        {
            /*TODO: add some randomness to catch timing issues */
            RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Init_GAP_GATT );
            RUN_TEST_CASE( Full_BLE, BLE_Advertising_SetProperties );       /*@TOTO, incomplete */
            RUN_TEST_CASE( Full_BLE, BLE_Connection_RemoveAllBonds );
            RUN_TEST_CASE( Full_BLE, BLE_Advertising_SetAvertisementData ); /*@TOTO, incomplete */

            for( uint16_t loop = 0; loop < STRESS_TEST_MODULE_REPETITION; loop++ )
            {
                RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Connection_ReConnect );
            }

            RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Service_Create );

            for( uint16_t loop = 0; loop < STRESS_TEST_MODULE_REPETITION; loop++ )
            {
                RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Service_Restart );
            }

            RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Start_Connection );

            for( uint16_t loop = 0; loop < STRESS_TEST_MODULE_REPETITION; loop++ )
            {
                RUN_TEST_CASE( Full_BLE, BLE_Property_WriteCharacteristic );
                RUN_TEST_CASE( Full_BLE, BLE_Property_WriteDescriptor );
                RUN_TEST_CASE( Full_BLE, BLE_Property_ReadCharacteristic );
                RUN_TEST_CASE( Full_BLE, BLE_Property_ReadDescriptor );
            }

            RUN_TEST_CASE( Full_BLE, BLE_Property_Enable_Indication_Notification );

            for( uint16_t loop = 0; loop < STRESS_TEST_MODULE_REPETITION; loop++ )
            {
                RUN_TEST_CASE( Full_BLE, BLE_Property_Notification );
                RUN_TEST_CASE( Full_BLE, BLE_Property_Indication );
            }

            RUN_TEST_CASE( Full_BLE, BLE_Property_Disable_Indication_Notification );
            RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Teardown );
        }

        RUN_TEST_CASE( Full_BLE_Stress_Test, BLE_Memory_Cleanup );
    }

    RUN_TEST_CASE( Full_BLE, BLE_Free );
}

TEST( Full_BLE_Stress_Test, Initialize_common_GAP )
{
    IotTestBleHal_BLEManagerInit( &_xBTManagerCb );
}

TEST( Full_BLE_Stress_Test, BLE_Start_Connection )
{
    IotTestBleHal_StartAdvertisement();
    IotTestBleHal_WaitConnection( true );
}

TEST( Full_BLE_Stress_Test, BLE_Init_GAP_GATT )
{
    IotTestBleHal_BLEEnable( true );
    IotTestBleHal_BLEGAPInit( &_xBTBleAdapterCb, true );
    IotTestBleHal_BLEGATTInit( &_xBTGattServerCb, true );
}

TEST( Full_BLE_Stress_Test, BLE_Stack_Disable )
{
    IotTestBleHal_BLEEnable( false );
}

TEST( Full_BLE_Stress_Test, BLE_Memory_Cleanup )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = _pxBTInterface->pxBtManagerCleanup();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

TEST( Full_BLE_Stress_Test, BLE_Teardown )
{
    IotTestBleHal_StopService( &_xSrvcA );
    IotTestBleHal_StopService( &_xSrvcB );
    IotTestBleHal_DeleteService( &_xSrvcA );
    IotTestBleHal_DeleteService( &_xSrvcB );
    IotTestBleHal_BTUnregister();
    IotTestBleHal_BLEEnable( false );
}

TEST( Full_BLE_Stress_Test, BLE_Service_Create )
{
    BTStatus_t xStatus;

    /* Try to create using blob service API first.
     * If blob is not supported then try legacy APIs. */
    xStatus = _pxGattServerInterface->pxAddServiceBlob( _ucBLEServerIf, &_xSrvcA );

    if( xStatus != eBTStatusUnsupported )
    {
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
        xStatus = _pxGattServerInterface->pxAddServiceBlob( _ucBLEServerIf, &_xSrvcB );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    }
    else
    {
        /* Create service A&B */
        IotTestBleHal_CreateServiceA();
        IotTestBleHal_CreateServiceB( false );

        /* Start service A&B */
        IotTestBleHal_StartService( &_xSrvcA );
        IotTestBleHal_StartService( &_xSrvcB );
    }
}

TEST( Full_BLE_Stress_Test, BLE_Service_Restart )
{
    prvRestartService( &_xSrvcA );
    prvRestartService( &_xSrvcB );
}

TEST( Full_BLE_Stress_Test, BLE_Connection_ReConnect )
{
    IotTestBleHal_StartAdvertisement();
    IotTestBleHal_WaitConnection( true );
    IotTestBleHal_WaitConnection( false );
}

void prvRestartService( BTService_t * xRefSrvc )
{
    IotTestBleHal_StopService( xRefSrvc );
    IotTestBleHal_StartService( xRefSrvc );
}
