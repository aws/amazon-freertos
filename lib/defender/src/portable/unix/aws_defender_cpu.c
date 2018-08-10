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
#include <time.h>

static int DEFENDER_cpu_load_percent;

int DEFENDER_CpuLoadGet( void )
{
    return DEFENDER_cpu_load_percent;
}

void DEFENDER_CpuLoadRefresh( void )
{
    static int prev_clock_time = 0;
    static time_t prev_time = 0;
    int clock_time = clock();
    time_t current_time = 0;

    time( &current_time );
    int delta_t_sec = difftime( current_time, prev_time );
    int delta_clock_ticks = clock_time - prev_clock_time;
    int delta_t_ticks = delta_t_sec * CLOCKS_PER_SEC;

    if( 0 < delta_t_ticks )
    {
        int temp = delta_clock_ticks * 100;
        DEFENDER_cpu_load_percent = temp / delta_t_ticks;
    }

    prev_clock_time = clock_time;
    prev_time = current_time;
}
