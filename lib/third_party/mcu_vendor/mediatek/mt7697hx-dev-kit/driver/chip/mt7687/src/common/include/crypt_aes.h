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
 * @file crypt_aes.h
 *
 * AES ECB, CBC modes and key length 128/192/256 are supported.
 * Interrupt is raised to indicate encrypt/decrypt operation done.
 *
 */
#ifndef __CRYPT_AES_H__
#define __CRYPT_AES_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AES_KEY_SOURCE_EFUSE    (0)
#define AES_KEY_SOURCE_SW       (1)
#define AES_KEY_SOURCE_NUM      (2)

#define AES_KEY_SOURCE_EFUSE_1        (0)
#define AES_KEY_SOURCE_EFUSE_2        (1)
#define AES_KEY_SOURCE_EFUSE_NUM      (2)

#define AES_TYPE_ECB        (0)
#define AES_TYPE_CBC        (1)
#define AES_TYPE_NUM        (2)

#define AES_MODE_DECRYPT    (0)
#define AES_MODE_ENCRYPT    (1)
#define AES_MODE_NUM        (2)

/**
 *
 * @brief  Configure key source and mode of AES module.
 *
 * @param  key_source [IN] Key source from 0:efuse and 1:software are supported.
 * @param  type [IN] AES 0:ECB and 1:CBC are supported.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note If key source set as from efuse, corresponding efuse value has to be configured by efuse setting function.
 */
int32_t aes_configure(uint8_t key_source, uint8_t type);


/**
 * @brief  If key source set as from software, function aes_set_key is used to configure key value.
 *
 * @param  key_byte_length [IN] Key length in unit of byte, ex. key_byte_length is 16 for AES 128.
 * @param  key [IN] Key value.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note If key source set as from efuse, corresponding efuse value has to be configured by efuse setting function.
 */
int32_t aes_set_key(uint8_t key_byte_length, uint8_t *key);

/**
 * @brief  If key source set as from efuse, function aes_set_key_slot is used to select key slot.
 *
 * @param  slot [IN] Key slot, ex. slot is 1 for customer.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note If key source set as from efuse, corresponding efuse value has to be configured by efuse setting function.
 */
int32_t aes_set_key_slot(uint8_t slot);

/**
 * @brief  AES encrypt and decrypt operation.
 *
 * @param  cipher_text [IN/OUT] Pointer indicates cipher text buffer.
 * @param  cipher_text_length [IN] Length of cipher_text must be multiple of 16.
 * @param  plain_text [IN/OUT] Pointer indicates plain text buffer.
 * @param  plain_text_length [IN] Length of plain_text must be multiple of 16.
 * @param  initial_vector [IN] Pointer indicates initial vector buffer.
 * @param  mode [IN] 0: decrypt, 1: encrypt.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note Key value has to be configured before function aes_operate.
 */
int32_t aes_operate(uint8_t *cipher_text, uint16_t cipher_text_length, uint8_t *plain_text, uint16_t plain_text_length, uint8_t *initial_vector, uint8_t mode);

int32_t cmac_operate(uint8_t *plain_text, uint16_t plain_text_length, uint8_t *iv);

#ifdef __cplusplus
}
#endif

#endif /* __CRYPT_AES_H__ */
