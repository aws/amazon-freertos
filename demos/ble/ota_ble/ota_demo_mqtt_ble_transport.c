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

/**
 * @file ota_demo_mqtt_ble_transport.c
 * @brief OTA update example using BLE transport interface.
 */

/* Standard includes. */
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

/* OTA library and demo configuration macros. */
#include "ota_config.h"
#include "ota_demo_config.h"

#include "aws_demo_config.h"

#include "iot_ble_config.h"

/* Include header for connection configurations. */
#include "aws_clientcredential.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* CoreMQTT-Agent APIS for running MQTT in a multithreaded environment. */
#include "freertos_agent_message.h"
#include "freertos_command_pool.h"

/* CoreMQTT-Agent include. */
#include "core_mqtt_agent.h"

/* Includes helpers for managing MQTT subscriptions. */
#include "mqtt_subscription_manager.h"


/* BLE transport interface headers */
#include "iot_ble_mqtt_transport.h"

/* OTA Library include. */
#include "ota.h"

/* OTA Library Interface include. */
#include "ota_os_freertos.h"
#include "ota_mqtt_interface.h"

/* PAL abstraction layer APIs. */
#include "ota_pal.h"

/* Includes the OTA Application version number. */
#include "ota_appversion32.h"

/*------------- Demo configurations -------------------------*/

/**
 * @brief The MQTT broker endpoint used for this demo.
 */
#ifndef democonfigMQTT_BROKER_ENDPOINT
    #define democonfigMQTT_BROKER_ENDPOINT    clientcredentialMQTT_BROKER_ENDPOINT
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
 * @brief The delay used in the main OTA Demo task loop to periodically output
 * the OTA statistics like number of packets received, dropped, processed and
 * queued per connection.
 */
#define otaexampleEXAMPLE_TASK_DELAY_MS    ( 1000U )

/**
 * @brief The timeout for waiting for the agent to get suspended after closing
 * the connection.
 *
 * Timeout value should be large enough for OTA agent to finish any pending MQTT
 * operations and suspend itself.
 *
 */
#define otaexampleSUSPEND_TIMEOUT_MS       ( 10000U )

/**
 * @brief The maximum size of the file paths used in the demo.
 */
#define otaexampleMAX_FILE_PATH_SIZE       ( 260U )

/**
 * @brief The maximum size of the stream name required for downloading update
 * file from streaming service.
 */
#define otaexampleMAX_STREAM_NAME_SIZE     ( 128U )

/**
 * @brief Size of the network buffer to receive the MQTT message.
 *
 * The largest message size is data size from the AWS IoT streaming service,
 * otaconfigFILE_BLOCK_SIZE + extra for headers.
 */

#define otaexampleNETWORK_BUFFER_SIZE               ( 5120U )

/**
 * @brief The common prefix for all OTA topics.
 *
 * Thing name is substituted with a wildcard symbol `+`. OTA agent
 * registers with MQTT broker with the thing name in the topic. This topic
 * filter is used to match incoming packet received and route them to OTA.
 * Thing name is not needed for this matching.
 */
#define otaexampleTOPIC_PREFIX                      "$aws/things/+/"

/**
 * @brief Wildcard topic filter for job notification.
 * The filter is used to match the constructed job notify topic filter from OTA
 * agent and register appropirate callback for it.
 */
#define otaexampleJOB_NOTIFY_TOPIC_FILTER           otaexampleTOPIC_PREFIX "jobs/notify-next"

/**
 * @brief Length of job notification topic filter.
 */
#define otaexampleJOB_NOTIFY_TOPIC_FILTER_LENGTH    ( ( uint16_t ) ( sizeof( otaexampleJOB_NOTIFY_TOPIC_FILTER ) - 1 ) )


/**
 * @brief Wildcard topic filter for matching job response messages.
 * This topic filter is used to match the responses from OTA service for OTA
 * agent job requests.
 */
#define otaexampleJOB_RESPONSE_TOPIC_FILTER           otaexampleTOPIC_PREFIX "jobs/$next/get/+"

/**
 * @brief Length of job accepted response topic filter.
 */
#define otaexampleJOB_RESPONSE_TOPIC_FILTER_LENGTH    ( ( uint16_t ) ( sizeof( otaexampleJOB_RESPONSE_TOPIC_FILTER ) - 1 ) )


/**
 * @brief Wildcard topic filter for matching job accepted response messages.
 * This topic filter is used to match the accepted job responses from OTA service for the OTA
 * agent job requests.
 */
#define otaexampleJOB_ACCEPTED_RESPONSE_TOPIC_FILTER           otaexampleTOPIC_PREFIX "jobs/$next/get/accepted"

/**
 * @brief Length of job accepted response topic filter.
 */
#define otaexampleJOB_ACCEPTED_RESPONSE_TOPIC_FILTER_LENGTH    ( ( uint16_t ) ( sizeof( otaexampleJOB_ACCEPTED_RESPONSE_TOPIC_FILTER ) - 1 ) )


/**
 * @brief Wildcard topic filter for matching OTA data packets.
 *  The filter is used to match the constructed data stream topic filter from
 * OTA agent and register appropirate callback for it.
 */
#define otaexampleDATA_STREAM_TOPIC_FILTER           otaexampleTOPIC_PREFIX  "streams/#"

/**
 * @brief Length of data stream topic filter.
 */
#define otaexampleDATA_STREAM_TOPIC_FILTER_LENGTH    ( ( uint16_t ) ( sizeof( otaexampleDATA_STREAM_TOPIC_FILTER ) - 1 ) )


/**
 * @brief Default topic filter for OTA.
 * This is used to route all the packets for OTA reserved topics which OTA agent
 * has not subscribed for.
 */
#define otaexampleDEFAULT_TOPIC_FILTER              otaexampleTOPIC_PREFIX "jobs/#"

/**
 * @brief Length of default topic filter.
 */
#define otaexampleDEFAULT_TOPIC_FILTER_LENGTH       ( ( uint16_t ) ( sizeof( otaexampleDEFAULT_TOPIC_FILTER ) - 1 ) )

/**
 * @brief Stack size required for OTA agent task.
 */
#define otaexampleAGENT_TASK_STACK_SIZE             ( 4000U )

/**
 * @brief Priority required for OTA agent task.
 */
#define otaexampleAGENT_TASK_PRIORITY               ( tskIDLE_PRIORITY )

/**
 * @brief The number of ticks to wait for the OTA Agent to complete the shutdown.
 */
#define otaexampleOTA_SHUTDOWN_WAIT_TICKS           ( 0U )

/**
 * @brief Unsubscribe from the job topics when shutdown is called.
 */
#define otaexampleUNSUBSCRIBE_AFTER_OTA_SHUTDOWN    ( 1U )

/**
 * @brief ALPN (Application-Layer Protocol Negotiation) protocol name for AWS
 * IoT MQTT.
 *
 * This will be used if the AWS_MQTT_PORT is configured as 443 for AWS IoT MQTT
 * broker. Please see more details about the ALPN protocol for AWS IoT MQTT
 * endpoint in the link below.
 * https://aws.amazon.com/blogs/iot/mqtt-with-tls-client-authentication-on-port-443-why-it-is-useful-and-how-it-works/
 */
#define AWS_IOT_MQTT_ALPN                           "\x0ex-amzn-mqtt-ca"

/**
 * @brief Length of ALPN protocol name.
 */
#define AWS_IOT_MQTT_ALPN_LENGTH                    ( ( uint16_t ) ( sizeof( AWS_IOT_MQTT_ALPN ) - 1 ) )

/**
 * @brief Timeout for receiving CONNACK packet in milli seconds.
 */
#define CONNACK_RECV_TIMEOUT_MS                     ( 2000U )

/**
 * @brief The maximum time interval in seconds which is allowed to elapse
 * between two Control Packets.
 *
 * It is the responsibility of the Client to ensure that the interval between
 * Control Packets being sent does not exceed the this Keep Alive value. In the
 * absence of sending any other Control Packets, the Client MUST send a
 * PINGREQ Packet.
 */
#define MQTT_KEEP_ALIVE_INTERVAL_SECONDS            ( 60U )

/**
 * @brief Stack size required for MQTT agent task.
 * MQTT agent task takes care of TLS connection and reconnection, keeping task
 * stack size to high enough required for TLS connection.
 */
#define MQTT_AGENT_TASK_STACK_SIZE                  ( 4000U )

/**
 * @brief Priority required for OTA statistics task.
 */
#define MQTT_AGENT_TASK_PRIORITY                    ( tskIDLE_PRIORITY )

/**
 * @brief The maximum amount of time in milliseconds to wait for the commands
 * to be posted to the MQTT agent should the MQTT agent's command queue be full.
 * Tasks wait in the Blocked state, so don't use any CPU time.
 */
#define MQTT_AGENT_SEND_BLOCK_TIME_MS               ( 200U )

/**
 * @brief This demo uses task notifications to signal tasks from MQTT callback
 * functions.  mqttexampleMS_TO_WAIT_FOR_NOTIFICATION defines the time, in ticks,
 * to wait for such a callback.
 */
#define MQTT_AGENT_MS_TO_WAIT_FOR_NOTIFICATION      ( 5000U )

/**
 * @brief Milliseconds per second.
 */
#define MILLISECONDS_PER_SECOND                     ( 1000U )

/**
 * @brief Milliseconds per FreeRTOS tick.
 */
#define MILLISECONDS_PER_TICK                       ( MILLISECONDS_PER_SECOND / configTICK_RATE_HZ )

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

/**
 * @brief Structure used to store the topic filter to ota callback mappings.
 */
typedef struct OtaTopicFilterCallback
{
    const char * pcTopicFilter;
    uint16_t usTopicFilterLength;
    IncomingPubCallback_t xCallback;
} OtaTopicFilterCallback_t;

/**
 * @brief Defines the structure to use as the command callback context in this
 * demo.
 */
struct MQTTAgentCommandContext
{
    MQTTStatus_t xReturnStatus;
    TaskHandle_t xTaskToNotify;
    uint32_t ulNotificationValue;
    void * pArgs;
};

/**
 * @brief The MQTT agent context.
 * In case of sharing the mqtt connection with other demos using the MQTT agent,
 * this context should be declared non-static so that's it shared across all
 * demo files.
 */
static MQTTAgentContext_t xGlobalMqttAgentContext;

/**
 * @brief The buffer is used to hold the serialized packets for transmission to
 * and from the transport interface.
 */
static uint8_t pucNetworkBuffer[ MQTT_AGENT_NETWORK_BUFFER_SIZE ];

/**
 * @brief FreeRTOS blocking queue to be used as MQTT Agent context.
 */
static MQTTAgentMessageContext_t xCommandQueue;

/**
 * @brief The interface context used to post commands to the agent.
 * For FreeRTOS it's implemented using a FreeRTOS blocking queue.
 */
static MQTTAgentMessageInterface_t xMessageInterface;

/**
 * @brief The global array of subscription elements.
 *
 * @note The subscription manager implementation expects that the array of the
 * subscription elements used for storing subscriptions to be initialized to 0.
 * As this is a global array, it will be intialized to 0 by default.
 */
static SubscriptionElement_t pxGlobalSubscriptionList[ SUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS ];

/**
 * @brief Network connection context used in this demo for MQTT connection.
 */
static NetworkContext_t xNetworkContextMqtt;

/**
 * @brief Semaphore for synchronizing buffer operations.
 */
static SemaphoreHandle_t xBufferSemaphore;

/**
 * @brief Ble Transport Parameters structure to store the data channel.
 */
static BleTransportParams_t xBleTransportParams;

/**
 * @brief Semaphore used to synchronize with the data transfer channel callback.
 */
static SemaphoreHandle_t xChannelSemaphore;

/**
 * @brief Update File path buffer.
 */
static uint8_t pucUpdateFilePath[ otaexampleMAX_FILE_PATH_SIZE ];

/**
 * @brief Certificate File path buffer.
 */
static uint8_t pucCertFilePath[ otaexampleMAX_FILE_PATH_SIZE ];

/**
 * @brief Stream name buffer.
 */
static uint8_t pucStreamName[ otaexampleMAX_STREAM_NAME_SIZE ];

/**
 * @brief Decode memory.
 */
static uint8_t pucDecodeMem[ otaconfigFILE_BLOCK_SIZE ];

/**
 * @brief Bitmap memory.
 */
static uint8_t pucBitmap[ OTA_MAX_BLOCK_BITMAP_SIZE ];

/**
 * @brief Event buffer.
 */
static OtaEventData_t pxEventBuffer[ otaconfigMAX_NUM_OTA_DATA_BUFFERS ];

/**
 * @brief Buffer used by the transport interface to queue incoming packets for use.
 */
static uint8_t ucTransportBuffer[ MQTT_AGENT_NETWORK_BUFFER_SIZE ];

/**
 * @brief Global entry time into the application to use as a reference timestamp
 * in the #prvGetTimeMs function. #prvGetTimeMs will always return the difference
 * between the current time and the global entry time. This will reduce the
 * chances of overflow for the 32 bit unsigned integer used for holding the
 * timestamp.
 */
static uint32_t ulGlobalEntryTimeMs;

/**
 * @brief The buffer passed to the OTA Agent from application while initializing.
 */
static OtaAppBuffer_t xOtaBuffer =
{
    .pUpdateFilePath    = pucUpdateFilePath,
    .updateFilePathsize = otaexampleMAX_FILE_PATH_SIZE,
    .pCertFilePath      = pucCertFilePath,
    .certFilePathSize   = otaexampleMAX_FILE_PATH_SIZE,
    .pStreamName        = pucStreamName,
    .streamNameSize     = otaexampleMAX_STREAM_NAME_SIZE,
    .pDecodeMemory      = pucDecodeMem,
    .decodeMemorySize   = otaconfigFILE_BLOCK_SIZE,
    .pFileBitmap        = pucBitmap,
    .fileBitmapSize     = OTA_MAX_BLOCK_BITMAP_SIZE,
};

/*-----------------------------------------------------------*/

/**
 * @brief Callback for BLE data channel, handles events when the data channel is first opened
 * and when data is received by the channel.
 *
 * @param[in] xEvent The event type received from the data transfer channel.
 * @param[in] pxChannel Pointer to data transfer channel.
 * @param[in] pxContext Pointer to user context passed during channel initialization.
 */
static void prvBLEChannelCallback( IotBleDataTransferChannelEvent_t xEvent,
                                   IotBleDataTransferChannel_t * pxChannel,
                                   void * pvContext );

/**
 * @brief Initializes the tranport interface, including calling the init function for the transport layer
 * and initializing the BLE channel.
 *
 * @param[in] pxContext Pointer to transport interface context
 * @return pdTRUE if the connection was successful / pdFALSE otherwise.
 */
static BaseType_t prvCreateBLETransportInterfaceConnection( NetworkContext_t * pxContext );

/**
 * @brief Disconnect BLE transport interface.
 *
 * @param[in] pTransportCtxt Context passed to transport interface from MQTT library.
 * @return pdTRUE if the disconnect was successful / pdFALSE otherwise.
 */
static BaseType_t prvBLETransportInterfaceDisconnect( NetworkContext_t * pxContext );

/**
 * @brief Initializes an MQTT Agent including transport interface and
 * network buffer.
 *
 * @return `MQTTSuccess` if the initialization succeeds, else `MQTTBadParameter`
 */
static MQTTStatus_t prvMqttAgentInit( void );

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
 * @param[in] pcTopic Mqtt topic filter.
 *
 * @param[in] usTopicLen Length of the topic filter.
 *
 * @param[in] pcMsg Message to publish.
 *
 * @param[in] ulMsgSize Message size.
 *
 * @param[in] ucQOS Quality of Service
 *
 * @return OtaMqttSuccess if success , other error code on failure.
 */
