/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *
 */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"
#include "queue.h"
#include "FreeRTOSConfig.h"
#include "port_amazon_freertos.h"
#include "wiced_rtos.h"
#include "wiced_time.h"

/******************************************************
 *                      Macros
 ******************************************************/

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

const uint32_t millis_to_tick_ratio = (uint32_t)( 1000 / configTICK_RATE_HZ );

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_result_t wiced_rtos_init_semaphore( wiced_semaphore_t* sem )
{
    SemaphoreHandle_t xsemaphore = NULL;
    if (!sem)
    {
        return WICED_ERROR;
    }

    xsemaphore = xSemaphoreCreateCounting( (unsigned portBASE_TYPE) 0x7fffffff, (unsigned portBASE_TYPE) 0 );
    if (xsemaphore)
    {
        sem->_semaphore = (void *)xsemaphore;
        return WICED_SUCCESS;
    }

    return WICED_ERROR;
}

wiced_result_t wiced_rtos_get_semaphore( wiced_semaphore_t* sem, uint32_t timeout_ms )
{
    if (!sem || !sem->_semaphore )
    {
        return WICED_ERROR;
    }

    if ( pdTRUE == xSemaphoreTake((SemaphoreHandle_t)sem->_semaphore, (TickType_t) ( timeout_ms * ( 1000/configTICK_RATE_HZ ) ) ) )
    {
        return WICED_SUCCESS;
    }
    else
    {
        return WICED_ERROR;
    }
}

wiced_result_t wiced_rtos_set_semaphore( wiced_semaphore_t* semaphore )
{
    signed portBASE_TYPE result;

    if (!semaphore || !semaphore->_semaphore )
    {
        return WICED_ERROR;
    }

    /* TODO : Add implementation to set Semaphore from ISR context */
    result = xSemaphoreGive((SemaphoreHandle_t)semaphore->_semaphore);
    wiced_assert( "Unable to set semaphore", result == pdTRUE );
    return ( result == pdPASS )? WICED_SUCCESS : WICED_ERROR;
}

wiced_result_t wiced_rtos_deinit_semaphore( wiced_semaphore_t* semaphore )
{
    if (!semaphore || !semaphore->_semaphore )
    {
        return WICED_ERROR;
    }

    vQueueDelete( (SemaphoreHandle_t)semaphore->_semaphore );
    semaphore->_semaphore = NULL;

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_init_mutex( wiced_mutex_t* mutex )
{
    if (mutex == NULL)
    {
        return WICED_ERROR;
    }

    SemaphoreHandle_t Mutex = NULL;

    /* Mutex uses priority inheritance */
    Mutex = xSemaphoreCreateRecursiveMutex( );
    if (Mutex)
    {
        mutex->_mutex = (void *)Mutex;
        return WICED_SUCCESS;
    }

    return WICED_ERROR;
}

wiced_result_t wiced_rtos_lock_mutex( wiced_mutex_t* mutex )
{
    if (mutex == NULL)
    {
        return WICED_ERROR;
    }

    if ( xSemaphoreTakeRecursive( (SemaphoreHandle_t)mutex->_mutex, WICED_NEVER_TIMEOUT ) != pdPASS )
    {
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_unlock_mutex( wiced_mutex_t* mutex )
{
    if (mutex == NULL)
    {
        return WICED_ERROR;
    }

    if ( xSemaphoreGiveRecursive( (SemaphoreHandle_t)mutex->_mutex ) != pdPASS )
    {
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_deinit_mutex( wiced_mutex_t* mutex )
{
    if (mutex == NULL)
    {
        return WICED_ERROR;
    }

    vSemaphoreDelete( (SemaphoreHandle_t)mutex->_mutex );

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_create_thread( wiced_thread_t* thread, uint8_t priority, const char* name, wiced_thread_function_t function, uint32_t stack_size, void* arg )
{
    /* TODO: Handle Priority adjustments */
    signed portBASE_TYPE result;
    TaskHandle_t task = 0;

    if (!thread)
    {
        return WICED_ERROR;
    }

    result = xTaskCreate( (TaskFunction_t)function, name, (unsigned short)(stack_size / sizeof( portSTACK_TYPE )), (void*)arg, (unsigned portBASE_TYPE) priority, &task );

    if ( result == (signed portBASE_TYPE) pdPASS )
    {
        thread->_thread = (void *)task;
        return WICED_SUCCESS;
    }

    return WICED_ERROR;
}

wiced_result_t wiced_rtos_create_thread_with_stack( wiced_thread_t* thread, uint8_t priority, const char* name, wiced_thread_function_t function, void* stack, uint32_t stack_size, void* arg )
{
    uint32_t tcb_size, adjusted_stack_size;
    void *tcb_buffer, *stack_buffer;
    TaskHandle_t task = 0;

    if (!thread)
    {
        return WICED_ERROR;
    }

    /* TODO: Handle Priority adjustments */

    /* FreeRTOS requires a TCB buffer as well. Allocate this space from the start of the buffer passed in. */
    tcb_size            = (sizeof( StaticTask_t ) + portBYTE_ALIGNMENT - 1U) & ~(portBYTE_ALIGNMENT-1U);
    tcb_buffer          = (void*)(((uint32_t)stack + portBYTE_ALIGNMENT - 1) & ~(uint32_t)(portBYTE_ALIGNMENT-1));
    stack_buffer        = (uint8_t*)tcb_buffer + tcb_size;
    adjusted_stack_size = (uint32_t)stack + stack_size - (uint32_t)stack_buffer;

    task = xTaskCreateStatic( (TaskFunction_t)function, name, (unsigned short)(adjusted_stack_size / sizeof( portSTACK_TYPE )), (void*)arg, (unsigned portBASE_TYPE) priority, stack_buffer, tcb_buffer );
    if (task)
    {
        thread->_thread = (void *)task;
        // memset(thread->_error, 0, sizeof(thread->_error));
        return WICED_SUCCESS;
    }

    // SET_ERRNO( (task == NULL), EINVAL);

    return WICED_ERROR;
}

static void worker_thread_main( wiced_thread_arg_t arg )
{
    wiced_worker_thread_t* worker_thread = (wiced_worker_thread_t*) arg;

    while ( 1 )
    {
        wiced_event_message_t message;

        if ( wiced_rtos_pop_from_queue( &worker_thread->_que, &message, WICED_WAIT_FOREVER ) == WICED_SUCCESS )
        {
            message.function( message.arg );
        }
    }
}

wiced_result_t wiced_rtos_create_worker_thread( wiced_worker_thread_t* worker_thread, uint8_t priority, uint32_t stack_size, uint32_t event_queue_size )
{
    if (!worker_thread)
    {
        return WICED_BADARG;
    }

    memset( worker_thread, 0, sizeof( *worker_thread ) );

    if ( wiced_rtos_init_queue( &worker_thread->_que, "worker queue", sizeof(wiced_event_message_t), event_queue_size ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }

    if ( wiced_rtos_create_thread( &worker_thread->_thread, priority , "worker thread", worker_thread_main, stack_size, (void*) worker_thread ) != WICED_SUCCESS )
    {
        wiced_rtos_deinit_queue( &worker_thread->_que );
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_delete_worker_thread( wiced_worker_thread_t* worker_thread )
{
    if (!worker_thread)
    {
        return WICED_BADARG;
    }

    if ( wiced_rtos_delete_thread( &worker_thread->_thread ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }

    if ( wiced_rtos_deinit_queue( &worker_thread->_que ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_send_asynchronous_event( wiced_worker_thread_t* worker_thread, event_handler_t function, void* arg )
{
    wiced_event_message_t message;
    if (!worker_thread)
    {
        return WICED_BADARG;
    }

    message.function = function;
    message.arg = arg;

    return wiced_rtos_push_to_queue( &worker_thread->_que, &message, WICED_NO_WAIT );
}

wiced_result_t wiced_rtos_join_thread( wiced_thread_t* thread )
{
    if (!thread || !thread->_thread)
    {
        return WICED_ERROR;
    }

#if defined INCLUDE_xTaskIsTaskFinished && (INCLUDE_xTaskIsTaskFinished == 1)
    while ( xTaskIsTaskFinished( thread->_thread ) != pdTRUE )
    {
        (void) wiced_rtos_delay_milliseconds( (uint32_t) 10 );
    }
    return WICED_SUCCESS;
#else
    return WICED_UNSUPPORTED;
#endif
}

wiced_result_t wiced_rtos_delete_thread( wiced_thread_t* thread )
{
    if (thread == NULL)
    {
        /* Delete the current thread */
        vTaskDelete(NULL);
    }
    else if (thread->_thread == NULL)
    {
        return WICED_ERROR;
    }
    else
    {
        vTaskDelete( thread->_thread );
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_yield_thread (void)
{
    portYIELD_WITHIN_API();
    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_force_awake_thread( wiced_thread_t* thread )
{
    if (!thread || !thread->_thread)
    {
        return WICED_ERROR;
    }

    xTaskAbortDelay(thread->_thread);

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_is_current_thread( wiced_thread_t* thread )
{
    if (!thread || !thread->_thread)
    {
        return WICED_BADARG;
    }

#if ( ( INCLUDE_xTaskGetCurrentTaskHandle == 1 ) || ( configUSE_MUTEXES == 1 ) )
    if ( xTaskGetCurrentTaskHandle() == thread->_thread )
    {
        return WICED_SUCCESS;
    }
    else
    {
        return WICED_ERROR;
    }
#else
    return WICED_UNSUPPORTED;
#endif

}

wiced_result_t wiced_rtos_init_queue( wiced_queue_t* queue, const char* name, uint32_t message_size, uint32_t number_of_messages )
{
    QueueHandle_t xQueue = NULL;
    if (!queue)
    {
        return WICED_ERROR;
    }

    queue->_name = name;

    xQueue = xQueueCreate( (unsigned portBASE_TYPE) number_of_messages, (unsigned portBASE_TYPE) message_size );
    if (xQueue)
    {
        queue->_que = (void *)xQueue;
        return WICED_SUCCESS;
    }

    return WICED_ERROR;
}

wiced_result_t wiced_rtos_deinit_queue( wiced_queue_t* queue )
{
    if (!queue)
    {
        return WICED_ERROR;
    }

    vQueueDelete( (QueueHandle_t)queue->_que );

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_is_queue_empty( wiced_queue_t* queue )
{
    signed portBASE_TYPE result;

    if (!queue)
    {
        return WICED_ERROR;
    }

    taskENTER_CRITICAL();
    result = xQueueIsQueueEmptyFromISR( (QueueHandle_t)queue->_que );
    taskEXIT_CRITICAL();

    return ( result != 0 ) ? WICED_SUCCESS : WICED_ERROR;
}

wiced_result_t wiced_rtos_is_queue_full( wiced_queue_t* queue )
{
    signed portBASE_TYPE result;

    if (!queue)
    {
        return WICED_ERROR;
    }

    taskENTER_CRITICAL();
    result = xQueueIsQueueFullFromISR( (QueueHandle_t)queue->_que );
    taskEXIT_CRITICAL();

    return ( result != 0 ) ? WICED_SUCCESS : WICED_ERROR;
}

wiced_result_t wiced_rtos_push_to_queue( wiced_queue_t* queue, void* message, uint32_t timeout_ms )
{
    signed portBASE_TYPE retval;

    if (!queue)
    {
        return WICED_ERROR;
    }

    /* TODO: Add support for pushing to queue inside ISR context... */
    retval = xQueueSendToBack( (QueueHandle_t)queue->_que, message, (TickType_t) ( timeout_ms / millis_to_tick_ratio ) );
    if ( retval != pdPASS )
    {
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_pop_from_queue( wiced_queue_t* queue, void* message, uint32_t timeout_ms )
{
    signed portBASE_TYPE retval;

    if (!queue)
    {
        return WICED_ERROR;
    }

    /* TODO: Add support for popping from queue inside ISR context... */
    retval = xQueueReceive( (QueueHandle_t)queue->_que, message, (TickType_t) ( timeout_ms / millis_to_tick_ratio ) );

    if ( retval == errQUEUE_EMPTY )
    {
        return WICED_ERROR;
    }
    else if ( retval != pdPASS )
    {
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

static void internal_timer_callback( TimerHandle_t handle )
{
    wiced_timer_t* timer = (wiced_timer_t*) pvTimerGetTimerID( handle );

    if (timer->_function)
    {
        timer->_function(timer->_arg);
    }
}

wiced_result_t wiced_rtos_init_timer( wiced_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg )
{
    TimerHandle_t xTimer = NULL;

    if (timer == NULL)
    {
        return WICED_ERROR;
    }

    xTimer = xTimerCreate( (const char *)"", (TickType_t)( time_ms / millis_to_tick_ratio ), pdTRUE, timer, (TimerCallbackFunction_t)internal_timer_callback );
    if (xTimer)
    {
        timer->_tmr = (void *)xTimer;
        timer->_function = function;
        timer->_arg      = arg;

        return WICED_SUCCESS;
    }

    return WICED_ERROR;
}

wiced_result_t wiced_rtos_start_timer( wiced_timer_t* timer )
{
    if (!timer || !timer->_tmr)
    {
        return WICED_ERROR;
    }

    /* TODO: Add support for Starting a timer from ISR context */

    if (xTimerReset((TimerHandle_t)timer->_tmr, WICED_WAIT_FOREVER) != pdPASS)
    {
        return WICED_ERROR;
    }

    if (xTimerStart((TimerHandle_t)timer->_tmr, WICED_WAIT_FOREVER ) != pdPASS )
    {
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_stop_timer( wiced_timer_t* timer )
{
    if (!timer || !timer->_tmr)
    {
        return WICED_ERROR;
    }

    /* TODO: Add support for Stopping a timer from ISR context */

    if (xTimerStop( (TimerHandle_t)timer->_tmr, WICED_WAIT_FOREVER ) != pdPASS )
    {
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_deinit_timer( wiced_timer_t* timer )
{
    if (!timer || !timer->_tmr)
    {
        return WICED_ERROR;
    }

    if (xTimerDelete((TimerHandle_t)timer->_tmr, WICED_WAIT_FOREVER ) != pdPASS)
    {
        return WICED_ERROR;
    }

    timer->_tmr = NULL;

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_is_timer_running( wiced_timer_t* timer )
{
    if (!timer || timer->_tmr == NULL)
    {
        return WICED_ERROR;
    }

    return ( xTimerIsTimerActive((TimerHandle_t)timer->_tmr) != pdFALSE ) ? WICED_SUCCESS : WICED_ERROR;
}

wiced_result_t wiced_time_get_time(wiced_time_t* time_ptr)
{
    *time_ptr = (wiced_time_t) ( xTaskGetTickCount( ) * millis_to_tick_ratio );
    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_init_event_flags( wiced_event_flags_t* event_flags )
{
    EventGroupHandle_t xEvents = NULL;

    if (!event_flags)
    {
        return WICED_ERROR;
    }

    xEvents = xEventGroupCreateStatic( (StaticEventGroup_t *)pvPortMalloc( sizeof(StaticEventGroup_t) ) );
    if (xEvents)
    {
        event_flags->_evt_flags = (void *)xEvents;
        return WICED_SUCCESS;
    }

    return WICED_ERROR;
}

#if configUSE_16_BIT_TICKS == 1
    #define eventEVENT_BITS_MASK    0xffU           /* 8-bit */
#else
    #define eventEVENT_BITS_MASK    0xffffffUL      /* 24-bit */
#endif

wiced_result_t wiced_rtos_wait_for_event_flags( wiced_event_flags_t* event_flags, uint32_t flags_to_wait_for, uint32_t* flags_set, wiced_bool_t clear_set_flags, wiced_event_flags_wait_option_t wait_option, uint32_t timeout_ms )
{
    EventBits_t uxBitsToWaitFor = (flags_to_wait_for & eventEVENT_BITS_MASK);
    EventBits_t uxBits;

    if (!event_flags || !event_flags->_evt_flags)
    {
        return WICED_ERROR;
    }

    /*
     * CAUTION: depending on assert (not error return)
     *          in wiced_rtos_set_event_flags
     * No error checking here as wiced_rtos_set_event_flags
     * should have asserted if it is out of supported range
     */

    uxBits = xEventGroupWaitBits(
        (EventGroupHandle_t)event_flags->_evt_flags,                               /* xEventGroup */
        uxBitsToWaitFor,                            /* uxBitsToWaitFor */
        ((clear_set_flags == 1) ? pdTRUE : pdFALSE),       /* xClearOnExit */
        ((wait_option == WAIT_FOR_ALL_EVENTS) ? pdTRUE : pdFALSE),  /* xWaitForAllBits */
        (timeout_ms / millis_to_tick_ratio)             /* xTicksToWait */
    );

    /* Timeout (nothing detected) */
    if ((uxBits & uxBitsToWaitFor) == 0)
    {
        /* handle Time-out error codes separately */
        return WICED_ERROR;
        // return WICED_TIMEOUT;
    }

    *flags_set = uxBits;

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_set_event_flags( wiced_event_flags_t* event_flags, uint32_t flags_to_set )
{
    EventBits_t uxBitsToSet = (flags_to_set & eventEVENT_BITS_MASK);

    /* Out of EventBits range supported by FreeRTOS */
    wiced_assert("Out of supported range", uxBitsToSet == flags_to_set);

    if (!event_flags || !event_flags->_evt_flags)
    {
        return WICED_ERROR;
    }

    xEventGroupSetBits( (EventGroupHandle_t)event_flags->_evt_flags, uxBitsToSet );
    /* Ignore the return result (EventBits_t: all active bits) */

    /* WARNING: Do NOT compare return value with uxBitsToSet
     * for SUCCESS/FAIL.
     * The setting bit might had been cleared when this call returns
     */
    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_deinit_event_flags( wiced_event_flags_t* event_flags )
{
    if (!event_flags || !event_flags->_evt_flags)
    {
        return WICED_ERROR;
    }

    vEventGroupDelete((EventGroupHandle_t)event_flags->_evt_flags);  /* No return value */

    vPortFree((EventGroupHandle_t)event_flags->_evt_flags);

    event_flags->_evt_flags = NULL;

    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_delay_milliseconds( uint32_t num_ms )
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
    return WICED_SUCCESS;
}
