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
 * @file aws_ble_device_information.h
 * @brief GATT service which exposes Amazon FreeRTOS device information
 */
#ifndef AWS_BLE_DEVICE_INFORMATION_H_
#define AWS_BLE_DEVICE_INFORMATION_H_

#include "aws_ble.h"
#include "aws_ble_config.h"


/**
 * @brief Service, characteristic and descriptor UUIDS for Device information Service
 */
#define deviceInfoCHAR_UUID_BASE          bleconfigDEVICE_INFO_SERVICE_UUID
#define deviceInfoCHAR_VERSION_UUID       ( 0xFF01 )
#define deviceInfoBROKER_ENDPOINT_UUID    ( 0xFF02 )
#define deviceInfoCHAR_MTU_UUID           ( 0xFF03 )
#define deviceInfoCLIENT_CHAR_CFG_UUID    ( 0x2902 )

/**
 * @brief Number of characteristics, descriptors and included services for Device Information Service
 */
#define deviceInfoMAX_CHARS               3
#define deviceInfoMAX_DESCRS              1
#define deviceInfoMAX_INC_SVCS            0

/**
 * @brief Characteristics for Device Inforamtion Service.
 */
typedef enum
{
    eDeviceInfoVersionChar = 0,         /**< eDeviceInfoVersionChar Exposes the services version for the device */
    eDeviceInfoMQTTBrokerEndpointChar, /**< eDeviceInfoMQTTBrokerEndpointChar Exposes the IOT broker endpoint with which the device is provisioned */
    eDeviceInfoMtuChar,                /**< eDeviceInfoMtuChar Expose the BLE MTU for the device */
} DeviceInfoCharacteristic_t;

/**
 * @brief Descriptors for Device Information Service
 */
typedef enum
{
    eDeviceInfoMtuCharDescr = 0, /**< eDeviceInfoMtuCharDescr Client Characteristic configuration descriptor for MTU characteristic */
} DeviceInfoDescriptor_t;

/**
 *
 * JSON tokens used within the messages exchanged between GATT client and Server.
 *
 */

#define INT_MAX_WIDTH                  ( 6 )
#define deviceInfoMTU_WIDTH            ( INT_MAX_WIDTH )
#define deviceInfoVERSION_WIDTH        ( INT_MAX_WIDTH )
#define deviceInfoMTU                  "mtu"
#define deviceInfoVerison              "version"
#define deviceInfoBROKER_ENDPOIINT     "brokerEndpoint"

#define JSON_STR( x )    STR( x )
#define STR( x )         # x

/**
 * JSON format for serializing the response payloads
 */
#define deviceInfoMTU_MSG_FORMAT    \
    "{"                             \
    JSON_STR( deviceInfoMTU ) ":%d" \
                              "}"
#define deviceInfoMTU_MSG_LEN               ( sizeof( deviceInfoMTU_MSG_FORMAT ) + deviceInfoMTU_WIDTH )

#define deviceInfoVERSION_MSG_FORMAT                \
    "{"                                             \
    JSON_STR( deviceInfoVerison      ) ":\"%.*s\""  \
                                     "}"
#define deviceInfoVERSION_MSG_LEN      ( sizeof( deviceInfoVERSION_MSG_FORMAT ) )

#define deviceInfoBROKERENDPOINT_MSG_FORMAT            \
    "{"                                                \
    JSON_STR( deviceInfoBROKER_ENDPOIINT ) ":\"%.*s\"" \
                                           "}"
#define deviceInfoBROKERENDPOINT_MSG_LEN    ( sizeof( deviceInfoBROKERENDPOINT_MSG_FORMAT ) )

/**
 * @brief Structure used for Device Information Service
 */
typedef struct DeviceInfoService
{
    BLEService_t * pxBLEService;
    uint16_t usCCFGVal[ deviceInfoMAX_DESCRS ];
    uint16_t usBLEConnId;
    uint16_t usBLEMtu;
} DeviceInfoService_t;

/**
 * @Brief Creates and starts Amazon FreeRTOS device information service
 *
 * @return pdTRUE if the service is initialized successfully, pdFALSE otherwise
 */
BaseType_t AFRDeviceInfoSvc_Init( void );


#endif /* AWS_BLE_DEVICE_INFORMATION_H_ */