static OtaMqttStatus_t prvMqttPublish( const char * const pcTopic,
                                       uint16_t usTopicLen,
                                       const char * pcMsg,
                                       uint32_t ulMsgSize,
                                       uint8_t ucQOS );

/**
 * @brief Subscribe to the Mqtt topics.
 *
 * This function subscribes to the Mqtt topics with the Quality of service
 * received as parameter. This function also registers a callback for the
 * topicfilter.
 *
 * @param[in] pcTopicFilter Mqtt topic filter.
 *
 * @param[in] usTopicFilterLength Length of the topic filter.
 *
 * @param[in] ucQOS Quality of Service
 *
 * @return OtaMqttSuccess if success , other error code on failure.
 */
static OtaMqttStatus_t prvMqttSubscribeOTACallback( const char * pcTopicFilter,
                                                    uint16_t usTopicFilterLength,
                                                    uint8_t ucQOS );

/**
 * @brief Unsubscribe to the Mqtt topics.
 *
 * This function unsubscribes to the Mqtt topics with the Quality of service
 * received as parameter.
 *
 * @param[in] pcTopicFilter Mqtt topic filter.
 *
 * @param[in] usTopicFilterLength Length of the topic filter.
 *
 * @param[in] ucQOS Quality of Service
 *
 * @return  OtaMqttSuccess if success , other error code on failure.
 */
static OtaMqttStatus_t prvMqttUnSubscribeOTACallback( const char * pcTopicFilter,
                                                      uint16_t usTopicFilterLength,
                                                      uint8_t ucQOS );

/**
 * @brief Attempt to connect to the MQTT broker.
 *
 * @return pdPASS if a connection is established.
 */
static BaseType_t prvConnectToMQTTBroker( void );

/**
 * @brief Disconnects from the MQTT broker.
 * Initiates an MQTT disconnect and then teardown underlying TCP connection.
 *
 */
static void prvDisconnectFromMQTTBroker( void );

/**
 * @brief Task for OTA agent.
 * Task runs OTA agent  loop which process OTA events. Task returns only when
 * OTA agent is shutdown by invoking OTA_Shutdown() API.
 *
 * @param[in] pParam Can be used to pass down functionality to the agent task
 */
static void prvOTAAgentTask( void * pParam );

/**
 * @brief Task for MQTT agent.
 * Task runs MQTT agent command loop, which returns only when the user
 * disconnects MQTT, terminates agent, or the mqtt connection is broken. If the
 * mqtt connection is broken, the task suspends OTA agent reconnects to the
 * broker and then resumes OTA agent.
 *
 * @param[in] pParam Can be used to pass down functionality to the agent task
 */
static void prvMQTTAgentTask( void * pParam );


/**
 * @brief Callback invoked by agent for a command process completion.
 *
 * @param[in] pxCommandContext User context passed by caller along with the
 * command.
 * @param[in] pxReturnInfo Info containing return code and output of command
 * from agent.
 */
static void prvMQTTAgentCmdCompleteCallback( MQTTAgentCommandContext_t * pxCommandContext,
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
 * @param[in]  pxOtaInterfaces pointer to OTA interface structure.
 *
 * @return   None.
 */
static void prvSetOtaInterfaces( OtaInterfaces_t * pxOtaInterfaces );


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
 * @param[in] xEvent Event from OTA lib of type OtaJobEvent_t.
 * @return None.
 */
static void prvOtaAppCallback( OtaJobEvent_t xEvent,
                               const void * pData );


/**
 * @brief Common callback registered with MQTT agent to receive all publish
 * packets. Packets received using the callback is distributed to subscribed
 * topics using subscription manager.
 *
 * @param[in] pxMqttAgentContext MQTT agent context for the connection.
 * @param[in] usPacketId Packet identifier for the packet.
 * @param[in] pPublishInfo MQTT packet information which stores details of the
 * job document.
 */
static void prvIncomingPublishCallback( MQTTAgentContext_t * pxMqttAgentContext,
                                        uint16_t usPacketId,
                                        MQTTPublishInfo_t * pxPublishInfo );

/**
 * @brief Register OTA callbacks with the subscription manager.
 *
 * @param[in] pcTopicFilter The topic filter for which a  callback needs to be
 * registered for.
 * @param[in] usTopicFilterLength length of the topic filter.
 *
 */
static void prvRegisterOTACallback( const char * pcTopicFilter,
                                    uint16_t usTopicFilterLength );


/**
 * @brief Callback registered with the OTA library that notifies the OTA agent
 * of an incoming PUBLISH containing a job document.
 *
 * @param[in] pContext MQTT context which stores the connection.
 * @param[in] pxPublishInfo MQTT packet information which stores details of the
 * job document.
 */
static void prvMqttJobCallback( void * pContext,
                                MQTTPublishInfo_t * pxPublishInfo );


/**
 * @brief Callback that notifies the OTA library when a data block is received.
 *
 * @param[in] pContext MQTT context which stores the connection.
 * @param[in] pxPublishInfo MQTT packet that stores the information of the file
 * block.
 */
static void prvMqttDataCallback( void * pContext,
                                 MQTTPublishInfo_t * pxPublishInfo );

/**
 * @brief Default callback used to receive unsolicited messages for OTA.
 *
 * The callback is not subscribed with MQTT broker, but only with local
 * subscription manager. A wildcard OTA job topic is used for subscription so
 * that all unsolicited messages related to OTA is forwarded to this callback for
 * filteration. Right now the callback is used to filter responses to job
 * requests from the OTA service.
 *
 * @param[in] pvIncomingPublishCallbackContext MQTT context which stores the
 * connection.
 * @param[in] pxPublishInfo MQTT packet that stores the information of the file
 * block.
 */
static void prvMqttDefaultCallback( void * pvIncomingPublishCallbackContext,
                                    MQTTPublishInfo_t * pxPublishInfo );

/*-----------------------------------------------------------*/

/**
 * @brief Registry for all  mqtt topic filters to their corresponding callbacks
 * for OTA.
 */
static OtaTopicFilterCallback_t xOtaTopicFilterCallbacks[] =
{
    {
        .pcTopicFilter = otaexampleJOB_NOTIFY_TOPIC_FILTER,
        .usTopicFilterLength = otaexampleJOB_NOTIFY_TOPIC_FILTER_LENGTH,
        .xCallback = prvMqttJobCallback
    },
    {
        .pcTopicFilter = otaexampleDATA_STREAM_TOPIC_FILTER,
        .usTopicFilterLength = otaexampleDATA_STREAM_TOPIC_FILTER_LENGTH,
        .xCallback = prvMqttDataCallback
    },
    {
        .pcTopicFilter = otaexampleDEFAULT_TOPIC_FILTER,
        .usTopicFilterLength = otaexampleDEFAULT_TOPIC_FILTER_LENGTH,
        .xCallback = prvMqttDefaultCallback
    }
};
/*-----------------------------------------------------------*/

static void prvOtaEventBufferFree( OtaEventData_t * const pxBuffer )
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

static OtaEventData_t * prvOtaEventBufferGet( void )
{
    uint32_t ulIndex = 0;
    OtaEventData_t * pxFreeBuffer = NULL;

    if( xSemaphoreTake( xBufferSemaphore, portMAX_DELAY ) == pdTRUE )
    {
        for( ulIndex = 0; ulIndex < otaconfigMAX_NUM_OTA_DATA_BUFFERS; ulIndex++ )
        {
            if( pxEventBuffer[ ulIndex ].bufferUsed == false )
            {
                pxEventBuffer[ ulIndex ].bufferUsed = true;
                pxFreeBuffer = &pxEventBuffer[ ulIndex ];
                break;
            }
        }

        ( void ) xSemaphoreGive( xBufferSemaphore );
    }
    else
    {
        LogError( ( "Failed to get buffer semaphore." ) );
    }

    return pxFreeBuffer;
}
/*-----------------------------------------------------------*/

static void prvOtaAppCallback( OtaJobEvent_t xEvent,
                               const void * pData )
{
    OtaErr_t xOtaError = OtaErrUninitialized;

    switch( xEvent )
    {
        case OtaJobEventActivate:
            LogInfo( ( "Received OtaJobEventActivate callback from OTA Agent." ) );

            /* Activate the new firmware image. */
            OTA_ActivateNewImage();

            /* Initiate Shutdown of OTA Agent.
             * If it is required that the unsubscribe operations are not
             * performed while shutting down please set the second parameter to
             * 0 instead of 1.
             */
            OTA_Shutdown( otaexampleOTA_SHUTDOWN_WAIT_TICKS, otaexampleUNSUBSCRIBE_AFTER_OTA_SHUTDOWN );

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
            xOtaError = OTA_SetImageState( OtaImageStateAccepted );

            if( xOtaError != OtaErrNone )
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
                prvOtaEventBufferFree( ( OtaEventData_t * ) pData );
            }

            break;

        case OtaJobEventSelfTestFailed:
            LogDebug( ( "Received OtaJobEventSelfTestFailed callback from OTA Agent." ) );

            /* Requires manual activation of previous image as self-test for
             * new image downloaded failed.*/
            LogError( ( "Self-test of new image failed, shutting down OTA Agent." ) );

            /* Initiate Shutdown of OTA Agent.
             * If it is required that the unsubscribe operations are not
             * performed while shutting down please set the second parameter to 0 instead of 1
             */
            OTA_Shutdown( otaexampleOTA_SHUTDOWN_WAIT_TICKS, otaexampleUNSUBSCRIBE_AFTER_OTA_SHUTDOWN );

            break;

        default:
            LogDebug( ( "Received invalid callback event from OTA Agent." ) );
    }
}
/*-----------------------------------------------------------*/

