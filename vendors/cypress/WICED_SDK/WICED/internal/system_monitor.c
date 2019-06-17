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
#include "wiced_platform.h"
#include "wiced_rtos.h"
#include "wwd_assert.h"
#include "RTOS/wwd_rtos_interface.h"
#include "wiced_framework.h"
#include "wiced_internal_api.h"
#include <string.h>

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#ifndef MAX_WAKE_FROM_SLEEP_DELAY_TICKS
#define MAX_WAKE_FROM_SLEEP_DELAY_TICKS (100)
#endif

#ifdef APPLICATION_WATCHDOG_TIMEOUT_SECONDS
#define APPLICATION_WATCHDOG_TIMEOUT_MILLISECONDS  (APPLICATION_WATCHDOG_TIMEOUT_SECONDS * 1000)
#ifdef DEFAULT_SYSTEM_MONITOR_PERIOD
#if DEFAULT_SYSTEM_MONITOR_PERIOD > ( APPLICATION_WATCHDOG_TIMEOUT_MILLISECONDS * 8/10 - MAX_WAKE_FROM_SLEEP_DELAY_TICKS)
#undef DEFAULT_SYSTEM_MONITOR_PERIOD
/* Set the system monitor thread delay time to 20% less than the watchdog timer to fit both actual and debug/shadow watchdog timeout */
#define DEFAULT_SYSTEM_MONITOR_PERIOD ( APPLICATION_WATCHDOG_TIMEOUT_MILLISECONDS * 8/10 - MAX_WAKE_FROM_SLEEP_DELAY_TICKS)
#endif /* DEFAULT_SYSTEM_MONITOR_PERIOD > ( APPLICATION_WATCHDOG_TIMEOUT_MILLISECONDS * 8/10 - MAX_WAKE_FROM_SLEEP_DELAY_TICKS) */
#else  /* DEFAULT_SYSTEM_MONITOR_PERIOD */
/* Set the system monitor thread delay time to 20% less than the watchdog timer to fit both actual and debug/shadow watchdog timeout */
#define DEFAULT_SYSTEM_MONITOR_PERIOD ( APPLICATION_WATCHDOG_TIMEOUT_MILLISECONDS * 8/10 - MAX_WAKE_FROM_SLEEP_DELAY_TICKS)
#endif /* DEFAULT_SYSTEM_MONITOR_PERIOD */
#else
#ifndef DEFAULT_SYSTEM_MONITOR_PERIOD
#define DEFAULT_SYSTEM_MONITOR_PERIOD   (5000)
#endif
#endif

#ifndef MAXIMUM_NUMBER_OF_SYSTEM_MONITORS
#define MAXIMUM_NUMBER_OF_SYSTEM_MONITORS    (5)
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
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

static wiced_system_monitor_t* system_monitors[MAXIMUM_NUMBER_OF_SYSTEM_MONITORS];
static wiced_semaphore_t       system_monitor_thread_semaphore;

/******************************************************
 *               Function Definitions
 ******************************************************/

void system_monitor_thread_main( wiced_thread_arg_t arg )
{
    UNUSED_PARAMETER(arg);

    memset(system_monitors, 0, sizeof(system_monitors));

    if (wiced_rtos_init_semaphore(&system_monitor_thread_semaphore) != WICED_SUCCESS)
    {
        wiced_assert("semaphore init failed", 0);
    }

    /* - Can watch threads
     * Each thread can set a counter that is decremented every event.
     * Once any timer == 0, the watchdog is no longer kicked
     *
     * - Can watch packet buffer status
     * If no RX packets are available, take timestamp (T). If (current time - T) > X seconds, stop kicking watchdog.
     * X can be 10 second default. Time will be set to zero once RX buffer is freed
     *
     * - Can watch bus data credits
     * If no credits are available, take timestamp (B). If (current time - B) > X seconds, stop kicking watchdog.
     * This will be dependent on if WLAN is up. Timer will be set to 0 if credits become available.
     */
    while (1)
    {
        int a;
        uint32_t current_time = host_rtos_get_time();

        for (a = 0; a < MAXIMUM_NUMBER_OF_SYSTEM_MONITORS; ++a)
        {
            if (system_monitors[a] != NULL)
            {
                if ((current_time - system_monitors[a]->last_update) > system_monitors[a]->longest_permitted_delay)
                {
                    /* A system monitor update period has been missed and hence explicitly resetting the MCU rather than waiting for the watchdog to bite */
                    wiced_framework_reboot();
                }
            }
        }
        wiced_watchdog_kick();
        if ( wiced_rtos_get_semaphore(&system_monitor_thread_semaphore, DEFAULT_SYSTEM_MONITOR_PERIOD) == WICED_WWD_SEMAPHORE_ERROR )
        {
            continue;
        }
    }

    /* Should never get here */
    wiced_rtos_deinit_semaphore(&system_monitor_thread_semaphore);

    WICED_END_OF_CURRENT_THREAD( );
}

wiced_result_t wiced_register_system_monitor(wiced_system_monitor_t* system_monitor, uint32_t initial_permitted_delay)
{
    int a;

    /* Find spare entry and add the new system monitor */
    for ( a = 0; a < MAXIMUM_NUMBER_OF_SYSTEM_MONITORS; ++a )
    {
        if (system_monitors[a] == NULL)
        {
            system_monitor->last_update = host_rtos_get_time();
            system_monitor->longest_permitted_delay = initial_permitted_delay;
            system_monitors[a] = system_monitor;
            return WICED_SUCCESS;
        }
    }

    return WICED_ERROR;
}

wiced_result_t wiced_update_system_monitor(wiced_system_monitor_t* system_monitor, uint32_t permitted_delay)
{
    uint32_t current_time = host_rtos_get_time();
    /* Update the system monitor if it hasn't already passed it's permitted delay */
    if ((current_time - system_monitor->last_update) <= system_monitor->longest_permitted_delay)
    {
        system_monitor->last_update             = current_time;
        system_monitor->longest_permitted_delay = permitted_delay;
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_wakeup_system_monitor_thread(void)
{
    return wiced_rtos_set_semaphore(&system_monitor_thread_semaphore);
}
