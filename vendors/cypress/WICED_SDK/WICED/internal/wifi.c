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
#include "wiced_network.h"
#include "wwd_events.h"
#include "platform_dct.h"
#include "string.h"
#include "wwd_wifi.h"
#ifndef WICED_AMAZON_FREERTOS_SDK
#include "wiced_wps.h"
#endif
#include "wiced_utilities.h"
#include "internal/wiced_internal_api.h"
#include "wwd_management.h"
#include "wiced_management.h"
#include "wiced_platform.h"
#include "wiced_framework.h"
#include "wiced_crypto.h"
#include "wiced_low_power.h"
#include "internal/wwd_internal.h"
#include "wwd_wifi_sleep.h"
#include "internal/wwd_sdpcm.h"
#include "network/wwd_buffer_interface.h"
#include "platform/wwd_platform_interface.h"
#include "wiced_power_logger.h"
#include "wifi_utils.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define CHECK_IOCTL_BUFFER( buff )  if ( buff == NULL ) {  wiced_assert("Allocation failed\n", 0 == 1); return WICED_WWD_BUFFER_ALLOC_FAIL; }
#define CHECK_RETURN( expr )  { wiced_result_t check_res = (wiced_result_t)(expr); if ( check_res != WICED_SUCCESS ) { wiced_assert("Command failed\n", 0); return check_res; } }

#define DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_SEC        ( 1 )
#define MAX_RETRY_BACKOFF_TIMEOUT_IN_SEC            ( DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_SEC << 5 )

/******************************************************
 *                    Constants
 ******************************************************/

#define WLC_EVENT_MSG_LINK      ( 0x0001 )  /** link is up */

#define SCAN_BSSID_LIST_LENGTH  ( 100 )
#define SCAN_LONGEST_WAIT_TIME  ( 10000 )   /* Dual band devices take over 4000 milliseconds to complete a scan. In the associated case, this time could be doubled */
#define HANDSHAKE_TIMEOUT_MS    ( 3000 )

#define RRM_TIMEOUT             (40)

#define NAN_EVENT_BUF_SIZE      (512)
unsigned char nan_event_buf[NAN_EVENT_BUF_SIZE];

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
typedef struct
{
    wiced_scan_result_handler_t results_handler;
    void*                       user_data;
    wiced_bool_t                is_pno_scan;
} internal_scan_handler_t;
static internal_scan_handler_t scan_handler;

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static void*          wiced_link_events_handler ( const wwd_event_header_t* event_header, const uint8_t* event_data, void* handler_user_data );
static void           wiced_scan_result_handler ( wiced_scan_result_t** result_ptr, void* user_data, wiced_scan_status_t status );
static void           find_ap_scan_handler      ( wiced_scan_result_t** result_ptr, void* user_data, wiced_scan_status_t status );
static void           handshake_timeout_handler ( void* arg );
static wiced_result_t handshake_error_callback  ( void* arg );
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
static void           link_up                   ( wiced_interface_t interface );
static void           link_down                 ( wiced_interface_t interface );
#else
static void           link_up                   ( void );
static void           link_down                 ( void );
#endif
static void*          softap_event_handler      ( const wwd_event_header_t* event_header, const uint8_t* event_data, /*@returned@*/void* handler_user_data );
static wiced_result_t wiced_wifi_init           ( void );
static void           wiced_wifi_scan_cleanup( void );


/******************************************************
 *               Variable Definitions
 ******************************************************/

/* Link management variables */
static const wwd_event_num_t        link_events[]           = { WLC_E_LINK, WLC_E_DEAUTH_IND, WLC_E_DISASSOC_IND, WLC_E_PSK_SUP, WLC_E_NONE };
static const wwd_event_num_t        ap_client_events[]      = { WLC_E_DEAUTH, WLC_E_DEAUTH_IND, WLC_E_DISASSOC, WLC_E_DISASSOC_IND, WLC_E_ASSOC_IND, WLC_E_REASSOC_IND, WLC_E_NONE };
static const wwd_event_num_t        nan_events[]            = { WLC_E_NAN, WLC_E_NONE };
static wiced_bool_t                 WICED_DEEP_SLEEP_SAVED_VAR( wiced_wlan_initialised )  = WICED_FALSE;
static wiced_timer_t                wiced_sta_handshake_timer;
static wiced_timed_event_t          wiced_sta_join_retry_timer;

#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
static wiced_bool_t                 WICED_DEEP_SLEEP_SAVED_VAR( wiced_sta_link_up[WICED_INTERFACE_MAX] )       = {WICED_FALSE};
static wiced_security_t             WICED_DEEP_SLEEP_SAVED_VAR( wiced_sta_security_type[WICED_INTERFACE_MAX] ) = {WICED_SECURITY_UNKNOWN};
static wiced_bool_t                 link_up_event_received[WICED_INTERFACE_MAX]  = {WICED_FALSE};
#else
static wiced_bool_t                 WICED_DEEP_SLEEP_SAVED_VAR( wiced_sta_link_up )       = WICED_FALSE;
static wiced_security_t             WICED_DEEP_SLEEP_SAVED_VAR( wiced_sta_security_type ) = WICED_SECURITY_UNKNOWN;
static wiced_bool_t                 link_up_event_received  = WICED_FALSE;
#endif


/* Scanning variables */
static wiced_bool_t                 g_scan_semaphore_and_timer_inited;
static wiced_semaphore_t            scan_semaphore;

static wiced_scan_handler_result_t* scan_result_ptr;
static wiced_mac_t*                 scan_bssid_list = NULL;
static int                          current_bssid_list_length = 0;

static wiced_wifi_softap_event_handler_t ap_event_handler;

static wiced_country_code_t         WICED_DEEP_SLEEP_SAVED_VAR( country_code ) = WICED_DEFAULT_COUNTRY_CODE;
static wiced_wifi_rrm_event_handler_t rrm_event_handler;

static wiced_wifi_nan_event_handler_t nan_event_handler;

static wiced_ssid_t*                wiced_wifi_pno_ssid     = NULL;
static wiced_security_t             wiced_wifi_pno_security = WICED_SECURITY_UNKNOWN;
static char                         last_joined_ssid[SSID_NAME_SIZE+1]= ""; /* 32 characters + terminating null */

static uint8_t                      retry_backoff_timeout = DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_SEC;
/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_result_t wiced_init( void )
{
    /* used for monitoring init run-time */
    wiced_result_t status = WICED_SUCCESS;
    wwd_time_t elapsed_time;

    WPRINT_WICED_INFO( ("\nStarting WICED " WICED_VERSION "\n") );

    CHECK_RETURN( wiced_core_init( ) );

    /* Track and log how long to start up */
    elapsed_time = host_rtos_get_time( );

    status = wiced_wlan_connectivity_init( );
    if ( status != WICED_SUCCESS )
    {
        wiced_core_deinit( );
    }
    else
    {
       /************************************************************************
             Note: init time under 500ms is expected, generally (measured on 43364 SDIO system).
             Init times that are excessively elongated will result in unexpected failures that will
             generally appear during Wi-FI bus enablement or access.
            ************************************************************************/
        WPRINT_WICED_DEBUG( ("Init took ~%lu ms\n", host_rtos_get_time( ) - elapsed_time) );
    }

    UNUSED_PARAMETER( elapsed_time );
    return status;
}

wiced_result_t wiced_resume_after_deep_sleep( void )
{
    /* used for monitoring init run-time */
    wwd_time_t start_time;

    WPRINT_WICED_INFO( ("\nResuming WICED v" WICED_VERSION "\n") );

    if ( !WICED_DEEP_SLEEP_IS_ENABLED( ) )
    {
        wiced_assert( "Deep-sleep is not supported", 0 );
        return WICED_UNSUPPORTED;
    }

    if ( !WICED_DEEP_SLEEP_IS_WARMBOOT_HANDLE( ) )
    {
        wiced_assert( "Deep-sleep is warmboot not handled", 0 );
        return WICED_ERROR;
    }

    CHECK_RETURN( wiced_core_init( ) );

    /* Track and log how long to resume */
    start_time = host_rtos_get_time( );

    CHECK_RETURN( wiced_wlan_connectivity_resume_after_deep_sleep( ) );
    WICED_SLEEP_CALL_EVENT_HANDLERS( WICED_DEEP_SLEEP_IS_WARMBOOT( ), WICED_SLEEP_EVENT_WLAN_RESUME );

    WPRINT_WICED_INFO( ("\nResume took ~%lu ms\n", host_rtos_get_time( ) - start_time) );

    UNUSED_PARAMETER( start_time );

    return WICED_SUCCESS;
}

wiced_result_t wiced_wlan_connectivity_init( void )
{
    wiced_result_t              result;
    platform_dct_wifi_config_t* wifi_config;
    platform_dct_misc_config_t* dct_misc;
    uint32_t                    wlan_rand;
    wiced_bool_t                random_seed_needed = WICED_TRUE;

#ifdef WPRINT_ENABLE_NETWORK_INFO
    wiced_mac_t  mac;
    char         version[200];
#endif
    uint32_t cycles = host_platform_get_cycle_count( );

    if ( wiced_wlan_initialised == WICED_TRUE )
    {
        return WICED_SUCCESS;
    }

    CHECK_RETURN( wiced_network_init( ) );

    /* Initialise Wiced */
    CHECK_RETURN( wiced_dct_read_lock( (void**) &wifi_config, WICED_FALSE, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t) ) );
    CHECK_RETURN( wiced_dct_read_lock( (void**) &dct_misc, WICED_FALSE, DCT_MISC_SECTION, 0, sizeof(platform_dct_misc_config_t) ) );

    if (wifi_config->device_configured == WICED_TRUE)
    {
        country_code = wifi_config->country_code;
    }
    wiced_dct_read_unlock( wifi_config, WICED_FALSE );
    wiced_dct_read_unlock( dct_misc, WICED_FALSE );
    WPRINT_NETWORK_DEBUG( ( "WWD " BUS " interface initializing with %c%c/%d\n",
         ((country_code) >>  0) & 0xff, ((country_code) >>  8) & 0xff, ((country_code) >> 16) & 0xffff) );

    result = ( wiced_result_t )wwd_management_wifi_on( country_code );

    /* e.g. user requested an abort or other error */
    if ( result != WICED_SUCCESS )
    {
        wiced_network_deinit( );

        WPRINT_NETWORK_DEBUG( ( "WWD " BUS " interface initialization FAILED: result %d\n", result) );
        return result;
    }

    /* get a seed, using hardware on wlan module (if supported); otherwise, seed comes due to difference in cycles for wiced_init completion */
    result = ( wiced_result_t )wwd_wifi_get_iovar_value( IOVAR_STR_RAND, &wlan_rand, WWD_STA_INTERFACE );

    /* Not always a supported operation */
    if ( result == WICED_SUCCESS )
    {
        random_seed_needed = WICED_FALSE;
        wiced_crypto_add_entropy( &wlan_rand, sizeof( wlan_rand ) );
    }
    /* else generate a seed using elapsed time of wiced_init; at end of function */

    memset( &wlan_rand, 0, sizeof( wlan_rand ) );

    wiced_wifi_up();

    if ( WICED_DEEP_SLEEP_IS_ENABLED() )
    {
        wwd_wifi_get_and_cache_mac_address( WWD_STA_INTERFACE );
    }