static void prvIncomingPublishCallback( MQTTAgentContext_t * pxMqttAgentContext,
                                        uint16_t usPacketId,
                                        MQTTPublishInfo_t * pxPublishInfo )
{
    bool xPublishHandled = false;
    char cOriginalChar, * pcLocation;

    ( void ) usPacketId;

    /* Fan out the incoming publishes to the callbacks registered using
     * subscription manager. */
    xPublishHandled = SubscriptionManager_HandleIncomingPublishes( ( SubscriptionElement_t * ) pxMqttAgentContext->pIncomingCallbackContext,
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
    OtaEventData_t * pxEventData;
    OtaEventMsg_t pxEventMsg = { 0 };

    configASSERT( pxPublishInfo != NULL );
    ( void ) pvIncomingPublishCallbackContext;

    LogInfo( ( "Received job message callback, size %ld.\n\n", pxPublishInfo->payloadLength ) );

    pxEventData = prvOtaEventBufferGet();

    if( pxEventData != NULL )
    {
        memcpy( pxEventData->data, pxPublishInfo->pPayload, pxPublishInfo->payloadLength );
        pxEventData->dataLength = pxPublishInfo->payloadLength;
        pxEventMsg.eventId = OtaAgentEventReceivedJobDocument;
        pxEventMsg.pEventData = pxEventData;

        /* Send job document received event. */
        OTA_SignalEvent( &pxEventMsg );
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
    bool xIsMatch = false;

    ( void ) MQTT_MatchTopic( pxPublishInfo->pTopicName,
                              pxPublishInfo->topicNameLength,
                              otaexampleJOB_ACCEPTED_RESPONSE_TOPIC_FILTER,
                              otaexampleJOB_ACCEPTED_RESPONSE_TOPIC_FILTER_LENGTH,
                              &xIsMatch );

    if( xIsMatch == true )
    {
        prvMqttJobCallback( pvIncomingPublishCallbackContext, pxPublishInfo );
    }
}

/*-----------------------------------------------------------*/

static void prvMqttDataCallback( void * pvIncomingPublishCallbackContext,
                                 MQTTPublishInfo_t * pxPublishInfo )
{
    OtaEventData_t * pxEventData;
    OtaEventMsg_t pxEventMsg = { 0 };

    configASSERT( pxPublishInfo != NULL );
    ( void ) pvIncomingPublishCallbackContext;

    LogInfo( ( "Received data message callback, size %zu.\n\n", pxPublishInfo->payloadLength ) );

    pxEventData = prvOtaEventBufferGet();

    if( pxEventData != NULL )
    {
        memcpy( pxEventData->data, pxPublishInfo->pPayload, pxPublishInfo->payloadLength );
        pxEventData->dataLength = pxPublishInfo->payloadLength;
        pxEventMsg.eventId = OtaAgentEventReceivedFileBlock;
        pxEventMsg.pEventData = pxEventData;

        /* Send job document received event. */
        OTA_SignalEvent( &pxEventMsg );
    }
    else
    {
        LogError( ( "Error: No OTA data buffers available.\r\n" ) );
    }
}

/*-----------------------------------------------------------*/

static void prvMQTTAgentCmdCompleteCallback( MQTTAgentCommandContext_t * pxCommandContext,
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

static void prvRegisterOTACallback( const char * pcTopicFilter,
                                    uint16_t usTopicFilterLength )
{
    bool xIsMatch = false;
    bool xSubscriptionAdded;
    MQTTStatus_t xMqttStatus = MQTTSuccess;
    uint16_t usIndex = 0U;
    uint16_t ulNumTopicFilters = sizeof( xOtaTopicFilterCallbacks ) / sizeof( OtaTopicFilterCallback_t );

    /* Match the input topic filter against the wild-card pattern of topics filters
    * relevant for the OTA Update service to determine the type of topic filter. */
    for( usIndex = 0U; usIndex < ulNumTopicFilters; usIndex++ )
    {
        xMqttStatus = MQTT_MatchTopic( pcTopicFilter,
                                       usTopicFilterLength,
                                       xOtaTopicFilterCallbacks[ usIndex ].pcTopicFilter,
                                       xOtaTopicFilterCallbacks[ usIndex ].usTopicFilterLength,
                                       &xIsMatch );
        assert( xMqttStatus == MQTTSuccess );

        if( xIsMatch )
        {
            /* Add subscription so that incoming publishes are routed to the
             * application callback. */
            xSubscriptionAdded = SubscriptionManager_AddSubscription( ( SubscriptionElement_t * ) xGlobalMqttAgentContext.pIncomingCallbackContext,
                                                                      pcTopicFilter,
                                                                      usTopicFilterLength,
                                                                      xOtaTopicFilterCallbacks[ usIndex ].xCallback,
                                                                      NULL );

            if( xSubscriptionAdded == false )
            {
                LogError( ( "Failed to register a publish callback for topic %.*s.",
                            pcTopicFilter,
                            usTopicFilterLength ) );
            }
        }
    }
}
/*-----------------------------------------------------------*/

static void prvMQTTSubscribeCompleteCallback( MQTTAgentCommandContext_t * pxCommandContext,
                                              MQTTAgentReturnInfo_t * pxReturnInfo )
{
    MQTTAgentSubscribeArgs_t * pxSubscribeArgs;

    if( pxReturnInfo->returnCode == MQTTSuccess )
    {
        pxSubscribeArgs = ( MQTTAgentSubscribeArgs_t * ) ( pxCommandContext->pArgs );
        prvRegisterOTACallback( pxSubscribeArgs->pSubscribeInfo->pTopicFilter, pxSubscribeArgs->pSubscribeInfo->topicFilterLength );
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

static void prvMQTTUnsubscribeCompleteCallback( MQTTAgentCommandContext_t * pxCommandContext,
                                                MQTTAgentReturnInfo_t * pxReturnInfo )
{
    MQTTAgentSubscribeArgs_t * pxSubscribeArgs;

    if( pxReturnInfo->returnCode == MQTTSuccess )
    {
        pxSubscribeArgs = ( MQTTAgentSubscribeArgs_t * ) ( pxCommandContext->pArgs );

        /* Add subscription so that incoming publishes are routed to the
         * application callback. */
        SubscriptionManager_RemoveSubscription( ( SubscriptionElement_t * ) xGlobalMqttAgentContext.pIncomingCallbackContext,
                                                pxSubscribeArgs->pSubscribeInfo->pTopicFilter,
                                                pxSubscribeArgs->pSubscribeInfo->topicFilterLength );

        LogInfo( ( "Removed registration for topic %.*s.",
                   pxSubscribeArgs->pSubscribeInfo->topicFilterLength,
                   pxSubscribeArgs->pSubscribeInfo->pTopicFilter ) );
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

static MQTTStatus_t prvMqttAgentInit( void )
{
    TransportInterface_t xTransport;
    MQTTStatus_t xReturn;
    MQTTFixedBuffer_t xFixedBuffer = { .pBuffer = pucNetworkBuffer, .size = MQTT_AGENT_NETWORK_BUFFER_SIZE };
    static uint8_t ucStaticQueueStorageArea[ MQTT_AGENT_COMMAND_QUEUE_LENGTH * sizeof( MQTTAgentCommand_t * ) ];
    static StaticQueue_t xStaticQueueStructure;

    LogDebug( ( "Creating command queue." ) );
    xCommandQueue.queue = xQueueCreateStatic( MQTT_AGENT_COMMAND_QUEUE_LENGTH,
                                              sizeof( MQTTAgentCommand_t * ),
                                              ucStaticQueueStorageArea,
                                              &xStaticQueueStructure );

    /* Initialize the agent task pool. */
    Agent_InitializePool();

    xMessageInterface.pMsgCtx = &xCommandQueue;
    xMessageInterface.recv = Agent_MessageReceive;
    xMessageInterface.send = Agent_MessageSend;
    xMessageInterface.getCommand = Agent_GetCommand;
    xMessageInterface.releaseCommand = Agent_ReleaseCommand;


    /* Fill in Transport Interface send and receive function pointers. */
    xTransport.pNetworkContext = &xNetworkContextMqtt;
    xTransport.send = IotBleMqttTransportSend;
    xTransport.recv = IotBleMqttTransportReceive;

    /* Initialize MQTT Agent. */
    xReturn = MQTTAgent_Init( &xGlobalMqttAgentContext,
                              &xMessageInterface,
                              &xFixedBuffer,
                              &xTransport,
                              prvGetTimeMs,
                              prvIncomingPublishCallback,
                              /* Context to pass into the callback. Passing the pointer to subscription array. */
                              pxGlobalSubscriptionList );

    return xReturn;
}

/*-----------------------------------------------------------*/

static MQTTStatus_t prvMQTTConnect( void )
{
    MQTTStatus_t xMqttStatus = MQTTBadParameter;
    MQTTConnectInfo_t xConnectInfo = { 0 };

    bool xSessionPresent = false;

    /* Establish MQTT session by sending a CONNECT packet. */

    /* If #createCleanSession is true, start with a clean session
     * i.e. direct the MQTT broker to discard any previous session data.
     * If #createCleanSession is false, directs the broker to attempt to
     * reestablish a session which was already present. */
    xConnectInfo.cleanSession = true;

    /* The client identifier is used to uniquely identify this MQTT client to
     * the MQTT broker. In a production device the identifier can be something
     * unique, such as a device serial number. */
    xConnectInfo.pClientIdentifier = democonfigCLIENT_IDENTIFIER;
    xConnectInfo.clientIdentifierLength = ( uint16_t ) strlen( democonfigCLIENT_IDENTIFIER );

    /* The maximum time interval in seconds which is allowed to elapse
     * between two Control Packets.
     * It is the responsibility of the Client to ensure that the interval between
     * Control Packets being sent does not exceed the this Keep Alive value. In the
     * absence of sending any other Control Packets, the Client MUST send a
     * PINGREQ Packet. */
    xConnectInfo.keepAliveSeconds = MQTT_KEEP_ALIVE_INTERVAL_SECONDS;

    /* Send MQTT CONNECT packet to broker. */
    xMqttStatus = MQTT_Connect( &xGlobalMqttAgentContext.mqttContext, &xConnectInfo, NULL, CONNACK_RECV_TIMEOUT_MS, &xSessionPresent );

    return xMqttStatus;
}

/*-----------------------------------------------------------*/

static void prvBLEChannelCallback( IotBleDataTransferChannelEvent_t xEvent,
                                   IotBleDataTransferChannel_t * pxChannel,
                                   void * pvContext )
{
    /* Unused parameters. */
    ( void ) pxChannel;
    ( void ) pvContext;

    /* Event to see when the data channel is ready to receive data. */
    if( xEvent == IOT_BLE_DATA_TRANSFER_CHANNEL_OPENED )
    {
        LogInfo( ( "BLE Channel is Open." ) );
        ( void ) xSemaphoreGive( xChannelSemaphore );
    }

    /* Event for when data is received over the channel. */
    else if( xEvent == IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVED )
    {
        ( void ) IotBleMqttTransportAcceptData( &xNetworkContextMqtt );
    }

    /* Event for when channel is closed. */
    else if( xEvent == IOT_BLE_DATA_TRANSFER_CHANNEL_CLOSED )
    {
        LogInfo( ( "BLE Channel is closed." ) );
    }

    else
    {
        /* Empty else, MISRA 2012 15.7 */
    }
}

/*--------------------------------------------------------------*/

static BaseType_t prvCreateBLETransportInterfaceConnection( NetworkContext_t * pxTransportContext )
{
    BaseType_t xStatus = pdFALSE;

    pxTransportContext->pParams->pChannel = IotBleDataTransfer_Open( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT );

    if( pxTransportContext->pParams->pChannel != NULL )
    {
        xChannelSemaphore = xSemaphoreCreateBinary();

        if( xChannelSemaphore != NULL )
        {
            ( void ) IotBleDataTransfer_SetCallback( pxTransportContext->pParams->pChannel, prvBLEChannelCallback, NULL );

            /* Open is a handshake procedure where a smartphone application sends back an acknowledgement to indicate that channel
             * is ready to use. An channel opened event is sent on a successful handshake. The demo code waits here for an
             * opened event from channel.
             */
            if( xSemaphoreTake( xChannelSemaphore, pdMS_TO_TICKS( IOT_BLE_MQTT_CREATE_CONNECTION_WAIT_MS ) ) == pdTRUE )
            {
                LogInfo( ( "The channel was initialized successfully" ) );
                xStatus = pdTRUE;
            }
            else
            {
                LogError( ( "The BLE Connection timed out" ) );
                xStatus = pdFALSE;
            }
        }
        else
        {
            LogError( ( "Could not create BLE channel semaphore." ) );
            xStatus = pdFALSE;
        }
    }

    if( xStatus == pdTRUE )
    {
        if( IotBleMqttTransportInit( ucTransportBuffer, MQTT_AGENT_NETWORK_BUFFER_SIZE, pxTransportContext ) != true )
        {
            LogError( ( "Failed to create BLE transport interface." ) );
            xStatus = pdFALSE;
        }
    }

    if( xStatus == pdTRUE )
    {
        /* Set receive timeout to zero to make transport interface non blocking for MQTT agent. */
        IotBleMqttTransportSetReceiveTimeout( pxTransportContext, 0U );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/


static BaseType_t prvConnectToMQTTBroker( void )
{
    BaseType_t xStatus = pdFAIL;

    xNetworkContextMqtt.pParams = &xBleTransportParams;

    /* Attempt to create connection to broker over BLE transport interface. */
    xStatus = prvCreateBLETransportInterfaceConnection( &xNetworkContextMqtt );

    if( xStatus != pdPASS )
    {
        /* Log error to indicate connection failure. */
        LogError( ( "Failed to connect to MQTT broker %s.",
                    democonfigMQTT_BROKER_ENDPOINT ) );
    }

    if( xStatus == pdPASS )
    {
        if( prvMqttAgentInit() != MQTTSuccess )
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
/*-----------------------------------------------------------*/

static BaseType_t prvBLETransportInterfaceDisconnect( NetworkContext_t * pxContext )
{
    ( void ) IotBleMqttTransportCleanup( pxContext );
    ( void ) IotBleDataTransfer_Close( pxContext->pParams->pChannel );
    ( void ) IotBleDataTransfer_Reset( pxContext->pParams->pChannel );
    ( void ) vSemaphoreDelete( xChannelSemaphore );

    return pdTRUE;
}

/*-----------------------------------------------------------*/

static void prvDisconnectFromMQTTBroker( void )
{
    MQTTAgentCommandContext_t xCommandContext = { 0 };
    MQTTAgentCommandInfo_t xCommandParams = { 0 };
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

    /* End BLE transport interface connection. */
    ( void ) prvBLETransportInterfaceDisconnect( &xNetworkContextMqtt );
}
/*-----------------------------------------------------------*/

static OtaMqttStatus_t prvMqttSubscribe( const char * pcTopicFilter,
                                         uint16_t usTopicFilterLength,
                                         uint8_t ucQOS )
{
    OtaMqttStatus_t xOtaMqttStatus = OtaMqttSuccess;
    MQTTStatus_t xCommandStatus;
    MQTTAgentCommandInfo_t xCommandParams = { 0 };
    MQTTAgentCommandContext_t xCommandContext = { 0 };
    MQTTSubscribeInfo_t xSubscription = { 0 };
    MQTTAgentSubscribeArgs_t xSubscribeArgs = { 0 };


    assert( pcTopicFilter != NULL );
    assert( usTopicFilterLength > 0 );

    xSubscription.qos = ucQOS;
    xSubscription.pTopicFilter = pcTopicFilter;
    xSubscription.topicFilterLength = usTopicFilterLength;
    xSubscribeArgs.numSubscriptions = 1;
    xSubscribeArgs.pSubscribeInfo = &xSubscription;


    xCommandParams.blockTimeMs = MQTT_AGENT_SEND_BLOCK_TIME_MS;
    xCommandParams.cmdCompleteCallback = prvMQTTSubscribeCompleteCallback;
    xCommandParams.pCmdCompleteCallbackContext = &xCommandContext;
    xCommandContext.xTaskToNotify = xTaskGetCurrentTaskHandle();
    xCommandContext.pArgs = &xSubscribeArgs;
    xCommandContext.xReturnStatus = MQTTSendFailed;

    /* Disconnect MQTT session. */
    xCommandStatus = MQTTAgent_Subscribe( &xGlobalMqttAgentContext, &xSubscribeArgs, &xCommandParams );
    configASSERT( xCommandStatus == MQTTSuccess );

    xTaskNotifyWait( 0,
                     0,
                     NULL,
                     pdMS_TO_TICKS( MQTT_AGENT_MS_TO_WAIT_FOR_NOTIFICATION ) );

    if( xCommandContext.xReturnStatus != MQTTSuccess )
    {
        LogError( ( "Failed to send SUBSCRIBE packet to broker with error = %u.", xCommandContext.xReturnStatus ) );
        xOtaMqttStatus = OtaMqttSubscribeFailed;
    }
    else
    {
        LogInfo( ( "SUBSCRIBED to topic %.*s to broker.\n\n",
                   usTopicFilterLength,
                   pcTopicFilter ) );
    }

    return xOtaMqttStatus;
}


/*-----------------------------------------------------------*/

static OtaMqttStatus_t prvSubscribeToJobResponseTopic( void )
{
    OtaMqttStatus_t xStatus = OtaMqttSubscribeFailed;
    char * pcTopicFilter = NULL;
    const char * pcClientIdentifier = democonfigCLIENT_IDENTIFIER;
    size_t xTopicFilterLength;

    configASSERT( pcClientIdentifier != NULL );

    xTopicFilterLength = otaexampleJOB_ACCEPTED_RESPONSE_TOPIC_FILTER_LENGTH + strlen( pcClientIdentifier );

    pcTopicFilter = pvPortMalloc( xTopicFilterLength );

    if( pcTopicFilter != NULL )
    {
        xTopicFilterLength = snprintf( pcTopicFilter,
                                       xTopicFilterLength,
                                       "$aws/things/%.*s/jobs/$next/get/accepted",
                                       strlen( pcClientIdentifier ),
                                       pcClientIdentifier );
        xStatus = prvMqttSubscribe( pcTopicFilter, xTopicFilterLength, 1U );

        vPortFree( pcTopicFilter );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static OtaMqttStatus_t prvMqttSubscribeOTACallback( const char * pcTopicFilter,
                                                    uint16_t usTopicFilterLength,
                                                    uint8_t ucQOS )
{
    OtaMqttStatus_t xStatus;

    xStatus = prvMqttSubscribe( pcTopicFilter, usTopicFilterLength, ucQOS );

    if( xStatus == OtaMqttSuccess )
    {
        xStatus = prvSubscribeToJobResponseTopic();
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static OtaMqttStatus_t prvMqttPublish( const char * const pcTopic,
                                       uint16_t usTopicLen,
                                       const char * pcMsg,
                                       uint32_t ulMsgSize,
                                       uint8_t ucQOS )
{
    OtaMqttStatus_t xOtaMqttStatus = OtaMqttSuccess;
    MQTTStatus_t xCommandStatus;
    MQTTAgentCommandInfo_t xCommandParams = { 0 };
    MQTTAgentCommandContext_t xCommandContext = { 0 };
    MQTTPublishInfo_t xPublishInfo = { 0 };

    /* Set the required publish parameters. */
    xPublishInfo.pTopicName = pcTopic;
    xPublishInfo.topicNameLength = usTopicLen;
    xPublishInfo.qos = ucQOS;
    xPublishInfo.pPayload = pcMsg;
    xPublishInfo.payloadLength = ulMsgSize;

    xCommandParams.blockTimeMs = MQTT_AGENT_SEND_BLOCK_TIME_MS;
    xCommandParams.cmdCompleteCallback = prvMQTTAgentCmdCompleteCallback;
    xCommandParams.pCmdCompleteCallbackContext = &xCommandContext;
    xCommandContext.xTaskToNotify = xTaskGetCurrentTaskHandle();
    xCommandContext.pArgs = NULL;
    xCommandContext.xReturnStatus = MQTTSendFailed;

    xCommandStatus = MQTTAgent_Publish( &xGlobalMqttAgentContext, &xPublishInfo, &xCommandParams );
    configASSERT( xCommandStatus == MQTTSuccess );

    xTaskNotifyWait( 0,
                     0,
                     NULL,
                     pdMS_TO_TICKS( MQTT_AGENT_MS_TO_WAIT_FOR_NOTIFICATION ) );

    if( xCommandContext.xReturnStatus != MQTTSuccess )
    {
        LogError( ( "Failed to send PUBLISH packet to broker with error = %u.", xCommandContext.xReturnStatus ) );
        xOtaMqttStatus = OtaMqttPublishFailed;
    }
    else
    {
        LogInfo( ( "Sent PUBLISH packet to broker %.*s to broker.\n\n",
                   usTopicLen,
                   pcTopic ) );
    }

    return xOtaMqttStatus;
}
/*-----------------------------------------------------------*/

static OtaMqttStatus_t prvMqttUnSubscribe( const char * pcTopicFilter,
                                           uint16_t usTopicFilterLength,
                                           uint8_t ucQOS )
{
    OtaMqttStatus_t xOtaMqttStatus = OtaMqttSuccess;
    MQTTStatus_t xCommandStatus;
    MQTTAgentCommandInfo_t xCommandParams = { 0 };
    MQTTAgentCommandContext_t xCommandContext = { 0 };
    MQTTSubscribeInfo_t xSubscription = { 0 };
    MQTTAgentSubscribeArgs_t xSubscribeArgs = { 0 };

    xSubscription.qos = ucQOS;
    xSubscription.pTopicFilter = pcTopicFilter;
    xSubscription.topicFilterLength = usTopicFilterLength;
    xSubscribeArgs.numSubscriptions = 1;
    xSubscribeArgs.pSubscribeInfo = &xSubscription;


    xCommandParams.blockTimeMs = MQTT_AGENT_SEND_BLOCK_TIME_MS;
    xCommandParams.cmdCompleteCallback = prvMQTTUnsubscribeCompleteCallback;
    xCommandParams.pCmdCompleteCallbackContext = &xCommandContext;
    xCommandContext.xTaskToNotify = xTaskGetCurrentTaskHandle();
    xCommandContext.pArgs = &xSubscribeArgs;
    xCommandContext.xReturnStatus = MQTTSendFailed;

    /* Unsubscribe to topics. */
    xCommandStatus = MQTTAgent_Unsubscribe( &xGlobalMqttAgentContext, &xSubscribeArgs, &xCommandParams );
    configASSERT( xCommandStatus == MQTTSuccess );

    xTaskNotifyWait( 0,
                     0,
                     NULL,
                     pdMS_TO_TICKS( MQTT_AGENT_MS_TO_WAIT_FOR_NOTIFICATION ) );

    if( xCommandContext.xReturnStatus != MQTTSuccess )
    {
        LogError( ( "Failed to send unsubscribe packet to broker with error = %u.", xCommandContext.xReturnStatus ) );
        xOtaMqttStatus = OtaMqttSubscribeFailed;
    }
    else
    {
        LogInfo( ( "Unsubscribed from topic %.*s to broker.\n\n",
                   usTopicFilterLength,
                   pcTopicFilter ) );
    }

    return xOtaMqttStatus;
}
/*-----------------------------------------------------------*/



static OtaMqttStatus_t prvUnSubscribeFromJobResponseTopic( void )
{
    OtaMqttStatus_t xStatus = OtaMqttSubscribeFailed;
    char * pcTopicFilter = NULL;
    const char * pcClientIdentifier = democonfigCLIENT_IDENTIFIER;
    size_t xTopicFilterLength;

    configASSERT( pcClientIdentifier != NULL );

    xTopicFilterLength = otaexampleJOB_ACCEPTED_RESPONSE_TOPIC_FILTER_LENGTH + strlen( pcClientIdentifier );

    pcTopicFilter = pvPortMalloc( xTopicFilterLength );

    if( pcTopicFilter != NULL )
    {
        xTopicFilterLength = snprintf( pcTopicFilter,
                                       xTopicFilterLength,
                                       "$aws/things/%.*s/jobs/$next/get/accepted",
                                       strlen( pcClientIdentifier ),
                                       pcClientIdentifier );
        xStatus = prvMqttUnSubscribe( pcTopicFilter, xTopicFilterLength, 1U );

        vPortFree( pcTopicFilter );
    }

    return xStatus;
}


/*-----------------------------------------------------------*/


static OtaMqttStatus_t prvMqttUnSubscribeOTACallback( const char * pcTopicFilter,
                                                      uint16_t usTopicFilterLength,
                                                      uint8_t ucQOS )
{
    OtaMqttStatus_t xStatus;

    xStatus = prvMqttUnSubscribe( pcTopicFilter, usTopicFilterLength, ucQOS );

    if( xStatus == OtaMqttSuccess )
    {
        xStatus = prvUnSubscribeFromJobResponseTopic();
    }

    return xStatus;
}

/*-----------------------------------------------------------*/


static void prvSetOtaInterfaces( OtaInterfaces_t * pxOtaInterfaces )
{
    /* Initialize OTA library OS Interface. */
    pxOtaInterfaces->os.event.init = OtaInitEvent_FreeRTOS;
    pxOtaInterfaces->os.event.send = OtaSendEvent_FreeRTOS;
    pxOtaInterfaces->os.event.recv = OtaReceiveEvent_FreeRTOS;
    pxOtaInterfaces->os.event.deinit = OtaDeinitEvent_FreeRTOS;
    pxOtaInterfaces->os.timer.start = OtaStartTimer_FreeRTOS;
    pxOtaInterfaces->os.timer.stop = OtaStopTimer_FreeRTOS;
    pxOtaInterfaces->os.timer.delete = OtaDeleteTimer_FreeRTOS;
    pxOtaInterfaces->os.mem.malloc = Malloc_FreeRTOS;
    pxOtaInterfaces->os.mem.free = Free_FreeRTOS;

    /* Initialize the OTA library MQTT Interface.*/
    pxOtaInterfaces->mqtt.subscribe = prvMqttSubscribeOTACallback;
    pxOtaInterfaces->mqtt.publish = prvMqttPublish;
    pxOtaInterfaces->mqtt.unsubscribe = prvMqttUnSubscribeOTACallback;

    /* Initialize the OTA library PAL Interface.*/
    pxOtaInterfaces->pal.getPlatformImageState = otaPal_GetPlatformImageState;
    pxOtaInterfaces->pal.setPlatformImageState = otaPal_SetPlatformImageState;
    pxOtaInterfaces->pal.writeBlock = otaPal_WriteBlock;
    pxOtaInterfaces->pal.activate = otaPal_ActivateNewImage;
    pxOtaInterfaces->pal.closeFile = otaPal_CloseFile;
    pxOtaInterfaces->pal.reset = otaPal_ResetDevice;
    pxOtaInterfaces->pal.abort = otaPal_Abort;
    pxOtaInterfaces->pal.createFile = otaPal_CreateFileForRx;
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
         * which could be a disconnect.  If an error occurs the MQTT error code
         * is returned and the queue left uncleared so there can be an attempt to
         * clean up and reconnect however the application writer prefers. */
        xMQTTStatus = MQTTAgent_CommandLoop( &xGlobalMqttAgentContext );

        /* Clear Agent queue so that any pending MQTT operations are not processed. */
        MQTTAgent_CancelAll( &xGlobalMqttAgentContext );

        /* Success is returned for application intiated disconnect or termination.
         * The socket will also be disconnected by the caller. */
        if( xMQTTStatus != MQTTSuccess )
        {
            xResult = prvSuspendOTA();
            configASSERT( xResult == pdPASS );

            LogInfo( ( "Suspended OTA agent." ) );

            /* Reconnect TCP. */
            ( void ) prvBLETransportInterfaceDisconnect( &xNetworkContextMqtt );

            xResult = prvConnectToMQTTBroker();
            configASSERT( xResult == pdPASS );

            xResult = prvResumeOTA();
            configASSERT( xResult == pdPASS );

            LogInfo( ( "Resumed OTA agent." ) );
        }
    } while( xMQTTStatus != MQTTSuccess );

    vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/

static BaseType_t prvSuspendOTA( void )
{
    /* OTA library return status. */
    OtaErr_t xOtaError = OtaErrUninitialized;
    BaseType_t xStatus = pdFAIL;
    uint32_t ulSuspendTimeout;

    xOtaError = OTA_Suspend();

    if( xOtaError == OtaErrNone )
    {
        ulSuspendTimeout = otaexampleSUSPEND_TIMEOUT_MS;

        while( ( OTA_GetState() != OtaAgentStateSuspended ) && ( ulSuspendTimeout > 0 ) )
        {
            /* Wait for OTA Library state to suspend */
            vTaskDelay( pdMS_TO_TICKS( otaexampleEXAMPLE_TASK_DELAY_MS ) );
            ulSuspendTimeout -= otaexampleEXAMPLE_TASK_DELAY_MS;
        }

        if( OTA_GetState() != OtaAgentStateSuspended )
        {
            LogError( ( "Failed to suspend OTA." ) );
        }
        else
        {
            xStatus = pdPASS;
        }
    }
    else
    {
        LogError( ( "Error while trying to suspend OTA agent %d", xOtaError ) );
    }

    return xStatus;
}
/*-----------------------------------------------------------*/

static BaseType_t prvResumeOTA( void )
{
    /* OTA library return status. */
    OtaErr_t xOtaError = OtaErrUninitialized;
    BaseType_t xStatus = pdFAIL;
    uint32_t ulSuspendTimeout;

    xOtaError = OTA_Resume();

    if( xOtaError == OtaErrNone )
    {
        ulSuspendTimeout = otaexampleSUSPEND_TIMEOUT_MS;

        while( ( OTA_GetState() == OtaAgentStateSuspended ) && ( ulSuspendTimeout > 0 ) )
        {
            /* Wait for OTA Library state to suspend */
            vTaskDelay( pdMS_TO_TICKS( otaexampleEXAMPLE_TASK_DELAY_MS ) );
            ulSuspendTimeout -= otaexampleEXAMPLE_TASK_DELAY_MS;
        }

        if( OTA_GetState() == OtaAgentStateSuspended )
        {
            LogError( ( "Failed to resume OTA." ) );
        }
        else
        {
            xStatus = pdPASS;
        }
    }
    else
    {
        LogError( ( "Error while trying to resume OTA agent %d", xOtaError ) );
    }

    return xStatus;
}
/*-----------------------------------------------------------*/

static BaseType_t prvRunOTADemo( void )
{
    /* Status indicating a successful demo or not. */
    BaseType_t xStatus = pdFAIL;

    /* OTA library return status. */
    OtaErr_t xOtaError = OtaErrUninitialized;

    /* OTA event message used for sending event to OTA Agent.*/
    OtaEventMsg_t xEventMsg = { 0 };

    /* OTA interface context required for library interface functions.*/
    OtaInterfaces_t xOtaInterfaces;

    /* OTA library packet statistics per job.*/
    OtaAgentStatistics_t xOtaStatistics = { 0 };

    /* OTA Agent state returned from calling OTA_GetState.*/
    OtaState_t xOtaState = OtaAgentStateStopped;

    /* Set OTA Library interfaces.*/
    prvSetOtaInterfaces( &xOtaInterfaces );


    /*************************** Init OTA Library. ***************************/

    if( ( xOtaError = OTA_Init( &xOtaBuffer,
                                &xOtaInterfaces,
                                ( const uint8_t * ) ( democonfigCLIENT_IDENTIFIER ),
                                prvOtaAppCallback ) ) != OtaErrNone )
    {
        LogError( ( "Failed to initialize OTA Agent, exiting = %u.",
                    xOtaError ) );
    }
    else
    {
        xStatus = pdPASS;
    }

    /************************ Create OTA Agent Task. ************************/

    if( xStatus == pdPASS )
    {
        xStatus = xTaskCreate( prvOTAAgentTask,
                               "OTA Agent Task",
                               otaexampleAGENT_TASK_STACK_SIZE,
                               NULL,
                               otaexampleAGENT_TASK_PRIORITY,
                               NULL );

        if( xStatus != pdPASS )
        {
            LogError( ( "Failed to create OTA agent task:" ) );
        }
    }

    /****************************** Start OTA ******************************/

    if( xStatus == pdPASS )
    {
        /* Send start event to OTA Agent.*/
        xEventMsg.eventId = OtaAgentEventStart;
        OTA_SignalEvent( &xEventMsg );
    }

    /******************** Loop and display OTA statistics ********************/

    if( xStatus == pdPASS )
    {
        while( ( xOtaState = OTA_GetState() ) != OtaAgentStateStopped )
        {
            /* Get OTA statistics for currently executing job. */
            if( xOtaState != OtaAgentStateSuspended )
            {
                OTA_GetStatistics( &xOtaStatistics );

                LogInfo( ( " Received: %u   Queued: %u   Processed: %u   Dropped: %u",
                           xOtaStatistics.otaPacketsReceived,
                           xOtaStatistics.otaPacketsQueued,
                           xOtaStatistics.otaPacketsProcessed,
                           xOtaStatistics.otaPacketsDropped ) );
            }

            vTaskDelay( pdMS_TO_TICKS( otaexampleEXAMPLE_TASK_DELAY_MS ) );
        }
    }

    return xStatus;
}
/*-----------------------------------------------------------*/

/**
 * @brief Entry point of demo.
 *
 * This examples demonstrates performing over the updates using MQTT packets over
 * BLE using a companion mobile app as the proxy to the cloud. Demo creates a transport
 * interface connection over BLE and runs the OTA and MQTT agent in separate tasks.
 * MQTT agent is used to handle thread safety of the MQTT library and underlying transport
 * interface.
 *
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE`
 * otherwise.
 *
 */
int RunOtaMqttBLETransportDemo( void )
{
    BaseType_t xDemoStatus = pdFAIL;
    BaseType_t xMqttInitialized = pdFALSE;

    LogInfo( ( "OTA over MQTT demo, Application version %u.%u.%u",
               appFirmwareVersion.u.x.major,
               appFirmwareVersion.u.x.minor,
               appFirmwareVersion.u.x.build ) );

    /* Initialize semaphore for buffer operations. */
    xBufferSemaphore = xSemaphoreCreateMutex();

    if( xBufferSemaphore == NULL )
    {
        LogError( ( "Failed to initialize buffer semaphore." ) );
    }
    else
    {
        xDemoStatus = pdPASS;
    }

    /****************************** Init MQTT ******************************/

    if( xDemoStatus == pdPASS )
    {
        if( prvConnectToMQTTBroker() != pdPASS )
        {
            LogError( ( "Failed to initialize MQTT, exiting" ) );
            xDemoStatus = pdFAIL;
        }
        else
        {
            xMqttInitialized = pdTRUE;
        }
    }

    /**
     * Register a callback for receiving messages intended for OTA agent from broker,
     * for which the topic has not been subscribed for.
     */
    prvRegisterOTACallback( otaexampleDEFAULT_TOPIC_FILTER, otaexampleDEFAULT_TOPIC_FILTER_LENGTH );

    /************************ Create MQTT Agent Task. ************************/

    if( xDemoStatus == pdPASS )
    {
        if( xTaskCreate( prvMQTTAgentTask,
                         "MQTT Agent Task",
                         MQTT_AGENT_TASK_STACK_SIZE,
                         NULL,
                         MQTT_AGENT_TASK_PRIORITY,
                         NULL ) != pdPASS )
        {
            xDemoStatus = pdFAIL;
            LogError( ( "Failed to create MQTT agent task:" ) );
        }
    }

    if( xDemoStatus == pdPASS )
    {
        /* Start OTA demo. The function returns only if OTA completes successfully and a
         * shutdown of OTA is triggered for a manual restart of the device.*/
        xDemoStatus = prvRunOTADemo();
    }

    /****************************** Cleanup ******************************/

    /**
     * Remvove callback for receiving messages intended for OTA agent from broker,
     * for which the topic has not been subscribed for.
     */
    SubscriptionManager_RemoveSubscription( ( SubscriptionElement_t * ) xGlobalMqttAgentContext.pIncomingCallbackContext,
                                            otaexampleDEFAULT_TOPIC_FILTER,
                                            otaexampleDEFAULT_TOPIC_FILTER_LENGTH );

    if( xMqttInitialized == pdTRUE )
    {
        prvDisconnectFromMQTTBroker();
    }

    if( xBufferSemaphore != NULL )
    {
        /* Cleanup semaphore created for buffer operations. */
        vSemaphoreDelete( xBufferSemaphore );
    }

    return( ( xDemoStatus == pdPASS ) ? EXIT_SUCCESS : EXIT_FAILURE );
}
