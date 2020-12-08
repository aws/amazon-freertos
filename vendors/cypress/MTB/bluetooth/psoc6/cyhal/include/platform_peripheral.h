/*
 * $ Copyright Cypress Semiconductor $
 */

/**
 *
 * How-To
 * ======
 * The platform on which the Bluetooth stack is being ported must support access to hardware such as UART and GPIO.
 * For example - A platform must be able to initialize, configure and control an UART port(connected to Bluetooth Controller).
 * Similarly, the platform must have well defined GPIO structures to toggle the GPIO lines and help establishing control-flow between Host and BT Controller.
 *
 * Step # 1 - 'platform_gpio_t', 'platform_uart_t' and 'platform_uart_driver_t' shall be defined as per underlying platform.
 *
 * Step # 2 - Interface methods/functions like platform_gpio_init/deinit/toggle/output/input,
 * platform_uart_init/deinit/receive/transmit and other methods should be tested and verified on the underlying platform.
 *
 * Step # 3 - Bluetooth controller specific 'bluetooth_wiced_init_platform'  and 'bluetooth_wiced_init_config_uart' APIs shall be implemented
 * to handle any special power-on sequence, reset-sequence, hardware/gpio programming.
 *
 * Step # 4 - Bluetooth libraries should be used along with implementation defined above(step 1 to 3). Bluetooth libraries should be compiled
 * against same architecture as your underlying platform. For example - CR4 bluetooth library may not work with CM3 platform and so on.
 *
 *
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "cy_syslib.h"
#include "cy_sysclk.h"
#include "cy_scb_uart.h"
#include "cy_gpio.h"

#include "ring_buffer.h"

#include "wiced_rtos.h"

#include "platform_bluetooth.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define PLATFORM_SUCCESS                WICED_SUCCESS
#define PLATFORM_UNSUPPORTED            WICED_UNSUPPORTED
#define PLATFORM_ERROR                  WICED_ERROR
#define PLATFORM_BADARG                 WICED_BADARG
#define PLATFORM_TIMEOUT                WICED_TIMEOUT

/******************************************************
 *                      Macros
 ******************************************************/

/* Assign PCLK_XXX_CLOCK to a divider instance. */
#define DEFINE_PLATFORM_CLOCK_DIV_X( _PERIPH_, _DIVNUM_, _DIVTYPE_, _MAX_ )       \
    /* wiced_static_assert(                    */                \
    /*        _DIVNUM__exceeds_max_value,      */                \
    /*        ( (_DIVNUM_) < (_MAX_) )         */                \
    /*);                                       */                \
    platform_peripheral_clock_t const peripheral_clock_ ## _PERIPH_ = {\
            .clock_dest     = (_PERIPH_),                        \
            .divider_type   = (_DIVTYPE_),                       \
            .divider_num    = (_DIVNUM_),                        \
    }
/* Returns platform_peripheral_clock_t const* from a peripheral clock instance. */
#define PLATFORM_CLOCK_DIV_PTR( _PERIPH_ )  (&peripheral_clock_ ## _PERIPH_ )

/* Define an 8-bit clock divider peripheral assignment. */
#define DEFINE_PLATFORM_CLOCK_DIV_8( _PERIPH_, _DIVNUM_ ) \
    DEFINE_PLATFORM_CLOCK_DIV_X( _PERIPH_, _DIVNUM_, CY_SYSCLK_DIV_8_BIT, PLATFORM_SYSCLK_DIV_8_BIT_MAX )

/* Define a 16-bit clock divider peripheral assignment. */
#define DEFINE_PLATFORM_CLOCK_DIV_16( _PERIPH_, _DIVNUM_ ) \
    DEFINE_PLATFORM_CLOCK_DIV_X( _PERIPH_, _DIVNUM_, CY_SYSCLK_DIV_16_BIT, PLATFORM_SYSCLK_DIV_16_BIT_MAX )

/******************************************************
 *                    Constants
 ******************************************************/

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

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct
{
    uint32_t hf_clock;                   /* clkHf index */
    cy_en_clkhf_in_sources_t clock_path; /* Selects which clkHf input, or root mux, to configure. */
} platform_root_clock_t;

typedef struct
{
    uint32_t            port_num;       /* Port Number of the GPIO port */
    uint32_t            pin_num;        /* Pin Number within Port */
    en_hsiom_sel_t      hsiom;          /* HSIOM Configuration for GPIO operation for this Pin */
} platform_gpio_t;

typedef struct
{
    en_clk_dst_t clock_dest;
    cy_en_divider_types_t divider_type;
    uint32_t divider_num;
} platform_peripheral_clock_t;

typedef struct {
    CySCB_Type*                         scb_base;
    const platform_peripheral_clock_t*  pclk;
    IRQn_Type                           irq_num;
} platform_scb_t;

typedef struct
{
    const uint32_t             port;
    const platform_scb_t*      scb;
    const platform_gpio_t*     rx_pin;
    const platform_gpio_t*     tx_pin;
    const platform_gpio_t*     cts_pin;
    const platform_gpio_t*     rts_pin;
} platform_uart_t;

