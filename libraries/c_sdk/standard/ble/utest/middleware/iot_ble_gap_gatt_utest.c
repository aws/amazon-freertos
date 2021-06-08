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
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "iot_linear_containers.h"

#include "mock_bt_hal_gatt_server.h"
#include "mock_bt_hal_manager_adapter_ble.h"
#include "mock_bt_hal_manager.h"
#include "mock_iot_ble_interface.h"
#include "mock_iot_threads.h"
#include "mock_portable.h"
#include "mock_iot_logging.h"
#include "iot_ble.h"


#define CUSTOM_BLE_DEVICE_NAME              "abcxyz"
#define CONNECTION_INTERVAL_VALID_RANGE     { 0x0006UL, 0x0C80UL }

#define MAX_SEMAPHORES_USED                 ( 1 )

#define MAX_MUTEXES_USED                    ( 4 )

#define DUMMY_MTU_SIZE                      ( 512 )

#define DUMMY_CONNECTION_ID                 ( 1 )

#define SERVICE_HANDLE                      ( 100 )

#define NUM_SERVICE_ATTRIBUTES              ( 4 )

#define MAX_ATTRIBUTE_HANDLE                ( SERVICE_HANDLE + NUM_SERVICE_ATTRIBUTES )

#define SECONDARY_SERVICE_HANDLE            ( 200 )

#define NUM_SECONDARY_SERVICE_ATTRIBUTES    ( 1 )


typedef struct BleConfiguration
{
    char deviceName[ IOT_BLE_DEVICE_LOCAL_NAME_MAX_LENGTH + 1 ];
    bool needSecureConnection;
    bool shouldBond;
    BTIOtypes_t ioCapability;
    uint16_t mtu;
} BleConfiguration_t;


typedef struct SynchronizationObj
{
    void * pObj;
    uint32_t value;
} SynchronizationObj_t;

static void prvDummyAtttibuteCallback( IotBleAttributeEvent_t * pEventParam );

static BTInterface_t bleInterfaceMock =
{
    .pxBtManagerInit                = prvBleTestBtManagerInit,
    .pxBtManagerCleanup             = prvBleTestBtManagerCleanup,
    .pxEnable                       = prvBleTestEnable,
    .pxDisable                      = prvBleTestDisable,
    .pxGetAllDeviceProperties       = prvBleTestGetAllDeviceProperties,
    .pxGetDeviceProperty            = prvBleTestGetDeviceProperty,
    .pxSetDeviceProperty            = prvBleTestSetDeviceProperty,
    .pxGetAllRemoteDeviceProperties = prvBleTestGetAllRemoteDeviceProperties,
    .pxGetRemoteDeviceProperty      = prvBleTestGetRemoteDeviceProperty,
    .pxSetRemoteDeviceProperty      = prvBleTestSetRemoteDeviceProperty,
    .pxPair                         = prvBleTestPair,
    .pxCreateBondOutOfBand          = prvBleTestCreateBondOutOfBand,
    .pxCancelBond                   = prvBleTestCancelBond,
    .pxRemoveBond                   = prvBleTestRemoveBond,
    .pxGetConnectionState           = prvBleTestGetConnectionState,
    .pxPinReply                     = prvBleTestPinReply,
    .pxSspReply                     = prvBleTestSspReply,
    .pxReadEnergyInfo               = prvBleTestReadEnergyInfo,
    .pxDutModeConfigure             = prvBleTestDutModeConfigure,
    .pxDutModeSend                  = prvBleTestDutModeSend,
    .pxLeTestMode                   = prvBleTestLeTestMode,
    .pxConfigHCISnoopLog            = prvBleTestConfigHCISnoopLog,
    .pxConfigClear                  = prvBleTestConfigClear,
    .pxReadRssi                     = prvBleTestReadRssi,
    .pxGetTxpower                   = prvBleTestGetTxpower,
    .pxGetClassicAdapter            = prvBleTestGetClassicAdapter,
    .pxGetLeAdapter                 = prvBleTestGetLeAdapter,
};

static BTBleAdapter_t bleAdapterMock =
{
    .pxBleAdapterInit                  = prvBleTestBleAdapterInit,
    .pxRegisterBleApp                  = prvBleTestRegisterBleApp,
    .pxUnregisterBleApp                = prvBleTestUnregisterBleApp,
    .pxGetBleAdapterProperty           = prvBleTestGetBleAdapterProperty,
    .pxSetBleAdapterProperty           = prvBleTestSetBleAdapterProperty,
    .pxGetallBleRemoteDeviceProperties = prvBleTestGetallBleRemoteDeviceProperties,
    .pxGetBleRemoteDeviceProperty      = prvBleTestGetBleRemoteDeviceProperty,
    .pxSetBleRemoteDeviceProperty      = prvBleTestSetBleRemoteDeviceProperty,
    .pxScan                            = prvBleTestScan,
    .pxConnect                         = prvBleTestConnect,
    .pxDisconnect                      = prvBleTestDisconnect,
    .pxStartAdv                        = prvBleTestStartAdv,
    .pxStopAdv                         = prvBleTestStopAdv,
    .pxReadRemoteRssi                  = prvBleTestReadRemoteRssi,
    .pxScanFilterParamSetup            = prvBleTestScanFilterParamSetup,
    .pxScanFilterAddRemove             = prvBleTestScanFilterAddRemove,
    .pxScanFilterEnable                = prvBleTestScanFilterEnable,
    .pxGetDeviceType                   = prvBleTestGetDeviceType,
    .pxSetAdvData                      = prvBleTestSetAdvData,
    .pxSetAdvRawData                   = prvBleTestSetAdvRawData,
    .pxConnParameterUpdateRequest      = prvBleTestConnParameterUpdateRequest,
    .pxSetScanParameters               = prvBleTestSetScanParameters,
    .pxMultiAdvEnable                  = prvBleTestMultiAdvEnable,
    .pxMultiAdvUpdate                  = prvBleTestMultiAdvUpdate,
    .pxMultiAdvSetInstData             = prvBleTestMultiAdvSetInstData,
    .pxMultiAdvDisable                 = prvBleTestMultiAdvDisable,
    .pxBatchscanCfgStorage             = prvBleTestBatchscanCfgStorage,
    .pxBatchscanEndBatchScan           = prvBleTestBatchscanEndBatchScan,
    .pxBatchscanDisBatchScan           = prvBleTestBatchscanDisBatchScan,
    .pxBatchscanReadReports            = prvBleTestBatchscanReadReports,
    .pxSetPreferredPhy                 = prvBleTestSetPreferredPhy,
    .pxReadPhy                         = prvBleTestReadPhy,
    .ppvGetGattClientInterface         = prvBleTestGetGattClientInterface,
    .ppvGetGattServerInterface         = prvBleTestGetGattServerInterface
};

static BTGattServerInterface_t bleGattServerMock =
{
    .pxRegisterServer     = prvBleTestRegisterServer,
    .pxUnregisterServer   = prvBleTestUnregisterServer,
    .pxGattServerInit     = prvBleTestGattServerInit,
    .pxConnect            = prvBleTestConnect,
    .pxDisconnect         = prvBleTestDisconnect,
    .pxAddServiceBlob     = prvBleTestAddServiceBlob,
    .pxAddService         = prvBleTestAddService,
    .pxAddIncludedService = prvBleTestAddIncludedService,
    .pxAddCharacteristic  = prvBleTestAddCharacteristic,
    .pxSetVal             = prvBleTestSetVal,
    .pxAddDescriptor      = prvBleTestAddDescriptor,
    .pxStartService       = prvBleTestStartService,
    .pxStopService        = prvBleTestStopService,
    .pxDeleteService      = prvBleTestDeleteService,
    .pxSendIndication     = prvBleTestSendIndication,
    .pxSendResponse       = prvBleTestSendResponse,
    .pxConfigureMtu       = prvBleTestConfigureMtu
};

static BTService_t includedService;

static BTAttribute_t inclAttributeTable[ NUM_SECONDARY_SERVICE_ATTRIBUTES ] =
{
    {
        .xAttributeType = eBTDbSecondaryService,
        .xServiceUUID.ucType = eBTuuidType128,
        .xServiceUUID.uu.uu128 ={ 0xCC                              },
    }
};

static BTAttribute_t attributeTable[ NUM_SERVICE_ATTRIBUTES ] =
{
    {
        .xAttributeType = eBTDbPrimaryService,
        .xServiceUUID.ucType = eBTuuidType128,
        .xServiceUUID.uu.uu128 ={ 0xAA                              },
    },
    {
        .xAttributeType = eBTDbCharacteristic,
        .xCharacteristic =
        {
            .xUuid.ucType   = eBTuuidType128,
            .xUuid.uu.uu128 ={ 0xBB                              },
            .xPermissions   = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM ),
            .xProperties    = ( eBTPropRead | eBTPropWrite )
        }
    },
    {
        .xAttributeType = eBTDbDescriptor,
        .xCharacteristicDescr =
        {
            .xUuid.ucType  = eBTuuidType16,
            .xUuid.uu.uu16 = 0xCCCC,
            .xPermissions  = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM )
        }
    },
    {
        .xAttributeType = eBTDbIncludedService,
        .xIncludedService =
        {
            .xUuid.ucType   = eBTuuidType128,
            .xUuid.uu.uu128 ={ 0xCC                              },
            .pxPtrToService = &includedService
        }
    }
};
static IotBleAttributeEventCallback_t attributeCallbacks[ 4 ] =
{
    NULL,
    prvDummyAtttibuteCallback,
    prvDummyAtttibuteCallback,
    NULL
};


static uint32_t eventCallbackCount[ eNbEvents ] = { 0 };
static uint32_t attributeInvokedCount[ NUM_SERVICE_ATTRIBUTES ][ 5 ] = { 0 };
static uint32_t alloc_mem_blocks;
static uint32_t numAdvertisementStatusCalls;
static SynchronizationObj_t semaphores[ MAX_SEMAPHORES_USED ];
static SynchronizationObj_t mutexes[ MAX_MUTEXES_USED ];
static int mutexFailAfterCount = MAX_MUTEXES_USED;


static BTCallbacks_t middlewareBTCallback;
static BTBleAdapterCallbacks_t middlewareBleCallback;
static BTGattServerCallbacks_t middlewareGATTCallback;
static BleConfiguration_t bleConfig;

static void prvDummyAtttibuteCallback( IotBleAttributeEvent_t * pEventParam )
{
    uint16_t handle;

    switch( pEventParam->xEventType )
    {
        case eBLEWrite:
            TEST_ASSERT_GREATER_OR_EQUAL( SERVICE_HANDLE, pEventParam->pParamWrite->attrHandle );
            TEST_ASSERT_LESS_THAN( MAX_ATTRIBUTE_HANDLE, pEventParam->pParamWrite->attrHandle );
            handle = pEventParam->pParamWrite->attrHandle - SERVICE_HANDLE;
            attributeInvokedCount[ handle ][ 0 ]++;
            break;

        case eBLEWriteNoResponse:
            TEST_ASSERT_GREATER_OR_EQUAL( SERVICE_HANDLE, pEventParam->pParamWrite->attrHandle );
            TEST_ASSERT_LESS_THAN( MAX_ATTRIBUTE_HANDLE, pEventParam->pParamWrite->attrHandle );
            handle = pEventParam->pParamWrite->attrHandle - SERVICE_HANDLE;
            attributeInvokedCount[ handle ][ 1 ]++;
            break;

        case eBLERead:
            TEST_ASSERT_GREATER_OR_EQUAL( SERVICE_HANDLE, pEventParam->pParamRead->attrHandle );
            TEST_ASSERT_LESS_THAN( MAX_ATTRIBUTE_HANDLE, pEventParam->pParamRead->attrHandle );
            handle = pEventParam->pParamRead->attrHandle - SERVICE_HANDLE;
            attributeInvokedCount[ handle ][ 2 ]++;
            break;

        case eBLEExecWrite:
            TEST_ASSERT_GREATER_OR_EQUAL( SERVICE_HANDLE, pEventParam->pParamExecWrite->transId );
            TEST_ASSERT_LESS_THAN( MAX_ATTRIBUTE_HANDLE, pEventParam->pParamExecWrite->transId );
            TEST_ASSERT_TRUE( pEventParam->pParamExecWrite->execWrite );
            handle = pEventParam->pParamExecWrite->transId - SERVICE_HANDLE;
            attributeInvokedCount[ handle ][ 3 ]++;
            break;

        case eBLEResponseConfirmation:
            TEST_ASSERT_GREATER_OR_EQUAL( SERVICE_HANDLE, pEventParam->pParamRespConfirm->handle );
            TEST_ASSERT_LESS_THAN( MAX_ATTRIBUTE_HANDLE, pEventParam->pParamRespConfirm->handle );
            handle = pEventParam->pParamRespConfirm->handle - SERVICE_HANDLE;
            attributeInvokedCount[ handle ][ 4 ]++;
            break;

        case eBLEIndicationConfirmReceived:
            break;

        default:
            TEST_FAIL_MESSAGE( "Unhandled event type received." );
            break;
    }

    attributeInvokedCount[ 0 ][ 0 ]++;
}

static void * pvPortMalloc_Callback( size_t xSize,
                                     int n_calls )
{
    alloc_mem_blocks++; /* Free + malloc calls should cancel out in the end */

    void * pNew = malloc( xSize );

    TEST_ASSERT_MESSAGE( pNew, "Test Stub for malloc failed!" );

    return pNew;
}

static void vPortFree_Callback( void * pMem,
                                int n_calls )
{
    alloc_mem_blocks--;
    free( pMem );
}


static BTStatus_t prvSaveConfiguration( const BTProperty_t * pxProperty,
                                        int n_calls )
{
    switch( pxProperty->xType )
    {
        case eBTpropertyBdname:
            TEST_ASSERT( pxProperty->xLen < sizeof( bleConfig.deviceName ) );
            memcpy( bleConfig.deviceName, pxProperty->pvVal, pxProperty->xLen );
            break;

        case eBTpropertySecureConnectionOnly:
            bleConfig.needSecureConnection = *( ( bool * ) ( pxProperty->pvVal ) );
            break;

        case eBTpropertyBondable:
            bleConfig.shouldBond = *( ( bool * ) ( pxProperty->pvVal ) );
            break;

        case eBTpropertyIO:
            bleConfig.ioCapability = *( ( BTIOtypes_t * ) ( pxProperty->pvVal ) );
            break;

        case eBTpropertyLocalMTUSize:
            bleConfig.mtu = *( ( uint16_t * ) ( pxProperty->pvVal ) );
            break;

        default:
            TEST_FAIL_MESSAGE( "Unhandled property config received." );
            break;
    }

    middlewareBTCallback.pxAdapterPropertiesCb( eBTStatusSuccess, 1, ( BTProperty_t * ) pxProperty );

    return eBTStatusSuccess;
}


static BTStatus_t prvRegisterBTCallback( const BTCallbacks_t * pxCallbacks,
                                         int cmock_num_calls )
{
    if( cmock_num_calls == 0 )
    {
        middlewareBTCallback = *pxCallbacks;
    }

    return eBTStatusSuccess;
}

static BTStatus_t prvEnableBTCallback( uint8_t ucGuestMode,
                                       int cmock_num_calls )
{
    middlewareBTCallback.pxDeviceStateChangedCb( eBTstateOn );
    return eBTStatusSuccess;
}

static BTStatus_t prvRegisterBleCallback( const BTBleAdapterCallbacks_t * pxCallbacks,
                                          int cmock_num_calls )
{
    if( cmock_num_calls == 0 )
    {
        middlewareBleCallback = *pxCallbacks;
    }

    return eBTStatusSuccess;
}

static BTStatus_t prvRegisterAppCallback( BTUuid_t * pxAppUuid,
                                          int cmock_num_calls )
{
    middlewareBleCallback.pxRegisterBleAdapterCb( eBTStatusSuccess, 0, pxAppUuid );
    return eBTStatusSuccess;
}


static BTStatus_t prvRegisterGattCallback( const BTGattServerCallbacks_t * prvBleTestCallbacks,
                                           int cmock_num_calls )
{
    if( cmock_num_calls == 0 )
    {
        middlewareGATTCallback = *prvBleTestCallbacks;
    }

    return eBTStatusSuccess;
}

static BTStatus_t prvRegisterServerCallback( BTUuid_t * prvBleTestUuid,
                                             int cmock_num_calls )
{
    middlewareGATTCallback.pxRegisterServerCb( eBTStatusSuccess, 0, prvBleTestUuid );
    return eBTStatusSuccess;
}

static BTStatus_t prvUnRegisterServerCallback( uint8_t ucServerIf,
                                               int cmock_num_calls )
{
    middlewareGATTCallback.pxUnregisterServerCb( eBTStatusSuccess, 0 );
    return eBTStatusSuccess;
}

static BTStatus_t prvDisableBTCallback( int cmock_num_calls )
{
    middlewareBTCallback.pxDeviceStateChangedCb( eBTstateOff );
    return eBTStatusSuccess;
}

BTStatus_t prvValidateAdvertisingData( uint8_t ucAdapterIf,
                                       BTGattAdvertismentParams_t * pxParams,
                                       uint16_t usManufacturerLen,
                                       char * pcManufacturerData,
                                       uint16_t usServiceDataLen,
                                       char * pcServiceData,
                                       BTUuid_t * pxServiceUuid,
                                       size_t xNbServices,
                                       int cmock_num_calls )
{
    uint32_t connIntervalRange[ 2 ] = CONNECTION_INTERVAL_VALID_RANGE;

    TEST_ASSERT_EQUAL( true, pxParams->bIncludeTxPower );

    /* Advertise connection interval only in scan response. */
    if( pxParams->bSetScanRsp )
    {
        TEST_ASSERT_TRUE( ( pxParams->ulMinInterval >= connIntervalRange[ 0 ] ) &&
                          ( pxParams->ulMinInterval <= connIntervalRange[ 1 ] ) );
        TEST_ASSERT_TRUE( ( pxParams->ulMaxInterval >= connIntervalRange[ 0 ] ) &&
                          ( pxParams->ulMaxInterval <= connIntervalRange[ 1 ] ) );
        TEST_ASSERT_TRUE( pxParams->ulMinInterval <= pxParams->ulMaxInterval );
    }
    else
    {
        TEST_ASSERT_EQUAL( 0, pxParams->ulMinInterval );
        TEST_ASSERT_EQUAL( 0, pxParams->ulMaxInterval );
    }

    /* Do not advertise name in scan response to save bytes. */
    if( pxParams->bSetScanRsp )
    {
        TEST_ASSERT_EQUAL( BTGattAdvNameNone, pxParams->ucName.xType );
        TEST_ASSERT_EQUAL( 0, pxParams->ucName.ucShortNameLen );
    }
    else
    {
        /* Advertise only short name to save bytes. */
        TEST_ASSERT_EQUAL( BTGattAdvNameShort, pxParams->ucName.xType );
        TEST_ASSERT_EQUAL( 4, pxParams->ucName.ucShortNameLen );
    }

    TEST_ASSERT_EQUAL( 0, pxParams->ucChannelMap );
    TEST_ASSERT_EQUAL( BTAdvInd, pxParams->usAdvertisingEventProperties );
    TEST_ASSERT_EQUAL( 0, pxParams->usTimeout );
    TEST_ASSERT_EQUAL( BTAddrTypePublic, pxParams->xAddrType );

    middlewareBleCallback.pxSetAdvDataCb( eBTStatusSuccess );

    return eBTStatusSuccess;
}

