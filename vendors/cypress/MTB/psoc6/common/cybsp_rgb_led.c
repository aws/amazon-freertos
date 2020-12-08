/***************************************************************************//**
* \file cybsp_rgb_led.c
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

#include "cybsp_rgb_led.h"
#include "cybsp_types.h"
#include "cy_syspm.h"
#include "cyhal.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(CYBSP_LED_RGB_RED) && defined(CYBSP_LED_RGB_GREEN) && defined(CYBSP_LED_RGB_BLUE)

/* PWM period in micro seconds */
#define CYBSP_RGB_LED_PWM_PERIOD_US          (255u)

/* RGB LED OFF  */
#define CYBSP_RGB_LED_OFF                    (0u)

/* RGB LED ON  */
#define CYBSP_RGB_LED_ON                     (1u)

/* TCPWM clock divider */
#define TCPWM_CLK_DIVIDER                    (1000000u)

/* Modes to be skipped for the RGB LED during low power mode transition */
#define TCPWM_LP_MODE_CHECK_SKIP        (CY_SYSPM_SKIP_CHECK_FAIL | CY_SYSPM_BEFORE_TRANSITION | CY_SYSPM_AFTER_TRANSITION)

/* This is the handler function to ensure proper operation of RGB LED during
 * device power mode transition. */
static cy_en_syspm_status_t cybsp_rgb_led_lp_readiness(cy_stc_syspm_callback_params_t *,  cy_en_syspm_callback_mode_t);

/* TCPWM instances for RGB LED control */
static cyhal_pwm_t pwm_red_obj = { .base = NULL };
static cyhal_pwm_t pwm_green_obj = { .base = NULL };
static cyhal_pwm_t pwm_blue_obj = { .base = NULL };

/* Variables used to track the LED state (ON/OFF, Color, Brightness) */
static uint32_t led_color = CYBSP_RGB_LED_COLOR_OFF;
static uint8_t led_brightness;
static bool rgb_led_state = CYBSP_RGB_LED_OFF;

/* Structure with syspm configuration elements (refer to cy_syspm.h) */
static cy_stc_syspm_callback_params_t lp_param = { NULL, NULL};
static cy_stc_syspm_callback_t lp_config =
{
    &cybsp_rgb_led_lp_readiness,
    CY_SYSPM_DEEPSLEEP,
    TCPWM_LP_MODE_CHECK_SKIP,
    &lp_param,
    NULL,
    NULL,
    0
};

/*
 * Initializes three TCPWMs in PWM mode for RGB LED control. If any of the TCPWMs
 * is not available (i.e. reserved by the hardware resource manager), then
 * RGB LED initialization fails.
 */
cy_rslt_t cybsp_rgb_led_init(void)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    cyhal_clock_divider_t clk_TCPWM1;

    /* Clock divder for generating the TCPWM clock. Must be at least 1. */
    uint32_t div = (cy_PeriClkFreqHz + TCPWM_CLK_DIVIDER - 1) / TCPWM_CLK_DIVIDER;

    /* Allocate and assign the clock for TCPWMs for RGB LED control */
    result = cyhal_hwmgr_allocate_clock(&clk_TCPWM1, CY_SYSCLK_DIV_16_BIT, true);
    if (result == CY_RSLT_SUCCESS)
    {
        Cy_SysClk_PeriphSetDivider(clk_TCPWM1.div_type, clk_TCPWM1.div_num, div - 1);
        Cy_SysClk_PeriphEnableDivider(clk_TCPWM1.div_type, clk_TCPWM1.div_num);

        /* Initialize PWM to control Red LED. If TCPWM instance is not available
         * return failure, else success
         */
        result = cyhal_pwm_init(&pwm_red_obj, (cyhal_gpio_t)CYBSP_LED_RGB_RED, &clk_TCPWM1);

        if (result == CY_RSLT_SUCCESS)
        {
            /* Initialize PWM to control Green LED. If TCPWM instance is not available
             * return failure, else success
             */
            result = cyhal_pwm_init(&pwm_green_obj, (cyhal_gpio_t)CYBSP_LED_RGB_GREEN, &clk_TCPWM1);

            if (result == CY_RSLT_SUCCESS)
            {
                /* Initialize PWM to control Blue LED. If TCPWM instance is not available
                 * return failure, else success
                 */
                result = cyhal_pwm_init(&pwm_blue_obj, (cyhal_gpio_t)CYBSP_LED_RGB_BLUE, &clk_TCPWM1);
            }
        }
        if (result != CY_RSLT_SUCCESS)
        {
            /* If any of the RGB LED pin fails to accquire a TCPWM resource,
             * deallocate the TCPWM resources that were assigned succesfully
             */
            if (pwm_red_obj.base != NULL)
            {
                cyhal_pwm_free(&pwm_red_obj);
            }

            if (pwm_green_obj.base != NULL)
            {
                cyhal_pwm_free(&pwm_green_obj);
            }

            if (pwm_blue_obj.base != NULL)
            {
                cyhal_pwm_free(&pwm_blue_obj);
            }

            /* Failed to allocated TCPWM resource for RGB LED control */
            result = CYBSP_RSLT_RGB_LED_PWM_FAIL;
        }
    }
    else
    {
        /* Assigning clock to TCPWM failed */
        return CYBSP_RSLT_RGB_LED_CLK_FAIL;
    }

    return result;
}

