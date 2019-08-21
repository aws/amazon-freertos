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

#include "hal_sha.h"

#ifdef HAL_SHA_MODULE_ENABLED

#include "stddef.h"
#include "crypt_sha.h"
#include "hal_log.h"
#include "hal_crypt_internal.h"
#ifdef HAL_DFS_MODULE_ENABLED
#include "hal_dfs_internal.h"
#endif

hal_sha_status_t hal_sha1_init(hal_sha1_context_t *context)
{
    hal_sha_status_t status;

    hal_crypt_lock_take();
    status = crypt_sha_init((SHA_CTX_STRUC *)context, HAL_SHA_INTERNAL_SHA1);
    hal_crypt_lock_give();

    return status;
}

hal_sha_status_t hal_sha1_append(hal_sha1_context_t *context, uint8_t *message, uint32_t length)
{
    if (NULL == context) {
        log_hal_error("context is NULL.");
        return HAL_SHA_STATUS_ERROR;
    }
    if (NULL == message) {
        log_hal_error("message is NULL.");
        return HAL_SHA_STATUS_ERROR;
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
            crypt_sha_append((SHA1_CTX_STRUC *)context, message, MAX_APPEND_LENGTH, HAL_SHA_INTERNAL_SHA1);
            message += MAX_APPEND_LENGTH;
            length -= MAX_APPEND_LENGTH;
        } else {
            crypt_sha_append((SHA1_CTX_STRUC *)context, message, length, HAL_SHA_INTERNAL_SHA1);
            break;
        }
    }

#ifdef HAL_DFS_MODULE_ENABLED
    hal_dfs_target_cpu_frequency(192);
#endif

    hal_crypt_lock_give();
    return HAL_SHA_STATUS_OK;
}

hal_sha_status_t hal_sha1_end(hal_sha1_context_t *context, uint8_t digest_message[HAL_SHA1_DIGEST_SIZE])
{
    hal_sha_status_t status;

    hal_crypt_lock_take();

#ifdef HAL_DFS_MODULE_ENABLED
    if(hal_dfs_get_cpu_frequency() == 192) {
        /* dfs doesn't return error.*/
        hal_dfs_target_cpu_frequency(96);
    }
#endif

    status = crypt_sha_end((SHA1_CTX_STRUC *)context, digest_message, HAL_SHA_INTERNAL_SHA1);

#ifdef HAL_DFS_MODULE_ENABLED
    hal_dfs_target_cpu_frequency(192);
#endif

    hal_crypt_lock_give();
    return status;
}

hal_sha_status_t hal_sha224_init(hal_sha224_context_t *context)
{
    hal_sha_status_t status;

    hal_crypt_lock_take();
    status = crypt_sha_init((SHA_CTX_STRUC *)context, HAL_SHA_INTERNAL_SHA224);
    hal_crypt_lock_give();

    return status;

}

hal_sha_status_t hal_sha224_append(hal_sha224_context_t *context, uint8_t *message, uint32_t length)
{
    if (NULL == context) {
        log_hal_error("context is NULL.");
        return HAL_SHA_STATUS_ERROR;
    }
    if (NULL == message) {
        log_hal_error("message is NULL.");
        return HAL_SHA_STATUS_ERROR;
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
            crypt_sha_append((SHA224_CTX_STRUC *)context, message, MAX_APPEND_LENGTH, HAL_SHA_INTERNAL_SHA224);
            message += MAX_APPEND_LENGTH;
            length -= MAX_APPEND_LENGTH;
        } else {
            crypt_sha_append((SHA224_CTX_STRUC *)context, message, length, HAL_SHA_INTERNAL_SHA224);
            break;
        }
    }

#ifdef HAL_DFS_MODULE_ENABLED
    hal_dfs_target_cpu_frequency(192);
#endif

    hal_crypt_lock_give();
    return HAL_SHA_STATUS_OK;
}

hal_sha_status_t hal_sha224_end(hal_sha224_context_t *context, uint8_t digest_message[HAL_SHA224_DIGEST_SIZE])
{
    hal_sha_status_t status;
    hal_crypt_lock_take();

#ifdef HAL_DFS_MODULE_ENABLED
    if(hal_dfs_get_cpu_frequency() == 192) {
        /* dfs doesn't return error.*/
        hal_dfs_target_cpu_frequency(96);
    }
#endif

    status = crypt_sha_end((SHA224_CTX_STRUC *)context, digest_message, HAL_SHA_INTERNAL_SHA224);

#ifdef HAL_DFS_MODULE_ENABLED
    hal_dfs_target_cpu_frequency(192);
#endif

    hal_crypt_lock_give();
    return status;
}

hal_sha_status_t hal_sha256_init(hal_sha256_context_t *context)
{
    hal_sha_status_t status;

    hal_crypt_lock_take();
    status = crypt_sha_init((SHA_CTX_STRUC *)context, HAL_SHA_INTERNAL_SHA256);
    hal_crypt_lock_give();

    return status;
}

hal_sha_status_t hal_sha256_append(hal_sha256_context_t *context, uint8_t *message, uint32_t length)
{
    if (NULL == context) {
        log_hal_error("context is NULL.");
        return HAL_SHA_STATUS_ERROR;
    }
    if (NULL == message) {
        log_hal_error("message is NULL.");
        return HAL_SHA_STATUS_ERROR;
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
            crypt_sha_append((SHA256_CTX_STRUC *)context, message, MAX_APPEND_LENGTH, HAL_SHA_INTERNAL_SHA256);
            message += MAX_APPEND_LENGTH;
            length -= MAX_APPEND_LENGTH;
        } else {
            crypt_sha_append((SHA256_CTX_STRUC *)context, message, length, HAL_SHA_INTERNAL_SHA256);
            break;
        }
    }

#ifdef HAL_DFS_MODULE_ENABLED
    hal_dfs_target_cpu_frequency(192);
#endif

    hal_crypt_lock_give();
    return HAL_SHA_STATUS_OK;
}

hal_sha_status_t hal_sha256_end(hal_sha256_context_t *context, uint8_t digest_message[HAL_SHA256_DIGEST_SIZE])
{
    hal_sha_status_t status;

    hal_crypt_lock_take();

#ifdef HAL_DFS_MODULE_ENABLED
    if(hal_dfs_get_cpu_frequency() == 192) {
        /* dfs doesn't return error.*/
        hal_dfs_target_cpu_frequency(96);
    }
#endif

    status = crypt_sha_end((SHA256_CTX_STRUC *)context, digest_message, HAL_SHA_INTERNAL_SHA256);

#ifdef HAL_DFS_MODULE_ENABLED
    hal_dfs_target_cpu_frequency(192);
#endif

    hal_crypt_lock_give();

    return status;
}

hal_sha_status_t hal_sha384_init(hal_sha384_context_t *context)
{
    hal_sha_status_t status;

    hal_crypt_lock_take();
    status = crypt_sha_init((SHA_CTX_STRUC *)context, HAL_SHA_INTERNAL_SHA384);
    hal_crypt_lock_give();

    return status;
}

hal_sha_status_t hal_sha384_append(hal_sha384_context_t *context, uint8_t *message, uint32_t length)
{
    if (NULL == context) {
        log_hal_error("context is NULL.");
        return HAL_SHA_STATUS_ERROR;
    }
    if (NULL == message) {
        log_hal_error("message is NULL.");
        return HAL_SHA_STATUS_ERROR;
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
            crypt_sha_append((SHA384_CTX_STRUC *)context, message, MAX_APPEND_LENGTH, HAL_SHA_INTERNAL_SHA384);
            message += MAX_APPEND_LENGTH;
            length -= MAX_APPEND_LENGTH;
        } else {
            crypt_sha_append((SHA384_CTX_STRUC *)context, message, length, HAL_SHA_INTERNAL_SHA384);
            break;
        }
    }

#ifdef HAL_DFS_MODULE_ENABLED
    hal_dfs_target_cpu_frequency(192);
#endif

    hal_crypt_lock_give();
    return HAL_SHA_STATUS_OK;
}

hal_sha_status_t hal_sha384_end(hal_sha384_context_t *context, uint8_t digest_message[HAL_SHA384_DIGEST_SIZE])
{
    hal_sha_status_t status;

    hal_crypt_lock_take();

#ifdef HAL_DFS_MODULE_ENABLED
    if(hal_dfs_get_cpu_frequency() == 192) {
        /* dfs doesn't return error.*/
        hal_dfs_target_cpu_frequency(96);
    }
#endif

    status = crypt_sha_end((SHA384_CTX_STRUC *)context, digest_message, HAL_SHA_INTERNAL_SHA384);

#ifdef HAL_DFS_MODULE_ENABLED
    hal_dfs_target_cpu_frequency(192);
#endif

    hal_crypt_lock_give();

    return status;
}

hal_sha_status_t hal_sha512_init(hal_sha512_context_t *context)
{
    hal_sha_status_t status;

    hal_crypt_lock_take();
    status = crypt_sha_init((SHA_CTX_STRUC *)context, HAL_SHA_INTERNAL_SHA512);
    hal_crypt_lock_give();

    return status;
}

hal_sha_status_t hal_sha512_append(hal_sha512_context_t *context, uint8_t *message, uint32_t length)
{

    if (NULL == context) {
        log_hal_error("context is NULL.");
        return HAL_SHA_STATUS_ERROR;
    }
    if (NULL == message) {
        log_hal_error("message is NULL.");
        return HAL_SHA_STATUS_ERROR;
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
            crypt_sha_append((SHA512_CTX_STRUC *)context, message, MAX_APPEND_LENGTH, HAL_SHA_INTERNAL_SHA512);
            message += MAX_APPEND_LENGTH;
            length -= MAX_APPEND_LENGTH;
        } else {
            crypt_sha_append((SHA512_CTX_STRUC *)context, message, length, HAL_SHA_INTERNAL_SHA512);
            break;
        }
    }

#ifdef HAL_DFS_MODULE_ENABLED
    hal_dfs_target_cpu_frequency(192);
#endif

    hal_crypt_lock_give();
    return HAL_SHA_STATUS_OK;
}

hal_sha_status_t hal_sha512_end(hal_sha512_context_t *context, uint8_t digest_message[HAL_SHA512_DIGEST_SIZE])
{
    hal_sha_status_t status;

    hal_crypt_lock_take();

#ifdef HAL_DFS_MODULE_ENABLED
    if(hal_dfs_get_cpu_frequency() == 192) {
        /* dfs doesn't return error.*/
        hal_dfs_target_cpu_frequency(96);
    }
#endif

    status = crypt_sha_end((SHA512_CTX_STRUC *)context, digest_message, HAL_SHA_INTERNAL_SHA512);

#ifdef HAL_DFS_MODULE_ENABLED
    hal_dfs_target_cpu_frequency(192);
#endif

    hal_crypt_lock_give();

    return status;
}

#endif /* HAL_SHA_MODULE_ENABLED */

