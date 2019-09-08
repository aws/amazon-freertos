/*
 * Amazon FreeRTOS V201908.00
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file iot_demo_https_async.c
 * @brief Demonstrates asynchronous usage of the HTTPS library by performing a partial content GET request on a file
 * from S3.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* C Standard includes. */
#include <stdbool.h>
#include <string.h>

/* Set up logging for this demo. */
#include "iot_demo_logging.h"

/* Amazon FreeRTOS includes. */
#include "iot_demo_https_common.h"
#include "iot_https_utils.h"
#include "aws_demo_config.h"
#include "platform/iot_network.h"
#include "platform/iot_threads.h"
#include "private/iot_error.h"
#include "platform/iot_clock.h"

/**
 * This demonstrates downloading a file from S3 using a pre-signed URL using the Amazon FreeRTOS HTTP Client library.
 * The HTTPS Client library is a generic HTTP/1.1 client library that be used to download files from other webservers as
 * well.
 *
 * A presigned URL is required to run this demo. Please see the demos/https/README.md for instructions on how to
 * generate one.
 *
 * The file is downloaded incrementally using HTTP Partial Content headers. This is done by requesting ranges of the
 * bytes in a file with the header: "Range: bytes=N-M", where N is the starting range and M is the ending range. The
 * S3 HTTP server will response with a 206 Partial Content type of response and the file byte range requested. Please
 * note that not all HTTP servers support a Partial Content download with a byte range.
 */

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default values for undefined configuration settings. You can see these configurations in iot_config.h.
 */

/* Presigned URL for S3 GET Object access. */
#ifndef IOT_DEMO_HTTPS_PRESIGNED_GET_URL
    #define IOT_DEMO_HTTPS_PRESIGNED_GET_URL    "Please configure a presigned GET URL in iot_config.h."
#endif

/* TLS port for HTTPS. */
#ifndef IOT_DEMO_HTTPS_PORT
    #define IOT_DEMO_HTTPS_PORT    ( ( uint16_t ) 443 )
#endif

#ifndef IOT_DEMO_HTTPS_TRUSTED_ROOT_CA
    /* This the Baltimore Cybertrust associated with the S3 server certificate. */
    #define IOT_DEMO_HTTPS_TRUSTED_ROOT_CA                               \
    "-----BEGIN CERTIFICATE-----\n"                                      \
    "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n" \
    "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n" \
    "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n" \
    "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n" \
    "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n" \
    "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n" \
    "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n" \
    "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n" \
    "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n" \
    "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n" \
    "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n" \
    "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n" \
    "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n" \
    "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n" \
    "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n" \
    "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n" \
    "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n" \
    "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n" \
    "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"                             \
    "-----END CERTIFICATE-----\n"
#endif /* ifndef IOT_DEMO_HTTPS_TRUSTED_ROOT_CA */

/* The total number of requests in the pool of HTTPS Client library configurations and handles. In order to send
 * a request asynchronously the memory for both the request buffers and the response buffers must not be shared between
 * other asynchronous requests on the same connection. You can reuse the buffer only after the request/response process
 * has been finished. It is finished when the responseCompleteCallback() is invoked. We create a pool of memory so that
 * all available requests in the pool can be scheduled right away without over-writing each other. */
#ifndef IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS
    #define IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS    ( ( int ) 3 )
#endif

/* Size in bytes of the User Buffer used to store the internal connection context. The size presented here accounts for
 * storage of the internal connection context. The minimum size can be found in extern const unint32_t
 * connectionUserBufferMinimumSize. */
#ifndef IOT_DEMO_HTTPS_CONN_BUFFER_SIZE
    #define IOT_DEMO_HTTPS_CONN_BUFFER_SIZE    ( ( int ) 400 )
#endif

/* Size in bytes of the user buffer used to store the internal request context and HTTP request header lines.
 * The size presented here accounts for the storage of the internal context, the first request line in the HTTP
 * formatted header and extra headers. The minimum size can be found in extern const uint32_t
 * requestUserBufferMinimumSize. Keep in mind that this requestUserBufferMinimumSize does not include the size of the
 * path in the request line. The path could be well over 100 characters long as it includes not only the object key name
 * in S3, but also the query following. The query following has the AWSAccessKeyId, the expiration time, and the
 * AWS Signature Version 4 signature.*/
#ifndef IOT_DEMO_HTTPS_REQ_USER_BUFFER_SIZE
    #define IOT_DEMO_HTTPS_REQ_USER_BUFFER_SIZE    ( ( int ) 512 )
#endif

/* Size in bytes of the user buffer used to store the internal response context and the HTTP response header lines.
 * The size presented here accounts for the storage of the internal context, the first request line in the HTTP
 * formatted header and extra headers. The minimum can be found in responseUserBufferMinimumSize.
 * Keep in mind that if the headers from the response do not all fit into this buffer, then the rest of the headers
 * will be discarded. The minimum size can be found in extern const uint32_t responseUserBufferMinimumSize. */
#ifndef IOT_DEMO_HTTPS_RESP_USER_BUFFER_SIZE
    #define IOT_DEMO_HTTPS_RESP_USER_BUFFER_SIZE    ( ( int ) 1024 )
#endif

/* Size in bytes of the buffer used to store the response body (parts of it). This should be greater than or equal to
 * the size of the file we want to download.*/
#ifndef IOT_DEMO_HTTPS_RESP_BODY_BUFFER_SIZE
    #define IOT_DEMO_HTTPS_RESP_BODY_BUFFER_SIZE    ( ( int ) 512 )
#endif

/* Timeout in milliseconds to wait for all asynchronous requests to finish. This timeout starts when the last
 * IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS sent has been scheduled. */
#ifndef IOT_HTTPS_DEMO_ASYNC_TIMEOUT_MS
    #define IOT_HTTPS_DEMO_ASYNC_TIMEOUT_MS    ( ( uint32_t ) 300000 )   /* 5 minute timeout for this demo. */
#endif

