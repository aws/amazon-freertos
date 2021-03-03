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
 * @file ota_demo_core_mqtt.c
 * @brief OTA update example using coreMQTT.
 */

/* Standard includes. */
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include "aws_demo_config.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "iot_network.h"

/* Agent  APIS for running MQTT in a multithreaded environment. */
#include "freertos_mqtt_agent.h"

/* Includes helpers for managing MQTT subscriptions. */
#include "subscription_manager.h"

/* Header include. */
#include "agent_message.h"

/* Retry utilities include. */
#include "backoff_algorithm.h"

/* Include PKCS11 helper for random number generation. */
#include "pkcs11_helpers.h"

/*Include backoff algorithm header for retry logic.*/
#include "backoff_algorithm.h"

/* Transport interface include. */
#include "transport_interface.h"

/* Transport interface implementation include header for TLS. */
#include "transport_secure_sockets.h"

/* Include header for connection configurations. */
#include "aws_clientcredential.h"

/* Include header for client credentials. */
#include "aws_clientcredential_keys.h"

/* Include header for root CA certificates. */
#include "iot_default_root_certificates.h"

/* OTA Library include. */
#include "ota.h"
#include "ota_config.h"

/* OTA Library Interface include. */
#include "ota_os_freertos.h"
#include "ota_mqtt_interface.h"

/* Helper functions such as subscription management. */
#include "ota_demo_helpers.h"

/* PAL abstraction layer APIs. */
#include "ota_pal.h"

/* Includes the OTA Application version number. */
#include "ota_appversion32.h"

/*------------- Demo configurations -------------------------*/

/** Note: The device client certificate and private key credentials are
 * obtained by the transport interface implementation (with Secure Sockets)
 * from the demos/include/aws_clientcredential_keys.h file.
 *
 * The following macros SHOULD be defined for this demo which uses both server
 * and client authentications for TLS session:
 *   - keyCLIENT_CERTIFICATE_PEM for client certificate.
 *   - keyCLIENT_PRIVATE_KEY_PEM for client private key.
 */

/**
 * @brief The MQTT broker endpoint used for this demo.
 */
#ifndef democonfigMQTT_BROKER_ENDPOINT
    #define democonfigMQTT_BROKER_ENDPOINT    clientcredentialMQTT_BROKER_ENDPOINT
#endif

/**
 * @brief The root CA certificate belonging to the broker.
 */
#ifndef democonfigROOT_CA_PEM
    #define democonfigROOT_CA_PEM    tlsATS1_ROOT_CERTIFICATE_PEM
#endif

#ifndef democonfigCLIENT_IDENTIFIER

/**
 * @brief The MQTT client identifier used in this example.  Each client identifier
 * must be unique so edit as required to ensure no two clients connecting to the
 * same broker use the same client identifier.
 */
    #define democonfigCLIENT_IDENTIFIER    clientcredentialIOT_THING_NAME
#endif

#ifndef democonfigMQTT_BROKER_PORT

/**
 * @brief The port to use for the demo.
 */
    #define democonfigMQTT_BROKER_PORT    clientcredentialMQTT_BROKER_PORT
#endif

/**
 * @brief Transport timeout in milliseconds for transport send and receive.
 */
#define otaexampleMQTT_TRANSPORT_SEND_RECV_TIMEOUT_MS    ( 500U )

/**
 * @brief The maximum number of retries for network operation with server.
 */
#define RETRY_MAX_ATTEMPTS                               ( 5U )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying failed operation
 *  with server.
 */
#define RETRY_MAX_BACKOFF_DELAY_MS                       ( 5000U )

/**
 * @brief The base back-off delay (in milliseconds) to use for network operation retry
 * attempts.
 */
#define RETRY_BACKOFF_BASE_MS                            ( 500U )

/**
 * @brief ALPN (Application-Layer Protocol Negotiation) protocol name for AWS IoT MQTT.
 *
 * This will be used if the AWS_MQTT_PORT is configured as 443 for AWS IoT MQTT broker.
 * Please see more details about the ALPN protocol for AWS IoT MQTT endpoint
 * in the link below.
 * https://aws.amazon.com/blogs/iot/mqtt-with-tls-client-authentication-on-port-443-why-it-is-useful-and-how-it-works/
 */
#define AWS_IOT_MQTT_ALPN                                "\x0ex-amzn-mqtt-ca"

/**
 * @brief Length of ALPN protocol name.
 */
#define AWS_IOT_MQTT_ALPN_LENGTH                         ( ( uint16_t ) ( sizeof( AWS_IOT_MQTT_ALPN ) - 1 ) )

/**
 * @brief Timeout for receiving CONNACK packet in milli seconds.
 */
#define CONNACK_RECV_TIMEOUT_MS                          ( 2000U )

/**
 * @brief The maximum time interval in seconds which is allowed to elapse
 * between two Control Packets.
 *
 * It is the responsibility of the Client to ensure that the interval between
 * Control Packets being sent does not exceed the this Keep Alive value. In the
 * absence of sending any other Control Packets, the Client MUST send a
 * PINGREQ Packet.
 */
#define MQTT_KEEP_ALIVE_INTERVAL_SECONDS                 ( 60U )


/**
 * @brief The delay used in the main OTA Demo task loop to periodically output the OTA
 * statistics like number of packets received, dropped, processed and queued per connection.
 */
#define OTA_EXAMPLE_TASK_DELAY_MS                ( 1000U )

/**
 * @brief The timeout for waiting for the agent to get suspended after closing the
 * connection.
 *
 * Timeout value should be large enough for OTA agent to finish any pending MQTT operations
 * and suspend itself.
 *
 */
#define OTA_SUSPEND_TIMEOUT_MS                   ( 10000U )

/**
 * @brief The maximum size of the file paths used in the demo.
 */
#define OTA_MAX_FILE_PATH_SIZE                   ( 260U )

/**
 * @brief The maximum size of the stream name required for downloading update file
 * from streaming service.
 */
#define OTA_MAX_STREAM_NAME_SIZE                 ( 128U )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying connection to server.
 */
#define CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS    ( 5000U )

/**
 * @brief The base back-off delay (in milliseconds) to use for connection retry attempts.
 */
#define CONNECTION_RETRY_BACKOFF_BASE_MS         ( 500U )

/**
 * @brief Number of milliseconds in a second.
 */
#define NUM_MILLISECONDS_IN_SECOND               ( 1000U )

/**
 * @brief Size of the network buffer to receive the MQTT message.
 *
 * The largest message size is data size from the AWS IoT streaming service,
 * otaconfigFILE_BLOCK_SIZE + extra for headers.
 */

#define OTA_NETWORK_BUFFER_SIZE                          ( 5120U )

/**
 * @brief The maximum number of retries for connecting to server.
 */
#define CONNECTION_RETRY_MAX_ATTEMPTS                    ( 5U )

/**
 * @brief The common prefix for all OTA topics.
 *
 * Thing name is substituted with a wildcard symbol `+`. OTA agent
 * registers with MQTT broker with the thing name in the topic. This topic
 * filter is used to match incoming packet received and route them to OTA.
 * Thing name is not needed for this matching.
 */
#define OTA_TOPIC_PREFIX                                 "$aws/things/+/"

/**
 * @brief Wildcard topic filter for job notification.
 * The filter is used to match the constructed job notify topic filter from OTA agent and register
 * appropirate callback for it.
 */
#define OTA_JOB_NOTIFY_TOPIC_FILTER                      OTA_TOPIC_PREFIX "jobs/notify-next"

/**
 * @brief Length of job notification topic filter.
 */
#define OTA_JOB_NOTIFY_TOPIC_FILTER_LENGTH               ( ( uint16_t ) ( sizeof( OTA_JOB_NOTIFY_TOPIC_FILTER ) - 1 ) )

/**
 * @brief Wildcard topic filter for matching job response messages.
 * This topic filter is used to match the responses from OTA service for OTA agent job requests. THe
 * topic filter is a reserved topic which is not subscribed with MQTT broker.
 *
 */
#define OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER           OTA_TOPIC_PREFIX "jobs/$next/get/accepted"

/**
 * @brief Length of job accepted response topic filter.
 */
#define OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER_LENGTH    ( ( uint16_t ) ( sizeof( OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER ) - 1 ) )


/**
 * @brief Wildcard topic filter for matching OTA data packets.
 *  The filter is used to match the constructed data stream topic filter from OTA agent and register
 * appropirate callback for it.
 */
#define OTA_DATA_STREAM_TOPIC_FILTER           OTA_TOPIC_PREFIX  "streams/#"

/**
 * @brief Length of data stream topic filter.
 */
#define OTA_DATA_STREAM_TOPIC_FILTER_LENGTH    ( ( uint16_t ) ( sizeof( OTA_DATA_STREAM_TOPIC_FILTER ) - 1 ) )


/**
 * @brief Default topic filter for OTA.
 * This is used to route all the packets for OTA reserved topics which OTA agent has not subscribed for.
 */
#define OTA_DEFAULT_TOPIC_FILTER           OTA_TOPIC_PREFIX "jobs/#"

/**
 * @brief Length of default topic filter.
 */
#define OTA_DEFAULT_TOPIC_FILTER_LENGTH    ( ( uint16_t ) ( sizeof( OTA_DEFAULT_TOPIC_FILTER ) - 1 ) )


/**
 * @brief Stack size required for OTA agent task.
 */
#define OTA_AGENT_TASK_STACK_SIZE        ( 6000U )

/**
 * @brief Priority required for OTA agent task.
 */
#define OTA_AGENT_TASK_PRIORITY          ( tskIDLE_PRIORITY )

/**
 * @brief Stack size required for OTA statistics  task.
 */
#define MQTT_AGENT_TASK_STACK_SIZE       ( 2048U )

/**
 * @brief Priority required for OTA statistics task.
 */
#define MQTT_AGENT_TASK_PRIORITY         ( tskIDLE_PRIORITY )

/**
 * @brief The maximum amount of time in milliseconds to wait for the commands
 * to be posted to the MQTT agent should the MQTT agent's command queue be full.
 * Tasks wait in the Blocked state, so don't use any CPU time.
 */
#define MQTT_AGENT_SEND_BLOCK_TIME_MS    ( 200U )


/**
 * @brief This demo uses task notifications to signal tasks from MQTT callback
 * functions.  mqttexampleMS_TO_WAIT_FOR_NOTIFICATION defines the time, in ticks,
 * to wait for such a callback.
 */
#define MQTT_AGENT_MS_TO_WAIT_FOR_NOTIFICATION    ( 5000U )

/**
 * @brief Milliseconds per second.
 */
#define MILLISECONDS_PER_SECOND                   ( 1000U )

/**
 * @brief Milliseconds per FreeRTOS tick.
 */
#define MILLISECONDS_PER_TICK                     ( MILLISECONDS_PER_SECOND / configTICK_RATE_HZ )

/**
 * @brief Defines the structure to use as the agent IPC mechanism.
 */
struct AgentMessageContext
{
    QueueHandle_t queue;
};

/**
 * @brief Structure used to store the topic filter to ota callback mappings.
 */
typedef struct OtaTopicFilterCallback
{
    const char * pTopicFilter;
    uint16_t topicFilterLength;
    IncomingPubCallback_t callback;
} OtaTopicFilterCallback_t;

/**
 * @brief Defines the structure to use as the command callback context in this
 * demo.
 */
struct CommandContext
{
    MQTTStatus_t xReturnStatus;
    TaskHandle_t xTaskToNotify;
    uint32_t ulNotificationValue;
    void * pArgs;
};

/**
 * @brief The MQTT agent context.
 * In case of sharing the mqtt connection with other demos using the MQTT agent, this context
 * should be declared non-static so that's it shared across all demo files.
 */
static MQTTAgentContext_t xGlobalMqttAgentContext;

/**
 * @brief The buffer is used to hold the serialized packets for transmission to and from
 * the transport interface.
 */
static uint8_t xNetworkBuffer[ MQTT_AGENT_NETWORK_BUFFER_SIZE ];

/**
 * @brief The interface context used to post commands to the agent.
 * For FreeRTOS its implemented using a FreeRTOS blocking queue.
 */
static AgentMessageContext_t xCommandQueue;

/**
 * @brief The global array of subscription elements.
 *
 * @note No thread safety is required to this array, since the updates the array
 * elements are done only from one task at a time. The subscription manager
 * implementation expects that the array of the subscription elements used for
 * storing subscriptions to be initialized to 0. As this is a global array, it
 * will be intialized to 0 by default.
 */
static SubscriptionElement_t xGlobalSubscriptionList[ SUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS ];

/**
 * @brief Network connection context used in this demo for MQTT connection.
 */
static NetworkContext_t networkContextMqtt;

/**
 * @brief MQTT connection context used in this demo.
 */
static MQTTContext_t mqttContext;

/**
 * @brief Semaphore for synchronizing buffer operations.
 */
static SemaphoreHandle_t xBufferSemaphore;

/**
 * @brief The network buffer must remain valid when OTA library task is running.
 */
static uint8_t otaNetworkBuffer[ OTA_NETWORK_BUFFER_SIZE ];

/**
 * @brief Update File path buffer.
 */
static uint8_t updateFilePath[ OTA_MAX_FILE_PATH_SIZE ];

/**
 * @brief Certificate File path buffer.
 */
static uint8_t certFilePath[ OTA_MAX_FILE_PATH_SIZE ];

/**
 * @brief Stream name buffer.
 */
static uint8_t streamName[ OTA_MAX_STREAM_NAME_SIZE ];

/**
 * @brief Decode memory.
 */
static uint8_t decodeMem[ otaconfigFILE_BLOCK_SIZE ];

/**
 * @brief Bitmap memory.
 */
static uint8_t bitmap[ OTA_MAX_BLOCK_BITMAP_SIZE ];

/**
 * @brief Event buffer.
 */
static OtaEventData_t eventBuffer[ otaconfigMAX_NUM_OTA_DATA_BUFFERS ];

/**
 * @brief Global entry time into the application to use as a reference timestamp
 * in the #prvGetTimeMs function. #prvGetTimeMs will always return the difference
 * between the current time and the global entry time. This will reduce the chances
 * of overflow for the 32 bit unsigned integer used for holding the timestamp.
 */
static uint32_t ulGlobalEntryTimeMs;

/**
 * @brief The buffer passed to the OTA Agent from application while initializing.
 */
static OtaAppBuffer_t otaBuffer =
{
    .pUpdateFilePath    = updateFilePath,
    .updateFilePathsize = OTA_MAX_FILE_PATH_SIZE,
    .pCertFilePath      = certFilePath,
    .certFilePathSize   = OTA_MAX_FILE_PATH_SIZE,
    .pStreamName        = streamName,
    .streamNameSize     = OTA_MAX_STREAM_NAME_SIZE,
    .pDecodeMemory      = decodeMem,
    .decodeMemorySize   = otaconfigFILE_BLOCK_SIZE,
    .pFileBitmap        = bitmap,
    .fileBitmapSize     = OTA_MAX_BLOCK_BITMAP_SIZE,
};

/*-----------------------------------------------------------*/

/**
 * @brief Initializes an MQTT context, including transport interface and
 * network buffer.
 *
 * @return `MQTTSuccess` if the initialization succeeds, else `MQTTBadParameter`.
 */
static MQTTStatus_t prvMqttInit( void );

/**
 * @brief Sends an MQTT CONNECT packet over the already connected TCP socket.
 *
 * @return MQTTSuccess if an MQTT session is established;
 * EXIT_FAILURE otherwise.
 */
static MQTTStatus_t prvMQTTConnect( void );

/**
 * @brief Publish message to a topic.
 *
 * This function publishes a message to a given topic & QoS.
 *
 * @param[in] pTopic Mqtt topic filter.
 *
 * @param[in] topicLen Length of the topic filter.
 *
 * @param[in] pMsg Message to publish.
 *
 * @param[in] msgSize Message size.
 *
 * @param[in] qos Quality of Service
 *
 * @return OtaMqttSuccess if success , other error code on failure.
 */
static OtaMqttStatus_t prvMqttPublish( const char * const pTopic,
                                       uint16_t topicLen,
                                       const char * pMsg,
                                       uint32_t msgSize,
                                       uint8_t qos );

