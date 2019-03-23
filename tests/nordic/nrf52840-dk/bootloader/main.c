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



#include "app_error.h"
#include "app_error_weak.h"
#include "boards.h"
#include "bootloader.h"
#include "crypto.h"
#include "nrf_mbr.h"
#include "nrf_nvmc.h"
#include "nrf_sdm.h"
#include "nrf_bootloader_info.h"
#include "nrf_bootloader.h"
#include "nrf_delay.h"
#include <stdint.h>

#define IRQ_ENABLED              0x01 /**< Field identifying if an interrupt is enabled. */
#define MAX_NUMBER_INTERRUPTS    32   /**< Maximum number of interrupts available. */

ret_code_t nrf_bootloader_flash_protect( uint32_t address,
                                         uint32_t size,
                                         bool read_protect );
void vStartApplication( uint32_t start_addr );
void vSwapImages();
BankState_t xGetBankState( Bank_t xBank );

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

static void vDisableInterrupts( void )
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


/** Boots firmware at specific address */
void vBoot( uint32_t address )
{
    volatile uint32_t xErrCode = NRF_SUCCESS;

    xCryptoUnInit();

    vDisableInterrupts();

    sd_mbr_command_t com =
    {
        SD_MBR_COMMAND_INIT_SD,
    };

    xErrCode = sd_mbr_command( &com );
    APP_ERROR_CHECK( xErrCode );

    xErrCode = sd_softdevice_vector_table_base_set( BOOTLOADER_REGION_START );
    APP_ERROR_CHECK( xErrCode );

    xErrCode = sd_softdevice_vector_table_base_set( address );

    /* Either there was no DFU functionality enabled in this project or the DFU */
    /* module detected no ongoing DFU operation and found a valid main */
    /* application. Boot the main application. */
    /* nrf_bootloader_app_start(); */
    vStartApplication( address );
}

/** Erases memory region
 *  Warning: it can erase more than it was asked for if given an address not aligned with code page
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
void vSetBankFlag( uint8_t usBank,
                   ImageFlags_t xFlag )
{
    ImageDescriptor_t new_desciptor;
    size_t ulCodeRegion = CODE_REGION_1_START;
    ImageDescriptor_t * pxDescriptor = BANK1_DESCRIPTOR;

    if( usBank == SECOND_BANK )
    {
        ulCodeRegion = CODE_REGION_2_START;
        pxDescriptor = BANK2_DESCRIPTOR;
    }

    memcpy( &new_desciptor, pxDescriptor, sizeof( new_desciptor ) );
    new_desciptor.usImageFlags = xFlag;
    vEraseRegion( ( uint8_t * ) ulCodeRegion, DESCRIPTOR_SIZE );
    nrf_nvmc_write_bytes( ulCodeRegion, ( uint8_t * ) &new_desciptor,
                          sizeof( new_desciptor ) );
}

/** Erases the second bank **/
void vEraseSecondBank()
{
    vEraseRegion( ( uint8_t * ) CODE_REGION_2_START, MAX_FIRMWARE_SIZE );
}

/** Commits the image from the second bank **/
void vCommitSecondBank()
{
    BankState_t xBank2State = xGetBankState( SECOND_BANK );

    if( ( xBank2State == BANK_NORDIC ) || ( xBank2State == BANK_NORDIC_DESCRIPTOR ) )
    {
        /* In the case of a nordic firmware in the second bank we don't know its length, so we copy the whole second bank to the first */
        vEraseRegion( ( uint8_t * ) CODE_REGION_1_START, CODE_REGION_2_START - CODE_REGION_1_START );
        /* Copy the firmware */
        nrf_nvmc_write_bytes( CODE_REGION_1_START,
                              ( uint8_t * ) CODE_REGION_2_START,
                              CODE_REGION_2_START - CODE_REGION_1_START );
    }
    else
    {
        vEraseRegion( ( uint8_t * ) CODE_REGION_1_START,
                      DESCRIPTOR_SIZE + BANK2_DESCRIPTOR->ulEndAddress -
                      BANK2_DESCRIPTOR->ulStartAddress );
        /* Copy descriptor */
        nrf_nvmc_write_bytes( CODE_REGION_1_START, ( uint8_t * ) BANK2_DESCRIPTOR,
                              sizeof( ImageDescriptor_t ) );
        /* Copy the firmware */
        nrf_nvmc_write_bytes( CODE_REGION_1_START + DESCRIPTOR_SIZE,
                              ( uint8_t * ) CODE_REGION_2_START + DESCRIPTOR_SIZE,
                              BANK2_DESCRIPTOR->ulEndAddress -
                              BANK2_DESCRIPTOR->ulStartAddress );
    }
}




