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
#include <stdlib.h>
#include "wiced_utilities.h"
#include "rtos.h"
#include "RTOS/wwd_rtos_interface.h"
#include "platform/wwd_platform_interface.h"
#include "wwd_assert.h"
#include "wwd_debug.h"
#include "wiced_rtos.h"
#include "wiced_defaults.h"
#include "wiced_result.h"
#include "wiced_rtos_common.h"
#include "platform_config.h"
#include "wwd_rtos_interface.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define CPU_CYCLES_PER_MICROSECOND    ( CPU_CLOCK_HZ / 1000000 )

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    event_handler_t function;
    void* arg;
} wiced_event_message_t;

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static void timed_event_handler( void* arg );
static host_rtos_init_timer_t wiced_rtos_init_timer_wrapper;
static host_rtos_start_timer_t wiced_rtos_start_timer_wrapper;
static host_rtos_stop_timer_t wiced_rtos_stop_timer_wrapper;
static host_rtos_deinit_timer_t wiced_rtos_deinit_timer_wrapper;
static host_rtos_is_timer_running_t wiced_rtos_is_timer_running_wrapper;
static host_rtos_alloc_timer_t wiced_rtos_alloc_timer;
static host_rtos_free_timer_t wiced_rtos_free_timer;
static host_rtos_timer_ifc_t host_rtos_timer_ifc_local =
    {

    .host_rtos_init_timer   = &wiced_rtos_init_timer_wrapper,
    .host_rtos_deinit_timer = &wiced_rtos_deinit_timer_wrapper,

    .host_rtos_alloc_timer  = &wiced_rtos_alloc_timer,
    .host_rtos_free_timer   = &wiced_rtos_free_timer,

    .host_rtos_start_timer  = &wiced_rtos_start_timer_wrapper,
    .host_rtos_stop_timer   = &wiced_rtos_stop_timer_wrapper,

    .host_rtos_is_timer_running = &wiced_rtos_is_timer_running_wrapper

    };

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_result_t wiced_rtos_init( void )
{
    wiced_result_t result = WICED_SUCCESS;

    WPRINT_RTOS_INFO( ("Started " RTOS_NAME " " RTOS_VERSION "\n") );

    result = wiced_rtos_create_worker_thread( WICED_HARDWARE_IO_WORKER_THREAD, WICED_DEFAULT_WORKER_PRIORITY, HARDWARE_IO_WORKER_THREAD_STACK_SIZE, HARDWARE_IO_WORKER_THREAD_QUEUE_SIZE );
    if ( result != WICED_SUCCESS )
    {
        WPRINT_RTOS_ERROR( ("Failed to create WICED_HARDWARE_IO_WORKER_THREAD\n") );
        return result;
    }

    result = wiced_rtos_create_worker_thread( WICED_NETWORKING_WORKER_THREAD, WICED_NETWORK_WORKER_PRIORITY, NETWORKING_WORKER_THREAD_STACK_SIZE, NETWORKING_WORKER_THREAD_QUEUE_SIZE );
    if ( result != WICED_SUCCESS )
    {
        WPRINT_RTOS_ERROR( ("Failed to create WICED_NETWORKING_WORKER_THREAD\n") );
    }

    host_rtos_register_timer_ifc( &host_rtos_timer_ifc_local );

    return result;
}

wiced_result_t wiced_rtos_deinit( void )
{
    wiced_result_t result = wiced_rtos_delete_worker_thread( WICED_HARDWARE_IO_WORKER_THREAD );

    if ( result == WICED_SUCCESS )
    {
        result = wiced_rtos_delete_worker_thread( WICED_NETWORKING_WORKER_THREAD );
    }

    host_rtos_unregister_timer_ifc( host_rtos_timer_ifc );

    return result;
}

static int wiced_rtos_init_timer_wrapper( void* timer, uint32_t time_ms, host_rtos_timer_handler_t function, void* arg )
{
    return (int)wiced_rtos_init_timer( timer, time_ms, function, arg );
}

static int wiced_rtos_start_timer_wrapper( void* timer )
{
    return (int)wiced_rtos_start_timer( timer );
}

static int wiced_rtos_stop_timer_wrapper( void* timer )
{
    return (int)wiced_rtos_stop_timer( timer );
}

static int wiced_rtos_deinit_timer_wrapper( void* timer )
{
    return (int)wiced_rtos_deinit_timer( timer );
}

static int wiced_rtos_is_timer_running_wrapper( void* timer )
{
    return (int)wiced_rtos_is_timer_running( timer );
}

static int wiced_rtos_alloc_timer( void** timer )
{
    if ( NULL == timer )
    {
        wiced_assert("invalid arg passed to alloc timer\n", NULL != timer );
        goto error;
    }

    *timer = calloc( 1, sizeof( wiced_timer_t ) );

    if ( NULL == *timer )
    {
        goto error;
    }

    return (int)WICED_SUCCESS;
error:
    return (int)WICED_ERROR;
}

static int wiced_rtos_free_timer( void** timer )
{
    if ( NULL == timer )
    {
        wiced_assert("invalid arg passed to free timer\n", NULL != timer );
        goto error;
    }

    free( *timer );
    *timer = NULL;
    return (int)WICED_SUCCESS;
error:
    return (int)WICED_ERROR;
}

