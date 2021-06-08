/*******************************************************************************
* \file cybt_platform_task.c
*
* \brief
* Provides functions for OS task communication.
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

#include "cyabs_rtos.h"

#include "cybt_platform_config.h"
#include "cybt_platform_task.h"
#include "cybt_platform_interface.h"
#include "cybt_platform_util.h"

#include "cybt_platform_trace.h"

#include "wiced_memory.h"


/******************************************************************************
 *                           Variables Definitions
 ******************************************************************************/
cy_thread_t cybt_task[BT_TASK_NUM] = {0};
cy_queue_t  cybt_task_queue[BT_TASK_NUM] = {0};

wiced_bt_heap_t *gp_task_heap = NULL;


/******************************************************************************
 *                           Function Definitions
 ******************************************************************************/
cybt_result_t cybt_platform_task_init(void)
{
    extern void cybt_bt_task(cy_thread_arg_t arg);
    extern void cybt_hci_task(cy_thread_arg_t arg);

    cy_rslt_t cy_result;
    cybt_result_t task_result;
    const cybt_platform_config_t *p_bt_platform_cfg = cybt_platform_get_config();

    uint32_t task_pool_size = p_bt_platform_cfg->task_mem_pool_size;
    if(0 == task_pool_size)
    {
        task_pool_size = CYBT_TASK_DEFAULT_POOL_SIZE;
    }

    task_result = cybt_platform_task_mempool_init(task_pool_size);
    if(CYBT_SUCCESS != task_result)
    {
        MAIN_TRACE_ERROR("task_init(): Create mempool failed (0x%x)",
                         task_result
                        );
        return CYBT_ERR_INIT_MEMPOOL_FAILED;
    }

    cy_result = cy_rtos_init_queue(&BTU_TASK_QUEUE,
                                   BTU_TASK_QUEUE_COUNT,
                                   BTU_TASK_QUEUE_ITEM_SIZE
                                  );
    if(CY_RSLT_SUCCESS != cy_result)
    {
        MAIN_TRACE_ERROR("task_init(): Init bt task queue failed (0x%x)",
                         cy_result
                        );
        return CYBT_ERR_INIT_QUEUE_FAILED;
    }

    cy_result = cy_rtos_init_queue(&HCI_TASK_QUEUE,
                                   HCI_TASK_QUEUE_COUNT,
                                   HCI_TASK_QUEUE_ITEM_SIZE
                                  );
    if(CY_RSLT_SUCCESS != cy_result)
    {
        MAIN_TRACE_ERROR("task_init(): Init hci task queue failed (0x%x)",
                         cy_result
                        );
        return CYBT_ERR_INIT_QUEUE_FAILED;
    }

    cy_result = cy_rtos_create_thread(&cybt_task[BT_TASK_ID_BTU],
                                      cybt_bt_task,
                                      BT_TASK_NAME_BTU,
                                      NULL,
                                      BTU_TASK_STACK_SIZE,
                                      BTU_TASK_PRIORITY,
                                      (cy_thread_arg_t) NULL
                                     );
    if(CY_RSLT_SUCCESS != cy_result)
    {
        MAIN_TRACE_ERROR("task_init(): Create bt task failed (0x%x)",
                         cy_result
                        );
        return CYBT_ERR_CREATE_TASK_FAILED;
    }

    cy_result = cy_rtos_create_thread(&cybt_task[BT_TASK_ID_HCI],
                                      cybt_hci_task,
                                      BT_TASK_NAME_HCI,
                                      NULL,
                                      HCI_TASK_STACK_SIZE,
                                      HCI_TASK_PRIORITY,
                                      (cy_thread_arg_t) NULL
                                     );
    if(CY_RSLT_SUCCESS != cy_result)
    {
        MAIN_TRACE_ERROR("task_init(): Create hci task failed (0x%x)",
                         cy_result
                        );
        return CYBT_ERR_CREATE_TASK_FAILED;
    }

    return CYBT_SUCCESS;
}

