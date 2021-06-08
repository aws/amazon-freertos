/*
 * FreeRTOS V202012.00
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
 */

/**
 * @file core_mqtt_system_test.c
 * @brief Integration tests for the coreMQTT library for the FreeRTOS platform
 * over Secure Sockets as the transport layer.
 */

/* Standard header includes. */
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

/* Unity Test framework include. */
#include "unity_fixture.h"

/* Include connection configurations header. */
#include "aws_clientcredential.h"

/* Include header for root CA certificates. */
#include "iot_default_root_certificates.h"

/* Include task.h for delay function. */
#include "FreeRTOS.h"
#include "task.h"

/* Include PKCS11 headers. */
#include "core_pkcs11.h"
#include "pkcs11.h"

/* Include header for retrying network operations, like connection, with
 * exponential backoff and jitter.*/
#include "backoff_algorithm.h"

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Include logging header files and define logging macros in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define the LIBRARY_LOG_NAME and LIBRARY_LOG_LEVEL macros depending on
 * the logging configuration for the integration test.
 * 3. Include the header file "logging_stack.h", if logging is enabled for the
 * integration test.
 */

#include "logging_levels.h"

/* Logging configuration for the integration test. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    "TEST"
#endif

#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif
#include "logging_stack.h"

/* The logging configuration macros are defined above to ensure they are not
 * superceded by definitions in the following header files. */

/* Include the secure sockets implementation of the transport interface. */
#include "transport_secure_sockets.h"

/* Include paths for public enums, structures, and macros. These are included */
#include "core_mqtt.h"
#include "core_mqtt_state.h"

/**************Default Configurations values***********************/

/**
 * This file contains 2 test groups:
 * - coreMQTT_Integration_AWS_IoT_Compatible for running tests against AWS IoT
 * - coreMQTT_Integration for running tests against a non-AWS IoT MQTT 3.1.1 MQTT broker.
 *
 * Thus, the 2 test groups have different broker endpoint and TLS credentials
 * for running to connect against their brokers. However, this test shares the same
 * macros (BROKER_ENDPOINT, BROKER_PORT, SERVER_ROOT_CA_CERT, and macros in
 * tests/include/aws_clientcredential_key.h file) for the test groups.
 * Thus, ONLY ONE test group (either against AWS IoT or a different broker) can be run
 * for a single build depending on the credentials provided.
 */

#ifndef BROKER_ENDPOINT
    #define BROKER_ENDPOINT    clientcredentialMQTT_BROKER_ENDPOINT
#endif

#ifndef BROKER_PORT
    #define BROKER_PORT    clientcredentialMQTT_BROKER_PORT
#endif

/* Use Starfield Root CA as the default Root CA because the TI C3220 Launchpad board
 * requires that the Root CA certificate have its certificate self-signed. The Amazon Root CAs
 * are cross-signed by Starfield Root CA.*/
#ifndef SERVER_ROOT_CA_CERT
    #define SERVER_ROOT_CA_CERT    tlsSTARFIELD_ROOT_CERTIFICATE_PEM
#endif /* ifndef SERVER_ROOT_CA_CERT_NON_AWS */

/**
 * @brief Client identifier for MQTT session in the tests.
 */
#ifndef TEST_CLIENT_IDENTIFIER
    #define TEST_CLIENT_IDENTIFIER    clientcredentialIOT_THING_NAME
#endif

/**********************End Configurations********************************/

/**
 * @brief A valid starting packet ID per MQTT spec. Start from 1.
 */
#define MQTT_FIRST_VALID_PACKET_ID                 ( 1 )

/**
 * @brief A PINGREQ packet is always 2 bytes in size, defined by MQTT 3.1.1 spec.
 */
#define MQTT_PACKET_PINGREQ_SIZE                   ( 2U )

/**
 * @brief A packet type not handled by MQTT_ProcessLoop.
 */
#define MQTT_PACKET_TYPE_INVALID                   ( 0U )

/**
 * @brief Number of milliseconds in a second.
 */
#define MQTT_ONE_SECOND_TO_MS                      ( 1000U )

/**
 * @brief Length of the MQTT network buffer.
 */
#define MQTT_TEST_BUFFER_LENGTH                    ( 128 )

/**
 * @brief Sample length of remaining serialized data.
 */
#define MQTT_SAMPLE_REMAINING_LENGTH               ( 64 )

/**
 * @brief Subtract this value from max value of global entry time
 * for the timer overflow test.
 */
#define MQTT_OVERFLOW_OFFSET                       ( 3 )

/**
 * @brief Sample topic filter to subscribe to.
 */
#define TEST_MQTT_TOPIC                            TEST_CLIENT_IDENTIFIER "/iot/integration/test"

/**
 * @brief Sample topic filter 2 to use in tests.
 */
#define TEST_MQTT_TOPIC_2                          TEST_CLIENT_IDENTIFIER "/iot/integration/test2"

/**
 * @brief Length of sample topic filter.
 */
#define TEST_MQTT_TOPIC_LENGTH                     ( sizeof( TEST_MQTT_TOPIC ) - 1 )

/**
 * @brief Sample topic filter to subscribe to.
 */
#define TEST_MQTT_LWT_TOPIC                        TEST_CLIENT_IDENTIFIER "/iot/integration/test/lwt"

/**
 * @brief Length of sample topic filter.
 */
#define TEST_MQTT_LWT_TOPIC_LENGTH                 ( sizeof( TEST_MQTT_LWT_TOPIC ) - 1 )

/**
 * @brief Size of the network buffer for MQTT packets.
 */
#define NETWORK_BUFFER_SIZE                        ( 1024U )

/**
 * @brief Length of the client identifier.
 */
#define TEST_CLIENT_IDENTIFIER_LENGTH              ( sizeof( TEST_CLIENT_IDENTIFIER ) - 1u )

/**
 * @brief Client identifier for use in LWT tests.
 */
#define TEST_CLIENT_IDENTIFIER_LWT                 TEST_CLIENT_IDENTIFIER "-LWT"

/**
 * @brief Length of LWT client identifier.
 */
#define TEST_CLIENT_IDENTIFIER_LWT_LENGTH          ( sizeof( TEST_CLIENT_IDENTIFIER_LWT ) - 1u )

/**
 * @brief The maximum number of retries for network operation with server.
 */
#define RETRY_MAX_ATTEMPTS                         ( 5U )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying failed network
 * operations with server.
 */
#define RETRY_MAX_BACKOFF_DELAY_MS                 ( 5000U )

/**
 * @brief The base back-off delay (in milliseconds) to use for network operation retry
 * attempts.
 */
#define RETRY_BACKOFF_BASE_MS                      ( 500U )

/**
 * @brief Transport timeout in milliseconds for transport send and receive.
 */
#define TRANSPORT_SEND_RECV_TIMEOUT_MS             ( 200U )

/**
 * @brief Timeout for receiving CONNACK packet in milli seconds.
 */
#define CONNACK_RECV_TIMEOUT_MS                    ( 20000U )

/**
 * @brief Time interval in seconds at which an MQTT PINGREQ need to be sent to
 * broker.
 */
#define MQTT_KEEP_ALIVE_INTERVAL_SECONDS           ( 60U )

/**
 * @brief The number of milliseconds to wait for AWS IoT Core Message Broker
 * to resend a PUBLISH that has not been acknowledged.
 */
#define AWS_IOT_CORE_REPUBLISH_INTERVAL_MS         ( 30000U )

/**
 * @brief Timeout for MQTT_ProcessLoop() function in milliseconds.
 */
#define MQTT_PROCESS_LOOP_TIMEOUT_MS               ( 700U )

/**
 * @brief The maximum number of times to call MQTT_ProcessLoop() when polling
 * for a specific packet from the broker.
 */
#define MQTT_PROCESS_LOOP_PACKET_WAIT_COUNT_MAX    ( 30U )

/**
 * @brief The MQTT message published in this example.
 */
#define MQTT_EXAMPLE_MESSAGE                       "Hello World!"

/**
 * @brief Milliseconds per second.
 */
#define MILLISECONDS_PER_SECOND                    ( 1000U )

/**
 * @brief Milliseconds per FreeRTOS tick.
 */
#define MILLISECONDS_PER_TICK                      ( MILLISECONDS_PER_SECOND / configTICK_RATE_HZ )

/**
 * @brief Helper function-like macro to test for a specified incoming packet
 * from the broker. This helper macro allows us to wait for the specified
 * incoming packet without unnecessarily blocking on MQTT_ProcessLoop() for
 * beyond its arrival.
 *
 * By using a macro instead of a function, the line number in the test case will
 * be printed in a failure.
 *
 * @param[in] flag Packet flag to test for true.
 * @param[in] expectedStatus Status code expected to be returned from MQTT_Process
 */
#define WAIT_FOR_PACKET( flag, expectedStatus )                                         \
    do {                                                                                \
        uint8_t count = 0;                                                              \
        MQTTStatus_t currentStatus = MQTTSuccess;                                       \
                                                                                        \
        while( ( flag == false ) &&                                                     \
               ( count++ < MQTT_PROCESS_LOOP_PACKET_WAIT_COUNT_MAX ) &&                 \
               ( currentStatus == MQTTSuccess ) )                                       \
        {                                                                               \
            currentStatus = MQTT_ProcessLoop( &context, MQTT_PROCESS_LOOP_TIMEOUT_MS ); \
        }                                                                               \
                                                                                        \
        TEST_ASSERT_EQUAL( expectedStatus, currentStatus );                             \
        TEST_ASSERT_TRUE( flag );                                                       \
    } while( 0 )

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
 * @brief Packet Identifier generated when Publish request was sent to the broker;
 * it is used to match acknowledgement responses to the transmitted publish
 * request.
 */
static uint16_t globalPublishPacketIdentifier = 0U;

/**
 * @brief Represents the secure sockets network context used for the TLS session
 * with the broker for tests.
 */
static NetworkContext_t networkContext;

/**
 * @brief The parameters for the network context using a TLS channel.
 */
static SecureSocketsTransportParams_t secureSocketsTransportParams = { 0 };

/**
 * @brief Represents the hostname and port of the broker.
 */
static ServerInfo_t serverInfo;

/**
 * @brief TLS credentials needed to connect to the broker.
 */
static SocketsConfig_t socketsConfig;

/**
 * @brief The context representing the MQTT connection with the broker for
 * the test case.
 */
static MQTTContext_t context;

/**
 * @brief Flag that represents whether a persistent session was resumed
 * with the broker for the test.
 */
static bool persistentSession = false;

/**
 * @brief Flag to indicate if LWT is being used when establishing a connection.
 */
static bool useLWTClientIdentifier = false;

/**
 * @brief Flag to represent whether a SUBACK is received from the broker.
 */
static bool receivedSubAck = false;

/**
 * @brief Flag to represent whether an UNSUBACK is received from the broker.
 */
static bool receivedUnsubAck = false;

/**
 * @brief Flag to represent whether a PUBACK is received from the broker.
 */
static bool receivedPubAck = false;

/**
 * @brief Flag to represent whether a PUBREC is received from the broker.
 */
static bool receivedPubRec = false;

/**
 * @brief Flag to represent whether a PUBREL is received from the broker.
 */
static bool receivedPubRel = false;

/**
 * @brief Flag to represent whether a PUBCOMP is received from the broker.
 */
static bool receivedPubComp = false;

/**
 * @brief Flag to represent whether an incoming PUBLISH packet is received
 * with the "retain" flag set.
 */
static bool receivedRetainedMessage = false;

/**
 * @brief Flag to represent whether the tests are being run against AWS IoT
 * Core.
 */
static bool testingAgainstAWS = false;

/**
 * @brief Represents incoming PUBLISH information.
 */
static MQTTPublishInfo_t incomingInfo;

/**
 * @brief Flag to represent whether a PUBLISH is received from the broker.
 */
static bool receivedPublish = false;

/**
 * @brief Buffer to store incoming publish topic name for assertions in tests.
 */
static uint8_t incomingTopicBuffer[ TEST_MQTT_LWT_TOPIC_LENGTH ];

/**
 * @brief Buffer to store incoming PUBLUSH payload for assertions in tests.
 */
static uint8_t incomingPayloadBuffer[ sizeof( MQTT_EXAMPLE_MESSAGE ) ];

/**
 * @brief Disconnect when receiving this packet type. Used for session
 * restoration tests.
 */
static uint8_t packetTypeForDisconnection = MQTT_PACKET_TYPE_INVALID;

/**
 * @brief Flag to represent whether the packetTypeForDisconnection is received
 * from the broker.
 */
static bool receivedPacketTypeForDisconnection = false;

/**
 * @brief Global entry time to use as a reference timestamp in the getTimeMs()
 * function. getTimeMs() function will always return the difference between the
 * current time and the global entry time. This reduces the chances of overflow
 * for the 32 bit unsigned integer used for holding the timestamp.
 */
static uint32_t globalEntryTime;

/**
 * @brief The timer query function provided to the MQTT context.
 *
 * @return Time in milliseconds.
 */
static uint32_t getTimeMs();

/**
 * @brief Sends an MQTT CONNECT packet over the already connected TCP socket.
 *
 * @param[in] pContext MQTT context pointer.
 * @param[in] pNetworkContext Network context for secure sockets transport implementation.
 * @param[in] createCleanSession Creates a new MQTT session if true.
 * If false, tries to establish the existing session if there was session
 * already present in broker.
 * @param[out] pSessionPresent Session was already present in the broker or not.
 * Session present response is obtained from the CONNACK from broker.
 */
static void establishMqttSession( MQTTContext_t * pContext,
                                  NetworkContext_t * pNetworkContext,
                                  bool createCleanSession,
                                  bool * pSessionPresent );

/**
 * @brief Handler for incoming acknowledgement packets from the broker.
 *
 * @param[in] pPacketInfo Info for the incoming acknowledgement packet.
 * @param[in] packetIdentifier The ID of the incoming packet.
 */
static void handleAckEvents( MQTTPacketInfo_t * pPacketInfo,
                             uint16_t packetIdentifier );

/**
 * @brief The application callback function that is expected to be invoked by the
 * MQTT library for incoming publish and incoming acks received over the network.
 *
 * @param[in] pContext MQTT context pointer.
 * @param[in] pPacketInfo Packet Info pointer for the incoming packet.
 * @param[in] pDeserializedInfo Deserialized information from the incoming packet.
 */
static void eventCallback( MQTTContext_t * pContext,
                           MQTTPacketInfo_t * pPacketInfo,
                           MQTTDeserializedInfo_t * pDeserializedInfo );

/**
 * @brief Helper function to subscribe to the given MQTT topic.
 *
 * @param[in] pContext MQTT context pointer.
 * @param[in] pTopic MQTT topic to subscribe to.
 * @param[in] qos MQTT subscribe packet quality of service.
 *
 * @return MQTTSuccess if successful, error code otherwise. See #MQTT_Subscribe
 * for possible return codes.
 */
static MQTTStatus_t subscribeToTopic( MQTTContext_t * pContext,
                                      const char * pTopic,
                                      MQTTQoS_t qos );

/**
 * @brief Helper function to unsubscribe from the given MQTT topic.
 *
 * @param[in] pContext MQTT context pointer.
 * @param[in] pTopic MQTT topic to unsubscribe from.
 *
 * @return MQTTSuccess if successful, error code otherwise. See #MQTT_Unsubscribe
 * for possible return codes.
 */
static MQTTStatus_t unsubscribeFromTopic( MQTTContext_t * pContext,
                                          const char * pTopic );

/**
 * @brief Helper function to publish to the given MQTT topic.
 *
 * @param[in] pContext MQTT context pointer.
 * @param[in] pTopic MQTT topic to publish to.
 * @param[in] pPayload Message payload.
 * @param[in] setRetainFlag If this is true, then the retrain flag will be set.
 * @param[in] isDuplicate If this is true, then the duplicate flag will be set.
 * @param[in] qos The PUBLISH packet quality of service.
 * @param[in] packetId A packet ID generated by #MQTT_GetPacketId.
 *
 * @return MQTTSuccess if successful, error code otherwise. See #MQTT_Publish
 * for possible return codes.
 */
static MQTTStatus_t publishToTopic( MQTTContext_t * pContext,
                                    const char * pTopic,
                                    const char * pPayload,
                                    bool setRetainFlag,
                                    bool isDuplicate,
                                    MQTTQoS_t qos,
                                    uint16_t packetId );

/**
 * @brief Helper function to clear any possible retained message, on the broker,
 * for the given MQTT topic.
 *
 * @param[in] pContext MQTT context pointer.
 * @param[in] pTopic MQTT topic to clear a retained message from.
 */
static void clearRetainedMessage( MQTTContext_t * pContext,
                                  const char * pTopic );

/**
 * @brief Implementation of TransportSend_t interface that terminates the TLS
 * and TCP connection with the broker and returns failure.
 *
 * @param[in] pNetworkContext The context associated with the network connection
 * to be disconnected.
 * @param[in] pBuffer This parameter is ignored.
 * @param[in] bytesToRecv This parameter is ignored.
 *
 * @return -1 to represent failure.
 */
static int32_t failedRecv( NetworkContext_t * pNetworkContext,
                           void * pBuffer,
                           size_t bytesToRecv );

/**
 * @brief The pseudo random number generator to use for exponential backoff with
 * jitter calculation for connection retries.
 * This function is an implementation the #BackoffAlgorithm_RNG_t interface type
 * of the backoff algorithm library API.
 *
 * The PKCS11 module is used to generate the random random number as it allows
 * access to a True Random Number Generator (TRNG) if the vendor platform supports it.
 *
 * @return The generated random number. This function ALWAYS succeeds
 * in generating a random number.
 */
static uint32_t generateRandomNumber();

/**
 * @brief Connect to the MQTT broker with reconnection retries.
 *
 * If connection fails, retry is attempted after a timeout.
 * The timeout value will exponentially increased until the maximum
 * timeout value is reached or the number of attempts are exhausted.
 *
 * @param[out] pNetworkContext The created network context.
 *
 * @return false on failure; true on successful connection.
 */
static bool connectToServerWithBackoffRetries( NetworkContext_t * pNetworkContext );

/**
 * @brief Helper function to start a new persistent session.
 * It terminates the existing "clean session", and creates a new connection
 * with the "clean session" flag set to 0 to create a persistent session
 * with the broker.
 */
static void startPersistentSession();

/**
 * @brief Helper function to resume connection in persistent session
 * with the broker.
 * It resumes the session with the broker by establishing a new connection
 * with the "clean session" flag set to 0.
 */
static void resumePersistentSession();

/*-----------------------------------------------------------*/

static uint32_t getTimeMs()
{
    TickType_t tickCount = 0;
    uint32_t timeMs = 0UL;

    /* Get the current tick count. */
    tickCount = xTaskGetTickCount();

    /* Convert the ticks to milliseconds. */
    timeMs = ( uint32_t ) tickCount * MILLISECONDS_PER_TICK;

    /* Reduce globalEntryTime from obtained time so as to always return the
     * elapsed time in the application. */
    timeMs = ( uint32_t ) ( timeMs - globalEntryTime );

    return timeMs;
}

static void establishMqttSession( MQTTContext_t * pContext,
                                  NetworkContext_t * pNetworkContext,
                                  bool createCleanSession,
                                  bool * pSessionPresent )
{
    MQTTConnectInfo_t connectInfo;
    TransportInterface_t transport;
    MQTTFixedBuffer_t networkBuffer;
    MQTTPublishInfo_t lwtInfo;

    assert( pContext != NULL );
    assert( pNetworkContext != NULL );

    /* The network buffer must remain valid for the lifetime of the MQTT context. */
    static uint8_t buffer[ NETWORK_BUFFER_SIZE ];

    /* Setup the transport interface object for the library. */
    transport.pNetworkContext = pNetworkContext;
    transport.send = SecureSocketsTransport_Send;
    transport.recv = SecureSocketsTransport_Recv;

    /* Fill the values for network buffer. */
    networkBuffer.pBuffer = buffer;
    networkBuffer.size = NETWORK_BUFFER_SIZE;

    /* Clear the state of the MQTT context when creating a clean session. */
    if( createCleanSession == true )
    {
        /* Initialize MQTT library. */
        TEST_ASSERT_EQUAL( MQTTSuccess, MQTT_Init( pContext,
                                                   &transport,
                                                   getTimeMs,
                                                   eventCallback,
                                                   &networkBuffer ) );
    }

    /* Establish MQTT session with a CONNECT packet. */

    connectInfo.cleanSession = createCleanSession;

    if( useLWTClientIdentifier )
    {
        connectInfo.clientIdentifierLength = TEST_CLIENT_IDENTIFIER_LWT_LENGTH;
        connectInfo.pClientIdentifier = TEST_CLIENT_IDENTIFIER_LWT;
    }
    else
    {
        connectInfo.clientIdentifierLength = TEST_CLIENT_IDENTIFIER_LENGTH;
        connectInfo.pClientIdentifier = TEST_CLIENT_IDENTIFIER;
    }

    LogDebug( ( "Client ID for MQTT for MQTT connection={%.*s}",
                connectInfo.clientIdentifierLength,
                connectInfo.pClientIdentifier ) );

    /* The interval at which an MQTT PINGREQ needs to be sent out to broker. */
    connectInfo.keepAliveSeconds = MQTT_KEEP_ALIVE_INTERVAL_SECONDS;

    /* Username and password for authentication. Not used in this test. */
    connectInfo.pUserName = NULL;
    connectInfo.userNameLength = 0U;
    connectInfo.pPassword = NULL;
    connectInfo.passwordLength = 0U;

    /* LWT Info. */
    lwtInfo.pTopicName = TEST_MQTT_LWT_TOPIC;
    lwtInfo.topicNameLength = TEST_MQTT_LWT_TOPIC_LENGTH;
    lwtInfo.pPayload = MQTT_EXAMPLE_MESSAGE;
    lwtInfo.payloadLength = strlen( MQTT_EXAMPLE_MESSAGE );
    lwtInfo.qos = MQTTQoS0;
    lwtInfo.dup = false;
    lwtInfo.retain = false;

    /* Send MQTT CONNECT packet to broker. */
    TEST_ASSERT_EQUAL( MQTTSuccess, MQTT_Connect( pContext,
                                                  &connectInfo,
                                                  &lwtInfo,
                                                  CONNACK_RECV_TIMEOUT_MS,
                                                  pSessionPresent ) );
}

static void handleAckEvents( MQTTPacketInfo_t * pPacketInfo,
                             uint16_t packetIdentifier )
{
    /* Handle other packets. */
    switch( pPacketInfo->type )
    {
        case MQTT_PACKET_TYPE_SUBACK:
            /* Set the flag to represent reception of SUBACK. */
            receivedSubAck = true;

            LogDebug( ( "Received SUBACK: PacketID=%u",
                        packetIdentifier ) );
            /* Make sure ACK packet identifier matches with Request packet identifier. */
            TEST_ASSERT_EQUAL( globalSubscribePacketIdentifier, packetIdentifier );
            break;

        case MQTT_PACKET_TYPE_PINGRESP:

            /* Nothing to be done from application as library handles
             * PINGRESP. */
            LogDebug( ( "Received PINGRESP" ) );
            break;

        case MQTT_PACKET_TYPE_UNSUBACK:
            /* Set the flag to represent reception of UNSUBACK. */
            receivedUnsubAck = true;

            LogDebug( ( "Received UNSUBACK: PacketID=%u",
                        packetIdentifier ) );
            /* Make sure ACK packet identifier matches with Request packet identifier. */
            TEST_ASSERT_EQUAL( globalUnsubscribePacketIdentifier, packetIdentifier );
            break;

        case MQTT_PACKET_TYPE_PUBACK:
            /* Set the flag to represent reception of PUBACK. */
            receivedPubAck = true;

            /* Make sure ACK packet identifier matches with Request packet identifier. */
            TEST_ASSERT_EQUAL( globalPublishPacketIdentifier, packetIdentifier );

            LogDebug( ( "Received PUBACK: PacketID=%u",
                        packetIdentifier ) );
            break;

        case MQTT_PACKET_TYPE_PUBREC:
            /* Set the flag to represent reception of PUBREC. */
            receivedPubRec = true;

            /* Make sure ACK packet identifier matches with Request packet identifier. */
            TEST_ASSERT_EQUAL( globalPublishPacketIdentifier, packetIdentifier );

            LogDebug( ( "Received PUBREC: PacketID=%u",
                        packetIdentifier ) );
            break;

        case MQTT_PACKET_TYPE_PUBREL:
            /* Set the flag to represent reception of PUBREL. */
            receivedPubRel = true;

            /* Nothing to be done from application as library handles
             * PUBREL. */
            LogDebug( ( "Received PUBREL: PacketID=%u",
                        packetIdentifier ) );
            break;

        case MQTT_PACKET_TYPE_PUBCOMP:
            /* Set the flag to represent reception of PUBACK. */
            receivedPubComp = true;

            /* Make sure ACK packet identifier matches with Request packet identifier. */
            TEST_ASSERT_EQUAL( globalPublishPacketIdentifier, packetIdentifier );

            /* Nothing to be done from application as library handles
             * PUBCOMP. */
            LogDebug( ( "Received PUBCOMP: PacketID=%u",
                        packetIdentifier ) );
            break;

        /* Any other packet type is invalid. */
        default:
            LogError( ( "Unknown packet type received:(%02x).",
                        pPacketInfo->type ) );
    }
}

static void eventCallback( MQTTContext_t * pContext,
                           MQTTPacketInfo_t * pPacketInfo,
                           MQTTDeserializedInfo_t * pDeserializedInfo )
{
    MQTTPublishInfo_t * pPublishInfo = NULL;

    assert( pContext != NULL );
    assert( pPacketInfo != NULL );
    assert( pDeserializedInfo != NULL );

    /* Suppress unused parameter warning when asserts are disabled in build. */
    ( void ) pContext;

    TEST_ASSERT_EQUAL( MQTTSuccess, pDeserializedInfo->deserializationResult );
    pPublishInfo = pDeserializedInfo->pPublishInfo;

    if( ( pPacketInfo->type == packetTypeForDisconnection ) ||
        ( ( pPacketInfo->type & 0xF0U ) == packetTypeForDisconnection ) )
    {
        /* Set the flag to represent reception of the disconnection packet type. */
        receivedPacketTypeForDisconnection = true;
        LogDebug( ( "Received packet type %u, disconnecting...", pPacketInfo->type ) );

        /* Terminate TLS session and TCP connection to test session restoration
         * across network connection. */
        ( void ) SecureSocketsTransport_Disconnect( &networkContext );
    }
    else
    {
        /* Handle incoming publish. The lower 4 bits of the publish packet
         * type is used for the dup, QoS, and retain flags. Hence masking
         * out the lower bits to check if the packet is publish. */
        if( ( pPacketInfo->type & 0xF0U ) == MQTT_PACKET_TYPE_PUBLISH )
        {
            assert( pPublishInfo != NULL );
            /* Handle incoming publish. */

            /* Set the flag to represent reception of a PUBLISH packet. */
            receivedPublish = true;
            LogDebug( ( "Received PUBLISH: PacketID=%u", pDeserializedInfo->packetIdentifier ) );
            LogDebug( ( "PUBLISH topic: %.*s", pPublishInfo->topicNameLength, pPublishInfo->pTopicName ) );
            LogDebug( ( "PUBLISH payload: %.*s", pPublishInfo->payloadLength, pPublishInfo->pPayload ) );
            LogDebug( ( "PUBLISH retain flag: %d", pPublishInfo->retain ) );

            /* Cache information about the incoming PUBLISH message to process
             * in test case. */
            memcpy( &incomingInfo, pPublishInfo, sizeof( MQTTPublishInfo_t ) );
            incomingInfo.pTopicName = NULL;
            incomingInfo.pPayload = NULL;
            /* Copy information of topic name and payload. */
            memcpy( ( void * ) incomingTopicBuffer, pPublishInfo->pTopicName, pPublishInfo->topicNameLength );
            incomingInfo.pTopicName = incomingTopicBuffer;
            memcpy( ( void * ) incomingPayloadBuffer, pPublishInfo->pPayload, pPublishInfo->payloadLength );
            incomingInfo.pPayload = incomingPayloadBuffer;

            /* Update the global variable if the incoming PUBLISH packet
             * represents a retained message. */
            receivedRetainedMessage = pPublishInfo->retain;
        }
        else
        {
            handleAckEvents( pPacketInfo, pDeserializedInfo->packetIdentifier );
        }
    }
}

static MQTTStatus_t subscribeToTopic( MQTTContext_t * pContext,
                                      const char * pTopic,
                                      MQTTQoS_t qos )
{
    MQTTSubscribeInfo_t pSubscriptionList[ 1 ];

    assert( pContext != NULL );
    assert( pTopic != NULL );

    /* Start with everything at 0. */
    ( void ) memset( ( void * ) pSubscriptionList, 0x00, sizeof( pSubscriptionList ) );

    pSubscriptionList[ 0 ].qos = qos;
    pSubscriptionList[ 0 ].pTopicFilter = pTopic;
    pSubscriptionList[ 0 ].topicFilterLength = strlen( pTopic );

    /* Generate packet identifier for the SUBSCRIBE packet. */
    globalSubscribePacketIdentifier = MQTT_GetPacketId( pContext );

    /* Send SUBSCRIBE packet. */
    return MQTT_Subscribe( pContext,
                           pSubscriptionList,
                           sizeof( pSubscriptionList ) / sizeof( MQTTSubscribeInfo_t ),
                           globalSubscribePacketIdentifier );
}

static MQTTStatus_t unsubscribeFromTopic( MQTTContext_t * pContext,
                                          const char * pTopic )
{
    MQTTSubscribeInfo_t pSubscriptionList[ 1 ];

    assert( pContext != NULL );
    assert( pTopic != NULL );

    /* Start with everything at 0. */
    ( void ) memset( ( void * ) pSubscriptionList, 0x00, sizeof( pSubscriptionList ) );

    pSubscriptionList[ 0 ].pTopicFilter = pTopic;
    pSubscriptionList[ 0 ].topicFilterLength = strlen( pTopic );

    /* Generate packet identifier for the UNSUBSCRIBE packet. */
    globalUnsubscribePacketIdentifier = MQTT_GetPacketId( pContext );

    /* Send UNSUBSCRIBE packet. */
    return MQTT_Unsubscribe( pContext,
                             pSubscriptionList,
                             sizeof( pSubscriptionList ) / sizeof( MQTTSubscribeInfo_t ),
                             globalUnsubscribePacketIdentifier );
}

static MQTTStatus_t publishToTopic( MQTTContext_t * pContext,
                                    const char * pTopic,
                                    const char * pPayload,
                                    bool setRetainFlag,
                                    bool isDuplicate,
                                    MQTTQoS_t qos,
                                    uint16_t packetId )
{
    MQTTPublishInfo_t publishInfo;

    assert( pContext != NULL );
    assert( pTopic != NULL );

    publishInfo.retain = setRetainFlag;

    publishInfo.qos = qos;
    publishInfo.dup = isDuplicate;
    publishInfo.pTopicName = pTopic;
    publishInfo.topicNameLength = strlen( pTopic );
    publishInfo.pPayload = pPayload;

    if( pPayload != NULL )
    {
        publishInfo.payloadLength = strlen( pPayload );
    }
    else
    {
        publishInfo.payloadLength = 0;
    }

    /* Get a new packet id. */
    globalPublishPacketIdentifier = packetId;

    /* Send PUBLISH packet. */
    return MQTT_Publish( pContext,
                         &publishInfo,
                         packetId );
}

static void clearRetainedMessage( MQTTContext_t * pContext,
                                  const char * pTopic )
{
    assert( pContext != NULL );
    assert( pTopic != NULL );

    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           pContext,
                           pTopic,
                           NULL,
                           true,
                           false,
                           MQTTQoS1,
                           MQTT_GetPacketId( &context ) ) );

    /* Ensure the broker received the message by waiting for a PUBACK. */
    WAIT_FOR_PACKET( receivedPubAck, MQTTSuccess );
    /* Clear the PUBACK received for the publish. */
    receivedPubAck = false;
}

static int32_t failedRecv( NetworkContext_t * pNetworkContext,
                           void * pBuffer,
                           size_t bytesToRecv )
{
    ( void ) pBuffer;
    ( void ) bytesToRecv;

    /* Terminate the TLS+TCP connection with the broker for the test. */
    ( void ) SecureSocketsTransport_Disconnect( pNetworkContext );

    return -1;
}

static uint32_t generateRandomNumber()
{
    UBaseType_t uxRandNum = 0;

    CK_FUNCTION_LIST_PTR pFunctionList = NULL;
    CK_SESSION_HANDLE session = CK_INVALID_HANDLE;

    /* Get list of functions supported by the PKCS11 port. */
    TEST_ASSERT_EQUAL( CKR_OK, C_GetFunctionList( &pFunctionList ) );
    TEST_ASSERT_TRUE( pFunctionList != NULL );

    /* Initialize PKCS11 module and create a new session. */
    TEST_ASSERT_EQUAL( CKR_OK, xInitializePkcs11Session( &session ) );
    TEST_ASSERT_TRUE( session != CK_INVALID_HANDLE );

    /*
     * Seed random number generator with PKCS11.
     * Use of PKCS11 can allow use of True Random Number Generator (TRNG)
     * if the platform supports it.
     */
    TEST_ASSERT_EQUAL( CKR_OK, pFunctionList->C_GenerateRandom( session,
                                                                ( unsigned char * ) &uxRandNum,
                                                                sizeof( uxRandNum ) ) );


    /* Close PKCS11 session. */
    TEST_ASSERT_EQUAL( CKR_OK, pFunctionList->C_CloseSession( session ) );

    return uxRandNum;
}

static bool connectToServerWithBackoffRetries( NetworkContext_t * pNetworkContext )
{
    bool isSuccessful = false;
    BackoffAlgorithmStatus_t BackoffAlgStatus = BackoffAlgorithmSuccess;
    BackoffAlgorithmContext_t reconnectParams;
    uint16_t nextRetryBackOff = 0U;
    TransportSocketStatus_t transportStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;

    /* Initializer server information. */
    serverInfo.pHostName = BROKER_ENDPOINT;
    serverInfo.hostNameLength = strlen( BROKER_ENDPOINT );
    serverInfo.port = BROKER_PORT;

    /* Initialize SocketsConfig. */
    socketsConfig.enableTls = true;
    socketsConfig.pAlpnProtos = NULL;
    socketsConfig.maxFragmentLength = 0;
    socketsConfig.disableSni = true;
    socketsConfig.pRootCa = SERVER_ROOT_CA_CERT;
    socketsConfig.rootCaSize = strlen( SERVER_ROOT_CA_CERT ) + 1U;
    socketsConfig.sendTimeoutMs = TRANSPORT_SEND_RECV_TIMEOUT_MS;
    socketsConfig.recvTimeoutMs = TRANSPORT_SEND_RECV_TIMEOUT_MS;

    /* Initialize reconnect attempts and interval. */
    BackoffAlgorithm_InitializeParams( &reconnectParams,
                                       RETRY_BACKOFF_BASE_MS,
                                       RETRY_MAX_BACKOFF_DELAY_MS,
                                       RETRY_MAX_ATTEMPTS );

    /* Attempt to connect to MQTT broker. If connection fails, retry after
     * a timeout. Timeout value will exponentially increase till maximum
     * attempts are reached.
     */
    do
    {
        /* Establish a TCP connection with the server endpoint, then
         * establish TLS session on top of TCP connection. */
        transportStatus = SecureSocketsTransport_Connect( pNetworkContext,
                                                          &serverInfo,
                                                          &socketsConfig );

        if( transportStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
        {
            /* Generate random number and get back-off value for the next connection retry. */
            BackoffAlgStatus = BackoffAlgorithm_GetNextBackoff( &reconnectParams,
                                                                generateRandomNumber(),
                                                                &nextRetryBackOff );

            if( BackoffAlgStatus == BackoffAlgorithmRetriesExhausted )
            {
                LogError( ( "Connection to the broker failed, all attempts exhausted." ) );
            }

            else if( BackoffAlgStatus == BackoffAlgorithmSuccess )
            {
                LogWarn( ( "Connection to the broker failed. Retrying connection after backoff delay." ) );
                vTaskDelay( pdMS_TO_TICKS( nextRetryBackOff ) );
            }
        }
        else
        {
            isSuccessful = true;
        }
    } while( ( transportStatus != TRANSPORT_SOCKET_STATUS_SUCCESS ) && ( BackoffAlgStatus == BackoffAlgorithmSuccess ) );

    return isSuccessful;
}

static void startPersistentSession()
{
    /* Terminate TLS session and TCP network connection to discard the current MQTT session
     * that was created as a "clean session". */
    ( void ) SecureSocketsTransport_Disconnect( &networkContext );

    /* Establish a new MQTT connection over TLS with the broker with the "clean session" flag set to 0
     * to start a persistent session with the broker. */

    /* Create the TLS+TCP connection with the broker. */
    TEST_ASSERT_TRUE( connectToServerWithBackoffRetries( &networkContext ) );

    /* Establish a new MQTT connection for a persistent session with the broker. */
    establishMqttSession( &context, &networkContext, false, &persistentSession );
    TEST_ASSERT_FALSE( persistentSession );
}

static void resumePersistentSession()
{
    /* Create a new TLS+TCP network connection with the server. */
    TEST_ASSERT_TRUE( connectToServerWithBackoffRetries( &networkContext ) );

    /* Re-establish the persistent session with the broker by connecting with "clean session" flag set to 0. */
    TEST_ASSERT_FALSE( persistentSession );
    establishMqttSession( &context, &networkContext, false, &persistentSession );

    /* Verify that the session was resumed. */
    TEST_ASSERT_TRUE( persistentSession );
}

/* ============================   UNITY FIXTURES ============================ */

/* Called before each test method. */
void testSetUp()
{
    /* Reset file-scoped global variables. */
    receivedSubAck = false;
    receivedUnsubAck = false;
    receivedPubAck = false;
    receivedPubRec = false;
    receivedPubRel = false;
    receivedPubComp = false;
    receivedRetainedMessage = false;
    persistentSession = false;
    useLWTClientIdentifier = false;
    packetTypeForDisconnection = MQTT_PACKET_TYPE_INVALID;
    receivedPacketTypeForDisconnection = false;
    memset( &incomingInfo, 0u, sizeof( MQTTPublishInfo_t ) );
    receivedPublish = false;
    networkContext.pParams = &secureSocketsTransportParams;

    /* Establish TLS over TCP connection with retry attempts on failures. */
    TEST_ASSERT_TRUE( connectToServerWithBackoffRetries( &networkContext ) );

    /* Establish MQTT session on top of the TCP+TLS connection. */
    establishMqttSession( &context, &networkContext, true, &persistentSession );

    /* Ensure there are no retained messages on the test topics. AWS IoT does
     * not support retained messages. If a request is made to AWS IoT to retain
     * messages, the connection is disconnected.*/
    if( testingAgainstAWS == false )
    {
        clearRetainedMessage( &context, TEST_MQTT_TOPIC );
        clearRetainedMessage( &context, TEST_MQTT_TOPIC_2 );
    }
}

/* Called after each test method. */
void testTearDown()
{
    MQTTStatus_t mqttStatus;
    TransportSocketStatus_t transportStatus;

    /* Terminate MQTT connection. */
    mqttStatus = MQTT_Disconnect( &context );

    /* Terminate TLS session and TCP connection. */
    transportStatus = SecureSocketsTransport_Disconnect( &networkContext );

    /* Make any assertions at the end so that all memory is deallocated before
     * the end of this function. */
    TEST_ASSERT_EQUAL( MQTTSuccess, mqttStatus );
    TEST_ASSERT_EQUAL( TRANSPORT_SOCKET_STATUS_SUCCESS, transportStatus );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group for coreMQTT system tests of MQTT 3.1.1
 * features that are supported by AWS IoT.
 */
TEST_GROUP( coreMQTT_Integration_AWS_IoT_Compatible );

TEST_SETUP( coreMQTT_Integration_AWS_IoT_Compatible )
{
    testSetUp();
}

TEST_TEAR_DOWN( coreMQTT_Integration_AWS_IoT_Compatible )
{
    testTearDown();
}

/**
 * @brief Test group for running coreMQTT system tests with a broker
 * that supports all features of MQTT 3.1.1 specification.
 */
TEST_GROUP( coreMQTT_Integration );

TEST_SETUP( coreMQTT_Integration )
{
    testSetUp();
}

TEST_TEAR_DOWN( coreMQTT_Integration )
{
    testTearDown();
}

/* ========================== Test Cases ============================ */

/**
 * @brief Test group runner for MQTT system tests that can be run against AWS IoT.
 */
TEST_GROUP_RUNNER( coreMQTT_Integration_AWS_IoT_Compatible )
{
    testingAgainstAWS = true;
    RUN_TEST_CASE( coreMQTT_Integration_AWS_IoT_Compatible, Subscribe_Publish_With_Qos_0 );
    RUN_TEST_CASE( coreMQTT_Integration_AWS_IoT_Compatible, Subscribe_Publish_With_Qos_1 );
    RUN_TEST_CASE( coreMQTT_Integration_AWS_IoT_Compatible, Connect_LWT );
    RUN_TEST_CASE( coreMQTT_Integration_AWS_IoT_Compatible, ProcessLoop_KeepAlive );
    RUN_TEST_CASE( coreMQTT_Integration_AWS_IoT_Compatible, Resend_Unacked_Publish_QoS1 );
    RUN_TEST_CASE( coreMQTT_Integration_AWS_IoT_Compatible, Restore_Session_Duplicate_Incoming_Publish_Qos1 );
}

/**
 * @brief Test group runner for MQTT system tests against an non-AWS IoT MQTT 3.1.1 broker.
 */
TEST_GROUP_RUNNER( coreMQTT_Integration )
{
    testingAgainstAWS = false;
    RUN_TEST_CASE( coreMQTT_Integration, Subscribe_Publish_With_Qos_0 );
    RUN_TEST_CASE( coreMQTT_Integration, Subscribe_Publish_With_Qos_1 );
    RUN_TEST_CASE( coreMQTT_Integration, Connect_LWT );
    RUN_TEST_CASE( coreMQTT_Integration, ProcessLoop_KeepAlive );
    RUN_TEST_CASE( coreMQTT_Integration, Resend_Unacked_Publish_QoS1 );
    RUN_TEST_CASE( coreMQTT_Integration, Restore_Session_Duplicate_Incoming_Publish_Qos1 );
    RUN_TEST_CASE( coreMQTT_Integration, Restore_Session_Resend_PubRel );
    RUN_TEST_CASE( coreMQTT_Integration, Subscribe_Publish_With_Qos_2 );
    RUN_TEST_CASE( coreMQTT_Integration, Restore_Session_Incoming_Duplicate_PubRel );
    RUN_TEST_CASE( coreMQTT_Integration, Resend_Unacked_Publish_QoS2 );
    RUN_TEST_CASE( coreMQTT_Integration, Restore_Session_Duplicate_Incoming_Publish_Qos2 );
    RUN_TEST_CASE( coreMQTT_Integration, Publish_With_Retain_Flag );
}

/* ========================== Test Cases ============================ */

/**
 * @brief Tests Subscribe and Publish operations with the MQTT broker using QoS 0.
 * The test subscribes to a topic, and then publishes to the same topic. The
 * broker is expected to route the publish message back to the test.
 */
void Subscribe_Publish_With_Qos_0()
{
    /* Subscribe to a topic with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS0 ) );

    /* We expect a SUBACK from the broker for the subscribe operation. */
    TEST_ASSERT_FALSE( receivedSubAck );
    WAIT_FOR_PACKET( receivedSubAck, MQTTSuccess );

    /* Publish to the same topic, that we subscribed to, with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           &context,
                           TEST_MQTT_TOPIC,
                           MQTT_EXAMPLE_MESSAGE,
                           false, /* setRetainFlag */
                           false, /* isDuplicate */
                           MQTTQoS0,
                           MQTT_GetPacketId( &context ) ) );

    /* Call the MQTT library for the expectation to read an incoming PUBLISH for
     * the same message that we published (as we have subscribed to the same topic). */
    TEST_ASSERT_FALSE( receivedPubAck );
    WAIT_FOR_PACKET( receivedPublish, MQTTSuccess );
    /* We do not expect a PUBACK from the broker for the QoS 0 PUBLISH. */
    TEST_ASSERT_FALSE( receivedPubAck );

    /* Make sure that we have received the same message from the server,
     * that was published (as we have subscribed to the same topic). */
    TEST_ASSERT_EQUAL( MQTTQoS0, incomingInfo.qos );
    TEST_ASSERT_EQUAL( TEST_MQTT_TOPIC_LENGTH, incomingInfo.topicNameLength );
    TEST_ASSERT_EQUAL_MEMORY( TEST_MQTT_TOPIC,
                              incomingInfo.pTopicName,
                              TEST_MQTT_TOPIC_LENGTH );
    TEST_ASSERT_EQUAL( strlen( MQTT_EXAMPLE_MESSAGE ), incomingInfo.payloadLength );
    TEST_ASSERT_EQUAL_MEMORY( MQTT_EXAMPLE_MESSAGE,
                              incomingInfo.pPayload,
                              incomingInfo.payloadLength );

    /* Un-subscribe from a topic with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, unsubscribeFromTopic( &context, TEST_MQTT_TOPIC ) );

    /* We expect an UNSUBACK from the broker for the unsubscribe operation. */
    TEST_ASSERT_FALSE( receivedUnsubAck );
    WAIT_FOR_PACKET( receivedUnsubAck, MQTTSuccess );
}

/* Include Subscribe_Publish_With_Qos_0 test case in both test groups to run it against AWS IoT
 * and a different broker */
TEST( coreMQTT_Integration_AWS_IoT_Compatible, Subscribe_Publish_With_Qos_0 )
{
    Subscribe_Publish_With_Qos_0();
}

TEST( coreMQTT_Integration, Subscribe_Publish_With_Qos_0 )
{
    Subscribe_Publish_With_Qos_0();
}

/**
 * @brief Tests Subscribe and Publish operations with the MQTT broken using QoS 1.
 * The test subscribes to a topic, and then publishes to the same topic. The
 * broker is expected to route the publish message back to the test.
 */
void Subscribe_Publish_With_Qos_1()
{
    /* Subscribe to a topic with Qos 1. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS1 ) );

    /* Expect a SUBACK from the broker for the subscribe operation. */
    TEST_ASSERT_FALSE( receivedSubAck );
    WAIT_FOR_PACKET( receivedSubAck, MQTTSuccess );

    /* Publish to the same topic, that we subscribed to, with Qos 1. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           &context,
                           TEST_MQTT_TOPIC,
                           MQTT_EXAMPLE_MESSAGE,
                           false, /* setRetainFlag */
                           false, /* isDuplicate */
                           MQTTQoS1,
                           MQTT_GetPacketId( &context ) ) );

    /* Make sure that the MQTT context state was updated after the PUBLISH request. */
    TEST_ASSERT_EQUAL( MQTTQoS1, context.outgoingPublishRecords[ 0 ].qos );
    TEST_ASSERT_EQUAL( globalPublishPacketIdentifier, context.outgoingPublishRecords[ 0 ].packetId );
    TEST_ASSERT_EQUAL( MQTTPubAckPending, context.outgoingPublishRecords[ 0 ].publishState );

    /* Expect a PUBACK response for the PUBLISH and an incoming PUBLISH for the
     * same message that we published (as we have subscribed to the same topic). */
    TEST_ASSERT_FALSE( receivedPubAck );
    /* Make sure we have received PUBACK response. */
    WAIT_FOR_PACKET( receivedPubAck, MQTTSuccess );

    /* Make sure that we have received the same message from the server,
     * that was published (as we have subscribed to the same topic). */
    WAIT_FOR_PACKET( receivedPublish, MQTTSuccess );
    TEST_ASSERT_EQUAL( MQTTQoS1, incomingInfo.qos );
    TEST_ASSERT_EQUAL( TEST_MQTT_TOPIC_LENGTH, incomingInfo.topicNameLength );
    TEST_ASSERT_EQUAL_MEMORY( TEST_MQTT_TOPIC,
                              incomingInfo.pTopicName,
                              TEST_MQTT_TOPIC_LENGTH );
    TEST_ASSERT_EQUAL( strlen( MQTT_EXAMPLE_MESSAGE ), incomingInfo.payloadLength );
    TEST_ASSERT_EQUAL_MEMORY( MQTT_EXAMPLE_MESSAGE,
                              incomingInfo.pPayload,
                              incomingInfo.payloadLength );

    /* Un-subscribe from a topic with Qos 1. */
    TEST_ASSERT_EQUAL( MQTTSuccess, unsubscribeFromTopic( &context, TEST_MQTT_TOPIC ) );

    /* Expect an UNSUBACK from the broker for the unsubscribe operation. */
    TEST_ASSERT_FALSE( receivedUnsubAck );
    WAIT_FOR_PACKET( receivedUnsubAck, MQTTSuccess );
}

/* Include Subscribe_Publish_With_Qos_1 test case in both test groups to run it against AWS IoT
 * and a different broker */
TEST( coreMQTT_Integration_AWS_IoT_Compatible, Subscribe_Publish_With_Qos_1 )
{
    Subscribe_Publish_With_Qos_1();
}

TEST( coreMQTT_Integration, Subscribe_Publish_With_Qos_1 )
{
    Subscribe_Publish_With_Qos_1();
}


/**
 * @brief Tests Subscribe and Publish operations with the MQTT broken using QoS 2.
 * The test subscribes to a topic, and then publishes to the same topic. The
 * broker is expected to route the publish message back to the test.
 */
TEST( coreMQTT_Integration, Subscribe_Publish_With_Qos_2 )
{
    /* Subscribe to a topic with Qos 2. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS2 ) );

    /* Expect a SUBACK from the broker for the subscribe operation. */
    TEST_ASSERT_FALSE( receivedSubAck );
    WAIT_FOR_PACKET( receivedSubAck, MQTTSuccess );

    /* Publish to the same topic, that we subscribed to, with Qos 2. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           &context,
                           TEST_MQTT_TOPIC,
                           MQTT_EXAMPLE_MESSAGE,
                           false, /* setRetainFlag */
                           false, /* isDuplicate */
                           MQTTQoS2,
                           MQTT_GetPacketId( &context ) ) );

    /* Make sure that the MQTT context state was updated after the PUBLISH request. */
    TEST_ASSERT_EQUAL( MQTTQoS2, context.outgoingPublishRecords[ 0 ].qos );
    TEST_ASSERT_EQUAL( globalPublishPacketIdentifier, context.outgoingPublishRecords[ 0 ].packetId );
    TEST_ASSERT_EQUAL( MQTTPubRecPending, context.outgoingPublishRecords[ 0 ].publishState );

    /* We expect PUBREC and PUBCOMP responses for the PUBLISH request, and
     * incoming PUBLISH with the same message that we published (as we are subscribed
     * to the same topic). Also, we expect a PUBREL ack response from the server for
     * the incoming PUBLISH (as we subscribed and publish with QoS 2). Since it takes
     * longer to complete a QoS 2 publish, we run the process loop longer to allow it
     * ample time. */
    TEST_ASSERT_FALSE( receivedPubAck );
    TEST_ASSERT_FALSE( receivedPubRec );
    TEST_ASSERT_FALSE( receivedPubComp );
    TEST_ASSERT_FALSE( receivedPubRel );
    WAIT_FOR_PACKET( receivedPubRec, MQTTSuccess );
    WAIT_FOR_PACKET( receivedPubComp, MQTTSuccess );
    WAIT_FOR_PACKET( receivedPubRel, MQTTSuccess );
    TEST_ASSERT_FALSE( receivedPubAck );

    /* Make sure that we have received the same message from the server,
     * that was published (as we have subscribed to the same topic). */
    WAIT_FOR_PACKET( receivedPublish, MQTTSuccess );
    TEST_ASSERT_EQUAL( MQTTQoS2, incomingInfo.qos );
    TEST_ASSERT_EQUAL( TEST_MQTT_TOPIC_LENGTH, incomingInfo.topicNameLength );
    TEST_ASSERT_EQUAL_MEMORY( TEST_MQTT_TOPIC,
                              incomingInfo.pTopicName,
                              TEST_MQTT_TOPIC_LENGTH );
    TEST_ASSERT_EQUAL( strlen( MQTT_EXAMPLE_MESSAGE ), incomingInfo.payloadLength );
    TEST_ASSERT_EQUAL_MEMORY( MQTT_EXAMPLE_MESSAGE,
                              incomingInfo.pPayload,
                              incomingInfo.payloadLength );

    /* Un-subscribe from a topic with Qos 2. */
    TEST_ASSERT_EQUAL( MQTTSuccess, unsubscribeFromTopic( &context, TEST_MQTT_TOPIC ) );

    /* Expect an UNSUBACK from the broker for the unsubscribe operation. */
    TEST_ASSERT_FALSE( receivedUnsubAck );
    WAIT_FOR_PACKET( receivedUnsubAck, MQTTSuccess );
}

/**
 * @brief Verifies that the MQTT library supports the "Last Will and Testament" feature when
 * establishing a connection with a broker.
 */
void Connect_LWT()
{
    NetworkContext_t secondNetworkContext = { 0 };
    SecureSocketsTransportParams_t secondSecureSocketsTransportParams = { 0 };
    bool sessionPresent;
    MQTTContext_t secondContext;

    secondNetworkContext.pParams = &secondSecureSocketsTransportParams;

    /* Establish a second TCP connection with the server endpoint, then
     * a TLS session. The server info and credentials can be reused. */
    TEST_ASSERT_TRUE( connectToServerWithBackoffRetries( &secondNetworkContext ) );

    /* Establish MQTT session on top of the TCP+TLS connection. */
    useLWTClientIdentifier = true;
    establishMqttSession( &secondContext, &secondNetworkContext, true, &sessionPresent );

    /* Subscribe to LWT Topic. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic(
                           &context, TEST_MQTT_LWT_TOPIC, MQTTQoS0 ) );

    /* Wait for the SUBACK response from the broker for the subscribe request. */
    TEST_ASSERT_FALSE( receivedSubAck );
    WAIT_FOR_PACKET( receivedSubAck, MQTTSuccess );

    /* Abruptly terminate TCP connection. */
    ( void ) SecureSocketsTransport_Disconnect( &secondNetworkContext );

    /* Run the process loop to receive the LWT. Allow some more time for the
     * server to realize the connection is closed. */
    WAIT_FOR_PACKET( receivedPublish, MQTTSuccess );

    /* Test if we have received the LWT. */
    TEST_ASSERT_EQUAL( MQTTQoS0, incomingInfo.qos );
    TEST_ASSERT_EQUAL( TEST_MQTT_LWT_TOPIC_LENGTH, incomingInfo.topicNameLength );
    TEST_ASSERT_EQUAL_MEMORY( TEST_MQTT_LWT_TOPIC,
                              incomingInfo.pTopicName,
                              TEST_MQTT_LWT_TOPIC_LENGTH );
    TEST_ASSERT_EQUAL( strlen( MQTT_EXAMPLE_MESSAGE ), incomingInfo.payloadLength );
    TEST_ASSERT_EQUAL_MEMORY( MQTT_EXAMPLE_MESSAGE,
                              incomingInfo.pPayload,
                              incomingInfo.payloadLength );

    /* Un-subscribe from a topic with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, unsubscribeFromTopic( &context, TEST_MQTT_TOPIC ) );

    /* We expect an UNSUBACK from the broker for the unsubscribe operation. */
    TEST_ASSERT_FALSE( receivedUnsubAck );
    WAIT_FOR_PACKET( receivedUnsubAck, MQTTSuccess );
}

/* Include Connect_LWT test group case both test groups to run it against AWS IoT
 * and a different broker */
TEST( coreMQTT_Integration_AWS_IoT_Compatible, Connect_LWT )
{
    Connect_LWT();
}

TEST( coreMQTT_Integration, Connect_LWT )
{
    Connect_LWT();
}


/**
 * @brief Verifies that the MQTT library sends a Ping Request packet if the connection is
 * idle for more than the keep-alive period.
 */
void ProcessLoop_KeepAlive()
{
    uint32_t connectPacketTime = context.lastPacketTime;
    uint32_t elapsedTime = 0;

    TEST_ASSERT_EQUAL( 0, context.pingReqSendTimeMs );

    /* Sleep until control packet needs to be sent. */
    vTaskDelay( pdMS_TO_TICKS( MQTT_KEEP_ALIVE_INTERVAL_SECONDS * 1000 ) );
    TEST_ASSERT_EQUAL( MQTTSuccess, MQTT_ProcessLoop( &context, MQTT_PROCESS_LOOP_TIMEOUT_MS ) );

    TEST_ASSERT_NOT_EQUAL( 0, context.pingReqSendTimeMs );
    TEST_ASSERT_NOT_EQUAL( connectPacketTime, context.lastPacketTime );
    /* Test that the ping was sent within 1.5 times the keep alive interval. */
    elapsedTime = context.lastPacketTime - connectPacketTime;
    TEST_ASSERT_LESS_OR_EQUAL( MQTT_KEEP_ALIVE_INTERVAL_SECONDS * 1500, elapsedTime );
}

/* Include ProcessLoop_KeepAlive test case in both test groups to run it against AWS IoT
 * and a different broker */
TEST( coreMQTT_Integration_AWS_IoT_Compatible, ProcessLoop_KeepAlive )
{
    ProcessLoop_KeepAlive();
}

TEST( coreMQTT_Integration, ProcessLoop_KeepAlive )
{
    ProcessLoop_KeepAlive();
}


/**
 * @brief Verifies the behavior of the MQTT library in a restored session connection with the broker
 * for a PUBLISH operation that was incomplete in the previous connection.
 * Tests that the library resends PUBREL packets to the broker in a restored session for an incomplete
 * PUBLISH operation in a previous connection.
 */
TEST( coreMQTT_Integration, Restore_Session_Resend_PubRel )
{
    /* Start a persistent session with the broker. */
    startPersistentSession();

    /* Publish to a topic with Qos 2. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           &context,
                           TEST_MQTT_TOPIC,
                           MQTT_EXAMPLE_MESSAGE,
                           false, /* setRetainFlag */
                           false, /* isDuplicate */
                           MQTTQoS2,
                           MQTT_GetPacketId( &context ) ) );

    /* Disconnect on receiving PUBREC so that we are not able to complete the QoS 2 PUBLISH in the current connection. */
    TEST_ASSERT_FALSE( receivedPubComp );
    TEST_ASSERT_FALSE( receivedPacketTypeForDisconnection );
    packetTypeForDisconnection = MQTT_PACKET_TYPE_PUBREC;
    WAIT_FOR_PACKET( receivedPacketTypeForDisconnection, MQTTSendFailed );
    TEST_ASSERT_FALSE( receivedPubComp );

    /* Clear the global variable. */
    packetTypeForDisconnection = MQTT_PACKET_TYPE_INVALID;

    /* We will re-establish an MQTT over TLS connection with the broker to restore
     * the persistent session. */
    resumePersistentSession();

    /* Resume the incomplete QoS 2 PUBLISH in previous MQTT connection. */
    /* Test that the MQTT library has completed the QoS 2 publish by sending the PUBREL flag. */
    TEST_ASSERT_FALSE( receivedPubComp );
    WAIT_FOR_PACKET( receivedPubComp, MQTTSuccess );
}

/**
 * @brief Verifies the behavior of the MQTT library on receiving a duplicate
 * PUBREL packet from the broker in a restored session connection.
 * Tests that the library sends a PUBCOMP packet in response to the broker for the
 * incoming QoS 2 PUBLISH operation that was incomplete in a previous connection
 * of the same session.
 */
TEST( coreMQTT_Integration, Restore_Session_Incoming_Duplicate_PubRel )
{
    /* Start a persistent session with the broker. */
    startPersistentSession();

    /* Subscribe to a topic from which we will be receiving an incomplete incoming
     * QoS 2 PUBLISH transaction in this connection. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS2 ) );
    TEST_ASSERT_FALSE( receivedSubAck );
    WAIT_FOR_PACKET( receivedSubAck, MQTTSuccess );

    /* Publish to the same topic with Qos 2 (so that the broker can re-publish it back to us). */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           &context,
                           TEST_MQTT_TOPIC,
                           MQTT_EXAMPLE_MESSAGE,
                           false, /* setRetainFlag */
                           false, /* isDuplicate */
                           MQTTQoS2,
                           MQTT_GetPacketId( &context ) ) );

    /* Disconnect on receiving PUBREL so that we are not able to complete in the incoming QoS2
     * PUBLISH in the current connection. */
    packetTypeForDisconnection = MQTT_PACKET_TYPE_PUBREL;
    TEST_ASSERT_FALSE( receivedPacketTypeForDisconnection );
    WAIT_FOR_PACKET( receivedPacketTypeForDisconnection, MQTTSendFailed );

    /* We will re-establish an MQTT over TLS connection with the broker to restore
     * the persistent session. */
    resumePersistentSession();

    /* Clear the global variable for not disconnecting on PUBREL
     * that we receive from the broker on the session restoration. */
    packetTypeForDisconnection = MQTT_PACKET_TYPE_INVALID;

    /* Resume the incomplete incoming QoS 2 PUBLISH transaction from the previous MQTT connection. */
    TEST_ASSERT_FALSE( receivedPubRel );
    /* Make sure that the broker resent the PUBREL packet on session restoration. */
    WAIT_FOR_PACKET( receivedPubRel, MQTTSuccess );

    /* Make sure that the library sent a PUBCOMP packet in response to the PUBREL packet
     * from the server to complete the incoming PUBLISH QoS2 transaction. */
    TEST_ASSERT_EQUAL( MQTT_PACKET_ID_INVALID, context.incomingPublishRecords[ 0 ].packetId );
}

/**
 * @brief Verifies that the MQTT library supports resending a PUBLISH QoS 1 packet which is
 * un-acknowledged in its first attempt.
 * Tests that the library is able to support resending the PUBLISH packet with the DUP flag.
 */
void Resend_Unacked_Publish_QoS1()
{
    /* Start a persistent session with the broker. */
    startPersistentSession();

    /* Initiate the PUBLISH operation at QoS 1. The library should add an
     * outgoing PUBLISH record in the context. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           &context,
                           TEST_MQTT_TOPIC,
                           MQTT_EXAMPLE_MESSAGE,
                           false, /* setRetainFlag */
                           false, /* isDuplicate */
                           MQTTQoS1,
                           MQTT_GetPacketId( &context ) ) );

    /* Setup the MQTT connection to terminate to simulate incomplete PUBLISH operation. */
    context.transportInterface.recv = failedRecv;

    /* Attempt to complete the PUBLISH operation at QoS1 which should fail due
     * to terminated network connection.
     * The abrupt network disconnection should cause the PUBLISH packet to be left
     * in an un-acknowledged state in the MQTT context. */
    TEST_ASSERT_EQUAL( MQTTRecvFailed,
                       MQTT_ProcessLoop( &context, 2 * MQTT_PROCESS_LOOP_TIMEOUT_MS ) );

    /* Verify that the library has stored the PUBLISH as an incomplete operation. */
    TEST_ASSERT_NOT_EQUAL( MQTT_PACKET_ID_INVALID, context.outgoingPublishRecords[ 0 ].packetId );


    /* Reset the transport receive function in the context. */
    context.transportInterface.recv = SecureSocketsTransport_Recv;

    /* We will re-establish an MQTT over TLS connection with the broker to restore
     * the persistent session. */
    resumePersistentSession();

    /* Obtain the packet ID of the PUBLISH packet that didn't complete in the previous connection. */
    MQTTStateCursor_t cursor = MQTT_STATE_CURSOR_INITIALIZER;
    uint16_t publishPackedId = MQTT_PublishToResend( &context, &cursor );
    TEST_ASSERT_NOT_EQUAL( MQTT_PACKET_ID_INVALID, publishPackedId );

    /* Make sure that the packet ID is maintained in the outgoing publish state records. */
    TEST_ASSERT_EQUAL( context.outgoingPublishRecords[ 0 ].packetId, publishPackedId );

    /* Resend the PUBLISH packet that didn't complete in the previous connection. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           &context,
                           TEST_MQTT_TOPIC,
                           MQTT_EXAMPLE_MESSAGE,
                           false, /* setRetainFlag */
                           true,  /* isDuplicate */
                           MQTTQoS1,
                           publishPackedId ) );

    /* Complete the QoS 1 PUBLISH resend operation. */
    TEST_ASSERT_FALSE( receivedPubAck );
    /* Make sure that the PUBLISH resend was complete. */
    WAIT_FOR_PACKET( receivedPubAck, MQTTSuccess );

    /* Make sure that the library has removed the record for the outgoing PUBLISH packet. */
    TEST_ASSERT_EQUAL( MQTT_PACKET_ID_INVALID, context.outgoingPublishRecords[ 0 ].packetId );
}

/* Include Resend_Unacked_Publish_QoS1 test case in both test groups to run it against AWS IoT
 * and a different broker */
TEST( coreMQTT_Integration_AWS_IoT_Compatible, Resend_Unacked_Publish_QoS1 )
{
    Resend_Unacked_Publish_QoS1();
}

TEST( coreMQTT_Integration, Resend_Unacked_Publish_QoS1 )
{
    Resend_Unacked_Publish_QoS1();
}


/**
 * @brief Verifies that the MQTT library supports resending a PUBLISH QoS 2 packet which is
 * un-acknowledged in its first attempt.
 * Tests that the library is able to support resending the PUBLISH packet with the DUP flag.
 */
TEST( coreMQTT_Integration, Resend_Unacked_Publish_QoS2 )
{
    /* Start a persistent session with the broker. */
    startPersistentSession();

    /* Initiate the PUBLISH operation at QoS 2. The library should add an
     * outgoing PUBLISH record in the context. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           &context,
                           TEST_MQTT_TOPIC,
                           MQTT_EXAMPLE_MESSAGE,
                           false, /* setRetainFlag */
                           false, /* isDuplicate */
                           MQTTQoS2,
                           MQTT_GetPacketId( &context ) ) );

    /* Setup the MQTT connection to terminate to simulate incomplete PUBLISH operation. */
    context.transportInterface.recv = failedRecv;

    /* Attempt to complete the PUBLISH operation at QoS 2 which should fail due
     * to terminated network connection.
     * The abrupt network disconnection should cause the PUBLISH packet to be left
     * in an un-acknowledged state in the MQTT context. */
    TEST_ASSERT_EQUAL( MQTTRecvFailed,
                       MQTT_ProcessLoop( &context, 2 * MQTT_PROCESS_LOOP_TIMEOUT_MS ) );

    /* Verify that the library has stored the PUBLISH as an incomplete operation. */
    TEST_ASSERT_NOT_EQUAL( MQTT_PACKET_ID_INVALID, context.outgoingPublishRecords[ 0 ].packetId );

    /* Reset the transport receive function in the context. */
    context.transportInterface.recv = SecureSocketsTransport_Recv;

    /* We will re-establish an MQTT over TLS connection with the broker to restore
     * the persistent session. */
    resumePersistentSession();

    /* Obtain the packet ID of the PUBLISH packet that didn't complete in the previous connection. */
    MQTTStateCursor_t cursor = MQTT_STATE_CURSOR_INITIALIZER;
    uint16_t publishPackedId = MQTT_PublishToResend( &context, &cursor );
    TEST_ASSERT_NOT_EQUAL( MQTT_PACKET_ID_INVALID, publishPackedId );

    /* Make sure that the packet ID is maintained in the outgoing publish state records. */
    TEST_ASSERT_EQUAL( context.outgoingPublishRecords[ 0 ].packetId, publishPackedId );

    /* Resend the PUBLISH packet that didn't complete in the previous connection. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           &context,
                           TEST_MQTT_TOPIC,
                           MQTT_EXAMPLE_MESSAGE,
                           false, /* setRetainFlag */
                           true,  /* isDuplicate */
                           MQTTQoS2,
                           publishPackedId ) );

    /* Complete the QoS 2 PUBLISH resend operation. */
    TEST_ASSERT_FALSE( receivedPubRec );
    TEST_ASSERT_FALSE( receivedPubComp );
    /* Make sure that the QoS 2 PUBLISH re-transmission was complete. */
    WAIT_FOR_PACKET( receivedPubRec, MQTTSuccess );
    WAIT_FOR_PACKET( receivedPubComp, MQTTSuccess );

    /* Make sure that the library has removed the record for the outgoing PUBLISH packet. */
    TEST_ASSERT_EQUAL( MQTT_PACKET_ID_INVALID, context.outgoingPublishRecords[ 0 ].packetId );
}

/**
 * @brief Verifies the behavior of the MQTT library on receiving a duplicate
 * QoS 1 PUBLISH packet from the broker in a restored session connection.
 * Tests that the library responds with a PUBACK to the duplicate incoming QoS 1 PUBLISH
 * packet that was un-acknowledged in a previous connection of the same session.
 */
void Restore_Session_Duplicate_Incoming_Publish_Qos1()
{
    /* Start a persistent session with the broker. */
    startPersistentSession();

    /* Subscribe to a topic from which we will be receiving an incomplete incoming
     * QoS 2 PUBLISH transaction in this connection. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS1 ) );
    TEST_ASSERT_FALSE( receivedSubAck );
    WAIT_FOR_PACKET( receivedSubAck, MQTTSuccess );

    /* Publish to the same topic with Qos 1 (so that the broker can re-publish it back to us). */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           &context,
                           TEST_MQTT_TOPIC,
                           MQTT_EXAMPLE_MESSAGE,
                           false, /* setRetainFlag */
                           false, /* isDuplicate */
                           MQTTQoS1,
                           MQTT_GetPacketId( &context ) ) );

    /* Disconnect on receiving the incoming PUBLISH packet from the broker so that
     * an acknowledgement cannot be sent to the broker. */
    packetTypeForDisconnection = MQTT_PACKET_TYPE_PUBLISH;
    TEST_ASSERT_FALSE( receivedPacketTypeForDisconnection );
    WAIT_FOR_PACKET( receivedPacketTypeForDisconnection, MQTTSendFailed );

    /* Make sure that a record was created for the incoming PUBLISH packet. */
    TEST_ASSERT_NOT_EQUAL( MQTT_PACKET_ID_INVALID, context.incomingPublishRecords[ 0 ].packetId );

    if( testingAgainstAWS )
    {
        /* Add some delay to wait for AWS IoT Core to resend the PUBLISH. */
        vTaskDelay( pdMS_TO_TICKS( AWS_IOT_CORE_REPUBLISH_INTERVAL_MS ) );
    }

    /* We will re-establish an MQTT over TLS connection with the broker to restore
     * the persistent session. */
    resumePersistentSession();

    /* Clear the global variable for not disconnecting on the duplicate PUBLISH
     * packet that we receive from the broker on the session restoration. */
    packetTypeForDisconnection = MQTT_PACKET_TYPE_INVALID;

    /* Process the duplicate incoming QoS 1 PUBLISH that will be sent by the broker
     * to re-attempt the PUBLISH operation. */
    TEST_ASSERT_NOT_EQUAL( MQTT_PACKET_ID_INVALID, context.incomingPublishRecords[ 0 ].packetId );
    /* Make sure that the library cleared the record for the incoming QoS 1 PUBLISH packet. */
    WAIT_FOR_PACKET( ( MQTT_PACKET_ID_INVALID == context.incomingPublishRecords[ 0 ].packetId ),
                     MQTTSuccess );
}

/* Include Restore_Session_Duplicate_Incoming_Publish_Qos1 test case in both test groups to run it against AWS IoT
 * and a different broker */
TEST( coreMQTT_Integration_AWS_IoT_Compatible, Restore_Session_Duplicate_Incoming_Publish_Qos1 )
{
    Restore_Session_Duplicate_Incoming_Publish_Qos1();
}

TEST( coreMQTT_Integration, Restore_Session_Duplicate_Incoming_Publish_Qos1 )
{
    Restore_Session_Duplicate_Incoming_Publish_Qos1();
}


/**
 * @brief Verifies the behavior of the MQTT library on receiving a duplicate
 * QoS 2 PUBLISH packet from the broker in a restored session connection.
 * Tests that the library responds with the ack packets for the incoming duplicate
 * QoS 2 PUBLISH packet that was un-acknowledged in a previous connection of the same session.
 */
TEST( coreMQTT_Integration, Restore_Session_Duplicate_Incoming_Publish_Qos2 )
{
    /* Start a persistent session with the broker. */
    startPersistentSession();

    /* Subscribe to a topic from which we will be receiving an incomplete incoming
     * QoS 2 PUBLISH transaction in this connection. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS2 ) );
    TEST_ASSERT_FALSE( receivedSubAck )
    WAIT_FOR_PACKET( receivedSubAck, MQTTSuccess );

    /* Publish to the same topic with Qos 2 (so that the broker can re-publish it back to us). */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           &context,
                           TEST_MQTT_TOPIC,
                           MQTT_EXAMPLE_MESSAGE,
                           false, /* setRetainFlag */
                           false, /* isDuplicate */
                           MQTTQoS2,
                           MQTT_GetPacketId( &context ) ) );

    /* Disconnect on receiving the incoming PUBLISH packet from the broker so that
     * an acknowledgement cannot be sent to the broker. */
    packetTypeForDisconnection = MQTT_PACKET_TYPE_PUBLISH;
    TEST_ASSERT_FALSE( receivedPacketTypeForDisconnection )
    WAIT_FOR_PACKET( receivedPacketTypeForDisconnection, MQTTSendFailed );

    /* Make sure that a record was created for the incoming PUBLISH packet. */
    TEST_ASSERT_NOT_EQUAL( MQTT_PACKET_ID_INVALID, context.incomingPublishRecords[ 0 ].packetId );

    /* We will re-establish an MQTT over TLS connection with the broker to restore
     * the persistent session. */
    resumePersistentSession();

    /* Clear the global variable for not disconnecting on the duplicate PUBLISH
     * packet that we receive from the broker on the session restoration. */
    packetTypeForDisconnection = MQTT_PACKET_TYPE_INVALID;

    /* Process the duplicate incoming QoS 2 PUBLISH that will be sent by the broker
     * to re-attempt the PUBLISH operation. */
    TEST_ASSERT_FALSE( receivedPubRel );
    /* Make sure that the incoming QoS 2 transaction was completed. */
    WAIT_FOR_PACKET( receivedPubRel, MQTTSuccess );

    /* Make sure that the library cleared the record for the incoming QoS 2 PUBLISH packet. */
    TEST_ASSERT_EQUAL( MQTT_PACKET_ID_INVALID, context.incomingPublishRecords[ 0 ].packetId );
}

/**
 * @brief Verifies that the library supports notifying the broker to retain a PUBLISH message
 * for a topic using the retain flag.
 */
TEST( coreMQTT_Integration, Publish_With_Retain_Flag )
{
    /* Publish to a topic with the "retain" flag set. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic( &context,
                                                    TEST_MQTT_TOPIC,
                                                    MQTT_EXAMPLE_MESSAGE,
                                                    true,  /* setRetainFlag */
                                                    false, /* isDuplicate */
                                                    MQTTQoS1,
                                                    MQTT_GetPacketId( &context ) ) );
    /* Complete the QoS 1 PUBLISH operation. */
    TEST_ASSERT_FALSE( receivedPubAck );
    WAIT_FOR_PACKET( receivedPubAck, MQTTSuccess );

    /* Subscribe to the same topic that we published the message to.
     * The broker should send the "retained" message with the "retain" flag set. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS1 ) );
    TEST_ASSERT_FALSE( receivedSubAck );
    WAIT_FOR_PACKET( receivedSubAck, MQTTSuccess );

    /* Make sure that the library invoked the event callback with the incoming PUBLISH from
     * the broker containing the "retained" flag set. */
    WAIT_FOR_PACKET( receivedRetainedMessage, MQTTSuccess );

    /* Reset the global variables for the remainder of the test. */
    receivedPubAck = false;
    receivedSubAck = false;
    receivedUnsubAck = false;
    receivedRetainedMessage = false;

    /* Publish to another topic with the "retain" flag set to 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic( &context,
                                                    TEST_MQTT_TOPIC_2,
                                                    MQTT_EXAMPLE_MESSAGE,
                                                    false, /* setRetainFlag */
                                                    false, /* isDuplicate */
                                                    MQTTQoS1,
                                                    MQTT_GetPacketId( &context ) ) );

    /* Complete the QoS 1 PUBLISH operation. */
    TEST_ASSERT_FALSE( receivedPubAck );
    WAIT_FOR_PACKET( receivedPubAck, MQTTSuccess );

    /* Again, subscribe to the same topic that we just published to.
     * We don't expect the broker to send the message to us (as we
     * PUBLISHed without a retain flag set). */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic(
                           &context, TEST_MQTT_TOPIC_2, MQTTQoS1 ) );
    TEST_ASSERT_FALSE( receivedSubAck );
    WAIT_FOR_PACKET( receivedSubAck, MQTTSuccess );

    /* Make sure that the library did not receive an incoming PUBLISH from the broker. */
    TEST_ASSERT_FALSE( receivedRetainedMessage );
}
