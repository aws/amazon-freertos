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

#ifdef MTK_NVDM_ENABLE

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include "nvdm.h"

#ifndef NULL
#define NULL    (void *)(0)
#endif

#if  defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
#endif /* __GNUC__ */

#define CONTEXT_INFO_GROUP_NAME "context_info"
#define CONTEXT_INFO_INDEX_NAME "index"
#define CONTEXT_INFO_DATA_ITEM_NAME "info"
#define CONTEXT_INFO_INDEX 0xff
#define CONTEXT_INFO_INDEX_MAX 3

static uint8_t g_context_info_index = CONTEXT_INFO_INDEX;

__weak nvdm_status_t nvdm_read_data_item(const char *group_name,
        const char *data_item_name,
        uint8_t *buffer,
        uint32_t *size)
{
    return NVDM_STATUS_ERROR;
}

__weak nvdm_status_t nvdm_write_data_item(const char *group_name,
        const char *data_item_name,
        nvdm_data_item_type_t type,
        const uint8_t *buffer,
        uint32_t size)
{
    return NVDM_STATUS_ERROR;
}

bool context_info_save(uint8_t *context_info, uint32_t length)
{
    uint8_t temp_index;
    char data_name[16];
    uint32_t size;
    nvdm_status_t status;

    if ((context_info == NULL) || (length > 2048)) {
        return false;
    }

    if (g_context_info_index == CONTEXT_INFO_INDEX) {
        size = 1;
        status = nvdm_read_data_item(CONTEXT_INFO_GROUP_NAME, CONTEXT_INFO_INDEX_NAME, &temp_index, &size);
        if (status == NVDM_STATUS_OK) {
            g_context_info_index = temp_index;
        } else if (status == NVDM_STATUS_ITEM_NOT_FOUND) {
            g_context_info_index = 0;
        }
    }

    sprintf(data_name, "info_%d", g_context_info_index);
    status = nvdm_write_data_item(CONTEXT_INFO_GROUP_NAME, data_name, NVDM_DATA_ITEM_TYPE_STRING, context_info, length);
    if (status != NVDM_STATUS_OK) {
        assert(0);
        return false;
    }

    if (g_context_info_index < CONTEXT_INFO_INDEX_MAX - 1) {
        g_context_info_index++;
    } else {
        g_context_info_index = 0;
    }
    temp_index = g_context_info_index + '0';
    status = nvdm_write_data_item(CONTEXT_INFO_GROUP_NAME, CONTEXT_INFO_INDEX_NAME, NVDM_DATA_ITEM_TYPE_RAW_DATA, &temp_index, 1);
    if (status != NVDM_STATUS_OK) {
        assert(0);
        return false;
    }

    return true;
}

#endif

