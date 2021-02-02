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
 * @file iot_test_ble_hal_common.c
 * @brief Tests for ble.
 */
#include "iot_ble_config.h"
#include "iot_test_ble_hal_common.h"

void pushToQueue( IotLink_t * pEventList );

BTGattServerInterface_t * _pxGattServerInterface;
BTBleAdapter_t * _pxBTLeAdapterInterface;
BTInterface_t * _pxBTInterface;
uint8_t _ucBLEAdapterIf;
uint8_t _ucBLEServerIf;
uint16_t _usBLEConnId;
BTBdaddr_t _xAddressConnectedDevice;

BTService_t _xSrvcA;
BTService_t _xSrvcB;
BTService_t _xSrvcC;
uint16_t usHandlesBufferB[ bletestATTR_SRVCB_NUMBER ];
response_t ucRespBuffer[ bletestATTR_SRVCB_NUMBER ];

uint16_t usHandlesBufferA[ bletestATTR_SRVCA_NUMBER ];
uint16_t usHandlesBufferB[ bletestATTR_SRVCB_NUMBER ];
uint16_t usHandlesBufferC[ bletestATTR_SRVCC_NUMBER ];

uint8_t ucCbPropertyBuffer[ bletestsMAX_PROPERTY_SIZE ];
uint32_t usCbConnInterval;
uint16_t _bletestsMTU_SIZE = IOT_BLE_PREFERRED_MTU_SIZE;

const int ServiceB_CharNumber = 6;
const int ServiceB_CharArray[] = { bletestATTR_SRVCB_CHAR_A, bletestATTR_SRVCB_CHAR_B, bletestATTR_SRVCB_CHAR_C, bletestATTR_SRVCB_CHAR_D, bletestATTR_SRVCB_CHAR_E, bletestATTR_SRVCB_CHAR_F };
int ServiceB_Char = 0;
bool bCharAddedComplete = false;

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
    #if ENABLE_TC_AFQP_ADD_INCLUDED_SERVICE
        {
            .xAttributeType = eBTDbIncludedService,
            .xIncludedService =
            {
                .xUuid          = bletestsFREERTOS_SVC_A_UUID,
                .pxPtrToService = &_xSrvcA
            }
        },
    #endif
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
    }
};

/* service C */

static const BTAttribute_t pxAttributeTableC[] =
{
    {
        .xAttributeType = eBTDbSecondaryService,
        .xServiceUUID = bletestsFREERTOS_SVC_C_UUID
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid        = bletestsFREERTOS_CHAR_G_UUID,
            .xPermissions = ( bletestsFULL_PERMISSIONS ),
            .xProperties  = ( eBTPropRead | eBTPropWrite )
        }
    }
};

BTService_t _xSrvcC =
{
    .ucInstId            = 0,
    .xType               = eBTServiceTypeSecondary,
    .xNumberOfAttributes = bletestATTR_SRVCC_NUMBER,
    .pusHandlesBuffer    = usHandlesBufferC,
    .pxBLEAttributes     = ( BTAttribute_t * ) pxAttributeTableC
};

BTGattAdvertismentParams_t xAdvertisementConfigA =
{
    .usAdvertisingEventProperties = BTAdvInd,
    .bIncludeTxPower              = true,
    .ucName                       = { BTGattAdvNameNone,               0},
    .bSetScanRsp                  = false,
    .ulAppearance                 = 0,
    .ulMinInterval                = 0,
    .ulMaxInterval                = 0,
    .usMinAdvInterval             = bletestsMIN_ADVERTISEMENT_INTERVAL,
    .usMaxAdvInterval             = bletestsMAX_ADVERTISEMENT_INTERVAL,
    .ucChannelMap                 = 0,
    .ucPrimaryAdvertisingPhy      = 0,
    .ucSecondaryAdvertisingPhy    = 0,
    .xAddrType                    = BTAddrTypePublic,
};

BTGattAdvertismentParams_t xAdvertisementConfigB =
{
    .usAdvertisingEventProperties = BTAdvInd,
    .bIncludeTxPower              = true,
    .ucName                       = { BTGattAdvNameShort,              4},
    .bSetScanRsp                  = true,
    .ulAppearance                 = 0,
    .ulMinInterval                = bletestsMIN_CONNECTION_INTERVAL,
    .ulMaxInterval                = bletestsMAX_CONNECTION_INTERVAL,
    .usMinAdvInterval             = bletestsMIN_ADVERTISEMENT_INTERVAL,
    .usMaxAdvInterval             = bletestsMAX_ADVERTISEMENT_INTERVAL,
    .ucChannelMap                 = 0,
    .ucPrimaryAdvertisingPhy      = 0,
    .ucSecondaryAdvertisingPhy    = 0,
    .xAddrType                    = BTAddrTypePublic,
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

BTService_t _xSrvcB =
{
    .ucInstId            = 0,
    .xType               = eBTServiceTypePrimary,
    .xNumberOfAttributes = bletestATTR_SRVCB_NUMBER,
    .pusHandlesBuffer    = usHandlesBufferB,
    .pxBLEAttributes     = ( BTAttribute_t * ) pxAttributeTableB
};

IotListDouble_t eventQueueHead;
IotMutex_t threadSafetyMutex;
IotSemaphore_t eventSemaphore;

response_t ucRespBuffer[ bletestATTR_SRVCB_NUMBER ];

BTCallbacks_t _xBTManagerCb =
{
    .pxDeviceStateChangedCb     = prvDeviceStateChangedCb,
    .pxAdapterPropertiesCb      = prvAdapterPropertiesCb,
    .pxRemoteDevicePropertiesCb = NULL,
    .pxSspRequestCb             = prvSspRequestCb,
    .pxPairingStateChangedCb    = prvPairingStateChangedCb,
    .pxDutModeRecvCb            = NULL,
    .pxleTestModeCb             = NULL,
    .pxEnergyInfoCb             = NULL,
    .pxReadRssiCb               = NULL,
    .pxTxPowerCb                = NULL,
    .pxSlaveSecurityRequestCb   = NULL,
};

BTBleAdapterCallbacks_t _xBTBleAdapterCb =
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


BTGattServerCallbacks_t _xBTGattServerCb =
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
    .pxMtuChangedCb           = prvMtuChangedCb
};

