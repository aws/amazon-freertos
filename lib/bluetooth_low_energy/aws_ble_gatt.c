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

#include "string.h"
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

BTStatus_t prvCreateDescriptors( BLEService_t * pxService,
                                 BLECharacteristic_t * pxCharacteristic,
                                 BLECharacteristicDescr_t * pxDescriptor )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    /* Fill up the field and create */
    pxDescriptor->xAttributeData.xHandle = 0;
    pxDescriptor->pxParentService = pxService;
    pxDescriptor->pxParentCharacteristic = pxCharacteristic;

    /* @TODO Check status */
    xBTInterface.pxGattServerInterface->pxAddDescriptor( xBTInterface.ucServerIf,
                                                         pxService->xAttributeData.xHandle,
                                                         &pxDescriptor->xAttributeData.xUuid,
                                                         pxDescriptor->xPermissions );
    /* Wait for attribute creation */
    xEventGroupWaitBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete,
                         1 << eBLEHALEventCharDescrAddedCb,
                         pdTRUE,
						 pdTRUE,
                         portMAX_DELAY );

    /* Error checks */
    if( ( xBTInterface.xCbStatus != eBTStatusSuccess ) ||
        ( pxDescriptor->xAttributeData.xHandle == 0 ) )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvCreateCharacteristicAndDescriptors( BLEService_t * pxService,
                                                  BLECharacteristic_t * pxCharacteristic,
                                                  BLEAttribute_t * pxAttributesPtr,
                                                  size_t * pxAttributeCounter )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    size_t xIndex;

    /* Fill up the field and create */
    pxCharacteristic->xAttributeData.xHandle = 0;
    pxCharacteristic->pxParentService = pxService;

    /* Empty attribute for characteristic declaration */
    pxAttributesPtr[ *pxAttributeCounter ].xAttributeType = eBTDbCharacteristicDecl;
    pxAttributesPtr[ ( *pxAttributeCounter )++ ].pxCharDeclaration = NULL;

    /* Add characteristic pointer to the attribute table */
    pxAttributesPtr[ *pxAttributeCounter ].xAttributeType = eBTDbCharacteristic;
    pxAttributesPtr[ ( *pxAttributeCounter )++ ].pxCharacteristic = pxCharacteristic;

    /* @TODO check status here */
    xBTInterface.pxGattServerInterface->pxAddCharacteristic( xBTInterface.ucServerIf,
                                                             pxService->xAttributeData.xHandle,
                                                             &pxCharacteristic->xAttributeData.xUuid,
                                                             pxCharacteristic->xProperties,
                                                             pxCharacteristic->xPermissions );

    /* Wait for attribute creation */
    xEventGroupWaitBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete,
                         1 << eBLEHALEventCharAddedCb,
						 pdTRUE,
						 pdTRUE,
                         portMAX_DELAY );

    /* Error checks */
    if( ( xBTInterface.xCbStatus != eBTStatusSuccess ) ||
        ( pxCharacteristic->xAttributeData.xHandle == 0 ) )
    {
        xStatus = eBTStatusFail;
    }

    /* Now create all descriptors associated with the characteristics */
    if( xStatus == eBTStatusSuccess )
    {
        for( xIndex = 0; xIndex < pxCharacteristic->xNbDescriptors; xIndex++ )
        {
            pxAttributesPtr[ *pxAttributeCounter ].xAttributeType = eBTDbDescriptor;
            pxAttributesPtr[ ( *pxAttributeCounter )++ ].pxCharacteristicDescr = pxCharacteristic->pxDescriptors[ xIndex ];
            xStatus = prvCreateDescriptors( pxService, pxCharacteristic, pxCharacteristic->pxDescriptors[ xIndex ] );
        }
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

void prvServiceClean( BLEServiceListElement_t * pxServiceElem )
{
    size_t xCharId;

    if( pxServiceElem != NULL )
    {
        listREMOVE( &pxServiceElem->xServiceList );

        /* Delete service from memory */
        if( pxServiceElem->pxAttributesPtr != NULL )
        {
            vPortFree( pxServiceElem->pxAttributesPtr );
        }

        if( pxServiceElem->pxService != NULL )
        {
			if( pxServiceElem->pxService->pxCharacteristics != NULL )
			{
				for( xCharId = 0; xCharId < pxServiceElem->pxService->xNbCharacteristics; xCharId++ )
				{
					if( pxServiceElem->pxService->pxCharacteristics[ xCharId ].pxDescriptors != NULL )
					{
						vPortFree( pxServiceElem->pxService->pxCharacteristics[ xCharId ].pxDescriptors );
					}
				}

				vPortFree( pxServiceElem->pxService->pxCharacteristics );
			}

			if( pxServiceElem->pxService->pxDescriptors != NULL )
			{
				vPortFree( pxServiceElem->pxService->pxDescriptors );
			}

			if( pxServiceElem->pxService->pxIncludedServices != NULL )
			{
				vPortFree( pxServiceElem->pxService->pxIncludedServices );
			}

            vPortFree( pxServiceElem->pxService );
        }

        vPortFree( pxServiceElem );
    }
}


/*-----------------------------------------------------------*/

void prvTriggerAttriButeCallback( BLEAttribute_t * pxAttribute,
                                  BLEAttributeEvent_t * pxEventParam )
{
    switch( pxAttribute->xAttributeType )
    {
        case eBTDbCharacteristic:
            configASSERT( pxAttribute->pxCharacteristic->pxAttributeEventCallback != NULL );
            pxAttribute->pxCharacteristic->pxAttributeEventCallback( pxAttribute, pxEventParam );
            break;

        case eBTDbDescriptor:
            configASSERT( pxAttribute->pxCharacteristicDescr->pxAttributeEventCallback != NULL );
            pxAttribute->pxCharacteristicDescr->pxAttributeEventCallback( pxAttribute, pxEventParam );
            break;

        default:
            configPRINTF( ( "Unsupported Read/Write callback on Attribute type\n" ) );
            break;
    }
}

/*-----------------------------------------------------------*/

BaseType_t prvGetAttributeFromHandle( uint16_t usAttrHandle,
                                      BLEAttribute_t * pxAttribute )
{
    Link_t * pxTmpElem;
    BLEServiceListElement_t * pxServiceElem;
    BaseType_t bFoundService = pdFAIL;

    /* The service that was just added is the last in the list */
    if( xSemaphoreTake( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
        listFOR_EACH( pxTmpElem, &xBTInterface.xServiceListHead )
        {
            pxServiceElem = listCONTAINER( pxTmpElem, BLEServiceListElement_t, xServiceList );

            if( ( usAttrHandle >= pxServiceElem->usStartHandle ) && ( usAttrHandle <= pxServiceElem->usEndHandle ) )
            {
                bFoundService = pdPASS;
                break;
            }
        }
        xSemaphoreGive( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex );
    }

    if( bFoundService == pdPASS )
    {
        *pxAttribute = pxServiceElem->pxAttributesPtr[ usAttrHandle - pxServiceElem->usStartHandle ];
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
    Link_t * pxEventListIndex;
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

                listADD( &xBTInterface.xConnectionListHead, &pxConnInfoListElem->xConnectionList );
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
                listREMOVE( &pxConnInfoListElem->xConnectionList );
                vPortFree( pxConnInfoListElem );
            }
        }

        /* Get the event associated to the callback */
        listFOR_EACH( pxEventListIndex, &xBTInterface.xSubscrEventListHead[ eBLEConnection ] )
        {
            pxEventIndex = listCONTAINER( pxEventListIndex, BLESubscrEventListElement_t, xEventList );
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
    if( !listIS_EMPTY( &xBTInterface.xServiceListHead ) )
    {
        pxServiceElem = listCONTAINER( xBTInterface.xServiceListHead.pxNext, BLEServiceListElement_t, xServiceList );
    }

    return pxServiceElem;
}

/*-----------------------------------------------------------*/

BLEServiceListElement_t * prvGetServiceListElemFromHandle( uint16_t usSrvcHandle )
{
    Link_t * pxTmpElem;
    BLEServiceListElement_t * pxServiceElem = NULL;

    /* Remove service from service list */
    listFOR_EACH( pxTmpElem, &xBTInterface.xServiceListHead )
    {
    	pxServiceElem = listCONTAINER( pxTmpElem, BLEServiceListElement_t, xServiceList );

    	if( pxServiceElem->pxService->xAttributeData.xHandle == usSrvcHandle )
    	{
    		break;
    	}
    }

    return pxServiceElem;
}

/*-----------------------------------------------------------*/

void vSeviceAddedCb( BTStatus_t xStatus,
                     uint8_t ucServerIf,
                     BTGattSrvcId_t * pxSrvcId,
                     uint16_t usSrvcHandle )
{
    BLEServiceListElement_t * pxServiceElem = prvGetLastAddedServiceElem();

    /* Now that service is found, add the handle */
    if( pxServiceElem != NULL )
    {
        pxServiceElem->usStartHandle += usSrvcHandle;
        pxServiceElem->usEndHandle += usSrvcHandle;
        pxServiceElem->pxService->xAttributeData.xHandle = usSrvcHandle;
    }

    xBTInterface.xCbStatus = xStatus;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete, 1 << eBLEHALEventSeviceAddedCb );
}

/*-----------------------------------------------------------*/

void vCharAddedCb( BTStatus_t xStatus,
                   uint8_t ucServerIf,
                   BTUuid_t * pxUuid,
                   uint16_t usSrvcHandle,
                   uint16_t usHandle )
{
    BLEServiceListElement_t * pxServiceElem = prvGetLastAddedServiceElem();
    BLECharacteristic_t * pxCharacteristic;

    /* Now that service is found, add the handle */
    if( pxServiceElem != NULL )
    {
        pxCharacteristic = pxServiceElem->pxAttributesPtr[ usHandle - pxServiceElem->usStartHandle ].pxCharacteristic;

        pxCharacteristic->xAttributeData.xHandle = usHandle;
    }

    xBTInterface.xCbStatus = xStatus;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete, 1 << eBLEHALEventCharAddedCb );
}

/*-----------------------------------------------------------*/

void vCharDescrAddedCb( BTStatus_t xStatus,
                        uint8_t ucServerIf,
                        BTUuid_t * pxUuid,
                        uint16_t usSrvcHandle,
                        uint16_t usHandle )
{
    BLEServiceListElement_t * pxServiceElem = prvGetLastAddedServiceElem();
    BLECharacteristicDescr_t * pxCharacteristicDescr;

    /* Now that service is found, add the handle */
    if( pxServiceElem != NULL )
    {
        pxCharacteristicDescr = pxServiceElem->pxAttributesPtr[ usHandle - pxServiceElem->usStartHandle ].pxCharacteristicDescr;

        pxCharacteristicDescr->xAttributeData.xHandle = usHandle;
    }

    xBTInterface.xCbStatus = xStatus;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete, 1 << eBLEHALEventCharDescrAddedCb );
}

/*-----------------------------------------------------------*/

void vServiceStartedCb( BTStatus_t xStatus,
                        uint8_t ucServerIf,
                        uint16_t usSrvcHandle )
{
    BLEServiceListElement_t * pxServiceElem;

    if( xStatus == eBTStatusSuccess )
    {
    	pxServiceElem = prvGetServiceListElemFromHandle( usSrvcHandle );

        if( pxServiceElem != NULL )
        {
            ( ( BLEServiceStartedCallback_t ) xBTInterface.pxPendingServiceEvent )( xStatus, pxServiceElem->pxService );
        }
        else
        {
            configPRINTF( ( "Started service not registered in service list\n" ) );
        }
    }
}

