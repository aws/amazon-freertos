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

#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

#include "iot_ble.h"
#include "string.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "semphr.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "iot_ble.h"
#include "iot_ble_internal.h"


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

static BTCallbacks_t xBTManagerCb =
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

static BTBleAdapterCallbacks_t xBTBleAdapterCb =
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
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &_BTInterface.waitOperationComplete, 1 << eBLEHALEventAdapterPropertiesCb );
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
        if( xSemaphoreTake( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex, portMAX_DELAY ) == pdPASS )
        {
            /* Get the event associated to the callback */
            IotContainers_ForEach( &_BTInterface.subscrEventListHead[ eBLENumericComparisonCallback ], pEventListIndex )
            {
                pEventIndex = IotLink_Container( _bleSubscrEventListElement_t, pEventListIndex, eventList );
                pEventIndex->subscribedEventCb.pNumericComparisonCb( pRemoteBdAddr, passKey );
            }
            xSemaphoreGive( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex );
        }
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

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
        /* Get the event associated to the callback */
        IotContainers_ForEach( &_BTInterface.subscrEventListHead[ eBLEPairingStateChanged ], pEventListIndex )
        {
            pEventIndex = IotLink_Container( _bleSubscrEventListElement_t, pEventListIndex, eventList );
            pEventIndex->subscribedEventCb.pGAPPairingStateChangedCb( status, pRemoteBdAddr, securityLevel, reason );
        }

        xSemaphoreGive( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex );
    }
}

/*-----------------------------------------------------------*/

void _registerBleAdapterCb( BTStatus_t status,
                              uint8_t adapter_if,
                              BTUuid_t * pAppUuid )
{
    _BTInterface.cbStatus = status;
    _BTInterface.adapterIf = adapter_if;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &_BTInterface.waitOperationComplete, 1 << eBLEHALEventRegisterBleAdapterCb );
}

/*-----------------------------------------------------------*/

void _advStartCb( BTStatus_t status,
                    uint32_t serverIf )
{
    if( _BTInterface.pBLEEventsCallbacks.pStartAdvCb != NULL )
    {
        _BTInterface.pBLEEventsCallbacks.pStartAdvCb( status );
    }
}

/*-----------------------------------------------------------*/

void _setAdvDataCb( BTStatus_t status )
{
    if( _BTInterface.pBLEEventsCallbacks.pSetAdvDataCb != NULL )
    {
        _BTInterface.pBLEEventsCallbacks.pSetAdvDataCb( status );
    }
}

/*-----------------------------------------------------------*/

void _bondedCb( BTStatus_t status,
                  BTBdaddr_t * pRemoteBdAddr,
                  bool isBonded )
{
    IotLink_t * pEventListIndex;
    _bleSubscrEventListElement_t * pEventIndex;

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
        /* Get the event associated to the callback */
        IotContainers_ForEach( &_BTInterface.subscrEventListHead[ eBLEBonded ], pEventListIndex )
        {
            pEventIndex = IotLink_Container( _bleSubscrEventListElement_t, pEventListIndex, eventList );
            pEventIndex->subscribedEventCb.pBondedCb( status, pRemoteBdAddr, isBonded );
        }

        xSemaphoreGive( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex );
    }
}

/*-----------------------------------------------------------*/

