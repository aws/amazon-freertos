/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file aws_ble_hal_internals.h
 * @brief Internally shared functions and variable for HAL ble stack.
 */

#ifndef _BT_HAL_INTERNALS_
#define _BT_HAL_INTERNALS_

#include <icall.h>
#include <bcomdef.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include <icall_ble_api.h>

#include "bt_hal_manager.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_gatt_server.h"
#include "iot_ble_hal_mq_task.h"
#include "iot_ble_hal_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern BTBleAdapterCallbacks_t _xBTBleAdapterCallbacks;
extern BTGattServerCallbacks_t _xGattServerCallbacks;
extern BTCallbacks_t _xBTCallbacks;
extern uint32_t ulGattServerIFhandle;


void vProcessGapDeviceInit( gapDeviceInitDoneEvent_t *pxPkt );
const void * _prvBTGetGattServerInterface();
const void * _prvGetLeAdapter();
BTStatus_t xBTCleanUp();
BTStatus_t xBTStop();

#ifdef __cplusplus
}
#endif

#endif /* ifndef _BT_HAL_INTERNALS_ */
