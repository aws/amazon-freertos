/*******************************************************************************
* \file cybt_hci_task.c
*
* \brief
* Implement HCI task which handles HCI packet transmission and reception.
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
#include <stdbool.h>

#include "cyabs_rtos.h"

#include "hcidefs.h"

#include "cybt_platform_task.h"
#include "cybt_platform_hci.h"
#include "cybt_platform_trace.h"
#include "cybt_platform_config.h"
#include "cybt_platform_util.h"


/******************************************************************************
 *                                Constants
 ******************************************************************************/
#define HCI_EVT_MSG_HDR_SIZE  (BT_MSG_HDR_SIZE + HCIE_PREAMBLE_SIZE)
#define HCI_ACL_MSG_HDR_SIZE  (BT_MSG_HDR_SIZE + HCI_DATA_PREAMBLE_SIZE)
#define HCI_SCO_MSG_HDR_SIZE  (BT_MSG_HDR_SIZE + HCI_SCO_PREAMBLE_SIZE)

#define HCI_RX_DISCARD_BUF_SIZE    (32)

#define IS_HCI_RX_READY_EVT(evt)   (((evt & 0xFFFFFFFC) ^ BT_EVT_HCI_DATA_READY_BASE)? false: true)

#define HCI_MAX_READ_PACKET_NUM_PER_ROUND   (10)

/******************************************************************************
 *                           Variables Definitions
 ******************************************************************************/
uint32_t hci_task_dropped_packet_cnt = 0;


/******************************************************************************
 *                           Function Definitions
 ******************************************************************************/
cybt_result_t cybt_send_msg_to_hci_task(BT_MSG_HDR *p_bt_msg,
                                                     bool is_from_isr
                                                    )
{
    cy_rslt_t result;

    if(NULL == p_bt_msg)
    {
        return CYBT_ERR_BADARG;
    }

    result = cy_rtos_put_queue(&HCI_TASK_QUEUE, (void *) &p_bt_msg, 0, is_from_isr);

    if(CY_RSLT_SUCCESS != result)
    {
        hci_task_dropped_packet_cnt++;

        if(false == is_from_isr)
        {
            HCITASK_TRACE_ERROR("send_msg_to_hci_task(): send failure (0x%x), drop cnt = %d",
                                result,
                                hci_task_dropped_packet_cnt
                               );
        }

        if(false == IS_HCI_RX_READY_EVT((uint32)p_bt_msg))
        {
            cybt_platform_task_mempool_free((void *)p_bt_msg);
        }

        return CYBT_ERR_SEND_QUEUE_FAILED;
    }

    return CYBT_SUCCESS;
}

void discard_hci_rx_data(hci_packet_type_t packet_type, uint32_t size)
{
    uint8_t   data_buf[HCI_RX_DISCARD_BUF_SIZE];
    uint32_t  read_size = size;
    uint32_t  left_size = size;

    HCITASK_TRACE_ERROR("discard_hci_rx_data(): type = %d, size = %d",
                        packet_type,
                        size
                       );
    while (left_size)
    {
        if (read_size > HCI_RX_DISCARD_BUF_SIZE)
        {
            read_size = HCI_RX_DISCARD_BUF_SIZE;
        }

        cybt_platform_hci_read(HCI_PACKET_TYPE_EVENT,
                               data_buf,
                               &read_size,
                               CY_RTOS_NEVER_TIMEOUT
                              );
        if (left_size >= read_size)
        {
            left_size -= read_size;
            read_size  = left_size;
        }
    }
}

void handle_hci_tx_command(BT_MSG_HDR *p_bt_msg)
{
    uint8_t *p_hci_payload;
    cybt_result_t result;

    if(NULL == p_bt_msg)
    {
        HCITASK_TRACE_ERROR("handle_hci_tx_command(): Invalid task msg");
        return;
    }

    p_hci_payload = (uint8_t *)(p_bt_msg + 1);
    result = cybt_platform_hci_write(HCI_PACKET_TYPE_COMMAND,
                                     p_hci_payload,
                                     p_bt_msg->length
                                    );
    if(CYBT_SUCCESS != result)
    {
        HCITASK_TRACE_ERROR("handle_hci_tx_command(): hci write failed (0x%x)",
                            result
                           );
    }

    cybt_platform_task_mempool_free(p_bt_msg);
}

void handle_hci_tx_acl(BT_MSG_HDR *p_bt_msg)
{
    uint8_t *p_hci_payload;
    cybt_result_t result;

    if(NULL == p_bt_msg)
    {
        HCITASK_TRACE_ERROR("handle_hci_tx_acl(): Invalid task msg");
        return;
    }

    p_hci_payload = (uint8_t *)(p_bt_msg + 1);
    result = cybt_platform_hci_write(HCI_PACKET_TYPE_ACL,
                                     p_hci_payload,
                                     p_bt_msg->length
                                    );
    if(CYBT_SUCCESS != result)
    {
        HCITASK_TRACE_ERROR("handle_hci_tx_acl(): hci write failed (0x%x)",
                            result
                           );
    }

    cybt_platform_task_mempool_free(p_bt_msg);
}

void handle_hci_tx_sco(BT_MSG_HDR *p_bt_msg)
{
    uint8_t *p_hci_payload;
    cybt_result_t result;

    if(NULL == p_bt_msg)
    {
        HCITASK_TRACE_ERROR("handle_hci_tx_sco(): Invalid task msg");
        return;
    }

    p_hci_payload = (uint8_t *)(p_bt_msg + 1);
    result = cybt_platform_hci_write(HCI_PACKET_TYPE_SCO,
                                     p_hci_payload,
                                     p_bt_msg->length
                                    );
    if(CYBT_SUCCESS != result)
    {
        HCITASK_TRACE_ERROR("handle_hci_tx_sco(): hci write failed (0x%x)",
                            result
                           );
    }

    cybt_platform_task_mempool_free(p_bt_msg);
}

void handle_hci_rx_event(void)
{
    hci_event_packet_header_t  hci_evt_header = {0};
    uint32_t                   read_len = 0;
    BT_MSG_HDR                 *p_bt_msg = NULL;
    cybt_result_t              result;
    uint8_t                    *p;

    read_len = sizeof(hci_event_packet_header_t);
    result = cybt_platform_hci_read(HCI_PACKET_TYPE_EVENT,
                                    (uint8_t *) &hci_evt_header,
                                    &read_len,
                                    CY_RTOS_NEVER_TIMEOUT
                                   );
    
    p_bt_msg = (BT_MSG_HDR *) cybt_platform_task_mempool_alloc(HCI_EVT_MSG_HDR_SIZE
                                                               + hci_evt_header.content_length
                                                              );
    if(NULL == p_bt_msg)
    {
        HCITASK_TRACE_ERROR("handle_hci_rx_event(): Fail to get memory");
        discard_hci_rx_data(HCI_PACKET_TYPE_EVENT, hci_evt_header.content_length);
        return;
    }

    p_bt_msg->event = BT_EVT_TO_BTU_EVENT;
    p_bt_msg->length = HCIE_PREAMBLE_SIZE + hci_evt_header.content_length;

    p = (uint8_t *)(p_bt_msg + 1);
    memcpy(p, &hci_evt_header, sizeof(hci_event_packet_header_t));
    p += sizeof(hci_event_packet_header_t);

    read_len = hci_evt_header.content_length;
    result = cybt_platform_hci_read(HCI_PACKET_TYPE_EVENT,
                                    p,
                                    &read_len,
                                    CY_RTOS_NEVER_TIMEOUT
                                   );
    if(CYBT_SUCCESS != result)
    {
        HCITASK_TRACE_ERROR("handle_hci_rx_event(): read failed (0x%x)", result);
        cybt_platform_task_mempool_free((void *) p_bt_msg);

        return;
    }

    result = cybt_send_msg_to_bt_task(p_bt_msg, false);
    if(CYBT_SUCCESS != result)
    {
        HCITASK_TRACE_ERROR("handle_hci_rx_event(): Fail to send to BT task (ret = 0x%x)",
                            result
                           );
    }
}

void handle_hci_rx_acl(void)
{
    hci_acl_packet_header_t  hci_acl_header = {0};
    uint32_t                 read_len = 0;
    BT_MSG_HDR               *p_bt_msg = NULL;
    cybt_result_t            result;
    uint8_t                  *p;

    read_len = sizeof(hci_acl_packet_header_t);
    result = cybt_platform_hci_read(HCI_PACKET_TYPE_ACL,
                                    (uint8_t *) &hci_acl_header,
                                    &read_len,
                                    CY_RTOS_NEVER_TIMEOUT
                                   );

    p_bt_msg = (BT_MSG_HDR *) cybt_platform_task_mempool_alloc(HCI_ACL_MSG_HDR_SIZE
                                                               + hci_acl_header.content_length
                                                              );
    if(NULL == p_bt_msg)
    {
        HCITASK_TRACE_ERROR("handle_hci_rx_acl(): Fail to get memory");
        discard_hci_rx_data(HCI_PACKET_TYPE_ACL, hci_acl_header.content_length);
        return;
    }

    p_bt_msg->event = BT_EVT_TO_BTU_ACL;
    p_bt_msg->length = HCI_DATA_PREAMBLE_SIZE + hci_acl_header.content_length;

    p = (uint8_t *)(p_bt_msg + 1);
    memcpy(p, &hci_acl_header, sizeof(hci_acl_packet_header_t));
    p += sizeof(hci_acl_packet_header_t);

    read_len = hci_acl_header.content_length;
    result = cybt_platform_hci_read(HCI_PACKET_TYPE_ACL,
                                    p,
                                    &read_len,
                                    CY_RTOS_NEVER_TIMEOUT
                                   );

    if(CYBT_SUCCESS != result)
    {
        HCITASK_TRACE_ERROR("handle_hci_rx_acl(): read failed (0x%x)", result);
        cybt_platform_task_mempool_free((void *) p_bt_msg);

        return;
    }

    result = cybt_send_msg_to_bt_task(p_bt_msg, false);
    if(CYBT_SUCCESS != result)
    {
        HCITASK_TRACE_ERROR("handle_hci_rx_acl(): Fail to send BT task (ret = 0x%x)",
                            result
                           );
    }
}

void handle_hci_rx_sco(void)
{
    hci_sco_packet_header_t  hci_sco_header = {0};
    uint32_t                 read_len = 0;
    BT_MSG_HDR               *p_bt_msg = NULL;
    cybt_result_t            result;
    uint8_t                  *p;
    
    read_len = sizeof(hci_sco_packet_header_t);
    result = cybt_platform_hci_read(HCI_PACKET_TYPE_EVENT,
                                    (uint8_t *) &hci_sco_header,
                                    &read_len,
                                    CY_RTOS_NEVER_TIMEOUT
                                   );
     
    p_bt_msg = (BT_MSG_HDR *) cybt_platform_task_mempool_alloc(HCI_SCO_MSG_HDR_SIZE
                                                               + hci_sco_header.content_length
                                                              );
    if(NULL == p_bt_msg)
    {
        HCITASK_TRACE_ERROR("handle_hci_rx_sco(): Fail to get memory");
        discard_hci_rx_data(HCI_PACKET_TYPE_SCO, hci_sco_header.content_length);
        return;
    }

    p_bt_msg->event = BT_EVT_TO_BTU_EVENT;
    p_bt_msg->length = HCI_SCO_PREAMBLE_SIZE + hci_sco_header.content_length;

    p = (uint8_t *)(p_bt_msg + 1);
    memcpy(p, &hci_sco_header, sizeof(hci_sco_packet_header_t));
    p += sizeof(hci_sco_packet_header_t);

    read_len = hci_sco_header.content_length;
    result = cybt_platform_hci_read(HCI_PACKET_TYPE_SCO,
                                    p,
                                    &read_len,
                                    CY_RTOS_NEVER_TIMEOUT
                                   );

    if(CYBT_SUCCESS != result)
    {
        HCITASK_TRACE_ERROR("handle_hci_rx_sco(): read failed (0x%x)", result);
        cybt_platform_task_mempool_free((void *) p_bt_msg);

        return;
    }

    result = cybt_send_msg_to_bt_task(p_bt_msg, false);
    if(CYBT_SUCCESS != result)
    {
        HCITASK_TRACE_ERROR("handle_hci_rx_sco(): Fail to send BT task (ret = 0x%x)",
                            result
                           );
    }
}

