/***************************************************************************//**
* \file cybt_platform_freertos.c
*
* \brief
* Implementation for BT porting interface on FreeRTOS
*
********************************************************************************
* \copyright
* Copyright 2018-2019 Cypress Semiconductor Corporation
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "wiced_data_types.h"

#include "cycfg_pins.h"
#include "cycfg_peripherals.h"
#include "cycfg_system.h"
#include "cyhal_uart.h"
#include "cyhal_gpio.h"
#include "cyhal_lptimer.h"
#include "cyhal_syspm.h"

#include "cybt_platform_task.h"
#include "cybt_platform_interface.h"
#include "cybt_platform_trace.h"
#include "cybt_platform_config.h"
#include "cybt_platform_util.h"

/******************************************************************************
 *                                Constants
 ******************************************************************************/
#define HCI_SEMAPHORE_MAX_COUNT  (1)
#define HCI_SEMAPHORE_INIT_COUNT (0)

#define PLATFORM_SLEEP_IDLE_TIMEOUT_MS     (3000)

#define SLEEP_TASK_NAME               "sleep_task"
#define SLEEP_TASK_STACK_SIZE         (1024)
#define SLEEP_TASK_PRIORITY           (CY_RTOS_PRIORITY_REALTIME)
#define SLEEP_TASK_QUEUE_COUNT        (32)
#define SLEEP_TASK_QUEUE_ITEM_SIZE    (sizeof(uint8_t))

/*****************************************************************************
 *                           Type Definitions
 *****************************************************************************/
#define SLEEP_ACT_START_IDLE_TIMER    (0x20)
#define SLEEP_ACT_STOP_IDLE_TIMER     (0x40)
#define SLEEP_ACT_EXIT_SLEEP_TASK     (0xFF)
typedef uint8_t sleep_action_t;

typedef struct 
{
    bool            inited;
    cyhal_uart_t    hal_obj;
    cy_semaphore_t  tx_complete;
    cy_semaphore_t  rx_complete;
    cy_mutex_t      tx_atomic;
    cy_mutex_t      rx_atomic;
} hci_uart_cb_t;


/******************************************************************************
 *                           Variables Definitions
 ******************************************************************************/
hci_uart_cb_t   hci_uart_cb;

// This timer is only used in active mode,
// hence it's implemented by cy_rtos_timer API .
cy_timer_t      platform_sleep_idle_timer;
bool            platform_sleep_lock = false;

cy_thread_t     sleep_timer_task;
cy_queue_t      sleep_timer_task_queue;

cyhal_lptimer_t bt_stack_lptimer;
uint8_t         lptimer_freq_shift;


/******************************************************************************
 *                          Function Declarations
 ******************************************************************************/
void cybt_idle_timer_cback(cy_timer_callback_arg_t arg);

cybt_result_t cybt_send_action_to_sleep_task(sleep_action_t action);
void cybt_sleep_timer_task(cy_thread_arg_t arg);


/******************************************************************************
 *                           Function Definitions
 ******************************************************************************/
uint8_t calculate_lptimer_freq_shift(uint32_t clock_freq)
{
    uint8_t idx;

    for (idx = 31; idx > 0; idx--)
    {
        if ((1U << idx) == clock_freq)
        {
            return idx;
        }
    }

    return 0;
}

void platform_stack_lptimer_cback(void *callback_arg, cyhal_lptimer_event_t event)
{
    BT_MSG_HDR *p_bt_msg = (BT_MSG_HDR *) cybt_platform_task_mempool_alloc(BT_MSG_HDR_SIZE);

    if(NULL == p_bt_msg)
    {
        CY_ASSERT(0);
        return;
    }

    cybt_platform_sleep_lock();

    p_bt_msg->event = BT_EVT_TO_BTU_TIMER;
    p_bt_msg->length = 0;
    cybt_send_msg_to_bt_task(p_bt_msg, true);

    cybt_platform_sleep_unlock();
}


void *cybt_platform_malloc(uint32_t req_size)
{
    return malloc((size_t) req_size);
}

void cybt_platform_free(void *p_mem_block)
{
    free(p_mem_block);
}

void cybt_platform_disable_irq(void)
{
    __disable_irq();
}

void cybt_platform_enable_irq(void)
{
    __enable_irq();
}

void cybt_platform_init(void)
{
    cy_rtos_init_timer(&platform_sleep_idle_timer,
                       CY_TIMER_TYPE_ONCE,
                       cybt_idle_timer_cback,
                       0
                      );
    MAIN_TRACE_DEBUG("cybt_platform_init(): platform_sleep_idle_timer = 0x%x", &platform_sleep_idle_timer);

    cyhal_lptimer_init(&bt_stack_lptimer);
    cyhal_lptimer_enable_event(&bt_stack_lptimer,
                               CYHAL_LPTIMER_COMPARE_MATCH,
                               CYHAL_ISR_PRIORITY_DEFAULT,
                               false
                              );
    cyhal_lptimer_register_callback(&bt_stack_lptimer, &platform_stack_lptimer_cback, NULL);

    lptimer_freq_shift = calculate_lptimer_freq_shift(CY_CFG_SYSCLK_CLKLF_FREQ_HZ);

    cy_rtos_init_queue(&sleep_timer_task_queue,
                       SLEEP_TASK_QUEUE_COUNT,
                       SLEEP_TASK_QUEUE_ITEM_SIZE
                      );

    cy_rtos_create_thread(&sleep_timer_task,
                          cybt_sleep_timer_task,
                          SLEEP_TASK_NAME,
                          NULL,
                          SLEEP_TASK_STACK_SIZE,
                          SLEEP_TASK_PRIORITY,
                          (cy_thread_arg_t) 0
                         );
}

void cybt_platform_deinit(void)
{
    MAIN_TRACE_DEBUG("cybt_platform_deinit()");

    cybt_send_action_to_sleep_task(SLEEP_ACT_EXIT_SLEEP_TASK);

    cyhal_lptimer_free(&bt_stack_lptimer);

    cy_rtos_deinit_timer(&platform_sleep_idle_timer);
}

void cybt_platform_sleep_lock(void)
{
    cyhal_uart_event_t enable_irq_event = (cyhal_uart_event_t)(CYHAL_UART_IRQ_RX_DONE
                                           | CYHAL_UART_IRQ_TX_DONE
                                           | CYHAL_UART_IRQ_RX_NOT_EMPTY
                                          );

    cybt_platform_disable_irq();

    if(false == platform_sleep_lock)
    {
        cyhal_syspm_lock_deepsleep();

        platform_sleep_lock = true;

        cyhal_uart_enable_event(&hci_uart_cb.hal_obj,
                                enable_irq_event,
                                CYHAL_ISR_PRIORITY_DEFAULT,
                                true
                               );
    }

    cybt_send_action_to_sleep_task(SLEEP_ACT_STOP_IDLE_TIMER);
    cybt_platform_enable_irq();
}

void cybt_platform_sleep_unlock(void)
{
    cybt_platform_disable_irq();

    if(true == platform_sleep_lock)
    {
        cybt_send_action_to_sleep_task(SLEEP_ACT_START_IDLE_TIMER);
        cybt_platform_enable_irq();
    }
    else
    {
        cybt_platform_enable_irq();
    }
}

uint64_t cybt_platform_get_tick_count_us(void)
{
    static uint64_t   abs_tick_cnt_hi = 0;
    static uint32_t   last_tick_cnt = 0;
    static uint64_t   lptick_remainder = 0;

    uint32_t          cur_time_in_lpticks = 0;
    uint64_t          cur_time_in_lpticks64 = 0;
    uint64_t          cur_time_in_us = 0;
    uint64_t          temp_cnt64 = 0;

    cur_time_in_lpticks = cyhal_lptimer_read(&bt_stack_lptimer);

    if (cur_time_in_lpticks < last_tick_cnt)
    {
        abs_tick_cnt_hi += 0x100000000;
    }

    last_tick_cnt = cur_time_in_lpticks;
    cur_time_in_lpticks64 = cur_time_in_lpticks + abs_tick_cnt_hi;

    // convert tick to us
    temp_cnt64 = cur_time_in_lpticks64 * 1000000;
    cur_time_in_us = temp_cnt64 >> lptimer_freq_shift;

    lptick_remainder += temp_cnt64 - (cur_time_in_us << lptimer_freq_shift);
    if(lptick_remainder > CY_CFG_SYSCLK_CLKLF_FREQ_HZ)
    {
        cur_time_in_us += 1;
        lptick_remainder -= CY_CFG_SYSCLK_CLKLF_FREQ_HZ;
    }

    return cur_time_in_us;
}

void cybt_platform_set_next_timeout(uint64_t abs_tick_us_to_expire)
{
    uint64_t curr_time_in_us = cybt_platform_get_tick_count_us();
    uint64_t time_to_expire_in_us = abs_tick_us_to_expire - curr_time_in_us;
    uint32_t time_to_expire_in_lpticks;

    if(abs_tick_us_to_expire <= curr_time_in_us)
    {
        // Already expired...
        BT_MSG_HDR *p_bt_msg = (BT_MSG_HDR *) cybt_platform_task_mempool_alloc(BT_MSG_HDR_SIZE);
        if(p_bt_msg)
        {
            p_bt_msg->event = BT_EVT_TO_BTU_TIMER;
            p_bt_msg->length = 0;
            cybt_send_msg_to_bt_task(p_bt_msg, false);
        }
        else
        {
            SPIF_TRACE_ERROR("set_next_timeout(): Fail to allocate memory");
        }

        return;
    }

    // convert us to tick
    time_to_expire_in_lpticks = ((time_to_expire_in_us << lptimer_freq_shift) + 1000000 - 1) / 1000000;

    cyhal_lptimer_set_delay(&bt_stack_lptimer, time_to_expire_in_lpticks);
}

void cybt_platform_log_print(const char *fmt_str, ...)
{
    char buffer[CYBT_TRACE_BUFFER_SIZE];
    va_list ap;
    cy_time_t time;

    cy_rtos_get_time(&time);

    va_start(ap, fmt_str);
    vsnprintf(buffer, CYBT_TRACE_BUFFER_SIZE, fmt_str, ap);
    va_end(ap);

    printf("[%lu] %s\r\n", (uint32_t)time, buffer);
}

static void cybt_uart_rx_not_empty(void)
{
    uint32_t data_ready_evt = BT_EVT_TO_HCI_DATA_READY_UNKNOWN;
    cybt_result_t result;

    cyhal_uart_enable_event(&hci_uart_cb.hal_obj,
                            CYHAL_UART_IRQ_RX_NOT_EMPTY,
                            CYHAL_ISR_PRIORITY_DEFAULT,
                            false
                           );

    result = cybt_send_msg_to_hci_task((BT_MSG_HDR *)data_ready_evt, true);
    if(CYBT_SUCCESS != result)
    {
        // Somehow hci task is unable to receive message, 
        // enable rx interrupt to wait for next one
        cyhal_uart_enable_event(&hci_uart_cb.hal_obj,
                                CYHAL_UART_IRQ_RX_NOT_EMPTY,
                                CYHAL_ISR_PRIORITY_DEFAULT,
                                true
                               );
    }
}

static void cybt_uart_tx_done_irq(void)
{
    cy_rtos_set_semaphore(&hci_uart_cb.tx_complete, true);
}

static void cybt_uart_rx_done_irq(void)
{
    cy_rtos_set_semaphore(&hci_uart_cb.rx_complete, true);
}

static void cybt_uart_irq_handler(void *handler_arg, cyhal_uart_event_t event)
{
    switch(event)
    {
        case CYHAL_UART_IRQ_RX_NOT_EMPTY:
            cybt_uart_rx_not_empty();
            break;
        case CYHAL_UART_IRQ_TX_DONE:
            cybt_uart_tx_done_irq();
            break;
        case CYHAL_UART_IRQ_RX_DONE:
            cybt_uart_rx_done_irq();
            break;
        default:
            break;
    }
}

