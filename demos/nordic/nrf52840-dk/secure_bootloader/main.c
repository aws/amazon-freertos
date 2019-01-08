/**
 * Copyright (c) 2016 - 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be
 * reverse engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** @file
 *
 * @defgroup bootloader_secure_ble main.c
 * @{
 * @ingroup dfu_bootloader_api
 * @brief Bootloader project main file for secure DFU.
 *
 */

#include "nrf_mbr.h"
#include "nrf_nvmc.h"
#include "nrf_sdm.h"

#include "app_error.h"
#include "app_error_weak.h"
#include "boards.h"
#include "bootloader.h"
#include "crypto.h"
#include "nrf_bootloader.h"
#include "nrf_bootloader_info.h"
#include "nrf_delay.h"
#include <stdint.h>

#define IRQ_ENABLED              0x01 /**< Field identifying if an interrupt is enabled. */
#define MAX_NUMBER_INTERRUPTS    32   /**< Maximum number of interrupts available. \
                                       */

ret_code_t nrf_bootloader_flash_protect( uint32_t address,
                                         uint32_t size,
                                         bool read_protect );
void StartApplication( uint32_t start_addr );

static void on_error( void )
{
    #if NRF_MODULE_ENABLED( NRF_LOG_BACKEND_RTT )
        /* To allow the buffer to be flushed by the host. */
        nrf_delay_ms( 100 );
    #endif
    #ifdef NRF_DFU_DEBUG_VERSION
        NRF_BREAKPOINT_COND;
    #endif
    NVIC_SystemReset();
}

static void interrupts_disable( void )
{
    uint32_t interrupt_setting_mask;
    uint8_t irq;

    /* We start the loop from first interrupt, i.e. interrupt 0. */
    irq = 0;
    /* Fetch the current interrupt settings. */
    interrupt_setting_mask = NVIC->ISER[ 0 ];

    for( ; irq < MAX_NUMBER_INTERRUPTS; irq++ )
    {
        if( interrupt_setting_mask & ( IRQ_ENABLED << irq ) )
        {
            /* The interrupt was enabled, and hence disable it. */
            NVIC_DisableIRQ( ( IRQn_Type ) irq );
        }
    }
}

void app_error_handler( uint32_t error_code,
                        uint32_t line_num,
                        const uint8_t * p_file_name )
{
    on_error();
}

void app_error_fault_handler( uint32_t id,
                              uint32_t pc,
                              uint32_t info )
{
    on_error();
}

void app_error_handler_bare( uint32_t error_code )
{
    on_error();
}

/** Check if the magick number of image located at the given is correct */
bool bMagickCorrect( uint8_t * address )
{
    if( memcmp( address, AFR_MAGICK, MAGICK_SIZE ) == 0 )
    {
        return true;
    }

    return false;
}

/** Checks if the header of image located at the given is correct */
ret_code_t bValidateHeader( uint8_t * address )
{
    ImageDescriptor_t * descriptor = ( ImageDescriptor_t * ) address;

    if( !bMagickCorrect( address ) )
    {
        return NRF_ERROR_INVALID_DATA;
    }

    if( descriptor->ulHardwareID != HARDWARE_ID )
    {
        return NRF_ERROR_INVALID_DATA;
    }

    return NRF_SUCCESS;
}

/** Boots firmware at specific address */
void vBoot( uint8_t * address )
{
    volatile uint32_t xErrCode = 0;

    xCryptoUnInit();

    interrupts_disable();

    sd_mbr_command_t com =
    {
        SD_MBR_COMMAND_INIT_SD,
    };

    xErrCode = sd_mbr_command( &com );
    APP_ERROR_CHECK( xErrCode );

    xErrCode = sd_softdevice_vector_table_base_set( BOOTLOADER_REGION_START );
    APP_ERROR_CHECK( xErrCode );

    xErrCode = sd_softdevice_vector_table_base_set( ( uint32_t ) address );

    /* Either there was no DFU functionality enabled in this project or the DFU */
    /* module detected no ongoing DFU operation and found a valid main */
    /* application. Boot the main application. */
    /* nrf_bootloader_app_start(); */
    StartApplication( ( uint32_t ) address );
}

/** Erases memory region
    Warning: it can erase more than it was asked for if given an address not aligned with code page
*/
void vEraseRegion( uint8_t * address,
                   size_t length )
{
    uint8_t * pusCurrentPageAddress = address - ( uint32_t ) address % CODE_PAGE_SIZE;

    while( pusCurrentPageAddress < address + length )
    {
        nrf_nvmc_page_erase( ( uint32_t ) pusCurrentPageAddress );
        pusCurrentPageAddress += CODE_PAGE_SIZE;
    }
}

/** Writes the image flag for the second bank */
void vSetSecondBankFlag( ImageFlags_t xFlag )
{
    ImageDescriptor_t new_desciptor;

    memcpy( &new_desciptor, BANK2_DESCRIPTOR, sizeof( new_desciptor ) );
    new_desciptor.usImageFlags = xFlag;
    vEraseRegion( ( uint8_t * ) CODE_REGION_2_START, DESCRIPTOR_SIZE );
    nrf_nvmc_write_bytes( CODE_REGION_2_START, ( uint8_t * ) &new_desciptor,
                          sizeof( new_desciptor ) );
}

/** Erases the descriptor of the second bank
 *  We do not erase the entire bank to save flash resource
 **/
void vEraseSecondBank()
{
    vEraseRegion( ( uint8_t * ) CODE_REGION_2_START, DESCRIPTOR_SIZE );
}

