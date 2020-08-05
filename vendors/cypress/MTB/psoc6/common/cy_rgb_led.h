/***************************************************************************//**
* \file cy_rgb_led.h
*
* Description:
* Provides APIs for controlling the RGB LED on the Cypress kits.
*
********************************************************************************
* \copyright
* Copyright 2018-2020 Cypress Semiconductor Corporation
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
* \addtogroup group_board_libs RGB LED
* \{
*/

#pragma once

#include <stdint.h>
#include "cy_result.h"
#include "cyhal.h"

#if defined(__cplusplus)
extern "C" {
#endif

/** Error code for RGB LED operation: Peripheral clock assignment failure */
#define CY_RSLT_RGB_LED_CLK_FAIL                (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_BOARD_LIB_RGB_LED , 0))

/** Error code for RGB LED operation: PWM resource allocation failure */
#define CY_RSLT_RGB_LED_PWM_FAIL                (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_BOARD_LIB_RGB_LED , 1))

/** Error code for RGB LED operation: Low power callback function registration failure */
#define CY_RSLT_RGB_LED_LP_CB_REG_FAIL          (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_BOARD_LIB_RGB_LED , 2))

/** Error code for RGB LED operation: Low power callback function deregistration failure */
#define CY_RSLT_RGB_LED_LP_CB_DEREG_FAIL        (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_BOARD_LIB_RGB_LED , 3))

/** Bit position for Red LED */
#define CY_RGB_LED_RED_POS            (16u)

/** Bit position for Green LED */
#define CY_RGB_LED_GREEN_POS          (8u)

/** Bit position for Blue LED */
#define CY_RGB_LED_BLUE_POS           (0u)

/** 32-bit encoding for Red Color \n */
/** \b Note:
    LED Color is encoded as 0x00RRGGBB
    where RR - byte field for Red color
          GG - byte field for Green color
          BB - byte field for Blue color

    Each byte represents 256 (0x00 to 0xFF) different brightness level
    for each LED color (Red, Green, Blue). Colors can be generated varying
    these byte fields.

    For example:
    0x00FFFFFF (RR = 0xFF; GG = 0xFF BB = 0xFF) produces white color. \n
    0x00FFFF00 (RR = 0xFF; GG = 0xFF BB = 0x00) produces yellow color. \n

 */
#define CY_RGB_LED_COLOR_RED              (0xFF << CY_RGB_LED_RED_POS)
/** 32-bit encoding for Green Color. Refer to \ref CY_RGB_LED_COLOR_RED for color encoding scheme */
#define CY_RGB_LED_COLOR_GREEN            (0xFF << CY_RGB_LED_GREEN_POS)
/** 32-bit encoding for Blue Color. Refer to \ref CY_RGB_LED_COLOR_RED for color encoding scheme */
#define CY_RGB_LED_COLOR_BLUE             (0xFF << CY_RGB_LED_BLUE_POS)
/** 32-bit encoding for White Color. Refer to \ref CY_RGB_LED_COLOR_RED for color encoding scheme */
#define CY_RGB_LED_COLOR_WHITE            (CY_RGB_LED_COLOR_RED | CY_RGB_LED_COLOR_GREEN | CY_RGB_LED_COLOR_BLUE)
/** 32-bit encoding for Yellow Color. Refer to \ref CY_RGB_LED_COLOR_RED for color encoding scheme */
#define CY_RGB_LED_COLOR_YELLOW           (CY_RGB_LED_COLOR_RED | CY_RGB_LED_COLOR_GREEN)
/** 32-bit encoding for Magenta Color. Refer to \ref CY_RGB_LED_COLOR_RED for color encoding scheme */
#define CY_RGB_LED_COLOR_MAGENTA          (CY_RGB_LED_COLOR_RED | CY_RGB_LED_COLOR_BLUE)
/** Deprecated. Use CY_RGB_LED_COLOR_MAGENTA instead */
#define CY_RGB_LED_COLOR_PURPLE           (CY_RGB_LED_COLOR_MAGENTA)
/** 32-bit encoding for Cyan Color. Refer to \ref CY_RGB_LED_COLOR_RED for color encoding scheme */
#define CY_RGB_LED_COLOR_CYAN             (CY_RGB_LED_COLOR_GREEN | CY_RGB_LED_COLOR_BLUE)
/** No Color. Refer to \ref CY_RGB_LED_COLOR_RED for color encoding scheme*/
#define CY_RGB_LED_COLOR_OFF              (0x00000000)
/** Maximum LED Brightness */
#define CY_RGB_LED_MAX_BRIGHTNESS         (100u)
/** RGB LED active HIGH logic */
#define CY_RGB_LED_ACTIVE_HIGH            (true)
/** RGB LED active LOW logic */
#define CY_RGB_LED_ACTIVE_LOW             (false)

/**
 * \brief Initializes RGB LED on the board.
 * \param pin_red GPIO for Red component.
 * \param pin_green GPIO for Green component.
 * \param pin_blue GPIO for Blue component.
 * \param led_active_logic Active logic (low or high) for the RGB LED, all three
 *        LEDs must be connected in same active logic.
 * \returns CY_RSLT_SUCCESS if the initialization was successful, an error code
 *          otherwise.
 */
cy_rslt_t cy_rgb_led_init(cyhal_gpio_t pin_red, cyhal_gpio_t pin_green, cyhal_gpio_t pin_blue, bool led_active_logic);

/**
 * \brief De-initializes the RGB LED on the board.
 */
void cy_rgb_led_deinit(void);

/**
 * \brief Turns on RGB LED with specified color and brightness.
 * \param color LED color
 * \param brightness Brightness of the LED (Valid range: 0 to CY_RGB_LED_MAX_BRIGHTNESS).
 */
void cy_rgb_led_on(uint32_t color, uint8_t brightness);

/**
 * \brief Turns off the RGB LED on the board.
 */
void cy_rgb_led_off(void);

/**
 * \brief Toggles the RGB LED on the board.
 */
void cy_rgb_led_toggle(void);

/**
 * \brief Sets color of the RGB LED.
 * \param color LED color
 */
void cy_rgb_led_set_color(uint32_t color);

/**
 * \brief Returns the current color of the RGB LED.
 * \returns 4-byte encoded color.
 */
uint32_t cy_rgb_led_get_color(void);

/**
 * \brief Create a custom color code for the RGB LED.
 * \param red 1-byte value for Red component.
 * \param green 1-byte value for Green component.
 * \param blue 1-byte value for Blue component.
 * \returns 32-bit encoded color code.
 */
static inline uint32_t cy_rgb_led_create_color(uint8_t red, uint8_t green, uint8_t blue)
{
    return (((uint32_t)red) << CY_RGB_LED_RED_POS | ((uint32_t)green) << CY_RGB_LED_GREEN_POS | ((uint32_t)blue) << CY_RGB_LED_BLUE_POS);
}

/**
 * \brief Sets brightness of the RGB LED.
 * \param brightness LED brightness (Valid range: 0 to CY_RGB_LED_MAX_BRIGHTNESS)
 */
void cy_rgb_led_set_brightness(uint8_t brightness);

/**
 * \brief Returns the current brightness of the RGB LED.
 * \returns uint8_t LED brightness.
 */
uint8_t cy_rgb_led_get_brightness(void);

#if defined(__cplusplus)
}
#endif

/** \} group_board_libs */
