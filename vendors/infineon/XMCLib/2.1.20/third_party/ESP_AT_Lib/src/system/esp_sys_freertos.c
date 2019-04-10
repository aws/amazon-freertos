/**	
 * \file            esp_sys_template.c
 * \brief           System dependant functions
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
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#include "system/esp_sys.h"

#if ESP_CFG_SYS_PORT == ESP_SYS_PORT_FREERTOS

#include <xmc_common.h>

/*******************************************/
/*******************************************/
/**   Modify this file for your system    **/
/*******************************************/
/*******************************************/
__STATIC_INLINE bool isInterrupt(void)
{
  return ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0);
}

static SemaphoreHandle_t sys_mutex;

/**
 * \brief           Init system dependant parameters
 * \note            Called from high-level application layer when required
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_init(void)
{
  sys_mutex = xSemaphoreCreateRecursiveMutex();           /* Create system mutex */
  if (sys_mutex != NULL)
  {
	return 1;
  }

  return 0;
}

/**
 * \brief           Get current time in units of milliseconds
 * \return          Current time in units of milliseconds
 */
uint32_t esp_sys_now(void)
{
  return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

/**
 * \brief           Protect stack core
 * \note            This function is required with OS
 *
 * \note            This function may be called multiple times, recursive protection is required
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_protect(void)
{
  if (xSemaphoreTakeRecursive(sys_mutex, portMAX_DELAY) == pdTRUE)
  {
    return 1;
  }
  else
  {
	return 0;
  }
}

/**
 * \brief           Protect stack core
 * \note            This function is required with OS
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_unprotect(void)
{
  return (xSemaphoreGiveRecursive(sys_mutex) == pdTRUE);
}

/**
 * \brief           Create a new mutex and pass it to input pointer
 * \note            This function is required with OS
 * \note            Recursive mutex must be created as it may be locked multiple times before unlocked
 * \param[out]      p: Pointer to mutex structure to save result to
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_mutex_create(esp_sys_mutex_t* p)
{
  p->mut = xSemaphoreCreateRecursiveMutex();
  if (p->mut == NULL) {
    return 0;
  }
  return 1;
}

/**
 * \brief           Delete mutex from OS
 * \note            This function is required with OS
 * \param[in]       p: Pointer to mutex structure
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_mutex_delete(esp_sys_mutex_t* p)
{
  vSemaphoreDelete(p->mut);
  p->mut = NULL;
  return 1;
}

/**
 * \brief           Wait forever to lock the mutex
 * \note            This function is required with OS
 * \param[in]       p: Pointer to mutex structure
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_mutex_lock(esp_sys_mutex_t* p)
{
  return (xSemaphoreTakeRecursive(p->mut, portMAX_DELAY) == pdPASS);
}

/**
 * \brief           Unlock mutex
 * \note            This function is required with OS
 * \param[in]       p: Pointer to mutex structure
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_mutex_unlock(esp_sys_mutex_t* p)
{
  return (xSemaphoreGiveRecursive(p->mut) == pdTRUE);
}

/**
 * \brief           Check if mutex structure is valid OS entry
 * \note            This function is required with OS
 * \param[in]       p: Pointer to mutex structure
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_mutex_isvalid(esp_sys_mutex_t* p)
{
	return ((p != NULL) && (p->mut != NULL));       /* Check if mutex is valid */
}

/**
 * \brief           Set mutex structure as invalid
 * \note            This function is required with OS
 * \param[in]       p: Pointer to mutex structure
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_mutex_invalid(esp_sys_mutex_t* p)
{
	p->mut = ESP_SYS_MUTEX_NULL;                    /* Set mutex as invalid */
    return 1;
}

