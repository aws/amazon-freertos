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
 * BCM43909 Platform SDIO Host Driver Interface
 */

#include <stdint.h>
#include "typedefs.h"

#include "osl.h"

#include "sdio.h"       /* SDIO Device and Protocol Specs */
#include "sdioh.h"      /* Standard SDIO Host Controller Specification */
#include "bcmsdbus.h"   /* bcmsdh to/from specific controller APIs */
#include "bcmsdstd.h"
#include "platform_sdio.h"

#include "platform_appscr4.h"
#include "wwd_rtos_isr.h"
#include "wiced_power_logger.h"

/******************************************************
 *             Constants
 ******************************************************/
#define SD_BLOCK_SZ   (512)
#define SUPPORT_SDMMC_READ_MULTI_BLOCKS
#define SUPPORT_SDMMC_WRITE_MULTI_BLOCKS

/******************************************************
 *             Structures
 ******************************************************/

/******************************************************
 *             Variables
 ******************************************************/

/******************************************************
 *             Static Function Declarations
 ******************************************************/

/******************************************************
 *             Function definitions
 ******************************************************/

wiced_result_t platform_sdio_host_init( sdio_isr_callback_function_t client_check_isr_function )
{
    glob_sd = sdioh_attach( NULL, client_check_isr_function, 0 );

    if ( glob_sd == NULL )
    {
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

wiced_result_t platform_sdio_host_deinit( void )
{
    sdioh_detach( NULL, glob_sd );

    return WICED_SUCCESS;
}

wiced_result_t platform_sdio_host_transfer( sdio_transfer_direction_t direction, sdio_command_t command, sdio_transfer_mode_t mode, sdio_block_size_t block_size, uint32_t argument, /*@null@*/ uint32_t* data, uint16_t data_size, sdio_response_needed_t response_expected, /*@out@*/ /*@null@*/ uint32_t* response )
{
    SDIOH_API_RC result;
    sdio_cmd_argument_t arg = { .value = argument };

    UNUSED_PARAMETER( response_expected );
    UNUSED_PARAMETER( block_size );

    if ( command == SDIO_CMD_53 )
    {
        uint rw = direction == SDIO_READ ? SDIOH_READ : SDIOH_WRITE;
        uint pio_dma = mode == SDIO_BLOCK_MODE ? SDIOH_DATA_DMA : SDIOH_DATA_PIO;
        glob_sd->sd_blockmode = (bool)pio_dma;
        if ( data_size == 2 || data_size == 4 )
        {
            uint32 data_word = direction == SDIO_WRITE ? *data : 0;
            result = sdioh_request_word(glob_sd, SDIOH_CMD_TYPE_NORMAL, rw, arg.cmd53.function_number, arg.cmd53.register_address, &data_word, data_size);
            if ( direction == SDIO_READ )
            {
                if ( response != NULL )
                {
                    *response = data_word;
                }
                if ( data != NULL )
                {
                    *data = data_word;
                }
            }
        }
        else
        {
            /* XXX: Adjust count if an odd number */
            if ( data_size & 1 )
            {
                data_size++;
            }

            /* Use DMA if block mode */
            if ( data_size >= block_size )
            {
                mode = SDIO_BLOCK_MODE;
                pio_dma = SDIOH_DATA_DMA;
                glob_sd->sd_blockmode = (bool)pio_dma;
            }

            /* Roudup to block size alignment */
            if ( mode == SDIO_BLOCK_MODE && (data_size % block_size) != 0 )
            {
                data_size = (uint16_t)(arg.cmd53.count * block_size);
            }
            result = sdioh_request_buffer(glob_sd, pio_dma, arg.cmd53.op_code, rw, arg.cmd53.function_number, arg.cmd53.register_address, 4, data_size, (uint8 *) data, NULL);
        }
    }
    else if ( command == SDIO_CMD_52 )
    {
        uint8 data_byte;
        if ( direction == SDIO_WRITE )
        {
            data_byte = arg.cmd52.write_data;
            result = sdioh_cfg_write(glob_sd, arg.cmd52.function_number, arg.cmd52.register_address, &data_byte);
        }
        else
        {
            result = sdioh_cfg_read(glob_sd, arg.cmd52.function_number, arg.cmd52.register_address, &data_byte);
            *response = data_byte;
        }
    }
    else
    {
        return WICED_ERROR;
    }

    return (SDIOH_API_SUCCESS(result) ? WICED_SUCCESS : WICED_ERROR);
}

void platform_sdio_host_enable_high_speed( void )
{
    uint32_t value;
    uint8_t  baseclk;

    value = 1;

    sdioh_iovar_op(glob_sd, "sd_highspeed", NULL, 0, (void *)&value, sizeof(value), TRUE);

    /* Get base clock in MHz */
    baseclk = GFIELD(glob_sd->caps, CAP_BASECLK);
    /* Calculate divisor number */
    value = DIV_ROUND_UP(baseclk, 50);

    sdioh_iovar_op(glob_sd, "sd_divisor", NULL, 0, (void *)&value, sizeof(value), TRUE);

}

wiced_result_t platform_sdio_host_enable_interrupt( void )
{
    SDIOH_API_RC status;

    /* let's route this bus line to SDIO_REMAPPED_ExtIRQn bit. */
    platform_irq_remap_sink( OOB_AOUT_SDIO_HOST_INTR, SDIO_REMAPPED_ExtIRQn );

    /* enable this line. */
    platform_irq_enable_irq( SDIO_REMAPPED_ExtIRQn );

    /* Enable interrupt */
    status = sdioh_interrupt_set(glob_sd, TRUE);

    return (SDIOH_API_SUCCESS(status) ? WICED_SUCCESS : WICED_ERROR);
}

wiced_result_t platform_sdio_host_disable_interrupt( void )
{
    /* Disable interrupt */
    sdioh_interrupt_set(glob_sd, FALSE);
    platform_irq_disable_irq( SDIO_REMAPPED_ExtIRQn );

    return  WICED_SUCCESS;
}

wiced_result_t platform_sdio_host_read_block( uint32_t address, uint32_t count, uint8_t* data )
{
    SDIOH_API_RC result = SDIOH_API_RC_FAIL;

    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SDIO, EVENT_DESC_SPI_SFLASH_READ );

    /* address translation for SDSC */
    if ( glob_sd->csd_sdsc == 1 )
    {
        address *= SD_BLOCK_SZ;
    }

    if ( count == 1 )
    {
        result = sdioh_sdmmc_read_block( glob_sd, address, data );
    }
    else if ( count > 1 )
    {
#ifdef SUPPORT_SDMMC_READ_MULTI_BLOCKS
        result = sdioh_sdmmc_read_blocks( glob_sd, address, count, data );
#else
        int i;
        uint32_t addr = address;
        uint8_t* ptr = data;
        for ( i = 0; i < count; i++)
        {
            result = sdioh_sdmmc_read_block( glob_sd, addr, ptr );
            if ( result != SDIOH_API_RC_SUCCESS )
            {
                WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SDIO, EVENT_DESC_SPI_SFLASH_IDLE );
                return WICED_ERROR;
            }
            addr += SD_BLOCK_SZ;
            ptr += SD_BLOCK_SZ;
        }
#endif
    }

    if ( result != SDIOH_API_RC_SUCCESS )
    {
        WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SDIO, EVENT_DESC_SPI_SFLASH_IDLE );
        return WICED_ERROR;
    }

    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SDIO, EVENT_DESC_SPI_SFLASH_IDLE );
    return WICED_SUCCESS;
}

wiced_result_t platform_sdio_host_write_block( uint32_t address, uint32_t count, const uint8_t* data )
{
    SDIOH_API_RC result = SDIOH_API_RC_FAIL;

    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SDIO, EVENT_DESC_SPI_SFLASH_WRITE );

    /* address translation for SDSC */
    if ( glob_sd->csd_sdsc == 1 )
    {
        address *= SD_BLOCK_SZ;
    }

    if ( count == 1 )
    {
        result = sdioh_sdmmc_write_block( glob_sd, address, data );
    }
    else if ( count > 1 )
    {
#ifdef SUPPORT_SDMMC_WRITE_MULTI_BLOCKS
        result = sdioh_sdmmc_write_blocks( glob_sd, address, count, data );
#else
        int i;
        uint32_t addr = address;
        uint8_t* ptr = ( uint8_t * ) data;
        for ( i = 0; i < count; i++)
        {
            result = sdioh_sdmmc_write_block( glob_sd, addr, ptr );
            if ( result != SDIOH_API_RC_SUCCESS )
            {
                WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SDIO, EVENT_DESC_SPI_SFLASH_IDLE );
                return WICED_ERROR;
            }
            addr += SD_BLOCK_SZ;
            ptr += SD_BLOCK_SZ;
        }
#endif
    }

    if ( result != SDIOH_API_RC_SUCCESS )
    {
        WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SDIO, EVENT_DESC_SPI_SFLASH_IDLE );
        return WICED_ERROR;
    }

    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SDIO, EVENT_DESC_SPI_SFLASH_IDLE );
    return WICED_SUCCESS;
}

/******************************************************
 *             IRQ Handler Definitions
 ******************************************************/
WWD_RTOS_DEFINE_ISR( platform_sdio_host_isr )
{
    sdioh_info_t *sd;

    sd = glob_sd;
    sd->local_intrcount++;

    if (!sd->card_init_done) {
        sd_err(("%s: Hey Bogus intr...not even initted: \n", __FUNCTION__));
    } else {
        check_client_intr(sd);
    }
}
/******************************************************
 *             IRQ Handler Mapping
 ******************************************************/
WWD_RTOS_MAP_ISR( platform_sdio_host_isr, SDIO_HOST_ISR )

