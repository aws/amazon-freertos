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

/**
 * @file
 *
 * SPI Flash Write Application
 *
 * This application is used by OpenOCD and the WICED build
 * system to write to serial flash. The app is loaded into
 * MCU RAM directly and once running, interacts with OpenOCD
 * to write consecutive chunks of data received via JTAG to
 * serial flash.
 *
 * The linker script places two of the structures
 * defined by the app at the beginning of RAM. These structure
 * variables contain all the details of the commands
 * to be executed and data to be written
 *
 */

#include <stdio.h>
#include <string.h>
#include "spi_flash.h"
#include "wwd_assert.h"
#include "wiced_utilities.h"
#include "platform_config.h"
#include "platform_init.h"
#include "platform_peripheral.h"
#include "platform_sflash_dct.h"
#include "platform_toolchain.h"
#include "linker_symbols.h"

/*
 * Test mode defines
 *
 * #define DEBUG_PRINT
 * #define WIPE_SFLASH
 * #define TEST_SFLASH_WRITE
 * #define TEST_SFLASH_READ
 * #define DEBUG_PRINT_READ_CONTENT
 * #define DEBUG_PRINT_VERIFY_READ_CONTENT
 */

/******************************************************
 *                      Macros
 ******************************************************/
#ifdef DEBUG_PRINT
#define DEBUG_PRINTF(x) printf x
#else
#define DEBUG_PRINTF(x)
#endif /* ifdef DEBUG_PRINT */

/******************************************************
 *                    Constants
 ******************************************************/
/*
 * Commands to execute - bitwise OR together
 * TCL script write_sflash.tcl must match these defines
 */
#define MFG_SPI_FLASH_COMMAND_NONE                      (0x00000000)

#define MFG_SPI_FLASH_COMMAND_INITIAL_VERIFY            (0x00000001)
#define MFG_SPI_FLASH_COMMAND_ERASE_CHIP                (0x00000002)
#define MFG_SPI_FLASH_COMMAND_WRITE                     (0x00000004)
#define MFG_SPI_FLASH_COMMAND_POST_WRITE_VERIFY         (0x00000008)
#define MFG_SPI_FLASH_COMMAND_VERIFY_CHIP_ERASURE       (0x00000010)
#define MFG_SPI_FLASH_COMMAND_READ                      (0x00000040)
#define MFG_SPI_FLASH_COMMAND_WRITE_ERASE_IF_NEEDED     (0x00000080)


#define WRITE_CHUNK_SIZE        (8*1024)  /* Writing in chunks is only needed to prevent reset by watchdog */
#ifndef SECTOR_SIZE
#define SECTOR_SIZE             (4096)
#endif
/******************************************************
 *                   Enumerations
 ******************************************************/
/*
 * Result codes
 * TCL script write_sflash.tcl must match this enum
 */
typedef enum
{
    MFG_SPI_FLASH_RESULT_IN_PROGRESS               = 0xffffffff,
    MFG_SPI_FLASH_RESULT_OK                        = 0,
    MFG_SPI_FLASH_RESULT_ERASE_FAILED              = 1,
    MFG_SPI_FLASH_RESULT_VERIFY_AFTER_WRITE_FAILED = 2,
    MFG_SPI_FLASH_RESULT_SIZE_TOO_BIG_BUFFER       = 3,
    MFG_SPI_FLASH_RESULT_SIZE_TOO_BIG_CHIP         = 4,
    MFG_SPI_FLASH_RESULT_DCT_LOC_NOT_FOUND         = 5,
    MFG_SPI_FLASH_RESULT_WRITE_FAILED              = 6,
    MFG_SPI_FLASH_RESULT_READ_FAILED               = 7,
    MFG_SPI_FLASH_RESULT_END                       = 0x7fffffff /* force to 32 bits */
} mfg_spi_flash_result_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
/*
 * TCL script write_sflash.tcl must match this structure
 */
typedef struct
{
    void *        entry_point;
    void *        stack_addr;
    unsigned long data_buffer_size;
} data_config_area_t;

/*
 * TCL script write_sflash.tcl must match this structure
 */

