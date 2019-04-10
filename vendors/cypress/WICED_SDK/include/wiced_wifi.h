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
 * @file
 *  Defines functions to perform Wi-Fi operations
 */

#pragma once

#include "wiced_utilities.h"
#include "wwd_wifi.h"
#include "wwd_debug.h"
#include "wiced_rtos.h"
#include "wiced_tcpip.h"
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/** @defgroup wifi    Wi-Fi (802.11) functions
 *
 *  WICED WiFi functions
 */
/*****************************************************************************/



/******************************************************
 *                    Macros
 ******************************************************/

/**
 * Macro to determine the band (2.4GHz, 5GHz) of a channel
 */
#define WICED_WIFI_CH_TO_BAND( channel ) ( ( wwd_channel_to_wl_band( channel ) == WL_CHANSPEC_BAND_2G ) ? WICED_802_11_BAND_2_4GHZ : WICED_802_11_BAND_5GHZ )
#define WICED_WIFI_ULP_MIN_MILLISECONDS 5000 /* Min time between calling ds1 enter and going into ds1 */

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/**
 * WPS Connection Mode
 */
typedef enum
{
    WICED_WPS_PBC_MODE = 1,  /**< Push button mode */
    WICED_WPS_PIN_MODE = 2   /**< PIN mode         */
} wiced_wps_mode_t;

/**
 * WPS Device Category from the WSC2.0 spec
 */
typedef enum
{
    WICED_WPS_DEVICE_COMPUTER               = 1,     /**< COMPUTER               */
    WICED_WPS_DEVICE_INPUT                  = 2,     /**< INPUT                  */
    WICED_WPS_DEVICE_PRINT_SCAN_FAX_COPY    = 3,     /**< PRINT_SCAN_FAX_COPY    */
    WICED_WPS_DEVICE_CAMERA                 = 4,     /**< CAMERA                 */
    WICED_WPS_DEVICE_STORAGE                = 5,     /**< STORAGE                */
    WICED_WPS_DEVICE_NETWORK_INFRASTRUCTURE = 6,     /**< NETWORK_INFRASTRUCTURE */
    WICED_WPS_DEVICE_DISPLAY                = 7,     /**< DISPLAY                */
    WICED_WPS_DEVICE_MULTIMEDIA             = 8,     /**< MULTIMEDIA             */
    WICED_WPS_DEVICE_GAMING                 = 9,     /**< GAMING                 */
    WICED_WPS_DEVICE_TELEPHONE              = 10,    /**< TELEPHONE              */
    WICED_WPS_DEVICE_AUDIO                  = 11,    /**< AUDIO                  */
    WICED_WPS_DEVICE_OTHER                  = 0xFF,  /**< OTHER                  */
} wiced_wps_device_category_t;

/**
 * WPS Configuration Methods from the WSC2.0 spec
 */
typedef enum
{
    WPS_CONFIG_USBA                  = 0x0001,  /**< USBA                 */
    WPS_CONFIG_ETHERNET              = 0x0002,  /**< ETHERNET             */
    WPS_CONFIG_LABEL                 = 0x0004,  /**< LABEL                */
    WPS_CONFIG_DISPLAY               = 0x0008,  /**< DISPLAY              */
    WPS_CONFIG_EXTERNAL_NFC_TOKEN    = 0x0010,  /**< EXTERNAL_NFC_TOKEN   */
    WPS_CONFIG_INTEGRATED_NFC_TOKEN  = 0x0020,  /**< INTEGRATED_NFC_TOKEN */
    WPS_CONFIG_NFC_INTERFACE         = 0x0040,  /**< NFC_INTERFACE        */
    WPS_CONFIG_PUSH_BUTTON           = 0x0080,  /**< PUSH_BUTTON          */
    WPS_CONFIG_KEYPAD                = 0x0100,  /**< KEYPAD               */
    WPS_CONFIG_VIRTUAL_PUSH_BUTTON   = 0x0280,  /**< VIRTUAL_PUSH_BUTTON  */
    WPS_CONFIG_PHYSICAL_PUSH_BUTTON  = 0x0480,  /**< PHYSICAL_PUSH_BUTTON */
    WPS_CONFIG_VIRTUAL_DISPLAY_PIN   = 0x2008,  /**< VIRTUAL_DISPLAY_PIN  */
    WPS_CONFIG_PHYSICAL_DISPLAY_PIN  = 0x4008   /**< PHYSICAL_DISPLAY_PIN */
} wiced_wps_configuration_method_t;

/**
 * WICED SoftAP events
 */
typedef enum
{
    WICED_AP_UNKNOWN_EVENT,         /**< Unknown SoftAP event       */
    WICED_AP_STA_JOINED_EVENT,      /**< a STA joined our SoftAP    */
    WICED_AP_STA_LEAVE_EVENT,       /**< a STA left our SoftAP      */
} wiced_wifi_softap_event_t;

/**
 * Misc WiFi flags stored in DCT
 * structure
 *      platform_dct_misc_config_t
 *    field
 *      wifi_flags
 */