/**
 * @brief Subscribe to the Mqtt topics.
 *
 * This function subscribes to the Mqtt topics with the Quality of service
 * received as parameter. This function also registers a callback for the
 * topicfilter.
 *
 * @param[in] pTopicFilter Mqtt topic filter.
 *
 * @param[in] topicFilterLength Length of the topic filter.
 *
 * @param[in] qos Quality of Service
 *
 * @param[in] callback Callback to be registered.
 *
 * @return OtaMqttSuccess if success , other error code on failure.
 */
static OtaMqttStatus_t prvMqttSubscribe( const char * pTopicFilter,
                                         uint16_t topicFilterLength,
                                         uint8_t qos );

/**
 * @brief Unsubscribe to the Mqtt topics.
 *
 * This function unsubscribes to the Mqtt topics with the Quality of service
 * received as parameter.
 *
 * @param[in] pTopicFilter Mqtt topic filter.
 *
 * @param[in] topicFilterLength Length of the topic filter.
 *
 * @param[qos] qos Quality of Service
 *
 * @return  OtaMqttSuccess if success , other error code on failure.
 */
static OtaMqttStatus_t prvMqttUnSubscribe( const char * pTopicFilter,
                                           uint16_t topicFilterLength,
                                           uint8_t qos );

/**
 * @brief Attempt to connect to the MQTT broker.
 *
 * @return pdPASS if a connection is established.
 */
static BaseType_t prvConnectToMQTTBroker( void );

/**
 * @brief Retry logic to establish a connection to the MQTT broker.
 *
 * If the connection fails, keep retrying with exponentially increasing
 * timeout value, until max retries, max timeout or successful connect.
 *
 * @param[in] pNetworkContext Network context to connect on.
 * @return int pdFALSE if connection failed after retries.
 */
static BaseType_t prvCreateSocketConnectionToMQTTBroker( NetworkContext_t * pNetworkContext );

/**
 * @brief Disconnects from the MQTT broker.
 * Initiates an MQTT disconnect and then teardown underlying TCP connection.
 *
 */
static void prvDisconnectFromMQTTBroker( void );

/**
 * @brief Task for OTA agent.
 * Task runs OTA agent  loop which process OTA events. Task returns only when OTA agent is shutdown by
 * invoking OTA_Shutdown() API.
 *
 * @param[in] pParam Can be used to pass down functionality to the agent task
 */
static void prvOTAAgentTask( void * pParam );

/**
 * @brief Task for MQTT agent.
 * Task runs MQTT agent command loop, which returns only when the user disconnects
 * MQTT, terminates agent, or the mqtt connection is broken. If the mqtt connection is broken, the task
 * suspends OTA agent reconnects to the broker and then resumes OTA agent.
 *
 * @param[in] pParam Can be used to pass down functionality to the agent task
 */
static void prvMQTTAgentTask( void * pParam );


/**
 * @brief Callback invoked by agent for a command process completion.
 *
 * @param[in] pxCommandContext User context passed by caller along with the command.
 * @param[in] pxReturnInfo Info containing return code and output of command from agent.
 */
static void prvMQTTAgentCmdCompleteCallback( CommandContext_t * pxCommandContext,
                                             MQTTAgentReturnInfo_t * pxReturnInfo );

/**
 * @brief Start OTA demo.
 *
 * @return   pPASS or pdFAIL.
 */
static BaseType_t prvRunOTADemo( void );

/**
 * @brief Suspend OTA demo.
 *
 * @return   pPASS or pdFAIL.
 */
static BaseType_t prvSuspendOTA( void );

/**
 * @brief Resume OTA demo.
 *
 * @return   pPASS or pdFAIL.
 */
static BaseType_t prvResumeOTA( void );

/**
 * @brief Set OTA interfaces.
 *
 * @param[in]  pOtaInterfaces pointer to OTA interface structure.
 *
 * @return   None.
 */
static void setOtaInterfaces( OtaInterfaces_t * pOtaInterfaces );

/**
 * @brief Random number to be used as a back-off value for retrying connection.
 *
 * @return uint32_t The generated random number.
 */
static int32_t prvGenerateRandomNumber();

/* Callbacks used to handle different events. */

/**
 * @brief The OTA agent has completed the update job or it is in
 * self test mode. If it was accepted, we want to activate the new image.
 * This typically means we should reset the device to run the new firmware.
 * If now is not a good time to reset the device, it may be activated later
 * by your user code. If the update was rejected, just return without doing
 * anything and we'll wait for another job. If it reported that we should
 * start test mode, normally we would perform some kind of system checks to
 * make sure our new firmware does the basic things we think it should do
 * but we'll just go ahead and set the image as accepted for demo purposes.
 * The accept function varies depending on your platform. Refer to the OTA
 * PAL implementation for your platform in aws_ota_pal.c to see what it
 * does for you.
 *
 * @param[in] event Event from OTA lib of type OtaJobEvent_t.
 * @return None.
 */
static void otaAppCallback( OtaJobEvent_t event,
                            const void * pData );


/**
 * @brief Common callback registered with MQTT agent to receive all publish packets.
 * Packets received using the callback is distributed to subscribed topics using subscription manager.
 *
 * @param[in] pMqttAgentContext MQTT agent context for the connection.
 * @param[in] packetId Packet identifier for the packet.
 * @param[in] pPublishInfo MQTT packet information which stores details of the
 * job document.
 */
static void prvIncomingPublishCallback( MQTTAgentContext_t * pMqttAgentContext,
                                        uint16_t packetId,
                                        MQTTPublishInfo_t * pxPublishInfo );

/**
 * @brief Register OTA callbacks with the subscription manager.
 *
 * @param[in] pTopicFilter The topic filter for which a  callback needs to be registered for.
 * @param[in] topicFilterLength length of the topic filter.
 *
 */
static void prvRegisterOTACallback( const char * pTopicFilter,
                                    uint16_t topicFilterLength );


/**
 * @brief Callback registered with the OTA library that notifies the OTA agent
 * of an incoming PUBLISH containing a job document.
 *
 * @param[in] pContext MQTT context which stores the connection.
 * @param[in] pPublishInfo MQTT packet information which stores details of the
 * job document.
 */
static void prvMqttJobCallback( void * pContext,
                                MQTTPublishInfo_t * pPublish );


/**
 * @brief Callback that notifies the OTA library when a data block is received.
 *
 * @param[in] pContext MQTT context which stores the connection.
 * @param[in] pPublishInfo MQTT packet that stores the information of the file block.
 */
static void prvMqttDataCallback( void * pContext,
                                 MQTTPublishInfo_t * pPublish );

/**
 * @brief Default callback used to receive unsolicited messages for OTA.
 *
 * The callback is not subscribed with MQTT broker, but only with local subscription manager.
 * A wildcard OTA job topic is used for subscription so that all unsolicited messages related to OTA is
 * forwarded to this callback for filteration. Right now the callback is used to filter responses to job requests
 * from the OTA service.
 *
 * @param[in] pvIncomingPublishCallbackContext MQTT context which stores the connection.
 * @param[in] pPublishInfo MQTT packet that stores the information of the file block.
 */
static void prvMqttDefaultCallback( void * pvIncomingPublishCallbackContext,
                                    MQTTPublishInfo_t * pxPublishInfo );


/**
 * @brief Registry for all  mqtt topic filters to their corresponding callbacks for OTA.
 */
static OtaTopicFilterCallback_t otaTopicFilterCallbacks[] =
{
    {
        .pTopicFilter = OTA_JOB_NOTIFY_TOPIC_FILTER,
        .topicFilterLength = OTA_JOB_NOTIFY_TOPIC_FILTER_LENGTH,
        .callback = prvMqttJobCallback
    },
    {
        .pTopicFilter = OTA_DATA_STREAM_TOPIC_FILTER,
        .topicFilterLength = OTA_DATA_STREAM_TOPIC_FILTER_LENGTH,
        .callback = prvMqttDataCallback
    },
    {
        .pTopicFilter = OTA_DEFAULT_TOPIC_FILTER,
        .topicFilterLength = OTA_DEFAULT_TOPIC_FILTER_LENGTH,
        .callback = prvMqttDefaultCallback
    }
};

/*-----------------------------------------------------------*/

