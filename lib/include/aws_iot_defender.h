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

/**
 * @mainpage
 *
 * ## Introduction
 * AWS IoT Device Defender is an IoT security service that allows you to audit the configuration of your devices,
 * monitor connected devices to detect abnormal behavior, and to mitigate security risks.
 * Part of it relies on an dedicated agent to collect device-side metrics and send to AWS Iot.
 *
 * Amazon FreeRTOS provides a library that allows your Amazon FreeRTOS-based devices to work with AWS IoT Device Defender.
 *
 * ## Dependencies
 * * MQTT library
 * * Serializer library
 * * Platform(POSIX) libraries
 * * Metrics library
 */

#ifndef _AWS_IOT_DEFENDER_H_
#define _AWS_IOT_DEFENDER_H_

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stdint.h>
#include <stdlib.h>

/* Network include. */
#include "platform/iot_network.h"

/* MQTT include. */
#include "iot_mqtt.h"

/**
 * @page Defender_constants Constants
 * @brief Defined constants of the Defender library.
 * - @ref DefenderFormat "Serialization Format"
 * - @ref DefenderMetricsFlags "Metrics Flags"
 * - @ref DefenderInitializers "Initializers"
 */

/**
 * @anchor DefenderFormat
 * @name Serialization Format
 *
 * @brief Format constants: Cbor or Json.
 * @warning JSON format is not supported for now.
 */
/**@{ */
#define AWS_IOT_DEFENDER_FORMAT_CBOR    1                                      /**< CBOR format. */
#define AWS_IOT_DEFENDER_FORMAT_JSON    2                                      /**< JSON format (NOT supported). */
/**@} */

/**
 * @anchor DefenderMetricsFlags
 * @name Metrics Flags
 *
 * @brief Bit flags or metrics used by @ref Defender_function_SetMetrics function.
 */
/**@{ */
#define AWS_IOT_DEFENDER_METRICS_ALL                                        0xffffffff /**< Flag to indicate including all metrics. */

#define AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL          0x00000001 /**< Total count of established TCP connections. */
#define AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_REMOTE_ADDR    0x00000004 /**< Remote address (IP:port) of established TCP connections. For example, 192.168.0.1:8000. */

/**
 * Connections metrics including only remote address. Local port number is not supported.
 *
 */
#define AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_CONNECTIONS \
    ( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_REMOTE_ADDR ) \


/**
 * Established connections metrics including connections metrics and total count.
 */
#define AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED                                                                          \
    ( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_CONNECTIONS | AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL ) \

/**@} end of DefenderMetricsFlags */

/**
 * @anchor DefenderInitializers
 * @name Initializers
 *
 * @brief Intializers of data handles.
 */
/**@{ */
#define AWS_IOT_DEFENDER_START_INFO_INITIALIZER    { 0 } /**< Initializer of #AwsIotDefenderCallbackInfo_t. */
/**@} */

/**
 * @defgroup Defender_datatypes_enums Enumerated types
 * @brief Enumerated types of the Defender library.
 *
 * @defgroup Defender_datatypes_paramstructs Parameter structures
 * @brief Structures passed as parameters to [Defender functions](@ref Defender_functions)
 *
 * These structures are passed as parameters to library functions.
 * Documentation for these structures will state the functions associated with each parameter structure and the purpose of each member.
 */

/**
 * @ingroup Defender_datatypes_enums
 * @brief Metrics group options for AwsIotDefender_SetMetrics() function.
 *
 * Metrics group is defined based on the "metrics blocks" listed in [AWS IoT Defender document]
 * (https://docs.aws.amazon.com/iot/latest/developerguide/device-defender-detect.html#DetectMetricsMessages)
 */
typedef enum
{
    AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS, /**< TCP connection metrics group. */
} AwsIotDefenderMetricsGroup_t;

/**
 * @ingroup Defender_datatypes_enums
 *  @brief Return codes of defender functions.
 */
typedef enum
{
    AWS_IOT_DEFENDER_SUCCESS = 0,      /**< Defender operation completed successfully. */
    AWS_IOT_DEFENDER_INVALID_INPUT,    /**< At least one input parameter is invalid. */
    AWS_IOT_DEFENDER_ALREADY_STARTED,  /**< Defender has been already started. */
    AWS_IOT_DEFENDER_PERIOD_TOO_SHORT, /**< Given period is too short. */
    AWS_IOT_DEFENDER_ERROR_NO_MEMORY,  /**< Defender operation failed due to memory allocation failure. */
    AWS_IOT_DEFENDER_INTERNAL_FAILURE  /**< Defender operation failed due to internal unexpected cause. */
} AwsIotDefenderError_t;

/**
 * @ingroup Defender_datatypes_enums
 * @brief Event codes passed into AwsIotDefenderCallbackInfo_t
 */
typedef enum
{
    AWS_IOT_DEFENDER_METRICS_ACCEPTED = 0,  /**< Metrics report was accepted by defender service. */
    AWS_IOT_DEFENDER_METRICS_REJECTED,      /**< Metrics report was rejected by defender service. */
    AWS_IOT_DEFENDER_FAILURE_MQTT,          /**< Defender failed to perform MQTT operation. */
    AWS_IOT_DEFENDER_FAILURE_METRICS_REPORT /**< Defender failed to create metrics report. */
} AwsIotDefenderEventType_t;

