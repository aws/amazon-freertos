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
 *  Defines the WICED RTOS Interface.
 *
 *  Provides prototypes for functions that allow WICED to use RTOS resources
 *  such as threads, semaphores & timing functions in an abstract way.
 */

#ifndef INCLUDED_WWD_RTOS_INTERFACE_H_
#define INCLUDED_WWD_RTOS_INTERFACE_H_

#include <stdint.h>
#include "wwd_rtos.h"
#include "wwd_structures.h"

#ifndef RTOS_HIGHER_PRIORTIY_THAN
#error wwd_rtos.h must define RTOS_HIGHER_PRIORTIY_THAN(x)
#endif
#ifndef RTOS_LOWER_PRIORTIY_THAN
#error wwd_rtos.h must define RTOS_LOWER_PRIORTIY_THAN(x)
#endif
#ifndef RTOS_LOWEST_PRIORITY
#error wwd_rtos.h must define RTOS_LOWEST_PRIORITY
#endif
#ifndef RTOS_HIGHEST_PRIORITY
#error wwd_rtos.h must define RTOS_HIGHEST_PRIORITY
#endif
#ifndef RTOS_DEFAULT_THREAD_PRIORITY
#error wwd_rtos.h must define RTOS_DEFAULT_THREAD_PRIORITY
#endif
#if !(defined(RTOS_USE_DYNAMIC_THREAD_STACK) || defined(RTOS_USE_STATIC_THREAD_STACK))
#error wwd_rtos.h must define either RTOS_USE_DYNAMIC_THREAD_STACK or RTOS_USE_STATIC_THREAD_STACK
#endif

#ifdef __x86_64__
typedef uint64_t wwd_thread_arg_t;
#else
typedef uint32_t wwd_thread_arg_t;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 * @cond       Constants
 ******************************************************/

#define NEVER_TIMEOUT ((uint32_t)0xffffffffUL)  /* Used with host_rtos_get_semaphore */

/******************************************************
 *             Structures & Typedefs
 ******************************************************/

/** @endcond */


/** @addtogroup rtosif RTOS Interface
 * Allows WICED to use use RTOS resources
 *  such as threads, semaphores & timing functions in an abstract way.
 *  @{
 */

/******************************************************
 *             Function declarations
 ******************************************************/

/* Thread management functions */

/**
 * Create a thread
 *
 * Implemented in the WICED RTOS interface which is specific to the
 * RTOS in use.
 * WICED uses this function to create a new thread and start it running.
 *
 * @param thread         : pointer to a variable which will receive the new thread handle
 * @param entry_function : function pointer which points to the main function for the new thread
 * @param name           : a string thread name used for a debugger
 * @param stack_size     : the size of the thread stack in bytes
 * @param priority       : the priority of the thread
 *
 * @return WWD_SUCCESS or Error code
 */
extern wwd_result_t host_rtos_create_thread( /*@out@*/ host_thread_type_t* thread, void(*entry_function)( wwd_thread_arg_t arg ), const char* name, /*@null@*/ void* stack, uint32_t stack_size, uint32_t priority ) /*@modifies *thread@*/;


/**
 * Create a thread with specific thread argument
 *
 * Implemented in the WICED RTOS interface which is specific to the
 * RTOS in use.
 *
 * @param thread         : pointer to a variable which will receive the new thread handle
 * @param entry_function : function pointer which points to the main function for the new thread
 * @param name           : a string thread name used for a debugger
 * @param stack_size     : the size of the thread stack in bytes
 * @param priority       : the priority of the thread
 * @param arg            : the argument to pass to the new thread
 *
 * @return WWD result code
 */
extern wwd_result_t host_rtos_create_thread_with_arg( /*@out@*/ host_thread_type_t* thread, void(*entry_function)( wwd_thread_arg_t arg ), const char* name, /*@null@*/ void* stack, uint32_t stack_size, uint32_t priority, wwd_thread_arg_t arg );


/**
 * Note: different RTOS have different parameters for creating threads.
 * Use this function carefully if portability is important.
 * Create a thread with RTOS specific thread argument (E.g. specify time-slicing behavior)
 *
 * Implemented in the WICED RTOS interface which is specific to the
 * RTOS in use.
 *
 * @param thread         : pointer to a variable which will receive the new thread handle
 * @param entry_function : function pointer which points to the main function for the new thread
 * @param name           : a string thread name used for a debugger
 * @param stack_size     : the size of the thread stack in bytes
 * @param priority       : the priority of the thread
 * @param arg            : the argument to pass to the new thread
 * @param config        : os specific thread configuration
 * @return WWD result code
 */
