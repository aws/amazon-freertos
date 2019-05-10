/*
 * Amazon FreeRTOS
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
#if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 1 )
    #include "iot_ble_wifi_provisioning.h"
#endif
#include "iot_ble_data_transfer.h"

#if ( IOT_BLE_ADVERTISING_UUID_SIZE == 2 )
    #define BT_ADV_UUID_TYPE    eBTuuidType16
#else
    #define BT_ADV_UUID_TYPE    eBTuuidType128
#endif

static const BTUuid_t _advUUID =
{
    .uu.uu128 = IOT_BLE_ADVERTISING_UUID,
    .ucType   = BT_ADV_UUID_TYPE
};
static const BTUuid_t _serverUUID =
{
    .ucType   = eBTuuidType128,
    .uu.uu128 = IOT_BLE_SERVER_UUID
};
static IotBleAdvertisementParams_t _scanRespParams =
{
    .includeTxPower    = true,
    .nameType        = BTGattAdvNameNone,
    .setScanRsp        = true,
    .appearance        = IOT_BLE_ADVERTISING_APPEARANCE,
    .minInterval       = IOT_BLE_ADVERTISING_CONN_INTERVAL_MIN,
    .maxInterval       = IOT_BLE_ADVERTISING_CONN_INTERVAL_MAX,
    .serviceDataLen    =                                0,
    .pServiceData      = NULL,
    .manufacturerLen   =                                0,
    .pManufacturerData = NULL,
    .pUUID1            = NULL,
    .pUUID2            = NULL
};

static IotBleAdvertisementParams_t _advParams =
{
    .includeTxPower    = false,
    .nameType        = BTGattAdvNameShort,
    .setScanRsp        = false,
    .appearance        = IOT_BLE_ADVERTISING_APPEARANCE,
    .minInterval       = 0,
    .maxInterval       = 0,
    .serviceDataLen    =                                0,
    .pServiceData      = NULL,
    .manufacturerLen   =                                0,
    .pManufacturerData = NULL,
    .pUUID1            = ( BTUuid_t * ) &_advUUID,
    .pUUID2            = NULL
};


const uint32_t usMtu = IOT_BLE_PREFERRED_MTU_SIZE;
const BTIOtypes_t xIO = IOT_BLE_INPUT_OUTPUT;
const bool bIsBondable = IOT_BLE_ENABLE_BONDING;
const bool bSecureConnection = IOT_BLE_ENABLE_SECURE_CONNECTION;

const BTProperty_t _deviceProperties[] =
{
    {
        .xType = eBTpropertyBdname,
        .xLen = sizeof( IOT_BLE_DEVICE_COMPLETE_LOCAL_NAME ) - 1,
        .pvVal = ( void * ) IOT_BLE_DEVICE_COMPLETE_LOCAL_NAME
    },
    {
        .xType = eBTpropertyBondable,
        .xLen = 1,
        .pvVal = ( void * ) &bIsBondable
    },
    {
        .xType = eBTpropertySecureConnectionOnly,
        .xLen = 1,
        .pvVal = ( void * ) &bSecureConnection
    },
    {
        .xType = eBTpropertyIO,
        .xLen = 1,
        .pvVal = ( void * ) &xIO
    },
    {
        .xType = eBTpropertyLocalMTUSize,
        .xLen = 1,
        .pvVal = ( void * ) &usMtu
    }
};

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
static void _advStartCb( BTStatus_t status,
                         uint32_t serverIf );
static void _setAdvDataCb( BTStatus_t status );
static void _bondedCb( BTStatus_t status,
                       BTBdaddr_t * pRemoteBdAddr,
                       bool isBonded );

static const BTCallbacks_t _BTManagerCb =
{
    .pxDeviceStateChangedCb     = _deviceStateChangedCb,
    .pxAdapterPropertiesCb      = _adapterPropertiesCb,
    .pxRemoteDevicePropertiesCb = NULL,
    .pxSspRequestCb             = _sspRequestCb,
    .pxPairingStateChangedCb    = _pairingStateChangedCb,
    .pxBondedCb                 = _bondedCb,
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
    .pxAdvStartCb                    = _advStartCb,
    .pxSetAdvDataCb                  = _setAdvDataCb,
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
}

/*-----------------------------------------------------------*/