typedef struct
{
    unsigned long          size;
    unsigned long          sflash_address;
    unsigned long          command;
    mfg_spi_flash_result_t result;
    unsigned char          data[__JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__];
} data_transfer_area_t;




/******************************************************
 *               Static Function Declarations
 ******************************************************/
#ifdef WIPE_SFLASH
    static void add_wipe_data( void );
#elif defined( TEST_SFLASH_WRITE )
    static void add_test_data( void );
#elif defined( TEST_SFLASH_READ )
    static void read_test_data2( void );
#endif /* ifdef TEST_SFLASH_READ */

/******************************************************
 *               Variable Definitions
 ******************************************************/
static uint8_t   Rx_Buffer[SECTOR_SIZE +10];   /* A temporary buffer used for reading data from the Serial flash when performing verification */

/******************************************************************************
 * This structure provides configuration parameters, and communication area
 * to the TCL OpenOCD script
 * It will be located at the start of RAM by the linker script
 *****************************************************************************/
#if defined(__ICCARM__)
/* IAR specific */
#pragma section= "data_config_section"
const data_config_area_t data_config @ "data_config_section";
#endif /* #if defined(__ICCARM__) */
const data_config_area_t   data_config =
{
        .entry_point = (void*) ENTRY_ADDRESS,
        .stack_addr  = &link_stack_end,
        .data_buffer_size = __JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__,
};

/******************************************************************************
 * This structure provides a transfer area for communications with the
 * TCL OpenOCD script
 * It will be located immediately after the data_config structure at the
 * start of RAM by the linker script
 *****************************************************************************/
#if defined (__ICCARM__)
/* IAR specific */
#pragma section= "data_transfer_section"
data_transfer_area_t data_transfer @ "data_transfer_section";
#else /* #if defined (__ICCARM__) */
data_transfer_area_t data_transfer;
#endif /* #if defined (__ICCARM__) */


/******************************************************
 *               Function Definitions
 ******************************************************/