/**
 * \brief           Create a new binary semaphore and set initial state
 * \note            Semaphore may only have 1 token available
 * \note            This function is required with OS
 * \param[out]      p: Pointer to semaphore structure to fill with result
 * \param[in]       cnt: Count indicating default semaphore state:
 *                     0: Lock it immediteally
 *                     1: Leave it unlocked
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_sem_create(esp_sys_sem_t* p, uint8_t cnt)
{
  p->sem = xSemaphoreCreateBinary();
  if (p->sem == NULL)
  {
	return 0;
  }

  if (cnt == 1)
  {
    xSemaphoreGive(p->sem);
  }
  return 1;
}

/**
 * \brief           Delete binary semaphore
 * \note            This function is required with OS
 * \param[in]       p: Pointer to semaphore structure
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_sem_delete(esp_sys_sem_t* p)
{
  vSemaphoreDelete(p->sem);
  p->sem = NULL;

  return 1;
}

/**
 * \brief           Wait for semaphore to be available
 * \note            This function is required with OS
 * \param[in]       p: Pointer to semaphore structure
 * \param[in]       timeout: Timeout to wait in milliseconds. When 0 is applied, wait forever
 * \return          Number of milliseconds waited for semaphore to become available
 */
uint32_t
esp_sys_sem_wait(esp_sys_sem_t* p, uint32_t timeout)
{
  uint32_t time_start , time_end;

  time_start = esp_sys_now();

  if(!timeout)
  {
    /* wait infinite */
	while (xSemaphoreTake(p->sem, portMAX_DELAY) != pdTRUE);
    time_end = esp_sys_now();
  }
  else
  {
    TickType_t timeout_ticks = pdMS_TO_TICKS(timeout);
    if (xSemaphoreTake(p->sem, timeout_ticks) == errQUEUE_EMPTY)
    {
      /* timed out */
      return ESP_SYS_TIMEOUT;
    }
    time_end = esp_sys_now();
  }

  return (((time_end - time_start) == 0) ? 1 : (time_end - time_start));

}

/**
 * \brief           Release semaphore
 * \note            This function is required with OS
 * \param[in]       p: Pointer to semaphore structure
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_sem_release(esp_sys_sem_t* p)
{
  return (xSemaphoreGive(p->sem) == pdTRUE);
}

/**
 * \brief           Check if semaphore is valid
 * \note            This function is required with OS
 * \param[in]       p: Pointer to semaphore structure
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_sem_isvalid(esp_sys_sem_t* p)
{
  return ((p != NULL) && (p->sem != NULL));                                /* Check if valid */
}

/**
 * \brief           Invalid semaphore
 * \note            This function is required with OS
 * \param[in]       p: Pointer to semaphore structure
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_sem_invalid(esp_sys_sem_t* p)
{
  p->sem = ESP_SYS_SEM_NULL;                    /* Invaldiate semaphore */
  return 1;
}

/**
 * \brief           Create a new message queue with entry type of "void *"
 * \note            This function is required with OS
 * \param[out]      b: Pointer to message queue structure
 * \param[in]       size: Number of entries for message queue to hold
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_mbox_create(esp_sys_mbox_t* b, size_t size)
{
  b->mbx = xQueueCreate((UBaseType_t)size, sizeof(void *));
  if(b->mbx == NULL)
  {
    return 0;
  }
  return 1;
}

/**
 * \brief           Delete message queue
 * \note            This function is required with OS
 * \param[in]       b: Pointer to message queue structure
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_mbox_delete(esp_sys_mbox_t* b)
{
  if (uxQueueMessagesWaiting(b->mbx))
  {
    return 0;
  }

  vQueueDelete(b->mbx);
  return 1;
}

/**
 * \brief           Put a new entry to message queue and wait until memory available
 * \note            This function is required with OS
 * \param[in]       b: Pointer to message queue structure
 * \param[in]       m: Pointer to entry to insert to message queue
 * \return          Time in units of milliseconds needed to put a message to queue
 */
uint32_t esp_sys_mbox_put(esp_sys_mbox_t* b, void* m)
{
  uint32_t time_in = esp_sys_now();
  if (xQueueSendToBack(b->mbx, &m, portMAX_DELAY) == pdPASS)
  {
    return esp_sys_now() - time_in;
  }
  else
  {
    return ESP_SYS_TIMEOUT;
  }
}

/**
 * \brief           Get a new entry from message queue with timeout
 * \note            This function is required with OS
 * \param[in]       b: Pointer to message queue structure
 * \param[in]       m: Pointer to pointer to result to save value from message queue to
 * \param[in]       timeout: Maximal timeout to wait for new message. When 0 is applied, wait for unlimited time
 * \return          Time in units of milliseconds needed to put a message to queue
 */
