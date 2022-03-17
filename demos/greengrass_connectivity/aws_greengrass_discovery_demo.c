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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file aws_greengrass_discovery_demo.c
 * @brief A simple Greengrass discovery example.
 *
 * A simple example that perform discovery of the greengrass core device.
 * The JSON file is retrieved.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* Demo include. */
#include "aws_demo_config.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "platform/iot_network.h"
#include "platform/iot_clock.h"

/* Greengrass includes. */
#include "aws_ggd_config.h"
#include "aws_ggd_config_defaults.h"
#include "aws_greengrass_discovery.h"

/* Includes for initialization. */
#include "core_mqtt.h"

/* Include header for connection configurations. */
#include "aws_clientcredential.h"

/* Include header for client credentials. */
#include "aws_clientcredential_keys.h"

/* Include PKCS11 helpers header. */
#include "pkcs11_helpers.h"

/* Include AWS IoT metrics macros header. */
#include "aws_iot_metrics.h"

/* Include header for root CA certificates. */
#include "iot_default_root_certificates.h"

/* Transport interface implementation include header for TLS. */
#include "transport_secure_sockets.h"

/* Retry utilities include. */
#include "backoff_algorithm.h"

#define ggdDEMO_MAX_MQTT_MESSAGES              3
#define ggdDEMO_MAX_MQTT_MSG_SIZE              500
#define ggdDEMO_DISCOVERY_FILE_SIZE            2500
#define ggdDEMO_MQTT_MSG_TOPIC                 "freertos/demos/ggd"
#define ggdDEMO_MQTT_MSG_DISCOVERY             "{\"message\":\"Hello #%lu from FreeRTOS to Greengrass Core.\"}"

#define ggdDEMO_KEEP_ALIVE_INTERVAL_SECONDS    1200
/* Number of times to try MQTT connection. */
#define ggdDEMO_NUM_TRIES                      3
#define ggdDEMO_RETRY_WAIT_MS                  2000

#define NETWORK_BUFFER_SIZE                    ( 1024U )

/**
 * @brief Milliseconds per second.
 */
#define MILLISECONDS_PER_SECOND                ( 1000U )

/**
 * @brief Milliseconds per FreeRTOS tick.
 */
#define MILLISECONDS_PER_TICK                  ( MILLISECONDS_PER_SECOND / configTICK_RATE_HZ )

/**
 * @brief The maximum number of retries for network operation with server.
 */
#define RETRY_MAX_ATTEMPTS                     ( 5U )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying failed operation
 *  with server.
 */
#define RETRY_MAX_BACKOFF_DELAY_MS             ( 5000U )

/**
 * @brief The base back-off delay (in milliseconds) to use for network operation retry
 * attempts.
 */
#define RETRY_BACKOFF_BASE_MS                  ( 500U )

/**
 * @brief Timeout for receiving CONNACK packet in milliseconds.
 */
#define CONNACK_RECV_TIMEOUT_MS                ( 1000U )

/**
 * @brief Transport timeout in milliseconds for transport send and receive.
 */
#define TRANSPORT_SEND_RECV_TIMEOUT_MS         ( 500U )

/**
 * @brief Contains the user data for callback processing.
 */
typedef struct
{
    const char * pcExpectedString;      /**< Informs the MQTT callback of the next expected string. */
    BaseType_t xCallbackStatus;         /**< Used to communicate the success or failure of the callback function.
                                         * xCallbackStatus is set to pdFALSE before the callback is executed, and is
                                         * set to pdPASS inside the callback only if the callback receives the expected
                                         * data. */
    SemaphoreHandle_t xWakeUpSemaphore; /**< Handle of semaphore to wake up the task. */
    char * pcTopic;                     /**< Topic to subscribe and publish to. */
} GGDUserData_t;

/**
 * @brief Defines Network Context to be used in this demo.
 */
struct NetworkContext
{
    SecureSocketsTransportParams_t * pParams;
};

/**
 * @brief Global entry time into the application to use as a reference timestamp
 * in the #prvGetTimeMs function. #prvGetTimeMs will always return the difference
 * between the current time and the global entry time. This will reduce the chances
 * of overflow for the 32 bit unsigned integer used for holding the timestamp.
 */
