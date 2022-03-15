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
 * @file ota_demo_core_http.c
 * @brief OTA update example using coreMQTT and coreHTTP.
 */

/* Standard includes. */
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Demo include. */
#include "aws_demo_config.h"
#include "iot_network.h"

/* OTA library and demo configuration macros. */
#include "ota_config.h"
#include "ota_demo_config.h"

/* CoreMQTT-Agent APIS for running MQTT in a multithreaded environment. */
#include "freertos_agent_message.h"
#include "freertos_command_pool.h"

/* CoreMQTT-Agent include. */
#include "core_mqtt_agent.h"

/* Includes helpers for managing MQTT subscriptions. */
#include "mqtt_subscription_manager.h"

/* HTTP include. */
#include "core_http_client.h"

/* Include PKCS11 helper for random number generation. */
#include "pkcs11_helpers.h"

/* Common HTTP demo utilities. */
#include "http_demo_utils.h"

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

/* OTA Library Interface include. */
#include "ota_os_freertos.h"
#include "ota_mqtt_interface.h"

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

/**
 * @brief The MQTT client identifier used in this example.  Each client
 * identifier must be unique so edit as required to ensure no two clients
 * connecting to the same broker use the same client identifier.
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

/*
 * @brief Server's root CA certificate for TLS authentication with S3.
 *
 * @note This certificate should be PEM-encoded.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 *
 */
#ifndef democonfigHTTPS_ROOT_CA_PEM
    #define democonfigHTTPS_ROOT_CA_PEM    tlsATS1_ROOT_CERTIFICATE_PEM
#endif /* ifndef democonfigHTTPS_ROOT_CA_PEM */

/**
 * @brief AWS IoT Core server port number for HTTPS connections.
 *
 * For this demo, an X.509 certificate is used to verify the client.
 *
 * @note Port 443 requires use of the ALPN TLS extension with the ALPN protocol
 * name being x-amzn-http-ca. When using port 8443, ALPN is not required.
 */
#ifndef democonfigHTTPS_PORT
    #define democonfigHTTPS_PORT    443
#endif

/**
 * @brief Maximum length of the S3 Presigned URL generated for the demo.
 */
#define democonfigS3_PRESIGNED_GET_URL_LENGTH             ( 256U )

/**
 * @brief Transport timeout in milliseconds for transport send and receive.
 */
#define otaexampleHTTPS_TRANSPORT_SEND_RECV_TIMEOUT_MS    ( 2000U )


/**
 * @brief Transport timeout in milliseconds for transport send and receive.
 */
#define otaexampleMQTT_TRANSPORT_SEND_RECV_TIMEOUT_MS          ( 500U )

/**
 * @brief The common prefix for all OTA topics.
 *
 * Thing name is substituted with a wildcard symbol `+`. OTA agent
 * registers with MQTT broker with the thing name in the topic. This topic
 * filter is used to match incoming packet received and route them to OTA.
 * Thing name is not needed for this matching.
 */
#define otaexampleTOPIC_PREFIX                                 "$aws/things/+/"

/**
 * @brief Wildcard topic filter for job notification.
 * The filter is used to match the constructed job notify topic filter from OTA
 * agent and register appropirate callback for it.
 */
#define otaexampleJOB_NOTIFY_TOPIC_FILTER                      otaexampleTOPIC_PREFIX "jobs/notify-next"

/**
 * @brief Length of job notification topic filter.
 */
#define otaexampleJOB_NOTIFY_TOPIC_FILTER_LENGTH               ( ( uint16_t ) ( sizeof( otaexampleJOB_NOTIFY_TOPIC_FILTER ) - 1 ) )

/**
 * @brief Wildcard topic filter for matching job response messages.
 * This topic filter is used to match the responses from OTA service for OTA
 * agent job requests. The topic filter is a reserved topic which is not
 * subscribed with MQTT broker.
 */
#define otaexampleJOB_ACCEPTED_RESPONSE_TOPIC_FILTER           otaexampleTOPIC_PREFIX "jobs/$next/get/accepted"

/**
 * @brief Length of job accepted response topic filter.
 */
#define otaexampleJOB_ACCEPTED_RESPONSE_TOPIC_FILTER_LENGTH    ( ( uint16_t ) ( sizeof( otaexampleJOB_ACCEPTED_RESPONSE_TOPIC_FILTER ) - 1 ) )


/**
 * @brief Wildcard topic filter for matching OTA data packets.
 * The filter is used to match the constructed data stream topic filter from
 * OTA agent and register appropriate callback for it.
 */
#define otaexampleDATA_STREAM_TOPIC_FILTER           otaexampleTOPIC_PREFIX  "streams/#"

/**
 * @brief Length of data stream topic filter.
 */
#define otaexampleDATA_STREAM_TOPIC_FILTER_LENGTH    ( ( uint16_t ) ( sizeof( otaexampleDATA_STREAM_TOPIC_FILTER ) - 1 ) )


/**
 * @brief Default topic filter for OTA.
 * This is used to route all the packets for OTA reserved topics which OTA
 * agent has not subscribed for.
 */
#define otaexampleDEFAULT_TOPIC_FILTER           otaexampleTOPIC_PREFIX "jobs/#"

/**
 * @brief Length of default topic filter.
 */
#define otaexampleDEFAULT_TOPIC_FILTER_LENGTH    ( ( uint16_t ) ( sizeof( otaexampleDEFAULT_TOPIC_FILTER ) - 1 ) )

/**
 * @brief Stack size required for OTA agent task.
 * OTA agent task takes care of TLS connection and reconnection to S3 endpoint,
 * keeping task stack size to high enough required for TLS connection.
 */
#define otaexampleAGENT_TASK_STACK_SIZE          ( 6000U )

/**
 * @brief Priority required for OTA agent task.
 */
#define otaexampleAGENT_TASK_PRIORITY            ( tskIDLE_PRIORITY )

/**
 * @brief The delay used in the main OTA Demo task loop to periodically output
 * the OTA statistics like number of packets received, dropped, processed and
 * queued per connection.
 */
#define otaexampleTASK_DELAY_MS                  ( 1000U )

/**
 * @brief The timeout for waiting for the agent to get suspended after closing
 * the connection.
 * Timeout value should be large enough for OTA agent to finish any pending MQTT
 * operations and suspend itself.
 */
#define otaexampleSUSPEND_TIMEOUT_MS             ( 10000U )

/**
 * @brief The maximum size of the file paths used in the demo.
 */
#define otaexampleMAX_FILE_PATH_SIZE             ( 260U )

/**
 * @brief The maximum size of the stream name required for downloading update file
 * from streaming service.
 */
#define otaexampleMAX_STREAM_NAME_SIZE           ( 128U )

/**
 * @brief Maximum size of the url.
 */
#define otaexampleMAX_URL_SIZE                   ( 2048U )

/**
 * @brief Maximum size of the auth scheme.
 */
#define otaexampleMAX_AUTH_SCHEME_SIZE           ( 48U )

/**
 * @brief Size of the network buffer to receive the MQTT message.
 *
 * The largest message size is data size from the AWS IoT streaming service,
 * otaconfigFILE_BLOCK_SIZE + extra for headers.
 */

#define otaexampleNETWORK_BUFFER_SIZE               ( otaconfigFILE_BLOCK_SIZE + otaexampleMAX_URL_SIZE + 128 )

/**
 * @brief The number of ticks to wait for the OTA Agent to complete the shutdown.
 */
#define otaexampleOTA_SHUTDOWN_WAIT_TICKS           ( 0U )

/**
 * @brief Unsubscribe from the job topics when shutdown is called.
 */
#define otaexampleUNSUBSCRIBE_AFTER_OTA_SHUTDOWN    ( 1U )

/**
 * @brief The maximum number of retries for network operation with server.
 */
#define RETRY_MAX_ATTEMPTS                          ( 5U )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying failed
 * operation with server.
 */
#define RETRY_MAX_BACKOFF_DELAY_MS                  ( 5000U )

/**
 * @brief The base back-off delay (in milliseconds) to use for network operation
 * retry attempts.
 */
#define RETRY_BACKOFF_BASE_MS                       ( 500U )

/**
 * @brief ALPN (Application-Layer Protocol Negotiation) protocol name for AWS IoT MQTT.
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
#define MQTT_AGENT_TASK_STACK_SIZE                  ( 6000U )

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
 * @brief The maximum back-off delay (in milliseconds) for retrying connection
 * to server.
 */
#define CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS       ( 5000U )

/**
 * @brief The base back-off delay (in milliseconds) to use for connection
 * retry attempts.
 */
#define CONNECTION_RETRY_BACKOFF_BASE_MS            ( 500U )

/**
 * @brief The maximum number of retries for connecting to server.
 */
#define CONNECTION_RETRY_MAX_ATTEMPTS               ( 5U )

/**
 * @brief The maximum size of the HTTP header.
 */
#define HTTP_HEADER_SIZE_MAX                        ( 1024U )

/* HTTP buffers used for http request and response. */
#define HTTP_USER_BUFFER_LENGTH                     ( otaconfigFILE_BLOCK_SIZE + HTTP_HEADER_SIZE_MAX )

/**
 * @brief HTTP response codes used in this demo.
 */
#define HTTP_RESPONSE_PARTIAL_CONTENT               ( 206 )
#define HTTP_RESPONSE_BAD_REQUEST                   ( 400 )
#define HTTP_RESPONSE_FORBIDDEN                     ( 403 )
#define HTTP_RESPONSE_NOT_FOUND                     ( 404 )

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
 * When using multiple transports in the same compilation unit, define this
 * pointer as void* .
 *
 * @note Transport stacks are defined in `amazon-freertos/libraries/abstractions/transport/secure_sockets/transport_secure_sockets.h`.
 */
struct NetworkContext
{
    SecureSocketsTransportParams_t * pParams;
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
 * @brief The interface context used to post commands to the agent.
 * For FreeRTOS it's implemented using a FreeRTOS blocking queue.
 */
static MQTTAgentMessageInterface_t xMessageInterface;

/**
 * @brief FreeRTOS blocking queue to be used as MQTT Agent context.
 */
static MQTTAgentMessageContext_t xCommandQueue;

/**
 * @brief Flag for connection status to S3 service.
 */
static BaseType_t xHttpConnectionStatus;

/**
 * @brief The global array of subscription elements.
 *
 * @note The subscription manager implementation expects that the array of the
 * subscription elements used for storing subscriptions to be initialized to 0.
 * As this is a global array, it will be intialized to 0 by default.
 */
static SubscriptionElement_t pxGlobalSubscriptionList[ SUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS ];

/**
 * @brief The parameters for the network context using a TLS channel.
 */
static SecureSocketsTransportParams_t xMQTTSecureSocketsTransportParams;

/**
 * @brief Network connection context used in this demo for MQTT connection.
 */
static NetworkContext_t xNetworkContextMqtt;

/**
 * @brief Network connection context used for HTTP connection.
 */
static NetworkContext_t xNetworkContextHttp;

/**
 * @brief The host address string extracted from the pre-signed URL.
 *
 * @note S3_PRESIGNED_GET_URL_LENGTH is set as the array length here as the
 * length of the host name string cannot exceed this value.
 */
static char pcServerHost[ democonfigS3_PRESIGNED_GET_URL_LENGTH ];

/**
 * @brief The length of the host address found in the pre-signed URL.
 */
static size_t xServerHostLength;

/**
 * @brief A buffer used in the demo for storing HTTP request headers and
 * HTTP response headers and body.
 *
 * @note This demo shows how the same buffer can be re-used for storing the HTTP
 * response after the HTTP request is sent out. However, the user can also
 * decide to use separate buffers for storing the HTTP request and response.
 */
static uint8_t pucHttpUserBuffer[ HTTP_USER_BUFFER_LENGTH ];

/**
 * @brief The parameters for the network context using a TLS channel.
 */
static SecureSocketsTransportParams_t xHTTPSecureSocketsTransportParams;

/* The transport layer interface used by the HTTP Client library. */
static TransportInterface_t xTransportInterfaceHttp;

/**
 * @brief Semaphore for synchronizing buffer operations.
 */
static SemaphoreHandle_t xBufferSemaphore;

/**
 * @brief The location of the path within the pre-signed URL.
 */
static const char * pcPath;

/**
 * @brief Update File path buffer.
 */
static uint8_t pucUpdateFilePath[ otaexampleMAX_FILE_PATH_SIZE ];

/**
 * @brief Certificate File path buffer.
 */
static uint8_t pucCertFilePath[ otaexampleMAX_FILE_PATH_SIZE ];

/**
 * @brief Decode memory.
 */
static uint8_t pucDecodeMem[ otaconfigFILE_BLOCK_SIZE ];

/**
 * @brief Bitmap memory.
 */
static uint8_t pucBitmap[ OTA_MAX_BLOCK_BITMAP_SIZE ];

/**
 * @brief Certificate File path buffer.
 */
static uint8_t pucUpdateUrl[ otaexampleMAX_URL_SIZE ];

/**
 * @brief Auth scheme buffer.
 */
static uint8_t pucAuthScheme[ otaexampleMAX_URL_SIZE ];

/**
 * @brief Event buffer.
 */
static OtaEventData_t pxEventBuffer[ otaconfigMAX_NUM_OTA_DATA_BUFFERS ];

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
    .pDecodeMemory      = pucDecodeMem,
    .decodeMemorySize   = otaconfigFILE_BLOCK_SIZE,
    .pFileBitmap        = pucBitmap,
    .fileBitmapSize     = OTA_MAX_BLOCK_BITMAP_SIZE,
    .pUrl               = pucUpdateUrl,
    .urlSize            = otaexampleMAX_URL_SIZE,
    .pAuthScheme        = pucAuthScheme,
    .authSchemeSize     = otaexampleMAX_AUTH_SCHEME_SIZE
};

/*-----------------------------------------------------------*/

/**
 * @brief Initializes an MQTT Agent including transport interface and
 * network buffer.
 *
 * @return `MQTTSuccess` if the initialization succeeds, else `MQTTBadParameter`.
 */
static MQTTStatus_t prvMqttAgentInit( void );

/**
 * @brief Sends an MQTT CONNECT packet over the already connected TCP socket.
 *
 * @return MQTTSuccess if an MQTT session is established.
 * EXIT_FAILURE otherwise.
 */
static MQTTStatus_t prvMQTTConnect( void );

/**
 * @brief Publish message to a topic.
 *
 * This function publishes a message to a given topic & QoS.
 *
 * @param[in] pcTopic Mqtt topic filter.
 * @param[in] usTopicLen Length of the topic filter.
 * @param[in] pcMsg Message to publish.
 * @param[in] ulMsgSize Message size.
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
 * @param[in] usTopicFilterLength Length of the topic filter.
 * @param[in] ucQOS Quality of Service
 *
 * @return OtaMqttSuccess if success , other error code on failure.
 */
static OtaMqttStatus_t prvMqttSubscribe( const char * pcTopicFilter,
                                         uint16_t usTopicFilterLength,
                                         uint8_t ucQOS );

/**
 * @brief Unsubscribe to the Mqtt topics.
 *
 * This function unsubscribes to the Mqtt topics with the Quality of service
 * received as parameter.
 *
 * @param[in] pcTopicFilter Mqtt topic filter.
 * @param[in] usTopicFilterLength Length of the topic filter.
 * @param[in] ucQOS Quality of Service
 *
 * @return  OtaMqttSuccess if success , other error code on failure.
 */
static OtaMqttStatus_t prvMqttUnSubscribe( const char * pcTopicFilter,
                                           uint16_t usTopicFilterLength,
                                           uint8_t ucQOS );

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
 * @param[in] pxNetworkContext Network context to connect on.
 *
 * @return int pdFALSE if connection failed after retries.
 */
static BaseType_t prvCreateSocketConnectionToMQTTBroker( NetworkContext_t * pxNetworkContext );

/**
 * @brief Disconnect from the MQTT broker.
 *
 */
static void prvDisconnectFromMQTTBroker( void );

/**
 * @brief Handle HTTP response.
 *
 * @param[in] pxResponse Pointer to http response buffer.
 *
 * @return OtaHttpStatus_t OtaHttpSuccess if success or failure code otherwise.
 */
static OtaHttpStatus_t prvHandleHttpResponse( const HTTPResponse_t * pxResponse );

/**
 * @brief Initialize OTA Http interface.
 *
 * @param[in] pcUrl Pointer to the pre-signed url for downloading update file.
 *
 * @return OtaHttpStatus_t OtaHttpSuccess if success ,
 *                         OtaHttpInitFailed on failure.
 */
static OtaHttpStatus_t prvHttpInit( char * pcUrl );

/**
 * @brief Request file block over HTTP.
 *
 * @param[in] ulRangeStart  Starting index of the file data
 * @param[in] ulRangeEnd    Last index of the file data
 *
 * @return OtaHttpStatus_t OtaHttpSuccess if success ,
 *                         other errors on failure.
 */
static OtaHttpStatus_t prvHttpRequest( uint32_t ulRangeStart,
                                       uint32_t ulRangeEnd );

