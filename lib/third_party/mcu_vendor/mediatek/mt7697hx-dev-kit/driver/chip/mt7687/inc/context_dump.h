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

/**
 * @file context_dump.h
 *
 *  connsys dump related function.
 *
 */
#ifndef __CONTEXT_DUMP_H__
#define __CONTEXT_DUMP_H__

#include "stdint.h"

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE) 

typedef struct _connsys_dump_parameters{
    uint8_t trigger_source;           /*BT/WIFI/N9 heart/N9 crash*/
    uint8_t dump_type;                /*mini dump/full dump*/
    uint8_t process_flag;             /*ready for write/finish writing*/
    uint8_t mem_reseted;              /**/
    uint32_t dump_start_addr;         /*N9 use*/
    uint32_t dump_len;                /*N9 use*/
    uint32_t reserved;
} connsys_dump_parameter_t;

/**
 * @brief  dump init.
 *
 * @return 
 *
 */
void context_dump_init(void);

/**
* @brief process dump interrupt 
*
* @return  >=0 means success, <0 means fail
*/
int32_t context_dump_intr_receive_process(void);

/**
 * @brief  trigger dumop n9 process.
 *
 * @param  *dump_param [IN] dump parameters .
 *
 * @return =0 means successful, <0 means fail.
 *
 */
int8_t context_dump_trigger(connsys_dump_parameter_t *dump_param);

/**
 * @brief  trigger dumop n9 process use semaphore.
 *
 * @param  *dump_param [IN] dump parameters .
 *
 * @return =0 means successful, <0 means fail.
 *
 */
int8_t context_dump_trigger_by_sema(connsys_dump_parameter_t *dump_param);

/**
* @brief get dump status
*
* @return  =0 means dumped, 1 means dumping, -1 means no dump cmd
*/
int8_t context_dump_status(void);

/**
 * @brief  dump debug info.
 * *
 * @return 
 *
 */
void context_dump_n9_debug(void);

/**
 * @brief  set inband cmd no rsp count.
 * *
 * @return 
 *
 */
void context_dump_set_inband_cmd_no_rsp_cnt( uint8_t rsp_cnt);

/**
 * @brief  get inband cmd no rsp count.
 * *
 * @return inband cmd no rsp count
 *
 */
uint8_t context_dump_get_inband_cmd_no_rsp_cnt(void);

#endif

#endif
