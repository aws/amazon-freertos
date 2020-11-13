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
 */

/*
 * Demo for showing use of the HTTP API using a server-authenticated network
 * connection.
 *
 * This example resolves a domain, establishes a TCP connection, validates the
 * server's certificate using the root CA certificate defined in the config
 * header, then finally performs a TLS handshake with the HTTP server so that
 * all communication is encrypted. After which, an HTTP thread is started which
 * uses HTTP Client library API to send requests it reads from the request
 * queue, and writes the responses to the response queue. The main thread sends
 * requests on the request queue, which are used to download the S3 file by
 * sending multiple range requests. While it is doing this, the main thread also
 * reads responses from the response queue and prints them until the entire file
 * is received. If any request fails, an error code is returned.
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

/* FreeRTOS includes. */
#include "FreeRTOS_POSIX.h"

/* FreeRTOS+POSIX. */
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/sys/types.h"

/* Include demo config as the first non-system header. */
#include "http_demo_s3_download_multithreaded_config.h"

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

/*------------- Demo configurations -------------------------*/

/* Check that TLS port of the server is defined. */
#ifndef democonfigHTTPS_PORT
    #error "Please define a democonfigHTTPS_PORT."
#endif

/* Check that the root CA certificate is defined. */
#ifndef democonfigROOT_CA_PEM
    #error "Please define a democonfigROOT_CA_PEM."
#endif

/* Check the the queue size is defined. */
#ifndef democonfigQUEUE_SIZE
    #error "Please define a QUEUE_SIZE."
#endif

/* Check that the pre-signed GET URL is defined. */
#ifndef democonfigS3_PRESIGNED_GET_URL
    #define democonfigS3_PRESIGNED_GET_URL    "GET-URL"
#endif

/* Check that transport timeout for transport send and receive is defined. */
#ifndef democonfigTRANSPORT_SEND_RECV_TIMEOUT_MS
    #define democonfigTRANSPORT_SEND_RECV_TIMEOUT_MS    ( 1000 )
#endif

/* Check that size of the user buffer is defined. */
#ifndef democonfigUSER_BUFFER_LENGTH
    #define democonfigUSER_BUFFER_LENGTH    ( 4096 )
#endif

/* Check that the range request length is defined. */
#ifndef democonfigRANGE_REQUEST_LENGTH
    #define democonfigRANGE_REQUEST_LENGTH    ( 2048 )
#endif

/**
 * @brief The name of the HTTP thread's input queue. Must begin with a slash and
 * be a valid pathname.
 */
#define httpexampleREQUEST_QUEUE                             "/demo_request_queue"

/**
 * @brief The name of the HTTP thread's output queue. Must begin with a slash
 * and be a valid pathname.
 */
#define httpexampleRESPONSE_QUEUE                            "/demo_response_queue"

/* Posix file permissions for the queues. Allows read and write access to the
 * user this demo is running as. */
#define httpexampleQUEUE_PERMISSIONS                         0600

/**
 * @brief Length of the pre-signed GET URL defined in demo_config.h.
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
 * @brief Data type for request queue.
 *
 * In addition to sending #HTTPRequestHeaders_t, we need to send the buffer it uses.
 * The pointer to the buffer in #HTTPRequestHeaders_t will be incorrect after
 * it is received, so it will need to be fixed.
 */
typedef struct RequestItem
{
    HTTPRequestHeaders_t xRequestHeaders;
    uint8_t ucHeaderBuffer[ democonfigUSER_BUFFER_LENGTH ];
} RequestItem_t;

/**
 * @brief Data type for response queue.
 *
 * In addition to sending the #HTTPResponse_t, we need to send the buffer it uses.
 * The pointer to the buffer in #HTTPResponse_t will be incorrect after
 * it is received, so it will need to be fixed.
 */
typedef struct ResponseItem
{
    HTTPResponse_t xResponse;
    uint8_t ucResponseBuffer[ democonfigUSER_BUFFER_LENGTH ];
} ResponseItem_t;

/*
 * @brief Struct used for sending requests to the HTTP thread.
 *
 * This is used by both the main and HTTP threads. We have this as a global so
 * that it will be located at the same address in the main and HTTP threads so
 * that way pointers remain valid when copied over.
 */
static RequestItem_t xRequestItem = { 0 };

/**
 * @brief Struct used for receiving responses from the HTTP thread.
 *
 * This is used by both the main and HTTP threads. We have this as a global so
 * that it will be located at the same address in the main and HTTP threads so
 * that way pointers remain valid when copied over.
 */
static ResponseItem_t xResponseItem = { 0 };

/**
 * @brief Queue for HTTP requests. Requests are written by the main thread, and
 * serviced by the HTTP thread.
 */
static mqd_t xRequestQueue = ( mqd_t ) -1;

/**
 * @brief Queue for HTTP responses. Responses are written by the HTTP thread,
 * and read by the main thread.
 */
static mqd_t xResponseQueue = ( mqd_t ) -1;

/**
 * @brief The return status for requestS3ObjectRange() and retrieveHTTPResponse().
 */
typedef enum QueueOpStatus
{
    /**
     * @brief The function completed successfully.
     */
    QUEUE_OP_SUCCESS,

    /**
     * @brief The function was given a non-blocking queue and would have blocked
     * were it a blocking queue.
     */
    QUEUE_OP_WOULD_BLOCK,

    /**
     * @brief The function encountered an error.
     */
    QUEUE_OP_FAILURE,
} QueueOpStatus_t;

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
 * @brief Send an HTTP request based on a specified method and path, and
 * print the response received from the server.
 *
 * @param[in] pcHost The host name of the server.
 * @param[in] xHostLen The length of pcHost.
 * @param[in] pcRequest The HTTP Request-URI.
 * @param[in] xRequestUriLen The length of pcRequest.
 * @param[in] xFileSize The length of the file at democonfigS3_PRESIGNED_GET_URL.
 * @param[in] xRequestQueue The queue to which HTTP requests should be written.
 * @param[in] xResponseQueue The queue from which HTTP responses should be read.
 *
 * @return pdFAIL on failure; pdPASS on success.
 */
static BaseType_t prvDownloadS3ObjectFile( const char * pcHost,
                                           const size_t xHostLen,
                                           const char * pcRequest,
                                           const size_t xRequestUriLen,
                                           mqd_t xRequestQueue,
                                           mqd_t xResponseQueue );

/**
 * @brief Enqueue an HTTP request for a range of the S3 file.
 *
 * @param[in] pxRequestInfo The #HTTPRequestInfo_t for configuring the request.
 * @param[in] xRequestQueue The queue to which HTTP requests should be written.
 * @param[in] xStart The position of the first byte in the range.
 * @param[in] xEnd The position of the last byte in the range, inclusive.
 *
 * @return QUEUE_OP_FAILURE on failure; QUEUE_OP_WOULD_BLOCK if would block,
 * QUEUE_OP_SUCCESS on success.
 */
static QueueOpStatus_t prvRequestS3ObjectRange( const HTTPRequestInfo_t * pxRequestInfo,
                                                mqd_t xRequestQueue,
                                                const size_t xStart,
                                                const size_t xEnd );

/**
 * @brief Processes an HTTP response from the response queue.
 *
 * @param[in] xResponseQueue The queue from which HTTP responses should be read.
 * @param[out] pxResponseItem The HTTP response received.
 *
 * @return QUEUE_OP_FAILURE on failure; QUEUE_OP_WOULD_BLOCK if would block,
 * QUEUE_OP_SUCCESS on success.
 */
static QueueOpStatus_t prvRetrieveHTTPResponse( mqd_t xResponseQueue,
                                                ResponseItem_t * pxResponseItem );