extern wwd_result_t host_rtos_create_configed_thread(  /*@out@*/ host_thread_type_t* thread, void(*entry_function)( uint32_t ), const char* name, /*@null@*/ void* stack, uint32_t stack_size, uint32_t priority, host_rtos_thread_config_type_t *config );


/**
 * Exit a thread
 *
 * Implemented in the WICED RTOS interface which is specific to the
 * RTOS in use.
 * WICED uses this function to exit the current thread just before its main
 * function would otherwise return. Some RTOSs allow threads to exit by simply returning
 * from their main function. If this is the case, then the implementation of
 * this function should be empty.
 *
 * @param thread         : Pointer to the current thread handle
 *
 * @return WWD_SUCCESS or Error code
 */
extern wwd_result_t host_rtos_finish_thread( host_thread_type_t* thread ) /*@modifies *thread@*/;


/**
 * Deletes a terminated thread
 *
 * Implemented in the WICED RTOS interface which is specific to the
 * RTOS in use.
 * Some RTOS implementations require that another thread deletes any terminated thread
 * If RTOS does not require this, leave empty
 *
 * @param thread         : handle of the terminated thread to delete
 *
 * @returns WWD_SUCCESS on success, Error code otherwise
 */
extern wwd_result_t host_rtos_delete_terminated_thread( host_thread_type_t* thread );


/**
 * Waits for a thread to complete
 *
 * Implemented in the WICED RTOS interface which is specific to the
 * RTOS in use.
 *
 * @param thread         : handle of the thread to wait for
 *
 * @returns WWD_SUCCESS on success, Error code otherwise
 */
extern wwd_result_t host_rtos_join_thread( host_thread_type_t* thread );

/* Semaphore management functions */

/**
 * Create a Semaphore
 *
 * Implemented in the WICED RTOS interface which is specific to the
 * RTOS in use.
 * WICED uses this function to create a semaphore
 *
 * @param semaphore         : Pointer to the semaphore handle to be initialized
 *
 * @return WWD_SUCCESS or Error code
 */
extern wwd_result_t host_rtos_init_semaphore(  /*@special@*/ /*@out@*/ host_semaphore_type_t* semaphore ) /*@allocates *semaphore@*/  /*@defines **semaphore@*/;

/**
 * Get a semaphore
 *
 * Implemented in the WICED RTOS interface which is specific to the
 * RTOS in use.
 * WICED uses this function to get a semaphore.
 * - If the semaphore value is greater than zero, the sempahore value is decremented and the function returns immediately.
 * - If the semaphore value is zero, the current thread is put on a queue of threads waiting on the
 *   semaphore, and sleeps until another thread sets the semaphore and causes it to wake. Upon waking, the
 *   semaphore is decremented and the function returns.
 *
 * @note : This function must not be called from an interrupt context as it may block.
 *
 * @param semaphore   : Pointer to the semaphore handle
 * @param timeout_ms  : Maximum number of milliseconds to wait while attempting to get
 *                      the semaphore. Use the NEVER_TIMEOUT constant to wait forever.
 * @param will_set_in_isr : True if the semaphore will be set in an ISR. Currently only used for NoOS/NoNS
 *
 * @return wwd_result_t : WWD_SUCCESS if semaphore was successfully acquired
 *                     : WICED_TIMEOUT if semaphore was not acquired before timeout_ms period
 */
extern wwd_result_t host_rtos_get_semaphore( host_semaphore_type_t* semaphore, uint32_t timeout_ms, wiced_bool_t will_set_in_isr ) /*@modifies internalState@*/;

/**
 * Set a semaphore
 *
 * Implemented in the WICED RTOS interface which is specific to the
 * RTOS in use.
 * WICED uses this function to set a semaphore.
 * The value of the semaphore is incremented, and if there are any threads waiting on the semaphore,
 * then the first waiting thread is woken.
 *
 * Some RTOS implementations require different processing when setting a semaphore from within an ISR.
 * A parameter is provided to allow this.
 *
 * @param semaphore       : Pointer to the semaphore handle
 * @param called_from_ISR : Value of WICED_TRUE indicates calling from interrupt context
 *                          Value of WICED_FALSE indicates calling from normal thread context
 *
 * @return wwd_result_t : WWD_SUCCESS if semaphore was successfully set
 *                        : Error code if an error occurred
 *
 */
extern wwd_result_t host_rtos_set_semaphore( host_semaphore_type_t* semaphore, wiced_bool_t called_from_ISR );

/**
 * Deletes a semaphore
 *
 * Implemented in the WICED RTOS interface which is specific to the
 * RTOS in use.
 * WICED uses this function to delete a semaphore.
 *
 * @param semaphore         : Pointer to the semaphore handle
 *
 * @return wwd_result_t : WWD_SUCCESS if semaphore was successfully deleted
 *                        : Error code if an error occurred
 */
extern wwd_result_t host_rtos_deinit_semaphore( /*@special@*/ host_semaphore_type_t* semaphore ) /*@releases *semaphore@*/;


/* Time management functions */

/**
 * Gets time in milliseconds since RTOS start
 *
 * Implemented in the WICED RTOS interface which is specific to the
 * RTOS in use.
 * WICED uses this function to retrieve the current time.
 *
 * @note: Since this is only 32 bits, it will roll over every 49 days, 17 hours, 2 mins, 47.296 seconds
 *
 * @returns Time in milliseconds since the RTOS started.
 */
extern wwd_time_t host_rtos_get_time( void )  /*@modifies internalState@*/;

/**
 * Delay for a number of milliseconds
 *
 * Implemented in the WICED RTOS interface which is specific to the
 * RTOS in use.
 * WICED uses this function to delay processing
 * Processing of this function depends on the minimum sleep
 * time resolution of the RTOS.
 * The current thread should sleep for the longest period possible which
 * is less than the delay required, then makes up the difference
 * with a tight loop
 *
 * @return wwd_result_t : WWD_SUCCESS if delay was successful
 *                        : Error code if an error occurred
 *
 */
extern wwd_result_t host_rtos_delay_milliseconds( uint32_t num_ms );


/* Message queue management functions */

extern wwd_result_t host_rtos_init_queue( /*@special@*/ /*@out@*/ host_queue_type_t* queue, void* buffer, uint32_t buffer_size, uint32_t message_size ) /*@allocates *queue@*/  /*@defines **queue@*/;

extern wwd_result_t host_rtos_push_to_queue( host_queue_type_t* queue, void* message, uint32_t timeout_ms );

extern wwd_result_t host_rtos_pop_from_queue( host_queue_type_t* queue, void* message, uint32_t timeout_ms );

extern wwd_result_t host_rtos_deinit_queue( /*@special@*/host_queue_type_t* queue ) /*@releases *queue@*/;

extern wwd_result_t host_rtos_init_mutex( host_mutex_type_t* mutex );
extern wwd_result_t host_rtos_lock_mutex( host_mutex_type_t* mutex );
extern wwd_result_t host_rtos_unlock_mutex( host_mutex_type_t* mutex );
extern wwd_result_t host_rtos_deinit_mutex( host_mutex_type_t* mutex );


unsigned long host_rtos_get_tickrate( void );

/* Support timers for WWD */
typedef void (host_rtos_timer_handler_t)( void * );
typedef int (host_rtos_init_timer_t)( void* timer, uint32_t time_ms, host_rtos_timer_handler_t function, void* arg );
typedef int (host_rtos_start_timer_t)( void* timer );
typedef int (host_rtos_stop_timer_t)( void* timer );
typedef int (host_rtos_deinit_timer_t)( void* timer );
typedef int (host_rtos_is_timer_running_t)( void* timer );
typedef int (host_rtos_alloc_timer_t)( void** timer );
typedef int (host_rtos_free_timer_t)( void** timer );

typedef struct
{
    /* order of operations should be: first allocate, then init, then start/stop/is_running any number of times, then deinit, then free */
    host_rtos_alloc_timer_t      *host_rtos_alloc_timer;

    /* These functions behave identically to the wiced_rtos.h functions */
    host_rtos_init_timer_t       *host_rtos_init_timer;

    host_rtos_start_timer_t      *host_rtos_start_timer;
    host_rtos_stop_timer_t       *host_rtos_stop_timer;
    host_rtos_is_timer_running_t *host_rtos_is_timer_running;

    host_rtos_deinit_timer_t     *host_rtos_deinit_timer;

    /* call after successful deinit */
    host_rtos_free_timer_t       *host_rtos_free_timer;
} host_rtos_timer_ifc_t;

extern const host_rtos_timer_ifc_t *host_rtos_timer_ifc;

/**
 * Register a timer interface for WWD to use.
 *
 * @return wwd_result_t : WWD_SUCCESS if successful
 *                        : Error code if an error occurred
 *
 */
extern wwd_result_t host_rtos_register_timer_ifc( const host_rtos_timer_ifc_t *host_rtos_timer_ifc );

/**
 * Unregister a timer interface WWD was using.
 *
 * @return wwd_result_t : WWD_SUCCESS if successful
 *                        : Error code if an error occurred
 *
 */
extern wwd_result_t host_rtos_unregister_timer_ifc( const host_rtos_timer_ifc_t *host_rtos_timer_ifc );

#define HOST_RTOS_TIMER_IFC host_rtos_timer_ifc
#define HOST_RTOS_TIMER_IFC_CALL(func) ( *(HOST_RTOS_TIMER_IFC->func) )

/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* ifndef INCLUDED_WWD_RTOS_INTERFACE_H_ */