#define WIFI_FLAG_MESH                      (1 << 0)  /**< MESH master enable/disable */
#define WIFI_FLAG_MESH_MCAST_REBROADCAST    (1 << 1)  /**< MESH mcast rebroadcast */
#define WIFI_MESH_DHCP_IP                   (1 << 4)
#define WIFI_FLAG_MESH_GATEWAY              (1 << 5)  /**< This machine a Gateway with an additional STA interface (to connect to AP) */

#define WIFI_FLAG_MESH_ALL_FLAGS            (WIFI_FLAG_MESH | WIFI_FLAG_MESH_MCAST_REBROADCAST | WIFI_MESH_DHCP_IP | WIFI_FLAG_MESH_GATEWAY)


/******************************************************
 *                 Type Definitions
 ******************************************************/

/**
 * Soft AP event handler
 *
 * @param[in] event         : SoftAP event that caused this callback
 * @param[in] mac_address   : Mac address of STA that caused this event
 *
 */
typedef void (*wiced_wifi_softap_event_handler_t)( wiced_wifi_softap_event_t event, const wiced_mac_t* mac_address );

/**
 * Event handler to get NAN event header and NAN event data
 * @param[out]  const void* event_header :  event header
 * @param[out]  const uint8_t* event_data: event_data
 *
 */
typedef void (*wiced_wifi_nan_event_handler_t)( const void* event_header, const uint8_t* event_data );

/**
* Event handler to get RRM event header and RRM event data
 * @param[out]  const void* event_header :  event header
 * @param[out]  const uint8_t* event_data: event_data
 *
 */
typedef void (*wiced_wifi_rrm_event_handler_t)( const void* event_header, const uint8_t* event_data );


/******************************************************
 *                    Structures
 ******************************************************/

/**
 * Wi-Fi scan result
 */
typedef struct
{
    wiced_scan_result_t ap_details;   /**< Access point details */
    wiced_scan_status_t status;       /**< status               */
    void*               user_data;    /**< Pointer to user data passed into wiced_wifi_scan_networks() function */
    void*               next;         /**< Pointer to next scan result */
} wiced_scan_handler_result_t;

/** @cond !ADDTHIS*/
/* Note: Do NOT modify this type definition.
 * Internal code make assumptions based on it's current definition
 * */
typedef wiced_result_t (*wiced_scan_result_handler_t)( wiced_scan_handler_result_t* malloced_scan_result );

/** @endcond */


/**
 * WPS Device category holds WSC2.0 device category information
 */
typedef struct
{
    const wiced_wps_device_category_t device_category; /**< Device category                */
    const uint16_t sub_category;                       /**< Device sub-category            */
    const char*    device_name;                        /**< Device name                    */
    const char*    manufacturer;                       /**< Manufacturer details           */
    const char*    model_name;                         /**< Model name                     */
    const char*    model_number;                       /**< Model number                   */
    const char*    serial_number;                      /**< Serial number                  */
    const uint32_t config_methods;                     /**< Configuration methods          */
    const uint32_t os_version;                         /**< Operating system version       */
    const uint16_t authentication_type_flags;          /**< Supported authentication types */
    const uint16_t encryption_type_flags;              /**< Supported encryption types     */
    const uint8_t  add_config_methods_to_probe_resp;   /**< Add configuration methods to probe response for Windows enrollees (this is non-WPS 2.0 compliant) */
} wiced_wps_device_detail_t;

/**
 * WPS Credentials
 */
typedef struct
{
    wiced_ssid_t     ssid;               /**< AP SSID (name)       */
    wiced_security_t security;           /**< AP security type     */
    uint8_t          passphrase[64];     /**< AP passphrase        */
    uint8_t          passphrase_length;  /**< AP passphrase length */
} wiced_wps_credential_t;

/**
 * Vendor IE details
 */
typedef struct
{
    uint8_t         oui[WIFI_IE_OUI_LENGTH];     /**< Unique identifier for the IE */
    uint8_t         subtype;                     /**< Sub-type of the IE */
    void*           data;                        /**< Pointer to IE data */
    uint16_t        length;                      /**< IE data length */
    uint16_t        which_packets;               /**< Mask of the packet in which this IE details to be included */
} wiced_custom_ie_info_t;

/******************************************************
 *               Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/


/*****************************************************************************/
/** @addtogroup wificonn  WiFi Connectivity initialization and de-initialization
 *  @ingroup wifi
 *
 *  WICED functions to WiFi initialize/de-initialize WLAN connectivity.
 *
 *  @{
 */
/*****************************************************************************/

/** Initializes the WLAN parts of WICED.  Also adds entropy to the WICED pseudo-random number generator.
 *
 * @note: The WICED core should have already been initialized when this is called
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_wlan_connectivity_init  ( void );

/** Deinitializes the WLAN parts of WICED
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_wlan_connectivity_deinit( void );

/** Resume the WLAN parts of WICED after host deep-sleep
 *
 * @note: The WICED core should have already been initialized when this is called
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_wlan_connectivity_resume_after_deep_sleep( void );

/** @} */


/*****************************************************************************/
/** @addtogroup wifijoin   WiFi Join, Scan and Halt Functions
 *  @ingroup wifi
 *
 *  WICED functions to Wifi join/scan
 *
 *  @{
 */
/*****************************************************************************/

