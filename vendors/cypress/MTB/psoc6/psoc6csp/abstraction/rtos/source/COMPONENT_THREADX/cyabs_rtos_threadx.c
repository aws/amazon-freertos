/***************************************************************************//**
* \file cyabs_rtos_threadx.c
*
* \brief
* Implementation for ThreadX abstraction
*
********************************************************************************
* \copyright
* Copyright 2018-2020 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include <cy_result.h>
#include <cy_utils.h>
#include <cyabs_rtos.h>
#include <tx_api.h>
#include <stdlib.h>

static const uint32_t WRAPPER_IDENT = 0xABCDEF01U;
static const uint32_t MAX_QUEUE_MESSAGE_SIZE = 16;
#define MAX_32_BIT (0xFFFFFFFFU)
#define ALL_EVENT_FLAGS (0xFFFFFFFFU)
#define MILLISECONDS_PER_SECOND (1000)

static cy_rtos_error_t last_error;

static cy_time_t convert_ms_to_ticks(cy_time_t timeout_ms)
{
    if (timeout_ms == CY_RTOS_NEVER_TIMEOUT)
    {
        return TX_WAIT_FOREVER;
    }
    else if (timeout_ms == 0)
    {
        return 0;
    }
    else
    {
        cy_time_t ticks = timeout_ms * TX_TIMER_TICKS_PER_SECOND / MILLISECONDS_PER_SECOND;
        if (ticks == 0)
        {
            ticks = 1;
        }
        else if (ticks > MAX_32_BIT)
        {
            // if ticks if more than 32 bits, change ticks to max possible value that isn't TX_WAIT_FOREVER.
            ticks = MAX_32_BIT - 1;
        }
        return ticks;
    }
}

static inline cy_time_t convert_ticks_to_ms(cy_time_t timeout_ticks)
{
    return timeout_ticks * MILLISECONDS_PER_SECOND / TX_TIMER_TICKS_PER_SECOND;
}

static inline cy_rslt_t convert_error(cy_rtos_error_t error)
{
    if (error != TX_SUCCESS)
    {
        last_error = error;
        return CY_RTOS_GENERAL_ERROR;
    }
    return CY_RSLT_SUCCESS;
}

/******************************************************
*                 Last Error
******************************************************/

cy_rtos_error_t cy_rtos_last_error()
{
    return last_error;
}

/******************************************************
*                 Threads
******************************************************/

typedef struct
{
    TX_THREAD thread;
    uint32_t magic;
    void *memptr;
} cy_thread_wrapper_t;


cy_rslt_t cy_rtos_create_thread(cy_thread_t *thread, cy_thread_entry_fn_t entry_function,
    const char *name, void *stack, uint32_t stack_size, cy_thread_priority_t priority, cy_thread_arg_t arg)
{
    stack_size &= ~CY_RTOS_ALIGNMENT_MASK; // make stack pointer 8-byte aligned
    if (thread == NULL || stack_size < CY_RTOS_MIN_STACK_SIZE)
    {
        return CY_RTOS_BAD_PARAM;
    }

    if (stack != NULL && (0 != (((uint32_t) stack) & CY_RTOS_ALIGNMENT_MASK)))
    {
        return CY_RTOS_ALIGNMENT_ERROR;
    }

    size_t malloc_size = sizeof(cy_thread_wrapper_t);
    if (stack == NULL)
    {
        malloc_size += stack_size;
    }
    void* buffer = malloc(malloc_size);
    if (buffer == NULL)
    {
        return CY_RTOS_NO_MEMORY;
    }

    cy_thread_wrapper_t *wrapper_ptr;
    if (stack == NULL)
    {
        stack = buffer;
        // Have stack be in front of wrapper since stack size is 8-byte aligned.
        wrapper_ptr = (cy_thread_wrapper_t *)(buffer + stack_size);
        wrapper_ptr->memptr = stack;
    }
    else
    {
        wrapper_ptr = buffer;
        wrapper_ptr->memptr = NULL;
    }
    wrapper_ptr->magic = WRAPPER_IDENT;

    *thread = (cy_thread_t)wrapper_ptr;

    cy_rtos_error_t tx_rslt = tx_thread_create(*thread, (CHAR *) name, entry_function, arg, stack, stack_size, priority, priority, TX_NO_TIME_SLICE, TX_AUTO_START); // Disable preemption-thresholding and time slicing
    if (TX_SUCCESS != tx_rslt)
    {
        last_error = tx_rslt;
        free(buffer);
        return CY_RTOS_GENERAL_ERROR;
    }

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_rtos_exit_thread()
{
   // No need to do anything before thread exit
   return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_rtos_terminate_thread(cy_thread_t *thread)
{
    if (thread == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }

    return convert_error(tx_thread_terminate(*thread));
}

cy_rslt_t cy_rtos_is_thread_running(cy_thread_t *thread, bool *running)
{
    if (thread == NULL || running == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }

    // Only true when the given thread is the current one
    *running = (*thread == tx_thread_identify());
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_rtos_get_thread_state(cy_thread_t *thread, cy_thread_state_t *state)
{
    if (thread == NULL || state == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }

    bool running;
    cy_rslt_t rslt = cy_rtos_is_thread_running(thread, &running);
    if (CY_RSLT_SUCCESS != rslt)
    {
        return rslt;
    }
    else if (running)
    {
        *state = CY_THREAD_STATE_RUNNING;
        return CY_RSLT_SUCCESS;
    }

    UINT thread_state;
    cy_rtos_error_t tx_rslt = tx_thread_info_get(*thread, TX_NULL, &thread_state, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    if (TX_SUCCESS != tx_rslt)
    {
        last_error = tx_rslt;
        return CY_RTOS_GENERAL_ERROR;
    }

    // Descriptions of these states are not given in the ThreadX user guide - these are best guesses as to their meanings
    switch(thread_state)
    {
        case TX_READY:
            *state = CY_THREAD_STATE_READY;
            break;
        case TX_COMPLETED:
        case TX_TERMINATED:
            *state = CY_THREAD_STATE_TERMINATED;
            break;
        case TX_SUSPENDED:
        case TX_SLEEP:
        case TX_QUEUE_SUSP:
        case TX_SEMAPHORE_SUSP:
        case TX_MUTEX_SUSP:
        case TX_EVENT_FLAG: // Likely waiting for event flags to be set (tx_event_flags_get)
        case TX_BLOCK_MEMORY: // Likely waiting to allocate a memory block (tx_block_allocate)
        case TX_BYTE_MEMORY: // Likely waiting to allocate a byte pool (tx_byte_allocate)
            *state = CY_THREAD_STATE_BLOCKED;
            break;
        default:
            *state = CY_THREAD_STATE_UNKNOWN;
            break;
    }

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_rtos_join_thread(cy_thread_t *thread)
{
    if (thread == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }

    // ThreadX doesn't have a join method itself, so just repeatedly check the thread's state until it completes or is terminated.
    // Check if the thread we are joining has a higher priority, if it does, we need to lower our priority to that of the other thread.
    UINT thread_state;

    cy_rtos_error_t tx_rslt = tx_thread_info_get(*thread, TX_NULL,  &thread_state, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    if (TX_SUCCESS != tx_rslt)
    {
        last_error = tx_rslt;
        return CY_RTOS_GENERAL_ERROR;
    }

    while(TX_TERMINATED != thread_state && TX_COMPLETED != thread_state)
    {
        tx_rslt = tx_thread_sleep(1);
        if (TX_SUCCESS != tx_rslt)
        {
            last_error = tx_rslt;
            return CY_RTOS_GENERAL_ERROR;
        }

        tx_rslt = tx_thread_info_get(*thread, TX_NULL, &thread_state, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
        if (TX_SUCCESS != tx_rslt)
        {
            last_error = tx_rslt;
            return CY_RTOS_GENERAL_ERROR;
        }
    }

    tx_rslt = tx_thread_delete(*thread);
    if (TX_SUCCESS != tx_rslt)
    {
        last_error = tx_rslt;
        return CY_RTOS_GENERAL_ERROR;
    }

    cy_thread_wrapper_t *wrapper_ptr = (cy_thread_wrapper_t *)(*thread);
    if (wrapper_ptr->magic == WRAPPER_IDENT)
    {
        if (wrapper_ptr->memptr != NULL)
        {
            free(wrapper_ptr->memptr);
        }
        else
        {
            free(wrapper_ptr);
        }
    }
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_rtos_get_thread_handle(cy_thread_t *thread)
{
    *thread = tx_thread_identify();
    return CY_RSLT_SUCCESS;
}

/******************************************************
*                 Mutexes
******************************************************/

cy_rslt_t cy_rtos_init_mutex2(cy_mutex_t *mutex, bool recursive)
{
    /* Non recursive mutex is not supported by ThreadX. A recursive mutex is returned
    * even if a non-recursive mutex was requested. This is ok because in all the cases
    * where the behavior of the two types differs would have ended in a deadlock. So
    * the difference in behavior should not have a functional impact on application.
    */
    CY_UNUSED_PARAMETER(recursive);
    return convert_error(tx_mutex_create(mutex, TX_NULL, TX_INHERIT));
}

cy_rslt_t cy_rtos_get_mutex(cy_mutex_t *mutex, cy_time_t timeout_ms)
{
    cy_rtos_error_t tx_rslt = tx_mutex_get(mutex, convert_ms_to_ticks(timeout_ms));
    if (TX_NOT_AVAILABLE == tx_rslt)
    {
        return CY_RTOS_TIMEOUT;
    }
    else
    {
        return convert_error(tx_rslt);
    }
}

cy_rslt_t cy_rtos_set_mutex(cy_mutex_t *mutex)
{
    return convert_error(tx_mutex_put(mutex));
}

cy_rslt_t cy_rtos_deinit_mutex(cy_mutex_t *mutex)
{
    return convert_error(tx_mutex_delete(mutex));
}


/******************************************************
*                 Semaphores
******************************************************/

cy_rslt_t cy_rtos_init_semaphore(cy_semaphore_t *semaphore, uint32_t maxcount, uint32_t initcount)
{
    if (semaphore == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }
    semaphore->maxcount = maxcount;
    return convert_error(tx_semaphore_create(&(semaphore->tx_semaphore), TX_NULL, initcount));
}

cy_rslt_t cy_rtos_get_semaphore(cy_semaphore_t *semaphore, cy_time_t timeout_ms, bool in_isr)
{
    if (semaphore == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }
    cy_rtos_error_t tx_rslt = tx_semaphore_get(&(semaphore->tx_semaphore), convert_ms_to_ticks(timeout_ms));
    if (TX_NO_INSTANCE == tx_rslt)
    {
        return CY_RTOS_TIMEOUT;
    }
    else
    {
        return convert_error(tx_rslt);
    }
}

cy_rslt_t cy_rtos_set_semaphore(cy_semaphore_t *semaphore, bool in_isr)
{
    if (semaphore == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }
    return convert_error(tx_semaphore_ceiling_put(&(semaphore->tx_semaphore), semaphore->maxcount));
}

cy_rslt_t cy_rtos_get_count_semaphore(cy_semaphore_t *semaphore, size_t *count)
{
    if (semaphore == NULL || count == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }
    return convert_error(tx_semaphore_info_get(&(semaphore->tx_semaphore), TX_NULL, (ULONG*)count, TX_NULL, TX_NULL, TX_NULL));
}

cy_rslt_t cy_rtos_deinit_semaphore(cy_semaphore_t *semaphore)
{
    if (semaphore == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }
    return convert_error(tx_semaphore_delete(&(semaphore->tx_semaphore)));
}


/******************************************************
*                 Events
******************************************************/

cy_rslt_t cy_rtos_init_event(cy_event_t *event)
{
    return convert_error(tx_event_flags_create(event, TX_NULL));
}

cy_rslt_t cy_rtos_setbits_event(cy_event_t *event, uint32_t bits, bool in_isr)
{
    return convert_error(tx_event_flags_set(event, bits, TX_OR));
}

cy_rslt_t cy_rtos_clearbits_event(cy_event_t *event, uint32_t bits, bool in_isr)
{
    return convert_error(tx_event_flags_set(event, ~bits, TX_AND));
}

cy_rslt_t cy_rtos_getbits_event(cy_event_t *event, uint32_t *bits)
{
    cy_rtos_error_t tx_rslt = tx_event_flags_get(event, ALL_EVENT_FLAGS, TX_OR, bits, TX_NO_WAIT);
    if (TX_NO_EVENTS == tx_rslt) // If timeout error occur with ALL_EVENT_FLAGS and TX_OR, then no flag is set
    {
        *bits = 0;
        return CY_RSLT_SUCCESS;
    }
    else
    {
        return convert_error(tx_rslt);
    }
}

cy_rslt_t cy_rtos_waitbits_event(cy_event_t *event, uint32_t *bits, bool clear, bool all, cy_time_t timeout_ms)
{
    UINT get_option;
    if (all)
    {
        get_option = clear ? TX_AND_CLEAR : TX_AND;
    }
    else
    {
        get_option = clear ? TX_OR_CLEAR : TX_OR;
    }

    cy_rtos_error_t tx_rslt = tx_event_flags_get(event, *bits, get_option, bits, convert_ms_to_ticks(timeout_ms));
    if (TX_NO_EVENTS == tx_rslt)
    {
        return CY_RTOS_TIMEOUT;
    }
    else
    {
        return convert_error(tx_rslt);
    }
}

cy_rslt_t cy_rtos_deinit_event(cy_event_t *event)
{
    return convert_error(tx_event_flags_delete(event));
}


/******************************************************
*                 Queues
******************************************************/

cy_rslt_t cy_rtos_init_queue(cy_queue_t *queue, size_t length, size_t itemsize)
{
    // Valid message lengths are {1-ULONG, 2-ULONG, 4-ULONG, 8-ULONG, 16-ULONG}
    static const uint32_t BYTES_PER_QUEUE_WORD = sizeof(ULONG);

    if (queue == NULL || itemsize == 0 || itemsize > BYTES_PER_QUEUE_WORD * MAX_QUEUE_MESSAGE_SIZE)
    {
        return CY_RTOS_BAD_PARAM;
    }

    // round message words to next power of 2 times word size.
    UINT message_words = 1;
    while (message_words * BYTES_PER_QUEUE_WORD < itemsize)
    {
        message_words <<= 1;
    }

    queue->itemsize = itemsize;
    ULONG queue_size = length * message_words * BYTES_PER_QUEUE_WORD;
    queue->mem = malloc(queue_size);
    if (queue->mem == NULL)
    {
        return CY_RTOS_NO_MEMORY;
    }

    cy_rtos_error_t tx_rslt = tx_queue_create(&(queue->tx_queue), TX_NULL, message_words, queue->mem, queue_size);
    if (TX_SUCCESS != tx_rslt)
    {
        last_error = tx_rslt;
        free(queue->mem);
        return CY_RTOS_GENERAL_ERROR;
    }

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_rtos_put_queue(cy_queue_t *queue, const void *item_ptr, cy_time_t timeout_ms, bool in_isr)
{
    if (queue == NULL || (in_isr && (timeout_ms != 0)))
    {
        return CY_RTOS_BAD_PARAM;
    }

    cy_rtos_error_t tx_rslt = tx_queue_send(&(queue->tx_queue), (void *) item_ptr, convert_ms_to_ticks(timeout_ms));
    if (TX_QUEUE_FULL == tx_rslt)
    {
        return CY_RTOS_NO_MEMORY;
    }
    else
    {
        return convert_error(tx_rslt);
    }
}

cy_rslt_t cy_rtos_get_queue(cy_queue_t *queue, void *item_ptr, cy_time_t timeout_ms, bool in_isr)
{
    ULONG buffer[MAX_QUEUE_MESSAGE_SIZE];
    if (queue == NULL || (in_isr && (timeout_ms != 0)))
    {
        return CY_RTOS_BAD_PARAM;
    }

    cy_rtos_error_t tx_rslt = tx_queue_receive(&(queue->tx_queue), buffer, convert_ms_to_ticks(timeout_ms));
    if (TX_QUEUE_EMPTY == tx_rslt)
    {
        return CY_RTOS_TIMEOUT;
    }
    else if (tx_rslt == TX_SUCCESS)
    {
        memcpy(item_ptr, buffer, queue->itemsize);
        return CY_RSLT_SUCCESS;
    }
    else
    {
        last_error = tx_rslt;
        return CY_RTOS_GENERAL_ERROR;
    }
}

cy_rslt_t cy_rtos_count_queue(cy_queue_t *queue, size_t *num_waiting)
{
    if (queue == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }
    return convert_error(tx_queue_info_get(&(queue->tx_queue), TX_NULL, (ULONG*)num_waiting, TX_NULL, TX_NULL, TX_NULL, TX_NULL));
}

cy_rslt_t cy_rtos_space_queue(cy_queue_t *queue, size_t *num_spaces)
{
    if (queue == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }
    return convert_error(tx_queue_info_get(&(queue->tx_queue), TX_NULL, TX_NULL, (ULONG*)num_spaces, TX_NULL, TX_NULL, TX_NULL));
}

cy_rslt_t cy_rtos_reset_queue(cy_queue_t *queue)
{
    if (queue == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }
    return convert_error(tx_queue_flush(&(queue->tx_queue)));
}

cy_rslt_t cy_rtos_deinit_queue(cy_queue_t *queue)
{
    if (queue == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }
    cy_rslt_t result = convert_error(tx_queue_delete(&(queue->tx_queue)));
    if (result == CY_RSLT_SUCCESS)
    {
        free(queue->mem);
    }
    return result;
}


/******************************************************
*                 Timers
******************************************************/

cy_rslt_t cy_rtos_init_timer(cy_timer_t *timer, cy_timer_trigger_type_t type,
    cy_timer_callback_t fun, cy_timer_callback_arg_t arg)
{
    if (timer == NULL || fun == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }
    timer->oneshot = (type == CY_TIMER_TYPE_ONCE);
    // Use 1s here as default timeouts since these are going to get changed anyway
    return convert_error(tx_timer_create(&(timer->tx_timer), TX_NULL, fun, arg, 1, 1, TX_NO_ACTIVATE));
}

cy_rslt_t cy_rtos_start_timer(cy_timer_t *timer, cy_time_t num_ms)
{
    if (timer == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }
    ULONG timer_ticks = convert_ms_to_ticks(num_ms);
    cy_rslt_t rslt = convert_error(tx_timer_change(&(timer->tx_timer), timer_ticks, timer->oneshot ? 0 : timer_ticks));
    if (CY_RSLT_SUCCESS != rslt)
    {
        return rslt;
    }
    return convert_error(tx_timer_activate(&(timer->tx_timer)));
}

cy_rslt_t cy_rtos_stop_timer(cy_timer_t *timer)
{
    if (timer == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }
    return convert_error(tx_timer_deactivate(&(timer->tx_timer)));
}

cy_rslt_t cy_rtos_is_running_timer(cy_timer_t *timer, bool *state)
{
    if (timer == NULL || state == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }

    UINT active;
    cy_rslt_t rslt = convert_error(tx_timer_info_get(&(timer->tx_timer), TX_NULL, &active, TX_NULL, TX_NULL, TX_NULL));
    if (CY_RSLT_SUCCESS == rslt)
    {
        *state = (active == TX_TRUE);
    }
    return rslt;
}

cy_rslt_t cy_rtos_deinit_timer(cy_timer_t *timer)
{
    if (timer == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }
    return convert_error(tx_timer_delete(&(timer->tx_timer)));
}


/******************************************************
*                 Time
******************************************************/

cy_rslt_t cy_rtos_get_time(cy_time_t *tval)
{
    if (tval == NULL)
    {
        return CY_RTOS_BAD_PARAM;
    }

    *tval = convert_ticks_to_ms(tx_time_get());

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_rtos_delay_milliseconds(cy_time_t num_ms)
{
    cy_time_t ticks = convert_ms_to_ticks(num_ms);

    while (ticks > 0)
    {
        cy_time_t wait_ticks = (ticks >= MAX_32_BIT) ? (MAX_32_BIT - 1) : ticks;
        cy_rslt_t rslt = convert_error(tx_thread_sleep(wait_ticks));
        if (CY_RSLT_SUCCESS != rslt)
        {
            return rslt;
        }
        ticks -= wait_ticks;
    }
    return CY_RSLT_SUCCESS;
}
