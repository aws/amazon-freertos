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

#include <string.h>
#include "wwd_debug.h"
#include "wwd_assert.h"
#include "network/wwd_buffer_interface.h"
#include "platform/wwd_resource_interface.h"
#include "network/wwd_network_constants.h"
#include "wwd_bus_protocol_interface.h"
#include "../wwd_internal.h"   /* TODO: fix include dependency */
#include "chip_constants.h"
#include "platform_toolchain.h"
#include "wwd_structures.h"
#include "platform_isr.h"
#include "wwd_rtos_interface.h"
#include "platform_mcu_peripheral.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define VERIFY_RESULT( x ) \
    { \
        wwd_result_t verify_result; \
        verify_result = (x); \
        if ( verify_result != WWD_SUCCESS ) \
        { \
            wiced_assert( "command failed", ( 0 == 1 )); \
            return verify_result; \
        } \
    }

#ifndef WICED_SAVE_INTERRUPTS
#define WICED_SAVE_INTERRUPTS( flags ) do { UNUSED_PARAMETER( flags ); } while ( 0 );
#define WICED_RESTORE_INTERRUPTS( flags ) do { } while ( 0 );
#endif /* WICED_SAVE_INTERRUPTS */


/******************************************************
 *             Constants
 ******************************************************/
#define INDIRECT_BUFFER_SIZE                    ( 1024 )
#define WWD_BUS_ROUND_UP_ALIGNMENT              ( 64 )
#ifdef WWD_DIRECT_RESOURCES
#define WWD_BUS_MAX_TRANSFER_SIZE               ( 16 * 1024 )
#else /* ifdef WWD_DIRECT_RESOURCES */
#define WWD_BUS_MAX_TRANSFER_SIZE               ( WWD_BUS_MAX_BACKPLANE_TRANSFER_SIZE )
#endif /* ifdef WWD_DIRECT_RESOURCES */

#define WWD_BUS_WLAN_ALLOW_SLEEP_INVALID_MS  ((uint32_t)-1)

/******************************************************
 *             Structures
 ******************************************************/

/******************************************************
 *             Variables
 ******************************************************/
#if PLATFORM_WLAN_ALLOW_BUS_TO_SLEEP_DELAY_MS
static wwd_time_t delayed_bus_release_deadline                          = 0;
static wiced_bool_t delayed_bus_release_scheduled                       = WICED_FALSE;
static uint32_t delayed_bus_release_timeout_ms                          = PLATFORM_WLAN_ALLOW_BUS_TO_SLEEP_DELAY_MS;
static volatile uint32_t delayed_bus_release_timeout_ms_request         = WWD_BUS_WLAN_ALLOW_SLEEP_INVALID_MS;
#endif /* PLATFORM_WLAN_ALLOW_BUS_TO_SLEEP_DELAY_MS */
static uint32_t                 backplane_window_current_base_address   = 0;
static volatile wiced_bool_t    resource_download_abort                 = WICED_FALSE;
#ifdef WLAN_ARM_CR4
static uint32_t                 reset_instr                               = 0;
#endif

/******************************************************
 *             Function declarations
 ******************************************************/
static wwd_result_t download_resource( wwd_resource_t resource, uint32_t address );
#if defined( WWD_DIRECT_RESOURCES )
static wwd_result_t wwd_bus_data_tranfer( wiced_bool_t write, uint32_t address, uint8_t *data, uint32_t image_size );
#endif
/******************************************************
 *             Function definitions
 ******************************************************/
void wwd_bus_wlan_set_delayed_bus_powersave_milliseconds( uint32_t time_ms )
{
#if PLATFORM_WLAN_ALLOW_BUS_TO_SLEEP_DELAY_MS

    delayed_bus_release_timeout_ms_request = time_ms;
    wwd_thread_notify( );

#else

    UNUSED_PARAMETER( time_ms );

#endif /* PLATFORM_WLAN_ALLOW_BUS_TO_SLEEP_DELAY_MS */
}

#if PLATFORM_WLAN_ALLOW_BUS_TO_SLEEP_DELAY_MS
void wwd_delayed_bus_release_schedule_update( wiced_bool_t is_scheduled )
{
    delayed_bus_release_scheduled = is_scheduled;
    delayed_bus_release_deadline = 0;
}
#endif /* PLATFORM_WLAN_ALLOW_BUS_TO_SLEEP_DELAY_MS */

