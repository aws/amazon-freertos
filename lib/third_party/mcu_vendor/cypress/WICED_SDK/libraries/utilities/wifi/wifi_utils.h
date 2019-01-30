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
 * @file Wifi utilities
 *
 * Utilities to help do specialized (not general purpose) Wi-Fi specific things
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#define MAX_SAE_PASSWORD_LEN ( 128 )
#define MAX_PASSPHRASE_LEN   ( 64 )
#define MIN_PASSPHRASE_LEN   ( 8 )
 #define A_SHA_DIGEST_LEN    ( 20 )
#define DOT11_PMK_LEN        ( 32 )
#define MAX_SSID_LEN 32

/*!
 ******************************************************************************
 * Join an 802.11 Access Point (AP) and bring up the TCP/IP address stack afterwards, using
 * the given IPv4 address parameters if apecified.  If none specified, use DHCP to configure IPv4.
 *
 * @param[in] ssid AP's SSID
 * @param[in] ssid_length length of the SSID
 * @param[in] auth_type AP's authentication
 * @param[in] key key to use for joining; may be NULL
 * @param[in] key_length length of key param
 * @param[in] ip IPv4 address to use after join; may be NULL
 * @param[in] netmask netmask to use after join; can be NULL only if ip parameter is also NULL
 * @param[in] gateway gateway to use after join; can be NULL only if ip parameter is also NULL
 *
 * @return 0 if successful; non-zero otherwise
 */
int wifi_utils_join( char* ssid, uint8_t ssid_length, wiced_security_t auth_type, uint8_t* key, uint16_t key_length, char* ip, char* netmask, char* gateway );

/*!
 ******************************************************************************
 * Convert a security authentication type string to a wiced_security_t.
 *
 * @param[in] arg  The string containing the value.
 *
 * @return    The value represented by the string.
 */
wiced_security_t wifi_utils_str_to_authtype( char* arg );

/*!
 ******************************************************************************
 * Convert a security authentication type string to an enterprise wiced_security_t.
 *
 * @param[in] arg  The string containing the value.
 *
 * @return    The value represented by the string.
 */
wiced_security_t wifi_utils_str_to_enterprise_authtype( char* arg );

/*!
 ******************************************************************************
 * Convert a MAC string (xx:xx:xx:xx:xx) to a wiced_mac_t.
 *
 * @param[in] arg  The string containing the value.
 * @param[out] mac  The mac address to write into.
 *
 * @return    The value represented by the string.
 */
void wifi_utils_str_to_mac( char* arg, wiced_mac_t* mac );

/*!
 ******************************************************************************
 * Convert a security authentication type string to a wiced_security_t.
 *
 * @param[in] interface  The interface to query and print a network status report.
 *
 * @return
 */
void wifi_utils_network_print_status( wiced_interface_t interface );

/*!
 ******************************************************************************
 * Analyse failed join result
 *
 * @param[in] join_result  Result of join attempts.
 *
 * @return
 */
void wifi_utils_analyse_failed_join_result( wiced_result_t join_result );

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
int wifi_utils_str_to_channel_list( char* arg, uint16_t chan_list[] );

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
wiced_bool_t wifi_utils_str_to_extended_params( char* arg, wiced_scan_extended_params_t* extended_params );

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
int wifi_utils_str_to_channel_list( char* arg, uint16_t chan_list[] );

/*!
  ******************************************************************************
 * Convert an IOCTL to a string.
 *
 * @param[in] cmd  The ioct_log value.
 * @param[out] ioctl_str The string value after conversion.
 * @param[out] ioctl_str_len The string length of the IOCTL string.
 *
 * @result
 */
void wifi_utils_ioctl_to_string(uint32_t cmd, char* ioctl_str, uint16_t ioctl_str_len);


/*!
  ******************************************************************************
 * Convert event, status and reason value coming from firmware to string.
 *
 * @param[in] cmd  The event value in numeric form.
 * @param[in] flag  The status value in numeric form.
 * @param[in] reason  The reson value in numeric form.
 * @param[out] ioctl_str  The string representation of event, status and reason.
 * @param[out] ioctl_str_len  The str_len of ioctl_str.
 *
 * @result
 */
void wifi_utils_event_info_to_string(uint32_t cmd, uint16_t flag, uint32_t reason, char* ioctl_str, uint16_t ioctl_str_len);

/*!
  ******************************************************************************
 * Prints Hexdump and ASCII dump for data passed as argument.
 *
 * @param[in] data  The data which has to be converted into hex and ascii format.
 * @param[in] data_len The length of data.
 *
 * @result
 */
void wifi_utils_hexdump(uint8_t *data, uint32_t data_len);



#ifdef __cplusplus
} /* extern "C" */
#endif
