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


/* Standard includes. */
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes for task syncrhonization and delay. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Header to include the broker endpoint and Thing Name. */
#include "aws_clientcredential.h"

/* Defines configs used by BLE library and transport interface demo. */
#include "iot_ble_config.h"
#include "mqtt_ble_demo_config.h"

#include "core_mqtt.h"
#include "iot_ble_mqtt_transport.h"
#include "iot_ble_data_transfer.h"

/*
 * Demo for showing the use of MQTT API with transport interface implementation based on the BLE connectivity layer
 * to send MQTT packets to cloud assisted by a companion mobile device.
 *
 * The Example establishes a BLE data transfer channel over BLE GATT connection with a companion mobile
 * device and uses MQTT APIs to create and send packets over the channel to a broker.
 * This example is single threaded and uses statically allocated
 * memory. It uses QoS0 by default for sending to and receiving messages from the broker.
 *
 * A smartphone application using the FreeRTOS companion mobile SDK, in BLE central role should scan and
 * connect to this device successfully, before starting the demo.
 * Broker endpoint and the thing name should be configured in demos/include/aws_clientcredential.h.
 */


/**
 * @brief The first characters in the client identifier. A timestamp is appended
 * to this prefix to create a unique client identifer.
 *
 */
#define CLIENT_IDENTIFIER_PREFIX           "iotdemo"

/**
 * @brief The maximum length the demo will use for client identifier.
 */
#define CLIENT_IDENTIFIER_MAX_LENGTH       12U

/**
 * @brief The size of the buffer in bytes passed to the transport code to hold streaming data.
 */
#define SINGLE_INCOMING_PACKET_MAX_SIZE    128U

/**
 * @brief The size of the circular buffer used by the transport code to hold incoming packets.
 */
#define INCOMING_PACKET_QUEUE_SIZE         256U

/**
 * @brief The number of subscription/unsubscription requests sent per packet in this demo
 *        IMPORTANT: If you change thie value, update the static subscriptionArray variable with this number of topics.
 */
#define NUM_SUBS_AT_ONCE                   2U


/**
 * @brief The first topic to which the application subscribes to. The demo publishes the sample
 * payload to this topic and checks whether the same payload is received back.
 */
#define mqttexampleTOPIC1    clientcredentialIOT_THING_NAME "/example/topic1"

/**
 * @brief The second topic to which the application subscribes to. The demo publishes the sample
 * payload to this topic and checks whether the same payload is received back.
 */
#define mqttexampleTOPIC2    clientcredentialIOT_THING_NAME "/example/topic2"


/**
 * @brief The QoS used by the demo.  Can be MQTTQoS0 or MQTTQoS1.  Qos 2 is not currently supported.
 */
#define DEMO_QOS                             MQTTQoS0

/**
 * @brief The MQTT message published in this example.
 */
#define MQTT_EXAMPLE_MESSAGE                 "Hello MQTT World!"

/**
 * @brief Keep alive period in seconds for MQTT connection.
 */
#define MQTT_KEEP_ALIVE_PERIOD_SECONDS       5U

/**
 * @brief Milliseconds per second definition.
 */
#define MILLISECONDS_PER_SECOND              1000U

/**
 * @brief Number of time network receive will be attempted
 * if it fails due to transportTimeout.
 */
#define MQTT_MAX_RECV_ATTEMPTS               20U

/**
 * @brief Delay between two demo iterations.
 */
#define MQTT_DEMO_ITERATION_DELAY_SECONDS    5U

#define HIGH_BYTE_MASK                       0xF0U

/*-----------------------------------------------------------*/

/**
 * @brief Each compilation unit that consumes the NetworkContext must define it.
 * It should contain a single pointer to the type of your desired transport.
 * When using multiple transports in the same compilation unit, define this pointer as void *.
 *
 * @note Transport stacks are defined in amazon-freertos/libraries/c_sdk/ble/include/iot_ble_mqtt_transport.h.
 */
struct NetworkContext
{
    BleTransportParams_t * pParams;
};

/*-----------------------------------------------------------*/

/**
 * @brief Callback for the demo, handles events when the data channel is first opened
 * and when data is received by the channel.
 */
