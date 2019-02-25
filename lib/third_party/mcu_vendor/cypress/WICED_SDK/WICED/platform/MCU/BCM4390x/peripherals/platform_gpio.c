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

#include <stdint.h>
#include <string.h>
#include "platform_constants.h"
#include "platform_peripheral.h"
#include "platform_map.h"
#include "platform_appscr4.h"
#include "wwd_rtos.h"
#include "RTOS/wwd_rtos_interface.h"
#include "wwd_assert.h"
#include "platform_pinmux.h"

typedef struct
{
    platform_pin_function_t gpio_pin_function;
    uint32_t                gpio_function_bit;
} chipcommon_gpio_function_t;

typedef struct
{
    uint8_t output_disable              ;
    uint8_t pullup_enable               ;
    uint8_t pulldown_enable             ;
    uint8_t schmitt_trigger_input_enable;
    uint8_t drive_strength              ;
    uint8_t input_disable               ;
} platform_pin_gpio_config_t;

/* Structure of runtime GPIO IRQ data */
typedef struct
{
    platform_gpio_irq_callback_t handler;  /* User callback function for this GPIO IRQ */
    void*                        arg;      /* User argument passed to callback function */
    platform_gpio_irq_trigger_t  trigger;  /* What triggers interrupt, e.g. high level, low level, raising edge, etc */
} platform_gpio_irq_data_t;

static chipcommon_gpio_function_t chipcommon_gpio_mapping[GPIO_TOTAL_PIN_NUMBERS] =
{
    {.gpio_pin_function = PIN_FUNCTION_GPIO_0,  .gpio_function_bit = 0},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_1,  .gpio_function_bit = 1},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_2,  .gpio_function_bit = 2},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_3,  .gpio_function_bit = 3},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_4,  .gpio_function_bit = 4},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_5,  .gpio_function_bit = 5},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_6,  .gpio_function_bit = 6},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_7,  .gpio_function_bit = 7},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_8,  .gpio_function_bit = 8},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_9,  .gpio_function_bit = 9},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_10, .gpio_function_bit = 10},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_11, .gpio_function_bit = 11},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_12, .gpio_function_bit = 12},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_13, .gpio_function_bit = 13},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_14, .gpio_function_bit = 14},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_15, .gpio_function_bit = 15},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_16, .gpio_function_bit = 16},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_17, .gpio_function_bit = 17},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_18, .gpio_function_bit = 18},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_19, .gpio_function_bit = 19},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_20, .gpio_function_bit = 20},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_21, .gpio_function_bit = 21},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_22, .gpio_function_bit = 22},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_23, .gpio_function_bit = 23},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_24, .gpio_function_bit = 24},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_25, .gpio_function_bit = 25},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_26, .gpio_function_bit = 26},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_27, .gpio_function_bit = 27},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_28, .gpio_function_bit = 28},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_29, .gpio_function_bit = 29},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_30, .gpio_function_bit = 30},
    {.gpio_pin_function = PIN_FUNCTION_GPIO_31, .gpio_function_bit = 31}
};

/*
 * Run-time IRQ mapping of GPIO bit and associated pin owning the GPIO IRQ line
 */
static platform_pin_t gpio_irq_mapping[GPIO_TOTAL_PIN_NUMBERS] =
{
    [0]  = PIN_MAX,
    [1]  = PIN_MAX,
    [2]  = PIN_MAX,
    [3]  = PIN_MAX,
    [4]  = PIN_MAX,
    [5]  = PIN_MAX,
    [6]  = PIN_MAX,
    [7]  = PIN_MAX,
    [8]  = PIN_MAX,
    [9]  = PIN_MAX,
    [10] = PIN_MAX,
    [11] = PIN_MAX,
    [12] = PIN_MAX,
    [13] = PIN_MAX,
    [14] = PIN_MAX,
    [15] = PIN_MAX,
    [16] = PIN_MAX,
    [17] = PIN_MAX,
    [18] = PIN_MAX,
    [19] = PIN_MAX,
    [20] = PIN_MAX,
    [21] = PIN_MAX,
    [22] = PIN_MAX,
    [23] = PIN_MAX,
    [24] = PIN_MAX,
    [25] = PIN_MAX,
    [26] = PIN_MAX,
    [27] = PIN_MAX,
    [28] = PIN_MAX,
    [29] = PIN_MAX,
    [30] = PIN_MAX,
    [31] = PIN_MAX
};

/*
 * Run-time mapping of GPIO bit and associated GPIO operating mode
 */
static platform_pin_config_t gpio_config_mapping[GPIO_TOTAL_PIN_NUMBERS] =
{
    [0]  = INPUT_HIGH_IMPEDANCE,
    [1]  = INPUT_HIGH_IMPEDANCE,
    [2]  = INPUT_HIGH_IMPEDANCE,
    [3]  = INPUT_HIGH_IMPEDANCE,
    [4]  = INPUT_HIGH_IMPEDANCE,
    [5]  = INPUT_HIGH_IMPEDANCE,
    [6]  = INPUT_HIGH_IMPEDANCE,
    [7]  = INPUT_HIGH_IMPEDANCE,
    [8]  = INPUT_HIGH_IMPEDANCE,
    [9]  = INPUT_HIGH_IMPEDANCE,
    [10] = INPUT_HIGH_IMPEDANCE,
    [11] = INPUT_HIGH_IMPEDANCE,
    [12] = INPUT_HIGH_IMPEDANCE,
    [13] = INPUT_HIGH_IMPEDANCE,
    [14] = INPUT_HIGH_IMPEDANCE,
    [15] = INPUT_HIGH_IMPEDANCE,
    [16] = INPUT_HIGH_IMPEDANCE,
    [17] = INPUT_HIGH_IMPEDANCE,
    [18] = INPUT_HIGH_IMPEDANCE,
    [19] = INPUT_HIGH_IMPEDANCE,
    [20] = INPUT_HIGH_IMPEDANCE,
    [21] = INPUT_HIGH_IMPEDANCE,
    [22] = INPUT_HIGH_IMPEDANCE,
    [23] = INPUT_HIGH_IMPEDANCE,
    [24] = INPUT_HIGH_IMPEDANCE,
    [25] = INPUT_HIGH_IMPEDANCE,
    [26] = INPUT_HIGH_IMPEDANCE,
    [27] = INPUT_HIGH_IMPEDANCE,
    [28] = INPUT_HIGH_IMPEDANCE,
    [29] = INPUT_HIGH_IMPEDANCE,
    [30] = INPUT_HIGH_IMPEDANCE,
    [31] = INPUT_HIGH_IMPEDANCE
};

/*
 * Run-time mapping of 43909 GPIO pin and associated GPIO bit, indexed using the GPIO pin.
 * This array enables fast lookup of GPIO pin in order to achieve rapid read/write of GPIO.
 */
