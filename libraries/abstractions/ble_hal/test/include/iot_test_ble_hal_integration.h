/*
 * FreeRTOS BLE HAL V5.1.0
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
 *
 * @file iot_test_ble_hal_integration.h
 * @brief
 */



#ifndef _IOT_TEST_BLE_HAL_INTEGRATION_H_
#define _IOT_TEST_BLE_HAL_INTEGRATION_H_

#include "iot_test_ble_hal_common.h"

#define bletestsFAIL_CHAR_VALUE          "fail"
#define bletests_MANUFACTURERDATA_LEN    3
#define bletests_MANUFACTURERDATA        { 0xE5, 0x02, 0x05 }
#define bletests_SERVICEDATA_LEN         3
#define bletests_SERVICEDATA             { 0xEF, 0x12, 0xD6 }

void GAP_common_teardown();
void GAP_common_setup();
void GATT_teardown();
void GATT_setup();

void prvInitWithNULLCb( void );

void prvGetResult( bletestAttSrvB_t xAttribute,
                   bool IsPrep,
                   uint16_t usOffset );
void prvShortWaitConnection( void );
void prvCreateStartServicesWithNULLCb( void );
void prvStartServiceWithNULLCb( BTService_t * xRefSrvc );
void prvSetAdvPropertyWithNULLCb( void );
void prvRemoveAllBondWithNULLCb( void );
void prvRemoveBondWithNULLCb( BTBdaddr_t * pxDeviceAddress );
void prvSetAdvDataWithNULLCb( BTuuidType_t type,
                              uint16_t usManufacturerLen,
                              char * pcManufacturerData );
void prvSetAdvertisementWithNULLCb( BTGattAdvertismentParams_t * pxParams,
                                    uint16_t usServiceDataLen,
                                    char * pcServiceData,
                                    BTUuid_t * pxServiceUuid,
                                    size_t xNbServices,
                                    uint16_t usManufacturerLen,
                                    char * pcManufacturerData );
void prvStartStopAdvertisementWithNULLCb( void );
void prvStopServiceWithNULLCb( BTService_t * xRefSrvc );
void prvDeleteServiceWithNULLCb( BTService_t * xRefSrvc );
void prvBTUnregisterWithNULLCb( void );

#endif /* ifndef _IOT_TEST_BLE_HAL_INTEGRATION_H_ */