/*-----------------------------------------------------------*/

void vServiceStoppedCb( BTStatus_t xStatus,
                        uint8_t ucServerIf,
                        uint16_t usSrvcHandle )
{
    BLEServiceListElement_t * pxServiceElem;

    if( xStatus == eBTStatusSuccess )
    {
    	pxServiceElem = prvGetServiceListElemFromHandle( usSrvcHandle );

        if( pxServiceElem != NULL )
        {
            ( ( BLEServiceStoppedCallback_t ) xBTInterface.pxPendingServiceEvent )( xStatus, pxServiceElem->pxService );
        }
        else
        {
            configPRINTF( ( "Stopped service not registered in service list\n" ) );
        }
    }
}

/*-----------------------------------------------------------*/

void vServiceDeletedCb( BTStatus_t xStatus,
                        uint8_t ucServerIf,
                        uint16_t usSrvcHandle )
{
    BLEServiceListElement_t * pxServiceElem;

    pxServiceElem = prvGetServiceListElemFromHandle( usSrvcHandle );

    if( pxServiceElem != NULL )
    {
    	prvServiceClean( pxServiceElem );
    }
    else
    {
    	xStatus = eBTStatusFail;
    }

    xBTInterface.xCbStatus = xStatus;

    ( ( BLEServiceDeletedCallback_t ) xBTInterface.pxPendingServiceEvent )( xStatus, usSrvcHandle );
}

