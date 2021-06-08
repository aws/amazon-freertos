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
 * @file aws_test_ble_integration.c
 * @brief Tests for ble.
 */

#include "iot_ble_config.h"
#include "iot_test_ble_hal_integration.h"
extern BTCallbacks_t _xBTManagerCb;
extern BTBleAdapterCallbacks_t _xBTBleAdapterCb;
extern BTGattServerCallbacks_t _xBTGattServerCb;
extern BTCallbacks_t _xBTManager_NULL_Cb;
extern BTBleAdapterCallbacks_t _xBTBleAdapter_NULL_Cb;
extern BTGattServerCallbacks_t _xBTGattServer_NULL_Cb;
extern BTGattServerCallbacks_t _xBTGattServer_Nested_Cb;

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
extern BTGattAdvertismentParams_t xAdvertisementConfigA;
extern BTGattAdvertismentParams_t xAdvertisementConfigB;
extern BTUuid_t xServerUUID;
extern BTUuid_t xAppUUID;
extern bool bCharAddedComplete;
extern uint16_t _bletestsMTU_SIZE;

static uint8_t ucLargeBuffer[ IOT_BLE_PREFERRED_MTU_SIZE + 2 ];

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
    IotTestBleHal_CreateStartServiceB( false );

    /* Advertise and Connect */
    IotTestBleHal_SetAdvProperty();
    IotTestBleHal_SetAdvData( eBTuuidType128, 0, NULL, 0, NULL );
    IotTestBleHal_StartAdvertisement();
    IotTestBleHal_WaitConnection( true );
}

TEST_TEAR_DOWN( Full_BLE_Integration_Test_Connection )
{
    /* Disconnect */
    IotTestBleHal_WaitConnection( false );
    IotTestBleHal_StopService( &_xSrvcB );
    IotTestBleHal_DeleteService( &_xSrvcB );

    GATT_teardown();
}

TEST_GROUP_RUNNER( Full_BLE_Integration_Test_Connection )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_BLE_Integration_Test )
{
    RUN_TEST_CASE( Full_BLE, BLE_Setup );

    #if ENABLE_TC_INTEGRATION_ADD_CHARACTERISTIC_IN_CALLBACK
        RUN_TEST_CASE( Full_BLE_Integration_Test_common_GATT, BLE_Add_Characteristic_In_Callback );
    #endif
    #if ENABLE_TC_INTEGRATION_ADVERTISE_WITHOUT_PROPERTY
        RUN_TEST_CASE( Full_BLE_Integration_Test_Advertisement, BLE_Advertise_Without_Properties );
    #endif
    #if ENABLE_TC_INTEGRATION_ADVERTISE_WITH_16BIT_SERVICEUUID
        RUN_TEST_CASE( Full_BLE_Integration_Test_Advertisement, BLE_Advertise_With_16bit_ServiceUUID );
    #endif
    #if ENABLE_TC_INTEGRATION_ADVERTISE_WITH_MANUFACTUREDATA
        RUN_TEST_CASE( Full_BLE_Integration_Test_Advertisement, BLE_Advertise_With_ManufactureData );
    #endif
    #if ENABLE_TC_INTEGRATION_ADVERTISE_WITH_SERVICEDATA
        RUN_TEST_CASE( Full_BLE_Integration_Test_Advertisement, BLE_Advertise_With_ServiceData );
    #endif
    #if ENABLE_TC_INTEGRATION_ADVERTISE_INTERVAL_CONSISTENT_AFTER_BT_RESET
        RUN_TEST_CASE( Full_BLE_Integration_Test_Advertisement, BLE_Advertise_Interval_Consistent_After_BT_Reset );
    #endif
    #if ENABLE_TC_INTEGRATION_WRITE_NOTIFICATION_SIZE_GREATER_THAN_MTU_3
        RUN_TEST_CASE( Full_BLE_Integration_Test_Connection, BLE_Write_Notification_Size_Greater_Than_MTU_3 );
    #endif
    #if ENABLE_TC_INTEGRATION_CONNECTION_TIMEOUT
        RUN_TEST_CASE( Full_BLE_Integration_Test_Advertisement, BLE_Integration_Connection_Timeout );
    #endif
    #if ENABLE_TC_INTEGRATION_SEND_DATA_AFTER_DISCONNECTED
        RUN_TEST_CASE( Full_BLE_Integration_Test_Connection, BLE_Send_Data_After_Disconnected );
    #endif
    #if ENABLE_TC_INTEGRATION_CALLBACK_NULL_CHECK
        RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Callback_NULL_Check );
    #endif
    #if ENABLE_TC_INTEGRATION_INIT_ENABLE_TWICE
        RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Init_Enable_Twice );
    #endif
    #if ENABLE_TC_INTEGRATION_CHECK_BOND_STATE
        RUN_TEST_CASE( Full_BLE_Integration_Test_Advertisement, BLE_Check_Bond_State );
    #endif
    #if ENABLE_TC_INTEGRATION_ENABLE_DISABLE_BT_MODULE
        RUN_TEST_CASE( Full_BLE_Integration_Test_common_GATT, BLE_Enable_Disable_BT_Module );
    #endif
    #if ENABLE_TC_INTEGRATION_CHANGE_MTU_SIZE
        RUN_TEST_CASE( Full_BLE_Integration_Test_Advertisement, BLE_Change_MTU_Size )
    #endif

    /*TODO: Test sequence to back to pxSetAdvData, pxSetScanResponse, pxStartAdv()*/
    /* RUN_TEST_CASE( Full_BLE_Integration_Test, BLE_Advertise_Before_Set_Data ); */

    RUN_TEST_CASE( Full_BLE, BLE_Free );
}

#if ENABLE_TC_INTEGRATION_CHANGE_MTU_SIZE
    TEST( Full_BLE_Integration_Test_Advertisement, BLE_Change_MTU_Size )
    {
        BTStatus_t xStatus;
        BTProperty_t pxProperty;
        BLETESTMtuChangedCallback_t xMtuChangedEvent;
        uint16_t usMTUsize = IOT_BLE_PREFERRED_MTU_SIZE;

        IotTestBleHal_SetAdvProperty();

        xStatus = _pxGattServerInterface->pxConfigureMtu( _ucBLEServerIf, usMTUsize );

        IotTestBleHal_SetAdvData( eBTuuidType128, 0, NULL, 0, NULL );
        IotTestBleHal_StartAdvertisement();
        IotTestBleHal_WaitConnection( true );

        xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventMtuChangedCb, NO_HANDLE, ( void * ) &xMtuChangedEvent, sizeof( BLETESTMtuChangedCallback_t ), BLE_TESTS_WAIT );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
        TEST_ASSERT_EQUAL( _usBLEConnId, xMtuChangedEvent.usConnId );
        TEST_ASSERT_EQUAL( usMTUsize, xMtuChangedEvent.usMtu );

        IotTestBleHal_WaitConnection( false );
    }
#endif /* if ENABLE_TC_INTEGRATION_CHANGE_MTU_SIZE */

#if ENABLE_TC_INTEGRATION_CHECK_BOND_STATE
    TEST( Full_BLE_Integration_Test_Advertisement, BLE_Check_Bond_State )
    {
        BTStatus_t xStatus;
        BLETESTsspRequestCallback_t xSSPrequestEvent;
        BLETESTPairingStateChangedCallback_t xPairingStateChangedEvent;

        IotTestBleHal_SetAdvProperty();
        IotTestBleHal_SetAdvData( eBTuuidType128, 0, NULL, 0, NULL );
        IotTestBleHal_StartAdvertisement();
        IotTestBleHal_WaitConnection( true );

        IotTestBleHal_CreateSecureConnection_Model1Level4( false );

        IotTestBleHal_WaitConnection( false );
    }
#endif /* if ENABLE_TC_INTEGRATION_CHECK_BOND_STATE */

#if ENABLE_TC_INTEGRATION_ADD_CHARACTERISTIC_IN_CALLBACK
    TEST( Full_BLE_Integration_Test_common_GATT, BLE_Add_Characteristic_In_Callback )
    {
        BTStatus_t xStatus;

        bCharAddedComplete = false;

        IotTestBleHal_BLEGAPInit( &_xBTBleAdapterCb, true );
        IotTestBleHal_BLEGATTInit( &_xBTGattServer_Nested_Cb, true );

        xStatus = _pxGattServerInterface->pxAddServiceBlob( _ucBLEServerIf, &_xSrvcB );

        if( xStatus != eBTStatusUnsupported )
        {
            TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
        }
        else
        {
            /* Create service B */
            IotTestBleHal_CreateServiceB_Nested();
            /* Start service B */
            IotTestBleHal_StartService( &_xSrvcB );
        }

        IotTestBleHal_StopService( &_xSrvcB );
        IotTestBleHal_DeleteService( &_xSrvcB );

        IotTestBleHal_BTUnregister();
    }
#endif /* if ENABLE_TC_INTEGRATION_ADD_CHARACTERISTIC_IN_CALLBACK */

#if ENABLE_TC_INTEGRATION_CALLBACK_NULL_CHECK
    TEST( Full_BLE_Integration_Test, BLE_Callback_NULL_Check )
    {
        BTStatus_t xStatus;

        /* Initialize with NULL Cb */
        prvInitWithNULLCb();

        /* Create and Start Service */
        prvCreateStartServicesWithNULLCb();

        /* Set Adv property */
        prvSetAdvPropertyWithNULLCb();

        /* Remove all bonds */
        prvRemoveAllBondWithNULLCb();


        /* Set Adv Data */
        prvSetAdvDataWithNULLCb( eBTuuidType128, 0, NULL );

        /* Start Advertisement */
        prvStartStopAdvertisementWithNULLCb();

        /* Deinitialize */
        prvStopServiceWithNULLCb( &_xSrvcA );
        prvStopServiceWithNULLCb( &_xSrvcB );
        prvDeleteServiceWithNULLCb( &_xSrvcA );
        prvDeleteServiceWithNULLCb( &_xSrvcB );
        prvBTUnregisterWithNULLCb();
        xStatus = _pxBTInterface->pxDisable();
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
        xStatus = _pxBTInterface->pxBtManagerCleanup();
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
        IotTestBleHal_ClearEventQueue();
    }
#endif /* if ENABLE_TC_INTEGRATION_CALLBACK_NULL_CHECK */

/* Advertisement should work without initializing optional properties (device's name) */
#if ENABLE_TC_INTEGRATION_ADVERTISE_WITHOUT_PROPERTY
    TEST( Full_BLE_Integration_Test_Advertisement, BLE_Advertise_Without_Properties )
    {
        IotTestBleHal_SetAdvData( eBTuuidType128, 0, NULL, 0, NULL );
        IotTestBleHal_StartAdvertisement();
        /* Connect for evaluate KPI for next test case. */
        IotTestBleHal_WaitConnection( true );
        /* Disconnect */
        IotTestBleHal_WaitConnection( false );
    }
#endif

/* Advertisement should work with 16bit Service UUID as well */
#if ENABLE_TC_INTEGRATION_ADVERTISE_WITH_16BIT_SERVICEUUID
    TEST( Full_BLE_Integration_Test_Advertisement, BLE_Advertise_With_16bit_ServiceUUID )
    {
        IotTestBleHal_SetAdvProperty();
        IotTestBleHal_SetAdvData( eBTuuidType16, 0, NULL, 0, NULL );
        IotTestBleHal_StartAdvertisement();
        /* Simple Connect */
        IotTestBleHal_WaitConnection( true );
        /* Disconnect */
        IotTestBleHal_WaitConnection( false );
    }
#endif /* if ENABLE_TC_INTEGRATION_ADVERTISE_WITH_16BIT_SERVICEUUID */

