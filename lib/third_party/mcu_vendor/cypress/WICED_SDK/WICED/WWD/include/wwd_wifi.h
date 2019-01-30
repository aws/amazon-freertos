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
 *  Prototypes of functions for controlling the Wi-Fi system
 *
 *  This file provides prototypes for end-user functions which allow
 *  actions such as scanning for Wi-Fi networks, joining Wi-Fi
 *  networks, getting the MAC address, etc
 *
 */

#ifndef INCLUDED_WWD_WIFI_H
#define INCLUDED_WWD_WIFI_H

#include <stdint.h>
#include "wwd_constants.h"                  /* For wwd_result_t */
#include "wwd_structures.h"
#include "chip_constants.h"
#include "RTOS/wwd_rtos_interface.h"        /* For semaphores */
#include "network/wwd_network_interface.h"  /* For interface definitions */
#include "wwd_structures.h"
#include "wwd_wifi_sleep.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** @cond !ADDTHIS*/

#ifndef WICED_MAXIMUM_LINK_CALLBACK_SUBSCRIPTIONS
#define WICED_MAXIMUM_LINK_CALLBACK_SUBSCRIPTIONS     (5)
#endif

/* Packet Filter Offsets for Ethernet Frames */
#define FILTER_OFFSET_PACKET_START                       0
#define FILTER_OFFSET_ETH_HEADER_DESTINATION_ADDRESS     0
#define FILTER_OFFSET_ETH_HEADER_SOURCE_ADDRESS          6
#define FILTER_OFFSET_ETH_HEADER_ETHERTYPE              12
#define FILTER_OFFSET_ETH_DATA                          14

/* Packet Filter Offsets for ARP Packets */
#define FILTER_OFFSET_ARP_HEADER_START                  14
#define FILTER_OFFSET_ARP_HEADER_HTYPE                  14
#define FILTER_OFFSET_ARP_HEADER_PTYPE                  16
#define FILTER_OFFSET_ARP_HEADER_HLEN                   18
#define FILTER_OFFSET_ARP_HEADER_PLEN                   19
#define FILTER_OFFSET_ARP_HEADER_OPER                   20
#define FILTER_OFFSET_ARP_HEADER_SHA                    22
#define FILTER_OFFSET_ARP_HEADER_SPA                    28
#define FILTER_OFFSET_ARP_HEADER_THA                    30
#define FILTER_OFFSET_ARP_HEADER_TPA                    36

/* Packet Filter Offsets for IPv4 Packets */
#define FILTER_OFFSET_IPV4_HEADER_START                 14
#define FILTER_OFFSET_IPV4_HEADER_VER_IHL               14
#define FILTER_OFFSET_IPV4_HEADER_DSCP_ECN              15
#define FILTER_OFFSET_IPV4_HEADER_TOTAL_LEN             16
#define FILTER_OFFSET_IPV4_HEADER_ID                    18
#define FILTER_OFFSET_IPV4_HEADER_FLAGS_FRAGMENT_OFFSET 20
#define FILTER_OFFSET_IPV4_HEADER_TTL                   22
#define FILTER_OFFSET_IPV4_HEADER_PROTOCOL              23
#define FILTER_OFFSET_IPV4_HEADER_CHECKSUM              24
#define FILTER_OFFSET_IPV4_HEADER_SOURCE_ADDR           26
#define FILTER_OFFSET_IPV4_HEADER_DESTINATION_ADDR      30
#define FILTER_OFFSET_IPV4_HEADER_OPTIONS               34
#define FILTER_OFFSET_IPV4_DATA_START                   38

/* Packet Filter Offsets for IPv4 Packets */
#define FILTER_OFFSET_IPV6_HEADER_START                 14
#define FILTER_OFFSET_IPV6_HEADER_PAYLOAD_LENGTH        18
#define FILTER_OFFSET_IPV6_HEADER_NEXT_HEADER           20
#define FILTER_OFFSET_IPV6_HEADER_HOP_LIMIT             21
#define FILTER_OFFSET_IPV6_HEADER_SOURCE_ADDRESS        22
#define FILTER_OFFSET_IPV6_HEADER_DESTINATION_ADDRESS   38
#define FILTER_OFFSET_IPV6_DATA_START                   54

/* Packet Filter Offsets for ICMP Packets */
#define FILTER_OFFSET_ICMP_HEADER_START                 14

/** @endcond */

#define PM1_POWERSAVE_MODE          ( 1 )
#define PM2_POWERSAVE_MODE          ( 2 )
#define NO_POWERSAVE_MODE           ( 0 )

/* Roaming trigger options */
#define WICED_WIFI_DEFAULT_ROAMING TRIGGER              ( 0 )
#define WICED_WIFI_OPTIMIZE_BANDWIDTH_ROAMING_TRIGGER   ( 1 )
#define WICED_WIFI_OPTIMIZE_DISTANCE_ROAMING_TRIGGER    ( 2 )

/* Phyrate counts */
#define WICED_WIFI_PHYRATE_COUNT       16
#define WICED_WIFI_PHYRATE_LOG_SIZE    WL_PHYRATE_LOG_SIZE
#define WICED_WIFI_PHYRATE_LOG_OFF     0
#define WICED_WIFI_PHYRATE_LOG_TX      1
#define WICED_WIFI_PHYRATE_LOG_RX      2

/* Preferred Network Offload: time between scans
  * Set based on desired reconnect responsiveness after the AP has been off
  * for a long time.  (Reconnect will occur via roam if only off for 10s or less.)
  * Also, larger numbers will generally consume less energy.
  */
#define WICED_WIFI_PNO_SCAN_PERIOD     20

/* Translate wwd index to a zero based index */
#define WWD_INTERFACE_INDEX( interface ) ( int )( interface & ( WWD_INTERFACE_MAX - 1 ) )
#define WWD_INDEX_TO_INTERFACE( index ) ( wwd_interface_t )( index )

/******************************************************
 *                   Enumerations
 ******************************************************/
/******************************************************
 *                   Structures
 ******************************************************/

typedef void (*wwd_wifi_raw_packet_processor_t)( wiced_buffer_t buffer, wwd_interface_t interface );

/******************************************************
 *             Function declarations
 ******************************************************/


/*****************************************************************************/
/** @addtogroup wifijoin   WiFi Join, Scan and Halt Functions
 *  @ingroup wifi
 *
 *  WICED functions to Wifi join/scan
 *
 *  @{
 */
/*****************************************************************************/


/*@-exportlocal@*/ /* Lint: These are API functions it is ok if they are not all used externally - they will be garbage collected by the linker if needed */

/** Scan result callback function pointer type
 *
 * @param result_ptr  : A pointer to the pointer that indicates where to put the next scan result
 * @param user_data   : User provided data
 * @param status      : Status of scan process
 */
typedef void (*wiced_scan_result_callback_t)( wiced_scan_result_t** result_ptr, void* user_data, wiced_scan_status_t status );

/** Initiates a scan to search for 802.11 networks.
 *
 *  The scan progressively accumulates results over time, and may take between 1 and 10 seconds to complete.
 *  The results of the scan will be individually provided to the callback function.
 *  Note: The callback function will be executed in the context of the WICED thread and so must not perform any
 *  actions that may cause a bus transaction.
 *
 * @param[in]  scan_type     : Specifies whether the scan should be Active, Passive or scan Prohibited channels
 * @param[in]  bss_type      : Specifies whether the scan should search for Infrastructure networks (those using
 *                             an Access Point), Ad-hoc networks, or both types.
 * @param[in]  optional_ssid : If this is non-Null, then the scan will only search for networks using the specified SSID.
 * @param[in]  optional_mac  : If this is non-Null, then the scan will only search for networks where
 *                             the BSSID (MAC address of the Access Point) matches the specified MAC address.
 * @param[in] optional_channel_list    : If this is non-Null, then the scan will only search for networks on the
 *                                       specified channels - array of channel numbers to search, terminated with a zero
 * @param[in] optional_extended_params : If this is non-Null, then the scan will obey the specifications about
 *                                       dwell times and number of probes.
 * @param callback[in]   : the callback function which will receive and process the result data.
 * @param result_ptr[in] : a pointer to a pointer to a result storage structure.
 * @param user_data[in]  : user specific data that will be passed directly to the callback function
 *
 * @note : When scanning specific channels, devices with a strong signal strength on nearby channels may be detected
 * @note : Callback must not use blocking functions, nor use WICED functions, since it is called from the context of the
 *         WWD thread.
 * @note : The callback, result_ptr and user_data variables will be referenced after the function returns.
 *         Those variables must remain valid until the scan is complete.
 *
 * @return    WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_wifi_scan( wiced_scan_type_t                              scan_type,
                                   wiced_bss_type_t                               bss_type,
                                   /*@null@*/ const wiced_ssid_t*                 optional_ssid,
                                   /*@null@*/ const wiced_mac_t*                  optional_mac,
                                   /*@null@*/ /*@unique@*/ const uint16_t*        optional_channel_list,
                                   /*@null@*/ const wiced_scan_extended_params_t* optional_extended_params,
                                   wiced_scan_result_callback_t                   callback,
                                   wiced_scan_result_t**                          result_ptr,
                                   /*@null@*/ void*                               user_data,
                                   wwd_interface_t                                interface );

/** Abort a previously issued scan
 *
 * @return    WICED_SUCCESS or WICED_ERROR
 */
extern wwd_result_t wwd_wifi_abort_scan( void );

/** Enable or disable scan suppression; a state that disallows all Wi-Fi scans
 *
 * @return    WICED_SUCCESS or WICED_ERROR
 */
wwd_result_t wwd_wifi_set_scan_suppress( wiced_bool_t enable_suppression );

/** Sets default scan parameters in FW
 *
 * @param[in]  assoc_time    : Specifies dwell time per channel in associated state
 * @param[in]  unassoc_time  : Specifies dwell time per channel in unassociated state
 * @param[in]  passive_time  : Specifies dwell time per channel for passive scanning
 * @param[in]  home_time     : Specifies dwell time for the home channel between channel scans
 * @param[in]  nprobes       : Specifies number of probes per channel
 *
 * @return    WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_wifi_set_scan_params( uint32_t assoc_time,
                                              uint32_t unassoc_time,
                                              uint32_t passive_time,
                                              uint32_t home_time,
                                              uint32_t nprobes );

/** Sets default scan parameters in FW
 *
 * @param[out]  assoc_time    : Dwell time per channel in associated state
 * @param[out]  unassoc_time  : Dwell time per channel in unassociated state
 * @param[out]  passive_time  : Dwell time per channel for passive scanning
 * @param[out]  home_time     : Dwell time for the home channel between channel scans
 * @param[out]  nprobes       : Number of probes per channel
 *
 * @return    WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_wifi_get_scan_params( uint32_t* assoc_time,
                                              uint32_t* unassoc_time,
                                              uint32_t* passive_time,
                                              uint32_t* home_time,
                                              uint32_t* nprobes );

/** Joins a Wi-Fi network
 *
 * Scans for, associates and authenticates with a Wi-Fi network.
 * On successful return, the system is ready to send data packets.
 *
 * @param[in] ssid       : A null terminated string containing the SSID name of the network to join
 * @param[in] auth_type  : Authentication type:
 *                         - WICED_SECURITY_OPEN           - Open Security
 *                         - WICED_SECURITY_WEP_PSK        - WEP Security with open authentication
 *                         - WICED_SECURITY_WEP_SHARED     - WEP Security with shared authentication
 *                         - WICED_SECURITY_WPA_TKIP_PSK   - WPA Security
 *                         - WICED_SECURITY_WPA2_AES_PSK   - WPA2 Security using AES cipher
 *                         - WICED_SECURITY_WPA2_TKIP_PSK  - WPA2 Security using TKIP cipher
 *                         - WICED_SECURITY_WPA2_MIXED_PSK - WPA2 Security using AES and/or TKIP ciphers
 * @param[in] security_key : A byte array containing either the cleartext security key for WPA/WPA2
 *                           secured networks, or a pointer to an array of wiced_wep_key_t structures
 *                           for WEP secured networks
 * @param[in] key_length  : The length of the security_key in bytes.
 * @param[in] semaphore   : A user provided semaphore that is flagged when the join is complete
 * @param[in] interface   : interface
 *
 * @return    WWD_SUCCESS : when the system is joined and ready to send data packets
 *            Error code   : if an error occurred
 */