int main( void )
{
    unsigned long pos;
    sflash_handle_t sflash_handle;
    unsigned long chip_size;
#if defined ( __IAR_SYSTEMS_ICC__ )
/* IAR allows init functions in __low_level_init(), but it is run before global
 * variables have been initialised, so the following init still needs to be done
 * When using GCC, this is done in crt0_GCC.c
 */
    platform_init_mcu_infrastructure( );
    platform_init_external_devices( );
#endif /* #elif defined ( __IAR_SYSTEMS_ICC__ ) */

    NoOS_setup_timing( );

#ifdef WIPE_SFLASH
    add_wipe_data( );
#elif defined( TEST_SFLASH_WRITE )
    add_test_data( );
#elif defined( TEST_SFLASH_READ )
    read_test_data2( );
#endif /* ifdef TEST_SFLASH_READ */

#ifdef PLATFORM_HAS_OTP
    platform_otp_setup( );
#endif /* ifdef PLATFORM_HAS_OTP */

    /* loop forever */
    while ( 1 == 1 )
    {
        DEBUG_PRINTF(( "Waiting for command\n" ));

        /* wait for a command to be written. */
        /*@-infloopsuncon@*/ /* Lint: Loop variable is modified by JTAG writing directly into the data transfer memory */
        while ( data_transfer.command == MFG_SPI_FLASH_COMMAND_NONE )
        {
            (void)platform_watchdog_kick( );
        }
        /*@-infloopsuncon@*/

        data_transfer.result = MFG_SPI_FLASH_RESULT_IN_PROGRESS;

        DEBUG_PRINTF(( "Received command: %s%s%s%s%s%s%s\n",
                        ( data_transfer.command & MFG_SPI_FLASH_COMMAND_INITIAL_VERIFY       )?   "INITIAL_VERIFY "        : "",
                        ( data_transfer.command & MFG_SPI_FLASH_COMMAND_ERASE_CHIP           )?   "ERASE_CHIP "            : "",
                        ( data_transfer.command & MFG_SPI_FLASH_COMMAND_WRITE                )?   "WRITE "                 : "",
                        ( data_transfer.command & MFG_SPI_FLASH_COMMAND_POST_WRITE_VERIFY    )?   "POST_WRITE_VERIFY "     : "",
                        ( data_transfer.command & MFG_SPI_FLASH_COMMAND_VERIFY_CHIP_ERASURE  )?   "VERIFY_CHIP_ERASURE "   : "",
                        ( data_transfer.command & MFG_SPI_FLASH_COMMAND_READ                 )?   "READ "                  : "",
                        ( data_transfer.command & MFG_SPI_FLASH_COMMAND_WRITE_ERASE_IF_NEEDED)?   "WRTIE_ERASE_IF_NEEDED " : ""
                    ));
        DEBUG_PRINTF(( "Destination address: %lu\n", data_transfer.sflash_address ));
        DEBUG_PRINTF(( "Size: %lu\n", data_transfer.size ));

        /* Check the data size is sane - cannot be bigger than the data storage area */
        if ( data_transfer.size > (unsigned long) __JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__ )
        {
            data_transfer.result = MFG_SPI_FLASH_RESULT_SIZE_TOO_BIG_BUFFER;
            DEBUG_PRINTF(( "Size %lu too big to for storage area %d - aborting!\n", data_transfer.size, __JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__));
            goto back_to_idle;
        }

        DEBUG_PRINTF(( "Initialising serial flash\n" ));

        /* Initialise the serial flash driver */
        if ( init_sflash( &sflash_handle, 0, SFLASH_WRITE_ALLOWED ) != 0 )
        {
            /* return from main in order to try rebooting... */
            return -1;
        }

        DEBUG_PRINTF(( "Done initialising\n" ));

        if ( ( data_transfer.command & MFG_SPI_FLASH_COMMAND_READ ) != 0 )
        {
            DEBUG_PRINTF(( "Reading data!\n" ));

            /* Read data from SPI FLASH memory */
            pos = 0;

            if ( 0 != sflash_read( &sflash_handle, (unsigned long) (pos + data_transfer.sflash_address) , data_transfer.data, (unsigned int) data_transfer.size ) )
            {
                /* Read failed */
                data_transfer.result = MFG_SPI_FLASH_RESULT_READ_FAILED;
                DEBUG_PRINTF(( "Read error - abort!\n" ));
                goto back_to_idle;
            }
#ifdef DEBUG_PRINT_READ_CONTENT
            {
                int i;
                for( i = 0;i < data_transfer.size; i++ )
                {
                    DEBUG_PRINTF( ( "%02X ", data_transfer.data[i]));
                    if( ( i & 0xf ) == 0xf )
                    {
                        DEBUG_PRINTF( ( "\n") );
                    }
                }
            }
#endif /* DEBUG_PRINT_READ_CONTENT */


            data_transfer.result = MFG_SPI_FLASH_RESULT_OK;
            DEBUG_PRINTF(( "Finished Read!\n" ));
            goto back_to_idle;
        }


        /* Check the data will fit on the sflash chip */
        sflash_get_size( &sflash_handle, &chip_size );
        DEBUG_PRINTF(("\n\nSFLASH Device ID ( 0x%lx) \n", sflash_handle.device_id ));
        DEBUG_PRINTF(("SFLASH Size      ( 0x%lx)\n\n", chip_size ));
        if ( ( chip_size != 0 ) && ( data_transfer.size + data_transfer.sflash_address  > chip_size ) )
        {
            data_transfer.result = MFG_SPI_FLASH_RESULT_SIZE_TOO_BIG_CHIP;
            DEBUG_PRINTF(( "Size (%lu from address %lu) too big to fit on chip (%lu) - aborting!\n", data_transfer.size, data_transfer.sflash_address, chip_size ));
            goto back_to_idle;
        }
        if ( ( data_transfer.command & MFG_SPI_FLASH_COMMAND_INITIAL_VERIFY ) != 0 )
        {
            DEBUG_PRINTF(( "Verifying existing data!\n" ));

            /* Read data from SPI FLASH memory */
            pos = 0;
            while ( pos < data_transfer.size )
            {
                unsigned int read_size = ( data_transfer.size - pos > (unsigned long) sizeof(Rx_Buffer) )? (unsigned int) sizeof(Rx_Buffer) : (unsigned int) ( data_transfer.size - pos );
                if ( 0 != sflash_read( &sflash_handle, pos + data_transfer.sflash_address, Rx_Buffer, read_size ) )
                {
                    /* Verify Error - Chip not erased properly */
                    data_transfer.result = MFG_SPI_FLASH_RESULT_READ_FAILED;
                    DEBUG_PRINTF(( "Read error - abort!\n" ));
                    goto back_to_idle;
                }
                if ( 0 != memcmp( Rx_Buffer, &data_transfer.data[pos], (size_t) read_size ) )
                {
                    /* Existing data different */
                    DEBUG_PRINTF(( "Existing data is different - stop verification\n" ));
                    /*@innerbreak@*/ /* Only break out of inner-most loop */
                    break;
                }
                pos += read_size;
                (void) platform_watchdog_kick( );
            }
            if ( pos >= data_transfer.size )
            {
                /* Existing data matches */
                /* No write required */
                data_transfer.result = MFG_SPI_FLASH_RESULT_OK;
                DEBUG_PRINTF(( "Existing data matches - successfully aborting!\n" ));
                goto back_to_idle;
            }
        }



        if ( ( data_transfer.command & MFG_SPI_FLASH_COMMAND_ERASE_CHIP ) != 0 )
        {
            DEBUG_PRINTF(( "Erasing entire chip\n" ));

            /* Erase the serial flash chip */
            sflash_chip_erase( &sflash_handle );
            if ( ( data_transfer.command & MFG_SPI_FLASH_COMMAND_VERIFY_CHIP_ERASURE ) != 0 )
            {
                DEBUG_PRINTF(( "Verifying erasure of entire chip\n" ));

                /* Verify Erasure */
                pos = 0;
                while ( pos < chip_size )
                {
                    uint8_t* cmp_ptr;
                    uint8_t* end_ptr;
                    unsigned int read_size;

                    read_size = ( chip_size - pos > (unsigned long) sizeof(Rx_Buffer) )? (unsigned int) sizeof(Rx_Buffer) : (unsigned int) ( chip_size - pos );
                    if ( 0 != sflash_read( &sflash_handle, (unsigned long) pos, Rx_Buffer, read_size ) )
                    {
                        /* Verify Error - Chip not erased properly */
                        data_transfer.result = MFG_SPI_FLASH_RESULT_READ_FAILED;
                        DEBUG_PRINTF(( "Read error - abort!\n" ));
                        goto back_to_idle;
                    }
                    cmp_ptr = Rx_Buffer;
                    end_ptr = &Rx_Buffer[read_size];
                    while ( ( cmp_ptr <  end_ptr ) && ( *cmp_ptr == (uint8_t) 0xff ) )
                    {
                        cmp_ptr++;
                    }
                    if ( cmp_ptr < end_ptr )
                    {
                        /* Verify Error - Chip not erased properly */
                        data_transfer.result = MFG_SPI_FLASH_RESULT_ERASE_FAILED;
                        DEBUG_PRINTF(( "Chip was not erased properly - abort!\n" ));
                        goto back_to_idle;
                    }
                    pos += read_size;
                    (void) platform_watchdog_kick( );
                }
            }
        }

        if ( ( data_transfer.command & MFG_SPI_FLASH_COMMAND_WRITE ) != 0 )
        {
            DEBUG_PRINTF(( "Writing location\n" ));

            /* Write the WLAN firmware into memory */
            pos = 0;
            while ( pos < data_transfer.size )
            {
                unsigned int write_size = ( data_transfer.size - pos > (unsigned long) WRITE_CHUNK_SIZE )? (unsigned int) WRITE_CHUNK_SIZE : (unsigned int) ( data_transfer.size - pos );
                if ( 0 != sflash_write( &sflash_handle, (unsigned long)(pos + data_transfer.sflash_address ), &data_transfer.data[pos], write_size ) )
                {
                    /* Verify Error - Chip not erased properly */
                    data_transfer.result = MFG_SPI_FLASH_RESULT_WRITE_FAILED;
                    DEBUG_PRINTF(( "Write error - abort!\n" ));
                    goto back_to_idle;
                }
                pos += write_size;
                (void) platform_watchdog_kick( );
            }

        }

        if ( ( data_transfer.command & MFG_SPI_FLASH_COMMAND_WRITE_ERASE_IF_NEEDED ) != 0 )
        {
            uint32_t compare_start = (uint32_t) data_transfer.sflash_address % SECTOR_SIZE;
            int32_t size_left_to_compare = (int32_t) data_transfer.size;
            unsigned char* cmp_ptr = data_transfer.data;
            pos = (data_transfer.sflash_address / SECTOR_SIZE) * SECTOR_SIZE;
            DEBUG_PRINTF(( "Verifying existing data!\n" ));

            /* Read data from SPI FLASH memory */
            while ( size_left_to_compare > 0 )
            {
                uint32_t cmp_len;
                if ( 0 != sflash_read( &sflash_handle, pos, Rx_Buffer, (unsigned int) SECTOR_SIZE ) )
                {
                    /* Verify Error - Chip not read properly */
                    data_transfer.result = MFG_SPI_FLASH_RESULT_READ_FAILED;
                    DEBUG_PRINTF(( "Read error - abort!\n" ));
                    goto back_to_idle;
                }
                cmp_len = (uint32_t) MIN( (int32_t) ( SECTOR_SIZE - compare_start ), size_left_to_compare );
#ifndef ALWAYS_ERASE_SECTOR
                if ( 0 != memcmp( &Rx_Buffer[compare_start], cmp_ptr, cmp_len ) )
#endif /* ALWAYS_ERASE_SECTOR */
                {
                    /* Existing data different */
                    DEBUG_PRINTF(( "Erasing sector %lu\n", pos / SECTOR_SIZE ));

                    if ( 0 != sflash_sector_erase( &sflash_handle, pos ) )
                    {
                        /* Sector Erase Error - Chip not erased properly */
                        data_transfer.result = MFG_SPI_FLASH_RESULT_ERASE_FAILED;
                        DEBUG_PRINTF(( "Sector erase error - abort!\n" ));
                        goto back_to_idle;
                    }

#ifdef VERIFY_ERASE_SECTOR
                    {
                        uint8_t* cmp_ptr2;
                        uint8_t* end_ptr;
                        cmp_ptr2 = Rx_Buffer;
#ifdef DEBUG
                        memset(Rx_Buffer,0xa5,SECTOR_SIZE);
#endif /* DEBUG */
                        if ( 0 != sflash_read( &sflash_handle, pos, Rx_Buffer, (unsigned int) SECTOR_SIZE ) )
                        {
                            /* Verify Error - Chip not read properly */
                            data_transfer.result = MFG_SPI_FLASH_RESULT_READ_FAILED;
                            DEBUG_PRINTF(( "Read error - abort!\n" ));
                            goto back_to_idle;
                        }

                        end_ptr = &Rx_Buffer[SECTOR_SIZE];
                        while ( ( cmp_ptr2 <  end_ptr ) && ( *cmp_ptr2 == (uint8_t) 0xff ) )
                        {
                            cmp_ptr2++;
                        }
                        if ( cmp_ptr2 < end_ptr )
                        {
                            /* Verify Error - sector not erased properly */
                            data_transfer.result = MFG_SPI_FLASH_RESULT_ERASE_FAILED;
                            DEBUG_PRINTF(( "Sector was not erased properly - abort!\n" ));
                            DEBUG_PRINTF(( "First bystes read: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n", Rx_Buffer[0], Rx_Buffer[1], Rx_Buffer[2], Rx_Buffer[3],
                                                                                                                           Rx_Buffer[4], Rx_Buffer[5], Rx_Buffer[6], Rx_Buffer[7] ) );
                            goto back_to_idle;
                        }
                    }
#endif /* ifdef VERIFY_ERASE_SECTOR */

                    DEBUG_PRINTF(( "Writing sector %lu\n", pos / SECTOR_SIZE ));
                    memcpy( &Rx_Buffer[compare_start], cmp_ptr, cmp_len );
                    if ( 0 != sflash_write( &sflash_handle, pos, Rx_Buffer, (unsigned int) SECTOR_SIZE ) )
                    {
                        /* Write Error - Chip not written properly */
                        data_transfer.result = MFG_SPI_FLASH_RESULT_WRITE_FAILED;
                        DEBUG_PRINTF(( "Write error - abort!\n" ));
                        goto back_to_idle;
                    }
                }
                cmp_ptr += cmp_len;
                size_left_to_compare -= cmp_len;
                compare_start = 0;

                pos += SECTOR_SIZE;
                (void) platform_watchdog_kick( );
            }

        }

        if ( ( data_transfer.command & MFG_SPI_FLASH_COMMAND_POST_WRITE_VERIFY ) != 0 )
        {
            DEBUG_PRINTF(( "Verifying after write\n" ));

            /* Read data from SPI FLASH memory */
            pos = 0;
            while ( pos < data_transfer.size )
            {
                unsigned int read_size = ( data_transfer.size - pos > (unsigned long) sizeof(Rx_Buffer) )? (unsigned int) sizeof(Rx_Buffer) : data_transfer.size - pos;
                if ( 0 != sflash_read( &sflash_handle, pos + data_transfer.sflash_address, Rx_Buffer, read_size ) )
                {
                    /* Verify Error - Chip not erased properly */
                    data_transfer.result = MFG_SPI_FLASH_RESULT_READ_FAILED;
                    DEBUG_PRINTF(( "Read error - abort!\n" ));
                    goto back_to_idle;
                }
#ifdef DEBUG_PRINT_VERIFY_READ_CONTENT
                {
                    int i;
                    for( i = 0;i < read_size; i++ )
                    {
                        DEBUG_PRINTF( ( "%02X ", Rx_Buffer[i]));
                        if( ( i & 0xf ) == 0xf )
                        {
                            DEBUG_PRINTF( ( "\n") );
                        }
                    }
                }
#endif /* DEBUG_PRINT_VERIFY_READ_CONTENT */
                if ( 0 != memcmp( Rx_Buffer, &data_transfer.data[pos], (size_t) read_size ) )
                {
                    /* Verify Error - Read data different to written data */
                    data_transfer.result = MFG_SPI_FLASH_RESULT_VERIFY_AFTER_WRITE_FAILED;
                    DEBUG_PRINTF(( "Verify error - Data was not written successfully - abort!\n" ));
                    goto back_to_idle;
                }
                pos += read_size;
                (void) platform_watchdog_kick( );
            }
        }


        /* OK! */
        data_transfer.result = MFG_SPI_FLASH_RESULT_OK;

back_to_idle:
        DEBUG_PRINTF(( "Going back to idle\n" ));
        deinit_sflash( &sflash_handle);
        data_transfer.command = MFG_SPI_FLASH_COMMAND_NONE;
        data_transfer.sflash_address = 0;
        data_transfer.size = 0;
    }

    return 0;
}


