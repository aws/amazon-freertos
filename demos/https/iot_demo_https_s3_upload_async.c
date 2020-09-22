/*
 * FreeRTOS V202007.00
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
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file iot_demo_https_s3_upload_async.c
 * @brief Demonstrates asynchronous usage of the HTTPS library by performing a PUT requests on a file to S3.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* C Standard includes. */
#include <stdbool.h>
#include <string.h>

/* Set up logging for this demo. */
#include "iot_demo_logging.h"

/* FreeRTOS includes. */
#include "iot_demo_https_common.h"
#include "iot_https_utils.h"
#include "aws_demo_config.h"
#include "platform/iot_network.h"
#include "platform/iot_threads.h"
#include "private/iot_error.h"
#include "platform/iot_clock.h"

/**
 * This demonstrates uploading a file to S3 using a pre-signed URL using the FreeRTOS HTTP Client library.
 * The HTTPS Client library is a generic HTTP/1.1 client library that be used to upload files to other webservers as
 * well.
 *
 * A presigned URL is required to run this demo. Please see the demos/https/README.md for instructions on how to
 * generate one.
 */

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default values for undefined configuration settings.
 */

/* Presigned URL for S3 PUT Object access. A GET URL is defined to verify the demo.*/
#ifndef IOT_DEMO_HTTPS_PRESIGNED_PUT_URL
    #define IOT_DEMO_HTTPS_PRESIGNED_PUT_URL    "Please configure a presigned PUT URL in iot_config.h."
#endif

/* TLS port for HTTPS. */
#ifndef IOT_DEMO_HTTPS_PORT
    #define IOT_DEMO_HTTPS_PORT    ( ( uint16_t ) 443 )
#endif

#ifndef IOT_DEMO_HTTPS_TRUSTED_ROOT_CA
    /* This the Baltimore Cybertrust root CA associated with the S3 server certificate. */
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

/* Size in bytes of the User Buffer used to store the internal connection context. The size presented here accounts for
 * storage of the internal connection context. The minimum size can be found in extern const unint32_t
 * connectionUserBufferMinimumSize. */
#ifndef IOT_DEMO_HTTPS_CONN_BUFFER_SIZE
    #define IOT_DEMO_HTTPS_CONN_BUFFER_SIZE    ( ( int ) 512 )
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

/* Pointer to the data to upload.*/
#ifndef IOT_DEMO_HTTPS_UPLOAD_DATA
    #define IOT_DEMO_HTTPS_UPLOAD_DATA    "Hello World!"
#endif

/* The size of the data in bytes to upload. */
#ifndef IOT_DEMO_HTTPS_UPLOAD_DATA_SIZE
    #define IOT_DEMO_HTTPS_UPLOAD_DATA_SIZE    ( sizeof( IOT_DEMO_HTTPS_UPLOAD_DATA ) - 1 )
#endif

/* Time to wait in milliseconds before retrying the HTTPS Connection. A connection is only attempted again if
 * IOT_HTTPS_CONNECTION_ERROR is returned from IotHttpsClient_Connect(). This indicates an error in the network
 * layer. To view logging for network errors update IOT_LOG_LEVEL_NETWORK to IOT_LOG_ERROR in iot_config.h */
#ifndef IOT_DEMO_HTTPS_CONNECTION_RETRY_WAIT_MS
    #define IOT_DEMO_HTTPS_CONNECTION_RETRY_WAIT_MS    ( ( uint32_t ) 3000 )
#endif

/* Number of times to retry the HTTPS connection. A connection is only attempted again if
 * IOT_HTTPS_CONNECTION_ERROR is returned from IotHttpsClient_Connect(). This indicates an error in the network
 * layer. To view logging for network errors update IOT_LOG_LEVEL_NETWORK to IOT_LOG_ERROR in iot_config.h */
#ifndef IOT_DEMO_HTTPS_CONNECTION_NUM_RETRY
    #define IOT_DEMO_HTTPS_CONNECTION_NUM_RETRY    ( ( uint32_t ) 3 )
