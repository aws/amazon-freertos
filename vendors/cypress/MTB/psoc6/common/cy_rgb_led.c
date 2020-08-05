/***************************************************************************//**
* \file cy_rgb_led.c
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

#include "cy_rgb_led.h"
#include "cy_syspm.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* PWM period in micro seconds */
#define CY_RGB_LED_PWM_PERIOD_US          (255u)

/* RGB LED OFF  */
#define CY_RGB_LED_OFF                    (0u)

/* RGB LED ON  */
#define CY_RGB_LED_ON                     (1u)

/* TCPWM clock divider */
#define CY_RGB_LED_TCPWM_CLK_DIVIDER      (1000000u)

/* Modes to be skipped for the RGB LED during low power mode transition */
#define CY_RGB_LED_TCPWM_LP_MODE_CHECK_SKIP        (CY_SYSPM_SKIP_CHECK_FAIL | CY_SYSPM_BEFORE_TRANSITION | CY_SYSPM_AFTER_TRANSITION)

/* This is the handler function to ensure proper operation of RGB LED during
 * device power mode transition. */
static cy_en_syspm_status_t cy_rgb_led_lp_readiness(cy_stc_syspm_callback_params_t *,  cy_en_syspm_callback_mode_t);

/* TCPWM instances for RGB LED control */
static cyhal_pwm_t pwm_red_obj;
static cyhal_pwm_t pwm_green_obj;
static cyhal_pwm_t pwm_blue_obj;

/* Variables used to track the LED state (ON/OFF, Color, Brightness) */
static uint32_t led_color = CY_RGB_LED_COLOR_OFF;
static uint8_t led_brightness;
static bool rgb_led_state = CY_RGB_LED_OFF;

/* Variable to track the active logic of the RGB LED */
static bool rgb_led_active_logic = CY_RGB_LED_ACTIVE_LOW;

/* Structure with syspm configuration elements (refer to cy_syspm.h) */
static cy_stc_syspm_callback_params_t lp_param = { NULL, NULL};
static cy_stc_syspm_callback_t lp_config =
{
    &cy_rgb_led_lp_readiness,
    CY_SYSPM_DEEPSLEEP,
    CY_RGB_LED_TCPWM_LP_MODE_CHECK_SKIP,
    &lp_param,
    NULL,
    NULL,
    0
};

/*
 * This function registers the handler to take necessary actions (for the proper
 * operation of RGB LED functionality) during device power mode transition
 * (Deep Sleep to Active and vice-versa).
 */
static cy_rslt_t cy_rgb_led_register_lp_cb(void)
{
    bool result = Cy_SysPm_RegisterCallback(&lp_config);

    if (result)
    {
        return CY_RSLT_SUCCESS;
    }
    else
    {
        return CY_RSLT_RGB_LED_LP_CB_REG_FAIL;
    }
}

/*
 * This is the handler function to ensure proper operation of RGB LED during
 * device power mode transition (Deep Sleep to Active and vice-versa). This
 * low power callback function is registered as part of \ref cy_rgb_led_init
 * function.
 */
static cy_en_syspm_status_t cy_rgb_led_lp_readiness(cy_stc_syspm_callback_params_t * lp_param,  cy_en_syspm_callback_mode_t mode)
{
    cy_en_syspm_status_t retVal;

    /* Remove unused parameter warning */
    (void)lp_param;

    if (mode == CY_SYSPM_CHECK_READY && rgb_led_state == CY_RGB_LED_OFF)
    {
        retVal = CY_SYSPM_SUCCESS;
    }
    else
    {
        retVal = CY_SYSPM_FAIL;
    }

    return retVal;
}

/*
 * Initializes three TCPWMs in PWM mode for RGB LED control. If any of the TCPWMs
 * is not available (i.e. reserved by the hardware resource manager), then
 * RGB LED initialization fails.
 */
cy_rslt_t cy_rgb_led_init(cyhal_gpio_t pin_red, cyhal_gpio_t pin_green, cyhal_gpio_t pin_blue, bool led_active_logic)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;
    cyhal_clock_divider_t clk_TCPWM1;

    rgb_led_active_logic = led_active_logic;

    /* Clock divder for generating the TCPWM clock. Must be at least 1. */
    uint32_t div = (Cy_SysClk_ClkPeriGetFrequency() + CY_RGB_LED_TCPWM_CLK_DIVIDER - 1) / CY_RGB_LED_TCPWM_CLK_DIVIDER;

    /* Allocate and assign the clock for TCPWMs for RGB LED control */
    result = cyhal_hwmgr_allocate_clock(&clk_TCPWM1, CY_SYSCLK_DIV_16_BIT, true);
    if (result == CY_RSLT_SUCCESS)
    {
        Cy_SysClk_PeriphSetDivider(clk_TCPWM1.div_type, clk_TCPWM1.div_num, div - 1);
        Cy_SysClk_PeriphEnableDivider(clk_TCPWM1.div_type, clk_TCPWM1.div_num);

        /* Attempt to initialize PWM to control Red LED. */
        result = cyhal_pwm_init(&pwm_red_obj, pin_red, &clk_TCPWM1);

        if (result == CY_RSLT_SUCCESS)
        {
            /* Attempt to initialize PWM to control Green LED. */
            result = cyhal_pwm_init(&pwm_green_obj, pin_green, &clk_TCPWM1);

            if (result == CY_RSLT_SUCCESS)
            {
                /* Attempt to initialize PWM to control Blue LED. */
                result = cyhal_pwm_init(&pwm_blue_obj, pin_blue, &clk_TCPWM1);

                /* If the Blue LED could not be initialized free the Green LED */
                if (result != CY_RSLT_SUCCESS)
                {
                    cyhal_pwm_free(&pwm_green_obj);
                }
            }

            /* If the Blue or Green LED could not be initialized free the Red LED */
            if (result != CY_RSLT_SUCCESS)
            {
                cyhal_pwm_free(&pwm_red_obj);
            }
        }

        if (result != CY_RSLT_SUCCESS)
        {
            /* Failed to allocated TCPWM resource for RGB LED control */
            result = CY_RSLT_RGB_LED_PWM_FAIL;
        }
        else
        {
            /* Register low power handler if all the three PWM are successfully
             * initialized.
             */
            result = cy_rgb_led_register_lp_cb();
        }
    }
    else
    {
        /* Assigning clock to TCPWM failed */
        return CY_RSLT_RGB_LED_CLK_FAIL;
    }

    return result;
}

/* Deinitialize the TCPWM instances used for RGB LED control. */
void cy_rgb_led_deinit(void)
{
    /* Deinitialize and free all the TCPWM instances used for RGB LED control */
    cyhal_pwm_free(&pwm_red_obj);
    cyhal_pwm_free(&pwm_green_obj);
    cyhal_pwm_free(&pwm_blue_obj);

    /* De-register the low power handler */
    Cy_SysPm_UnregisterCallback(&lp_config);
}

/* This function turns ON the RGB LED with specified color and brightness. */
void cy_rgb_led_on(uint32_t color, uint8_t brightness)
{
    rgb_led_state = CY_RGB_LED_ON;
    led_color = color;

    /* Turn on the PWMs */
    cyhal_pwm_start(&pwm_red_obj);
    cyhal_pwm_start(&pwm_green_obj);
    cyhal_pwm_start(&pwm_blue_obj);

    /* Set the RGB LED brightness */
    cy_rgb_led_set_brightness(brightness);
}

/* This function turns OFF the RGB LED. */
void cy_rgb_led_off(void)
{
    rgb_led_state = CY_RGB_LED_OFF;

    /* Turn off the PWMs */
    cyhal_pwm_stop(&pwm_red_obj);
    cyhal_pwm_stop(&pwm_green_obj);
    cyhal_pwm_stop(&pwm_blue_obj);
}

/*
 * This function sets the RGB LED color.
 * The brightness of each LED is varied by changing the ON duty cycle of the PWM
 * output. Using different combination of brightness for each of the RGB component,
 * different colors can be generated.
 */
void cy_rgb_led_set_color (uint32_t color)
{
    led_color = color;

    uint32_t pwm_red_pulse_width = (led_brightness * (uint8_t)(led_color >> CY_RGB_LED_RED_POS))/CY_RGB_LED_MAX_BRIGHTNESS;
    uint32_t pwm_green_pulse_width = (led_brightness * (uint8_t)(led_color >> CY_RGB_LED_GREEN_POS))/CY_RGB_LED_MAX_BRIGHTNESS;
    uint32_t pwm_blue_pulse_width = (led_brightness * (uint8_t)(led_color >> CY_RGB_LED_BLUE_POS))/CY_RGB_LED_MAX_BRIGHTNESS;

    if(rgb_led_active_logic == CY_RGB_LED_ACTIVE_LOW)
    {
        pwm_red_pulse_width = CY_RGB_LED_PWM_PERIOD_US - pwm_red_pulse_width;
        pwm_green_pulse_width = CY_RGB_LED_PWM_PERIOD_US - pwm_green_pulse_width;
        pwm_blue_pulse_width = CY_RGB_LED_PWM_PERIOD_US - pwm_blue_pulse_width;
    }

    cyhal_pwm_set_period(&pwm_red_obj, CY_RGB_LED_PWM_PERIOD_US, pwm_red_pulse_width);
    cyhal_pwm_set_period(&pwm_green_obj, CY_RGB_LED_PWM_PERIOD_US, pwm_green_pulse_width);
    cyhal_pwm_set_period(&pwm_blue_obj, CY_RGB_LED_PWM_PERIOD_US, pwm_blue_pulse_width);
}

/* This function sets the RGB LED brightness. */
void cy_rgb_led_set_brightness(uint8_t brightness)
{
    led_brightness = (brightness < CY_RGB_LED_MAX_BRIGHTNESS) ? brightness : CY_RGB_LED_MAX_BRIGHTNESS;
    cy_rgb_led_set_color (led_color);
}

/* Provides the current color of the RGB LED. */
uint32_t cy_rgb_led_get_color(void)
{
    if (rgb_led_state == CY_RGB_LED_OFF)
    {
        return CY_RGB_LED_COLOR_OFF;
    }
    else
    {
        return led_color;
    }
}

/* Provides the current brightness of the RGB LED. */
uint8_t cy_rgb_led_get_brightness(void)
{
    if (rgb_led_state == CY_RGB_LED_OFF)
    {
        return 0;
    }
    else
    {
        return led_brightness;
    }
}

/* Toggles the RGB LED on the board */
void cy_rgb_led_toggle(void)
{
    if (rgb_led_state == CY_RGB_LED_OFF)
    {
        cy_rgb_led_on(led_color, led_brightness);
    }
    else
    {
        cy_rgb_led_off();
    }
}

#if defined(__cplusplus)
}
#endif