static void demoCallback( IotBleDataTransferChannelEvent_t event,
                          IotBleDataTransferChannel_t * pChannel,
                          void * context );

/**
 * @brief Initializes the demo, including calling the init function for the transport layer
 * and initializing the BLE channel.
 */
static MQTTStatus_t demoInitChannel( void );


/**
 * @brief Generate and return monotonically increasing packet identifier.
 *
 * @return The next PacketId.
 *
 * @note This function is not thread safe.
 */
static uint16_t getNextPacketIdentifier( void );


/*-----------------------------------------------------------*/

/**
 * @brief Array of topic filters to subscribe and publish to.
 * IMPORTANT: If you alter this array, change NUM_SUBS_AT_ONCE above to get expected behavior.
 */
static char * subscriptionArray[ NUM_SUBS_AT_ONCE ] = { mqttexampleTOPIC1, mqttexampleTOPIC2 };

/**
 * @brief Packet Identifier generated when Subscribe request was sent to the broker;
 * it is used to match received Subscribe ACK to the transmitted SUBSCRIBE request.
 */
static uint16_t subscribePacketIdentifier;

/**
 * @brief Packet Identifier generated when Unsubscribe request was sent to the broker
 * it is used to match received Unsubscribe response to the transmitted unsubscribe
 * request.
 */
static uint16_t unsubscribePacketIdentifier;

/**
 * @brief Network context structure to store the BleTransportParams_t pointer.
 */
static NetworkContext_t xContext;

/**
 * @brief Ble Transport Parameters structure to store the data channel.
 */
static BleTransportParams_t xBleTransportParams;

/**
 * @brief Flag to mark if the channel has been disconnected at all
 */
static volatile bool channelActive = false;

/**
 * @brief Buffer used by the transport interface to queue incoming packets for use.
 */
static uint8_t contextBuf[ INCOMING_PACKET_QUEUE_SIZE ];

/**
 * @brief Buffer used by the demo to store the previous incoming packet.
 */
static uint8_t fixedBufferBuf[ SINGLE_INCOMING_PACKET_MAX_SIZE ];

/**
 * @brief Semaphore used to synchronize with the data transfer channel callback.
 */
static SemaphoreHandle_t channelSemaphore;

static void mqttProcessIncomingPacket( MQTTFixedBuffer_t * buf );

/*-----------------------------------------------------------*/

static void demoCallback( IotBleDataTransferChannelEvent_t event,
                          IotBleDataTransferChannel_t * pChannel,
                          void * context )
{
    MQTTStatus_t acceptCode;

    /* Unused parameters. */
    ( void ) pChannel;
    ( void ) context;

    /* Event to see when the data channel is ready to receive data. */
    if( event == IOT_BLE_DATA_TRANSFER_CHANNEL_OPENED )
    {
        ( void ) xSemaphoreGive( channelSemaphore );
    }

    /* Event for when data is received over the channel. */
    else if( event == IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVED )
    {
        acceptCode = IotBleMqttTransportAcceptData( &xContext );
        configASSERT( acceptCode == MQTTSuccess );
    }

    /* Event for when channel is closed. */
    else if( event == IOT_BLE_DATA_TRANSFER_CHANNEL_CLOSED )
    {
        channelActive = false;
    }

    else
    {
        /* Empty else, MISRA 2012 15.7 */
    }
}

static MQTTStatus_t demoInitChannel( void )
{
    MQTTStatus_t status = MQTTSuccess;

    /* Memory that will contain the incoming packet queue used by the transport code.
     * Here we use static memory, but dynamic is OK too.
     * It is the responsibility of the user application to allocate this buffer.
     */
    IotBleMqttTransportInit( contextBuf, INCOMING_PACKET_QUEUE_SIZE, &xContext );

    /* Open is a handshake proceture, so we need to wait until it is ready to use. */
    xContext.pParams->pChannel = IotBleDataTransfer_Open( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT );

    if( xContext.pParams->pChannel != NULL )
    {
        channelSemaphore = xSemaphoreCreateBinary();

        if( channelSemaphore != NULL )
        {
            ( void ) IotBleDataTransfer_SetCallback( xContext.pParams->pChannel, demoCallback, NULL );

            if( xSemaphoreTake( channelSemaphore, pdMS_TO_TICKS( IOT_BLE_MQTT_CREATE_CONNECTION_WAIT_MS ) ) == pdTRUE )
            {
                LogInfo( ( "The channel was initialized successfully" ) );

                status = MQTTSuccess;
                channelActive = true;
            }
            else
            {
                LogError( ( "The BLE Connection timed out" ) );
                status = MQTTServerRefused;
            }
        }
        else
        {
            LogError( ( "Could not create network connection semaphore." ) );
            status = MQTTNoMemory;
        }
    }

    if( status != MQTTSuccess )
    {
        LogError( ( "Something went wrong in initializing the demo" ) );
    }

    return status;
}


static uint16_t getNextPacketIdentifier( void )
{
    static uint16_t packetId = 1;

    /* Since ID can never be 0, we traverse odd numbers */
    packetId += 2U;

    return packetId;
}

static MQTTStatus_t getNewData( const MQTTFixedBuffer_t * buf,
                                MQTTPacketInfo_t * incomingPacket )
{
    MQTTStatus_t result = MQTTSuccess;
    size_t receiveAttempts = 0;
    uint8_t bufferIndex = 0;
    uint32_t bytesReturned = 0;
    size_t bytesToRead = 0;

    /* Waits until there is data available from the channel to receive it.
     * A "No data was received from the transport." may appear from each unsuccessful attempt.
     * This program will stop after 20 attempts, with 250 milliseconds in between by default.
     * If you have high latency, you can adjust MQTT_MAX_RECV_ATTEMPTS above. */
    do
    {
        taskYIELD();
        result = MQTT_GetIncomingPacketTypeAndLength( IotBleMqttTransportReceive, &xContext, incomingPacket );
        receiveAttempts++;
    } while( ( receiveAttempts < MQTT_MAX_RECV_ATTEMPTS ) && ( result != MQTTSuccess ) );

    configASSERT( result == MQTTSuccess );
    configASSERT( incomingPacket->remainingLength < SINGLE_INCOMING_PACKET_MAX_SIZE );

    receiveAttempts = 0;
    bytesToRead = incomingPacket->remainingLength;

    /* Now receive the remaining packet into statically allocated buffer. */
    do
    {
        bytesReturned = ( size_t ) IotBleMqttTransportReceive( &xContext, &buf->pBuffer[ bufferIndex ], bytesToRead );
        receiveAttempts++;

        /* We are guaranteed to read up to the amount of requested bytes.
         * Update the amount of bytes still needed if we read less than requested.
         * Adjust the buffer index to write coniguously in memory. */
        if( bytesReturned <= bytesToRead )
        {
            bufferIndex += bytesReturned;
            bytesToRead -= bytesReturned;
        }

        taskYIELD();
    } while( ( receiveAttempts < MQTT_MAX_RECV_ATTEMPTS ) && ( bytesToRead > 0U ) );

    incomingPacket->pRemainingData = buf->pBuffer;

    return result;
}
/*-----------------------------------------------------------*/

