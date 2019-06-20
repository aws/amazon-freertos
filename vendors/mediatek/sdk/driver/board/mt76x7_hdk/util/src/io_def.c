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

#include <stdint.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include <dma_sw.h>
#include <hal_uart.h>

#include "io_def.h"
#include "memory_attribute.h"

#if configUSE_TICKLESS_IDLE == 2
#include "hal_sleep_manager.h"
#include "timers.h"

#if configTICKLESS_DEEP_SLEEP
#include "port_tick.h"
#endif

#endif


#if  defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
#endif /* __GNUC__ */


#define VFIFO_TX_SIZE           512
#define VFIFO_RX_SIZE           128
#define VFIFO_ALERT_LENGTH      0


/* Block UART definition ----------------------------------------------------*/

static bool is_io_ready = false;

static SemaphoreHandle_t    _g_semaphore_tx = NULL;
static SemaphoreHandle_t    _g_semaphore_rx = NULL;

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t              g_tx_vfifo[VFIFO_TX_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t              g_rx_vfifo[VFIFO_RX_SIZE];


/****************************************************************************
 * Sleep Functions
 ****************************************************************************/


/**
  * @brief  Retargets the C library getchar function to the USART.
  * @param  None
  * @retval None
  */
#ifdef HAL_SLEEP_MANAGER_ENABLED
#if configUSE_TICKLESS_IDLE == 2
static uint8_t tickless_handle_index = 0xFF;
static TimerHandle_t xSleepLockTimer;

static void sleeplock_timeout_callback(TimerHandle_t expiredTimer)
{
    hal_sleep_manager_unlock_sleep(tickless_handle_index);
}
#endif
#endif


/****************************************************************************
 * Private Functions
 ****************************************************************************/


static void _uart_event(hal_uart_callback_event_t event, void *user_data)
{
    BaseType_t  x_higher_priority_task_woken = pdFALSE;

    switch (event) {
    case HAL_UART_EVENT_READY_TO_READ:
        xSemaphoreGiveFromISR(_g_semaphore_rx, &x_higher_priority_task_woken);
        break;
    case HAL_UART_EVENT_READY_TO_WRITE:
        xSemaphoreGiveFromISR(_g_semaphore_tx, &x_higher_priority_task_woken);
        break;
    }

    /*
     * xSemaphoreGiveFromISR() will set *pxHigherPriorityTaskWoken to pdTRUE
     * if giving _g_semaphore_rx or _g_semaphore_tx caused a task to unblock,
     * and the unblocked task has a priority higher than the currently running
     * task. If xSemaphoreGiveFromISR() sets this value to pdTRUE then a
     * context switch should be requested before the interrupt exits.
     */

    portYIELD_FROM_ISR(x_higher_priority_task_woken);
}


/**
 * Check for conditions that blocking APIs can not be used.
 *
 * 1. Before OS starts.
 * 2. When interrupt is disabled.
 * 3. Currently in an ISR.
 */
static int8_t _uart_dma_blocking_is_safe(void)
{
#if ( ( INCLUDE_xTaskGetSchedulerState == 1 ) || ( configUSE_TIMERS == 1 ) )
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING &&
        __get_PRIMASK() == 0                              &&
        ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) >> SCB_ICSR_VECTACTIVE_Pos) == 0)
    {
        return 1;
    }
#endif

    return 0;
}


/****************************************************************************
 * Public Functions
 ****************************************************************************/


#if defined (__CC_ARM) || defined (__ICCARM__)
__weak int fputc(int ch, FILE *f)
{
    return io_def_uart_putchar(ch);
}
#endif


__weak int __io_putchar(int ch)
{
    return io_def_uart_putchar(ch);
}


__weak int __io_getchar(void)
{
    return io_def_uart_getchar();
}


/**
  * @brief  Retargets the C library printf function to the USART.
  *
  * This API correspond to __io_putchar() for many open source C library
  * implementations. Most commonly, it is called by printf series APIs.
  * It could be used before or after OS starts running (scheduler starts).
  *
  * Check _uart_dma_blocking_is_safe() for details of statuses that blocking
  * APIs can not be used.
  *
  * @param  ch  the character to be sent.
  *
  * @retval None
  */
