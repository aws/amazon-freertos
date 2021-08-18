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
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/*
 * This demo creates multiple tasks, all of which use the MQTT agent API to
 * communicate with an MQTT broker through the same MQTT connection.
 *
 * This file contains the initial task created after the TCP/IP stack connects
 * to the network.  The task:
 *
 * 1) Connects to the MQTT broker.
 * 2) Creates the other demo tasks, in accordance with the #defines set in
 *    demo_config.h.  For example, if demo_config.h contains the following
 *    setting:
 *
 *    #define democonfigNUM_SIMPLE_SUB_PUB_TASKS_TO_CREATE 3
 *
 *    then the initial task will create three instances of the task
 *    implemented in simple_sub_pub_demo.c.  See the comments at the top
 *    of that file for more information.
 *
 * 3) After creating the demo tasks the initial task will create the MQTT
 *    agent task.
 */


/* Standard includes. */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* Demo Specific configs. */
#include "mqtt_agent_demo_config.h"

/* MQTT agent include. */
#include "core_mqtt_agent.h"

/* MQTT Agent ports. */
#include "freertos_agent_message.h"
#include "freertos_command_pool.h"

/* Exponential backoff retry include. */
#include "backoff_algorithm.h"

/* Include PKCS11 helpers header. */
#include "pkcs11_helpers.h"

/* Subscription manager header include. */
#include "subscription_manager.h"

/* Transport interface implementation include header for TLS. */
#include "transport_secure_sockets.h"

/* Credentials for the MQTT broker endpoint. */
#include "aws_clientcredential.h"

/* Include header for root CA certificates. */
#include "iot_default_root_certificates.h"

/* Include AWS IoT metrics macros header. */
#include "aws_iot_metrics.h"

/**
 * These configuration settings are required to run the demo.
 */
#ifndef democonfigCLIENT_IDENTIFIER

/**
 * @brief The MQTT client identifier used in this example.  Each client identifier
 * must be unique so edit as required to ensure no two clients connecting to the
 * same broker use the same client identifier.
 */
    #define democonfigCLIENT_IDENTIFIER    clientcredentialIOT_THING_NAME
#endif

/* Provide default values for undefined configs. */
#ifndef democonfigMQTT_BROKER_ENDPOINT
    #define democonfigMQTT_BROKER_ENDPOINT    clientcredentialMQTT_BROKER_ENDPOINT
#endif

#ifndef democonfigMQTT_BROKER_PORT
    #define democonfigMQTT_BROKER_PORT    clientcredentialMQTT_BROKER_PORT
#endif

#ifndef democonfigROOT_CA_PEM
    #define democonfigROOT_CA_PEM    tlsATS1_ROOT_CERTIFICATE_PEM
#endif

/* This demo uses compile time options to select the demo tasks to created.
 * Ensure the compile time options are defined.  These should be defined in
 * demo_config.h. */
#if !defined( democonfigNUM_SIMPLE_SUB_PUB_TASKS_TO_CREATE ) || ( democonfigNUM_SIMPLE_SUB_PUB_TASKS_TO_CREATE < 1 )
    #error Please set democonfigNUM_SIMPLE_SUB_PUB_TASKS_TO_CREATE to the number of tasks to create in vStartSimpleSubscribePublishTask().
#endif

#if ( democonfigNUM_SIMPLE_SUB_PUB_TASKS_TO_CREATE > 0 ) && !defined( democonfigSIMPLE_SUB_PUB_TASK_STACK_SIZE )
    #error Please define democonfigSIMPLE_SUB_PUB_TASK_STACK_SIZE in demo_config.h to set the stack size (in words, not bytes) for the tasks created by vStartSimpleSubscribePublishTask().
#endif

/**
 * @brief Dimensions the buffer used to serialize and deserialize MQTT packets.
 * @note Specified in bytes.  Must be large enough to hold the maximum
 * anticipated MQTT payload.
 */
#ifndef MQTT_AGENT_NETWORK_BUFFER_SIZE
    #define MQTT_AGENT_NETWORK_BUFFER_SIZE    ( 5000 )
#endif

/**
 * @brief The length of the queue used to hold commands for the agent.
 */
#ifndef MQTT_AGENT_COMMAND_QUEUE_LENGTH
    #define MQTT_AGENT_COMMAND_QUEUE_LENGTH    ( 10 )
#endif

/**
 * @brief Length of client identifier.
 */
#define democonfigCLIENT_IDENTIFIER_LENGTH    ( ( uint16_t ) ( sizeof( democonfigCLIENT_IDENTIFIER ) - 1 ) )

/**
 * @brief Length of MQTT server host name.
 */
#define democonfigBROKER_ENDPOINT_LENGTH      ( ( uint16_t ) ( sizeof( democonfigMQTT_BROKER_ENDPOINT ) - 1 ) )

/**
 * These configuration settings are required to run the demo.
 */

/**
 * @brief Timeout for receiving CONNACK after sending an MQTT CONNECT packet.
 * Defined in milliseconds.
 */
#define mqttexampleCONNACK_RECV_TIMEOUT_MS           ( 1000U )

/**
 * @brief The maximum number of retries for network operation with server.
 */
#define RETRY_MAX_ATTEMPTS                           ( 5U )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying failed operation
 *  with server.
 */
#define RETRY_MAX_BACKOFF_DELAY_MS                   ( 5000U )

/**
 * @brief The base back-off delay (in milliseconds) to use for network operation retry
 * attempts.
 */
#define RETRY_BACKOFF_BASE_MS                        ( 500U )

/**
 * @brief The maximum time interval in seconds which is allowed to elapse
 *  between two Control Packets.
 *
 *  It is the responsibility of the Client to ensure that the interval between
 *  Control Packets being sent does not exceed the this Keep Alive value. In the
 *  absence of sending any other Control Packets, the Client MUST send a
 *  PINGREQ Packet.
 *//*_RB_ Move to be the responsibility of the agent. */
#define mqttexampleKEEP_ALIVE_INTERVAL_SECONDS       ( 60U )

/**
 * @brief Socket send and receive timeouts to use.  Specified in milliseconds.
 */
#define mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS    ( 750 )

/**
 * @brief Used to convert times to/from ticks and milliseconds.
 */
#define mqttexampleMILLISECONDS_PER_SECOND           ( 1000U )
#define mqttexampleMILLISECONDS_PER_TICK             ( mqttexampleMILLISECONDS_PER_SECOND / configTICK_RATE_HZ )

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

/**
 * @brief Parameters for subscribe-publish tasks.
 */
struct DemoParams
{
    uint32_t ulTaskNumber;
    bool xSuccess;
};

/*-----------------------------------------------------------*/

/**
 * @brief Initializes an MQTT Agent context, including transport interface,
 * network buffer, and publish callback.
 *
 * @return `MQTTSuccess` if the initialization succeeds, else `MQTTBadParameter`.
 */
static MQTTStatus_t prvMQTTAgentInit( void );

/**
 * @brief Sends an MQTT Connect packet over the already connected TCP socket.
 *
 * @param[in] pxMQTTContext MQTT context pointer.
 * @param[in] xCleanSession If a clean session should be established.
 *
 * @return `MQTTSuccess` if connection succeeds, else appropriate error code
 * from MQTT_Connect.
 */
static MQTTStatus_t prvMQTTConnect( bool xCleanSession );

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
 * @brief Connect a TCP socket to the MQTT broker.
 *
 * @param[in] pxNetworkContext Network context.
 *
 * @return `pdPASS` if connection succeeds, else `pdFAIL`.
 */
static BaseType_t prvSocketConnect( NetworkContext_t * pxNetworkContext );

/**
 * @brief Disconnect a TCP connection.
 *
 * @param[in] pxNetworkContext Network context.
 *
 * @return `pdPASS` if disconnect succeeds, else `pdFAIL`.
 */
static BaseType_t prvSocketDisconnect( NetworkContext_t * pxNetworkContext );

/**
 * @brief Fan out the incoming publishes to the callbacks registered by different
 * tasks. If there are no callbacks registered for the incoming publish, it will be
 * passed to the unsolicited publish handler.
 *
 * @param[in] pMqttAgentContext Agent context.
 * @param[in] packetId Packet ID of publish.
 * @param[in] pxPublishInfo Info of incoming publish.
 */
static void prvIncomingPublishCallback( MQTTAgentContext_t * pMqttAgentContext,
                                        uint16_t packetId,
                                        MQTTPublishInfo_t * pxPublishInfo );

/**
 * @brief Function to attempt to resubscribe to the topics already present in the
 * subscription list.
 *
 * This function will be invoked when this demo requests the broker to
 * reestablish the session and the broker cannot do so. This function will
 * enqueue commands to the MQTT Agent queue and will be processed once the
 * command loop starts.
 *
 * @return `MQTTSuccess` if adding subscribes to the command queue succeeds, else
 * appropriate error code from MQTTAgent_Subscribe.
 * */
static MQTTStatus_t prvHandleResubscribe( void );

