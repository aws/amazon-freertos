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

/** @file
 *  Defines the Bus part of the WICED Platform Interface.
 *
 *  Provides prototypes for functions that allow WICED to start and stop
 *  the hardware communications bus for a platform.
 */

#include "network/wwd_buffer_interface.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup platif Platform Interface
 *  @{
 */

/** @name Bus Functions
 *  Functions that enable WICED to start and stop
 *  the hardware communications bus for a paritcular platform.
 */
/**@{*/

/******************************************************
 *             Function declarations
 ******************************************************/

/**
 * Initializes the WICED Bus
 *
 * Implemented in the WICED Platform interface which is specific to the
 * platform in use.
 * This function should set the bus GPIO pins to the correct modes,  set up
 * the interrupts for the WICED bus, enable power and clocks to the bus, and
 * set up the bus peripheral.
 *
 * @return WWD_SUCCESS or Error code
 */
extern wwd_result_t host_platform_bus_init( void );

/**
 * De-Initializes the WICED Bus
 *
 * Implemented in the WICED Platform interface which is specific to the
 * platform in use.
 * This function does the reverse of @ref host_platform_bus_init
 *
 * @return WWD_SUCCESS or Error code
 */
extern wwd_result_t host_platform_bus_deinit( void );


/**
 * Informs WWD of an interrupt
 *
 * This function should be called from the SDIO/SPI interrupt function
 * and usually indicates newly received data is available.
 * It wakes the WWD Thread, forcing it to check the send/receive
 *
 */
extern void wwd_thread_notify_irq( void );


/**
 * Enables the bus interrupt
 *
 * This function is called by WICED during init, once
 * the system is ready to receive interrupts
 */
extern wwd_result_t host_platform_bus_enable_interrupt( void );

/**
 * Disables the bus interrupt
 *
 * This function is called by WICED during de-init, to stop
 * the system supplying any more interrupts in preparation
 * for shutdown
 */
extern wwd_result_t host_platform_bus_disable_interrupt( void );


/**
 * Informs the platform bus implementation that a buffer has been freed
 *
 * This function is called by WICED during buffer release to allow
 * the platform to reuse the released buffer - especially where
 * a DMA chain needs to be refilled.
 */
extern void host_platform_bus_buffer_freed( wwd_buffer_dir_t direction );

/** @} */
/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif
