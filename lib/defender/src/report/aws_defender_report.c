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
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static DefenderMetric_t xDEFENDER_metrics_list[ DEFENDER_MAX_METRICS_COUNT ];
static int32_t xDEFENDER_metrics_count;

DefenderErr_t DEFENDER_MetricsInitFunc( DefenderMetric_t * ppxMetrics,
                                        int32_t xMetric_count )
{
    int32_t lI;

    if( DEFENDER_MAX_METRICS_COUNT < xMetric_count )
    {
        return eDefenderErrTooManyMetrics;
    }

    xDEFENDER_metrics_count = xMetric_count;

    for( lI = 0; lI < xMetric_count; ++lI )
    {
        xDEFENDER_metrics_list[ lI ] = ppxMetrics[ lI ];
    }

    return 0;
}

cbor_handle_t DEFENDER_CreateReport( void )
{
    cbor_handle_t pxReport = CBOR_New( 0 );

    cbor_handle_t pxHeader = DEFENDER_GetHeader();

    CBOR_AppendKeyWithMap( pxReport, DEFENDER_header_tag, pxHeader );
    CBOR_Delete( &pxHeader );

    cbor_handle_t pxMetrics = CBOR_New( 0 );

    for( int xI = 0; xI < xDEFENDER_metrics_count; ++xI )
    {
        xDEFENDER_metrics_list[ xI ]->UpdateMetric();
        cbor_handle_t pxMetric = xDEFENDER_metrics_list[ xI ]->ReportMetric();
        CBOR_AppendMap( pxMetrics, pxMetric );
        CBOR_Delete( &pxMetric );
    }

    CBOR_AppendKeyWithMap( pxReport, DEFENDER_metrics_tag, pxMetrics );
    CBOR_Delete( &pxMetrics );

    if( eCBOR_ERR_NO_ERROR != CBOR_CheckError( pxReport ) )
    {
        CBOR_Delete( &pxReport );
    }

    return pxReport;
}