extern wwd_result_t wwd_wifi_join( const wiced_ssid_t* ssid, wiced_security_t auth_type, const uint8_t* security_key, uint8_t key_length, host_semaphore_type_t* semaphore, wwd_interface_t interface );

/**
 * Halt any joins, including ongoing ones
 * @param[in] halt: WICED_TRUE: halt all join attempts; WICED_FALSE: allow join attempts to proceed
 * Calling applications are expected to manage the join state independently from this API.  That is, this API gives no guarantee that
 * an in progress join is cancelled successfully.  However, calling wwd_wifi_leave after this API would be acceptable to ensure the host is in an unassociated state.
 * !!!!NOTE: wwd_wifi_join_halt( WICED_FALSE ) needs to be called after any wwd_wifi_join_halt( WICED_TRUE ) call
 * to allow subsequent join attempts to proceed.
 * @return @ref wiced_result_t WICED_SUCCESS
 */
extern wwd_result_t wwd_wifi_join_halt( wiced_bool_t halt );

/* Return WICED_TRUE if if something else besides STA is up and ready for IO or STA not ready for IO */
extern wiced_bool_t wwd_wifi_sta_is_only_connected( void );

/**
 * Query: Is there a join in progress that can be halted?
 * @param[in] interface: interface to halt join on
 * @return @ref wiced_result_t WICED_TRUE if join can be halted currently; WICED_FALSE otherwise
 */
extern wiced_bool_t wwd_wifi_join_is_ready_to_halt( wwd_interface_t interface );

/** Joins a specific Wi-Fi network
 *
 * Associates and authenticates with a specific Wi-Fi access point.
 * On successful return, the system is ready to send data packets.
 *
 * @param[in] ap           : A pointer to a wiced_scan_result_t structure containing AP details
 * @param[in] security_key : A byte array containing either the cleartext security key for WPA/WPA2
 *                           secured networks, or a pointer to an array of wiced_wep_key_t structures
 *                           for WEP secured networks
 * @param[in] key_length   : The length of the security_key in bytes.
 * @param[in] semaphore    : A user provided semaphore that is flagged when the join is complete
 *
 * @return    WWD_SUCCESS : when the system is joined and ready to send data packets
 *            Error code   : if an error occurred
 */
extern wwd_result_t wwd_wifi_join_specific( const wiced_scan_result_t* ap, const uint8_t* security_key, uint8_t key_length, host_semaphore_type_t* semaphore, wwd_interface_t interface );

/** Disassociates from a Wi-Fi network.
 *
 * @return    WWD_SUCCESS : On successful disassociation from the AP
 *            Error code   : If an error occurred
 */
extern wwd_result_t wwd_wifi_leave( wwd_interface_t interface );


/** @} */

/*****************************************************************************/
/** @addtogroup wifiutilities   WiFi Utility Functions
 *  @ingroup wifi
 *
 *  WICED functions to
 *    - find AP
 *    - find best channel
 *    - Add/remove custome ie
 *    - wifi up/down
 *    - set/get roam triggers
 *    - get channel
 *    - get MAC address of Wifi interface
 *    - get wifi counters
 *    - set/get listen intervals
 *    - set/get HT (high throughput) mode
 *    - disable 11n support
 *
 *  @{
 */
/*****************************************************************************/


/** Deauthenticates a STA which may or may not be associated to SoftAP or Group Owner
 *
 * @param[in] mac       : Pointer to a variable containing the MAC address to which the deauthentication will be sent
 * @param[in] reason    : Deauthentication reason code
 * @param[in] interface : SoftAP interface or P2P interface

 * @return    WWD_SUCCESS : On successful deauthentication of the other STA
 *            WWD_ERROR   : If an error occurred
 */
extern wwd_result_t  wwd_wifi_deauth_sta( const wiced_mac_t* mac, wwd_dot11_reason_code_t reason, wwd_interface_t interface );

/** Deauthenticates all client STAs associated to SoftAP or Group Owner
 *
 * @param[in] reason    : Deauthentication reason code
 * @param[in] interface : SoftAP interface or P2P interface

 * @return    WWD_SUCCESS : On successful deauthentication of the other STA
 *            WWD_ERROR   : If an error occurred
 */
extern wwd_result_t wwd_wifi_deauth_all_associated_client_stas( wwd_dot11_reason_code_t reason, wwd_interface_t interface );

/** Retrieves the current Media Access Control (MAC) address
 *  (or Ethernet hardware address) of the 802.11 device
 *
 * @param mac Pointer to a variable that the current MAC address will be written to
 * @return    WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_wifi_get_mac_address( wiced_mac_t* mac, wwd_interface_t interface );

/** Retrieves the current Media Access Control (MAC) address
 *  (or Ethernet hardware address) of the 802.11 device
 *  and store it to local cache, so subsequent
 *  wwd_wifi_get_mac_address() be faster.
 *
 * @return    WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_wifi_get_and_cache_mac_address( wwd_interface_t interface );

/** ----------------------------------------------------------------------
 *  WARNING : This function is for internal use only!
 *  ----------------------------------------------------------------------
 *  This function sets the current Media Access Control (MAC) address of the
 *  802.11 device.  To override the MAC address in the Wi-Fi OTP or NVRAM add
 *  a global define in the application makefile as shown below. With this define
 *  in place, the MAC address stored in the DCT is used instead of the MAC in the
 *  OTP or NVRAM.
 *
 *  In <WICED-SDK>/App/my_app/my_app.mk add the following global define
 *    GLOBAL_DEFINES := MAC_ADDRESS_SET_BY_HOST
 *  Further information about MAC addresses is available in the following
 *  automatically generated file AFTER building your first application
 *  <WICED-SDK>/generated_mac_address.txt
 *
 *  NOTE:
 *     Ensure Wi-Fi core and network is down before invoking this function.
 *     Refer wiced_wifi_down() API for details.
 *
 * @param[in] mac Wi-Fi MAC address
 * @return    WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_wifi_set_mac_address( wiced_mac_t mac, wwd_interface_t interface );

/** @} */

#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
extern wwd_result_t wwd_wifi_start_sta( wwd_interface_t interface );
extern wwd_result_t wwd_wifi_set_AP_mac_address( wiced_mac_t mac );
#endif

/*****************************************************************************/
/** @addtogroup wifisoftap     WiFi Soft AP
 *  @ingroup wifi
 *  WICED Wi-Fi functions for Starting/Stopping SoftAP
 *
 *  @{
 */
/****************************************************************************/

/** Starts an infrastructure WiFi network
 *
 * @warning If a STA interface is active when this function is called, the softAP will\n
 *          start on the same channel as the STA. It will NOT use the channel provided!
 *
 * @param[in] ssid       : A null terminated string containing the SSID name of the network to join
 * @param[in] auth_type  : Authentication type: \n
 *                         - WICED_SECURITY_OPEN           - Open Security \n
 *                         - WICED_SECURITY_WPA_TKIP_PSK   - WPA Security \n
 *                         - WICED_SECURITY_WPA2_AES_PSK   - WPA2 Security using AES cipher \n
 *                         - WICED_SECURITY_WPA2_MIXED_PSK - WPA2 Security using AES and/or TKIP ciphers \n
 *                         - WEP security is NOT IMPLEMENTED. It is NOT SECURE! \n
 * @param[in] security_key : A byte array containing the cleartext security key for the network
 * @param[in] key_length   : The length of the security_key in bytes.
 * @param[in] channel      : 802.11 channel number
 *
 * @return    WWD_SUCCESS : if successfully creates an AP
 *            Error code   : if an error occurred
 */
extern wwd_result_t wwd_wifi_start_ap( wiced_ssid_t* ssid, wiced_security_t auth_type, /*@unique@*/ const uint8_t* security_key, uint8_t key_length, uint8_t channel );

/** Stops an existing infrastructure WiFi network
 *
 * @return    WWD_SUCCESS : if the AP is successfully stopped or if the AP has not yet been brought up
 *            Error code   : if an error occurred
 */
extern wwd_result_t wwd_wifi_stop_ap( void );

/** Setup SoftAP
 *
 * @param[in] ssid       : A null terminated string containing the SSID name of the network to start
 * @param[in] auth_type  : Authentication type: \n
 *                         - WICED_SECURITY_OPEN           - Open Security \n
 *                         - WICED_SECURITY_WPA_TKIP_PSK   - WPA Security \n
 *                         - WICED_SECURITY_WPA2_AES_PSK   - WPA2 Security using AES cipher \n
 *                         - WICED_SECURITY_WPA2_MIXED_PSK - WPA2 Security using AES and/or TKIP ciphers \n
 *                         - WEP security is NOT IMPLEMENTED. It is NOT SECURE! \n
 * @param[in] security_key : A byte array containing the cleartext security key for the network
 * @param[in] key_length   : The length of the security_key in bytes.
 * @param[in] channel      : 802.11 channel number
 * @return     @ref wwd_result_t
 */
extern wwd_result_t wwd_wifi_ap_init( wiced_ssid_t* ssid, wiced_security_t auth_type, const uint8_t* security_key, uint8_t key_length, uint8_t channel );

/** start SoftAP
 *
 * @param[in] none
 * @return     @ref wwd_result_t
 */
extern wwd_result_t wwd_wifi_ap_up( void );



/** @} */


/*****************************************************************************/
/** @addtogroup wifipowersave    WiFi Power Saving functions
 *  @ingroup wifi
 *  WICED Wi-Fi functions for WLAN low power modes
 *
 *  @{
 */

/** Enables powersave mode without regard for throughput reduction
 *
 *  This function enables (legacy) 802.11 PS-Poll mode and should be used
 *  to achieve the lowest power consumption possible when the Wi-Fi device
 *  is primarily passively listening to the network
 *
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_wifi_enable_powersave( void );

/** Enables powersave mode on specified interface without regard for throughput reduction
 *
 *  This function enables (legacy) 802.11 PS-Poll mode and should be used
 *  to achieve the lowest power consumption possible when the Wi-Fi device
 *  is primarily passively listening to the network
 *
 * @param[in]   interface             : The variable to set WLAN interface type
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_wifi_enable_powersave_interface( wwd_interface_t interface );

/** Get powersave mode on specified interface
 *
 * @param[in]   interface             : The interface to query for powersave state
 * @param[out]  mode                  : the value of the current powersave state: PM1_POWERSAVE_MODE, PM2_POWERSAVE_MODE, NO_POWERSAVE_MODE
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_wifi_get_powersave_interface( wwd_interface_t interface, uint32_t* mode );

/**
 * Enables powersave mode while attempting to maximise throughput
 *
 * Network traffic is typically bursty. Reception of a packet often means that another
 * packet will be received shortly afterwards (and vice versa for transmit).
 *
 * In high throughput powersave mode, rather then entering powersave mode immediately
 * after receiving or sending a packet, the WLAN chip waits for a timeout period before
 * returning to sleep.
 *
 * @return  WWD_SUCCESS : if power save mode was successfully enabled
 *          Error code   : if power save mode was not successfully enabled
 *
 * @param[in] return_to_sleep_delay : The variable to set return to sleep delay.*
 *
 * return to sleep delay must be set to a multiple of 10 and not equal to zero.
 */
