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
 * Defines Device Configuration Table (DCT) structures
 *
 * Used in previous SDKs that are DIFFERENT from the current SDK
 *
 * structure names used in the code when referencing the current version:
 *
 * platform_dct_header_t
 * platform_dct_bt_config_t
 * platform_dct_ethernet_config_t
 * platform_dct_network_config_t
 * platform_p2p_config_t
 * platform_dct_ota2_config_t
 * platform_dct_version_t
 *
 * Different structures are named for the SDK when they were added or changed.
 *
 *
 * platform_dct_header_sdk_3_0_0_t  - initial version
 * platform_dct_header_sdk_3_1_1_t
 *          added apps_locations[DCT_MAX_APP_COUNT]
 * platform_dct_header_sdk_3_1_2_t
 *          added optional padding[] (DCT_HEADER_ALIGN_SIZE)
 * platform_dct_header_sdk_3_5_2_t
 *          added CRC, sequence number, initial_write
 *          moved magic_number, write_incomplete
 *          removed is_current_dct
 * platform_dct_header_sdk_3_7_0_t
 *          remove CDC, sequence_number, initial_write
 *          move magic_number and write_incomplete
 *          add is_current_dct
 *
 * platform_dct_bt_config_sdk_3_1_2_t  - initial version
 *          New OPTIONAL struct
 * platform_dct_bt_config_sdk_3_4_0_t
 *          added bluetooth_device_class
 *          changed padding
 *
 * platform_dct_ethernet_config_sdk_3_3_0_t  - initial version
 *          New structure
 *
 * platform_dct_network_config_sdk_3_3_0_t  - initial version
 *          New structure
 * platform_dct_network_config_sdk_3_3_1_t
 *          changed char             hostname[ HOSTNAME_SIZE + 1 ];
 *          to      wiced_hostname_t hostname;
 *
 * platform_p2p_config_sdk_3_5_1_t  - initial version
 *          New OPTIONAL struct
 *
 * platform_dct_ota2_config_sdk_3_5_2_t  - initial version
 *          New OPTIONAL struct
 * platform_dct_ota2_config_sdk_3_6_0_t
 *          changed padding[1] to force_factory_reset (structure size unchanged)
 *
 * platform_dct_version_sdk_3_7_0_t  - initial version
 *          New structure
 *
 * SDK-3.0.1:
 * Starting WICED v3.0.1
 * DCT                                 offset   size
 * platform_dct_data_t              :          0x1c5c
 * platform_dct_header_t            :  0x0000  0x0064
 * platform_dct_mfg_info_t          :  0x0064  0x009c
 * platform_dct_security_t          :  0x0100  0x1810
 * platform_dct_wifi_config_t       :  0x1910  0x034c
 *
 * SDK-3.1.0:
 * Starting WICED v3.1.0
 * DCT                                 offset   size
 * platform_dct_data_t              :          0x1c5c
 * platform_dct_header_t            :  0x0000  0x0064
 * platform_dct_mfg_info_t          :  0x0064  0x009c
 * platform_dct_security_t          :  0x0100  0x1810
 * platform_dct_wifi_config_t       :  0x1910  0x034c
 *
 * SDK-3.1.1:
 * Starting WICED v3.1.1
 * DCT                                 offset   size
 * platform_dct_data_t              :          0x1d7c
 * platform_dct_header_t            :  0x0000  0x0184 ** new fields
 * platform_dct_mfg_info_t          :  0x0184  0x009c
 * platform_dct_security_t          :  0x0220  0x1810
 * platform_dct_wifi_config_t       :  0x1a30  0x034c
 *
 * SDK-3.1.2:
 * Starting WICED v3.1.2
 * DCT                                 offset   size
 * platform_dct_data_t              :          0x1e80
 * platform_dct_header_t            :  0x0000  0x0184 ** optional padding added
 * platform_dct_mfg_info_t          :  0x0184  0x009c
 * platform_dct_security_t          :  0x0220  0x1810
 * platform_dct_wifi_config_t       :  0x1a30  0x034c
 * platform_dct_bt_config_t         :  0x1d7c  0x0101  ** new struct
 *
 * SDK-3.3.0:
 * Starting WICED v3.3.0
 * DCT                                 offset   size
 * platform_dct_data_t              :          0x1eac
 * platform_dct_header_t            :  0x0000  0x0184
 * platform_dct_mfg_info_t          :  0x0184  0x009c
 * platform_dct_security_t          :  0x0220  0x1810
 * platform_dct_wifi_config_t       :  0x1a30  0x034c
 * platform_dct_ethernet_config_t   :  0x1d7c  0x0008 ** new struct
 * platform_dct_network_config_t    :  0x1d84  0x0024 ** new struct
 * platform_dct_bt_config_t         :  0x1da8  0x0101
 *
 * SDK-3.3.1:
 * Starting WICED v3.3.1
 * DCT                                 offset   size
 * platform_dct_data_t              :          0x1eac
 * platform_dct_header_t            :  0x0000  0x0184
 * platform_dct_mfg_info_t          :  0x0184  0x009c
 * platform_dct_security_t          :  0x0220  0x1810
 * platform_dct_wifi_config_t       :  0x1a30  0x034c
 * platform_dct_ethernet_config_t   :  0x1d7c  0x0008
 * platform_dct_network_config_t    :  0x1d84  0x0024 ** field change, size is the same
 * platform_dct_bt_config_t         :  0x1da8  0x0101
 *
 * SDK-3.4.0:
 * Starting WICED v3.4.0
 * DCT                                 offset   size
 * platform_dct_data_t              :          0x1eb0
 * platform_dct_header_t            :  0x0000  0x0184
 * platform_dct_mfg_info_t          :  0x0184  0x009c
 * platform_dct_security_t          :  0x0220  0x1810
 * platform_dct_wifi_config_t       :  0x1a30  0x034c
 * platform_dct_ethernet_config_t   :  0x1d7c  0x0008
 * platform_dct_network_config_t    :  0x1d84  0x0024
 * platform_dct_bt_config_t         :  0x1da8  0x0105 ** added field
 *
 * SDK-3.5.1:
 * Starting WICED v3.5.1
 * DCT                                 offset   size
 * platform_dct_data_t              :          0x1f24
 * platform_dct_header_t            :  0x0000  0x0184
 * platform_dct_mfg_info_t          :  0x0184  0x009c
 * platform_dct_security_t          :  0x0220  0x1810
 * platform_dct_wifi_config_t       :  0x1a30  0x034c
 * platform_dct_ethernet_config_t   :  0x1d7c  0x0008
 * platform_dct_network_config_t    :  0x1d84  0x0024
 * platform_dct_bt_config_t         :  0x1da8  0x0105
 * platform_dct_p2p_config_t        :  0x1eb0  0x0074 ** new struct
 *
 * SDK-3.5.2:
 * Starting WICED v3.5.2
 * DCT                                 offset   size
 * platform_dct_data_t              :          0x1f30
 * platform_dct_header_t            :  0x0000  0x018c ** changed fields
 * platform_dct_mfg_info_t          :  0x018c  0x009c
 * platform_dct_dct_security_t      :  0x0228  0x1810
 * platform_dct_wifi_config_t       :  0x1a38  0x034c
 * platform_dct_ethernet_config_t   :  0x1d84  0x0008
 * platform_dct_network_config_t    :  0x1d8c  0x0024
 * platform_dct_bt_config_t         :  0x1db0  0x0105
 * platform_dct_p2p_config_t        :  0x1eb8  0x0074
 * platform_dct_ota2_config_t       :  0x1f2c  0x0004 ** new struct
 *
 * Starting WICED v3.6.0
 * DCT                                 offset   size
 * platform_dct_data_t              :          0x1f30
 * platform_dct_header_t            :  0x0000  0x018c
 * platform_dct_mfg_info_t          :  0x018c  0x009c
 * platform_dct_dct_security_t      :  0x0228  0x1810
 * platform_dct_wifi_config_t       :  0x1a38  0x034c
 * platform_dct_ethernet_config_t   :  0x1d84  0x0008
 * platform_dct_network_config_t    :  0x1d8c  0x0024
 * platform_dct_bt_config_t         :  0x1db0  0x0105
 * platform_dct_p2p_config_t        :  0x1eb8  0x0074
 * platform_dct_ota2_config_t       :  0x1f2c  0x0004
 *
 * SDK-3.6.1:
 * Starting WICED v3.6.1
 * DCT                                 offset   size
 * platform_dct_data_t              :          0x1f30
 * platform_dct_header_t            :  0x0000  0x018c
 * platform_dct_mfg_info_t          :  0x018c  0x009c
 * platform_dct_dct_security_t      :  0x0228  0x1810
 * platform_dct_wifi_config_t       :  0x1a38  0x034c
 * platform_dct_ethernet_config_t   :  0x1d84  0x0008
 * platform_dct_network_config_t    :  0x1d8c  0x0024
 * platform_dct_bt_config_t         :  0x1db0  0x0105
 * platform_dct_p2p_config_t        :  0x1eb8  0x0074
 * platform_dct_ota2_config_t       :  0x1f2c  0x0004
 *
 * SDK-3.6.2:
 * Starting WICED v3.6.2-RC1
 * DCT                                 offset   size
 * platform_dct_data_t              :          0x1f30
 * platform_dct_header_t            :  0x0000  0x018c
 * platform_dct_mfg_info_t          :  0x018c  0x009c
 * platform_dct_security_t          :  0x0228  0x1810
 * platform_dct_wifi_config_t       :  0x1a38  0x034c
 * platform_dct_ethernet_config_t   :  0x1d84  0x0008
 * platform_dct_network_config_t    :  0x1d8c  0x0024
 * platform_dct_bt_config_t         :  0x1db0  0x0105
 * platform_dct_p2p_config_t        :  0x1eb8  0x0074
 * platform_dct_ota2_config_t       :  0x1f2c  0x0004 ** Field changed, size stayed the same
 *
 * SDK-3.7.0:
 * Starting WICED v3.3.DEVELOPMENT
 * DCT                                 offset   size
 * platform_dct_data_t              :          0x1f38
 * platform_dct_header_t            :  0x0000  0x0184 ** changed
 * platform_dct_mfg_info_t          :  0x0184  0x009c
 * platform_dct_dct_security_t      :  0x0220  0x1810
 * platform_dct_wifi_config_t       :  0x1a30  0x034c
 * platform_dct_ethernet_config_t   :  0x1d7c  0x0008
 * platform_dct_network_config_t    :  0x1d84  0x0024
 * platform_dct_bt_config_t         :  0x1da8  0x0105
 * platform_dct_p2p_config_t        :  0x1eb0  0x0074
 * platform_dct_ota2_config_t       :  0x1f24  0x0004
 * platform_dct_version_t           :  0x1f28  0x0010 ** new struct
 *
 * SDK-4.0.1
 * Starting WICED v4.x-DEVELOPMENT
 * DCT                                 offset   size
 * platform_dct_data_t               :          0x1f3c
 * platform_dct_header_t             :  0x0000  0x0184
 * platform_dct_mfg_info_t           :  0x0184  0x009c
 * platform_dct_dct_security_t       :  0x0220  0x1810
 * platform_dct_wifi_config_t        :  0x1a30  0x034c
 * platform_dct_ethernet_config_t    :  0x1d7c  0x0008
 * platform_dct_network_config_t     :  0x1d84  0x0024
 * platform_dct_bt_config_t          :  0x1da8  0x0105
 * platform_dct_p2p_config_t         :  0x1eb0  0x0074
 * platform_dct_ota2_config_t        :  0x1f24  0x0004
 * platform_dct_version_t            :  0x1f28  0x0010
 * platform_dct_misc_config_t        :  0x1f38  0x0004 ** new struct
 *
 * Starting WICED-SDK 5.0.1
 * DCT                                 offset   size
 * Application DCT 0x0501               offset   size
 * platform_dct_data_t               :          0x1f3c
 * platform_dct_header_t             :  0x0000  0x0184
 * platform_dct_mfg_info_t           :  0x0184  0x009c
 * platform_dct_dct_security_t       :  0x0220  0x1810
 * platform_dct_wifi_config_t        :  0x1a30  0x034c
 * platform_dct_ethernet_config_t    :  0x1d7c  0x0008
 * platform_dct_network_config_t     :  0x1d84  0x0024
 * platform_dct_bt_config_t          :  0x1da8  0x0105
 * platform_dct_p2p_config_t         :  0x1eb0  0x0074
 * platform_dct_ota2_config_t        :  0x1f24  0x0004
 * platform_dct_version_t            :  0x1f28  0x0010
 * platform_dct_misc_config_t        :  0x1f38  0x0004 ** Changed Field aggregate_code to wifi_flags
 *
 * Starting WICED-SDK 5.1
 * DCT                                 offset   size
 * Application DCT 0x0510               offset   size
 * platform_dct_data_t               :          0x1f40
 * platform_dct_header_t             :  0x0000  0x0184
 * platform_dct_mfg_info_t           :  0x0184  0x009c
 * platform_dct_dct_security_t       :  0x0220  0x1810
 * platform_dct_wifi_config_t        :  0x1a30  0x034c
 * platform_dct_ethernet_config_t    :  0x1d7c  0x0008
 * platform_dct_network_config_t     :  0x1d84  0x0024
 * platform_dct_bt_config_t          :  0x1da8  0x0105
 * platform_dct_p2p_config_t         :  0x1eb0  0x0074
 * platform_dct_ota2_config_t        :  0x1f24  0x0004
 * platform_dct_version_t            :  0x1f28  0x0010
 * platform_dct_misc_config_t        :  0x1f38  0x0004
 * uint64_t force_to_8_bytes[]       :  0x1f40  0x0000   ** added so platform_data_dct_t ends on an 8-byte boundary so that app_dct is 8-byte aligned
 * app data offset (end of dct_data) :  0x1f40  0x1f40
 *
 */
