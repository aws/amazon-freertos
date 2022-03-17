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

/**
 * @file Wiced log routines
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "wwd_wlioctl.h"
#include "wwd_wifi.h"
#include "string.h"
#include "wwd_debug.h"
#include "wwd_assert.h"
#include "network/wwd_network_interface.h"
#include "wwd_management.h"
#include "wiced_management.h"
#include "wiced_crypto.h"
#include "wiced.h"
#include "internal/wiced_internal_api.h"
#include "wiced_utilities.h"

#include "wifi_utils.h"
/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/
typedef enum
{
    ERR_CMD_OK           =  0,
    ERR_UNKNOWN          = -1,
    ERR_UNKNOWN_CMD      = -2,
    ERR_INSUFFICENT_ARGS = -3,
    ERR_TOO_MANY_ARGS    = -4,
    ERR_ADDRESS          = -5,
    ERR_NO_CMD           = -6,
    ERR_TOO_LARGE_ARG    = -7,
    ERR_OUT_OF_HEAP      = -8,
    ERR_BAD_ARG          = -9,
    /* !!!when adding values here, also update command_console.c:console_default_error_strings */
    ERR_LAST_ERROR       = -10
} cmd_err_t;


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
static char last_joined_ssid[SSID_NAME_SIZE+1] = ""; /* 32 characters + terminating null */

/******************************************************
 *               Function Definitions
 *
 */

int wifi_utils_join(char* ssid, uint8_t ssid_length, wiced_security_t auth_type, uint8_t* key, uint16_t key_length, char* ip, char* netmask, char* gateway )
{
    wiced_network_config_t      network_config;
    wiced_ip_setting_t*         ip_settings = NULL;
    wiced_ip_setting_t          static_ip_settings;
    platform_dct_wifi_config_t* dct_wifi_config;
    wiced_result_t              result;

    if (ssid_length > SSID_NAME_SIZE)
    {
        WPRINT_APP_INFO(("SSID too long\n"));
        return ERR_TOO_LARGE_ARG;
    }

    if (wwd_wifi_is_ready_to_transceive(WWD_STA_INTERFACE) == WWD_SUCCESS)
    {
        WPRINT_APP_INFO(("STA already joined; use leave command first\n"));
        return ERR_CMD_OK;
    }

    /* Read config */
    wiced_dct_read_lock( (void**) &dct_wifi_config, WICED_TRUE, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t) );

    /* Modify config */
    dct_wifi_config->stored_ap_list[0].details.SSID.length = ssid_length;
    memset( dct_wifi_config->stored_ap_list[0].details.SSID.value, 0, sizeof(dct_wifi_config->stored_ap_list[0].details.SSID.value) );
    memcpy( (char*)dct_wifi_config->stored_ap_list[0].details.SSID.value, ssid, ssid_length );
    dct_wifi_config->stored_ap_list[0].details.security = auth_type;
    if ( ( auth_type & ENTERPRISE_ENABLED ) == 0 )
    {
        /* Save credentials for non-enterprise AP */
        memcpy((char*)dct_wifi_config->stored_ap_list[0].security_key, (char*)key, MAX_PASSPHRASE_LEN);
        dct_wifi_config->stored_ap_list[0].security_key_length = key_length;
    }

    /* Write config */
    wiced_dct_write( (const void*) dct_wifi_config, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t) );

    /* Tell the network stack to setup it's interface */
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
        ip_settings = &static_ip_settings;
    }

    if ( ( result = wiced_network_up( WICED_STA_INTERFACE, network_config, ip_settings ) ) != WICED_SUCCESS )
    {
        if ( auth_type == WICED_SECURITY_WEP_PSK ) /* Now try shared instead of open authentication */
        {
            dct_wifi_config->stored_ap_list[0].details.security = WICED_SECURITY_WEP_SHARED;
            wiced_dct_write( (const void*) dct_wifi_config, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t) );
            WPRINT_APP_INFO(("WEP with open authentication failed, trying WEP with shared authentication...\n"));

            if ( wiced_network_up( WICED_STA_INTERFACE, network_config, ip_settings ) != WICED_SUCCESS ) /* Restore old value */
            {
                WPRINT_APP_INFO(("Trying shared wep\n"));
                dct_wifi_config->stored_ap_list[0].details.security = WICED_SECURITY_WEP_PSK;
                wiced_dct_write( (const void*) dct_wifi_config, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t) );
            }
            else
            {
                memset( dct_wifi_config->stored_ap_list[0].security_key, 0, SECURITY_KEY_SIZE );
                wiced_dct_read_unlock( (void*) dct_wifi_config, WICED_TRUE );
                return ERR_CMD_OK;
            }
        }

        wifi_utils_analyse_failed_join_result( result );
        memset( dct_wifi_config->stored_ap_list[0].security_key, 0, SECURITY_KEY_SIZE );
        wiced_dct_read_unlock( (void*) dct_wifi_config, WICED_TRUE );

        return ERR_UNKNOWN;
    }

    strlcpy( last_joined_ssid, ssid, MIN( sizeof(last_joined_ssid), ssid_length+1 ) );

    memset( dct_wifi_config->stored_ap_list[0].security_key, 0, SECURITY_KEY_SIZE );
    wiced_dct_read_unlock( (void*) dct_wifi_config, WICED_TRUE );

    return ERR_CMD_OK;
}

