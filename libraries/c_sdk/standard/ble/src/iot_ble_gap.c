/*
 * FreeRTOS BLE V2.2.0
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
 * @file iot_ble_gap.c
 * @brief BLE GAP API.
 */

/* The config header is always included first. */
#include "iot_config.h"

#include <string.h>
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "platform/iot_threads.h"
#include "iot_ble_config.h"
#include "iot_ble.h"
#include "iot_ble_internal.h"
#include "iot_ble_device_information.h"
#include "iot_ble_data_transfer.h"

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_GLOBAL
    #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
#else
    #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
#endif

#define LIBRARY_LOG_NAME         ( "BLE_GAP" )
#include "iot_logging_setup.h"


#ifndef IOT_BLE_ADVERTISING_UUID
    #error "Set IOT_BLE_ADVERTISING_UUID to a valid advertising UUID for the device."
#endif

#ifndef IOT_BLE_ADVERTISING_UUID_SIZE
    #error "Set IOT_BLE_ADVERTISING_UUID_SIZE to the size in bytes of the advertising UUID."
#endif

#if ( IOT_BLE_ADVERTISING_UUID_SIZE == 2 )
    #define BT_ADV_UUID_TYPE    eBTuuidType16
#elif ( IOT_BLE_ADVERTISING_UUID_SIZE == 4 )
    #define BT_ADV_UUID_TYPE    eBTuuidType32
#else
    #define BT_ADV_UUID_TYPE    eBTuuidType128
#endif

static const BTUuid_t advertisementUUID =
{
    .uu.uu128 = IOT_BLE_ADVERTISING_UUID,
    .ucType   = BT_ADV_UUID_TYPE
};

static const BTUuid_t serverUUID =
{
    .ucType   = eBTuuidType128,
    .uu.uu128 = IOT_BLE_SERVER_UUID
};


/**
 * @brief Contains parameters to be set in the scan response data.
 *
 * Note that total available data size in scan response
 * is 31 bytes. Parameters are chosen below such that overall size
 * does not exceed 31 bytes.
 */
static IotBleAdvertisementParams_t _scanRespParams =
{
    .includeTxPower    = true,
    .name              =
    {
        BTGattAdvNameNone,
        0
    },
    .setScanRsp        = true,
    .appearance        = IOT_BLE_ADVERTISING_APPEARANCE,
    .minInterval       = IOT_BLE_ADVERTISING_CONN_INTERVAL_MIN,
    .maxInterval       = IOT_BLE_ADVERTISING_CONN_INTERVAL_MAX,
    .serviceDataLen    = 0,
    .pServiceData      = NULL,
    .manufacturerLen   = 0,
    .pManufacturerData = NULL,
    .pUUID1            = NULL,
    .pUUID2            = NULL
};

/**
 * @brief Contains parameters to be set in the advertisement data.
 *
 * Note that total available data size in advertisement
 * is 31 bytes. Parameters are chosen below such that overall size
 * does not exceed 31 bytes.
 */

static IotBleAdvertisementParams_t _advParams =
{
    .includeTxPower    = true,
    .name              =
    {
        BTGattAdvNameShort,
        IOT_BLE_DEVICE_SHORT_LOCAL_NAME_SIZE
    },
    .setScanRsp        = false,
    .appearance        = IOT_BLE_ADVERTISING_APPEARANCE,
    .minInterval       = 0,
    .maxInterval       = 0,
    .serviceDataLen    = 0,
    .pServiceData      = NULL,
    .manufacturerLen   = 0,
    .pManufacturerData = NULL,
    .pUUID1            = ( BTUuid_t * ) &advertisementUUID,
    .pUUID2            = NULL
};

static char bleDeviceName[ IOT_BLE_DEVICE_LOCAL_NAME_MAX_LENGTH + 1 ] = { 0 };
static bool isBLEOn = false;

/* 2 is the Maximum number of UUID that can be advertised in the same advertisement message */
#define _BLE_MAX_UUID_PER_ADV_MESSAGE    2


_bleInterface_t _BTInterface;

static void _deviceStateChangedCb( BTState_t xState );
static void _adapterPropertiesCb( BTStatus_t status,
                                  uint32_t numProperties,
                                  BTProperty_t * pProperties );