wiced_result_t wiced_rtos_thread_join( wiced_thread_t* thread )
{
    return (wiced_result_t) host_rtos_join_thread( WICED_GET_THREAD_HANDLE( thread ) );
}

wiced_result_t wiced_rtos_delay_milliseconds( uint32_t milliseconds )
{
    return (wiced_result_t) host_rtos_delay_milliseconds( milliseconds );
}

wiced_result_t wiced_rtos_delay_microseconds( uint32_t microseconds )
{
    uint32_t current_time;
    uint32_t duration;
    uint32_t elapsed_time = 0;

    current_time = host_platform_get_cycle_count( );
    duration     = ( microseconds * CPU_CYCLES_PER_MICROSECOND );
    while ( elapsed_time < duration )
    {
        elapsed_time = host_platform_get_cycle_count( ) - current_time;
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_init_semaphore( wiced_semaphore_t* semaphore )
{
    return (wiced_result_t) host_rtos_init_semaphore( (host_semaphore_type_t*) semaphore );
}

wiced_result_t wiced_rtos_set_semaphore( wiced_semaphore_t* semaphore )
{
    return (wiced_result_t) host_rtos_set_semaphore( (host_semaphore_type_t*) semaphore, host_platform_is_in_interrupt_context( ) );
}

wiced_result_t wiced_rtos_get_semaphore( wiced_semaphore_t* semaphore, uint32_t timeout_ms )
{
    return (wiced_result_t) host_rtos_get_semaphore( (host_semaphore_type_t*) semaphore, timeout_ms, WICED_FALSE );
}

wiced_result_t wiced_rtos_deinit_semaphore( wiced_semaphore_t* semaphore )
{
    return (wiced_result_t) host_rtos_deinit_semaphore( (host_semaphore_type_t*) semaphore );
}

wiced_result_t wiced_rtos_push_to_queue( wiced_queue_t* queue, void* message, uint32_t timeout_ms )
{
    return (wiced_result_t) host_rtos_push_to_queue( WICED_GET_QUEUE_HANDLE( queue ), message, timeout_ms );
}

wiced_result_t wiced_rtos_pop_from_queue( wiced_queue_t* queue, void* message, uint32_t timeout_ms )
{
    return (wiced_result_t) host_rtos_pop_from_queue( WICED_GET_QUEUE_HANDLE( queue ), message, timeout_ms );
}

static void worker_thread_main( wiced_thread_arg_t arg )
{
    wiced_worker_thread_t* worker_thread = (wiced_worker_thread_t*) arg;

    while ( 1 )
    {
        wiced_event_message_t message;

        if ( wiced_rtos_pop_from_queue( &worker_thread->event_queue, &message, WICED_WAIT_FOREVER ) == WICED_SUCCESS )
        {
            message.function( message.arg );
        }
    }
}

wiced_result_t wiced_rtos_create_worker_thread( wiced_worker_thread_t* worker_thread, uint8_t priority, uint32_t stack_size, uint32_t event_queue_size )
{
    memset( worker_thread, 0, sizeof( *worker_thread ) );

    if ( wiced_rtos_init_queue( &worker_thread->event_queue, "worker queue", sizeof(wiced_event_message_t), event_queue_size ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }

    if ( wiced_rtos_create_thread( &worker_thread->thread, priority , "worker thread", worker_thread_main, stack_size, (void*) worker_thread ) != WICED_SUCCESS )
    {
        wiced_rtos_deinit_queue( &worker_thread->event_queue );
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_delete_worker_thread( wiced_worker_thread_t* worker_thread )
{
    if ( wiced_rtos_delete_thread( &worker_thread->thread ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }

    if ( wiced_rtos_deinit_queue( &worker_thread->event_queue ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_register_timed_event( wiced_timed_event_t* event_object, wiced_worker_thread_t* worker_thread, event_handler_t function, uint32_t time_ms, void* arg )
{
    if ( wiced_rtos_init_timer( &event_object->timer, time_ms, timed_event_handler, (void*) event_object ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }

    event_object->function = function;
    event_object->thread = worker_thread;
    event_object->arg = arg;

    if ( wiced_rtos_start_timer( &event_object->timer ) != WICED_SUCCESS )
    {
        wiced_rtos_deinit_timer( &event_object->timer );
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_deregister_timed_event( wiced_timed_event_t* event_object )
{
    if ( wiced_rtos_deinit_timer( &event_object->timer ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }

    memset( event_object, 0, sizeof( *event_object ) );

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_send_asynchronous_event( wiced_worker_thread_t* worker_thread, event_handler_t function, void* arg )
{
    wiced_event_message_t message;

    message.function = function;
    message.arg = arg;

    return wiced_rtos_push_to_queue( &worker_thread->event_queue, &message, WICED_NO_WAIT );
}

static void timed_event_handler( void* arg )
{
    wiced_timed_event_t* event_object = (wiced_timed_event_t*) arg;
    wiced_event_message_t message;

    message.function = event_object->function;
    message.arg = event_object->arg;

    wiced_rtos_push_to_queue( &event_object->thread->event_queue, &message, WICED_NO_WAIT );
}

