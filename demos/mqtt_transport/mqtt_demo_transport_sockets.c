/*
 * Amazon FreeRTOS V201908.00
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Set up logging for this demo. */
#include "iot_demo_logging.h"

/* Network config include */
#include "types/iot_network_types.h"
#include "aws_iot_network_config.h"

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_network.h"
#include "transport_secure_sockets.h"

/* Transport interface include*/
#include "transport_interface.h"

/* MQTT include. */
#include "mqtt.h"
#include "mqtt_config.h"
#include "mqtt_lightweight.h"

/*-----------------------------------------------------------*/

/**
 * @brief The first characters in the client identifier. A timestamp is appended
 *        to this prefix to create a unique client identifier.
 *
 *        This prefix is also used to generate topic names and topic filters used in this
 *        demo.
 */
#define CLIENT_IDENTIFIER_PREFIX                 "iotdemo"

/**
 * @brief The longest client identifier that an MQTT server must accept (as defined
 *        by the MQTT 3.1.1 spec) is 23 characters. Add 1 to include the length of the NULL
 *        terminator.
 */
#define CLIENT_IDENTIFIER_MAX_LENGTH             ( 24U )


/**
 * @brief Size of the network buffer for MQTT packets.
 */
#define NETWORK_BUFFER_SIZE                      ( 1024U )

/**
 * @brief Timeout for receiving packet in milli seconds.
 */
#define RECV_TIMEOUT_MS                          ( 5000U )

/**
 * @brief Default loop counts for a topic to be send.
 */
#define DEFAULT_LOOP_COUNT                       ( 10U )

/**
 * @brief Transport timeout in milliseconds for transport send.
 */
#define DEMO_TRANSPORT_SND_TIMEOUT_MS            ( 5000U )

/**
 * @brief Transport timeout in milliseconds for transport receive.
 */
#define DEMO_TRANSPORT_RCV_TIMEOUT_MS            ( 5000U )

/**
 * @brief The MQTT message published and subscribe topic in this example.
 */
#define DEMO_TOPIC                               "iotdemo/transport/sockets"

/**
 * @brief The MQTT message payload in this example.
 */
#define DEMO_BUFFER                              "Hello world from network FreeRTOS transport sockets!"

/*-----------------------------------------------------------*/


/**
 * @brief The network buffer must remain valid for the lifetime of the MQTT context.
 */
uint8_t mqttSharedBuffer[ NETWORK_BUFFER_SIZE ];

/*-----------------------------------------------------------*/

/**
 * @brief The function that runs the LTS MQTT with transport over secure sockets
 *        demo called by the demo runner.
 *
 * @param[in] awsIotMqttMode Specify if this demo is running with the AWS IoT
 *            MQTT server. Set this to `false` if using another MQTT server.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 *            establishing the MQTT connection.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 *            establishing the MQTT connection.
 * @param[in] pNetworkInterface The network interface to use for this demo.
 *
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE` otherwise.
 */
int RunMQTTTransportDemoSocekts( bool awsIotMqttMode,
                                 const char * pIdentifier,
                                 void * pNetworkServerInfo,
                                 void * pNetworkCredentialInfo,
                                 const IotNetworkInterface_t * pUnused );

/**
 * @brief Create a context and Connect to Server.
 *
 * @param[out] pNetworkContext The output parameter to return the created network context.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 *            establishing the MQTT connection.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 *            establishing the MQTT connection.
 *
 * @return #EXIT_SUCCESS on success;
 *         #EXIT_FAIL on failure.
 */
static int32_t connectToServerWithBroker( NetworkContext_t * pNetworkContext,
                                          void * pNetworkServerInfo,
                                          void * pNetworkCredentialInfo );

/**
 * @brief Closes a TLS session on top of a TCP connection using the Secure Sockets API.
 *
 * @param[out] pNetworkContext The output parameter to end the TLS session and
 *             clean the created network context.
 *
 * @return #EXIT_SUCCESS on success;
 *         #EXIT_FAIL on failure.
 */
static int32_t disconnectToServerWithBroker( NetworkContext_t * pNetworkContext );


/**
 * @brief The application callback function for getting the incoming publish
 * and incoming acks reported from MQTT library.
 *
 * @param[in] pContext MQTT context pointer.
 * @param[in] pPacketInfo Packet Info pointer for the incoming packet.
 * @param[in] packetIdentifier Packet Info pointer for the incoming packet.
 * @param[in] pPublishInfo Pointer to publish info of the incoming publish.
 */
static void eventCallback( MQTTContext_t * pContext,
                           MQTTPacketInfo_t * pPacketInfo,
                           uint16_t packetIdentifier,
                           MQTTPublishInfo_t * pPublishInfo );


/**
 * @brief Load the configENABLED_NETWORKS netwokrs into the network list.
 *
 * @param[out] pNetworkList The enabled network lists.
 *
 * @return Configured count of networks.
 *
 */
static uint8_t getConfiguredNetworkList( uint8_t * pNetworkList );

/**
 * @brief Initializes the MQTT library.
 *
 * @param[in] pMqttContext MQTT context pointer.
 * @param[in] pNetworkContext The network context pointer.
 *
 * @return EXIT_SUCCESS if the MQTT library is initialized;
 *         EXIT_FAIL otherwise.
 */
static int32_t initializeMqtt( MQTTContext_t * pMqttContext,
                               NetworkContext_t * pNetworkContext );

/**
 * @brief Sends an MQTT CONNECT packet over the already connected TCP socket.
 *
 * @param[in] pContext MQTT context pointer.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier.
 *
 * @return EXIT_SUCCESS if an MQTT session is established;
 *         EXIT_FAIL otherwise.
 */
static int32_t establishMqttSession( MQTTContext_t * pContext,
                                     const char * pIdentifier );

/**
 * @brief Sends an MQTT PUBLISH to #DEMO_TOPIC defined at
 * the top of the file.
 *
 * @param[in] pMqttContext MQTT context pointer.
 * @param[in] networkIndex Indicates which transport type is used.
 *
 * @return EXIT_SUCCESS if PUBLISH was successfully sent;
 *         EXIT_FAIL otherwise.
 */
static int32_t publishToTopic( MQTTContext_t * pContext );

/**
 * @brief Sends an MQTT SUBSCRIBE to #DEMO_TOPIC defined at
 * the top of the file.
 *
 * @param[in] pMqttContext MQTT context pointer.
 * @param[in] networkIndex Indicates which transport type is used.
 *
 * @return EXIT_SUCCESS if SUBSCRIBE was successfully sent;
 *         EXIT_FAIL otherwise.
 */
static int32_t subscribeToTopic( MQTTContext_t * pContext );

/*-----------------------------------------------------------*/

static int32_t connectToServerWithBroker( NetworkContext_t * pNetworkContext,
                                          void * pNetworkServerInfo,
                                          void * pNetworkCredentialInfo )
{
    TransportSocketStatus_t TransportSocketStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;
    int32_t returnStatus = EXIT_SUCCESS;
    SocketsConfig_t SocketsConfig = { .enableTls = false };
    ServerInfo_t serverInfo = { .hostNameLength = 0 };
    /* Cast function parameters to correct types. */
    const IotNetworkServerInfo_t * pServerInfo = pNetworkServerInfo;
    const IotNetworkCredentials_t * pAfrCredentials = pNetworkCredentialInfo;

    /* Inilitialize SocketsConfig. */
    SocketsConfig.enableTls = true;
    SocketsConfig.pAlpnProtos = pAfrCredentials->pAlpnProtos;
    SocketsConfig.maxFragmentLength = pAfrCredentials->maxFragmentLength;
    SocketsConfig.disableSni = pAfrCredentials->disableSni;
    SocketsConfig.pRootCa = pAfrCredentials->pRootCa;
    SocketsConfig.rootCaSize = pAfrCredentials->rootCaSize;
    SocketsConfig.pClientCert = pAfrCredentials->pClientCert;
    SocketsConfig.clientCertSize = pAfrCredentials->clientCertSize;
    SocketsConfig.pPrivateKey = pAfrCredentials->pPrivateKey;
    SocketsConfig.privateKeySize = pAfrCredentials->privateKeySize;
    SocketsConfig.sendTimeoutMs = DEMO_TRANSPORT_SND_TIMEOUT_MS;
    SocketsConfig.recvTimeoutMs = DEMO_TRANSPORT_RCV_TIMEOUT_MS;
    serverInfo.pHostName = pServerInfo->pHostName;
    serverInfo.port = pServerInfo->port;
    serverInfo.hostNameLength = strlen( pServerInfo->pHostName );

    if( pNetworkServerInfo == NULL )
    {
        returnStatus = EXIT_FAILURE;
    }
    else
    {
        TransportSocketStatus = SecureSocketsTrasnport_Connect( pNetworkContext,
                                                                &serverInfo,
                                                                &SocketsConfig );
        if( TransportSocketStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
        {
            returnStatus = EXIT_FAILURE;
        }
    }

    return returnStatus;
}

/*-----------------------------------------------------------*/

static int32_t disconnectToServerWithBroker( NetworkContext_t * pNetworkContext )
{
    TransportSocketStatus_t TransportSocketStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;
    int32_t returnStatus = EXIT_SUCCESS;

    if( pNetworkContext == NULL )
    {
        returnStatus = EXIT_FAILURE;
    }
    else
    {
        TransportSocketStatus = SecureSocketsTrasnport_Disconnect( pNetworkContext );
        if( TransportSocketStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
        {
            returnStatus = EXIT_FAILURE;
        }
    }

    return returnStatus;
}

/*-----------------------------------------------------------*/

static void eventCallback( MQTTContext_t * pContext,
                           MQTTPacketInfo_t * pPacketInfo,
                           uint16_t packetIdentifier,
                           MQTTPublishInfo_t * pPublishInfo )
{
    configASSERT( pContext != NULL );
    configASSERT( pPacketInfo != NULL );

    /* Handle incoming publish. The lower 4 bits of the publish packet
     * type is used for the dup, QoS, and retain flags. Hence masking
     * out the lower bits to check if the packet is publish. */
    if( ( pPacketInfo->type & 0xF0U ) == MQTT_PACKET_TYPE_PUBLISH )
    {
        configASSERT( pPublishInfo != NULL );
        /* Handle incoming publish. */
        IotLogInfo( "Incoming Publish Topic Name: %s matches subscribed topic. Incoming Publish message Packet Id is %u",
                    pPublishInfo->pTopicName,
                    packetIdentifier );
    }
    else
    {
        IotLogInfo( "packet type received:(%02x)", pPacketInfo->type );

        /* Handle other packets. */
        switch( pPacketInfo->type )
        {
            case MQTT_PACKET_TYPE_SUBACK:

                /* Check status of the subscription request. If globalSubAckStatus does not indicate
                 * server refusal of the request (MQTTSubAckFailure), it contains the QoS level granted
                 * by the server, indicating a successful subscription attempt. */
                IotLogInfo( "PUBACK received for packet id %u", packetIdentifier );
                break;

            case MQTT_PACKET_TYPE_UNSUBACK:
                configASSERT( pPublishInfo != NULL );
                IotLogInfo( "Unsubscribed from the topic %s", pPublishInfo->pTopicName );
                break;

            case MQTT_PACKET_TYPE_PINGRESP:

                /* Nothing to be done from application as library handles
                 * PINGRESP. */
                IotLogInfo( "PINGRESP should not be handled by the application callback when using MQTT_ProcessLoop" );
                break;

            case MQTT_PACKET_TYPE_PUBACK:
                IotLogInfo( "PUBACK received for packet id %u", packetIdentifier );
                break;

            /* Any other packet type is invalid. */
            default:
                IotLogError( "Unknown packet type received:(%02x)", pPacketInfo->type );
        }
    }
}

/*-----------------------------------------------------------*/

static int32_t initializeMqtt( MQTTContext_t * pMqttContext,
                               NetworkContext_t * pNetworkContext )
{
    int32_t returnStatus = EXIT_SUCCESS;
    MQTTStatus_t mqttStatus = MQTTSuccess;
    MQTTApplicationCallbacks_t callbacks = { .getTime = NULL };
    MQTTFixedBuffer_t networkBuffer = { .pBuffer = NULL };
    TransportInterface_t transportInterface =
    {
        .send            = SecureSocketsTrasnport_Send,
        .recv            = SecureSocketsTrasnport_Recv,
        .pNetworkContext = pNetworkContext
    };

    callbacks.appCallback = eventCallback;
    callbacks.getTime = IotClock_GetTimeMs;

    assert( pMqttContext != NULL );
    assert( pNetworkContext != NULL );

    /* Fill the values for network buffer. */
    networkBuffer.pBuffer = mqttSharedBuffer;
    networkBuffer.size = NETWORK_BUFFER_SIZE;

    mqttStatus = MQTT_Init( pMqttContext, &transportInterface, &callbacks, &networkBuffer );

    if( mqttStatus != MQTTSuccess )
    {
        returnStatus = EXIT_FAILURE;
        IotLogError( "MQTT init failed: Status = %s.", MQTT_Status_strerror( mqttStatus ) );
    }

    return returnStatus;
}

/*-----------------------------------------------------------*/


static int32_t establishMqttSession( MQTTContext_t * pContext,
                                     const char * pIdentifier )
{
    int32_t status = EXIT_SUCCESS;
    MQTTStatus_t mqttStatus = MQTTSuccess;
    MQTTConnectInfo_t connectInfo = { .cleanSession = true };
    char pClientIdentifierBuffer[ CLIENT_IDENTIFIER_MAX_LENGTH ] = { 0 };
    bool sessionPresent = false;

    /* Establish MQTT session with a CONNECT packet. */
    connectInfo.cleanSession = true;

    if( ( pIdentifier != NULL ) && ( pIdentifier[ 0 ] != '\0' ) )
    {
        connectInfo.pClientIdentifier = pIdentifier;
        connectInfo.clientIdentifierLength = ( uint16_t ) strlen( pIdentifier );
    }
    else
    {
        /* Every active MQTT connection must have a unique client identifier. The demos
         * generate this unique client identifier by appending a timestamp to a common
         * prefix. */
        status = snprintf( pClientIdentifierBuffer,
                           CLIENT_IDENTIFIER_MAX_LENGTH,
                           CLIENT_IDENTIFIER_PREFIX "%lu",
                           ( long unsigned int ) IotClock_GetTimeMs() );

        /* Check for errors from snprintf. */
        if( status < 0 )
        {
            IotLogError( "Failed to generate unique client identifier for demo." );
            status = EXIT_FAILURE;
        }
        else
        {
            /* Set the client identifier buffer and length. */
            connectInfo.pClientIdentifier = pClientIdentifierBuffer;
            connectInfo.clientIdentifierLength = ( uint16_t ) status;
            status = EXIT_SUCCESS;
        }
    }

    if( status == EXIT_SUCCESS )
    {
        connectInfo.keepAliveSeconds = 60;
        connectInfo.pUserName = NULL;
        connectInfo.userNameLength = 0;
        connectInfo.pPassword = NULL;
        connectInfo.passwordLength = 0;

        IotLogInfo( "MQTT demo client identifier is %.*s (length %hu).",
                    connectInfo.clientIdentifierLength,
                    connectInfo.pClientIdentifier,
                    connectInfo.clientIdentifierLength );

        mqttStatus = MQTT_Connect( pContext, &connectInfo, NULL, RECV_TIMEOUT_MS, &sessionPresent );

        if( mqttStatus != MQTTSuccess )
        {
            status = EXIT_FAILURE;
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

static int32_t publishToTopic( MQTTContext_t * pContext )
{
    int32_t status = EXIT_SUCCESS;
    uint8_t i = 0;
    MQTTPublishInfo_t publishInfo = { .qos = 0 };


    publishInfo.qos = MQTTQoS1;
    publishInfo.pTopicName = DEMO_TOPIC;
    publishInfo.topicNameLength = sizeof( DEMO_TOPIC );
    publishInfo.retain = false;
    publishInfo.pPayload = DEMO_BUFFER;
    publishInfo.payloadLength = sizeof( DEMO_BUFFER );
    publishInfo.dup = false;

    if( pContext == NULL )
    {
        status = EXIT_FAILURE;
    }
    else
    {
        for( i = 0; i < DEFAULT_LOOP_COUNT; i++ )
        {
            status = ( int32_t ) MQTT_Publish( pContext, &publishInfo, MQTT_GetPacketId( pContext ) );
            IotLogInfo( "MQTT_Publish status %d", status );

            if( status == EXIT_SUCCESS )
            {
                status = ( int32_t ) MQTT_ProcessLoop( pContext, RECV_TIMEOUT_MS );
                IotLogInfo( "receive Puback done" );
            }

            if( status != EXIT_SUCCESS )
            {
                break;
            }
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

static int subscribeToTopic( MQTTContext_t * pMqttContext )
{
    int returnStatus = EXIT_SUCCESS;
    MQTTStatus_t mqttStatus;
    MQTTSubscribeInfo_t subscribeInfo = { .qos = 0 };

    assert( pMqttContext != NULL );

    /* This example subscribes to only one topic and uses QOS1. */
    subscribeInfo.qos = MQTTQoS1;
    subscribeInfo.pTopicFilter = DEMO_TOPIC;
    subscribeInfo.topicFilterLength = sizeof( DEMO_TOPIC );

    /* Send SUBSCRIBE packet. */
    mqttStatus = MQTT_Subscribe( pMqttContext,
                                 &subscribeInfo,
                                 1,
                                 MQTT_GetPacketId( pMqttContext ) );

    if( mqttStatus != MQTTSuccess )
    {
        IotLogError( "Failed to send SUBSCRIBE packet to broker with error = %s.",
                     MQTT_Status_strerror( mqttStatus ) );
        returnStatus = EXIT_FAILURE;
    }
    else
    {
        IotLogInfo( "SUBSCRIBE sent for topic %.*s to broker.\n\n",
                    sizeof( DEMO_TOPIC ),
                    DEMO_TOPIC );
    }

    return returnStatus;
}

/*-----------------------------------------------------------*/

int RunMQTTTransportDemoSocekts( bool awsIotMqttMode,
                                 const char * pIdentifier,
                                 void * pNetworkServerInfo,
                                 void * pNetworkCredentialInfo,
                                 const IotNetworkInterface_t * pUnused )
{
    /* Return value of this function and the exit status of this program. */
    int32_t returnStatus = EXIT_SUCCESS;
    MQTTContext_t mqttContext = { .callbacks = NULL };
    MQTTStatus_t mqttStatus = MQTTSuccess;
    NetworkContext_t networkContext = { .pContext = NULL };


    /* pUnused is un used in this demo, we are not using the IotNetworkInterface_t
     * in this demo, the demo will show how the new Transport Interface will work with the
     * new MQTT library, even though the existing demo app runner provides the legacy
     * IotNetworkInterface_t. */
    ( void ) pUnused;

    /* awsIotMqttMode is not used in this demo. */
    ( void ) awsIotMqttMode;

    /* Set the common members of the publish info.
     * Verify transport write by sending out publish info.
     * Verify transport read by receiving PUBACK with using QoS 1 for publish. */

    returnStatus = initializeMqtt( &mqttContext, &networkContext );

    if( returnStatus == EXIT_SUCCESS )
    {
        returnStatus = connectToServerWithBroker( &networkContext,
                                                  pNetworkServerInfo,
                                                  pNetworkCredentialInfo );
    }

    if( returnStatus == EXIT_SUCCESS )
    {
        IotLogInfo( "instantiate transport success, now establish mqtt connection" );
        returnStatus = establishMqttSession( &mqttContext,
                                           pIdentifier );
    }

    if( returnStatus == EXIT_SUCCESS )
    {
        returnStatus = subscribeToTopic( &mqttContext );
    }

    if( returnStatus == EXIT_SUCCESS )
    {
        /* Process incoming packet from the broker. Acknowledgment for subscription
         * ( SUBACK ) will be received here. However after sending the subscribe, the
         * client may receive a publish before it receives a subscribe ack. Since this
         * demo is subscribing to the topic to which no one is publishing, probability
         * of receiving publish message before subscribe ack is zero; but application
         * must be ready to receive any packet. This demo uses MQTT_ProcessLoop to
         * receive packet from network. */
        returnStatus = MQTT_ProcessLoop( &mqttContext, RECV_TIMEOUT_MS );

        if( mqttStatus != MQTTSuccess )
        {
            returnStatus = EXIT_FAILURE;
            IotLogError( "MQTT_ProcessLoop returned with status = %s.",
                         MQTT_Status_strerror( mqttStatus ) );
        }
    }

    if( returnStatus == TRANSPORT_SOCKET_STATUS_SUCCESS )
    {
        returnStatus = publishToTopic( &mqttContext );
    }

    if( MQTT_Disconnect( &mqttContext ) != MQTTSuccess )
    {
        IotLogError( "MQTT disconnect failed" );
    }

    if( disconnectToServerWithBroker( &networkContext ) != TRANSPORT_SOCKET_STATUS_SUCCESS )
    {
        IotLogError( "disconnectToServerWithBroker failed" );
    }

    if( returnStatus != EXIT_SUCCESS )
    {
        IotLogError( "Encountered error durring demo" );
    }
    else
    {
        IotLogInfo( "Successfully conenct to end point, Send Publish, Receive PubAck" );
    }

    IotLogInfo( "MQTT Transport over Secure Sockets Demo end with status %d", returnStatus );

    return returnStatus;
}

/*-----------------------------------------------------------*/
