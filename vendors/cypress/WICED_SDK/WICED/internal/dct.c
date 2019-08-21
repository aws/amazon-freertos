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
#ifndef MODUSTOOLBOX
#include "platform_config.h"
#endif
/*
*
*  Known by-product:
*  on External-SFLASH bootloader, you need to set the global define
*         BOOTLOADER_LOAD_MAIN_APP_FROM_EXTERNAL_LOCATION
*
*/
#include "generated_security_dct.h"
#include "platform_dct.h"
#include "wiced_defaults.h"
#include "wiced_result.h"
#include "wiced_apps_common.h"

#ifdef WIFI_CONFIG_APPLICATION_DEFINED
#include "wifi_config_dct.h"
#else/* #ifdef WIFI_CONFIG_APPLICATION_DEFINED */
#include "default_wifi_config_dct.h"
#endif /* #ifdef WIFI_CONFIG_APPLICATION_DEFINED */

#ifdef NETWORK_CONFIG_APPLICATION_DEFINED
#include "network_config_dct.h"
#else/* #ifdef NETWORK_CONFIG_APPLICATION_DEFINED */
#include "default_network_config_dct.h"
#endif /* #ifdef NETWORK_CONFIG_APPLICATION_DEFINED */

#ifdef WICED_DCT_INCLUDE_BT_CONFIG
#ifdef BT_CONFIG_APPLICATION_DEFINED
#include "bt_config_dct.h"
#else/* #ifdef BT_CONFIG_APPLICATION_DEFINED */
#include "default_bt_config_dct.h"
#endif /* #ifdef BT_CONFIG_APPLICATION_DEFINED */
#endif /* #ifdef WICED_DCT_INCLUDE_BT_CONFIG */

#ifdef WICED_DCT_INCLUDE_P2P_CONFIG
#ifdef P2P_CONFIG_APPLICATION_DEFINED
#include "p2p_config_dct.h"
#else/* #ifdef P2P_CONFIG_APPLICATION_DEFINED */
#include "default_p2p_config_dct.h"
#endif /* #ifdef P2P_CONFIG_APPLICATION_DEFINED */
#endif /* #ifdef WICED_DCT_INCLUDE_P2P_CONFIG */

#ifdef MODUSTOOLBOX
#include "generated_mac_address.txt"
#else
#include "../generated_mac_address.txt"
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

extern const void * const dct_full_size_loc; /* Defined by linker script */

#if defined(__ICCARM__)
#pragma section="initial_dct_section"
extern const void * const dct_used_size_loc; /* Defined by linker script */
#else
extern const void * const dct_used_size_loc; /* Defined by linker script */
#endif

#ifndef SOFT_AP_PASSPHRASE_LENGTH
#define SOFT_AP_PASSPHRASE_LENGTH      sizeof(SOFT_AP_PASSPHRASE)-1
#endif

#ifndef CONFIG_AP_PASSPHRASE_LENGTH
#define CONFIG_AP_PASSPHRASE_LENGTH    sizeof(CONFIG_AP_PASSPHRASE)-1
#endif

#ifndef P2P_GROUP_OWNER_PASSPHRASE_LENGTH
#define P2P_GROUP_OWNER_PASSPHRASE_LENGTH    sizeof(P2P_GROUP_OWNER_PASSPHRASE)-1
#endif

#ifndef COOEE_KEY_STRING
#define COOEE_KEY_STRING       "abcdabcdabcdabcd"
#endif

#ifndef WICED_NETWORK_INTERFACE
#define WICED_NETWORK_INTERFACE   WICED_STA_INTERFACE
#endif

