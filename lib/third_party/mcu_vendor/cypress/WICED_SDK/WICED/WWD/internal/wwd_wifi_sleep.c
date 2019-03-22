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

#include <string.h> /* For strlen, stricmp, memcpy. memset */
#include <stddef.h>
#include "wwd_management.h"
#include "wwd_wifi.h"
#include "wwd_assert.h"
#include "wwd_wlioctl.h"
#include "wwd_debug.h"
#include "platform/wwd_platform_interface.h"
#include "network/wwd_buffer_interface.h"
#include "network/wwd_network_constants.h"
#include "RTOS/wwd_rtos_interface.h"
#include "internal/wwd_sdpcm.h"
#include "internal/wwd_bcmendian.h"
#include "internal/wwd_ap.h"
#include "internal/wwd_internal.h"
#include "wwd_bus_protocol.h"
#include "wiced_utilities.h"
#include "wiced_low_power.h"
#include "wwd_events.h"
#include "wwd_wifi_sleep.h"
#include "internal/bus_protocols/wwd_bus_protocol_interface.h"
#include "wwd_wifi_chip_common.h"

/******************************************************
 *             Macros
 ******************************************************/
//#define WWD_WIFI_SLEEP_DEBUG_OUTPUT/* uncomment for all debug output */
#ifdef WWD_WIFI_SLEEP_DEBUG_OUTPUT
#define WWD_WIFI_SLEEP_DEBUG( args ) WPRINT_APP_INFO( args )
#define WWD_WIFI_SLEEP_INFO( args )  WPRINT_APP_INFO( args )
#define WWD_WIFI_SLEEP_ERROR( args ) WPRINT_APP_INFO( args )
#else
#define WWD_WIFI_SLEEP_DEBUG( args ) WPRINT_WWD_DEBUG( args )
#define WWD_WIFI_SLEEP_INFO( args )  WPRINT_WWD_INFO( args )
#define WWD_WIFI_SLEEP_ERROR( args ) WPRINT_WWD_ERROR( args )
#endif

#define CHECK_RETURN( expr )  { wwd_result_t check_res = (expr); if ( check_res != WWD_SUCCESS ) { wiced_assert("Command failed\n", 0 == 1); return check_res; } }

/******************************************************
 *             Local Structures
 ******************************************************/
/* ULP SHM Offsets info */
typedef struct ulp_shm_info {
    uint32_t m_ulp_ctrl_sdio;
    uint32_t m_ulp_wakeevt_ind;
    uint32_t m_ulp_wakeind;
} ulp_shm_info_t;

ulp_shm_info_t ulp_offsets;

/******************************************************
 *             Static Variables
 ******************************************************/

static const wwd_event_num_t ulp_events[] = { WLC_E_ULP, WLC_E_NONE };

static volatile wwd_ds_state_t WICED_DEEP_SLEEP_SAVED_VAR( wwd_wifi_ds1_state ) = STATE_DS_DISABLED;

static wiced_bool_t last_wake_from_fw = WICED_FALSE;
static wwd_result_t last_wake_result = WWD_SUCCESS;

static volatile wwd_ds1_state_change_callback_t WICED_DEEP_SLEEP_SAVED_VAR( ds_notify_callback ) = NULL;
static volatile void* WICED_DEEP_SLEEP_SAVED_VAR( ds_notify_callback_user_parameter ) = NULL;

/******************************************************
 *            Function prototypes
 ******************************************************/
static void ds_state_set( wwd_ds_state_t new_state );
static wwd_result_t wwd_wifi_ds1_finish_wake( wiced_bool_t wake_from_firmware );
void* wwd_wifi_event_ulp_handler( const wwd_event_header_t* event_header, const uint8_t* event_data, /*@returned@*/ void* handler_user_data );

