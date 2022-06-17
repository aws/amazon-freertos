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
 */

/*
 * Demo for showing use of the HTTP API using a server-authenticated network
 * connection.
 *
 * This example, using a pre-signed URL, resolves an AWS S3 domain, establishes a TCP
 * connection, validates the server's certificate using the root CA certificate
 * configured in the demo config file, and then finally completes the TLS handshake
 * with the HTTP server so that all communication is encrypted. The AWS S3 service
 * validates the demo client from the signature, that is part of the pre-signed URL
 * configured in the demo config file, provided in the HTTP request.
 *
 * Afterwards, the demo creates a thread-safe queue and the HTTP task, a worker task for
 * HTTP operations. It reads requests from its queue and executes them using the
 * HTTP Client library API. The HTTP task notifies the HTTP operation status back to
 * the main task through a direct-to-task notification. The main task sends HTTP requests
 * as well as memory for storing server responses to the HTTP task queue. The HTTP task
 * uses the passed information for calling the HTTPClient_Send API for requesting S3
 * bucket data in chunks using range requests. While doing so, the main task waits
 * for notification from the HTTP task. If the HTTP request fails, the HTTP task
 * notifies the failure status through the direct-to-task notification mechanism to
 * the main task.
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

/* Include header for root CA certificates. */
#include "iot_default_root_certificates.h"

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

/**
 * @brief The default value for the maximum number of times to run the loop in
 * this demo.
 */
#ifndef httpexampleMAX_DEMO_COUNT
    #define httpexampleMAX_DEMO_COUNT    ( 3 )
#endif

#ifndef httpexampleTASK_STACK_SIZE
    #define httpexampleTASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 4 )
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
 * @brief Data type for the queue of the HTTP Task that represents data for performing
 * an HTTP request/response operation.
 *
 * An element in the queue represents the memory of the HTTP request headers that
 * are used for sending the HTTP request and memory of the HTTP response in which the
 * HTTP task will populate the server response and its parsed information.
 */
typedef struct HttpTaskQueueItem
{
    HTTPRequestHeaders_t * pxRequestHeaders;
    HTTPResponse_t * pxResponse;
} HttpTaskQueueItem_t;

/**
 * @brief Queue for HTTP requests. The HTTP request headers and memory for HTTP response
 * is provided by the main task and pushed to this queue to notify HTTP task.
 * The HTTP task is responsible for receiving the HTTP request data from the queue,
 * executing the HTTP operation with the HTTPClient_Send() API and notifying the main
 * task about the operation result with a direct-to-task notification.
 */
static QueueHandle_t xHttpTaskQueue;

/**
 * @brief The buffer, owned by the main task, for storing the data of HTTP request headers.
 * This buffer is passed to the HTTP task for performing HTTP operation.
 *
 * @note This demo shows how memory owned by the main task is passed to the HTTP task for
 * performing HTTP Send operations without any expensive copy operation between the tasks.
 * Queues and direct-to-task notification synchronization mechanisms are used for
 * thread-safe access to the buffer between the main and HTTP task.
 */
static uint8_t pusHttpRequestBuffer[ democonfigUSER_BUFFER_LENGTH ];

/**
 * @brief The buffer, owned by the main task, for storing the HTTP response received from the
 * server. This buffer is supplied by the main task to the HTTP task through the queue.
 *
 * @note This demo shows how memory owned by the main task is passed to the HTTP task for
 * performing HTTP Send operations without any expensive copy operation between the tasks.
 * Queues and direct-to-task notification synchronization mechanisms are used for
 * thread-safe access to the buffer between the main and HTTP task.
 */
static uint8_t pusHttpResponseBuffer[ democonfigUSER_BUFFER_LENGTH ];

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
 * @param[in] xHttpTaskQueue The queue to which HTTP requests should be written.
 *
 * @return pdFAIL on failure; pdPASS on success.
 */
static BaseType_t prvDownloadS3ObjectFile( const char * pcHost,
                                           const size_t xHostLen,
                                           const char * pcRequest,
                                           const size_t xRequestUriLen,
                                           QueueHandle_t xHttpTaskQueue );

/**
 * @brief Enqueue an HTTP GET request for a given range of the S3 file.
 *
 * @param[in] pxRequestInfo The #HTTPRequestInfo_t for configuring the request.
 * @param[in] xHttpTaskQueue The queue to which HTTP requests should be written.
 * @param[in] xStart The position of the first byte in the range.
 * @param[in] xEnd The position of the last byte in the range, inclusive.
 *
 * @return pdFAIL on failure; pdPASS on success.
 */