BTCallbacks_t _xBTManager_NULL_Cb =
{
    .pxDeviceStateChangedCb     = NULL,
    .pxAdapterPropertiesCb      = prvAdapterPropertiesCb,
    .pxRemoteDevicePropertiesCb = NULL,
    .pxSspRequestCb             = NULL,
    .pxPairingStateChangedCb    = NULL,
    .pxDutModeRecvCb            = NULL,
    .pxleTestModeCb             = NULL,
    .pxEnergyInfoCb             = NULL,
    .pxReadRssiCb               = NULL,
    .pxTxPowerCb                = NULL,
    .pxSlaveSecurityRequestCb   = NULL,
};

BTBleAdapterCallbacks_t _xBTBleAdapter_NULL_Cb =
{
    .pxRegisterBleAdapterCb          = NULL,
    .pxScanResultCb                  = NULL,
    .pxBleAdapterPropertiesCb        = NULL,
    .pxBleRemoteDevicePropertiesCb   = NULL,
    .pxOpenCb                        = NULL,
    .pxCloseCb                       = NULL,
    .pxReadRemoteRssiCb              = NULL,
    .pxAdvStatusCb                   = NULL,
    .pxSetAdvDataCb                  = NULL,
    .pxConnParameterUpdateCb         = NULL,
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

BTGattServerCallbacks_t _xBTGattServer_NULL_Cb =
{
    .pxRegisterServerCb       = prvBTRegisterServerCb,
    .pxUnregisterServerCb     = NULL,
    .pxConnectionCb           = prvConnectionCb,
    .pxServiceAddedCb         = prvServiceAddedCb,
    .pxIncludedServiceAddedCb = prvIncludedServiceAddedCb,
    .pxCharacteristicAddedCb  = prvCharacteristicAddedCb,
    .pxSetValCallbackCb       = NULL,
    .pxDescriptorAddedCb      = prvCharacteristicDescrAddedCb,
    .pxServiceStartedCb       = NULL,
    .pxServiceStoppedCb       = NULL,
    .pxServiceDeletedCb       = NULL,
    .pxRequestReadCb          = NULL,
    .pxRequestWriteCb         = NULL,
    .pxRequestExecWriteCb     = NULL,
    .pxResponseConfirmationCb = NULL,
    .pxIndicationSentCb       = NULL,
    .pxCongestionCb           = NULL,
    .pxMtuChangedCb           = NULL
};

BTGattServerCallbacks_t _xBTGattServer_Nested_Cb =
{
    .pxRegisterServerCb       = prvBTRegisterServerCb,
    .pxUnregisterServerCb     = prvBTUnregisterServerCb,
    .pxConnectionCb           = prvConnectionCb,
    .pxServiceAddedCb         = prvServiceAddedCb,
    .pxIncludedServiceAddedCb = prvIncludedServiceAddedCb,
    .pxCharacteristicAddedCb  = prvCharAddedNestedCb,
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
    .pxMtuChangedCb           = prvMtuChangedCb
};

void IotTestBleHal_BLESetUp()
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

void IotTestBleHal_BLEFree( void )
{
    IotTestBleHal_ClearEventQueue();
    IotMutex_Destroy( &threadSafetyMutex );
    IotSemaphore_Destroy( &eventSemaphore );
}

void IotTestBleHal_BLEManagerInit( BTCallbacks_t * pBTmanagerCb )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    /* Get BT interface */
    _pxBTInterface = ( BTInterface_t * ) BTGetBluetoothInterface();
    TEST_ASSERT_NOT_EQUAL( NULL, _pxBTInterface );

    /* Initialize callbacks */
    xStatus = _pxBTInterface->pxBtManagerInit( pBTmanagerCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

void IotTestBleHal_BLEGAPInit( BTBleAdapterCallbacks_t * pBTBleAdapterCb,
                               bool bEnableCb )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTInitDeinitCallback_t xInitDeinitCb;

    _pxBTLeAdapterInterface = ( BTBleAdapter_t * ) _pxBTInterface->pxGetLeAdapter();
    TEST_ASSERT_NOT_EQUAL( NULL, _pxBTLeAdapterInterface );

    xStatus = _pxBTLeAdapterInterface->pxBleAdapterInit( pBTBleAdapterCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = _pxBTLeAdapterInterface->pxRegisterBleApp( &xAppUUID );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    if( bEnableCb == true )
    {
        xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventRegisterBleAdapterCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xInitDeinitCb.xStatus );
    }
}

void IotTestBleHal_BLEGATTInit( BTGattServerCallbacks_t * pBTGattServerCb,
                                bool bEnableCb )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTInitDeinitCallback_t xInitDeinitCb;

    _pxGattServerInterface = ( BTGattServerInterface_t * ) _pxBTLeAdapterInterface->ppvGetGattServerInterface();
    TEST_ASSERT_NOT_EQUAL( NULL, _pxGattServerInterface );

    _pxGattServerInterface->pxGattServerInit( pBTGattServerCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = _pxGattServerInterface->pxRegisterServer( &xServerUUID );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    if( bEnableCb == true )
    {
        xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventRegisterUnregisterGattServerCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xInitDeinitCb.xStatus );
    }
}

void IotTestBleHal_StartAdvertisement( void )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = _pxBTLeAdapterInterface->pxStartAdv( _ucBLEAdapterIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    IotTestBleHal_StartStopAdvCheck( true );
}

void IotTestBleHal_BLEEnable( bool bEnable )
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
        xStatus = _pxBTInterface->pxDisable();
    }

    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventEnableDisableCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
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

void IotTestBleHal_StartService( BTService_t * xRefSrvc )
{
    BLETESTServiceCallback_t xStartServiceCb;
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = _pxGattServerInterface->pxStartService( _ucBLEServerIf, xRefSrvc->pusHandlesBuffer[ 0 ], BTTransportLe );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventServiceStartedCb, xRefSrvc->pusHandlesBuffer[ 0 ], ( void * ) &xStartServiceCb, sizeof( BLETESTServiceCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStartServiceCb.xStatus );
}

size_t IotTestBleHal_ComputeNumberOfHandles( BTService_t * pxService )
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

static void prvCreateService( BTService_t * xRefSrvc )
{
    BLETESTServiceCallback_t xCreateServiceCb;
    BTStatus_t xStatus = eBTStatusSuccess;
    BTGattSrvcId_t xSrvcId;

    uint16_t usNumHandles = IotTestBleHal_ComputeNumberOfHandles( xRefSrvc );

    xSrvcId.xId.ucInstId = xRefSrvc->ucInstId;
    xSrvcId.xId.xUuid = xRefSrvc->pxBLEAttributes[ 0 ].xServiceUUID;
    xSrvcId.xServiceType = xRefSrvc->xType;

    xStatus = _pxGattServerInterface->pxAddService( _ucBLEServerIf, &xSrvcId, usNumHandles );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventServiceAddedCb, NO_HANDLE, ( void * ) &xCreateServiceCb, sizeof( BLETESTServiceCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xCreateServiceCb.xStatus );
    TEST_ASSERT_EQUAL( 0, memcmp( &xCreateServiceCb.xSrvcId.xId.xUuid, &xRefSrvc->pxBLEAttributes[ 0 ].xServiceUUID, sizeof( BTUuid_t ) ) );
    TEST_ASSERT_EQUAL( xRefSrvc->ucInstId, xCreateServiceCb.xSrvcId.xId.ucInstId );
    TEST_ASSERT_EQUAL( xRefSrvc->xType, xCreateServiceCb.xSrvcId.xServiceType );

    xRefSrvc->pusHandlesBuffer[ 0 ] = xCreateServiceCb.usAttrHandle;
}

static void prvCreateCharacteristicDescriptor( BTService_t * xSrvc,
                                               int xAttribute )
{
    BLETESTAttrCallback_t xBLETESTCharDescrCb;
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = _pxGattServerInterface->pxAddDescriptor( _ucBLEServerIf,
                                                       xSrvc->pusHandlesBuffer[ 0 ],
                                                       &xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristicDescr.xUuid,
                                                       xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristicDescr.xPermissions );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventCharDescrAddedCb, NO_HANDLE, ( void * ) &xBLETESTCharDescrCb, sizeof( BLETESTAttrCallback_t ), BLE_TESTS_WAIT );
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

static void prvCreateCharacteristic( BTService_t * xSrvc,
                                     int xAttribute,
                                     bool bEnableCb )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTAttrCallback_t xBLETESTCharCb;

    xStatus = _pxGattServerInterface->pxAddCharacteristic( _ucBLEServerIf,
                                                           xSrvc->pusHandlesBuffer[ 0 ],
                                                           &xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristic.xUuid,
                                                           xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristic.xProperties,
                                                           xSrvc->pxBLEAttributes[ xAttribute ].xCharacteristic.xPermissions );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    bCharAddedComplete = true;

    if( bEnableCb == true )
    {
        xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventCharAddedCb, NO_HANDLE, ( void * ) &xBLETESTCharCb, sizeof( BLETESTAttrCallback_t ), BLE_TESTS_WAIT );
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
}

void prvCreateIncludedService( BTService_t * xSrvc,
                               int xAttribute )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTAttrCallback_t xBLETESTIncludedSvcCb;

    xStatus = _pxGattServerInterface->pxAddIncludedService( _ucBLEServerIf,
                                                            xSrvc->pusHandlesBuffer[ 0 ],
                                                            _xSrvcA.pusHandlesBuffer[ 0 ] );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventIncludedServiceAdded, NO_HANDLE, ( void * ) &xBLETESTIncludedSvcCb, sizeof( BLETESTAttrCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xBLETESTIncludedSvcCb.xStatus );
    TEST_ASSERT_EQUAL( xSrvc->pusHandlesBuffer[ 0 ], xBLETESTIncludedSvcCb.usSrvHandle );
    xSrvc->pusHandlesBuffer[ xAttribute ] = xBLETESTIncludedSvcCb.usAttrHandle;
}

void IotTestBleHal_CreateStartServiceA()
{
    BTStatus_t xStatus;

    /* Try to create using blob service API first.
     * If blob is not supported then try legacy APIs. */
    xStatus = _pxGattServerInterface->pxAddServiceBlob( _ucBLEServerIf, &_xSrvcA );

    if( xStatus == eBTStatusUnsupported )
    {
        /* Create service A */
        IotTestBleHal_CreateServiceA();
        /* Start service A */
        IotTestBleHal_StartService( &_xSrvcA );
    }
}

void IotTestBleHal_CreateStartServiceB( bool bEnableIncludedSrvice )
{
    BTStatus_t xStatus;

    /* Try to create using blob service API first.
     * If blob is not supported then try legacy APIs. */
    xStatus = _pxGattServerInterface->pxAddServiceBlob( _ucBLEServerIf, &_xSrvcB );

    if( xStatus == eBTStatusUnsupported )
    {
        /* Create service B */
        IotTestBleHal_CreateServiceB( bEnableIncludedSrvice );
        /* Start service B */
        IotTestBleHal_StartService( &_xSrvcB );
    }
}

void IotTestBleHal_CreateStartServiceC()
{
    BTStatus_t xStatus;

    /* Try to create using blob service API first.
     * If blob is not supported then try legacy APIs. */
    xStatus = _pxGattServerInterface->pxAddServiceBlob( _ucBLEServerIf, &_xSrvcC );

    if( xStatus == eBTStatusUnsupported )
    {
        /* Create service C */
        IotTestBleHal_CreateServiceC();
        /* Start service C */
        IotTestBleHal_StartService( &_xSrvcC );
    }
}

void IotTestBleHal_CreateServiceA()
{
    prvCreateService( &_xSrvcA );
    prvCreateCharacteristic( &_xSrvcA, bletestATTR_SRVCA_CHAR_A, true );
}

void IotTestBleHal_CreateServiceB( bool bEnableIncludedSrvice )
{
    prvCreateService( &_xSrvcB );

    if( bEnableIncludedSrvice )
    {
        #if ENABLE_TC_AFQP_ADD_INCLUDED_SERVICE
            prvCreateIncludedService( &_xSrvcB, bletestATTR_INCLUDED_SERVICE );
        #endif
    }

    prvCreateCharacteristic( &_xSrvcB, bletestATTR_SRVCB_CHAR_A, true );
    prvCreateCharacteristic( &_xSrvcB, bletestATTR_SRVCB_CHAR_B, true );
    prvCreateCharacteristic( &_xSrvcB, bletestATTR_SRVCB_CHAR_C, true );
    prvCreateCharacteristic( &_xSrvcB, bletestATTR_SRVCB_CHAR_D, true );
    prvCreateCharacteristic( &_xSrvcB, bletestATTR_SRVCB_CHAR_E, true );
    prvCreateCharacteristicDescriptor( &_xSrvcB, bletestATTR_SRVCB_CCCD_E );
    prvCreateCharacteristic( &_xSrvcB, bletestATTR_SRVCB_CHAR_F, true );
    prvCreateCharacteristicDescriptor( &_xSrvcB, bletestATTR_SRVCB_CCCD_F );
    prvCreateCharacteristicDescriptor( &_xSrvcB, bletestATTR_SRVCB_CHARF_DESCR_A );
    prvCreateCharacteristicDescriptor( &_xSrvcB, bletestATTR_SRVCB_CHARF_DESCR_B );
    prvCreateCharacteristicDescriptor( &_xSrvcB, bletestATTR_SRVCB_CHARF_DESCR_C );
    prvCreateCharacteristicDescriptor( &_xSrvcB, bletestATTR_SRVCB_CHARF_DESCR_D );
}

void IotTestBleHal_CreateServiceC()
{
    prvCreateService( &_xSrvcC );
    prvCreateCharacteristic( &_xSrvcC, bletestATTR_SRVCC_CHAR_A, true );
}

#if ENABLE_TC_INTEGRATION_ADD_CHARACTERISTIC_IN_CALLBACK
    void IotTestBleHal_CreateServiceB_Nested()
    {
        prvCreateService( &_xSrvcB );
        prvCreateCharacteristic( &_xSrvcB, ServiceB_CharArray[ ServiceB_Char++ ], false );
    }
#endif

void IotTestBleHal_WriteCheckAndResponse( bletestAttSrvB_t xAttribute,
                                          bool bNeedRsp,
                                          bool IsPrep,
                                          uint16_t usOffset )
{
    BLETESTwriteAttrCallback_t xWriteEvent;

    xWriteEvent = IotTestBleHal_WriteReceive( xAttribute, bNeedRsp, IsPrep, usOffset );

    if( xWriteEvent.bNeedRsp == true ) /* this flag is different depending on different stack implementation */
    {
        IotTestBleHal_WriteResponse( xAttribute, xWriteEvent, true );
    }
}

