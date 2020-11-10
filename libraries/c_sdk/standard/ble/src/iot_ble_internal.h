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
 * @file aws_ble_internal.h
 * @brief Sharing internal variables and function
 */

#ifndef _AWS_BLE_INTERNALS_H_
#define _AWS_BLE_INTERNALS_H_

#include "iot_linear_containers.h"

/* Place holder for saving Char descriptors into memory. */
typedef struct
{
    uint8_t data;
    BTUuid_t charDescrUuid;
    BTUuid_t parentServiceUuid;
    BTUuid_t parentCharacteristicUuid;
    uint8_t instId;
} _bleCharDescriptorReccord_t;

typedef struct
{
    BTBdaddr_t bdAddr;
    size_t charDescriptorListSize;
    _bleCharDescriptorReccord_t pCharDescriptorList[];
} _bleBondingDataFile_t;

typedef struct
{
    IotLink_t characDescrList;
    BTCharacteristicDescr_t * pCharDescr;
} _bleCharDescriptorListElement_t;
/* End of place holder. */

typedef struct
{
    IotLink_t serviceList;
    BTService_t * pService;
    IotBleAttributeEventCallback_t * pEventsCallbacks;
    uint16_t endHandle;
} BLEServiceListElement_t;

typedef struct
{
    IotLink_t eventList;
    IotBleEventsCallbacks_t subscribedEventCb;
} _bleSubscrEventListElement_t;

typedef struct
{
    void * pPendingServiceEvent;

    /*  @TODO prepare write works for only on  one simultaneous connection */
    /*  @TODO pending indication response with a global only works for one simultaneous connection */
    uint16_t handlePendingIndicationResponse;
    uint8_t serverIf;
    IotListDouble_t serviceListHead;
    IotListDouble_t connectionListHead;
    IotListDouble_t subscrEventListHead[ eNbEvents ]; /**< Any task can subscribe to events in that array, several callback can subscribe to the same event */
    uint16_t handlePendingPrepareWrite;
    IotBle_StartAdvCallback_t pStartAdvCb;
    IotBle_StopAdvCallback_t pStopAdvCb;
    BTInterface_t * pBTInterface;
    BTBleAdapter_t * pBTLeAdapterInterface;
    BTGattServerInterface_t * pGattServerInterface;
    uint8_t adapterIf;
    IotMutex_t threadSafetyMutex;
    IotMutex_t waitCbMutex;
    IotSemaphore_t callbackSemaphore;
    BTStatus_t cbStatus;
} _bleInterface_t;
extern _bleInterface_t _BTInterface;

extern const BTGattServerCallbacks_t _BTGattServerCb;
extern void _bleStartAdvCb( BTStatus_t status );


#endif /* ifndef _AWS_BLE_INTERNALS_H_ */