static BaseType_t prvRequestS3ObjectRange( const HTTPRequestInfo_t * pxRequestInfo,
                                           const HttpTaskQueueItem_t * pxQueueItem,
                                           QueueHandle_t xHttpTaskQueue,
                                           const size_t xStart,
                                           const size_t xEnd );

/**
 * @brief Retrieve the size of the S3 object that is specified in pcPath using
 * HTTP task.
 *
 * @param[in] pxRequestInfo The #HTTPRequestInfo_t for configuring the request.
 * @param[in] xHttpTaskQueue The queue to which HTTP requests should be written.
 * @param[out] pxFileSize - The size of the S3 object.
 *
 * @return pdFAIL on failure; pdPASS on success.
 */
static BaseType_t prvGetS3ObjectFileSize( const HTTPRequestInfo_t * pxRequestInfo,
                                          const HttpTaskQueueItem_t * pxQueueItem,
                                          QueueHandle_t xHttpTaskQueue,
                                          size_t * pxFileSize );

/**
 * @brief Services HTTP requests pushed by the main task in its queue and notifies
 * the main task of the status of the HTTP operations through direct-to-task notifications.
 *
 * @note The HTTP task receives the HTTP request header data as well as memory for
 * storing and parsing the server response from the main task through the queue.
 *
 * @param[in] pvArgs Parameters as passed at the time of task creation. Not used
 * in this example.
 * */
static void prvStartHTTPTask( void * pvArgs );

/**
 * @brief Clean up resources created by demo.
 *
 * @param[in] xHandle The HTTP task handle.
 * @param[in] xHttpTaskQueue The queue of the HTTP task.
 */
