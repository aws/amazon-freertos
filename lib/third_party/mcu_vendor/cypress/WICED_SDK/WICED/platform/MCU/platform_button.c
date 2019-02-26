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
 * Common Button implementation for platforms that only use GPIO buttons.
 * If a platform has other, non-GPIO based buttons it must override all the functions defined within this file.
 */

#include "wiced_platform.h"
#include "platform.h"
#include "platform_button.h"
#include "gpio_button.h"
#include "wwd_debug.h"

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

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static void platform_button_state_change_callback( gpio_button_t* button, wiced_bool_t new_state );

/******************************************************
 *               Variable Definitions
 ******************************************************/

extern const gpio_button_t platform_gpio_buttons[PLATFORM_BUTTON_MAX];

static platform_button_state_change_callback_t user_callback;

/******************************************************
 *               Function Definitions
 ******************************************************/

WEAK platform_result_t platform_button_init( platform_button_t button )
{
    return (platform_result_t)gpio_button_init( &platform_gpio_buttons[ button ] );
}

WEAK platform_result_t  platform_button_deinit( platform_button_t button )
{
    return (platform_result_t)gpio_button_deinit( &platform_gpio_buttons[ button ] );
}

WEAK platform_result_t platform_button_enable( platform_button_t button )
{
    return (platform_result_t)gpio_button_enable( &platform_gpio_buttons[ button ] );
}

WEAK platform_result_t platform_button_disable( platform_button_t button )
{
    return (platform_result_t)gpio_button_disable( &platform_gpio_buttons[ button ] );
}

WEAK wiced_bool_t platform_button_get_value( platform_button_t button )
{
    return gpio_button_get_value( &platform_gpio_buttons[ button ] );
}

WEAK platform_result_t platform_button_register_state_change_callback( platform_button_state_change_callback_t callback )
{
    user_callback = callback;
    return (platform_result_t)gpio_button_register_state_change_callback( platform_button_state_change_callback );
}

static void platform_button_state_change_callback( gpio_button_t* button, wiced_bool_t new_state )
{
    user_callback( ( platform_button_t ) ARRAY_POSITION( platform_gpio_buttons, button ), new_state );
}
