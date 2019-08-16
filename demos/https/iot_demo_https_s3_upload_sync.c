/*
 * Amazon FreeRTOS V201906.00 Major
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
 * @file iot_demo_https_s3_upload_sync.c
 * @brief Demonstrates synchronous usage of the HTTPS library by performing a PUT requests on a file from S3.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* C Standard includes. */
#include <stdbool.h>
#include <string.h>

/* Set up logging for this demo. */
#include "iot_demo_logging.h"

/* Amazon FreeRTOS includes. */
#include "iot_https_client.h"
#include "iot_https_utils.h"
#include "aws_demo_config.h"
#include "platform/iot_network.h"
#include "private/iot_error.h"
#include "iot_demo_https_common.h"
#include "platform/iot_clock.h"

/**
 * This demonstrates uploading a file to S3 using a pre-signed URL using the Amazon FreeRTOS HTTP Client library.
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
    #define IOT_DEMO_HTTPS_CONN_BUFFER_SIZE    ( 512 )
#endif

/* Size in bytes of the user buffer used to store the internal request context and HTTP request header lines.
 * The size presented here accounts for the storage of the internal context, the first request line in the HTTP
 * formatted header and extra headers. The minimum size can be found in extern const uint32_t
 * requestUserBufferMinimumSize. Keep in mind that this requestUserBufferMinimumSize does not include the size of the
 * path in the request line. The path could be well over 100 characters long as it includes not only the object key name
 * in S3, but also the query following. The query following has the AWSAccessKeyId, the expiration time, and the
 * AWS Signature Version 4 signature.*/
#ifndef IOT_DEMO_HTTPS_REQ_USER_BUFFER_SIZE
    #define IOT_DEMO_HTTPS_REQ_USER_BUFFER_SIZE    ( 512 )
#endif

/* Size in bytes of the user buffer used to store the internal response context and the HTTP response header lines.
 * The size presented here accounts for the storage of the internal context, the first request line in the HTTP
 * formatted header and extra headers. The minimum can be found in responseUserBufferMinimumSize.
 * Keep in mind that if the headers from the response do not all fit into this buffer, then the rest of the headers
 * will be discarded. The minimum size can be found in extern const uint32_t responseUserBufferMinimumSize. */
#ifndef IOT_DEMO_HTTPS_RESP_USER_BUFFER_SIZE
    #define IOT_DEMO_HTTPS_RESP_USER_BUFFER_SIZE    ( 1024 )
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
int RunHttpsSyncUploadDemo( bool awsIotMqttMode,
                            const char * pIdentifier,
                            void * pNetworkServerInfo,
                            void * pNetworkCredentialInfo,
                            const IotNetworkInterface_t * pNetworkInterface );

/*-----------------------------------------------------------*/

/**
 * @brief The function that runs the HTTPS Synchronous Upload demo.
 *
 * @param[in] awsIotMqttMode Specify if this demo is running with the AWS IoT MQTT server. This is ignored in this demo.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier. This is ignored in this demo.
 * @param[in] pNetworkServerInfo Contains network information specific for the MQTT demo. This is ignored in this demo.
 * @param[in] pNetworkCredentialInfo Contains credential Info for a TLS connection.
 * @param[in] pNetworkInterface Network interface to use for this demo.
 *
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE` otherwise.
 */
