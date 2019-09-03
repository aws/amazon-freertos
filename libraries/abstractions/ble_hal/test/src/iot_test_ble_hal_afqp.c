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

BTInterface_t * _pxBTInterface;
BTBleAdapter_t * _pxBTLeAdapterInterface;
uint8_t _ucBLEAdapterIf;
uint8_t _ucBLEServerIf;
BTBdaddr_t _xAddressConnectedDevice;
BTGattServerInterface_t * _pxGattServerInterface;
uint16_t _usBLEConnId;


uint16_t usHandlesBufferA[ bletestATTR_SRVCA_NUMBER ];
uint16_t usHandlesBufferB[ bletestATTR_SRVCB_NUMBER ];

/* service A */

static const BTAttribute_t pxAttributeTableA[] =
{
    {
        .xServiceUUID = bletestsFREERTOS_SVC_A_UUID
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = bletestsFREERTOS_CHAR_DUMMY_UUID,
            .xPermissions = ( bletestsFULL_PERMISSIONS ),
            .xProperties  = ( eBTPropRead | eBTPropWrite )
        }
    }
};

BTService_t _xSrvcA =
{
    .ucInstId            = 0,
    .xType               = eBTServiceTypePrimary,
    .xNumberOfAttributes = bletestATTR_SRVCA_NUMBER,
    .pusHandlesBuffer    = usHandlesBufferA,
    .pxBLEAttributes     = ( BTAttribute_t * ) pxAttributeTableA
};

/* Service B */

static const BTAttribute_t pxAttributeTableB[] =
{
    {
        .xServiceUUID = bletestsFREERTOS_SVC_B_UUID
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = bletestsFREERTOS_CHAR_A_UUID,
            .xPermissions = ( bletestsFULL_PERMISSIONS ),
            .xProperties  = ( eBTPropRead | eBTPropWrite )
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = bletestsFREERTOS_CHAR_B_UUID,
            .xPermissions = ( eBTPermReadEncryptedMitm | eBTPermWriteEncryptedMitm ),
            .xProperties  = ( eBTPropRead | eBTPropWrite )
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = bletestsFREERTOS_CHAR_C_UUID,
            .xPermissions = ( eBTPermReadEncrypted | eBTPermWriteEncrypted ),
            .xProperties  = eBTPropRead | eBTPropWrite
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = bletestsFREERTOS_CHAR_D_UUID,
            .xPermissions = ( bletestsFULL_PERMISSIONS ),
            .xProperties  = ( eBTPropWriteNoResponse )
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = bletestsFREERTOS_CHAR_E_UUID,
            .xPermissions = ( bletestsFULL_PERMISSIONS ),
            .xProperties  = ( eBTPropNotify )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid        = bletestsCCCD,
            .xPermissions = ( eBTPermRead | eBTPermWrite )
        }
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = bletestsFREERTOS_CHAR_F_UUID,
            .xPermissions = ( bletestsFULL_PERMISSIONS ),
            .xProperties  = ( eBTPropIndicate )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid        = bletestsCCCD,
            .xPermissions = ( eBTPermRead | eBTPermWrite )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid        = bletestsFREERTOS_DESCR_A_UUID,
            .xPermissions = ( eBTPermRead | eBTPermWrite )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid        = bletestsFREERTOS_DESCR_B_UUID,
            .xPermissions = ( eBTPermReadEncryptedMitm | eBTPermWriteEncryptedMitm )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid        = bletestsFREERTOS_DESCR_C_UUID,
            .xPermissions = ( eBTPermReadEncrypted | eBTPermWriteEncrypted )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid        = bletestsFREERTOS_DESCR_D_UUID,
            .xPermissions = ( eBTPermRead )
        }
    },
    {
        .xAttributeType = eBTDbIncludedService,
        .xIncludedService =
        {
            .xUuid          = bletestsFREERTOS_SVC_A_UUID,
            .pxPtrToService = &_xSrvcA
        }
    }
};

BTService_t _xSrvcB =
{
    .ucInstId            = 0,
    .xType               = eBTServiceTypePrimary,
    .xNumberOfAttributes = bletestATTR_SRVCB_NUMBER,
    .pusHandlesBuffer    = usHandlesBufferB,
    .pxBLEAttributes     = ( BTAttribute_t * ) pxAttributeTableB
};


BTGattAdvertismentParams_t xAdvertisementConfigA =
{
    .usAdvertisingEventProperties = BTAdvInd,
    .bIncludeTxPower              = true,
    .ucName                       = { BTGattAdvNameNone,                   0},
    .bSetScanRsp                  = false,
    .ulAppearance                 = 0,
    .ulMinInterval                = bletestsMAX_ADVERTISEMENT_INTERVAL / 2,
    .ulMaxInterval                = bletestsMAX_ADVERTISEMENT_INTERVAL,
    .ucChannelMap                 = 0,
    .ucPrimaryAdvertisingPhy      = 0,
    .ucSecondaryAdvertisingPhy    = 0,
    .xAddrType                    = BTAddrTypePublic,
};

BTGattAdvertismentParams_t xAdvertisementConfigB =
{
    .usAdvertisingEventProperties = BTAdvInd,
    .bIncludeTxPower              = true,
    .ucName                       = { BTGattAdvNameShort,                  4},
    .bSetScanRsp                  = true,
    .ulAppearance                 = 0,
    .ulMinInterval                = bletestsMAX_ADVERTISEMENT_INTERVAL / 2,
    .ulMaxInterval                = bletestsMAX_ADVERTISEMENT_INTERVAL,
    .ucChannelMap                 = 0,
    .ucPrimaryAdvertisingPhy      = 0,
    .ucSecondaryAdvertisingPhy    = 0,
    .xAddrType                    = BTAddrTypePublic,
};

IotBleConnectionParam_t xConnectionParamA =
{
    .minInterval = bletestsMIN_CONNECTION_INTERVAL,
    .maxInterval = bletestsMAX_CONNECTION_INTERVAL,
    .latency     = 4,
    .timeout     = 400
};

BTUuid_t xServerUUID =
{
    .ucType   = eBTuuidType128,
    .uu.uu128 = bletestsSERVER_UUID
};
BTUuid_t xAppUUID =
{
    .ucType   = eBTuuidType128,
    .uu.uu128 = bletestsAPP_UUID
};

IotListDouble_t eventQueueHead;
IotMutex_t threadSafetyMutex;
IotSemaphore_t eventSemaphore;

uint8_t ucCbPropertyBuffer[ bletestsMAX_PROPERTY_SIZE ];
uint32_t usCbConnInterval;

response_t ucRespBuffer[ bletestATTR_SRVCB_NUMBER ];


const uint32_t bletestWAIT_MODE1_LEVEL2_QUERY = 10000; /* Wait 10s max */

BTCallbacks_t _xBTManagerCb =
{
    .pxDeviceStateChangedCb     = prvDeviceStateChangedCb,
    .pxAdapterPropertiesCb      = prvAdapterPropertiesCb,
    .pxRemoteDevicePropertiesCb = NULL,
    .pxSspRequestCb             = prvSspRequestCb,
    .pxPairingStateChangedCb    = prvPairingStateChangedCb,
    .pxBondedCb                 = prvBondedCb,
    .pxDutModeRecvCb            = NULL,
    .pxleTestModeCb             = NULL,
    .pxEnergyInfoCb             = NULL,
    .pxReadRssiCb               = NULL,
    .pxTxPowerCb                = NULL,
    .pxSlaveSecurityRequestCb   = NULL,
};

BTBleAdapterCallbacks_t xBTBleAdapterCb =
{
    .pxRegisterBleAdapterCb          = prvRegisterBleAdapterCb,
    .pxScanResultCb                  = NULL,
    .pxBleAdapterPropertiesCb        = NULL,
    .pxBleRemoteDevicePropertiesCb   = NULL,
    .pxOpenCb                        = NULL,
    .pxCloseCb                       = NULL,
    .pxReadRemoteRssiCb              = NULL,
    .pxAdvStatusCb                   = prvAdvStatusCb,
    .pxSetAdvDataCb                  = prvSetAdvDataCb,
    .pxConnParameterUpdateCb         = prvConnParameterUpdateCb,
    .pxScanFilterCfgCb               = NULL,
    .pxScanFilterParamCb             = NULL,
    .pxScanFilterStatusCb            = NULL,
    .pxMultiAdvEnableCb              = NULL,
    .pxMultiAdvUpdateCb              = NULL,
    .pxMultiAdvDataCb                = NULL,
    .pxMultiAdvDisableCb             = NULL,
    .pxCongestionCb                  = NULL,
    .pxBatchscanCfgStorageCb         = NULL,
    .pxBatchscanEnbDisableCb         = NULL,
    .pxBatchscanReportsCb            = NULL,
    .pxBatchscanThresholdCb          = NULL,
    .pxTrackAdvEventCb               = NULL,
    .pxScanParameterSetupCompletedCb = NULL,
    .pxPhyUpdatedCb                  = NULL,
};