wiced_security_t wifi_utils_str_to_authtype( char* arg )
{
    if ( strcmp( arg, "open" ) == 0 )
    {
        return WICED_SECURITY_OPEN;
    }
    else if ( strcmp( arg, "wep" ) == 0 )
    {
        return WICED_SECURITY_WEP_PSK;
    }
    else if ( strcmp( arg, "wep_shared" ) == 0 )
    {
        return WICED_SECURITY_WEP_SHARED;
    }
    else if ( strcmp( arg, "wpa2_tkip" ) == 0 )
    {
        return WICED_SECURITY_WPA2_TKIP_PSK;
    }
    else if ( strcmp( arg, "wpa2_aes" ) == 0 )
    {
        return WICED_SECURITY_WPA2_AES_PSK;
    }
    else if ( strcmp( arg, "wpa2" ) == 0 )
    {
        return WICED_SECURITY_WPA2_MIXED_PSK;
    }
    else if ( strcmp( arg, "wpa_aes" ) == 0 )
    {
        return WICED_SECURITY_WPA_AES_PSK;
    }
    else if ( strcmp( arg, "wpa_tkip" ) == 0 )
    {
        return WICED_SECURITY_WPA_TKIP_PSK;
    }
    else if ( strcmp( arg, "wpa2_fbt" ) == 0 )
    {
        return WICED_SECURITY_WPA2_FBT_PSK;
    }
    else if ( strcmp( arg, "wpa3" ) == 0 )
    {
        return WICED_SECURITY_WPA3_SAE;
    }
    else if ( strcmp( arg, "wpa3_wpa2" ) == 0)
    {
        return WICED_SECURITY_WPA3_WPA2_PSK;
    }
    else
    {
        WPRINT_APP_INFO( ("Bad auth type: '%s'\r\n", arg) );
        return WICED_SECURITY_UNKNOWN;
    }
}

wiced_security_t wifi_utils_str_to_enterprise_authtype( char* arg )
{
    if ( strcmp( arg, "wpa2_tkip" ) == 0 )
    {
        return WICED_SECURITY_WPA2_TKIP_ENT;
    }
    else if ( strcmp( arg, "wpa2" ) == 0 )
    {
        return WICED_SECURITY_WPA2_MIXED_ENT;
    }
    else if ( strcmp( arg, "wpa" ) == 0 )
    {
        return WICED_SECURITY_WPA_MIXED_ENT;
    }
    else if ( strcmp( arg, "wpa_tkip" ) == 0 )
    {
        return WICED_SECURITY_WPA_TKIP_ENT;
    }
    else if ( strcmp( arg, "wpa2_fbt" ) == 0 )
    {
        return WICED_SECURITY_WPA2_FBT_ENT;
    }
    else
    {
        WPRINT_APP_INFO( ("Bad auth type: '%s'\r\n", arg) );
        return WICED_SECURITY_UNKNOWN;
    }
}

void wifi_utils_str_to_mac( char* arg, wiced_mac_t* mac )
{
    char* start = arg;
    char* end;
    int a = 0;
    do
    {
        uint32_t tmp_val;
        end = strchr( start, ':' );
        if ( end != NULL )
        {
            *end = '\0';
        }
        string_to_unsigned( start, 2, &tmp_val, 1);
        mac->octet[a] = (uint8_t) tmp_val;
        if ( end != NULL )
        {
            *end = ':';
            start = end + 1;
        }
        ++a;
    } while ( a < 6 && end != NULL );
}

void wifi_utils_network_print_status( wiced_interface_t interface )
{
    if ( wiced_network_is_ip_up( interface ) )
    {
        wiced_ip_address_t netmask;
        wiced_ip_address_t gateway;
        wiced_ip_address_t ipv4_address;
        wiced_ip_get_netmask( interface, &netmask );
        wiced_ip_get_gateway_address( interface, &gateway );
        wiced_ip_get_ipv4_address( interface, &ipv4_address );
        WPRINT_APP_INFO( ( "   IP Addr     : %u.%u.%u.%u\n", (uint8_t)(GET_IPV4_ADDRESS(ipv4_address) >> 24),
                                                             (uint8_t)(GET_IPV4_ADDRESS(ipv4_address) >> 16),
                                                             (uint8_t)(GET_IPV4_ADDRESS(ipv4_address) >> 8),
                                                             (uint8_t)(GET_IPV4_ADDRESS(ipv4_address) >> 0) ) );

        WPRINT_APP_INFO( ( "   Gateway     : %u.%u.%u.%u\n", (uint8_t)(GET_IPV4_ADDRESS(gateway) >> 24),
                                                             (uint8_t)(GET_IPV4_ADDRESS(gateway) >> 16),
                                                             (uint8_t)(GET_IPV4_ADDRESS(gateway) >> 8),
                                                             (uint8_t)(GET_IPV4_ADDRESS(gateway) >> 0) ) );

        WPRINT_APP_INFO( ( "   Netmask     : %u.%u.%u.%u\n", (uint8_t)(GET_IPV4_ADDRESS(netmask) >> 24),
                                                             (uint8_t)(GET_IPV4_ADDRESS(netmask) >> 16),
                                                             (uint8_t)(GET_IPV4_ADDRESS(netmask) >> 8),
                                                             (uint8_t)(GET_IPV4_ADDRESS(netmask) >> 0) ) );
    }
}

void wifi_utils_analyse_failed_join_result( wiced_result_t join_result )
{
    /* Note that DHCP timeouts and EAPOL key timeouts may happen at the edge of the cell. If possible move closer to the AP. */
    /* Also note that the default number of join attempts is three and the join result is returned for the last attempt. */
    WPRINT_APP_INFO( ("Join result %u: ", (unsigned int)join_result) );
    switch( join_result )
    {
        case WICED_ERROR:
            WPRINT_APP_INFO( ("General error\n") ); /* Getting a DHCP address may fail if at the edge of a cell and the join may timeout before DHCP has completed. */
            break;

        case WWD_NETWORK_NOT_FOUND:
            WPRINT_APP_INFO( ("Failed to find network\n") ); /* Check that he SSID is correct and that the AP is up */
            break;

        case WWD_NOT_AUTHENTICATED:
            WPRINT_APP_INFO( ("Failed to authenticate\n") ); /* May happen at the edge of the cell. Try moving closer to the AP. */
            break;

        case WWD_EAPOL_KEY_PACKET_M1_TIMEOUT:
            WPRINT_APP_INFO( ("Timeout waiting for first EAPOL key frame from AP\n") );
            break;

        case WWD_EAPOL_KEY_PACKET_M3_TIMEOUT:
            WPRINT_APP_INFO( ("Check the passphrase and try again\n") ); /* The M3 timeout will occur if the passphrase is incorrect */
            break;

        case WWD_EAPOL_KEY_PACKET_G1_TIMEOUT:
            WPRINT_APP_INFO( ("Timeout waiting for group key from AP\n") );
            break;

        case WWD_INVALID_JOIN_STATUS:
            WPRINT_APP_INFO( ("Some part of the join process did not complete\n") ); /* May happen at the edge of the cell. Try moving closer to the AP. */
            break;

        default:
            WPRINT_APP_INFO( ("\n") );
            break;
    }
}