#ifdef WPRINT_ENABLE_NETWORK_INFO
    if ( wwd_wifi_get_mac_address( &mac, WWD_STA_INTERFACE ) == WWD_SUCCESS )
    {
        WPRINT_NETWORK_INFO(("WLAN MAC Address : %02X:%02X:%02X:%02X:%02X:%02X\r\n", mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5]));
    }

    if ( wwd_wifi_get_wifi_version( version, sizeof( version ) ) == WWD_SUCCESS )
    {
        WPRINT_NETWORK_INFO( ("WLAN Firmware    : %s\r\n", version ) );
    }
#if !defined(BOOTLOADER) && !defined(TINY_BOOTLOADER)
    /* minimize bootloader usage and start time from UART output */
    if ( wwd_wifi_get_clm_version( version, sizeof( version )) == WWD_SUCCESS )
    {
        WPRINT_NETWORK_INFO( ("WLAN CLM         : %s\r\n", version ) );
    }
#endif
#endif

    /* Configure roaming parameters */
    wwd_wifi_set_roam_trigger( WICED_WIFI_ROAMING_TRIGGER_MODE );
    wwd_wifi_set_roam_delta( WICED_WIFI_ROAMING_TRIGGER_DELTA_IN_DBM );
    wwd_wifi_set_roam_scan_period( WICED_WIFI_ROAMING_SCAN_PERIOD_IN_SECONDS );

    wiced_wlan_initialised = WICED_TRUE;

    ap_event_handler = NULL;

    rrm_event_handler = NULL;

    /* if unable to get random from Wi-Fi iovar, then do seeding here */
    if ( WICED_TRUE == random_seed_needed )
    {
        /* Small amount of variance in cycles taken to complete wiced_init; seed of PRNG */
        wiced_crypto_prng_add_low_variability_entropy( host_platform_get_cycle_count( ) - cycles );
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_wlan_connectivity_resume_after_deep_sleep( void )
{
    if ( !WICED_DEEP_SLEEP_IS_ENABLED( ) )
    {
        wiced_assert( "Deep-sleep is not supported", 0 );
        return WICED_UNSUPPORTED;
    }
    else
    {

        CHECK_RETURN( ( wiced_wlan_initialised == WICED_TRUE ) ? WICED_SUCCESS : WICED_NOTUP );

        CHECK_RETURN( wiced_network_init( ) );

        CHECK_RETURN( wiced_wifi_init( ) );

        CHECK_RETURN( (wiced_result_t) wwd_management_wifi_platform_init( country_code, WICED_TRUE ) );

        CHECK_RETURN( (wiced_result_t) wwd_management_set_event_handler_locally( link_events, wiced_link_events_handler, NULL, WWD_STA_INTERFACE ) );

        wwd_wlan_status.state = WLAN_UP;

        return WICED_SUCCESS;
    }
}

static wiced_result_t wiced_wifi_init( void )
{
    if( g_scan_semaphore_and_timer_inited == WICED_FALSE)
    {
        CHECK_RETURN( wiced_rtos_init_semaphore( &scan_semaphore ) );
        CHECK_RETURN( wiced_rtos_set_semaphore( &scan_semaphore ) ); /* Semaphore starts at 1 */

        CHECK_RETURN( wiced_rtos_init_timer( &wiced_sta_handshake_timer, HANDSHAKE_TIMEOUT_MS, handshake_timeout_handler, 0 ) );
        retry_backoff_timeout = DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_SEC;
        g_scan_semaphore_and_timer_inited = WICED_TRUE;
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_wifi_up( void )
{
    if ( wwd_wlan_status.state == WLAN_DOWN )
    {
        CHECK_RETURN( wwd_wifi_set_up( ) );
    }

    CHECK_RETURN( wiced_wifi_init( ) );

    return WICED_SUCCESS;
}

wiced_result_t wiced_wlan_connectivity_deinit( void )
{
    if ( wiced_wlan_initialised != WICED_TRUE )
    {
        return WICED_SUCCESS;
    }

    wiced_wifi_down();

    wwd_management_wifi_off( );

    wiced_network_deinit( );

    wiced_wlan_initialised = WICED_FALSE;

    return WICED_SUCCESS;
}

wiced_result_t wiced_wifi_down( void )
{
    CHECK_RETURN( wiced_network_down( WICED_AP_INTERFACE ) );
    CHECK_RETURN( wiced_network_down( WICED_STA_INTERFACE ) );
    CHECK_RETURN( wiced_network_down( WICED_P2P_INTERFACE ) );

    if( g_scan_semaphore_and_timer_inited == WICED_TRUE)
    {
        CHECK_RETURN( wiced_rtos_deinit_timer( &wiced_sta_handshake_timer ) );
        wiced_rtos_deregister_timed_event( &wiced_sta_join_retry_timer );
        retry_backoff_timeout = DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_SEC;
        CHECK_RETURN( wiced_rtos_deinit_semaphore( &scan_semaphore ) );
        g_scan_semaphore_and_timer_inited = WICED_FALSE;
    }

    if ( wwd_wlan_status.state == WLAN_UP )
    {
        CHECK_RETURN( wwd_wifi_set_down( ) );
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_deinit( void )
{
    wiced_wlan_connectivity_deinit( );
    wiced_core_deinit( );

    return WICED_SUCCESS;
}

#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
wiced_result_t wiced_start_sta( wiced_interface_t interface )
{
    return (wiced_result_t) wwd_wifi_start_sta( WICED_TO_WWD_INTERFACE(interface) );
}
#endif

wiced_result_t wiced_start_ap( wiced_ssid_t* ssid, wiced_security_t security, const char* key, uint8_t channel )
{
    return (wiced_result_t) wwd_wifi_start_ap(ssid, security, (uint8_t*)key, (uint8_t) strlen(key), channel);
}

wiced_result_t wiced_stop_ap( void )
{
    return (wiced_result_t) wwd_wifi_stop_ap();
}

wiced_result_t wiced_wifi_register_softap_event_handler( wiced_wifi_softap_event_handler_t event_handler )
{
    ap_event_handler = event_handler;
    return (wiced_result_t) wwd_management_set_event_handler( ap_client_events, softap_event_handler, NULL, WWD_AP_INTERFACE );
}

wiced_result_t wiced_wifi_unregister_softap_event_handler( void )
{
    return (wiced_result_t) wwd_management_set_event_handler( ap_client_events, NULL, NULL, WWD_AP_INTERFACE );
}


wiced_result_t wiced_wifi_register_rrm_event_handler( wiced_wifi_rrm_event_handler_t event_handler )
{
    rrm_event_handler = event_handler;
    return WICED_SUCCESS;
}

wiced_result_t wiced_wifi_unregister_rrm_event_handler( void  )
{
    rrm_event_handler = NULL;
    return WICED_SUCCESS;
}


wiced_result_t wiced_wifi_register_nan_event_handler( wiced_wifi_nan_event_handler_t event_handler )
{
    nan_event_handler = event_handler;
    return WICED_SUCCESS;
}


wiced_result_t wiced_wifi_unregister_nan_event_handler ( void )
{
    nan_event_handler = NULL;
    return WICED_SUCCESS;
}


wiced_result_t wiced_nan_config_enable ( wiced_wifi_nan_event_handler_t event_handler )
{
    wwd_result_t result;
    memset(nan_event_buf, 0, sizeof(nan_event_buf));
    wwd_management_set_event_handler( nan_events, wiced_nan_scan_handler, nan_event_buf, WWD_STA_INTERFACE );
    result = wwd_nan_config_enable ();
    if( result == WWD_SUCCESS ) {
        wiced_wifi_register_nan_event_handler (event_handler );
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_nan_config_disable ( void )
{
    wwd_nan_config_disable();
    nan_event_handler = NULL;
    return WICED_SUCCESS;
}

wiced_result_t wiced_enable_powersave( void )
{
    wiced_result_t result;

    /* Enable MCU powersave */
    wiced_platform_mcu_enable_powersave();

    /* Enable Wi-Fi powersave */
    result = wiced_wifi_enable_powersave();
    if (result != WICED_SUCCESS)
    {
        goto done;
    }

    /* Suspend networking timers */
    result = wiced_network_suspend();

    done:
        return result;
}

wiced_result_t wiced_wifi_join_specific(char* ssid, uint8_t ssid_length, wiced_security_t auth_type, uint8_t* security_key, uint16_t key_length, char* bssid, char* channel, char* ip, char* netmask, char* gateway)
{
    wiced_network_config_t network_config;
    wiced_ip_setting_t     static_ip_settings;
    wiced_scan_result_t    ap;
    wiced_result_t         result;

    if (wwd_wifi_is_ready_to_transceive(WWD_STA_INTERFACE) == WWD_SUCCESS)
    {
        return WICED_SUCCESS;
    }

    memset( &ap, 0, sizeof( ap ) );
    ap.SSID.length = ssid_length;
    memcpy( ap.SSID.value, ssid, ap.SSID.length );
    wifi_utils_str_to_mac( bssid, &ap.BSSID );
    ap.channel = (uint8_t)atoi( channel );
    ap.security = auth_type;
    ap.band = WICED_WIFI_CH_TO_BAND( ap.channel );
    ap.bss_type = WICED_BSS_TYPE_INFRASTRUCTURE;

    if ( !( NULL_MAC(ap.BSSID.octet) ) )
    {
        result = (wiced_result_t)wwd_wifi_join_specific( &ap, security_key, (uint8_t)key_length, NULL, WWD_STA_INTERFACE );
        if ( result == WICED_SUCCESS )
        {
            /* Tell the network stack to setup its interface */
            if ( NULL == ip )
            {
                network_config = WICED_USE_EXTERNAL_DHCP_SERVER;
            }
            else
            {
                network_config = WICED_USE_STATIC_IP;
                str_to_ip( ip,      &static_ip_settings.ip_address );
                str_to_ip( netmask, &static_ip_settings.netmask );
                str_to_ip( gateway, &static_ip_settings.gateway );
            }

            if ( ( result = wiced_ip_up( WICED_STA_INTERFACE, network_config, &static_ip_settings ) ) == WICED_SUCCESS )
            {
                strlcpy( last_joined_ssid, ssid, MIN(sizeof(last_joined_ssid), (size_t)ssid_length+1) );
                return result;
            }
        }
        return result;
    }
    return WICED_ERROR;
}

wiced_result_t wiced_wifi_join_adhoc(char* ssid, uint8_t ssid_length, wiced_security_t auth_type, uint8_t* security_key, uint16_t key_length, char* channel, char* ip, char* netmask, char* gateway)
{
    int chan;
    wiced_network_config_t network_config;
    wiced_ip_setting_t     static_ip_settings;
    wiced_scan_result_t    ap;
    wiced_result_t         result;

    if (wwd_wifi_is_ready_to_transceive(WWD_STA_INTERFACE) == WWD_SUCCESS)
    {
        return WICED_SUCCESS;
    }

    chan = atoi(channel);

    memset( &ap, 0, sizeof( ap ) );
    ap.SSID.length = ssid_length;
    memcpy( ap.SSID.value, ssid, ap.SSID.length );
    if (chan > 0)
    {
       ap.channel = (uint8_t)chan;
    }
    else
    {
       WPRINT_APP_INFO(("Channel 0 is not valid, defaulting to channel 1\n"));
       ap.channel = 1;
    }
    ap.bss_type = WICED_BSS_TYPE_ADHOC;
    ap.security = auth_type;
    ap.band = WICED_WIFI_CH_TO_BAND( ap.channel );

    result = (wiced_result_t)wwd_wifi_join_specific( &ap, security_key, (uint8_t)key_length, NULL, WWD_STA_INTERFACE );
    if ( result == WICED_SUCCESS )
    {
        network_config = WICED_USE_STATIC_IP;
        str_to_ip( ip,      &static_ip_settings.ip_address );
        str_to_ip( netmask, &static_ip_settings.netmask );
        str_to_ip( gateway, &static_ip_settings.gateway );

        if ( ( result = wiced_ip_up( WICED_STA_INTERFACE, network_config, &static_ip_settings ) ) == WICED_SUCCESS )
        {
            strlcpy( last_joined_ssid, ssid, MIN(sizeof(last_joined_ssid), (size_t)ssid_length+1) );
            return result;
        }
    }

    return result;
}

wiced_result_t wiced_disable_powersave( void )
{
    wiced_network_resume();
    wiced_wifi_disable_powersave();
    wiced_platform_mcu_disable_powersave();
    return WICED_SUCCESS;
}

/* Configure halt of any joins or scans in progress */
wiced_result_t wiced_wifi_join_halt( wiced_bool_t halt )
{
    return (wiced_result_t)wwd_wifi_join_halt( halt );
}

/** Join a Wi-Fi network as a client
 *      The AP SSID/Passphrase credentials used to join the AP must be available in the DCT
 *      Use the low-level wiced_wifi_join() API to directly join a specific AP if required
 *
 * @return WICED_SUCCESS : Connection to the AP was successful
 *         WICED_ERROR   : Connection to the AP was NOT successful
 */
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
wiced_result_t wiced_join_ap( wiced_interface_t interface )
#else
wiced_result_t wiced_join_ap( void )
#endif
{
    unsigned int             a;
    int                      retries;
    wiced_config_ap_entry_t* ap;
    wiced_result_t           result = WICED_NO_STORED_AP_IN_DCT;

    unsigned int start = 0;
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
    /* 0th interface uses first AP in list, 1st AP uses 1st AP in list.
     * This kills the fallback mechanism for the 0th interface util something better is put in place.
     */
    start = wwd_get_bss_index( interface );
#else
    wiced_interface_t interface = WICED_STA_INTERFACE;
#endif

    if (wwd_wifi_is_mesh_enabled() && (interface == WICED_STA_INTERFACE)) {
        result = WICED_SUCCESS;
        goto exit;
    }

    for ( retries = WICED_JOIN_RETRY_ATTEMPTS; retries != 0; --retries )
    {
        /* Try all stored APs */
        for ( a = start; a < CONFIG_AP_LIST_SIZE; ++a )
        {
            /* Check if the entry is valid */
            CHECK_RETURN( wiced_dct_read_lock( (void**) &ap, WICED_FALSE, DCT_WIFI_CONFIG_SECTION, (uint32_t) ( OFFSETOF(platform_dct_wifi_config_t,stored_ap_list) + a * sizeof(wiced_config_ap_entry_t) ), sizeof(wiced_config_ap_entry_t) ) );
            if ( ap->details.SSID.length != 0 )
            {
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
                result = wiced_join_ap_specific( interface, &ap->details, ap->security_key_length, ap->security_key );
#else
                result = wiced_join_ap_specific( &ap->details, ap->security_key_length, ap->security_key );
#endif

#ifdef FIRMWARE_WITH_PMK_CALC_SUPPORT
                if ( result == WICED_SUCCESS )
                {
                    /* Extract the calculated PMK and store it in the DCT to speed up future associations */
                    if ( ( ( ( ap->details.security & WPA_SECURITY ) || ( ap->details.security & WPA2_SECURITY ) ) &&  ( ( ap->details.security & ENTERPRISE_ENABLED ) == 0 ) ) &&
                            ( ap->security_key_length != WSEC_MAX_PSK_LEN ) && ( ( ap->details.security & IBSS_ENABLED ) == 0 ) )
                    {
                        wiced_config_ap_entry_t ap_temp;
                        memcpy( &ap_temp, ap, sizeof(wiced_config_ap_entry_t) );
                        if ( wwd_wifi_get_pmk( ap_temp.security_key, ap_temp.security_key_length, ap_temp.security_key ) == WWD_SUCCESS )
                        {
                            ap_temp.security_key_length = WSEC_MAX_PSK_LEN;
                            if ( WICED_SUCCESS != wiced_dct_write( &ap_temp, DCT_WIFI_CONFIG_SECTION, (uint32_t) ( OFFSETOF(platform_dct_wifi_config_t,stored_ap_list) + a * sizeof(wiced_config_ap_entry_t) ), sizeof(wiced_config_ap_entry_t) ) )
                            {
                                 WPRINT_WICED_INFO(("Failed to write ap list to DCT: \n"));
                            }

                        }
                    }
                }
#endif /* FIRMWARE_WITH_PMK_CALC_SUPPORT */
            }

            wiced_dct_read_unlock( (wiced_config_ap_entry_t*) ap, WICED_FALSE );

            if ( result == WICED_SUCCESS )
            {
                return result;
            }
        }
    }
exit:
    return result;
}

#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
wiced_result_t wiced_join_ap_specific( wiced_interface_t interface, wiced_ap_info_t* details, uint8_t security_key_length, const char security_key[ SECURITY_KEY_SIZE ] )
#else
wiced_result_t wiced_join_ap_specific( wiced_ap_info_t* details, uint8_t security_key_length, const char security_key[ SECURITY_KEY_SIZE ] )
#endif
{

    wiced_result_t      join_result = WICED_STA_JOIN_FAILED;
    wiced_scan_result_t temp_scan_result;
#ifndef WICED_USE_WIFI_TWO_STA_INTERFACE
    wiced_interface_t interface = WICED_STA_INTERFACE;
#endif

    char                ssid_name[SSID_NAME_SIZE + 1];
    memset(ssid_name, 0, sizeof(ssid_name));
    memcpy(ssid_name, details->SSID.value, details->SSID.length);
    WPRINT_WICED_INFO(("Joining : %s\n", ssid_name));

    memcpy( &temp_scan_result, details, sizeof( *details ) );

    /* Try join AP with last know specific details */
    if ( !( NULL_MAC(details->BSSID.octet) ) && details->channel != 0 )
    {
        join_result = (wiced_result_t) wwd_wifi_join_specific( &temp_scan_result, (uint8_t*) security_key, security_key_length, NULL, WICED_TO_WWD_INTERFACE(interface) );
    }

    if ( join_result != WICED_SUCCESS )
    {
        wiced_security_t security;

        security = details->security;
        if (details->bss_type == WICED_BSS_TYPE_ADHOC)
        {
            WPRINT_WICED_INFO(("%s: Network is ADHOC\n", __FUNCTION__));
            security |= IBSS_ENABLED;
        }

        /* If join-specific failed, try scan and join AP */
        join_result = (wiced_result_t) wwd_wifi_join( &details->SSID, security, (uint8_t*) security_key, security_key_length, NULL, WICED_TO_WWD_INTERFACE(interface) );
    }

    if ( join_result == WICED_SUCCESS )
    {
        WPRINT_WICED_INFO( ( "Successfully joined : %s\n", ssid_name) );

#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
        wiced_sta_link_up[interface]       = WICED_TRUE;
        wiced_sta_security_type[interface] = details->security;
#else
        wiced_sta_link_up       = WICED_TRUE;
        wiced_sta_security_type = details->security;
#endif

        wwd_management_set_event_handler( link_events, wiced_link_events_handler, NULL, WICED_TO_WWD_INTERFACE(interface) );
        return WICED_SUCCESS;
    }
    else
    {
        WPRINT_WICED_INFO(("Failed to join : %s\n", ssid_name));
    }
    return join_result;
}


wiced_result_t wiced_leave_ap( wiced_interface_t interface )
{
    /* Deregister the link event handler and leave the current AP */
    wwd_management_set_event_handler(link_events, NULL, 0, WICED_TO_WWD_INTERFACE( interface ) );
    wwd_wifi_leave( WICED_TO_WWD_INTERFACE( interface ) );
    if ( (interface == WICED_STA_INTERFACE )
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
        || (interface == WICED_AP_INTERFACE)
#endif
    )
    {
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
        wiced_sta_link_up[interface] = WICED_FALSE;
#else
        wiced_sta_link_up = WICED_FALSE;
#endif
    }
    return WICED_SUCCESS;
}

wiced_result_t wiced_wifi_scan_disable( void )
{
    wiced_result_t result = (wiced_result_t )wwd_wifi_abort_scan( );

    /* If there's a failure; something went wrong. Abort req without active scan looks like a no-op */
    if ( WICED_SUCCESS != result )
    {
        WPRINT_WICED_ERROR(("Failed to abort scan (none active?). Result=%d\n", result));
        return result;
    }

    /* suppression means firmware is prevented from doing scans even user requested */
    return (wiced_result_t)wwd_wifi_set_scan_suppress( WICED_TRUE );
}

wiced_result_t wiced_wifi_scan_enable( void )
{
    /* allow all scans again */
    return (wiced_result_t)wwd_wifi_set_scan_suppress( WICED_FALSE );
}

wiced_result_t wiced_wifi_scan_networks( wiced_scan_result_handler_t results_handler, void* user_data )
{
    return wiced_wifi_scan_networks_ex( results_handler, user_data, WICED_SCAN_TYPE_ACTIVE,  WICED_BSS_TYPE_ANY, NULL, NULL, NULL, NULL, WICED_STA_INTERFACE );
}

wiced_result_t wiced_wifi_scan_networks_ex ( wiced_scan_result_handler_t results_handler, void* user_data, wiced_scan_type_t scan_type,
        wiced_bss_type_t bss_type, const wiced_ssid_t* ssid, const wiced_mac_t* optional_mac, const uint16_t*  optional_channel_list,
        const wiced_scan_extended_params_t* optional_extended_params, wiced_interface_t interface )
{
    wiced_result_t result;

    wiced_assert("Bad args", results_handler != NULL);

    /* A requested scan will override the last disable */
    wiced_wifi_scan_enable( );

    result = wiced_rtos_get_semaphore( &scan_semaphore, SCAN_LONGEST_WAIT_TIME );
    if ( result != WICED_SUCCESS )
    {
        /* Return error result, but set the semaphore to work the next time */
        wiced_rtos_set_semaphore( &scan_semaphore );
        return result;
    }

    if ( !(scan_type & WICED_SCAN_TYPE_NO_BSSID_FILTER) )
    {
      current_bssid_list_length = 0;

      /* Prepare space to keep track of seen BSSIDs */
      if (scan_bssid_list != NULL)
      {
        free(scan_bssid_list);
      }
      scan_bssid_list = (wiced_mac_t*) malloc(SCAN_BSSID_LIST_LENGTH * sizeof(wiced_mac_t));
      if (scan_bssid_list == NULL)
      {
        goto exit;
      }
      memset(scan_bssid_list, 0, SCAN_BSSID_LIST_LENGTH * sizeof(wiced_mac_t));
    }

    /* Convert function pointer to object so it can be passed around */
    scan_handler.results_handler  = results_handler;
    scan_handler.user_data        = user_data;
    scan_handler.is_pno_scan      = ( scan_type == WICED_SCAN_TYPE_PNO ) ? WICED_TRUE : WICED_FALSE;

    /* Initiate scan */
    scan_result_ptr = MALLOC_OBJECT("scan result", wiced_scan_handler_result_t);
    if (scan_result_ptr == NULL)
    {
        goto error_with_bssid_list;
    }
    memset( scan_result_ptr, 0, sizeof( wiced_scan_handler_result_t ) );
    scan_result_ptr->status    = WICED_SCAN_INCOMPLETE;
    scan_result_ptr->user_data = user_data;

    if ( scan_type == WICED_SCAN_TYPE_PNO )
    {
        wiced_wifi_pno_ssid = malloc( sizeof( *wiced_wifi_pno_ssid ) );

        if ( wiced_wifi_pno_ssid == NULL )
        {
            goto error_with_result_ptr;

        }
        wiced_wifi_pno_ssid->length = ssid->length;
        memset( wiced_wifi_pno_ssid->value, 0, sizeof( wiced_wifi_pno_ssid->value ) );
        memcpy( wiced_wifi_pno_ssid->value, ssid->value, ssid->length );
    }
    WICED_POWER_LOGGER( EVENT_PROC_ID_WIFI, EVENT_ID_WIFI_DATA, EVENT_DESC_WIFI_SCAN_TIME );
    if ( wwd_wifi_scan( scan_type, bss_type, ssid, optional_mac, optional_channel_list, optional_extended_params, wiced_scan_result_handler, (wiced_scan_result_t**) &scan_result_ptr, &scan_handler, (wwd_interface_t) interface ) != WWD_SUCCESS )
    {
        WICED_POWER_LOGGER( EVENT_PROC_ID_WIFI, EVENT_ID_WIFI_DATA, EVENT_DESC_WIFI_IDLE );
        goto error_with_pno_ssid;
    }

    return WICED_SUCCESS;

error_with_pno_ssid:
    /* may be NULL if not pno scan */
    if ( wiced_wifi_pno_ssid != NULL )
    {
        free( wiced_wifi_pno_ssid );
        wiced_wifi_pno_ssid = NULL;
    }

error_with_result_ptr:
    free(scan_result_ptr);
    scan_result_ptr = NULL;

error_with_bssid_list:
    free(scan_bssid_list);
    scan_bssid_list = NULL;
exit:
    wiced_rtos_set_semaphore(&scan_semaphore);
    return WICED_ERROR;
}

wiced_result_t wiced_wifi_add_custom_ie( wiced_interface_t interface, const wiced_custom_ie_info_t* ie_info )
{
    wiced_assert("Bad args", ie_info != NULL);

    return (wiced_result_t) wwd_wifi_manage_custom_ie( WICED_TO_WWD_INTERFACE( interface ), WICED_ADD_CUSTOM_IE, (const uint8_t*)ie_info->oui, ie_info->subtype, (const void*)ie_info->data, ie_info->length, ie_info->which_packets );
}

wiced_result_t wiced_wifi_remove_custom_ie( wiced_interface_t interface, const wiced_custom_ie_info_t* ie_info )
{
    wiced_assert("Bad args", ie_info != NULL);

    return (wiced_result_t) wwd_wifi_manage_custom_ie( WICED_TO_WWD_INTERFACE( interface ), WICED_REMOVE_CUSTOM_IE, (const uint8_t*)ie_info->oui, ie_info->subtype, (const void*)ie_info->data, ie_info->length, ie_info->which_packets );
}
#ifndef WICED_AMAZON_FREERTOS_SDK
wiced_result_t wiced_wps_enrollee(wiced_wps_mode_t mode, const wiced_wps_device_detail_t* details, const char* password, wiced_wps_credential_t* credentials, uint16_t credential_count)
{
    wiced_result_t result    = WICED_SUCCESS;
    wps_agent_t*   workspace = (wps_agent_t*) calloc_named("wps", 1, sizeof(wps_agent_t));

    if ( workspace == NULL )
    {
        return WICED_OUT_OF_HEAP_SPACE;
    }

    besl_wps_init ( workspace, (besl_wps_device_detail_t*) details, WPS_ENROLLEE_AGENT, WWD_STA_INTERFACE );
    result = (wiced_result_t) besl_wps_start( workspace, (besl_wps_mode_t) mode, password, (besl_wps_credential_t*) credentials, credential_count );
    host_rtos_delay_milliseconds( 10 ); /* Delay required to allow WPS thread to run and initialize */
    if ( result == WICED_SUCCESS )
    {
        besl_wps_wait_till_complete( workspace );
        result = (wiced_result_t) besl_wps_get_result( workspace );
    }

    besl_wps_deinit( workspace );
    free( workspace );
    workspace = NULL;

    return result;
}

wiced_result_t wiced_wps_registrar(wiced_wps_mode_t mode, const wiced_wps_device_detail_t* details, const char* password, wiced_wps_credential_t* credentials, uint16_t credential_count)
{
    wiced_result_t result    = WICED_SUCCESS;
    wps_agent_t*   workspace = (wps_agent_t*) calloc_named("wps", 1, sizeof(wps_agent_t));

    if ( workspace == NULL )
    {
        return WICED_OUT_OF_HEAP_SPACE;
    }

    besl_wps_init ( workspace, (besl_wps_device_detail_t*) details, WPS_REGISTRAR_AGENT, WWD_AP_INTERFACE );
    result = (wiced_result_t) besl_wps_start( workspace, (besl_wps_mode_t) mode, password, (besl_wps_credential_t*) credentials, credential_count );
    if ( result == WICED_SUCCESS )
    {
        besl_wps_wait_till_complete( workspace );
        result = (wiced_result_t) besl_wps_get_result( workspace );
    }

    besl_wps_deinit( workspace );
    free( workspace );
    workspace = NULL;

    return result;
}
#endif
/******************************************************
 *                    Static Functions
 ******************************************************/

#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
static void link_up( wiced_interface_t interface )
#else
static void link_up( void )
#endif
{
    wiced_result_t res;
#ifndef WICED_USE_WIFI_TWO_STA_INTERFACE
    wiced_interface_t interface  = WICED_STA_INTERFACE;
#endif

#ifndef WICED_USE_WIFI_TWO_STA_INTERFACE
    if ( wiced_sta_link_up == WICED_FALSE )
#else
    if ( wiced_sta_link_up[interface] == WICED_FALSE )
#endif
    {
        wiced_network_notify_link_up( interface );
        res = wiced_rtos_send_asynchronous_event( WICED_NETWORKING_WORKER_THREAD, wiced_wireless_link_up_handler, 0 );
        if (res != WICED_SUCCESS)
        {
            WPRINT_NETWORK_DEBUG((" L%d : %s() : ERROR : Failed to handle link up event. Err = [%d]\r\n", __LINE__, __FUNCTION__, res));
            /* Fall through */
        }

#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
        wiced_sta_link_up[interface] = WICED_TRUE;
#else
        wiced_sta_link_up = WICED_TRUE;
#endif
    }
    else
    {
        res = wiced_rtos_send_asynchronous_event( WICED_NETWORKING_WORKER_THREAD, wiced_wireless_link_renew_handler, 0 );
        if (res != WICED_SUCCESS)
        {
            WPRINT_NETWORK_DEBUG((" L%d : %s() : ERROR : Failed to handle link renew event. Err = [%d]\r\n", __LINE__, __FUNCTION__, res));
            /* Fall through */
        }
    }
}

#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
static void link_down( wiced_interface_t interface )
#else
static void link_down( void )
#endif
{
#ifndef WICED_USE_WIFI_TWO_STA_INTERFACE
    wiced_interface_t interface = WICED_STA_INTERFACE;
    if ( wiced_sta_link_up == WICED_TRUE )
#else
    if ( wiced_sta_link_up[interface] == WICED_TRUE )
#endif
    {
        wiced_result_t res;

        /* Notify network stack that the link is down. Further processing will be done in the link down handler */
        wiced_network_notify_link_down( interface );

        /* Force awake the networking thread. It might still be blocked on receive or send timeouts */
        wiced_rtos_thread_force_awake( &(WICED_NETWORKING_WORKER_THREAD->thread) );

        res = wiced_rtos_send_asynchronous_event( WICED_NETWORKING_WORKER_THREAD, wiced_wireless_link_down_handler, 0 );
        if (res != WICED_SUCCESS)
        {
            WPRINT_NETWORK_DEBUG((" L%d : %s() : ERROR : Failed to handle link down event. Err = [%d]\r\n", __LINE__, __FUNCTION__, res));
            /* Fall through */
        }

#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
        wiced_sta_link_up[interface] = WICED_FALSE;
#else
        wiced_sta_link_up = WICED_FALSE;
#endif
    }
}

static wiced_result_t handshake_error_callback( void* arg )
{
    wiced_result_t          res = WICED_SUCCESS;
    unsigned int            a;
    int                     retries;
    wiced_config_ap_entry_t *ap;

    UNUSED_PARAMETER( arg );

    wiced_rtos_deregister_timed_event( &wiced_sta_join_retry_timer);

    /* Explicitly leave AP and then rejoin */
    wiced_leave_ap( WICED_STA_INTERFACE );

    for ( retries = WICED_JOIN_RETRY_ATTEMPTS; retries != 0; --retries )
    {
        /* Try all stored APs */
        for ( a = 0; a < CONFIG_AP_LIST_SIZE; ++a )
        {
            /* Check if the entry is valid */
            CHECK_RETURN( wiced_dct_read_lock( (void**) &ap, WICED_FALSE, DCT_WIFI_CONFIG_SECTION, (uint32_t) ( OFFSETOF(platform_dct_wifi_config_t,stored_ap_list) + a * sizeof(wiced_config_ap_entry_t) ), sizeof(wiced_config_ap_entry_t) ) );
            if ( ap->details.SSID.length != 0 )
            {
                wiced_result_t      join_result = WICED_ERROR;
                wiced_scan_result_t temp_scan_result;

                memcpy( &temp_scan_result, &ap->details, sizeof( ap->details ) );

                /* Try join AP with last know specific details */
                if ( !( NULL_MAC(ap->details.BSSID.octet) ) && ap->details.channel != 0 )
                {
                    join_result = (wiced_result_t) wwd_wifi_join_specific( &temp_scan_result, (uint8_t*) ap->security_key, ap->security_key_length, NULL, WWD_STA_INTERFACE );
                }

                if ( join_result != WICED_SUCCESS )
                {
                    /* If join-specific failed, try scan and join AP */
                    join_result = (wiced_result_t) wwd_wifi_join( &ap->details.SSID, ap->details.security, (uint8_t*) ap->security_key, ap->security_key_length, NULL, WWD_STA_INTERFACE );
                }

                if ( join_result == WICED_SUCCESS )
                {
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
                    link_up(WICED_STA_INTERFACE); /* XXX XXX Hardcoded this interface for now */
                    wiced_sta_security_type[WICED_STA_INTERFACE] = ap->details.security;
#else
                    link_up();
                    wiced_sta_security_type = ap->details.security;
#endif

                    wwd_management_set_event_handler( link_events, wiced_link_events_handler, NULL, WWD_STA_INTERFACE );

#ifdef FIRMWARE_WITH_PMK_CALC_SUPPORT
                    /* Extract the calculated PMK and store it in the DCT to speed up future associations */
                    if ( ( ( ( ap->details.security & WPA_SECURITY ) || ( ap->details.security & WPA2_SECURITY ) ) &&  ( ( ap->details.security & ENTERPRISE_ENABLED ) == 0 ) ) &&
                            ( ap->security_key_length != WSEC_MAX_PSK_LEN ) && ( ( ap->details.security & IBSS_ENABLED ) == 0 ) )
                    {
                        wiced_config_ap_entry_t ap_temp;
                        memcpy( &ap_temp, ap, sizeof(wiced_config_ap_entry_t) );
                        if ( wwd_wifi_get_pmk( ap_temp.security_key, ap_temp.security_key_length, ap_temp.security_key ) == WWD_SUCCESS )
                        {
                            ap_temp.security_key_length = WSEC_MAX_PSK_LEN;
                            if ( WICED_SUCCESS != wiced_dct_write( &ap_temp, DCT_WIFI_CONFIG_SECTION, (uint32_t) ( OFFSETOF(platform_dct_wifi_config_t,stored_ap_list) + a * sizeof(wiced_config_ap_entry_t) ), sizeof(wiced_config_ap_entry_t) ) )
                            {
                                 WPRINT_WICED_INFO(("Failed to write ap list to DCT: \n"));
                            }

                        }
                    }
#endif /* FIRMWARE_WITH_PMK_CALC_SUPPORT */

                    wiced_dct_read_unlock( (wiced_config_ap_entry_t*) ap, WICED_FALSE );

                    /* Reset retry-backoff-timeout index */
                    retry_backoff_timeout = DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_SEC;
                    return WICED_SUCCESS;
                }
            }
            wiced_dct_read_unlock( (wiced_config_ap_entry_t*) ap, WICED_FALSE );
        }
    }

    /* Register retry with network worker thread */
    WPRINT_NETWORK_DEBUG((" L%d : %s() : Retrying to join with back-off [%d secs]\r\n", __LINE__, __FUNCTION__, retry_backoff_timeout));
    res = wiced_rtos_register_timed_event( &wiced_sta_join_retry_timer, WICED_NETWORKING_WORKER_THREAD, handshake_error_callback, (uint32_t)retry_backoff_timeout * 1000, 0 );
    if (res != WICED_SUCCESS)
    {
        WPRINT_NETWORK_DEBUG((" L%d : %s() : ERROR : Failed in join retry with back-off. Err = [%d]\r\n", __LINE__, __FUNCTION__, res));
    }
    /* Update backoff timeout */
    retry_backoff_timeout = (retry_backoff_timeout < MAX_RETRY_BACKOFF_TIMEOUT_IN_SEC)? (uint8_t)(retry_backoff_timeout << 1) : MAX_RETRY_BACKOFF_TIMEOUT_IN_SEC;

    return WICED_STA_JOIN_FAILED;
}

static void handshake_timeout_handler( void* arg )
{
    wiced_result_t result;
    UNUSED_PARAMETER( arg );

    wiced_rtos_stop_timer( &wiced_sta_handshake_timer );
    wiced_rtos_deregister_timed_event( &wiced_sta_join_retry_timer );
    retry_backoff_timeout = DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_SEC;

    /* This RTOS timer callback runs in interrupt context. Defer event management to WICED_NETWORKING_WORKER_THREAD */
    result = wiced_rtos_send_asynchronous_event( WICED_NETWORKING_WORKER_THREAD, handshake_error_callback, 0 );
    if (result != WICED_SUCCESS)
    {
        WPRINT_WICED_ERROR((" L%d : %s() : Failed to send async event to n/w worker thread. Err = [%d]\r\n", __LINE__, __FUNCTION__, result));
    }
}

static void* wiced_link_events_handler( const wwd_event_header_t* event_header, const uint8_t* event_data, void* handler_user_data )
{
    UNUSED_PARAMETER( event_data );
#ifndef WICED_AMAZON_FREERTOS_SDK
    WPRINT_MACRO( ("Link event (type, status, reason, flags) %u %u %u %u\n", (unsigned int)event_header->event_type, (unsigned int)event_header->status,
            (unsigned int)event_header->reason, (unsigned int)event_header->flags )) ;
#endif
    if ( (event_header->interface != (uint8_t) WICED_STA_INTERFACE )
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
        && (event_header->interface != (uint8_t) WICED_AP_INTERFACE)
#endif
        )
    {
        return handler_user_data;
    }

    switch ( event_header->event_type )
    {
        case WLC_E_LINK:
            if ( ( event_header->flags & WLC_EVENT_MSG_LINK ) != 0 )
            {
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
                switch ( wiced_sta_security_type[event_header->interface] )
#else
                switch ( wiced_sta_security_type )
#endif
                {
                    case WICED_SECURITY_OPEN:
                    case WICED_SECURITY_IBSS_OPEN:
                    case WICED_SECURITY_WPS_OPEN:
                    case WICED_SECURITY_WPS_SECURE:
                    case WICED_SECURITY_WEP_PSK:
                    case WICED_SECURITY_WEP_SHARED:
                    {
                        /* Advertise link-up immediately as no EAPOL is required */
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
                        link_up(event_header->interface);
#else
                        link_up();
#endif
                        break;
                    }

                    case WICED_SECURITY_WPA_TKIP_PSK:
                    case WICED_SECURITY_WPA_AES_PSK:
                    case WICED_SECURITY_WPA_MIXED_PSK:
                    case WICED_SECURITY_WPA2_AES_PSK:
                    case WICED_SECURITY_WPA2_TKIP_PSK:
                    case WICED_SECURITY_WPA2_MIXED_PSK:
                    case WICED_SECURITY_WPA_TKIP_ENT:
                    case WICED_SECURITY_WPA_AES_ENT:
                    case WICED_SECURITY_WPA_MIXED_ENT:
                    case WICED_SECURITY_WPA2_TKIP_ENT:
                    case WICED_SECURITY_WPA2_AES_ENT:
                    case WICED_SECURITY_WPA2_MIXED_ENT:
                    case WICED_SECURITY_WPA2_FBT_PSK:
                    case WICED_SECURITY_WPA2_FBT_ENT:
                    case WICED_SECURITY_WPA3_SAE:
                    case WICED_SECURITY_WPA3_WPA2_PSK:
                    {
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
                        link_up_event_received[event_header->interface] = WICED_TRUE;
#else
                        link_up_event_received = WICED_TRUE;
#endif
                        /* Start a timer and wait for WLC_E_PSK_SUP event */
                        wiced_rtos_start_timer( &wiced_sta_handshake_timer );
                        break;
                    }

                    case WICED_SECURITY_UNKNOWN:
                    case WICED_SECURITY_FORCE_32_BIT:
                    default:
                    {
                        wiced_assert( "Bad Security type\r\n", 0 != 0 );
                        break;
                    }
                }
            }
            else
            {
                /* WPA handshake is aborted because of link down. Stop handshake timer. */
                wiced_rtos_stop_timer( &wiced_sta_handshake_timer );
                wiced_rtos_deregister_timed_event( &wiced_sta_join_retry_timer );
                retry_backoff_timeout = DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_SEC;

#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
                link_down( event_header->interface );
#else
                link_down( );
#endif
            }
            break;

        case WLC_E_DEAUTH_IND:
        case WLC_E_DISASSOC_IND:
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
            link_down( event_header->interface );
#else
            link_down( );
#endif
            break;

        case WLC_E_PSK_SUP:
            {
                /* WLC_E_PSK_SUP is received. Check for status and reason. */
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
                if ( event_header->status == WLC_SUP_KEYED && event_header->reason == WLC_E_SUP_OTHER && link_up_event_received[event_header->interface] == WICED_TRUE )
                {
                    /* Successful WPA key exchange. Stop timer and announce link up event to application */
                    wiced_rtos_stop_timer( &wiced_sta_handshake_timer );
                    wiced_rtos_deregister_timed_event( &wiced_sta_join_retry_timer );
                    retry_backoff_timeout = DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_SEC;
                    link_up(event_header->interface);
                    link_up_event_received[event_header->interface] = WICED_FALSE;
                }
#else
                if ( event_header->status == WLC_SUP_KEYED && event_header->reason == WLC_E_SUP_OTHER && link_up_event_received == WICED_TRUE )
                {
                    /* Successful WPA key exchange. Stop timer and announce link up event to application */
                    wiced_rtos_stop_timer( &wiced_sta_handshake_timer );
                    wiced_rtos_deregister_timed_event( &wiced_sta_join_retry_timer );
                    retry_backoff_timeout = DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_SEC;
                    link_up();
                    link_up_event_received = WICED_FALSE;
                }
#endif
                break;
            }

        /* Note - These are listed to keep gcc pedantic checking happy */
        case WLC_E_NONE:
        case WLC_E_ROAM:
        case WLC_E_SET_SSID:
        case WLC_E_DEAUTH:
        case WLC_E_DISASSOC:
        case WLC_E_JOIN:
        case WLC_E_START:
        case WLC_E_AUTH:
        case WLC_E_AUTH_IND:
        case WLC_E_ASSOC:
        case WLC_E_ASSOC_IND:
        case WLC_E_REASSOC:
        case WLC_E_REASSOC_IND:
        case WLC_E_QUIET_START:
        case WLC_E_QUIET_END:
        case WLC_E_BEACON_RX:
        case WLC_E_MIC_ERROR:
        case WLC_E_NDIS_LINK:
        case WLC_E_TXFAIL:
        case WLC_E_PMKID_CACHE:
        case WLC_E_RETROGRADE_TSF:
        case WLC_E_PRUNE:
        case WLC_E_AUTOAUTH:
        case WLC_E_EAPOL_MSG:
        case WLC_E_SCAN_COMPLETE:
        case WLC_E_ADDTS_IND:
        case WLC_E_DELTS_IND:
        case WLC_E_BCNSENT_IND:
        case WLC_E_BCNRX_MSG:
        case WLC_E_BCNLOST_MSG:
        case WLC_E_ROAM_PREP:
        case WLC_E_PFN_NET_FOUND:
        case WLC_E_PFN_NET_LOST:
        case WLC_E_RESET_COMPLETE:
        case WLC_E_JOIN_START:
        case WLC_E_ROAM_START:
        case WLC_E_ASSOC_START:
        case WLC_E_IBSS_ASSOC:
        case WLC_E_RADIO:
        case WLC_E_PSM_WATCHDOG:
        case WLC_E_CCX_ASSOC_START:
        case WLC_E_CCX_ASSOC_ABORT:
        case WLC_E_PROBREQ_MSG:
        case WLC_E_SCAN_CONFIRM_IND:
        case WLC_E_COUNTRY_CODE_CHANGED:
        case WLC_E_EXCEEDED_MEDIUM_TIME:
        case WLC_E_ICV_ERROR:
        case WLC_E_UNICAST_DECODE_ERROR:
        case WLC_E_MULTICAST_DECODE_ERROR:
        case WLC_E_TRACE:
        case WLC_E_BTA_HCI_EVENT:
        case WLC_E_IF:
        case WLC_E_P2P_DISC_LISTEN_COMPLETE:
        case WLC_E_RSSI:
        case WLC_E_PFN_SCAN_COMPLETE:
        case WLC_E_EXTLOG_MSG:
        case WLC_E_ACTION_FRAME:
        case WLC_E_ACTION_FRAME_COMPLETE:
        case WLC_E_PRE_ASSOC_IND:
        case WLC_E_PRE_REASSOC_IND:
        case WLC_E_CHANNEL_ADOPTED:
        case WLC_E_AP_STARTED:
        case WLC_E_DFS_AP_STOP:
        case WLC_E_DFS_AP_RESUME:
        case WLC_E_WAI_STA_EVENT:
        case WLC_E_WAI_MSG:
        case WLC_E_ESCAN_RESULT:
        case WLC_E_ACTION_FRAME_OFF_CHAN_COMPLETE:
        case WLC_E_PROBRESP_MSG:
        case WLC_E_P2P_PROBREQ_MSG:
        case WLC_E_DCS_REQUEST:
        case WLC_E_FIFO_CREDIT_MAP:
        case WLC_E_ACTION_FRAME_RX:
        case WLC_E_WAKE_EVENT:
        case WLC_E_RM_COMPLETE:
        case WLC_E_HTSFSYNC:
        case WLC_E_OVERLAY_REQ:
        case WLC_E_CSA_COMPLETE_IND:
        case WLC_E_EXCESS_PM_WAKE_EVENT:
        case WLC_E_PFN_SCAN_NONE:
        case WLC_E_PFN_SCAN_ALLGONE:
        case WLC_E_GTK_PLUMBED:
        case WLC_E_ASSOC_IND_NDIS:
        case WLC_E_REASSOC_IND_NDIS:
        case WLC_E_ASSOC_REQ_IE:
        case WLC_E_ASSOC_RESP_IE:
        case WLC_E_ASSOC_RECREATED:
        case WLC_E_ACTION_FRAME_RX_NDIS:
        case WLC_E_AUTH_REQ:
        case WLC_E_TDLS_PEER_EVENT:
        case WLC_E_SPEEDY_RECREATE_FAIL:
        case WLC_E_NATIVE:
        case WLC_E_PKTDELAY_IND:
        case WLC_E_AWDL_AW:
        case WLC_E_AWDL_ROLE:
        case WLC_E_AWDL_EVENT:
        case WLC_E_NIC_AF_TXS:
        case WLC_E_NAN:
        case WLC_E_BEACON_FRAME_RX:
        case WLC_E_SERVICE_FOUND:
        case WLC_E_GAS_FRAGMENT_RX:
        case WLC_E_GAS_COMPLETE:
        case WLC_E_P2PO_ADD_DEVICE:
        case WLC_E_P2PO_DEL_DEVICE:
        case WLC_E_WNM_STA_SLEEP:
        case WLC_E_TXFAIL_THRESH:
        case WLC_E_PROXD:
        case WLC_E_IBSS_COALESCE:
        case WLC_E_AWDL_RX_PRB_RESP:
        case WLC_E_AWDL_RX_ACT_FRAME:
        case WLC_E_AWDL_WOWL_NULLPKT:
        case WLC_E_AWDL_PHYCAL_STATUS:
        case WLC_E_AWDL_OOB_AF_STATUS:
        case WLC_E_AWDL_SCAN_STATUS:
        case WLC_E_AWDL_AW_START:
        case WLC_E_AWDL_AW_END:
        case WLC_E_AWDL_AW_EXT:
        case WLC_E_AWDL_PEER_CACHE_CONTROL:
        case WLC_E_CSA_START_IND:
        case WLC_E_CSA_DONE_IND:
        case WLC_E_CSA_FAILURE_IND:
        case WLC_E_CCA_CHAN_QUAL:
        case WLC_E_BSSID:
        case WLC_E_TX_STAT_ERROR:
        case WLC_E_BCMC_CREDIT_SUPPORT:
        case WLC_E_PSTA_PRIMARY_INTF_IND:
        case WLC_E_RRM:
        case WLC_E_ULP:
        case WLC_E_LAST:
        case WLC_E_FORCE_32_BIT:
        case WLC_E_BT_WIFI_HANDOVER_REQ:
        case WLC_E_PFN_BEST_BATCHING:
        case WLC_E_SPW_TXINHIBIT:
        case WLC_E_FBT_AUTH_REQ_IND:
        case WLC_E_RSSI_LQM:
        case WLC_E_PFN_GSCAN_FULL_RESULT:
        case WLC_E_PFN_SWC:
        case WLC_E_AUTHORIZED:
        case WLC_E_PROBREQ_MSG_RX:
        case WLC_E_RMC_EVENT:
        case WLC_E_DPSTA_INTF_IND:
        default:
            wiced_assert( "Received event which was not registered\n", 0 != 0 );
            break;
    }
    return handler_user_data;
}

static void wiced_wifi_scan_cleanup( void )
{
    if ( scan_bssid_list != NULL )
    {
        malloc_transfer_to_curr_thread(scan_bssid_list);
        free(scan_bssid_list);
    }

    if ( wiced_wifi_pno_ssid != NULL )
    {
        malloc_transfer_to_curr_thread( wiced_wifi_pno_ssid );
        free( wiced_wifi_pno_ssid );
    }

    scan_bssid_list     = NULL;
    wiced_wifi_pno_ssid = NULL;
    wiced_rtos_set_semaphore(&scan_semaphore);
}

/* we're at the end of the scan job, report up the off channel results and the final result (finished or aborted) */
static wiced_result_t wiced_scan_finish_handler( wiced_scan_handler_result_t* malloced_scan_result )
{
    /* report last result to app */
    scan_handler.results_handler( malloced_scan_result );

    /* clean up after the scan */
    wiced_wifi_scan_cleanup( );

    WICED_POWER_LOGGER( EVENT_PROC_ID_WIFI, EVENT_ID_WIFI_DATA, EVENT_DESC_WIFI_IDLE );
    return WICED_SUCCESS;
}

static void wiced_scan_result_handler( wiced_scan_result_t** result_ptr, void* user_data, wiced_scan_status_t status )
{
    wiced_result_t               result;
    wiced_scan_handler_result_t* current_result;
    wiced_mac_t*                 mac_iter     = NULL;
    internal_scan_handler_t*     int_scan_handler = (internal_scan_handler_t*) user_data;

    /* Check if we don't have a scan result to send to the user */
    if ( scan_result_ptr == NULL )
    {
        return;
    }

    current_result = (wiced_scan_handler_result_t*)(*result_ptr);

    /* check and filter pno scan results, to ensure only desired ssids are passed back */
    if ( int_scan_handler->is_pno_scan == WICED_TRUE )
    {
        /* if the result don't match our one and only ssid, then bail out and ignore */
        if ( wiced_wifi_pno_ssid == NULL || wiced_wifi_pno_ssid->length != current_result->ap_details.SSID.length || memcmp(current_result->ap_details.SSID.value, wiced_wifi_pno_ssid->value, wiced_wifi_pno_ssid->length) != 0 )
        {
            return;
        }
        else
        {
            /* fill out the legit security, saved from ealier as FW doesn't supply currently */
            current_result->ap_details.security = wiced_wifi_pno_security;
        }
    }

    /* Check if scan is complete */
    if ( status == WICED_SCAN_COMPLETED_SUCCESSFULLY || status == WICED_SCAN_ABORTED )
    {

        /* Indicate to the scan handler that scanning is complete */
        scan_result_ptr->status = status;

        /* send out a final event, for further processing */
        if ( (result = wiced_rtos_send_asynchronous_event( WICED_NETWORKING_WORKER_THREAD, (event_handler_t)wiced_scan_finish_handler, (void*) ( scan_result_ptr ) )) != WICED_SUCCESS )
        {
            /* Unable to Queue the message so call scan results
                       * handler directly with the scan status.
                       */
            wiced_scan_finish_handler( scan_result_ptr );
        }
        return;
    }

    if ( scan_bssid_list != NULL )
    {
       /* Skip BSSID filtering of repeated beacons and/or probe responses for a BSSID if disabled (no BSSID array provided) */
       for ( mac_iter = scan_bssid_list; ( mac_iter < scan_bssid_list + current_bssid_list_length ); ++mac_iter )
       {
           /* Check for matching MAC address */
           if ( CMP_MAC( mac_iter->octet, current_result->ap_details.BSSID.octet ) )
           {
              /* Ignore this result. Clean up the result and let it be reused */
              memset( *result_ptr, 0, sizeof(wiced_scan_result_t) );
              return;
           }
        }
     }

    /* Add it to the BSSID list */
    if ( ( current_bssid_list_length < SCAN_BSSID_LIST_LENGTH ) && ( mac_iter != NULL ) )
    {
         memcpy( &mac_iter->octet, current_result->ap_details.BSSID.octet, sizeof(wiced_mac_t) );
         current_bssid_list_length++;
    }

    /* Post event in worker thread */
    if ( (result = wiced_rtos_send_asynchronous_event( WICED_NETWORKING_WORKER_THREAD, (event_handler_t) int_scan_handler->results_handler, (void*) ( scan_result_ptr ) )) != WICED_SUCCESS )
    {
        free( *result_ptr );
        *result_ptr = NULL;
    }


    /* Allocate new storage space for next scan result */
    scan_result_ptr = MALLOC_OBJECT("scan result", wiced_scan_handler_result_t);
    if (scan_result_ptr != NULL)
    {
        memset( scan_result_ptr, 0, sizeof(wiced_scan_handler_result_t));
        scan_result_ptr->status    = WICED_SCAN_INCOMPLETE;
        scan_result_ptr->user_data = int_scan_handler->user_data;
    }
    *result_ptr = (wiced_scan_result_t*)scan_result_ptr;
}

static void* softap_event_handler( const wwd_event_header_t* event_header, const uint8_t* event_data, /*@returned@*/ void* handler_user_data )
{
    wiced_wifi_softap_event_t event = WICED_AP_UNKNOWN_EVENT;

    UNUSED_PARAMETER( handler_user_data );
    UNUSED_PARAMETER( event_data );

    if ( ap_event_handler != NULL )
    {
        if ( event_header->interface == WWD_AP_INTERFACE )
        {
            if ( ( event_header->event_type == WLC_E_DEAUTH ) ||
                 ( event_header->event_type == WLC_E_DEAUTH_IND ) ||
                 ( event_header->event_type == WLC_E_DISASSOC ) ||
                 ( event_header->event_type == WLC_E_DISASSOC_IND ) )
            {
                event = WICED_AP_STA_LEAVE_EVENT;
            }
            else if ( event_header->event_type == WLC_E_ASSOC_IND || event_header->event_type == WLC_E_REASSOC_IND )
            {
                event = WICED_AP_STA_JOINED_EVENT;
            }

            ap_event_handler( event, &event_header->addr );
        }
    }

    return NULL;
}

wiced_result_t wiced_wifi_find_ap( const char* ssid, wiced_scan_result_t* ap_info, const uint16_t* optional_channel_list)
{
    wiced_result_t  result;
    wiced_ssid_t    ssid_info;
    wiced_semaphore_t semaphore;

    wiced_rtos_init_semaphore( &semaphore );

    /* Validate input arguments */
    wiced_assert( "Bad args", ((ssid != NULL) && (ap_info != NULL) && (strlen(ssid) < sizeof(ssid_info.value))) );

    memset( ap_info, 0, sizeof( *ap_info ) );

    /* Populate SSID information */
    memset( &ssid_info, 0, sizeof( ssid_info ) );
    ssid_info.length = (uint8_t) strlen( ssid );
    memcpy( ssid_info.value, ssid, ssid_info.length );

    /* Initiate the scan */
    result = ( wiced_result_t ) wwd_wifi_scan( WICED_SCAN_TYPE_ACTIVE, WICED_BSS_TYPE_ANY, &ssid_info, NULL, optional_channel_list, NULL, find_ap_scan_handler, (wiced_scan_result_t**) &ap_info, &semaphore, WWD_STA_INTERFACE );
    if ( result != WICED_SUCCESS )
    {
        goto exit;
    }

    result = wiced_rtos_get_semaphore( &semaphore, SCAN_LONGEST_WAIT_TIME );

    if ( ap_info->channel == 0 )
    {
        result = WICED_WWD_ACCESS_POINT_NOT_FOUND;
    }

exit:
    wiced_rtos_deinit_semaphore( &semaphore );
    memset(&semaphore, 0, sizeof(semaphore));

    return result;
}

wiced_result_t wiced_wifi_pno_start( wiced_ssid_t *ssid, wiced_security_t security, wiced_scan_result_handler_t handler, void *user_data )
{
    wiced_result_t  result;

    /* add the network */
    CHECK_RETURN( wwd_wifi_pno_add_network( ssid, security ) );
    wiced_wifi_pno_security = security;

    /* Initiate the scan */
    result =  wiced_wifi_scan_networks_ex(handler, user_data, WICED_SCAN_TYPE_PNO, WICED_BSS_TYPE_ANY,
            ssid, NULL, NULL, NULL, WICED_STA_INTERFACE);

    return result;
}

wiced_result_t wiced_wifi_pno_stop( void )
{
    /* bring pno down and clear out any networks */
    CHECK_RETURN( wwd_wifi_pno_clear( ) );

    /* clean up any memory which may outstanding */
    wiced_wifi_scan_cleanup( );

    return WICED_SUCCESS;
}

static void find_ap_scan_handler( wiced_scan_result_t** result_ptr, void* user_data, wiced_scan_status_t status )
{
    wiced_semaphore_t* semaphore = (wiced_semaphore_t*)user_data;
    UNUSED_PARAMETER( result_ptr );

    /* Check if scan is complete */
    if ( status == WICED_SCAN_COMPLETED_SUCCESSFULLY || status == WICED_SCAN_ABORTED )
    {
        wiced_rtos_set_semaphore( semaphore );
        return;
    }
}

/* This is callback function to handle the events
 * @param1: wwd_event_header_t
 * @param2: event_data
 * @param3: void* (user_data: semaphore)
 */
void* wiced_rrm_report_handler( const wwd_event_header_t* event_header, const uint8_t* event_data, /*@returned@*/ void* handler_user_data )
{

    wiced_semaphore_t* semaphore = (wiced_semaphore_t*)handler_user_data;

   if (rrm_event_handler != NULL ) {
         rrm_event_handler(event_header, event_data );
    }

    wiced_rtos_set_semaphore( semaphore );
    return handler_user_data;
}

/* This is callback function to handle the events
 * @param1: wwd_event_header_t
 * @param2: event_data
 * @param3: void* (user_data: semaphore)
 */
void* wiced_nan_scan_handler( const wwd_event_header_t* event_header, const uint8_t* event_data, /*@returned@*/ void* handler_user_data )
{

    wiced_semaphore_t* semaphore = (wiced_semaphore_t*)handler_user_data;

   if (nan_event_handler != NULL )
   {
       nan_event_handler(event_header, event_data );
   }

    wiced_rtos_set_semaphore( semaphore );
    return handler_user_data;
}

/* This function send Radio Resource Request by calling WWD API to send the request
 * maintains the semaphore so that only one call is sent and response is received in the
 * callback function
 * @param1: wwd_rrm_report_type_t
 * @param2: wwd_event_handler_t
 * @param3: radio resource management request structure
 * @param4: wwd_rrm_report_t**
 * @param5: wwd_interface_t
 * @return: wwd_result_t
 */
wwd_result_t wiced_wifi_rrm_request(  wwd_rrm_report_type_t                      report_type,
                                      wwd_event_handler_t                        callback,
                                      void*                                      rrm_req,
                                      wwd_rrm_report_t**                         report_ptr,
                                      wwd_interface_t                            interface
                                     )
{

    wwd_result_t result = WWD_SUCCESS;
    wiced_semaphore_t rrm_req_complete_semaphore;

    wiced_rtos_init_semaphore( &rrm_req_complete_semaphore );

    wwd_wifi_rrm_request( report_type, callback,  rrm_req, report_ptr, &rrm_req_complete_semaphore, interface);

    wiced_rtos_get_semaphore(&rrm_req_complete_semaphore, RRM_TIMEOUT);

    wiced_rtos_deinit_semaphore( &rrm_req_complete_semaphore );
    memset(&rrm_req_complete_semaphore, 0, sizeof(rrm_req_complete_semaphore));

    return result;
}

wiced_result_t wiced_wifi_start_ap_with_custom_ie( wiced_ssid_t* ssid, wiced_security_t security, const char* key, uint8_t channel, const wiced_custom_ie_info_t* ie)
{
    CHECK_RETURN( wwd_wifi_ap_init( ssid, security, (const uint8_t*)key, (uint8_t)strlen(key), channel ) );
    CHECK_RETURN(wwd_wifi_manage_custom_ie(WICED_TO_WWD_INTERFACE(WICED_AP_INTERFACE), WICED_ADD_CUSTOM_IE,
                                           ie->oui, ie->subtype, ie->data, ie->length, ie->which_packets));
    CHECK_RETURN( wwd_wifi_ap_up() );

    return WICED_SUCCESS;
}

wiced_bool_t wiced_wifi_is_sta_link_up( void )
{
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
    return wiced_sta_link_up[WWD_STA_INTERFACE];
#else
    return wiced_sta_link_up;
#endif
}



/******************************************************
 *
 *           Wiced Wifi-BT Function Implementations
 *
 *****************************************************/
wiced_result_t wiced_wifi_set_gci_mask ( uint32_t gci_mask )
{
    return (wiced_result_t) wwd_wifi_set_iovar_value( IOVAR_STR_BWTE_BWTE_GCI_MASK, gci_mask, WICED_STA_INTERFACE );
}

wiced_result_t wiced_wifi_get_gci_mask ( uint32_t* gci_mask )
{
    return (wiced_result_t) wwd_wifi_get_iovar_value( IOVAR_STR_BWTE_BWTE_GCI_MASK, gci_mask, WICED_STA_INTERFACE );
}


wiced_result_t wiced_wifi_send_gci_mailbox_message ( uint32_t data )
{
    return (wiced_result_t) wwd_wifi_set_iovar_value( IOVAR_STR_BWTE_GCI_SENDMSG, data, WICED_STA_INTERFACE );
}

/******************************************************
 *           Wiced WiFi Function Implementations
 *
 *
 ******************************************************/
wiced_result_t wiced_wifi_set_roam_trigger( int32_t trigger_level )
{
    return (wiced_result_t) wwd_wifi_set_roam_trigger( trigger_level );
}

wiced_result_t wiced_wifi_set_roam_trigger_per_band( int32_t trigger_level, wiced_802_11_band_t band )
{
    return (wiced_result_t) wwd_wifi_set_roam_trigger_per_band( trigger_level, band );
}

wiced_result_t wiced_wifi_get_roam_trigger( int32_t* trigger_level )
{
    return (wiced_result_t) wwd_wifi_get_roam_trigger( trigger_level );
}

wiced_result_t wiced_wifi_get_roam_trigger_per_band( int32_t* trigger_level, wiced_802_11_band_t band )
{
    return (wiced_result_t) wwd_wifi_get_roam_trigger_per_band( trigger_level, band );
}

wiced_result_t wiced_wifi_get_channel( uint32_t* channel )
{
    return (wiced_result_t) wwd_wifi_get_channel(WWD_STA_INTERFACE, channel);
}

wiced_result_t wiced_wifi_get_mac_address( wiced_mac_t* mac )
{
    return (wiced_result_t) wwd_wifi_get_mac_address( mac, WWD_STA_INTERFACE );
}

wiced_result_t wiced_wifi_get_counters(wwd_interface_t interface, wiced_counters_t* counters )
{
    return (wiced_result_t) wwd_wifi_get_counters( interface, counters );
}

wiced_result_t wiced_wifi_set_listen_interval( uint8_t listen_interval, wiced_listen_interval_time_unit_t time_unit )
{
    return (wiced_result_t) wwd_wifi_set_listen_interval( listen_interval, time_unit );
}

wiced_result_t wiced_wifi_set_listen_interval_assoc( uint16_t listen_interval )
{
    return (wiced_result_t) wwd_wifi_set_listen_interval_assoc( listen_interval );
}

wiced_result_t wiced_wifi_get_listen_interval( wiced_listen_interval_t* li )
{
    return (wiced_result_t) wwd_wifi_get_listen_interval( li );
}

wiced_result_t wiced_wifi_enable_powersave( void )
{
    return (wiced_result_t) wwd_wifi_enable_powersave_interface( WWD_STA_INTERFACE );
}

wiced_result_t wiced_wifi_enable_powersave_interface( wiced_interface_t interface )
{
    return (wiced_result_t) wwd_wifi_enable_powersave_interface( (wwd_interface_t)interface );
}

wiced_result_t wiced_wifi_enable_powersave_with_throughput( uint16_t return_to_sleep_delay_ms )
{
    return (wiced_result_t) wwd_wifi_enable_powersave_with_throughput_interface( return_to_sleep_delay_ms, WWD_STA_INTERFACE );
}

wiced_result_t wiced_wifi_enable_powersave_with_throughput_interface( uint16_t return_to_sleep_delay_ms, wiced_interface_t interface )
{
    return (wiced_result_t) wwd_wifi_enable_powersave_with_throughput_interface( return_to_sleep_delay_ms, (wwd_interface_t)interface );
}

wiced_result_t wiced_wifi_disable_powersave( void )
{
    return (wiced_result_t) wwd_wifi_disable_powersave_interface( WWD_STA_INTERFACE );
}

wiced_result_t wiced_wifi_disable_powersave_interface( wiced_interface_t interface )
{
    return (wiced_result_t) wwd_wifi_disable_powersave_interface( (wwd_interface_t)interface );
}

wiced_result_t wiced_wifi_set_packet_filter_mode( wiced_packet_filter_mode_t mode )
{
    return (wiced_result_t) wwd_wifi_set_packet_filter_mode( mode );
}

wiced_result_t wiced_wifi_add_packet_filter( const wiced_packet_filter_t* settings )
{
    return (wiced_result_t) wwd_wifi_add_packet_filter( settings );
}

wiced_result_t wiced_wifi_remove_packet_filter( uint8_t filter_id )
{
    return (wiced_result_t) wwd_wifi_remove_packet_filter( filter_id );
}

wiced_result_t wiced_wifi_enable_packet_filter( uint8_t filter_id )
{
    return (wiced_result_t) wwd_wifi_enable_packet_filter( filter_id );
}

wiced_result_t wiced_wifi_disable_packet_filter( uint8_t filter_id )
{
    return (wiced_result_t) wwd_wifi_disable_packet_filter( filter_id );
}

wiced_result_t wiced_wifi_get_packet_filter_stats( uint8_t filter_id, wiced_packet_filter_stats_t* stats )
{
    return (wiced_result_t) wwd_wifi_get_packet_filter_stats( filter_id, stats );
}

wiced_result_t wiced_wifi_clear_packet_filter_stats( uint32_t filter_id )
{
    return  (wiced_result_t) wwd_wifi_clear_packet_filter_stats( filter_id );
}

wiced_result_t wiced_wifi_get_packet_filters( uint32_t max_count, uint32_t offset, wiced_packet_filter_t* list,  uint32_t* count_out )
{
    return (wiced_result_t) wwd_wifi_get_packet_filters( max_count, offset, list, count_out );
}

wiced_result_t wiced_wifi_get_packet_filter_mask_and_pattern( uint32_t filter_id, uint32_t max_size, uint8_t* mask, uint8_t* pattern, uint32_t* size_out )
{
    return (wiced_result_t) wwd_wifi_get_packet_filter_mask_and_pattern( filter_id, max_size, mask, pattern, size_out );
}

wiced_result_t wiced_wifi_add_keep_alive( wiced_keep_alive_packet_t* keep_alive_packet_info )
{
    return (wiced_result_t) wwd_wifi_add_keep_alive( keep_alive_packet_info );
}

wiced_result_t wiced_wifi_get_keep_alive( wiced_keep_alive_packet_t* keep_alive_packet_info )
{
    return (wiced_result_t) wwd_wifi_get_keep_alive( keep_alive_packet_info );
}

wiced_result_t wiced_wifi_disable_keep_alive( uint8_t id )
{
    return (wiced_result_t) wwd_wifi_disable_keep_alive( id );
}

wiced_result_t wiced_wifi_get_associated_client_list( void* client_list_buffer, uint16_t buffer_length )
{
    return (wiced_result_t) wwd_wifi_get_associated_client_list( client_list_buffer, buffer_length );
}

wiced_result_t wiced_wifi_get_ap_client_rssi( int32_t* rssi, const wiced_mac_t* client_mac_addr )
{
    return (wiced_result_t) wwd_wifi_get_ap_client_rssi( rssi, client_mac_addr );
}

wiced_result_t wiced_wifi_get_ap_info( wiced_bss_info_t* ap_info, wiced_security_t* security )
{
    return (wiced_result_t) wwd_wifi_get_ap_info( ap_info, security );
}

wiced_result_t wiced_wifi_set_ht_mode( wiced_interface_t interface, wiced_ht_mode_t ht_mode )
{
    return (wiced_result_t) wwd_wifi_set_ht_mode( (wwd_interface_t)interface, ht_mode );
}

wiced_result_t wiced_wifi_get_ht_mode( wiced_interface_t interface, wiced_ht_mode_t* ht_mode )
{
    return (wiced_result_t) wwd_wifi_get_ht_mode( (wwd_interface_t)interface, ht_mode );
}

wiced_result_t wiced_wifi_disable_11n_support( wiced_interface_t interface, wiced_bool_t disable )
{
    return (wiced_result_t) wwd_wifi_set_11n_support( (wwd_interface_t)interface, (disable==WICED_FALSE)?WICED_11N_SUPPORT_ENABLED : WICED_11N_SUPPORT_DISABLED );
}
