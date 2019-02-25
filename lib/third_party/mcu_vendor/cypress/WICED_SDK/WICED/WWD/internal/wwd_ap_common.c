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
 *  Provides generic APSTA functionality that chip specific files use
 */
#include <string.h>
#include "wwd_rtos.h"
#include "wwd_events.h"
#include "wwd_assert.h"
#include "wwd_management.h"
#include "wwd_wifi.h"
#include "network/wwd_buffer_interface.h"
#include "internal/wwd_sdpcm.h"
#include "internal/wwd_internal.h"
#include "internal/wwd_ap.h"
#include "platform_toolchain.h"
#include "internal/wwd_ap_common.h"
#include "wiced_result.h"
#include "wifi_utils.h"

static wiced_bool_t ap_is_up = WICED_FALSE;

void wwd_wifi_set_ap_is_up( wiced_bool_t new_state )
{
    if ( ap_is_up != new_state )
    {
        ap_is_up = new_state;
        wwd_wifi_link_update( );
    }
}

wiced_bool_t wwd_wifi_get_ap_is_up( void )
{
    return ap_is_up;
}

wwd_result_t wwd_wifi_set_block_ack_window_size_common( wwd_interface_t interface, uint16_t ap_win_size, uint16_t sta_win_size )
{
    wwd_result_t retval;
    uint16_t block_ack_window_size = ap_win_size;

    /* If the AP interface is already up then don't change the Block Ack window size */
    if ( wwd_wifi_is_ready_to_transceive( WWD_AP_INTERFACE ) == WWD_SUCCESS )
    {
        return WWD_SUCCESS;
    }

    if ( interface == WWD_STA_INTERFACE )
    {
        block_ack_window_size = sta_win_size;
    }

    retval = wwd_wifi_set_iovar_value( IOVAR_STR_AMPDU_BA_WINDOW_SIZE, ( uint32_t ) block_ack_window_size, WWD_STA_INTERFACE );

    wiced_assert("set_block_ack_window_size: Failed to set block ack window size\r\n", retval == WWD_SUCCESS );

    return retval;
}

wwd_result_t wwd_wifi_set_ampdu_parameters_common( wwd_interface_t interface, uint8_t ba_window_size, int8_t ampdu_mpdu, uint8_t rx_factor )
{
    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_AMPDU_BA_WINDOW_SIZE, ba_window_size, interface ) );

    /* Set number of MPDUs available for AMPDU */
    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_AMPDU_MPDU, ( uint32_t ) ampdu_mpdu, interface ) );

    if ( rx_factor != AMPDU_RX_FACTOR_INVALID )
    {
        CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_AMPDU_RX_FACTOR, rx_factor, interface ) );
    }
    return WWD_SUCCESS;
}


wwd_result_t wwd_wifi_set_chanspec ( wwd_interface_t interface, uint8_t channel, host_semaphore_type_t *wwd_wifi_sleep_flag )
{
    uint16_t       chanspec;
    uint32_t       bandwidth = BANDWIDTH_20MHZ;
    uint32_t*      data;
    wiced_buffer_t buffer;

    bandwidth = wwd_wifi_get_bandwidth();
    if ( bandwidth == BANDWIDTH_40MHZ )
    {
         chanspec = (wl_chanspec_t) htod16((channel | WL_CHANSPEC_BW_40 | WL_CHANSPEC_CTL_SB_NONE));
    }
#ifdef DOT11AC_CHIP
    else if ( bandwidth == BANDWIDTH_80MHZ )
    {
         chanspec = (wl_chanspec_t) htod16((channel | WL_CHANSPEC_BW_80 | WL_CHANSPEC_CTL_SB_NONE));
    }
#endif
    else
    {
         chanspec = (wl_chanspec_t) htod16((channel | WL_CHANSPEC_BW_20 | WL_CHANSPEC_CTL_SB_NONE));
    }

    chanspec |= wwd_channel_to_wl_band( channel );

    /* set the chanspec */
    data = wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 4, "chanspec" );
    CHECK_IOCTL_BUFFER( data );
    *data = chanspec;
    WPRINT_WWD_DEBUG (( "\n calling chanspec IOVAR with chanspec=%x channel=%d\n", chanspec, channel ));
    CHECK_RETURN_WITH_SEMAPHORE ( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, interface ), wwd_wifi_sleep_flag );

    return WWD_SUCCESS;
}