/**
 * Halt any joins, including ongoing ones
 * @param[in] halt: WICED_TRUE: halt all join attempts; WICED_FALSE: allow join attempts to proceed
 * Calling applications are expected to manage the join state independently from this API.  That is, this API gives no guarantee that
 * an in progress join is cancelled successfully.  However, calling wiced_leave_ap after this API would be acceptable toensure the host is in an unassociated state.
 * !!!!NOTE: wwd_wifi_join_halt( WICED_FALSE ) needs to be called after any wwd_wifi_join_halt( WICED_TRUE ) call
 * to allow subsequent join attempts to proceed.
 * @return @ref wiced_result_t WICED_SUCCESS
 */
wiced_result_t wiced_wifi_join_halt( wiced_bool_t halt );

/**Joins a specific access point using the BSSID
 *
 * @param[in] ssid          : An argument that specifies the SSID of the AP
 * @param[in] ssid_length   : Specifies the length of the ssid
 * @param[in] auth_type     : Specifies the authentication type
 * @param[in] security_key  : Security passpharse given by the user
 * @param[in] key_length    : Length of the security_key given
 * @param[in] bssid         : An argument that specifies the BSSID (MAC address of the Access Point) to join
 * @param[in] channel       : Specifies the Channel number of the particular AP to join
 * @param[in] ip            : Specifies the IP address of the device (optional)
 * @param[in] netmask       : Specifies the netmask (optional)
 * @param[in] gateway       : Specifies the gateway IP address (optional)
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_wifi_join_specific(char* ssid, uint8_t ssid_length, wiced_security_t auth_type, uint8_t* security_key, uint16_t key_length, char* bssid, char* channel, char* ip, char* netmask, char* gateway);

/**Joins a adhoc network
 *
 * @param[in] ssid          : An argument that specifies the SSID of the AP
 * @param[in] ssid_length   : Specifies the length of the ssid
 * @param[in] auth_type     : Specifies the authentication type
 * @param[in] security_key  : Security passpharse given by the user
 * @param[in] key_length    : Length of the security_key given
 * @param[in] channel       : Specifies the channel number of the particular AP to join
 * @param[in] ip            : Specifies the IP address of the device
 * @param[in] netmask       : Specifies the netmask
 * @param[in] gateway       : Specifies the gateway IP address
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_wifi_join_adhoc(char* ssid, uint8_t ssid_length, wiced_security_t auth_type, uint8_t* security_key, uint16_t key_length, char* channel, char* ip, char* netmask, char* gateway);

/**
 * Scans for Wi-Fi networks
 *
 * @param[in] results_handler  : A function pointer for the handler that will process
 *                               the network details as they arrive.
 * @param[in] user_data        : An argument that will be passed to the results_handler function
 *                               of this device
 *
 * @note @li The results_handler and user_data variables will be referenced after the function returns.
 *           Those variables must remain valid until the scan is complete.
 *       @li This call is non-blocking.
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_scan_networks( wiced_scan_result_handler_t results_handler, void* user_data );


/** Scans for Wi-Fi networks with extended parameters.  The function returns after the scan is started.  Results are reported in an asynchronous manner to the results_handler.
 *
 * @param[in] results_handler  : A function pointer for the handler that will process the scan results as they arrive.
 *
 * @param[in] user_data        : An argument that will be passed through to the results_handler function
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
 *
 * @note @li The results_handler and user_data variables will be referenced after the function returns.
 *           Those variables must remain valid until the scan is complete.
 *       @li This call is non-blocking.
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_scan_networks_ex( wiced_scan_result_handler_t results_handler, void* user_data, wiced_scan_type_t scan_type, wiced_bss_type_t bss_type, const wiced_ssid_t* optional_ssid, const wiced_mac_t* optional_mac, const uint16_t*  optional_channel_list, const wiced_scan_extended_params_t* optional_extended_params, wiced_interface_t interface);

/**
 * Enables/disables scans for wi-fi networks, including most roam scans done by firmware.
 *   !!!WARNING!!! Disable will abort any ongoing scans.
 *   Note: enable can used to allow firmware to do its normal roam scanning and any other internal scans.
 *   Calling wiced_wifi_scan_networks after a disable will cause wiced_wifi_scan_enable to be invoked.
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_scan_disable( void );
extern wiced_result_t wiced_wifi_scan_enable( void );
/** @} */

/*****************************************************************************/
/** @addtogroup wifiwps   WiFi Protected Setup
 *  @ingroup wifi
 *
 *  WICED functions to WPS (Wifi Protected Setup)
 *
 *  @{
 */
/*****************************************************************************/