static void prvSetAdvertisement( BTGattAdvertismentParams_t * pxParams,
                                 uint16_t usServiceDataLen,
                                 char * pcServiceData,
                                 BTUuid_t * pxServiceUuid,
                                 size_t xNbServices,
                                 uint16_t usManufacturerLen,
                                 char * pcManufacturerData )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTAdvParamCallback_t xAdvParamCallback;

    xStatus = _pxBTLeAdapterInterface->pxSetAdvData( _ucBLEAdapterIf,
                                                     pxParams,
                                                     usManufacturerLen,
                                                     pcManufacturerData,
                                                     usServiceDataLen,
                                                     pcServiceData,
                                                     pxServiceUuid,
                                                     xNbServices );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventSetAdvCb, NO_HANDLE, ( void * ) &xAdvParamCallback, sizeof( BLETESTAdvParamCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xAdvParamCallback.xStatus );
}


void IotTestBleHal_SetAdvData( BTuuidType_t type,
                               uint16_t usServiceDataLen,
                               char * pcServiceData,
                               uint16_t usManufacturerLen,
                               char * pcManufacturerData )
{
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

    xNbServices = 1;

    l_xAdvertisementConfigA = xAdvertisementConfigA;
    l_xAdvertisementConfigB = xAdvertisementConfigB;

    prvSetAdvertisement( &l_xAdvertisementConfigA,
                         usServiceDataLen,
                         pcServiceData,
                         &xServiceUuid,
                         xNbServices,
                         usManufacturerLen,
                         pcManufacturerData );

    prvSetAdvertisement( &l_xAdvertisementConfigB,
                         usServiceDataLen,
                         pcServiceData,
                         NULL,
                         0,
                         usManufacturerLen,
                         pcManufacturerData );
}

void IotTestBleHal_CheckIndicationNotification( bool IsIndication,
                                                bool IsConnected )
{
    BTStatus_t xStatus;
    BLETESTindicateCallback_t xIndicateEvent;
    bletestAttSrvB_t xAttribute;

    if( IsIndication )
    {
        xAttribute = bletestATTR_SRVCB_CHAR_F;
    }
    else
    {
        xAttribute = bletestATTR_SRVCB_CHAR_E;
    }

    memcpy( ucRespBuffer[ xAttribute ].ucBuffer, bletestsDEFAULT_CHAR_VALUE, sizeof( bletestsDEFAULT_CHAR_VALUE ) - 1 );
    ucRespBuffer[ xAttribute ].xLength = sizeof( bletestsDEFAULT_CHAR_VALUE ) - 1;

    if( IsConnected == true )
    {
        if( IsIndication )
        {
            prvSendNotification( xAttribute, true );
        }
        else
        {
            prvSendNotification( xAttribute, false );
        }

        xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventIndicateCb, NO_HANDLE, ( void * ) &xIndicateEvent, sizeof( BLETESTindicateCallback_t ), BLE_TESTS_WAIT );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
        TEST_ASSERT_EQUAL( _usBLEConnId, xIndicateEvent.usConnId );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xIndicateEvent.xStatus );
    }
    else
    {
        xStatus = _pxGattServerInterface->pxSendIndication( _ucBLEServerIf,
                                                            usHandlesBufferB[ xAttribute ],
                                                            _usBLEConnId,
                                                            ucRespBuffer[ xAttribute ].xLength,
                                                            ucRespBuffer[ xAttribute ].ucBuffer,
                                                            true );
        TEST_ASSERT_NOT_EQUAL( eBTStatusSuccess, xStatus );
        xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventIndicateCb, NO_HANDLE, ( void * ) &xIndicateEvent, sizeof( BLETESTindicateCallback_t ), BLE_TESTS_WAIT );
        TEST_ASSERT_NOT_EQUAL( eBTStatusSuccess, xStatus );
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

void IotTestBleHal_SetGetProperty( BTProperty_t * pxProperty,
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

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventPropertyCb, NO_HANDLE, ( void * ) &xSetGetPropertyCb, sizeof( BLETESTSetGetPropertyCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xSetGetPropertyCb.xStatus );
    TEST_ASSERT_EQUAL( 1, xSetGetPropertyCb.ulNumProperties );
    TEST_ASSERT_EQUAL( xSetGetPropertyCb.xProperties.xType, pxProperty->xType );
    TEST_ASSERT_LESS_THAN( bletestsMAX_PROPERTY_SIZE, xSetGetPropertyCb.xProperties.xLen );

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

void IotTestBleHal_StartStopAdvCheck( bool start )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTAdvParamCallback_t xAdvParamCallback;

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventStartAdvCb, NO_HANDLE, ( void * ) &xAdvParamCallback, sizeof( BLETESTAdvParamCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( start, xAdvParamCallback.bStart );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xAdvParamCallback.xStatus );
}

void IotTestBleHal_SetAdvProperty( void )
{
    BTProperty_t pxProperty;
    uint16_t usMTUsize = IOT_BLE_PREFERRED_MTU_SIZE;

    pxProperty.xType = eBTpropertyBdname;
    pxProperty.xLen = strlen( bletestsDEVICE_NAME );
    pxProperty.pvVal = ( void * ) bletestsDEVICE_NAME;

    /* Clear event queue: Bluedroid trigger eBLEHALEventPropertyCb after pxEnable */
    IotTestBleHal_ClearEventQueue();

    /* Set the name */
    IotTestBleHal_SetGetProperty( &pxProperty, true );

    /* Get the name to check it is set */
/*@TODO IotTestBleHal_SetGetProperty(&pxProperty, false); */

    pxProperty.xType = eBTpropertyLocalMTUSize;
    pxProperty.xLen = sizeof( usMTUsize );
    pxProperty.pvVal = &usMTUsize;

    /* Set the MTU size */
    IotTestBleHal_SetGetProperty( &pxProperty, true );

    /* Get the MTU size to check it is set */
    /*@TODOIotTestBleHal_SetGetProperty(&pxProperty, false); */
}

void IotTestBleHal_DeleteService( BTService_t * xRefSrvc )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTServiceCallback_t xStopDeleteServiceCb;

    xStatus = _pxGattServerInterface->pxDeleteService( _ucBLEServerIf, xRefSrvc->pusHandlesBuffer[ 0 ] );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventServiceDeletedCb, xRefSrvc->pusHandlesBuffer[ 0 ], ( void * ) &xStopDeleteServiceCb, sizeof( BLETESTServiceCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStopDeleteServiceCb.xStatus );
}

void IotTestBleHal_StopService( BTService_t * xRefSrvc )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTServiceCallback_t xStopDeleteServiceCb;

    xStatus = _pxGattServerInterface->pxStopService( _ucBLEServerIf, xRefSrvc->pusHandlesBuffer[ 0 ] );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventServiceStoppedCb, xRefSrvc->pusHandlesBuffer[ 0 ], ( void * ) &xStopDeleteServiceCb, sizeof( BLETESTServiceCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStopDeleteServiceCb.xStatus );
}

void IotTestBleHal_BTUnregister( void )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLETESTInitDeinitCallback_t xInitDeinitCb;

    xStatus = _pxGattServerInterface->pxUnregisterServer( _ucBLEServerIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventRegisterUnregisterGattServerCb, NO_HANDLE, ( void * ) &xInitDeinitCb, sizeof( BLETESTInitDeinitCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xInitDeinitCb.xStatus );


    xStatus = _pxBTLeAdapterInterface->pxUnregisterBleApp( _ucBLEAdapterIf );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

void IotTestBleHal_CreateSecureConnection_Model1Level4( bool IsBondSucc )
{
    BTStatus_t xStatus;
    BLEHALEventsTypes_t xEventsReceived;
    BLETESTsspRequestCallback_t xSSPrequestEvent;
    BLETESTPairingStateChangedCallback_t xPairingStateChangedEvent;

    /* Wait secure connection. Secure connection is triggered by writing to bletestsCHARB. */

    /* SSP Pairing request callback is optional for a board to support. Some boards handle this directly in stack and will invoke only a passkey confirmation. */
    xEventsReceived = IotTestBleHal_WaitForEvents( ( eBLEHALEventSSPrequestCb | eBLEHALEventSSPrequestConfirmationCb ), BLE_TESTS_WAIT );
    TEST_ASSERT_NOT_EQUAL( eBLEHALEventNone, xEventsReceived );

    if( ( xEventsReceived & eBLEHALEventSSPrequestCb ) == eBLEHALEventSSPrequestCb )
    {
        xStatus = IotTestBleHal_GetEventFromQueueWithMatch( eBLEHALEventSSPrequestCb, NO_HANDLE, ( void * ) &xSSPrequestEvent, sizeof( BLETESTsspRequestCallback_t ), NULL );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

        if( IsBondSucc == true )
        {
            TEST_ASSERT_EQUAL( 0, memcmp( &xSSPrequestEvent.xRemoteBdAddr, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
            TEST_ASSERT_EQUAL( eBTsspVariantConsent, xSSPrequestEvent.xPairingVariant );
        }

        IotTestBleHal_ClearEventQueue();
        xStatus = _pxBTInterface->pxSspReply( &xSSPrequestEvent.xRemoteBdAddr, eBTsspVariantConsent, true, 0 );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    }

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventSSPrequestConfirmationCb, NO_HANDLE, ( void * ) &xSSPrequestEvent, sizeof( BLETESTsspRequestCallback_t ), BLE_TESTS_WAIT );

    if( IsBondSucc == true )
    {
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
        TEST_ASSERT_EQUAL( 0, memcmp( &xSSPrequestEvent.xRemoteBdAddr, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
        TEST_ASSERT_EQUAL( eBTsspVariantPasskeyConfirmation, xSSPrequestEvent.xPairingVariant );
    }

    if( xStatus == eBTStatusSuccess )
    {
        xStatus = _pxBTInterface->pxSspReply( &xSSPrequestEvent.xRemoteBdAddr, eBTsspVariantPasskeyConfirmation, true, 0 );
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
    }

    xStatus = IotTestBleHal_WaitEventFromQueue( eBLEHALEventPairingStateChangedCb, NO_HANDLE, ( void * ) &xPairingStateChangedEvent, sizeof( BLETESTPairingStateChangedCallback_t ), BLE_TESTS_WAIT );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );

    if( IsBondSucc == true )
    {
        TEST_ASSERT_EQUAL( eBTStatusSuccess, xPairingStateChangedEvent.xStatus );
        TEST_ASSERT_EQUAL( eBTbondStateBonded, xPairingStateChangedEvent.xBondState );
        TEST_ASSERT_EQUAL( 0, memcmp( &xPairingStateChangedEvent.xRemoteBdAddr, &_xAddressConnectedDevice, sizeof( BTBdaddr_t ) ) );
        TEST_ASSERT_EQUAL( eBTSecLevelSecureConnect, xPairingStateChangedEvent.xSecurityLevel );
    }
    else
    {
        TEST_ASSERT_NOT_EQUAL( eBTStatusSuccess, xPairingStateChangedEvent.xStatus );
        TEST_ASSERT_EQUAL( eBTbondStateNone, xPairingStateChangedEvent.xBondState );
    }
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

#if ENABLE_TC_INTEGRATION_ADD_CHARACTERISTIC_IN_CALLBACK
    void prvCharAddedNestedCb( BTStatus_t xStatus,
                               uint8_t ucServerIf,
                               BTUuid_t * pxUuid,
                               uint16_t usServiceHandle,
                               uint16_t usCharHandle )
    {
        BTGattSrvcId_t xSrvcId;
        uint16_t usNumHandles;

        if( ( bCharAddedComplete != true ) && ( ServiceB_Char < ServiceB_CharNumber ) )
        {
            prvCreateCharacteristic( &_xSrvcB, ServiceB_CharArray[ ServiceB_Char++ ], false );
        }
        else if( ServiceB_Char >= ServiceB_CharNumber )
        {
            TEST_ASSERT_EQUAL( true, bCharAddedComplete );
        }

        prvCharacteristicAddedCb( xStatus, ucServerIf, pxUuid, usServiceHandle, usCharHandle );
    }
#endif /* if ENABLE_TC_INTEGRATION_ADD_CHARACTERISTIC_IN_CALLBACK */

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
                    memcpy( pxSetGetPropertyCb->xProperties.pvVal, pxProperties->pvVal, pxProperties->xLen );
                }
                else
                {
                    memset( pxSetGetPropertyCb->xProperties.pvVal, 0, pxProperties->xLen );
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
                     uint8_t ucAdapterIf,
                     bool bStart )
{
    BLETESTAdvParamCallback_t * xdvParamCallback = IotTest_Malloc( sizeof( BLETESTAdvParamCallback_t ) );

    xdvParamCallback->xStatus = xStatus;
    xdvParamCallback->bStart = bStart;
    xdvParamCallback->xEvent.xEventTypes = eBLEHALEventStartAdvCb;
    xdvParamCallback->xEvent.lHandle = NO_HANDLE;

    if( xdvParamCallback->xStatus == eBTStatusSuccess )
    {
        if( ucAdapterIf != _ucBLEAdapterIf )
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
            memcpy( pxWriteAttrCallback->ucValue, pucValue, bletestsSTRINGYFIED_UUID_SIZE );
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

static BTStatus_t popEventFromQueueWithMatch( BLEHALEventsTypes_t xEventName,
                                              int32_t lhandle,
                                              void * pEventBuffer,
                                              size_t bufferLength,
                                              bool ( * pxMatch )( void * pvEvent ) )
{
    BLEHALEventsInternals_t * pEvent;
    IotLink_t * pEventLink;
    BTStatus_t xStatus = eBTStatusFail;

    IotBle_Assert( pEventBuffer != NULL );
    IotBle_Assert( bufferLength >= sizeof( BLEHALEventsInternals_t ) );

    IotMutex_Lock( &threadSafetyMutex );

    /* Get the event associated to the callback */
    IotContainers_ForEach( &eventQueueHead, pEventLink )
    {
        pEvent = IotLink_Container( BLEHALEventsInternals_t, pEventLink, eventList );

        if( ( pEvent->xEventTypes == xEventName ) &&
            ( pEvent->lHandle == lhandle ) )
        {
            if( ( pxMatch == NULL ) || ( pxMatch( pEvent ) == true ) )
            {
                IotListDouble_Remove( &pEvent->eventList );
                memcpy( pEventBuffer, pEvent, bufferLength );
                IotTest_Free( pEvent );
                xStatus = eBTStatusSuccess;
                break; /* If the right event is received, exit. */
            }
        }
    }

    IotMutex_Unlock( &threadSafetyMutex );

    return xStatus;
}

static BLEHALEventsTypes_t checkQueueForEvents( BLEHALEventsTypes_t xEventsToCheck )
{
    BLEHALEventsTypes_t eventSet = eBLEHALEventNone;
    BLEHALEventsInternals_t * pEvent;
    IotLink_t * pEventLink;

    IotMutex_Lock( &threadSafetyMutex );

    /* Get the event associated to the callback */
    IotContainers_ForEach( &eventQueueHead, pEventLink )
    {
        pEvent = IotLink_Container( BLEHALEventsInternals_t, pEventLink, eventList );

        if( ( xEventsToCheck & pEvent->xEventTypes ) == pEvent->xEventTypes )
        {
            eventSet |= pEvent->xEventTypes;
        }
    }

    IotMutex_Unlock( &threadSafetyMutex );

    return eventSet;
}

/* This function first check if an event is waiting in the list. If not, it will go and wait on the queue.
 * When an event is received on the queue, if it is not the expected event, it goes on the waiting list.
 */
BLEHALEventsTypes_t IotTestBleHal_WaitForEvents( BLEHALEventsTypes_t xEventsToWaitFor,
                                                 uint32_t timeoutMs )
{
    BLEHALEventsTypes_t eventsSet;

    IotLog( IOT_LOG_DEBUG,
            &_logHideAll,
            "WaitForEvents: %0x",
            xEventsToWaitFor );

    eventsSet = checkQueueForEvents( xEventsToWaitFor );

    /* There are no events currently, Wait for an event for timeout period. */
    if( ( eventsSet == eBLEHALEventNone ) && ( timeoutMs > 0 ) )
    {
        while( IotSemaphore_TimedWait( &eventSemaphore, timeoutMs ) == true )
        {
            eventsSet = checkQueueForEvents( xEventsToWaitFor );

            if( eventsSet != eBLEHALEventNone )
            {
                break; /* If the any of the event is received, exit. */
            }
        }
    }

    return eventsSet;
}

bool IotTestBleHal_CheckBondState( void * pvEvent )
{
    return ( ( BLETESTPairingStateChangedCallback_t * ) pvEvent )->xBondState != eBTbondStateBonding;
}


BTStatus_t IotTestBleHal_GetEventFromQueueWithMatch( BLEHALEventsTypes_t xEventName,
                                                     int32_t lhandle,
                                                     void * pxMessage,
                                                     size_t xMessageLength,
                                                     bool ( * pxMatch )( void * pvEvent ) )
{
    return popEventFromQueueWithMatch( xEventName, lhandle, pxMessage, xMessageLength, pxMatch );
}

BTStatus_t IotTestBleHal_WaitEventFromQueue( BLEHALEventsTypes_t xEventName,
                                             int32_t lhandle,
                                             void * pxMessage,
                                             size_t xMessageLength,
                                             uint32_t timeoutMs )
{
    return IotTestBleHal_WaitEventFromQueueWithMatch( xEventName, lhandle, pxMessage, xMessageLength, timeoutMs, NULL );
}

/* This function first check if an event is waiting in the list. If not, it will go and wait on the queue.
 * When an event is received on the queue, if it is not the expected event, it goes on the waiting list.
 */
BTStatus_t IotTestBleHal_WaitEventFromQueueWithMatch( BLEHALEventsTypes_t xEventName,
                                                      int32_t lhandle,
                                                      void * pxMessage,
                                                      size_t xMessageLength,
                                                      uint32_t timeoutMs,
                                                      bool ( * pxMatch )( void * pvEvent ) )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    IotLog( IOT_LOG_DEBUG,
            &_logHideAll,
            "WaitEvent: %d, handle=0x%x",
            xEventName,
            ( uint32_t ) lhandle );

    xStatus = popEventFromQueueWithMatch( xEventName, lhandle, pxMessage, xMessageLength, pxMatch );

    /* If event is not waiting then wait for it. */
    if( ( xStatus != eBTStatusSuccess ) && ( timeoutMs > 0 ) )
    {
        do
        {
            /* TODO check event list here */
            if( IotSemaphore_TimedWait( &eventSemaphore, timeoutMs ) == true )
            {
                xStatus = popEventFromQueueWithMatch( xEventName, lhandle, pxMessage, xMessageLength, pxMatch );
            }
            else
            {
                /* We did not receive any events after timeout time. Break and return failure.*/
                break;
            }
        } while( xStatus != eBTStatusSuccess ); /* If right event is not received continue waiting. */
    }

    if( xStatus == eBTStatusSuccess )
    {
        IotLog( IOT_LOG_DEBUG,
                &_logHideAll,
                "Event Received: %d, handle=0x%x",
                xEventName,
                ( uint32_t ) lhandle );
    }
    else
    {
        IotLog( IOT_LOG_DEBUG,
                &_logHideAll,
                "WaitEvent TIMEOUT!!! %d, handle=0x%x",
                xEventName,
                ( uint32_t ) lhandle );
    }

    return xStatus;
}

void IotTestBleHal_ClearEventQueue( void )
{
    IotMutex_Lock( &threadSafetyMutex );

    IotListDouble_RemoveAll( &eventQueueHead, IotTest_Free, offsetof( BLEHALEventsInternals_t, eventList ) );

    IotMutex_Unlock( &threadSafetyMutex );
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


void prvMtuChangedCb( uint16_t usConnId,
                      uint16_t usMtu )
{
    BLETESTMtuChangedCallback_t * pxMtuChangedCallback = IotTest_Malloc( sizeof( BLETESTMtuChangedCallback_t ) );

    if( pxMtuChangedCallback != NULL )
    {
        pxMtuChangedCallback->xEvent.xEventTypes = eBLEHALEventMtuChangedCb;
        pxMtuChangedCallback->xEvent.lHandle = NO_HANDLE;
        pxMtuChangedCallback->usConnId = usConnId;
        pxMtuChangedCallback->usMtu = usMtu;

        _bletestsMTU_SIZE = usMtu;

        pushToQueue( &pxMtuChangedCallback->xEvent.eventList );
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
        pxPairingStateChangedCallback->xBondState = xState;

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

void pushToQueue( IotLink_t * pEventList )
{
    BLEHALEventsInternals_t * pEventIndex = IotLink_Container( BLEHALEventsInternals_t, pEventList, eventList );

    IotLog( IOT_LOG_DEBUG,
            &_logHideAll,
            "pushToQueue: event=%d, handl=0x%x",
            pEventIndex->xEventTypes,
            ( uint32_t ) pEventIndex->lHandle );

    IotMutex_Lock( &threadSafetyMutex );

    IotListDouble_InsertHead( &eventQueueHead,
                              pEventList );
    IotSemaphore_Post( &eventSemaphore );

    IotMutex_Unlock( &threadSafetyMutex );
}
