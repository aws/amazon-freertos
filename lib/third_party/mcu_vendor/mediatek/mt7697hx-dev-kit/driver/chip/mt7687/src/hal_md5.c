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

#include "hal_md5.h"

#ifdef HAL_MD5_MODULE_ENABLED

#include "stddef.h"
#include "crypt_md5.h"
#include "hal_log.h"
#include "hal_crypt_internal.h"
#ifdef HAL_DFS_MODULE_ENABLED
#include "hal_dfs_internal.h"
#endif

hal_md5_status_t hal_md5_init(hal_md5_context_t *context)
{
    if (NULL == context) {
        log_hal_error("context is NULL.");
        return HAL_MD5_STATUS_ERROR;
    }

    hal_crypt_lock_take();

    crypt_md5_init((MD5_CTX_STRUC *)context);

    hal_crypt_lock_give();
    return HAL_MD5_STATUS_OK;
}

hal_md5_status_t hal_md5_append(hal_md5_context_t *context, uint8_t *message, uint32_t length)
{
    if (NULL == context) {
        log_hal_error("context is NULL.");
        return HAL_MD5_STATUS_ERROR;
    }

    hal_crypt_lock_take();

#ifdef HAL_DFS_MODULE_ENABLED
    if(hal_dfs_get_cpu_frequency() == 192) {
        /* dfs doesn't return error.*/
        hal_dfs_target_cpu_frequency(96);
    }
#endif

    while (1) {
        if (length >= MAX_APPEND_LENGTH) {
            crypt_md5_append((MD5_CTX_STRUC *)context, message, MAX_APPEND_LENGTH);
            message += MAX_APPEND_LENGTH;
            length -= MAX_APPEND_LENGTH;
        } else {
            crypt_md5_append((MD5_CTX_STRUC *)context, message, length);
            break;
        }
    }

#ifdef HAL_DFS_MODULE_ENABLED
    hal_dfs_target_cpu_frequency(192);
#endif

    hal_crypt_lock_give();
    return HAL_MD5_STATUS_OK;
}

hal_md5_status_t hal_md5_end(hal_md5_context_t *context, uint8_t digest_message[HAL_MD5_DIGEST_SIZE])
{
    if (NULL == context) {
        log_hal_error("context is NULL.");
        return HAL_MD5_STATUS_ERROR;
    }
    if (NULL == digest_message) {
        log_hal_error("digest_message is NULL.");
        return HAL_MD5_STATUS_ERROR;
    }

    hal_crypt_lock_take();

#ifdef HAL_DFS_MODULE_ENABLED
    if(hal_dfs_get_cpu_frequency() == 192) {
        /* dfs doesn't return error.*/
        hal_dfs_target_cpu_frequency(96);
    }
#endif

    crypt_md5_end((MD5_CTX_STRUC *)context, digest_message);

#ifdef HAL_DFS_MODULE_ENABLED
    hal_dfs_target_cpu_frequency(192);
#endif

    hal_crypt_lock_give();
    return HAL_MD5_STATUS_OK;
}

#endif /* HAL_MD5_MODULE_ENABLED */