/* enable and set up wowl patterns */
wwd_result_t wwd_wifi_wowl_enable( wwd_interface_t interface, uint32_t wowl_caps, uint32_t wowl_os,
     wl_mkeep_alive_pkt_t *wowl_keepalive_data, uint8_t *pattern_data, uint32_t pattern_data_size, uint32_t *arp_host_ip_v4_address )
{
    wwd_result_t wwd_result = wwd_wifi_set_iovar_value( IOVAR_STR_WOWL, wowl_caps, interface );
    WWD_WIFI_SLEEP_DEBUG(("wowl\n"));

    if ( WWD_SUCCESS != wwd_result )
    {
        WWD_WIFI_SLEEP_ERROR(("Error on wowl set %d\n", wwd_result));
        return wwd_result;
    }

    /* Only bother setting wowl_os to a non-zero value */
    if ( 0 != wowl_os )
    {
        WWD_WIFI_SLEEP_DEBUG(("wowl-os\n"));
        CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_WOWL_OS, wowl_os, interface ) );
    }

    /* Set up any type of wowl requested */
    if ( NULL != wowl_keepalive_data )
    {
        CHECK_RETURN( wwd_wifi_set_iovar_buffer( IOVAR_STR_WOWL_KEEP_ALIVE, wowl_keepalive_data, (uint16_t) ( wowl_keepalive_data->length + wowl_keepalive_data->len_bytes ), interface ) );
    }

    if ( NULL != pattern_data )
    {
        const void *buffer_pointers[2] = { IOVAR_STR_WOWL_PATTERN_ADD, (const void*)pattern_data };
        uint16_t buffer_sizes[2] = { sizeof(IOVAR_STR_WOWL_PATTERN_ADD), (uint16_t)pattern_data_size };

        WWD_WIFI_SLEEP_DEBUG(("pre-pattern add\n"));
        wwd_result = wwd_wifi_set_iovar_buffers( IOVAR_STR_WOWL_PATTERN, buffer_pointers, buffer_sizes, 2, interface );

        if ( WWD_SUCCESS != wwd_result )
        {
            WWD_WIFI_SLEEP_DEBUG(("add pattern result=%d\n", (int)wwd_result));
        }

        WWD_WIFI_SLEEP_DEBUG(("post-pattern add\n"));
    }

    if ( NULL != arp_host_ip_v4_address )
    {
        CHECK_RETURN( wwd_wifi_set_iovar_buffer( IOVAR_STR_WOWL_ARP_HOST_IP, arp_host_ip_v4_address, sizeof( *arp_host_ip_v4_address ), interface ) );
    }

    WWD_WIFI_SLEEP_DEBUG(("End wowl enable\n"));
    return wwd_result;
}

/* called when state is changed */
static void ds_state_set( wwd_ds_state_t new_state )
{
    wwd_wifi_ds1_state = new_state;

    if ( NULL != ds_notify_callback )
    {
        ds_notify_callback( (void*)ds_notify_callback_user_parameter );
    }
}

void wwd_wifi_ds1_get_status_string( uint8_t *output, uint16_t max_output_length )
{
    const char *state = NULL;
    switch ( wwd_wifi_ds1_get_state( ) )
    {
        case STATE_DS_DISABLED:
            state = "STATE_DS_DISABLED";
            break;
        case STATE_DS_ENABLING:
            state = "STATE_DS_ENABLING";
            break;
        case STATE_DS_ENABLED:
            state = "STATE_DS_ENABLED";
            break;
        case STATE_DS_DISABLING:
            state = "STATE_DS_DISABLING";
            break;
        default:
            wiced_assert("Unknown DS1 state", 0 != 0 );
            state = "unknown";
    }

    snprintf( (char*)output, max_output_length, "%s. Last %s wake result %d ", state,
        ( WICED_TRUE == last_wake_from_fw ) ? "FIRMWARE" : "HOST", last_wake_result );
}

/* Register a callback for deep sleep state changes; current state can then be queried */
wwd_result_t wwd_wifi_ds1_set_state_change_callback( wwd_ds1_state_change_callback_t callback, void *user_parameter )
{
    ds_notify_callback                = callback;
    ds_notify_callback_user_parameter = user_parameter;

    return WWD_SUCCESS;
}