static int8_t gpio_bit_mapping[PIN_MAX] =
{
    [PIN_GPIO_0]       = PIN_FUNCTION_UNSUPPORTED,
    [PIN_GPIO_1]       = PIN_FUNCTION_UNSUPPORTED,
    [PIN_GPIO_2]       = PIN_FUNCTION_UNSUPPORTED,
    [PIN_GPIO_3]       = PIN_FUNCTION_UNSUPPORTED,
    [PIN_GPIO_4]       = PIN_FUNCTION_UNSUPPORTED,
    [PIN_GPIO_5]       = PIN_FUNCTION_UNSUPPORTED,
    [PIN_GPIO_6]       = PIN_FUNCTION_UNSUPPORTED,
    [PIN_GPIO_7]       = PIN_FUNCTION_UNSUPPORTED,
    [PIN_GPIO_8]       = PIN_FUNCTION_UNSUPPORTED,
    [PIN_GPIO_9]       = PIN_FUNCTION_UNSUPPORTED,
    [PIN_GPIO_10]      = PIN_FUNCTION_UNSUPPORTED,
    [PIN_GPIO_11]      = PIN_FUNCTION_UNSUPPORTED,
    [PIN_GPIO_12]      = PIN_FUNCTION_UNSUPPORTED,
    [PIN_GPIO_13]      = PIN_FUNCTION_UNSUPPORTED,
    [PIN_GPIO_14]      = PIN_FUNCTION_UNSUPPORTED,
    [PIN_GPIO_15]      = PIN_FUNCTION_UNSUPPORTED,
    [PIN_GPIO_16]      = PIN_FUNCTION_UNSUPPORTED,
    [PIN_SDIO_CLK]     = PIN_FUNCTION_UNSUPPORTED,
    [PIN_SDIO_CMD]     = PIN_FUNCTION_UNSUPPORTED,
    [PIN_SDIO_DATA_0]  = PIN_FUNCTION_UNSUPPORTED,
    [PIN_SDIO_DATA_1]  = PIN_FUNCTION_UNSUPPORTED,
    [PIN_SDIO_DATA_2]  = PIN_FUNCTION_UNSUPPORTED,
    [PIN_SDIO_DATA_3]  = PIN_FUNCTION_UNSUPPORTED,
    [PIN_UART0_CTS]    = PIN_FUNCTION_UNSUPPORTED,
    [PIN_UART0_RTS]    = PIN_FUNCTION_UNSUPPORTED,
    [PIN_UART0_RXD]    = PIN_FUNCTION_UNSUPPORTED,
    [PIN_UART0_TXD]    = PIN_FUNCTION_UNSUPPORTED,
    [PIN_PWM_0]        = PIN_FUNCTION_UNSUPPORTED,
    [PIN_PWM_1]        = PIN_FUNCTION_UNSUPPORTED,
    [PIN_PWM_2]        = PIN_FUNCTION_UNSUPPORTED,
    [PIN_PWM_3]        = PIN_FUNCTION_UNSUPPORTED,
    [PIN_PWM_4]        = PIN_FUNCTION_UNSUPPORTED,
    [PIN_PWM_5]        = PIN_FUNCTION_UNSUPPORTED,
    [PIN_RF_SW_CTRL_5] = PIN_FUNCTION_UNSUPPORTED,
    [PIN_RF_SW_CTRL_6] = PIN_FUNCTION_UNSUPPORTED,
    [PIN_RF_SW_CTRL_7] = PIN_FUNCTION_UNSUPPORTED,
    [PIN_RF_SW_CTRL_8] = PIN_FUNCTION_UNSUPPORTED,
    [PIN_RF_SW_CTRL_9] = PIN_FUNCTION_UNSUPPORTED,
    [PIN_SPI_0_MISO]   = PIN_FUNCTION_UNSUPPORTED,
    [PIN_SPI_0_CLK]    = PIN_FUNCTION_UNSUPPORTED,
    [PIN_SPI_0_MOSI]   = PIN_FUNCTION_UNSUPPORTED,
    [PIN_SPI_0_CS]     = PIN_FUNCTION_UNSUPPORTED,
    [PIN_I2C0_SDATA]   = PIN_FUNCTION_UNSUPPORTED,
    [PIN_I2C0_CLK]     = PIN_FUNCTION_UNSUPPORTED,
    [PIN_I2S_MCLK0]    = PIN_FUNCTION_UNSUPPORTED,
    [PIN_I2S_SCLK0]    = PIN_FUNCTION_UNSUPPORTED,
    [PIN_I2S_LRCLK0]   = PIN_FUNCTION_UNSUPPORTED,
    [PIN_I2S_SDATAI0]  = PIN_FUNCTION_UNSUPPORTED,
    [PIN_I2S_SDATAO0]  = PIN_FUNCTION_UNSUPPORTED,
    [PIN_I2S_SDATAO1]  = PIN_FUNCTION_UNSUPPORTED,
    [PIN_I2S_SDATAI1]  = PIN_FUNCTION_UNSUPPORTED,
    [PIN_I2S_MCLK1]    = PIN_FUNCTION_UNSUPPORTED,
    [PIN_I2S_SCLK1]    = PIN_FUNCTION_UNSUPPORTED,
    [PIN_I2S_LRCLK1]   = PIN_FUNCTION_UNSUPPORTED,
    [PIN_SPI_1_CLK]    = PIN_FUNCTION_UNSUPPORTED,
    [PIN_SPI_1_MISO]   = PIN_FUNCTION_UNSUPPORTED,
    [PIN_SPI_1_MOSI]   = PIN_FUNCTION_UNSUPPORTED,
    [PIN_SPI_1_CS]     = PIN_FUNCTION_UNSUPPORTED,
    [PIN_I2C1_CLK]     = PIN_FUNCTION_UNSUPPORTED,
    [PIN_I2C1_SDATA]   = PIN_FUNCTION_UNSUPPORTED
};

/*
 * If pin bit present in this mask then:
 *     a) IRQ_TRIGGER_BOTH_EDGES is supported.
 *     b) All edge interrupts are emulated by using level interrupts
 *        and switching their polarity in driver.
 * Otherwise:
 *     a) IRQ_TRIGGER_BOTH_EDGES is NOT supported for this pin.
 *     b) IRQ_TRIGGER_RISING_EDGE and IRQ_TRIGGER_FALLING_EDGE are
 *        implemented using hardware support.
 * Configuration is per-pin. One pin can be configured to use first method
 * while another second method.
 *
 * Level interrupts are not affected by this mask,
 * so following is applied to edge interrupts only:
 *     a) If need to have guaranteed delivery, then second method
 *        should be used as it does not depend on ISR latency.
 *        E.g. if programmed as IRQ_TRIGGER_RISING_EDGE and pin has
 *        quick transition from low to high and then back, ISR would
 *        see low->high transition in case of second method and not
 *        guaranteed in case of first method.
 *        But this is for first transition only, all subsequent may be
 *        not detected if ISR was triggered too late.
 *     b) If IRQ_TRIGGER_BOTH_EDGES is needed,
 *        then first method is what suggested to use.
 *     c) First method is suggested to use with buttons as it seems
 *        better cope with button bouncing.
 */
#ifndef GPIO_EDGE_HANDLING_VIA_LEVEL_MASK
#define GPIO_EDGE_HANDLING_VIA_LEVEL_MASK    (0xFFFFFFFF)
#endif

static platform_gpio_irq_data_t gpio_irq_data[GPIO_TOTAL_PIN_NUMBERS] = {{0}};

static int
platform_pin_chipcommon_gpio_function_bit( const platform_pin_internal_config_t *pin_conf, uint32_t pin_function_index)
{
    int      cc_gpio_index = 0;
    int      cc_gpio_bit   = PIN_FUNCTION_UNSUPPORTED;

    if ((pin_conf == NULL) || (pin_function_index >= PIN_FUNCTION_MAX_COUNT))
    {
        return PIN_FUNCTION_UNSUPPORTED;
    }

    /* Lookup the ChipCommon GPIO register bit for this pin */
    for (cc_gpio_index = 0 ; cc_gpio_index < GPIO_TOTAL_PIN_NUMBERS ; cc_gpio_index++)
    {
        if (chipcommon_gpio_mapping[cc_gpio_index].gpio_pin_function == pin_conf->pin_function_selection[pin_function_index].pin_function)
        {
            cc_gpio_bit = chipcommon_gpio_mapping[cc_gpio_index].gpio_function_bit;
            return cc_gpio_bit;
        }
    }

    return PIN_FUNCTION_UNSUPPORTED;
}

static void
platform_pin_set_gpio_bit_mapping( const platform_pin_internal_config_t *pin_conf, uint32_t pin_function_index)
{
    int gpio_index = 0;
    int gpio_bit   = PIN_FUNCTION_UNSUPPORTED;

    /* Lookup the GPIO bit for this GPIO pin */
    gpio_bit = platform_pin_chipcommon_gpio_function_bit(pin_conf, pin_function_index);

    /* Clear any GPIO pin mappings for this GPIO bit */
    for (gpio_index = 0 ; gpio_index < PIN_MAX ; gpio_index++)
    {
        if (gpio_bit_mapping[gpio_index] == gpio_bit)
        {
            gpio_bit_mapping[gpio_index] = PIN_FUNCTION_UNSUPPORTED;
        }
    }

    /* Set the GPIO bit mapping for this GPIO pin */
    gpio_bit_mapping[pin_conf->pin_pad_name] = gpio_bit;
}

platform_result_t
platform_chipcommon_gpio_init( const platform_pin_internal_config_t *pin_conf, uint32_t pin_function_index, platform_pin_config_t config)
{
    uint32_t flags;
    int cc_gpio_bit = PIN_FUNCTION_UNSUPPORTED;
    platform_pin_gpio_config_t pin_gpio_conf =
    {
        .output_disable               = 0,
        .pullup_enable                = 0,
        .pulldown_enable              = 0,
        .schmitt_trigger_input_enable = 0,
        .drive_strength               = 0,
        .input_disable                = 0
    };

    if ((pin_conf == NULL) || (pin_function_index >= PIN_FUNCTION_MAX_COUNT))
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Check if the pin function selection is GPIO */
    if (pin_conf->pin_function_selection[pin_function_index].pin_function_type != PIN_FUNCTION_TYPE_GPIO)
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Setup GPIO configuration for this pin */
    switch (config)
    {
        case INPUT_PULL_UP:
            pin_gpio_conf.output_disable = 1;
            pin_gpio_conf.pullup_enable  = 1;
            break;

        case INPUT_PULL_DOWN:
            pin_gpio_conf.output_disable  = 1;
            pin_gpio_conf.pulldown_enable = 1;
            break;

        case INPUT_HIGH_IMPEDANCE:
            pin_gpio_conf.output_disable = 1;
            break;

        case OUTPUT_PUSH_PULL:
            break;

        case OUTPUT_OPEN_DRAIN_NO_PULL:
            break;

        case OUTPUT_OPEN_DRAIN_PULL_UP:
            break;

        default:
            wiced_assert( "Not supported", 0 );
            return PLATFORM_UNSUPPORTED;
    }

    /* Lookup the ChipCommon GPIO register bit for this GPIO pin function */
    cc_gpio_bit = platform_pin_chipcommon_gpio_function_bit(pin_conf, pin_function_index);

    if (cc_gpio_bit == PIN_FUNCTION_UNSUPPORTED)
    {
        return PLATFORM_UNSUPPORTED;
    }

    WICED_SAVE_INTERRUPTS(flags);

    if ( (config == OUTPUT_OPEN_DRAIN_NO_PULL) || (config == OUTPUT_OPEN_DRAIN_PULL_UP) )
    {
        /* For open-drain modes, the GPIO output is driven active-low */
        PLATFORM_CHIPCOMMON->gpio.output &= (~( 1 << cc_gpio_bit ));
    }

    /* Initialize the appropriate ChipCommon GPIO registers */
    PLATFORM_CHIPCOMMON->gpio.pull_down     = ( PLATFORM_CHIPCOMMON->gpio.pull_down     & (~( 1 << cc_gpio_bit ))) | ( (pin_gpio_conf.pulldown_enable)? (1 << cc_gpio_bit) : 0 );
    PLATFORM_CHIPCOMMON->gpio.pull_up       = ( PLATFORM_CHIPCOMMON->gpio.pull_up       & (~( 1 << cc_gpio_bit ))) | ( (pin_gpio_conf.pullup_enable)  ? (1 << cc_gpio_bit) : 0 );
    PLATFORM_CHIPCOMMON->gpio.output_enable = ( PLATFORM_CHIPCOMMON->gpio.output_enable & (~( 1 << cc_gpio_bit ))) | ( (pin_gpio_conf.output_disable) ? 0 : (1 << cc_gpio_bit) );
    PLATFORM_CHIPCOMMON->gpio.control       = ( PLATFORM_CHIPCOMMON->gpio.control       & (~( 1 << cc_gpio_bit )));

    /* Initialize the appropriate GPIO configuration */
    gpio_config_mapping[cc_gpio_bit] = config;

    WICED_RESTORE_INTERRUPTS(flags);

    return PLATFORM_SUCCESS;
}

platform_result_t
platform_chipcommon_gpio_deinit( const platform_pin_internal_config_t *pin_conf, uint32_t pin_function_index)
{
    uint32_t flags;
    int      cc_gpio_bit  = PIN_FUNCTION_UNSUPPORTED;
    uint32_t pin_func_idx = PIN_FUNCTION_MAX_COUNT;

    if ((pin_conf == NULL) || (pin_function_index >= PIN_FUNCTION_MAX_COUNT))
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Check if the pin function selection is GPIO */
    if (pin_conf->pin_function_selection[pin_function_index].pin_function_type != PIN_FUNCTION_TYPE_GPIO)
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Read the current function index value for this pin */
    platform_pinmux_function_get(pin_conf, &pin_func_idx);

    if (pin_func_idx != pin_function_index)
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Lookup the ChipCommon GPIO register bit for this GPIO pin function */
    cc_gpio_bit = platform_pin_chipcommon_gpio_function_bit(pin_conf, pin_function_index);

    if (cc_gpio_bit == PIN_FUNCTION_UNSUPPORTED)
    {
        return PLATFORM_UNSUPPORTED;
    }

    WICED_SAVE_INTERRUPTS(flags);

    /* Reset the appropriate ChipCommon GPIO registers */
    PLATFORM_CHIPCOMMON->gpio.pull_down     &= (~( 1 << cc_gpio_bit ));
    PLATFORM_CHIPCOMMON->gpio.pull_up       &= (~( 1 << cc_gpio_bit ));
    PLATFORM_CHIPCOMMON->gpio.output_enable &= (~( 1 << cc_gpio_bit ));
    PLATFORM_CHIPCOMMON->gpio.control       &= (~( 1 << cc_gpio_bit ));

    /* Reset the appropriate GPIO configuration */
    gpio_config_mapping[cc_gpio_bit] = INPUT_HIGH_IMPEDANCE;

    WICED_RESTORE_INTERRUPTS(flags);

    return PLATFORM_SUCCESS;
}

platform_result_t platform_gpio_init( const platform_gpio_t* gpio, platform_pin_config_t config )
{
    platform_result_t result = PLATFORM_UNSUPPORTED;
    uint32_t pin_function_index = PIN_FUNCTION_MAX_COUNT;
    const platform_pin_internal_config_t *pin_conf = NULL;

    if ((gpio == NULL) || (gpio->pin >= PIN_MAX))
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Lookup the desired pin internal function configuration */
    pin_conf = platform_pinmux_get_internal_config(gpio->pin);

    if ( (pin_conf == NULL) || (pin_conf->pin_pad_name != gpio->pin) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Initialize the GPIO pin function configuration */
    if ( (config != INPUT_PULL_UP) && (config != INPUT_PULL_DOWN) &&
         (config != INPUT_HIGH_IMPEDANCE) && (config != OUTPUT_PUSH_PULL) &&
         (config != OUTPUT_OPEN_DRAIN_NO_PULL) && (config != OUTPUT_OPEN_DRAIN_PULL_UP) )
    {
        wiced_assert( "Not supported", 0 );
        return PLATFORM_UNSUPPORTED;
    }

    /*
     * Iterate through the function selections supported by this pin
     * and acquire a GPIO function selection that is currently available.
     */

    for (pin_function_index = 0 ; (pin_function_index < PIN_FUNCTION_MAX_COUNT) ; pin_function_index++)
    {
        if (pin_conf->pin_function_selection[pin_function_index].pin_function_type == PIN_FUNCTION_TYPE_GPIO)
        {
            /* Try to acquire this GPIO function if currently not enabled */
            result = platform_pinmux_function_init(pin_conf, pin_function_index, config);

            if (result == PLATFORM_SUCCESS)
            {
                /* The pin was successfully initialized with this GPIO function */
                platform_pin_set_gpio_bit_mapping(pin_conf, pin_function_index);
                platform_gpio_irq_disable(gpio);
                return result;
            }
        }
    }

    return PLATFORM_UNSUPPORTED;
}

platform_result_t platform_gpio_deinit( const platform_gpio_t* gpio )
{
    platform_result_t result = PLATFORM_UNSUPPORTED;
    uint32_t pin_function_index = PIN_FUNCTION_MAX_COUNT;
    const platform_pin_internal_config_t *pin_conf = NULL;

    if ((gpio == NULL) || (gpio->pin >= PIN_MAX))
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Lookup the pin internal configuration and current function index value */
    result = platform_pinmux_get_function_config(gpio->pin, &pin_conf, &pin_function_index);

    if (result != PLATFORM_SUCCESS)
    {
        return result;
    }

    if ((pin_conf == NULL) || (pin_conf->pin_pad_name != gpio->pin) || (pin_function_index >= PIN_FUNCTION_MAX_COUNT))
    {
        return PLATFORM_UNSUPPORTED;
    }

    if (pin_conf->pin_function_selection[pin_function_index].pin_function_type != PIN_FUNCTION_TYPE_GPIO)
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Reset the pin GPIO function selection */
    result = platform_pinmux_function_deinit(pin_conf, pin_function_index);

    if (result == PLATFORM_SUCCESS)
    {
        /* The pin GPIO function selection was successfully reset */
        platform_gpio_irq_disable(gpio);
        gpio_bit_mapping[pin_conf->pin_pad_name] = PIN_FUNCTION_UNSUPPORTED;
    }

    return result;
}

platform_result_t platform_gpio_output_low( const platform_gpio_t* gpio )
{
    uint32_t flags;
    int cc_gpio_bit = PIN_FUNCTION_UNSUPPORTED;

    if ((gpio == NULL) || (gpio->pin >= PIN_MAX))
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Lookup the ChipCommon GPIO number for this pin */
    cc_gpio_bit = gpio_bit_mapping[gpio->pin];

    if ((cc_gpio_bit >= GPIO_TOTAL_PIN_NUMBERS) || (cc_gpio_bit < 0))
    {
        return PLATFORM_UNSUPPORTED;
    }

    WICED_SAVE_INTERRUPTS(flags);

    /* Drive the GPIO pin output low */
    PLATFORM_CHIPCOMMON->gpio.output &= (~( 1 << cc_gpio_bit ));

    if ( gpio_config_mapping[cc_gpio_bit] == OUTPUT_OPEN_DRAIN_NO_PULL )
    {
        PLATFORM_CHIPCOMMON->gpio.output_enable |= ( 1 << cc_gpio_bit );
    }
    else if ( gpio_config_mapping[cc_gpio_bit] == OUTPUT_OPEN_DRAIN_PULL_UP )
    {
        PLATFORM_CHIPCOMMON->gpio.output_enable |= ( 1 << cc_gpio_bit );
        PLATFORM_CHIPCOMMON->gpio.pull_up       &= (~( 1 << cc_gpio_bit ));
    }

    WICED_RESTORE_INTERRUPTS(flags);

    return PLATFORM_SUCCESS;
}

platform_result_t platform_gpio_output_high( const platform_gpio_t* gpio )
{
    uint32_t flags;
    int cc_gpio_bit = PIN_FUNCTION_UNSUPPORTED;

    if ((gpio == NULL) || (gpio->pin >= PIN_MAX))
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Lookup the ChipCommon GPIO number for this pin */
    cc_gpio_bit = gpio_bit_mapping[gpio->pin];

    if ((cc_gpio_bit >= GPIO_TOTAL_PIN_NUMBERS) || (cc_gpio_bit < 0))
    {
        return PLATFORM_UNSUPPORTED;
    }

    WICED_SAVE_INTERRUPTS(flags);

    if ( gpio_config_mapping[cc_gpio_bit] == OUTPUT_OPEN_DRAIN_NO_PULL )
    {
        PLATFORM_CHIPCOMMON->gpio.output_enable &= (~( 1 << cc_gpio_bit ));
    }
    else if ( gpio_config_mapping[cc_gpio_bit] == OUTPUT_OPEN_DRAIN_PULL_UP )
    {
        PLATFORM_CHIPCOMMON->gpio.pull_up       |= ( 1 << cc_gpio_bit );
        PLATFORM_CHIPCOMMON->gpio.output_enable &= (~( 1 << cc_gpio_bit ));
    }
    else
    {
        /* Drive the GPIO pin output high */
        PLATFORM_CHIPCOMMON->gpio.output |= ( 1 << cc_gpio_bit );
    }

    WICED_RESTORE_INTERRUPTS(flags);

    return PLATFORM_SUCCESS;
}

wiced_bool_t platform_gpio_input_get( const platform_gpio_t* gpio )
{
    wiced_bool_t gpio_input;
    int cc_gpio_bit = PIN_FUNCTION_UNSUPPORTED;

    if ((gpio == NULL) || (gpio->pin >= PIN_MAX))
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Lookup the ChipCommon GPIO number for this pin */
    cc_gpio_bit = gpio_bit_mapping[gpio->pin];

    if ((cc_gpio_bit >= GPIO_TOTAL_PIN_NUMBERS) || (cc_gpio_bit < 0))
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Get the GPIO pin input */
    gpio_input = ( ( PLATFORM_CHIPCOMMON->gpio.input & ( 1 << cc_gpio_bit ) ) == 0 ) ? WICED_FALSE : WICED_TRUE;

    return gpio_input;
}

platform_result_t platform_gpio_irq_enable( const platform_gpio_t* gpio, platform_gpio_irq_trigger_t trigger, platform_gpio_irq_callback_t handler, void* arg )
{
    wiced_bool_t level_trigger_enable;
    wiced_bool_t edge_handling_via_level;
    uint32_t cc_gpio_bit_mask;
    int cc_gpio_bit;
    uint32_t flags;

    if ((handler == NULL) || (gpio == NULL) || (gpio->pin >= PIN_MAX))
    {
        return PLATFORM_ERROR;
    }

    /* Lookup the ChipCommon GPIO number for this pin */
    cc_gpio_bit = gpio_bit_mapping[gpio->pin];

    if ((cc_gpio_bit >= GPIO_TOTAL_PIN_NUMBERS) || (cc_gpio_bit < 0))
    {
        /* GPIO pin not initialized for GPIO function */
        return PLATFORM_ERROR;
    }

    cc_gpio_bit_mask = (1 << cc_gpio_bit);
    edge_handling_via_level = (GPIO_EDGE_HANDLING_VIA_LEVEL_MASK & cc_gpio_bit_mask) ? WICED_TRUE : WICED_FALSE;

    /* Identify the GPIO interrupt trigger type */
    switch (trigger)
    {
        case IRQ_TRIGGER_BOTH_EDGES:
            if (!edge_handling_via_level)
            {
                /* Both edges are not supported without emulation via level interrupts */
                return PLATFORM_UNSUPPORTED;
            }
            /* Fall through */
        case IRQ_TRIGGER_RISING_EDGE:
        case IRQ_TRIGGER_FALLING_EDGE:
            level_trigger_enable = WICED_FALSE;
            break;

        case IRQ_TRIGGER_LEVEL_HIGH:
        case IRQ_TRIGGER_LEVEL_LOW:
            level_trigger_enable = WICED_TRUE;
            break;

        default:
            wiced_assert("bad trigger type", 0);
            return PLATFORM_UNSUPPORTED;
    }

    if ((PLATFORM_CHIPCOMMON->gpio.output_enable & cc_gpio_bit_mask) != 0)
    {
        /* GPIO pin not configured for input direction */
        return PLATFORM_ERROR;
    }

    WICED_SAVE_INTERRUPTS(flags);

    /* Disable GPIO interrupts */
    PLATFORM_CHIPCOMMON->gpio.int_mask &= ~cc_gpio_bit_mask;
    PLATFORM_CHIPCOMMON->gpio.event_int_mask &= ~cc_gpio_bit_mask;

    /* Setup the GPIO interrupt parameters */
    if (level_trigger_enable == WICED_TRUE)
    {
        if (trigger == IRQ_TRIGGER_LEVEL_HIGH)
        {
            PLATFORM_CHIPCOMMON->gpio.int_polarity &= ~cc_gpio_bit_mask;
        }
        else if (trigger == IRQ_TRIGGER_LEVEL_LOW)
        {
            PLATFORM_CHIPCOMMON->gpio.int_polarity |= cc_gpio_bit_mask;
        }
        else
        {
            wiced_assert("bad trigger type", 0);
        }

        /* Enable the GPIO level interrupt */
        PLATFORM_CHIPCOMMON->gpio.int_mask |= cc_gpio_bit_mask;
    }
    else if (edge_handling_via_level == WICED_TRUE)
    {
        /*
         * Use level interrupts to emulate edge interrupts.
         * Initial polarity is opposite to current value.
         */
        if (PLATFORM_CHIPCOMMON->gpio.input & cc_gpio_bit_mask)
        {
            PLATFORM_CHIPCOMMON->gpio.int_polarity |= cc_gpio_bit_mask;
        }
        else
        {
            PLATFORM_CHIPCOMMON->gpio.int_polarity &= ~cc_gpio_bit_mask;
        }

        /* Enable the GPIO level interrupt */
        PLATFORM_CHIPCOMMON->gpio.int_mask |= cc_gpio_bit_mask;
    }
    else
    {
        if (trigger == IRQ_TRIGGER_RISING_EDGE)
        {
            PLATFORM_CHIPCOMMON->gpio.event_int_polarity &= ~cc_gpio_bit_mask;
        }
        else if (trigger == IRQ_TRIGGER_FALLING_EDGE)
        {
            PLATFORM_CHIPCOMMON->gpio.event_int_polarity |= cc_gpio_bit_mask;
        }
        else
        {
            wiced_assert("bad trigger type", 0);
        }

        /* Clear and enable the GPIO edge interrupt */
        PLATFORM_CHIPCOMMON->gpio.event |= cc_gpio_bit_mask;
        PLATFORM_CHIPCOMMON->gpio.event_int_mask |= cc_gpio_bit_mask;
    }

    gpio_irq_mapping[cc_gpio_bit] = gpio->pin;
    gpio_irq_data[cc_gpio_bit].handler = handler;
    gpio_irq_data[cc_gpio_bit].arg = arg;
    gpio_irq_data[cc_gpio_bit].trigger = trigger;

    WICED_RESTORE_INTERRUPTS(flags);

    /* Make sure GPIO interrupts are enabled in ChipCommon interrupt mask */
    platform_common_chipcontrol(&(PLATFORM_CHIPCOMMON->interrupt.mask.raw), 0x0, CHIPCOMMON_GPIO_INT_MASK);

    /* Make sure ChipCommon Core external interrupt to APPS core is enabled */
    platform_chipcommon_enable_irq();

    return PLATFORM_SUCCESS;
}

platform_result_t platform_gpio_irq_disable( const platform_gpio_t* gpio )
{
    int cc_gpio_bit;
    uint32_t flags;

    if ((gpio == NULL) || (gpio->pin >= PIN_MAX))
    {
        return PLATFORM_ERROR;
    }

    for (cc_gpio_bit = 0 ; cc_gpio_bit < GPIO_TOTAL_PIN_NUMBERS ; cc_gpio_bit++)
    {
        if (gpio_irq_mapping[cc_gpio_bit] == gpio->pin)
        {
            WICED_SAVE_INTERRUPTS(flags);

            /* Disable the GPIO interrupt for this GPIO pin */
            PLATFORM_CHIPCOMMON->gpio.int_mask &= ~(1 << cc_gpio_bit);
            PLATFORM_CHIPCOMMON->gpio.event_int_mask &= ~(1 << cc_gpio_bit);

            gpio_irq_mapping[cc_gpio_bit] = PIN_MAX;
            gpio_irq_data[cc_gpio_bit].handler = NULL;

            WICED_RESTORE_INTERRUPTS(flags);
        }
    }

    return PLATFORM_SUCCESS;
}

void platform_gpio_irq( void )
{
    uint32_t input = PLATFORM_CHIPCOMMON->gpio.input;
    uint32_t orig_level_polarity = PLATFORM_CHIPCOMMON->gpio.int_polarity;
    uint32_t level_polarity = orig_level_polarity;
    uint32_t level_triggered = (input ^ level_polarity) & PLATFORM_CHIPCOMMON->gpio.int_mask;
    uint32_t edge_triggered = PLATFORM_CHIPCOMMON->gpio.event & PLATFORM_CHIPCOMMON->gpio.event_int_mask;
    int bit;

    if (edge_triggered != 0)
    {
        PLATFORM_CHIPCOMMON->gpio.event = edge_triggered;
    }

    for (bit = 0 ; bit < GPIO_TOTAL_PIN_NUMBERS ; bit++)
    {
        uint32_t mask = 1 << bit;

        if ((edge_triggered | level_triggered) & mask)
        {
            platform_gpio_irq_data_t* irq_data = &gpio_irq_data[bit];

            wiced_assert("must be configured as one type", ((edge_triggered & mask) != (level_triggered & mask)));
            wiced_assert("must be configured", (irq_data->handler != NULL));

            if (edge_triggered & mask)
            {
                irq_data->handler(irq_data->arg);
            }
            else
            {
                switch (irq_data->trigger)
                {
                    case IRQ_TRIGGER_LEVEL_HIGH:
                    case IRQ_TRIGGER_LEVEL_LOW:
                        irq_data->handler(irq_data->arg);
                        break;

                    case IRQ_TRIGGER_RISING_EDGE:
                        if ((input & mask) != 0)
                        {
                            irq_data->handler(irq_data->arg);
                        }
                        level_polarity ^= mask;
                        break;

                    case IRQ_TRIGGER_FALLING_EDGE:
                        if ((input & mask) == 0)
                        {
                            irq_data->handler(irq_data->arg);
                        }
                        level_polarity ^= mask;
                        break;

                    case IRQ_TRIGGER_BOTH_EDGES:
                        irq_data->handler(irq_data->arg);
                        level_polarity ^= mask;
                        break;

                    default:
                        wiced_assert("bad trigger", 0);
                        break;
                }
            }
        }
    }

    if (level_polarity != orig_level_polarity)
    {
        PLATFORM_CHIPCOMMON->gpio.int_polarity = level_polarity;
    }
}
