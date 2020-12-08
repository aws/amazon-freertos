/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 * PSoC 6 platform UART driver
 */

#include "FreeRTOS.h"
#include <stdint.h>
#include <string.h>

#include "platform_peripheral.h"

#include "wiced_rtos.h"

#include "system_psoc6.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define platform_mcu_powersave_disable()
#define platform_mcu_powersave_enable()

//FIX_ME: !!!
#define WICED_SAVE_INTERRUPTS(flags)
#define WICED_RESTORE_INTERRUPTS(flags)

#define MIN(a,b) a < b ? a : b

#define PSOC6_SCB_UART_POLL_MODE

/******************************************************
 *                    Constants
 ******************************************************/

#define SCB_UART_TX_BUFFER_SIZE 8
#define SCB_UART_RX_BUFFER_SIZE 8
#define SCB_UART_TX_MAX_WAIT_TIME_MS (100)
#define SCB_UART_PORT_MAX 8

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    uint32_t baud_rate;
    uint32_t peri_clock;
    uint32_t divider;
    uint32_t oversample;
} scb_uart_baudrate_config_t;

typedef struct
{
    wiced_ring_buffer_t tx_buffer;
    uint8_t tx_data[SCB_UART_TX_BUFFER_SIZE];
    wiced_ring_buffer_t rx_buffer;
    uint8_t rx_data[SCB_UART_RX_BUFFER_SIZE];
} scb_uart_buffer_t;

/******************************************************
 *               Static Function Declarations
 ******************************************************/
extern void vTaskDelay(const TickType_t xTicksToDelay);

/******************************************************
 *               Variable Definitions
 ******************************************************/

    /* UART baudrate = peri_clock / (divider + 1) / oversample */
    scb_uart_baudrate_config_t baudrate_config[] =
        {
            {.baud_rate = 110, .peri_clock = 100000000, .divider = 75757, .oversample = 12},
            {.baud_rate = 300, .peri_clock = 100000000, .divider = 27775, .oversample = 12},
            {.baud_rate = 600, .peri_clock = 100000000, .divider = 13887, .oversample = 12},
            {.baud_rate = 1200, .peri_clock = 100000000, .divider = 6943, .oversample = 12},
            {.baud_rate = 2400, .peri_clock = 100000000, .divider = 3471, .oversample = 12},
            {.baud_rate = 4800, .peri_clock = 100000000, .divider = 1735, .oversample = 12},
            {.baud_rate = 9600, .peri_clock = 100000000, .divider = 867, .oversample = 12},
            {.baud_rate = 14400, .peri_clock = 100000000, .divider = 575, .oversample = 12},
            {.baud_rate = 19200, .peri_clock = 100000000, .divider = 433, .oversample = 12},
            {.baud_rate = 38400, .peri_clock = 100000000, .divider = 216, .oversample = 12},
            {.baud_rate = 57600, .peri_clock = 100000000, .divider = 143, .oversample = 12},
            {.baud_rate = 115200, .peri_clock = 100000000, .divider = 71, .oversample = 12},
            {.baud_rate = 230400, .peri_clock = 100000000, .divider = 35, .oversample = 12},
            {.baud_rate = 460800, .peri_clock = 100000000, .divider = 17, .oversample = 12},
            {.baud_rate = 921600, .peri_clock = 100000000, .divider = 8, .oversample = 12},
            {.baud_rate = 3125000, .peri_clock = 100000000, .divider = 2, .oversample = 16},
};

/* Driver handles for SCB UART instances */
platform_uart_driver_t *scb_uart_driver[SCB_UART_PORT_MAX];

/* Ring buffers for SCB UART instances */
static scb_uart_buffer_t scb_uart_buffer[SCB_UART_PORT_MAX];

extern platform_result_t platform_peripheral_clock_init(platform_peripheral_clock_t const *peripheral_clock, uint32_t div_int, uint32_t div_frac);

/******************************************************
 *               Function Definitions
 ******************************************************/

