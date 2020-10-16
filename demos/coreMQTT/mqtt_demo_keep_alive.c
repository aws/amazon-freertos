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
 *
 */

/*
 * Demo that shows use of the MQTT API without its keep-alive feature.
 * This demo instead implements the keep-alive functionality in the application.
 *
 * The example shown below uses this API to create MQTT messages and
 * send them over the TCP connection established using a Secure Sockets
 * based transport interface implementation.
 * It shows how the MQTT API can be used without the keep-alive feature,
 * so that the application can implements its own keep-alive functionality
 * for MQTT. The example is single threaded and uses statically allocated memory;
 * it uses QOS0, and therefore it does not implement any retransmission
 * mechanism for publish messages.
 *
 * !!! NOTE !!!
 * This MQTT demo does not authenticate the server nor the client.
 * Hence, this demo should not be used as production ready code.
 */

/* Demo specific configurations. */
#include "mqtt_demo_keep_alive_config.h"

/* Standard includes. */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* MQTT library includes. */
#include "core_mqtt.h"

/* Retry utilities include. */
#include "retry_utils.h"

/* Transport interface include. */
#include "transport_secure_sockets.h"

/* Client credential include for the default MQTT broker endpoint. */
#include "aws_clientcredential.h"

/*-----------------------------------------------------------*/

/* Compile time error for undefined configs. */

#ifndef democonfigMQTT_BROKER_ENDPOINT
    #define democonfigMQTT_BROKER_ENDPOINT    clientcredentialMQTT_BROKER_ENDPOINT
#endif

/**
 * @brief The MQTT client identifier used in this example.  Each client identifier
 * must be unique so edit as required to ensure no two clients connecting to the
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
 * @brief Timeout for receiving CONNACK packet in milliseconds.
 */
#define mqttexampleCONNACK_RECV_TIMEOUT_MS           ( 1000U )

/**
 * @brief The topic to subscribe and publish to in the example.
 *
 * The topic name starts with the client identifier to ensure that each demo
 * interacts with a unique topic name.
 */
#define mqttexampleTOPIC                             democonfigCLIENT_IDENTIFIER "/example/topic"

/**
 * @brief The number of topic filters to subscribe.
 */
#define mqttexampleTOPIC_COUNT                       ( 1 )

/**
 * @brief The MQTT message published in this example.
 */
#define mqttexampleMESSAGE                           "Hello World!"

/**
 * @brief Dimensions a file scope buffer currently used to send and receive MQTT data
 * from a socket.
 */
#define mqttexampleSHARED_BUFFER_SIZE                ( 500U )

/**
 * @brief Time to wait between each cycle of the demo implemented by
 * RunCoreMqttKeepAliveDemo().
 */
#define mqttexampleDELAY_BETWEEN_DEMO_ITERATIONS     ( pdMS_TO_TICKS( 5000U ) )

/**
 * @brief Timeout for MQTT_ReceiveLoop in milliseconds.
 */
#define mqttexampleRECEIVE_LOOP_TIMEOUT_MS           ( 500U )

/**
 * @brief Keep-alive time reported to the broker while establishing an MQTT connection.
 *
 * It is the responsibility of the client to ensure that the interval between
 * control packets being sent does not exceed the this keep-alive value. In the
 * absence of sending any other control packets, the client MUST send a
 * PINGREQ Packet.
 */
#define mqttexampleKEEP_ALIVE_TIMEOUT_SECONDS        ( 60U )

/**
 * @brief Time to wait before sending ping request to keep MQTT connection alive.
 *
 * A PINGREQ is attempted to be sent at every ( #mqttexampleKEEP_ALIVE_TIMEOUT_SECONDS / 4 )
 * seconds. This is to make sure that a PINGREQ is always sent before the timeout
 * expires in broker.
 */
#define mqttexampleKEEP_ALIVE_DELAY                  ( pdMS_TO_TICKS( ( ( mqttexampleKEEP_ALIVE_TIMEOUT_SECONDS / 4 ) * 1000 ) ) )

/**
 * @brief Delay between MQTT publishes. Note that the receive loop also has a
 * timeout, so the total time between publishes is the sum of the two delays. The
 * keep-alive delay is added here so the keep-alive timer callback executes.
 */
#define mqttexampleDELAY_BETWEEN_PUBLISHES           ( mqttexampleKEEP_ALIVE_DELAY + pdMS_TO_TICKS( 500U ) )

/**
 * @brief Transport timeout in milliseconds for transport send and receive.
 */
#define mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS    ( 200U )

/*-----------------------------------------------------------*/

/**
 * @brief A PINGREQ packet is always 2 bytes in size, defined by MQTT 3.1.1 spec.
 */
#define MQTT_PACKET_PINGREQ_SIZE    ( 2U )

/*-----------------------------------------------------------*/

#define MILLISECONDS_PER_SECOND    ( 1000U )                                                        /**< @brief Milliseconds per second. */
#define MILLISECONDS_PER_TICK      ( MILLISECONDS_PER_SECOND / configTICK_RATE_HZ )                 /**< Milliseconds per FreeRTOS tick. */

