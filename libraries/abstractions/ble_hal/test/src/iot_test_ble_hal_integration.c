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


#include "iot_test_ble_hal_integration.h"
extern BTCallbacks_t _xBTManagerCb;

extern BTGattServerInterface_t * _pxGattServerInterface;
extern BTBleAdapter_t * _pxBTLeAdapterInterface;
extern BTInterface_t * _pxBTInterface;
extern uint8_t _ucBLEAdapterIf;
extern uint8_t _ucBLEServerIf;
extern uint16_t _usBLEConnId;
extern BTBdaddr_t _xAddressConnectedDevice;

extern BTService_t _xSrvcA;
extern BTService_t _xSrvcB;
extern uint16_t usHandlesBufferB[ bletestATTR_SRVCB_NUMBER ];
extern response_t ucRespBuffer[ bletestATTR_SRVCB_NUMBER ];

TEST_GROUP( Full_BLE_Integration_Test );

TEST_SETUP( Full_BLE_Integration_Test )
{
}

TEST_TEAR_DOWN( Full_BLE_Integration_Test )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP( Full_BLE_Integration_Test_common_GATT );

TEST_SETUP( Full_BLE_Integration_Test_common_GATT )
{
    GAP_common_setup();
}

TEST_TEAR_DOWN( Full_BLE_Integration_Test_common_GATT )
{
    GAP_common_teardown();
}

TEST_GROUP_RUNNER( Full_BLE_Integration_Test_common_GATT )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP( Full_BLE_Integration_Test_Advertisement );

TEST_SETUP( Full_BLE_Integration_Test_Advertisement )
{
    GATT_setup();
}

TEST_TEAR_DOWN( Full_BLE_Integration_Test_Advertisement )
{
    GATT_teardown();
}

TEST_GROUP_RUNNER( Full_BLE_Integration_Test_Advertisement )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP( Full_BLE_Integration_Test_Connection );

TEST_SETUP( Full_BLE_Integration_Test_Connection )
{
    /* Initialize */
    GATT_setup();
    prvCreateAndStartServiceB();

    /* Advertise and Connect */
    IotTestBleHal_SetAdvProperty();
    IotTestBleHal_SetAdvData( eBTuuidType128, 0, NULL );
    IotTestBleHal_StartAdvertisement();
    IotTestBleHal_WaitConnection( true );
}

TEST_TEAR_DOWN( Full_BLE_Integration_Test_Connection )
{
    /* Disconnect */
    IotTestBleHal_WaitConnection( false );
    GATT_teardown();
}

TEST_GROUP_RUNNER( Full_BLE_Integration_Test_Connection )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_BLE_Integration_Test )
{
    RUN_TEST_CASE( Full_BLE, BLE_Setup );

    RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Init_Enable_Twice );
    RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Enable_Disable_Time_Limit );
    RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Advertise_Without_Properties );

    /*TODO: no need to test now. ACE MW change the sequence to back to pxSetAdvData, pxSetScanResponse, pxStartAdv()*/
    /* RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Advertise_Before_Set_Data ); */

    RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Advertise_Interval_Consistent_After_BT_Reset );
    RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Write_Notification_Size_Greater_Than_MTU_3 );
    RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Integration_Connection_Timeout );
    RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Integration_Teardown );

    RUN_TEST_CASE( Full_BLE_Integration_Test_Advertisement, BLE_Advertise_With_16bit_ServiceUUID );
    RUN_TEST_CASE( Full_BLE_Integration_Test_Advertisement, BLE_Advertise_With_ManufactureData );

    RUN_TEST_CASE( Full_BLE_Integration_Test_Connection, BLE_Send_Data_After_Disconected );
    RUN_TEST_CASE( Full_BLE, BLE_Free );
}

/* Advertisement should work without initializing optional properties (device's name) */
TEST( Full_BLE_Integration_Test, BLE_Advertise_Without_Properties )
{
    IotTestBleHal_BLEGAPInit();
    IotTestBleHal_BLEGATTInit();
    IotTestBleHal_SetAdvData( eBTuuidType128, 0, NULL );
    IotTestBleHal_StartAdvertisement();
    /* Connect for evaluate KPI for next test case. */
    IotTestBleHal_WaitConnection( true );
}

/* Advertisement should work with 16bit Service UUID as well */
TEST( Full_BLE_Integration_Test_Advertisement, BLE_Advertise_With_16bit_ServiceUUID )
{
    IotTestBleHal_SetAdvProperty();
    IotTestBleHal_SetAdvData( eBTuuidType16, 0, NULL );
    IotTestBleHal_StartAdvertisement();
    /* Simple Connect */
    IotTestBleHal_WaitConnection( true );
    /* Disconnect */
    IotTestBleHal_WaitConnection( false );
}