/*-----------------------------------------------------------*/

void vIncludedServiceAdded( BTStatus_t xStatus,
                            uint8_t ucServerIf,
                            uint16_t usSrvcHandle,
                            uint16_t usInclSrvcHandle )
{
    BLEServiceListElement_t * pxServiceElem = prvGetLastAddedServiceElem();
    BLEService_t * pxIncludedService;

    /* Now that service is found, add the handle */
    if( pxServiceElem != NULL )
    {
        pxIncludedService = pxServiceElem->pxAttributesPtr[ usInclSrvcHandle - pxServiceElem->usStartHandle ].pxService;

        pxIncludedService->xAttributeData.xHandle = usInclSrvcHandle;
    }

    xBTInterface.xCbStatus = xStatus;
    ( void ) xEventGroupSetBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete, 1 << eBLEHALEventIncludedServiceAdded );
}

/*-----------------------------------------------------------*/

void vRequestReadCb( uint16_t usConnId,
                     uint32_t ulTransId,
                     BTBdaddr_t * pxBda,
                     uint16_t usAttrHandle,
                     uint16_t usOffset )
{
    BLEAttribute_t xAttribute;
    BLEReadEventParams_t xReadParam;
    BLEAttributeEvent_t xEventParam;

    if( prvGetAttributeFromHandle( usAttrHandle, &xAttribute ) == pdPASS )
    {
        xReadParam.pxAttribute = &xAttribute;
        xReadParam.pxRemoteBdAddr = pxBda;
        xReadParam.ulTransId = ulTransId;
        xReadParam.usConnId = usConnId;
        xReadParam.usOffset = usOffset;

        xEventParam.xEventType = eBLERead;
        xEventParam.pxParamRead = &xReadParam;

        prvTriggerAttriButeCallback( &xAttribute, &xEventParam );
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
    BLEAttribute_t xAttribute;
    BLEWriteEventParams_t xWriteParam;
    BLEAttributeEvent_t xEventParam;

    if( prvGetAttributeFromHandle( usAttrHandle, &xAttribute ) == pdPASS )
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

        xWriteParam.pxAttribute = &xAttribute;
        xWriteParam.bIsPrep = bIsPrep;
        xWriteParam.pucValue = pucValue;
        xWriteParam.pxRemoteBdAddr = pxBda;
        xWriteParam.ulTransId = ulTransId;
        xWriteParam.usConnId = usConnId;
        xWriteParam.usOffset = usOffset;
        xWriteParam.xLength = xLength;

        xEventParam.pxParamWrite = &xWriteParam;

        prvTriggerAttriButeCallback( &xAttribute, &xEventParam );
    }
}

/*-----------------------------------------------------------*/

void vExecWriteCb( uint16_t usConnId,
                   uint32_t ulTransId,
                   BTBdaddr_t * pxBda,
                   bool bExecWrite )
{
    BLEAttribute_t xAttribute;
    BLEExecWriteEventParams_t xExecWriteParam;
    BLEAttributeEvent_t xEventParam;

    if( prvGetAttributeFromHandle( xBTInterface.usHandlePendingPrepareWrite, &xAttribute ) == pdPASS )
    {
        xExecWriteParam.pxAttribute = &xAttribute;
        xExecWriteParam.pxRemoteBdAddr = pxBda;
        xExecWriteParam.ulTransId = ulTransId;
        xExecWriteParam.usConnId = usConnId;

        xEventParam.xEventType = eBLEExecWrite;
        xEventParam.pxParamExecWrite = &xExecWriteParam;

        prvTriggerAttriButeCallback( &xAttribute, &xEventParam );
    }
}

/*-----------------------------------------------------------*/

void vMtuChangedCb( uint16_t usConnId,
                    uint16_t usMtu )
{
    Link_t * pxEventListIndex;
    BLESubscrEventListElement_t * pxEventIndex;

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
        /* Get the event associated to the callback */
        listFOR_EACH( pxEventListIndex, &xBTInterface.xSubscrEventListHead[ eBLEMtuChanged ] )
        {
            pxEventIndex = listCONTAINER( pxEventListIndex, BLESubscrEventListElement_t, xEventList );
            pxEventIndex->xSubscribedEventCb.pxMtuChangedCb( usConnId, usMtu );
        }
        xSemaphoreGive( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex );
    }
}

/*-----------------------------------------------------------*/

static void vResponseConfirmationCb( BTStatus_t xStatus,
                                     uint16_t usHandle )
{
    BLEAttribute_t xAttribute;
    BLERespConfirmEventParams_t xRespConfirmParam;
    BLEAttributeEvent_t xEventParam;

    if( prvGetAttributeFromHandle( usHandle, &xAttribute ) == pdPASS )
    {
        xRespConfirmParam.pxAttribute = &xAttribute;
        xRespConfirmParam.xStatus = xStatus;

        xEventParam.xEventType = eBLEResponseConfirmation;
        xEventParam.pxParamRespConfim = &xRespConfirmParam;

        prvTriggerAttriButeCallback( &xAttribute, &xEventParam );
    }
}

/*-----------------------------------------------------------*/

static void vIndicationSentCb( uint16_t usConnId,
                               BTStatus_t xStatus )
{
    BLEAttribute_t xAttribute;
    BLEIndicationSentEventParams_t xIndicationSentParam;
    BLEAttributeEvent_t xEventParam;

    if( prvGetAttributeFromHandle( xBTInterface.usHandlePendingIndicationResponse, &xAttribute ) == pdPASS )
    {
        xIndicationSentParam.pxAttribute = &xAttribute;
        xIndicationSentParam.usConnId = usConnId;
        xIndicationSentParam.xStatus = xStatus;

        xEventParam.pxParamIndicationSent = &xIndicationSentParam;
        xEventParam.xEventType = eBLEIndicationConfirmReceived;

        prvTriggerAttriButeCallback( &xAttribute, &xEventParam );
    }
}

/*-----------------------------------------------------------*/

BTStatus_t BLE_CreateService( BLEService_t ** ppxService,
                              size_t xNbCharacteristic,
                              size_t xNbDescriptors,
                              size_t xNumDescrsPerChar[],
                              size_t xNbIncludedServices )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLEServiceListElement_t * pxNewElem = NULL;
    uint16_t usNumAttributes;
    size_t xCharId;

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {

        if( ppxService == NULL )
        {
            xStatus = eBTStatusParamInvalid;
        }

        /* Create a space in the list for the service. */
        if( xStatus == eBTStatusSuccess )
        {
            pxNewElem = pvPortMalloc( sizeof( BLEServiceListElement_t ) );
            memset( pxNewElem, 0, sizeof( BLEServiceListElement_t ) );

            if( pxNewElem != NULL )
            {

                    /* Compute the total number of handles
                     * Multiply characteristics by 2 since they need a declaration. */
                    usNumAttributes = xNbCharacteristic * 2
                                      + xNbDescriptors
                                      + xNbIncludedServices + 1;

                    pxNewElem->usStartHandle = 0;
                    pxNewElem->usEndHandle = usNumAttributes - 1;
                    pxNewElem->pxAttributesPtr = pvPortMalloc( usNumAttributes * sizeof( BLEAttribute_t ) );

                    if( pxNewElem->pxAttributesPtr != NULL )
                    {
                        memset( pxNewElem->pxAttributesPtr, 0, sizeof( BLEAttribute_t ) * usNumAttributes );
                        listADD( &xBTInterface.xServiceListHead, &pxNewElem->xServiceList );
                    }
                    else
                    {
                        xStatus = eBTStatusNoMem;
                    }
            }
            else
            {
                xStatus = eBTStatusFail;
            }
        }

        /* Allocate Memory for the service. */
        if( xStatus == eBTStatusSuccess )
        {
            *ppxService = pvPortMalloc( sizeof( BLEService_t ) );
            memset( *ppxService, 0, sizeof( BLEService_t ) );

            if( *ppxService == NULL )
            {
                xStatus = eBTStatusNoMem;
            }
            else
            {
                /* Add newly created service to the list. */
                pxNewElem->pxService = *ppxService;
                ( *ppxService )->xNbCharacteristics = xNbCharacteristic;
                ( *ppxService )->xNbDescriptors = xNbDescriptors;
                ( *ppxService )->xNbIncludedServices = xNbIncludedServices;
                ( *ppxService )->pxCharacteristics = NULL;
                ( *ppxService )->pxDescriptors = NULL;
                ( *ppxService )->pxIncludedServices = NULL;

                if( xNbCharacteristic > 0 )
                {
                    ( *ppxService )->pxCharacteristics = pvPortMalloc( sizeof( BLECharacteristic_t ) * xNbCharacteristic );

                    if( ( *ppxService )->pxCharacteristics != NULL )
                    {
                        memset( ( *ppxService )->pxCharacteristics, 0, sizeof( BLECharacteristic_t ) * xNbCharacteristic );

                        for( xCharId = 0; xCharId < xNbCharacteristic; xCharId++ )
                        {
                            if( xNumDescrsPerChar[ xCharId ] > 0 )
                            {
                                ( *ppxService )->pxCharacteristics[ xCharId ].pxDescriptors = pvPortMalloc( sizeof( BLECharacteristicDescr_t * ) * xNumDescrsPerChar[ xCharId ] );

                                if( ( *ppxService )->pxCharacteristics[ xCharId ].pxDescriptors == NULL )
                                {
                                    xStatus = eBTStatusNoMem;
                                    break;
                                }
                                else
                                {
                                    memset( ( *ppxService )->pxCharacteristics[ xCharId ].pxDescriptors, 0, sizeof( BLECharacteristicDescr_t * ) * xNumDescrsPerChar[ xCharId ] );
                                }
                            }
                            else
                            {
                                ( *ppxService )->pxCharacteristics[ xCharId ].pxDescriptors = NULL;
                            }
                        }
                    }
                    else
                    {
                        xStatus = eBTStatusNoMem;
                    }
                }

                if( ( xNbDescriptors > 0 ) && ( xStatus == eBTStatusSuccess ) )
                {
                    ( *ppxService )->pxDescriptors = pvPortMalloc( sizeof( BLECharacteristicDescr_t ) * xNbDescriptors );

                    if( ( *ppxService )->pxDescriptors != NULL )
                    {
                        memset( ( *ppxService )->pxDescriptors, 0, sizeof( BLECharacteristicDescr_t ) * xNbDescriptors );
                    }
                    else
                    {
                        xStatus = eBTStatusNoMem;
                    }
                }

                if( ( xNbIncludedServices > 0 ) && ( xStatus == eBTStatusSuccess ) )
                {
                    ( *ppxService )->pxIncludedServices = pvPortMalloc( sizeof( BLEService_t * ) * xNbIncludedServices );

                    if( ( *ppxService )->pxIncludedServices != NULL )
                    {
                        memset( ( *ppxService )->pxIncludedServices, 0, sizeof( BLEService_t * ) * xNbIncludedServices );
                    }
                    else
                    {
                        xStatus = eBTStatusNoMem;
                    }
                }
            }
        }

        if( xStatus != eBTStatusSuccess )
        {
            prvServiceClean( pxNewElem );
        }

        xSemaphoreGive( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex );
    }
    else
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t BLE_AddService( BLEService_t * pxService )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BTGattSrvcId_t pxSrvcId;
    BLEServiceListElement_t * pxServiceElem;
    size_t xIndex;
    size_t xAttributeCounter = 0;

    if( pxService != NULL )
    {
        if( xSemaphoreTake( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
        {
            pxSrvcId.xId.ucInstId = pxService->ucInstId;
            pxSrvcId.xServiceType = pxService->xServiceType;
            pxSrvcId.xId.xUuid.ucType = pxService->xAttributeData.xUuid.ucType;

            memcpy( pxSrvcId.xId.xUuid.uu.uu128, pxService->xAttributeData.xUuid.uu.uu128, bt128BIT_UUID_LEN );
            pxServiceElem = prvGetLastAddedServiceElem();

            if(pxServiceElem != NULL)
            {
				if( pxService->xServiceType == eBTServiceTypePrimary )
				{
					pxServiceElem->pxAttributesPtr[ xAttributeCounter ].xAttributeType = eBTDbPrimaryService;
				}
				else
				{
					pxServiceElem->pxAttributesPtr[ xAttributeCounter ].xAttributeType = eBTDbSecondaryService;
				}

				pxServiceElem->pxAttributesPtr[ xAttributeCounter++ ].pxService = pxService;

				/* Create Service. Start handle is 0 so the number of handle is usEndHandle + 1.
				 * The real handle is assigned in the callback of that call.*/
				xBTInterface.pxGattServerInterface->pxAddService( xBTInterface.ucServerIf,
																  &pxSrvcId,
																  pxServiceElem->usEndHandle + 1 );
				xEventGroupWaitBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete,
									 1 << eBLEHALEventSeviceAddedCb,
									 pdTRUE,
									 pdTRUE,
									 portMAX_DELAY );

				xStatus = xBTInterface.xCbStatus;
            }else
            {
                xStatus = eBTStatusFail;
            }

            /* Create Characteristics and associated descriptors. */
            if( xStatus == eBTStatusSuccess )
            {
                for( xIndex = 0; xIndex < pxService->xNbCharacteristics; xIndex++ )
                {
                    xStatus = prvCreateCharacteristicAndDescriptors( pxService, &pxService->pxCharacteristics[ xIndex ], pxServiceElem->pxAttributesPtr, &xAttributeCounter );

                    if( xStatus != eBTStatusSuccess )
                    {
                        break;
                    }
                }
            }

            /* Create Included Services. */
            if( xStatus == eBTStatusSuccess )
            {
                for( xIndex = 0; xIndex < pxService->xNbIncludedServices; xIndex++ )
                {
                    pxServiceElem->pxAttributesPtr[ xAttributeCounter ].xAttributeType = eBTDbIncludedService;
                    pxServiceElem->pxAttributesPtr[ xAttributeCounter++ ].pxIncludedService = &pxService->pxIncludedServices[ xIndex ];

                    pxService->pxIncludedServices[ xIndex ].xAttributeData.xHandle = 0;
                    xBTInterface.pxGattServerInterface->pxAddIncludedService( xBTInterface.ucServerIf,
                                                                              pxService->xAttributeData.xHandle,
                                                                              pxService->pxIncludedServices->pxPtrToService->xAttributeData.xHandle );
                    xEventGroupWaitBits( ( EventGroupHandle_t ) &xBTInterface.xWaitOperationComplete,
                                         1 << eBLEHALEventIncludedServiceAdded,
										 pdTRUE,
										 pdTRUE,
                                         portMAX_DELAY );

                    if( ( xBTInterface.xCbStatus != eBTStatusSuccess ) ||
                        ( pxService->pxIncludedServices[ xIndex ].xAttributeData.xHandle == 0 ) )
                    {
                        xStatus = eBTStatusFail;
                        break;
                    }
                }
            }

            xSemaphoreGive( ( SemaphoreHandle_t ) &xBTInterface.xThreadSafetyMutex );
        }
        else
        {
            xStatus = eBTStatusFail;
        }
    }
    else
    {
        xStatus = eBTStatusParamInvalid;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t BLE_StartService( BLEService_t * pxService,
                             BLEServiceStartedCallback_t pxCallback )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    if( pxService != NULL )
    {
        xBTInterface.pxPendingServiceEvent = ( void * ) pxCallback;
        xStatus = xBTInterface.pxGattServerInterface->pxStartService( xBTInterface.ucServerIf,
                                                                      pxService->xAttributeData.xHandle,
                                                                      BTTransportLe );
    }
    else
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t BLE_StopService( BLEService_t * pxService,
                            BLEServiceStoppedCallback_t pxCallback )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    if( pxService != NULL )
    {
        xBTInterface.pxPendingServiceEvent = ( void * ) pxCallback;
        xStatus = xBTInterface.pxGattServerInterface->pxStopService( xBTInterface.ucServerIf,
                                                                     pxService->xAttributeData.xHandle );
    }
    else
    {
        xStatus = eBTStatusParamInvalid;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t BLE_DeleteService( BLEService_t * pxService,
                              BLEServiceDeletedCallback_t pxCallback )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    if( pxService != NULL )
    {
        xBTInterface.pxPendingServiceEvent = ( void * ) pxCallback;
        xStatus = xBTInterface.pxGattServerInterface->pxDeleteService( xBTInterface.ucServerIf,
                                                                       pxService->xAttributeData.xHandle );
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

BTStatus_t BLE_GetConnectionInfoList( Link_t ** ppxConnectionInfoList )
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
    Link_t * pxConnListIndex;

    listFOR_EACH( pxConnListIndex, &xBTInterface.xConnectionListHead )
    {
        pxConnInfoListElem = listCONTAINER( pxConnListIndex, BLEConnectionInfoListElement_t, xConnectionList );

        if( usConnId == pxConnInfoListElem->usConnId )
        {
            xStatus = eBTStatusSuccess;
            *ppvConnectionInfo = pxConnInfoListElem;
            break;
        }
    }

    return xStatus;
}
