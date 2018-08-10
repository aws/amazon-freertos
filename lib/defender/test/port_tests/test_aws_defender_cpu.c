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
#include "aws_defender_cpu.h"
#include "unity_fixture.h"

#ifdef __linux__
    #include <sys/sysinfo.h>
    #include <unistd.h>

    static void idle_delay()
    {
        int sleep_time = 1;

        while( sleep_time )
        {
            sleep_time = sleep( sleep_time );
        }
    }

    static void active_delay()
    {
        struct sysinfo current_sys_info = { 0 };

        ( void ) sysinfo( &current_sys_info );
        int current_time = current_sys_info.uptime;
        int stop_time = current_time + 2;

        while( current_time < stop_time )
        {
            ( void ) sysinfo( &current_sys_info );
            current_time = current_sys_info.uptime;
        }
    }

#elif defined __free_rtos__
    #include "FreeRTOS.h"
    #include "task.h"

    static void idle_delay()
    {
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }

    static void active_delay()
    {
        TickType_t delay_period_ticks = pdMS_TO_TICKS( 1000 );
        TickType_t current_tick = xTaskGetTickCount();

        while( xTaskGetTickCount() - current_tick < delay_period_ticks )
        {
        }
    }

#endif /* ifdef __linux__ */

TEST_GROUP( aws_defender_cpu );

TEST_SETUP( aws_defender_cpu )
{
}

TEST_TEAR_DOWN( aws_defender_cpu )
{
}

TEST_GROUP_RUNNER( aws_defender_cpu )
{
    RUN_TEST_CASE( aws_defender_cpu, CpuLoadGet_idle_period );
    RUN_TEST_CASE( aws_defender_cpu, CpuLoadGet_active_period );
}

TEST( aws_defender_cpu, CpuLoadGet_idle_period )
{
    DEFENDER_CpuLoadRefresh();
    idle_delay();
    DEFENDER_CpuLoadRefresh();
    int idle_load = DEFENDER_CpuLoadGet();
    TEST_ASSERT_LESS_THAN( 25, idle_load );
}

TEST( aws_defender_cpu, CpuLoadGet_active_period )
{
    DEFENDER_CpuLoadRefresh();
    active_delay();
    DEFENDER_CpuLoadRefresh();
    int active_load = DEFENDER_CpuLoadGet();
    TEST_ASSERT_GREATER_THAN_MESSAGE( 50, active_load,
                                      "Re-run. If it fails consistently, then there's likely a problem." );
}
