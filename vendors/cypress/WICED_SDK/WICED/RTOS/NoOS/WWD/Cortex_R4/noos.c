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
 *  NoOS functions specific to the Cortex-R4 Platform
 *
 */

#include <stdint.h>
#include "wwd_rtos.h"
#include "wwd_rtos_isr.h"
#include "platform_assert.h"

volatile uint32_t noos_total_time = 0;

#define CYCLES_PER_SYSTICK  ( ( CPU_CLOCK_HZ / SYSTICK_FREQUENCY ) - 1 )

void NoOS_setup_timing( void )
{
    platform_tick_start( );

    WICED_ENABLE_INTERRUPTS( );
}

void NoOS_stop_timing( void )
{
    WICED_DISABLE_INTERRUPTS();

    platform_tick_stop( );
}

/* NoOS SysTick handler */
WWD_RTOS_DEFINE_ISR( platform_tick_isr )
{
    if ( platform_tick_irq_handler( ) )
    {
        noos_total_time++;
    }
}

WWD_RTOS_MAP_ISR( platform_tick_isr, Timer_ISR )
