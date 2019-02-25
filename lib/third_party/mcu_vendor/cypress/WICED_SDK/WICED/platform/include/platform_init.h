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
 * Defines platform initialisation functions called by CRT0
 */
#pragma once
#include <stdint.h>
#include "wiced_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Main
 *
 * @param[in] : void
 * @return    : int
 *
 * @usage
 * \li Defined by RTOS or application and called by CRT0
 */
int main( void );


/**
 * Initialise the platform during wiced_init
 * @return    : result code
 *
 */
wiced_result_t wiced_platform_init( void );

/**
 * Initialise system clock(s)
 * This function includes initialisation of PLL and switching to fast clock
 *
 * @param[in] : void
 * @return    : void
 *
 * @usage
 * \li Defined internally in platforms/MCU/<MCU>/platform_init.c and called by CRT0
 * \li Weakly defined in platforms/MCU/<MCU>/platform_init.c. Users may override it as desired
 */
extern void platform_init_system_clocks( void );

/**
 * Initialise memory subsystem
 * This function initialises memory subsystem such as external RAM
 *
 * @param[in] : void
 * @return    : void
 *
 * @usage
 * \li Defined internally in platforms/MCU/<MCU>/platform_init.c and called by CRT0
 * \li Weakly defined in platforms/MCU/<MCU>/platform_init.c. Users may override it as desired
 */
extern void platform_init_memory( void );

/**
 * Initialise default MCU infrastructure
 * This function initialises default MCU infrastructure such as watchdog
 *
 * @param[in] : void
 * @return    : void
 *
 * @usage
 * \li Defined and used internally in platforms/MCU/<MCU>/platform_init.c
 */
extern void platform_init_mcu_infrastructure( void );

/**
 * Initialise connectivity module(s)
 * This function initialises and puts connectivity modules (Wi-Fi, Bluetooth, etc) into their reset state
 *
 * @param[in] : void
 * @return    : void
 *
 * @usage
 * \li Defined and used internally in platforms/MCU/<MCU>/platform_init.c
 */
extern void platform_init_connectivity_module( void );

/**
 * Initialise external devices
 * This function initialises and puts external peripheral devices on the board such as LEDs, buttons, sensors, etc into their reset state
 *
 * @param[in] : void
 * @return    : void
 *
 * @usage     :
 * \li MUST be defined in platforms/<Platform>/platform.c
 * \li Called by @ref platform_init_mcu_infrastructure()
 */
extern void platform_init_external_devices( void );

/**
 * Initialise priorities of interrupts used by the platform peripherals
 *
 * @param[in] : void
 * @return    : void
 *
 * @usage
 * \li MUST be defined in platforms/<Platform>/platform.c
 * \li Called by @ref platform_init_mcu_infrastructure()
 */
extern void platform_init_peripheral_irq_priorities( void );

/**
 * Initialise priorities of interrupts used by the RTOS
 *
 * @param[in] : void
 * @return    : void

 * @usage
 * \li MUST be defined by the RTOS
 * \li Called by @ref platform_init_mcu_infrastructure()
 */
extern void platform_init_rtos_irq_priorities( void );

/**
 * Used to run last step initialisation
 *
 * @param[in] : void
 * @return    : void
 *
 * @usage
 * \li Defined internally in platforms/MCU/<MCU>/platform_init.c and called by CRT0
 * \li Weakly defined in platforms/MCU/<MCU>/platform_init.c. Users may override it as desired
 */
extern void platform_init_complete( void );

/**
 * Used to get MCU chip revision
 *
 * @param[in] : void
 * @return    : uint8_t
 *
 * @usage
 * \li Defined internally in platforms/MCU/<MCU>/platform_init.c and called by wiced_platform_init()
 * \li Weakly defined in WICED/platform/MCU/wiced_platform_common.c. MCU may override it as desired
 */
extern uint8_t platform_get_chip_revision ( void );

#ifdef __cplusplus
} /*extern "C" */
#endif