void _adapterPropertiesCb( BTStatus_t status,
                           uint32_t numProperties,
                           BTProperty_t * pProperties )
{
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
        pEventIndex->subscribedEventCb.pGAPPairingStateChangedCb( status, pRemoteBdAddr, securityLevel, reason );
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

    IotSemaphore_Post( &_BTInterface.callbackSemaphore );
}

/*-----------------------------------------------------------*/

void _advStartCb( BTStatus_t status,
                  uint32_t serverIf )
{
    _BTInterface.cbStatus = status;
    IotSemaphore_Post( &_BTInterface.callbackSemaphore );
}

/*-----------------------------------------------------------*/

void _setAdvDataCb( BTStatus_t status )
{
    _BTInterface.cbStatus = status;
    IotSemaphore_Post( &_BTInterface.callbackSemaphore );
}

/*-----------------------------------------------------------*/

void _bondedCb( BTStatus_t status,
                BTBdaddr_t * pRemoteBdAddr,
                bool isBonded )
{
    IotLink_t * pEventListIndex;
    _bleSubscrEventListElement_t * pEventIndex;

    IotMutex_Lock( &_BTInterface.threadSafetyMutex );
    /* Get the event associated to the callback */
    IotContainers_ForEach( &_BTInterface.subscrEventListHead[ eBLEBonded ], pEventListIndex )
    {
        pEventIndex = IotLink_Container( _bleSubscrEventListElement_t, pEventListIndex, eventList );
        pEventIndex->subscribedEventCb.pBondedCb( status, pRemoteBdAddr, isBonded );
    }

    IotMutex_Unlock( &_BTInterface.threadSafetyMutex );
}


/*-----------------------------------------------------------*/

BTStatus_t _startAllServices()
{
    BTStatus_t status = eBTStatusSuccess;
    BaseType_t error;

    error = IotBleDeviceInfo_Init();

    #if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 1 )
        if( error == pdPASS )
        {
            error = IotBleWifiProv_Init();
        }
    #endif

    if( error == pdPASS )
    {
    	if( IotBleDataTransfer_Init() == false )
	{
	    error = pdFAIL;
	}
    }

    if( error != pdPASS )
    {
        status = eBTStatusFail;
    }

    #if ( IOT_BLE_ADD_CUSTOM_SERVICES == 1 )
        IotBle_AddCustomServicesCb();
    #endif

    return status;
}
/*-----------------------------------------------------------*/