static BTStatus_t prvStartAdvCallback( uint8_t ucAdapterIf,
                                       int cmock_num_calls )
{
    middlewareBleCallback.pxAdvStatusCb( eBTStatusSuccess, 0, true );
    return eBTStatusSuccess;
}

static BTStatus_t prvStopAdvCallback( uint8_t ucAdapterIf,
                                      int cmock_num_calls )
{
    middlewareBleCallback.pxAdvStatusCb( eBTStatusSuccess, 0, false );
    return eBTStatusSuccess;
}

static BTStatus_t prvHALDisconnectStub( uint8_t ucAdapterIf,
                                        const BTBdaddr_t * pxBdAddr,
                                        uint16_t usConnId,
                                        int cmock_num_calls )
{
    middlewareGATTCallback.pxConnectionCb( usConnId, 0, false, ( BTBdaddr_t * ) pxBdAddr );
    return eBTStatusSuccess;
}

static bool prvSemaphoreCreateStub( IotSemaphore_t * pNewSemaphore,
                                    uint32_t initialValue,
                                    uint32_t maxValue,
                                    int cmock_num_calls )
{
    TEST_ASSERT_LESS_THAN( MAX_SEMAPHORES_USED, cmock_num_calls );
    semaphores[ cmock_num_calls ].pObj = ( void * ) pNewSemaphore;
    semaphores[ cmock_num_calls ].value = 0;
    return true;
}

static void prvSemaphorePostStub( IotSemaphore_t * pSemaphore,
                                  int cmock_num_calls )
{
    int index;
    bool found = false;

    for( index = 0; index < MAX_SEMAPHORES_USED; index++ )
    {
        if( semaphores[ index ].pObj == pSemaphore )
        {
            semaphores[ index ].value = 1;
            found = true;
        }
    }

    TEST_ASSERT_TRUE( found );
}

static void prvSemaphoreWaitStub( IotSemaphore_t * pSemaphore,
                                  int cmock_num_calls )
{
    int index;
    bool found = false;

    for( index = 0; index < MAX_SEMAPHORES_USED; index++ )
    {
        if( semaphores[ index ].pObj == pSemaphore )
        {
            /* Make sure the semaphore is posted. */
            TEST_ASSERT_TRUE( semaphores[ index ].value == 1 )
            semaphores[ index ].value = 0;
            found = true;
        }
    }

    TEST_ASSERT_TRUE( found );
}

static void makeMutexCreateFailAfterCount( int count )
{
    mutexFailAfterCount = count;
}

static bool prvMutexCreateStub( IotMutex_t * pNewMutex,
                                bool recursive,
                                int cmock_num_calls )
{
    TEST_ASSERT_LESS_THAN( MAX_MUTEXES_USED, cmock_num_calls );
    TEST_ASSERT_FALSE( recursive );
    bool result = false;

    if( cmock_num_calls < mutexFailAfterCount )
    {
        mutexes[ cmock_num_calls ].pObj = ( void * ) pNewMutex;
        mutexes[ cmock_num_calls ].value = 0;
        result = true;
    }

    return result;
}

static void prvMutexLockStub( IotMutex_t * pMutex,
                              int cmock_num_calls )
{
    int index;
    bool found = false;

    for( index = 0; index < MAX_MUTEXES_USED; index++ )
    {
        if( mutexes[ index ].pObj == pMutex )
        {
            /* Verify there are no recursive locks. */
            TEST_ASSERT_EQUAL( 0, mutexes[ index ].value );
            mutexes[ index ].value = 1;
            found = true;
        }
    }

    TEST_ASSERT_TRUE( found );
}

static void prvMutexUnlockStub( IotMutex_t * pMutex,
                                int cmock_num_calls )
{
    int index;
    bool found = false;

    for( index = 0; index < MAX_MUTEXES_USED; index++ )
    {
        if( mutexes[ index ].pObj == pMutex )
        {
            TEST_ASSERT_EQUAL( 1, mutexes[ index ].value );
            mutexes[ index ].value = 0;
            found = true;
        }
    }

    TEST_ASSERT_TRUE( found );
}


static void prvTestInitBLE( void )
{
    /* Initialize BLE middleware before each test. */
    BTGetBluetoothInterface_IgnoreAndReturn( &bleInterfaceMock );
    prvBleTestGetLeAdapter_IgnoreAndReturn( &bleAdapterMock );
    prvBleTestGetGattServerInterface_IgnoreAndReturn( &bleGattServerMock );
    prvBleTestBtManagerInit_Stub( prvRegisterBTCallback );
    prvBleTestEnable_Stub( prvEnableBTCallback );
    prvBleTestBleAdapterInit_Stub( prvRegisterBleCallback );
    prvBleTestRegisterBleApp_Stub( prvRegisterAppCallback );
    prvBleTestSetDeviceProperty_Stub( prvSaveConfiguration );
    prvBleTestGattServerInit_Stub( prvRegisterGattCallback );
    prvBleTestRegisterServer_Stub( prvRegisterServerCallback );
    prvBleTestSetAdvData_Stub( prvValidateAdvertisingData );
    prvBleTestStartAdv_Stub( prvStartAdvCallback );
    prvBleTestStopAdv_Stub( prvStopAdvCallback );
    prvBleTestDisconnect_Stub( prvHALDisconnectStub );
    prvBleTestUnregisterServer_Stub( prvUnRegisterServerCallback );
    prvBleTestUnregisterBleApp_IgnoreAndReturn( eBTStatusSuccess );
    prvBleTestDisable_Stub( prvDisableBTCallback );
    prvBleTestBtManagerCleanup_IgnoreAndReturn( eBTStatusSuccess );


    TEST_ASSERT_EQUAL_MESSAGE( eBTStatusSuccess, IotBle_Init(), "Failed to initialize BLE Middleware" );
}


static void prvTestTurnOnBLE( void )
{
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_On() );
}

void prvTestTurnOffBLE( void )
{
    TEST_ASSERT_EQUAL_MESSAGE( eBTStatusSuccess, IotBle_Off(), "Failed to turn off BLE" );
}

static void prvAdvertisementCallback( BTStatus_t status )
{
    numAdvertisementStatusCalls++;
}


/* Called at the beginning of each test. */
void setUp( void )
{
    numAdvertisementStatusCalls = 0;
    mutexFailAfterCount = MAX_MUTEXES_USED;

    /* Initialize synchronization objects and stubs. */
    memset( mutexes, 0x00, sizeof( mutexes ) );
    memset( semaphores, 0x00, sizeof( semaphores ) );
    memset( &bleConfig, 0x00, sizeof( bleConfig ) );
    memset( eventCallbackCount, 0x00, sizeof( eventCallbackCount ) );
    memset( attributeInvokedCount, 0x00, sizeof( attributeInvokedCount ) );

    IotMutex_Create_Stub( prvMutexCreateStub );
    IotMutex_Lock_Stub( prvMutexLockStub );
    IotMutex_Unlock_Stub( prvMutexUnlockStub );
    IotSemaphore_Create_Stub( prvSemaphoreCreateStub );
    IotSemaphore_Post_Stub( prvSemaphorePostStub );
    IotSemaphore_Wait_Stub( prvSemaphoreWaitStub );

    alloc_mem_blocks = 0;
    pvPortMalloc_Stub( pvPortMalloc_Callback );
    vPortFree_Stub( vPortFree_Callback );

    IotLog_Generic_Ignore();

    prvTestInitBLE();
}

/* Called at the end of each test. */
void tearDown( void )
{
    int index = 0;

    /* Verify all mutexes are unlocked. */
    for( index = 0; index < MAX_MUTEXES_USED; index++ )
    {
        TEST_ASSERT_EQUAL( 0, mutexes[ index ].value );
    }

    /* Verify there are no memory blocks which are not freed.*/
    TEST_ASSERT_EQUAL_MESSAGE( 0, alloc_mem_blocks, "Memory leak detected" );
}

/* called at the beginning of the whole suite */
void suiteSetUp()
{
}

/* called at the end of the whole suite */
int suiteTearDown( int numFailures )
{
    return 0;
}

/**
 * @brief Tests BLE middleware initialization failure cases.
 */
void test_IotBle_Init_NullInterface( void )
{
    IotMutex_Create_IgnoreAndReturn( true );
    IotSemaphore_Create_IgnoreAndReturn( true );
    BTGetBluetoothInterface_IgnoreAndReturn( NULL );
    prvBleTestGetLeAdapter_IgnoreAndReturn( &bleAdapterMock );
    prvBleTestGetGattServerInterface_IgnoreAndReturn( &bleGattServerMock );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_Init() );
}

/**
 * @brief Tests BLE middleware initialization failure cases
 */
void test_IotBle_Init_NullAdapter( void )
{
    IotMutex_Create_IgnoreAndReturn( true );
    IotSemaphore_Create_IgnoreAndReturn( true );
    BTGetBluetoothInterface_IgnoreAndReturn( &bleInterfaceMock );
    prvBleTestGetLeAdapter_IgnoreAndReturn( NULL );
    prvBleTestGetGattServerInterface_IgnoreAndReturn( &bleGattServerMock );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_Init() );
}

/**
 * @brief Tests BLE middleware initialization failure cases
 */
void test_IotBle_Init_NullGattInterface( void )
{
    IotMutex_Create_IgnoreAndReturn( true );
    IotSemaphore_Create_IgnoreAndReturn( true );
    BTGetBluetoothInterface_IgnoreAndReturn( &bleInterfaceMock );
    prvBleTestGetLeAdapter_IgnoreAndReturn( &bleAdapterMock );
    prvBleTestGetGattServerInterface_IgnoreAndReturn( NULL );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_Init() );
}

/**
 * @brief Tests BLE middleware initialization failure cases
 */
void test_IotBle_Init_Mutex_Create_Failed_1( void )
{
    BTGetBluetoothInterface_IgnoreAndReturn( &bleInterfaceMock );
    prvBleTestGetLeAdapter_IgnoreAndReturn( &bleAdapterMock );
    prvBleTestGetGattServerInterface_IgnoreAndReturn( &bleGattServerMock );
    IotSemaphore_Create_IgnoreAndReturn( true );


    makeMutexCreateFailAfterCount( 0 );
    TEST_ASSERT_EQUAL( eBTStatusNoMem, IotBle_Init() );
}

void test_IotBle_Init_Mutex_Create_Failed_2( void )
{
    BTGetBluetoothInterface_IgnoreAndReturn( &bleInterfaceMock );
    prvBleTestGetLeAdapter_IgnoreAndReturn( &bleAdapterMock );
    prvBleTestGetGattServerInterface_IgnoreAndReturn( &bleGattServerMock );
    IotSemaphore_Create_IgnoreAndReturn( true );

    makeMutexCreateFailAfterCount( 1 );
    IotMutex_Destroy_Ignore();
    TEST_ASSERT_EQUAL( eBTStatusNoMem, IotBle_Init() );
}

void test_IotBle_Init_Mutex_Create_Failed_3( void )
{
    BTGetBluetoothInterface_IgnoreAndReturn( &bleInterfaceMock );
    prvBleTestGetLeAdapter_IgnoreAndReturn( &bleAdapterMock );
    prvBleTestGetGattServerInterface_IgnoreAndReturn( &bleGattServerMock );
    IotSemaphore_Create_IgnoreAndReturn( true );

    makeMutexCreateFailAfterCount( 2 );
    IotMutex_Destroy_Ignore();
    TEST_ASSERT_EQUAL( eBTStatusNoMem, IotBle_Init() );
}

/**
 * @brief Tests BLE middleware initialization failure cases
 */
void test_IotBle_Init_Semaphore_Create_Failed( void )
{
    BTGetBluetoothInterface_IgnoreAndReturn( &bleInterfaceMock );
    prvBleTestGetLeAdapter_IgnoreAndReturn( &bleAdapterMock );
    prvBleTestGetGattServerInterface_IgnoreAndReturn( &bleGattServerMock );


    IotMutex_Create_IgnoreAndReturn( true );
    IotSemaphore_Create_IgnoreAndReturn( false );
    IotMutex_Destroy_Ignore();
    TEST_ASSERT_EQUAL( eBTStatusNoMem, IotBle_Init() );
}

void test_IoTBle_On_ManagerInitFailed()
{
    prvBleTestBtManagerInit_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_On() );
}

void test_IoTBle_On_EnableFailed()
{
    prvBleTestEnable_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_On() );
}

void test_IotBle_On_AdapterInit_Failed()
{
    prvBleTestBleAdapterInit_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_On() );
}

void test_IotBle_On_RegisterApp_Failed()
{
    prvBleTestRegisterBleApp_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_On() );
}

void test_IotBle_On_DeviceProp_Failed()
{
    prvBleTestSetDeviceProperty_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_On() );
}

void test_IotBle_On_GattServer_Init_Failed()
{
    prvBleTestGattServerInit_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_On() );
}

void test_IotBle_On_Register_Server_Failed()
{
    prvBleTestRegisterServer_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_On() );
}

void test_IotBle_On_SetAdvData_Failed()
{
    prvBleTestSetAdvData_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_On() );
}
void test_IotBle_On_Start_Adv_Failed()
{
    prvBleTestStartAdv_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_On() );
}

void test_IotBle_Off_Stop_Adv_Failed()
{
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_On() );
    prvBleTestStopAdv_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_Off() );
}

void test_IotBle_Off_UnregisterServer_Failed()
{
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_On() );
    prvBleTestUnregisterServer_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_Off() );
}


void test_IotBle_Off_Disable_Failed()
{
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_On() );
    prvBleTestDisable_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_Off() );
}

void test_IotBle_On_OFF( void )
{
    prvTestTurnOnBLE();

    TEST_ASSERT_EQUAL( 0, strcmp( bleConfig.deviceName, IOT_BLE_DEVICE_COMPLETE_LOCAL_NAME ) );
    TEST_ASSERT_EQUAL( true, bleConfig.needSecureConnection );
    TEST_ASSERT_EQUAL( true, bleConfig.shouldBond );
    TEST_ASSERT_EQUAL( eBTIODisplayYesNo, bleConfig.ioCapability );

    prvTestTurnOffBLE();
}

void test_IotBle_On_OFF_MultipleTimes( void )
{
    prvTestTurnOnBLE();
    prvTestTurnOnBLE();
    prvTestTurnOffBLE();
    prvTestTurnOffBLE();
}

void test_IotBleSetDeviceName_While_BLE_Is_On( void )
{
    prvTestTurnOnBLE();

    prvBleTestSetDeviceProperty_Stub( prvSaveConfiguration );

    prvBleTestSetAdvData_IgnoreAndReturn( eBTStatusSuccess );
    IotSemaphore_Wait_Ignore();
    prvBleTestSetAdvData_IgnoreAndReturn( eBTStatusSuccess );
    IotSemaphore_Wait_Ignore();
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_SetDeviceName( CUSTOM_BLE_DEVICE_NAME, strlen( CUSTOM_BLE_DEVICE_NAME ) ) );

    TEST_ASSERT_EQUAL( 0, strncmp( bleConfig.deviceName, CUSTOM_BLE_DEVICE_NAME, strlen( CUSTOM_BLE_DEVICE_NAME ) ) );

    prvTestTurnOffBLE();
}

void test_IotBleSetDeviceName_While_BLE_Is_Off( void )
{
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_SetDeviceName( CUSTOM_BLE_DEVICE_NAME, strlen( CUSTOM_BLE_DEVICE_NAME ) ) );

    /* Make sure BLE turns on with the new device name. */
    prvTestTurnOnBLE();

    TEST_ASSERT_EQUAL( 0, strncmp( bleConfig.deviceName, CUSTOM_BLE_DEVICE_NAME, strlen( CUSTOM_BLE_DEVICE_NAME ) ) );

    prvTestTurnOffBLE();
}


void test_IotBleSetDeviceName_ToggleBLE( void )
{
    prvTestTurnOnBLE();

    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_SetDeviceName( CUSTOM_BLE_DEVICE_NAME, strlen( CUSTOM_BLE_DEVICE_NAME ) ) );

    TEST_ASSERT_EQUAL( 0, strncmp( bleConfig.deviceName, CUSTOM_BLE_DEVICE_NAME, strlen( CUSTOM_BLE_DEVICE_NAME ) ) );

    prvTestTurnOffBLE();
    prvTestTurnOnBLE();
    TEST_ASSERT_EQUAL( 0, strncmp( bleConfig.deviceName, CUSTOM_BLE_DEVICE_NAME, strlen( CUSTOM_BLE_DEVICE_NAME ) ) );
    prvTestTurnOffBLE();
}


void test_IotBleStartAdvertisement( void )
{
    prvTestTurnOnBLE();

    /* Start Advertisement */
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_StartAdv( prvAdvertisementCallback ) );

    TEST_ASSERT_EQUAL( 1, numAdvertisementStatusCalls );
    prvTestTurnOffBLE();
}

void test_IotBleStopAdvertisement( void )
{
    prvTestTurnOnBLE();
    prvBleTestStopAdv_Stub( prvStopAdvCallback );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_StopAdv( prvAdvertisementCallback ) );
    TEST_ASSERT_EQUAL( 1, numAdvertisementStatusCalls );
    prvTestTurnOffBLE();
}

void test_IotBleSetStopAdvertisementCallback( void )
{
    prvTestTurnOnBLE();
    prvBleTestStopAdv_Stub( prvStopAdvCallback );

    /* Register callback for a stop advertisement event from stack. */
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_SetStopAdvCallback( prvAdvertisementCallback ) );

    /* Verify that stop advertisement event from stack invokes the callback. */
    middlewareBleCallback.pxAdvStatusCb( eBTStatusSuccess, 0, false );
    TEST_ASSERT_EQUAL( 1, numAdvertisementStatusCalls );

    /* Verify that start advertisement event from stack does not invoke the callback. */
    middlewareBleCallback.pxAdvStatusCb( eBTStatusSuccess, 0, true );
    TEST_ASSERT_EQUAL( 1, numAdvertisementStatusCalls );

    prvTestTurnOffBLE();
}

static void prvMtuChangedCallback( uint16_t connId,
                                   uint16_t mtu )
{
    eventCallbackCount[ eBLEMtuChanged ]++;
}

static void prvConnectionCallback( BTStatus_t status,
                                   uint16_t connId,
                                   bool connected,
                                   BTBdaddr_t * pRemoteBdAddr )
{
    eventCallbackCount[ eBLEConnection ]++;
}
static void prvPairingStateChanged( BTStatus_t status,
                                    BTBdaddr_t * pRemoteBdAddr,
                                    BTBondState_t bondstate,
                                    BTSecurityLevel_t securityLevel,
                                    BTAuthFailureReason_t reason )
{
    eventCallbackCount[ eBLEPairingStateChanged ]++;
}

static void prvConnParameterUpdateRequestCallback( BTStatus_t status,
                                                   const BTBdaddr_t * pRemoteBdAddr,
                                                   IotBleConnectionParam_t * pConnectionParam,
                                                   uint32_t connInterval )
{
    eventCallbackCount[ eBLEConnParameterUpdateRequestCallback ]++;
}
static void prvNumericComparisonCallback( BTBdaddr_t * pRemoteBdAddr,
                                          uint32_t passKey )
{
    TEST_ASSERT_EQUAL( 123, passKey );
    eventCallbackCount[ eBLENumericComparisonCallback ]++;
}


static IotBleEventsCallbacks_t eventCallbacks[ eNbEvents ] =
{
    { .pMtuChangedCb                 = prvMtuChangedCallback                 },
    { .pConnectionCb                 = prvConnectionCallback                 },
    { .pGAPPairingStateChangedCb     = prvPairingStateChanged                },
    { .pConnParameterUpdateRequestCb = prvConnParameterUpdateRequestCallback },
    { .pNumericComparisonCb          = prvNumericComparisonCallback          }
};

static void prvSendIndicationMTUChangeCallback( uint16_t connId,
                                                uint16_t mtu )
{
    IotBleAttributeData_t attr =
    {
        .handle = 101
    };
    IotBleEventResponse_t response =
    {
        .pAttrData = &attr
    };

    TEST_ASSERT_EQUAL( DUMMY_MTU_SIZE, mtu );
    TEST_ASSERT_EQUAL( DUMMY_CONNECTION_ID, connId );
    eventCallbackCount[ eBLEMtuChanged ]++;

    IotBle_SendIndication( &response, DUMMY_CONNECTION_ID, false );
}

static BTStatus_t pxSendIndicationStub( uint8_t ucServerIf,
                                        uint16_t usAttributeHandle,
                                        uint16_t usConnId,
                                        size_t xLen,
                                        uint8_t * pucValue,
                                        bool bConfirm,
                                        int cmock_calls )
{
    TEST_ASSERT_EQUAL( DUMMY_CONNECTION_ID, usConnId );
    middlewareGATTCallback.pxIndicationSentCb( 0, eBTStatusSuccess );
    return eBTStatusSuccess;
}


void test_IotBleRegisterCallbacks( void )
{
    uint8_t ucx;
    BTBdaddr_t dummyAddr = { 0 };
    BTBdname_t dummyName = { 0 };

    prvTestTurnOnBLE();
    IotBleEventsCallbacks_t nullCallback = { 0 };

    for( ucx = 0; ucx < eNbEvents; ucx++ )
    {
        TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_RegisterEventCb( ( IotBleEvents_t ) ucx, eventCallbacks[ ucx ] ) );
    }

    /* Send an advertisement stop callback from stack. */
    middlewareGATTCallback.pxConnectionCb( 1, 0, true, &dummyAddr );
    middlewareGATTCallback.pxMtuChangedCb( 1, IOT_BLE_PREFERRED_MTU_SIZE );
    middlewareBTCallback.pxSspRequestCb( &dummyAddr, &dummyName, 0, eBTsspVariantPasskeyConfirmation, 123 );
    middlewareBTCallback.pxPairingStateChangedCb( eBTStatusSuccess, &dummyAddr, eBTbondStateBonded, eBTSecLevelAuthenticatedPairing, eBTauthSuccess );

    for( ucx = 0; ucx < eNbEvents; ucx++ )
    {
        /* Connection parameter callback not implemented. */
        if( ucx != eBLEConnParameterUpdateRequestCallback )
        {
            TEST_ASSERT_EQUAL( 1, eventCallbackCount[ ucx ] );
        }
    }

    /* Register a null callback should fail. */
    TEST_ASSERT_EQUAL( eBTStatusParamInvalid, IotBle_RegisterEventCb( eBLEMtuChanged, nullCallback ) );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_UnRegisterEventCb( eBLEMtuChanged, nullCallback ) );

    for( ucx = 0; ucx < eNbEvents; ucx++ )
    {
        TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_UnRegisterEventCb( ( IotBleEvents_t ) ucx, eventCallbacks[ ucx ] ) );
    }

    /* Unregister a callback twice should fail. */
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_UnRegisterEventCb( eBLEMtuChanged, eventCallbacks[ eBLEMtuChanged ] ) );


    prvTestTurnOffBLE();
}

void test_IotBleConnectionParameterUpdateRequest( void )
{
    BTBdaddr_t dummyAddr = { 0 };
    IotBleConnectionParam_t connectionParams = { 0 };

    prvTestTurnOnBLE();

    /* Currently not implemented. */
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_ConnParameterUpdateRequest( &dummyAddr, &connectionParams ) );

    prvTestTurnOffBLE();
}

void test_IotBleRemoveBond( void )
{
    BTBdaddr_t dummyAddr = { 0 };

    prvTestTurnOnBLE();

    prvBleTestRemoveBond_ExpectAndReturn( &dummyAddr, eBTStatusSuccess );

    /* Currently not implemented. */
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_RemoveBond( &dummyAddr ) );

    prvTestTurnOffBLE();
}

static BTStatus_t prvAddServiceBlobStub( uint8_t ucServerIf,
                                         BTService_t * pService,
                                         int cmock_num_calls )
{
    size_t index = 0;

    for( index = 0; index < pService->xNumberOfAttributes; index++ )
    {
        pService->pusHandlesBuffer[ index ] = SERVICE_HANDLE + index;
    }

    middlewareGATTCallback.pxServiceAddedCb( eBTStatusSuccess, ucServerIf, NULL, SERVICE_HANDLE );

    return eBTStatusSuccess;
}

static BTStatus_t prvServiceDeleteStub( uint8_t ucServerIf,
                                        uint16_t usServiceHandle,
                                        int cmock_num_calls )
{
    middlewareGATTCallback.pxServiceDeletedCb( eBTStatusSuccess, 0, usServiceHandle );
    return eBTStatusSuccess;
}

void test_IotBleCreateServiceBlob( void )
{
    uint16_t handlesBuffer[ NUM_SERVICE_ATTRIBUTES ] = { 0 };
    uint16_t inclHandlesBuffer[ NUM_SECONDARY_SERVICE_ATTRIBUTES ] = { 0 };

    includedService.pusHandlesBuffer = inclHandlesBuffer;
    includedService.ucInstId = 1;
    includedService.xType = eBTDbSecondaryService;
    includedService.xNumberOfAttributes = NUM_SECONDARY_SERVICE_ATTRIBUTES;

    BTService_t service =
    {
        .ucInstId            = 0,
        .xType               = eBTDbPrimaryService,
        .xNumberOfAttributes = NUM_SERVICE_ATTRIBUTES,
        .pusHandlesBuffer    = handlesBuffer,
        .pxBLEAttributes     = attributeTable
    };

    prvTestTurnOnBLE();


    prvBleTestAddServiceBlob_Stub( prvAddServiceBlobStub );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_CreateService( &service, attributeCallbacks ) );

    prvBleTestStopService_IgnoreAndReturn( eBTStatusSuccess );
    prvBleTestDeleteService_Stub( prvServiceDeleteStub );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_DeleteService( &service ) );

    prvTestTurnOffBLE();
}
static BTStatus_t prvAddServiceStub( uint8_t ucServerIf,
                                     BTGattSrvcId_t * prvBleTestSrvcId,
                                     uint16_t usNumHandles,
                                     int cmock_num_calls )
{
    middlewareGATTCallback.pxServiceAddedCb( eBTStatusSuccess, 0, prvBleTestSrvcId, SERVICE_HANDLE );
    return eBTStatusSuccess;
}

static BTStatus_t prvAddCharStub( uint8_t ucServerIf,
                                  uint16_t usServiceHandle,
                                  BTUuid_t * prvBleTestUuid,
                                  BTCharProperties_t xProperties,
                                  BTCharPermissions_t xPermissions,
                                  int cmock_num_calls )
{
    TEST_ASSERT_EQUAL( SERVICE_HANDLE, usServiceHandle );
    middlewareGATTCallback.pxCharacteristicAddedCb( eBTStatusSuccess, 0, prvBleTestUuid, usServiceHandle, SERVICE_HANDLE + 1 );
    return eBTStatusSuccess;
}

