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

/**
 * @file mqtt_demo_helpers.c
 *
 * @brief This file provides helper functions used by the Shadow demo application to
 * do MQTT operations over a mutually authenticated TLS connection.
 *
 * A mutually authenticated TLS connection is used to connect to the AWS IoT
 * MQTT message broker in this example.  Define democonfigMQTT_BROKER_ENDPOINT
 * and democonfigROOT_CA_PEM, in shadow_demo_config.h, and the client
 * private key and certificate, in aws_clientcredential_keys.h, to establish a
 * mutually authenticated connection.
 */

/* Standard includes. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* MQTT API header. */
#include "core_mqtt.h"

/* Shadow includes */
#include "mqtt_demo_helpers.h"

/* Include AWS IoT metrics macros header. */
#include "aws_iot_metrics.h"

/* Retry utilities include. */
#include "backoff_algorithm.h"

/* Include PKCS11 helpers header. */
#include "pkcs11_helpers.h"

/* Include header for client credentials. */
#include "aws_clientcredential.h"

/* Include header for root CA certificates. */
#include "iot_default_root_certificates.h"

/* Include the secure sockets implementation of the transport interface. */
#include "transport_secure_sockets.h"

/*-----------------------------------------------------------*/

/**
 * @brief The maximum number of retries for connecting to server.
 */
#define CONNECTION_RETRY_MAX_ATTEMPTS                ( 5U )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying connection to server.
 */
#define CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS        ( 5000U )

/**
 * @brief The base back-off delay (in milliseconds) to use for connection retry attempts.
 */
#define CONNECTION_RETRY_BACKOFF_BASE_MS             ( 500U )

/**
 * @brief The maximum number of retries for subscribing to topic filter when broker rejects an attempt.
 */
#define SUBSCRIBE_RETRY_MAX_ATTEMPTS                 ( 4U )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying subscription to topic.
 */
#define SUBSCRIBE_RETRY_MAX_BACKOFF_DELAY_MS         ( 2500U )

/**
 * @brief The base back-off delay (in milliseconds) to use for subscription retry attempts.
 */
#define SUBSCRIBE_RETRY_BACKOFF_BASE_MS              ( 500U )

/**
 * @brief Timeout for receiving CONNACK packet in milliseconds.
 */
#define mqttexampleCONNACK_RECV_TIMEOUT_MS           ( 1000U )

/**
 * @brief The number of topic filters to subscribe.
 */
#define mqttexampleTOPIC_COUNT                       ( 1 )

/**
 * @brief Time to wait between each cycle of the demo implemented by prvMQTTDemoTask().
 */
#define mqttexampleDELAY_BETWEEN_DEMO_ITERATIONS     ( pdMS_TO_TICKS( 5000U ) )

/**
 * @brief Timeout for MQTT_ProcessLoop in milliseconds.
 */
#define mqttexamplePROCESS_LOOP_TIMEOUT_MS           ( 500U )

/**
 * @brief The maximum number of times to call MQTT_ProcessLoop() when polling
 * for a specific packet from the broker.
 */
#define MQTT_PROCESS_LOOP_PACKET_WAIT_COUNT_MAX      ( 30U )

/**
 * @brief Keep alive time reported to the broker while establishing an MQTT connection.
 *
 * It is the responsibility of the Client to ensure that the interval between
 * Control Packets being sent does not exceed the this Keep Alive value. In the
 * absence of sending any other Control Packets, the Client MUST send a
 * PINGREQ Packet.
 */
#define mqttexampleKEEP_ALIVE_TIMEOUT_SECONDS        ( 60U )

/**
 * @brief Delay between MQTT publishes. Note that the process loop also has a
 * timeout, so the total time between publishes is the sum of the two delays.
 */
#define mqttexampleDELAY_BETWEEN_PUBLISHES           ( pdMS_TO_TICKS( 500U ) )

/**
 * @brief Transport timeout in milliseconds for transport send and receive.
 */
#define mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS    ( 200U )

/**
 * @brief Maximum number of outgoing publishes maintained in the application
 * until an ack is received from the broker.
 */
#define MAX_OUTGOING_PUBLISHES                       ( 1U )

/**
 * @brief Milliseconds per second.
 */
#define MILLISECONDS_PER_SECOND                      ( 1000U )

/**
 * @brief Milliseconds per FreeRTOS tick.
 */
#define MILLISECONDS_PER_TICK                        ( MILLISECONDS_PER_SECOND / configTICK_RATE_HZ )

/*-----------------------------------------------------------*/

/**
 * @brief Each compilation unit that consumes the NetworkContext must define it.
 * It should contain a single pointer to the type of your desired transport.
 * When using multiple transports in the same compilation unit, define this pointer as void *.
 *
 * @note Transport stacks are defined in amazon-freertos/libraries/abstractions/transport/secure_sockets/transport_secure_sockets.h.
 */
struct NetworkContext
{
    SecureSocketsTransportParams_t * pParams;
};

/*-----------------------------------------------------------*/

/**
 * @brief Structure to keep the MQTT publish packets until an ack is received
 * for QoS1 publishes.
 */
typedef struct PublishPackets
{
    /**
     * @brief Packet identifier of the publish packet.
     */
    uint16_t packetId;

    /**
     * @brief Publish info of the publish packet.
     */
    MQTTPublishInfo_t pubInfo;
} PublishPackets_t;

/**
 * @brief Global entry time into the application to use as a reference timestamp
 * in the #prvGetTimeMs function. #prvGetTimeMs will always return the difference
 * between the current time and the global entry time. This will reduce the chances
 * of overflow for the 32 bit unsigned integer used for holding the timestamp.
 */
static uint32_t ulGlobalEntryTimeMs;

/*-----------------------------------------------------------*/

/**
 * @brief Packet Identifier generated when Subscribe request was sent to the broker;
 * it is used to match received Subscribe ACK to the transmitted subscribe.
 */
static uint16_t globalSubscribePacketIdentifier = 0U;

/**
 * @brief Packet Identifier generated when Unsubscribe request was sent to the broker;
 * it is used to match received Unsubscribe ACK to the transmitted unsubscribe
 * request.
 */
static uint16_t globalUnsubscribePacketIdentifier = 0U;

/**
 * @brief MQTT packet type received from the MQTT broker.
 *
 * @note Only on receiving incoming SUBACK or UNSUBACK, this
 * variable is updated. For MQTT packets PUBACK and PINGRESP, the variable is
 * not updated since there is no need to specifically wait for it in this demo.
 * A single variable suffices as this demo uses single task and requests one operation
 * (of SUBSCRIBE or UNSUBSCRIBE) at a time before expecting response from
 * the broker. Hence it is not possible to receive multiple packets of type
 * SUBACK and UNSUBACK in a single call of #prvWaitForPacket.
 * For a multi task application, consider a different method to wait for the packet, if needed.
 */
static uint16_t globalPacketTypeReceived = 0U;

/**
 * @brief A pair containing a topic filter and its SUBACK status.
 */
typedef struct topicFilterContext
{
    const char * pcTopicFilter;
    MQTTSubAckStatus_t xSubAckStatus;
} topicFilterContext_t;

/**
 * @brief A struct containing the context of a SUBACK; the SUBACK status
 * of a filter is updated when the event callback processes a SUBACK.
 */
static topicFilterContext_t xTopicFilterContext;

/**
 * @brief Array to keep the outgoing publish messages.
 * These stored outgoing publish messages are kept until a successful ack
 * is received.
 */
static PublishPackets_t outgoingPublishPackets[ MAX_OUTGOING_PUBLISHES ] = { 0 };

/**
 * @brief The flag to indicate the mqtt session changed.
 */
static bool mqttSessionEstablished = false;

/**
 * @brief The parameters for the network context using a TLS channel.
 */
static SecureSocketsTransportParams_t xSecureSocketsTransportParams;

/*-----------------------------------------------------------*/

/**
 * @brief Calculate and perform an exponential backoff with jitter delay for
 * the next retry attempt of a failed network operation with the server.
 *
 * The function generates a random number, calculates the next backoff period
 * with the generated random number, and performs the backoff delay operation if the
 * number of retries have not exhausted.
 *
 * @note The PKCS11 module is used to generate the random number as it allows access
 * to a True Random Number Generator (TRNG) if the vendor platform supports it.
 * It is recommended to seed the random number generator with a device-specific entropy
 * source so that probability of collisions from devices in connection retries is mitigated.
 *
 * @note The backoff period is calculated using the backoffAlgorithm library.
 *
 * @param[in, out] pxRetryAttempts The context to use for backoff period calculation
 * with the backoffAlgorithm library.
 *
 * @return pdPASS if calculating the backoff period was successful; otherwise pdFAIL
 * if there was failure in random number generation OR all retry attempts had exhausted.
 */
static BaseType_t prvBackoffForRetry( BackoffAlgorithmContext_t * pxRetryParams );

/**
 * @brief Connect to MQTT broker with reconnection retries.
 *
 * If connection fails, retry is attempted after a timeout.
 * Timeout value will exponentially increase until maximum
 * timeout value is reached or the number of attempts are exhausted.
 *
 * @param[out] pxNetworkContext The output parameter to return the created network context.
 *
 * @return The status of the final connection attempt.
 */
static TransportSocketStatus_t prvConnectToServerWithBackoffRetries( NetworkContext_t * pxNetworkContext );

/**
 * @brief Helper function to wait for a specific incoming packet from the
 * broker.
 *
 * @param[in] pxMQTTContext MQTT context pointer.
 * @param[in] usPacketType Packet type to wait for.
 *
 * @return pdFAIL if the expected packet from the broker never arrives;
 * pdPASS if it arrives.
 */
static BaseType_t prvWaitForPacket( MQTTContext_t * pxMQTTContext,
                                    uint16_t usPacketType );

/**
 * @brief Function to get the free index at which an outgoing publish
 * can be stored.
 *
 * @param[out] pucIndex The output parameter to return the index at which an
 * outgoing publish message can be stored.
 *
 * @return pdFAIL if no more publishes can be stored;
 * pdPASS if an index to store the next outgoing publish is obtained.
 */
static BaseType_t prvGetNextFreeIndexForOutgoingPublishes( uint8_t * pucIndex );

/**
 * @brief Function to clean up an outgoing publish at given index from the
 * #outgoingPublishPackets array.
 *
 * @param[in] ucIndex The index at which a publish message has to be cleaned up.
 */
static void vCleanupOutgoingPublishAt( uint8_t ucIndex );

/**
 * @brief Function to clean up all the outgoing publishes maintained in the
 * array.
 */
static void vCleanupOutgoingPublishes( void );

/**
 * @brief Function to clean up the publish packet with the given packet id.
 *
 * @param[in] usPacketId Packet identifier of the packet to be cleaned up from
 * the array.
 */
