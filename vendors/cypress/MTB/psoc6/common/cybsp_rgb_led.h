/***************************************************************************//**
* \file cybsp_rgb_led.h
*
* Description:
* Provides APIs for controlling the RGB LED on the Cypress kits.
*
********************************************************************************
* \copyright
* Copyright 2018-2019 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/**
* \addtogroup group_bsp_rgb_led RGB LED
* \{
* Driver for controlling the state, color and brightness of the RGB LED on
* Cypress boards.
*
* \defgroup group_bsp_rgb_led_macros Macros
* \defgroup group_bsp_rgb_led_functions Functions
*/

#pragma once

#include <stdint.h>
#include "cy_result.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
* \addtogroup group_bsp_rgb_led_macros
* \{
*/

/** Error code for RGB LED operation: Peripheral clock assignment failure */
#define CYBSP_RSLT_RGB_LED_CLK_FAIL                (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_ABSTRACTION_BSP, 0))

/** Error code for RGB LED operation: TCPWM resource allocation failure */
#define CYBSP_RSLT_RGB_LED_PWM_FAIL                (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_ABSTRACTION_BSP, 1))

/** Error code for RGB LED operation: Low power callback function registration failure */
#define CYBSP_RSLT_RGB_LED_LP_CB_REG_FAIL          (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_ABSTRACTION_BSP, 2))

/** Error code for RGB LED operation: Low power callback function deregistration failure */
#define CYBSP_RSLT_RGB_LED_LP_CB_DEREG_FAIL        (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_ABSTRACTION_BSP, 3))

/** Bit position for Red LED */
#define CYBSP_RGB_LED_RED_POS            (0u)

/** Bit position for Green LED */
#define CYBSP_RGB_LED_GREEN_POS          (8u)

/** Bit position for Blue LED */
#define CYBSP_RGB_LED_BLUE_POS           (16u)

/** 32-bit encoding for Red Color \n */
/** \b Note:
    LED Color is encoded as 0x00BBGGRR
    where BB - byte field for Blue color
          GG - byte field for Green color
          RR - byte field for Red color

    Each byte represents 256 (0x00 to 0xFF) different brightness level
    for each LED color (Red, Green, Blue). Colors can be generated varying
    these byte fields.

    For example:
    0x00FFFFFF (RR = 0xFF; GG = 0xFF BB = 0xFF) produces white color. \n
    0x0000FFFF (RR = 0xFF; GG = 0xFF BB = 0x00) produces yellow color. \n

 */
#define CYBSP_RGB_LED_COLOR_RED              0x000000FF
/** 32-bit encoding for Green Color. Refer to \ref CYBSP_RGB_LED_COLOR_RED for color encoding scheme */
#define CYBSP_RGB_LED_COLOR_GREEN            0x0000FF00
/** 32-bit encoding for Blue Color. Refer to \ref CYBSP_RGB_LED_COLOR_RED for color encoding scheme */
#define CYBSP_RGB_LED_COLOR_BLUE             0x00FF0000
/** 32-bit encoding for White Color. Refer to \ref CYBSP_RGB_LED_COLOR_RED for color encoding scheme */
#define CYBSP_RGB_LED_COLOR_WHITE            0x00FFFFFF
/** 32-bit encoding for Yellow Color. Refer to \ref CYBSP_RGB_LED_COLOR_RED for color encoding scheme */
#define CYBSP_RGB_LED_COLOR_YELLOW           0x0000FFFF
/** 32-bit encoding for Purple Color. Refer to \ref CYBSP_RGB_LED_COLOR_RED for color encoding scheme */
#define CYBSP_RGB_LED_COLOR_PURPLE           0x00FF00FF
/** 32-bit encoding for Cyan Color. Refer to \ref CYBSP_RGB_LED_COLOR_RED for color encoding scheme */
#define CYBSP_RGB_LED_COLOR_CYAN             0x00FFFF00
/** No Color. Refer to \ref CYBSP_RGB_LED_COLOR_RED for color encoding scheme*/
#define CYBSP_RGB_LED_COLOR_OFF              0x00000000
/** Maximum LED Brightness */
#define CYBSP_RGB_LED_MAX_BRIGHTNESS         (100u)

/** \} group_bsp_rgb_led_macros */

/**
 * \addtogroup group_bsp_rgb_led_functions
 * \{
 */

/**
 * \brief Initializes RGB LED on the board.
 *
 * \b Note: By default, cybsp_init() reserves the CYBSP_LED_RGB_RED,
 * CYBSP_LED_RGB_GREEN and CYBSP_LED_RGB_BLUE pins. To use these pins
 * for RGB LED functionality, they must be freed by calling
 * \ref cybsp_rgb_led_acquire_pins before calling the cybsp_rgb_led_init().
 * \returns CY_RSLT_SUCCESS if the initialization was successful, an error code
 *          otherwise.
 */
cy_rslt_t cybsp_rgb_led_init(void);

/**
 * \brief De-initializes the RGB LED on the board.
 */
void cybsp_rgb_led_deinit(void);

/**
 * \brief Acquires the GPIOs for the RGB LED control on the board.
 * \b Note: This forcibly frees up the RGB GPIOs initialized as part of cybsp_init.
 */
void cybsp_rgb_led_acquire_pins(void);

/**
 * \brief Turns on RGB LED with specified color and brightness.
 * \param color LED color
 * \param brightness Brightness of the LED (Valid range: 0 to CYBSP_RGB_LED_MAX_BRIGHTNESS).
 */
void cybsp_rgb_led_on(uint32_t color, uint8_t brightness);

/**
 * \brief Turns off the RGB LED on the board.
 */
void cybsp_rgb_led_off(void);

/**
 * \brief Toggles the RGB LED on the board.
 */
void cybsp_rgb_led_toggle(void);

/**
 * \brief Sets color of the RGB LED.
 * \param color LED color
 */
void cybsp_rgb_led_set_color(uint32_t color);

/**
 * \brief Returns the current color of the RGB LED.
 * \returns 4-byte encoded color.
 */
uint32_t cybsp_rgb_led_get_color(void);

/**
 * \brief Create a custom color code for the RGB LED.
 * \param red 1-byte value for Red component.
 * \param green 1-byte value for Green component.
 * \param blue 1-byte value for Blue component.
 * \returns 32-bit encoded color code.
 */
static inline uint32_t cybsp_rgb_led_create_color(uint8_t red, uint8_t green, uint8_t blue)
{
    return (((uint32_t)red) << CYBSP_RGB_LED_RED_POS | ((uint32_t)green) << CYBSP_RGB_LED_GREEN_POS | ((uint32_t)blue) << CYBSP_RGB_LED_BLUE_POS);
}

/**
 * \brief Sets brightness of the RGB LED.
 * \param brightness LED brightness (Valid range: 0 to CYBSP_RGB_LED_MAX_BRIGHTNESS)
 */
void cybsp_rgb_led_set_brightness(uint8_t brightness);

/**
 * \brief Returns the current brightness of the RGB LED.
 * \returns uint8_t LED brightness.
 */
uint8_t cybsp_rgb_led_get_brightness(void);

/**
 * \brief This function registers the handler to take necessary actions (for the proper
 * operation of RGB LED functionality) during device power mode transition
 * (Deep Sleep to Active and vice-versa).
 * \returns Status of the operation.
 */
cy_rslt_t cybsp_rgb_led_register_lp_cb(void);

/**
 * \brief This function unregisters the callback function handling the low power transition
 * sequence for the RGB LED.
 * \returns Status of the operation.
 */
cy_rslt_t cybsp_rgb_led_unregister_lp_cb(void);

/** \} group_bsp_rgb_led_functions */

#if defined(__cplusplus)
}
#endif

/** \} group_bsp_rgb_led */