/*!
  ******************************************************************************
 * Convert a channel_list string (xx,xx,xx,xx,xx) to an array.
 *
 * @param[in] arg  The string containing list in bracket, delimited by ',', no space.
 *
 * @param[out] chan_list   The value represented by the string.
 *
 * @result valid number size including ending 0.
 */
int wifi_utils_str_to_channel_list( char* arg, uint16_t chan_list[] )
{
    char* p = strtok(arg, "(),");
    int i = 0;

    while ( (p != NULL) && (i < WICED_MAX_CHANNEL_NUM) )
    {
       chan_list[i++] = atoi( p );
       p = strtok(NULL, "(),");
    }

    chan_list[i++] = 0;
    return i;
}


/*!
 ******************************************************************************
 * Parse extended_params string tuple (xx,xx,xx,xx) fill in structure values.
 *
 * @param[in] arg  The string containing list in bracket, delimited by ',', no space.
 *
 * @param[out] extended_params  The structure to fill in values.
 *
 * @result @ref wiced_bool_t indicating if the operation success.
 */
wiced_bool_t wifi_utils_str_to_extended_params( char* arg, wiced_scan_extended_params_t* extended_params )
{
    char* p = strtok(arg, "(),");
    int i = 0;

    while ( p != NULL )
    {
        switch (i)
        {
            case 0:
                extended_params->number_of_probes_per_channel = atoi( p );
                break;
            case 1:
                extended_params->scan_active_dwell_time_per_channel_ms = atoi( p );
                break;
            case 2:
                extended_params->scan_passive_dwell_time_per_channel_ms = atoi( p );
                break;
            case 3:
                extended_params->scan_home_channel_dwell_time_between_channels_ms = atoi( p );
                break;
            default:
                return WICED_FALSE;
        }

        i++;
        p = strtok(NULL, "(),");
    }

    if ( i < 4 )
        return WICED_FALSE;
    else
        return WICED_TRUE;
}

/*
 *  This Wiced API sets iovar value of type uint32
 */
wiced_result_t wiced_wifi_set_iovar_value ( const char* iovar, uint32_t value, wiced_interface_t interface )
{
   return wwd_wifi_set_iovar_value( iovar, value, interface );
}

/*
 *  This Wiced API gets iovar value of type uint32
 */
wiced_result_t wiced_wifi_get_iovar_value ( const char* iovar, uint32_t* value, wiced_interface_t interface )
{
   return wwd_wifi_get_iovar_value( iovar, value, interface );
}

void wifi_utils_hexdump(uint8_t *data, uint32_t data_len)
{
    uint32_t i;
    uint8_t buff[16] = {0};

    for ( i = 0; i < data_len; i++ )
    {
        if ( (i % 16) == 0 )
        {
            if (i != 0)
            {
                printf("  %s\n", buff);
            }
            printf("%04lx ", i);
        }
        printf(" %02x", data[i]);

        if ( ( data[i] < 0x20 ) || ( data[i] > 0x7e ) )
        {
            buff[i % 16] = '.';
        }
        else
        {
            buff[i % 16] = data[i];
        }
        buff[(i % 16) + 1] = '\0';
    }
    while ( (i % 16) != 0 )
    {
        printf("   ");
        i++;
    }
    printf("  %s\n", buff);
}