TEST( Full_BLE_Integration_Test_Advertisement, BLE_Advertise_With_ManufactureData )
{
    /* Manufacturer-specific Data
     * First two bytes are company ID (randomly select Espressif(0x02E5) for test purpose)
     * Next bytes are defined by the company (randomly select unit8_t 5 for test purpose)*/
    uint16_t usManufacturerLen = 3;
    uint8_t pusManufacturerData[] = { 0xE5, 0x02, 0x05 };

    IotTestBleHal_SetAdvProperty();

    /* Check when manufacture data length is 0, but pointer is valid */
    IotTestBleHal_SetAdvData( eBTuuidType128, 0, ( char * ) pusManufacturerData );
    IotTestBleHal_StartAdvertisement();
    IotTestBleHal_WaitConnection( true );
    IotTestBleHal_WaitConnection( false );

    /* Check when manufacture data pointer is NULL, but length is not 0 */
    IotTestBleHal_SetAdvData( eBTuuidType128, usManufacturerLen, NULL );
    IotTestBleHal_StartAdvertisement();
    IotTestBleHal_WaitConnection( true );
    IotTestBleHal_WaitConnection( false );

    /* Check when manufacture data length is not 0, and pointer is valid */
    IotTestBleHal_SetAdvData( eBTuuidType128, usManufacturerLen, ( char * ) pusManufacturerData );
    IotTestBleHal_StartAdvertisement();
    IotTestBleHal_WaitConnection( true );
    IotTestBleHal_WaitConnection( false );
}

