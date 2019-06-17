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
#if defined ( IAR_TOOLCHAIN )
#include "platform_cmsis.h"
#endif
#include <string.h>
#include <stdint.h>
#include "wwd_buffer.h"
#include "wwd_constants.h"
#include "wwd_assert.h"
#include "network/wwd_buffer_interface.h"

static /*@null@*/ wiced_buffer_t internal_buffer           = 0;
static uint16_t                      internal_buffer_max_size  = 0;
static uint16_t                      internal_buffer_curr_size = 0;

wwd_result_t wwd_buffer_init( void * native_arg )
{
    wiced_assert("Error: Invalid native_arg\n", native_arg != NULL);

    internal_buffer = (wiced_buffer_t) ( (nons_buffer_init_t*) native_arg )->internal_buffer;
    internal_buffer_max_size = ( (nons_buffer_init_t*) native_arg )->buff_size;
    return WWD_SUCCESS;
}

wwd_result_t host_buffer_check_leaked( void )
{
    /* Nothing to do */
    return WWD_SUCCESS;
}

wwd_result_t internal_host_buffer_get( wiced_buffer_t * buffer, wwd_buffer_dir_t direction, unsigned short size, unsigned long timeout_ms )
{
    /*@-noeffect@*/
    UNUSED_PARAMETER( direction );
    UNUSED_PARAMETER( timeout_ms );
    /*@+noeffect@*/

    if ( (uint16_t) size > internal_buffer_max_size )
    {
        *buffer = NULL;
        return WWD_BUFFER_UNAVAILABLE_PERMANENT;
    }
    internal_buffer_curr_size = (uint16_t) size;
    *buffer = internal_buffer;

    return WWD_SUCCESS;
}

wwd_result_t host_buffer_get( /*@out@*/ wiced_buffer_t * buffer, wwd_buffer_dir_t direction, unsigned short size, wiced_bool_t wait )
{
    wiced_assert("Error: Invalid buffer size\n", size != 0);
    return internal_host_buffer_get(buffer, direction, size, wait);
}

void host_buffer_release( /*@only@*/ wiced_buffer_t buffer, wwd_buffer_dir_t direction )
{
    /*@-noeffect@*/
    UNUSED_PARAMETER( buffer );
    UNUSED_PARAMETER( direction );
    /*@+noeffect@*/

    /*@-mustfree@*/ /* Buffers are not actually allocated in NoNS, hence are not freed */
    return;
    /*@+mustfree@*/
}

/*@exposed@*/ uint8_t* host_buffer_get_current_piece_data_pointer( /*@dependent@*/ wiced_buffer_t buffer )
{
    wiced_assert("Error: Invalid buffer\n", buffer != NULL);
    return (uint8_t *) buffer;
}

uint16_t host_buffer_get_current_piece_size( wiced_buffer_t buffer )
{
    /*@-noeffect@*/
    UNUSED_PARAMETER( buffer );
    /*@+noeffect@*/
    return internal_buffer_curr_size;
}

/*@exposed@*/ /*@null@*/ wiced_buffer_t host_buffer_get_next_piece( wiced_buffer_t buffer )
{
    /*@-noeffect@*/
    UNUSED_PARAMETER( buffer );
    /*@+noeffect@*/
    return NULL;
}

wwd_result_t host_buffer_add_remove_at_front( wiced_buffer_t * buffer, int32_t add_remove_amount )
{
    *buffer += add_remove_amount;
    internal_buffer_curr_size = (uint16_t) ( internal_buffer_curr_size - add_remove_amount );

    return WWD_SUCCESS;
}

wwd_result_t host_buffer_set_size( wiced_buffer_t buffer, unsigned short size )
{
    UNUSED_PARAMETER( buffer );
    internal_buffer_curr_size = size;
    return WWD_SUCCESS;
}
