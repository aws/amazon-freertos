/*
 * FreeRTOS HTTPS Client V1.2.0
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
 * @file iot_test_access_https.h
 * @brief Declares the functions that provide access to the internal functions
 * and variables of the HTTPS Client library.
 */

#ifndef IOT_TEST_ACCESS_HTTPS_
#define IOT_TEST_ACCESS_HTTPS_

/*--------------------------- iot_https_client.c ---------------------------*/

/**
 * @brief Test access function for #_initializeResponse.
 *
 * @see #_initializeResponse.
 */
IotHttpsReturnCode_t IotTestHttps_initializeResponse( IotHttpsResponseHandle_t * pRespHandle,
                                                      IotHttpsResponseInfo_t * pRespInfo,
                                                      IotHttpsRequestHandle_t reqHandle );

/**
 * @brief Test access function for #_networkReceiveCallback.
 *
 * @see #_networkReceiveCallback.
 */
void IotTestHttps_networkReceiveCallback( void * pNetworkConnection,
                                          void * pReceiveContext );


#endif /* ifndef IOT_TEST_ACCESS_HTTPS_ */
