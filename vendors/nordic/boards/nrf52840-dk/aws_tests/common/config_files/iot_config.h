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

#ifndef _IOT_TEST_CONFIG_H_
#define _IOT_TEST_CONFIG_H_

/* Standard include. */
#include <stdbool.h>

/* Uncomment one of these definitions to override the log level configuration for
 * a specific library. */
/* #define IOT_LOG_LEVEL_PLATFORM               IOT_LOG_NONE
#define IOT_LOG_LEVEL_NETWORK                IOT_LOG_NONE
#define IOT_LOG_LEVEL_MQTT                   IOT_LOG_NONE
#define AWS_IOT_LOG_LEVEL_SHADOW             IOT_LOG_NONE */

extern bool IotBleMqtt_InitSerialize( void );
extern void IotBleMqtt_CleanupSerialize( void );
#define _IotMqtt_InitSerializeAdditional IotBleMqtt_InitSerialize
#define _IotMqtt_CleanupSerializeAdditional IotBleMqtt_CleanupSerialize

#define IOT_THREAD_DEFAULT_STACK_SIZE    (4 * configMINIMAL_STACK_SIZE)
#define IOT_THREAD_DEFAULT_PRIORITY      (tskIDLE_PRIORITY)

#define DEFAULT_NETWORK AWSIOT_NETWORK_TYPE_BLE
#define BLE_SUPPORTED 1
#define WIFI_SUPPORTED 0


/* Include the common configuration file for FreeRTOS. */
#include "iot_config_common.h"

#endif /* ifndef _IOT_TEST_CONFIG_H_ */
