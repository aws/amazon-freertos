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

static struct defender_metric_s xDEFENDER_tcp_connections_s =
{
    DEFENDER_TcpConnRefresh,
    DEFENDER_TcpConnReportGet,
};

DefenderMetric_t pxDEFENDER_tcp_connections = &xDEFENDER_tcp_connections_s;

cbor_handle_t DEFENDER_TcpConnReportGet( void )
{
    cbor_handle_t pxEst_conn = CBOR_New( 0 );

    CBOR_AssignKeyWithInt( pxEst_conn, DEFENDER_total_tag, DEFENDER_TcpConnGet() );

    cbor_handle_t pxTcp_conn_metrics = CBOR_New( 0 );
    CBOR_AssignKeyWithMap( pxTcp_conn_metrics, DEFENDER_est_conn_tag, pxEst_conn );
    CBOR_Delete( &pxEst_conn );

    cbor_handle_t pxTcp_conn_report = CBOR_New( 0 );
    CBOR_AssignKeyWithMap(
        pxTcp_conn_report, DEFENDER_tcp_conn_tag, pxTcp_conn_metrics );
    CBOR_Delete( &pxTcp_conn_metrics );

    return pxTcp_conn_report;
}
