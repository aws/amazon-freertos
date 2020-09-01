/*******************************************************************************
* \file cybt_bt_task.c
*
* \brief
* Implement BT task which handles HCI packet from HCI task, and timer interrupt.
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

#include "stdbool.h"

#include "cyabs_rtos.h"

#include "cybt_platform_task.h"
#include "cybt_platform_hci.h"
#include "cybt_platform_trace.h"

#include "wiced_bt_stack_platform.h"


/******************************************************************************
 *                                Constants
 ******************************************************************************/
#define BT_TASK_QUEUE_UTIL_HIGH_THRESHHOLD    (90)


/******************************************************************************
 *                           Variables Definitions
 ******************************************************************************/
uint32_t bt_task_dropped_packet_cnt = 0;


/******************************************************************************
 *                          Function Declarations
 ******************************************************************************/
extern void cybt_core_stack_init(void);
extern void host_stack_platform_interface_deinit(void);


/******************************************************************************
 *                           Function Definitions
 ******************************************************************************/
cybt_result_t cybt_send_msg_to_bt_task(BT_MSG_HDR *p_bt_msg,
                                                   bool is_from_isr
                                                  )
{
    cy_rslt_t result;
    uint8_t util_in_percentage = cybt_platform_task_queue_utilization(BT_TASK_ID_BTU);

    if(NULL == p_bt_msg)
    {
        return CYBT_ERR_BADARG;
    }

    if(BT_TASK_QUEUE_UTIL_HIGH_THRESHHOLD < util_in_percentage)
    {
        if(p_bt_msg->event == BT_EVT_TO_BTU_ACL ||
           p_bt_msg->event == BT_EVT_TO_BTU_SCO
          )
        {
            bt_task_dropped_packet_cnt++;

            if(false == is_from_isr)
            {
                BTTASK_TRACE_ERROR("send_msg_to_bt_task(): almost full (usage = %d%%), drop count = %d",
                                   util_in_percentage,
                                   bt_task_dropped_packet_cnt
                                  );
            }
            cybt_platform_task_mempool_free((void *) p_bt_msg);

            return CYBT_ERR_QUEUE_ALMOST_FULL;
        }
    }

    result = cy_rtos_put_queue(&BTU_TASK_QUEUE, (void *) &p_bt_msg, 0, is_from_isr);

    if(CY_RSLT_SUCCESS != result)
    {
        bt_task_dropped_packet_cnt++;

        if(false == is_from_isr)
        {
            BTTASK_TRACE_ERROR("send_msg_to_bt_task(): send failure (0x%x)", result);
        }
        cybt_platform_task_mempool_free((void *)p_bt_msg);

        return CYBT_ERR_SEND_QUEUE_FAILED;
    }

    return CYBT_SUCCESS;
}

void cybt_bt_task(cy_thread_arg_t arg)
{
    BT_MSG_HDR *p_bt_msg;
    cy_rslt_t  result;
    uint8_t    *p;

    BTTASK_TRACE_DEBUG("bt_task(): start");

    cybt_core_stack_init();

    while(1)
    {
        p_bt_msg = NULL;

        result = cy_rtos_get_queue(&BTU_TASK_QUEUE,
                                   (void *)&p_bt_msg,
                                   CY_RTOS_NEVER_TIMEOUT,
                                   false
                                  );

        if(CY_RSLT_SUCCESS != result || NULL == p_bt_msg)
        {
            BTTASK_TRACE_WARNING("bt_task(): queue error (0x%x), msg = 0x%p",
                                 result,
                                 p_bt_msg
                                );
            continue;
        }

        if(BT_EVT_TASK_SHUTDOWN == (uint32_t)p_bt_msg)
        {
            while(CY_RSLT_SUCCESS == cy_rtos_get_queue(&BTU_TASK_QUEUE,
                                                       (void *)&p_bt_msg,
                                                       0,
                                                       false
                                                      )
                 )
            {
                cybt_platform_task_mempool_free((void *) p_bt_msg);
            }
            cy_rtos_deinit_queue(&BTU_TASK_QUEUE);
            wiced_bt_stack_shutdown();
            break;
        }

        p = (uint8_t *)(p_bt_msg + 1);

        switch(p_bt_msg->event)
        {
            case BT_EVT_TO_BTU_EVENT:
                wiced_bt_process_hci_events(p, p_bt_msg->length);
                break;
            case BT_EVT_TO_BTU_ACL:
                wiced_bt_process_acl_data(p, p_bt_msg->length);
                break;
            case BT_EVT_TO_BTU_SCO:
                break;
            case BT_EVT_TO_BTU_TIMER:
                wiced_bt_process_timer();
                break;
            default:
                BTTASK_TRACE_ERROR("bt_task(): Unknown event (0x%x)", p_bt_msg->event);
                break;
        }

        cybt_platform_task_mempool_free(p_bt_msg);
    }

    host_stack_platform_interface_deinit();
    cy_rtos_exit_thread();
}