#if ENABLE_TC_INTEGRATION_ADVERTISE_WITH_MANUFACTUREDATA
    TEST( Full_BLE_Integration_Test_Advertisement, BLE_Advertise_With_ManufactureData )
    {
        /* Manufacturer-specific Data
         * First two bytes are company ID (randomly select Espressif(0x02E5) for test purpose)
         * Next bytes are defined by the company (randomly select 0x05 for test purpose)*/
        uint16_t usManufacturerLen = bletests_MANUFACTURERDATA_LEN;
        uint8_t pusManufacturerData[ bletests_MANUFACTURERDATA_LEN ] = bletests_MANUFACTURERDATA;

        IotTestBleHal_SetAdvProperty();

        /* Check when manufacture data length is 0, but pointer is valid */
        IotTestBleHal_SetAdvData( eBTuuidType16, 0, NULL, 0, ( char * ) pusManufacturerData );
        IotTestBleHal_StartAdvertisement();
        IotTestBleHal_WaitConnection( true );
        IotTestBleHal_WaitConnection( false );

        /* Check when manufacture data pointer is NULL, but length is not 0 */
        IotTestBleHal_SetAdvData( eBTuuidType16, 0, NULL, usManufacturerLen, NULL );
        IotTestBleHal_StartAdvertisement();
        IotTestBleHal_WaitConnection( true );
        IotTestBleHal_WaitConnection( false );

        /* Check when manufacture data length is not 0, and pointer is valid */
        IotTestBleHal_SetAdvData( eBTuuidType16, 0, NULL, usManufacturerLen, ( char * ) pusManufacturerData );
        IotTestBleHal_StartAdvertisement();
        IotTestBleHal_WaitConnection( true );
        IotTestBleHal_WaitConnection( false );
    }
#endif /* if ENABLE_TC_INTEGRATION_ADVERTISE_WITH_MANUFACTUREDATA */

#if ENABLE_TC_INTEGRATION_ADVERTISE_WITH_SERVICEDATA
    TEST( Full_BLE_Integration_Test_Advertisement, BLE_Advertise_With_ServiceData )
    {
        /* Service Data,
         * First two bytes are 16bit Service UUID (randomly select 0xEF12 for test purpose)
         * Next bytes are Service Data (randomly select 0xD6 for test purpose)*/
        uint16_t usServiceDataLen = bletests_SERVICEDATA_LEN;
        uint8_t pusServiceData[ bletests_SERVICEDATA_LEN ] = bletests_SERVICEDATA;

        IotTestBleHal_SetAdvProperty();

        /* Check when service data length is 0, but pointer is valid */
        IotTestBleHal_SetAdvData( eBTuuidType16, 0, ( char * ) pusServiceData, 0, NULL );
        IotTestBleHal_StartAdvertisement();
        IotTestBleHal_WaitConnection( true );
        IotTestBleHal_WaitConnection( false );

        /* Check when service data pointer is NULL, but length is not 0 */
        IotTestBleHal_SetAdvData( eBTuuidType16, usServiceDataLen, NULL, 0, NULL );
        IotTestBleHal_StartAdvertisement();
        IotTestBleHal_WaitConnection( true );
        IotTestBleHal_WaitConnection( false );

        /* Check when service data length is not 0, and pointer is NULL */
        IotTestBleHal_SetAdvData( eBTuuidType16, usServiceDataLen, ( char * ) pusServiceData, 0, NULL );
        IotTestBleHal_StartAdvertisement();
        IotTestBleHal_WaitConnection( true );
        IotTestBleHal_WaitConnection( false );
    }
#endif /* if ENABLE_TC_INTEGRATION_ADVERTISE_WITH_SERVICEDATA */

