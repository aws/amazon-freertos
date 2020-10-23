/*
 * FreeRTOS V202010.00
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 */

/*
 * Demo for showing use of the lightweight MQTT serializer API. The serializer
 * API allows the user to serialize and deserialize MQTT messages into a user
 * provided buffer. This API allows use of a statically allocated buffer.
 *
 * The example shown below uses this API to create MQTT messages and
 * send them over the connection established using Secure Sockets.
 * The example is single threaded and uses statically allocated memory;
 * it uses QOS0 and therefore does not implement any retransmission
 * mechanism for Publish messages.
 *
 * !!! NOTE !!!
 * This MQTT demo does not authenticate the server or the client.
 * Hence, this demo code is not recommended to be used in production
 * systems requiring secure connections.
 */

/* Demo specific configs. */
#include "mqtt_demo_serializer_config.h"

/* Standard includes. */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Secure sockets include for the TCP stack. */
#include "iot_secure_sockets.h"

/* MQTT library includes. */
#include "core_mqtt.h"

/* Retry utilities include. */
#include "retry_utils.h"

/* Client Credential include for the default broker endpoint, port, and client ID. */
#include "aws_clientcredential.h"

/*-----------------------------------------------------------*/

/* Default values for configs. */

#ifndef democonfigMQTT_BROKER_ENDPOINT
    #define democonfigMQTT_BROKER_ENDPOINT    clientcredentialMQTT_BROKER_ENDPOINT
#endif

/**
 * @brief The MQTT client identifier used in this example. Each client identifier
 * must be unique, so edit as required to ensure no two clients connecting to the
 * same broker use the same client identifier.
 */
#ifndef democonfigCLIENT_IDENTIFIER
    #define democonfigCLIENT_IDENTIFIER    clientcredentialIOT_THING_NAME
#endif

/**
 * @brief The port to use for the demo.
 */
#ifndef democonfigMQTT_BROKER_PORT
    #define democonfigMQTT_BROKER_PORT    clientcredentialMQTT_BROKER_PORT
#endif

/**
 * @brief The maximum number of times to run the subscribe publish loop in this
 * demo.
 */
#ifndef democonfigMQTT_MAX_DEMO_COUNT
    #define democonfigMQTT_MAX_DEMO_COUNT    ( 3 )
#endif

/*-----------------------------------------------------------*/

/**
 * @brief The topic to subscribe and publish to in the example.
 *
 * The topic name starts with the client identifier to ensure that each demo
 * interacts with a unique topic name.
 */
#define mqttexampleTOPIC                            democonfigCLIENT_IDENTIFIER "/example/topic"

/**
 * @brief The number of topic filters to subscribe.
 */
#define mqttexampleTOPIC_COUNT                      ( 1 )

/**
 * @brief The MQTT message published in this example.
 */
#define mqttexampleMESSAGE                          "Hello Serializer MQTT World!"

/**
 * @brief Dimensions a file scope buffer currently used to send and receive MQTT
 * data from a socket.
 */
#define mqttexampleSHARED_BUFFER_SIZE               ( 500U )

/**
 * @brief Time to wait between each cycle of the demo implemented by
 * RunCoreMqttSerializerDemo().
 */
#define mqttexampleDELAY_BETWEEN_DEMO_ITERATIONS    ( pdMS_TO_TICKS( 5000U ) )

/**
 * @brief Keep alive time reported to the broker while establishing an MQTT connection.
 *
 * It is the responsibility of the client to ensure that the interval between
 * control packets being sent does not exceed the this keep-alive value. In the
 * absence of sending any other control packets, the client MUST send a
 * PINGREQ Packet.
 */
#define mqttexampleKEEP_ALIVE_TIMEOUT_SECONDS       ( 10U )

/**
 * @brief Time to wait before sending ping request to keep MQTT connection alive.
 *
 * A PINGREQ is attempted to be sent at every ( #mqttexampleKEEP_ALIVE_TIMEOUT_SECONDS / 4 )
 * seconds. This is to make sure that a PINGREQ is always sent before the timeout
 * expires in broker.
 */
#define mqttexampleKEEP_ALIVE_DELAY                 ( pdMS_TO_TICKS( ( ( mqttexampleKEEP_ALIVE_TIMEOUT_SECONDS / 4 ) * 1000 ) ) )

/**
 * @brief Maximum number of times to call FreeRTOS_recv when initiating a
 * graceful socket shutdown.
 */
#define mqttexampleMAX_SOCKET_SHUTDOWN_LOOPS        ( 3 )

/*-----------------------------------------------------------*/

/**
 * @brief Creates a TCP connection to the MQTT broker as specified by
 * democonfigMQTT_BROKER_ENDPOINT and democonfigMQTT_BROKER_PORT defined at the
 * top of this file.
 *
 * @return A valid socket on  a successful TCP connection to the broker; otherwise,
 * SOCKETS_INVALID_SOCKET on failure.
 */
static Socket_t prvCreateTCPConnectionToBroker( void );

/**
 * @brief Connect to MQTT broker with reconnection retries.
 *
 * If connection fails, retry is attempted after a timeout.
 * Timeout value will exponentially increase until maximum
 * timeout value is reached or the number of attempts are exhausted.
 *
 * @param xMQTTSocket is a TCP socket that is connected to an MQTT broker.
 *
 * @return The socket of the final connection attempt.
 */
static Socket_t prvConnectToServerWithBackoffRetries();

/**
 * @brief Sends an MQTT Connect packet over the already connected TCP socket.
 *
 * @param xMQTTSocket is a TCP socket that is connected to an MQTT broker.
 *
 * @return pdPASS if the MQTT connection was successful, pdFAIL otherwise.
 */
static BaseType_t prvCreateMQTTConnectionWithBroker( Socket_t xMQTTSocket );

/**
 * @brief Performs a graceful shutdown and close of the socket passed in as its
 * parameter.
 *
 * @param xMQTTSocket is a TCP socket that is connected to an MQTT broker to which
 * an MQTT connection has been established.
 *
 * @return pdPASS if the graceful shutdown of the socket was successful,
 * pdFAIL otherwise.
 */
static BaseType_t prvGracefulShutDown( Socket_t xSocket );

/**
 * @brief Subscribes to the topic as specified in mqttexampleTOPIC at the top of
 * this file.
 *
 * @param xMQTTSocket is a TCP socket that is connected to an MQTT broker to which
 * an MQTT connection has been established.
 *
 * @return pdPASS if the MQTT subscription was successful, pdFAIL otherwise.
 */
static BaseType_t prvMQTTSubscribeToTopic( Socket_t xMQTTSocket );

/**
 * @brief Subscribes to the topic as specified in mqttexampleTOPIC at the top of
 * this file. In the case of a Subscribe ACK failure, then subscription is
 * retried using an exponential backoff strategy with jitter.
 *
 * @param xMQTTSocket is a TCP socket that is connected to an MQTT broker to which
 * an MQTT connection has been established.
 *
 * @return pdPASS if the MQTT subscription was successful, pdFAIL otherwise.
 */
static BaseType_t prvMQTTSubscribeWithBackoffRetries( Socket_t xMQTTSocket );

/**
 * @brief Function to update variable #xTopicFilterContext with status
 * information from Subscribe ACK. Called by the event callback after processing
 * an incoming SUBACK packet.
 *
 * @param Server response to the subscription request.
 */
static void prvMQTTUpdateSubAckStatus( MQTTPacketInfo_t * pxPacketInfo );

/**
 * @brief  Publishes a message mqttexampleMESSAGE on mqttexampleTOPIC topic.
 *
 * @param xMQTTSocket is a TCP socket that is connected to an MQTT broker to which
 * an MQTT connection has been established.
 *
 * @return pdPASS if the MQTT publish was successful, pdFAIL otherwise.
 */
static BaseType_t prvMQTTPublishToTopic( Socket_t xMQTTSocket );

/**
 * @brief Unsubscribes from the previously subscribed topic as specified
 * in mqttexampleTOPIC.
 *
 * @param xMQTTSocket is a TCP socket that is connected to an MQTT broker to which
 * an MQTT connection has been established.
 *
 * @return pdPASS if the unsubscribe was successful, pdFAIL otherwise.
 */
static BaseType_t prvMQTTUnsubscribeFromTopic( Socket_t xMQTTSocket );

/**
 * @brief Send MQTT Ping Request to the broker.
 * Ping request is used to keep connection to the broker alive.
 *
 * @param xMQTTSocket is a TCP socket that is connected to an MQTT broker to which
 * an MQTT connection has been established.
 *
 * @return pdPASS if the ping request was successful, pdFAIL otherwise.
 */
static BaseType_t prvMQTTKeepAlive( Socket_t xMQTTSocket );

/**
 * @brief Disconnect From the MQTT broker.
 *
 * @param xMQTTSocket is a TCP socket that is connected to an MQTT broker to which
 * an MQTT connection has been established.
 *
 * @return pdPASS if the disconnect was successful, pdFAIL otherwise.
 */
static BaseType_t prvMQTTDisconnect( Socket_t xMQTTSocket );

/**
 * @brief Process a response or ack to an MQTT request (PING, SUBSCRIBE
 * or UNSUBSCRIBE). This function processes PING_RESP, SUB_ACK, UNSUB_ACK
 *
 * @param pxIncomingPacket is a pointer to structure containing deserialized
 * MQTT response.
 * @param usPacketId is the packet identifier from the ack received.
 */
static void prvMQTTProcessResponse( MQTTPacketInfo_t * pxIncomingPacket,
                                    uint16_t usPacketId );

/**
 * @brief Process incoming Publish message.
 *
 * @param pxPublishInfo is a pointer to structure containing deserialized
 * Publish message.
 */
static void prvMQTTProcessIncomingPublish( MQTTPublishInfo_t * pxPublishInfo );

/**
 * @brief Receive and validate MQTT packet from the broker, determine the type
 * of the packet and processes the packet based on the type.
 *
 * @param xMQTTSocket is a TCP socket that is connected to an MQTT broker to which
 * an MQTT connection has been established.
 *
 * @return pdPASS if the MQTT packet from the broker was successfully received
 * and validated.
 */
static BaseType_t prvMQTTProcessIncomingPacket( Socket_t xMQTTSocket );

/**
 * @brief The transport receive wrapper function supplied to the MQTT library for
 * receiving type and length of an incoming MQTT packet.
 *
 * @param[in] pxContext Pointer to network context.
 * @param[out] pBuffer Buffer for receiving data.
 * @param[in] bytesToRecv Size of pBuffer.
 *
 * @return Number of bytes received or zero to indicate transportTimeout;
 * negative value on error.
 */
static int32_t prvTransportRecv( NetworkContext_t * pxContext,
                                 void * pvBuffer,
                                 size_t xBytesToRecv );

/**
 * @brief Generate and return monotonically increasing packet identifier.
 *
 * @return The next PacketId.
 *
 * @note This function is not thread safe.
 */
static uint16_t prvGetNextPacketIdentifier( void );

/*-----------------------------------------------------------*/

/* @brief Static buffer used to hold MQTT messages being sent and received. */
static uint8_t ucSharedBuffer[ mqttexampleSHARED_BUFFER_SIZE ];

/**
 * @brief Packet Identifier generated when Subscribe request was sent to the broker;
 * it is used to match received Subscribe ACK to the transmitted subscribe request.
 */
static uint16_t usSubscribePacketIdentifier;

/**
 * @brief Packet Identifier generated when Unsubscribe request was sent to the broker;
 * it is used to match received Unsubscribe response to the transmitted unsubscribe
 * request.
 */
static uint16_t usUnsubscribePacketIdentifier;

/**
 * @brief A pair containing a topic filter and its SUBACK status.
 */
typedef struct topicFilterContext
{
    const char * pcTopicFilter;
    bool xSubAckSuccess;
} topicFilterContext_t;

/**
 * @brief An array containing the context of a SUBACK; the SUBACK status
 * of a filter is updated when a SUBACK packet is processed.
 */
static topicFilterContext_t xTopicFilterContext[ mqttexampleTOPIC_COUNT ] =
{
    { mqttexampleTOPIC, false }
};


/** @brief Static buffer used to hold MQTT messages being sent and received. */
const static MQTTFixedBuffer_t xBuffer =
{
    .pBuffer = ucSharedBuffer,
    .size    = mqttexampleSHARED_BUFFER_SIZE
};

/*-----------------------------------------------------------*/

/**
 * @brief The Network Context implementation. This context is passed to the
 * transport interface functions.
 *
 * This example uses transport interface function only to read the packet type
 * and length of the incoming packet from network.
 */
struct NetworkContext
{
    Socket_t xTcpSocket;
};

/*-----------------------------------------------------------*/

int RunCoreMqttSerializerDemo( bool awsIotMqttMode,
                               const char * pIdentifier,
                               void * pNetworkServerInfo,
                               void * pNetworkCredentialInfo,
                               const void * pNetworkInterface )
{
    Socket_t xMQTTSocket = SOCKETS_INVALID_SOCKET;
    BaseType_t xStatus = pdPASS;
    uint32_t ulPublishCount = 0U, ulTopicCount = 0U;
    const uint32_t ulMaxPublishCount = 5UL;
    uint32_t ulDemoRunCount = 0;
    BaseType_t xIsConnectionEstablished = pdFALSE;

    /* Remove compiler warnings about unused parameters. */
    ( void ) awsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;
    ( void ) pNetworkInterface;

    for( ; ulDemoRunCount < democonfigMQTT_MAX_DEMO_COUNT; ulDemoRunCount++ )
    {
        /****************************** Connect. ******************************/

        /* Attempt to connect to the MQTT broker. If connection fails, retry after
         * a timeout. The timeout value will be exponentially increased until the
         * maximum number of attempts are reached or the maximum timeout value is
         * reached. The function below returns a failure status if the TCP connection
         * cannot be established to the broker after the configured number of attempts. */
        xMQTTSocket = prvConnectToServerWithBackoffRetries();

        if( xMQTTSocket == SOCKETS_INVALID_SOCKET )
        {
            xStatus = pdFAIL;
        }
        else
        {
            /* Set a flag indicating a MQTT connection exists. This is done to
             * disconnect if the loop exits before disconnection happens. */
            xIsConnectionEstablished = pdTRUE;
        }

        if( xStatus == pdPASS )
        {
            /* Sends an MQTT Connect packet over the already connected TCP socket
             * xMQTTSocket, and waits for connection acknowledgment (CONNACK) packet. */
            LogInfo( ( "Creating an MQTT connection to %s.",
                       democonfigMQTT_BROKER_ENDPOINT ) );
            xStatus = prvCreateMQTTConnectionWithBroker( xMQTTSocket );
        }

        /**************************** Subscribe. ******************************/

        if( xStatus == pdPASS )
        {
            /* If the broker rejected the subscription request, attempt to resubscribe
             * to the same topic. Attempts are made according to the exponential backofy
             * retry strategy implemented in retry_utils.h. */
            xStatus = prvMQTTSubscribeWithBackoffRetries( xMQTTSocket );
        }

        /******************* Publish and Keep Alive Loop. *********************/

        /* Publish messages with QoS0, then send and process keep alive messages. */
        for( ulPublishCount = 0;
             ( xStatus == pdPASS ) && ( ulPublishCount < ulMaxPublishCount );
             ulPublishCount++ )
        {
            LogInfo( ( "Publish to the MQTT topic %s.", mqttexampleTOPIC ) );
            xStatus = prvMQTTPublishToTopic( xMQTTSocket );

            if( xStatus == pdPASS )
            {
                /* Process the incoming publish echo. Since the application subscribed
                 * to the same topic, the broker will send the same publish message
                 * back to the application. */
                LogInfo( ( "Attempt to receive publish message from broker." ) );
                xStatus = prvMQTTProcessIncomingPacket( xMQTTSocket );
            }

            if( xStatus == pdPASS )
            {
                /* Leave the connection idle for some time */
                LogInfo( ( "Keeping Connection Idle." ) );
                vTaskDelay( mqttexampleKEEP_ALIVE_DELAY );

                /* Send a ping request to broker and receive the ping response. */
                LogInfo( ( "Sending Ping Request to the broker." ) );
                xStatus = prvMQTTKeepAlive( xMQTTSocket );
            }

            if( xStatus == pdPASS )
            {
                /* Process the incoming packet from the broker. */
                xStatus = prvMQTTProcessIncomingPacket( xMQTTSocket );
            }
        }

        /******************** Unsubscribe from the topic. *********************/

        if( xStatus == pdPASS )
        {
            LogInfo( ( "Unsubscribe from the MQTT topic %s.", mqttexampleTOPIC ) );
            xStatus = prvMQTTUnsubscribeFromTopic( xMQTTSocket );
        }

        if( xStatus == pdPASS )
        {
            /* Process the incoming packet from the broker. */
            xStatus = prvMQTTProcessIncomingPacket( xMQTTSocket );
        }

        /**************************** Disconnect. ******************************/

        /* Whether the demo operations preceeding are successful or not, we
         * always disconnect an open connection to free up system resources. */
        if( xIsConnectionEstablished == pdTRUE )

        {
            /* Send an MQTT Disconnect packet over the currently connected TCP socket.
             * There is no corresponding response for a disconnect packet. After
             * sending disconnect, the client must close the network connection. */
            LogInfo( ( "Disconnecting the MQTT connection with %s.", democonfigMQTT_BROKER_ENDPOINT ) );
            xStatus = prvMQTTDisconnect( xMQTTSocket );

            /* Close the network connection. */
            xStatus = prvGracefulShutDown( xMQTTSocket );
            xIsConnectionEstablished = pdFALSE;
        }

        if( xStatus == pdPASS )
        {
            /* Reset SUBACK status for each topic filter after completion of
             * subscription request cycle. */
            for( ulTopicCount = 0; ulTopicCount < mqttexampleTOPIC_COUNT; ulTopicCount++ )
            {
                xTopicFilterContext[ ulTopicCount ].xSubAckSuccess = false;
            }

            /* Wait for some time between two iterations to ensure that we do not
             * bombard the MQTT broker. */
            LogInfo( ( "Demo iteration %lu completed successfully.", ( ulDemoRunCount + 1UL ) ) );
            LogInfo( ( "Short delay before starting the next iteration.... " ) );
            vTaskDelay( mqttexampleDELAY_BETWEEN_DEMO_ITERATIONS );
        }
        else
        {
            LogInfo( ( "Demo failed at iteration %lu.", ( ulDemoRunCount + 1UL ) ) );
            break;
        }
    }

    return ( xStatus == pdPASS ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
/*-----------------------------------------------------------*/

static BaseType_t prvGracefulShutDown( Socket_t xSocket )
{
    BaseType_t xStatus = pdFAIL;
    int32_t lSocketStatus = ( int32_t ) SOCKETS_SOCKET_ERROR;

    /* Call Secure Sockets shutdown function to close connection. */
    lSocketStatus = SOCKETS_Shutdown( xSocket, SOCKETS_SHUT_RDWR );

    if( lSocketStatus != ( int32_t ) SOCKETS_ERROR_NONE )
    {
        LogError( ( "Failed to close connection: SOCKETS_Shutdown call failed. SocketStatus %d.", lSocketStatus ) );
    }
    else
    {
        /* Call Secure Sockets close function to close socket. */
        lSocketStatus = SOCKETS_Close( xSocket );

        if( lSocketStatus != ( int32_t ) SOCKETS_ERROR_NONE )
        {
            LogError( ( "Failed to close connection: SOCKETS_Close call failed. SocketStatus %d.", lSocketStatus ) );
        }
        else
        {
            xStatus = pdPASS;
        }
    }

    return xStatus;
}
/*-----------------------------------------------------------*/

static int32_t prvTransportRecv( NetworkContext_t * pxContext,
                                 void * pvBuffer,
                                 size_t xBytesToRecv )
{
    int32_t lResult;

    configASSERT( pxContext != NULL );

    /* Receive upto xBytesToRecv from network. */
    lResult = ( int32_t ) SOCKETS_Recv( ( Socket_t ) pxContext->xTcpSocket,
                                        pvBuffer,
                                        xBytesToRecv,
                                        0 );

    return lResult;
}
/*-----------------------------------------------------------*/

static uint16_t prvGetNextPacketIdentifier()
{
    static uint16_t usPacketId = 0;

    usPacketId++;

    /* Since 0 is invalid packet identifier value,
     * take care of it when it rolls over */
    if( usPacketId == 0 )
    {
        usPacketId = 1;
    }

    return usPacketId;
}
/*-----------------------------------------------------------*/

static Socket_t prvCreateTCPConnectionToBroker( void )
{
    Socket_t xMQTTSocket = ( Socket_t ) SOCKETS_INVALID_SOCKET;
    uint32_t ulBrokerIPAddress;
    SocketsSockaddr_t xBrokerAddress;
    int32_t lSocketStatus = SOCKETS_ERROR_NONE;

    /*  Locate the endpoint, then connect to the MQTT broker. */
    ulBrokerIPAddress = SOCKETS_GetHostByName( democonfigMQTT_BROKER_ENDPOINT );

    if( ulBrokerIPAddress != 0 )
    {
        /* This is the socket used to connect to the MQTT broker. */
        xMQTTSocket = SOCKETS_Socket( SOCKETS_AF_INET,
                                      SOCKETS_SOCK_STREAM,
                                      SOCKETS_IPPROTO_TCP );

        if( xMQTTSocket != SOCKETS_INVALID_SOCKET )
        {
            xBrokerAddress.usPort = SOCKETS_htons( democonfigMQTT_BROKER_PORT );
            xBrokerAddress.ulAddress = ulBrokerIPAddress;

            lSocketStatus = SOCKETS_Connect( xMQTTSocket, &xBrokerAddress, sizeof( xBrokerAddress ) );

            if( lSocketStatus != SOCKETS_ERROR_NONE )
            {
                LogError( ( "Could not connect to MQTT broker %s: SOCKETS_Connect() failed. SocketStatus=%d.",
                            democonfigMQTT_BROKER_ENDPOINT,
                            lSocketStatus ) );
                SOCKETS_Close( xMQTTSocket );
                xMQTTSocket = SOCKETS_INVALID_SOCKET;
            }
        }
        else
        {
            LogError( ( "Could not create TCP socket." ) );
        }
    }
    else
    {
        LogError( ( "Could not connect to broker: DNS resolution failed: Broker=%s.",
                    democonfigMQTT_BROKER_ENDPOINT ) );
    }

    return xMQTTSocket;
}
/*-----------------------------------------------------------*/

static Socket_t prvConnectToServerWithBackoffRetries()
{
    Socket_t xSocket;
    RetryUtilsStatus_t xRetryUtilsStatus = RetryUtilsSuccess;
    RetryUtilsParams_t xReconnectParams;

    /* Initialize reconnect attempts and interval. */
    RetryUtils_ParamsReset( &xReconnectParams );
    xReconnectParams.maxRetryAttempts = MAX_RETRY_ATTEMPTS;

    /* Attempt to connect to MQTT broker. If connection fails, retry after
     * a timeout. Timeout value will exponentially increase till maximum
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
        xSocket = prvCreateTCPConnectionToBroker();

        if( xSocket == SOCKETS_INVALID_SOCKET )
        {
            LogWarn( ( "Connection to the broker failed. Retrying connection with backoff and jitter." ) );
            xRetryUtilsStatus = RetryUtils_BackoffAndSleep( &xReconnectParams );
        }

        if( xRetryUtilsStatus == RetryUtilsRetriesExhausted )
        {
            LogError( ( "Connection to the broker failed, all attempts exhausted." ) );
            xSocket = SOCKETS_INVALID_SOCKET;
        }
    } while( ( xSocket == SOCKETS_INVALID_SOCKET ) && ( xRetryUtilsStatus == RetryUtilsSuccess ) );

    return xSocket;
}
/*-----------------------------------------------------------*/

static BaseType_t prvCreateMQTTConnectionWithBroker( Socket_t xMQTTSocket )
{
    BaseType_t xStatus = pdPASS;
    size_t xRemainingLength;
    size_t xPacketSize;
    MQTTStatus_t xResult;
    MQTTPacketInfo_t xIncomingPacket;
    MQTTConnectInfo_t xConnectInfo;
    uint16_t usPacketId;
    bool xSessionPresent;
    NetworkContext_t xNetworkContext;

    /* Many fields are not used in this demo so start with everything at 0. */
    memset( ( void * ) &xConnectInfo, 0x00, sizeof( xConnectInfo ) );

    /* Reset all fields of the incoming packet structure. */
    ( void ) memset( ( void * ) &xIncomingPacket, 0x00, sizeof( MQTTPacketInfo_t ) );

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

    /* Set MQTT keep-alive period. It is the responsibility of the application to ensure
     * that the interval between control Packets being sent does not exceed the keep-alive value.
     * In the absence of sending any other control packets, the client MUST send a PINGREQ Packet. */
    xConnectInfo.keepAliveSeconds = mqttexampleKEEP_ALIVE_TIMEOUT_SECONDS;

    /* Get size requirement for the connect packet.
     * Last Will and Testament is not used in this demo. It is passed as NULL. */
    xResult = MQTT_GetConnectPacketSize( &xConnectInfo,
                                         NULL,
                                         &xRemainingLength,
                                         &xPacketSize );

    /* Make sure the packet size is less than static buffer size. */
    if( xResult != MQTTSuccess )
    {
        xStatus = pdFAIL;
        LogError( ( "MQTT_GetConnectPacketSize() failed with error status %d.",
                    xResult ) );
    }

    if( xPacketSize >= mqttexampleSHARED_BUFFER_SIZE )
    {
        xStatus = pdFAIL;
        LogError( ( "The size of the MQTT CONNECT packet must be less than the "
                    "size of the demo's shared buffer. ConnectPacketSize=%lu,"
                    "SharedBufferSize=%lu",
                    ( unsigned long ) xPacketSize,
                    ( unsigned long ) mqttexampleSHARED_BUFFER_SIZE ) );
    }

    if( xStatus == pdPASS )
    {
        /* Serialize MQTT connect packet into the provided buffer. */
        xResult = MQTT_SerializeConnect( &xConnectInfo,
                                         NULL,
                                         xRemainingLength,
                                         &xBuffer );

        if( xResult != MQTTSuccess )
        {
            xStatus = pdFAIL;
            LogError( ( "MQTT_SerializeConnect() failed with error status %d.",
                        xResult ) );
        }
    }

    if( xStatus == pdPASS )
    {
        /* In this demo, for reduced complexity, SOCKET_Send() is invoked just
         * once for sending the entire MQTT CONNECT packet. In this demo, the
         * size of the MQTT CONNECT packet is the length of the client identifier
         * plus a maximum of 16 bytes. The assumption is made here that the
         * client identifier is not very long, so the probability of the single
         * socket call not sending the entire packet is low. */
        xStatus = SOCKETS_Send( xMQTTSocket,
                                ( const void * ) xBuffer.pBuffer,
                                xPacketSize,
                                0 );

        /* Replace the status, which will be the number of bytes expected to be
         * sent, with either passing or failing. */
        if( xStatus != ( BaseType_t ) xPacketSize )
        {
            LogError( ( "SOCKETS_Send() failed with return code %d.", xStatus ) );
            xStatus = pdFAIL;
        }
        else
        {
            xStatus = pdPASS;
        }
    }

    if( xStatus == pdPASS )
    {
        /* Wait for a connection acknowledgment. We cannot assume received data
         * is in the connection acknowledgment. Therefore this function reads the
         * type and remaining length of the received packet, before processing
         * the entire packet.
         */
        xNetworkContext.xTcpSocket = xMQTTSocket;

        xResult = MQTT_GetIncomingPacketTypeAndLength( prvTransportRecv,
                                                       &xNetworkContext,
                                                       &xIncomingPacket );

        if( xResult != MQTTSuccess )
        {
            xStatus = pdFAIL;
            LogError( ( "MQTT_GetIncomingPacketTypeAndLength() failed with error status %d.",
                        xResult ) );
        }

        if( xIncomingPacket.type != MQTT_PACKET_TYPE_CONNACK )
        {
            xStatus = pdFAIL;
            LogError( ( "Expected an incoming CONNACK (%u) packet, but instead "
                        "got packet type %u.",
                        MQTT_PACKET_TYPE_CONNACK,
                        xIncomingPacket.type ) );
        }

        if( xIncomingPacket.remainingLength > mqttexampleSHARED_BUFFER_SIZE )
        {
            xStatus = pdFAIL;
            LogError( ( "The length of the incoming packet must not exceed the "
                        "size of the shared demo buffer. RemainingLength=%lu, "
                        "SharedBufferSize=%lu",
                        ( unsigned long ) xIncomingPacket.remainingLength,
                        ( unsigned long ) mqttexampleSHARED_BUFFER_SIZE ) );
        }
    }

    /* Now receive the reset of the packet into the statically allocated demo
     * buffer. */
    if( xStatus == pdPASS )
    {
        /* The size of the CONNACK packet from the broker is 4 bytes. The first
         * call to MQTT_GetIncomingPacketTypeAndLength() gets the first two bytes.
         * The next two bytes are received below. Given the small size of the
         * receive, the probability of not receiving the entire rest of the
         * packet from the network is very low. */
        xStatus = SOCKETS_Recv( xMQTTSocket,
                                ( void * ) xBuffer.pBuffer,
                                xIncomingPacket.remainingLength,
                                0 );

        if( xStatus != ( BaseType_t ) xIncomingPacket.remainingLength )
        {
            LogError( ( "SOCKETS_Recv() failed with return code %d.",
                        xStatus ) );
            xStatus = pdFAIL;
        }
        else
        {
            xStatus = pdPASS;
        }
    }

    if( xStatus == pdPASS )
    {
        xIncomingPacket.pRemainingData = xBuffer.pBuffer;
        xResult = MQTT_DeserializeAck( &xIncomingPacket,
                                       &usPacketId,
                                       &xSessionPresent );

        if( xResult != MQTTSuccess )
        {
            xStatus = pdFAIL;
            LogError( ( "MQTT_DeserializeAck failed with error status %d.",
                        xResult ) );
        }
    }

    if( xStatus == pdFAIL )
    {
        LogInfo( ( "Connection with MQTT broker failed." ) );
    }

    return xStatus;
}
/*-----------------------------------------------------------*/

static BaseType_t prvMQTTSubscribeToTopic( Socket_t xMQTTSocket )
{
    MQTTStatus_t xResult = MQTTSuccess;
    size_t xRemainingLength = 0;
    size_t xPacketSize = 0;
    BaseType_t xStatus = pdPASS;
    MQTTSubscribeInfo_t xMQTTSubscription[ mqttexampleTOPIC_COUNT ];

    /* Some fields not used by this demo so start with everything at 0. */
    ( void ) memset( ( void * ) &xMQTTSubscription, 0x00, sizeof( xMQTTSubscription ) );

    /* Subscribe to the mqttexampleTOPIC topic filter. This example subscribes to
     * only one topic and uses QoS0. */
    xMQTTSubscription[ 0 ].qos = MQTTQoS0;
    xMQTTSubscription[ 0 ].pTopicFilter = mqttexampleTOPIC;
    xMQTTSubscription[ 0 ].topicFilterLength = ( uint16_t ) strlen( mqttexampleTOPIC );

    xResult = MQTT_GetSubscribePacketSize( xMQTTSubscription,
                                           sizeof( xMQTTSubscription ) / sizeof( MQTTSubscribeInfo_t ),
                                           &xRemainingLength,
                                           &xPacketSize );

    if( xResult != MQTTSuccess )
    {
        xStatus = pdFAIL;
        LogError( ( "MQTT_GetSubscribePacketSize() failed with error status %d.",
                    xResult ) );
    }

    /* Make sure the packet size is less than static buffer size. */
    if( xPacketSize >= mqttexampleSHARED_BUFFER_SIZE )
    {
        xStatus = pdFAIL;
        LogError( ( "The size of the MQTT SUBSCRIBE packet must be less than the "
                    "size of the demo's shared buffer. ConnectPacketSize=%lu,"
                    "SharedBufferSize=%lu",
                    ( unsigned long ) xPacketSize,
                    ( unsigned long ) mqttexampleSHARED_BUFFER_SIZE ) );
    }

    if( xStatus == pdPASS )
    {
        /* Get a unique packet id. */
        usSubscribePacketIdentifier = prvGetNextPacketIdentifier();

        /* Make sure the packet id obtained is valid. */
        if( usUnsubscribePacketIdentifier == 0 )
        {
            xStatus = pdFAIL;
            LogError( ( "Invalid packet identifer 0 retrieved for the unsubscribe packet." ) );
        }
    }

    /* Serialize subscribe into statically allocated ucSharedBuffer. */
    xResult = MQTT_SerializeSubscribe( xMQTTSubscription,
                                       sizeof( xMQTTSubscription ) / sizeof( MQTTSubscribeInfo_t ),
                                       usSubscribePacketIdentifier,
                                       xRemainingLength,
                                       &xBuffer );

    if( xResult != MQTTSuccess )
    {
        xStatus = pdFAIL;
        LogError( ( "MQTT_SerializeSubscribe() failed with error status %d.",
                    xResult ) );
    }

    /* Send Unsubscribe request to the broker. */
    xStatus = SOCKETS_Send( xMQTTSocket, ( void * ) xBuffer.pBuffer, xPacketSize, 0 );

    if( xStatus != ( BaseType_t ) xPacketSize )
    {
        LogError( ( "SOCKETS_Send() failed with return code %d.", xStatus ) );
        xStatus = pdFAIL;
    }
    else
    {
        xStatus = pdPASS;
    }

    return xStatus;
}
/*-----------------------------------------------------------*/

static BaseType_t prvMQTTSubscribeWithBackoffRetries( Socket_t xMQTTSocket )
{
    uint32_t ulTopicCount = 0U;
    RetryUtilsStatus_t xRetryUtilsStatus = RetryUtilsSuccess;
    RetryUtilsParams_t xRetryParams;
    bool xFailedSubscribeToTopic = false;
    BaseType_t xStatus = pdPASS;

    /* Initialize retry attempts and interval. */
    RetryUtils_ParamsReset( &xRetryParams );
    xRetryParams.maxRetryAttempts = MAX_RETRY_ATTEMPTS;

    do
    {
        /* The client is now connected to the broker. Subscribe to the topic
         * as specified in mqttexampleTOPIC at the top of this file by sending a
         * subscribe packet then waiting for a subscribe acknowledgment (SUBACK).
         * This client will then publish to the same topic it subscribed to, so it
         * will expect all the messages it sends to the broker to be sent back to it
         * from the broker. This demo uses QOS0 in Subscribe, therefore, the publish
         * messages received from the broker will have QOS0. */
        LogInfo( ( "Attempt to subscribe to the MQTT topic %s.", mqttexampleTOPIC ) );
        xStatus = prvMQTTSubscribeToTopic( xMQTTSocket );

        /* Break out of this loop on failures in sending the subscription request
         * to the broker. We retry subscriptions only if the broker was unable to
         * satisfy the subscription request. Failures to send the subscription over
         * the network or in the MQTT library API are irrecoverable with retrying. */
        if( xStatus != pdPASS )
        {
            /* Errors in subscribing are logged in prvMQTTSubscribeToTopic(). */
            break;
        }

        LogInfo( ( "SUBSCRIBE sent for topic %s to broker.\n\n", mqttexampleTOPIC ) );

        /* Process incoming packet from the broker. After sending the subscribe, the
         * client may receive a publish before it receives a subscribe ack. Therefore,
         * call generic incoming packet processing function. Since this demo is
         * subscribing to the topic to which no one is publishing, probability of
         * receiving Publish message before subscribe ack is zero; but application
         * must be ready to receive any packet.  This demo uses the generic packet
         * processing function everywhere to highlight this fact. */
        xStatus = prvMQTTProcessIncomingPacket( xMQTTSocket );

        /* Failure to process the incoming packet is a result of a failure in
         * receiving over the network or in the MQTT library API. These failures
         * are irrecoverable with retrying. */
        if( xStatus != pdPASS )
        {
            /* Errors in processing the packet are logged in prvMQTTProcessIncomingPacket(). */
            break;
        }

        /* Reset flag before checking suback responses. */
        xFailedSubscribeToTopic = false;

        /* Check if recent subscription request has been rejected. #xTopicFilterContext is updated
         * in the event callback to reflect the status of the SUBACK sent by the broker. It represents
         * either the QoS level granted by the server upon subscription, or acknowledgement of
         * server rejection of the subscription request. */
        for( ulTopicCount = 0; ulTopicCount < mqttexampleTOPIC_COUNT; ulTopicCount++ )
        {
            if( xTopicFilterContext[ ulTopicCount ].xSubAckSuccess == false )
            {
                LogWarn( ( "Broker rejected subscription request. Attempting to re-subscribe to topic %s.",
                           xTopicFilterContext[ ulTopicCount ].pcTopicFilter ) );
                xFailedSubscribeToTopic = true;
                xRetryUtilsStatus = RetryUtils_BackoffAndSleep( &xRetryParams );
                break;
            }
        }

        if( xRetryUtilsStatus == RetryUtilsRetriesExhausted )
        {
            xStatus = pdFAIL;
            LogError( ( "Failed to subscribe to the broker, subscription request"
                        " retries exhausted. NumberOfAttempts=%lu",
                        ( unsigned long ) ( xRetryParams.maxRetryAttempts ) ) );
        }
    } while( ( xFailedSubscribeToTopic == true ) && ( xRetryUtilsStatus == RetryUtilsSuccess ) );

    return xStatus;
}
/*-----------------------------------------------------------*/

static void prvMQTTUpdateSubAckStatus( MQTTPacketInfo_t * pxPacketInfo )
{
    uint8_t * pucPayload = NULL;
    uint32_t ulTopicCount = 0U;
    size_t ulSize = 0U;

    /* Defensive assertions. */

    /* Check if the pxPacketInfo contains a valid SUBACK packet. */
    configASSERT( pxPacketInfo != NULL );
    configASSERT( pxPacketInfo->type == MQTT_PACKET_TYPE_SUBACK );
    configASSERT( pxPacketInfo->pRemainingData != NULL );

    /* A SUBACK must have a remaining length of at least 3 to accommodate the
     * packet identifier and at least 1 return code. */
    configASSERT( pxPacketInfo->remainingLength >= 3U );

    /* According to the MQTT 3.1.1 protocol specification, the "Remaining Length" field is a
     * length of the variable header (2 bytes) plus the length of the payload.
     * Therefore, we add 2 positions for the starting address of the payload, and
     * subtract 2 bytes from the remaining length for the length of the payload.*/
    pucPayload = pxPacketInfo->pRemainingData + ( ( uint16_t ) sizeof( uint16_t ) );
    ulSize = pxPacketInfo->remainingLength - sizeof( uint16_t );

    for( ulTopicCount = 0; ulTopicCount < ulSize; ulTopicCount++ )
    {
        /* 0x80 denotes that the broker rejected subscription to a topic filter. */
        if( pucPayload[ ulTopicCount ] == 0x80 )
        {
            xTopicFilterContext[ ulTopicCount ].xSubAckSuccess = false;
        }
        else
        {
            xTopicFilterContext[ ulTopicCount ].xSubAckSuccess = true;
        }
    }
}
/*-----------------------------------------------------------*/

static BaseType_t prvMQTTPublishToTopic( Socket_t xMQTTSocket )
{
    MQTTStatus_t xResult = MQTTSuccess;
    MQTTPublishInfo_t xMQTTPublishInfo;
    size_t xRemainingLength = 0;
    size_t xPacketSize = 0;
    size_t xHeaderSize = 0;
    BaseType_t xStatus = pdPASS;

    /* Some fields not used by this demo so start with everything at 0. */
    ( void ) memset( ( void * ) &xMQTTPublishInfo, 0x00, sizeof( xMQTTPublishInfo ) );

    /* This demo uses QoS0. */
    xMQTTPublishInfo.qos = MQTTQoS0;
    xMQTTPublishInfo.retain = false;
    xMQTTPublishInfo.pTopicName = mqttexampleTOPIC;
    xMQTTPublishInfo.topicNameLength = ( uint16_t ) strlen( mqttexampleTOPIC );
    xMQTTPublishInfo.pPayload = mqttexampleMESSAGE;
    xMQTTPublishInfo.payloadLength = strlen( mqttexampleMESSAGE );

    /* Find out length of Publish packet size. */
    xResult = MQTT_GetPublishPacketSize( &xMQTTPublishInfo,
                                         &xRemainingLength,
                                         &xPacketSize );

    if( xResult != MQTTSuccess )
    {
        xStatus = pdFAIL;
        LogError( ( "MQTT_GetPublishPacketSize() failed with error status %d.",
                    xResult ) );
    }

    /* Make sure the packet size is less than static buffer size. */
    if( xPacketSize >= mqttexampleSHARED_BUFFER_SIZE )
    {
        xStatus = pdFAIL;
        LogError( ( "The size of the MQTT PUBLISH packet must be less than the "
                    "size of the demo's shared buffer. PublishPacketSize=%lu,"
                    "SharedBufferSize=%lu",
                    ( unsigned long ) xPacketSize,
                    ( unsigned long ) mqttexampleSHARED_BUFFER_SIZE ) );
    }

    if( xStatus == pdPASS )
    {
        /* Serialize MQTT Publish packet header. The publish message payload will
         * be sent directly in order to avoid copying it into the buffer.
         * QOS0 does not make use of packet identifier, therefore value of 0 is used */
        xResult = MQTT_SerializePublishHeader( &xMQTTPublishInfo,
                                               0,
                                               xRemainingLength,
                                               &xBuffer,
                                               &xHeaderSize );

        if( xResult != MQTTSuccess )
        {
            xStatus = pdFAIL;
            LogError( ( "MQTT_SerializePublishHeader() failed with error status %d.",
                        xResult ) );
        }
    }

    if( xStatus == pdPASS )
    {
        /* Send Publish header to the broker. */
        xStatus = SOCKETS_Send( xMQTTSocket,
                                ( void * ) xBuffer.pBuffer,
                                xHeaderSize,
                                0 );

        if( xStatus != ( BaseType_t ) xHeaderSize )
        {
            LogError( ( "SOCKETS_Send() failed with return code %d.", xStatus ) );
            xStatus = pdFAIL;
        }
        else
        {
            xStatus = pdPASS;
        }
    }

    if( xStatus == pdPASS )
    {
        /* Send Publish payload to the broker. */
        xStatus = SOCKETS_Send( xMQTTSocket,
                                ( void * ) xMQTTPublishInfo.pPayload,
                                xMQTTPublishInfo.payloadLength,
                                0 );

        if( xStatus != ( BaseType_t ) xMQTTPublishInfo.payloadLength )
        {
            LogError( ( "SOCKETS_Send() failed with return code %d.", xStatus ) );
            xStatus = pdFAIL;
        }
        else
        {
            xStatus = pdPASS;
        }
    }

    return xStatus;
}
/*-----------------------------------------------------------*/

static BaseType_t prvMQTTUnsubscribeFromTopic( Socket_t xMQTTSocket )
{
    MQTTStatus_t xResult;
    size_t xRemainingLength;
    size_t xPacketSize;
    BaseType_t xStatus = pdPASS;
    MQTTSubscribeInfo_t xMQTTSubscription[ 1 ];

    /* Some fields not used by this demo so start with everything at 0. */
    ( void ) memset( ( void * ) &xMQTTSubscription, 0x00, sizeof( xMQTTSubscription ) );

    /* Subscribe to the mqttexampleTOPIC topic filter. This example subscribes to
     * only one topic and uses QoS0. */
    xMQTTSubscription[ 0 ].qos = MQTTQoS0;
    xMQTTSubscription[ 0 ].pTopicFilter = mqttexampleTOPIC;
    xMQTTSubscription[ 0 ].topicFilterLength = ( uint16_t ) strlen( mqttexampleTOPIC );


    xResult = MQTT_GetUnsubscribePacketSize( xMQTTSubscription,
                                             sizeof( xMQTTSubscription ) / sizeof( MQTTSubscribeInfo_t ),
                                             &xRemainingLength,
                                             &xPacketSize );

    if( xResult != MQTTSuccess )
    {
        xStatus = pdFAIL;
        LogError( ( "MQTT_GetUnsubscribePacketSize() failed with error status %d.",
                    xResult ) );
    }

    /* Make sure the packet size is less than static buffer size */
    if( xPacketSize >= mqttexampleSHARED_BUFFER_SIZE )
    {
        xStatus = pdFAIL;
        LogError( ( "The size of the MQTT UNSUBSCRIBE packet must be less than the "
                    "size of the demo's shared buffer. UnsubscribePacketSize=%lu,"
                    "SharedBufferSize=%lu",
                    ( unsigned long ) xPacketSize,
                    ( unsigned long ) mqttexampleSHARED_BUFFER_SIZE ) );
    }

    if( xStatus == pdPASS )
    {
        /* Get next unique packet identifier */
        usUnsubscribePacketIdentifier = prvGetNextPacketIdentifier();

        /* Make sure the packet id obtained is valid. */
        if( usUnsubscribePacketIdentifier == 0 )
        {
            xStatus = pdFAIL;
            LogError( ( "Invalid packet identifer 0 retrieved for the unsubscribe packet." ) );
        }
    }

    xResult = MQTT_SerializeUnsubscribe( xMQTTSubscription,
                                         sizeof( xMQTTSubscription ) / sizeof( MQTTSubscribeInfo_t ),
                                         usUnsubscribePacketIdentifier,
                                         xRemainingLength,
                                         &xBuffer );

    if( xResult != MQTTSuccess )
    {
        xStatus = pdFAIL;
        LogError( ( "MQTT_SerializeUnsubscribe() failed with error status %d.",
                    xResult ) );
    }

    /* Send Unsubscribe request to the broker. */
    xStatus = SOCKETS_Send( xMQTTSocket, ( void * ) xBuffer.pBuffer, xPacketSize, 0 );

    if( xStatus != ( BaseType_t ) xPacketSize )
    {
        LogError( ( "SOCKETS_Send() failed with return code %d.", xStatus ) );
        xStatus = pdFAIL;
    }
    else
    {
        xStatus = pdPASS;
    }

    return xStatus;
}
/*-----------------------------------------------------------*/

static BaseType_t prvMQTTKeepAlive( Socket_t xMQTTSocket )
{
    MQTTStatus_t xResult;
    BaseType_t xStatus = pdPASS;
    size_t xPacketSize;

    /* Calculate PING request size. */
    xResult = MQTT_GetPingreqPacketSize( &xPacketSize );

    if( xResult != MQTTSuccess )
    {
        xStatus = pdFAIL;
        LogError( ( "MQTT_GetPingreqPacketSize() failed with error status %d.",
                    xResult ) );
    }

    if( xPacketSize > mqttexampleSHARED_BUFFER_SIZE )
    {
        xStatus = pdFAIL;
        LogError( ( "The length of the ping packet must not exceed the "
                    "size of the shared demo buffer. PingPacketSize=%lu, "
                    "SharedBufferSize=%lu",
                    ( unsigned long ) xPacketSize,
                    ( unsigned long ) mqttexampleSHARED_BUFFER_SIZE ) );
    }

    if( xStatus == pdPASS )
    {
        xResult = MQTT_SerializePingreq( &xBuffer );

        if( xResult != MQTTSuccess )
        {
            xStatus = pdFAIL;
            LogError( ( "MQTT_SerializePingreq() failed with error status %d.",
                        xResult ) );
        }
    }

    /* Send Ping Request to the broker. */
    if( xStatus == pdPASS )
    {
        xStatus = SOCKETS_Send( xMQTTSocket,
                                ( void * ) xBuffer.pBuffer,
                                xPacketSize,
                                0 );

        if( xStatus != ( BaseType_t ) xPacketSize )
        {
            LogError( ( "SOCKETS_Send() failed with return code %d.", xStatus ) );
            xStatus = pdFAIL;
        }
        else
        {
            xStatus = pdPASS;
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prvMQTTDisconnect( Socket_t xMQTTSocket )
{
    MQTTStatus_t xResult;
    BaseType_t xStatus;
    size_t xPacketSize;

    /* Calculate DISCONNECT packet size. */
    xResult = MQTT_GetDisconnectPacketSize( &xPacketSize );

    if( xResult != MQTTSuccess )
    {
        xStatus = pdFAIL;
        LogError( ( "MQTT_GetDisconnectPacketSize() failed with error status %d.",
                    xResult ) );
    }

    if( xPacketSize >= mqttexampleSHARED_BUFFER_SIZE )
    {
        xStatus = pdFAIL;
        LogError( ( "The size of the MQTT DISCONNECT packet must be less than the "
                    "size of the demo's shared buffer. ConnectPacketSize=%lu,"
                    "SharedBufferSize=%lu",
                    ( unsigned long ) xPacketSize,
                    ( unsigned long ) mqttexampleSHARED_BUFFER_SIZE ) );
    }

    xResult = MQTT_SerializeDisconnect( &xBuffer );

    if( xResult != MQTTSuccess )
    {
        xStatus = pdFAIL;
        LogError( ( "MQTT_SerializeDisconnect() failed with error status %d.",
                    xResult ) );
    }

    xStatus = SOCKETS_Send( xMQTTSocket,
                            ( void * ) xBuffer.pBuffer,
                            xPacketSize,
                            0 );

    if( xStatus != ( BaseType_t ) xPacketSize )
    {
        LogError( ( "SOCKETS_Send() failed with return code %d.", xStatus ) );
        xStatus = pdFAIL;
    }
    else
    {
        xStatus = pdPASS;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static void prvMQTTProcessResponse( MQTTPacketInfo_t * pxIncomingPacket,
                                    uint16_t usPacketId )
{
    uint32_t ulTopicCount = 0U;

    switch( pxIncomingPacket->type )
    {
        case MQTT_PACKET_TYPE_SUBACK:

            /* Check if recent subscription request has been accepted. #xTopicFilterContext is updated
             * in #prvMQTTProcessIncomingPacket to reflect the status of the SUBACK sent by the broker. */
            for( ulTopicCount = 0; ulTopicCount < mqttexampleTOPIC_COUNT; ulTopicCount++ )
            {
                if( xTopicFilterContext[ ulTopicCount ].xSubAckSuccess != false )
                {
                    LogInfo( ( "Subscribed to the topic %s with maximum QoS %u.",
                               xTopicFilterContext[ ulTopicCount ].pcTopicFilter,
                               xTopicFilterContext[ ulTopicCount ].xSubAckSuccess ) );
                }
            }

            /* Make sure ACK packet identifier matches with Request packet identifier. */
            configASSERT( usSubscribePacketIdentifier == usPacketId );
            break;

        case MQTT_PACKET_TYPE_UNSUBACK:
            LogInfo( ( "Unsubscribed from the topic %s.", mqttexampleTOPIC ) );
            /* Make sure ACK packet identifier matches with Request packet identifier. */
            configASSERT( usUnsubscribePacketIdentifier == usPacketId );
            break;

        case MQTT_PACKET_TYPE_PINGRESP:
            LogInfo( ( "Ping Response successfully received." ) );
            break;

        /* Any other packet type is invalid. */
        default:
            LogWarn( ( "prvMQTTProcessResponse() called with unknown packet type:(%02X).",
                       pxIncomingPacket->type ) );
    }
}

/*-----------------------------------------------------------*/

static void prvMQTTProcessIncomingPublish( MQTTPublishInfo_t * pxPublishInfo )
{
    /* The input pxPublishInfo should never be NULL. */
    configASSERT( pxPublishInfo != NULL );

    /* Process incoming Publish. */
    LogInfo( ( "Incoming QoS : %d\n", pxPublishInfo->qos ) );

    /* Verify the received publish is for the we have subscribed to. */
    if( ( pxPublishInfo->topicNameLength == strlen( mqttexampleTOPIC ) ) &&
        ( 0 == strncmp( mqttexampleTOPIC,
                        pxPublishInfo->pTopicName,
                        pxPublishInfo->topicNameLength ) ) )
    {
        LogInfo( ( "Incoming Publish Topic Name: %.*s matches subscribed topic.\r\n"
                   "Incoming Publish Message : %.*s",
                   pxPublishInfo->topicNameLength,
                   pxPublishInfo->pTopicName,
                   pxPublishInfo->payloadLength,
                   pxPublishInfo->pPayload ) );
    }
    else
    {
        LogInfo( ( "Incoming Publish Topic Name: %.*s does not match subscribed topic.",
                   pxPublishInfo->topicNameLength,
                   pxPublishInfo->pTopicName ) );
    }
}

/*-----------------------------------------------------------*/

static BaseType_t prvMQTTProcessIncomingPacket( Socket_t xMQTTSocket )
{
    MQTTStatus_t xResult;
    MQTTPacketInfo_t xIncomingPacket;
    BaseType_t xStatus = pdPASS;
    MQTTPublishInfo_t xPublishInfo;
    uint16_t usPacketId;
    NetworkContext_t xNetworkContext;

    ( void ) memset( ( void * ) &xIncomingPacket, 0x00, sizeof( MQTTPacketInfo_t ) );

    /* Determine incoming packet type and remaining length. */
    xNetworkContext.xTcpSocket = xMQTTSocket;

    xResult = MQTT_GetIncomingPacketTypeAndLength( prvTransportRecv,
                                                   &xNetworkContext,
                                                   &xIncomingPacket );

    if( xResult != MQTTNoDataAvailable )
    {
        if( xResult != MQTTSuccess )
        {
            xStatus = pdFAIL;
            LogError( ( "MQTT_GetIncomingPacketTypeAndLength() failed with error status %d.",
                        xResult ) );
        }

        if( xIncomingPacket.remainingLength > mqttexampleSHARED_BUFFER_SIZE )
        {
            xStatus = pdFAIL;
            LogError( ( "The length of the incoming packet must not exceed the "
                        "size of the shared demo buffer. RemainingLength=%lu, "
                        "SharedBufferSize=%lu",
                        ( unsigned long ) xIncomingPacket.remainingLength,
                        ( unsigned long ) mqttexampleSHARED_BUFFER_SIZE ) );
        }

        /* Current implementation expects an incoming Publish and three different
         * responses ( SUBACK, PINGRESP and UNSUBACK ). */

        /* Receive the remaining bytes. In case of PINGRESP, remaining length
         * will be zero. Skip reading from network for a remaining length of zero. */
        if( ( xStatus == pdPASS ) && ( xIncomingPacket.remainingLength > 0 ) )
        {
            xStatus = SOCKETS_Recv( xMQTTSocket,
                                    ( void * ) xBuffer.pBuffer,
                                    xIncomingPacket.remainingLength,
                                    0 );

            if( xStatus != ( BaseType_t ) xIncomingPacket.remainingLength )
            {
                LogError( ( "SOCKETS_Recv() failed with return code %d.",
                            xStatus ) );
                xStatus = pdFAIL;
            }
            else
            {
                xStatus = pdPASS;
                xIncomingPacket.pRemainingData = xBuffer.pBuffer;
            }
        }

        if( xStatus == pdPASS )
        {
            /* Check if the incoming packet is a publish packet. */
            if( ( xIncomingPacket.type & 0xf0 ) == MQTT_PACKET_TYPE_PUBLISH )
            {
                xResult = MQTT_DeserializePublish( &xIncomingPacket, &usPacketId, &xPublishInfo );

                if( xResult != MQTTSuccess )
                {
                    xStatus = pdFAIL;
                    LogError( ( "MQTT_DeserializePublish() failed with error status %d.",
                                xResult ) );
                }
                else
                {
                    /* Process the incoming publish message. */
                    prvMQTTProcessIncomingPublish( &xPublishInfo );
                }
            }
            else
            {
                /* If the received packet is not a Publish message, then it is an ACK for one
                 * of the messages we sent out, verify that the ACK packet is a valid MQTT
                 * packet. Session present is only valid for a CONNACK. CONNACK is not
                 * expected to be received here. Hence pass NULL for pointer to session
                 * present. */
                xResult = MQTT_DeserializeAck( &xIncomingPacket, &usPacketId, NULL );

                /* #MQTTServerRefused is returned when the broker refuses the client
                 * to subscribe to a specific topic filter. */
                if( ( xResult != MQTTSuccess ) && ( xResult != MQTTServerRefused ) )
                {
                    xStatus = pdFAIL;
                    LogError( ( "MQTT_DeserializeAck() failed with error status %d.",
                                xResult ) );
                }

                if( ( xStatus == pdPASS ) &&
                    ( xIncomingPacket.type == MQTT_PACKET_TYPE_SUBACK ) )
                {
                    prvMQTTUpdateSubAckStatus( &xIncomingPacket );
                }
                else
                {
                    /* if the packet type is not a SUBACK, then the only valid result
                     * from MQTT_DeserializeAck() is MQTTSuccess. */
                    if( xResult != MQTTSuccess )
                    {
                        xStatus = pdFAIL;
                        LogError( ( "MQTT_DeserializeAck() failed with error status %d.",
                                    xResult ) );
                    }
                }

                /* Process the response. */
                prvMQTTProcessResponse( &xIncomingPacket, usPacketId );
            }
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/
