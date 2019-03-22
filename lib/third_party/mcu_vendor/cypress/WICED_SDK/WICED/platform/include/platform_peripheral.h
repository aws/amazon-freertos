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
 *  Defines generic data and enumerated types used by Platform Peripheral API
 *  Declares function prototypes for Platform peripheral API
 *
 *
 *   ------------------------------------------------------------------------------------------------------------
 *   Porting Notes
 *   ------------------------------------------------------------------------------------------------------------
 *   * This file defines and consolidates various standardized data types required by the Platform Peripheral API
 *   * Generic Platform Peripheral data types are defined in this file
 *   * MCU-specific data types are defined in <WICED-SDK>/platform/MCU/<MCU_family>/platform_mcu_peripheral.h
 *   * platform_mcu_peripheral.h may include declarations, definitions, and function prototypes which are local to the MCU family
 *   * The following table outlines the structures that need to be defined in platform_mcu_peripheral.h:
 *   +------------------------+-----------------------+
 *   | Name                   | Description           |
 *   +------------------------+-----------------------+
 *   | platform_gpio_t        | GPIO interface        |
 *   | platform_uart_t        | UART interface        |
 *   | platform_uart_driver_t | UART driver interface |
 *   | platform_spi_t         | SPI  interface        |
 *   | platform_i2c_t         | I2C  interface        |
 *   | platform_pwm_t         | PWM  interface        |
 *   | platform_adc_t         | ADC  interface        |
 *   +------------------------+-----------------------+
 *
 *
*/

#pragma once
#include "stdint.h"
#include "platform_mcu_peripheral.h" /* Include MCU-specific types */
#include "platform_toolchain.h"
#include "ring_buffer.h"
#include "platform_cache_def.h"