#ifdef WIPE_SFLASH

static void add_wipe_data( void )
{
    data_transfer.command = MFG_SPI_FLASH_COMMAND_ERASE_CHIP;
    data_transfer.sflash_address = 0;
    data_transfer.size = 0;
    data_transfer.result = MFG_SPI_FLASH_RESULT_IN_PROGRESS;
}

#elif defined( TEST_SFLASH_WRITE )

static void add_test_data( void )
{
    const char test_data[] = /* 0 */
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             /* 1024 */
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             /* 2048 */
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             /* 3072 */
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             /* 4096 */
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
                             "0123456789abcdef0123456789abcdef012";
                             /* 4643 */

    data_transfer.command = MFG_SPI_FLASH_COMMAND_WRITE_ERASE_IF_NEEDED | MFG_SPI_FLASH_COMMAND_POST_WRITE_VERIFY;
//    data_transfer.command |=  MFG_SPI_FLASH_COMMAND_ERASE_CHIP;
    data_transfer.sflash_address = 11;
    data_transfer.size = sizeof(test_data)-1;
    data_transfer.result = MFG_SPI_FLASH_RESULT_IN_PROGRESS;
    memcpy( data_transfer.data, test_data, sizeof(test_data)-1);
}

#elif defined( TEST_SFLASH_READ )

static void read_test_data2( void )
{
    data_transfer.command = MFG_SPI_FLASH_COMMAND_READ;
    data_transfer.sflash_address = 0x00;
    data_transfer.size = 0x1000;
    data_transfer.result = MFG_SPI_FLASH_RESULT_IN_PROGRESS;
}

#endif /* ifdef TEST_SFLASH_READ */





