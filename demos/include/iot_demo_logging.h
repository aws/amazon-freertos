/*
 * FreeRTOS V202203.00
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
 * @file iot_demo_logging.h
 * @brief Sets the log level for all demos.
 */

#ifndef IOT_DEMO_LOGGING_H_
#define IOT_DEMO_LOGGING_H_

/* The config header is always included first. */
#include "iot_config.h"

/* Configure logs for the demos. The demos will have a log level of:
 * - IOT_LOG_LEVEL_DEMO if defined.
 * - IOT_LOG_LEVEL_GLOBAL if defined and IOT_LOG_LEVEL_DEMO is undefined.
 * - IOT_LOG_NONE if neither IOT_LOG_LEVEL_DEMO nor IOT_LOG_LEVEL_GLOBAL are defined.
 */
#ifdef IOT_LOG_LEVEL_DEMO
    #define LIBRARY_LOG_LEVEL        IOT_LOG_LEVEL_DEMO
#else
    #ifdef IOT_LOG_LEVEL_GLOBAL
        #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
    #else
        #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
    #endif
#endif

/* Set the library name to print with the demos. */
#define LIBRARY_LOG_NAME    ( "DEMO" )

/* Include the logging setup header. This enables the logs. */
#include "iot_logging_setup.h"

#endif /* ifndef IOT_DEMO_LOGGING_H_ */
