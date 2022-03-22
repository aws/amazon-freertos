/*
 * FreeRTOS V202203.00
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#ifndef REPORT_BUILDER_H_
#define REPORT_BUILDER_H_

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Metrics collector. */
#include "metrics_collector.h"

/**
 * @brief Return codes from report builder APIs.
 */
typedef enum
{
    ReportBuilderSuccess = 0,
    ReportBuilderBadParameter,
    ReportBuilderBufferTooSmall
} ReportBuilderStatus_t;

/**
 * @brief Represents metrics to be included in the report, including custom metrics.
 *
 * This demo demonstrates the use of the stack high water mark and list of
 * running task ids as custom metrics sent to AWS IoT Device Defender service.
 *
 * For more information on custom metrics, refer to the following AWS document:
 * https://docs.aws.amazon.com/iot/latest/developerguide/dd-detect-custom-metrics.html
 */
typedef struct ReportMetrics
{
    NetworkStats_t * pNetworkStats;
    uint16_t * pOpenTcpPortsArray;
    size_t openTcpPortsArrayLength;
    uint16_t * pOpenUdpPortsArray;
    size_t openUdpPortsArrayLength;
    Connection_t * pEstablishedConnectionsArray;
    size_t establishedConnectionsArrayLength;
    /* Custom metrics */
    uint32_t stackHighWaterMark;
    TaskStatus_t * pTaskStatusArray;
    size_t taskStatusArrayLength;
} ReportMetrics_t;

/**
 * @brief Generate a report in the format expected by the AWS IoT Device Defender
 * Service.
 *
 * @param[in] pBuffer The buffer to write the report into.
 * @param[in] bufferLength The length of the buffer.
 * @param[in] pMetrics Metrics to write in the generated report.
 * @param[in] majorReportVersion Major version of the report.
 * @param[in] minorReportVersion Minor version of the report.
 * @param[in] reportId Value to be used as the reportId in the generated report.
 * @param[out] pOutReportLength The length of the generated report.
 *
 * @return #ReportBuilderSuccess if the report is successfully generated;
 * #ReportBuilderBadParameter if invalid parameters are passed;
 * #ReportBuilderBufferTooSmall if the buffer cannot hold the full report.
 */
ReportBuilderStatus_t GenerateJsonReport( char * pBuffer,
                                          size_t bufferLength,
                                          const ReportMetrics_t * pMetrics,
                                          uint32_t majorReportVersion,
                                          uint32_t minorReportVersion,
                                          uint32_t reportId,
                                          size_t * pOutReportLength );

#endif /* ifndef REPORT_BUILDER_H_ */
