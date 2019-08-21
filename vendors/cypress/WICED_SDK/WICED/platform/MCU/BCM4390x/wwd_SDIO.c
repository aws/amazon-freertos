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
 * Defines WWD SDIO functions for BCM4390x SDIOH
 */
#include <typedefs.h>
#include <osl.h>

#include "platform/wwd_platform_interface.h"
#include "platform/wwd_bus_interface.h"
#include "platform_sdio.h"

/******************************************************
 *             Constants
 ******************************************************/
#ifndef WICED_DISABLE_MCU_POWERSAVE
#error "Not support WICED_DISABLE_MCU_POWERSAVE"
#endif /* ifndef  WICED_DISABLE_MCU_POWERSAVE */

/******************************************************
 *             Structures
 ******************************************************/

/******************************************************
 *             Variables
 ******************************************************/
extern sdioh_info_t *glob_sd;

/******************************************************
 *             Function declarations
 ******************************************************/
void sdio_client_check_isr( void* );

/******************************************************
 *             Function definitions
 ******************************************************/
void host_platform_power_wifi( wiced_bool_t power_enabled )
{
    UNUSED_PARAMETER( power_enabled );
}

wwd_result_t host_platform_bus_init( void )
{
    if ( platform_sdio_host_init( sdio_client_check_isr ) != WICED_SUCCESS )
    {
        WPRINT_WWD_ERROR(("SDIO Host init FAIL\n"));
        return WWD_SDIO_BUS_UP_FAIL;
    }
    else
    {
        return WWD_SUCCESS;
    }
}

wwd_result_t host_platform_sdio_enumerate( void )
{
    /* Select card already done in sdioh_attach */
    return WWD_SUCCESS;
}

wwd_result_t host_platform_bus_deinit( void )
{
    return WWD_SUCCESS;
}

wwd_result_t host_platform_sdio_transfer( wwd_bus_transfer_direction_t direction, sdio_command_t command, sdio_transfer_mode_t mode, sdio_block_size_t block_size, uint32_t argument, /*@null@*/ uint32_t* data, uint16_t data_size, sdio_response_needed_t response_expected, /*@out@*/ /*@null@*/ uint32_t* response )
{
    wiced_result_t wiced_result;

    wiced_result = platform_sdio_host_transfer( direction == BUS_READ ? SDIO_READ : SDIO_WRITE,
                                                command, mode, block_size, argument, data, data_size, response_expected, response );

    return (wiced_result == WICED_SUCCESS ? WWD_SUCCESS : WWD_WLAN_SDIO_ERROR);
}

wwd_result_t host_platform_enable_high_speed_sdio( void )
{
    platform_sdio_host_enable_high_speed();
    return WWD_SUCCESS;
}

wwd_result_t host_platform_bus_enable_interrupt( void )
{
    platform_sdio_host_enable_interrupt();
    return  WWD_SUCCESS;
}

wwd_result_t host_platform_bus_disable_interrupt( void )
{
    platform_sdio_host_disable_interrupt();
    return  WWD_SUCCESS;
}

#ifdef WICED_PLATFORM_MASKS_BUS_IRQ
wwd_result_t host_platform_unmask_sdio_interrupt( void )
{
    return host_platform_bus_enable_interrupt();
}
#endif

void host_platform_bus_buffer_freed( wwd_buffer_dir_t direction )
{
    UNUSED_PARAMETER( direction );
}

/* Client Interrupt handler */
void sdio_client_check_isr( void* user_data )
{
    UNUSED_PARAMETER( user_data );
    host_platform_bus_disable_interrupt();
    wwd_thread_notify_irq();
}

