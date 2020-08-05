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
#if defined ( IAR_TOOLCHAIN )
#include "platform_cmsis.h"
#endif
#include <string.h>
#include "ring_buffer.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define MIN(a,b) a < b ? a : b

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

/******************************************************
 *               Function Definitions
 ******************************************************/


wiced_result_t ring_buffer_init( /*@out@*/ wiced_ring_buffer_t* ring_buffer, /*@keep@*/ uint8_t* buffer, uint32_t buffer_size )
{
    if (ring_buffer)
    {
        ring_buffer->buffer = (uint8_t*)buffer;
        ring_buffer->size   = buffer_size;
        ring_buffer->head   = 0;
        ring_buffer->tail   = 0;
        return WICED_SUCCESS;
    }
    else
        return WICED_BADARG;
}

wiced_result_t ring_buffer_deinit( wiced_ring_buffer_t* ring_buffer )
{
    return WICED_SUCCESS;
}

uint32_t ring_buffer_write( wiced_ring_buffer_t* ring_buffer, const uint8_t* data, uint32_t data_length )
{
    uint32_t tail;
    uint32_t tail_to_end;
    uint32_t free_space;
    uint32_t amount_to_copy;

    tail = ring_buffer->tail;
    /* Count the number of number of elements (inclusively) between the tail element and the end of the buffer */
    tail_to_end = ring_buffer->size - tail;
    /* Max amount available for writing */
    free_space = (tail_to_end - 1 + ring_buffer->head) % ring_buffer->size;
    /* Calculate the maximum amount we can copy */
    amount_to_copy = MIN(data_length, free_space);

    /* Copy as much as we can until we fall off the end of the buffer */
    memcpy(&ring_buffer->buffer[tail], data, MIN(amount_to_copy, tail_to_end));

    /* Check if we have more to copy to the front of the buffer */
    if ( tail_to_end < amount_to_copy )
    {
        memcpy( &ring_buffer->buffer[ 0 ], data + tail_to_end, amount_to_copy - tail_to_end );
    }

    /* Update the tail */
    ring_buffer->tail = (ring_buffer->tail + amount_to_copy) % ring_buffer->size;

    return amount_to_copy;
}

wiced_result_t ring_buffer_get_data( wiced_ring_buffer_t* ring_buffer, uint8_t** data, uint32_t* contiguous_bytes )
{
    uint32_t head;
    uint32_t head_to_end;
    uint32_t used_space;

    head = ring_buffer->head;
    /* Count the number of number of elements (inclusively) between the head element and the end of the buffer */
    head_to_end = ring_buffer->size - head;
    /* Max amount available for reading */
    used_space = (head_to_end + ring_buffer->tail) % ring_buffer->size;

    /* Get a pointer to the start of data in the buffer */
    *data = &ring_buffer->buffer[head];

    /* Calculate the amount of contiguous elements */
    *contiguous_bytes = MIN(head_to_end, used_space);

    return WICED_SUCCESS;
}

wiced_result_t ring_buffer_consume( wiced_ring_buffer_t* ring_buffer, uint32_t bytes_consumed )
{
    /* Consume elements by updating the head */
    ring_buffer->head = (ring_buffer->head + bytes_consumed) % ring_buffer->size;
    return WICED_SUCCESS;
}


wiced_result_t ring_buffer_read( wiced_ring_buffer_t* ring_buffer, uint8_t* data, uint32_t data_length, uint32_t* number_of_bytes_read )
{
    uint32_t max_bytes_to_read;
    uint32_t i;
    uint32_t head;

    // wiced_assert("Bad args", ring_buffer != NULL && data != NULL && number_of_bytes_read != NULL);
    head = ring_buffer->head;

    /* read used space -- putting this in MIN macro leads to a race condition */
    max_bytes_to_read = ring_buffer_used_space(ring_buffer);

    /* Calculate the amount of data to read out of the buffer */
    max_bytes_to_read = MIN(data_length, max_bytes_to_read);

    /* Copy data from the ring buffer to the output buffer */
    if ( max_bytes_to_read != 0 )
    {
        for ( i = 0; i != max_bytes_to_read; i++, ( head = ( head + 1 ) % ring_buffer->size ) )
        {
            data[ i ] = ring_buffer->buffer[ head ];
        }

        ring_buffer_consume( ring_buffer, max_bytes_to_read );
    }

    *number_of_bytes_read = max_bytes_to_read;
    return WICED_SUCCESS;
}

uint32_t ring_buffer_free_space( wiced_ring_buffer_t* ring_buffer )
{
    uint32_t tail_to_end = ring_buffer->size - ring_buffer->tail;
    return ((tail_to_end - 1 + ring_buffer->head) % ring_buffer->size);
}

uint32_t ring_buffer_used_space( wiced_ring_buffer_t* ring_buffer )
{
    uint32_t head_to_end = ring_buffer->size - ring_buffer->head;
    return ((head_to_end + ring_buffer->tail) % ring_buffer->size);
}
