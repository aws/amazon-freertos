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

#ifndef __WIFI_OS_API_H__
#define __WIFI_OS_API_H__

/**
 * This header file describes the apis which might have different implementation if use differnt platform.
 * Once change the platform, these api need to be implemented by user.
 */

#include <stdbool.h>
#include <stdint.h>
#include "wifi_freertos_adapter.h"

#if defined(__GNUC__)
#include <sys/time.h>
#elif defined(__ICCARM__) || defined(__CC_ARM)
#if LWIP_TIMEVAL_PRIVATE
#include <lwip/sockets.h>
#else
struct timeval {
  long      tv_sec;
  long      tv_usec;
};
#endif
#endif

typedef void (*timer_handle_t)(os_tmr_t);
typedef void (*task_func_t)(void *);

os_queue_t wifi_os_queue_create( uint32_t queue_len, uint32_t item_size);
void wifi_os_queue_delete(os_queue_t queue);
int32_t wifi_os_queue_receive(os_queue_t queue, void *buf, struct timeval *wait_time);
int32_t wifi_os_queue_send(os_queue_t queue, void *item, struct timeval *wait_time);
int32_t wifi_os_queue_send_front(os_queue_t queue, void *item, struct timeval *wait_time);
int32_t wifi_os_queue_send_from_isr(os_queue_t queue, void *item);
int32_t wifi_os_queue_send_front_from_isr(os_queue_t queue, void *item);
int32_t wifi_os_receive_from_isr(os_queue_t queue, void *buf);
uint32_t wifi_os_queue_get_space( const os_queue_t queue );

os_semaphore_t wifi_os_semphr_create_mutex(void);
os_semaphore_t wifi_os_semphr_create_binary(void);

#define WIFI_OS_SEMPHR_TAKE_DBG
#ifndef WIFI_OS_SEMPHR_TAKE_DBG
int32_t wifi_os_semphr_take(os_semaphore_t semphr, struct timeval *wait_time);
#else
#define wifi_os_semphr_take(_semphr, _wait_time) ( (xSemaphoreTake( (SemaphoreHandle_t) _semphr, ( (_wait_time) == NULL ) ? portMAX_DELAY : ( (((struct timeval *)_wait_time)->tv_sec * 1000 + ((struct timeval *)_wait_time)->tv_usec / 1000) / portTICK_PERIOD_MS ) ) == pdTRUE ) ? 0 : -1 )
#endif
int32_t wifi_os_semphr_give(os_semaphore_t semphr);
int32_t wifi_os_semphr_take_from_isr(os_semaphore_t semphr);
int32_t wifi_os_semphr_give_from_isr(os_semaphore_t semphr);
void wifi_os_semphr_delete(os_semaphore_t semphr);

os_tmr_t wifi_os_timer_init(char *name, struct timeval *expires, UBaseType_t auto_reload, timer_handle_t handler);
int32_t wifi_os_timer_deinit(os_tmr_t timer);
int32_t wifi_os_timer_start(os_tmr_t timer);
int32_t wifi_os_timer_stop(os_tmr_t timer);

int32_t wifi_os_task_create(task_func_t handler,
                        char *name,
                        uint16_t stack_size,
                        void *params,
                        UBaseType_t prio,
                        os_task_t *task_handle
                        );
void wifi_os_task_delete(os_task_t task);
void wifi_os_task_sleep(uint32_t ms);
void wifi_os_task_enter_critical(void);
void wifi_os_task_exit_critical(void);
os_task_t wifi_os_task_get_handle(void);
char * wifi_os_task_get_name(os_task_t task_handle);
BaseType_t wifi_os_task_get_scheduler_state(void);


void* wifi_os_malloc(size_t size);
void* wifi_os_zalloc(size_t size);
void wifi_os_free(void *mem);
size_t wifi_os_get_free_heap_size(void);

void switch_context(void);

#endif /* __WIFI_OS_API_H__ */
