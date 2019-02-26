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
 *
 */
#include <stdint.h>
#include <string.h>
#include "typedefs.h"
#include "platform_peripheral.h"
#include "platform_isr.h"
#include "platform_isr_interface.h"
#include "wwd_rtos.h"
#include "wwd_rtos_isr.h"

#include "wiced_osl.h"
#include "wiced_platform.h"
#include "platform_peripheral.h"
#include "platform_mcu_peripheral.h"
#include "platform_map.h"
#include "platform_pinmux.h"
#include "platform_gspi_slave.h"

/******************************************************
 *                      Macros
 ******************************************************/
/******************************************************
 *                    Constants
 ******************************************************/

#define GSPI_SLAVE_REG_ADR              PLATFORM_SDIOD_REGBASE(0)
#define GSPI_TOSBMAIL_INT_MASK          0xf
#define GSPI_TOHOST_INT                 0x1
#define GSPI_TOHOST_SLAVE_READY         0x2
#define GSPI_SLAVE_INT_MASK             GSPI_TOSBMAIL_INT_MASK

/******************************************************
 *             Variables
 ******************************************************/

static volatile gspi_slave_reg_t* gspi_slave_reg     = (gspi_slave_reg_t *)GSPI_SLAVE_REG_ADR;
/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 *
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
typedef struct shared_info
{
        uint32_t magic_number;
        uint32_t tx_buffer_address;
        uint32_t tx_buffer_size;
        uint32_t rx_buffer_address;
        uint32_t rx_buffer_size;
} shared_info_t;

/******************************************************
 *               Function Declarations
 ******************************************************/
static void gspi_core_enable( void );
static platform_result_t platform_gspi_slave_enable_interrupt( void );

/******************************************************
 *               Variables Definitions
 ******************************************************/
static uint32_t gspi_inited = 0;
static uint32_t gspi_int_count = 0;
static platform_gspi_slave_driver_t *driver;
/******************************************************
 *               Function Definitions
 ******************************************************/
static void gspi_core_enable( void )
{
    osl_wrapper_enable( (void*)PLATFORM_SDIOD_MASTER_WRAPPER_REGBASE(0x0) );
}

platform_result_t platform_gspi_slave_init( platform_gspi_slave_driver_t *slave_driver )
{
    platform_result_t result;

    if ( gspi_inited == 0 )
    {
        gspi_core_enable( );

        driver = slave_driver;
        host_rtos_init_semaphore( &slave_driver->transfer_complete );

        /* Enable ToSBMailEn(3:0) */
        gspi_slave_reg->intmask = GSPI_SLAVE_INT_MASK;
        result = platform_gspi_slave_enable_interrupt( );

        if ( result != PLATFORM_SUCCESS )
        {
            printf("[%s] Enable interrupt failed\n", __FUNCTION__ );
            return PLATFORM_ERROR;
        }

        gspi_inited = 1;
    }

    return PLATFORM_SUCCESS;
}

void platform_gspi_slave_indicate_slave_ready( void )
{
    gspi_slave_reg->tohostmailbox = GSPI_TOHOST_SLAVE_READY;
}

void platform_gspi_set_shared_address( uint32_t address )
{
    gspi_slave_reg->tohostmailboxdata = address;
}


void platform_gspi_slave_notify_master ( void )
{
    gspi_slave_reg->tohostmailbox = GSPI_TOHOST_INT;
}


platform_result_t platform_gspi_slave_receive_command( platform_gspi_slave_driver_t* slave_driver, uint32_t timeout_ms )
{
    host_rtos_get_semaphore( &slave_driver->transfer_complete, timeout_ms, WICED_TRUE );

    return PLATFORM_SUCCESS;
}

void platform_gspi_transfer_data(uint8_t* desc_address, uint8_t* source_address, uint32_t data_length )
{
    memcpy( desc_address, source_address, data_length);
}

static platform_result_t platform_gspi_slave_enable_interrupt( void )
{

    platform_irq_remap_sink( OOB_AOUT_GSPI_SLAVE_INTR, SDIO_REMAPPED_ExtIRQn );

    /* enable this line. */
    platform_irq_enable_irq( SDIO_REMAPPED_ExtIRQn );

    return PLATFORM_SUCCESS;

}

WWD_RTOS_DEFINE_ISR( platform_gspi_isr )
{
    uint32_t interrupt_status;
    interrupt_status = gspi_slave_reg->intstatus;
    interrupt_status &= GSPI_SLAVE_INT_MASK;

    /* Ack interrupt */
    gspi_slave_reg->intstatus = interrupt_status;
    gspi_int_count++;
    host_rtos_set_semaphore( &driver->transfer_complete, WICED_TRUE );
}

WWD_RTOS_MAP_ISR( platform_gspi_isr, GSPI_SLAVE_ISR )
