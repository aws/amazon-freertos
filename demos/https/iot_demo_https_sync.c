/*
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
 */

/**
 * @file iot_demo_https_sync.c
 * @brief Demonstrates synchronous usage of the HTTPS library by performing partial content GET requests on a file from S3.
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

/**
 * This demo downloads an object from S3 and outputs to the console. A presigned URL is required to run this demo. Please
 * see the demos/https/README.md for instructions on how to generate one.
 */

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default values for undefined configuration settings.
 */

/* Presigned URL for S3 GET Object access. */
#ifndef IOT_DEMO_HTTPS_PRESIGNED_URL
    #define IOT_DEMO_HTTPS_PRESIGNED_URL     "Please configure a presigned URL in iot_config.h."
#endif

/* TLS port for HTTPS. */
#ifndef IOT_DEMO_HTTPS_PORT
    #define IOT_DEMO_HTTPS_PORT              ( 443 )
#endif

#ifndef IOT_DEMO_HTTPS_TRUSTED_ROOT_CA
    /* This the Baltomore Cybertrust issued root CA that the s3 server uses. */
    #define IOT_DEMO_HTTPS_TRUSTED_ROOT_CA    \
    "-----BEGIN CERTIFICATE-----\n"\
    "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n"\
    "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n"\
    "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n"\
    "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n"\
    "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n"\
    "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n"\
    "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n"\
    "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n"\
    "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n"\
    "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n"\
    "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n"\
    "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n"\
    "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n"\
    "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n"\
    "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n"\
    "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n"\
    "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n"\
    "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n"\
    "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"\
    "-----END CERTIFICATE-----\n"
#endif

/* Size in bytes of the User Buffer used to store the internal connection context. The size presented here accounts for
   storage of the internal connection context. The minimum size can be found in extern const unint32_t connectionUserBufferMinimumSize. */
#ifndef IOT_DEMO_HTTPS_CONN_BUFFER_SIZE
    #define IOT_DEMO_HTTPS_CONN_BUFFER_SIZE         ( 256 )
#endif

/* Size in bytes of the user buffer used to store the internal request context and HTTP request header lines. 
   The size presented here accounts for the storeage of the internal context, the first request line in the HTTP
   formatted header and extra headers. The minimum size can be found in extern const uint32_t requestUserBufferMinimumSize. */
#ifndef IOT_DEMO_HTTPS_REQ_USER_BUFFER_SIZE
    #define IOT_DEMO_HTTPS_REQ_USER_BUFFER_SIZE     ( 512 )
#endif

/* Size in bytes of the user buffer used to store the internal response context and the HTTP response header lines. 
   The size presented here accounts for the storeage of the internal context, the first request line in the HTTP
   formatted header and extra headers. The minimum can be found in requestUserBufferMinimumSize.
   Keep in mind that if the headers from the response do not all fit into this buffer, then the rest of the headers
   will be discarded. The minimum size can be found in extern const uint32_t responseUserBufferMinimumSize. */
#ifndef IOT_DEMO_HTTPS_RESP_USER_BUFFER_SIZE
    #define IOT_DEMO_HTTPS_RESP_USER_BUFFER_SIZE    ( 512 )
#endif

/* Size in bytes of the buffer used to store the response body (parts of it). This should be greater than or equal to
   the size of the file we want to download.*/
#ifndef IOT_DEMO_HTTPS_RESP_BODY_BUFFER_SIZE
    #define IOT_DEMO_HTTPS_RESP_BODY_BUFFER_SIZE    ( 512 )
#endif

/** @endcond */

/**
 * @brief The size of the header value string for the Range: field. This is used to specify which parts of the file
 * we want to download. Let's say the maximum file size is what can fit in a 32 bit unsigned integer. 2^32 = 4294967296
 * which is 10 digits. The header value string is of the form: "bytes=N-M" where N and M are integers. So the length
 * of this string is strlen(N) + strlen(M) + strlen("bytes=-") + NULL terminator. Given the maximum number of digits is
 * 10 we get the maximum length of this header value as: 10 * 2 + 7 + 1.
 */
#define RANGE_VALUE_MAX_LENGTH      ( 28 )

/**
 * @brief HTTP standard header field "Range".
 */
#define RANGE_HEADER_FIELD          "Range"