uint32_t wwd_bus_handle_delayed_release( void )
{
    uint32_t time_until_release = 0;

#if PLATFORM_WLAN_ALLOW_BUS_TO_SLEEP_DELAY_MS
    if ( delayed_bus_release_timeout_ms_request != WWD_BUS_WLAN_ALLOW_SLEEP_INVALID_MS )
    {
        wiced_bool_t schedule = ( ( delayed_bus_release_scheduled != 0 ) || ( delayed_bus_release_deadline != 0 ) ) ? WICED_TRUE : WICED_FALSE;
        uint32_t     flags;

        WICED_SAVE_INTERRUPTS( flags );
        delayed_bus_release_timeout_ms         = delayed_bus_release_timeout_ms_request;
        delayed_bus_release_timeout_ms_request = WWD_BUS_WLAN_ALLOW_SLEEP_INVALID_MS;
        WICED_RESTORE_INTERRUPTS( flags );

        DELAYED_BUS_RELEASE_SCHEDULE( schedule );
    }

    if ( delayed_bus_release_scheduled == WICED_TRUE )
    {
        delayed_bus_release_scheduled = WICED_FALSE;

        if ( delayed_bus_release_timeout_ms != 0 )
        {
            delayed_bus_release_deadline = host_rtos_get_time() + delayed_bus_release_timeout_ms;
            time_until_release = delayed_bus_release_timeout_ms;
        }
    }
    else if ( delayed_bus_release_deadline != 0 )
    {
        wwd_time_t now = host_rtos_get_time( );

        if ( delayed_bus_release_deadline - now <= delayed_bus_release_timeout_ms )
        {
            time_until_release = delayed_bus_release_deadline - now;
        }

        if ( time_until_release == 0 )
        {
            delayed_bus_release_deadline = 0;
        }
    }

    if ( time_until_release != 0 )
    {
        if ( wwd_bus_is_up( ) == WICED_FALSE )
        {
            time_until_release = 0;
        }
        else if ( wwd_bus_platform_mcu_power_save_deep_sleep_enabled( ) )
        {
            time_until_release = 0;
        }
    }
#endif /* PLATFORM_WLAN_ALLOW_BUS_TO_SLEEP_DELAY_MS */
    return time_until_release;
}

WEAK wiced_bool_t wwd_bus_platform_mcu_power_save_deep_sleep_enabled( void )
{
    return WICED_FALSE;
}

void wwd_bus_init_backplane_window( void )
{
    backplane_window_current_base_address = 0;
}


WEAK wwd_result_t wwd_bus_write_wifi_firmware_image( void )
{
    wwd_result_t result = WWD_SUCCESS;

#ifdef WLAN_ARM_CR4
     result = download_resource( WWD_RESOURCE_WLAN_FIRMWARE, ATCM_RAM_BASE_ADDRESS );
#else
     result = download_resource( WWD_RESOURCE_WLAN_FIRMWARE, 0 );
#endif
    return result;
}

WEAK wwd_result_t wwd_bus_write_wifi_nvram_image( void )
{
    wwd_result_t result;
    uint32_t image_size;
    uint32_t img_base;
    uint32_t img_end;
    /* Get the size of the variable image */
    host_platform_resource_size( WWD_RESOURCE_WLAN_NVRAM, &image_size );

    /* Round up the size of the image */
    image_size = ROUND_UP( image_size, WWD_BUS_ROUND_UP_ALIGNMENT );

    /* Write image */
    img_end = CHIP_RAM_SIZE - 4;
    img_base = ( img_end - image_size );
#ifdef WLAN_ARM_CR4
    img_base += ATCM_RAM_BASE_ADDRESS;
#endif
    result = download_resource( WWD_RESOURCE_WLAN_NVRAM, img_base );

    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    /* Write the variable image size at the end */
    image_size = ( ~( image_size / 4 ) << 16 ) | ( image_size / 4 );
#ifdef WLAN_ARM_CR4
    img_end += ATCM_RAM_BASE_ADDRESS;
#endif

    result = wwd_bus_write_backplane_value( (uint32_t)img_end, 4, image_size );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }
#ifdef WLAN_ARM_CR4

    /* write address 0 with reset instruction */
    result = wwd_bus_write_backplane_value( 0 , sizeof(reset_instr), reset_instr );


    if ( result == WWD_SUCCESS ) {
      uint32_t tmp;

      /* verify reset instruction value */
      result = wwd_bus_read_backplane_value( 0 , sizeof(tmp), (uint8_t *)&tmp );

     if ( result == WWD_SUCCESS && tmp != reset_instr ) {
         WPRINT_WWD_ERROR(("%s: Failed to write 0x%08lx to addr 0\n",
                                  __FUNCTION__, reset_instr));
         WPRINT_WWD_ERROR(("%s: contents of addr 0 is 0x%08lx\n",
                                  __FUNCTION__, tmp));
        return WWD_WLAN_SDIO_ERROR;
      }
    }
