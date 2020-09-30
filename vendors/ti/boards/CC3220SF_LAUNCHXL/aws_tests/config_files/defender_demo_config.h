/*
 * FreeRTOS V1.4.8
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
 * @file defender_demo_config.h
 * @brief specify defender config
 */

#ifndef _DEFENDER_DEMO_CONFIG_H_
#define _DEFENDER_DEMO_CONFIG_H_

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Include logging header files and define logging macros in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define the LIBRARY_LOG_NAME and LIBRARY_LOG_LEVEL macros.
 * 3. Include the header file "logging_stack.h".
 */

#include "logging_levels.h"

/* Logging configuration for the Defender demo. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    "Device_Defender_Demo"
#endif

#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_ERROR
#endif

#include "logging_stack.h"

/************ End of logging configuration ****************/

/**
 * @brief Size of the network buffer for MQTT packets.
 */
#define NETWORK_BUFFER_SIZE                    ( 1024U )

/**
 * @brief Size of the open TCP ports array.
 *
 * A maximum of these many open TCP ports will be sent in the device defender
 * report.
 */
#define OPEN_TCP_PORTS_ARRAY_SIZE              10

/**
 * @brief Size of the open UDP ports array.
 *
 * A maximum of these many open UDP ports will be sent in the device defender
 * report.
 */
#define OPEN_UDP_PORTS_ARRAY_SIZE              10

/**
 * @brief Size of the established connections array.
 *
 * A maximum of these many established connections will be sent in the device
 * defender report.
 */
#define ESTABLISHED_CONNECTIONS_ARRAY_SIZE     10

/**
 * @brief Size of the buffer which contains the generated device defender report.
 *
 * If the generated report is larger than this, it is rejected.
 */
#define DEVICE_METRICS_REPORT_BUFFER_SIZE      1000

/**
 * @brief Major version number of the device defender report.
 */
#define DEVICE_METRICS_REPORT_MAJOR_VERSION    1

/**
 * @brief Minor version number of the device defender report.
 */
#define DEVICE_METRICS_REPORT_MINOR_VERSION    0


#endif /* _DEFENDER_DEMO_CONFIG_H_ */
