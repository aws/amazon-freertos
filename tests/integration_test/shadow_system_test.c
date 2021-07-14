/*
 * FreeRTOS V202107.00
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
 * @file shadow_system_test.c
 * @brief Integration tests for the SHADOW library via MQTT Library
 * for the FreeRTOS platform over Secure Sockets as the transport layer.
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

/* Include paths for public enums, structures, and macros. */
#include "core_mqtt.h"
#include "core_mqtt_state.h"

/* SHADOW API header. */
#include "shadow.h"

/* Include OpenSSL implementation of transport interface. */
#include "transport_secure_sockets.h"

/* Include task.h for delay function. */
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
 * the logging configuration for DEMO.
 * 3. Include the header file "logging_stack.h", if logging is enabled for DEMO.
 */

#include "logging_levels.h"

/* Logging configuration for the Tests. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    "SHADOW_TEST"
#endif

#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif
#include "logging_stack.h"

/* Default values for configs. */

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
#endif

/**
 * @brief Client identifier for MQTT session in the tests.
 */
#ifndef TEST_CLIENT_IDENTIFIER
    #define TEST_CLIENT_IDENTIFIER    clientcredentialIOT_THING_NAME
#endif

/**
 * @brief Size of the network buffer for MQTT packets.
 */
#define NETWORK_BUFFER_SIZE                 ( 1024U )

/**
 * @brief Length of the client identifier.
 */
#define TEST_CLIENT_IDENTIFIER_LENGTH       ( sizeof( TEST_CLIENT_IDENTIFIER ) - 1U )

/**
 * @brief The maximum number of retries for network operation with server.
 */
#define RETRY_MAX_ATTEMPTS                  ( 5U )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying failed network
 * operations with server.
 */
#define RETRY_MAX_BACKOFF_DELAY_MS          ( 5000U )

/**
 * @brief The base back-off delay (in milliseconds) to use for network operation retry
 * attempts.
 */
#define RETRY_BACKOFF_BASE_MS               ( 500U )

/**
 * @brief Transport timeout in milliseconds for transport send and receive.
 */
#define TRANSPORT_SEND_RECV_TIMEOUT_MS      ( 2000U )

/**
 * @brief Timeout for receiving CONNACK packet in milliseconds.
 */
#define CONNACK_RECV_TIMEOUT_MS             ( 1000U )

/**
 * @brief Time interval in seconds at which an MQTT PINGREQ need to be sent to
 * broker.
 */
#define MQTT_KEEP_ALIVE_INTERVAL_SECONDS    ( 5U )

/**
 * @brief Timeout for MQTT_ProcessLoop() function in milliseconds.
 * The timeout value is appropriately chosen for receiving an incoming
 * PUBLISH message and ack responses for QoS 1 and QoS 2 communications
 * with the broker.
 */
#define MQTT_PROCESS_LOOP_TIMEOUT_MS        ( 700U )

/**
 * @brief Milliseconds per second.
 */
#define MILLISECONDS_PER_SECOND             ( 1000U )

/**
 * @brief Milliseconds per FreeRTOS tick.
 */
#define MILLISECONDS_PER_TICK               ( MILLISECONDS_PER_SECOND / configTICK_RATE_HZ )

/**
 * @brief The exampled predefine thing name.
 *
 * This is the example predefine thing name and could be compiled in ROM code.
 */
#ifndef THING_NAME
    #define THING_NAME    clientcredentialIOT_THING_NAME
#endif

/**
 * @brief The size of #THING_NAME.
 */
#define THING_NAME_LENGTH             ( ( uint16_t ) ( sizeof( THING_NAME ) - 1 ) )

/**
 * @brief The Shadow document used for update topic desired tests.
 */
#define TEST_SHADOW_DESIRED           "{\"state\": {\"desired\": {\"key\": true}}, \"clientToken\":\"shadowSystemTest\"}"

/**
 * @brief The length of #TEST_SHADOW_DESIRED.
 */
#define TEST_SHADOW_DESIRED_LENGTH    ( sizeof( TEST_SHADOW_DESIRED ) - 1 )

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
 * @brief Global variable used by the pseudo random number generator.
 * The random number generator is used for calculating exponential back-off
 * with jitter for retry attempts of failed network operations with the broker.
 */
static uint32_t nextRand;

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
 * @brief Represents the OpenSSL context used for TLS session with the broker
 * for tests.
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
 * @brief Flag to represent result from a /update/accepted received from the broker.
 */
static bool receivedUpdateAcceptedResult = false;

/**
 * @brief Flag to represent result from a /update/rejected received from the broker.
 */
static bool receivedUpdateRejectedResult = false;

/**
 * @brief Flag to represent result from a /update/delta sending from the broker.
 */
static bool receivedUpdateDeltaResult = false;

/**
 * @brief Flag to represent result from a /update/documents sending from the broker.
 */
static bool receivedUpdateDocumentsResult = false;

/**
 * @brief Flag to represent result from a /delete/accepted sending from the broker.
 */
static bool receivedDeleteAcceptedResult = false;

/**
 * @brief Flag to represent result from a /delete/rejected sending from the broker.
 */
static bool receivedDeleteRejectedResult = false;

/**
 * @brief Flag to represent result from a /get/accepted is received from the broker.
 */
static bool receivedGetAcceptedResult = false;

/**
 * @brief Flag to represent result from a /get/rejected  is received from the broker.
 */
static bool receivedGetRejectedResult = false;

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
 * @brief Sends an MQTT CONNECT packet over the already connected TCP socket.
 *
 * @param[in] pContext MQTT context pointer.
 * @param[in] pNetworkContext Network context for OpenSSL transport implementation.
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

/*-----------------------------------------------------------*/

static void establishMqttSession( MQTTContext_t * pContext,
                                  NetworkContext_t * pNetworkContext,
                                  bool createCleanSession,
                                  bool * pSessionPresent )
{
    MQTTConnectInfo_t connectInfo;
    TransportInterface_t transport;
    MQTTFixedBuffer_t networkBuffer;

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

    connectInfo.clientIdentifierLength = TEST_CLIENT_IDENTIFIER_LENGTH;
    connectInfo.pClientIdentifier = TEST_CLIENT_IDENTIFIER;

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

    /* Send MQTT CONNECT packet to broker. */
    TEST_ASSERT_EQUAL( MQTTSuccess, MQTT_Connect( pContext,
                                                  &connectInfo,
                                                  NULL,
                                                  CONNACK_RECV_TIMEOUT_MS,
                                                  pSessionPresent ) );
}

/*-----------------------------------------------------------*/