BTGattServerCallbacks_t xBTGattServerCb =
{
    .pxRegisterServerCb       = prvBTRegisterServerCb,
    .pxUnregisterServerCb     = prvBTUnregisterServerCb,
    .pxConnectionCb           = prvConnectionCb,
    .pxServiceAddedCb         = prvServiceAddedCb,
    .pxIncludedServiceAddedCb = prvIncludedServiceAddedCb,
    .pxCharacteristicAddedCb  = prvCharacteristicAddedCb,
    .pxSetValCallbackCb       = NULL,
    .pxDescriptorAddedCb      = prvCharacteristicDescrAddedCb,
    .pxServiceStartedCb       = prvServiceStartedCb,
    .pxServiceStoppedCb       = prvServiceStoppedCb,
    .pxServiceDeletedCb       = prvServiceDeletedCb,
    .pxRequestReadCb          = prvRequestReadCb,
    .pxRequestWriteCb         = prvRequestWriteCb,
    .pxRequestExecWriteCb     = prvRequestExecWriteCb,
    .pxResponseConfirmationCb = prvResponseConfirmationCb,
    .pxIndicationSentCb       = prvIndicationSentCb,
    .pxCongestionCb           = NULL,
    .pxMtuChangedCb           = NULL
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

    xStatus = prvWaitEventFromQueue( eBLEHALEventBondedCb, NO_HANDLE, ( void * ) &xBondedEvent, sizeof( BLETESTBondedCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xBondedEvent.xStatus );
    TEST_ASSERT_EQUAL( false, xBondedEvent.bIsBonded );
    TEST_ASSERT_EQUAL( 0, memcmp( &xBondedEvent.xRemoteBdAddr, pxDeviceAddress, sizeof( BTBdaddr_t ) ) );
}

TEST( Full_BLE, BLE_Setup )
{
    prvBLESetUp();
}

TEST( Full_BLE, BLE_Free )
{
    BLEHALEventsInternals_t * pEventIndex;
    IotLink_t * pEventListIndex;

    IotMutex_Lock( &threadSafetyMutex );

    /* Get the event associated to the callback */
    IotContainers_ForEach( &eventQueueHead, pEventListIndex )
    {
        pEventIndex = IotLink_Container( BLEHALEventsInternals_t, pEventListIndex, eventList );
        IotTest_Free( pEventIndex );
    }

    IotMutex_Unlock( &threadSafetyMutex );

    IotMutex_Destroy( &threadSafetyMutex );
    IotSemaphore_Destroy( &eventSemaphore );
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

void prvWaitConnection( bool bConnected )
{
    BLETESTConnectionCallback_t xConnectionEvent;
    BTStatus_t xStatus;

    xStatus = prvWaitEventFromQueue( eBLEHALEventConnectionCb, NO_HANDLE, ( void * ) &xConnectionEvent, sizeof( BLETESTConnectionCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( bConnected, xConnectionEvent.bConnected );
    TEST_ASSERT_EQUAL( _ucBLEServerIf, xConnectionEvent.ucServerIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xConnectionEvent.xStatus );

    /* Stop advertisement. */
    xStatus = _pxBTLeAdapterInterface->pxStopAdv( _ucBLEAdapterIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    prvStartStopAdvCheck( false );
}

TEST( Full_BLE, BLE_Connection_Mode1Level2 )
{
    BTStatus_t xStatus;
    BLETESTPairingStateChangedCallback_t xPairingStateChangedEvent;
    BLETESTsspRequestCallback_t xSSPrequestEvent;

    prvStartAdvertisement();
    prvWaitConnection( true );

    xStatus = prvWaitEventFromQueue( eBLEHALEventSSPrequestCb, NO_HANDLE, ( void * ) &xSSPrequestEvent, sizeof( BLETESTsspRequestCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xSSPrequestEvent.xRemoteBdAddr, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( eBTsspVariantConsent, xSSPrequestEvent.xPairingVariant );

    xStatus = _pxBTInterface->pxSspReply( &xSSPrequestEvent.xRemoteBdAddr, eBTsspVariantConsent, true, 0 );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventPairingStateChangedCb, NO_HANDLE, ( void * ) &xPairingStateChangedEvent, sizeof( BLETESTPairingStateChangedCallback_t ), bletestWAIT_MODE1_LEVEL2_QUERY );
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

    prvStartAdvertisement();
    prvWaitConnection( true );

    xStatus = prvWaitEventFromQueue( eBLEHALEventPairingStateChangedCb, NO_HANDLE, ( void * ) &xPairingStateChangedEvent, sizeof( BLETESTPairingStateChangedCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xPairingStateChangedEvent.xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xPairingStateChangedEvent.xRemoteBdAddr, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( eBTSecLevelSecureConnect, xPairingStateChangedEvent.xSecurityLevel );

    xStatus = prvWaitEventFromQueue( eBLEHALEventBondedCb, NO_HANDLE, ( void * ) &xBondedEvent, sizeof( BLETESTBondedCallback_t ), BLE_TESTS_WAIT );
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
    prvWaitConnection( false );
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
    xStatus = prvWaitEventFromQueue( eBLEHALEventSSPrequestCb, NO_HANDLE, ( void * ) &xSSPrequestEvent, sizeof( BLETESTsspRequestCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xSSPrequestEvent.xRemoteBdAddr, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( eBTsspVariantConsent, xSSPrequestEvent.xPairingVariant );

    xStatus = _pxBTInterface->pxSspReply( &xSSPrequestEvent.xRemoteBdAddr, eBTsspVariantConsent, true, 0 );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventSSPrequestConfirmationCb, NO_HANDLE, ( void * ) &xSSPrequestEvent, sizeof( BLETESTsspRequestCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xSSPrequestEvent.xRemoteBdAddr, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( eBTsspVariantPasskeyConfirmation, xSSPrequestEvent.xPairingVariant );

    xStatus = _pxBTInterface->pxSspReply( &xSSPrequestEvent.xRemoteBdAddr, eBTsspVariantPasskeyConfirmation, true, 0 );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventPairingStateChangedCb, NO_HANDLE, ( void * ) &xPairingStateChangedEvent, sizeof( BLETESTPairingStateChangedCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xPairingStateChangedEvent.xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xPairingStateChangedEvent.xRemoteBdAddr, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( eBTSecLevelSecureConnect, xPairingStateChangedEvent.xSecurityLevel );

    xStatus = prvWaitEventFromQueue( eBLEHALEventBondedCb, NO_HANDLE, ( void * ) &xBondedEvent, sizeof( BLETESTBondedCallback_t ), BLE_TESTS_WAIT );
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
    BLETESTconfirmCallback_t xConfirmEvent;
    BTGattResponse_t xGattResponse;
    BTStatus_t xStatus;

    /* Wait write event on char A*/
    xStatus = prvWaitEventFromQueue( eBLEHALEventWriteAttrCb, usHandlesBufferB[ xAttribute ], ( void * ) &xWriteEvent, sizeof( BLETESTwriteAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( IsPrep, xWriteEvent.bIsPrep );
    TEST_ASSERT_EQUAL( bNeedRsp, xWriteEvent.bNeedRsp );
    TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xWriteEvent.usAttrHandle );
    TEST_ASSERT_EQUAL( _usBLEConnId, xWriteEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, memcmp( &xWriteEvent.xBda, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
    TEST_ASSERT_EQUAL( usOffset, xWriteEvent.usOffset );
    TEST_ASSERT_EQUAL( bletestsSTRINGYFIED_UUID_SIZE, xWriteEvent.xLength );

    if( bNeedRsp == true )
    {
        xGattResponse.usHandle = xWriteEvent.usAttrHandle;
        xGattResponse.xAttrValue.usHandle = xWriteEvent.usAttrHandle;
        xGattResponse.xAttrValue.usOffset = xWriteEvent.usOffset;
        xGattResponse.xAttrValue.xLen = xWriteEvent.xLength;
        ucRespBuffer[ xAttribute ].xLength = xWriteEvent.xLength;
        memcpy( ucRespBuffer[ xAttribute ].ucBuffer, xWriteEvent.ucValue, xWriteEvent.xLength );
        xGattResponse.xAttrValue.pucValue = ucRespBuffer[ xAttribute ].ucBuffer;
        xStatus = _pxGattServerInterface->pxSendResponse( xWriteEvent.usConnId, xWriteEvent.ulTransId, eBTStatusSuccess, &xGattResponse );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

        xStatus = prvWaitEventFromQueue( eBLEHALEventConfimCb, xWriteEvent.usAttrHandle, ( void * ) &xConfirmEvent, sizeof( BLETESTconfirmCallback_t ), BLE_TESTS_WAIT );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xConfirmEvent.xStatus );
        TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xConfirmEvent.usAttrHandle );
    }
}

void prvSendNotification( bletestAttSrvB_t xAttribute,
                          bool bConfirm )
{
    BTStatus_t xStatus;

    xStatus = _pxGattServerInterface->pxSendIndication( _ucBLEServerIf,
                                                        usHandlesBufferB[ xAttribute ],
                                                        _usBLEConnId,
                                                        ucRespBuffer[ xAttribute ].xLength,
                                                        ucRespBuffer[ xAttribute ].ucBuffer,
                                                        bConfirm );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

void checkNotificationIndication( bletestAttSrvB_t xAttribute,
                                  bool enable )
{
    BTGattResponse_t xGattResponse;
    BTStatus_t xStatus;
    BLETESTwriteAttrCallback_t xWriteEvent;
    BLETESTconfirmCallback_t xConfirmEvent;

    xStatus = prvWaitEventFromQueue( eBLEHALEventWriteAttrCb, usHandlesBufferB[ xAttribute ], ( void * ) &xWriteEvent, sizeof( BLETESTwriteAttrCallback_t ), BLE_TESTS_WAIT );
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

    xStatus = prvWaitEventFromQueue( eBLEHALEventConfimCb, xWriteEvent.usAttrHandle, ( void * ) &xConfirmEvent, sizeof( BLETESTconfirmCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xConfirmEvent.xStatus );
    TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xConfirmEvent.usAttrHandle );
}

TEST( Full_BLE, BLE_Property_Enable_Indication_Notification )
{
    checkNotificationIndication( bletestATTR_SRVCB_CCCD_E, true );
    checkNotificationIndication( bletestATTR_SRVCB_CCCD_F, true );
}

TEST( Full_BLE, BLE_Property_Disable_Indication_Notification )
{
    checkNotificationIndication( bletestATTR_SRVCB_CCCD_E, false );
    checkNotificationIndication( bletestATTR_SRVCB_CCCD_F, false );
}


TEST( Full_BLE, BLE_Property_Indication )
{
    BTStatus_t xStatus;
    BLETESTindicateCallback_t xIndicateEvent;

    memcpy( ucRespBuffer[ bletestATTR_SRVCB_CHAR_F ].ucBuffer, bletestsDEFAULT_CHAR_VALUE, sizeof( bletestsDEFAULT_CHAR_VALUE ) - 1 );
    ucRespBuffer[ bletestATTR_SRVCB_CHAR_F ].xLength = sizeof( bletestsDEFAULT_CHAR_VALUE ) - 1;

    prvSendNotification( bletestATTR_SRVCB_CHAR_F, true );
    xStatus = prvWaitEventFromQueue( eBLEHALEventIndicateCb, NO_HANDLE, ( void * ) &xIndicateEvent, sizeof( BLETESTindicateCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( _usBLEConnId, xIndicateEvent.usConnId );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xIndicateEvent.xStatus );
}

TEST( Full_BLE, BLE_Property_Notification )
{
    BTStatus_t xStatus;
    BLETESTindicateCallback_t xIndicateEvent;

    memcpy( ucRespBuffer[ bletestATTR_SRVCB_CHAR_E ].ucBuffer, bletestsDEFAULT_CHAR_VALUE, sizeof( bletestsDEFAULT_CHAR_VALUE ) - 1 );
    ucRespBuffer[ bletestATTR_SRVCB_CHAR_E ].xLength = sizeof( bletestsDEFAULT_CHAR_VALUE ) - 1;

    prvSendNotification( bletestATTR_SRVCB_CHAR_E, false );
    /* Wait a possible confirm for 2 max connections interval */
    xStatus = prvWaitEventFromQueue( eBLEHALEventIndicateCb, NO_HANDLE, ( void * ) &xIndicateEvent, sizeof( BLETESTindicateCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( _usBLEConnId, xIndicateEvent.usConnId );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xIndicateEvent.xStatus );
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
    BTGattResponse_t xGattResponse;
    BLETESTconfirmCallback_t xConfirmEvent;
    BTStatus_t xStatus;

    xStatus = prvWaitEventFromQueue( eBLEHALEventReadAttrCb, usHandlesBufferB[ xAttribute ], ( void * ) &xReadEvent, sizeof( BLETESTreadAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xReadEvent.usAttrHandle );
    TEST_ASSERT_EQUAL( _usBLEConnId, xReadEvent.usConnId );
    TEST_ASSERT_EQUAL( 0, xReadEvent.usOffset );
    TEST_ASSERT_EQUAL( 0, memcmp( &xReadEvent.xBda, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );

    xGattResponse.usHandle = xReadEvent.usAttrHandle;
    xGattResponse.xAttrValue.usHandle = xReadEvent.usAttrHandle;
    xGattResponse.xAttrValue.usOffset = xReadEvent.usOffset;
    xGattResponse.xAttrValue.xLen = ucRespBuffer[ xAttribute ].xLength;
    xGattResponse.xAttrValue.pucValue = ucRespBuffer[ xAttribute ].ucBuffer;
    _pxGattServerInterface->pxSendResponse( xReadEvent.usConnId, xReadEvent.ulTransId, eBTStatusSuccess, &xGattResponse );

    xStatus = prvWaitEventFromQueue( eBLEHALEventConfimCb, usHandlesBufferB[ xAttribute ], ( void * ) &xConfirmEvent, sizeof( BLETESTconfirmCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xConfirmEvent.xStatus );
    TEST_ASSERT_EQUAL( usHandlesBufferB[ xAttribute ], xConfirmEvent.usAttrHandle );
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

    xStatus = prvWaitEventFromQueue( eBLEHALEventConnectionUpdateCb, NO_HANDLE, ( void * ) &xUpdateConnectionParamReq, sizeof( BLETESTUpdateConnectionParamReqCallback_t ), BLE_TESTS_WAIT );
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
    prvWaitConnection( true );
}


void prvSetAdvertisement( BTGattAdvertismentParams_t * pxParams,
                          uint16_t usServiceDataLen,
                          char * pcServiceData,
                          BTUuid_t * pxServiceUuid,
                          size_t xNbServices )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTAdvParamCallback_t xAdvParamCallback;

    xStatus = _pxBTLeAdapterInterface->pxSetAdvData( _ucBLEAdapterIf,
                                                     pxParams,
                                                     0,
                                                     NULL,
                                                     usServiceDataLen,
                                                     pcServiceData,
                                                     pxServiceUuid,
                                                     xNbServices );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventSetAdvCb, NO_HANDLE, ( void * ) &xAdvParamCallback, sizeof( BLETESTAdvParamCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xAdvParamCallback.xStatus );
}

void prvStartStopAdvCheck( bool start )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTAdvParamCallback_t xAdvParamCallback;

    xStatus = prvWaitEventFromQueue( eBLEHALEventStartAdvCb, NO_HANDLE, ( void * ) &xAdvParamCallback, sizeof( BLETESTAdvParamCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( start, xAdvParamCallback.bStart );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xAdvParamCallback.xStatus );
}

void prvStartAdvertisement( void )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = _pxBTLeAdapterInterface->pxStartAdv( _ucBLEAdapterIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    prvStartStopAdvCheck( true );
}

TEST( Full_BLE, BLE_Advertising_StartAdvertisement )
{
    prvStartAdvertisement();
}

TEST( Full_BLE, BLE_Advertising_SetAvertisementData )
{
    prvSetAdvData();
}


void prvSetAdvData( void )
{
    uint16_t usServiceDataLen;
    char * pcServiceData;
    BTUuid_t xServiceUuid =
    {
        .ucType   = eBTuuidType128,
        .uu.uu128 = bletestsFREERTOS_SVC_UUID
    };

    size_t xNbServices;

    usServiceDataLen = 0;
    pcServiceData = NULL;
    xNbServices = 1;

    prvSetAdvertisement( &xAdvertisementConfigA,
                         usServiceDataLen,
                         pcServiceData,
                         &xServiceUuid,
                         xNbServices );


    prvSetAdvertisement( &xAdvertisementConfigB,
                         usServiceDataLen,
                         pcServiceData,
                         NULL,
                         0 );
}

void prvSetGetProperty( BTProperty_t * pxProperty,
                        bool bIsSet )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTSetGetPropertyCallback_t xSetGetPropertyCb;

    if( bIsSet == true )
    {
        xStatus = _pxBTInterface->pxSetDeviceProperty( pxProperty );
    }
    else
    {
        xStatus = _pxBTInterface->pxGetDeviceProperty( pxProperty->xType );
    }

    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventPropertyCb, NO_HANDLE, ( void * ) &xSetGetPropertyCb, sizeof( BLETESTSetGetPropertyCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xSetGetPropertyCb.xStatus );
    TEST_ASSERT_EQUAL( 1, xSetGetPropertyCb.ulNumProperties );
    TEST_ASSERT_EQUAL( xSetGetPropertyCb.xProperties.xType, pxProperty->xType );

    if( bIsSet == true )
    {
        TEST_ASSERT_EQUAL( xSetGetPropertyCb.xProperties.xLen, pxProperty->xLen );
        TEST_ASSERT_EQUAL( 0, memcmp( pxProperty->pvVal, xSetGetPropertyCb.xProperties.pvVal, xSetGetPropertyCb.xProperties.xLen ) );
    }
    else
    {
        pxProperty->xLen = xSetGetPropertyCb.xProperties.xLen;
        pxProperty->pvVal = xSetGetPropertyCb.xProperties.pvVal;
    }
}

TEST( Full_BLE, BLE_Advertising_SetProperties )
{
    prvSetAdvProperty();
}

void prvSetAdvProperty( void )
{
    BTProperty_t pxProperty;
    uint16_t usMTUsize = bletestsMTU_SIZE1;

    pxProperty.xType = eBTpropertyBdname;
    pxProperty.xLen = strlen( bletestsDEVICE_NAME );
    pxProperty.pvVal = ( void * ) bletestsDEVICE_NAME;

    /* Set the name */
    prvSetGetProperty( &pxProperty, true );

    /* Get the name to check it is set */
/*@TODO prvSetGetProperty(&pxProperty, false); */

    pxProperty.xType = eBTpropertyLocalMTUSize;
    pxProperty.xLen = sizeof( usMTUsize );
    pxProperty.pvVal = &usMTUsize;

    /* Set the MTU size */
    prvSetGetProperty( &pxProperty, true );

    /* Get the MTU size to check it is set */
    /*@TODOprvSetGetProperty(&pxProperty, false); */
}

void prvStartService( BTService_t * xRefSrvc )
{
    BLETESTServiceCallback_t xStartServiceCb;
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = _pxGattServerInterface->pxStartService( _ucBLEServerIf, xRefSrvc->pusHandlesBuffer[ 0 ], BTTransportLe );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventServiceStartedCb, xRefSrvc->pusHandlesBuffer[ 0 ], ( void * ) &xStartServiceCb, sizeof( BLETESTServiceCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStartServiceCb.xStatus );
}

TEST( Full_BLE, BLE_CreateAttTable_IncludedService )
{
    BLETESTAttrCallback_t xBLETESTInclServiceCb;
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = _pxGattServerInterface->pxAddIncludedService( _ucBLEServerIf,
                                                            _xSrvcB.pusHandlesBuffer[ 0 ],
                                                            _xSrvcA.pusHandlesBuffer[ 0 ] );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventIncludedServiceAdded, NO_HANDLE, ( void * ) &xBLETESTInclServiceCb, sizeof( BLETESTAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xBLETESTInclServiceCb.xStatus );
    TEST_ASSERT_EQUAL( _xSrvcB.pusHandlesBuffer[ 0 ], xBLETESTInclServiceCb.usSrvHandle );
    _xSrvcB.pusHandlesBuffer[ bletestATTR_INCLUDED_SERVICE ] = xBLETESTInclServiceCb.usAttrHandle;
}

void prvCreateCharacteristicDescriptor( BTService_t * xSrvc,
                                        int xAttribute )
{
    BLETESTAttrCallback_t xBLETESTCharDescrCb;
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = _pxGattServerInterface->pxAddDescriptor( _ucBLEServerIf,
                                                       xSrvc->pusHandlesBuffer[ 0 ],
                                                       &xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristicDescr.xUuid,
                                                       xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristicDescr.xPermissions );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventCharDescrAddedCb, NO_HANDLE, ( void * ) &xBLETESTCharDescrCb, sizeof( BLETESTAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xBLETESTCharDescrCb.xStatus );

    if( xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristicDescr.xUuid.ucType == eBTuuidType16 )
    {
        TEST_ASSERT_EQUAL( eBTuuidType16, xBLETESTCharDescrCb.xUUID.ucType );
        TEST_ASSERT_EQUAL( xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristicDescr.xUuid.uu.uu16, xBLETESTCharDescrCb.xUUID.uu.uu16 );
    }
    else
    {
        TEST_ASSERT_EQUAL( 0, memcmp( &xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristicDescr.xUuid, &xBLETESTCharDescrCb.xUUID, sizeof( BTUuid_t ) ) );
    }

    TEST_ASSERT_EQUAL( xSrvc->pusHandlesBuffer[ 0 ], xBLETESTCharDescrCb.usSrvHandle );
    xSrvc->pusHandlesBuffer[ xAttribute ] = xBLETESTCharDescrCb.usAttrHandle;
}

void prvCreateCharacteristic( BTService_t * xSrvc,
                              int xAttribute )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTAttrCallback_t xBLETESTCharCb;

    xStatus = _pxGattServerInterface->pxAddCharacteristic( _ucBLEServerIf,
                                                           xSrvc->pusHandlesBuffer[ 0 ],
                                                           &xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristic.xUuid,
                                                           xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristic.xProperties,
                                                           xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristic.xPermissions );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventCharAddedCb, NO_HANDLE, ( void * ) &xBLETESTCharCb, sizeof( BLETESTAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xBLETESTCharCb.xStatus );

    if( xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristic.xUuid.ucType == eBTuuidType16 )
    {
        TEST_ASSERT_EQUAL( eBTuuidType16, xBLETESTCharCb.xUUID.ucType );
        TEST_ASSERT_EQUAL( xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristic.xUuid.uu.uu16, xBLETESTCharCb.xUUID.uu.uu16 );
    }
    else
    {
        TEST_ASSERT_EQUAL( 0, memcmp( &xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristic.xUuid, &xBLETESTCharCb.xUUID, sizeof( BTUuid_t ) ) );
    }

    TEST_ASSERT_EQUAL( xSrvc->pusHandlesBuffer[ 0 ], xBLETESTCharCb.usSrvHandle );
    xSrvc->pusHandlesBuffer[ xAttribute ] = xBLETESTCharCb.usAttrHandle;
}

static size_t prvComputeNumberOfHandles( BTService_t * pxService )
{
    size_t xIndex;
    size_t nbHandles = 0;

    for( xIndex = 0; xIndex < pxService->xNumberOfAttributes; xIndex++ )
    {
        /* Increment by 2 to account for characteristic declaration */
        if( pxService->pxBLEAttributes[ xIndex ].xAttributeType == eBTDbCharacteristic )
        {
            nbHandles += 2;
        }
        else
        {
            nbHandles++;
        }
    }

    return nbHandles;
}

void prvCreateService( BTService_t * xRefSrvc )
{
    BLETESTServiceCallback_t xCreateServiceCb;
    BTStatus_t xStatus = eBTStatusSuccess;
    BTGattSrvcId_t xSrvcId;

    uint16_t usNumHandles = prvComputeNumberOfHandles( xRefSrvc );

    xSrvcId.xId.ucInstId = xRefSrvc->ucInstId;
    xSrvcId.xId.xUuid = xRefSrvc->pxBLEAttributes[ 0 ].xServiceUUID;
    xSrvcId.xServiceType = xRefSrvc->xType;

    xStatus = _pxGattServerInterface->pxAddService( _ucBLEServerIf, &xSrvcId, usNumHandles );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventServiceAddedCb, NO_HANDLE, ( void * ) &xCreateServiceCb, sizeof( BLETESTServiceCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xCreateServiceCb.xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xCreateServiceCb.xSrvcId.xId.xUuid, &xRefSrvc->pxBLEAttributes[ 0 ].xServiceUUID, sizeof( BTUuid_t ) ) );
    TEST_ASSERT_EQUAL( xRefSrvc->ucInstId, xCreateServiceCb.xSrvcId.xId.ucInstId );
    TEST_ASSERT_EQUAL( xRefSrvc->xType, xCreateServiceCb.xSrvcId.xServiceType );

    xRefSrvc->pusHandlesBuffer[ 0 ] = xCreateServiceCb.usAttrHandle;
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
        prvCreateServiceA();

        /* Create service B */
        prvCreateServiceB();

        /* Start service A */
        prvStartService( &_xSrvcA );
        /* Start service B */
        prvStartService( &_xSrvcB );
    }
}

void prvCreateServiceA()
{
    prvCreateService( &_xSrvcA );
    prvCreateCharacteristic( &_xSrvcA, bletestATTR_SRVCA_CHAR_A );
}

void prvCreateServiceB()
{
    prvCreateService( &_xSrvcB );
    prvCreateCharacteristic( &_xSrvcB, bletestATTR_SRVCB_CHAR_A );
    prvCreateCharacteristic( &_xSrvcB, bletestATTR_SRVCB_CHAR_B );
    prvCreateCharacteristic( &_xSrvcB, bletestATTR_SRVCB_CHAR_C );
    prvCreateCharacteristic( &_xSrvcB, bletestATTR_SRVCB_CHAR_D );
    prvCreateCharacteristic( &_xSrvcB, bletestATTR_SRVCB_CHAR_E );
    prvCreateCharacteristicDescriptor( &_xSrvcB, bletestATTR_SRVCB_CCCD_E );
    prvCreateCharacteristic( &_xSrvcB, bletestATTR_SRVCB_CHAR_F );
    prvCreateCharacteristicDescriptor( &_xSrvcB, bletestATTR_SRVCB_CCCD_F );
    prvCreateCharacteristicDescriptor( &_xSrvcB, bletestATTR_SRVCB_CHARF_DESCR_A );
    prvCreateCharacteristicDescriptor( &_xSrvcB, bletestATTR_SRVCB_CHARF_DESCR_B );
    prvCreateCharacteristicDescriptor( &_xSrvcB, bletestATTR_SRVCB_CHARF_DESCR_C );
    prvCreateCharacteristicDescriptor( &_xSrvcB, bletestATTR_SRVCB_CHARF_DESCR_D );
}



TEST( Full_BLE, BLE_Initialize_BLE_GATT )
{
    prvBLEGATTInit();
}

void prvBLEGATTInit()
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTInitDeinitCallback_t xInitDeinitCb;

    _pxGattServerInterface = ( BTGattServerInterface_t * ) _pxBTLeAdapterInterface->ppvGetGattServerInterface();
    TEST_ASSERT_NOT_EQUAL( NULL, _pxGattServerInterface );

    _pxGattServerInterface->pxGattServerInit( &xBTGattServerCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = _pxGattServerInterface->pxRegisterServer( &xServerUUID );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventRegisterUnregisterGattServerCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xInitDeinitCb.xStatus );
}

TEST( Full_BLE, BLE_Initialize_common_GAP )
{
    prvBLEManagerInit();
    prvBLEEnable( true );
}

TEST( Full_BLE, BLE_Initialize_BLE_GAP )
{
    prvBLEGAPInit();
}

void prvBLEManagerInit()
{
    BTStatus_t xStatus = eBTStatusSuccess;

    /* Get BT interface */
    _pxBTInterface = ( BTInterface_t * ) BTGetBluetoothInterface();
    TEST_ASSERT_NOT_EQUAL( NULL, _pxBTInterface );

    /* Initialize callbacks */
    xStatus = _pxBTInterface->pxBtManagerInit( &_xBTManagerCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

void prvBLEGAPInit()
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTInitDeinitCallback_t xInitDeinitCb;

    _pxBTLeAdapterInterface = ( BTBleAdapter_t * ) _pxBTInterface->pxGetLeAdapter();
    TEST_ASSERT_NOT_EQUAL( NULL, _pxBTLeAdapterInterface );

    xStatus = _pxBTLeAdapterInterface->pxBleAdapterInit( &xBTBleAdapterCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = _pxBTLeAdapterInterface->pxRegisterBleApp( &xAppUUID );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventRegisterBleAdapterCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xInitDeinitCb.xStatus );
}

void prvBLESetUp()
{
    BTStatus_t xStatus;

    xStatus = bleStackInit();

    if( xStatus != eBTStatusSuccess )
    {
        TEST_FAIL_MESSAGE( "Unable to initialize BLE stack.\n" );
    }

    /* Create a queue, semaphore and mutexes for callbacks. */
    if( IotMutex_Create( &threadSafetyMutex, false ) != true )
    {
        TEST_FAIL_MESSAGE( "Could not create threadSafetyMutex.\n" );
    }

    if( IotSemaphore_Create( &eventSemaphore, 0, MAX_EVENT ) != true )
    {
        TEST_FAIL_MESSAGE( "Could not create eventSemaphore.\n" );
    }

    IotListDouble_Create( &eventQueueHead );
}

void prvBLEEnable( bool bEnable )
{
    BLETESTInitDeinitCallback_t xInitDeinitCb;
    BTStatus_t xStatus = eBTStatusSuccess;

    /* Enable RADIO and wait for callback. */
    if( bEnable )
    {
        xStatus = _pxBTInterface->pxEnable( 0 );
    }
    else
    {
        xStatus = _pxBTInterface->pxDisable( 0 );
    }

    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventEnableDisableCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    if( bEnable )
    {
        TEST_ASSERT_EQUAL( eBTstateOn, xInitDeinitCb.xBLEState );
    }
    else
    {
        TEST_ASSERT_EQUAL( eBTstateOff, xInitDeinitCb.xBLEState );
    }
}


void prvDeleteService( BTService_t * xRefSrvc )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTServiceCallback_t xStopDeleteServiceCb;

    xStatus = _pxGattServerInterface->pxDeleteService( _ucBLEServerIf, xRefSrvc->pusHandlesBuffer[ 0 ] );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventServiceDeletedCb, xRefSrvc->pusHandlesBuffer[ 0 ], ( void * ) &xStopDeleteServiceCb, sizeof( BLETESTServiceCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStopDeleteServiceCb.xStatus );
}

void prvStopService( BTService_t * xRefSrvc )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTServiceCallback_t xStopDeleteServiceCb;

    xStatus = _pxGattServerInterface->pxStopService( _ucBLEServerIf, xRefSrvc->pusHandlesBuffer[ 0 ] );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventServiceStoppedCb, xRefSrvc->pusHandlesBuffer[ 0 ], ( void * ) &xStopDeleteServiceCb, sizeof( BLETESTServiceCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStopDeleteServiceCb.xStatus );
}

TEST( Full_BLE, BLE_DeInitialize )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    prvStopService( &_xSrvcA );
    prvStopService( &_xSrvcB );

    prvDeleteService( &_xSrvcA );
    prvDeleteService( &_xSrvcB );

    prvBTUnregister();
    prvBLEEnable( false );
    xStatus = _pxBTInterface->pxBtManagerCleanup();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

void prvBTUnregister( void )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTInitDeinitCallback_t xInitDeinitCb;

    xStatus = _pxGattServerInterface->pxUnregisterServer( _ucBLEServerIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = prvWaitEventFromQueue( eBLEHALEventRegisterUnregisterGattServerCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xInitDeinitCb.xStatus );


    xStatus = _pxBTLeAdapterInterface->pxUnregisterBleApp( _ucBLEAdapterIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

void prvRegisterBleAdapterCb( BTStatus_t xStatus,
                              uint8_t ucAdapter_if,
                              BTUuid_t * pxAppUUID )
{
    BLETESTInitDeinitCallback_t * pxInitDeinitCb = IotTest_Malloc( sizeof( BLETESTInitDeinitCallback_t ) );

    pxInitDeinitCb->xStatus = xStatus;
    _ucBLEAdapterIf = ucAdapter_if;

    if( pxInitDeinitCb->xStatus == eBTStatusSuccess )
    {
        if( pxAppUUID == NULL )
        {
            pxInitDeinitCb->xStatus = eBTStatusFail;
        }
    }

    if( pxInitDeinitCb->xStatus == eBTStatusSuccess )
    {
        if( pxAppUUID->ucType != xAppUUID.ucType )
        {
            pxInitDeinitCb->xStatus = eBTStatusFail;
        }
    }

    if( pxInitDeinitCb->xStatus == eBTStatusSuccess )
    {
        if( memcmp( pxAppUUID->uu.uu128, xAppUUID.uu.uu128, bt128BIT_UUID_LEN ) )
        {
            pxInitDeinitCb->xStatus = eBTStatusFail;
        }
    }

    pxInitDeinitCb->xEvent.lHandle = NO_HANDLE;
    pxInitDeinitCb->xEvent.xEventTypes = eBLEHALEventRegisterBleAdapterCb;
    pushToQueue( &pxInitDeinitCb->xEvent.eventList );
}

void prvDeviceStateChangedCb( BTState_t xState )
{
    BLETESTInitDeinitCallback_t * pxInitDeinitCb = IotTest_Malloc( sizeof( BLETESTInitDeinitCallback_t ) );

    pxInitDeinitCb->xBLEState = xState;
    pxInitDeinitCb->xEvent.lHandle = NO_HANDLE;
    pxInitDeinitCb->xEvent.xEventTypes = eBLEHALEventEnableDisableCb;
    pushToQueue( &pxInitDeinitCb->xEvent.eventList );
}

void prvBTRegisterServerCb( BTStatus_t xStatus,
                            uint8_t ucServerIf,
                            BTUuid_t * pxAppUUID )
{
    BLETESTInitDeinitCallback_t * pxInitDeinitCb = IotTest_Malloc( sizeof( BLETESTInitDeinitCallback_t ) );

    pxInitDeinitCb->xStatus = xStatus;
    _ucBLEServerIf = ucServerIf;

    if( pxInitDeinitCb->xStatus == eBTStatusSuccess )
    {
        if( pxAppUUID == NULL )
        {
            pxInitDeinitCb->xStatus = eBTStatusFail;
        }
    }

    if( pxInitDeinitCb->xStatus == eBTStatusSuccess )
    {
        if( pxAppUUID->ucType != xServerUUID.ucType )
        {
            pxInitDeinitCb->xStatus = eBTStatusFail;
        }
    }

    if( pxInitDeinitCb->xStatus == eBTStatusSuccess )
    {
        if( 0 != memcmp( pxAppUUID->uu.uu128, xServerUUID.uu.uu128, bt128BIT_UUID_LEN ) )
        {
            pxInitDeinitCb->xStatus = eBTStatusFail;
        }
    }

    pxInitDeinitCb->xEvent.lHandle = NO_HANDLE;
    pxInitDeinitCb->xEvent.xEventTypes = eBLEHALEventRegisterUnregisterGattServerCb;

    pushToQueue( &pxInitDeinitCb->xEvent.eventList );
}

void prvBTUnregisterServerCb( BTStatus_t xStatus,
                              uint8_t ucServerIf )
{
    BLETESTInitDeinitCallback_t * pxInitDeinitCb = IotTest_Malloc( sizeof( BLETESTInitDeinitCallback_t ) );

    pxInitDeinitCb->xStatus = xStatus;

    if( pxInitDeinitCb->xStatus == eBTStatusSuccess )
    {
        if( ucServerIf != _ucBLEServerIf )
        {
            pxInitDeinitCb->xStatus = eBTStatusFail;
        }
    }

    pxInitDeinitCb->xEvent.lHandle = NO_HANDLE;
    pxInitDeinitCb->xEvent.xEventTypes = eBLEHALEventRegisterUnregisterGattServerCb;

    pushToQueue( &pxInitDeinitCb->xEvent.eventList );
}

void prvServiceAddedCb( BTStatus_t xStatus,
                        uint8_t ucServerIf,
                        BTGattSrvcId_t * pxSrvcId,
                        uint16_t usServiceHandle )
{
    BLETESTServiceCallback_t * pxAddedServiceCb = IotTest_Malloc( sizeof( BLETESTServiceCallback_t ) );

    pxAddedServiceCb->xStatus = xStatus;

    if( pxSrvcId != NULL )
    {
        memcpy( &pxAddedServiceCb->xSrvcId, pxSrvcId, sizeof( BTGattSrvcId_t ) );
    }
    else
    {
        memset( &pxAddedServiceCb->xSrvcId, 0, sizeof( BTGattSrvcId_t ) );
        pxAddedServiceCb->xStatus = eBTStatusFail;
    }

    if( pxAddedServiceCb->xStatus == eBTStatusSuccess )
    {
        if( ucServerIf != _ucBLEServerIf )
        {
            pxAddedServiceCb->xStatus = eBTStatusFail;
        }
    }

    pxAddedServiceCb->usAttrHandle = usServiceHandle;
    pxAddedServiceCb->xEvent.xEventTypes = eBLEHALEventServiceAddedCb;
    pxAddedServiceCb->xEvent.lHandle = NO_HANDLE;

    pushToQueue( &pxAddedServiceCb->xEvent.eventList );
}

void prvServiceStartedCb( BTStatus_t xStatus,
                          uint8_t ucServerIf,
                          uint16_t usServiceHandle )
{
    BLETESTServiceCallback_t * pxStartServiceCb = IotTest_Malloc( sizeof( BLETESTServiceCallback_t ) );

    pxStartServiceCb->xStatus = xStatus;

    if( pxStartServiceCb->xStatus == eBTStatusSuccess )
    {
        if( ucServerIf != _ucBLEServerIf )
        {
            pxStartServiceCb->xStatus = eBTStatusFail;
        }
    }

    pxStartServiceCb->xEvent.xEventTypes = eBLEHALEventServiceStartedCb;
    pxStartServiceCb->xEvent.lHandle = usServiceHandle;

    pushToQueue( &pxStartServiceCb->xEvent.eventList );
}

void prvServiceStoppedCb( BTStatus_t xStatus,
                          uint8_t ucServerIf,
                          uint16_t usServiceHandle )
{
    BLETESTServiceCallback_t * pxStopServiceCb = IotTest_Malloc( sizeof( BLETESTServiceCallback_t ) );

    pxStopServiceCb->xStatus = xStatus;

    if( pxStopServiceCb->xStatus == eBTStatusSuccess )
    {
        if( ucServerIf != _ucBLEServerIf )
        {
            pxStopServiceCb->xStatus = eBTStatusFail;
        }
    }

    pxStopServiceCb->xEvent.xEventTypes = eBLEHALEventServiceStoppedCb;
    pxStopServiceCb->xEvent.lHandle = usServiceHandle;

    pushToQueue( &pxStopServiceCb->xEvent.eventList );
}

void prvServiceDeletedCb( BTStatus_t xStatus,
                          uint8_t ucServerIf,
                          uint16_t usServiceHandle )
{
    BLETESTServiceCallback_t * pxDeleteServiceCb = IotTest_Malloc( sizeof( BLETESTServiceCallback_t ) );

    pxDeleteServiceCb->xStatus = xStatus;

    if( pxDeleteServiceCb->xStatus == eBTStatusSuccess )
    {
        if( ucServerIf != _ucBLEServerIf )
        {
            pxDeleteServiceCb->xStatus = eBTStatusFail;
        }
    }

    pxDeleteServiceCb->xEvent.xEventTypes = eBLEHALEventServiceDeletedCb;
    pxDeleteServiceCb->xEvent.lHandle = usServiceHandle;

    pushToQueue( &pxDeleteServiceCb->xEvent.eventList );
}

void prvCharacteristicAddedCb( BTStatus_t xStatus,
                               uint8_t ucServerIf,
                               BTUuid_t * pxUuid,
                               uint16_t usServiceHandle,
                               uint16_t usCharHandle )
{
    BLETESTAttrCallback_t * pxAttrCb = IotTest_Malloc( sizeof( BLETESTAttrCallback_t ) );

    pxAttrCb->xStatus = xStatus;
    pxAttrCb->usSrvHandle = usServiceHandle;
    pxAttrCb->usAttrHandle = usCharHandle;

    if( pxUuid != NULL )
    {
        memcpy( &pxAttrCb->xUUID, pxUuid, sizeof( BTUuid_t ) );
    }
    else
    {
        memset( &pxAttrCb->xUUID, 0, sizeof( BTUuid_t ) );
        pxAttrCb->xStatus = eBTStatusFail;
    }

    if( pxAttrCb->xStatus == eBTStatusSuccess )
    {
        if( ucServerIf != _ucBLEServerIf )
        {
            pxAttrCb->xStatus = eBTStatusFail;
        }
    }

    pxAttrCb->xEvent.xEventTypes = eBLEHALEventCharAddedCb;
    pxAttrCb->xEvent.lHandle = NO_HANDLE;

    pushToQueue( &pxAttrCb->xEvent.eventList );
}

void prvCharacteristicDescrAddedCb( BTStatus_t xStatus,
                                    uint8_t ucServerIf,
                                    BTUuid_t * pxUuid,
                                    uint16_t usServiceHandle,
                                    uint16_t usCharHandle )
{
    BLETESTAttrCallback_t * pxAttrCb = IotTest_Malloc( sizeof( BLETESTAttrCallback_t ) );

    pxAttrCb->xStatus = xStatus;
    pxAttrCb->usSrvHandle = usServiceHandle;
    pxAttrCb->usAttrHandle = usCharHandle;

    if( pxUuid != NULL )
    {
        memcpy( &pxAttrCb->xUUID, pxUuid, sizeof( BTUuid_t ) );
    }
    else
    {
        memset( &pxAttrCb->xUUID, 0, sizeof( BTUuid_t ) );
        pxAttrCb->xStatus = eBTStatusFail;
    }

    if( pxAttrCb->xStatus == eBTStatusSuccess )
    {
        if( ucServerIf != _ucBLEServerIf )
        {
            pxAttrCb->xStatus = eBTStatusFail;
        }
    }

    pxAttrCb->xEvent.xEventTypes = eBLEHALEventCharDescrAddedCb;
    pxAttrCb->xEvent.lHandle = NO_HANDLE;

    pushToQueue( &pxAttrCb->xEvent.eventList );
}


void prvIncludedServiceAddedCb( BTStatus_t xStatus,
                                uint8_t ucServerIf,
                                uint16_t usServiceHandle,
                                uint16_t usInclSrvcHandle )
{
    BLETESTAttrCallback_t * pxAttrCb = IotTest_Malloc( sizeof( BLETESTAttrCallback_t ) );

    pxAttrCb->xStatus = xStatus;
    pxAttrCb->usSrvHandle = usServiceHandle;
    pxAttrCb->usAttrHandle = usInclSrvcHandle;

    if( pxAttrCb->xStatus == eBTStatusSuccess )
    {
        if( ucServerIf != _ucBLEServerIf )
        {
            pxAttrCb->xStatus = eBTStatusFail;
        }
    }

    pxAttrCb->xEvent.xEventTypes = eBLEHALEventIncludedServiceAdded;
    pxAttrCb->xEvent.lHandle = NO_HANDLE;

    pushToQueue( &pxAttrCb->xEvent.eventList );
}

void prvAdapterPropertiesCb( BTStatus_t xStatus,
                             uint32_t ulNumProperties,
                             BTProperty_t * pxProperties )
{
    BLETESTSetGetPropertyCallback_t * pxSetGetPropertyCb = IotTest_Malloc( sizeof( BLETESTSetGetPropertyCallback_t ) );

    pxSetGetPropertyCb->xStatus = xStatus;
    pxSetGetPropertyCb->ulNumProperties = ulNumProperties;
    pxSetGetPropertyCb->xProperties.xLen = pxProperties->xLen;
    pxSetGetPropertyCb->xProperties.xType = pxProperties->xType;
    pxSetGetPropertyCb->xProperties.pvVal = ucCbPropertyBuffer;

    if( pxProperties->xLen < bletestsMAX_PROPERTY_SIZE )
    {
        switch( pxProperties->xType )
        {
            case eBTpropertyAdapterBondedDevices:

                if( pxProperties->pvVal != NULL )
                {
                    memcpy( pxSetGetPropertyCb->xProperties.pvVal, pxProperties->pvVal, sizeof( BTBdaddr_t ) );
                }
                else
                {
                    memset( pxSetGetPropertyCb->xProperties.pvVal, 0, sizeof( BTBdaddr_t ) );
                }

                break;

            case eBTpropertyBdname:

                if( pxProperties->pvVal != NULL )
                {
                    memcpy( pxSetGetPropertyCb->xProperties.pvVal, pxProperties->pvVal, strlen( bletestsDEVICE_NAME ) );
                }
                else
                {
                    memset( pxSetGetPropertyCb->xProperties.pvVal, 0, strlen( bletestsDEVICE_NAME ) );
                }

                break;

            case eBTpropertyBdaddr:
                pxSetGetPropertyCb->xStatus = eBTStatusFail;
                break;

            case eBTpropertyTypeOfDevice:
                pxSetGetPropertyCb->xStatus = eBTStatusFail;
                break;

            case eBTpropertyLocalMTUSize:
                memcpy( pxSetGetPropertyCb->xProperties.pvVal, pxProperties->pvVal, sizeof( uint16_t ) );
                break;

            case eBTpropertyBondable:
                pxSetGetPropertyCb->xStatus = eBTStatusFail;
                break;

            case eBTpropertyIO:
                pxSetGetPropertyCb->xStatus = eBTStatusFail;
                break;

            case eBTpropertySecureConnectionOnly:
            default:
                pxSetGetPropertyCb->xStatus = eBTStatusFail;
        }
    }
    else
    {
        pxSetGetPropertyCb->xStatus = eBTStatusFail;
    }

    pxSetGetPropertyCb->xEvent.xEventTypes = eBLEHALEventPropertyCb;
    pxSetGetPropertyCb->xEvent.lHandle = NO_HANDLE;

    pushToQueue( &pxSetGetPropertyCb->xEvent.eventList );
}

void prvSetAdvDataCb( BTStatus_t xStatus )
{
    BLETESTAdvParamCallback_t * xdvParamCallback = IotTest_Malloc( sizeof( BLETESTAdvParamCallback_t ) );

    xdvParamCallback->xStatus = xStatus;
    xdvParamCallback->xEvent.xEventTypes = eBLEHALEventSetAdvCb;
    xdvParamCallback->xEvent.lHandle = NO_HANDLE;

    pushToQueue( &xdvParamCallback->xEvent.eventList );
}

void prvAdvStatusCb( BTStatus_t xStatus,
                     uint32_t ulServerIf,
                     bool bStart )
{
    BLETESTAdvParamCallback_t * xdvParamCallback = IotTest_Malloc( sizeof( BLETESTAdvParamCallback_t ) );

    xdvParamCallback->xStatus = xStatus;
    xdvParamCallback->bStart = bStart;
    xdvParamCallback->xEvent.xEventTypes = eBLEHALEventStartAdvCb;
    xdvParamCallback->xEvent.lHandle = NO_HANDLE;

    if( xdvParamCallback->xStatus == eBTStatusSuccess )
    {
        if( ulServerIf != _ucBLEServerIf )
        {
            xdvParamCallback->xStatus = eBTStatusFail;
        }
    }

    pushToQueue( &xdvParamCallback->xEvent.eventList );
}

void prvConnectionCb( uint16_t usConnId,
                      uint8_t ucServerIf,
                      bool bConnected,
                      BTBdaddr_t * pxBda )
{
    BLETESTConnectionCallback_t * pxConnectionCallback = IotTest_Malloc( sizeof( BLETESTConnectionCallback_t ) );

    pxConnectionCallback->xStatus = eBTStatusSuccess;

    if( pxConnectionCallback != NULL )
    {
        pxConnectionCallback->bConnected = bConnected;

        if( pxBda != NULL )
        {
            memcpy( &pxConnectionCallback->pxBda, pxBda, sizeof( BTBdaddr_t ) );
            memcpy( &_xAddressConnectedDevice, pxBda, sizeof( BTBdaddr_t ) );
        }
        else
        {
            pxConnectionCallback->xStatus = eBTStatusFail;
            memset( &pxConnectionCallback->pxBda, 0, sizeof( BTBdaddr_t ) );
            memset( &_xAddressConnectedDevice, 0, sizeof( BTBdaddr_t ) );
        }

        pxConnectionCallback->ucServerIf = ucServerIf;
        pxConnectionCallback->usConnId = usConnId;
        pxConnectionCallback->xEvent.xEventTypes = eBLEHALEventConnectionCb;
        pxConnectionCallback->xEvent.lHandle = NO_HANDLE;
        _usBLEConnId = usConnId;

        pushToQueue( &pxConnectionCallback->xEvent.eventList );
    }
}

void prvConnParameterUpdateCb( BTStatus_t xStatus,
                               const BTBdaddr_t * pxBdAddr,
                               uint32_t minInterval,
                               uint32_t maxInterval,
                               uint32_t latency,
                               uint32_t usConnInterval,
                               uint32_t timeout )
{
    BLETESTUpdateConnectionParamReqCallback_t * pxUpdateConnectionParamReq = IotTest_Malloc( sizeof( BLETESTUpdateConnectionParamReqCallback_t ) );

    pxUpdateConnectionParamReq->xConnParam.minInterval = minInterval;
    pxUpdateConnectionParamReq->xConnParam.maxInterval = maxInterval;
    pxUpdateConnectionParamReq->xConnParam.latency = latency;
    pxUpdateConnectionParamReq->xConnParam.timeout = timeout;
    pxUpdateConnectionParamReq->xEvent.xEventTypes = eBLEHALEventConnectionUpdateCb;
    pxUpdateConnectionParamReq->xEvent.lHandle = NO_HANDLE;

    if( pxBdAddr != NULL )
    {
        memcpy( &pxUpdateConnectionParamReq->xBda, pxBdAddr, sizeof( BTBdaddr_t ) );
    }
    else
    {
        memset( &pxUpdateConnectionParamReq->xBda, 0, sizeof( BTBdaddr_t ) );
    }

    usCbConnInterval = usConnInterval;

    pushToQueue( &pxUpdateConnectionParamReq->xEvent.eventList );
}
void prvRequestReadCb( uint16_t usConnId,
                       uint32_t ulTransId,
                       BTBdaddr_t * pxBda,
                       uint16_t usAttrHandle,
                       uint16_t usOffset )
{
    BLETESTreadAttrCallback_t * pxreadAttrCallback = IotTest_Malloc( sizeof( BLETESTreadAttrCallback_t ) );

    if( pxreadAttrCallback != NULL )
    {
        pxreadAttrCallback->usConnId = usConnId;
        pxreadAttrCallback->ulTransId = ulTransId;

        if( pxBda != NULL )
        {
            memcpy( &pxreadAttrCallback->xBda, pxBda, sizeof( BTBdaddr_t ) );
        }
        else
        {
            memset( &pxreadAttrCallback->xBda, 0, sizeof( BTBdaddr_t ) );
        }

        pxreadAttrCallback->usAttrHandle = usAttrHandle;
        pxreadAttrCallback->usOffset = usOffset;
        pxreadAttrCallback->xEvent.xEventTypes = eBLEHALEventReadAttrCb;
        pxreadAttrCallback->xEvent.lHandle = usAttrHandle;

        pushToQueue( &pxreadAttrCallback->xEvent.eventList );
    }
}

void prvRequestWriteCb( uint16_t usConnId,
                        uint32_t ulTransId,
                        BTBdaddr_t * pxBda,
                        uint16_t usAttrHandle,
                        uint16_t usOffset,
                        size_t xLength,
                        bool bNeedRsp,
                        bool bIsPrep,
                        uint8_t * pucValue )
{
    BLETESTwriteAttrCallback_t * pxWriteAttrCallback = IotTest_Malloc( sizeof( BLETESTwriteAttrCallback_t ) );

    if( pxWriteAttrCallback != NULL )
    {
        pxWriteAttrCallback->usConnId = usConnId;
        pxWriteAttrCallback->ulTransId = ulTransId;

        if( pxBda != NULL )
        {
            memcpy( &pxWriteAttrCallback->xBda, pxBda, sizeof( BTBdaddr_t ) );
        }
        else
        {
            memset( &pxWriteAttrCallback->xBda, 0, sizeof( BTBdaddr_t ) );
        }

        pxWriteAttrCallback->usOffset = usOffset;
        pxWriteAttrCallback->xLength = xLength;
        pxWriteAttrCallback->bNeedRsp = bNeedRsp;
        pxWriteAttrCallback->bIsPrep = bIsPrep;
        pxWriteAttrCallback->usAttrHandle = usAttrHandle;

        if( ( xLength <= bletestsSTRINGYFIED_UUID_SIZE ) && ( pucValue != NULL ) )
        {
            memcpy( pxWriteAttrCallback->ucValue, pucValue, xLength );
        }
        else
        {
            memset( pxWriteAttrCallback->ucValue, 0, bletestsSTRINGYFIED_UUID_SIZE );
        }

        pxWriteAttrCallback->xEvent.xEventTypes = eBLEHALEventWriteAttrCb;
        pxWriteAttrCallback->xEvent.lHandle = usAttrHandle;

        pushToQueue( &pxWriteAttrCallback->xEvent.eventList );
    }

/*
 *  BTGattResponse_t xGattResponse;
 *
 *  if ( xLength <= bletestsSTRINGYFIED_UUID_SIZE)
 *  {
 *      memcpy(ucResponseBuffer, pucValue, bletestsSTRINGYFIED_UUID_SIZE);
 *  }else
 *  {
 *      memset(ucResponseBuffer, 0, bletestsSTRINGYFIED_UUID_SIZE);
 *  }
 *  xGattResponse.usHandle = usAttrHandle;
 *  xGattResponse.xAttrValue.pucValue = ucResponseBuffer;
 *  xGattResponse.xAttrValue.xLen = bletestsSTRINGYFIED_UUID_SIZE;
 *  xGattResponse.xAttrValue.usOffset = 0;
 *  xGattResponse.xAttrValue.xRspErrorStatus = eBTRspErrorNone;
 */
}

void * checkQueueForEvent( BLEHALEventsTypes_t xEventName,
                           int32_t lhandle )
{
    BLEHALEventsInternals_t * pEventIndex;
    IotLink_t * pEventListIndex;
    void * pvPtr = NULL;

    IotMutex_Lock( &threadSafetyMutex );

    /* Get the event associated to the callback */
    IotContainers_ForEach( &eventQueueHead, pEventListIndex )
    {
        pEventIndex = IotLink_Container( BLEHALEventsInternals_t, pEventListIndex, eventList );

        if( ( pEventIndex->xEventTypes == xEventName ) &&
            ( pEventIndex->lHandle == lhandle ) )
        {
            pvPtr = pEventIndex;
            IotListDouble_Remove( &pEventIndex->eventList );
            break; /* If the right event is received, exit. */
        }
    }

    IotMutex_Unlock( &threadSafetyMutex );

    return pvPtr;
}

/* This function first check if an event is waiting in the list. If not, it will go and wait on the queue.
 * When an event is received on the queue, if it is not the expected event, it goes on the waiting list.
 */
BTStatus_t prvWaitEventFromQueue( BLEHALEventsTypes_t xEventName,
                                  int32_t lhandle,
                                  void * pxMessage,
                                  size_t xMessageLength,
                                  uint32_t timeoutMs )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    void * pvPtr = NULL;

    pvPtr = checkQueueForEvent( xEventName, lhandle );

    /* If event is not waiting then wait for it. */
    if( pvPtr == NULL )
    {
        do
        {
            /* TODO check event list here */
            if( IotSemaphore_TimedWait( &eventSemaphore, timeoutMs ) == true )
            {
                pvPtr = checkQueueForEvent( xEventName, lhandle );

                if( pvPtr != NULL )
                {
                    break; /* If the right event is received, exit. */
                }
            }
            else
            {
                xStatus = eBTStatusFail;
            }
        }
        while( xStatus == eBTStatusSuccess ); /* If there is an error exit */
    }

    if( xStatus == eBTStatusSuccess )
    {
        memcpy( pxMessage, pvPtr, xMessageLength );
        IotTest_Free( pvPtr );
    }

    return xStatus;
}

void prvIndicationSentCb( uint16_t usConnId,
                          BTStatus_t xStatus )
{
    BLETESTindicateCallback_t * pxIndicateCallback = IotTest_Malloc( sizeof( BLETESTindicateCallback_t ) );

    if( pxIndicateCallback != NULL )
    {
        pxIndicateCallback->xEvent.xEventTypes = eBLEHALEventIndicateCb;
        pxIndicateCallback->xEvent.lHandle = NO_HANDLE;
        pxIndicateCallback->usConnId = usConnId;
        pxIndicateCallback->xStatus = xStatus;

        pushToQueue( &pxIndicateCallback->xEvent.eventList );
    }
}

void prvResponseConfirmationCb( BTStatus_t xStatus,
                                uint16_t usHandle )
{
    BLETESTconfirmCallback_t * pxConfirmCallback = IotTest_Malloc( sizeof( BLETESTconfirmCallback_t ) );

    if( pxConfirmCallback != NULL )
    {
        pxConfirmCallback->xEvent.xEventTypes = eBLEHALEventConfimCb;
        pxConfirmCallback->xEvent.lHandle = usHandle;
        pxConfirmCallback->usAttrHandle = usHandle;
        pxConfirmCallback->xStatus = xStatus;

        pushToQueue( &pxConfirmCallback->xEvent.eventList );
    }
}

void prvSspRequestCb( BTBdaddr_t * pxRemoteBdAddr,
                      BTBdname_t * pxRemoteBdName,
                      uint32_t ulCod,
                      BTSspVariant_t xPairingVariant,
                      uint32_t ulPassKey )
{
    BLETESTsspRequestCallback_t * pxSSPrequestCallback = IotTest_Malloc( sizeof( BLETESTsspRequestCallback_t ) );

    if( pxSSPrequestCallback != NULL )
    {
        if( pxRemoteBdAddr != NULL )
        {
            memcpy( &pxSSPrequestCallback->xRemoteBdAddr, pxRemoteBdAddr, sizeof( BTBdaddr_t ) );
        }
        else
        {
            memset( &pxSSPrequestCallback->xRemoteBdAddr, 0, sizeof( BTBdaddr_t ) );
        }

        pxSSPrequestCallback->xEvent.lHandle = NO_HANDLE;
        pxSSPrequestCallback->ulCod = ulCod;
        pxSSPrequestCallback->xPairingVariant = xPairingVariant;
        pxSSPrequestCallback->ulPassKey = ulPassKey;

        switch( pxSSPrequestCallback->xPairingVariant )
        {
            case eBTsspVariantPasskeyConfirmation:
                pxSSPrequestCallback->xEvent.xEventTypes = eBLEHALEventSSPrequestConfirmationCb;
                break;

            case eBTsspVariantConsent:
                pxSSPrequestCallback->xEvent.xEventTypes = eBLEHALEventSSPrequestCb;
                break;

            case eBTsspVariantPasskeyNotification:
            case eBTsspVariantPasskeyEntry:
            default:
                pxSSPrequestCallback->xEvent.xEventTypes = eBLEHALEventSSPrequestCb;
        }

        pushToQueue( &pxSSPrequestCallback->xEvent.eventList );
    }
}

void prvPairingStateChangedCb( BTStatus_t xStatus,
                               BTBdaddr_t * pxRemoteBdAddr,
                               BTBondState_t xState,
                               BTSecurityLevel_t xSecurityLevel,
                               BTAuthFailureReason_t xReason )
{
    BLETESTPairingStateChangedCallback_t * pxPairingStateChangedCallback = IotTest_Malloc( sizeof( BLETESTPairingStateChangedCallback_t ) );

    if( pxPairingStateChangedCallback != NULL )
    {
        pxPairingStateChangedCallback->xEvent.xEventTypes = eBLEHALEventPairingStateChangedCb;
        pxPairingStateChangedCallback->xEvent.lHandle = NO_HANDLE;
        pxPairingStateChangedCallback->xStatus = xStatus;

        if( pxRemoteBdAddr != NULL )
        {
            memcpy( &pxPairingStateChangedCallback->xRemoteBdAddr, pxRemoteBdAddr, sizeof( BTBdaddr_t ) );
        }
        else
        {
            memset( &pxPairingStateChangedCallback->xRemoteBdAddr, 0, sizeof( BTBdaddr_t ) );
        }

        pxPairingStateChangedCallback->xSecurityLevel = xSecurityLevel;
        pxPairingStateChangedCallback->xReason = xReason;

        pushToQueue( &pxPairingStateChangedCallback->xEvent.eventList );
    }
}

void prvRequestExecWriteCb( uint16_t usConnId,
                            uint32_t ulTransId,
                            BTBdaddr_t * pxBda,
                            bool bExecWrite )
{
    BLETESTRequestExecWriteCallback_t * pxRequestExecWriteCallback = IotTest_Malloc( sizeof( BLETESTRequestExecWriteCallback_t ) );

    if( pxRequestExecWriteCallback != NULL )
    {
        pxRequestExecWriteCallback->xEvent.xEventTypes = eBLEHALEventRequestExecWriteCb;
        pxRequestExecWriteCallback->xEvent.lHandle = NO_HANDLE;
        pxRequestExecWriteCallback->ulTransId = ulTransId;
        pxRequestExecWriteCallback->usConnId = usConnId;

        if( pxBda != NULL )
        {
            memcpy( &pxRequestExecWriteCallback->xBda, pxBda, sizeof( BTBdaddr_t ) );
        }
        else
        {
            memset( &pxRequestExecWriteCallback->xBda, 0, sizeof( BTBdaddr_t ) );
        }

        pxRequestExecWriteCallback->bExecWrite = bExecWrite;

        pushToQueue( &pxRequestExecWriteCallback->xEvent.eventList );
    }
}

void prvBondedCb( BTStatus_t xStatus,
                  BTBdaddr_t * pxRemoteBdAddr,
                  bool bIsBonded )
{
    BLETESTBondedCallback_t * pxBondedCallback = IotTest_Malloc( sizeof( BLETESTBondedCallback_t ) );

    if( pxBondedCallback != NULL )
    {
        pxBondedCallback->bIsBonded = bIsBonded;

        if( pxRemoteBdAddr != NULL )
        {
            memcpy( &pxBondedCallback->xRemoteBdAddr, pxRemoteBdAddr, sizeof( BTBdaddr_t ) );
        }
        else
        {
            memset( &pxBondedCallback->xRemoteBdAddr, 0, sizeof( BTBdaddr_t ) );
        }

        pxBondedCallback->xStatus = xStatus;
        pxBondedCallback->xEvent.xEventTypes = eBLEHALEventBondedCb;
        pxBondedCallback->xEvent.lHandle = NO_HANDLE;

        pushToQueue( &pxBondedCallback->xEvent.eventList );
    }
}

void pushToQueue( IotLink_t * pEventList )
{
    IotMutex_Lock( &threadSafetyMutex );

    IotListDouble_InsertHead( &eventQueueHead,
                              pEventList );
    IotSemaphore_Post( &eventSemaphore );

    IotMutex_Unlock( &threadSafetyMutex );
}
