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
 * Defines board support package for CY943907AEVAL1F board
 */
#include "platform_config.h"
#include "platform_ethernet.h"
#include "platform_appscr4.h"
#include "platform_mfi.h"
#include "wiced_filesystem.h"
#include "wiced_platform.h"
#include "gpio_button.h"

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
 *               Variables Definitions
 ******************************************************/

/* Platform pin mapping table. Used by WICED/platform/MCU/wiced_platform_common.c.*/
const platform_gpio_t platform_gpio_pins[] =
{
    [WICED_GPIO_1          ]    = { PIN_GPIO_0 },
    [WICED_GPIO_2          ]    = { PIN_GPIO_1 },
    [WICED_GPIO_3          ]    = { PIN_GPIO_7 },
    [WICED_GPIO_4          ]    = { PIN_GPIO_8 },
    [WICED_GPIO_5          ]    = { PIN_GPIO_9 },
    [WICED_GPIO_6          ]    = { PIN_GPIO_10 },
    [WICED_GPIO_7          ]    = { PIN_GPIO_11 },
    [WICED_GPIO_8          ]    = { PIN_GPIO_12 },
    [WICED_GPIO_9          ]    = { PIN_GPIO_13 },
    [WICED_GPIO_10         ]    = { PIN_GPIO_14 },
    [WICED_GPIO_11         ]    = { PIN_GPIO_15 },
    [WICED_GPIO_12         ]    = { PIN_GPIO_16 },
    [WICED_GPIO_13         ]    = { PIN_PWM_0 },
    [WICED_GPIO_14         ]    = { PIN_PWM_1 },
    [WICED_GPIO_15         ]    = { PIN_PWM_2 },
    [WICED_GPIO_16         ]    = { PIN_PWM_3 },
    [WICED_GPIO_17         ]    = { PIN_PWM_4 },
    [WICED_GPIO_18         ]    = { PIN_PWM_5 },
    [WICED_GPIO_19         ]    = { PIN_SPI_0_MISO },
    [WICED_GPIO_20         ]    = { PIN_SPI_0_CLK },
    [WICED_GPIO_21         ]    = { PIN_SPI_0_MOSI },
    [WICED_GPIO_22         ]    = { PIN_SPI_0_CS },
    [WICED_GPIO_23         ]    = { PIN_GPIO_2 },
    [WICED_GPIO_24         ]    = { PIN_GPIO_3 },
    [WICED_GPIO_25         ]    = { PIN_GPIO_4 },
    [WICED_GPIO_26         ]    = { PIN_GPIO_5 },
    [WICED_GPIO_27         ]    = { PIN_GPIO_6 },
    [WICED_GPIO_28         ]    = { PIN_I2S_MCLK0 },
    [WICED_GPIO_29         ]    = { PIN_I2S_SCLK0 },
    [WICED_GPIO_30         ]    = { PIN_I2S_LRCLK0 },
    [WICED_GPIO_31         ]    = { PIN_I2S_SDATAI0 },
    [WICED_GPIO_32         ]    = { PIN_I2S_SDATAO0 },
    [WICED_GPIO_33         ]    = { PIN_I2S_MCLK1 },
    [WICED_GPIO_34         ]    = { PIN_I2S_SCLK1 },
    [WICED_GPIO_35         ]    = { PIN_I2S_LRCLK1 },
    [WICED_GPIO_36         ]    = { PIN_I2S_SDATAI1 },
    [WICED_GPIO_37         ]    = { PIN_I2S_SDATAO1 },
    [WICED_GPIO_38         ]    = { PIN_SPI_1_CLK },
    [WICED_GPIO_39         ]    = { PIN_SPI_1_MISO },
    [WICED_GPIO_40         ]    = { PIN_SPI_1_MOSI },
    [WICED_GPIO_41         ]    = { PIN_SPI_1_CS },
    [WICED_GPIO_42         ]    = { PIN_SDIO_CLK },
    [WICED_GPIO_43         ]    = { PIN_SDIO_CMD },
    [WICED_GPIO_44         ]    = { PIN_SDIO_DATA_0 },
    [WICED_GPIO_45         ]    = { PIN_SDIO_DATA_1 },
    [WICED_GPIO_46         ]    = { PIN_SDIO_DATA_2 },
    [WICED_GPIO_47         ]    = { PIN_SDIO_DATA_3 },
    [WICED_GPIO_48         ]    = { PIN_I2C0_SDATA },
    [WICED_GPIO_49         ]    = { PIN_I2C0_CLK },
    [WICED_GPIO_50         ]    = { PIN_I2C1_SDATA },
    [WICED_GPIO_51         ]    = { PIN_I2C1_CLK },


    [WICED_PERIPHERAL_PIN_1]    = { PIN_RF_SW_CTRL_6 },
    [WICED_PERIPHERAL_PIN_2]    = { PIN_RF_SW_CTRL_7 },
    [WICED_PERIPHERAL_PIN_3]    = { PIN_UART0_RXD },
    [WICED_PERIPHERAL_PIN_4]    = { PIN_UART0_TXD },
    [WICED_PERIPHERAL_PIN_5]    = { PIN_UART0_CTS },
    [WICED_PERIPHERAL_PIN_6]    = { PIN_UART0_RTS },
    [WICED_PERIPHERAL_PIN_7]    = { PIN_RF_SW_CTRL_8 },
    [WICED_PERIPHERAL_PIN_8]    = { PIN_RF_SW_CTRL_9 },
};

/* Ethernet configuration table. */
platform_ethernet_config_t platform_ethernet_config =
{
    .phy_addr      = 0x0,
    .phy_interface = PLATFORM_ETHERNET_PHY_MII,
    .wd_period_ms  = 1000,
    .speed_force   = PLATFORM_ETHERNET_SPEED_AUTO,
    .speed_adv     = PLATFORM_ETHERNET_SPEED_ADV(AUTO),
};

#ifdef USING_EXTERNAL_ADC
/* ADC peripherals. Used WICED/platform/MCU/wiced_platform_common.c */
const platform_adc_t platform_adc_peripherals[] =
{
    [WICED_ADC_1] = { (uint8_t)0x00, &max11615_adc_driver },
    [WICED_ADC_2] = { (uint8_t)0x01, &max11615_adc_driver },
    [WICED_ADC_3] = { (uint8_t)0x02, &max11615_adc_driver },
    [WICED_ADC_4] = { (uint8_t)0x03, &max11615_adc_driver },
    [WICED_ADC_5] = { (uint8_t)0x04, &max11615_adc_driver },
    [WICED_ADC_6] = { (uint8_t)0x05, &max11615_adc_driver },
};
#endif /* USING_EXTERNAL_ADC */

/* PWM peripherals. Used by WICED/platform/MCU/wiced_platform_common.c */
/* The PWMs can be re-assigned to different I/Os using the pin mux functionality
 * The comments below indicate which pins each PWM can be assigned to
 * In order to re-assign a PWM, just change the first argument in the entry for that row to the desired pin name
 * For example, to re-assign WICED_PWM_1 to PIN_GPIO_0, just change PIN_GPIO_10 to PIN_GPIO_0
 */
const platform_pwm_t platform_pwm_peripherals[] =
{
    [WICED_PWM_1]  = {PIN_GPIO_10,  PIN_FUNCTION_PWM0, },   /* or PIN_GPIO_0, PIN_GPIO_8,  PIN_GPIO_12, PIN_GPIO_14, PIN_GPIO_16, PIN_PWM_0   */
    [WICED_PWM_2]  = {PIN_GPIO_11,  PIN_FUNCTION_PWM1, },   /* or PIN_GPIO_1, PIN_GPIO_7,  PIN_GPIO_9,  PIN_GPIO_13, PIN_GPIO_15, PIN_PWM_1   */
    [WICED_PWM_3]  = {PIN_GPIO_16,  PIN_FUNCTION_PWM2, },   /* or PIN_GPIO_8, PIN_GPIO_0,  PIN_GPIO_10, PIN_GPIO_12, PIN_GPIO_14, PIN_PWM_2   */
    [WICED_PWM_4]  = {PIN_GPIO_15,  PIN_FUNCTION_PWM3, },   /* or PIN_GPIO_1, PIN_GPIO_7,  PIN_GPIO_9,  PIN_GPIO_11, PIN_GPIO_13, PIN_PWM_3   */
    [WICED_PWM_5]  = {PIN_PWM_4,    PIN_FUNCTION_PWM4, },   /* or PIN_GPIO_0, PIN_GPIO_8,  PIN_GPIO_10, PIN_GPIO_12, PIN_GPIO_14, PIN_GPIO_16 */
    [WICED_PWM_6]  = {PIN_GPIO_7,   PIN_FUNCTION_PWM5, },   /* or PIN_GPIO_1, PIN_GPIO_9,  PIN_GPIO_11, PIN_GPIO_13, PIN_GPIO_15, PIN_PWM_5   */
};

const platform_spi_t platform_spi_peripherals[] =
{
    [WICED_SPI_1]  =
    {
        .port                    = BCM4390X_SPI_0,
        .pin_mosi                = &platform_gpio_pins[WICED_GPIO_21],
        .pin_miso                = &platform_gpio_pins[WICED_GPIO_19],
        .pin_clock               = &platform_gpio_pins[WICED_GPIO_20],
        .pin_cs                  = &platform_gpio_pins[WICED_GPIO_22],
        .driver                  = &spi_gsio_driver,
    },

    [WICED_SPI_2]  =
    {
        .port                    = BCM4390X_SPI_1,
        .driver                  = &spi_gsio_driver,
    },
};

/* I2C peripherals. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_i2c_t platform_i2c_peripherals[] =
{
    [WICED_I2C_1] =
    {
        .port                    = BCM4390X_I2C_0,
        .pin_sda                 = &platform_gpio_pins[WICED_GPIO_48],
        .pin_scl                 = &platform_gpio_pins[WICED_GPIO_49],
        .driver                  = &i2c_gsio_driver,
    },

    [WICED_I2C_2] =
    {
        .port                    = BCM4390X_I2C_1,
        .pin_sda                 = &platform_gpio_pins[WICED_GPIO_50],
        .pin_scl                 = &platform_gpio_pins[WICED_GPIO_51],
        .driver                  = &i2c_gsio_driver,
    },
};

/* UART peripherals and runtime drivers. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_uart_t platform_uart_peripherals[] =
{
    [WICED_UART_1] = /* ChipCommon Slow UART */
    {
        .port    = UART_SLOW,
        .rx_pin  = &platform_gpio_pins[WICED_PERIPHERAL_PIN_1],
        .tx_pin  = &platform_gpio_pins[WICED_PERIPHERAL_PIN_2],
        .cts_pin = NULL,
        .rts_pin = NULL,
        .src_clk = CLOCK_ALP,
    },
    [WICED_UART_2] = /* ChipCommon Fast UART */
    {
        .port    = UART_FAST,
        .rx_pin  = &platform_gpio_pins[WICED_PERIPHERAL_PIN_3],
        .tx_pin  = &platform_gpio_pins[WICED_PERIPHERAL_PIN_4],
        .cts_pin = &platform_gpio_pins[WICED_PERIPHERAL_PIN_5],
        .rts_pin = &platform_gpio_pins[WICED_PERIPHERAL_PIN_6],
        .src_clk = CLOCK_HT,
    },
    [WICED_UART_3] = /* GCI UART */
    {
        .port    = UART_GCI,
        .rx_pin  = &platform_gpio_pins[WICED_PERIPHERAL_PIN_7],
        .tx_pin  = &platform_gpio_pins[WICED_PERIPHERAL_PIN_8],
        .cts_pin = NULL,
        .rts_pin = NULL,
        .src_clk = CLOCK_ALP,
    }
};
platform_uart_driver_t platform_uart_drivers[WICED_UART_MAX];

/* UART standard I/O configuration */
#ifndef WICED_DISABLE_STDIO
static const platform_uart_config_t stdio_config =
{
    .baud_rate    = 115200,
    .data_width   = DATA_WIDTH_8BIT,
    .parity       = NO_PARITY,
    .stop_bits    = STOP_BITS_1,
    .flow_control = FLOW_CONTROL_DISABLED,
};
#endif

