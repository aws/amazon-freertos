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
 * @brief User-facing functions and structs of AWS IoT Device Defender libraries
 *
 * [Link to AWS documentation](https://docs.aws.amazon.com/iot/latest/developerguide/device-defender-detect.html)
 *
 */

#ifndef _AWS_IOT_DEFENDER_H_
#define _AWS_IOT_DEFENDER_H_

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stdint.h>

/* MQTT include. */
#include "aws_iot_mqtt.h"

/* Platform network include. */
#include "platform/aws_iot_network.h"

#define AWS_IOT_DEFENDER_METRICS_ALL                                        0xffffffff /**< @brief Flag to indicate including all metrics. */

#define AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL          0x00000001 /**< @brief Total count of established TCP connections. */
#define AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_LOCAL_PORT     0x00000002 /**< @brief Local port number of established TCP connections. */
#define AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_REMOTE_ADDR    0x00000004 /**< @brief Remote address (IP:port) of established TCP connections. For example, 192.168.0.1:8000. */

/**
 * @brief Connections metrics including local port number and remote address.
 */
#define AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_CONNECTIONS                                                                   \
    ( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_LOCAL_PORT | AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_REMOTE_ADDR ) \


/**
 * @brief Established connections metrics including connections metrics and total count.
 */
#define AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED                                                                          \
    ( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_CONNECTIONS | AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL ) \

#define AWS_IOT_DEFENDER_METRICS_LISTENING_TCP_TOTAL    0x00000001 /**< @brief Total count of listening TCP ports. */
#define AWS_IOT_DEFENDER_METRICS_LISTENING_TCP_PORT     0x00000002 /**< @brief Port number of listening TCP ports. */

/**
 * @brief Ports metrics including port number.
 */
#define AWS_IOT_DEFENDER_METRICS_LISTENING_TCP_PORTS    ( AWS_IOT_DEFENDER_METRICS_LISTENING_TCP_PORT )


#define AWS_IOT_DEFENDER_START_INFO_INITIALIZER         { 0 }

/**
 * @brief Metrics group options for AwsIotDefender_SetMetrics() function.
 */
typedef enum
{
    AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
    AWS_IOT_DEFENDER_METRICS_LISTENING_TCP,
} AwsIotDefenderMetricsGroup_t;

/**
 *  @brief Return codes of defender functions.
 */
typedef enum
{
    AWS_IOT_DEFENDER_SUCCESS = 0,
    AWS_IOT_DEFENDER_INVALID_INPUT,
    AWS_IOT_DEFENDER_ALREADY_STARTED,
    AWS_IOT_DEFENDER_PERIOD_TOO_SHORT,
    AWS_IOT_DEFENDER_PERIOD_TOO_LONG,
    AWS_IOT_DEFENDER_ERROR_NO_MEMORY,
    AWS_IOT_DEFENDER_INTERNAL_FAILURE
} AwsIotDefenderError_t;

/**
 * @brief Event codes passed into AwsIotDefenderCallbackInfo_t
 */
typedef enum
{
    AWS_IOT_DEFENDER_METRICS_ACCEPTED = 0,
    AWS_IOT_DEFENDER_METRICS_REJECTED,
    AWS_IOT_DEFENDER_NETWORK_CONNECTION_FAILED,
    AWS_IOT_DEFENDER_NETWORK_SET_CALLBACK_FAILED,
    AWS_IOT_DEFENDER_MQTT_CONNECTION_FAILED,
    AWS_IOT_DEFENDER_MQTT_SUBSCRIPTION_FAILED,
    AWS_IOT_DEFENDER_MQTT_PUBLISH_FAILED,
    AWS_IOT_DEFENDER_METRICS_SERIALIZATION_FAILED,
    AWS_IOT_DEFENDER_EVENT_NO_MEMORY
} AwsIotDefenderEventType_t;

/**
 * @brief Callback parameters.
 */
typedef struct AwsIotDefenderCallbackInfo
{
    const uint8_t * pMetricsReport;      /**< The published metrics report if there is one. */
    size_t metricsReportLength;          /**< Length of the published metrics report. */
    const uint8_t * pPayload;            /**< The received message if there is one. */
    size_t payloadLength;                /**< Length of the message. */
    AwsIotDefenderEventType_t eventType; /**< Event code. */
} AwsIotDefenderCallbackInfo_t;

/**
 * @brief User provided callback handle.
 */
typedef struct AwsIotDefenderCallback
{
    void * param1; /**< @brief The first parameter to pass to the callback function. */
    void ( * function )( void *,
                         AwsIotDefenderCallbackInfo_t * const );
} AwsIotDefenderCallback_t;

/**
 * @brief Parameters of AwsIotDefender_Start function.
 */
typedef struct AwsIotDefenderStartInfo
{
    const char * pAwsIotEndpoint;

    uint16_t port;

    AwsIotNetworkTlsInfo_t * pTlsInfo;

    const char * pThingName;
    uint16_t thingNameLength;

    AwsIotDefenderCallback_t callback;
} AwsIotDefenderStartInfo_t;


/**
 * @brief Set metrics that defender needs to collect for a metrics group.
 *
 * @param[in] metricsGroup Metrics group defined in AwsIotDefenderMetricsGroup_t
 * @param[in] metrics Bit-flag to define what metrics defender needs to collect
 */
AwsIotDefenderError_t AwsIotDefender_SetMetrics( AwsIotDefenderMetricsGroup_t metricsGroup,
                                                 uint32_t metrics );

/**
 * @brief Start the defender agent.
 *
 * Periodically, defender agent collects metrics and publish to specifc AWS reserved MQTT topic.
 */
AwsIotDefenderError_t AwsIotDefender_Start( AwsIotDefenderStartInfo_t * pStartInfo );

/**
 * @brief Stop the defender
 */
AwsIotDefenderError_t AwsIotDefender_Stop( void );

/**
 * @brief Set period in seconds.
 *
 * @param[in] periodSeconds Period is specified in seconds. Mininum is 300 (5 minutes)
 */
AwsIotDefenderError_t AwsIotDefender_SetPeriod( uint64_t periodSeconds );

/**
 * @brief Get period in seconds.
 */
uint64_t AwsIotDefender_GetPeriod( void );

/**
 * @brief Return a string that describes AwsIotDefenderError_t
 */
const char * AwsIotDefender_strerror( AwsIotDefenderError_t error );

/**
 * @brief Return a string that describes AwsIotDefenderEventType_t
 */
const char * AwsIotDefender_DescribeEventType( AwsIotDefenderEventType_t eventType );

#endif /* end of include guard: _AWS_IOT_DEFENDER_H_ */
