/*
 * FreeRTOS V1.2.7
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
 * @file aws_pkcs11_config.h
 * @brief PCKS#11 config options.
 */


#ifndef _AWS_PKCS11_CONFIG_H_
#define _AWS_PKCS11_CONFIG_H_

#include "FreeRTOS.h"

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Include logging header files and define logging macros in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define the LIBRARY_LOG_NAME and LIBRARY_LOG_LEVEL macros depending on
 * the logging configuration for PKCS #11.
 * 3. Include the header file "logging_stack.h", if logging is enabled for PKCS #11.
 */
#include "logging_levels.h"

/* Logging configuration for the PKCS #11 library. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    "PKCS11"
#endif

#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_ERROR
#endif

#include "logging_stack.h"

/**
 * @brief Malloc API used by core_pkcs11.h
 */
#define PKCS11_MALLOC pvPortMalloc

/**
 * @brief Free API used by core_pkcs11.h
 */
#define PKCS11_FREE vPortFree

/**
 * @brief File storage location definitions.
 */
#define pkcs11configFILE_NAME_CLIENT_CERTIFICATE    "ESP_P11_Cert"
#define pkcs11configFILE_NAME_KEY                   "ESP_P11_Key"

#endif /* _AWS_PKCS11_CONFIG_H_ include guard. */
