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

/**
 * Internal header of Defender library. This header should not be included in
 * typical application code.
 */

#ifndef _AWS_IOT_DEFENDER_INTERNAL_H_
#define _AWS_IOT_DEFENDER_INTERNAL_H_

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Defender include. */
#include "aws_iot_defender.h"

/* Serializer include. */
#include "aws_iot_serializer.h"

/* Platform thread include. */
#include "platform/aws_iot_threads.h"

#include "aws_iot_queue.h"

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
        #include <assert.h>
        #define AwsIotDefender_Assert( expression )    assert( expression )
    #endif
#else
    #define AwsIotDefender_Assert( expression )
#endif

/* Configure logs for Defender functions. */
#ifdef AWS_IOT_LOG_LEVEL_DEFENDER
    #define _LIBRARY_LOG_LEVEL        AWS_IOT_LOG_LEVEL_DEFENDER
#else
    #ifdef AWS_IOT_LOG_LEVEL_GLOBAL
        #define _LIBRARY_LOG_LEVEL    AWS_IOT_LOG_LEVEL_GLOBAL
    #else
        #define _LIBRARY_LOG_LEVEL    AWS_IOT_LOG_NONE
    #endif
#endif

#define _LIBRARY_LOG_NAME    ( "Defender" )
#include "aws_iot_logging_setup.h"

/*
 * Provide default values for undefined memory allocation functions based on
 * the usage of dynamic memory allocation.
 */
#if AWS_IOT_STATIC_MEMORY_ONLY == 1
    #include "platform/aws_iot_static_memory.h"

    /**
     * @brief Allocate an array of uint8_t. This function should have the same
     * signature as [malloc]
     * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
     */
    #ifndef AwsIotDefender_MallocReport
        #define AwsIotDefender_MallocReport    AwsIot_MallocDefenderReport
    #endif

    /**
     * @brief Free an array of uint8_t. This function should have the same
     * signature as [free]
     * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
     */
    #ifndef AwsIotDefender_FreeReport
        #define AwsIotDefender_FreeReport    AwsIot_FreeDefenderReport
    #endif

    /**
     * @brief Allocate an array of char. This function should have the same
     * signature as [malloc]
     * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
     */
    #ifndef AwsIotDefender_MallocTopic
        #define AwsIotDefender_MallocTopic    AwsIot_MallocDefenderTopic
    #endif

    /**
     * @brief Free an array of char. This function should have the same
     * signature as [free]
     * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
     */
    #ifndef AwsIotDefender_FreeTopic
        #define AwsIotDefender_FreeTopic    AwsIot_FreeDefenderTopic
    #endif

#else /* if AWS_IOT_STATIC_MEMORY_ONLY */
    #include <stdlib.h>

    #ifndef AwsIotDefender_MallocReport
        #define AwsIotDefender_MallocReport    malloc
    #endif

    #ifndef AwsIotDefender_FreeReport
        #define AwsIotDefender_FreeReport    free
    #endif

    #ifndef AwsIotDefender_MallocTopic
        #define AwsIotDefender_MallocTopic    malloc
    #endif

    #ifndef AwsIotDefender_FreeTopic
        #define AwsIotDefender_FreeTopic    free
    #endif

#endif /* if AWS_IOT_STATIC_MEMORY_ONLY */

/**
 * @page Defender_Config Configuration
 * @brief Configuration settings of the Defender library
 * @par configpagemarker
 *
 * @section AWS_IOT_DEFENDER_FORMAT
 * @brief Default format for metrics data serialization.
 *
 * <b>Possible values:</b>  #AWS_IOT_DEFENDER_FORMAT_CBOR or #AWS_IOT_DEFENDER_FORMAT_JSON <br>
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

#ifndef AWS_IOT_DEFENDER_USE_LONG_TAG
    #define AWS_IOT_DEFENDER_USE_LONG_TAG    ( 0 )
#endif

/*----------------- Below this line is INTERNAL used only --------------------*/

/* This MUST be consistent with enum AwsIotDefenderMetricsGroup_t. */
#define _DEFENDER_METRICS_GROUP_COUNT    1

/**
 * Define encoder/decoder based on configuration AWS_IOT_DEFENDER_FORMAT.
 */
#if AWS_IOT_DEFENDER_FORMAT == AWS_IOT_DEFENDER_FORMAT_CBOR

    #define _DEFENDER_FORMAT          "cbor"
    #define _AwsIotDefenderEncoder    _AwsIotSerializerCborEncoder /**< Global defined in aws_iot_serializer.h . */
    #define _AwsIotDefenderDecoder    _AwsIotSerializerCborDecoder /**< Global defined in aws_iot_serializer.h . */

#elif AWS_IOT_DEFENDER_FORMAT == AWS_IOT_DEFENDER_FORMAT_JSON

    #define _DEFENDER_FORMAT          "json"
    #define _AwsIotDefenderEncoder    _AwsIotSerializerJsonEncoder /**< Global defined in aws_iot_serializer.h . */
    #define _AwsIotDefenderDecoder    _AwsIotSerializerJsonDecoder /**< Global defined in aws_iot_serializer.h . */

#else /* if AWS_IOT_DEFENDER_FORMAT == AWS_IOT_DEFENDER_FORMAT_CBOR */
    #error "AWS_IOT_DEFENDER_FORMAT must be either AWS_IOT_DEFENDER_FORMAT_CBOR or AWS_IOT_DEFENDER_FORMAT_JSON."

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
#define _defenderToSeconds( millisecondValue )    ( millisecondValue ) / 1000

/**
 * Determine if serialization succeeds based on whether ignoring "small buffer" error.
 */
#define _defenderSerializeSuccess( serializerError, ignoreTooSmallBuffer )                                                           \
    ( ignoreTooSmallBuffer ? serializerError == AWS_IOT_SERIALIZER_SUCCESS || serializerError == AWS_IOT_SERIALIZER_BUFFER_TOO_SMALL \
      : serializerError == AWS_IOT_SERIALIZER_SUCCESS )                                                                              \


/**
 * Structure to hold a metrics report.
 */
typedef struct _defenderReport
{
    AwsIotSerializerEncoderObject_t object; /* Encoder object handle. */
    uint8_t * pDataBuffer;                  /* Raw data buffer to be published with MQTT. */
    size_t size;                            /* Raw data size. */
} _defenderReport_t;

/**
 * Structure to hold the metrics.
 */
typedef struct _defenderMetrics
{
    /**
     * Array of bit-flag of metrics. The index is enum value of AwsIotDefenderMetricsGroup_t.
     */
    uint32_t metricsFlag[ _DEFENDER_METRICS_GROUP_COUNT ];

    /**
     * Mutex to protect _AwsIotDefenderMetricsFlag referenced by:
     * - metrics timer callback
     * - SetMetrics API
     */
    AwsIotMutex_t mutex;
} _defenderMetrics_t;

/**
 * Callback parameters passed into IotMetricsListCallback_t.
 */
typedef struct _defenderMetricsCallbackInfo
{
    bool ignoreTooSmallBuffer;
    AwsIotSerializerEncoderObject_t * pEncoderObject;
} _defenderMetricsCallbackInfo_t;

/**
 * Create a report, memory is allocated inside the function.
 */
bool AwsIotDefenderInternal_CreateReport( _defenderReport_t * pReport,
                                          AwsIotDefenderEventType_t * pEventType );

/**
 * Delete a report when it is useless. Internally, memory will be freed.
 */
void AwsIotDefenderInternal_DeleteReport( _defenderReport_t * report );

void * AwsIotDefenderInternal_TcpConnectionsCallback( void * param1,
                                                      AwsIotList_t * pTcpConnectionsMetricsList );

/**
 * Build three topics names used by defender library.
 */
bool AwsIotDefenderInternal_BuildTopicsNames( const char * pThingName,
                                              uint16_t thingNameLength,
                                              AwsIotDefenderError_t * error );

/**
 * Free the memory of three topics names.
 */
void AwsIotDefenderInternal_DeleteTopicsNames();

/**
 * Creat a network connection.
 */
bool AwsIotDefenderInternal_NetworkConnect( const char * pAwsIotEndpoint,
                                            uint16_t port,
                                            AwsIotNetworkTlsInfo_t * pTlsInfo );

/**
 * Set the network connection to callback MQTT.
 */
bool AwsIotDefenderInternal_SetMqttCallback();

/**
 * Connect to AWS with MQTT.
 */
bool AwsIotDefenderInternal_MqttConnect( const char * pThingName,
                                         uint16_t thingNameLength,
                                         AwsIotDefenderEventType_t * pEventType );

/**
 * Subscribe accept/reject defender topics.
 */
bool AwsIotDefenderInternal_MqttSubscribe( AwsIotMqttCallbackInfo_t acceptCallback,
                                           AwsIotMqttCallbackInfo_t rejectCallback,
                                           AwsIotDefenderEventType_t * pEventType );

/**
 * Publish metrics data to defender topic.
 */
bool AwsIotDefenderInternal_MqttPublish( uint8_t * pData,
                                         size_t dataLength,
                                         AwsIotDefenderEventType_t * pEventType );

/**
 * Disconnect with AWS MQTT.
 */
void AwsIotDefenderInternal_MqttDisconnect();

/**
 * Close network connection.
 */
void AwsIotDefenderInternal_NetworkClose();

/**
 * Destory network connection.
 */
void AwsIotDefenderInternal_NetworkDestroy();

/*----------------- Below this line are INTERNAL global variables --------------------*/

extern _defenderMetrics_t _AwsIotDefenderMetrics;

#endif /* ifndef _AWS_IOT_DEFENDER_INTERNAL_H_ */
