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

#include <stdint.h>
#include "wiced_wifi.h"
#include "wiced_management.h"

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

/**
 * WICED Cooee Element Type
 */
typedef enum
{
    WICED_COOEE_SSID       = 0,
    WICED_COOEE_WPA_KEY    = 1,
    WICED_COOEE_IP_ADDRESS = 2,
    WICED_COOEE_WEP_KEY    = 3,
} wiced_cooee_element_type_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef void (*wiced_easy_setup_cooee_callback_t)( uint8_t* cooee_data, uint16_t data_length );
typedef void (*wiced_easy_setup_wps_callback_t)( wiced_wps_credential_t* malloced_credentials, uint16_t credential_count );

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/** Start the Cypress Easy Setup procedure utilizing Cooee
 *
 * @param[in] callback : Callback to process received Cooee data
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_easy_setup_start_cooee( wiced_easy_setup_cooee_callback_t callback );


/** Start the Cypress Easy Setup procedure utilizing Soft AP with web server
 *
 * @param[in] config  : an array of user configurable variables in configuration_entry_t format.
 *                      The array must be terminated with a "null" entry {0,0,0,0}
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_easy_setup_start_softap( const configuration_entry_t* config );


/** Start the Cypress Easy Setup procedure utilizing iAP
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_easy_setup_start_iap( void );


/** Start the Cypress Easy Setup procedure utilizing WPS
 *
 * @param[in] mode     : Indicates whether to use Push-Button (PBC) or PIN Number mode for WPS
 * @param[in] details  : Pointer to a structure containing manufacturing details
 *                       of this device
 * @param[in] password : Password for WPS PIN mode connections
 * @param[in] callback : Callback to process received credentials
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_easy_setup_start_wps( wiced_wps_mode_t mode, const wiced_wps_device_detail_t* details, char* password, wiced_easy_setup_wps_callback_t callback );


/** Stop the all Easy Setup procedures
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_easy_setup_stop( void );


#ifdef __cplusplus
} /* extern "C" */
#endif