/**
 * @brief Deinitialize and cleanup of the HTTP connection.
 *
 * @return OtaHttpStatus_t  OtaHttpSuccess if success ,
 *                          OtaHttpRequestFailed on failure.
 */
static OtaHttpStatus_t prvHttpDeinit( void );

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

/**
 * @brief Calculate and perform an exponential backoff with jitter delay for
 * the next retry attempt of a failed network operation with the server.
 *
 * The function generates a random number, calculates the next backoff period
 * with the generated random number, and performs the backoff delay operation if
 * the number of retries have not exhausted.
 *
 * @note The PKCS11 module is used to generate the random number as it allows
 * access to a True Random Number Generator (TRNG) if the vendor platform
 * supports it. It is recommended to seed the random number generator with a
 * device-specific entropy source so that probability of collisions from devices
 * in connection retries is mitigated.
 *
 * @note The backoff period is calculated using the backoffAlgorithm library.
 *
 * @param[in, out] pxRetryAttempts The context to use for backoff period
 * calculation with the backoffAlgorithm library.
 *
 * @return pdPASS if calculating the backoff period was successful; otherwise
 * pdFAIL if there was failure in random number generation OR all retry attempts
 * had exhausted.
 */
static BaseType_t prvBackoffForRetry( BackoffAlgorithmContext_t * pxRetryParams );

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
 *
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
 * @param[in] pxPublishInfo MQTT packet information which stores details of the
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
 * that all unsolicited messages related to OTA is forwarded to this callback
 * for filteration. Right now the callback is used to filter responses to job
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
             * performed while shutting down please set the second parameter to 0 instead of 1.
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
    OtaEventData_t * pxData;
    OtaEventMsg_t xEventMsg = { 0 };

    configASSERT( pxPublishInfo != NULL );
    ( void ) pvIncomingPublishCallbackContext;

    LogInfo( ( "Received job message callback, size %ld.\n\n", pxPublishInfo->payloadLength ) );

    pxData = prvOtaEventBufferGet();

    if( pxData != NULL )
    {
        memcpy( pxData->data, pxPublishInfo->pPayload, pxPublishInfo->payloadLength );
        pxData->dataLength = pxPublishInfo->payloadLength;
        xEventMsg.eventId = OtaAgentEventReceivedJobDocument;
        xEventMsg.pEventData = pxData;

        /* Send job document received event. */
        ( void ) OTA_SignalEvent( &xEventMsg );
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
    OtaEventData_t * pxData;
    OtaEventMsg_t xEventMsg = { 0 };

    configASSERT( pxPublishInfo != NULL );
    ( void ) pvIncomingPublishCallbackContext;

    LogInfo( ( "Received data message callback, size %zu.\n\n", pxPublishInfo->payloadLength ) );

    pxData = prvOtaEventBufferGet();

    if( pxData != NULL )
    {
        memcpy( pxData->data, pxPublishInfo->pPayload, pxPublishInfo->payloadLength );
        pxData->dataLength = pxPublishInfo->payloadLength;
        xEventMsg.eventId = OtaAgentEventReceivedFileBlock;
        xEventMsg.pEventData = pxData;

        /* Send job document received event. */
        ( void ) OTA_SignalEvent( &xEventMsg );
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
    uint16_t usNumTopicFilters = sizeof( xOtaTopicFilterCallbacks ) / sizeof( OtaTopicFilterCallback_t );

    /* Match the input topic filter against the wild-card pattern of topics filters
    * relevant for the OTA Update service to determine the type of topic filter. */
    for( usIndex = 0U; usIndex < usNumTopicFilters; usIndex++ )
    {
        xMqttStatus = MQTT_MatchTopic( pcTopicFilter,
                                       usTopicFilterLength,
                                       xOtaTopicFilterCallbacks[ usIndex ].pcTopicFilter,
                                       xOtaTopicFilterCallbacks[ usIndex ].usTopicFilterLength,
                                       &xIsMatch );
        assert( xMqttStatus == MQTTSuccess );

        if( xIsMatch )
        {
            /* Add subscription so that incoming publishes are routed to the application callback. */
            xSubscriptionAdded = SubscriptionManager_AddSubscription( ( SubscriptionElement_t * ) xGlobalMqttAgentContext.pIncomingCallbackContext,
                                                                      pcTopicFilter,
                                                                      usTopicFilterLength,
                                                                      xOtaTopicFilterCallbacks[ usIndex ].xCallback,
                                                                      NULL );

            if( xSubscriptionAdded == false )
            {
                LogError( ( "Failed to register a publish callback for topic %.*s.",
                            usTopicFilterLength,
                            pcTopicFilter ) );
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
        /* Add subscription so that incoming publishes are routed to the application callback. */
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

static BaseType_t prvBackoffForRetry( BackoffAlgorithmContext_t * pxRetryParams )
{
    BaseType_t xReturnStatus = pdFAIL;
    uint16_t usNextRetryBackOff = 0U;
    BackoffAlgorithmStatus_t xBackoffAlgStatus = BackoffAlgorithmSuccess;

    /**
     * To calculate the backoff period for the next retry attempt, we will
     * generate a random number to provide to the backoffAlgorithm library.
     *
     * Note: The PKCS11 module is used to generate the random number as it allows
     * access to a True Random Number Generator (TRNG) if the vendor platform
     * supports it. It is recommended to use a random number generator seeded with
     * a device-specific entropy source so that probability of collisions from
     * devices in connection retries is mitigated.
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
                       pxRetryParams->attemptsDone,
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
    xTransport.send = SecureSocketsTransport_Send;
    xTransport.recv = SecureSocketsTransport_Recv;

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

static BaseType_t prvCreateSocketConnectionToMQTTBroker( NetworkContext_t * pxNetworkContext )
{
    ServerInfo_t xServerInfo = { 0 };
    SocketsConfig_t xSocketsConfig = { 0 };
    BaseType_t xStatus = pdPASS;
    TransportSocketStatus_t xNetworkStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;
    BackoffAlgorithmContext_t xReconnectParams;

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
                                       RETRY_MAX_ATTEMPTS );

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
        xNetworkStatus = SecureSocketsTransport_Connect( pxNetworkContext,
                                                         &xServerInfo,
                                                         &xSocketsConfig );

        if( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
        {
            xStatus = prvBackoffForRetry( &xReconnectParams );
        }
    } while( ( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS ) && ( xStatus == pdPASS ) );

    return xStatus;
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

static BaseType_t prvConnectToMQTTBroker( void )
{
    BaseType_t xStatus = pdFAIL;

    xNetworkContextMqtt.pParams = &xMQTTSecureSocketsTransportParams;

    /* Attempt to connect to the MQTT broker. If connection fails, retry after
     * a timeout. Timeout value will be exponentially increased till the maximum
     * attempts are reached or maximum timeout value is reached. The function
     * returns EXIT_FAILURE if the TCP connection cannot be established to
     * broker after configured number of attempts. */
    xStatus = prvCreateSocketConnectionToMQTTBroker( &xNetworkContextMqtt );

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

static void prvDisconnectFromMQTTBroker( void )
{
    MQTTAgentCommandContext_t xCommandContext;
    MQTTAgentCommandInfo_t xCommandParams = { 0 };
    MQTTStatus_t xCommandStatus;

    memset( &( xCommandContext ), 0, sizeof( MQTTAgentCommandContext_t ) );

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
    ( void ) SecureSocketsTransport_Disconnect( &xNetworkContextMqtt );
}
/*-----------------------------------------------------------*/

static int32_t prvConnectToS3Server( NetworkContext_t * pxNetworkContext,
                                     const char * pcUrl )
{
    BaseType_t returnStatus = pdPASS;
    HTTPStatus_t xHttpStatus = HTTPSuccess;
    /* The location of the host address within the pre-signed URL. */
    const char * pcAddress = NULL;
    TransportSocketStatus_t xNetworkStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;
    SocketsConfig_t xSocketsConfig = { 0 };
    /* Information about the server to send the HTTP requests. */
    ServerInfo_t xServerInfo = { 0 };

    /* Configure credentials for TLS mutual authenticated session. */
    xSocketsConfig.enableTls = true;
    xSocketsConfig.pAlpnProtos = NULL;
    xSocketsConfig.maxFragmentLength = 0;
    xSocketsConfig.disableSni = false;
    xSocketsConfig.pRootCa = democonfigHTTPS_ROOT_CA_PEM;
    xSocketsConfig.rootCaSize = sizeof( democonfigHTTPS_ROOT_CA_PEM );
    xSocketsConfig.sendTimeoutMs = otaexampleHTTPS_TRANSPORT_SEND_RECV_TIMEOUT_MS;
    xSocketsConfig.recvTimeoutMs = otaexampleHTTPS_TRANSPORT_SEND_RECV_TIMEOUT_MS;

    /* Retrieve the address location and length from S3_PRESIGNED_GET_URL. */
    if( pcUrl != NULL )
    {
        /* Retrieve the address location and length from S3_PRESIGNED_GET_URL. */
        xHttpStatus = getUrlAddress( pcUrl,
                                     strlen( pcUrl ),
                                     &pcAddress,
                                     &xServerHostLength );

        if( xHttpStatus != HTTPSuccess )
        {
            LogError( ( "URL %s parsing failed. Error code: %d",
                        pcUrl,
                        xHttpStatus ) );
            returnStatus = pdFAIL;
        }
        else
        {
            /* pcServerHost should consist only of the host address. */
            memcpy( pcServerHost, pcAddress, xServerHostLength );
            pcServerHost[ xServerHostLength ] = '\0';
        }
    }

    if( returnStatus != pdFAIL )
    {
        /* Initialize server information. */
        xServerInfo.pHostName = pcServerHost;
        xServerInfo.hostNameLength = xServerHostLength;
        xServerInfo.port = democonfigHTTPS_PORT;

        /* Establish a TLS session with the HTTP server. This example connects
         * to the HTTP server as specified in SERVER_HOST and HTTPS_PORT in
         * demo_config.h. */
        LogInfo( ( "Establishing a TLS session with %s:%d.",
                   pcServerHost,
                   democonfigHTTPS_PORT ) );

        xNetworkStatus = SecureSocketsTransport_Connect( pxNetworkContext,
                                                         &xServerInfo,
                                                         &xSocketsConfig );

        returnStatus = ( xNetworkStatus == TRANSPORT_SOCKET_STATUS_SUCCESS ) ? pdPASS : pdFAIL;
    }

    return ( returnStatus == pdPASS ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
/*-----------------------------------------------------------*/

static OtaHttpStatus_t prvHandleHttpResponse( const HTTPResponse_t * pxResponse )
{
    /* Return error code. */
    OtaHttpStatus_t xReturnStatus = OtaHttpRequestFailed;

    OtaEventData_t * pxData;
    OtaEventMsg_t xEventMsg = { 0 };

    switch( pxResponse->statusCode )
    {
        case HTTP_RESPONSE_PARTIAL_CONTENT:
            /* Get buffer to send event & data. */
            pxData = prvOtaEventBufferGet();

            if( pxData != NULL )
            {
                /* Get the data from response buffer. */
                memcpy( pxData->data, pxResponse->pBody, pxResponse->bodyLen );
                pxData->dataLength = pxResponse->bodyLen;

                /* Send job document received event. */
                xEventMsg.eventId = OtaAgentEventReceivedFileBlock;
                xEventMsg.pEventData = pxData;
                ( void ) OTA_SignalEvent( &xEventMsg );

                xReturnStatus = OtaHttpSuccess;
            }
            else
            {
                LogError( ( "Error: No OTA data buffers available." ) );

                xReturnStatus = OtaHttpRequestFailed;
            }

            break;

        case HTTP_RESPONSE_BAD_REQUEST:
        case HTTP_RESPONSE_FORBIDDEN:
        case HTTP_RESPONSE_NOT_FOUND:
            LogDebug( ( "HTTP response received: %d", pxResponse->statusCode ) );
            /* Request the job document to get new url. */
            xEventMsg.eventId = OtaAgentEventRequestJobDocument;
            xEventMsg.pEventData = NULL;
            ( void ) OTA_SignalEvent( &xEventMsg );

            xReturnStatus = OtaHttpSuccess;
            break;

        default:
            LogError( ( "Unhandled http response code: %d.",
                        pxResponse->statusCode ) );

            xReturnStatus = OtaHttpRequestFailed;
    }

    return xReturnStatus;
}
/*-----------------------------------------------------------*/

static OtaHttpStatus_t prvHttpInit( char * pcUrl )
{
    /* OTA lib return error code. */
    OtaHttpStatus_t xReturnStatus = OtaHttpSuccess;

    /* HTTPS Client library return status. */
    HTTPStatus_t xHttpStatus = HTTPSuccess;

    /* The length of the path within the pre-signed URL. This variable is
     * defined in order to store the length returned from parsing the URL, but
     * it is unused. The path used for the requests in this demo needs all the
     * query information following the location of the object, to the end of the
     * S3 presigned URL. */
    size_t pathLen = 0;

    /* Establish HTTPs connection */
    LogInfo( ( "Performing TLS handshake on top of the TCP connection." ) );

    xNetworkContextHttp.pParams = &xHTTPSecureSocketsTransportParams;

    if( xHttpConnectionStatus == pdTRUE )
    {
        /* End TLS session, then close TCP connection. */
        ( void ) SecureSocketsTransport_Disconnect( &xNetworkContextHttp );
    }

    /* Attempt to connect to the HTTPs server. */
    if( prvConnectToS3Server( &xNetworkContextHttp, pcUrl ) == EXIT_SUCCESS )
    {
        xHttpConnectionStatus = pdTRUE;

        /* Define the transport interface. */
        ( void ) memset( &xTransportInterfaceHttp, 0, sizeof( xTransportInterfaceHttp ) );
        xTransportInterfaceHttp.recv = SecureSocketsTransport_Recv;
        xTransportInterfaceHttp.send = SecureSocketsTransport_Send;
        xTransportInterfaceHttp.pNetworkContext = &xNetworkContextHttp;

        /* Retrieve the path location from url. This
         * function returns the length of the path without the query into
         * pathLen, which is left unused in this demo. */
        xHttpStatus = getUrlPath( pcUrl,
                                  strlen( pcUrl ),
                                  &pcPath,
                                  &pathLen );

        xReturnStatus = ( xHttpStatus == HTTPSuccess ) ? OtaHttpSuccess : OtaHttpInitFailed;
    }
    else
    {
        /* Log an error to indicate connection failure after all
         * reconnect attempts are over. */
        LogError( ( "Failed to connect to HTTP server %s.",
                    pcServerHost ) );

        xHttpConnectionStatus = pdFALSE;

        xReturnStatus = OtaHttpInitFailed;
    }

    return xReturnStatus;
}
/*-----------------------------------------------------------*/

static OtaHttpStatus_t prvHttpRequest( uint32_t ulRangeStart,
                                       uint32_t ulRangeEnd )
{
    /* OTA lib return error code. */
    OtaHttpStatus_t xReturnStatus = OtaHttpSuccess;

    /* Configurations of the initial request headers that are passed to
     * #HTTPClient_InitializeRequestHeaders. */
    HTTPRequestInfo_t xRequestInfo;
    /* Represents a response returned from an HTTP server. */
    HTTPResponse_t xResponse;
    /* Represents header data that will be sent in an HTTP request. */
    HTTPRequestHeaders_t xRequestHeaders;

    /* Return value of all methods from the HTTP Client library API. */
    HTTPStatus_t xHttpStatus = HTTPSuccess;

    /* Initialize all HTTP Client library API structs to 0. */
    ( void ) memset( &xRequestInfo, 0, sizeof( xRequestInfo ) );
    ( void ) memset( &xResponse, 0, sizeof( xResponse ) );
    ( void ) memset( &xRequestHeaders, 0, sizeof( xRequestHeaders ) );

    /* Initialize the request object. */
    xRequestInfo.pHost = pcServerHost;
    xRequestInfo.hostLen = xServerHostLength;
    xRequestInfo.pMethod = HTTP_METHOD_GET;
    xRequestInfo.methodLen = sizeof( HTTP_METHOD_GET ) - 1;
    xRequestInfo.pPath = pcPath;
    xRequestInfo.pathLen = strlen( pcPath );

    /* Set "Connection" HTTP header to "keep-alive" so that multiple requests
     * can be sent over the same established TCP connection. */
    xRequestInfo.reqFlags = HTTP_REQUEST_KEEP_ALIVE_FLAG;

    /* Set the buffer used for storing request headers. */
    xRequestHeaders.pBuffer = pucHttpUserBuffer;
    xRequestHeaders.bufferLen = HTTP_USER_BUFFER_LENGTH;

    xHttpStatus = HTTPClient_InitializeRequestHeaders( &xRequestHeaders,
                                                       &xRequestInfo );

    HTTPClient_AddRangeHeader( &xRequestHeaders, ulRangeStart, ulRangeEnd );

    if( xHttpStatus == HTTPSuccess )
    {
        /* Initialize the response object. The same buffer used for storing
         * request headers is reused here. */
        xResponse.pBuffer = pucHttpUserBuffer;
        xResponse.bufferLen = HTTP_USER_BUFFER_LENGTH;

        /* Send the request and receive the response. */
        xHttpStatus = HTTPClient_Send( &xTransportInterfaceHttp,
                                       &xRequestHeaders,
                                       NULL,
                                       0,
                                       &xResponse,
                                       0 );
    }
    else
    {
        LogError( ( "Failed to initialize HTTP request headers: Error=%s.",
                    HTTPClient_strerror( xHttpStatus ) ) );
    }

    if( xHttpStatus != HTTPSuccess )
    {
        if( ( xHttpStatus == HTTPNoResponse ) || ( xHttpStatus == HTTPNetworkError ) )
        {
            xHttpConnectionStatus = pdFALSE;
        }
        else
        {
            LogError( ( "HTTPClient_Send failed: Error=%s.",
                        HTTPClient_strerror( xHttpStatus ) ) );

            xReturnStatus = OtaHttpRequestFailed;
        }
    }
    else
    {
        /* Check if reconnection required. */
        if( xResponse.respFlags & HTTP_RESPONSE_CONNECTION_CLOSE_FLAG )
        {
            xHttpConnectionStatus = pdFALSE;
        }

        /* Handle the http response received. */
        xReturnStatus = prvHandleHttpResponse( &xResponse );
    }

    if( xHttpConnectionStatus == pdFALSE )
    {
        /* End TLS session, then close TCP connection. */
        ( void ) SecureSocketsTransport_Disconnect( &xNetworkContextHttp );

        /* Try establishing connection to S3 server again. */
        if( prvConnectToS3Server( &xNetworkContextHttp, NULL ) == EXIT_SUCCESS )
        {
            xHttpConnectionStatus = pdTRUE;

            xReturnStatus = OtaHttpSuccess;
        }
        else
        {
            xHttpConnectionStatus = pdFALSE;

            /* Log an error to indicate connection failure after all
             * reconnect attempts are over. */
            LogError( ( "Failed to connect to HTTP server %s.",
                        pcServerHost ) );

            xReturnStatus = OtaHttpRequestFailed;
        }
    }

    return xReturnStatus;
}
/*-----------------------------------------------------------*/

static OtaHttpStatus_t prvHttpDeinit( void )
{
    OtaHttpStatus_t xReturnStatus = OtaHttpSuccess;

    /* Nothing special to do here .*/

    return xReturnStatus;
}
/*-----------------------------------------------------------*/

static OtaMqttStatus_t prvMqttSubscribe( const char * pcTopicFilter,
                                         uint16_t usTopicFilterLength,
                                         uint8_t ucQOS )
{
    OtaMqttStatus_t xMqttStatus = OtaMqttSuccess;
    MQTTStatus_t xCommandStatus;
    MQTTAgentCommandInfo_t xCommandParams = { 0 };
    MQTTAgentCommandContext_t xCommandContext;
    MQTTSubscribeInfo_t xSubscription;
    MQTTAgentSubscribeArgs_t xSubscribeArgs = { 0 };

    memset( &( xCommandContext ), 0, sizeof( MQTTAgentCommandContext_t ) );
    memset( &( xSubscription ), 0, sizeof( MQTTSubscribeInfo_t ) );

    assert( pcTopicFilter != NULL );
    assert( usTopicFilterLength > 0 );

    xSubscription.qos = ( MQTTQoS_t ) ucQOS;
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
        xMqttStatus = OtaMqttSubscribeFailed;
    }
    else
    {
        LogInfo( ( "SUBSCRIBED to topic %.*s to broker.\n\n",
                   usTopicFilterLength,
                   pcTopicFilter ) );
    }

    return xMqttStatus;
}
/*-----------------------------------------------------------*/

static OtaMqttStatus_t prvMqttPublish( const char * const pcTopic,
                                       uint16_t usTopicLen,
                                       const char * pcMsg,
                                       uint32_t ulMsgSize,
                                       uint8_t ucQOS )
{
    OtaMqttStatus_t xMqttStatus = OtaMqttSuccess;
    MQTTStatus_t xCommandStatus;
    MQTTAgentCommandInfo_t xCommandParams = { 0 };
    MQTTAgentCommandContext_t xCommandContext;
    MQTTPublishInfo_t publishInfo;

    memset( &( xCommandContext ), 0, sizeof( MQTTAgentCommandContext_t ) );
    memset( &( publishInfo ), 0, sizeof( MQTTPublishInfo_t ) );

    /* Set the required publish parameters. */
    publishInfo.pTopicName = pcTopic;
    publishInfo.topicNameLength = usTopicLen;
    publishInfo.qos = ( MQTTQoS_t ) ucQOS;
    publishInfo.pPayload = pcMsg;
    publishInfo.payloadLength = ulMsgSize;

    xCommandParams.blockTimeMs = MQTT_AGENT_SEND_BLOCK_TIME_MS;
    xCommandParams.cmdCompleteCallback = prvMQTTAgentCmdCompleteCallback;
    xCommandParams.pCmdCompleteCallbackContext = &xCommandContext;
    xCommandContext.xTaskToNotify = xTaskGetCurrentTaskHandle();
    xCommandContext.pArgs = NULL;
    xCommandContext.xReturnStatus = MQTTSendFailed;

    xCommandStatus = MQTTAgent_Publish( &xGlobalMqttAgentContext, &publishInfo, &xCommandParams );
    configASSERT( xCommandStatus == MQTTSuccess );

    xTaskNotifyWait( 0,
                     0,
                     NULL,
                     pdMS_TO_TICKS( MQTT_AGENT_MS_TO_WAIT_FOR_NOTIFICATION ) );

    if( xCommandContext.xReturnStatus != MQTTSuccess )
    {
        LogError( ( "Failed to send PUBLISH packet to broker with error = %u.", xCommandContext.xReturnStatus ) );
        xMqttStatus = OtaMqttPublishFailed;
    }
    else
    {
        LogInfo( ( "Sent PUBLISH packet to broker %.*s to broker.\n\n",
                   usTopicLen,
                   pcTopic ) );
    }

    return xMqttStatus;
}
/*-----------------------------------------------------------*/

static OtaMqttStatus_t prvMqttUnSubscribe( const char * pcTopicFilter,
                                           uint16_t usTopicFilterLength,
                                           uint8_t ucQOS )
{
    OtaMqttStatus_t xMqttStatus = OtaMqttSuccess;
    MQTTStatus_t xCommandStatus;
    MQTTAgentCommandInfo_t xCommandParams = { 0 };
    MQTTAgentCommandContext_t xCommandContext;
    MQTTSubscribeInfo_t xSubscription;
    MQTTAgentSubscribeArgs_t xSubscribeArgs = { 0 };

    memset( &( xCommandContext ), 0, sizeof( MQTTAgentCommandContext_t ) );
    memset( &( xSubscription ), 0, sizeof( MQTTSubscribeInfo_t ) );

    xSubscription.qos = ( MQTTQoS_t ) ucQOS;
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
        xMqttStatus = OtaMqttSubscribeFailed;
    }
    else
    {
        LogInfo( ( "Unsubscribed from topic %.*s to broker.\n\n",
                   usTopicFilterLength,
                   pcTopicFilter ) );
    }

    return xMqttStatus;
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
    pxOtaInterfaces->mqtt.subscribe = prvMqttSubscribe;
    pxOtaInterfaces->mqtt.publish = prvMqttPublish;
    pxOtaInterfaces->mqtt.unsubscribe = prvMqttUnSubscribe;

    /* Initialize the OTA library HTTP Interface.*/
    pxOtaInterfaces->http.init = prvHttpInit;
    pxOtaInterfaces->http.request = prvHttpRequest;
    pxOtaInterfaces->http.deinit = prvHttpDeinit;

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
            ( void ) SecureSocketsTransport_Disconnect( &xNetworkContextMqtt );

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
            vTaskDelay( pdMS_TO_TICKS( otaexampleTASK_DELAY_MS ) );
            ulSuspendTimeout -= otaexampleTASK_DELAY_MS;
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
            vTaskDelay( pdMS_TO_TICKS( otaexampleTASK_DELAY_MS ) );
            ulSuspendTimeout -= otaexampleTASK_DELAY_MS;
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
    OtaInterfaces_t otaInterfaces;

    /* OTA library packet statistics per job.*/
    OtaAgentStatistics_t otaStatistics = { 0 };

    /* OTA Agent state returned from calling OTA_GetState.*/
    OtaState_t state = OtaAgentStateStopped;

    /* Set OTA Library interfaces.*/
    prvSetOtaInterfaces( &otaInterfaces );

    /****************************** Init OTA Library. ******************************/

    if( ( xOtaError = OTA_Init( &xOtaBuffer,
                                &otaInterfaces,
                                ( const uint8_t * ) ( democonfigCLIENT_IDENTIFIER ),
                                prvOtaAppCallback ) ) != OtaErrNone )
    {
        LogError( ( "Failed to initialize OTA Agent, exiting = %u.",
                    xOtaError ) );

        xStatus = pdFAIL;
    }
    else
    {
        xStatus = pdPASS;
    }

    /************************* Create OTA Agent Task. *************************/

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
        ( void ) OTA_SignalEvent( &xEventMsg );
    }

    /********************* Loop and display OTA statistics ********************/

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
            else
            {
                LogInfo( ( "OTA Agent is suspended." ) );
            }

            vTaskDelay( pdMS_TO_TICKS( otaexampleTASK_DELAY_MS ) );
        }
    }

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
 * @param[in] xAwsIotMqttMode Specify if this demo is running with the AWS IoT
 * MQTT server. Set this to `false` if using another MQTT server.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pxNetworkInterface The network interface to use for this demo.
 *
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE`
 * otherwise.
 *
 */
int RunOtaCoreHttpDemo( bool xAwsIotMqttMode,
                        const char * pIdentifier,
                        void * pNetworkServerInfo,
                        void * pNetworkCredentialInfo,
                        const IotNetworkInterface_t * pxNetworkInterface )
{
    ( void ) xAwsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;
    ( void ) pxNetworkInterface;

    /* Return error status. */
    BaseType_t xDemoStatus = pdFAIL;
    BaseType_t xMqttInitialized = pdFALSE;

    LogInfo( ( "OTA over HTTP demo, Application version %u.%u.%u",
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

    /******************************* Init MQTT *******************************/

    if( xDemoStatus == pdPASS )
    {
        if( prvConnectToMQTTBroker() != pdPASS )
        {
            LogError( ( "Failed to initialize MQTT, exiting" ) );
            xDemoStatus = pdFAIL;
        }
        else
        {
            xMqttInitialized = true;
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
        if( prvRunOTADemo() != pdPASS )
        {
            xDemoStatus = pdFAIL;
        }
    }

    /****************************** Cleanup ******************************/

    /**
     * Remvove callback for receiving messages intended for OTA agent from broker,
     * for which the topic has not been subscribed for.
     */
    SubscriptionManager_RemoveSubscription( ( SubscriptionElement_t * ) xGlobalMqttAgentContext.pIncomingCallbackContext,
                                            otaexampleDEFAULT_TOPIC_FILTER,
                                            otaexampleDEFAULT_TOPIC_FILTER_LENGTH );

    if( xMqttInitialized )
    {
        prvDisconnectFromMQTTBroker();
        ( void ) SecureSocketsTransport_Disconnect( &xNetworkContextHttp );
    }

    if( xBufferSemaphore != NULL )
    {
        /* Cleanup semaphore created for buffer operations. */
        vSemaphoreDelete( xBufferSemaphore );
    }

    return( ( xDemoStatus == pdPASS ) ? EXIT_SUCCESS : EXIT_FAILURE );
}