#endif

/* Timeout in milliseconds to wait for the asynchronous request to finish. This timeout starts when the last
 * IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS sent has been scheduled. */
#ifndef IOT_HTTPS_DEMO_ASYNC_TIMEOUT_MS
    #define IOT_HTTPS_DEMO_ASYNC_TIMEOUT_MS    ( ( uint32_t ) 300000 )   /* 5 minute timeout for this demo. */
#endif

/** @endcond */

/*-----------------------------------------------------------*/

/**
 * @brief Buffer used to store the internal connection context.
 */
static uint8_t _pConnUserBuffer[ IOT_DEMO_HTTPS_CONN_BUFFER_SIZE ] = { 0 };

/**
 * @brief Buffer used to store the request context and the HTTP request header lines.
 */
static uint8_t _pReqUserBuffer[ IOT_DEMO_HTTPS_REQ_USER_BUFFER_SIZE ] = { 0 };

/**
 * @brief Buffer used to store the response context and the HTTP response header lines.
 */
static uint8_t _pRespUserBuffer[ IOT_DEMO_HTTPS_RESP_USER_BUFFER_SIZE ] = { 0 };

/*-----------------------------------------------------------*/

/* Declaration of demo function. */
int RunHttpsAsyncUploadDemo( bool awsIotMqttMode,
                             const char * pIdentifier,
                             void * pNetworkServerInfo,
                             void * pNetworkCredentialInfo,
                             const IotNetworkInterface_t * pNetworkInterface );

/*-----------------------------------------------------------*/

/**
 * @brief Semaphore use to signal that the demo is finished.
 *
 * The upload is completely finished when the response is fully received with a 200 HTTP response status.
 */
static IotSemaphore_t _uploadFinishedSem = { 0 };

/*-----------------------------------------------------------*/

/**
 * @brief Callback for an asynchronous request to write data to the network.
 *
 * @param[in] pPrivData - User private data configured with the HTTPS Client library request configuration.
 * @param[in] reqHandle - Identifier for the current request in progress.
 */