/**
 * Negotiates securely with a Wi-Fi Protected Setup (WPS) registrar (usually an
 *  Access Point) and obtains credentials necessary to join the AP.
 *
 * @param[in] mode              : Indicates whether to use Push-Button (PBC) or PIN Number mode for WPS
 * @param[in] details           : Pointer to a structure containing manufacturing details
 *                                of this device
 * @param[in] password          : Password for WPS PIN mode connections
 * @param[out] credentials      : An array of credential structures that will receive the
 *                                securely negotiated credentials
 * @param[out] credential_count : The number of structures in the credentials parameter
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wps_enrollee( wiced_wps_mode_t mode, const wiced_wps_device_detail_t* details, const char* password, wiced_wps_credential_t* credentials, uint16_t credential_count );


/**
 * Negotiates securely with a Wi-Fi Protected Setup (WPS) enrollee (usually a
 *  client device) and provides credentials necessary to join a SoftAP.
 *
 * @param[in] mode              : Indicates whether to use Push-Button (PBC) or PIN Number mode for WPS
 * @param[in] details           : Pointer to a structure containing manufacturing details
 *                                of this device
 * @param[in] password          : Password for WPS PIN mode connections
 * @param[out] credentials      : An array of credential structures that will provide the
 *                                securely negotiated credentials
 * @param[out] credential_count : The number of structures in the credentials parameter
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wps_registrar( wiced_wps_mode_t mode, const wiced_wps_device_detail_t* details, const char* password, wiced_wps_credential_t* credentials, uint16_t credential_count );

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

/**
 * Finds the AP and its information for the given SSID
 *
 * @param[in] ssid                     : SSID of the access point for which user wants to find information.
 *                                       It must be a NULL terminated string 32 characters or less
 *
 * @param[out] ap_info                 : Pointer to the structure to store AP information.
 *
 * @param[in] optional_channel_list    : An optional channel list to restrict which channels are scanned. Note that the last entry must be 0
 *                                       If NULL, the scan will be performed on all supported Wi-Fi channels.
 *
 * @return @ref wiced_result_t
 */

extern wiced_result_t wiced_wifi_find_ap( const char* ssid, wiced_scan_result_t* ap_info, const uint16_t* optional_channel_list);

/**
 * Add Wi-Fi custom IE
 *
 * @param[in] interface : Interface to add custom IE
 * @param[in] ie_info   : Pointer to the structure which contains custom IE information
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_add_custom_ie( wiced_interface_t interface, const wiced_custom_ie_info_t* ie_info );

/**
 * Remove Wi-Fi custom IE
 *
 * @param[in] interface : Interface to remove custom IE
 * @param[in] ie_info   : Pointer to the structure which contains custom IE information
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_remove_custom_ie( wiced_interface_t interface, const wiced_custom_ie_info_t* ie_info );

/**
 * Brings up Wi-Fi core
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_up( void );

/**
 * Bring down Wi-Fi core preserving calibration
 *
 *  WARNING:
 *     This brings down the Wi-Fi core and all existing network connections.
 *     Bring up the Wi-Fi core using wiced_wifi_up() and bring up the required
 *     network connections using wiced_network_up().
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_down( void );

/**
 * Set roam trigger level for all bands
 *
 * @param[in] trigger_level : Trigger level in dBm. The Wi-Fi device will search for a new AP to connect to once the \n
 *                            signal from the AP (it is currently associated with) drops below the roam trigger level.
 *                            Valid value range: 2 to -100
 *                                      0 : Default roaming trigger
 *                                      1 : Optimize for bandwidth roaming trigger
 *                                      2 : Optimize for distance roaming trigger
 *                              -1 to -100: Roaming will be triggered based on the specified RSSI value
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_set_roam_trigger( int32_t trigger_level );

/**
* Set roam trigger level for given band
*
* @param[in] trigger_level : Trigger level in dBm. The Wi-Fi device will search for a new AP to connect to once the \n
*                            signal from the AP (it is currently associated with) drops below the roam trigger level.
*                            Valid value range: 2 to -100
*                                      0 : Default roaming trigger
*                                      1 : Optimize for bandwidth roaming trigger
*                                      2 : Optimize for distance roaming trigger
*                              -1 to -100: Roaming will be triggered based on the specified RSSI value
* @param[in] band : band on which to set the roam trigger
* @return @ref wiced_result_t
*/

extern wiced_result_t wiced_wifi_set_roam_trigger_per_band( int32_t trigger_level, wiced_802_11_band_t band );

