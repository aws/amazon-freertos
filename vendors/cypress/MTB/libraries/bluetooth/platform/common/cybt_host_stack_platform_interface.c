/*******************************************************************************
* \file cybt_host_stack_platform_interface.c
*
* \brief
* This file implements the WICED BT stack porting interface.
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

#include <string.h>

#include "cyabs_rtos.h"

#include "wiced_bt_stack_platform.h"

#include "cybt_platform_hci.h"
#include "cybt_platform_task.h"
#include "cybt_platform_interface.h"
#include "cybt_platform_config.h"
#include "cybt_platform_trace.h"
#include "cybt_platform_util.h"

#include "cyhal_lptimer.h"
#include "cycfg_system.h"


/******************************************************************************
 *                           Variables Definitions
 ******************************************************************************/
cy_mutex_t   bt_stack_mutex;

char         bt_trace_buf[CYBT_TRACE_BUFFER_SIZE];

/******************************************************************************
 *                           Function Definitions
 ******************************************************************************/
void host_stack_exception_handler(uint16_t code, char* msg, void* ptr)
{
    SPIF_TRACE_ERROR("[Exception] code = 0x%x, msg = %s", code, msg);
}

void host_stack_mutex_lock(void * p_lock_context)
{
    cy_rtos_get_mutex(&bt_stack_mutex, CY_RTOS_NEVER_TIMEOUT);
}

void host_stack_mutex_unlock(void * p_lock_context)
{
    cy_rtos_set_mutex(&bt_stack_mutex);
}

uint8_t *host_stack_get_acl_to_lower_buffer(wiced_bt_transport_t transport, uint32_t size)
{
    uint16_t    msg_packet_len;
    uint16_t    payload_len;
    uint8_t     *p;
    const cybt_platform_config_t *p_bt_platform_cfg = cybt_platform_get_config();

    if(CYBT_HCI_UART == p_bt_platform_cfg->hci_config.hci_transport)
    {
        // One extra byte is added for HCI UART packet type
        msg_packet_len = BT_MSG_HDR_SIZE + HCI_UART_TYPE_HEADER_SIZE + size;
        payload_len = size + HCI_UART_TYPE_HEADER_SIZE;
    }
    else
    {
        SPIF_TRACE_ERROR("get_acl_to_lower_buffer(): Unknown transport (%d)",
                         p_bt_platform_cfg->hci_config.hci_transport
                        );
        return NULL;
    }

    BT_MSG_HDR *p_bt_msg = (BT_MSG_HDR *) cybt_platform_task_mempool_alloc(msg_packet_len);

    if(NULL == p_bt_msg)
    {
        SPIF_TRACE_ERROR("get_acl_to_lower_buffer(): Unable to alloc memory");
        return NULL;
    }

    p = (uint8_t *)(p_bt_msg + 1);

    p_bt_msg->event = BT_EVT_TO_HCI_ACL;
    p_bt_msg->length = payload_len;

    if(CYBT_HCI_UART == p_bt_platform_cfg->hci_config.hci_transport)
    {
        *p++ = HCI_PACKET_TYPE_ACL;
    }

    return p;
}

wiced_result_t host_stack_send_acl_to_lower(wiced_bt_transport_t transport,
                                               uint8_t *p_data,
                                               uint16_t len
                                              )
{
    cybt_result_t result;
    BT_MSG_HDR  *p_msg_hdr;
    const cybt_platform_config_t *p_bt_platform_cfg = cybt_platform_get_config();

    if(CYBT_HCI_UART == p_bt_platform_cfg->hci_config.hci_transport)
    {
        // One extra byte is for HCI UART packet type
        p_msg_hdr = (BT_MSG_HDR *)(p_data - BT_MSG_HDR_SIZE - HCI_UART_TYPE_HEADER_SIZE);
    }
    else
    {
        SPIF_TRACE_ERROR("send_acl_to_lower(): Unknown transport (%d)",
                         p_bt_platform_cfg->hci_config.hci_transport
                        );
        return WICED_ERROR;
    }

    SPIF_TRACE_DEBUG("send_acl_to_lower(): trans = %d, p_data = 0x%p, len = %d",
                     transport,
                     p_data,
                     len
                    );

    if(NULL == p_data || 0 == len)
    {
        SPIF_TRACE_ERROR("send_acl_to_lower(): Invalid data(0x%p) or length(%d)",
                         p_data,
                         len
                        );
        return WICED_ERROR;
    }

    result = cybt_send_msg_to_hci_task(p_msg_hdr, false);
    if(CYBT_SUCCESS == result)
    {
        return WICED_SUCCESS;
    }
    else
    {
        SPIF_TRACE_ERROR("send_acl_to_lower(): Send hci queue failed (ret = 0x%x)",
                         result
                        );
        return WICED_ERROR;
    }
}

wiced_result_t host_stack_send_cmd_to_lower(uint8_t *p_cmd, uint16_t cmd_len)
{
    cybt_result_t result;
    BT_MSG_HDR  *p_msg_hdr;
    uint8_t     *p;
    uint16_t    msg_packet_len;
    uint16_t    payload_len;
    const cybt_platform_config_t *p_bt_platform_cfg = cybt_platform_get_config();

    if(CYBT_HCI_UART == p_bt_platform_cfg->hci_config.hci_transport)
    {
        // One extra byte is added for HCI UART packet type
        msg_packet_len = BT_MSG_HDR_SIZE + HCI_UART_TYPE_HEADER_SIZE + cmd_len;
        payload_len = cmd_len + HCI_UART_TYPE_HEADER_SIZE;
    }
    else
    {
        SPIF_TRACE_ERROR("send_cmd_to_lower(): Unknown transport (%d)",
                         p_bt_platform_cfg->hci_config.hci_transport
                        );
        return WICED_ERROR;
    }

    p_msg_hdr = (BT_MSG_HDR  *) cybt_platform_task_mempool_alloc(msg_packet_len);
    if(NULL == p_msg_hdr)
    {
        SPIF_TRACE_ERROR("send_cmd_to_lower(): Unable to alloc memory");
        return WICED_ERROR;
    }

    SPIF_TRACE_DEBUG("send_cmd_to_lower(): p_cmd = 0x%x, len = %d",
                     p_cmd,
                     cmd_len
                    );

    p_msg_hdr->event = BT_EVT_TO_HCI_COMMAND;
    p_msg_hdr->length = payload_len;

    p = (uint8_t *)(p_msg_hdr + 1);

    if(CYBT_HCI_UART == p_bt_platform_cfg->hci_config.hci_transport)
    {
        *p++ = HCI_PACKET_TYPE_COMMAND;
    }

    memcpy(p, p_cmd, cmd_len);

    result = cybt_send_msg_to_hci_task(p_msg_hdr, false);
    if(CYBT_SUCCESS == result)
    {
        return WICED_SUCCESS;
    }
    else
    {
        SPIF_TRACE_ERROR("send_cmd_to_lower(): Send hci queue failed (ret = 0x%x)",
                         result
                        );
        return WICED_ERROR;
    }
}

wiced_result_t host_stack_send_sco_to_lower(uint16_t handle, uint8_t* p_data, uint8_t len)
{
    cybt_result_t result;
    BT_MSG_HDR  *p_msg_hdr;
    uint8_t     *p;
    uint16_t    msg_packet_len;
    uint16_t    payload_len;
    hci_sco_packet_header_t *p_sco_hdr;
    const cybt_platform_config_t *p_bt_platform_cfg = cybt_platform_get_config();

    if(CYBT_HCI_UART == p_bt_platform_cfg->hci_config.hci_transport)
    {
        // One extra byte is added for HCI UART packet type
        msg_packet_len = BT_MSG_HDR_SIZE + HCI_UART_TYPE_HEADER_SIZE + sizeof(hci_sco_packet_header_t) + len;;
        payload_len = HCI_UART_TYPE_HEADER_SIZE + sizeof(hci_sco_packet_header_t) + len;
    }
    else
    {
        SPIF_TRACE_ERROR("send_sco_to_lower(): Unknown transport (%d)",
                         p_bt_platform_cfg->hci_config.hci_transport
                        );
        return WICED_ERROR;
    }

    p_msg_hdr = (BT_MSG_HDR  *) cybt_platform_task_mempool_alloc(msg_packet_len);
    if(NULL == p_msg_hdr)
    {
        SPIF_TRACE_ERROR("send_sco_to_lower(): Unable to alloc memory");
        return WICED_ERROR;
    }

    SPIF_TRACE_DEBUG("send_sco_to_lower(): handle = 0x%x, p_data = 0x%p, len = %d",
                     handle,
                     p_data,
                     len
                    );

    p_msg_hdr->event = BT_EVT_TO_HCI_SCO;
    p_msg_hdr->length = payload_len;

    p = (uint8_t *)(p_msg_hdr + 1);
    *p++ = HCI_PACKET_TYPE_SCO;

    p_sco_hdr = (hci_sco_packet_header_t *) p;
    p_sco_hdr->hci_handle = handle;
    p_sco_hdr->content_length = len;

    p += sizeof(hci_sco_packet_header_t);
    memcpy(p, p_data, len);

    result = cybt_send_msg_to_hci_task(p_msg_hdr, false);
    if(CYBT_SUCCESS == result)
    {
        return WICED_SUCCESS;
    }
    else
    {
        SPIF_TRACE_ERROR("send_sco_to_lower(): Send hci queue failed (ret = 0x%x)",
                         result
                        );
        return WICED_ERROR;
    }
}

