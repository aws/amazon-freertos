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
 * @file iot_ble_mqtt.h
 * @brief GATT service for transferring MQTT packets over BLE
 */

#ifndef IOT_BLE_MQTT_H
#define IOT_BLE_MQTT_H

#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "stream_buffer.h"

#include "iot_ble.h"
#include "platform/iot_network_ble.h"
#include "platform/iot_threads.h"

/**
 * @constantspage{iotblemqtt,ble library,MQTT proxy}
 *
 * @section ble_constants_mqtt_proxy BLE constants for MQTT proxy service.
 * @brief GATT Service, characteristic and descriptor UUIDs used by the MQTT proxy service.
 *
 * @snippet this define_ble_constants_mqtt_proxy
 *
 * @section ble_constants_mqtt_proxy_timeout BLE timeout constants for MQTT proxy service.
 * @brief BLE constants for modifying the default timeout value for MQTT proxy service.
 * - #IOT_BLE_MQTT_DEFAULT_SEND_TIMEOUT_MS <br>
 *   @copybrief IOT_BLE_MQTT_DEFAULT_SEND_TIMEOUT_MS
 *
 * @section ble_constants_mqtt_proxy_mtu BLE constant for default MTU size.
 * @brief BLE constants for modifying the default MTU size.
 * - #IOT_BLE_MQTT_DEFAULT_MTU_SIZE <br>
 *   @copybrief IOT_BLE_MQTT_DEFAULT_MTU_SIZE
 *
 */
/* @[define_ble_data_transfer_service_uuids] */
#define IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK   0xC3, 0x4C, 0x04, 0x48, 0x02, 0xA0, 0xA9, 0x40, 0x2E, 0xD7, 0x6A, 0x16, 0xD7, 0xA9  /**< @brief UUID mask. */

#define IOT_BLE_DATA_TRANSFER_SERVICE_UUID( SERVICE_ID )         { 0x00, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }      /**< @brief Base UUID. */
#define IOT_BLE_DATA_TRANSFER_TX_CHAR_UUID( SERVICE_ID )         { 0x01, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }                                                         /**< @brief TX buffer. */
#define IOT_BLE_DATA_TRANSFER_RX_CHAR_UUID( SERVICE_ID )         { 0x02, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }                                                         /**< @brief RX buffer. */
#define IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR_UUID( SERVICE_ID)    { 0x03, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }                                                         /**< @brief TX large buffer. */
#define IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR_UUID( SERVICE_ID )   { 0x04, SERVICE_ID, IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK }                                                         /**< @brief RX large buffer. */
#define IOT_BLE_DATA_TRANSFER_CCFG_UUID                       ( 0x2902 )                                                                                          /**< @brief Client configuration. */
/* @[define_ble_data_transfer_service_uuids] */

#define IOT_BLE_MQTT_SERVICE_ID                0x00

#define IOT_BLE_WIFI_PROVISIONING_SERVICE_ID   0x01

typedef enum IotBleDataTransferAttributes
{
    IOT_BLE_DATA_TRANSFER_SERVICE = 0,
    IOT_BLE_DATA_TRANSFER_TX_CHAR,             /*!< IOT_BLE_DATA_TRANSFER_TX_CHAR Characteristic to send message. */
    IOT_BLE_DATA_TRANSFER_TX_CHAR_DESCR,       /*!< IOT_BLE_DATA_TRANSFER_TX_CHAR_DESCR Characteristic descriptor. */
    IOT_BLE_DATA_TRANSFER_RX_CHAR,             /*!< IOT_BLE_DATA_TRANSFER_RX_CHAR Characteristic to receive a message.*/
    IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR,       /*!< IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR Characteristic to send a large message (> BLE MTU Size). */
    IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR_DESCR, /*!< IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR_DESCR Characteristic descriptor. */
    IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR,       /*!< IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR Characteristic to receive large message (> BLE MTU Size). */
    IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES       /*!< IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES Max number of attributes for ble data transfer service. */

} IotBleDataTransferAttributes_t;


/**
 * @ingroup ble_datatypes_structs
 * MQTT connection over BLE.
 */
typedef struct IotBleMqttConnectionType
{
    StreamBufferHandle_t sendBuffer;              /**< Send buffer. */
    IotSemaphore_t sendLock;                      /**< Lock to protect access to the send buffer. */
    uint32_t sendTimeout;                         /**< Send timeout. */
    uint8_t * pRecvBuffer;                        /**< Receive buffer. */
    uint32_t recvBufferLen;                       /**< Length of the receive buffer in bytes. */                         
    IotSemaphore_t recvLock;                      /**< Lock to protect access to the receive buffer. */
    IotNetworkReceiveCallback_t receiveCallback;  /**< Callback invoked when data received on the connection. */
    void *pContext;                               /**< Context set for the receive callback. */
} IotBleMqttConnectionType_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief MQTT BLE Service structure.
 */
typedef struct IotBleDataTransferService
{
    BTService_t * pGattServicePtr;            /**< A pointer to the GATT structure for the service.. */
    uint16_t ccfgValue;                       /**< Enable/disable the CCFG descriptor value for the service. */
    bool initialized;                         /**< If service is initialized */
    IotBleMqttConnectionType_t connection;    /**< The representation of the MQTT connection. */
} IotBleDataTransferService_t;

/**
 * @functionspage{iotblemqtt,GATT Service for MQTT Proxy, MQTT Proxy}
 * - @functionname{iotblemqtt_function_init}
 * - @functionname{iotblemqtt_function_createconnection}
 * - @functionname{iotblemqtt_function_closeconnection}
 * - @functionname{iotblemqtt_function_destroyconnection}
 * - @functionname{iotblemqtt_function_send}
 */

/**
 * @functionpage{IotBleMqtt_Init,iotblemqtt,init}
 * @functionpage{IotBleMqtt_CreateConnection,iotblemqtt,createconnection}
 * @functionpage{IotBleMqtt_CloseConnection,iotblemqtt,closeconnection}
 * @functionpage{IotBleMqtt_DestroyConnection,iotblemqtt,destroyconnection}
 * @functionpage{IotBleMqtt_Send,iotblemqtt,send}
 */

/**
 * @brief Initializes a data transfer service.
 * Creates the GATT service attribute table and intialize  he service.
 * 
 * @return true if initialization is successful.
 *         false if initialization failed.
 */
/* @[declare_iotblemqtt_init] */
bool IotBleDataTransferService_Init( IotBleDataTransferServiceID_t serviceID );
/* @[declare_iotblemqtt_init] */

#endif /* IOT_BLE_MQTT_H */
