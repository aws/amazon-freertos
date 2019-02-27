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

#include <string.h>
#include <stdint.h>
#include "elf.h"

#include "platform_peripheral.h"
#include "platform_checkpoint.h"
#include "platform_toolchain.h"
#include "platform_dct.h"
#include "platform_config.h"

#include "spi_flash.h"

#include "wiced_low_power.h"
#include "wiced_apps_common.h"
#include "waf_platform.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define link_bss_size   ((unsigned long)&link_bss_end  -  (unsigned long)&link_bss_location )
#define link_dma_size   ((unsigned long)&link_dma_end  -  (unsigned long)&link_dma_location )
#define link_data_size  ((unsigned long)&link_aon_data_end  -  (unsigned long)&link_aon_data_location )

/******************************************************
 *                    Constants
 ******************************************************/

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
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

/* Linker script defined symbols */
extern void* link_bss_location;
extern void* link_bss_end;
extern void* link_dma_location;
extern void* link_dma_end;
extern void* link_aon_data_location;
extern void* link_aon_data_end;
extern void* link_ram_data_location;
extern void* _low_start;

static wiced_deep_sleep_tiny_bootloader_config_t SECTION(".config") config = { .entry_point = (uint32_t)&_low_start };

/******************************************************
 *               Function Declarations
 ******************************************************/

void _start( void )      NORETURN;
void _exit( int status ) NORETURN;

/******************************************************
 *               Function Definitions
 ******************************************************/

static void load_from_sflash( uint32_t app_address, uint32_t offset, void* data_addr, uint32_t size  )
{
#if defined(BOOTLOADER_LOAD_MAIN_APP_FROM_FILESYSTEM)

    static sflash_handle_t sflash_handle;
    static wiced_bool_t    inited = WICED_FALSE;

    if ( !inited )
    {
        init_sflash( &sflash_handle, 0, SFLASH_WRITE_NOT_ALLOWED );
        inited = WICED_TRUE;
    }

    sflash_read( &sflash_handle, app_address + offset, data_addr, size );

#elif defined(BOOTLOADER_LOAD_MAIN_APP_FROM_EXTERNAL_LOCATION)

    image_location_t app_header_location = { .detail.external_fixed.location = app_address };

    wiced_apps_read( &app_header_location, data_addr, offset, size);

#else

#error "Not supported"

#endif
}

static void load_app( uint32_t app_address )
{
    int i;
    elf_header_t header;
    elf_program_header_t prog_header;
    wiced_bool_t xip_segment ;
    WICED_BOOT_CHECKPOINT_WRITE_C( 250 );

    load_from_sflash( app_address, 0, &header, sizeof(header) );

    WICED_BOOT_CHECKPOINT_WRITE_C( 251 );

    for( i = 0; i < header.program_header_entry_count; i++ )
    {
        load_from_sflash( app_address, (header.program_header_offset + header.program_header_entry_size * i), &prog_header, sizeof(prog_header) );

        if ( ( prog_header.data_size_in_file == 0 ) ||     /* size is zero */
             ( ( prog_header.type & 0x1 ) == 0 ) )         /* non- loadable segment */
        {
            continue;
        }

        xip_segment = WICED_IS_XIP_SEGMENT( prog_header.physical_address, prog_header.data_size_in_file );
        if ( !WICED_DEEP_SLEEP_IS_AON_SEGMENT( prog_header.physical_address, prog_header.data_size_in_file ) && !xip_segment )
        {

            load_from_sflash( app_address, prog_header.data_offset, (void*)prog_header.physical_address, prog_header.data_size_in_file );
        }
    }

    WICED_BOOT_CHECKPOINT_WRITE_C( 252 );

    platform_start_app( header.entry );

    WICED_BOOT_CHECKPOINT_WRITE_C( 253 );
}

void _start(void)
{
    /* Initialize BSS */
    memset( &link_bss_location, 0, (size_t) link_bss_size );

    /* Initialize DMA */
    memset( &link_dma_location, 0, (size_t) link_dma_size );

    /* Copy data section from AoN-RAM to RAM */
    memcpy( &link_ram_data_location, &link_aon_data_location, link_data_size );

    WICED_BOOT_CHECKPOINT_WRITE_C( 200 );

    platform_cpu_core_init( );

    WICED_BOOT_CHECKPOINT_WRITE_C( 201 );

    /* Check address, load and run application */
    if ( config.app_address != 0 )
    {
        load_app( config.app_address );
    }

    WICED_BOOT_CHECKPOINT_WRITE_C( 202 );

    /* Nothing can do */
#if !WICED_BOOT_CHECKPOINT_ENABLED
    platform_mcu_reset( ); /* Failed to run, try reboot via cold boot */
#else
    while( 1 ); /* Loop forever */
#endif
}

void _exit( int status )
{
    UNUSED_PARAMETER( status );
    platform_mcu_reset( ); /* Failed to run, try reboot via cold boot */
}
