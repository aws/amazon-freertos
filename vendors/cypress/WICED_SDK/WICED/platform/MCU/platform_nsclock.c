/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 *
 */

#include "platform_config.h"
#include "platform/wwd_platform_interface.h"
#include "platform_peripheral.h"
/* FIXME Needed for CPU_CLOCK_HZ! */
#include "platform_isr.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define absolute_value(a) ( (a) < 0 ) ? -(a) : (a)

/******************************************************
 *                    Constants
 ******************************************************/

const uint32_t NR_NS_PER_SECOND     = 1*1000*1000*1000;

/******************************************************
 *               Variables Definitions
 ******************************************************/

/* Nanosecond accumulator. */
static uint64_t nsclock_nsec = 0;
static uint32_t nsclock_sec = 0;
static uint32_t prev_cycles = 0;
static uint32_t ns_divisor = 0;

/******************************************************
 *               Function Definitions
 ******************************************************/

uint64_t platform_get_nanosecond_clock_value(void)
{
    uint64_t nanos;
    uint32_t cycles;
    uint32_t diff;

    cycles = host_platform_get_cycle_count( );

    /* add values to the ns part of the time which can be divided by ns_divisor */
    /* every time such value is added we will increment our clock by 1 / (CPU_CLOCK_HZ / ns_divisor).
     * For example, for CPU_CLOCK_HZ of 120MHz, ns_divisor of 3, the granularity is 25ns = 1 / (120MHz/3) or 1 / (40MHz).
     * Note that the cycle counter is running on the CPU frequency.
     */
    /* Values will be a multiple of ns_divisor (e.g. 1*ns_divisor, 2*ns_divisor, 3*ns_divisor, ...).
     * Roll-over taken care of by subtraction
     */
    diff = cycles - prev_cycles;
    {
        const uint32_t ns_per_unit = NR_NS_PER_SECOND / (CPU_CLOCK_HZ / ns_divisor);
        nsclock_nsec += ( (uint64_t)( diff / ns_divisor ) * ns_per_unit);
    }

    /* when ns counter rolls over, add one second */
    if( nsclock_nsec >= NR_NS_PER_SECOND )
    {
        /* Accumulate seconds portion of nanoseconds. */
        nsclock_sec += (uint32_t)(nsclock_nsec / NR_NS_PER_SECOND);
        /* Store remaining nanoseconds. */
        nsclock_nsec = nsclock_nsec - (nsclock_nsec / NR_NS_PER_SECOND) * NR_NS_PER_SECOND;
    }
    /* Subtract off unaccounted for cycles, so that they are accounted next time. */
    prev_cycles = cycles - (diff % ns_divisor);

    nanos = nsclock_sec;
    nanos *= NR_NS_PER_SECOND;
    nanos += nsclock_nsec;
    return nanos;
}


void platform_deinit_nanosecond_clock(void)
{
    platform_reset_nanosecond_clock( );

    ns_divisor   = 0;
}

void platform_reset_nanosecond_clock(void)
{
    nsclock_nsec = 0;
    nsclock_sec  = 0;
    prev_cycles  = 0;
}


void platform_init_nanosecond_clock(void)
{
    platform_reset_nanosecond_clock( );
    ns_divisor = 0;
    /* Calculate a divisor that will produce an
     * integer nanosecond value for the CPU
     * clock frequency.
     */
    while (NR_NS_PER_SECOND % (CPU_CLOCK_HZ / ++ns_divisor) != 0);
}
