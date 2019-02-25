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
 *  Provides prototypes for initialization and other management functions for Wiced system
 *
 */

#ifndef INCLUDED_WWD_MANAGEMENT_H
#define INCLUDED_WWD_MANAGEMENT_H

#include "wwd_constants.h"  /* for wwd_result_t and country codes */
#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup mgmt WICED Management
 *  User functions for initialization and other management functions for the WICED system
 *  @{
 */

/******************************************************
 *             Function declarations
 ******************************************************/
/*@-exportlocal@*/

/**
 * Initialise Wi-Fi platform
 *
 * - Initialises the required parts of the hardware platform
 *   i.e. pins for SDIO/SPI, interrupt, reset, power etc.
 *
 * - Initialises the Wiced thread which arbitrates access
 *   to the SDIO/SPI bus
 *
 * @return WWD_SUCCESS if initialization is successful, Error code otherwise
 */
wwd_result_t wwd_management_wifi_platform_init( wiced_country_code_t country, wiced_bool_t resume_after_deep_sleep );

/*
 * WARNING: After setting to WICED_TRUE,
 * Call wwd_management_wifi_platform_init_halt( WICED_FALSE )
 * prior to next wwd_management_wifi_platform_init.
 * Halt Wi-Fi platform init by causing abort of firmware download loop.
 *
 * @return WWD_SUCCESS if successfully set the flag to abort
 */
wwd_result_t wwd_management_wifi_platform_init_halt( wiced_bool_t halt );

/**
 * Turn on the Wi-Fi device
 *
 * - Initialise Wi-Fi platform
 *
 * - Program various WiFi parameters and modes
 *
 * @return WWD_SUCCESS if initialization is successful, error code otherwise
 */
wwd_result_t wwd_management_wifi_on( wiced_country_code_t country );

/**
 * Turn off the Wi-Fi device
 *
 * - De-Initialises the required parts of the hardware platform
 *   i.e. pins for SDIO/SPI, interrupt, reset, power etc.
 *
 * - De-Initialises the Wiced thread which arbitrates access
 *   to the SDIO/SPI bus
 *
 * @return WWD_SUCCESS if deinitialization is successful, Error code otherwise
 */
wwd_result_t wwd_management_wifi_off( void );

/*@+exportlocal@*/

/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* ifndef INCLUDED_WWD_MANAGEMENT_H */
