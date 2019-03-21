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
 * @brief Service and characteristic UUIDs used by MQTT proxy service.
 */
#define IOT_BLE_MQTT_SERVICE_UUID			      { 0x00, 0xFF, 0xC3, 0x4C, 0x04, 0x48, 0x02, 0xA0, 0xA9, 0x40, 0x2E, 0xD7, 0x6A, 0x16, 0xD7, 0xA9 }
#define IOT_BLE_MQTT_CHAR_UUID_MASK	          0xC3, 0x4C, 0x04, 0x48, 0x02, 0xA0, 0xA9, 0x40, 0x2E, 0xD7, 0x6A, 0x16, 0xD7, 0xA9
#define IOT_BLE_MQTT_CHAR_CONTROL_UUID          {0x01, 0xFF, IOT_BLE_MQTT_CHAR_UUID_MASK }
#define IOT_BLE_MQTT_CHAR_TX_MESG_UUID          {0x02, 0xFF, IOT_BLE_MQTT_CHAR_UUID_MASK }
#define IOT_BLE_MQTT_CHAR_RX_MESG_UUID	  {0x03, 0xFF, IOT_BLE_MQTT_CHAR_UUID_MASK }
#define IOT_BLE_MQTT_CHAR_TX_LARGE_MESG_UUID    {0x04, 0xFF, IOT_BLE_MQTT_CHAR_UUID_MASK }
#define IOT_BLE_MQTT_CHAR_RX_LARGE_MESG_UUID    {0x05, 0xFF, IOT_BLE_MQTT_CHAR_UUID_MASK }

/**
 * @brief Client Characteristic configuration Descriptor UUID.
 *
 */
#define IOT_BLE_MQTT_CCFG_UUID                  ( 0x2902 )

/**
 * @brief Number of characteristics, descriptors and included services for MQTT service
 */
#define IOT_BLE_MQTT_NUM_CHARS                 ( 5 )
#define IOT_BLE_MQTT_NUM_CHAR_DESCRS           ( 2 )
#define IOT_BLE_MQTT_NUM_INCLUDED_SERVICES     ( 0 )

/**
 * @brief Characteristics used by MQTT Service.
 */

typedef enum {
  IOT_BLE_MQTT_SERVICE,
  IOT_BLE_MQTT_CHAR_CONTROL,                //!< IOT_BLE_MQTT_Control Characteristic to enable/disable transferring data over MQTT service
  IOT_BLE_MQTT_CHAR_TX_MESG,                //!< IOT_BLE_MQTT_TXMessage Characteristic to send notification containing a message to MQTT broker
  IOT_BLE_MQTT_CHAR_DESCR_TX_MESG,          //!< IOT_BLE_MQTT_CHAR_DESCR_TX_MESG Characteristic descriptor
  IOT_BLE_MQTT_CHAR_RX_MESG,                //!< IOT_BLE_MQTT_RXMessage Characteristic to receive a message from MQTT broker
  IOT_BLE_MQTT_CHAR_TX_LARGE_MESG,               //!< IOT_BLE_MQTT_TXLargeMessage Characteristic to send a large message (> BLE MTU Size) to MQTT broker
  IOT_BLE_MQTT_CHAR_DESCR_TX_LARGE_MESG,    //!< IOT_BLE_MQTT_CHAR_DESCR_TX_LARGE_MESG Characteristic descripto
  IOT_BLE_MQTT_CHAR_RX_LARGE,                    //!  IOT_BLE_MQTT_RXLargeMessage Characteristic to receive large message (> BLE MTU Size) from MQTT broke
  IOT_BLE_MQTT_NUMBER_ATTRIBUTES                       //!< IOT_BLE_MQTT_Attributes_t Number of attributes in eMQTT service
} IotBleMqttAttributes_t;

/**
 * Descriptors used by MQTT service
 */
typedef enum
{
	IOT_BLE_MQTT_TX_MSG_DESCR = 0, //!< IOT_BLE_MQTT_TX_MSG_DESCR Client Characteristic Configuration Descriptor to enable notification for TXMessage Characteristic
	IOT_BLE_MQTT_TX_LARGE_MSG_DESCR //!< IOT_BLE_MQTT_TX_LARGE_MSG_DESCR Client Characteristic Configuration Descriptor to enable notification for TXLargeMessage Characteristic
} IotMqttCharacteristicDescr_t;

/**
 * @brief Used to set and retrieve the state of BLE proxy.
 */
#define IOT_BLE_MQTT_PARAM_STATE             "l"

#define IOT_BLE_MQTT_NUM_CONTROL_MESG_PARAMS  ( 1 )