BTStatus_t IotBle_StartAdv( IotBle_StartAdvCallback_t pStartAdvCb )
{
    BTStatus_t status = eBTStatusSuccess;

    _BTInterface.pBLEEventsCallbacks.pStartAdvCb = pStartAdvCb;
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

BTStatus_t IotBle_SetAdvData( BTAdvProperties_t advertisingEventProperties,
                           IotBleAdvertismentParams_t * pAdvParams,
                           IotBle_SetAdvDataCallback_t pSetAdvDataCb )
{
    BTStatus_t status = eBTStatusSuccess;
    BTGattAdvertismentParams_t pParams;
    size_t countService = 0;
    BTUuid_t pServiceUuide[ _BLE_MAX_UUID_PER_ADV_MESSAGE ];

    _BTInterface.pBLEEventsCallbacks.pSetAdvDataCb = pSetAdvDataCb;

    pParams.bIncludeName = pAdvParams->includeName;
    pParams.bIncludeTxPower = pAdvParams->includeTxPower;
    pParams.bSetScanRsp = pAdvParams->setScanRsp;

    pParams.ulAppearance = pAdvParams->appearance;
    pParams.ulMaxInterval = pAdvParams->maxInterval;
    pParams.ulMinInterval = pAdvParams->minInterval;
    pParams.usAdvertisingEventProperties = advertisingEventProperties;

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
    IotLink_t * pTempLink;
    IotBleConnectionInfoListElement_t * pConnInfo;

    status = IotBle_GetConnectionInfoList( &pConnectionListHead );

    if( status == eBTStatusSuccess )
    {
        /* Get the event associated to the callback */
    	IotContainers_ForEachSafe( pConnectionListHead, pConnectionListElem, pTempLink )
        {
            pConnInfo = listCONTAINER( pConnectionListElem, IotBleConnectionInfoListElement_t, connectionList );
            status = _BTInterface.pBTLeAdapterInterface->pxDisconnect(
                _BTInterface.adapterIf,
                &pConnInfo->remoteBdAddr,
                pConnInfo->connId );

            if( status != eBTStatusSuccess )
            {
                break;
            }
        }
    }

    /* Currently Disabled due to a bug with ESP32 : https://github.com/espressif/esp-idf/issues/2070 */

    /* _BTInterface.p_BTInterface->pxDisable(); */
    return status;
}

/*-----------------------------------------------------------*/

BTStatus_t IotBle_Init( BTUuid_t * pAppUuid,
                     const BTProperty_t * pProperty,
                     size_t nbProperties )
{
    BTStatus_t status = eBTStatusSuccess;
    uint16_t index;

    _BTInterface.pBTInterface = ( BTInterface_t * ) BTGetBluetoothInterface();

    if( ( _BTInterface.pBTInterface != NULL ) &&
        ( pAppUuid != NULL ) )
    {
        ( void ) xSemaphoreCreateMutexStatic( &_BTInterface.threadSafetyMutex );
        ( void ) xEventGroupCreateStatic( ( EventGroupHandle_t ) &_BTInterface.waitOperationComplete );

        status = _BTInterface.pBTInterface->pxBtManagerInit( &xBTManagerCb );
        _BTInterface.pBTLeAdapterInterface = ( BTBleAdapter_t * ) _BTInterface.pBTInterface->pxGetLeAdapter();
    }
    else
    {
        status = eBTStatusParamInvalid;
    }

    if( ( _BTInterface.pBTLeAdapterInterface != NULL ) && ( status == eBTStatusSuccess ) )
    {
        status = _BTInterface.pBTLeAdapterInterface->pxBleAdapterInit( &xBTBleAdapterCb );
    }
    else
    {
        status = eBTStatusFail;
    }

    if( status == eBTStatusSuccess )
    {
        _BTInterface.pBTLeAdapterInterface->pxRegisterBleApp( pAppUuid );
        xEventGroupWaitBits( ( EventGroupHandle_t ) &_BTInterface.waitOperationComplete,
                             1 << eBLEHALEventRegisterBleAdapterCb,
                             pdTRUE,
                             pdTRUE,
                             portMAX_DELAY );

        status = _BTInterface.cbStatus;
    }

    if( status == eBTStatusSuccess )
    {
        for( index = 0; index < nbProperties; index++ )
        {
            status = _BTInterface.pBTInterface->pxSetDeviceProperty( &pProperty[ index ] );

            if( status == eBTStatusSuccess )
            {
                xEventGroupWaitBits( ( EventGroupHandle_t ) &_BTInterface.waitOperationComplete,
                                     1 << eBLEHALEventAdapterPropertiesCb,
                                     pdTRUE,
                                     pdTRUE,
                                     portMAX_DELAY );

                status = _BTInterface.cbStatus;
            }
            else
            {
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
                status = _BTInterface.pGattServerInterface->pxRegisterServer( pAppUuid );
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
    else
    {
        configPRINTF( ( "NULL callback or server interface\n" ) );
        status = eBTStatusParamInvalid;
    }

    if( status == eBTStatusSuccess )
    {
        IotListDouble_Create( &_BTInterface.serviceListHead );
        IotListDouble_Create( &_BTInterface.connectionListHead );

        /* Initialize the event list. */
        for( index = 0; index < eNbEvents; index++ )
        {
            IotListDouble_Create( &_BTInterface.subscrEventListHead[ index ] );
        }

        xEventGroupWaitBits( ( EventGroupHandle_t ) &_BTInterface.waitOperationComplete,
                             1 << eBLEHALEventServerRegisteredCb,
                             pdTRUE,
                             pdTRUE,
                             portMAX_DELAY );

        status = _BTInterface.cbStatus;
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
            if( xSemaphoreTake( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex, portMAX_DELAY ) == pdPASS )
            {
                pNewEvent->subscribedEventCb = xBLEEventsCallbacks;
                IotListDouble_InsertHead( &_BTInterface.subscrEventListHead[ xEvent ],
                                          &pNewEvent->eventList );

                xSemaphoreGive( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex );
            }
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

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
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

        xSemaphoreGive( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex );
    }

    return status;
}