void host_stack_print_trace_log(char *p_trace_buf,
                                int trace_buf_len,
                                wiced_bt_trace_type_t trace_type
                               )
{
    switch(trace_type)
    {
        case WICED_BT_TRACE_ERROR:
            STACK_TRACE_ERROR("%s", p_trace_buf);
            break;
        case WICED_BT_TRACE_WARN:
            STACK_TRACE_WARNING("%s", p_trace_buf);
            break;
        case WICED_BT_TRACE_API:
            STACK_TRACE_API("%s", p_trace_buf);
            break;
        case WICED_BT_TRACE_EVENT:
            STACK_TRACE_EVENT("%s", p_trace_buf);
            break;
        case WICED_BT_TRACE_DEBUG:
            STACK_TRACE_DEBUG("%s", p_trace_buf);
            break;
        default:
            break;
    }
}

void host_stack_platform_interface_init(void)
{
    wiced_bt_stack_platform_t host_stack_platform_if = {0};
    wiced_result_t result;

    extern void bt_post_reset_cback(void);

    host_stack_platform_if.pf_exception               = host_stack_exception_handler;
    host_stack_platform_if.pf_os_malloc               = cybt_platform_malloc;
    host_stack_platform_if.pf_os_free                 = cybt_platform_free;
    host_stack_platform_if.pf_get_tick_count_64       = cybt_platform_get_tick_count_us;
    host_stack_platform_if.pf_set_next_timeout        = cybt_platform_set_next_timeout;
    host_stack_platform_if.stack_lock.p_lock_context  = NULL;
    host_stack_platform_if.stack_lock.pf_lock_func    = host_stack_mutex_lock;
    host_stack_platform_if.stack_lock.pf_unlock_func  = host_stack_mutex_unlock;
    host_stack_platform_if.pf_get_acl_to_lower_buffer = host_stack_get_acl_to_lower_buffer;
    host_stack_platform_if.pf_write_acl_to_lower      = host_stack_send_acl_to_lower;
    host_stack_platform_if.pf_write_cmd_to_lower      = host_stack_send_cmd_to_lower;
    host_stack_platform_if.pf_write_sco_to_lower      = host_stack_send_sco_to_lower;
    host_stack_platform_if.pf_hci_trace_cback_t       = NULL;
    host_stack_platform_if.pf_debug_trace             = host_stack_print_trace_log;
    host_stack_platform_if.trace_buffer               = bt_trace_buf;
    host_stack_platform_if.trace_buffer_len           = CYBT_TRACE_BUFFER_SIZE;
    host_stack_platform_if.pf_patch_download          = bt_post_reset_cback;

    memset(bt_trace_buf, 0, CYBT_TRACE_BUFFER_SIZE);

    cy_rtos_init_mutex(&bt_stack_mutex);

    result = wiced_bt_stack_platform_initialize(&host_stack_platform_if);

    if(WICED_SUCCESS != result)
    {
        SPIF_TRACE_ERROR("platform_interface_init(): failed, result = 0x%x", result);
    }
}

void host_stack_platform_interface_deinit(void)
{
    wiced_bt_stack_platform_t host_stack_platform_if = {0};
    wiced_result_t result;

    result = wiced_bt_stack_platform_initialize(&host_stack_platform_if);
    
    if(WICED_SUCCESS != result)
    {
        SPIF_TRACE_ERROR("platform_interface_deinit(): failed, result = 0x%x", result);
    }

    cy_rtos_deinit_mutex(&bt_stack_mutex);
}