/**
 * @defgroup Defender_datatypes_paramstructs Parameter structures
 * @brief Structures passed as parameters to [Defender functions](@ref Defender_functions)
 *
 * These structures are passed as parameters to library functions.
 * Documentation for these structures will state the functions associated with each parameter structure and the purpose of each member.
 */

/**
 * @ingroup Defender_datatypes_paramstructs
 * @brief Callback parameters.
 */
typedef struct AwsIotDefenderCallbackInfo
{
    const uint8_t * pMetricsReport;      /**< The published metrics report(could be NULL). */
    size_t metricsReportLength;          /**< Length of the published metrics report. */
    const uint8_t * pPayload;            /**< The received message if there is one(could be NULL). */
    size_t payloadLength;                /**< Length of the message. */
    AwsIotDefenderEventType_t eventType; /**< Event code(always valid). */
} AwsIotDefenderCallbackInfo_t;

/**
 * @ingroup Defender_datatypes_paramstructs
 * @brief User provided callback handle.
 */
typedef struct AwsIotDefenderCallback
{
    void * param1;                                               /**< The first parameter to pass to the callback function(optional). */
    void ( * function )( void *,
                         AwsIotDefenderCallbackInfo_t * const ); /**< Callback function signature(optional). */
} AwsIotDefenderCallback_t;

/**
 * @ingroup Defender_datatypes_paramstructs
 * @brief Parameters of AwsIotDefender_Start function.
 */
typedef struct AwsIotDefenderStartInfo
{
    IotMqttNetworkInfo_t mqttNetworkInfo;    /**< MQTT Network info used by defender (required). */
    IotMqttConnectInfo_t mqttConnectionInfo; /**< MQTT connection info used by defender (required). */
    AwsIotDefenderCallback_t callback;       /**< Callback function parameter(optional). */
} AwsIotDefenderStartInfo_t;

/**
 * @page Defender_functions Functions
 * @brief Functions of the Defender library.
 * - @subpage Defender_function_SetMetrics @copybrief Defender_function_SetMetrics
 * - @subpage Defender_function_Start @copybrief Defender_function_Start
 * - @subpage Defender_function_Stop @copybrief Defender_function_Stop
 * - @subpage Defender_function_SetPeriod @copybrief Defender_function_SetPeriod
 * - @subpage Defender_function_GetPeriod @copybrief Defender_function_GetPeriod
 * - @subpage Defender_function_strerror @copybrief Defender_function_strerror
 * - @subpage Defender_function_DescribeEventType @copybrief Defender_function_DescribeEventType
 */

/**
 * @page Defender_function_SetMetrics AwsIotDefender_SetMetrics
 * @snippet this declare_defender_setmetrics
 * @brief Set metrics that defender agent needs to collect for a metrics group.
 *
 * * If defender agent is not started, this function will provide the metrics to be collected.
 * * If defender agent is started, this function will update the metrics and take effect in defender agent's next iteration.
 *
 * @param[in] metricsGroup Metrics group defined in #AwsIotDefenderMetricsGroup_t
 * @param[in] metrics Bit-flags to specify what metrics to collect.
 * If all metrics in a group is needed, simply set metrics to #AWS_IOT_DEFENDER_METRICS_ALL.
 * See @ref DefenderMetricsFlags "Metrics flags" for details.
 *
 * @return
 * * On success, #AWS_IOT_DEFENDER_SUCCESS is returned.
 * * If metricsGroup is invalid, #AWS_IOT_DEFENDER_INVALID_INPUT is returned.
 * @note This function is thread safe.
 * @note @ref Defender_function_Stop "AwsIotDefender_Stop" will clear the metrics.
 */

/* @[declare_defender_setmetrics] */
AwsIotDefenderError_t AwsIotDefender_SetMetrics( AwsIotDefenderMetricsGroup_t metricsGroup,
                                                 uint32_t metrics );
/* @[declare_defender_setmetrics] */