static uint32_t ulGlobalEntryTimeMs;

static const uint32_t _timeBetweenPublishMs = 1500UL;

static char pcJSONFile[ ggdDEMO_DISCOVERY_FILE_SIZE ];

/**
 * @brief Static buffer used to hold MQTT messages being sent and received.
 */
static uint8_t ucSharedBuffer[ NETWORK_BUFFER_SIZE ];

/** @brief Static buffer used to hold MQTT messages being sent and received. */
static MQTTFixedBuffer_t xBuffer =
{
    ucSharedBuffer,
    NETWORK_BUFFER_SIZE
};

/**
 * @brief This function discovers the greengrass core device.
 * It fetches the ip address and certificate of greengrass core device.
 *
 * @return EXIT_SUCCESS if device is successfully discovered, else EXIT_FAILURE .
 */
static int discoverGreengrassCore();

/**
 * @brief This function publish messages to the discovered greengrass core device.
 *
 * @param[in] pxMQTTContext MQTT context pointer.
 */
static void sendMessageToGGC( MQTTContext_t * pxMQTTContext );

/**
 * @brief The application callback function for getting the incoming publishes,
 * incoming acks, and ping responses reported from the MQTT library.
 *
 * @param[in] pxMQTTContext MQTT context pointer.
 * @param[in] pxPacketInfo Packet Info pointer for the incoming packet.
 * @param[in] pxDeserializedInfo Deserialized information from the incoming packet.
 */
static void prvEventCallback( MQTTContext_t * pxMQTTContext,
                              MQTTPacketInfo_t * pxPacketInfo,
                              MQTTDeserializedInfo_t * pxDeserializedInfo );

/**
 * @brief The timer query function provided to the MQTT context.
 *
 * @return Time in milliseconds.
 */
static uint32_t prvGetTimeMs( void );

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
 * @brief Connect to Greengrass core with reconnection retries.
 *
 * If connection fails, retry is attempted after a timeout.
 * Timeout value will exponentially increase until maximum
 * timeout value is reached or the number of attempts are exhausted.
 *
 * @param[in] pxHostAddressData Discovered Greengrass core device data.
 * @param[in] pxMQTTContext MQTT context pointer.
 * @param[out] pxNetworkContext The output parameter to return the created network context.
 *
 * @return pdFAIL on failure; pdPASS on successful TLS+TCP network connection.
 */
static BaseType_t prvConnectToServerWithBackoffRetries( GGD_HostAddressData_t * pxHostAddressData,
                                                        MQTTContext_t * pxMQTTContext,
                                                        NetworkContext_t * pxNetworkContext );

/**
 * @brief Sends an MQTT Connect packet over the already connected TLS over TCP connection.
 *
 * @param[in, out] pxMQTTContext MQTT context pointer.
 * @param[in] pxNetworkContext Network context.
 *
 * @return pdFAIL on failure; pdPASS on successful MQTT connection.
 */
static BaseType_t prvCreateMQTTConnectionWithBroker( MQTTContext_t * pxMQTTContext,
                                                     NetworkContext_t * pxNetworkContext );

/*-----------------------------------------------------------*/
static void prvEventCallback( MQTTContext_t * pxMQTTContext,
                              MQTTPacketInfo_t * pxPacketInfo,
                              MQTTDeserializedInfo_t * pxDeserializedInfo )
{
    /* Unused parameters */
    ( void ) pxMQTTContext;
    ( void ) pxPacketInfo;
    ( void ) pxDeserializedInfo;
}

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
        }
    }
    else
    {
        LogError( ( "Unable to retry operation with broker: Random number generation failed" ) );
    }

    return xReturnStatus;
}

