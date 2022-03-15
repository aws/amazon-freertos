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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 */

/*
 * Demo for showing use of the HTTP API using a mutually-authenticated network
 * connection.
 *
 * The example shown below uses HTTP APIs to first create a
 * mutually-authenticated network connection with an HTTP server, and then send
 * a POST request containing a simple message. This example is single threaded
 * and uses statically allocated memory. It uses QoS1 for sending and receiving
 * messages from the server.
 *
 * A mutually-authenticated TLS connection is used to connect to the HTTP server
 * in this example. Define democonfigAWS_IOT_ENDPOINT and democonfigROOT_CA_PEM
 * in http_demo_mutual_auth_config.h, and define the client private key and
 * certificate in aws_clientcredential_keys.h, to establish a mutually
 * authenticated connection.
 */

/**
 * @file http_demo_mutual_auth.c
 * @brief Demonstrates usage of the HTTP library.
 *
 * @note This demo uses retry logic to connect to the server if connection attempts fail.
 * The FreeRTOS/backoffAlgorithm library is used to calculate the retry interval with an exponential
 * backoff and jitter algorithm. For generating random number required by the algorithm, the PKCS11
 * module is used as it allows access to a True Random Number Generator (TRNG) if the vendor platform
 * supports it.
 * It is RECOMMENDED to seed the random number generator with a device-specific entropy source so that
 * probability of collisions from devices in connection retries is mitigated.
 */
/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Demo Specific configs. */
#include "http_demo_mutual_auth_config.h"

/* Include common demo header. */
#include "aws_demo.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Transport interface implementation include header for TLS. */
#include "transport_secure_sockets.h"

/* Common HTTP demo utilities. */
#include "http_demo_utils.h"

/* HTTP API header. */
#include "core_http_client.h"

/* Include header for connection configurations. */
#include "aws_clientcredential.h"

/* Include header for root CA certificates. */
#include "iot_default_root_certificates.h"

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

/* Check that a path for HTTP Method POST is defined. */
#ifndef democonfigPOST_PATH
    #error "Please define democonfigPOST_PATH."
#endif

/* Check that a request body to send for the POST request is defined. */
#ifndef democonfigREQUEST_BODY
    #error "Please define a democonfigREQUEST_BODY."
#endif

/* Check that the AWS IoT Core endpoint is defined. */
#ifndef democonfigAWS_IOT_ENDPOINT
    #define democonfigAWS_IOT_ENDPOINT    clientcredentialMQTT_BROKER_ENDPOINT
#endif

/* Check that a TLS port for AWS IoT Core is defined. */
#ifndef democonfigAWS_HTTP_PORT
    #define democonfigAWS_HTTP_PORT    clientcredentialMQTT_BROKER_PORT
#endif

/* Check that the root CA certificate is defined. */
#ifndef democonfigROOT_CA_PEM
    #define democonfigROOT_CA_PEM    tlsATS1_ROOT_CERTIFICATE_PEM
#endif

/* Check that a transport timeout for transport send and receive is defined. */
#ifndef democonfigTRANSPORT_SEND_RECV_TIMEOUT_MS
    #define democonfigTRANSPORT_SEND_RECV_TIMEOUT_MS    ( 1000 )
#endif

/* Check that a size for the user buffer is defined. */
#ifndef democonfigUSER_BUFFER_LENGTH
    #define democonfigUSER_BUFFER_LENGTH    ( 2048 )
#endif

/**
 * @brief The length of the AWS IoT Endpoint.
 */
#define httpexampleAWS_IOT_ENDPOINT_LENGTH    ( sizeof( democonfigAWS_IOT_ENDPOINT ) - 1 )

/**
 * @brief The length of the HTTP POST method.
 */
#define httpexampleHTTP_METHOD_POST_LENGTH    ( sizeof( HTTP_METHOD_POST ) - 1 )

/**
 * @brief The length of the HTTP POST path.
 */
#define httpexamplePOST_PATH_LENGTH           ( sizeof( democonfigPOST_PATH ) - 1 )

/**
 * @brief Length of the request body.
 */
#define httpexampleREQUEST_BODY_LENGTH        ( sizeof( democonfigREQUEST_BODY ) - 1 )

/**
 * @brief The maximum number of times to run the loop in this demo.
 */
#ifndef httpexampleMAX_DEMO_COUNT
    #define httpexampleMAX_DEMO_COUNT    ( 3 )
#endif

/**
 * @brief Time in ticks to wait between each cycle of the demo implemented
 * by RunCoreHttpMutualAuthDemo().
 */
#define httpexampleDELAY_BETWEEN_DEMO_ITERATIONS_TICKS    ( pdMS_TO_TICKS( 5000U ) )


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

/*-----------------------------------------------------------*/

/**
 * @brief A buffer used in the demo for storing HTTP request headers and
 * HTTP response headers and body.
 *
 * @note This demo shows how the same buffer can be re-used for storing the HTTP
 * response after the HTTP request is sent out. However, the user can also
 * decide to use separate buffers for storing the HTTP request and response.
 */
static uint8_t ucUserBuffer[ democonfigUSER_BUFFER_LENGTH ];

/*-----------------------------------------------------------*/

/**
 * @brief Connect to HTTP server with reconnection retries.
 *
 * @param[out] pxNetworkContext The output parameter to return the created network context.
 *
 * @return pdPASS on successful connection, pdFAIL otherwise.
 */
static BaseType_t prvConnectToServer( NetworkContext_t * pxNetworkContext );

/**
 * @brief Send an HTTP request based on a specified method and path, then
 * print the response received from the server.
 *
 * @param[in] pxTransportInterface The transport interface for making network calls.
 * @param[in] pcMethod The HTTP request method.
 * @param[in] xMethodLen The length of the HTTP request method.
 * @param[in] pcPath The Request-URI to the objects of interest.
 * @param[in] xPathLen The length of the Request-URI.
 *
 * @return pdFAIL on failure; pdPASS on success.
 */
static BaseType_t prvSendHttpRequest( const TransportInterface_t * pxTransportInterface,
                                      const char * pcMethod,
                                      size_t xMethodLen,
                                      const char * pcPath,
                                      size_t xPathLen );

/*-----------------------------------------------------------*/

/**
 * @brief Entry point of the demo.
 *
 * This example resolves the AWS IoT Core endpoint, establishes a TCP
 * connection, and performs a mutually authenticated TLS handshake such that all
 * further communication is encrypted. After which, the HTTP Client Library API
 * is used to make a POST request to AWS IoT Core in order to publish a message
 * to a topic named "topic" with QoS=1 so that all clients subscribed to this
 * topic receive the message at least once. Any possible errors are also logged.
 *
 * @note This example is single-threaded and uses statically allocated memory.
 *
 */