extern wwd_result_t wwd_wifi_enable_powersave_with_throughput( uint16_t return_to_sleep_delay );

/**
 * Enables powersave mode on specified interface while attempting to maximise throughput
 *
 * Network traffic is typically bursty. Reception of a packet often means that another
 * packet will be received shortly afterwards (and vice versa for transmit).
 *
 * In high throughput powersave mode, rather then entering powersave mode immediately
 * after receiving or sending a packet, the WLAN chip waits for a timeout period before
 * returning to sleep.
 *
 * @return  WWD_SUCCESS : if power save mode was successfully enabled
 *          Error code   : if power save mode was not successfully enabled
 *
 * @param[in] return_to_sleep_delay : The variable to set return to sleep delay.*
 * @param[in] interface             : The variable to set WLAN interface type
 *
 * return to sleep delay must be set to a multiple of 10 and not equal to zero.
 */
extern wwd_result_t wwd_wifi_enable_powersave_with_throughput_interface( uint16_t return_to_sleep_delay, wwd_interface_t interface );

/** Disables 802.11 power save mode
 *
 * @return  WWD_SUCCESS : if power save mode was successfully disabled
 *          Error code   : if power save mode was not successfully disabled
 */
extern wwd_result_t wwd_wifi_disable_powersave( void );

/** Disables 802.11 power save mode on specified interface
 *
 * @return  WWD_SUCCESS : if power save mode was successfully disabled
 *          Error code   : if power save mode was not successfully disabled
 *
 * @param[in] interface             : The variable to set WLAN interface type
 */
extern wwd_result_t wwd_wifi_disable_powersave_interface( wwd_interface_t interface );

/** @} */

/** @addtogroup wifiutilities   WiFi Utility Functions
 *  @ingroup wifi
 *
 *  @{
 *
 */

/** Determines if a particular interface is ready to transceive ethernet packets
 *
 * @param     Radio interface to check, options are WICED_STA_INTERFACE, WICED_AP_INTERFACE
 * @return    WWD_SUCCESS  : if the interface is ready to transceive ethernet packets
 * @return    WICED_NOTFOUND : no AP with a matching SSID was found
 * @return    WICED_NOT_AUTHENTICATED: a matching AP was found but it won't let you authenticate.
 *                                     This can occur if this device is in the block list on the AP.
 * @return    WICED_NOT_KEYED: the device has authenticated and associated but has not completed the
 *                             key exchange. This can occur if the passphrase is incorrect.
 * @return    Error code    : if the interface is not ready to transceive ethernet packets
 */
extern wwd_result_t wwd_wifi_is_ready_to_transceive( wwd_interface_t interface );

/** Gets the tx power in dBm units
 *
 * @param dbm : The variable to receive the tx power in dbm.
 *
 * @return  WWD_SUCCESS : if successful
 *          Error code   : if not successful
 */
extern wwd_result_t wwd_wifi_get_tx_power( uint8_t* dbm );

/** Sets the tx power in dBm units
 *
 * @param dbm : The desired tx power in dbm. If set to -1 (0xFF) the default value is restored.
 *
 * @return  WWD_SUCCESS : if tx power was successfully set
 *          Error code   : if tx power was not successfully set
 */
extern wwd_result_t wwd_wifi_set_tx_power( uint8_t dbm );

/** Sets the 802.11 powersave listen interval for a Wi-Fi client, and communicates
 *  the listen interval to the Access Point. The listen interval will be set to
 *  (listen_interval x time_unit) seconds.
 *
 *  The default value for the listen interval is 0. With the default value set,
 *  the Wi-Fi device wakes to listen for AP beacons every DTIM period.
 *
 *  If the DTIM listen interval is non-zero, the DTIM listen interval will over ride
 *  the beacon listen interval value.
 *
 *  If it is necessary to set the listen interval sent to the AP to a value other
 *  than the value set by this function, use the additional association listen
 *  interval API : wwd_wifi_set_listen_interval_assoc()
 *
 *  NOTE: This function applies to 802.11 powersave operation. Please read the
 *  WICED powersave application note for further information about the
 *  operation of the 802.11 listen interval.
 *
 * @param listen_interval : The desired beacon listen interval
 * @param time_unit       : The listen interval time unit; options are beacon period or DTIM period.
 *
 * @return  WWD_SUCCESS : If the listen interval was successfully set.
 *          Error code   : If the listen interval was not successfully set.
 */
extern wwd_result_t wwd_wifi_set_listen_interval( uint8_t listen_interval, wiced_listen_interval_time_unit_t time_unit );

/** Sets the 802.11 powersave beacon listen interval communicated to Wi-Fi Access Points
 *
 *  This function is used by Wi-Fi clients to set the value of the beacon
 *  listen interval sent to the AP (in the association request frame) during
 *  the association process.
 *
 *  To set the client listen interval as well, use the wwd_wifi_set_listen_interval() API
 *
 *  This function applies to 802.11 powersave operation. Please read the
 *  WICED powersave application note for further information about the
 *  operation of the 802.11 listen interval.
 *
 * @param listen_interval : The beacon listen interval sent to the AP during association.
 *                          The time unit is specified in multiples of beacon periods.
 *
 * @return  WWD_SUCCESS : if listen interval was successfully set
 *          Error code   : if listen interval was not successfully set
 */
extern wwd_result_t wwd_wifi_set_listen_interval_assoc( uint16_t listen_interval );

/** Gets the current value of all beacon listen interval variables
 *
 * @param listen_interval_beacon : The current value of the listen interval set as a multiple of the beacon period
 * @param listen_interval_dtim   : The current value of the listen interval set as a multiple of the DTIM period
 * @param listen_interval_assoc  : The current value of the listen interval sent to access points in an association request frame
 *
 * @return  WWD_SUCCESS : If all listen interval values are read successfully
 *          Error code   : If at least one of the listen interval values are NOT read successfully
 */
extern wwd_result_t wwd_wifi_get_listen_interval( wiced_listen_interval_t* li );

/** Registers interest in a multicast address
 * Once a multicast address has been registered, all packets detected on the
 * medium destined for that address are forwarded to the host.
 * Otherwise they are ignored.
 *
 * @param mac: Ethernet MAC address
 *
 * @return  WWD_SUCCESS : if the address was registered successfully
 *          Error code   : if the address was not registered
 */
extern wwd_result_t wwd_wifi_register_multicast_address( const wiced_mac_t* mac );

/** Registers interest in a multicast address
 * Similar to wwd_wifi_register_multicast_address but able to define interface
 *
 * @param mac      : Ethernet MAC address
 * @param interface: Wireless interface
 *
 * @return  WWD_SUCCESS : if the address was registered successfully
 *          Error code   : if the address was not registered
 */
extern wwd_result_t wwd_wifi_register_multicast_address_for_interface( const wiced_mac_t* mac, wwd_interface_t interface );

/** Unregisters interest in a multicast address
 * Once a multicast address has been unregistered, all packets detected on the
 * medium destined for that address are ignored.
 *
 * @param mac: Ethernet MAC address
 *
 * @return  WWD_SUCCESS : if the address was unregistered successfully
 *          Error code  : if the address was not unregistered
 */
extern wwd_result_t wwd_wifi_unregister_multicast_address( const wiced_mac_t* mac );

/** Unregisters interest in a multicast address
 * Similar to wwd_wifi_unregister_multicast_address but able to define interface.
 *
 * @param mac      : Ethernet MAC address
 * @param interface: Wireless interface
 *
 * @return  WWD_SUCCESS : if the address was unregistered successfully
 *          Error code   : if the address was not unregistered
 */
extern wwd_result_t wwd_wifi_unregister_multicast_address_for_interface( const wiced_mac_t* mac, wwd_interface_t interface );

/** Retrieve the latest RSSI value
 *
 * @param rssi: The location where the RSSI value will be stored
 *
 * @return  WWD_SUCCESS : if the RSSI was succesfully retrieved
 *          Error code  : if the RSSI was not retrieved
 */
extern wwd_result_t wwd_wifi_get_rssi( int32_t* rssi );

/** Retrieve the latest RSSI value of the AP client
 *
 * @param rssi: The location where the RSSI value will be stored
 * @param client_mac_addr: Mac address of the AP client
 *                         Please note that you can get the full list of AP clients
 *                         currently connected to Wiced AP by calling a function
 *                         wwd_wifi_get_associated_client_list
 *
 * @return  WWD_SUCCESS : if the RSSI was succesfully retrieved
 *          Error code   : if the RSSI was not retrieved
 */
extern wwd_result_t wwd_wifi_get_ap_client_rssi( int32_t* rssi, const wiced_mac_t* client_mac_addr );

/** Select the Wi-Fi antenna
 *    antenna = 0 -> select antenna 0
 *    antenna = 1 -> select antenna 1
 *    antenna = 3 -> enable auto antenna selection ie. automatic diversity
 *
 * @param antenna: The antenna configuration to use
 *
 * @return  WWD_SUCCESS : if the antenna selection was successfully set
 *          Error code   : if the antenna selection was not set
 */
extern wwd_result_t wwd_wifi_select_antenna( wiced_antenna_t antenna );

/** Given a specific channel, return Clear Channel Assesment (CCA) score for that channel.
 *
 * @param  channels: List of 20 Mhz channels to measure
 * @param  duration: How long to measure for each channel, in milliseconds.
 * @param  scores:   List of resulting scores.
 * @param  nchans:   Number of entries in channels and scores.
 *
 *
 * @return  WWD_SUCCESS : Success
 *          Error code   : Unable to perform measurement
 */
extern wwd_result_t wwd_wifi_get_cca_for_channel(uint32_t *channels, uint32_t duration, uint8_t *scores, uint32_t nchans);

/** Bring down the Wi-Fi core
 *
 *  WARNING / NOTE:
 *     This brings down the Wi-Fi core and existing network connections will be lost.
 *     Re-establish the network by calling wiced_wifi_up() and wiced_network_up().
 *     Refer those APIs for more details.
 *
 * @return  WWD_SUCCESS : if success
 *          Error code  : if fails
 */
extern wwd_result_t wwd_wifi_set_down( void );

/** Brings up the Wi-Fi core
 *
 * @return  WWD_SUCCESS : if success
 *          Error code  : if fails
 */
extern wwd_result_t wwd_wifi_set_up( void );

/** Print out additional information for SET operations
 * @param enable: try read back and print out values after they are SET in firmware if this is true
 * @return  WWD_SUCCESS : if success
 *          Error code  : if fails
 */
extern wwd_result_t wwd_wifi_set_fw_cmd_debug_mode( wiced_bool_t enable );

/** Manage the addition and removal of custom IEs
 *
 * @param interface    : interface on which the operation to be performed
 * @param action       : the action to take (add or remove IE)
 * @param oui          : the oui of the custom IE
 * @param subtype      : the IE sub-type
 * @param data         : a pointer to the buffer that hold the custom IE
 * @param length       : the length of the buffer pointed to by 'data'
 * @param which_packets: a mask of which packets this IE should be included in. See wiced_ie_packet_flag_t
 *
 * @return WWD_SUCCESS : if the custom IE action was successful
 *         Error code  : if the custom IE action failed
 */