static BaseType_t prvCreateMQTTConnectionWithBroker( MQTTContext_t * pxMQTTContext,
                                                     NetworkContext_t * pxNetworkContext )
{
    MQTTStatus_t xResult;
    MQTTConnectInfo_t xConnectInfo;
    bool xSessionPresent;
    TransportInterface_t xTransport;
    BaseType_t xStatus = pdFAIL;

    /* Fill in Transport Interface send and receive function pointers. */
    xTransport.pNetworkContext = pxNetworkContext;
    xTransport.send = SecureSocketsTransport_Send;
    xTransport.recv = SecureSocketsTransport_Recv;

    /* Initialize MQTT library. */
    xResult = MQTT_Init( pxMQTTContext, &xTransport, prvGetTimeMs, prvEventCallback, &xBuffer );
    configASSERT( xResult == MQTTSuccess );

    /* Some fields are not used in this demo so start with everything at 0. */
    ( void ) memset( ( void * ) &xConnectInfo, 0x00, sizeof( xConnectInfo ) );

    /* Start with a clean session i.e. direct the MQTT broker to discard any
     * previous session data. Also, establishing a connection with clean session
     * will ensure that the broker does not store any data when this client
     * gets disconnected. */
    xConnectInfo.cleanSession = true;

    /* The client identifier is used to uniquely identify this MQTT client to
     * the MQTT broker. In a production device the identifier can be something
     * unique, such as a device serial number. */
    xConnectInfo.pClientIdentifier = ( const char * ) ( clientcredentialIOT_THING_NAME );
    xConnectInfo.clientIdentifierLength = ( uint16_t ) ( strlen( clientcredentialIOT_THING_NAME ) );

    /* Use the metrics string as username to report the OS and MQTT client version
     * metrics to AWS IoT. */
    xConnectInfo.pUserName = AWS_IOT_METRICS_STRING;
    xConnectInfo.userNameLength = AWS_IOT_METRICS_STRING_LENGTH;

    /* Set MQTT keep-alive period. If the application does not send packets at an interval less than
     * the keep-alive period, the MQTT library will send PINGREQ packets. */
    xConnectInfo.keepAliveSeconds = ggdDEMO_KEEP_ALIVE_INTERVAL_SECONDS;

    /* Send MQTT CONNECT packet to broker. LWT is not used in this demo, so it
     * is passed as NULL. */
    xResult = MQTT_Connect( pxMQTTContext,
                            &xConnectInfo,
                            NULL,
                            CONNACK_RECV_TIMEOUT_MS,
                            &xSessionPresent );

    if( xResult == MQTTSuccess )
    {
        xStatus = pdPASS;
    }

    return xStatus;
}

static BaseType_t prvConnectToServerWithBackoffRetries( GGD_HostAddressData_t * pxHostAddressData,
                                                        MQTTContext_t * pxMQTTContext,
                                                        NetworkContext_t * pxNetworkContext )
{
    ServerInfo_t xServerInfo = { 0 };

    SocketsConfig_t xSocketsConfig = { 0 };
    TransportSocketStatus_t xNetworkStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;
    BackoffAlgorithmContext_t xReconnectParams;
    BaseType_t xBackoffStatus = pdFAIL;
    BaseType_t xMqttStatus = pdFAIL;

    /* Set the credentials for establishing a TLS connection. */
    /* Initializer server information. */
    xServerInfo.pHostName = pxHostAddressData->pcHostAddress;
    xServerInfo.hostNameLength = strlen( pxHostAddressData->pcHostAddress );
    xServerInfo.port = clientcredentialMQTT_BROKER_PORT;

    /* Configure credentials for TLS mutual authenticated session. */
    xSocketsConfig.enableTls = true;
    xSocketsConfig.pAlpnProtos = NULL;
    xSocketsConfig.maxFragmentLength = 0;
    xSocketsConfig.disableSni = true;
    xSocketsConfig.pRootCa = pxHostAddressData->pcCertificate;
    xSocketsConfig.rootCaSize = ( size_t ) pxHostAddressData->ulCertificateSize;
    xSocketsConfig.sendTimeoutMs = TRANSPORT_SEND_RECV_TIMEOUT_MS;
    xSocketsConfig.recvTimeoutMs = TRANSPORT_SEND_RECV_TIMEOUT_MS;

    /* Initialize reconnect attempts and interval. */
    BackoffAlgorithm_InitializeParams( &xReconnectParams,
                                       RETRY_BACKOFF_BASE_MS,
                                       RETRY_MAX_BACKOFF_DELAY_MS,
                                       RETRY_MAX_ATTEMPTS );

    /* Attempt to connect to MQTT broker. If connection fails, retry after
     * a timeout. Timeout value will exponentially increase until maximum
     * attempts are reached.
     */
    do
    {
        /* Establish a TLS session with the MQTT broker. This example connects to
         * the Greengrass Core as specified in pxHostAddressData->pcHostAddress and
         * clientcredentialMQTT_BROKER_PORT at the top of this file. */
        LogInfo( ( "Creating a TLS connection to %s:%u.",
                   pxHostAddressData->pcHostAddress, clientcredentialMQTT_BROKER_PORT ) );
        /* Attempt to create a mutually authenticated TLS connection. */
        xNetworkStatus = SecureSocketsTransport_Connect( pxNetworkContext,
                                                         &xServerInfo,
                                                         &xSocketsConfig );

        if( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
        {
            LogWarn( ( "Connection to the broker failed. Attempting connection retry after backoff delay." ) );

            /* As the connection attempt failed, we will retry the connection after an
             * exponential backoff with jitter delay. */

            /* Calculate the backoff period for the next retry attempt and perform the wait operation. */
            xBackoffStatus = prvBackoffForRetry( &xReconnectParams );
        }
        else
        {
            xMqttStatus = prvCreateMQTTConnectionWithBroker( pxMQTTContext, pxNetworkContext );

            if( xMqttStatus != pdPASS )
            {
                LogError( ( "Failed to establish MQTT connection: Server=%s", pxHostAddressData->pcHostAddress ) );

                /* Calculate the backoff period for the next retry attempt and perform the wait operation. */
                xBackoffStatus = prvBackoffForRetry( &xReconnectParams );
                /* Close the network connection.  */
                xNetworkStatus = SecureSocketsTransport_Disconnect( pxNetworkContext );

                if( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
                {
                    LogError( ( "SecureSocketsTransport_Disconnect() failed to close the network connection. "
                                "StatusCode=%d.", ( int ) xNetworkStatus ) );
                }
            }
            else
            {
                /* Successfully established MQTT connection with the broker. */
                LogInfo( ( "An MQTT connection is established with %s.", pxHostAddressData->pcHostAddress ) );
            }
        }
    } while( ( xMqttStatus != pdPASS ) && ( xBackoffStatus == pdPASS ) );

    return xMqttStatus;
}

static void sendMessageToGGC( MQTTContext_t * pxMQTTContext )
{
    MQTTStatus_t xResult;
    MQTTPublishInfo_t xMQTTPublishInfo;
    uint32_t ulMessageCounter;
    char cBuffer[ ggdDEMO_MAX_MQTT_MSG_SIZE ];
    const char * pcTopic = ggdDEMO_MQTT_MSG_TOPIC;

    /* Some fields are not used by this demo so start with everything at 0. */
    ( void ) memset( ( void * ) &xMQTTPublishInfo, 0x00, sizeof( xMQTTPublishInfo ) );

    /* This demo uses QoS0. */
    xMQTTPublishInfo.qos = MQTTQoS0;
    xMQTTPublishInfo.retain = false;
    xMQTTPublishInfo.pTopicName = pcTopic;
    xMQTTPublishInfo.topicNameLength = ( uint16_t ) strlen( pcTopic );

    for( ulMessageCounter = 0; ulMessageCounter < ( uint32_t ) ggdDEMO_MAX_MQTT_MESSAGES; ulMessageCounter++ )
    {
        xMQTTPublishInfo.pPayload = ( const void * ) cBuffer;
        xMQTTPublishInfo.payloadLength = ( uint32_t ) sprintf( cBuffer, ggdDEMO_MQTT_MSG_DISCOVERY, ( long unsigned int ) ulMessageCounter ); /*lint !e586 sprintf can be used for specific demo. */

        /* Send PUBLISH packet. Packet ID is not used for a QoS0 publish. */
        xResult = MQTT_Publish( pxMQTTContext, &xMQTTPublishInfo, 0 );

        if( xResult != MQTTSuccess )
        {
            LogError( ( "Failed to send PUBLISH message to broker: Topic=%s, Error=%s",
                        pcTopic,
                        MQTT_Status_strerror( xResult ) ) );
        }

        LogInfo( ( "MQTT PUBLISH message sent successfully to the broker: Topic=%s", pcTopic ) );
        vTaskDelay( pdMS_TO_TICKS( _timeBetweenPublishMs ) );
    }
}

static int discoverGreengrassCore()
{
    int status = EXIT_SUCCESS;
    GGD_HostAddressData_t xHostAddressData;
    BaseType_t xDemoStatus = pdFAIL;
    SecureSocketsTransportParams_t secureSocketsTransportParams = { 0 };
    TransportSocketStatus_t xNetworkStatus;
    NetworkContext_t xNetworkContext = { 0 };
    MQTTContext_t xMQTTContext = { 0 };
    MQTTStatus_t xMQTTStatus;


    memset( &xHostAddressData, 0, sizeof( xHostAddressData ) );

    /* Demonstrate automated connection. */
    LogInfo( ( "Attempting automated selection of Greengrass device\r\n" ) );

    if( GGD_GetGGCIPandCertificate( clientcredentialMQTT_BROKER_ENDPOINT,
                                    clientcredentialGREENGRASS_DISCOVERY_PORT,
                                    clientcredentialIOT_THING_NAME,
                                    pcJSONFile,
                                    ggdDEMO_DISCOVERY_FILE_SIZE,
                                    &xHostAddressData )
        == pdPASS )
    {
        LogInfo( ( "Greengrass device discovered." ) );

        /* Set the entry time of the demo application. This entry time will be used
         * to calculate relative time elapsed in the execution of the demo application,
         * by the timer utility function that is provided to the MQTT library.
         */
        ulGlobalEntryTimeMs = prvGetTimeMs();
        xNetworkContext.pParams = &secureSocketsTransportParams;

        /****************************** Connect. ******************************/

        /* Attempt to establish TLS session with MQTT broker. If connection fails,
         * retry after a timeout. Timeout value will be exponentially increased until
         * the maximum number of attempts are reached or the maximum timeout value is reached.
         * The function returns a failure status if the TLS over TCP connection cannot be established
         * to the broker after the configured number of attempts. */
        xDemoStatus = prvConnectToServerWithBackoffRetries( &xHostAddressData, &xMQTTContext, &xNetworkContext );

        if( xDemoStatus == pdPASS )
        {
            sendMessageToGGC( &xMQTTContext );

            LogInfo( ( "Disconnecting from broker." ) );

            /* Call MQTT DISCONNECT function. */
            xMQTTStatus = MQTT_Disconnect( &xMQTTContext );

            if( xMQTTStatus != MQTTSuccess )
            {
                LogError( ( "Failed to DISCONNECT MQTT connection: MQTTStatus=%s", MQTT_Status_strerror( xMQTTStatus ) ) );
            }

            /* We will always close the network connection, even if an error may have occurred during
             * demo execution, to clean up the system resources that it may have consumed. */
            /* Close the network connection.  */
            xNetworkStatus = SecureSocketsTransport_Disconnect( &xNetworkContext );

            if( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
            {
                status = EXIT_FAILURE;
                LogError( ( "SecureSocketsTransport_Disconnect() failed to close the network connection. "
                            "StatusCode=%d.", ( int ) xNetworkStatus ) );
            }

            LogInfo( ( "Disconnected from the broker." ) );
        }
        else
        {
            LogError( ( "Could not connect to the Broker." ) );
            status = EXIT_FAILURE;
        }

        /* Report on space efficiency of this demo task. */
        #if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )
            {
                configPRINTF( ( "Heap low watermark: %u. Stack high watermark: %u.\r\n",
                                xPortGetMinimumEverFreeHeapSize(),
                                uxTaskGetStackHighWaterMark( NULL ) ) );
            }
        #endif
    }
    else
    {
        LogError( ( "Auto-connect: Failed to retrieve Greengrass address and certificate." ) );
        status = EXIT_FAILURE;
    }

    return status;
}

/*-----------------------------------------------------------*/

int vStartGreenGrassDiscoveryTask( bool awsIotMqttMode,
                                   const char * pIdentifier,
                                   void * pNetworkServerInfo,
                                   void * pNetworkCredentialInfo,
                                   const IotNetworkInterface_t * pNetworkInterface )
{
    /* Exit status of this program. */
    int status = EXIT_SUCCESS;

    /* Unused parameters */
    ( void ) awsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;

    status = discoverGreengrassCore();

    return status;
}
