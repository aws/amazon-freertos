/*
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
 * @file mqtt_system_test.c
 * @brief Integration tests for the MQTT library when communication with AWS IoT
 * from a POSIX platform.
 */

#include <string.h>
#include <stdbool.h>
#include <assert.h>

/* Include config file before other non-system includes. */
#include "test_config.h"

#include "unity.h"
/* Include paths for public enums, structures, and macros. */
#include "mqtt.h"

/* Include header for TLS utilities. */
#include "transport_utils.h"

#ifndef BROKER_ENDPOINT
    #error "BROKER_ENDPOINT should be defined for the MQTT integration tests."
#endif

#ifndef SERVER_ROOT_CA_CERT_PATH
    #error "SERVER_ROOT_CA_CERT_PATH should be defined for the MQTT integration tests."
#endif

#ifndef CLIENT_IDENTIFIER
    #error "CLIENT_IDENTIFIER should be defined for the MQTT integration tests."
#endif

/**
 * @brief A valid starting packet ID per MQTT spec. Start from 1.
 */
#define MQTT_FIRST_VALID_PACKET_ID          ( 1 )

/**
 * @brief A PINGREQ packet is always 2 bytes in size, defined by MQTT 3.1.1 spec.
 */
#define MQTT_PACKET_PINGREQ_SIZE            ( 2U )

/**
 * @brief A packet type not handled by MQTT_ProcessLoop.
 */
#define MQTT_PACKET_TYPE_INVALID            ( 0U )

/**
 * @brief Number of milliseconds in a second.
 */
#define MQTT_ONE_SECOND_TO_MS               ( 1000U )

/**
 * @brief Length of the MQTT network buffer.
 */
#define MQTT_TEST_BUFFER_LENGTH             ( 128 )

/**
 * @brief Sample length of remaining serialized data.
 */
#define MQTT_SAMPLE_REMAINING_LENGTH        ( 64 )

/**
 * @brief Subtract this value from max value of global entry time
 * for the timer overflow test.
 */
#define MQTT_OVERFLOW_OFFSET                ( 3 )

/**
 * @brief Sample topic filter to subscribe to.
 */
#define TEST_MQTT_TOPIC                     "/iot/integration/test"

/**
 * @brief Length of sample topic filter.
 */
#define TEST_MQTT_TOPIC_LENGTH              ( sizeof( TEST_MQTT_TOPIC ) - 1 )

/**
 * @brief Size of the network buffer for MQTT packets.
 */
#define NETWORK_BUFFER_SIZE                 ( 1024U )

/**
 * @brief Client identifier for MQTT session in the tests.
 */
#define TEST_CLIENT_IDENTIFIER              "MQTT-Test"

/**
 * @brief Length of the client identifier.
 */
#define TEST_CLIENT_IDENTIFIER_LENGTH       ( sizeof( TEST_CLIENT_IDENTIFIER ) - 1u )

/**
 * @brief Timeout for receiving CONNACK packet in milli seconds.
 */
#define CONNACK_RECV_TIMEOUT_MS             ( 1000U )

/**
 * @brief Time interval in seconds at which an MQTT PINGREQ need to be sent to
 * broker.
 */
#define MQTT_KEEP_ALIVE_INTERVAL_SECONDS    ( 30U )

/**
 * @brief Timeout for MQTT_ProcessLoop() function in milliseconds.
 * The timeout value is appropriately chosen for receiving an incoming
 * PUBLISH message and ack responses for QoS 1 and QoS 2 communications
 * with the broker.
 */
#define MQTT_PROCESS_LOOP_TIMEOUT_MS        ( 700U )

/**
 * @brief The MQTT message published in this example.
 */
#define MQTT_EXAMPLE_MESSAGE                "Hello World!"

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
 * @brief Variable to store TCP socket descriptor used for connecting to and
 * disconnecting from the broker.
 */
static int tcpSocket;

/**
 * @brief Represents the OpenSSL context used for TLS session with the broker
 * for tests.
 */
static SSL * pSslContext;

/**
 * @brief The context representing the MQTT connection with the broker for
 * the test case.
 */
static MQTTContext_t context;

/**
 * @brief Flag that represents whether a persistent session should be
 * established with the broker for the test.
 */
static bool persistentSession = false;

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
 * @brief Represents incoming PUBLISH information.
 */
static MQTTPublishInfo_t incomingInfo;

/**
 * @brief Sends an MQTT CONNECT packet over the already connected TCP socket.
 *
 * @param[in] pContext MQTT context pointer.
 * @param[in] pSslContext SSL context.
 * @param[in] createCleanSession Creates a new MQTT session if true.
 * If false, tries to establish the existing session if there was session
 * already present in broker.
 * @param[out] pSessionPresent Session was already present in the broker or not.
 * Session present response is obtained from the CONNACK from broker.
 */
static void establishMqttSession( MQTTContext_t * pContext,
                                  SSL * pSslContext,
                                  bool createCleanSession,
                                  bool * pSessionPresent );

/**
 * @brief The application callback function that is expected to be invoked by the
 * MQTT library for incoming publish and incoming acks received over the network.
 *
 * @param[in] pContext MQTT context pointer.
 * @param[in] pPacketInfo Packet Info pointer for the incoming packet.
 * @param[in] packetIdentifier Packet identifier of the incoming packet.
 * @param[in] pPublishInfo Deserialized publish info pointer for the incoming
 * packet.
 */
static void eventCallback( MQTTContext_t * pContext,
                           MQTTPacketInfo_t * pPacketInfo,
                           uint16_t packetIdentifier,
                           MQTTPublishInfo_t * pPublishInfo );

/*-----------------------------------------------------------*/

static void establishMqttSession( MQTTContext_t * pContext,
                                  SSL * pSslContext,
                                  bool createCleanSession,
                                  bool * pSessionPresent )
{
    MQTTConnectInfo_t connectInfo;
    MQTTTransportInterface_t transport;
    MQTTFixedBuffer_t networkBuffer;
    MQTTApplicationCallbacks_t callbacks;

    assert( pContext != NULL );
    assert( pSslContext != NULL );

    /* The network buffer must remain valid for the lifetime of the MQTT context. */
    static uint8_t buffer[ NETWORK_BUFFER_SIZE ];

    /* Setup the transport interface object for the library. */
    transport.networkContext = pSslContext;
    transport.send = transportSend;
    transport.recv = transportRecv;

    /* Fill the values for network buffer. */
    networkBuffer.pBuffer = buffer;
    networkBuffer.size = NETWORK_BUFFER_SIZE;

    /* Application callbacks for receiving incoming publishes and incoming acks
     * from MQTT library. */
    callbacks.appCallback = eventCallback;

    /* Application callback for getting the time for MQTT library. This time
     * function will be used to calculate intervals in MQTT library.*/
    callbacks.getTime = getTimeMs;

    /* Initialize MQTT library. */
    TEST_ASSERT_EQUAL( MQTTSuccess, MQTT_Init( pContext,
                                               &transport,
                                               &callbacks,
                                               &networkBuffer ) );

    /* Establish MQTT session with a CONNECT packet. */

    connectInfo.cleanSession = createCleanSession;

    connectInfo.pClientIdentifier = TEST_CLIENT_IDENTIFIER;
    connectInfo.clientIdentifierLength = TEST_CLIENT_IDENTIFIER_LENGTH;

    /* The interval at which an MQTT PINGREQ needs to be sent out to broker. */
    connectInfo.keepAliveSeconds = MQTT_KEEP_ALIVE_INTERVAL_SECONDS;

    /* Username and password for authentication. Not used in this test. */
    connectInfo.pUserName = NULL;
    connectInfo.userNameLength = 0U;
    connectInfo.pPassword = NULL;
    connectInfo.passwordLength = 0U;

    /* Send MQTT CONNECT packet to broker. */
    TEST_ASSERT_EQUAL( MQTTSuccess, MQTT_Connect( pContext,
                                                  &connectInfo,
                                                  NULL,
                                                  CONNACK_RECV_TIMEOUT_MS,
                                                  pSessionPresent ) );
}

static void eventCallback( MQTTContext_t * pContext,
                           MQTTPacketInfo_t * pPacketInfo,
                           uint16_t packetIdentifier,
                           MQTTPublishInfo_t * pPublishInfo )
{
    assert( pContext != NULL );
    assert( pPacketInfo != NULL );

    /* Handle incoming publish. The lower 4 bits of the publish packet
     * type is used for the dup, QoS, and retain flags. Hence masking
     * out the lower bits to check if the packet is publish. */
    if( ( pPacketInfo->type & 0xF0U ) == MQTT_PACKET_TYPE_PUBLISH )
    {
        assert( pPublishInfo != NULL );
        /* Handle incoming publish. */

        /* Cache information about the incoming PUBLISH message to process
         * in test case. */
        memcpy( &incomingInfo, pPublishInfo, sizeof( MQTTPublishInfo_t ) );
        incomingInfo.pTopicName = NULL;
        incomingInfo.pPayload = NULL;
        /* Allocate buffers and copy information of topic name and payload. */
        incomingInfo.pTopicName = malloc( pPublishInfo->topicNameLength );
        TEST_ASSERT_NOT_NULL( incomingInfo.pTopicName );
        memcpy( ( void * ) incomingInfo.pTopicName, pPublishInfo->pTopicName, pPublishInfo->topicNameLength );
        incomingInfo.pPayload = malloc( pPublishInfo->payloadLength );
        TEST_ASSERT_NOT_NULL( incomingInfo.pPayload );
        memcpy( ( void * ) incomingInfo.pPayload, pPublishInfo->pPayload, pPublishInfo->payloadLength );
    }
    else
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

            case MQTT_PACKET_TYPE_PINGRESP:

                /* Nothing to be done from application as library handles
                 * PINGRESP. */
                LogDebug( ( "Received PINGRESP" ) );
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
                LogDebug( ( "Unexpected PUBCOMP received: PacketID=%u",
                            packetIdentifier ) );
                break;

            /* Any other packet type is invalid. */
            default:
                LogError( ( "Unknown packet type received:(%02x).",
                            pPacketInfo->type ) );
        }
    }
}

static MQTTStatus_t subscribeToTopic( MQTTContext_t * pContext,
                                      const char * pTopic,
                                      MQTTQoS_t qos )
{
    MQTTSubscribeInfo_t pSubscriptionList[ 1 ];

    assert( pContext != NULL );

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
                                          const char * pTopic,
                                          MQTTQoS_t qos )
{
    MQTTSubscribeInfo_t pSubscriptionList[ 1 ];

    assert( pContext != NULL );

    /* Start with everything at 0. */
    ( void ) memset( ( void * ) pSubscriptionList, 0x00, sizeof( pSubscriptionList ) );

    pSubscriptionList[ 0 ].qos = qos;
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
                                    MQTTQoS_t qos )
{
    assert( pContext != NULL );
    MQTTPublishInfo_t publishInfo;

    /* Set the retain flag to false to avoid side-effects across test runs. */
    publishInfo.retain = false;

    publishInfo.qos = qos;
    publishInfo.dup = false;
    publishInfo.pTopicName = pTopic;
    publishInfo.topicNameLength = strlen( pTopic );
    publishInfo.pPayload = MQTT_EXAMPLE_MESSAGE;
    publishInfo.payloadLength = strlen( MQTT_EXAMPLE_MESSAGE );

    /* Get a new packet id. */
    globalPublishPacketIdentifier = MQTT_GetPacketId( pContext );

    /* Send PUBLISH packet. */
    return MQTT_Publish( pContext,
                         &publishInfo,
                         globalPublishPacketIdentifier );
}

/* ============================   UNITY FIXTURES ============================ */

/* Called before each test method. */
void setUp()
{
    /* Reset file-scoped global variables. */
    receivedSubAck = false;
    receivedUnsubAck = false;
    receivedPubAck = false;
    receivedPubRec = false;
    receivedPubRel = false;
    receivedPubComp = false;
    persistentSession = false;
    memset( &incomingInfo, 0u, sizeof( MQTTPublishInfo_t ) );

    /* Establish a TCP connection with the server endpoint. */
    TEST_ASSERT_EQUAL( EXIT_SUCCESS,
                       tcpConnectToServer( BROKER_ENDPOINT, BROKER_PORT, &tcpSocket ) );
    TEST_ASSERT_NOT_EQUAL( -1, tcpSocket );

    /* Establish TLS connection on top of TCP connection. */
    TEST_ASSERT_EQUAL( EXIT_SUCCESS, tlsSetup( tcpSocket, SERVER_ROOT_CA_CERT_PATH, &pSslContext ) );
    TEST_ASSERT_NOT_NULL( pSslContext );

    /* Establish MQTT session on top of the TCP+TLS connection. */
    establishMqttSession( &context, pSslContext, true, &persistentSession );
}

/* Called after each test method. */
void tearDown()
{
    /* Free memory, if allocated during test case execution. */
    if( incomingInfo.pTopicName != NULL )
    {
        free( ( void * ) incomingInfo.pTopicName );
    }

    if( incomingInfo.pPayload != NULL )
    {
        free( ( void * ) incomingInfo.pPayload );
    }

    /* Terminate MQTT connection. */
    TEST_ASSERT_EQUAL( MQTTSuccess, MQTT_Disconnect( &context ) );

    /* Terminate TLS session. */
    ( void ) closeTlsSession( pSslContext );

    /* Terminate TCP connection. */
    ( void ) closeTcpConnection( tcpSocket );
}

/* ========================== Test Cases ============================ */

/**
 * @brief Tests Subscribe and Publish operations with the MQTT broken using QoS 0.
 * The test subscribes to a topic, and then publishes to the same topic. The
 * broker is expected to route the publish message back to the test.
 */
void test_MQTT_Subscribe_Publish_With_Qos_0( void )
{
    /* Subscribe to a topic with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS0 ) );

    /* We expect a SUBACK from the broker for the subscribe operation. */
    TEST_ASSERT_EQUAL( MQTTSuccess,
                       MQTT_ProcessLoop( &context, MQTT_PROCESS_LOOP_TIMEOUT_MS ) );
    TEST_ASSERT_TRUE( receivedSubAck );

    /* Publish to the same topic, that we subscribed to, with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS0 ) );

    /* Call the MQTT library for the expectation to read an incoming PUBLISH for
     * the same message that we published (as we have subscribed to the same topic). */
    TEST_ASSERT_EQUAL( MQTTSuccess,
                       MQTT_ProcessLoop( &context, MQTT_PROCESS_LOOP_TIMEOUT_MS ) );
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
    TEST_ASSERT_EQUAL( MQTTSuccess, unsubscribeFromTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS0 ) );

    /* We expect an UNSUBACK from the broker for the unsubscribe operation. */
    TEST_ASSERT_EQUAL( MQTTSuccess,
                       MQTT_ProcessLoop( &context, MQTT_PROCESS_LOOP_TIMEOUT_MS ) );
    TEST_ASSERT_TRUE( receivedUnsubAck );
}

/**
 * @brief Tests Subscribe and Publish operations with the MQTT broken using QoS 1.
 * The test subscribes to a topic, and then publishes to the same topic. The
 * broker is expected to route the publish message back to the test.
 */
void test_MQTT_Subscribe_Publish_With_Qos_1( void )
{
    /* Subscribe to a topic with Qos 1. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS1 ) );

    /* Expect a SUBACK from the broker for the subscribe operation. */
    TEST_ASSERT_EQUAL( MQTTSuccess,
                       MQTT_ProcessLoop( &context, MQTT_PROCESS_LOOP_TIMEOUT_MS ) );
    TEST_ASSERT_TRUE( receivedSubAck );

    /* Publish to the same topic, that we subscribed to, with Qos 1. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS1 ) );

    /* Make sure that the MQTT context state was updated after the PUBLISH request. */
    TEST_ASSERT_EQUAL( MQTTQoS1, context.outgoingPublishRecords[ 0 ].qos );
    TEST_ASSERT_EQUAL( globalPublishPacketIdentifier, context.outgoingPublishRecords[ 0 ].packetId );
    TEST_ASSERT_EQUAL( MQTTPubAckPending, context.outgoingPublishRecords[ 0 ].publishState );

    /* Expect a PUBACK response for the PUBLISH and an incoming PUBLISH for the
     * same message that we published (as we have subscribed to the same topic). */
    TEST_ASSERT_EQUAL( MQTTSuccess,
                       MQTT_ProcessLoop( &context, MQTT_PROCESS_LOOP_TIMEOUT_MS ) );
    /* Make sure we have received PUBACK response. */
    TEST_ASSERT_TRUE( receivedPubAck );

    /* Make sure that we have received the same message from the server,
     * that was published (as we have subscribed to the same topic). */
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
    TEST_ASSERT_EQUAL( MQTTSuccess, unsubscribeFromTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS1 ) );

    /* Expect an UNSUBACK from the broker for the unsubscribe operation. */
    TEST_ASSERT_EQUAL( MQTTSuccess,
                       MQTT_ProcessLoop( &context, MQTT_PROCESS_LOOP_TIMEOUT_MS ) );
    TEST_ASSERT_TRUE( receivedUnsubAck );
}

/**
 * @brief Tests Subscribe and Publish operations with the MQTT broken using QoS 2.
 * The test subscribes to a topic, and then publishes to the same topic. The
 * broker is expected to route the publish message back to the test.
 */
void test_MQTT_Subscribe_Publish_With_Qos_2( void )
{
    /* Subscribe to a topic with Qos 2. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS2 ) );

    /* Expect a SUBACK from the broker for the subscribe operation. */
    TEST_ASSERT_EQUAL( MQTTSuccess,
                       MQTT_ProcessLoop( &context, MQTT_PROCESS_LOOP_TIMEOUT_MS ) );
    TEST_ASSERT_TRUE( receivedSubAck );

    /* Publish to the same topic, that we subscribed to, with Qos 2. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS2 ) );

    /* Make sure that the MQTT context state was updated after the PUBLISH request. */
    TEST_ASSERT_EQUAL( MQTTQoS2, context.outgoingPublishRecords[ 0 ].qos );
    TEST_ASSERT_EQUAL( globalPublishPacketIdentifier, context.outgoingPublishRecords[ 0 ].packetId );
    TEST_ASSERT_EQUAL( MQTTPubRecPending, context.outgoingPublishRecords[ 0 ].publishState );

    /* We expect PUBREC and PUBCOMP responses for the PUBLISH request, and
     * incoming PUBLISH with the same message that we published (as we are subscribed
     * to the same topic). Also, we expect a PUBREL ack response from the server for
     * the incoming PUBLISH (as we subscribed and publish with QoS 2). */
    TEST_ASSERT_EQUAL( MQTTSuccess,
                       MQTT_ProcessLoop( &context, MQTT_PROCESS_LOOP_TIMEOUT_MS ) );
    TEST_ASSERT_FALSE( receivedPubAck );
    TEST_ASSERT_TRUE( receivedPubRec );
    TEST_ASSERT_TRUE( receivedPubComp );
    TEST_ASSERT_TRUE( receivedPubRel );

    /* Make sure that we have received the same message from the server,
     * that was published (as we have subscribed to the same topic). */
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
    TEST_ASSERT_EQUAL( MQTTSuccess, unsubscribeFromTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS2 ) );

    /* Expect an UNSUBACK from the broker for the unsubscribe operation. */
    TEST_ASSERT_EQUAL( MQTTSuccess,
                       MQTT_ProcessLoop( &context, MQTT_PROCESS_LOOP_TIMEOUT_MS ) );
    TEST_ASSERT_TRUE( receivedUnsubAck );
}
