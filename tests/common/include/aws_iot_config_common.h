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

/* This file contains default configuration settings for the tests on FreeRTOS. */

#ifndef _AWS_IOT_CONFIG_COMMON_H_
#define _AWS_IOT_CONFIG_COMMON_H_

/* FreeRTOS+POSIX include. */
#include "FreeRTOS_POSIX.h"

/* Credentials include. */
#include "aws_clientcredential.h"

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
#ifndef POSIX_UNISTD_HEADER
    #define POSIX_UNISTD_HEADER       "FreeRTOS_POSIX/unistd.h"
#endif

/* Include necessary POSIX headers for platform layer types. */
#include POSIX_PTHREAD_HEADER
#include POSIX_SEMAPHORE_HEADER

/* SDK version. */
#define AWS_IOT_SDK_VERSION       "4.0.0"

/* Platform layer types. */
#define AWS_IOT_TIMER_TYPE        void *
#define AWS_IOT_MUTEX_TYPE        pthread_mutex_t
#define AWS_IOT_SEMAPHORE_TYPE    sem_t

/* Standard library function overrides. */
#define AwsIotLogging_Puts( str )           configPRINTF( ( "%s\n", str ) )
#define AwsIotQueue_Assert( expression )    configASSERT( expression )
#define AwsIotMqtt_Assert( expression )     configASSERT( expression )

/* Control the usage of dynamic memory allocation. */
#ifndef AWS_IOT_STATIC_MEMORY_ONLY
    #define AWS_IOT_STATIC_MEMORY_ONLY    ( 0 )
#endif

/* Memory allocation configuration. Note that these functions will not be affected
 * by AWS_IOT_STATIC_MEMORY_ONLY. */
#define AwsIotNetwork_Malloc    pvPortMalloc
#define AwsIotNetwork_Free      vPortFree
#define AwsIotClock_Malloc      pvPortMalloc
#define AwsIotClock_Free        vPortFree
#define AwsIotThreads_Malloc    pvPortMalloc
#define AwsIotThreads_Free      vPortFree
#define AwsIotLogging_Malloc    pvPortMalloc
#define AwsIotLogging_Free      vPortFree
/* #define AwsIotLogging_StaticBufferSize */
#define AwsIotTest_Malloc       pvPortMalloc
#define AwsIotTest_Free         vPortFree

/* Memory allocation function configuration for the MQTT library. The MQTT library
 * will be affected by AWS_IOT_STATIC_MEMORY_ONLY. */
#if AWS_IOT_STATIC_MEMORY_ONLY == 0
    #define AwsIotMqtt_MallocConnection        pvPortMalloc
    #define AwsIotMqtt_FreeConnection          vPortFree
    #define AwsIotMqtt_MallocMessage           pvPortMalloc
    #define AwsIotMqtt_FreeMessage             vPortFree
    #define AwsIotMqtt_MallocOperation         pvPortMalloc
    #define AwsIotMqtt_FreeOperation           vPortFree
    #define AwsIotMqtt_MallocSubscription      pvPortMalloc
    #define AwsIotMqtt_FreeSubscription        vPortFree
    #define AwsIotMqtt_MallocTimerEvent        pvPortMalloc
    #define AwsIotMqtt_FreeTimerEvent          vPortFree
    #define AwsIotShadow_MallocOperation       pvPortMalloc
    #define AwsIotShadow_FreeOperation         vPortFree
    #define AwsIotShadow_MallocString          pvPortMalloc
    #define AwsIotShadow_FreeString            vPortFree
    #define AwsIotShadow_MallocSubscription    pvPortMalloc
    #define AwsIotShadow_FreeSubscription      vPortFree
#endif /* if AWS_IOT_STATIC_MEMORY_ONLY == 0 */

/* Settings required to build the libraries for testing. */
#define AWS_IOT_QUEUE_ENABLE_ASSERTS                ( 1 )
#define AWS_IOT_MQTT_ENABLE_ASSERTS                 ( 1 )
#define AWS_IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES    ( 1 )
#define AWS_IOT_MQTT_TEST                           ( 1 )
#define AWS_IOT_SHADOW_ENABLE_ASSERTS               ( 1 )