/* The sequence of set advertisement data and start advertisement can change. */
TEST( Full_BLE_Integration_Test, BLE_Advertise_Before_Set_Data )
{
    IotTestBleHal_StartAdvertisement();
    IotTestBleHal_SetAdvData( eBTuuidType128, 0, NULL );
    BTStatus_t xStatus = _pxBTLeAdapterInterface->pxStopAdv( _ucBLEAdapterIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

/* pxEnable/pxDisable can only return current. Make sure pxEnalbe/pxDisable works.
 * Make sure stack state is enable (callback received) no later than 2.5 seconds after pxEnable returns*/
TEST( Full_BLE_Integration_Test, BLE_Enable_Disable_Time_Limit )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTInitDeinitCallback_t xInitDeinitCb;
    clock_t returnTime, cbRecvTime;

    /* disable */
    IotTestBleHal_BLEEnable( false );

    /* enable */
    xStatus = _pxBTInterface->pxEnable( 0 );
    returnTime = clock();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventEnableDisableCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
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
    /* First time connection disconnects. */
    IotTestBleHal_WaitConnection( false );

    /* Second time reconnection. Got Second KPI. */
    IotTestBleHal_StartAdvertisement();
    IotTestBleHal_WaitConnection( true );
    IotTestBleHal_WaitConnection( false );

    /* BT reset. */
    BTStatus_t xStatus = eBTStatusSuccess;

    IotTestBleHal_BTUnregister();
    IotTestBleHal_BLEEnable( false );

    xStatus = _pxBTInterface->pxBtManagerCleanup();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = _pxBTInterface->pxBtManagerInit( &_xBTManagerCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    IotTestBleHal_BLEEnable( true );
    IotTestBleHal_BLEGAPInit();
    IotTestBleHal_BLEGATTInit();
    prvCreateAndStartServiceB();
    IotTestBleHal_SetAdvProperty();
    IotTestBleHal_SetAdvData( eBTuuidType128, 0, NULL );

    /* Third time connection begins. Got third KPI. */
    IotTestBleHal_StartAdvertisement();

    IotTestBleHal_WaitConnection( true );

    /* Result is on RPI. Write it back to device. */
    prvGetResult( bletestATTR_SRVCB_CHAR_D,
                  false,
                  0 );
}

/* If data size is > MTU - 3 then BT stack can truncate it to MTU - 3 and keep trying to send it over to other peer.
 * Make sure calling pxSendIndication() with xLen > MTU - 3 and HAL returns failure.*/
/* 2 chars has the same descriptor uuid which can cause read/write the descriptors of chars to return wrong values. */
TEST( Full_BLE_Integration_Test, BLE_Write_Notification_Size_Greater_Than_MTU_3 )
{
    BTStatus_t xStatus, xfStatus;
    uint8_t ucLargeBuffer[ bletestsMTU_SIZE1 + 2 ];
    uint8_t cccdFValue;

    /* Create a data payload whose length = MTU + 1. */
    static char bletests_MTU_2_CHAR_VALUE[ bletestsMTU_SIZE1 + 2 ];

    memset( bletests_MTU_2_CHAR_VALUE, 'a', ( bletestsMTU_SIZE1 + 1 ) * sizeof( char ) );
    bletests_MTU_2_CHAR_VALUE[ bletestsMTU_SIZE1 + 1 ] = '\0';

    cccdFValue = ucRespBuffer[ bletestATTR_SRVCB_CCCD_F ].ucBuffer[ 0 ];
    /* check the value of cccd E is changed from 0 to 1. */
    IotTestBleHal_checkNotificationIndication( bletestATTR_SRVCB_CCCD_E, true );
    /* check the value of cccd F does not change */
    TEST_ASSERT_EQUAL( ucRespBuffer[ bletestATTR_SRVCB_CCCD_F ].ucBuffer[ 0 ], cccdFValue );

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
        /* Notify RPI failure here. Expect to receive "fail" messaprvSetAdvertisementge. */
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

TEST( Full_BLE_Integration_Test_Connection, BLE_Send_Data_After_Disconected )
{
    BLETESTwriteAttrCallback_t xWriteEvent;
    BLETESTreadAttrCallback_t xReadEvent;

    /* Check communication */
    xWriteEvent = IotTestBleHal_WriteReceive( bletestATTR_SRVCB_CHAR_A, true, false, 0 );
    IotTestBleHal_WriteResponse( bletestATTR_SRVCB_CHAR_A, xWriteEvent, true );
    xReadEvent = IotTestBleHal_ReadReceive( bletestATTR_SRVCB_CHAR_A );
    IotTestBleHal_ReadResponse( bletestATTR_SRVCB_CHAR_A, xReadEvent, true );

    IotTestBleHal_checkNotificationIndication( bletestATTR_SRVCB_CCCD_E, true );
    IotTestBleHal_checkNotificationIndication( bletestATTR_SRVCB_CCCD_F, true );
    IotTestBleHal_CheckIndicationNotification( false, true );
    IotTestBleHal_CheckIndicationNotification( true, true );

    /* Disconnect from RPi and Check communication*/
    IotTestBleHal_WaitConnection( false );

    IotTestBleHal_WriteResponse( bletestATTR_SRVCB_CHAR_A, xWriteEvent, false );
    IotTestBleHal_ReadResponse( bletestATTR_SRVCB_CHAR_A, xReadEvent, false );
    IotTestBleHal_CheckIndicationNotification( false, false );
    IotTestBleHal_CheckIndicationNotification( true, false );

    /* Advertise and Reconnect */
    IotTestBleHal_SetAdvProperty();
    IotTestBleHal_SetAdvData( eBTuuidType128, 0, NULL );
    IotTestBleHal_StartAdvertisement();
    IotTestBleHal_WaitConnection( true );

    /* Check communication */
    IotTestBleHal_checkNotificationIndication( bletestATTR_SRVCB_CCCD_E, true );
    IotTestBleHal_checkNotificationIndication( bletestATTR_SRVCB_CCCD_F, true );

    xWriteEvent = IotTestBleHal_WriteReceive( bletestATTR_SRVCB_CHAR_A, true, false, 0 );
    IotTestBleHal_WriteResponse( bletestATTR_SRVCB_CHAR_A, xWriteEvent, true );
    xReadEvent = IotTestBleHal_ReadReceive( bletestATTR_SRVCB_CHAR_A );
    IotTestBleHal_ReadResponse( bletestATTR_SRVCB_CHAR_A, xReadEvent, true );
    IotTestBleHal_CheckIndicationNotification( false, true );
    IotTestBleHal_CheckIndicationNotification( true, true );
}

/* trigger Adv Stop callback AdvStartCB(with start=false) when Adv timeout. */
TEST( Full_BLE_Integration_Test, BLE_Integration_Connection_Timeout )
{
    IotTestBleHal_WaitConnection( false );
    IotTestBleHal_StartAdvertisement();
    prvShortWaitConnection();
}

TEST( Full_BLE_Integration_Test, BLE_Integration_Teardown )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    IotTestBleHal_StopService( &_xSrvcB );
    IotTestBleHal_DeleteService( &_xSrvcB );
    IotTestBleHal_BTUnregister();
    IotTestBleHal_BLEEnable( false );

    xStatus = _pxBTInterface->pxBtManagerCleanup();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

void prvGetResult( bletestAttSrvB_t xAttribute,
                   bool IsPrep,
                   uint16_t usOffset )
{
    BLETESTwriteAttrCallback_t xWriteEvent;
    BTStatus_t xStatus;

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventWriteAttrCb, usHandlesBufferB[ xAttribute ], ( void * ) &xWriteEvent, sizeof( BLETESTwriteAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( IsPrep, xWriteEvent.bIsPrep );
    TEST_ASSERT_EQUAL( 49, xWriteEvent.ucValue[ 0 ] );
    TEST_ASSERT_EQUAL( _usBLEConnId, xWriteEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, memcmp( &xWriteEvent.xBda, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( usOffset, xWriteEvent.usOffset );
}

/* wait for connection establish for a short timeout. */
void prvShortWaitConnection()
{
    BLETESTConnectionCallback_t xConnectionEvent;
    BTStatus_t xStatus;

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventConnectionCb, NO_HANDLE, ( void * ) &xConnectionEvent, sizeof( BLETESTConnectionCallback_t ), BLE_TESTS_SHORT_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusFail, xStatus );
    TEST_ASSERT_NOT_EQUAL( eBTStatusSuccess, xConnectionEvent.xStatus );
    IotTestBleHal_StartStopAdvCheck( false );
}

void prvCreateAndStartServiceB()
{
    BTStatus_t xStatus;

    xStatus = _pxGattServerInterface->pxAddServiceBlob( _ucBLEServerIf, &_xSrvcB );

    if( xStatus == eBTStatusUnsupported )
    {
        IotTestBleHal_CreateServiceB();
        IotTestBleHal_StartService( &_xSrvcB );
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
    clock_t returnTime, cbRecvTime;

    /* Get BT interface */
    _pxBTInterface = ( BTInterface_t * ) BTGetBluetoothInterface();
    TEST_ASSERT_NOT_EQUAL( NULL, _pxBTInterface );

    /* First time init */
    xStatus = _pxBTInterface->pxBtManagerInit( &_xBTManagerCb );
    returnTime = clock();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    /* First time enable */
    xStatus = _pxBTInterface->pxEnable( 0 );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventEnableDisableCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
    cbRecvTime = clock();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTstateOn, xInitDeinitCb.xBLEState );
    TEST_ASSERT_LESS_THAN( CLOCKS_PER_SEC * 5, ( cbRecvTime - returnTime ) * 2 );

    /*First time Deinit*/
    xStatus = _pxBTInterface->pxBtManagerCleanup();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    /* Second time init */
    xStatus = _pxBTInterface->pxBtManagerInit( &_xBTManagerCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    /*Second time check if BT stack is enabled after deinit and init*/
    TEST_ASSERT_EQUAL( eBTstateOn, xInitDeinitCb.xBLEState );

    /* Second time enable */
    IotTestBleHal_BLEEnable( true );
}

void GAP_common_teardown()
{
    BTStatus_t xStatus = eBTStatusSuccess;

    /* Disable */
    IotTestBleHal_BLEEnable( false );

    /* Deinit */
    xStatus = _pxBTInterface->pxBtManagerCleanup();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}
/*-----------------------------------------------------------*/

void GAP_common_setup()
{
    BTStatus_t xStatus = eBTStatusSuccess;

    /* Get BT interface */
    _pxBTInterface = ( BTInterface_t * ) BTGetBluetoothInterface();
    TEST_ASSERT_NOT_EQUAL( NULL, _pxBTInterface );

    /* First time init */
    xStatus = _pxBTInterface->pxBtManagerInit( &_xBTManagerCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    /* Enable */
    IotTestBleHal_BLEEnable( true );
}

void GATT_teardown()
{
    IotTestBleHal_BTUnregister();
    GAP_common_teardown();
}
/*-----------------------------------------------------------*/

void GATT_setup()
{
    GAP_common_setup();
    IotTestBleHal_BLEGAPInit();
    IotTestBleHal_BLEGATTInit();
}

void Advertisement_teardown()
{
    GATT_teardown();
}
/*-----------------------------------------------------------*/

void Advertisement_setup()
{
    GATT_setup();
    prvCreateAndStartServiceB();
    IotTestBleHal_SetAdvProperty();
    IotTestBleHal_SetAdvData( eBTuuidType128, 0, NULL );

    /* Second time connection begins. Got second KPI. */
    IotTestBleHal_StartAdvertisement();
}
