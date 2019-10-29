/*
 * Amazon FreeRTOS BLE HAL V2.0.0
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
 * @file aws_test_ble.c
 * @brief Tests for ble.
 */
#include "iot_test_ble_hal_afqp.h"

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

const uint32_t bletestWAIT_MODE1_LEVEL2_QUERY = 10000; /* Wait 10s max */

IotBleConnectionParam_t xConnectionParamA =
{
    .minInterval = bletestsMIN_CONNECTION_INTERVAL,
    .maxInterval = bletestsMAX_CONNECTION_INTERVAL,
    .latency     = 4,
    .timeout     = 400
};



/*-----------------------------------------------------------*/

TEST_GROUP( Full_BLE );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_BLE )
{
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_BLE )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_BLE )
{
    /* Initializations that need to be done once for all the tests. */

    RUN_TEST_CASE( Full_BLE, BLE_Setup );
    RUN_TEST_CASE( Full_BLE, BLE_Initialize_common_GAP );
    RUN_TEST_CASE( Full_BLE, BLE_Initialize_BLE_GAP );
    RUN_TEST_CASE( Full_BLE, BLE_Initialize_BLE_GATT );


    RUN_TEST_CASE( Full_BLE, BLE_CreateAttTable_CreateServices );

    RUN_TEST_CASE( Full_BLE, BLE_Advertising_SetProperties ); /*@TOTO, incomplete */
    RUN_TEST_CASE( Full_BLE, BLE_Connection_RemoveAllBonds );

    RUN_TEST_CASE( Full_BLE, BLE_Advertising_SetAvertisementData ); /*@TOTO, incomplete */
    RUN_TEST_CASE( Full_BLE, BLE_Advertising_StartAdvertisement );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_SimpleConnection );
/*RUN_TEST_CASE( Full_BLE, BLE_Connection_UpdateConnectionParamReq ); */

/*RUN_TEST_CASE( Full_BLE, BLE_Connection_ChangeMTUsize ); */
    RUN_TEST_CASE( Full_BLE, BLE_Property_WriteCharacteristic );
    RUN_TEST_CASE( Full_BLE, BLE_Property_WriteDescriptor );
    RUN_TEST_CASE( Full_BLE, BLE_Property_ReadCharacteristic );
    RUN_TEST_CASE( Full_BLE, BLE_Property_ReadDescriptor );
    RUN_TEST_CASE( Full_BLE, BLE_Property_WriteNoResponse );
    RUN_TEST_CASE( Full_BLE, BLE_Property_Enable_Indication_Notification );
    RUN_TEST_CASE( Full_BLE, BLE_Property_Notification );
    RUN_TEST_CASE( Full_BLE, BLE_Property_Indication );
    RUN_TEST_CASE( Full_BLE, BLE_Property_Disable_Indication_Notification );

    RUN_TEST_CASE( Full_BLE, BLE_Connection_Mode1Level4 );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_Mode1Level4_Property_WriteDescr );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_Mode1Level4_Property_WriteChar );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_Disconnect );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_BondedReconnectAndPair );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_Disconnect );

    RUN_TEST_CASE( Full_BLE, BLE_Connection_CheckBonding );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_RemoveBonding );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_Mode1Level2 );

    RUN_TEST_CASE( Full_BLE, BLE_DeInitialize );
    RUN_TEST_CASE( Full_BLE, BLE_Free );
}

void prvRemoveBond( BTBdaddr_t * pxDeviceAddress )
{
    BTStatus_t xStatus;
    BLETESTBondedCallback_t xBondedEvent;

    xStatus = _pxBTInterface->pxRemoveBond( pxDeviceAddress );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventBondedCb, NO_HANDLE, ( void * ) &xBondedEvent, sizeof( BLETESTBondedCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xBondedEvent.xStatus );
    TEST_ASSERT_EQUAL( false, xBondedEvent.bIsBonded );
    TEST_ASSERT_EQUAL( 0, memcmp( &xBondedEvent.xRemoteBdAddr, pxDeviceAddress, sizeof( BTBdaddr_t ) ) );
}

TEST( Full_BLE, BLE_Setup )
{
    IotTestBleHal_BLESetUp();
}

TEST( Full_BLE, BLE_Free )
{
    IotTestBleHal_BLEFree();
}

TEST( Full_BLE, BLE_Connection_RemoveAllBonds )
{
    BTProperty_t pxProperty;
    uint16_t usIndex;

    /* Set the name */
    pxProperty.xType = eBTpropertyAdapterBondedDevices;

    /* Get bonded devices */
    prvSetGetProperty( &pxProperty, false );

    for( usIndex = 0; usIndex < pxProperty.xLen; usIndex++ )
    {
        prvRemoveBond( &( ( BTBdaddr_t * ) pxProperty.pvVal )[ usIndex ] );
    }

    /* Get bonded devices. */
    prvSetGetProperty( &pxProperty, false );
    /* Check none are left. */
    TEST_ASSERT_EQUAL( 0, pxProperty.xLen );
}

bool prvGetCheckDeviceBonded( BTBdaddr_t * pxDeviceAddress )
{
    BTProperty_t pxProperty;
    uint16_t usIndex;
    bool bFoundRemoteDevice = false;

    pxProperty.xType = eBTpropertyAdapterBondedDevices;

    /* Set the name */
    prvSetGetProperty( &pxProperty, false );

    for( usIndex = 0; usIndex < pxProperty.xLen; usIndex++ )
    {
        if( 0 == memcmp( &( ( BTBdaddr_t * ) pxProperty.pvVal )[ usIndex ], pxDeviceAddress, sizeof( BTBdaddr_t ) ) )
        {
            bFoundRemoteDevice = true;
            break;
        }
    }

    return bFoundRemoteDevice;
}

void IotTestBleHal_WaitConnection( bool bConnected )
{
    BLETESTConnectionCallback_t xConnectionEvent;
    BTStatus_t xStatus;

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventConnectionCb, NO_HANDLE, ( void * ) &xConnectionEvent, sizeof( BLETESTConnectionCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( bConnected, xConnectionEvent.bConnected );
    TEST_ASSERT_EQUAL( _ucBLEServerIf, xConnectionEvent.ucServerIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xConnectionEvent.xStatus );

    /* Stop advertisement. */
    xStatus = _pxBTLeAdapterInterface->pxStopAdv( _ucBLEAdapterIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    IotTestBleHal_StartStopAdvCheck( false );
}

TEST( Full_BLE, BLE_Connection_Mode1Level2 )
{
    BTStatus_t xStatus;
    BLETESTPairingStateChangedCallback_t xPairingStateChangedEvent;
    BLETESTsspRequestCallback_t xSSPrequestEvent;

    IotTestBleHal_StartAdvertisement();
    IotTestBleHal_WaitConnection( true );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventSSPrequestCb, NO_HANDLE, ( void * ) &xSSPrequestEvent, sizeof( BLETESTsspRequestCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xSSPrequestEvent.xRemoteBdAddr, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( eBTsspVariantConsent, xSSPrequestEvent.xPairingVariant );

    xStatus = _pxBTInterface->pxSspReply( &xSSPrequestEvent.xRemoteBdAddr, eBTsspVariantConsent, true, 0 );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventPairingStateChangedCb, NO_HANDLE, ( void * ) &xPairingStateChangedEvent, sizeof( BLETESTPairingStateChangedCallback_t ), bletestWAIT_MODE1_LEVEL2_QUERY );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );                        /* Pairing should never come since it is secure connection only */
    TEST_ASSERT_EQUAL( eBTStatusFail, xPairingStateChangedEvent.xStatus ); /* Pairing should never come since it is secure connection only */
    /* @TODO add correct flag */
    /* TEST_ASSERT_EQUAL(eBTauthFailInsuffSecurity, xPairingStateChangedEvent.xReason); */ /* Pairing should never come since it is secure connection only */
}

TEST( Full_BLE, BLE_Connection_RemoveBonding )
{
    bool bFoundRemoteDevice;

    prvRemoveBond( &_xAddressConnectedDevice );

    bFoundRemoteDevice = prvGetCheckDeviceBonded( &_xAddressConnectedDevice );
    TEST_ASSERT_EQUAL( false, bFoundRemoteDevice );
}


TEST( Full_BLE, BLE_Connection_CheckBonding )
{
    bool bFoundRemoteDevice;

    bFoundRemoteDevice = prvGetCheckDeviceBonded( &_xAddressConnectedDevice );
    TEST_ASSERT_EQUAL( true, bFoundRemoteDevice );
}



TEST( Full_BLE, BLE_Connection_BondedReconnectAndPair )
{
    BTStatus_t xStatus;
    BLETESTPairingStateChangedCallback_t xPairingStateChangedEvent;
    BLETESTBondedCallback_t xBondedEvent;

    IotTestBleHal_StartAdvertisement();
    IotTestBleHal_WaitConnection( true );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventPairingStateChangedCb, NO_HANDLE, ( void * ) &xPairingStateChangedEvent, sizeof( BLETESTPairingStateChangedCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xPairingStateChangedEvent.xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xPairingStateChangedEvent.xRemoteBdAddr, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( eBTSecLevelSecureConnect, xPairingStateChangedEvent.xSecurityLevel );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventBondedCb, NO_HANDLE, ( void * ) &xBondedEvent, sizeof( BLETESTBondedCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xBondedEvent.xStatus );
    TEST_ASSERT_EQUAL( true, xBondedEvent.bIsBonded );
    TEST_ASSERT_EQUAL( 0, memcmp( &xBondedEvent.xRemoteBdAddr, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );

    prvWriteCheckAndResponse( bletestATTR_SRVCB_CHAR_B,
                              true,
                              false,
                              0 );
}


TEST( Full_BLE, BLE_Connection_Disconnect )
{
    IotTestBleHal_WaitConnection( false );
}

TEST( Full_BLE, BLE_Connection_Mode1Level4_Property_WriteDescr )
{
    prvWriteCheckAndResponse( bletestATTR_SRVCB_CHARF_DESCR_B,
                              true,
                              false,
                              0 );
}

TEST( Full_BLE, BLE_Connection_Mode1Level4_Property_WriteChar )
{
    prvWriteCheckAndResponse( bletestATTR_SRVCB_CHAR_B,
                              true,
                              false,
                              0 );
}

TEST( Full_BLE, BLE_Connection_Mode1Level4 )
{
    BTStatus_t xStatus;
    BLETESTsspRequestCallback_t xSSPrequestEvent;
    BLETESTPairingStateChangedCallback_t xPairingStateChangedEvent;
    BLETESTBondedCallback_t xBondedEvent;

    /* Wait secure connection. Secure connection is triggered by writting to bletestsCHARB. */
    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventSSPrequestCb, NO_HANDLE, ( void * ) &xSSPrequestEvent, sizeof( BLETESTsspRequestCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xSSPrequestEvent.xRemoteBdAddr, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( eBTsspVariantConsent, xSSPrequestEvent.xPairingVariant );

    xStatus = _pxBTInterface->pxSspReply( &xSSPrequestEvent.xRemoteBdAddr, eBTsspVariantConsent, true, 0 );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventSSPrequestConfirmationCb, NO_HANDLE, ( void * ) &xSSPrequestEvent, sizeof( BLETESTsspRequestCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xSSPrequestEvent.xRemoteBdAddr, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( eBTsspVariantPasskeyConfirmation, xSSPrequestEvent.xPairingVariant );

    xStatus = _pxBTInterface->pxSspReply( &xSSPrequestEvent.xRemoteBdAddr, eBTsspVariantPasskeyConfirmation, true, 0 );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventPairingStateChangedCb, NO_HANDLE, ( void * ) &xPairingStateChangedEvent, sizeof( BLETESTPairingStateChangedCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xPairingStateChangedEvent.xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xPairingStateChangedEvent.xRemoteBdAddr, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( eBTSecLevelSecureConnect, xPairingStateChangedEvent.xSecurityLevel );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventBondedCb, NO_HANDLE, ( void * ) &xBondedEvent, sizeof( BLETESTBondedCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xBondedEvent.xStatus );
    TEST_ASSERT_EQUAL( true, xBondedEvent.bIsBonded );
    TEST_ASSERT_EQUAL( 0, memcmp( &xBondedEvent.xRemoteBdAddr, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );

    prvWriteCheckAndResponse( bletestATTR_SRVCB_CHAR_B,
                              true,
                              false,
                              0 );
}

void prvWriteCheckAndResponse( bletestAttSrvB_t xAttribute,
                               bool bNeedRsp,
                               bool IsPrep,
                               uint16_t usOffset )
{
    BLETESTwriteAttrCallback_t xWriteEvent;

    xWriteEvent = IotTestBleHal_WriteReceive( xAttribute, bNeedRsp, IsPrep, usOffset );

    if( bNeedRsp == true )
    {
        IotTestBleHal_WriteResponse( xAttribute, xWriteEvent, true );
    }
}

BLETESTwriteAttrCallback_t IotTestBleHal_WriteReceive( bletestAttSrvB_t xAttribute,
                                                       bool bNeedRsp,
                                                       bool IsPrep,
                                                       uint16_t usOffset )
{
    BLETESTwriteAttrCallback_t xWriteEvent;
    BTStatus_t xStatus;

    /* Wait write event on char A*/
    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventWriteAttrCb, usHandlesBufferB[ xAttribute ], ( void * ) &xWriteEvent, sizeof( BLETESTwriteAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( IsPrep, xWriteEvent.bIsPrep );
    TEST_ASSERT_EQUAL( bNeedRsp, xWriteEvent.bNeedRsp );
    TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xWriteEvent.usAttrHandle );
    TEST_ASSERT_EQUAL( _usBLEConnId, xWriteEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, memcmp( &xWriteEvent.xBda, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( usOffset, xWriteEvent.usOffset );
    TEST_ASSERT_EQUAL( bletestsSTRINGYFIED_UUID_SIZE, xWriteEvent.xLength );

    return xWriteEvent;
}

void IotTestBleHal_WriteResponse( bletestAttSrvB_t xAttribute,
                                  BLETESTwriteAttrCallback_t xWriteEvent,
                                  bool IsConnected )
{
    BLETESTconfirmCallback_t xConfirmEvent;
    BTGattResponse_t xGattResponse;
    BTStatus_t xStatus;

    xGattResponse.usHandle = xWriteEvent.usAttrHandle;
    xGattResponse.xAttrValue.usHandle = xWriteEvent.usAttrHandle;
    xGattResponse.xAttrValue.usOffset = xWriteEvent.usOffset;
    xGattResponse.xAttrValue.xLen = xWriteEvent.xLength;
    ucRespBuffer[ xAttribute ].xLength = xWriteEvent.xLength;
    memcpy( ucRespBuffer[ xAttribute ].ucBuffer, xWriteEvent.ucValue, xWriteEvent.xLength );
    xGattResponse.xAttrValue.pucValue = ucRespBuffer[ xAttribute ].ucBuffer;
    xStatus = _pxGattServerInterface->pxSendResponse( xWriteEvent.usConnId, xWriteEvent.ulTransId, eBTStatusSuccess, &xGattResponse );


    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventConfimCb, xWriteEvent.usAttrHandle, ( void * ) &xConfirmEvent, sizeof( BLETESTconfirmCallback_t ), BLE_TESTS_WAIT );

    if( IsConnected == true )
    {
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xConfirmEvent.xStatus );
        TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xConfirmEvent.usAttrHandle );
    }
    else
    {
        TEST_ASSERT_NOT_EQUAL( eBTStatusSuccess, xConfirmEvent.xStatus );
    }
}

void IotTestBleHal_checkNotificationIndication( bletestAttSrvB_t xAttribute,
                                                bool enable )
{
    BTGattResponse_t xGattResponse;
    BTStatus_t xStatus;
    BLETESTwriteAttrCallback_t xWriteEvent;
    BLETESTconfirmCallback_t xConfirmEvent;

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventWriteAttrCb, usHandlesBufferB[ xAttribute ], ( void * ) &xWriteEvent, sizeof( BLETESTwriteAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xWriteEvent.usAttrHandle );
    TEST_ASSERT_EQUAL( _usBLEConnId, xWriteEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, memcmp( &xWriteEvent.xBda, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );

    /* for Indication or Notification a flag on the first byte is toggled .*/
    if( enable == true )
    {
        TEST_ASSERT_NOT_EQUAL( 0, xWriteEvent.ucValue[ 0 ] );
    }
    else
    {
        TEST_ASSERT_EQUAL( 0, xWriteEvent.ucValue[ 0 ] );
    }

    xGattResponse.usHandle = xWriteEvent.usAttrHandle;
    xGattResponse.xAttrValue.usHandle = xWriteEvent.usAttrHandle;
    xGattResponse.xAttrValue.usOffset = 0;
    xGattResponse.xAttrValue.xLen = xWriteEvent.xLength;
    xGattResponse.xAttrValue.pucValue = xWriteEvent.ucValue;
    _pxGattServerInterface->pxSendResponse( xWriteEvent.usConnId, xWriteEvent.ulTransId, eBTStatusSuccess, &xGattResponse );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventConfimCb, xWriteEvent.usAttrHandle, ( void * ) &xConfirmEvent, sizeof( BLETESTconfirmCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xConfirmEvent.xStatus );
    TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xConfirmEvent.usAttrHandle );
}

TEST( Full_BLE, BLE_Property_Enable_Indication_Notification )
{
    IotTestBleHal_checkNotificationIndication( bletestATTR_SRVCB_CCCD_E, true );
    IotTestBleHal_checkNotificationIndication( bletestATTR_SRVCB_CCCD_F, true );
}

TEST( Full_BLE, BLE_Property_Disable_Indication_Notification )
{
    IotTestBleHal_checkNotificationIndication( bletestATTR_SRVCB_CCCD_E, false );
    IotTestBleHal_checkNotificationIndication( bletestATTR_SRVCB_CCCD_F, false );
}


TEST( Full_BLE, BLE_Property_Indication )
{
    IotTestBleHal_CheckIndicationNotification( true, true );
}

TEST( Full_BLE, BLE_Property_Notification )
{
    IotTestBleHal_CheckIndicationNotification( false, true );
}

TEST( Full_BLE, BLE_Property_WriteNoResponse )
{
    prvWriteCheckAndResponse( bletestATTR_SRVCB_CHAR_D,
                              false,
                              false,
                              0 );
}

void prvReadCheckAndResponse( bletestAttSrvB_t xAttribute )
{
    BLETESTreadAttrCallback_t xReadEvent;

    xReadEvent = IotTestBleHal_ReadReceive( xAttribute );
    IotTestBleHal_ReadResponse( xAttribute, xReadEvent, true );
}

BLETESTreadAttrCallback_t IotTestBleHal_ReadReceive( bletestAttSrvB_t xAttribute )
{
    BLETESTreadAttrCallback_t xReadEvent;
    BTStatus_t xStatus;

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventReadAttrCb, usHandlesBufferB[ xAttribute ], ( void * ) &xReadEvent, sizeof( BLETESTreadAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xReadEvent.usAttrHandle );
    TEST_ASSERT_EQUAL( _usBLEConnId, xReadEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, xReadEvent.usOffset );
    TEST_ASSERT_EQUAL( 0, memcmp( &xReadEvent.xBda, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );

    return xReadEvent;
}

void IotTestBleHal_ReadResponse( bletestAttSrvB_t xAttribute,
                                 BLETESTreadAttrCallback_t xReadEvent,
                                 bool IsConnected )
{
    BTGattResponse_t xGattResponse;
    BLETESTconfirmCallback_t xConfirmEvent;
    BTStatus_t xStatus;

    xGattResponse.usHandle = xReadEvent.usAttrHandle;
    xGattResponse.xAttrValue.usHandle = xReadEvent.usAttrHandle;
    xGattResponse.xAttrValue.usOffset = xReadEvent.usOffset;
    xGattResponse.xAttrValue.xLen = ucRespBuffer[ xAttribute ].xLength;
    xGattResponse.xAttrValue.pucValue = ucRespBuffer[ xAttribute ].ucBuffer;
    _pxGattServerInterface->pxSendResponse( xReadEvent.usConnId, xReadEvent.ulTransId, eBTStatusSuccess, &xGattResponse );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventConfimCb, usHandlesBufferB[ xAttribute ], ( void * ) &xConfirmEvent, sizeof( BLETESTconfirmCallback_t ), BLE_TESTS_WAIT );

    if( IsConnected == true )
    {
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xConfirmEvent.xStatus );
        TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xConfirmEvent.usAttrHandle );
    }
    else
    {
        TEST_ASSERT_NOT_EQUAL( eBTStatusSuccess, xConfirmEvent.xStatus );
    }
}

TEST( Full_BLE, BLE_Property_ReadDescriptor )
{
    prvReadCheckAndResponse( bletestATTR_SRVCB_CHARF_DESCR_A );
}

TEST( Full_BLE, BLE_Property_ReadCharacteristic )
{
    prvReadCheckAndResponse( bletestATTR_SRVCB_CHAR_A );
}

TEST( Full_BLE, BLE_Property_WriteDescriptor )
{
    prvWriteCheckAndResponse( bletestATTR_SRVCB_CHARF_DESCR_A,
                              true,
                              false,
                              0 );
}

TEST( Full_BLE, BLE_Property_WriteCharacteristic )
{
    prvWriteCheckAndResponse( bletestATTR_SRVCB_CHAR_A,
                              true,
                              false,
                              0 );
}

TEST( Full_BLE, BLE_Connection_ChangeMTUsize )
{
}

TEST( Full_BLE, BLE_Connection_UpdateConnectionParamReq )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTUpdateConnectionParamReqCallback_t xUpdateConnectionParamReq;

    xStatus = _pxBTLeAdapterInterface->pxConnParameterUpdateRequest( &_xAddressConnectedDevice,
                                                                     xConnectionParamA.minInterval,
                                                                     xConnectionParamA.maxInterval,
                                                                     xConnectionParamA.latency,
                                                                     xConnectionParamA.timeout );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventConnectionUpdateCb, NO_HANDLE, ( void * ) &xUpdateConnectionParamReq, sizeof( BLETESTUpdateConnectionParamReqCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xUpdateConnectionParamReq.xStatus );

    TEST_ASSERT_EQUAL( 0, memcmp( &xUpdateConnectionParamReq.xBda, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( xConnectionParamA.minInterval, xUpdateConnectionParamReq.xConnParam.minInterval );
    TEST_ASSERT_EQUAL( xConnectionParamA.maxInterval, xUpdateConnectionParamReq.xConnParam.maxInterval );
    TEST_ASSERT_EQUAL( xConnectionParamA.latency, xUpdateConnectionParamReq.xConnParam.latency );
    TEST_ASSERT_EQUAL( xConnectionParamA.timeout, xUpdateConnectionParamReq.xConnParam.timeout );
}

TEST( Full_BLE, BLE_Connection_SimpleConnection )
{
    IotTestBleHal_WaitConnection( true );
}


TEST( Full_BLE, BLE_Advertising_StartAdvertisement )
{
    IotTestBleHal_StartAdvertisement();
}

TEST( Full_BLE, BLE_Advertising_SetAvertisementData )
{
    IotTestBleHal_SetAdvData( eBTuuidType128, 0, NULL );
}

TEST( Full_BLE, BLE_Advertising_SetProperties )
{
    IotTestBleHal_SetAdvProperty();
}

TEST( Full_BLE, BLE_CreateAttTable_IncludedService )
{
    BLETESTAttrCallback_t xBLETESTInclServiceCb;
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = _pxGattServerInterface->pxAddIncludedService( _ucBLEServerIf,
                                                            _xSrvcB.pusHandlesBuffer[ 0 ],
                                                            _xSrvcA.pusHandlesBuffer[ 0 ] );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventIncludedServiceAdded, NO_HANDLE, ( void * ) &xBLETESTInclServiceCb, sizeof( BLETESTAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xBLETESTInclServiceCb.xStatus );
    TEST_ASSERT_EQUAL( _xSrvcB.pusHandlesBuffer[ 0 ], xBLETESTInclServiceCb.usSrvHandle );
    _xSrvcB.pusHandlesBuffer[ bletestATTR_INCLUDED_SERVICE ] = xBLETESTInclServiceCb.usAttrHandle;
}

TEST( Full_BLE, BLE_CreateAttTable_CreateServices )
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
        IotTestBleHal_CreateServiceB();

        /* Start service A */
        IotTestBleHal_StartService( &_xSrvcA );
        /* Start service B */
        IotTestBleHal_StartService( &_xSrvcB );
    }
}



TEST( Full_BLE, BLE_Initialize_BLE_GATT )
{
    IotTestBleHal_BLEGATTInit();
}

TEST( Full_BLE, BLE_Initialize_common_GAP )
{
    IotTestBleHal_BLEManagerInit();
    IotTestBleHal_BLEEnable( true );
}

TEST( Full_BLE, BLE_Initialize_BLE_GAP )
{
    IotTestBleHal_BLEGAPInit();
}

TEST( Full_BLE, BLE_DeInitialize )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    IotTestBleHal_StopService( &_xSrvcA );
    IotTestBleHal_StopService( &_xSrvcB );

    IotTestBleHal_DeleteService( &_xSrvcA );
    IotTestBleHal_DeleteService( &_xSrvcB );

    IotTestBleHal_BTUnregister();
    IotTestBleHal_BLEEnable( false );
    xStatus = _pxBTInterface->pxBtManagerCleanup();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}
