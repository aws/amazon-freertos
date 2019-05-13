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
 */
#pragma once

#include "wiced_result.h"
#include "wwd_buffer.h"

#include "platform_peripheral.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#define PLATFORM_ETHERNET_SPEED_ADV(mode) (1 << PLATFORM_ETHERNET_SPEED_##mode)

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    PLATFORM_ETHERNET_PHY_MII,
    PLATFORM_ETHERNET_PHY_RMII,
} platform_ethernet_phy_interface_t;

typedef enum
{
    PLATFORM_ETHERNET_SPEED_AUTO,
    PLATFORM_ETHERNET_SPEED_10FULL,
    PLATFORM_ETHERNET_SPEED_10HALF,
    PLATFORM_ETHERNET_SPEED_100FULL,
    PLATFORM_ETHERNET_SPEED_100HALF,
    PLATFORM_ETHERNET_SPEED_1000FULL,
    PLATFORM_ETHERNET_SPEED_1000HALF,
} platform_ethernet_speed_mode_t;

typedef enum
{
    PLATFORM_ETHERNET_LOOPBACK_DISABLE,
    PLATFORM_ETHERNET_LOOPBACK_DMA,
    PLATFORM_ETHERNET_LOOPBACK_PHY
} platform_ethernet_loopback_mode_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct
{
    wiced_mac_t                       mac_addr;
    uint8_t                           phy_addr;
    platform_ethernet_phy_interface_t phy_interface;
    uint16_t                          wd_period_ms;
    platform_ethernet_speed_mode_t    speed_force;
    uint32_t                          speed_adv;
} platform_ethernet_config_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

platform_result_t platform_ethernet_init                  ( void );
platform_result_t platform_ethernet_deinit                ( void );
wiced_bool_t      platform_ethernet_is_inited             ( void );
platform_result_t platform_ethernet_start                 ( void );
platform_result_t platform_ethernet_stop                  ( void );
platform_result_t platform_ethernet_send_data             ( wiced_buffer_t buffer );
platform_result_t platform_ethernet_get_config            ( platform_ethernet_config_t** config );
wiced_bool_t      platform_ethernet_is_ready_to_transceive( void );
platform_result_t platform_ethernet_set_loopback_mode     ( platform_ethernet_loopback_mode_t loopback_mode );
platform_result_t platform_ethernet_add_multicast_address   ( wiced_mac_t* mac );
platform_result_t platform_ethernet_remove_multicast_address( wiced_mac_t* mac );

#ifdef __cplusplus
} /*extern "C" */
#endif