static void vCleanupOutgoingPublishWithPacketID( uint16_t usPacketId );

/**
 * @brief Function to resend the publishes if a session is re-established with
 * the broker. This function handles the resending of the QoS1 publish packets,
 * which are maintained locally.
 *
 * @param[in] pxMqttContext MQTT context pointer.
 */
static BaseType_t handlePublishResend( MQTTContext_t * pxMqttContext );

/**
 * @brief The timer query function provided to the MQTT context.
 *
 * @return Time in milliseconds.
 */
static uint32_t prvGetTimeMs( void );

/*-----------------------------------------------------------*/

static BaseType_t prvBackoffForRetry( BackoffAlgorithmContext_t * pxRetryParams )
{
    BaseType_t xReturnStatus = pdFAIL;
    uint16_t usNextRetryBackOff = 0U;
    BackoffAlgorithmStatus_t xBackoffAlgStatus = BackoffAlgorithmSuccess;

    /**
     * To calculate the backoff period for the next retry attempt, we will
     * generate a random number to provide to the backoffAlgorithm library.
     *
     * Note: The PKCS11 module is used to generate the random number as it allows access
     * to a True Random Number Generator (TRNG) if the vendor platform supports it.
     * It is recommended to use a random number generator seeded with a device-specific
     * entropy source so that probability of collisions from devices in connection retries
     * is mitigated.
     */
    uint32_t ulRandomNum = 0;

    if( xPkcs11GenerateRandomNumber( ( uint8_t * ) &ulRandomNum,
                                     sizeof( ulRandomNum ) ) == pdPASS )
    {
        /* Get back-off value (in milliseconds) for the next retry attempt. */
        xBackoffAlgStatus = BackoffAlgorithm_GetNextBackoff( pxRetryParams, ulRandomNum, &usNextRetryBackOff );

        if( xBackoffAlgStatus == BackoffAlgorithmRetriesExhausted )
        {
            LogError( ( "All retry attempts have exhausted. Operation will not be retried" ) );
        }
        else if( xBackoffAlgStatus == BackoffAlgorithmSuccess )
        {
            /* Perform the backoff delay. */
            vTaskDelay( pdMS_TO_TICKS( usNextRetryBackOff ) );

            xReturnStatus = pdPASS;

            LogInfo( ( "Retry attempt %lu out of maximum retry attempts %lu.",
                       ( pxRetryParams->attemptsDone + 1 ),
                       pxRetryParams->maxRetryAttempts ) );
        }
    }
    else
    {
        LogError( ( "Unable to retry operation with broker: Random number generation failed" ) );
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

static TransportSocketStatus_t prvConnectToServerWithBackoffRetries( NetworkContext_t * pxNetworkContext )
{
    TransportSocketStatus_t xNetworkStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;
    BackoffAlgorithmContext_t xReconnectParams = { 0 };
    BaseType_t xBackoffStatus = pdPASS;
    ServerInfo_t xServerInfo = { 0 };
    SocketsConfig_t xSocketConfig = { 0 };

    /* Initialize information to connect to the MQTT broker. */
    xServerInfo.pHostName = democonfigMQTT_BROKER_ENDPOINT;
    xServerInfo.hostNameLength = sizeof( democonfigMQTT_BROKER_ENDPOINT ) - 1U;
    xServerInfo.port = democonfigMQTT_BROKER_PORT;

    /* Set the Secure Socket configurations. */
    xSocketConfig.enableTls = true;

    /* Pass the ALPN protocol name depending on the port being used.
     * Please see more details about the ALPN protocol for the AWS IoT MQTT
     * endpoint in the link below.
     * https://aws.amazon.com/blogs/iot/mqtt-with-tls-client-authentication-on-port-443-why-it-is-useful-and-how-it-works/
     */
    if( xServerInfo.port == 443 )
    {
        xSocketConfig.pAlpnProtos = socketsAWS_IOT_ALPN_MQTT;
    }

    xSocketConfig.maxFragmentLength = 0;
    xSocketConfig.disableSni = false;
    xSocketConfig.sendTimeoutMs = mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS;
    xSocketConfig.recvTimeoutMs = mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS;

    /* Initialize reconnect attempts and interval. */
    BackoffAlgorithm_InitializeParams( &xReconnectParams,
                                       CONNECTION_RETRY_BACKOFF_BASE_MS,
                                       CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS,
                                       CONNECTION_RETRY_MAX_ATTEMPTS );

    /* Attempt to connect to MQTT broker. If connection fails, retry after
     * a timeout. Timeout value will exponentially increase until maximum
     * attempts are reached.
     */
    do
    {
        /* Establish a TCP connection with the MQTT broker. This example connects to
         * the MQTT broker as specified in democonfigMQTT_BROKER_ENDPOINT and
         * democonfigMQTT_BROKER_PORT at the top of this file. */
        LogInfo( ( "Create a TCP connection to %s:%d.",
                   democonfigMQTT_BROKER_ENDPOINT,
                   democonfigMQTT_BROKER_PORT ) );
        xNetworkStatus = SecureSocketsTransport_Connect( pxNetworkContext,
                                                         &xServerInfo,
                                                         &xSocketConfig );

        if( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
        {
            LogWarn( ( "Connection to the broker failed. Attempting connection retry after backoff delay." ) );

            /* As the connection attempt failed, we will retry the connection after an
             * exponential backoff with jitter delay. */

            /* Calculate the backoff period for the next retry attempt and perform the wait operation. */
            xBackoffStatus = prvBackoffForRetry( &xReconnectParams );
        }
    } while( ( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS ) && ( xBackoffStatus == pdPASS ) );

    return xNetworkStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prvWaitForPacket( MQTTContext_t * pxMQTTContext,
                                    uint16_t usPacketType )
{
    uint8_t ucCount = 0U;
    MQTTStatus_t xMQTTStatus = MQTTSuccess;
    BaseType_t xStatus = pdFAIL;

    /* Reset the packet type received. */
    globalPacketTypeReceived = 0U;

    /* Call MQTT_ProcessLoop multiple times over small timeouts instead of a single
     * large timeout so that we can unblock immediately on receiving the packet. */
    while( ( globalPacketTypeReceived != usPacketType ) &&
           ( ucCount++ < MQTT_PROCESS_LOOP_PACKET_WAIT_COUNT_MAX ) &&
           ( xMQTTStatus == MQTTSuccess ) )
    {
        /* Event callback will set #usPacketTypeReceived when receiving appropriate packet. This
         * will wait for at most mqttexamplePROCESS_LOOP_TIMEOUT_MS. */
        xMQTTStatus = MQTT_ProcessLoop( pxMQTTContext, mqttexamplePROCESS_LOOP_TIMEOUT_MS );
    }

    if( ( xMQTTStatus != MQTTSuccess ) || ( globalPacketTypeReceived != usPacketType ) )
    {
        xStatus = pdFAIL;
        LogError( ( "MQTT_ProcessLoop failed to receive packet: Packet type=%02X, LoopDuration=%u, Status=%s",
                    usPacketType,
                    ( mqttexamplePROCESS_LOOP_TIMEOUT_MS * ucCount ),
                    MQTT_Status_strerror( xMQTTStatus ) ) );
    }
    else
    {
        xStatus = pdPASS;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BaseType_t SubscribeToTopic( MQTTContext_t * pxMqttContext,
                             const char * pcTopicFilter,
                             uint16_t usTopicFilterLength )
{
    MQTTStatus_t xMqttStatus = MQTTSuccess;
    BackoffAlgorithmContext_t xRetryParams;
    BaseType_t xBackoffStatus = pdFAIL;
    BaseType_t xSubscribeStatus = pdFAIL;
    MQTTSubscribeInfo_t xMQTTSubscription;

    assert( pxMqttContext != NULL );
    assert( pcTopicFilter != NULL );
    assert( usTopicFilterLength > 0 );

    /* Some fields not used so start with everything at 0. */
    ( void ) memset( ( void * ) &xMQTTSubscription, 0x00, sizeof( xMQTTSubscription ) );

    /* Initialize the status of the subscription acknowledgement. */
    xTopicFilterContext.pcTopicFilter = pcTopicFilter;
    xTopicFilterContext.xSubAckStatus = MQTTSubAckFailure;

    /* Get a unique packet id. */
    globalSubscribePacketIdentifier = MQTT_GetPacketId( pxMqttContext );

    /* Subscribe to the #pcTopicFilter topic filter that is passed by a demo application. */
    xMQTTSubscription.qos = MQTTQoS1;
    xMQTTSubscription.pTopicFilter = pcTopicFilter;
    xMQTTSubscription.topicFilterLength = usTopicFilterLength;

    /* Initialize retry attempts and interval. */
    BackoffAlgorithm_InitializeParams( &xRetryParams,
                                       SUBSCRIBE_RETRY_BACKOFF_BASE_MS,
                                       SUBSCRIBE_RETRY_MAX_BACKOFF_DELAY_MS,
                                       SUBSCRIBE_RETRY_MAX_ATTEMPTS );

    do
    {
        xSubscribeStatus = pdFAIL;

        /* The client should now be connected to the broker. Subscribe to the topic
         * as specified in #pcTopicFilter by sending a subscribe packet. */
        LogInfo( ( "Attempt to subscribe to the MQTT topic %s.", pcTopicFilter ) );
        xMqttStatus = MQTT_Subscribe( pxMqttContext,
                                      &xMQTTSubscription,
                                      sizeof( xMQTTSubscription ) / sizeof( MQTTSubscribeInfo_t ),
                                      globalSubscribePacketIdentifier );

        if( xMqttStatus != MQTTSuccess )
        {
            LogError( ( "Failed to SUBSCRIBE to MQTT topic %s. Error=%s",
                        pcTopicFilter, usTopicFilterLength ) );
        }
        else
        {
            LogInfo( ( "SUBSCRIBE sent for topic %s to broker.", pcTopicFilter ) );

            /* Process incoming packet from the broker. After sending the subscribe, the
             * client may receive a publish before it receives a subscribe ack. Therefore,
             * call generic incoming packet processing function. The application
             * must be ready to receive any packet. This demo uses the generic packet
             * processing function everywhere to highlight this fact. */
            xSubscribeStatus = prvWaitForPacket( pxMqttContext, MQTT_PACKET_TYPE_SUBACK );
        }

        if( xSubscribeStatus == pdFAIL )
        {
            LogError( ( "SUBACK never arrived for subscription attempt to topic %s.",
                        xTopicFilterContext.pcTopicFilter ) );
            break;
        }
        else
        {
            /* Check if recent subscription request has been rejected. #xTopicFilterContext is updated
             * in the event callback to reflect the status of the SUBACK sent by the broker. It represents
             * either the QoS level granted by the server upon subscription, or acknowledgement of
             * server rejection of the subscription request. */
            if( xTopicFilterContext.xSubAckStatus == MQTTSubAckFailure )
            {
                xSubscribeStatus = pdFAIL;

                /* As the subscribe attempt failed, we will retry the subscription request after an
                 * exponential backoff with jitter delay. */

                /* Retry subscribe after exponential back-off. */
                LogWarn( ( "Server rejected subscription request. Attempting to re-subscribe to topic %s.",
                           xTopicFilterContext.pcTopicFilter ) );

                xBackoffStatus = prvBackoffForRetry( &xRetryParams );
            }
        }
    } while( ( xSubscribeStatus == pdFAIL ) && ( xBackoffStatus == pdPASS ) );

    return xSubscribeStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prvGetNextFreeIndexForOutgoingPublishes( uint8_t * pucIndex )
{
    BaseType_t xReturnStatus = pdFAIL;
    uint8_t ucIndex = 0;

    assert( outgoingPublishPackets != NULL );
    assert( pucIndex != NULL );

    for( ucIndex = 0; ucIndex < MAX_OUTGOING_PUBLISHES; ucIndex++ )
    {
        /* A free ucIndex is marked by invalid packet id.
         * Check if the the ucIndex has a free slot. */
        if( outgoingPublishPackets[ ucIndex ].packetId == MQTT_PACKET_ID_INVALID )
        {
            xReturnStatus = pdPASS;
            break;
        }
    }

    /* Copy the available ucIndex into the output param. */
    *pucIndex = ucIndex;

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

static void vCleanupOutgoingPublishAt( uint8_t ucIndex )
{
    assert( outgoingPublishPackets != NULL );
    assert( ucIndex < MAX_OUTGOING_PUBLISHES );

    /* Clear the outgoing publish packet. */
    ( void ) memset( &( outgoingPublishPackets[ ucIndex ] ),
                     0x00,
                     sizeof( outgoingPublishPackets[ ucIndex ] ) );
}

/*-----------------------------------------------------------*/

static void vCleanupOutgoingPublishes( void )
{
    assert( outgoingPublishPackets != NULL );

    /* Clean up all the outgoing publish packets. */
    ( void ) memset( outgoingPublishPackets, 0x00, sizeof( outgoingPublishPackets ) );
}

/*-----------------------------------------------------------*/

static void vCleanupOutgoingPublishWithPacketID( uint16_t usPacketId )
{
    uint8_t ucIndex = 0;

    assert( outgoingPublishPackets != NULL );
    assert( usPacketId != MQTT_PACKET_ID_INVALID );

    /* Clean up all the saved outgoing publishes. */
    for( ; ucIndex < MAX_OUTGOING_PUBLISHES; ucIndex++ )
    {
        if( outgoingPublishPackets[ ucIndex ].packetId == usPacketId )
        {
            vCleanupOutgoingPublishAt( ucIndex );
            LogInfo( ( "Cleaned up outgoing publish packet with packet id %u.\n\n",
                       usPacketId ) );
            break;
        }
    }
}

/*-----------------------------------------------------------*/

void vHandleOtherIncomingPacket( MQTTPacketInfo_t * pxPacketInfo,
                                 uint16_t usPacketIdentifier )
{
    MQTTStatus_t xResult = MQTTSuccess;
    uint8_t * pucPayload = NULL;
    size_t xSize = 0;

    /* Handle other packets. */
    switch( pxPacketInfo->type )
    {
        case MQTT_PACKET_TYPE_SUBACK:
            LogInfo( ( "MQTT_PACKET_TYPE_SUBACK.\n\n" ) );
            /* Update the packet type received to SUBACK. */
            globalPacketTypeReceived = MQTT_PACKET_TYPE_SUBACK;

            /* A SUBACK from the broker, containing the server response to our subscription request, has been received.
             * It contains the status code indicating server approval/rejection for the subscription to the single topic
             * requested. The SUBACK will be parsed to obtain the status code, and this status code will be stored in global
             * variable #xTopicFilterContext. */
            xResult = MQTT_GetSubAckStatusCodes( pxPacketInfo, &pucPayload, &xSize );

            /* MQTT_GetSubAckStatusCodes always returns success if called with packet info
             * from the event callback and non-NULL parameters. */
            configASSERT( xResult == MQTTSuccess );
            /* Only a single topic filter is expected for each SUBSCRIBE packet. */
            configASSERT( xSize == 1UL );
            xTopicFilterContext.xSubAckStatus = ( MQTTSubAckStatus_t ) *pucPayload;

            if( xTopicFilterContext.xSubAckStatus != MQTTSubAckFailure )
            {
                LogInfo( ( "Subscribed to the topic %s with maximum QoS %u.",
                           xTopicFilterContext.pcTopicFilter,
                           xTopicFilterContext.xSubAckStatus ) );
            }

            /* Make sure ACK packet identifier matches with Request packet identifier. */
            assert( globalSubscribePacketIdentifier == usPacketIdentifier );
            break;

        case MQTT_PACKET_TYPE_UNSUBACK:
            LogInfo( ( "MQTT_PACKET_TYPE_UNSUBACK.\n\n" ) );
            /* Update the packet type received to UNSUBACK. */
            globalPacketTypeReceived = MQTT_PACKET_TYPE_UNSUBACK;
            /* Make sure ACK packet identifier matches with Request packet identifier. */
            assert( globalUnsubscribePacketIdentifier == usPacketIdentifier );
            break;

        case MQTT_PACKET_TYPE_PINGRESP:

            /* Nothing to be done from application as library handles
             * PINGRESP. */
            LogWarn( ( "PINGRESP should not be handled by the application "
                       "callback when using MQTT_ProcessLoop.\n\n" ) );
            break;

        case MQTT_PACKET_TYPE_PUBACK:
            LogInfo( ( "PUBACK received for packet id %u.\n\n",
                       usPacketIdentifier ) );
            /* Cleanup publish packet when a PUBACK is received. */
            vCleanupOutgoingPublishWithPacketID( usPacketIdentifier );
            break;

        /* Any other packet type is invalid. */
        default:
            LogError( ( "Unknown packet type received:(%02x).\n\n",
                        pxPacketInfo->type ) );
    }
}

/*-----------------------------------------------------------*/

static BaseType_t handlePublishResend( MQTTContext_t * pxMqttContext )
{
    BaseType_t xReturnStatus = pdPASS;
    MQTTStatus_t eMqttStatus = MQTTSuccess;
    uint8_t ucIndex = 0U;

    assert( outgoingPublishPackets != NULL );

    /* Resend all the QoS1 publishes still in the array. These are the
     * publishes that hasn't received a PUBACK. When a PUBACK is
     * received, the publish is removed from the array. */
    for( ucIndex = 0U; ucIndex < MAX_OUTGOING_PUBLISHES; ucIndex++ )
    {
        if( outgoingPublishPackets[ ucIndex ].packetId != MQTT_PACKET_ID_INVALID )
        {
            outgoingPublishPackets[ ucIndex ].pubInfo.dup = true;

            LogInfo( ( "Sending duplicate PUBLISH with packet id %u.",
                       outgoingPublishPackets[ ucIndex ].packetId ) );
            eMqttStatus = MQTT_Publish( pxMqttContext,
                                        &outgoingPublishPackets[ ucIndex ].pubInfo,
                                        outgoingPublishPackets[ ucIndex ].packetId );

            if( eMqttStatus != MQTTSuccess )
            {
                LogError( ( "Sending duplicate PUBLISH for packet id %u "
                            " failed with status %s.",
                            outgoingPublishPackets[ ucIndex ].packetId,
                            MQTT_Status_strerror( eMqttStatus ) ) );
                xReturnStatus = pdFAIL;
                break;
            }
            else
            {
                LogInfo( ( "Sent duplicate PUBLISH successfully for packet id %u.\n\n",
                           outgoingPublishPackets[ ucIndex ].packetId ) );
            }
        }
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

BaseType_t EstablishMqttSession( MQTTContext_t * pxMqttContext,
                                 NetworkContext_t * pxNetworkContext,
                                 MQTTFixedBuffer_t * pxNetworkBuffer,
                                 MQTTEventCallback_t eventCallback )
{
    BaseType_t xReturnStatus = pdPASS;
    MQTTStatus_t eMqttStatus = MQTTSuccess;
    MQTTConnectInfo_t xConnectInfo = { 0 };
    TransportInterface_t xTransport = { 0 };
    bool sessionPresent = false;

    assert( pxMqttContext != NULL );
    assert( pxNetworkContext != NULL );

    /* Initialize the mqtt context and network context. */
    ( void ) memset( pxMqttContext, 0U, sizeof( MQTTContext_t ) );
    ( void ) memset( pxNetworkContext, 0U, sizeof( NetworkContext_t ) );


    pxNetworkContext->pParams = &xSecureSocketsTransportParams;

    if( prvConnectToServerWithBackoffRetries( pxNetworkContext ) != TRANSPORT_SOCKET_STATUS_SUCCESS )
    {
        /* Log error to indicate connection failure after all
         * reconnect attempts are over. */
        LogError( ( "Failed to connect to MQTT broker %.*s.",
                    strlen( democonfigMQTT_BROKER_ENDPOINT ),
                    democonfigMQTT_BROKER_ENDPOINT ) );
        xReturnStatus = pdFAIL;
    }
    else
    {
        /* Fill in Transport Interface send and receive function pointers. */
        xTransport.pNetworkContext = pxNetworkContext;
        xTransport.send = SecureSocketsTransport_Send;
        xTransport.recv = SecureSocketsTransport_Recv;

        /* Initialize MQTT library. */
        eMqttStatus = MQTT_Init( pxMqttContext,
                                 &xTransport,
                                 prvGetTimeMs,
                                 eventCallback,
                                 pxNetworkBuffer );

        if( eMqttStatus != MQTTSuccess )
        {
            xReturnStatus = pdFAIL;
            LogError( ( "MQTT init failed with status %s.",
                        MQTT_Status_strerror( eMqttStatus ) ) );
        }
        else
        {
            /* Establish MQTT session by sending a CONNECT packet. */

            /* Many fields not used in this demo so start with everything at 0. */
            ( void ) memset( ( void * ) &xConnectInfo, 0x00, sizeof( xConnectInfo ) );

            /* Start with a clean session i.e. direct the MQTT broker to discard any
             * previous session data. Also, establishing a connection with clean session
             * will ensure that the broker does not store any data when this client
             * gets disconnected. */
            xConnectInfo.cleanSession = true;

            /* The client identifier is used to uniquely identify this MQTT client to
             * the MQTT broker. In a production device the identifier can be something
             * unique, such as a device serial number. */
            xConnectInfo.pClientIdentifier = democonfigCLIENT_IDENTIFIER;
            xConnectInfo.clientIdentifierLength = ( uint16_t ) strlen( democonfigCLIENT_IDENTIFIER );

            /* Use the metrics string as username to report the OS and MQTT client version
             * metrics to AWS IoT. */
            xConnectInfo.pUserName = AWS_IOT_METRICS_STRING;
            xConnectInfo.userNameLength = AWS_IOT_METRICS_STRING_LENGTH;
            /* Password for authentication is not used. */
            xConnectInfo.pPassword = NULL;
            xConnectInfo.passwordLength = 0U;

            /* The maximum time interval in seconds which is allowed to elapse
             * between two Control Packets.
             * It is the responsibility of the Client to ensure that the interval between
             * Control Packets being sent does not exceed the this Keep Alive value. In the
             * absence of sending any other Control Packets, the Client MUST send a
             * PINGREQ Packet. */
            xConnectInfo.keepAliveSeconds = mqttexampleKEEP_ALIVE_TIMEOUT_SECONDS;

            /* Send MQTT CONNECT packet to broker. */
            eMqttStatus = MQTT_Connect( pxMqttContext,
                                        &xConnectInfo,
                                        NULL,
                                        mqttexampleCONNACK_RECV_TIMEOUT_MS,
                                        &sessionPresent );

            if( eMqttStatus != MQTTSuccess )
            {
                xReturnStatus = pdFAIL;
                LogError( ( "Connection with MQTT broker failed with status %s.",
                            MQTT_Status_strerror( eMqttStatus ) ) );
            }
            else
            {
                LogInfo( ( "MQTT connection successfully established with broker.\n\n" ) );
            }
        }

        if( xReturnStatus == pdPASS )
        {
            /* Keep a flag for indicating if MQTT session is established. This
             * flag will mark that an MQTT DISCONNECT has to be sent at the end
             * of the demo even if there are intermediate failures. */
            mqttSessionEstablished = true;
        }

        if( xReturnStatus == pdPASS )
        {
            /* Check if session is present and if there are any outgoing publishes
             * that need to resend. This is only valid if the broker is
             * re-establishing a session which was already present. */
            if( sessionPresent == true )
            {
                LogInfo( ( "An MQTT session with broker is re-established. "
                           "Resending unacked publishes." ) );

                /* Handle all the resend of publish messages. */
                xReturnStatus = handlePublishResend( pxMqttContext );
            }
            else
            {
                LogInfo( ( "A clean MQTT connection is established."
                           " Cleaning up all the stored outgoing publishes.\n\n" ) );

                /* Clean up the outgoing publishes waiting for ack as this new
                 * connection doesn't re-establish an existing session. */
                vCleanupOutgoingPublishes();
            }
        }
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

BaseType_t DisconnectMqttSession( MQTTContext_t * pxMqttContext,
                                  NetworkContext_t * pxNetworkContext )
{
    MQTTStatus_t eMqttStatus = MQTTSuccess;
    BaseType_t xReturnStatus = pdPASS;
    TransportSocketStatus_t xNetworkStatus;

    assert( pxMqttContext != NULL );
    assert( pxNetworkContext != NULL );

    if( mqttSessionEstablished == true )
    {
        /* Send DISCONNECT. */
        eMqttStatus = MQTT_Disconnect( pxMqttContext );

        if( eMqttStatus != MQTTSuccess )
        {
            LogError( ( "Sending MQTT DISCONNECT failed with status=%s.",
                        MQTT_Status_strerror( eMqttStatus ) ) );
            xReturnStatus = pdFAIL;
        }
    }

    /* Close the network connection.  */
    xNetworkStatus = SecureSocketsTransport_Disconnect( pxNetworkContext );

    if( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
    {
        LogError( ( "Disconnecting from SecureSocket failed with status=%u.",
                    xNetworkStatus ) );
        xReturnStatus = pdFAIL;
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

BaseType_t UnsubscribeFromTopic( MQTTContext_t * pxMqttContext,
                                 const char * pcTopicFilter,
                                 uint16_t usTopicFilterLength )
{
    BaseType_t xReturnStatus = pdPASS;
    MQTTStatus_t eMqttStatus;
    MQTTSubscribeInfo_t pSubscriptionList[ 1 ];

    assert( pxMqttContext != NULL );
    assert( pcTopicFilter != NULL );
    assert( usTopicFilterLength > 0 );

    /* Start with everything at 0. */
    ( void ) memset( ( void * ) pSubscriptionList, 0x00, sizeof( pSubscriptionList ) );

    /* This example subscribes to only one topic and uses QOS1. */
    pSubscriptionList[ 0 ].qos = MQTTQoS1;
    pSubscriptionList[ 0 ].pTopicFilter = pcTopicFilter;
    pSubscriptionList[ 0 ].topicFilterLength = usTopicFilterLength;

    /* Generate packet identifier for the UNSUBSCRIBE packet. */
    globalUnsubscribePacketIdentifier = MQTT_GetPacketId( pxMqttContext );

    /* Send UNSUBSCRIBE packet. */
    eMqttStatus = MQTT_Unsubscribe( pxMqttContext,
                                    pSubscriptionList,
                                    sizeof( pSubscriptionList ) / sizeof( MQTTSubscribeInfo_t ),
                                    globalUnsubscribePacketIdentifier );

    if( eMqttStatus != MQTTSuccess )
    {
        LogError( ( "Failed to send UNSUBSCRIBE packet to broker with error = %s.",
                    MQTT_Status_strerror( eMqttStatus ) ) );
        xReturnStatus = pdFAIL;
    }
    else
    {
        LogInfo( ( "UNSUBSCRIBE sent topic %.*s to broker.\n\n",
                   usTopicFilterLength,
                   pcTopicFilter ) );

        /* Process incoming packet from the broker. Acknowledgment for unsubscription
         * ( UNSUBACK ) will be received here. */
        xReturnStatus = prvWaitForPacket( pxMqttContext, MQTT_PACKET_TYPE_UNSUBACK );

        if( xReturnStatus == pdFAIL )
        {
            LogError( ( "UNSUBACK never arrived when unsubscribing to topic %s.",
                        pcTopicFilter ) );
        }
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

BaseType_t PublishToTopic( MQTTContext_t * pxMqttContext,
                           const char * pcTopicFilter,
                           int32_t topicFilterLength,
                           const char * pcPayload,
                           size_t payloadLength )
{
    BaseType_t xReturnStatus = pdPASS;
    MQTTStatus_t eMqttStatus = MQTTSuccess;
    uint8_t ucPublishIndex = MAX_OUTGOING_PUBLISHES;

    assert( pxMqttContext != NULL );
    assert( pcTopicFilter != NULL );
    assert( topicFilterLength > 0 );

    /* Get the next free index for the outgoing publish. All QoS1 outgoing
     * publishes are stored until a PUBACK is received. These messages are
     * stored for supporting a resend if a network connection is broken before
     * receiving a PUBACK. */
    xReturnStatus = prvGetNextFreeIndexForOutgoingPublishes( &ucPublishIndex );

    if( xReturnStatus == pdFAIL )
    {
        LogError( ( "Unable to find a free spot for outgoing PUBLISH message.\n\n" ) );
    }
    else
    {
        LogInfo( ( "the published payload:%.*s \r\n ", payloadLength, pcPayload ) );
        /* This example publishes to only one topic and uses QOS1. */
        outgoingPublishPackets[ ucPublishIndex ].pubInfo.qos = MQTTQoS1;
        outgoingPublishPackets[ ucPublishIndex ].pubInfo.pTopicName = pcTopicFilter;
        outgoingPublishPackets[ ucPublishIndex ].pubInfo.topicNameLength = topicFilterLength;
        outgoingPublishPackets[ ucPublishIndex ].pubInfo.pPayload = pcPayload;
        outgoingPublishPackets[ ucPublishIndex ].pubInfo.payloadLength = payloadLength;

        /* Get a new packet id. */
        outgoingPublishPackets[ ucPublishIndex ].packetId = MQTT_GetPacketId( pxMqttContext );

        /* Send PUBLISH packet. */
        eMqttStatus = MQTT_Publish( pxMqttContext,
                                    &outgoingPublishPackets[ ucPublishIndex ].pubInfo,
                                    outgoingPublishPackets[ ucPublishIndex ].packetId );

        if( eMqttStatus != MQTTSuccess )
        {
            LogError( ( "Failed to send PUBLISH packet to broker with error = %s.",
                        MQTT_Status_strerror( eMqttStatus ) ) );
            vCleanupOutgoingPublishAt( ucPublishIndex );
            xReturnStatus = pdFAIL;
        }
        else
        {
            LogInfo( ( "PUBLISH sent for topic %.*s to broker with packet ID %u.\n\n",
                       topicFilterLength,
                       pcTopicFilter,
                       outgoingPublishPackets[ ucPublishIndex ].packetId ) );

            /* Calling MQTT_ProcessLoop to process incoming publish echo, since
             * application subscribed to the same topic the broker will send
             * publish message back to the application. This function also
             * sends ping request to broker if MQTT_KEEP_ALIVE_INTERVAL_SECONDS
             * has expired since the last MQTT packet sent and receive
             * ping responses. */
            eMqttStatus = MQTT_ProcessLoop( pxMqttContext, mqttexamplePROCESS_LOOP_TIMEOUT_MS );

            if( eMqttStatus != MQTTSuccess )
            {
                LogWarn( ( "MQTT_ProcessLoop returned with status = %s.",
                           MQTT_Status_strerror( eMqttStatus ) ) );
            }
        }
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

BaseType_t ProcessLoop( MQTTContext_t * pxMqttContext,
                        uint32_t ulTimeoutMs )
{
    BaseType_t xReturnStatus = pdFAIL;
    MQTTStatus_t eMqttStatus = MQTTSuccess;

    eMqttStatus = MQTT_ProcessLoop( pxMqttContext, ulTimeoutMs );

    if( eMqttStatus != MQTTSuccess )
    {
        LogWarn( ( "MQTT_ProcessLoop returned with status = %s.",
                   MQTT_Status_strerror( eMqttStatus ) ) );
    }
    else
    {
        LogDebug( ( "MQTT_ProcessLoop successful." ) );
        xReturnStatus = pdPASS;
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

static uint32_t prvGetTimeMs( void )
{
    TickType_t xTickCount = 0;
    uint32_t ulTimeMs = 0UL;

    /* Get the current tick count. */
    xTickCount = xTaskGetTickCount();

    /* Convert the ticks to milliseconds. */
    ulTimeMs = ( uint32_t ) xTickCount * MILLISECONDS_PER_TICK;

    /* Reduce ulGlobalEntryTimeMs from obtained time so as to always return the
     * elapsed time in the application. */
    ulTimeMs = ( uint32_t ) ( ulTimeMs - ulGlobalEntryTimeMs );

    return ulTimeMs;
}

/*-----------------------------------------------------------*/
