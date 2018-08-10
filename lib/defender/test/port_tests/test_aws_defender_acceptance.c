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
#include "aws_defender.h"
#include "unity_fixture.h"
#include <assert.h>

void test_delay( int sec )
{
    assert( 0 <= sec );
}

TEST_GROUP( aws_defender_acceptance );

TEST_SETUP( aws_defender_acceptance )
{
}

TEST_TEAR_DOWN( aws_defender_acceptance )
{
}

TEST_GROUP_RUNNER( aws_defender_acceptance )
{
    RUN_TEST_CASE( aws_defender_acceptance, acceptance_test );
}

TEST( aws_defender_acceptance, acceptance_test )
{
    DefenderMetric_t metrics_list[] =
    {
        /* DEFENDER_listening_tcp_ports, */
        /* DEFENDER_listening_udp_ports, */
        pxDEFENDER_tcp_connections,
    };

    ( void ) DEFENDER_MetricsInit( metrics_list );

    int report_period = 5;
    ( void ) DEFENDER_ReportPeriodSet( report_period );

    DEFENDER_Start();
    test_delay( report_period + 1 );
    DEFENDER_Stop();
}

/* List of unwritten tests: */
