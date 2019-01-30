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

#include "platform_dct.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                   Constants
 ******************************************************/

/******************************************************
 *                    Macros
 ******************************************************/
#ifndef SECTOR_SIZE
#define SECTOR_SIZE (4096)
#endif

#if defined(OTA2_SUPPORT)
#define PLATFORM_DCT_COPY1_START_ADDRESS     OTA2_IMAGE_CURR_DCT_1_AREA_BASE
#else
#define PLATFORM_DCT_COPY1_START_ADDRESS     NORMAL_IMAGE_DCT_1_AREA_BASE
#endif
#ifndef PLATFORM_DCT_COPY1_SIZE
#define PLATFORM_DCT_COPY1_SIZE              ( 4 * SECTOR_SIZE )
#endif

#define PLATFORM_DCT_COPY2_START_ADDRESS     ( PLATFORM_DCT_COPY1_START_ADDRESS + PLATFORM_DCT_COPY1_SIZE )

#define PLATFORM_DCT_COPY1_START_SECTOR      ( (PLATFORM_DCT_COPY1_START_ADDRESS) / SECTOR_SIZE )
#define PLATFORM_DCT_COPY1_END_SECTOR        ( (PLATFORM_DCT_COPY1_START_ADDRESS + PLATFORM_DCT_COPY1_SIZE) / SECTOR_SIZE )
#define PLATFORM_DCT_COPY1_END_ADDRESS       (  PLATFORM_DCT_COPY1_START_ADDRESS + PLATFORM_DCT_COPY1_SIZE )

#define PLATFORM_DCT_COPY2_SIZE              ( PLATFORM_DCT_COPY1_SIZE )
#define PLATFORM_DCT_COPY2_START_SECTOR      ( (PLATFORM_DCT_COPY2_START_ADDRESS) / SECTOR_SIZE )
#define PLATFORM_DCT_COPY2_END_SECTOR        ( (PLATFORM_DCT_COPY2_START_ADDRESS + PLATFORM_DCT_COPY2_SIZE) / SECTOR_SIZE )
#define PLATFORM_DCT_COPY2_END_ADDRESS       (  PLATFORM_DCT_COPY2_START_ADDRESS + PLATFORM_DCT_COPY2_SIZE )

#define PLATFORM_SFLASH_PERIPHERAL_ID        (0)

/* Statically allocate array of size DCT_SFLASH_COPY_BUFFER_SIZE_ON_STACK
 * Used by DCT code that is common for bootloader and application
 * Recommended : 1024
 */
#define DCT_SFLASH_COPY_BUFFER_SIZE_ON_STACK 1024


/* DEFAULT APPS (eg FR and OTA) need to be loaded always. */
#define PLATFORM_DEFAULT_LOAD                ( WICED_FRAMEWORK_LOAD_ALWAYS )

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
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/* WAF platform functions */
void platform_start_app             ( uint32_t entry_point ); /* Defined in start_GCC.S */
void platform_load_app_chunk        ( const image_location_t* app_header_location, uint32_t offset, void* physical_address, uint32_t size );
void platform_load_app_chunk_from_fs( const image_location_t* app_header_location, void* file_handler, void* physical_address, uint32_t size );
void platform_erase_app_area        ( uint32_t physical_address, uint32_t size );

/*
 * Instruct the tiny bootloader to load an application when returning from Deepsleep
 *
 *   For BOOTLOADER_LOAD_MAIN_APP_FROM_EXTERNAL_LOCATION
 *       - app_idx is one of DCT_APP0_INDEX, DCT_APP1_INDEX, etc.
 *
 * @param   app_idx : application index to load
 *
 * @return  PLATFORM_SUCCESS
 *          PLATFORM_ERROR      - No Application entry in the Application LUT
 */
platform_result_t platform_deepsleep_set_boot(uint8_t app_idx);

/**
 * Generic Button / flash LED get press time
 *
 * NOTE: This uses wiced_rtos_delay_milliseconds(), and blocks the current thread until:
 * - The button is released
 * - the max_time has expired (ms)
 *
 * NOTE: You must at least call NoOS_setup_timing(); before calling this.
 *
 *        To change the granularity of the timer (default 100us), change
 *           platforms/<platform>/platform.h: PLATFORM_BUTTON_PRESS_CHECK_PERIOD
 *           NOTE: This also changes the "flashing" frequency of the LED)
 *
 *        To change the polarity of the button (default is pressed = low), change
 *           platforms/<platform>/platform.h: PLATFORM_BUTTON_PRESSED_STATE
 *
 * @param   button_index     - platform_button_t (generic button index, see platforms/<platform>/platform.c for table)
 * @param   led_index        - wiced_led_index_t (generic led index, see platfors/<platform>/platform.c for table)
 *
 * @return  ms button was held
 *
 */
uint32_t  platform_get_button_press_time ( int button_index, int led_index, uint32_t max_time );

/* Check length of time the "Factory Reset" button is pressed
 *
 * NOTES: This is used for bootloader (PLATFORM_HAS_OTA) and ota2_bootloader.
 *        You must at least call NoOS_setup_timing(); before calling this.
 *
 *        To change the button used for this test on your platform, change
 *           platforms/<platform>/platform.h: PLATFORM_FACTORY_RESET_BUTTON_INDEX
 *
 *        To change the LED used for this purpose, see the usage of PLATFORM_RED_LED_INDEX
 *           platforms/<platform>/platform.c:
 *
 * USAGE for OTA support (see <Wiced-SDK>/WICED/platform/MCU/wiced_waf_common.c::wiced_waf_check_factory_reset() )
 *            > 5 seconds - initiate Factory Reset
 *
 * USAGE for OTA2 support (Over The Air version 2 see <Wiced-SDK>/apps/waf/ota2_bootloader/ota2_bootloader.c).
 *             ~5 seconds - start SoftAP
 *            ~10 seconds - initiate Factory Reset
 *
 * param    max_time    - maximum time to wait
 *
 * returns  usecs button was held
 *
 */
uint32_t  platform_get_factory_reset_button_time ( uint32_t max_time );

#ifdef __cplusplus
} /* extern "C" */
#endif

