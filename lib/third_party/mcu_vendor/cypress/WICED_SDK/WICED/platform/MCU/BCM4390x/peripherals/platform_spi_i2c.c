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

/*
 * License for FreeBSD I2C Bit-Banging Driver (sys/dev/iicbus/iicbb.c)
 * -
 * Copyright (c) 1998, 2001 Nicolas Souchu
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: head/sys/dev/iicbus/iicbb.c 261844 2014-02-13 18:22:49Z loos $
 */

/** @file
 *
 */
#include <stdint.h>
#include <string.h>
#include "typedefs.h"
#include "platform_peripheral.h"
#include "platform_isr.h"
#include "platform_isr_interface.h"
#include "wwd_rtos.h"
#include "wwd_assert.h"
#include "wiced_osl.h"
#include "wiced_platform.h"

#include "platform_mcu_peripheral.h"
#include "platform_map.h"
#include "platform_pinmux.h"

#include "bcmutils.h"
#include "sbchipc.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define REG32(ADDR)  *((volatile uint32_t*)(ADDR))

#define GSIO_I2C_WRITE_REG_VAL          \
    (                                   \
        GSIO_GC_I2C_SOURCE_FROMDATA |   \
        GSIO_READ_I2C_WRITE         |   \
        GSIO_NOACK_I2C_CHECK_ACK    |   \
        GSIO_NOD_1                  |   \
        GSIO_ENDIANNESS_BIG         |   \
        GSIO_GM_I2C                 |   \
        0                               \
    )

#define GSIO_I2C_READ_REG_VAL           \
    (                                   \
        GSIO_GC_I2C_SOURCE_FROMDATA |   \
        GSIO_READ_I2C_READ          |   \
        GSIO_NOD_1                  |   \
        GSIO_ENDIANNESS_BIG         |   \
        GSIO_GM_I2C                 |   \
        0                               \
    )

#define GSIO_VERIFY(x)  {platform_result_t res = (x); if (res != PLATFORM_SUCCESS){return res;}}

#define I2C_DRIVER(i2c) ( i2c->driver )

#define SPI_DRIVER(spi) ( spi->driver )

/******************************************************
 *                    Constants
 ******************************************************/

#define I2C_TRANSACTION_TIMEOUT_MS  ( 5 )

#define I2C_START_FLAG              (1U << 0)
#define I2C_STOP_FLAG               (1U << 1)

#define GSIO_0_IF                   (PLATFORM_CHIPCOMMON_REGBASE(OFFSETOF(chipcregs_t, gsioctrl)))
#define GSIO_0_CLKDIV               (PLATFORM_CHIPCOMMON_REGBASE(OFFSETOF(chipcregs_t, clkdiv2)))
#define GSIO_1_IF                   (PLATFORM_CHIPCOMMON_REGBASE(OFFSETOF(chipcregs_t, gsio1ctrl)))
#define GSIO_1_CLKDIV               (PLATFORM_CHIPCOMMON_REGBASE(OFFSETOF(chipcregs_t, gsio1clkdiv)))
#define GSIO_2_IF                   (PLATFORM_CHIPCOMMON_REGBASE(OFFSETOF(chipcregs_t, gsio2ctrl)))
#define GSIO_2_CLKDIV               (PLATFORM_CHIPCOMMON_REGBASE(OFFSETOF(chipcregs_t, gsio2clkdiv)))
#define GSIO_3_IF                   (PLATFORM_CHIPCOMMON_REGBASE(OFFSETOF(chipcregs_t, gsio3ctrl)))
#define GSIO_3_CLKDIV               (PLATFORM_CHIPCOMMON_REGBASE(OFFSETOF(chipcregs_t, gsio3clkdiv)))

#ifndef GSIO_I2C_REPEATED_START_NEEDS_GPIO
#define GSIO_I2C_REPEATED_START_NEEDS_GPIO 0
#endif

/* Function selection for SPI1_MISO (GCI Chip Control 9 Register) */
#define PIN_FUNCTION_SEL_SPI1_MISO  (1U << 27)

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct gsio_43909_interface_regs gsio_43909_interface_regs_t;
typedef struct gsio_43909_regs gsio_43909_regs_t;

typedef uint32_t (gsio_divider_function_t)(uint32_t clk_rate, uint32_t target_speed_hz);

typedef platform_result_t (i2c_io_fn_t)( const platform_i2c_t*, const platform_i2c_config_t*, uint8_t flags, uint8_t* buffer, uint16_t buffer_length);

typedef platform_result_t (*i2c_init_func_t)        ( const platform_i2c_t* i2c, const platform_i2c_config_t* config );
typedef platform_result_t (*i2c_deinit_func_t)      ( const platform_i2c_t* i2c, const platform_i2c_config_t* config );
typedef platform_result_t (*i2c_read_func_t)        ( const platform_i2c_t *i2c, const platform_i2c_config_t *config, uint8_t flags, uint8_t *data_in, uint16_t length );
typedef platform_result_t (*i2c_write_func_t)       ( const platform_i2c_t *i2c, const platform_i2c_config_t *config, uint8_t flags, uint8_t *data_out, uint16_t length );
typedef platform_result_t (*i2c_transfer_func_t)    ( const platform_i2c_t* i2c, const platform_i2c_config_t *config, uint16_t flags, void *buffer, uint16_t buffer_length, i2c_io_fn_t* fn );

typedef platform_result_t (*spi_init_func_t)               ( const platform_spi_t* spi, const platform_spi_config_t* config );
typedef platform_result_t (*spi_deinit_func_t)             ( const platform_spi_t* spi );
typedef platform_result_t (*spi_transfer_func_t)           ( const platform_spi_t* spi, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments );
typedef platform_result_t (*spi_transfer_nosetup_func_t)   ( const platform_spi_t* spi, const platform_spi_config_t* config, const uint8_t* send_ptr, uint8_t* rcv_ptr, uint32_t length );
typedef platform_result_t (*spi_chip_select_toggle_func_t) ( const platform_spi_t* spi, const platform_spi_config_t* config, wiced_bool_t activate );

/******************************************************
 *                    Structures
 ******************************************************/

struct gsio_43909_interface_regs
{
     uint32_t   ctrl;
     uint32_t   address;
     uint32_t   data;
     /* clockdiv outside of this struct */
};

struct gsio_43909_regs
{
    volatile gsio_43909_interface_regs_t    *interface;
    volatile uint32_t                       *clkdiv;
};

struct i2c_driver
{
    i2c_init_func_t init;
    i2c_deinit_func_t deinit;
    i2c_read_func_t read;
    i2c_write_func_t write;
    i2c_transfer_func_t transfer;
};

struct spi_driver
{
    spi_init_func_t init;
    spi_deinit_func_t deinit;
    spi_transfer_func_t transfer;
    spi_transfer_nosetup_func_t transfer_nosetup;
    spi_chip_select_toggle_func_t chip_select_toggle;
};

/******************************************************
 *               Function Declarations
 ******************************************************/

static void                 gsio_set_clock      ( int port, uint32_t target_speed_hz, gsio_divider_function_t * );
static platform_result_t    gsio_wait_for_xfer_to_complete( int port );
static platform_result_t    i2c_xfer            ( const platform_i2c_t *i2c, uint32_t regval );
static void                 i2c_stop            ( const platform_i2c_t *i2c );

/* GSIO I2C bus interface */
static platform_result_t    i2c_init            ( const platform_i2c_t* i2c, const platform_i2c_config_t* config );
static platform_result_t    i2c_deinit          ( const platform_i2c_t* i2c, const platform_i2c_config_t* config );
static platform_result_t    i2c_read            ( const platform_i2c_t *i2c, const platform_i2c_config_t *config, uint8_t flags, uint8_t *data_in, uint16_t length );
static platform_result_t    i2c_write           ( const platform_i2c_t *i2c, const platform_i2c_config_t *config, uint8_t flags, uint8_t *data_out, uint16_t length );
static platform_result_t    i2c_transfer        ( const platform_i2c_t* i2c, const platform_i2c_config_t *config, uint16_t flags, void *buffer, uint16_t buffer_length, i2c_io_fn_t* fn );

/* Bit-Banging I2C bus interface */
static platform_result_t    i2c_bb_init        ( const platform_i2c_t* i2c, const platform_i2c_config_t* config );
static platform_result_t    i2c_bb_deinit      ( const platform_i2c_t* i2c, const platform_i2c_config_t* config );
static platform_result_t    i2c_bb_read        ( const platform_i2c_t *i2c, const platform_i2c_config_t *config, uint8_t flags, uint8_t *data_in, uint16_t length );
static platform_result_t    i2c_bb_write       ( const platform_i2c_t *i2c, const platform_i2c_config_t *config, uint8_t flags, uint8_t *data_out, uint16_t length );
static platform_result_t    i2c_bb_transfer    ( const platform_i2c_t* i2c, const platform_i2c_config_t *config, uint16_t flags, void *buffer, uint16_t buffer_length, i2c_io_fn_t* fn );

/* GSIO SPI bus interface */
static platform_result_t    spi_init                  ( const platform_spi_t* spi, const platform_spi_config_t* config );
static platform_result_t    spi_deinit                ( const platform_spi_t* spi );
static platform_result_t    spi_transfer              ( const platform_spi_t* spi, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments );
static platform_result_t    spi_transfer_nosetup      ( const platform_spi_t* spi, const platform_spi_config_t* config, const uint8_t* send_ptr, uint8_t* rcv_ptr, uint32_t length );
static platform_result_t    spi_chip_select_toggle    ( const platform_spi_t* spi, const platform_spi_config_t* config, wiced_bool_t activate );

/* Bit-Banging SPI bus interface */
static platform_result_t    spi_bb_init               ( const platform_spi_t* spi, const platform_spi_config_t* config );
static platform_result_t    spi_bb_deinit             ( const platform_spi_t* spi );
static platform_result_t    spi_bb_transfer           ( const platform_spi_t* spi, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments );
static platform_result_t    spi_bb_transfer_nosetup   ( const platform_spi_t* spi, const platform_spi_config_t* config, const uint8_t* send_ptr, uint8_t* rcv_ptr, uint32_t length );
static platform_result_t    spi_bb_chip_select_toggle ( const platform_spi_t* spi, const platform_spi_config_t* config, wiced_bool_t activate );

/******************************************************
 *               Variables Definitions
 ******************************************************/

static const gsio_43909_regs_t gsio_regs[BCM4390X_GSIO_MAX] =
{
    /* BCM4390X_GSIO_0 */
    {
        .interface          = (volatile gsio_43909_interface_regs_t *)GSIO_0_IF,
        .clkdiv             = (volatile uint32_t *)GSIO_0_CLKDIV,
    },

    /* BCM4390X_GSIO_1 */
    {
        .interface          = (volatile gsio_43909_interface_regs_t *)GSIO_1_IF,
        .clkdiv             = (volatile uint32_t *)GSIO_1_CLKDIV,
    },

    /* BCM4390X_GSIO_2 */
    {
        .interface          = (volatile gsio_43909_interface_regs_t *)GSIO_2_IF,
        .clkdiv             = (volatile uint32_t *)GSIO_2_CLKDIV,
    },

    /* BCM4390X_GSIO_3 */
    {
        .interface          = (volatile gsio_43909_interface_regs_t *)GSIO_3_IF,
        .clkdiv             = (volatile uint32_t *)GSIO_3_CLKDIV,
    },
};

/* GSIO I2C bus driver functions */
const i2c_driver_t i2c_gsio_driver =
{
    .init = i2c_init,
    .deinit = i2c_deinit,
    .read = i2c_read,
    .write = i2c_write,
    .transfer = i2c_transfer
};

/* Bit-Banging I2C bus driver functions */
const i2c_driver_t i2c_bb_driver =
{
        .init = i2c_bb_init,
        .deinit = i2c_bb_deinit,
        .read = i2c_bb_read,
        .write = i2c_bb_write,
        .transfer = i2c_bb_transfer
};

/* GSIO SPI bus driver functions */
const spi_driver_t spi_gsio_driver =
{
    .init = spi_init,
    .deinit = spi_deinit,
    .transfer = spi_transfer,
    .transfer_nosetup = spi_transfer_nosetup,
    .chip_select_toggle = spi_chip_select_toggle
};

/* Bit-Banging SPI bus driver functions */
const spi_driver_t spi_bb_driver =
{
    .init = spi_bb_init,
    .deinit = spi_bb_deinit,
    .transfer = spi_bb_transfer,
    .transfer_nosetup = spi_bb_transfer_nosetup,
    .chip_select_toggle = spi_bb_chip_select_toggle
};

/******************************************************
 *               Function Definitions
 ******************************************************/

/*
 * Resets I2C pins (if GPIO-muxing enabled) to HW/Power-On default function,
 * thereby clearing any previous initialization of GPIO alternate function.
 */
static inline void gsio_i2c_reset_pins( const platform_i2c_t* i2c )
{
    if ( i2c->pin_sda != NULL )
    {
        platform_gpio_deinit( i2c->pin_sda );
    }

    if ( i2c->pin_scl != NULL )
    {
        platform_gpio_deinit( i2c->pin_scl );
    }
}

static void gsio_clock_request( void )
{
    platform_mcu_powersave_request_clock( PLATFORM_MCU_POWERSAVE_CLOCK_BACKPLANE_ON_HT );
}

static void gsio_clock_release( void )
{
    platform_mcu_powersave_release_clock( PLATFORM_MCU_POWERSAVE_CLOCK_BACKPLANE_ON_HT );
}

static void gsio_set_clock( int port, uint32_t target_speed_hz, gsio_divider_function_t *divider_func )
{
    uint32_t clk_rate, divider;

    /* FIXME This code assumes that the HT clock is always employed and remains unchanged
     * throughout the lifetime an initialized I2C/SPI port.
     */
    clk_rate = platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_BACKPLANE );
    wiced_assert("Backplane clock value can not be identified properly", clk_rate != 0);

    divider = (*divider_func)(clk_rate, target_speed_hz);
    divider <<= GSIO_GD_SHIFT;
    divider &= GSIO_GD_MASK;

    REG32(gsio_regs[port].clkdiv) &= ~GSIO_GD_MASK;
    REG32(gsio_regs[port].clkdiv) |= divider;
}

static uint32_t gsio_i2c_divider( uint32_t clk_rate, uint32_t target_speed_hz )
{
    uint32_t divider;

    divider = clk_rate / target_speed_hz;
    if (clk_rate % target_speed_hz) {
        divider++;
    }
    divider /= 4;
    divider -= 1;

    return divider;
}

static platform_result_t i2c_init( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
    uint32_t target_speed_hz;

    if ( ( config->flags & I2C_DEVICE_USE_DMA ) || (config->address_width != I2C_ADDRESS_WIDTH_7BIT && config->address_width != I2C_ADDRESS_WIDTH_10BIT) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    if ( config->speed_mode == I2C_LOW_SPEED_MODE )
    {
        target_speed_hz = 10000;
    }
    else if ( config->speed_mode == I2C_STANDARD_SPEED_MODE )
    {
        target_speed_hz = 100000;
    }
    else if ( config->speed_mode == I2C_HIGH_SPEED_MODE )
    {
        target_speed_hz = 400000;
    }
    else
    {
        return PLATFORM_BADOPTION;
    }

    gsio_i2c_reset_pins( i2c );

    gsio_set_clock(i2c->port, target_speed_hz, gsio_i2c_divider);

    /* XXX Is it appropriate to set the address here? */
    gsio_regs[i2c->port].interface->address = config->address;

    /* WAR to get SCL line in hi-z since GSIO defaults to SPI and
     * drives SCL low.
     */
    gsio_regs[i2c->port].interface->ctrl = GSIO_GM_I2C;
    /* XXX Might need to wait here. */

    return PLATFORM_SUCCESS;
}

static platform_result_t i2c_deinit( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
    UNUSED_PARAMETER( i2c );
    UNUSED_PARAMETER( config );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_i2c_init( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
    platform_result_t result;

    wiced_assert( "bad argument", ( i2c != NULL ) && ( i2c->driver != NULL ) && ( config != NULL ) && ( config->flags & I2C_DEVICE_USE_DMA ) == 0);

    result = I2C_DRIVER(i2c)->init(i2c, config);

    return result;
}

platform_result_t platform_i2c_deinit( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
    platform_result_t result;

    wiced_assert( "bad argument", ( i2c != NULL ) && ( i2c->driver != NULL ) && ( config != NULL ) );

    result = I2C_DRIVER(i2c)->deinit(i2c, config);

    return result;
}

static platform_result_t gsio_wait_for_xfer_to_complete( int port )
{
    uint32_t reg_ctrl_val;

    do
    {
        reg_ctrl_val    = gsio_regs[port].interface->ctrl;
    }
    while
    (
        /* FIXME Add "relaxing" here so as not to starve backplane access. */
        ( ( reg_ctrl_val & GSIO_SB_MASK ) == GSIO_SB_BUSY )
    );

    /* Likely due to .Read=1, .NoAck=0, .NoA=1 and slave-address NACK'd. */
    /* Ignore error since it would take a big-hammer reset to chip common
     * to clear this bit.  Experimentation shows that GSIOCtrl.Error can be safely
     * ignored.
     */
    //wiced_assert("GSIO error", (gsio_regs[i2c->port].interface->ctrl & GSIO_ERROR_MASK) != GSIO_ERROR);

    return PLATFORM_SUCCESS;
}

static platform_result_t i2c_xfer( const platform_i2c_t *i2c, uint32_t regval )
{
    gsio_regs[i2c->port].interface->ctrl  = regval;
    gsio_regs[i2c->port].interface->ctrl |= GSIO_SB_BUSY;

    /* Wait until transaction is complete. */
    return gsio_wait_for_xfer_to_complete( i2c->port );
}

/* GSIO will transfer one more byte before generating stop-condition (P). */
static void i2c_stop( const platform_i2c_t *i2c )
{
    gsio_regs[i2c->port].interface->ctrl  = GSIO_GM_I2C | GSIO_READ_I2C_READ | GSIO_NOACK_I2C_NO_ACK | GSIO_GG_I2C_GENERATE_STOP;
    gsio_regs[i2c->port].interface->ctrl |= GSIO_SB_BUSY;

    (void)gsio_wait_for_xfer_to_complete( i2c->port );

    return;
}

static uint32_t i2c_get_gsio_noa_i2c_start( platform_i2c_bus_address_width_t address_width )
{
    const uint32_t gsio_noa_i2c_start = address_width == I2C_ADDRESS_WIDTH_7BIT  ? GSIO_NOA_I2C_START_7BITS  :
                                        address_width == I2C_ADDRESS_WIDTH_10BIT ? GSIO_NOA_I2C_START_10BITS :
                                                                                   GSIO_NOA_I2C_START_NONE   ;
    wiced_assert( "bad address width", gsio_noa_i2c_start != GSIO_NOA_I2C_START_NONE );

    return gsio_noa_i2c_start;
}

static platform_result_t i2c_read( const platform_i2c_t *i2c, const platform_i2c_config_t *config,
        uint8_t flags, uint8_t *data_in, uint16_t length )
{
    platform_result_t   result;
    unsigned            i;

    result = PLATFORM_SUCCESS;

    if ( data_in == NULL || length == 0 )
    {
        return PLATFORM_BADARG;
    }

    /* XXX This is only necessary if the configuration changes! */
    gsio_regs[i2c->port].interface->address = config->address;

    for ( i = 0; i < length; i++ )
    {
        uint32_t tmpreg = 0;

        gsio_regs[i2c->port].interface->data = 0;

        if ( i == 0 )
        {
            /* First byte. */
            if ( (flags & I2C_START_FLAG) != 0 )
            {
                /* Start condition. */
                tmpreg |= i2c_get_gsio_noa_i2c_start( config->address_width );
            }
        }
        if ( i < length-1 )
        {
            /* Not the last byte. */
            tmpreg |= GSIO_NOACK_I2C_SEND_ACK;
        }
        else
        {
            /* Last byte. */
            if ( (flags & I2C_STOP_FLAG) != 0 )
            {
                /* Stop condition. */
                tmpreg |= GSIO_GG_I2C_GENERATE_STOP;
                tmpreg |= GSIO_NOACK_I2C_NO_ACK;
            }
            else
            {
                tmpreg |= GSIO_NOACK_I2C_SEND_ACK;
            }
        }

        /* Execute GSIO command. */
        result = i2c_xfer(i2c, GSIO_I2C_READ_REG_VAL | tmpreg);

        /* Insert a stop-condition if the slave-address is NACK'd.
         * In the 1-byte case, this will be handled above.  With >1 bytes,
         * the GSIOCtrl.RxAck will be a NACK if the start-procedure NACKs.
         * Note that GSIOCtrl.RxAck seems to reflect our ACK,
         * which will be a NACK for the last byte according the the I2C
         * spec.
         */
        /* FIXME Won't error if slave-address is NACK'd in 1-byte case!
         *
         */
        if ( i == 0 && length > 1 && (gsio_regs[i2c->port].interface->ctrl & GSIO_RXACK_MASK) == GSIO_RXACK_I2C_NACK)
        {
            i2c_stop(i2c);
            result = PLATFORM_ERROR;
        }

        if ( PLATFORM_SUCCESS != result )
        {
            break;
        }

        /* Copy-out. */
        data_in[i] = (gsio_regs[i2c->port].interface->data >> 24) & 0xFF;
    }

    return result;
}

static platform_result_t i2c_write( const platform_i2c_t *i2c, const platform_i2c_config_t *config,
        uint8_t flags, uint8_t *data_out, uint16_t length )
{
    platform_result_t   result;
    unsigned            i;

    result = PLATFORM_SUCCESS;

    if ( data_out == NULL || length == 0 )
    {
        return PLATFORM_BADARG;
    }

    /* XXX This is only necessary if the configuration changes! */
    gsio_regs[i2c->port].interface->address = config->address;

    for ( i = 0; i < length; i++ )
    {
        uint32_t tmpreg = GSIO_NOACK_I2C_CHECK_ACK;

        /* Prepare one byte on GSIOData register. */
        gsio_regs[i2c->port].interface->data = (data_out[i] << 24);

        if ( i == 0 )
        {
            /* First byte. */
            if ( (flags & I2C_START_FLAG) != 0 )
            {
                /* Start-condition. */
                tmpreg |= i2c_get_gsio_noa_i2c_start( config->address_width );;
            }
        }
        if ( i == length-1 )
        {
            /* Last byte. */
            if ( (flags & I2C_STOP_FLAG) != 0 )
            {
                /* Stop condition. */
                tmpreg |= GSIO_GG_I2C_GENERATE_STOP;
            }
        }

        /* Execute GSIO command. */
        result = i2c_xfer(i2c, GSIO_I2C_WRITE_REG_VAL | tmpreg);

        /* Slave NACK'd. */
        if ( ( gsio_regs[i2c->port].interface->ctrl & GSIO_RXACK_MASK ) == GSIO_RXACK_I2C_NACK )
        {
            /* Check if stop-condition already generated. */
            if ( ( tmpreg & GSIO_GG_MASK ) != GSIO_GG_I2C_GENERATE_STOP )
            {
                /* Generate a stop-condition. */
                i2c_stop(i2c);
            }
            /* A NACK is an error. */
            result = PLATFORM_ERROR;
        }

        if ( PLATFORM_SUCCESS != result )
        {
            break;
        }
    }

    return result;
}

wiced_bool_t platform_i2c_probe_device( const platform_i2c_t *i2c, const platform_i2c_config_t *config, int retries )
{
    uint32_t    i;
    uint8_t     dummy[2];
    platform_result_t result;

    /* Read two bytes from the addressed-slave.  The slave-address won't be
     * acknowledged if it isn't on the I2C bus.  The read won't trigger
     * a NACK from the slave (unless of error), since only the receiver can do that.
     * Can't use one byte here because the last byte NACK seems to be reflected back
     * on GSIOCtrl.RxAck.
     * If the slave-address is not acknowledged it will generate an error in GSIOCtrl,
     * which we happily ignore so that we can carry-on.
     * Don't use an I2C write here because the GSIO doesn't support
     * issuing only the I2C start-procedure.
     */
    for ( i = 0; i < retries; i++ )
    {
        result = I2C_DRIVER(i2c)->read(i2c, config, I2C_START_FLAG | I2C_STOP_FLAG, dummy, sizeof dummy);

        if (  result == PLATFORM_SUCCESS )
        {
            return WICED_TRUE;
        }
    }

    return WICED_FALSE;
}

platform_result_t platform_i2c_transfer( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* messages, uint16_t number_of_messages )
{
    platform_result_t result = PLATFORM_SUCCESS;
    uint32_t          message_count;
    uint32_t          retries;

    /* Check for message validity. Combo message is unsupported */
    for ( message_count = 0; message_count < number_of_messages; message_count++ )
    {
        if ( messages[message_count].rx_buffer != NULL && messages[message_count].tx_buffer != NULL )
        {
            return PLATFORM_UNSUPPORTED;
        }

        if ( (messages[message_count].tx_buffer == NULL && messages[message_count].tx_length != 0)  ||
             (messages[message_count].tx_buffer != NULL && messages[message_count].tx_length == 0)  ||
             (messages[message_count].rx_buffer == NULL && messages[message_count].rx_length != 0)  ||
             (messages[message_count].rx_buffer != NULL && messages[message_count].rx_length == 0)  )
        {
            return PLATFORM_BADARG;
        }

        if ( messages[message_count].tx_buffer == NULL && messages[message_count].rx_buffer == NULL )
        {
            return PLATFORM_BADARG;
        }
    }

    gsio_clock_request();

    for ( message_count = 0; message_count < number_of_messages; message_count++ )
    {
        for ( retries = 0; retries < messages[message_count].retries; retries++ )
        {
            if ( messages[message_count].tx_length != 0 )
            {
                result = I2C_DRIVER(i2c)->write(i2c, config, I2C_START_FLAG | I2C_STOP_FLAG, (uint8_t*) messages[message_count].tx_buffer, messages[message_count].tx_length);

                if ( result == PLATFORM_SUCCESS )
                {
                    /* Transaction successful. Break from the inner loop and continue with the next message */
                    break;
                }
            }
            else if ( messages[message_count].rx_length != 0 )
            {
                result = I2C_DRIVER(i2c)->read(i2c, config, I2C_START_FLAG | I2C_STOP_FLAG, (uint8_t*) messages[message_count].rx_buffer, messages[message_count].rx_length);

                if ( result == PLATFORM_SUCCESS )
                {
                    /* Transaction successful. Break from the inner loop and continue with the next message */
                    break;
                }
            }
        }

        /* Check if retry is maxed out. If yes, return immediately */
        if ( retries == messages[message_count].retries && result != PLATFORM_SUCCESS )
        {
            break;
        }
    }

    gsio_clock_release();

    return result;
}

platform_result_t platform_i2c_init_tx_message( platform_i2c_message_t* message, const void* tx_buffer, uint16_t tx_buffer_length, uint16_t retries, wiced_bool_t disable_dma )
{
    wiced_assert( "bad argument", ( message != NULL ) && ( tx_buffer != NULL ) && ( tx_buffer_length != 0 ) );

    /* This I2C peripheral does not support DMA */
    if ( disable_dma == WICED_FALSE )
    {
        return PLATFORM_UNSUPPORTED;
    }

    memset( message, 0x00, sizeof( *message ) );
    message->tx_buffer = tx_buffer;
    message->retries   = retries;
    message->tx_length = tx_buffer_length;
    message->flags     = 0;

    return PLATFORM_SUCCESS;
}

platform_result_t platform_i2c_init_rx_message( platform_i2c_message_t* message, void* rx_buffer, uint16_t rx_buffer_length, uint16_t retries, wiced_bool_t disable_dma )
{
    wiced_assert( "bad argument", ( message != NULL ) && ( rx_buffer != NULL ) && ( rx_buffer_length != 0 ) );

    /* This I2C peripheral does not support DMA */
    if ( disable_dma == WICED_FALSE )
    {
        return PLATFORM_UNSUPPORTED;
    }

    memset( message, 0x00, sizeof( *message ) );

    message->rx_buffer = rx_buffer;
    message->retries   = retries;
    message->rx_length = rx_buffer_length;
    message->flags     = 0;

    return PLATFORM_SUCCESS;
}

/* This repeat-start can only be generated over I2C_0 because I2C_1 does not have GPIO functionality
 *
 * This function transfers control of SCL/SDA temporarily to the GPIO core to block I2C writing on these lines.
 * A dummy message is written over I2C to "pull" both lines high (but since GPIO has control the lines are not effected).
 * Control is returned to I2C which pulls SDA and SCL HIGH (in that order - without generating a stop) and then a start is written.
 */
static platform_result_t i2c_setup_repeated_start( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
    /* Initializing the GPIOs pulls control away from the I2C core.
     *
     * SDA should be initialized before SCL in case the GPIO core pulls the lines high on initialization.
     * GPIOs assume previous state.
     */
    GSIO_VERIFY( platform_gpio_init( i2c->pin_sda, OUTPUT_PUSH_PULL ) );
    GSIO_VERIFY( platform_gpio_init( i2c->pin_scl, OUTPUT_PUSH_PULL ) );

    /* GSIO to configure SCL/SDA active high - no output is seen on SCL/SDA because the GPIOs currently have control.
     *
     * The I2C core has a precondition that both SCL and SDA be HIGH before a start.
     * The I2C core leaves both SCL and SDA HIGH after a stop.
     * Configure GSIO to generate a STOP.  A real STOP isn't generated because GSIO isn't controlling the lines.
     */
    i2c_stop( i2c );

    /* Control is returned to the I2C core.
     *
     * SDA must be returned first because the I2C core will pull the lines high (if they are not high already) immediately upon regaining control.
     */
    GSIO_VERIFY( platform_gpio_deinit( i2c->pin_sda ) );
    GSIO_VERIFY( platform_gpio_deinit( i2c->pin_scl ) );

    return PLATFORM_SUCCESS;
}

static platform_result_t i2c_transfer( const platform_i2c_t* i2c, const platform_i2c_config_t *config, uint16_t flags, void *buffer, uint16_t buffer_length, i2c_io_fn_t* fn )
{
    uint8_t i2c_flags = 0;
    platform_result_t result;

    gsio_clock_request();

    if ( (flags & WICED_I2C_REPEATED_START_FLAG) != 0 )
    {
        /* Combined message is being requested. */
        if ( GSIO_I2C_REPEATED_START_NEEDS_GPIO != 0 )
        {
            /* The I2C peripheral pins need to be GPIO muxable. */
            if ( i2c->pin_scl == NULL || i2c->pin_sda == NULL )
            {
                wiced_assert( "repeated-start unsupported on this I2C port", 0==1 );
                return PLATFORM_UNSUPPORTED;
            }

            /* Setup the repeated-start precondition. */
            GSIO_VERIFY( i2c_setup_repeated_start( i2c, config ) );
        }
    }

    if ( ( flags & ( WICED_I2C_START_FLAG | WICED_I2C_REPEATED_START_FLAG) ) != 0 )
    {
        i2c_flags |= I2C_START_FLAG;
    }
    if ( ( flags & WICED_I2C_STOP_FLAG ) != 0 )
    {
        i2c_flags |= I2C_STOP_FLAG;
    }

    result = (*fn)( i2c, config, i2c_flags, buffer, buffer_length );

    gsio_clock_release();

    return result;
}


platform_result_t platform_i2c_write( const platform_i2c_t* i2c, const platform_i2c_config_t* config, uint16_t flags, const void* buffer, uint16_t buffer_length )
{
    platform_result_t result;

    result = I2C_DRIVER(i2c)->transfer(i2c, config, flags, (void *) buffer, buffer_length, I2C_DRIVER(i2c)->write);

    return result;
}


platform_result_t platform_i2c_read( const platform_i2c_t* i2c, const platform_i2c_config_t* config, uint16_t flags, void* buffer, uint16_t buffer_length )
{
    platform_result_t result;

    result = I2C_DRIVER(i2c)->transfer(i2c, config, flags, (void *) buffer, buffer_length, I2C_DRIVER(i2c)->read);

    return result;
}

#if 0
platform_result_t platform_i2c_init_combined_message( platform_i2c_message_t* message, const void* tx_buffer, void* rx_buffer, uint16_t tx_buffer_length, uint16_t rx_buffer_length, uint16_t retries, wiced_bool_t disable_dma )
{
    wiced_assert( "bad argument", ( message != NULL ) && ( tx_buffer != NULL ) && ( tx_buffer_length != 0 ) && ( rx_buffer != NULL ) && ( rx_buffer_length != 0 ) );

    /* This I2C peripheral does not support DMA. */
    if ( disable_dma == WICED_FALSE )
    {
        return PLATFORM_UNSUPPORTED;
    }

    memset( message, 0x00, sizeof( *message ) );

    message->rx_buffer = rx_buffer;
    message->tx_buffer = tx_buffer;
    message->combined  = WICED_TRUE;
    message->retries   = retries;
    message->tx_length = tx_buffer_length;
    message->rx_length = rx_buffer_length;
    message->flags     = 0;

    return PLATFORM_SUCCESS;
}
#endif

/*****************************************************************************
 * 4390x I2C Bit-Banging Driver Implementation. WICED port based on FreeBSD. *
 * (Refer to the comment header at the top of this file for FreeBSD license) *
 *****************************************************************************/

/* signal toggle delay (default is low speed) */
static int i2c_bb_signal_toggle_delay_us = 100;

/* signal toggle timeout factor (default is 100) */
#define I2C_BB_SIGNAL_TOGGLE_TIMEOUT 100

/* Read/Write bit */
#define I2C_BB_RW_BIT 0x1

/* Ack not received until timeout */
#define I2C_BB_ENOACK 0x2

#define I2C_BB_SETDIR_INPUT( pin )     ( platform_gpio_init( pin, INPUT_PULL_UP ) )
#define I2C_BB_SETDIR_OUTPUT( pin )    ( platform_gpio_init( pin, OUTPUT_PUSH_PULL ) )

#define I2C_BB_GETPIN( pin )    ( platform_gpio_input_get( pin ) )

/* Gets the value on the SCL line */
#define I2C_BB_GETSCL( i2c )    I2C_BB_GETPIN( i2c->pin_scl )
/* Gets the value on the SDA line */
#define I2C_BB_GETSDA( i2c )    I2C_BB_GETPIN( i2c->pin_sda )

#define I2C_BB_SETPIN( pin, val, udelay )                                               \
    do                                                                                  \
    {                                                                                   \
        if ( val )                                                                      \
        {                                                                               \
            platform_gpio_output_high( pin );                                           \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            platform_gpio_output_low( pin );                                            \
        }                                                                               \
        OSL_DELAY( udelay );                                                            \
    }                                                                                   \
    while ( 0 )

/* Sets the value on the SCL line. Implements clock stretching to handle wait states. */
#define I2C_BB_SETSCL( i2c, val )                                                       \
    do                                                                                  \
    {                                                                                   \
        int k = 0;                                                                      \
                                                                                        \
        if ( val )                                                                      \
        {                                                                               \
            I2C_BB_SETPIN( i2c->pin_scl, val, 0 );                                      \
                                                                                        \
            I2C_BB_SETDIR_INPUT( i2c->pin_scl );                                        \
                                                                                        \
            while ( !I2C_BB_GETSCL( i2c ) && k++ < I2C_BB_SIGNAL_TOGGLE_TIMEOUT )       \
            {                                                                           \
                OSL_DELAY( i2c_bb_signal_toggle_delay_us );                             \
                I2C_BB_SETDIR_OUTPUT( i2c->pin_scl );                                   \
                I2C_BB_SETPIN( i2c->pin_scl, val, 0 );                                  \
                I2C_BB_SETDIR_INPUT( i2c->pin_scl );                                    \
            }                                                                           \
                                                                                        \
            I2C_BB_SETDIR_OUTPUT( i2c->pin_scl );                                       \
        }                                                                               \
                                                                                        \
        I2C_BB_SETPIN( i2c->pin_scl, val, i2c_bb_signal_toggle_delay_us );              \
    }                                                                                   \
    while ( 0 )

/* Sets the value on the SDA line */
#define I2C_BB_SETSDA( i2c, val )    I2C_BB_SETPIN( i2c->pin_sda, val, i2c_bb_signal_toggle_delay_us )

/* Sets the values on SCL and SDA lines */
#define I2C_BB_SET( i2c, ctrl, data )                                                   \
    do                                                                                  \
    {                                                                                   \
        I2C_BB_SETSCL( i2c, ctrl );                                                     \
        I2C_BB_SETSDA( i2c, data );                                                     \
    }                                                                                   \
    while ( 0 )

static int i2c_bb_debug = 0;

#define I2C_BB_DEBUG( x )                                                               \
    do                                                                                  \
    {                                                                                   \
        if ( i2c_bb_debug )                                                             \
        {                                                                               \
            ( x );                                                                      \
        }                                                                               \
    }                                                                                   \
    while ( 0 )

/*
 * Resets the GPIO-muxed bit-banging I2C pins to HW/Power-On default function,
 * thereby clearing any previous initialization of GPIO alternate function.
 */
static inline void
i2c_bb_reset_pins( const platform_i2c_t* i2c )
{
    if ( i2c->pin_sda != NULL )
    {
        platform_gpio_deinit( i2c->pin_sda );
    }

    if ( i2c->pin_scl != NULL )
    {
        platform_gpio_deinit( i2c->pin_scl );
    }
}

/* Transmits a high bit over the I2C bus */
static void
i2c_bb_one( const platform_i2c_t* i2c, int timeout )
{
    I2C_BB_SETSCL( i2c, 0);
    I2C_BB_SETSDA( i2c, 1);
    I2C_BB_SETSCL( i2c, 1);
    I2C_BB_SETSCL( i2c, 0);

    return;
}

/* Transmits a low bit over the I2C bus */
static void
i2c_bb_zero( const platform_i2c_t* i2c, int timeout )
{
    I2C_BB_SETSCL( i2c, 0);
    I2C_BB_SETSDA( i2c, 0);
    I2C_BB_SETSCL( i2c, 1);
    I2C_BB_SETSCL( i2c, 0);

    return;
}

/*
 * Waiting for ACKNOWLEDGE.
 *
 * When a chip is being addressed or has received data it will issue an
 * ACKNOWLEDGE pulse. Therefore the MASTER must release the DATA line
 * (set it to high level) and then release the CLOCK line.
 * Now it must wait for the SLAVE to pull the DATA line low.
 * Actually on the bus this looks like a START condition so nothing happens
 * because of the fact that the IC's that have not been addressed are doing
 * nothing.
 *
 * When the SLAVE has pulled this line low the MASTER will take the CLOCK
 * line low and then the SLAVE will release the SDA (data) line.
 */
static int
i2c_bb_ack( const platform_i2c_t* i2c, int timeout )
{
    int noack;
    int k = 0;

    I2C_BB_SET( i2c, 0, 1 );

    I2C_BB_SETDIR_INPUT( i2c->pin_sda );
    I2C_BB_SETSCL( i2c, 1 );

    do
    {
        noack = I2C_BB_GETSDA( i2c );
        if ( !noack )
        {
            break;
        }
        OSL_DELAY( 1 );
        k++;
    }
    while ( k < timeout );

    I2C_BB_SETSCL( i2c, 0 );
    I2C_BB_SETDIR_OUTPUT( i2c->pin_sda );

    I2C_BB_DEBUG( printf("%c ", noack ? '-' : '+') );

    return ( noack );
}

/* Transmits a single byte over the I2C bus */
static void
i2c_bb_sendbyte( const platform_i2c_t* i2c, uint8_t data, int timeout )
{
    int i;

    for ( i = 7 ; i >= 0 ; i-- )
    {
        if ( data & (1 << i) )
        {
            i2c_bb_one(i2c, timeout);
        }
        else
        {
            i2c_bb_zero(i2c, timeout);
        }
    }

    I2C_BB_DEBUG( printf("w%02x", (int)data) );

    return;
}

/* Receives a single byte over the I2C bus. NACK if last byte, ACK otherwise. */
static uint8_t
i2c_bb_readbyte( const platform_i2c_t* i2c, int last, int timeout )
{
    int i;
    uint8_t data = 0;

    I2C_BB_SET( i2c, 0, 1 );

    I2C_BB_SETDIR_INPUT( i2c->pin_sda );

    for ( i = 7 ; i >= 0 ; i-- )
    {
        I2C_BB_SETSCL( i2c, 1 );
        if ( I2C_BB_GETSDA( i2c ) )
        {
            data |= (1 << i);
        }
        I2C_BB_SETSCL( i2c, 0 );
    }

    I2C_BB_SETDIR_OUTPUT( i2c->pin_sda );

    if ( last )
    {
        i2c_bb_one( i2c, timeout );
    }
    else
    {
        i2c_bb_zero( i2c, timeout );
    }

    I2C_BB_DEBUG( printf("r%02x%c ", (int)data, last ? '-' : '+') );

    return data;
}

/* Transmits a STOP condition over the I2C bus */
static int
i2c_bb_stop( const platform_i2c_t* i2c )
{
    I2C_BB_SETSCL( i2c, 0 );
    I2C_BB_SETSDA( i2c, 0 );
    I2C_BB_SETSCL( i2c, 1 );
    I2C_BB_SETSDA( i2c, 1 );

    I2C_BB_DEBUG( printf(">") );
    I2C_BB_DEBUG( printf("\n") );

    return ( 0 );
}

/* Transmits a START condition over the I2C bus, followed by address byte */
static int
i2c_bb_start( const platform_i2c_t* i2c, uint8_t slave, int timeout )
{
    int error;

    I2C_BB_DEBUG( printf("<") );

    I2C_BB_SETSDA( i2c, 1 );
    I2C_BB_SETSCL( i2c, 1 );
    I2C_BB_SETSDA( i2c, 0 );
    I2C_BB_SETSCL( i2c, 0 );

    /* send address */
    i2c_bb_sendbyte( i2c, slave, timeout);

    /* check for ack */
    if ( i2c_bb_ack( i2c, timeout ) )
    {
        error = I2C_BB_ENOACK;
        goto error;
    }

    return ( 0 );

error:
    i2c_bb_stop( i2c );
    return ( error );
}

/* Transmits a sequence of bytes over the I2C bus, checking ACK for each byte sent */
static int
i2c_bb_writebytes( const platform_i2c_t* i2c, const uint8_t* buf, uint16_t len, int* sent, int timeout )
{
    int bytes, error = 0;

    bytes = 0;
    while ( len )
    {
        /* send byte */
        i2c_bb_sendbyte( i2c, *buf++, timeout );

        /* check for ack */
        if ( i2c_bb_ack( i2c, timeout ) )
        {
            error = I2C_BB_ENOACK;
            goto error;
        }
        bytes++;
        len--;
    }

error:
    *sent = bytes;
    return ( error );
}

/* Receives a sequence of bytes over the I2C bus */
static int
i2c_bb_readbytes( const platform_i2c_t* i2c, uint8_t* buf, uint16_t len, int* read, int last, int timeout )
{
    int bytes;

    bytes = 0;
    while ( len )
    {
        *buf++ = i2c_bb_readbyte( i2c, (len == 1) ? last : 0, timeout );

        bytes++;
        len--;
    }

    *read = bytes;
    return ( 0 );
}

static platform_result_t
i2c_bb_init( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
    if ( ( config->flags & I2C_DEVICE_USE_DMA ) || ( config->address_width != I2C_ADDRESS_WIDTH_7BIT ) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    if ( i2c->pin_sda == NULL || i2c->pin_scl == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    if ( config->speed_mode == I2C_LOW_SPEED_MODE )
    {
        i2c_bb_signal_toggle_delay_us = 100;
    }
    else if ( config->speed_mode == I2C_STANDARD_SPEED_MODE )
    {
        i2c_bb_signal_toggle_delay_us = 10;
    }
    else if ( config->speed_mode == I2C_HIGH_SPEED_MODE )
    {
        i2c_bb_signal_toggle_delay_us = 5;
    }
    else
    {
        return PLATFORM_BADOPTION;
    }

    i2c_bb_reset_pins( i2c );

    I2C_BB_SETDIR_OUTPUT( i2c->pin_sda );
    I2C_BB_SETDIR_OUTPUT( i2c->pin_scl );

    OSL_DELAY( i2c_bb_signal_toggle_delay_us );

    return PLATFORM_SUCCESS;
}

static platform_result_t
i2c_bb_deinit( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
    UNUSED_PARAMETER( config );

    platform_gpio_deinit( i2c->pin_sda );
    platform_gpio_deinit( i2c->pin_scl );

    return PLATFORM_SUCCESS;
}

static platform_result_t
i2c_bb_read( const platform_i2c_t *i2c, const platform_i2c_config_t *config, uint8_t flags, uint8_t *data_in, uint16_t length )
{
    int error;
    int read;
    uint8_t address;

    if ( data_in == NULL || length == 0 )
    {
        return PLATFORM_BADARG;
    }

    /* Slave address and R/W bit */
    address = config->address << 1;
    address |= I2C_BB_RW_BIT;

    /* Start Condition */
    if ( (flags & I2C_START_FLAG) != 0 )
    {
        error = i2c_bb_start( i2c, address, I2C_BB_SIGNAL_TOGGLE_TIMEOUT );
        if ( error != 0 )
        {
            return PLATFORM_ERROR;
        }
    }

    /* Read the data */
    error = i2c_bb_readbytes( i2c, data_in, length, &read, ((flags & I2C_STOP_FLAG) != 0) ? 1 : 0, I2C_BB_SIGNAL_TOGGLE_TIMEOUT );

    /* Stop Condition */
    if ( (error != 0) || ((flags & I2C_STOP_FLAG) != 0) )
    {
        i2c_bb_stop( i2c );
    }

    if ( error != 0 )
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t
i2c_bb_write( const platform_i2c_t *i2c, const platform_i2c_config_t *config, uint8_t flags, uint8_t *data_out, uint16_t length )
{
    int error;
    int sent;
    uint8_t address;

    if ( data_out == NULL || length == 0 )
    {
        return PLATFORM_BADARG;
    }

    /* Slave address and R/W bit */
    address = config->address << 1;
    address &= ~I2C_BB_RW_BIT;

    /* Start Condition */
    if ( (flags & I2C_START_FLAG) != 0 )
    {
        error = i2c_bb_start( i2c, address, I2C_BB_SIGNAL_TOGGLE_TIMEOUT );
        if ( error != 0 )
        {
            return PLATFORM_ERROR;
        }
    }

    /* Write the data */
    error = i2c_bb_writebytes( i2c, data_out, length, &sent, I2C_BB_SIGNAL_TOGGLE_TIMEOUT );

    /* Stop Condition */
    if ( (error != 0) || ((flags & I2C_STOP_FLAG) != 0) )
    {
        i2c_bb_stop( i2c );
    }

    if ( error != 0 )
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t
i2c_bb_transfer( const platform_i2c_t* i2c, const platform_i2c_config_t *config, uint16_t flags, void *buffer, uint16_t buffer_length, i2c_io_fn_t* fn )
{
    uint8_t i2c_flags = 0;

    if ( ( flags & (WICED_I2C_START_FLAG | WICED_I2C_REPEATED_START_FLAG) ) != 0 )
    {
        i2c_flags |= I2C_START_FLAG;
    }
    if ( ( flags & WICED_I2C_STOP_FLAG ) != 0 )
    {
        i2c_flags |= I2C_STOP_FLAG;
    }

    return (*fn)( i2c, config, i2c_flags, buffer, buffer_length );
}

/*****************************************************************************/

static uint32_t gsio_spi_divider( uint32_t clk_rate, uint32_t target_speed_hz )
{
    uint32_t divider;

    divider = clk_rate / target_speed_hz;
    divider -= 2;

    return divider;
}

static void gsio_spi_pinmux_init( const platform_spi_t* spi )
{
    /* Setup the pin-mux for SPI MOSI function */
    if ( spi->pin_mosi != NULL )
    {
        if ( spi->port == BCM4390X_SPI_0 )
        {
            platform_pinmux_init( spi->pin_mosi->pin, PIN_FUNCTION_SPI0_MOSI );
        }
        else if ( spi->port == BCM4390X_SPI_1 )
        {
            platform_pinmux_init( spi->pin_mosi->pin, PIN_FUNCTION_SPI1_MOSI );
        }
    }

    /* Setup the pin-mux for SPI MISO function */
    if ( spi->pin_miso != NULL )
    {
        if ( spi->port == BCM4390X_SPI_0 )
        {
            platform_pinmux_init( spi->pin_miso->pin, PIN_FUNCTION_SPI0_MISO );
        }
        else if ( spi->port == BCM4390X_SPI_1 )
        {
            platform_pinmux_init( spi->pin_miso->pin, PIN_FUNCTION_SPI1_MISO );

            /* Workaround from ASIC team for routing SPI_1 MISO
             * on the muxed pin (instead of the dedicated pin) */
            platform_gci_chipcontrol( GCI_CHIPCONTROL_REG_9, 0, PIN_FUNCTION_SEL_SPI1_MISO );
        }
    }

    /* Setup the pin-mux for SPI Clock function */
    if ( spi->pin_clock != NULL )
    {
        if ( spi->port == BCM4390X_SPI_0 )
        {
            platform_pinmux_init( spi->pin_clock->pin, PIN_FUNCTION_SPI0_CLK );
        }
        else if ( spi->port == BCM4390X_SPI_1 )
        {
            platform_pinmux_init( spi->pin_clock->pin, PIN_FUNCTION_SPI1_CLK );
        }
    }

    /* Setup the pin-mux for SPI CS function */
    if ( spi->pin_cs != NULL )
    {
        if ( spi->port == BCM4390X_SPI_0 )
        {
            platform_pinmux_init( spi->pin_cs->pin, PIN_FUNCTION_SPI0_CS );
        }
        else if ( spi->port == BCM4390X_SPI_1 )
        {
            platform_pinmux_init( spi->pin_cs->pin, PIN_FUNCTION_SPI1_CS );
        }
    }
}

static platform_result_t spi_chip_select_toggle( const platform_spi_t* spi, const platform_spi_config_t* config, wiced_bool_t activate )
{
    if ( config->chip_select != NULL )
    {
        if ( activate == WICED_TRUE )
        {
            if ( config->mode & SPI_CS_ACTIVE_HIGH )
            {
                platform_gpio_output_high( config->chip_select );
            }
            else
            {
                platform_gpio_output_low( config->chip_select );
            }
        }
        else
        {
            if ( config->mode & SPI_CS_ACTIVE_HIGH )
            {
                platform_gpio_output_low( config->chip_select );
            }
            else
            {
                platform_gpio_output_high( config->chip_select );
            }
        }
    }
    else
    {
        if ( activate == WICED_TRUE)
        {
            gsio_regs[ spi->port ].interface->ctrl |=  GSIO_GG_SPI_CHIP_SELECT;
        }
        else
        {
            gsio_regs[ spi->port ].interface->ctrl &= ( ~GSIO_GG_SPI_CHIP_SELECT );
        }
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t spi_init( const platform_spi_t* spi, const platform_spi_config_t* config )
{
    platform_result_t result = PLATFORM_SUCCESS;

    gsio_set_clock(spi->port, config->speed, gsio_spi_divider);

    gsio_regs[spi->port].interface->ctrl = ( 0 & GSIO_GO_MASK) | GSIO_GC_SPI_NOD_DATA_IO | GSIO_NOD_1 | GSIO_GM_SPI;

    gsio_spi_pinmux_init(spi);

    if ( config->chip_select != NULL )
    {
        /* GPIO used as chip select. */
        result = platform_gpio_init( config->chip_select, OUTPUT_PUSH_PULL );
    }
    else
    {
        /* Use h/w peripheral pin for chip select. */
        ;
    }

    /* XXX Might need to wait here. */

    return result;
}

static platform_result_t spi_deinit( const platform_spi_t* spi )
{
    UNUSED_PARAMETER( spi );
    /* TODO: unimplemented */
    return PLATFORM_UNSUPPORTED;
}

static platform_result_t spi_transfer_nosetup( const platform_spi_t* spi, const platform_spi_config_t* config, const uint8_t* send_ptr, uint8_t* rcv_ptr, uint32_t length )
{
    uint32_t gsio_control_register_backup;
    uint32_t gsio_control_register_GSIO_NOD_1;
    uint32_t gsio_control_register_GSIO_NOD_4;

    /* Save value of gsio control register */
    gsio_control_register_backup = gsio_regs[spi->port].interface->ctrl;

    gsio_control_register_GSIO_NOD_1 = ( gsio_regs[spi->port].interface->ctrl & ~( (uint32_t)( GSIO_NOD_MASK ) ) ) | GSIO_NOD_1;
    gsio_control_register_GSIO_NOD_4 = ( gsio_regs[spi->port].interface->ctrl & ~( (uint32_t)( GSIO_NOD_MASK ) ) ) | GSIO_NOD_4;

    while ( length )
    {
        uint32_t data = 0xFFFFFFFF;
        uint8_t sub_length;

        if ( length >= 4 )
        {
            gsio_regs[spi->port].interface->ctrl = gsio_control_register_GSIO_NOD_4;
            sub_length = 4;
        }
        else
        {
            gsio_regs[spi->port].interface->ctrl = gsio_control_register_GSIO_NOD_1;
            sub_length = 1;
        }

        if ( send_ptr != NULL )
        {
            memcpy( &data, send_ptr, sub_length );
            send_ptr += sub_length;
        }

        gsio_regs[spi->port].interface->data = data;
        gsio_regs[spi->port].interface->ctrl |= GSIO_SB_BUSY;
        gsio_wait_for_xfer_to_complete( spi->port );
        data = gsio_regs[spi->port].interface->data;

        if ( rcv_ptr != NULL )
        {
            memcpy( rcv_ptr, &data, sub_length );
            rcv_ptr += sub_length;
        }

        length -= sub_length;
    }

    /* Restore value of gsio control register */
    gsio_regs[spi->port].interface->ctrl = gsio_control_register_backup;

    return PLATFORM_SUCCESS;
}

static platform_result_t spi_transfer( const platform_spi_t* spi, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
{
    platform_result_t result      = PLATFORM_SUCCESS;
    uint16_t          i;

    platform_spi_core_clock_toggle( WICED_TRUE );

    result = spi_init( spi, config );
    if ( result != PLATFORM_SUCCESS )
    {
        platform_spi_core_clock_toggle( WICED_FALSE );
        return result;
    }

    /* Activate chip select */
    spi_chip_select_toggle( spi, config, WICED_TRUE );

    /* SPI transfer after init and CS */
    for ( i = 0; i < number_of_segments; i++ )
    {
        const uint8_t* send_ptr = ( const uint8_t* )segments[i].tx_buffer;
        uint8_t*       rcv_ptr  = ( uint8_t* )segments[i].rx_buffer;

        spi_transfer_nosetup( spi, config, send_ptr, rcv_ptr, segments[i].length );
    }

    /* Deassert chip select */
    spi_chip_select_toggle( spi, config, WICED_FALSE );

    platform_spi_core_clock_toggle( WICED_FALSE );

    return result;
}

/*****************************************************************************
 *            4390x SPI Master Bit-Banging Driver Implementation.            *
 *                                                                           *
 *****************************************************************************/

#define SPI_BB_CS_SETUP_DELAY_CYCLES 25
#define SPI_BB_CS_HOLD_DELAY_CYCLES  25

static uint32_t spi_bb_clock_toggle_delay_cycles;

#ifdef SPI_BB_TRANSFER_DELAY_SUPPORT
#define SPI_BB_DELAY( cycles )                                                          \
    do                                                                                  \
    {                                                                                   \
        uint32_t delay_cycles = cycles;                                                 \
        uint32_t last_stamp  = PLATFORM_APPSCR4->cycle_cnt;                             \
                                                                                        \
        while (delay_cycles != 0)                                                       \
        {                                                                               \
            uint32_t current_stamp = PLATFORM_APPSCR4->cycle_cnt;                       \
            uint32_t passed_cycles = current_stamp - last_stamp;                        \
                                                                                        \
            if (passed_cycles >= delay_cycles)                                          \
            {                                                                           \
                break;                                                                  \
            }                                                                           \
                                                                                        \
            delay_cycles -= passed_cycles;                                              \
            last_stamp = current_stamp;                                                 \
        }                                                                               \
    }                                                                                   \
    while ( 0 )
#else
#define SPI_BB_DELAY( cycles )
#endif /* SPI_BB_TRANSFER_DELAY_SUPPORT */

#define SPI_BB_SETDIR_INPUT( pin )     ( platform_gpio_init( pin, INPUT_PULL_UP ) )
#define SPI_BB_SETDIR_OUTPUT( pin )    ( platform_gpio_init( pin, OUTPUT_PUSH_PULL ) )

#define SPI_BB_GETPIN( pin )         ( ( platform_gpio_input_get( pin ) == WICED_TRUE ) ? 1 : 0 )
#define SPI_BB_SETPIN( pin, val )    ( val ? platform_gpio_output_high( pin ) : platform_gpio_output_low( pin ) )

/* SPI Clock Polarity (CPOL) configuration */
#define SPI_BB_CPOL_HIGH( config )        ( ( config->mode & SPI_CLOCK_IDLE_HIGH ) ? 1 : 0 )
/* SPI Clock Phase (CPHA) configuration */
#define SPI_BB_CPHA_HIGH( config )        ( ( config->mode & SPI_CLOCK_RISING_EDGE ) ? SPI_BB_CPOL_HIGH( config ) : !SPI_BB_CPOL_HIGH( config ) )
/* SPI Data Transfer Order configuration */
#define SPI_BB_MSB_FIRST( config )        ( ( config->mode & SPI_MSB_FIRST ) ? 1 : 0 )
/* SPI Chip Select Level configuration */
#define SPI_BB_CS_ACTIVEHIGH( config )    ( ( config->mode & SPI_CS_ACTIVE_HIGH ) ? 1 : 0 )

/* Sets the value on the CLOCK line */
#define SPI_BB_SET_CLOCK( spi, val )    SPI_BB_SETPIN( spi->pin_clock, val )
/* Sets the value on the MOSI line */
#define SPI_BB_SET_MOSI( spi, val )     SPI_BB_SETPIN( spi->pin_mosi, val )
/* Gets the value on the MISO line */
#define SPI_BB_GET_MISO( spi )          SPI_BB_GETPIN( spi->pin_miso )
/* Sets the value on the CS line */
#define SPI_BB_SET_CS( spi, val )       SPI_BB_SETPIN( spi->pin_cs, val )

static void spi_bb_cs_activate( const platform_spi_t* spi, const platform_spi_config_t* config, wiced_bool_t activate )
{
    SPI_BB_DELAY( SPI_BB_CS_SETUP_DELAY_CYCLES );
    SPI_BB_SET_CS( spi, ((activate == WICED_TRUE) ? SPI_BB_CS_ACTIVEHIGH(config) : !SPI_BB_CS_ACTIVEHIGH(config)) );
    SPI_BB_DELAY( SPI_BB_CS_HOLD_DELAY_CYCLES );
}

static platform_result_t spi_bb_chip_select_toggle( const platform_spi_t* spi, const platform_spi_config_t* config, wiced_bool_t activate )
{
    spi_bb_cs_activate( spi, config, activate );

    return PLATFORM_SUCCESS;
}

static platform_result_t spi_bb_init( const platform_spi_t* spi, const platform_spi_config_t* config )
{
    if ( (spi->pin_clock == NULL) || (spi->pin_mosi == NULL) || (spi->pin_miso == NULL) || (spi->pin_cs == NULL) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Setup the signal toggle delay cycles for SPI CLOCK */
    spi_bb_clock_toggle_delay_cycles = (((CPU_CLOCK_HZ + (config->speed / 2)) / config->speed) + 1) / 2;

    /* Setup the GPIOs for CLOCK, MOSI, MISO, CS */
    SPI_BB_SETDIR_OUTPUT( spi->pin_clock );
    SPI_BB_SETDIR_OUTPUT( spi->pin_mosi );
    SPI_BB_SETDIR_INPUT( spi->pin_miso );
    SPI_BB_SETDIR_OUTPUT( spi->pin_cs );

    return PLATFORM_SUCCESS;
}

static platform_result_t spi_bb_deinit( const platform_spi_t* spi )
{
    platform_gpio_deinit( spi->pin_clock );
    platform_gpio_deinit( spi->pin_mosi );
    platform_gpio_deinit( spi->pin_miso );
    platform_gpio_deinit( spi->pin_cs );

    return PLATFORM_SUCCESS;
}

static uint8_t spi_bb_transfer_byte( const platform_spi_t* spi, const platform_spi_config_t* config, uint8_t send_byte )
{
    uint8_t spi_mode;
    uint8_t rcv_byte;
    uint8_t data_bit;

    spi_mode = ( SPI_BB_CPOL_HIGH(config) << 1 ) | ( SPI_BB_CPHA_HIGH(config) );
    rcv_byte = 0x00;

    if ( SPI_BB_MSB_FIRST(config) != 0 )
    {
        data_bit = 0x80;
    }
    else
    {
        data_bit = 0x01;
    }

    while ( data_bit )
    {
        switch (spi_mode)
        {
            case 0: /* CPOL = 0, CPHA = 0 */
                /* output data on MOSI */
                SPI_BB_SET_MOSI(spi, ((send_byte & data_bit) ? 1 : 0));

                /* CLOCK setup time delay */
                SPI_BB_DELAY(spi_bb_clock_toggle_delay_cycles);

                /* drive CLOCK to high */
                SPI_BB_SET_CLOCK(spi, 1);

                /* sample data on MISO */
                if ( SPI_BB_GET_MISO(spi) != 0 )
                {
                    rcv_byte |= data_bit;
                }

                /* CLOCK hold time delay */
                SPI_BB_DELAY(spi_bb_clock_toggle_delay_cycles);

                /* drive CLOCK to low */
                SPI_BB_SET_CLOCK(spi, 0);

                break;
            case 1: /* CPOL = 0, CPHA = 1 */
                /* drive CLOCK to high */
                SPI_BB_SET_CLOCK(spi, 1);

                /* output data on MOSI */
                SPI_BB_SET_MOSI(spi, ((send_byte & data_bit) ? 1 : 0));

                /* CLOCK setup time delay */
                SPI_BB_DELAY(spi_bb_clock_toggle_delay_cycles);

                /* drive CLOCK to low */
                SPI_BB_SET_CLOCK(spi, 0);

                /* sample data on MISO */
                if ( SPI_BB_GET_MISO(spi) != 0 )
                {
                    rcv_byte |= data_bit;
                }

                /* CLOCK hold time delay */
                SPI_BB_DELAY(spi_bb_clock_toggle_delay_cycles);

                break;
            case 2: /* CPOL = 1, CPHA = 0 */
                /* output data on MOSI */
                SPI_BB_SET_MOSI(spi, ((send_byte & data_bit) ? 1 : 0));

                /* CLOCK setup time delay */
                SPI_BB_DELAY(spi_bb_clock_toggle_delay_cycles);

                /* drive CLOCK to low */
                SPI_BB_SET_CLOCK(spi, 0);

                /* sample data on MISO */
                if ( SPI_BB_GET_MISO(spi) != 0 )
                {
                    rcv_byte |= data_bit;
                }

                /* CLOCK hold time delay */
                SPI_BB_DELAY(spi_bb_clock_toggle_delay_cycles);

                /* drive CLOCK to high */
                SPI_BB_SET_CLOCK(spi, 1);

                break;
            case 3: /* CPOL = 1, CPHA = 1 */
                /* drive CLOCK to low */
                SPI_BB_SET_CLOCK(spi, 0);

                /* output data on MOSI */
                SPI_BB_SET_MOSI(spi, ((send_byte & data_bit) ? 1 : 0));

                /* CLOCK setup time delay */
                SPI_BB_DELAY(spi_bb_clock_toggle_delay_cycles);

                /* drive CLOCK to high */
                SPI_BB_SET_CLOCK(spi, 1);

                /* sample data on MISO */
                if ( SPI_BB_GET_MISO(spi) != 0 )
                {
                    rcv_byte |= data_bit;
                }

                /* CLOCK hold time delay */
                SPI_BB_DELAY(spi_bb_clock_toggle_delay_cycles);

                break;
        }

        if ( SPI_BB_MSB_FIRST(config) != 0 )
        {
            data_bit >>= 1;
        }
        else
        {
            data_bit <<= 1;
        }
    }

    return rcv_byte;
}

static platform_result_t spi_bb_transfer_nosetup( const platform_spi_t* spi, const platform_spi_config_t* config, const uint8_t* send_ptr, uint8_t* rcv_ptr, uint32_t length )
{
    while ( length-- )
    {
        uint8_t send_byte = 0x00;
        uint8_t rcv_byte = 0x00;

        if ( send_ptr != NULL )
        {
            send_byte = *send_ptr++;
        }

        rcv_byte = spi_bb_transfer_byte( spi, config, send_byte );

        if ( rcv_ptr != NULL )
        {
            *rcv_ptr++ = rcv_byte;
        }
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t spi_bb_transfer( const platform_spi_t* spi, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
{
    platform_result_t result = PLATFORM_SUCCESS;
    uint16_t i;

    platform_spi_core_clock_toggle( WICED_TRUE );

    /* Activate chip select */
    spi_bb_cs_activate( spi, config, WICED_TRUE );

    /* Start SPI transfer */
    for ( i = 0; i < number_of_segments; i++ )
    {
        const uint8_t* send_ptr = ( const uint8_t* )segments[i].tx_buffer;
        uint8_t*       rcv_ptr  = ( uint8_t* )segments[i].rx_buffer;

        spi_bb_transfer_nosetup( spi, config, send_ptr, rcv_ptr, segments[i].length );
    }

    /* Deactivate chip select */
    spi_bb_cs_activate( spi, config, WICED_FALSE );

    platform_spi_core_clock_toggle( WICED_FALSE );

    return result;
}

platform_result_t platform_spi_init( const platform_spi_t* spi, const platform_spi_config_t* config )
{
    platform_result_t result;

    wiced_assert( "bad argument", ( spi != NULL ) && ( spi->driver != NULL ) && ( config != NULL ) && (( config->mode & SPI_USE_DMA ) == 0 ));

    result = SPI_DRIVER(spi)->init(spi, config);

    return result;
}

platform_result_t platform_spi_deinit( const platform_spi_t* spi )
{
    platform_result_t result;

    wiced_assert( "bad argument", ( spi != NULL ) && ( spi->driver != NULL ) );

    result = SPI_DRIVER(spi)->deinit(spi);

    return result;
}

platform_result_t platform_spi_transfer( const platform_spi_t* spi, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
{
    platform_result_t result;

    wiced_assert( "bad argument", ( spi != NULL ) && ( spi->driver != NULL ) && ( config != NULL ) && ( segments != NULL ) && ( number_of_segments != 0 ) && ( config->bits == 8 ) );

    result = SPI_DRIVER(spi)->transfer(spi, config, segments, number_of_segments);

    return result;
}

platform_result_t platform_spi_transfer_nosetup( const platform_spi_t* spi, const platform_spi_config_t* config, const uint8_t* send_ptr, uint8_t* rcv_ptr, uint32_t length )
{
    platform_result_t result;

    result = SPI_DRIVER(spi)->transfer_nosetup(spi, config, send_ptr, rcv_ptr, length);

    return result;
}

platform_result_t platform_spi_chip_select_toggle( const platform_spi_t* spi, const platform_spi_config_t* config, wiced_bool_t activate )
{
    platform_result_t result;

    result = SPI_DRIVER(spi)->chip_select_toggle(spi, config, activate);

    return result;
}

platform_result_t platform_spi_core_clock_toggle( wiced_bool_t request )
{
    if ( request == WICED_TRUE )
    {
        gsio_clock_request( );
    }
    else
    {
        gsio_clock_release( );
    }

    return PLATFORM_SUCCESS;
}