static void _writeCallback( void * pPrivData,
                            IotHttpsRequestHandle_t reqHandle )
{
    IotHttpsReturnCode_t writeStatus = IOT_HTTPS_OK;
    bool * pUploadSuccess = ( bool * ) pPrivData;

    /* Write the upload data to the network. isComplete the last parameter must always be set to 1. */
    writeStatus = IotHttpsClient_WriteRequestBody( reqHandle,
                                                   ( uint8_t * ) IOT_DEMO_HTTPS_UPLOAD_DATA,
                                                   IOT_DEMO_HTTPS_UPLOAD_DATA_SIZE,
                                                   1 );

    if( writeStatus != IOT_HTTPS_OK )
    {
        IotLogError( "Error writing the request body to the network. Return code %d.", writeStatus );
        *pUploadSuccess = false;
        IotHttpsClient_CancelRequestAsync( reqHandle );
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
    ( void ) rc;
    ( void ) respHandle;

    bool * pUploadSuccess = ( bool * ) pPrivData;

    /* When the remote server response with 200 OK, the file was successfully uploaded. */
    if( status == IOT_HTTPS_STATUS_OK )
    {
        *pUploadSuccess = true;
    }
    else
    {
        *pUploadSuccess = false;
    }

    /* Post to the semaphore that the upload is finished. */
    IotSemaphore_Post( &( _uploadFinishedSem ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief The function that runs the HTTPS Asynchronous Upload demo.
 *
 * @param[in] awsIotMqttMode Specify if this demo is running with the AWS IoT MQTT server. This is ignored in this demo.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier. This is ignored in this demo.
 * @param[in] pNetworkServerInfo Contains network information specific for the MQTT demo. This is ignored in this demo.
 * @param[in] pNetworkCredentialInfo Contains credential info for a TLS connection.
 * @param[in] pNetworkInterface Network interface to use for this demo.
 *
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE` otherwise.
 */
int RunHttpsAsyncUploadDemo( bool awsIotMqttMode,
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
    IotHttpsReturnCode_t httpsClientStatus = IOT_HTTPS_OK;

    /* Configurations for the HTTPS connection. */
    IotHttpsConnectionInfo_t connConfig = { 0 };
    /* Handle identifying the HTTPS connection. */
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    /* Configurations for the HTTPS request. */
    IotHttpsRequestInfo_t reqConfig = { 0 };
    /* Configurations for the HTTPS response. */
    IotHttpsResponseInfo_t respConfig = { 0 };

    /* Handle identifying the HTTP request. This is valid after the request has been initialized with
     * IotHttpsClient_InitializeRequest(). */
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    /* Handle identifying the HTTP response. This handle is used only in the asynchronous callback. */
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    /* Asynchronous request specific configurations. */
    IotHttpsAsyncInfo_t asyncInfo = { 0 };

    /* Signal if the global upload finished semaphore was created for cleanup. */
    bool uploadFinishedSemCreated = false;

    /* The location of the path within string IOT_DEMO_HTTPS_PRESIGNED_PUT_URL. */
    const char * pPath = NULL;
    /* The length of the path within string IOT_DEMO_HTTPS_PRESIGNED_PUT_URL. */
    size_t pathLen = 0;
    /* The location of the address within string IOT_DEMO_HTTPS_PRESIGNED_PUT_URL. */
    const char * pAddress = NULL;
    /* The length of the address within string IOT_DEMO_HTTPS_PRESIGNED_PUT_URL. */
    size_t addressLen = 0;
    /* The current attempt in the number of connection tries. */
    uint32_t connAttempt = 0;
    /* Context into the asynchronous callback to denote a success or failure back to the application. */
    bool uploadSuccess = true;

    IotLogInfo( "HTTPS Client Asynchronous S3 upload demo using pre-signed URL: %s", IOT_DEMO_HTTPS_PRESIGNED_PUT_URL );

    /* Retrieve the path location from IOT_DEMO_HTTPS_PRESIGNED_GET_URL. This function returns the length of the path
     * without the query into pathLen. */
    httpsClientStatus = IotHttpsClient_GetUrlPath( IOT_DEMO_HTTPS_PRESIGNED_PUT_URL,
                                                   strlen( IOT_DEMO_HTTPS_PRESIGNED_PUT_URL ),
                                                   &pPath,
                                                   &pathLen );

    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "An error occurred in IotHttpsClient_GetUrlPath() on URL %s. Error code: %d",
                     IOT_DEMO_HTTPS_PRESIGNED_PUT_URL,
                     httpsClientStatus );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Retrieve the address location and length from the IOT_DEMO_HTTPS_PRESIGNED_PUT_URL. */
    httpsClientStatus = IotHttpsClient_GetUrlAddress( IOT_DEMO_HTTPS_PRESIGNED_PUT_URL,
                                                      strlen( IOT_DEMO_HTTPS_PRESIGNED_PUT_URL ),
                                                      &pAddress,
                                                      &addressLen );

    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "An error occurred in IotHttpsClient_GetUrlAddress() on URL %s\r\n. Error code %d",
                     IOT_DEMO_HTTPS_PRESIGNED_PUT_URL,
                     httpsClientStatus );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Set the connection configurations. */
    connConfig.pAddress = pAddress;
    connConfig.addressLen = addressLen;
    connConfig.port = IOT_DEMO_HTTPS_PORT;
    connConfig.pCaCert = IOT_DEMO_HTTPS_TRUSTED_ROOT_CA;
    connConfig.caCertLen = sizeof( IOT_DEMO_HTTPS_TRUSTED_ROOT_CA );
    connConfig.userBuffer.pBuffer = _pConnUserBuffer;
    connConfig.userBuffer.bufferLen = sizeof( _pConnUserBuffer );
    connConfig.pClientCert = ( ( IotNetworkCredentials_t * ) pNetworkCredentialInfo )->pClientCert;
    connConfig.clientCertLen = ( ( IotNetworkCredentials_t * ) pNetworkCredentialInfo )->clientCertSize;
    connConfig.pPrivateKey = ( ( IotNetworkCredentials_t * ) pNetworkCredentialInfo )->pPrivateKey;
    connConfig.privateKeyLen = ( ( IotNetworkCredentials_t * ) pNetworkCredentialInfo )->privateKeySize;
    connConfig.pNetworkInterface = pNetworkInterface;

    asyncInfo.callbacks.writeCallback = _writeCallback;
    asyncInfo.callbacks.responseCompleteCallback = _responseCompleteCallback;
    asyncInfo.pPrivData = &uploadSuccess;

    /* The path is everything that is not the address. It also includes the query. So we get the strlen( pPath ) to
     * acquire everything following in IOT_DEMO_HTTPS_PRESIGNED_PUT_URL. */
    reqConfig.pPath = pPath;
    reqConfig.pathLen = strlen( pPath );
    reqConfig.pHost = pAddress;
    reqConfig.hostLen = addressLen;

    /* The PUT method is used to upload an object to S3 because it is simpler than using POST. POST requires AWS S3
     * access specific header fields in the body of the message in a multipart/form-data encoded message. See
     * https://docs.aws.amazon.com/AmazonS3/latest/API/RESTObjectPOST.html for more information about POST object. See
     * https://docs.aws.amazon.com/AmazonS3/latest/API/RESTObjectPUT.html for more information about PUT object. */
    reqConfig.method = IOT_HTTPS_METHOD_PUT;
    reqConfig.isNonPersistent = false;
    reqConfig.userBuffer.pBuffer = _pReqUserBuffer;
    reqConfig.userBuffer.bufferLen = sizeof( _pReqUserBuffer );
    reqConfig.isAsync = true;
    reqConfig.u.pAsyncInfo = &asyncInfo;

    respConfig.userBuffer.pBuffer = _pRespUserBuffer;
    respConfig.userBuffer.bufferLen = sizeof( _pRespUserBuffer );
    respConfig.pSyncInfo = NULL;

    /* Create the semaphore for waiting for the whole upload response to finish. */
    uploadFinishedSemCreated = IotSemaphore_Create( &( _uploadFinishedSem ), 0 /* Initial value. */, 1 /* Max value. */ );

    if( uploadFinishedSemCreated == false )
    {
        IotLogError( "Failed to create a semaphore to wait for the response to finish." );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Initialize the HTTPS library. */
    httpsClientStatus = IotHttpsClient_Init();

    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "An error occurred initializing the HTTPS library. Error code: %d", httpsClientStatus );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Initialize the request to initialize internal request context, write the HTTP request first line to the
     * _pReqUserBuffer header space, and write headers User-Agent and Host. */
    httpsClientStatus = IotHttpsClient_InitializeRequest( &reqHandle, &reqConfig );

    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "An error occurred in IotHttpsClient_InitializeRequest() with error code: %d", httpsClientStatus );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Connect to S3. */
    for( connAttempt = 1; connAttempt <= IOT_DEMO_HTTPS_CONNECTION_NUM_RETRY; connAttempt++ )
    {
        httpsClientStatus = IotHttpsClient_Connect( &connHandle, &connConfig );

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
        IotLogError( "Failed to connect to the server. Error code: %d.", httpsClientStatus );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Send the request and receive the response asynchronously. This will schedule the async request. We
     * will return immediately after scheduling. */
    httpsClientStatus = IotHttpsClient_SendAsync( connHandle, reqHandle, &respHandle, &respConfig );

    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "Failed to send the request asynchronously with error code: %d", httpsClientStatus );
        /* If we failed to schedule an async request then this is an error and we should exit the loop. */
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Timing out waiting for the response to finish will fail the demo. */
    if( IotSemaphore_TimedWait( &( _uploadFinishedSem ), IOT_HTTPS_DEMO_ASYNC_TIMEOUT_MS ) == false )
    {
        IotLogError( "Timed out waiting for the asynchronous request to complete." );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Check that the upload was successful. This variable is set during the _responseCompleteCallback(). */
    if( uploadSuccess == false )
    {
        IotLogError( "Upload did not complete successfully." );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    #if defined( IOT_DEMO_HTTPS_PRESIGNED_GET_URL )
        /* The size of uploaded file from a GET of the file size. */
        uint32_t fileSize = 0;

        IotLogInfo( "Now checking %.*s for the file uploaded...", addressLen, pAddress );

        /* Retrieve the path location and length from IOT_DEMO_HTTPS_PRESIGNED_GET_URL. */
        httpsClientStatus = IotHttpsClient_GetUrlPath( IOT_DEMO_HTTPS_PRESIGNED_GET_URL,
                                                       strlen( IOT_DEMO_HTTPS_PRESIGNED_GET_URL ),
                                                       &pPath,
                                                       &pathLen );

        if( httpsClientStatus != IOT_HTTPS_OK )
        {
            IotLogError( "An error occurred in IotHttpsClient_GetUrlPath() on URL %s. Error code: %d",
                         IOT_DEMO_HTTPS_PRESIGNED_GET_URL,
                         httpsClientStatus );
            IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
        }

        /* Retrieve the address location and length from the IOT_DEMO_HTTPS_PRESIGNED_GET_URL. */
        httpsClientStatus = IotHttpsClient_GetUrlAddress( IOT_DEMO_HTTPS_PRESIGNED_GET_URL,
                                                          strlen( IOT_DEMO_HTTPS_PRESIGNED_GET_URL ),
                                                          &pAddress,
                                                          &addressLen );

        if( httpsClientStatus != IOT_HTTPS_OK )
        {
            IotLogError( "An error occurred in IotHttpsClient_GetUrlAddress() on URL %s\r\n. Error code %d",
                         IOT_DEMO_HTTPS_PRESIGNED_GET_URL,
                         httpsClientStatus );
            IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
        }

        /* Verify the file was uploaded by retrieving the file size. */
        if( _IotHttpsDemo_GetS3ObjectFileSize( &fileSize,
                                               connHandle,
                                               pPath,
                                               strlen( pPath ),
                                               pAddress,
                                               addressLen,
                                               _pReqUserBuffer,
                                               IOT_DEMO_HTTPS_REQ_USER_BUFFER_SIZE,
                                               _pRespUserBuffer,
                                               IOT_DEMO_HTTPS_RESP_USER_BUFFER_SIZE ) != EXIT_SUCCESS )
        {
            IotLogError( "Failed to retrieve the s3 object size." );
            IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
        }

        if( fileSize != IOT_DEMO_HTTPS_UPLOAD_DATA_SIZE )
        {
            IotLogError( "Failed to upload the data to s3. Found the file size to be %d, but it should be %d.",
                         fileSize,
                         IOT_DEMO_HTTPS_UPLOAD_DATA );
            IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
        }
        IotLogInfo( "Verified file size on S3 is %d. File size specified to upload is %d.", fileSize, IOT_DEMO_HTTPS_UPLOAD_DATA_SIZE );
    #endif /* if defined( IOT_DEMO_HTTPS_PRESIGNED_GET_URL ) */

    IOT_FUNCTION_CLEANUP_BEGIN();

    if( uploadFinishedSemCreated )
    {
        IotSemaphore_Destroy( &( _uploadFinishedSem ) );
    }

    /* Disconnect from the server even if it is already disconnected. */
    IotHttpsClient_Disconnect( connHandle );
    /* Clean up the library because we are done using it. */
    IotHttpsClient_Cleanup();

    IOT_FUNCTION_CLEANUP_END();
}
