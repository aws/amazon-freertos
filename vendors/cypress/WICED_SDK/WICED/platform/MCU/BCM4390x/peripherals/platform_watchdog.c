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

#include "platform_peripheral.h"
#include "platform_appscr4.h"
#include "platform_assert.h"
#include "platform_peripheral.h"
#include "platform_config.h"

#include "cr4.h"

#include "typedefs.h"
#include "sbchipc.h"

#include "wiced_defaults.h"
#include "wiced_osl.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#ifndef APPLICATION_WATCHDOG_TIMEOUT_SECONDS
#define APPLICATION_WATCHDOG_TIMEOUT_SECONDS    MAX_WATCHDOG_TIMEOUT_SECONDS
#endif

#ifndef APPLICATION_WATCHDOG_HIGH_PRECISION
#define APPLICATION_WATCHDOG_HIGH_PRECISION     0
#endif

#ifndef PLATFORM_MCU_RESET_MAX_REBOOT_SECONDS
#define PLATFORM_MCU_RESET_MAX_REBOOT_SECONDS   60
#endif

#ifndef PLATFORM_MCU_RESET_MAX_WAIT_SECONDS
#define PLATFORM_MCU_RESET_MAX_WAIT_SECONDS     3
#endif

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

static wiced_bool_t platform_watchdog_inited = WICED_FALSE;

/******************************************************
 *               Function Definitions
 ******************************************************/

#ifndef WICED_DISABLE_WATCHDOG
static uint32_t
platform_watchdog_milliseconds_to_ticks( uint32_t milliseconds )
{
#if APPLICATION_WATCHDOG_HIGH_PRECISION
    const uint64_t freq  = platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_ILP );
    const uint64_t ticks = ( (uint64_t)milliseconds * freq + 999 ) / 1000;

    wiced_assert( "exceeds range", ticks <= 0xFFFFFFFF );

    return (uint32_t)ticks;
#else
    const uint32_t freq     = platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_ILP );
    const uint32_t freq_khz = ( freq + 999 ) / 1000;
    const uint32_t ticks    = milliseconds * freq_khz;

    wiced_assert( "exceeds range", ( ticks / freq_khz ) == milliseconds );

    return ticks;
#endif
}
#endif

static void
platform_watchdog_set( uint32_t timeout )
{
    if ( platform_watchdog_inited )
    {
        while ( PLATFORM_PMU->pmustatus & PST_SLOW_WR_PENDING );
        PLATFORM_PMU->pmuwatchdog = timeout;
    }
}

platform_result_t platform_watchdog_init( void )
{
    platform_watchdog_inited = WICED_TRUE;

#ifndef WICED_DISABLE_WATCHDOG
    return platform_watchdog_kick();
#else
    platform_watchdog_deinit();
    return PLATFORM_FEATURE_DISABLED;
#endif
}

platform_result_t platform_watchdog_deinit( void )
{
    platform_watchdog_set( 0 );

    platform_watchdog_inited = WICED_FALSE;

    return PLATFORM_SUCCESS;
}

platform_result_t platform_watchdog_kick_milliseconds( uint32_t milliseconds )
{
#ifndef WICED_DISABLE_WATCHDOG
    platform_watchdog_set( platform_watchdog_milliseconds_to_ticks( milliseconds ) );
    return PLATFORM_SUCCESS;
#else
    return PLATFORM_FEATURE_DISABLED;
#endif
}

platform_result_t platform_watchdog_stop( void )
{
#ifndef WICED_DISABLE_WATCHDOG
    platform_watchdog_set( 0 );
    return PLATFORM_SUCCESS;
#else
    return PLATFORM_FEATURE_DISABLED;
#endif
}

platform_result_t platform_watchdog_kick( void )
{
    return platform_watchdog_kick_milliseconds( APPLICATION_WATCHDOG_TIMEOUT_SECONDS * 1000 );
}

wiced_bool_t platform_watchdog_check_last_reset( void )
{
#ifndef WICED_DISABLE_WATCHDOG
    if ( PLATFORM_PMU->pmustatus & PST_WDRESET )
    {
        /* Clear the flag and return */
        PLATFORM_PMU->pmustatus = PST_WDRESET;
        return WICED_TRUE;
    }
#endif

    return WICED_FALSE;
}

void platform_mcu_specific_reset( platform_reset_type_t type )
{
    WICED_DISABLE_INTERRUPTS();

    /*
     * This reset type does not break JTAG.
     * So set it up here, just in case it is earlier changed.
     */
    platform_common_chipcontrol( &PLATFORM_PMU->pmucontrol, PMU_CONTROL_RESETCONTROL_MASK, PMU_CONTROL_RESETCONTROL_RESTORE_RES_MASKS );

#ifndef WICED_DISABLE_WATCHDOG
    /*
     * Fire PMU watchdog far in a future, use hard-coded ILP clock to reduce application sizes which do not use watchdog.
     * If board would hang during rebooting, PMU watchdog will try to reset board.
     * After application loaded it will kick or deinit watchdog.
     */
    platform_watchdog_set( PLATFORM_MCU_RESET_MAX_REBOOT_SECONDS * ILP_CLOCK );
#endif

    /*
     * Make sure board not enter endless reset cycle.
     * During wait loop we should have JTAG operational.
     * Don't run loop in tiny-bootloader as after resetting boot goes slow cold path, and normal bootloader will run this loop.
     */
#if !defined(TINY_BOOTLOADER) && PLATFORM_MCU_RESET_MAX_WAIT_SECONDS
    if ( !platform_is_init_completed() )
    {
        cr4_init_cycle_counter();
    }
    while( ( cr4_get_cycle_counter() < PLATFORM_MCU_RESET_MAX_WAIT_SECONDS * CPU_CLOCK_HZ ) && !cr4_is_cycle_counter_overflowed() );
#endif

    /* Choose reset type */
    if ( type == PLATFORM_RESET_TYPE_POWERCYCLE )
    {
        platform_common_chipcontrol( &PLATFORM_PMU->pmucontrol, PMU_CONTROL_RESETCONTROL_MASK, PMU_CONTROL_RESETCONTROL_RESET );
    }

    /* Deinitalize power-save */
    if ( type != PLATFORM_RESET_TYPE_POWERCYCLE )
    {
        platform_cores_powersave_deinit( );
    }

    /* Set watchdog to reset system on next tick */
    PLATFORM_CHIPCOMMON->clock_control.watchdog_counter = 1;

    /* Loop forever */
    while (1);
}

void platform_mcu_reset( void )
{
    platform_mcu_specific_reset( PLATFORM_RESET_TYPE_POWERCYCLE );
}