/**
 * Get roam trigger level for the 2.4 Gigahertz band
 *
 * @param trigger_level  : Trigger level in dBm. Pointer to store current roam trigger level value
 * @return  @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_get_roam_trigger( int32_t* trigger_level );

/**
 * Get roam trigger level for the given band
 *
 * @param trigger_level  : Trigger level in dBm. Pointer to store current roam trigger level value
 * @param band : which band to use for the roam trigger query
 * @return  @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_get_roam_trigger_per_band( int32_t* trigger_level, wiced_802_11_band_t band );

/**
 * Get the current channel on STA interface
 *
 * @param[out] channel : A pointer to the variable where the channel value will be written
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_get_channel( uint32_t* channel );


/**
 * Retrieves the current Media Access Control (MAC) address
 *  (or Ethernet hardware address) of the 802.11 device
 *
 * @param mac Pointer to a variable that the current MAC address will be written to
 * @return    @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_get_mac_address( wiced_mac_t* mac );

/**
 * Get WLAN counter statistics for the interface provided
 *
 * @param[in] interface : The interface for which the counters are requested
 * @param[out] counters : A pointer to the structure where the counter data will be written
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_get_counters( wwd_interface_t interface, wiced_counters_t* counters );


/**
 * Sets the 802.11 powersave listen interval for a Wi-Fi client, and communicates
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
 *  interval API : wiced_wifi_set_listen_interval_assoc()
 *
 *  @note This function applies to 802.11 powersave operation. Please read the
 *  WICED Powersave Application Note provided in the WICED-SDK/Doc directory for further
 *  information about the operation of the 802.11 listen interval.
 *
 * @param[in] listen_interval : The desired beacon listen interval
 * @param[in] time_unit       : The listen interval time unit; options are beacon period or DTIM period
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_set_listen_interval( uint8_t listen_interval, wiced_listen_interval_time_unit_t time_unit );


/**
 * Sets the 802.11 powersave beacon listen interval communicated to Wi-Fi Access Points
 *
 *  This function is used by Wi-Fi clients to set the value of the beacon
 *  listen interval sent to the AP (in the association request frame) during
 *  the association process.
 *
 *  To set the client listen interval as well, use the wiced_wifi_set_listen_interval() API
 *
 *  @note This function applies to 802.11 powersave operation. Please read the
 *  WICED Powersave Application Note provided in the WICED-SDK/Doc directory for further
 *  information about the operation of the 802.11 listen interval.
 *
 * @param listen_interval : The beacon listen interval sent to the AP during association.
 *                          The time unit is specified in multiples of beacon periods.
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_set_listen_interval_assoc( uint16_t listen_interval );


/**
 * Gets the current value of all beacon listen interval variables
 *
 * @param[out] li : The current value of all listen interval settings
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_get_listen_interval( wiced_listen_interval_t* li );

/**
 * Sets the HT mode for the given interface
 *
 *  NOTE:
 *     Ensure WiFi core and network is down before invoking this function.
 *     Refer wiced_wifi_down() and wiced_network_down() functions for details.
 *
 * @param[in]   ht_mode     : HT mode to be set for the given interface
 * @param[in]   interface   : Interface for which HT Mode to be set
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_set_ht_mode( wiced_interface_t interface, wiced_ht_mode_t ht_mode );

/**
 * Gets the HT mode for the given interface
 *
 * @param[out]  ht_mode     : Pointer to the enum to store the currently used HT mode of the given interface.
 * @param[in]   interface   : Interface for which HT mode to be identified.
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_get_ht_mode( wiced_interface_t interface, wiced_ht_mode_t* ht_mode );

/**
 * Disable / enable 11n mode
 *
 *  NOTE:
 *     Ensure WiFi core and network is down before invoking this function.
 *     Refer wiced_wifi_down() API for details.
 *
 * @param[in]   interface : Disables 11n mode on the given interface
 * @param[in]   disable   : Boolean to indicate if 11n mode to be disabled/enabled. If set to WICED_TRUE, 11n mode will be disabled.
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_disable_11n_support( wiced_interface_t interface, wiced_bool_t disable );

/** @} */

/*****************************************************************************/
/** @addtogroup wifisoftap     WiFi Soft AP
 *  @ingroup wifi
 *  WICED Wi-Fi functions for Starting/Stopping SoftAP
 *
 *  @{
 */
/****************************************************************************/

/**
 * Start soft AP with custom IE
 *
 * @param[in] AP's ssid
 * @param[in] security type
 * @param[in] security key
 * @param[in] chanel
 * @param[in] custom IE
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_start_ap_with_custom_ie( wiced_ssid_t* ssid, wiced_security_t security, const char* key, uint8_t channel, const wiced_custom_ie_info_t* ie);

/**
 * Stop soft AP
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_stop_ap( void );


/** start soft AP with custom IE
 *
 * @param[in] AP's ssid
 * @param[in] security type
 * @param[in] security key
 * @param[in] chanel
 * @param[in] custom IE
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_start_ap_with_custom_ie( wiced_ssid_t* ssid, wiced_security_t security, const char* key, uint8_t channel, const wiced_custom_ie_info_t* ie);

/**
 * Stop soft AP
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_stop_ap( void );

/**
 * Register soft AP event handler
 *
 * @param[in] softap_event_handler  : A function pointer to the event handler
  *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_register_softap_event_handler( wiced_wifi_softap_event_handler_t softap_event_handler );


/**
 * Unregister soft AP event handler
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_unregister_softap_event_handler( void );

/**
 * Gets information about associated clients.
 *
 * @note Only applicable if softAP interface is up
 *
 * @param[out] client_list_buffer : pointer to a buffer that will be populated with a variable length structure defined by @ref wiced_maclist_t
 * @param[in]  buffer_length      : length of the buffer
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_get_associated_client_list( void* client_list_buffer, uint16_t buffer_length );

/**
 * Gets information about the AP the client interface is currently associated to
 *
 * @note Only applicable if STA (client) interface is associated to an AP
 *
 * @param[out] ap_info  : Pointer to structure that will be populated with AP information
 * @param[out] security : Pointer to structure that will be populated with AP security type
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_get_ap_info( wiced_bss_info_t* ap_info, wiced_security_t* security );

/**
 * Gets rssi information about the AP's client, as selected by mac address
 *
 * @note Only applicable if STA (client) interface is associated to an AP
 *
 * @param[out] rssi     : Pointer to RSSI (received signal strength)
 * @param[in] security  : Pointer to Client MAC address
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_get_ap_client_rssi( int32_t* rssi, const wiced_mac_t* client_mac_addr );

/**
 * Return WICED_TRUE if the STA interface has reported a link up event (due to 802.11 association) with no corresponding link down yet.
 *
 * @return @ref wiced_bool_t
 */