extern wwd_result_t wwd_wifi_manage_custom_ie( wwd_interface_t interface, wiced_custom_ie_action_t action, /*@unique@*/ const uint8_t* oui, uint8_t subtype, const void* data, uint16_t length, uint16_t which_packets );

/** Set roam trigger level for all bands
 *
 * @param trigger_level : Trigger level in dBm. The Wi-Fi device will search for a new AP to connect to once the \n
 *                        signal from the AP (it is currently associated with) drops below the roam trigger level
 *
 * @return  WWD_SUCCESS : if the roam trigger was successfully set
 *          Error code  : if the roam trigger was not successfully set
 */
extern wwd_result_t wwd_wifi_set_roam_trigger( int32_t trigger_level );

/** Set roam trigger level for the specified band
 *
 * @param trigger_level : Trigger level in dBm. The Wi-Fi device will search for a new AP to connect to once the \n
 *                        signal from the AP (it is currently associated with) drops below the roam trigger level
 *
 * @param band : Modify the roam trigger for this band
 * @return  WWD_SUCCESS : if the roam trigger was successfully set
 *          Error code  : if the roam trigger was not successfully set
 */
extern wwd_result_t wwd_wifi_set_roam_trigger_per_band( int32_t trigger_level, wiced_802_11_band_t band );

/** Get roam trigger level for the 2.4 Gigahertz band
 *
 * @param trigger_level  : Trigger level in dBm. Pointer to store current roam trigger level value
 * @return  WWD_SUCCESS  : if the roam trigger was successfully get
 *          Error code   : if the roam trigger was not successfully get
 */
extern wwd_result_t wwd_wifi_get_roam_trigger( int32_t* trigger_level );

/** Get roam trigger level for the given band
 *
 * @param trigger_level  : Trigger level in dBm. Pointer to store current roam trigger level value
 * @param band : Get roam trigger for this band
 * @return  WWD_SUCCESS  : if the roam trigger was successfully get
 *          Error code   : if the roam trigger was not successfully get
 */
extern wwd_result_t wwd_wifi_get_roam_trigger_per_band( int32_t* trigger_level, wiced_802_11_band_t band );

/** Set roam trigger delta value for all bands
 *
 * @param trigger_delta : Trigger delta is in dBm. After a roaming is triggered - The successful roam will happen \n
 *                        when a target AP with RSSI better than the current serving AP by at least trigger_delta (in dB)
 *
 * @return  WWD_SUCCESS : if the roam trigger delta was successfully set
 *          Error code  : if the roam trigger delta was not successfully set
 */
extern wwd_result_t wwd_wifi_set_roam_delta( int32_t trigger_delta );

/** Set roam trigger delta value for given band
 *
 * @param trigger_delta : Trigger delta is in dBm. After a roaming is triggered - The successful roam will happen \n
 *                        when a target AP with RSSI better than the current serving AP by at least trigger_delta (in dB)
 * @param band : Get roam delta for this band
 * @return  WWD_SUCCESS : if the roam trigger delta was successfully set
 *          Error code  : if the roam trigger delta was not successfully set
 */
extern wwd_result_t wwd_wifi_set_roam_delta_per_band( int32_t trigger_delta, wiced_802_11_band_t band );

/** Get roam trigger delta value for 2.4 Gigahertz band
 *
 * @param trigger_delta : Trigger delta is in dBm. Pointer to store the current roam trigger delta value
 * @return  WWD_SUCCESS : if the roam trigger delta was successfully get
 *          Error code  : if the roam trigger delta was not successfully get
 */
extern wwd_result_t wwd_wifi_get_roam_delta( int32_t* trigger_delta );

/** Get roam trigger delta value for given band
 *
 * @param trigger_delta : Trigger delta is in dBm. Pointer to store the current roam trigger delta value
 * @param band : Get roam delta for this band
 * @return  WWD_SUCCESS : if the roam trigger delta was successfully get
 *          Error code  : if the roam trigger delta was not successfully get
 */
extern wwd_result_t wwd_wifi_get_roam_delta_per_band( int32_t* trigger_delta, wiced_802_11_band_t band );

/** Set roam scan period
 *
 * @param roam_scan_period : Roam scan period is in secs. Updates the partial scan period - for partial scan - Only for STA
 * @return  WWD_SUCCESS    : if the roam scan period was successfully set
 *          Error code     : if the roam scan period was not successfully set
 */
extern wwd_result_t wwd_wifi_set_roam_scan_period( uint32_t roam_scan_period );

/** Get roam scan period
 *
 * @param roam_scan_period : Roam scan period is in secs. Pointer to store the current partial scan period
 * @return  WWD_SUCCESS    : if the roam scan period was successfully get
 *          Error code     : if the roam scan period was not successfully get
 */
extern wwd_result_t wwd_wifi_get_roam_scan_period( uint32_t* roam_scan_period );

/** Turn off roaming
 *
 * @param disable         : Boolean value which if TRUE will turn roaming off and if FALSE will turn roaming on
 *
 * @return  WICED_SUCCESS : if the roaming was successfully turned off
 *          WICED_ERROR   : if the roaming was not successfully turned off
 */
extern wwd_result_t wwd_wifi_turn_off_roam( wiced_bool_t disable );

/** Send a pre-prepared action frame
 *
 * @param action_frame   : A pointer to a pre-prepared action frame structure
 * @param interface      : The interface that is sending the action frame (WWD_STA_INTERFACE, WWD_AP_INTERFACE or WWD_P2P_INTERFACE)
 *
 * @return WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_wifi_send_action_frame( const wiced_action_frame_t* action_frame, wwd_interface_t interface );

/**
 * Used by WICED to get notified that wireless link state has changed
 * The callback function any time STA, AP, or GO link states change
 */
extern void wwd_wifi_register_link_update_callback( void (*callback_function)( void ) );

/**
* Called to notify WWD that there has been a link change
* WWD in turn will call the link update callback, if it has been registered
*/
extern void wwd_wifi_link_update( void );

/**
 * Set whether the p2p GO is up or not
 * @param is_up   : specify whether the p2p GO is up currently or not
 */
extern void wwd_wifi_p2p_set_go_is_up( wiced_bool_t is_up );

/**
 * @return WICED_TRUE if the P2P GO is currently up
 */
extern wiced_bool_t wwd_wifi_p2p_is_go_up( void );

/** Retrieve the latest STA EDCF AC parameters
 *
 * Retrieve the latest Station (STA) interface EDCF (Enhanced Distributed
 * Coordination Function) Access Category parameters
 *
 * @param acp: The location where the array of AC parameters will be stored
 *
 * @return  WWD_SUCCESS : if the AC Parameters were successfully retrieved
 *          Error code   : if the AC Parameters were not retrieved
 */
extern wwd_result_t wwd_wifi_get_acparams_sta( wiced_edcf_ac_param_t *acp );

/** Prioritize access category parameters as a function of min and max contention windows and backoff slots
 *
 * @param acp:       Pointer to an array of AC parameters
 * @param priority:  Pointer to a matching array of priority values
 *
 * @return
 */
extern void wwd_wifi_prioritize_acparams( const wiced_edcf_ac_param_t *acp, int *priority );

/** For each traffic priority (0..7) look up the 802.11 Access Category that is mapped to this type
 *  of service and update the TOS map with the priority that the AP actually allows
 *
 * @return  WICED_SUCCESS : if the
 *          WICED_ERROR   : if the AC Parameters were not retrieved
 */
extern wwd_result_t wwd_wifi_update_tos_map( void );

/** Print access category parameters with their priority (1-4, where 4 is highest priority)
 *
 * @param acp:       Pointer to an array of AC parameters
 * @param priority:  Pointer to a matching array of priority values
 *
 * @return
 */
extern void wwd_wifi_edcf_ac_params_print( const wiced_edcf_ac_param_t *acp, const int *priority );


/** Get the current channel on the WLAN radio
 *
 * NOTE: on most WLAN devices this will get the channel for both AP *AND* STA
 *       (since there is only one radio - it cannot be on two channels simulaneously)
 *
 * @param interface : The interface to set
 * @param channel   : pointer which receives the current channel
 *
 * @return  WWD_SUCCESS : if the channel was successfully retrieved
 *          Error code   : if the channel was not successfully retrieved
 */
extern wwd_result_t wwd_wifi_get_channel( wwd_interface_t interface, uint32_t* channel );

/** Set the current channel on the WLAN radio
 *
 * NOTE: on most WLAN devices this will set the channel for both AP *AND* STA
 *       (since there is only one radio - it cannot be on two channels simulaneously)
 *
 * @param interface : The interface to set
 * @param channel   : The desired channel
 *
 * @return  WWD_SUCCESS : if the channel was successfully set
 *          Error code   : if the channel was not successfully set
 */
extern wwd_result_t wwd_wifi_set_channel( wwd_interface_t interface, uint32_t channel );

/** Get the channel list on the WLAN radio
 *
 * NOTE: on most WLAN devices this will get the channel list for both AP *AND* STA
 *
 * @param interface : The interface to set
 * @param channels   : pointer which receives the channel list
 *
 * @return  WWD_SUCCESS : if the channel was successfully retrieved
 *          Error code   : if the channel was not successfully retrieved
 */
extern wwd_result_t wwd_wifi_get_channels ( wwd_interface_t interface, wl_uint32_list_t* channels );

/** Get the counters for the provided interface
 *
 * @param interface  : The interface from which the counters are requested
 *        counters   : A pointer to the structure where the counter data will be written
 *
 * @return  WWD_SUCCESS : if the counters were successfully read
 *          Error code   : if the counters were not successfully read
 */
extern wwd_result_t wwd_wifi_get_counters( wwd_interface_t interface, wiced_counters_t* counters );

/** Get the maximum number of associations supported by all interfaces (STA and Soft AP)
 *
 * @param max_assoc  : The maximum number of associations supported by the STA and Soft AP interfaces. For example
 *                     if the STA interface is associated then the Soft AP can support (max_assoc - 1)
 *                     associated clients.
 *
 * @return  WICED_SUCCESS : if the maximum number of associated clients was successfully read
 *          WICED_ERROR   : if the maximum number of associated clients was not successfully read
 */
extern wwd_result_t wwd_wifi_get_max_associations( uint32_t* max_assoc );


/** Get the current data rate for the provided interface
 *
 * @param interface  : The interface from which the rate is requested
 *        rate   : A pointer to the uint32_t where the value will be returned in 500Kbits/s units, 0 for auto
 *
 * @return  WWD_SUCCESS : if the rate was successfully read
 *          Error code   : if the rate was not successfully read
 */
extern wwd_result_t wwd_wifi_get_rate( wwd_interface_t interface, uint32_t* rate);

/** Set the legacy (CCK/OFDM) transmit data rate for the provided interface
 *
 * @param interface  : The interface for which the rate is going to be set
 *        rate       : uint32_t where the rate value is given in 500Kbits/s units, 0 for auto
 *
 * @return  WWD_SUCCESS : if the rate was successfully set
 *          Error code   : if the rate was not successfully set
 */
extern wwd_result_t wwd_wifi_set_legacy_rate( wwd_interface_t interface, int32_t rate);

/** Set the MCS index transmit data rate for the provided interface
 *
 * @param interface  : The interface for which the rate is going to be set
 *        mcs        : int32_t where the mcs index is given, -1 for auto
 *        mcsonly    : indicate that only the mcs index should be changed
 *
 * @return  WWD_SUCCESS : if the rate was successfully set
 *          Error code   : if the rate was not successfully set
 */
extern wwd_result_t wwd_wifi_set_mcs_rate( wwd_interface_t interface, int32_t mcs, wiced_bool_t mcsonly);

/** Enable or disable 11n support (support only for pre-11n modes)
 *
 *  NOTE:
 *     Ensure Wi-Fi core and network is down before invoking this function.
 *     Refer to wiced_wifi_down() for more details.
 *
 * @param interface       : The interface for which 11n mode is being controlled. Currently only STA supported
 *        disable         : Boolean value which if TRUE will turn 11n off and if FALSE will turn 11n on
 *
 * @return  WICED_SUCCESS : if the 11n was successfully turned off
 *          WICED_ERROR   : if the 11n was not successfully turned off
 */
extern wwd_result_t wwd_wifi_set_11n_support( wwd_interface_t interface, wiced_11n_support_t value );

/** Set the AMPDU parameters for both Soft AP and STA
 *
 * Sets various AMPDU parameters for Soft AP and STA to ensure that the number of buffers dedicated to AMPDUs does
 * not exceed the resources of the chip. Both Soft AP and STA interfaces must be down.
 *
 * @return  WICED_SUCCESS : if the AMPDU parameters were successfully set
 *          WICED_ERROR   : if the AMPDU parameters were not successfully set
 */
extern wwd_result_t wwd_wifi_set_ampdu_parameters( void );

/** Set the AMPDU Block Ack window size for both Soft AP and STA
 *
 * Sets the AMPDU Block Ack window size for Soft AP and STA. Soft AP and STA interfaces may be up.
 *
 * @param interface  : STA or Soft AP interface.
 *
 * @return  WICED_SUCCESS : if the Block Ack window size was successfully set
 *          WICED_ERROR   : if the Block Ack window size was not successfully set
 */
extern wwd_result_t wwd_wifi_set_block_ack_window_size( wwd_interface_t interface );

/** Get the average PHY noise detected on the antenna. This is valid only after TX.
 *  Defined only on STA interface
 *
 * @param noise : reports average noise
 *
 * @return  WWD_SUCCESS : if success
 *          Error code   : if Link quality was not enabled or not successful
 */
extern wwd_result_t wwd_wifi_get_noise( int32_t *noise );

/** Get the bands supported by the radio chip
 *
 * @param band_list : pointer to a structure that will hold the band list information
 *
 * @return  WWD_SUCCESS : if success
 *          Error code  : if not successful
 */
extern wwd_result_t wwd_wifi_get_supported_band_list( wiced_band_list_t* band_list );

/** Set the preferred band for association by the radio chip
 *  Defined only on STA interface
 *
 * @param band : preferred band (auto, 2.4 GHz or 5 GHz)
 *
 * @return  WWD_SUCCESS : if success
 *          Error code   : if setting the preferred band was not successful
 */
wwd_result_t wwd_wifi_set_preferred_association_band( int32_t band );

/** Get the preferred band for association by the radio chip
 *
 * @param band : pointer to a variable that will hold the band information (auto, 2.4 GHz or 5 GHz)
 *
 * @return  WWD_SUCCESS : if success
 *          Error code  : if not successful
 */
wwd_result_t wwd_wifi_get_preferred_association_band( int32_t* band );

/** Sets HT mode for the given interface
 *
 *  NOTE:
 *     Ensure WiFi core and network is down before invoking this function.
 *     Refer wiced_wifi_down() API for more details.
 *
 * @param interface       : the interface for which HT mode to be changed.
 *        ht_mode         : enumeration value which indicates the HT mode
 *
 * @return  WICED_SUCCESS : if success
 *          Error code    : error code to indicate the type of error, if HT mode could not be successfully set
 */
extern wwd_result_t wwd_wifi_set_ht_mode( wwd_interface_t interface, wiced_ht_mode_t ht_mode );

/** Gets the current HT mode of the given interface
 *
 * @param interface       : the interface for which current HT mode to be identified
 *        ht_mode         : pointers to store the results (i.e., currently configured HT mode)
 *
 * @return  WICED_SUCCESS : if success
 *          Error code    : error code to indicate the type of error, if HT mode could not be successfully get
 */
extern wwd_result_t wwd_wifi_get_ht_mode( wwd_interface_t interface, wiced_ht_mode_t* ht_mode );

/** Gets the BSS index that the given interface is mapped to in Wiced
 *
 * @param interface       : the interface for which to get the BSS index
 *
 * @return  BSS index
 */
extern uint32_t     wwd_get_bss_index( wwd_interface_t interface );

/** Gets the current EAPOL key timeout for the given interface
 *
 * @param interface         : the interface for which we want the EAPOL key timeout
 *        eapol_key_timeout : pointer to store the EAPOL key timeout value
 *
 * @return  WICED_SUCCESS : if success
 *          Error code    : error code to indicate the type of error
 */
extern wwd_result_t wwd_wifi_get_supplicant_eapol_key_timeout( wwd_interface_t interface, int32_t* eapol_key_timeout );

/** Sets the current EAPOL key timeout for the given interface
 *
 * @param interface         : the interface for which we want to set the EAPOL key timeout
 *        eapol_key_timeout : EAPOL key timeout value
 *
 * @return  WICED_SUCCESS : if success
 *          Error code    : error code to indicate the type of error
 */
extern wwd_result_t wwd_wifi_set_supplicant_eapol_key_timeout( wwd_interface_t interface, int32_t eapol_key_timeout );

/*@+exportlocal@*/


/** Retrieves the WLAN firmware version
 *
 * @param[out] Pointer to a buffer that version information will be written to
 * @param[in]  Length of the buffer
 * @return     @ref wwd_result_t
 */
extern wwd_result_t wwd_wifi_get_wifi_version( char* version, uint8_t length );

/** Retrieves the WLAN CLM version
 *
 * @param[out] Pointer to a buffer that version information will be written to
 * @param[in]  Length of the buffer
 * @return     @ref wwd_result_t
 */
extern wwd_result_t wwd_wifi_get_clm_version( char* version, uint8_t length );

/** Retrieves current memory usage of WLAN processor
 *
 * @param[out] Pointer to a buffer that information will be written to
 * @param[in]  Length of the buffer
 * @return     @ref wwd_result_t
 */
extern wwd_result_t wwd_wifi_get_wifi_memuse( char* version, uint8_t length );

/**
 * This function gets the feature capabilities string from the WLAN firmware.
 *
 * @param char *         : pointer to string buffer
 * @param uint16_t       : length of string buffer
 * @param const char *   : Optional.  If nonnull string, return WWD_SUCCESS if cap is present. Failure otherwise. Allows for simple checking of a specific capability.
 *
 * @returns : status WWD_SUCCESS or failure
 */
extern wwd_result_t wwd_wifi_get_cap( char* buffer, uint16_t buflen, char *cap );


/** Set a custom WLAN country code
 *
 * @param[in] country_code: Country code information
 * @return     @ref wwd_result_t
 */
extern wwd_result_t wwd_wifi_set_custom_country_code( const wiced_country_info_t* country_code );

/**
 * This function will send a channel switch announcement and switch to the specificed channel at the specified time.
 * @param[in] wiced_chan_switch_t: pointer to channel switch information
 * @param interface              : WWD_AP_INTERFACE (works only in AP mode)
 * @return     @ref wwd_result_t
 */
extern wwd_result_t wwd_wifi_send_csa( const wiced_chan_switch_t* csa, wwd_interface_t interface );

extern wwd_result_t print_hex_bytes ( uint8_t* bytes, uint16_t length );


/** @} */

/* AP & STA info API */
extern wwd_result_t wwd_wifi_get_associated_client_list( void* client_list_buffer, uint16_t buffer_length );
extern wwd_result_t wwd_wifi_get_ap_info( wiced_bss_info_t* ap_info, wiced_security_t* security );
extern wwd_result_t wwd_wifi_get_bssid( wiced_mac_t* bssid );

/* Monitor Mode API */
extern wwd_result_t wwd_wifi_enable_monitor_mode     ( void );
extern wwd_result_t wwd_wifi_disable_monitor_mode    ( void );
extern wiced_bool_t wwd_wifi_monitor_mode_is_enabled ( void );
extern wwd_result_t wwd_wifi_set_raw_packet_processor( wwd_wifi_raw_packet_processor_t function );

/* Duty cycle control API */
extern wwd_result_t wwd_wifi_set_ofdm_dutycycle( uint8_t  duty_cycle_val );
extern wwd_result_t wwd_wifi_set_cck_dutycycle ( uint8_t  duty_cycle_val );
extern wwd_result_t wwd_wifi_get_ofdm_dutycycle( uint8_t* duty_cycle_val );
extern wwd_result_t wwd_wifi_get_cck_dutycycle ( uint8_t* duty_cycle_val );

/* PMK retrieval API */
extern wwd_result_t wwd_wifi_get_pmk( const char* psk, uint8_t psk_length, char* pmk );

/* Packet filter API */
extern wwd_result_t wwd_wifi_add_packet_filter                 ( const wiced_packet_filter_t* filter_settings );
extern wwd_result_t wwd_wifi_set_packet_filter_mode            ( wiced_packet_filter_mode_t mode );
extern wwd_result_t wwd_wifi_remove_packet_filter              ( uint8_t filter_id );
extern wwd_result_t wwd_wifi_enable_packet_filter              ( uint8_t filter_id );
extern wwd_result_t wwd_wifi_disable_packet_filter             ( uint8_t filter_id );
extern wwd_result_t wwd_wifi_get_packet_filter_stats           ( uint8_t filter_id, wiced_packet_filter_stats_t* stats );
extern wwd_result_t wwd_wifi_clear_packet_filter_stats         ( uint32_t filter_id );
extern wwd_result_t wwd_wifi_get_packet_filters                ( uint32_t max_count, uint32_t offset, wiced_packet_filter_t* list,  uint32_t* count_out );
extern wwd_result_t wwd_wifi_get_packet_filter_mask_and_pattern( uint32_t filter_id, uint32_t max_size, uint8_t* mask, uint8_t* pattern, uint32_t* size_out );

/* These functions are not exposed to the external WICED API */
extern wwd_result_t wwd_wifi_toggle_packet_filter( uint8_t filter_id, wiced_bool_t enable );

/* Network Keep Alive API */
extern wwd_result_t wwd_wifi_add_keep_alive    ( const wiced_keep_alive_packet_t* keep_alive_packet_info );
extern wwd_result_t wwd_wifi_get_keep_alive    ( wiced_keep_alive_packet_t* keep_alive_packet_info );
extern wwd_result_t wwd_wifi_disable_keep_alive( uint8_t id );
/** @endcond */


/* Enable or Disable feature that leaves driver firmware running but powers off radio when not in use. */
extern wwd_result_t wwd_wifi_enable_minimum_power_consumption( void );
extern wwd_result_t wwd_wifi_disable_minimum_power_consumption( void );
/*
 * APIs to write a bit/word to OTP at the specified bit/word offset. (An OTP word is 16 bits)
 * These APIs work only with the MFG WLAN FW and are not supported on the Production WLAN FW.
 */
extern wwd_result_t wwd_wifi_otp_write_bit( uint16_t bit_offset, uint16_t write_bit );
extern wwd_result_t wwd_wifi_otp_write_word( uint16_t word_offset, uint16_t write_word );

extern wwd_result_t wwd_wifi_test_credentials( wiced_scan_result_t* ap, const uint8_t* security_key, uint8_t key_length );


/* These functions are used for link down event handling while in power save mode */
extern uint8_t  wiced_wifi_get_powersave_mode( void );
extern uint16_t wiced_wifi_get_return_to_sleep_delay( void );

