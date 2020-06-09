/*
 * FreeRTOS Defender V3.0.2
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
 * Internal header of Defender library. This header should not be included in
 * typical application code.
 */

#ifndef AWS_IOT_DEFENDER_INTERNAL_H_
#define AWS_IOT_DEFENDER_INTERNAL_H_

/* The config header is always included first. */
#include "iot_config.h"

/* Defender include. */
#include "aws_iot_defender.h"

/* Serializer include. */
#include "iot_serializer.h"

/* Platform thread include. */
#include "platform/iot_threads.h"

/* Double linked list include. */
#include "iot_linear_containers.h"

/**
 * @def AwsIotDefender_Assert( expression )
 * @brief Assertion macro for the Defender library.
 *
 * Set @ref AWS_IOT_DEFENDER_ENABLE_ASSERTS to `1` to enable assertions in the Defender
 * library.
 *
 * @param[in] expression Expression to be evaluated.
 */
#if AWS_IOT_DEFENDER_ENABLE_ASSERTS == 1
    #ifndef AwsIotDefender_Assert
        #ifdef Iot_DefaultAssert
            #define AwsIotDefender_Assert( expression )    Iot_DefaultAssert( expression )
        #else
            #error "Asserts are enabled for Defender, but AwsIotDefender_Assert is not defined"
        #endif
    #endif
#else /* if AWS_IOT_DEFENDER_ENABLE_ASSERTS == 1 */
    #define AwsIotDefender_Assert( expression )
#endif /* if AWS_IOT_DEFENDER_ENABLE_ASSERTS == 1 */

/* Configure logs for Defender functions. */
#ifdef AWS_IOT_LOG_LEVEL_DEFENDER
    #define LIBRARY_LOG_LEVEL        AWS_IOT_LOG_LEVEL_DEFENDER
#else
    #ifdef AWS_IOT_LOG_LEVEL_GLOBAL
        #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
    #else
        #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
    #endif
#endif

#define LIBRARY_LOG_NAME    ( "Defender" )
#include "iot_logging_setup.h"

/*
 * Provide default values for undefined memory allocation functions based on
 * the usage of dynamic memory allocation.
 */
#if IOT_STATIC_MEMORY_ONLY == 1
    #include "private/iot_static_memory.h"

/**
 * @brief Allocate a Defender report. This function should have the same
 * signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    #define AwsIotDefender_MallocReport    Iot_MallocMessageBuffer

/**
 * @brief Free a Defender report. This function should have the same
 * signature as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    #define AwsIotDefender_FreeReport      Iot_FreeMessageBuffer

/**
 * @brief Allocate a Defender topic. This function should have the same
 * signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    #define AwsIotDefender_MallocTopic     Iot_MallocMessageBuffer

/**
 * @brief Free a Defender topic. This function should have the same
 * signature as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    #define AwsIotDefender_FreeTopic       Iot_FreeMessageBuffer
#else /* if IOT_STATIC_MEMORY_ONLY */
    #ifndef AwsIotDefender_MallocReport
        #ifdef Iot_DefaultMalloc
            #define AwsIotDefender_MallocReport    Iot_DefaultMalloc
        #else
            #error "No malloc function defined for AwsIotDefender_MallocReport"
        #endif
    #endif

    #ifndef AwsIotDefender_FreeReport
        #ifdef Iot_DefaultFree
            #define AwsIotDefender_FreeReport    Iot_DefaultFree
        #else
            #error "No free function defined for AwsIotDefender_FreeReport"
        #endif
    #endif

    #ifndef AwsIotDefender_MallocTopic
        #ifdef Iot_DefaultMalloc
            #define AwsIotDefender_MallocTopic    Iot_DefaultMalloc
        #else
            #error "No malloc function defined for AwsIotDefender_MallocTopic"
        #endif
    #endif

    #ifndef AwsIotDefender_FreeTopic
        #ifdef Iot_DefaultFree
            #define AwsIotDefender_FreeTopic    Iot_DefaultFree
        #else
            #error "No free function defined for AwsIotDefender_FreeTopic"
        #endif
    #endif
#endif /* if IOT_STATIC_MEMORY_ONLY */

