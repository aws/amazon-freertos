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
 */

/**
 * @file iot_mqtt_ble_system_test.c
 * @brief Integration tests for the coreMQTT library for the FreeRTOS platform
 * over BLE transport interface.
 */

/* Standard header includes. */
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "FreeRTOS.h"

/* Include task.h for delay function. */
#include "task.h"

/* Include sempahore for callback synchronization. */
#include "semphr.h"

/* Unity Test framework include. */
#include "unity_fixture.h"

/* Include connection configurations header. */
#include "aws_clientcredential.h"

/* Include paths for public enums, structures, and macros. */
#include "core_mqtt.h"
#include "core_mqtt_state.h"

#include "iot_ble.h"

/* Include BLE implementation of transport interface. */
#include "iot_ble_mqtt_transport.h"



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

/* Logging configuration for the Demo. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    "CoreMQTTBLETest"
#endif

#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif
#include "logging_stack.h"

/**************Default Configurations values***********************/

#ifndef BROKER_ENDPOINT
    #define BROKER_ENDPOINT    clientcredentialMQTT_BROKER_ENDPOINT
#endif

#ifndef BROKER_PORT
    #define BROKER_PORT    clientcredentialMQTT_BROKER_PORT
#endif

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
#define MQTT_FIRST_VALID_PACKET_ID            ( 1 )

/**
 * @brief A PINGREQ packet is always 2 bytes in size, defined by MQTT 3.1.1 spec.
 */
#define MQTT_PACKET_PINGREQ_SIZE              ( 2U )

/**
 * @brief A packet type not handled by MQTT_ProcessLoop.
 */
#define MQTT_PACKET_TYPE_INVALID              ( 0U )

/**
 * @brief Number of milliseconds in a second.
 */
#define MQTT_ONE_SECOND_TO_MS                 ( 1000U )

/**
 * @brief Length of the MQTT network buffer.
 */
#define MQTT_TEST_BUFFER_LENGTH               ( 128 )

/**
 * @brief Sample length of remaining serialized data.
 */
#define MQTT_SAMPLE_REMAINING_LENGTH          ( 64 )

/**
 * @brief Subtract this value from max value of global entry time
 * for the timer overflow test.
 */
#define MQTT_OVERFLOW_OFFSET                  ( 3 )

/**
 * @brief Sample topic filter to subscribe to.
 */
#define TEST_MQTT_TOPIC                       TEST_CLIENT_IDENTIFIER "/iot/integration/test"

/**
 * @brief Length of sample topic filter.
 */
#define TEST_MQTT_TOPIC_LENGTH                ( sizeof( TEST_MQTT_TOPIC ) - 1 )

/**
 * @brief Size of the network buffer for MQTT packets.
 */
#define NETWORK_BUFFER_SIZE                   ( 1024U )

/**
 * @brief Length of the client identifier.
 */
#define TEST_CLIENT_IDENTIFIER_LENGTH         ( sizeof( TEST_CLIENT_IDENTIFIER ) - 1u )

/**
 * @brief Client identifier for use in LWT tests.
 */
#define TEST_CLIENT_IDENTIFIER_LWT            TEST_CLIENT_IDENTIFIER "-LWT"

/**
 * @brief Length of LWT client identifier.
 */
#define TEST_CLIENT_IDENTIFIER_LWT_LENGTH     ( sizeof( TEST_CLIENT_IDENTIFIER_LWT ) - 1u )

/**
 * @brief Timeout waiting for a BLE connection to be established.
 */
#define TEST_BLE_CONNECTION_TIMEOUT_MS        ( 10000U )

/**
 * @brief BLE transport buffer size.
 **/
#define TRANSPORT_BUFFER_SIZE                 ( 256U )

/**
 * @brief Timeout for receiving CONNACK packet in milli seconds.
 */
#define CONNACK_RECV_TIMEOUT_MS               ( 5000U )

/**
 * @brief Time interval in seconds at which an MQTT PINGREQ need to be sent to
 * broker.
 */
#define MQTT_KEEP_ALIVE_INTERVAL_SECONDS      ( 5U )

/**
 * @brief The number of milliseconds to wait for AWS IoT Core Message Broker
 * to resend a PUBLISH that has not been acknowledged.
 */
#define AWS_IOT_CORE_REPUBLISH_INTERVAL_MS    ( 30000U )

/**
 * @brief Timeout for MQTT_ProcessLoop() function in milliseconds.
 * The timeout value is appropriately chosen for receiving an incoming
 * PUBLISH message and ack responses for QoS 1 and QoS 2 communications
 * with the broker.
 */
#define MQTT_PROCESS_LOOP_TIMEOUT_MS          ( 5000U )

/**
 * @brief The MQTT message published in this example.
 */
#define MQTT_EXAMPLE_MESSAGE                  "Hello World!"

/**
 * @brief Milliseconds per second.
 */
#define MILLISECONDS_PER_SECOND               ( 1000U )

/**
 * @brief Milliseconds per FreeRTOS tick.
 */
#define MILLISECONDS_PER_TICK                 ( MILLISECONDS_PER_SECOND / configTICK_RATE_HZ )

/*-----------------------------------------------------------*/

/**
 * @brief Each compilation unit that consumes the NetworkContext must define it.
 * It should contain a single pointer to the type of your desired transport.
 * When using multiple transports in the same compilation unit, define this pointer as void *.
 *
 * @note Transport stacks are defined in amazon-freertos/libraries/ble/include/iot_ble_mqtt_transport.h.
 */
struct NetworkContext
{
    BleTransportParams_t * pParams;
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
 * @brief Represents the BLE transport interface context used to send data over BLE channel.
 */
static NetworkContext_t networkContext = { 0 };

/**
 * @brief Ble Transport Parameters structure to store the data channel.
 */
static BleTransportParams_t xBleTransportParams = { 0 };

/**
 * @brief The context representing the MQTT connection with the broker for
 * the test case.
 */
static MQTTContext_t context;

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
 * @brief Represents incoming PUBLISH information.
 */
static MQTTPublishInfo_t incomingInfo;

/**
 * @brief Buffer to store incoming publish topic name for assertions in tests.
 */
static uint8_t incomingTopicBuffer[ TEST_MQTT_TOPIC_LENGTH ];

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
 * @brief Global entry time to use as a reference timestamp in the getTimeMs()
 * function. getTimeMs() function will always return the difference between the
 * current time and the global entry time. This reduces the chances of overflow
 * for the 32 bit unsigned integer used for holding the timestamp.
 */
static uint32_t globalEntryTime;

/**
 * @brief Flag to represent if BLE is connected.
 */
static bool isBleConnected;

/**
 * @brief Semaphore used to synchronize with BLE connection callback.
 */
static SemaphoreHandle_t bleConnectionSem;

/**
 * @brief Semaphore used to synchronize with BLE channel callback.
 */
static SemaphoreHandle_t bleChannelSem;

/**
 * @brief The timer query function provided to the MQTT context.
 *
 * @return Time in milliseconds.
 */
static uint32_t getTimeMs();

/**
 * @brief Global variable to keep track of BLE status and enable only once.
 */
static BaseType_t bleEnabled = pdFALSE;

/**
 * @brief Sends an MQTT CONNECT packet over connected BLE Channel.
 *
 * @param[in] pContext MQTT context pointer.
 * @param[in] pNetworkContext Network context for OpenSSL transport implementation.
 * @param[in] createCleanSession Creates a new MQTT session if true.
 * If false, tries to establish the existing session if there was session
 * already present in broker.
 */
static void establishMqttSession( MQTTContext_t * pContext,
                                  NetworkContext_t * pNetworkContext,
                                  bool createCleanSession );

/**
 * @brief Handler for incoming acknowledgement packets from the broker.
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

/*
 * @brief Callback invoked when physical BLE connection is established.
 * @param[in] status Status of the BlE callback
 * @param[in] connId Connection handle identifier
 * @param[in] connected BLE is connected or disconnected.
 * @param[in] pBa Bluetooth address of the peer.
 */
static void bleConnectionCallback( BTStatus_t status,
                                   uint16_t connId,
                                   bool connected,
                                   BTBdaddr_t * pBa );

/**
 * @brief Enables BLE and starts Bluetooth advertisement.
 * Waits for the BLE central to be connected.
 */
static void bleEnable( void );

/**
 * @brief Disconnects with BLE central, turns off advertisment and disable BLE.
 */
static void bleDisable( void );

/**
 * @brief Sets up transport interface used for sending and receiving MQTT data over BLE.
 * @param[in] pContext BLE transport interface context, used by the MQTT library.
 */
static void setupBleTransportInterface( NetworkContext_t * pContext );

/*
 * @brief Cleans up transport interface used for sending receiving MQTT data.
 * @param[in] pContext BLE transport interface context, used by the MQTT library.
 */
static void teardownBleTransportInterface( NetworkContext_t * pContext );


/**
 * @brief The function for initializing the BLE stack.
 * The function implementation is provided by the vendor.
 * @return Success or Failure if stack initialization was successfull or not.
 */
extern BTStatus_t bleStackInit( void );


/*-----------------------------------------------------------*/

static void _BLEConnectionCallback( BTStatus_t status,
                                    uint16_t connId,
                                    bool connected,
                                    BTBdaddr_t * pBa )
{
    if( connected == true )
    {
        IotBle_StopAdv( NULL );
    }
    else
    {
        ( void ) IotBle_StartAdv( NULL );
    }

    isBleConnected = connected;
    ( void ) xSemaphoreGive( bleConnectionSem );
}


static void bleEnable( void )
{
    IotBleEventsCallbacks_t eventCb;
    BTStatus_t status = eBTStatusSuccess;
    static bool isBleStackInitialized = false;

    /** One time initialization of stack and Middleware **/
    if( isBleStackInitialized == false )
    {
        status = bleStackInit();

        TEST_ASSERT_EQUAL( eBTStatusSuccess, status );

        status = IotBle_Init();

        TEST_ASSERT_EQUAL( eBTStatusSuccess, status );

        isBleStackInitialized = true;
    }

    bleConnectionSem = xSemaphoreCreateBinary();

    TEST_ASSERT_NOT_NULL( bleConnectionSem );

    eventCb.pConnectionCb = _BLEConnectionCallback;

    status = IotBle_RegisterEventCb( eBLEConnection, eventCb );

    TEST_ASSERT_EQUAL( eBTStatusSuccess, status );

    status = IotBle_On();

    TEST_ASSERT_EQUAL_MESSAGE( eBTStatusSuccess, status, "Failed to turn on BLE" );

    TEST_ASSERT_EQUAL_MESSAGE( pdTRUE, xSemaphoreTake( bleConnectionSem, pdMS_TO_TICKS( TEST_BLE_CONNECTION_TIMEOUT_MS ) ), "Failed to establish BLE connection" );

    TEST_ASSERT_TRUE_MESSAGE( isBleConnected, "Failed to establish BLE connection" );
}

static void bleDisable( void )
{
    BTStatus_t status;
    IotBleEventsCallbacks_t eventCb;

    status = IotBle_Off();
    TEST_ASSERT_EQUAL_MESSAGE( eBTStatusSuccess, status, "Failed to turn off BLE" );

    TEST_ASSERT_EQUAL_MESSAGE( pdTRUE, xSemaphoreTake( bleConnectionSem, pdMS_TO_TICKS( TEST_BLE_CONNECTION_TIMEOUT_MS ) ), "Failed to disconnect BLE" );
    TEST_ASSERT_FALSE_MESSAGE( isBleConnected, "Failed to disconnect BLE" );

    eventCb.pConnectionCb = _BLEConnectionCallback;
    status = IotBle_UnRegisterEventCb( eBLEConnection, eventCb );
    TEST_ASSERT_EQUAL( eBTStatusSuccess, status );

    vSemaphoreDelete( bleConnectionSem );
}


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
                                  bool createCleanSession )
{
    MQTTConnectInfo_t connectInfo;
    TransportInterface_t transport;
    MQTTFixedBuffer_t networkBuffer;
    bool sessionPresent;

    assert( pContext != NULL );
    assert( pNetworkContext != NULL );

    /* The network buffer must remain valid for the lifetime of the MQTT context. */
    static uint8_t buffer[ NETWORK_BUFFER_SIZE ];

    /* Setup the transport interface object for the library. */
    transport.pNetworkContext = pNetworkContext;
    transport.send = IotBleMqttTransportSend;
    transport.recv = IotBleMqttTransportReceive;

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
                                                  &sessionPresent ) );
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
            TEST_FAIL_MESSAGE( "Unexpected PUBREC packet received." );
            break;

        case MQTT_PACKET_TYPE_PUBREL:
            TEST_FAIL_MESSAGE( "Unexpected PUBREL packet received." );
            break;

        case MQTT_PACKET_TYPE_PUBCOMP:
            TEST_FAIL_MESSAGE( "Unexpected PUBCOMP packet received." );
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
        /* Terminate BLE channel to test session restoration
         * across network connection. */
        ( void ) teardownBleTransportInterface( &networkContext );
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
                                    bool setRetainFlag,
                                    bool isDuplicate,
                                    MQTTQoS_t qos,
                                    uint16_t packetId )
{
    assert( pContext != NULL );
    MQTTPublishInfo_t publishInfo;

    publishInfo.retain = setRetainFlag;

    publishInfo.qos = qos;
    publishInfo.dup = isDuplicate;
    publishInfo.pTopicName = pTopic;
    publishInfo.topicNameLength = strlen( pTopic );
    publishInfo.pPayload = MQTT_EXAMPLE_MESSAGE;
    publishInfo.payloadLength = strlen( MQTT_EXAMPLE_MESSAGE );

    /* Get a new packet id. */
    globalPublishPacketIdentifier = packetId;

    /* Send PUBLISH packet. */
    return MQTT_Publish( pContext,
                         &publishInfo,
                         packetId );
}