static void otaEventBufferFree( OtaEventData_t * const pxBuffer )
{
    if( xSemaphoreTake( xBufferSemaphore, portMAX_DELAY ) == pdTRUE )
    {
        pxBuffer->bufferUsed = false;
        ( void ) xSemaphoreGive( xBufferSemaphore );
    }
    else
    {
        LogError( ( "Failed to get buffer semaphore." ) );
    }
}

/*-----------------------------------------------------------*/

static OtaEventData_t * otaEventBufferGet( void )
{
    uint32_t ulIndex = 0;
    OtaEventData_t * pFreeBuffer = NULL;

    if( xSemaphoreTake( xBufferSemaphore, portMAX_DELAY ) == pdTRUE )
    {
        for( ulIndex = 0; ulIndex < otaconfigMAX_NUM_OTA_DATA_BUFFERS; ulIndex++ )
        {
            if( eventBuffer[ ulIndex ].bufferUsed == false )
            {
                eventBuffer[ ulIndex ].bufferUsed = true;
                pFreeBuffer = &eventBuffer[ ulIndex ];
                break;
            }
        }

        ( void ) xSemaphoreGive( xBufferSemaphore );
    }
    else
    {
        LogError( ( "Failed to get buffer semaphore." ) );
    }

    return pFreeBuffer;
}


/*-----------------------------------------------------------*/

static void otaAppCallback( OtaJobEvent_t event,
                            const void * pData )
{
    OtaErr_t err = OtaErrUninitialized;

    switch( event )
    {
        case OtaJobEventActivate:
            LogInfo( ( "Received OtaJobEventActivate callback from OTA Agent." ) );

            /* Activate the new firmware image. */
            OTA_ActivateNewImage();

            /* Initiate Shutdown of OTA Agent.
             * If it is required that the unsubscribe operations are not
             * performed while shutting down please set the second parameter to 0 instead of 1.
             */
            OTA_Shutdown( 0, 1 );

            /* Requires manual activation of new image.*/
            LogError( ( "New image activation failed." ) );

            break;

        case OtaJobEventFail:
            LogInfo( ( "Received OtaJobEventFail callback from OTA Agent." ) );

            /* Nothing special to do. The OTA agent handles it. */
            break;

        case OtaJobEventStartTest:

            /* This demo just accepts the image since it was a good OTA update and networking
             * and services are all working (or we would not have made it this far). If this
             * were some custom device that wants to test other things before validating new
             * image, this would be the place to kick off those tests before calling
             * OTA_SetImageState() with the final result of either accepted or rejected. */

            LogInfo( ( "Received OtaJobEventStartTest callback from OTA Agent." ) );
            err = OTA_SetImageState( OtaImageStateAccepted );

            if( err != OtaErrNone )
            {
                LogError( ( " Failed to set image state as accepted." ) );
            }
            else
            {
                LogInfo( ( "Successfully updated with the new image." ) );
            }

            break;

        case OtaJobEventProcessed:
            LogDebug( ( "Received OtaJobEventProcessed callback from OTA Agent." ) );

            if( pData != NULL )
            {
                otaEventBufferFree( ( OtaEventData_t * ) pData );
            }

            break;

        case OtaJobEventSelfTestFailed:
            LogDebug( ( "Received OtaJobEventSelfTestFailed callback from OTA Agent." ) );

            /* Requires manual activation of previous image as self-test for
             * new image downloaded failed.*/
            LogError( ( "Self-test of new image failed, shutting down OTA Agent." ) );

            /* Initiate Shutdown of OTA Agent.
             * If it is required that the unsubscribe operations are not
             * performed while shutting down please set the second parameter to 0 instead of 1.
             */
            OTA_Shutdown( 0, 1 );

            break;

        default:
            LogDebug( ( "Received invalid callback event from OTA Agent." ) );
    }
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
        LogWarn( ( "WARN:  Received an unsolicited publish from topic %s", pxPublishInfo->pTopicName ) );
        *pcLocation = cOriginalChar;
    }
}

/*-----------------------------------------------------------*/

static void prvMqttJobCallback( void * pvIncomingPublishCallbackContext,
                                MQTTPublishInfo_t * pxPublishInfo )
{
    OtaEventData_t * pData;
    OtaEventMsg_t eventMsg = { 0 };

    configASSERT( pxPublishInfo != NULL );
    ( void ) pvIncomingPublishCallbackContext;

    LogInfo( ( "Received job message callback, size %ld.\n\n", pxPublishInfo->payloadLength ) );

    pData = otaEventBufferGet();

    if( pData != NULL )
    {
        memcpy( pData->data, pxPublishInfo->pPayload, pxPublishInfo->payloadLength );
        pData->dataLength = pxPublishInfo->payloadLength;
        eventMsg.eventId = OtaAgentEventReceivedJobDocument;
        eventMsg.pEventData = pData;

        /* Send job document received event. */
        OTA_SignalEvent( &eventMsg );
    }
    else
    {
        LogError( ( "Error: No OTA data buffers available.\r\n" ) );
    }
}

/*-----------------------------------------------------------*/
static void prvMqttDefaultCallback( void * pvIncomingPublishCallbackContext,
                                    MQTTPublishInfo_t * pxPublishInfo )
{
    bool isMatch = false;

    ( void ) MQTT_MatchTopic( pxPublishInfo->pTopicName,
                              pxPublishInfo->topicNameLength,
                              OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER,
                              OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER_LENGTH,
                              &isMatch );

    if( isMatch == true )
    {
        prvMqttJobCallback( pvIncomingPublishCallbackContext, pxPublishInfo );
    }
}

/*-----------------------------------------------------------*/
static void prvMqttDataCallback( void * pvIncomingPublishCallbackContext,
                                 MQTTPublishInfo_t * pxPublishInfo )
{
    OtaEventData_t * pData;
    OtaEventMsg_t eventMsg = { 0 };

    configASSERT( pxPublishInfo != NULL );
    ( void ) pvIncomingPublishCallbackContext;

    LogInfo( ( "Received data message callback, size %zu.\n\n", pxPublishInfo->payloadLength ) );

    pData = otaEventBufferGet();

    if( pData != NULL )
    {
        memcpy( pData->data, pxPublishInfo->pPayload, pxPublishInfo->payloadLength );
        pData->dataLength = pxPublishInfo->payloadLength;
        eventMsg.eventId = OtaAgentEventReceivedFileBlock;
        eventMsg.pEventData = pData;

        /* Send job document received event. */
        OTA_SignalEvent( &eventMsg );
    }
    else
    {
        LogError( ( "Error: No OTA data buffers available.\r\n" ) );
    }
}

/*-----------------------------------------------------------*/
static void prvMQTTAgentCmdCompleteCallback( CommandContext_t * pxCommandContext,
                                             MQTTAgentReturnInfo_t * pxReturnInfo )
{
    /* Store the result in the application defined context so the task that
     * initiated the publish can check the operation's status. */
    pxCommandContext->xReturnStatus = pxReturnInfo->returnCode;

    if( pxCommandContext->xTaskToNotify != NULL )
    {
        /* Send the context's ulNotificationValue as the notification value so
         * the receiving task can check the value it set in the context matches
         * the value it receives in the notification. */
        xTaskNotify( pxCommandContext->xTaskToNotify,
                     pxCommandContext->ulNotificationValue,
                     eSetValueWithOverwrite );
    }
}

/*-----------------------------------------------------------*/

static void prvRegisterOTACallback( const char * pTopicFilter,
                                    uint16_t topicFilterLength )
{
    bool isMatch = false;
    MQTTStatus_t mqttStatus = MQTTSuccess;
    uint16_t index = 0U;
    uint16_t numTopicFilters = sizeof( otaTopicFilterCallbacks ) / sizeof( OtaTopicFilterCallback_t );


    bool subscriptionAdded;

    ( void ) mqttStatus;

    /* Match the input topic filter against the wild-card pattern of topics filters
    * relevant for the OTA Update service to determine the type of topic filter. */
    for( ; index < numTopicFilters; index++ )
    {
        mqttStatus = MQTT_MatchTopic( pTopicFilter,
                                      topicFilterLength,
                                      otaTopicFilterCallbacks[ index ].pTopicFilter,
                                      otaTopicFilterCallbacks[ index ].topicFilterLength,
                                      &isMatch );
        assert( mqttStatus == MQTTSuccess );

        if( isMatch )
        {
            /* Add subscription so that incoming publishes are routed to the application callback. */
            subscriptionAdded = addSubscription( ( SubscriptionElement_t * ) xGlobalMqttAgentContext.pIncomingCallbackContext,
                                                 pTopicFilter,
                                                 topicFilterLength,
                                                 otaTopicFilterCallbacks[ index ].callback,
                                                 NULL );

            if( subscriptionAdded == false )
            {
                LogError( ( "Failed to register a publish callback for topic %.*s.",
                            pTopicFilter,
                            topicFilterLength ) );
            }
        }
    }
}