int RunCoreHttpMutualAuthDemo( bool awsIotMqttMode,
                               const char * pIdentifier,
                               void * pNetworkServerInfo,
                               void * pNetworkCredentialInfo,
                               const IotNetworkInterface_t * pNetworkInterface )
{
    /* The transport layer interface used by the HTTP Client library. */
    TransportInterface_t xTransportInterface;
    /* The network context for the transport layer interface. */
    NetworkContext_t xNetworkContext = { 0 };
    TransportSocketStatus_t xNetworkStatus;
    BaseType_t xIsConnectionEstablished = pdFALSE;
    UBaseType_t uxDemoRunCount = 0UL;
    SecureSocketsTransportParams_t secureSocketsTransportParams = { 0 };

    /* Upon return, pdPASS will indicate a successful demo execution.
    * pdFAIL will indicate some failures occurred during execution. The
    * user of this demo must check the logs for any failure codes. */
    BaseType_t xDemoStatus = pdPASS;

    /* Remove compiler warnings about unused parameters. */
    ( void ) awsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;
    ( void ) pNetworkInterface;

    xNetworkContext.pParams = &secureSocketsTransportParams;

    do
    {
        /**************************** Connect. ******************************/

        /* Attempt to connect to the HTTP server. If connection fails, retry
         * after a timeout. The timeout value will be exponentially increased
         * until either the maximum number of attempts or the maximum timeout
         * value is reached. The function returns pdFAIL if the TCP connection
         * cannot be established with the broker after the configured number of
         * attempts. */
        xDemoStatus = connectToServerWithBackoffRetries( prvConnectToServer,
                                                         &xNetworkContext );

        if( xDemoStatus == pdPASS )
        {
            /* Set a flag indicating that a TLS connection exists. */
            xIsConnectionEstablished = pdTRUE;

            /* Define the transport interface. */
            xTransportInterface.pNetworkContext = &xNetworkContext;
            xTransportInterface.send = SecureSocketsTransport_Send;
            xTransportInterface.recv = SecureSocketsTransport_Recv;
        }
        else
        {
            /* Log error to indicate connection failure after all
             * reconnect attempts are over. */
            LogError( ( "Failed to connect to HTTP server %.*s.",
                        ( int32_t ) httpexampleAWS_IOT_ENDPOINT_LENGTH,
                        democonfigAWS_IOT_ENDPOINT ) );
        }

        /*********************** Send HTTP request.************************/

        if( xDemoStatus == pdPASS )
        {
            xDemoStatus = prvSendHttpRequest( &xTransportInterface,
                                              HTTP_METHOD_POST,
                                              httpexampleHTTP_METHOD_POST_LENGTH,
                                              democonfigPOST_PATH,
                                              httpexamplePOST_PATH_LENGTH );
        }

        /**************************** Disconnect. ******************************/

        /* Close the network connection to clean up any system resources that the
         * demo may have consumed. */
        if( xIsConnectionEstablished == pdTRUE )
        {
            /* Close the network connection.  */
            xNetworkStatus = SecureSocketsTransport_Disconnect( &xNetworkContext );

            if( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
            {
                xDemoStatus = pdFAIL;
                LogError( ( "SecureSocketsTransport_Disconnect() failed to close the network connection. "
                            "StatusCode=%d.", ( int ) xNetworkStatus ) );
            }
        }

        /* Increment the demo run count. */
        uxDemoRunCount++;

        if( xDemoStatus == pdPASS )
        {
            LogInfo( ( "Demo iteration %lu was successful.", uxDemoRunCount ) );
        }
        /* Attempt to retry a failed demo iteration for up to #httpexampleMAX_DEMO_COUNT times. */
        else if( uxDemoRunCount < httpexampleMAX_DEMO_COUNT )
        {
            LogWarn( ( "Demo iteration %lu failed. Retrying...", uxDemoRunCount ) );
            vTaskDelay( httpexampleDELAY_BETWEEN_DEMO_ITERATIONS_TICKS );
        }
        /* Failed all #httpexampleMAX_DEMO_COUNT demo iterations. */
        else
        {
            LogError( ( "All %d demo iterations failed.", httpexampleMAX_DEMO_COUNT ) );
            break;
        }
    } while( xDemoStatus != pdPASS );

    if( xDemoStatus == pdPASS )
    {
        LogInfo( ( "Demo completed successfully." ) );
    }

    return ( xDemoStatus == pdPASS ) ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*-----------------------------------------------------------*/

static BaseType_t prvConnectToServer( NetworkContext_t * pxNetworkContext )
{
    ServerInfo_t xServerInfo = { 0 };
    SocketsConfig_t xSocketsConfig = { 0 };
    BaseType_t xStatus = pdPASS;
    TransportSocketStatus_t xNetworkStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;

    /* Initializer server information. */
    xServerInfo.pHostName = democonfigAWS_IOT_ENDPOINT;
    xServerInfo.hostNameLength = httpexampleAWS_IOT_ENDPOINT_LENGTH;
    xServerInfo.port = democonfigAWS_HTTP_PORT;

    /* Configure credentials for TLS mutual authenticated session. */
    xSocketsConfig.enableTls = true;
    xSocketsConfig.pAlpnProtos = NULL;
    xSocketsConfig.maxFragmentLength = 0;
    xSocketsConfig.disableSni = false;
    xSocketsConfig.pRootCa = democonfigROOT_CA_PEM;
    xSocketsConfig.rootCaSize = sizeof( democonfigROOT_CA_PEM );
    xSocketsConfig.sendTimeoutMs = democonfigTRANSPORT_SEND_RECV_TIMEOUT_MS;
    xSocketsConfig.recvTimeoutMs = democonfigTRANSPORT_SEND_RECV_TIMEOUT_MS;

    /* Establish a TLS session with the HTTP server. This example connects to
     * the HTTP server as specified in democonfigAWS_IOT_ENDPOINT and
     * democonfigAWS_HTTP_PORT in http_demo_mutual_auth_config.h. */
    LogInfo( ( "Establishing a TLS session to %.*s:%d.",
               ( int32_t ) httpexampleAWS_IOT_ENDPOINT_LENGTH,
               democonfigAWS_IOT_ENDPOINT,
               democonfigAWS_HTTP_PORT ) );

    /* Attempt to create a mutually authenticated TLS connection. */
    xNetworkStatus = SecureSocketsTransport_Connect( pxNetworkContext,
                                                     &xServerInfo,
                                                     &xSocketsConfig );

    if( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
    {
        xStatus = pdFAIL;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prvSendHttpRequest( const TransportInterface_t * pxTransportInterface,
                                      const char * pcMethod,
                                      size_t xMethodLen,
                                      const char * pcPath,
                                      size_t xPathLen )
{
    /* Return value of this method. */
    BaseType_t xStatus = pdPASS;

    /* Configurations of the initial request headers that are passed to
     * #HTTPClient_InitializeRequestHeaders. */
    HTTPRequestInfo_t xRequestInfo;
    /* Represents a response returned from an HTTP server. */
    HTTPResponse_t xResponse;
    /* Represents header data that will be sent in an HTTP request. */
    HTTPRequestHeaders_t xRequestHeaders;

    /* Return value of all methods from the HTTP Client library API. */
    HTTPStatus_t xHTTPStatus = HTTPSuccess;

    configASSERT( pcMethod != NULL );
    configASSERT( pcPath != NULL );

    /* Initialize all HTTP Client library API structs to 0. */
    ( void ) memset( &xRequestInfo, 0, sizeof( xRequestInfo ) );
    ( void ) memset( &xResponse, 0, sizeof( xResponse ) );
    ( void ) memset( &xRequestHeaders, 0, sizeof( xRequestHeaders ) );

    /* Initialize the request object. */
    xRequestInfo.pHost = democonfigAWS_IOT_ENDPOINT;
    xRequestInfo.hostLen = httpexampleAWS_IOT_ENDPOINT_LENGTH;
    xRequestInfo.pMethod = pcMethod;
    xRequestInfo.methodLen = xMethodLen;
    xRequestInfo.pPath = pcPath;
    xRequestInfo.pathLen = xPathLen;

    /* Set "Connection" HTTP header to "keep-alive" so that multiple requests
     * can be sent over the same established TCP connection. */
    xRequestInfo.reqFlags = HTTP_REQUEST_KEEP_ALIVE_FLAG;

    /* Set the buffer used for storing request headers. */
    xRequestHeaders.pBuffer = ucUserBuffer;
    xRequestHeaders.bufferLen = democonfigUSER_BUFFER_LENGTH;

    xHTTPStatus = HTTPClient_InitializeRequestHeaders( &xRequestHeaders,
                                                       &xRequestInfo );

    if( xHTTPStatus == HTTPSuccess )
    {
        /* Initialize the response object. The same buffer used for storing
         * request headers is reused here. */
        xResponse.pBuffer = ucUserBuffer;
        xResponse.bufferLen = democonfigUSER_BUFFER_LENGTH;

        LogInfo( ( "Sending HTTP %.*s request to %.*s%.*s...",
                   ( int32_t ) xRequestInfo.methodLen, xRequestInfo.pMethod,
                   ( int32_t ) httpexampleAWS_IOT_ENDPOINT_LENGTH, democonfigAWS_IOT_ENDPOINT,
                   ( int32_t ) xRequestInfo.pathLen, xRequestInfo.pPath ) );
        LogDebug( ( "Request Headers:\n%.*s\n"
                    "Request Body:\n%.*s\n",
                    ( int32_t ) xRequestHeaders.headersLen,
                    ( char * ) xRequestHeaders.pBuffer,
                    ( int32_t ) httpexampleREQUEST_BODY_LENGTH, democonfigREQUEST_BODY ) );

        /* Send the request and receive the response. */
        xHTTPStatus = HTTPClient_Send( pxTransportInterface,
                                       &xRequestHeaders,
                                       ( uint8_t * ) democonfigREQUEST_BODY,
                                       httpexampleREQUEST_BODY_LENGTH,
                                       &xResponse,
                                       0 );
    }
    else
    {
        LogError( ( "Failed to initialize HTTP request headers: Error=%s.",
                    HTTPClient_strerror( xHTTPStatus ) ) );
    }

    if( xHTTPStatus == HTTPSuccess )
    {
        LogInfo( ( "Received HTTP response from %.*s%.*s...\n",
                   ( int32_t ) httpexampleAWS_IOT_ENDPOINT_LENGTH, democonfigAWS_IOT_ENDPOINT,
                   ( int32_t ) xRequestInfo.pathLen, xRequestInfo.pPath ) );
        LogDebug( ( "Response Headers:\n%.*s\n",
                    ( int32_t ) xResponse.headersLen, xResponse.pHeaders ) );
        LogDebug( ( "Status Code:\n%u\n",
                    xResponse.statusCode ) );
        LogDebug( ( "Response Body:\n%.*s\n",
                    ( int32_t ) xResponse.bodyLen, xResponse.pBody ) );
    }
    else
    {
        LogError( ( "Failed to send HTTP %.*s request to %.*s%.*s: Error=%s.",
                    ( int32_t ) xRequestInfo.methodLen, xRequestInfo.pMethod,
                    ( int32_t ) httpexampleAWS_IOT_ENDPOINT_LENGTH, democonfigAWS_IOT_ENDPOINT,
                    ( int32_t ) xRequestInfo.pathLen, xRequestInfo.pPath,
                    HTTPClient_strerror( xHTTPStatus ) ) );
    }

    if( xHTTPStatus != HTTPSuccess )
    {
        xStatus = pdFAIL;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/
