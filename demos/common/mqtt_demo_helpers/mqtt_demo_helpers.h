/*
 * FreeRTOS V202203.00
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 */

#ifndef MQTT_DEMO_HELPERS_H_
#define MQTT_DEMO_HELPERS_H_

/* MQTT API header. */
#include "core_mqtt.h"

/* Transport interface implementation include header for TLS. */
#include "transport_secure_sockets.h"

/* Include header for connection configurations. */
#include "aws_clientcredential.h"

/*------------- Demo configurations -------------------------*/

/** Note: The device client certificate and private key credentials are
 * obtained by the transport interface implementation (with Secure Sockets)
 * from the demos/include/aws_clientcredential_keys.h file.
 *
 * The following macros SHOULD be defined for this demo which uses both server
 * and client authentications for TLS session:
 *   - keyCLIENT_CERTIFICATE_PEM for client certificate.
 *   - keyCLIENT_PRIVATE_KEY_PEM for client private key.
 */

#ifndef democonfigMQTT_BROKER_ENDPOINT
    #define democonfigMQTT_BROKER_ENDPOINT    clientcredentialMQTT_BROKER_ENDPOINT
#endif

#ifndef democonfigCLIENT_IDENTIFIER

/**
 * @brief The MQTT client identifier used in this example.  Each client identifier
 * must be unique so edit as required to ensure no two clients connecting to the
 * same broker use the same client identifier.
 */
    #define democonfigCLIENT_IDENTIFIER    clientcredentialIOT_THING_NAME
#endif

#ifndef democonfigMQTT_BROKER_PORT

/**
 * @brief The port to use for the demo.
 */
    #define democonfigMQTT_BROKER_PORT    clientcredentialMQTT_BROKER_PORT
#endif

/*-----------------------------------------------------------*/

/**
 * @brief Establish a MQTT connection.
 *
 * @param[in] pxContext The memory for the MQTTContext_t that will be used for the
 * MQTT connection.
 * @param[in] pxNetContext The memory for the NetworkContext_t required for the
 * MQTT connection.
 * @param[in] pxNetworkBuffer The buffer space for initializing the @p pxContext MQTT
 * context used in the MQTT connection.
 * @param[in] appCallback The callback function used to receive incoming
 * publishes and incoming acks from MQTT library.
 *
 * @return The status of the final connection attempt.
 */
BaseType_t EstablishMqttSession( MQTTContext_t * pxContext,
                                 NetworkContext_t * pxNetContext,
                                 MQTTFixedBuffer_t * pxNetworkBuffer,
                                 MQTTEventCallback_t eventCallback );

/**
 * @brief Invoked by the event callback to handle SUBACK and UNSUBACK packets
 * from the broker.
 *
 * @param[in] pxPacketInfo Packet Info pointer for the incoming packet.
 * @param[in] usPacketIdentifier Packet identifier of the incoming packet.
 */
void vHandleOtherIncomingPacket( MQTTPacketInfo_t * pxPacketInfo,
                                 uint16_t usPacketIdentifier );

/**
 * @brief Close the MQTT connection.
 *
 * @param[in] pxContext The MQTT context for the MQTT connection to close.
 *
 * @return pdPASS if DISCONNECT was successfully sent;
 * pdFAIL otherwise.
 */
BaseType_t DisconnectMqttSession( MQTTContext_t * pxContext,
                                  NetworkContext_t * pxNetContext );

/**
 * @brief Subscribe to a MQTT topic filter.
 *
 * @param[in] pxContext The MQTT context for the MQTT connection.
 * @param[in] pcTopicFilter Pointer to the shadow topic buffer.
 * @param[in] usTopicFilterLength Indicates the length of the shadow
 * topic buffer.
 *
 * @return pdPASS if SUBSCRIBE was successfully sent;
 * pdFAIL otherwise.
 */
BaseType_t SubscribeToTopic( MQTTContext_t * pxContext,
                             const char * pcTopicFilter,
                             uint16_t usTopicFilterLength );

/**
 * @brief Sends an MQTT UNSUBSCRIBE to unsubscribe from the shadow
 * topic.
 *
 * @param[in] pxContext The MQTT context for the MQTT connection.
 * @param[in] pcTopicFilter Pointer to the shadow topic buffer.
 * @param[in] usTopicFilterLength Indicates the length of the shadow
 * topic buffer.
 *
 * @return pdPASS if UNSUBSCRIBE was successfully sent;
 * pdFAIL otherwise.
 */
BaseType_t UnsubscribeFromTopic( MQTTContext_t * pxContext,
                                 const char * pcTopicFilter,
                                 uint16_t usTopicFilterLength );

/**
 * @brief Publish a message to a MQTT topic.
 *
 * @param[in] pxContext The MQTT context for the MQTT connection.
 * @param[in] pcTopicFilter Points to the topic.
 * @param[in] topicFilterLength The length of the topic.
 * @param[in] pcPayload Points to the payload.
 * @param[in] payloadLength The length of the payload.
 *
 * @return pdPASS if PUBLISH was successfully sent;
 * pdFAIL otherwise.
 */
BaseType_t PublishToTopic( MQTTContext_t * pxContext,
                           const char * pcTopicFilter,
                           int32_t topicFilterLength,
                           const char * pcPayload,
                           size_t payloadLength );

/**
 * @brief Invoke the core MQTT library's process loop function.
 *
 * @param[in] pxMqttContext The MQTT context for the MQTT connection.
 * @param[in] ulTimeoutMs Minimum time for the loop to run, if no error occurs.
 *
 * @return pdPASS if process loop was successful;
 * pdFAIL otherwise.
 */
BaseType_t ProcessLoop( MQTTContext_t * pxMqttContext,
                        uint32_t ulTimeoutMs );

#endif /* ifndef MQTT_DEMO_HELPERS_H_ */