#pragma once

#include <stdint.h>
#include "wwd_structures.h"

#ifdef __cplusplus
extern "C" {
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


/***********************************************************************************************
 *                version SDK-3.0.0 structures
 *
 * platform_dct_header_sdk_3_0_0_t  - initial version
 *
 ***********************************************************************************************/
typedef struct
{
    unsigned long full_size;
    unsigned long used_size;
    char write_incomplete;
    char is_current_dct;
    char app_valid;
    char mfg_info_programmed;
    unsigned long magic_number;
    boot_detail_t boot_detail;
    void (*load_app_func)(void); /* WARNING: TEMPORARY */
} platform_dct_header_sdk_3_0_0_t;

/***********************************************************************************************
 *                 SDK-3.1.0
 *
 *                 no changes from SDK-3.0.0
 *
 ***********************************************************************************************/

/***********************************************************************************************
 *                 SDK-3.1.1
 *
 * platform_dct_header_sdk_3_1_1_t
 *  added apps_locations[DCT_MAX_APP_COUNT]
 *    affects:
 *      platform_dct_header_sdk_3_1_1_t
 *
 ***********************************************************************************************/

typedef struct
{
        unsigned long full_size;
        unsigned long used_size;
        char write_incomplete;
        char is_current_dct;
        char app_valid;
        char mfg_info_programmed;
        unsigned long magic_number;
        boot_detail_t boot_detail;
        image_location_t apps_locations[ DCT_MAX_APP_COUNT ]; /* added in SDK 3.1.1 */
        void (*load_app_func)( void ); /* WARNING: TEMPORARY */
} platform_dct_header_sdk_3_1_1_t;


/***********************************************************************************************
 *                 SDK-3.1.2
 *
 * platform_dct_header_sdk_3_1_2_t
 *          added padding[]
 *
 * platform_dct_bt_config_sdk_3_1_2_t  - initial version
 *          New OPTIONAL struct
 *
 ***********************************************************************************************/

/* This is used to calculate the padding of platform_dct_header.
 * Should always match platform_dct_header_t, except for the padding.
 */
struct platform_dct_header_sdk_3_1_2_s
{
        unsigned long full_size;
        unsigned long used_size;
        char write_incomplete;
        char is_current_dct;
        char app_valid;
        char mfg_info_programmed;
        unsigned long magic_number;
        boot_detail_t boot_detail;
        image_location_t apps_locations[ DCT_MAX_APP_COUNT ];
        void (*load_app_func)( void ); /* WARNING: TEMPORARY */
};

typedef struct
{
        unsigned long full_size;
        unsigned long used_size;
        char write_incomplete;
        char is_current_dct;
        char app_valid;
        char mfg_info_programmed;
        unsigned long magic_number;
        boot_detail_t boot_detail;
        image_location_t apps_locations[ DCT_MAX_APP_COUNT ];
        void (*load_app_func)( void ); /* WARNING: TEMPORARY */
#ifdef  DCT_HEADER_ALIGN_SIZE
        uint8_t padding[DCT_HEADER_ALIGN_SIZE - sizeof(struct platform_dct_header_sdk_3_1_2_s)];
#endif
} platform_dct_header_sdk_3_1_2_t;


typedef struct
{
    uint8_t bluetooth_device_address[6];
    uint8_t bluetooth_device_name[249]; /* including null termination */
    wiced_bool_t ssp_debug_mode;
    uint8_t padding[1];   /* to ensure 32-bit aligned size */
} platform_dct_bt_config_sdk_3_1_2_t;




/***********************************************************************************************
 *                 SDK-3.3.0
 *
 * platform_dct_ethernet_config_sdk_3_3_0_t  - initial version
 *          New structure
 *
 * platform_dct_network_config_sdk_3_3_0_t  - initial version
 *          New structure
 ***********************************************************************************************/

typedef struct
{
    image_location_id_t     id;
    union
    {
        fixed_location_t    internal_fixed;
        fixed_location_t    external_fixed;
        char                filesystem_filename[32];       /* fixed filesystem_filename name in this version */
    } detail;
} image_location_sdk_3_3_0_t;

typedef struct
{
    wiced_mac_t             mac_address;
    uint8_t                 padding[2];                     /* to ensure 32bit aligned size */
} platform_dct_ethernet_config_sdk_3_3_0_t;

typedef struct
{
    wiced_interface_t       interface;
    char                    hostname[ HOSTNAME_SIZE + 1 ];
    uint8_t                 padding[2];                     /* to ensure 32bit aligned size */
} platform_dct_network_config_sdk_3_3_0_t;

/***********************************************************************************************
 *                 SDK-3.3.1
 *
 * platform_dct_network_config_sdk_3_3_1_t
 *          changed char             hostname[ HOSTNAME_SIZE + 1 ];
 *          to      wiced_hostname_t hostname;
 *
 ***********************************************************************************************/

typedef struct
{
    wiced_interface_t         interface;
    wiced_hostname_t          hostname;
    uint8_t                   padding[2];  /* to ensure 32bit aligned size */
} platform_dct_network_config_sdk_3_3_1_t;

/***********************************************************************************************
 *                 SDK-3.4.0
 *
 *  platform_dct_bt_config_sdk_3_4_0_t
 *      added bluetooth_device_class
 *      changed padding
 *
 ***********************************************************************************************/
typedef struct
{
    uint8_t         bluetooth_device_address[6];
    uint8_t         bluetooth_device_name[249];     /* including null termination */
    uint8_t         bluetooth_device_class[3];
    wiced_bool_t    ssp_debug_mode;
    uint8_t         padding[2];   /* to ensure 32-bit aligned size */
} platform_dct_bt_config_sdk_3_4_0_t;

/***********************************************************************************************
 *                 SDK-3.5.1
 *
 * platform_p2p_config_sdk_3_5_1_t
 *          New OPTIONAL struct
 *
 ***********************************************************************************************/
typedef struct
{
    wiced_config_soft_ap_t  p2p_group_owner_settings;
    uint8_t                 padding[2];                 /* to ensure 32-bit aligned size */
} platform_dct_p2p_config_sdk_3_5_1_t;

/***********************************************************************************************
 *                 SDK-3.5.2
 *
 * platform_dct_header_sdk_3_5_2_t
 *          added CRC, sequence number, initial_write
 *          moved magic_number, write_incomplete
 *          removed is_current_dct
 *
 ***********************************************************************************************/
struct platform_dct_header_sdk_3_5_2_s
{
        unsigned long full_size;
        unsigned long used_size;
        unsigned long magic_number;
        char write_incomplete;
        char app_valid;
        char mfg_info_programmed;
        char initial_write;         /* first time DCT is written at manufacture */
        unsigned long sequence;
        CRC_TYPE        crc32;
        boot_detail_t boot_detail;
        image_location_t apps_locations[ DCT_MAX_APP_COUNT ];
        void (*load_app_func)( void ); /* WARNING: TEMPORARY */
};

typedef struct
{
        unsigned long full_size;
        unsigned long used_size;
        unsigned long magic_number;
        char write_incomplete;
        char app_valid;
        char mfg_info_programmed;
        char initial_write;         /* first time DCT is written at manufacture */
        unsigned long sequence;
        CRC_TYPE      crc32;
        boot_detail_t boot_detail;
        image_location_t apps_locations[ DCT_MAX_APP_COUNT ];
        void (*load_app_func)( void ); /* WARNING: TEMPORARY */
#ifdef  DCT_HEADER_ALIGN_SIZE
        uint8_t padding[DCT_HEADER_ALIGN_SIZE - sizeof(struct platform_dct_header_sdk_3_5_2_s)];
#endif
} platform_dct_header_sdk_3_5_2_t;

typedef struct
{
        uint16_t        update_count;     /* 0x00 when first programmed, incremented when updated -or- factory reset */
        uint8_t         boot_type;        /* value = ota2_boot_type_t */
        uint8_t         padding[1];       /* to ensure 32-bit aligned size */
} platform_dct_ota2_config_sdk_3_5_2_t;

/***********************************************************************************************
 *                 SDK-3.6.0
 *
 * platform_dct_ota2_config_sdk_3_6_0_t
 *          changed padding[1] to force_factory_reset (structure size unchanged)
 *
 ***********************************************************************************************/
typedef struct
{
        uint16_t        update_count;           /* 0x00 when first programmed, incremented when updated -or- factory reset */
        uint8_t         boot_type;              /* value = ota2_boot_type_t */
        uint8_t         force_factory_reset;    /* call wiced_ota2_force_factory_reset_on_reboot() - set to non-zero to force reboot )  */
} platform_dct_ota2_config_sdk_3_6_0_t;

/***********************************************************************************************
 *                 SDK-3.6.1
 *
 *                 no changes from SDK-3.6.0
 *
 ***********************************************************************************************/

/***********************************************************************************************
 *                 SDK-3.6.2
 *
 *                 no changes from SDK-3.6.1
 *
 ***********************************************************************************************/

/***********************************************************************************************
 *                 SDK-3.7.0
 *
 * platform_dct_header_sdk_3_7_0_t
 *          remove CDC, sequence_number, initial_write
 *          move magic_number and write_incomplete
 *          add is_current_dct
 ***********************************************************************************************/
struct platform_dct_header_current_sdk_3_7_0_s
{
        unsigned long full_size;
        unsigned long used_size;
        char write_incomplete;
        char is_current_dct;
        char app_valid;
        char mfg_info_programmed;
        unsigned long magic_number;
        boot_detail_t boot_detail;
        image_location_t apps_locations[ DCT_MAX_APP_COUNT ];
        void (*load_app_func)( void ); /* WARNING: TEMPORARY */
};

typedef struct
{
        unsigned long full_size;
        unsigned long used_size;
        char write_incomplete;
        char is_current_dct;
        char app_valid;
        char mfg_info_programmed;
        unsigned long magic_number;
        boot_detail_t boot_detail;
        image_location_t apps_locations[ DCT_MAX_APP_COUNT ];
        void (*load_app_func)( void ); /* WARNING: TEMPORARY */
#ifdef DCT_HEADER_ALIGN_SIZE
        uint8_t padding[DCT_HEADER_ALIGN_SIZE - sizeof(struct platform_dct_header_current_sdk_3_7_0_s)];
#endif
} platform_dct_header_current_sdk_3_7_0_t;


typedef struct
{
    uint32_t                magic_number;           /* DCT_VERSION_MAGIC_NUMBER                                         */
    CRC_TYPE                crc32;                  /* crc for this DCT (always 0 for SDKs 3.5.2 thru 3.6.2             */
    unsigned long           sequence;               /* sequence number to know which DCT is the latest                  */
    wiced_dct_config_flag_t data_dct_usage_flags;   /* which of the optional sub-structures are in use                  */
    char                    initial_write;          /* first time DCT is written at manufacture (crc will also be 0x00) */
    uint8_t                 padding[1];             /* ensure 32-bit aligned size                                       */
} platform_dct_version_sdk_3_7_0_t;

/***********************************************************************************************
 *                 SDK-4.0.1
 *
 * platform_dct_misc_config_sdk_4_0_1_t
 *          New struct
 *          add aggregate_code
 ***********************************************************************************************/
typedef struct
{
    wiced_aggregate_code_t    aggregate_code;
} platform_dct_misc_config_sdk_4_0_1_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif
