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

#include <stdlib.h>
#include "wiced_wifi.h"
#include "wwd_debug.h"
#include "wwd_assert.h"
#include "internal/wiced_internal_api.h"
#include "wiced_low_power.h"
#include "wwd_wifi_sleep.h"
#include "wiced_wifi_deep_sleep.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define CHECK_RETURN( expr )  { wiced_result_t check_res = (wiced_result_t)(expr); if ( check_res != WICED_SUCCESS ) { wiced_assert("Command failed\n", 0); return check_res; } }

/* Debug macros */
//#define WICED_WIFI_DEBUG_DEEP_SLEEP /* uncomment for lots of debug output */

#ifdef WICED_WIFI_DEBUG_DEEP_SLEEP
#define WICED_WIFI_DEEP_SLEEP_INFO  WPRINT_APP_INFO
#define WICED_WIFI_DEEP_SLEEP_ERROR WPRINT_APP_INFO
#define WICED_WIFI_DEEP_SLEEP_DEBUG WPRINT_APP_INFO
#else
#define WICED_WIFI_DEEP_SLEEP_INFO  WPRINT_WICED_INFO
#define WICED_WIFI_DEEP_SLEEP_ERROR WPRINT_WICED_ERROR
#define WICED_WIFI_DEEP_SLEEP_DEBUG WPRINT_WICED_DEBUG
#endif /* WICED_WIFI_DEBUG_DEEP_SLEEP */

/******************************************************
 *                    Structures
 ******************************************************/
typedef struct
{
    wiced_wifi_ds1_complete_callback_t callback;
    void *user_parameter;
} wiced_wifi_ds1_callback_data_t;

typedef struct
{
    wiced_offloads_container_t *conditions;
    uint32_t ulp_wait_milliseconds;
} wiced_wifi_ds1_config_value_t;

typedef struct
{
    wiced_wifi_ds1_config_value_t config;                     /* configuration for DS1 */
    wiced_result_t                last_fw_enable_result;      /* the result of the last attempt to enable DS1 in firmware */
    wiced_bool_t                  config_valid;               /* is the configuration valid */
    wiced_bool_t                  host_enabled;               /* tracks if DS1 is enabled in WICED */
    wiced_bool_t                  firmware_enabled;           /* tracks if DS1 state enabled in firmware */
    uint32_t                      initial_powersave_mode;     /* get powersave mode (prior DS1 enable) */
} wiced_wifi_ds1_state_t;

/******************************************************
 *               Static Function Declarations
 ******************************************************/
static void           wiced_wifi_ds1_wwd_callback( void* );
static void           wiced_wifi_wowl_get_values( wiced_offload_t offload_type, uint32_t *wowl, uint32_t *wowl_os, wiced_ds1_debug_t *debug_overrides );
static wiced_result_t wiced_wifi_ds1_fw_enable( wiced_interface_t interface );
static wiced_bool_t   wiced_wifi_ds1_needs_fw_disable( void );
static wiced_result_t wiced_wifi_ds1_enable_wowl_conditions( wiced_interface_t interface, wiced_offloads_container_t *conditions, wiced_ds1_debug_t *debug_overrides );
static wiced_result_t wiced_wifi_deep_sleep_link_up_handler( void *arg );
static void           wiced_wifi_deep_sleep_enqueue_link_up( void );
static void           wiced_wifi_restore_powersave_state( wiced_interface_t interface );


/******************************************************
 *               Static Variable Declarations
 ******************************************************/
static wiced_wifi_ds1_callback_data_t WICED_DEEP_SLEEP_SAVED_VAR( wiced_wifi_ds1_callback_data ) = { NULL, NULL };
static wiced_wifi_ds1_state_t ds1_state;

/******************************************************
 *               Function Definitions
 ******************************************************/

static void wiced_wifi_deep_sleep_enqueue_link_up( void )
{
    /* enqueue a work item to see what needs to be done on a different thread */
    wiced_result_t result = wiced_rtos_send_asynchronous_event( WICED_NETWORKING_WORKER_THREAD, wiced_wifi_deep_sleep_link_up_handler, 0 );

    (void)result;/* used only for debugging */

    if ( WICED_SUCCESS != result )
    {
        WICED_WIFI_DEEP_SLEEP_ERROR(("Err %d: wi-fi deep sleep: unable to send event\n", result));
    }
}

void wiced_wifi_ds1_notify_link_change( void )
{
    wiced_wifi_deep_sleep_enqueue_link_up( );
}

static wiced_result_t wiced_wifi_deep_sleep_link_up_handler( void *arg )
{
    wiced_result_t result = WICED_SUCCESS;
    UNUSED_PARAMETER( arg );
    WICED_WIFI_DEEP_SLEEP_DEBUG(( "Link change notify\n" ));

    /* try to disable if needed */
    if ( wiced_wifi_ds1_needs_fw_disable( ) )
    {
        /* we disable DS1 in firmware */
        WICED_WIFI_DEEP_SLEEP_DEBUG(( "Disable attempt begin\n" ));

        result = wiced_wifi_ds1_disable( WICED_STA_INTERFACE );
    }
    else
    {
        /* see if need to enable */
        WICED_WIFI_DEEP_SLEEP_DEBUG(( "Enable attempt started\n" ));

        result = wiced_wifi_ds1_fw_enable( WICED_STA_INTERFACE );
    }
    WICED_WIFI_DEEP_SLEEP_DEBUG(( "Result=%d\n", result ));

    return result;
}

static wiced_bool_t wiced_wifi_ds1_needs_fw_disable( void )
{
    if ( WICED_TRUE == ds1_state.config_valid && WICED_TRUE == ds1_state.host_enabled &&
        WICED_TRUE == ds1_state.firmware_enabled && WICED_FALSE == wwd_wifi_sta_is_only_connected( ) )
    {
        return WICED_TRUE;
    }

    return WICED_FALSE;
}

static wiced_result_t wiced_wifi_ds1_fw_enable( wiced_interface_t interface )
{
    wiced_result_t result = WICED_SUCCESS; /* return success for no-op; it's ok */

    /* check preconditions such as one STA associated only */
    if ( WICED_FALSE == wwd_wifi_sta_is_only_connected( ) )
    {
        WICED_WIFI_DEEP_SLEEP_DEBUG(( "Not entering Wi-Fi Deep Sleep 1: STA (and only the STA) needs to be associated first\n" ));
    }
    /* Ensure ds1 is enabled */
    else if ( WICED_FALSE == ds1_state.host_enabled )
    {
        WICED_WIFI_DEEP_SLEEP_DEBUG(( "Not entering Wi-Fi Deep Sleep 1: DS1 not enabled yet. Config %s valid\n", ( WICED_TRUE == ds1_state.config_valid ) ? "is" : "isn't" ));
    }
    /* already done enabling firmware */
    else if ( WICED_TRUE == ds1_state.firmware_enabled )
    {
        /* Already done */
        WICED_WIFI_DEEP_SLEEP_DEBUG(( "Not entering Wi-Fi Deep Sleep 1: already enabled\n" ));
    }
    else
    {
        /* then try to enter if all is ok */
        result = wiced_wifi_ds1_enable_wowl_conditions( interface, ds1_state.config.conditions, NULL );

        if ( WICED_SUCCESS == result )
        {
            result = wwd_wifi_enter_ds1( WICED_TO_WWD_INTERFACE( interface ), ds1_state.config.ulp_wait_milliseconds );
        }

        WICED_WIFI_DEEP_SLEEP_DEBUG(( "Attempt to enter DS1 result=%d\n", (int)result ));

        if ( WICED_SUCCESS == result )
        {
            ds1_state.firmware_enabled = WICED_TRUE;
            WICED_WIFI_DEEP_SLEEP_DEBUG(( "Deep Sleep enabled in firmware\n" ));
        }
    }

    ds1_state.last_fw_enable_result = result;

    return result;
}

wiced_result_t wiced_wifi_ds1_config( wiced_interface_t interface, wiced_offloads_container_t *offload_value, uint32_t ulp_wait_milliseconds )
{
    wiced_result_t result = WICED_SUCCESS;

    if ( ulp_wait_milliseconds < WICED_WIFI_ULP_MIN_MILLISECONDS )
    {
        WPRINT_WICED_INFO(( "Invalid ulp value %d; min is %d\n", (int)ulp_wait_milliseconds, (int)WICED_WIFI_ULP_MIN_MILLISECONDS ));
        return WICED_ERROR;
    }

    if ( WICED_TRUE == ds1_state.host_enabled )
    {
        result = wiced_wifi_ds1_disable( interface );
    }

    if ( WICED_SUCCESS != result )
    {
        WICED_WIFI_DEEP_SLEEP_ERROR(( "Unable to config DS1 (unable to disable): Err %d\n", result ));
        return result;
    }

    wwd_wifi_register_link_update_callback( wiced_wifi_ds1_notify_link_change );

    memset( &ds1_state, 0, sizeof( ds1_state ) );

    /* save config */
    ds1_state.config.conditions            = offload_value;
    ds1_state.config.ulp_wait_milliseconds = ulp_wait_milliseconds;
    ds1_state.config_valid          = WICED_TRUE;
    ds1_state.host_enabled          = WICED_FALSE;
    ds1_state.firmware_enabled      = WICED_FALSE;

    return result;
}

wiced_result_t wiced_wifi_ds1_enable( wiced_interface_t interface )
{
    wiced_result_t result = WICED_SUCCESS;

    /* ensure we're configured */
    if ( WICED_FALSE == ds1_state.config_valid )
    {
        WICED_WIFI_DEEP_SLEEP_ERROR(( "Wi-Fi deep sleep is not configured. Call wiced_wifi_ds1_config first\n" ));
        result = WICED_ERROR;
    }
    else
    {
        /* enabled */
        ds1_state.host_enabled = WICED_TRUE;

        /* try to get things going: if can't enter right away, no problem; code will try later */
        result = wiced_wifi_ds1_fw_enable( interface );

    }

    return result;
}

wiced_result_t wiced_wifi_ds1_disable( wiced_interface_t interface )
{
    wwd_result_t err = WWD_SUCCESS;

    if ( WICED_TRUE == ds1_state.firmware_enabled )
    {
        err = wwd_wifi_ds1_disable( WICED_TO_WWD_INTERFACE( interface ) );
        WICED_WIFI_DEEP_SLEEP_DEBUG(("Err %d while disabling\n", err));
    }

    if ( WWD_SUCCESS == err )
    {
        /* disabled; remember it */
        ds1_state.host_enabled     = WICED_FALSE;
        ds1_state.firmware_enabled = WICED_FALSE;
        return WICED_SUCCESS;
    }
    WICED_WIFI_DEEP_SLEEP_ERROR(("Error %d disabling\n", err));

    return WICED_ERROR;
}

void wiced_wifi_deep_sleep_get_status_string( uint8_t *output, uint16_t max_output_length )
{
    snprintf( (char*)output, max_output_length, "configed=%s host_enab=%s fw_enab=%s last_fw_enab result=%d Init PM=%s\n",
        ( WICED_TRUE == ds1_state.config_valid ) ? "Yes" : "No",
        ( WICED_TRUE == ds1_state.host_enabled ) ? "Yes" : "No",
        ( WICED_TRUE == ds1_state.firmware_enabled ) ? "Yes" : "No",
        ds1_state.last_fw_enable_result,
        ( NO_POWERSAVE_MODE == ds1_state.initial_powersave_mode ) ? "OFF" : "ON" );
}

static wiced_result_t wiced_xlate_pattern_to_wl_pattern( wiced_packet_pattern_t *pattern_in, wl_wowl_pattern_list_t *pattern_list)
{
    wl_wowl_pattern_t *pattern_out = &pattern_list->pattern[0];

    /* one and only for now */
    pattern_list->count = 1;

    pattern_out->masksize      = pattern_in->mask_size;
    pattern_out->offset        = pattern_in->match_offset;
    pattern_out->patternoffset = pattern_in->mask_size + sizeof( *pattern_list );
    pattern_out->patternsize   = pattern_in->pattern_size;

    if ( 0 != pattern_in->mask_size )
    {
        if ( NULL == pattern_in->mask )
        {
            return WICED_BADARG;
        }

        memcpy( ( uint8_t* )pattern_list + sizeof( *pattern_list ), pattern_in->mask, pattern_in->mask_size );
    }

    if ( 0 != pattern_in->pattern_size )
    {
        if ( NULL == pattern_in->pattern )
        {
            wiced_assert( "Null pattern data", 0 != 0 );
            return WICED_BADARG;
        }

        memcpy( ( uint8_t* )pattern_list + pattern_out->patternoffset, pattern_in->pattern, pattern_in->pattern_size );
    }

    return WICED_SUCCESS;
}

static wiced_result_t wiced_xlate_keep_alive_to_wl_keep_alive( wiced_keep_alive_packet_t *keep_alive_in, wl_mkeep_alive_pkt_t *keep_alive_out )
{
    keep_alive_out->version     = WL_MKEEP_ALIVE_VERSION;
    keep_alive_out->length      = WL_MKEEP_ALIVE_FIXED_LEN;
    keep_alive_out->period_msec = keep_alive_in->period_msec;

    /* the one and only wowl keep alive set to FW (for now) */
    keep_alive_out->keep_alive_id = 0;
    keep_alive_out->len_bytes     = keep_alive_in->packet_length;

    if ( 0 != keep_alive_out->len_bytes )
    {
        if ( NULL == keep_alive_in->packet )
        {
            wiced_assert( "Null keepalive data", 0 != 0 );
            return WICED_BADARG;
        }

        memcpy( keep_alive_out->data, keep_alive_in->packet, keep_alive_out->len_bytes );
    }

    return WICED_SUCCESS;
}

/*
  * Enter ds1 mode with a given offload
  */
wiced_result_t wiced_wifi_enter_ds1( wiced_interface_t interface, wiced_offload_t offload_type, wiced_offload_value_t *offload_value, uint32_t ulp_wait_milliseconds )
{
    /* Reuse the debug function, but don't supply any debug info; will be ignored */
    return wiced_wifi_enter_ds1_debug( interface, offload_type, offload_value, ulp_wait_milliseconds, NULL );
}

static void wiced_wifi_wowl_get_values(wiced_offload_t offload_type, uint32_t *wowl, uint32_t *wowl_os, wiced_ds1_debug_t *debug_overrides)
{
    /* first set values as normal */
    switch ( offload_type )
    {
        case WICED_OFFLOAD_MAGIC:
        {
            *wowl    = WL_WOWL_MAGIC;
            *wowl_os = 0;
        }
        break;
        case WICED_OFFLOAD_PATTERN:
        {
            *wowl    = WL_WOWL_NET;
            *wowl_os = *wowl;
        }
        break;
        case WICED_OFFLOAD_ARP_HOSTIP:
        case WICED_OFFLOAD_KEEP_ALIVE:
        {
            *wowl    = WL_WOWL_TRAFFIC;
            *wowl_os = *wowl;
        }
        break;
        case WICED_OFFLOAD_GTK:
        {
            *wowl    = WL_WOWL_GTK;
            *wowl_os = WL_WOWL_GTK;
        }
        break;
        case WICED_OFFLOAD_DEAUTH:
        {
            *wowl    = WL_WOWL_DEAUTH;
            *wowl_os = 0;
        }
        break;
        case WICED_OFFLOAD_ALL:
        {
            *wowl    = WL_WOWL_ALL;
            *wowl_os = WL_WOWL_ALL;
        }
        break;

        default:
            wiced_assert( "Invalid offload type", 0 != 0 );
            *wowl    = 0;
            *wowl_os = 0;
        break;
    }


    if ( NULL != debug_overrides )
    {
        if ( WICED_TRUE == debug_overrides->wowl_valid )
        {
            *wowl = debug_overrides->wowl;
        }

        if ( WICED_TRUE == debug_overrides->wowl_os_valid )
        {
            *wowl_os = debug_overrides->wowl_os;
        }
    }
}

/* check out what the original powersave state is and disable powersave if needed */
static void wiced_wifi_restore_powersave_state( wiced_interface_t interface )
{
    /* this module only modifies powersave state when it starts in the OFF mode */
    if ( NO_POWERSAVE_MODE == ds1_state.initial_powersave_mode )
    {
        wwd_result_t result = wwd_wifi_disable_powersave_interface( interface );

        /* if error, at least log it: then limp along */
        if ( WWD_SUCCESS != result )
            WPRINT_WICED_ERROR(("Error on restore of powersave state %d\n", result));
    }
}

static wiced_result_t wiced_wifi_ds1_enable_wowl_conditions( wiced_interface_t interface, wiced_offloads_container_t *conditions, wiced_ds1_debug_t *debug_overrides )
{
    uint32_t        wowl      = 0;
    uint32_t        wowl_os   = 0;

    wiced_offload_value_t *offload_value = NULL;
    wiced_offload_t offload_type;

    int offload_index;

    uint8_t                *pattern_cmd         = NULL;
    uint32_t                pattern_data_size   = 0;
    wl_mkeep_alive_pkt_t  *wowl_keepalive_data  = NULL;
    uint32_t arp_host_ip_v4_address             = 0;
    wiced_bool_t arp_address_valid              = WICED_FALSE;
    wiced_result_t result     = WICED_SUCCESS;
    wwd_result_t   wwd_result = WWD_SUCCESS;

     /* sanity check validity */
    if ( NULL == conditions || 0 == conditions->num_offloads || NULL == conditions->types || NULL == conditions->values )
    {
        WICED_WIFI_DEEP_SLEEP_ERROR(("Invalid wowl conditions; can't enable\n"));
        WICED_WIFI_DEEP_SLEEP_ERROR(("conditions=%p\n", (void*)conditions));
        if ( NULL != conditions )
        {
            WICED_WIFI_DEEP_SLEEP_ERROR(("num_offloads=%d types=%p values=%p\n", conditions->num_offloads, (void*)conditions->types, (void*)conditions->values));
        }
        else
        {
            WICED_WIFI_DEEP_SLEEP_ERROR(("Wowl conditions NULL\n"));
        }

        return WICED_ERROR;
    }

    /* check powersave: save current state and if needed, enable it */
    wwd_result = wwd_wifi_get_powersave_interface( WWD_STA_INTERFACE, &ds1_state.initial_powersave_mode );
    if ( WWD_SUCCESS != wwd_result )
    {
        WICED_WIFI_DEEP_SLEEP_ERROR(("Err %d: unable to query power save\n", wwd_result));
        return WICED_ERROR;
    }

    /* Mode is OFF, so must enable it */
    if ( NO_POWERSAVE_MODE == ds1_state.initial_powersave_mode )
    {
        wwd_result = wwd_wifi_enable_powersave( );
        if ( WWD_SUCCESS != wwd_result )
        {
            WICED_WIFI_DEEP_SLEEP_ERROR(("Err %d: unable to enable power save\n", wwd_result));
            /* bail here, as no memory has been allocated and no powersave mode changed */
            return WICED_ERROR;
        }
    }

    /* get bitmask for conditions */
    for ( offload_index = 0 ; offload_index < conditions->num_offloads ; offload_index++ )
    {
        uint32_t        wowl_temp    = 0;
        uint32_t        wowl_os_temp = 0;

        offload_type = conditions->types[offload_index];

        /* translate the offload type to corresponding values (or use override) */
        wiced_wifi_wowl_get_values( offload_type, &wowl_temp, &wowl_os_temp, debug_overrides );

        wowl    |= wowl_temp;
        wowl_os |= wowl_os_temp;
    }

    /* get all the data formatted for sending to WWD */
    for ( offload_index = 0 ; offload_index < conditions->num_offloads ; offload_index++ )
    {
        offload_type  = conditions->types[offload_index];
        offload_value = &conditions->values[offload_index];

        switch ( offload_type )
        {
            case WICED_OFFLOAD_MAGIC:
            case WICED_OFFLOAD_GTK:
            case WICED_OFFLOAD_ALL:
            case WICED_OFFLOAD_DEAUTH:
            {
                continue;/* The wowl and wowl_os values will cause these to start */
            }
            break;
            case WICED_OFFLOAD_PATTERN:
            {
                wl_wowl_pattern_list_t *pattern_list = NULL;
                pattern_data_size = sizeof( *pattern_list ) + offload_value->pattern.mask_size + offload_value->pattern.pattern_size;

                /* only one pattern supported */
                if ( NULL != pattern_cmd )
                {
                    result = WICED_BADARG;
                    break;
                }

                /* allocate memory and xlate */
                pattern_cmd  = calloc( pattern_data_size, 1 );

                if ( NULL == pattern_cmd )
                {
                    result = WICED_OUT_OF_HEAP_SPACE;
                    break;
                }

                pattern_list = ( wl_wowl_pattern_list_t* )( pattern_cmd );
                result = wiced_xlate_pattern_to_wl_pattern( &offload_value->pattern, pattern_list );

                if ( WICED_SUCCESS != result )
                {
                    break;
                }
            }
            break;
            case WICED_OFFLOAD_KEEP_ALIVE:
            {
                if ( NULL != wowl_keepalive_data )
                {
                    result = WICED_BADARG;
                    break;
                }

                /* allocate memory and xlate */
                wowl_keepalive_data = malloc( sizeof( *wowl_keepalive_data ) + offload_value->keep_alive_packet_info.packet_length );

                if ( NULL == wowl_keepalive_data )
                {
                    result = WICED_OUT_OF_HEAP_SPACE;
                    break;
                }

                result = wiced_xlate_keep_alive_to_wl_keep_alive( &offload_value->keep_alive_packet_info, wowl_keepalive_data );

                if ( WICED_SUCCESS != result )
                {
                    break;
                }
            }
            break;
            case WICED_OFFLOAD_ARP_HOSTIP:
            {
                if ( WICED_IPV4 != offload_value->ipv4_address.version )
                {
                    WICED_WIFI_DEEP_SLEEP_ERROR(("IpV6 not applicable for ARP offload\n"));
                    /* Ipv6 support can't exist for ARP */
                    result = WICED_BADARG;
                    break;
                }

                memcpy( &arp_host_ip_v4_address, &offload_value->ipv4_address.ip.v4, sizeof( arp_host_ip_v4_address ) );
                arp_address_valid = WICED_TRUE;
            }
            break;
            default:
                WICED_WIFI_DEEP_SLEEP_ERROR(("Bad offload type\n"));
                result = WICED_BADARG;
            break;
        }
    }

    if ( WICED_SUCCESS == result )
    {
        result = wwd_wifi_wowl_enable( WICED_TO_WWD_INTERFACE( interface ), wowl, wowl_os, wowl_keepalive_data, pattern_cmd, pattern_data_size, ( WICED_FALSE == arp_address_valid ) ? NULL : &arp_host_ip_v4_address );
    }

    /* if there's an error, try to restore PM state to previous value if needed */
    if ( WICED_SUCCESS != result )
    {
        wiced_wifi_restore_powersave_state( interface );
    }

    /* clean up allocated memory */
    if ( NULL != pattern_cmd )
    {
        free( pattern_cmd );
    }

    if ( NULL != wowl_keepalive_data  )
    {
        free( wowl_keepalive_data );
    }

    WICED_WIFI_DEEP_SLEEP_DEBUG(("%d wowl conditions enabled\n", offload_index));

    return result;
}

wiced_result_t wiced_wifi_enter_ds1_debug( wiced_interface_t interface, wiced_offload_t offload_type, wiced_offload_value_t *offload_value, uint32_t ulp_wait_milliseconds, wiced_ds1_debug_t *debug_overrides )
{
    wiced_offloads_container_t conditions;
    wiced_result_t  result    = WICED_SUCCESS;

    /* Must be associated prior to enter DS1 */
    if ( WICED_FALSE == wiced_wifi_is_sta_link_up( ) )
    {
        WPRINT_WICED_INFO(( "Invalid state for DS; STA needs to be associated first\n" ));
        return WICED_ERROR;
    }

    if ( ulp_wait_milliseconds < WICED_WIFI_ULP_MIN_MILLISECONDS )
    {
        WPRINT_WICED_INFO(("Invalid ulp value %d; min is %d\n", (int)ulp_wait_milliseconds, (int)WICED_WIFI_ULP_MIN_MILLISECONDS));
        return WICED_ERROR;
    }

    conditions.num_offloads = 1;
    conditions.types        = &offload_type;
    conditions.values       = offload_value;

    result = wiced_wifi_ds1_enable_wowl_conditions( interface, &conditions, debug_overrides );

    if ( WICED_SUCCESS == result )
    {
        CHECK_RETURN( wwd_wifi_enter_ds1( WICED_TO_WWD_INTERFACE( interface ), ulp_wait_milliseconds ) );

        /* success */
        ds1_state.firmware_enabled = WICED_TRUE;
    }

    return result;
}

static void wiced_wifi_ds1_wwd_callback( void *user_parameter )
{
    if ( STATE_DS_ENABLED == wwd_wifi_ds1_get_state( ) )
    {
        wiced_wifi_ds1_callback_data_t *callback_data = (wiced_wifi_ds1_callback_data_t *)user_parameter;
        if ( NULL != callback_data->callback )
        {
            callback_data->callback( callback_data->user_parameter );
        }
    }
}

wiced_result_t wiced_wifi_ds1_set_complete_callback( wiced_wifi_ds1_complete_callback_t callback, void *user_parameter )
{
    wiced_wifi_ds1_callback_data.callback       = callback;
    wiced_wifi_ds1_callback_data.user_parameter = user_parameter;

    if ( WWD_SUCCESS == wwd_wifi_ds1_set_state_change_callback( (wwd_ds1_state_change_callback_t)wiced_wifi_ds1_wwd_callback, &wiced_wifi_ds1_callback_data ) )
    {
        return WICED_SUCCESS;
    }

    return WICED_ERROR;
}

wiced_result_t wiced_wifi_wake_ds1( wiced_interface_t interface )
{
    char version[200];
    UNUSED_PARAMETER( interface );

    /* force waking from DS1 */
    if ( wwd_wifi_get_wifi_version( version, sizeof( version ) ) == WWD_SUCCESS )
    {
        printf("WLAN Firmware    : %s\r\n", version );
    }

    /* Note: powersave and wowl still enabled here: if client wants to exit it can do so; want to avoid unnecessary side-effects */
    return WICED_SUCCESS;
}