void handle_hci_rx_data_ready(hci_packet_type_t hci_packet_type)
{
    uint32_t read_len = 0;
    cybt_result_t result;
    const cybt_platform_config_t *p_bt_platform_cfg = cybt_platform_get_config();

    if(CYBT_HCI_UART == p_bt_platform_cfg->hci_config.hci_transport)
    {
        uint8_t count = 0;

        for(; count < HCI_MAX_READ_PACKET_NUM_PER_ROUND; count++)
        {
            read_len = 1;
            result = cybt_platform_hci_read(HCI_PACKET_TYPE_IGNORE,
                                            (uint8_t *) &hci_packet_type,
                                            &read_len,
                                            0
                                           );
            if(CYBT_SUCCESS != result || 0 == read_len)
            {
                // No data is read from UART FIFO
                return;
            }
    
            switch(hci_packet_type)
            {
                case HCI_PACKET_TYPE_ACL:
                    handle_hci_rx_acl();
                    break;
                case HCI_PACKET_TYPE_EVENT:
                    handle_hci_rx_event();
                    break;
                case HCI_PACKET_TYPE_SCO:
                    handle_hci_rx_sco();
                    break;
                default:
                    HCITASK_TRACE_ERROR("handle_hci_rx_data_ready(): unkown type (0x%02x)",
                                        hci_packet_type
                                       );
                break;
            }
        }
    }
}

void handle_hci_timer(BT_MSG_HDR *p_bt_msg)
{
    cybt_platform_task_mempool_free(p_bt_msg);
}

void cybt_hci_task(cy_thread_arg_t arg)
{
    HCITASK_TRACE_DEBUG("hci_task(): start");

    cybt_platform_hci_open();

    while(1)
    {
        BT_MSG_HDR *p_bt_msg = NULL;
        cy_rslt_t  result;

        result = cy_rtos_get_queue(&HCI_TASK_QUEUE,
                                   (void *)&p_bt_msg,
                                   CY_RTOS_NEVER_TIMEOUT,
                                   false
                                  );

        if(CY_RSLT_SUCCESS != result || NULL == p_bt_msg)
        {
            HCITASK_TRACE_WARNING("hci_task(): queue error (0x%x)", result);
            continue;
        }

        if(IS_HCI_RX_READY_EVT((uint32_t)p_bt_msg))
        {
            switch((uint32_t)p_bt_msg)
            {
                case BT_EVT_TO_HCI_DATA_READY_UNKNOWN:
                    handle_hci_rx_data_ready(HCI_PACKET_TYPE_IGNORE);
                    break;
                case BT_EVT_TO_HCI_DATA_READY_ACL:
                    handle_hci_rx_acl();
                    break;
                case BT_EVT_TO_HCI_DATA_READY_SCO:
                    handle_hci_rx_sco();
                    break;
                case BT_EVT_TO_HCI_DATA_READY_EVT:
                    handle_hci_rx_event();
                    break;
                default:
                    break;
            }

            cybt_platform_hci_irq_rx_data_ind(true);
            continue;
        }

        if(BT_EVT_TASK_SHUTDOWN == (uint32_t)p_bt_msg)
        {
            while(CY_RSLT_SUCCESS == cy_rtos_get_queue(&HCI_TASK_QUEUE,
                                                       (void *)&p_bt_msg,
                                                       0,
                                                       false
                                                      )
                 )
            {
                cybt_platform_task_mempool_free((void *) p_bt_msg);
            }
            cy_rtos_deinit_queue(&HCI_TASK_QUEUE);

            cybt_platform_hci_close();
            break;
        }

        switch(p_bt_msg->event)
        {
            case BT_EVT_TO_HCI_COMMAND:
                handle_hci_tx_command(p_bt_msg);
                break;
            case BT_EVT_TO_HCI_ACL:
                handle_hci_tx_acl(p_bt_msg);
                break;
            case BT_EVT_TO_HCI_SCO:
                handle_hci_tx_sco(p_bt_msg);
                break;
            case BT_EVT_TO_HCI_TIMER:
                handle_hci_timer(p_bt_msg);
                break;
            default:
                HCITASK_TRACE_ERROR("hci_task(): Unknown event (0x%x)", p_bt_msg->event);
                cybt_platform_task_mempool_free((void *) p_bt_msg);
                break;
        }
    }
    cy_rtos_exit_thread();
}