static BTStatus_t prvAddDescrStub( uint8_t ucServerIf,
                                   uint16_t usServiceHandle,
                                   BTUuid_t * prvBleTestUuid,
                                   BTCharPermissions_t ulPermissions,
                                   int cmock_num_calls )
{
    TEST_ASSERT_EQUAL( SERVICE_HANDLE, usServiceHandle );
    middlewareGATTCallback.pxDescriptorAddedCb( eBTStatusSuccess, 0, prvBleTestUuid, usServiceHandle, SERVICE_HANDLE + 2 );
    return eBTStatusSuccess;
}
static BTStatus_t prvAddIncludedServiceStub( uint8_t ucServerIf,
                                             uint16_t usServiceHandle,
                                             uint16_t usIncludedHandle,
                                             int cmock_num_calls )

{
    TEST_ASSERT_EQUAL( SERVICE_HANDLE, usServiceHandle );
    TEST_ASSERT_EQUAL( SECONDARY_SERVICE_HANDLE, usIncludedHandle );
    middlewareGATTCallback.pxIncludedServiceAddedCb( eBTStatusSuccess, 0, usServiceHandle, SERVICE_HANDLE + 3 );
    return eBTStatusSuccess;
}

static BTStatus_t prvStartServiceStub( uint8_t ucServerIf,
                                       uint16_t usServiceHandle,
                                       BTTransport_t xTransport,
                                       int cmock_num_calls )
{
    middlewareGATTCallback.pxServiceStartedCb( eBTStatusSuccess, 0, usServiceHandle );
    return eBTStatusSuccess;
}

static BTStatus_t prvStopServiceStub( uint8_t ucServerIf,
                                      uint16_t usServiceHandle,
                                      int cmock_num_calls )
{
    middlewareGATTCallback.pxServiceStoppedCb( eBTStatusSuccess, 0, usServiceHandle );
    return eBTStatusSuccess;
}


void test_IotBleCreateService( void )
{
    uint16_t handlesBuffer[ NUM_SERVICE_ATTRIBUTES ] = { 0 };
    uint16_t inclHandlesBuffer[ NUM_SECONDARY_SERVICE_ATTRIBUTES ] = { SECONDARY_SERVICE_HANDLE };

    includedService.pusHandlesBuffer = inclHandlesBuffer;
    includedService.ucInstId = 1;
    includedService.xType = eBTDbSecondaryService;
    includedService.xNumberOfAttributes = NUM_SECONDARY_SERVICE_ATTRIBUTES;
    includedService.pxBLEAttributes = inclAttributeTable;

    BTService_t service =
    {
        .ucInstId            = 0,
        .xType               = eBTDbPrimaryService,
        .xNumberOfAttributes = NUM_SERVICE_ATTRIBUTES,
        .pusHandlesBuffer    = handlesBuffer,
        .pxBLEAttributes     = attributeTable
    };

    prvTestTurnOnBLE();

    prvBleTestStartService_Stub( prvStartServiceStub );
    prvBleTestAddServiceBlob_IgnoreAndReturn( eBTStatusUnsupported );
    prvBleTestAddService_Stub( prvAddServiceStub );
    prvBleTestAddCharacteristic_Stub( prvAddCharStub );
    prvBleTestAddDescriptor_Stub( prvAddDescrStub );
    prvBleTestAddIncludedService_Stub( prvAddIncludedServiceStub );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_CreateService( &service, attributeCallbacks ) );

    prvBleTestStopService_Stub( prvStopServiceStub );
    prvBleTestDeleteService_Stub( prvServiceDeleteStub );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_DeleteService( &service ) );

    prvTestTurnOffBLE();
}


void test_IotBleCreateServiceBlob_ReturnFailure( void )
{
    uint16_t handlesBuffer[ NUM_SERVICE_ATTRIBUTES ] = { 0 };
    uint16_t inclHandlesBuffer[ NUM_SECONDARY_SERVICE_ATTRIBUTES ] = { SECONDARY_SERVICE_HANDLE };

    includedService.pusHandlesBuffer = inclHandlesBuffer;
    includedService.ucInstId = 1;
    includedService.xType = eBTDbSecondaryService;
    includedService.xNumberOfAttributes = NUM_SECONDARY_SERVICE_ATTRIBUTES;
    includedService.pxBLEAttributes = inclAttributeTable;

    BTService_t service =
    {
        .ucInstId            = 0,
        .xType               = eBTDbPrimaryService,
        .xNumberOfAttributes = NUM_SERVICE_ATTRIBUTES,
        .pusHandlesBuffer    = handlesBuffer,
        .pxBLEAttributes     = attributeTable
    };

    prvTestTurnOnBLE();

    prvBleTestAddServiceBlob_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_CreateService( &service, attributeCallbacks ) );

    prvTestTurnOffBLE();
}


static BTStatus_t prvAddServiceFailureStub( uint8_t ucServerIf,
                                            BTGattSrvcId_t * prvBleTestSrvcId,
                                            uint16_t usNumHandles,
                                            int cmock_num_calls )
{
    middlewareGATTCallback.pxServiceAddedCb( eBTStatusFail, 0, prvBleTestSrvcId, SERVICE_HANDLE );
    return eBTStatusSuccess;
}

static BTStatus_t prvAddCharFailureStub( uint8_t ucServerIf,
                                         uint16_t usServiceHandle,
                                         BTUuid_t * prvBleTestUuid,
                                         BTCharProperties_t xProperties,
                                         BTCharPermissions_t xPermissions,
                                         int cmock_num_calls )
{
    TEST_ASSERT_EQUAL( SERVICE_HANDLE, usServiceHandle );
    middlewareGATTCallback.pxCharacteristicAddedCb( eBTStatusFail, 0, prvBleTestUuid, usServiceHandle, SERVICE_HANDLE + 1 );
    return eBTStatusSuccess;
}

static BTStatus_t prvAddDescrFailureStub( uint8_t ucServerIf,
                                          uint16_t usServiceHandle,
                                          BTUuid_t * prvBleTestUuid,
                                          BTCharPermissions_t ulPermissions,
                                          int cmock_num_calls )
{
    TEST_ASSERT_EQUAL( SERVICE_HANDLE, usServiceHandle );
    middlewareGATTCallback.pxDescriptorAddedCb( eBTStatusFail, 0, prvBleTestUuid, usServiceHandle, SERVICE_HANDLE + 2 );
    return eBTStatusSuccess;
}
static BTStatus_t prvAddIncludedServiceFailureStub( uint8_t ucServerIf,
                                                    uint16_t usServiceHandle,
                                                    uint16_t usIncludedHandle,
                                                    int cmock_num_calls )

{
    TEST_ASSERT_EQUAL( SERVICE_HANDLE, usServiceHandle );
    TEST_ASSERT_EQUAL( SECONDARY_SERVICE_HANDLE, usIncludedHandle );
    middlewareGATTCallback.pxIncludedServiceAddedCb( eBTStatusFail, 0, usServiceHandle, SERVICE_HANDLE + 3 );
    return eBTStatusSuccess;
}

static BTStatus_t prvStartServiceFailureStub( uint8_t ucServerIf,
                                              uint16_t usServiceHandle,
                                              BTTransport_t xTransport,
                                              int cmock_num_calls )
{
    middlewareGATTCallback.pxServiceStartedCb( eBTStatusFail, 0, usServiceHandle );
    return eBTStatusSuccess;
}

void test_IotBleCreateService_FailureCases( void )
{
    uint16_t handlesBuffer[ NUM_SERVICE_ATTRIBUTES ] = { 0 };
    uint16_t inclHandlesBuffer[ NUM_SECONDARY_SERVICE_ATTRIBUTES ] = { SECONDARY_SERVICE_HANDLE };

    includedService.pusHandlesBuffer = inclHandlesBuffer;
    includedService.ucInstId = 1;
    includedService.xType = eBTDbSecondaryService;
    includedService.xNumberOfAttributes = NUM_SECONDARY_SERVICE_ATTRIBUTES;
    includedService.pxBLEAttributes = inclAttributeTable;

    BTService_t service =
    {
        .ucInstId            = 0,
        .xType               = eBTDbPrimaryService,
        .xNumberOfAttributes = NUM_SERVICE_ATTRIBUTES,
        .pusHandlesBuffer    = handlesBuffer,
        .pxBLEAttributes     = attributeTable
    };

    prvTestTurnOnBLE();

    prvBleTestAddServiceBlob_IgnoreAndReturn( eBTStatusUnsupported );

    prvBleTestStartService_Stub( prvStartServiceStub );
    prvBleTestAddService_Stub( prvAddServiceStub );
    prvBleTestAddCharacteristic_Stub( prvAddCharStub );
    prvBleTestAddDescriptor_Stub( prvAddDescrStub );
    prvBleTestAddIncludedService_Stub( prvAddIncludedServiceStub );

    /* Add service returns failure */
    prvBleTestAddService_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_CreateService( &service, attributeCallbacks ) );
    prvBleTestAddService_Stub( prvAddServiceFailureStub );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_CreateService( &service, attributeCallbacks ) );

    /* Add characteristic returns failure */
    prvBleTestAddService_Stub( prvAddServiceStub );
    prvBleTestAddCharacteristic_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_CreateService( &service, attributeCallbacks ) );
    prvBleTestAddCharacteristic_Stub( prvAddCharFailureStub );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_CreateService( &service, attributeCallbacks ) );

    /* Add descriptor returns failure */
    prvBleTestAddCharacteristic_Stub( prvAddCharStub );
    prvBleTestAddDescriptor_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_CreateService( &service, attributeCallbacks ) );
    prvBleTestAddDescriptor_Stub( prvAddDescrFailureStub );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_CreateService( &service, attributeCallbacks ) );

    /* Add inclded service returns failure */
    prvBleTestAddCharacteristic_Stub( prvAddCharStub );
    prvBleTestAddDescriptor_Stub( prvAddDescrStub );
    prvBleTestAddIncludedService_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_CreateService( &service, attributeCallbacks ) );
    prvBleTestAddIncludedService_Stub( prvAddIncludedServiceFailureStub );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_CreateService( &service, attributeCallbacks ) );

    /* Start service returns failure */
    prvBleTestAddIncludedService_Stub( prvAddIncludedServiceStub );
    prvBleTestStartService_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_CreateService( &service, attributeCallbacks ) );
    prvBleTestStartService_Stub( prvStartServiceFailureStub );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_CreateService( &service, attributeCallbacks ) );

    /* Malloc failed. */
    pvPortMalloc_IgnoreAndReturn( NULL );
    TEST_ASSERT_EQUAL( eBTStatusNoMem, IotBle_CreateService( &service, attributeCallbacks ) );

    pvPortMalloc_Stub( pvPortMalloc_Callback );
    prvTestTurnOffBLE();
}