/*-----------------------------------------------------------*/

static void prvMQTTSubscribeCompleteCallback( CommandContext_t * pxCommandContext,
                                              MQTTAgentReturnInfo_t * pxReturnInfo )
{
    MQTTAgentSubscribeArgs_t * pSubsribeArgs;

    if( pxReturnInfo->returnCode == MQTTSuccess )
    {
        pSubsribeArgs = ( MQTTAgentSubscribeArgs_t * ) ( pxCommandContext->pArgs );
        prvRegisterOTACallback( pSubsribeArgs->pSubscribeInfo->pTopicFilter, pSubsribeArgs->pSubscribeInfo->topicFilterLength );
    }

    /* Store the result in the application defined context so the task that
     * initiated the publish can check the operation's status. */
    pxCommandContext->xReturnStatus = pxReturnInfo->returnCode;

    if( pxCommandContext->xTaskToNotify != NULL )
    {
        /* Send the context's ulNotificationValue as the notification value so
         * the receiving task can check the value it set in the context matches
         * the value it receives in the notification. */
        xTaskNotify( pxCommandContext->xTaskToNotify,
                     pxCommandContext->ulNotificationValue,
                     eSetValueWithOverwrite );
    }
}

/*-----------------------------------------------------------*/

static void prvMQTTUnsubscribeCompleteCallback( CommandContext_t * pxCommandContext,
                                                MQTTAgentReturnInfo_t * pxReturnInfo )
{
    MQTTAgentSubscribeArgs_t * pSubsribeArgs;

    if( pxReturnInfo->returnCode == MQTTSuccess )
    {
        pSubsribeArgs = ( MQTTAgentSubscribeArgs_t * ) ( pxCommandContext->pArgs );
        /* Add subscription so that incoming publishes are routed to the application callback. */
        removeSubscription( ( SubscriptionElement_t * ) xGlobalMqttAgentContext.pIncomingCallbackContext,
                            pSubsribeArgs->pSubscribeInfo->pTopicFilter,
                            pSubsribeArgs->pSubscribeInfo->topicFilterLength );

        LogInfo( ( "Removed registration for topic %.*s.",
                   pSubsribeArgs->pSubscribeInfo->topicFilterLength,
                   pSubsribeArgs->pSubscribeInfo->pTopicFilter ) );
    }

    /* Store the result in the application defined context so the task that
     * initiated the publish can check the operation's status. */
    pxCommandContext->xReturnStatus = pxReturnInfo->returnCode;

    if( pxCommandContext->xTaskToNotify != NULL )
    {
        /* Send the context's ulNotificationValue as the notification value so
         * the receiving task can check the value it set in the context matches
         * the value it receives in the notification. */
        xTaskNotify( pxCommandContext->xTaskToNotify,
                     pxCommandContext->ulNotificationValue,
                     eSetValueWithOverwrite );
    }
}