/**
 * @brief Retrieve the size of the S3 object that is specified in pcPath using
 * HTTP thread.
 *
 * @param[in] pxRequestInfo The #HTTPRequestInfo_t for configuring the request.
 * @param[in] xRequestQueue The queue to which HTTP requests should be written.
 * @param[in] xResponseQueue The queue from which HTTP responses should be read.
 * @param[out] pxFileSize - The size of the S3 object.
 *
 * @return pdFAIL on failure; pdPASS on success.
 */
static BaseType_t prvGetS3ObjectFileSize( const HTTPRequestInfo_t * pxRequestInfo,
                                          mqd_t xRequestQueue,
                                          mqd_t xResponseQueue,
                                          size_t * pxFileSize );

/**
 * @brief Services HTTP requests from the request queue and writes the
 * responses to the response queue.
 *
 * @param[in] pTransportInterface The transport interface for making network calls.
 */
static void prvStartHTTPThread( void * pvArgs );

/**
 * @brief Clean up resources created by demo.
 *
 * @param[in] httpThread The HTTP thread.
 * @param[in] xRequestQueue The request queue.
 * @param[in] xResponseQueue The response queue.
 */
static void prvTearDown( pthread_t xHttpThread,
                         mqd_t xRequestQueue,
                         mqd_t xResponseQueue );

/*-----------------------------------------------------------*/

