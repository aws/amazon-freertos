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
#include "platform_dct.h"
#include "wiced_result.h"
#include "wiced_apps_common.h"
/* THis is generated separately in ModusToolbox */
#ifdef MODUSTOOLBOX
#include "lut_addresses.h"
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#if defined ( __IAR_SYSTEMS_ICC__ )
/* Define the application look-up table as the
 * entry point variable to force the symbol into
 * the resulting image.  Simply forcing the symbol
 * into the image won't work because this file is
 * compiled and linked alone to produce a binary.
 * IAR requires an entry point to link.  The
 * look-up table is never referenced outside of this
 * file.
 */
#define WICED_APPS_LUT              _start
#define PAD_INITIALIZER             { 0, }
#else
#define WICED_APPS_LUT              wiced_apps_lut
#define PAD_INITIALIZER             { [0 ... WICED_APPS_LUT_PAD_SIZE - 1] = 0xFF }
#endif /* if defined ( __IAR_SYSTEMS_ICC__ ) */

/******************************************************
 *                    Constants
 ******************************************************/

#define SFLASH_SECTOR_SIZE          (4096)

#define WICED_APPS_LUT_PAD_SIZE \
    ( SFLASH_SECTOR_SIZE - sizeof( app_headers_array_t ) )

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct wiced_apps_lut wiced_apps_lut_t;
typedef app_header_t app_headers_array_t[8];

/******************************************************
 *                    Structures
 ******************************************************/

struct wiced_apps_lut
{
    app_headers_array_t data;
    /* Pad up to sector size. */
    uint8_t _pad[WICED_APPS_LUT_PAD_SIZE];
};

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Declarations
 ******************************************************/

#if defined ( __IAR_SYSTEMS_ICC__ )
#pragma section="wiced_apps_lut_section"
const wiced_apps_lut_t WICED_APPS_LUT @ "wiced_apps_lut_section";
#endif /* if defined ( __IAR_SYSTEMS_ICC__ ) */

/******************************************************
 *               Variables Definitions
 ******************************************************/

const wiced_apps_lut_t WICED_APPS_LUT =
{
    .data =
    {
        [DCT_FR_APP_INDEX] =
            {
                .count   = FR_APP_ENTRY_COUNT,
                .secure  = FR_APP_IS_SECURE,
                .sectors = { {FR_APP_SECTOR_START,FR_APP_SECTOR_COUNT} }
            },
        [DCT_DCT_IMAGE_INDEX] =
            {
                .count   = DCT_IMAGE_ENTRY_COUNT,
                .secure  = DCT_IMAGE_IS_SECURE,
                .sectors = { {DCT_IMAGE_SECTOR_START,DCT_IMAGE_SECTOR_COUNT} }
            },
        [DCT_OTA_APP_INDEX] =
            {
                .count   = OTA_APP_ENTRY_COUNT,
                .secure  = OTA_APP_IS_SECURE,
                .sectors = { {OTA_APP_SECTOR_START,OTA_APP_SECTOR_COUNT} }
            },
        [DCT_FILESYSTEM_IMAGE_INDEX] =
            {
                .count   = FILESYSTEM_IMAGE_ENTRY_COUNT,
                .secure  = FILESYSTEM_IMAGE_IS_SECURE,
                .sectors = { {FILESYSTEM_IMAGE_SECTOR_START,FILESYSTEM_IMAGE_SECTOR_COUNT} }
            },
        [DCT_WIFI_FIRMWARE_INDEX] =
            {
                .count   = WIFI_FIRMWARE_ENTRY_COUNT,
                .secure  = WIFI_FIRMWARE_IS_SECURE,
                .sectors = { {WIFI_FIRMWARE_SECTOR_START,WIFI_FIRMWARE_SECTOR_COUNT} }
            },
        [DCT_APP0_INDEX] =
            {
                .count   = APP0_ENTRY_COUNT,
                .secure  = APP0_IS_SECURE,
                .sectors = { {APP0_SECTOR_START,APP0_SECTOR_COUNT} }
            },
        [DCT_APP1_INDEX] =
            {
                .count   = APP1_ENTRY_COUNT,
                .secure  = APP1_IS_SECURE,
                .sectors = { {APP1_SECTOR_START,APP1_SECTOR_COUNT} }
            },
        [DCT_APP2_INDEX] =
            {
                .count   = APP2_ENTRY_COUNT,
                .secure  = APP2_IS_SECURE,
                .sectors = { {APP2_SECTOR_START,APP2_SECTOR_COUNT} }
            },
    },

    ._pad = PAD_INITIALIZER,
};


/******************************************************
 *               Function Definitions
 ******************************************************/
