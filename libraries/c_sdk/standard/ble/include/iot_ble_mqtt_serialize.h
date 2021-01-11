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
 * @file iot_ble_mqtt_serialize.h
 * @brief File exposes types and serializer, deserializer APIs for sending and receiving MQTT messages
 * over BLE using a companion mobile device (iOS/Android) SDK.
 */

#ifndef IOT_BLE_MQTT_SERIALIZE_H
#define IOT_BLE_MQTT_SERIALIZE_H

/* bool is defined in only C99+. */
#if defined( __cplusplus ) || ( defined( __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901L ) )
    #include <stdbool.h>
#elif !defined( bool ) && !defined( false ) && !defined( true )
    #define bool     int8_t
    #define false    ( int8_t ) 0
    #define true     ( int8_t ) 1
#endif

/**
 *  @defgroup
 *  Keys for MQTT message parameters exchanged between device and the companion BLE device SDK.
 */
/** @{ */
#define IOT_BLE_MQTT_MSG_TYPE         "w"
#define IOT_BLE_MQTT_CLIENT_ID        "d"
#define IOT_BLE_MQTT_BROKER_EP        "a"
#define IOT_BLE_MQTT_CLEAN_SESSION    "c"
#define IOT_BLE_MQTT_TOPIC            "u"
#define IOT_BLE_MQTT_TOPIC_LIST       "v"
#define IOT_BLE_MQTT_QOS              "n"
#define IOT_BLE_MQTT_QOS_LIST         "o"
#define IOT_BLE_MQTT_MESSAGE_ID       "i"
#define IOT_BLE_MQTT_PAYLOAD          "k"
#define IOT_BLE_MQTT_STATUS           "s"
/** @} */

/**
 * @defgroup
 * MQTT message types exchanged between the device and the companion BLE device SDK.
 */
/** @{ */
#define IOT_BLE_MQTT_MSG_TYPE_CONNECT        ( 1 )
#define IOT_BLE_MQTT_MSG_TYPE_CONNACK        ( 2 )
#define IOT_BLE_MQTT_MSG_TYPE_PUBLISH        ( 3 )
#define IOT_BLE_MQTT_MSG_TYPE_PUBACK         ( 4 )
#define IOT_BLE_MQTT_MSG_TYPE_PUBREC         ( 5 )
#define IOT_BLE_MQTT_MSG_TYPE_PUBREL         ( 6 )
#define IOT_BLE_MQTT_MSG_TYPE_PUBCOMP        ( 7 )
#define IOT_BLE_MQTT_MSG_TYPE_SUBSCRIBE      ( 8 )
#define IOT_BLE_MQTT_MSG_TYPE_SUBACK         ( 9 )
#define IOT_BLE_MQTT_MSG_TYPE_UNSUBSCRIBE    ( 10 )
#define IOT_BLE_MQTT_MSG_TYPE_UNSUBACK       ( 11 )
#define IOT_BLE_MQTT_MSG_TYPE_PINGREQ        ( 12 )
#define IOT_BLE_MQTT_MSG_TYPE_PINGRESP       ( 13 )
#define IOT_BLE_MQTT_MSG_TYPE_DISCONNECT     ( 14 )
#define IOT_BLE_MQTT_MSG_TYPE_INVALID        ( 0xFF )
/** @{ */

/**
 * @defgroup
 * CONNECT Response code exchanged between the device and the companion BLE device SDK.
 */
/** @{ */
typedef enum
{
    IOT_BLE_MQTT_STATUS_UNKNOWN = 0,        /*!< IOT_BLE_MQTT_STATUS_UNKNOWN  Connection status unknown by the SDK. */
    IOT_BLE_MQTT_STATUS_CONNECTING,         /*!< IOT_BLE_MQTT_STATUS_CONNECTING SDK has sent CONNECT request to server and waiting for the response. */
    IOT_BLE_MQTT_STATUS_CONNECTED,          /*!< IOT_BLE_MQTT_STATUS_CONNECTED  SDK is connected to the MQTT server. */
    IOT_BLE_MQTT_STATUS_DISCONNECTED,       /*!< IOT_BLE_MQTT_STATUS_DISCONNECTED SDK is disconnected with the MQTT server. */
    IOT_BLE_MQTT_STATUS_CONNECTION_REFUSED, /*!< IOT_BLE_MQTT_STATUS_CONNECTION_REFUSED Server refused connection with the SDK. */
    IOT_BLE_MQTT_STATUS_CONNECTION_ERROR,   /*!< IOT_BLE_MQTT_STATUS_CONNECTION_ERROR Internal error while connecting to the server. */
    IOT_BLE_MQTT_STATUS_PROTOCOL_ERROR      /*!< IOT_BLE_MQTT_STATUS_PROTOCOL_ERROR CONNECT message from the device was malformed. */
} MQTTBLEConnectStatus_t;
/** @} */

