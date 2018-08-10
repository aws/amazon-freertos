/*
 * Amazon FreeRTOS Device Defender Agent V1.0.0
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
#include "aws_cbor.h"
#include "aws_cbor_print.h"
#include "aws_defender_internals.h"
#include "unity_fixture.h"

extern int DEFENDER_current_tcp_conn_count;

TEST_GROUP( aws_defender_report_tcp_conn );

TEST_SETUP( aws_defender_report_tcp_conn )
{
}

TEST_TEAR_DOWN( aws_defender_report_tcp_conn )
{
}

TEST_GROUP_RUNNER( aws_defender_report_tcp_conn )
{
    RUN_TEST_CASE(
        aws_defender_report_tcp_conn, TcpConnReportGet_has_total_as_integer );
    RUN_TEST_CASE( aws_defender_report_tcp_conn,
                   TcpConnReportGet_returns_result_from_ConnGet );
    RUN_TEST_CASE( aws_defender_report_tcp_conn,
                   TcpConnReportGet_updates_when_tcp_stat_refreshes );
    RUN_TEST_CASE( aws_defender_report_tcp_conn,
                   TcpConnReportGet_remains_unchanged_unless_refreshed );
}

TEST( aws_defender_report_tcp_conn, TcpConnReportGet_has_total_as_integer )
{
    cbor_handle_t tcp_report = DEFENDER_TcpConnReportGet();
    char * tcp_report_str = CBOR_AsString( tcp_report );
    cbor_handle_t tcp_metrics =
        CBOR_FromKeyReadMap( tcp_report, DEFENDER_tcp_conn_tag );

    CBOR_Delete( &tcp_report );
    cbor_handle_t est_conn =
        CBOR_FromKeyReadMap( tcp_metrics, DEFENDER_est_conn_tag );
    CBOR_Delete( &tcp_metrics );
    ( void ) CBOR_FromKeyReadInt( est_conn, DEFENDER_total_tag );
    eCBOR_ERR_t err = CBOR_CheckError( est_conn );
    CBOR_Delete( &est_conn );

    TEST_ASSERT_EQUAL_MESSAGE( eCBOR_ERR_NO_ERROR, err, tcp_report_str );
    free( tcp_report_str );
}

TEST( aws_defender_report_tcp_conn, TcpConnReportGet_returns_result_from_ConnGet )
{
    cbor_handle_t tcp_report = DEFENDER_TcpConnReportGet();
    char * tcp_report_str = CBOR_AsString( tcp_report );
    cbor_handle_t tcp_metrics =
        CBOR_FromKeyReadMap( tcp_report, DEFENDER_tcp_conn_tag );

    CBOR_Delete( &tcp_report );
    cbor_handle_t est_conn =
        CBOR_FromKeyReadMap( tcp_metrics, DEFENDER_est_conn_tag );
    CBOR_Delete( &tcp_metrics );

    int actual = CBOR_FromKeyReadInt( est_conn, DEFENDER_total_tag );
    CBOR_Delete( &est_conn );

    int expected = DEFENDER_TcpConnGet();
    TEST_ASSERT_EQUAL_MESSAGE( expected, actual, tcp_report_str );
    free( tcp_report_str );
}

TEST( aws_defender_report_tcp_conn,
      TcpConnReportGet_updates_when_tcp_stat_refreshes )
{
    DEFENDER_current_tcp_conn_count++;
    DEFENDER_TcpConnRefresh();
    int expected = DEFENDER_current_tcp_conn_count;

    cbor_handle_t tcp_report = DEFENDER_TcpConnReportGet();
    char * tcp_report_str = CBOR_AsString( tcp_report );
    cbor_handle_t tcp_metrics =
        CBOR_FromKeyReadMap( tcp_report, DEFENDER_tcp_conn_tag );
    CBOR_Delete( &tcp_report );
    cbor_handle_t est_conn =
        CBOR_FromKeyReadMap( tcp_metrics, DEFENDER_est_conn_tag );
    CBOR_Delete( &tcp_metrics );

    int actual = CBOR_FromKeyReadInt( est_conn, DEFENDER_total_tag );
    CBOR_Delete( &est_conn );

    TEST_ASSERT_EQUAL_MESSAGE( expected, actual, tcp_report_str );
    free( tcp_report_str );
}

TEST( aws_defender_report_tcp_conn,
      TcpConnReportGet_remains_unchanged_unless_refreshed )
{
    int expected = DEFENDER_current_tcp_conn_count;

    DEFENDER_current_tcp_conn_count++;

    cbor_handle_t tcp_report = DEFENDER_TcpConnReportGet();
    char * tcp_report_str = CBOR_AsString( tcp_report );
    cbor_handle_t tcp_metrics =
        CBOR_FromKeyReadMap( tcp_report, DEFENDER_tcp_conn_tag );
    CBOR_Delete( &tcp_report );
    cbor_handle_t est_conn =
        CBOR_FromKeyReadMap( tcp_metrics, DEFENDER_est_conn_tag );
    CBOR_Delete( &tcp_metrics );

    int actual = CBOR_FromKeyReadInt( est_conn, DEFENDER_total_tag );
    CBOR_Delete( &est_conn );

    TEST_ASSERT_EQUAL_MESSAGE( expected, actual, tcp_report_str );
    free( tcp_report_str );
}