extern wwd_result_t wwd_wifi_read_wlan_log( char* buffer, uint32_t buffer_size );

extern wwd_result_t wwd_wifi_set_passphrase( const uint8_t* security_key, uint8_t key_length, wwd_interface_t interface );
extern wwd_result_t wwd_wifi_enable_sup_set_passphrase( const uint8_t* security_key_psk, uint8_t psk_length, wiced_security_t auth_type );
extern wwd_result_t wwd_wifi_sae_password( const uint8_t* security_key, uint8_t key_length, wwd_interface_t interface );

/* Set and get IOVAR values */
/* do a parameterless SET; for example for up or down*/
extern wwd_result_t wwd_wifi_set_iovar_void( const char* iovar, wwd_interface_t interface );
extern wwd_result_t wwd_wifi_set_iovar_value( const char* iovar, uint32_t  value, wwd_interface_t interface );
extern wwd_result_t wwd_wifi_get_iovar_value( const char* iovar, uint32_t* value, wwd_interface_t interface );
extern wwd_result_t wwd_wifi_get_iovar_buffer( const char* iovar_name, uint8_t* out_buffer, uint16_t out_length, wwd_interface_t interface );
extern wwd_result_t wwd_wifi_set_iovar_buffer( const char* iovar, void *buffer, uint16_t buffer_length, wwd_interface_t interface );
wwd_result_t wwd_wifi_set_iovar_buffers( const char* iovar, const void **in_buffers, const uint16_t* in_buffer_lengths, const uint8_t num_buffers, wwd_interface_t interface );
extern wwd_result_t wwd_wifi_get_iovar_buffer_with_param( const char* iovar_name, void *param, uint32_t paramlen, uint8_t* out_buffer, uint32_t out_length, wwd_interface_t interface );
extern wwd_result_t wwd_wifi_set_iovar_buffer_with_param( const char* iovar_name, void *param, uint32_t paramlen, uint8_t* out_buffer, uint32_t out_length, wwd_interface_t interface );

/* Set and get IOCTL values */
extern wwd_result_t wwd_wifi_set_ioctl_void( uint32_t ioctl, wwd_interface_t interface );
extern wwd_result_t wwd_wifi_set_ioctl_value( uint32_t ioctl, uint32_t  value, wwd_interface_t interface );
extern wwd_result_t wwd_wifi_get_ioctl_value( uint32_t ioctl, uint32_t* value, wwd_interface_t interface );
extern wwd_result_t wwd_wifi_set_ioctl_buffer( uint32_t ioctl, void *buffer, uint16_t buffer_length, wwd_interface_t interface );
extern wwd_result_t wwd_wifi_get_ioctl_buffer( uint32_t ioctl, uint8_t* out_buffer, uint16_t out_length, wwd_interface_t interface );

extern wwd_result_t wwd_wifi_get_revision_info( wwd_interface_t interface, wlc_rev_info_t *buf, uint16_t buflen );



/** @addtogroup wifi11k 802.11K (Radio Measurement) APIs
 *  @ingroup wifi
 *  WICED Wi-Fi functions for registering/de-registering Radio Resource Management
 *  event handlers
 *
 *  @{
 */

/** RRM report callback function pointer type
 * @param result_ptr  : A pointer to the pointer that indicates where to put the next RRM report
 *
 */
typedef void (*wiced_rrm_report_callback_t)( wwd_rrm_report_t** result_ptr );

/**
 *  This function gets Radio Resource Management Capabilities and parses them and
 *  then passes them to user application to format the data.
 *
 * @param interface                                     : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param radio_resource_management_capability_ie_t     : The data structure get the different Radio Resource capabilities.
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
extern wwd_result_t wwd_wifi_get_radio_resource_management_capabilities( wwd_interface_t interface, radio_resource_management_capability_ie_t* rrm_cap );

/**
 *
 *  This function sets Radio Resource Management Capabilities in the WLAN firmware.
 *
 * @param interface                                     : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param radio_resource_management_capability_ie_t     : The data structure to set the different Radio Resource capabilities.
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
extern wwd_result_t wwd_wifi_set_radio_resource_management_capabilities( wwd_interface_t interface, radio_resource_management_capability_ie_t* rrm_cap );


/**
 *
 *  This function send 11k neighbor report measurement request for the particular SSID in the WLAN firmware.
 *
 * @param interface       : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param wiced_ssid_t    : The data structure of the SSID.
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
extern wwd_result_t wwd_wifi_radio_resource_management_neighbor_req( wwd_interface_t interface, wiced_ssid_t* ssid );

/**
 *
 *  This function sets 11k link measurement request for the particular BSSID in the WLAN firmware.
 *
 * @param interface      : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param wiced_mac_t    : MAC Address of the destination
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
extern wwd_result_t wwd_wifi_radio_resource_management_link_management_req( wwd_interface_t interface, wiced_mac_t* ea );

/**
 *
 *  This function sets 11k beacon measurement request in the WLAN firmware.
 *
 * @param interface                                : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param radio_resource_management_beacon_req_t   : pointer to data structure of rrm_bcn_req_t
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
extern wwd_result_t wwd_wifi_radio_resource_management_beacon_req( wwd_interface_t interface, radio_resource_management_beacon_req_t* rrm_bcn_req );

/**
 *
 *  This function sets 11k channel load measurement request in the WLAN firmware.
 *
 * @param interface                       : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param radio_resource_management_req_t : pointer to data structure of rrm_chload_req
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
extern wwd_result_t wwd_wifi_radio_resource_management_channel_load_req( wwd_interface_t interface, radio_resource_management_req_t* rrm_chload_req );

/**
 *
 *  This function sets 11k noise measurement request in the WLAN firmware.
 *
 * @param interface                            : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param radio_resource_management_req_t      : pointer to data structure of rrm_noise_req
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
extern wwd_result_t wwd_wifi_radio_resource_management_noise_req( wwd_interface_t interface, radio_resource_management_req_t* rrm_noise_req );

/**
 *
 *  This function sets 11k frame measurement request in the WLAN firmware.
 *
 * @param interface                            : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param radio_resource_management_framereq_t : pointer to data structure of rrm_framereq
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
extern wwd_result_t wwd_wifi_radio_resource_management_frame_req( wwd_interface_t interface, radio_resource_management_framereq_t* rrm_framereq );

/**
 *
 *  This function sets 11k stat measurement request in the WLAN firmware.
 *
 * @param interface                            : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param radio_resource_management_statreq_t  : pointer to data structure of rrm_statreq
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
extern wwd_result_t wwd_wifi_radio_resource_management_stat_req( wwd_interface_t interface, radio_resource_management_statreq_t* rrm_statreq );

/**
 *
 *  This function gets 11k neighbor report list works from the WLAN firmware.
 *
 * @param interface      : WWD_AP_INTERFACE (works only in AP mode)
 * @param uint8_t        : buffer pointer to data structure
 * @param uint16_t       : buffer length
 *
 * @returns : status WWD_SUCCESS or failure
 */
extern wwd_result_t wwd_wifi_radio_resource_management_neighbor_list( wwd_interface_t interface, uint8_t* buffer, uint16_t buflen );

/**
 *
 *  This function deletes node from 11k neighbor report list
 *
 * @param interface      : WWD_AP_INTERFACE (works only in AP mode)
 * @param wiced_mac_t    : BSSID of the node to be deleted from neighbor report list
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
extern wwd_result_t wwd_wifi_radio_resource_management_neighbor_del_neighbor( wwd_interface_t interface, wiced_mac_t* bssid);

/**
 *
 *  This function adds a node to  Neighbor list
 *
 * @param interface           : WWD_AP_INTERFACE (works only in AP mode)
 * @param rrm_nbr_element_t   : pointer to the neighbor element data structure.
 * @param buflen              : buffer length of the neighbor element data.
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
extern wwd_result_t wwd_wifi_radio_resource_management_neighbor_add_neighbor( wwd_interface_t interface, radio_resource_management_nbr_element_t* nbr_elt, uint16_t buflen );

/** @} */

/** @addtogroup wifi11r 802.11R(Fast BSS Transition) APIs
 *  @ingroup wifi
 *  WICED Wi-Fi functions for registering/de-registering Radio Resource Management
 *  event handlers
 *
 *  @{
 */

/**
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
extern wwd_result_t wwd_wifi_fast_bss_transition_over_distribution_system( wwd_interface_t interface, wiced_bool_t set, int* value);

/**
 *
 *  This function  returns the value of WLFBT (1 if Driver 4-way handshake & reassoc (WLFBT)  is enabled 1 and 0 if disabled)
 *
 * @param interface           : WWD_STA_INTERFACE or WWD_AP_INTERFACE
 * @param value               : gets value of the FBT capabilities.
 *
 *
 * @returns : status WWD_SUCCESS or failure
 */
extern wwd_result_t wwd_wifi_fast_bss_transition_capabilities( wwd_interface_t interface, wiced_bool_t* enable );

/** @} */


/** NAN (Neighbor Area Networking) APIs */

/*****************************************************************************/
/** @addtogroup wifinan       Wi-Fi Neighborhood Area Networking
 *  @ingroup wifi
 *    WLAN NAN (neighborhood area networking)
 *    WICED Wi-Fi functions for NAN enable, disable and register/de-register of
 *  event handlers.
 *
 *  @{
 */
/****************************************************************************/

/**
 * Enables NAN (Neighbor Area Networking)
 * @param[in] none
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_config_enable ( void );

/** disable NAN services
  *
  * @param[in] none
  * @return @ref wwd_result_t
  */
extern wwd_result_t wwd_nan_config_disable( void );

/** set/get NAN device state
 *
 * @param[in] set/get  : 1 is to set and 0 is to get
 * @param[in/out] role : uint8_t role type
                         WL_NAN_ROLE_AUTO = 0,
                         WL_NAN_ROLE_NON_MASTER_NON_SYNC = 1,
                         WL_NAN_ROLE_NON_MASTER_SYNC = 2,
                         WL_NAN_ROLE_MASTER = 3,
                         WL_NAN_ROLE_ANCHOR_MASTER = 4
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_config_device_state( wiced_bool_t set, uint8_t* role );

/** set/get NAN hop count
 *
 * @param[in] set/get : 1 is to set and 0 is to get
 * @param[in/out]     : uint8_t hop count
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_config_hop_count( wiced_bool_t set, uint8_t* hop_count );

/** set/get NAN hop limit
 *
 * @param[in] set/get : 1 is to set and 0 is to get
 * @param[in/out]     : uint8_t hop limit
 *                      beacon is discarded if hop_count in the beacon is
 *                      larger than this implementation specific threshold (hop_limit)
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_config_hop_limit ( wiced_bool_t set, uint8_t* hop_limit );


/** set/get NAN warmup time
 *
 * @param[in] set/get : 1 is to set and 0 is to get
 * @param[in/out]     : uint32_t warmup_time
 *                      warm up time for nan start in unit of DW (Discovery Window)
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_config_warmup_time ( wiced_bool_t set, uint32_t *warmup_time );

/** set/get NAN RSSI threshold
 *
 * @param[in] set/get : 1 is to set and 0 is to get
 * @param[in/out]     : wwd_nan_config_rssi_threshold_t
 *                      uint8_t nan_band
 *                      int8 rssi_close
 *                      int8 rssi_mid
 *                      uint8 pad
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_config_rssi_threshold ( wiced_bool_t set, wwd_nan_config_rssi_threshold_t* rssi_thresh );

/** get NAN status
 *
 * @param[out]      : wwd_nan_state_t
 *                    uint8 enabled                            : NAN status enabled(1)/disabled(0)
 *                    uint8 inited                             : NAN status initialized or not
 *                    uint8 joined                             : NAN status joined to cluster or not
 *                    uint8 role                               : NAN role
 *                    uint32 chspec[2]                         : NAN channel specification
 *                    uint8 mr[8]                              : Master Rank
 *                    uint8 amr[8]                             : Anchor Master Rank
 *                    uint32 cnt_pend_txfrm                    : pending TX frames
 *                    wwd_nan_config_count_t nan_config_status : NAN TX/RX status
 *                    uint32 ambtt                             : Anchor master beacon target time
 *                    wwd_nan_config_params_t                  : NAN config parameters
 *
 * @return @ref wwd_result_t
 *
 */
