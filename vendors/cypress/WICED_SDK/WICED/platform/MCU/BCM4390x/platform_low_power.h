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
 *  Sleep support functions.
 *
 */

#pragma once

#include <stdint.h>
#include "platform_config.h"
#include "platform_map.h"
#include "platform_mcu_peripheral.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *             Macros
 ******************************************************/

#ifndef PLATFORM_LOW_POWER_HEADER_INCLUDED
#error "Platform header file must not be included directly, Please use wiced_low_power.h instead."
#endif

#define WICED_SLEEP_STR_EXPAND( name )                      #name

#define WICED_DEEP_SLEEP_SECTION_NAME_SAVED_VAR( name )     ".deep_sleep_saved_vars."WICED_SLEEP_STR_EXPAND( name )

#define WICED_DEEP_SLEEP_IS_WARMBOOT( ) \
    platform_mcu_powersave_is_warmboot( )

#if PLATFORM_APPS_POWERSAVE

#define WICED_DEEP_SLEEP_SAVED_VAR( var ) \
    SECTION( WICED_DEEP_SLEEP_SECTION_NAME_SAVED_VAR( var ) ) var

#define WICED_DEEP_SLEEP_IS_ENABLED( )                           1

#endif /* PLATFORM_APPS_POWERSAVE */

#define WICED_DEEP_SLEEP_IS_AON_SEGMENT( segment_addr, segment_size ) \
    ( ( (segment_addr) >= PLATFORM_SOCSRAM_CH0_AON_RAM_BASE(0x0)) && ( (segment_addr) + (segment_size) <= PLATFORM_SOCSRAM_CH0_AON_RAM_BASE(PLATFORM_SOCSRAM_AON_RAM_SIZE) ) )

/******************************************************
 *             Constants
 ******************************************************/

/******************************************************
 *             Enumerations
 ******************************************************/

/******************************************************
 *             Structures
 ******************************************************/

typedef struct
{
    uint32_t entry_point;
    uint32_t app_address;
} wiced_deep_sleep_tiny_bootloader_config_t;

/******************************************************
 *             Variables
 ******************************************************/

/******************************************************
 *             Function declarations
 ******************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif
