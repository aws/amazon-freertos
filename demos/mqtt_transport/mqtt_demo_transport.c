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

/* Include demo_config.h first for logging and other configuration */
#include "demo_config.h"

#include "FreeRTOS.h"
#include "task.h"

#include "iot_ble_mqtt_transport.h"
#include "iot_ble_data_transfer.h"
#include "iot_ble_config_defaults.h"
#include "platform/iot_clock.h"

/* Standard includes. */
#include <stdlib.h>
#include <string.h>

/* POSIX socket includes. */
#include <netdb.h>
#include <unistd.h>
#include <assert.h>

#include <sys/socket.h>
#include <sys/types.h>



/************ End of logging configuration ****************/

/**
 * @brief The first characters in the client identifier. A timestamp is appended
 * to this prefix to create a unique client identifer.
 *
 * This prefix is also used to generate topic names and topic filters used in this
 * demo.
 */
#define CLIENT_IDENTIFIER_PREFIX             "iotdemo"

/**
 * @brief The maximum length the demo will use for client identifier.
 */
#define CLIENT_IDENTIFIER_MAX_LENGTH         12U

/**
 * @brief The topic the demo will subscribe and publish to.
 */
#define MQTT_EXAMPLE_TOPIC                   "iotdemo/test/abc"

/**
 * @brief The size of the buffer in bytes passed to the transport code to hold streaming data.
 */
#define STATIC_BUFFER_SIZE                   200U

/**
 * @brief The MQTT message published in this example.
 */
#define MQTT_EXAMPLE_MESSAGE                 "Hello MQTT World!"

/**
 * @brief Keep alive period in seconds for MQTT connection.
 */
#define MQTT_KEEP_ALIVE_PERIOD_SECONDS       5U

/**
 * @brief Number of time network receive will be attempted
 * if it fails due to transportTimeout.
 */
#define MQTT_MAX_RECV_ATTEMPTS               100U

/**
 * @brief Delay between two demo iterations.
 */
#define MQTT_DEMO_ITERATION_DELAY_SECONDS    5U

#define HIGH_BYTE_MASK                       0xF0U

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
 * @brief Packet Identifier generated when Subscribe request was sent to the broker;
 * it is used to match received Subscribe ACK to the transmitted SUBSCRIBE request.
 */
static uint16_t subscribePacketIdentifier;

/**
 * @brief Packet Identifier generated when Unsubscribe request was sent to the broker;
 * it is used to match received Unsubscribe response to the transmitted unsubscribe
 * request.
 */
static uint16_t unsubscribePacketIdentifier;

/**
 * @brief Network context structure to store the data channel.
 */
static NetworkContext_t pContext;

/**
 * @brief Flag to mark if the channel has been disconnected at all
 */
static volatile bool channelActive = true;


/*-----------------------------------------------------------*/

static void demoCallback( IotBleDataTransferChannelEvent_t event,
                          IotBleDataTransferChannel_t * pChannel,
                          void * context )
{
    /* Unused parameters. */
    ( void ) pChannel;
    ( void ) context;

    /* Event to see when the data channel is ready to receive data. */
    if( event == IOT_BLE_DATA_TRANSFER_CHANNEL_OPENED )
    {
        IotSemaphore_Post( &pContext.isReady );
    }

    /* Event for when data is received over the channel. */
    else if( event == IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVED )
    {
        IotBleMqttTransportAcceptData( &pContext );
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

    /* Must initialize the channel, pContext must contain the buffer and buf size at this point. */
    IotBleMqttTransportInit( &pContext );

    /* Open is a handshake proceture, so we need to wait until it is ready to use. */
    pContext.pChannel = IotBleDataTransfer_Open( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT );

    if( pContext.pChannel != NULL )
    {
        if( IotSemaphore_Create( &pContext.isReady, 0, 1 ) == true )
        {
            ( void ) IotBleDataTransfer_SetCallback( pContext.pChannel, demoCallback, NULL );

            if( IotSemaphore_TimedWait( &pContext.isReady, IOT_BLE_MQTT_CREATE_CONNECTION_WAIT_MS ) == true )
            {
                LogInfo( ( "The channel was initialized successfully" ) );

                status = MQTTSuccess;
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
        status = MQTTServerRefused;
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

    do
    {
        taskYIELD();
        result = MQTT_GetIncomingPacketTypeAndLength( IotBleMqttTransportReceive, &pContext, incomingPacket );
        receiveAttempts++;
    } while( ( receiveAttempts < MQTT_MAX_RECV_ATTEMPTS ) && ( result != MQTTSuccess ) );

    assert( result == MQTTSuccess );

    receiveAttempts = 0;
    bytesToRead = incomingPacket->remainingLength;

    /* Now receive the remaining packet into statically allocated buffer. */
    do
    {
        bytesReturned = ( size_t ) IotBleMqttTransportReceive( &pContext, &buf->pBuffer[ bufferIndex ], bytesToRead );
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

    LogDebug( ( "Trying to send a connect packet to the server" ) );

    /* Many fields not used in this demo so start with everything at 0. */
    ( void ) memset( ( void * ) &mqttConnectInfo, 0x00, sizeof( mqttConnectInfo ) );

    /* Start with a clean session i.e. direct IoT Core to discard any
     * previous session data. Also, establishing a connection with clean session
     * will ensure that the broker does not store any data when this client
     * gets disconnected. */
    mqttConnectInfo.cleanSession = true;

    /* Generate the payload for the PUBLISH. */
    status = ( size_t ) snprintf( demoClientIdentifier,
                                  CLIENT_IDENTIFIER_MAX_LENGTH,
                                  CLIENT_IDENTIFIER_PREFIX "%u",
                                  ( uint16_t ) IotClock_GetTimeMs() );

    LogInfo( ( "Generated client identifier is %s", demoClientIdentifier ) );

    /* The client identifier is used to uniquely identify this MQTT client to
     * IoT Core. In a production device the identifier can be something
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
    assert( result == MQTTSuccess );

    /* Send the serialized connect packet to IoT Core */
    status = ( size_t ) IotBleMqttTransportSend( &pContext, ( void * ) buf->pBuffer, packetSize );
    assert( status == packetSize );

    LogDebug( ( "Successfully sent a connect packet to the server" ) );
    LogDebug( ( "Waiting for a connection acknowledgement from the server" ) );

    /* Reset all fields of the incoming packet structure. */
    ( void ) memset( ( void * ) &incomingPacket, 0x00, sizeof( MQTTPacketInfo_t ) );

    status = getNewData( buf, &incomingPacket );

    /* Deserialize the received packet to make sure the content of the CONNACK
     * is valid. Note that the packetId is not present in the connection ack. */
    result = MQTT_DeserializeAck( &incomingPacket, &packetId, &sessionPresent );

    LogDebug( ( "Successfully received a connack packet" ) );

    if( result != MQTTSuccess )
    {
        LogError( ( "Connection with IoT Core failed.\r\n" ) );
    }
    else
    {
        LogInfo( ( "Successfully connected with IoT Core\r\n" ) );
    }

    return result;
}


static void mqttSubscribeToTopic( const MQTTFixedBuffer_t * buf )
{
    MQTTStatus_t result = MQTTSuccess;
    MQTTSubscribeInfo_t mqttSubscription[ 1 ];
    size_t remainingLength = 0;
    size_t packetSize = 0;
    size_t status = 0;

    LogDebug( ( "Trying to send a subscribe packet to the server" ) );

    /***
     * For readability, error handling in this function is restricted to the use of
     * asserts().
     ***/

    /* Some fields not used by this demo so start with everything as 0. */
    ( void ) memset( ( void * ) &mqttSubscription, 0x00, sizeof( mqttSubscription ) );

    /* Subscribe to the MQTT_EXAMPLE_TOPIC topic filter. This example subscribes to
     * only one topic and uses QOS0. */
    mqttSubscription[ 0 ].qos = MQTTQoS0;
    mqttSubscription[ 0 ].pTopicFilter = MQTT_EXAMPLE_TOPIC;
    mqttSubscription[ 0 ].topicFilterLength = ( uint16_t ) strlen( MQTT_EXAMPLE_TOPIC );

    result = MQTT_GetSubscribePacketSize( mqttSubscription,
                                          sizeof( mqttSubscription ) / sizeof( MQTTSubscribeInfo_t ),
                                          &remainingLength, &packetSize );

    assert( result == MQTTSuccess );

    subscribePacketIdentifier = getNextPacketIdentifier();

    /* Serialize subscribe into statically allocated buffer. */
    result = MQTT_SerializeSubscribe( mqttSubscription,
                                      sizeof( mqttSubscription ) / sizeof( MQTTSubscribeInfo_t ),
                                      subscribePacketIdentifier,
                                      remainingLength,
                                      buf );
    assert( result == MQTTSuccess );

    /* Send Subscribe request to the broker. */
    status = ( size_t ) IotBleMqttTransportSend( &pContext, buf->pBuffer, packetSize );
    assert( status == packetSize );

    LogDebug( ( "Successfully sent subscribe packet to the server" ) );
}


static void mqttUnsubscribeFromTopic( const MQTTFixedBuffer_t * buf )
{
    MQTTStatus_t result = MQTTSuccess;
    MQTTSubscribeInfo_t mqttSubscription[ 1 ];
    size_t remainingLength = 0;
    size_t packetSize = 0;
    size_t status = 0;

    LogDebug( ( "Trying to send an unsubscribe packet to the server" ) );

    /* Some fields not used by this demo so start with everything at 0. */
    ( void ) memset( ( void * ) &mqttSubscription, 0x00, sizeof( mqttSubscription ) );

    /* Unsubscribe to the MQTT_EXAMPLE_TOPIC topic filter. */
    mqttSubscription[ 0 ].qos = MQTTQoS0;
    mqttSubscription[ 0 ].pTopicFilter = MQTT_EXAMPLE_TOPIC;
    mqttSubscription[ 0 ].topicFilterLength = ( uint16_t ) strlen( MQTT_EXAMPLE_TOPIC );

    result = MQTT_GetUnsubscribePacketSize( mqttSubscription,
                                            sizeof( mqttSubscription ) / sizeof( MQTTSubscribeInfo_t ),
                                            &remainingLength,
                                            &packetSize );
    assert( result == MQTTSuccess );

    /* Get next unique packet identifier */
    unsubscribePacketIdentifier = getNextPacketIdentifier();

    result = MQTT_SerializeUnsubscribe( mqttSubscription,
                                        sizeof( mqttSubscription ) / sizeof( MQTTSubscribeInfo_t ),
                                        unsubscribePacketIdentifier,
                                        remainingLength,
                                        buf );
    assert( result == MQTTSuccess );

    /* Send Unsubscribe request to the broker. */
    status = ( size_t ) IotBleMqttTransportSend( &pContext, buf->pBuffer, packetSize );
    assert( status == packetSize );

    LogDebug( ( "Successfully sent an unsubscribe packet to the server" ) );
}


static void mqttKeepAlive( const MQTTFixedBuffer_t * buf )
{
    MQTTStatus_t result = MQTTSuccess;
    int32_t status = 0;
    size_t packetSize = 0;

    LogDebug( ( "Trying to send a ping request packet to the server" ) );

    /* Calculate PING request size. */
    result = MQTT_GetPingreqPacketSize( &packetSize );

    assert( packetSize > 0U );

    result = MQTT_SerializePingreq( buf );
    assert( result == MQTTSuccess );

    /* Send Ping Request to the broker. */
    status = IotBleMqttTransportSend( &pContext, buf->pBuffer, packetSize );
    assert( status == ( int32_t ) packetSize );

    LogDebug( ( "Successfully sent a ping request packet to the server" ) );
}


static void mqttDisconnect( const MQTTFixedBuffer_t * buf )
{
    MQTTStatus_t result = MQTTSuccess;
    int32_t status = 0;
    size_t packetSize = 0;

    LogDebug( ( "Trying to send a disconnect packet to the server" ) );

    result = MQTT_GetDisconnectPacketSize( &packetSize );

    assert( packetSize > 0U );

    result = MQTT_SerializeDisconnect( buf );
    assert( result == MQTTSuccess );

    /* Send disconnect packet to the broker */
    status = IotBleMqttTransportSend( &pContext, buf->pBuffer, packetSize );
    assert( status == ( int32_t ) packetSize );

    LogDebug( ( "Successfully sent a disconnect packet to the server" ) );
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
     * asserts().
     ***/

    LogDebug( ( "Trying to send a publish packet to the server" ) );

    /* Some fields not used by this demo so start with everything as 0. */
    ( void ) memset( ( void * ) &mqttPublishInfo, 0x00, sizeof( mqttPublishInfo ) );

    /* This demo uses QOS0 */
    mqttPublishInfo.qos = MQTTQoS0;
    mqttPublishInfo.retain = false;
    mqttPublishInfo.pTopicName = MQTT_EXAMPLE_TOPIC;
    mqttPublishInfo.topicNameLength = ( uint16_t ) strlen( MQTT_EXAMPLE_TOPIC );
    mqttPublishInfo.pPayload = MQTT_EXAMPLE_MESSAGE;
    mqttPublishInfo.payloadLength = strlen( MQTT_EXAMPLE_MESSAGE );
    mqttPublishInfo.dup = false;

    /* Find out length of Publish packet size. */
    result = MQTT_GetPublishPacketSize( &mqttPublishInfo, &remainingLength, &packetSize );
    assert( result == MQTTSuccess );

    subPacketId = getNextPacketIdentifier();

    /* Serialize MQTT Publish packet header. The publish message payload will
     * be sent directly in order to avoid copying it into the buffer.
     * QOS0 does not make use of packet identifier, therefore value of 0 is used */
    result = MQTT_SerializePublish( &mqttPublishInfo,
                                    subPacketId,
                                    remainingLength,
                                    buf );
    assert( result == MQTTSuccess );

    bytesSent = ( size_t ) IotBleMqttTransportSend( &pContext, buf->pBuffer, packetSize );

    assert( bytesSent == ( size_t ) packetSize );

    LogDebug( ( "Successfully sent a publish packet to the server" ) );
}


static void mqttProcessResponse( const MQTTPacketInfo_t * pIncomingPacket,
                                 const uint16_t packetId )
{
    switch( pIncomingPacket->type & HIGH_BYTE_MASK )
    {
        case MQTT_PACKET_TYPE_SUBACK:
            LogDebug( ( "Subscribed to the topic %s.\r\n", MQTT_EXAMPLE_TOPIC ) );
            /* Make sure ACK packet identifier matches with Request packet identifier. */
            assert( subscribePacketIdentifier == packetId );
            break;

        case MQTT_PACKET_TYPE_UNSUBACK:
            LogDebug( ( "Unsubscribed from the topic %s.\r\n", MQTT_EXAMPLE_TOPIC ) );
            /* Make sure ACK packet identifier matches with Request packet identifier. */
            assert( unsubscribePacketIdentifier == packetId );
            break;

        case MQTT_PACKET_TYPE_PINGRESP:
            LogDebug( ( "Ping Response successfully received.\r\n" ) );
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
    assert( pPubInfo != NULL );

    /* Since this example does not make use of QOS1 or QOS2,
     * packet identifier is not required. */
    ( void ) packetId;

    /* Verify the received publish is for the topic we have subscribed to. */
    if( ( pPubInfo->topicNameLength == strlen( MQTT_EXAMPLE_TOPIC ) ) &&
        ( 0 == strncmp( MQTT_EXAMPLE_TOPIC, pPubInfo->pTopicName, pPubInfo->topicNameLength ) ) )
    {
        LogInfo( ( "Incoming Publish Topic Name: %.*s matches subscribed topic.\n",
                   pPubInfo->topicNameLength,
                   pPubInfo->pTopicName ) );
        LogInfo( ( "Incoming Publish Message : %.*s\n",
                   pPubInfo->payloadLength,
                   ( char * ) pPubInfo->pPayload ) );
    }
    else
    {
        LogError( ( "Incoming Publish Topic Name: %.*s does not match subscribed topic. \n",
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

    LogDebug( ( "Trying to receive an incoming packet" ) );

    /***
     * For readability, error handling in this function is restricted to the use of
     * asserts().
     ***/

    ( void ) memset( ( void * ) &incomingPacket, 0x00, sizeof( MQTTPacketInfo_t ) );

    result = getNewData( buf, &incomingPacket );

    if( ( incomingPacket.type & HIGH_BYTE_MASK ) == MQTT_PACKET_TYPE_PUBLISH )
    {
        result = MQTT_DeserializePublish( &incomingPacket, &responsePacketId, &publishInfo );
        assert( result == MQTTSuccess );
        LogDebug( ( "Incoming publish packet received successfully." ) );

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
        assert( result == MQTTSuccess );

        /* Process the response. */
        mqttProcessResponse( &incomingPacket, responsePacketId );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Entry point of demo.
 */
MQTTStatus_t RunMQTTTransportDemo( void )
{
    MQTTStatus_t status = MQTTSuccess;
    MQTTFixedBuffer_t fixedBuffer;
    uint16_t loopCount = 0;
    const uint16_t maxLoopCount = 5U;
    uint16_t demoIterations = 0;
    const uint16_t maxDemoIterations = 5U;
    bool publishPacketSent = false;

    /***
     * The network context buffer must be provided by the application.
     * Here we use static memory, but dynamic memory is OK too, so
     * long as it is provided and allocated by the user
     ***/
    uint8_t buf[ STATIC_BUFFER_SIZE ];

    pContext.buf = buf;
    pContext.bufSize = STATIC_BUFFER_SIZE;

    /***
     * Set Fixed size buffer structure that is required by API to serialize
     * and deserialize data. pBuffer is pointing to a fixed sized mqttSharedBuffer.
     * The application may allocate dynamic memory as well.
     ***/
    uint8_t demoStaticBuffer[ STATIC_BUFFER_SIZE ];
    fixedBuffer.pBuffer = demoStaticBuffer;
    fixedBuffer.size = STATIC_BUFFER_SIZE;

    status = demoInitChannel();

    if( status != MQTTSuccess )
    {
        LogError( ( "There was a problem initializing the data channel" ) );
        assert( false );
    }

    for( demoIterations = 0; demoIterations < maxDemoIterations; demoIterations++ )
    {
        if( status == MQTTSuccess )
        {
            /* Sends an MQTT Connect packet over the already connected TCP socket
             * tcpSocket, and waits for connection acknowledgment (CONNACK) packet. */
            LogInfo( ( "Establishing MQTT connection to server" ) );
            status = createMQTTConnectionWithBroker( &fixedBuffer );

            if( ( status != MQTTSuccess ) && ( channelActive == false ) )
            {
                break;
            }

            /**************************** Subscribe. ******************************/

            /* The client is now connected to the broker. Subscribe to the topic
             * as specified in MQTT_EXAMPLE_TOPIC at the top of this file by sending a
             * subscribe packet then waiting for a subscribe acknowledgment (SUBACK).
             * This client will then publish to the same topic it subscribed to, so it
             * will expect all the messages it sends to the broker to be sent back to it
             * from the broker. This demo uses QOS0 in subscribe, therefore, the Publish
             * messages received from the broker will have QOS0. */
            /* Subscribe and SUBACK */
            LogInfo( ( "Attempt to subscribe to the MQTT topic %s\r\n", MQTT_EXAMPLE_TOPIC ) );
            mqttSubscribeToTopic( &fixedBuffer );

            /* Process incoming packet from the broker. After sending the subscribe, the
             * client may receive a publish before it receives a subscribe ack. Therefore,
             * call generic incoming packet processing function. Since this demo is
             * subscribing to the topic to which no one is publishing, probability of
             * receiving Publish message before subscribe ack is zero; but application
             * must be ready to receive any packet.  This demo uses the generic packet
             * processing function everywhere to highlight this fact. */
            mqttProcessIncomingPacket( &fixedBuffer );

            /********************* Publish and Keep Alive Loop. ********************/
            /* Publish messages with QOS0, send and process Keep alive messages. */
            for( loopCount = 0; loopCount < maxLoopCount; loopCount++ )
            {
                if( channelActive == false )
                {
                    break;
                }

                /* Get the current time stamp */
                /* Publish to the topic every other time to trigger sending of PINGREQ  */
                if( publishPacketSent == false )
                {
                    LogInfo( ( "Publish to the MQTT topic %s\r\n", MQTT_EXAMPLE_TOPIC ) );
                    mqttPublishToTopic( &fixedBuffer );

                    /* Set control packet sent flag to true so that the lastControlPacketSent
                     * timestamp will be updated. */
                    publishPacketSent = true;
                }
                else
                {
                    /* Check if the keep-alive period has elapsed, since the last control packet was sent.
                     * If the period has elapsed, send out MQTT PINGREQ to the broker.  */

                    /* Send PINGREQ to the broker */
                    LogInfo( ( "Sending PINGREQ to the broker\n " ) );
                    mqttKeepAlive( &fixedBuffer );

                    /* Since PUBLISH packet is not sent for this iteration, set publishPacketSent to false
                     * so the next iteration will send PUBLISH .*/
                    publishPacketSent = false;
                }

                /* Since the application is subscribed publishing messages to the same topic,
                 * the broker will send the same message back to the application.
                 * Process incoming PUBLISH echo or PINGRESP. */
                mqttProcessIncomingPacket( &fixedBuffer );

                /* Sleep until keep alive time period, so that for the next iteration this
                 * loop will send out a PINGREQ if PUBLISH was not sent for this iteration.
                 * The broker will wait till 1.5 times keep-alive period before it disconnects
                 * the client. */
                ( void ) sleep( MQTT_KEEP_ALIVE_PERIOD_SECONDS );
            }

            if( channelActive == false )
            {
                break;
            }

            /* Unsubscribe from the previously subscribed topic */
            LogInfo( ( "Unsubscribe from the MQTT topic %s.\r\n", MQTT_EXAMPLE_TOPIC ) );
            mqttUnsubscribeFromTopic( &fixedBuffer );
            /* Process Incoming unsubscribe ack from the broker. */
            mqttProcessIncomingPacket( &fixedBuffer );

            /* Send an MQTT Disconnect packet over the already connected TCP socket.
             * There is no corresponding response for the disconnect packet. After sending
             * disconnect, client must close the network connection. */
            LogInfo( ( "Disconnecting the MQTT connection with %s.\r\n", MQTT_EXAMPLE_TOPIC ) );
            mqttDisconnect( &fixedBuffer );
        }

        if( demoIterations < ( maxDemoIterations - 1U ) )
        {
            /* Wait for some time between two iterations to ensure that we do not
             * bombard the public test mosquitto broker. */
            LogInfo( ( "Short delay before starting the next iteration.... \r\n\r\n" ) );
            ( void ) sleep( MQTT_DEMO_ITERATION_DELAY_SECONDS );

            IotBleMqttTransportCleanup();
            IotBleMqttTransportInit( &pContext );
        }
    }

    if( channelActive == false )
    {
        IotBleMqttTransportCleanup();
        LogError( ( "BLE disconnected unexpectedly" ) );
        status = MQTTBadResponse;
    }
    else
    {
        LogInfo( ( "Demo completed successfully.\r\n" ) );
    }

    return status;
}

/*-----------------------------------------------------------*/