/* Time to wait in milliseconds before retrying the HTTPS Connection. A connection is only attempted again if
* IOT_HTTPS_CONNECTION_ERROR is returned from IotHttpsClient_Connect(). This indicates an error in the network
* layer. To view logging for network errors update IOT_LOG_LEVEL_NETWORK to IOT_LOG_ERROR in iot_config.h. */
#ifndef IOT_DEMO_HTTPS_CONNECTION_RETRY_WAIT_MS
    #define IOT_DEMO_HTTPS_CONNECTION_RETRY_WAIT_MS    ( ( uint32_t ) 3000 )
#endif

/* Number of times to retry the HTTPS connection. A connection is only attempted again if
 * IOT_HTTPS_CONNECTION_ERROR is returned from IotHttpsClient_Connect(). This indicates an error in the network
 * layer. To view logging for network errors update IOT_LOG_LEVEL_NETWORK to IOT_LOG_ERROR in iot_config.h. */
#ifndef IOT_DEMO_HTTPS_CONNECTION_NUM_RETRY
    #define IOT_DEMO_HTTPS_CONNECTION_NUM_RETRY    ( ( uint32_t ) 3 )
#endif

/** @endcond */

/**
 * @brief The time for the application task to wait to try again finding a free request from the pool of requests.
 */
#define GET_FREE_REQUEST_RETRY_WAIT_TIME_MS    ( ( uint32_t ) 300 )

/**
 * @brief The log2 of the number of bits in a uint8_t.
 *
 * This is for calculation of how many uint8_t words there should be in the bitmap.
 */
#define LOG2_BITS_PER_BYTE                     ( ( uint32_t ) 3 )
#define BITS_PER_BYTE                          ( ( ( uint32_t ) 1 ) << LOG2_BITS_PER_BYTE ) /**< @brief The number of bits in a byte. */

/**
 * @brief Bitmask calculation for setting the file download information bitmaps.
 */
#define BITMASK( rangeBlock )        ( ( ( uint32_t ) 1 ) << ( ( ( uint32_t ) rangeBlock ) % BITS_PER_BYTE ) )

/**
 * @brief Byte offset calculation for indexing to the correct byte in the file download information bitmap.
 */
#define BYTE_OFFSET( rangeBlock )    ( ( ( uint32_t ) rangeBlock ) >> LOG2_BITS_PER_BYTE )

/*-----------------------------------------------------------*/

/**
 * @brief This structure defines data consumed and kept track of in the async callbacks per request.
 */
typedef struct _requestData
{
    char rangeValueStr[ RANGE_VALUE_MAX_LENGTH ]; /**< @brief This string is generated outside of the callback context and used to set the request Range header within the callback context. */
    int reqNum;                                   /**< @brief This is the current request number in the static pool of request memory. */
    uint32_t currRange;                           /**< @brief This is the current range block that is being downloaded. */
    uint32_t currDownloaded;                      /**< @brief The current number of bytes downloaded in this request. */
    uint32_t numReqBytes;                         /**< @brief The number of bytes we want to downloaded in this request. */
    IotHttpsConnectionHandle_t * pConnHandle;     /**< @brief The connection handle for the request/response. */
    IotHttpsConnectionInfo_t * pConnConfig;       /**< @brief Pointer to the connection handle for the request/response. */
    bool scheduled;                               /**< @brief This is set to true when the request has been scheduled */
} _requestData_t;

/**
 * @brief This structure defines information related to the file we wish to download.
 */
typedef struct _fileDownloadInfo
{
    uint32_t fileSize;          /**< @brief The total size of the file to be downloaded. */
    int32_t totalRanges;        /**< @brief the total number of ranges in the file size. */
    uint8_t * downloadedBitmap; /**< @brief Bitmap to keep track of chunks downlaoded. */
    uint8_t * scheduledBitmap;  /**< @brief Bitmap of ranges scheduled, but not completed. */
    int32_t rangesRemaining;    /**< @brief The total number of ranges remaining to download. */
} _fileDownloadInfo_t;

/*-----------------------------------------------------------*/

/**
 * @brief Buffer to store parts of the response body.
 *
 * Since all of the requests in this demo are on the same connection, only one async task at a time will be accessing
 * this buffer to retrieve data from the network to print.
 */
static uint8_t _pRespBodyBuffer[ IOT_DEMO_HTTPS_RESP_BODY_BUFFER_SIZE ] = { 0 };

/**
 * @brief Semaphore use to signal that the demo is finished.
 * The task that downloads the final increment of the file posts to this semaphore.
 */
static IotSemaphore_t _fileFinishedSem = { 0 };

/**
 * @brief Array to keep track of HTTPS Client library request and response resources (buffers, handles, and configs)
 * in use.
 */
static bool _pInUseRequests[ IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS ] = { 0 };

/**
 * @brief Mutex to protect retrieving and setting the requests in use.
 *
 * This protects the buffers from being reused.
 */
static IotMutex_t _inUseRequestsMutex = { 0 };

/**
 * @brief Mutex protecting the scheduling of requests.
 *
 * This protects scheduling activities. If The connection is closed by the server, then the asynchronous callback
 * context will reschedule requests that have not been sent yet. The demo application thread also schedules requests
 * when there is a request user buffer available. This mutex ensures requests are not schedule multiple times.
 */
static IotMutex_t _requestSchedulingMutex = { 0 };

/**
 * @brief Configurations for the HTTPS connection. '
 */
static IotHttpsConnectionInfo_t _connConfig = { 0 };

/**
 * @brief The pool of asynchronous request specific configurations.
 */
static IotHttpsAsyncInfo_t _pAsyncInfo[ IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS ] = { 0 };

/**
 * @brief The pool of HTTPS Client library request configurations.
 */
static IotHttpsRequestInfo_t _pReqConfigs[ IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS ] = { 0 };

/**
 * @brief The pool of HTTPS Client library response configurations.
 */
static IotHttpsResponseInfo_t _pRespConfigs[ IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS ] = { 0 };

