/*
 * FreeRTOS V202011.00
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

/* Include header for client credentials. */
#include "aws_clientcredential_keys.h"

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

/* Check that a TLS port for AWS IoT Core is defined. */
#ifndef democonfigAWS_HTTP_PORT
    #error "Please define democonfigAWS_HTTP_PORT."
#endif

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
    #define democonfigAWS_IOT_ENDPOINT    ""
#endif

/* Check that the Root CA certificate is defined. */
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
#define AWS_IOT_ENDPOINT_LENGTH    ( sizeof( democonfigAWS_IOT_ENDPOINT ) - 1 )

/**
 * @brief The length of the HTTP POST method.
 */
#define HTTP_METHOD_POST_LENGTH    ( sizeof( HTTP_METHOD_POST ) - 1 )

/**
 * @brief The length of the HTTP POST path.
 */
#define POST_PATH_LENGTH           ( sizeof( democonfigPOST_PATH ) - 1 )

/**
 * @brief Length of the request body.
 */
#define REQUEST_BODY_LENGTH        ( sizeof( democonfigREQUEST_BODY ) - 1 )

/**
 * @brief A buffer used in the demo for storing HTTP request headers and
 * HTTP response headers and body.
 *
 * @note This demo shows how the same buffer can be re-used for storing the HTTP
 * response after the HTTP request is sent out. However, the user can also
 * decide to use separate buffers for storing the HTTP request and response.
 */
static uint8_t userBuffer[ democonfigUSER_BUFFER_LENGTH ];

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
 * @param[in] pTransportInterface The transport interface for making network calls.
 * @param[in] pMethod The HTTP request method.
 * @param[in] methodLen The length of the HTTP request method.
 * @param[in] pPath The Request-URI to the objects of interest.
 * @param[in] pathLen The length of the Request-URI.
 *
 * @return pdFAIL on failure; pdPASS on success.
 */
static BaseType_t prvSendHttpRequest( const TransportInterface_t * pTransportInterface,
                                      const char * pMethod,
                                      size_t methodLen,
                                      const char * pPath,
                                      size_t pathLen );

/*-----------------------------------------------------------*/