#if defined(CLIENT_AP_2_SSID)
#define DEFAULT_AP_LIST_ENTRY_2 \
    [1] = \
    {     \
        .details = {{sizeof(CLIENT_AP_2_SSID)-1, CLIENT_AP_2_SSID},{{0,0,0,0,0,0}}, 0, 0, CLIENT_AP_2_BSS_TYPE, CLIENT_AP_2_SECURITY, CLIENT_AP_2_CHANNEL, CLIENT_AP_2_BAND}, \
        .security_key_length = sizeof(CLIENT_AP_2_PASSPHRASE)-1, \
        .security_key = CLIENT_AP_2_PASSPHRASE \
    },

#else
#define DEFAULT_AP_LIST_ENTRY_2
#endif

#define DEFAULT_AP_LIST  \
    { \
        [0] = \
        { \
            .details = {{sizeof(CLIENT_AP_SSID)-1, CLIENT_AP_SSID},{{0,0,0,0,0,0}}, 0, 0, CLIENT_AP_BSS_TYPE, CLIENT_AP_SECURITY, CLIENT_AP_CHANNEL, CLIENT_AP_BAND}, \
            .security_key_length = sizeof(CLIENT_AP_PASSPHRASE)-1, \
            .security_key = CLIENT_AP_PASSPHRASE\
        }, \
        DEFAULT_AP_LIST_ENTRY_2 \
    }

#if defined ( __IAR_SYSTEMS_ICC__ )
#pragma section="initial_dct_section"
const platform_dct_data_t initial_dct @ "initial_dct_section";
#endif /* if defined ( __IAR_SYSTEMS_ICC__ ) */