/**
 * @brief Passed into MQTTAgent_Subscribe() as the callback to execute when the
 * broker ACKs the SUBSCRIBE message. This callback implementation is used for
 * handling the completion of resubscribes. Any topic filter failed to resubscribe
 * will be removed from the subscription list.
 *
 * See https://freertos.org/mqtt/mqtt-agent-demo.html#example_mqtt_api_call
 *
 * @param[in] pxCommandContext Context of the initial command.
 * @param[in] pxReturnInfo The result of the command.
 */
static void prvSubscriptionCommandCallback( MQTTAgentCommandContext_t * pxCommandContext,
                                            MQTTAgentReturnInfo_t * pxReturnInfo );

/**
 * @brief Task used to run the MQTT agent.  In this example the first task that
 * is created is responsible for creating all the other demo tasks.  Then,
 * rather than create prvMQTTAgentTask() as a separate task, it simply calls
 * prvMQTTAgentTask() to become the agent task itself.
 *
 * This task calls MQTTAgent_CommandLoop() in a loop, until MQTTAgent_Terminate()
 * is called. If an error occurs in the command loop, then it will reconnect the
 * TCP and MQTT connections.
 *
 * @param[in] pvParameters Parameters as passed at the time of task creation. Not
 * used in this example.
 */
static void prvMQTTAgentTask( void * pvParameters );

/**
 * @brief The main task used in the MQTT demo.
 *
 * This task creates the network connection and all other demo tasks. Then,
 * rather than create prvMQTTAgentTask() as a separate task, it simply calls
 * prvMQTTAgentTask() to become the agent task itself.
 *
 * @param[in] pvParameters Parameters as passed at the time of task creation. Not
 * used in this example.
 *
 * @return EXIT_SUCCESS if demo completes successfully, else EXIT_FAILURE.
 */
static int prvConnectAndCreateDemoTasks( void * pvParameters );

/**
 * @brief The timer query function provided to the MQTT context.
 *
 * @return Time in milliseconds.
 */
static uint32_t prvGetTimeMs( void );

/**
 * @brief Connects a TCP socket to the MQTT broker, then creates and MQTT
 * connection to the same.
 *
 * @param[in] xCreateCleanSession Whether to create a clean session.
 */
static BaseType_t prvConnectToMQTTBroker( bool xCreateCleanSession );

/*
 * Function that starts the tasks demonstrated by this project.
 */
extern void vStartSimpleSubscribePublishTask( uint32_t ulTaskNumber,
                                              configSTACK_DEPTH_TYPE uxStackSize,
                                              UBaseType_t uxPriority,
                                              struct DemoParams * pxParams );

/*-----------------------------------------------------------*/

/**
 * @brief The network context used by the MQTT library transport interface.
 * See https://www.freertos.org/network-interface.html
 */
static NetworkContext_t xNetworkContext;

/**
 * @brief The parameters for the network context using a TLS channel.
 */
static SecureSocketsTransportParams_t secureSocketsTransportParams;

/**
 * @brief Global entry time into the application to use as a reference timestamp
 * in the #prvGetTimeMs function. #prvGetTimeMs will always return the difference
 * between the current time and the global entry time. This will reduce the chances
 * of overflow for the 32 bit unsigned integer used for holding the timestamp.
 */
static uint32_t ulGlobalEntryTimeMs;

/**
 * @brief Global MQTT Agent context used by every task.
 */
MQTTAgentContext_t xGlobalMqttAgentContext;

/**
 * @brief Network buffer for coreMQTT.
 */
static uint8_t xNetworkBuffer[ MQTT_AGENT_NETWORK_BUFFER_SIZE ];

/**
 * @brief Message queue used to deliver commands to the agent task.
 */
static MQTTAgentMessageContext_t xCommandQueue;

/**
 * @brief Structs to hold input and output parameters for each subscribe-publish task.
 */
static struct DemoParams taskParameters[ democonfigNUM_SIMPLE_SUB_PUB_TASKS_TO_CREATE ];

/**
 * @brief The global array of subscription elements.
 *
 * @note No thread safety is required to this array, since updates to the array
 * elements are done only from the MQTT agent task. The subscription manager
 * implementation expects that the array of the subscription elements used for
 * storing subscriptions to be initialized to 0. As this is a global array, it
 * will be initialized to 0 by default.
 */
SubscriptionElement_t xGlobalSubscriptionList[ SUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS ];

/*-----------------------------------------------------------*/

/*
 * @brief Create the task that demonstrates the sharing of an MQTT connection
 * using the coreMQTT Agent library.
 */
int RunCoreMqttAgentDemo( bool awsIotMqttMode,
                          const char * pIdentifier,
                          void * pNetworkServerInfo,
                          void * pNetworkCredentialInfo,
                          const void * pNetworkInterface )
{
    BaseType_t xNetworkStatus = pdFAIL;
    BaseType_t xResult = pdFALSE;
    BaseType_t xNetworkConnectionCreated = pdFALSE;
    uint32_t ulNotification = 0UL;

    uint32_t ulDemoCount = 0UL;
    uint32_t ulDemoSuccessCount = 0UL;
    int ret = EXIT_SUCCESS;

    ( void ) awsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;
    ( void ) pNetworkInterface;

    for( ulDemoCount = 0UL; ( ulDemoCount < democonfigMQTT_MAX_DEMO_COUNT ); ulDemoCount++ )
    {
        ret = prvConnectAndCreateDemoTasks( NULL );

        if( ret == EXIT_SUCCESS )
        {
            LogInfo( ( "Demo iteration %lu successful.", ( ulDemoCount + 1 ) ) );
            break;
        }
        else if( ulDemoCount < ( democonfigMQTT_MAX_DEMO_COUNT - 1 ) )
        {
            LogWarn( ( "Demo iteration %lu failed. Retrying...", ( ulDemoCount + 1 ) ) );
        }
        else
        {
            LogError( ( "All %d iterations failed", democonfigMQTT_MAX_DEMO_COUNT ) );
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

static MQTTStatus_t prvMQTTAgentInit( void )
{
    TransportInterface_t xTransport;
    MQTTStatus_t xReturn;
    MQTTFixedBuffer_t xFixedBuffer = { .pBuffer = xNetworkBuffer, .size = MQTT_AGENT_NETWORK_BUFFER_SIZE };
    static uint8_t staticQueueStorageArea[ MQTT_AGENT_COMMAND_QUEUE_LENGTH * sizeof( MQTTAgentCommand_t * ) ];
    static StaticQueue_t staticQueueStructure;
    MQTTAgentMessageInterface_t messageInterface =
    {
        .pMsgCtx        = NULL,
        .send           = Agent_MessageSend,
        .recv           = Agent_MessageReceive,
        .getCommand     = Agent_GetCommand,
        .releaseCommand = Agent_ReleaseCommand
    };

    LogDebug( ( "Creating command queue." ) );
    xCommandQueue.queue = xQueueCreateStatic( MQTT_AGENT_COMMAND_QUEUE_LENGTH,
                                              sizeof( MQTTAgentCommand_t * ),
                                              staticQueueStorageArea,
                                              &staticQueueStructure );
    configASSERT( xCommandQueue.queue );
    messageInterface.pMsgCtx = &xCommandQueue;

    /* Initialize the command struct pool. */
    Agent_InitializePool();

    /* Fill in Transport Interface send and receive function pointers. */
    xTransport.pNetworkContext = &xNetworkContext;
    xTransport.send = SecureSocketsTransport_Send;
    xTransport.recv = SecureSocketsTransport_Recv;

    /* Initialize MQTT library. */
    xReturn = MQTTAgent_Init( &xGlobalMqttAgentContext,
                              &messageInterface,
                              &xFixedBuffer,
                              &xTransport,
                              prvGetTimeMs,
                              prvIncomingPublishCallback,
                              /* Context to pass into the callback. Passing the pointer to subscription array. */
                              xGlobalSubscriptionList );

    return xReturn;
}

/*-----------------------------------------------------------*/

static MQTTStatus_t prvMQTTConnect( bool xCleanSession )
{
    MQTTStatus_t xResult;
    MQTTConnectInfo_t xConnectInfo;
    bool xSessionPresent = false;

    /* Many fields are not used in this demo so start with everything at 0. */
    memset( &xConnectInfo, 0x00, sizeof( xConnectInfo ) );

    /* Start with a clean session i.e. direct the MQTT broker to discard any
     * previous session data. Also, establishing a connection with clean session
     * will ensure that the broker does not store any data when this client
     * gets disconnected. */
    xConnectInfo.cleanSession = xCleanSession;

    /* The client identifier is used to uniquely identify this MQTT client to
     * the MQTT broker. In a production device the identifier can be something
     * unique, such as a device serial number. */
    xConnectInfo.pClientIdentifier = democonfigCLIENT_IDENTIFIER;
    xConnectInfo.clientIdentifierLength = ( uint16_t ) strlen( democonfigCLIENT_IDENTIFIER );

    /* Set MQTT keep-alive period. It is the responsibility of the application
     * to ensure that the interval between Control Packets being sent does not
     * exceed the Keep Alive value. In the absence of sending any other Control
     * Packets, the Client MUST send a PINGREQ Packet.  This responsibility will
     * be moved inside the agent. */
    xConnectInfo.keepAliveSeconds = mqttexampleKEEP_ALIVE_INTERVAL_SECONDS;

    /* Append metrics when connecting to the AWS IoT Core broker. */
    #ifdef democonfigUSE_AWS_IOT_CORE_BROKER
        #ifdef democonfigCLIENT_USERNAME
            xConnectInfo.pUserName = CLIENT_USERNAME_WITH_METRICS;
            xConnectInfo.userNameLength = ( uint16_t ) strlen( CLIENT_USERNAME_WITH_METRICS );
            xConnectInfo.pPassword = democonfigCLIENT_PASSWORD;
            xConnectInfo.passwordLength = ( uint16_t ) strlen( democonfigCLIENT_PASSWORD );
        #else
            xConnectInfo.pUserName = AWS_IOT_METRICS_STRING;
            xConnectInfo.userNameLength = AWS_IOT_METRICS_STRING_LENGTH;
            /* Password for authentication is not used. */
            xConnectInfo.pPassword = NULL;
            xConnectInfo.passwordLength = 0U;
        #endif
    #else /* ifdef democonfigUSE_AWS_IOT_CORE_BROKER */
        #ifdef democonfigCLIENT_USERNAME
            xConnectInfo.pUserName = democonfigCLIENT_USERNAME;
            xConnectInfo.userNameLength = ( uint16_t ) strlen( democonfigCLIENT_USERNAME );
            xConnectInfo.pPassword = democonfigCLIENT_PASSWORD;
            xConnectInfo.passwordLength = ( uint16_t ) strlen( democonfigCLIENT_PASSWORD );
        #endif /* ifdef democonfigCLIENT_USERNAME */
    #endif /* ifdef democonfigUSE_AWS_IOT_CORE_BROKER */

    /* Send MQTT CONNECT packet to broker. MQTT's Last Will and Testament feature
     * is not used in this demo, so it is passed as NULL. */
    xResult = MQTT_Connect( &( xGlobalMqttAgentContext.mqttContext ),
                            &xConnectInfo,
                            NULL,
                            mqttexampleCONNACK_RECV_TIMEOUT_MS,
                            &xSessionPresent );

    LogInfo( ( "Session present: %d\n", xSessionPresent ) );

    /* Resume a session if desired. */
    if( ( xResult == MQTTSuccess ) && ( xCleanSession == false ) )
    {
        xResult = MQTTAgent_ResumeSession( &xGlobalMqttAgentContext, xSessionPresent );

        /* Resubscribe to all the subscribed topics. */
        if( ( xResult == MQTTSuccess ) && ( xSessionPresent == false ) )
        {
            xResult = prvHandleResubscribe();
        }
    }

    return xResult;
}

/*-----------------------------------------------------------*/

static MQTTStatus_t prvHandleResubscribe( void )
{
    MQTTStatus_t xResult = MQTTBadParameter;
    uint32_t ulIndex = 0U;
    uint16_t usNumSubscriptions = 0U;

    /* These variables need to stay in scope until command completes. */
    static MQTTAgentSubscribeArgs_t xSubArgs = { 0 };
    static MQTTSubscribeInfo_t xSubInfo[ SUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS ] = { 0 };
    static MQTTAgentCommandInfo_t xCommandParams = { 0 };

    /* Loop through each subscription in the subscription list and add a subscribe
     * command to the command queue. */
    for( ulIndex = 0U; ulIndex < SUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS; ulIndex++ )
    {
        /* Check if there is a subscription in the subscription list. This demo
         * doesn't check for duplicate subscriptions. */
        if( xGlobalSubscriptionList[ ulIndex ].usFilterStringLength != 0 )
        {
            xSubInfo[ usNumSubscriptions ].pTopicFilter = xGlobalSubscriptionList[ ulIndex ].pcSubscriptionFilterString;
            xSubInfo[ usNumSubscriptions ].topicFilterLength = xGlobalSubscriptionList[ ulIndex ].usFilterStringLength;

            /* QoS1 is used for all the subscriptions in this demo. */
            xSubInfo[ usNumSubscriptions ].qos = MQTTQoS1;

            LogInfo( ( "Resubscribe to the topic %.*s will be attempted.",
                       xSubInfo[ usNumSubscriptions ].topicFilterLength,
                       xSubInfo[ usNumSubscriptions ].pTopicFilter ) );

            usNumSubscriptions++;
        }
    }

    if( usNumSubscriptions > 0U )
    {
        xSubArgs.pSubscribeInfo = xSubInfo;
        xSubArgs.numSubscriptions = usNumSubscriptions;

        /* The block time can be 0 as the command loop is not running at this point. */
        xCommandParams.blockTimeMs = 0U;
        xCommandParams.cmdCompleteCallback = prvSubscriptionCommandCallback;
        xCommandParams.pCmdCompleteCallbackContext = ( void * ) &xSubArgs;

        /* Enqueue subscribe to the command queue. These commands will be processed only
         * when command loop starts. */
        xResult = MQTTAgent_Subscribe( &xGlobalMqttAgentContext, &xSubArgs, &xCommandParams );
    }
    else
    {
        /* Mark the resubscribe as success if there is nothing to be subscribed. */
        xResult = MQTTSuccess;
    }

    if( xResult != MQTTSuccess )
    {
        LogError( ( "Failed to enqueue the MQTT subscribe command. xResult=%s.",
                    MQTT_Status_strerror( xResult ) ) );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

static void prvSubscriptionCommandCallback( MQTTAgentCommandContext_t * pxCommandContext,
                                            MQTTAgentReturnInfo_t * pxReturnInfo )
{
    size_t lIndex = 0;
    MQTTAgentSubscribeArgs_t * pxSubscribeArgs = ( MQTTAgentSubscribeArgs_t * ) pxCommandContext;

    /* If the return code is success, no further action is required as all the topic filters
     * are already part of the subscription list. */
    if( pxReturnInfo->returnCode != MQTTSuccess )
    {
        /* Check through each of the suback codes and determine if there are any failures. */
        for( lIndex = 0; lIndex < pxSubscribeArgs->numSubscriptions; lIndex++ )
        {
            /* This demo doesn't attempt to resubscribe in the event that a SUBACK failed. */
            if( pxReturnInfo->pSubackCodes[ lIndex ] == MQTTSubAckFailure )
            {
                LogError( ( "Failed to resubscribe to topic %.*s.",
                            pxSubscribeArgs->pSubscribeInfo[ lIndex ].topicFilterLength,
                            pxSubscribeArgs->pSubscribeInfo[ lIndex ].pTopicFilter ) );
                /* Remove subscription callback for unsubscribe. */
                removeSubscription( xGlobalSubscriptionList,
                                    pxSubscribeArgs->pSubscribeInfo[ lIndex ].pTopicFilter,
                                    pxSubscribeArgs->pSubscribeInfo[ lIndex ].topicFilterLength );
            }
        }
    }
}

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

static BaseType_t prvSocketConnect( NetworkContext_t * pxNetworkContext )
{
    BaseType_t xConnected = pdFAIL;
    TransportSocketStatus_t xNetworkStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;
    ServerInfo_t xServerInfo = { 0 };
    SocketsConfig_t xSocketConfig = { 0 };
    /* Set the receive timeout to a small nonzero value. */
    const TickType_t xTransportTimeout = 1UL;

    /* Initialize the MQTT broker information. */
    xServerInfo.pHostName = democonfigMQTT_BROKER_ENDPOINT;
    xServerInfo.hostNameLength = sizeof( democonfigMQTT_BROKER_ENDPOINT ) - 1U;
    xServerInfo.port = democonfigMQTT_BROKER_PORT;

    /* Set the Secure Socket configurations. */
    xSocketConfig.enableTls = true;
    xSocketConfig.disableSni = false;
    xSocketConfig.sendTimeoutMs = mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS;
    xSocketConfig.recvTimeoutMs = mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS;
    xSocketConfig.pRootCa = democonfigROOT_CA_PEM;
    xSocketConfig.rootCaSize = sizeof( democonfigROOT_CA_PEM );

    /* Establish a TCP connection with the MQTT broker. This example connects to
     * the MQTT broker as specified in democonfigMQTT_BROKER_ENDPOINT and
     * democonfigMQTT_BROKER_PORT at the top of this file. */
    LogInfo( ( "Creating a TLS connection to %s:%d.",
               democonfigMQTT_BROKER_ENDPOINT,
               democonfigMQTT_BROKER_PORT ) );

    xNetworkStatus = SecureSocketsTransport_Connect( pxNetworkContext, &xServerInfo, &xSocketConfig );

    xConnected = ( xNetworkStatus == TRANSPORT_SOCKET_STATUS_SUCCESS ) ? pdPASS : pdFAIL;

    /* Set the socket wakeup callback and ensure the read block time. */
    if( xConnected )
    {
        SOCKETS_SetSockOpt( pxNetworkContext->pParams->tcpSocket,
                            0,
                            SOCKETS_SO_RCVTIMEO,
                            &xTransportTimeout,
                            sizeof( TickType_t ) );
    }

    return xConnected;
}

/*-----------------------------------------------------------*/

static BaseType_t prvSocketDisconnect( NetworkContext_t * pxNetworkContext )
{
    LogInfo( ( "Disconnecting TLS connection.\n" ) );
    TransportSocketStatus_t xNetworkStatus = SecureSocketsTransport_Disconnect( pxNetworkContext );

    return ( xNetworkStatus == TRANSPORT_SOCKET_STATUS_SUCCESS ) ? pdPASS : pdFAIL;
}

/*-----------------------------------------------------------*/

static void prvIncomingPublishCallback( MQTTAgentContext_t * pMqttAgentContext,
                                        uint16_t packetId,
                                        MQTTPublishInfo_t * pxPublishInfo )
{
    bool xPublishHandled = false;
    char cOriginalChar, * pcLocation;

    ( void ) packetId;

    /* Fan out the incoming publishes to the callbacks registered using
     * subscription manager. */
    xPublishHandled = handleIncomingPublishes( ( SubscriptionElement_t * ) pMqttAgentContext->pIncomingCallbackContext,
                                               pxPublishInfo );

    /* If there are no callbacks to handle the incoming publishes,
     * handle it as an unsolicited publish. */
    if( xPublishHandled != true )
    {
        /* Ensure the topic string is terminated for printing.  This will over-
         * write the message ID, which is restored afterwards. */
        pcLocation = ( char * ) &( pxPublishInfo->pTopicName[ pxPublishInfo->topicNameLength ] );
        cOriginalChar = *pcLocation;
        *pcLocation = 0x00;
        LogWarn( ( "Received an unsolicited publish from topic %s", pxPublishInfo->pTopicName ) );
        *pcLocation = cOriginalChar;
    }
}

/*-----------------------------------------------------------*/

static void prvMQTTAgentTask( void * pvParameters )
{
    BaseType_t xNetworkResult = pdFAIL;
    MQTTStatus_t xMQTTStatus = MQTTSuccess, xConnectStatus = MQTTSuccess;
    MQTTContext_t * pMqttContext = &( xGlobalMqttAgentContext.mqttContext );

    ( void ) pvParameters;

    do
    {
        /* MQTTAgent_CommandLoop() is effectively the agent implementation.  It
         * will manage the MQTT protocol until such time that an error occurs,
         * which could be a disconnect.  If an error occurs the MQTT context on
         * which the error happened is returned so there can be an attempt to
         * clean up and reconnect however the application writer prefers. */
        xMQTTStatus = MQTTAgent_CommandLoop( &xGlobalMqttAgentContext );

        /* Success is returned for disconnect or termination. The socket should
         * be disconnected. */
        if( ( xMQTTStatus == MQTTSuccess ) && ( xGlobalMqttAgentContext.mqttContext.connectStatus == MQTTNotConnected ) )
        {
            /* MQTT Disconnect. Disconnect the socket. */
            xNetworkResult = prvSocketDisconnect( &xNetworkContext );
        }
        else if( xMQTTStatus == MQTTSuccess )
        {
            /* MQTTAgent_Terminate() was called, but MQTT was not disconnected. */
            xConnectStatus = MQTT_Disconnect( &( xGlobalMqttAgentContext.mqttContext ) );
            xNetworkResult = prvSocketDisconnect( &xNetworkContext );
            break;
        }
        /* Any error. */
        else
        {
            /* Reconnect TCP. */
            xNetworkResult = prvSocketDisconnect( &xNetworkContext );

            if( xNetworkResult == pdPASS )
            {
                pMqttContext->connectStatus = MQTTNotConnected;

                /* MQTT Connect with a persistent session. */
                xNetworkResult = prvConnectToMQTTBroker( false );
            }

            if( xNetworkResult != pdPASS )
            {
                LogError( ( "Could not reconnect to MQTT broker" ) );
                break;
            }
        }
    } while( xMQTTStatus != MQTTSuccess );

    /* Delete the task if it is complete. */
    LogInfo( ( "MQTT Agent task completed." ) );
    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/

static BaseType_t prvConnectToMQTTBroker( bool xCreateCleanSession )
{
    MQTTStatus_t xMQTTStatus = MQTTBadParameter;
    BaseType_t xConnected = pdFAIL;
    BackoffAlgorithmContext_t xReconnectParams;
    BaseType_t xBackoffStatus = pdFAIL;

    /* Initialize reconnect attempts and interval. */
    BackoffAlgorithm_InitializeParams( &xReconnectParams,
                                       RETRY_BACKOFF_BASE_MS,
                                       RETRY_MAX_BACKOFF_DELAY_MS,
                                       RETRY_MAX_ATTEMPTS );

    /* Attempt to connect to MQTT broker. If connection fails, retry after a
     * timeout. Timeout value will exponentially increase until the maximum
     * number of attempts are reached.
     */
    do
    {
        /* Connect a TCP socket to the broker. */
        xConnected = prvSocketConnect( &xNetworkContext );

        if( xConnected == pdPASS )
        {
            /* Form an MQTT connection. */
            xMQTTStatus = prvMQTTConnect( xCreateCleanSession );

            if( xMQTTStatus != MQTTSuccess )
            {
                /* Close connection before next retry. */
                prvSocketDisconnect( &xNetworkContext );
            }
        }

        if( xMQTTStatus != MQTTSuccess )
        {
            LogWarn( ( "Connection to the broker failed. Attempting connection retry after backoff delay." ) );

            /* As the connection attempt failed, we will retry the connection after an
             * exponential backoff with jitter delay. */

            /* Calculate the backoff period for the next retry attempt and perform the wait operation. */
            xBackoffStatus = prvBackoffForRetry( &xReconnectParams );
        }
    } while( ( xMQTTStatus != MQTTSuccess ) && ( xBackoffStatus == pdPASS ) );

    return ( xMQTTStatus == MQTTSuccess ) ? pdPASS : pdFAIL;
}
/*-----------------------------------------------------------*/

static int prvConnectAndCreateDemoTasks( void * pvParameters )
{
    MQTTAgentCommandInfo_t xCommandParams = { 0 };
    uint32_t i, numSuccess = 0;
    BaseType_t xResult = pdFAIL;
    MQTTStatus_t xMQTTStatus = MQTTBadParameter;

    ( void ) pvParameters;

    /* Miscellaneous initialization. */
    ulGlobalEntryTimeMs = prvGetTimeMs();

    /* Set the pParams member of the network context with desired transport. */
    xNetworkContext.pParams = &secureSocketsTransportParams;

    /* Initialize the MQTT context with the buffer and transport interface. */
    xMQTTStatus = prvMQTTAgentInit();

    if( xMQTTStatus == MQTTSuccess )
    {
        /* Create the TCP connection to the broker, then the MQTT connection to the
         * same. */
        xResult = prvConnectToMQTTBroker( true );
    }

    if( xResult == pdPASS )
    {
        /* Create demo tasks as per the configuration macro settings. */
        vStartSimpleSubscribePublishTask( democonfigNUM_SIMPLE_SUB_PUB_TASKS_TO_CREATE,
                                          democonfigSIMPLE_SUB_PUB_TASK_STACK_SIZE,
                                          tskIDLE_PRIORITY,
                                          taskParameters );

        /* Create an instance of the MQTT agent task. Give it higher priority than the
         * subscribe-publish tasks so that the agent's command queue will not become full,
         * as those tasks need to send commands to the queue. */
        xTaskCreate( prvMQTTAgentTask,
                     "MQTT Agent",
                     democonfigSIMPLE_SUB_PUB_TASK_STACK_SIZE,
                     NULL,
                     tskIDLE_PRIORITY + 1,
                     NULL );

        /* Wait for all tasks to exit. */
        for( i = 0; i < democonfigNUM_SIMPLE_SUB_PUB_TASKS_TO_CREATE; i++ )
        {
            ulTaskNotifyTake( pdFALSE, portMAX_DELAY );
        }

        /* Terminate the agent task. */
        MQTTAgent_Terminate( &xGlobalMqttAgentContext, &xCommandParams );

        for( i = 0; i < democonfigNUM_SIMPLE_SUB_PUB_TASKS_TO_CREATE; i++ )
        {
            if( taskParameters[ i ].xSuccess )
            {
                numSuccess++;
            }
        }
    }

    LogInfo( ( "%lu/%lu tasks successful.", numSuccess, democonfigNUM_SIMPLE_SUB_PUB_TASKS_TO_CREATE ) );

    return ( numSuccess == democonfigNUM_SIMPLE_SUB_PUB_TASKS_TO_CREATE ) ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*-----------------------------------------------------------*/

static uint32_t prvGetTimeMs( void )
{
    TickType_t xTickCount = 0;
    uint32_t ulTimeMs = 0UL;

    /* Get the current tick count. */
    xTickCount = xTaskGetTickCount();

    /* Convert the ticks to milliseconds. */
    ulTimeMs = ( uint32_t ) xTickCount * mqttexampleMILLISECONDS_PER_TICK;

    /* Reduce ulGlobalEntryTimeMs from obtained time so as to always return the
     * elapsed time in the application. */
    ulTimeMs = ( uint32_t ) ( ulTimeMs - ulGlobalEntryTimeMs );

    return ulTimeMs;
}

/*-----------------------------------------------------------*/