int io_def_uart_putchar(int ch)
{
    uint32_t n;

    if (!is_io_ready) {
        return 0;
    }

    if (_uart_dma_blocking_is_safe()) {
        do {
            n = hal_uart_send_dma(CONSOLE_UART, (uint8_t *)&ch, 1);
        } while (!n && xSemaphoreTake(_g_semaphore_tx, 100/*portMAX_DELAY*/) == pdTRUE);

        if (ch != '\n')
            return ch;

        do {
            n = hal_uart_send_dma(CONSOLE_UART, (uint8_t *)"\r", 1);
        } while (!n && xSemaphoreTake(_g_semaphore_tx, 100/*portMAX_DELAY*/) == pdTRUE);
    } else {
        do {
            n = hal_uart_send_dma(CONSOLE_UART, (uint8_t *)&ch, 1);
        } while (!n);

        if (ch != '\n')
            return ch;

        do {
            n = hal_uart_send_dma(CONSOLE_UART, (uint8_t *)"\r", 1);
        } while (!n);
    }

    return ch;
}


int io_def_uart_getchar(void)
{
    /* Blocked UART Getchar */

#if configUSE_TICKLESS_IDLE == 2
    if (tickless_handle_index == 0xFF) {
        xSleepLockTimer = xTimerCreate("xSleepLockTimer",
                                       (30000 / portTICK_PERIOD_MS),
                                       false,
                                       NULL,
                                       sleeplock_timeout_callback);

        tickless_handle_index = hal_sleep_manager_set_sleep_handle("tickless");
    }
#endif

    while (1)
    {
        uint32_t    len;

        len = hal_uart_get_available_receive_bytes(CONSOLE_UART);

        if (len > 0) {
            uint8_t ret;

            hal_uart_receive_dma(CONSOLE_UART, &ret, 1);

#ifdef HAL_SLEEP_MANAGER_ENABLED
#if configUSE_TICKLESS_IDLE == 2
#if configTICKLESS_DEEP_SLEEP
            if (1) {
#endif
            if (!(hal_sleep_manager_get_lock_status() & (1 << tickless_handle_index)))
                hal_sleep_manager_lock_sleep(tickless_handle_index);

            if (xTimerIsTimerActive(xSleepLockTimer)) {
                xTimerReset(xSleepLockTimer, 0);
            } else {
                xTimerStart(xSleepLockTimer, 0);
            }
#if configTICKLESS_DEEP_SLEEP
            }
#endif
#endif /* configUSE_TICKLESS_IDLE == 2 */
#endif

            return ret;
        } else {
            xSemaphoreTake(_g_semaphore_rx, portMAX_DELAY);
        }
    }
}

int log_write(char *buf, int len)
{
    int left;
    char last_char;

    if (!is_io_ready || len < 1) {
        return 0;
    }

    last_char = *(buf + len - 1);

    if (_uart_dma_blocking_is_safe())
    {
        left = len;

        do {
            left -= hal_uart_send_dma(CONSOLE_UART, (uint8_t *)(buf + len - left), left);
        } while (left && xSemaphoreTake(_g_semaphore_tx, 1000));

        if (last_char != '\n') {
            return len;
        }

        left = 1;

        do {
            left -= hal_uart_send_dma(CONSOLE_UART, (uint8_t *)"\r", left);
        } while (left && xSemaphoreTake(_g_semaphore_tx, 1000));
    }
    else
    {
        left = len;

        do {
            left -= hal_uart_send_dma(CONSOLE_UART, (uint8_t *)(buf + len - left), left);
        } while (left);

        if (last_char != '\n') {
            return len;
        }

        left = 1;

        do {
            left -= hal_uart_send_dma(CONSOLE_UART, (uint8_t *)"\r", left);
        } while (left);
    }

    return len;
}

void io_def_uart_init(void)
{
    hal_uart_config_t       uart_config = {
        .baudrate                       = HAL_UART_BAUDRATE_115200,
        .parity                         = HAL_UART_PARITY_NONE,
        .stop_bit                       = HAL_UART_STOP_BIT_1,
        .word_length                    = HAL_UART_WORD_LENGTH_8
    };
    hal_uart_dma_config_t   dma_config  = {
        .send_vfifo_buffer              = g_tx_vfifo,
        .send_vfifo_buffer_size         = VFIFO_TX_SIZE,
        .send_vfifo_threshold_size      = VFIFO_TX_SIZE/8,
        .receive_vfifo_buffer           = g_rx_vfifo,
        .receive_vfifo_buffer_size      = VFIFO_RX_SIZE,
        .receive_vfifo_threshold_size   = VFIFO_RX_SIZE,
        .receive_vfifo_alert_size       = VFIFO_ALERT_LENGTH
    };

    hal_uart_init(CONSOLE_UART, &uart_config);

    is_io_ready = true;

    /* initialize Semephore */
    _g_semaphore_tx = xSemaphoreCreateBinary();
    _g_semaphore_rx = xSemaphoreCreateBinary();

    hal_uart_set_dma(CONSOLE_UART, &dma_config);

    hal_uart_register_callback(CONSOLE_UART, _uart_event, NULL);
}

