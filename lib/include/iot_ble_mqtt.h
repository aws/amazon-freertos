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
#include "semphr.h"
#include "iot_ble.h"
#include "iot_mqtt.h"
#include "iot_ble_mqtt_serialize.h"

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
/* @[define_ble_constants_mqtt_proxy] */
#define IOT_BLE_MQTT_SERVICE_UUID               { 0x00, 0xFF, 0xC3, 0x4C, 0x04, 0x48, 0x02, 0xA0, 0xA9, 0x40, 0x2E, 0xD7, 0x6A, 0x16, 0xD7, 0xA9 }  /**< @brief Base UUID. */
#define IOT_BLE_MQTT_CHAR_UUID_MASK             0xC3, 0x4C, 0x04, 0x48, 0x02, 0xA0, 0xA9, 0x40, 0x2E, 0xD7, 0x6A, 0x16, 0xD7, 0xA9                  /**< @brief UUID mask. */
#define IOT_BLE_MQTT_CHAR_CONTROL_UUID          { 0x01, 0xFF, IOT_BLE_MQTT_CHAR_UUID_MASK }                                                         /**< @brief Control. */
#define IOT_BLE_MQTT_CHAR_TX_MESG_UUID          { 0x02, 0xFF, IOT_BLE_MQTT_CHAR_UUID_MASK }                                                         /**< @brief TX buffer. */
#define IOT_BLE_MQTT_CHAR_RX_MESG_UUID          { 0x03, 0xFF, IOT_BLE_MQTT_CHAR_UUID_MASK }                                                         /**< @brief RX buffer. */
#define IOT_BLE_MQTT_CHAR_TX_LARGE_MESG_UUID    { 0x04, 0xFF, IOT_BLE_MQTT_CHAR_UUID_MASK }                                                         /**< @brief TX large buffer. */
#define IOT_BLE_MQTT_CHAR_RX_LARGE_MESG_UUID    { 0x05, 0xFF, IOT_BLE_MQTT_CHAR_UUID_MASK }                                                         /**< @brief RX large buffer. */
#define IOT_BLE_MQTT_CCFG_UUID                  ( 0x2902 )                                                                                          /**< @brief Client configuration. */
/* @[define_ble_constants_mqtt_proxy] */

/**
 * @brief The default timeout in milliseconds for sending a message to the proxy.
 */
#define IOT_BLE_MQTT_DEFAULT_SEND_TIMEOUT_MS    ( 2000 )

/**
 * @brief Default MTU size for the BLE connection.
 */
#define IOT_BLE_MQTT_DEFAULT_MTU_SIZE           ( IOT_BLE_PREFERRED_MTU_SIZE )

/**
 * @ingroup ble_datatypes_enums
 * @brief Characteristics used by MQTT Service.
 */
typedef enum
{
    IOT_BLE_MQTT_SERVICE,
    IOT_BLE_MQTT_CHAR_CONTROL,             /*!< IOT_BLE_MQTT_Control Characteristic to enable/disable transferring data over MQTT service */
    IOT_BLE_MQTT_CHAR_TX_MESG,             /*!< IOT_BLE_MQTT_TXMessage Characteristic to send notification containing a message to MQTT broker */
    IOT_BLE_MQTT_CHAR_DESCR_TX_MESG,       /*!< IOT_BLE_MQTT_CHAR_DESCR_TX_MESG Characteristic descriptor */
    IOT_BLE_MQTT_CHAR_RX_MESG,             /*!< IOT_BLE_MQTT_RXMessage Characteristic to receive a message from MQTT broker */
    IOT_BLE_MQTT_CHAR_TX_LARGE_MESG,       /*!< IOT_BLE_MQTT_TXLargeMessage Characteristic to send a large message (> BLE MTU Size) to MQTT broker */
    IOT_BLE_MQTT_CHAR_DESCR_TX_LARGE_MESG, /*!< IOT_BLE_MQTT_CHAR_DESCR_TX_LARGE_MESG Characteristic descripto */
    IOT_BLE_MQTT_CHAR_RX_LARGE,            /*!  IOT_BLE_MQTT_RXLargeMessage Characteristic to receive large message (> BLE MTU Size) from MQTT broke */
    IOT_BLE_MQTT_NUMBER_ATTRIBUTES         /*!< IOT_BLE_MQTT_Attributes_t Number of attributes in eMQTT service */
} IotBleMqttAttributes_t;

/**
 * @ingroup ble_datatypes_enums
 * Descriptors used by MQTT service
 */
typedef enum
{
    IOT_BLE_MQTT_TX_MSG_DESCR = 0,  /*!< IOT_BLE_MQTT_TX_MSG_DESCR Client Characteristic Configuration Descriptor to enable notification for TXMessage Characteristic */
    IOT_BLE_MQTT_TX_LARGE_MSG_DESCR /*!< IOT_BLE_MQTT_TX_LARGE_MSG_DESCR Client Characteristic Configuration Descriptor to enable notification for TXLargeMessage Characteristic */
} IotMqttCharacteristicDescr_t;


/**
 * @ingroup ble_datatypes_structs
 * MQTT connection over BLE.
 */
typedef struct IotBleMqttConnectionType
{
    StreamBufferHandle_t sendBuffer;       /**< Send buffer. */
    SemaphoreHandle_t sendLock;            /**< Lock to protect access to the send buffer. */
    TickType_t sendTimeout;                /**< Send timeout. */
    uint8_t * pRecvBuffer;                 /**< Receive buffer. */
    uint32_t recvBufferLen;                /**< Length of the receive buffer in bytes. */
    SemaphoreHandle_t recvLock;            /**< Lock to protect access to the receive buffer. */
    IotMqttConnection_t * pMqttConnection; /**< A pointer to the MQTT connection. */
} IotBleMqttConnectionType_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief MQTT BLE Service structure.
 */
typedef struct  IotBleMqttService
{
    IotBleMqttConnectionType_t connection; /**< The representation of the MQTT connection. */
    BTService_t * pServicePtr;             /**< A pointer to the BLE GATT service. */
    uint16_t descrVal[ 2 ];                /**< A buffer for the value descriptor. */
    uint16_t BLEConnId;                    /**< The BLE connection ID. */
    bool isInit;                           /**< A flag to indicate whether this connection is initialized. */
    bool isEnabled;                        /**< A flag to indicate whether this connection is enbled. */
} IotBleMqttService_t;

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
 * @brief Initializes the MQTT service instances.
 *
 * Creates and starts the GATT service instances for MQTT. Number of service instances are predefined.
 *
 * @return pdTRUE if initialization is successful.
 *         pdFALSE if initialization fails.
 */
/* @[declare_iotblemqtt_init] */
bool IotBleMqtt_Init( void );
/* @[declare_iotblemqtt_init] */

/**
 *
 * @brief Creates a new BLE network connection for the given MQTT connection.
 *
 * Function finds an unused MQTT service instance, and assigns the MQTT connection handle
 * to the GATT service.
 *
 * @param[in] pConnectionInfo Pointer to the instance of MQTT connection information to initialize the connection with.
 * @param[in] pCredentialInfo Pointer to the instance of MQTT credentials information to initialize the connection with.
 * @param[out] pConnection Handle to the BLE network Connection
 * @return pdTRUE If the operation is successful
 *         pdFALSE If the operation failed
 */
/* @[declare_iotblemqtt_createconnection] */
IotNetworkError_t IotBleMqtt_CreateConnection( void * pConnectionInfo,
                                               void * pCredentialInfo,
                                               void ** pConnection );
/* @[declare_iotblemqtt_createconnection] */

/**
 * @brief Closes the BLE network connection
 *
 * Function closes the BLE network connection, further data transfer over the connection returns failure.
 *
 * @param[in] pConnection Handle to the MQTT BLE connection
 * @return pdTRUE if the Connection is closed successfully
 *         pdFALSE if the connection is already closed.
 */
/* @[declare_iotblemqtt_closeconnection] */
IotNetworkError_t IotBleMqtt_CloseConnection( void * pConnection );
/* @[declare_iotblemqtt_closeconnection] */

/**
 *@brief Destroys the BLE network connection
 *
 * Function frees the resources associated with the BLE network connection
 * @param[in] pConnection Handle to the BLE network connection
 */
/* @[declare_iotblemqtt_destroyconnection] */
IotNetworkError_t IotBleMqtt_DestroyConnection( void * pConnection );
/* @[declare_iotblemqtt_destroyconnection] */

/**
 * @brief Transfers data through a BLE network connection
 *
 * @param[in] pConnection Handle to the BLE network connection
 * @param[in] pMessage Pointer to the message to be transferred
 * @param[in] messageLength Length of the message to be transferred
 * @return Number of bytes of data transferred.
 */
/* @[declare_iotblemqtt_send] */
size_t IotBleMqtt_Send( void * pConnection,
                        const uint8_t * const pMessage,
                        size_t messageLength );
/* @[declare_iotblemqtt_send] */

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Declaration of a network interface struct using the functions in this file.
 */
extern const IotNetworkInterface_t IotNetworkBle;
/** @endcond */

#endif /* IOT_BLE_MQTT_H */
