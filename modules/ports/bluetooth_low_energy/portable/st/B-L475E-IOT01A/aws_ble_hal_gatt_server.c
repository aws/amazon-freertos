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
 * @file aws_ble_hal_gatt_server.c
 * @brief Hardware Abstraction Layer for GATT server ble stack.
 */

#include <string.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "aws_ble_hal_common.h"

#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "aws_ble_event_manager.h"
#include "aws_ble_hal_internals.h"
#include "aws_doubly_linked_list.h"

#define MAX_NUM_CHARS 	128

extern uint16_t connection_handle;
uint16_t lastAddedCharIdx = 0;				// Index of last added char
BTBdaddr_t btadd;

typedef struct{
	uint16_t Conn_Handle_To_Notify;
	uint16_t Service_Handle;
	uint16_t Char_Handle;
	uint8_t Update_Type;
	uint16_t Char_Length;
	uint16_t Value_Offset;
	uint8_t Value_Length;
	uint8_t Value[220];
}indicateValue_t;
indicateValue_t indicateValue;

indicateValue_t * queueBuffer;
#define QUEUE_LENGTH    1
#define ITEM_SIZE       sizeof( indicateValue_t *)


enum{
	WRITE_RESPONSE,
	READ_RESPONSE
};

typedef enum{
	ATTR_TYPE_SERVICE,
	ATTR_TYPE_CHAR,
	ATTR_TYPE_CHAR_DECL,
	ATTR_TYPE_DESCR,
}attributeTypes;

typedef struct{
	uint16_t usHandle;
	uint16_t usAttributeType;
	uint16_t Char_Handle; /* if it is a descriptor */
}AttributeElement_t;

typedef struct{
	Link_t xServiceList;
	AttributeElement_t * xAttributes;
	size_t xNbElements;
	uint16_t usLastCharHandle;
	uint16_t usStartHandle;
	uint16_t usEndHandle;
}serviceListElement_t;

Link_t xServiceListHead;
static StaticSemaphore_t xThreadSafetyMutex;
static StaticSemaphore_t xIndicationComplete;
static StaticQueue_t xIndicateQueue;

static BTGattServerCallbacks_t xGattServerCb;

uint32_t ulGattServerIFhandle;
bool bInterfaceCreated = false;

void prvCopytoBlueNRGCharUUID(Char_UUID_t * pxBlueNRGCharuuid, BTUuid_t * pxUuid);
void prvCopytoBlueNRGDescrUUID(Char_Desc_Uuid_t * pxBlueNRGDescruuid, BTUuid_t * pxUuid);
void prvCopytoBlueNRGServUUID(Service_UUID_t * pxBlueNRGuuid, BTUuid_t * pxUuid);

static BTStatus_t prvBTRegisterServer(BTUuid_t * pxUuid);
static BTStatus_t prvBTUnregisterServer(uint8_t ucServerIf);
static BTStatus_t prvBTGattServerInit(const BTGattServerCallbacks_t * pxCallbacks);
static BTStatus_t prvBTConnect(uint8_t ucServerIf, const BTBdaddr_t * pxBdAddr, bool bIsDirect, BTTransport_t xTransport);
static BTStatus_t prvBTDisconnect(uint8_t ucServerIf, const BTBdaddr_t * pxBdAddr, uint16_t usConnId);
static BTStatus_t prvBTAddService(uint8_t ucServerIf, BTGattSrvcId_t * pxSrvcId, uint16_t usNumHandles);
static BTStatus_t prvBTAddIncludedService(uint8_t ucServerIf, uint16_t usServiceHandle, uint16_t usIncludedHandle);
static BTStatus_t prvBTAddCharacteristic(uint8_t ucServerIf, uint16_t usServiceHandle, BTUuid_t * pxUuid, BTCharProperties_t xProperties, BTCharPermissions_t xPermissions);
static BTStatus_t prvBTSetVal(BTGattResponse_t * pxValue);
static BTStatus_t prvBTAddDescriptor(uint8_t ucServerIf, uint16_t usServiceHandle, BTUuid_t * pxUuid, BTCharPermissions_t xPermissions);
static BTStatus_t prvBTStartService(uint8_t ucServerIf, uint16_t usServiceHandle, BTTransport_t xTransport);
static BTStatus_t prvBTStopService(uint8_t ucServerIf, uint16_t usServiceHandle);
static BTStatus_t prvBTDeleteService(uint8_t ucServerIf, uint16_t usServiceHandle);
static BTStatus_t prvBTSendIndication(uint8_t ucServerIf, uint16_t usAttributeHandle, uint16_t usConnId, size_t xLen, uint8_t * pucValue, bool bConfirm);
static BTStatus_t prvBTSendResponse(uint16_t usConnId, uint32_t ulTransId, BTStatus_t xStatus, BTGattResponse_t * pxResponse);

static BTGattServerInterface_t xGATTserverInterface =
{
		.pxRegisterServer = prvBTRegisterServer,
		.pxUnregisterServer = prvBTUnregisterServer,
		.pxGattServerInit = prvBTGattServerInit,
		.pxConnect = prvBTConnect,
		.pxDisconnect = prvBTDisconnect,
		.pxAddService = prvBTAddService,
		.pxAddIncludedService = prvBTAddIncludedService,
		.pxAddCharacteristic = prvBTAddCharacteristic,
		.pxSetVal = prvBTSetVal,
		.pxAddDescriptor = prvBTAddDescriptor,
		.pxStartService = prvBTStartService,
		.pxStopService = prvBTStopService,
		.pxDeleteService = prvBTDeleteService,
		.pxSendIndication = prvBTSendIndication,
		.pxSendResponse = prvBTSendResponse
};

serviceListElement_t * prvGetService(uint16_t usHandle)
{
    Link_t * pxTmpElem;
    serviceListElement_t * pxServiceElem = NULL;

    /* The service that was just added is the last in the list */
	if( xSemaphoreTake( ( SemaphoreHandle_t ) &xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
	{
	    listFOR_EACH( pxTmpElem, &xServiceListHead )
	    {
	        pxServiceElem = listCONTAINER( pxTmpElem, serviceListElement_t, xServiceList );

	        if(( usHandle >= pxServiceElem->usStartHandle )&&(usHandle <= pxServiceElem->usEndHandle ))
	        {
	            break;
	        }

	        pxServiceElem = NULL;
	    }
		xSemaphoreGive( ( SemaphoreHandle_t ) &xThreadSafetyMutex );
	}

    return pxServiceElem;
}


void prvCopytoBlueNRGCharUUID(Char_UUID_t * pxBlueNRGCharuuid, BTUuid_t * pxUuid) {
	switch (pxUuid->ucType) {
	case eBTuuidType16:
		pxBlueNRGCharuuid->Char_UUID_16 = pxUuid->uu.uu16;
		break;
	case eBTuuidType128:
	default:
		Osal_MemCpy(&pxBlueNRGCharuuid->Char_UUID_128, pxUuid->uu.uu128, 16);
		break;
	}
}

void prvCopytoBlueNRGDescrUUID(Char_Desc_Uuid_t * pxBlueNRGDescruuid, BTUuid_t * pxUuid) {
	switch (pxUuid->ucType) {
	case eBTuuidType16:
		pxBlueNRGDescruuid->Char_UUID_16 = pxUuid->uu.uu16;
		break;
	case eBTuuidType128:
	default:
		Osal_MemCpy(&pxBlueNRGDescruuid->Char_UUID_128, pxUuid->uu.uu128, 16);
		break;
	}
}

void prvCopytoBlueNRGServUUID(Service_UUID_t * pxBlueNRGServuuid, BTUuid_t * pxUuid) {
	switch (pxUuid->ucType) {
	case eBTuuidType16:
		pxBlueNRGServuuid->Service_UUID_16 = pxUuid->uu.uu16;
		break;
	case eBTuuidType128:
	default:
		Osal_MemCpy(&pxBlueNRGServuuid->Service_UUID_128, pxUuid->uu.uu128, 16);
		break;
	}
}

//void prvGATTeventHandler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
//	BTStatus_t xStatus = eBTStatusSuccess;
//	void * pvEventParams = NULL;
//	bool bFoundEvent;
//
//	vEVTMNGRgetEventParameters(ulGATTEvtMngHandle, event, &pvEventParams, &bFoundEvent);
//
//	configPRINTF(( "GATT EVent %d\n", event));
//
//	switch (event) {
//	case ESP_GATTS_REG_EVT:
//		if (bFoundEvent == true) {
//			ulGattServerIFhandle = gatts_if;
//			if (xGattServerCb.pxRegisterServerCb != NULL) {
//				if (param->reg.status != ESP_OK) {
//					xStatus = eBTStatusFail;
//					bInterfaceCreated = false;
//				} else {
//					bInterfaceCreated = true;
//				}
//				xGattServerCb.pxRegisterServerCb(xStatus, ulGattServerIFhandle, (BTUuid_t *) pvEventParams);
//			}
//		}
//		break;
//	case ESP_GATTS_CREATE_EVT:
//		if (bFoundEvent == true) {
//			if (xGattServerCb.pxServiceAddedCb != NULL) {
//				if (param->create.status != ESP_OK) {
//					xStatus = eBTStatusFail;
//				}
//				xGattServerCb.pxServiceAddedCb(xStatus, ulGattServerIFhandle, (BTGattSrvcId_t *) pvEventParams, param->create.service_handle);
//			}
//		}
//		break;
//	case ESP_GATTS_ADD_INCL_SRVC_EVT:
//		if (xGattServerCb.pxIncludedServiceAddedCb != NULL) {
//			if (param->add_incl_srvc.status != ESP_OK) {
//				xStatus = eBTStatusFail;
//			}
//			xGattServerCb.pxIncludedServiceAddedCb(xStatus, ulGattServerIFhandle, param->add_incl_srvc.service_handle, param->add_incl_srvc.attr_handle);
//		}
//		break;
//	case ESP_GATTS_ADD_CHAR_EVT:
//		if (xGattServerCb.pxCharacteristicAddedCb != NULL) {
//			if (param->add_char.status != ESP_OK) {
//				xStatus = eBTStatusFail;
//			}
//			xGattServerCb.pxCharacteristicAddedCb(xStatus, ulGattServerIFhandle, (BTUuid_t *) &param->add_char.char_uuid.uuid, param->add_char.service_handle, param->add_char.attr_handle);
//		}
//		break;
//	case ESP_GATTS_ADD_CHAR_DESCR_EVT:
//		if (xGattServerCb.pxDescriptorAddedCb != NULL) {
//			if (param->add_char_descr.status != ESP_OK) {
//				xStatus = eBTStatusFail;
//			}
//			xGattServerCb.pxDescriptorAddedCb(xStatus, ulGattServerIFhandle, (BTUuid_t *) &param->add_char_descr.descr_uuid.uuid, param->add_char_descr.service_handle, param->add_char_descr.attr_handle);
//		}
//		break;
//	case ESP_GATTS_START_EVT:
//		if (xGattServerCb.pxServiceStartedCb != NULL) {
//			if (param->start.status != ESP_OK) {
//				xStatus = eBTStatusFail;
//			}
//			xGattServerCb.pxServiceStartedCb(xStatus, ulGattServerIFhandle, param->start.service_handle);
//		}
//		break;
//	case ESP_GATTS_STOP_EVT:
//		if (xGattServerCb.pxServiceStoppedCb != NULL) {
//			if (param->stop.status != ESP_OK) {
//				xStatus = eBTStatusFail;
//			}
//			xGattServerCb.pxServiceStoppedCb(xStatus, ulGattServerIFhandle, param->stop.service_handle);
//		}
//		break;
//	case ESP_GATTS_DELETE_EVT:
//		if (xGattServerCb.pxServiceDeletedCb != NULL) {
//			if (param->del.status != ESP_OK) {
//				xStatus = eBTStatusFail;
//			}
//			xGattServerCb.pxServiceDeletedCb(xStatus, ulGattServerIFhandle, param->del.service_handle);
//		}
//		break;
//	case ESP_GATTS_CONF_EVT:
//		if (xGattServerCb.pxIndicationSentCb != NULL) {
//			if (param->conf.status != ESP_OK) {
//				xStatus = eBTStatusFail;
//			}
//			xGattServerCb.pxIndicationSentCb(param->conf.conn_id, xStatus);
//		}
//		break;
//	case ESP_GATTS_RESPONSE_EVT:
//		if (xGattServerCb.pxResponseConfirmationCb != NULL) {
//			if (param->rsp.status != ESP_OK) {
//				xStatus = eBTStatusFail;
//			}
//			xGattServerCb.pxResponseConfirmationCb(xStatus, param->rsp.handle);
//		}
//		break;
//	case ESP_GATTS_UNREG_EVT:
//		vEVTMNGRsetEvent(ulGATTEvtMngHandle, event, xStatus);
//		break;
//	case ESP_GATTS_CONNECT_EVT: OK
//		if (xGattServerCb.pxConnectionCb != NULL) {
//			xGattServerCb.pxConnectionCb(param->connect.conn_id, ulGattServerIFhandle, true, (BTBdaddr_t *) &param->connect.remote_bda);
//		}
//		break;
//	case ESP_GATTS_DISCONNECT_EVT: OK
//		if (xGattServerCb.pxConnectionCb != NULL) {
//			xGattServerCb.pxConnectionCb(param->disconnect.conn_id, ulGattServerIFhandle, false, (BTBdaddr_t *) &param->disconnect.remote_bda);
//		}
//		break;
//	case ESP_GATTS_READ_EVT:
//		if (xGattServerCb.pxRequestReadCb != NULL) {
//			xGattServerCb.pxRequestReadCb(param->read.conn_id, param->read.trans_id, (BTBdaddr_t *) &param->read.bda, param->read.handle, param->read.offset);
//		}
//		break;
//	case ESP_GATTS_WRITE_EVT:
//		if (xGattServerCb.pxRequestWriteCb != NULL) {
//			xGattServerCb.pxRequestWriteCb(param->write.conn_id, param->write.trans_id, (BTBdaddr_t *) &param->write.bda, param->write.handle, param->write.offset, param->write.len, param->write.need_rsp, param->write.is_prep, param->write.value);
//		}
//		break;
//	case ESP_GATTS_EXEC_WRITE_EVT:
//		if (xGattServerCb.pxRequestExecWriteCb != NULL) {
//			xGattServerCb.pxRequestExecWriteCb(param->exec_write.conn_id, param->exec_write.trans_id, (BTBdaddr_t *) &param->exec_write.bda, param->exec_write.exec_write_flag);
//		}
//		break;
//	case ESP_GATTS_MTU_EVT:
//		if (xGattServerCb.pxMtuChangedCb != NULL) {
//			xGattServerCb.pxMtuChangedCb(param->mtu.conn_id, param->mtu.mtu);
//		}
//		break;
//	case ESP_GATTS_SET_ATTR_VAL_EVT:
//		if (xGattServerCb.pxSetValCallbackCb != NULL) {
//			if (param->set_attr_val.status != ESP_OK) {
//				xStatus = eBTStatusFail;
//			}
//			xGattServerCb.pxSetValCallbackCb(xStatus, param->set_attr_val.attr_handle);
//		}
//		break;
//	default:
//		break;
//	}
//
//	if (pvEventParams != NULL) {
//		(void) vPortFree(pvEventParams);
//	}
//}


void hci_le_connection_complete_event(uint8_t Status, uint16_t Connection_Handle, uint8_t Role, uint8_t Peer_Address_Type, uint8_t Peer_Address[6], uint16_t Conn_Interval, uint16_t Conn_Latency, uint16_t Supervision_Timeout, uint8_t Master_Clock_Accuracy) {
	if (xGattServerCb.pxConnectionCb != NULL) {
		connection_handle = Connection_Handle;
		Osal_MemCpy(&btadd.ucAddress, Peer_Address, 6);
		xGattServerCb.pxConnectionCb(Connection_Handle, 0, true, &btadd);
	}
}


void hci_disconnection_complete_event(uint8_t Status, uint16_t Connection_Handle, uint8_t Reason) {

	if (xGattServerCb.pxConnectionCb != NULL) {
		uint8_t cleanAddr[6] = { 0, 0, 0, 0, 0, 0 };
		xGattServerCb.pxConnectionCb(Connection_Handle, 0, false, &btadd);
		Osal_MemCpy(&btadd.ucAddress, cleanAddr, 6);
	}
}

void aci_gatt_read_permit_req_event(uint16_t Connection_Handle,
                                    uint16_t Attribute_Handle,
                                    uint16_t Offset)
{
	xGattServerCb.pxRequestReadCb(Connection_Handle, READ_RESPONSE, &btadd, Attribute_Handle, Offset);
}
char bufferWriteResponse[220];
void aci_gatt_write_permit_req_event(uint16_t Connection_Handle,
                                     uint16_t Attribute_Handle,
                                     uint8_t Data_Length,
                                     uint8_t Data[])
{
	memcpy(bufferWriteResponse, Data, Data_Length);
	xGattServerCb.pxRequestWriteCb(Connection_Handle, WRITE_RESPONSE, &btadd, Attribute_Handle, 0, Data_Length, true, false, Data);
}

void aci_att_exchange_mtu_resp_event(uint16_t Connection_Handle, uint16_t Server_RX_MTU) {

	if (xGattServerCb.pxMtuChangedCb != NULL) {
		xGattServerCb.pxMtuChangedCb(Connection_Handle, Server_RX_MTU);
	}

}

BTStatus_t prvBTRegisterServer(BTUuid_t * pxUuid) {
	BTStatus_t xStatus = eBTStatusSuccess;
	xGattServerCb.pxRegisterServerCb(xStatus, ulGattServerIFhandle, pxUuid);
	return eBTStatusSuccess;
}

BTStatus_t prvBTUnregisterServer(uint8_t ucServerIf) {
	BTStatus_t xStatus = eBTStatusUnsupported;

//	if (bInterfaceCreated == false) {
//		if (esp_ble_gatts_app_unregister(ulGattServerIFhandle) != ESP_OK) {
//			xStatus = eBTStatusFail;
//		} else {
//			if (vEVTMNGRwaitEvent(ulGATTEvtMngHandle, ESP_GATTS_UNREG_EVT) != ESP_OK) {
//				xStatus = eBTStatusFail;
//			} else {
//				bInterfaceCreated = false;
//			}
//		}
//	}

	return xStatus;
}

BTStatus_t prvBTGattServerInit(const BTGattServerCallbacks_t * pxCallbacks) {
	BTStatus_t xStatus = eBTStatusSuccess;
	if (xStatus == eBTStatusSuccess) {
		if (pxCallbacks != NULL) {
			xGattServerCb = *pxCallbacks;
			listINIT_HEAD( &xServiceListHead );
			( void ) xSemaphoreCreateMutexStatic( &xThreadSafetyMutex );
			( void ) xSemaphoreCreateMutexStatic( &xIndicationComplete );
			xQueueCreateStatic( QUEUE_LENGTH,
			                                 ITEM_SIZE,
			                                 (void *)&queueBuffer,
			                                 &xIndicateQueue );
		} else {
			xStatus = eBTStatusParamInvalid;
		}
	}
	return xStatus;
}

BTStatus_t prvBTConnect(uint8_t ucServerIf, const BTBdaddr_t * pxBdAddr, bool bIsDirect, BTTransport_t xTransport) {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTDisconnect(uint8_t ucServerIf, const BTBdaddr_t * pxBdAddr, uint16_t usConnId) {
	return eBTStatusUnsupported;
}

BTStatus_t prvBTAddService(uint8_t ucServerIf, BTGattSrvcId_t * pxSrvcId, uint16_t usNumHandles) {

	Service_UUID_t xSrvUUID;
	uint8_t xSrvType = PRIMARY_SERVICE;
	uint8_t xSrv_UUID_Type = UUID_TYPE_128;
	uint16_t xServHandle = 0;
	uint8_t xBleStatus = BLE_STATUS_SUCCESS;
	serviceListElement_t * pxNewServiceElement;
	BTStatus_t xStatus = eBTStatusSuccess;

	if ((pxSrvcId->xServiceType) == eBTServiceTypeSecondary) {
		xSrvType = SECONDARY_SERVICE;
	}
	if (pxSrvcId->xId.xUuid.ucType == eBTuuidType16) {
		xSrv_UUID_Type = UUID_TYPE_16;
	}

	prvCopytoBlueNRGServUUID(&xSrvUUID, &pxSrvcId->xId.xUuid);
	xBleStatus = aci_gatt_add_service(xSrv_UUID_Type, &xSrvUUID, xSrvType, usNumHandles, &xServHandle);
	xGattServerCb.pxServiceAddedCb((xBleStatus == BLE_STATUS_SUCCESS) ? eBTStatusSuccess : eBTStatusFail, ucServerIf, pxSrvcId, xServHandle);

	if(xBleStatus == BLE_STATUS_SUCCESS)
	{
		pxNewServiceElement = pvPortMalloc( sizeof( serviceListElement_t ) );

		if( pxNewServiceElement != NULL )
		{
			pxNewServiceElement->usStartHandle = xServHandle;
			pxNewServiceElement->usEndHandle = xServHandle;
			pxNewServiceElement->xAttributes = pvPortMalloc( sizeof( AttributeElement_t )*usNumHandles );
			pxNewServiceElement->xNbElements = usNumHandles;
			pxNewServiceElement->usLastCharHandle = 0;

			if(pxNewServiceElement->xAttributes != NULL)
			{
				if( xSemaphoreTake( ( SemaphoreHandle_t ) &xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
				{
					listADD( &xServiceListHead,  &pxNewServiceElement->xServiceList );
					xSemaphoreGive( ( SemaphoreHandle_t ) &xThreadSafetyMutex );
				}
			}else
			{
				xStatus = eBTStatusFail;
			}
		}
		else
		{
			xStatus = eBTStatusFail;
		}
	}else
	{
		xStatus = eBTStatusFail;
	}

	return xStatus;
}

BTStatus_t prvBTAddIncludedService(uint8_t ucServerIf, uint16_t usServiceHandle, uint16_t usIncludedHandle) {
//	esp_err_t xESPstatus = ESP_OK;
	BTStatus_t xStatus = eBTStatusSuccess;
//



//	aci_gatt_include_service(usServiceHandle, )
//
//	tBleStatus aci_gatt_include_service(uint16_t Service_Handle,
//	                                    uint16_t Include_Start_Handle,
//	                                    uint16_t Include_End_Handle,
//	                                    uint8_t Include_UUID_Type,
//	                                    Include_UUID_t *Include_UUID,
//	                                    uint16_t *Include_Handle)
//
//	aci_gatt_include_service()

//	xESPstatus = esp_ble_gatts_add_included_service(usServiceHandle, usIncludedHandle);
//	if (xESPstatus != ESP_OK) {
//		xStatus = eBTStatusFail;
//	}

	return xStatus;
}

BTStatus_t prvConvertPermtoSTPerm(BTCharPermissions_t xPermissions, uint8_t * Security_Permissions )
{
	BTStatus_t xStatus = eBTStatusSuccess;
	*Security_Permissions = ATTR_PERMISSION_AUTHOR_READ|ATTR_PERMISSION_AUTHOR_WRITE;

	if(( xPermissions & eBTPermReadEncrypted) != 0)
	{
		*Security_Permissions |= ATTR_PERMISSION_ENCRY_READ;
	}

	if(( xPermissions & eBTPermReadEncryptedMitm) != 0)
	{
		*Security_Permissions |= ATTR_PERMISSION_AUTHEN_READ;
		*Security_Permissions |= ATTR_PERMISSION_ENCRY_READ; // RR 11/21 added
	}

	if(( xPermissions = eBTPermWriteEncrypted) != 0)
	{
		*Security_Permissions |= ATTR_PERMISSION_ENCRY_WRITE;
	}

	if(( xPermissions & eBTPermWriteEncryptedMitm) != 0)
	{
		*Security_Permissions |= ATTR_PERMISSION_AUTHEN_WRITE;
		*Security_Permissions |= ATTR_PERMISSION_ENCRY_WRITE; // RR 11/21 added
	}

	if(( xPermissions & eBTPermWriteSigned) != 0)
	{
		xStatus = eBTStatusUnsupported;
	}

	if(( xPermissions & eBTPermWriteSignedMitm) != 0)
	{
		xStatus = eBTStatusUnsupported;
	}

	return xStatus;
}

BTStatus_t prvBTAddCharacteristic(uint8_t ucServerIf, uint16_t usServiceHandle, BTUuid_t * pxUuid, BTCharProperties_t xProperties, BTCharPermissions_t xPermissions) {

	Char_UUID_t xCharUUID;
	uint8_t xChar_UUID_Type = UUID_TYPE_128;
	uint8_t xBleStatus = BLE_STATUS_SUCCESS;
	uint16_t xCharHandle = 0;
	uint8_t Char_Value_Length = 24;
	serviceListElement_t * xServiceElement;
	uint16_t usAttributeIndex;
	BTStatus_t xStatus = eBTStatusSuccess;
	uint8_t Security_Permissions;
	uint8_t GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP | GATT_NOTIFY_ATTRIBUTE_WRITE;

	if (pxUuid->ucType == eBTuuidType16) {
		xChar_UUID_Type = UUID_TYPE_16;
	}

	prvCopytoBlueNRGCharUUID(&xCharUUID, pxUuid);
	xStatus = prvConvertPermtoSTPerm(xPermissions, &Security_Permissions);
	if(xStatus == eBTStatusSuccess)
	{
		if((pxUuid->uu.uu128[0] == 0x02)&&(pxUuid->uu.uu128[1] == 0xFF)&&(pxUuid->uu.uu128[2] == 0xC3))
		{
			Char_Value_Length = 170;
		}
		if((pxUuid->uu.uu128[0] == 0x03)&&(pxUuid->uu.uu128[1] == 0xFF)&&(pxUuid->uu.uu128[2] == 0xC3))
		{
			Char_Value_Length = 170;
		}

		xBleStatus = aci_gatt_add_char(usServiceHandle, xChar_UUID_Type, &xCharUUID, Char_Value_Length, (uint8_t) xProperties, (uint8_t) Security_Permissions, GATT_Evt_Mask, 16, 1, &xCharHandle);
		xCharHandle++; /* The handle of the char declaration is returned, the char value is after. */

		xGattServerCb.pxCharacteristicAddedCb((xBleStatus == BLE_STATUS_SUCCESS) ? eBTStatusSuccess : eBTStatusFail, ucServerIf, pxUuid, usServiceHandle, xCharHandle);

		if(xBleStatus == BLE_STATUS_SUCCESS)
		{
			xServiceElement = prvGetService(usServiceHandle);

			if(xServiceElement == NULL)
			{
				xStatus = eBTStatusFail;
			}
		}else
		{
			xStatus = eBTStatusFail;
		}
	}

	if(xStatus == eBTStatusSuccess)
	{
		if( xSemaphoreTake( ( SemaphoreHandle_t ) &xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
		{
			usAttributeIndex = xServiceElement->usEndHandle - xServiceElement->usStartHandle + 1;

			if(usAttributeIndex < xServiceElement->xNbElements)
			{
				xServiceElement->xAttributes[usAttributeIndex].usAttributeType = ATTR_TYPE_CHAR_DECL;
				xServiceElement->xAttributes[usAttributeIndex].usHandle = xCharHandle - 1;

				xServiceElement->xAttributes[usAttributeIndex + 1].usAttributeType = ATTR_TYPE_CHAR;
				xServiceElement->xAttributes[usAttributeIndex + 1].usHandle = xCharHandle;
				xServiceElement->usEndHandle += 2;

				xServiceElement->usLastCharHandle = xCharHandle;
			}else
			{
				xStatus = eBTStatusFail;
			}

			xSemaphoreGive( ( SemaphoreHandle_t ) &xThreadSafetyMutex );
		}else
		{
			xStatus = eBTStatusFail;
		}
	}else
	{
		xStatus = eBTStatusFail;
	}

	return xStatus;
}

BTStatus_t prvBTSetVal(BTGattResponse_t * pxValue) {

	uint8_t xBleStatus = BLE_STATUS_ERROR;
/*
	xServiceElement = prvGetService(pxResponse->xAttrValue.usHandle);
	usServiceHandle = xServiceElement->usStartHandle;
	offset = pxResponse->xAttrValue.usHandle - xServiceElement->usStartHandle;
	xBleStatus = aci_gatt_update_char_value_ext(connection_handle, xServiceHandle, pxValue->usHandle, xUpdate_Type, xCharLength, pxValue->xAttrValue.usOffset, pxValue->xAttrValue.xLen, pxValue->xAttrValue.pucValue);
*/
	return (xBleStatus == BLE_STATUS_SUCCESS) ? eBTStatusSuccess : eBTStatusFail;
}

BTStatus_t prvBTAddDescriptor(uint8_t ucServerIf, uint16_t usServiceHandle, BTUuid_t * pxUuid, BTCharPermissions_t xPermissions) {

	uint8_t xBleStatus = BLE_STATUS_ERROR;
	BTStatus_t xStatus = eBTStatusSuccess;
	Char_Desc_Uuid_t xDescrUUID;
	uint16_t xDescrHandle = 0;
	uint8_t xDescr_UUID_Type = UUID_TYPE_128;
	uint8_t xChar_Desc_Value_Max_Len = 1;
	uint8_t xChar_Desc_Value_Length = 1;
	uint8_t xAccess_Permissions = ATTR_ACCESS_READ_WRITE | ATTR_ACCESS_WRITE_WITHOUT_RESPONSE | ATTR_ACCESS_SIGNED_WRITE_ALLOWED; // Default
	uint8_t xGATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP | GATT_NOTIFY_ATTRIBUTE_WRITE;
	serviceListElement_t * xServiceElement;
	uint16_t usAttributeIndex;
	uint8_t Security_Permissions;

	if (pxUuid->ucType == eBTuuidType16) {
		xDescr_UUID_Type = UUID_TYPE_16;
	}

	prvCopytoBlueNRGDescrUUID(&xDescrUUID, pxUuid);
	xServiceElement = prvGetService(usServiceHandle);

	if(xServiceElement == NULL)
	{
		xStatus = eBTStatusFail;
	}

	if(xStatus == eBTStatusSuccess)
	{
		xStatus = prvConvertPermtoSTPerm(xPermissions, &Security_Permissions);
	}

	if(xStatus == eBTStatusSuccess)
	{
		if( xSemaphoreTake( ( SemaphoreHandle_t ) &xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
		{
			usAttributeIndex = xServiceElement->usEndHandle - xServiceElement->usStartHandle + 1;
			if(usAttributeIndex < xServiceElement->xNbElements)
			{
				xServiceElement->xAttributes[usAttributeIndex].usAttributeType = ATTR_TYPE_DESCR;

				if (((pxUuid->ucType == eBTuuidType16) && (xDescrUUID.Char_UUID_16 == 0x2902))||
					((pxUuid->ucType == eBTuuidType128) && (((xDescrUUID.Char_UUID_128[2]<<8) +xDescrUUID.Char_UUID_128[3])  == 0x2902)))
				{
					xDescrHandle = xServiceElement->xAttributes[usAttributeIndex - 1].usHandle + 1;
					xServiceElement->xAttributes[usAttributeIndex].usHandle = xDescrHandle;
					xServiceElement->xAttributes[usAttributeIndex].Char_Handle = xServiceElement->usLastCharHandle;
				} else {
					xBleStatus = aci_gatt_add_char_desc(usServiceHandle, xServiceElement->usLastCharHandle, xDescr_UUID_Type, &xDescrUUID, xChar_Desc_Value_Max_Len, xChar_Desc_Value_Length, NULL, (uint8_t) Security_Permissions, xAccess_Permissions, xGATT_Evt_Mask, 16, 1, &xDescrHandle);
					if (xBleStatus == BLE_STATUS_SUCCESS)
					{
						xServiceElement->xAttributes[usAttributeIndex].usHandle = xDescrHandle;
						xServiceElement->xAttributes[usAttributeIndex].Char_Handle = xServiceElement->usLastCharHandle;
					}
					else
						xStatus = eBTStatusFail;
				}

				xServiceElement->usEndHandle++;
			}else
			{
				xStatus = eBTStatusFail;
			}


			xSemaphoreGive( ( SemaphoreHandle_t ) &xThreadSafetyMutex );
		}

		xGattServerCb.pxDescriptorAddedCb(xStatus, ucServerIf, pxUuid, usServiceHandle, xDescrHandle);
	}

	return xStatus;
}

BTStatus_t prvBTStartService(uint8_t ucServerIf, uint16_t usServiceHandle, BTTransport_t xTransport) {
	xGattServerCb.pxServiceStartedCb(eBTStatusSuccess, ucServerIf, usServiceHandle);
	return eBTStatusSuccess;
}

BTStatus_t prvBTStopService(uint8_t ucServerIf, uint16_t usServiceHandle) {
	xGattServerCb.pxServiceStoppedCb(eBTStatusSuccess, ucServerIf, usServiceHandle);
	return eBTStatusSuccess;
}

BTStatus_t prvBTDeleteService(uint8_t ucServerIf, uint16_t usServiceHandle) {

	BTStatus_t xStatus = eBTStatusSuccess;
	uint8_t xBleStatus = BLE_STATUS_SUCCESS;
	serviceListElement_t * xServiceElement;

	xBleStatus = aci_gatt_del_service(usServiceHandle);

	if (xBleStatus != BLE_STATUS_SUCCESS) {
		xStatus = eBTStatusFail;
	}

	if( xStatus == eBTStatusSuccess)
	{
		xServiceElement = prvGetService(usServiceHandle);

		if(xServiceElement != NULL)
		{
		    if( xSemaphoreTake( ( SemaphoreHandle_t ) &xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
		    {
				listREMOVE( &xServiceElement->xServiceList );
				vPortFree( xServiceElement->xAttributes );
				vPortFree( xServiceElement );
				( void ) xSemaphoreGive( ( SemaphoreHandle_t ) &xThreadSafetyMutex );
		    }
		}else
		{
			xStatus = eBTStatusFail;
		}
	}

	return xStatus;
}

/*
tBleStatus prvchunkUpdateChar(uint16_t Conn_Handle_To_Notify,
        uint16_t Service_Handle,
        uint16_t Char_Handle,
        uint8_t Update_Type,
        uint16_t Char_Length,
        uint16_t Value_Offset,
        uint8_t Value_Length,
        uint8_t Value[])
{
	int offset = 0;
	tBleStatus xBleStatus = 0;
	const size_t chunckSize = 20;

	do{
		if(offset + chunckSize < Value_Length)
		{
			aci_gatt_update_char_value_ext(Conn_Handle_To_Notify, Service_Handle, Char_Handle, 0, Char_Length, offset, chunckSize, &Value[offset]);
		}else
		{
			xBleStatus = aci_gatt_update_char_value_ext(Conn_Handle_To_Notify, Service_Handle, Char_Handle, Update_Type, Char_Length, offset, Value_Length - offset, &Value[offset]);
		}

		offset+= chunckSize;
	}while(offset < Value_Length);

	return xBleStatus;
}
*/
void AppTick(void)
{
	indicateValue_t * pxIndicateValue;
	if(xQueueReceive((QueueHandle_t )&xIndicateQueue, &pxIndicateValue, 0) == pdTRUE)
	{
    	aci_gatt_update_char_value_ext(pxIndicateValue->Conn_Handle_To_Notify,
    			pxIndicateValue->Service_Handle,
				pxIndicateValue->Char_Handle,
				pxIndicateValue->Update_Type,
				pxIndicateValue->Char_Length,
				pxIndicateValue->Value_Offset,
				pxIndicateValue->Value_Length,
				pxIndicateValue->Value);
	}
}

BTStatus_t prvBTSendIndication(uint8_t ucServerIf, uint16_t usAttributeHandle, uint16_t usConnId, size_t xLen, uint8_t * pucValue, bool bConfirm) {

	uint8_t xBleStatus = BLE_STATUS_ERROR;
	uint8_t xUpdate_Type = 0x00;
	uint16_t usServiceHandle;
	serviceListElement_t * xServiceElement;

	xServiceElement = prvGetService(usAttributeHandle);
	usServiceHandle = xServiceElement->usStartHandle;

	if (bConfirm == true)
		xUpdate_Type = 0x02; // Indication
	else
		xUpdate_Type = 0x01; // Notification

    if( xSemaphoreTake( ( SemaphoreHandle_t ) &xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
		indicateValue.Conn_Handle_To_Notify = usConnId;
		indicateValue.Service_Handle = usServiceHandle;
		indicateValue.Char_Handle = usAttributeHandle - 1;
		indicateValue.Update_Type = xUpdate_Type;
		indicateValue.Char_Length = xLen;
		indicateValue.Value_Offset = 0;
		indicateValue.Value_Length = xLen;
		memcpy(indicateValue.Value, pucValue, xLen);
		indicateValue_t * pxIndicateValue = &indicateValue;
		xQueueSend((QueueHandle_t )&xIndicateQueue, (void *)&pxIndicateValue, portMAX_DELAY);
		xBleStatus  = BLE_STATUS_SUCCESS;
		//xBleStatus = aci_gatt_update_char_value_ext(usConnId, usServiceHandle, usAttributeHandle - 1, xUpdate_Type, xLen, 0, xLen, pucValue);
    	//xBleStatus = prvchunkUpdateChar(usConnId, usServiceHandle, usAttributeHandle - 1, xUpdate_Type, xLen, 0, xLen, pucValue);
    	( void ) xSemaphoreGive( ( SemaphoreHandle_t ) &xThreadSafetyMutex );
    }
	return (xBleStatus == BLE_STATUS_SUCCESS) ? eBTStatusSuccess : eBTStatusFail;
}


BTStatus_t prvBTSendResponse(uint16_t usConnId, uint32_t ulTransId, BTStatus_t xStatus, BTGattResponse_t * pxResponse) {
//	esp_err_t xESPstatus = ESP_OK;
	tBleStatus xBleStatus = 0;
	BTStatus_t xReturnStatus = eBTStatusSuccess;
	uint16_t usServiceHandle;
	serviceListElement_t * xServiceElement;
	uint8_t xUpdate_Type = 0x00; // No notification or indication (local characteristic value update)
	uint16_t offset;

	xServiceElement = prvGetService(pxResponse->xAttrValue.usHandle);
	usServiceHandle = xServiceElement->usStartHandle;
	offset = pxResponse->xAttrValue.usHandle - xServiceElement->usStartHandle;
    if( xSemaphoreTake( ( SemaphoreHandle_t ) &xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
		switch(ulTransId)
		{
			case WRITE_RESPONSE:
			{
				//@TODO  status may not match
				memcpy(pxResponse->xAttrValue.pucValue, bufferWriteResponse, pxResponse->xAttrValue.xLen);
				xBleStatus = aci_gatt_write_resp(usConnId, pxResponse->xAttrValue.usHandle, xStatus, pxResponse->xAttrValue.xRspErrorStatus, pxResponse->xAttrValue.xLen, pxResponse->xAttrValue.pucValue);
				if(BLE_STATUS_SUCCESS != xBleStatus)
				{
					configPRINTF(("ERROR WRITTING MESSAGE\n"));

				}
				break;
			}
			case READ_RESPONSE:
			{
				if(xServiceElement->xAttributes[offset].usAttributeType == ATTR_TYPE_CHAR)
				{
					xBleStatus = aci_gatt_update_char_value_ext(usConnId, usServiceHandle, pxResponse->xAttrValue.usHandle - 1, xUpdate_Type, pxResponse->xAttrValue.xLen, 0, pxResponse->xAttrValue.xLen, pxResponse->xAttrValue.pucValue);
					//xBleStatus = prvchunkUpdateChar(usConnId, usServiceHandle, pxResponse->xAttrValue.usHandle - 1, xUpdate_Type, pxResponse->xAttrValue.xLen, 0, pxResponse->xAttrValue.xLen, pxResponse->xAttrValue.pucValue);
				}else
				{
					xBleStatus = aci_gatt_set_desc_value(usServiceHandle, pxResponse->xAttrValue.usHandle, xServiceElement->xAttributes[offset].Char_Handle, pxResponse->xAttrValue.usOffset, pxResponse->xAttrValue.xLen, pxResponse->xAttrValue.pucValue);
				}

				if(BLE_STATUS_SUCCESS == xBleStatus)
				{
					xBleStatus = aci_gatt_allow_read(usConnId);
				}else
				{
			    	configPRINTF(("ERROR\n"));
				}
				break;
			}
			default:;
		}
		( void ) xSemaphoreGive( ( SemaphoreHandle_t ) &xThreadSafetyMutex );
    }
	return (xBleStatus == BLE_STATUS_SUCCESS) ? eBTStatusSuccess : eBTStatusFail;
}

const void * prvBTGetGattServerInterface() {
	return &xGATTserverInterface;
}