/**
 * @page Defender_function_Start AwsIotDefender_Start
 * @snippet this declare_defender_start
 * @brief Start the defender agent.
 *
 * @param[in] pStartInfo Pointer of parameters of start function
 *
 * Periodically, defender agent collects metrics and publish to specifc AWS reserved MQTT topic.
 *
 * @return
 * * On success, #AWS_IOT_DEFENDER_SUCCESS is returned.
 * * If pStartInfo is invalid, #AWS_IOT_DEFENDER_INVALID_INPUT is returned.
 * * If memory allocation fails, #AWS_IOT_DEFENDER_ERROR_NO_MEMORY is returned.
 * * If defender is already started, #AWS_IOT_DEFENDER_ALREADY_STARTED is returned.
 *
 * @warning This function is not thread safe.
 *
 * @note No need to manage the memory allocated for #AwsIotDefenderCallbackInfo_t. This function save the information internally.
 *
 * Example:
 *
 * @code{c}
 * void logDefenderCallback(void * param1, AwsIotDefenderCallbackInfo_t * const pCallbackInfo)
 * {
 *     const char * pEventStr = AwsIotDefender_DescribeEventType(pCallbackInfo->eventType);
 *     // log pEventStr
 *
 *     if (pCallbackInfo->pPayload != NULL)
 *     {
 *         // log pCallbackInfo->pPayload which has length pCallbackInfo->payloadLength
 *     }
 *
 *     if (pCallbackInfo->pMetricsReport != NULL)
 *     {
 *         // log pCallbackInfo->pMetricsReport which has length pCallbackInfo->metricsReportLength
 *     }
 * }
 *
 * void startDefender()
 * {
 *     // assume a valid AwsIotNetworkTlsInfo_t is created.
 *     const AwsIotNetworkTlsInfo_t tlsInfo;
 *
 *     // define a simple callback function which simply logs
 *     const AwsIotDefenderCallback_t callback = { .function = logDefenderCallback,.param1 = NULL };
 *
 *     // define parameters of AwsIotDefender_Start function
 *     const AwsIotDefenderStartInfo_t startInfo = {
 *           .tlsInfo = tlsInfo,                                    // copy TLS info
 *           .pAwsIotEndpoint = "iot endpoint",
 *           .port = 8883,
 *           .pThingName = "some thing name",
 *           .thingNameLength = strlen("some thing name"),
 *           .callback = callback };
 *
 *     // specify two TCP connections metrics: total count and local port
 *     AwsIotDefenderError_t error = AwsIotDefender_SetMetrics(AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS,
 *                                                             AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL |
 *                                                             AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_REMOTE_ADDR );
 *
 *     if (error == AWS_IOT_DEFENDER_SUCCESS)
 *     {
 *         // set metrics report period to 10 minutes (600 seconds)
 *         error = AwsIotDefender_SetPeriod(600);
 *     }
 *
 *     if (error == AWS_IOT_DEFENDER_SUCCESS)
 *     {
 *         // start the defender
 *         error = AwsIotDefender_Start(&startInfo);
 *     }
 *
 *     if (error != AWS_IOT_DEFENDER_SUCCESS)
 *     {
 *         const char * pError = AwsIotDefender_strerror(error);
 *         // log pError
 *     }
 * }
 *
 * void stopDefender()
 * {
 *     //stop the defender
 *     AwsIotDefender_Stop();
 * }
 *
 * @endcode
 */
/* @[declare_defender_start] */
AwsIotDefenderError_t AwsIotDefender_Start( AwsIotDefenderStartInfo_t * pStartInfo );
/* @[declare_defender_start] */

/**
 * @page Defender_function_Stop AwsIotDefender_Stop
 * @snippet this declare_defender_stop
 * @brief Stop the defender agent.
 *
 * @warning This function must be called after successfully calling @ref Defender_function_Start "AwsIotDefender_Start".
 * @warning This function is not thread safe.
 */
/* @[declare_defender_stop] */
void AwsIotDefender_Stop( void );
/* @[declare_defender_stop] */

/**
 * @page Defender_function_SetPeriod AwsIotDefender_SetPeriod
 * @snippet this declare_defender_setperiod
 * @brief Set period in seconds.
 *
 *
 * @param[in] periodSeconds Period is specified in seconds. Mininum is 300 (5 minutes)
 *
 * @return
 * * On success, #AWS_IOT_DEFENDER_SUCCESS is returned.
 * * If defender is not started yet, AWS_IOT_DEFENDER_NOT_STARTED is returned.
 *
 * @warning This function is not thread safe.
 *
 * @note If this function is called when defender agent is started, the period is re-calculated and updated in next iteration.
 */
/* @[declare_defender_setperiod] */
AwsIotDefenderError_t AwsIotDefender_SetPeriod( uint32_t periodSeconds );
/* @[declare_defender_setperiod] */

/**
 * @page Defender_function_GetPeriod AwsIotDefender_GetPeriod
 * @snippet this declare_defender_getperiod
 * @brief Get period in seconds.
 */
/* @[declare_defender_getperiod] */
uint32_t AwsIotDefender_GetPeriod( void );
/* @[declare_defender_getperiod] */

/**
 * @page Defender_function_strerror AwsIotDefender_strerror
 * @snippet this declare_defender_strerror
 * @brief Return a string that describes #AwsIotDefenderError_t
 */
/* @[declare_defender_strerror] */
const char * AwsIotDefender_strerror( AwsIotDefenderError_t error );
/* @[declare_defender_strerror] */

/**
 * @page Defender_function_DescribeEventType AwsIotDefender_DescribeEventType
 * @snippet this declare_defender_describeeventtype
 * @brief Return a string that describes #AwsIotDefenderEventType_t
 */
/* @[declare_defender_describeeventtype] */
const char * AwsIotDefender_GetEventError();
/* @[declare_defender_describeeventtype] */

#endif /* end of include guard: _AWS_IOT_DEFENDER_H_ */