/* The sequence of set advertisement data and start advertisement can change. */
TEST( Full_BLE_Integration_Test, BLE_Advertise_Before_Set_Data )
{
    IotTestBleHal_StartAdvertisement();
    IotTestBleHal_SetAdvData( eBTuuidType128, 0, NULL, 0, NULL );
    BTStatus_t xStatus = _pxBTLeAdapterInterface->pxStopAdv( _ucBLEAdapterIf );

    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

#if ENABLE_TC_INTEGRATION_INIT_ENABLE_TWICE
    TEST( Full_BLE_Integration_Test, BLE_Init_Enable_Twice )
    {
        BTStatus_t xStatus = eBTStatusSuccess;

        IotTestBleHal_BLEManagerInit( &_xBTManagerCb );
        IotTestBleHal_BLEEnable( true );

        /* First time disable */
        IotTestBleHal_BLEEnable( false );

        /*First time Deinit*/
        xStatus = _pxBTInterface->pxBtManagerCleanup();
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

        /* Second time init */
        IotTestBleHal_BLEManagerInit( &_xBTManagerCb );
        IotTestBleHal_BLEEnable( true );

        GAP_common_teardown();
    }
#endif /* if ENABLE_TC_INTEGRATION_INIT_ENABLE_TWICE */

/* pxEnable/pxDisable can only return current. Make sure pxEnable/pxDisable works.
 * Make sure stack state is enable (callback received) no later than 2.5 seconds after pxEnable returns*/
#if ENABLE_TC_INTEGRATION_ENABLE_DISABLE_BT_MODULE
    TEST( Full_BLE_Integration_Test_common_GATT, BLE_Enable_Disable_BT_Module )
    {
        BTStatus_t xStatus = eBTStatusSuccess;
        BLETESTInitDeinitCallback_t xInitDeinitCb;
        uint64_t xReturnTime, xCbRecvTime;

        /* disable */
        IotTestBleHal_BLEEnable( false );

        /* enable */
        xStatus = _pxBTInterface->pxEnable( 0 );
        xReturnTime = IotClock_GetTimeMs();
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

        xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventEnableDisableCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
        xCbRecvTime = IotClock_GetTimeMs();
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
        TEST_ASSERT_EQUAL( eBTstateOn, xInitDeinitCb.xBLEState );
        TEST_ASSERT_LESS_THAN( BLE_TIME_LIMIT, xCbRecvTime - xReturnTime );
    }
#endif /* if ENABLE_TC_INTEGRATION_ENABLE_DISABLE_BT_MODULE */

/*Advertisement interval measured OTA can be out the range set by app, after reset BT stack, adv interval can change to 1.28s.
 * Make sure KPI is consistent after reset BT.*/
#if ENABLE_TC_INTEGRATION_ADVERTISE_INTERVAL_CONSISTENT_AFTER_BT_RESET
    TEST( Full_BLE_Integration_Test_Advertisement, BLE_Advertise_Interval_Consistent_After_BT_Reset )
    {
        BTStatus_t xStatus = eBTStatusSuccess;

        IotTestBleHal_SetAdvData( eBTuuidType128, 0, NULL, 0, NULL );
        IotTestBleHal_StartAdvertisement();

        IotTestBleHal_WaitConnection( true );
        /* Second time reconnection. Got Second KPI. */
        IotTestBleHal_WaitConnection( false );

        IotTestBleHal_BTUnregister();
        IotTestBleHal_BLEEnable( false );

        xStatus = _pxBTInterface->pxBtManagerCleanup();
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

        xStatus = _pxBTInterface->pxBtManagerInit( &_xBTManagerCb );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

        IotTestBleHal_BLEEnable( true );
        IotTestBleHal_BLEGAPInit( &_xBTBleAdapterCb, true );
        IotTestBleHal_BLEGATTInit( &_xBTGattServerCb, true );
        IotTestBleHal_CreateStartServiceB( false );
        IotTestBleHal_SetAdvProperty();
        IotTestBleHal_SetAdvData( eBTuuidType128, 0, NULL, 0, NULL );

        /* Third time connection begins. Got third KPI. */
        IotTestBleHal_StartAdvertisement();

        IotTestBleHal_WaitConnection( true );

        /* Result is on RPI. Write it back to device. */
        prvGetResult( bletestATTR_SRVCB_CHAR_D,
                      false,
                      0 );

        /* Disconnect */
        IotTestBleHal_WaitConnection( false );
        IotTestBleHal_StopService( &_xSrvcB );
        IotTestBleHal_DeleteService( &_xSrvcB );
    }
#endif /* if ENABLE_TC_INTEGRATION_ADVERTISE_INTERVAL_CONSISTENT_AFTER_BT_RESET */

#if ENABLE_TC_INTEGRATION_WRITE_NOTIFICATION_SIZE_GREATER_THAN_MTU_3
    TEST( Full_BLE_Integration_Test_Connection, BLE_Write_Notification_Size_Greater_Than_MTU_3 )
    {
        BTStatus_t xStatus;
        BLETESTindicateCallback_t xIndicateEvent;

        IotTestBleHal_checkNotificationIndication( bletestATTR_SRVCB_CCCD_E, true );

        memset( ucLargeBuffer, 'a', ( IOT_BLE_PREFERRED_MTU_SIZE + 2 ) * sizeof( char ) );

        xStatus = _pxGattServerInterface->pxSendIndication( _ucBLEServerIf,
                                                            usHandlesBufferB[ bletestATTR_SRVCB_CHAR_E ],
                                                            _usBLEConnId,
                                                            IOT_BLE_PREFERRED_MTU_SIZE + 2,
                                                            ucLargeBuffer,
                                                            false );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

        xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventIndicateCb, NO_HANDLE, ( void * ) &xIndicateEvent, sizeof( BLETESTindicateCallback_t ), BLE_TESTS_WAIT );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
        TEST_ASSERT_EQUAL( _usBLEConnId, xIndicateEvent.usConnId );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xIndicateEvent.xStatus );

        IotTestBleHal_checkNotificationIndication( bletestATTR_SRVCB_CCCD_E, false );
    }
