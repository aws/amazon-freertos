/*
 * FreeRTOS V202012.00
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
 * Demo for showing use of the HTTP API using a server-authenticated network
 * connection.
 *
 * This example, using a pre-signed URL, resolves a S3 domain, establishes a TCP
 * connection, validates the server's certificate using the root CA certificate
 * defined in the config header, then finally performs a TLS handshake with the
 * HTTP server so that all communication is encrypted.
 *
 * Afterwards, two thread-safe queues are created -- a request and response
 * queue -- to be shared among two tasks, the main task and the HTTP task. The
 * main task adds HTTP request headers into the request queue, for the HTTP task
 * to retrieve and send to the server using the HTTP Client library API. The
 * HTTP task then places the server's response into the response queue, which
 * the main task parses and evaluates. The requests created by the main task are
 * range requests, used to download the S3 file in chunks. The main task reads
 * responses from the response queue continuously until the entire file is
 * received. If any request fails, an error code is returned.
 *
 * @note This demo requires user-generated pre-signed URLs to be pasted into
 * http_s3_download_multithreaded_demo_config.h. Please use the provided script
 * "presigned_urls_gen.py" (located in http_demo_helpers) to generate these
 * URLs. For detailed instructions, see the accompanied README.md.
 *
 * @note If your file requires more than 99 range requests to S3 (depending on the
 * size of the file and the length specified in democonfigRANGE_REQUEST_LENGTH),
 * your connection may be dropped by S3. In this case, either increase the
 * buffer size and range request length (if feasible), to reduce the number of
 * requests required, or re-establish the connection with S3 after receiving a
 * "Connection: close" response header.
 *
 * @note This demo uses retry logic to connect to the server if connection
 * attempts fail. The FreeRTOS/backoffAlgorithm library is used to calculate the
 * retry interval with an exponential backoff and jitter algorithm. For
 * generating random number required by the algorithm, the PKCS11 module is used
 * as it allows access to a True Random Number Generator (TRNG) if the vendor
 * platform supports it.
 * It is RECOMMENDED to seed the random number generator with a device-specific
 * entropy source so that probability of collisions from devices in connection
 * retries is mitigated.
 */

/**
 * @file http_demo_s3_download_multithreaded.c
 * @brief Demonstrates usage of the HTTP library.
 */

/* Standard includes. */
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Include demo config as the first non-system header. */
#include "http_demo_s3_download_multithreaded_config.h"

/* Include common demo header. */
#include "aws_demo.h"

/* Transport interface implementation include header for TLS. */
#include "transport_secure_sockets.h"

/* Common HTTP demo utilities. */
#include "http_demo_utils.h"

/* HTTP API header. */
#include "core_http_client.h"

/*------------- Demo configurations -------------------------*/

/* Check that the TLS port of the server is defined. */
#ifndef democonfigHTTPS_PORT
    #error "Please define a democonfigHTTPS_PORT in http_demo_s3_download_multithreaded_config.h."
#endif

/* Check that the root CA certificate is defined. */
#ifndef democonfigROOT_CA_PEM
    #error "Please define a democonfigROOT_CA_PEM in http_demo_s3_download_multithreaded_config.h."
#endif

/* Check the the queue size is defined. */
#ifndef democonfigQUEUE_SIZE
    #error "Please define a QUEUE_SIZE in http_demo_s3_download_multithreaded_config.h."
#endif

/* The default value of the pre-signed GET URL macro. */
#ifndef democonfigS3_PRESIGNED_GET_URL
    #define democonfigS3_PRESIGNED_GET_URL    "GET-URL"
#endif

/* The default value for the timeout of the transport send and receive
 * functions. */
#ifndef democonfigTRANSPORT_SEND_RECV_TIMEOUT_MS
    #define democonfigTRANSPORT_SEND_RECV_TIMEOUT_MS    ( 5000 )
#endif

/* The default value for the size of the user buffer. */
#ifndef democonfigUSER_BUFFER_LENGTH
    #define democonfigUSER_BUFFER_LENGTH    ( 2048 )
#endif

/* The default value for the range request length. */
#ifndef democonfigRANGE_REQUEST_LENGTH
    #define democonfigRANGE_REQUEST_LENGTH    ( 1024 )
#endif

/* The default value for the stack size to use for HTTP tasks. */
#ifndef httpexampleTASK_STACK_SIZE
    #define httpexampleTASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 4 )
#endif

/**
 * @brief The default value for the maximum number of times to run the loop in
 * this demo.
 */
#ifndef httpexampleMAX_DEMO_COUNT
    #define httpexampleMAX_DEMO_COUNT    ( 3 )
#endif

/**
 * @brief Length of the pre-signed GET URL defined in http_demo_s3_download_multithreaded_config.h.
 */
#define httpexampleS3_PRESIGNED_GET_URL_LENGTH               ( sizeof( democonfigS3_PRESIGNED_GET_URL ) - 1 )

/**
 * @brief The length of the HTTP GET method.
 */
