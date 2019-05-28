/**
 * Copyright (c) 2016 - 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
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
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "sdk_config.h"
#include <stdint.h>
#include "nrf.h"
#include "boards.h"
#include "nrf_peripherals.h"
#include "nrf_bootloader_info.h"
#include "nrf_dfu_types.h"
#include "nrf_nvmc.h"
#include "nrf_delay.h"
#include "nrf_assert.h"
#include "nrf_log.h"
#include "sdk_config.h"
#include "bootloader.h"
#include "crypto.h"


/* Enabling the NRF_BOOTLOADER_READ_PROTECT define is untested. */
/* Read-protecting the bootloader requires certain functions to run from RAM. */
/* In GCC and SES this is done automatically when the define is enabled. You will */
/* get warnings which can be ignored. */
/* In Keil you must change project settings to run the entire file from RAM. */
#ifndef NRF_BOOTLOADER_READ_PROTECT
    #define NRF_BOOTLOADER_READ_PROTECT    0
#endif


#define HANDLER_MODE_EXIT             0xFFFFFFF9 /* When this is jumped to, the CPU will exit interrupt context */
                                                 /* (handler mode), and pop values from the stack into registers. */
                                                 /* See ARM's documentation for "Exception entry and return". */
#define EXCEPTION_STACK_WORD_COUNT    8          /* The number of words popped from the stack when */
                                                 /* HANDLER_MODE_EXIT is branched to. */


/**@brief Function that sets the stack pointer and link register, and starts executing a particular address.
 *
 * @param[in]  new_msp  The new value to set in the main stack pointer.
 * @param[in]  new_lr   The new value to set in the link register.
 * @param[in]  addr     The address to execute.
 */
#if defined( __CC_ARM )
    __ASM void StartApplication( uint32_t start_addr )
    {
        LDR R2, [ R0 ];
        Get App MSP.
           MSR MSP, R2;
        Set the main stack pointer to the applications MSP.
           LDR R3, [ R0, # 0x00000004 ];
        Get application reset vector address.
           BX R3;

        No return -stack code is now activated only through SVC and plain interrupts.

                  ALIGN
    }

#else /* if defined( __CC_ARM ) */
    void vStartApplication( uint32_t start_addr )
    {
        __ASM volatile (
            "LDR   R2, [%0]              \n" /*Get App MSP. */
            "MSR   MSP, R2               \n" /*Set the main stack pointer to the applications MSP. */
            "LDR   R3, [%0, #0x00000004] \n" /*Get application reset vector address. */
            "BX    R3                    \n" /*No return - stack code is now activated only through SVC and plain interrupts. */
            ".ALIGN                      \n"
            ::"r" ( start_addr )             /* Argument list for the gcc assembly. start_addr is %0. */
            );
    }
#endif /* if defined( __CC_ARM ) */



#if NRF_BOOTLOADER_READ_PROTECT
    #ifdef __ICCARM__
        __ramfunc
    #elif  defined( __GNUC__ ) || defined( __SES_ARM )
        __attribute__( ( noinline, long_call, section( ".data" ) ) )
    #elif  defined( __CC_ARM )
        #warning "Keil requires changes to project settings to run this file from RAM. Ignore this warning if configuration has been made."
    #endif
#endif
ret_code_t nrf_bootloader_flash_protect( uint32_t address,
                                         uint32_t size,
                                         bool read_protect )
{
    if( ( size & ( CODE_PAGE_SIZE - 1 ) ) || ( address > BOOTLOADER_SETTINGS_ADDRESS ) )
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    #if defined( ACL_PRESENT )
        /* Protect using ACL. */
        static uint32_t acl_instance = 0;

        uint32_t const wmask = ( ACL_ACL_PERM_WRITE_Disable << ACL_ACL_PERM_WRITE_Pos );
        uint32_t const rwmask = wmask | ( ACL_ACL_PERM_READ_Disable << ACL_ACL_PERM_READ_Pos );
        uint32_t const mask = read_protect ? rwmask : wmask;

        do
        {
            if( acl_instance >= ACL_REGIONS_COUNT )
            {
                return NRF_ERROR_NO_MEM;
            }

            NRF_ACL->ACL[ acl_instance ].ADDR = address;
            NRF_ACL->ACL[ acl_instance ].SIZE = size;
            NRF_ACL->ACL[ acl_instance ].PERM = mask;

            acl_instance++;
        } while( NRF_ACL->ACL[ acl_instance - 1 ].ADDR != address ||
                 NRF_ACL->ACL[ acl_instance - 1 ].SIZE != size ||
                 NRF_ACL->ACL[ acl_instance - 1 ].PERM != mask ); /* Check whether the acl_instance has been used before. */
    #elif defined( BPROT_PRESENT )
        /* Protect using BPROT. BPROT does not support read protection. */
        uint32_t pagenum_start = address / CODE_PAGE_SIZE;
        uint32_t pagenum_end = pagenum_start + ( ( size - 1 ) / CODE_PAGE_SIZE );

        for( uint32_t i = pagenum_start; i <= pagenum_end; i++ )
        {
            uint32_t config_index = i / 32;
            uint32_t mask = ( 1 << ( i - config_index * 32 ) );

            switch( config_index )
            {
                case 0:
                    NRF_BPROT->CONFIG0 = mask;
                    break;

                case 1:
                    NRF_BPROT->CONFIG1 = mask;
                    break;

                    #if BPROT_REGIONS_NUM > 64
                        case 2:
                            NRF_BPROT->CONFIG2 = mask;
                            break;

                        case 3:
                            NRF_BPROT->CONFIG3 = mask;
                            break;
                    #endif
            }
        }
    #endif /* if defined( ACL_PRESENT ) */

    return NRF_SUCCESS;
}



#ifdef  __GNUC__
    #pragma GCC push_options
    #pragma GCC optimize ("O0")
#endif
int vBlinkLeds( LedStatus_t xStatus )
{
    nrf_gpio_cfg_output( LED_1 );
    nrf_gpio_cfg_output( LED_2 );
    nrf_gpio_cfg_output( LED_3 );
    nrf_gpio_pin_set( LED_1 );
    nrf_gpio_pin_set( LED_2 );
    switch( xStatus )
    {
        case LED_BOOT:

            for( int i = 0; i < 4; i++ )
            {
                nrf_gpio_pin_toggle( LED_1 );
                nrf_delay_ms( 100 );
            }

            break;

        case LED_NO_CORRECT_FIRMWARE:

            for( ; ; )
            {
                nrf_gpio_pin_toggle( LED_2 );
                nrf_delay_ms( 400 );
            }

            break;
        case LED_SWAP_IN_PROCESS:
            nrf_gpio_pin_toggle( LED_3 );
            break;
    }

    return 0;
}
#ifdef  __GNUC__
    #pragma GCC pop_options
#endif

inline static void vMoveChunk( size_t ulTo,
                               size_t ulFrom,
                               size_t ulChunkPages)
{
    for( uint8_t i = 0; i < ulChunkPages; ++i )
    {
        uint32_t to = ulTo + i * CODE_PAGE_SIZE;
        nrf_nvmc_page_erase( to );
    }

    nrf_nvmc_write_bytes( ulTo, ( uint8_t * ) ulFrom, ulChunkPages * CODE_PAGE_SIZE );
}

inline static void setSwapState( size_t ulCurrentChunk,
                                 SwapState_t xSwapState )
{
    nrf_nvmc_write_byte( ( uint32_t ) SWAP_STATUS_PAGE + 1 + ulCurrentChunk, xSwapState );
}

void vSwapImages()
{
    size_t ulCurrentChunk = 0;
    SwapState_t xSwapState = SWAP_NONE;

    /* Check whether the swaping process was interrupted */
    if( SWAP_STATUS_PAGE[ 0 ] == SWAP_IN_PROGRESS )
    {
        /* Search for the first unswapped page */
        while( ulCurrentChunk * SWAP_AREA_SIZE < MAX_FIRMWARE_PAGES &&
               SWAP_STATUS_PAGE[ ulCurrentChunk + 1 ] == 0xF8 )
        {
            ++ulCurrentChunk;
        }
    }

    nrf_nvmc_write_byte( (uint32_t) SWAP_STATUS_PAGE, SWAP_IN_PROGRESS );

    /* Do the swapping, make sure that all flags are updated after actual moving is performed */
    while( xSwapState != SWAP_COMPLETED )
    {
        size_t ulCurrentChunkSize = MIN(SWAP_AREA_SIZE, MAX_FIRMWARE_PAGES - ulCurrentChunk * SWAP_AREA_SIZE);
        switch( SWAP_STATUS_PAGE[ ulCurrentChunk + 1 ] )
        {
            case SWAP_NONE:
                vMoveChunk( ( uint32_t ) SWAP_AREA_BEGIN, 
                            CODE_REGION_1_START + ulCurrentChunk * CHUNK_SIZE_BYTES, ulCurrentChunkSize );
                setSwapState( ulCurrentChunk, SWAP_FIRST_TO_TEMP );
                vBlinkLeds(LED_SWAP_IN_PROCESS);
                break;

            case SWAP_FIRST_TO_TEMP:
                vMoveChunk( CODE_REGION_1_START + ulCurrentChunk * CHUNK_SIZE_BYTES, 
                            CODE_REGION_2_START + ulCurrentChunk * CHUNK_SIZE_BYTES, ulCurrentChunkSize );
                setSwapState( ulCurrentChunk, SWAP_SECOND_TO_FIRST );
                vBlinkLeds(LED_SWAP_IN_PROCESS);
                break;

            case SWAP_SECOND_TO_FIRST:
                vMoveChunk( CODE_REGION_2_START + ulCurrentChunk * CHUNK_SIZE_BYTES, 
                            ( uint32_t ) SWAP_AREA_BEGIN, ulCurrentChunkSize );
                setSwapState( ulCurrentChunk, SWAP_TEMP_TO_SECOND );
                ulCurrentChunk += 1;

                if( ulCurrentChunk * SWAP_AREA_SIZE >= MAX_FIRMWARE_PAGES )
                {
                    xSwapState = SWAP_COMPLETED;
                }
                vBlinkLeds(LED_SWAP_IN_PROCESS);
                break;

            default:
                break;
        }
          
    }
    nrf_gpio_pin_set( LED_3 );
    /* Erase the status page so it would be ready for future swaps */
    nrf_nvmc_page_erase( ( uint32_t ) SWAP_STATUS_PAGE );
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


BankState_t xGetBankState( Bank_t xBank )
{
    long ulCodeStart = xBank == FIRST_BANK ? CODE_REGION_1_START : CODE_REGION_2_START;
    bool bBankAFRHeader =
        ( bValidateHeader( ( uint8_t * ) ulCodeStart ) == NRF_SUCCESS );

    if( bBankAFRHeader )
    {
        if( xVerifyImageSignature( (uint8_t*)ulCodeStart ) != NRF_SUCCESS )
        {
            return BANK_INVALID;
        }

        switch( ( ( ImageDescriptor_t * ) ( ulCodeStart ) )->usImageFlags )
        {
            case IMAGE_FLAG_VALID:
                return BANK_VALID;

            case IMAGE_FLAG_INVALID:
                return BANK_INVALID;

            case IMAGE_FLAG_COMMIT_PENDING:
                return BANK_COMMIT_PENDING;

            case IMAGE_FLAG_NEW:
                return BANK_NEW;

            default:
                return BANK_INVALID;
        }
    }
    else
    {
        if( *( size_t * ) ( ulCodeStart + DESCRIPTOR_SIZE ) == STACK_BEGIN )
        {
            return BANK_NORDIC_DESCRIPTOR;
        }
        else if( *( size_t * ) ( ulCodeStart ) == STACK_BEGIN )
        {
            return BANK_NORDIC;
        }
    }

    return BANK_NONE;
}