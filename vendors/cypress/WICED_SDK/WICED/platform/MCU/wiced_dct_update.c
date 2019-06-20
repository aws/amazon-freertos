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
 * Converters for older DCT to current DCT
 *
 * To determine DCT version (starting with SDK-3.6.4)
 *
 *
 *  platform_dct_header_t
 *      Changes from SDK-3.1.0 --> SDK-3.1.1
 *          added apps_locations[DCT_MAX_APP_COUNT]
 *          struct image_location_t :: enum id moved to separate enum image_location_id_t (possible field size issue ? )
 *      Changes from SDK-3.1.1  --> SDK-3.1.2
 *          added padding[]
 *      { these changes negate each other
 *          Changes from SDK-3.5.1 --> SDK-3.5.2
 *              added CRC, sequence number, initial_write
 *              moved magic_number, write_incomplete
 *              removed is_current_dct
 *          Changes from SDK-3.6.3 --> SDK-3.6.4 (current)
 *              remove CDC, sequence_number, initial_write
 *              move magic_number and write_incomplete
 *              add is_current_dct
 *      }
 *
 * platform_dct_ethernet_config_t
 *      Changes from SDK-3.1.2 --> SDK-3.3.0
 *          New structure
 *
 *  platform_dct_network_config_t
 *      Changes from SDK-3.1.2 --> SDK-3.3.0
 *          New structure
 *      Changes from SDK-3.3.0 --> SDK-3.3.1
 *          changed char             hostname[ HOSTNAME_SIZE + 1 ];
 *          to      wiced_hostname_t hostname;
 *
 * platform_dct_bt_config_t
 *      Changes from SDK-3.1.1  --> SDK-3.1.2
 *          New OPTIONAL struct
 *      Changes from SDK-3.1.2 --> SDK-3.4.0
 *          added bluetooth_device_class
 *          changed padding
 *
 * platform_p2p_config_t
 *      Changes from SDK-3.4.0 --> SDK-3.5.1
 *          New OPTIONAL struct
 *
 * platform_dct_ota2_config_t
 *      Changes from SDK-3.5.1 --> SDK-3.5.2
 *          New OPTIONAL struct
 *      Changes from SDK-3.5.2 --> SDK-3.6.0
 *          changed padding[1] to force_factory_reset (structure size unchanged)
 *
 * platform_dct_version_t
 *      Changes from SDK-3.6.3 --> SDK-3.6.4
 *          New structure
 *
 * platform_dct_misc_t
 *      Changes from SDK-3.7.0 --> SDK-4.0.1
 *          New structure
 *      Changes from SDK-4.0.1 --> SDK-5.0.1
 *          changed aggregate_code to wifi_flags (structure size unchanged)
 *
 * platform_dct_data_t
 *      Changes from SDK-4.0.1 --> SDK-5.0.1
 *          added 8-byte field to force 8-byte size of platform dct structure
 */

#include "string.h"
#include "wwd_assert.h"
#include "wiced_result.h"
#include "spi_flash.h"
#include "platform_dct.h"
#include "waf_platform.h"
#include "wiced_framework.h"
#include "wiced_dct_common.h"
#include "wiced_apps_common.h"
#include "platform_mcu_peripheral.h"
#include "elf.h"
#include "../../../libraries/utilities/crc/crc.h"

// DEBUGGING
#if 0
#include "../../../libraries/utilities/mini_printf/mini_printf.h"
#else
#define mini_printf(a, ...)
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
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/


/******************************************************
  *               Function Definitions
  ******************************************************/

wiced_result_t wiced_dct_minimal_check_dct_header( platform_dct_header_t *dct_header)
{
    /* Since the headers area always compatible between bootloader & application DCTs, just check for the common flags
     * as a minimal test - do not just count on this to be correct!!!
     */
     if ( (dct_header != NULL) && (dct_header->used_size <= dct_header->full_size) &&
          (dct_header->magic_number == BOOTLOADER_MAGIC_NUMBER) &&
//#if !defined(DCT_BOOTLOADER_CRC_IS_IN_HEADER) // in determining version, we don't care about if it's current!
//          (dct_header->is_current_dct   == 1) &&
//#endif
         (dct_header->write_incomplete == 0) )
         {
         /* this is at least true */
         return WICED_SUCCESS;
     }

     return WICED_ERROR;
}

 wiced_result_t wiced_dct_minimal_check_dct_version( platform_dct_version_t *dct_version)
 {
     /* Check for the common flags as a minimal test - do not just count on this to be correct!!! */
      if ( (dct_version != NULL) &&
           (dct_version->magic_number == DCT_VERSION_MAGIC_NUMBER) &&
          (dct_version->version != 0x0000) && (dct_version->version != 0xFFFF) &&
          (dct_version->version != DCT_BOOTLOADER_SDK_UNKNOWN) &&
          (dct_version->version <= DCT_BOOTLOADER_SDK_CURRENT) )
      {
          /* this is at least true */
          return WICED_SUCCESS;
      }

      return WICED_ERROR;
 }

#if !defined(BOOTLOADER)
/*
 *  WE CANNOT CHANGE THE LAYOUT OF platform_dct_header_t FROM THE SDK VERSION THE BOOTLOADER WAS BUILT WITH
 *
 *  While the SDKs have different platform_dct_header_t structures, when building an update Applciation,
 *  we always use the platform_dct_header_t structure from the SDK that the Bootloader was built with.
 *
 * platform_dct_header_t
 *      Changes from SDK-3.1.0 --> SDK-3.1.1
 *          added apps_locations[DCT_MAX_APP_COUNT]
 *      Changes from SDK-3.1.1  --> SDK-3.1.2
 *          added padding[]
 *      Changes from SDK-3.5.1 --> SDK-3.5.2
 *          added CRC, sequence number, initial_write
 *          moved magic_number, write_incomplete
 *          removed is_current_dct
 *      Changes from SDK-3.6.3 --> SDK-3.6.4 (current)
 *          remove CDC, sequence_number, initial_write
 *          move magic_number and write_incomplete
 *          add is_current_dct
 *
 */

wiced_result_t wiced_dct_update_header_to_current (platform_dct_header_t* dct_hdr_destination,
                                                   platform_dct_header_t* dct_hdr_source)
{
    if ((dct_hdr_destination == NULL) || (dct_hdr_source == NULL))
    {
        return WICED_ERROR;
    }
    /* just copy the dct header (this is always the same between bootloader and application) */
    memcpy(dct_hdr_destination, dct_hdr_source, sizeof(platform_dct_header_t));

    /* check a few things... */
    if (dct_hdr_destination->full_size == 0)
    {
        /* If it is 0x00, Set it here */
        dct_hdr_destination->full_size = PLATFORM_DCT_COPY2_START_ADDRESS - PLATFORM_DCT_COPY1_START_ADDRESS;
    }
    if (dct_hdr_destination->used_size == 0)
    {
        /* If it is 0x00, Set it here */
        dct_hdr_destination->used_size = (PLATFORM_DCT_COPY2_START_ADDRESS - PLATFORM_DCT_COPY1_START_ADDRESS) - APPLICATION_DCT_WITH_APP_DCT_DATA_SIZE;
    }
#if defined(DCT_BOOTLOADER_CRC_IS_IN_HEADER)
    dct_hdr_destination->sequence++;
#endif

    return WICED_SUCCESS;
}
wiced_result_t wiced_dct_update_mfg_info_to_current (platform_dct_mfg_info_t* mfg_info_destination,
                                                     platform_dct_mfg_info_t* mfg_info_source)
{
    if ((mfg_info_destination == NULL) || (mfg_info_source == NULL))
    {
        return WICED_ERROR;
    }

    /* start by copying the data */
    memcpy(mfg_info_destination, mfg_info_source, sizeof(platform_dct_mfg_info_t));

    return WICED_SUCCESS;
}
wiced_result_t wiced_dct_update_security_to_current (platform_dct_security_t* security_credentials_destination,
                                                     platform_dct_security_t*  security_credentials_source)
{
    if ((security_credentials_destination == NULL) || (security_credentials_source == NULL))
    {
        return WICED_ERROR;
    }

    /* just copy the data - this has not changed in any SDK */
    memcpy(security_credentials_destination, security_credentials_source, sizeof(platform_dct_security_t));

    return WICED_SUCCESS;
}
wiced_result_t wiced_dct_update_security_private_key_to_current     (char*  private_destination, char*  private_source)
{
    if ((private_destination == NULL) || (private_source == NULL))
    {
        return WICED_ERROR;
    }

    /* just copy the data - this has not changed in any SDK */
    memcpy(private_destination, private_source, PRIVATE_KEY_SIZE);

    return WICED_SUCCESS;
}
wiced_result_t wiced_dct_update_security_certificate_to_current (char*  certificate_destination, char*  certificate_source)
{
    if ((certificate_destination == NULL) || (certificate_source == NULL))
    {
        return WICED_ERROR;
    }

    /* just copy the data - this has not changed in any SDK */
    memcpy(certificate_destination, certificate_source, CERTIFICATE_SIZE);

    return WICED_SUCCESS;
}
wiced_result_t wiced_dct_update_security_cooee_key_to_current   (uint8_t*  cooee_key_destination, uint8_t*  cooee_key_source)
{
    if ((cooee_key_destination == NULL) || (cooee_key_source == NULL))
    {
        return WICED_ERROR;
    }

    /* just copy the data - this has not changed in any SDK */
    memcpy(cooee_key_destination, cooee_key_source, COOEE_KEY_SIZE);

    return WICED_SUCCESS;
}

wiced_result_t wiced_dct_update_wifi_config_to_current (platform_dct_wifi_config_t* wifi_config_destination,
                                                        platform_dct_wifi_config_t* wifi_config_source)
{
    if ((wifi_config_destination == NULL) || (wifi_config_source == NULL))
    {
        return WICED_ERROR;
    }

    /* just copy the data - this has not changed in any SDK */
    memcpy(wifi_config_destination, wifi_config_source, sizeof(platform_dct_wifi_config_t));

    return WICED_SUCCESS;
}

wiced_result_t wiced_dct_update_ethernet_config_to_current (platform_dct_ethernet_config_t* ethernet_config_destination,
                                                            platform_dct_ethernet_config_t* ethernet_config_source)
{
    if ((ethernet_config_destination == NULL) || (ethernet_config_source == NULL))
    {
        return WICED_ERROR;
    }
    /* just copy the data - this has not changed in any SDK */
    memcpy(ethernet_config_destination, ethernet_config_source, sizeof(platform_dct_ethernet_config_t));
    return WICED_SUCCESS;
}

wiced_result_t wiced_dct_update_network_config_to_current (platform_dct_network_config_t* network_config_destination,
                                                           bootloader_dct_network_config_t* network_config_source)
{
    if ((network_config_destination == NULL) || (network_config_source == NULL))
    {
        return WICED_ERROR;
    }

    /*
     * Changes from SDK-3.3.0 --> SDK-3.3.1
     *
     * The change is from:
     *          char             hostname[ HOSTNAME_SIZE + 1 ];
     *          wiced_hostname_t hostname;
     *
     *  where
     *          typedef struct
     *          {
     *              char value[ HOSTNAME_SIZE + 1 ];
     *          } wiced_hostname_t;
     *
     *  sizeof(hostname) is the same for both builds, just copy the data
     */
    memcpy(network_config_destination, network_config_source, sizeof(platform_dct_network_config_t));

    return WICED_SUCCESS;
}

wiced_result_t wiced_dct_update_bt_config_to_current (platform_dct_bt_config_t* bt_config_destination,
                                                      bootloader_dct_bt_config_t* bt_config_source)
{
    if ((bt_config_destination == NULL) || (bt_config_source == NULL))
    {
        return WICED_ERROR;
    }

    /* The bootloader was built with DCT_BOOTLOADER_SDK_VERSION,
     * make sure the bt_config structure matches DCT_BOOTLOADER_SDK_CURRENT
     */
    switch (DCT_BOOTLOADER_SDK_VERSION)
    {
        case DCT_BOOTLOADER_SDK_UNKNOWN:
        case DCT_BOOTLOADER_SDK_3_0_1:
        case DCT_BOOTLOADER_SDK_3_1_0:
        case DCT_BOOTLOADER_SDK_3_1_1:
            /* no bt_config in these SDKs - nothing to do */
            break;

        case DCT_BOOTLOADER_SDK_3_1_2:
        case DCT_BOOTLOADER_SDK_3_3_0:
        case DCT_BOOTLOADER_SDK_3_3_1:
            /* Changes from SDK-3.3.1 --> SDK-3.4.0
             * platform_dct_bt_config_t
             *  added bluetooth_device_class
             *  changed padding
             */
            memcpy(bt_config_destination->bluetooth_device_address, bt_config_source->bluetooth_device_address, sizeof(bt_config_destination->bluetooth_device_address));
            memcpy(bt_config_destination->bluetooth_device_name,    bt_config_source->bluetooth_device_name,    sizeof(bt_config_destination->bluetooth_device_name));
            bt_config_destination->ssp_debug_mode                   = bt_config_source->ssp_debug_mode;
            break;

        default:
        case DCT_BOOTLOADER_SDK_3_4_0:
        case DCT_BOOTLOADER_SDK_3_5_1:
        case DCT_BOOTLOADER_SDK_3_5_2:
        case DCT_BOOTLOADER_SDK_3_6_0:
        case DCT_BOOTLOADER_SDK_3_6_1:
        case DCT_BOOTLOADER_SDK_3_6_2:
        case DCT_BOOTLOADER_SDK_CURRENT:
            /* all these SDKs match bt_config structure - just copy */
            memcpy(bt_config_destination, bt_config_source, sizeof(platform_dct_bt_config_t));
            break;
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_dct_update_p2p_config_to_current (platform_dct_p2p_config_t* p2p_config_destination,
                                                       platform_dct_p2p_config_t* p2p_config_source)
{
    if ((p2p_config_destination == NULL) || (p2p_config_source == NULL))
    {
        return WICED_ERROR;
    }

    if (DCT_BOOTLOADER_SDK_VERSION >= DCT_BOOTLOADER_SDK_3_5_1)
    {
        memcpy(p2p_config_destination, p2p_config_source, sizeof(platform_dct_p2p_config_t));
    }
    return WICED_SUCCESS;
}

wiced_result_t wiced_dct_update_ota2_config_to_current (platform_dct_ota2_config_t* ota2_config_destination,
                                                        bootloader_dct_ota2_config_t* ota2_config_source)
{
    if ((ota2_config_destination == NULL) || (ota2_config_source == NULL))
    {
        return WICED_ERROR;
    }

    if (DCT_BOOTLOADER_SDK_VERSION >= DCT_BOOTLOADER_SDK_3_5_2 )
    {
        /* padding changed to a value - struct size is the same - just copy */
        memcpy(ota2_config_destination, ota2_config_source, sizeof(platform_dct_ota2_config_t));
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_dct_update_version_to_current (platform_dct_version_t* dct_version_destination,
                                                    platform_dct_version_t* dct_version_source)
{
    if (DCT_BOOTLOADER_SDK_VERSION >= DCT_BOOTLOADER_SDK_4_0_1)
    {
        memcpy(dct_version_destination, dct_version_source, sizeof(platform_dct_version_t));
        dct_version_destination->magic_number           = DCT_VERSION_MAGIC_NUMBER;
        dct_version_destination->data_dct_usage_flags   = WICED_DCT_CONFIG_FLAGS;
        dct_version_destination->version                = DCT_BOOTLOADER_SDK_CURRENT;
        dct_version_destination->sequence++;
    }
    else
    {
        memset(dct_version_destination, 0x00, sizeof(platform_dct_version_t));
        dct_version_destination->magic_number           = DCT_VERSION_MAGIC_NUMBER;
        dct_version_destination->data_dct_usage_flags   = WICED_DCT_CONFIG_FLAGS;
        dct_version_destination->version                = DCT_BOOTLOADER_SDK_CURRENT;
        dct_version_destination->sequence               = 1;
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_dct_update_misc_config_to_current(platform_dct_misc_config_t* misc_config_destination,
                                                      platform_dct_misc_config_t* misc_config_source)
{
    if ((misc_config_destination == NULL) || (misc_config_source == NULL))
    {
        return WICED_ERROR;
    }

    /* changes from SDK 4.0.1 -> 6.0.0
     * deprecated aggregate_code
     * added wifi_flags
     */
    if (DCT_BOOTLOADER_SDK_VERSION >= DCT_BOOTLOADER_SDK_5_0_1)
    {
        /* copy it */
        memcpy(misc_config_destination, misc_config_source, sizeof(platform_dct_misc_config_t));
    }
    else
    {
        /* clear it out - fields completely changed */
        memset(misc_config_destination, 0x00, sizeof(platform_dct_misc_config_t));
    }

    return WICED_SUCCESS;
}

#endif  /* !defined(BOOTLOADER) */
