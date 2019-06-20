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
 *  Defines the WICED Buffer Interface.
 *
 *  Provides prototypes for functions that allow WICED to use packet
 *  buffers in an abstract way.
 *
 */

#ifndef INCLUDED_WWD_BUFFER_INTERFACE_H_
#define INCLUDED_WWD_BUFFER_INTERFACE_H_

#include "wwd_buffer.h"
#include "wwd_constants.h"
#include "wwd_bus_protocol.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 * @cond       Constants
 ******************************************************/

typedef enum
{
    WWD_NETWORK_TX,
    WWD_NETWORK_RX
} wwd_buffer_dir_t;


typedef wiced_buffer_t  wwd_buffer_queue_ptr_t;

#pragma pack(1)

typedef struct
{
    /*@owned@*/  wwd_buffer_queue_ptr_t  queue_next;
#ifdef WWD_BUS_HAS_HEADER
                 wwd_bus_header_t        bus_header;
#endif /* ifdef WWD_BUS_HAS_HEADER */
} wwd_buffer_header_t;

#pragma pack()

/** @endcond */

/** @addtogroup buffif Buffer Interface
 * Allows WICED to use packet buffers in an abstract way.
 *  @{
 */

/******************************************************
 *             Function declarations
 ******************************************************/

/**
 * Initialize the packet buffer interface
 *
 * Implemented in the WICED buffer interface which is specific to the
 * buffering scheme in use.
 * Some implementations of the packet buffer interface may need additional
 * information for initialization, especially the location of packet buffer
 * pool(s). These can be passed via the 'native_arg' parameter.
 *
 * @param native_arg  An implementation specific argument
 *
 * @return WWD_SUCCESS = Success, Error code = Failure
 */

extern wwd_result_t wwd_buffer_init( /*@null@*/ void* native_arg );

/**
 * Deinitialize the packet buffer interface
 *
 * Implemented in the WICED buffer interface which is specific to the
 * buffering scheme in use.
 *
 * @return WWD_SUCCESS = Success, Error code = Failure
 */

extern wwd_result_t wwd_buffer_deinit( void );

/**
 * @brief Allocates a packet buffer
 *
 * Implemented in the WICED buffer interface which is specific to the
 * buffering scheme in use.
 * Attempts to allocate a packet buffer of the size requested. It can do this
 * by allocating a pre-existing packet from a pool, using a static buffer,
 * or by dynamically allocating memory. The method of allocation does not
 * concern WICED, however it must match the way the network stack expects packet
 * buffers to be allocated.
 *
 * @param buffer     A pointer which receives the allocated packet buffer handle
 * @param direction : Indicates transmit/receive direction that the packet buffer is
 *                    used for. This may be needed if tx/rx pools are separate.
 * @param size      : The number of bytes to allocate.
 * @param timeout_ms : Maximum period to block for. Can be passed NEVER_TIMEOUT to request no timeout
 *
 * @return WWD_SUCCESS = Success, Error code = Failure
 *
 */

extern wwd_result_t internal_host_buffer_get( /*@special@*/ /*@out@*/ wiced_buffer_t* buffer, wwd_buffer_dir_t direction, unsigned short size, unsigned long timeout_ms ) /*@allocates *buffer@*/  /*@defines **buffer@*/;

/**
 * @brief Allocates a packet buffer
 *
 * Implemented in the WICED buffer interface which is specific to the
 * buffering scheme in use.
 * Attempts to allocate a packet buffer of the size requested. It can do this
 * by allocating a pre-existing packet from a pool, using a static buffer,
 * or by dynamically allocating memory. The method of allocation does not
 * concern WICED, however it must match the way the network stack expects packet
 * buffers to be allocated.
 *
 * @param buffer     A pointer which receives the allocated packet buffer handle
 * @param direction : Indicates transmit/receive direction that the packet buffer is
 *                    used for. This may be needed if tx/rx pools are separate.
 * @param size      : The number of bytes to allocate.
 * @param wait      : Whether to wait for a packet buffer to be available
 *
 * @return WWD_SUCCESS = Success, Error code = Failure
 *
 */

extern wwd_result_t host_buffer_get( /*@special@*/ /*@out@*/ wiced_buffer_t* buffer, wwd_buffer_dir_t direction, unsigned short size, wiced_bool_t wait ) /*@allocates *buffer@*/  /*@defines **buffer@*/;

/**
 * Releases a packet buffer
 *
 * Implemented in the Wiced buffer interface, which will be specific to the
 * buffering scheme in use.
 * This function is used by WICED to indicate that it no longer requires
 * a packet buffer. The buffer can then be released back into a pool for
 * reuse, or the dynamically allocated memory can be freed, according to
 * how the packet was allocated.
 * Returns void since WICED cannot do anything about failures
 *
 * @param buffer    : the handle of the packet buffer to be released
 * @param direction : indicates transmit/receive direction that the packet buffer has
 *                    been used for. This might be needed if tx/rx pools are separate.
 *
 */
extern void host_buffer_release( /*@only@*/ wiced_buffer_t buffer, wwd_buffer_dir_t direction );

/**
 * Retrieves the current pointer of a packet buffer
 *
 * Implemented in the WICED buffer interface which is specific to the
 * buffering scheme in use.
 * Since packet buffers usually need to be created with space at the
 * front for additional headers, this function allows WICED to get
 * the current 'front' location pointer.
 *
 * @param buffer : The handle of the packet buffer whose pointer is to be retrieved
 *
 * @return The packet buffer's current pointer.
 */