#endif /* if ENABLE_TC_INTEGRATION_WRITE_NOTIFICATION_SIZE_GREATER_THAN_MTU_3 */

#if ENABLE_TC_INTEGRATION_SEND_DATA_AFTER_DISCONNECTED
    TEST( Full_BLE_Integration_Test_Connection, BLE_Send_Data_After_Disconnected )
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
        IotTestBleHal_SetAdvData( eBTuuidType128, 0, NULL, 0, NULL );
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

        /* unsubscribe notification&indication */
        IotTestBleHal_checkNotificationIndication( bletestATTR_SRVCB_CCCD_E, false );
        IotTestBleHal_checkNotificationIndication( bletestATTR_SRVCB_CCCD_F, false );
    }
#endif /* if ENABLE_TC_INTEGRATION_SEND_DATA_AFTER_DISCONNECTED */

/* trigger Adv Stop callback AdvStartCB(with start=false) when Adv timeout. */
#if ENABLE_TC_INTEGRATION_CONNECTION_TIMEOUT
    TEST( Full_BLE_Integration_Test_Advertisement, BLE_Integration_Connection_Timeout )
    {
        /* Advertise and Connect */
        IotTestBleHal_SetAdvProperty();
        IotTestBleHal_SetAdvData( eBTuuidType16, 0, NULL, 0, NULL );
        IotTestBleHal_StartAdvertisement();
        prvShortWaitConnection();
    }
#endif

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
#if ENABLE_TC_INTEGRATION_CONNECTION_TIMEOUT
    void prvShortWaitConnection()
    {
        BLETESTConnectionCallback_t xConnectionEvent;
        BTStatus_t xStatus;

        xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventConnectionCb, NO_HANDLE, ( void * ) &xConnectionEvent, sizeof( BLETESTConnectionCallback_t ), BLE_TESTS_SHORT_WAIT );
        TEST_ASSERT_EQUAL( eBTStatusFail, xStatus );
        TEST_ASSERT_NOT_EQUAL( eBTStatusSuccess, xConnectionEvent.xStatus );
        IotTestBleHal_StartStopAdvCheck( false );
    }
#endif /* if ENABLE_TC_INTEGRATION_CONNECTION_TIMEOUT */

