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
#include "aws_defender_uptime.h"
#include "unity_fixture.h"

#ifdef __linux__
    #include <unistd.h>

    static void delay()
    {
        int sleep_time = 1;

        while( sleep_time )
        {
            sleep_time = sleep( sleep_time );
        }
    }
#elif defined __free_rtos__
    #include "FreeRTOS.h"
    #include "task.h"
    static void delay()
    {
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
#endif /* ifdef __linux__ */

TEST_GROUP( aws_defender_uptime );

TEST_SETUP( aws_defender_uptime )
{
}

TEST_TEAR_DOWN( aws_defender_uptime )
{
}

TEST_GROUP_RUNNER( aws_defender_uptime )
{
    RUN_TEST_CASE( aws_defender_uptime, UptimeGet_result_remains_unchanged );
    RUN_TEST_CASE( aws_defender_uptime, UptimeGet_changes_upon_refresh );
}

TEST( aws_defender_uptime, UptimeGet_result_remains_unchanged )
{
    int expected = DEFENDER_UptimeSecondsGet();

    delay();
    int result = DEFENDER_UptimeSecondsGet();
    TEST_ASSERT_EQUAL( expected, result );
}

TEST( aws_defender_uptime, UptimeGet_changes_upon_refresh )
{
    DEFENDER_UptimeRefresh();
    int uptime_before = DEFENDER_UptimeSecondsGet();
    int uptime_after;
    delay();
    DEFENDER_UptimeRefresh();
    uptime_after = DEFENDER_UptimeSecondsGet();
    TEST_ASSERT_GREATER_THAN( uptime_before, uptime_after );
    TEST_ASSERT_LESS_THAN( uptime_before + 3, uptime_after );
}
