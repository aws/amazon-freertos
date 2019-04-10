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
 * @file iot_common.h
 * @brief Provides function signatures for intialization and cleanup of common
 * libraries.
 */

#ifndef _IOT_COMMON_H_
#define _IOT_COMMON_H_

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stdbool.h>

/**
 * @brief One-time initialization function for all common libraries.
 *
 * This function initializes common libraries, such as static memory and task
 * pool. <b>It must be called once (and only once) before calling any other
 * function in this SDK.</b> Calling this function more than once without first
 * calling `IotCommon_Cleanup` may result in a crash.
 *
 * @return `true` if initialization succeeded; `false` otherwise. Logs may be
 * printed in case of failure.
 *
 * @warning No thread-safety guarantees are provided for this function.
 */
bool IotCommon_Init( void );

/**
 * @brief One-time deinitialization function for all common libraries.
 *
 * This function frees resources taken in `IotCommon_Init`. No other function
 * in this SDK may be called after calling this function unless `IotCommon_Init`
 * is called again.
 *
 * @warning No thread-safety guarantees are provided for this function.
 */
void IotCommon_Cleanup( void );

#endif
