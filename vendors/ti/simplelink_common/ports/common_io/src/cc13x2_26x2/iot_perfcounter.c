/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    iot_perfcounter.c
 * @brief   This file contains the definitions of perfcounter APIs using TI DriverLib.
 */
#include <stdint.h>
#include <stdbool.h>

/* Common IO Header */
#include <iot_perfcounter.h>

/* TI DPL */
#include <ti/drivers/dpl/ClockP.h>

/* TI DriverLib */
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_cpu_dwt.h)

void iot_perfcounter_open( void )
{
    /* Enable CYCCNT */
    HWREG(CPU_DWT_BASE + CPU_DWT_O_CTRL) |= 1;
}

/*-----------------------------------------------------------*/

uint64_t iot_perfcounter_get_value( void )
{
    return HWREG(CPU_DWT_BASE + CPU_DWT_O_CYCCNT);
}

/*-----------------------------------------------------------*/

uint32_t iot_perfcounter_get_frequency( void )
{
    ClockP_FreqHz freq;
    ClockP_getCpuFreq(&freq);
    return freq.lo;
}

/*-----------------------------------------------------------*/

void iot_perfcounter_close( void )
{
    return;
}
