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

#ifndef __INBAND_QUEUE_H__
#define __INBAND_QUEUE_H__

#include <stdint.h>
#include "type_def.h"
#include "memory_attribute.h"
#include "connsys_adapter.h"


#define CFG_INBAND_CMD_RSP_WAIT_TIME_VAL    5000 /* unit is ms */
#define CFG_INBAND_CMD_RSP_WAIT_PERIOD_VAL    10 /* unit is ms */

#define CFG_INBAND_CMD_IS_QUERY_SET            0
#define CFG_INBAND_CMD_IS_QUERY_QUERY          1
#define CFG_INBAND_CMD_IS_QUERY_NEED_STATUS    2

#define DBG_INBAND(FEATURE, _Fmt) \
        {                                       \
            if (g_inband_debug_feature & FEATURE)   \
            {               \
                printf _Fmt; \
            }   \
        }

#define INBAND_DBG_MAIN   (1<<0)


typedef enum _ENUM_INBNAD_HANDLE_T {
    INBAND_HANDLE_NON_HANDLE = 0,
    INBAND_HANDLE_RSP,
    INBAND_HANDLE_EVENT_HANDLER
} ENUM_INBNAD_HANDLE_T;

typedef enum _ENUM_INBNAD_RSP_STATUS_T {
    INBAND_NOT_RSP    = 0,
    INBAND_RSP_OK     = 1,
    INBAND_RSP_NO_BUF = 2,
    INBAND_RSP_SHORT  = 3,
    INBAND_RSP_LONG   = 4
} ENUM_INBNAD_RSP_STATUS_T;


typedef void (*inband_handle_func_t)(void *pkt_ptr, unsigned char *payload, unsigned int len);


/**
 * Initialize inband queue modules.
 *
 * @retval 0 succeeded.
 * @retval 1 already initialized.
 * @retval 2 out-of-memory.
 * @retval 3 other initilization failure.
 */
uint8_t inband_queue_init(void);

int inband_queue_query(unsigned char cmd_id, unsigned char *buf, ssize_t len, unsigned char *ret_buf, ssize_t out_len, unsigned char is_from_isr, unsigned char is_ext_cmd);

int inband_queue_exec(unsigned char cmd_id, unsigned char *buf, ssize_t len, unsigned char is_from_isr, unsigned char is_ext_cmd);

int inband_queue_exec_ex(unsigned char  cmd_id,unsigned char  *buf,ssize_t len, unsigned char *ret_buf,ssize_t out_len, unsigned char  is_from_isr,unsigned char   is_ext_cmd);

int inband_query_tx_frame(unsigned char cmd_id, unsigned char *buf, ssize_t len, unsigned char is_query, unsigned char is_ext_cmd);

int inband_queue_evt_handler(void *pkt_ptr, unsigned char *payload, unsigned int len);

void inband_queue_register_callback(inband_handle_func_t func);

void inband_queue_handler(void *pkt_ptr, unsigned char *payload, unsigned int len);

uint32_t inband_get_mq_spaces(void);

#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
void inband_evt_rescan(uint8_t retry);
#endif

PKT_HANDLE_RESULT_T connsys_deliver_inband(void *pkt, uint8_t *payload, uint32_t len);

#endif /* INBAND_QUEUE */
