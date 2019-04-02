/*
 * Amazon FreeRTOS
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @brief GATT service which exposes Amazon FreeRTOS device information
 */
#ifndef IOT_BLE_DEVICE_INFORMATION_H_
#define IOT_BLE_DEVICE_INFORMATION_H_

#include "iot_ble.h"

/**
 * @constantspage{iotbledeviceinfo,ble library,Device Information}
 *
 * @section ble_constants_device_info BLE constants for Device Information service.
 * @brief GATT Service, characteristic and descriptor UUIDs used by the Device information service.
 *
 * @snippet this define_ble_constants_device_info
 *
 */
/* @[define_ble_constants_device_info] */
#define IOT_BLE_DEVICE_INFO_CHAR_UUID_BASE          IOT_BLE_DEVICE_INFO_SERVICE_UUID                        /**< @brief Base UUID. */
#define IOT_BLE_DEVICE_INFO_VERSION_UUID            { 0x01, 0xFF, IOT_BLE_DEVICE_INFO_SERVICE_UUID_MASK }   /**< @brief Firmware version. */
#define IOT_BLE_DEVICE_INFO_BROKER_ENDPOINT_UUID    { 0x02, 0xFF, IOT_BLE_DEVICE_INFO_SERVICE_UUID_MASK }   /**< @brief Broker endpoint. */
#define IOT_BLE_DEVICE_INFO_CHAR_MTU_UUID           { 0x03, 0xFF, IOT_BLE_DEVICE_INFO_SERVICE_UUID_MASK }   /**< @brief MTU size. */
#define IOT_BLE_DEVICE_INFO_CLIENT_CHAR_CFG_UUID    0x2902                                                  /**< @brief Client configuration. */
/* @[define_ble_constants_device_info] */

/**
 * @brief Characteristics for Device Inforamtion Service.
 */
typedef enum
{
    IOT_BLE_DEVICE_INFO_VERSION_CHAR = 0,           /**< IOT_BLE_DEVICE_INFO_VERSION_CHAR Exposes the services version for the device */
    IOT_BLE_DEVICE_INFO_MQTT_BROKER_END_POINT_CHAR, /**< IOT_BLE_DEVICE_INFO_MQTT_BROKER_END_POINT_CHAR Exposes the IOT broker endpoint with which the device is provisioned */
    IOT_BLE_DEVICE_INFO_MTU_CHAR,                   /**< IOT_BLE_DEVICE_INFO_MTU_CHAR Expose the BLE MTU for the device */
    IOT_BLE_DEVICE_INFO_ENCODING_CHAR,              /**< IOT_BLE_DEVICE_INFO_ENCODING_CHAR Tells what type of encoding is used by the device. */
} IotBleDeviceInfoCharacteristic_t;

/**
 * @brief Descriptors for Device Information Service
 */
typedef enum
{
    IOT_BLE_DEVICE_INFO_MTU_CHAR_DESCR = 0, /**< IOT_BLE_DEVICE_INFO_MTU_CHAR_DESCR Client Characteristic configuration descriptor for MTU characteristic */
} IotBleDeviceInfoDescriptor_t;

/**
 * @brief Structure used for Device Information Service
 */
typedef struct IotBleDeviceInfoService
{
    BTService_t * pBLEService; /**< A pointer to the GATT service for Device Information. */
    uint16_t CCFGVal[ 1 ];     /**< The configuration descriptor. */
    uint16_t BLEConnId;        /**< The connection ID. */
    uint16_t BLEMtu;           /**< The MTU size. */
} IotBleDeviceInfoService_t;

/**
 * @functionspage{iotbledeviceinfo,GATT Service for Device Information,Device Information}
 * - @functionname{iotbledeviceinfo_function_init}
 */

/**
 * @functionpage{IotBleDeviceInfo_Init,iotbledeviceinfo,init}
 */

/**
 * @brief Creates and starts Amazon FreeRTOS device information service
 *
 * @return pdTRUE if the service is initialized successfully, pdFALSE otherwise
 */
/* @[declare_iotbledeviceinfo_init] */
bool IotBleDeviceInfo_Init( void );
/* @[declare_iotbledeviceinfo_init] */

#endif /* IOT_BLE_DEVICE_INFORMATION_H_ */