static void bleChannelCallback( IotBleDataTransferChannelEvent_t event,
                                IotBleDataTransferChannel_t * pChannel,
                                void * context )
{
    MQTTStatus_t status;

    /* Unused parameters. */
    ( void ) pChannel;
    ( void ) context;

    /* Event to see when the data channel is ready to receive data. */
    if( event == IOT_BLE_DATA_TRANSFER_CHANNEL_OPENED )
    {
        ( void ) xSemaphoreGive( bleChannelSem );
    }

    /* Event for when data is received over the channel. */
    else if( event == IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVED )
    {
        status = IotBleMqttTransportAcceptData( &networkContext );
        TEST_ASSERT_EQUAL( MQTTSuccess, status );
    }
}

static void setupBleTransportInterface( NetworkContext_t * pContext )
{
    bool status;

    /* Static memory that will contain the incoming packet queue used by the transport code. */
    static uint8_t buffer[ TRANSPORT_BUFFER_SIZE ] = { 0 };

    status = IotBleMqttTransportInit( buffer, TRANSPORT_BUFFER_SIZE, pContext );
    TEST_ASSERT_TRUE_MESSAGE( status, "Failed to initialize transport interface for BLE" );

    /* Open is a handshake procedure, so we need to wait until it is ready to use. */
    pContext->pParams->pChannel = IotBleDataTransfer_Open( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT );
    TEST_ASSERT_NOT_NULL_MESSAGE( pContext->pParams->pChannel, "Failed to create BLE data transfer channel." );

    bleChannelSem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL( bleChannelSem );

    status = IotBleDataTransfer_SetCallback( pContext->pParams->pChannel, bleChannelCallback, NULL );
    TEST_ASSERT_TRUE( status );

    TEST_ASSERT_EQUAL_MESSAGE( pdTRUE, xSemaphoreTake( bleChannelSem, pdMS_TO_TICKS( TEST_BLE_CONNECTION_TIMEOUT_MS ) ), "Timed out to open BLE data transfer channel" );
}