/*-----------------------------------------------------------*/

/**
 * @brief Connect to MQTT broker with reconnection retries.
 *
 * If connection fails, retry is attempted after a timeout.
 * Timeout value will exponentially increase until maximum
 * timeout value is reached or the number of attempts are exhausted.
 *
 * @param pxNetworkContext The output parameter to return the created network context.
 *
 * @return The status of the final connection attempt.
 */
static TransportSocketStatus_t prvConnectToServerWithBackoffRetries( NetworkContext_t * pxNetworkContext );

/**
 * @brief Sends an MQTT Connect packet over the already connected TCP socket.
 *
 * @param pxMQTTContext MQTT context pointer.
 * @param pxNetworkContext Network context.
 *
 * @return pdPASS if the MQTT connection was successful, pdFAIL otherwise.
 */
static BaseType_t prvCreateMQTTConnectionWithBroker( MQTTContext_t * pxMQTTContext,
                                                     NetworkContext_t * pxNetworkContext );

/**
 * @brief Function to update variable #xGlobalSubAckStatus with status
 * information from Subscribe ACK. Called by the event callback after processing
 * an incoming SUBACK packet.
 *
 * @param Server response to the subscription request.
 */
static void prvUpdateSubAckStatus( MQTTPacketInfo_t * pxPacketInfo );

/**
 * @brief Subscribes to the topic as specified in mqttexampleTOPIC at the top of
 * this file. In the case of a Subscribe ACK failure, then subscription is
 * retried using an exponential backoff strategy with jitter.
 *
 * @param pxMQTTContext MQTT context pointer.
 *
 * @return pdPASS if the subscribe was successful, pdFAIL otherwise.
 */
static BaseType_t prvMQTTSubscribeWithBackoffRetries( MQTTContext_t * pxMQTTContext );

/**
 * @brief  Publishes a message mqttexampleMESSAGE on mqttexampleTOPIC topic.
 *
 * @param pxMQTTContext MQTT context pointer.
 *
 * @return pdPASS if the publish was success, pdFAIL otherwise.
 */
static BaseType_t prvMQTTPublishToTopic( MQTTContext_t * pxMQTTContext );

/**
 * @brief Unsubscribes from the previously subscribed topic as specified
 * in mqttexampleTOPIC.
 *
 * @param pxMQTTContext MQTT context pointer.
 *
 * @return pdPASS if the unsubscribe was successful, pdFAIL otherwise.
 */
static BaseType_t prvMQTTUnsubscribeFromTopic( MQTTContext_t * pxMQTTContext );

/**
 * @brief The timer query function provided to the MQTT context.
 *
 * @return Time in milliseconds.
 */
static uint32_t prvGetTimeMs( void );

/**
 * @brief Process a response or ack to an MQTT request (PING, SUBSCRIBE
 * or UNSUBSCRIBE). This function processes PINGRESP, SUBACK, UNSUBACK
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
 * publish message.
 */
static void prvMQTTProcessIncomingPublish( MQTTPublishInfo_t * pxPublishInfo );

/**
 * @brief Check if the amount of time waiting for PINGRESP has exceeded
 * the specified timeout, then reset the keep-alive timer.
 *
 * This should only be called after a control packet has been sent.
 *
 * @param pxTimer The auto-reload software timer for handling keep-alive.
 *
 * @return The status returned by #xTimerReset.
 */
static BaseType_t prvCheckTimeoutThenResetTimer( TimerHandle_t pxTimer );

/**
 * @brief This callback is invoked through an auto-reload software timer.
 *
 * Its responsibility is to send a PINGREQ packet if a PINGRESP is not pending
 * and no control packets have been sent after some given interval.
 *
 * @param pxTimer The auto-reload software timer for handling keep-alive.
 */
static void prvKeepAliveTimerCallback( TimerHandle_t pxTimer );

/**
 * @brief The application callback function for getting the incoming publish
 * and incoming acks reported from the MQTT library.
 *
 * @param pxMQTTContext MQTT context pointer.
 * @param pxPacketInfo Packet Info pointer for the incoming packet.
 * @param pxDeserializedInfo Deserialized information from the incoming packet.
 */
static void prvEventCallback( MQTTContext_t * pxMQTTContext,
                              MQTTPacketInfo_t * pxPacketInfo,
                              MQTTDeserializedInfo_t * pxDeserializedInfo );

/*-----------------------------------------------------------*/

/**
 * @brief Static buffer used to hold MQTT messages being sent and received.
 */
static uint8_t ucSharedBuffer[ mqttexampleSHARED_BUFFER_SIZE ];

/**
 * @brief Static buffer used to hold PINGREQ messages to be sent.
 */
static uint8_t ucPingReqBuffer[ MQTT_PACKET_PINGREQ_SIZE ];

/**
 * @brief Global entry time into the application to use as a reference timestamp
 * in the #prvGetTimeMs function. #prvGetTimeMs will always return the difference
 * between the current time and the global entry time. This will reduce the chances
 * of overflow for the 32 bit unsigned integer used for holding the timestamp.
 */
static uint32_t ulGlobalEntryTimeMs;

/**
 * @brief Packet Identifier generated when Subscribe request was sent to the broker;
 * it is used to match received Subscribe ACK to the transmitted ACK.
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
    MQTTSubAckStatus_t xSubAckStatus;
} topicFilterContext_t;

/**
 * @brief An array containing the context of a SUBACK; the SUBACK status
 * of a filter is updated when the event callback processes a SUBACK.
 */
static topicFilterContext_t xTopicFilterContext[ mqttexampleTOPIC_COUNT ] =
{
    { mqttexampleTOPIC, MQTTSubAckFailure }
};

/**
 * @brief Timer to handle the MQTT keep-alive mechanism.
 */
static TimerHandle_t xKeepAliveTimer;

/**
 * @brief Storage space for xKeepAliveTimer.
 */
static StaticTimer_t xKeepAliveTimerBuffer;

/**
 * @brief Set to true when PINGREQ is sent then false when PINGRESP is received.
 */
static volatile bool xWaitingForPingResp = false;

/**
 * @brief Set to true when an error occurs in the prvKeepAliveTimerCallback().
 */
static volatile bool xKeepAliveError = false;

/**
 * @brief The last time when a PINGREQ was sent over the network.
 */
static uint32_t ulPingReqSendTimeMs;

/**
 * @brief Timeout for a pending PINGRESP from the MQTT broker.
 */
static uint32_t ulPingRespTimeoutMs = ( mqttexampleKEEP_ALIVE_TIMEOUT_SECONDS / 4 ) * MILLISECONDS_PER_SECOND;

/**
 * @brief Static buffer used to hold an MQTT PINGREQ packet for keep-alive mechanism.
 */
const static MQTTFixedBuffer_t xPingReqBuffer =
{
    .pBuffer = ucPingReqBuffer,
    .size    = MQTT_PACKET_PINGREQ_SIZE
};

/** @brief Static buffer used to hold MQTT messages being sent and received. */
static MQTTFixedBuffer_t xBuffer =
{
    .pBuffer = ucSharedBuffer,
    .size    = mqttexampleSHARED_BUFFER_SIZE
};

/*-----------------------------------------------------------*/

/**
 * @brief This example uses MQTT APIs to send and receive MQTT packets over a
 * TCP connection established using a transport interface implementation based
 * on the Secure Sockets library. This example is single threaded and uses
 * statically allocated memory. This example uses QOS0 and therefore does not
 * implement any retransmission mechanism for publish messages. This example
 * runs for democonfigMQTT_MAX_DEMO_COUNT, if the connection to the broker goes
 * down, the code tries to reconnect to the broker with an exponential backoff
 * mechanism. This example also creates a keep-alive timer task to handle
 * periodically sending a PINGREQ to the broker. Because the demo application
 * handles the MQTT keep-alive, this example uses the MQTT_RecieveLoop API
 * instead of the MQTT_ProcessLoop API used in the MQTT plaintext demo.
 */