/**
 * @page Defender_Config Configuration
 * @brief Configuration settings of the Defender library
 * @par configpagemarker
 *
 * @section AWS_IOT_SECURE_SOCKETS_METRICS_ENABLED
 * @brief Enable secure sockets metrics
 * <b>Possible values:</b> 0 or 1 <br>
 * <b>Recommended values:</b> 1 <br>
 * <b>Default value (if undefined):</b> 0 <br>
 *
 * This macro must be defined for device defender library to collect sockets metrics correctly.
 * Without defining it, the behavior is unknown.
 *
 * @code{c}
 * #define AWS_IOT_SECURE_SOCKETS_METRICS_ENABLED (1)
 * @endcode
 *
 * @section AWS_IOT_DEFENDER_FORMAT
 * @brief Default format for metrics data serialization.
 *
 * <b>Possible values:</b>  #AWS_IOT_DEFENDER_FORMAT_CBOR (JSON is not supported for now) <br>
 * <b>Recommended values:</b> Cbor is more compact than Json, thus more efficient. <br>
 * <b>Default value (if undefined):</b>  #AWS_IOT_DEFENDER_FORMAT_CBOR <br>
 *
 * @section AWS_IOT_DEFENDER_USE_LONG_TAG
 * @brief Use long tag or short tag for metrics report.
 *
 * <b>Possible values:</b>  `0` or `1` <br>
 * <b>Recommended values:</b> 0 to use short tag to reduce network transmit cost. <br>
 * <b>Default value (if undefined):</b> `0` <br>
 *
 * @section AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS
 * @brief Default period constants if users don't provide their own.
 *
 * If metrics is sent faster than 5 minutes for one "thing", it may be throttled.
 *
 * <b>Possible values:</b>  greater than or equal to `300` <br>
 * <b>Recommended values:</b>  greater than or equal to `300` seconds; defender service might throttle if the period is too short <br>
 * <b>Default value (if undefined):</b>  `300` <br>
 *
 * @section AWS_IOT_DEFENDER_MQTT_CONNECT_TIMEOUT_SECONDS
 * @brief Default MQTT connect timeout.
 *
 * <b>Possible values:</b>  greater than 0 <br>
 * <b>Default value (if undefined):</b>  `10` <br>
 *
 * @section AWS_IOT_DEFENDER_MQTT_SUBSCRIBE_TIMEOUT_SECONDS
 * @brief Default MQTT subscribe timeout.
 *
 * <b>Possible values:</b>  greater than 0 <br>
 * <b>Default value (if undefined):</b>  `10` <br>
 *
 * @section AWS_IOT_DEFENDER_MQTT_PUBLISH_TIMEOUT_SECONDS
 * @brief Default MQTT publish timeout.
 *
 * <b>Possible values:</b>  greater than 0 <br>
 * <b>Default value (if undefined):</b>  `10` <br>
 */

#ifndef AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS
    #define AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS    ( 300 )
#endif

#ifndef AWS_IOT_DEFENDER_WAIT_SERVER_MAX_SECONDS
    #define AWS_IOT_DEFENDER_WAIT_SERVER_MAX_SECONDS    ( 3 )
#endif

#ifndef AWS_IOT_DEFENDER_MQTT_CONNECT_TIMEOUT_SECONDS
    #define AWS_IOT_DEFENDER_MQTT_CONNECT_TIMEOUT_SECONDS    ( 10U )
#endif

#ifndef AWS_IOT_DEFENDER_MQTT_SUBSCRIBE_TIMEOUT_SECONDS
    #define AWS_IOT_DEFENDER_MQTT_SUBSCRIBE_TIMEOUT_SECONDS    ( 10U )
#endif

#ifndef AWS_IOT_DEFENDER_MQTT_PUBLISH_TIMEOUT_SECONDS
    #define AWS_IOT_DEFENDER_MQTT_PUBLISH_TIMEOUT_SECONDS    ( 10U )
#endif

#ifndef AWS_IOT_DEFENDER_FORMAT
    #define AWS_IOT_DEFENDER_FORMAT    AWS_IOT_DEFENDER_FORMAT_CBOR
#endif