static void _sspRequestCb( BTBdaddr_t * pxemoteBdAddr,
                           BTBdname_t * pRemoteBdName,
                           uint32_t cod,
                           BTSspVariant_t pairingVariant,
                           uint32_t passKey );
static void _pairingStateChangedCb( BTStatus_t status,
                                    BTBdaddr_t * pRemoteBdAddr,
                                    BTBondState_t state,
                                    BTSecurityLevel_t securityLevel,
                                    BTAuthFailureReason_t reason );
static void _registerBleAdapterCb( BTStatus_t status,
                                   uint8_t adapter_if,
                                   BTUuid_t * pAppUuid );
static void _advStatusCb( BTStatus_t status,
                          uint8_t adapterIf,
                          bool bStart );

static void prvSetAdvDataCb( BTStatus_t status );

static BTStatus_t prvSetAdvData( IotBleAdvertisementParams_t * pAdvParams );

static BTStatus_t prvSetDeviceProperty( BTPropertyType_t type,
                                        const void * pValue,
                                        size_t length );

static const BTCallbacks_t _BTManagerCb =
{
    .pxDeviceStateChangedCb     = _deviceStateChangedCb,
    .pxAdapterPropertiesCb      = _adapterPropertiesCb,
    .pxRemoteDevicePropertiesCb = NULL,
    .pxSspRequestCb             = _sspRequestCb,
    .pxPairingStateChangedCb    = _pairingStateChangedCb,
    .pxDutModeRecvCb            = NULL,
    .pxleTestModeCb             = NULL,
    .pxEnergyInfoCb             = NULL,
    .pxReadRssiCb               = NULL,
    .pxTxPowerCb                = NULL,
    .pxSlaveSecurityRequestCb   = NULL,
};

