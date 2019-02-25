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
 *  Defines functions to access functions provided by the RTOS
 *  in a generic way
 *
 */

#pragma once


/* Include the actual RTOS definitions for:
 * - wiced_timed_event_t
 * - timed_event_handler_t
 */
#include "rtos.h"
#include "wiced_result.h"
#include "RTOS/wwd_rtos_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *              Enumerations
 ******************************************************/

typedef enum
{
    WAIT_FOR_ANY_EVENT,
    WAIT_FOR_ALL_EVENTS,
} wiced_event_flags_wait_option_t;

/******************************************************
 *             Structures
 ******************************************************/

typedef wwd_thread_arg_t wiced_thread_arg_t;

typedef void (*wiced_thread_function_t)( wiced_thread_arg_t arg );

/******************************************************
 *             Function declarations
 *
 ******************************************************/

/*****************************************************************************/
/** @defgroup rtos       RTOS
 *
 *  WICED Real-Time Operating System Functions includes
 *   - WICED Thread management functions
 *   - WICED Semaphore management functions
 *   - WICED Mutex management functions
 *   - WICED Queue management functions
 *   - WICED Timer management functions
 *   - WICED Event management functions
 *
 */
/*****************************************************************************/


/*****************************************************************************/
/** @addtogroup threads       Threads
 *  @ingroup rtos
 *
 * Thread management functions. The following functions create a thread with a priority as defined below,
 * deletes a thread, cancel a thread , creates a thread with stack size and checks for the current thread etc.
 *
 *                  - WICED thread priority table
 *                         - +----------+-----------------+
 *                         - | Priority |      Thread     |
 *                         - |----------|-----------------|
 *                         - |     0    |      Wiced      |   Highest priority
 *                         - |     1    |     Network     |
 *                         - |     2    |                 |
 *                         - |     3    | Network worker  |
 *                         - |     4    |                 |
 *                         - |     5    | Default Library |
 *                         - |          | Default worker  |
 *                         - |     6    |                 |
 *                         - |     7    |   Application   |
 *                         - |     8    |                 |
 *                         - |     9    |      Idle       |   Lowest priority
 *                         - +----------+-----------------+
 *                         - WICED_NETWORK_WORKER_PRIORITY      (3)
 *                         - WICED_DEFAULT_WORKER_PRIORITY      (5)
 *                         - WICED_DEFAULT_LIBRARY_PRIORITY     (5)
 *                         - WICED_APPLICATION_PRIORITY         (7)
 *
 *  RTOS for example ThreadX is pre-emptive .i.e Highest priority task always runs first, and the lower priority
 *  task runs later, if lower priority task is holding a Mutex and if higher priority thread is accessing
 *  the resource held by lower priority Mutex, then priority of the lower thread is increased to be the same
 *  of highest priority ( priority inheritance) so that Mutex is released and acquired by Higher Priority Task
 *
 *  API's to be called from an ISR(Interrupt Service Routine)
 *  Currently there is no WICED API which is called from an ISR, there is a low level API wwd_thread_notify_irq( )
 *  which is called from an ISR, this is called by SDIO OOB (out of band) IRQ handler function for SDIO based
 *  chips such as 43438 and for 4390x it is platform_m2mdma_isr, which is registered in function
 *  platform_gpio_irq_enable( ) or PLATFORM_MAP_ISR() for 4390x
 *
 *  @{
 */
/*****************************************************************************/

/** Creates and starts a new thread with given priority, name and stack size.
 *
 * @param[out] thread     : Pointer to variable that will receive the thread handle
 * @param[in]  priority   : A priority number or WICED_APPLICATION_PRIORITY.
 *
 * @param[in]  name       : A text name for the thread (can be null)
 * @param[in]  function   : The main thread function
 * @param[in]  stack_size : Stack size for this thread
 * @param[in]  arg        : Argument which will be passed to thread function
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_create_thread( wiced_thread_t* thread, uint8_t priority, const char* name, wiced_thread_function_t function, uint32_t stack_size, void* arg );


/** Creates and starts a new thread with user provided stack
 *
 *
 * @param[out] thread     : Pointer to variable that will receive the thread handle
 * @param[in]  priority   : A priority number or WICED_APPLICATION_PRIORITY.
 * @param[in]  name       : A text name for the thread (can be null)
 * @param[in]  function   : The main thread function
 * @param[in]  stack      : The stack for this thread
 * @param[in]  stack_size : Stack size for this thread
 * @param[in]  arg        : Argument which will be passed to thread function
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_create_thread_with_stack( wiced_thread_t* thread, uint8_t priority, const char* name, wiced_thread_function_t function, void* stack, uint32_t stack_size, void* arg );


/** Deletes a terminated thread
 *
 * @param[in] thread     : The handle of the thread to delete
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_delete_thread( wiced_thread_t* thread );


/** Sleep for a given period of milliseconds
 *
 * Causes the current thread to sleep for AT LEAST the
 * specified number of milliseconds. If the processor is heavily loaded
 * with higher priority tasks, the delay may be much longer than requested.
 *
 * @param[in] milliseconds : The time to sleep in milliseconds
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_delay_milliseconds( uint32_t milliseconds );

/** Delay for a given period of microseconds
 *
 * Causes the current thread to block for AT LEAST the
 * specified number of microseconds. If the processor is heavily loaded
 * with higher priority tasks, the delay may be much longer than requested.
 *
 * NOTE: All threads with equal or lower priority than the current thread
 *       will not be able to run while the delay is occurring.
 *
 * @param[in] microseconds : The time to delay in microseconds
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_delay_microseconds( uint32_t microseconds );

/** Sleeps until another thread has terminated
 *
 * Causes the current thread to sleep until the specified other thread
 * has terminated. If the processor is heavily loaded
 * with higher priority tasks, this thread may not wake until significantly
 * after the thread termination.
 *
 * @param[in] thread : The handle of the other thread which will terminate
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_thread_join( wiced_thread_t* thread );


/** Forcibly wakes another thread
 *
 * Causes the specified thread to wake from suspension. This will usually
 * cause an error or timeout in that thread, since the task it was waiting on
 * is not complete.
 *
 * @param[in] thread : The handle of the other thread which will be woken
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_thread_force_awake( wiced_thread_t* thread );


/** Checks if a specified thread is the current running thread
 *
 *
 * @param[in] thread : The handle of the other thread against which the current thread will be compared
 *
 * @return    WICED_SUCCESS : specified thread is the current thread
 * @return    WICED_ERROR   : specified thread is not currently running
 */
wiced_result_t wiced_rtos_is_current_thread( wiced_thread_t* thread );


/** Checks the stack of the current thread
 *
 * @return    WICED_SUCCESS : if the current thread stack is within limits
 * @return    WICED_ERROR   : if the current thread stack has extended beyond its limits
 */
wiced_result_t wiced_rtos_check_stack( void );


/** Yield to higher priority thread
 *
 * @return WICED_SUCCESS : if the current running thread is suspended
 * @return WICED_ERROR   : if the current running thread is not suspended.
 *
 */
wiced_result_t wiced_rtos_thread_yield (void);

/** @} */
/*****************************************************************************/
/** @addtogroup semaphores       Semaphores
 *  @ingroup rtos
 *
 * Semaphore management functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Initializes a semaphore
 *
 *
 * @param[in] semaphore : A pointer to the semaphore handle to be initialized
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_init_semaphore( wiced_semaphore_t* semaphore );


/** Set (post/put/increment) a semaphore
 *
 *
 * @param[in] semaphore : A pointer to the semaphore handle to be set
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_set_semaphore( wiced_semaphore_t* semaphore );


/** Get (wait/decrement) a semaphore
 *
 * Attempts to get (wait/decrement) a semaphore. If semaphore is at zero already,
 * then the calling thread will be suspended until another thread sets the
 * semaphore with @ref wiced_rtos_set_semaphore
 *
 * @param[in] semaphore : A pointer to the semaphore handle
 * @param[in] timeout_ms: The number of milliseconds to wait before returning
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_get_semaphore( wiced_semaphore_t* semaphore, uint32_t timeout_ms );


/** De-initialize a semaphore
 *
 * Deletes a semaphore created with @ref wiced_rtos_init_semaphore
 *
 * @param[in] semaphore : A pointer to the semaphore handle
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_deinit_semaphore( wiced_semaphore_t* semaphore );

/** @} */
/*****************************************************************************/
/** @addtogroup mutexes       Mutexes
 *  @ingroup rtos
 *
 * Mutex management functions
 *
 * For RTOS (ThreadX) The Mutex are recursive in nature. If the calling thread held a Mutex lock
 * and re-acquires it, then the count(tx_mutex_ownership_count) is incremented.
 * The thread has to release the Mutex lock by unlocking as many times as the count value.
 *
 *  @{
 */
/*****************************************************************************/

/** Initializes a mutex
 *
 * A mutex is different from a semaphore in that a thread that already holds
 * the lock on the mutex can request the lock again (nested) without causing
 * it to be suspended.
 *
 * @param[in] mutex : A pointer to the mutex handle to be initialized
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_init_mutex( wiced_mutex_t* mutex );


/** Obtains the lock on a mutex
 *
 * Attempts to obtain the lock on a mutex. If the lock is already held
 * by another thread, the calling thread will be suspended until
 * the mutex lock is released by the other thread.
 *
 * @param[in] mutex : A pointer to the mutex handle to be locked
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_lock_mutex( wiced_mutex_t* mutex );


/** Releases the lock on a mutex
 *
 * Releases a currently held lock on a mutex. If another thread
 * is waiting on the mutex lock, then it will be resumed.
 *
 * @param[in] mutex : A pointer to the mutex handle to be unlocked
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_unlock_mutex( wiced_mutex_t* mutex );


/** De-initialize a mutex
 *
 * Deletes a mutex created with @ref wiced_rtos_init_mutex
 *
 * @param[in] mutex : A pointer to the mutex handle
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_deinit_mutex( wiced_mutex_t* mutex );


/** @} */
/*****************************************************************************/
/** @addtogroup queues       Queues
 *  @ingroup rtos
 *
 * Queue management functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Initializes a FIFO queue
 *
 *
 * @param[in] queue              : A pointer to the queue handle to be initialized
 * @param[in] name               : A text string name for the queue (NULL is allowed)
 * @param[in] message_size       : Size in bytes of objects that will be held in the queue
 * @param[in] number_of_messages : Depth of the queue - i.e. max number of objects in the queue
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_init_queue( wiced_queue_t* queue, const char* name, uint32_t message_size, uint32_t number_of_messages );


/** Pushes an object onto a queue
 *
 *
 * @param[in] queue      : A pointer to the queue handle
 * @param[in] message    : The object to be added to the queue. Size is assumed to be
 *                         the size specified in @ref wiced_rtos_init_queue
 * @param[in] timeout_ms : The number of milliseconds to wait before returning
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error or timeout occurred
 */
wiced_result_t wiced_rtos_push_to_queue( wiced_queue_t* queue, void* message, uint32_t timeout_ms );


/** Pops an object off a queue
 *
 * Pops an object off a queue
 *
 * @param[in] queue      : A pointer to the queue handle
 * @param[out] message   : Pointer to a buffer that will receive the object being
 *                         popped off the queue. Size is assumed to be
 *                         the size specified in @ref wiced_rtos_init_queue, hence
 *                         you must ensure the buffer is long enough or memory
 *                         corruption will result
 * @param[in] timeout_ms : The number of milliseconds to wait before returning
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error or timeout occurred
 */
wiced_result_t wiced_rtos_pop_from_queue( wiced_queue_t* queue, void* message, uint32_t timeout_ms );


/** De-initialize a queue
 *
 * Deletes a queue created with @ref wiced_rtos_init_queue
 *
 * @param[in] queue : A pointer to the queue handle
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_deinit_queue( wiced_queue_t* queue );


/** Check if a queue is empty
 *
 * @param[in] queue : A pointer to the queue handle
 *
 * @return    WICED_SUCCESS : queue is empty.
 * @return    WICED_ERROR   : queue is not empty.
 */
wiced_result_t wiced_rtos_is_queue_empty( wiced_queue_t* queue );


/** Check if a queue is full
 *
 * @param[in] queue : A pointer to the queue handle
 *
 * @return    WICED_SUCCESS : queue is full.
 * @return    WICED_ERROR   : queue is not full.
 */
wiced_result_t wiced_rtos_is_queue_full( wiced_queue_t* queue );


/** Get the queue occupancy
 *
 * @param[in]  queue : A pointer to the queue handle
 * @param[out] count : Pointer to integer for storing occupancy count
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_get_queue_occupancy( wiced_queue_t* queue, uint32_t *count );


/** @} */
/*****************************************************************************/
/** @addtogroup rtostmr       RTOS timers
 *  @ingroup rtos
 *
 * RTOS timer management functions
 * These timers are based on the RTOS time-slice scheduling, so are not
 * highly accurate. They are also affected by high loading on the processor.
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Initializes a RTOS timer
 *
 * Timer does not start running until @ref wiced_rtos_start_timer is called
 *
 * @param[in] timer    : A pointer to the timer handle to be initialized
 * @param[in] time_ms  : Timer period in milliseconds
 * @param[in] function : The callback handler function that is called each
 *                       time the timer expires
 * @param[in] arg      : An argument that will be passed to the callback
 *                       function
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_init_timer( wiced_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg );


/** Starts a RTOS timer
 *
 * Timer must have been previously
 * initialized with @ref wiced_rtos_init_timer
 *
 * @param[in] timer    : A pointer to the timer handle to start
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_start_timer( wiced_timer_t* timer );


/** Stops a running RTOS timer
 *
 * Timer must have been previously
 * started with @ref wiced_rtos_start_timer
 *
 * @param[in] timer    : A pointer to the timer handle to stop
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_stop_timer( wiced_timer_t* timer );


/** De-initialize a RTOS timer
 *
 * Deletes a RTOS timer created with @ref wiced_rtos_init_timer
 *
 * @param[in] timer : A pointer to the RTOS timer handle
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_deinit_timer( wiced_timer_t* timer );


/** Check if an RTOS timer is running
 *
 * @param[in] timer : A pointer to the RTOS timer handle
 *
 * @return    WICED_SUCCESS : if running.
 * @return    WICED_ERROR   : if not running
 */
wiced_result_t wiced_rtos_is_timer_running( wiced_timer_t* timer );

/** @} */
/*****************************************************************************/
/** @addtogroup worker       Worker Threads
 *  @ingroup rtos
 *
 * Worker thread management functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Creates a worker thread
 *
 * A worker thread is a thread in whose context timed and asynchronous events
 * execute.
 *
 * @param[in] worker_thread    : A pointer to the worker thread to be created
 * @param[in] priority         : Thread priority
 * @param[in] stack_size       : Thread's stack size in number of bytes
 * @param[in] event_queue_size : Number of events can be pushed into the queue
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_create_worker_thread( wiced_worker_thread_t* worker_thread, uint8_t priority, uint32_t stack_size, uint32_t event_queue_size );


/** Deletes a worker thread
 *
 *
 * @param[in] worker_thread : A pointer to the worker thread to be created
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_delete_worker_thread( wiced_worker_thread_t* worker_thread );

/** @} */
/*****************************************************************************/
/** @addtogroup events       Events
 *  @ingroup rtos
 *
 * Event management functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Requests a function be called at a regular interval
 *
 * This function registers a function that will be called at a regular
 * interval. Since this is based on the RTOS time-slice scheduling, the
 * accuracy is not high, and is affected by processor load.
 *
 * @param[in] event_object  : Pointer to a event handle which will be initialized
 * @param[in] worker_thread : Pointer to the worker thread in whose context the
 *                            callback function runs on
 * @param[in] function      : The callback function that is to be called regularly
 * @param[in] time_ms       : The time period between function calls in milliseconds
 * @param[in] arg           : An argument that will be supplied to the function when
 *                            it is called
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_register_timed_event( wiced_timed_event_t* event_object, wiced_worker_thread_t* worker_thread, event_handler_t function, uint32_t time_ms, void* arg );


/** Removes a request for function to be called at regular interval.
 *
 * This function De-register's a function that has previously been set-up
 * with @ref wiced_rtos_register_timed_event.
 *
 * @param[in] event_object : The event handle used with @ref wiced_rtos_register_timed_event
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_deregister_timed_event( wiced_timed_event_t* event_object );


/** Sends an asynchronous event to the associated worker thread
 *
 *
 * @param[in] worker_thread : The worker thread in which context the callback should execute from
 * @param[in] function      : The callback function to be called from the worker thread
 * @param[in] arg           : The argument to be passed to the callback function
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_send_asynchronous_event( wiced_worker_thread_t* worker_thread, event_handler_t function, void* arg );

/** @} */
/*****************************************************************************/
/** @addtogroup eventflags       Event Flags
 *  @ingroup rtos
 *
 * Event flags management functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Initialize an event flags
 *
 * @param[in] event_flags : A pointer to the event flags handle
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_init_event_flags( wiced_event_flags_t* event_flags );


/** Wait for event flags to be set
 *
 * @param[in]  event_flags        : Pointer to the event flags handle
 * @param[in]  flags_to_wait_for  : Group of event flags (ORed bit-fields) to wait for
 * @param[out] flags_set          : Event flag(s) set
 * @param[in]  clear_set_flags    : TRUE to clear set flag, FALSE leaves flags unchanged.
 * @param[in]  wait_option        : Wait option
 * @param[in]  timeout_ms         : Timeout in milliseconds
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_wait_for_event_flags( wiced_event_flags_t* event_flags, uint32_t flags_to_wait_for, uint32_t* flags_set, wiced_bool_t clear_set_flags, wiced_event_flags_wait_option_t wait_option, uint32_t timeout_ms );


/** Set event flags
 *
 * @param[in] event_flags  : Pointer to the event flags handle
 * @param[in] flags_to_set : Group of event flags (ORed bit-fields) to set
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_set_event_flags( wiced_event_flags_t* event_flags, uint32_t flags_to_set );


/** De-initialize an event flags
 *
 * @param[in] event_flags : Pointer to the event flags handle
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_deinit_event_flags( wiced_event_flags_t* event_flags );

/** @} */


#ifdef __cplusplus
} /*extern "C" */
#endif
