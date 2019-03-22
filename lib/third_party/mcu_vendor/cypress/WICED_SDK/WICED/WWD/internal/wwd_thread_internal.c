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

#include "wwd_rtos.h"
#include "wwd_assert.h"
#include "internal/wwd_sdpcm.h"
#include "internal/wwd_internal.h"
#include "internal/bus_protocols/wwd_bus_protocol_interface.h"
#include "RTOS/wwd_rtos_interface.h"
#include "platform_toolchain.h"

/******************************************************
 *             Macros
 ******************************************************/

#ifndef WWD_THREAD_POLL_TIMEOUT
#define WWD_THREAD_POLL_TIMEOUT      (NEVER_TIMEOUT)
#endif

#ifndef WWD_THREAD_POKE_TIMEOUT
#define WWD_THREAD_POKE_TIMEOUT      (100)
#endif

/******************************************************
 *             Global Functions
 ******************************************************/

WEAK void wwd_wait_for_wlan_event( host_semaphore_type_t* transceive_semaphore )
{
    wwd_result_t result = WWD_SUCCESS;

    REFERENCE_DEBUG_ONLY_VARIABLE( result );

    /* Check if we have run out of bus credits */
    if ( wwd_sdpcm_get_available_credits( ) == 0 )
    {
        /* Keep poking the WLAN until it gives us more credits */
        result = wwd_bus_poke_wlan( );
        wiced_assert( "Poking failed!", result == WWD_SUCCESS );

        result = host_rtos_get_semaphore( transceive_semaphore, (uint32_t) WWD_THREAD_POKE_TIMEOUT, WICED_FALSE );
    }
    else
    {
        /* Put the bus to sleep and wait for something else to do */
        if ( WWD_WLAN_MAY_SLEEP( ) )
        {
            result = wwd_allow_wlan_bus_to_sleep( );
            wiced_assert( "Error setting wlan sleep", result == WWD_SUCCESS );
        }
        result = host_rtos_get_semaphore( transceive_semaphore, (uint32_t) WWD_THREAD_POLL_TIMEOUT, WICED_FALSE );
    }
    wiced_assert("Could not get wwd sleep semaphore\n", (result == WWD_SUCCESS)||(result == WWD_TIMEOUT) );
}
