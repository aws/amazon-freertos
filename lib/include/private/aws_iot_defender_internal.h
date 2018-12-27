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
 * @file aws_iot_defender_internal.h
 * @brief Internal header of Defender library. This header should not be included in
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

/*
 * @brief Format constants: Cbor and Json.
 */
#define AWS_IOT_DEFENDER_FORMAT_CBOR    1
#define AWS_IOT_DEFENDER_FORMAT_JSON    2

/*
 * @brief Default period constants if users don't provide their own.
 *
 * If metrics is sent faster than 5 minutes for one "thing", it may be throttled.
 */
#ifndef AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS
    #define AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS    ( 300 )
#endif

/*
 * @brief Default MQTT connect timeout.
 */
#ifndef AWS_IOT_DEFENDER_MQTT_CONNECT_TIMEOUT_SECONDS
    #define AWS_IOT_DEFENDER_MQTT_CONNECT_TIMEOUT_SECONDS    ( 10U )
#endif

/*
 * @brief Default MQTT subscribe timeout.
 */
#ifndef AWS_IOT_DEFENDER_MQTT_SUBSCRIBE_TIMEOUT_SECONDS
    #define AWS_IOT_DEFENDER_MQTT_SUBSCRIBE_TIMEOUT_SECONDS    ( 10U )
#endif

/*
 * @brief Default MQTT publish timeout.
 */
#ifndef AWS_IOT_DEFENDER_MQTT_PUBLISH_TIMEOUT_SECONDS
    #define AWS_IOT_DEFENDER_MQTT_PUBLISH_TIMEOUT_SECONDS    ( 10U )
#endif

/*
 * @brief Default format for metrics data serialization.
 *
 * Set default to Cbor which is more compact than Json.
 */
#ifndef AWS_IOT_DEFENDER_FORMAT
    #define AWS_IOT_DEFENDER_FORMAT    AWS_IOT_DEFENDER_FORMAT_CBOR
#endif

/*
 * @brief Default tag option for metrics data.
 *
 * AWS IoT defender service supports both long tag and short tag.
 * Set default to short tag to reduce costs.
 */
#ifndef AWS_IOT_DEFENDER_USE_LONG_TAG
    #define AWS_IOT_DEFENDER_USE_LONG_TAG    ( 0 )
#endif

/* Below is INTERNAL used only. */
/*-----------------------------------------------------------*/

/* This MUST be consistent with enum AwsIotDefenderMetricsGroup_t. */
#define _DEFENDER_METRICS_GROUP_COUNT    2

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
 * helper macros to convert seconds to milliseconds and vice versa.
 */
#define _defenderToMilliseconds( secondValue )    ( secondValue ) * 1000
#define _defenderToSeconds( millisecondValue )    ( millisecondValue ) / 1000

/**
 * Structure to hold a metrics report.
 */
typedef struct _defenderReport
{
    AwsIotSerializerEncoderObject_t object; /**< Encoder object handle. */
    uint8_t * pDataBuffer;                  /**< Raw data buffer to be published with MQTT. */
    size_t size;                            /**< Raw data size. */
} _defenderReport_t;

/**
 * Create a report, memory is allocated inside the function.
 */
bool AwsIotDefenderInternal_CreateReport( _defenderReport_t * pReport,
                                          AwsIotDefenderEventType_t * pEventType );

/**
 * This is invoked when the report is useless. Internally, memory will be freed.
 */
void AwsIotDefenderInternal_DeleteReport( _defenderReport_t * report );

/**
 * Established TCP connections metrics is added to the encoder object.
 */
AwsIotSerializerError_t AwsIotDefenderInternal_GetMetricsTcpConnections( AwsIotSerializerEncoderObject_t * pEncoderObject );

/**
 * Listening TCP ports metrics is added to the encoder object.
 */
AwsIotSerializerError_t AwsIotDefenderInternal_GetMetricsListeningTcpPorts( AwsIotSerializerEncoderObject_t * pEncoderObject );

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
                                            AwsIotNetworkTlsInfo_t * pTlsInfo,
                                            AwsIotDefenderEventType_t * pEventType );

/**
 * Set the network connection to callback MQTT.
 */
bool AwsIotDefenderInternal_SetMqttCallback( AwsIotDefenderEventType_t * pEventType );

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

void AwsIotDefenderInternal_MqttDisconnect();

void AwsIotDefenderInternal_NetworkDestroy();

/**
 * Array of bit-flag of metrics. The index is enum value of AwsIotDefenderMetricsGroup_t.
 */
extern uint32_t _AwsIotDefenderMetricsFlag[];

#endif /* ifndef _AWS_IOT_DEFENDER_INTERNAL_H_ */