int RunHttpsSyncUploadDemo( bool awsIotMqttMode,
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

    /* Handle identifying the HTTP response. This is valid after the reponse has been received with
     * IotHttpsClient_SendSync(). */
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    /* Synchronous request specific configurations. */
    IotHttpsSyncInfo_t reqSyncInfo = { 0 };
    /* Synchronous response specific configurations. */
    IotHttpsSyncInfo_t respSyncInfo = { 0 };

    /* The location of the path within string IOT_DEMO_HTTPS_PRESIGNED_PUT_URL. */
    const char * pPath = NULL;
    /* The length of the path within string IOT_DEMO_HTTPS_PRESIGNED_PUT_URL. */
    size_t pathLen = 0;
    /* The location of the address within string IOT_DEMO_HTTPS_PRESIGNED_PUT_URL. */
    const char * pAddress = NULL;
    /* The length of the address within string IOT_DEMO_HTTPS_PRESIGNED_PUT_URL. */
    size_t addressLen = 0;
    /* The status of HTTP response. */
    uint16_t respStatus = IOT_HTTPS_STATUS_OK;
     /* The current index in the number of connection tries. */
    uint32_t connIndex = 0;

    /* Retrieve the path location and length from IOT_DEMO_HTTPS_PRESIGNED_PUT_URL. */
    httpsClientStatus = IotHttpsClient_GetUrlPath( IOT_DEMO_HTTPS_PRESIGNED_PUT_URL,
                                                   ( size_t ) strlen( IOT_DEMO_HTTPS_PRESIGNED_PUT_URL ),
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
                                                      ( size_t ) strlen( IOT_DEMO_HTTPS_PRESIGNED_PUT_URL ),
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
    connConfig.flags |= IOT_HTTPS_DISABLE_SNI; /* Disable SNI, enable TLS, enable Persistent connections, is HTTP/1.1 */
    connConfig.pCaCert = IOT_DEMO_HTTPS_TRUSTED_ROOT_CA;
    connConfig.caCertLen = sizeof( IOT_DEMO_HTTPS_TRUSTED_ROOT_CA );
    connConfig.userBuffer.pBuffer = _pConnUserBuffer;
    connConfig.userBuffer.bufferLen = sizeof( _pConnUserBuffer );
    connConfig.pClientCert = ( ( IotNetworkCredentials_t * ) pNetworkCredentialInfo )->pClientCert;
    connConfig.clientCertLen = ( ( IotNetworkCredentials_t * ) pNetworkCredentialInfo )->clientCertSize;
    connConfig.pPrivateKey = ( ( IotNetworkCredentials_t * ) pNetworkCredentialInfo )->pPrivateKey;
    connConfig.privateKeyLen = ( ( IotNetworkCredentials_t * ) pNetworkCredentialInfo )->privateKeySize;
    connConfig.pNetworkInterface = pNetworkInterface;

    /* Set the configurations needed for a synchronous request. */
    reqSyncInfo.pBody = ( uint8_t * ) ( IOT_DEMO_HTTPS_UPLOAD_DATA ); /* Pointer to the file/buffer of data we want to upload. */
    reqSyncInfo.bodyLen = IOT_DEMO_HTTPS_UPLOAD_DATA_SIZE;

    /* Ignore the response body in the synchronous response. */
    respSyncInfo.pBody = NULL;
    respSyncInfo.bodyLen = 0;

    /* The path is everything that is not the address. It also includes the query. So we get the strlen( pPath ) to
     * acquire everything following in IOT_DEMO_HTTPS_PRESIGNED_PUT_URL. */
    /* Set the request configurations. */
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
    reqConfig.isAsync = false;
    reqConfig.u.pSyncInfo = &reqSyncInfo;

    /* Set the response configurations. */
    respConfig.userBuffer.pBuffer = _pRespUserBuffer;
    respConfig.userBuffer.bufferLen = sizeof( _pRespUserBuffer );
    /* Even though the body is being ignored, #IotHttpsResponseInfo_t.pSyncInfo should only be set to NULL if the 
     * response is being received asynchronously. */
    respConfig.pSyncInfo = &respSyncInfo;

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
    for( connIndex = 1; connIndex <= IOT_DEMO_HTTPS_CONNECTION_NUM_RETRY; connIndex++ )
    {
        httpsClientStatus = IotHttpsClient_Connect( &connHandle, &connConfig );

        if( ( httpsClientStatus == IOT_HTTPS_CONNECTION_ERROR ) &&
            ( connIndex < IOT_DEMO_HTTPS_CONNECTION_NUM_RETRY ) )
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

    /* Send the upload request. */
    httpsClientStatus = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &respConfig, 0 );

    /* If there was network error try again one more time. */
    if( httpsClientStatus == IOT_HTTPS_NETWORK_ERROR )
    {
        /* Maybe the network error was because the server disconnected us. */
        httpsClientStatus = IotHttpsClient_Connect( &connHandle, &connConfig );

        if( httpsClientStatus != IOT_HTTPS_OK )
        {
            IotLogError( "Failed to reconnect to the S3 server after a network error on IotHttpsClient_SendSync(). Error code %d.", httpsClientStatus );
            IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
        }

        httpsClientStatus = IotHttpsClient_SendSync( connHandle, reqHandle, &respHandle, &respConfig, 0 );

        if( httpsClientStatus != IOT_HTTPS_OK )
        {
            IotLogError( "Failed receiving the response on a second try after a network error. The error code is: %d", httpsClientStatus );
            IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
        }
    }

    httpsClientStatus = IotHttpsClient_ReadResponseStatus( respHandle, &respStatus );
    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "Error in retreiving the response status. Error code %d", httpsClientStatus );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    if( respStatus != IOT_HTTPS_STATUS_OK )
    {
        IotLogError( "Failed to upload the data to s3. Response status: %d", respStatus );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    IotLogInfo( "File was successfully uploaded." );

    #if defined( IOT_DEMO_HTTPS_PRESIGNED_GET_URL )
        /* The size of uploaded file from a GET of the file size. */
        uint32_t fileSize = 0;

        IotLogInfo( "Now checking %.*s for the file uploaded...", addressLen, pAddress );

        /* Retrieve the path location and length from IOT_DEMO_HTTPS_PRESIGNED_GET_URL. */
        httpsClientStatus = IotHttpsClient_GetUrlPath( IOT_DEMO_HTTPS_PRESIGNED_GET_URL,
                                                       ( size_t ) strlen( IOT_DEMO_HTTPS_PRESIGNED_GET_URL ),
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
                                                          ( size_t ) strlen( IOT_DEMO_HTTPS_PRESIGNED_GET_URL ),
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

    /* Disconnect from the server even if the server may have already disconnected us. */
    if( connHandle != NULL )
    {
        IotHttpsClient_Disconnect( connHandle );
    }

    /* Deinitialize the library because we are done using it. */
    IotHttpsClient_Deinit();

    IOT_FUNCTION_CLEANUP_END();
}
