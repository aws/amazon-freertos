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

/* This file contains default configuration settings for the demos on FreeRTOS. */

#ifndef _IOT_CONFIG_COMMON_H_
#define _IOT_CONFIG_COMMON_H_

/* POSIX header overrides for FreeRTOS+POSIX. */
#ifndef POSIX_ERRNO_HEADER
    #define POSIX_ERRNO_HEADER        "FreeRTOS_POSIX/errno.h"
#endif
#ifndef POSIX_LIMITS_HEADER
    #define POSIX_LIMITS_HEADER       "FreeRTOS_POSIX.h"
#endif
#ifndef POSIX_PTHREAD_HEADER
    #define POSIX_PTHREAD_HEADER      "FreeRTOS_POSIX/pthread.h"
#endif
#ifndef POSIX_SEMAPHORE_HEADER
    #define POSIX_SEMAPHORE_HEADER    "FreeRTOS_POSIX/semaphore.h"
#endif
#ifndef POSIX_SIGNAL_HEADER
    #define POSIX_SIGNAL_HEADER       "FreeRTOS_POSIX/signal.h"
#endif
#ifndef POSIX_TIME_HEADER
    #define POSIX_TIME_HEADER         "FreeRTOS_POSIX/time.h"
#endif
#ifndef POSIX_TYPES_HEADER
    #define POSIX_TYPES_HEADER        "FreeRTOS_POSIX/sys/types.h"
#endif
#ifndef POSIX_UNISTD_HEADER
    #define POSIX_UNISTD_HEADER       "FreeRTOS_POSIX/unistd.h"
#endif

/* FreeRTOS+POSIX include. */
#include "FreeRTOS_POSIX.h"

/* Use POSIX platform types on FreeRTOS. */
//#include "platform/iot_platform_types_posix.h"
#include "platform/iot_platform_types_afr.h"

/* SDK version. */
#define IOT_SDK_VERSION    "4.0.0"

/* Standard library function overrides. */
#define IotLogging_Puts( str )                 configPRINTF( ( "%s\n", str ) )
#define IotContainers_Assert( expression )     configASSERT( expression )
#define IotMqtt_Assert( expression )           configASSERT( expression )

#define AwsIotDefender_Assert( expression )    configASSERT( expression )
#define IotMetrics_Assert( expression )        configASSERT( expression )
#define AwsIotDemo_Assert( expression )        configASSERT( expression )

/* Control the usage of dynamic memory allocation. */
#ifndef IOT_STATIC_MEMORY_ONLY
    #define IOT_STATIC_MEMORY_ONLY    ( 0 )
#endif

/* Memory allocation configuration. Note that these functions will not be affected
 * by IOT_STATIC_MEMORY_ONLY. */
#define IotNetwork_Malloc    pvPortMalloc
#define IotNetwork_Free      vPortFree
#define IotThreads_Malloc    pvPortMalloc
#define IotThreads_Free      vPortFree
#define IotLogging_Malloc    pvPortMalloc
#define IotLogging_Free      vPortFree
/* #define IotLogging_StaticBufferSize */

/* Memory allocation function configuration for the MQTT and Defender library.
 * These libraries will be affected by IOT_STATIC_MEMORY_ONLY. */
#if IOT_STATIC_MEMORY_ONLY == 0
    #define IotMqtt_MallocConnection             pvPortMalloc
    #define IotMqtt_FreeConnection               vPortFree
    #define IotMqtt_MallocMessage                pvPortMalloc
    #define IotMqtt_FreeMessage                  vPortFree
    #define IotMqtt_MallocOperation              pvPortMalloc
    #define IotMqtt_FreeOperation                vPortFree
    #define IotMqtt_MallocSubscription           pvPortMalloc
    #define IotMqtt_FreeSubscription             vPortFree

    #define AwsIotDefender_MallocReport          pvPortMalloc
    #define AwsIotDefender_FreeReport            vPortFree
    #define AwsIotDefender_MallocTopic           pvPortMalloc
    #define AwsIotDefender_FreeTopic             vPortFree

    #define AwsIotMetrics_MallocTcpConnection    pvPortMalloc
    #define AwsIotMetrics_FreeTcpConnection      vPortFree
#endif /* if IOT_STATIC_MEMORY_ONLY == 0 */

/* Static memory configuration. */
#if IOT_STATIC_MEMORY_ONLY == 1
    #ifndef IOT_MESSAGE_BUFFERS
        #define IOT_MESSAGE_BUFFERS                    ( 8 )
    #endif
    #ifndef IOT_MESSAGE_BUFFER_SIZE
        #define IOT_MESSAGE_BUFFER_SIZE                ( 4096 )
    #endif
    #ifndef IOT_MQTT_CONNECTIONS
        #define IOT_MQTT_CONNECTIONS                   ( 2 )
    #endif
    #ifndef IOT_MQTT_MAX_IN_PROGRESS_OPERATIONS
        #define IOT_MQTT_MAX_IN_PROGRESS_OPERATIONS    ( 10 )
    #endif
    #ifndef IOT_MQTT_SUBSCRIPTIONS
        #define IOT_MQTT_SUBSCRIPTIONS                 ( 16 )
    #endif
#endif /* if IOT_STATIC_MEMORY_ONLY == 1 */

/* Platform network configuration. */
#ifndef IOT_NETWORK_RECEIVE_BUFFER_SIZE
    #define IOT_NETWORK_RECEIVE_BUFFER_SIZE    ( 1024 )
#endif
#ifndef IOT_NETWORK_SHORT_TIMEOUT_MS
    #define IOT_NETWORK_SHORT_TIMEOUT_MS       ( 100 )
#endif

/* Linear containers configuration. */
#ifndef IOT_CONTAINERS_ENABLE_ASSERTS
    #define IOT_CONTAINERS_ENABLE_ASSERTS    ( 1 )
#endif

/* Priority of network receive task. */
#ifndef IOT_NETWORK_RECEIVE_TASK_PRIORITY
    #define IOT_NETWORK_RECEIVE_TASK_PRIORITY    ( tskIDLE_PRIORITY + 1 )
#endif

/* Stack size of the network receive task. */
#ifndef IOT_NETWORK_RECEIVE_TASK_STACK_SIZE
    #define IOT_NETWORK_RECEIVE_TASK_STACK_SIZE    ( 5 * configMINIMAL_STACK_SIZE )
#endif

/* Default iot thread stack_size. */
#ifndef IOT_THREAD_DEFAULT_STACK_SIZE
    #define IOT_THREAD_DEFAULT_STACK_SIZE    configMINIMAL_STACK_SIZE
#endif

/* Default iot thread priority. */
#ifndef IOT_THREAD_DEFAULT_PRIORITY
    #define IOT_THREAD_DEFAULT_PRIORITY      5
#endif

/* MQTT library configuration. */
#ifndef IOT_MQTT_ENABLE_METRICS
    #define IOT_MQTT_ENABLE_METRICS    ( 1 )
#endif
#ifndef IOT_MQTT_ENABLE_ASSERTS
    #define IOT_MQTT_ENABLE_ASSERTS        ( 1 )
#endif
#ifndef IOT_MQTT_RESPONSE_WAIT_MS
    #define IOT_MQTT_RESPONSE_WAIT_MS      ( 1000 )
#endif
#ifndef IOT_MQTT_RETRY_MS_CEILING
    #define IOT_MQTT_RETRY_MS_CEILING      ( 60000 )
#endif

/* Required settings for the MQTT demos. */
#define IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES    ( 1 )
#define IOT_MQTT_TEST                           ( 0 )

/* Enable metrics library assert. */
#ifndef AWS_IOT_METRICS_ENABLE_ASSERTS
    #define AWS_IOT_METRICS_ENABLE_ASSERTS    ( 1 )
#endif

/* Enable defender library assert. */
#ifndef AWS_IOT_DEFENDER_ENABLE_ASSERTS
    #define AWS_IOT_DEFENDER_ENABLE_ASSERTS    ( 1 )
#endif

/* Configuration for defender demo: set format to CBOR. */
#define AWS_IOT_DEFENDER_FORMAT          AWS_IOT_DEFENDER_FORMAT_CBOR

/* Configuration for defender demo: use long tag for readable output. Please use short tag for the real application. */
#define AWS_IOT_DEFENDER_USE_LONG_TAG    ( 1 )

#endif /* ifndef _IOT_CONFIG_COMMON_H_ */