/** Commits the image from the second bank **/
void vCommitSecondBank()
{
    vEraseRegion( ( uint8_t * ) CODE_REGION_1_START,
                  DESCRIPTOR_SIZE + BANK2_DESCRIPTOR->ulEndAddress -
                  BANK2_DESCRIPTOR->ulStartAddress );
    ImageDescriptor_t new_desciptor;
    /* We move image so we need to update addresses */
    memcpy( &new_desciptor, BANK2_DESCRIPTOR, sizeof( new_desciptor ) );
    new_desciptor.ulStartAddress = BANK2_DESCRIPTOR->ulStartAddress -
                                   ( CODE_REGION_2_START - CODE_REGION_1_START );
    new_desciptor.ulEndAddress = BANK2_DESCRIPTOR->ulEndAddress -
                                 ( CODE_REGION_2_START - CODE_REGION_1_START );
    new_desciptor.ulExecutionAddress =
        BANK2_DESCRIPTOR->ulExecutionAddress -
        ( CODE_REGION_2_START - CODE_REGION_1_START );
    nrf_nvmc_write_bytes( CODE_REGION_1_START, ( uint8_t * ) &new_desciptor,
                          sizeof( new_desciptor ) );
    /* Copy the firmware */
    nrf_nvmc_write_bytes( CODE_REGION_1_START + DESCRIPTOR_SIZE,
                          ( uint8_t * ) CODE_REGION_2_START + DESCRIPTOR_SIZE,
                          BANK2_DESCRIPTOR->ulEndAddress -
                          BANK2_DESCRIPTOR->ulStartAddress );
}

/**@brief Function for application main entry. */
int main( void )
{
    volatile uint32_t xErrCode = 0;

    /* Protect MBR and bootloader code from being overwritten. */
    xErrCode = nrf_bootloader_flash_protect( 0, MBR_SIZE, false );
    APP_ERROR_CHECK( xErrCode );
    xErrCode = nrf_bootloader_flash_protect( BOOTLOADER_START_ADDR,
                                             BOOTLOADER_SIZE, false );
    APP_ERROR_CHECK( xErrCode );

    /*    uint32_t err_code = sd_softdevice_disable(); */
    /*    APP_ERROR_CHECK(err_code); */

    bool bBank1AFRHeader =
        ( bValidateHeader( ( uint8_t * ) CODE_REGION_1_START ) == NRF_SUCCESS );

    xCryptoInit();
    xErrCode = bValidateHeader( ( uint8_t * ) CODE_REGION_2_START );

    if( xErrCode == NRF_SUCCESS ) /* We have a firmware in the second bank */
    {
        if( bBank1AFRHeader && ( BANK1_DESCRIPTOR->ulSequenceNumber >=
                                 BANK2_DESCRIPTOR->ulSequenceNumber ) )
        {
            /** The firmware in the second bank has lower number than the one in the
             * first bank. It is incorrect and the second bank must be cleaned */
            vEraseSecondBank();
        }
        else
        {
            xErrCode = xVerifyImageSignature( ( uint8_t * ) CODE_REGION_2_START );

            if( xErrCode != NRF_SUCCESS ) /* The firmware in the second bank has an
                                           * incorrect signature, erase its header */
            {
                vEraseRegion( ( uint8_t * ) CODE_REGION_2_START, DESCRIPTOR_SIZE );
            }
            else
            {
                switch( BANK2_DESCRIPTOR->usImageFlags )
                {
                    case IMAGE_FLAG_NEW:
                        vSetSecondBankFlag( IMAGE_FLAG_COMMIT_PENDING );
                        vBoot( ( uint8_t * ) BANK2_DESCRIPTOR->ulExecutionAddress );
                        break;

                    case IMAGE_FLAG_VALID:
                        vCommitSecondBank();
                        xErrCode = xVerifyImageSignature( ( uint8_t * ) CODE_REGION_1_START );

                        if( xErrCode == NRF_SUCCESS )
                        {
                            vEraseSecondBank();
                            vBoot( ( uint8_t * ) BANK1_DESCRIPTOR->ulExecutionAddress );
                        }
                        else /* Something went wrong, try again */
                        {
                            NVIC_SystemReset();
                        }

                        break;

                    case IMAGE_FLAG_INVALID:
                        vEraseSecondBank();
                        break;

                    case IMAGE_FLAG_COMMIT_PENDING:

                        /* We should never get here, so something went wrong. Off with the
                         * header! */
                        vEraseSecondBank();
                        break;
                }
            }
        }
    }

    /* If we've got here, then the second bank is empty or invalid.
     *  Now we need to check if the first bank contains a classic firmware or AFR
     * image */
    xErrCode = bValidateHeader( ( uint8_t * ) CODE_REGION_1_START );

    if( xErrCode == NRF_SUCCESS ) /* We have a firmware in the second bank */
    {
        xErrCode = xVerifyImageSignature( ( uint8_t * ) CODE_REGION_1_START );

        if( xErrCode != NRF_SUCCESS ) /* The first image is corrupted we have no
                                       * choice but reboot */
        {
            NVIC_SystemReset();
        }
        else
        {
            vBoot( ( uint8_t * ) BANK1_DESCRIPTOR->ulExecutionAddress );
        }
    }
    else
    {
        /* We are at the last resort, i.e. we have a classic Nordic firmware, so we
         * just boot it */
        vBoot( ( uint8_t * ) CODE_REGION_1_START );
    }
}

/**
 * @}
 */
