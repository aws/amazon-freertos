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
 * @file aws_iot_defender.h
 * @brief
 */

#ifndef _AWS_IOT_DEFENDER_H_
#define _AWS_IOT_DEFENDER_H_

/* Standard includes. */
#include <stdint.h>

/* MQTT include. */
#include "aws_iot_mqtt.h"

/* Network include. */
#include "platform/aws_iot_network.h"

/**
 * This enum type is the ID of metrics. It can be specified in AwsIotDefender_SetMetrics function.
 */
typedef enum
{
    AWS_IOT_DEFENDER_METRIC_TCP_ESTABLISHED_TOTAL,
} AwsIotDefenderMetric_t;

/**
 * Return codes of defender functions.
 */
typedef enum
{
    AWS_IOT_DEFENDER_SUCCESS = 0,
    AWS_IOT_DEFENDER_ALREADY_STARTED,
    AWS_IOT_DEFENDER_PERIOD_TOO_SHORT,
    AWS_IOT_DEFENDER_PERIOD_TOO_LARGE,
    AWS_IOT_DEFENDER_NO_MEMORY,

    AWS_IOT_DEFENDER_INTERNAL_FAILURE
} AwsIotDefenderError_t;

/**
 * Root cause code of failure, used in callback
 */
typedef enum
{
    AWS_IOT_DEFENDER_METRICS_ACCEPTED = 0,
    AWS_IOT_DEFENDER_METRICS_REJECTED,
    AWS_IOT_DEFENDER_NETWORK_CONNECTION_FAILED,
    AWS_IOT_DEFENDER_NETWORK_SET_CALLBACK_FAILED,
    AWS_IOT_DEFENDER_MQTT_CONNECTION_FAILED,
    AWS_IOT_DEFENDER_MQTT_SUBSCRIPTION_FAILED,
    AWS_IOT_DEFENDER_MQTT_PUBLISH_FAILED
} AwsIotDefenderEventType_t;

/**
 * Callback parameters.
 */
typedef struct AwsIotDefenderCallbackInfo
{
    const uint8_t * pPayload; /**< The received message if there is. */
    size_t payloadLength;     /**< Length of the message. */
    AwsIotDefenderEventType_t eventType;
} AwsIotDefenderCallbackInfo_t;

/**
 * Callback handle.
 */
typedef struct AwsIotDefenderCallback
{
    void * param1;
    void ( * function )( void *,
                         AwsIotDefenderCallbackInfo_t * const );
} AwsIotDefenderCallback_t;

/**
 * Parameters of AwsIotDefender_Start function.
 */
typedef struct AwsIotDefenderStartInfo
{
    const char * pAwsIotEndpoint;

    uint16_t port;

    AwsIotNetworkTlsInfo_t tlsInfo;

    const char * pThingName;
    uint16_t thingNameLength;

    AwsIotDefenderCallback_t callback;
} AwsIotDefenderStartInfo_t;

/**
 * Helper macro for setting an array of metrics.
 *
 * metricsArray: must be a type of array. Do not pass pointer.
 */
#define AwsIotDefender_SetMetricsHelper( metricsArray ) \
    AwsIotDefender_SetMetrics(                          \
        metricsArray, sizeof( metricsArray ) / sizeof( AwsIotDefenderMetric_t ) )

/**
 * Set an array of metrics.
 */
AwsIotDefenderError_t AwsIotDefender_SetMetrics( const AwsIotDefenderMetric_t * const pMetricsArray,
                                                 size_t metricsCount );

/**
 * Start the defender */
AwsIotDefenderError_t AwsIotDefender_Start( AwsIotDefenderStartInfo_t * pStartInfo );

/**
 * Stop the defender
 */
AwsIotDefenderError_t AwsIotDefender_Stop( void );

/**
 * Set period in seconds.
 */
AwsIotDefenderError_t AwsIotDefender_SetPeriod( uint64_t periodSeconds );

/**
 * Get period in seconds.
 */
uint64_t AwsIotDefender_GetPeriod( void );

/**
 * Return a string that describes AwsIotDefenderError_t
 */
const char * AwsIotDefender_strerror( AwsIotDefenderError_t error );

/**
 * Return a string that describes AwsIotDefenderEventType_t
 */
const char * AwsIotDefender_DescribeEventType( AwsIotDefenderEventType_t eventType );

#endif /* end of include guard: _AWS_IOT_DEFENDER_H_ */