/**
 * @defgroup
 * @brief Quality of service values supported for MQTT over BLE.
 * Qos2 is not supported for BLE.
 */
/** @{ */
typedef enum MQTTBLEQoS
{
    MQTTBLEQoS0 = 0, /**< Delivery at most once. */
    MQTTBLEQoS1 = 1, /**< Delivery at least once. */
} MQTTBLEQoS_t;
/** @} */


/**
 * @defgroup
 * @brief Structure used to serialize the MQTT SUBSCRIBE parameters over BLE.
 */
/** @{ */
typedef struct MQTTBLESubscribeInfo
{
    /**
     * @brief Quality of Service for subscription.
     */
    MQTTBLEQoS_t qos;

    /**
     * @brief Topic filter to subscribe to.
     */
    const char * pTopicFilter;

    /**
     * @brief Length of subscription topic filter.
     */
    uint16_t topicFilterLength;
} MQTTBLESubscribeInfo_t;
/** @} */


/**
 * @defgroup
 * @brief Structure used to serialize MQTT CONNECT packet parameters over BLE.
 */
/** @{ */
typedef struct MQTTBLEConnectInfo
{
    /**
     * @brief Whether to establish a new, clean session or resume a previous session.
     */
    bool cleanSession;

    /**
     * @brief MQTT keep alive period.
     */
    uint16_t keepAliveSeconds;

    /**
     * @brief MQTT client identifier. Must be unique per client.
     */
    const char * pClientIdentifier;

    /**
     * @brief Length of the client identifier.
     */
    uint16_t clientIdentifierLength;

    /**
     * @brief MQTT user name. Set to NULL if not used.
     */
    const char * pUserName;

    /**
     * @brief Length of MQTT user name. Set to 0 if not used.
     */
    uint16_t userNameLength;

    /**
     * @brief MQTT password. Set to NULL if not used.
     */
    const char * pPassword;

    /**
     * @brief Length of MQTT password. Set to 0 if not used.
     */
    uint16_t passwordLength;
} MQTTBLEConnectInfo_t;
/** @} */

/**
 * @defgroup
 * @brief Structure used to serialize MQTT PUBLISH packet parameters over BLE.
 */
/** @{ */
typedef struct MQTTBLEPublishInfo
{
    /**
     * @brief Quality of Service for message.
     */
    MQTTBLEQoS_t qos;

    /**
     * @brief Whether this is a retained message.
     */
    bool retain;

    /**
     * @brief Whether this is a duplicate publish message.
     */
    bool dup;

    /**
     * @brief Topic name on which the message is published.
     */
    const char * pTopicName;

    /**
     * @brief Length of topic name.
     */
    uint16_t topicNameLength;

    /**
     * @brief Message payload.
     */
    const void * pPayload;

    /**
     * @brief Message payload length.
     */
    size_t payloadLength;

    /**
     * @brief Pending flag for continuation of publish packet.
     */
    bool pending;

    /**
     * @brief Packet Identifier for QoS1, QoS2 publish packet.
     */
    uint16_t packetIdentifier;
} MQTTBLEPublishInfo_t;
/** @} */

/**
 * @defgroup
 * @brief Return codes from MQTT BLE serializer APIS
 */
/** @{ */
typedef enum MQTTBLEStatus
{
    MQTTBLESuccess = 0,  /**< Function completed successfully. */
    MQTTBLEBadParameter, /**< At least one parameter was invalid. */
    MQTTBLENoMemory,     /**< A provided buffer was too small. */
    MQTTBLEBadResponse,  /**< An invalid packet was received from the server. */
    MQTTBLEServerRefused /**< Server refused a connection. */
} MQTTBLEStatus_t;


/**
 * @brief Serialize the MQTT CONNECT message sent over BLE connection.
 *
 * Serialize the CONNECT message parameters into a CBOR map to be sent over BLE connection.
 *
 * @param[in] pConnectInfo MQTT CONNECT parameters.
 * @param[in] pWillInfo Will Message parameters.
 * @param[out] pConnectPacket The buffer containing the serialized data.
 * @param[in] pPacketSize The Size of the serialized data.
 *
 * @return #MQTTBLESuccess or #MQTTBLENoMemory.
 */
MQTTBLEStatus_t IotBleMqtt_SerializeConnect( const MQTTBLEConnectInfo_t * const pConnectInfo,
                                             uint8_t ** const pConnectPacket,
                                             size_t * const pPacketSize );

/**
 * @brief Deserialize MQTT CONNACK message received over BLE connection.
 *
 * Deserialize CONNACK parameters from a CBOR map received over BLE connection.
 *
 * @param[in] pBuffer Pointer to start of the CONNACK message within a buffer.
 * @param[in] length Length of buffer containing the CONNACK message.
 *
 * @return  #MQTTBLESuccess or #MQTTBLEBadResponse.
 *
 */
MQTTBLEStatus_t IotBleMqtt_DeserializeConnack( const uint8_t * pBuffer,
                                               size_t length );

/**
 * @brief Serialize MQTT PUBLISH message sent over BLE connection.
 *
 * Serialize PUBLISH message parameters into a CBOR map to be sent over BLE connection.
 *
 * @param[in] pPublishInfo Pointer to the structure containing PUBLISH message parameters.
 * @param[out] pPublishPacket Pointer to the serialized PUBLISH message.
 * @param[out] pPacketSize Length of the serialized PUBLISH message.
 * @param[out] pPacketIdentifier Unique Identifier for the PUBLISH message.
 *
 * @return #MQTTBLESuccess or #MQTTBLENoMemory.
 *
 */
MQTTBLEStatus_t IotBleMqtt_SerializePublish( const MQTTBLEPublishInfo_t * const pPublishInfo,
                                             uint8_t ** const pPublishPacket,
                                             size_t * const pPacketSize,
                                             uint16_t packetIdentifier );

/**
 * @brief Set the DUP flag to indicate its a duplicate QoS1/QoS2 message.
 *
 * Sets the DUP flag in the serialized message to indicate its a duplicate QoS1/QoS2 message.
 *
 * @param[in] pPublishOperation The operation containing the PUBLISH message.
 *
 */
void IotBleMqtt_PublishSetDup( uint8_t * const pPublishPacket,
                               uint8_t * pPacketIdentifierHigh,
                               uint16_t * const pNewPacketIdentifier );

/**
 * @brief Deserialize MQTT PUBLISH message received over BLE connection.
 *
 * Deserializes the PUBLISH parameters from a CBOR map received over BLE connection.
 *
 * @param[in] pBuffer Pointer to start of the PUBLISH message within a buffer.
 * @param[in] length Length of buffer containing the PUBLISH message.
 * @param[out] pPublishInfo Parameters parsed from PUBLISH message.
 * @param[out] pPacketIdentifier Unique identifier for the PUBLISH message.
 *
 * @return  #MQTTBLESuccess or #MQTTBLEBadResponse.
 *
 */
MQTTBLEStatus_t IotBleMqtt_DeserializePublish( uint8_t * pBuffer,
                                               size_t length,
                                               MQTTBLEPublishInfo_t * publishInfo,
                                               uint16_t * packetIdentifier );

/**
 * @brief Serialize MQTT PUBACK message sent over BLE connection.
 *
 * Serializes the PUBACK parameters into a CBOR map to be sent over BLE connection.
 *
 * @param[in] packetIdentifier Identifier to be included in the PUBACK message.
 * @param[out] pPubackPacket Pointer to the start of the PUBACK message.
 * @param[out] pPacketSize Length of the PUBACK message.
 *
 * @return #MQTTBLESuccess or #MQTTBLENoMemory.
 *
 */
MQTTBLEStatus_t IotBleMqtt_SerializePuback( uint16_t packetIdentifier,
                                            uint8_t ** const pPubackPacket,
                                            size_t * const pPacketSize );

/**
 * @brief Deserialize MQTT PUBACK message received from a BLE connection.
 *
 * Deserializes the PUBACK parameters from a CBOR map received over BLE connection.
 *
 * @param[in] pBuffer Pointer to start of the PUBACK message within a buffer.
 * @param[in] length Length of buffer containing the PUBACK message.
 * @param[out] pPacketIdentifier Unique identifier for the PUBACK message.
 *
 * @return  #MQTTBLESuccess or #MQTTBLEBadResponse.
 *
 */
MQTTBLEStatus_t IotBleMqtt_DeserializePuback( uint8_t * pBuffer,
                                              size_t length,
                                              uint16_t * packetIdentifier );

/**
 * @brief Serialize MQTT SUBSCRIBE message sent over BLE connection.
 *
 * Serialize the SUBSCRIBE message parameters into a CBOR map to be sent over BLE connection.
 *
 * @param[in] pSubscriptionList Pointer to a array of subscriptions.
 * @param[out] subscriptionCount Number of subscriptions.
 * @param[out] pSubscribePacket Pointer to the SUBSCRIBE message.
 * @param[out] pPacketSize Length of the SUBSCRIBE message.
 * @param[out] pPacketIdentifier Unique identifier for the SUBSCRIBE message.
 *
 * @return #MQTTBLESuccess or #MQTTBLENoMemory.
 *
 */
MQTTBLEStatus_t IotBleMqtt_SerializeSubscribe( const MQTTBLESubscribeInfo_t * const pSubscriptionList,
                                               size_t subscriptionCount,
                                               uint8_t ** const pSubscribePacket,
                                               size_t * const pPacketSize,
                                               uint16_t * const pPacketIdentifier );

/**
 * @brief Deserialize MQTT SUBACK message received over BLE connection.
 *
 * Deserializes MQTT SUBACK parameters from a CBOR map received over BLE connection.
 *
 * @param[in] pBuffer Pointer to the start of suback packet.
 * @param[in] length  Length of the buffer containing suback packet
 * @param[out] pPacketIdentifier Unique identifier for the SUBACK message.
 * @param[out] pStatusCode Status code of the subscription.
 *
 * @return  #MQTTBLESuccess or #MQTTBLEBadResponse.
 *
 */
MQTTBLEStatus_t IotBleMqtt_DeserializeSuback( const uint8_t * pBuffer,
                                              size_t length,
                                              uint16_t * packetIdentifier,
                                              uint8_t * pStatusCode );

/**
 * @brief Serialize MQTT UNSUBSCRIBE message sent over BLE connection.
 *
 * Serialize the UNSUBSCRIBE message parameters into a CBOR map to be sent over BLE connection.
 *
 * @param[in] pSubscriptionList Pointer to a array of subscriptions.
 * @param[out] subscriptionCount Number of subscriptions.
 * @param[out] pUnsubscribePacket Pointer to the UNSUBSCRIBE message.
 * @param[out] pPacketSize Length of the UNSUBSCRIBE message.
 * @param[out] pPacketIdentifier Unique identifier for the UNSUBSCRIBE message.
 *
 * @return #MQTTSuccess or #MQTTNoMemory.
 *
 */
MQTTBLEStatus_t IotBleMqtt_SerializeUnsubscribe( const MQTTBLESubscribeInfo_t * const pSubscriptionList,
                                                 size_t subscriptionCount,
                                                 uint8_t ** const pUnsubscribePacket,
                                                 size_t * const pPacketSize,
                                                 uint16_t * const pPacketIdentifier );

/**
 * @brief Deserialize MQTT UNSUBACK message received over BLE connection.
 *
 * Deserializes the UNSUBACK parameters from a CBOR map received over BLE connection.
 *
 * @param[in] pBuffer Start of the UNSUBACK message within a buffer.
 * @param[in] length Length of the buffer containing UNSUBACK message
 * @param[out] pPacketIdentifier Unique identifier for the UNSUBACK message.
 *
 * @return  #MQTTBLESuccess or #MQTTBLEBadResponse.
 *
 */
MQTTBLEStatus_t IotBleMqtt_DeserializeUnsuback( uint8_t * pBuffer,
                                                size_t length,
                                                uint16_t * packetIdentifier );

/**
 * @brief Serialize MQTT DISCONNECT message sent over BLE connection.
 *
 * Serialize the DISCONNECT message parameters into a CBOR map to be sent over BLE connection.
 *
 * @param[out] pDisconnectPacket Pointer to the DISCONNECT message
 * @param[out] pPacketSize Length of the DISCONNECT message.
 *
 * @return #MQTTBLESuccess or #MQTTBLENoMemory.
 *
 */
MQTTBLEStatus_t IotBleMqtt_SerializeDisconnect( uint8_t ** const pDisconnectPacket,
                                                size_t * const pPacketSize );

/**
 * @brief Serializes MQTT PING request message sent over BLE connection.
 *
 * Serializes PING request message parameters into a cbor map to be sent over BLE connection.
 *
 * @param[out]  pPingreqPacket Pointer to the PING request packet
 * @param[out]  pPacketSize Size of the PING request packet
 *
 * @return #MQTTBLESuccess or #MQTTBLENoMemory.
 */
MQTTBLEStatus_t IotBleMqtt_SerializePingreq( uint8_t ** const pPingreqPacket,
                                             size_t * const pPacketSize );

/**
 *
 * @brief Deserializes MQTT PINGRESP received over BLE connection.
 *
 * Deserializes PINGRESP parameters from a CBOR map received over BLE connection.
 *
 * @param[in] pBuffer Pointer to start of buffer containing PING response
 * @param[in] length Length of the buffer containing PING response
 * @return  #MQTTSuccess or #MQTTBadResponse.
 */
MQTTBLEStatus_t IotBleMqtt_DeserializePingresp( const uint8_t * pBuffer,
                                                size_t length );

/**
 * @brief Gets the packet type for the MQTT message.
 *
 * Parses the CBOR message received and gets the packet type.
 *
 * @param[in] pBuffer Buffer pointing to the serialized packet
 * @param[in] length Length of the buffer containing the packet
 * @return Packet type for the packet
 */
uint8_t IotBleMqtt_GetPacketType( const uint8_t * pBuffer,
                                  size_t length );


/**
 * @brief Frees an MQTT message.
 *
 * @param[in] Pointer to the message.
 */
void IotBleMqtt_FreePacket( uint8_t * pPacket );

#endif /* IOT_MQTT_SERIALIZE_BLE_H */
