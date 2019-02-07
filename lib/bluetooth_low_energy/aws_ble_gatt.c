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
 * @brief BLE GATT API.
 */

#include <string.h>
#include "FreeRTOS.h"
#include "event_groups.h"
#include "semphr.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "aws_ble.h"
#include "aws_ble_internals.h"


static void vServerRegisteredCb( BTStatus_t xStatus,
                                 uint8_t ucServerIf,
                                 BTUuid_t * pxAppUuid );
static void vServerUnregisteredCb( BTStatus_t xStatus,
                                   uint8_t ucServerIf );
static void vConnectionCb( uint16_t usConnId,
                           uint8_t ucServerIf,
                           bool bConnected,
                           BTBdaddr_t * pxBda );
static void vSeviceAddedCb( BTStatus_t xStatus,
                            uint8_t ucServerIf,
                            BTGattSrvcId_t * pxSrvcId,
                            uint16_t usSrvcHandle );
static void vCharAddedCb( BTStatus_t xStatus,
                          uint8_t ucServerIf,
                          BTUuid_t * pxUuid,
                          uint16_t usSrvcHandle,
                          uint16_t usHandle );
static void vCharDescrAddedCb( BTStatus_t xStatus,
                               uint8_t ucServerIf,
                               BTUuid_t * pxUuid,
                               uint16_t usSrvcHandle,
                               uint16_t usHandle );
static void vServiceStartedCb( BTStatus_t xStatus,
                               uint8_t ucServerIf,
                               uint16_t usSrvcHandle );
static void vServiceStoppedCb( BTStatus_t xStatus,
                               uint8_t ucServerIf,
                               uint16_t usSrvcHandle );
static void vServiceDeletedCb( BTStatus_t xStatus,
                               uint8_t ucServerIf,
                               uint16_t usSrvcHandle );
static void vIncludedServiceAdded( BTStatus_t xStatus,
                                   uint8_t ucServerIf,
                                   uint16_t usSrvcHandle,
                                   uint16_t usInclSrvcHandle );
static void vRequestReadCb( uint16_t usConnId,
                            uint32_t ulTransId,
                            BTBdaddr_t * pxBda,
                            uint16_t usAttrHandle,
                            uint16_t usOffset );
static void vRequestWriteCb( uint16_t usConnId,
                             uint32_t ulTransId,
                             BTBdaddr_t * pxBda,
                             uint16_t usAttrHandle,
                             uint16_t usOffset,
                             size_t xLength,
                             bool bNeedRsp,
                             bool bIsPrep,
                             uint8_t * pucValue );
static void vExecWriteCb( uint16_t usConnId,
                          uint32_t ulTransId,
                          BTBdaddr_t * pxBda,
                          bool bExecWrite );
static void vMtuChangedCb( uint16_t usConnId,
                           uint16_t usMtu );
static void vResponseConfirmationCb( BTStatus_t xStatus,
                                     uint16_t usHandle );
static void vIndicationSentCb( uint16_t usConnId,
                               BTStatus_t xStatus );

BTGattServerCallbacks_t xBTGattServerCb =
{
    .pxRegisterServerCb       = vServerRegisteredCb,
    .pxUnregisterServerCb     = vServerUnregisteredCb,
    .pxConnectionCb           = vConnectionCb,
    .pxServiceAddedCb         = vSeviceAddedCb,
    .pxIncludedServiceAddedCb = vIncludedServiceAdded,
    .pxCharacteristicAddedCb  = vCharAddedCb,
    .pxSetValCallbackCb       = NULL,
    .pxDescriptorAddedCb      = vCharDescrAddedCb,
    .pxServiceStartedCb       = vServiceStartedCb,
    .pxServiceStoppedCb       = vServiceStoppedCb,
    .pxServiceDeletedCb       = vServiceDeletedCb,
    .pxRequestReadCb          = vRequestReadCb,
    .pxRequestWriteCb         = vRequestWriteCb,
    .pxRequestExecWriteCb     = vExecWriteCb,
    .pxResponseConfirmationCb = vResponseConfirmationCb,
    .pxIndicationSentCb       = vIndicationSentCb,
    .pxCongestionCb           = NULL,
    .pxMtuChangedCb           = vMtuChangedCb
};


/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

void prvServiceClean( BLEServiceListElement_t * pxServiceElem )
{
    if( pxServiceElem != NULL )
    {
        IotListDouble_Remove( &pxServiceElem->xServiceList );
        vPortFree( pxServiceElem );
    }
}

/*-----------------------------------------------------------*/

BLEServiceListElement_t * prvGetServiceListElemFromHandle( uint16_t usHandle )
{
    IotLink_t * pxTmpElem;
    BLEServiceListElement_t * pxServiceElem = NULL;

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
        /* Remove service from service list */
        IotContainers_ForEach( &xBTInterface.xServiceListHead, pxTmpElem )
        {
            pxServiceElem = IotLink_Container( BLEServiceListElement_t, pxTmpElem, xServiceList );

            if( ( pxServiceElem->pxService->pusHandlesBuffer[ 0 ] <= usHandle ) &&
                ( usHandle <= pxServiceElem->usEndHandle ) )
            {
                break;
            }
        }

        xSemaphoreGive( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex );
    }

    return pxServiceElem;
}

/*-----------------------------------------------------------*/

BaseType_t prvGetCallbackFromHandle( uint16_t usAttrHandle,
                                     BLEAttributeEventCallback_t * pxEventsCallbacks )
{
    BLEServiceListElement_t * pxServiceElem;
    BaseType_t bFoundService = pdFAIL;
    size_t xAttributeIndex;

    /* The service that was just added is the last in the list */
    pxServiceElem = prvGetServiceListElemFromHandle( usAttrHandle );

    if( pxServiceElem != NULL )
    {
        for( xAttributeIndex = 0; xAttributeIndex < pxServiceElem->pxService->xNumberOfAttributes; xAttributeIndex++ )
        {
            if( pxServiceElem->pxService->pusHandlesBuffer[ xAttributeIndex ] == usAttrHandle )
            {
                *pxEventsCallbacks = pxServiceElem->pxEventsCallbacks[ xAttributeIndex ];
                bFoundService = pdPASS;
                break;
            }
        }
    }

    return bFoundService;
}

/*-----------------------------------------------------------*/

void vServerRegisteredCb( BTStatus_t xStatus,
                          uint8_t ucServerIf,
                          BTUuid_t * pxAppUuid )
{
    xBTInterface.ucServerIf = ucServerIf;
    xBTInterface.xCbStatus = xStatus;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete, 1 << eBLEHALEventServerRegisteredCb );
}

/*-----------------------------------------------------------*/

void vServerUnregisteredCb( BTStatus_t xStatus,
                            uint8_t ucServerIf )
{
    xBTInterface.ucServerIf = ucServerIf;
    xBTInterface.xCbStatus = xStatus;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete, 1 << eBLEHALEventServerRegisteredCb );
}

/*-----------------------------------------------------------*/

void vConnectionCb( uint16_t usConnId,
                    uint8_t ucServerIf,
                    bool bConnected,
                    BTBdaddr_t * pxBda )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLEConnectionInfoListElement_t * pxConnInfoListElem;
    IotLink_t * pxEventListIndex;

    BLESubscrEventListElement_t * pxEventIndex;

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
        if( bConnected == true )
        {
            /* Add a new connection to the list */
            pxConnInfoListElem = pvPortMalloc( sizeof( BLEConnectionInfoListElement_t ) );

            if( pxConnInfoListElem != NULL )
            {
                /* Initialize the new connection element */
                listINIT_HEAD( &pxConnInfoListElem->xClientCharDescrListHead );
                memcpy( &pxConnInfoListElem->pxRemoteBdAddr, pxBda, sizeof( BTBdaddr_t ) );
                pxConnInfoListElem->usConnId = usConnId;

                IotListDouble_InsertHead( &xBTInterface.xConnectionListHead, &pxConnInfoListElem->xConnectionList );
            }
            else
            {
                xStatus = eBTStatusNoMem;
            }
        }
        else
        {
            xStatus = BLE_GetConnectionInfo( usConnId, &pxConnInfoListElem );

            if( xStatus == eBTStatusSuccess )
            {
                /* Remove connection from the list safely */
                IotListDouble_Remove( &pxConnInfoListElem->xConnectionList );
                vPortFree( pxConnInfoListElem );
            }
        }

        /* Get the event associated to the callback */
        IotContainers_ForEach( &xBTInterface.xSubscrEventListHead[ eBLEConnection ], pxEventListIndex )
        {
            pxEventIndex = IotLink_Container( BLESubscrEventListElement_t, pxEventListIndex, xEventList );
            pxEventIndex->xSubscribedEventCb.pxConnectionCb( xStatus, usConnId, bConnected, pxBda );
        }

        ( void ) xSemaphoreGive( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex );
    }
}

/*-----------------------------------------------------------*/

BLEServiceListElement_t * prvGetLastAddedServiceElem( void )
{
    BLEServiceListElement_t * pxServiceElem = NULL;

    /* The service that was just added is the first in the list */
    if( !IotListDouble_IsEmpty( &xBTInterface.xServiceListHead ) )

    {
        pxServiceElem = IotLink_Container( BLEServiceListElement_t, xBTInterface.xServiceListHead.pNext, xServiceList );
    }

    return pxServiceElem;
}

/*-----------------------------------------------------------*/
void prvAttributeAdded( uint16_t usHandle,
                        BTStatus_t xStatus )
{
    BLEServiceListElement_t * pxServiceElem = prvGetLastAddedServiceElem();
    uint16_t usIndex;

    /* Now that service is found, add the handle */
    if( pxServiceElem != NULL )
    {
        for( usIndex = 0; usIndex < pxServiceElem->pxService->xNumberOfAttributes; usIndex++ )
        {
            if( pxServiceElem->pxService->pusHandlesBuffer[ usIndex ] == 0 )
            {
                pxServiceElem->pxService->pusHandlesBuffer[ usIndex ] = usHandle;
                break;
            }
        }

        pxServiceElem->usEndHandle = usHandle;
    }

    xBTInterface.xCbStatus = xStatus;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete, 1 << eBLEHALEventAttributeAddedCb );
}

/*-----------------------------------------------------------*/

void vSeviceAddedCb( BTStatus_t xStatus,
                     uint8_t ucServerIf,
                     BTGattSrvcId_t * pxSrvcId,
                     uint16_t usSrvcHandle )
{
    prvAttributeAdded( usSrvcHandle, xStatus );
}

/*-----------------------------------------------------------*/

void vCharAddedCb( BTStatus_t xStatus,
                   uint8_t ucServerIf,
                   BTUuid_t * pxUuid,
                   uint16_t usSrvcHandle,
                   uint16_t usHandle )
{
    prvAttributeAdded( usHandle, xStatus );
}

/*-----------------------------------------------------------*/

void vCharDescrAddedCb( BTStatus_t xStatus,
                        uint8_t ucServerIf,
                        BTUuid_t * pxUuid,
                        uint16_t usSrvcHandle,
                        uint16_t usHandle )
{
    prvAttributeAdded( usHandle, xStatus );
}


/*-----------------------------------------------------------*/

void vIncludedServiceAdded( BTStatus_t xStatus,
                            uint8_t ucServerIf,
                            uint16_t usSrvcHandle,
                            uint16_t usInclSrvcHandle )
{
    prvAttributeAdded( usInclSrvcHandle, xStatus );
}

/*-----------------------------------------------------------*/

void vServiceStartedCb( BTStatus_t xStatus,
                        uint8_t ucServerIf,
                        uint16_t usSrvcHandle )
{
    xBTInterface.xCbStatus = xStatus;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete, 1 << eBLEHALEventServiceStartedCb );
}

/*-----------------------------------------------------------*/

void vServiceStoppedCb( BTStatus_t xStatus,
                        uint8_t ucServerIf,
                        uint16_t usSrvcHandle )
{
    xBTInterface.xCbStatus = xStatus;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete, 1 << eBLEHALEventServiceStoppedCb );
}

/*-----------------------------------------------------------*/

void vServiceDeletedCb( BTStatus_t xStatus,
                        uint8_t ucServerIf,
                        uint16_t usSrvcHandle )
{
    BLEServiceListElement_t * pxServiceElem;

    /* The service has been stopped so it can be deleted safely */
    pxServiceElem = prvGetServiceListElemFromHandle( usSrvcHandle );

    if( pxServiceElem != NULL )
    {
        if( xSemaphoreTake( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
        {
            prvServiceClean( pxServiceElem );
            xSemaphoreGive( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex );
        }
        else
        {
            xStatus = eBTStatusFail;
        }
    }
    else
    {
        xStatus = eBTStatusFail;
    }

    xBTInterface.xCbStatus = xStatus;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete, 1 << eBLEHALEventServiceDeletedCb );
}

/*-----------------------------------------------------------*/

void vRequestReadCb( uint16_t usConnId,
                     uint32_t ulTransId,
                     BTBdaddr_t * pxBda,
                     uint16_t usAttrHandle,
                     uint16_t usOffset )
{
    BLEAttributeEventCallback_t xEventsCallbacks;
    BLEReadEventParams_t xReadParam;
    BLEAttributeEvent_t xEventParam;

    if( prvGetCallbackFromHandle( usAttrHandle, &xEventsCallbacks ) == pdPASS )
    {
        xReadParam.usAttrHandle = usAttrHandle;
        xReadParam.pxRemoteBdAddr = pxBda;
        xReadParam.ulTransId = ulTransId;
        xReadParam.usConnId = usConnId;
        xReadParam.usOffset = usOffset;

        xEventParam.xEventType = eBLERead;
        xEventParam.pxParamRead = &xReadParam;

        xEventsCallbacks( &xEventParam );
    }
}

/*-----------------------------------------------------------*/

void vRequestWriteCb( uint16_t usConnId,
                      uint32_t ulTransId,
                      BTBdaddr_t * pxBda,
                      uint16_t usAttrHandle,
                      uint16_t usOffset,
                      size_t xLength,
                      bool bNeedRsp,
                      bool bIsPrep,
                      uint8_t * pucValue )
{
    BLEWriteEventParams_t xWriteParam;
    BLEAttributeEvent_t xEventParam;
    BLEAttributeEventCallback_t xEventsCallbacks;

    if( prvGetCallbackFromHandle( usAttrHandle, &xEventsCallbacks ) == pdPASS )
    {
        if( bIsPrep == true )
        {
            xBTInterface.usHandlePendingPrepareWrite = usAttrHandle;
        }

        if( bNeedRsp == true )
        {
            xEventParam.xEventType = eBLEWrite;
        }
        else
        {
            xEventParam.xEventType = eBLEWriteNoResponse;
        }

        xWriteParam.usAttrHandle = usAttrHandle;
        xWriteParam.bIsPrep = bIsPrep;
        xWriteParam.pucValue = pucValue;
        xWriteParam.pxRemoteBdAddr = pxBda;
        xWriteParam.ulTransId = ulTransId;
        xWriteParam.usConnId = usConnId;
        xWriteParam.usOffset = usOffset;
        xWriteParam.xLength = xLength;

        xEventParam.pxParamWrite = &xWriteParam;

        xEventsCallbacks( &xEventParam );
    }
}

/*-----------------------------------------------------------*/

void vExecWriteCb( uint16_t usConnId,
                   uint32_t ulTransId,
                   BTBdaddr_t * pxBda,
                   bool bExecWrite )
{
    BLEExecWriteEventParams_t xExecWriteParam;
    BLEAttributeEvent_t xEventParam;
    BLEAttributeEventCallback_t xEventsCallbacks;

    if( prvGetCallbackFromHandle( xBTInterface.usHandlePendingPrepareWrite, &xEventsCallbacks ) == pdPASS )
    {
        xExecWriteParam.pxRemoteBdAddr = pxBda;
        xExecWriteParam.ulTransId = ulTransId;
        xExecWriteParam.usConnId = usConnId;

        xEventParam.xEventType = eBLEExecWrite;
        xEventParam.pxParamExecWrite = &xExecWriteParam;

        xEventsCallbacks( &xEventParam );
    }
}

/*-----------------------------------------------------------*/

void vMtuChangedCb( uint16_t usConnId,
                    uint16_t usMtu )
{
    IotLink_t * pxEventListIndex;
    BLESubscrEventListElement_t * pxEventIndex;

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
        /* Get the event associated to the callback */
        IotContainers_ForEach( &xBTInterface.xSubscrEventListHead[ eBLEMtuChanged ], pxEventListIndex )
        {
            pxEventIndex = IotLink_Container( BLESubscrEventListElement_t, pxEventListIndex, xEventList );
            pxEventIndex->xSubscribedEventCb.pxMtuChangedCb( usConnId, usMtu );
        }

        xSemaphoreGive( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex );
    }
}