extern wwd_result_t wwd_nan_config_get_status ( wwd_nan_state_t* nan_state );

/**
 * get/set NAN OUI
 *
 * @param[in] set/get : 1 is to set and 0 is to get
 * @param[in/out]     : wwd_nan_config_oui_type_t
 *                    : uint8 nan_oui[DOT11_OUI_LEN]
 *                    : uint8 type
 * @return @ref wwd_result_t
 *
 */
extern wwd_result_t wwd_nan_config_oui ( wiced_bool_t enable, wwd_nan_config_oui_type_t* oui_type );

/**
 * get NAN count
 *
 * @param[out]     : wwd_nan_config_count_t
 *                    uint32_t cnt_bcn_tx          : TX discovery/sync beacon count
 *                    uint32_t cnt_bcn_rx          : RX discovery/sync beacon count
 *                    uint32_t cnt_svc_disc_tx     : TX service discovery frame count
 *                    uint32_t cnt_svc_disc_rx     : RX service discovery frame count
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_config_get_count ( wwd_nan_config_count_t* config_count );

/**
 * Clear NAN counters
 *
 * @param[in] : none
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_config_clear_counters ( void );

/**
 * Configure NAN channel
 *
 * @param[in] : chanspec_t
 * @return @ref wwd_result_t
 *
 */
extern wwd_result_t wwd_nan_config_set_chanspec( chanspec_t* chanspec );

/**
 * Configure NAN band
 *
 * @param[in] : uint8_t band
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_config_band ( uint8_t band );

/**
 * set/get cluster id
 *
 * @param[in] set/get : 1 is to set and 0 is to get
 * @param[in/out]     : wwd_nan_cluster_id_t
 *                      uint8_t ether_addr[6] :  48-bit Ethernet address
 * @return @ref wwd_result_t
 *
 */
extern wwd_result_t wwd_nan_config_cluster_id ( wiced_bool_t set, wwd_nan_cluster_id_t* ether_addr );

/**
 * set/get interface address
 *
 * @param[in] set/get : 1 is to set and 0 is get
 * @param[in/out]     : ether_addr
 *                      uint8_t ether_addr[6] : 48 bit I/F address
 * @return @ref wwd_result_t
 *
 */
extern wwd_result_t wwd_nan_config_interface_address ( wiced_bool_t set, struct ether_addr* addr );

/**
 * set/get NAN discovery beacon interval
 *
 * @param[in] set/get : 1 is to set and 0 is get
 * @param[in/out]     : uint16_t
 *                      NAN discovery beacon interval
 *
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_config_discovery_beacon_interval ( wiced_bool_t set, uint16_t* interval );

/**
 * set/get NAN service discovery frame Transmit Time
 *
 * @param[in] set/get : 1 is to set and 0 is get
 * @param[in/out]     : uint16_t
 *                      NAN service discovery frame Transmit Time
 *
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_config_service_discovery_frame_tx_time ( wiced_bool_t set, uint16_t* time );

/**
 * set/get NAN STOP beacon transmit
 *
 * @param[in] set/get : 1 is to set and 0 is get
 * @param[in/out]     : uint16_t
 *                      stop beacon transmit for a given band
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_config_stop_beacon_transmit ( wiced_bool_t set, uint16_t* stop_beacon );

/**
 * set/get NAN  Service ID beacon
 *
 * @param[in] set/get : 1 is to set and 0 is get
 * @param[in/out]     : wwd_nan_sid_beacon_control_t
 *                      uint8 sid_enable  :   Flag to indicate the inclusion of Service IDs in Beacons
 *                      uint8 sid_count   : Limit for number of Service IDs to be included in Beacons
 *                      uint8 pad[2]
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_config_service_id_beacon ( wiced_bool_t set, wwd_nan_sid_beacon_control_t* service_id_bcn_control );

/**
 * set/get NAN Discovery Window length
 *
 * @param[in] set/get : 1 is to set and 0 is get
 * @param[in/out]     : uint16_t
 *                      NAN Discovery Window length
 * @return @ref wwd_result_t
 *
 */
extern wwd_result_t wwd_nan_config_discover_window_length ( wiced_bool_t set, uint16_t* dw_len );

/**
 * set/get NAN  host enable
 *
 * @param[in] set/get : 1 is to set and 0 is get
 * @param[in/out]     : uint8_t
 *                      host_enable command for nan election join, merge and stop.
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_election_host_enable ( wiced_bool_t set, uint8_t* host_enable );

/**
 * set NAN election metrics configuration
 *
 * @param[in] : wwd_nan_election_metric_config_t
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_election_metric_config ( wwd_nan_election_metric_config_t* config );

/**
 * get NAN election metrics state
 *
 * @param[out] : wwd_nan_election_metric_config_t
 *               uint8 random_factor : configured random factor
 *               uint8 master_pref   : configured master preference
 *               uint8 pad[2]
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_election_metric_state_get ( wwd_nan_election_metric_config_t* config );

/**
 * NAN election join
 *
 * @param[in] : wwd_nan_join_t
 *              uint8 start_cluster             : Start a cluster
 *              uint8 pad[3]
 *              wwd_nan_cluster_id_t cluster_id : Cluster ID to join
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_election_join ( wwd_nan_join_t* join );

/**
 * NAN election merge get/set
 *
 * @param[in] set/get : 1 is to set and 0 is get
 * @param[in/out]     : uint8_t
 *                    : enable_merge (i.e enable cluster merge if set)
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_election_merge ( wiced_bool_t set, uint8_t* enable_merge);


/**
 * NAN command election stop
 *
 * @param[in] : wwd_nan_cluster_id_t
 *            : struct ether_addr cluster_id (stop participating in the cluster id)
 *
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_election_stop ( wwd_nan_cluster_id_t* cluster_id );

/**
 * NAN set  timeslot reserve
 *
 * @param[in] : wwd_nan_timeslot_t
 *            uint32  abitmap  : available bitmap
 *            uint32  chanlist[NAN_MAX_TIMESLOT]
 *
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_sync_timeslot_reserve ( wwd_nan_timeslot_t* reserve );


/**
 * NAN sync timeslot release
 *
 * @param[in] : uint32_t
 *              NAN timeslot bitmap to release
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_sync_timeslot_release( uint32_t* release );

/**
 * NAN Service Discovery Publish
 *
 * @param[in] : 1 is to set and 0 is get
 * @param[in] : wwd_nan_sd_publish_t
 *               uint8_t instance_id : if get then Instance ID of an active publish instance *
 *               uint16  length : length including options
 *               uint16  flags  : bitmap representing aforesaid optional flags
 *               uint8   svc_hash[WL_NAN_SVC_HASH_LEN]: Hash for the service name
 *               uint8   instance_id    : Instance of the current service
 *               int8    proximity_rssi : RSSI limit to RX subscribe or publish SDF 0 no effect
 *               uint8   period         : period of the unsolicited SDF transmission in DWs
 *               int32   ttl            : TTL for this instance id, -1 will run till cancelled
 *               tlv_t   optional[1]    :optional fields in the SDF  as appropriate
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_sd_publish ( wiced_bool_t set, wwd_nan_sd_publish_t* nan_sd_params );

/**
 * Get NAN Service Discovery publish list
 *
 *
 * @param[in] :   wwd_nan_service_list_t
 *                 uint16_t id_count                           : Number of registered publish/subscribe services
 *                 wwd_nan_service_info_t list[1]              : service info defined by nan_service instance
 *                   uint8_t instance_id                       : Publish instance ID
 *                   uint8_t service_hash[WL_NAN_SVC_HASH_LEN] : Hash for service name
 *
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_sd_publish_list ( wwd_nan_service_list_t* nan_service_list );

/**
 * NAN Service Discovery cancel publish
 *
 * @param[in] : uint8_t instance_id
 *
 * @return @ref wwd_result_t
 *
 */
extern wwd_result_t wwd_nan_sd_cancel_publish ( uint8_t instance_id );

/**
 * NAN Service Discovery Subscribe
 *
 * @param[in] : 1 is to set and 0 is get
 * @param[in] : wwd_nan_sd_subscribe_t
 *               uint8_t instance_id :  if get then Instance ID of an active subscribe instance
 *               uint16  length : length including options
 *               uint16  flags  : bitmap representing aforesaid optional flags
 *               uint8   svc_hash[WL_NAN_SVC_HASH_LEN]: Hash for the service name
 *               uint8   instance_id    : Instance of the current service
 *               int8    proximity_rssi : RSSI limit to RX subscribe or publish SDF 0 no effect
 *               uint8   period         : period of the unsolicited SDF transmission in DWs
 *               int32   ttl            : TTL for this instance id, -1 will run till cancelled
 *               tlv_t   optional[1]    :optional fields in the SDF  as appropriate
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_sd_subscribe ( wiced_bool_t set, wwd_nan_sd_subscribe_t* nan_sd_params );

/**
 * Get NAN Service Discovery subscribe list
 *
 *
 * @param[in] :   wwd_nan_service_list_t
 *                 uint16_t id_count                           : Number of registered publish/subscribe services
 *                 wwd_nan_service_info_t list[1]              : service info defined by nan_service instance
 *                   uint8_t instance_id                       : Subscribe instance ID
 *                   uint8_t service_hash[WL_NAN_SVC_HASH_LEN] : Hash for service name
 *
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_sd_subscribe_list ( wwd_nan_service_list_t* nan_service_list );

/**
 * NAN Service Discovery cancel subscribe
 *
 * @param[in] : uint8_t instance_id
 *
 * @return @ref wwd_result_t
 *
 */
extern wwd_result_t wwd_nan_sd_cancel_subscribe ( uint8_t instance_id );


/**
 * NAN Service Discovery Transmit
 *
 * @param[in] : wwd_nan_sd_transmit_t
 *               uint8_t local_service_id               : Sender Service ID
 *               uint8_t requestor_service_id           : Destination Service ID
 *               struct ether_addr destination_addr     : Destination MAC
 *               uint16_t token                         : follow_up_token when a follow-up message is queued successfully
 *               uint8_t priority                       : requested relative priority
 *               uint8_t service_info_len               : size in bytes of the service info payload
 *               wwd_nan_service_info_t service_info[1] : Service Info payload
 * @return @ref wwd_result_t
 */
extern wwd_result_t wwd_nan_sd_transmit ( wwd_nan_sd_transmit_t* nan_sd_transmit );

extern wwd_result_t wwd_xtlv_batch_cmd_buffer (wiced_buffer_t* buffer, bcm_iov_batch_buf_t** xtlv_iov_buf, uint8_t** piov_buf, const char* iovar );
extern wwd_result_t wwd_pack_xtlv ( uint8_t** buffer, uint16_t cmd_id, uint16_t iovar_data_length );
extern wwd_result_t wwd_unpack_xtlv( wiced_buffer_t* buffer, bcm_iov_batch_subcmd_t** data, uint16_t iovar_data_len );