BTStatus_t _setAdvData( IotBleAdvertisementParams_t * pAdvParams )
{
    BTStatus_t status = eBTStatusSuccess;
    BTGattAdvertismentParams_t pParams;
    size_t countService = 0;
    BTUuid_t pServiceUuide[ _BLE_MAX_UUID_PER_ADV_MESSAGE ];

    pParams.ucNameType = pAdvParams->nameType;
    pParams.bIncludeTxPower = pAdvParams->includeTxPower;
    pParams.bSetScanRsp = pAdvParams->setScanRsp;

    pParams.ulAppearance = pAdvParams->appearance;
    pParams.ulMaxInterval = pAdvParams->maxInterval;
    pParams.ulMinInterval = pAdvParams->minInterval;
    pParams.usAdvertisingEventProperties = BTAdvInd;

    pParams.xAddrType = BTAddrTypePublic;
    pParams.ucChannelMap = 0;
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

BTStatus_t IotBle_StopAdv( void )
{
    BTStatus_t status = eBTStatusSuccess;

    status = _BTInterface.pBTLeAdapterInterface->pxStopAdv( _BTInterface.adapterIf );

    return status;
}

/*-----------------------------------------------------------*/

BTStatus_t IotBle_ConnParameterUpdateRequest( const BTBdaddr_t * pBdAddr,
                                              IotBleConnectionParam_t * pConnectionParam )
{
    BTStatus_t status = eBTStatusSuccess;


    /*pxConnParameterUpdateRequest */
    return status;
}

/*-----------------------------------------------------------*/

BTStatus_t IotBle_On( void )
{
    /* Currently Disabled due to a bug with ESP32 : https://github.com/espressif/esp-idf/issues/2070 */

    /* _BTInterface.p_BTInterface->pxEnable(0); */
    return eBTStatusSuccess;
}

/*-----------------------------------------------------------*/

BTStatus_t IotBle_Off( void )
{
    BTStatus_t status = eBTStatusSuccess;
    IotLink_t * pConnectionListHead, * pConnectionListElem;
    IotBleConnectionInfoListElement_t * pConnInfo;
    BTBdaddr_t bdAddr;
    uint16_t connId;

    status = IotBle_GetConnectionInfoList( &pConnectionListHead );

    if( status == eBTStatusSuccess )
    {
        do
        {
            pConnInfo = NULL;
            IotMutex_Lock( &_BTInterface.threadSafetyMutex );
            /* Get the event associated to the callback */
            IotContainers_ForEach( pConnectionListHead, pConnectionListElem )
            {
                pConnInfo = IotLink_Container( IotBleConnectionInfoListElement_t, pConnectionListElem, connectionList );
                memcpy( &bdAddr, &pConnInfo->remoteBdAddr, sizeof( BTBdaddr_t ) );
                connId = pConnInfo->connId;
                break;
            }

            IotMutex_Unlock( &_BTInterface.threadSafetyMutex );

            if( pConnInfo != NULL )
            {
                ( void ) _BTInterface.pBTLeAdapterInterface->pxDisconnect( _BTInterface.adapterIf,
                                                                           &bdAddr,
                                                                           connId );
            }
        } while( pConnInfo != NULL );
    }

    /* Currently Disabled due to a bug with ESP32 : https://github.com/espressif/esp-idf/issues/2070 */

    /* _BTInterface.p_BTInterface->pxDisable(); */
    return status;
}

/*-----------------------------------------------------------*/

BTStatus_t IotBle_Init( void )
{
    BTStatus_t status = eBTStatusSuccess;
    uint16_t index;
    bool createdThreadSafetyMutex = false;
    bool createdWaitCbMutex = false;
    bool createdCallbackSemaphore = false;

    uint32_t nbProperties = sizeof( _deviceProperties ) / sizeof( _deviceProperties[ 0 ] );

    _BTInterface.pBTInterface = ( BTInterface_t * ) BTGetBluetoothInterface();

    if( _BTInterface.pBTInterface != NULL )
    {
        if( IotMutex_Create( &_BTInterface.threadSafetyMutex, false ) == true )
        {
            createdThreadSafetyMutex = true;

            if( IotMutex_Create( &_BTInterface.waitCbMutex, false ) == true )
            {
                createdWaitCbMutex = true;
            }
            else
            {
                status = eBTStatusNoMem;
                configPRINTF( ( "Cannot create mutex.\n" ) );
            }
        }
        else
        {
            status = eBTStatusNoMem;
            configPRINTF( ( "Cannot create mutex.\n" ) );
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
                configPRINTF( ( "Cannot create semaphore.\n" ) );
            }
        }

        status = _BTInterface.pBTInterface->pxBtManagerInit( &_BTManagerCb );
        _BTInterface.pBTLeAdapterInterface = ( BTBleAdapter_t * ) _BTInterface.pBTInterface->pxGetLeAdapter();
    }
    else
    {
        status = eBTStatusParamInvalid;
    }

    if( ( _BTInterface.pBTLeAdapterInterface != NULL ) && ( status == eBTStatusSuccess ) )
    {
        status = _BTInterface.pBTLeAdapterInterface->pxBleAdapterInit( &_BTBleAdapterCb );
    }
    else
    {
        status = eBTStatusFail;
    }

    /* Register application. */
    if( status == eBTStatusSuccess )
    {
        status = _BTInterface.pBTLeAdapterInterface->pxRegisterBleApp( ( BTUuid_t * ) &_serverUUID );

        if( status == eBTStatusSuccess )
        {
            IotSemaphore_Wait( &_BTInterface.callbackSemaphore );
            status = _BTInterface.cbStatus;
        }
    }

    /* Set GAP properties. */
    if( status == eBTStatusSuccess )
    {
        for( index = 0; index < nbProperties; index++ )
        {
            status = _BTInterface.pBTInterface->pxSetDeviceProperty( &_deviceProperties[ index ] );

            if( status == eBTStatusSuccess )
            {
                IotSemaphore_Wait( &_BTInterface.callbackSemaphore );
                status = _BTInterface.cbStatus;

                if( status != eBTStatusSuccess )
                {
                    configPRINTF( ( "Callback error in property %d, error returned %d \n", index, status ) );
                }
            }
            else
            {
                configPRINTF( ( "Unable to set device property %d, error returned %d \n", index, status ) );
                break;
            }
        }
    }

    /* Initialize the GATT server. */
    if( status == eBTStatusSuccess )
    {
        _BTInterface.pGattServerInterface = ( BTGattServerInterface_t * ) _BTInterface.pBTLeAdapterInterface->ppvGetGattServerInterface();

        if( _BTInterface.pGattServerInterface != NULL )
        {
            if( _BTInterface.pGattServerInterface->pxGattServerInit( &_BTGattServerCb ) == eBTStatusSuccess )
            {
                status = _BTInterface.pGattServerInterface->pxRegisterServer( ( BTUuid_t * ) &_serverUUID );

                if( status == eBTStatusSuccess )
                {
                    IotSemaphore_Wait( &_BTInterface.callbackSemaphore );
                    status = _BTInterface.cbStatus;
                }
            }
            else
            {
                status = eBTStatusFail;
                configPRINTF( ( "Cannot initialize GATT interface\n" ) );
            }
        }
        else
        {
            status = eBTStatusFail;
            configPRINTF( ( "Cannot get GATT server interface\n" ) );
        }
    }

    /* Initialize lists. */
    if( status == eBTStatusSuccess )
    {
        IotListDouble_Create( &_BTInterface.serviceListHead );
        IotListDouble_Create( &_BTInterface.connectionListHead );

        /* Initialize the event list. */
        for( index = 0; index < eNbEvents; index++ )
        {
            IotListDouble_Create( &_BTInterface.subscrEventListHead[ index ] );
        }
    }

    /* Start services. */
    if( status == eBTStatusSuccess )
    {
        status = _startAllServices();
    }

    /* Initialize advertisement and scan response. */
    if( status == eBTStatusSuccess )
    {
		#if ( IOT_BLE_SET_CUSTOM_ADVERTISEMENT_MSG == 0 )
        status = _setAdvData( &_advParams );

        if( status == eBTStatusSuccess )
        {
            status = _setAdvData( &_scanRespParams );
        }
		#else
        IotBle_SetCustomAdvCb(&_advParams, &_scanRespParams );
		#endif
    }

    /* Start advertisement. */
    if( status == eBTStatusSuccess )
    {
        IotBle_StartAdv( NULL );
    }

    /* Clean up memory. */
    if( status != eBTStatusSuccess )
    {
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
        pNewEvent = pvPortMalloc( sizeof( _bleSubscrEventListElement_t ) );

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
        vPortFree( pEventIndex );
    }

    IotMutex_Unlock( &_BTInterface.threadSafetyMutex );

    return status;
}