extern wiced_bool_t wiced_wifi_is_sta_link_up( void );
/** @} */

/*****************************************************************************/
/** @addtogroup wifirrm      WiFi Radio Resource Management
 *  @ingroup wifi
 *  WICED Wi-Fi functions for registering/de-registering Radio Resource Management
 *  event handlers
 *
 *  @{
 */
/****************************************************************************/


/**
 * Register RRM event handler
 * @param[in] wiced_wifi_rrm_event_handler_t : A function pointer to the event handler
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_register_rrm_event_handler( wiced_wifi_rrm_event_handler_t event_handler );

/**
 * DeRegister RRM event handler
 * @param void:
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_unregister_rrm_event_handler( void );

/** @} */

/*****************************************************************************/
/** @addtogroup wifinan       WiFi Neighborhood Area Networking
 *  @ingroup wifi
 *    WLAN NAN (neighborhood area networking)
 *    WICED Wi-Fi functions for NAN enable, disable and register/de-register of
 *  event handlers.
 *
 *  @{
 */
/****************************************************************************/

/**
 * Enable NAN service
 * @param[in] wiced_wifi_nan_event_handler_t : A function pointer to the event handler
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_nan_config_enable ( wiced_wifi_nan_event_handler_t nan_event_handler );

/**
 * Disable NAN service
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_nan_config_disable ( void );


/**
 * Register NAN event handler
 * @param[in] wiced_wifi_nan_event_handler_t : A function pointer to the event handler
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_register_nan_event_handler( wiced_wifi_nan_event_handler_t event_handler );

/**
 * DeRegister NAN event handler
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_unregister_nan_event_handler( void );
/** @} */

/*****************************************************************************/
/** @addtogroup wifipno     WiFi (Preferred Network Offload)
 *  @ingroup wifi
 *  WICED Wi-Fi functions for WLAN Preferred Network Offload
 *
 *  @{
 */
/*****************************************************************************/

/**
 * Preferred Network Offload start
 * @param[in] wiced_ssid_t*  :ssid
 * @param[in] wiced_security_t* : security
 * @param[in] wiced_scan_result_handler_t :  A function to the event handler
 * @param[in] void* : user_data
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_pno_start( wiced_ssid_t *ssid, wiced_security_t security, wiced_scan_result_handler_t handler, void *user_data );

/**
 * Preferred Network Offload stop
 * Halts the preferred network offload scanning process and clears all state associated with it
 * @return @ref wiced_result_t
 *
 */
extern wiced_result_t wiced_wifi_pno_stop( void );

/**
 * Preferred Network register callback function
 * @param[in] wiced_scan_result_handler_t : pno_handler
 * @param[in] void* : user_data
 * @return @ref wiced_result_t
 *
 */
extern wiced_result_t wiced_wifi_register_pno_callback( wiced_scan_result_handler_t pno_handler, void *user_data );

/*
 * Preferred Network de-register/unregister callback function
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_unregister_pno_callback( void );

/** @} */

/*****************************************************************************/
/** @addtogroup wifipowersave    WiFi Power Saving functions
 *  @ingroup wifi
 *  WICED Wi-Fi functions for WLAN low power modes
 *
 *  @{
 */
/*****************************************************************************/