#define httpexampleHTTP_METHOD_GET_LENGTH                    ( sizeof( HTTP_METHOD_GET ) - 1 )

/**
 * @brief Field name of the HTTP range header to read from server response.
 */
#define httpexampleHTTP_CONTENT_RANGE_HEADER_FIELD           "Content-Range"

/**
 * @brief Length of the HTTP range header field.
 */
#define httpexampleHTTP_CONTENT_RANGE_HEADER_FIELD_LENGTH    ( sizeof( httpexampleHTTP_CONTENT_RANGE_HEADER_FIELD ) - 1 )

/**
 * @brief The HTTP status code returned for partial content.
 */
#define httpexampleHTTP_STATUS_CODE_PARTIAL_CONTENT          206

/**
 * @brief Time in ticks to wait between each iteration of
 * RunCoreHttpS3DownloadMultithreadedDemo().
 */
#define httpexampleDELAY_BETWEEN_DEMO_ITERATIONS_TICKS       ( pdMS_TO_TICKS( 5000U ) )

/**
 * @brief Ticks to wait for task notifications.
 */
#define httpexampleDEMO_TICKS_TO_WAIT                        pdMS_TO_TICKS( 1000 )

/**
 * @brief Notification bit indicating HTTPClient_Send() error in HTTP task.
 */
#define httpexampleHTTP_SEND_ERROR                           ( 1U << 1 )

/**
 * @brief The maximum number of loop iterations to wait after the last received
 * server response, before declaring failure.
 */
#define httpexampleMAX_WAIT_ITERATIONS                       ( 10 )

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
 * @brief Represents the network context used for the TLS session with the
 * server.
 */
static NetworkContext_t xNetworkContext;

/**
 * @brief The host address string extracted from the pre-signed URL.
 *
 * @note httpexampleS3_PRESIGNED_GET_URL_LENGTH is set as the array length here
 * as the length of the host name string cannot exceed this value.
 */
static char cServerHost[ httpexampleS3_PRESIGNED_GET_URL_LENGTH ];

/**
 * @brief The length of the host address found in the pre-signed URL.
 */
static size_t xServerHostLength;

/**
 * @brief Data type for the request queue.
 *
 * Contains the request header struct and its corresponding buffer, to be
 * populated and enqueued by the main task, and read by the HTTP task. The
 * buffer is included to avoid pointer inaccuracy during queue copy operations.
 */
typedef struct RequestItem
{
    HTTPRequestHeaders_t xRequestHeaders;
    uint8_t ucHeaderBuffer[ democonfigUSER_BUFFER_LENGTH ];
} RequestItem_t;

/**
 * @brief Data type for the response queue.
 *
 * Contains the response data type and its corresponding buffer, to be enqueued
 * by the HTTP task, and interpreted by the main task. The buffer is included to
 * avoid pointer inaccuracy during queue copy operations.
 */
typedef struct ResponseItem
{
    HTTPResponse_t xResponse;
    uint8_t ucResponseBuffer[ democonfigUSER_BUFFER_LENGTH ];
} ResponseItem_t;

/**
 * @brief Struct used by the main task to add requests to the request queue.
 *
 * This structure is modified only by the main task. Since queue operations are
 * done by-copy, it is safe for the main task to modify this struct once the
 * previous request has been successfully enqueued.
 */
static RequestItem_t xRequestItem = { 0 };

/**
 * @brief Struct used by the main task to receive responses from the response
 * queue.
 *
 * This structure is modified only by the main task. Since queue operations are
 * done by-copy, it is safe for the main task to modify this struct once the
 * previous response has been parsed.
 */
static ResponseItem_t xResponseItem = { 0 };

/**
 * @brief Struct used by the HTTP task to send requests to the server.
 *
 * This structure is modified only by the HTTP task, and is used to receive
 * requests off of the request queue and send them to the HTTP server. Since
 * queue operations are done by-copy, it is safe for the HTTP task to modify
 * this struct once the previous request has been sent to the server.
 */
static RequestItem_t xHTTPRequestItem = { 0 };

/**
 * @brief Struct used by the HTTP task to receive responses from the server and
 * place them on the response queue.
 *
 * This structure is modified only by the HTTP task. Since queue operations are
 * done by-copy, it is safe for the HTTP task to modify this struct once the
 * previous response has been successfully enqueued.
 */
static ResponseItem_t xHTTPResponseItem = { 0 };

/**
 * @brief Queue for HTTP requests. Requests are written by the main task, and
 * executed by the HTTP task.
 */
static QueueHandle_t xRequestQueue;

/**
 * @brief Queue for HTTP responses. Responses are written by the HTTP task,
 * and read by the main task.
 */
static QueueHandle_t xResponseQueue;

/**
 * @brief Handle of prvStartHTTPTask.
 */
static TaskHandle_t xHTTPTask;

/**
 * @brief Handle for the main task.
 */
static TaskHandle_t xMainTask;

/*-----------------------------------------------------------*/

