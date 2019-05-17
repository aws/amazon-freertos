/*
 * Amazon FreeRTOS Device Defender Agent V1.0.3
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

static struct DefenderMetric_s xDefenderTCPConnectionsS =
{
    TcpConnRefresh,
    TcpConnReportGet,
};

DefenderMetric_t xDefenderTCPConnections = &xDefenderTCPConnectionsS;

CBORHandle_t TcpConnReportGet( void )
{
    CBORHandle_t xEstConn = CBOR_New( 0 );

    CBOR_AssignKeyWithInt( xEstConn, DEFENDER_TOTAL_TAG, TcpConnGet() );

    CBORHandle_t xTcpConnMetrics = CBOR_New( 0 );
    CBOR_AssignKeyWithMap( xTcpConnMetrics, DEFENDER_EST_CONN_TAG, xEstConn );
    CBOR_Delete( &xEstConn );

    CBORHandle_t xTcpConnReport = CBOR_New( 0 );
    CBOR_AssignKeyWithMap(
        xTcpConnReport, DEFENDER_TCP_CONN_TAG, xTcpConnMetrics );
    CBOR_Delete( &xTcpConnMetrics );

    return xTcpConnReport;
}