#endif
    return WWD_SUCCESS;
}

WEAK void wwd_bus_set_resource_download_halt( wiced_bool_t halt )
{
    resource_download_abort = halt;
}


#if defined( WWD_DIRECT_RESOURCES )
static wwd_result_t wwd_bus_data_tranfer( wiced_bool_t write, uint32_t address, uint8_t *data, uint32_t image_size )
{
    uint32_t transfer_progress;
    uint16_t transfer_size;
    wwd_result_t result;

    for ( transfer_progress = 0; transfer_progress < image_size; transfer_progress += transfer_size, address += transfer_size, data += transfer_size )
       {
           if ( resource_download_abort == WICED_TRUE )
           {
               WPRINT_WWD_INFO(("Download_resource is aborted; terminating after %d iterations\n", transfer_progress));
               return WWD_UNFINISHED;
           }

           /* Set the backplane window */
           if ( WWD_SUCCESS != ( result = wwd_bus_set_backplane_window( address ) ) )
           {
               return result;
           }
           transfer_size = (uint16_t) MIN( WWD_BUS_MAX_TRANSFER_SIZE, (int) ( image_size - transfer_progress ) );
           /* Round up the size of the chunk */
           transfer_size = (uint16_t) ROUND_UP( transfer_size, WWD_BUS_ROUND_UP_ALIGNMENT );
           if ( ( result = wwd_bus_transfer_bytes( write, BACKPLANE_FUNCTION, address & BACKPLANE_ADDRESS_MASK, transfer_size, (wwd_transfer_bytes_packet_t*) data ) ) != WWD_SUCCESS)
           {
               return result;
           }

       }
    return WWD_SUCCESS;
}
static wwd_result_t download_resource( wwd_resource_t resource, uint32_t address )
{
    wwd_result_t result;
    uint8_t* image;



    uint32_t image_size;

    host_platform_resource_size( resource, &image_size );

    result = host_platform_resource_size( resource, &image_size );

    if( result != WWD_SUCCESS )
    {
        WPRINT_WWD_ERROR(("Fatal error: download_resource doesn't exist\n"));
        return result;
    }

    if ( image_size <= 0 )
    {
        WPRINT_WWD_ERROR(("Fatal error: download_resource cannot load with invalid size\n"));
        return WWD_BADARG;
    }

    host_platform_resource_read_direct( resource, (const void**)&image );
#ifdef WLAN_ARM_CR4
    if ( resource == WWD_RESOURCE_WLAN_FIRMWARE )
    reset_instr = *(((uint32_t *)image));
#endif
    result = wwd_bus_data_tranfer( BUS_WRITE, address, image, image_size );

    return WWD_SUCCESS;
}

#else /* ! defined( WWD_DIRECT_RESOURCES ) */

static wwd_result_t download_resource( wwd_resource_t resource, uint32_t address )
{
    uint32_t transfer_progress;
    wiced_buffer_t buffer;
    uint32_t buffer_size = INDIRECT_BUFFER_SIZE;

    uint32_t size;
    wwd_result_t result = WWD_SUCCESS;

    result = host_platform_resource_size( resource, &size );

    if( result != WWD_SUCCESS )
    {
        WPRINT_WWD_ERROR(("Fatal error: download_resource doesn't exist\n"));
        goto exit;
    }

    if ( size <= 0 )
    {
        WPRINT_WWD_ERROR(("Fatal error: download_resource cannot load with invalid size\n"));
        result = WWD_BADARG;
        goto exit;
    }

    /* Transfer firmware image into the RAM */
    transfer_progress = 0;

    do
    {
        result = host_buffer_get( &buffer, WWD_NETWORK_TX, (unsigned short) ( buffer_size + sizeof(wwd_buffer_header_t) ), WICED_FALSE );
    } while ( ( result == WWD_BUFFER_UNAVAILABLE_PERMANENT ) && ( ( buffer_size >>= 1 ) > 1 ) );

    while ( transfer_progress < size )
    {
        uint8_t* packet;
        uint16_t transfer_size;
        uint32_t segment_size;

        if ( result != WWD_SUCCESS )
        {
            WPRINT_WWD_ERROR(("Fatal error: download_resource cannot allocate buffer\n"));
            goto exit;
        }
        packet = (uint8_t*) host_buffer_get_current_piece_data_pointer( buffer );

        host_platform_resource_read_indirect( resource, transfer_progress, packet + sizeof(wwd_buffer_header_t), buffer_size, &segment_size );

#ifdef WLAN_ARM_CR4
    if ( ( resource == WWD_RESOURCE_WLAN_FIRMWARE ) && ( reset_instr == 0 ) )
    {
        reset_instr = *((uint32_t*)(packet + sizeof(wwd_buffer_header_t)));
    }
#endif /* WLAN_ARM_CR4 */
        for ( ; segment_size != 0; segment_size -= transfer_size, packet += transfer_size, transfer_progress += transfer_size, address += transfer_size )
        {
            if ( resource_download_abort == WICED_TRUE )
            {
                WPRINT_WWD_ERROR(("Download_resource is aborted; terminating after %lu iterations\n", transfer_progress));
                host_buffer_release( buffer, WWD_NETWORK_TX );
                result = WWD_UNFINISHED;
                goto exit;
            }
            transfer_size = (uint16_t) MIN( WWD_BUS_MAX_TRANSFER_SIZE, segment_size );
            result = wwd_bus_set_backplane_window( address );

            if ( result != WWD_SUCCESS )
            {
                host_buffer_release( buffer, WWD_NETWORK_TX );
                goto exit;
            }
            result = wwd_bus_transfer_bytes( BUS_WRITE, BACKPLANE_FUNCTION, ( address & BACKPLANE_ADDRESS_MASK ), transfer_size, (wwd_transfer_bytes_packet_t*) ( packet + sizeof(wwd_buffer_queue_ptr_t)) );
            if ( result != WWD_SUCCESS )
            {
                host_buffer_release( buffer, WWD_NETWORK_TX );
                goto exit;
            }
        }

    }

    host_buffer_release( buffer, WWD_NETWORK_TX );
exit:
    return result;
}

#endif /* if defined( WWD_DIRECT_RESOURCES ) */

/*
 * Update the backplane window registers
 */
WEAK wwd_result_t wwd_bus_set_backplane_window( uint32_t addr )
{
    wwd_result_t result = WWD_BUS_WRITE_REGISTER_ERROR;
    uint32_t base = addr & ( (uint32_t) ~BACKPLANE_ADDRESS_MASK );
    const uint32_t upper_32bit_mask         = 0xFF000000;
    const uint32_t upper_middle_32bit_mask  = 0x00FF0000;
    const uint32_t lower_middle_32bit_mask  = 0x0000FF00;

    if ( base == backplane_window_current_base_address )
    {
        return WWD_SUCCESS;
    }
    if ( ( base & upper_32bit_mask ) != ( backplane_window_current_base_address & upper_32bit_mask ) )
    {
        if ( WWD_SUCCESS != ( result = wwd_bus_write_register_value( BACKPLANE_FUNCTION, SDIO_BACKPLANE_ADDRESS_HIGH, (uint8_t) 1, ( base >> 24 ) ) ) )
        {
            return result;
        }
        /* clear old */
        backplane_window_current_base_address &= ~upper_32bit_mask;
        /* set new */
        backplane_window_current_base_address |= ( base & upper_32bit_mask );
    }

    if ( ( base & upper_middle_32bit_mask ) != ( backplane_window_current_base_address & upper_middle_32bit_mask ) )
    {
        if ( WWD_SUCCESS != ( result = wwd_bus_write_register_value( BACKPLANE_FUNCTION, SDIO_BACKPLANE_ADDRESS_MID, (uint8_t) 1, ( base >> 16 ) ) ) )
        {
            return result;
        }
        /* clear old */
        backplane_window_current_base_address &= ~upper_middle_32bit_mask;
        /* set new */
        backplane_window_current_base_address |= ( base & upper_middle_32bit_mask );
    }

    if ( ( base & lower_middle_32bit_mask ) != ( backplane_window_current_base_address & lower_middle_32bit_mask ) )
    {
        if ( WWD_SUCCESS != ( result = wwd_bus_write_register_value( BACKPLANE_FUNCTION, SDIO_BACKPLANE_ADDRESS_LOW, (uint8_t) 1, ( base >> 8 ) ) ) )
        {
            return result;
        }

        /* clear old */
        backplane_window_current_base_address &= ~lower_middle_32bit_mask;
        /* set new */
        backplane_window_current_base_address |= ( base & lower_middle_32bit_mask );
    }

    return WWD_SUCCESS;
}

