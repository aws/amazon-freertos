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
 * @file aws_test_ble_integration.c
 * @brief Tests for ble.
 */


#include <time.h>
/* #include <unistd.h> */


#include "iot_test_ble_hal_integration.h"

extern BTCallbacks_t _xBTManagerCb;

extern BTGattServerInterface_t * _pxGattServerInterface;
extern BTBleAdapter_t * _pxBTLeAdapterInterface;
extern BTInterface_t * _pxBTInterface;
extern uint8_t _ucBLEAdapterIf;
extern uint8_t _ucBLEServerIf;
extern uint16_t _usBLEConnId;
extern BTBdaddr_t _xAddressConnectedDevice;

extern BTService_t _xSrvcB;
extern uint16_t usHandlesBufferB[ bletestATTR_SRVCB_NUMBER ];


TEST_GROUP( Full_BLE_Integration_Test );

TEST_SETUP( Full_BLE_Integration_Test )
{
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_BLE_Integration_Test )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_BLE_Integration_Test )
{
    RUN_TEST_CASE( Full_BLE, BLE_Setup );
    RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Init_Enable_Twice );
    RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Enable_Disable_BT_Module );

    RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Advertise_Without_Properties );

    /*TODO: no need to test now. ACE MW change the sequence to back to pxSetAdvData, pxSetScanResponse, pxStartAdv()*/
    /* RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Advertise_Before_Set_Data ); */

    RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Advertise_Interval_Consistent_After_BT_Reset );

    RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Write_Notification_Size_Greater_Than_MTU_3 );

    RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Integration_Teardown );
    RUN_TEST_CASE( Full_BLE, BLE_Free );
}

/* Advertisement should work without initializing optional properties (device's name) */
TEST( Full_BLE_Integration_Test, BLE_Advertise_Without_Properties )
{
    prvBLEGAPInit();
    prvBLEGATTInit();
    prvSetAdvData();
    prvStartAdvertisement();
    /* Connect for evaluate KPI for next test case. */
    prvWaitConnection( true );
}

/* The sequence of set advertisement data and start advertisement can change. */
TEST( Full_BLE_Integration_Test, BLE_Advertise_Before_Set_Data )
{
    prvStartAdvertisement();
    prvSetAdvData();
    BTStatus_t xStatus = _pxBTLeAdapterInterface->pxStopAdv( _ucBLEAdapterIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

/* pxEnable/pxDisable can only return current. Make sure pxEnalbe/pxDisable works.
 * Make sure stack state is enable (callback received) no later than 2.5 seconds after pxEnable returns*/
TEST( Full_BLE_Integration_Test, BLE_Enable_Disable_BT_Module )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTInitDeinitCallback_t xInitDeinitCb;
    clock_t returnTime, cbRecvTime;

    /* disable */
    prvBLEEnable( false );

    /* enable */
    xStatus = _pxBTInterface->pxEnable( 0 );
    returnTime = clock();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventEnableDisableCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
    cbRecvTime = clock();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTstateOn, xInitDeinitCb.xBLEState );
    TEST_ASSERT_LESS_THAN( CLOCKS_PER_SEC * 5, ( cbRecvTime - returnTime ) * 2 );
}


/* Crash if calling pxEnable twice (MTK)
 * (1)init -> (2)enable -> (3)deinit -> (4)init -> (5)enable
 * There are 2 issues with this sequence:
 * (4)init reset stack state to disabled even though it's still enabled
 * (5)enable trigger pxEnable again while MTK stack is enabled -> mtk crashed   */
TEST( Full_BLE_Integration_Test, BLE_Init_Enable_Twice )
{
    prvGAPInitEnableTwice();
}

/*Advertisement interval measured OTA can be out the range set by app, after reset BT stack, adv interval can change to 1.28s.
 * Make sure KPI is consistent after reset BT.*/
