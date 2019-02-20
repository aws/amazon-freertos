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
 * @file aws_iot_mqtt_serialize_ble.h
 * @brief File contains serializer and deserializer APIS for sending and receiving MQTT messages
 * over BLE using a companion device SDK.
 */

#ifndef AWS_IOT_MQTT_SERIALIZE_BLE_H
#define AWS_IOT_MQTT_SERIALIZE_BLE_H

#include "aws_iot_mqtt.h"
#include "aws_clientcredential.h"

/**
 *  @defgroup
 *  Keys for MQTT message parameters exchanged between device and the companion BLE device SDK.
 */
/** @{ */
#define mqttBLEMSG_TYPE        "w"
#define mqttBLECLIENT_ID       "d"
#define mqttBLEBROKER_EP       "a"
#define mqttBLECLEAN_SESSION   "c"
#define mqttBLETOPIC           "u"
#define mqttBLETOPIC_LIST      "v"
#define mqttBLEQOS             "n"
#define mqttBLEQOS_LIST        "o"
#define mqttBLEMESSAGE_ID      "i"
#define mqttBLEPAYLOAD         "k"
#define mqttBLESTATUS          "s"
/** @} */

/**
 * @defgroup
 * MQTT message types exchanged between the device and the companion BLE device SDK.
 */
/** @{ */
#define mqttBLEMSG_TYPE_CONNECT          ( 1 )
#define mqttBLEMSG_TYPE_CONNACK          ( 2 )
#define mqttBLEMSG_TYPE_PUBLISH          ( 3 )
#define mqttBLEMSG_TYPE_PUBACK           ( 4 )
#define mqttBLEMSG_TYPE_PUBREC           ( 5 )
#define mqttBLEMSG_TYPE_PUBREL           ( 6 )
#define mqttBLEMSG_TYPE_PUBCOMP          ( 7 )
#define mqttBLEMSG_TYPE_SUBSCRIBE        ( 8 )
#define mqttBLEMSG_TYPE_SUBACK           ( 9 )
#define mqttBLEMSG_TYPE_UNSUBSCRIBE      ( 10 )
#define mqttBLEMSG_TYPE_UNSUBACK         ( 11 )
#define mqttBLEMSG_TYPE_PINGREQ          ( 12 )
#define mqttBLEMSG_TYPE_PINGRESP         ( 13 )
#define mqttBLEMSG_TYPE_DISCONNECT       ( 14 )
/** @} */

/**
 * @defgroup
 * CONNECT Response code exchanged between the device and the companion BLE device SDK.
 */
/** @{ */
typedef enum {

	eMQTTBLEStatusUnknown = 0,        //!< eMQTTBLEStatusUnknown  Connection status unknown by the SDK.
	eMQTTBLEStatusConnecting,         //!< eMQTTBLEStatusConnecting SDK has sent CONNECT request to server and waiting for the response.
	eMQTTBLEStatusConnected,          //!< eMQTTBLEStatusConnected  SDK is connected to the MQTT server.
	eMQTTBLEStatusDisconnected,       //!< eMQTTBLEStatusDisconnected SDK is disconnected with the MQTT server.
	eMQTTBLEStatusConnectionRefused,  //!< eMQTTBLEStatusConnectionRefused Server refused connection with the SDK.
	eMQTTBLEStatusConnectionError,    //!< eMQTTBLEStatusConnectionError Internal error while connecting to the server.
	eMQTTBLEStatusProtocolError       //!< eMQTTBLEStatusProtocolError CONNECT message from the device was malformed.
} MQTTBLEConnectStatus_t;
/** @} */

/**
 * @brief Initializes the serializer for MQTT messages over BLE.
 * @return true or false
 */
bool AwsIotMqttBLE_InitSerialize( void );


/**
 * @brief Cleanup the serializerfor Mqtt messages over BLE.
 */
void AwsIotMqttBLE_CleanupSerialize( void );