void wifi_utils_ioctl_to_string(uint32_t cmd, char* ioctl_str, uint16_t ioctl_str_len)
{
    if (cmd == 2)
    {
        strncpy( ioctl_str, "WLC_UP", ioctl_str_len );
    }
    else if (cmd == 20)
    {
        strncpy( ioctl_str, "WLC_SET_INFRA", ioctl_str_len );
    }
    else if (cmd == 22)
    {
        strncpy( ioctl_str, "WLC_SET_AUTH", ioctl_str_len );
    }
    else if (cmd == 26)
    {
        strncpy( ioctl_str, "WLC_SET_SSID", ioctl_str_len );
    }
    else if (cmd == 52)
    {
        strncpy( ioctl_str, "WLC_DISASSOC", ioctl_str_len );
    }
    else if (cmd == 55)
    {
        strncpy( ioctl_str, "WLC_SET_ROAM_TRIGGER", ioctl_str_len );
    }
    else if (cmd == 57)
    {
        strncpy( ioctl_str, "WLC_SET_ROAM_DELTA", ioctl_str_len );
    }
    else if (cmd == 59)
    {
        strncpy( ioctl_str, "WLC_SET_ROAM_SCAN_PERIOD", ioctl_str_len );
    }
    else if (cmd == 110)
    {
        strncpy( ioctl_str, "WLC_SET_GMODE", ioctl_str_len );
    }
    else if (cmd == 116)
    {
        strncpy( ioctl_str, "WLC_SET_SCANSUPPRESS", ioctl_str_len );
    }
    else if (cmd == 134)
    {
        strncpy( ioctl_str, "WLC_SET_WSEC", ioctl_str_len );
    }
    else if (cmd == 165)
    {
        strncpy( ioctl_str, "WLC_SET_WPA_AUTH", ioctl_str_len );
    }
    else if (cmd == 268)
    {
        strncpy( ioctl_str, "WLC_SET_WSEC_PMK", ioctl_str_len );
    }
}

void wifi_utils_event_info_to_string(uint32_t cmd, uint16_t flag, uint32_t reason, char* ioctl_str, uint16_t ioctl_str_len)
{
    if (cmd == 0)
    {
        strncpy( ioctl_str, "WLC_E_SET_SSID", ioctl_str_len );
    }
    else if (cmd == 3)
    {
        strncpy( ioctl_str, "WLC_E_AUTH    ", ioctl_str_len );
    }
    else if (cmd == 16)
    {
        strncpy( ioctl_str, "WLC_E_LINK    ", ioctl_str_len );
    }
    else if (cmd == 46)
    {
        strncpy( ioctl_str, "WLC_E_PSK_SUP ", ioctl_str_len );
    }
    else if (cmd == 54)
    {
        strncpy( ioctl_str, "WLC_E_IF      ", ioctl_str_len );
    }
    else if (cmd == 69)
    {
        strncpy( ioctl_str, "WLC_E_ESCAN_RESULT", ioctl_str_len );
    }

    if ( flag == 0 )
    {
        strncat(ioctl_str, "  WLC_E_STATUS_SUCCESS",  ioctl_str_len);
    }
    if ( flag == 8 )
    {
        strncat(ioctl_str, "  WLC_E_STATUS_PARTIAL",  ioctl_str_len);
    }
    else if ( flag == 262)
    {
        strncat(ioctl_str, "  WLC_SUP_KEYED       ",  ioctl_str_len);
    }

    if( reason == 0 )
    {
        strncat(ioctl_str, "    WLC_E_REASON_INITIAL_ASSOC",  ioctl_str_len);
    }
    else if( reason == 512 )
    {
        strncat(ioctl_str, "    WLC_E_SUP_OTHER",  ioctl_str_len);
    }
}