TEST( Full_BLE_Integration_Test, BLE_Advertise_Interval_Consistent_After_BT_Reset )
{
    /* First time connection disconnects. Got First KPI. */
    prvWaitConnection( false );

    /* BT reset. */
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTInitDeinitCallback_t xInitDeinitCb;

    prvBTUnregister();
    prvBLEEnable( false );

    xStatus = _pxBTInterface->pxBtManagerCleanup();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = _pxBTInterface->pxBtManagerInit( &_xBTManagerCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    prvBLEEnable( true );
    prvBLEGAPInit();
    prvBLEGATTInit();
    prvCreateAndStartServiceB();
    prvSetAdvProperty();
    prvSetAdvData();

    /* Second time connection begins. Got second KPI. */
    prvStartAdvertisement();

    prvWaitConnection( true );

    /* Result is on RPI. Write it back to device. */
    prvGetResult( bletestATTR_SRVCB_CHAR_D,
                  false,
                  0 );
}

/* If data size is > MTU - 3 then BT stack can truncate it to MTU - 3 and keep trying to send it over to other peer.
 * Make sure calling pxSendIndication() with xLen > MTU - 3 and stack returns failure.*/
TEST( Full_BLE_Integration_Test, BLE_Write_Notification_Size_Greater_Than_MTU_3 )
{
    BTStatus_t xStatus, xfStatus;
    BLETESTindicateCallback_t xIndicateEvent;
    uint8_t ucLargeBuffer[ bletestsMTU_SIZE1 + 2 ];

    /* Create a data payload whose length = MTU + 1. */
    static char bletests_MTU_2_CHAR_VALUE[ bletestsMTU_SIZE1 + 2 ];

    /* for( int i = 0; i < bletestsMTU_SIZE1 + 1; i++ ) */
    /* { */
    /*     bletests_MTU_2_CHAR_VALUE[ i ] = 'a'; */
    /* } */
    memset( bletests_MTU_2_CHAR_VALUE, 'a', ( bletestsMTU_SIZE1 + 1 ) * sizeof( char ) );

    bletests_MTU_2_CHAR_VALUE[ bletestsMTU_SIZE1 + 1 ] = '\0';

    checkNotificationIndication( bletestATTR_SRVCB_CCCD_E, true );
    memcpy( ucLargeBuffer, bletests_MTU_2_CHAR_VALUE, bletestsMTU_SIZE1 + 1 );

    /* Expect to return failure here. */
    xStatus = _pxGattServerInterface->pxSendIndication( _ucBLEServerIf,
                                                        usHandlesBufferB[ bletestATTR_SRVCB_CHAR_E ],
                                                        _usBLEConnId,
                                                        bletestsMTU_SIZE1 + 1,
                                                        ucLargeBuffer,
                                                        false );
    TEST_ASSERT_NOT_EQUAL( eBTStatusSuccess, xStatus );

    if( xStatus != eBTStatusSuccess )
    {
        /* Notify RPI failure here. Expect to receive "fail" message. */
        memcpy( ucLargeBuffer, bletestsFAIL_CHAR_VALUE, sizeof( bletestsFAIL_CHAR_VALUE ) - 1 );
        xfStatus = _pxGattServerInterface->pxSendIndication( _ucBLEServerIf,
                                                             usHandlesBufferB[ bletestATTR_SRVCB_CHAR_E ],
                                                             _usBLEConnId,
                                                             sizeof( bletestsFAIL_CHAR_VALUE ) - 1,
                                                             ucLargeBuffer,
                                                             false );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xfStatus );
    }
}

TEST( Full_BLE_Integration_Test, BLE_Integration_Teardown )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    prvWaitConnection( false );
    prvStopService( &_xSrvcB );
    prvDeleteService( &_xSrvcB );
    prvBTUnregister();
    prvBLEEnable( false );

    xStatus = _pxBTInterface->pxBtManagerCleanup();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}


void prvGetResult( bletestAttSrvB_t xAttribute,
                   bool IsPrep,
                   uint16_t usOffset )
{
    BLETESTwriteAttrCallback_t xWriteEvent;
    BLETESTconfirmCallback_t xConfirmEvent;
    BTGattResponse_t xGattResponse;
    BTStatus_t xStatus;

    xStatus = prvWaitEventFromQueue( eBLEHALEventWriteAttrCb, usHandlesBufferB[ xAttribute ], ( void * ) &xWriteEvent, sizeof( BLETESTwriteAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( IsPrep, xWriteEvent.bIsPrep );
    TEST_ASSERT_EQUAL( 49, xWriteEvent.ucValue[ 0 ] );
    TEST_ASSERT_EQUAL( _usBLEConnId, xWriteEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, memcmp( &xWriteEvent.xBda, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( usOffset, xWriteEvent.usOffset );
}

void prvCreateAndStartServiceB()
{
    BTStatus_t xStatus;

    xStatus = _pxGattServerInterface->pxAddServiceBlob( _ucBLEServerIf, &_xSrvcB );

    if( xStatus == eBTStatusUnsupported )
    {
        prvCreateServiceB();
        prvStartService( &_xSrvcB );
    }
    else
    {
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    }
}


void prvGAPInitEnableTwice()
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTInitDeinitCallback_t xInitDeinitCb;

    /* Get BT interface */
    _pxBTInterface = ( BTInterface_t * ) BTGetBluetoothInterface();
    TEST_ASSERT_NOT_EQUAL( NULL, _pxBTInterface );

    /* First time init */
    xStatus = _pxBTInterface->pxBtManagerInit( &_xBTManagerCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    /* First time enable */
    xStatus = _pxBTInterface->pxEnable( 0 );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventEnableDisableCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTstateOn, xInitDeinitCb.xBLEState );

    /*First time Deinit*/
    xStatus = _pxBTInterface->pxBtManagerCleanup();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    /* Second time init */
    xStatus = _pxBTInterface->pxBtManagerInit( &_xBTManagerCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    /*Second time check if BT stack is enabled after deinit and init*/
    TEST_ASSERT_EQUAL( eBTstateOn, xInitDeinitCb.xBLEState );

    /* Second time enable */
    prvBLEEnable( true );
}
