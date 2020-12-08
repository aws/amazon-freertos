/*
 * $ Copyright Broadcom Corporation $
 */

/** @file
 *
 * used only for wiced builds and not in AFR builds.
 */

#include "wiced_rtos.h"
#include "platform_bluetooth.h"
#include "platform_peripheral.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define RETURN_IF_FAILURE( x ) \
    do \
    { \
        platform_result_t _result = (x); \
        if ( _result != PLATFORM_SUCCESS ) \
        { \
            return (wiced_result_t) _result; \
        } \
    } while( 0 )

/******************************************************
 *                    Constants
 ******************************************************/

#ifndef BLUETOOTH_CHIP_STABILIZATION_DELAY
#define BLUETOOTH_CHIP_STABILIZATION_DELAY    (500)
#endif

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
 *               Function Declarations
 ******************************************************/

platform_result_t bluetooth_wiced_init_config_uart( const platform_uart_config_t* bt_uart_config );

/******************************************************
 *               Variables Definitions
 ******************************************************/

static volatile wiced_ring_buffer_t rx_ring_buffer;
DEFINE_RING_BUFFER_DATA(static volatile uint8_t, rx_data, UART_RX_FIFO_SIZE)

/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_result_t bluetooth_wiced_init_platform( void )
{
    if ( wiced_bt_control_pins[ WICED_BT_PIN_HOST_WAKE ] != NULL )
    {
        RETURN_IF_FAILURE( platform_gpio_init( wiced_bt_control_pins[WICED_BT_PIN_HOST_WAKE], INPUT_HIGH_IMPEDANCE ) );
    }

    if ( wiced_bt_control_pins[ WICED_BT_PIN_DEVICE_WAKE ] != NULL )
    {
        RETURN_IF_FAILURE( platform_gpio_init( wiced_bt_control_pins[ WICED_BT_PIN_DEVICE_WAKE ], OUTPUT_PUSH_PULL ) );
        RETURN_IF_FAILURE( platform_gpio_output_low( wiced_bt_control_pins[ WICED_BT_PIN_DEVICE_WAKE ] ) );
        wiced_rtos_delay_milliseconds( 100 );
    }

    /* Configure Reg Enable pin to output. Set to HIGH */
    if ( wiced_bt_control_pins[ WICED_BT_PIN_POWER ] != NULL )
    {
        RETURN_IF_FAILURE( platform_gpio_init( wiced_bt_control_pins[ WICED_BT_PIN_POWER ], OUTPUT_OPEN_DRAIN_PULL_UP ) );
        if ( wiced_bt_control_pins[ WICED_BT_PIN_RESET ] == NULL )
        {
            RETURN_IF_FAILURE( platform_gpio_output_low( wiced_bt_control_pins[ WICED_BT_PIN_POWER ] ) );
            wiced_rtos_delay_milliseconds( 10 );
            RETURN_IF_FAILURE( platform_gpio_output_high( wiced_bt_control_pins[ WICED_BT_PIN_POWER ] ) );
        }
    }

    if ( wiced_bt_uart_config.flow_control == FLOW_CONTROL_DISABLED )
    {
        /* Configure RTS pin to output. Set to HIGH */
        RETURN_IF_FAILURE( platform_gpio_init( wiced_bt_uart_pins[WICED_BT_PIN_UART_RTS], OUTPUT_OPEN_DRAIN_PULL_UP ) );
        RETURN_IF_FAILURE( platform_gpio_output_high( wiced_bt_uart_pins[WICED_BT_PIN_UART_RTS] ) );

        /* Configure CTS pin to input pull-up */
        RETURN_IF_FAILURE( platform_gpio_init( wiced_bt_uart_pins[WICED_BT_PIN_UART_CTS], INPUT_PULL_UP ) );
    }

    if ( wiced_bt_control_pins[ WICED_BT_PIN_RESET ] != NULL )
    {
        RETURN_IF_FAILURE( platform_gpio_init( wiced_bt_control_pins[ WICED_BT_PIN_RESET ], OUTPUT_PUSH_PULL ) );
        RETURN_IF_FAILURE( platform_gpio_output_high( wiced_bt_control_pins[ WICED_BT_PIN_RESET ] ) );

        /* Configure USART comms */
        RETURN_IF_FAILURE( bluetooth_wiced_init_config_uart( &wiced_bt_uart_config ) );

        /* Reset bluetooth chip */
        RETURN_IF_FAILURE( platform_gpio_output_low( wiced_bt_control_pins[ WICED_BT_PIN_RESET ] ) );
        wiced_rtos_delay_milliseconds( 10 );
        RETURN_IF_FAILURE( platform_gpio_output_high( wiced_bt_control_pins[ WICED_BT_PIN_RESET ] ) );
    }
    else
    {
        /* Configure USART comms */
        RETURN_IF_FAILURE( bluetooth_wiced_init_config_uart( &wiced_bt_uart_config ) );
    }

    wiced_rtos_delay_milliseconds( BLUETOOTH_CHIP_STABILIZATION_DELAY );

    if ( wiced_bt_uart_config.flow_control == FLOW_CONTROL_DISABLED )
    {
        /* Bluetooth chip is ready. Pull host's RTS low */
        RETURN_IF_FAILURE( platform_gpio_output_low( wiced_bt_uart_pins[WICED_BT_PIN_UART_RTS] ) );
    }

    /* Wait for Bluetooth chip to pull its RTS (host's CTS) low. From observation using CRO, it takes the bluetooth chip > 170ms to pull its RTS low after CTS low */
    while ( platform_gpio_input_get( wiced_bt_uart_pins[ WICED_BT_PIN_UART_CTS ] ) == WICED_TRUE )
    {
        wiced_rtos_delay_milliseconds( 10 );
    }
    return WICED_SUCCESS;
}

platform_result_t bluetooth_wiced_init_config_uart( const platform_uart_config_t* bt_uart_config )
{
    platform_result_t result;

    // platform_init_peripheral_irq_priorities();
    ring_buffer_init( (wiced_ring_buffer_t*) &rx_ring_buffer, (uint8_t*) rx_data, sizeof( rx_data ) );
    result = platform_uart_init( wiced_bt_uart_driver, wiced_bt_uart_peripheral, bt_uart_config, (wiced_ring_buffer_t*) &rx_ring_buffer );
    return result;
}