/**
 * @brief HTTP standard header value for requesting a range of bytes from 0 to 0.
 * 
 * This is used to get the size of the file from S3. Performing a HEAD request with S3 requires generating a Sigv4 
 * signature in an Authorization header field. We work around this by performing a GET on Range: bytes=0-0. Then 
 * extracting the size of the file from the Content-Range header field in the response. */
#define RANGE_0_TO_0_HEADER_VALUE   "bytes=0-0"

/**
 * @brief HTTP standard header field "Content-Range"
 */
#define CONTENT_RANGE_HEADER_FIELD  "Content-Range"

/*-----------------------------------------------------------*/

/**
 * @brief Buffer used to store the internal connection context. 
 */
static uint8_t _pConnUserBuffer[IOT_DEMO_HTTPS_CONN_BUFFER_SIZE] = { 0 };

/**
 * @brief Buffer used to store the request context and the HTTP request header lines. 
 */
static uint8_t _pReqUserBuffer[IOT_DEMO_HTTPS_REQ_USER_BUFFER_SIZE] = { 0 };

/**
 * @brief Buffer used to store the response context and the HTTP response header lines. 
 */
static uint8_t _pRespUserBuffer[IOT_DEMO_HTTPS_RESP_USER_BUFFER_SIZE] = { 0 };

/**
 * @brief Buffer used to store parts of the response body.
 */
static uint8_t _pRespBodyBuffer[IOT_DEMO_HTTPS_RESP_BODY_BUFFER_SIZE] = { 0 };

/* Declaration of demo function. */
int RunHttpsSyncDemo( bool awsIotMqttMode,
                 const char * pIdentifier,
                 void * pNetworkServerInfo,
                 void * pNetworkCredentialInfo,
                 const IotNetworkInterface_t * pNetworkInterface );

/*-----------------------------------------------------------*/     

/**
 * @brief The function that runs the HTTPS Synchronous demo.
 * 
 * @param[in] awsIotMqttMode Specify if this demo is running with the AWS IoT
 * MQTT server. Set this to `false` if using another MQTT server.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier. The demo starting parameters are built for core MQTT.
 *  but this demo ignores these parameters.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkInterface The network interface to use for this demo.
 *
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE` otherwise.
 */