/**@brief Function for application main entry. */
int main( void )
{
    volatile uint32_t xErrCode = NRF_SUCCESS;

    vBlinkLeds( LED_BOOT );

    /* Make sure that previous swapping completed successfully */
    if( SWAP_STATUS_PAGE[ 0 ] == SWAP_IN_PROGRESS )
    {
        vSwapImages();
    }

    xCryptoInit();
    BankState_t xBank1State = xGetBankState( FIRST_BANK );
    BankState_t xBank2State = xGetBankState( SECOND_BANK );

    switch( xBank1State )
    {
        case BANK_VALID:

            switch( xBank2State )
            {
                case BANK_VALID:

                    if( BANK1_DESCRIPTOR->ulSequenceNumber <= BANK2_DESCRIPTOR->ulSequenceNumber )
                    { /* We shouldn't get this case, so revert the image */
                        vCommitSecondBank();
                        vEraseSecondBank();
                    }
                    else
                    {
                        /* Happens right after OTA completion, we don't need the image in the second bank anymore */
                        vEraseSecondBank();
                    }

                    break;

                case BANK_INVALID:
                case BANK_COMMIT_PENDING:
                case BANK_NORDIC:
                case BANK_NORDIC_DESCRIPTOR:
                    /* Invalid and not accectable cases, erase the second bank */
                    vEraseSecondBank();
                    break;

                case BANK_NEW:

                    if( BANK1_DESCRIPTOR->ulSequenceNumber < BANK2_DESCRIPTOR->ulSequenceNumber )
                    {
                        /* Happens after we receive the OTA update, set the correct flag and continue */
                        vSwapImages();
                        vSetBankFlag( FIRST_BANK, IMAGE_FLAG_COMMIT_PENDING );
                    }
                    else
                    {
                        /* New image is older than old? Erase it */
                        vEraseSecondBank();
                    }

                    break;

                default:
                    break;
            }

            break;

        case BANK_NEW:

            switch( xBank2State )
            {
                case BANK_VALID:

                    if( BANK1_DESCRIPTOR->ulSequenceNumber <= BANK2_DESCRIPTOR->ulSequenceNumber )
                    {
                        /* Something went wrong, revert to the previous image */
                        vCommitSecondBank();
                        vEraseSecondBank();
                    }
                    else
                    {
                        /* Can happen if the swap was done succesfully but power off happened right before setting the flag. Set the flag */
                        vSetBankFlag( FIRST_BANK, IMAGE_FLAG_COMMIT_PENDING );
                    }

                    break;

                default:
                    /* Can happen if the swap was done succesfully but power off happened right before setting the flag. Set the flag */
                    vSetBankFlag( FIRST_BANK, IMAGE_FLAG_COMMIT_PENDING );
                    break;
            }

            break;

        case BANK_COMMIT_PENDING:
            /* That state means that loading the new firmware was unsuccesfull or there was power loss before after setting the flag but before booting.
             * We cannot distinguish these cases but the latter case should not happen often, so it might be simpler to just consider the OTA process failed.
             * Revert image from the second bank if we have it
             */
            switch( xBank2State )
            {
                case BANK_VALID:
                case BANK_NORDIC:
                case BANK_NORDIC_DESCRIPTOR:
                    vCommitSecondBank();
                    vEraseSecondBank();
                    break;

                default:
                    vSetBankFlag( FIRST_BANK, IMAGE_FLAG_INVALID );
                    break;
            }

            break;

        case BANK_NORDIC:
        case BANK_NORDIC_DESCRIPTOR:
            /* Cases for the plain old Nordic image in the first bank. Treat it as a valid image but without checking sequence numbers */
            switch( xBank2State )
            {
                case BANK_VALID:
                    vCommitSecondBank();
                    vEraseSecondBank();
                    break;

                case BANK_NEW:
                    vSwapImages();
                    vSetBankFlag( FIRST_BANK, IMAGE_FLAG_COMMIT_PENDING );
                    break;
                case BANK_NORDIC:
                case BANK_NORDIC_DESCRIPTOR:
                case BANK_COMMIT_PENDING:
                case BANK_INVALID:
                    vEraseSecondBank();
                    break;

                default:
                    break;
            }

            break;

        case BANK_INVALID:
            /* Can happen after failed self-test. Check if we have valid firmware in the second bank and revert to it */
            switch( xBank2State )
            {
                case BANK_VALID:
                case BANK_NORDIC:
                case BANK_NORDIC_DESCRIPTOR:
                    vCommitSecondBank();
                    vEraseSecondBank();
                    break;

                default:
                    break;
            }

            break;
    }

    xBank1State = xGetBankState( FIRST_BANK );

    if( ( xBank1State == BANK_VALID ) || ( xBank1State == BANK_COMMIT_PENDING ) )
    {
        vBoot( CODE_REGION_1_START + DESCRIPTOR_SIZE );
    }
    else if( xBank1State == BANK_NORDIC )
    {
        vBoot( CODE_REGION_1_START );
    }
    else if( xBank1State == BANK_NORDIC_DESCRIPTOR )
    {
        vBoot( CODE_REGION_1_START + DESCRIPTOR_SIZE );
    }
    else
    {
        vBlinkLeds( LED_NO_CORRECT_FIRMWARE ); /* Will blink the LED2 indefinitely */
    }
}

/**
 * @}
 */