static void teardownBleTransportInterface( NetworkContext_t * pContext )
{
    IotBleMqttTransportCleanup( pContext );
    IotBleDataTransfer_Close( pContext->pParams->pChannel );
    IotBleDataTransfer_Reset( pContext->pParams->pChannel );
    vSemaphoreDelete( bleChannelSem );
}


/* ============================   UNITY FIXTURES ============================ */

/* Called before each test method. */
static void testSetUp()
{
    /* Reset file-scoped global variables. */
    receivedSubAck = false;
    receivedUnsubAck = false;
    receivedPubAck = false;
    packetTypeForDisconnection = MQTT_PACKET_TYPE_INVALID;
    memset( &incomingInfo, 0u, sizeof( MQTTPublishInfo_t ) );

    networkContext.pParams = &xBleTransportParams;

    if( bleEnabled == pdFALSE )
    {
        bleEnable();
        bleEnabled = pdTRUE;
    }

    /* setup BLE transport interface. */
    setupBleTransportInterface( &networkContext );

    /* Establish MQTT session on top of BLE connection. */
    establishMqttSession( &context, &networkContext, true );
}

/* Called after each test method. */
static void testTearDown()
{
    /* Terminate MQTT connection. */
    TEST_ASSERT_EQUAL( MQTTSuccess, MQTT_Disconnect( &context ) );

    /* Teardown BLE transport interface */
    teardownBleTransportInterface( &networkContext );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group for running coreMQTT system tests with a broker
 * that supports all features of MQTT 3.1.1 specification.
 */
TEST_GROUP( coreMQTT_Integration_BLE );

TEST_SETUP( coreMQTT_Integration_BLE )
{
    testSetUp();
}

TEST_TEAR_DOWN( coreMQTT_Integration_BLE )
{
    testTearDown();
}

/* ========================== Test Cases ============================ */

/**
 * @brief Test group runner for MQTT system tests that run over BLE assisted by a companion device.
 * These tests are a subset of MQTT integration tests supported over BLE and by AWS IoT.
 */
TEST_GROUP_RUNNER( coreMQTT_Integration_BLE )
{
    RUN_TEST_CASE( coreMQTT_Integration_BLE, Subscribe_Publish_With_Qos_0 );
    RUN_TEST_CASE( coreMQTT_Integration_BLE, Subscribe_Publish_With_Qos_1 );
    RUN_TEST_CASE( coreMQTT_Integration_BLE, ProcessLoop_KeepAlive );

    /* Disconnect and turn off BLE after all tests in the group. */
    if( bleEnabled == pdTRUE )
    {
        bleDisable();
    }
}

/* ========================== Test Cases ============================ */

/**
 * @brief Tests Subscribe and Publish operations with the MQTT broker using QoS 0.
 * The test subscribes to a topic, and then publishes to the same topic. The
 * broker is expected to route the publish message back to the test.
 */
static void Subscribe_Publish_With_Qos_0()
{
    /* Subscribe to a topic with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS0 ) );

    /* We expect a SUBACK from the broker for the subscribe operation. */
    TEST_ASSERT_FALSE( receivedSubAck );
    TEST_ASSERT_EQUAL( MQTTSuccess,
                       MQTT_ProcessLoop( &context, MQTT_PROCESS_LOOP_TIMEOUT_MS ) );
    TEST_ASSERT_TRUE( receivedSubAck );

    /* Publish to the same topic, that we subscribed to, with Qos 0. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           &context,
                           TEST_MQTT_TOPIC,
                           false, /* setRetainFlag */
                           false, /* isDuplicate */
                           MQTTQoS0,
                           MQTT_GetPacketId( &context ) ) );

    /* Call the MQTT library for the expectation to read an incoming PUBLISH for
     * the same message that we published (as we have subscribed to the same topic). */
    TEST_ASSERT_FALSE( receivedPubAck );
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
    TEST_ASSERT_EQUAL( MQTTSuccess, unsubscribeFromTopic( &context, TEST_MQTT_TOPIC ) );

    /* We expect an UNSUBACK from the broker for the unsubscribe operation. */
    TEST_ASSERT_EQUAL( MQTTSuccess,
                       MQTT_ProcessLoop( &context, MQTT_PROCESS_LOOP_TIMEOUT_MS ) );
    TEST_ASSERT_TRUE( receivedUnsubAck );
}

TEST( coreMQTT_Integration_BLE, Subscribe_Publish_With_Qos_0 )
{
    Subscribe_Publish_With_Qos_0();
}

/**
 * @brief Tests Subscribe and Publish operations with the MQTT broken using QoS 1.
 * The test subscribes to a topic, and then publishes to the same topic. The
 * broker is expected to route the publish message back to the test.
 */
static void Subscribe_Publish_With_Qos_1()
{
    /* Subscribe to a topic with Qos 1. */
    TEST_ASSERT_EQUAL( MQTTSuccess, subscribeToTopic(
                           &context, TEST_MQTT_TOPIC, MQTTQoS1 ) );

    /* Expect a SUBACK from the broker for the subscribe operation. */
    TEST_ASSERT_FALSE( receivedSubAck );
    TEST_ASSERT_EQUAL( MQTTSuccess,
                       MQTT_ProcessLoop( &context, MQTT_PROCESS_LOOP_TIMEOUT_MS ) );
    TEST_ASSERT_TRUE( receivedSubAck );

    /* Publish to the same topic, that we subscribed to, with Qos 1. */
    TEST_ASSERT_EQUAL( MQTTSuccess, publishToTopic(
                           &context,
                           TEST_MQTT_TOPIC,
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
    TEST_ASSERT_EQUAL( MQTTSuccess, unsubscribeFromTopic( &context, TEST_MQTT_TOPIC ) );

    /* Expect an UNSUBACK from the broker for the unsubscribe operation. */
    TEST_ASSERT_EQUAL( MQTTSuccess,
                       MQTT_ProcessLoop( &context, MQTT_PROCESS_LOOP_TIMEOUT_MS ) );
    TEST_ASSERT_TRUE( receivedUnsubAck );
}

TEST( coreMQTT_Integration_BLE, Subscribe_Publish_With_Qos_1 )
{
    Subscribe_Publish_With_Qos_1();
}

/**
 * @brief Verifies that the MQTT library sends a Ping Request packet if the connection is
 * idle for more than the keep-alive period.
 */
static void ProcessLoop_KeepAlive()
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
TEST( coreMQTT_Integration_BLE, ProcessLoop_KeepAlive )
{
    ProcessLoop_KeepAlive();
}