static MQTTStatus_t createMQTTConnectionWithBroker( const MQTTFixedBuffer_t * buf )
{
    MQTTConnectInfo_t mqttConnectInfo;
    size_t remainingLength;
    size_t packetSize;
    MQTTStatus_t result;
    MQTTPacketInfo_t incomingPacket;
    size_t status;
    char demoClientIdentifier[ CLIENT_IDENTIFIER_MAX_LENGTH ];
    uint16_t packetId = 0;
    bool sessionPresent = false;

    LogInfo( ( "Trying to send a connect packet to the broker" ) );

    /* Many fields not used in this demo so start with everything at 0. */
    ( void ) memset( ( void * ) &mqttConnectInfo, 0x00, sizeof( mqttConnectInfo ) );

    /* Start with a clean session i.e. direct the broker to discard any
     * previous session data. Also, establishing a connection with clean session
     * will ensure that the broker does not store any data when this client
     * gets disconnected. */
    mqttConnectInfo.cleanSession = true;

    /* Generate the payload for the PUBLISH. */
    status = ( size_t ) snprintf( demoClientIdentifier,
                                  CLIENT_IDENTIFIER_MAX_LENGTH,
                                  CLIENT_IDENTIFIER_PREFIX "%u",
                                  ( uint16_t ) xTaskGetTickCount() );

    LogInfo( ( "Generated client identifier is %s", demoClientIdentifier ) );

    /* The client identifier is used to uniquely identify this MQTT client to
     * the broker In a production device the identifier can be something
     * unique, such as a device serial number. */
    mqttConnectInfo.pClientIdentifier = demoClientIdentifier;
    mqttConnectInfo.clientIdentifierLength = ( uint16_t ) strlen( demoClientIdentifier );

    /* Set MQTT keep-alive period. It is the responsibility of the application to ensure
     * that the interval between Control Packets being sent does not exceed the Keep Alive value.
     * In the absence of sending any other Control Packets, the Client MUST send a PINGREQ Packet. */
    mqttConnectInfo.keepAliveSeconds = MQTT_KEEP_ALIVE_PERIOD_SECONDS;

    /* Get size requirement for the connect packet */
    result = MQTT_GetConnectPacketSize( &mqttConnectInfo, NULL, &remainingLength, &packetSize );

    /* Serialize MQTT connect packet into the provided buffer. */
    result = MQTT_SerializeConnect( &mqttConnectInfo, NULL, packetSize, buf );
    configASSERT( result == MQTTSuccess );

    /* Send the serialized connect packet to the broker */
    status = ( size_t ) IotBleMqttTransportSend( &xContext, ( void * ) buf->pBuffer, packetSize );
    configASSERT( status == packetSize );

    LogInfo( ( "Successfully sent a connect packet to the broker" ) );
    LogInfo( ( "Waiting for a connection acknowledgement from the broker" ) );

    /* Reset all fields of the incoming packet structure. */
    ( void ) memset( ( void * ) &incomingPacket, 0x00, sizeof( MQTTPacketInfo_t ) );

    status = getNewData( buf, &incomingPacket );

    /* Deserialize the received packet to make sure the content of the CONNACK
     * is valid. Note that the packetId is not present in the connection ack. */
    result = MQTT_DeserializeAck( &incomingPacket, &packetId, &sessionPresent );

    LogInfo( ( "Successfully received a CONNACK packet" ) );

    if( result != MQTTSuccess )
    {
        LogError( ( "Connection with the broker failed." ) );
    }
    else
    {
        LogInfo( ( "Successfully connected with the broker" ) );
    }

    return result;
}


static void mqttSubscribeToTopics( MQTTFixedBuffer_t * const buf )
{
    MQTTStatus_t result = MQTTSuccess;
    MQTTSubscribeInfo_t mqttSubscription;
    size_t remainingLength = 0;
    size_t packetSize = 0;
    size_t status = 0;

    /* When possible, it is recommended to send multiple topics in a single SUBSCRIBE packet.
     * However, for improved compatibility with our iOS and Android companion mobile apps,
     * we send one SUBSCRIBE packet per topic. */
    for( size_t topic_i = 0; topic_i < NUM_SUBS_AT_ONCE; topic_i++ )
    {
        LogInfo( ( "Trying to send a subscribe packet to the broker" ) );

        /***
         * For readability, error handling in this function is restricted to the use of
         * configASSERTs().
         ***/

        /* Some fields not used by this demo so start with everything as 0. */
        remainingLength = 0u;
        packetSize = 0u;
        status = 0;
        ( void ) memset( ( void * ) &mqttSubscription, 0x00, sizeof( mqttSubscription ) );

        /* Populate the topic filter names with user defined strings. */
        mqttSubscription.pTopicFilter = subscriptionArray[ topic_i ];
        mqttSubscription.topicFilterLength = ( uint16_t ) strlen( subscriptionArray[ topic_i ] );

        result = MQTT_GetSubscribePacketSize( &mqttSubscription,
                                              1u,
                                              &remainingLength,
                                              &packetSize );
        configASSERT( result == MQTTSuccess );


        /* Serialize subscribe into statically allocated buffer. */
        subscribePacketIdentifier = getNextPacketIdentifier();
        result = MQTT_SerializeSubscribe( &mqttSubscription,
                                          1u,
                                          subscribePacketIdentifier,
                                          remainingLength,
                                          buf );
        configASSERT( result == MQTTSuccess );

        /* Send Subscribe request to the broker. */
        status = ( size_t ) IotBleMqttTransportSend( &xContext, buf->pBuffer, packetSize );

        configASSERT( status == packetSize );

        /* Process incoming packets from the broker. Recall, we specifically sent a
         * distinct SUBSCRIBE per topic, to improve compatibility with our
         * Android and iOS companion mobile apps. So we expect one SUBACK per topic.
         * There remains the possiblity that another
         * person will publish to the topic before the SUBACK is received, but since
         * we are the only ones using this topic filter name, here that chance is zero.
         * However, one should use a generic incoming packet function for higher use
         * topic filters in case a PUBLISH arrives before the SUBACK. */
        mqttProcessIncomingPacket( buf );

        LogInfo( ( "Successfully sent subscribe packet to the broker" ) );
    }
}


