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
 *  Implements user functions for controlling the Wi-Fi system
 *
 *  This file provides end-user functions which allow actions such as scanning for
 *  Wi-Fi networks, joining Wi-Fi networks, getting the MAC address, etc
 *
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
#include "wiced_wifi.h"
#include "wwd_ap_common.h"
#include "wwd_wifi.h"
#include "wiced_management.h"

/******************************************************
 * @cond       Constants
 ******************************************************/

/* These are the flags in the BSS Capability Information field as defined in section 7.3.1.4 of IEEE Std 802.11-2007  */
#define DOT11_CAP_ESS                     (0x0001)   /** Extended service set capability */
#define DOT11_CAP_IBSS                    (0x0002)   /** Ad-hoc capability (Independent Basic Service Set) */
#define DOT11_CAP_PRIVACY                 (0x0010)   /** Privacy subfield - indicates data confidentiality is required for all data frames exchanged */
#define WL_CHANSPEC_CHAN_MASK             (0x00ff)
#define CHSPEC_CHANNEL(chspec)            ((uint8_t)((chspec) & WL_CHANSPEC_CHAN_MASK))
#define CH20MHZ_CHSPEC(channel)           (chanspec_t)((chanspec_t)(channel) | WL_CHANSPEC_BW_20 | \
                                            WL_CHANSPEC_CTL_SB_NONE | (((channel) <= CH_MAX_2G_CHANNEL) ? \
                                            WL_CHANSPEC_BAND_2G : WL_CHANSPEC_BAND_5G))
#define LEGACY_WL_BSS_INFO_VERSION           (107)   /** older version of wl_bss_info struct */

#define WICED_CREDENTIAL_TEST_TIMEOUT     (1500)

#define WL_BSS_FLAGS_RSSI_ONCHANNEL 0x04   /* rssi info was received on channel (vs offchannel) */
#define WL_BSS_FLAGS_FROM_BEACON    0x01   /* bss_info derived from beacon                      */

#define WPA_OUI_TYPE1                     "\x00\x50\xF2\x01"   /** WPA OUI */

#define MAX_SUPPORTED_MCAST_ENTRIES   (10)

#define WLC_EVENT_MSG_LINK      (0x01)

#define JOIN_ASSOCIATED             (uint32_t)(1 << 0)
#define JOIN_AUTHENTICATED          (uint32_t)(1 << 1)
#define JOIN_LINK_READY             (uint32_t)(1 << 2)
#define JOIN_SECURITY_COMPLETE      (uint32_t)(1 << 3)
#define JOIN_SSID_SET               (uint32_t)(1 << 4)
#define JOIN_NO_NETWORKS            (uint32_t)(1 << 5)
#define JOIN_EAPOL_KEY_M1_TIMEOUT   (uint32_t)(1 << 6)
#define JOIN_EAPOL_KEY_M3_TIMEOUT   (uint32_t)(1 << 7)
#define JOIN_EAPOL_KEY_G1_TIMEOUT   (uint32_t)(1 << 8)
#define JOIN_EAPOL_KEY_FAILURE      (uint32_t)(1 << 9)

#define JOIN_SECURITY_FLAGS_MASK    ( JOIN_SECURITY_COMPLETE | JOIN_EAPOL_KEY_M1_TIMEOUT | JOIN_EAPOL_KEY_M3_TIMEOUT | JOIN_EAPOL_KEY_G1_TIMEOUT | JOIN_EAPOL_KEY_FAILURE)

#define DEFAULT_JOIN_ATTEMPT_TIMEOUT     (7000) /* Overall join attempt timeout in milliseconds. */
#define DEFAULT_EAPOL_KEY_PACKET_TIMEOUT (2500) /* Timeout when waiting for EAPOL key packet M1 or M3 in milliseconds. Some APs may be slow to provide M1 and 1000 ms is not long enough for edge of cell. */

#ifndef DEFAULT_PM2_SLEEP_RET_TIME
#define DEFAULT_PM2_SLEEP_RET_TIME   (200)
#endif

#define PM2_SLEEP_RET_TIME_MIN       (10)   /* Minimum return-to-sleep in milliseconds */
#define PM2_SLEEP_RET_TIME_MAX       (2000) /* Maximum return-to-sleep in milliseconds */

#define NULL_FRAMES_WITH_PM_SET_LIMIT ( 100 )

#define VALID_SECURITY_MASK    0x00FFFFFF

#define CHECK_RETURN_UNSUPPORTED_OK( expr )  { wwd_result_t check_res = (expr); if ( check_res != WWD_SUCCESS ) { wiced_assert("Command failed\n", check_res == WWD_WLAN_UNSUPPORTED); return check_res; } }
#define CHECK_RETURN_UNSUPPORTED_CONTINUE( expr )  { wwd_result_t check_res = (expr); if ( check_res != WWD_SUCCESS && check_res != WWD_WLAN_UNSUPPORTED ) { wiced_assert("Command failed\n", 0 == 1); return check_res; } }
#define RETURN_WITH_ASSERT( expr )  { wwd_result_t check_res = (expr); wiced_assert("Command failed\n", check_res == WWD_SUCCESS ); return check_res; }

#define PACKET_FILTER_LIST_BUFFER_MAX_LEN ( WICED_LINK_MTU - sizeof(IOVAR_STR_PKT_FILTER_LIST) - IOCTL_OFFSET )

#define RSPEC_KBPS_MASK (0x7f)
#define RSPEC_500KBPS( rate ) ((rate) &  RSPEC_KBPS_MASK)
#define RSPEC_TO_KBPS( rate ) (RSPEC_500KBPS((rate)) * (unsigned int) 500)

#define OTP_WORD_SIZE 16    /* Word size in bits */

#define UNSIGNED_CHAR_TO_CHAR( uch ) ((uch) & 0x7f)

#define MAC_ADDRESS_LOCALLY_ADMINISTERED_BIT 0x02
/** @endcond */

/******************************************************
 *             Local Structures
 ******************************************************/

#pragma pack(1)

typedef struct
{
    uint32_t    entry_count;
    wiced_mac_t macs[1];
} mcast_list_t;

typedef struct
{
    uint32_t cfg;
    uint32_t val;
} bss_setbuf_t;

typedef struct
{
    int32_t     value;
    wiced_mac_t mac_address;
} client_rssi_t;

typedef struct
{
    wiced_security_t security;
    int32_t auth_algo;
    int32_t wpa_auth;
    int32_t wsec;
} wwd_security_to_auth_wsec_map_t;

#pragma pack()

/******************************************************
 *             Static Variables
 ******************************************************/

static wiced_scan_result_callback_t   scan_result_callback = NULL;
static wiced_scan_result_t**          wwd_scan_result_ptr  = NULL;
static wwd_interface_t       WICED_DEEP_SLEEP_SAVED_VAR( wiced_cached_mac_interface ) = WWD_INTERFACE_MAX;
static wiced_mac_t           WICED_DEEP_SLEEP_SAVED_VAR( wiced_cached_mac );
static uint32_t              WICED_DEEP_SLEEP_SAVED_VAR( wiced_join_status[WWD_INTERFACE_MAX] );

/* LOOK: !!!When adding events below, please modify wwd_event_to_string!!! */
const wwd_event_num_t        join_events[]  = { WLC_E_SET_SSID, WLC_E_LINK, WLC_E_AUTH, WLC_E_DEAUTH_IND, WLC_E_DISASSOC_IND, WLC_E_PSK_SUP, WLC_E_CSA_COMPLETE_IND, WLC_E_NONE };
static const wwd_event_num_t scan_events[]  = { WLC_E_ESCAN_RESULT, WLC_E_NONE };
static const wwd_event_num_t pno_events[]   = { WLC_E_PFN_NET_FOUND, WLC_E_NONE };
static const wwd_event_num_t rrm_events[]   = { WLC_E_RRM, WLC_E_NONE };

static uint8_t  wiced_wifi_powersave_mode = NO_POWERSAVE_MODE;
static uint16_t wiced_wifi_return_to_sleep_delay;

/* Note: monitor_mode_enabled variable is accessed by SDPCM */
static wiced_bool_t wwd_sdpcm_monitor_mode_enabled = WICED_FALSE;

/* The semaphore used to wait for completion of a join; wwd_wifi_join_halt uses this to release waiting threads (if any) */
static host_semaphore_type_t* WICED_DEEP_SLEEP_SAVED_VAR( active_join_semaphore ) = NULL;

/* protect multi-threaded access of above semaphore pointer */
static host_semaphore_type_t  WICED_DEEP_SLEEP_SAVED_VAR( active_join_mutex );
static volatile wiced_bool_t  WICED_DEEP_SLEEP_SAVED_VAR( active_join_mutex_initted ) = WICED_FALSE;

/* track whether joins are all halted per the wwd_wifi_join_halt API */
static volatile wiced_bool_t  WICED_DEEP_SLEEP_SAVED_VAR( join_halt_triggered ) = WICED_FALSE;

static wiced_bool_t wwd_wifi_p2p_go_is_up = WICED_FALSE;
void (*wwd_wifi_link_update_callback)( void ) = NULL;

/* Values are in 100's of Kbit/sec (1 = 100Kbit/s). Arranged as:
 * [Bit index]
 *    [0] = 20Mhz only
 *       [0] = Long GI
 *       [1] = Short GI
 *    [1] = 40MHz support
 *       [0] = Long GI
 *       [1] = Short GI
 */
static const uint16_t mcs_data_rate_lookup_table[32][2][2] =
{
    [0 ] = { {  65  ,   72  },  {   135 ,   150 } },
    [1 ] = { {  130 ,   144 },  {   270 ,   300 } },
    [2 ] = { {  195 ,   217 },  {   405 ,   450 } },
    [3 ] = { {  260 ,   289 },  {   540 ,   600 } },
    [4 ] = { {  390 ,   433 },  {   810 ,   900 } },
    [5 ] = { {  520 ,   578 },  {   1080,   1200} },
    [6 ] = { {  585 ,   650 },  {   1215,   1350} },
    [7 ] = { {  650 ,   722 },  {   1350,   1500} },
    [8 ] = { {  130 ,   144 },  {   270 ,   300 } },
    [9 ] = { {  260 ,   289 },  {   540 ,   600 } },
    [10] = { {  390 ,   433 },  {   810 ,   900 } },
    [11] = { {  520 ,   578 },  {   1080,   1200} },
    [12] = { {  780 ,   867 },  {   1620,   1800} },
    [13] = { {  1040,   1156},  {   2160,   2400} },
    [14] = { {  1170,   1300},  {   2430,   2700} },
    [15] = { {  1300,   1444},  {   2700,   3000} },
    [16] = { {  195 ,   217 },  {   405 ,   450 } },
    [17] = { {  390 ,   433 },  {   810 ,   900 } },
    [18] = { {  585 ,   650 },  {   1215,   1350} },
    [19] = { {  780 ,   867 },  {   1620,   1800} },
    [20] = { {  1170,   1300},  {   2430,   2700} },
    [21] = { {  1560,   1733},  {   3240,   3600} },
    [22] = { {  1755,   1950},  {   3645,   4050} },
    [23] = { {  1950,   2167},  {   4050,   4500} },
    [24] = { {  260 ,   288 },  {   540 ,   600 } },
    [25] = { {  520 ,   576 },  {   1080,   1200} },
    [26] = { {  780 ,   868 },  {   1620,   1800} },
    [27] = { {  1040,   1156},  {   2160,   2400} },
    [28] = { {  1560,   1732},  {   3240,   3600} },
    [29] = { {  2080,   2312},  {   4320,   4800} },
    [30] = { {  2340,   2600},  {   4860,   5400} },
    [31] = { {  2600,   2888},  {   5400,   6000} },
};

/* Note that the qos_map is accessed by SDPCM */
uint8_t wwd_tos_map[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
static uint32_t bandwidth = 20;          /* default 20Mhz bandwidth */
static uint32_t wwd_wifi_mesh_flags;

/******************************************************
 *             Static Function prototypes
 ******************************************************/
static inline /*@null@*/ tlv8_header_t* wlu_parse_tlvs              ( const tlv8_header_t* tlv_buf, uint32_t buflen, dot11_ie_id_t key );
static            wiced_bool_t    wlu_is_wpa_ie                     ( vendor_specific_ie_header_t* wpaie, tlv8_header_t** tlvs, uint32_t* tlvs_len );
static /*@null@*/ void*           wiced_join_events_handler         ( const wwd_event_header_t* event_header, const uint8_t* event_data, /*@returned@*/ void* handler_user_data );
static            void*           wwd_scan_events_handler           ( const wwd_event_header_t* event_header, const uint8_t* event_data, /*@returned@*/ void* handler_user_data );
static            wwd_result_t    wwd_wifi_prepare_join             ( wwd_interface_t interface, wiced_security_t security, /*@unique@*/ const uint8_t* security_key, uint8_t key_length, host_semaphore_type_t* semaphore );
static            wwd_result_t    wwd_wifi_get_packet_filters_inner ( uint32_t max_count, uint32_t offset, wiced_packet_filter_t* list, wiced_bool_t enabled_list, uint32_t* count_out );
static            wwd_result_t    wwd_wifi_set_band_specific_rate   ( wwd_interface_t interface, uint32_t rate );
static            wwd_result_t    wwd_wifi_check_join_status        ( wwd_interface_t interface );
static            tlv8_header_t*  wwd_parse_dot11_tlvs              ( const tlv8_header_t* tlv_buf, uint32_t buflen, dot11_ie_id_t key );
static            wwd_result_t    wwd_wifi_pno_set_enable( wiced_bool_t enable );
static            void            wwd_wifi_active_join_deinit ( host_semaphore_type_t *stack_semaphore, wwd_result_t result, host_semaphore_type_t *client_semaphore );
static            wwd_result_t    wwd_wifi_active_join_init         ( wiced_security_t auth_type, const uint8_t* security_key, uint8_t key_length, host_semaphore_type_t *semaphore, wwd_interface_t interface );
static            uint32_t        wwd_wifi_band_to_wlc_band( wiced_802_11_band_t band );


/******************************************************
 *             Debug helper functionality
 ******************************************************/
#ifdef WPRINT_ENABLE_WWD_DEBUG
static            const char*     wwd_event_to_string         (wwd_event_num_t var);
static            char*           wwd_ssid_to_string          (uint8_t *value, uint8_t length, char *ssid_buf, uint8_t ssid_buf_len);
static            const char*     wwd_status_to_string        (wwd_event_status_t status);
static            const char*     wwd_reason_to_string        (wwd_event_reason_t reason);
static            const char*     wwd_interface_to_string      (wwd_interface_t interface);
#endif /* ifdef WPRINT_ENABLE_WWD_DEBUG */


static wwd_result_t wwd_nan_config (wiced_bool_t enable);

/******************************************************
 *             Function definitions
 ******************************************************/

wiced_bool_t wwd_wifi_p2p_is_go_up( void )
{
    return wwd_wifi_p2p_go_is_up;
}

void wwd_wifi_p2p_set_go_is_up( wiced_bool_t is_up )
{
    if ( wwd_wifi_p2p_go_is_up != is_up )
    {
        wwd_wifi_p2p_go_is_up = is_up;
        wwd_wifi_link_update( );
    }
}

wwd_result_t    wwd_nan_config_enable   ( void );
/*
 * NOTE: search references of function wlu_get in wl/exe/wlu.c to find what format the returned IOCTL data is.
 */

wwd_result_t wwd_wifi_scan( wiced_scan_type_t                              scan_type,
                            wiced_bss_type_t                               bss_type,
                            /*@null@*/ const wiced_ssid_t*                 optional_ssid,
                            /*@null@*/ const wiced_mac_t*                  optional_mac,
                            /*@null@*/ /*@unique@*/ const uint16_t*        optional_channel_list,
                            /*@null@*/ const wiced_scan_extended_params_t* optional_extended_params,
                            wiced_scan_result_callback_t                   callback,
                            wiced_scan_result_t**                          result_ptr,
                            /*@null@*/ void*                               user_data,
                            wwd_interface_t                                interface
                          )
{
    wiced_buffer_t     buffer;
    wl_escan_params_t* scan_params;
    wwd_result_t       retval;
    uint16_t           param_size        = offsetof( wl_escan_params_t, params ) + WL_SCAN_PARAMS_FIXED_SIZE;
    uint16_t           channel_list_size = 0;

    wiced_assert("Bad args", callback != NULL);

    /* Determine size of channel_list, and add it to the parameter size so correct sized buffer can be allocated */
    if ( optional_channel_list != NULL )
    {
        /* Look for entry with channel number 0, which suggests the end of channel_list */
        for ( channel_list_size = 0; optional_channel_list[channel_list_size] != 0; channel_list_size++ )
        {
        }
        param_size = (uint16_t) ( param_size + channel_list_size * sizeof(uint16_t) );
    }

    if ( scan_type == WICED_SCAN_TYPE_PNO )
    {
        CHECK_RETURN( wwd_management_set_event_handler( pno_events, wwd_scan_events_handler, user_data, interface ) );

        scan_result_callback = callback;
        wwd_scan_result_ptr = result_ptr;

        return wwd_wifi_pno_start( );
    }
    /* else: regular scan */

    CHECK_RETURN( wwd_management_set_event_handler( scan_events, wwd_scan_events_handler, user_data, interface ) );

    /* Allocate a buffer for the IOCTL message */
    scan_params = (wl_escan_params_t*) wwd_sdpcm_get_iovar_buffer( &buffer, param_size, IOVAR_STR_ESCAN );
    CHECK_IOCTL_BUFFER( scan_params );

    /* Clear the scan parameters structure */
    memset( scan_params, 0, sizeof(wl_escan_params_t) );

    /* Fill in the appropriate details of the scan parameters structure */
    scan_params->version          = htod32(ESCAN_REQ_VERSION);
    scan_params->action           = htod16(WL_SCAN_ACTION_START);
    scan_params->params.scan_type = (int8_t) scan_type;
    scan_params->params.bss_type  = (int8_t) bss_type;

    /* Fill out the SSID parameter if provided */
    if ( optional_ssid != NULL )
    {
        scan_params->params.ssid.SSID_len = optional_ssid->length;
        memcpy( scan_params->params.ssid.SSID, optional_ssid->value, scan_params->params.ssid.SSID_len );
    }

    /* Fill out the BSSID parameter if provided */
    if ( optional_mac != NULL )
    {
        memcpy( scan_params->params.bssid.octet, optional_mac, sizeof(wiced_mac_t) );
    }
    else
    {
        memset( scan_params->params.bssid.octet, 0xff, sizeof(wiced_mac_t) );
    }

    /* Fill out the extended parameters if provided */
    if ( optional_extended_params != NULL )
    {
        scan_params->params.nprobes      = optional_extended_params->number_of_probes_per_channel;
        scan_params->params.active_time  = optional_extended_params->scan_active_dwell_time_per_channel_ms;
        scan_params->params.passive_time = optional_extended_params->scan_passive_dwell_time_per_channel_ms;
        scan_params->params.home_time    = optional_extended_params->scan_home_channel_dwell_time_between_channels_ms;
    }
    else
    {
        scan_params->params.nprobes      = (int32_t) -1;
        scan_params->params.active_time  = (int32_t) -1;
        scan_params->params.passive_time = (int32_t) -1;
        scan_params->params.home_time    = (int32_t) -1;
    }

    /* Copy the channel list parameter if provided */
    if ( ( channel_list_size > 0 ) && ( optional_channel_list != NULL ) )
    {
        int i;

        for ( i = 0; i < channel_list_size; i++)
        {
             scan_params->params.channel_list[i] = CH20MHZ_CHSPEC(optional_channel_list[i]);
        }
        scan_params->params.channel_num = (int32_t) channel_list_size;
    }

    scan_result_callback = callback;
    wwd_scan_result_ptr = result_ptr;

    /* Send the Incremental Scan IOVAR message - blocks until the response is received */
    retval = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, interface );

    /* Return the success of the IOCTL */
    return retval;
}

wwd_result_t wwd_wifi_set_scan_suppress( wiced_bool_t enable_suppression )
{
    return wwd_wifi_set_ioctl_value( WLC_SET_SCANSUPPRESS, ( WICED_TRUE == enable_suppression ) ? 1 : 0,
        WWD_STA_INTERFACE );
}

wwd_result_t wwd_wifi_abort_scan( void )
{
    wiced_buffer_t     buffer;
    wl_escan_params_t* scan_params;
    wwd_result_t       result;

    /* Allocate a buffer for the IOCTL message */
    scan_params = (wl_escan_params_t*) wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(wl_escan_params_t), IOVAR_STR_ESCAN );
    CHECK_IOCTL_BUFFER( scan_params );

    /* Clear the scan parameters structure */
    memset( scan_params, 0, sizeof(wl_escan_params_t) );

    /* Fill in the appropriate details of the scan parameters structure */
    scan_params->version = ESCAN_REQ_VERSION;
    scan_params->action  = WL_SCAN_ACTION_ABORT;

    /* Send the Scan IOVAR message to abort scan*/
    result = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );

    return result;
}

/** Handles scan result events
 *
 *  This function receives scan record events, and parses them into a better format, then passes the results
 *  to the user application.
 *
 * @param event_header     : The event details
 * @param event_data       : The data for the event which contains the scan result structure
 * @param handler_user_data: data which will be passed to user application
 *
 * @returns : handler_user_data parameter
 *
 */
static void* wwd_scan_events_handler( const wwd_event_header_t* event_header, const uint8_t* event_data, /*@returned@*/ void* handler_user_data )
{
    wiced_scan_result_t*     record;
    wl_escan_result_t*       eresult;
    wl_bss_info_t*           bss_info;
    uint16_t                 chanspec;
    uint32_t                 version;
    tlv8_header_t*           cp;
    uint32_t                 len;
    uint16_t                 ie_offset;
    uint32_t                 bss_info_length;
    country_info_ie_fixed_portion_t*  country_info_ie;
    rsn_ie_fixed_portion_t*  rsnie;
    wpa_ie_fixed_portion_t*  wpaie = NULL;
    uint8_t                  rate_num;
    ht_capabilities_ie_t*    ht_capabilities_ie = NULL;
    uint32_t                 count_tmp = 0;
    uint16_t                 temp16;
    uint16_t                 bss_count;
    wl_pfn_scanresult_t*     pfn_result    = NULL;
    wl_pfn_net_info_t*       pfn_net_info  = NULL;
    wl_pfn_subnet_info_t*    pfn_subnet    = NULL;

    if ( scan_result_callback == NULL )
    {
        return handler_user_data;
    }

    /* process the pfn net found */
    if ( event_header->event_type == WLC_E_PFN_NET_FOUND )
    {
        if ( wwd_scan_result_ptr != NULL )
        {
            record = (wiced_scan_result_t*) ( *wwd_scan_result_ptr );
            if ( record == NULL )
            {
                scan_result_callback( (wiced_scan_result_t**)event_data, handler_user_data, WICED_SCAN_INCOMPLETE );
                return handler_user_data;
            }

            pfn_result    = (wl_pfn_scanresult_t *)event_data;
            pfn_subnet    = &pfn_result->netinfo.pfnsubnet;
            pfn_net_info  = &pfn_result->netinfo;

            /* Firmware needs to be patched to allow returning the bss info in the result, if we want full scan results from PNO.
                    * As the supported use case doesn't require that, leaving as-is to reduce memory usage here and in FW.
                    */
            record->SSID.length = pfn_subnet->SSID_len;
            memset( record->SSID.value, 0, sizeof(record->SSID.value) );
            memcpy( record->SSID.value, pfn_subnet->SSID, pfn_subnet->SSID_len );
            memcpy( record->BSSID.octet, pfn_subnet->BSSID.octet, sizeof(record->BSSID.octet) );
            record->signal_strength = pfn_net_info->RSSI;
            record->bss_type        = WICED_BSS_TYPE_INFRASTRUCTURE;
            record->channel         = pfn_subnet->channel;
            record->band            = ( wwd_channel_to_wl_band( record->channel ) == (uint16_t) WL_CHANSPEC_BAND_2G ) ? WICED_802_11_BAND_2_4GHZ : WICED_802_11_BAND_5GHZ;;

            /* For PNO, the user has already specified security to look for; Wiced layer will update this from recorded data */
            record->security        = WICED_SECURITY_UNKNOWN;
        }

        /* event header status checks don't make sense for pno events
               * Also, firmware isn't passing up valid data in the bss_info field, so filled out and now return what we have
               */
        scan_result_callback( wwd_scan_result_ptr, handler_user_data, WICED_SCAN_INCOMPLETE );
        return handler_user_data;
    }

    if ( event_header->status == WLC_E_STATUS_SUCCESS)
    {
        scan_result_callback( NULL, handler_user_data, WICED_SCAN_COMPLETED_SUCCESSFULLY );
        scan_result_callback = NULL;
        return handler_user_data;
    }
    if ( event_header->status == WLC_E_STATUS_NEWSCAN ||
         event_header->status == WLC_E_STATUS_NEWASSOC ||
         event_header->status == WLC_E_STATUS_ABORT )
    {
        scan_result_callback( NULL, handler_user_data, WICED_SCAN_ABORTED );
        scan_result_callback = NULL;
        return handler_user_data;
    }

    if ( event_header->status != WLC_E_STATUS_PARTIAL )
    {
        return handler_user_data;
    }

    eresult = (wl_escan_result_t*) event_data;
    bss_info = &eresult->bss_info[0];
    bss_count = eresult->bss_count;

    version = WICED_READ_32( &bss_info->version );
    wiced_minor_assert( "wl_bss_info_t has wrong version", version == WL_BSS_INFO_VERSION );


    if ( wwd_scan_result_ptr == NULL )
    {
        scan_result_callback( (wiced_scan_result_t**)event_data, handler_user_data, WICED_SCAN_INCOMPLETE );
        return handler_user_data;
    }

    /* PNO bss info doesn't contain the correct bss info version */
    if ( version != WL_BSS_INFO_VERSION )
    {
        wiced_minor_assert( "Invalid bss_info version returned by firmware\n", version != WL_BSS_INFO_VERSION );

        return handler_user_data;
    }

    wiced_minor_assert( "More than one result returned by firmware", bss_count == 1 );
    if ( bss_count != 1 )
    {
        return handler_user_data;
    }

    /*
     * check the SSID length and bssinfo ie offset for buffer overflow
     */
    if ( ( bss_info->SSID_len  > sizeof(bss_info->SSID)) ||
         ( bss_info->ie_offset < sizeof(wl_bss_info_t)) ||
         ( bss_info->ie_offset > (sizeof(wl_bss_info_t) + bss_info->ie_length)) )
    {
        WPRINT_WWD_ERROR (( "Invalid bss length check %s: SSID_len:%d,ie_len:%ld,ie_off:%d\n",
                            __FUNCTION__, bss_info->SSID_len,
                            bss_info->ie_length, bss_info->ie_offset ));
        wiced_minor_assert( " bss length check failed\n", bss_info->SSID_len != sizeof(bss_info->SSID) );
        return handler_user_data;
    }

    /* Safe to access *wwd_scan_result_ptr, as wwd_scan_result_ptr == NULL case is handled above */
    record = (wiced_scan_result_t*) ( *wwd_scan_result_ptr );

    /*
     * Totally ignore off channel results.  This can only happen with DSSS (1 and 2 Mb).  It is better to
     * totally ignore it when it happens.  It is hard to argue it is "significant" given that it can't
     * happen in 5G with OFDM (or other 2G modulations).  This is left here so that it could be
     * passed as a scan result for debugging only.
     */
    if ( !(bss_info->flags & WL_BSS_FLAGS_RSSI_ONCHANNEL) )
    {
         record->flags |= WICED_SCAN_RESULT_FLAG_RSSI_OFF_CHANNEL;
         /* Comment out this return statement to pass along an off channel result for debugging */
         return handler_user_data;
    }


    /* Copy the SSID into the output record structure */
    record->SSID.length = (uint8_t) MIN(sizeof(record->SSID.value), bss_info->SSID_len);
    memset( record->SSID.value, 0, sizeof(record->SSID.value) );
    memcpy( record->SSID.value, bss_info->SSID, record->SSID.length );

    /* Copy the BSSID into the output record structure */
    memcpy( (void*) record->BSSID.octet, (const void*) bss_info->BSSID.octet, sizeof(bss_info->BSSID.octet) );

    /* Copy the RSSI into the output record structure */
    record->signal_strength = (int16_t)(WICED_READ_16(&bss_info->RSSI));

    /* Find maximum data rate and put it in the output record structure */
    record->max_data_rate = 0;
    count_tmp = WICED_READ_32(&bss_info->rateset.count);
    if ( count_tmp > 16 )
    {
        count_tmp = 16;
    }

#ifdef WPRINT_ENABLE_WWD_DEBUG
    /* print out scan results info */
    {
        char ea_buf[WICED_ETHER_ADDR_STR_LEN];
        char ssid_buf[SSID_NAME_SIZE + 1];

        WPRINT_WWD_DEBUG(("Scan result: channel=%d signal=%d ssid=%s bssid=%s\n", record->channel, record->signal_strength,
            wwd_ssid_to_string(record->SSID.value, record->SSID.length, ssid_buf, (uint8_t)sizeof(ssid_buf)),
            wiced_ether_ntoa(( const uint8_t * ) bss_info->BSSID.octet, ea_buf, sizeof( ea_buf ))));
    }
#endif /* WPRINT_ENABLE_WWD_DEBUG */

    for ( rate_num = 0; rate_num < count_tmp; rate_num++ )
    {
        uint32_t rate = RSPEC_TO_KBPS( bss_info->rateset.rates[rate_num] );
        if ( record->max_data_rate < rate )
        {
            record->max_data_rate = rate;
        }
    }

    /* Write the BSS type into the output record structure */
    record->bss_type = ( ( bss_info->capability & DOT11_CAP_ESS  ) != 0 ) ? WICED_BSS_TYPE_INFRASTRUCTURE :
                       ( ( bss_info->capability & DOT11_CAP_IBSS ) != 0 ) ? WICED_BSS_TYPE_ADHOC :
                                                                            WICED_BSS_TYPE_UNKNOWN;

    /* Determine the network security.
     * Some of this section has been copied from the standard broadcom host driver file wl/exe/wlu.c function wl_dump_wpa_rsn_ies
     */

    ie_offset = WICED_READ_16(&bss_info->ie_offset);
    cp  = (tlv8_header_t*) ( ( (uint8_t*) bss_info ) + ie_offset );
    len = WICED_READ_32(&bss_info->ie_length);
    bss_info_length = WICED_READ_32(&bss_info->length);

    /* Validate the length of the IE section */
    if ( ( ie_offset > bss_info_length ) ||
         ( len > bss_info_length - ie_offset ) )
    {
        wiced_minor_assert( "Invalid ie length", 1 == 0 );
        return handler_user_data;
    }

    /* If its an unknown type, try parsing for Mesh ID tag */
    if (record->bss_type == WICED_BSS_TYPE_UNKNOWN) {
        if (wwd_parse_dot11_tlvs( cp, len, DOT11_IE_ID_MESH_ID ) != NULL) {
            record->bss_type = WICED_BSS_TYPE_MESH;
        }
    }

    /* Find an RSN IE (Robust-Security-Network Information-Element) */
    rsnie = (rsn_ie_fixed_portion_t*) wwd_parse_dot11_tlvs( cp, len, DOT11_IE_ID_RSN );

    /* Find a WPA IE */
    if ( rsnie == NULL )
    {
        tlv8_header_t*  parse = cp;
        uint32_t        parse_len = len;
        while ( ( wpaie = (wpa_ie_fixed_portion_t*) wlu_parse_tlvs( parse, parse_len, DOT11_IE_ID_VENDOR_SPECIFIC ) ) != 0 )
        {
            if ( wlu_is_wpa_ie( (vendor_specific_ie_header_t*) wpaie, &parse, &parse_len ) != WICED_FALSE )
            {
                break;
            }
        }
    }

    temp16 = WICED_READ_16(&bss_info->capability);

    /* Check if AP is configured for RSN */
    if ( ( rsnie != NULL ) &&
         ( rsnie->tlv_header.length >= RSN_IE_MINIMUM_LENGTH + rsnie->pairwise_suite_count * sizeof(uint32_t) ) )
    {
        uint16_t a;
        uint32_t group_key_suite;
        akm_suite_portion_t* akm_suites;
        akm_suites = (akm_suite_portion_t*) &(rsnie->pairwise_suite_list[rsnie->pairwise_suite_count]);
        for ( a = 0; a < akm_suites->akm_suite_count; ++a )
        {
            uint32_t akm_suite_list_item = NTOH32(akm_suites->akm_suite_list[a]) & 0xFF;
            if ( akm_suite_list_item == (uint32_t) WICED_AKM_PSK )
            {
                record->security |= WPA2_SECURITY;
            }
            if ( akm_suite_list_item == (uint32_t) WICED_AKM_SAE_SHA256 )
            {
                record->security |= WPA3_SECURITY;
            }
            if ( akm_suite_list_item == (uint32_t) WICED_AKM_8021X )
            {
                record->security |= ENTERPRISE_ENABLED;
            }
            if ( akm_suite_list_item == (uint32_t) WICED_AKM_FT_8021X )
            {
                record->security |= FBT_ENABLED;
                record->security |= ENTERPRISE_ENABLED;
            }
            if ( akm_suite_list_item == (uint32_t) WICED_AKM_FT_PSK )
            {
                record->security |= FBT_ENABLED;
            }
        }

        group_key_suite = NTOH32( rsnie->group_key_suite ) & 0xFF;
        /* Check the RSN contents to see if there are any references to TKIP cipher (2) in the group key or pairwise keys. If so it must be mixed mode. */
        if ( group_key_suite == (uint32_t) WICED_CIPHER_TKIP )
        {
            record->security |= TKIP_ENABLED;
        }
        if ( group_key_suite == (uint32_t) WICED_CIPHER_CCMP_128 )
        {
            record->security |= AES_ENABLED;
        }

        for ( a = 0; a < rsnie->pairwise_suite_count; ++a )
        {
            uint32_t pairwise_suite_list_item = NTOH32( rsnie->pairwise_suite_list[a] ) & 0xFF;
            if ( pairwise_suite_list_item == (uint32_t) WICED_CIPHER_TKIP )
            {
                record->security |= TKIP_ENABLED;
            }

            if ( pairwise_suite_list_item == (uint32_t) WICED_CIPHER_CCMP_128 )
            {
                record->security |= AES_ENABLED;
            }
        }
    }
    /* Check if AP is configured for WPA */
    else if ( ( wpaie != NULL ) &&
              ( wpaie->vendor_specific_header.tlv_header.length >= WPA_IE_MINIMUM_LENGTH + wpaie->unicast_suite_count * sizeof(uint32_t) ) )
    {
        uint16_t a;
        uint32_t group_key_suite;
        akm_suite_portion_t* akm_suites;

        record->security = (wiced_security_t) WPA_SECURITY;
        group_key_suite = NTOH32( wpaie->multicast_suite ) & 0xFF;
        if ( group_key_suite == (uint32_t) WICED_CIPHER_TKIP )
        {
            record->security |= TKIP_ENABLED;
        }
        if ( group_key_suite == (uint32_t) WICED_CIPHER_CCMP_128 )
        {
            record->security |= AES_ENABLED;
        }

        akm_suites = (akm_suite_portion_t*) &(wpaie->unicast_suite_list[wpaie->unicast_suite_count]);
        for ( a = 0; a < akm_suites->akm_suite_count; ++a )
        {
            uint32_t akm_suite_list_item = NTOH32(akm_suites->akm_suite_list[a]) & 0xFF;
            if ( akm_suite_list_item == (uint32_t) WICED_AKM_8021X )
            {
                record->security |= ENTERPRISE_ENABLED;
            }
        }

        for ( a = 0; a < wpaie->unicast_suite_count; ++a )
        {
            if ( wpaie->unicast_suite_list[a][3] == (uint32_t) WICED_CIPHER_CCMP_128 )
            {
                record->security |= AES_ENABLED;
            }
        }
    }
    /* Check if AP is configured for WEP, that is, if the capabilities field indicates privacy, then security supports WEP */
    else if ( ( temp16 & DOT11_CAP_PRIVACY ) != 0 )
    {
        record->security = WICED_SECURITY_WEP_PSK;
    }
    else
    {
        /* Otherwise no security */
        record->security = WICED_SECURITY_OPEN;
    }

    /* Update the maximum data rate with 11n rates from the HT Capabilities IE */
    ht_capabilities_ie = (ht_capabilities_ie_t*)wlu_parse_tlvs( cp, len, DOT11_IE_ID_HT_CAPABILITIES );
    if ( ( ht_capabilities_ie != NULL ) &&
         ( ht_capabilities_ie->tlv_header.length == HT_CAPABILITIES_IE_LENGTH ) )
    {
        uint8_t a;
        uint8_t supports_40mhz =   ( ht_capabilities_ie->ht_capabilities_info & HT_CAPABILITIES_INFO_SUPPORTED_CHANNEL_WIDTH_SET ) != 0 ? 1 : 0;
        uint8_t short_gi[2]    = { ( ht_capabilities_ie->ht_capabilities_info & HT_CAPABILITIES_INFO_SHORT_GI_FOR_20MHZ          ) != 0 ? 1 : 0,
                                   ( ht_capabilities_ie->ht_capabilities_info & HT_CAPABILITIES_INFO_SHORT_GI_FOR_40MHZ          ) != 0 ? 1 : 0 };

        /* Find highest bit from MCS info */
        for (a = 31; a != 0xFF; --a)
        {
            if ( ( ht_capabilities_ie->rx_mcs[a / 8] & ( 1 << ( a % 8 ) )) != 0 )
            {
                break;
            }
        }
        if (a != 0xFF)
        {
            record->max_data_rate = (uint32_t)( 100UL * mcs_data_rate_lookup_table[a][supports_40mhz][short_gi[supports_40mhz]] );
        }
    }

    if ( bss_info->flags & WL_BSS_FLAGS_FROM_BEACON )
    {
         record->flags |= WICED_SCAN_RESULT_FLAG_BEACON;
    }

    /* Get the channel for pre-N and control channel for n/HT or later */
    chanspec        = WICED_READ_16( &bss_info->chanspec );
    if (bss_info->n_cap)
    {

        /* Check control channel first.The channel that chanspec reports is the center frequency which might not be the same as
         * the 20 MHz channel that the beacons is on (primary or control channel) if it's an 802.11n/AC 40MHz or wider channel.
         */
        record->channel = bss_info->ctl_ch;
    }
    else
    {
        /* 11 a/b/g and 20MHz bandwidth only */
        record->channel = ( (uint8_t) ( chanspec & WL_CHANSPEC_CHAN_MASK ) );
    }

    /* Find country info IE (Country-Information Information-Element) */
    country_info_ie = (country_info_ie_fixed_portion_t*) wwd_parse_dot11_tlvs( cp, len, DOT11_IE_ID_COUNTRY );
    if (( country_info_ie != NULL ) && ( country_info_ie->tlv_header.length >= COUNTRY_INFO_IE_MINIMUM_LENGTH ))
    {
        record->ccode[0] = UNSIGNED_CHAR_TO_CHAR( country_info_ie->ccode[0] );
        record->ccode[1] = UNSIGNED_CHAR_TO_CHAR( country_info_ie->ccode[1] );
    }

    record->band = ( ( chanspec & WL_CHANSPEC_BAND_MASK ) == (uint16_t) WL_CHANSPEC_BAND_2G ) ? WICED_802_11_BAND_2_4GHZ : WICED_802_11_BAND_5GHZ;

    scan_result_callback( wwd_scan_result_ptr, handler_user_data, WICED_SCAN_INCOMPLETE );
    if ( *wwd_scan_result_ptr == NULL )
    {
#if 0
        wwd_management_set_event_handler( scan_events, NULL, NULL );
#endif /* if 0 */
    }

    return handler_user_data;
}

wwd_result_t wwd_wifi_set_scan_params( uint32_t assoc_time,
                                       uint32_t unassoc_time,
                                       uint32_t passive_time,
                                       uint32_t home_time,
                                       uint32_t nprobes )
{
    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_SCAN_ASSOC_TIME, assoc_time, WWD_STA_INTERFACE ));
    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_SCAN_UNASSOC_TIME, unassoc_time, WWD_STA_INTERFACE ));
    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_SCAN_PASSIVE_TIME, passive_time, WWD_STA_INTERFACE ));
    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_SCAN_HOME_TIME, home_time, WWD_STA_INTERFACE ));
    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_SCAN_NPROBES, nprobes, WWD_STA_INTERFACE ));
    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_get_scan_params( uint32_t* assoc_time,
                                       uint32_t* unassoc_time,
                                       uint32_t* passive_time,
                                       uint32_t* home_time,
                                       uint32_t* nprobes )
{
    CHECK_RETURN( wwd_wifi_get_iovar_value( IOVAR_STR_SCAN_ASSOC_TIME, assoc_time, WWD_STA_INTERFACE ));
    CHECK_RETURN( wwd_wifi_get_iovar_value( IOVAR_STR_SCAN_UNASSOC_TIME, unassoc_time, WWD_STA_INTERFACE ));
    CHECK_RETURN( wwd_wifi_get_iovar_value( IOVAR_STR_SCAN_PASSIVE_TIME, passive_time, WWD_STA_INTERFACE ));
    CHECK_RETURN( wwd_wifi_get_iovar_value( IOVAR_STR_SCAN_HOME_TIME, home_time, WWD_STA_INTERFACE ));
    CHECK_RETURN( wwd_wifi_get_iovar_value( IOVAR_STR_SCAN_NPROBES, nprobes, WWD_STA_INTERFACE ));
    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_join_halt( wiced_bool_t join_halt )
{
    if ( join_halt == join_halt_triggered )
    {
        return WWD_SUCCESS;
    }

    join_halt_triggered = join_halt;

    /* if halt is wanted and a join has gone through init process (in progress) */
    if ( join_halt_triggered == WICED_TRUE && active_join_mutex_initted == WICED_TRUE )
    {
        /* check if an active wait for join is happening (or about to); if so, release the semaphore */
        host_rtos_get_semaphore( &active_join_mutex, NEVER_TIMEOUT, WICED_FALSE );
        if ( active_join_semaphore != NULL )
        {
            host_rtos_set_semaphore( active_join_semaphore, WICED_FALSE );
        }
        /* else - halt before going into the wait state */
        host_rtos_set_semaphore( &active_join_mutex, WICED_FALSE );
    }
    /* else - halt not wanted; no-op for now, OR join hasn't gone through init: code in join path does halt */

    return WWD_SUCCESS;
}

/* Do any needed preparation prior to launching a join */
static wwd_result_t wwd_wifi_active_join_init( wiced_security_t auth_type, const uint8_t* security_key, uint8_t key_length, host_semaphore_type_t *semaphore, wwd_interface_t interface)
{
    wwd_result_t result = WWD_SUCCESS;
    if ( active_join_mutex_initted == WICED_FALSE )
    {
        CHECK_RETURN( host_rtos_init_semaphore( &active_join_mutex ) );
        active_join_mutex_initted = WICED_TRUE;
        host_rtos_set_semaphore( &active_join_mutex, WICED_FALSE );
    }

    host_rtos_get_semaphore( &active_join_mutex, NEVER_TIMEOUT, WICED_FALSE );
    active_join_semaphore = semaphore;
    host_rtos_set_semaphore( &active_join_mutex, WICED_FALSE );

    /* a halt was set off, perhaps while waiting for mutex above; catch it here and bail */
    if ( join_halt_triggered == WICED_TRUE )
    {
        return WWD_UNFINISHED;
    }

    result = wwd_wifi_prepare_join( interface, auth_type, security_key, key_length, semaphore );
    return result;
}

/* do any needed tear down after join
 * @param stack_semaphore - semaphore used to control execution if client_semaphore is NULL
 * @param client_semaphore - semaphore used to control execution if client passes this in
*/
static void wwd_wifi_active_join_deinit( host_semaphore_type_t *stack_semaphore, wwd_result_t result, host_semaphore_type_t *client_semaphore )
{
    /* deinit join specific variables, with protection from mutex */
    host_rtos_get_semaphore( &active_join_mutex, NEVER_TIMEOUT, WICED_FALSE );
    active_join_semaphore = NULL;

    /* Only deinit the semaphore if the client semaphore wasn't specified; when it is specified, stack semaphore won't be initted */
    if ( client_semaphore == NULL )
    {
        host_rtos_deinit_semaphore( stack_semaphore );
    }

    if ( WWD_SUCCESS != result )
    {
        WPRINT_WWD_INFO(("Failed join (err %d)\n", result));
    }

    host_rtos_set_semaphore( &active_join_mutex, WICED_FALSE );

    /* we forced the chip to be up during join, now let it sleep */
    WWD_WLAN_LET_SLEEP( );

}

wwd_result_t wwd_wifi_join( const wiced_ssid_t* ssid, wiced_security_t auth_type, const uint8_t* security_key, uint8_t key_length, host_semaphore_type_t* semaphore, wwd_interface_t interface )
{
    host_semaphore_type_t join_sema;
    wwd_result_t          result;
    wiced_buffer_t        buffer;
    wlc_ssid_t*           ssid_params;

    if ( ssid->length > (size_t) SSID_NAME_SIZE )
    {
        WPRINT_WWD_ERROR(("%s: failure: SSID too long\n", __func__));
        return WWD_WLAN_BADSSIDLEN;
    }

    if ( join_halt_triggered == WICED_TRUE )
    {
        return WWD_UNFINISHED;
    }

    /* Keep WLAN awake while joining */
    WWD_WLAN_KEEP_AWAKE( );

    if ( wwd_wifi_set_block_ack_window_size( interface ) != WWD_SUCCESS )
    {
        WPRINT_WWD_ERROR(("%s: failed to set ba win\n", __func__));
        WWD_WLAN_LET_SLEEP( );
        return WWD_SET_BLOCK_ACK_WINDOW_FAIL;
    }

    if ( semaphore == NULL )
    {
        CHECK_RETURN( host_rtos_init_semaphore( &join_sema ) );
        result = wwd_wifi_active_join_init( auth_type, security_key, key_length, &join_sema, interface );
    }
    else
    {
        result = wwd_wifi_active_join_init( auth_type, security_key, key_length, semaphore, interface );
    }

    if ( result == WWD_SUCCESS )
    {
        /* Join network */
        ssid_params = (struct wlc_ssid *) wwd_sdpcm_get_ioctl_buffer( &buffer, sizeof(wlc_ssid_t) );
        CHECK_IOCTL_BUFFER( ssid_params );
        memset( ssid_params, 0, sizeof(wlc_ssid_t) );
        ssid_params->SSID_len = ssid->length;
        memcpy( ssid_params->SSID, ssid->value, ssid_params->SSID_len );
        result = wwd_sdpcm_send_ioctl( SDPCM_SET, WLC_SET_SSID, buffer, 0, interface );

        if ( result == WWD_SUCCESS && semaphore == NULL )
        {
            result = host_rtos_get_semaphore( &join_sema, DEFAULT_JOIN_ATTEMPT_TIMEOUT, WICED_FALSE );
            wiced_assert( "Get semaphore failed", ( result == WWD_SUCCESS ) || ( result == WWD_TIMEOUT ) );
            REFERENCE_DEBUG_ONLY_VARIABLE( result );

            result = wwd_wifi_is_ready_to_transceive( interface );
            if ( result != WWD_SUCCESS )
            {
                wwd_wifi_leave( interface );
                WPRINT_WWD_INFO(("%s: not ready to transceive (err %d); left network\n", __func__, result));
            }


#ifdef DOT11AC_CHIP
            /* For 11 AC MAX throughput set the frame burst and MPDU per AMPDU
             *
             */
            CHECK_RETURN( wwd_wifi_set_ioctl_value( WLC_SET_FAKEFRAG, 1, WWD_STA_INTERFACE ) );
            CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_MPDU_PER_AMPDU, 64, WWD_STA_INTERFACE ) );
#endif
        }
    }

    /* clean up from the join attempt */
    wwd_wifi_active_join_deinit( &join_sema, result, semaphore );

    return result;
}


wwd_result_t wwd_wifi_join_specific( const wiced_scan_result_t* ap, const uint8_t* security_key, uint8_t key_length, host_semaphore_type_t* semaphore, wwd_interface_t interface )
{
    wiced_buffer_t        buffer;
    host_semaphore_type_t join_semaphore;
    wwd_result_t          result;
    wl_extjoin_params_t*  ext_join_params;
    wl_join_params_t*     join_params;
    wiced_security_t      security            = ap->security;
    uint16_t              wl_band_for_channel = 0;
    /* Keep WLAN awake while joining */
    WWD_WLAN_KEEP_AWAKE( );

    if ( wwd_wifi_set_block_ack_window_size( interface ) != WWD_SUCCESS )
    {
        WWD_WLAN_LET_SLEEP( );
        WPRINT_WWD_INFO(("%s: ba set failed\n", __FUNCTION__));
        return WWD_SET_BLOCK_ACK_WINDOW_FAIL;
    }

    if ( ap->bss_type == WICED_BSS_TYPE_ADHOC )
    {
        security |= IBSS_ENABLED;
    }

    if ( semaphore == NULL )
    {
        CHECK_RETURN( host_rtos_init_semaphore( &join_semaphore ) );
        result = wwd_wifi_active_join_init( security, security_key, key_length, &join_semaphore, interface);
    }
    else
    {
        result = wwd_wifi_active_join_init( security, security_key, key_length, semaphore, interface);
    }

    if ( result == WWD_SUCCESS )
    {
        /* Check if soft AP is running, if so, move its current channel to the the destination AP */
        if ( wwd_wifi_is_ready_to_transceive( WWD_AP_INTERFACE ) == WWD_SUCCESS )
        {
            uint32_t current_softap_channel = 0;
            wwd_wifi_get_channel( WWD_AP_INTERFACE, &current_softap_channel );
            if ( current_softap_channel != ap->channel )
            {
                wwd_wifi_set_channel( WWD_AP_INTERFACE, ap->channel );
                WPRINT_WWD_DEBUG(("WARN: moving soft-AP channel from %lu to %d due to STA join\n", current_softap_channel, ap->channel));
                host_rtos_delay_milliseconds( 100 );
            }
        }
        else
        {
            if (ap->bss_type == WICED_BSS_TYPE_ADHOC)
                wwd_wifi_set_channel( WWD_STA_INTERFACE, ap->channel );
        }

        /* Join network */
        ext_join_params = (wl_extjoin_params_t*) wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(wl_extjoin_params_t), "join" );
        CHECK_IOCTL_BUFFER( ext_join_params );
        memset( ext_join_params, 0, sizeof(wl_extjoin_params_t) );

        ext_join_params->ssid.SSID_len = ap->SSID.length;
        memcpy( ext_join_params->ssid.SSID, ap->SSID.value, ext_join_params->ssid.SSID_len );
        memcpy( &ext_join_params->assoc_params.bssid, &ap->BSSID, sizeof(wiced_mac_t) );
        ext_join_params->scan_params.scan_type    = 0;
        ext_join_params->scan_params.active_time  = -1;
        ext_join_params->scan_params.home_time    = -1;
        ext_join_params->scan_params.nprobes      = -1;
        ext_join_params->scan_params.passive_time = -1;
#ifdef CHIP_HAS_BSSID_CNT_IN_ASSOC_PARAMS
        ext_join_params->assoc_params.bssid_cnt = 0;
#endif /* ifdef CHIP_HAS_BSSID_CNT_IN_ASSOC_PARAMS */
        ext_join_params->assoc_params.chanspec_num = (uint32_t) 1;
        ext_join_params->assoc_params.chanspec_list[0] = (wl_chanspec_t) htod16((ap->channel | WL_CHANSPEC_BW_20 | WL_CHANSPEC_CTL_SB_NONE));

        /* set band properly */
        wl_band_for_channel = wwd_channel_to_wl_band( ap->channel );

        ext_join_params->assoc_params.chanspec_list[0] |= wl_band_for_channel;

        result = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, interface );

        WPRINT_WWD_INFO(("%s: set_ssid result (err %d); left network\n", __func__, result));

        /* Some firmware, e.g. for 4390, does not support the join IOVAR, so use the older IOCTL call instead */
        if ( result == WWD_WLAN_UNSUPPORTED )
        {
            join_params = (wl_join_params_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, sizeof(wl_join_params_t) );
            CHECK_IOCTL_BUFFER( join_params );
            memset( join_params, 0, sizeof(wl_join_params_t) );
            memcpy( &join_params->ssid, &ext_join_params->ssid, sizeof ( wlc_ssid_t ) );
            memcpy( &join_params->params.bssid, &ap->BSSID, sizeof(wiced_mac_t) );
#ifdef CHIP_HAS_BSSID_CNT_IN_ASSOC_PARAMS
            join_params->params.bssid_cnt        = 0;
#endif /* ifdef CHIP_HAS_BSSID_CNT_IN_ASSOC_PARAMS */
            join_params->params.chanspec_num     = (uint32_t) 1;
            join_params->params.chanspec_list[0] = (wl_chanspec_t) htod16((ap->channel | WL_CHANSPEC_BW_20 | WL_CHANSPEC_CTL_SB_NONE));

            /* set band properly */
            join_params->params.chanspec_list[0] |= wl_band_for_channel;

            result = wwd_sdpcm_send_ioctl( SDPCM_SET, WLC_SET_SSID, buffer, 0, interface );
        }

        if ( result == WWD_SUCCESS && semaphore == NULL )
        {
            host_rtos_get_semaphore( &join_semaphore, (uint32_t) DEFAULT_JOIN_ATTEMPT_TIMEOUT, WICED_FALSE );
            result = wwd_wifi_is_ready_to_transceive( interface );
            if ( result != WWD_SUCCESS )
            {
                wwd_wifi_leave( interface );
                WPRINT_WWD_INFO(("%s:2 not ready to transceive (err %d); left network\n", __func__, result));
            }
        }
        else
        {
            WPRINT_WWD_INFO(("%s:3 not ready to transceive (err %d); left network\n", __func__, result));
        }
    }
    else
    {
        WPRINT_WWD_INFO(("%s: active join init failed: (%d)\n", __FUNCTION__, result));
    }
    /* clean up from the join attempt */
    wwd_wifi_active_join_deinit( &join_semaphore, result, semaphore );

    return result;
}

static wwd_result_t wwd_wifi_prepare_join( wwd_interface_t interface, wiced_security_t auth_type, /*@unique@*/ const uint8_t* security_key, uint8_t key_length, host_semaphore_type_t* semaphore )
{
    wiced_buffer_t buffer;
    uint32_t auth_mfp = WL_MFP_NONE;
    wwd_result_t   retval = WWD_SUCCESS;
    wiced_result_t check_result = WICED_SUCCESS;
    uint16_t       a;
    uint32_t*      data;
    uint32_t*      wpa_auth;
    uint32_t       bss_index = 0;
    uint32_t       auth;
    (void)bss_index;

    if ( ( ( ( key_length > (uint8_t) WSEC_MAX_PSK_LEN ) ||
             ( key_length < (uint8_t) WSEC_MIN_PSK_LEN ) ) &&
           ( ( auth_type == WICED_SECURITY_WPA_TKIP_PSK ) ||
             ( auth_type == WICED_SECURITY_WPA_AES_PSK ) ||
             ( auth_type == WICED_SECURITY_WPA2_AES_PSK ) ||
             ( auth_type == WICED_SECURITY_WPA2_TKIP_PSK ) ||
             ( auth_type == WICED_SECURITY_WPA2_MIXED_PSK ) ) ) ||
           ( (key_length > (uint8_t) WSEC_MAX_SAE_PASSWORD_LEN) &&
             ( ( auth_type == WICED_SECURITY_WPA3_SAE) ||
               ( auth_type == WICED_SECURITY_WPA3_WPA2_PSK ) ) ) )
    {
        return WWD_INVALID_KEY;
    }

    (void)interface, (void)auth_type, (void)security_key, (void)key_length, (void)semaphore;
    if ( !( ( interface == WWD_STA_INTERFACE ) || ( ( interface == WWD_P2P_INTERFACE ) && ( wwd_wifi_p2p_go_is_up == WICED_FALSE ) )
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
        ||  (interface == WWD_AP_INTERFACE)
#endif
        ) )
    {
        WPRINT_APP_INFO (( "%s: INVALID INTERFACE\n", __FUNCTION__));
        return WWD_INVALID_INTERFACE;
    }

    /* Clear the current join status */
    wiced_join_status[ interface ] = 0;

    /* Setting wsec will overwrite mfp setting in older branches, store value before setting wsec */
    wwd_wifi_get_iovar_value ( IOVAR_STR_MFP, &auth_mfp, WWD_STA_INTERFACE );

    /* Set Wireless Security Type */
    CHECK_RETURN( wwd_wifi_set_ioctl_value( WLC_SET_WSEC, (uint32_t) ( ( auth_type & 0xFF ) & ~WPS_ENABLED), interface ) );

    /* Map the interface to a BSS index */
    bss_index = wwd_get_bss_index( interface );

    /* Set supplicant variable - mfg app doesn't support these iovars, so don't care if return fails */
    data = wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 8, "bsscfg:" IOVAR_STR_SUP_WPA );
    CHECK_IOCTL_BUFFER( data );
    data[0] = bss_index;
    data[1] = (uint32_t) ( ( ( ( auth_type & WPA_SECURITY )  != 0 ) ||
                           ( ( auth_type & WPA2_SECURITY ) != 0 ) ||
                             ( auth_type & WPA3_SECURITY ) != 0 ) ? 1 : 0 );
    (void)wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );

    /* Set the EAPOL version to whatever the AP is using (-1) */
    data = wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 8, "bsscfg:" IOVAR_STR_SUP_WPA2_EAPVER );
    CHECK_IOCTL_BUFFER( data );
    data[0] = bss_index;
    data[1] = (uint32_t)-1;
    (void)wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );

    /* Send WPA Key */
    switch ( auth_type )
    {
        case WICED_SECURITY_OPEN:
        case WICED_SECURITY_IBSS_OPEN:
        case WICED_SECURITY_WPS_OPEN:
        case WICED_SECURITY_WPS_SECURE:
            break;
        case WICED_SECURITY_WPA_TKIP_PSK:
        case WICED_SECURITY_WPA_AES_PSK:
        case WICED_SECURITY_WPA_MIXED_PSK:
        case WICED_SECURITY_WPA2_AES_PSK:
        case WICED_SECURITY_WPA2_TKIP_PSK:
        case WICED_SECURITY_WPA2_MIXED_PSK:
        case WICED_SECURITY_WPA2_FBT_PSK:
            /* Set the EAPOL key packet timeout value, otherwise unsuccessful supplicant events aren't reported. If the IOVAR is unsupported then continue. */
            CHECK_RETURN_UNSUPPORTED_CONTINUE( wwd_wifi_set_supplicant_eapol_key_timeout( interface, DEFAULT_EAPOL_KEY_PACKET_TIMEOUT ) );
            CHECK_RETURN( wwd_wifi_set_passphrase( security_key, key_length, interface ) );
            break;
        case WICED_SECURITY_WPA3_SAE:
        case WICED_SECURITY_WPA3_WPA2_PSK:
            /* Set the EAPOL key packet timeout value, otherwise unsuccessful supplicant events aren't reported. If the IOVAR is unsupported then continue. */
            CHECK_RETURN_UNSUPPORTED_CONTINUE( wwd_wifi_set_supplicant_eapol_key_timeout( interface, DEFAULT_EAPOL_KEY_PACKET_TIMEOUT ) );
            CHECK_RETURN( wwd_wifi_sae_password(security_key, key_length, interface) );
            break;

        case WICED_SECURITY_WPA_TKIP_ENT:
        case WICED_SECURITY_WPA_AES_ENT:
        case WICED_SECURITY_WPA_MIXED_ENT:
        case WICED_SECURITY_WPA2_TKIP_ENT:
        case WICED_SECURITY_WPA2_AES_ENT:
        case WICED_SECURITY_WPA2_MIXED_ENT:
        case WICED_SECURITY_WPA2_FBT_ENT:
            /* Disable eapol timer by setting to value 0 */
            CHECK_RETURN_UNSUPPORTED_CONTINUE( wwd_wifi_set_supplicant_eapol_key_timeout( interface, 0 ) );
            break;

        case WICED_SECURITY_WEP_PSK:
        case WICED_SECURITY_WEP_SHARED:
            for ( a = 0; a < key_length; a = (uint16_t) ( a + 2 + security_key[1] ) )
            {
                const wiced_wep_key_t* in_key = (const wiced_wep_key_t*) &security_key[a];
                wl_wsec_key_t* out_key = (wl_wsec_key_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, sizeof(wl_wsec_key_t) );
                CHECK_IOCTL_BUFFER( out_key );
                memset( out_key, 0, sizeof(wl_wsec_key_t) );
                out_key->index = in_key->index;
                out_key->len = in_key->length;
                memcpy( out_key->data, in_key->data, in_key->length );
                switch ( in_key->length )
                {
                    case 5:
                        out_key->algo = (uint32_t) CRYPTO_ALGO_WEP1;
                        break;
                    case 13:
                        out_key->algo = (uint32_t)CRYPTO_ALGO_WEP128;
                        break;
                    case 16:
                        /* default to AES-CCM */
                        out_key->algo = (uint32_t) CRYPTO_ALGO_AES_CCM;
                        break;
                    case 32:
                        out_key->algo = (uint32_t) CRYPTO_ALGO_TKIP;
                        break;
                    default:
                        host_buffer_release(buffer, WWD_NETWORK_TX);
                        return WWD_INVALID_KEY;
                }
                /* Set the first entry as primary key by default */
                if ( a == 0 )
                {
                    out_key->flags |= WL_PRIMARY_KEY;
                }
                out_key->index = htod32(out_key->index);
                out_key->len   = htod32(out_key->len);
                out_key->algo  = htod32(out_key->algo);
                out_key->flags = htod32(out_key->flags);
                CHECK_RETURN( wwd_sdpcm_send_ioctl( SDPCM_SET, WLC_SET_KEY, buffer, NULL, interface ) );
            }
            break;
        case WICED_SECURITY_FORCE_32_BIT:
        case WICED_SECURITY_UNKNOWN:
        default:
            wiced_assert("wiced_wifi_prepare_join: Unsupported security type\n", 0 != 0 );
            break;
    }
    /* Set infrastructure mode */
    CHECK_RETURN( wwd_wifi_set_ioctl_value( WLC_SET_INFRA, ( ( auth_type & IBSS_ENABLED ) == 0 ) ? 1 : 0, interface ) );

    /* Set authentication type */
    if ( auth_type == WICED_SECURITY_WEP_SHARED )
    {
        auth = WL_AUTH_SHARED_KEY;
    }
    else if ( auth_type == WICED_SECURITY_WPA3_SAE || auth_type == WICED_SECURITY_WPA3_WPA2_PSK )
    {
        auth = WL_AUTH_SAE;
    }
    else
    {
        auth = WL_AUTH_OPEN_SYSTEM;
    }
    CHECK_RETURN( wwd_wifi_set_ioctl_value( WLC_SET_AUTH, auth, interface ) );

    /* From PMF cert test plan,
     * 2.2 Out of Box Requirements
     * When WPA2 security is enabled on the DUT, then by defaults the DUT shall:
     * Enable Robust Management Frame Protection Capable (MFPC) functionality
     */
    if ( auth_type == WICED_SECURITY_WPA3_SAE || auth_type == WICED_SECURITY_WPA3_WPA2_PSK || auth_type & WPA2_SECURITY )
    {
        auth_mfp = WL_MFP_CAPABLE;
    }

    check_result = wwd_wifi_set_iovar_value ( IOVAR_STR_MFP, auth_mfp , WWD_STA_INTERFACE );
    if ( check_result != WICED_SUCCESS )
    {
        WPRINT_WWD_DEBUG( ( "Older chipsets might not support MFP..Ignore result\n" ) );
    }

    /* Set WPA authentication mode */
    wpa_auth = (uint32_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, (uint16_t) 4 );
    CHECK_IOCTL_BUFFER( wpa_auth );

    switch ( auth_type )
    {
        case WICED_SECURITY_IBSS_OPEN:
            wiced_join_status[interface] |= JOIN_AUTHENTICATED;  /* IBSS does not get authenticated onto an AP */
            /* intentional fall-thru */
            /* Disables Eclipse static analysis warning */
            /* no break */
            /* Fall-Through */
        case WICED_SECURITY_OPEN:
        case WICED_SECURITY_WPS_OPEN:
        case WICED_SECURITY_WPS_SECURE:
            *wpa_auth = WPA_AUTH_DISABLED;
            wiced_join_status[interface] |= JOIN_SECURITY_COMPLETE;  /* Open Networks do not have to complete security */
            break;
        case WICED_SECURITY_WPA_TKIP_PSK:
        case WICED_SECURITY_WPA_AES_PSK:
        case WICED_SECURITY_WPA_MIXED_PSK:
            *wpa_auth = (uint32_t) WPA_AUTH_PSK;
            break;
        case WICED_SECURITY_WPA2_AES_PSK:
        case WICED_SECURITY_WPA2_TKIP_PSK:
        case WICED_SECURITY_WPA2_MIXED_PSK:
            *wpa_auth = (uint32_t) WPA2_AUTH_PSK;
            break;
        case WICED_SECURITY_WPA2_FBT_PSK:
            *wpa_auth = (uint32_t) (WPA2_AUTH_PSK | WPA2_AUTH_FT);
            break;
        case WICED_SECURITY_WPA3_SAE:
        case WICED_SECURITY_WPA3_WPA2_PSK:
            *wpa_auth = (uint32_t) WPA3_AUTH_SAE_PSK;
            break;
        case WICED_SECURITY_WPA_TKIP_ENT:
        case WICED_SECURITY_WPA_AES_ENT:
        case WICED_SECURITY_WPA_MIXED_ENT:
            *wpa_auth = (uint32_t) WPA_AUTH_UNSPECIFIED;
            break;

        case WICED_SECURITY_WPA2_TKIP_ENT:
        case WICED_SECURITY_WPA2_AES_ENT:
        case WICED_SECURITY_WPA2_MIXED_ENT:
            *wpa_auth = (uint32_t) WPA2_AUTH_UNSPECIFIED;
            break;
        case WICED_SECURITY_WPA2_FBT_ENT:
            *wpa_auth = (uint32_t) (WPA2_AUTH_UNSPECIFIED | WPA2_AUTH_FT);
            break;

        case WICED_SECURITY_WEP_PSK:
        case WICED_SECURITY_WEP_SHARED:
            *wpa_auth = WPA_AUTH_DISABLED;
            wiced_join_status[interface] |= JOIN_SECURITY_COMPLETE;
            break;
        case WICED_SECURITY_UNKNOWN:
        case WICED_SECURITY_FORCE_32_BIT:
        default:
            WPRINT_WWD_DEBUG(("Unsupported Security type\n"));
            *wpa_auth = WPA_AUTH_DISABLED;
            break;
    }

    CHECK_RETURN( wwd_sdpcm_send_ioctl( SDPCM_SET, WLC_SET_WPA_AUTH, buffer, 0, interface ) );

    retval = wwd_management_set_event_handler( join_events, wiced_join_events_handler, (void*) semaphore, interface );
    wiced_assert("Set join Event handler failed\n", retval == WWD_SUCCESS );

    return retval;
}