/*
 * Deintializes the TCPWM instances used for RGB LED control.
 * Note: This forcibly frees up the GPIOs initialized as part of cybsp_init.
 * Before using this function, ensure that these pins are not used for any other
 * purpose in your design to avoid any unexpected behavior.
 */
void cybsp_rgb_led_accquire_pins(void)
{
    cyhal_gpio_free((cyhal_gpio_t)CYBSP_LED_RGB_RED);
    cyhal_gpio_free((cyhal_gpio_t)CYBSP_LED_RGB_GREEN);
    cyhal_gpio_free((cyhal_gpio_t)CYBSP_LED_RGB_BLUE);
}

/* Deintializes the TCPWM instances used for RGB LED control. */
void cybsp_rgb_led_deinit(void)
{
    /* Deintialize and free all the TCPWM instances used for RGB LED control */
    cyhal_pwm_free(&pwm_red_obj);
    cyhal_pwm_free(&pwm_green_obj);
    cyhal_pwm_free(&pwm_blue_obj);
}

/* This function turns ON the RGB LED with specified color and brightness. */
void cybsp_rgb_led_on(uint32_t color, uint8_t brightness)
{
    rgb_led_state = CYBSP_RGB_LED_ON;
    led_color = color;

    /* Turn on the PWMs */
    cyhal_pwm_start(&pwm_red_obj);
    cyhal_pwm_start(&pwm_green_obj);
    cyhal_pwm_start(&pwm_blue_obj);

    /* Set the RGB LED brightness */
    cybsp_rgb_led_set_brightness(brightness);
}

/* This function turns OFF the RGB LED. */
void cybsp_rgb_led_off(void)
{
    rgb_led_state = CYBSP_RGB_LED_OFF;

    /* Turn off the PWMs */
    cyhal_pwm_stop(&pwm_red_obj);
    cyhal_pwm_stop(&pwm_green_obj);
    cyhal_pwm_stop(&pwm_blue_obj);
}

/*
 * This function sets the RGB LED color.
 * The brightness of each LED is varied by changing the ON duty cycle of the PWM
 * output. Using different combination of brightness for each of the RGB component,
 * different colors can be generated. Note that, by default inverted PWM output
 * is connected to the GPIOs connected to the LEDs. Hence the pulse width is set to
 * pulse_width = CYBSP_RGB_LED_PWM_PERIOD_US - ON duty cycle
 * where ON duty cycle corresponds to the brightness level.
 * If you are using the non-inverting PWM output, then set the pulse width to
 * pulse_width = ON duty cycle
 * Note: This fucntion will only work if the RGB led pins are connected to
 * the inverting output of the TCPWMs.
 */
void cybsp_rgb_led_set_color (uint32_t color)
{
    led_color = color;
    cyhal_pwm_set_period(&pwm_red_obj, CYBSP_RGB_LED_PWM_PERIOD_US, CYBSP_RGB_LED_PWM_PERIOD_US - (led_brightness*(uint8_t)(led_color >> CYBSP_RGB_LED_RED_POS))/CYBSP_RGB_LED_MAX_BRIGHTNESS);
    cyhal_pwm_set_period(&pwm_green_obj, CYBSP_RGB_LED_PWM_PERIOD_US, CYBSP_RGB_LED_PWM_PERIOD_US - (led_brightness*(uint8_t)(led_color >> CYBSP_RGB_LED_GREEN_POS))/CYBSP_RGB_LED_MAX_BRIGHTNESS);
    cyhal_pwm_set_period(&pwm_blue_obj, CYBSP_RGB_LED_PWM_PERIOD_US, CYBSP_RGB_LED_PWM_PERIOD_US - (led_brightness*(uint8_t)(led_color >> CYBSP_RGB_LED_BLUE_POS))/CYBSP_RGB_LED_MAX_BRIGHTNESS);
}

/* This function sets the RGB LED brightness. */
void cybsp_rgb_led_set_brightness(uint8_t brightness)
{
    led_brightness = (brightness < CYBSP_RGB_LED_MAX_BRIGHTNESS) ? brightness : CYBSP_RGB_LED_MAX_BRIGHTNESS;
    cybsp_rgb_led_set_color (led_color);
}

/* Provides the current color of the RGB LED. */
uint32_t cybsp_rgb_led_get_color(void)
{
    if (rgb_led_state == CYBSP_RGB_LED_OFF)
    {
        return CYBSP_RGB_LED_COLOR_OFF;
    }
    else
    {
        return led_color;
    }
}

/* Provides the current brightness of the RGB LED. */
uint8_t cybsp_rgb_led_get_brightness(void)
{
    if (rgb_led_state == CYBSP_RGB_LED_OFF)
    {
        return 0;
    }
    else
    {
        return led_brightness;
    }
}

/* Toggles the RGB LED on the board */
void cybsp_rgb_led_toggle(void)
{
    if (rgb_led_state == CYBSP_RGB_LED_OFF)
    {
        cybsp_rgb_led_on(led_color, led_brightness);
    }
    else
    {
        cybsp_rgb_led_off();
    }
}

/*
 * This function registers the handler to take necessary actions (for the proper
 * operation of RGB LED functionality) during device power mode transition
 * (Deep Sleep to Active and vice-versa).
 */
cy_rslt_t cybsp_rgb_led_register_lp_cb(void)
{
    bool result = Cy_SysPm_RegisterCallback(&lp_config);

    if (result)
    {
        return CY_RSLT_SUCCESS;
    }
    else
    {
        return CYBSP_RSLT_RGB_LED_LP_CB_REG_FAIL;
    }
}

/*
 * This is the handler function to ensure proper operation of RGB LED during
 * device power mode transition (Deep Sleep to Active and vice-versa). Once the
 * handler is registered using cybsp_rgb_register_lp_cb(), this function is
 * automatically invoked during device power mode transition.
 */
static cy_en_syspm_status_t cybsp_rgb_led_lp_readiness(cy_stc_syspm_callback_params_t * lp_param,  cy_en_syspm_callback_mode_t mode)
{
    cy_en_syspm_status_t retVal;
    if (mode == CY_SYSPM_CHECK_READY && rgb_led_state == CYBSP_RGB_LED_OFF)
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
 * This function unregisters the callback function handling the low power
 * transition sequence for the RGB LED.
 */
cy_rslt_t cybsp_rgb_led_unregister_lp_cb(void)
{
    bool result = Cy_SysPm_UnregisterCallback(&lp_config);

    if (result)
    {
        return CY_RSLT_SUCCESS;
    }
    else
    {
        return CYBSP_RSLT_RGB_LED_LP_CB_DEREG_FAIL;
    }
}

#endif /* defined(CYBSP_LED_RGB_RED) && defined(CYBSP_LED_RGB_GREEN) && defined(CYBSP_LED_RGB_BLUE) */

#if defined(__cplusplus)
}
#endif