static BaseType_t prvConnectToServer( NetworkContext_t * pxNetworkContext )
{
    ServerInfo_t xServerInfo = { 0 };
    SocketsConfig_t xSocketsConfig = { 0 };
    BaseType_t xStatus = pdPASS;
    TransportSocketStatus_t xNetworkStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;

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
        xStatus = pdFAIL;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prvDownloadS3ObjectFile( const char * pcHost,
                                           const size_t xHostLen,
                                           const char * pcRequest,
                                           const size_t xRequestUriLen,
                                           mqd_t xRequestQueue,
                                           mqd_t xResponseQueue )
{
    BaseType_t xStatus = pdPASS;
    QueueOpStatus_t xQueueOpStatus = QUEUE_OP_SUCCESS;

    size_t xRequestCount = 0;

    /* Configurations of the initial request headers. */
    HTTPRequestInfo_t xRequestInfo = { 0 };

    /* The length of the file at democonfigS3_PRESIGNED_GET_URL. */
    size_t xFileSize = 0;

    /* The number of bytes we want to request within each range of the file. */
    size_t xNumReqBytes = 0;
    /* xCurByte indicates which starting byte we want to download next. */
    size_t xCurByte = 0;

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

    if( xFileSize < democonfigRANGE_REQUEST_LENGTH )
    {
        xNumReqBytes = xFileSize;
    }
    else
    {
        xNumReqBytes = democonfigRANGE_REQUEST_LENGTH;
    }

    /* Here we iterate sending byte range requests and retrieving responses
     * until the full file has been downloaded. We keep track of the next byte
     * to download with xCurByte. When this reaches the xFileSize we stop
     * downloading. We keep track of the number of responses we are waiting for
     * with xRequestCount.
     */
    while( ( xStatus != pdFAIL ) && ( xCurByte < xFileSize || xRequestCount > 0 ) )
    {
        /* Send range request if remaining. */
        if( xCurByte < xFileSize )
        {
            xQueueOpStatus = prvRequestS3ObjectRange( &xRequestInfo,
                                                      xRequestQueue,
                                                      xCurByte,
                                                      xCurByte + xNumReqBytes - 1 );

            xStatus = ( xQueueOpStatus == QUEUE_OP_SUCCESS ) ? pdPASS : pdFAIL;

            if( xStatus == pdPASS )
            {
                xRequestCount += 1;
                xCurByte += xNumReqBytes;

                if( ( xFileSize - xCurByte ) < xNumReqBytes )
                {
                    xNumReqBytes = xFileSize - xCurByte;
                }
            }
        }

        /* Retrieve response. */
        if( ( xRequestCount > 0 ) && ( xQueueOpStatus != QUEUE_OP_FAILURE ) )
        {
            xQueueOpStatus = prvRetrieveHTTPResponse( xResponseQueue, &xResponseItem );

            xStatus = ( xQueueOpStatus == QUEUE_OP_SUCCESS ) ? pdPASS : pdFAIL;

            if( xStatus == pdPASS )
            {
                LogInfo( ( "Main thread received HTTP response" ) );
                LogDebug( ( "Response Headers:\n%.*s",
                            ( int32_t ) xResponseItem.xResponse.headersLen,
                            xResponseItem.xResponse.pHeaders ) );
                LogDebug( ( "Response Status:\n%u", xResponseItem.xResponse.statusCode ) );
                LogDebug( ( "Response Body:\n%.*s\n", ( int32_t ) xResponseItem.xResponse.bodyLen,
                            xResponseItem.xResponse.pBody ) );

                if( xResponseItem.xResponse.statusCode != httpexampleHTTP_STATUS_CODE_PARTIAL_CONTENT )
                {
                    LogError( ( "Received response with unexpected status code: %d", xResponseItem.xResponse.statusCode ) );
                    xStatus = pdFAIL;
                }
                else
                {
                    xRequestCount -= 1;
                }
            }
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static QueueOpStatus_t prvRequestS3ObjectRange( const HTTPRequestInfo_t * pxRequestInfo,
                                                mqd_t xRequestQueue,
                                                const size_t xStart,
                                                const size_t xEnd )
{
    QueueOpStatus_t xStatus = QUEUE_OP_SUCCESS;
    HTTPStatus_t xHTTPStatus = HTTPSuccess;

    /* Return value of mq_send. */
    int mqerror = 0;

    /* Set the buffer used for storing request headers. */
    xRequestItem.xRequestHeaders.pBuffer = xRequestItem.ucHeaderBuffer;
    xRequestItem.xRequestHeaders.bufferLen = democonfigUSER_BUFFER_LENGTH;

    xHTTPStatus = HTTPClient_InitializeRequestHeaders( &( xRequestItem.xRequestHeaders ),
                                                       pxRequestInfo );

    if( xHTTPStatus != HTTPSuccess )
    {
        LogError( ( "Failed to initialize HTTP request headers: Error=%s.",
                    HTTPClient_strerror( xHTTPStatus ) ) );
        xStatus = QUEUE_OP_FAILURE;
    }

    if( xStatus == QUEUE_OP_SUCCESS )
    {
        xHTTPStatus = HTTPClient_AddRangeHeader( &( xRequestItem.xRequestHeaders ),
                                                 xStart,
                                                 xEnd );

        if( xHTTPStatus != HTTPSuccess )
        {
            LogError( ( "Failed to add Range header to request headers: Error=%s.",
                        HTTPClient_strerror( xHTTPStatus ) ) );
            xStatus = QUEUE_OP_FAILURE;
        }
    }

    if( xStatus == QUEUE_OP_SUCCESS )
    {
        /* Enqueue the request. */
        LogInfo( ( "Enqueuing bytes %d to %d of S3 Object:  ",
                   ( int32_t ) xStart,
                   ( int32_t ) xEnd ) );
        LogDebug( ( "Request Headers:\n%.*s",
                    ( int32_t ) xRequestItem.xRequestHeaders.headersLen,
                    ( char * ) xRequestItem.xRequestHeaders.pBuffer ) );

        mqerror = mq_send( xRequestQueue,
                           ( char * ) &xRequestItem,
                           sizeof( RequestItem_t ),
                           0 );

        if( mqerror == -1 )
        {
            if( errno != EAGAIN )
            {
                /* Error other than due to not blocking. */
                LogError( ( "Failed to write to request queue with error %s.",
                            strerror( errno ) ) );
                xStatus = QUEUE_OP_FAILURE;
            }
            else
            {
                xStatus = QUEUE_OP_WOULD_BLOCK;
            }
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static QueueOpStatus_t prvRetrieveHTTPResponse( mqd_t xResponseQueue,
                                                ResponseItem_t * pxResponseItem )
{
    QueueOpStatus_t xStatus = QUEUE_OP_SUCCESS;
    /* Return value of mq_receive. */
    int mqread = 0;

    /* Read response from queue. */
    mqread = mq_receive( xResponseQueue, ( char * ) pxResponseItem,
                         sizeof( ResponseItem_t ), NULL );

    if( mqread == -1 )
    {
        if( errno != EAGAIN )
        {
            /* Error other than due to not blocking. */
            LogError( ( "Failed to read from response queue with error %s.",
                        strerror( errno ) ) );
            xStatus = QUEUE_OP_FAILURE;
        }
        else
        {
            xStatus = QUEUE_OP_WOULD_BLOCK;
        }
    }

    if( xStatus == QUEUE_OP_SUCCESS )
    {
        if( mqread != sizeof( ResponseItem_t ) )
        {
            LogError( ( "Response from response queue has incorrect size." ) );
            xStatus = QUEUE_OP_FAILURE;
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prvGetS3ObjectFileSize( const HTTPRequestInfo_t * pxRequestInfo,
                                          mqd_t xRequestQueue,
                                          mqd_t xResponseQueue,
                                          size_t * pxFileSize )
{
    BaseType_t xStatus = pdPASS;
    HTTPStatus_t xHTTPStatus = HTTPSuccess;
    QueueOpStatus_t xQueueOpStatus = QUEUE_OP_SUCCESS;

    /* The location of the file size in pcContentRangeValStr. */
    char * pcFileSizeStr = NULL;

    /* String to store the Content-Range header value. */
    char * pcContentRangeValStr = NULL;
    size_t xContentRangeValStrLength = 0;

    LogInfo( ( "Getting file object size from host..." ) );

    /* Request bytes 0 to 0. S3 will respond with a Content-Range
     * header that contains the size of the file in it. This header will look
     * like: "Content-Range: bytes 0-0/FILESIZE". The body will have a single
     * byte that we are ignoring. */
    xQueueOpStatus = prvRequestS3ObjectRange( pxRequestInfo,
                                              xRequestQueue,
                                              0,
                                              0 );

    xStatus = ( xQueueOpStatus == QUEUE_OP_SUCCESS ) ? pdPASS : pdFAIL;

    if( xStatus == pdPASS )
    {
        do
        {
            xQueueOpStatus = prvRetrieveHTTPResponse( xResponseQueue, &xResponseItem );
        } while( xQueueOpStatus == QUEUE_OP_WOULD_BLOCK );

        if( xQueueOpStatus == QUEUE_OP_FAILURE )
        {
            xStatus = pdFAIL;
        }
        else if( xResponseItem.xResponse.statusCode != httpexampleHTTP_STATUS_CODE_PARTIAL_CONTENT )
        {
            LogError( ( "Received response with unexpected status code: %d.", xResponseItem.xResponse.statusCode ) );
            xStatus = pdFAIL;
        }
    }

    if( xStatus == pdPASS )
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

static void prvStartHTTPThread( void * pvArgs )
{
    TransportInterface_t xTransport = *( TransportInterface_t * ) pvArgs;
    HTTPStatus_t xHTTPStatus = HTTPSuccess;
    /* Return value of mq_receive. */
    int mqread = 0;
    /* Return value of mq_send. */
    int mqerror = 0;

    /* Initialize response struct. */
    xResponseItem.xResponse.pBuffer = xResponseItem.ucResponseBuffer;
    xResponseItem.xResponse.bufferLen = sizeof( xResponseItem.ucResponseBuffer );

    for( ; ; )
    {
        xHTTPStatus = HTTPSuccess;
        mqread = 0;
        mqerror = 0;

        /* Read request from queue. */
        mqread = mq_receive( xRequestQueue,
                             ( char * ) &xRequestItem,
                             sizeof( RequestItem_t ),
                             NULL );

        if( mqread == -1 )
        {
            LogError( ( "Failed to read from request queue with error %s.",
                        strerror( errno ) ) );
        }

        if( mqread != sizeof( RequestItem_t ) )
        {
            LogError( ( "Response from request queue has incorrect size." ) );
        }

        LogInfo( ( "HTTP thread retrieved request." ) );
        LogDebug( ( "Request Headers:\n%.*s",
                    ( int32_t ) xRequestItem.xRequestHeaders.headersLen,
                    ( char * ) xRequestItem.xRequestHeaders.pBuffer ) );

        xHTTPStatus = HTTPClient_Send( &xTransport,
                                       &xRequestItem.xRequestHeaders,
                                       NULL,
                                       0,
                                       &xResponseItem.xResponse,
                                       0 );

        if( xHTTPStatus != HTTPSuccess )
        {
            LogError( ( "Failed to send HTTP request: Error=%s.",
                        HTTPClient_strerror( xHTTPStatus ) ) );
        }
        else
        {
            LogInfo( ( "HTTP thread received HTTP response" ) );
            /* Write response to queue. */
            mqerror = mq_send( xResponseQueue,
                               ( char * ) &xResponseItem,
                               sizeof( ResponseItem_t ),
                               0 );

            if( mqerror != 0 )
            {
                LogError( ( "Failed to write to response queue with error %s.",
                            strerror( errno ) ) );
            }
        }
    }
}

/*-----------------------------------------------------------*/

void prvTearDown( pthread_t xHttpThread,
                  mqd_t xRequestQueue,
                  mqd_t xResponseQueue )
{
    /* Wait for the thread to terminate. */
    ( void ) pthread_join( xHttpThread, NULL );

    /* Close and then delete the queues. */
    if( xRequestQueue != ( mqd_t ) -1 )
    {
        if( mq_close( xRequestQueue ) == -1 )
        {
            LogError( ( "Failed to close request queue with error %s.",
                        strerror( errno ) ) );
        }

        if( mq_unlink( httpexampleREQUEST_QUEUE ) == -1 )
        {
            LogError( ( "Failed to delete request queue with error %s.",
                        strerror( errno ) ) );
        }
    }

    if( xResponseQueue != ( mqd_t ) -1 )
    {
        if( mq_close( xResponseQueue ) == -1 )
        {
            LogError( ( "Failed to close response queue with error %s.",
                        strerror( errno ) ) );
        }

        if( mq_unlink( httpexampleRESPONSE_QUEUE ) == -1 )
        {
            LogError( ( "Failed to delete response queue with error %s.",
                        strerror( errno ) ) );
        }
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Entry point of the demo.
 *
 * This example resolves a domain, establishes a TCP connection, validates the
 * server's certificate using the root CA certificate defined in the config
 * header, then finally performs a TLS handshake with the HTTP server so that
 * all communication is encrypted. After which, an HTTP thread is started which
 * uses HTTP Client library API to send requests it reads from the request
 * queue, and writes the responses to the response queue. The main thread sends
 * requests on the request queue, which are used to download the S3 file by
 * sending multiple range requests. While it is doing this, the main thread also
 * reads responses from the response queue and prints them until the entire file
 * is received. If any request fails, an error code is returned.
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
    /* The transport layer interface used by the HTTP Client library. */
    TransportInterface_t xTransportInterface;
    /* The network context for the transport layer interface. */
    NetworkContext_t xNetworkContext = { 0 };
    TransportSocketStatus_t xNetworkStatus;
    BaseType_t xIsConnectionEstablished = pdFALSE;
    /* HTTPS Client library return status. */
    HTTPStatus_t xHTTPStatus = HTTPSuccess;
    /* The location of the host address within the pre-signed URL. */
    const char * pcAddress = NULL;
    /* The location of the path within the pre-signed URL. */
    const char * pcPath = NULL;

    /* Upon return, pdPASS will indicate a successful demo execution.
    * pdFAIL will indicate some failures occurred during execution. The
    * user of this demo must check the logs for any failure codes. */
    BaseType_t xDemoStatus = pdPASS;

    /* The length of the path within the pre-signed URL. This variable is
     * defined in order to store the length returned from parsing the URL, but
     * it is unused. The path used for the requests in this demo needs all the
     * query information following the location of the object, to the end of the
     * S3 presigned URL. */
    size_t pathLen = 0;
    /* The length of the Request-URI within string S3_PRESIGNED_GET_URL */
    size_t xRequestUriLen = 0;

    /* Settings for constructing queues. */
    struct mq_attr xQueueSettings;
    /* HTTP thread. */
    pthread_t xThread = { 0 };

    /* Remove compiler warnings about unused parameters. */
    ( void ) awsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;
    ( void ) pNetworkInterface;

    LogInfo( ( "HTTP Client multi-threaded S3 download demo using pre-signed URL:\n%s", democonfigS3_PRESIGNED_GET_URL ) );

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
         * after a timeout. The timeout value will be exponentially increased
         * until either the maximum number of attempts or the maximum timeout
         * value is reached. The function returns pdFAIL if the TCP connection
         * cannot be established with the broker after the configured number of
         * attempts. */
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

        /* Define the transport interface. */
        xTransportInterface.pNetworkContext = &xNetworkContext;
        xTransportInterface.send = SecureSocketsTransport_Send;
        xTransportInterface.recv = SecureSocketsTransport_Recv;
    }

    /******************** Open queues and HTTP task. *******************/

    /* Open request and response queues. */
    if( xDemoStatus == pdPASS )
    {
        xQueueSettings.mq_maxmsg = democonfigQUEUE_SIZE;
        xQueueSettings.mq_msgsize = sizeof( RequestItem_t );

        xRequestQueue = mq_open( httpexampleREQUEST_QUEUE,
                                 O_CREAT | O_RDWR,
                                 ( mode_t ) 0,
                                 &xQueueSettings );

        if( xRequestQueue == ( mqd_t ) -1 )
        {
            LogError( ( "Failed to open request queue with error %s.",
                        strerror( errno ) ) );
            xDemoStatus = pdFAIL;
        }

        xQueueSettings.mq_msgsize = sizeof( ResponseItem_t );

        xResponseQueue = mq_open( httpexampleRESPONSE_QUEUE,
                                  O_CREAT | O_RDWR,
                                  ( mode_t ) 0,
                                  &xQueueSettings );

        if( xResponseQueue == ( mqd_t ) -1 )
        {
            LogError( ( "Failed to open response queue with error %s.",
                        strerror( errno ) ) );
            xDemoStatus = pdFAIL;
        }
    }

    /* Start the HTTP thread which services requests in xRequestQueue. */
    if( xDemoStatus == pdPASS )
    {
        ( void ) pthread_create( &( xThread ), NULL, prvStartHTTPThread, &xTransportInterface );
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
            xDemoStatus = pdFAIL;
            LogError( ( "SecureSocketsTransport_Disconnect() failed to close the network connection. "
                        "StatusCode=%d.", ( int ) xNetworkStatus ) );
        }
    }

    if( xDemoStatus == pdPASS )
    {
        LogInfo( ( "Demo completed successfully." ) );
    }

    /******************** Clean up queues and HTTP thread. ****************/

    prvTearDown( xThread, xRequestQueue, xResponseQueue );

    return ( xDemoStatus == pdPASS ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
