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
 * @file iot_ble_gatt.c
 * @brief BLE GATT API.
 */

#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

#include <string.h>
#include "FreeRTOS.h"
#include "event_groups.h"
#include "semphr.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "iot_ble.h"
#include "iot_ble_internal.h"


static BTStatus_t _createAttributes( BTService_t * pService );
static size_t _computeNumberOfHandles( BTService_t * pService );
static void _serviceClean( BLEServiceListElement_t * pServiceElem );
static BLEServiceListElement_t * _getServiceListElemFromHandle( uint16_t handle );
static BaseType_t _getCallbackFromHandle( uint16_t attrHandle,
                                     IotBleAttributeEventCallback_t * pEventsCallbacks );
static BLEServiceListElement_t * _getLastAddedServiceElem( void );
static void _attributeAdded( uint16_t handle,
                        BTStatus_t status );
static BTStatus_t _addServiceToList( BTService_t * pService,
        IotBleAttributeEventCallback_t pEventsCallbacks[] );
static size_t _computeNumberOfHandles( BTService_t * pService );


static void _serverRegisteredCb( BTStatus_t status,
                                 uint8_t serverIf,
                                 BTUuid_t * pAppUuid );
static void _serverUnregisteredCb( BTStatus_t status,
                                   uint8_t serverIf );
static void _connectionCb( uint16_t connId,
                           uint8_t serverIf,
                           bool connected,
                           BTBdaddr_t * pBda );
static void _seviceAddedCb( BTStatus_t status,
                            uint8_t serverIf,
                            BTGattSrvcId_t * pSrvcId,
                            uint16_t srvcHandle );
static void _charAddedCb( BTStatus_t status,
                          uint8_t serverIf,
                          BTUuid_t * pUuid,
                          uint16_t srvcHandle,
                          uint16_t handle );
static void _charDescrAddedCb( BTStatus_t status,
                               uint8_t serverIf,
                               BTUuid_t * pUuid,
                               uint16_t srvcHandle,
                               uint16_t handle );
static void _serviceStartedCb( BTStatus_t status,
                               uint8_t serverIf,
                               uint16_t srvcHandle );
static void _serviceStoppedCb( BTStatus_t status,
                               uint8_t serverIf,
                               uint16_t srvcHandle );
static void _serviceDeletedCb( BTStatus_t status,
                               uint8_t serverIf,
                               uint16_t srvcHandle );
static void _includedServiceAdded( BTStatus_t status,
                                   uint8_t serverIf,
                                   uint16_t srvcHandle,
                                   uint16_t inclSrvcHandle );
static void _requestReadCb( uint16_t connId,
                            uint32_t transId,
                            BTBdaddr_t * pBda,
                            uint16_t attrHandle,
                            uint16_t offset );
static void _requestWriteCb( uint16_t connId,
                             uint32_t transId,
                             BTBdaddr_t * pBda,
                             uint16_t attrHandle,
                             uint16_t offset,
                             size_t length,
                             bool needRsp,
                             bool isPrep,
                             uint8_t * pValue );
static void _execWriteCb( uint16_t connId,
                          uint32_t transId,
                          BTBdaddr_t * pBda,
                          bool execWrite );
static void _mtuChangedCb( uint16_t connId,
                           uint16_t mtu );
static void _responseConfirmationCb( BTStatus_t status,
                                     uint16_t handle );
static void _indicationSentCb( uint16_t connId,
                               BTStatus_t status );

BTGattServerCallbacks_t _BTGattServerCb =
{
    .pxRegisterServerCb       = _serverRegisteredCb,
    .pxUnregisterServerCb     = _serverUnregisteredCb,
    .pxConnectionCb           = _connectionCb,
    .pxServiceAddedCb         = _seviceAddedCb,
    .pxIncludedServiceAddedCb = _includedServiceAdded,
    .pxCharacteristicAddedCb  = _charAddedCb,
    .pxSetValCallbackCb       = NULL,
    .pxDescriptorAddedCb      = _charDescrAddedCb,
    .pxServiceStartedCb       = _serviceStartedCb,
    .pxServiceStoppedCb       = _serviceStoppedCb,
    .pxServiceDeletedCb       = _serviceDeletedCb,
    .pxRequestReadCb          = _requestReadCb,
    .pxRequestWriteCb         = _requestWriteCb,
    .pxRequestExecWriteCb     = _execWriteCb,
    .pxResponseConfirmationCb = _responseConfirmationCb,
    .pxIndicationSentCb       = _indicationSentCb,
    .pxCongestionCb           = NULL,
    .pxMtuChangedCb           = _mtuChangedCb
};


/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

void _serviceClean( BLEServiceListElement_t * pServiceElem )
{
    if( pServiceElem != NULL )
    {
        IotListDouble_Remove( &pServiceElem->serviceList );
        vPortFree( pServiceElem );
    }
}

/*-----------------------------------------------------------*/

BLEServiceListElement_t * _getServiceListElemFromHandle( uint16_t handle )
{
    IotLink_t * pTmpElem;
    BLEServiceListElement_t * pServiceElem = NULL;

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
        /* Remove service from service list */
       // IotContainers_ForEach( &_BTInterface.xServiceListHead, pxTmpElem )
		 for( ( pTmpElem ) = _BTInterface.serviceListHead.pNext; ( pTmpElem ) != ( &_BTInterface.serviceListHead ); ( pTmpElem ) = ( pTmpElem )->pNext )
        {
            pServiceElem = IotLink_Container( BLEServiceListElement_t, pTmpElem, serviceList );

            if( ( pServiceElem->pService->pusHandlesBuffer[ 0 ] <= handle ) &&
                ( handle <= pServiceElem->endHandle ) )
            {
                break;
            }
        }

        xSemaphoreGive( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex );
    }

    return pServiceElem;
}

/*-----------------------------------------------------------*/

BaseType_t _getCallbackFromHandle( uint16_t attrHandle,
                                     IotBleAttributeEventCallback_t * pEventsCallbacks )
{
    BLEServiceListElement_t * pServiceElem;
    BaseType_t foundService = pdFAIL;
    size_t attributeIndex;

    /* The service that was just added is the last in the list */
    pServiceElem = _getServiceListElemFromHandle( attrHandle );

    if( pServiceElem != NULL )
    {
        for( attributeIndex = 0; attributeIndex < pServiceElem->pService->xNumberOfAttributes; attributeIndex++ )
        {
            if( pServiceElem->pService->pusHandlesBuffer[ attributeIndex ] == attrHandle )
            {
                *pEventsCallbacks = pServiceElem->pEventsCallbacks[ attributeIndex ];
                foundService = pdPASS;
                break;
            }
        }
    }

    return foundService;
}

/*-----------------------------------------------------------*/

void _serverRegisteredCb( BTStatus_t status,
                          uint8_t serverIf,
                          BTUuid_t * pAppUuid )
{
    _BTInterface.serverIf = serverIf;
    _BTInterface.cbStatus = status;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &_BTInterface.waitOperationComplete, 1 << eBLEHALEventServerRegisteredCb );
}

/*-----------------------------------------------------------*/

void _serverUnregisteredCb( BTStatus_t status,
                            uint8_t serverIf )
{
    _BTInterface.serverIf = serverIf;
    _BTInterface.cbStatus = status;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &_BTInterface.waitOperationComplete, 1 << eBLEHALEventServerRegisteredCb );
}

/*-----------------------------------------------------------*/

void _connectionCb( uint16_t connId,
                    uint8_t serverIf,
                    bool connected,
                    BTBdaddr_t * pBda )
{
    BTStatus_t status = eBTStatusSuccess;
    IotBleConnectionInfoListElement_t * pConnInfoListElem;
    IotLink_t * pEventListIndex;
    _bleSubscrEventListElement_t * pEventIndex;

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
        if( connected == true )
        {
            /* Add a new connection to the list */
            pConnInfoListElem = pvPortMalloc( sizeof( IotBleConnectionInfoListElement_t ) );

            if( pConnInfoListElem != NULL )
            {
                /* Initialize the new connection element */
                listINIT_HEAD( &pConnInfoListElem->clientCharDescrListHead );
                memcpy( &pConnInfoListElem->remoteBdAddr, pBda, sizeof( BTBdaddr_t ) );
                pConnInfoListElem->connId = connId;

                IotListDouble_InsertHead( &_BTInterface.connectionListHead, &pConnInfoListElem->connectionList );
            }
            else
            {
                status = eBTStatusNoMem;
            }
        }
        else
        {
            status = IotBle_GetConnectionInfo( connId, &pConnInfoListElem );

            if( status == eBTStatusSuccess )
            {
                /* Remove connection from the list safely */
                IotListDouble_Remove( &pConnInfoListElem->connectionList );
                vPortFree( pConnInfoListElem );
            }
        }

        /* Get the event associated to the callback */
        IotContainers_ForEach( &_BTInterface.subscrEventListHead[ eBLEConnection ], pEventListIndex )
        {
            pEventIndex = IotLink_Container( _bleSubscrEventListElement_t, pEventListIndex, eventList );
            pEventIndex->subscribedEventCb.pConnectionCb( status, connId, connected, pBda );
        }

        ( void ) xSemaphoreGive( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex );
    }
}

/*-----------------------------------------------------------*/

BLEServiceListElement_t * _getLastAddedServiceElem( void )
{
    BLEServiceListElement_t * pServiceElem = NULL;

    /* The service that was just added is the first in the list */
    if( !IotListDouble_IsEmpty( &_BTInterface.serviceListHead ) )

    {
        pServiceElem = IotLink_Container( BLEServiceListElement_t, _BTInterface.serviceListHead.pNext, serviceList );
    }

    return pServiceElem;
}

/*-----------------------------------------------------------*/
void _attributeAdded( uint16_t handle,
                        BTStatus_t status )
{
    BLEServiceListElement_t * pServiceElem = _getLastAddedServiceElem();
    uint16_t index;

    /* Now that service is found, add the handle */
    if( pServiceElem != NULL )
    {
        for( index = 0; index < pServiceElem->pService->xNumberOfAttributes; index++ )
        {
            if( pServiceElem->pService->pusHandlesBuffer[ index ] == 0 )
            {
                pServiceElem->pService->pusHandlesBuffer[ index ] = handle;
                break;
            }
        }

        pServiceElem->endHandle = handle;
    }

    _BTInterface.cbStatus = status;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &_BTInterface.waitOperationComplete, 1 << eBLEHALEventAttributeAddedCb );
}

/*-----------------------------------------------------------*/

void _seviceAddedCb( BTStatus_t status,
                     uint8_t serverIf,
                     BTGattSrvcId_t * pSrvcId,
                     uint16_t srvcHandle )
{
    _attributeAdded( srvcHandle, status );
}

/*-----------------------------------------------------------*/

void _charAddedCb( BTStatus_t status,
                   uint8_t serverIf,
                   BTUuid_t * pUuid,
                   uint16_t srvcHandle,
                   uint16_t handle )
{
    _attributeAdded( handle, status );
}

/*-----------------------------------------------------------*/

void _charDescrAddedCb( BTStatus_t status,
                        uint8_t serverIf,
                        BTUuid_t * pUuid,
                        uint16_t srvcHandle,
                        uint16_t handle )
{
    _attributeAdded( handle, status );
}


/*-----------------------------------------------------------*/

void _includedServiceAdded( BTStatus_t status,
                            uint8_t serverIf,
                            uint16_t srvcHandle,
                            uint16_t inclSrvcHandle )
{
    _attributeAdded( inclSrvcHandle, status );
}

/*-----------------------------------------------------------*/

void _serviceStartedCb( BTStatus_t status,
                        uint8_t serverIf,
                        uint16_t srvcHandle )
{
    _BTInterface.cbStatus = status;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &_BTInterface.waitOperationComplete, 1 << eBLEHALEventServiceStartedCb );
}

/*-----------------------------------------------------------*/