/**
 * @brief Handle identifying the HTTPS connection.
 */
static IotHttpsConnectionHandle_t _connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;

/**
 * @brief The pool of HTTPS Client library request handles.
 */
static IotHttpsRequestHandle_t _pReqHandles[ IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS ] = { 0 };

/**
 * @brief The pool of HTTPS Client library response handles.
 */
static IotHttpsResponseHandle_t _pRespHandles[ IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS ] = { 0 };

/**
 * @brief Connection buffer use to store the internal connection context.
 */
static uint8_t _pConnUserBuffer[ IOT_DEMO_HTTPS_CONN_BUFFER_SIZE ] = { 0 };

/**
 * @brief The pool of buffers used to store the request context and the HTTP request header lines.
 */
static uint8_t _pReqUserBuffers[ IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS ][ IOT_DEMO_HTTPS_REQ_USER_BUFFER_SIZE ] = { 0 };

/**
 * @brief The pool of buffers used to store the response context and the HTTP response header lines.
 */
static uint8_t _pRespUserBuffers[ IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS ][ IOT_DEMO_HTTPS_RESP_USER_BUFFER_SIZE ] = { 0 };

/**
 * @brief The pool of data kept track of and used in the callback context for each request.
 */
static _requestData_t _pRequestDatas[ IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS ] = { 0 };

/**
 * @brief Information about the file that is to be downloaded.
 */
static _fileDownloadInfo_t _fileDownloadInfo = { 0 };

/*-----------------------------------------------------------*/

/* Declaration of the demo function. */
int RunHttpsAsyncDownloadDemo( bool awsIotMqttMode,
                               const char * pIdentifier,
                               void * pNetworkServerInfo,
                               void * pNetworkCredentialInfo,
                               const IotNetworkInterface_t * pNetworkInterface );

/*-----------------------------------------------------------*/

/**
 * @brief Get an index to a free HTTPS Client library request resource.
 *
 * The resource pool is defined above in the static variables section.
 *
 * @return The free index if one is found. -1 if one is not found.
 */
static int _getFreeRequestIndex( void )
{
    int i = 0;
    int freeIndex = -1;

    IotMutex_Lock( &( _inUseRequestsMutex ) );

    for( i = 0; i < IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS; i++ )
    {
        if( !_pInUseRequests[ i ] )
        {
            _pInUseRequests[ i ] = true;
            freeIndex = i;
            break;
        }
    }

    IotMutex_Unlock( &( _inUseRequestsMutex ) );

    /* If we exited the loop, then none were found. */
    return freeIndex;
}

/*-----------------------------------------------------------*/

/**
 * @brief Clear data for the request in preparation for the next request to use.
 *
 * User buffers and body buffers are zeroed out by the HTTPS Client library.
 */
static void _clearRequestData( int i )
{
    memset( &_pRequestDatas[ i ], 0, sizeof( _requestData_t ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Free an index in the pool HTTPS Client library request resource.
 *
 * The resource pool is defined above in the static variables section.
 *
 * @param[in] i The index in the request pool to mark as free for use.
 */
static void _freeRequestIndex( int i )
{
    IotMutex_Lock( &( _inUseRequestsMutex ) );
    _pInUseRequests[ i ] = false;
    _clearRequestData( i );
    IotMutex_Unlock( &( _inUseRequestsMutex ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Free all requests in the pool that have been marked as scheduled.
 */
static void _freeAllScheduledRequests( void )
{
    /* Index into the request pool. */
    int i = 0;

    IotMutex_Lock( &( _inUseRequestsMutex ) );

    for( i = 0; i < IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS; i++ )
    {
        if( _pRequestDatas[ i ].scheduled == true )
        {
            _pInUseRequests[ i ] = false;
            _clearRequestData( i );
        }
    }

    IotMutex_Unlock( &( _inUseRequestsMutex ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Clear all scheduled ranges in the bitmap for the file to download.
 */
static void _clearAllScheduledRanges( void )
{
    /* The current range to check if it is scheduled. */
    int32_t rangeIndex = 0;
    /* Bit mask to check in the scheduled bit map if the range has been scheduled. */
    uint32_t bitMask = 0;
    /* Byte offset into the scheduled bit map where the range will be located. */
    uint32_t byteOffset = 0;

    for( rangeIndex = 0; rangeIndex < _fileDownloadInfo.totalRanges; rangeIndex++ )
    {
        bitMask = BITMASK( rangeIndex );
        byteOffset = BYTE_OFFSET( rangeIndex );

        if( ( _fileDownloadInfo.scheduledBitmap[ byteOffset ] & bitMask ) != ( ( uint8_t ) 0 ) )
        {
            _fileDownloadInfo.scheduledBitmap[ byteOffset ] &= ( ~( bitMask ) );
        }
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Callback for an asynchronous request to append the Range header to the request header buffer.
 *
 * @param[in] pPrivData - User private data configured with the HTTPS Client library request configuration.
 * @param[in] reqHandle - Identifier for the current request in progress.
 */
static void _appendHeaderCallback( void * pPrivData,
                                   IotHttpsRequestHandle_t reqHandle )
{
    /* The range value string, of the increment of file to download, from the user private data will be referenced with
     * this variable. */
    char * rangeValueStr = ( ( _requestData_t * ) ( pPrivData ) )->rangeValueStr;

    IotLogInfo( "Inside of the append header callback for part %s", rangeValueStr );
    /* The length of the range value string. */
    uint32_t rangeValueLen = strlen( rangeValueStr );
    /* Set the header for a range request and check the HTTPS Client library return code. */
    IotHttpsReturnCode_t status = IotHttpsClient_AddHeader( reqHandle, RANGE_HEADER_FIELD, RANGE_HEADER_FIELD_LENGTH, rangeValueStr, rangeValueLen );

    if( status != IOT_HTTPS_OK )
    {
        IotLogError( "Failed to write the header Range: %.*s into the request. With error code: %d", rangeValueLen, rangeValueStr, status );
        IotHttpsClient_CancelRequestAsync( reqHandle );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Callback for an asynchronous request to read data from the network.
 *
 * @note If you wanted to read any headers you must do so in this callback.
 *
 * @param[in] pPrivData - User private data configured with the HTTPS Client library request configuration.
 * @param[in] respHandle - Identifier for the current response in progress.
 * @param[in] rc - Return code from the HTTPS Client Library signaling a possible error.
 * @param[in] status - The HTTP response status.
 */
static void _readReadyCallback( void * pPrivData,
                                IotHttpsResponseHandle_t respHandle,
                                IotHttpsReturnCode_t rc,
                                uint16_t status )
{
    /* The amount of body read during this callback. */
    uint32_t readLen;

    /* The range value string, of the increment of file to download, from the user private data will be referenced with
     * this variable. */
    char * rangeValueStr;
    /* The HTTP Client Library return code. */
    IotHttpsReturnCode_t returnStatus;
    /* The user private data dereferenced. */
    _requestData_t * pRequestData = ( _requestData_t * ) ( pPrivData );
    /* The content length of this HTTP response. */
    uint32_t contentLength = 0;

    /* Buffer to read the Connection header value into.  The possible values are "close" and "keep-alive". */
    char connectionValueStr[ RANGE_VALUE_MAX_LENGTH ] = { 0 };

    IotLogInfo( "Inside of the read ready callback for part %s with network return code: %d", pRequestData->rangeValueStr, rc );

    /* If this function is invoked multiple times, we may only want to read the Content-Length header and check the
     * HTTP response status once. */
    if( pRequestData->currDownloaded == 0 )
    {
        /* If do not get a successful partial content delivery, as indicated on the HTTP response code 206, then cancel this
         * request. */
        if( status != IOT_HTTPS_STATUS_PARTIAL_CONTENT )
        {
            IotLogError( "Could not retrieve file from S3. Status code %d", status );
            IotHttpsClient_CancelResponseAsync( respHandle );
            return;
        }

        returnStatus = IotHttpsClient_ReadContentLength( respHandle, &contentLength );

        if( ( returnStatus != IOT_HTTPS_OK ) || ( contentLength == 0 ) )
        {
            IotLogError( "Failed to retrieve the Content-Length from the response. ",
                         "Please try increasing the size of IOT_DEMO_HTTPS_RESP_USER_BUFFER_SIZE." );
            IotHttpsClient_CancelResponseAsync( respHandle );
            return;
        }

        /* If the content length of the message is not equal to the size of the byte range we want to download then
         * cancel the request. */
        if( contentLength != pRequestData->numReqBytes )
        {
            IotLogError( "The Content-Length found in this file does not equal the number of bytes requested. So we may ",
                         "not have download the file completely. The content length is %d and the requested number of bytes for ",
                         "this request is %d", contentLength, pRequestData->numReqBytes );
            IotHttpsClient_CancelResponseAsync( respHandle );
            return;
        }
    }

    /* Get the the amount of data to read. We want to fill the entire response body buffer. */
    readLen = sizeof( _pRespBodyBuffer );

    /* Read the data from the network. */
    returnStatus = IotHttpsClient_ReadResponseBody( respHandle, _pRespBodyBuffer, &readLen );

    if( returnStatus != IOT_HTTPS_OK )
    {
        IotLogError( "Failed to read the response body with error %d", returnStatus );
        IotHttpsClient_CancelResponseAsync( respHandle );
        return;
    }

    /* Process the body here. */
    rangeValueStr = pRequestData->rangeValueStr;
    IotLogInfo( "Response return code: %d for %s", status, rangeValueStr );
    IotLogInfo( "Response Body for %s:\r\n%.*s", rangeValueStr, readLen, _pRespBodyBuffer );

    /* This callback could be invoked again if there is still more data on the network to be read for this response, so
     * we increment the current amount downloaded. */
    pRequestData->currDownloaded += readLen;

    if( pRequestData->currDownloaded > pRequestData->numReqBytes )
    {
        IotLogError( "There is more data received on this response than expected. Received %d data, but requested %d.",
                     pRequestData->currDownloaded,
                     pRequestData->numReqBytes );
        IotHttpsClient_CancelResponseAsync( respHandle );
        return;
    }

    /* Only in the non-error case of the downloaded data equalling the number of requested bytes do we mark that the
     * range has been downloaded. Also in the non-error case it is idea to check if the server closed the connection. */
    if( pRequestData->currDownloaded == pRequestData->numReqBytes )
    {
        _fileDownloadInfo.rangesRemaining--;
        _fileDownloadInfo.downloadedBitmap[ BYTE_OFFSET( pRequestData->currRange ) ] |= BITMASK( pRequestData->currRange );

        /* Check if the server closed the connection. This done in the readReadyCallback() because a possible pending request
         * is being sent right after this response's body is finished being read. This means responseCompleteCallback() is
         * invoked at the same time as another request is sending on the same connetion. This is to increase parallelism in
         * the library. Because the responseCompeteCallback() could be executing at the same time a request is being sent,
         * if the server closed the connection, a reconnection needs to be made before the next request sends and gets a
         * network error. Please see the design flow for more information:
         * https://docs.aws.amazon.com/freertos/latest/lib-ref/https/https_design.html#Asynchronous_Design */
        returnStatus = IotHttpsClient_ReadHeader( respHandle,
                                                  CONNECTION_HEADER_FIELD,
                                                  CONNECTION_HEADER_FILED_LENGTH,
                                                  connectionValueStr,
                                                  sizeof( connectionValueStr ) );

        /* If there is any other error besides "not found", then that is a fatal error. S3's response will include the
         * Connection header only if it closes the connection after the response. */
        if( ( returnStatus != IOT_HTTPS_OK ) && ( returnStatus != IOT_HTTPS_NOT_FOUND ) )
        {
            IotLogError( "Failed to read header %s. Error code: %d.", CONNECTION_HEADER_FIELD, returnStatus );
            IotHttpsClient_CancelResponseAsync( respHandle );
            return;
        }

        if( strncmp( CONNECTION_CLOSE_HEADER_VALUE, connectionValueStr, CONNECTION_CLOSE_HEADER_VALUE_LENGTH ) == 0 )
        {
            IotLogInfo( "The S3 server closed the connection. Reconnecting and rescheduling requests." );
            IotMutex_Lock( &( _requestSchedulingMutex ) );

            /* All requests scheduled on this connection are dropped during a reconnect. Reconnection disconnects
             * a connection that is in the open state. When the connection is disconnected it's queue of pending requests
             * is cleared. Since this affects the state of what is scheduled, this action is performed within the
             * _requestSchedulingMutex. */
            returnStatus = IotHttpsClient_Connect( pRequestData->pConnHandle, pRequestData->pConnConfig );

            if( returnStatus != IOT_HTTPS_OK )
            {
                IotLogError( "Failed to reconnect to the S3 server. Error code: %d.", returnStatus );
                IotHttpsClient_CancelResponseAsync( respHandle );
            }
            else
            {
                /* For all requests from the pool that have been scheduled. We want to free them so they will get
                 * rescheduled by the main application. */
                _freeAllScheduledRequests();

                /* For each of the scheduled ranges mark then as unscheduled so that the main application will assign then
                 * to a free request. This is done during the _responseCompleteCallback(), so that if the range completed,
                 * it will not be rescheduled even if the the scheduled bitmap says it is not scheduled. */
                _clearAllScheduledRanges();
            }

            IotMutex_Unlock( &( _requestSchedulingMutex ) );
        }
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Callback for an asynchronous request to notify that the response is complete.
 *
 * @param[in] pPrivData - User private data configured with the HTTPS Client library request configuration.
 * @param[in] respHandle - Identifier for the current response finished.
 * @param[in] rc - Return code from the HTTPS Client Library signaling a possible error.
 * @param[in] status - The HTTP response status.
 */
static void _responseCompleteCallback( void * pPrivData,
                                       IotHttpsResponseHandle_t respHandle,
                                       IotHttpsReturnCode_t rc,
                                       uint16_t status )
{
    _requestData_t * pRequestData = ( _requestData_t * ) ( pPrivData );

    ( void ) respHandle;
    ( void ) rc;
    ( void ) status;

    IotLogInfo( "Part %s has been fully processed.", pRequestData->rangeValueStr );

    /* If there is an error in the response processing. */
    if( rc != IOT_HTTPS_OK )
    {
        IotLogError( "There was a problem with the current response %d. Error code: %d. ", respHandle, rc );
        IotSemaphore_Post( &( _fileFinishedSem ) );
    }
    else if( pRequestData->currDownloaded != pRequestData->numReqBytes )
    {
        /* If in this response the total amount read does not equal the number of bytes we requested, then something went
         * wrong. */
        IotLogError( "There was a problem downloading the range of the file. We downloaded %d. but wanted %d.",
                     pRequestData->currDownloaded,
                     pRequestData->numReqBytes );
        IotSemaphore_Post( &( _fileFinishedSem ) );
    }
    else
    {
        /* The main application is waiting for the response to finish. We signal it is finished when the number
         * of ranges downloaded equals the total. */
        IotLogInfo( "Downloaded: %d/%d range blocks", _fileDownloadInfo.totalRanges - _fileDownloadInfo.rangesRemaining, _fileDownloadInfo.totalRanges );

        if( _fileDownloadInfo.rangesRemaining <= 0 )
        {
            IotLogDebug( "File fully downloaded. Range blocks downloaded: %d", _fileDownloadInfo.totalRanges - _fileDownloadInfo.rangesRemaining );
            IotSemaphore_Post( &( _fileFinishedSem ) );
        }
    }

    /* Free up this request from the request pool. This is done last to ensure that the pDownload data is not
     * overwritten by a new request. */
    _freeRequestIndex( pRequestData->reqNum );
}

/*-----------------------------------------------------------*/

/**
 * @brief Callback for an asynchronous request to notify that the connection was closed.
 *
 * The connection will close and this callback will be invoked if there are network errors or if there was an error
 * parsing the response.
 * The connection will also close and this callback invoked, if a request is marked as non-persistent in
 * #IotHttpsRequestInfo_t.isNonPersistent.
 * This demo does not send a non-persistent marked request.
 * This callback will not be invoked when the application calls API IotHttpsClient_Disconnect() to disconnect
 * explicitly.
 *
 * @param[in] pPrivData - User private data configured with the HTTPS Client library request configuration.
 * @param[in] connHandle - Identifier for the current connection.
 * @param[in] rc - Return code from the HTTPS Client Library signaling a possible error..
 */
static void _connectionClosedCallback( void * pPrivData,
                                       IotHttpsConnectionHandle_t connHandle,
                                       IotHttpsReturnCode_t rc )
{
    ( void ) pPrivData;
    ( void ) connHandle;
    ( void ) rc;
    IotLogInfo( "Connection with the s3 server has been closed." );
}

/*-----------------------------------------------------------*/

/**
 * @brief Callback to notify of errors that occurred during this asynchronous request.
 *
 * @param[in] pPrivData - User private data configured with the HTTPS Client library request configuration.
 * @param[in] reqHandle - Identifier for the request.
 * @param[in] rc - Return code from the HTTPS Client Library of the error.
 */
static void _errorCallback( void * pPrivData,
                            IotHttpsRequestHandle_t reqHandle,
                            IotHttpsResponseHandle_t respHandle,
                            IotHttpsReturnCode_t rc )
{
    ( void ) reqHandle;
    char * rangeValueStr = ( ( _requestData_t * ) ( pPrivData ) )->rangeValueStr;
    IotLogError( "An error occurred during asynchronous operation with code: %d", rangeValueStr, rc );
}

/*-----------------------------------------------------------*/

/**
 * @brief Schedule the asynchronous request.
 *
 * This routine will prepare the request then schedule it.
 *
 * @param[in] reqIndex The index into the request resource pool.
 * @param[in] currentRange The current partial file range to request.
 *
 * @return `EXIT_SUCCESS` if the request was successfully scheduled. `EXIT_FAILURE` otherwise.
 */
static int _scheduleAsyncRequest( int reqIndex,
                                  uint32_t currentRange )
{
    IOT_FUNCTION_ENTRY( int, EXIT_SUCCESS );

    /* HTTPS Client library return status. */
    IotHttpsReturnCode_t httpsClientStatus;
    /* The number of bytes we want to request with in each range of the file bytes. */
    uint32_t numReqBytes = 0;
    /* curByte indicates which starting byte we want to download next. */
    uint32_t curByte = 0;

    /* Get the Range header value string. */
    curByte = currentRange * IOT_DEMO_HTTPS_RESP_BODY_BUFFER_SIZE;

    /* The number of bytes we want to request each time is the size of the body buffer or it is the remaining file
     * range left. */
    numReqBytes = IOT_DEMO_HTTPS_RESP_BODY_BUFFER_SIZE;

    if( ( _fileDownloadInfo.fileSize - curByte ) < numReqBytes )
    {
        numReqBytes = _fileDownloadInfo.fileSize - curByte;
    }

    int numWritten = snprintf( _pRequestDatas[ reqIndex ].rangeValueStr,
                               sizeof( _pRequestDatas[ reqIndex ].rangeValueStr ),
                               "bytes=%u-%u",
                               ( unsigned int ) curByte,
                               ( unsigned int ) ( curByte + numReqBytes - 1 ) );

    if( ( numWritten < 0 ) || ( numWritten >= sizeof( _pRequestDatas[ reqIndex ].rangeValueStr ) ) )
    {
        IotLogError( "Failed to write the header value: \"bytes=%d-%d\" . Error code: %d",
                     curByte,
                     curByte + numReqBytes - 1,
                     numWritten );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Set the user private data to use in the asynchronous callback context. */
    _pRequestDatas[ reqIndex ].pConnHandle = &_connHandle;
    _pRequestDatas[ reqIndex ].pConnConfig = &_connConfig;
    _pRequestDatas[ reqIndex ].reqNum = reqIndex;
    _pRequestDatas[ reqIndex ].currRange = currentRange;
    _pRequestDatas[ reqIndex ].currDownloaded = 0;
    _pRequestDatas[ reqIndex ].numReqBytes = numReqBytes;

    /* Re-initialize the request to reuse the request. If we do not reinitialize then data from the last response
     * associated with this request will linger. */
    httpsClientStatus = IotHttpsClient_InitializeRequest( &( _pReqHandles[ reqIndex ] ),
                                                          &( _pReqConfigs[ reqIndex ] ) );

    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "An error occurred in IotHttpsClient_InitializeRequest() with error code: %d",
                     httpsClientStatus );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Send the request and receive the response asynchronously. This will schedule the async request. We
     * will return immediately after scheduling. */
    httpsClientStatus = IotHttpsClient_SendAsync( _connHandle,
                                                  _pReqHandles[ reqIndex ],
                                                  &( _pRespHandles[ reqIndex ] ),
                                                  &( _pRespConfigs[ reqIndex ] ) );
    _fileDownloadInfo.scheduledBitmap[ BYTE_OFFSET( currentRange ) ] |= BITMASK( currentRange );

    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "Failed to send the request asynchronously with error code: %d", httpsClientStatus );
        /* If we failed to schedule an async request then this is an error and we should exit the loop. */
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

/**
 * @brief The function that runs the HTTPS Asynchronous demo.
 *
 * @param[in] awsIotMqttMode Specify if this demo is running with the AWS IoT MQTT server. This is ignored in this demo.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier. This is ignored in this demo.
 * @param[in] pNetworkServerInfo Contains network information specific for the MQTT demo. This is ignored in this demo.
 * @param[in] pNetworkCredentialInfo Contains credential info for a TLS connection.
 * @param[in] pNetworkInterface Network interface to use for this demo.
 *
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE` otherwise.
 */
int RunHttpsAsyncDownloadDemo( bool awsIotMqttMode,
                               const char * pIdentifier,
                               void * pNetworkServerInfo,
                               void * pNetworkCredentialInfo,
                               const IotNetworkInterface_t * pNetworkInterface )
{
    IOT_FUNCTION_ENTRY( int, EXIT_SUCCESS );

    /* Unused parameters. */
    ( void ) awsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;

    /* HTTPS Client library return status. */
    IotHttpsReturnCode_t httpsClientStatus;

    /* The location of the path within string IOT_DEMO_HTTPS_PRESIGNED_GET_URL. */
    const char * pPath = NULL;
    /* The length of the path within string IOT_DEMO_HTTPS_PRESIGNED_GET_URL. */
    size_t pathLen = 0;
    /* The location of the address within string IOT_DEMO_HTTPS_PRESIGNED_GET_URL. */
    const char * pAddress = NULL;
    /* The length of the address within string IOT_DEMO_HTTPS_PRESIGNED_GET_URL. */
    size_t addressLen = 0;

    /* The current attempt in the number of connection tries. */
    uint32_t connAttempt = 0;
    /* The current request index being processed. */
    int reqIndex = 0;
    /* The length of the bitmap for allocating a new bitmap. */
    uint32_t bitmapLength;
    /* The current range that is getting ready to schedule a request for. */
    uint32_t currentRange = 0;
    /* An intermediate bitMask calculation for checking if the currentRange is already downloaded or not. */
    uint32_t bitMask = 0;
    /* An intermediate byteOffset into the _rangeBitMap to check if the currentRange is already downloaded or not. */
    uint32_t byteOffset = 0;

    /* Signal if the global semaphores were created for cleanup. */
    bool inUseRequestMutexCreated = false;
    bool fileFinishedSemCreated = false;
    bool requestSchedulingMutexCreated = false;

    IotLogInfo( "HTTPS Client Asynchronous S3 download demo using pre-signed URL: %s", IOT_DEMO_HTTPS_PRESIGNED_GET_URL );

    /* Retrieve the path location and length from IOT_DEMO_HTTPS_PRESIGNED_GET_URL. */
    httpsClientStatus = IotHttpsClient_GetUrlPath( IOT_DEMO_HTTPS_PRESIGNED_GET_URL, strlen( IOT_DEMO_HTTPS_PRESIGNED_GET_URL ), &pPath, &pathLen );

    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "An error occurred in IotHttpsClient_GetUrlPath() with error code %d.", httpsClientStatus );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* The path is everything that is not the address. It also includes the query. So we get the strlen( pPath ) to
     * acquire everything following in IOT_DEMO_HTTPS_PRESIGNED_GET_URL. */
    pathLen = strlen( pPath );

    /* Retrieve the address location and length from the IOT_DEMO_HTTPS_PRESIGNED_GET_URL. */
    httpsClientStatus = IotHttpsClient_GetUrlAddress( IOT_DEMO_HTTPS_PRESIGNED_GET_URL, strlen( IOT_DEMO_HTTPS_PRESIGNED_GET_URL ), &pAddress, &addressLen );

    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "An error occurred in IotHttpsClient_GetUrlAddress() with error code %d.", httpsClientStatus );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Set the connection configurations. */
    _connConfig.pAddress = pAddress;
    _connConfig.addressLen = addressLen;
    _connConfig.port = IOT_DEMO_HTTPS_PORT;
    _connConfig.pCaCert = IOT_DEMO_HTTPS_TRUSTED_ROOT_CA;
    _connConfig.caCertLen = sizeof( IOT_DEMO_HTTPS_TRUSTED_ROOT_CA );
    _connConfig.userBuffer.pBuffer = _pConnUserBuffer;
    _connConfig.userBuffer.bufferLen = sizeof( _pConnUserBuffer );
    _connConfig.pClientCert = ( ( IotNetworkCredentials_t * ) pNetworkCredentialInfo )->pClientCert;
    _connConfig.clientCertLen = ( ( IotNetworkCredentials_t * ) pNetworkCredentialInfo )->clientCertSize;
    _connConfig.pPrivateKey = ( ( IotNetworkCredentials_t * ) pNetworkCredentialInfo )->pPrivateKey;
    _connConfig.privateKeyLen = ( ( IotNetworkCredentials_t * ) pNetworkCredentialInfo )->privateKeySize;
    _connConfig.pNetworkInterface = pNetworkInterface;

    /* Setup constant request information shared by all requests. */
    for( reqIndex = 0; reqIndex < IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS; reqIndex++ )
    {
        /* Set the HTTP request configurations. */
        _pReqConfigs[ reqIndex ].pPath = pPath;

        _pReqConfigs[ reqIndex ].pathLen = pathLen;
        _pReqConfigs[ reqIndex ].pHost = pAddress;
        _pReqConfigs[ reqIndex ].hostLen = addressLen;
        _pReqConfigs[ reqIndex ].method = IOT_HTTPS_METHOD_GET;
        _pReqConfigs[ reqIndex ].userBuffer.pBuffer = _pReqUserBuffers[ reqIndex ];
        _pReqConfigs[ reqIndex ].userBuffer.bufferLen = sizeof( _pReqUserBuffers[ reqIndex ] );
        _pReqConfigs[ reqIndex ].isAsync = true;

        /* Set the HTTP response configurations. */
        _pRespConfigs[ reqIndex ].userBuffer.pBuffer = _pRespUserBuffers[ reqIndex ];
        _pRespConfigs[ reqIndex ].userBuffer.bufferLen = sizeof( _pRespUserBuffers[ reqIndex ] );
        _pRespConfigs[ reqIndex ].pSyncInfo = NULL;

        /* Set the configurations needed for an asynchronous request. */
        _pAsyncInfo[ reqIndex ].callbacks.appendHeaderCallback = _appendHeaderCallback;
        _pAsyncInfo[ reqIndex ].callbacks.readReadyCallback = _readReadyCallback;
        _pAsyncInfo[ reqIndex ].callbacks.responseCompleteCallback = _responseCompleteCallback;
        _pAsyncInfo[ reqIndex ].callbacks.connectionClosedCallback = _connectionClosedCallback;
        _pAsyncInfo[ reqIndex ].callbacks.errorCallback = _errorCallback;
        _pAsyncInfo[ reqIndex ].pPrivData = ( void * ) ( &( _pRequestDatas[ reqIndex ] ) );
        _pReqConfigs[ reqIndex ].u.pAsyncInfo = &( _pAsyncInfo[ reqIndex ] );
    }

    /* Create the mutex to protect the pool of requests. */
    inUseRequestMutexCreated = IotMutex_Create( &( _inUseRequestsMutex ), false );

    if( inUseRequestMutexCreated == false )
    {
        IotLogError( "Failed to create a mutex to protect the request pool." );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Create the semaphore for waiting for the response to finish. */
    fileFinishedSemCreated = IotSemaphore_Create( &( _fileFinishedSem ), 0 /* Initial count. */, 1 /* Max count. */ );

    if( fileFinishedSemCreated == false )
    {
        IotLogError( "Failed to create a semaphore to wait for the file to finish." );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Create the mutex protecting the file download information. */
    requestSchedulingMutexCreated = IotMutex_Create( &( _requestSchedulingMutex ), false );

    if( requestSchedulingMutexCreated == false )
    {
        IotLogError( "Failed to create a mutex to protect the file download data." );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Initialize the HTTPS library. */
    httpsClientStatus = IotHttpsClient_Init();

    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "An error occurred initializing the HTTPS library. Error code: %d", httpsClientStatus );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Connect to S3. */
    for( connAttempt = 1; connAttempt <= IOT_DEMO_HTTPS_CONNECTION_NUM_RETRY; connAttempt++ )
    {
        httpsClientStatus = IotHttpsClient_Connect( &_connHandle, &_connConfig );

        if( ( httpsClientStatus == IOT_HTTPS_CONNECTION_ERROR ) &&
            ( connAttempt < IOT_DEMO_HTTPS_CONNECTION_NUM_RETRY ) )
        {
            IotLogError( "Failed to connect to the S3 server, retrying after %d ms.",
                         IOT_DEMO_HTTPS_CONNECTION_RETRY_WAIT_MS );
            IotClock_SleepMs( IOT_DEMO_HTTPS_CONNECTION_RETRY_WAIT_MS );
            continue;
        }
        else
        {
            break;
        }
    }

    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "Failed to connect to the S3 server. Error code: %d.", httpsClientStatus );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Retrieve the size of the file specified in the S3 pre-signed URL. */
    if( _IotHttpsDemo_GetS3ObjectFileSize( &_fileDownloadInfo.fileSize,
                                           _connHandle,
                                           pPath,
                                           strlen( pPath ),
                                           pAddress,
                                           addressLen,
                                           _pReqUserBuffers[ 0 ],
                                           IOT_DEMO_HTTPS_REQ_USER_BUFFER_SIZE,
                                           _pRespUserBuffers[ 0 ],
                                           IOT_DEMO_HTTPS_RESP_USER_BUFFER_SIZE ) != EXIT_SUCCESS )
    {
        IotLogError( "Failed to retrieve the s3 object size." );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Allocate the bitmap. */

    /* Cet the total number of ranges needed to download the file. */
    _fileDownloadInfo.totalRanges = ( _fileDownloadInfo.fileSize + IOT_DEMO_HTTPS_RESP_BODY_BUFFER_SIZE - 1 ) / IOT_DEMO_HTTPS_RESP_BODY_BUFFER_SIZE;
    _fileDownloadInfo.rangesRemaining = _fileDownloadInfo.totalRanges;
    /* Calculate the length of the rangeBitmap to keep track of already downloaded ranges. */
    bitmapLength = ( _fileDownloadInfo.totalRanges + ( BITS_PER_BYTE - 1U ) ) >> LOG2_BITS_PER_BYTE;
    _fileDownloadInfo.downloadedBitmap = ( uint8_t * ) IotDemo_Malloc( bitmapLength );

    if( _fileDownloadInfo.downloadedBitmap == NULL )
    {
        IotLogError( "Could not allocate a new bitmap for keeping track of downloaded ranges." );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    _fileDownloadInfo.scheduledBitmap = ( uint8_t * ) IotDemo_Malloc( bitmapLength );

    if( _fileDownloadInfo.scheduledBitmap == NULL )
    {
        IotLogError( "Could not allocate a new bitmap for keeping track of scheduled ranges." );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    memset( _fileDownloadInfo.downloadedBitmap, 0, bitmapLength );
    memset( _fileDownloadInfo.scheduledBitmap, 0, bitmapLength );

    while( IotSemaphore_GetCount( &( _fileFinishedSem ) ) == 0 )
    {
        /* Retrieve a free request. If there are no free requests then put the application thread to sleep to check
         * again later if there is one. */
        reqIndex = _getFreeRequestIndex();

        if( reqIndex == -1 )
        {
            IotClock_SleepMs( GET_FREE_REQUEST_RETRY_WAIT_TIME_MS );
            continue;
        }

        /* Checking for a scheduledBitmap and scheduling the request is locked in the entirety. This is just incase we
         * we need to reconnect a closed connection during one of the asynchronous responses. */
        IotMutex_Lock( &( _requestSchedulingMutex ) );
        /* Check if the currentRange is already downloaded find the next range that has not been downloaded. */
        int32_t rangeCheckCount = 0;

        do
        {
            bitMask = BITMASK( currentRange );
            byteOffset = BYTE_OFFSET( currentRange );

            if( ( ( _fileDownloadInfo.downloadedBitmap[ byteOffset ] & bitMask ) == ( ( uint8_t ) 0 ) ) &&
                ( ( _fileDownloadInfo.scheduledBitmap[ byteOffset ] & bitMask ) == ( ( uint8_t ) 0 ) ) )
            {
                break;
            }

            rangeCheckCount++;
            currentRange++;

            /* This is less expensive than a modulus. */
            if( currentRange == _fileDownloadInfo.totalRanges )
            {
                currentRange = 0;
            }
        } while( rangeCheckCount < _fileDownloadInfo.totalRanges );

        if( rangeCheckCount == _fileDownloadInfo.totalRanges )
        {
            IotLogWarn( "All ranges have been marked as downloaded or are already scheduled." );
        }
        else
        {
            if( _scheduleAsyncRequest( reqIndex, currentRange ) != EXIT_SUCCESS )
            {
                IotLogError( "Error scheduling the asynchronous request." );
                IotMutex_Unlock( &( _requestSchedulingMutex ) );
                IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
            }
        }

        IotMutex_Unlock( &( _requestSchedulingMutex ) );
    }

    IOT_FUNCTION_CLEANUP_BEGIN();

    /* Clean up all resources created with this demo. */
    if( inUseRequestMutexCreated )
    {
        IotMutex_Destroy( &( _inUseRequestsMutex ) );
    }

    if( fileFinishedSemCreated )
    {
        IotSemaphore_Destroy( &( _fileFinishedSem ) );
    }

    if( requestSchedulingMutexCreated )
    {
        IotMutex_Destroy( &( _requestSchedulingMutex ) );
    }

    if( _fileDownloadInfo.downloadedBitmap != NULL )
    {
        IotDemo_Free( _fileDownloadInfo.downloadedBitmap );
    }

    if( _fileDownloadInfo.scheduledBitmap != NULL )
    {
        IotDemo_Free( _fileDownloadInfo.scheduledBitmap );
    }

    /* Disconnect from the server even if it is already disconnected. */
    IotHttpsClient_Disconnect( _connHandle );
    /* De-initialize the library because we are done using it. */
    IotHttpsClient_Deinit();

    IOT_FUNCTION_CLEANUP_END();
}