/* Static memory configuration. */
#if AWS_IOT_STATIC_MEMORY_ONLY == 1
    #ifndef AWS_IOT_MESSAGE_BUFFERS
        #define AWS_IOT_MESSAGE_BUFFERS        ( 8 )
    #endif
    #ifndef AWS_IOT_MESSAGE_BUFFER_SIZE
        #define AWS_IOT_MESSAGE_BUFFER_SIZE    ( 4096 )
    #endif
    #ifndef AWS_IOT_MQTT_CONNECTIONS
        #define AWS_IOT_MQTT_CONNECTIONS       ( 2 )
    #endif
    #ifndef AWS_IOT_MQTT_SUBSCRIPTIONS
        #define AWS_IOT_MQTT_SUBSCRIPTIONS     ( 16 )
    #endif
#endif /* if AWS_IOT_STATIC_MEMORY_ONLY == 1 */

/* Platform network configuration. */
#ifndef AWS_IOT_NETWORK_RECEIVE_BUFFER_SIZE
    #define AWS_IOT_NETWORK_RECEIVE_BUFFER_SIZE    ( 4 )
#endif
#ifndef AWS_IOT_NETWORK_SHORT_TIMEOUT_MS
    #define AWS_IOT_NETWORK_SHORT_TIMEOUT_MS       ( 100 )
#endif

/* Priority of MQTT receive task. */
#ifndef AWS_IOT_MQTT_RECEIVE_TASK_PRIORITY
    #define AWS_IOT_MQTT_RECEIVE_TASK_PRIORITY    ( tskIDLE_PRIORITY + 1 )
#endif

/* Stack size of the MQTT receive task. */
#ifndef AWS_IOT_MQTT_RECEIVE_TASK_STACK_SIZE
    #define AWS_IOT_MQTT_RECEIVE_TASK_STACK_SIZE    ( 5 * configMINIMAL_STACK_SIZE )
#endif

/* MQTT library configuration. */
#ifndef AWS_IOT_MQTT_ENABLE_METRICS
    #define AWS_IOT_MQTT_ENABLE_METRICS              ( 1 )
#endif
#ifndef AWS_IOT_MQTT_MAX_CALLBACK_THREADS
    #define AWS_IOT_MQTT_MAX_CALLBACK_THREADS        ( 2 )
#endif
#ifndef AWS_IOT_MQTT_MAX_SEND_THREADS
    #define AWS_IOT_MQTT_MAX_SEND_THREADS            ( 1 )
#endif
#ifndef AWS_IOT_MQTT_RESPONSE_WAIT_MS
    #define AWS_IOT_MQTT_RESPONSE_WAIT_MS            ( 1000 )
#endif
#ifndef AWS_IOT_MQTT_RETRY_MS_CEILING
    #define AWS_IOT_MQTT_RETRY_MS_CEILING            ( 60000 )
#endif
#ifndef AWS_IOT_MQTT_TIMER_EVENT_THRESHOLD_MS
    #define AWS_IOT_MQTT_TIMER_EVENT_THRESHOLD_MS    ( 100 )
#endif

/* Shadow library configuration. */
#ifndef AWS_IOT_SHADOW_DEFAULT_MQTT_TIMEOUT_MS
    #define AWS_IOT_SHADOW_DEFAULT_MQTT_TIMEOUT_MS    ( 5000 )
#endif

/* Set test credentials and Thing Name. */
#define AWS_IOT_TEST_SERVER                clientcredentialMQTT_BROKER_ENDPOINT
#define AWS_IOT_TEST_PORT                  443
#define AWS_IOT_TEST_SECURED_CONNECTION    1
#define AWS_IOT_TEST_ROOT_CA               NULL
#define AWS_IOT_TEST_CLIENT_CERT           clientcredentialCLIENT_CERTIFICATE_PEM
#define AWS_IOT_TEST_CLIENT_CERT_LENGTH    clientcredentialCLIENT_CERTIFICATE_LENGTH
#define AWS_IOT_TEST_PRIVATE_KEY           clientcredentialCLIENT_PRIVATE_KEY_PEM
#define AWS_IOT_TEST_PRIVATE_KEY_LENGTH    clientcredentialCLIENT_PRIVATE_KEY_LENGTH
#define AWS_IOT_TEST_SHADOW_THING_NAME     clientcredentialIOT_THING_NAME

#endif /* ifndef _AWS_IOT_CONFIG_COMMON_H_ */
