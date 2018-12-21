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

#ifndef _CONNSYS_ADAPTER_H__
#define _CONNSYS_ADAPTER_H__

#include <stdint.h>
#include "net_task.h"
#include "wifi_api.h"

/***************************  RX  adapter *****************************/

typedef enum _PKT_HANDLE_RESULT_E {
    PKT_HANDLE_NON_COMPLETE = 0,
    PKT_HANDLE_COMPLETE,
} PKT_HANDLE_RESULT_T;


typedef void (*connsys_alloc_pkt_t)(uint32_t allocate_len,
                                    void **pkt_ptr,
                                    uint8_t **payload_ptr);

typedef void (*connsys_free_pkt_t)(void *);
typedef uint8_t (*connsys_enqueue_t)(int32_t port, PNETFUNC bottom_half);

typedef PKT_HANDLE_RESULT_T(*connsys_deliver_inband_t)(void *pkt, uint8_t *payload, uint32_t len);
typedef PKT_HANDLE_RESULT_T(*connsys_deliver_data_t)(void *pkt, uint8_t *payload, uint32_t len, int inf);
typedef uint8_t (*connsys_advance_pkt_t)(void *pkt, int16_t len);

typedef struct mt76x7_connsys_ops {
    connsys_alloc_pkt_t         alloc;
    connsys_free_pkt_t          free;
    connsys_advance_pkt_t       advance_pkt_hdr;
    connsys_enqueue_t           enqueue_bottom_half;
    connsys_deliver_data_t      deliver_tcpip;
    connsys_deliver_inband_t    deliver_inband;
} *connsys_ops_t;

extern connsys_ops_t connsys_ops;

extern void connsys_set_rxraw_handler(wifi_rx_handler_t handler);
#endif

