/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "portmacro.h"
#include "queue.h"
#include <timers.h>
#include <string.h>

#include "wifi_os_api.h"
#include "os.h"

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
os_queue_t wifi_os_queue_create( uint32_t queue_len, uint32_t item_size)
{
    return (os_queue_t) xQueueCreate(queue_len, item_size);
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void wifi_os_queue_delete(os_queue_t queue)
{
    vQueueDelete(queue);
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
uint32_t wifi_os_queue_get_space( const os_queue_t queue )
{
    configASSERT(queue);

    return (uint32_t)uxQueueSpacesAvailable(queue);
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_queue_receive(os_queue_t queue, void *buf, struct timeval *wait_time)
{
    configASSERT(queue);

    TickType_t ticks;

    if(wait_time == NULL){
        ticks = portMAX_DELAY;
    }else{
        ticks = (wait_time->tv_sec * 1000 + wait_time->tv_usec / 1000) / portTICK_PERIOD_MS;
    }

    if(pdTRUE == xQueueReceive(queue, buf, ticks))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_queue_send(os_queue_t queue, void *item, struct timeval *wait_time)
{
    configASSERT(queue);

    TickType_t ticks;

    if(wait_time == NULL){
        ticks = portMAX_DELAY;
    }else{
        ticks = (wait_time->tv_sec * 1000 + wait_time->tv_usec / 1000) / portTICK_PERIOD_MS;
    }

    if(pdTRUE == xQueueSend(queue, item, ticks))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_queue_send_front(os_queue_t queue, void *item, struct timeval *wait_time)
{
    configASSERT(queue);

    TickType_t ticks;

    if(wait_time == NULL){
        ticks = portMAX_DELAY;
    }else{
        ticks = (wait_time->tv_sec * 1000 + wait_time->tv_usec / 1000) / portTICK_PERIOD_MS;
    }

    if(pdTRUE == xQueueSendToFront(queue, item, ticks))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_queue_send_from_isr(os_queue_t queue, void *item)
{
    configASSERT(queue);

    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    if(pdPASS == xQueueSendToBackFromISR(queue, item, &xHigherPriorityTaskWoken))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_queue_send_front_from_isr(os_queue_t queue, void *item)
{
    configASSERT(queue);

    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    if(pdPASS == xQueueSendToFrontFromISR(queue, item, &xHigherPriorityTaskWoken))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_receive_from_isr(os_queue_t queue, void *buf)
{
    configASSERT(queue);

    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    if(pdPASS == xQueueReceiveFromISR( queue, buf, &xHigherPriorityTaskWoken))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
os_semaphore_t wifi_os_semphr_create_mutex(void)
{
    return (os_semaphore_t) xSemaphoreCreateMutex();
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
os_semaphore_t wifi_os_semphr_create_binary(void)
{
    return (os_semaphore_t) xSemaphoreCreateBinary();
}

#ifndef WIFI_OS_SEMPHR_TAKE_DBG
/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_semphr_take(os_semaphore_t semphr, struct timeval *wait_time)
{
    configASSERT(semphr);

    TickType_t ticks;

    if(wait_time == NULL){
        ticks = portMAX_DELAY;
    }else{
        ticks = (wait_time->tv_sec * 1000 + wait_time->tv_usec / 1000) / portTICK_PERIOD_MS;
    }

    if(pdTRUE == xSemaphoreTake((SemaphoreHandle_t)semphr, ticks))
        return 0;

    return -1;
}
#endif

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_semphr_give(os_semaphore_t semphr)
{
    configASSERT(semphr);

    if(pdTRUE == xSemaphoreGive((SemaphoreHandle_t)semphr))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_semphr_take_from_isr(os_semaphore_t semphr)
{
    configASSERT(semphr);

    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    if(pdTRUE == xSemaphoreTakeFromISR((SemaphoreHandle_t)semphr, &xHigherPriorityTaskWoken))
        return 0;

    return -1;    
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_semphr_give_from_isr(os_semaphore_t semphr)
{
    configASSERT(semphr);
    
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    if(pdTRUE == xSemaphoreGiveFromISR((SemaphoreHandle_t)semphr, &xHigherPriorityTaskWoken))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void wifi_os_semphr_delete(os_semaphore_t semphr)
{
    configASSERT(semphr);
    vSemaphoreDelete((SemaphoreHandle_t)semphr);
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
os_tmr_t wifi_os_timer_init(char *name, 
                               struct timeval *expires,
                               UBaseType_t auto_reload,
                               timer_handle_t handler
                               )
{
    TickType_t ticks;

    if(expires == NULL){
        return NULL;
    }else{
        ticks = (expires->tv_sec * 1000 + expires->tv_usec / 1000) / portTICK_PERIOD_MS;
    }

    return xTimerCreate(name, ticks, auto_reload, NULL, handler);
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_timer_deinit(os_tmr_t timer)
{
    configASSERT(timer);

    if(pdPASS == xTimerDelete(timer, (TickType_t)0))
        return -1;

    return 0;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_timer_start(os_tmr_t timer)
{
    configASSERT(timer);

    if(pdPASS == xTimerStart(timer, (TickType_t)0))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_timer_stop(os_tmr_t timer)
{
    configASSERT(timer);

    if(pdPASS == xTimerStop(timer, (TickType_t)0))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void switch_context(void)
{
    taskYIELD();
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
int32_t wifi_os_task_create(task_func_t handler,
                        char *name,
                        uint16_t stack_size,
                        void *params,
                        UBaseType_t prio,
                        os_task_t *task_handle
                        )
{
    if(pdPASS == xTaskCreate(handler,name,stack_size/4,params,prio,task_handle))
        return 0;

    return -1;
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void wifi_os_task_delete(os_task_t task)
{
    vTaskDelete(task);
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void wifi_os_task_sleep(uint32_t ms)
{
    vTaskDelay((TickType_t) (ms/portTICK_PERIOD_MS));
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void wifi_os_task_enter_critical(void)
{
    taskENTER_CRITICAL();
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void wifi_os_task_exit_critical(void)
{
    taskEXIT_CRITICAL();
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
os_task_t wifi_os_task_get_handle(void)
{
    return xTaskGetCurrentTaskHandle();
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
char * wifi_os_task_get_name(os_task_t task_handle)
{
    return pcTaskGetTaskName(task_handle);
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
BaseType_t wifi_os_task_get_scheduler_state(void)
{
    return xTaskGetSchedulerState();
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void* wifi_os_malloc(size_t size)
{
    return os_malloc(size);
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void* wifi_os_zalloc(size_t size)
{
    return os_zalloc(size);
}

/**
* @brief WiFi OS Adapter Layer API for internal usage, not open for other module or user
* @param
* @return
*/
void wifi_os_free(void *mem)
{
    os_free(mem);
}

size_t wifi_os_get_free_heap_size(void)
{
    return xPortGetFreeHeapSize();
}

