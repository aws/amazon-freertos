/**	
 * \file            esp_sys_template.h
 * \brief           Template file for system functions
 */

/*
 * Copyright (c) 2018 Tilen Majerle
 *  
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of ESP-AT.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#ifndef __ESP_SYSTEM_H
#define __ESP_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stdint.h"
#include "stdlib.h"

#include "esp_config.h"

/**
 * \ingroup         ESP_PORT
 * \defgroup        ESP_SYS System functions
 * \brief           System based function for OS management, timings, etc
 * \{
 */

#if ESP_CFG_OS || __DOXYGEN__
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/**
 * \brief           ESP system mutex ID type
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
struct _sys_mut {
  void *mut;
};
typedef struct _sys_mut         esp_sys_mutex_t;

/**
 * \brief           ESP system semaphore ID type
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
struct _sys_sem {
  void *sem;
};
typedef struct _sys_sem         esp_sys_sem_t;

/**
 * \brief           ESP system message queue ID type
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
struct _sys_mbox {
  void *mbx;
};
typedef struct _sys_mbox        esp_sys_mbox_t;

/**
 * \brief           ESP system thread ID type
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
struct _sys_thread {
  void *thread_handle;
};
typedef struct _sys_thread      esp_sys_thread_t;

/**
 * \brief           ESP system thread priority type
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
typedef UBaseType_t             esp_sys_thread_prio_t;

/**
 * \brief           Value indicating message queue is not valid
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define ESP_SYS_MBOX_NULL           NULL

/**
 * \brief           Value indicating semaphore is not valid
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define ESP_SYS_SEM_NULL            NULL

/**
 * \brief           Value indicating mutex is not valid
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define ESP_SYS_MUTEX_NULL          NULL

/**
 * \brief           Value indicating timeout for OS timings
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define ESP_SYS_TIMEOUT             ((uint32_t)portMAX_DELAY)

/**
 * \brief           ESP stack threads priority parameter
 * \note            Usually normal priority is ok. If many threads are in the system and high traffic is introduced
 *                  This value might need to be set to higher value
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define ESP_SYS_THREAD_PRIO         (configMAX_PRIORITIES - 1)

/**
 * \brief           Stack size of system threads
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define ESP_SYS_THREAD_SS           (1024)
#endif /* ESP_OS || __DOXYGEN__ */

uint8_t     esp_sys_init(void);
uint32_t    esp_sys_now(void);

uint8_t     esp_sys_protect(void);
uint8_t     esp_sys_unprotect(void);

uint8_t     esp_sys_mutex_create(esp_sys_mutex_t* p);
uint8_t     esp_sys_mutex_delete(esp_sys_mutex_t* p);
uint8_t     esp_sys_mutex_lock(esp_sys_mutex_t* p);
uint8_t     esp_sys_mutex_unlock(esp_sys_mutex_t* p);
uint8_t     esp_sys_mutex_isvalid(esp_sys_mutex_t* p);
uint8_t     esp_sys_mutex_invalid(esp_sys_mutex_t* p);

uint8_t     esp_sys_sem_create(esp_sys_sem_t* p, uint8_t cnt);
uint8_t     esp_sys_sem_delete(esp_sys_sem_t* p);
uint32_t    esp_sys_sem_wait(esp_sys_sem_t* p, uint32_t timeout);
uint8_t     esp_sys_sem_release(esp_sys_sem_t* p);
uint8_t     esp_sys_sem_isvalid(esp_sys_sem_t* p);
uint8_t     esp_sys_sem_invalid(esp_sys_sem_t* p);

uint8_t     esp_sys_mbox_create(esp_sys_mbox_t* b, size_t size);
uint8_t     esp_sys_mbox_delete(esp_sys_mbox_t* b);
uint32_t    esp_sys_mbox_put(esp_sys_mbox_t* b, void* m);
uint32_t    esp_sys_mbox_get(esp_sys_mbox_t* b, void** m, uint32_t timeout);
uint8_t     esp_sys_mbox_putnow(esp_sys_mbox_t* b, void* m);
uint8_t     esp_sys_mbox_getnow(esp_sys_mbox_t* b, void** m);
uint8_t     esp_sys_mbox_isvalid(esp_sys_mbox_t* b);
uint8_t     esp_sys_mbox_invalid(esp_sys_mbox_t* b);

uint8_t     esp_sys_thread_create(esp_sys_thread_t* t, const char* name, esp_sys_thread_fn thread_func, void* const arg, size_t stack_size, esp_sys_thread_prio_t prio);
uint8_t     esp_sys_thread_terminate(esp_sys_thread_t* t);
uint8_t     esp_sys_thread_yield(void);
 
/**
 * \}
 */

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* __ESP_SYSTEM_H */