wwd_result_t wwd_wifi_leave( wwd_interface_t interface )
{
    wwd_result_t   result = WWD_SUCCESS;

    /* If interface is greater than max return error */
    if (interface >= WWD_INTERFACE_MAX) {
          WPRINT_APP_INFO(("%s: Bad interface 2\n", __FUNCTION__));
          return WWD_BADARG;
    }
    CHECK_RETURN( wwd_management_set_event_handler( join_events, NULL, NULL, interface ) );

    /* Disassociate from AP */
    result = wwd_wifi_set_ioctl_void( WLC_DISASSOC, interface );

    if ( result != WWD_SUCCESS )
    {
        WPRINT_WWD_DEBUG(( "wwd_sdpcm_send_ioctl(WLC_DISASSOC) failed:%d\r\n", result ));
    }

    wiced_join_status[ interface ] = 0;

    wwd_wifi_link_update( );

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_deauth_sta( const wiced_mac_t* mac, wwd_dot11_reason_code_t reason, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    wwd_result_t result;
    scb_val_t* scb_val;

    scb_val = (scb_val_t *) wwd_sdpcm_get_ioctl_buffer( &buffer, sizeof(scb_val_t) );
    CHECK_IOCTL_BUFFER( scb_val );
    memset((char *)scb_val, 0, sizeof(scb_val_t));
    memcpy((char *)&scb_val->ea, (char *) mac, sizeof(wiced_mac_t));
    scb_val->val = (uint32_t)reason;
    result = wwd_sdpcm_send_ioctl( SDPCM_SET, WLC_SCB_DEAUTHENTICATE_FOR_REASON, buffer, 0, interface );

    return result;
}

wwd_result_t wwd_wifi_deauth_all_associated_client_stas( wwd_dot11_reason_code_t reason, wwd_interface_t interface )
{
    uint8_t*           buffer      = NULL;
    wiced_maclist_t*   clients     = NULL;
    const wiced_mac_t* current;
    wwd_result_t       result;
    wl_bss_info_t      ap_info;
    wiced_security_t   sec;
    uint32_t           max_clients = 0;
    size_t             size        = 0;

    result = wwd_wifi_get_max_associations( &max_clients );
    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO( ("Failed to get max number of associated clients\n") );
        max_clients = 5;
    }

    size = ( sizeof(uint32_t) + (max_clients * sizeof(wiced_mac_t)));
    buffer = calloc( 1, size );

    if ( buffer == NULL )
    {
        WPRINT_APP_INFO(( "Unable to allocate memory for associated clients list\n" ));
        return WWD_MALLOC_FAILURE;
    }

    clients = (wiced_maclist_t*)buffer;
    clients->count = max_clients;
    memset(&ap_info, 0, sizeof(wl_bss_info_t));

    result = wwd_wifi_get_associated_client_list( clients, ( uint16_t )size );
    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(( "Failed to get client list\n" ));
        free( buffer );
        return result;
    }

    current = &clients->mac_list[0];
    wwd_wifi_get_ap_info( &ap_info, &sec );

    while ((clients->count > 0) && (!(NULL_MAC(current->octet))))
    {
        if (memcmp(current->octet, &(ap_info.BSSID), sizeof(wiced_mac_t) ) != 0)
        {
            WPRINT_APP_INFO(("Deauthenticating STA MAC: %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n", current->octet[0], current->octet[1], current->octet[2], current->octet[3], current->octet[4], current->octet[5]));
            result = wwd_wifi_deauth_sta(current, reason, interface );
            if ( result  != WWD_SUCCESS)
            {
                WPRINT_APP_INFO(("Failed to deauth client\n"));
            }
        }

        --clients->count;
        ++current;
    }

    free( buffer );

    return WWD_SUCCESS;
}

/* given a list of channels, return the cca score for each channel */
wwd_result_t
wwd_wifi_get_cca_for_channel(uint32_t *channels, uint32_t duration, uint8_t *scores, uint32_t nchans)
{
    wiced_buffer_t req_buffer;
    wiced_buffer_t rep_buffer;
    wiced_buffer_t response;
    wl_rm_req_t *rm_req;
    wl_rm_rep_t *rm_rep;
    wl_rm_rep_elt_t *rep_elt;
    uint32_t* rep_data;
    int err;
    int i;
    int len;
    uint8_t *data;
    uint16_t buflen;

    /* Fixed report + report_element for each channel, used for req and reports */
    buflen = (uint16_t)(sizeof(wl_rm_rep_t) + (nchans * sizeof(wl_rm_rep_elt_t)));

    /*
     * Alloc & populate request
     */
    rm_req = (wl_rm_req_t *)wwd_sdpcm_get_iovar_buffer(&req_buffer, buflen, IOVAR_STR_RM_REQ);
    if (rm_req == NULL) {
        WPRINT_APP_INFO(("%s:Unable to alloc request buffer\n", __FUNCTION__));
        return WWD_WLAN_ERROR;
    }

    memset(rm_req, 0, buflen);

    for (i = 0; i < (int)nchans; i++) {
        rm_req->req[i].type = WL_RM_TYPE_CCA;
        rm_req->req[i].dur = duration;
        rm_req->req[i].chanspec = CH20MHZ_CHSPEC(channels[i]);
    }
    rm_req->count = nchans;

    err = wwd_sdpcm_send_iovar(SDPCM_SET, req_buffer, &response, WWD_STA_INTERFACE);
    if (err != WWD_SUCCESS) {
        WPRINT_APP_INFO(("%s: send_iovar req_buf FAIL %d\n", __FUNCTION__, err));
        return WWD_WLAN_ERROR;
    }
    host_buffer_release(response, WWD_NETWORK_RX);

    /*
     * Delay 1 default beacon period per chan measurment
     * to wait for measurments to complete
     */
    host_rtos_delay_milliseconds((nchans * 100));

    /*
     * Get the results
     */
    rep_data = (uint32_t*)wwd_sdpcm_get_iovar_buffer(&rep_buffer, buflen, IOVAR_STR_RM_REP);
    if (rep_data == NULL) {
        WPRINT_APP_INFO(("%s: Failed to get IOVAR buf for rm_rep.\n", __FUNCTION__));
        return WWD_WLAN_ERROR;
    }

    err = wwd_sdpcm_send_iovar(SDPCM_GET, rep_buffer, &response, WWD_STA_INTERFACE);
    if (err != WWD_SUCCESS) {
        WPRINT_APP_INFO(("%s: Can't get report buffer (%d)\n", __FUNCTION__, err));
        return WWD_WLAN_ERROR;
    }

    /*
     * Parse results
     */
    rm_rep = (wl_rm_rep_t *)host_buffer_get_current_piece_data_pointer(response);
    len = (int)rm_rep->len;
    data = (uint8_t*)rm_rep->rep;

    rep_elt = (wl_rm_rep_elt_t *)data;

    /* Sanity check */
    if (rep_elt->len != 4) {
        WPRINT_APP_INFO(("%s: rep_elt.len unexpected len (%lu)\n", __FUNCTION__, rep_elt->len));
        goto fail;
    }

    for (i=0; len > 0 && i < (int)nchans; i++, len -= (int)rep_elt->len, data += rep_elt->len) {
        data += WL_RM_REP_ELT_FIXED_LEN;
        len -= WL_RM_REP_ELT_FIXED_LEN;

        /*
         * Sanity check the results
         */
        if (rm_rep->rep[i].flags) {
            WPRINT_APP_INFO(("%s: Chan %d: Flags 0x%x\n", __FUNCTION__, rm_rep->rep[i].chanspec & 0xff, rm_rep->rep[i].flags));
            goto fail;
        }
        if (rm_rep->rep[i].dur != duration) {
            WPRINT_APP_INFO(("%s: Chan %d: Duration %ld != expected %ld\n", __FUNCTION__, rm_rep->rep[i].chanspec & 0xff, rm_rep->rep[i].dur, duration));
            goto fail;
        }

        /* results need to be in same order we requested so 'scores' will be the same order as 'channels' */
        if ((channels[i] & 0xff) != (rm_rep->rep[i].chanspec & 0xff)) {
            WPRINT_APP_INFO(("%s: Channel mismatch: Sent %lu, Rx'ed %d\n", __FUNCTION__, channels[i], rm_rep->rep[i].chanspec & 0xff));
            goto fail;
        }

        /* Update results */
        scores[i] = rm_rep->rep[i].data[0];
    }

    if (i != (int)nchans) {
        WPRINT_APP_INFO(("%s: Requested channels %ld chans only got %d\n", __FUNCTION__, nchans, i));
        goto fail;
    }

    host_buffer_release(response, WWD_NETWORK_RX);
    return WWD_SUCCESS;

fail:
    host_buffer_release(response, WWD_NETWORK_RX);
    return WWD_WLAN_ERROR;
}

#ifdef WPRINT_ENABLE_WWD_DEBUG
static char* wwd_ssid_to_string( uint8_t *value, uint8_t length, char *ssid_buf, uint8_t ssid_buf_len )
{
    memset(ssid_buf, 0, ssid_buf_len);

    if (ssid_buf_len > 0)
    {
        memcpy(ssid_buf, value, ssid_buf_len < length ? ssid_buf_len : length);
    }

    return ssid_buf;
}

/* When adding new events, update this switch statement to print correct string */
#define CASE_RETURN_STRING(value) case value: return #value;
#define CASE_RETURN(value) case value: break;

static const char* wwd_event_to_string( wwd_event_num_t value )
{
    switch ( value )
    {
        CASE_RETURN_STRING(WLC_E_ULP)
        CASE_RETURN(WLC_E_BT_WIFI_HANDOVER_REQ)
        CASE_RETURN(WLC_E_SPW_TXINHIBIT)
        CASE_RETURN(WLC_E_FBT_AUTH_REQ_IND)
        CASE_RETURN(WLC_E_RSSI_LQM)
        CASE_RETURN(WLC_E_PFN_GSCAN_FULL_RESULT)
        CASE_RETURN(WLC_E_PFN_SWC)
        CASE_RETURN(WLC_E_AUTHORIZED)
        CASE_RETURN(WLC_E_PROBREQ_MSG_RX)
        CASE_RETURN(WLC_E_RMC_EVENT)
        CASE_RETURN(WLC_E_DPSTA_INTF_IND)
        CASE_RETURN_STRING(WLC_E_NONE)
        CASE_RETURN_STRING(WLC_E_SET_SSID)
        CASE_RETURN(WLC_E_PFN_BEST_BATCHING)
        CASE_RETURN(WLC_E_JOIN)
        CASE_RETURN(WLC_E_START)
        CASE_RETURN_STRING(WLC_E_AUTH)
        CASE_RETURN(WLC_E_AUTH_IND)
        CASE_RETURN(WLC_E_DEAUTH)
        CASE_RETURN_STRING(WLC_E_DEAUTH_IND)
        CASE_RETURN(WLC_E_ASSOC)
        CASE_RETURN(WLC_E_ASSOC_IND)
        CASE_RETURN(WLC_E_REASSOC)
        CASE_RETURN(WLC_E_REASSOC_IND)
        CASE_RETURN(WLC_E_DISASSOC)
        CASE_RETURN_STRING(WLC_E_DISASSOC_IND)
        CASE_RETURN(WLC_E_ROAM)
        CASE_RETURN(WLC_E_ROAM_PREP)
        CASE_RETURN(WLC_E_ROAM_START)
        CASE_RETURN(WLC_E_QUIET_START)
        CASE_RETURN(WLC_E_QUIET_END)
        CASE_RETURN(WLC_E_BEACON_RX)
        CASE_RETURN_STRING(WLC_E_LINK)
        CASE_RETURN_STRING(WLC_E_RRM)
        CASE_RETURN(WLC_E_MIC_ERROR)
        CASE_RETURN(WLC_E_NDIS_LINK)
        CASE_RETURN(WLC_E_TXFAIL)
        CASE_RETURN(WLC_E_PMKID_CACHE)
        CASE_RETURN(WLC_E_RETROGRADE_TSF)
        CASE_RETURN(WLC_E_PRUNE)
        CASE_RETURN(WLC_E_AUTOAUTH)
        CASE_RETURN(WLC_E_EAPOL_MSG)
        CASE_RETURN(WLC_E_SCAN_COMPLETE)
        CASE_RETURN(WLC_E_ADDTS_IND)
        CASE_RETURN(WLC_E_DELTS_IND)
        CASE_RETURN(WLC_E_BCNSENT_IND)
        CASE_RETURN(WLC_E_BCNRX_MSG)
        CASE_RETURN(WLC_E_BCNLOST_MSG)
        CASE_RETURN_STRING(WLC_E_PFN_NET_FOUND)
        CASE_RETURN(WLC_E_PFN_NET_LOST)
        CASE_RETURN(WLC_E_RESET_COMPLETE)
        CASE_RETURN(WLC_E_JOIN_START)
        CASE_RETURN(WLC_E_ASSOC_START)
        CASE_RETURN(WLC_E_IBSS_ASSOC)
        CASE_RETURN(WLC_E_RADIO)
        CASE_RETURN(WLC_E_PSM_WATCHDOG)
        CASE_RETURN(WLC_E_CCX_ASSOC_START)
        CASE_RETURN(WLC_E_CCX_ASSOC_ABORT)
        CASE_RETURN(WLC_E_PROBREQ_MSG)
        CASE_RETURN(WLC_E_SCAN_CONFIRM_IND)
        CASE_RETURN_STRING(WLC_E_PSK_SUP)
        CASE_RETURN(WLC_E_COUNTRY_CODE_CHANGED)
        CASE_RETURN(WLC_E_EXCEEDED_MEDIUM_TIME)
        CASE_RETURN(WLC_E_ICV_ERROR)
        CASE_RETURN(WLC_E_UNICAST_DECODE_ERROR)
        CASE_RETURN(WLC_E_MULTICAST_DECODE_ERROR)
        CASE_RETURN(WLC_E_TRACE)
        CASE_RETURN(WLC_E_BTA_HCI_EVENT)
        CASE_RETURN(WLC_E_IF)
        CASE_RETURN(WLC_E_P2P_DISC_LISTEN_COMPLETE)
        CASE_RETURN(WLC_E_RSSI)
        CASE_RETURN_STRING(WLC_E_PFN_SCAN_COMPLETE)
        CASE_RETURN(WLC_E_EXTLOG_MSG)
        CASE_RETURN(WLC_E_ACTION_FRAME)
        CASE_RETURN(WLC_E_ACTION_FRAME_COMPLETE)
        CASE_RETURN(WLC_E_PRE_ASSOC_IND)
        CASE_RETURN(WLC_E_PRE_REASSOC_IND)
        CASE_RETURN(WLC_E_CHANNEL_ADOPTED)
        CASE_RETURN(WLC_E_AP_STARTED)
        CASE_RETURN(WLC_E_DFS_AP_STOP)
        CASE_RETURN(WLC_E_DFS_AP_RESUME)
        CASE_RETURN(WLC_E_WAI_STA_EVENT)
        CASE_RETURN(WLC_E_WAI_MSG)
        CASE_RETURN_STRING(WLC_E_ESCAN_RESULT)
        CASE_RETURN(WLC_E_ACTION_FRAME_OFF_CHAN_COMPLETE)
        CASE_RETURN(WLC_E_PROBRESP_MSG)
        CASE_RETURN(WLC_E_P2P_PROBREQ_MSG)
        CASE_RETURN(WLC_E_DCS_REQUEST)
        CASE_RETURN(WLC_E_FIFO_CREDIT_MAP)
        CASE_RETURN(WLC_E_ACTION_FRAME_RX)
        CASE_RETURN(WLC_E_WAKE_EVENT)
        CASE_RETURN(WLC_E_RM_COMPLETE)
        CASE_RETURN(WLC_E_HTSFSYNC)
        CASE_RETURN(WLC_E_OVERLAY_REQ)
        CASE_RETURN_STRING(WLC_E_CSA_COMPLETE_IND)
        CASE_RETURN(WLC_E_EXCESS_PM_WAKE_EVENT)
        CASE_RETURN(WLC_E_PFN_SCAN_NONE)
        CASE_RETURN(WLC_E_PFN_SCAN_ALLGONE)
        CASE_RETURN(WLC_E_GTK_PLUMBED)
        CASE_RETURN(WLC_E_ASSOC_IND_NDIS)
        CASE_RETURN(WLC_E_REASSOC_IND_NDIS)
        CASE_RETURN(WLC_E_ASSOC_REQ_IE)
        CASE_RETURN(WLC_E_ASSOC_RESP_IE)
        CASE_RETURN(WLC_E_ASSOC_RECREATED)
        CASE_RETURN(WLC_E_ACTION_FRAME_RX_NDIS)
        CASE_RETURN(WLC_E_AUTH_REQ)
        CASE_RETURN(WLC_E_TDLS_PEER_EVENT)
        CASE_RETURN(WLC_E_SPEEDY_RECREATE_FAIL)
        CASE_RETURN(WLC_E_NATIVE)
        CASE_RETURN(WLC_E_PKTDELAY_IND)
        CASE_RETURN(WLC_E_AWDL_AW)
        CASE_RETURN(WLC_E_AWDL_ROLE)
        CASE_RETURN(WLC_E_AWDL_EVENT)
        CASE_RETURN(WLC_E_NIC_AF_TXS)
        CASE_RETURN(WLC_E_NAN)
        CASE_RETURN(WLC_E_BEACON_FRAME_RX)
        CASE_RETURN(WLC_E_SERVICE_FOUND)
        CASE_RETURN(WLC_E_GAS_FRAGMENT_RX)
        CASE_RETURN(WLC_E_GAS_COMPLETE)
        CASE_RETURN(WLC_E_P2PO_ADD_DEVICE)
        CASE_RETURN(WLC_E_P2PO_DEL_DEVICE)
        CASE_RETURN(WLC_E_WNM_STA_SLEEP)
        CASE_RETURN(WLC_E_TXFAIL_THRESH)
        CASE_RETURN(WLC_E_PROXD)
        CASE_RETURN(WLC_E_IBSS_COALESCE)
        CASE_RETURN(WLC_E_AWDL_RX_PRB_RESP)
        CASE_RETURN(WLC_E_AWDL_RX_ACT_FRAME)
        CASE_RETURN(WLC_E_AWDL_WOWL_NULLPKT)
        CASE_RETURN(WLC_E_AWDL_PHYCAL_STATUS)
        CASE_RETURN(WLC_E_AWDL_OOB_AF_STATUS)
        CASE_RETURN(WLC_E_AWDL_SCAN_STATUS)
        CASE_RETURN(WLC_E_AWDL_AW_START)
        CASE_RETURN(WLC_E_AWDL_AW_END)
        CASE_RETURN(WLC_E_AWDL_AW_EXT)
        CASE_RETURN(WLC_E_AWDL_PEER_CACHE_CONTROL)
        CASE_RETURN(WLC_E_CSA_START_IND)
        CASE_RETURN(WLC_E_CSA_DONE_IND)
        CASE_RETURN(WLC_E_CSA_FAILURE_IND)
        CASE_RETURN(WLC_E_CCA_CHAN_QUAL)
        CASE_RETURN(WLC_E_BSSID)
        CASE_RETURN(WLC_E_TX_STAT_ERROR)
        CASE_RETURN(WLC_E_BCMC_CREDIT_SUPPORT)
        CASE_RETURN(WLC_E_PSTA_PRIMARY_INTF_IND)
        CASE_RETURN(WLC_E_FORCE_32_BIT)
        case WLC_E_LAST:
        default:
            return "Unknown";
        break;
    }

    return "Unknown";
}

static const char* wwd_status_to_string(wwd_event_status_t status)
{
    switch ( status )
    {
        CASE_RETURN_STRING(WLC_E_STATUS_SUCCESS)
        CASE_RETURN_STRING(WLC_E_STATUS_FAIL)
        CASE_RETURN_STRING(WLC_E_STATUS_TIMEOUT)
        CASE_RETURN_STRING(WLC_E_STATUS_NO_NETWORKS)
        CASE_RETURN_STRING(WLC_E_STATUS_ABORT)
        CASE_RETURN_STRING(WLC_E_STATUS_NO_ACK)
        CASE_RETURN_STRING(WLC_E_STATUS_UNSOLICITED)
        CASE_RETURN_STRING(WLC_E_STATUS_ATTEMPT)
        CASE_RETURN_STRING(WLC_E_STATUS_PARTIAL)
        CASE_RETURN_STRING(WLC_E_STATUS_NEWSCAN)
        CASE_RETURN_STRING(WLC_E_STATUS_NEWASSOC)
        CASE_RETURN_STRING(WLC_E_STATUS_11HQUIET)
        CASE_RETURN_STRING(WLC_E_STATUS_SUPPRESS)
        CASE_RETURN_STRING(WLC_E_STATUS_NOCHANS)
        CASE_RETURN_STRING(WLC_E_STATUS_CCXFASTRM)
        CASE_RETURN_STRING(WLC_E_STATUS_CS_ABORT)
        CASE_RETURN_STRING(WLC_SUP_DISCONNECTED)
        CASE_RETURN_STRING(WLC_SUP_CONNECTING)
        CASE_RETURN_STRING(WLC_SUP_IDREQUIRED)
        CASE_RETURN_STRING(WLC_SUP_AUTHENTICATING)
        CASE_RETURN_STRING(WLC_SUP_AUTHENTICATED)
        CASE_RETURN_STRING(WLC_SUP_KEYXCHANGE)
        CASE_RETURN_STRING(WLC_SUP_KEYED)
        CASE_RETURN_STRING(WLC_SUP_TIMEOUT)
        CASE_RETURN_STRING(WLC_SUP_LAST_BASIC_STATE)
        CASE_RETURN_STRING(WLC_SUP_KEYXCHANGE_PREP_M4)
        CASE_RETURN_STRING(WLC_SUP_KEYXCHANGE_WAIT_G1)
        CASE_RETURN_STRING(WLC_SUP_KEYXCHANGE_PREP_G2)
        CASE_RETURN_STRING(WLC_DOT11_SC_SUCCESS)
        CASE_RETURN_STRING(WLC_DOT11_SC_FAILURE)
        CASE_RETURN_STRING(WLC_DOT11_SC_CAP_MISMATCH)
        CASE_RETURN_STRING(WLC_DOT11_SC_REASSOC_FAIL)
        CASE_RETURN_STRING(WLC_DOT11_SC_ASSOC_FAIL)
        CASE_RETURN_STRING(WLC_DOT11_SC_AUTH_MISMATCH)
        CASE_RETURN_STRING(WLC_DOT11_SC_AUTH_SEQ)
        CASE_RETURN_STRING(WLC_DOT11_SC_AUTH_CHALLENGE_FAIL)
        CASE_RETURN_STRING(WLC_DOT11_SC_AUTH_TIMEOUT)
        CASE_RETURN_STRING(WLC_DOT11_SC_ASSOC_BUSY_FAIL)
        CASE_RETURN_STRING(WLC_DOT11_SC_ASSOC_RATE_MISMATCH)
        CASE_RETURN_STRING(WLC_DOT11_SC_ASSOC_SHORT_REQUIRED)
        CASE_RETURN_STRING(WLC_DOT11_SC_ASSOC_PBCC_REQUIRED)
        CASE_RETURN_STRING(WLC_DOT11_SC_ASSOC_AGILITY_REQUIRED)
        CASE_RETURN_STRING(WLC_DOT11_SC_ASSOC_SPECTRUM_REQUIRED)
        CASE_RETURN_STRING(WLC_DOT11_SC_ASSOC_BAD_POWER_CAP)
        CASE_RETURN_STRING(WLC_DOT11_SC_ASSOC_BAD_SUP_CHANNELS)
        CASE_RETURN_STRING(WLC_DOT11_SC_ASSOC_SHORTSLOT_REQUIRED)
        CASE_RETURN_STRING(WLC_DOT11_SC_ASSOC_ERPBCC_REQUIRED)
        CASE_RETURN_STRING(WLC_DOT11_SC_ASSOC_DSSOFDM_REQUIRED)
        CASE_RETURN_STRING(WLC_DOT11_SC_DECLINED)
        CASE_RETURN_STRING(WLC_DOT11_SC_INVALID_PARAMS)
        CASE_RETURN_STRING(WLC_DOT11_SC_INVALID_AKMP)
        CASE_RETURN_STRING(WLC_DOT11_SC_INVALID_MDID)
        CASE_RETURN_STRING(WLC_DOT11_SC_INVALID_FTIE)
        case WLC_E_STATUS_FORCE_32_BIT:
        default:
            break;
    }
    return "Unknown";
}

static const char* wwd_reason_to_string(wwd_event_reason_t reason)
{
    switch ( reason )
    {
        CASE_RETURN_STRING(WLC_E_REASON_INITIAL_ASSOC)
        CASE_RETURN_STRING(WLC_E_REASON_LOW_RSSI)
        CASE_RETURN_STRING(WLC_E_REASON_DEAUTH)
        CASE_RETURN_STRING(WLC_E_REASON_DISASSOC)
        CASE_RETURN_STRING(WLC_E_REASON_BCNS_LOST)
        CASE_RETURN_STRING(WLC_E_REASON_FAST_ROAM_FAILED)
        CASE_RETURN_STRING(WLC_E_REASON_DIRECTED_ROAM)
        CASE_RETURN_STRING(WLC_E_REASON_TSPEC_REJECTED)
        CASE_RETURN_STRING(WLC_E_REASON_BETTER_AP)
        CASE_RETURN_STRING(WLC_E_PRUNE_ENCR_MISMATCH)
        CASE_RETURN_STRING(WLC_E_PRUNE_BCAST_BSSID)
        CASE_RETURN_STRING(WLC_E_PRUNE_MAC_DENY)
        CASE_RETURN_STRING(WLC_E_PRUNE_MAC_NA)
        CASE_RETURN_STRING(WLC_E_PRUNE_REG_PASSV)
        CASE_RETURN_STRING(WLC_E_PRUNE_SPCT_MGMT)
        CASE_RETURN_STRING(WLC_E_PRUNE_RADAR)
        CASE_RETURN_STRING(WLC_E_RSN_MISMATCH)
        CASE_RETURN_STRING(WLC_E_PRUNE_NO_COMMON_RATES)
        CASE_RETURN_STRING(WLC_E_PRUNE_BASIC_RATES)
        CASE_RETURN_STRING(WLC_E_PRUNE_CCXFAST_PREVAP)
        CASE_RETURN_STRING(WLC_E_PRUNE_CIPHER_NA)
        CASE_RETURN_STRING(WLC_E_PRUNE_KNOWN_STA)
        CASE_RETURN_STRING(WLC_E_PRUNE_CCXFAST_DROAM)
        CASE_RETURN_STRING(WLC_E_PRUNE_WDS_PEER)
        CASE_RETURN_STRING(WLC_E_PRUNE_QBSS_LOAD)
        CASE_RETURN_STRING(WLC_E_PRUNE_HOME_AP)
        CASE_RETURN_STRING(WLC_E_PRUNE_AP_BLOCKED)
        CASE_RETURN_STRING(WLC_E_PRUNE_NO_DIAG_SUPPORT)
        CASE_RETURN_STRING(WLC_E_SUP_OTHER)
        CASE_RETURN_STRING(WLC_E_SUP_DECRYPT_KEY_DATA)
        CASE_RETURN_STRING(WLC_E_SUP_BAD_UCAST_WEP128)
        CASE_RETURN_STRING(WLC_E_SUP_BAD_UCAST_WEP40)
        CASE_RETURN_STRING(WLC_E_SUP_UNSUP_KEY_LEN)
        CASE_RETURN_STRING(WLC_E_SUP_PW_KEY_CIPHER)
        CASE_RETURN_STRING(WLC_E_SUP_MSG3_TOO_MANY_IE)
        CASE_RETURN_STRING(WLC_E_SUP_MSG3_IE_MISMATCH)
        CASE_RETURN_STRING(WLC_E_SUP_NO_INSTALL_FLAG)
        CASE_RETURN_STRING(WLC_E_SUP_MSG3_NO_GTK)
        CASE_RETURN_STRING(WLC_E_SUP_GRP_KEY_CIPHER)
        CASE_RETURN_STRING(WLC_E_SUP_GRP_MSG1_NO_GTK)
        CASE_RETURN_STRING(WLC_E_SUP_GTK_DECRYPT_FAIL)
        CASE_RETURN_STRING(WLC_E_SUP_SEND_FAIL)
        CASE_RETURN_STRING(WLC_E_SUP_DEAUTH)
        CASE_RETURN_STRING(WLC_E_SUP_WPA_PSK_TMO)
        CASE_RETURN_STRING(DOT11_RC_RESERVED)
        CASE_RETURN_STRING(DOT11_RC_UNSPECIFIED)
        CASE_RETURN_STRING(DOT11_RC_AUTH_INVAL)
        CASE_RETURN_STRING(DOT11_RC_DEAUTH_LEAVING)
        CASE_RETURN_STRING(DOT11_RC_INACTIVITY)
        CASE_RETURN_STRING(DOT11_RC_BUSY)
        CASE_RETURN_STRING(DOT11_RC_INVAL_CLASS_2)
        CASE_RETURN_STRING(DOT11_RC_INVAL_CLASS_3)
        CASE_RETURN_STRING(DOT11_RC_DISASSOC_LEAVING)
        CASE_RETURN_STRING(DOT11_RC_NOT_AUTH)
        CASE_RETURN_STRING(DOT11_RC_BAD_PC)
        CASE_RETURN_STRING(DOT11_RC_BAD_CHANNELS)
        CASE_RETURN_STRING(DOT11_RC_UNSPECIFIED_QOS)
        CASE_RETURN_STRING(DOT11_RC_INSUFFCIENT_BW)
        CASE_RETURN_STRING(DOT11_RC_EXCESSIVE_FRAMES)
        CASE_RETURN_STRING(DOT11_RC_TX_OUTSIDE_TXOP)
        CASE_RETURN_STRING(DOT11_RC_LEAVING_QBSS)
        CASE_RETURN_STRING(DOT11_RC_BAD_MECHANISM)
        CASE_RETURN_STRING(DOT11_RC_SETUP_NEEDED)
        CASE_RETURN_STRING(DOT11_RC_TIMEOUT)
        CASE_RETURN_STRING(WLC_E_NAN_EVENT_STATUS_CHG)
        CASE_RETURN_STRING(WLC_E_NAN_EVENT_MERGE)
        CASE_RETURN_STRING(WLC_E_NAN_EVENT_STOP)
        CASE_RETURN_STRING(WLC_E_NAN_EVENT_P2P)
        CASE_RETURN_STRING(WLC_E_NAN_EVENT_WINDOW_BEGIN_P2P)
        CASE_RETURN_STRING(WLC_E_NAN_EVENT_WINDOW_BEGIN_MESH)
        CASE_RETURN_STRING(WLC_E_NAN_EVENT_WINDOW_BEGIN_IBSS)
        CASE_RETURN_STRING(WLC_E_NAN_EVENT_WINDOW_BEGIN_RANGING)
        CASE_RETURN_STRING(WLC_E_NAN_EVENT_POST_DISC)
        CASE_RETURN_STRING(WLC_E_NAN_EVENT_DATA_IF_ADD)
        CASE_RETURN_STRING(WLC_E_NAN_EVENT_DATA_PEER_ADD)
        CASE_RETURN_STRING(WLC_E_NAN_EVENT_DATA_IND)
        CASE_RETURN_STRING(WLC_E_NAN_EVENT_DATA_CONF)
        CASE_RETURN_STRING(WLC_E_NAN_EVENT_SDF_RX)
        CASE_RETURN_STRING(WLC_E_NAN_EVENT_DATA_END)
        CASE_RETURN_STRING(WLC_E_NAN_EVENT_BCN_RX)
        case DOT11_RC_MAX:
        case WLC_E_REASON_FORCE_32_BIT:
        default:
            break;
    }

    return "Unknown";
}

static const char* wwd_interface_to_string(wwd_interface_t interface)
{
    switch ( interface )
    {
        CASE_RETURN_STRING(WWD_STA_INTERFACE)
        CASE_RETURN_STRING(WWD_AP_INTERFACE)
        CASE_RETURN_STRING(WWD_P2P_INTERFACE)
        CASE_RETURN_STRING(WWD_ETHERNET_INTERFACE)
        case WWD_INTERFACE_MAX:
        case WWD_INTERFACE_FORCE_32_BIT:
        default:
            break;
    }
    return "Unknown";
}
#endif /* WPRINT_ENABLE_WWD_DEBUG */

void wwd_log_event( const wwd_event_header_t* event_header, const uint8_t* event_data )
{
    UNUSED_PARAMETER(event_header);
    UNUSED_PARAMETER(event_data);
    WWD_IOCTL_LOG_ADD_EVENT(event_header->event_type, event_header->status,
        event_header->reason);
    WPRINT_WWD_DEBUG(("%lu: Event (interface, type, status, reason): %s %s %s %s\n",
        (uint32_t) host_rtos_get_time( ),
        wwd_interface_to_string(event_header->interface),
        wwd_event_to_string(event_header->event_type),
        wwd_status_to_string(event_header->status),
        wwd_reason_to_string(event_header->reason)));
}

/** Callback for join events
 *  This is called when the WLC_E_SET_SSID event is received,
 *  indicating that the system has joined successfully.
 *  Wakes the thread which was doing the join, allowing it to resume.
 */
static /*@null@*/ void* wiced_join_events_handler( const wwd_event_header_t* event_header, const uint8_t* event_data, /*@returned@*/ void* handler_user_data )
{
    host_semaphore_type_t* semaphore = (host_semaphore_type_t*) handler_user_data;
    wiced_bool_t join_attempt_complete = WICED_FALSE;

    UNUSED_PARAMETER(event_data);

    if ( ( (uint32_t)event_header->interface != WWD_STA_INTERFACE ) && ( (uint32_t)event_header->interface != WWD_P2P_INTERFACE )
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
        && ( (uint32_t)event_header->interface != WWD_AP_INTERFACE )
#endif
    )
    {
        //WPRINT_APP_INFO(("%s: Bad event header interface, 0x%x\n", __FUNCTION__, event_header->interface));
        return handler_user_data;
    }

     if (event_header->interface >= WWD_INTERFACE_MAX) {
        WPRINT_APP_INFO(("%s: event_header: Bad interface\n", __FUNCTION__));
     }
    switch ( event_header->event_type )
    {
        case WLC_E_PSK_SUP:
            /* Ignore WLC_E_PSK_SUP event if link is not up */
            if (  (wiced_join_status[event_header->interface] & JOIN_LINK_READY ) != 0 )
            {
                if ( event_header->status == WLC_SUP_KEYED )
                {
                    /* Successful WPA key exchange */
                    wiced_join_status[event_header->interface] &= ~JOIN_SECURITY_FLAGS_MASK;
                    wiced_join_status[event_header->interface] |= JOIN_SECURITY_COMPLETE;
                }
                else
                {
                    /* join has completed with an error */
                    join_attempt_complete = WICED_TRUE;
                    if ( ( event_header->status == WLC_SUP_KEYXCHANGE_WAIT_M1 ) && ( event_header->reason == WLC_E_SUP_WPA_PSK_TMO ) )
                    {
                        /* A timeout waiting for M1 may occur at the edge of the cell or if the AP is particularly slow. */
                        WPRINT_WWD_DEBUG(("Supplicant M1 timeout event\n"));
                        wiced_join_status[event_header->interface] |= JOIN_EAPOL_KEY_M1_TIMEOUT;
                    }
                    else if ( ( event_header->status == WLC_SUP_KEYXCHANGE_WAIT_M3 ) && ( event_header->reason == WLC_E_SUP_WPA_PSK_TMO ) )
                    {
                        /* A timeout waiting for M3 is an indicator that the passphrase may be incorrect. */
                        WPRINT_WWD_DEBUG(("Supplicant M3 timeout event\n"));
                        wiced_join_status[event_header->interface] |= JOIN_EAPOL_KEY_M3_TIMEOUT;
                    }
                    else if ( ( event_header->status == WLC_SUP_KEYXCHANGE_WAIT_G1 ) && ( event_header->reason == WLC_E_SUP_WPA_PSK_TMO ) )
                    {
                        /* A timeout waiting for G1 (group key) may occur at the edge of the cell. */
                        WPRINT_WWD_DEBUG(("Supplicant G1 timeout event\n"));
                        wiced_join_status[event_header->interface] |= JOIN_EAPOL_KEY_G1_TIMEOUT;
                    }
                    else
                    {
                        WPRINT_WWD_DEBUG(("Unsuccessful supplicant event; status=0x%x\n", event_header->status));
                        /* Unknown failure during EAPOL key handshake */
                        wiced_join_status[event_header->interface] |= JOIN_EAPOL_KEY_FAILURE;
                    }
                }
            }
            break;

        case WLC_E_SET_SSID:
            if ( event_header->status == WLC_E_STATUS_SUCCESS )
            {
                /* SSID has been successfully set. */
                wiced_join_status[event_header->interface] |= JOIN_SSID_SET;
            }
            else if ( event_header->status == WLC_E_STATUS_NO_NETWORKS ) /* We don't bail out on this event or things like WPS won't work if the AP is rebooting after configuration */
            {
                wiced_join_status[event_header->interface] |= JOIN_NO_NETWORKS;
            }
            else
            {
                join_attempt_complete = WICED_TRUE;
            }
            break;

        case WLC_E_LINK:
            if ( ( event_header->flags & WLC_EVENT_MSG_LINK ) != 0 )
            {
                wiced_join_status[event_header->interface] |= JOIN_LINK_READY;
            }
            else
            {
                wiced_join_status[event_header->interface] &= ~JOIN_LINK_READY;
            }
            break;

        case WLC_E_DEAUTH_IND:
        case WLC_E_DISASSOC_IND:
            wiced_join_status[event_header->interface] &= ~( JOIN_AUTHENTICATED | JOIN_LINK_READY );
            break;

        case WLC_E_AUTH:
            if ( event_header->status == WLC_E_STATUS_SUCCESS )
            {
                wiced_join_status[event_header->interface] |= JOIN_AUTHENTICATED;
            }
            else if ( event_header->status == WLC_E_STATUS_UNSOLICITED )
            {
                WPRINT_WWD_DEBUG(("Ignore UNSOLICITED pkt event\n"));
            }
            else
            {
                /* We cannot authenticate. Perhaps we're blocked or at the edge of a cell. */
                join_attempt_complete = WICED_TRUE;
            }
            break;

        case WLC_E_CSA_COMPLETE_IND:
            if (event_header->datalen >= sizeof(wl_chan_switch_t))
            {
                wl_chan_switch_t*  wl_csa = (wl_chan_switch_t *)event_data;
                WPRINT_APP_INFO(("CSA event => chan %d\n", wl_csa->chspec & 0xff));
            }
            break;

        /* Note - These are listed to keep gcc pedantic checking happy */
        case WLC_E_RRM:
        case WLC_E_NONE:
        case WLC_E_ROAM:
        case WLC_E_JOIN:
        case WLC_E_START:
        case WLC_E_AUTH_IND:
        case WLC_E_DEAUTH:
        case WLC_E_ASSOC:
        case WLC_E_ASSOC_IND:
        case WLC_E_REASSOC:
        case WLC_E_REASSOC_IND:
        case WLC_E_DISASSOC:
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
        case WLC_E_PFN_BEST_BATCHING:
        case WLC_E_RSSI:
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
        case WLC_E_P2P_DISC_LISTEN_COMPLETE:
        case WLC_E_BT_WIFI_HANDOVER_REQ:
        case WLC_E_SPW_TXINHIBIT:
        case WLC_E_FBT_AUTH_REQ_IND:
        case WLC_E_RSSI_LQM:
        case WLC_E_PFN_GSCAN_FULL_RESULT:
        case WLC_E_PFN_SWC:
        case WLC_E_AUTHORIZED:
        case WLC_E_PROBREQ_MSG_RX:
        case WLC_E_PFN_SCAN_COMPLETE:
        case WLC_E_RMC_EVENT:
        case WLC_E_DPSTA_INTF_IND:
        case WLC_E_FORCE_32_BIT:
        case WLC_E_ULP:
        case WLC_E_LAST:
        default:
            wiced_assert( "Received event which was not registered\n", 0 != 0 );
            break;
    }

    if ( wwd_wifi_is_ready_to_transceive( (wwd_interface_t) event_header->interface ) == WWD_SUCCESS )
    {
        join_attempt_complete = WICED_TRUE;
        wwd_wifi_link_update( );
    }

    if ( join_attempt_complete == WICED_TRUE )
    {
        if ( semaphore != NULL )
        {
            host_rtos_get_semaphore( &active_join_mutex, NEVER_TIMEOUT, WICED_FALSE );
            if ( active_join_semaphore != NULL )
            {
                wiced_assert( "Unexpected semaphore\n", active_join_semaphore == semaphore );
                host_rtos_set_semaphore( active_join_semaphore, WICED_FALSE );
            }
            host_rtos_set_semaphore( &active_join_mutex, WICED_FALSE );
        }
        return NULL;
    }
    else
    {
        return handler_user_data;
    }
}

wwd_result_t wwd_wifi_send_csa( const wiced_chan_switch_t* wiced_csa, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    wl_chan_switch_t*      wl_csa;

    wl_csa = (wl_chan_switch_t*) wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(wl_chan_switch_t), IOVAR_STR_CSA );
    CHECK_IOCTL_BUFFER( wl_csa );
    memset( wl_csa, 0, sizeof(wl_chan_switch_t) );
    wl_csa->mode = wiced_csa->mode;
    wl_csa->count = wiced_csa->count;
    wl_csa->chspec = wiced_csa->chspec;
    CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, interface ) );
    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_get_mac_address( wiced_mac_t* mac, wwd_interface_t interface )
{
    if ( interface == wiced_cached_mac_interface )
    {
        memcpy( mac, &wiced_cached_mac, sizeof( wiced_cached_mac ) );
    }
    else
    {
        wiced_buffer_t buffer;
        wiced_buffer_t response;

        CHECK_IOCTL_BUFFER( wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(wiced_mac_t), IOVAR_STR_CUR_ETHERADDR ) );

        CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, interface ) );

        memcpy( mac, host_buffer_get_current_piece_data_pointer( response ), sizeof(wiced_mac_t) );
        host_buffer_release( response, WWD_NETWORK_RX );
    }

    return WWD_SUCCESS;
}

#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
wwd_result_t wwd_wifi_set_AP_mac_address( wiced_mac_t mac )
{
    wiced_buffer_t buffer;
    uint32_t*      data;

    /* Assert when WLAN is UP */
    wiced_assert("WLAN is UP. Ensure WLAN is DOWN before invoking this API", wwd_wlan_status.state == WLAN_DOWN );

    /* Set the MAC address */
    data = (uint32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(wiced_mac_t), IOVAR_STR_CUR_ETHERADDR );
    CHECK_IOCTL_BUFFER( data );
    memcpy( data, &mac, sizeof(wiced_mac_t) );
    CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, WWD_AP_INTERFACE ) );

    return WWD_SUCCESS;
}
#endif

wwd_result_t wwd_wifi_set_mac_address( wiced_mac_t mac, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    uint32_t*      data;

    /* Assert when WLAN is UP */
    wiced_assert("WLAN is UP. Ensure WLAN is DOWN before invoking this API", wwd_wlan_status.state == WLAN_DOWN );

    /* AP interface needs to come up with MAC different from STA  */
#ifdef APOLLO_AUDIO
    /* Work around the issue of asking API to set one address and it sets a different address.
     * This will cause any comparison of set and get mac address to fail.  TODO: move twiddling this
     * bit to a higher level.
     */
    if ( 0 )
#else
    if ( interface == WWD_AP_INTERFACE )
#endif
    {
        wiced_mac_t ap_mac_address;

        memcpy(&ap_mac_address, &mac, sizeof(wiced_mac_t ));
        if ( ap_mac_address.octet[0] & MAC_ADDRESS_LOCALLY_ADMINISTERED_BIT )
        {
            ap_mac_address.octet[0] &= (uint8_t)~(MAC_ADDRESS_LOCALLY_ADMINISTERED_BIT);
        }
        else
        {
            ap_mac_address.octet[0] |= MAC_ADDRESS_LOCALLY_ADMINISTERED_BIT;
        }

        data = (uint32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(wiced_mac_t), IOVAR_STR_CUR_ETHERADDR );
        CHECK_IOCTL_BUFFER( data );
        memcpy( data, &ap_mac_address, sizeof(wiced_mac_t) );
        CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, interface ) );

        if ( memcmp ( &mac, &ap_mac_address, sizeof(wiced_mac_t))  != 0 )
        {
            WPRINT_APP_INFO ((" STA MAC address : %02x:%02x:%02x:%02x:%02x:%02x \n"
                    " AP  MAC address : %02x:%02x:%02x:%02x:%02x:%02x \n",
                    mac.octet[0], mac.octet[1], mac.octet[2],
                    mac.octet[3], mac.octet[4], mac.octet[3],
                    ap_mac_address.octet[0], ap_mac_address.octet[1], ap_mac_address.octet[2],
                    ap_mac_address.octet[3], ap_mac_address.octet[4], ap_mac_address.octet[3] ));
        }
    }
    else
    {
        data = (uint32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(wiced_mac_t), IOVAR_STR_CUR_ETHERADDR );
        CHECK_IOCTL_BUFFER( data );
        memcpy( data, &mac, sizeof(wiced_mac_t) );
        CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, interface ) );
    }

    /* Update cache */
    if ( wiced_cached_mac_interface == WWD_STA_INTERFACE )
    {
        memcpy( &wiced_cached_mac, &mac, sizeof( wiced_cached_mac ) );
    }

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_get_and_cache_mac_address( wwd_interface_t interface )
{
    wwd_result_t result;

    if ( interface == WWD_INTERFACE_MAX )
    {
        wiced_cached_mac_interface = interface;
        return WWD_SUCCESS;
    }

    result = wwd_wifi_get_mac_address( &wiced_cached_mac, interface );
    if ( result == WWD_SUCCESS )
    {
        wiced_cached_mac_interface = interface;
    }

    return result;
}

wwd_result_t wwd_wifi_is_ready_to_transceive( wwd_interface_t interface )
{
#if 0
    /* Handle Splint bug */ /*@-noeffect@*/ (void) wwd_wifi_ap_is_up; /*@+noeffect@*/
#endif

    switch ( interface )
    {
        case WWD_AP_INTERFACE:
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
            return wwd_wifi_check_join_status( interface );
#else
            return ( wwd_wifi_get_ap_is_up() == WICED_TRUE ) ? WWD_SUCCESS : WWD_INTERFACE_NOT_UP;
#endif

        case WWD_P2P_INTERFACE:
            if ( wwd_wifi_p2p_is_go_up( ) == WICED_TRUE )
            {
                return WWD_SUCCESS;
            }
            // Otherwise fall through and check P2P client join status
            /* Disables Eclipse static analysis warning */
            /* no break */
            /* Fall Through */
        case WWD_STA_INTERFACE:
                return wwd_wifi_check_join_status( interface );
            /* Disables Eclipse static analysis warning */
            /* No break needed due to returns in all case paths */
            /* no break */
            /* Fall Through */
        case WWD_ETHERNET_INTERFACE:
        case WWD_INTERFACE_MAX:
        case WWD_INTERFACE_FORCE_32_BIT:
            /* Fall Through */
        default:
            return WWD_UNKNOWN_INTERFACE;
    }
}

void wwd_wifi_register_link_update_callback( void (*callback_function)( void ) )
{
    wwd_wifi_link_update_callback = callback_function;
}

void wwd_wifi_link_update( void )
{
    if ( NULL != wwd_wifi_link_update_callback )
    {
        wwd_wifi_link_update_callback( );
    }
}

wwd_result_t wwd_wifi_enable_powersave_with_throughput( uint16_t return_to_sleep_delay_ms )
{
    return wwd_wifi_enable_powersave_with_throughput_interface( return_to_sleep_delay_ms, WWD_STA_INTERFACE );
}


wwd_result_t wwd_wifi_enable_powersave_with_throughput_interface( uint16_t return_to_sleep_delay_ms, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    uint32_t* data;

    if ( return_to_sleep_delay_ms == 0 )
    {
        return_to_sleep_delay_ms = DEFAULT_PM2_SLEEP_RET_TIME;
    }
    else if ( return_to_sleep_delay_ms < PM2_SLEEP_RET_TIME_MIN )
    {
        return WWD_DELAY_TOO_SHORT;
    }
    else if ( return_to_sleep_delay_ms > PM2_SLEEP_RET_TIME_MAX )
    {
        return WWD_DELAY_TOO_LONG;
    }

    /* Set the maximum time to wait before going back to sleep */
    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_PM2_SLEEP_RET, (uint32_t)( return_to_sleep_delay_ms / 10 ) * 10, interface ) );

#ifdef CHIP_FIRMWARE_SUPPORTS_PM_LIMIT_IOVAR
    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_PM_LIMIT, NULL_FRAMES_WITH_PM_SET_LIMIT, interface ) );
#endif /* ifdef CHIP_FIRMWARE_SUPPORTS_PM_LIMIT_IOVAR */

    /* set PM2 fast return to sleep powersave mode */
    data = (uint32_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, (uint16_t) 4 );
    CHECK_IOCTL_BUFFER( data );
    *data = (uint32_t) PM2_POWERSAVE_MODE;

    if ( interface == WWD_STA_INTERFACE )
    {
        /* Record the power save mode and the return to sleep delay */
        wiced_wifi_powersave_mode        = PM2_POWERSAVE_MODE;
        wiced_wifi_return_to_sleep_delay = return_to_sleep_delay_ms;
    }

    RETURN_WITH_ASSERT( wwd_sdpcm_send_ioctl( SDPCM_SET, WLC_SET_PM, buffer, NULL, interface ) );
}


wwd_result_t wwd_wifi_enable_powersave( void )
{
    return wwd_wifi_enable_powersave_interface( WWD_STA_INTERFACE );
}

wwd_result_t wwd_wifi_get_powersave_interface( wwd_interface_t interface, uint32_t *value )
{
    return wwd_wifi_get_ioctl_value( WLC_GET_PM, value, interface );
}

static void wwd_wifi_fw_cmd_exit_hook( sdpcm_command_type_t type, uint32_t command, const char* name, wwd_interface_t interface )
{
    wwd_result_t result;
    uint32_t val;
    static wiced_bool_t executing_hook = WICED_FALSE;
    UNUSED_PARAMETER( type );

    /* if command is set iovar */
    if ( name[0] == ' ' || name[0] == '\0' || WLC_GET_VAR == command || WICED_TRUE == executing_hook )
    {
        /* NOOP */
        return;
    }
    else if ( WLC_SET_VAR == command )
    {
        WPRINT_APP_INFO(( "Iovar \'%s\' interface %d\n", name, interface ));
        executing_hook = WICED_TRUE;

        result = wwd_wifi_get_iovar_value( name, &val, interface );
    }
    else
    {
        /* Assume it's a set ioctl */
        WPRINT_APP_INFO(( "Ioctl %lu; interface %d\n", command, interface ));
        executing_hook = WICED_TRUE;

        /* subtract one to change to get ioctl */
        result = wwd_wifi_get_ioctl_value( command - 1, &val, interface );
    }

    if ( WWD_SUCCESS == result )
    {
        WPRINT_APP_INFO(( "FW value: 0x%x\n", (unsigned int)val ));
    }
    else
    {
        WPRINT_APP_INFO(( "FW value: unknown (err %d)\n", result ));
    }

    executing_hook = WICED_FALSE;
}

wwd_result_t wwd_wifi_set_fw_cmd_debug_mode( wiced_bool_t enable )
{
    if ( WICED_TRUE == enable )
    {
        return wwd_sdpcm_register_fw_cmd_exit_hook( wwd_wifi_fw_cmd_exit_hook );
    }
    else
    {
        return wwd_sdpcm_register_fw_cmd_exit_hook( NULL );
    }
}

wwd_result_t wwd_wifi_enable_powersave_interface( wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    uint32_t* data;

    /* Set legacy powersave mode - PM1 */
    data = (uint32_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, (uint16_t) 4 );
    CHECK_IOCTL_BUFFER( data );
    *data = (uint32_t) PM1_POWERSAVE_MODE;

    if ( interface == WWD_STA_INTERFACE )
    {
        /* Save the power save mode */
        wiced_wifi_powersave_mode = PM1_POWERSAVE_MODE;
    }

    RETURN_WITH_ASSERT( wwd_sdpcm_send_ioctl( SDPCM_SET, WLC_SET_PM, buffer, NULL, interface ) );
}


wwd_result_t wwd_wifi_disable_powersave( void )
{
    return wwd_wifi_disable_powersave_interface( WWD_STA_INTERFACE );
}


wwd_result_t wwd_wifi_disable_powersave_interface( wwd_interface_t interface )
{
    wiced_buffer_t buffer;

    uint32_t* data = (uint32_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, (uint16_t) 4 );
    CHECK_IOCTL_BUFFER( data );
    *data = NO_POWERSAVE_MODE;

    if ( interface == WWD_STA_INTERFACE )
    {
        /* Save the power save mode */
        wiced_wifi_powersave_mode = NO_POWERSAVE_MODE;
    }

    return wwd_sdpcm_send_ioctl( SDPCM_SET, WLC_SET_PM, buffer, NULL, interface );
}


uint8_t wiced_wifi_get_powersave_mode( void )
{
    return wiced_wifi_powersave_mode;
}

uint16_t wiced_wifi_get_return_to_sleep_delay( void )
{
    return wiced_wifi_return_to_sleep_delay;
}

wwd_result_t wwd_wifi_get_tx_power( uint8_t* dbm )
{
    uint32_t data;
    CHECK_RETURN( wwd_wifi_get_iovar_value( IOVAR_STR_QTXPOWER, &data, WWD_STA_INTERFACE ) );
    *dbm = ( uint8_t ) ( data / 4 );

    return WWD_SUCCESS;
}


wwd_result_t wwd_wifi_set_tx_power( uint8_t dbm )
{
    uint32_t data;

    /* Maximum value of QTX Power is 127/4 which is less than 32 */
    if ( dbm >= (uint8_t) MAX_QTX_POWER )
    {
        WPRINT_APP_INFO(("Tx Power too large, set to less than 32dbm \n"));
        return WWD_BADARG;
    }

    data = (uint32_t) ( 4 * dbm );
    RETURN_WITH_ASSERT( wwd_wifi_set_iovar_value( IOVAR_STR_QTXPOWER, data, WWD_STA_INTERFACE ) );
}


wwd_result_t wwd_wifi_set_listen_interval( uint8_t listen_interval, wiced_listen_interval_time_unit_t time_unit )
{
    uint8_t listen_interval_dtim;

    if ( time_unit == WICED_LISTEN_INTERVAL_TIME_UNIT_DTIM )
    {
        listen_interval_dtim = listen_interval;
    }
    else
    {
        /* If the wake interval measured in DTIMs is set to 0, the wake interval is measured in beacon periods */
        listen_interval_dtim = 0;

        /* The wake period is measured in beacon periods, set the value as required */
        CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_LISTEN_INTERVAL_BEACON, listen_interval, WWD_STA_INTERFACE ) );
    }

    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_LISTEN_INTERVAL_DTIM, listen_interval_dtim, WWD_STA_INTERFACE ) );

    return wwd_wifi_set_listen_interval_assoc( (uint16_t)listen_interval );
}


wwd_result_t wwd_wifi_set_listen_interval_assoc( uint16_t listen_interval )
{
    RETURN_WITH_ASSERT( wwd_wifi_set_iovar_value( IOVAR_STR_LISTEN_INTERVAL_ASSOC, listen_interval, WWD_STA_INTERFACE ) );
}


wwd_result_t wwd_wifi_get_listen_interval( wiced_listen_interval_t* li )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    int*           data;

    data = (int*)wwd_sdpcm_get_iovar_buffer( &buffer, 4, IOVAR_STR_LISTEN_INTERVAL_BEACON );
    CHECK_IOCTL_BUFFER( data );
    memset( data, 0, 1 );
    CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, WWD_STA_INTERFACE ) );

    memcpy( (uint8_t *) &(li->beacon), (char *)host_buffer_get_current_piece_data_pointer( response ), 1 );
    host_buffer_release(response, WWD_NETWORK_RX);

    data = (int*)wwd_sdpcm_get_iovar_buffer( &buffer, 4, IOVAR_STR_LISTEN_INTERVAL_DTIM );
    CHECK_IOCTL_BUFFER( data );
    memset( data, 0, 1 );
    CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, WWD_STA_INTERFACE ) );

    memcpy( (uint8_t *) &(li->dtim), (char *)host_buffer_get_current_piece_data_pointer( response ), 1 );
    host_buffer_release(response, WWD_NETWORK_RX);

    data = (int*)wwd_sdpcm_get_iovar_buffer( &buffer, 4, IOVAR_STR_LISTEN_INTERVAL_ASSOC );
    CHECK_IOCTL_BUFFER( data );
    memset( data, 0, 4 );
    CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, WWD_STA_INTERFACE ) );

    memcpy( (uint16_t *) &(li->assoc), (char *)host_buffer_get_current_piece_data_pointer( response ), 2 );
    host_buffer_release(response, WWD_NETWORK_RX);

    return WWD_SUCCESS;
}


wwd_result_t wwd_wifi_set_ofdm_dutycycle( uint8_t duty_cycle_val )
{
    if ( duty_cycle_val > 100 )
    {
        return WWD_INVALID_DUTY_CYCLE;
    }

    RETURN_WITH_ASSERT( wwd_wifi_set_iovar_value( IOVAR_STR_DUTY_CYCLE_OFDM, duty_cycle_val, WWD_STA_INTERFACE ) );
}


wwd_result_t wwd_wifi_set_cck_dutycycle( uint8_t duty_cycle_val )
{
    if ( duty_cycle_val > 100 )
    {
        return WWD_INVALID_DUTY_CYCLE;
    }

    RETURN_WITH_ASSERT( wwd_wifi_set_iovar_value( IOVAR_STR_DUTY_CYCLE_CCK, duty_cycle_val, WWD_STA_INTERFACE ) );
}


wwd_result_t wwd_wifi_get_ofdm_dutycycle( uint8_t* duty_cycle_value )
{
    uint32_t data;
    CHECK_RETURN( wwd_wifi_get_iovar_value( IOVAR_STR_DUTY_CYCLE_OFDM, &data, WWD_STA_INTERFACE ) );

    *duty_cycle_value = (uint8_t)data;

    return WWD_SUCCESS;
}


wwd_result_t wwd_wifi_get_cck_dutycycle( uint8_t* duty_cycle_value )
{
    uint32_t data;
    CHECK_RETURN( wwd_wifi_get_iovar_value( IOVAR_STR_DUTY_CYCLE_CCK, &data, WWD_STA_INTERFACE ) );

    *duty_cycle_value = (uint8_t)data;

    return WWD_SUCCESS;
}


wwd_result_t wwd_wifi_get_pmk( const char* psk, uint8_t psk_length, char* pmk )
{
    wsec_pmk_t*    psk_ioctl;
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    wsec_pmk_t*    psk_info;

    psk_ioctl = ( wsec_pmk_t* )wwd_sdpcm_get_ioctl_buffer( &buffer, sizeof(wsec_pmk_t) );
    CHECK_IOCTL_BUFFER( psk_ioctl );

    /* validate the input: can be called from command line */
    if ( psk_length > sizeof(psk_ioctl->key) )
    {
        return WWD_WLAN_ERROR;
    }
    else if (psk_length < sizeof(psk_ioctl->key))
    {
        /* 0 terminate if possible */
        psk_ioctl->key[psk_length] = 0;
    }

    memcpy( psk_ioctl->key, psk, psk_length );

    psk_ioctl->key_len      = psk_length;
    psk_ioctl->flags        = 0;

    CHECK_RETURN( wwd_sdpcm_send_ioctl( SDPCM_GET, WLC_GET_WSEC_PMK, buffer, &response, WWD_STA_INTERFACE ) );

    psk_info = (wsec_pmk_t*) host_buffer_get_current_piece_data_pointer( response );
    if ( psk_info->key_len != WSEC_MAX_PSK_LEN )
    {
        host_buffer_release( response, WWD_NETWORK_RX );
        return WWD_PMK_WRONG_LENGTH;
    }
    memcpy( pmk, psk_info->key, psk_info->key_len );
    host_buffer_release( response, WWD_NETWORK_RX );
    return WWD_SUCCESS;
}


wwd_result_t wwd_wifi_register_multicast_address_for_interface( const wiced_mac_t* mac, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    uint16_t a;
    mcast_list_t* orig_mcast_list;
    mcast_list_t* new_mcast_list;

    /* Get the current multicast list */
    CHECK_IOCTL_BUFFER( wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(uint32_t) + MAX_SUPPORTED_MCAST_ENTRIES * sizeof(wiced_mac_t), IOVAR_STR_MCAST_LIST ) );
    CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, interface ) );

    /* Verify address is not currently registered */
    orig_mcast_list = (mcast_list_t*) host_buffer_get_current_piece_data_pointer( response );
    for ( a = 0; a < orig_mcast_list->entry_count; ++a )
    {
        /* Check if any address matches */
        if ( 0 == memcmp( mac, &orig_mcast_list->macs[a], sizeof(wiced_mac_t) ) )
        {
            /* A matching address has been found so we can stop now. */
            host_buffer_release( response, WWD_NETWORK_RX );
            return WWD_SUCCESS;
        }
    }

    /* Add the provided address to the list and write the new multicast list */
    new_mcast_list = (mcast_list_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) ( sizeof(uint32_t) + ( orig_mcast_list->entry_count + 1 ) * sizeof(wiced_mac_t) ), IOVAR_STR_MCAST_LIST );
    CHECK_IOCTL_BUFFER( new_mcast_list );
    new_mcast_list->entry_count = orig_mcast_list->entry_count;
    memcpy( new_mcast_list->macs, orig_mcast_list->macs, orig_mcast_list->entry_count * sizeof(wiced_mac_t) );
    host_buffer_release( response, WWD_NETWORK_RX );
    memcpy( &new_mcast_list->macs[new_mcast_list->entry_count], mac, sizeof(wiced_mac_t) );
    ++new_mcast_list->entry_count;
    RETURN_WITH_ASSERT( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, interface ) );
}


wwd_result_t wwd_wifi_register_multicast_address( const wiced_mac_t* mac )
{
    return wwd_wifi_register_multicast_address_for_interface( mac, WWD_STA_INTERFACE );
}

wwd_result_t wwd_wifi_unregister_multicast_address_for_interface( const wiced_mac_t* mac, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    uint16_t a;
    mcast_list_t* orig_mcast_list;

    /* Get the current multicast list */
    CHECK_IOCTL_BUFFER( wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(uint32_t) + MAX_SUPPORTED_MCAST_ENTRIES * sizeof(wiced_mac_t), IOVAR_STR_MCAST_LIST ) );
    CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, interface ) );

    /* Find the address, assuming it is part of the list */
    orig_mcast_list = (mcast_list_t*) host_buffer_get_current_piece_data_pointer( response );
    if ( orig_mcast_list->entry_count != 0 )
    {
        mcast_list_t* new_mcast_list = (mcast_list_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) ( sizeof(uint32_t) + ( orig_mcast_list->entry_count - 1 ) * sizeof(wiced_mac_t) ), IOVAR_STR_MCAST_LIST );
        CHECK_IOCTL_BUFFER( new_mcast_list );
        for ( a = 0; a < orig_mcast_list->entry_count; ++a )
        {
            if ( 0 == memcmp( mac, &orig_mcast_list->macs[a], sizeof(wiced_mac_t) ) )
            {
                /* Copy the existing list up to the matching address */
                memcpy( new_mcast_list->macs, orig_mcast_list->macs, a * sizeof(wiced_mac_t) );

                /* Skip the current address and copy the remaining entries */
                memcpy( &new_mcast_list->macs[a], &orig_mcast_list->macs[a + 1], ( size_t )( orig_mcast_list->entry_count - a - 1 ) * sizeof(wiced_mac_t) );

                new_mcast_list->entry_count = orig_mcast_list->entry_count - 1;
                host_buffer_release( response, WWD_NETWORK_RX );
                RETURN_WITH_ASSERT( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, interface ) );
            }
        }
        /* There was something in the list, but the request MAC wasn't there */
        host_buffer_release( buffer, WWD_NETWORK_TX );
    }
    /* If we get here than the address wasn't in the list or the list was empty */
    host_buffer_release( response, WWD_NETWORK_RX );
    return WWD_SUCCESS;
}


wwd_result_t wwd_wifi_unregister_multicast_address( const wiced_mac_t* mac )
{
    return wwd_wifi_unregister_multicast_address_for_interface( mac, WWD_STA_INTERFACE );
}

wwd_result_t wwd_wifi_get_rssi( int32_t* rssi )
{
    return wwd_wifi_get_ioctl_buffer( WLC_GET_RSSI, (uint8_t*)rssi, sizeof(*rssi), WWD_STA_INTERFACE );
}

wwd_result_t wwd_wifi_get_noise( int32_t* noise )
{
    return wwd_wifi_get_ioctl_buffer( WLC_GET_PHY_NOISE, (uint8_t*)noise, sizeof(*noise), WWD_STA_INTERFACE );
}

wwd_result_t wwd_wifi_get_ap_client_rssi( int32_t* rssi, const wiced_mac_t* client_mac_addr  )
{
    client_rssi_t* client_rssi;
    wiced_buffer_t buffer;
    wiced_buffer_t response;

    /* WLAN expects buffer size to be 4-byte aligned */
    client_rssi = (client_rssi_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, ROUND_UP( sizeof(client_rssi_t), sizeof(uint32_t)) );
    CHECK_IOCTL_BUFFER( client_rssi );

    memcpy(&client_rssi->mac_address, client_mac_addr, sizeof(wiced_mac_t));
    client_rssi->value = 0;

    CHECK_RETURN( wwd_sdpcm_send_ioctl( SDPCM_GET, WLC_GET_RSSI, buffer, &response, WWD_AP_INTERFACE ) );

    memcpy( rssi, host_buffer_get_current_piece_data_pointer( response ), sizeof( int32_t ) );
    host_buffer_release( response, WWD_NETWORK_RX );
    return WWD_SUCCESS;
}


wwd_result_t wwd_wifi_select_antenna( wiced_antenna_t antenna )
{
    CHECK_RETURN( wwd_wifi_set_ioctl_value( WLC_SET_TXANT, (uint32_t)antenna, WWD_STA_INTERFACE ) );
    CHECK_RETURN( wwd_wifi_set_ioctl_value( WLC_SET_ANTDIV, (uint32_t)antenna, WWD_STA_INTERFACE ) );

    return WWD_SUCCESS;
}

static uint32_t wwd_wifi_band_to_wlc_band( wiced_802_11_band_t band )
{
    switch ( band )
    {
        case WICED_802_11_BAND_2_4GHZ:   return WLC_BAND_2G;
        case WICED_802_11_BAND_5GHZ:   return WLC_BAND_5G;
        default:
            wiced_assert("Bad args", 0 != 0 );
    }

    return WLC_BAND_2G;
}

wwd_result_t wwd_wifi_set_roam_trigger( int32_t trigger_level )
{
    uint32_t roaming_trigger[2];

    roaming_trigger[0] = (uint32_t) trigger_level;
    roaming_trigger[1] = WLC_BAND_ALL;

    return wwd_wifi_set_ioctl_buffer( WLC_SET_ROAM_TRIGGER, roaming_trigger, sizeof( roaming_trigger ), WWD_STA_INTERFACE );
}

wwd_result_t wwd_wifi_set_roam_trigger_per_band( int32_t trigger_level, wiced_802_11_band_t band )
{
    uint32_t roaming_trigger[2];

    roaming_trigger[0] = (uint32_t) trigger_level;
    roaming_trigger[1] = wwd_wifi_band_to_wlc_band( band );

    return wwd_wifi_set_ioctl_buffer( WLC_SET_ROAM_TRIGGER, roaming_trigger, sizeof( roaming_trigger ), WWD_STA_INTERFACE );
}

wwd_result_t wwd_wifi_get_roam_trigger_per_band( int32_t* trigger_level, wiced_802_11_band_t band )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    uint32_t wlc_band;
    uint32_t *ioctl_buffer = NULL;

    /* Validate input arguments */
    wiced_assert("Bad args", trigger_level != NULL );

    /* WLAN expects buffer size to be 4-byte aligned */
    ioctl_buffer = (uint32_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, sizeof( uint32_t ) * 2 );
    CHECK_IOCTL_BUFFER( ioctl_buffer );
    wlc_band = wwd_wifi_band_to_wlc_band( band );

    memcpy( &ioctl_buffer[1], &wlc_band, sizeof( ioctl_buffer[1] ) );

    CHECK_RETURN( wwd_sdpcm_send_ioctl( SDPCM_GET, WLC_GET_ROAM_TRIGGER, buffer, &response, WWD_STA_INTERFACE ) );

    memcpy( trigger_level, host_buffer_get_current_piece_data_pointer( response ), sizeof( int32_t ) );
    host_buffer_release( response, WWD_NETWORK_RX );
    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_get_roam_trigger( int32_t* trigger_level )
{
    return wwd_wifi_get_roam_trigger_per_band( trigger_level, WICED_802_11_BAND_2_4GHZ );
}

wwd_result_t wwd_wifi_set_roam_delta( int32_t trigger_delta )
{
    uint32_t roaming_trigger[2];

    roaming_trigger[0] = (uint32_t)trigger_delta;
    roaming_trigger[1] = WLC_BAND_ALL;

    return wwd_wifi_set_ioctl_buffer( WLC_SET_ROAM_DELTA, roaming_trigger, sizeof( roaming_trigger ), WWD_STA_INTERFACE );
}

wwd_result_t wwd_wifi_set_roam_delta_per_band( int32_t trigger_delta, wiced_802_11_band_t band )
{
    uint32_t roaming_trigger[2];

    roaming_trigger[0] = (uint32_t)trigger_delta;
    roaming_trigger[1] = wwd_wifi_band_to_wlc_band( band );

    return wwd_wifi_set_ioctl_buffer( WLC_SET_ROAM_DELTA, roaming_trigger, sizeof( roaming_trigger ), WWD_STA_INTERFACE );
}

wwd_result_t wwd_wifi_get_roam_delta( int32_t* trigger_delta )
{
    return wwd_wifi_get_roam_delta_per_band( trigger_delta, WICED_802_11_BAND_2_4GHZ );
}

wwd_result_t wwd_wifi_get_roam_delta_per_band( int32_t* trigger_delta, wiced_802_11_band_t band )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    uint32_t wlc_band;
    uint32_t *ioctl_buffer = NULL;

    /* Validate input arguments */
    wiced_assert("Bad args", trigger_delta != NULL );

    /* WLAN expects buffer size to be 4-byte aligned */
    ioctl_buffer = (uint32_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, sizeof( uint32_t ) * 2 );
    CHECK_IOCTL_BUFFER( ioctl_buffer );
    wlc_band = wwd_wifi_band_to_wlc_band( band );

    memcpy( &ioctl_buffer[1], &wlc_band, sizeof( ioctl_buffer[1] ) );

    CHECK_RETURN( wwd_sdpcm_send_ioctl( SDPCM_GET, WLC_GET_ROAM_DELTA, buffer, &response, WWD_STA_INTERFACE ) );

    memcpy( trigger_delta, host_buffer_get_current_piece_data_pointer( response ), sizeof( int32_t ) );
    host_buffer_release( response, WWD_NETWORK_RX );
    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_set_roam_scan_period( uint32_t roam_scan_period )
{
    return wwd_wifi_set_ioctl_value( WLC_SET_ROAM_SCAN_PERIOD, roam_scan_period, WWD_STA_INTERFACE );
}

wwd_result_t wwd_wifi_get_roam_scan_period( uint32_t* roam_scan_period )
{
    return wwd_wifi_get_ioctl_value( WLC_GET_ROAM_SCAN_PERIOD, roam_scan_period, WWD_STA_INTERFACE );
}

wwd_result_t wwd_wifi_turn_off_roam( wiced_bool_t disable )
{
    return wwd_wifi_set_iovar_value( IOVAR_STR_ROAM_OFF, (uint32_t)disable, WWD_STA_INTERFACE );
}

wwd_result_t wwd_wifi_send_action_frame( const wl_action_frame_t* action_frame, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    wiced_action_frame_t* frame;
    uint32_t* a = (uint32_t*) wwd_sdpcm_get_iovar_buffer(&buffer, sizeof(wiced_action_frame_t)+4, "bsscfg:" IOVAR_STR_ACTION_FRAME );
    CHECK_IOCTL_BUFFER( a );
    *a = wwd_get_bss_index( interface );
    frame = (wiced_action_frame_t*)(a+1);
    memcpy(frame, action_frame, sizeof(wiced_action_frame_t));
    RETURN_WITH_ASSERT( wwd_sdpcm_send_iovar(SDPCM_SET, buffer, NULL, WWD_STA_INTERFACE) );
}


wwd_result_t wwd_wifi_get_acparams_sta( edcf_acparam_t *acp )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;

    int* data = (int*) wwd_sdpcm_get_iovar_buffer( &buffer, 64, IOVAR_STR_AC_PARAMS_STA );
    CHECK_IOCTL_BUFFER( data );
    memset( data, 0, 64 );
    CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, WWD_STA_INTERFACE ) );

    memcpy( (char *)acp, (char *)host_buffer_get_current_piece_data_pointer( response ), ( sizeof( edcf_acparam_t ) * 4 ) );
    host_buffer_release(response, WWD_NETWORK_RX);

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_enable_monitor_mode( void )
{
    /* Enable allmulti mode */
    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_ALLMULTI, 1, WWD_STA_INTERFACE ) );

    /* Enable monitor mode */
    CHECK_RETURN( wwd_wifi_set_ioctl_value( WLC_SET_MONITOR, 1, WWD_STA_INTERFACE ) );

    wwd_sdpcm_monitor_mode_enabled = WICED_TRUE;

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_disable_monitor_mode( void )
{
    /* Disable allmulti mode */
    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_ALLMULTI, 0, WWD_STA_INTERFACE ) );

    /* Disable monitor mode */
    CHECK_RETURN( wwd_wifi_set_ioctl_value( WLC_SET_MONITOR, 0, WWD_STA_INTERFACE ) );

    wwd_sdpcm_monitor_mode_enabled = WICED_FALSE;
    return WWD_SUCCESS;
}

wiced_bool_t wwd_wifi_monitor_mode_is_enabled( void )
{
    return wwd_sdpcm_monitor_mode_enabled;
}

wwd_result_t wwd_wifi_get_bssid( wiced_mac_t* bssid )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    wwd_result_t   result;

    memset( bssid, 0, sizeof( wiced_mac_t ) );

    CHECK_IOCTL_BUFFER( wwd_sdpcm_get_ioctl_buffer( &buffer, sizeof( wiced_mac_t ) ) );
    if ( ( result = wwd_sdpcm_send_ioctl( SDPCM_GET, WLC_GET_BSSID, buffer, &response, WWD_STA_INTERFACE ) ) == WWD_SUCCESS )
    {
        memcpy( bssid->octet, host_buffer_get_current_piece_data_pointer( response ), sizeof( wiced_mac_t ) );
        host_buffer_release( response, WWD_NETWORK_RX );
    }

    return result;
}

wwd_result_t wwd_wifi_set_preferred_association_band( int32_t band )
{
    RETURN_WITH_ASSERT( wwd_wifi_set_ioctl_value( WLC_SET_ASSOC_PREFER, (uint32_t)band, WWD_STA_INTERFACE ) );
}

wwd_result_t wwd_wifi_get_preferred_association_band( int32_t* band )
{
    wiced_assert("Bad args", band != NULL);
    return wwd_wifi_get_ioctl_value( WLC_GET_ASSOC_PREFER, (uint32_t*)band, WWD_STA_INTERFACE );
}


/******************************************************
 *             Wiced-internal functions
 ******************************************************/

/** Set the Wi-Fi device down. Internal use only
 *
 * @param interface
 * @return
 */
wwd_result_t wwd_wifi_set_down( void )
{
    if (wwd_wlan_status.state != WLAN_UP)
    {
        WPRINT_WWD_INFO(("wwd_wifi_set_down: already down.\n"));
        return WWD_INTERFACE_NOT_UP;
    }

    /* Send DOWN command */
    CHECK_RETURN( wwd_wifi_set_ioctl_void( WLC_DOWN, WWD_STA_INTERFACE ) );

    /* Update wlan status */
    wwd_wlan_status.state = WLAN_DOWN;

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_set_up( void )
{
    if (wwd_wlan_status.state == WLAN_UP)
    {
        WPRINT_WWD_INFO(("wwd_wifi_set_up: already up.\n"));
        return WWD_SUCCESS;
    }

    /* Send UP command */
    CHECK_RETURN( wwd_wifi_set_ioctl_void( WLC_UP, WWD_STA_INTERFACE ) );

    /* Update wlan status */
    wwd_wlan_status.state = WLAN_UP;

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_manage_custom_ie( wwd_interface_t interface, wiced_custom_ie_action_t action, /*@unique@*/ const uint8_t* oui, uint8_t subtype, const void* data, uint16_t length, uint16_t which_packets)
{
    wiced_buffer_t    buffer;
    vndr_ie_setbuf_t* ie_setbuf;
    uint32_t*         iovar_data;

    wiced_assert("Bad Args", oui != NULL);

    iovar_data = (uint32_t*)wwd_sdpcm_get_iovar_buffer(&buffer, (uint16_t)(sizeof(vndr_ie_setbuf_t) + length + 4), "bsscfg:" IOVAR_STR_VENDOR_IE );
    CHECK_IOCTL_BUFFER( iovar_data );
    *iovar_data = wwd_get_bss_index( interface );
    ie_setbuf = (vndr_ie_setbuf_t*)(iovar_data+1);

    /* Copy the vndr_ie SET command ("add"/"del") to the buffer */
    if (action == WICED_ADD_CUSTOM_IE)
    {
        memcpy( (char*)ie_setbuf->cmd, "add", 3 );
    }
    else
    {
        memcpy( (char*)ie_setbuf->cmd, "del", 3 );
    }
    ie_setbuf->cmd[3] = 0;

    /* Set the values */
    ie_setbuf->vndr_ie_buffer.iecount = (int32_t) 1;

    ie_setbuf->vndr_ie_buffer.vndr_ie_list[0].pktflag = which_packets;
    ie_setbuf->vndr_ie_buffer.vndr_ie_list[0].vndr_ie_data.id  = 0xdd;
    ie_setbuf->vndr_ie_buffer.vndr_ie_list[0].vndr_ie_data.len = (uint8_t)(length + sizeof(ie_setbuf->vndr_ie_buffer.vndr_ie_list[0].vndr_ie_data.oui) + 1); /* +1: one byte for sub type */

    /*@-usedef@*/ /* Stop lint warning about vndr_ie_list array element not yet being defined */
    memcpy( ie_setbuf->vndr_ie_buffer.vndr_ie_list[0].vndr_ie_data.oui, oui, (size_t)WIFI_IE_OUI_LENGTH );
    /*@+usedef@*/

    ie_setbuf->vndr_ie_buffer.vndr_ie_list[0].vndr_ie_data.data[0] = subtype;

    memcpy(&ie_setbuf->vndr_ie_buffer.vndr_ie_list[0].vndr_ie_data.data[1], data, length);

    RETURN_WITH_ASSERT( wwd_sdpcm_send_iovar(SDPCM_SET, buffer, NULL, WWD_STA_INTERFACE ) );
}


void wwd_wifi_prioritize_acparams( const edcf_acparam_t *acp, int *priority )
{
    int aci;
    int aifsn;
    int ecwmin;
    int ecwmax;
    int acm;
    int ranking_basis[AC_COUNT];
    int *p;
    int i;

    p = priority;

    for (aci = 0; aci < AC_COUNT; aci++, acp++, p++)
    {
        aifsn  = acp->ACI & EDCF_AIFSN_MASK;
        acm = (acp->ACI & EDCF_ACM_MASK) ? 1 : 0;
        ecwmin = acp->ECW & EDCF_ECWMIN_MASK;
        ecwmax = (acp->ECW & EDCF_ECWMAX_MASK) >> EDCF_ECWMAX_SHIFT;
        ranking_basis[aci] = aifsn + ecwmin + ecwmax; /* Default AC_VO will be the littlest ranking value */

        /* If ACM is set, STA can't use this AC as per 802.11. Change the ranking to BE */
        if ((aci > 0) && (acm == 1))
            ranking_basis[aci] = ranking_basis[0];
        *p = 1; /* Initialise priority starting at 1 */
    }

    p = priority;

    /* Primitive ranking method which works for AC priority swapping when values for cwmin, cwmax and aifsn are varied */
    for (aci = 0; aci < AC_COUNT; aci++, p++) /* Compare each ACI against each other ACI */
    {
        for (i = 0; i < AC_COUNT; i++)
        {
            if ( i != aci )
            {
                /* Smaller ranking value has higher priority, so increment priority for each ACI which has a higher ranking value */
                if (ranking_basis[aci] < ranking_basis[i])
                {
                    *p = *p + 1;
                }
            }
        }
    }

    // Now invert so that highest priority is the lowest number, e.g. for default settings voice will be priority 1 and background will be priority 4
    p = priority;
    for (i = 0; i < AC_COUNT; i++, p++)
    {
        *p = AC_COUNT + 1 - *p;
    }
}

wwd_result_t wwd_wifi_update_tos_map( void )
{
    edcf_acparam_t ac_params[AC_COUNT];
    const wiced_qos_access_category_t tos_to_ac_map[8] = { WMM_AC_BE, WMM_AC_BK, WMM_AC_BK, WMM_AC_BE, WMM_AC_VI, WMM_AC_VI, WMM_AC_VO, WMM_AC_VO };
    wiced_qos_access_category_t ac;
    wiced_bool_t admission_control_mandatory[AC_COUNT];
    int i, j;

    if ( wwd_wifi_get_acparams_sta( ac_params ) != WWD_SUCCESS )
    {
        return WWD_WLAN_ERROR;
    }

    /* For each access category, record whether admission control is necessary or not */
    for (i = 0; i < AC_COUNT; i++)
    {
        j = (ac_params[i].ACI & EDCF_ACI_MASK) >> EDCF_ACI_SHIFT; // Determine which access category (in case they're out of order)
        if ( ( ac_params[i].ACI & EDCF_ACM_MASK ) == 0 )
        {
            admission_control_mandatory[j] = WICED_FALSE;
        }
        else
        {
            admission_control_mandatory[j] = WICED_TRUE;
        }
    }

    /* For each type of service value look up the Access Category that is mapped to this type of service and update the TOS map
     * with what the AP actually allows, if necessary */
    for (i = 0; i < 8; i++ )
    {
        ac = tos_to_ac_map[i];
        if ( admission_control_mandatory[ac] == WICED_FALSE ) /* No need to re-map to lower priority */
        {
            wwd_tos_map[i] = (uint8_t)i;
        }
        else
        {
            if ( ac == WMM_AC_VO )
            {
                if ( admission_control_mandatory[WMM_AC_VI] == WICED_FALSE )
                {
                    wwd_tos_map[i] = WMM_AC_VI;
                }
                else if ( admission_control_mandatory[WMM_AC_BE] == WICED_FALSE )
                {
                    wwd_tos_map[i] = WMM_AC_BE;
                }
                else
                {
                    wwd_tos_map[i] = WMM_AC_BK;
                }
            }
            else if ( ac == WMM_AC_VI )
            {
                if ( admission_control_mandatory[WMM_AC_BE] == WICED_FALSE )
                {
                    wwd_tos_map[i] = WMM_AC_BE;
                }
                else
                {
                    wwd_tos_map[i] = WMM_AC_BK;
                }
            }
            else /* Case for best effort and background is the same since we are going to send at lowest priority anyway */
            {
                wwd_tos_map[i] = WMM_AC_BK;
            }
        }
    }

    return WWD_SUCCESS;
}

void wwd_wifi_edcf_ac_params_print( const wiced_edcf_ac_param_t *acp, const int *priority )
{
#ifdef WPRINT_ENABLE_WWD_DEBUG
    int aci;
    int acm, aifsn, ecwmin, ecwmax, txop;
    static const char ac_names[AC_COUNT][6] = {"AC_BE", "AC_BK", "AC_VI", "AC_VO"};

    if ( acp != NULL )
    {
        for (aci = 0; aci < AC_COUNT; aci++, acp++)
        {
            if (((acp->ACI & EDCF_ACI_MASK) >> EDCF_ACI_SHIFT) != aci)
            {
                WPRINT_WWD_ERROR(("Warning: AC params out of order\n"));
            }
            acm = (acp->ACI & EDCF_ACM_MASK) ? 1 : 0;
            aifsn = acp->ACI & EDCF_AIFSN_MASK;
            ecwmin = acp->ECW & EDCF_ECWMIN_MASK;
            ecwmax = (acp->ECW & EDCF_ECWMAX_MASK) >> EDCF_ECWMAX_SHIFT;
            txop = (uint16_t)acp->TXOP;
            WPRINT_WWD_DEBUG(("%s: raw: ACI 0x%x ECW 0x%x TXOP 0x%x\n", ac_names[aci], acp->ACI, acp->ECW, txop));
            WPRINT_WWD_DEBUG(("       dec: aci %d acm %d aifsn %d " "ecwmin %d ecwmax %d txop 0x%x\n", aci, acm, aifsn, ecwmin, ecwmax, txop) );
                /* CWmin = 2^(ECWmin) - 1 */
                /* CWmax = 2^(ECWmax) - 1 */
                /* TXOP = number of 32 us units */
            WPRINT_WWD_DEBUG(("       eff: CWmin %d CWmax %d TXop %dusec\n", EDCF_ECW2CW(ecwmin), EDCF_ECW2CW(ecwmax), EDCF_TXOP2USEC(txop)));
        }
    }

    if ( priority != NULL )
    {
        for (aci = 0; aci < AC_COUNT; aci++, priority++)
        {
            WPRINT_WWD_DEBUG(("%s: ACI %d Priority %d\n", ac_names[aci], aci, *priority));
        }
    }
#else /* ifdef WPRINT_ENABLE_WWD_DEBUG */
    UNUSED_PARAMETER( acp );
    UNUSED_PARAMETER( priority );
#endif /* ifdef WPRINT_ENABLE_WWD_DEBUG */
}


wwd_result_t wwd_wifi_get_channel( wwd_interface_t interface, uint32_t* channel )
{
    wiced_buffer_t  buffer;
    wiced_buffer_t  response;
    channel_info_t* channel_info;

    CHECK_IOCTL_BUFFER( wwd_sdpcm_get_ioctl_buffer( &buffer, sizeof(channel_info_t) ) );

    CHECK_RETURN( wwd_sdpcm_send_ioctl( SDPCM_GET, WLC_GET_CHANNEL, buffer, &response, interface ) );

    channel_info = (channel_info_t*) host_buffer_get_current_piece_data_pointer( response );
    *channel = (uint32_t)channel_info->hw_channel;
    host_buffer_release( response, WWD_NETWORK_RX );
    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_set_channel( wwd_interface_t interface, uint32_t channel )
{
    wiced_buffer_t    buffer;
    uint32_t*         data;
    wl_chan_switch_t* chan_switch;
    wwd_interface_t   temp_interface = interface;

    /* Map P2P interface to either STA or AP interface depending if it's running as group owner or client */
    if ( interface == WWD_P2P_INTERFACE )
    {
        if ( wwd_wifi_p2p_go_is_up == WICED_TRUE )
        {
            temp_interface = WWD_AP_INTERFACE;
        }
        else
        {
            temp_interface = WWD_STA_INTERFACE;
        }
    }

    switch ( temp_interface )
    {
        case WWD_STA_INTERFACE:
            data = (uint32_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, sizeof(uint32_t) );
            CHECK_IOCTL_BUFFER( data );
            *data = channel;
            CHECK_RETURN( wwd_sdpcm_send_ioctl( SDPCM_GET, WLC_SET_CHANNEL, buffer, NULL, temp_interface ) );
            break;

        case WWD_AP_INTERFACE:
            chan_switch = (wl_chan_switch_t*)wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(wl_chan_switch_t), IOVAR_STR_CSA );
            CHECK_IOCTL_BUFFER( chan_switch );
            chan_switch->chspec = (wl_chanspec_t)(WL_CHANSPEC_BW_20 | WL_CHANSPEC_CTL_SB_NONE | channel);
            chan_switch->chspec |= wwd_channel_to_wl_band( channel );
            chan_switch->count  = 1;
            chan_switch->mode   = 1;
            chan_switch->reg    = 0;
            CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, temp_interface ) );
            break;

        case WWD_P2P_INTERFACE:
        case WWD_ETHERNET_INTERFACE:
        case WWD_INTERFACE_MAX:
        case WWD_INTERFACE_FORCE_32_BIT:
        default:
            wiced_assert( "Bad interface", 0 != 0 );
            return WWD_UNKNOWN_INTERFACE;
    }

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_get_channels( wwd_interface_t interface, wl_uint32_list_t* channel_list )
{
    wiced_buffer_t  buffer;
    wiced_buffer_t  response;
    wl_uint32_list_t *list;
    uint16_t buffer_length;

    buffer_length = sizeof(uint32_t) * (WICED_WIFI_MAX_CHANNELS + 1);

    list = (wl_uint32_list_t *)wwd_sdpcm_get_ioctl_buffer( &buffer, buffer_length );
    CHECK_IOCTL_BUFFER( list );

    memset(list, 0, buffer_length);
    list->count = htod32(WICED_WIFI_MAX_CHANNELS);
    CHECK_RETURN( wwd_sdpcm_send_ioctl( SDPCM_GET, WLC_GET_VALID_CHANNELS, buffer, &response, interface ) );

    list = (wl_uint32_list_t *) host_buffer_get_current_piece_data_pointer( response );
    memcpy( channel_list, list, (size_t)MIN(host_buffer_get_current_piece_size(response), buffer_length) );

    host_buffer_release( response, WWD_NETWORK_RX );

    return WWD_SUCCESS;
}


wwd_result_t wwd_wifi_get_counters( wwd_interface_t interface, wiced_counters_t* counters )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    wiced_counters_t* received_counters;

    CHECK_IOCTL_BUFFER( wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(wiced_counters_t), IOVAR_STR_COUNTERS ) );

    CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, interface ) );

    received_counters = (wiced_counters_t*) host_buffer_get_current_piece_data_pointer( response );
    memcpy(counters, received_counters, sizeof(wiced_counters_t));
    host_buffer_release( response, WWD_NETWORK_RX );
    return WWD_SUCCESS;
}

/*
 * format an iovar buffer
 * iovar name is converted to lower case
 */
static wwd_result_t
wwd_iovar_mkbuf( const char *name, char *data, uint32_t datalen, char *iovar_buf, uint16_t buflen )
{
    uint32_t iovar_len;

    iovar_len = strlen(name) + 1;

    /* check for overflow */
    if ( ( iovar_len + datalen ) > buflen ) {
        return WWD_BADARG;
    }

    /* copy data to the buffer past the end of the iovar name string */
    if ( datalen > 0 )
        memmove( &iovar_buf[iovar_len], data, datalen );

    /* copy the name to the beginning of the buffer */
    strncpy( iovar_buf, name, ( iovar_len - 1 ) );

    return WWD_SUCCESS;
}

wwd_result_t wwd_get_dump( wwd_interface_t interface, uint8_t *dump_buf, uint16_t len, char *cmd, uint16_t cmd_len )
{
    if ( cmd == NULL ) {
        /* query for the 'dump' without any argument */
        return wwd_wifi_get_iovar_buffer_with_param( "dump", NULL, 0,
                    (uint8_t *)dump_buf, len, interface );
    } else {
        return wwd_wifi_get_iovar_buffer_with_param( "dump", cmd, cmd_len,
                   (uint8_t *)dump_buf, len, interface );
    }
}

wwd_result_t wwd_ampdu_clear_dump( wwd_interface_t interface )
{
    int32_t ret;
    ret =  wwd_wifi_set_iovar_void( "ampdu_clear_dump", interface );

    return ret;
}

wwd_result_t wwd_wifi_get_current_band( wwd_interface_t interface, uint32_t *current_band )
{
    uint32_t band;
    uint32_t bandlist[3];

    /* Get band */
    CHECK_RETURN ( wwd_wifi_get_ioctl_value( WLC_GET_BAND, &band, interface ) );

    /* Get supported Band List */
    CHECK_RETURN ( wwd_wifi_get_ioctl_buffer( WLC_GET_BANDLIST, ( uint8_t * )bandlist, sizeof(bandlist), interface ) );

    /* only support a maximum of 2 bands */
    if (!bandlist[0])
        return WWD_WLAN_ERROR;
    else if (bandlist[0] > 2)
        bandlist[0] = 2;

    switch (band) {
    /* In case of auto band selection the current associated band will be in the entry 1 */
    case WLC_BAND_AUTO :
        if ( bandlist[1] == WLC_BAND_5G )
            *current_band = WLC_BAND_5G;
        else if ( bandlist[1] == WLC_BAND_2G )
            *current_band = WLC_BAND_2G;
        else
            return WWD_WLAN_ERROR;

        break;

    case WLC_BAND_5G :
        *current_band = WLC_BAND_5G;
        break;

    case WLC_BAND_2G :
        *current_band = WLC_BAND_2G;
        break;

    default :
        return WWD_WLAN_ERROR;
        break;
    }

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_get_bw( uint32_t *bwidth )
{
    wl_bss_info_t      bss;
    wiced_security_t   sec;

    wwd_wifi_get_ap_info( &bss, &sec );

    switch( bss.chanspec & WL_CHANSPEC_BW_MASK ) {
#ifdef WL_CHANSPEC_BW_5
    case WL_CHANSPEC_BW_5 :
        *bwidth = 5;
        break;
#endif
    case WL_CHANSPEC_BW_10 :
        *bwidth = 10;
        break;

    case WL_CHANSPEC_BW_20 :
        *bwidth = 20;
        break;

    case WL_CHANSPEC_BW_40 :
        *bwidth = 40;
        break;
#ifdef WL_CHANSPEC_BW_80
    case WL_CHANSPEC_BW_80 :
        *bwidth = 80;
        break;
#endif
#ifdef WL_CHANSPEC_BW_160
    case WL_CHANSPEC_BW_160 :
        *bwidth = 160;
        break;
#endif
#ifdef WL_CHANSPEC_BW_8080
    case WL_CHANSPEC_BW_8080 :
        *bwidth = 8080;
        break;
#endif
    default :
        return WWD_WLAN_ERROR;
        break;
    }

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_get_pm_mode( wwd_interface_t interface, uint32_t* pmmode )
{
   CHECK_RETURN ( wwd_wifi_get_ioctl_value( WLC_GET_PM, pmmode, interface ) );

   return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_get_rate( wwd_interface_t interface, uint32_t* rate )
{
    CHECK_RETURN ( wwd_wifi_get_ioctl_value( WLC_GET_RATE, rate, interface ) );

    if ( *rate == (uint32_t)-1 )
    {
        *rate = 0;
    }

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_set_band_specific_rate( wwd_interface_t interface, uint32_t rate )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    uint32_t band;
    uint32_t bandlist[3];
    char aname[] = IOVAR_STR_ARATE;
    char bgname[] = IOVAR_STR_BGRATE;
    char *name;
    int32_t *data;

    /* Get band */
    CHECK_IOCTL_BUFFER( wwd_sdpcm_get_ioctl_buffer( &buffer, sizeof(uint32_t) ) );
    CHECK_RETURN (wwd_sdpcm_send_ioctl( SDPCM_GET, WLC_GET_BAND, buffer, &response, interface ));
    band = *(uint32_t*) host_buffer_get_current_piece_data_pointer( response );
    host_buffer_release(response, WWD_NETWORK_RX);

    /* Get supported Band List */
    CHECK_IOCTL_BUFFER( wwd_sdpcm_get_ioctl_buffer( &buffer, sizeof(bandlist) ) );
    CHECK_RETURN (wwd_sdpcm_send_ioctl( SDPCM_GET, WLC_GET_BANDLIST, buffer, &response, interface ));
    memcpy(bandlist, (uint32_t*) host_buffer_get_current_piece_data_pointer( response ), sizeof(bandlist));
    host_buffer_release(response, WWD_NETWORK_RX);

    /* only support a maximum of 2 bands */
    if (!bandlist[0])
        return WWD_WLAN_ERROR;
    else if (bandlist[0] > 2)
        bandlist[0] = 2;

    switch (band) {
    /* In case of auto band selection the current associated band will be in the entry 1 */
    case WLC_BAND_AUTO :
        if (bandlist[1] == WLC_BAND_5G)
            name = (char *)aname;
        else if (bandlist[1] == WLC_BAND_2G)
            name = (char *)bgname;
        else
            return WWD_WLAN_ERROR;

        break;

    case WLC_BAND_5G :
        name = (char *)aname;
        break;

    case WLC_BAND_2G :
        name = (char *)bgname;
        break;

    default :
        return WWD_WLAN_ERROR;
        break;
    }

    data = (int32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(int32_t), name );
    CHECK_IOCTL_BUFFER( data );
    if (rate == 0)
        *data = -1; // Auto rate
    else
        *data = (int32_t)rate;

    RETURN_WITH_ASSERT( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, WWD_STA_INTERFACE ) );
}

wwd_result_t wwd_wifi_get_supported_band_list( wiced_band_list_t* band_list )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;

    /* Get supported Band List */
    CHECK_IOCTL_BUFFER( wwd_sdpcm_get_ioctl_buffer( &buffer, sizeof(wiced_band_list_t) ) );
    CHECK_RETURN (wwd_sdpcm_send_ioctl( SDPCM_GET, WLC_GET_BANDLIST, buffer, &response, WWD_STA_INTERFACE ));
    memcpy(band_list, (uint32_t*) host_buffer_get_current_piece_data_pointer( response ), sizeof(wiced_band_list_t));
    host_buffer_release(response, WWD_NETWORK_RX);

    if ( band_list->number_of_bands == 0 )
    {
        return WWD_WLAN_ERROR;
    }

    return WWD_SUCCESS;
}


/* STF parameters are not yet supported - FIXME*/
wwd_result_t wwd_wifi_set_mcs_rate( wwd_interface_t interface, int32_t mcs, wiced_bool_t mcsonly )
{
    uint32_t nrate = 0;

    if ( mcs != -1 )
    {
        nrate |= mcs & NRATE_RATE_MASK;
        nrate |= NRATE_MCS_INUSE;

        if ( WICED_TRUE == mcsonly )
        {
            nrate |= NRATE_OVERRIDE_MCS_ONLY;
        }

        RETURN_WITH_ASSERT( wwd_wifi_set_iovar_value( IOVAR_STR_NRATE, (uint32_t)nrate, interface ) );
    }
    else
    {
        /* reset rates to auto */
        RETURN_WITH_ASSERT( wwd_wifi_set_band_specific_rate( interface, 0 ) );
    }
}

wwd_result_t wwd_wifi_set_legacy_rate( wwd_interface_t interface, int32_t rate )
{
    uint32_t nrate = 0;

    if (rate != 0)
    {
        nrate |= rate & NRATE_RATE_MASK;
        RETURN_WITH_ASSERT( wwd_wifi_set_iovar_value( IOVAR_STR_NRATE, nrate, interface ) );
    }
    else
    {
        /* reset rates to auto */
        RETURN_WITH_ASSERT( wwd_wifi_set_iovar_value( IOVAR_STR_BGRATE, (uint32_t)-1, interface ) );
    }
}

wwd_result_t wwd_wifi_set_11n_support( wwd_interface_t interface, wiced_11n_support_t value )
{
    /* Assert when WLAN is UP */
    wiced_assert("WLAN is UP. Ensure WLAN is DOWN before invoking this API", wwd_wlan_status.state == WLAN_DOWN );

    /* Configure PHY for N-Mode */
    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_NMODE, (uint32_t)value, interface ) );
    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_set_packet_filter_mode( wiced_packet_filter_mode_t mode )
{
    RETURN_WITH_ASSERT( wwd_wifi_set_iovar_value( IOVAR_STR_PKT_FILTER_MODE, (uint32_t)mode, WWD_STA_INTERFACE ) );
}

wwd_result_t wwd_wifi_add_packet_filter( const wiced_packet_filter_t* settings )
{
    wl_pkt_filter_t* packet_filter;
    wiced_buffer_t   buffer;
    uint32_t         buffer_length = (uint32_t)(( 2 * (uint32_t)settings->mask_size ) + WL_PKT_FILTER_FIXED_LEN + WL_PKT_FILTER_PATTERN_FIXED_LEN);

    packet_filter = (wl_pkt_filter_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t)buffer_length , IOVAR_STR_PKT_FILTER_ADD );
    CHECK_IOCTL_BUFFER( packet_filter );

    /* Copy filter entries */
    packet_filter->id                   = settings->id;
    packet_filter->type                 = 0;
    packet_filter->negate_match         = settings->rule;
    packet_filter->u.pattern.offset     = (uint32_t)settings->offset;
    packet_filter->u.pattern.size_bytes = settings->mask_size;

    /* Copy mask */
    memcpy( packet_filter->u.pattern.mask_and_pattern, settings->mask, settings->mask_size );

    /* Copy filter pattern */
    memcpy( packet_filter->u.pattern.mask_and_pattern + settings->mask_size, settings->pattern, settings->mask_size );

    RETURN_WITH_ASSERT( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, WWD_STA_INTERFACE ) );
}

wwd_result_t wwd_wifi_remove_packet_filter( uint8_t filter_id )
{
    wiced_buffer_t buffer;

    uint32_t* data = (uint32_t*)wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(uint32_t), IOVAR_STR_PKT_FILTER_DELETE );
    CHECK_IOCTL_BUFFER( data );
    *data = (uint32_t)filter_id;
    RETURN_WITH_ASSERT( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, WWD_STA_INTERFACE ) );
}

wwd_result_t wwd_wifi_enable_packet_filter( uint8_t filter_id )
{
    return wwd_wifi_toggle_packet_filter( filter_id, WICED_TRUE );
}

wwd_result_t wwd_wifi_disable_packet_filter( uint8_t filter_id )
{
    return wwd_wifi_toggle_packet_filter( filter_id, WICED_FALSE );
}

wwd_result_t wwd_wifi_toggle_packet_filter( uint8_t filter_id, wiced_bool_t enable )
{
    wiced_buffer_t buffer;

    wl_pkt_filter_enable_t* data = (wl_pkt_filter_enable_t*) wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(wl_pkt_filter_enable_t), IOVAR_STR_PKT_FILTER_ENABLE );
    CHECK_IOCTL_BUFFER( data );
    data->id     = (uint32_t)filter_id;
    data->enable = (uint32_t)enable;
    RETURN_WITH_ASSERT( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, WWD_STA_INTERFACE ) );
}