static void mqttUnsubscribeFromTopics( MQTTFixedBuffer_t * const buf )
{
    MQTTStatus_t result = MQTTSuccess;
    MQTTSubscribeInfo_t mqttUnsubscription;
    size_t remainingLength = 0;
    size_t packetSize = 0;
    size_t status = 0;

    for( size_t topic_i = 0; topic_i < NUM_SUBS_AT_ONCE; topic_i++ )
    {
        LogInfo( ( "Trying to send an unsubscribe packet to the broker" ) );

        /* Some fields not used by this demo so start with everything at 0. */
        remainingLength = 0;
        packetSize = 0;
        status = 0;
        ( void ) memset( ( void * ) &mqttUnsubscription, 0x00, sizeof( mqttUnsubscription ) );

        /* Populate the topic filter names with user defined strings. */
        mqttUnsubscription.pTopicFilter = subscriptionArray[ topic_i ];
        mqttUnsubscription.topicFilterLength = ( uint16_t ) strlen( subscriptionArray[ topic_i ] );

        result = MQTT_GetUnsubscribePacketSize( &mqttUnsubscription,
                                                1u,
                                                &remainingLength,
                                                &packetSize );
        configASSERT( result == MQTTSuccess );

        /* Get next unique packet identifier */
        unsubscribePacketIdentifier = getNextPacketIdentifier();
        result = MQTT_SerializeUnsubscribe( &mqttUnsubscription,
                                            1u,
                                            unsubscribePacketIdentifier,
                                            remainingLength,
                                            buf );
        configASSERT( result == MQTTSuccess );

        /* Send Unsubscribe request to the broker. */
        status = ( size_t ) IotBleMqttTransportSend( &xContext, buf->pBuffer, packetSize );
        configASSERT( status == packetSize );

        /* Handle the UNSUBACK */
        mqttProcessIncomingPacket( buf );

        LogInfo( ( "Successfully sent an unsubscribe packet to the broker" ) );
    }
}


static void mqttKeepAlive( const MQTTFixedBuffer_t * buf )
{
    MQTTStatus_t result = MQTTSuccess;
    int32_t status = 0;
    size_t packetSize = 0;

    LogInfo( ( "Trying to send a ping request packet to the broker" ) );

    /* Calculate PING request size. */
    result = MQTT_GetPingreqPacketSize( &packetSize );

    configASSERT( packetSize > 0U );

    result = MQTT_SerializePingreq( buf );
    configASSERT( result == MQTTSuccess );

    /* Send Ping Request to the broker. */
    status = IotBleMqttTransportSend( &xContext, buf->pBuffer, packetSize );
    configASSERT( status == ( int32_t ) packetSize );

    LogInfo( ( "Successfully sent a ping request packet to the broker" ) );
}


static void mqttDisconnect( const MQTTFixedBuffer_t * buf )
{
    MQTTStatus_t result = MQTTSuccess;
    int32_t status = 0;
    size_t packetSize = 0;

    LogInfo( ( "Trying to send a disconnect packet to the broker" ) );

    result = MQTT_GetDisconnectPacketSize( &packetSize );

    configASSERT( packetSize > 0U );

    result = MQTT_SerializeDisconnect( buf );
    configASSERT( result == MQTTSuccess );

    /* Send disconnect packet to the broker */
    status = IotBleMqttTransportSend( &xContext, buf->pBuffer, packetSize );
    configASSERT( status == ( int32_t ) packetSize );

    LogInfo( ( "Successfully sent a disconnect packet to the broker" ) );
}


static void mqttPublishToTopic( const MQTTFixedBuffer_t * buf )
{
    MQTTStatus_t result;
    MQTTPublishInfo_t mqttPublishInfo;
    size_t remainingLength = 0;
    size_t packetSize = 0;
    uint16_t subPacketId = 0;
    size_t bytesSent = 0;

    /***
     * For readability, error handling in this function is restricted to the use of
     * configASSERTs().
     ***/

    LogInfo( ( "Trying to send a publish packet to the broker" ) );

    /* Some fields not used by this demo so start with everything as 0. */
    ( void ) memset( ( void * ) &mqttPublishInfo, 0x00, sizeof( mqttPublishInfo ) );

    for( int subscriptionNumber = 0; subscriptionNumber < NUM_SUBS_AT_ONCE; ++subscriptionNumber )
    {
        /* This demo uses QOS0 */
        mqttPublishInfo.qos = MQTTQoS0;
        mqttPublishInfo.retain = false;
        mqttPublishInfo.pTopicName = subscriptionArray[ subscriptionNumber ];
        mqttPublishInfo.topicNameLength = ( uint16_t ) strlen( subscriptionArray[ subscriptionNumber ] );
        mqttPublishInfo.pPayload = MQTT_EXAMPLE_MESSAGE;
        mqttPublishInfo.payloadLength = strlen( MQTT_EXAMPLE_MESSAGE );
        mqttPublishInfo.dup = false;

        /* Find out length of Publish packet size. */
        result = MQTT_GetPublishPacketSize( &mqttPublishInfo, &remainingLength, &packetSize );
        configASSERT( result == MQTTSuccess );

        subPacketId = getNextPacketIdentifier();

        /* Serialize MQTT Publish packet header. The publish message payload will
         * be sent directly in order to avoid copying it into the buffer.
         * QOS0 does not make use of packet identifier, therefore value of 0 is used */
        result = MQTT_SerializePublish( &mqttPublishInfo,
                                        subPacketId,
                                        remainingLength,
                                        buf );
        configASSERT( result == MQTTSuccess );

        bytesSent = ( size_t ) IotBleMqttTransportSend( &xContext, buf->pBuffer, packetSize );

        configASSERT( bytesSent == ( size_t ) packetSize );

        LogInfo( ( "Successfully published to %s", mqttPublishInfo.pTopicName ) );
    }
}


static void mqttProcessResponse( const MQTTPacketInfo_t * pIncomingPacket,
                                 const uint16_t packetId )
{
    switch( pIncomingPacket->type & HIGH_BYTE_MASK )
    {
        case MQTT_PACKET_TYPE_SUBACK:
            LogInfo( ( "Successfully subscribed to an MQTT topic (received SUBACK)" ) );
            /* Make sure ACK packet identifier matches with Request packet identifier. */
            configASSERT( subscribePacketIdentifier == packetId );
            break;

        case MQTT_PACKET_TYPE_UNSUBACK:
            LogInfo( ( "Successfully unsubscribed to an MQTT topic (recieved UNSUBACK)" ) );
            /* Make sure ACK packet identifier matches with Request packet identifier. */
            configASSERT( unsubscribePacketIdentifier == packetId );
            break;

        case MQTT_PACKET_TYPE_PINGRESP:
            LogInfo( ( "Ping Response successfully received (received PINGRESP)" ) );
            break;

        /* Any other packet type is invalid. */
        default:
            LogWarn( ( "mqttProcessResponse() called with unknown packet type:(%u).",
                       ( uint8_t ) pIncomingPacket->type ) );
            break;
    }
}


static void mqttProcessIncomingPublish( const MQTTPublishInfo_t * pPubInfo,
                                        const uint16_t packetId )
{
    configASSERT( pPubInfo != NULL );
    bool matchedSubscription = false;

    /* Since this example does not make use of QOS1 or QOS2,
     * packet identifier is not required. */
    ( void ) packetId;

    /* Verify the received publish is for the topic we have subscribed to. */
    for( size_t i = 0; i < NUM_SUBS_AT_ONCE; ++i )
    {
        if( 0 == strncmp( subscriptionArray[ i ], pPubInfo->pTopicName, pPubInfo->topicNameLength ) )
        {
            LogInfo( ( "Incoming Publish Topic Name: %.*s matches subscribed topic.",
                       pPubInfo->topicNameLength,
                       pPubInfo->pTopicName ) );
            LogInfo( ( "Incoming Publish Message : %.*s",
                       pPubInfo->payloadLength,
                       ( char * ) pPubInfo->pPayload ) );
            matchedSubscription = true;
            break;
        }
    }

    if( !matchedSubscription )
    {
        LogError( ( "Incoming Publish Topic Name: %.*s does not match any subscribed topic.",
                    pPubInfo->topicNameLength,
                    pPubInfo->pTopicName ) );
    }
}


static void mqttProcessIncomingPacket( MQTTFixedBuffer_t * buf )
{
    MQTTStatus_t result = MQTTSuccess;
    MQTTPacketInfo_t incomingPacket;
    MQTTPublishInfo_t publishInfo;
    uint16_t responsePacketId = 0;
    bool sessionPresent = false;

    LogInfo( ( "Trying to receive an incoming packet" ) );

    /***
     * For readability, error handling in this function is restricted to the use of
     * configASSERTs().
     ***/

    ( void ) memset( ( void * ) &incomingPacket, 0x00, sizeof( MQTTPacketInfo_t ) );

    result = getNewData( buf, &incomingPacket );

    if( ( incomingPacket.type & HIGH_BYTE_MASK ) == MQTT_PACKET_TYPE_PUBLISH )
    {
        result = MQTT_DeserializePublish( &incomingPacket, &responsePacketId, &publishInfo );
        configASSERT( result == MQTTSuccess );
        LogInfo( ( "Incoming publish packet received successfully." ) );

        /* Process incoming Publish message. */
        mqttProcessIncomingPublish( &publishInfo, responsePacketId );
    }
    else
    {
        /* If the received packet is not a Publish message, then it is an ACK for one
         * of the messages we sent out, verify that the ACK packet is a valid MQTT
         * packet. Since CONNACK is already processed, session present parameter is
         * to NULL */
        result = MQTT_DeserializeAck( &incomingPacket, &responsePacketId, &sessionPresent );
        configASSERT( result == MQTTSuccess );

        /* Process the response. */
        mqttProcessResponse( &incomingPacket, responsePacketId );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Entry point of demo.
 */
MQTTStatus_t RunMQTTBLETransportDemo( void )
{
    MQTTStatus_t status = MQTTSuccess;
    MQTTFixedBuffer_t fixedBuffer;
    uint16_t loopCount = 0;
    const uint16_t maxLoopCount = 5U;
    uint16_t demoIterations = 0;
    const uint16_t maxDemoIterations = 5U;
    bool publishPacketSent = false;

    /***
     * Set Fixed size buffer structure that is required by API to serialize
     * and deserialize data. pBuffer is pointing to a fixed sized mqttSharedBuffer.
     * The application may allocate dynamic memory as well.
     ***/
    fixedBuffer.pBuffer = fixedBufferBuf;
    fixedBuffer.size = SINGLE_INCOMING_PACKET_MAX_SIZE;

    xContext.pParams = &xBleTransportParams;

    status = demoInitChannel();

    if( status != MQTTSuccess )
    {
        LogError( ( "There was a problem initializing the data channel" ) );
        configASSERT( false );
    }

    for( demoIterations = 0; demoIterations < maxDemoIterations; demoIterations++ )
    {
        if( status == MQTTSuccess )
        {
            /* Send a connect packet to the broker and waits for a connack packet
             * to establish an end to end connection */
            status = createMQTTConnectionWithBroker( &fixedBuffer );

            if( ( status != MQTTSuccess ) && ( channelActive == false ) )
            {
                break;
            }

            /**************************** Subscribe. ******************************/

            /* The client is now connected to the broker Subscribe to each of the user defined topics.
             * This client will then publish to the same topic it subscribed to, so it
             * will expect all the messages it sends to the broker to be sent back to it
             * from the broker. This demo uses QOS0 in subscribe, therefore, the Publish
             * messages received from the broker will have QOS0. */
            mqttSubscribeToTopics( &fixedBuffer );

            /********************* Publish and Keep Alive Loop. ********************/
            /* Publish messages with QOS0, send and process Keep alive messages. */
            for( loopCount = 0; loopCount < maxLoopCount; loopCount++ )
            {
                /* If we disconnected along the way, exit the loop and log an error */
                if( channelActive == false )
                {
                    break;
                }

                /* Get the current time stamp */
                /* Publish to the topic every other time to trigger sending of PINGREQ  */
                if( publishPacketSent == false )
                {
                    mqttPublishToTopic( &fixedBuffer );

                    /* Set control packet sent flag to true so that the lastControlPacketSent
                     * timestamp will be updated. */
                    publishPacketSent = true;

                    /* Loop through and accept each incoming publish from the ones we just sent. */
                    for( size_t subscriptionNumber = 0; subscriptionNumber < NUM_SUBS_AT_ONCE; ++subscriptionNumber )
                    {
                        mqttProcessIncomingPacket( &fixedBuffer );
                    }
                }
                else
                {
                    /* Check if the keep-alive period has elapsed, since the last control packet was sent.
                     * If the period has elapsed, send out MQTT PINGREQ to the broker.  */
                    mqttKeepAlive( &fixedBuffer );

                    /* Since PUBLISH packet is not sent for this iteration, set publishPacketSent to false
                     * so the next iteration will send PUBLISH .*/
                    publishPacketSent = false;

                    /* Get the Pingresp */
                    mqttProcessIncomingPacket( &fixedBuffer );
                }

                /* Sleep until keep alive time period, so that for the next iteration this
                 * loop will send out a PINGREQ if PUBLISH was not sent for this iteration.
                 * The broker will wait till 1.5 times keep-alive period before it disconnects
                 * the client. */
                ( void ) vTaskDelay( pdMS_TO_TICKS( MQTT_KEEP_ALIVE_PERIOD_SECONDS * MILLISECONDS_PER_SECOND ) );
            }

            /* If we disconnected along the way, exit the loop and log an error */
            if( channelActive == false )
            {
                break;
            }

            mqttUnsubscribeFromTopics( &fixedBuffer );

            /* Send an MQTT Disconnect packet over the already connected BLE channel.
             * There is no corresponding response for the disconnect packet. After sending
             * disconnect, client must close the network connection. */
            LogInfo( ( "Disconnecting the MQTT connection with the broker " ) );
            mqttDisconnect( &fixedBuffer );
        }

        if( demoIterations < ( maxDemoIterations - 1U ) )
        {
            /* Wait for some time between two iterations to ensure that we do not
             * bombard the broker. */
            LogInfo( ( "Short delay before starting the next iteration.... \r\n\r\n" ) );
            ( void ) vTaskDelay( pdMS_TO_TICKS( MQTT_DEMO_ITERATION_DELAY_SECONDS * MILLISECONDS_PER_SECOND ) );

            /* Clean up the channel in between iterations */
            IotBleMqttTransportCleanup( &xContext );
            IotBleMqttTransportInit( contextBuf, INCOMING_PACKET_QUEUE_SIZE, &xContext );
        }
    }

    IotBleMqttTransportCleanup( &xContext );

    if( channelActive == false )
    {
        LogError( ( "BLE disconnected unexpectedly" ) );
        status = MQTTBadResponse;
    }
    else
    {
        LogInfo( ( "Demo completed successfully." ) );
    }

    return status;
}

/*-----------------------------------------------------------*/
