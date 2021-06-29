/*
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* This file contains configuration settings for the demos. */

#ifndef IOT_CONFIG_H_
#define IOT_CONFIG_H_

/* Library logging configuration. IOT_LOG_LEVEL_GLOBAL provides a global log
 * level for all libraries; the library-specific settings override the global
 * setting. If both the library-specific and global settings are undefined,
 * no logs will be printed. */
#define IOT_LOG_LEVEL_GLOBAL                    IOT_LOG_NONE
#define IOT_LOG_LEVEL_DEMO                      IOT_LOG_NONE
#define IOT_LOG_LEVEL_PLATFORM                  IOT_LOG_NONE
#define IOT_LOG_LEVEL_NETWORK                   IOT_LOG_NONE
#define IOT_LOG_LEVEL_TASKPOOL                  IOT_LOG_NONE
#define IOT_LOG_LEVEL_MQTT                      IOT_LOG_NONE
#define AWS_IOT_LOG_LEVEL_SHADOW                IOT_LOG_NONE
#define AWS_IOT_LOG_LEVEL_DEFENDER              IOT_LOG_NONE
#define IOT_LOG_LEVEL_HTTPS                     IOT_LOG_NONE

/* Platform thread stack size and priority. */
#define IOT_THREAD_DEFAULT_STACK_SIZE           2048
#define IOT_THREAD_DEFAULT_PRIORITY             5
#define IOT_HTTPS_DISPATCH_TASK_COUNT           1
#define IOT_HTTPS_DISPATCH_QUEUE_SIZE           2
#define IOT_HTTPS_DISPATCH_USE_STATIC_MEMORY    1
#define IOT_HTTPS_DISPATCH_TASK_STACK_SIZE      ( configMINIMAL_STACK_SIZE * 4 )

/* Set the number loop iterations in the task pool tests. This value is smaller than
 * the default. */
#define TEST_TASKPOOL_ITERATIONS         50

/* Include the common configuration file for FreeRTOS. */
#include "iot_config_common.h"

#endif /* ifndef IOT_CONFIG_H_ */
