/*
* FreeRTOS
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
 * @file iot_ble_hal_internals.h
 * @brief Internally shared functions and variable for HAL ble stack.
 */

#ifndef _AWS_BLE_INTERNALS_H_
#define _AWS_BLE_INTERNALS_H_

typedef struct
{
    BTIOtypes_t xPropertyIO;
    bool bBondable;
    bool bSecureConnectionOnly;
    uint32_t ulMtu;
} BTProperties_t;

extern BTBleAdapterCallbacks_t xBTBleAdapterCallbacks;
extern uint32_t ulGattServerIFhandle;
extern BTProperties_t xProperties;

const void * prvBTGetGattServerInterface();
const void * prvGetLeAdapter();
void prvGAPeventHandler( esp_gap_ble_cb_event_t event,
                         esp_ble_gap_cb_param_t * param );
void prvGATTeventHandler( esp_gatts_cb_event_t event,
                          esp_gatt_if_t gatts_if,
                          esp_ble_gatts_cb_param_t * param );
void prvCopytoESPUUID( esp_bt_uuid_t * pxESPuuid,
                       BTUuid_t * pxUuid );
BTStatus_t prvSetIOs( BTIOtypes_t xPropertyIO );
BTStatus_t prvToggleBondableFlag( bool bEnable );
BTStatus_t prvToggleSecureConnectionOnlyMode( bool bEnable );
char * prxESPGetBLEDeviceName( void );

#endif /* ifndef _AWS_BLE_INTERNALS_H_ */
