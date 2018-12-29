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

/* This file contains configuration settings for the demos on FreeRTOS running
 * on Windows Simulator. */

#ifndef _AWS_IOT_CONFIG_H_
#define _AWS_IOT_CONFIG_H_

/* Library logging configuration. */
#define AWS_IOT_LOG_LEVEL_GLOBAL      AWS_IOT_LOG_INFO
#define AWS_IOT_LOG_LEVEL_PLATFORM    AWS_IOT_LOG_INFO
#define AWS_IOT_LOG_LEVEL_NETWORK     AWS_IOT_LOG_INFO
#define AWS_IOT_LOG_LEVEL_MQTT        AWS_IOT_LOG_INFO
#define AWS_IOT_LOG_LEVEL_DEFENDER    AWS_IOT_LOG_INFO
#define AWS_IOT_LOG_LEVEL_DEMO        AWS_IOT_LOG_INFO

/* Include the default configuration file at the bottom of this file. */
#include "aws_iot_config_common.h"

#endif /* ifndef _AWS_IOT_CONFIG_H_ */
