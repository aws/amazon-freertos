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
 * OTA2 specific DCT functions for internal flash
 */
#if defined(OTA2_SUPPORT)
/* Only for OTA2 */
#include "string.h"
#include "wwd_assert.h"
#include "wiced_result.h"
#include "spi_flash.h"
#include "platform_dct.h"
#include "waf_platform.h"
#include "wiced_framework.h"
#include "wiced_dct_common.h"
#include "wiced_ota2_image.h"


/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#ifndef PLATFORM_DCT_COPY1_SIZE
#define PLATFORM_DCT_COPY1_SIZE (PLATFORM_DCT_COPY1_END_ADDRESS - PLATFORM_DCT_COPY1_START_ADDRESS)
#endif

#ifndef SECTOR_SIZE
#define SECTOR_SIZE ((uint32_t)(2048))
#endif
#if !defined(BOOTLOADER)

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
static const uint32_t DCT_saved_area_section_offsets[ ] =
{
    [DCT_APP_SECTION]           = sizeof( bootloader_dct_data_t ),
    [DCT_HK_INFO_SECTION]       = sizeof( bootloader_dct_data_t ),
    [DCT_SECURITY_SECTION]      = OFFSETOF( bootloader_dct_data_t, security_credentials ),
    [DCT_MFG_INFO_SECTION]      = OFFSETOF( bootloader_dct_data_t, mfg_info ),
    [DCT_WIFI_CONFIG_SECTION]   = OFFSETOF( bootloader_dct_data_t, wifi_config ),
#ifndef BOOTLOADER_NO_ETHER_CONFIG
    [DCT_ETHERNET_CONFIG_SECTION] = OFFSETOF( bootloader_dct_data_t, ethernet_config ),
#endif
#ifndef BOOTLOADER_NO_NETWORK_CONFIG
    [DCT_NETWORK_CONFIG_SECTION]  = OFFSETOF( bootloader_dct_data_t, network_config ),
#endif
#ifndef BOOTLOADER_NO_BT_CONFIG
    [DCT_BT_CONFIG_SECTION] = OFFSETOF( bootloader_dct_data_t, bt_config ),
#endif
#ifndef BOOTLOADER_NO_P2P_CONFIG
    [DCT_P2P_CONFIG_SECTION] = OFFSETOF( bootloader_dct_data_t, p2p_config ),
#endif
#ifndef BOOTLOADER_NO_OTA2_CONFIG
    [DCT_OTA2_CONFIG_SECTION] = OFFSETOF( bootloader_dct_data_t, ota2_config ),
#endif
#ifndef BOOTLOADER_NO_VERSION
    [DCT_VERSION_SECTION]        = OFFSETOF( bootloader_dct_data_t, dct_version ),
#endif
#ifndef BOOTLOADER_NO_MISC_CONFIG
    [DCT_MISC_SECTION]        = OFFSETOF( bootloader_dct_data_t, dct_misc ),
#endif
    [DCT_INTERNAL_SECTION]      = 0,
};

/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_result_t wiced_dct_ota2_save_copy( uint8_t type )
{

    /* copy current DCT to OTA2_IMAGE_APP_DCT_SAVE_AREA_BASE
     * Change the values for boot_type and update_count
     * to reflect the update type
     */

    platform_dct_ota2_config_t  ota2_dct_header;
    ota2_boot_type_t            saved_boot_type = OTA2_BOOT_NORMAL;

    /*
     * Check if saved DCT is from a FACTORY_RESET or UPDATE. If so, Don't copy over.
     * This is so that if we get multiple resets during an extract, we don't trash the user settings that
     * were initially saved on the first extraction try.
     *
     * NOTE: Application MUST save to saved DCT copy with boot type as "NORMAL" after extraction for this to work.
     *       see snip/ota2_example/ota2_test.c
     *
     *       over_the_air_2_app_restore_settings_after_update(player);
     *
     *       //Set the reboot type back to normal so we don't think we updated on the next reboot
     *       wiced_dct_ota2_save_copy( OTA2_BOOT_NORMAL );
     *
     */

    /* we are trying to save the DCT for a Factory Reset or an UPDATE - make sure we haven't done so already! */
    if (wiced_dct_ota2_read_saved_copy( &ota2_dct_header, DCT_OTA2_CONFIG_SECTION, 0, sizeof(platform_dct_ota2_config_t)) == WICED_SUCCESS)
    {
        saved_boot_type = ota2_dct_header.boot_type;
    }

    /* Load the current ota2 structure in the current DCT
     * NOTE: this over-writes the read from the save area, we got the value we wanted
     */
    if (wiced_dct_read_with_copy( (void**)&ota2_dct_header, DCT_OTA2_CONFIG_SECTION, 0, sizeof(platform_dct_ota2_config_t) ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }

    /* If we are saving an "Update" or Factory Reset" type, and
     *    If the saved DCT boot_type != NORMAL, we are in the middle of an extraction
     *       Do not copy the DCT to the saved area, as we have already done so!
     */
    if ((type != OTA2_BOOT_NORMAL) && (saved_boot_type != OTA2_BOOT_NORMAL))
    {
        /* Do not copy onto an already "saved" DCT */
        WPRINT_LIB_INFO(("Do not copy over Saved DCT.\r\n"));
        return WICED_SUCCESS;
    }

    /* update boot type */
    ota2_dct_header.boot_type  = type;
    ota2_dct_header.update_count++;

    if (wiced_dct_write( (void**)&ota2_dct_header, DCT_OTA2_CONFIG_SECTION, 0, sizeof(platform_dct_ota2_config_t) ) != WICED_SUCCESS)
    {
        WPRINT_LIB_INFO(("Update Current DCT to boot_type %d FAILED.\r\n", type));
        return WICED_ERROR;
    }

    WPRINT_LIB_INFO(("Copy Current DCT to Saved DCT.\r\n"));

    /* erase DCT copy area and copy current DCT */
    if (wiced_dct_ota2_erase_save_area_and_copy_dct( OTA2_IMAGE_APP_DCT_SAVE_AREA_BASE ) != WICED_SUCCESS)
    {
        WPRINT_LIB_INFO(("Copy Current DCT to Saved DCT FAILED.\r\n"));
        return WICED_ERROR;
    }

    return WICED_SUCCESS;

}

wiced_result_t wiced_dct_ota2_saved_copy_get_version( wiced_dct_sdk_ver_t *dct_version  )
{
    if (dct_version == NULL)
    {
        return WICED_BADARG;
    }

    /* do standard checks to see if the DCT copy is valid */
    *dct_version = wiced_dct_validate_and_determine_version(OTA2_IMAGE_APP_DCT_SAVE_AREA_BASE, OTA2_IMAGE_APP_DCT_SAVE_AREA_BASE + PLATFORM_DCT_COPY1_SIZE, NULL, NULL);

    return (*dct_version == DCT_BOOTLOADER_SDK_UNKNOWN) ? WICED_ERROR : WICED_SUCCESS;
}

wiced_result_t wiced_dct_ota2_read_saved_copy( void* info_ptr, dct_section_t section, uint32_t offset, uint32_t size )
{
    wiced_dct_sdk_ver_t     sdk_dct_version = DCT_BOOTLOADER_SDK_UNKNOWN;

    if (info_ptr == NULL)
    {
        return WICED_BADARG;
    }

/* do standard checks to see if the DCT copy is valid */
    sdk_dct_version = wiced_dct_validate_and_determine_version(OTA2_IMAGE_APP_DCT_SAVE_AREA_BASE, OTA2_IMAGE_APP_DCT_SAVE_AREA_BASE + PLATFORM_DCT_COPY1_SIZE, NULL, NULL);
    /* SDK version must be > SDK-3.5.2, as that is the first SDK with OTA2 */
    if ( (sdk_dct_version != DCT_BOOTLOADER_SDK_UNKNOWN) && (sdk_dct_version > DCT_BOOTLOADER_SDK_3_5_2))
    {
        uint32_t                curr_dct;

        /* read the portion the caller asked for */
        curr_dct = (uint32_t) OTA2_IMAGE_APP_DCT_SAVE_AREA_BASE + DCT_saved_area_section_offsets[ section ];
        memcpy(info_ptr, (char *)(curr_dct + offset), size);
        return WICED_SUCCESS;
    }

    return WICED_ERROR;
}

#endif  /* !defined(BOOTLOADER) */

#endif /* OTA2 Support */