void cybt_platform_assert_bt_wake(void)
{
    if(true == cybt_platform_get_sleep_mode_status())
    {
        bool wake_polarity;
        const cybt_platform_config_t *p_bt_platform_cfg = cybt_platform_get_config();

        switch(p_bt_platform_cfg->controller_config.sleep_mode.device_wake_polarity)
        {
            case CYBT_WAKE_ACTIVE_LOW:
                wake_polarity = false;
                break;
            case CYBT_WAKE_ACTIVE_HIGH:
                wake_polarity = true;
                break;
            default:
                HCIDRV_TRACE_ERROR("ASSERT_BT_WAKE: unknown polarity (%d)",
                                   p_bt_platform_cfg->controller_config.sleep_mode.device_wake_polarity
                                  );
                return;
        }

        cyhal_gpio_write(p_bt_platform_cfg->controller_config.sleep_mode.device_wakeup_pin,
                         wake_polarity
                        );
    }
}

void cybt_platform_deassert_bt_wake(void)
{
    if(true == cybt_platform_get_sleep_mode_status())
    {
        bool sleep_polarity;
        const cybt_platform_config_t *p_bt_platform_cfg = cybt_platform_get_config();

        switch(p_bt_platform_cfg->controller_config.sleep_mode.device_wake_polarity)
        {
            case CYBT_WAKE_ACTIVE_LOW:
                sleep_polarity = true;
                break;
            case CYBT_WAKE_ACTIVE_HIGH:
                sleep_polarity = false;
                break;
            default:
                HCIDRV_TRACE_ERROR("DEASSERT_BT_WAKE: unknown polarity (%d)",
                                   p_bt_platform_cfg->controller_config.sleep_mode.device_wake_polarity
                                  );
                return;
        }

        cyhal_gpio_write(p_bt_platform_cfg->controller_config.sleep_mode.device_wakeup_pin,
                         sleep_polarity
                        );
    }
}


void cybt_host_wake_irq_handler(void *callback_arg, cyhal_gpio_event_t event)
{
    const cybt_platform_config_t *p_bt_platform_cfg = cybt_platform_get_config();

    switch(event)
    {
        case CYHAL_GPIO_IRQ_RISE:
            if(CYBT_WAKE_ACTIVE_HIGH == p_bt_platform_cfg->controller_config.sleep_mode.host_wake_polarity)
            {
                cybt_platform_sleep_lock();
            }
            else
            {
                cybt_platform_sleep_unlock();
            }
            break;
        case CYHAL_GPIO_IRQ_FALL:
            if(CYBT_WAKE_ACTIVE_LOW == p_bt_platform_cfg->controller_config.sleep_mode.host_wake_polarity)
            {
                cybt_platform_sleep_lock();
            }
            else
            {
                cybt_platform_sleep_unlock();
            }
            break;
        default:
            break;
    }
}

void cybt_idle_timer_cback(cy_timer_callback_arg_t arg)
{
    cybt_platform_disable_irq();

    cyhal_syspm_unlock_deepsleep();
    platform_sleep_lock = false;

    cybt_platform_enable_irq();
}

cybt_result_t cybt_platform_hci_open(void)
{
    cyhal_uart_event_t enable_irq_event = (cyhal_uart_event_t)(CYHAL_UART_IRQ_RX_DONE
                                           | CYHAL_UART_IRQ_TX_DONE
                                           | CYHAL_UART_IRQ_RX_NOT_EMPTY
                                          );
    uint32_t actual_baud_rate;
    cy_rslt_t result;
    cyhal_uart_cfg_t bt_uart_cfg = {0};
    const cybt_platform_config_t *p_bt_platform_cfg = cybt_platform_get_config();

    if(true == hci_uart_cb.inited)
    {
        return  CYBT_SUCCESS;
    }
    
    memset(&hci_uart_cb, 0, sizeof(hci_uart_cb_t));

    cy_rtos_init_semaphore(&hci_uart_cb.tx_complete,
                           HCI_SEMAPHORE_MAX_COUNT,
                           HCI_SEMAPHORE_INIT_COUNT
                          );
    cy_rtos_init_semaphore(&hci_uart_cb.rx_complete,
                           HCI_SEMAPHORE_MAX_COUNT,
                           HCI_SEMAPHORE_INIT_COUNT
                          );
    cy_rtos_init_mutex(&hci_uart_cb.tx_atomic);
    cy_rtos_init_mutex(&hci_uart_cb.rx_atomic);

    if(NC != p_bt_platform_cfg->controller_config.sleep_mode.host_wakeup_pin)
    {
        cyhal_gpio_init(p_bt_platform_cfg->controller_config.sleep_mode.host_wakeup_pin,
                        CYHAL_GPIO_DIR_INPUT,
                        CYHAL_GPIO_DRIVE_NONE,
                        0
                       );
    }

    if((CYBT_SLEEP_MODE_ENABLED == p_bt_platform_cfg->controller_config.sleep_mode.sleep_mode_enabled)
       && (NC != p_bt_platform_cfg->controller_config.sleep_mode.device_wakeup_pin)
       && (NC != p_bt_platform_cfg->controller_config.sleep_mode.host_wakeup_pin)
      )
    {
        cyhal_gpio_event_t event;

        if(CYBT_WAKE_ACTIVE_HIGH == p_bt_platform_cfg->controller_config.sleep_mode.host_wake_polarity)
        {
            event = CYHAL_GPIO_IRQ_RISE;
        }
        else if(CYBT_WAKE_ACTIVE_LOW == p_bt_platform_cfg->controller_config.sleep_mode.host_wake_polarity)
        {
            event = CYHAL_GPIO_IRQ_FALL;
        }
        else
        {
            HCIDRV_TRACE_ERROR("hci_open(): Unknown host wake polarity(%d)",
                               p_bt_platform_cfg->controller_config.sleep_mode.host_wake_polarity
                              );
            return  CYBT_ERR_GENERIC;
        }

        cyhal_gpio_register_callback(p_bt_platform_cfg->controller_config.sleep_mode.host_wakeup_pin,
                                     cybt_host_wake_irq_handler,
                                     NULL
                                    );
        cyhal_gpio_enable_event(p_bt_platform_cfg->controller_config.sleep_mode.host_wakeup_pin,
                                event,
                                CYHAL_ISR_PRIORITY_DEFAULT,
                                true
                               );
    }

    if(NC != p_bt_platform_cfg->controller_config.sleep_mode.device_wakeup_pin)
    {
        cyhal_gpio_init(p_bt_platform_cfg->controller_config.sleep_mode.device_wakeup_pin,
                        CYHAL_GPIO_DIR_OUTPUT,
                        CYHAL_GPIO_DRIVE_STRONG,
                        0
                       );
        cyhal_gpio_write(p_bt_platform_cfg->controller_config.sleep_mode.device_wakeup_pin,
                         false
                        );
        cy_rtos_delay_milliseconds(100);
    }

    cyhal_gpio_init(p_bt_platform_cfg->controller_config.bt_power_pin,
                    CYHAL_GPIO_DIR_OUTPUT,
                    CYHAL_GPIO_DRIVE_PULLUP,
                    1
                   );
    cyhal_gpio_write(p_bt_platform_cfg->controller_config.bt_power_pin,
                     true
                    );
    cy_rtos_delay_milliseconds(500);

    bt_uart_cfg.data_bits = p_bt_platform_cfg->hci_config.hci.hci_uart.data_bits;
    bt_uart_cfg.stop_bits = p_bt_platform_cfg->hci_config.hci.hci_uart.stop_bits;
    bt_uart_cfg.parity = p_bt_platform_cfg->hci_config.hci.hci_uart.parity;
    bt_uart_cfg.rx_buffer = NULL;
    bt_uart_cfg.rx_buffer_size = 0;
    result = cyhal_uart_init(&hci_uart_cb.hal_obj,
                             p_bt_platform_cfg->hci_config.hci.hci_uart.uart_tx_pin,
                             p_bt_platform_cfg->hci_config.hci.hci_uart.uart_rx_pin,
                             NULL,
                             &bt_uart_cfg
                            );
    if(CY_RSLT_SUCCESS != result)
    {
        HCIDRV_TRACE_ERROR("hci_open(): init error (0x%x)", result);
        return  CYBT_ERR_HCI_INIT_FAILED;
    }

    result = cyhal_uart_set_baud(&hci_uart_cb.hal_obj,
                                 HCI_UART_DEFAULT_BAUDRATE,
                                 &actual_baud_rate
                                );
    if(CY_RSLT_SUCCESS != result)
    {
        HCIDRV_TRACE_ERROR("hci_open(): Set baud rate failed (0x%x)",
                           result
                          );
        return  CYBT_ERR_HCI_SET_BAUDRATE_FAILED;
    }
    HCIDRV_TRACE_DEBUG("hci_open(): act baud rate  = %d", actual_baud_rate);

    if(true == p_bt_platform_cfg->hci_config.hci.hci_uart.flow_control)
    {
        result= cyhal_uart_set_flow_control(&hci_uart_cb.hal_obj,
                                            p_bt_platform_cfg->hci_config.hci.hci_uart.uart_cts_pin,
                                            p_bt_platform_cfg->hci_config.hci.hci_uart.uart_rts_pin
                                           );
        if(CY_RSLT_SUCCESS != result)
        {
            HCIDRV_TRACE_ERROR("hci_open(): Set flow control failed (0x%x)",
                               result
                              );
            return  CYBT_ERR_HCI_SET_FLOW_CTRL_FAILED;
        }
    }

    cyhal_uart_register_callback(&hci_uart_cb.hal_obj,
                                 cybt_uart_irq_handler,
                                 NULL
                                );

    cyhal_uart_enable_event(&hci_uart_cb.hal_obj,
                            enable_irq_event,
                            CYHAL_ISR_PRIORITY_DEFAULT,
                            true
                           );

    HCIDRV_TRACE_DEBUG("hci_open(): Wait CTS low");
    while(true == cyhal_gpio_read(p_bt_platform_cfg->hci_config.hci.hci_uart.uart_cts_pin))
    {
        cy_rtos_delay_milliseconds(10);
    }

    hci_uart_cb.inited = true;

    HCIDRV_TRACE_DEBUG("hci_open(): Done");

    return  CYBT_SUCCESS;
}

cybt_result_t cybt_platform_hci_set_baudrate(uint32_t baudrate)
{
    uint32_t actual_baud;
    cy_rslt_t result = cyhal_uart_set_baud(&hci_uart_cb.hal_obj, baudrate, &actual_baud);

    if(false == hci_uart_cb.inited)
    {
        HCIDRV_TRACE_ERROR("set_baudrate(): UART is NOT initialized");
        return  CYBT_ERR_HCI_NOT_INITIALIZE;
    }

    if(CY_RSLT_SUCCESS == result)
    {
        HCIDRV_TRACE_ERROR("set_baudrate(): SUCCESS, req = %d, actual = %d",
                           baudrate,
                           actual_baud
                          );
        return  CYBT_SUCCESS;
    }
    else
    {
        HCIDRV_TRACE_ERROR("set_baudrate(): FAILED(0x%x)", result);
        return  CYBT_ERR_HCI_SET_BAUDRATE_FAILED;
    }
}

cybt_result_t cybt_platform_hci_write(hci_packet_type_t type,
                                                  uint8_t          *p_data,
                                                  uint32_t         length
                                                 )
{
    cy_rslt_t result;
    cybt_result_t return_status =  CYBT_SUCCESS;
    
    if(false == hci_uart_cb.inited)
    {
        HCIDRV_TRACE_ERROR("hci_write(): UART is NOT initialized");
        return  CYBT_ERR_HCI_NOT_INITIALIZE;
    }
    
    result = cy_rtos_get_mutex(&hci_uart_cb.tx_atomic, CY_RTOS_NEVER_TIMEOUT);
    if(CY_RSLT_SUCCESS != result)
    {
        HCIDRV_TRACE_ERROR("hci_write(): Get mutex error (0x%x)\n", result);
        return  CYBT_ERR_HCI_GET_TX_MUTEX_FAILED;
    }

    cybt_platform_sleep_lock();
    cybt_platform_assert_bt_wake();

    result = cyhal_uart_write_async(&hci_uart_cb.hal_obj,
                                    (void *) p_data,
                                    (size_t) length
                                   );
    if(CY_RSLT_SUCCESS == result)
    {
        cy_rtos_get_semaphore(&hci_uart_cb.tx_complete, CY_RTOS_NEVER_TIMEOUT, false);
    }
    else
    {
        HCIDRV_TRACE_ERROR("hci_write(): failure (0x%x)\n", result);
        return_status =  CYBT_ERR_HCI_WRITE_FAILED;
    }

    cybt_platform_deassert_bt_wake();
    cybt_platform_sleep_unlock();

    cy_rtos_set_mutex(&hci_uart_cb.tx_atomic);
        
    return return_status;
}

cybt_result_t cybt_platform_hci_read(hci_packet_type_t type,
                                                uint8_t           *p_data,
                                                uint32_t          *p_length,
                                                uint32_t          timeout_ms
                                               )
{
    uint32_t  req_len = *p_length;
    cy_rslt_t result;
    cybt_result_t return_status;

    if(false == hci_uart_cb.inited)
    {
        HCIDRV_TRACE_ERROR("hci_read(): UART is NOT initialized");
        return  CYBT_ERR_HCI_NOT_INITIALIZE;
    }

    result = cy_rtos_get_mutex(&hci_uart_cb.rx_atomic, timeout_ms);
    if(CY_RSLT_SUCCESS != result)
    {
        HCIDRV_TRACE_ERROR("hci_read(): Get mutex error (0x%x)\n", result);
        return  CYBT_ERR_HCI_GET_RX_MUTEX_FAILED;
    }

    cybt_platform_sleep_lock();

    if(0 < timeout_ms)
    {
        result = cyhal_uart_read_async(&hci_uart_cb.hal_obj,
                                       (void *) p_data,
                                       (size_t) req_len
                                      );
        if(CY_RSLT_SUCCESS == result)
        {
            result = cy_rtos_get_semaphore(&hci_uart_cb.rx_complete,
                                           timeout_ms,
                                           false
                                          );
        }
        else
        {
            HCIDRV_TRACE_ERROR("hci_read(): failure code = 0x%x\n", result);

            cybt_platform_sleep_unlock();
            cy_rtos_set_mutex(&hci_uart_cb.rx_atomic);
            return  CYBT_ERR_HCI_READ_FAILED;
        }
    
        if(CY_RSLT_SUCCESS == result)
        {
            return_status = CYBT_SUCCESS;
        }
        else
        {
            HCIDRV_TRACE_ERROR("hci_read(): failed (0x%x), read size = %d",
                               result,
                               hci_uart_cb.hal_obj.context.rxBufIdx
                              );
    
            if(CY_RTOS_TIMEOUT == result)
            {
                return_status =  CYBT_ERR_TIMEOUT;
            }
            else
            {
                return_status =  CYBT_ERR_GENERIC;
            }
            *p_length = hci_uart_cb.hal_obj.context.rxBufIdx;
    
            cyhal_uart_read_abort(&hci_uart_cb.hal_obj);
        }
    }
    else
    {
        result = cyhal_uart_read(&hci_uart_cb.hal_obj,
                                 (void *) p_data,
                                 (size_t *)p_length
                                );
        if(CY_RSLT_SUCCESS == result)
        {
            return_status = CYBT_SUCCESS;
        }
        else
        {
            return_status =  CYBT_ERR_HCI_READ_FAILED;
        }
    }

    cybt_platform_sleep_unlock();
    cy_rtos_set_mutex(&hci_uart_cb.rx_atomic);

    return return_status;

}

cybt_result_t cybt_platform_hci_close(void)
{
    cyhal_uart_event_t enable_irq_event = (cyhal_uart_event_t)(CYHAL_UART_IRQ_RX_DONE
                                           | CYHAL_UART_IRQ_TX_DONE
                                           | CYHAL_UART_IRQ_RX_NOT_EMPTY
                                          );
    const cybt_platform_config_t *p_bt_platform_cfg = cybt_platform_get_config();
    if(false == hci_uart_cb.inited)
    {
        HCIDRV_TRACE_ERROR("hci_close(): Not inited\n");
        return  CYBT_ERR_HCI_NOT_INITIALIZE;
    }

    cyhal_uart_enable_event(&hci_uart_cb.hal_obj,
                            enable_irq_event,
                            CYHAL_ISR_PRIORITY_DEFAULT,
                            false
                           );
    cyhal_uart_register_callback(&hci_uart_cb.hal_obj,
                                 NULL,
                                 NULL
                                );
    cyhal_uart_free(&hci_uart_cb.hal_obj);

    if((CYBT_SLEEP_MODE_ENABLED == p_bt_platform_cfg->controller_config.sleep_mode.sleep_mode_enabled)
       && (NC != p_bt_platform_cfg->controller_config.sleep_mode.device_wakeup_pin)
       && (NC != p_bt_platform_cfg->controller_config.sleep_mode.host_wakeup_pin)
      )
    {
        cyhal_gpio_enable_event(p_bt_platform_cfg->controller_config.sleep_mode.host_wakeup_pin,
                                CYHAL_GPIO_IRQ_NONE,
                                CYHAL_ISR_PRIORITY_DEFAULT,
                                true
                               );
        cyhal_gpio_register_callback(p_bt_platform_cfg->controller_config.sleep_mode.host_wakeup_pin,
                                     NULL,
                                     NULL
                                    );
    }

    if(NC != p_bt_platform_cfg->controller_config.sleep_mode.device_wakeup_pin)
    {
        cyhal_gpio_free(p_bt_platform_cfg->controller_config.sleep_mode.device_wakeup_pin);
    }

    if(NC != p_bt_platform_cfg->controller_config.sleep_mode.host_wakeup_pin)
    {
        cyhal_gpio_free(p_bt_platform_cfg->controller_config.sleep_mode.host_wakeup_pin);
    }

    cy_rtos_deinit_mutex(&hci_uart_cb.tx_atomic);
    cy_rtos_deinit_mutex(&hci_uart_cb.rx_atomic);
    cy_rtos_deinit_semaphore(&hci_uart_cb.tx_complete);
    cy_rtos_deinit_semaphore(&hci_uart_cb.rx_complete);

    cyhal_gpio_write(p_bt_platform_cfg->controller_config.bt_power_pin,
                     false
                    );
    cyhal_gpio_free(p_bt_platform_cfg->controller_config.bt_power_pin);

    memset(&hci_uart_cb, 0, sizeof(hci_uart_cb_t));

    return  CYBT_SUCCESS; 
}

void cybt_platform_hci_irq_rx_data_ind(bool enable)
{
    cyhal_uart_enable_event(&hci_uart_cb.hal_obj,
                            CYHAL_UART_IRQ_RX_NOT_EMPTY,
                            CYHAL_ISR_PRIORITY_DEFAULT,
                            enable
                           );
}

cybt_result_t cybt_send_action_to_sleep_task(sleep_action_t action)
{
    bool is_from_isr = (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;

    cy_rslt_t result = cy_rtos_put_queue(&sleep_timer_task_queue,
                                         (void *)&action,
                                         0,
                                         is_from_isr
                                        );

    if(CY_RSLT_SUCCESS != result)
    {
        CY_ASSERT(0);
        return CYBT_ERR_SEND_QUEUE_FAILED;
    }

    return CYBT_SUCCESS;
}

void cybt_sleep_timer_task(cy_thread_arg_t arg)
{
    cy_rslt_t result;
    sleep_action_t action = 0;

    while(1)
    {
        result = cy_rtos_get_queue(&sleep_timer_task_queue,
                                   (void *)&action,
                                   CY_RTOS_NEVER_TIMEOUT,
                                   false
                                  );

        if(CY_RSLT_SUCCESS != result)
        {
            MAIN_TRACE_WARNING("sleep_task(): queue error (0x%x)",
                                result
                              );
            continue;
        }

        if (SLEEP_ACT_EXIT_SLEEP_TASK == action)
        {
            cy_rtos_deinit_queue(&sleep_timer_task_queue);
            break;
        }

        switch(action)
        {
            case SLEEP_ACT_START_IDLE_TIMER:
                cy_rtos_start_timer(&platform_sleep_idle_timer,
                                    PLATFORM_SLEEP_IDLE_TIMEOUT_MS
                                   );
                break;
            case SLEEP_ACT_STOP_IDLE_TIMER:
                {
                    bool is_timer_running = false;
                    cy_rslt_t result;

                    result = cy_rtos_is_running_timer(&platform_sleep_idle_timer,
                                                      &is_timer_running
                                                     );
                    if(CY_RSLT_SUCCESS == result && true == is_timer_running)
                    {
                        cy_rtos_stop_timer(&platform_sleep_idle_timer);
                    }
                }
                break;
            default:
                MAIN_TRACE_ERROR("sleep_task(): Unknown action = 0x%x", action);
                break;
        }
    }
    cy_rtos_exit_thread();
}