#ifdef __cplusplus
extern "C" {
#endif



/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define WICED_PERIPHERAL_UNSUPPORTED ( 0xFFFFFFFF )

/* SPI mode constants */
#define SPI_CLOCK_RISING_EDGE  ( 1 << 0 )
#define SPI_CLOCK_FALLING_EDGE ( 0 << 0 )
#define SPI_CLOCK_IDLE_HIGH    ( 1 << 1 )
#define SPI_CLOCK_IDLE_LOW     ( 0 << 1 )
#define SPI_USE_DMA            ( 1 << 2 )
#define SPI_NO_DMA             ( 0 << 2 )
#define SPI_MSB_FIRST          ( 1 << 3 )
#define SPI_LSB_FIRST          ( 0 << 3 )
#define SPI_CS_ACTIVE_HIGH     ( 1 << 4 )
#define SPI_CS_ACTIVE_LOW      ( 0 << 4 )

/* I2C flags constants */
#define I2C_DEVICE_DMA_MASK_POSN ( 0 )
#define I2C_DEVICE_NO_DMA        ( 1 << I2C_DEVICE_DMA_MASK_POSN )
#define I2C_DEVICE_USE_DMA       ( 0 << I2C_DEVICE_DMA_MASK_POSN )

/******************************************************
 *                   Enumerations
 ******************************************************/

/**
 * Pin configuration
 */
typedef enum
{
    INPUT_PULL_UP,             /**< Input with an internal pull-up resistor - use with devices that actively drive the signal low - e.g. button connected to ground */
    INPUT_PULL_DOWN,           /**< Input with an internal pull-down resistor - use with devices that actively drive the signal high - e.g. button connected to a power rail */
    OUTPUT_PUSH_PULL,          /**< Output actively driven high and actively driven low - must not be connected to other active outputs - e.g. LED output */
    INPUT_HIGH_IMPEDANCE,      /**< Input - must always be driven, either actively or by an external pullup resistor */
    OUTPUT_OPEN_DRAIN_NO_PULL, /**< Output actively driven low but is high-impedance when set high - can be connected to other open-drain/open-collector outputs. Needs an external pull-up resistor */
    OUTPUT_OPEN_DRAIN_PULL_UP, /**< Output actively driven low and is pulled high with an internal resistor when set high - can be connected to other open-drain/open-collector outputs. */
} platform_pin_config_t;

/**
 * GPIO interrupt trigger
 */
typedef enum
{
    IRQ_TRIGGER_RISING_EDGE  = 0x1, /**< Interrupt triggered at input signal's rising edge  */
    IRQ_TRIGGER_FALLING_EDGE = 0x2, /**< Interrupt triggered at input signal's falling edge */
    IRQ_TRIGGER_BOTH_EDGES   = IRQ_TRIGGER_RISING_EDGE | IRQ_TRIGGER_FALLING_EDGE, /**< Interrupt triggered at input signal's rising and falling edge */
    IRQ_TRIGGER_LEVEL_HIGH   = 0x4, /**< Interrupt triggered when input signal's level is high */
    IRQ_TRIGGER_LEVEL_LOW    = 0x8, /**< Interrupt triggered when input signal's level is low  */
} platform_gpio_irq_trigger_t;

/**
 * UART data width
 */
typedef enum
{
    DATA_WIDTH_5BIT,    /**< 5 Bit data */
    DATA_WIDTH_6BIT,    /**< 6 Bit data */
    DATA_WIDTH_7BIT,    /**< 7 Bit data */
    DATA_WIDTH_8BIT,    /**< 8 Bit data */
    DATA_WIDTH_9BIT     /**< 9 Bit data */
} platform_uart_data_width_t;

/**
 * UART stop bits
 */
typedef enum
{
    STOP_BITS_1,        /**< 1 Stop bit  */
    STOP_BITS_2,        /**< 2 Stop bits */
} platform_uart_stop_bits_t;

/**
 * UART flow control
 */
typedef enum
{
    FLOW_CONTROL_DISABLED,      /**< No Flow Control  */
    FLOW_CONTROL_CTS,           /**< CTS Flow Control  */
    FLOW_CONTROL_RTS,           /**< RTS Flow Control  */
    FLOW_CONTROL_CTS_RTS        /**< CTS RTS Flow Control */
} platform_uart_flow_control_t;

/**
 * UART parity
 */
typedef enum
{
    NO_PARITY,      /**< No Parity   */
    ODD_PARITY,     /**< Odd Parity  */
    EVEN_PARITY,    /**< Even Parity */
} platform_uart_parity_t;

/**
 * I2C address width
 */
typedef enum
{
    I2C_ADDRESS_WIDTH_7BIT,     /**< 7 Bit I2C address  */
    I2C_ADDRESS_WIDTH_10BIT,    /**< 10 Bit I2C address */
    I2C_ADDRESS_WIDTH_16BIT,    /**< 16 Bit I2C address */
} platform_i2c_bus_address_width_t;

/**
 * I2C speed mode
 */
typedef enum
{
    I2C_LOW_SPEED_MODE,         /**< 10Khz devices  */
    I2C_STANDARD_SPEED_MODE,    /**< 100Khz devices */
    I2C_HIGH_SPEED_MODE         /**< 400Khz devices */
} platform_i2c_speed_mode_t;

/**
 * SPI slave transfer direction
 */
typedef enum
{
    SPI_SLAVE_TRANSFER_WRITE, /**< SPI master writes data to the SPI slave device  */
    SPI_SLAVE_TRANSFER_READ   /**< SPI master reads data from the SPI slave device */
} platform_spi_slave_transfer_direction_t;

typedef enum
{
    SPI_SLAVE_TRANSFER_SUCCESS,             /**< SPI transfer successful */
    SPI_SLAVE_TRANSFER_INVALID_COMMAND,     /**< Command is invalid */
    SPI_SLAVE_TRANSFER_ADDRESS_UNAVAILABLE, /**< Address specified in the command is unavailable */
    SPI_SLAVE_TRANSFER_LENGTH_MISMATCH,     /**< Length specified in the command doesn't match with the actual data length */
    SPI_SLAVE_TRANSFER_READ_NOT_ALLOWED,    /**< Read operation is not allowed for the address specified */
    SPI_SLAVE_TRANSFER_WRITE_NOT_ALLOWED,   /**< Write operation is not allowed for the address specified */
    SPI_SLAVE_TRANSFER_HARDWARE_ERROR,      /**< Hardware error occurred during transfer */
    SPI_SLAVE_TRANSFER_STATUS_MAX = 0xff,   /**< Denotes maximum value. Not a valid status */
} platform_spi_slave_transfer_status_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/**
 * GPIO interrupt callback handler
 */
typedef void (*platform_gpio_irq_callback_t)( void* arg );      /**< GPIO interrupt callback handler */

/******************************************************
 *                    Structures
 ******************************************************/

/**
 * UART configuration
 */
typedef struct
{
    uint32_t                     baud_rate;     /**< Baud rate */
    platform_uart_data_width_t   data_width;    /**< Data width */
    platform_uart_parity_t       parity;        /**< Parity */
    platform_uart_stop_bits_t    stop_bits;     /**< Number of stop bits */
    platform_uart_flow_control_t flow_control;
} platform_uart_config_t;

/**
 * SPI configuration
 */
typedef struct
{
    uint32_t               speed;       /*<< Speed */
    uint8_t                mode;        /*<< Use one/combination of SPI Mode constants */
    uint8_t                bits;        /*<< Data transfer width */
    const platform_gpio_t* chip_select; /*<< Chip Select pin */
} platform_spi_config_t;

#pragma pack(1)
typedef struct platform_spi_slave_command
{
    platform_spi_slave_transfer_direction_t direction;      /**< Write/Read from Master */
    uint16_t                                address;        /**< Address */
    uint16_t                                data_length;    /**< Data length */
} platform_spi_slave_command_t;

typedef struct
{
    uint16_t                             data_length;       /**< Data length */
    platform_spi_slave_transfer_status_t status;            /**< Status */
    uint8_t                              data[1];           /**< Data */
} platform_spi_slave_data_buffer_t;

/**
 * SPI slave configuration
 */
typedef struct platform_spi_slave_config
{
    uint32_t speed;     /**< Speed */
    uint8_t  mode;      /**< Use one/combination of SPI Mode constants */
    uint8_t  bits;      /**< Data transfer width */
} platform_spi_slave_config_t;

#pragma pack()



/**
 * SPI message segment
 */
typedef struct
{
    const void* tx_buffer;      /**< Pointer to data to be sent to SPI slave */
    void*       rx_buffer;      /**< Pointer to data to be received from SPI slave */
    uint32_t    length;         /**< length of data to be sent */
} platform_spi_message_segment_t;

/**
 * I2C configuration
 */
typedef struct
{
    uint16_t                         address;           /**<* The address of the device on the i2c bus */
    platform_i2c_bus_address_width_t address_width;     /**<  Address witdth */
    uint8_t                          flags;             /**<  WICED_I2C_START_FLAG  / WICED_I2C_REPEATED_START_FLAG */
    platform_i2c_speed_mode_t        speed_mode;        /**<* Speed mode the device operates in */
} platform_i2c_config_t;

/**
 * I2C message
 */
typedef struct
{
    const void*  tx_buffer;     /**< tx_buffer */
    void*        rx_buffer;     /**< rx_buffer */
    uint16_t     tx_length;     /**< tx_length */
    uint16_t     rx_length;     /**< rx_length */
    uint16_t     retries;       /**< Number of times to retry the message */
    uint8_t      flags;         /**< MESSAGE_DISABLE_DMA : if set, this flag disables use of DMA for the message */
} platform_i2c_message_t;

/**
 * RTC time
 */
typedef struct
{
    uint8_t sec;                /**< Seconds */
    uint8_t min;                /**< Minutes */
    uint8_t hr;                 /**< Hours */
    uint8_t weekday;            /**< 1-sunday... 7-saturday */
    uint8_t date;               /**< Date */
    uint8_t month;              /**< Month */
    uint8_t year;               /**< Year */
} platform_rtc_time_t;

/**
 * 802.1AS time
 */
typedef struct
{
    uint32_t master_secs;         /**< Master time seconds value.             */
    uint32_t master_nanosecs;     /**< Master time nanoseconds value.         */
    uint32_t local_secs;          /**< Local time seconds value.              */
    uint32_t local_nanosecs;      /**< Local time nanoseconds value.          */
    uint32_t audio_sample_rate;   /**< Audio sample rate is an INPUT argument */
    uint32_t audio_time_secs;     /**< Audio time seconds value               */
    uint32_t audio_time_nanosecs; /**< Audio time nanoseconds value           */
} platform_8021as_time_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

extern uint32_t platform_ddr_size;

/******************************************************
 *               Function Declarations
 ******************************************************/

/**
 * performs complete reset operation
 */
void platform_mcu_reset( void ) NORETURN;


/**
 * Initialise the specified GPIO pin
 *
 * @param[in] gpio   : gpio pin
 * @param[in] config : pin configuration
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_gpio_init( const platform_gpio_t* gpio, platform_pin_config_t config );


/**
 * Deinitialise the specified GPIO pin
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_gpio_deinit( const platform_gpio_t* gpio );


/**
 * Toggle the specified GPIO pin output high
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_gpio_output_high( const platform_gpio_t* gpio );


/**
 * Toggle the specified GPIO pin output low
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_gpio_output_low( const platform_gpio_t* gpio );


/**
 * Retrieve logic level of the GPIO input pin specified
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref platform_result_t
 */
wiced_bool_t platform_gpio_input_get( const platform_gpio_t* gpio );


/**
 * Enable interrupt on the GPIO input pin specified
 *
 * @param[in] gpio    : gpio pin
 * @param[in] trigger : interrupt trigger type
 * @param[in] handler : callback function that will be called when an interrupt occurs
 * @param[in] arg     : argument that will be passed into the callback function
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_gpio_irq_enable( const platform_gpio_t* gpio, platform_gpio_irq_trigger_t trigger, platform_gpio_irq_callback_t handler, void* arg );


/**
 * Disable interrupt on the GPIO input pin specified
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_gpio_irq_disable( const platform_gpio_t* gpio );


/**
 * Set the state of an LED
 *
 * @param[in] led_index : LED index into table in platforms/<platform>/platform.c :: platform_gpio_leds[]
 *                        0 <= led_index < PLATFORM_LED_COUNT    PLATFORM_LED_COUNT defined in platforms/<platform>/platform.h
 * @param[in] off_on    : 0 (WICED_LED_OFF) or 1 (WICED_LED_ON)
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_led_set_state(int led_index, int off_on );

/**
 * Enable MCU powersave
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_mcu_powersave_enable( void );


/**
 * Disable MCU powersave
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_mcu_powersave_disable( void );


/**
 * Notify the software stack that MCU has exited powersave mode due to interrupt
 *
 * @return @ref platform_result_t
 */
void platform_mcu_powersave_exit_notify( void );


/**
 * Refresh the watchdog
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_watchdog_kick( void );


/**
 * Check if last reset occurred due to watchdog reset
 *
 * @return @ref platform_result_t
 */
wiced_bool_t platform_watchdog_check_last_reset( void );


/**
 * Initialise the specified UART port
 *
 * @return @ref platform_result_t
 */

platform_result_t platform_uart_init( platform_uart_driver_t* driver, const platform_uart_t* peripheral, const platform_uart_config_t* config, wiced_ring_buffer_t* optional_ring_buffer );


/**
 * Deinitialise the specified UART port
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_uart_deinit( platform_uart_driver_t* driver );


/**
 * Transmit data over the specified UART port
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_uart_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size );


/**
 * Transmit data over the specified UART port
 * This should be special version of transmit function used in CPU exception context,
 * simplest implementation without interrupts.
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_uart_exception_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size );

/**
 * Receive data over the specified UART port
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_uart_receive_bytes( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t* expected_data_size, uint32_t timeout_ms );


/**
 * Invoke the UART powersave wakeup function
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_uart_powersave_wakeup_handler ( const platform_uart_t* peripheral );

/**
 * Invoke the UART powersave sleep function
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_uart_powersave_sleep_handler ( const platform_uart_t* peripheral );

/**
 * Initialise the specified SPI interface
 *
 * @param[in] spi_interface : SPI interface
 * @param[in] config        : SPI configuratin
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_spi_init( const platform_spi_t* spi, const platform_spi_config_t* config );


/**
 * Deinitialise the specified SPI interface
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_spi_deinit( const platform_spi_t* spi );


/**
 * Transfer data to the specified SPI interface
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_spi_transmit( const platform_spi_t* spi, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments );


/**
 * Transfer data over the specified SPI interface
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_spi_transfer( const platform_spi_t* spi, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments );

/**
 * Transfer raw data over the specified SPI interface.
 * No prior setup (toggle chip-select, etc.) is done.
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_spi_transfer_nosetup( const platform_spi_t* spi, const platform_spi_config_t* config, const uint8_t* send_ptr, uint8_t* recv_ptr, uint32_t length );

/**
 * Toggle chip-select for the specified SPI interface.
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_spi_chip_select_toggle( const platform_spi_t* spi, const platform_spi_config_t* config, wiced_bool_t activate );


/** Initialises a SPI slave interface
 *
 * @param[in]  driver     : the SPI slave driver to be initialised
 * @param[in]  peripheral : the SPI peripheral interface to be initialised
 * @param[in]  config     : SPI slave configuration
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_spi_slave_init( platform_spi_slave_driver_t* driver, const platform_spi_t* peripheral, const platform_spi_slave_config_t* config );


/** De-initialises a SPI slave interface
 *
 * @param[in]  driver : the SPI slave driver to be de-initialised
 *
 * @return @ref platform_result_t
 */

platform_result_t platform_spi_slave_deinit( platform_spi_slave_driver_t* driver );


/** Receive command from the remote SPI master
 *
 * @param[in]   driver      : the SPI slave driver
 * @param[out]  command     : pointer to the variable which will contained the received command
 * @param[in]   timeout_ms  : timeout in milliseconds
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_spi_slave_receive_command( platform_spi_slave_driver_t* driver, platform_spi_slave_command_t* command, uint32_t timeout_ms );


/** Transfer data to/from the remote SPI master
 *
 * @param[in]  driver      : the SPI slave driver
 * @param[in]  direction   : transfer direction
 * @param[in]  buffer      : the buffer which contain the data to transfer
 * @param[in]  timeout_ms  : timeout in milliseconds
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_spi_slave_transfer_data( platform_spi_slave_driver_t* driver, platform_spi_slave_transfer_direction_t direction, platform_spi_slave_data_buffer_t* buffer, uint32_t timeout_ms );


/** Send an error status over the SPI slave interface
 *
 * @param[in]  driver       : the SPI slave driver
 * @param[in]  error_status : SPI slave error status
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_spi_slave_send_error_status( platform_spi_slave_driver_t* driver, platform_spi_slave_transfer_status_t error_status );


/** Generate an interrupt on the SPI slave interface
 *
 * @param[in]  driver            : the SPI slave driver
 * @param[in]  pulse_duration_ms : interrupt pulse duration in milliseconds
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_spi_slave_generate_interrupt( platform_spi_slave_driver_t* driver, uint32_t pulse_duration_ms );


/**
 * Initialise ADC interface
 *
 * @param[in] adc_interface : adc_interface
 * @param[in] sample_cycle  : sample cycle
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_adc_init( const platform_adc_t* adc, uint32_t sample_cycle );


/**
 * Deinitialise ADC interface
 *
 * @param[in] adc_interface : adc_interface
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_adc_deinit( const platform_adc_t* adc );


/**
 * Take ADC sample
 *
 * @param[in]  adc_interface : adc_interface
 * @param[out] output        : variable that will contain the sample output
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_adc_take_sample( const platform_adc_t* adc, uint16_t* output );


/**
 * Take ADC sample
 *
 * @param[in]  adc_interface : ADC interface
 * @param[out] buffer        : buffer that will contain the sample stream output
 * @param[in]  buffer_length : buffer length
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_adc_take_sample_stream( const platform_adc_t* adc, void* buffer, uint16_t buffer_length );


/**
 * Initialise I2C interface
 *
 * @param[in] i2c_interface : I2C interface
 * @param[in] config        : I2C configuration
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_i2c_init( const platform_i2c_t* i2c, const platform_i2c_config_t* config );


/**
 * Deinitialise I2C interface
 *
 * @param[in] i2c_interface : I2C interface
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_i2c_deinit( const platform_i2c_t* i2c, const platform_i2c_config_t* config );


/**
 * Probe I2C slave device
 *
 * @param[in] i2c_interface : I2C interface
 * @param[in] retries       : number of retries
 *
 * @return @ref platform_result_t
 */
wiced_bool_t platform_i2c_probe_device( const platform_i2c_t* i2c, const platform_i2c_config_t* config, int retries );


/**
 * Initialise I2C transmit message
 *
 * @param[in,out] message          : I2C message
 * @param[in]     tx_buffer        : transmit buffer
 * @param[in]     tx_buffer_length : transmit buffer length is bytes
 * @param[in]     retries          : number of transmission retries
 * @param[in]     disable_dma      : WICED_TRUE if DMA is disabled
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_i2c_init_tx_message( platform_i2c_message_t* message, const void* tx_buffer, uint16_t tx_buffer_length, uint16_t retries, wiced_bool_t disable_dma );


/**
 * Initialise I2C receive message
 *
 * @param[in,out] message          : I2C message
 * @param[in]     rx_buffer        : receive buffer
 * @param[in]     rx_buffer_length : receive buffer length is bytes
 * @param[in]     retries          : number of transmission retries
 * @param[in]     disable_dma      : WICED_TRUE if DMA is disabled
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_i2c_init_rx_message( platform_i2c_message_t* message, void* rx_buffer, uint16_t rx_buffer_length, uint16_t retries, wiced_bool_t disable_dma );


/**
 * Initialise I2C combined message
 *
 * @param[in,out] message          : I2C message
 * @param[in]     tx_buffer        : transmit buffer
 * @param[in]     rx_buffer        : receive buffer
 * @param[in]     tx_buffer_length : transmit buffer length is bytes
 * @param[in]     rx_buffer_length : receive buffer length is bytes
 * @param[in]     retries          : number of transmission retries
 * @param[in]     disable_dma      : WICED_TRUE if DMA is disabled
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_i2c_init_combined_message( platform_i2c_message_t* message, const void* tx_buffer, void* rx_buffer, uint16_t tx_buffer_length, uint16_t rx_buffer_length, uint16_t retries, wiced_bool_t disable_dma );


/**
 * Transfer data via the I2C interface
 *
 * @param[in] i2c_interface      : I2C interface
 * @param[in] messages           : pointer to an array of messages to transceive
 * @param[in] number_of_messages : number of messages in the array
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_i2c_transfer( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* messages, uint16_t number_of_messages );


/**
 * Read bytes via the I2C interface
 *
 * @param[in]  i2c                : I2C interface
 * @param[in]  config             : settings and flags used for transfer
 * @param[in]  flags              : flags for controlling the the transfer
 * @param[out] buffer             : pointer to a receiving buffer
 * @param[in]  buffer_length      : length in bytes of the receiving buffer
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_i2c_read( const platform_i2c_t* i2c, const platform_i2c_config_t* config, uint16_t flags, void* buffer, uint16_t buffer_length );


/**
 * Write bytes via the I2C interface
 *
 * @param[in]  i2c                : I2C interface
 * @param[in]  config             : settings and flags used for transfer
 * @param[in]  flags              : flags for controlling the the transfer
 * @param[in]  buffer             : pointer to a transmit buffer
 * @param[in]  buffer_length      : length in bytes of the transmit buffer
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_i2c_write( const platform_i2c_t* i2c, const platform_i2c_config_t* config, uint16_t flags, const void* buffer, uint16_t buffer_length );

/**
 * Initialise PWM interface
 *
 * @param[in] pwm_interface : PWM interface
 * @param[in] frequency     : PWM signal frequency in Hz
 * @param[in] duty_cycle    : PWM signal duty cycle in percentage point
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_pwm_init( const platform_pwm_t* pwm, uint32_t frequency, float duty_cycle );


/**
 * Start generating PWM signal on the specified PWM interface
 *
 * @param[in] pwm_interface : PWM interface
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_pwm_start( const platform_pwm_t* pwm );


/**
 * Stop generating PWM signal on the specified PWM interface
 *
 * @param[in] pwm_interface : PWM interface
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_pwm_stop( const platform_pwm_t* pwm );


/**
 * Get current real-time clock
 *
 * @param[in] time : variable that will contain the current real-time clock
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_rtc_get_time( platform_rtc_time_t* time );


/**
 * Set real-time clock
 *
 * @param[in] time : real-time clock
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_rtc_set_time( const platform_rtc_time_t* time );


/**
 * Initialise UART standard I/O
 *
 * @param[in,out] driver    : UART STDIO driver
 * @param[in]     interface : UART STDIO interface
 * @param[in]     config    : UART STDIO configuration
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_stdio_init ( platform_uart_driver_t* driver, const platform_uart_t* interface, const platform_uart_config_t* config );


/**
 * Get current value of nanosecond clock
 *
*/
uint64_t platform_get_nanosecond_clock_value( void );


/**
 * Deinitialize nanosecond clock
 *
 */
void platform_deinit_nanosecond_clock( void );


/**
 * Reset nanosecond clock
 *
 */
void platform_reset_nanosecond_clock( void );


/**
 * Initialize nanosecond clock
 *
 */
void platform_init_nanosecond_clock( void );

/**
 * Enter hibernation
 *
 * @param[in] ticks_to_wakeup : how many ticks to spend in hibernation
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_hibernation_start( uint32_t ticks_to_wakeup );

/**
 * Return WICED_TRUE if returned from hibernation
 *
 * @return @ref wiced_bool_t
 */
wiced_bool_t platform_hibernation_is_returned_from( void );

/**
 * Return number of ticks system spent in hibernation mode
 *
 * @return @ref uint32_t
 */
uint32_t platform_hibernation_get_ticks_spent( void );

/**
 * Return hibernation timer frequency
 *
 * @return @ref uint32_t
 */
uint32_t platform_hibernation_get_clock_freq( void );

/**
 * Return maximum ticks number hibernation timer can use
 *
 * @return @ref uint32_t
 */
uint32_t platform_hibernation_get_max_ticks( void );

/**
 * Enable the 802.1AS time functionality.
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_time_enable_8021as(void);


/**
 * Disable the 802.1AS time functionality.
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_time_disable_8021as(void);


/**
 * Read the 802.1AS time.
 *
 * Retrieve the origin timestamp in the last sync message, correct for the
 * intervening interval and return the corrected time in seconds + nanoseconds.
 * Optionally, retrieve corresponding audio time.
 *
 * @param[in/out] as_time: pointer to 802.1AS structure @ref platform_8021as_time_t
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_time_read_8021as(platform_8021as_time_t* as_time);


/**
 * Configure GPIO to wakeup MCU from deep-sleep.
 *
 * @param[in] gpio    : GPIO Pin to wake up from Deep Sleep
 * @param[in] trigger : Wakeup trigger
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : on failure
 */
platform_result_t platform_gpio_deepsleep_wakeup_enable(const platform_gpio_t* gpio, platform_gpio_irq_trigger_t trigger);

#ifdef __cplusplus
} /*"C" */
#endif