/**
 * @brief Serialize the CONNECT message sent towards MQTT BLE proxy.
 *
 * Serialize the CONNECT message parameters into a JSON structure
 *
 * @param[in] pConnectInfo MQTT CONNECT parameters.
 * @param[in] pWillInfo Will Message parameters.
 * @param[out] pConnectPacket The buffer containing the serialized data.
 * @param[in] pPacketSize The Size of the serialized data.
 *
 * @return #AWS_IOT_MQTT_SUCCESS or #AWS_IOT_MQTT_NO_MEMORY.
 */
AwsIotMqttError_t AwsIotMqttBLE_SerializeConnect( const AwsIotMqttConnectInfo_t * const pConnectInfo,
                                                           const AwsIotMqttPublishInfo_t * const pWillInfo,
                                                           uint8_t ** const pConnectPacket,
                                                           size_t * const pPacketSize );

/**
 * @brief Deserialize a CONNACK message received from MQTT BLE Proxy.
 *
 * Deserialize the CONNACK parameters from JSON structure format received from MQTT BLE proxy.
 *
 * @param[in] pConnackStart Pointer to start of the CONNACK message within a buffer.
 * @param[in] dataLength Length of buffer containing the CONNACK message.
 * @param[out] pBytesProcessed Length of the CONNACK message parsed.
 *
 * @return  #AWS_IOT_MQTT_SUCCESS or #AWS_IOT_MQTT_BAD_RESPONSE.
 *
 */
AwsIotMqttError_t AwsIotMqttBLE_DeserializeConnack( const uint8_t * const pConnackStart,
                                                         size_t dataLength,
                                                         size_t * const pBytesProcessed );



/**
 * @brief Serialize the PUBLISH message sent to MQTT BLE Proxy.
 *
 * Serialize the PUBLISH message parameters into JSON structure format read by MQTT BLE Proxy.
 *
 * @param[in] pPublishInfo Pointer to the structure containing PUBLISH message parameters.
 * @param[out] pPublishPacket Pointer to the serialized PUBLISH message.
 * @param[out] pPacketSize Length of the serialized PUBLISH message.
 * @param[out] pPacketIdentifier Unique Identifier for the PUBLISH message.
 *
 * @return #AWS_IOT_MQTT_SUCCESS or #AWS_IOT_MQTT_NO_MEMORY.
 *
 */
AwsIotMqttError_t AwsIotMqttBLE_SerializePublish( const AwsIotMqttPublishInfo_t * const pPublishInfo,
                                                       uint8_t ** const pPublishPacket,
                                                       size_t * const pPacketSize,
                                                       uint16_t * const pPacketIdentifier );

/**
 * @brief Set the DUP flag to indicate its a duplicate QoS1/QoS2 message.
 *
 * Sets the DUP flag in the serialized message to indicate its a duplicate QoS1/QoS2 message.
 *
 * @param[in] pPublishOperation The operation containing the PUBLISH message.
 *
 */
void AwsIotMqttBLE_PublishSetDup( bool awsIotMqttMode, uint8_t * const pPublishPacket, uint16_t * const pNewPacketIdentifier );


/**
 * @brief Deserialize the PUBLISH message from MQTT BLE Proxy.
 *
 * Deserializes the PUBLISH parameters from JSON format structure format received from MQTT BLE proxy.
 *
 * @param[in] pPublishStart Pointer to start of the PUBLISH message within a buffer.
 * @param[in] dataLength Length of buffer containing the PUBLISH message.
 * @param[out] pOutput Parameters parsed from PUBLISH message.
 * @param[out] pPacketIdentifier Unique identifier for the PUBLISH message.
 * @param[out] pBytesProcessed Length of the PUBLISH message parsed.
 *
 * @return  #AWS_IOT_MQTT_SUCCESS or #AWS_IOT_MQTT_BAD_RESPONSE.
 *
 */
AwsIotMqttError_t AwsIotMqttBLE_DeserializePublish( const uint8_t * const pPublishStart,
                                                         size_t dataLength,
                                                         AwsIotMqttPublishInfo_t * const pOutput,
                                                         uint16_t * const pPacketIdentifier,
                                                         size_t * const pBytesProcessed );