/* In current release, JSON format is not supported. */
#if AWS_IOT_DEFENDER_FORMAT == AWS_IOT_DEFENDER_FORMAT_JSON
    #error "AWS_IOT_DEFENDER_FORMAT_JSON is not supported."
#endif

/* Default to short tag to save memory and network. */
#ifndef AWS_IOT_DEFENDER_USE_LONG_TAG
    #define AWS_IOT_DEFENDER_USE_LONG_TAG    ( 0 )
#endif

/*----------------- Below this line is INTERNAL used only --------------------*/

/* This MUST be consistent with enum AwsIotDefenderMetricsGroup_t. */
#define DEFENDER_METRICS_GROUP_COUNT    1

/**
 * Define encoder/decoder based on configuration AWS_IOT_DEFENDER_FORMAT.
 */
#if AWS_IOT_DEFENDER_FORMAT == AWS_IOT_DEFENDER_FORMAT_CBOR
    #define DEFENDER_FORMAT    "cbor"
#else /* if AWS_IOT_DEFENDER_FORMAT == AWS_IOT_DEFENDER_FORMAT_CBOR */
    #error "AWS_IOT_DEFENDER_FORMAT must be AWS_IOT_DEFENDER_FORMAT_CBOR"
#endif /* if AWS_IOT_DEFENDER_FORMAT == AWS_IOT_DEFENDER_FORMAT_CBOR */

/**
 * Define a helper macro to select long tag or short tag based on configuration AWS_IOT_DEFENDER_USE_LONG_TAG.
 */
#if  AWS_IOT_DEFENDER_USE_LONG_TAG == 1
    #define AwsIotDefenderInternal_SelectTag( long_tag, short_tag )    ( long_tag )
#else
    #define AwsIotDefenderInternal_SelectTag( long_tag, short_tag )    ( short_tag )
#endif

/**
 * Convert seconds to milliseconds and vice versa.
 */
#define _defenderToMilliseconds( secondValue )    ( secondValue ) * 1000
#define _defenderToSeconds( millisecondValue ) \
    ( millisecondValue ) / 1000                \


/**
 * Structure to hold the metrics.
 */
typedef struct _defenderMetrics
{
    /**
     * Array of bit-flag of metrics. The index is enum value of AwsIotDefenderMetricsGroup_t.
     */
    uint32_t metricsFlag[ DEFENDER_METRICS_GROUP_COUNT ];

    /**
     * Mutex to protect _AwsIotDefenderMetricsFlag referenced by:
     * - metrics timer callback
     * - SetMetrics API
     */
    IotMutex_t mutex;
} _defenderMetrics_t;

/**
 * Create a report, memory is allocated inside the function.
 */
bool AwsIotDefenderInternal_CreateReport( void );

/**
 * Get the buffer pointer of report.
 */
uint8_t * AwsIotDefenderInternal_GetReportBuffer( void );

/**
 * Get the buffer size of report.
 */
size_t AwsIotDefenderInternal_GetReportBufferSize( void );

/**
 * Delete a report when it is useless. Internally, memory will be freed.
 */
void AwsIotDefenderInternal_DeleteReport( void );

/**
 * Build three topics names used by defender library.
 */
AwsIotDefenderError_t AwsIotDefenderInternal_BuildTopicsNames( void );

/**
 * Free the memory of three topics names.
 */
void AwsIotDefenderInternal_DeleteTopicsNames( void );

/**
 * Subscribe accept/reject defender topics.
 */

IotMqttError_t AwsIotDefenderInternal_MqttSubscribe( void );

/**
 * Publish metrics data to defender topic.
 */
IotMqttError_t AwsIotDefenderInternal_MqttPublish( uint8_t * pData,
                                                   size_t dataLength );

/**
 * Unsubscribe accept/reject defender topics.
 */
IotMqttError_t AwsIotDefenderInternal_MqttUnSubscribe( void );


/*----------------- Below this line are INTERNAL global variables --------------------*/

extern _defenderMetrics_t _AwsIotDefenderMetrics;

extern const IotSerializerEncodeInterface_t * _pAwsIotDefenderEncoder;
extern const IotSerializerDecodeInterface_t * _pAwsIotDefenderDecoder;

#endif /* ifndef AWS_IOT_DEFENDER_INTERNAL_H_ */
