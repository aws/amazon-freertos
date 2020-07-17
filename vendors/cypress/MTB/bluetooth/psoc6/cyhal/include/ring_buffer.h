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
#pragma once

#include <stdint.h>
#include "wiced_result.h"
#include "platform_cache_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#define DEFINE_RING_BUFFER_DATA( type, name, size ) DEFINE_CACHE_LINE_ALIGNED_ARRAY( type, name, size )

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

typedef /*@abstract@*/ /*@immutable@*/ struct
{
    uint8_t*  buffer;
    uint32_t  size;
    volatile uint32_t  head; /* Index of the buffer to read from */
    volatile uint32_t  tail; /* Index of the buffer to write to */
} wiced_ring_buffer_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/


/**
 *
 *  @addtogroup ringbuf     Ring buffer
 *  @ingroup utilities
 *
 * This library implements a lock-free ringbuffer as described in Donald E. Knuth, The Art of Computer Programming, Volume 3: Sorting and Searching.
 * Note that since the last entry of the ring buffer is not used in a lock free implementation, you should allocate one more entry then the total
 * number of entries you plan to use.
 *
 *  @{
 */


/** Initialize a ring buffer.
 *
 * Note: This is a non-locking ring buffer implementation that will store a
 *       maximum of buffer_size - 1 bytes of data.
 *
 * @param[out] ring_buffer : Pointer to the ring buffer structure to be initialized.
 * @param[in]  buffer      : Pointer to the buffer to use for the ring buffer.
 * @param[in]  buffer_size : Size of the buffer (maximum buffer_size - 1 bytes will be stored).
 *
 * @return @ref wiced_result_t
 */
wiced_result_t ring_buffer_init       ( /*@out@*/ wiced_ring_buffer_t* ring_buffer, /*@keep@*/ uint8_t* buffer, uint32_t buffer_size );

/** De-initialize a ring buffer.
 *
 * @param[in] ring_buffer : Pointer to the ring buffer structure.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t ring_buffer_deinit     ( wiced_ring_buffer_t* ring_buffer );

/** Write to a ring buffer.
 *
 * @param[in] ring_buffer : Pointer to the ring buffer structure.
 * @param[in] data        : Pointer to the data to write to the buffer.
 * @param[in] data_length : Length of data in bytes.
 *
 * @return Length of data successfully written to the buffer.
 */
uint32_t       ring_buffer_write      ( wiced_ring_buffer_t* ring_buffer, const uint8_t* data, uint32_t data_length );

/** Return the amount of used space in a ring buffer.
 *
 * @param[in] ring_buffer : Pointer to the ring buffer structure.
 *
 * @return Length of data currently in the buffer.
 */
uint32_t       ring_buffer_used_space ( wiced_ring_buffer_t* ring_buffer );

/** Return the amount of available space in a ring buffer.
 *
 * @param[in] ring_buffer : Pointer to the ring buffer structure.
 *
 * @return Length of available space in the buffer.
 */
uint32_t       ring_buffer_free_space ( wiced_ring_buffer_t* ring_buffer );

/** Get a pointer to the start of data in a ring buffer.
 *
 * @param[in] ring_buffer  : Pointer to the ring buffer structure.
 * @param[out] data        : Address of the pointer for the data start in the buffer.
 * @param[out] data_length : Length of contiguous data bytes in the buffer.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t ring_buffer_get_data   ( wiced_ring_buffer_t* ring_buffer, uint8_t** data, uint32_t* contiguous_bytes );

/** Consume (discard) data in a ring buffer.
 *
 * @param[in] ring_buffer    : Pointer to the ring buffer structure.
 * @param[in] bytes_consumed : Number of bytes to consume.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t ring_buffer_consume    ( wiced_ring_buffer_t* ring_buffer, uint32_t bytes_consumed );

/** Read data from a ring buffer.
 *
 * @param[in] ring_buffer           : Pointer to the ring buffer structure.
 * @param[in] data                  : Pointer to the buffer for read data.
 * @param[in] data_length           : Length of the data buffer.
 * @param[out] number_of_bytes_read : Number of bytes read from the ring buffer.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t ring_buffer_read       ( wiced_ring_buffer_t* ring_buffer, uint8_t* data, uint32_t data_length, uint32_t* number_of_bytes_read );

/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif
