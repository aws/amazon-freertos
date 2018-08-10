/*
 * Amazon FreeRTOS Device Defender Agent V1.0.1
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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */
#include "aws_defender_internals.h"

static struct defender_metric_s xDEFENDER_metric_uptime_s =
{
    DEFENDER_UptimeRefresh,
    DEFENDER_UptimeReportGet,
};

DefenderMetric_t pxDEFENDER_metric_uptime = &xDEFENDER_metric_uptime_s;

cbor_handle_t DEFENDER_UptimeReportGet( void )
{
    cbor_handle_t pxUptime_metrics = CBOR_New( 0 );

    CBOR_AppendKeyWithInt( pxUptime_metrics, "ut", DEFENDER_UptimeSecondsGet() );

    return pxUptime_metrics;
}