/* Default implementation of WWD bus resume function, which does nothing */
WEAK wwd_result_t wwd_bus_resume_after_deep_sleep( void )
{
    wiced_assert( "In order to support deep-sleep platform need to implement this function", 0 );
    return WWD_UNSUPPORTED;
}

wwd_result_t wwd_bus_transfer_backplane_bytes( wwd_bus_transfer_direction_t direction, uint32_t address, uint32_t size, /*@in@*/ /*@out@*/ uint8_t* data )
{
    wiced_buffer_t pkt_buffer = NULL;
    uint8_t*       packet;
    uint32_t       transfer_size;
    uint32_t       remaining_buf_size;
    uint32_t       window_offset_address;
    wwd_result_t   result;

    result = host_buffer_get( &pkt_buffer, ( direction == BUS_READ )? WWD_NETWORK_RX : WWD_NETWORK_TX,
                  (uint16_t) ( WWD_BUS_MAX_BACKPLANE_TRANSFER_SIZE + WWD_BUS_BACKPLANE_READ_PADD_SIZE +
                  WWD_BUS_HEADER_SIZE ), WICED_TRUE );
    if ( result != WWD_SUCCESS )
    {
        goto done;
    }
    packet = (uint8_t*) host_buffer_get_current_piece_data_pointer( pkt_buffer );

    result = wwd_ensure_wlan_bus_is_up();
    if ( result != WWD_SUCCESS )
    {
        goto done;
    }

    remaining_buf_size = size;
    for (; remaining_buf_size != 0; remaining_buf_size -= transfer_size, address += transfer_size )
    {
        transfer_size = ( remaining_buf_size > WWD_BUS_MAX_BACKPLANE_TRANSFER_SIZE ) ?
                              WWD_BUS_MAX_BACKPLANE_TRANSFER_SIZE : remaining_buf_size;

        /* Check if the transfer crosses the backplane window boundary */
        window_offset_address = address & BACKPLANE_ADDRESS_MASK;
        if ( ( window_offset_address + transfer_size ) > BACKPLANE_ADDRESS_MASK )
        {
            /* Adjust the transfer size to within current window */
            transfer_size = BACKPLANE_WINDOW_SIZE - window_offset_address;
        }
        result = wwd_bus_set_backplane_window( address );
        if ( result != WWD_SUCCESS )
        {
            goto done;
        }

        if ( direction == BUS_WRITE )
        {
            memcpy( packet + WWD_BUS_HEADER_SIZE, data + size - remaining_buf_size, transfer_size );
            result = wwd_bus_transfer_bytes( direction, BACKPLANE_FUNCTION, ( address & BACKPLANE_ADDRESS_MASK ),
                        (uint16_t) transfer_size, (wwd_transfer_bytes_packet_t *) packet );
            if ( result != WWD_SUCCESS )
            {
                goto done;
            }
        }
        else
        {
            result = wwd_bus_transfer_bytes( direction, BACKPLANE_FUNCTION, ( address & BACKPLANE_ADDRESS_MASK ),
                        (uint16_t) ( transfer_size + WWD_BUS_BACKPLANE_READ_PADD_SIZE ),
                        (wwd_transfer_bytes_packet_t *) packet );
            if ( result != WWD_SUCCESS )
            {
                goto done;
            }
            memcpy( data + size - remaining_buf_size,
                    packet + WWD_BUS_HEADER_SIZE + WWD_BUS_BACKPLANE_READ_PADD_SIZE, transfer_size );
        }
    }

done:
    wwd_bus_set_backplane_window( CHIPCOMMON_BASE_ADDRESS );
    if ( pkt_buffer != NULL )
    {
        host_buffer_release( pkt_buffer, ( direction == BUS_READ )? WWD_NETWORK_RX : WWD_NETWORK_TX );
    }
    return result;
}

WEAK void wwd_bus_init_stats( void )
{
    /* To be implemented */
}

WEAK wwd_result_t wwd_bus_print_stats( wiced_bool_t reset_after_print )
{
    /* To be implemented */
    UNUSED_VARIABLE( reset_after_print );
    WPRINT_MACRO(( "Bus stats not available\n" ));
    return WWD_SUCCESS;
}

WEAK wiced_bool_t wwd_bus_wake_interrupt_present( void )
{
    /* functionality is only currently needed and present on SDIO */
    return WICED_FALSE;
}

WEAK wwd_result_t wwd_bus_reinit( wiced_bool_t wake_from_firmware )
{
    UNUSED_PARAMETER( wake_from_firmware );
    /* functionality is only currently needed and present on SDIO */
    return WWD_UNSUPPORTED;
}