static BTStatus_t prvStopServiceFailureStub( uint8_t ucServerIf,
                                             uint16_t usServiceHandle,
                                             int cmock_num_calls )
{
    middlewareGATTCallback.pxServiceStoppedCb( eBTStatusFail, 0, usServiceHandle );
    return eBTStatusSuccess;
}


void test_IotBleDeleteService_ReturnsFailure( void )
{
    uint16_t handlesBuffer[ NUM_SERVICE_ATTRIBUTES ] = { 0 };
    uint16_t inclHandlesBuffer[ NUM_SECONDARY_SERVICE_ATTRIBUTES ] = { SECONDARY_SERVICE_HANDLE };

    includedService.pusHandlesBuffer = inclHandlesBuffer;
    includedService.ucInstId = 1;
    includedService.xType = eBTDbSecondaryService;
    includedService.xNumberOfAttributes = NUM_SECONDARY_SERVICE_ATTRIBUTES;
    includedService.pxBLEAttributes = inclAttributeTable;

    BTService_t service =
    {
        .ucInstId            = 0,
        .xType               = eBTDbPrimaryService,
        .xNumberOfAttributes = NUM_SERVICE_ATTRIBUTES,
        .pusHandlesBuffer    = handlesBuffer,
        .pxBLEAttributes     = attributeTable
    };

    prvTestTurnOnBLE();

    /* Create new service. */
    prvBleTestStartService_Stub( prvStartServiceStub );
    prvBleTestAddServiceBlob_IgnoreAndReturn( eBTStatusUnsupported );
    prvBleTestAddService_Stub( prvAddServiceStub );
    prvBleTestAddCharacteristic_Stub( prvAddCharStub );
    prvBleTestAddDescriptor_Stub( prvAddDescrStub );
    prvBleTestAddIncludedService_Stub( prvAddIncludedServiceStub );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_CreateService( &service, attributeCallbacks ) );

    prvBleTestDeleteService_Stub( prvServiceDeleteStub );

    /* Stop service returns failure. */
    prvBleTestStopService_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_DeleteService( &service ) );
    prvBleTestStopService_Stub( prvStopServiceFailureStub );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_DeleteService( &service ) );

    prvBleTestStopService_Stub( prvStopServiceStub );
    prvBleTestDeleteService_IgnoreAndReturn( eBTStatusFail );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_DeleteService( &service ) );

    prvBleTestStopService_Stub( prvStopServiceStub );
    prvBleTestDeleteService_Stub( prvServiceDeleteStub );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_DeleteService( &service ) );

    prvTestTurnOffBLE();
}

void test_ServiceAttributeInteractions( void )
{
    uint16_t handlesBuffer[ NUM_SERVICE_ATTRIBUTES ] = { 0 };
    uint16_t inclHandlesBuffer[ NUM_SECONDARY_SERVICE_ATTRIBUTES ] = { SECONDARY_SERVICE_HANDLE };

    includedService.pusHandlesBuffer = inclHandlesBuffer;
    includedService.ucInstId = 1;
    includedService.xType = eBTDbSecondaryService;
    includedService.xNumberOfAttributes = NUM_SECONDARY_SERVICE_ATTRIBUTES;
    includedService.pxBLEAttributes = inclAttributeTable;

    BTService_t service =
    {
        .ucInstId            = 0,
        .xType               = eBTDbPrimaryService,
        .xNumberOfAttributes = NUM_SERVICE_ATTRIBUTES,
        .pusHandlesBuffer    = handlesBuffer,
        .pxBLEAttributes     = attributeTable
    };

    BTBdaddr_t dummyAddr = { 0 };
    uint8_t value = 0x01;
    IotBleAttributeData_t attr =
    {
        .handle = 101
    };
    IotBleEventResponse_t response =
    {
        .pAttrData = &attr
    };

    prvTestTurnOnBLE();

    prvBleTestAddServiceBlob_Stub( prvAddServiceBlobStub );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_CreateService( &service, attributeCallbacks ) );

    /* Write request on characteristic with response required. */
    middlewareGATTCallback.pxRequestWriteCb( 0, 0, &dummyAddr, 101, 0, 1, true, false, &value );
    TEST_ASSERT_EQUAL( 1, attributeInvokedCount[ 1 ][ 0 ] );
    TEST_ASSERT_EQUAL( 1, attributeInvokedCount[ 0 ][ 0 ] );

    /* Write command on characteristic without a reponse required. */
    middlewareGATTCallback.pxRequestWriteCb( 0, 0, &dummyAddr, 101, 0, 1, false, false, &value );
    TEST_ASSERT_EQUAL( 1, attributeInvokedCount[ 1 ][ 1 ] );
    TEST_ASSERT_EQUAL( 2, attributeInvokedCount[ 0 ][ 0 ] );

    /* Read request on characteristic with a response required. */
    middlewareGATTCallback.pxRequestReadCb( 0, 0, &dummyAddr, 101, 0 );
    TEST_ASSERT_EQUAL( 1, attributeInvokedCount[ 1 ][ 2 ] );
    TEST_ASSERT_EQUAL( 3, attributeInvokedCount[ 0 ][ 0 ] );

    /* Write request on descriptor. */
    middlewareGATTCallback.pxRequestWriteCb( 0, 0, &dummyAddr, 102, 0, 1, true, false, &value );
    TEST_ASSERT_EQUAL( 1, attributeInvokedCount[ 2 ][ 0 ] );
    TEST_ASSERT_EQUAL( 4, attributeInvokedCount[ 0 ][ 0 ] );

    /* Execute Write request before prepare write request should be handled gracefully. */
    middlewareGATTCallback.pxRequestExecWriteCb( 0, 101, &dummyAddr, true );
    TEST_ASSERT_EQUAL( 1, attributeInvokedCount[ 1 ][ 0 ] );
    TEST_ASSERT_EQUAL( 4, attributeInvokedCount[ 0 ][ 0 ] );

    /* Prepare Write request on characteristic. */
    middlewareGATTCallback.pxRequestWriteCb( 0, 0, &dummyAddr, 101, 0, 1, true, true, &value );
    TEST_ASSERT_EQUAL( 2, attributeInvokedCount[ 1 ][ 0 ] );
    TEST_ASSERT_EQUAL( 5, attributeInvokedCount[ 0 ][ 0 ] );

    /* Execute Write request on characteristic. */
    middlewareGATTCallback.pxRequestExecWriteCb( 0, 101, &dummyAddr, true );
    TEST_ASSERT_EQUAL( 1, attributeInvokedCount[ 1 ][ 3 ] );
    TEST_ASSERT_EQUAL( 6, attributeInvokedCount[ 0 ][ 0 ] );



    /* Send Response on a characteristic.*/
    prvBleTestSendResponse_IgnoreAndReturn( eBTStatusSuccess );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_SendResponse( &response, 0, true ) );
    middlewareGATTCallback.pxResponseConfirmationCb( eBTStatusSuccess, 101 );
    TEST_ASSERT_EQUAL( 1, attributeInvokedCount[ 1 ][ 4 ] );
    TEST_ASSERT_EQUAL( 7, attributeInvokedCount[ 0 ][ 0 ] );

    /* Response received before sending indication. */
    middlewareGATTCallback.pxIndicationSentCb( 0, eBTStatusSuccess );
    TEST_ASSERT_EQUAL( 7, attributeInvokedCount[ 0 ][ 0 ] );


    /* Send indication with a response for a characteristic. */
    prvBleTestSendIndication_IgnoreAndReturn( eBTStatusSuccess );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_SendIndication( &response, 0, true ) );
    middlewareGATTCallback.pxIndicationSentCb( 0, eBTStatusSuccess );
    TEST_ASSERT_EQUAL( 8, attributeInvokedCount[ 0 ][ 0 ] );

    /*
     * Test for out of range handles. Callbacks should be handled gracefully
     *  and should not invoke user level callback.
     */
    middlewareGATTCallback.pxRequestWriteCb( 0, 0, &dummyAddr, 107, 0, 1, true, false, &value );
    middlewareGATTCallback.pxRequestWriteCb( 0, 0, &dummyAddr, 108, 0, 1, false, true, &value );
    middlewareGATTCallback.pxRequestReadCb( 0, 0, &dummyAddr, 109, 0 );
    middlewareGATTCallback.pxRequestWriteCb( 0, 0, &dummyAddr, 107, 0, 1, true, true, &value );
    middlewareGATTCallback.pxResponseConfirmationCb( eBTStatusSuccess, 110 );
    TEST_ASSERT_EQUAL( 8, attributeInvokedCount[ 0 ][ 0 ] );

    prvBleTestStopService_Stub( prvStopServiceStub );
    prvBleTestDeleteService_Stub( prvServiceDeleteStub );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_DeleteService( &service ) );

    prvTestTurnOffBLE();
}

