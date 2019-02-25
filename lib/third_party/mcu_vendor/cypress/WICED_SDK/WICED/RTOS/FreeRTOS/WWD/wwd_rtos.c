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
 *  Implementation of wiced_rtos.c for FreeRTOS
 *
 *  This is the FreeRTOS implementation of the Wiced RTOS
 *  abstraction layer.
 *  It provides Wiced with standard ways of using threads,
 *  semaphores and time functions
 *
 */

#include "wwd_rtos.h"
#include <stdint.h>
#include "wwd_constants.h"
#include "wwd_assert.h"
#include "RTOS/wwd_rtos_interface.h"
#include "platform/wwd_platform_interface.h"    /* host_platform_is_in_interrupt_context */
//#include "StackMacros.h"
#include "stack_macros.h"
#include "wiced_utilities.h"

/******************************************************
 *             Constants
 ******************************************************/

#define     WWD_THREAD_PRIORITY           ((unsigned long)configMAX_PRIORITIES-1)  /** in FreeRTOS, a higher number is a higher priority */


const uint32_t  ms_to_tick_ratio = (uint32_t)( 1000 / configTICK_RATE_HZ );

extern void vApplicationStackOverflowHook( TaskHandle_t *pxTask, signed portCHAR *pcTaskName );
extern signed portBASE_TYPE xTaskIsTaskFinished( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;
extern TaskHandle_t xTaskGetCurrentThread( void ) PRIVILEGED_FUNCTION;

/******************************************************
 *             Function definitions
 ******************************************************/

/**
 * Creates a new thread
 *
 * @param thread         : pointer to variable which will receive handle of created thread
 * @param entry_function : main thread function
 * @param name           : a string thread name used for a debugger
 *
 * @returns WWD_SUCCESS on success, WICED_ERROR otherwise
 */
wwd_result_t host_rtos_create_thread( /*@out@*/ host_thread_type_t* thread, void(*entry_function)( uint32_t ), const char* name, /*@null@*/ void* stack, uint32_t stack_size, uint32_t priority )
{
    return host_rtos_create_thread_with_arg( thread, entry_function, name, stack, stack_size, priority, 0 );
}

wwd_result_t host_rtos_create_thread_with_arg( /*@out@*/ host_thread_type_t* thread, void(*entry_function)( uint32_t ), const char* name, /*@null@*/ void* stack, uint32_t stack_size, uint32_t priority, uint32_t arg )
{
    if ( stack )
    {
        uint32_t tcb_size, adjusted_stack_size;
        void *tcb_buffer, *stack_buffer;

        /* FreeRTOS requires a TCB buffer as well. Allocate this space from the start of the buffer passed in. */
        tcb_size            = (sizeof( StaticTask_t ) + portBYTE_ALIGNMENT - 1U) & ~(portBYTE_ALIGNMENT-1U);
        tcb_buffer          = (void*)(((uint32_t)stack + portBYTE_ALIGNMENT - 1) & ~(uint32_t)(portBYTE_ALIGNMENT-1));
        stack_buffer        = (uint8_t*)tcb_buffer + tcb_size;
        adjusted_stack_size = (uint32_t)stack + stack_size - (uint32_t)stack_buffer;

        *thread = xTaskCreateStatic( (TaskFunction_t)entry_function, name, (unsigned short)(adjusted_stack_size / sizeof( portSTACK_TYPE )), (void*)arg, (unsigned portBASE_TYPE) priority, stack_buffer, tcb_buffer );

        return ( *thread == NULL ) ? WWD_THREAD_CREATE_FAILED : WWD_SUCCESS;
    }
    else
    {
        signed portBASE_TYPE result;

        result = xTaskCreate( (TaskFunction_t)entry_function, name, (unsigned short)(stack_size / sizeof( portSTACK_TYPE )), (void*)arg, (unsigned portBASE_TYPE) priority, thread );

        return ( result == (signed portBASE_TYPE) pdPASS ) ? WWD_SUCCESS : WWD_THREAD_CREATE_FAILED;
    }
}

wwd_result_t host_rtos_create_configed_thread(  /*@out@*/ host_thread_type_t* thread, void(*entry_function)( uint32_t ), const char* name, /*@null@*/ void* stack, uint32_t stack_size, uint32_t priority, host_rtos_thread_config_type_t *config )
{
    UNUSED_PARAMETER( config );
    return host_rtos_create_thread( thread, entry_function, name, stack, stack_size, priority );
}

/**
 * Terminates the current thread
 *
 * @param thread         : handle of the thread to terminate
 *
 * @returns WWD_SUCCESS on success, WICED_ERROR otherwise
 */
wwd_result_t host_rtos_finish_thread( host_thread_type_t* thread )
{
    malloc_leak_check(*thread, LEAK_CHECK_THREAD);
    vTaskDelete( *thread );

    return WWD_SUCCESS;
}


/**
 * Blocks the current thread until the indicated thread is complete
 *
 * @param thread         : handle of the thread to terminate
 *
 * @returns WWD_SUCCESS on success, WICED_ERROR otherwise
 */
wwd_result_t host_rtos_join_thread( host_thread_type_t* thread )
{
    /*@-infloopsuncon@*/ /* Not an infinite loop - signalled by other thread */
    while ( xTaskIsTaskFinished( *thread ) != pdTRUE )
    {
        (void) host_rtos_delay_milliseconds( (uint32_t) 10 );
    }
    /*@+infloopsuncon@*/
    return WWD_SUCCESS;
}

/**
 * Deletes a terminated thread
 *
 * FreeRTOS does not require that another thread deletes any terminated thread
 *
 * @param thread         : handle of the terminated thread to delete
 *
 * @returns WWD_SUCCESS on success, WICED_ERROR otherwise
 */
wwd_result_t host_rtos_delete_terminated_thread( host_thread_type_t* thread )
{
    UNUSED_PARAMETER( thread );

    /* The kernel takes care of this */
    return WWD_SUCCESS;
}


/**
 * Creates a semaphore
 *
 * In FreeRTOS a semaphore is represented with a counting semaphore
 *
 * @param semaphore         : pointer to variable which will receive handle of created semaphore
 *
 * @returns WWD_SUCCESS on success, WICED_ERROR otherwise
 */
wwd_result_t host_rtos_init_semaphore(  /*@special@*/ /*@out@*/ host_semaphore_type_t* semaphore ) /*@allocates *semaphore@*/  /*@defines **semaphore@*/
{
    *semaphore = xSemaphoreCreateCounting( (unsigned portBASE_TYPE) 0x7fffffff, (unsigned portBASE_TYPE) 0 );

    /*@-compdef@*/ /* Lint doesnt realise allocation has occurred */
    return ( *semaphore != NULL ) ? WWD_SUCCESS : WWD_SEMAPHORE_ERROR;
    /*@+compdef@*/
}


/**
 * Gets a semaphore
 *
 * If value of semaphore is larger than zero, then the semaphore is decremented and function returns
 * Else If value of semaphore is zero, then current thread is suspended until semaphore is set.
 * Value of semaphore should never be below zero
 *
 * Must not be called from interrupt context, since it could block, and since an interrupt is not a
 * normal thread, so could cause RTOS problems if it tries to suspend it.
 *
 * @param semaphore       : Pointer to variable which will receive handle of created semaphore
 * @param timeout_ms      : Maximum period to block for. Can be passed NEVER_TIMEOUT to request no timeout
 * @param will_set_in_isr : True if the semaphore will be set in an ISR. Currently only used for NoOS/NoNS
 *
 */

wwd_result_t host_rtos_get_semaphore( host_semaphore_type_t* semaphore, uint32_t timeout_ms, /*@unused@*/ wiced_bool_t will_set_in_isr ) /*@modifies internalState@*/
{
    UNUSED_PARAMETER( will_set_in_isr );

    if ( pdTRUE == xSemaphoreTake( *semaphore, (TickType_t) ( timeout_ms * ( 1000/configTICK_RATE_HZ ) ) ) )
    {
        return WWD_SUCCESS;
    }
    else
    {
        return WWD_TIMEOUT;
    }
}


/**
 * Sets a semaphore
 *
 * If any threads are waiting on the semaphore, the first thread is resumed
 * Else increment semaphore.
 *
 * Can be called from interrupt context, so must be able to handle resuming other
 * threads from interrupt context.
 *
 * @param semaphore       : Pointer to variable which will receive handle of created semaphore
 * @param called_from_ISR : Value of WICED_TRUE indicates calling from interrupt context
 *                          Value of WICED_FALSE indicates calling from normal thread context
 *
 * @return wwd_result_t : WWD_SUCCESS if semaphore was successfully set
 *                        : WICED_ERROR if an error occurred
 *
 */

wwd_result_t host_rtos_set_semaphore( host_semaphore_type_t* semaphore, wiced_bool_t called_from_ISR )
{
    signed portBASE_TYPE result;

    if ( called_from_ISR == WICED_TRUE )
    {
        signed portBASE_TYPE xHigherPriorityTaskWoken;
        result = xSemaphoreGiveFromISR( *semaphore, &xHigherPriorityTaskWoken );

        wiced_assert( "Unable to set semaphore", result == pdTRUE );

        /* If xSemaphoreGiveFromISR() unblocked a task, and the unblocked task has
         * a higher priority than the currently executing task, then
         * xHigherPriorityTaskWoken will have been set to pdTRUE and this ISR should
         * return directly to the higher priority unblocked task.
         */
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    }
    else
    {
        result = xSemaphoreGive( *semaphore );
        wiced_assert( "Unable to set semaphore", result == pdTRUE );
    }

    return ( result == pdPASS )? WWD_SUCCESS : WWD_SEMAPHORE_ERROR;
}


/**
 * Deletes a semaphore
 *
 * WICED uses this function to delete a semaphore.
 *
 * @param semaphore         : Pointer to the semaphore handle
 *
 * @return wwd_result_t : WWD_SUCCESS if semaphore was successfully deleted
 *                        : WICED_ERROR if an error occurred
 *
 */
wwd_result_t host_rtos_deinit_semaphore( /*@special@*/ host_semaphore_type_t* semaphore ) /*@releases *semaphore@*/
{
    if (semaphore != NULL)
    {
        vQueueDelete( *semaphore );
        *semaphore = NULL;
    }
    return WWD_SUCCESS;
}


/**
 * Gets time in milliseconds since RTOS start
 *
 * @Note: since this is only 32 bits, it will roll over every 49 days, 17 hours.
 *
 * @returns Time in milliseconds since RTOS started.
 */
wwd_time_t host_rtos_get_time( void )  /*@modifies internalState@*/
{
    return (wwd_time_t) ( xTaskGetTickCount( ) * ( 1000 / configTICK_RATE_HZ ) );
}


/**
 * Delay for a number of milliseconds
 *
 * Processing of this function depends on the minimum sleep
 * time resolution of the RTOS.
 * The current thread sleeps for the longest period possible which
 * is less than the delay required, then makes up the difference
 * with a tight loop
 *
 * @return wwd_result_t : WWD_SUCCESS if delay was successful
 *                        : WICED_ERROR if an error occurred
 *
 */
wwd_result_t host_rtos_delay_milliseconds( uint32_t num_ms )
{
    uint32_t remainder;

    if ( ( num_ms / ( 1000 / configTICK_RATE_HZ ) ) != 0 )
    {
        vTaskDelay( (TickType_t) ( num_ms / ( 1000 / configTICK_RATE_HZ ) ) );
    }

    remainder = num_ms % ( 1000 / configTICK_RATE_HZ );

    if ( remainder != 0 )
    {
        volatile uint32_t clock_in_kilohertz = (uint32_t) ( configCPU_CLOCK_HZ / 1000 );
        for ( ; clock_in_kilohertz != 0; clock_in_kilohertz-- )
        {
            volatile uint32_t tmp_ms = remainder;
            for ( ; tmp_ms != 0; tmp_ms-- )
            {
                /* do nothing */
            }
        }
    }
    return WWD_SUCCESS;

}

wwd_result_t host_rtos_init_mutex( host_mutex_type_t* mutex )
{
    UNUSED_VARIABLE( mutex );
    return WWD_UNSUPPORTED;
}

wwd_result_t host_rtos_lock_mutex( host_mutex_type_t* mutex )
{
    UNUSED_VARIABLE( mutex );
    return WWD_UNSUPPORTED;
}

wwd_result_t host_rtos_unlock_mutex( host_mutex_type_t* mutex )
{
    UNUSED_VARIABLE( mutex );
    return WWD_UNSUPPORTED;
}

wwd_result_t host_rtos_deinit_mutex( host_mutex_type_t* mutex )
{
    UNUSED_VARIABLE( mutex );
    return WWD_UNSUPPORTED;
}
#ifndef WICED_AMAZON_FREERTOS_SDK
void vApplicationStackOverflowHook( TaskHandle_t *pxTask, signed portCHAR *pcTaskName )
{
    UNUSED_PARAMETER( pxTask );
    UNUSED_PARAMETER( pcTaskName ); /* unused parameter in release build */

    wiced_assert("Stack Overflow Detected", 0 != 0);
}

void vApplicationMallocFailedHook( void )
{
    WPRINT_RTOS_DEBUG(("Heap is out of memory during malloc\n"));
    wiced_assert("Malloc failed!", 0 != 0);
}

#endif
wwd_result_t host_rtos_init_queue( /*@special@*/ /*@out@*/ host_queue_type_t* queue, void* buffer, uint32_t buffer_size, uint32_t message_size ) /*@allocates *queue@*/  /*@defines **queue@*/
{
    UNUSED_PARAMETER(buffer);
    if ( ( *queue = xQueueCreate( (unsigned portBASE_TYPE) ( buffer_size / message_size ), (unsigned portBASE_TYPE) message_size ) ) == NULL )
    {
        /*@-compdef@*/ /* Creation failed - no allocation needed*/
        return WWD_QUEUE_ERROR;
        /*@+compdef@*/
    }

    /*@-compdef@*/ /* Lint doesnt realise allocation has occurred */
    return WWD_SUCCESS;
    /*@+compdef@*/
}


wwd_result_t host_rtos_push_to_queue( host_queue_type_t* queue, void* message, uint32_t timeout_ms )
{
    signed portBASE_TYPE retval;

    if (host_platform_is_in_interrupt_context() == WICED_TRUE)
    {   /* in Interrupt */
        signed portBASE_TYPE xHigherPriorityTaskWoken;

        retval = xQueueSendToBackFromISR( *queue, message, &xHigherPriorityTaskWoken );
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    }
    else
    {   /* in Task */
        retval = xQueueSendToBack( *queue, message, (TickType_t) ( timeout_ms / ms_to_tick_ratio ) );
    }

    if ( retval != pdPASS )
    {
        return WWD_QUEUE_ERROR;
    }

    return WWD_SUCCESS;
}


wwd_result_t host_rtos_pop_from_queue( host_queue_type_t* queue, void* message, uint32_t timeout_ms )
{
    signed portBASE_TYPE retval;

    if (host_platform_is_in_interrupt_context() == WICED_TRUE)
    {   /* in Interrupt */
        signed portBASE_TYPE xHigherPriorityTaskWoken = pdTRUE;

        retval = xQueueReceiveFromISR( *queue, message, &xHigherPriorityTaskWoken );
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    }
    else
    {   /* in Task */
        retval = xQueueReceive( *queue, message, (TickType_t) ( timeout_ms / ms_to_tick_ratio ) );
    }

    if ( retval == errQUEUE_EMPTY )
    {
        return WWD_TIMEOUT;
    }
    else if ( retval != pdPASS )
    {
        return WWD_QUEUE_ERROR;
    }
    return WWD_SUCCESS;
}

wwd_result_t host_rtos_deinit_queue( /*@special@*/host_queue_type_t* queue ) /*@releases *queue@*/
{
    vQueueDelete( *queue );
    return WWD_SUCCESS;
}