/**
 * @brief Entry point of demo.
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

    /**************************** Connect. ******************************/

    /* Attempt to connect to the HTTP server. If connection fails, retry after
     * a timeout. Timeout value will be exponentially increased till the maximum
     * attempts are reached or maximum timeout value is reached. The function
     * returns EXIT_FAILURE if the TCP connection cannot be established to
     * broker after configured number of attempts. */
    xDemoStatus = connectToServerWithBackoffRetries( prvConnectToServer,
                                                     &xNetworkContext );

    if( xDemoStatus == pdPASS )
    {
        /* Set a flag indicating a TLS connection exists. This is done to
         * disconnect if the loop exits before disconnection happens. */
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
                    ( int32_t ) AWS_IOT_ENDPOINT_LENGTH,
                    democonfigAWS_IOT_ENDPOINT ) );
    }

    /*********************** Send HTTP request.************************/

    if( xDemoStatus == pdPASS )
    {
        xDemoStatus = prvSendHttpRequest( &xTransportInterface,
                                          HTTP_METHOD_POST,
                                          HTTP_METHOD_POST_LENGTH,
                                          democonfigPOST_PATH,
                                          POST_PATH_LENGTH );
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
    xServerInfo.hostNameLength = AWS_IOT_ENDPOINT_LENGTH;
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
     * democonfigAWS_HTTP_PORT in demo_config.h. */
    LogInfo( ( "Establishing a TLS session to %.*s:%d.",
               ( int32_t ) AWS_IOT_ENDPOINT_LENGTH,
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

static BaseType_t prvSendHttpRequest( const TransportInterface_t * pTransportInterface,
                                      const char * pMethod,
                                      size_t methodLen,
                                      const char * pPath,
                                      size_t pathLen )
{
    /* Return value of this method. */
    BaseType_t xStatus = pdPASS;

    /* Configurations of the initial request headers that are passed to
     * #HTTPClient_InitializeRequestHeaders. */
    HTTPRequestInfo_t requestInfo;
    /* Represents a response returned from an HTTP server. */
    HTTPResponse_t response;
    /* Represents header data that will be sent in an HTTP request. */
    HTTPRequestHeaders_t requestHeaders;

    /* Return value of all methods from the HTTP Client library API. */
    HTTPStatus_t httpStatus = HTTPSuccess;

    configASSERT( pMethod != NULL );
    configASSERT( pPath != NULL );

    /* Initialize all HTTP Client library API structs to 0. */
    ( void ) memset( &requestInfo, 0, sizeof( requestInfo ) );
    ( void ) memset( &response, 0, sizeof( response ) );
    ( void ) memset( &requestHeaders, 0, sizeof( requestHeaders ) );

    /* Initialize the request object. */
    requestInfo.pHost = democonfigAWS_IOT_ENDPOINT;
    requestInfo.hostLen = AWS_IOT_ENDPOINT_LENGTH;
    requestInfo.pMethod = pMethod;
    requestInfo.methodLen = methodLen;
    requestInfo.pPath = pPath;
    requestInfo.pathLen = pathLen;

    /* Set "Connection" HTTP header to "keep-alive" so that multiple requests
     * can be sent over the same established TCP connection. */
    requestInfo.reqFlags = HTTP_REQUEST_KEEP_ALIVE_FLAG;

    /* Set the buffer used for storing request headers. */
    requestHeaders.pBuffer = userBuffer;
    requestHeaders.bufferLen = democonfigUSER_BUFFER_LENGTH;

    httpStatus = HTTPClient_InitializeRequestHeaders( &requestHeaders,
                                                      &requestInfo );

    if( httpStatus == HTTPSuccess )
    {
        /* Initialize the response object. The same buffer used for storing
         * request headers is reused here. */
        response.pBuffer = userBuffer;
        response.bufferLen = democonfigUSER_BUFFER_LENGTH;

        LogInfo( ( "Sending HTTP %.*s request to %.*s%.*s...",
                   ( int32_t ) requestInfo.methodLen, requestInfo.pMethod,
                   ( int32_t ) AWS_IOT_ENDPOINT_LENGTH, democonfigAWS_IOT_ENDPOINT,
                   ( int32_t ) requestInfo.pathLen, requestInfo.pPath ) );
        LogDebug( ( "Request Headers:\n%.*s\n"
                    "Request Body:\n%.*s\n",
                    ( int32_t ) requestHeaders.headersLen,
                    ( char * ) requestHeaders.pBuffer,
                    ( int32_t ) REQUEST_BODY_LENGTH, democonfigREQUEST_BODY ) );

        /* Send the request and receive the response. */
        httpStatus = HTTPClient_Send( pTransportInterface,
                                      &requestHeaders,
                                      ( uint8_t * ) democonfigREQUEST_BODY,
                                      REQUEST_BODY_LENGTH,
                                      &response,
                                      0 );
    }
    else
    {
        LogError( ( "Failed to initialize HTTP request headers: Error=%s.",
                    HTTPClient_strerror( httpStatus ) ) );
    }

    if( httpStatus == HTTPSuccess )
    {
        LogInfo( ( "Received HTTP response from %.*s%.*s...\n",
                   ( int32_t ) AWS_IOT_ENDPOINT_LENGTH, democonfigAWS_IOT_ENDPOINT,
                   ( int32_t ) requestInfo.pathLen, requestInfo.pPath ) );
        LogDebug( ( "Response Headers:\n%.*s\n",
                    ( int32_t ) response.headersLen, response.pHeaders ) );
        LogDebug( ( "Status Code:\n%u\n",
                    response.statusCode ) );
        LogDebug( ( "Response Body:\n%.*s\n",
                    ( int32_t ) response.bodyLen, response.pBody ) );
    }
    else
    {
        LogError( ( "Failed to send HTTP %.*s request to %.*s%.*s: Error=%s.",
                    ( int32_t ) requestInfo.methodLen, requestInfo.pMethod,
                    ( int32_t ) AWS_IOT_ENDPOINT_LENGTH, democonfigAWS_IOT_ENDPOINT,
                    ( int32_t ) requestInfo.pathLen, requestInfo.pPath,
                    HTTPClient_strerror( httpStatus ) ) );
    }

    if( httpStatus != HTTPSuccess )
    {
        xStatus = pdFAIL;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/
