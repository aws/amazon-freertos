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
#include "platform_toolchain.h"
#include "wwd_management.h"
#include "wwd_wifi.h"
#include "wwd_assert.h"
#include "wwd_wlioctl.h"
#include "wwd_debug.h"
#include "wwd_bus_protocol.h"
#include "wwd_wifi_chip_common.h"
#include "internal/bus_protocols/wwd_bus_protocol_interface.h"


WEAK wwd_result_t wwd_wlan_bus_complete_ds_wake( wiced_bool_t wake_from_firmware, uint32_t wake_event_indication_addr, uint32_t wake_indication_addr, uint32_t sdio_control_addr )
{
    UNUSED_PARAMETER( wake_from_firmware );

    UNUSED_PARAMETER( wake_event_indication_addr );
    UNUSED_PARAMETER( wake_indication_addr );
    UNUSED_PARAMETER( sdio_control_addr );

    return WWD_UNSUPPORTED;
}

wwd_result_t wwd_wlan_register_read( uint32_t addr, uint32_t size, uint32_t *value )
{
    wwd_result_t result;

    *value = 0;
    result = wwd_bus_read_backplane_value( addr, (uint8_t)size, (uint8_t*)value );

    if ( WWD_SUCCESS != result )
    {
        wiced_assert( "Unable to read register\n", WWD_SUCCESS == result );
        WPRINT_WWD_ERROR(("Error %d: Unable to read register\n", result));
        return WWD_TIMEOUT;
    }

    return WWD_SUCCESS;
}

wwd_result_t wwd_wlan_register_write( uint32_t addr, uint32_t size, uint32_t value )
{
    wwd_result_t result = wwd_bus_write_backplane_value( addr, (uint8_t)size, value );

    if ( WWD_SUCCESS != result )
    {
        wiced_assert( "Unable to write register\n", WWD_SUCCESS == result );
        WPRINT_WWD_ERROR(("Error %d: Unable to write register\n", result));
        return WWD_TIMEOUT;
    }

    return WWD_SUCCESS;
}

wwd_result_t wwd_wlan_shared_memory_read( uint32_t addr, uint8_t size, uint32_t *value )
{
    wwd_result_t result;

    *value = 0;
    result = wwd_bus_read_backplane_value( addr, (uint8_t)size, (uint8_t*)value );

    if ( WWD_SUCCESS != result )
    {
        wiced_assert( "Unable to read shm\n", WWD_SUCCESS == result );
        WPRINT_WWD_ERROR(("Error %d: Unable to read shm\n", result));
        return WWD_TIMEOUT;
    }

    return WWD_SUCCESS;
}