cybt_result_t cybt_platform_task_deinit(void)
{
    uint8_t idx;
    cy_rslt_t result;

    MAIN_TRACE_DEBUG("cybt_platform_task_deinit()");

    for(idx = 0; idx < BT_TASK_NUM; idx++)
    {
        uint32_t task_shutdown_evt = BT_EVT_TASK_SHUTDOWN;

        result = cy_rtos_put_queue(&cybt_task_queue[idx],
                                   (void *)&task_shutdown_evt,
                                   0,
                                   false
                                  );
        if(CY_RSLT_SUCCESS != result)
        {
            MAIN_TRACE_ERROR("task_deinit(): send queue failure (0x%x, task = %d)",
                             result,
                             idx
                            );

            return CYBT_ERR_SEND_QUEUE_FAILED;
        }
    }

    cybt_platform_task_mempool_deinit();

    return CYBT_SUCCESS;
}

cybt_result_t cybt_platform_task_mempool_init(uint32_t total_size)
{
    void *p_heap_mem = NULL;

    MEM_TRACE_DEBUG("task_mempool_init(): size = %d", total_size);

    p_heap_mem = (wiced_bt_heap_t *)cybt_platform_malloc(total_size);
    gp_task_heap = wiced_bt_create_heap("CYBT_TASK_POOL",
                                        p_heap_mem,
                                        total_size,
                                        NULL,
                                        WICED_FALSE
                                       );

    if(NULL == gp_task_heap)
    {
        MEM_TRACE_ERROR("task_mempool_init(): Create heap failed");
        return CYBT_ERR_OUT_OF_MEMORY;
    }

    return CYBT_SUCCESS;
}

void *cybt_platform_task_mempool_alloc(uint32_t req_size)
{
    void *p_mem_block;

    if(NULL == gp_task_heap)
    {
        MEM_TRACE_ERROR("task_memory_alloc(): Invalid Heap");
        return NULL;
    }

    cybt_platform_disable_irq();

    p_mem_block = (void *) wiced_bt_get_buffer_from_heap(gp_task_heap, req_size);

    cybt_platform_enable_irq();

    return p_mem_block;
}

void cybt_platform_task_mempool_free(void *p_mem_block)
{
    if(NULL == gp_task_heap)
    {
        MEM_TRACE_ERROR("task_memory_free(): Invalid Heap");
        return;
    }

    cybt_platform_disable_irq();

    wiced_bt_free_buffer((wiced_bt_buffer_t *) p_mem_block);

    cybt_platform_enable_irq();
}

void cybt_platform_task_mempool_deinit(void)
{
    MEM_TRACE_DEBUG("task_mempool_deinit()");

    wiced_bt_delete_heap(gp_task_heap);
    cybt_platform_free(gp_task_heap);
    gp_task_heap = NULL;
}

uint8_t cybt_platform_task_queue_utilization(uint8_t task_id)
{
    size_t     item_cnt_in_queue = 0;
    cy_rslt_t  result;
    uint16_t   queue_total_cnt = 0;

    switch(task_id)
    {
        case BT_TASK_ID_BTU:
            queue_total_cnt = BTU_TASK_QUEUE_COUNT;
            break;
        case BT_TASK_ID_HCI:
            queue_total_cnt = HCI_TASK_QUEUE_COUNT;
            break;
        default:
            MAIN_TRACE_ERROR("task_queue_utilization(): unknown task (%d)", task_id);
            return 0;
    }

    result = cy_rtos_count_queue(&cybt_task_queue[task_id], &item_cnt_in_queue);
    if(CY_RSLT_SUCCESS != result)
    {
        MAIN_TRACE_ERROR("task_queue_utilization(): get count failed (0x%x)", result);
        return CYBT_INVALID_QUEUE_UTILIZATION;
    }

    return (item_cnt_in_queue * 100 / queue_total_cnt);
}