static const BTBleAdapterCallbacks_t _BTBleAdapterCb =
{
    .pxRegisterBleAdapterCb          = _registerBleAdapterCb,
    .pxScanResultCb                  = NULL,
    .pxBleAdapterPropertiesCb        = NULL,
    .pxBleRemoteDevicePropertiesCb   = NULL,
    .pxOpenCb                        = NULL,
    .pxCloseCb                       = NULL,
    .pxReadRemoteRssiCb              = NULL,
    .pxAdvStatusCb                   = _advStatusCb,
    .pxSetAdvDataCb                  = prvSetAdvDataCb,
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

/*-----------------------------------------------------------*/

void _deviceStateChangedCb( BTState_t state )
{
    ( void ) state;
    IotSemaphore_Post( &_BTInterface.callbackSemaphore );
}

/*-----------------------------------------------------------*/

void _adapterPropertiesCb( BTStatus_t status,
                           uint32_t numProperties,
                           BTProperty_t * pProperties )
{
    ( void ) numProperties;
    ( void ) pProperties;
    _BTInterface.cbStatus = status;
    IotSemaphore_Post( &_BTInterface.callbackSemaphore );
}

/*-----------------------------------------------------------*/

void _sspRequestCb( BTBdaddr_t * pRemoteBdAddr,
                    BTBdname_t * pRemoteBdName,
                    uint32_t cod,
                    BTSspVariant_t pairingVariant,
                    uint32_t passKey )
{
    IotLink_t * pEventListIndex;
    _bleSubscrEventListElement_t * pEventIndex;

    ( void ) pRemoteBdName;
    ( void ) cod;

    if( pairingVariant == eBTsspVariantPasskeyConfirmation )
    {
        /* If confirmation is needed, trigger the hooks on the APP side. */
        IotMutex_Lock( &_BTInterface.threadSafetyMutex );

        /* Get the event associated to the callback */
        IotContainers_ForEach( &_BTInterface.subscrEventListHead[ eBLENumericComparisonCallback ], pEventListIndex )
        {
            pEventIndex = IotLink_Container( _bleSubscrEventListElement_t, pEventListIndex, eventList );
            pEventIndex->subscribedEventCb.pNumericComparisonCb( pRemoteBdAddr, passKey );
        }
        IotMutex_Unlock( &_BTInterface.threadSafetyMutex );
    }
    else
    {
        _BTInterface.pBTInterface->pxSspReply( pRemoteBdAddr, eBTsspVariantConsent, true, 0 );
    }
}

/*-----------------------------------------------------------*/

void _pairingStateChangedCb( BTStatus_t status,
                             BTBdaddr_t * pRemoteBdAddr,
                             BTBondState_t state,
                             BTSecurityLevel_t securityLevel,
                             BTAuthFailureReason_t reason )
{
    IotLink_t * pEventListIndex;
    _bleSubscrEventListElement_t * pEventIndex;

    IotMutex_Lock( &_BTInterface.threadSafetyMutex );
    /* Get the event associated to the callback */
    IotContainers_ForEach( &_BTInterface.subscrEventListHead[ eBLEPairingStateChanged ], pEventListIndex )
    {
        pEventIndex = IotLink_Container( _bleSubscrEventListElement_t, pEventListIndex, eventList );
        pEventIndex->subscribedEventCb.pGAPPairingStateChangedCb( status, pRemoteBdAddr, state, securityLevel, reason );
    }

    IotMutex_Unlock( &_BTInterface.threadSafetyMutex );
}

/*-----------------------------------------------------------*/

void _registerBleAdapterCb( BTStatus_t status,
                            uint8_t adapter_if,
                            BTUuid_t * pAppUuid )
{
    _BTInterface.cbStatus = status;
    _BTInterface.adapterIf = adapter_if;

    ( void ) pAppUuid;

    IotSemaphore_Post( &_BTInterface.callbackSemaphore );
}


/*-----------------------------------------------------------*/

void _advStatusCb( BTStatus_t status,
                   uint8_t adapterIf,
                   bool bStart )
{
    ( void ) adapterIf;

    _BTInterface.cbStatus = status;

    if( bStart == true )
    {
        if( _BTInterface.pStartAdvCb != NULL )
        {
            _BTInterface.pStartAdvCb( status );
        }
    }
    else
    {
        if( _BTInterface.pStopAdvCb != NULL )
        {
            _BTInterface.pStopAdvCb( status );
        }
    }
}
/*-----------------------------------------------------------*/

void _bleStartAdvCb( BTStatus_t status )
{
    IotSemaphore_Post( &_BTInterface.callbackSemaphore );
    _BTInterface.cbStatus = status;
}

/*-----------------------------------------------------------*/


void _bleStopAdvCb( BTStatus_t status )
{
    IotSemaphore_Post( &_BTInterface.callbackSemaphore );
    _BTInterface.cbStatus = status;
}

/*------------------------------------------------------------*/

static void prvSetAdvDataCb( BTStatus_t status )
{
    _BTInterface.cbStatus = status;
    IotSemaphore_Post( &_BTInterface.callbackSemaphore );
}

/*-----------------------------------------------------------*/

BTStatus_t _startGATTServices()
{
    BTStatus_t ret = eBTStatusSuccess;
    bool status = true;

    #if ( IOT_BLE_ENABLE_DEVICE_INFO_SERVICE == 1 )
        status = IotBleDeviceInfo_Init();
    #endif

    #if ( IOT_BLE_ENABLE_DATA_TRANSFER_SERVICE == 1 )
        if( status == true )
        {
            status = IotBleDataTransfer_Init();
        }
    #endif

    if( status == false )
    {
        ret = eBTStatusFail;
    }

    #if ( IOT_BLE_ADD_CUSTOM_SERVICES == 1 )
        IotBle_AddCustomServicesCb();
    #endif

    return ret;
}

/*-----------------------------------------------------------*/

BTStatus_t _stopGATTServices()
{
    BTStatus_t ret = eBTStatusSuccess;
    bool status = true;

    #if ( IOT_BLE_ENABLE_DEVICE_INFO_SERVICE == 1 )
        status = IotBleDeviceInfo_Cleanup();
    #endif

    #if ( IOT_BLE_ENABLE_DATA_TRANSFER_SERVICE == 1 )
        if( status == true )
        {
            status = IotBleDataTransfer_Cleanup();
        }
    #endif

    if( status == false )
    {
        ret = eBTStatusFail;
    }

    return ret;
}

/*---------------------------------------------------------------*/

static BTStatus_t prvSetAdvData( IotBleAdvertisementParams_t * pAdvParams )
{
    BTStatus_t status = eBTStatusSuccess;
    BTGattAdvertismentParams_t pParams = { 0 };
    size_t countService = 0;
    BTUuid_t pServiceUuide[ _BLE_MAX_UUID_PER_ADV_MESSAGE ];

    pParams.ucName = pAdvParams->name;
    pParams.bIncludeTxPower = pAdvParams->includeTxPower;
    pParams.bSetScanRsp = pAdvParams->setScanRsp;

    pParams.ulAppearance = pAdvParams->appearance;
    pParams.ulMaxInterval = pAdvParams->maxInterval;
    pParams.ulMinInterval = pAdvParams->minInterval;

    pParams.usMinAdvInterval = IOT_BLE_ADVERTISING_INTERVAL;
    pParams.usMaxAdvInterval = ( IOT_BLE_ADVERTISING_INTERVAL * 2 );
    pParams.usAdvertisingEventProperties = BTAdvInd;

    pParams.xAddrType = BTAddrTypePublic;
    pParams.ucChannelMap = 0;
    pParams.usTimeout = 0; /* Set to 0 to disable the advertisement duration. */
    pParams.ucPrimaryAdvertisingPhy = 0;
    pParams.ucSecondaryAdvertisingPhy = 0;

    /* If service UUID 1 is not NULL then put it inside the advertisement message */
    if( pAdvParams->pUUID1 != NULL )
    {
        memcpy( &pServiceUuide[ countService++ ], pAdvParams->pUUID1, sizeof( BTUuid_t ) );
    }

    /* If service UUID 2 is not NULL then put it inside the advertisement message */
    if( pAdvParams->pUUID2 != NULL )
    {
        memcpy( &pServiceUuide[ countService++ ], pAdvParams->pUUID2, sizeof( BTUuid_t ) );
    }

    status = _BTInterface.pBTLeAdapterInterface->pxSetAdvData( _BTInterface.adapterIf,
                                                               &pParams,
                                                               pAdvParams->manufacturerLen,
                                                               pAdvParams->pManufacturerData,
                                                               0,
                                                               NULL,
                                                               pServiceUuide,
                                                               countService );

    if( status == eBTStatusSuccess )
    {
        IotSemaphore_Wait( &_BTInterface.callbackSemaphore );
        status = _BTInterface.cbStatus;
    }

    return status;
}

static BTStatus_t prvSetDeviceProperty( BTPropertyType_t type,
                                        const void * pValue,
                                        size_t length )
{
    BTStatus_t status;
    BTProperty_t property =
    {
        .xType = type,
        .xLen  = length,
        .pvVal = ( void * ) pValue
    };

    status = _BTInterface.pBTInterface->pxSetDeviceProperty( &property );

    if( status == eBTStatusSuccess )
    {
        IotSemaphore_Wait( &_BTInterface.callbackSemaphore );
        status = _BTInterface.cbStatus;

        if( status != eBTStatusSuccess )
        {
            IotLogError( "Callback error in stting property type %d , error returned %d.", type, status );
        }
    }
    else
    {
        IotLogError( "Failed to set property type %d, error returned %d.", type, status );
    }

    return status;
}

/*-----------------------------------------------------------*/

BTStatus_t IotBle_StartAdv( IotBle_StartAdvCallback_t pStartAdvCb )
{
    BTStatus_t status = eBTStatusSuccess;

    _BTInterface.pStartAdvCb = pStartAdvCb;
    status = _BTInterface.pBTLeAdapterInterface->pxStartAdv( _BTInterface.adapterIf );

    return status;
}

/*-----------------------------------------------------------*/

BTStatus_t IotBle_StopAdv( IotBle_StopAdvCallback_t pStopAdvCb )
{
    BTStatus_t status = eBTStatusSuccess;

    _BTInterface.pStopAdvCb = pStopAdvCb;
    status = _BTInterface.pBTLeAdapterInterface->pxStopAdv( _BTInterface.adapterIf );

    return status;
}

/*-----------------------------------------------------------*/

BTStatus_t IotBle_ConnParameterUpdateRequest( const BTBdaddr_t * pBdAddr,
                                              IotBleConnectionParam_t * pConnectionParam )
{
    BTStatus_t status = eBTStatusSuccess;

    ( void ) pBdAddr;
    ( void ) pConnectionParam;

    /*pxConnParameterUpdateRequest */
    return status;
}

/*-----------------------------------------------------------*/

BTStatus_t IotBle_On( void )
{
    BTStatus_t status = eBTStatusSuccess;
    uint32_t usMtu = IOT_BLE_PREFERRED_MTU_SIZE;
    BTIOtypes_t xIO = IOT_BLE_INPUT_OUTPUT;
    bool bIsBondable = IOT_BLE_ENABLE_BONDING;
    bool bSecureConnection = IOT_BLE_ENABLE_SECURE_CONNECTION;

    if( !isBLEOn )
    {
        status = _BTInterface.pBTInterface->pxBtManagerInit( &_BTManagerCb );

        if( status == eBTStatusSuccess )
        {
            status = _BTInterface.pBTInterface->pxEnable( 0 );

            if( status == eBTStatusSuccess )
            {
                IotSemaphore_Wait( &_BTInterface.callbackSemaphore );
            }
        }

        if( status == eBTStatusSuccess )
        {
            status = _BTInterface.pBTLeAdapterInterface->pxBleAdapterInit( &_BTBleAdapterCb );
        }

        /* Register application. */
        if( status == eBTStatusSuccess )
        {
            status = _BTInterface.pBTLeAdapterInterface->pxRegisterBleApp( ( BTUuid_t * ) &serverUUID );

            if( status == eBTStatusSuccess )
            {
                IotSemaphore_Wait( &_BTInterface.callbackSemaphore );
                status = _BTInterface.cbStatus;
            }
        }

        if( status == eBTStatusSuccess )
        {
            status = prvSetDeviceProperty( eBTpropertyBdname, &bleDeviceName, strlen( bleDeviceName ) );
        }

        if( status == eBTStatusSuccess )
        {
            status = prvSetDeviceProperty( eBTpropertySecureConnectionOnly, &bSecureConnection, sizeof( bool ) );
        }

        if( status == eBTStatusSuccess )
        {
            status = prvSetDeviceProperty( eBTpropertyBondable, &bIsBondable, sizeof( bool ) );
        }

        if( status == eBTStatusSuccess )
        {
            status = prvSetDeviceProperty( eBTpropertyIO, &xIO, sizeof( BTIOtypes_t ) );
        }

        if( status == eBTStatusSuccess )
        {
            status = prvSetDeviceProperty( eBTpropertyLocalMTUSize, &usMtu, sizeof( uint32_t ) );
        }

        /* Initialize the GATT server. */
        if( status == eBTStatusSuccess )
        {
            if( _BTInterface.pGattServerInterface->pxGattServerInit( &_BTGattServerCb ) == eBTStatusSuccess )
            {
                status = _BTInterface.pGattServerInterface->pxRegisterServer( ( BTUuid_t * ) &serverUUID );

                if( status == eBTStatusSuccess )
                {
                    IotSemaphore_Wait( &_BTInterface.callbackSemaphore );
                    status = _BTInterface.cbStatus;
                }
            }
            else
            {
                status = eBTStatusFail;
                IotLogError( "Cannot initialize GATT interface." );
            }
        }

        /* Start GATT services. */
        if( status == eBTStatusSuccess )
        {
            status = _startGATTServices();
        }

        /* Initialize advertisement and scan response. */
        if( status == eBTStatusSuccess )
        {
            #if ( IOT_BLE_SET_CUSTOM_ADVERTISEMENT_MSG == 1 )
                IotBle_SetCustomAdvCb( &_advParams, &_scanRespParams );
            #endif

            status = prvSetAdvData( &_advParams );

            if( status == eBTStatusSuccess )
            {
                status = prvSetAdvData( &_scanRespParams );
            }
        }

        /* Start advertisement. */
        if( status == eBTStatusSuccess )
        {
            status = IotBle_StartAdv( &_bleStartAdvCb );

            if( status == eBTStatusSuccess )
            {
                IotSemaphore_Wait( &_BTInterface.callbackSemaphore );
                status = _BTInterface.cbStatus;
            }
        }

        if( status == eBTStatusSuccess )
        {
            isBLEOn = true;
        }
    }

    return status;
}

static void _disconnectCallback( BTStatus_t status,
                                 uint16_t connectionID,
                                 bool isConnected,
                                 BTBdaddr_t * pRemoteAddress )
{
    ( void ) connectionID;
    ( void ) pRemoteAddress;

    if( !isConnected )
    {
        _BTInterface.cbStatus = status;
        IotSemaphore_Post( &_BTInterface.callbackSemaphore );
    }
}

/*-----------------------------------------------------------*/

BTStatus_t _disconnectAllConnections( void )
{
    IotBleConnectionInfoListElement_t * pConnInfo;
    IotLink_t * pConnection;
    BTBdaddr_t bdAddr;
    uint16_t connId;
    BTStatus_t status = eBTStatusSuccess;
    IotBleEventsCallbacks_t eventCallback;
    bool registered = false;

    /* Register callback to wait for disconnect completion event from the stack. */
    eventCallback.pConnectionCb = _disconnectCallback;
    status = IotBle_RegisterEventCb( eBLEConnection, eventCallback );

    if( status == eBTStatusSuccess )
    {
        registered = true;
    }

    /* Iterate the list of open connections and send a disconnect for each connections. Wait for the disconnect
     * complete callback from the stack. The callback will ensure the connection is also removed from the list.
     * If there is an error in disconnect, break the loop and return the error status.
     */
    while( ( status == eBTStatusSuccess ) && ( !IotListDouble_IsEmpty( &_BTInterface.connectionListHead ) ) )
    {
        IotMutex_Lock( &_BTInterface.threadSafetyMutex );
        pConnection = IotListDouble_PeekHead( &_BTInterface.connectionListHead );
        pConnInfo = IotLink_Container( IotBleConnectionInfoListElement_t, pConnection, connectionList );
        memcpy( &bdAddr, &pConnInfo->remoteBdAddr, sizeof( BTBdaddr_t ) );
        connId = pConnInfo->connId;
        IotMutex_Unlock( &_BTInterface.threadSafetyMutex );

        status = _BTInterface.pBTLeAdapterInterface->pxDisconnect( _BTInterface.adapterIf,
                                                                   &bdAddr,
                                                                   connId );

        if( status == eBTStatusSuccess )
        {
            /* Block for disconnect complete callback triggered from the stack. */
            IotSemaphore_Wait( &_BTInterface.callbackSemaphore );
            status = _BTInterface.cbStatus;
        }
        else
        {
            IotLogError( "Failed to disconnect BLE connection, status = %d", status );
        }
    }

    if( registered )
    {
        IotBle_UnRegisterEventCb( eBLEConnection, eventCallback );
    }

    return status;
}

BTStatus_t IotBle_Off( void )
{
    BTStatus_t status = eBTStatusSuccess;

    if( isBLEOn )
    {
        /* Stop the advertisement to avoid new connections to the device. */
        status = IotBle_StopAdv( &_bleStopAdvCb );

        if( status == eBTStatusSuccess )
        {
            IotSemaphore_Wait( &_BTInterface.callbackSemaphore );
            status = _BTInterface.cbStatus;
        }

        /* Disconnect open BLE connections. */
        if( status == eBTStatusSuccess )
        {
            status = _disconnectAllConnections();
        }

        /* Stop all GATT services */
        if( status == eBTStatusSuccess )
        {
            status = _stopGATTServices();
        }

        /* Disable BLE stack. */
        if( status == eBTStatusSuccess )
        {
            status = _BTInterface.pGattServerInterface->pxUnregisterServer( _BTInterface.serverIf );

            if( status == eBTStatusSuccess )
            {
                IotSemaphore_Wait( &_BTInterface.callbackSemaphore );
                status = _BTInterface.cbStatus;
            }
        }

        if( status == eBTStatusSuccess )
        {
            status = _BTInterface.pBTLeAdapterInterface->pxUnregisterBleApp( _BTInterface.adapterIf );
        }

        if( status == eBTStatusSuccess )
        {
            status = _BTInterface.pBTInterface->pxDisable();

            if( status == eBTStatusSuccess )
            {
                IotSemaphore_Wait( &_BTInterface.callbackSemaphore );
            }
        }

        /* Cleanup BLE stack. */
        if( status == eBTStatusSuccess )
        {
            status = _BTInterface.pBTInterface->pxBtManagerCleanup();
        }

        if( status == eBTStatusSuccess )
        {
            isBLEOn = false;
        }
    }

    return status;
}

BTStatus_t _createSyncrhonizationObjects( void )
{
    bool createdThreadSafetyMutex = false;
    bool createdWaitCbMutex = false;
    bool createdCallbackSemaphore = false;
    BTStatus_t status = eBTStatusSuccess;

    if( IotMutex_Create( &_BTInterface.threadSafetyMutex, false ) == true )
    {
        createdThreadSafetyMutex = true;
    }
    else
    {
        status = eBTStatusNoMem;
        IotLogError( "Cannot create thread safety mutex." );
    }

    if( status == eBTStatusSuccess )
    {
        if( IotMutex_Create( &_BTInterface.waitCbMutex, false ) == true )
        {
            createdWaitCbMutex = true;
        }
        else
        {
            status = eBTStatusNoMem;
            IotLogError( "Cannot create waitCbMutex mutex." );
        }
    }

    if( status == eBTStatusSuccess )
    {
        if( IotSemaphore_Create( &_BTInterface.callbackSemaphore, 0, 1 ) == true )
        {
            createdCallbackSemaphore = true;
        }
        else
        {
            status = eBTStatusNoMem;
            IotLogError( "Cannot create semaphore." );
        }
    }

    if( status != eBTStatusSuccess )
    {
        /* Clean up memory. */
        if( createdThreadSafetyMutex == true )
        {
            IotMutex_Destroy( &_BTInterface.threadSafetyMutex );
        }

        if( createdWaitCbMutex == true )
        {
            IotMutex_Destroy( &_BTInterface.waitCbMutex );
        }

        if( createdCallbackSemaphore == true )
        {
            IotSemaphore_Destroy( &_BTInterface.callbackSemaphore );
        }
    }

    return status;
}

static void prvInitializeLists( void )
{
    size_t index;

    /* Initialize lists. */
    IotListDouble_Create( &_BTInterface.serviceListHead );
    IotListDouble_Create( &_BTInterface.connectionListHead );

    /* Initialize the event list. */
    for( index = 0; index < eNbEvents; index++ )
    {
        IotListDouble_Create( &_BTInterface.subscrEventListHead[ index ] );
    }
}



/*-----------------------------------------------------------*/

BTStatus_t IotBle_Init( void )
{
    BTStatus_t status = eBTStatusSuccess;
    const char * pDeviceName = IOT_BLE_DEVICE_COMPLETE_LOCAL_NAME;

    prvInitializeLists();

    isBLEOn = false;

    status = _createSyncrhonizationObjects();

    if( status == eBTStatusSuccess )
    {
        _BTInterface.pBTInterface = ( BTInterface_t * ) BTGetBluetoothInterface();

        if( _BTInterface.pBTInterface == NULL )
        {
            status = eBTStatusFail;
        }
    }

    if( status == eBTStatusSuccess )
    {
        _BTInterface.pBTLeAdapterInterface = ( BTBleAdapter_t * ) _BTInterface.pBTInterface->pxGetLeAdapter();

        if( _BTInterface.pBTLeAdapterInterface == NULL )
        {
            status = eBTStatusFail;
        }
    }

    if( status == eBTStatusSuccess )
    {
        _BTInterface.pGattServerInterface = ( BTGattServerInterface_t * ) _BTInterface.pBTLeAdapterInterface->ppvGetGattServerInterface();

        if( _BTInterface.pGattServerInterface == NULL )
        {
            status = eBTStatusFail;
        }
    }

    if( status == eBTStatusSuccess )
    {
        /*
         * If a BLE device name is provided in the config, use it to set the local BLE device name.
         */
        if( ( pDeviceName != NULL ) && ( strcmp( pDeviceName, "" ) != 0 ) )
        {
            status = IotBle_SetDeviceName( pDeviceName, strlen( pDeviceName ) );
        }
    }

    return status;
}

BTStatus_t IotBle_SetDeviceName( const char * pName,
                                 size_t length )
{
    BTStatus_t status;

    if( length > IOT_BLE_DEVICE_LOCAL_NAME_MAX_LENGTH )
    {
        status = eBTStatusNoMem;
    }
    else
    {
        /* Copy the device name to a buffer, so that device name does not change between
         * stack disable-enable cycles.
         */
        strncpy( bleDeviceName, pName, length );
        bleDeviceName[ length ] = '\0';

        if( isBLEOn )
        {
            /* If BLE is running, set the device property and advertisement data to propagate the
             * name change immediately in the advertisement.
             */

            status = prvSetDeviceProperty( eBTpropertyBdname, bleDeviceName, strlen( bleDeviceName ) );

            if( status == eBTStatusSuccess )
            {
                status = prvSetAdvData( &_advParams );
            }

            if( status == eBTStatusSuccess )
            {
                status = prvSetAdvData( &_scanRespParams );
            }
        }
        else
        {
            status = eBTStatusSuccess;
        }
    }

    return status;
}


/*-----------------------------------------------------------*/

BTStatus_t IotBle_ConfirmNumericComparisonKeys( BTBdaddr_t * pBdAddr,
                                                bool keyAccepted )
{
    return _BTInterface.pBTInterface->pxSspReply( pBdAddr,
                                                  eBTsspVariantPasskeyConfirmation,
                                                  keyAccepted,
                                                  0 );
}
/*-----------------------------------------------------------*/

BTStatus_t IotBle_RemoveBond( const BTBdaddr_t * pBdAddr )
{
    return _BTInterface.pBTInterface->pxRemoveBond( pBdAddr );
}

/* @TODO Implement real registration unregistration */
BTStatus_t IotBle_RegisterEventCb( IotBleEvents_t xEvent,
                                   IotBleEventsCallbacks_t xBLEEventsCallbacks )
{
    BTStatus_t status = eBTStatusSuccess;
    _bleSubscrEventListElement_t * pNewEvent;


    /* Check for invalid parameters */
    if( xBLEEventsCallbacks.pvPtr == NULL )
    {
        status = eBTStatusParamInvalid;
    }

    if( status == eBTStatusSuccess )
    {
        pNewEvent = IotBle_Malloc( sizeof( _bleSubscrEventListElement_t ) );

        if( pNewEvent != NULL )
        {
            IotMutex_Lock( &_BTInterface.threadSafetyMutex );
            pNewEvent->subscribedEventCb = xBLEEventsCallbacks;
            IotListDouble_InsertHead( &_BTInterface.subscrEventListHead[ xEvent ],
                                      &pNewEvent->eventList );

            IotMutex_Unlock( &_BTInterface.threadSafetyMutex );
        }
        else
        {
            status = eBTStatusFail;
        }
    }

    return status;
}

/* @TODO Implement real registration unregistration */
BTStatus_t IotBle_UnRegisterEventCb( IotBleEvents_t event,
                                     IotBleEventsCallbacks_t bleEventsCallbacks )
{
    BTStatus_t status = eBTStatusFail;
    _bleSubscrEventListElement_t * pEventIndex;
    IotLink_t * pEventListIndex;

    IotMutex_Lock( &_BTInterface.threadSafetyMutex );

    /* Get the event associated to the callback */
    IotContainers_ForEach( &_BTInterface.subscrEventListHead[ event ], pEventListIndex )
    {
        pEventIndex = IotLink_Container( _bleSubscrEventListElement_t, pEventListIndex, eventList );

        if( bleEventsCallbacks.pvPtr == pEventIndex->subscribedEventCb.pvPtr )
        {
            status = eBTStatusSuccess;
            break;
        }
    }

    /* If the event is found, free the remove it (safely) from the list and free it */
    if( status == eBTStatusSuccess )
    {
        IotListDouble_Remove( &pEventIndex->eventList );
        IotBle_Free( pEventIndex );
    }

    IotMutex_Unlock( &_BTInterface.threadSafetyMutex );

    return status;
}
