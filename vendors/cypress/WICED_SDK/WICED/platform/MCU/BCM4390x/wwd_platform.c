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
#include <stdint.h>
#include "wwd_constants.h"
#include "wwd_assert.h"
#include "platform_config.h"
#include "wiced_platform.h"
#include "platform/wwd_platform_interface.h"
#include "platform_appscr4.h"
#include "cr4.h"


/******************************************************
 *                      Macros
 ******************************************************/
#ifdef __GNUC__
#define TRIGGER_BREAKPOINT() __asm__("bkpt")
#elif defined ( __IAR_SYSTEMS_ICC__ )
#define TRIGGER_BREAKPOINT() __asm("bkpt 0")
#endif

/******************************************************
 *                    Constants
 ******************************************************/

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

/******************************************************
 *               Function Definitions
 ******************************************************/

wwd_result_t host_platform_init( void )
{
    wwd_result_t result = WWD_SUCCESS;

#ifdef USES_RESOURCE_FILESYSTEM
    if ( platform_filesystem_init() != PLATFORM_SUCCESS )
    {
        WPRINT_WWD_ERROR(("Host Platform init FAIL\n"));
        result = WWD_RTOS_ERROR;
    }
#endif/* USES_RESOURCE_FILESYSTEM */

    return result;
}

wwd_result_t host_platform_deinit( void )
{
    return WWD_SUCCESS;
}

uint32_t host_platform_get_cycle_count( void )
{
    return cr4_get_cycle_counter( );
}

wwd_result_t host_platform_init_wlan_powersave_clock( void )
{
    return WWD_SUCCESS;
}

wwd_result_t host_platform_deinit_wlan_powersave_clock( void )
{
    return WWD_SUCCESS;
}

wiced_bool_t host_platform_is_in_interrupt_context( void )
{
    uint32_t psr = get_CPSR();
    enum op_mode {USR = 0x10, FIQ = 0x11, IRQ = 0x12, SVC = 0x13, ABT = 0x17, UND = 0x1b, SYS = 0x1f};
    enum op_mode m_bits = psr & 0x1f;

    switch (m_bits)
    {
        case FIQ:
        case IRQ:
        case SVC:
            return WICED_TRUE;

        case USR:
        case ABT:
        case UND:
        case SYS:
        default:
            return WICED_FALSE;
    }
}

void host_platform_reset_wifi( wiced_bool_t reset_asserted )
{
    UNUSED_PARAMETER( reset_asserted );
}