static void eventCallback( MQTTContext_t * pContext,
                           MQTTPacketInfo_t * pPacketInfo,
                           MQTTDeserializedInfo_t * pDeserializedInfo )
{
    ShadowMessageType_t messageType = ShadowMessageTypeMaxNum;
    const char * pThingName = NULL;
    uint16_t thingNameLength = 0U;
    uint16_t packetIdentifier;

    ( void ) pContext;

    assert( pDeserializedInfo != NULL );
    assert( pContext != NULL );
    assert( pPacketInfo != NULL );

    packetIdentifier = pDeserializedInfo->packetIdentifier;

    /* Handle incoming publish. The lower 4 bits of the publish packet
     * type is used for the dup, QoS, and retain flags. Hence masking
     * out the lower bits to check if the packet is publish. */
    if( ( pPacketInfo->type & 0xF0U ) == MQTT_PACKET_TYPE_PUBLISH )
    {
        assert( pDeserializedInfo->pPublishInfo != NULL );

        /* Handle incoming publish. */
        /* Let the Device Shadow library tell us whether this is a device shadow message. */
        if( SHADOW_SUCCESS == Shadow_MatchTopic( pDeserializedInfo->pPublishInfo->pTopicName,
                                                 pDeserializedInfo->pPublishInfo->topicNameLength,
                                                 &messageType,
                                                 &pThingName,
                                                 &thingNameLength ) )
        {
            /* Upon successful return, the messageType has been filled in. */
            if( messageType == ShadowMessageTypeUpdateDelta )
            {
                receivedUpdateDeltaResult = true;
                LogInfo( ( "/update/delta payload:%s.\n\n", ( const char * ) pDeserializedInfo->pPublishInfo->pPayload ) );
            }
            else if( ( messageType == ShadowMessageTypeUpdateDocuments ) )
            {
                receivedUpdateDocumentsResult = true;
                LogInfo( ( "/update/documents json payload:%s.\n\n", ( const char * ) pDeserializedInfo->pPublishInfo->pPayload ) );
            }
            else if( ( messageType == ShadowMessageTypeUpdateAccepted ) )
            {
                receivedUpdateAcceptedResult = true;
                LogInfo( ( "/update/accepted json payload:%s.\n\n", ( const char * ) pDeserializedInfo->pPublishInfo->pPayload ) );
            }
            else if( ( messageType == ShadowMessageTypeUpdateRejected ) )
            {
                receivedUpdateRejectedResult = true;
                LogInfo( ( "/update/rejected json payload:%s.\n\n", ( const char * ) pDeserializedInfo->pPublishInfo->pPayload ) );
            }
            else if( ( messageType == ShadowMessageTypeGetAccepted ) )
            {
                receivedGetAcceptedResult = true;
                LogInfo( ( "/get/accepted json payload:%s.\n\n", ( const char * ) pDeserializedInfo->pPublishInfo->pPayload ) );
            }
            else if( ( messageType == ShadowMessageTypeGetRejected ) )
            {
                receivedGetRejectedResult = true;
                LogInfo( ( "/get/rejected json payload:%s.\n\n", ( const char * ) pDeserializedInfo->pPublishInfo->pPayload ) );
            }
            else if( ( messageType == ShadowMessageTypeDeleteAccepted ) )
            {
                receivedDeleteAcceptedResult = true;
                LogInfo( ( "/delete/accepted json payload:%s.\n\n", ( const char * ) pDeserializedInfo->pPublishInfo->pPayload ) );
            }
            else if( ( messageType == ShadowMessageTypeDeleteRejected ) )
            {
                receivedDeleteRejectedResult = true;
                LogInfo( ( "/delete/rejected json payload:%s.\n\n", ( const char * ) pDeserializedInfo->pPublishInfo->pPayload ) );
            }
            else
            {
                LogInfo( ( "other message type:%d !!\n\n", messageType ) );
            }
        }
        else
        {
            LogError( ( "Shadow_ValidateTopic parse failed:%s !!\n\n", ( const char * ) pDeserializedInfo->pPublishInfo->pTopicName ) );
        }
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
}

/*-----------------------------------------------------------*/

static MQTTStatus_t subscribeToTopic( MQTTContext_t * pContext,
                                      const char * pTopic,
                                      uint16_t topicLength,
                                      MQTTQoS_t qos )
{
    MQTTStatus_t mqttStatus = MQTTSuccess;
    MQTTSubscribeInfo_t pSubscriptionList[ 1 ];

    assert( pContext != NULL );
    assert( pTopic != NULL );
    assert( topicLength != 0 );

    /* Start with everything at 0. */
    ( void ) memset( ( void * ) pSubscriptionList, 0x00, sizeof( pSubscriptionList ) );

    /* topic /update/accepted and uses qos. */
    pSubscriptionList[ 0 ].qos = qos;
    pSubscriptionList[ 0 ].pTopicFilter = pTopic;
    pSubscriptionList[ 0 ].topicFilterLength = topicLength;

    /* Generate packet identifier for the SUBSCRIBE packet. */
    globalSubscribePacketIdentifier = MQTT_GetPacketId( pContext );

    /* Send SUBSCRIBE packet. */
    mqttStatus = MQTT_Subscribe( pContext,
                                 pSubscriptionList,
                                 sizeof( pSubscriptionList ) / sizeof( MQTTSubscribeInfo_t ),
                                 globalSubscribePacketIdentifier );

    if( mqttStatus == MQTTSuccess )
    {
        mqttStatus = MQTT_ProcessLoop( pContext, MQTT_PROCESS_LOOP_TIMEOUT_MS );
    }

    if( mqttStatus == MQTTSuccess )
    {
        TEST_ASSERT_TRUE( receivedSubAck );
        receivedSubAck = false;
    }

    return mqttStatus;
}

/*-----------------------------------------------------------*/

static MQTTStatus_t unsubscribeFromTopic( MQTTContext_t * pContext,
                                          const char * pTopic,
                                          uint16_t topicLength,
                                          MQTTQoS_t qos )
{
    MQTTStatus_t mqttStatus = MQTTSuccess;
    MQTTSubscribeInfo_t pSubscriptionList[ 1 ];

    assert( pContext != NULL );
    assert( pTopic != NULL );
    assert( topicLength != 0 );

    /* Start with everything at 0. */
    ( void ) memset( ( void * ) pSubscriptionList, 0x00, sizeof( pSubscriptionList ) );

    /* topic /update/accepted and uses qos. */
    pSubscriptionList[ 0 ].qos = qos;
    pSubscriptionList[ 0 ].pTopicFilter = pTopic;
    pSubscriptionList[ 0 ].topicFilterLength = topicLength;

    /* Generate packet identifier for the UNSUBSCRIBE packet. */
    globalUnsubscribePacketIdentifier = MQTT_GetPacketId( pContext );

    /* Send UNSUBSCRIBE packet. */
    mqttStatus = MQTT_Unsubscribe( pContext,
                                   pSubscriptionList,
                                   sizeof( pSubscriptionList ) / sizeof( MQTTSubscribeInfo_t ),
                                   globalUnsubscribePacketIdentifier );

    if( mqttStatus == MQTTSuccess )
    {
        mqttStatus = MQTT_ProcessLoop( pContext, MQTT_PROCESS_LOOP_TIMEOUT_MS );
    }

    if( mqttStatus == MQTTSuccess )
    {
        TEST_ASSERT_TRUE( receivedUnsubAck );
    }

    return mqttStatus;
}

/*-----------------------------------------------------------*/

static MQTTStatus_t publishToTopic( MQTTContext_t * pContext,
                                    const char * pTopic,
                                    const char * pPayload,
                                    MQTTQoS_t qos )
{
    MQTTStatus_t mqttStatus = MQTTSuccess;
    MQTTPublishInfo_t publishInfo;

    assert( pContext != NULL );

    /* Set the retain flag to false to avoid side-effects across test runs. */
    publishInfo.retain = false;

    publishInfo.qos = qos;
    publishInfo.dup = false;
    publishInfo.pTopicName = pTopic;
    publishInfo.topicNameLength = strlen( pTopic );
    publishInfo.pPayload = pPayload;
    publishInfo.payloadLength = strlen( pPayload );

    /* Get a new packet id. */
    globalPublishPacketIdentifier = MQTT_GetPacketId( pContext );

    /* Send PUBLISH packet. */
    mqttStatus = MQTT_Publish( pContext,
                               &publishInfo,
                               globalPublishPacketIdentifier );

    if( mqttStatus == MQTTSuccess )
    {
        mqttStatus = MQTT_ProcessLoop( pContext, MQTT_PROCESS_LOOP_TIMEOUT_MS );
    }

    if( mqttStatus == MQTTSuccess )
    {
        /* We do not expect a PUBACK from the broker for the QoS 0 PUBLISH. */
        TEST_ASSERT_FALSE( receivedPubAck );
    }

    return mqttStatus;
}

/*-----------------------------------------------------------*/

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

/*-----------------------------------------------------------*/

static bool connectToServerWithBackoffRetries( NetworkContext_t * pNetworkContext )
{
    bool isSuccessful = false;
    BackoffAlgorithmStatus_t BackoffAlgStatus = BackoffAlgorithmSuccess;
    BackoffAlgorithmContext_t reconnectParams;
    TransportSocketStatus_t transportStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;
    uint16_t nextRetryBackOff = 0U;

    /* Initializer server information. */
    serverInfo.pHostName = BROKER_ENDPOINT;
    serverInfo.hostNameLength = strlen( BROKER_ENDPOINT );
    serverInfo.port = BROKER_PORT;

    /* Initialize SocketsConfig. */
    socketsConfig.enableTls = true;
    socketsConfig.pAlpnProtos = NULL;
    socketsConfig.maxFragmentLength = 0;
    socketsConfig.disableSni = false;
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

/*-----------------------------------------------------------*/

/* ============================   UNITY FIXTURES ============================ */

/**
 * @brief Test group for device Shadow system tests.
 */
TEST_GROUP( deviceShadow_Integration );

TEST_SETUP( deviceShadow_Integration )
{
    /* Reset file-scoped global variables. */
    receivedSubAck = false;
    receivedUnsubAck = false;
    receivedPubAck = false;
    receivedPubRec = false;
    receivedPubRel = false;
    receivedPubComp = false;
    persistentSession = false;
    receivedUpdateDeltaResult = false;
    receivedUpdateDocumentsResult = false;
    receivedUpdateAcceptedResult = false;
    receivedUpdateRejectedResult = false;
    receivedDeleteAcceptedResult = false;
    receivedDeleteRejectedResult = false;
    receivedGetAcceptedResult = false;
    receivedGetRejectedResult = false;
    networkContext.pParams = &secureSocketsTransportParams;

    /* Establish TLS over TCP connection with retry attempts on failures. */
    TEST_ASSERT_TRUE( connectToServerWithBackoffRetries( &networkContext ) );

    /* Establish MQTT session on top of the TCP+TLS connection. */
    establishMqttSession( &context, &networkContext, true, &persistentSession );
}

TEST_TEAR_DOWN( deviceShadow_Integration )
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

/* ========================== Test Cases ============================ */



/**
 * @brief Test group runner for MQTT system tests that can be run against AWS IoT.
 */
TEST_GROUP_RUNNER( deviceShadow_Integration )
{
    RUN_TEST_CASE( deviceShadow_Integration, test_Shadow_System );
}

/**
 * @brief Subscribes the shadow topics: /update/delta, /update/documents,
 * /update/accepted, /delete/accepted, /get/accepted, then publish the
 * regarding payloads to verify if receiving the notification from the
 * subscribed topics.
 */
TEST( deviceShadow_Integration, test_Shadow_System )
{
    /* A buffer containing the update document. It has static duration to prevent
     * it from being placed on the stack. */
    static char emptyDocument[ 1 ] = { 0 };

    /* Subscribe to shadow topic /delete/accepted with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic( &context,
                                                      SHADOW_TOPIC_STRING_DELETE_ACCEPTED( THING_NAME ),
                                                      SHADOW_TOPIC_LENGTH_DELETE_ACCEPTED( THING_NAME_LENGTH ),
                                                      MQTTQoS0 ) );

    /* Subscribe to shadow topic /delete/rejected with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic( &context,
                                                      SHADOW_TOPIC_STRING_DELETE_REJECTED( THING_NAME ),
                                                      SHADOW_TOPIC_LENGTH_DELETE_REJECTED( THING_NAME_LENGTH ),
                                                      MQTTQoS0 ) );

    /* Subscribe to shadow topic /get/accepted with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic( &context,
                                                      SHADOW_TOPIC_STRING_GET_ACCEPTED( THING_NAME ),
                                                      SHADOW_TOPIC_LENGTH_GET_ACCEPTED( THING_NAME_LENGTH ),
                                                      MQTTQoS0 ) );

    /* Subscribe to shadow topic /get/rejected with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic( &context,
                                                      SHADOW_TOPIC_STRING_GET_REJECTED( THING_NAME ),
                                                      SHADOW_TOPIC_LENGTH_GET_REJECTED( THING_NAME_LENGTH ),
                                                      MQTTQoS0 ) );

    /* Subscribe to shadow topic /update/accepted with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic( &context,
                                                      SHADOW_TOPIC_STRING_UPDATE_ACCEPTED( THING_NAME ),
                                                      SHADOW_TOPIC_LENGTH_UPDATE_ACCEPTED( THING_NAME_LENGTH ),
                                                      MQTTQoS0 ) );

    /* Subscribe to shadow topic /update/rejected with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic( &context,
                                                      SHADOW_TOPIC_STRING_UPDATE_REJECTED( THING_NAME ),
                                                      SHADOW_TOPIC_LENGTH_UPDATE_REJECTED( THING_NAME_LENGTH ),
                                                      MQTTQoS0 ) );

    /* Subscribe to shadow topic /update/delta with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic( &context,
                                                      SHADOW_TOPIC_STRING_UPDATE_DELTA( THING_NAME ),
                                                      SHADOW_TOPIC_LENGTH_UPDATE_DELTA( THING_NAME_LENGTH ),
                                                      MQTTQoS0 ) );

    /* Subscribe to shadow topic /update/documents with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic( &context,
                                                      SHADOW_TOPIC_STRING_UPDATE_DOCUMENTS( THING_NAME ),
                                                      SHADOW_TOPIC_LENGTH_UPDATE_DOCUMENTS( THING_NAME_LENGTH ),
                                                      MQTTQoS0 ) );

    /* First of all, try to delete any Shadow document in the cloud.
     * This could trigger the /delete/accepted or /delete/rejected
     * based on the thing status on the cloud.
     */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic( &context,
                                                    SHADOW_TOPIC_STRING_DELETE( THING_NAME ),
                                                    emptyDocument,
                                                    MQTTQoS0 ) );

    /* Check the flag for /delete/accepted or /delete/rejected. */
    TEST_ASSERT_TRUE( ( receivedDeleteAcceptedResult || receivedDeleteRejectedResult ) );

    /* Publish to the shadow topic /update with reported payload,
     *  that we subscribed to, with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic( &context,
                                                    SHADOW_TOPIC_STRING_UPDATE( THING_NAME ),
                                                    TEST_SHADOW_DESIRED,
                                                    MQTTQoS0 ) );

    /* Check the flag for /update/documents*/
    TEST_ASSERT_TRUE( receivedUpdateDocumentsResult );

    /* Check the flag for /update/delta. */
    TEST_ASSERT_TRUE( receivedUpdateDeltaResult );

    /* Check the flag for /update/accepted and /update/rejected. */
    TEST_ASSERT_TRUE( ( receivedUpdateAcceptedResult || receivedUpdateRejectedResult ) );

    /* Finally, sending null payload on topic /get to trigger /get/accepted. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic( &context,
                                                    SHADOW_TOPIC_STRING_GET( THING_NAME ),
                                                    emptyDocument,
                                                    MQTTQoS0 ) );

    /* Check the flag for /get/accepted and /get/rejected. */
    TEST_ASSERT_TRUE( ( receivedGetAcceptedResult || receivedGetRejectedResult ) );

    /* Un-subscribe from a topic with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, unsubscribeFromTopic( &context,
                                                          SHADOW_TOPIC_STRING_UPDATE_DELTA( THING_NAME ),
                                                          SHADOW_TOPIC_LENGTH_UPDATE_DELTA( THING_NAME_LENGTH ),
                                                          MQTTQoS0 ) );
}