uint32_t esp_sys_mbox_get(esp_sys_mbox_t* b, void** m, uint32_t timeout)
{
  void *msg_dummy;
  uint32_t time_start , time_end;

  time_start = esp_sys_now();

  if (!m) {
    m = &msg_dummy;
  }

  if (!timeout)
  {
    /* wait infinite */
	while (xQueueReceive(b->mbx, &(*m), portMAX_DELAY) != pdTRUE);
    time_end = esp_sys_now();
  }
  else
  {
    TickType_t timeout_ticks = pdMS_TO_TICKS(timeout);
    if (xQueueReceive(b->mbx, &(*m), timeout_ticks) != pdTRUE)
    {
      /* timed out */
      *m = NULL;
      return ESP_SYS_TIMEOUT;

    }
    time_end = esp_sys_now();
  }

  return (((time_end - time_start) == 0) ? 1 : (time_end - time_start));
}

/**
 * \brief           Put a new entry to message queue without timeout (now or fail)
 * \note            This function is required with OS
 * \param[in]       b: Pointer to message queue structure
 * \param[in]       m: Pointer to message to save to queue
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_mbox_putnow(esp_sys_mbox_t* b, void* m)
{
  if (isInterrupt())
  {
    BaseType_t ret;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	ret = xQueueSendToBackFromISR(b->mbx, &m, &xHigherPriorityTaskWoken);
	if (ret == pdTRUE)
	{
	  if (xHigherPriorityTaskWoken == pdTRUE)
	  {
	    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	  }
	  return 1;
	}
	else
	{
	  return 0;
	}
  }
  else
  {
	return (xQueueSendToBack(b->mbx, &m, 0) == pdTRUE);
  }
}

/**
 * \brief           Get an entry from message queue immediatelly
 * \note            This function is required with OS
 * \param[in]       b: Pointer to message queue structure
 * \param[in]       m: Pointer to pointer to result to save value from message queue to
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_mbox_getnow(esp_sys_mbox_t* b, void** m)
{
  void *msg_dummy;

  if (!m)
  {
    m = &msg_dummy;
  }

  if (xQueueReceive(b->mbx, &(*m), 0) != pdTRUE)
  {
    *m = NULL;
    return 0;
  }

  return 1;
}

/**
 * \brief           Check if message queue is valid
 * \note            This function is required with OS
 * \param[in]       b: Pointer to message queue structure
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_mbox_isvalid(esp_sys_mbox_t* b)
{
  return ((b != NULL) && (b->mbx != NULL));       /* Return status if message box is valid */
}

/**
 * \brief           Invalid message queue
 * \note            This function is required with OS
 * \param[in]       b: Pointer to message queue structure
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_mbox_invalid(esp_sys_mbox_t* b)
{
  b->mbx = ESP_SYS_MBOX_NULL;                     /* Invalidate message box */
  return 1;
}

/**
 * \brief           Create a new thread
 * \note            This function is required with OS
 * \param[out]      t: Pointer to thread identifier if create was successful
 * \param[in]       name: Name of a new thread
 * \param[in]       thread_func: Thread function to use as thread body
 * \param[in]       arg: Thread function argument
 * \param[in]       stack_size: Size of thread stack in uints of bytes. If set to 0, reserve default stack size
 * \param[in]       prio: Thread priority 
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_thread_create(esp_sys_thread_t* t, const char* name, esp_sys_thread_fn thread_func, void* const arg, size_t stack_size, esp_sys_thread_prio_t prio)
{
  TaskHandle_t rtos_task;
  BaseType_t ret;

  ret = xTaskCreate(thread_func, name, stack_size, arg, prio, &rtos_task);

  if (ret != pdPASS)
  {
	return 0;
  }

  t->thread_handle = rtos_task;
  return 1;
}

/**
 * \brief           Terminate thread (shut it down and remove)
 * \note            This function is required with OS
 * \param[in]       t: Pointer to thread handle to terminate. If set to NULL, terminate current thread (thread from where function is called)
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_thread_terminate(esp_sys_thread_t* t)
{
  vTaskDelete(t->thread_handle);
  return 1;
}

/**
 * \brief           Yield current thread
 * \note            This function is required with OS
 * \return          1 on success, 0 otherwise
 */
uint8_t esp_sys_thread_yield(void)
{
  taskYIELD();
  return 1;
}

#endif