wwd_result_t wwd_wifi_get_packet_filter_stats( uint8_t filter_id, wiced_packet_filter_stats_t* stats )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;

    uint32_t* data = (uint32_t*)wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(uint32_t) + sizeof(wiced_packet_filter_stats_t), IOVAR_STR_PKT_FILTER_STATS );
    CHECK_IOCTL_BUFFER( data );

    memset( data, 0, sizeof(uint32_t) + sizeof(wiced_packet_filter_stats_t) );
    *data = (uint32_t)filter_id;

    CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, WWD_STA_INTERFACE ) );

    memcpy( (char *)stats, (char *)host_buffer_get_current_piece_data_pointer( response ), ( sizeof(wiced_packet_filter_stats_t) ) );
    host_buffer_release( response, WWD_NETWORK_RX );

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_clear_packet_filter_stats( uint32_t filter_id )
{
    RETURN_WITH_ASSERT( wwd_wifi_set_iovar_value( IOVAR_STR_PKT_FILTER_CLEAR_STATS, (uint32_t)filter_id, WWD_STA_INTERFACE ) );
}


wwd_result_t wwd_wifi_get_packet_filters( uint32_t max_count, uint32_t offset, wiced_packet_filter_t* list,  uint32_t* count_out )
{
    wiced_bool_t          enabled_list;

    *count_out = 0;

    for ( enabled_list = WICED_FALSE; enabled_list <= WICED_TRUE; enabled_list++ )
    {
        uint32_t num_returned = 0;
        wwd_result_t retval;
        retval = wwd_wifi_get_packet_filters_inner(  max_count, offset, list, enabled_list, &num_returned );
        list += num_returned;
        offset = MAX( offset - num_returned, 0 );
        max_count = MAX( max_count - num_returned, 0 );
        *count_out += num_returned;
        CHECK_RETURN( retval );
    }

    return WWD_SUCCESS;
}


static wwd_result_t wwd_wifi_get_packet_filters_inner( uint32_t max_count, uint32_t offset, wiced_packet_filter_t* list, wiced_bool_t enabled_list, uint32_t* count_out )
{
    wiced_buffer_t        buffer;
    wiced_buffer_t        response;
    uint32_t*             data;
    wl_pkt_filter_list_t* filter_list;
    wl_pkt_filter_t*       filter_ptr;
    uint32_t              i;

    wwd_result_t retval = WWD_SUCCESS;
    data = (uint32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, PACKET_FILTER_LIST_BUFFER_MAX_LEN, IOVAR_STR_PKT_FILTER_LIST );
    CHECK_IOCTL_BUFFER( data );
    *data = (uint32_t)enabled_list;

    CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, WWD_STA_INTERFACE ) );

    filter_list  = (wl_pkt_filter_list_t*)host_buffer_get_current_piece_data_pointer( response );
    filter_ptr   = filter_list->filter;

    *count_out = 0;

    for ( i = offset; i < filter_list->num; i++ )
    {
        wl_pkt_filter_t*       in_filter  = (wl_pkt_filter_t*)filter_ptr;
        wiced_packet_filter_t* out_filter = &list[i-offset];

        if ( i >= offset + max_count )
        {
            retval = WWD_PARTIAL_RESULTS;
            break;
        }

        out_filter->id        = in_filter->id;
        out_filter->rule      = ( in_filter->negate_match == 0 ) ? WICED_PACKET_FILTER_RULE_POSITIVE_MATCHING : WICED_PACKET_FILTER_RULE_NEGATIVE_MATCHING;
        out_filter->offset    = (uint16_t)in_filter->u.pattern.offset;
        out_filter->mask_size = (uint16_t)in_filter->u.pattern.size_bytes;
        out_filter->enabled_status = enabled_list;

        out_filter->mask      = NULL;
        out_filter->pattern   = NULL;

        /* Update WL filter pointer */
        filter_ptr = (wl_pkt_filter_t*) ( (char*)filter_ptr + (WL_PKT_FILTER_FIXED_LEN + WL_PKT_FILTER_PATTERN_FIXED_LEN + 2 * in_filter->u.pattern.size_bytes) );

        /* WLAN returns word-aligned filter list */
        filter_ptr = (wl_pkt_filter_t*) ROUND_UP( (unsigned long)filter_ptr, 4 );

        (*count_out)++;
    }

    host_buffer_release( response, WWD_NETWORK_RX );
    return retval;
}


wwd_result_t wwd_wifi_get_packet_filter_mask_and_pattern( uint32_t filter_id, uint32_t max_size, uint8_t* mask, uint8_t* pattern, uint32_t* size_out )
{
    wiced_bool_t          enabled_list;

    for ( enabled_list = WICED_FALSE; enabled_list <= WICED_TRUE; enabled_list++ )
    {

        wiced_buffer_t        buffer;
        wiced_buffer_t        response;
        uint32_t*             data;
        wl_pkt_filter_list_t* filter_list;
        wl_pkt_filter_t*      filter_ptr;
        uint32_t              i;
        wl_pkt_filter_t*      in_filter;

        data = wwd_sdpcm_get_iovar_buffer( &buffer, PACKET_FILTER_LIST_BUFFER_MAX_LEN, IOVAR_STR_PKT_FILTER_LIST );
        CHECK_IOCTL_BUFFER( data );
        *data = (uint32_t)enabled_list;

        CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, WWD_STA_INTERFACE ) );

        filter_list  = (wl_pkt_filter_list_t*)host_buffer_get_current_piece_data_pointer( response );
        filter_ptr   = filter_list->filter;

        for ( i = 0; i < filter_list->num; i++ )
        {
            in_filter  = filter_ptr;

            if ( in_filter->id == filter_id )
            {
                *size_out = MIN( in_filter->u.pattern.size_bytes, max_size );
                memcpy ( mask,    in_filter->u.pattern.mask_and_pattern, *size_out );
                memcpy ( pattern, in_filter->u.pattern.mask_and_pattern + in_filter->u.pattern.size_bytes, *size_out );
                host_buffer_release( response, WWD_NETWORK_RX );
                if ( *size_out < in_filter->u.pattern.size_bytes )
                {
                    return WWD_PARTIAL_RESULTS;
                }
                return WWD_SUCCESS;
            }

            /* Update WL filter pointer */
            filter_ptr = (wl_pkt_filter_t*)( (char*)filter_ptr + (WL_PKT_FILTER_FIXED_LEN + WL_PKT_FILTER_PATTERN_FIXED_LEN + 2 * in_filter->u.pattern.size_bytes) );

            /* WLAN returns word-aligned filter list */
            filter_ptr = (wl_pkt_filter_t*) ROUND_UP( (unsigned long)filter_ptr, 4 );
        }
        host_buffer_release( response, WWD_NETWORK_RX );
    }
    return WWD_FILTER_NOT_FOUND;
}



wwd_result_t wwd_wifi_add_keep_alive( const wiced_keep_alive_packet_t* keep_alive_packet_info )
{
    wiced_buffer_t buffer;

    uint16_t length = (uint16_t)(keep_alive_packet_info->packet_length + WL_MKEEP_ALIVE_FIXED_LEN);
    wl_mkeep_alive_pkt_t* packet_info = (wl_mkeep_alive_pkt_t*) wwd_sdpcm_get_iovar_buffer( &buffer, length, IOVAR_STR_MKEEP_ALIVE );
    CHECK_IOCTL_BUFFER( packet_info );

    packet_info->version       = htod16(WL_MKEEP_ALIVE_VERSION);
    packet_info->length        = htod16(WL_MKEEP_ALIVE_FIXED_LEN);
    packet_info->keep_alive_id = keep_alive_packet_info->keep_alive_id;
    packet_info->period_msec   = htod32(keep_alive_packet_info->period_msec);
    packet_info->len_bytes     = htod16(keep_alive_packet_info->packet_length);
    memcpy(packet_info->data, keep_alive_packet_info->packet, keep_alive_packet_info->packet_length);

    RETURN_WITH_ASSERT( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, WWD_STA_INTERFACE ) );
}

wwd_result_t wwd_wifi_get_keep_alive( wiced_keep_alive_packet_t* keep_alive_packet_info )
{
    wl_mkeep_alive_pkt_t* packet_info;
    wiced_buffer_t        buffer;
    wiced_buffer_t        response;
    uint32_t*             data;
    uint16_t              max_info_length = (uint16_t)(WL_MKEEP_ALIVE_FIXED_LEN + keep_alive_packet_info->packet_length);

    wiced_assert("Bad args", (keep_alive_packet_info != NULL) && (keep_alive_packet_info->packet_length > 4) && (keep_alive_packet_info->keep_alive_id <= 3));

    data = (uint32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, max_info_length, IOVAR_STR_MKEEP_ALIVE );  /* get a buffer to store the keep_alive info into */
    CHECK_IOCTL_BUFFER( data );
    memset( data, 0, max_info_length );
    data[0] = keep_alive_packet_info->keep_alive_id;

    CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, WWD_STA_INTERFACE ) );

    packet_info = (wl_mkeep_alive_pkt_t*)host_buffer_get_current_piece_data_pointer( response );
    keep_alive_packet_info->packet_length = packet_info->len_bytes;
    keep_alive_packet_info->period_msec = packet_info->period_msec;
    memcpy( keep_alive_packet_info->packet, packet_info->data, (size_t)MIN(keep_alive_packet_info->packet_length, (host_buffer_get_current_piece_size(response)-WL_MKEEP_ALIVE_FIXED_LEN)) );
    host_buffer_release( response, WWD_NETWORK_RX );

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_disable_keep_alive( uint8_t id )
{
    wiced_keep_alive_packet_t packet_info;
    packet_info.keep_alive_id = id;
    packet_info.period_msec   = 0;
    packet_info.packet_length = 0;
    packet_info.packet        = NULL;
    return wwd_wifi_add_keep_alive( &packet_info );
}

wwd_result_t wwd_wifi_get_associated_client_list( void* client_list_buffer, uint16_t buffer_length )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;

    wiced_maclist_t* data = NULL;

    /* Check if soft AP interface is up, if not, return a count of 0 as a result */
    if ( wwd_wifi_is_ready_to_transceive( WWD_AP_INTERFACE ) == WWD_SUCCESS )
    {
        data = (wiced_maclist_t*)wwd_sdpcm_get_ioctl_buffer( &buffer, buffer_length );
        CHECK_IOCTL_BUFFER( data );
        memset(data, 0, buffer_length);
        data->count = ((wiced_maclist_t*)client_list_buffer)->count;

        CHECK_RETURN( wwd_sdpcm_send_ioctl( SDPCM_GET, WLC_GET_ASSOCLIST, buffer, &response, WWD_AP_INTERFACE ) );

        memcpy( client_list_buffer, (void*) host_buffer_get_current_piece_data_pointer( response ), (size_t)MIN(host_buffer_get_current_piece_size(response), buffer_length) );

        host_buffer_release( response, WWD_NETWORK_RX );
    }
    else
    {
        /* not up, so can't have associated clients */
        ((wiced_maclist_t*)client_list_buffer)->count = 0;
    }
    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_get_max_associations( uint32_t* max_assoc )
{
    return wwd_wifi_get_iovar_value( IOVAR_STR_MAX_ASSOC, max_assoc, WWD_STA_INTERFACE );
}

wwd_result_t wwd_wifi_test_credentials( wiced_scan_result_t* ap, const uint8_t* security_key, uint8_t key_length )
{
    host_semaphore_type_t semaphore;
    wwd_result_t          result;
    uint32_t              previous_softap_channel = 0;

    host_rtos_init_semaphore( &semaphore );

    /* Check if soft AP interface is up, if so, record its current channel */
    if ( wwd_wifi_is_ready_to_transceive( WWD_AP_INTERFACE ) == WWD_SUCCESS )
    {
        wwd_wifi_get_channel( WWD_AP_INTERFACE, &previous_softap_channel );
    }

    /* Try and join the AP with the credentials provided, but only wait for a short time */
    result = wwd_wifi_join_specific( ap, security_key, key_length, &semaphore, WWD_STA_INTERFACE );
    if ( result != WWD_SUCCESS )
    {
        goto test_credentials_deinit;
    }

    result = host_rtos_get_semaphore( &semaphore, WICED_CREDENTIAL_TEST_TIMEOUT, WICED_FALSE );

    /* Immediately leave so we can go back to the original soft AP channel */
    wwd_wifi_leave( WWD_STA_INTERFACE );

    /* If applicable, move the soft AP back to its original channel */
    if ( ( previous_softap_channel != 0 ) && ( previous_softap_channel != ap->channel ) )
    {
        wwd_wifi_set_channel( WWD_AP_INTERFACE, previous_softap_channel );
    }
test_credentials_deinit:
    host_rtos_deinit_semaphore( &semaphore );

    return result;
}

wwd_result_t wwd_wifi_get_ap_info( wl_bss_info_t* ap_info, wiced_security_t* security )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    uint32_t       *data;
    wwd_result_t   result = WWD_SUCCESS;
    uint32_t       security_value; /* hold misc security values */

    /* Read the BSS info */
    data = (uint32_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, sizeof(wl_bss_info_t) + 4 );
    CHECK_IOCTL_BUFFER( data );
    *data = sizeof(wl_bss_info_t) + 4;
    CHECK_RETURN( wwd_sdpcm_send_ioctl( SDPCM_GET, WLC_GET_BSS_INFO, buffer, &response, WWD_STA_INTERFACE ) );

    memcpy( ap_info, (void*) (host_buffer_get_current_piece_data_pointer( response ) + 4), sizeof(wl_bss_info_t) );
    host_buffer_release( response, WWD_NETWORK_RX );

    /* Read the WSEC setting */
    CHECK_RETURN( wwd_wifi_get_ioctl_value( WLC_GET_WSEC, &security_value, WWD_STA_INTERFACE ) );

    *security = (wiced_security_t)(security_value);

    if ( *security == WICED_SECURITY_WEP_PSK )
    {
        /* Read the WEP auth setting */
        CHECK_RETURN( wwd_wifi_get_ioctl_value( WLC_GET_AUTH, &security_value, WWD_STA_INTERFACE ) );

        if ( security_value == SHARED_AUTH )
        {
            *security |= SHARED_ENABLED;
        }
    }
    else if ( (*security & (TKIP_ENABLED | AES_ENABLED) ) != 0 )
    {
        /* Read the WPA auth setting */
        CHECK_RETURN( wwd_wifi_get_ioctl_value( WLC_GET_WPA_AUTH, &security_value, WWD_STA_INTERFACE ) );

        if ( security_value == WPA2_AUTH_PSK )
        {
            *security |= WPA2_SECURITY;
        }
        else if ( security_value == WPA_AUTH_PSK )
        {
            *security |= WPA_SECURITY;
        }
    }
    else if ( *security != WICED_SECURITY_OPEN )
    {
        *security = WICED_SECURITY_UNKNOWN;
        result    = WWD_UNKNOWN_SECURITY_TYPE;
    }

    return result;
}

wwd_result_t wwd_wifi_set_ht_mode( wwd_interface_t interface, wiced_ht_mode_t ht_mode )
{
    /* Assert when WLAN is UP */
    wiced_assert("WLAN is UP. Ensure WLAN is DOWN before invoking this API", wwd_wlan_status.state == WLAN_DOWN );

    /* Set HT40 intolerance bit if the mode is HT20 */
    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_HT40_INTOLERANCE, (uint32_t)( ht_mode == WICED_HT_MODE_HT20 ) ? 1 : 0, interface ) );

    /* Set HT mode */
    CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_MIMO_BW_CAP, (uint32_t)ht_mode, interface ) );

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_get_ht_mode( wwd_interface_t interface, wiced_ht_mode_t* ht_mode )
{
    uint32_t      data;

    /* Validate the input arguments */
    wiced_assert("Bad args", ht_mode != NULL);

    CHECK_RETURN( wwd_wifi_get_iovar_value( IOVAR_STR_MIMO_BW_CAP, &data, interface ) );
    *ht_mode = (wiced_ht_mode_t)(data);

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_enable_sup_set_passphrase( const uint8_t* security_key_psk, uint8_t psk_length, wiced_security_t auth_type )
{
    wiced_interface_t interface;
    wiced_buffer_t buffer;
    uint32_t*      data;
    uint32_t       bss_index = 0;

    wiced_get_default_ready_interface( &interface );

    if ( ( ( psk_length > (uint8_t) WSEC_MAX_PSK_LEN ) ||
           ( psk_length < (uint8_t) WSEC_MIN_PSK_LEN ) ) &&
         ( auth_type == WICED_SECURITY_WPA3_WPA2_PSK ) )
    {
        return WWD_INVALID_KEY;
    }

    /* Map the interface to a BSS index */
    bss_index = wwd_get_bss_index( interface );

    /* Set supplicant variable - mfg app doesn't support these iovars, so don't care if return fails */
    data = wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 8, "bsscfg:" IOVAR_STR_SUP_WPA );
    CHECK_IOCTL_BUFFER( data );
    data[0] = bss_index;
    data[1] = (uint32_t) ( ( ( ( auth_type & WPA_SECURITY )  != 0 ) ||
                           ( ( auth_type & WPA2_SECURITY ) != 0 ) ||
                             ( auth_type & WPA3_SECURITY ) != 0 ) ? 1 : 0 );
    (void)wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );

    CHECK_RETURN( wwd_wifi_set_passphrase( security_key_psk, psk_length, interface ) );

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_set_passphrase( const uint8_t* security_key, uint8_t key_length, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    wsec_pmk_t* psk = (wsec_pmk_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, sizeof(wsec_pmk_t) );

    CHECK_IOCTL_BUFFER( psk );
    memset( psk, 0, sizeof(wsec_pmk_t) );
    memcpy( psk->key, security_key, key_length );
    psk->key_len = key_length;
    psk->flags = (uint16_t) WSEC_PASSPHRASE;

    host_rtos_delay_milliseconds( 1 ); /* Delay required to allow radio firmware to be ready to receive PMK and avoid intermittent failure */

    return wwd_sdpcm_send_ioctl( SDPCM_SET, WLC_SET_WSEC_PMK, buffer, 0, interface );
}

wwd_result_t wwd_wifi_sae_password( const uint8_t* security_key, uint8_t key_length, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    wsec_sae_password_t* sae_password = (wsec_sae_password_t*) wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(wsec_sae_password_t), IOVAR_STR_SAE_PASSWORD);

    CHECK_IOCTL_BUFFER( sae_password );
    memset( sae_password, 0, sizeof(wsec_sae_password_t) );
    memcpy( sae_password->password, security_key, key_length );
    sae_password->password_len = key_length;
    host_rtos_delay_milliseconds( 1 ); /* Delay required to allow radio firmware to be ready to receive PMK and avoid intermittent failure */
    return wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, interface );
}

wwd_result_t wwd_wifi_get_ioctl_buffer( uint32_t ioctl, uint8_t* out_buffer, uint16_t out_length, wwd_interface_t interface )
{

    wiced_buffer_t buffer;
    uint32_t*      data;
    wiced_buffer_t response;
    wwd_result_t result;

    data = (uint32_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, out_length );
    CHECK_IOCTL_BUFFER( data );

    result = wwd_sdpcm_send_ioctl( SDPCM_GET, ioctl, buffer, &response, interface );

    /* it worked: copy the result to the output buffer */
    if ( WWD_SUCCESS == result )
    {
        data = (uint32_t*) host_buffer_get_current_piece_data_pointer( response );
        memcpy( out_buffer, data, out_length );
        host_buffer_release( response, WWD_NETWORK_RX );
    }

    return result;
}

wwd_result_t wwd_wifi_set_ioctl_buffer( uint32_t ioctl, void *in_buffer, uint16_t in_buffer_length, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    uint32_t*      data;

    data = (uint32_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, in_buffer_length );
    CHECK_IOCTL_BUFFER( data );

    memcpy( data, in_buffer, in_buffer_length );

    return wwd_sdpcm_send_ioctl( SDPCM_SET, ioctl, buffer, NULL, interface );
}

wwd_result_t wwd_wifi_set_iovar_buffers( const char* iovar, const void **in_buffers, const uint16_t* lengths, const uint8_t num_buffers, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    uint32_t*      data;
    int            tot_in_buffer_length = 0;
    uint8_t        buffer_num           = 0;

    /* get total length of all buffers: they will be copied into memory one after the other. */
    for ( ; buffer_num < num_buffers ; buffer_num++ )
    {
        tot_in_buffer_length += lengths[buffer_num];
    }

    /* get a valid buffer */
    data = (uint32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t)tot_in_buffer_length, iovar );
    CHECK_IOCTL_BUFFER( data );

    /* copy all data into buffer */
    for ( buffer_num = 0 ; buffer_num < num_buffers ; buffer_num++ )
    {
        memcpy( data, in_buffers[buffer_num], lengths[buffer_num] );
        data += lengths[buffer_num];
    }

    /* send iovar */
    return wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, interface );
}

wwd_result_t wwd_wifi_set_iovar_buffer( const char* iovar, void *in_buffer, uint16_t in_buffer_length, wwd_interface_t interface )
{
    return wwd_wifi_set_iovar_buffers( iovar, (const void**)&in_buffer, (const uint16_t*)&in_buffer_length, 1, interface );
}

wwd_result_t wwd_wifi_set_iovar_void( const char* iovar, wwd_interface_t interface )
{
    wiced_buffer_t buffer;

    wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 0, iovar );

    return wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, interface );
}

wwd_result_t wwd_wifi_set_iovar_value( const char* iovar, uint32_t value, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    uint32_t*      data;

    data = (uint32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) sizeof(value), iovar );
    CHECK_IOCTL_BUFFER( data );
    *data = value;
    return wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, interface );
}

wwd_result_t wwd_wifi_get_iovar_value( const char* iovar, uint32_t* value, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;

    CHECK_IOCTL_BUFFER( wwd_sdpcm_get_iovar_buffer( &buffer, 4, iovar ) );
    CHECK_RETURN_UNSUPPORTED_OK( wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, interface ) );

    *value = *(uint32_t*) host_buffer_get_current_piece_data_pointer( response );
    host_buffer_release( response, WWD_NETWORK_RX );
    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_get_iovar_buffer( const char* iovar_name, uint8_t* out_buffer, uint16_t out_length, wwd_interface_t interface )
{
    uint32_t* data;
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    wwd_result_t result;

    data = wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t)out_length, iovar_name );
    CHECK_IOCTL_BUFFER( data );

    result = wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, interface );

    /* it worked: copy the result to the output buffer */
    if ( WWD_SUCCESS == result )
    {
        data = (uint32_t*) host_buffer_get_current_piece_data_pointer( response );
        memcpy( out_buffer, data, out_length );
        host_buffer_release( response, WWD_NETWORK_RX );
    }

    return result;
}

wwd_result_t wwd_wifi_get_iovar_buffer_with_param( const char* iovar_name, void *param, uint32_t paramlen, uint8_t* out_buffer, uint32_t out_length, wwd_interface_t interface )
{
    uint32_t* data;
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    wwd_result_t result;

    //Format the input string
    result = wwd_iovar_mkbuf(iovar_name, param, paramlen, (char *)out_buffer, (uint16_t)out_length );
    if (result != WWD_SUCCESS)
        return result;

    data = wwd_sdpcm_get_ioctl_buffer( &buffer, (uint16_t)out_length );

    if (data == NULL)
        return WWD_WLAN_NOMEM;

    memcpy( data, out_buffer, out_length );

    result = (wwd_result_t)wwd_sdpcm_send_ioctl( SDPCM_GET, WLC_GET_VAR, buffer, &response, (wwd_interface_t) interface );

    if (result == WWD_SUCCESS)
    {
        memcpy( out_buffer, host_buffer_get_current_piece_data_pointer( response ), MIN( host_buffer_get_current_piece_size( response ), out_length )  );
        host_buffer_release(response, WWD_NETWORK_RX);
    }

    return result;
}

wwd_result_t wwd_wifi_set_iovar_buffer_with_param( const char* iovar_name, void *param, uint32_t paramlen, uint8_t* out_buffer, uint32_t out_length, wwd_interface_t interface )
{
    uint32_t* data;
    wiced_buffer_t buffer;
    wwd_result_t result;

    /* Format the input string */
    result = wwd_iovar_mkbuf( iovar_name, param, paramlen, ( char * )out_buffer, ( uint16_t )out_length );
    if (result != WWD_SUCCESS)
        return result;

    data = wwd_sdpcm_get_ioctl_buffer( &buffer, ( uint16_t )out_length );

    if (data == NULL)
        return WWD_WLAN_NOMEM;

    memcpy( data, out_buffer, out_length );

    result = ( wwd_result_t )wwd_sdpcm_send_ioctl( SDPCM_SET, WLC_SET_VAR, buffer, 0, ( wwd_interface_t ) interface );

    if ( result == WWD_SUCCESS )
        host_buffer_release( buffer, WWD_NETWORK_TX );

    return result;
}

wwd_result_t wwd_wifi_set_ioctl_void( uint32_t ioctl, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    uint32_t*      data;

    data = (uint32_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, 0 );
    CHECK_IOCTL_BUFFER( data );

    return wwd_sdpcm_send_ioctl( SDPCM_SET, ioctl, buffer, 0, interface );
}

wwd_result_t wwd_wifi_set_ioctl_value( uint32_t ioctl, uint32_t value, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    uint32_t*      data;

    data = (uint32_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, (uint16_t) sizeof(value) );
    CHECK_IOCTL_BUFFER( data );
    *data = value;
    return wwd_sdpcm_send_ioctl( SDPCM_SET, ioctl, buffer, 0, interface );
}

wwd_result_t wwd_wifi_get_ioctl_value( uint32_t ioctl, uint32_t* value, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;

    CHECK_IOCTL_BUFFER( wwd_sdpcm_get_ioctl_buffer( &buffer, (uint16_t) sizeof(*value) ) );
    CHECK_RETURN_UNSUPPORTED_OK( wwd_sdpcm_send_ioctl( SDPCM_GET, ioctl, buffer, &response, interface ) );

    *value = * (uint32_t*) host_buffer_get_current_piece_data_pointer( response );
    host_buffer_release( response, WWD_NETWORK_RX );
    return WWD_SUCCESS;
}

/*
 *
 *  This function sets Radio Resource Management Capabilities in the WLAN firmware.
 *
 * @param interface                                     : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param radio_resource_management_capability_ie_t     : The data structure to set the different Radio Resource capabilities.
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
wwd_result_t wwd_wifi_set_radio_resource_management_capabilities(wwd_interface_t interface, radio_resource_management_capability_ie_t *rrm_cap )
{
    if ( sizeof(radio_resource_management_capability_ie_t) > RRM_CAPABILITIES_LEN )
    {
            WPRINT_WWD_DEBUG(("wwd_wifi_set_rrm_capabilities: RRM_CAPABILITIES_LEN too long\n"));
            return WWD_WLAN_BADLEN;
    }

    RETURN_WITH_ASSERT( wwd_wifi_set_iovar_buffer( IOVAR_STR_RRM, rrm_cap, sizeof(*rrm_cap), interface ) );
}

/*
 *
 *  This function gets Radio Resource Management Capabilities and parses them and
 *  then passes them to user application to format the data.
 *
 * @param interface                                     : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param radio_resource_management_capability_ie_t     : The data structure get the different Radio Resource capabilities.
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
wwd_result_t wwd_wifi_get_radio_resource_management_capabilities(wwd_interface_t interface, radio_resource_management_capability_ie_t *rrm_cap )
{
    wwd_result_t result;

    if ( sizeof(radio_resource_management_capability_ie_t) > RRM_CAPABILITIES_LEN )
    {
        WPRINT_WWD_DEBUG(("wwd_wifi_get_rrm_capabilities: RRM_CAPABILITIES_LEN too long\n"));
        return WWD_WLAN_BADLEN;
    }

    /* get wwd_wifi_get_radio_resource_management_capabilities buffer */
    result = ( wwd_wifi_get_iovar_buffer( IOVAR_STR_RRM, (void*)rrm_cap, sizeof(*rrm_cap), interface ) );

    if ( result != WWD_SUCCESS )
    {
        WPRINT_WWD_DEBUG(("wwd_wifi_get_radio_resource_management_capabilities failed..result:%d\n", result ));
        return WWD_WLAN_UNSUPPORTED;
    }

    return result;
}

/*
 *
 *  This function sets 11k beacon measurement request in the WLAN firmware.
 *
 * @param interface                                : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param radio_resource_management_beacon_req_t   : pointer to data structure of rrm_bcn_req_t
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
wwd_result_t wwd_wifi_radio_resource_management_beacon_req( wwd_interface_t interface, radio_resource_management_beacon_req_t *rrm_bcn_req )
{
    wwd_result_t result;

    result = ( wwd_wifi_get_iovar_buffer( IOVAR_STR_RRM_BCN_REQ, (void*)rrm_bcn_req, sizeof(*rrm_bcn_req), interface ) );

    if ( result != WWD_SUCCESS )
    {
           WPRINT_WWD_DEBUG(("wwd_wifi_radio_resource_management_beacon_req failed..result:%d\n", result ));
           return WWD_WLAN_UNSUPPORTED;
    }

    return WWD_SUCCESS;
}

/*
 *
 *  This function sets 11k channel load measurement request in the WLAN firmware.
 *
 * @param interface                       : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param radio_resource_management_req_t : pointer to data structure of rrm_chload_req
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
wwd_result_t wwd_wifi_radio_resource_management_channel_load_req( wwd_interface_t interface, radio_resource_management_req_t *rrm_chload_req )
{
    wwd_result_t   result;

    result = wwd_wifi_set_iovar_buffer( IOVAR_STR_RRM_CHLOAD_REQ, rrm_chload_req, sizeof(*rrm_chload_req), interface );

    if ( result != WWD_SUCCESS )
    {
          WPRINT_WWD_DEBUG(("wwd_wifi_radio_resource_management_channel_load_req failed..result:%d\n", result ));
          return WWD_WLAN_UNSUPPORTED;
    }

    return WWD_SUCCESS;
}

/*
 *
 *  This function sets 11k noise measurement request in the WLAN firmware.
 *
 * @param interface                            : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param radio_resource_management_req_t      : pointer to data structure of rrm_noise_req
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
wwd_result_t wwd_wifi_radio_resource_management_noise_req( wwd_interface_t interface, radio_resource_management_req_t *rrm_noise_req )
{
    wwd_result_t   result;

    /* get wwd_wifi_radio_resource_management_noise_req buffer */
    result = wwd_wifi_set_iovar_buffer( IOVAR_STR_RRM_NOISE_REQ, rrm_noise_req, sizeof(*rrm_noise_req), interface );

    if ( result != WWD_SUCCESS )
    {
             WPRINT_WWD_DEBUG(("wwd_wifi_radio_resource_management_noise_req failed..result:%d\n", result ));
             return WWD_WLAN_UNSUPPORTED;
    }

    return WWD_SUCCESS;
}

/*
 *
 *  This function sets 11k frame measurement request in the WLAN firmware.
 *
 * @param interface                            : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param radio_resource_management_framereq_t : pointer to data structure of rrm_framereq
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
wwd_result_t wwd_wifi_radio_resource_management_frame_req( wwd_interface_t interface, radio_resource_management_framereq_t *rrm_framereq )
{
    wwd_result_t   result;

    /* get wwd_wifi_radio_resource_management_frame_req buffer */
    result = wwd_wifi_set_iovar_buffer( IOVAR_STR_RRM_FRAME_REQ, rrm_framereq, sizeof(*rrm_framereq), interface );

    if ( result != WWD_SUCCESS )
    {
           WPRINT_WWD_DEBUG(("wwd_wifi_radio_resource_management_frame_req failed..result:%d\n", result ));
           return WWD_WLAN_UNSUPPORTED;
    }

    return WWD_SUCCESS;
}

/*
 *
 *  This function sets 11k stat measurement request in the WLAN firmware.
 *
 * @param interface                            : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param radio_resource_management_statreq_t  : pointer to data structure of rrm_statreq
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
wwd_result_t wwd_wifi_radio_resource_management_stat_req( wwd_interface_t interface, radio_resource_management_statreq_t *rrm_statreq )
{
    wwd_result_t   result;

    /* get wwd_wifi_radio_resource_management_stat_req buffer */
    result = wwd_wifi_set_iovar_buffer( IOVAR_STR_RRM_STAT_REQ, rrm_statreq, sizeof(*rrm_statreq), interface );

    if ( result != WWD_SUCCESS )
    {
           WPRINT_WWD_DEBUG(("wwd_wifi_radio_resource_management_stat_req failed..result:%d\n", result ));
           return WWD_WLAN_UNSUPPORTED;
    }

    return WWD_SUCCESS;
}

/* Return results of wl cap.  Optionally filter for specific cap/pattern */
wwd_result_t wwd_wifi_get_cap( char *buf, uint16_t buflen, char *cap )
{
    wwd_result_t   result;

    result = wwd_wifi_get_iovar_buffer( IOVAR_STR_CAP, (uint8_t*)buf, (uint32_t)buflen, WWD_STA_INTERFACE );

    if ( result != WWD_SUCCESS )
    {
          WPRINT_WWD_DEBUG(("CAP IOVAR failed, result: %d\n", result ));
          return WWD_WLAN_UNSUPPORTED;
    }

    if ( cap && strnstrn( buf, buflen, cap, (uint16_t)strlen( cap ) ) == NULL )
    {
        result = WWD_WLAN_UNSUPPORTED;
    }

    return result;

}

/*
 *
 *  This function gets 11k neighbor report list works from the WLAN firmware.
 *
 * @param interface      : WWD_AP_INTERFACE (works only in AP mode)
 * @param uint8_t        : buffer pointer to data structure
 * @param uint16_t       : buffer length
 *
 * @returns : status WWD_SUCCESS or failure
 */
wwd_result_t wwd_wifi_radio_resource_management_neighbor_list ( wwd_interface_t interface, uint8_t *buf, uint16_t buflen )
{

    /* get wwd_wifi_radio_resource_management_neighbor_list buffer */
    wwd_result_t result = wwd_wifi_get_iovar_buffer( IOVAR_STR_RRM_NBR_LIST, buf, buflen, interface );

    if ( result != WWD_SUCCESS )
    {
          WPRINT_WWD_DEBUG(("wwd_wifi_radio_resource_management_neighbor_list failed..result:%d\n", result ));
          return WWD_WLAN_UNSUPPORTED;
    }

    return WWD_SUCCESS;
}