/* drop static so it will be included in output! */
const platform_dct_data_t initial_dct =
{
    .dct_header.full_size            = (unsigned long)&dct_full_size_loc,
#if defined (__ICCARM__)
    .dct_header.used_size            = (unsigned long)__section_size("initial_dct_section"),
#else
    .dct_header.used_size            = (unsigned long)&dct_used_size_loc,
#endif
    .dct_header.magic_number         = BOOTLOADER_MAGIC_NUMBER,
    .dct_header.write_incomplete     = 0,                       /* Always 0x00 on first write of DCT */
    .dct_header.app_valid            = 1,
    .dct_header.mfg_info_programmed  = 0,
#if  defined(DCT_BOOTLOADER_CRC_IS_IN_HEADER)
    .dct_header.sequence             = 0,
    .dct_header.crc32                = 0,
    .dct_header.initial_write        = 1,
#else
    .dct_header.is_current_dct       = 1,                       /* Always 0x01 on first write of DCT */
#endif
    .dct_header.apps_locations[ DCT_FR_APP_INDEX ].id       = EXTERNAL_FIXED_LOCATION,
    .dct_header.apps_locations[ DCT_DCT_IMAGE_INDEX ].id    = EXTERNAL_FIXED_LOCATION,
    .dct_header.apps_locations[ DCT_OTA_APP_INDEX ].id      = EXTERNAL_FIXED_LOCATION,
    .dct_header.apps_locations[ DCT_FILESYSTEM_IMAGE_INDEX ].id   = EXTERNAL_FIXED_LOCATION,
    .dct_header.apps_locations[ DCT_WIFI_FIRMWARE_INDEX ].id= EXTERNAL_FIXED_LOCATION,
    .dct_header.apps_locations[ DCT_APP0_INDEX ].id         = EXTERNAL_FIXED_LOCATION,
    .dct_header.apps_locations[ DCT_APP1_INDEX ].id         = EXTERNAL_FIXED_LOCATION,
    .dct_header.apps_locations[ DCT_APP2_INDEX ].id         = EXTERNAL_FIXED_LOCATION,
    .dct_header.apps_locations[ DCT_FR_APP_INDEX ].detail.external_fixed.location    = SFLASH_APPS_HEADER_LOC + sizeof(app_header_t) * DCT_FR_APP_INDEX,
    .dct_header.apps_locations[ DCT_DCT_IMAGE_INDEX ].detail.external_fixed.location = SFLASH_APPS_HEADER_LOC + sizeof(app_header_t) * DCT_DCT_IMAGE_INDEX,
    .dct_header.apps_locations[ DCT_OTA_APP_INDEX ].detail.external_fixed.location   = SFLASH_APPS_HEADER_LOC + sizeof(app_header_t) * DCT_OTA_APP_INDEX,
    .dct_header.apps_locations[ DCT_APP0_INDEX ].detail.external_fixed.location      = SFLASH_APPS_HEADER_LOC + sizeof(app_header_t) * DCT_APP0_INDEX,
    .dct_header.apps_locations[ DCT_APP1_INDEX ].detail.external_fixed.location      = SFLASH_APPS_HEADER_LOC + sizeof(app_header_t) * DCT_APP1_INDEX,
    .dct_header.apps_locations[ DCT_APP2_INDEX ].detail.external_fixed.location      = SFLASH_APPS_HEADER_LOC + sizeof(app_header_t) * DCT_APP2_INDEX,
    .dct_header.apps_locations[ DCT_FILESYSTEM_IMAGE_INDEX ].detail.external_fixed.location      = SFLASH_APPS_HEADER_LOC + sizeof(app_header_t) * DCT_FILESYSTEM_IMAGE_INDEX,
    .dct_header.apps_locations[ DCT_WIFI_FIRMWARE_INDEX ].detail.external_fixed.location      = SFLASH_APPS_HEADER_LOC + sizeof(app_header_t) * DCT_WIFI_FIRMWARE_INDEX,

#if defined( BOOTLOADER_LOAD_MAIN_APP_FROM_FILESYSTEM )
    .dct_header.boot_detail.entry_point = 0,
    .dct_header.boot_detail.load_details.valid = 1,
    .dct_header.boot_detail.load_details.load_once = 0,
    .dct_header.boot_detail.load_details.source.id = EXTERNAL_FILESYSTEM_FILE,
    .dct_header.boot_detail.load_details.source.detail.filesystem_filename = "app.elf",
    .dct_header.boot_detail.load_details.destination.id = INTERNAL,
    /* Need to define BOOTLOADER_LOAD_MAIN_APP_FROM_EXTERNAL_LOCATION in the platforms/<platform_name>toplevel.mk */
#elif defined( BOOTLOADER_LOAD_MAIN_APP_FROM_EXTERNAL_LOCATION )
    /* Need to define BOOTLOADER_LOAD_MAIN_APP_FROM_EXTERNAL_LOCATION in the platforms/<platform_name>toplevel.mk */
    .dct_header.boot_detail.entry_point = 0,
    .dct_header.boot_detail.load_details.valid = 1,
    .dct_header.boot_detail.load_details.load_once = 0,
    .dct_header.boot_detail.load_details.source.id = EXTERNAL_FIXED_LOCATION,
    .dct_header.boot_detail.load_details.source.detail.external_fixed.location = SFLASH_APPS_HEADER_LOC + sizeof(app_header_t) * DCT_APP0_INDEX,
    .dct_header.boot_detail.load_details.destination.id = INTERNAL,
#else
    .dct_header.boot_detail.entry_point = 0,
    .dct_header.boot_detail.load_details.valid = 0,
    .dct_header.boot_detail.load_details.source.id = NONE,
    .dct_header.boot_detail.load_details.source.detail.external_fixed.location = 0,
    .dct_header.boot_detail.load_details.destination.id = INTERNAL,
#endif /* ifdef BOOTLOADER_LOAD_MAIN_APP_FROM_FILESYSTEM */

    .security_credentials.certificate = CERTIFICATE_STRING,
    .security_credentials.private_key = PRIVATE_KEY_STRING,
    .security_credentials.cooee_key   = COOEE_KEY_STRING,
#ifdef WIFI_CONFIG_APPLICATION_DEFINED
    .wifi_config.device_configured   = WICED_TRUE,
#else
    .wifi_config.device_configured   = WICED_FALSE,
#endif
    .wifi_config.stored_ap_list      = DEFAULT_AP_LIST,
    .wifi_config.soft_ap_settings    = {{sizeof(SOFT_AP_SSID)-1,   SOFT_AP_SSID},   SOFT_AP_SECURITY,   SOFT_AP_CHANNEL,   SOFT_AP_PASSPHRASE_LENGTH,   SOFT_AP_PASSPHRASE},
    .wifi_config.config_ap_settings  = {{sizeof(CONFIG_AP_SSID)-1, CONFIG_AP_SSID}, CONFIG_AP_SECURITY, CONFIG_AP_CHANNEL, CONFIG_AP_PASSPHRASE_LENGTH, CONFIG_AP_PASSPHRASE, CONFIG_VALIDITY_VALUE},
#ifdef WICED_COUNTRY_CODE
    .wifi_config.country_code        = WICED_COUNTRY_CODE,
#else
    .wifi_config.country_code        = WICED_DEFAULT_COUNTRY_CODE,
#endif

#ifdef DCT_GENERATED_MAC_ADDRESS
    .wifi_config.mac_address.octet     = DCT_GENERATED_MAC_ADDRESS,
#endif

    .network_config.interface          = CONFIG_NETWORK_INTERFACE,
    .network_config.hostname.value     = CONFIG_NETWORK_IP_HOSTNAME,

#ifdef DCT_GENERATED_ETHERNET_MAC_ADDRESS
    .ethernet_config.mac_address.octet = DCT_GENERATED_ETHERNET_MAC_ADDRESS,
#endif

#ifdef WICED_BLUETOOTH_DEVICE_ADDRESS
     .bt_config.bluetooth_device_address            = WICED_BLUETOOTH_DEVICE_ADDRESS,
     .bt_config.bluetooth_device_name               = WICED_BLUETOOTH_DEVICE_NAME,
     .bt_config.bluetooth_device_class              = WICED_BLUETOOTH_DEVICE_CLASS,
#endif
#ifdef WICED_BLUETOOTH_SSP_DEBUG_MODE
     .bt_config.ssp_debug_mode                      = WICED_BLUETOOTH_SSP_DEBUG_MODE,
#else
     .bt_config.ssp_debug_mode                      = WICED_FALSE,
#endif

#ifdef P2P_GROUP_OWNER_SSID
     .p2p_config.p2p_group_owner_settings = {{sizeof(P2P_GROUP_OWNER_SSID)-1, P2P_GROUP_OWNER_SSID}, P2P_GROUP_OWNER_SECURITY, P2P_GROUP_OWNER_CHANNEL, P2P_GROUP_OWNER_PASSPHRASE_LENGTH, P2P_GROUP_OWNER_PASSPHRASE, CONFIG_VALIDITY_VALUE},
#endif

    .ota2_config.update_count          = 0,
    .ota2_config.boot_type             = 0,
    .ota2_config.force_factory_reset   = 0,

    .dct_version.crc32                      = 0,    /* Always 0x00 on first write of the DCT */
    .dct_version.sequence                   = 1,
    .dct_version.initial_write              = 1,    /* Always 0x01 on the initial write of the DCT */
    .dct_version.version                    = DCT_BOOTLOADER_SDK_CURRENT,
    .dct_version.magic_number               = DCT_VERSION_MAGIC_NUMBER,
    .dct_version.data_dct_usage_flags       = WICED_DCT_CONFIG_FLAGS,

    .dct_misc.wifi_flags                    = 0,
};

#if defined ( __IAR_SYSTEMS_ICC__ ) || ( defined (__GNUC__) && (__GNUC__ >= 6) )
int _start(void)
{
    /* in iar we must reference dct structure, otherwise it may not be included in */
    /* the dct image */
    return initial_dct.dct_header.write_incomplete;
}
#endif /* if defined ( __IAR_SYSTEMS_ICC__ ) || ( defined (__GNUC__) && (__GNUC__ >= 6) ) */


/******************************************************
 *               Function Definitions
 ******************************************************/
