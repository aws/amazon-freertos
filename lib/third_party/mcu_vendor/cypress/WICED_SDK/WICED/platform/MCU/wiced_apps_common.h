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
#pragma once

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

typedef struct
{
        uint16_t    start;
        uint16_t    count;
}app_entry_t;

typedef struct
{
        uint8_t     count;          /* Number of entries */
#ifndef BOOTLOADER_APP_LUT_NO_SECURE_FLAG
        uint8_t     secure;         /* Is this app secure (Signed/Encrypted) or not - Added in SDK-3.4.0 */
#endif
        app_entry_t sectors[8];     /* An app can have a maximum of 8 amendments (for simplicity) */
}app_header_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
#if (DCT_BOOTLOADER_SDK_VERSION >= DCT_BOOTLOADER_SDK_3_1_1)
wiced_result_t wiced_apps_get_size( const image_location_t* app_header_location, uint32_t* size );
wiced_result_t wiced_apps_set_size( image_location_t* app_header_location, uint32_t size );
wiced_result_t wiced_apps_erase   ( const image_location_t* app_header_location );
wiced_result_t wiced_apps_write   ( const image_location_t* app_header_location, const uint8_t* data, uint32_t offset, uint32_t size, uint32_t* last_erased_sector );
wiced_result_t wiced_apps_read    ( const image_location_t* app_header_location, uint8_t* data, uint32_t offset, uint32_t size );
#endif /* (DCT_BOOTLOADER_SDK_VERSION >= DCT_BOOTLOADER_SDK_3_1_1) */

#ifdef __cplusplus
} /*extern "C" */
#endif