/*
 *
 *  This function deletes node from 11k neighbor report list
 *
 * @param interface      : WWD_AP_INTERFACE (works only in AP mode)
 * @param wiced_mac_t    : bssid of the node to be deleted from neighbor report list
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
wwd_result_t wwd_wifi_radio_resource_management_neighbor_del_neighbor( wwd_interface_t interface, wiced_mac_t *bssid )
{

    wwd_result_t   result;

    /* get wwd_wifi_radio_resource_management_neighbor_del_neighbor buffer */
    result = wwd_wifi_set_iovar_buffer( IOVAR_STR_RRM_NBR_DEL, bssid, sizeof(*bssid), interface );
    if ( result != WWD_SUCCESS )
    {
         WPRINT_WWD_DEBUG(("wwd_wifi_radio_resource_management_neighbor_del_neighbor failed..result:%d\n", result ));
         return WWD_WLAN_UNSUPPORTED;
    }

    return WWD_SUCCESS;
}

/*
 *
 *  This function adds a node to  Neighbor list
 *
 * @param interface                                 : WWD_AP_INTERFACE (works only in AP mode)
 * @param radio_resource_management_nbr_element_t   : pointer to the neighbor element data structure.
 * @param buflen                                    : buffer length of the neighbor element data.
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
wwd_result_t wwd_wifi_radio_resource_management_neighbor_add_neighbor( wwd_interface_t interface, radio_resource_management_nbr_element_t *nbr_elt, uint16_t buflen )
{
    wwd_result_t   result;

    /* get wwd_wifi_radio_resource_management_neighbor_add_neighbor buffer */
    result = wwd_wifi_set_iovar_buffer( IOVAR_STR_RRM_NBR_ADD, (void*)nbr_elt, buflen, interface );
    if ( result != WWD_SUCCESS )
    {
         WPRINT_WWD_DEBUG(("wwd_wifi_radio_resource_management_neighbor_add_neighbor failed..result:%d\n", result ));
         return WWD_WLAN_UNSUPPORTED;
    }

    return WWD_SUCCESS;
}

/*
 *
 *  This function sets 11k link measurement request for the particular BSSID in the WLAN firmware.
 *
 * @param interface      : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param wiced_mac_t    : Mac Address of the destination
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
wwd_result_t wwd_wifi_radio_resource_management_link_management_req( wwd_interface_t interface, wiced_mac_t *da )
{
    wwd_result_t   result;

    /* get wwd_wifi_radio_resource_management_link_management_req buffer */
    result = wwd_wifi_set_iovar_buffer( IOVAR_STR_RRM_LM_REQ, (void*)da, sizeof(*da), interface );
    if ( result != WWD_SUCCESS )
    {
            WPRINT_WWD_DEBUG(("wwd_wifi_radio_resource_management_link_management_req failed..result:%d\n", result ));
            return WWD_WLAN_UNSUPPORTED;
    }

    return WWD_SUCCESS;
}

/*
 *
 *  This function sets sets 11k link measurement request for the particular SSID in the WLAN firmware.
 *
 * @param interface       : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param wiced_ssid_t    : The data structure of the SSID.
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
wwd_result_t wwd_wifi_radio_resource_management_neighbor_req( wwd_interface_t interface, wiced_ssid_t *ssid )
{
    wiced_buffer_t buffer;
    uint8_t*      data;
    wwd_result_t   result;

    if ( ssid->length > (size_t) SSID_NAME_SIZE )
    {
           WPRINT_WWD_DEBUG(("wiced_wifi_join: SSID too long\n"));
           return WWD_WLAN_BADSSIDLEN;
    }

    result = wwd_wifi_check_join_status( interface );

    if ( result != WWD_SUCCESS ) {
           WPRINT_WWD_DEBUG(("STA is not associated to an AP\n"));
           return WWD_INTERFACE_NOT_UP;
    }

    /* get wwd_wifi_radio_resource_management_neighbor_req buffer */
    data = (uint8_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) sizeof(*ssid), IOVAR_STR_RRM_NBR_REQ );
    CHECK_IOCTL_BUFFER( data );
    memcpy( data , ssid->value, ssid->length );
    result = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, interface );
    if ( result != WWD_SUCCESS )
    {
          WPRINT_WWD_DEBUG(("wwd_wifi_radio_resource_management_neighbor_req failed..result:%d\n", result ));
          return WWD_WLAN_UNSUPPORTED;
    }

    return WWD_SUCCESS;
}

/*
 *
 *  This function sets/resets the value of FBT(Fast BSS Transition) Over-the-DS(Distribution System)
 *
 * @param interface       : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param set             : If the value 1 then FBT over the DS is allowed
 *                        : if the value is 0 then FBT over the DS is not allowed (over the air is the only option)
 * @param value           : value of the data.
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
wwd_result_t wwd_wifi_fast_bss_transition_over_distribution_system( wwd_interface_t interface, wiced_bool_t set, int *value )
{
    wwd_result_t   result;

    /* get wwd_wifi_fast_bss_transition_over_distribution_system buffer */
    if ( WICED_TRUE == set ) {
        result = wwd_wifi_set_iovar_buffer( IOVAR_STR_FBT_OVER_DS, (uint8_t*)value, sizeof(*value), interface );
        if ( result != WWD_SUCCESS )
        {
                 WPRINT_WWD_DEBUG(("wwd_wifi_fast_bss_transition_over_distribution_system failed..result:%d\n", result ));
                 return WWD_WLAN_UNSUPPORTED;
        }
    }
    else
    {
        result = wwd_wifi_get_iovar_buffer( IOVAR_STR_FBT_OVER_DS, (uint8_t*)value, sizeof(*value), interface );
        if ( result != WWD_SUCCESS )
        {
                WPRINT_WWD_DEBUG(("wwd_wifi_fast_bss_transition_over_distribution_system failed..result:%d\n", result ));
                return WWD_WLAN_UNSUPPORTED;
        }
    }

    return WWD_SUCCESS;
}

/*
 *
 *  This function  returns the value of Fast BSS Transition (1 if Driver 4-way handshake & reassoc  is enabled 1 and 0 if disabled)
 *
 * @param interface           : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param value               : gets value of the Fast BSS Transition capabilities.
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
wwd_result_t wwd_wifi_fast_bss_transition_capabilities( wwd_interface_t interface, wiced_bool_t* enable )
{

    wwd_result_t   result;

    /* get wwd_wifi_fast_bss_transition_capabilities buffer */
    result = wwd_wifi_get_iovar_buffer( IOVAR_STR_FBT_CAPABILITIES, (uint8_t*)enable, sizeof( *enable ), interface );
    if ( result != WWD_SUCCESS )
    {
                  WPRINT_WWD_DEBUG(("wwd_wifi_fast_bss_transition_capabilities failed..result:%d\n", result ));
                  return WWD_WLAN_UNSUPPORTED;
    }

    return WWD_SUCCESS;
}

void convert_security_type_to_string( wiced_security_t security, char *out_str, uint16_t out_str_len )
{

    if ( security == WICED_SECURITY_OPEN )
    {
        strncat(out_str, " Open", out_str_len );
    }
    if ( security & WEP_ENABLED )
    {
        strncat(out_str, " WEP", out_str_len );
    }
    if ( security & WPA3_SECURITY )
    {
        strncat(out_str, " WPA3", out_str_len );
    }
    if ( security & WPA2_SECURITY )
    {
        strncat( out_str, " WPA2", out_str_len );
    }
    if ( security & WPA_SECURITY)
    {
        strncat( out_str, " WPA", out_str_len );
    }
    if ( security & AES_ENABLED )
    {
        strncat( out_str, " AES", out_str_len );
    }
    if ( security & TKIP_ENABLED )
    {
        strncat( out_str, " TKIP", out_str_len );
    }
    if ( security & SHARED_ENABLED)
    {
        strncat( out_str, " SHARED", out_str_len );
    }
    if (security & ENTERPRISE_ENABLED)
    {
        strncat( out_str, " Enterprise", out_str_len );
    }
    if (security & WPS_ENABLED)
    {
        strncat( out_str, " WPS", out_str_len );
    }
    if ( security & FBT_ENABLED )
    {
        strncat( out_str, " FBT", out_str_len );
    }
    if (security & IBSS_ENABLED)
    {
        strncat( out_str, " IBSS", out_str_len );
    }
    if ( security == WICED_SECURITY_UNKNOWN )
    {
        strncat( out_str, " Unknown", out_str_len );
    }
    if( !( security & ENTERPRISE_ENABLED ) && ( security != WICED_SECURITY_OPEN ) && ( security != WICED_SECURITY_UNKNOWN ) )
    {
        strncat( out_str, " PSK", out_str_len );
    }
}

/*!
 ******************************************************************************
 * Prints partial details of a scan result on a single line
 *
 * @param[in] record  A pointer to the wiced_scan_result_t record
 *
 */
void print_scan_result( wiced_scan_result_t* record )
{
    const char *str = NULL;
    char sec_type_string[40] = {0};

    switch (record->bss_type) {
    case WICED_BSS_TYPE_ADHOC:
        str = "Adhoc";
        break;
    case WICED_BSS_TYPE_INFRASTRUCTURE:
        str = "Infra";
        break;
    case WICED_BSS_TYPE_ANY:
        str = "Any";
        break;
    case WICED_BSS_TYPE_UNKNOWN:
        str = "Unknown";
        break;
    case WICED_BSS_TYPE_MESH:
        str = "Mesh";
        break;
    default:
        str = "?";
        break;
    }
    WPRINT_APP_INFO( ( "%5s ", str));
    WPRINT_APP_INFO( ( "%02X:%02X:%02X:%02X:%02X:%02X ", record->BSSID.octet[0], record->BSSID.octet[1], record->BSSID.octet[2], record->BSSID.octet[3], record->BSSID.octet[4], record->BSSID.octet[5] ) );

    if ( record->flags & WICED_SCAN_RESULT_FLAG_RSSI_OFF_CHANNEL )
    {
         WPRINT_APP_INFO( ( "OFF " ) );
    }
    else
    {
         WPRINT_APP_INFO( ( "%d ", record->signal_strength ) );
    }

    if ( record->max_data_rate < 100000 )
    {
        WPRINT_APP_INFO( ( " %.1f ", (double) (record->max_data_rate / 1000.0) ) );
    }
    else
    {
        WPRINT_APP_INFO( ( "%.1f ", (double) (record->max_data_rate / 1000.0) ) );
    }
    WPRINT_APP_INFO( ( " %3d  ", record->channel ) );

    convert_security_type_to_string(record->security, sec_type_string, (sizeof(sec_type_string)-1));

    WPRINT_APP_INFO( ( "%-20s ", sec_type_string));
    WPRINT_APP_INFO( ( " %-32s ", record->SSID.value ) );

    if ( record->ccode[0] != '\0' )
    {
        WPRINT_APP_INFO( ( "%c%c    ", record->ccode[0], record->ccode[1]) );
    }
    else
    {
        WPRINT_APP_INFO( ( "      " ) );
    }

    if ( record->flags & WICED_SCAN_RESULT_FLAG_BEACON )
    {
        WPRINT_APP_INFO( ( " %-15s", " BEACON" ) );
    }
    else
    {
        WPRINT_APP_INFO( ( " %-15s", " PROBE " ) );
    }
    WPRINT_APP_INFO( ( "\n" ) );
}

wwd_result_t wwd_wifi_get_supplicant_eapol_key_timeout( wwd_interface_t interface, int32_t* eapol_key_timeout )
{
    CHECK_RETURN( wwd_wifi_get_iovar_buffer( IOVAR_STR_SUP_WPA_TMO, (uint8_t*)eapol_key_timeout, sizeof(*eapol_key_timeout), interface ) );

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_set_supplicant_eapol_key_timeout( wwd_interface_t interface, int32_t eapol_key_timeout )
{
    wiced_buffer_t buffer;
    int32_t*       data;
    uint32_t       bss_index = 0;

    /* Map the interface to a BSS index */
    bss_index = wwd_get_bss_index(interface );

    data = wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 8, "bsscfg:" IOVAR_STR_SUP_WPA_TMO );
    CHECK_IOCTL_BUFFER( data );
    data[0] = (int32_t)bss_index;
    data[1] = eapol_key_timeout;
    CHECK_RETURN( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE ) );

    return WWD_SUCCESS;
}

/******************************************************
 *             Static Functions
 ******************************************************/

/** Searches for a specific WiFi Information Element in a byte array
 *
 * Traverse a string of 1-byte tag/1-byte length/variable-length value
 * triples, returning a pointer to the substring whose first element
 * matches tag
 *
 * @note : This function has been copied directly from the standard Broadcom host driver file wl/exe/wlu.c
 *
 *
 * @param tlv_buf : The byte array containing the Information Elements (IEs)
 * @param buflen  : The length of the tlv_buf byte array
 * @param key     : The Information Element tag to search for
 *
 * @return    NULL : if no matching Information Element was found
 *            Non-Null : Pointer to the start of the matching Information Element
 */

static inline /*@null@*/ tlv8_header_t* wlu_parse_tlvs( const tlv8_header_t* tlv_buf, uint32_t buflen, dot11_ie_id_t key )
{

    return (tlv8_header_t*) tlv_find_tlv8( (const uint8_t*) tlv_buf, buflen, key );
}

/** Checks if a WiFi Information Element is a WPA entry
 *
 * Is this body of this tlvs entry a WPA entry? If
 * not update the tlvs buffer pointer/length
 *
 * @note : This function has been copied directly from the standard Broadcom host driver file wl/exe/wlu.c
 *
 * @param wpaie    : The byte array containing the Information Element (IE)
 * @param tlvs     : The larger IE array to be updated if not a WPA IE
 * @param tlvs_len : The current length of larger IE array
 *
 * @return    WICED_TRUE  : if IE matches the WPA OUI (Organizationally Unique Identifier) and its type = 1
 *            WICED_FALSE : otherwise
 */
static wiced_bool_t wlu_is_wpa_ie( vendor_specific_ie_header_t* wpaie, tlv8_header_t** tlvs, uint32_t* tlvs_len )
{
    vendor_specific_ie_header_t* ie = wpaie;

    /* If the contents match the WPA_OUI and type=1 */
    if ( ( ie->tlv_header.length >= (uint8_t) VENDOR_SPECIFIC_IE_MINIMUM_LENGTH ) &&
         ( memcmp( ie->oui, WPA_OUI_TYPE1, sizeof(ie->oui) ) == 0 ) )
    {
        /* Found the WPA IE */
        return WICED_TRUE;
    }

    /* point to the next ie */
    *tlvs = (tlv8_header_t*)( ((uint8_t*) ie) + ie->tlv_header.length + sizeof(tlv8_header_t) );

    /* calculate the length of the rest of the buffer */
    *tlvs_len -= (uint32_t) ( *tlvs - (tlv8_header_t*)ie );

    return WICED_FALSE;
}

/** Searches for a specific WiFi Information Element in a byte array
 *
 * Traverse a string of 1-byte tag/1-byte length/variable-length value
 * triples, returning a pointer to the substring whose first element
 * matches tag
 *
 * @note : This function has been copied directly from the standard Broadcom host driver file wl/exe/wlu.c
 *
 *
 * @param tlv_buf : The byte array containing the Information Elements (IEs)
 * @param buflen  : The length of the tlv_buf byte array
 * @param key     : The Information Element tag to search for
 *
 * @return    NULL : if no matching Information Element was found
 *            Non-Null : Pointer to the start of the matching Information Element
 */
static tlv8_header_t* wwd_parse_dot11_tlvs( const tlv8_header_t* tlv_buf, uint32_t buflen, dot11_ie_id_t key )
{

    return (tlv8_header_t*) tlv_find_tlv8( (const uint8_t*) tlv_buf, buflen, key );
}

/** Map channel to its band, comparing channel to max 2g channel
 *
 * @param channel     : The channel to map to a band
 *
 * @return                  : WL_CHANSPEC_BAND_2G or WL_CHANSPEC_BAND_5G
 */
inline wl_chanspec_t wwd_channel_to_wl_band( uint32_t channel )
{
    return ( ( (channel) <= CH_MAX_2G_CHANNEL ) ? WL_CHANSPEC_BAND_2G : WL_CHANSPEC_BAND_5G );
}

wwd_result_t wwd_wifi_get_wifi_version( char* buf, uint8_t length )
{
    wwd_result_t result = wwd_wifi_get_iovar_buffer( IOVAR_STR_VERSION, (uint8_t *)buf, length, WWD_STA_INTERFACE );
    uint32_t ver_len = strnlen(buf, length);

    if (ver_len > 1 && buf[ver_len - 1] == '\n')
    {
        buf[ver_len - 1] = '\0';
    }

    return result;
}

wwd_result_t wwd_wifi_get_wifi_memuse( char* buf, uint8_t length )
{
    return wwd_wifi_get_iovar_buffer( IOVAR_STR_MEMUSE, (uint8_t *)buf, length, WWD_STA_INTERFACE );
}

#if !defined(TINY_BOOTLOADER)
wwd_result_t wwd_wifi_get_clm_version( char* version, uint8_t length )
{
    wwd_result_t result;
    version[0] = '\0';
    result = wwd_wifi_get_iovar_buffer(IOVAR_STR_CLMVER, (uint8_t *)version, length, WWD_STA_INTERFACE );
    if ( result == WWD_SUCCESS && version[0] )
    {
        uint32_t version_length;
        char* p;

        version_length = (uint32_t) strnlen( version, length );
        version[version_length] = '\0';

        /* Replace all newline/linefeed characters with space character */
        p = version;
        while ( (p=strchr(p,'\n')) != NULL )
        {
            *p = ' ';
        }
    }
    return result;
}
#endif /* !TINY_BOOTLOADER */

/* GET firmware revision info */
wwd_result_t wwd_wifi_get_revision_info ( wwd_interface_t interface, wlc_rev_info_t *buf, uint16_t buflen )
{
    wwd_result_t   result;
    result = wwd_wifi_get_ioctl_buffer( WLC_GET_REVINFO, (uint8_t*)buf, buflen, interface );
    if ( result != WWD_SUCCESS )
    {
          WPRINT_WWD_DEBUG(("wwd_wifi_get_revision_info failed..result:%d\n", result ));
          return result;
    }

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_enable_minimum_power_consumption( void )
{
    return wwd_wifi_set_iovar_value( IOVAR_STR_MPC, 1, WWD_STA_INTERFACE );
}

wwd_result_t wwd_wifi_disable_minimum_power_consumption( void )
{
    return wwd_wifi_set_iovar_value( IOVAR_STR_MPC, 0, WWD_STA_INTERFACE );
}

static wwd_result_t wwd_wifi_check_join_status( wwd_interface_t iface )
{

    int interface = iface & 0x3;
    if (wwd_wifi_is_mesh_enabled() && (interface == WWD_STA_INTERFACE)) {
        return WWD_SUCCESS;
    }

     if ((uint32_t)interface >= WWD_INTERFACE_MAX) {
        WPRINT_APP_INFO(("%s: Bad interface 2\n", __FUNCTION__));
        return WWD_INVALID_JOIN_STATUS;
     }
    switch ( wiced_join_status[ interface ] )
    {
        case JOIN_NO_NETWORKS :
            return WWD_NETWORK_NOT_FOUND;

        case JOIN_AUTHENTICATED | JOIN_LINK_READY | JOIN_EAPOL_KEY_M1_TIMEOUT :
            return WWD_EAPOL_KEY_PACKET_M1_TIMEOUT;

        case JOIN_AUTHENTICATED | JOIN_LINK_READY | JOIN_EAPOL_KEY_M3_TIMEOUT :
        case JOIN_AUTHENTICATED | JOIN_LINK_READY | JOIN_SSID_SET | JOIN_EAPOL_KEY_M3_TIMEOUT :
            return WWD_EAPOL_KEY_PACKET_M3_TIMEOUT;

        case JOIN_AUTHENTICATED | JOIN_LINK_READY | JOIN_EAPOL_KEY_G1_TIMEOUT :
        case JOIN_AUTHENTICATED | JOIN_LINK_READY | JOIN_SSID_SET | JOIN_EAPOL_KEY_G1_TIMEOUT :
            return WWD_EAPOL_KEY_PACKET_G1_TIMEOUT;

        case JOIN_AUTHENTICATED | JOIN_LINK_READY | JOIN_EAPOL_KEY_FAILURE :
        case JOIN_AUTHENTICATED | JOIN_LINK_READY | JOIN_SSID_SET | JOIN_EAPOL_KEY_FAILURE :
            return WWD_EAPOL_KEY_FAILURE;

        case JOIN_AUTHENTICATED | JOIN_LINK_READY | JOIN_SSID_SET | JOIN_SECURITY_COMPLETE :
            return WWD_SUCCESS;

        case 0:
        case JOIN_SECURITY_COMPLETE : /* For open/WEP */
            return WWD_NOT_AUTHENTICATED;

        case JOIN_AUTHENTICATED | JOIN_LINK_READY | JOIN_SECURITY_COMPLETE :
            return WWD_JOIN_IN_PROGRESS;

        case JOIN_AUTHENTICATED | JOIN_LINK_READY :
        case JOIN_AUTHENTICATED | JOIN_LINK_READY | JOIN_SSID_SET :
            return WWD_NOT_KEYED;

        default:
            return WWD_INVALID_JOIN_STATUS;
    }
}

wiced_bool_t wwd_wifi_sta_is_only_connected( void )
{
    /* Rely on WWD_INTERFACE_MAX to enumerate interfaces */
    for ( int interface_index = 0 ; interface_index < WWD_INTERFACE_MAX ; interface_index++ )
    {
        wiced_bool_t ready = ( WWD_SUCCESS == wwd_wifi_is_ready_to_transceive( (wwd_interface_t)interface_index ) ) ? WICED_TRUE : WICED_FALSE;
        /* only FALSE if something else besides STA is up and ready for IO or STA not ready */
        if ( ( ( WWD_STA_INTERFACE != (wwd_interface_t)interface_index ) && WICED_TRUE == ready )
             ||
            ( ( WWD_STA_INTERFACE == (wwd_interface_t)interface_index ) && WICED_FALSE == ready ) )
        {
            return WICED_FALSE;
        }
    }
    return WICED_TRUE;
}

static wwd_result_t wwd_wifi_otp_write_bits( uint32_t bit_offset, uint32_t bit_length, uint8_t* bit_stream )
{
    wwd_result_t result;
    wiced_buffer_t request;
    uint16_t request_size;
    uint8_t* req;

    request_size = (uint16_t)(sizeof(bit_offset) + sizeof(bit_length) + ((ROUND_UP(bit_length, 32)) / 8));

    req = (uint8_t*)wwd_sdpcm_get_iovar_buffer( &request, request_size, IOVAR_STR_OTPRAW );
    if ( req == NULL )
    {
        WPRINT_WWD_DEBUG(( "IOVAR buffer allocation failed\n" ));
        return WWD_BUFFER_ALLOC_FAIL;
    }
    memset( req, 0, request_size );

    *(uint32_t*)req = htod32(bit_offset);
    req += 4;
    *(uint32_t*)req = htod32(bit_length);
    req += 4;
    memcpy( req, bit_stream, (DIV_ROUND_UP(bit_length, 8)) );

    /* Writes the specified bit stream into the OTP at the specified bit offset */
    result = wwd_sdpcm_send_iovar( SDPCM_SET, request, NULL, WWD_STA_INTERFACE );

    if (result != WWD_SUCCESS)
    {
        if ( result == WWD_WLAN_UNSUPPORTED )
        {
            WPRINT_WWD_DEBUG(( "WLAN IOVAR not supported: %d\n", result ));
            return WWD_WLAN_UNSUPPORTED;
        }
        else
        {
            WPRINT_WWD_DEBUG(( "WLAN IOVAR unknown error: %d\n", result ));
            return WWD_WLAN_ERROR;
        }
    }

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_otp_write_bit( uint16_t bit_offset, uint16_t write_bit )
{
    uint32_t offset = (uint32_t)(bit_offset);
    uint32_t length = 1;
    uint8_t stream = (uint8_t)(write_bit & 0x1);
    return wwd_wifi_otp_write_bits( offset, length, &stream );
}

wwd_result_t wwd_wifi_otp_write_word( uint16_t word_offset, uint16_t write_word )
{
    uint32_t offset = (uint32_t)(word_offset * OTP_WORD_SIZE);
    uint32_t length = OTP_WORD_SIZE;
    uint8_t stream[2] = {(uint8_t)(write_word & 0xFF), (uint8_t)((write_word >> 8) & 0xFF)};
    return wwd_wifi_otp_write_bits( offset, length, stream );
}

wwd_result_t wwd_reset_statistics_counters( void )
{
   uint32_t value;
   return wwd_wifi_get_iovar_value( IOVAR_STR_RESET_CNTS, &value, WWD_STA_INTERFACE );
}

wwd_result_t wwd_phyrate_log( unsigned int mode)
{
    return wwd_wifi_set_iovar_value( IOVAR_STR_PHYRATE_LOG, mode, WWD_STA_INTERFACE );
}

wwd_result_t wwd_get_phyrate_statistics_counters( wiced_phyrate_counters_t *counts_buffer, unsigned int size)
{
    wwd_interface_t interface = WWD_STA_INTERFACE;
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    wiced_counters_t* data;
    wwd_result_t result;

    UNUSED_PARAMETER(counts_buffer);
    if ( size < WICED_WIFI_PHYRATE_COUNT * sizeof(uint32_t) )
    {
        return WWD_BADARG;
    }

    wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(*data), IOVAR_STR_COUNTERS );

    result = wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, interface );

    if (result == WWD_SUCCESS)
    {
        data = (wiced_counters_t*)host_buffer_get_current_piece_data_pointer( response );
#ifndef WLCMD_VER_01
        memcpy( counts_buffer, &data->rx1mbps, WICED_WIFI_PHYRATE_COUNT * sizeof(uint32_t) );
#endif

        host_buffer_release( response, WWD_NETWORK_RX );
    }

    return result;
}

wwd_result_t wwd_get_phyrate_log_size( unsigned int *size )
{
    return wwd_wifi_get_iovar_value( IOVAR_STR_PHYRATE_LOG_SIZE, (uint32_t *)size, WWD_STA_INTERFACE );
}

wwd_result_t wwd_get_phyrate_log( wiced_phyrate_log_t *data )
{
    return wwd_wifi_get_iovar_buffer( IOVAR_STR_PHYRATE_LOG_DUMP, (uint8_t*)data, sizeof( *data ), WWD_STA_INTERFACE );
}

wwd_result_t wwd_get_counters( wiced_counters_t *data )
{
    return wwd_wifi_get_iovar_buffer( IOVAR_STR_COUNTERS, (uint8_t*)data, sizeof(*data), WWD_STA_INTERFACE );
}

wwd_result_t wwd_get_txrx_frag( uint32_t *txfrag, uint32_t *rxfrag )
{
    wiced_counters_t data;

    if ( wwd_wifi_get_iovar_buffer( IOVAR_STR_COUNTERS, (uint8_t*)&data, sizeof( data ), WWD_STA_INTERFACE ) != WWD_SUCCESS)
        return WWD_WLAN_ERROR;

#ifndef WLCMD_VER_01
/* 43012 based platforms do not have these members in wiced_counters_t so protecting with flag */
    *txfrag = data.txfrag;
    *rxfrag = data.rxfrag;
#else
    *txfrag = 0;
    *rxfrag = 0;
#endif

    return WWD_SUCCESS;
}

/** Handles Radio Resource Management events
 *
 *  This function receives Radio Resource Management events and calls the registered callback
 *  of user application.
 *
 * @param[in]  wwd_rrm_report_type_t     :  Radio Resource report type
 * @param[out] wwd_event_handler_t       : callback function to be call event is received
 * @param[in]  void*                     : pointer to Radio Resource management request
 * @param[out] wwd_rrm_report_t          : Radio Resource report pointer
 * @param[in]  void*                     : semaphore
 * @param[in]  wwd_interface_t           : interface type
 *
 */
wwd_result_t wwd_wifi_rrm_request(   wwd_rrm_report_type_t                    report_type,
                                     wwd_event_handler_t                      callback,
                                     void*                                    rrm_req,
                                     wwd_rrm_report_t**                       report_ptr,
                                     void*                                    user_data,
                                     wwd_interface_t                          interface
                                   )

{
    wwd_result_t err = WWD_SUCCESS;
    radio_resource_management_req_t *rrm_chload_req;
    radio_resource_management_beacon_req_t *bcnreq;
    wiced_ssid_t *wiced;
    wiced_mac_t *mac;


    UNUSED_PARAMETER(report_ptr);
    UNUSED_PARAMETER(user_data);
    wwd_management_set_event_handler( rrm_events, callback, user_data, interface );

    switch (report_type)
    {
         case WICED_RRM_CHLOAD_REPORT:
         {
                 rrm_chload_req = (radio_resource_management_req_t *)rrm_req;
                 err = wwd_wifi_radio_resource_management_channel_load_req( interface, rrm_chload_req );
                 break;
         }

         case WICED_RRM_NBR_LIST_REPORT:
         {
                 wiced = (wiced_ssid_t *)rrm_req;
                 err = wwd_wifi_radio_resource_management_neighbor_req( interface, wiced);
                 break;
         }

         case WICED_RRM_BCN_REPORT:
         {
                 bcnreq = (radio_resource_management_beacon_req_t *)rrm_req;
                 err = wwd_wifi_radio_resource_management_beacon_req(interface, bcnreq);
                 break;
         }

         case WICED_LM_REPORT:
         {
             mac = (wiced_mac_t *)rrm_req;
             wwd_wifi_radio_resource_management_link_management_req(WWD_STA_INTERFACE, mac);
             break;
         }
         default:
           break;
    }
    return err;
}

static wwd_result_t wwd_wiced_security_to_pfn_auth_wsec( wiced_security_t security, wl_pfn_t *pfn_net )
{
    wwd_result_t result = WWD_SUCCESS;
    uint8_t table_index = 0;

    wwd_security_to_auth_wsec_map_t wiced_security2pfn_auth_wsec[] =
    {
        { WICED_SECURITY_OPEN, AUTH_ALGO_80211_OPEN, WPA_AUTH_DISABLED, 0 },
        { WICED_SECURITY_WEP_PSK, AUTH_ALGO_80211_OPEN, WPA_AUTH_DISABLED, WEP_ENABLED },
        { WICED_SECURITY_WEP_SHARED, AUTH_ALGO_80211_SHARED_KEY, WPA_AUTH_DISABLED, WEP_ENABLED},
        { WICED_SECURITY_WPA2_TKIP_PSK, AUTH_ALGO_RSNA_PSK, WPA2_AUTH_PSK, TKIP_ENABLED },
        { WICED_SECURITY_WPA2_AES_PSK, AUTH_ALGO_RSNA_PSK, WPA2_AUTH_PSK, AES_ENABLED },
        { WICED_SECURITY_WPA_TKIP_PSK, AUTH_ALGO_WPA_PSK, WPA_AUTH_PSK, TKIP_ENABLED },
        { WICED_SECURITY_WPA_AES_PSK, AUTH_ALGO_WPA_PSK, WPA_AUTH_PSK, AES_ENABLED },

        { WICED_SECURITY_WPA_TKIP_ENT, AUTH_ALGO_WPA, WPA_AUTH_UNSPECIFIED, TKIP_ENABLED },
        { WICED_SECURITY_WPA_AES_ENT, AUTH_ALGO_WPA, WPA_AUTH_UNSPECIFIED, AES_ENABLED },
        { WICED_SECURITY_WPA2_TKIP_ENT, AUTH_ALGO_RSNA, WPA2_AUTH_UNSPECIFIED, TKIP_ENABLED },
        { WICED_SECURITY_WPA2_AES_ENT, AUTH_ALGO_RSNA, WPA2_AUTH_UNSPECIFIED, AES_ENABLED },

        { WICED_SECURITY_WPA_MIXED_PSK, AUTH_ALGO_WPA_PSK, WPA_AUTH_PSK, ( TKIP_ENABLED | AES_ENABLED ) },
        { WICED_SECURITY_WPA2_MIXED_PSK, AUTH_ALGO_RSNA_PSK, WPA2_AUTH_PSK, ( TKIP_ENABLED | AES_ENABLED ) },
        { WICED_SECURITY_WPA_MIXED_ENT, AUTH_ALGO_WPA_PSK, WPA_AUTH_PSK, ( TKIP_ENABLED | AES_ENABLED ) },
        { WICED_SECURITY_WPA2_MIXED_ENT, AUTH_ALGO_RSNA_PSK, WPA2_AUTH_PSK, ( TKIP_ENABLED | AES_ENABLED ) }
    };

    for ( table_index = 0; table_index < sizeof( wiced_security2pfn_auth_wsec )/sizeof( wiced_security2pfn_auth_wsec[0] ); table_index++ )
    {
        if ( security == wiced_security2pfn_auth_wsec[table_index].security )
        {
            pfn_net->auth     = wiced_security2pfn_auth_wsec[table_index].auth_algo;
            pfn_net->wpa_auth = wiced_security2pfn_auth_wsec[table_index].wpa_auth;
            pfn_net->wsec     = wiced_security2pfn_auth_wsec[table_index].wsec;
            break;
        }
    }

    /* unsupported security type */
    if ( table_index == sizeof( wiced_security2pfn_auth_wsec )/sizeof( wiced_security2pfn_auth_wsec[0] ) )
    {
        result = WWD_BADARG;
    }

    return result;
}