/**
 * @brief Connect to HTTP server with reconnection retries.
 *
 * @param[out] pxNetworkContext The output parameter to return the created
 * network context.
 *
 * @return pdFAIL on failure; pdPASS on successful connection.
 */
static BaseType_t prvConnectToServer( NetworkContext_t * pxNetworkContext );

/**
 * @brief Send continuous range requests until the entire S3 file is downloaded,
 * and log the corresponding responses received from the server.
 *
 * @param[in] pcHost The host name of the server.
 * @param[in] xHostLen The length of pcHost.
 * @param[in] pcRequest The HTTP Request-URI.
 * @param[in] xRequestUriLen The length of pcRequest.
 * @param[in] xRequestQueue The queue to which HTTP requests should be written.
 * @param[in] xResponseQueue The queue from which HTTP responses should be read.
 *
 * @return pdFAIL on failure; pdPASS on success.
 */
static BaseType_t prvDownloadS3ObjectFile( const char * pcHost,
                                           const size_t xHostLen,
                                           const char * pcRequest,
                                           const size_t xRequestUriLen,
                                           QueueHandle_t xRequestQueue,
                                           QueueHandle_t xResponseQueue );

/**
 * @brief Enqueue an HTTP GET request for a given range of the S3 file.
 *
 * @param[in] pxRequestInfo The #HTTPRequestInfo_t for configuring the request.
 * @param[in] xRequestQueue The queue to which HTTP requests should be written.
 * @param[in] xStart The position of the first byte in the range.
 * @param[in] xEnd The position of the last byte in the range, inclusive.
 *
 * @return pdFAIL on failure; pdPASS on success.
 */
static BaseType_t prvRequestS3ObjectRange( const HTTPRequestInfo_t * pxRequestInfo,
                                           QueueHandle_t xRequestQueue,
                                           const size_t xStart,
                                           const size_t xEnd );

/**
 * @brief Check for an HTTP task notification.
 *
 * @param[in] pulNotification pointer holding notification value.
 * @param[in] ulExpectedBits Bits to wait for.
 * @param[in] xClearBits If bits should be cleared.
 *
 * @return `pdTRUE` if notification received, `pdFALSE` otherwise.
 */
static BaseType_t prvCheckNotification( uint32_t * pulNotification,
                                        uint32_t ulExpectedBits,
                                        BaseType_t xClearBits );

/**
 * @brief Retrieve the size of the S3 object that is specified in pcPath using
 * HTTP task.
 *
 * @param[in] pxRequestInfo The #HTTPRequestInfo_t for configuring the request.
 * @param[in] xRequestQueue The queue to which HTTP requests should be written.
 * @param[in] xResponseQueue The queue from which HTTP responses should be read.
 * @param[out] pxFileSize - The size of the S3 object.
 *
 * @return pdFAIL on failure; pdPASS on success.
 */
static BaseType_t prvGetS3ObjectFileSize( const HTTPRequestInfo_t * pxRequestInfo,
                                          QueueHandle_t xRequestQueue,
                                          QueueHandle_t xResponseQueue,
                                          size_t * pxFileSize );

/**
 * @brief Services HTTP requests from the request queue and writes responses to
 * the response queue.
 *
 * @param[in] pvArgs Parameters as passed at the time of task creation. Not used
 * in this example.
 * */
static void prvStartHTTPTask( void * pvArgs );

/**
 * @brief Clean up resources created by demo.
 *
 * @param[in] xHandle The HTTP task handle.
 * @param[in] xRequestQueue The request queue.
 * @param[in] xResponseQueue The response queue.
 */
static void prvTearDown( TaskHandle_t xHandle,
                         QueueHandle_t xRequestQueue,
                         QueueHandle_t xResponseQueue );

/*-----------------------------------------------------------*/

