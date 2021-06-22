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
 * @file iot_ble_device_information.h
 * @brief GATT service which exposes FreeRTOS device information
 */
#ifndef IOT_BLE_DEVICE_INFORMATION_H_
#define IOT_BLE_DEVICE_INFORMATION_H_

#include <stdbool.h>

/**
 * @functions_page{iotbledeviceinfo, Device Information}
 * @functions_brief{GATT Service for Device Information}
 * - @function_name{iotbledeviceinfo_function_init}
 * @function_brief{iotbledeviceinfo_function_init}
 */

/**
 * @function_page{IotBleDeviceInfo_Init,iotbledeviceinfo,init}
 * @function_snippet{iotbledeviceinfo,init,this}
 * @copydoc IotBleDeviceInfo_Init
 */

/**
 * @brief Creates and starts FreeRTOS device information service
 *
 * @return true if the service is initialized successfully, false otherwise
 */
/* @[declare_iotbledeviceinfo_init] */
bool IotBleDeviceInfo_Init( void );
/* @[declare_iotbledeviceinfo_init] */

/**
 * @function_page{IotBleDeviceInfo_Cleanup,iotbledeviceinfo,cleanup}
 * @function_snippet{iotbledeviceinfo,cleanup,this}
 * @copydoc IotBleDeviceInfo_Cleanup
 */

/**
 * @brief Cleanup device information service.
 *
 * @return true If the cleanup is successful false otherwise.
 */
/* @[declare_iotbledeviceinfo_cleanup] */
bool IotBleDeviceInfo_Cleanup( void );
/* @[declare_iotbledeviceinfo_cleanup] */

#endif /* IOT_BLE_DEVICE_INFORMATION_H_ */
