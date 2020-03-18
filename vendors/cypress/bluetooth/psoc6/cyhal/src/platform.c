/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 * Defines board support package for CY8CKIT_062 board
 */

#include "platform.h"
#include "platform_peripheral.h"
#include "platform_bluetooth.h"


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

/******************************************************
 *               Variable Definitions
 ******************************************************/

/* GPIO pin table. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_gpio_t platform_gpio_pins[] =
{
    [WICED_GPIO_21] = { .port_num = 3,  .pin_num = (0U), .hsiom = P3_0_SCB2_UART_RX},
    [WICED_GPIO_22] = { .port_num = 3,  .pin_num = (1U), .hsiom = P3_1_SCB2_UART_TX},
    [WICED_GPIO_23] = { .port_num = 3,  .pin_num = (2U), .hsiom = P3_2_SCB2_UART_RTS},
    [WICED_GPIO_24] = { .port_num = 3,  .pin_num = (3U), .hsiom = P3_3_SCB2_UART_CTS},
    [WICED_GPIO_25] = { .port_num = 3,  .pin_num = (4U), .hsiom = P3_4_GPIO},
    [WICED_GPIO_26] = { .port_num = 3,  .pin_num = (5U), .hsiom = P3_5_GPIO},
    [WICED_GPIO_27] = { .port_num = 4,  .pin_num = (0U), .hsiom = P4_0_GPIO},
};

DEFINE_PLATFORM_CLOCK_DIV_16( PCLK_SCB2_CLOCK, 2 );

/* SCB port assignments. */
const platform_scb_t platform_scb2 = { .scb_base = SCB2, .pclk = PLATFORM_CLOCK_DIV_PTR( PCLK_SCB2_CLOCK ), .irq_num = scb_2_interrupt_IRQn  };

/* UART peripherals and runtime drivers. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_uart_t platform_uart_peripherals[] =
{
    [WICED_UART_3] =
    {
        .port    = 2,
        .scb     = &platform_scb2,
        .rx_pin  = &platform_gpio_pins[WICED_GPIO_21], /* UART RX pin */
        .tx_pin  = &platform_gpio_pins[WICED_GPIO_22], /* UART TX pin */
        .cts_pin = &platform_gpio_pins[WICED_GPIO_24], /* UART CTS pin */
        .rts_pin = &platform_gpio_pins[WICED_GPIO_23], /* UART RTS pin */
    }
};

platform_uart_driver_t platform_uart_drivers[WICED_UART_MAX];

/* Bluetooth control pins. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
const platform_gpio_t* wiced_bt_control_pins[] =
{
    /* Reset pin unavailable */
    [WICED_BT_PIN_POWER      ] = &platform_gpio_pins[WICED_GPIO_25],
    [WICED_BT_PIN_HOST_WAKE  ] = &platform_gpio_pins[WICED_GPIO_26],
    [WICED_BT_PIN_DEVICE_WAKE] = &platform_gpio_pins[WICED_GPIO_27],
    [WICED_BT_PIN_RESET      ] = NULL,
};

/* Bluetooth UART pins. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
const platform_gpio_t* wiced_bt_uart_pins[] =
{
    [WICED_BT_PIN_UART_TX ] = &platform_gpio_pins[WICED_GPIO_22],
    [WICED_BT_PIN_UART_RX ] = &platform_gpio_pins[WICED_GPIO_21],
    [WICED_BT_PIN_UART_CTS] = &platform_gpio_pins[WICED_GPIO_24],
    [WICED_BT_PIN_UART_RTS] = &platform_gpio_pins[WICED_GPIO_23],
};

/* Bluetooth UART peripheral and runtime driver. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
const platform_uart_t*  wiced_bt_uart_peripheral = &platform_uart_peripherals[WICED_UART_3];
platform_uart_driver_t* wiced_bt_uart_driver     = &platform_uart_drivers    [WICED_UART_3];

/* Bluetooth UART configuration. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
const platform_uart_config_t wiced_bt_uart_config =
{
    .baud_rate    = 115200,
    .data_width   = DATA_WIDTH_8BIT,
    .parity       = NO_PARITY,
    .stop_bits    = STOP_BITS_1,
    .flow_control = FLOW_CONTROL_CTS_RTS,
};

/* BT chip specific configuration information */
const platform_bluetooth_config_t wiced_bt_config =
{
    .patchram_download_mode      = PATCHRAM_DOWNLOAD_MODE_MINIDRV_CMD,
    .patchram_download_baud_rate = 115200,
    .featured_baud_rate          = 115200
};

/******************************************************
 *               Function Definitions
 ******************************************************/

void platform_init_peripheral_irq_priorities( void )
{
    /* Interrupt priority setup. Called by WICED/platform/MCU/PSoC6/platform_init.c */
    NVIC_SetPriority( cpuss_interrupts_dw1_1_IRQn, 3 ); /* WLAN SDIO DMA */
    NVIC_SetPriority( cpuss_interrupts_dw1_3_IRQn, 3 ); /* WLAN SDIO DMA */

    NVIC_SetPriority( csd_interrupt_IRQn,  5 );         /* CSD */
    NVIC_SetPriority( scb_0_interrupt_IRQn, 6 );        /* SCB 0 */
    NVIC_SetPriority( scb_1_interrupt_IRQn, 6 );        /* SCB 1 */
    NVIC_SetPriority( scb_2_interrupt_IRQn, 6 );        /* SCB 2 */
}