static BaseType_t prvConnectToServer( NetworkContext_t * pxNetworkContext )
{
    ServerInfo_t xServerInfo = { 0 };
    SocketsConfig_t xSocketsConfig = { 0 };
    BaseType_t xStatus = pdPASS;
    TransportSocketStatus_t xNetworkStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;

    configASSERT( pxNetworkContext != NULL );

    /* Initializer server information. */
    xServerInfo.pHostName = cServerHost;
    xServerInfo.hostNameLength = xServerHostLength;
    xServerInfo.port = democonfigHTTPS_PORT;

    /* Configure credentials for TLS server-authenticated session. */
    xSocketsConfig.enableTls = true;
    xSocketsConfig.pAlpnProtos = NULL;
    xSocketsConfig.maxFragmentLength = 0;
    xSocketsConfig.disableSni = false;
    xSocketsConfig.pRootCa = democonfigROOT_CA_PEM;
    xSocketsConfig.rootCaSize = sizeof( democonfigROOT_CA_PEM );
    xSocketsConfig.sendTimeoutMs = democonfigTRANSPORT_SEND_RECV_TIMEOUT_MS;
    xSocketsConfig.recvTimeoutMs = democonfigTRANSPORT_SEND_RECV_TIMEOUT_MS;

    /* Establish a TLS session with the HTTP server. This example connects
     * to the server host located in democonfigPRESIGNED_GET_URL and
     * democonfigHTTPS_PORT in http_s3_download_multithreaded_demo_config.h. */
    LogInfo( ( "Establishing a TLS session with %s:%d.",
               cServerHost,
               democonfigHTTPS_PORT ) );

    /* Attempt to create a server-authenticated TLS connection. */
    xNetworkStatus = SecureSocketsTransport_Connect( pxNetworkContext,
                                                     &xServerInfo,
                                                     &xSocketsConfig );

    if( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
    {
        LogWarn( ( "Unsuccessful connection attempt, received error code:%d",
                   ( int ) xNetworkStatus ) );
        xStatus = pdFAIL;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prvDownloadS3ObjectFile( const char * pcHost,
                                           const size_t xHostLen,
                                           const char * pcRequest,
                                           const size_t xRequestUriLen,
                                           QueueHandle_t xRequestQueue,
                                           QueueHandle_t xResponseQueue )
{
    BaseType_t xStatus = pdPASS;
    size_t xRemainingResponseCount = 0;
    uint32_t ulWaitCounter = 0;
    uint32_t ulNotification = 0;

    /* Configurations of the initial request headers. */
    HTTPRequestInfo_t xRequestInfo = { 0 };

    /* The length of the file at democonfigS3_PRESIGNED_GET_URL. */
    size_t xFileSize = 0;

    /* The number of bytes we want to request within each range of the file. */
    size_t xNumReqBytes = 0;
    /* The starting byte for the next range request. */
    size_t xCurByte = 0;

    configASSERT( pcHost != NULL );
    configASSERT( pcRequest != NULL );

    /* Initialize the request object. */
    xRequestInfo.pHost = pcHost;
    xRequestInfo.hostLen = xHostLen;
    xRequestInfo.pMethod = HTTP_METHOD_GET;
    xRequestInfo.methodLen = httpexampleHTTP_METHOD_GET_LENGTH;
    xRequestInfo.pPath = pcRequest;
    xRequestInfo.pathLen = xRequestUriLen;

    /* Set "Connection" HTTP header to "keep-alive" so that multiple requests
     * can be sent over the same established TCP connection. This is done in
     * order to download the file in parts. */
    xRequestInfo.reqFlags = HTTP_REQUEST_KEEP_ALIVE_FLAG;

    /* Get the length of the S3 file. */
    xStatus = prvGetS3ObjectFileSize( &xRequestInfo,
                                      xRequestQueue,
                                      xResponseQueue,
                                      &xFileSize );

    /* Set the number of bytes to request in each iteration, defined by the user
     * in democonfigRANGE_REQUEST_LENGTH. */
    if( xFileSize < democonfigRANGE_REQUEST_LENGTH )
    {
        xNumReqBytes = xFileSize;
    }
    else
    {
        xNumReqBytes = democonfigRANGE_REQUEST_LENGTH;
    }

    /* Here we iterate sending byte range requests to the request queue and
     * retrieving responses from the response queue until the entire file has
     * been downloaded. We keep track of the next starting byte to download with
     * xCurByte, and increment by xNumReqBytes after each iteration. When
     * xCurByte reaches xFileSize, we stop downloading. We keep track of the
     * number of responses we are waiting for with xRemainingResponseCount.
     */
    while( ( xStatus != pdFAIL ) && ( xCurByte < xFileSize || xRemainingResponseCount > 0 ) )
    {
        /* Send a range request for the specified bytes, if remaining. */
        if( xCurByte < xFileSize )
        {
            /* Add range request to the request queue. */
            xStatus = prvRequestS3ObjectRange( &xRequestInfo,
                                               xRequestQueue,
                                               xCurByte,
                                               xCurByte + xNumReqBytes - 1 );

            /* Exit loop if the request was not successfully enqueued. */
            if( xStatus != pdPASS )
            {
                break;
            }

            /* Update the starting byte for the next iteration.*/
            xCurByte += xNumReqBytes;

            /* If the number of bytes left to download is less than the
             * pre-defined constant xNumReqBytes, set xNumReqBytes to equal the
             * accurate number of remaining bytes left to download. */
            if( ( xFileSize - xCurByte ) < xNumReqBytes )
            {
                xNumReqBytes = xFileSize - xCurByte;
            }

            /* If the request was successfully enqueued, we expect a
             * corresponding response. */
            xRemainingResponseCount += 1;
        }

        /* Retrieve response from the response queue, if available. */
        if( xRemainingResponseCount > 0 )
        {
            if( xQueueReceive( xResponseQueue, &xResponseItem, httpexampleDEMO_TICKS_TO_WAIT ) != pdFAIL )
            {
                /* Ensure that the buffer pointer is accurate after being copied from the queue. */
                xResponseItem.xResponse.pBuffer = xResponseItem.ucResponseBuffer;

                LogInfo( ( "The main task retrieved a server response from the response queue." ) );
                LogDebug( ( "Response Headers:\n%.*s",
                            ( int32_t ) xResponseItem.xResponse.headersLen,
                            xResponseItem.xResponse.pHeaders ) );
                LogDebug( ( "Response Status:\n%u",
                            xResponseItem.xResponse.statusCode ) );
                LogInfo( ( "Response Body:\n%.*s\n",
                           ( int32_t ) xResponseItem.xResponse.bodyLen,
                           xResponseItem.xResponse.pBody ) );

                /* Check for a partial content status code (206), indicating a
                 * successful server response. */
                if( xResponseItem.xResponse.statusCode != httpexampleHTTP_STATUS_CODE_PARTIAL_CONTENT )
                {
                    LogError( ( "Received response with unexpected status code: %d", xResponseItem.xResponse.statusCode ) );
                    xStatus = pdFAIL;
                    break;
                }

                /* Reset the wait counter every time a response is received. */
                ulWaitCounter = 0;
                xRemainingResponseCount -= 1;
            }
            /* Check for a notification from the HTTP task about an HTTP send failure. */
            else if( prvCheckNotification( &ulNotification, httpexampleHTTP_SEND_ERROR, pdTRUE ) != pdFALSE )
            {
                LogError( ( "Received notification from the HTTP task indicating a HTTPClient_Send() error." ) );
                xStatus = pdFAIL;
                break;
            }
        }

        /* Break if we have been stuck waiting for a response for too long. The
         * total wait here will be the (notification check delay + queue check
         * delay), multiplied by `httpexampleMAX_WAIT_ITERATIONS`. For example,
         * with a 1000 ms delay for both checks, and a maximum iteration of 10,
         * this function will wait 20 seconds after receiving the last response
         * before exiting the loop. */
        if( ++ulWaitCounter > httpexampleMAX_WAIT_ITERATIONS )
        {
            LogError( ( "Response receive loop exceeded maximum wait time.\n" ) );
            xStatus = pdFAIL;
            break;
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prvRequestS3ObjectRange( const HTTPRequestInfo_t * pxRequestInfo,
                                           QueueHandle_t xRequestQueue,
                                           const size_t xStart,
                                           const size_t xEnd )
{
    HTTPStatus_t xHTTPStatus = HTTPSuccess;
    BaseType_t xStatus = pdPASS;

    configASSERT( pxRequestInfo != NULL );

    /* Set the buffer used for storing request headers. */
    xRequestItem.xRequestHeaders.pBuffer = xRequestItem.ucHeaderBuffer;
    xRequestItem.xRequestHeaders.bufferLen = democonfigUSER_BUFFER_LENGTH;

    xHTTPStatus = HTTPClient_InitializeRequestHeaders( &( xRequestItem.xRequestHeaders ),
                                                       pxRequestInfo );

    if( xHTTPStatus != HTTPSuccess )
    {
        LogError( ( "Failed to initialize HTTP request headers: Error=%s.",
                    HTTPClient_strerror( xHTTPStatus ) ) );
        xStatus = pdFAIL;
    }

    if( xStatus == pdPASS )
    {
        xHTTPStatus = HTTPClient_AddRangeHeader( &( xRequestItem.xRequestHeaders ),
                                                 xStart,
                                                 xEnd );

        if( xHTTPStatus != HTTPSuccess )
        {
            LogError( ( "Failed to add Range header to request headers: Error=%s.",
                        HTTPClient_strerror( xHTTPStatus ) ) );
            xStatus = pdFAIL;
        }
    }

    if( xStatus == pdPASS )
    {
        /* Enqueue the request. */
        LogInfo( ( "Enqueuing request for bytes %d to %d of S3 Object. ",
                   ( int32_t ) xStart,
                   ( int32_t ) xEnd ) );
        LogDebug( ( "Request Headers:\n%.*s",
                    ( int32_t ) xRequestItem.xRequestHeaders.headersLen,
                    ( char * ) xRequestItem.xRequestHeaders.pBuffer ) );

        xStatus = xQueueSendToBack( xRequestQueue,
                                    &xRequestItem,
                                    httpexampleDEMO_TICKS_TO_WAIT );

        /* Ensure request was added to the queue. */
        if( xStatus == pdFAIL )
        {
            LogError( ( "Could not enqueue request." ) );
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prvCheckNotification( uint32_t * pulNotification,
                                        uint32_t ulExpectedBits,
                                        BaseType_t xClearBits )
{
    BaseType_t xStatus = pdTRUE;

    configASSERT( pulNotification != NULL );

    xStatus = xTaskNotifyWait( 0,
                               ( xClearBits ) ? ulExpectedBits : 0,
                               pulNotification,
                               httpexampleDEMO_TICKS_TO_WAIT );

    if( xStatus == pdTRUE )
    {
        xStatus = ( ( *pulNotification & ulExpectedBits ) == ulExpectedBits ) ? pdTRUE : pdFALSE;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prvGetS3ObjectFileSize( const HTTPRequestInfo_t * pxRequestInfo,
                                          QueueHandle_t xRequestQueue,
                                          QueueHandle_t xResponseQueue,
                                          size_t * pxFileSize )
{
    BaseType_t xStatus = pdPASS;
    HTTPStatus_t xHTTPStatus = HTTPSuccess;
    uint32_t ulNotification = 0;

    /* The location of the file size in pcContentRangeValStr. */
    char * pcFileSizeStr = NULL;

    /* String to store the Content-Range header value. */
    char * pcContentRangeValStr = NULL;
    size_t xContentRangeValStrLength = 0;

    configASSERT( pxRequestInfo != NULL );
    configASSERT( pxFileSize != NULL );

    LogInfo( ( "Getting file object size from host..." ) );

    /* Request bytes 0 to 0. S3 will respond with a Content-Range
     * header that contains the size of the file in it. This header will look
     * like: "Content-Range: bytes 0-0/FILESIZE". The body will have a single
     * byte that we are ignoring. */
    xStatus = prvRequestS3ObjectRange( pxRequestInfo,
                                       xRequestQueue,
                                       0,
                                       0 );

    if( xStatus == pdPASS )
    {
        xStatus = xQueueReceive( xResponseQueue, &xResponseItem, httpexampleDEMO_TICKS_TO_WAIT );
    }

    if( xStatus == pdPASS )
    {
        /* Ensure that the buffer pointer is accurate after being copied from the queue. */
        xResponseItem.xResponse.pBuffer = xResponseItem.ucResponseBuffer;

        if( xResponseItem.xResponse.statusCode != httpexampleHTTP_STATUS_CODE_PARTIAL_CONTENT )
        {
            LogError( ( "Received response with unexpected status code: %d.", xResponseItem.xResponse.statusCode ) );
            xStatus = pdFAIL;
        }
        else
        {
            xHTTPStatus = HTTPClient_ReadHeader( &xResponseItem.xResponse,
                                                 ( char * ) httpexampleHTTP_CONTENT_RANGE_HEADER_FIELD,
                                                 ( size_t ) httpexampleHTTP_CONTENT_RANGE_HEADER_FIELD_LENGTH,
                                                 ( const char ** ) &pcContentRangeValStr,
                                                 &xContentRangeValStrLength );

            if( xHTTPStatus != HTTPSuccess )
            {
                LogError( ( "Failed to read Content-Range header from HTTP response: Error=%s.",
                            HTTPClient_strerror( xHTTPStatus ) ) );
                xStatus = pdFAIL;
            }
        }
    }
    /* Check for a notification from the HTTP task about an HTTP send failure. */
    else if( prvCheckNotification( &ulNotification, httpexampleHTTP_SEND_ERROR, pdTRUE ) != pdFALSE )
    {
        LogError( ( "Received notification from the HTTP task indicating a HTTPClient_Send() error." ) );
    }

    /* Parse the Content-Range header value to get the file size. */
    if( xStatus == pdPASS )
    {
        pcFileSizeStr = strstr( pcContentRangeValStr, "/" );

        if( pcFileSizeStr == NULL )
        {
            LogError( ( "'/' not present in Content-Range header value: %s.",
                        pcContentRangeValStr ) );
            xStatus = pdFAIL;
        }
    }

    if( xStatus == pdPASS )
    {
        pcFileSizeStr += sizeof( char );
        *pxFileSize = ( size_t ) strtoul( pcFileSizeStr, NULL, 10 );

        if( ( *pxFileSize == 0 ) || ( *pxFileSize == UINT32_MAX ) )
        {
            LogError( ( "Error using strtoul to get the file size from %s: xFileSize=%d.",
                        pcFileSizeStr, ( int32_t ) *pxFileSize ) );
            xStatus = pdFAIL;
        }
    }

    if( xStatus == pdPASS )
    {
        LogInfo( ( "The file is %d bytes long.", ( int32_t ) *pxFileSize ) );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static void prvStartHTTPTask( void * pvArgs )
{
    HTTPStatus_t xHTTPStatus = HTTPSuccess;
    BaseType_t xStatus = pdPASS;
    /* The transport layer interface used by the HTTP Client library. */
    TransportInterface_t xTransportInterface;

    ( void ) pvArgs;

    /* Define the transport interface. */
    xTransportInterface.pNetworkContext = &xNetworkContext;
    xTransportInterface.send = SecureSocketsTransport_Send;
    xTransportInterface.recv = SecureSocketsTransport_Recv;

    /* Initialize response struct. */
    xHTTPResponseItem.xResponse.pBuffer = xHTTPResponseItem.ucResponseBuffer;
    xHTTPResponseItem.xResponse.bufferLen = democonfigUSER_BUFFER_LENGTH;

    for( ; ; )
    {
        /* Read request from queue. */
        xStatus = xQueueReceive( xRequestQueue,
                                 &xHTTPRequestItem,
                                 httpexampleDEMO_TICKS_TO_WAIT );

        if( xStatus == pdFAIL )
        {
            LogInfo( ( "No requests in the queue. Trying again." ) );
            continue;
        }

        /* Ensure that the buffer pointer is accurate after being copied from the queue. */
        xHTTPRequestItem.xRequestHeaders.pBuffer = xHTTPRequestItem.ucHeaderBuffer;

        LogInfo( ( "The HTTP task retrieved a request from the request queue." ) );
        LogDebug( ( "Request Headers:\n%.*s",
                    ( int32_t ) xHTTPRequestItem.xRequestHeaders.headersLen,
                    ( char * ) xHTTPRequestItem.xRequestHeaders.pBuffer ) );

        xHTTPStatus = HTTPClient_Send( &xTransportInterface,
                                       &xHTTPRequestItem.xRequestHeaders,
                                       NULL,
                                       0,
                                       &xHTTPResponseItem.xResponse,
                                       0 );

        if( xHTTPStatus != HTTPSuccess )
        {
            LogError( ( "Failed to send HTTP request: Error=%s.",
                        HTTPClient_strerror( xHTTPStatus ) ) );
            /*Notify the main task of failure. */
            xTaskNotify( xMainTask, httpexampleHTTP_SEND_ERROR, eSetBits );
            break;
        }
        else
        {
            LogInfo( ( "The HTTP task received a response from the server." ) );
            /* Write response to queue. */
            xStatus = xQueueSendToBack( xResponseQueue,
                                        &xHTTPResponseItem,
                                        httpexampleDEMO_TICKS_TO_WAIT );

            /* Ensure response was added to the queue. */
            if( xStatus != pdPASS )
            {
                LogError( ( "Could not enqueue response." ) );
                break;
            }
        }
    }
}

/*-----------------------------------------------------------*/

void prvTearDown( TaskHandle_t xHandle,
                  QueueHandle_t xRequestQueue,
                  QueueHandle_t xResponseQueue )
{
    /* Delete HTTP task. */
    LogInfo( ( "Deleting HTTP task." ) );
    vTaskDelete( xHandle );

    /* Close and delete the queues. */
    if( xRequestQueue != NULL )
    {
        vQueueDelete( xRequestQueue );
    }

    if( xResponseQueue != NULL )
    {
        vQueueDelete( xResponseQueue );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Entry point of the demo.
 *
 * This example, using a pre-signed URL, resolves a S3 domain, establishes a TCP
 * connection, validates the server's certificate using the root CA certificate
 * defined in the config header, then finally performs a TLS handshake with the
 * HTTP server so that all communication is encrypted.
 *
 * Afterwards, an independent HTTP task is started, to read requests from the
 * request queue and execute them using the HTTP Client library API, and write
 * the corresponding responses to the response queue. The main task sends
 * requests to the request queue, which are used to download the S3 file in
 * chunks using range requests. While doing so, the main task reads responses
 * from the response queue continuously until the entire file is received. If
 * any request fails, an error code is returned.
 *
 * @note This example is multi-threaded and uses statically allocated memory.
 *
 */
int RunCoreHttpS3DownloadMultithreadedDemo( bool awsIotMqttMode,
                                            const char * pIdentifier,
                                            void * pNetworkServerInfo,
                                            void * pNetworkCredentialInfo,
                                            const IotNetworkInterface_t * pNetworkInterface )
{
    TransportSocketStatus_t xNetworkStatus;
    BaseType_t xIsConnectionEstablished = pdFALSE;
    /* HTTPS Client library return status. */
    HTTPStatus_t xHTTPStatus = HTTPSuccess;
    /* The location of the host address within the pre-signed URL. */
    const char * pcAddress = NULL;
    /* The location of the path within the pre-signed URL. */
    const char * pcPath = NULL;
    UBaseType_t uxDemoRunCount = 0UL;

    /* Upon return, pdPASS will indicate a successful demo execution. pdFAIL
     * will indicate some failures occurred during execution. The user of this
     * demo must check the logs for any failure codes. */
    BaseType_t xDemoStatus = pdPASS;

    /* The length of the path within the pre-signed URL. This variable is
     * defined in order to store the length returned from parsing the URL, but
     * it is unused. The path used for the requests in this demo needs all the
     * query information following the location of the object, to the end of the
     * S3 presigned URL. */
    size_t pathLen = 0;
    /* The length of the Request-URI within string S3_PRESIGNED_GET_URL */
    size_t xRequestUriLen = 0;

    SecureSocketsTransportParams_t secureSocketsTransportParams = { 0 };

    /* Remove compiler warnings about unused parameters. */
    ( void ) awsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;
    ( void ) pNetworkInterface;

    xNetworkContext.pParams = &secureSocketsTransportParams;

    xMainTask = xTaskGetCurrentTaskHandle();

    LogInfo( ( "HTTP Client multi-threaded S3 download demo using pre-signed URL:\n%s", democonfigS3_PRESIGNED_GET_URL ) );

    do
    {
        /* Reset for each iteration of the demo. */
        xDemoStatus = pdPASS;

        /**************************** Parse Signed URL. ******************************/
        if( xDemoStatus == pdPASS )
        {
            /* Retrieve the path location from democonfigS3_PRESIGNED_GET_URL. This
             * function returns the length of the path without the query into
             * pathLen. */
            xHTTPStatus = getUrlPath( democonfigS3_PRESIGNED_GET_URL,
                                      httpexampleS3_PRESIGNED_GET_URL_LENGTH,
                                      &pcPath,
                                      &pathLen );

            /* The path used for the requests in this demo needs
             * all the query information following the location of the object, to
             * the end of the S3 presigned URL. */
            xRequestUriLen = strlen( pcPath );

            xDemoStatus = ( xHTTPStatus == HTTPSuccess ) ? pdPASS : pdFAIL;
        }

        if( xDemoStatus == pdPASS )
        {
            /* Retrieve the address location and length from the democonfigS3_PRESIGNED_GET_URL. */
            xHTTPStatus = getUrlAddress( democonfigS3_PRESIGNED_GET_URL,
                                         httpexampleS3_PRESIGNED_GET_URL_LENGTH,
                                         &pcAddress,
                                         &xServerHostLength );

            xDemoStatus = ( xHTTPStatus == HTTPSuccess ) ? pdPASS : pdFAIL;
        }

        if( xDemoStatus == pdPASS )
        {
            /* cServerHost should consist only of the host address located in
             * democonfigS3_PRESIGNED_GET_URL. */
            memcpy( cServerHost, pcAddress, xServerHostLength );
            cServerHost[ xServerHostLength ] = '\0';
        }

        /**************************** Connect. ******************************/

        /* Establish a TLS connection on top of TCP connection using OpenSSL. */
        if( xDemoStatus == pdPASS )
        {
            /* Attempt to connect to the HTTP server. If connection fails, retry
             * after a timeout. The timeout value will be exponentially
             * increased until either the maximum number of attempts or the
             * maximum timeout value is reached. The function returns pdFAIL if
             * the TCP connection cannot be established with the server after
             * the configured number of attempts. */
            xDemoStatus = connectToServerWithBackoffRetries( prvConnectToServer,
                                                             &xNetworkContext );

            if( xDemoStatus == pdFAIL )
            {
                /* Log an error to indicate connection failure after all
                 * reconnect attempts are over. */
                LogError( ( "Failed to connect to HTTP server %s.",
                            cServerHost ) );
            }
        }

        /* Define the transport interface. */
        if( xDemoStatus == pdPASS )
        {
            /* Set a flag indicating that a TLS connection exists. */
            xIsConnectionEstablished = pdTRUE;
        }

        /***************** Open queues and create HTTP task. ****************/

        /* Open request and response queues. */
        if( xDemoStatus == pdPASS )
        {
            xRequestQueue = xQueueCreate( democonfigQUEUE_SIZE,
                                          sizeof( RequestItem_t ) );

            xResponseQueue = xQueueCreate( democonfigQUEUE_SIZE,
                                           sizeof( ResponseItem_t ) );

            xDemoStatus = ( ( xRequestQueue != NULL ) && ( xResponseQueue != NULL ) ) ? pdPASS : pdFAIL;
        }

        /* Start HTTP task. */
        if( xDemoStatus == pdPASS )
        {
            xDemoStatus = xTaskCreate( prvStartHTTPTask, "HTTPTask", httpexampleTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHTTPTask );
        }

        /******************** Download S3 Object File. **********************/

        if( xDemoStatus == pdPASS )
        {
            xDemoStatus = prvDownloadS3ObjectFile( cServerHost,
                                                   xServerHostLength,
                                                   pcPath,
                                                   xRequestUriLen,
                                                   xRequestQueue,
                                                   xResponseQueue );
        }

        /************************** Disconnect. *****************************/

        /* Close the network connection to clean up any system resources that the
         * demo may have consumed. */
        if( xIsConnectionEstablished == pdTRUE )
        {
            /* Close the network connection.  */
            xNetworkStatus = SecureSocketsTransport_Disconnect( &xNetworkContext );

            if( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
            {
                LogError( ( "SecureSocketsTransport_Disconnect() failed to close the network connection. "
                            "StatusCode=%d.", ( int ) xNetworkStatus ) );
                xDemoStatus = pdFAIL;
            }
        }

        /*********** Clean up and evaluate demo iteration status. ***********/

        /* Close and delete the queues. */
        prvTearDown( xHTTPTask, xRequestQueue, xResponseQueue );

        /* Increment the demo run count. */
        uxDemoRunCount++;

        if( xDemoStatus == pdPASS )
        {
            LogInfo( ( "Demo iteration %lu was successful.", uxDemoRunCount ) );
        }
        /* Attempt to retry a failed iteration of demo for up to #httpexampleMAX_DEMO_COUNT times. */
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