int RunHttpsSyncDemo( bool awsIotMqttMode,
                 const char * pIdentifier,
                 void * pNetworkServerInfo,
                 void * pNetworkCredentialInfo,
                 const IotNetworkInterface_t * pNetworkInterface )
{
    IOT_FUNCTION_ENTRY( int, EXIT_SUCCESS );
    
    /* Unused parameters. */
    ( void )awsIotMqttMode;
    ( void )pIdentifier;
    ( void )pNetworkServerInfo;

    /* HTTPS Client library return status. */
    IotHttpsReturnCode_t httpsClientStatus;

    /* Configurations for the HTTPS connection. */
    IotHttpsConnectionInfo_t connConfig = { 0 };
    /* Handle identifying the HTTPS connection. */
    IotHttpsConnectionHandle_t connHandle = NULL;
    /* Configurations for the HTTPS request. */
    IotHttpsRequestInfo_t reqConfig = { 0 };
    /* Handle identifying the HTTP request. This is valid after the request has been initialized with 
       IotHttpsClient_InitializeRequest(). */
    IotHttpsRequestHandle_t reqHandle = NULL;
    /* Handle identifying the HTTP response. This is valid after the reponse has been received with 
       IotHttpsClient_SendSync(). */
    IotHttpsResponseHandle_t respHandle = NULL;
    /* Synchronous request specific configurations. */
    IotHttpsSyncRequestInfo_t syncInfo = { 0 };

    /* The location of the path within string IOT_DEMO_HTTPS_PRESIGNED_URL. */
    const char *pPath = NULL;
    /* The length of the path within string IOT_DEMO_HTTPS_PRESIGNED_URL. */
    size_t pathLen = 0;
    /* The location of the address within string IOT_DEMO_HTTPS_PRESIGNED_URL. */
    const char *pAddress = NULL;
    /* The length of the address within string IOT_DEMO_HTTPS_PRESIGNED_URL. */
    size_t addressLen = 0;

    /* The status of HTTP responses for each request. */
    uint16_t respStatus = IOT_HTTPS_STATUS_OK;
    /* The content length of HTTP responses for each request. */
    uint32_t contentLength = 0;

    /* String to store the Content-Range header field value. This header field as we are requesting in this demo is of 
       the form: "Content-Range: bytes 0-0/FILESIZE", where file size would be the length of the maximum 32 bit integer
       which is 10.  Since the header field value "bytes 0-0/FILESIZE" is less than the maximum possible Range header
       field value, we size this string to the Range header field value.*/
    char contentRangeValStr[RANGE_VALUE_MAX_LENGTH]; 
    /* The location of the file size in the contentRangeValStr. */
    char * fileSizeStr = NULL;
    /* The size of the file we are trying to download in S3. */
    uint32_t fileSize = 0;
    /* The number of bytes we want to request with in each range of the file bytes. */
    uint32_t numReqBytes = 0;
    /* curByte indicates which starting byte we want to download next. */
    uint32_t curByte = 0;
    /* Buffer to write the Range: header value string. */
    char rangeValueStr[RANGE_VALUE_MAX_LENGTH] = { 0 };

    /* Retrieve the path location and length from IOT_DEMO_HTTPS_PRESIGNED_URL. */
    httpsClientStatus = IotHttpsClient_GetUrlPath(IOT_DEMO_HTTPS_PRESIGNED_URL, 
        (size_t)strlen( IOT_DEMO_HTTPS_PRESIGNED_URL ), 
        &pPath, 
        &pathLen);
    if ( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError("An error occurred in IotHttpsClient_GetUrlPath() on URL %s. Error code: %d", 
            IOT_DEMO_HTTPS_PRESIGNED_URL, 
            httpsClientStatus );
        /* At this point we have not allocated any resources or initialized the HTTPS Client library so we will just 
          exit the demo here. */
        return EXIT_FAILURE;
    }

    /* Retrieve the address location and length from the IOT_DEMO_HTTPS_PRESIGNED_URL. */
    httpsClientStatus = IotHttpsClient_GetUrlAddress(IOT_DEMO_HTTPS_PRESIGNED_URL, 
        (size_t)strlen( IOT_DEMO_HTTPS_PRESIGNED_URL ),
        &pAddress, 
        &addressLen);
    if ( httpsClientStatus != IOT_HTTPS_OK)
    {
        IotLogError("An error occurred in IotHttpsClient_GetUrlAddress() on URL %s\r\n. Error code %d", 
            IOT_DEMO_HTTPS_PRESIGNED_URL, 
            httpsClientStatus );
        /* At this point we have not allocated any resources or initialized the HTTPS Client library so we will just 
           exit the demo here. */
        return EXIT_FAILURE;
    }

    /* Set the connection configurations. */
    connConfig.pAddress = pAddress;
    connConfig.addressLen = addressLen;
    connConfig.port = IOT_DEMO_HTTPS_PORT;
    connConfig.flags |= IOT_HTTPS_DISABLE_SNI; /* Disable SNI, enable TLS, enable Persistent connections, is HTTP/1.1 */
    connConfig.pCaCert = IOT_DEMO_HTTPS_TRUSTED_ROOT_CA;
    connConfig.caCertLen = sizeof(IOT_DEMO_HTTPS_TRUSTED_ROOT_CA);
    connConfig.userBuffer.pBuffer = _pConnUserBuffer;
    connConfig.userBuffer.bufferLen = sizeof(_pConnUserBuffer);
    connConfig.pClientCert = ( ( IotNetworkCredentials_t* )pNetworkCredentialInfo )->pClientCert;
    connConfig.clientCertLen = ( ( IotNetworkCredentials_t* )pNetworkCredentialInfo )->clientCertSize;
    connConfig.pPrivateKey = ( ( IotNetworkCredentials_t* )pNetworkCredentialInfo )->pPrivateKey;
    connConfig.privateKeyLen = ( ( IotNetworkCredentials_t* )pNetworkCredentialInfo )->privateKeySize;
    connConfig.pNetworkInterface = pNetworkInterface;

    /* Set the configurations needed for a synchronous request. */
    syncInfo.pReqData = NULL;    /* This is a GET request so there is no data in the body. */
    syncInfo.reqDataLen = 0;    /* Since there is not data in the body the length is 0. */
    syncInfo.pRespData = _pRespBodyBuffer;            /* This is a GET request so should configure a place to retreive the 
                                                       response body. */
    syncInfo.respDataLen = sizeof(_pRespBodyBuffer); /* The length of the GET request's response body. This should be 
                                                       greater than or equal to the size of the file requested, for the 
                                                       best performance. */

    /* Set the request configurations. */
    reqConfig.pPath = pPath; 
    /* The path is everything that is not the address. It also includes the query. So we get the strlen( pPath ) to 
       acquire everything following in IOT_DEMO_HTTPS_PRESIGNED_URL. */
    reqConfig.pathLen = strlen( pPath );
    reqConfig.pHost = pAddress;
    reqConfig.hostLen = addressLen;
    reqConfig.method = IOT_HTTPS_METHOD_GET;
    reqConfig.reqUserBuffer.pBuffer = _pReqUserBuffer;
    reqConfig.reqUserBuffer.bufferLen = sizeof(_pReqUserBuffer);
    reqConfig.respUserBuffer.pBuffer = _pRespUserBuffer;
    reqConfig.respUserBuffer.bufferLen = sizeof(_pRespUserBuffer);
    reqConfig.pSyncInfo = &syncInfo;

    /* Initialize the HTTPS library. */
    httpsClientStatus = IotHttpsClient_Init();
    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "An error occurred initializing the HTTPS library. Error code: %d", httpsClientStatus );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Initialize the request. This will create a request line and add required headers into the reqUserBuffer. */
    httpsClientStatus = IotHttpsClient_InitializeRequest( &reqHandle, &reqConfig );
    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError("An error occurred in IotHttpsClient_InitializeRequest() with error code: %d", httpsClientStatus);
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Connect to the S3 server. */
    httpsClientStatus = IotHttpsClient_Connect( &connHandle, &connConfig );
    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError("An error occurred in IotHttpsClient_Connect() with error code: %d", httpsClientStatus);
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Get the size of the file specified in the S3 presigned URL. */

    /* Performing a HEAD request with S3 requires generating a Sigv4 signature in an Authorization header field. We work 
       around this by performing a GET on Range: bytes=0-0. Then extracting the size of the file from the 
       Content-Range header field in the response. */
    httpsClientStatus = IotHttpsClient_AddHeader( reqHandle, 
        RANGE_HEADER_FIELD, 
        RANGE_0_TO_0_HEADER_VALUE, 
        strlen( RANGE_0_TO_0_HEADER_VALUE ) );
    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "Failed to write the header \"Range: bytes=0-0\" into the request. With error code: %d", 
            httpsClientStatus );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Send the request synchronously. */
    httpsClientStatus = IotHttpsClient_SendSync( &connHandle, reqHandle, &respHandle );
    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "There has been an error receiving the response. The error code is: %d", httpsClientStatus );
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* If the server could not response with a 206 Partial Content response, then there was a problem retrieving the
       file size and we will exit the demo. */
    IotHttpsClient_ReadResponseStatus(respHandle, &respStatus );
    if( respStatus != IOT_HTTPS_STATUS_PARTIAL_CONTENT)
    {
        IotLogError("Could not retrieve the file size. s3 responded with response status: %d", respStatus);
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
    }

    /* Get the file size by parsing the "bytes 0-0/FILESIZE" Content-Range header value string. */
    httpsClientStatus = IotHttpsClient_ReadHeader( respHandle, 
        CONTENT_RANGE_HEADER_FIELD, 
        contentRangeValStr, 
        sizeof(contentRangeValStr) );
    fileSizeStr = strstr( contentRangeValStr, "/" );
    if(fileSizeStr == NULL)
    {
        IotLogError("Expected the header value \"bytes 0-0/FILESIZE\" to be retrieved, but \"/\" could not be found in \
        in the header value.");
        IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );

    }
    /* The file size string is the next character after the "/" */
    fileSizeStr += 1;
    fileSize = (uint32_t)strtoul(fileSizeStr, NULL, 10);

    /* The number of bytes we want to request each time is the size of the buffer or the file size if it is smaller than 
       the buffer size, then the size of the file. */
    numReqBytes = IOT_DEMO_HTTPS_RESP_BODY_BUFFER_SIZE;
    if( fileSize < IOT_DEMO_HTTPS_RESP_BODY_BUFFER_SIZE)
    {
        numReqBytes = fileSize;
    }

    /* Here we iterate sending byte range requests until the full file has been downloaded. We keep track of the next 
       byte to download with curByte. When this reaches the fileSize we stop downloading.
       */
    while( curByte < fileSize)
    {
        /* Re-initialize the request to reuse the request. If we do not reinitialize then data from the last response
           associated with this request will linger. We reuse reqHandle beccause we are sending a new sequential 
           synchronous request. IotHttpsClient_InitializeRequest will create a new request from the reqConfig and return
           a reqHandle that is ready to use as a NEW request. */
        httpsClientStatus = IotHttpsClient_InitializeRequest( &reqHandle, &reqConfig );
        if( httpsClientStatus != IOT_HTTPS_OK )
        {
            IotLogError("An error occurred in IotHttpsClient_InitializeRequest() with error code: %d", httpsClientStatus);
            IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
        }

        /* Get the Range header value string. */
        int numWritten = snprintf(rangeValueStr, RANGE_VALUE_MAX_LENGTH, "bytes=%d-%d", curByte, curByte + numReqBytes - 1);
        if(numWritten < 0)
        {
            IotLogError("Failed to write the header value: \"bytes=%d-%d\" . Error code: %d",
                curByte,
                curByte + numReqBytes - 1,
                numWritten);
            IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
        }

        /* Set the header for a range request. */
        httpsClientStatus = IotHttpsClient_AddHeader( reqHandle, RANGE_HEADER_FIELD, rangeValueStr, numWritten);
        if( httpsClientStatus != IOT_HTTPS_OK )
        {
            IotLogError( "Failed to write the header Range: %.*s into the request. With error code: %d", numWritten, rangeValueStr, httpsClientStatus );
            IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
        }

        /* Send the request and receive the response synchronously. */
        IotLogInfo("Now requesting Range: %s.", rangeValueStr);
        /* A new response handle is returned from IotHttpsClient_SendSync(). We reuse the respHandle variable because
           the last response was already processed fully.  */
        httpsClientStatus = IotHttpsClient_SendSync( &connHandle, reqHandle, &respHandle );
        if( httpsClientStatus != IOT_HTTPS_OK )
        {
            IotLogError( "There has been an error receiving the response. The error code is: %d", httpsClientStatus );
            IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
        }

        httpsClientStatus = IotHttpsClient_ReadResponseStatus(respHandle, &respStatus );
        if( httpsClientStatus != IOT_HTTPS_OK )
        {
            IotLogError("Error in retreiving the response status. Error code %d", httpsClientStatus);
            IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
        }
        
        if( respStatus != IOT_HTTPS_STATUS_PARTIAL_CONTENT)
        {
            IotLogError("Failed to retrieve the partial content response from s3. Response status: %d", respStatus);
            IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
        }

        /* Get the content length of the body for printing without a NULL terminator. */
        httpsClientStatus = IotHttpsClient_ReadContentLength( respHandle, &contentLength );
        if( httpsClientStatus != IOT_HTTPS_OK )
        {
            IotLogError("Failed to read the Content-Length from the response. Error code %d", httpsClientStatus);
            IOT_SET_AND_GOTO_CLEANUP( EXIT_FAILURE );
        }

        /* The response has been fully received. */
        IotLogInfo( "Response return code: %d", respStatus );
        IotLogInfo( "Response Body: \r\n%.*s", contentLength, _pRespBodyBuffer );

        /* We increment by the contentLength because the server may not have sent us the range we request. */
        curByte += contentLength;

        IotLogInfo("Downloaded %d/%d", curByte, fileSize);

        /* If amount of file remaining to request is less than the current amount of bytes to request next time, then
           udpdate the amount of bytes to request, on the next iteration, to be the amount remaining. */
        if( (fileSize - curByte) < numReqBytes)
        {
            numReqBytes = fileSize - curByte;
        }
    }

    IOT_FUNCTION_CLEANUP_BEGIN();

    /* Disconnect from the server even if the server may have already disconnected us. */
    if( connHandle != NULL)
    {
        IotHttpsClient_Disconnect( connHandle );
    }
    
    /* Deinitialize the library because we are done using it. */
    IotHttpsClient_Deinit();

    IOT_FUNCTION_CLEANUP_END();
}