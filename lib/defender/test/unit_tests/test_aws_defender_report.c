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
#include "aws_cbor_print.h"
#include "aws_defender_internals.h"
#include "unity_fixture.h"

TEST_GROUP( aws_defender_report );

TEST_SETUP( aws_defender_report )
{
    DefenderMetric_t metrics_list[] = {};

    ( void ) DEFENDER_MetricsInit( metrics_list );
}

TEST_TEAR_DOWN( aws_defender_report )
{
}

TEST_GROUP_RUNNER( aws_defender_report )
{
    RUN_TEST_CASE( aws_defender_report, CreateReport_creates_report_with_header );
    RUN_TEST_CASE(
        aws_defender_report, CreateReport_includes_tcp_connections_from_list );
    RUN_TEST_CASE(
        aws_defender_report, CreateReport_excludes_uptime_when_not_in_list );
}

TEST( aws_defender_report, CreateReport_creates_report_with_header )
{
    cbor_handle_t report = DEFENDER_CreateReport();
    char * rep_str = CBOR_AsString( report );
    cbor_handle_t header = CBOR_FromKeyReadMap( report, DEFENDER_header_tag );

    CBOR_Delete( &report );

    /* Header will have version key */
    bool is_header = CBOR_FindKey( header, DEFENDER_version_tag );
    CBOR_Delete( &header );
    TEST_ASSERT_TRUE_MESSAGE( is_header, rep_str );
    free( rep_str );
}

TEST( aws_defender_report, CreateReport_includes_tcp_connections_from_list )
{
    DefenderMetric_t metrics_list[] = { pxDEFENDER_tcp_connections };

    ( void ) DEFENDER_MetricsInit( metrics_list );

    cbor_handle_t report = DEFENDER_CreateReport();
    char * rep_str = CBOR_AsString( report );
    cbor_handle_t metrics = CBOR_FromKeyReadMap( report, DEFENDER_metrics_tag );
    CBOR_Delete( &report );

    bool has_tcp_conn = CBOR_FindKey( metrics, DEFENDER_tcp_conn_tag );
    CBOR_Delete( &metrics );
    TEST_ASSERT_TRUE_MESSAGE( has_tcp_conn, rep_str );
    free( rep_str );
}

TEST( aws_defender_report, CreateReport_excludes_uptime_when_not_in_list )
{
    cbor_handle_t report = DEFENDER_CreateReport();
    bool has_uptime = CBOR_FindKey( report, "ut" );

    CBOR_Delete( &report );
    TEST_ASSERT_FALSE( has_uptime );
}
