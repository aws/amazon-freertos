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
 * @file aws_test_ble.c
 * @brief Tests for ble.
 */
#include "iot_ble_config.h"
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
extern BTService_t _xSrvcC;
extern uint16_t usHandlesBufferB[ bletestATTR_SRVCB_NUMBER ];
extern response_t ucRespBuffer[ bletestATTR_SRVCB_NUMBER ];

extern BTCallbacks_t _xBTManagerCb;
extern BTBleAdapterCallbacks_t _xBTBleAdapterCb;
extern BTGattServerCallbacks_t _xBTGattServerCb;

static uint8_t LongReadBuffer[ bletests_LONG_READ_LEN ];

const uint32_t bletestWAIT_MODE1_LEVEL2_QUERY = 10000; /* Wait 10s max */

#if LIBRARY_LOG_LEVEL > IOT_LOG_NONE

/**
 * @brief If logging is enabled, define a log configuration that only prints the log
 * string. This is used when printing out details of deserialized MQTT packets.
 */
    static const IotLogConfig_t _logHideAll =
    {
        .hideLibraryName = true,
        .hideLogLevel    = true,
        .hideTimestring  = true
    };
#endif

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
    #if ENABLE_TC_AFQP_WRITE_LONG
        RUN_TEST_CASE( Full_BLE, BLE_Property_WriteLongCharacteristic );
    #endif
    #if ENABLE_TC_AFQP_READ_LONG
        RUN_TEST_CASE( Full_BLE, BLE_Property_ReadLongCharacteristic );
    #endif

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
    RUN_TEST_CASE( Full_BLE, BLE_Connection_Disconnect_From_Peripheral );

    RUN_TEST_CASE( Full_BLE, BLE_Connection_CheckBonding );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_RemoveBonding );
    RUN_TEST_CASE( Full_BLE, BLE_Connection_Mode1Level2 );

    RUN_TEST_CASE( Full_BLE, BLE_DeInitialize );
    RUN_TEST_CASE( Full_BLE, BLE_Free );
}

void prvRemoveBond( BTBdaddr_t * pxDeviceAddress )
{
    BTStatus_t xStatus;
    BLETESTPairingStateChangedCallback_t xPairingEvent;

    xStatus = _pxBTInterface->pxRemoveBond( pxDeviceAddress );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventPairingStateChangedCb, NO_HANDLE, ( void * ) &xPairingEvent, sizeof( BLETESTPairingStateChangedCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xPairingEvent.xStatus );
    TEST_ASSERT_EQUAL( eBTbondStateNone, xPairingEvent.xBondState );
    TEST_ASSERT_EQUAL( 0, memcmp( &xPairingEvent.xRemoteBdAddr, pxDeviceAddress, sizeof( BTBdaddr_t ) ) );
}

TEST( Full_BLE, BLE_Setup )
{
    IotTestBleHal_BLESetUp();
}

TEST( Full_BLE, BLE_Free )
{
    IotTestBleHal_BLEFree();
}

size_t prvGetNumberOfBonds()
{
    BTProperty_t xProperty;
    size_t xNumElements;

    xProperty.xType = eBTpropertyAdapterBondedDevices;

    /* Get bonded devices list */
    IotTestBleHal_SetGetProperty( &xProperty, false );
    xNumElements = ( xProperty.xLen ) / ( sizeof( BTBdaddr_t ) );

    return xNumElements;
}


TEST( Full_BLE, BLE_Connection_RemoveAllBonds )
{
    BTProperty_t xProperty;
    size_t xIndex, xNumElements;

    /* Set the name */
    xProperty.xType = eBTpropertyAdapterBondedDevices;

    /* Get bonded devices */
    IotTestBleHal_SetGetProperty( &xProperty, false );

    xNumElements = ( xProperty.xLen ) / ( sizeof( BTBdaddr_t ) );

    for( xIndex = 0; xIndex < xNumElements; xIndex++ )
    {
        prvRemoveBond( &( ( BTBdaddr_t * ) xProperty.pvVal )[ xIndex ] );
    }

    /* Get bonded devices. */
    xNumElements = prvGetNumberOfBonds();

    /* Check none are left. */
    TEST_ASSERT_EQUAL( 0, xNumElements );
}

bool prvGetCheckDeviceBonded( BTBdaddr_t * pxDeviceAddress )
{
    BTProperty_t xProperty;
    size_t xIndex, xNumElements;
    bool bFoundRemoteDevice = false;

    xProperty.xType = eBTpropertyAdapterBondedDevices;

    /* Set the name */
    IotTestBleHal_SetGetProperty( &xProperty, false );

    xNumElements = ( xProperty.xLen ) / ( sizeof( BTBdaddr_t ) );

    for( xIndex = 0; xIndex < xNumElements; xIndex++ )
    {
        if( 0 == memcmp( &( ( BTBdaddr_t * ) xProperty.pvVal )[ xIndex ], pxDeviceAddress, sizeof( BTBdaddr_t ) ) )
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
    if( bConnected == true )
    {
        xStatus = _pxBTLeAdapterInterface->pxStopAdv( _ucBLEAdapterIf );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
        IotTestBleHal_StartStopAdvCheck( false );
    }
}

TEST( Full_BLE, BLE_Connection_Mode1Level2 )
{
    BTStatus_t xStatus;
    BLETESTPairingStateChangedCallback_t xPairingStateChangedEvent;
    BLETESTsspRequestCallback_t xSSPrequestEvent;
    BLEHALEventsTypes_t xEventsReceived;

    IotTestBleHal_ClearEventQueue();

    IotTestBleHal_StartAdvertisement();
    IotTestBleHal_WaitConnection( true );

    /* SSP Pairing request callback is optional for a board to support. Some boards handle the requeust directly in stack and will emit only Pairing state change callback. */
    xEventsReceived = IotTestBleHal_WaitForEvents( ( eBLEHALEventSSPrequestCb | eBLEHALEventPairingStateChangedCb ), BLE_TESTS_WAIT );
    TEST_ASSERT_NOT_EQUAL( eBLEHALEventNone, xEventsReceived );

    if( ( xEventsReceived & eBLEHALEventSSPrequestCb ) == eBLEHALEventSSPrequestCb )
    {
        xStatus = IotTestBleHal_GetEventFromQueueWithMatch( eBLEHALEventSSPrequestCb, NO_HANDLE, ( void * ) &xSSPrequestEvent, sizeof( BLETESTsspRequestCallback_t ), NULL );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
        TEST_ASSERT_EQUAL( 0, memcmp( &xSSPrequestEvent.xRemoteBdAddr, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
        TEST_ASSERT_EQUAL( eBTsspVariantConsent, xSSPrequestEvent.xPairingVariant );

        xStatus = _pxBTInterface->pxSspReply( &xSSPrequestEvent.xRemoteBdAddr, eBTsspVariantConsent, true, 0 );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

        xStatus = IotTestBleHal_WaitEventFromQueueWithMatch( eBLEHALEventPairingStateChangedCb, NO_HANDLE, ( void * ) &xPairingStateChangedEvent, sizeof( BLETESTPairingStateChangedCallback_t ), bletestWAIT_MODE1_LEVEL2_QUERY, IotTestBleHal_CheckBondState );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );                        /* Pairing should never come since it is secure connection only */
        TEST_ASSERT_EQUAL( eBTStatusFail, xPairingStateChangedEvent.xStatus ); /* Pairing should never come since it is secure connection only */
        /* @TODO add correct flag */
        /* TEST_ASSERT_EQUAL(eBTauthFailInsuffSecurity, xPairingStateChangedEvent.xReason); */ /* Pairing should never come since it is secure connection only */
    }
    else
    {
        xStatus = IotTestBleHal_GetEventFromQueueWithMatch( eBLEHALEventPairingStateChangedCb, NO_HANDLE, ( void * ) &xPairingStateChangedEvent, sizeof( BLETESTPairingStateChangedCallback_t ), IotTestBleHal_CheckBondState );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );                        /* Pairing should never come since it is secure connection only */
        TEST_ASSERT_EQUAL( eBTStatusFail, xPairingStateChangedEvent.xStatus ); /* Pairing should never come since it is secure connection only */
        /* @TODO add correct flag */
        /* TEST_ASSERT_EQUAL(eBTauthFailInsuffSecurity, xPairingStateChangedEvent.xReason); */ /* Pairing should never come since it is secure connection only */
    }
}

TEST( Full_BLE, BLE_Connection_RemoveBonding )
{
    bool bFoundRemoteDevice;
    size_t xNumBonds;

    IotTestBleHal_ClearEventQueue();

    prvRemoveBond( &_xAddressConnectedDevice );

    xNumBonds = prvGetNumberOfBonds();
    TEST_ASSERT_EQUAL( 0, xNumBonds );

    bFoundRemoteDevice = prvGetCheckDeviceBonded( &_xAddressConnectedDevice );
    TEST_ASSERT_EQUAL( false, bFoundRemoteDevice );
}


TEST( Full_BLE, BLE_Connection_CheckBonding )
{
    bool bFoundRemoteDevice;
    size_t xNumBonds;

    xNumBonds = prvGetNumberOfBonds();
    TEST_ASSERT_EQUAL( 1, xNumBonds );

    bFoundRemoteDevice = prvGetCheckDeviceBonded( &_xAddressConnectedDevice );
    TEST_ASSERT_EQUAL( true, bFoundRemoteDevice );
}



TEST( Full_BLE, BLE_Connection_BondedReconnectAndPair )
{
    IotTestBleHal_StartAdvertisement();
    IotTestBleHal_WaitConnection( true );

    IotTestBleHal_WriteCheckAndResponse( bletestATTR_SRVCB_CHAR_B,
                                         true,
                                         false,
                                         0 );
}


TEST( Full_BLE, BLE_Connection_Disconnect )
{
    IotTestBleHal_WaitConnection( false );
}

TEST( Full_BLE, BLE_Connection_Disconnect_From_Peripheral )
{
    BTStatus_t xStatus;
    BLETESTConnectionCallback_t xConnectionEvent;

    xStatus = _pxBTLeAdapterInterface->pxDisconnect( _ucBLEAdapterIf, &_xAddressConnectedDevice, _usBLEConnId );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventConnectionCb, NO_HANDLE, ( void * ) &xConnectionEvent, sizeof( BLETESTConnectionCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( false, xConnectionEvent.bConnected );
    TEST_ASSERT_EQUAL( _ucBLEServerIf, xConnectionEvent.ucServerIf );
    TEST_ASSERT_EQUAL( 0, memcmp( &xConnectionEvent.pxBda, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xConnectionEvent.xStatus );
}

TEST( Full_BLE, BLE_Connection_Mode1Level4_Property_WriteDescr )
{
    IotTestBleHal_WriteCheckAndResponse( bletestATTR_SRVCB_CHARF_DESCR_B,
                                         true,
                                         false,
                                         0 );
}

TEST( Full_BLE, BLE_Connection_Mode1Level4_Property_WriteChar )
{
    IotTestBleHal_WriteCheckAndResponse( bletestATTR_SRVCB_CHAR_B,
                                         true,
                                         false,
                                         0 );
}

TEST( Full_BLE, BLE_Connection_Mode1Level4 )
{
    IotTestBleHal_CreateSecureConnection_Model1Level4( true );

    IotTestBleHal_WriteCheckAndResponse( bletestATTR_SRVCB_CHAR_B,
                                         true,
                                         false,
                                         0 );
}

/* TODO: to confirm ExecuteWrite doesn't need Resp */
void prvExecuteWriteCheckAndResponse( bletestAttSrvB_t xAttribute,
                                      bool bNeedRsp )
{
    BLETESTwriteAttrCallback_t xWriteEvent;
    BLETESTconfirmCallback_t xConfirmEvent;
    BTGattResponse_t xGattResponse;
    BTStatus_t xStatus;

    /* Wait write event on char A*/
    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventRequestExecWriteCb, NO_HANDLE, ( void * ) &xWriteEvent, sizeof( BLETESTwriteAttrCallback_t ), BLE_TESTS_WAIT );
    IotLog( IOT_LOG_DEBUG,
            &_logHideAll,
            "prvExecuteWriteCheckAndResponse: received event (connid=%d, ulTransId=%d)",
            xWriteEvent.usConnId,
            xWriteEvent.ulTransId );

    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    TEST_ASSERT_EQUAL( _usBLEConnId, xWriteEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, memcmp( &xWriteEvent.xBda, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );

    if( bNeedRsp )
    {
        memset( &xGattResponse, 0, sizeof( BTGattResponse_t ) );
        xGattResponse.usHandle = usHandlesBufferB[ xAttribute ];
        xGattResponse.xAttrValue.usHandle = usHandlesBufferB[ xAttribute ];
        /* /TODO: check & save long write value */
        /* ucRespBuffer[ xAttribute ].xLength = xWriteEvent.xLength; */
        /* memcpy( ucRespBuffer[ xAttribute ].ucBuffer, xWriteEvent.ucValue, xWriteEvent.xLength ); */
        /* xGattResponse.xAttrValue.pucValue = ucRespBuffer[ xAttribute ].ucBuffer; */
        xStatus = _pxGattServerInterface->pxSendResponse( xWriteEvent.usConnId, xWriteEvent.ulTransId, eBTStatusSuccess, &xGattResponse );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

        xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventConfimCb, usHandlesBufferB[ xAttribute ], ( void * ) &xConfirmEvent, sizeof( BLETESTconfirmCallback_t ), BLE_TESTS_WAIT );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xConfirmEvent.xStatus );
        /* TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xConfirmEvent.usAttrHandle ); */
    }
}

BLETESTwriteAttrCallback_t IotTestBleHal_WriteReceive( bletestAttSrvB_t xAttribute,
                                                       bool bNeedRsp,
                                                       bool IsPrep,
                                                       uint16_t usOffset )
{
    BLETESTwriteAttrCallback_t xWriteEvent;
    BTStatus_t xStatus;
    size_t xLength;

    /* Wait write event on char A*/
    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventWriteAttrCb, usHandlesBufferB[ xAttribute ], ( void * ) &xWriteEvent, sizeof( BLETESTwriteAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( IsPrep, xWriteEvent.bIsPrep );
    TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xWriteEvent.usAttrHandle );
    TEST_ASSERT_EQUAL( _usBLEConnId, xWriteEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, memcmp( &xWriteEvent.xBda, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( usOffset, xWriteEvent.usOffset );

    if( !IsPrep )                                            /* not a prepare write req */
    {
        TEST_ASSERT_EQUAL( bNeedRsp, xWriteEvent.bNeedRsp ); /* NOT check bNeedRsp for PrepareWrite because of stack differences */
        /* TODO: add check for Prepare Write Req */
        TEST_ASSERT_EQUAL( bletestsSTRINGYFIED_UUID_SIZE, xWriteEvent.xLength );
    }
    else
    {
        if( xWriteEvent.xLength < _bletestsMTU_SIZE - bletests_LONG_WRITE_HEADER_LEN )
        {
            TEST_ASSERT_EQUAL( bletests_LONG_WRITE_LEN, xWriteEvent.xLength + usOffset );
        }

        /* eBLEHALEventWriteAttrCb only returns first bletestsSTRINGYFIED_UUID_SIZE bytes when received data is longer than bletestsSTRINGYFIED_UUID_SIZE */
        xLength = xWriteEvent.xLength > bletestsSTRINGYFIED_UUID_SIZE ? bletestsSTRINGYFIED_UUID_SIZE : xWriteEvent.xLength;
        TEST_ASSERT_EACH_EQUAL_INT8( '1', xWriteEvent.ucValue, xLength );
    }

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
    IotTestBleHal_WriteCheckAndResponse( bletestATTR_SRVCB_CHAR_D,
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
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

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
    IotTestBleHal_WriteCheckAndResponse( bletestATTR_SRVCB_CHARF_DESCR_A,
                                         true,
                                         false,
                                         0 );
}

TEST( Full_BLE, BLE_Property_WriteCharacteristic )
{
    IotTestBleHal_WriteCheckAndResponse( bletestATTR_SRVCB_CHAR_A,
                                         true,
                                         false,
                                         0 );
}

/**
 * @brief test long write
 * write mtu + 10 bytes of data -> expecting 2 prepare write + 1 execute write from RPI
 */
TEST( Full_BLE, BLE_Property_WriteLongCharacteristic )
{
    BLETESTwriteAttrCallback_t xWriteEvent;
    BTStatus_t xStatus;
    uint16_t usOffset = 0;

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventWriteAttrCb, usHandlesBufferB[ bletestATTR_SRVCB_CHAR_A ], ( void * ) &xWriteEvent, sizeof( BLETESTwriteAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( _usBLEConnId, xWriteEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, memcmp( &xWriteEvent.xBda, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( 0, xWriteEvent.usOffset );

    if( xWriteEvent.bIsPrep == true )
    {
        TEST_ASSERT_EQUAL( usHandlesBufferB[ bletestATTR_SRVCB_CHAR_A ], xWriteEvent.usAttrHandle );
        TEST_ASSERT_EACH_EQUAL_INT8( '1', xWriteEvent.ucValue, bletestsSTRINGYFIED_UUID_SIZE );

        if( xWriteEvent.bNeedRsp == true ) /* this flag is different depending on different stack implementation */
        {
            IotTestBleHal_WriteResponse( bletestATTR_SRVCB_CHAR_A, xWriteEvent, true );
        }

        usOffset += _bletestsMTU_SIZE - bletests_LONG_WRITE_HEADER_LEN;

        while( usOffset < bletests_LONG_WRITE_LEN )
        {
            IotTestBleHal_WriteCheckAndResponse( bletestATTR_SRVCB_CHAR_A,
                                                 true,
                                                 true,
                                                 usOffset );
            usOffset += _bletestsMTU_SIZE - bletests_LONG_WRITE_HEADER_LEN;
        }

        prvExecuteWriteCheckAndResponse( bletestATTR_SRVCB_CHAR_A,
                                         true );
    }
    else
    {
        TEST_ASSERT_EQUAL( bletests_LONG_WRITE_LEN, xWriteEvent.xLength );
        TEST_ASSERT_EACH_EQUAL_INT8( '1', xWriteEvent.ucValue, bletestsSTRINGYFIED_UUID_SIZE );

        if( xWriteEvent.bNeedRsp == true )
        {
            IotTestBleHal_WriteResponse( bletestATTR_SRVCB_CHAR_A, xWriteEvent, true );
        }
    }
}

TEST( Full_BLE, BLE_Property_ReadLongCharacteristic )
{
    BLETESTreadAttrCallback_t xReadEvent;
    BTGattResponse_t xGattResponse;
    BLETESTconfirmCallback_t xConfirmEvent;
    BTStatus_t xStatus;
    uint16_t usPayloadLength;
    uint16_t usOffset = 0;

    memset( LongReadBuffer, '1', bletests_LONG_READ_LEN * sizeof( uint8_t ) );
    usPayloadLength = _bletestsMTU_SIZE > bletests_LONGEST_ATTR_LEN ? bletests_LONGEST_ATTR_LEN : _bletestsMTU_SIZE - 1;

    /* Read transaction */
    xReadEvent = IotTestBleHal_ReadReceive( bletestATTR_SRVCB_CHAR_A );

    xGattResponse.usHandle = xReadEvent.usAttrHandle;
    xGattResponse.xAttrValue.usHandle = xReadEvent.usAttrHandle;
    xGattResponse.xAttrValue.usOffset = xReadEvent.usOffset;
    xGattResponse.xAttrValue.xLen = bletests_LONG_READ_LEN;
    xGattResponse.xAttrValue.pucValue = LongReadBuffer;
    _pxGattServerInterface->pxSendResponse( xReadEvent.usConnId, xReadEvent.ulTransId, eBTStatusSuccess, &xGattResponse );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventConfimCb, usHandlesBufferB[ bletestATTR_SRVCB_CHAR_A ], ( void * ) &xConfirmEvent, sizeof( BLETESTconfirmCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xConfirmEvent.xStatus );
    TEST_ASSERT_EQUAL( usHandlesBufferB[ bletestATTR_SRVCB_CHAR_A ], xConfirmEvent.usAttrHandle );

    /* Read blob transaction */
    usOffset += usPayloadLength;

    while( usOffset < bletests_LONG_READ_LEN )
    {
        xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventReadAttrCb, usHandlesBufferB[ bletestATTR_SRVCB_CHAR_A ], ( void * ) &xReadEvent, sizeof( BLETESTreadAttrCallback_t ), BLE_TESTS_WAIT );
        xGattResponse.usHandle = xReadEvent.usAttrHandle;
        xGattResponse.xAttrValue.usHandle = xReadEvent.usAttrHandle;
        xGattResponse.xAttrValue.usOffset = xReadEvent.usOffset;
        xGattResponse.xAttrValue.xLen = bletests_LONG_READ_LEN - xReadEvent.usOffset;
        xGattResponse.xAttrValue.pucValue = LongReadBuffer + xReadEvent.usOffset;
        _pxGattServerInterface->pxSendResponse( xReadEvent.usConnId, xReadEvent.ulTransId, eBTStatusSuccess, &xGattResponse );

        xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventConfimCb, usHandlesBufferB[ bletestATTR_SRVCB_CHAR_A ], ( void * ) &xConfirmEvent, sizeof( BLETESTconfirmCallback_t ), BLE_TESTS_WAIT );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xConfirmEvent.xStatus );
        TEST_ASSERT_EQUAL( usHandlesBufferB[ bletestATTR_SRVCB_CHAR_A ], xConfirmEvent.usAttrHandle );

        usOffset += usPayloadLength;
    }
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
    BTStatus_t xStatus;
    BLETESTMtuChangedCallback_t xMtuChangedEvent;

    IotTestBleHal_WaitConnection( true );

    /* Check the MTU size */
    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventMtuChangedCb, NO_HANDLE, ( void * ) &xMtuChangedEvent, sizeof( BLETESTMtuChangedCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( _usBLEConnId, xMtuChangedEvent.usConnId );
    TEST_ASSERT_EQUAL( IOT_BLE_PREFERRED_MTU_SIZE, xMtuChangedEvent.usMtu );
}


TEST( Full_BLE, BLE_Advertising_StartAdvertisement )
{
    IotTestBleHal_StartAdvertisement();
}

TEST( Full_BLE, BLE_Advertising_SetAvertisementData )
{
    IotTestBleHal_SetAdvData( eBTuuidType128, 0, NULL, 0, NULL );
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
    IotTestBleHal_CreateStartServiceA();
    IotTestBleHal_CreateStartServiceB( true );
    #if ENABLE_TC_AFQP_SECONDARY_SERVICE
        IotTestBleHal_CreateStartServiceC();
    #endif
}


TEST( Full_BLE, BLE_Initialize_BLE_GATT )
{
    IotTestBleHal_BLEGATTInit( &_xBTGattServerCb, true );
}

TEST( Full_BLE, BLE_Initialize_common_GAP )
{
    IotTestBleHal_BLEManagerInit( &_xBTManagerCb );
    IotTestBleHal_BLEEnable( true );
}

TEST( Full_BLE, BLE_Initialize_BLE_GAP )
{
    IotTestBleHal_BLEGAPInit( &_xBTBleAdapterCb, true );
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