/* Get current DS1 state */
extern wwd_ds_state_t wwd_wifi_ds1_get_state( void )
{
    return wwd_wifi_ds1_state;
}

void* wwd_wifi_event_ulp_handler( const wwd_event_header_t* event_header, const uint8_t* event_data, /*@returned@*/ void* handler_user_data )
{
    wl_ulp_event_t*  ulp_evt = (wl_ulp_event_t *)event_data;

    UNUSED_PARAMETER(event_header);
    UNUSED_PARAMETER(event_data);
    UNUSED_PARAMETER(handler_user_data);

    WWD_WIFI_SLEEP_DEBUG(("%s : ULP event handler triggered [%d]\n", __FUNCTION__, ulp_evt->ulp_dongle_action));

    switch ( ulp_evt->ulp_dongle_action )
    {
        case WL_ULP_ENTRY:
          /* Just mark DS1 enabled flag to TRUE so that DS1 exit interrupt will be processed by WWD */
          ds_state_set( STATE_DS_ENABLED );
        break;
        default:
        break;
    }
    return NULL;
}

wwd_result_t wwd_wifi_ds1_disable( wwd_interface_t interface )
{
    wwd_result_t wwd_result = wwd_wifi_set_iovar_value( IOVAR_STR_ULP, 0, interface );

    if ( WWD_SUCCESS != wwd_result )
    {
        WWD_WIFI_SLEEP_ERROR(("Error %d\n", wwd_result));
        return wwd_result;
    }

    WWD_WIFI_SLEEP_DEBUG(("disable wowl\n"));
    CHECK_RETURN( wwd_wifi_wowl_disable( interface ) );
    return WWD_SUCCESS;
}

/* have device go into ds1 after given wait period */
wwd_result_t wwd_wifi_enter_ds1( wwd_interface_t interface, uint32_t ulp_wait_milliseconds )
{
    uint32_t ulp_enable         = 1;
    wwd_result_t result;

    result = wwd_wifi_get_iovar_buffer( "ulp_sdioctrl", (uint8_t*)&ulp_offsets, sizeof( ulp_offsets ), WWD_STA_INTERFACE );
    if ( WWD_SUCCESS != result )
    {
        WWD_WIFI_SLEEP_ERROR(("unable to load sdio ctl offsets\n"));
        goto error;
    }

    WWD_WIFI_SLEEP_DEBUG(("Setting ulp evt hdlr\n"));

    /* listen for ULP ready: avoid race condition by doing this first */
    CHECK_RETURN( wwd_management_set_event_handler( ulp_events, wwd_wifi_event_ulp_handler, NULL, WWD_STA_INTERFACE ) );

    WWD_WIFI_SLEEP_DEBUG(("ulp wait\n"));

    /* then trigger ULP */
    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_ULP_WAIT, ulp_wait_milliseconds, interface ) );
    WWD_WIFI_SLEEP_DEBUG(("ulp\n"));

    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_ULP, ulp_enable, interface ) );

    WWD_WIFI_SLEEP_DEBUG(("ulp set to enable\n"));

    /* wait for number of ulp wait milliseconds prior to setting enabled, to allow FW to settle */
    ds_state_set( STATE_DS_ENABLING );

    return WWD_SUCCESS;
error:
    return result;
}

/* turn off wowl and clear any previously added patterns */
wwd_result_t wwd_wifi_wowl_disable( wwd_interface_t interface )
{
    uint32_t val_disable = 0;
    WWD_WIFI_SLEEP_DEBUG(("clear wowl\n"));
    CHECK_RETURN( wwd_wifi_set_iovar_buffer( IOVAR_STR_WOWL, &val_disable, sizeof( val_disable ), interface) );
    WWD_WIFI_SLEEP_DEBUG(("clear wowl os\n"));
    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_WOWL_OS, val_disable, interface ) );
    WWD_WIFI_SLEEP_DEBUG(("clear pattern\n"));
    CHECK_RETURN( wwd_wifi_set_iovar_buffer( IOVAR_STR_WOWL_PATTERN, (uint8_t*)IOVAR_STR_WOWL_PATTERN_CLR, sizeof( IOVAR_STR_WOWL_PATTERN_CLR ), interface ) );
    return WWD_SUCCESS;
}