void _serviceStoppedCb( BTStatus_t status,
                        uint8_t serverIf,
                        uint16_t srvcHandle )
{
    _BTInterface.cbStatus = status;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &_BTInterface.waitOperationComplete, 1 << eBLEHALEventServiceStoppedCb );
}

/*-----------------------------------------------------------*/

void _serviceDeletedCb( BTStatus_t status,
                        uint8_t serverIf,
                        uint16_t srvcHandle )
{
    BLEServiceListElement_t * pServiceElem;

    /* The service has been stopped so it can be deleted safely */
    pServiceElem = _getServiceListElemFromHandle( srvcHandle );

    if( pServiceElem != NULL )
    {
        if( xSemaphoreTake( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex, portMAX_DELAY ) == pdPASS )
        {
            _serviceClean( pServiceElem );
            xSemaphoreGive( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex );
        }
        else
        {
            status = eBTStatusFail;
        }
    }
    else
    {
        status = eBTStatusFail;
    }

    _BTInterface.cbStatus = status;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &_BTInterface.waitOperationComplete, 1 << eBLEHALEventServiceDeletedCb );
}

/*-----------------------------------------------------------*/

void _requestReadCb( uint16_t connId,
                     uint32_t transId,
                     BTBdaddr_t * pBda,
                     uint16_t attrHandle,
                     uint16_t offset )
{
    IotBleAttributeEventCallback_t eventsCallbacks;
    IotBleReadEventParams_t readParam;
    IotBleAttributeEvent_t eventParam;

    if( _getCallbackFromHandle( attrHandle, &eventsCallbacks ) == pdPASS )
    {
        readParam.attrHandle = attrHandle;
        readParam.pRemoteBdAddr = pBda;
        readParam.transId = transId;
        readParam.connId = connId;
        readParam.offset = offset;

        eventParam.xEventType = eBLERead;
        eventParam.pParamRead = &readParam;

        eventsCallbacks( &eventParam );
    }
}

/*-----------------------------------------------------------*/

void _requestWriteCb( uint16_t connId,
                      uint32_t transId,
                      BTBdaddr_t * pBda,
                      uint16_t attrHandle,
                      uint16_t offset,
                      size_t length,
                      bool needRsp,
                      bool isPrep,
                      uint8_t * pValue )
{
    IotBleWriteEventParams_t writeParam;
    IotBleAttributeEvent_t eventParam;
    IotBleAttributeEventCallback_t eventsCallbacks;

    if( _getCallbackFromHandle( attrHandle, &eventsCallbacks ) == pdPASS )
    {
        if( isPrep == true )
        {
            _BTInterface.handlePendingPrepareWrite = attrHandle;
        }

        if( needRsp == true )
        {
            eventParam.xEventType = eBLEWrite;
        }
        else
        {
            eventParam.xEventType = eBLEWriteNoResponse;
        }

        writeParam.attrHandle = attrHandle;
        writeParam.isPrep = isPrep;
        writeParam.pValue = pValue;
        writeParam.pRemoteBdAddr = pBda;
        writeParam.transId = transId;
        writeParam.connId = connId;
        writeParam.offset = offset;
        writeParam.length = length;

        eventParam.pParamWrite = &writeParam;

        eventsCallbacks( &eventParam );
    }
}

/*-----------------------------------------------------------*/

void _execWriteCb( uint16_t connId,
                   uint32_t transId,
                   BTBdaddr_t * pBda,
                   bool execWrite )
{
    IotBleExecWriteEventParams_t execWriteParam;
    IotBleAttributeEvent_t eventParam;
    IotBleAttributeEventCallback_t eventsCallbacks;

    if( _getCallbackFromHandle( _BTInterface.handlePendingPrepareWrite, &eventsCallbacks ) == pdPASS )
    {
        execWriteParam.pRemoteBdAddr = pBda;
        execWriteParam.transId = transId;
        execWriteParam.connId = connId;

        eventParam.xEventType = eBLEExecWrite;
        eventParam.pParamExecWrite = &execWriteParam;

        eventsCallbacks( &eventParam );
    }
}

/*-----------------------------------------------------------*/

void _mtuChangedCb( uint16_t connId,
                    uint16_t mtu )
{
    IotLink_t * pEventListIndex;
    _bleSubscrEventListElement_t * pEventIndex;

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
        /* Get the event associated to the callback */
        IotContainers_ForEach( &_BTInterface.subscrEventListHead[ eBLEMtuChanged ], pEventListIndex )
        {
            pEventIndex = IotLink_Container( _bleSubscrEventListElement_t, pEventListIndex, eventList );
            pEventIndex->subscribedEventCb.pMtuChangedCb( connId, mtu );
        }

        xSemaphoreGive( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex );
    }
}

/*-----------------------------------------------------------*/

static void _responseConfirmationCb( BTStatus_t status,
                                     uint16_t handle )
{
    IotBleRespConfirmEventParams_t respConfirmParam;
    IotBleAttributeEvent_t eventParam;
    IotBleAttributeEventCallback_t eventsCallbacks;

    if( _getCallbackFromHandle( handle, &eventsCallbacks ) == pdPASS )
    {
        respConfirmParam.handle = handle;
        respConfirmParam.status = status;

        eventParam.xEventType = eBLEResponseConfirmation;
        eventParam.pParamRespConfim = &respConfirmParam;

        eventsCallbacks( &eventParam );
    }
}

/*-----------------------------------------------------------*/

static void _indicationSentCb( uint16_t connId,
                               BTStatus_t status )
{
    IotBleIndicationSentEventParams_t indicationSentParam;
    IotBleAttributeEvent_t eventParam;
    IotBleAttributeEventCallback_t eventsCallbacks;

    if( _getCallbackFromHandle( _BTInterface.handlePendingIndicationResponse, &eventsCallbacks ) == pdPASS )
    {
        indicationSentParam.connId = connId;
        indicationSentParam.status = status;

        eventParam.pParamIndicationSent = &indicationSentParam;
        eventParam.xEventType = eBLEIndicationConfirmReceived;

        eventsCallbacks( &eventParam );
    }
}

/*-----------------------------------------------------------*/
BTStatus_t _addServiceToList( BTService_t * pService,
                                IotBleAttributeEventCallback_t pEventsCallbacks[] )
{
    BTStatus_t status = eBTStatusSuccess;
    BLEServiceListElement_t * pNewElem;

    /* Create a space in the list for the service. */
    pNewElem = pvPortMalloc( sizeof( BLEServiceListElement_t ) );

    if( pNewElem != NULL )
    {
    	memset( pNewElem, 0, sizeof( BLEServiceListElement_t ) );
        pNewElem->pEventsCallbacks = pEventsCallbacks;
        pNewElem->pService = pService;

        if( xSemaphoreTake( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex, portMAX_DELAY ) == pdPASS )
        {
            IotListDouble_InsertHead( &_BTInterface.serviceListHead, &pNewElem->serviceList );
            xSemaphoreGive( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex );
        }
    }
    else
    {
        status = eBTStatusNoMem;
    }

    return status;
}

/*-----------------------------------------------------------*/
size_t _computeNumberOfHandles( BTService_t * pService )
{
	size_t index;
	size_t nbHandles = 0;

	for(index = 0; index < pService->xNumberOfAttributes; index++ )
	{
		/* Increment by 2 to account for characteristic declaration */
		if(pService->pxBLEAttributes[index].xAttributeType == eBTDbCharacteristic)
		{
			nbHandles += 2;
		}else
		{
			nbHandles++;
		}
	}

	return nbHandles;
}

BTStatus_t _createAttributes( BTService_t * pService )
{
    uint16_t attributes = 0;
    BTAttribute_t * pCurrentAtrribute;
    BTStatus_t status = eBTStatusFail;
    BTGattSrvcId_t tmpService;
    size_t nbHandles;

    for( attributes = 0; attributes < pService->xNumberOfAttributes; attributes++ )
    {
        pCurrentAtrribute = &pService->pxBLEAttributes[ attributes ];

        switch( pCurrentAtrribute->xAttributeType )
        {
            case eBTDbPrimaryService:

                /* Create Service. Start handle is 0 so the number of handle is usEndHandle + 1.
                 * The real handle is assigned in the callback of that call.*/
                tmpService.xId.ucInstId = pService->ucInstId;
                tmpService.xId.xUuid = pService->pxBLEAttributes[ 0 ].xServiceUUID;
                tmpService.xServiceType = pService->xType;
                nbHandles = _computeNumberOfHandles(pService);
                status = _BTInterface.pGattServerInterface->pxAddService( _BTInterface.serverIf,
                                                                            &tmpService,
																			nbHandles );

                break;

            case eBTDbIncludedService:
                status = _BTInterface.pGattServerInterface->pxAddIncludedService( _BTInterface.serverIf,
                                                                                    pService->pusHandlesBuffer[ 0 ],
                                                                                    pService->pxBLEAttributes[ attributes ].xIncludedService.pxPtrToService->pusHandlesBuffer[ 0 ] );
                break;

            case eBTDbDescriptor:
                status = _BTInterface.pGattServerInterface->pxAddDescriptor( _BTInterface.serverIf,
                                                                               pService->pusHandlesBuffer[ 0 ],
                                                                               &pService->pxBLEAttributes[ attributes ].xCharacteristicDescr.xUuid,
                                                                               pService->pxBLEAttributes[ attributes ].xCharacteristicDescr.xPermissions );
                break;

            case eBTDbCharacteristic:
                status = _BTInterface.pGattServerInterface->pxAddCharacteristic( _BTInterface.serverIf,
                                                                                   pService->pusHandlesBuffer[ 0 ],
                                                                                   &pService->pxBLEAttributes[ attributes ].xCharacteristic.xUuid,
                                                                                   pService->pxBLEAttributes[ attributes ].xCharacteristic.xProperties,
                                                                                   pService->pxBLEAttributes[ attributes ].xCharacteristic.xPermissions );
                break;

            default:
                status = eBTStatusFail;
        }

        if( status == eBTStatusSuccess )
        {
            xEventGroupWaitBits( ( EventGroupHandle_t ) &_BTInterface.waitOperationComplete,
                                 1 << eBLEHALEventAttributeAddedCb,
                                 pdTRUE,
                                 pdTRUE,
                                 portMAX_DELAY );
            if(_BTInterface.cbStatus != eBTStatusSuccess)
            {
            	status = _BTInterface.cbStatus;
            	break;
            }
        }
        else
        {
            break;
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

BTStatus_t IotBle_CreateService( BTService_t * pService,
                              IotBleAttributeEventCallback_t pEventsCallbacks[] )
{
    BTStatus_t status = eBTStatusParamInvalid;

    /* Create all attributes. */
    if( pService != NULL )
    {
        memset( pService->pusHandlesBuffer, 0, pService->xNumberOfAttributes );
        status = _addServiceToList( pService, pEventsCallbacks );
    }

    /* After all attributes have been create successfully, the service is added to the list. */
    if( status == eBTStatusSuccess )
    {
        if( xSemaphoreTake( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex, portMAX_DELAY ) == pdPASS )
        {
            status = _createAttributes( pService );
            xSemaphoreGive( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex );
        }
    }

    if( status == eBTStatusSuccess )
    {
        status = _BTInterface.pGattServerInterface->pxStartService( _BTInterface.serverIf,
                                                                      pService->pusHandlesBuffer[ 0 ],
                                                                      BTTransportLe );

        if( status == eBTStatusSuccess )
        {
            xEventGroupWaitBits( ( EventGroupHandle_t ) &_BTInterface.waitOperationComplete,
                                 1 << eBLEHALEventServiceStartedCb,
                                 pdTRUE,
                                 pdTRUE,
                                 portMAX_DELAY );
        }
    }

    return status;
}


/*-----------------------------------------------------------*/

BTStatus_t IotBle_DeleteService( BTService_t * pService )
{
    BTStatus_t status = eBTStatusSuccess;

    if( pService != NULL )
    {
        status = _BTInterface.pGattServerInterface->pxStopService( _BTInterface.serverIf,
                                                                     pService->pusHandlesBuffer[ 0 ] );

        if( status == eBTStatusSuccess )
        {
            xEventGroupWaitBits( ( EventGroupHandle_t ) &_BTInterface.waitOperationComplete,
                                 1 << eBLEHALEventServiceStoppedCb,
                                 pdTRUE,
                                 pdTRUE,
                                 portMAX_DELAY );
            /* To DO remove service from the list */


            status = _BTInterface.pGattServerInterface->pxDeleteService( _BTInterface.serverIf,
                                                                           pService->pusHandlesBuffer[ 0 ] );
        }

        if( status == eBTStatusSuccess )
        {
            xEventGroupWaitBits( ( EventGroupHandle_t ) &_BTInterface.waitOperationComplete,
                                 1 << eBLEHALEventServiceDeletedCb,
                                 pdTRUE,
                                 pdTRUE,
                                 portMAX_DELAY );
        }
    }
    else
    {
        status = eBTStatusParamInvalid;
    }

    return status;
}

/*-----------------------------------------------------------*/

BTStatus_t IotBle_SendIndication( IotBleEventResponse_t * pxResp,
                               uint16_t connId,
                               bool bConfirm )
{
    BTStatus_t status = eBTStatusSuccess;

    if( pxResp != NULL )
    {
        _BTInterface.handlePendingIndicationResponse = pxResp->pAttrData->handle;
        status = _BTInterface.pGattServerInterface->pxSendIndication( _BTInterface.serverIf,
                                                                        pxResp->pAttrData->handle,
                                                                        connId,
                                                                        pxResp->pAttrData->size,
                                                                        pxResp->pAttrData->pData,
                                                                        bConfirm );
    }
    else
    {
        status = eBTStatusParamInvalid;
    }

    return status;
}

/*-----------------------------------------------------------*/

BTStatus_t IotBle_SendResponse( IotBleEventResponse_t * pxResp,
                             uint16_t connId,
                             uint32_t transId )
{
    BTStatus_t status = eBTStatusSuccess;
    BTGattResponse_t response;

    if( pxResp != NULL )
    {
        response.xAttrValue.pucValue = pxResp->pAttrData->pData;
        response.xAttrValue.usOffset = pxResp->attrDataOffset;
        response.xAttrValue.usHandle = pxResp->pAttrData->handle;
        response.xAttrValue.xLen = pxResp->pAttrData->size;
        response.xAttrValue.xRspErrorStatus = pxResp->rspErrorStatus;
        _BTInterface.handlePendingIndicationResponse = response.xAttrValue.usHandle;

        status = _BTInterface.pGattServerInterface->pxSendResponse( connId,
                                                                      transId,
                                                                      pxResp->eventStatus,
                                                                      &response );
    }
    else
    {
        status = eBTStatusParamInvalid;
    }

    return status;
}

/*-----------------------------------------------------------*/

BTStatus_t IotBle_GetConnectionInfoList( IotLink_t ** pxConnectionInfoList )
{
    *pxConnectionInfoList = &_BTInterface.connectionListHead;

    return eBTStatusSuccess;
}

/*-----------------------------------------------------------*/

BTStatus_t IotBle_GetConnectionInfo( uint16_t connId,
                                  IotBleConnectionInfoListElement_t ** pvConnectionInfo )
{
    BTStatus_t status = eBTStatusFail;
    IotBleConnectionInfoListElement_t * pConnInfoListElem;
    IotLink_t * pConnListIndex;

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
        IotContainers_ForEach( &_BTInterface.connectionListHead, pConnListIndex )
        {
            pConnInfoListElem = IotLink_Container( IotBleConnectionInfoListElement_t, pConnListIndex, connectionList );

            if( connId == pConnInfoListElem->connId )
            {
                status = eBTStatusSuccess;
                *pvConnectionInfo = pConnInfoListElem;
                break;
            }
        }

        xSemaphoreGive( ( SemaphoreHandle_t ) &_BTInterface.threadSafetyMutex );
    }

    return status;
}