extern /*@exposed@*/ uint8_t* host_buffer_get_current_piece_data_pointer( /*@temp@*/ wiced_buffer_t buffer );

/**
 * Retrieves the size of a packet buffer
 *
 * Implemented in the WICED buffer interface which is specific to the
 * buffering scheme in use.
 * Since packet buffers usually need to be created with space at the
 * front for additional headers, the memory block used to contain a packet buffer
 * will often be larger than the current size of the packet buffer data.
 * This function allows WICED to retrieve the current size of a packet buffer's data.
 *
 * @param buffer : The handle of the packet buffer whose size is to be retrieved
 *
 * @return The size of the packet buffer.
 */
extern uint16_t host_buffer_get_current_piece_size( /*@temp@*/ wiced_buffer_t buffer );

/**
 * Sets the current size of a Wiced packet
 *
 * Implemented in the WICED buffer interface which is specific to the
 * buffering scheme in use.
 * This function sets the current length of a WICED packet buffer
 *
 * @param buffer : The packet to be modified
 * @param size   : The new size of the packet buffer
 *
 * @return WWD_SUCCESS = Success, Error code = Failure
 */
extern wwd_result_t host_buffer_set_size( /*@temp@*/ wiced_buffer_t buffer, unsigned short size );

/**
 * Retrieves the next piece of a set of daisy chained packet buffers
 *
 * Implemented in the WICED buffer interface which is specific to the
 * buffering scheme in use.
 * Some buffering schemes allow buffers to be daisy chained into linked lists.
 * This allows more flexibility with packet buffers and avoids memory copies.
 * It does however require scatter-gather DMA for the hardware bus.
 * This function retrieves the next buffer in a daisy chain of packet buffers.
 *
 * @param buffer : The handle of the packet buffer whose next buffer is to be retrieved
 *
 * @return The handle of the next buffer, or NULL if there is none.
 */
extern /*@exposed@*/ /*@dependent@*/ /*@null@*/ wiced_buffer_t host_buffer_get_next_piece( /*@dependent@*/ wiced_buffer_t buffer );

/**
 * Moves the current pointer of a packet buffer
 *
 * Implemented in the WICED buffer interface which is specific to the
 * buffering scheme in use.
 * Since packet buffers usually need to be created with space at the
 * front for additional headers, this function allows WICED to move
 * the current 'front' location pointer so that it has space to add headers
 * to transmit packets, and so that the network stack does not see the
 * internal WICED headers on received packets.
 *
 * @param buffer    : A pointer to the handle of the current packet buffer
 *                    for which the current pointer will be moved. On return
 *                    this may contain a pointer to a newly allocated packet
 *                    buffer which has been daisy chained to the front of the
 *                    given one. This would be the case if the given packet buffer
 *                    didn't have enough space at the front.
 * @param add_remove_amount : This is the number of bytes to move the current pointer
 *                            of the packet buffer - a negative value increases the space
 *                            for headers at the front of the packet, a positive value
 *                            decreases the space.
 * @return WWD_SUCCESS = Success, Error code = Failure
 */
extern wwd_result_t host_buffer_add_remove_at_front( wiced_buffer_t* buffer, int32_t add_remove_amount ); /* Adds or removes buffer parts as needed (and returns new handle) - new bytes must be contiguous with each other, but not necessarily with original bytes */


extern wwd_result_t host_buffer_set_next_piece(wiced_buffer_t buffer, wiced_buffer_t next_buffer );
extern wiced_buffer_t host_buffer_get_queue_next( wiced_buffer_t buffer );

extern wwd_result_t host_buffer_set_queue_next( wiced_buffer_t buffer , wiced_buffer_t next );


/**
 * Checks for buffers that have been leaked
 *
 * Implemented in the WICED buffer interface which is specific to the
 * buffering scheme in use.
 * This function must only be used when all buffers are expected to have been
 * released. Function triggers an assertion if any buffers are in use.
 *
 * @return WWD_SUCCESS = Success, Error code = Failure
 */
extern wwd_result_t host_buffer_check_leaked( void );

/**
 * Init fifo
 *
 * @param fifo : pointer to fifo structure
 */
extern void host_buffer_init_fifo( wiced_buffer_fifo_t* fifo );

/**
 * Push buffer to tail of fifo
 *
 * @param fifo : pointer to fifo structure
 * @param buffer : buffer to push
 * @param interface : which interface buffer belong
 */
extern void host_buffer_push_to_fifo( wiced_buffer_fifo_t* fifo, wiced_buffer_t buffer, wwd_interface_t interface );

/**
 * Pop packet from head of fifo.
 *
 * @param fifo - pointer to fifo structure
 * @param interface - out parameter which filled with interface buffer belong
 *
 * @return NULL if fifo empty, otherwise return buffer and store interface in 'interface' out parameter.
 */
extern wiced_buffer_t host_buffer_pop_from_fifo( wiced_buffer_fifo_t* fifo, wwd_interface_t* interface );

/**
 * Check whether pools are full or not (whether all packets are freed).
 *
 * @param direction - TX or RX pools to check
 *
 * @return WICED_TRUE if polls are full, otherwise WICED_FALSE.
 */
wiced_bool_t host_buffer_pool_is_full( wwd_buffer_dir_t direction );

wwd_result_t host_buffer_add_application_defined_pool( void* pool_in, wwd_buffer_dir_t direction );


/** @} */

#ifdef __cplusplus
} /*extern "C" */
#endif
#endif /* ifndef INCLUDED_WWD_BUFFER_INTERFACE_H_ */