/**
 * @brief Serialize the PUBACK message sent to MQTT BLE Proxy.
 *
 * Serializes the PUBACK parameters into a JSON structure format read by MQTT BLE proxy.
 *
 * @param[in] packetIdentifier Identifier to be included in the PUBACK message.
 * @param[out] pPubackPacket Pointer to the start of the PUBACK message.
 * @param[out] pPacketSize Length of the PUBACK message.
 *
 * @return #AWS_IOT_MQTT_SUCCESS or #AWS_IOT_MQTT_NO_MEMORY.
 *
 */
AwsIotMqttError_t AwsIotMqttBLE_SerializePuback( uint16_t packetIdentifier,
                                                      uint8_t ** const pPubackPacket,
                                                      size_t * const pPacketSize );
/**
 * @brief Deserialize the PUBACK message from MQTT BLE Proxy.
 *
 * Deserializes the PUBACK parameters from JSON format structure format received from MQTT BLE proxy.
 *
 * @param[in] pPubackStart Pointer to start of the PUBACK message within a buffer.
 * @param[in] dataLength Length of buffer containing the PUBACK message.
 * @param[out] pPacketIdentifier Unique identifier for the PUBACK message.
 * @param[out] pBytesProcessed Length of the PUBACK message parsed.
 *
 * @return  #AWS_IOT_MQTT_SUCCESS or #AWS_IOT_MQTT_BAD_RESPONSE.
 *
 */
AwsIotMqttError_t AwsIotMqttBLE_DeserializePuback( const uint8_t * const pPubackStart,
                                                        size_t dataLength,
                                                        uint16_t * const pPacketIdentifier,
                                                        size_t * const pBytesProcessed );

/**
 * @brief Serialize the SUBSCRIBE message sent to MQTT BLE Proxy.
 *
 * Serialize the SUBSCRIBE message parameters into JSON structure format read by MQTT BLE Proxy.
 *
 * @param[in] pSubscriptionList Pointer to a array of subscriptions.
 * @param[out] subscriptionCount Number of subscriptions.
 * @param[out] pSubscribePacket Pointer to the SUBSCRIBE message.
 * @param[out] pPacketSize Length of the SUBSCRIBE message.
 * @param[out] pPacketIdentifier Unique identifier for the SUBSCRIBE message.
 *
 * @return #AWS_IOT_MQTT_SUCCESS or #AWS_IOT_MQTT_NO_MEMORY.
 *
 */
AwsIotMqttError_t AwsIotMqttBLE_SerializeSubscribe( const AwsIotMqttSubscription_t * const pSubscriptionList,
                                                         size_t subscriptionCount,
                                                         uint8_t ** const pSubscribePacket,
                                                         size_t * const pPacketSize,
                                                         uint16_t * const pPacketIdentifier );
/**
 * @brief Deserialize the SUBACK message from MQTT BLE Proxy.
 *
 * Deserializes the SUBACK parameters from JSON format structure format received from MQTT BLE proxy.
 * Removes the subscription callback from the connection, if it received an error response for a subscription
 *
 * @param[in] mqttConnection The MQTT connection used.
 * @param[in] pSubackStart Start of the SUBACK message within a buffer.
 * @param[in] dataLength Length of the buffer containing SUBACK message
 * @param[out] pPacketIdentifier Unique identifier for the SUBACK message.
 * @param[out] pBytesProcessed Length of the SUBACK message parsed.
 *
 * @return  #AWS_IOT_MQTT_SUCCESS or #AWS_IOT_MQTT_BAD_RESPONSE.
 *
 */
AwsIotMqttError_t AwsIotMqttBLE_DeserializeSuback( AwsIotMqttConnection_t mqttConnection,
                                                        const uint8_t * const pSubackStart,
                                                        size_t dataLength,
                                                        uint16_t * const pPacketIdentifier,
                                                        size_t * const pBytesProcessed );
/**
 * @brief Serialize the UNSUBSCRIBE message sent to MQTT BLE Proxy.
 *
 * Serialize the UNSUBSCRIBE message parameters into JSON structure format read by MQTT BLE Proxy.
 *
 * @param[in] pSubscriptionList Pointer to a array of subscriptions.
 * @param[out] subscriptionCount Number of subscriptions.
 * @param[out] pUnsubscribePacket Pointer to the UNSUBSCRIBE message.
 * @param[out] pPacketSize Length of the UNSUBSCRIBE message.
 * @param[out] pPacketIdentifier Unique identifier for the UNSUBSCRIBE message.
 *
 * @return #AWS_IOT_MQTT_SUCCESS or #AWS_IOT_MQTT_NO_MEMORY.
 *
 */
AwsIotMqttError_t AwsIotMqttBLE_SerializeUnsubscribe( const AwsIotMqttSubscription_t * const pSubscriptionList,
		size_t subscriptionCount,
		uint8_t ** const pUnsubscribePacket,
		size_t * const pPacketSize,
		uint16_t * const pPacketIdentifier );


/**
 * @brief Deserialize the UNSUBACK message from MQTT BLE Proxy.
 *
 * Deserializes the UNSUBACK parameters from JSON format structure format received from MQTT BLE proxy.
 *
 * @param[in] pUnsubackStart Start of the UNSUBACK message within a buffer.
 * @param[in] dataLength Length of the buffer containing UNSUBACK message
 * @param[out] pPacketIdentifier Unique identifier for the UNSUBACK message.
 * @param[out] pBytesProcessed Length of the SUBACK message parsed.
 *
 * @return  #AWS_IOT_MQTT_SUCCESS or #AWS_IOT_MQTT_BAD_RESPONSE.
 *
 */
AwsIotMqttError_t AwsIotMqttBLE_DeserializeUnsuback( const uint8_t * const pUnsubackStart,
                                                          size_t dataLength,
                                                          uint16_t * const pPacketIdentifier,
                                                          size_t * const pBytesProcessed );
/**
 * @brief Serialize the DISCONNECT message sent to MQTT BLE Proxy.
 *
 * Serialize the DISCONNECT message parameters into JSON structure format read by MQTT BLE Proxy.
 *
 * @param[out] pDisconnectPacket Pointer to the DISCONNECT message
 * @param[out] pPacketSize Length of the DISCONNECT message.
 *
 * @return #AWS_IOT_MQTT_SUCCESS or #AWS_IOT_MQTT_NO_MEMORY.
 *
 */
AwsIotMqttError_t AwsIotMqttBLE_SerializeDisconnect( uint8_t ** const pDisconnectPacket,
                                                          size_t * const pPacketSize );

/**
 * @brief Serializes the PING request message.
 *
 * @param[out]  pPingreqPacket Pointer to the PING request packet
 * @param[out]  pPacketSize Size of the PING request packet
 *
 * @return #AWS_IOT_MQTT_SUCCESS or #AWS_IOT_MQTT_NO_MEMORY
 */
AwsIotMqttError_t AwsIotMqttBLE_SerializePingreq( uint8_t ** const pPingreqPacket,
                                                       size_t * const pPacketSize );
/**
 *
 * @brief Deserializes the PING response  message.
 *
 * @param[in] pPingrespStart Pointer to start of buffer containing PING response
 * @param[in] dataLength Length of the buffer containing PING response
 * @param[out] pBytesProcessed Number of bytes parsed
 * @return  #AWS_IOT_MQTT_SUCCESS or #AWS_IOT_MQTT_BAD_RESPONSE
 */
AwsIotMqttError_t AwsIotMqttBLE_DeserializePingresp( const uint8_t * const pPingrespStart,
                                                          size_t dataLength,
                                                          size_t * const pBytesProcessed );

/**
 * @brief Gets the packet type from the packet
 *
 * Parses the JSON packet received and gets the packet type.
 * @param[in] pPacket Pointer to the start of the packet
 * @param[in] packetSize length of the buffer containing the packet
 * @return Packet type for the packet
 */
uint8_t AwsIotMqttBLE_GetPacketType( const uint8_t * const pPacket, size_t packetSize );


/**
 * @brief Frees an MQTT message.
 *
 * @param[in] Pointer to the message.
 */
void AwsIotMqttBLE_FreePacket( uint8_t * pPacket );

#endif /* AWS_IOT_MQTT_SERIALIZE_BLE_H */