extern wwd_result_t wwd_xtlv_get_set_data( wiced_bool_t enable, uint16_t cmd_id, void* data, uint16_t data_len );

/** @} */


/*****************************************************************************/
/** @addtogroup wifiutilities   WiFi Utility Functions
 *  @ingroup wifi
 *
 *  @{
 */
/*****************************************************************************/

/** Map channel to its band, comparing channel to max 2g channel
 *
 * @param channel     : The channel to map to a band
 * @return                  : WL_CHANSPEC_BAND_2G or WL_CHANSPEC_BAND_5G
 */
extern wl_chanspec_t wwd_channel_to_wl_band( uint32_t channel );

/**
 ******************************************************************************
 * Prints partial details of a scan result on a single line
 * @param[in] record :  A pointer to the wiced_scan_result_t record
 *
 */
extern void print_scan_result( wiced_scan_result_t* record );

/**
 ******************************************************************************
 * Convert a security bitmap to string
 * @param[in] security :  security of type wiced_security_t
 * @param[in] out_str :  a character array to store output
 * @param[in] out_str_len :  length of out_str char array
 *
 */
extern void convert_security_type_to_string( wiced_security_t security, char* out_str, uint16_t out_str_len);

/**
 ******************************************************************************
 * Set current country code to ccode
 * @param[in] ccode     : Pointer to input ccode to be set
 * @return     @ref wwd_result_t
 *
 */
extern wwd_result_t wwd_wifi_set_ccode( wwd_country_t* ccode );

/**
 ******************************************************************************
 * Get current country code
 * @param[out] ccode    : Pointer to output current ccode
 * @return     @ref wwd_result_t
 */
extern wwd_result_t wwd_wifi_get_ccode( wwd_country_t* ccode );


/**
 ******************************************************************************
 * Resets WiFi driver statistic counters
 * @return     WWD_SUCCESS or Error code
 */

wwd_result_t wwd_reset_statistics_counters( void );

/**
 ******************************************************************************
 * Starts or stops the WiFi driver Phyrate logging facility.
 * @param[in] a mode selector where 0 = stop, 1 = start TX, 2= start RX
 * @return     WWD_SUCCESS or Error code
 */

wwd_result_t wwd_phyrate_log( unsigned int mode );

/**
 ******************************************************************************
 * Returns the WiFi driver phyrate statistics sinc the last reset.
 * @param[in] a pointer to the phyrate counts
 * @param[in] size of the phyrate counts buffer
 * @return     WWD_SUCCESS or Error code
 */

wwd_result_t wwd_get_phyrate_statistics_counters( wiced_phyrate_counters_t *counts_buffer, unsigned int size);

/**
 ******************************************************************************
 * Returns the WiFi driver phyrate log size since the last reset
 * @param[out] size of the phyrate counts buffer
 * @return     WWD_SUCCESS or Error code
 */

wwd_result_t wwd_get_phyrate_log_size( unsigned int *size);

/**
 ******************************************************************************
 * Returns the WiFi driver phyrate log since the last reset.
 * @param[in] a pointer to the phyrate counts buffer to fill
 * @return     WWD_SUCCESS or Error code
 */
wwd_result_t wwd_get_phyrate_log( wiced_phyrate_log_t *data);

/**
 ******************************************************************************
 * Returns the WiFi driver statistics counters since the last reset.
 * @param[in] a pointer to the counter statistics buffer to fill
 * @return     WWD_SUCCESS or Error code
 */
wwd_result_t wwd_get_counters( wiced_counters_t *data);

/**
 ******************************************************************************
 * Print out an event's information for debugging help
*/
void wwd_log_event( const wwd_event_header_t* event_header, const uint8_t* event_data );

/** @} */

/**
 ******************************************************************************
 * Preferred Network Offload functions (pno)
 * @addtogroup wifipno
 * @ingroup wifi
 * Wiced Wi-Fi Driver (WWD) functions for WLAN preferred Network Offload
 *
 * @{
 */

/**
 * Add another preferred network to be searched for in the background.
 * Adds are cumulative and can be called one after another.
 * @param[in] ssid: ssid of the network
 * @param[in] security: security settings for the preferred network
 * @return       WWD_SUCCESS or error; pno will always be left in a stopped state after calling this API;
 *                    use wwd_wifi_pno_start to get pno process started again.
 */
wwd_result_t wwd_wifi_pno_add_network( wiced_ssid_t *ssid, wiced_security_t security );

/**
 * clear added networks and disable pno scanning
 * @param[in] void: No parameters needed.
 * @return     WWD_SUCCESS or Error code
 */
wwd_result_t wwd_wifi_pno_clear( void );

/**
 * enable pno scan process now; use previously added networks
 * @param[in] void: No parameters needed.
 * @return     WWD_SUCCESS or Error code
 */
wwd_result_t wwd_wifi_pno_start( void );

/**
 * disable pno scan process now; do not clear previously added networks
 * @param[in] void: No parameters needed.
 * @return     WWD_SUCCESS or Error code
 */
wwd_result_t wwd_wifi_pno_stop( void );

/** @} */


/**
 ******************************************************************************
 * @addtogroup wifimesh Wi-Fi MESH Networking Functions
 * @ingroup wifi
 *
 * Wiced Wi-Fi Driver (WWD) functions for WLAN MESH
 *
 * @{
 */

/**
 * Set various mesh related flags on an interface
 * @param[in]  wifi_flags  : Set various flags
 * @param[in]  interface   : WICED_STA_INTERFACE for mesh network.
 * @return     WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_wifi_set_flags( uint32_t *wifi_flags, wwd_interface_t interface );

/**
 * Retrieve various mesh related flags on an interface
 * @param[in/out]  wifi_flags  : Retrieve various flags
 * @param[in]  interface   : WICED_STA_INTERFACE for mesh network.
 * @return     WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_wifi_get_flags( uint32_t *wifi_flags, wwd_interface_t interface );

/**
 * Query whether mesh networking is currently enabled.
 * @return     @ref wiced_bool_t
 */
extern wiced_bool_t wwd_wifi_is_mesh_enabled(void);

/**
 * Query whether mesh multicast rebroadcast is currently enabled.
 * @return     @ref wiced_bool_t
 */
extern wiced_bool_t wwd_wifi_is_mesh_mcast_rebroadcast_enabled(void);

/**
 * set channel for mesh network operation
 * @param[in]  channel     : Channel to operate on.
 * @param[in]  interface   : Which interface to use.  Use WICED_STA_INTERFACE for mesh network, WICED_AP_INTERFACE for Gateway interface.
 * @return     WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_set_mesh_channel( uint32_t channel, wwd_interface_t interface );

/**
 * enable/disable mesh auth proto
 * @param[in]  auth_proto  : Mesh security mode to operate.
 * @param[in]  interface   : Which interface to use.  Use WICED_STA_INTERFACE for mesh network, WICED_AP_INTERFACE for Gateway interface.
 */
extern wwd_result_t wwd_set_mesh_auth_proto( uint32_t auth_proto, wwd_interface_t interface );

/**
 * enable/disable mesh security
 * @param[in] auth_type  : Authentication type:
 *                         - WICED_SECURITY_OPEN           - Open Security
 *                         - WICED_SECURITY_WPA2_AES_PSK   - WPA2 Security using AES cipher
 * @param[in]  interface   : Which interface to use.  Use WICED_STA_INTERFACE for mesh network, WICED_AP_INTERFACE for Gateway interface.
 */
extern wwd_result_t wwd_set_mesh_security(wiced_security_t auth_type, wwd_interface_t interface );

/**
 * enable/disable mesh auto peering
 * @param[in]  auto_peer   : 0 (disable) or 1 (enable) mesh auto peering.
 * @param[in]  interface   : WICED_STA_INTERFACE for mesh network.
 * @return     WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_set_mesh_auto_peer( uint32_t auto_peer, wwd_interface_t interface );

/**
 * enable/disable mesh mcast rebroadcast
 * @param[in]  mcast_rebro : 0 (disable) or 1 (enable) mesh multicast rebroadcasting.
 * @param[in]  interface   : WICED_STA_INTERFACE for mesh network.
 * @return     WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_set_mesh_mcast_rebroadcast( uint32_t mcast_rebro, wwd_interface_t interface );

/**
 * Get mesh status
 * @param[in]  result_buf    : pointer to buffer to store status info.
 * @param[in]  result_buf_sz : size of buffer
 * @return     WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_mesh_status( char *result_buf, uint16_t result_buf_sz );

/**
 * join specified mesh network
 * @param[in]  ssid      : Name of Mesh network to join
 * @param[in] auth_type  : Authentication type:
 *                         - WICED_SECURITY_OPEN           - Open Security
 *                         - WICED_SECURITY_WPA2_AES_PSK   - WPA2 Security using AES cipher
 * @param[in] security_key : A byte array containing either the cleartext security key for WPA/WPA2
 *                           secured networks, or a pointer to an array of wiced_wep_key_t structures
 *                           for WEP secured networks
 * @param[in] key_length  : The length of the security_key in bytes.
 * @param[in]  interface : WICED_STA_INTERFACE should be used.
 * @return     WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_join_mesh( const wiced_ssid_t* ssid, wiced_security_t auth_type,
        const uint8_t* security_key, uint8_t key_length, wwd_interface_t interface );

/**
 * set mesh filter for specified MAC address to skip peering
 * @param[in]  mac       : MAC address of node to be filtered/blocked.
 * @param[in]  interface : WICED_STA_INTERFACE should be used.
 * @return     WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_mesh_filter( wiced_mac_t *mac, wwd_interface_t interface );
/** @} */

/**
 * To simulate "wl dump" command
 *
 * @param[out]  dump_buf : Output buffer from the dump command.
 * @param[in]    interface : Current Interface, ex:- STA, AP etc
 *                      len   : length of the output buffer
 *                      cmd : additional commands for the dump, ex:- ampdu
 *                      cmd_len : length of the command
 * @return @ref wwd_result_t
 */
wwd_result_t wwd_get_dump( wwd_interface_t interface, uint8_t *dump_buf, uint16_t len, char *cmd, uint16_t cmd_len );

/**
 * To simulate the "wl ampdu_clear_dump" command
 */
wwd_result_t wwd_ampdu_clear_dump( wwd_interface_t interface );

/**
 * To get the current band
 */
wwd_result_t wwd_wifi_get_current_band( wwd_interface_t interface, uint32_t *current_band );

/**
 * To get the bandwidth
 */
wwd_result_t wwd_wifi_get_bw( uint32_t *bandwidth );

/**
 * To get Wi-Fi PM mode
 */
wwd_result_t wwd_wifi_get_pm_mode( wwd_interface_t interface, uint32_t* pmmode );

/**
 * To get tx/rx frag counters data
 */
wwd_result_t wwd_get_txrx_frag( uint32_t *txfrag, uint32_t *rxfrag );

/*!
 ******************************************************************************
 * Set bandwidth
 *
 * @param[in] arg  : bandwidth 20/40/80
 *
 *
 * @result @ref wwd_result indicating if the operation success.
 */
wwd_result_t wwd_wifi_set_bandwidth( uint8_t bandwidth );

/*!
 ******************************************************************************
 * get bandwidth
 *
 *
 * @return uint32_t : bandwidth
 */
uint32_t wwd_wifi_get_bandwidth ( void );

/*@+exportlocal@*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* ifndef INCLUDED_WWD_WIFI_H */


