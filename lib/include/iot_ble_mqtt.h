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
 * @file aws_iot_mqtt_ble.h
 * @brief GATT service for transferring MQTT packets over BLE
 */

#ifndef AWS_IOT_MQTT_BLE_H
#define AWS_IOT_MQTT_BLE_H

#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "stream_buffer.h"
#include "semphr.h"
#include "iot_ble.h"
#include "aws_iot_mqtt.h"
#include "aws_iot_mqtt_serialize_ble.h"


/**
 * @brief Service and characteristic UUIDs used by MQTT proxy service.
 */
#define mqttBLESERVICE_UUID			      { 0x00, 0xFF, 0xC3, 0x4C, 0x04, 0x48, 0x02, 0xA0, 0xA9, 0x40, 0x2E, 0xD7, 0x6A, 0x16, 0xD7, 0xA9 }
#define mqttBLECHAR_UUID_MASK	          0xC3, 0x4C, 0x04, 0x48, 0x02, 0xA0, 0xA9, 0x40, 0x2E, 0xD7, 0x6A, 0x16, 0xD7, 0xA9
#define mqttBLECHAR_CONTROL_UUID          {0x01, 0xFF, mqttBLECHAR_UUID_MASK }
#define mqttBLECHAR_TX_MESG_UUID          {0x02, 0xFF, mqttBLECHAR_UUID_MASK }
#define mqttBLECHAR_RX_MESG_UUID	  {0x03, 0xFF, mqttBLECHAR_UUID_MASK }
#define mqttBLECHAR_TX_LARGE_MESG_UUID    {0x04, 0xFF, mqttBLECHAR_UUID_MASK }
#define mqttBLECHAR_RX_LARGE_MESG_UUID    {0x05, 0xFF, mqttBLECHAR_UUID_MASK }

/**
 * @brief Client Characteristic configuration Descriptor UUID.
 *
 */
#define mqttBLECCFG_UUID                  ( 0x2902 )

/**
 * @brief Number of characteristics, descriptors and included services for MQTT service
 */
#define mqttBLENUM_CHARS                 ( 5 )
#define mqttBLENUM_CHAR_DESCRS           ( 2 )
#define mqttBLENUM_INCLUDED_SERVICES     ( 0 )

/**
 * @brief Characteristics used by MQTT Service.
 */

typedef enum {
  eMQTTBLE_SERVICE,
  eMQTTBLE_CHAR_CONTROL,                //!< eMQTTBLEControl Characteristic to enable/disable transferring data over MQTT service
  eMQTTBLE_CHAR_TX_MESG,                //!< eMQTTBLETXMessage Characteristic to send notification containing a message to MQTT broker
  eMQTTBLE_CHAR_DESCR_TX_MESG,          //!< eMQTTBLE_CHAR_DESCR_TX_MESG Characteristic descriptor
  eMQTTBLE_CHAR_RX_MESG,                //!< eMQTTBLERXMessage Characteristic to receive a message from MQTT broker
  eMQTTBLE_CHAR_TX_LARGE_MESG,               //!< eMQTTBLETXLargeMessage Characteristic to send a large message (> BLE MTU Size) to MQTT broker
  eMQTTBLE_CHAR_DESCR_TX_LARGE_MESG,    //!< eMQTTBLE_CHAR_DESCR_TX_LARGE_MESG Characteristic descripto
  eMQTTBLE_CHAR_RX_LARGE,                    //!  eMQTTBLERXLargeMessage Characteristic to receive large message (> BLE MTU Size) from MQTT broke
  eMQTTBLE_NUM_ATTRIBUTES                       //!< eMQTTBLEAttributes_t Number of attributes in eMQTT service
} MQTTBLEAttributes_t;

/**
 * Descriptors used by MQTT service
 */
typedef enum
{
	eMQTTBLETXMessageDescr = 0, //!< eMQTTBLETXMessageDescr Client Characteristic Configuration Descriptor to enable notification for TXMessage Characteristic
	eMQTTBLETXLargeMessageDescr //!< eMQTTBLETXLargeMessageDescr Client Characteristic Configuration Descriptor to enable notification for TXLargeMessage Characteristic
} MQTTCharacteristicDescr_t;

/**
 * @brief Used to set and retrieve the state of BLE proxy.
 */
#define mqttBLEPARAM_STATE             "ps"

#define mqttBLE_NUM_CONTROL_MESG_PARAMS  ( 1 )

/**
 * @brief Default MTU size for the BLE connection
 */
#define mqttBLEDEFAULT_MTU_SIZE 			( IOT_BLE_PREFERRED_MTU_SIZE )

/**
 * @brief Maximum bytes which can be transferred at a time through the BLE connection.
 */
#define mqttBLETRANSFER_LEN( mtu )        ( ( mtu ) - 3 )
/**
 *@brief Size of the buffer used to store outstanding bytes to be send out.
 */
#define mqttBLETX_BUFFER_SIZE        ( 1024 )

/**
 * @brief Size of the buffer to hold the received message
 *
 */
#define mqttBLERX_BUFFER_SIZE        ( 1024 )

/**
 *
 * @brief The default timeout in milliseconds for sending a message to the proxy
 */
#define mqttBLEDEFAULT_SEND_TIMEOUT_MS          ( 2000 )

#define AWS_IOT_MQTT_BLE_CONNECTION_INITIALIZER       NULL       /* Initializer for MQTT connection over BLE */

typedef struct MqttBLEConnection
{
	StreamBufferHandle_t xSendBuffer;
	SemaphoreHandle_t xSendLock;
	TickType_t xSendTimeout;
	uint8_t *pRecvBuffer;
	size_t xRecvBufferLen;
	size_t xRecvOffset;
	SemaphoreHandle_t xRecvLock;
	AwsIotMqttConnection_t* pxMqttConnection;
} MqttBLEConnection_t;

typedef void* AwsIotMqttBLEConnection_t;


/**
 * @brief MQTT BLE Service structure
 */
typedef struct MqttBLEService
{
	BTService_t* pxServicePtr;
	uint16_t usDescrVal[ mqttBLENUM_CHAR_DESCRS ];
	uint16_t usBLEConnId;
        bool bIsInit;
	bool bIsEnabled;
	MqttBLEConnection_t xConnection;
} MqttBLEService_t;


/**
 * @brief Number of MQTT Proxy service instances to run.
 */
#define mqttBLEMAX_SVC_INSTANCES                ( 1 )

#define IS_SUCCESS( status )			( status == eBTStatusSuccess )


#define AWS_IOT_MQTT_BLE_INIT_SERIALIZER( pxNetworkInterface )                             \
do                                                                                         \
{                                                                                          \
    ( pxNetworkInterface )->serialize.connect       = AwsIotMqttBLE_SerializeConnect;      \
    ( pxNetworkInterface )->serialize.publish       = AwsIotMqttBLE_SerializePublish;      \
    ( pxNetworkInterface )->serialize.publishSetDup = AwsIotMqttBLE_PublishSetDup;         \
    ( pxNetworkInterface )->serialize.puback        = AwsIotMqttBLE_SerializePuback;       \
    ( pxNetworkInterface )->serialize.subscribe     = AwsIotMqttBLE_SerializeSubscribe;    \
    ( pxNetworkInterface )->serialize.unsubscribe   = AwsIotMqttBLE_SerializeUnsubscribe;  \
    ( pxNetworkInterface )->serialize.disconnect    = AwsIotMqttBLE_SerializeDisconnect;   \
    ( pxNetworkInterface )->freePacket              = AwsIotMqttBLE_FreePacket;            \
    ( pxNetworkInterface )->getPacketType           = AwsIotMqttBLE_GetPacketType;         \
    ( pxNetworkInterface )->deserialize.connack     = AwsIotMqttBLE_DeserializeConnack;    \
    ( pxNetworkInterface )->deserialize.publish     = AwsIotMqttBLE_DeserializePublish;    \
    ( pxNetworkInterface )->deserialize.puback      = AwsIotMqttBLE_DeserializePuback;     \
    ( pxNetworkInterface )->deserialize.suback      = AwsIotMqttBLE_DeserializeSuback;     \
    ( pxNetworkInterface )->deserialize.unsuback    = AwsIotMqttBLE_DeserializeUnsuback;   \
} while( 0 );


/**
 * @brief Initializes the MQTT service instances.
 *
 * Creates and starts the GATT service instances for MQTT. Number of service instances are predefined.
 *
 * @return pdTRUE if initialization is successful.
 *         pdFALSE if initialization fails.
 */
BaseType_t AwsIotMqttBLE_Init( void );
/**
 *
 * @brief Creates a new BLE network connection for the given MQTT connection.
 *
 * Function finds an unused MQTT service instance, and assigns the MQTT connection handle
 * to the GATT service.
 *
 * @param[in] pxMqttConnection Pointer to the MQTT connection
 * @param[out] pxConnection Handle to the BLE network Connection
 * @return pdTRUE If the operation is successful
 *         pdFALSE If the operation failed
 */
BaseType_t AwsIotMqttBLE_CreateConnection( AwsIotMqttConnection_t* pxMqttConnection, AwsIotMqttBLEConnection_t* pxConnection );

/**
 * @brief Sets the send timeout for the BLE network connection.
 *
 * @param xConnection The handle to the BLE network connection
 * @param usTimeoutMS Timeout value in milliseconds
 * @return pdTRUE if the operation is successful
 *         pdFALSE if the operation failed
 */
BaseType_t AwsIotMqttBLE_SetSendTimeout( AwsIotMqttBLEConnection_t xConnection, uint16_t usTimeoutMS );

/**
 * @brief Closes the BLE network connection
 *
 * Function closes the BLE network connection, further data transfer over the connection returns failure.
 *
 * @param[in] xConnection Handle to the MQTT BLE connection
 * @return pdTRUE if the Connection is closed successfully
 *         pdFALSE if the connection is already closed.
 */
void AwsIotMqttBLE_CloseConnection( AwsIotMqttBLEConnection_t xConnection );

/**
 *@brief Destroys the BLE network connection
 *
 *Function frees the resources associated with the BLE network connection
 * @param xConnection Handle to the BLE network connection
 */
void AwsIotMqttBLE_DestroyConnection( AwsIotMqttBLEConnection_t xConnection );


/**
 * @brief Transfers data through a BLE network connection
 *
 * @param[in] pvConnection Handle to the BLE network connection
 * @param[in] pvMessage Pointer to the message to be transferred
 * @param[in] xMessageLength Length of the message to be transferred
 * @return Number of bytes of data transferred.
 */
size_t AwsIotMqttBLE_Send( void* pvConnection, const void * const pvMessage, size_t xMessageLength );

#endif /* AWS_IOT_MQTT_BLE_H */
