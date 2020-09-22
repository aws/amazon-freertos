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
 * @file iot_demo_https_common.h
 * @brief Common routines used in the HTTPS S3 access demos.
 */

#ifndef IOT_DEMO_HTTPS_COMMON_H_
#define IOT_DEMO_HTTPS_COMMON_H_

/* C standard includes shared among the demos. */
#include <stdio.h>

/* Include of the HTTPS Client library. */
#include "iot_https_client.h"

/**
 * @brief The maximum size of the header value string for the "Range" field.
 *
 * This is used to specify which parts of the file
 * we want to download. Let's say the maximum file size is what can fit in a 32 bit unsigned integer. 2^32 = 4294967296
 * which is 10 digits. The header value string is of the form: "bytes=N-M" where N and M are integers. So the length
 * of this string is strlen(N) + strlen(M) + strlen("bytes=-") + NULL terminator. Given the maximum number of digits is
 * 10 we get the maximum length of this header value as: 10 * 2 + 7 + 1.
 */
#define RANGE_VALUE_MAX_LENGTH                       ( 28 )

/**
 * @brief HTTP standard header field "Range".
 */
#define RANGE_HEADER_FIELD                           "Range"
#define RANGE_HEADER_FIELD_LENGTH                    ( sizeof( RANGE_HEADER_FIELD ) - 1 ) /**< Length of the "Range" header field name. */

/**
 * @brief A closed connection header field and value strings.
 *
 * This value appears for the HTTP header "Connection". If this appears in the response, then the server will have
 * closed the connection after sending that response.
 */
#define CONNECTION_HEADER_FIELD                      "Connection"
#define CONNECTION_HEADER_FILED_LENGTH               ( sizeof( CONNECTION_HEADER_FIELD ) - 1 )
#define CONNECTION_CLOSE_HEADER_VALUE                "close"
#define CONNECTION_CLOSE_HEADER_VALUE_LENGTH         ( sizeof( CONNECTION_CLOSE_HEADER_VALUE ) - 1 )
#define CONNECTION_KEEP_ALIVE_HEADER_VALUE           "keep-alive"
#define CONNECTION_KEEP_ALIVE_HEADER_VALUE_LENGTH    ( sizeof( CONNECTION_KEEP_ALIVE_HEADER_VALUE ) - 1 )

/**
 * @brief Retrieve the size of the S3 object that is specified in pPath.
 *
 * @param[out] pFileSize - The size of the S3 object.
 * @param[in] connHandle - A valid connected connection handle.
 * @param[in] pPath - The path to the S3 object. This includes all of the queries in the pre-signed URL as well.
 * @param[in] pathLen - The length of the path to the S3 object. This length includes all of the queries following as well.
 * @param[in] pAddress - The address of the S3 host extracted from the presigned S3 URL.
 * @param[in] addressLen - The length of pAddress.
 * @param[in] pReqUserBuffer - Buffer for storing the request context and headers.
 * @param[in] reqUserBufferLen - Length of the request user buffer.
 * @param[in] pRespUserBuffer - Buffer for storing the response context and headers.
 * @param[in] respUserBufferLen - Length of the response user buffer.
 */
int _IotHttpsDemo_GetS3ObjectFileSize( uint32_t * pFileSize,
                                       IotHttpsConnectionHandle_t connHandle,
                                       const char * pPath,
                                       uint32_t pathLen,
                                       const char * pAddress,
                                       uint32_t addressLen,
                                       uint8_t * pReqUserBuffer,
                                       uint32_t reqUserBufferLen,
                                       uint8_t * pRespUserBuffer,
                                       uint32_t respUserBufferLen );

#endif /* ifndef IOT_DEMO_HTTPS_COMMON_H_ */