/*-----------------------------------------------------------*/

static void vResponseConfirmationCb( BTStatus_t xStatus,
                                     uint16_t usHandle )
{
    BLERespConfirmEventParams_t xRespConfirmParam;
    BLEAttributeEvent_t xEventParam;
    BLEAttributeEventCallback_t xEventsCallbacks;

    if( prvGetCallbackFromHandle( usHandle, &xEventsCallbacks ) == pdPASS )
    {
        xRespConfirmParam.usHandle = usHandle;
        xRespConfirmParam.xStatus = xStatus;

        xEventParam.xEventType = eBLEResponseConfirmation;
        xEventParam.pxParamRespConfim = &xRespConfirmParam;

        xEventsCallbacks( &xEventParam );
    }
}

/*-----------------------------------------------------------*/

static void vIndicationSentCb( uint16_t usConnId,
                               BTStatus_t xStatus )
{
    BLEAttribute_t xAttribute;
    BLEIndicationSentEventParams_t xIndicationSentParam;
    BLEAttributeEvent_t xEventParam;
    BLEAttributeEventCallback_t xEventsCallbacks;

    if( prvGetCallbackFromHandle( xBTInterface.usHandlePendingIndicationResponse, &xEventsCallbacks ) == pdPASS )
    {
        xIndicationSentParam.usConnId = usConnId;
        xIndicationSentParam.xStatus = xStatus;

        xEventParam.pxParamIndicationSent = &xIndicationSentParam;
        xEventParam.xEventType = eBLEIndicationConfirmReceived;

        xEventsCallbacks( &xEventParam );
    }
}