/**
 * @brief Default MTU size for the BLE connection
 */
#define IOT_BLE_MQTT_DEFAULT_MTU_SIZE 			( IOT_BLE_PREFERRED_MTU_SIZE )

/**
 * @brief Maximum bytes which can be transferred at a time through the BLE connection.
 */
#define IOT_BLE_MQTT_TRANSFER_LEN( mtu )        ( ( mtu ) - 3 )
/**
 *@brief Size of the buffer used to store outstanding bytes to be send out.
 */
#define IOT_BLE_MQTT_TX_BUFFER_SIZE        ( 1024 )

/**
 * @brief Size of the buffer to hold the received message
 *
 */
#define IOT_BLE_MQTT_RX_BUFFER_SIZE        ( 1024 )

/**
 *
 * @brief The default timeout in milliseconds for sending a message to the proxy
 */
#define IOT_BLE_MQTT_DEFAULT_SEND_TIMEOUT_MS          ( 2000 )

#define IOT_BLE_MQTT_CONNECTION_INITIALIZER       NULL       /* Initializer for MQTT connection over BLE */

typedef struct IotBleMqttConnectionType
{
	StreamBufferHandle_t sendBuffer;
	SemaphoreHandle_t sendLock;
	TickType_t sendTimeout;
	uint8_t * pRecvBuffer;
	size_t recvBufferLen;
	SemaphoreHandle_t recvLock;
	IotMqttConnection_t* pMqttConnection;
} IotBleMqttConnectionType_t;

typedef void*  IotBleMqttConnection_t;


/**
 * @brief MQTT BLE Service structure
 */
typedef struct  IotBleMqttService
{
	BTService_t* pServicePtr;
	uint16_t descrVal[ IOT_BLE_MQTT_NUM_CHAR_DESCRS ];
	uint16_t BLEConnId;
  bool isInit;
	bool isEnabled;
	IotBleMqttConnectionType_t connection;
}  IotBleMqttService_t;


/**
 * @brief Number of MQTT Proxy service instances to run.
 */
#define IOT_BLE_MQTT_MAX_SVC_INSTANCES                ( 1 )

#define IS_SUCCESS( status )			( status == eBTStatusSuccess )

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Declaration of a network interface struct using the functions in this file.
 */
extern const IotNetworkInterface_t IotNetworkBle;

/**
 * @brief Initializes the MQTT service instances.
 *
 * Creates and starts the GATT service instances for MQTT. Number of service instances are predefined.
 *
 * @return pdTRUE if initialization is successful.
 *         pdFALSE if initialization fails.
 */
BaseType_t IotBleMqtt_Init( void );
/**
 *
 * @brief Creates a new BLE network connection for the given MQTT connection.
 *
 * Function finds an unused MQTT service instance, and assigns the MQTT connection handle
 * to the GATT service.
 *
 * @param[in] pMqttConnection Pointer to the MQTT connection
 * @param[out] pConnection Handle to the BLE network Connection
 * @return pdTRUE If the operation is successful
 *         pdFALSE If the operation failed
 */
IotNetworkError_t IotBleMqtt_CreateConnection( void * pConnectionInfo, void * pCredentialInfo, void * pConnection );

/**
 * @brief Sets the send timeout for the BLE network connection.
 *
 * @param connection The handle to the BLE network connection
 * @param timeoutMs Timeout value in milliseconds
 * @return pdTRUE if the operation is successful
 *         pdFALSE if the operation failed
 */
BaseType_t IotBleMqtt_SetSendTimeout(  void * pConnection, uint16_t timeoutMs );

/**
 * @brief Closes the BLE network connection
 *
 * Function closes the BLE network connection, further data transfer over the connection returns failure.
 *
 * @param[in] connection Handle to the MQTT BLE connection
 * @return pdTRUE if the Connection is closed successfully
 *         pdFALSE if the connection is already closed.
 */
IotNetworkError_t IotBleMqtt_CloseConnection(  void * pConnection );

/**
 *@brief Destroys the BLE network connection
 *
 *Function frees the resources associated with the BLE network connection
 * @param connection Handle to the BLE network connection
 */
IotNetworkError_t IotBleMqtt_DestroyConnection(  void * pConnection );


/**
 * @brief Transfers data through a BLE network connection
 *
 * @param[in] pvConnection Handle to the BLE network connection
 * @param[in] pvMessage Pointer to the message to be transferred
 * @param[in] messageLength Length of the message to be transferred
 * @return Number of bytes of data transferred.
 */
size_t IotBleMqtt_Send( void* pConnection, const uint8_t * const pMessage, size_t messageLength );

#endif /* IOT_BLE_MQTT_H */
