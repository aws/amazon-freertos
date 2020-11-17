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
 * This example, using a pre-signed URL, resolves a S3 domain, establishes a TCP
 * connection, validates the server's certificate using the root CA certificate
 * defined in the config header, then finally performs a TLS handshake with the
 * HTTP server so that all communication is encrypted. After which, the HTTP
 * client library API is used to download the S3 file (by sending multiple GET
 * requests, filling up the response buffer each time until all parts are
 * downloaded). If any request fails, an error code is returned.
 */

/**
 * @file http_demo_s3_download.c
 * @brief Demonstrates usage of the HTTP library.
 */

/* Standard includes. */
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Include demo config as the first non-system header. */
#include "http_demo_s3_download_config.h"

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
 * @brief Length of the pre-signed GET URL defined in demo_config.h.
 */
#define S3_PRESIGNED_GET_URL_LENGTH               ( sizeof( democonfigS3_PRESIGNED_GET_URL ) - 1 )

/**
 * @brief The length of the HTTP GET method.
 */
#define HTTP_METHOD_GET_LENGTH                    ( sizeof( HTTP_METHOD_GET ) - 1 )

/**
 * @brief Field name of the HTTP range header to read from server response.
 */
#define HTTP_CONTENT_RANGE_HEADER_FIELD           "Content-Range"

/**
 * @brief Length of the HTTP range header field.
 */
#define HTTP_CONTENT_RANGE_HEADER_FIELD_LENGTH    ( sizeof( HTTP_CONTENT_RANGE_HEADER_FIELD ) - 1 )

/**
 * @brief The HTTP status code returned for partial content.
 */
#define HTTP_STATUS_CODE_PARTIAL_CONTENT          206

/**
 * @brief A buffer used in the demo for storing HTTP request headers, and HTTP
 * response headers and body.
 *
 * @note This demo shows how the same buffer can be re-used for storing the HTTP
 * response after the HTTP request is sent out. However, the user can decide how
 * to use buffers to store HTTP requests and responses.
 */
static uint8_t xUserBuffer[ democonfigUSER_BUFFER_LENGTH ];

/**
 * @brief Represents header data that will be sent in an HTTP request.
 */
static HTTPRequestHeaders_t xRequestHeaders;

/**
 * @brief Configurations of the initial request headers that are passed to
 * #HTTPClient_InitializeRequestHeaders.
 */
static HTTPRequestInfo_t xRequestInfo;

/**
 * @brief Represents a response returned from an HTTP server.
 */
static HTTPResponse_t xResponse;

/**
 * @brief The host address string extracted from the pre-signed URL.
 *
 * @note S3_PRESIGNED_GET_URL_LENGTH is set as the array length here as the
 * length of the host name string cannot exceed this value.
 */
static char pxServerHost[ S3_PRESIGNED_GET_URL_LENGTH ];

/**
 * @brief The length of the host address found in the pre-signed URL.
 */
static size_t serverHostLength;

/**
 * @brief The location of the path within the pre-signed URL.
 */
static const char * pPath;

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
 * @brief Retrieve the size of the S3 object that is specified in pPath.
 *
 * @param[out] pFileSize The size of the S3 object.
 * @param[in] pTransportInterface The transport interface for making network
 * calls.
 * @param[in] pHost The server host address. This string must be
 * null-terminated.
 * @param[in] hostLen The length of the server host address.
 * @param[in] pPath The Request-URI to the objects of interest. This string
 * should be null-terminated.
 *
 * @return The status of the file size acquisition using a GET request to the
 * server: pdPASS on success, pdFAIL on failure.
 */
static BaseType_t prvGetS3ObjectFileSize( size_t * pFileSize,
                                          const TransportInterface_t * pTransportInterface,
                                          const char * pHost,
                                          size_t hostLen,
                                          const char * pPath );

/**
 * @brief Send multiple HTTP GET requests, based on a specified path, to
 * download a file in chunks from the host S3 server.
 *
 * @param[in] pTransportInterface The transport interface for making network
 * calls.
 * @param[in] pPath The Request-URI to the objects of interest. This string
 * should be null-terminated.
 *
 * @return The status of the file download using multiple GET requests to the
 * server: pdPASS on success, pdFAIL on failure.
 */
static BaseType_t prvDownloadS3ObjectFile( const TransportInterface_t * pTransportInterface,
                                           const char * pPath );

/*-----------------------------------------------------------*/

static BaseType_t prvConnectToServer( NetworkContext_t * pxNetworkContext )
{
    ServerInfo_t xServerInfo = { 0 };
    SocketsConfig_t xSocketsConfig = { 0 };
    BaseType_t xStatus = pdPASS;
    TransportSocketStatus_t xNetworkStatus = TRANSPORT_SOCKET_STATUS_SUCCESS;
    HTTPStatus_t xHTTPStatus = HTTPSuccess;

    /* The location of the host address within the pre-signed URL. */
    const char * pAddress = NULL;

    /* Retrieve the address location and length from democonfigS3_PRESIGNED_GET_URL. */
    xHTTPStatus = getUrlAddress( democonfigS3_PRESIGNED_GET_URL,
                                 S3_PRESIGNED_GET_URL_LENGTH,
                                 &pAddress,
                                 &serverHostLength );

    xStatus = ( xHTTPStatus == HTTPSuccess ) ? pdPASS : pdFAIL;

    if( xStatus == pdPASS )
    {
        /* pxServerHost should consist only of the host address located in
         * democonfigS3_PRESIGNED_GET_URL. */
        memcpy( pxServerHost, pAddress, serverHostLength );
        pxServerHost[ serverHostLength ] = '\0';

        /* Initializer server information. */
        xServerInfo.pHostName = pxServerHost;
        xServerInfo.hostNameLength = serverHostLength;
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
         * democonfigHTTPS_PORT in demo_config.h. */
        LogInfo( ( "Establishing a TLS session with %s:%d.",
                   pxServerHost,
                   democonfigHTTPS_PORT ) );

        /* Attempt to create a server-authenticated TLS connection. */
        xNetworkStatus = SecureSocketsTransport_Connect( pxNetworkContext,
                                                         &xServerInfo,
                                                         &xSocketsConfig );

        if( xNetworkStatus != TRANSPORT_SOCKET_STATUS_SUCCESS )
        {
            xStatus = pdFAIL;
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prvGetS3ObjectFileSize( size_t * pFileSize,
                                          const TransportInterface_t * pTransportInterface,
                                          const char * pHost,
                                          size_t hostLen,
                                          const char * pPath )
{
    BaseType_t xStatus = pdPASS;
    HTTPStatus_t xHTTPStatus = HTTPSuccess;

    /* The location of the file size in pContentRangeValStr. */
    char * pFileSizeStr = NULL;

    /* String to store the Content-Range header value. */
    char * pContentRangeValStr = NULL;
    size_t contentRangeValStrLength = 0;

    configASSERT( pHost != NULL );
    configASSERT( pPath != NULL );

    /* Initialize all HTTP Client library API structs to 0. */
    ( void ) memset( &xRequestHeaders, 0, sizeof( xRequestHeaders ) );
    ( void ) memset( &xRequestInfo, 0, sizeof( xRequestInfo ) );
    ( void ) memset( &xResponse, 0, sizeof( xResponse ) );

    /* Initialize the request object. */
    xRequestInfo.pHost = pHost;
    xRequestInfo.hostLen = hostLen;
    xRequestInfo.pMethod = HTTP_METHOD_GET;
    xRequestInfo.methodLen = sizeof( HTTP_METHOD_GET ) - 1;
    xRequestInfo.pPath = pPath;
    xRequestInfo.pathLen = strlen( pPath );

    /* Set "Connection" HTTP header to "keep-alive" so that multiple requests
     * can be sent over the same established TCP connection. This is done in
     * order to download the file in parts. */
    xRequestInfo.reqFlags = HTTP_REQUEST_KEEP_ALIVE_FLAG;

    /* Set the buffer used for storing request headers. */
    xRequestHeaders.pBuffer = xUserBuffer;
    xRequestHeaders.bufferLen = democonfigUSER_BUFFER_LENGTH;

    /* Initialize the response object. The same buffer used for storing request
     * headers is reused here. */
    xResponse.pBuffer = xUserBuffer;
    xResponse.bufferLen = democonfigUSER_BUFFER_LENGTH;

    LogInfo( ( "Getting file object size from host..." ) );

    xHTTPStatus = HTTPClient_InitializeRequestHeaders( &xRequestHeaders,
                                                       &xRequestInfo );

    if( xHTTPStatus != HTTPSuccess )
    {
        LogError( ( "Failed to initialize HTTP request headers: Error=%s.",
                    HTTPClient_strerror( xHTTPStatus ) ) );
        xStatus = pdFAIL;
    }

    if( xStatus == pdPASS )
    {
        /* Add the header to get bytes=0-0. S3 will respond with a Content-Range
         * header that contains the size of the file in it. This header will
         * look like: "Content-Range: bytes 0-0/FILESIZE". The body will have a
         * single byte that we are ignoring. */
        xHTTPStatus = HTTPClient_AddRangeHeader( &xRequestHeaders, 0, 0 );

        if( xHTTPStatus != HTTPSuccess )
        {
            LogError( ( "Failed to add range header to request headers: Error=%s.",
                        HTTPClient_strerror( xHTTPStatus ) ) );
            xStatus = pdFAIL;
        }
    }

    if( xStatus == pdPASS )
    {
        /* Send the request and receive the response. */
        xHTTPStatus = HTTPClient_Send( pTransportInterface,
                                       &xRequestHeaders,
                                       NULL,
                                       0,
                                       &xResponse,
                                       0 );

        if( xHTTPStatus != HTTPSuccess )
        {
            LogError( ( "Failed to send HTTP GET request to %s%s: Error=%s.",
                        pHost, pPath, HTTPClient_strerror( xHTTPStatus ) ) );
            xStatus = pdFAIL;
        }
    }

    if( xStatus == pdPASS )
    {
        LogDebug( ( "Received HTTP response from %s%s...",
                    pHost, pPath ) );
        LogDebug( ( "Response Headers:\n%.*s",
                    ( int32_t ) xResponse.headersLen,
                    xResponse.pHeaders ) );
        LogDebug( ( "Response Body:\n%.*s\n",
                    ( int32_t ) xResponse.bodyLen,
                    xResponse.pBody ) );

        if( xResponse.statusCode != HTTP_STATUS_CODE_PARTIAL_CONTENT )
        {
            LogError( ( "Received an invalid response from the server "
                        "(Status Code: %u).",
                        xResponse.statusCode ) );
            xStatus = pdFAIL;
        }
    }

    if( xStatus == pdPASS )
    {
        LogInfo( ( "Received successful response from server "
                   "(Status Code: %u).",
                   xResponse.statusCode ) );

        xHTTPStatus = HTTPClient_ReadHeader( &xResponse,
                                             ( char * ) HTTP_CONTENT_RANGE_HEADER_FIELD,
                                             ( size_t ) HTTP_CONTENT_RANGE_HEADER_FIELD_LENGTH,
                                             ( const char ** ) &pContentRangeValStr,
                                             &contentRangeValStrLength );

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
        pFileSizeStr = strstr( pContentRangeValStr, "/" );

        if( pFileSizeStr == NULL )
        {
            LogError( ( "'/' not present in Content-Range header value: %s.",
                        pContentRangeValStr ) );
            xStatus = pdFAIL;
        }
    }

    if( xStatus == pdPASS )
    {
        pFileSizeStr += sizeof( char );
        *pFileSize = ( size_t ) strtoul( pFileSizeStr, NULL, 10 );

        if( ( *pFileSize == 0 ) || ( *pFileSize == UINT32_MAX ) )
        {
            LogError( ( "Error using strtoul to get the file size from %s: fileSize=%d.",
                        pFileSizeStr, ( int32_t ) *pFileSize ) );
            xStatus = pdFAIL;
        }
    }

    if( xStatus == pdPASS )
    {
        LogInfo( ( "The file is %d bytes long.", ( int32_t ) *pFileSize ) );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prvDownloadS3ObjectFile( const TransportInterface_t * pTransportInterface,
                                           const char * pPath )
{
    /* Return value of this method. */
    BaseType_t xStatus = pdFAIL;
    HTTPStatus_t xHTTPStatus = HTTPSuccess;

    /* The size of the file we are trying to download in S3. */
    size_t fileSize = 0;

    /* The number of bytes we want to request with in each range of the file
     * bytes. */
    size_t numReqBytes = 0;
    /* curByte indicates which starting byte we want to download next. */
    size_t curByte = 0;

    configASSERT( pPath != NULL );

    /* Initialize all HTTP Client library API structs to 0. */
    ( void ) memset( &xRequestHeaders, 0, sizeof( xRequestHeaders ) );
    ( void ) memset( &xRequestInfo, 0, sizeof( xRequestInfo ) );
    ( void ) memset( &xResponse, 0, sizeof( xResponse ) );

    /* Initialize the request object. */
    xRequestInfo.pHost = pxServerHost;
    xRequestInfo.hostLen = serverHostLength;
    xRequestInfo.pMethod = HTTP_METHOD_GET;
    xRequestInfo.methodLen = HTTP_METHOD_GET_LENGTH;
    xRequestInfo.pPath = pPath;
    xRequestInfo.pathLen = strlen( pPath );

    /* Set "Connection" HTTP header to "keep-alive" so that multiple requests
     * can be sent over the same established TCP connection. This is done in
     * order to download the file in parts. */
    xRequestInfo.reqFlags = HTTP_REQUEST_KEEP_ALIVE_FLAG;

    /* Set the buffer used for storing request headers. */
    xRequestHeaders.pBuffer = xUserBuffer;
    xRequestHeaders.bufferLen = democonfigUSER_BUFFER_LENGTH;

    /* Initialize the response object. The same buffer used for storing request
     * headers is reused here. */
    xResponse.pBuffer = xUserBuffer;
    xResponse.bufferLen = democonfigUSER_BUFFER_LENGTH;

    /* Verify the file exists by retrieving the file size. */
    xStatus = prvGetS3ObjectFileSize( &fileSize,
                                      pTransportInterface,
                                      pxServerHost,
                                      serverHostLength,
                                      pPath );

    if( fileSize < democonfigRANGE_REQUEST_LENGTH )
    {
        numReqBytes = fileSize;
    }
    else
    {
        numReqBytes = democonfigRANGE_REQUEST_LENGTH;
    }

    /* Here we iterate sending byte range requests until the full file has been
     * downloaded. We keep track of the next byte to download with curByte. When
     * this reaches the fileSize we stop downloading. */
    while( ( xStatus == pdPASS ) && ( xHTTPStatus == HTTPSuccess ) && ( curByte < fileSize ) )
    {
        xHTTPStatus = HTTPClient_InitializeRequestHeaders( &xRequestHeaders,
                                                           &xRequestInfo );

        if( xHTTPStatus == HTTPSuccess )
        {
            xHTTPStatus = HTTPClient_AddRangeHeader( &xRequestHeaders,
                                                     curByte,
                                                     curByte + numReqBytes - 1 );
        }
        else
        {
            LogError( ( "Failed to initialize HTTP request headers: Error=%s.",
                        HTTPClient_strerror( xHTTPStatus ) ) );
        }

        if( xHTTPStatus == HTTPSuccess )
        {
            LogInfo( ( "Downloading bytes %d-%d, out of %d total bytes, from %s...:  ",
                       ( int32_t ) ( curByte ),
                       ( int32_t ) ( curByte + numReqBytes - 1 ),
                       ( int32_t ) fileSize,
                       pxServerHost ) );
            LogDebug( ( "Request Headers:\n%.*s",
                        ( int32_t ) xRequestHeaders.headersLen,
                        ( char * ) xRequestHeaders.pBuffer ) );
            xHTTPStatus = HTTPClient_Send( pTransportInterface,
                                           &xRequestHeaders,
                                           NULL,
                                           0,
                                           &xResponse,
                                           0 );
        }
        else
        {
            LogError( ( "Failed to add Range header to request headers: Error=%s.",
                        HTTPClient_strerror( xHTTPStatus ) ) );
        }

        if( xHTTPStatus == HTTPSuccess )
        {
            LogDebug( ( "Received HTTP response from %s%s...",
                        pxServerHost, pPath ) );
            LogDebug( ( "Response Headers:\n%.*s",
                        ( int32_t ) xResponse.headersLen,
                        xResponse.pHeaders ) );
            LogInfo( ( "Response Body:\n%.*s\n",
                       ( int32_t ) xResponse.bodyLen,
                       xResponse.pBody ) );

            /* We increment by the content length because the server may not
             * have sent us the range we request. */
            curByte += xResponse.contentLength;

            if( ( fileSize - curByte ) < numReqBytes )
            {
                numReqBytes = fileSize - curByte;
            }

            xStatus = ( xResponse.statusCode == HTTP_STATUS_CODE_PARTIAL_CONTENT ) ? pdPASS : pdFAIL;
        }
        else
        {
            LogError( ( "An error occured in downloading the file. "
                        "Failed to send HTTP GET request to %s%s: Error=%s.",
                        pxServerHost, pPath, HTTPClient_strerror( xHTTPStatus ) ) );
        }

        if( xStatus != pdPASS )
        {
            LogError( ( "Received an invalid response from the server "
                        "(Status Code: %u).",
                        xResponse.statusCode ) );
        }
    }

    return( ( xStatus == pdPASS ) && ( xHTTPStatus == HTTPSuccess ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Entry point of demo.
 *
 * This example, using a pre-signed URL,  resolves a S3 domain, establishes a
 * TCP connection, validates the server's certificate using the root CA
 * certificate defined in the config header, then finally performs a TLS
 * handshake with the HTTP server so that all communication is encrypted. After
 * which, the HTTP Client library API is used to download the S3 file (by
 * sending multiple GET requests, filling up the response buffer each time until
 * all parts are downloaded). If any request fails, an error code is returned.
 *
 * @note This example is single-threaded and uses statically allocated memory.
 *
 */
int RunCoreHttpS3DownloadDemo( bool awsIotMqttMode,
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

    /* The length of the path within the pre-signed URL. This variable is
     * defined in order to store the length returned from parsing the URL, but
     * it is unused. The path used for the requests in this demo needs all the
     * query information following the location of the object, to the end of the
     * S3 presigned URL. */
    size_t pathLen = 0;

    LogInfo( ( "HTTP Client Synchronous S3 download demo using pre-signed URL:\n%s",
               democonfigS3_PRESIGNED_GET_URL ) );

    /**************************** Connect. ******************************/

    /* Establish TLS connection on top of TCP connection using Secure Sockets. */
    if( xDemoStatus == pdPASS )
    {
        /* Attempt to connect to S3. If connection fails, retry after a timeout.
         * The timeout value will be exponentially increased until either the
         * maximum number of attempts or the maximum timeout value is reached.
         * The function returns EXIT_FAILURE if the TCP connection cannot be
         * established to the broker after the configured number of attempts. */
        xDemoStatus = connectToServerWithBackoffRetries( prvConnectToServer,
                                                         &xNetworkContext );

        if( xDemoStatus == pdFAIL )
        {
            /* Log an error to indicate connection failure after all
             * reconnect attempts are over. */
            LogError( ( "Failed to connect to HTTP server %s.",
                        pxServerHost ) );
        }
    }

    /* Define the transport interface. */
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

    /******************** Download S3 Object File. **********************/

    if( xDemoStatus == pdPASS )
    {
        /* Retrieve the path location from democonfigS3_PRESIGNED_GET_URL. This
         * function returns the length of the path without the query into
         * pathLen, which is left unused in this demo. */
        xHTTPStatus = getUrlPath( democonfigS3_PRESIGNED_GET_URL,
                                  S3_PRESIGNED_GET_URL_LENGTH,
                                  &pPath,
                                  &pathLen );

        xDemoStatus = ( xHTTPStatus == HTTPSuccess ) ? pdPASS : pdFAIL;
    }

    if( xDemoStatus == pdPASS )
    {
        xDemoStatus = prvDownloadS3ObjectFile( &xTransportInterface,
                                               pPath );
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

    return ( xDemoStatus == pdPASS ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