typedef struct
{
    platform_uart_t*           peripheral;
    wiced_ring_buffer_t*       tx_buffer;
    wiced_ring_buffer_t*       rx_buffer;
    wiced_semaphore_t          tx_complete;
    wiced_semaphore_t          rx_complete;
    volatile uint32_t          tx_size;
    volatile uint32_t          rx_size;
    uint32_t                   tx_underflow;
    uint32_t                   rx_overflow;
} platform_uart_driver_t;

/**
 * Bluetooth library's Platform UART configuration structure.
 * The UART configuration structure is required to build the bluetooth library(*.a)
 * See @ref wiced_bt_uart_config
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
 * Bluetooth library's Patchram configuration structure.
 * The patchram configuration is required to generate bluetooth library(*.a) with correct patchram sequence.
 * See @ref wiced_bt_config
 */
typedef struct
{
    uint32_t                          patchram_download_baud_rate; /**< UART baud rate at which Patchram will be downloaded */
    wiced_bt_patchram_download_mode_t patchram_download_mode;      /**< If the Controller requires Patchram to be downloaded */
    uint32_t                          featured_baud_rate;          /**< Standard Baud rate at which Host will communicate to Bluetooth Controller */
} platform_bluetooth_config_t;

typedef wiced_result_t platform_result_t;

/**
 * Bluetooth device address callback registration API and constants
 */
typedef enum
{
    PLATFORM_BD_ADDRESS_PUBLIC,         /** Public BD address  */
    PLATFORM_BD_ADDRESS_STATIC_RANDOM   /** Random BD address */
} platform_bd_adddress_type_t;
typedef wiced_result_t (*platform_bt_get_dev_address)(uint8_t *dataPtr, uint8_t length, uint8_t addr_type);
typedef wiced_result_t (*platform_bt_get_dev_name)(uint8_t *dataPtr, uint8_t max_length);

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/**
 * Variables to be defined by underlying Platform.
 */

/**
 *  See wiced_bt_platform.h (in Bluetooth directory) for #wiced_bt_control_pin_t
 *
 *  Host <-> Bluetooth controller Control pins See @ref wiced_bt_control_pin_t
 *  For example:
 *      const platform_gpio_t* wiced_bt_control_pins[WICED_BT_PIN_MAX] =
 *      {
 *        [WICED_BT_PIN_POWER]       = NULL,
 *        [WICED_BT_PIN_RESET]       = NULL,
 *        [WICED_BT_PIN_HOST_WAKE]   = NULL,
 *        [WICED_BT_PIN_DEVICE_WAKE] = NULL
 *      };
 *
 */
extern const platform_gpio_t*        wiced_bt_control_pins[];

/**
 * See wiced_bt_platform.h (in Bluetooth directory) for #wiced_bt_uart_pin_t enum definition
 *
 * Host <-> Bluetooth controller UART pins see @ref wiced_bt_uart_pin_t
 * For example:
 *     const platform_gpio_t* wiced_bt_uart_pins[] =
 *     {
 *       [WICED_BT_PIN_UART_TX ] = NULL,
 *       [WICED_BT_PIN_UART_RX ] = NULL,
 *       [WICED_BT_PIN_UART_CTS] = NULL,
 *       [WICED_BT_PIN_UART_RTS] = NULL,
 *     };
 */
extern const platform_gpio_t*        wiced_bt_uart_pins[];

/* Defines the platform specific UART port(Host) which is connected to Bluetooth Controller */
extern const platform_uart_t*        wiced_bt_uart_peripheral;

/* Defines the platform specific UART driver(on Host) which controls the UART port(and hence the controller) */
extern       platform_uart_driver_t* wiced_bt_uart_driver;

/* Defined under CYHAL - but required by Bluetooth stack's Userial driver code. See @platform_bluetooth_config_default.c */
extern const platform_uart_config_t  wiced_bt_uart_config;

/* Defined under CYHAL - but required by Bluetooth stack's Userial driver code. See @platform_bluetooth_config_default.c */
extern const platform_bluetooth_config_t wiced_bt_config;


/******************************************************
 *               Function Declarations
 ******************************************************/

/**
 * Initialise the specified GPIO pin
 *
 * @param[in] gpio   @ref platform_gpio_t A GPIO pin instance as defined by underlying platform.
 * @param[in] config @ref platform_pin_config_t GPIO pin configuration to be used to initialize GPIO pin.
 *
 * @return @ref wiced_result_t returns WICED_SUCCESS on success
 *                             returns WICED_ERROR on error
 *                             returns WICED_BADARG on Bad Arguments
 */
wiced_result_t platform_gpio_init(const platform_gpio_t* gpio, platform_pin_config_t config);

/**
 * Deinitialise the specified GPIO pin
 *
 * @param[in] gpio @ref platform_gpio_t A GPIO pin instance as defined by underlying platform.
 *
 * @return @ref wiced_result_t returns WICED_SUCCESS on success
 *                             returns WICED_ERROR on error
 *                             returns WICED_BADARG on Bad Arguments
 */
wiced_result_t platform_gpio_deinit( const platform_gpio_t* gpio );

/**
 * Set the specified GPIO pin output low
 *
 * @param[in] gpio @ref platform_gpio_t A GPIO pin instance as defined by underlying platform.
 *
 * @return @ref wiced_result_t returns WICED_SUCCESS on success
 *                             returns WICED_ERROR on error
 *                             returns WICED_BADARG on Bad Arguments
 */

wiced_result_t platform_gpio_output_low(const platform_gpio_t* gpio);
/**
 * Set the specified GPIO pin output high
 *
 * @param[in] gpio @ref platform_gpio_t A GPIO pin instance as defined by underlying platform.
 *
 * @return @ref wiced_result_t returns WICED_SUCCESS on success
 *                             returns WICED_ERROR on error
 *                             returns WICED_BADARG on Bad Arguments
 */
wiced_result_t platform_gpio_output_high(const platform_gpio_t* gpio);
/**
 * Retrieve logic level of the GPIO input pin specified
 *
 * @param[in] gpio @ref platform_gpio_t A GPIO pin instance as defined by underlying platform.
 *
 * @return @ref wiced_result_t returns WICED_SUCCESS on success
 *                             returns WICED_ERROR on error
 *                             returns WICED_BADARG on Bad Arguments
 */
wiced_bool_t platform_gpio_input_get(const platform_gpio_t* gpio);

/**
 * Initialise the specified UART port
 *
 * @return @ref platform_result_t
 */

platform_result_t platform_uart_init( platform_uart_driver_t* driver, const platform_uart_t* peripheral, const platform_uart_config_t* config, wiced_ring_buffer_t* optional_ring_buffer );

/**
 * Deinitialise the specified UART port
 * @param[in] driver @ref platform_uart_driver_t Platform UART driver/handle to use
 *
 * @return @ref wiced_result_t returns WICED_SUCCESS on success
 *                             returns WICED_ERROR on error
 *                             returns WICED_BADARGS on Bad Arguments
 */
wiced_result_t platform_uart_deinit(platform_uart_driver_t* driver);

/**
 * Transmit data over the specified UART port
 * @param[in] driver @ref platform_uart_driver_t Platform UART driver/handle to use
 * @param[in] data_out Start address of the memory/buffer
 * @param[in] size     Length of the buffer to write in bytes
 *
 * @return @ref wiced_result_t returns WICED_SUCCESS on success
 *                             returns WICED_ERROR on error
 *                             returns WICED_BADARGS on Bad Arguments
 */
wiced_result_t platform_uart_transmit_bytes(platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size);

/**
 * Receive data over the specified UART port
 * @param[in] driver @ref platform_uart_driver_t Platform UART driver/handle to use
 * @param[out] data_in Start address of the buffer
 * @param[in,out] expected_data_size Number of bytes expected by the Caller.
 * @param[in] timeout_ms Timeout in milliseconds
 *
 * @return @ref wiced_result_t returns WICED_SUCCESS on success
 *                             returns WICED_ERROR on error/timeout
 *                             returns WICED_BADARGS on Bad Arguments
 */
wiced_result_t platform_uart_receive_bytes(platform_uart_driver_t* driver, uint8_t* data_in,  uint32_t* expected_data_size, uint32_t timeout_ms);

/**
 * Invoke platform-specific Bluetooth Initialization sequence(power-up, toggle RST lines, CTS/RTS operations, initialize UART etc.)
 * @note See libraries/drivers/bluetooth/BTE/WICED/wiced_bt.c for WICED platform implementation
 * @return @ref wiced_result_t returns WICED_SUCCESS on success
 *                             returns WICED_ERROR on error
 */
wiced_result_t bluetooth_wiced_init_platform(void);

/**
 * Invokes platform-specific Bluetooth UART configuration sequence.
 * @return @ref wiced_result_t returns WICED_SUCCESS on success
 *                             returns WICED_ERROR on error
 *                             returns WICED_BADARGS on Bad Arguments
 * @note See libraries/drivers/bluetooth/BTE/WICED/wiced_bt.c for WICED platform implementation
 */
wiced_result_t bluetooth_wiced_init_config_uart(const platform_uart_config_t* bt_uart_config);


/** Bluetooth platform device property access functions */
typedef struct {
    platform_bt_get_dev_address     get_bd_address;               /**< Bluetooth device address generation interface */
    platform_bt_get_dev_name        get_bd_device_name;           /**< Get Bluetooth device name */
} platform_bt_dev_property_interface_t;


/**
 * Amazon specific Bluetooth device property access registration API.
 *
 * @param[in] p_bt_dev_property  : Function pointer structure for different bluetooth device property functions
 *
 * @return @ref wiced_result_t. Always returns WICED_SUCCESS, Pass NULL to this API to de-register the interface
 */
wiced_result_t platform_bt_dev_property_interface_register(platform_bt_dev_property_interface_t *p_bt_dev_property);

#ifdef __cplusplus
} /*extern "C" */
#endif