static int32_t prvGenerateRandomNumber()
{
    uint32_t ulRandomNum;

    /* Set the return value as negative to indicate failure. */
    int32_t lOutput = -1;

    /* Use the PKCS11 module to generate a random number. */
    if( xPkcs11GenerateRandomNumber( ( uint8_t * ) &ulRandomNum,
                                     ( sizeof( ulRandomNum ) ) ) == pdPASS )
    {
        lOutput = ( int32_t ) ( ulRandomNum & INT32_MAX );
    }

    return lOutput;
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

static MQTTStatus_t prvMqttInit( void )
{
    TransportInterface_t xTransport;
    MQTTStatus_t xReturn;
    MQTTFixedBuffer_t xFixedBuffer = { .pBuffer = xNetworkBuffer, .size = MQTT_AGENT_NETWORK_BUFFER_SIZE };
    static uint8_t staticQueueStorageArea[ MQTT_AGENT_COMMAND_QUEUE_LENGTH * sizeof( Command_t * ) ];
    static StaticQueue_t staticQueueStructure;

    LogDebug( ( "Creating command queue." ) );
    xCommandQueue.queue = xQueueCreateStatic( MQTT_AGENT_COMMAND_QUEUE_LENGTH,
                                              sizeof( Command_t * ),
                                              staticQueueStorageArea,
                                              &staticQueueStructure );

    /* Fill in Transport Interface send and receive function pointers. */
    xTransport.pNetworkContext = &networkContextMqtt;
    xTransport.send = SecureSocketsTransport_Send;
    xTransport.recv = SecureSocketsTransport_Recv;

    /* Initialize MQTT library. */
    xReturn = MQTTAgent_Init( &xGlobalMqttAgentContext,
                              &xCommandQueue,
                              &xFixedBuffer,
                              &xTransport,
                              prvGetTimeMs,
                              prvIncomingPublishCallback,
                              /* Context to pass into the callback. Passing the pointer to subscription array. */
                              xGlobalSubscriptionList );

    return xReturn;
}

/*-----------------------------------------------------------*/
static BaseType_t prvCreateSocketConnectionToMQTTBroker( NetworkContext_t * pNetworkContext )
{
    ServerInfo_t xServerInfo = { 0 };
    SocketsConfig_t xSocketsConfig = { 0 };
    BaseType_t xStatus = pdPASS;
    TransportSocketStatus_t xNetworkStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;
    BackoffAlgorithmStatus_t xBackoffAlgStatus = BackoffAlgorithmSuccess;
    BackoffAlgorithmContext_t xReconnectParams;
    uint16_t usNextRetryBackOff = 0U;

    /* Set the credentials for establishing a TLS connection. */
    /* Initializer server information. */
    xServerInfo.pHostName = democonfigMQTT_BROKER_ENDPOINT;
    xServerInfo.hostNameLength = strlen( democonfigMQTT_BROKER_ENDPOINT );
    xServerInfo.port = democonfigMQTT_BROKER_PORT;

    /* Configure credentials for TLS mutual authenticated session. */
    xSocketsConfig.enableTls = true;
    xSocketsConfig.pAlpnProtos = NULL;
    xSocketsConfig.maxFragmentLength = 0;
    xSocketsConfig.disableSni = false;
    xSocketsConfig.pRootCa = democonfigROOT_CA_PEM;
    xSocketsConfig.rootCaSize = sizeof( democonfigROOT_CA_PEM );
    xSocketsConfig.sendTimeoutMs = otaexampleMQTT_TRANSPORT_SEND_RECV_TIMEOUT_MS;
    xSocketsConfig.recvTimeoutMs = otaexampleMQTT_TRANSPORT_SEND_RECV_TIMEOUT_MS;

    /* Initialize reconnect attempts and interval. */
    BackoffAlgorithm_InitializeParams( &xReconnectParams,
                                       RETRY_BACKOFF_BASE_MS,
                                       RETRY_MAX_BACKOFF_DELAY_MS,
                                       RETRY_MAX_ATTEMPTS,
                                       prvGenerateRandomNumber );

    /* Attempt to connect to MQTT broker. If connection fails, retry after
     * a timeout. Timeout value will exponentially increase till maximum
     * attempts are reached.
     */
    do
    {
        /* Establish a TLS session with the MQTT broker. This example connects to
         * the MQTT broker as specified in democonfigMQTT_BROKER_ENDPOINT and
         * democonfigMQTT_BROKER_PORT at the top of this file. */
        LogInfo( ( "Creating a TLS connection to %s:%u.",
                   democonfigMQTT_BROKER_ENDPOINT,
                   democonfigMQTT_BROKER_PORT ) );
        /* Attempt to create a mutually authenticated TLS connection. */
        xNetworkStatus = SecureSocketsTransport_Connect( pNetworkContext,
                                                         &xServerInfo,
                                                         &xSocketsConfig );

        if( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
        {
            /* Get back-off value (in milliseconds) for the next connection retry. */
            xBackoffAlgStatus = BackoffAlgorithm_GetNextBackoff( &xReconnectParams, &usNextRetryBackOff );
            configASSERT( xBackoffAlgStatus != BackoffAlgorithmRngFailure );

            if( xBackoffAlgStatus == BackoffAlgorithmRetriesExhausted )
            {
                LogError( ( "Connection to the broker failed, all attempts exhausted." ) );
                xStatus = pdFAIL;
            }
            else if( xBackoffAlgStatus == BackoffAlgorithmSuccess )
            {
                LogWarn( ( "Connection to the broker failed. Retrying connection after backoff delay." ) );
                vTaskDelay( pdMS_TO_TICKS( usNextRetryBackOff ) );

                LogInfo( ( "Retry attempt %lu out of maximum retry attempts %lu.",
                           ( xReconnectParams.attemptsDone + 1 ),
                           xReconnectParams.maxRetryAttempts ) );
            }
        }
    } while( ( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS ) && ( xBackoffAlgStatus == BackoffAlgorithmSuccess ) );

    return xStatus;
}

static MQTTStatus_t prvMQTTConnect( void )
{
    MQTTStatus_t mqttStatus = MQTTBadParameter;
    MQTTConnectInfo_t connectInfo = { 0 };

    bool sessionPresent = false;

    /* Establish MQTT session by sending a CONNECT packet. */

    /* If #createCleanSession is true, start with a clean session
     * i.e. direct the MQTT broker to discard any previous session data.
     * If #createCleanSession is false, directs the broker to attempt to
     * reestablish a session which was already present. */
    connectInfo.cleanSession = true;

    /* The client identifier is used to uniquely identify this MQTT client to
     * the MQTT broker. In a production device the identifier can be something
     * unique, such as a device serial number. */
    connectInfo.pClientIdentifier = democonfigCLIENT_IDENTIFIER;
    connectInfo.clientIdentifierLength = ( uint16_t ) strlen( democonfigCLIENT_IDENTIFIER );

    /* The maximum time interval in seconds which is allowed to elapse
     * between two Control Packets.
     * It is the responsibility of the Client to ensure that the interval between
     * Control Packets being sent does not exceed the this Keep Alive value. In the
     * absence of sending any other Control Packets, the Client MUST send a
     * PINGREQ Packet. */
    connectInfo.keepAliveSeconds = MQTT_KEEP_ALIVE_INTERVAL_SECONDS;

    /* Send MQTT CONNECT packet to broker. */
    mqttStatus = MQTT_Connect( &xGlobalMqttAgentContext.mqttContext, &connectInfo, NULL, CONNACK_RECV_TIMEOUT_MS, &sessionPresent );

    return mqttStatus;
}

static BaseType_t prvConnectToMQTTBroker( void )
{
    BaseType_t xStatus = pdFAIL;

    /* Attempt to connect to the MQTT broker. If connection fails, retry after
     * a timeout. Timeout value will be exponentially increased till the maximum
     * attempts are reached or maximum timeout value is reached. The function
     * returns EXIT_FAILURE if the TCP connection cannot be established to
     * broker after configured number of attempts. */
    xStatus = prvCreateSocketConnectionToMQTTBroker( &networkContextMqtt );

    if( xStatus != pdPASS )
    {
        /* Log error to indicate connection failure. */
        LogError( ( "Failed to connect to MQTT broker %s.",
                    democonfigMQTT_BROKER_ENDPOINT ) );
    }

    if( xStatus == pdPASS )
    {
        if( prvMqttInit() != MQTTSuccess )
        {
            LogError( ( "Failed initializing MQTT agent." ) );
            xStatus = pdFAIL;
        }
    }

    if( xStatus == pdPASS )
    {
        /* Sends an MQTT Connect packet over the already established TLS connection,
         * and waits for connection acknowledgment (CONNACK) packet. */
        LogInfo( ( "Creating an MQTT connection to %s.", democonfigMQTT_BROKER_ENDPOINT ) );

        if( prvMQTTConnect() != MQTTSuccess )
        {
            LogError( ( "Failed creating an MQTT connection to %s.",
                        democonfigMQTT_BROKER_ENDPOINT ) );
        }
        else
        {
            LogDebug( ( "Success creating MQTT connection to %s.",
                        democonfigMQTT_BROKER_ENDPOINT ) );
        }
    }

    return xStatus;
}

static void prvDisconnectFromMQTTBroker( void )
{
    CommandContext_t xCommandContext = { 0 };
    CommandInfo_t xCommandParams = { 0 };
    MQTTStatus_t xCommandStatus;

    /* Disconnect from broker. */
    LogInfo( ( "Disconnecting the MQTT connection with %s.", democonfigMQTT_BROKER_ENDPOINT ) );

    xCommandParams.blockTimeMs = MQTT_AGENT_SEND_BLOCK_TIME_MS;
    xCommandParams.cmdCompleteCallback = prvMQTTAgentCmdCompleteCallback;
    xCommandParams.pCmdCompleteCallbackContext = &xCommandContext;
    xCommandContext.xTaskToNotify = xTaskGetCurrentTaskHandle();
    xCommandContext.pArgs = NULL;
    xCommandContext.xReturnStatus = MQTTSendFailed;

    /* Disconnect MQTT session. */
    xCommandStatus = MQTTAgent_Disconnect( &xGlobalMqttAgentContext, &xCommandParams );
    configASSERT( xCommandStatus == MQTTSuccess );

    xTaskNotifyWait( 0,
                     0,
                     NULL,
                     pdMS_TO_TICKS( MQTT_AGENT_MS_TO_WAIT_FOR_NOTIFICATION ) );

    /* End TLS session, then close TCP connection. */
    ( void ) SecureSocketsTransport_Disconnect( &networkContextMqtt );
}

/*-----------------------------------------------------------*/



static OtaMqttStatus_t prvMqttSubscribe( const char * pTopicFilter,
                                         uint16_t topicFilterLength,
                                         uint8_t qos )
{
    OtaMqttStatus_t otaRet = OtaMqttSuccess;
    MQTTStatus_t commandStatus;
    CommandInfo_t commandParams = { 0 };
    CommandContext_t commandContext = { 0 };
    MQTTSubscribeInfo_t subscription = { 0 };
    MQTTAgentSubscribeArgs_t subscribeArgs = { 0 };


    assert( pTopicFilter != NULL );
    assert( topicFilterLength > 0 );

    subscription.qos = qos;
    subscription.pTopicFilter = pTopicFilter;
    subscription.topicFilterLength = topicFilterLength;
    subscribeArgs.numSubscriptions = 1;
    subscribeArgs.pSubscribeInfo = &subscription;


    commandParams.blockTimeMs = MQTT_AGENT_SEND_BLOCK_TIME_MS;
    commandParams.cmdCompleteCallback = prvMQTTSubscribeCompleteCallback;
    commandParams.pCmdCompleteCallbackContext = &commandContext;
    commandContext.xTaskToNotify = xTaskGetCurrentTaskHandle();
    commandContext.pArgs = &subscribeArgs;
    commandContext.xReturnStatus = MQTTSendFailed;

    /* Disconnect MQTT session. */
    commandStatus = MQTTAgent_Subscribe( &xGlobalMqttAgentContext, &subscribeArgs, &commandParams );
    configASSERT( commandStatus == MQTTSuccess );

    xTaskNotifyWait( 0,
                     0,
                     NULL,
                     pdMS_TO_TICKS( MQTT_AGENT_MS_TO_WAIT_FOR_NOTIFICATION ) );

    if( commandContext.xReturnStatus != MQTTSuccess )
    {
        LogError( ( "Failed to send SUBSCRIBE packet to broker with error = %u.", commandContext.xReturnStatus ) );
        otaRet = OtaMqttSubscribeFailed;
    }
    else
    {
        LogInfo( ( "SUBSCRIBED to topic %.*s to broker.\n\n",
                   topicFilterLength,
                   pTopicFilter ) );
    }

    return otaRet;
}

static OtaMqttStatus_t prvMqttPublish( const char * const pTopic,
                                       uint16_t topicLen,
                                       const char * pMsg,
                                       uint32_t msgSize,
                                       uint8_t qos )
{
    OtaMqttStatus_t otaRet = OtaMqttSuccess;
    MQTTStatus_t commandStatus;
    CommandInfo_t commandParams = { 0 };
    CommandContext_t commandContext = { 0 };
    MQTTPublishInfo_t publishInfo = { 0 };

    /* Set the required publish parameters. */
    publishInfo.pTopicName = pTopic;
    publishInfo.topicNameLength = topicLen;
    publishInfo.qos = qos;
    publishInfo.pPayload = pMsg;
    publishInfo.payloadLength = msgSize;

    commandParams.blockTimeMs = MQTT_AGENT_SEND_BLOCK_TIME_MS;
    commandParams.cmdCompleteCallback = prvMQTTAgentCmdCompleteCallback;
    commandParams.pCmdCompleteCallbackContext = &commandContext;
    commandContext.xTaskToNotify = xTaskGetCurrentTaskHandle();
    commandContext.pArgs = NULL;
    commandContext.xReturnStatus = MQTTSendFailed;

    commandStatus = MQTTAgent_Publish( &xGlobalMqttAgentContext, &publishInfo, &commandParams );
    configASSERT( commandStatus == MQTTSuccess );

    xTaskNotifyWait( 0,
                     0,
                     NULL,
                     pdMS_TO_TICKS( MQTT_AGENT_MS_TO_WAIT_FOR_NOTIFICATION ) );

    if( commandContext.xReturnStatus != MQTTSuccess )
    {
        LogError( ( "Failed to send PUBLISH packet to broker with error = %u.", commandContext.xReturnStatus ) );
        otaRet = OtaMqttPublishFailed;
    }
    else
    {
        LogInfo( ( "Sent PUBLISH packet to broker %.*s to broker.\n\n",
                   topicLen,
                   pTopic ) );
    }

    return otaRet;
}

static OtaMqttStatus_t prvMqttUnSubscribe( const char * pTopicFilter,
                                           uint16_t topicFilterLength,
                                           uint8_t qos )
{
    OtaMqttStatus_t otaRet = OtaMqttSuccess;
    MQTTStatus_t commandStatus;
    CommandInfo_t commandParams = { 0 };
    CommandContext_t commandContext = { 0 };
    MQTTSubscribeInfo_t subscription = { 0 };
    MQTTAgentSubscribeArgs_t subscribeArgs = { 0 };

    subscription.qos = qos;
    subscription.pTopicFilter = pTopicFilter;
    subscription.topicFilterLength = topicFilterLength;
    subscribeArgs.numSubscriptions = 1;
    subscribeArgs.pSubscribeInfo = &subscription;


    commandParams.blockTimeMs = MQTT_AGENT_SEND_BLOCK_TIME_MS;
    commandParams.cmdCompleteCallback = prvMQTTSubscribeCompleteCallback;
    commandParams.pCmdCompleteCallbackContext = &commandContext;
    commandContext.xTaskToNotify = xTaskGetCurrentTaskHandle();
    commandContext.pArgs = &subscribeArgs;
    commandContext.xReturnStatus = MQTTSendFailed;

    /* Unsubscribe to topics. */
    commandStatus = MQTTAgent_Unsubscribe( &xGlobalMqttAgentContext, &subscribeArgs, &commandParams );
    configASSERT( commandStatus == MQTTSuccess );

    xTaskNotifyWait( 0,
                     0,
                     NULL,
                     pdMS_TO_TICKS( MQTT_AGENT_MS_TO_WAIT_FOR_NOTIFICATION ) );

    if( commandContext.xReturnStatus != MQTTSuccess )
    {
        LogError( ( "Failed to send unsubsribe packet to broker with error = %u.", commandContext.xReturnStatus ) );
        otaRet = OtaMqttSubscribeFailed;
    }
    else
    {
        LogInfo( ( "Unsubsribed from topic %.*s to broker.\n\n",
                   topicFilterLength,
                   pTopicFilter ) );
    }

    return otaRet;
}

/*-----------------------------------------------------------*/

static void setOtaInterfaces( OtaInterfaces_t * pOtaInterfaces )
{
    /* Initialize OTA library OS Interface. */
    pOtaInterfaces->os.event.init = OtaInitEvent_FreeRTOS;
    pOtaInterfaces->os.event.send = OtaSendEvent_FreeRTOS;
    pOtaInterfaces->os.event.recv = OtaReceiveEvent_FreeRTOS;
    pOtaInterfaces->os.event.deinit = OtaDeinitEvent_FreeRTOS;
    pOtaInterfaces->os.timer.start = OtaStartTimer_FreeRTOS;
    pOtaInterfaces->os.timer.stop = OtaStopTimer_FreeRTOS;
    pOtaInterfaces->os.timer.delete = OtaDeleteTimer_FreeRTOS;
    pOtaInterfaces->os.mem.malloc = Malloc_FreeRTOS;
    pOtaInterfaces->os.mem.free = Free_FreeRTOS;

    /* Initialize the OTA library MQTT Interface.*/
    pOtaInterfaces->mqtt.subscribe = prvMqttSubscribe;
    pOtaInterfaces->mqtt.publish = prvMqttPublish;
    pOtaInterfaces->mqtt.unsubscribe = prvMqttUnSubscribe;

    /* Initialize the OTA library PAL Interface.*/
    pOtaInterfaces->pal.getPlatformImageState = otaPal_GetPlatformImageState;
    pOtaInterfaces->pal.setPlatformImageState = otaPal_SetPlatformImageState;
    pOtaInterfaces->pal.writeBlock = otaPal_WriteBlock;
    pOtaInterfaces->pal.activate = otaPal_ActivateNewImage;
    pOtaInterfaces->pal.closeFile = otaPal_CloseFile;
    pOtaInterfaces->pal.reset = otaPal_ResetDevice;
    pOtaInterfaces->pal.abort = otaPal_Abort;
    pOtaInterfaces->pal.createFile = otaPal_CreateFileForRx;
}

/*-----------------------------------------------------------*/

static void prvOTAAgentTask( void * pParam )
{
    /* Calling OTA agent task. */
    OTA_EventProcessingTask( pParam );
    LogInfo( ( "OTA Agent stopped." ) );

    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/

static void prvMQTTAgentTask( void * pParam )
{
    BaseType_t xResult = pdFAIL;
    MQTTStatus_t xMQTTStatus = MQTTSuccess;

    ( void ) pParam;

    do
    {
        /* MQTTAgent_CommandLoop() is effectively the agent implementation.  It
         * will manage the MQTT protocol until such time that an error occurs,
         * which could be a disconnect.  If an error occurs the MQTT context on
         * which the error happened is returned so there can be an attempt to
         * clean up and reconnect however the application writer prefers. */
        xMQTTStatus = MQTTAgent_CommandLoop( &xGlobalMqttAgentContext );

        /* Clear Agent queue so that no any pending MQTT operations are processed. */
        xQueueReset( xCommandQueue.queue );

        /* Success is returned for application intiated disconnect or termination. The socket will also be disconnected by the caller. */
        if( xMQTTStatus != MQTTSuccess )
        {
            xResult = prvSuspendOTA();
            configASSERT( xResult == pdPASS );

            LogInfo( ( "Suspended OTA agent." ) );

            /* Reconnect TCP. */
            ( void ) SecureSocketsTransport_Disconnect( &networkContextMqtt );

            xResult = prvConnectToMQTTBroker();
            configASSERT( xResult == pdPASS );

            xResult = prvResumeOTA();
            configASSERT( xResult == pdPASS );

            LogInfo( ( "Resumed OTA agent." ) );
        }
    } while( xMQTTStatus != MQTTSuccess );

    vTaskDelete( NULL );
}

static BaseType_t prvSuspendOTA( void )
{
    /* OTA library return status. */
    OtaErr_t otaRet = OtaErrNone;
    BaseType_t status = pdPASS;
    uint32_t suspendTimeout;

    otaRet = OTA_Suspend();

    if( otaRet == OtaErrNone )
    {
        suspendTimeout = OTA_SUSPEND_TIMEOUT_MS;

        while( ( OTA_GetState() != OtaAgentStateSuspended ) && ( suspendTimeout > 0 ) )
        {
            /* Wait for OTA Library state to suspend */
            vTaskDelay( pdMS_TO_TICKS( OTA_EXAMPLE_TASK_DELAY_MS ) );
            suspendTimeout -= OTA_EXAMPLE_TASK_DELAY_MS;
        }

        if( OTA_GetState() != OtaAgentStateSuspended )
        {
            LogError( ( "Failed to suspend OTA." ) );
            status = pdFAIL;
        }
    }
    else
    {
        LogError( ( "Error while trying to suspend OTA agent %d", otaRet ) );
        status = pdFAIL;
    }

    return status;
}

static BaseType_t prvResumeOTA( void )
{
    /* OTA library return status. */
    OtaErr_t otaRet = OtaErrNone;
    BaseType_t status = pdPASS;
    uint32_t suspendTimeout;

    otaRet = OTA_Resume();

    if( otaRet == OtaErrNone )
    {
        suspendTimeout = OTA_SUSPEND_TIMEOUT_MS;

        while( ( OTA_GetState() == OtaAgentStateSuspended ) && ( suspendTimeout > 0 ) )
        {
            /* Wait for OTA Library state to suspend */
            vTaskDelay( pdMS_TO_TICKS( OTA_EXAMPLE_TASK_DELAY_MS ) );
            suspendTimeout -= OTA_EXAMPLE_TASK_DELAY_MS;
        }

        if( OTA_GetState() == OtaAgentStateSuspended )
        {
            LogError( ( "Failed to resume OTA." ) );
            status = pdFAIL;
        }
    }
    else
    {
        LogError( ( "Error while trying to resume OTA agent %d", otaRet ) );
        status = pdFAIL;
    }

    return status;
}

/*-----------------------------------------------------------*/

static BaseType_t prvRunOTADemo( void )
{
    /* Status indicating a successful demo or not. */
    BaseType_t xStatus = pdPASS;

    /* OTA library return status. */
    OtaErr_t otaRet = OtaErrNone;

    /* OTA event message used for sending event to OTA Agent.*/
    OtaEventMsg_t eventMsg = { 0 };

    /* OTA interface context required for library interface functions.*/
    OtaInterfaces_t otaInterfaces;

    /* OTA library packet statistics per job.*/
    OtaAgentStatistics_t otaStatistics = { 0 };

    /* OTA Agent state returned from calling OTA_GetState.*/
    OtaState_t state = OtaAgentStateStopped;

    /* Set OTA Library interfaces.*/
    setOtaInterfaces( &otaInterfaces );

    /****************************** Init OTA Library. ******************************/

    if( xStatus == pdPASS )
    {
        if( ( otaRet = OTA_Init( &otaBuffer,
                                 &otaInterfaces,
                                 ( const uint8_t * ) ( democonfigCLIENT_IDENTIFIER ),
                                 otaAppCallback ) ) != OtaErrNone )
        {
            LogError( ( "Failed to initialize OTA Agent, exiting = %u.",
                        otaRet ) );

            xStatus = pdFAIL;
        }
    }

    /****************************** Create OTA Agent Task. ******************************/

    if( xStatus == pdPASS )
    {
        xStatus = xTaskCreate( prvOTAAgentTask,
                               "OTA Agent Task",
                               OTA_AGENT_TASK_STACK_SIZE,
                               NULL,
                               OTA_AGENT_TASK_PRIORITY,
                               NULL );

        if( xStatus != pdPASS )
        {
            LogError( ( "Failed to create OTA agent task:" ) );
        }
    }

    /**
     * Register a callback for receiving messages intended for OTA agent from broker,
     * for which the topic has not been subscribed for.
     */
    prvRegisterOTACallback( OTA_DEFAULT_TOPIC_FILTER, OTA_DEFAULT_TOPIC_FILTER_LENGTH );

    /****************************** Start OTA ******************************/

    if( xStatus == pdPASS )
    {
        /* Send start event to OTA Agent.*/
        eventMsg.eventId = OtaAgentEventStart;
        OTA_SignalEvent( &eventMsg );
    }

    /****************************** Loop and display OTA statistics ******************************/

    if( xStatus == pdPASS )
    {
        while( ( state = OTA_GetState() ) != OtaAgentStateStopped )
        {
            /* Get OTA statistics for currently executing job. */
            if( state != OtaAgentStateSuspended )
            {
                OTA_GetStatistics( &otaStatistics );

                LogInfo( ( " Received: %u   Queued: %u   Processed: %u   Dropped: %u",
                           otaStatistics.otaPacketsReceived,
                           otaStatistics.otaPacketsQueued,
                           otaStatistics.otaPacketsProcessed,
                           otaStatistics.otaPacketsDropped ) );
            }

            vTaskDelay( pdMS_TO_TICKS( OTA_EXAMPLE_TASK_DELAY_MS ) );
        }
    }

    /**
     * Remvove callback for receiving messages intended for OTA agent from broker,
     * for which the topic has not been subscribed for.
     */
    removeSubscription( ( SubscriptionElement_t * ) xGlobalMqttAgentContext.pIncomingCallbackContext,
                        OTA_DEFAULT_TOPIC_FILTER,
                        OTA_DEFAULT_TOPIC_FILTER_LENGTH );

    return xStatus;
}

/*-----------------------------------------------------------*/

/**
 * @brief Entry point of demo.
 *
 * This example initializes the OTA library to enable OTA updates via the
 * MQTT broker. It simply connects to the MQTT broker with the users
 * credentials and spins in an indefinite loop to allow MQTT messages to be
 * forwarded to the OTA agent for possible processing. The OTA agent does all
 * of the real work; checking to see if the message topic is one destined for
 * the OTA agent. If not, it is simply ignored.
 *
 * @param[in] awsIotMqttMode Specify if this demo is running with the AWS IoT
 * MQTT server. Set this to `false` if using another MQTT server.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkInterface The network interface to use for this demo.
 *
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE` otherwise.
 *
 */
int RunOtaCoreMqttDemo( bool awsIotMqttMode,
                        const char * pIdentifier,
                        void * pNetworkServerInfo,
                        void * pNetworkCredentialInfo,
                        const IotNetworkInterface_t * pNetworkInterface )
{
    ( void ) awsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;
    ( void ) pNetworkInterface;

    /* Return error status. */
    int returnStatus = EXIT_SUCCESS;

    bool mqttInitialized = false;

    LogInfo( ( "OTA over MQTT demo, Application version %u.%u.%u",
               appFirmwareVersion.u.x.major,
               appFirmwareVersion.u.x.minor,
               appFirmwareVersion.u.x.build ) );

    /* Initialize semaphore for buffer operations. */
    xBufferSemaphore = xSemaphoreCreateMutex();

    if( xBufferSemaphore == NULL )
    {
        LogError( ( "Failed to initialize buffer semaphore." ) );
        returnStatus = EXIT_FAILURE;
    }

    /****************************** Init MQTT ******************************/

    if( returnStatus == EXIT_SUCCESS )
    {
        if( prvConnectToMQTTBroker() != pdPASS )
        {
            LogError( ( "Failed to initialize MQTT, exiting" ) );
            returnStatus = EXIT_FAILURE;
        }
        else
        {
            mqttInitialized = true;
        }
    }

    /****************************** Create MQTT Agent Task. ******************************/

    if( returnStatus == EXIT_SUCCESS )
    {
        if( xTaskCreate( prvMQTTAgentTask,
                         "MQTT Agent Task",
                         MQTT_AGENT_TASK_STACK_SIZE,
                         NULL,
                         MQTT_AGENT_TASK_PRIORITY,
                         NULL ) != pdPASS )
        {
            returnStatus = EXIT_FAILURE;
            LogError( ( "Failed to create MQTT agent task:" ) );
        }
    }

    if( returnStatus == EXIT_SUCCESS )
    {
        /* Start OTA demo. The function returns only if OTA completes successfully and a
         * shutdown of OTA is triggered for a manual restart of the device.*/
        if( prvRunOTADemo() != pdPASS )
        {
            returnStatus = EXIT_FAILURE;
        }
    }

    /****************************** Cleanup ******************************/

    if( mqttInitialized )
    {
        prvDisconnectFromMQTTBroker();
    }

    if( xBufferSemaphore != NULL )
    {
        /* Cleanup semaphore created for buffer operations. */
        vSemaphoreDelete( xBufferSemaphore );
    }

    return returnStatus;
}