void test_IotBle_GATTAPIs_NullParams()
{
    prvTestTurnOnBLE();

    TEST_ASSERT_EQUAL( eBTStatusParamInvalid, IotBle_CreateService( NULL, NULL ) );

    prvBleTestSendResponse_IgnoreAndReturn( eBTStatusSuccess );
    TEST_ASSERT_EQUAL( eBTStatusParamInvalid, IotBle_SendResponse( NULL, 0, true ) );

    prvBleTestSendIndication_IgnoreAndReturn( eBTStatusSuccess );
    TEST_ASSERT_EQUAL( eBTStatusParamInvalid, IotBle_SendIndication( NULL, 0, true ) );


    prvBleTestStopService_IgnoreAndReturn( eBTStatusSuccess );
    prvBleTestDeleteService_Stub( prvServiceDeleteStub );
    TEST_ASSERT_EQUAL( eBTStatusParamInvalid, IotBle_DeleteService( NULL ) );
}

void test_GetConnections( void )
{
    BTBdaddr_t addr1 =
    {
        .ucAddress = { 0xAA }
    };
    BTBdaddr_t addr2 =
    {
        .ucAddress = { 0xAA }
    };
    uint8_t ucIdx;
    IotBleConnectionInfoListElement_t * pConnectionInfo;
    IotLink_t * pConnectionList, * pLink;

    prvTestTurnOnBLE();

    middlewareGATTCallback.pxConnectionCb( 0, 0, true, &addr1 );
    middlewareGATTCallback.pxConnectionCb( 1, 0, true, &addr2 );

    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_GetConnectionInfoList( &pConnectionList ) );
    ucIdx = 1;
    IotContainers_ForEach( pConnectionList, pLink )
    {
        pConnectionInfo = IotLink_Container( IotBleConnectionInfoListElement_t, pLink, connectionList );
        TEST_ASSERT_EQUAL( ucIdx, pConnectionInfo->connId );
        ucIdx--;
    }


    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_GetConnectionInfo( 0, &pConnectionInfo ) );
    TEST_ASSERT_EQUAL( 0, pConnectionInfo->connId );
    TEST_ASSERT_EQUAL_UINT8_ARRAY( addr1.ucAddress, pConnectionInfo->remoteBdAddr.ucAddress, btADDRESS_LEN );

    middlewareGATTCallback.pxConnectionCb( 0, 0, false, &addr1 );
    middlewareGATTCallback.pxConnectionCb( 1, 0, false, &addr2 );

    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_GetConnectionInfo( 0, &pConnectionInfo ) );

    /* Calling disconnect cb for a non existent ID should be a NOP */
    middlewareGATTCallback.pxConnectionCb( 1, 0, false, &addr2 );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_GetConnectionInfo( 0, &pConnectionInfo ) );

    prvTestTurnOffBLE();
}

void test_IotBleJustWorksPairing( void )
{
    BTBdaddr_t dummyAddr = { 0 };
    BTBdname_t dummyName = { 0 };

    prvTestTurnOnBLE();

    eventCallbackCount[ eBLENumericComparisonCallback ] = 0;

    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_RegisterEventCb( eBLENumericComparisonCallback, eventCallbacks[ eBLENumericComparisonCallback ] ) );

    prvBleTestSspReply_ExpectAndReturn( &dummyAddr, eBTsspVariantConsent, true, 0, eBTStatusSuccess );
    middlewareBTCallback.pxSspRequestCb( &dummyAddr, &dummyName, 0, eBTsspVariantConsent, 0 );

    TEST_ASSERT_EQUAL( 0, eventCallbackCount[ eBLENumericComparisonCallback ] );

    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_UnRegisterEventCb( eBLENumericComparisonCallback, eventCallbacks[ eBLENumericComparisonCallback ] ) );

    prvTestTurnOffBLE();
}

void test_IotBleNumericComparisonPairing( void )
{
    BTBdaddr_t dummyAddr = { 0 };
    BTBdname_t dummyName = { 0 };

    prvTestTurnOnBLE();

    eventCallbackCount[ eBLENumericComparisonCallback ] = 0;

    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_RegisterEventCb( eBLENumericComparisonCallback, eventCallbacks[ eBLENumericComparisonCallback ] ) );

    middlewareBTCallback.pxSspRequestCb( &dummyAddr, &dummyName, 0, eBTsspVariantPasskeyConfirmation, 123 );
    TEST_ASSERT_EQUAL( 1, eventCallbackCount[ eBLENumericComparisonCallback ] );

    prvBleTestSspReply_ExpectAndReturn( &dummyAddr, eBTsspVariantPasskeyConfirmation, true, 0, eBTStatusSuccess );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_ConfirmNumericComparisonKeys( &dummyAddr, true ) );

    prvBleTestSspReply_ExpectAndReturn( &dummyAddr, eBTsspVariantPasskeyConfirmation, false, 0, eBTStatusSuccess );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_ConfirmNumericComparisonKeys( &dummyAddr, false ) );

    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_UnRegisterEventCb( eBLENumericComparisonCallback, eventCallbacks[ eBLENumericComparisonCallback ] ) );

    prvTestTurnOffBLE();
}

void test_IotBleMallocFailure( void )
{
    BTBdaddr_t addr;
    IotBleConnectionInfoListElement_t * pConnectionInfo;

    prvTestTurnOnBLE();

    pvPortMalloc_IgnoreAndReturn( NULL );

    /* Adding a new connection should fail but the callback is exited gracefully. */
    middlewareGATTCallback.pxConnectionCb( 0, 0, true, &addr );
    TEST_ASSERT_EQUAL( eBTStatusFail, IotBle_GetConnectionInfo( 0, &pConnectionInfo ) );

    /* Numeric comparison callback should fail. */
    TEST_ASSERT_EQUAL( eBTStatusFail,
                       IotBle_RegisterEventCb( eBLENumericComparisonCallback, eventCallbacks[ eBLENumericComparisonCallback ] ) );

    pvPortMalloc_Stub( pvPortMalloc_Callback );
    prvTestTurnOffBLE();
}

void test_IotBleSendIndication_From_Event_Callback( void )
{
    uint16_t handlesBuffer[ NUM_SERVICE_ATTRIBUTES ] = { 0 };
    uint16_t inclHandlesBuffer[ NUM_SECONDARY_SERVICE_ATTRIBUTES ] = { SECONDARY_SERVICE_HANDLE };

    includedService.pusHandlesBuffer = inclHandlesBuffer;
    includedService.ucInstId = 1;
    includedService.xType = eBTDbSecondaryService;
    includedService.xNumberOfAttributes = NUM_SECONDARY_SERVICE_ATTRIBUTES;
    includedService.pxBLEAttributes = inclAttributeTable;

    BTService_t service =
    {
        .ucInstId            = 0,
        .xType               = eBTDbPrimaryService,
        .xNumberOfAttributes = NUM_SERVICE_ATTRIBUTES,
        .pusHandlesBuffer    = handlesBuffer,
        .pxBLEAttributes     = attributeTable
    };

    IotBleEventsCallbacks_t mtuCallback =
    {
        .pMtuChangedCb = prvSendIndicationMTUChangeCallback
    };

    prvTestTurnOnBLE();

    prvBleTestAddServiceBlob_Stub( prvAddServiceBlobStub );
    prvBleTestSendIndication_Stub( pxSendIndicationStub );
    prvBleTestStopService_IgnoreAndReturn( eBTStatusSuccess );
    prvBleTestDeleteService_Stub( prvServiceDeleteStub );

    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_CreateService( &service, attributeCallbacks ) );

    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_RegisterEventCb( eBLEMtuChanged, mtuCallback ) );

    middlewareGATTCallback.pxMtuChangedCb( DUMMY_CONNECTION_ID, DUMMY_MTU_SIZE );

    TEST_ASSERT_EQUAL( 1, eventCallbackCount[ eBLEMtuChanged ] );
    TEST_ASSERT_EQUAL( 1, attributeInvokedCount[ 0 ][ 0 ] );

    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_UnRegisterEventCb( eBLEMtuChanged, mtuCallback ) );

    TEST_ASSERT_EQUAL( eBTStatusSuccess, IotBle_DeleteService( &service ) );

    prvTestTurnOffBLE();
}