const platform_i2s_port_info_t i2s_port_info[BCM4390X_I2S_MAX] =
{
    [BCM4390X_I2S_0] =
    {
        .port               = BCM4390X_I2S_0,
        .is_master          = 1,
        .irqn               = I2S0_ExtIRQn,
        .audio_pll_ch       = BCM4390X_AUDIO_PLL_MCLK1,
    },
    [BCM4390X_I2S_1] =
    {
        .port               = BCM4390X_I2S_1,
        .is_master          = 1,
        .irqn               = I2S1_ExtIRQn,
        .audio_pll_ch       = BCM4390X_AUDIO_PLL_MCLK2,
    },
};

static const platform_i2s_port_info_t i2s_slave_port_info[BCM4390X_I2S_MAX] =
{
    [BCM4390X_I2S_0] =
    {
        .port               = BCM4390X_I2S_0,
        .is_master          = 0,
    },
    [BCM4390X_I2S_1] =
    {
        .port               = BCM4390X_I2S_1,
        .is_master          = 0,
    },
};

const platform_i2s_t i2s_interfaces[WICED_I2S_MAX] =
{
    [WICED_I2S_1] =
    {
        .port_info          = &i2s_slave_port_info[BCM4390X_I2S_0],
        .stream_direction   = PLATFORM_I2S_READ,
    },
    [WICED_I2S_2] =
    {
        .port_info          = &i2s_port_info[BCM4390X_I2S_0],
        .stream_direction   = PLATFORM_I2S_WRITE,
    },
    [WICED_I2S_3] =
    {
        .port_info          = &i2s_port_info[BCM4390X_I2S_1],
        .stream_direction   = PLATFORM_I2S_READ,
    },
    [WICED_I2S_4] =
    {
        .port_info          = &i2s_port_info[BCM4390X_I2S_1],
        .stream_direction   = PLATFORM_I2S_WRITE,
    },
    [WICED_I2S_5] =
    {
        .port_info          = &i2s_port_info[BCM4390X_I2S_1],
        .stream_direction   = PLATFORM_I2S_READ,
    },
};

const platform_hibernation_t hibernation_config =
{
    .clock              = PLATFORM_HIBERNATION_CLOCK_EXTERNAL,
    .hib_ext_clock_freq = 0, /* use default settings */
    .rc_code            = PLATFORM_HIB_WAKE_CTRL_REG_RCCODE,
};

const gpio_button_t platform_gpio_buttons[] =
{
    [PLATFORM_BUTTON_1] =
    {
        .polarity   = WICED_ACTIVE_HIGH,
        .gpio       = WICED_BUTTON1,
        .trigger    = IRQ_TRIGGER_BOTH_EDGES,
    },

    [PLATFORM_BUTTON_2] =
    {
        .polarity   = WICED_ACTIVE_HIGH,
        .gpio       = WICED_BUTTON2,
        .trigger    = IRQ_TRIGGER_BOTH_EDGES,
    }
};

const wiced_gpio_t platform_gpio_leds[PLATFORM_LED_COUNT] =
{
     [WICED_LED_INDEX_1] = WICED_LED1,
     [WICED_LED_INDEX_2] = WICED_LED2,
};

/* MFI-related variables */
const wiced_i2c_device_t auth_chip_i2c_device =
{
    .port          = AUTH_IC_I2C_PORT,
    .address       = 0x11,
    .address_width = I2C_ADDRESS_WIDTH_7BIT,
    .speed_mode    = I2C_STANDARD_SPEED_MODE,
};

const platform_mfi_auth_chip_t platform_auth_chip =
{
    .i2c_device = &auth_chip_i2c_device,
    .reset_pin  = WICED_GPIO_AUTH_RST
};

#if !PLATFORM_NO_SDIO
#ifdef WICED_SDMMC_SUPPORT

/* put this extern variable to platform_mcu_peripheral.h later */
extern const wiced_block_device_driver_t sdmmc_block_device_driver;

/** SD/MMC Filesystem */

/* Initialisation data for SD/MMC device */
static wiced_block_device_init_data_t block_device_sdmmc_init_data =
{
    .base_address_offset                        = -1,  /* Value -1 requests use of first free offset */
    .maximum_size                               = 0,   /* Value 0 requests use of maximum size of DDR */
    .volatile_and_requires_format_when_mounting = WICED_FALSE,
};

/* Block device for SD/MMC device */
wiced_block_device_t block_device_sdmmc =
{
    .init_data            = &block_device_sdmmc_init_data,
    .driver               = &sdmmc_block_device_driver,
    .device_specific_data = NULL,
};
#endif /* WICED_SDMMC_SUPPORT */
#endif /* !PLATFORM_NO_SDIO */

/* Block device for USB device */
wiced_block_device_t block_device_usb =
{
    .init_data            = NULL,
    .driver               = NULL,
    .device_specific_data = NULL,    /* store media handle provided by usb stack */
};

/* List of all filesystem devices on this platform - for interactive selection - e.g. console app */
const filesystem_list_t all_filesystem_devices[] =
{
#if !PLATFORM_NO_SDIO
#ifdef WICED_SDMMC_SUPPORT
    { &block_device_sdmmc,     WICED_FILESYSTEM_HANDLE_FILEX, "SD" },
#endif /* WICED_SDMMC_SUPPORT */
#endif /* !PLATFORM_NO_SDIO */
    { &block_device_usb,     WICED_FILESYSTEM_HANDLE_FILEX_USBX, "USB" },
    { NULL, 0, NULL },
};

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

/* LEDs on this platform are active LOW */
platform_result_t platform_led_set_state(int led_index, int off_on )
{
    if ((led_index >= 0) && (led_index < PLATFORM_LED_COUNT))
    {
        switch (off_on)
        {
            case WICED_LED_OFF:
                platform_gpio_output_high( &platform_gpio_pins[platform_gpio_leds[led_index]] );
                break;
            case WICED_LED_ON:
                platform_gpio_output_low( &platform_gpio_pins[platform_gpio_leds[led_index]] );
                break;
        }
        return PLATFORM_SUCCESS;
    }
    return PLATFORM_BADARG;
}

void platform_led_init( void )
{
    /* Initialise LEDs and turn off by default */
    platform_gpio_init( &platform_gpio_pins[WICED_LED1], OUTPUT_PUSH_PULL );
    platform_gpio_init( &platform_gpio_pins[WICED_LED2], OUTPUT_PUSH_PULL );

    platform_led_set_state(WICED_LED_INDEX_1, WICED_LED_ON);
    platform_led_set_state(WICED_LED_INDEX_2, WICED_LED_OFF);
 }

void platform_init_external_devices( void )
{
    /* Initialise LEDs and turn off by default */
    platform_led_init();

    /* Initialise buttons to input by default */
    platform_gpio_init( &platform_gpio_pins[WICED_BUTTON2], INPUT_PULL_UP );
    platform_gpio_init( &platform_gpio_pins[WICED_BUTTON1], INPUT_PULL_UP );

#ifndef WICED_DISABLE_STDIO
    /* Initialise UART standard I/O */
    platform_stdio_init( &platform_uart_drivers[STDIO_UART], &platform_uart_peripherals[STDIO_UART], &stdio_config );
#endif
}

uint32_t  platform_get_button_press_time ( int button_index, int led_index, uint32_t max_time )
{
    int             button_gpio;
    uint32_t        button_press_timer = 0;
    int             led_state = 0;

    /* Initialize input */
    button_gpio     = platform_gpio_buttons[button_index].gpio;
    platform_gpio_init( &platform_gpio_pins[ button_gpio ], INPUT_PULL_UP );

    while ( (PLATFORM_BUTTON_PRESSED_STATE == platform_gpio_input_get(&platform_gpio_pins[ button_gpio ])) )
    {
        /* wait a bit */
        host_rtos_delay_milliseconds( PLATFORM_BUTTON_PRESS_CHECK_PERIOD );

        /* Toggle LED */
        platform_led_set_state(led_index, (led_state == 0) ? WICED_LED_OFF : WICED_LED_ON);
        led_state ^= 0x01;

        /* keep track of time */
        button_press_timer += PLATFORM_BUTTON_PRESS_CHECK_PERIOD;
        if ((max_time > 0) && (button_press_timer >= max_time))
        {
            break;
        }
    }

     /* turn off the LED */
    platform_led_set_state(led_index, WICED_LED_OFF );

    return button_press_timer;
}

uint32_t  platform_get_factory_reset_button_time ( uint32_t max_time )
{
    return platform_get_button_press_time ( PLATFORM_FACTORY_RESET_BUTTON_INDEX, PLATFORM_RED_LED_INDEX, max_time );
}
