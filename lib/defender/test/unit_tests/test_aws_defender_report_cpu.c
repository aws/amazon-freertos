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
#include "aws_defender_cpu.h"
#include "aws_defender_report_cpu.h"
#include "unity_fixture.h"

extern int DEFENDER_current_load;

TEST_GROUP( aws_defender_report_cpu );

TEST_SETUP( aws_defender_report_cpu )
{
}

TEST_TEAR_DOWN( aws_defender_report_cpu )
{
}

TEST_GROUP_RUNNER( aws_defender_report_cpu )
{
    RUN_TEST_CASE( aws_defender_report_cpu, CpuGet_has_cpu_with_integer_result );
    RUN_TEST_CASE( aws_defender_report_cpu, CpuGet_returns_result_from_LoadGet );
    RUN_TEST_CASE(
        aws_defender_report_cpu, CpuGet_updates_when_cpu_stat_refreshes );
    RUN_TEST_CASE(
        aws_defender_report_cpu, CpuGet_remains_unchanged_unless_refreshed );
}

TEST( aws_defender_report_cpu, CpuGet_has_cpu_with_integer_result )
{
    cbor_handle_t cpu_metric = DEFENDER_CpuReportGet();

    ( void ) CBOR_FromKeyReadInt( cpu_metric, "cpu" );
    eCBOR_ERR_t err = CBOR_CheckError( cpu_metric );
    CBOR_Delete( &cpu_metric );
    TEST_ASSERT_EQUAL( eCBOR_ERR_NO_ERROR, err );
}

TEST( aws_defender_report_cpu, CpuGet_returns_result_from_LoadGet )
{
    cbor_handle_t cpu_metric = DEFENDER_CpuReportGet();
    cbor_int_t actual = CBOR_FromKeyReadInt( cpu_metric, "cpu" );
    int expected = DEFENDER_CpuLoadGet();

    CBOR_Delete( &cpu_metric );
    TEST_ASSERT_EQUAL( expected, actual );
}

TEST( aws_defender_report_cpu, CpuGet_updates_when_cpu_stat_refreshes )
{
    DEFENDER_current_load++;
    DEFENDER_CpuLoadRefresh();

    int expected = DEFENDER_current_load;
    cbor_handle_t cpu_metric = DEFENDER_CpuReportGet();
    cbor_int_t actual = CBOR_FromKeyReadInt( cpu_metric, "cpu" );
    CBOR_Delete( &cpu_metric );

    TEST_ASSERT_EQUAL( expected, actual );
}

TEST( aws_defender_report_cpu, CpuGet_remains_unchanged_unless_refreshed )
{
    int expected = DEFENDER_current_load;

    DEFENDER_current_load++;

    cbor_handle_t cpu_metric = DEFENDER_CpuReportGet();
    cbor_int_t actual = CBOR_FromKeyReadInt( cpu_metric, "cpu" );
    CBOR_Delete( &cpu_metric );

    TEST_ASSERT_EQUAL( expected, actual );
}