/**
 * Enables powersave mode without regard for throughput reduction
 * This function enables (legacy) 802.11 PS-Poll mode and should be used
 * to achieve the lowest power consumption possible when the Wi-Fi device
 * is primarily passively listening to the network
 *
 * @warning: An accurate 32kHz clock reference must be connected to the WLAN
 *          sleep clock input pin while the WLAN chip is in powersave mode!
 *          Failure to meet this requirement will result in poor WLAN performance
 *          The sleep clock reference is typically configured in the file:
 *          <WICED-SDK>/include/platforms/<PLATFORM_NAME>/platform.h
 *
 * @param[in] void: None
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_enable_powersave( void );

/**
 * Enables powersave mode on specified interface without regard for throughput reduction
 * This function enables (legacy) 802.11 PS-Poll mode and should be used
 * to achieve the lowest power consumption possible when the Wi-Fi device
 * is primarily passively listening to the network
 *
 * @warning: An accurate 32kHz clock reference must be connected to the WLAN
 *          sleep clock input pin while the WLAN chip is in powersave mode!
 *          Failure to meet this requirement will result in poor WLAN performance
 *          The sleep clock reference is typically configured in the file
 *          <WICED-SDK>/include/platforms/<PLATFORM_NAME>/platform.h
 *
 * @param[in] interface             : The variable to set WLAN interface type
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_enable_powersave_interface( wiced_interface_t interface );


/**
 * Enables power-save mode while attempting to maximize throughput
 *
 * Network traffic is typically bursty. Reception of a packet often means that another
 * packet will be received shortly afterwards (and vice versa for transmit)
 * \p
 * In high throughput power-save mode, rather then entering power-save mode immediately
 * after receiving or sending a packet, the WLAN chip will wait for a timeout period before
 * returning to sleep
 *
 * @note: return_to_sleep_delay must be set to a multiple of 10.
 *
 *
 * @warning: An accurate 32kHz clock reference must be connected to the WLAN
 *          sleep clock input pin while the WLAN chip is in powersave mode!
 *          Failure to meet this requirement will result in poor WLAN performance.
 *          The sleep clock reference is typically configured in the file:
 *          <WICED-SDK>/include/platforms/<PLATFORM_NAME>/platform.h
 *
 * @param[in] return_to_sleep_delay : Timeout period (in milliseconds) before the WLAN chip returns to sleep
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_enable_powersave_with_throughput( uint16_t return_to_sleep_delay_ms );


/**
 * Enables powersave mode on specified interface while attempting to maximise throughput
 *
 * Network traffic is typically bursty. Reception of a packet often means that another
 * packet will be received shortly afterwards (and vice versa for transmit)
 * \p
 * In high throughput powersave mode, rather then entering powersave mode immediately
 * after receiving or sending a packet, the WLAN chip will wait for a timeout period before
 * returning to sleep
 *
 * @note: return_to_sleep_delay must be set to a multiple of 10.
 *
 *
 * @warning: An accurate 32kHz clock reference must be connected to the WLAN
 *          sleep clock input pin while the WLAN chip is in powersave mode
 *          Failure to meet this requirement will result in poor WLAN performance.
 *          The sleep clock reference is typically configured in the file:
 *          <WICED-SDK>/include/platforms/<PLATFORM_NAME>/platform.h
 *
 * @param[in] return_to_sleep_delay : Timeout period (in milliseconds) before the WLAN chip returns to sleep
 * @param[in] interface             : The variable to set WLAN interface type
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_enable_powersave_with_throughput_interface( uint16_t return_to_sleep_delay_ms, wiced_interface_t interface );

/**
 * Disable 802.11 power save mode
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_disable_powersave( void );

/**
 * Disable 802.11 power save mode on specified interface
 *
 * @param[in] interface             : The variable to set WLAN interface type
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_disable_powersave_interface( wiced_interface_t interface );

/** @} */

/*****************************************************************************/
/** @addtogroup  packet-filter  Packet Filter functions
 *  @ingroup wifi
 *
 *  WICED Wi-Fi functions for manipulating packet filters.
 *
 *  @{
 *
 */
/*****************************************************************************/

/**
 * Sets the packet filter mode (or rule) to either forward or discard packets on a match
 *
 * @param[in] wiced_packet_filter_mode_t : Packet filter mode
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_set_packet_filter_mode( wiced_packet_filter_mode_t mode );


/** Adds an ethernet packet filter which causes the WLAN chip to drop all packets that
 * do NOT match the filter
 *
 * When a packet filter(s) is installed, incoming packets received by the WLAN chip are
 * run through the pre-installed filter(s). Filter criteria are added using this API function.
 * If the WLAN chip receives a packet that matches one of the currently installed filters,
 * the host MCU is notified, and the packet is forwarded to the MCU. Packets that do
 * not match any of the installed filters are dropped by the WLAN chip.
 * \p
 * If there are no packet filters installed, all received packets are passed from
 * the WLAN chip to the host MCU
 *
 * @param[in] wiced_packet_filter_t  : Packet filter settings
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_add_packet_filter( const wiced_packet_filter_t* settings );

/**
 * Removes (un-install's) a previously installed packet filter
 *
 * @param[in] filter_id : The unique user assigned ID for the filter
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_remove_packet_filter( uint8_t filter_id );


/** Enables a previously installed packet filter
 *
 * @param[in] filter_id : The unique user assigned ID for the filter
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_enable_packet_filter( uint8_t filter_id );


/**
 * Disables a previously installed packet filter
 *
 * @param[in] filter_id : The unique user assigned ID for the filter
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_disable_packet_filter( uint8_t filter_id );


/**
 * Gets packet filter statistics including packets matched, packets forwarded and packets discarded.
 *
 * @param[in]  filter_id : The unique user assigned ID for the filter
 * @param[out] wiced_packet_filter_stats_t  : A pointer to a structure that will be populated with filter statistics
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_get_packet_filter_stats( uint8_t filter_id, wiced_packet_filter_stats_t* stats );


/**
 * Clear all packet filter statistics
 *
 * @param[in] filter_id : The unique user assigned ID for the filter
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_clear_packet_filter_stats( uint32_t filter_id );


/**
 * Get details of packet filters
 *
 * Note: does not retrieve the Filter mask and pattern. use @ref wiced_wifi_get_packet_filter_mask_and_pattern to retreive those.
 *
 * @param[in] max_count : The maximum number of filters to return details for.
 * @param[in] offset    : The location (count) of the first filter to retrieve (0=beginning)
 * @param[out] list     : An array which will receive the filter descriptors - must be able to fit max_count items
 * @param[out] count_out: The number of filter descriptors retrieved
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_get_packet_filters( uint32_t max_count, uint32_t offset, wiced_packet_filter_t* list,  uint32_t* count_out );

/**
 * Get the filter pattern and mask for a packet filters
 *
 * @param[in] filter_id : The id used to create the packet filter
 * @param[in] max_size  : Size of the supplied pattern and mask buffers in bytes
 * @param[out] mask     : Byte array that will receive the packet filter mask
 * @param[out] pattern  : Byte array that will receive the packet filter pattern
 * @param[out] size_out : The number bytes returned in each of pattern and filter buffers
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_get_packet_filter_mask_and_pattern( uint32_t filter_id, uint32_t max_size, uint8_t* mask, uint8_t* pattern, uint32_t* size_out );

/** @} */

