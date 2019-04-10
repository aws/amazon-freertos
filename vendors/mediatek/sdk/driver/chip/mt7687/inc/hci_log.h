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
 * @file hci_log.h
 *
 *  HCI log out related function.
 *
 */
#ifndef __HCI_LOG_H__
#define __HCI_LOG_H__

#include "stdint.h"

typedef enum {
    HCI_COMMAND = 1,
    HCI_EVENT = 2,
    HCI_ACL_IN = 4,
    HCI_ACL_OUT = 8,
} ENUM_HCI_DATA_TYPE;


/**
 * @brief  record HCI command.
 *
 * @param  buf [IN] HCI command data.
 * @param  length [IN] indicate length of buf.
 *
 * @return size of buf had recorded, <0 means fail.
 *
 */
int32_t hci_log_cmd(unsigned char *buf, int32_t length);

/**
 * @brief  record HCI event.
 *
 * @param  buf [IN] HCI event data.
 * @param  length [IN] indicate length of buf.
 *
 * @return size of buf had recorded, <0 means fail.
 *
 */
int32_t hci_log_event(unsigned char *buf, int32_t length);

/**
 * @brief  record HCI ACL data out.
 *
 * @param  buf [IN] HCI ACL data out.
 * @param  length [IN] indicate length of buf.
 *
 * @return size of buf had recorded, <0 means fail.
 *
 */
int32_t hci_log_acl_out(unsigned char *buf, int32_t length);

/**
 * @brief  record HCI ACL data in.
 *
 * @param  buf [IN] HCI ACL data in.
 * @param  length [IN] indicate length of buf.
 *
 * @return size of buf had recorded, <0 means fail.
 *
 */
int32_t hci_log_acl_in(unsigned char *buf, int32_t length);





#endif
