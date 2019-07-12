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
 * @file iot_demo_https_common.h
 * @brief Common routines used in the HTTPS S3 access demos.
 */

#ifndef IOT_DEMO_HTTPS_COMMON_H_
#define IOT_DEMO_HTTPS_COMMON_H_

#include "iot_https_client.h"

/**
 * @brief Retrieve the size of the S3 object that is specified in pPath.
 * 
 * @param[out] pFileSize - The size of the S3 object.
 * @param[out] pConnHandle - When a connection is made implicitly here, then a valid connection handle will be returned.
 * @param[in] pPath - The path to the S3 object. This includes all of the queries in the pre-signed URL as well.
 * @param[in] pathLen - The length of the path to the S3 object. This length includes all of the queries following as well.
 * @param[in] pAddress - The address of the S3 host extracted from the presigned S3 URL.
 * @param[in] addressLen - The length of pAddress.
 * @param[in] pConnInfo - HTTPS Client library connection information. The pAddress and addressLen will be replaced with the parameters here. All other fields must be valid for a connection.
 * @param[in] pReqUserBuffer - Buffer for storing the request context and headers.
 * @param[in] reqUserBufferLen - Length of the request user buffer.
 * @param[in] pRespUserBuffer - Buffer for storing the response context and headers.
 * @param[in] respUserBufferLen - Length of the response user buffer.
 */
int _IotHttpsDemo_GetS3ObjectFileSize( uint32_t* pFileSize,
                                       IotHttpsConnectionHandle_t* pConnHandle,
                                       const char * pPath,
                                       uint32_t pathLen,
                                       const char * pAddress,
                                       uint32_t addressLen,
                                       IotHttpsConnectionInfo_t * pConnInfo,
                                       uint8_t * pReqUserBuffer,
                                       uint32_t reqUserBufferLen,
                                       uint8_t * pRespUserBuffer,
                                       uint32_t respUserBufferLen);

#endif