/*****************************************************************************/
/** @addtogroup wifi-bt-comm    Wifi-BT communication functions
 *  @ingroup wifi
 *
 *  WICED WiFi functions for communicating with Wifi and Bluetooth.
 *
 *  @{
 */
/*****************************************************************************/

/**
 * Set the GCI mask
 *
 * This API sets trigger mask for GCI mailbox send message
 * from WLAN to BT.
 *
 * @param[in] gci_mask : GCI Mask
 *     Currently, following mask bits are supported
 *       WL_GCI_DS1_ENTRY        (1 << 0) : Wake-up on DS1 Entry
 *       WL_GCI_DS1_EXIT         (1 << 1) : Wake-up on DS1 Exit
 *       WL_GCI_DIS              (1 << 2) : Wake-up on loss-of-link due to Disassociation
 *       WL_GCI_BCN_LOSS         (1 << 3) : Wake-up on loss of beacon
 *       WL_GCI_RX_DATA          (1 << 4) : Wake-up on receiving data
 *       WL_GCI_DHCP_FAIL        (1 << 5) : Wake-up on Auto DHCP renew failure
 *       WL_GCI_IP_ADDR_CHANGED  (1 << 6) : Wake-up on IP Address change on auto DHCP
 *
 * @return @ref wiced_result_t
 */

extern wiced_result_t wiced_wifi_set_gci_mask ( uint32_t gci_mask );


/**
 *  Get information about a GCI mask
 *
 *  This API gets trigger mask for GCI mailbox send message.
 *
 * @param[in,out] uint32_t : Pointer to uint32_t to get the GCI mask value
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_get_gci_mask ( uint32_t* gci_mask );


/**
 * This API sends GCI mailbox message form WLAN to Blue-tooth chip
 *
 * @param[in] data : data of the GCI mailbox message to be sent from WLAN to BT.
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_send_gci_mailbox_message ( uint32_t data );

/** @} */



/*****************************************************************************/
/** @addtogroup keep-alive       Keep-Alive functions
 *  @ingroup wifi
 *
 *  WICED WiFi functions for automatically sending regular keep alive packets
 *
 *  @{
 */
/*****************************************************************************/

/**
 * Add a network keep alive packet
 *
 * Keep alive functionality enables the WLAN chip to automatically send
 * an arbitrary IP packet and/or 802.11 Null Function data frame at
 * a regular interval
 *
 * This feature may be used to maintain connectivity with a Wi-Fi AP
 * and/or remote network application
 *
 *  A maximum of 4 keep alive packets can be configured to operate concurrently
 *  Keep alive packet functionality only works with client (STA) mode
 *  If the keep alive packet length is set to 0, a Null-Function Data frame is automatically used as the keep alive
 *  Any ethernet packet can be sent as a keep alive packet
 *
 * @param[in] keep_alive_packet_info : Pointer to a @ref wiced_keep_alive_packet_t structure used to setup the keep alive packet
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_add_keep_alive( wiced_keep_alive_packet_t* keep_alive_packet_info );


/**
 * Get information about a keep alive packet
 *
 *  The ID of the keep alive packet should be provided in the keep_alive_info structure
 *  The application must pre-allocate a buffer to store the keep alive packet that is read from the WLAN chip
 *  The length of the buffer must be provided in the packet_length field of the structure
 *  The repeat period and keep alive packet bytes are populated by this function upon successful return
 *
 * @param[in,out] keep_alive_packet_info : Pointer to the wiced_keep_alive_t structure to be populated
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_get_keep_alive( wiced_keep_alive_packet_t* keep_alive_packet_info );


/**
 * Disable a keep alive packet specified by id
 *
 * @param[in] id : ID of the keep alive packet to be disabled
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_disable_keep_alive( uint8_t id );


/*
 ******************************************************************************
 * Convert an ipv4 string to a uint32_t.
 *
 * @param     arg  The string containing the value.
 * @param     arg  The structure which will receive the IP address
 *
 * @return    0 if read successfully
 */
int str_to_ipaddr_array(const char *a, wwd_ipv4_addr_t *n);


/** @} */

/**
 * Helper function to print a given MAC address via WPRINT_APP_INFO
 *
 * @param[in]  mac    A pointer to the @ref wiced_mac_t address
 */
static inline void print_mac_address( const wiced_mac_t* mac )
{
    UNUSED_PARAMETER( mac );
    WPRINT_APP_INFO( ( "%02X:%02X:%02X:%02X:%02X:%02X", mac->octet[0],
                                                        mac->octet[1],
                                                        mac->octet[2],
                                                        mac->octet[3],
                                                        mac->octet[4],
                                                        mac->octet[5] ) );
}


#ifdef __cplusplus
} /*extern "C" */
#endif
