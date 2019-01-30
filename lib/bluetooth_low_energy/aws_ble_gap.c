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
 * @file aws_BLE.c
 * @brief BLE GAP API.
 */

#include <aws_ble.h>
#include "string.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "semphr.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "aws_ble.h"
#include "aws_ble_internals.h"


/* 2 is the Maximum number of UUID that can be advertised in the same advertisement message */
#define BLE_MAX_UUID_PER_ADV_MESSAGE    2


BLEInterface_t xBTInterface;

static void prvDeviceStateChangedCb( BTState_t xState );
static void prvAdapterPropertiesCb( BTStatus_t xStatus,
                                    uint32_t ulNumProperties,
                                    BTProperty_t * pxProperties );
static void prvSspRequestCb( BTBdaddr_t * pxRemoteBdAddr,
                             BTBdname_t * pxRemoteBdName,
                             uint32_t ulCod,
                             BTSspVariant_t xPairingVariant,
                             uint32_t ulPassKey );
static void prvPairingStateChangedCb( BTStatus_t xStatus,
                                      BTBdaddr_t * pxRemoteBdAddr,
                                      BTBondState_t xState,
                                      BTSecurityLevel_t xSecurityLevel,
				      BTAuthFailureReason_t xReason );
static void prvRegisterBleAdapterCb( BTStatus_t xStatus,
                                     uint8_t ucAdapter_if,
                                     BTUuid_t * pxAppUuid );
static void prvAdvStartCb( BTStatus_t xStatus,
                           uint32_t ulServerIf );
static void prvSetAdvDataCb( BTStatus_t xStatus );
static void prvBondedCb( BTStatus_t xStatus,
                         BTBdaddr_t * pxRemoteBdAddr,
                         bool bIsBonded );

static BTCallbacks_t xBTManagerCb =
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

static BTBleAdapterCallbacks_t xBTBleAdapterCb =
{
    .pxRegisterBleAdapterCb          = prvRegisterBleAdapterCb,
    .pxScanResultCb                  = NULL,
    .pxBleAdapterPropertiesCb        = NULL,
    .pxBleRemoteDevicePropertiesCb   = NULL,
    .pxOpenCb                        = NULL,
    .pxCloseCb                       = NULL,
    .pxReadRemoteRssiCb              = NULL,
    .pxAdvStartCb                    = prvAdvStartCb,
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

void prvDeviceStateChangedCb( BTState_t xState )
{
}

/*-----------------------------------------------------------*/

void prvAdapterPropertiesCb( BTStatus_t xStatus,
                             uint32_t ulNumProperties,
                             BTProperty_t * pxProperties )
{
    xBTInterface.xCbStatus = xStatus;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete, 1 << eBLEHALEventAdapterPropertiesCb );
}

/*-----------------------------------------------------------*/

void prvSspRequestCb( BTBdaddr_t * pxRemoteBdAddr,
                      BTBdname_t * pxRemoteBdName,
                      uint32_t ulCod,
                      BTSspVariant_t xPairingVariant,
                      uint32_t ulPassKey )
{
    Link_t * pxEventListIndex;
    BLESubscrEventListElement_t * pxEventIndex;

    if(xPairingVariant == eBTsspVariantPasskeyConfirmation)
    {
    	/* If confirmation is needed, trigger the hooks on the APP side. */
		if( xSemaphoreTake( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
		{
			/* Get the event associated to the callback */
			listFOR_EACH( pxEventListIndex, &xBTInterface.xSubscrEventListHead[ eBLENumericComparisonCallback ] )
			{
				pxEventIndex = listCONTAINER( pxEventListIndex, BLESubscrEventListElement_t, xEventList );
				pxEventIndex->xSubscribedEventCb.pxNumericComparisonCb( pxRemoteBdAddr, ulPassKey );
			}
			xSemaphoreGive( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex );
		}
    }else
    {
    	xBTInterface.pxBTInterface->pxSspReply( pxRemoteBdAddr, eBTsspVariantConsent, true, 0 );
    }
}

/*-----------------------------------------------------------*/

void prvPairingStateChangedCb( BTStatus_t xStatus,
                               BTBdaddr_t * pxRemoteBdAddr,
                               BTBondState_t xState,
                               BTSecurityLevel_t xSecurityLevel,
			       BTAuthFailureReason_t xReason )
{
    Link_t * pxEventListIndex;
    BLESubscrEventListElement_t * pxEventIndex;

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
        /* Get the event associated to the callback */
        listFOR_EACH( pxEventListIndex, &xBTInterface.xSubscrEventListHead[ eBLEPairingStateChanged ] )
        {
            pxEventIndex = listCONTAINER( pxEventListIndex, BLESubscrEventListElement_t, xEventList );
            pxEventIndex->xSubscribedEventCb.pxGAPPairingStateChangedCb( xStatus, pxRemoteBdAddr, xSecurityLevel, xReason );
        }
        xSemaphoreGive( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex );
    }
}

/*-----------------------------------------------------------*/

void prvRegisterBleAdapterCb( BTStatus_t xStatus,
                              uint8_t ucAdapter_if,
                              BTUuid_t * pxAppUuid )
{
    xBTInterface.xCbStatus = xStatus;
    xBTInterface.ucAdapterIf = ucAdapter_if;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete, 1 << eBLEHALEventRegisterBleAdapterCb );
}

/*-----------------------------------------------------------*/

void prvAdvStartCb( BTStatus_t xStatus,
                    uint32_t ulServerIf )
{
    if( xBTInterface.pxBLEEventsCallbacks.pxStartAdvCb != NULL )
    {
        xBTInterface.pxBLEEventsCallbacks.pxStartAdvCb( xStatus );
    }
}

/*-----------------------------------------------------------*/

void prvSetAdvDataCb( BTStatus_t xStatus )
{
    if( xBTInterface.pxBLEEventsCallbacks.pxSetAdvDataCb != NULL )
    {
        xBTInterface.pxBLEEventsCallbacks.pxSetAdvDataCb( xStatus );
    }
}

/*-----------------------------------------------------------*/

void prvBondedCb( BTStatus_t xStatus,
                  BTBdaddr_t * pxRemoteBdAddr,
                  bool bIsBonded )
{
    Link_t * pxEventListIndex;
    BLESubscrEventListElement_t * pxEventIndex;

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
        /* Get the event associated to the callback */
        listFOR_EACH( pxEventListIndex, &xBTInterface.xSubscrEventListHead[ eBLEBonded ] )
        {
            pxEventIndex = listCONTAINER( pxEventListIndex, BLESubscrEventListElement_t, xEventList );
            pxEventIndex->xSubscribedEventCb.pxBondedCb( xStatus, pxRemoteBdAddr, bIsBonded );
        }
        xSemaphoreGive( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex );
    }
}

/*-----------------------------------------------------------*/

BTStatus_t BLE_StartAdv( BLEStartAdvCallback_t pxStartAdvCb )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    xBTInterface.pxBLEEventsCallbacks.pxStartAdvCb = pxStartAdvCb;
    xStatus = xBTInterface.pxBTLeAdapterInterface->pxStartAdv( xBTInterface.ucAdapterIf );

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t BLE_StopAdv( void )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = xBTInterface.pxBTLeAdapterInterface->pxStopAdv( xBTInterface.ucAdapterIf );

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t BLE_SetAdvData( BTAdvProperties_t usAdvertisingEventProperties,
                           BLEAdvertismentParams_t * pxAdvParams,
                           BLESetAdvDataCallback_t pxSetAdvDataCb )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BTGattAdvertismentParams_t pxParams;
    size_t xCountService = 0;
    BTUuid_t pxServiceUuid[ BLE_MAX_UUID_PER_ADV_MESSAGE ];

    xBTInterface.pxBLEEventsCallbacks.pxSetAdvDataCb = pxSetAdvDataCb;

    pxParams.bIncludeName = pxAdvParams->bIncludeName;
    pxParams.bIncludeTxPower = pxAdvParams->bIncludeTxPower;
    pxParams.bSetScanRsp = pxAdvParams->bSetScanRsp;

    pxParams.ulAppearance = pxAdvParams->ulAppearance;
    pxParams.ulMaxInterval = pxAdvParams->ulMaxInterval;
    pxParams.ulMinInterval = pxAdvParams->ulMinInterval;
    pxParams.usAdvertisingEventProperties = usAdvertisingEventProperties;

    pxParams.xAddrType = BTAddrTypePublic;
    pxParams.ucChannelMap = 0;
    pxParams.ucPrimaryAdvertisingPhy = 0;
    pxParams.ucSecondaryAdvertisingPhy = 0;

    /* If service UUID 1 is not NULL then put it inside the advertisement message */
    if( pxAdvParams->pxUUID1 != NULL )
    {
        memcpy( &pxServiceUuid[ xCountService++ ], pxAdvParams->pxUUID1, sizeof( BTUuid_t ) );
    }

    /* If service UUID 2 is not NULL then put it inside the advertisement message */
    if( pxAdvParams->pxUUID2 != NULL )
    {
        memcpy( &pxServiceUuid[ xCountService++ ], pxAdvParams->pxUUID2, sizeof( BTUuid_t ) );
    }

    xStatus = xBTInterface.pxBTLeAdapterInterface->pxSetAdvData( xBTInterface.ucAdapterIf,
                                                                 &pxParams,
                                                                 pxAdvParams->usManufacturerLen,
                                                                 pxAdvParams->pcManufacturerData,
                                                                 0,
                                                                 NULL,
                                                                 pxServiceUuid,
                                                                 xCountService );


    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t BLE_ConnParameterUpdateRequest( const BTBdaddr_t * pxBdAddr,
                                           BLEConnectionParam_t * pxConnectionParam )
{
    BTStatus_t xStatus = eBTStatusSuccess;


    /*pxConnParameterUpdateRequest */
    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t BLE_ON( void )
{
    /* Currently Disabled due to a bug with ESP32 : https://github.com/espressif/esp-idf/issues/2070 */
    /* xBTInterface.pxBTInterface->pxEnable(0); */
	return eBTStatusSuccess;
}

BTStatus_t BLE_OFF( void )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    Link_t  *pxConnectionListHead,  *pxConnectionListElem, *pxTemp;
    BLEConnectionInfoListElement_t * pxConnInfo;

    xStatus = BLE_GetConnectionInfoList( &pxConnectionListHead );
    if( xStatus == eBTStatusSuccess )
    {
        listFOR_EACH_SAFE( pxConnectionListElem, pxTemp, pxConnectionListHead )
        {
            pxConnInfo = listCONTAINER( pxConnectionListElem, BLEConnectionInfoListElement_t, xConnectionList );
            xStatus = xBTInterface.pxBTLeAdapterInterface->pxDisconnect(
                    xBTInterface.ucAdapterIf,
                    &pxConnInfo->pxRemoteBdAddr,
                    pxConnInfo->usConnId );
            if( xStatus != eBTStatusSuccess )
            {
                break;
            }
        }
    }

    /* Currently Disabled due to a bug with ESP32 : https://github.com/espressif/esp-idf/issues/2070 */
    /* xBTInterface.pxBTInterface->pxDisable(); */
	return xStatus;
}

BTStatus_t BLE_Init( BTUuid_t * pxAppUuid,
                     const BTProperty_t * pxProperty,
                     size_t xNbProperties )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    uint16_t usIndex;

    xBTInterface.pxBTInterface = ( BTInterface_t * ) BTGetBluetoothInterface();

    if( ( xBTInterface.pxBTInterface != NULL ) &&
        ( pxAppUuid != NULL ) )
    {
        ( void ) xSemaphoreCreateMutexStatic( &xBTInterface.xThreadSafetyMutex );
        ( void ) xEventGroupCreateStatic( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete );

        xStatus = xBTInterface.pxBTInterface->pxBtManagerInit( &xBTManagerCb );
        xBTInterface.pxBTLeAdapterInterface = ( BTBleAdapter_t * ) xBTInterface.pxBTInterface->pxGetLeAdapter();
    }
    else
    {
        xStatus = eBTStatusParamInvalid;
    }

    if( ( xBTInterface.pxBTLeAdapterInterface != NULL ) && ( xStatus == eBTStatusSuccess ) )
    {
        xStatus = xBTInterface.pxBTLeAdapterInterface->pxBleAdapterInit( &xBTBleAdapterCb );
    }
    else
    {
        xStatus = eBTStatusFail;
    }

    if( xStatus == eBTStatusSuccess )
    {
        xBTInterface.pxBTLeAdapterInterface->pxRegisterBleApp( pxAppUuid );
        xEventGroupWaitBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete,
                             1 << eBLEHALEventRegisterBleAdapterCb,
							 pdTRUE,
							 pdTRUE,
                             portMAX_DELAY );

        xStatus = xBTInterface.xCbStatus;
    }

    if( xStatus == eBTStatusSuccess )
    {
        for( usIndex = 0; usIndex < xNbProperties; usIndex++ )
        {
            xStatus = xBTInterface.pxBTInterface->pxSetDeviceProperty( &pxProperty[ usIndex ] );

            if( xStatus == eBTStatusSuccess )
            {
                xEventGroupWaitBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete,
                                     1 << eBLEHALEventAdapterPropertiesCb,
									 pdTRUE,
									 pdTRUE,
                                     portMAX_DELAY );

                xStatus = xBTInterface.xCbStatus;
            }
            else
            {
                break;
            }
        }
    }

    /* Initialize the GATT server. */
    if( xStatus == eBTStatusSuccess )
    {
        xBTInterface.pxGattServerInterface = ( BTGattServerInterface_t * ) xBTInterface.pxBTLeAdapterInterface->ppvGetGattServerInterface();

        if( xBTInterface.pxGattServerInterface != NULL )
        {
            if( xBTInterface.pxGattServerInterface->pxGattServerInit( &xBTGattServerCb ) == eBTStatusSuccess )
            {
                xStatus = xBTInterface.pxGattServerInterface->pxRegisterServer( pxAppUuid );
            }
            else
            {
                xStatus = eBTStatusFail;
                configPRINTF( ( "Cannot initialize GATT interface\n" ) );
            }
        }
        else
        {
            xStatus = eBTStatusFail;
            configPRINTF( ( "Cannot get GATT server interface\n" ) );
        }
    }
    else
    {
        configPRINTF( ( "NULL callback or server interface\n" ) );
        xStatus = eBTStatusParamInvalid;
    }

    if( xStatus == eBTStatusSuccess )
    {
        listINIT_HEAD( &xBTInterface.xServiceListHead );
        listINIT_HEAD( &xBTInterface.xConnectionListHead );

        /* Initialize the event list. */
        for( usIndex = 0; usIndex < eNbEvents; usIndex++ )
        {
            listINIT_HEAD( &xBTInterface.xSubscrEventListHead[ usIndex ] );
        }

        xEventGroupWaitBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete,
                             1 << eBLEHALEventServerRegisteredCb,
							 pdTRUE,
							 pdTRUE,
                             portMAX_DELAY );

        xStatus = xBTInterface.xCbStatus;
    }

    return xStatus;
}

BTStatus_t BLE_ConfirmNumericComparisonKeys( BTBdaddr_t * pxBdAddr, bool bKeyAccepted )
{
	return xBTInterface.pxBTInterface->pxSspReply( pxBdAddr,
									       	       eBTsspVariantPasskeyConfirmation,
										           bKeyAccepted,
		                                           0 );
}
/*-----------------------------------------------------------*/

BTStatus_t BLE_RemoveBond( const BTBdaddr_t * pxBdAddr )
{
    return  xBTInterface.pxBTInterface->pxRemoveBond( pxBdAddr );;
}

/* @TODO Implement real registration unregistration */
BTStatus_t BLE_RegisterEventCb( BLEEvents_t xEvent,
                                BLEEventsCallbacks_t xBLEEventsCallbacks )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLESubscrEventListElement_t * pxNewEvent;


    /* Check for invalid parameters */
    if( xBLEEventsCallbacks.pvPtr == NULL )
    {
        xStatus = eBTStatusParamInvalid;
    }

    if( xStatus == eBTStatusSuccess )
    {
        pxNewEvent = pvPortMalloc( sizeof( BLESubscrEventListElement_t ) );

        if( pxNewEvent != NULL )
        {
            if( xSemaphoreTake( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
            {
                pxNewEvent->xSubscribedEventCb = xBLEEventsCallbacks;
                listADD( &xBTInterface.xSubscrEventListHead[ xEvent ],
                         &pxNewEvent->xEventList );

                xSemaphoreGive( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex );
            }
        }
        else
        {
            xStatus = eBTStatusFail;
        }
    }

    return xStatus;
}

/* @TODO Implement real registration unregistration */
BTStatus_t BLE_UnRegisterEventCb( BLEEvents_t xEvent,
                                  BLEEventsCallbacks_t xBLEEventsCallbacks )
{
    BTStatus_t xStatus = eBTStatusFail;
    BLESubscrEventListElement_t * pxEventIndex;
    Link_t * pxEventListIndex;

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
        /* Get the event associated to the callback */
        listFOR_EACH( pxEventListIndex, &xBTInterface.xSubscrEventListHead[ xEvent ] )
        {
            pxEventIndex = listCONTAINER( pxEventListIndex, BLESubscrEventListElement_t, xEventList );

            if( xBLEEventsCallbacks.pvPtr == pxEventIndex->xSubscribedEventCb.pvPtr )
            {
                xStatus = eBTStatusSuccess;
                break;
            }
        }

        /* If the event is found, free the remove it (safely) from the list and free it */
        if( xStatus == eBTStatusSuccess )
        {
            listREMOVE( &pxEventIndex->xEventList );
            vPortFree( pxEventIndex );
        }

        xSemaphoreGive( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex );
    }

    return xStatus;
}