/* process any wake requests */
wwd_result_t wwd_wifi_ds1_wake_handle( wiced_bool_t force )
{
    wiced_bool_t wake_from_firmware = WICED_FALSE;

    /* Is WWD in ds1? */
    if ( STATE_DS_ENABLED == wwd_wifi_ds1_state )
    {
        /* Wake need from host tx or forced (eg cmd line)? */
        if ( WICED_TRUE == wwd_sdpcm_has_tx_packet( ) || WICED_TRUE == force )
        {
            WWD_WIFI_SLEEP_DEBUG(("wake due %s\n", ( WICED_TRUE == force ) ? "force" : "tx"));
            wake_from_firmware = WICED_FALSE;
        }
        else if ( WICED_TRUE == wwd_bus_wake_interrupt_present( ) )
        {
            WWD_WIFI_SLEEP_DEBUG(("firmware wake need detected\n"));
            wake_from_firmware = WICED_TRUE;
        }
        /* else no wake reason so no-op */
        else
        {
            /* bail immediately; no more to do */
            return WWD_SUCCESS;
        }

        /* do the actual waking */
        return wwd_wifi_ds1_finish_wake( wake_from_firmware );
    }
    return WWD_SUCCESS;
}

static wwd_result_t wwd_wifi_ds1_finish_wake( wiced_bool_t wake_from_firmware )
{
    wwd_result_t wwd_result = WWD_SUCCESS;

    if ( STATE_DS_ENABLED != wwd_wifi_ds1_state )
    {
        WWD_WIFI_SLEEP_DEBUG(("DS1 not yet enabled; can't wake\n"));
        return WWD_SUCCESS;
    }

    WWD_WIFI_SLEEP_DEBUG(("wake from firmware=%d\n", wake_from_firmware));

    /* we are going to disable DS and return to normal mode */
    ds_state_set( STATE_DS_DISABLING );

    last_wake_from_fw = wake_from_firmware;

    /* reinit tx and rx counters FIRST, so code is in sync */
    wwd_sdpcm_bus_vars_init( );

    wwd_result = wwd_wlan_bus_complete_ds_wake( wake_from_firmware, ulp_offsets.m_ulp_wakeevt_ind, ulp_offsets.m_ulp_wakeind, ulp_offsets.m_ulp_ctrl_sdio );

    if ( WWD_SUCCESS != wwd_result )
    {
        WWD_WIFI_SLEEP_ERROR(("Error completing bus DS wake\n"));
        /*@-unreachable@*/ /* Reachable after hitting assert */
        goto exit;
        /*@+unreachable@*/
    }

    wwd_result = wwd_bus_reinit( wake_from_firmware );
    if ( wwd_result != WWD_SUCCESS )
    {
        WWD_WIFI_SLEEP_ERROR(("SDIO Reinit failed with error %d\n", wwd_result));
    }
    else
    {
        /* clear only if everything succeeded */
        ds_state_set( STATE_DS_DISABLED );
    }
    WWD_WIFI_SLEEP_DEBUG(("Finish wake done: result %d\n", (int)wwd_result));
exit:
    last_wake_result = wwd_result;

    WWD_WIFI_SLEEP_DEBUG(("Wake result: %s!\n", ( WWD_SUCCESS == wwd_result ) ? "Success" : "Fail" ));

    if ( WWD_SUCCESS != wwd_result )
    {
        ds_state_set( STATE_DS_ENABLED );
    }

    return wwd_result;
}