int RunCoreMqttKeepAliveDemo( bool awsIotMqttMode,
                              const char * pIdentifier,
                              void * pNetworkServerInfo,
                              void * pNetworkCredentialInfo,
                              const void * pNetworkInterface )
{
    uint32_t ulPublishCount = 0U, ulTopicCount = 0U, ulDemoRunCount = 0U;
    const uint32_t ulMaxPublishCount = 5UL;
    NetworkContext_t xNetworkContext = { 0 };
    MQTTContext_t xMQTTContext;
    MQTTStatus_t xMQTTStatus;
    TransportSocketStatus_t xNetworkStatus;
    BaseType_t xDemoStatus = pdPASS;
    BaseType_t xIsConnectionEstablished = pdFALSE;

    /* Remove compiler warnings about unused parameters. */
    ( void ) awsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;
    ( void ) pNetworkInterface;

    ulGlobalEntryTimeMs = prvGetTimeMs();

    /* Serialize a PINGREQ packet to send upon invoking the keep-alive timer
     * callback. */
    xMQTTStatus = MQTT_SerializePingreq( &xPingReqBuffer );

    if( xMQTTStatus != MQTTSuccess )
    {
        LogError( ( "MQTT_SerializePingreq() failed with error status %s.",
                    MQTT_Status_strerror( xMQTTStatus ) ) );
        xDemoStatus = pdFAIL;
    }

    for( ; ( ulDemoRunCount < democonfigMQTT_MAX_DEMO_COUNT ) && ( xDemoStatus == pdPASS ); ulDemoRunCount++ )
    {
        /****************************** Connect. ******************************/

        /* Attempt to connect to the MQTT broker. If connection fails, retry
         * after a timeout. The timeout value will be exponentially increased
         * until the maximum number of attempts are reached or the maximum
         * timeout value is reached. The function below returns a failure status
         * if the TCP connection cannot be established to the broker after
         * the configured number of attempts. */
        xNetworkStatus = prvConnectToServerWithBackoffRetries( &xNetworkContext );

        if( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
        {
            /* Errors are logged in prvConnectToServerWithBackoffRetries. */
            xDemoStatus = pdFAIL;
        }
        else
        {
            /* Set a flag indicating a TCP connection exists. This is done to
             * disconnect if the loop exits before disconnection happens. */
            xIsConnectionEstablished = pdTRUE;
        }

        if( xDemoStatus == pdPASS )
        {
            /* Sends an MQTT Connect packet over the already connected TCP
             * socket, and waits for connection acknowledgment (CONNACK) packet. */
            LogInfo( ( "Creating an MQTT connection to %s.",
                       democonfigMQTT_BROKER_ENDPOINT ) );
            xDemoStatus = prvCreateMQTTConnectionWithBroker( &xMQTTContext,
                                                             &xNetworkContext );
        }

        if( xDemoStatus == pdPASS )
        {
            /* Create an auto-reload timer to handle keep-alive. */
            xKeepAliveTimer = xTimerCreateStatic( "KeepAliveTimer",
                                                  mqttexampleKEEP_ALIVE_DELAY,
                                                  pdTRUE,
                                                  ( void * ) &xMQTTContext.transportInterface,
                                                  prvKeepAliveTimerCallback,
                                                  &xKeepAliveTimerBuffer );

            if( xKeepAliveTimer == NULL )
            {
                LogError( ( "xTimerCreateStatic() failed to create KeepAliveTimer." ) );
                xDemoStatus = pdFAIL;
            }
        }

        if( xDemoStatus == pdPASS )
        {
            /* Start the timer for keep-alive. */
            xDemoStatus = xTimerStart( xKeepAliveTimer, 0 );

            if( xDemoStatus != pdPASS )
            {
                LogError( ( "xTimerStart() failed to start KeepAliveTimer." ) );
            }
        }

        /****************************** Subscribe. ********************************/

        if( xDemoStatus == pdPASS )
        {
            /* If server rejected the subscription request, attempt to resubscribe to
             * topic. Attempts are made according to the exponential backoff retry
             * strategy implemented in retryUtils. */
            xDemoStatus = prvMQTTSubscribeWithBackoffRetries( &xMQTTContext );
        }

        /*********************** Publish and Receive Loop. ************************/

        /* Publish messages with QOS0, send and process keep-alive messages. */
        for( ulPublishCount = 0;
             ( xDemoStatus == pdPASS ) && ( ulPublishCount < ulMaxPublishCount );
             ulPublishCount++ )
        {
            LogInfo( ( "Publish to the MQTT topic %s.", mqttexampleTOPIC ) );
            xDemoStatus = prvMQTTPublishToTopic( &xMQTTContext );

            if( xDemoStatus == pdPASS )
            {
                /* Process incoming publish echo, since application subscribed
                 * to the same topic the broker will send the publish message
                 * back to the application. */
                LogInfo( ( "Attempt to receive publish message from broker." ) );
                xMQTTStatus = MQTT_ReceiveLoop( &xMQTTContext,
                                                mqttexampleRECEIVE_LOOP_TIMEOUT_MS );

                if( xMQTTStatus != MQTTSuccess )
                {
                    LogError( ( "MQTT_ReceiveLoop() failed with status %s.",
                                MQTT_Status_strerror( xMQTTStatus ) ) );
                    xDemoStatus = pdFAIL;
                }
            }

            /* Leave Connection Idle for some time. */
            LogInfo( ( "Keeping Connection Idle..." ) );
            vTaskDelay( mqttexampleDELAY_BETWEEN_PUBLISHES );
        }

        /********************** Unsubscribe from the topic. ***********************/

        if( xDemoStatus == pdPASS )
        {
            LogInfo( ( "Unsubscribe from the MQTT topic %s.", mqttexampleTOPIC ) );
            xDemoStatus = prvMQTTUnsubscribeFromTopic( &xMQTTContext );
        }

        /* Process Incoming packet from the broker. */
        if( xDemoStatus == pdPASS )
        {
            xMQTTStatus = MQTT_ReceiveLoop( &xMQTTContext, mqttexampleRECEIVE_LOOP_TIMEOUT_MS );

            if( xMQTTStatus != MQTTSuccess )
            {
                LogError( ( "MQTT_ReceiveLoop() failed with status %s.",
                            MQTT_Status_strerror( xMQTTStatus ) ) );
                xDemoStatus = pdFAIL;
            }
        }

        /**************************** Disconnect. *****************************/

        /* Whether the demo operations preceeding are successful or not, we
         * always disconnect an open connection to free up system resources. */
        if( xIsConnectionEstablished == pdTRUE )
        {
            /* Send an MQTT disconnect packet over the already connected TCP socket.
             * There is no corresponding response for the disconnect packet. After
             * sending the disconnect, the client must close the network connection. */
            LogInfo( ( "Disconnecting the MQTT connection with %s.",
                       democonfigMQTT_BROKER_ENDPOINT ) );
            xMQTTStatus = MQTT_Disconnect( &xMQTTContext );

            if( xMQTTStatus != MQTTSuccess )
            {
                LogWarn( ( "MQTT_Disconnect() failed with status %s.",
                           MQTT_Status_strerror( xMQTTStatus ) ) );
                xDemoStatus = pdFAIL;
            }

            /* Close the network connection.  */
            xNetworkStatus = SecureSocketsTransport_Disconnect( &xNetworkContext );

            if( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
            {
                LogError( ( "SecureSocketsTransport_Disconnect() failed to close "
                            "the connection with status code %d.",
                            ( int ) xNetworkStatus ) );
                xDemoStatus = pdFAIL;
            }

            xIsConnectionEstablished = pdFALSE;
        }

        /* Always stop the keep-alive timer for the next iteration. */
        xDemoStatus = xTimerStop( xKeepAliveTimer, 0 );

        if( xDemoStatus != pdPASS )
        {
            LogError( ( "xTimerStart() failed to stop KeepAliveTimer." ) );
        }

        /* Check for errors in the keep-alive timer. */
        if( xKeepAliveError == true )
        {
            LogError( ( "An error occurred in KeepAliveTimer task." ) );
            xDemoStatus = pdFAIL;
        }

        if( xDemoStatus == pdPASS )
        {
            /* Reset the SUBACK status for each topic filter after completion of
             * the subscription request cycle. */
            for( ulTopicCount = 0; ulTopicCount < mqttexampleTOPIC_COUNT; ulTopicCount++ )
            {
                xTopicFilterContext[ ulTopicCount ].xSubAckStatus = MQTTSubAckFailure;
            }

            /* Wait for some time between two iterations to ensure that we do not
             * bombard the broker. */
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

    return ( xDemoStatus == pdPASS ) ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*-----------------------------------------------------------*/

static TransportSocketStatus_t prvConnectToServerWithBackoffRetries( NetworkContext_t * pxNetworkContext )
{
    TransportSocketStatus_t xNetworkStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;
    RetryUtilsStatus_t xRetryUtilsStatus = RetryUtilsSuccess;
    RetryUtilsParams_t xReconnectParams = { 0 };
    ServerInfo_t xServerInfo = { 0 };
    SocketsConfig_t xSocketConfig = { 0 };

    /* Initialize the MQTT broker information. */
    xServerInfo.pHostName = democonfigMQTT_BROKER_ENDPOINT;
    xServerInfo.hostNameLength = sizeof( democonfigMQTT_BROKER_ENDPOINT ) - 1U;
    xServerInfo.port = democonfigMQTT_BROKER_PORT;

    /* Set the Secure Socket configurations. */
    xSocketConfig.enableTls = false;
    xSocketConfig.sendTimeoutMs = mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS;
    xSocketConfig.recvTimeoutMs = mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS;

    /* Initialize reconnect attempts and interval. */
    RetryUtils_ParamsReset( &xReconnectParams );
    xReconnectParams.maxRetryAttempts = MAX_RETRY_ATTEMPTS;

    /* Attempt to connect to the MQTT broker. If connection fails, retry after
     * a timeout. The timeout value will exponentially increase until the maximum
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
            LogWarn( ( "Connection to the broker failed. Retrying connection with backoff and jitter." ) );
            xRetryUtilsStatus = RetryUtils_BackoffAndSleep( &xReconnectParams );
        }

        if( xRetryUtilsStatus == RetryUtilsRetriesExhausted )
        {
            LogError( ( "Connection to the broker failed, all attempts exhausted." ) );
            xNetworkStatus = TRANSPORT_SOCKET_STATUS_CONNECT_FAILURE;
        }
    } while( ( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS ) && ( xRetryUtilsStatus == RetryUtilsSuccess ) );

    return xNetworkStatus;
}
/*-----------------------------------------------------------*/

static BaseType_t prvCreateMQTTConnectionWithBroker( MQTTContext_t * pxMQTTContext,
                                                     NetworkContext_t * pxNetworkContext )
{
    MQTTStatus_t xResult;
    MQTTConnectInfo_t xConnectInfo;
    bool xSessionPresent;
    TransportInterface_t xTransport;
    BaseType_t xReturnStatus = pdPASS;

    /* Fill in Transport Interface send and receive function pointers. */
    xTransport.pNetworkContext = pxNetworkContext;
    xTransport.send = SecureSocketsTransport_Send;
    xTransport.recv = SecureSocketsTransport_Recv;

    /* Initialize MQTT library. */
    xResult = MQTT_Init( pxMQTTContext, &xTransport, prvGetTimeMs, prvEventCallback, &xBuffer );

    if( xResult != MQTTSuccess )
    {
        LogError( ( "MQTT_Init() failed with status %s.",
                    MQTT_Status_strerror( xResult ) ) );
        xReturnStatus = pdFAIL;
    }

    if( xReturnStatus == pdPASS )
    {
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

        /* Set MQTT keep-alive period. It is the responsibility of the application
         * to ensure that the interval between control packets being sent does not
         * exceed the keep-alive value. In the absence of sending any other control
         * packets, the client MUST send a PINGREQ Packet. */
        xConnectInfo.keepAliveSeconds = mqttexampleKEEP_ALIVE_TIMEOUT_SECONDS;

        /* Send MQTT CONNECT packet to broker. LWT is not used in this demo, so it
         * is passed as NULL. */
        xResult = MQTT_Connect( pxMQTTContext,
                                &xConnectInfo,
                                NULL,
                                mqttexampleCONNACK_RECV_TIMEOUT_MS,
                                &xSessionPresent );

        if( xResult != MQTTSuccess )
        {
            LogError( ( "MQTT_Connect() failed with status %s.",
                        MQTT_Status_strerror( xResult ) ) );
        }
    }

    return xReturnStatus;
}
/*-----------------------------------------------------------*/

static void prvUpdateSubAckStatus( MQTTPacketInfo_t * pxPacketInfo )
{
    MQTTStatus_t xResult = MQTTSuccess;
    uint8_t * pucPayload = NULL;
    size_t ulSize = 0;
    uint32_t ulTopicCount = 0U;

    xResult = MQTT_GetSubAckStatusCodes( pxPacketInfo, &pucPayload, &ulSize );

    /* MQTT_GetSubAckStatusCodes always returns success if called with packet info
     * from the event callback and non-NULL parameters. */
    configASSERT( xResult == MQTTSuccess );

    for( ulTopicCount = 0; ulTopicCount < ulSize; ulTopicCount++ )
    {
        xTopicFilterContext[ ulTopicCount ].xSubAckStatus = pucPayload[ ulTopicCount ];
    }
}
/*-----------------------------------------------------------*/

static BaseType_t prvMQTTSubscribeWithBackoffRetries( MQTTContext_t * pxMQTTContext )
{
    MQTTStatus_t xResult = MQTTSuccess;
    RetryUtilsStatus_t xRetryUtilsStatus = RetryUtilsSuccess;
    RetryUtilsParams_t xRetryParams;
    MQTTSubscribeInfo_t xMQTTSubscription[ mqttexampleTOPIC_COUNT ];
    bool xFailedSubscribeToTopic = false;
    uint32_t ulTopicCount = 0U;
    BaseType_t xReturnStatus = pdPASS;

    /* Some fields not used by this demo so start with everything at 0. */
    ( void ) memset( ( void * ) &xMQTTSubscription, 0x00, sizeof( xMQTTSubscription ) );

    /* Get a unique packet id. */
    usSubscribePacketIdentifier = MQTT_GetPacketId( pxMQTTContext );

    /* Subscribe to the mqttexampleTOPIC topic filter. This example subscribes to
     * only one topic and uses QoS0. */
    xMQTTSubscription[ 0 ].qos = MQTTQoS0;
    xMQTTSubscription[ 0 ].pTopicFilter = mqttexampleTOPIC;
    xMQTTSubscription[ 0 ].topicFilterLength = ( uint16_t ) strlen( mqttexampleTOPIC );

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
         * from the broker. This demo uses QOS0 in the subscribe, therefore, the publish
         * messages received from the broker will have QOS0. */
        LogInfo( ( "Attempt to subscribe to the MQTT topic %s.", mqttexampleTOPIC ) );
        xResult = MQTT_Subscribe( pxMQTTContext,
                                  xMQTTSubscription,
                                  sizeof( xMQTTSubscription ) / sizeof( MQTTSubscribeInfo_t ),
                                  usSubscribePacketIdentifier );

        if( xResult != MQTTSuccess )
        {
            LogError( ( "MQTT_Subscribe() failed with status %s.",
                        MQTT_Status_strerror( xResult ) ) );
            xReturnStatus = pdFAIL;
            break;
        }

        LogInfo( ( "SUBSCRIBE sent for topic %s to broker.\n\n", mqttexampleTOPIC ) );

        /* Process incoming packet from the broker. After sending the subscribe, the
         * client may receive a publish before it receives a subscribe ack. Therefore,
         * call generic incoming packet processing function. Since this demo is
         * subscribing to the topic to which no one is publishing, probability of
         * receiving publish message before subscribe ack is zero; but application
         * must be ready to receive any packet.  This demo uses the generic packet
         * processing function everywhere to highlight this fact. */
        xResult = MQTT_ProcessLoop( pxMQTTContext, mqttexampleRECEIVE_LOOP_TIMEOUT_MS );

        if( xResult != MQTTSuccess )
        {
            LogError( ( "MQTT_ProcessLoop() failed with status %s.",
                        MQTT_Status_strerror( xResult ) ) );
            xReturnStatus = pdFAIL;
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
            if( xTopicFilterContext[ ulTopicCount ].xSubAckStatus == MQTTSubAckFailure )
            {
                LogWarn( ( "Server rejected subscription request. Attempting to re-subscribe to topic %s.",
                           xTopicFilterContext[ ulTopicCount ].pcTopicFilter ) );
                xFailedSubscribeToTopic = true;
                xRetryUtilsStatus = RetryUtils_BackoffAndSleep( &xRetryParams );
                break;
            }
        }

        if( xRetryUtilsStatus == RetryUtilsRetriesExhausted )
        {
            LogError( ( "Failed to subscribe to the broker: Subscription request"
                        " retries exhausted. NumberOfAttempts=%lu",
                        ( unsigned long ) ( xRetryParams.maxRetryAttempts ) ) );
            xReturnStatus = pdFAIL;
        }
    } while( ( xFailedSubscribeToTopic == true ) && ( xRetryUtilsStatus == RetryUtilsSuccess ) );

    return xReturnStatus;
}
/*-----------------------------------------------------------*/

static BaseType_t prvMQTTPublishToTopic( MQTTContext_t * pxMQTTContext )
{
    MQTTStatus_t xResult;
    MQTTPublishInfo_t xMQTTPublishInfo;
    BaseType_t xReturnStatus = pdPASS;

    /* Some fields not used by this demo so start with everything at 0. */
    ( void ) memset( ( void * ) &xMQTTPublishInfo, 0x00, sizeof( xMQTTPublishInfo ) );

    /* This demo uses QoS0. */
    xMQTTPublishInfo.qos = MQTTQoS0;
    xMQTTPublishInfo.retain = false;
    xMQTTPublishInfo.pTopicName = mqttexampleTOPIC;
    xMQTTPublishInfo.topicNameLength = ( uint16_t ) strlen( mqttexampleTOPIC );
    xMQTTPublishInfo.pPayload = mqttexampleMESSAGE;
    xMQTTPublishInfo.payloadLength = strlen( mqttexampleMESSAGE );

    /* Send PUBLISH packet. Packet ID is not used for a QoS0 publish. */
    xResult = MQTT_Publish( pxMQTTContext, &xMQTTPublishInfo, 0U );

    if( xResult != MQTTSuccess )
    {
        LogError( ( "MQTT_Publish() failed with status %s.",
                    MQTT_Status_strerror( xResult ) ) );
        xReturnStatus = pdFAIL;
    }

    /* When a PUBLISH packet has been sent, the keep-alive timer can be reset. */
    xReturnStatus = prvCheckTimeoutThenResetTimer( xKeepAliveTimer );
    /* Errors are logged in prvCheckTimeoutThenResetTimer(). */

    return xReturnStatus;
}
/*-----------------------------------------------------------*/

static BaseType_t prvMQTTUnsubscribeFromTopic( MQTTContext_t * pxMQTTContext )
{
    MQTTStatus_t xResult;
    MQTTSubscribeInfo_t xMQTTSubscription[ mqttexampleTOPIC_COUNT ];
    BaseType_t xReturnStatus = pdPASS;

    /* Some fields not used by this demo so start with everything at 0. */
    ( void ) memset( ( void * ) &xMQTTSubscription, 0x00, sizeof( xMQTTSubscription ) );

    /* Get a unique packet id. */
    usSubscribePacketIdentifier = MQTT_GetPacketId( pxMQTTContext );

    /* Subscribe to the mqttexampleTOPIC topic filter. This example subscribes to
     * only one topic and uses QoS0. */
    xMQTTSubscription[ 0 ].qos = MQTTQoS0;
    xMQTTSubscription[ 0 ].pTopicFilter = mqttexampleTOPIC;
    xMQTTSubscription[ 0 ].topicFilterLength = ( uint16_t ) strlen( mqttexampleTOPIC );

    /* Get next unique packet identifier. */
    usUnsubscribePacketIdentifier = MQTT_GetPacketId( pxMQTTContext );

    /* Send UNSUBSCRIBE packet. */
    xResult = MQTT_Unsubscribe( pxMQTTContext,
                                xMQTTSubscription,
                                sizeof( xMQTTSubscription ) / sizeof( MQTTSubscribeInfo_t ),
                                usUnsubscribePacketIdentifier );

    if( xResult != MQTTSuccess )
    {
        LogError( ( "MQTT_Unsubscribe() failed with status %s.",
                    MQTT_Status_strerror( xResult ) ) );
        xReturnStatus = pdFAIL;
    }

    return xReturnStatus;
}
/*-----------------------------------------------------------*/

static void prvMQTTProcessResponse( MQTTPacketInfo_t * pxIncomingPacket,
                                    uint16_t usPacketId )
{
    uint32_t ulTopicCount = 0U;

    switch( pxIncomingPacket->type )
    {
        case MQTT_PACKET_TYPE_SUBACK:

            /* A SUBACK from the broker, containing the server response to our subscription request, has been received.
             * It contains the status code indicating server approval/rejection for the subscription to the single topic
             * requested. The SUBACK will be parsed to obtain the status code, and this status code will be stored in global
             * variable #xTopicFilterContext. */
            prvUpdateSubAckStatus( pxIncomingPacket );

            for( ulTopicCount = 0; ulTopicCount < mqttexampleTOPIC_COUNT; ulTopicCount++ )
            {
                if( xTopicFilterContext[ ulTopicCount ].xSubAckStatus != MQTTSubAckFailure )
                {
                    LogInfo( ( "Subscribed to the topic %s with maximum QoS %u.",
                               xTopicFilterContext[ ulTopicCount ].pcTopicFilter,
                               xTopicFilterContext[ ulTopicCount ].xSubAckStatus ) );
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
            xWaitingForPingResp = false;
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
    configASSERT( pxPublishInfo != NULL );

    /* Process incoming publish. */
    LogInfo( ( "Incoming QoS : %d\n", pxPublishInfo->qos ) );

    /* Verify the received publish is for the we have subscribed to. */
    if( ( pxPublishInfo->topicNameLength == strlen( mqttexampleTOPIC ) ) &&
        ( 0 == strncmp( mqttexampleTOPIC, pxPublishInfo->pTopicName, pxPublishInfo->topicNameLength ) ) )
    {
        LogInfo( ( "Incoming PUBLISH Topic Name: %.*s matches subscribed topic.\r\n"
                   "Incoming PUBLISH Message : %.*s",
                   pxPublishInfo->topicNameLength,
                   pxPublishInfo->pTopicName,
                   pxPublishInfo->payloadLength,
                   pxPublishInfo->pPayload ) );
    }
    else
    {
        LogInfo( ( "Incoming PUBLISH Topic Name: %.*s does not match subscribed topic.",
                   pxPublishInfo->topicNameLength,
                   pxPublishInfo->pTopicName ) );
    }
}

/*-----------------------------------------------------------*/

static BaseType_t prvCheckTimeoutThenResetTimer( TimerHandle_t pxTimer )
{
    uint32_t now = 0U;
    BaseType_t xStatus = pdPASS;

    if( xWaitingForPingResp == true )
    {
        now = prvGetTimeMs();

        /* Check that the PINGRESP timeout has not expired. */
        if( ( now - ulPingReqSendTimeMs ) > ulPingRespTimeoutMs )
        {
            LogError( ( "PINGRESP timeout has expired. ElapsedTime=%ul, "
                        "PingReponseTimeoutMs=%ul",
                        ( unsigned long ) now - ulPingReqSendTimeMs,
                        ( unsigned long ) ulPingRespTimeoutMs ) );
            xStatus = pdFAIL;
        }
    }

    if( xStatus == pdPASS )
    {
        xStatus = xTimerReset( pxTimer, 0 );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static void prvKeepAliveTimerCallback( TimerHandle_t pxTimer )
{
    TransportInterface_t * pxTransport;
    int32_t xTransportStatus;
    uint32_t now = 0U;

    pxTransport = ( TransportInterface_t * ) pvTimerGetTimerID( pxTimer );

    if( xWaitingForPingResp == true )
    {
        now = prvGetTimeMs();

        /* Check that the PINGRESP timeout has not expired. */
        if( ( now - ulPingReqSendTimeMs ) > ulPingRespTimeoutMs )
        {
            LogError( ( "PINGRESP timeout has expired. ElapsedTime=%ul, "
                        "PingReponseTimeoutMs=%ul",
                        ( unsigned long ) ( now - ulPingReqSendTimeMs ),
                        ( unsigned long ) ulPingRespTimeoutMs ) );
            xKeepAliveError = true;
        }
    }
    else
    {
        /* Send Ping Request to the broker. */
        LogInfo( ( "Attempt to ping the MQTT broker." ) );
        xTransportStatus = pxTransport->send( pxTransport->pNetworkContext,
                                              ( void * ) xPingReqBuffer.pBuffer,
                                              xPingReqBuffer.size );

        if( ( xTransportStatus < 0U ) ||
            ( ( size_t ) xTransportStatus != xPingReqBuffer.size ) )
        {
            LogError( ( "Failed to send the ping request. TransportSendStatus=%d",
                        xTransportStatus ) );
            xKeepAliveError = true;
        }

        ulPingReqSendTimeMs = prvGetTimeMs();
        xWaitingForPingResp = true;
    }
}

/*-----------------------------------------------------------*/

static void prvEventCallback( MQTTContext_t * pxMQTTContext,
                              MQTTPacketInfo_t * pxPacketInfo,
                              MQTTDeserializedInfo_t * pxDeserializedInfo )
{
    /* The MQTT context is not used for this demo. */
    ( void ) pxMQTTContext;

    if( ( pxPacketInfo->type & 0xF0U ) == MQTT_PACKET_TYPE_PUBLISH )
    {
        prvMQTTProcessIncomingPublish( pxDeserializedInfo->pPublishInfo );
    }
    else
    {
        prvMQTTProcessResponse( pxPacketInfo, pxDeserializedInfo->packetIdentifier );
    }
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
