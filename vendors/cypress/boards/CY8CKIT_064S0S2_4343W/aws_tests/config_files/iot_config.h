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

#include <stdbool.h>

/* MQTT demo configuration. */
#define IOT_DEMO_MQTT_PUBLISH_BURST_COUNT    ( 10 )
#define IOT_DEMO_MQTT_PUBLISH_BURST_SIZE     ( 2 )

/* Shadow demo configuration. The demo publishes periodic Shadow updates and responds
 * to changing Shadows. */
#define AWS_IOT_DEMO_SHADOW_UPDATE_COUNT        ( 20 )   /* Number of updates to publish. */
#define AWS_IOT_DEMO_SHADOW_UPDATE_PERIOD_MS    ( 3000 ) /* Period of Shadow updates. */

/* Library logging configuration. IOT_LOG_LEVEL_GLOBAL provides a global log
 * level for all libraries; the library-specific settings override the global
 * setting. If both the library-specific and global settings are undefined,
 * no logs will be printed. */
#define IOT_LOG_LEVEL_GLOBAL                    IOT_LOG_NONE
#define IOT_LOG_LEVEL_DEMO                      IOT_LOG_NONE
#define IOT_LOG_LEVEL_PLATFORM                  IOT_LOG_NONE
#define IOT_LOG_LEVEL_NETWORK                   IOT_LOG_INFO
#define IOT_LOG_LEVEL_TASKPOOL                  IOT_LOG_NONE
#define IOT_LOG_LEVEL_MQTT                      IOT_LOG_NONE
#define AWS_IOT_LOG_LEVEL_SHADOW                IOT_LOG_NONE
#define AWS_IOT_LOG_LEVEL_DEFENDER              IOT_LOG_NONE


/* Provide additional serializer initialization functions. */
extern bool IotBleMqtt_InitSerialize( void );
extern void IotBleMqtt_CleanupSerialize( void );
#define _IotMqtt_InitSerializeAdditional IotBleMqtt_InitSerialize
#define _IotMqtt_CleanupSerializeAdditional IotBleMqtt_CleanupSerialize

/* Platform thread priority. */
#define IOT_THREAD_DEFAULT_PRIORITY      5

#define BLE_SUPPORTED      ( 1 )
#define WIFI_SUPPORTED     ( 1 )

/* Network type configuration for this board. */
#ifndef DEFAULT_NETWORK
#define DEFAULT_NETWORK    AWSIOT_NETWORK_TYPE_WIFI
#endif

/* Increasing MQTT and shadow timeout for the board, as there is a delay in sending large payload over BLE for the board. */
#define IOT_TEST_MQTT_TIMEOUT_MS   ( 30000 )
#define AWS_IOT_TEST_SHADOW_TIMEOUT ( 30000 )



/* Include the common configuration file for FreeRTOS. */
#include "iot_config_common.h"

#endif /* ifndef IOT_CONFIG_H_ */
