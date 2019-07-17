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
 * @file iot_demo_https_common.c
 * @brief Implementation of common routines used in the HTTPS S3 access demos.
 */
/* The config header is always included first. */
#include "iot_config.h"

/* C Standard headers. */
#include <string.h>

/* Common header. */
#include "iot_demo_https_common.h"

/* Amazon FreeRTOS includes. */
#include "iot_https_utils.h"
#include "iot_demo_logging.h"

/*-----------------------------------------------------------*/

/**
 * @brief The size of the header value string for the Range: field. 
 * 
 * This is used to specify which parts of the file
 * we want to download. Let's say the maximum file size is what can fit in a 32 bit unsigned integer. 2^32 = 4294967296
 * which is 10 digits. The header value string is of the form: "bytes=N-M" where N and M are integers. So the length
 * of this string is strlen(N) + strlen(M) + strlen("bytes=-") + NULL terminator. Given the maximum number of digits is
 * 10 we get the maximum length of this header value as: 10 * 2 + 7 + 1.
 */
#define RANGE_VALUE_MAX_LENGTH                      ( 28 )

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

int _IotHttpsDemo_GetS3ObjectFileSize(
    uint32_t* pFileSize,
    IotHttpsConnectionHandle_t* pConnHandle,
    const char * pPath,
    uint32_t pathLen,
    const char * pAddress,
    uint32_t addressLen,
    IotHttpsConnectionInfo_t * pConnInfo,
    uint8_t * pReqUserBuffer,
    uint32_t reqUserBufferLen,
    uint8_t * pRespUserBuffer,
    uint32_t respUserBufferLen)
{
    /* Status of HTTPS Client API. */
    IotHttpsReturnCode_t httpsClientStatus = IOT_HTTPS_OK;
    /* The HTTPS request configurations for getting the file size. */
    IotHttpsRequestInfo_t fileSizeReqConfig = { 0 };
    /* The HTTPS response configurations for getting the file size. */
    IotHttpsResponseInfo_t fileSizeRespConfig = { 0 };
    /* Synchronous request specific configurations. */
    IotHttpsSyncInfo_t reqSyncInfo = { 0 };
    /* Synchronous response specific configurations. */
    IotHttpsSyncInfo_t respSyncInfo = { 0 };
    /* Handle identifying the HTTP request. This is valid after the request has been initialized with 
       IotHttpsClient_InitializeRequest(). */
    IotHttpsRequestHandle_t fileSizeReqHandle = NULL;
    /* Handle identifying the HTTP response. This is valid after the reponse has been received with 
       IotHttpsClient_SendSync(). */
    IotHttpsResponseHandle_t fileSizeRespHandle = NULL;

    /* The status of HTTP response for this request. */
    uint16_t respStatus = IOT_HTTPS_STATUS_OK;


    /* String to store the Content-Range header field value. This header field as we are requesting in this demo is of 
       the form: "Content-Range: bytes 0-0/FILESIZE", where file size would be the length of the maximum 32 bit integer
       which is 10.  Since the header field value "bytes 0-0/FILESIZE" is less than the maximum possible Range header
       field value, we size this string to the Range header field value.*/
    char contentRangeValStr[RANGE_VALUE_MAX_LENGTH] = { 0 };
    /* The location of the file size in the contentRangeValStr. */
    char * pFileSizeStr = NULL;
    /* Size in bytes of a single character. */
    uint8_t sizeOfOneChar = 1;

    /* Update the input pConnInfo with the host address input. */
    pConnInfo->pAddress = pAddress;
    pConnInfo->addressLen = addressLen;

    /* We are retrieving the file size synchronously because we cannot run this demo without the file size anyways
       so it's OK to block. */
    reqSyncInfo.pBody = NULL;    /* This is a GET request so there is no data in the body. */
    reqSyncInfo.bodyLen = 0;    /* Since there is not data in the body the length is 0. */
    respSyncInfo.pBody = NULL;   /* We don't care about the body in this request since we want the file size that will
                                   be found in a header value. */
    respSyncInfo.bodyLen = 0;   /* Since we are not saving any response body the length is 0. */

    /* Set the request configurations. */
    fileSizeReqConfig.pPath = pPath; 
    fileSizeReqConfig.pathLen = strlen( pPath );
    fileSizeReqConfig.pHost = pAddress;
    fileSizeReqConfig.hostLen = addressLen;
    fileSizeReqConfig.method = IOT_HTTPS_METHOD_GET;  /* Performing a HEAD request with S3 requires generating a Sigv4 signature 
                                               in an Authorization header field. We work around this by performing a GET
                                               on Range: bytes=0-0. Then extracting the size of the file from the 
                                               Content-Range header field in the response. */
    fileSizeReqConfig.userBuffer.pBuffer = pReqUserBuffer;
    fileSizeReqConfig.userBuffer.bufferLen = reqUserBufferLen;
    fileSizeReqConfig.isAsync = false;
    fileSizeReqConfig.pSyncInfo = &reqSyncInfo;
    fileSizeReqConfig.pConnInfo = pConnInfo;

    /* Set the response configurations. */
    fileSizeRespConfig.userBuffer.pBuffer = pRespUserBuffer;
    fileSizeRespConfig.userBuffer.bufferLen = respUserBufferLen;
    fileSizeRespConfig.pSyncInfo = &respSyncInfo;

    /* Initialize the request to retrieve a request handle. */
    httpsClientStatus = IotHttpsClient_InitializeRequest( &fileSizeReqHandle, &fileSizeReqConfig );
    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError("An error occurred in IotHttpsClient_InitializeRequest() with error code: %d", httpsClientStatus);
        return EXIT_FAILURE;
    }

    /* Add the header to get bytes=0-0. S3 will respond with a Content-Range header that contains the size of the file 
       in it. This header will look like: "Content-Range: bytes 0-0/FILESIZE". The body will have a single byte that 
       we are ignoring. */
    httpsClientStatus = IotHttpsClient_AddHeader( fileSizeReqHandle, RANGE_HEADER_FIELD, RANGE_0_TO_0_HEADER_VALUE, strlen( RANGE_0_TO_0_HEADER_VALUE ) );
    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "Failed to write the header \"Range: bytes=0-0\" into the request. With error code: %d", httpsClientStatus );
        return EXIT_FAILURE;
    }

    /* Send the request synchronously. */
    httpsClientStatus = IotHttpsClient_SendSync( pConnHandle, fileSizeReqHandle, &fileSizeRespHandle, &fileSizeRespConfig, 0 );
    if( httpsClientStatus != IOT_HTTPS_OK )
    {
        IotLogError( "There has been an error receiving the response. The error code is: %d", httpsClientStatus );
        /* Even though there is an error we could still read headers and body that may have been 
        filled in the buffer depending on the error. */
    }

    /* Read the response status to see if we successfully got a partial content response. If we did not then
       we failed and will exit the demo. */
    httpsClientStatus = IotHttpsClient_ReadResponseStatus(fileSizeRespHandle, &respStatus );
    if( httpsClientStatus != IOT_HTTPS_OK)
    {
        IotLogError("Could not read the response status. Error code %d", httpsClientStatus);
        return EXIT_FAILURE;
    }
    if( respStatus != IOT_HTTPS_STATUS_PARTIAL_CONTENT)
    {
        IotLogError("Could not retrieve the file size. s3 responded with response status: %d", respStatus);
        return EXIT_FAILURE;
    }

    /* Get the file size by parsing the "bytes 0-0/FILESIZE" Content-Range header value string. */
    httpsClientStatus = IotHttpsClient_ReadHeader( fileSizeRespHandle, CONTENT_RANGE_HEADER_FIELD, contentRangeValStr, sizeof(contentRangeValStr) );
    if(httpsClientStatus != IOT_HTTPS_OK)
    {
        IotLogError("Could find the Content-Range header in the response. Error code %d",httpsClientStatus);
        return EXIT_FAILURE;
    }
    pFileSizeStr = strstr( contentRangeValStr, "/" ) + sizeOfOneChar;
    *pFileSize = (uint32_t)strtoul(pFileSizeStr, NULL, 10);

    return EXIT_SUCCESS;
}