#ifndef PSOC6_SCB_UART_POLL_MODE
    static void scb_uart_enable_tx_irq(platform_uart_driver_t *driver)
{
    CySCB_Type* scb_base = driver->peripheral->scb->scb_base;
    Cy_SCB_SetTxInterruptMask(scb_base, CY_SCB_UART_TX_NOT_FULL);
}

static void scb_uart_disable_tx_irq( platform_uart_driver_t* driver )
{
    CySCB_Type* scb_base = driver->peripheral->scb->scb_base;
    Cy_SCB_SetTxInterruptMask(scb_base, CY_SCB_CLEAR_ALL_INTR_SRC);
}

static void scb_uart_enable_rx_irq( platform_uart_driver_t* driver )
{
    CySCB_Type* scb_base = driver->peripheral->scb->scb_base;
    Cy_SCB_SetRxInterruptMask(scb_base, CY_SCB_UART_RX_NOT_EMPTY);
}

static void scb_uart_disable_rx_irq( platform_uart_driver_t* driver )
{
    CySCB_Type* scb_base = driver->peripheral->scb->scb_base;
    Cy_SCB_SetRxInterruptMask(scb_base, CY_SCB_CLEAR_ALL_INTR_SRC);
}
#endif /* !PSOC6_SCB_UART_POLL_MODE */

static scb_uart_baudrate_config_t* platform_uart_baudrate_config( uint32_t baud_rate )
{
    uint32_t idx;

    for ( idx = 0 ; idx < (sizeof(baudrate_config)/sizeof(baudrate_config[0])) ; idx++ )
    {
        if ( baudrate_config[idx].baud_rate == baud_rate )
        {
            return &baudrate_config[idx];
        }
    }

    return NULL;
}

static platform_result_t platform_uart_pin_init( platform_uart_driver_t* driver )
{
    GPIO_PRT_Type *port_base;

    if (driver->peripheral->rx_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( driver->peripheral->rx_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, driver->peripheral->rx_pin->pin_num, CY_GPIO_DM_HIGHZ, 0x1, driver->peripheral->rx_pin->hsiom );
    }

    if (driver->peripheral->tx_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( driver->peripheral->tx_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, driver->peripheral->tx_pin->pin_num, CY_GPIO_DM_STRONG_IN_OFF, 0x1, driver->peripheral->tx_pin->hsiom );
    }

    if (driver->peripheral->cts_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( driver->peripheral->cts_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, driver->peripheral->cts_pin->pin_num, CY_GPIO_DM_HIGHZ, 0x1, driver->peripheral->cts_pin->hsiom );
    }

    if (driver->peripheral->rts_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( driver->peripheral->rts_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, driver->peripheral->rts_pin->pin_num, CY_GPIO_DM_STRONG_IN_OFF, 0x1, driver->peripheral->rts_pin->hsiom );
    }

    return PLATFORM_SUCCESS;
}

#ifndef PSOC6_SCB_UART_POLL_MODE
static platform_result_t platform_uart_receive_bytes_irq( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t* data_size_left_to_read, uint32_t timeout_ms )
{
    platform_result_t result = PLATFORM_ERROR;

    if ( ( driver == NULL ) || ( data_in == NULL ) || ( data_size_left_to_read == NULL ) || ( *data_size_left_to_read == 0 ) )
    {
        wiced_assert( "bad argument", 0 );
        return PLATFORM_BADARG;
    }

    wiced_assert( "not inited", ( driver->rx_buffer != NULL ) );

    if ( driver->rx_buffer != NULL )
    {
        uint32_t flags;
        uint32_t bytes_read = 0;
        uint32_t read_index = 0;
        uint32_t data_size  = 0;
        uint32_t ring_size  = 0;

        result = PLATFORM_SUCCESS;

        while ( *data_size_left_to_read > 0 )
        {
            wiced_result_t sem_result;

            /* Get the semaphore whenever data needs to be consumed from the ring buffer */
            sem_result = wiced_rtos_get_semaphore( &driver->rx_complete, timeout_ms );
            if ( sem_result != WICED_SUCCESS )
            {
                /* Can't get the semaphore */
                result = ( sem_result == WICED_TIMEOUT ) ? PLATFORM_TIMEOUT : PLATFORM_ERROR;
                break;
            }

            WICED_SAVE_INTERRUPTS(flags);

            /* Read the data from the ring buffer */
            ring_size = ring_buffer_used_space( driver->rx_buffer );
            data_size = MIN( *data_size_left_to_read, ring_size );
            ring_buffer_read( driver->rx_buffer, &data_in[read_index], data_size, &bytes_read );
            read_index              += bytes_read;
            *data_size_left_to_read -= bytes_read;
            ring_size = ring_buffer_used_space( driver->rx_buffer );

            /* Make sure SCB UART RX interrupts are re-enabled, they could have
             * been disabled by the ISR if ring buffer was about to overflow */
            scb_uart_enable_rx_irq( driver );

            WICED_RESTORE_INTERRUPTS(flags);

            if ( ring_size > 0 )
            {
                /* Set the semaphore to indicate the ring buffer is not empty */
                wiced_rtos_set_semaphore( &driver->rx_complete );
            }
        }
    }

    return result;
}

static platform_result_t platform_uart_transmit_bytes_irq( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
    platform_result_t result = PLATFORM_ERROR;

    if ( ( driver == NULL ) || ( data_out == NULL ) || ( size == 0 ) )
    {
        wiced_assert( "bad argument", 0 );
        return PLATFORM_BADARG;
    }

    wiced_assert( "not inited", ( driver->tx_buffer != NULL ) );

    if ( driver->tx_buffer != NULL )
    {
        uint32_t flags;
        uint32_t write_index = 0;
        uint32_t data_size  = 0;
        uint32_t free_size  = 0;

        result = PLATFORM_SUCCESS;

        platform_mcu_powersave_disable();
        while ( size > 0 )
        {
            wiced_result_t sem_result;
            /* Get the semaphore whenever data needs to be produced into the ring buffer */
            sem_result = wiced_rtos_get_semaphore( &driver->tx_complete, SCB_UART_TX_MAX_WAIT_TIME_MS );
            if ( sem_result != WICED_SUCCESS )
            {
                /* Can't get the semaphore */
                result = ( sem_result == WICED_TIMEOUT ) ? PLATFORM_TIMEOUT : PLATFORM_ERROR;
                break;
            }

            WICED_SAVE_INTERRUPTS(flags);

            free_size = ring_buffer_free_space( driver->tx_buffer );
            data_size = MIN( size, free_size );

            /* Write the data into the ring buffer */
            while ( data_size != 0 )
            {
                driver->tx_buffer->buffer[driver->tx_buffer->tail] = data_out[write_index];
                driver->tx_buffer->tail = ( driver->tx_buffer->tail + 1 ) % driver->tx_buffer->size;
                write_index++;
                size--;
                data_size--;
            }

            free_size = ring_buffer_free_space( driver->tx_buffer );

            /* Make sure SCB UART TX interrupts are re-enabled, they could have
             * been disabled by the ISR if ring buffer was about to underflow */
            scb_uart_enable_tx_irq( driver );

            WICED_RESTORE_INTERRUPTS(flags);

            if ( free_size > 0 )
            {
                /* Set the semaphore to indicate the ring buffer is not full */
                wiced_rtos_set_semaphore( &driver->tx_complete );
            }
        }

        platform_mcu_powersave_enable();
    }

    return result;
}
#else
static platform_result_t platform_uart_receive_bytes_poll( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t* expected_data_size, uint32_t timeout_ms )
{
    uint32_t data_size_left_to_read;
    uint32_t rx_data;
    CySCB_Type const* scb_base = driver->peripheral->scb->scb_base;

    // UNUSED_PARAMETER(timeout_ms);

    data_size_left_to_read = *expected_data_size;

    do
    {
        /* Receive data over SCB UART interface */
        while ( (rx_data = Cy_SCB_UART_Get( scb_base )) == CY_SCB_UART_RX_NO_DATA )
        {
            vTaskDelay(10);
        }

        *data_in = (uint8_t)(rx_data);

        data_in++;
        data_size_left_to_read--;
    }
    while ( data_size_left_to_read != 0 );

    *expected_data_size -= data_size_left_to_read;

    return PLATFORM_SUCCESS;
}

static platform_result_t platform_uart_transmit_bytes_poll( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
    uint32_t tx_data;
    CySCB_Type* scb_base = driver->peripheral->scb->scb_base;

    do
    {
        tx_data = (uint32_t)(*data_out);

        /* Transmit data over SCB UART interface */
        while ( Cy_SCB_UART_Put( scb_base, tx_data ) == 0 );

        data_out++;
        size--;
    }
    while ( size != 0 );

    return PLATFORM_SUCCESS;
}
#endif /* !PSOC6_SCB_UART_POLL_MODE */

platform_result_t platform_uart_init( platform_uart_driver_t* driver, const platform_uart_t* peripheral, const platform_uart_config_t* config, wiced_ring_buffer_t* optional_ring_buffer )
{
    const platform_scb_t* platform_scb;
    CySCB_Type* scb_uart;
    cy_stc_scb_uart_config_t scb_uart_config;
    scb_uart_baudrate_config_t* baudrate_conf;

    platform_mcu_powersave_disable();

    if ( (driver == NULL) || (peripheral == NULL) || (config == NULL) )
    {
        wiced_assert( "Bad argument", 0 );
        return PLATFORM_ERROR;
    }

    if ( optional_ring_buffer != NULL )
    {
        if ( (optional_ring_buffer->buffer == NULL) || (optional_ring_buffer->size == 0) )
        {
            wiced_assert("Bad ring buffer", 0 );
            return PLATFORM_ERROR;
        }
    }

    /* Get port details. */
    platform_scb = peripheral->scb;

    /* Lookup the baudrate config */
    baudrate_conf = platform_uart_baudrate_config( config->baud_rate );

    if ( baudrate_conf == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Lookup the SCB UART instance */
    scb_uart = platform_scb->scb_base;

    if ( scb_uart == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Disable the SCB UART */
    Cy_SCB_UART_Disable( scb_uart, NULL );

    /* Reset SCB UART driver handle */
    scb_uart_driver[peripheral->port] = NULL;

    /* Initialize the SCB UART driver parameters */
    driver->rx_size      = 0;
    driver->tx_size      = 0;
    driver->rx_overflow  = 0;
    driver->tx_underflow = 0;
    driver->peripheral   = (platform_uart_t*)peripheral;

#ifndef PSOC6_SCB_UART_POLL_MODE
    wiced_rtos_init_semaphore( &driver->rx_complete );
    wiced_rtos_init_semaphore( &driver->tx_complete );
    wiced_rtos_set_semaphore( &driver->tx_complete );
#endif /* !PSOC6_SCB_UART_POLL_MODE */

    /* Setup the SCB UART configuration structure */
    scb_uart_config.uartMode                   = CY_SCB_UART_STANDARD;
    scb_uart_config.enableMutliProcessorMode   = false;
    scb_uart_config.smartCardRetryOnNack       = false;
    scb_uart_config.irdaInvertRx               = false;
    scb_uart_config.irdaEnableLowPowerReceiver = false;

    scb_uart_config.oversample                 = baudrate_conf->oversample;

    scb_uart_config.enableMsbFirst             = false;
    scb_uart_config.enableInputFilter          = false;
    scb_uart_config.breakWidth                 = 11;
    scb_uart_config.dropOnFrameError           = false;
    scb_uart_config.dropOnParityError          = false;

    scb_uart_config.receiverAddress            = 0x0;
    scb_uart_config.receiverAddressMask        = 0x0;
    scb_uart_config.acceptAddrInFifo           = false;

    scb_uart_config.rxFifoTriggerLevel         = 0;
    scb_uart_config.rxFifoIntEnableMask        = 0;

    scb_uart_config.txFifoTriggerLevel         = 0;
    scb_uart_config.txFifoIntEnableMask        = 0;

    switch ( config->data_width )
    {
        case DATA_WIDTH_5BIT:
            scb_uart_config.dataWidth = 5;
            break;

        case DATA_WIDTH_6BIT:
            scb_uart_config.dataWidth = 6;
            break;

        case DATA_WIDTH_7BIT:
            scb_uart_config.dataWidth = 7;
            break;

        case DATA_WIDTH_8BIT:
            scb_uart_config.dataWidth = 8;
            break;

        case DATA_WIDTH_9BIT:
            scb_uart_config.dataWidth = 9;
            break;

        default:
            return PLATFORM_BADARG;
    }

    switch ( config->parity )
    {
        case NO_PARITY:
            scb_uart_config.parity = CY_SCB_UART_PARITY_NONE;
            break;

        case EVEN_PARITY:
            scb_uart_config.parity = CY_SCB_UART_PARITY_EVEN;
            break;

        case ODD_PARITY:
            scb_uart_config.parity = CY_SCB_UART_PARITY_ODD;
            break;

        default:
            return PLATFORM_BADARG;
    }

    switch ( config->stop_bits )
    {
        case STOP_BITS_1:
            scb_uart_config.stopBits = CY_SCB_UART_STOP_BITS_1;
            break;

        case STOP_BITS_2:
            scb_uart_config.stopBits = CY_SCB_UART_STOP_BITS_2;
            break;

        default:
            return PLATFORM_BADARG;
    }

    switch ( config->flow_control )
    {
        case FLOW_CONTROL_DISABLED:
        {
            scb_uart_config.enableCts                  = false;
            scb_uart_config.ctsPolarity                = CY_SCB_UART_ACTIVE_LOW;
            scb_uart_config.rtsRxFifoLevel             = 0;
            scb_uart_config.rtsPolarity                = CY_SCB_UART_ACTIVE_LOW;
            break;
        }

        case FLOW_CONTROL_CTS:
        {
            scb_uart_config.enableCts                  = true;
            scb_uart_config.ctsPolarity                = CY_SCB_UART_ACTIVE_LOW;
            scb_uart_config.rtsRxFifoLevel             = 0;
            scb_uart_config.rtsPolarity                = CY_SCB_UART_ACTIVE_LOW;
            break;
        }

        case FLOW_CONTROL_RTS:
        {
            scb_uart_config.enableCts                  = false;
            scb_uart_config.ctsPolarity                = CY_SCB_UART_ACTIVE_LOW;
            scb_uart_config.rtsRxFifoLevel             = 1;
            scb_uart_config.rtsPolarity                = CY_SCB_UART_ACTIVE_LOW;
            break;
        }

        case FLOW_CONTROL_CTS_RTS:
        {
            scb_uart_config.enableCts                  = true;
            scb_uart_config.ctsPolarity                = CY_SCB_UART_ACTIVE_LOW;
            scb_uart_config.rtsRxFifoLevel             = 1;
            scb_uart_config.rtsPolarity                = CY_SCB_UART_ACTIVE_LOW;
            break;
        }

        default:
            return PLATFORM_BADARG;
    }

    /* Setup RX ring buffer and related parameters */
    if ( optional_ring_buffer != NULL )
    {
        driver->rx_buffer = optional_ring_buffer;
    }
    else
    {
        ring_buffer_init( &scb_uart_buffer[peripheral->port].rx_buffer, scb_uart_buffer[peripheral->port].rx_data, SCB_UART_RX_BUFFER_SIZE );
        driver->rx_buffer = &scb_uart_buffer[peripheral->port].rx_buffer;
    }

    /* Setup TX ring buffer and related parameters */
    ring_buffer_init( &scb_uart_buffer[peripheral->port].tx_buffer, scb_uart_buffer[peripheral->port].tx_data, SCB_UART_TX_BUFFER_SIZE );
    driver->tx_buffer = &scb_uart_buffer[peripheral->port].tx_buffer;

    /* Initialize the SCB UART clock */
    if (platform_peripheral_clock_init(platform_scb->pclk, ((baudrate_conf->divider+1) / (baudrate_conf->peri_clock / cy_PeriClkFreqHz))-1, 0) != PLATFORM_SUCCESS)
    {
        return PLATFORM_ERROR;
    }

    /* Initialize the SCB UART pins */
    if ( platform_uart_pin_init( driver ) != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Initialize the SCB UART */
    if ( Cy_SCB_UART_Init( scb_uart, &scb_uart_config, NULL ) != CY_SCB_UART_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Set SCB UART driver handle */
    scb_uart_driver[peripheral->port] = driver;

    /* Enable the SCB UART */
    Cy_SCB_UART_Enable( scb_uart );

#ifndef PSOC6_SCB_UART_POLL_MODE
    /* Enable SCB UART IRQ */
    NVIC_EnableIRQ( platform_scb->irq_num );
    scb_uart_enable_rx_irq( driver );
    scb_uart_enable_tx_irq( driver );
#endif /* !PSOC6_SCB_UART_POLL_MODE */

    platform_mcu_powersave_enable();

    return PLATFORM_SUCCESS;
}

platform_result_t platform_uart_deinit( platform_uart_driver_t* driver )
{
    CySCB_Type* scb_base;
    platform_scb_t const* peripheral_scb;

    if ( driver == NULL )
    {
        wiced_assert( "Bad argument", 0 );
        return PLATFORM_ERROR;
    }

    peripheral_scb = driver->peripheral->scb;
    scb_base = peripheral_scb->scb_base;

    platform_mcu_powersave_disable();

#ifndef PSOC6_SCB_UART_POLL_MODE
    /* Disable SCB UART IRQ */
    scb_uart_disable_rx_irq( driver );
    scb_uart_disable_tx_irq( driver );
    NVIC_DisableIRQ( peripheral_scb->irq_num );
#endif /* !PSOC6_SCB_UART_POLL_MODE */

    /* Disable the SCB UART */
    Cy_SCB_UART_Disable( scb_base, NULL );

    /* Reset SCB UART driver handle */
    scb_uart_driver[driver->peripheral->port] = NULL;

    /* De-initialize the SCB UART */
    Cy_SCB_UART_DeInit( scb_base );

    wiced_rtos_deinit_semaphore( &driver->rx_complete );
    wiced_rtos_deinit_semaphore( &driver->tx_complete );
    driver->rx_size      = 0;
    driver->tx_size      = 0;
    driver->rx_overflow  = 0;
    driver->tx_underflow = 0;
    driver->peripheral   = NULL;

    platform_mcu_powersave_enable();

    return PLATFORM_SUCCESS;
}

platform_result_t platform_uart_receive_bytes( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t* expected_data_size, uint32_t timeout_ms )
{
    platform_result_t result = PLATFORM_ERROR;
    uint32_t          data_size_left_to_read;

    if ( ( driver == NULL ) || ( data_in == NULL ) || ( expected_data_size == NULL ) || ( *expected_data_size == 0 ) )
    {
        wiced_assert( "bad argument", 0 );
        return PLATFORM_ERROR;
    }

    data_size_left_to_read = *expected_data_size;

#ifndef PSOC6_SCB_UART_POLL_MODE
    result = platform_uart_receive_bytes_irq( driver, data_in, &data_size_left_to_read, timeout_ms );
#else
    result = platform_uart_receive_bytes_poll( driver, data_in, &data_size_left_to_read, timeout_ms );
#endif /* !PSOC6_SCB_UART_POLL_MODE */

    *expected_data_size -= data_size_left_to_read;

    return result;
}

platform_result_t platform_uart_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
    platform_result_t result = PLATFORM_ERROR;

    if ( ( driver == NULL ) || ( data_out == NULL ) || ( size == 0 ) )
    {
        wiced_assert( "bad argument", 0 );
        return PLATFORM_ERROR;
    }

#ifndef PSOC6_SCB_UART_POLL_MODE
    result = platform_uart_transmit_bytes_irq( driver, data_out, size );
#else
    result = platform_uart_transmit_bytes_poll( driver, data_out, size );
#endif /* !PSOC6_SCB_UART_POLL_MODE */

    return result;
}

/******************************************************
 *            IRQ Handlers Definition
 ******************************************************/

#ifndef PSOC6_SCB_UART_POLL_MODE
static wiced_bool_t platform_uart_process_rx_irq( platform_uart_driver_t* driver )
{
    wiced_bool_t result = WICED_FALSE;

    if ( driver->rx_buffer == NULL )
    {
        /* SCB UART RX interrupts remain turned off */
        result = WICED_FALSE;
    }
    else
    {
        /*
         * Check whether the ring buffer is already about to overflow.
         * This condition cannot happen during correct operation of the driver, but checked here only as precaution
         * to protect against ring buffer overflows for e.g. if UART interrupts got inadvertently enabled elsewhere.
         */
        if ( ring_buffer_free_space( driver->rx_buffer ) == 0 )
        {
            /* RX overflow error counter */
            driver->rx_overflow++;

            /* SCB UART RX interrupts remain turned off */
            result = WICED_FALSE;
        }
        else
        {
            /* Transfer data from SCB UART into the ring buffer */
            driver->rx_buffer->buffer[driver->rx_buffer->tail] = (uint8_t)Cy_SCB_UART_Get( driver->peripheral->scb->scb_base );
            driver->rx_buffer->tail = ( driver->rx_buffer->tail + 1 ) % driver->rx_buffer->size;

            /* Check whether the ring buffer is about to overflow */
            if ( ring_buffer_free_space( driver->rx_buffer ) == 0 )
            {
                /* SCB UART RX interrupts remain turned off */
                result = WICED_FALSE;
            }
            else
            {
                result = WICED_TRUE;
            }

            if ( ring_buffer_used_space( driver->rx_buffer ) == 1 )
            {
                /* Set the semaphore to indicate the ring buffer is not empty */
                host_rtos_set_semaphore( &driver->rx_complete, WICED_TRUE );
            }
        }
    }

    return result;
}

static wiced_bool_t platform_uart_process_tx_irq( platform_uart_driver_t* driver )
{
    wiced_bool_t result = WICED_FALSE;

    if ( driver->tx_buffer == NULL )
    {
        /* SCB UART TX interrupts remain turned off */
        result = WICED_FALSE;
    }
    else
    {
        /*
         * Check whether the ring buffer is already about to underflow.
         * This condition cannot happen during normal operation of the driver, but checked here only as precaution
         * to protect against ring buffer underflows for e.g. if UART interrupts got inadvertently enabled elsewhere.
         */
        if ( ring_buffer_used_space( driver->tx_buffer ) == 0 )
        {
            /* TX underflow error counter */
            driver->tx_underflow++;

            /* SCB UART TX interrupts remain turned off */
            result = WICED_FALSE;
        }
        else
        {
            uint8_t data = 0;
            uint32_t size = 1;
            uint32_t read = 0;

            /* Transfer data from the ring buffer into SCB UART */
            ring_buffer_read( driver->tx_buffer, &data, size, &read );
            Cy_SCB_UART_Put( driver->peripheral->scb->scb_base, (uint32_t)data );

            /* Check whether the ring buffer is about to underflow */
            if ( ring_buffer_used_space( driver->tx_buffer ) == 0 )
            {
                /* SCB UART TX interrupts remain turned off */
                result = WICED_FALSE;
            }
            else
            {
                result = WICED_TRUE;
            }

            if ( ring_buffer_free_space( driver->tx_buffer ) == 1 )
            {
                /* Set the semaphore to indicate the ring buffer is not full */
                host_rtos_set_semaphore( &driver->tx_complete, WICED_TRUE );
            }
        }
    }

    return result;
}

static void platform_uart_rx_irq( platform_uart_driver_t* driver )
{
    /* Turn off SCB UART RX interrupt */
    scb_uart_disable_rx_irq( driver );

    if ( platform_uart_process_rx_irq( driver ) != WICED_TRUE )
    {
        /* Clear SCB UART RX interrupt */
        Cy_SCB_ClearRxInterrupt(driver->peripheral->scb->scb_base, CY_SCB_UART_RX_NOT_EMPTY);

        /* SCB UART RX interrupt remain turned off */
        return;
    }

    /* Clear SCB UART RX interrupt */
    Cy_SCB_ClearRxInterrupt(driver->peripheral->scb->scb_base, CY_SCB_UART_RX_NOT_EMPTY);

    /* Turn on SCB UART RX interrupt */
    scb_uart_enable_rx_irq( driver );
}

static void platform_uart_tx_irq( platform_uart_driver_t* driver )
{
    /* Turn off SCB UART TX interrupt */
    scb_uart_disable_tx_irq( driver );

    if ( platform_uart_process_tx_irq( driver ) != WICED_TRUE )
    {
        /* Clear SCB UART TX interrupt */
        Cy_SCB_ClearTxInterrupt(driver->peripheral->scb->scb_base, CY_SCB_UART_TX_NOT_FULL);

        /* SCB UART TX interrupt remain turned off */
        return;
    }

    /* Clear SCB UART TX interrupt */
    Cy_SCB_ClearTxInterrupt(driver->peripheral->scb->scb_base, CY_SCB_UART_TX_NOT_FULL);

    /* Turn on SCB UART TX interrupt */
    scb_uart_enable_tx_irq( driver );
}

void platform_uart_irq( uint32_t port )
{
    uint32_t flags;
    platform_uart_driver_t* driver;
    CySCB_Type* scb_base;

    WICED_SAVE_INTERRUPTS(flags);

    driver   = scb_uart_driver[port];
    scb_base = driver->peripheral->scb->scb_base;

    if ( (Cy_SCB_GetInterruptCause(scb_base) & CY_SCB_RX_INTR) != 0 )
    {
        if ( (Cy_SCB_GetRxInterruptStatusMasked(scb_base) & CY_SCB_UART_RX_NOT_EMPTY) != 0 )
        {
            platform_uart_rx_irq( driver );
        }
    }

    if ( (Cy_SCB_GetInterruptCause(scb_base) & CY_SCB_TX_INTR) != 0 )
    {
        if ( (Cy_SCB_GetTxInterruptStatusMasked(scb_base) & CY_SCB_UART_TX_NOT_FULL) != 0 )
        {
            platform_uart_tx_irq( driver );
        }
    }

    WICED_RESTORE_INTERRUPTS(flags);
}
#else
void platform_uart_irq( uint32_t port )
{

}
#endif /* !PSOC6_SCB_UART_POLL_MODE */

#if 0
static wiced_result_t platform_uart_ok_to_enter_deep_sleep( platform_uart_driver_t* driver )
{
    CySCB_Type* scb_base = driver->peripheral->scb->scb_base;
    wiced_result_t result = WICED_SUCCESS;

    WICED_SAVE_INTERRUPTS( flags );

    /* Prevent deep-Sleep if TX buffer is not empty */
    if ( ( ( driver->tx_buffer != NULL ) && ( ring_buffer_used_space( driver->tx_buffer ) != 0 ) ) ||
         ( ( driver->rx_buffer != NULL ) && ( ring_buffer_used_space( driver->rx_buffer ) != 0 ) ) ||
         ( WICED_TRUE != Cy_SCB_UART_IsTxComplete( scb_base ) ) ||
         ( 0UL != Cy_SCB_UART_GetNumInRxFifo( scb_base ) ) )
    {
        result = WICED_ERROR;
    }

    WICED_RESTORE_INTERRUPTS( flags );

    return result;
}
#endif