/*-----------------------------------------------------------*/
BTStatus_t prvAddServiceToList( BLEService_t * pxService,
                                BLEAttributeEventCallback_t pxEventsCallbacks[] )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLEServiceListElement_t * pxServiceElem;
    BLEServiceListElement_t * pxNewElem;

    /* Create a space in the list for the service. */
    pxNewElem = pvPortMalloc( sizeof( BLEServiceListElement_t ) );
    memset( pxNewElem, 0, sizeof( BLEServiceListElement_t ) );

    if( pxNewElem != NULL )
    {
        pxNewElem->pxEventsCallbacks = pxEventsCallbacks;
        pxNewElem->pxService = pxService;

        if( xSemaphoreTake( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
        {
            IotListDouble_InsertHead( &xBTInterface.xServiceListHead, &pxNewElem->xServiceList );
            xSemaphoreGive( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex );
        }
    }
    else
    {
        xStatus = eBTStatusNoMem;
    }

    return eBTStatusSuccess;
}

/*-----------------------------------------------------------*/

BTStatus_t prvCreateAttributes( BLEService_t * pxService )
{
    uint16_t usAttributes = 0;
    BLEAttribute_t * pxCurrentAtrribute;
    BTStatus_t xStatus = eBTStatusFail;
    BTGattSrvcId_t xTmpService;

    for( usAttributes = 0; usAttributes < pxService->xNumberOfAttributes; usAttributes++ )
    {
        pxCurrentAtrribute = &pxService->pxBLEAttributes[ usAttributes ];

        switch( pxCurrentAtrribute->xAttributeType )
        {
            case eBTDbPrimaryService:

                /* Create Service. Start handle is 0 so the number of handle is usEndHandle + 1.
                 * The real handle is assigned in the callback of that call.*/
                xTmpService.xId.ucInstId = pxService->ucInstId;
                xTmpService.xId.xUuid = pxService->pxBLEAttributes[ 0 ].xServiceUUID;
                xTmpService.xServiceType = pxService->xType;
                xStatus = xBTInterface.pxGattServerInterface->pxAddService( xBTInterface.ucServerIf,
                                                                            &xTmpService,
                                                                            pxService->xNumberOfAttributes );

                break;

            case eBTDbIncludedService:
                xStatus = xBTInterface.pxGattServerInterface->pxAddIncludedService( xBTInterface.ucServerIf,
                                                                                    pxService->pusHandlesBuffer[ 0 ],
                                                                                    pxService->pxBLEAttributes[ usAttributes ].xIncludedService.pxPtrToService->pusHandlesBuffer[ 0 ] );
                break;

            case eBTDbDescriptor:
                xStatus = xBTInterface.pxGattServerInterface->pxAddDescriptor( xBTInterface.ucServerIf,
                                                                               pxService->pusHandlesBuffer[ 0 ],
                                                                               &pxService->pxBLEAttributes[ usAttributes ].xCharacteristicDescr.xUuid,
                                                                               pxService->pxBLEAttributes[ usAttributes ].xCharacteristicDescr.xPermissions );
                break;

            case eBTDbCharacteristic:
                xStatus = xBTInterface.pxGattServerInterface->pxAddCharacteristic( xBTInterface.ucServerIf,
                                                                                   pxService->pusHandlesBuffer[ 0 ],
                                                                                   &pxService->pxBLEAttributes[ usAttributes ].xCharacteristic.xUuid,
                                                                                   pxService->pxBLEAttributes[ usAttributes ].xCharacteristic.xProperties,
                                                                                   pxService->pxBLEAttributes[ usAttributes ].xCharacteristic.xPermissions );
                break;

            default:
                xStatus = eBTStatusFail;
        }

        if( xStatus == eBTStatusSuccess )
        {
            xEventGroupWaitBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete,
                                 1 << eBLEHALEventAttributeAddedCb,
                                 pdTRUE,
                                 pdTRUE,
                                 portMAX_DELAY );
        }
        else
        {
            break;
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t BLE_CreateService( BLEService_t * pxService,
                              BLEAttributeEventCallback_t pxEventsCallbacks[] )
{
    BTStatus_t xStatus = eBTStatusParamInvalid;

    /* Create all attributes. */
    if( pxService != NULL )
    {
        memset( pxService->pusHandlesBuffer, 0, pxService->xNumberOfAttributes );
        xStatus = prvAddServiceToList( pxService, pxEventsCallbacks );
    }

    /* After all attributes have been create successfully, the service is added to the list. */
    if( xStatus == eBTStatusSuccess )
    {
        if( xSemaphoreTake( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
        {
            xStatus = prvCreateAttributes( pxService );
            xSemaphoreGive( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex );
        }
    }

    if( xStatus == eBTStatusSuccess )
    {
        xStatus = xBTInterface.pxGattServerInterface->pxStartService( xBTInterface.ucServerIf,
                                                                      pxService->pusHandlesBuffer[ 0 ],
                                                                      BTTransportLe );

        if( xStatus == eBTStatusSuccess )
        {
            xEventGroupWaitBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete,
                                 1 << eBLEHALEventServiceStartedCb,
                                 pdTRUE,
                                 pdTRUE,
                                 portMAX_DELAY );
        }
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t BLE_DeleteService( BLEService_t * pxService )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    if( pxService != NULL )
    {
        xStatus = xBTInterface.pxGattServerInterface->pxStopService( xBTInterface.ucServerIf,
                                                                     pxService->pusHandlesBuffer[ 0 ] );

        if( xStatus == eBTStatusSuccess )
        {
            xEventGroupWaitBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete,
                                 1 << eBLEHALEventServiceStoppedCb,
                                 pdTRUE,
                                 pdTRUE,
                                 portMAX_DELAY );
            /* To DO remove service from the list */


            xStatus = xBTInterface.pxGattServerInterface->pxDeleteService( xBTInterface.ucServerIf,
                                                                           pxService->pusHandlesBuffer[ 0 ] );
        }

        if( xStatus == eBTStatusSuccess )
        {
            xEventGroupWaitBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete,
                                 1 << eBLEHALEventServiceDeletedCb,
                                 pdTRUE,
                                 pdTRUE,
                                 portMAX_DELAY );
        }
    }
    else
    {
        xStatus = eBTStatusParamInvalid;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t BLE_SendIndication( BLEEventResponse_t * pxResp,
                               uint16_t usConnId,
                               bool bConfirm )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    if( pxResp != NULL )
    {
        xBTInterface.usHandlePendingIndicationResponse = pxResp->pxAttrData->xHandle;
        xStatus = xBTInterface.pxGattServerInterface->pxSendIndication( xBTInterface.ucServerIf,
                                                                        pxResp->pxAttrData->xHandle,
                                                                        usConnId,
                                                                        pxResp->pxAttrData->xSize,
                                                                        pxResp->pxAttrData->pucData,
                                                                        bConfirm );
    }
    else
    {
        xStatus = eBTStatusParamInvalid;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t BLE_SendResponse( BLEEventResponse_t * pxResp,
                             uint16_t usConnId,
                             uint32_t ulTransId )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BTGattResponse_t xResponse;

    if( pxResp != NULL )
    {
        xResponse.xAttrValue.pucValue = pxResp->pxAttrData->pucData;
        xResponse.xAttrValue.usOffset = pxResp->xAttrDataOffset;
        xResponse.xAttrValue.usHandle = pxResp->pxAttrData->xHandle;
        xResponse.xAttrValue.xLen = pxResp->pxAttrData->xSize;
        xResponse.xAttrValue.xRspErrorStatus = pxResp->xRspErrorStatus;
        xBTInterface.usHandlePendingIndicationResponse = xResponse.xAttrValue.usHandle;

        xStatus = xBTInterface.pxGattServerInterface->pxSendResponse( usConnId,
                                                                      ulTransId,
                                                                      pxResp->xEventStatus,
                                                                      &xResponse );
    }
    else
    {
        xStatus = eBTStatusParamInvalid;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t BLE_GetConnectionInfoList( IotLink_t ** ppxConnectionInfoList )
{
    *ppxConnectionInfoList = &xBTInterface.xConnectionListHead;

    return eBTStatusSuccess;
}

/*-----------------------------------------------------------*/

BTStatus_t BLE_GetConnectionInfo( uint16_t usConnId,
                                  BLEConnectionInfoListElement_t ** ppvConnectionInfo )
{
    BTStatus_t xStatus = eBTStatusFail;
    BLEConnectionInfoListElement_t * pxConnInfoListElem;
    IotLink_t * pxConnListIndex;

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
        IotContainers_ForEach( &xBTInterface.xConnectionListHead, pxConnListIndex )
        {
            pxConnInfoListElem = IotLink_Container( BLEConnectionInfoListElement_t, pxConnListIndex, xConnectionList );

            if( usConnId == pxConnInfoListElem->usConnId )
            {
                xStatus = eBTStatusSuccess;
                *ppvConnectionInfo = pxConnInfoListElem;
                break;
            }
        }

        xSemaphoreGive( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex );
    }

    return xStatus;
}
