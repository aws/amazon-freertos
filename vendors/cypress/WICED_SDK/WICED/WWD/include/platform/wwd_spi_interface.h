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
 *  Defines the SPI part of the WICED Platform Interface.
 *
 *  Provides constants and prototypes for functions that
 *  enable WICED to use a SPI bus on a particular hardware platform.
 */

#include <stdint.h>
#include "wiced_utilities.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup platif Platform Interface
 *  @{
 */

/** @name SPI Bus Functions
 *  Functions that enable WICED to use a SPI bus
 *  on a particular hardware platform.
 */
/**@{*/

/******************************************************
 *             Function declarations
 ******************************************************/


/**
 * Transfers SPI data
 *
 * Implemented in the WICED platform interface, which is specific to the
 * platform in use.
 * WICED uses this function as a generic way to transfer data
 * across a SPI bus.
 *
 * @param dir          : Direction of transfer - Write = To Wi-Fi device,
 *                                               Read  = from Wi-Fi device
 * @param buffer       : Pointer to data buffer where either data to send is
 *                       stored, or where received data is to be stored
 * @param buffer_length : Length of the data buffer provided
 *
 */
extern wwd_result_t host_platform_spi_transfer( wwd_bus_transfer_direction_t dir, uint8_t* buffer, uint16_t buffer_length );


/**
 * SPI interrupt handler
 *
 * This function is implemented by Wiced and must be called
 * from the SPI interrupt vector
 *
 */
extern void exti_irq( void );

/** @} */
/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif
