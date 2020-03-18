/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "os_wrapper/common.h"
#include "os_wrapper/mutex.h"
#include "os_wrapper/semaphore.h"
#include "os_wrapper/thread.h"


void *os_wrapper_thread_new(const char* name, int32_t stack_size,
                            os_wrapper_thread_func func, void *arg,
                            uint32_t priority)
{
    uint32_t stack = stack_size > 0U ? stack_size / sizeof(StackType_t)
	                 : configMINIMAL_STACK_SIZE;
    UBaseType_t prio = (UBaseType_t) priority;
    TaskHandle_t handle;

    if (xTaskCreate ((TaskFunction_t)func, name, (uint16_t)stack, arg, prio, &handle) != pdPASS) {
        return NULL;
    }
    
    return (void *)handle;
}


void *os_wrapper_semaphore_create(uint32_t max_count, uint32_t initial_count,
                                     const char* name)
{
    SemaphoreHandle_t handle;

    handle = xSemaphoreCreateCounting (max_count, initial_count);

    if (handle != NULL) {
#if (configQUEUE_REGISTRY_SIZE > 0)
        vQueueAddToRegistry (handle, name);
#else
        (void) name;
#endif
    }

    return (void *)handle;
}

uint32_t os_wrapper_semaphore_acquire(void *handle, uint32_t timeout)
{

    if (xSemaphoreTake((SemaphoreHandle_t)handle, (TickType_t) timeout)
		    != pdTRUE) {
        return OS_WRAPPER_ERROR;
    }

    return OS_WRAPPER_SUCCESS;
}

uint32_t os_wrapper_semaphore_release(void *handle)
{

    if (xSemaphoreGive((SemaphoreHandle_t)handle) != pdTRUE) {
        return OS_WRAPPER_ERROR;
    }

    return OS_WRAPPER_SUCCESS;
}

uint32_t os_wrapper_semaphore_delete(void *handle)
{
    SemaphoreHandle_t hsem = (SemaphoreHandle_t)handle;

#if (configQUEUE_REGISTRY_SIZE > 0)
    vQueueUnregisterQueue (hsem);
#endif
    vSemaphoreDelete(hsem);

    return OS_WRAPPER_SUCCESS;
}

void *os_wrapper_mutex_create(void)
{
    return (void *) xSemaphoreCreateMutex();
}

uint32_t os_wrapper_mutex_acquire(void *handle, uint32_t timeout)
{
    if (handle == NULL) {
        return OS_WRAPPER_ERROR;
    }

    if(xSemaphoreTake((SemaphoreHandle_t) handle, (TickType_t) timeout)
		    != pdTRUE) {
        return OS_WRAPPER_ERROR;
    }

    return OS_WRAPPER_SUCCESS;
}

uint32_t os_wrapper_mutex_release(void *handle)
{
    if (handle == NULL) {
        return OS_WRAPPER_ERROR;
    }

    if(xSemaphoreGive((SemaphoreHandle_t) handle) != pdTRUE) {
        return OS_WRAPPER_ERROR;
    }

    return OS_WRAPPER_SUCCESS;
}

uint32_t os_wrapper_mutex_delete(void *handle)
{
    if (handle == NULL) {
        return OS_WRAPPER_ERROR;
    }

    vSemaphoreDelete((SemaphoreHandle_t)handle);

    return OS_WRAPPER_SUCCESS;
}

void *os_wrapper_thread_get_handle(void)
{
    return (void *) xTaskGetCurrentTaskHandle();
}

uint32_t os_wrapper_thread_get_priority(void *handle, uint32_t *priority)
{
     *priority = (uint32_t) uxTaskPriorityGet((TaskHandle_t) handle);
     return OS_WRAPPER_SUCCESS;
}

void os_wrapper_thread_exit(void)
{
    vTaskDelete(NULL);
}

uint32_t os_wrapper_join_thread(void* handle)
{
    return OS_WRAPPER_SUCCESS;
}

void os_wrapper_delay(uint32_t ms)
{
   TickType_t tick = ms / portTICK_PERIOD_MS;

   vTaskDelay(tick);
}