static void prvTearDown( TaskHandle_t xHandle,
                         QueueHandle_t xHttpTaskQueue );

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
                                           QueueHandle_t xHttpTaskQueue )
{
    BaseType_t xStatus = pdPASS;
    size_t xRemainingResponseCount = 0;
    uint32_t ulWaitCounter = 0;

    /* Configurations of the initial request headers. */
    HTTPRequestInfo_t xRequestInfo = { 0 };

    HttpTaskQueueItem_t xQueueItem;
    HTTPRequestHeaders_t * pxRequestHeaders = NULL;
    HTTPResponse_t * pxResponse = NULL;

    /* The length of the file at democonfigS3_PRESIGNED_GET_URL. */
    size_t xFileSize = 0;

    /* The number of bytes we want to request within each range of the file. */
    size_t xNumReqBytes = 0;
    /* The starting byte for the next range request. */
    size_t xCurByte = 0;

    pxRequestHeaders = pvPortMalloc( sizeof( HTTPRequestHeaders_t ) );
    pxResponse = pvPortMalloc( sizeof( HTTPResponse_t ) );

    if( ( pxResponse == NULL ) || ( pxRequestHeaders == NULL ) )
    {
        LogError( ( "pvPortMalloc failed to allocate memory for HTTP request/response structures: pxResponse=%p, pxRequestHeaders=%p",
                    ( void * ) pxResponse, ( void * ) pxRequestHeaders ) );
        xStatus = pdFAIL;
    }
    else
    {
        memset( pxRequestHeaders, 0, sizeof( HTTPRequestHeaders_t ) );
        memset( pxResponse, 0, sizeof( HTTPResponse_t ) );

        pxRequestHeaders->pBuffer = pusHttpRequestBuffer;
        pxRequestHeaders->bufferLen = sizeof( pusHttpRequestBuffer );
        pxResponse->pBuffer = pusHttpResponseBuffer;
        pxResponse->bufferLen = sizeof( pusHttpResponseBuffer );

        xQueueItem.pxRequestHeaders = pxRequestHeaders;
        xQueueItem.pxResponse = pxResponse;
    }

    if( xStatus == pdPASS )
    {
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
                                          &xQueueItem,
                                          xHttpTaskQueue,
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
    }

    /* Until the entire file has been downloaded, we iterate the sending of byte range
     * requests to the HTTP task's queue and waiting for notifications of their
     * completion from the HTTP task. We keep track of the next starting byte to
     * download with xCurByte, and increment by xNumReqBytes after each iteration. When
     * xCurByte reaches xFileSize, we stop downloading. We keep track of the
     * number of responses we are waiting for with xRemainingResponseCount.
     */
    while( ( xStatus != pdFAIL ) && ( xCurByte < xFileSize || xRemainingResponseCount > 0 ) )
    {
        /* Send a range request for the specified bytes, if remaining. */
        if( xCurByte < xFileSize )
        {
            /* Enqueue a range request in the HTTP task queue. */
            xStatus = prvRequestS3ObjectRange( &xRequestInfo,
                                               &xQueueItem,
                                               xHttpTaskQueue,
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

        if( xRemainingResponseCount > 0 )
        {
            uint32_t httpSendStatus = 0U;

            /* Wait for notification of completion of HTTP operation from HTTP Task. */
            if( xTaskNotifyWait( 0,               /* No bits of notification value to clear on entry. */
                                 UINT32_MAX,      /* Clear notification value of task on exit. */
                                 &httpSendStatus, /* The notification value of HTTPClient_Send operation status
                                                   * from the HTTP task. */
                                 httpexampleDEMO_TICKS_TO_WAIT ) == pdFALSE )
            {
                LogError( ( "Unable to request bytes from S3 bucket: Timed out waiting for notification for HTTP operation from HTTP task" ) );
                xStatus = pdFAIL;
                break;
            }

            if( ( xStatus == pdPASS ) && ( ( HTTPStatus_t ) httpSendStatus == HTTPSuccess ) )
            {
                LogInfo( ( "Received notification of server response from HTTP task." ) );
                LogDebug( ( "Response Headers:\n%.*s",
                            ( int32_t ) pxResponse->headersLen,
                            pxResponse->pHeaders ) );
                LogDebug( ( "Response Status:\n%u",
                            pxResponse->statusCode ) );
                LogInfo( ( "Response Body:\n%.*s\n",
                           ( int32_t ) pxResponse->bodyLen,
                           pxResponse->pBody ) );

                /* Check for a partial content status code (206), indicating a
                 * successful server response. */
                if( pxResponse->statusCode != httpexampleHTTP_STATUS_CODE_PARTIAL_CONTENT )
                {
                    LogError( ( "Received response with unexpected status code: %d", pxResponse->statusCode ) );
                    xStatus = pdFAIL;
                    break;
                }

                /* Reset the wait counter every time a response is received. */
                ulWaitCounter = 0;
                xRemainingResponseCount -= 1;
            }
            else
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

    /* Clean up heap allocated memory for HTTP structures. */
    if( pxRequestHeaders != NULL )
    {
        vPortFree( pxRequestHeaders );
    }

    if( pxResponse != NULL )
    {
        vPortFree( pxResponse );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prvRequestS3ObjectRange( const HTTPRequestInfo_t * pxRequestInfo,
                                           const HttpTaskQueueItem_t * pxQueueItem,
                                           QueueHandle_t xHttpTaskQueue,
                                           const size_t xStart,
                                           const size_t xEnd )
{
    HTTPStatus_t xHTTPStatus = HTTPSuccess;
    BaseType_t xStatus = pdPASS;


    configASSERT( pxRequestInfo != NULL );

    xHTTPStatus = HTTPClient_InitializeRequestHeaders( pxQueueItem->pxRequestHeaders,
                                                       pxRequestInfo );

    if( xHTTPStatus != HTTPSuccess )
    {
        LogError( ( "Failed to initialize HTTP request headers: Error=%s.",
                    HTTPClient_strerror( xHTTPStatus ) ) );
        xStatus = pdFAIL;
    }

    if( xStatus == pdPASS )
    {
        xHTTPStatus = HTTPClient_AddRangeHeader( pxQueueItem->pxRequestHeaders,
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
                    ( int32_t ) pxQueueItem->pxRequestHeaders->headersLen,
                    ( char * ) pxQueueItem->pxRequestHeaders->pBuffer ) );

        xStatus = xQueueSendToBack( xHttpTaskQueue,
                                    pxQueueItem,
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

static BaseType_t prvGetS3ObjectFileSize( const HTTPRequestInfo_t * pxRequestInfo,
                                          const HttpTaskQueueItem_t * pxQueueItem,
                                          QueueHandle_t xHttpTaskQueue,
                                          size_t * pxFileSize )
{
    BaseType_t xStatus = pdPASS;
    HTTPStatus_t xHTTPStatus = HTTPSuccess;

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
                                       pxQueueItem,
                                       xHttpTaskQueue,
                                       0,
                                       0 );

    /* Wait for notification of the HTTP operation for the S3 file size request. */
    if( xStatus == pdPASS )
    {
        uint32_t ulHttpStatusValue = 0;

        if( xTaskNotifyWait( 0,                  /* No bits of notification value to clear on entry. */
                             UINT32_MAX,         /* Clear notification value of task on exit. */
                             &ulHttpStatusValue, /* Notification value is HTTPStatus_t value from HTTPClient_Send Call
                                                  * in the HTTP Task. */
                             httpexampleDEMO_TICKS_TO_WAIT ) == pdFALSE )
        {
            LogError( ( "Failure in HTTP request for requesting S3 bucket file size: Timed out waiting for notification from HTTP Task" ) );
            xStatus = pdFAIL;
        }
        else
        {
            xHTTPStatus = ( HTTPStatus_t ) ulHttpStatusValue;
        }
    }

    if( ( xStatus == pdPASS ) && ( xHTTPStatus == HTTPSuccess ) )
    {
        if( pxQueueItem->pxResponse->statusCode != httpexampleHTTP_STATUS_CODE_PARTIAL_CONTENT )
        {
            LogError( ( "Received response with unexpected status code: %d.", pxQueueItem->pxResponse->statusCode ) );
            xStatus = pdFAIL;
        }
        else
        {
            xHTTPStatus = HTTPClient_ReadHeader( pxQueueItem->pxResponse,
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
    else if( xStatus == pdPASS )
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
    HttpTaskQueueItem_t xHttpStructs = { 0 };

    ( void ) pvArgs;

    /* Define the transport interface. */
    xTransportInterface.pNetworkContext = &xNetworkContext;
    xTransportInterface.send = SecureSocketsTransport_Send;
    xTransportInterface.recv = SecureSocketsTransport_Recv;

    for( ; ; )
    {
        /* Read request from queue. */
        xStatus = xQueueReceive( xHttpTaskQueue,
                                 &xHttpStructs,
                                 httpexampleDEMO_TICKS_TO_WAIT );

        if( xStatus == pdFAIL )
        {
            LogInfo( ( "No requests in the queue. Trying again." ) );
            continue;
        }

        LogInfo( ( "The HTTP task retrieved an HTTP request from the queue." ) );
        LogDebug( ( "Request Headers:\n%.*s",
                    ( int32_t ) xHttpStructs.xRequestHeaders->headersLen,
                    ( char * ) xHttpStructs.xRequestHeaders->pBuffer ) );

        xHTTPStatus = HTTPClient_Send( &xTransportInterface,
                                       xHttpStructs.pxRequestHeaders,
                                       NULL,
                                       0,
                                       xHttpStructs.pxResponse,
                                       0 );

        /* Notify the main task of the return status of the coreHTTP API call
         * for HTTP operation of sending request and receiving response. */
        xTaskNotify( xMainTask, xHTTPStatus, eSetBits );

        if( xHTTPStatus != HTTPSuccess )
        {
            LogError( ( "Failed to send HTTP request: Error=%s.",
                        HTTPClient_strerror( xHTTPStatus ) ) );
            break;
        }
        else
        {
            LogInfo( ( "The HTTP task received a response from the server." ) );
        }
    }
}

/*-----------------------------------------------------------*/

void prvTearDown( TaskHandle_t xHandle,
                  QueueHandle_t xHttpTaskQueue )
{
    /* Delete HTTP task. */
    LogInfo( ( "Deleting HTTP task." ) );
    vTaskDelete( xHandle );

    /* Close and delete the queues. */
    if( xHttpTaskQueue != NULL )
    {
        vQueueDelete( xHttpTaskQueue );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Entry point of the demo.
 *
 * This example, using a pre-signed URL, resolves an AWS S3 domain, establishes a TCP
 * connection, validates the server's certificate using the root CA certificate
 * configured in the demo config file, and then finally completes the TLS handshake
 * with the HTTP server so that all communication is encrypted. The AWS S3 service
 * validates the demo client from the signature, that is part of the pre-signed URL
 * configured in the demo config file, provided in the HTTP request.
 *
 * The demo initiates an independent HTTP task to read requests from its queue
 * and execute them using the HTTP Client library API, and notify the HTTP operation
 * status back to the main task through direct-to-task notifications. The main task
 * sends HTTP requests as well as memory for storing server response to the HTTP task
 * queue. The HTTP task uses the passed information for calling the HTTPClient_Send
 * API for requesting S3 bucket data in chunks using range requests. While doing so,
 * the main task waits for notification from the HTTP task. If the HTTP request fails,
 * the HTTP task notifies the failure status through the direct-to-task notification
 * mechanism to the main task.
 *
 * @note This example is multi-threaded, uses statically allocated memory for HTTP request
 * and response buffers, and heap memory for the coreHTTP structures and the HTTP task
 * queue.
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

        /* Create the  queues. */
        if( xDemoStatus == pdPASS )
        {
            xHttpTaskQueue = xQueueCreate( democonfigQUEUE_SIZE,
                                           sizeof( HttpTaskQueueItem_t ) );

            xDemoStatus = ( xHttpTaskQueue != NULL ) ? pdPASS : pdFAIL;
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
                                                   xHttpTaskQueue );
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
        prvTearDown( xHTTPTask, xHttpTaskQueue );

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
