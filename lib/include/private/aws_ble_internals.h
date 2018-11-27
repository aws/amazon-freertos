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
 * @file aws_ble_internals.h
 * @brief Sharing internal variables and function
 */

#ifndef _AWS_BLE_INTERNALS_H_
#define _AWS_BLE_INTERNALS_H_

#include "aws_doubly_linked_list.h"

/* Place holder for saving Char descriptors into memory. */
typedef struct {
	uint8_t ucData;
	BTUuid_t xCharDescrUuid;
	BTUuid_t xParentServiceUuid;
	BTUuid_t xParentCharacteristicUuid;
	uint8_t ucInstId;
}BLECharDescriptorReccord_t;

typedef struct {
	BTBdaddr_t pxBdAddr;
	size_t xCharDescriptorListSize;
	BLECharDescriptorReccord_t pxCharDescriptorList[];
}BLEBondingDataFile_t;

typedef struct {
	Link_t xCharacDescrList;
	BLECharacteristicDescr_t * pxCharDescr;
}BLECharDescriptorListElement_t;
/* End of place holder. */

typedef struct{
	Link_t xServiceList;
	BLEAttribute_t * pxAttributesPtr;
	BLEService_t * pxService;
	uint16_t usStartHandle;
	uint16_t usEndHandle;
}BLEServiceListElement_t;

typedef enum{
	eBLEHALEventServerRegisteredCb,
	eBLEHALEventCharAddedCb,
	eBLEHALEventSeviceAddedCb,
	eBLEHALEventCharDescrAddedCb,
	eBLEHALEventIncludedServiceAdded,
	eBLEHALEventRegisterBleAdapterCb,
	eBLEHALEventAdapterPropertiesCb,
	eBLENbHALEvents,
}BLEHALEventsInternals_t;

typedef struct
{
	BLEStartAdvCallback_t pxStartAdvCb;
	BLESetAdvDataCallback_t pxSetAdvDataCb;
}BLEInternalEventsCallbacks_t;

typedef struct{
	Link_t xEventList;
	BLEEventsCallbacks_t xSubscribedEventCb;
}BLESubscrEventListElement_t;

typedef struct{
	void * pxPendingServiceEvent;

	/*  @TODO prepare write works for only on  one simultaneous connection */
	/*  @TODO pending indication response with a global only works for one simultaneous connection */
	uint16_t usHandlePendingIndicationResponse;
	uint8_t ucServerIf;
	Link_t xServiceListHead;
	Link_t xConnectionListHead;
	Link_t xSubscrEventListHead[eNbEvents];    /**< Any task can subscribe to events in that array, several callback can subscribe to the same event */
	uint16_t usHandlePendingPrepareWrite;
	BLEInternalEventsCallbacks_t pxBLEEventsCallbacks;
	BTInterface_t * pxBTInterface;
	BTBleAdapter_t * pxBTLeAdapterInterface;
	BTGattServerInterface_t * pxGattServerInterface;
	uint8_t ucAdapterIf;
	StaticSemaphore_t xThreadSafetyMutex;
	StaticEventGroup_t xWaitOperationComplete;
	BTStatus_t xCbStatus;
}BLEInterface_t;
extern BLEInterface_t xBTInterface;

extern BTGattServerCallbacks_t xBTGattServerCb;

#endif