static wwd_result_t wwd_pno_add_ssid_network( wiced_ssid_t *ssid, wiced_security_t security )
{
    wl_pfn_t pfn_net;
    uint8_t ssid_len = MIN( ssid->length, sizeof( pfn_net.ssid.SSID ) );

    /* add network */
    memset( &pfn_net, 0, sizeof( pfn_net ) );
    memcpy( pfn_net.ssid.SSID, ssid->value, ssid_len );

    pfn_net.ssid.SSID_len = ssid_len;
    CHECK_RETURN( wwd_wiced_security_to_pfn_auth_wsec( security, &pfn_net ) );

    /* Note: BSS is searched for as infra is set to 0 */

    /* do not report loss of network as a result -- we only want to know if and when it appears */
    pfn_net.flags |= WL_PFN_SUPPRESSLOST_MASK;

    /* call driver */
    CHECK_RETURN( wwd_wifi_set_iovar_buffer( IOVAR_STR_PNO_ADD, &pfn_net, sizeof( pfn_net ), WWD_STA_INTERFACE ) );
    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_pno_add_network( wiced_ssid_t *ssid, wiced_security_t security )
{
    /* must be stopped before adding */
    CHECK_RETURN( wwd_wifi_pno_stop( ) );

    CHECK_RETURN( wwd_pno_add_ssid_network( ssid, security ) );

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_pno_start( void )
{
    wl_pfn_param_t pfn_param;

    /* configure pfn parameters: must be called prior to starting */
    memset( &pfn_param, 0, sizeof(pfn_param) );

    pfn_param.version   = PFN_VERSION;

    /* in seconds: how frequent does this scan get launched */
    pfn_param.scan_freq = WICED_WIFI_PNO_SCAN_PERIOD;

    /* report event immediately */
    pfn_param.flags |= 1 << IMMEDIATE_EVENT_BIT;

    /* background scanning is fine */
    pfn_param.flags |= 1 << ENABLE_BKGRD_SCAN_BIT;

    /* kick off scan as soon as possible */
    pfn_param.flags |= 1 << IMMEDIATE_SCAN_BIT;

    /* hide ssid info in pfn scan */
    pfn_param.flags |= 1 << SUPPRESS_SSID_BIT;

    /* enable for nlo offload */
    pfn_param.flags |= 1 << ENABLE_NET_OFFLOAD_BIT;

    /* set pfn parameters */
    CHECK_RETURN( wwd_wifi_set_iovar_buffer( IOVAR_STR_PNO_SET, &pfn_param, sizeof( pfn_param ), WWD_STA_INTERFACE ) );

    /* re-enable pfn */
    CHECK_RETURN( wwd_wifi_pno_set_enable( WICED_TRUE ) );

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_pno_stop( void )
{
    CHECK_RETURN( wwd_wifi_pno_set_enable( WICED_FALSE ) );

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_pno_clear( void )
{
    /* must be stopped before clearing */
    CHECK_RETURN( wwd_wifi_pno_stop( ) );

    /* clear all added networks */
    CHECK_RETURN( wwd_wifi_set_iovar_void( IOVAR_STR_PNO_CLEAR, WWD_STA_INTERFACE ) );

    return WWD_SUCCESS;
}

static wwd_result_t wwd_wifi_pno_set_enable( wiced_bool_t enable )
{
    if ( enable == WICED_FALSE )
    {
        /* disable pfn */
        CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_PNO_ON, 0, WWD_STA_INTERFACE ) );
    }
    else
    {
        /* enable pfn */
        CHECK_RETURN( wwd_wifi_set_iovar_value( IOVAR_STR_PNO_ON, 1, WWD_STA_INTERFACE ) );
    }
    return WWD_SUCCESS;
}

/* Enable NAN (neighborhood awarness networking) */
wwd_result_t wwd_nan_config_enable( void )
{
    CHECK_RETURN (wwd_nan_config (WICED_TRUE) );

    return WWD_SUCCESS;
}

static wwd_result_t wwd_nan_config (wiced_bool_t enable)
{
    bcm_iov_batch_buf_t* xtlv_iov_buf;
    uint8_t* piov_buffer;
    bcm_iov_batch_subcmd_t* sub_cmd;
    wiced_buffer_t buffer = NULL;
    uint8_t iovar_data_len = sizeof(uint8_t);

    /* enable NAN  - 1
     * disable NAN - 0
     */
    uint8_t nan_enable = (uint8_t)enable;

    /* get wwd_nan_config_enable buffer */
    wwd_xtlv_batch_cmd_buffer( &buffer, &xtlv_iov_buf, &piov_buffer, IOVAR_NAN );

    sub_cmd = (bcm_iov_batch_subcmd_t* )piov_buffer;
    sub_cmd->u.options = BCM_XTLV_OPTION_ALIGN32;

    memcpy( &sub_cmd->data[0] , &nan_enable, sizeof(iovar_data_len) );
    sub_cmd->id = WL_NAN_CMD_CFG_ENABLE;

    wwd_pack_xtlv(&piov_buffer, sub_cmd->id, iovar_data_len);

    CHECK_RETURN ( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, WWD_STA_INTERFACE ));

    return WWD_SUCCESS;
}

wwd_result_t wwd_nan_config_disable ( void )
{
    CHECK_RETURN ( wwd_nan_config (WICED_FALSE) );

   return WWD_SUCCESS;
}

/* Build XTLV buffer for XTLV batch commands */
wwd_result_t wwd_xtlv_batch_cmd_buffer (wiced_buffer_t* buffer, bcm_iov_batch_buf_t** xtlv_iov_buf, uint8_t **piov_buf, const char* iovar )
{
    bcm_iov_batch_buf_t *batch_buf;
    wiced_buffer_t buf;
    int iov_len;

    iov_len = (int)(WLC_IOCTL_SMLEN - strlen(iovar) - 1);
    *xtlv_iov_buf = (bcm_iov_batch_buf_t* ) wwd_sdpcm_get_iovar_buffer(&buf, (uint16_t)iov_len, iovar);
    *buffer = buf;

    batch_buf = *xtlv_iov_buf;
    CHECK_IOCTL_BUFFER( batch_buf );

    batch_buf->version = htol16(BCM_IOV_BATCH_MASK);
    batch_buf->count = 1;
    *piov_buf = (uint8_t *)(&batch_buf->cmds[0]);
    CHECK_IOCTL_BUFFER( piov_buf );

    return WWD_SUCCESS;
}

/* Pack CMD ID and Length into the XTLV buffer */
wwd_result_t wwd_pack_xtlv ( uint8_t** buffer, uint16_t cmd_id, uint16_t iovar_data_len )
{
    /* length = iovar len + '\0' + iovar_data_len */
    uint16_t len = (uint16_t)(strlen(IOVAR_NAN) + 1 + iovar_data_len);
    uint8_t* p_buf = (uint8_t *)*buffer;
    uint8_t* p_buf_len = (uint8_t*)((uint8_t* )p_buf + sizeof(cmd_id));

    wiced_htol16_ua_store(cmd_id, p_buf);
    wiced_htol16_ua_store(len, p_buf_len);

    return WWD_SUCCESS;
}

wwd_result_t wwd_nan_config_device_state( wiced_bool_t set, uint8_t* role )
{
    return ( set == WICED_TRUE ) ? wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_CFG_STATE, role, sizeof(*role)) :
            wwd_xtlv_get_set_data(WICED_FALSE, WL_NAN_CMD_CFG_STATE, role, sizeof(*role));
}

wwd_result_t wwd_nan_config_hop_count( wiced_bool_t set, uint8_t* hop_count )
{
    return ( set == WICED_TRUE ) ? wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_CFG_HOP_CNT, hop_count, sizeof(*hop_count)) :
            wwd_xtlv_get_set_data(WICED_FALSE, WL_NAN_CMD_CFG_HOP_CNT, hop_count, sizeof(*hop_count));
}

wwd_result_t wwd_nan_config_hop_limit ( wiced_bool_t set, uint8_t* hop_limit )
{
    return ( set == WICED_TRUE ) ? wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_CFG_HOP_LIMIT, hop_limit, sizeof(*hop_limit)) :
            wwd_xtlv_get_set_data(WICED_FALSE, WL_NAN_CMD_CFG_HOP_LIMIT, hop_limit, sizeof(*hop_limit));
}


wwd_result_t wwd_nan_config_warmup_time ( wiced_bool_t set, uint32_t *warmup_time )
{
    return ( set == WICED_TRUE ) ? wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_CFG_WARMUP_TIME, warmup_time, sizeof(*warmup_time)) :
            wwd_xtlv_get_set_data(WICED_FALSE, WL_NAN_CMD_CFG_WARMUP_TIME, warmup_time, sizeof(*warmup_time));
}


wwd_result_t wwd_nan_config_rssi_threshold ( wiced_bool_t set, wwd_nan_config_rssi_threshold_t* rssi_thresh )
{
    return ( set == WICED_TRUE ) ? wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_CFG_RSSI_THRESHOLD, rssi_thresh, sizeof(*rssi_thresh)) :
            wwd_xtlv_get_set_data(WICED_FALSE, WL_NAN_CMD_CFG_RSSI_THRESHOLD, rssi_thresh, sizeof(*rssi_thresh));
}

wwd_result_t wwd_nan_config_get_status ( wwd_nan_state_t* nan_cfg_status )
{
    return  wwd_xtlv_get_set_data (WICED_FALSE, WL_NAN_CMD_CFG_STATUS, nan_cfg_status, sizeof(*nan_cfg_status));
}

wwd_result_t wwd_nan_config_oui ( wiced_bool_t set, wwd_nan_config_oui_type_t* oui_type )
{
    return ( set == WICED_TRUE ) ? wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_CFG_OUI, oui_type, sizeof(*oui_type)) :
              wwd_xtlv_get_set_data(WICED_FALSE, WL_NAN_CMD_CFG_OUI, oui_type, sizeof(*oui_type));
}

wwd_result_t wwd_nan_config_get_count ( wwd_nan_config_count_t* config_count )
{
    return wwd_xtlv_get_set_data (WICED_FALSE, WL_NAN_CMD_CFG_COUNT, config_count, sizeof(*config_count));
}

wwd_result_t wwd_nan_config_clear_counters ( void )
{
    return wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_CFG_CLEARCOUNT, NULL, 0);;
}

wwd_result_t wwd_nan_config_set_chanspec ( chanspec_t* chanspec )
{
    return wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_CFG_CHANNEL, chanspec, sizeof(*chanspec));
}

wwd_result_t wwd_nan_config_band ( uint8_t nan_band )
{
    return  wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_CFG_BAND, &nan_band, sizeof(nan_band) );
}

wwd_result_t wwd_nan_config_cluster_id ( wiced_bool_t set, wwd_nan_cluster_id_t* ether_addr )
{
    return ( set == WICED_TRUE ) ? wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_CFG_CID, ether_addr, sizeof(*ether_addr)) :
            wwd_xtlv_get_set_data(WICED_FALSE, WL_NAN_CMD_CFG_CID, ether_addr, sizeof(*ether_addr));
}

wwd_result_t wwd_nan_config_interface_address ( wiced_bool_t set , struct ether_addr* ether_addr )
{
    return ( set == WICED_TRUE ) ? wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_CFG_IF_ADDR, ether_addr, sizeof(*ether_addr)) :
               wwd_xtlv_get_set_data(WICED_FALSE, WL_NAN_CMD_CFG_IF_ADDR, ether_addr, sizeof(*ether_addr));
}

wwd_result_t wwd_nan_config_discovery_beacon_interval ( wiced_bool_t set, uint16_t* nan_discover_bcn_interval )
{
    return ( set == WICED_TRUE ) ?
            wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_CFG_BCN_INTERVAL, nan_discover_bcn_interval, sizeof(*nan_discover_bcn_interval)) :
            wwd_xtlv_get_set_data(WICED_FALSE, WL_NAN_CMD_CFG_BCN_INTERVAL, nan_discover_bcn_interval, sizeof(*nan_discover_bcn_interval));
}


wwd_result_t wwd_nan_config_service_discovery_frame_tx_time ( wiced_bool_t set, uint16_t* disc_txtime )
{
    return ( set == WICED_TRUE ) ?
                wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_CFG_SDF_TXTIME, disc_txtime, sizeof(*disc_txtime)) :
                wwd_xtlv_get_set_data(WICED_FALSE, WL_NAN_CMD_CFG_SDF_TXTIME, disc_txtime, sizeof(*disc_txtime));
}

wwd_result_t wwd_nan_config_stop_beacon_transmit ( wiced_bool_t set, uint16_t* stop_beacon )
{
    return ( set == WICED_TRUE ) ?
            wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_CFG_STOP_BCN_TX, stop_beacon, sizeof(*stop_beacon)) :
            wwd_xtlv_get_set_data(WICED_FALSE, WL_NAN_CMD_CFG_STOP_BCN_TX, stop_beacon, sizeof(*stop_beacon));
}

wwd_result_t wwd_nan_config_service_id_beacon ( wiced_bool_t set, wwd_nan_sid_beacon_control_t* service_id_bcn_control )
{
    return ( set == WICED_TRUE ) ?
            wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_CFG_SID_BEACON, service_id_bcn_control, sizeof(*service_id_bcn_control)) :
            wwd_xtlv_get_set_data(WICED_FALSE, WL_NAN_CMD_CFG_SID_BEACON, service_id_bcn_control, sizeof(*service_id_bcn_control));
}

wwd_result_t wwd_nan_config_discover_window_length ( wiced_bool_t set, uint16_t* dw_len )
{
    return ( set == WICED_TRUE ) ?
            wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_CFG_DW_LEN, dw_len, sizeof(*dw_len)) :
            wwd_xtlv_get_set_data(WICED_FALSE, WL_NAN_CMD_CFG_DW_LEN, dw_len, sizeof(*dw_len));
}

wwd_result_t wwd_nan_election_host_enable ( wiced_bool_t set, uint8_t* host_enable )
{
    return ( set == WICED_TRUE ) ?
            wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_ELECTION_HOST_ENABLE, host_enable, sizeof(*host_enable)) :
            wwd_xtlv_get_set_data(WICED_FALSE, WL_NAN_CMD_ELECTION_HOST_ENABLE, host_enable, sizeof(*host_enable));
}

wwd_result_t wwd_nan_election_metric_config ( wwd_nan_election_metric_config_t* config )
{
     return wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_ELECTION_METRICS_CONFIG, config, sizeof(*config));
}

wwd_result_t wwd_nan_election_metric_state_get ( wwd_nan_election_metric_config_t* config )
{
    return  wwd_xtlv_get_set_data ( WICED_FALSE, WL_NAN_CMD_ELECTION_METRICS_STATE, config, sizeof(*config));
}

wwd_result_t wwd_nan_election_join ( wwd_nan_join_t* join )
{
    return wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_ELECTION_JOIN, join, sizeof(*join));
}

wwd_result_t wwd_nan_election_merge ( wiced_bool_t set, uint8_t* enable_merge)
{
    return wwd_xtlv_get_set_data ( set, WL_NAN_CMD_ELECTION_MERGE, enable_merge, sizeof(*enable_merge));
}

wwd_result_t wwd_nan_election_stop ( wwd_nan_cluster_id_t* cluster_id )
{
    return wwd_xtlv_get_set_data ( WICED_TRUE, WL_NAN_CMD_ELECTION_STOP, cluster_id, sizeof(*cluster_id));
}

wwd_result_t wwd_nan_sync_timeslot_reserve ( wwd_nan_timeslot_t* reserve )
{
   return wwd_xtlv_get_set_data ( WICED_TRUE, WL_NAN_CMD_SYNC_TSRESERVE, reserve, sizeof(*reserve));
}

wwd_result_t wwd_nan_sync_timeslot_release ( uint32_t* release )
{
    return wwd_xtlv_get_set_data ( WICED_TRUE, WL_NAN_CMD_SYNC_TSRELEASE, release, sizeof(*release));
}

wwd_result_t wwd_nan_sd_publish ( wiced_bool_t set, wwd_nan_sd_publish_t* nan_sd_publish )
{
    return ( set == WICED_TRUE ) ?
                wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_SD_PUBLISH, nan_sd_publish, sizeof(*nan_sd_publish)) :
                wwd_xtlv_get_set_data(WICED_FALSE, WL_NAN_CMD_SD_PUBLISH, nan_sd_publish, sizeof(*nan_sd_publish));
}

wwd_result_t wwd_nan_sd_publish_list ( wwd_nan_service_list_t* nan_service_list )
{
    return wwd_xtlv_get_set_data ( WICED_FALSE, WL_NAN_CMD_SD_PUBLISH_LIST, nan_service_list, sizeof(*nan_service_list));
}

wwd_result_t wwd_nan_sd_cancel_publish ( uint8_t instance_id )
{
    return wwd_xtlv_get_set_data ( WICED_TRUE, WL_NAN_CMD_SD_CANCEL_PUBLISH, &instance_id, sizeof(instance_id));
}

wwd_result_t wwd_nan_sd_subscribe ( wiced_bool_t set, wwd_nan_sd_subscribe_t* nan_sd_subscribe )
{
    return ( set == WICED_TRUE ) ?
                    wwd_xtlv_get_set_data(WICED_TRUE, WL_NAN_CMD_SD_SUBSCRIBE, nan_sd_subscribe, sizeof(*nan_sd_subscribe)) :
                    wwd_xtlv_get_set_data(WICED_FALSE, WL_NAN_CMD_SD_SUBSCRIBE, nan_sd_subscribe, sizeof(*nan_sd_subscribe));

}

wwd_result_t wwd_nan_sd_subscribe_list ( wwd_nan_service_list_t* nan_service_list )
{
    return wwd_xtlv_get_set_data ( WICED_FALSE, WL_NAN_CMD_SD_SUBSCRIBE_LIST, nan_service_list, sizeof(*nan_service_list));
}

wwd_result_t wwd_nan_sd_cancel_subscribe ( uint8_t instance_id )
{
    return wwd_xtlv_get_set_data ( WICED_TRUE, WL_NAN_CMD_SD_CANCEL_SUBSCRIBE, &instance_id, sizeof(instance_id));
}

wwd_result_t wwd_nan_sd_transmit ( wwd_nan_sd_transmit_t* nan_sd_transmit )
{
   return wwd_xtlv_get_set_data ( WICED_TRUE, WL_NAN_CMD_SD_TRANSMIT, nan_sd_transmit, sizeof(*nan_sd_transmit));
}

wwd_result_t wwd_xtlv_get_set_data( wiced_bool_t set, uint16_t cmd_id, void* data, uint16_t iovar_data_len )
{
    bcm_iov_batch_buf_t* xtlv_iov_buf;
    uint8_t* piov_buffer;
    bcm_iov_batch_subcmd_t* sub_cmd, *resp_cmd = NULL;
    wiced_buffer_t buffer = NULL;
    wiced_buffer_t response;

    wwd_result_t result = WWD_SUCCESS;


    /* get wwd_xtlv_get_set_data buffer */
    result =  wwd_xtlv_batch_cmd_buffer( &buffer, &xtlv_iov_buf, &piov_buffer, IOVAR_NAN );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    sub_cmd = (bcm_iov_batch_subcmd_t* )piov_buffer;
    sub_cmd->u.options = BCM_XTLV_OPTION_ALIGN32;
    sub_cmd->id = cmd_id;

    if (set)
    {
        memcpy( &sub_cmd->data[0] , data, iovar_data_len );
        wwd_pack_xtlv(&piov_buffer, sub_cmd->id, iovar_data_len);
        CHECK_RETURN ( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, WWD_STA_INTERFACE ));
    }
    else
    {
        wwd_pack_xtlv(&piov_buffer, sub_cmd->id, iovar_data_len);
        CHECK_RETURN ( wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, WWD_STA_INTERFACE ));

        wwd_unpack_xtlv(&response, &resp_cmd, iovar_data_len);
        WPRINT_WWD_DEBUG (("resp->id:%d resp->len:%d resp->status:%lu\n", resp_cmd->id, resp_cmd->len, resp_cmd->u.status ));

        if (result == WWD_SUCCESS)
        {
             memcpy(data, resp_cmd->data, iovar_data_len);
             host_buffer_release(response, WWD_NETWORK_RX);
        }
     }

    return result;
}

wwd_result_t wwd_unpack_xtlv( wiced_buffer_t* buffer, bcm_iov_batch_subcmd_t** response, uint16_t iovar_data_len )
{

    bcm_iov_batch_buf_t* batch_buf;
    bcm_iov_batch_subcmd_t* xtlv;
    uint16_t data_len;

    batch_buf = (bcm_iov_batch_buf_t* )host_buffer_get_current_piece_data_pointer( *buffer ) ;
    xtlv = (bcm_iov_batch_subcmd_t*)((uint8_t*)batch_buf + offsetof(bcm_iov_batch_buf_t, cmds));
    print_hex_bytes ((uint8_t *)batch_buf, sizeof(*batch_buf));

    data_len = (uint16_t)(xtlv->len - offsetof(wwd_xtlv_t, data));

    if ( data_len > iovar_data_len )
    {
        WPRINT_WWD_DEBUG (("Length too large data_len:%d iovar_data_len:%d\n", data_len, iovar_data_len));
        return WWD_WLAN_BADLEN;
    }

    WPRINT_WWD_DEBUG (("xtlv->id:%d xtlv->len:%d data_len:%d iovar_data_len:%d status:%lu\n",
            xtlv->id, xtlv->len, data_len, iovar_data_len, xtlv->u.status));

    print_hex_bytes((uint8_t*)xtlv->data, data_len);
    *response = xtlv;

    return WWD_SUCCESS;
}





wwd_result_t print_hex_bytes ( uint8_t* bytes, uint16_t length )
{
    int i;

    UNUSED_PARAMETER(bytes);
    UNUSED_PARAMETER(length);

    WPRINT_WWD_DEBUG (("**bytes\n"));
    for ( i = 0; i < length; i++ )
    {
        WPRINT_WWD_DEBUG (("%02x ", bytes[i]));

        if ( ( (i % 16)  == 0 ) && ( i != 0 ) )
        {
            WPRINT_WWD_DEBUG (("\n"));
        }
    }
    WPRINT_WWD_DEBUG (("**\nbytes\n"));

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_set_flags( uint32_t *wifi_flags, wwd_interface_t interface )
{
    (void)interface;
    wwd_wifi_mesh_flags = *wifi_flags;
    wwd_wifi_link_update( );

    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_get_flags( uint32_t *wifi_flags, wwd_interface_t interface )
{
    (void)interface;
    *wifi_flags = wwd_wifi_mesh_flags;

    return WWD_SUCCESS;
}

wiced_bool_t wwd_wifi_is_mesh_enabled( void )
{
    if ( wwd_wifi_mesh_flags & WIFI_FLAG_MESH )
    {
        return WICED_TRUE;
    }

    return WICED_FALSE;
}

wiced_bool_t wwd_wifi_is_mesh_mcast_rebroadcast_enabled( void )
{
    if ( wwd_wifi_mesh_flags & WIFI_FLAG_MESH_MCAST_REBROADCAST )
    {
        return WICED_TRUE;
    }

    return WICED_FALSE;
}

wwd_result_t wwd_set_mesh_channel( uint32_t channel, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    uint32_t*      data;
    wwd_result_t   result;

    data = wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 4, "chanspec" );
    CHECK_IOCTL_BUFFER( data );

    if (channel >= 1 && channel <= 11)
        *data = 0x1000 | (channel & 0xFF);
    else if (channel >= 36 && channel <= 161)
        *data = 0xd000 | (channel & 0xFF);
    else
    {
        WPRINT_APP_INFO(("Invalid channel %u\n", (unsigned int)channel));
        return WWD_WLAN_ERROR;
    }

    result = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, interface );

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to set channel %u - err: %u\n", (unsigned int)channel, (unsigned int)result));
        return result;
    }

    return WWD_SUCCESS;
}

wwd_result_t wwd_set_mesh_auth_proto( uint32_t auth_proto, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    uint32_t*      data;
    wwd_result_t   result;

    data = wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 4, "mesh_auth_proto" );
    CHECK_IOCTL_BUFFER( data );

    *data  = auth_proto;

    result = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, interface );

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to set mesh_auth_proto - err: %u\n", (unsigned int)result));
        return result;
    }

    return WWD_SUCCESS;
}

wwd_result_t wwd_set_mesh_security(wiced_security_t auth_type, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    uint32_t*      data;
    uint32_t       bss_index = 0;


    /* Map the interface to a BSS index */
    bss_index = wwd_get_bss_index( interface );

    /* Set the security type */
    data = (uint32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 8, "bsscfg:" IOVAR_STR_WSEC );
    CHECK_IOCTL_BUFFER( data );
    data[0] = bss_index;
    data[1] = (uint32_t) ( ( (auth_type | WPS_ENABLED) & ( ~WPS_ENABLED ) ) | SES_OW_ENABLED );
    (void) wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, interface );

    /* Set supplicant variable - mfg app doesn't support these iovars, so don't care if return fails */
    data = wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 8, "bsscfg:" IOVAR_STR_SUP_WPA );
    CHECK_IOCTL_BUFFER( data );
    data[0] = bss_index;
    data[1] = (uint32_t) ( ( ( ( auth_type & WPA_SECURITY )  != 0 ) ||
                           ( ( auth_type & WPA2_SECURITY ) != 0 ) ) ? 1 : 0 );
    (void)wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );

    /* Set the wpa auth */
    data = (uint32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 8, "bsscfg:" IOVAR_STR_WPA_AUTH );
    CHECK_IOCTL_BUFFER( data );
    data[0] = bss_index;
    data[1] = (uint32_t) WPA2_AUTH_PSK;
    (void) wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );

    return WWD_SUCCESS;
}
wwd_result_t wwd_set_mesh_auto_peer( uint32_t auto_peer, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    uint32_t*      data;
    wwd_result_t   result;

    data = wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 4, "mesh_auto_peer" );
    CHECK_IOCTL_BUFFER( data );

    *data  = auto_peer;

    result = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, interface );

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to set mesh_auto_peer - err: %u\n", (unsigned int)result));
        return result;
    }

    return WWD_SUCCESS;
}

wwd_result_t wwd_set_mesh_mcast_rebroadcast( uint32_t mcast_rebro, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    uint32_t*      data;
    wwd_result_t   result;

    data = wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 4, "mesh_mcast_rebro" );
    CHECK_IOCTL_BUFFER( data );

    *data  = mcast_rebro;

    result = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, interface );

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to set mesh_mcast_rebro - err: %u\n", (unsigned int)result));
        return result;
    }

    return WWD_SUCCESS;
}

wwd_result_t wwd_join_mesh( const wiced_ssid_t* ssid, wiced_security_t auth_type,
                            const uint8_t* security_key, uint8_t key_length, wwd_interface_t interface )
{
    wiced_buffer_t      buffer;
    uint32_t*           data;
    char*               jparam;
    wwd_result_t        result;
    uint16_t            join_params_size;
    wl_join_params_t*   join_params = NULL;

    join_params_size        = WL_JOIN_PARAMS_FIXED_SIZE + WL_NUMCHANNELS * sizeof(chanspec_t);
    join_params             = malloc(join_params_size);

    if ( NULL == join_params )
    {
            WPRINT_APP_INFO(("Error allocating %u bytes for assoc params\n", (unsigned int)join_params_size));
            return WWD_MALLOC_FAILURE;
    }

    memset( join_params, 0, join_params_size );
    join_params->params.bssid.octet[0] = 0xFF;
    join_params->params.bssid.octet[1] = 0xFF;
    join_params->params.bssid.octet[2] = 0xFF;
    join_params->params.bssid.octet[3] = 0xFF;
    join_params->params.bssid.octet[4] = 0xFF;
    join_params->params.bssid.octet[5] = 0xFF;

    join_params->ssid.SSID_len = ssid->length;
    memcpy( join_params->ssid.SSID, ssid->value, ssid->length );

    data = wwd_sdpcm_get_ioctl_buffer( &buffer, sizeof(uint32_t) );
    if ( NULL == data )
    {
        result = WWD_BUFFER_UNAVAILABLE_TEMPORARY;
        goto error;
    }

    *data = 2;

    result = wwd_sdpcm_send_ioctl( SDPCM_SET, WLC_SET_INFRA, buffer, NULL, interface );

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to set imode to mesh - err: %u\n", (unsigned int)result));
        goto error;
    }

    data = wwd_sdpcm_get_ioctl_buffer(&buffer, sizeof(uint32_t));
    if ( NULL == data )
    {
        result = WWD_BUFFER_UNAVAILABLE_TEMPORARY;
        goto error;
    }
    *data = 0;

    result = wwd_sdpcm_send_ioctl(SDPCM_SET, WLC_SET_AUTH, buffer, NULL, interface);

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to set auth - err: %u\n", (unsigned int)result));
        goto error;
    }

    if ( auth_type == WICED_SECURITY_WPA2_AES_PSK )
    {
        wwd_set_mesh_security(auth_type, interface);
        wwd_wifi_set_passphrase(security_key, key_length, interface);
    }

    jparam = wwd_sdpcm_get_ioctl_buffer(&buffer, join_params_size);

    if ( NULL == jparam )
    {
        result = WWD_BUFFER_UNAVAILABLE_TEMPORARY;
        goto error;
    }

    memcpy( jparam, (char *)join_params, join_params_size );

    result = wwd_sdpcm_send_ioctl( SDPCM_SET, WLC_SET_SSID, buffer, NULL, interface );
    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to set SSID - err: %u\n", (unsigned int)result));
        goto error;
    }

    return WWD_SUCCESS;

error:
    free( join_params );
    return result;
}

wwd_result_t wwd_mesh_filter( wiced_mac_t *mac, wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    char*      data;
    wwd_result_t   result;

    data = wwd_sdpcm_get_iovar_buffer( &buffer, ETHER_ADDR_LEN, "mesh_filter" );
    CHECK_IOCTL_BUFFER( data );
    memcpy(data, (char *)&mac->octet, ETHER_ADDR_LEN);

    result = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, interface );

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to set mesh filter- err: %u\n", (unsigned int)result));
        return WWD_WLAN_BADLEN;
    }

    return WWD_SUCCESS;
}

wwd_result_t wwd_mesh_status( char *result_buf, uint16_t result_buf_sz )
{
    wiced_buffer_t buffer, response;
    mesh_peer_info_dump_t *data;
    wwd_result_t   result;

    data = wwd_sdpcm_get_iovar_buffer( &buffer, result_buf_sz, IOVAR_STR_MESH_PEER_STATUS );
    if (data == NULL) {
        WPRINT_APP_INFO(("%s: Alloc failed\n", __FUNCTION__));
        return WWD_BUFFER_ALLOC_FAIL;
    }

    result = wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, WICED_STA_INTERFACE);
    if (result != WWD_SUCCESS)
    {
        WPRINT_APP_INFO(("%s: send_iovar failed\n", __FUNCTION__));
        return WWD_BUFFER_ALLOC_FAIL;
    }

    data = (mesh_peer_info_dump_t*)host_buffer_get_current_piece_data_pointer( response );
    memcpy( result_buf, data, MIN(result_buf_sz, data->buflen) );
    host_buffer_release( response, WWD_NETWORK_RX );

    return result;
}

/** Country code related functions
 */

wwd_result_t wwd_wifi_get_ccode( wwd_country_t *country )
{
    char ch[2];
    memset(country, 0, sizeof(*country));

    ch[0]  =  (char ) ( (char)( wwd_wlan_status.country_code)  & 0xff);
    ch[1]  =  (char ) ( (char)( wwd_wlan_status.country_code >> 8) & 0xff );

    memcpy(country, ch, sizeof(ch));
    return WWD_SUCCESS;
}

wwd_result_t wwd_wifi_set_bandwidth( uint8_t bw )
{
    bandwidth = bw;
    return WWD_SUCCESS;
}

uint32_t wwd_wifi_get_bandwidth ( void )
{
    return bandwidth;
}

wwd_result_t wwd_wifi_set_ccode( wwd_country_t *country )
{
    wl_country_t*  country_struct;
    wiced_buffer_t buffer;
    wwd_result_t   retval;
    char *ccode = (char *)country;

    int len = 0;

    if ( ( ccode[2] != '\0' ) || ( ccode[3] != '\0' ) )
    {
        wiced_assert( "Country code must be 2 characters",  0 != 0 );
        return WWD_BADARG;
    }

    len = (int)strlen(*country);

    wiced_assert( "Wrong ccode format!", ccode[len] == '\0' );
    country_struct = (wl_country_t*) wwd_sdpcm_get_iovar_buffer( &buffer,
            (uint16_t) sizeof(wl_country_t), IOVAR_STR_COUNTRY );
    if ( country_struct == NULL )
    {
        wiced_assert( "Could not get buffer for IOCTL", 0 != 0 );
        return WWD_BUFFER_ALLOC_FAIL;
    }

    memset ( country_struct, 0, sizeof( *country_struct ) );
    memcpy ( country_struct->country_abbrev, country, sizeof(country_struct->country_abbrev) );
    memcpy ( country_struct->ccode, country, sizeof(country_struct->ccode) );


    /*  set regrev to -1 to let the fw to look for setting in aggregated code table */
    country_struct->rev = -1;

    retval = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );
    if ( retval == WWD_SUCCESS )
    {
        wwd_wlan_status.country_code = (wiced_country_code_t ) MK_CNTRY((unsigned char )country_struct->ccode[0], (unsigned char)country_struct->ccode[1], 0);
    }

    return retval;
}

wwd_result_t wwd_wifi_interface_up( wwd_interface_t interface );
/*****/
wwd_result_t wwd_wifi_interface_up( wwd_interface_t interface )
{
    wiced_buffer_t buffer;
    uint32_t*      data;
    uint32_t bss_index = wwd_get_bss_index(interface );

    data = (uint32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 8, IOVAR_STR_BSS );
    CHECK_IOCTL_BUFFER( data );
    data[0] = bss_index;
    data[1] = 1; //BSS_UP;
    if ( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE ) != WWD_SUCCESS) {
        WPRINT_APP_INFO(( "%s: Cannot bring up interface %ld\n", __FUNCTION__ , bss_index));
        return WWD_WLAN_ERROR;
    }

    return WWD_SUCCESS;
}
/*****/

#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
wwd_result_t wwd_wifi_start_sta( wwd_interface_t interface )
{
    /* taken from internal_ap_init(); */
    wiced_buffer_t response;
    wiced_buffer_t buffer;
    uint32_t*      data;
    uint32_t bss_index = wwd_get_bss_index(interface );

    /* For now, assume bss_index has not been initialized.
     * Going forward it will need to chck if it already exists
     */
    /* Query bss state (does it exist? if so is it UP?) */
    data = (uint32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 8, IOVAR_STR_BSS );
    CHECK_IOCTL_BUFFER( data );
    data[0] = bss_index;
    data[1] = 2; /* STA */
    if ( wwd_sdpcm_send_iovar( SDPCM_SET, buffer, &response, WWD_STA_INTERFACE ) != WWD_SUCCESS )
    {
        /* Note: Don't need to release the response packet since the iovar failed */
        WPRINT_APP_INFO(( "%s: BSS failed => cannot create interface %ld\n", __FUNCTION__ , bss_index));
        return WWD_WLAN_ERROR;
    }
    WPRINT_APP_INFO(( "Creating second STA on index %ld\n", bss_index));
    host_buffer_release( response, WWD_NETWORK_RX );

    return WWD_SUCCESS;
}
#endif