#if ENABLE_TC_INTEGRATION_CALLBACK_NULL_CHECK
    void prvInitWithNULLCb( void )
    {
        BTStatus_t xStatus;

        /* GAP common setup with NULL Cb */
        IotTestBleHal_BLEManagerInit( &_xBTManager_NULL_Cb );

        xStatus = _pxBTInterface->pxEnable( 0 );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

        /* BLEGAPInit with NULL Cb */
        IotTestBleHal_BLEGAPInit( &_xBTBleAdapter_NULL_Cb, false );

        /* BLEGATTInit with NULL Cb */
        IotTestBleHal_BLEGATTInit( &_xBTGattServer_NULL_Cb, false );
    }

    void prvCreateStartServicesWithNULLCb( void )
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
            /* Create service A */
            IotTestBleHal_CreateServiceA();

            /* Create service B */
            IotTestBleHal_CreateServiceB( false );

            /* Start service A */
            prvStartServiceWithNULLCb( &_xSrvcA );
            /* Start service B */
            prvStartServiceWithNULLCb( &_xSrvcB );
        }
    }

    void prvStartServiceWithNULLCb( BTService_t * xRefSrvc )
    {
        BTStatus_t xStatus = eBTStatusSuccess;

        xStatus = _pxGattServerInterface->pxStartService( _ucBLEServerIf, xRefSrvc->pusHandlesBuffer[ 0 ], BTTransportLe );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    }

    void prvSetAdvPropertyWithNULLCb( void )
    {
        BTProperty_t pxProperty;
        uint16_t usMTUsize = IOT_BLE_PREFERRED_MTU_SIZE;
        BTStatus_t xStatus = eBTStatusSuccess;

        pxProperty.xType = eBTpropertyBdname;
        pxProperty.xLen = strlen( bletestsDEVICE_NAME );
        pxProperty.pvVal = ( void * ) bletestsDEVICE_NAME;

        /* Set the name */
        xStatus = _pxBTInterface->pxSetDeviceProperty( &pxProperty );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

        /* Get the name to check it is set */
        /*@TODO IotTestBleHal_SetGetProperty(&pxProperty, false); */

        pxProperty.xType = eBTpropertyLocalMTUSize;
        pxProperty.xLen = sizeof( usMTUsize );
        pxProperty.pvVal = &usMTUsize;

        /* Set the MTU size */
        xStatus = _pxBTInterface->pxSetDeviceProperty( &pxProperty );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

        /* Get the MTU size to check it is set */
        /*@TODOIotTestBleHal_SetGetProperty(&pxProperty, false); */
    }

    void prvRemoveAllBondWithNULLCb( void )
    {
        BTProperty_t pxProperty;
        uint16_t usIndex;
        BTStatus_t xStatus = eBTStatusSuccess;

        /* Set the name */
        pxProperty.xType = eBTpropertyAdapterBondedDevices;

        /* Get bonded devices */
        IotTestBleHal_SetGetProperty( &pxProperty, false );

        for( usIndex = 0; usIndex < pxProperty.xLen; usIndex++ )
        {
            prvRemoveBondWithNULLCb( &( ( BTBdaddr_t * ) pxProperty.pvVal )[ usIndex ] );
            TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
        }

        /* Get bonded devices. */
        IotTestBleHal_SetGetProperty( &pxProperty, false );
        /* Check none are left. */
        TEST_ASSERT_EQUAL( 0, pxProperty.xLen );
    }

    void prvRemoveBondWithNULLCb( BTBdaddr_t * pxDeviceAddress )
    {
        BTStatus_t xStatus;

        xStatus = _pxBTInterface->pxRemoveBond( pxDeviceAddress );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    }

    void prvSetAdvDataWithNULLCb( BTuuidType_t type,
                                  uint16_t usManufacturerLen,
                                  char * pcManufacturerData )
    {
        uint16_t usServiceDataLen;
        char * pcServiceData;
        uint8_t serviceUUID_128[ bt128BIT_UUID_LEN ] = bletestsFREERTOS_SVC_UUID_128;
        /* To make sure stack creates their own pointers, use local variables */
        BTGattAdvertismentParams_t l_xAdvertisementConfigA;
        BTGattAdvertismentParams_t l_xAdvertisementConfigB;
        size_t xNbServices;

        BTUuid_t xServiceUuid =
        {
            .ucType = type
        };

        switch( type )
        {
            case eBTuuidType16:
                xServiceUuid.uu.uu16 = bletestsFREERTOS_SVC_UUID_16;
                break;

            case eBTuuidType32:
                xServiceUuid.uu.uu32 = bletestsFREERTOS_SVC_UUID_32;
                break;

            case eBTuuidType128:
                memcpy( xServiceUuid.uu.uu128, serviceUUID_128, sizeof( serviceUUID_128 ) );
                break;
        }

        usServiceDataLen = 0;
        pcServiceData = NULL;
        xNbServices = 1;

        l_xAdvertisementConfigA = xAdvertisementConfigA;
        l_xAdvertisementConfigB = xAdvertisementConfigB;

        prvSetAdvertisementWithNULLCb( &l_xAdvertisementConfigA,
                                       usServiceDataLen,
                                       pcServiceData,
                                       &xServiceUuid,
                                       xNbServices,
                                       usManufacturerLen,
                                       pcManufacturerData );

        prvSetAdvertisementWithNULLCb( &l_xAdvertisementConfigB,
                                       usServiceDataLen,
                                       pcServiceData,
                                       NULL,
                                       0,
                                       usManufacturerLen,
                                       pcManufacturerData );
    }

    void prvSetAdvertisementWithNULLCb( BTGattAdvertismentParams_t * pxParams,
                                        uint16_t usServiceDataLen,
                                        char * pcServiceData,
                                        BTUuid_t * pxServiceUuid,
                                        size_t xNbServices,
                                        uint16_t usManufacturerLen,
                                        char * pcManufacturerData )
    {
        BTStatus_t xStatus = eBTStatusSuccess;

        xStatus = _pxBTLeAdapterInterface->pxSetAdvData( _ucBLEAdapterIf,
                                                         pxParams,
                                                         usManufacturerLen,
                                                         pcManufacturerData,
                                                         usServiceDataLen,
                                                         pcServiceData,
                                                         pxServiceUuid,
                                                         xNbServices );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    }

    void prvStartStopAdvertisementWithNULLCb( void )
    {
        BLETESTConnectionCallback_t xConnectionEvent;
        BTStatus_t xStatus = eBTStatusSuccess;

        xStatus = _pxBTLeAdapterInterface->pxStartAdv( _ucBLEAdapterIf );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

        xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventConnectionCb, NO_HANDLE, ( void * ) &xConnectionEvent, sizeof( BLETESTConnectionCallback_t ), BLE_TESTS_WAIT );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
        TEST_ASSERT_EQUAL( true, xConnectionEvent.bConnected );

        xStatus = _pxBTLeAdapterInterface->pxStopAdv( _ucBLEAdapterIf );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

        xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventConnectionCb, NO_HANDLE, ( void * ) &xConnectionEvent, sizeof( BLETESTConnectionCallback_t ), BLE_TESTS_WAIT );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
        TEST_ASSERT_EQUAL( false, xConnectionEvent.bConnected );
    }

    void prvStopServiceWithNULLCb( BTService_t * xRefSrvc )
    {
        BTStatus_t xStatus = eBTStatusSuccess;

        xStatus = _pxGattServerInterface->pxStopService( _ucBLEServerIf, xRefSrvc->pusHandlesBuffer[ 0 ] );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    }

    void prvDeleteServiceWithNULLCb( BTService_t * xRefSrvc )
    {
        BTStatus_t xStatus = eBTStatusSuccess;

        xStatus = _pxGattServerInterface->pxDeleteService( _ucBLEServerIf, xRefSrvc->pusHandlesBuffer[ 0 ] );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    }

    void prvBTUnregisterWithNULLCb( void )
    {
        BTStatus_t xStatus = eBTStatusSuccess;

        xStatus = _pxGattServerInterface->pxUnregisterServer( _ucBLEServerIf );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

        xStatus = _pxBTLeAdapterInterface->pxUnregisterBleApp( _ucBLEAdapterIf );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    }
#endif /* if ENABLE_TC_INTEGRATION_CALLBACK_NULL_CHECK */

void GAP_common_teardown()
{
    BTStatus_t xStatus = eBTStatusSuccess;

    /* Disable */
    IotTestBleHal_BLEEnable( false );

    /* Deinit */
    xStatus = _pxBTInterface->pxBtManagerCleanup();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    IotTestBleHal_ClearEventQueue();
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
    IotTestBleHal_BLEGAPInit( &_xBTBleAdapterCb, true );
    IotTestBleHal_BLEGATTInit( &_xBTGattServerCb, true );
}
