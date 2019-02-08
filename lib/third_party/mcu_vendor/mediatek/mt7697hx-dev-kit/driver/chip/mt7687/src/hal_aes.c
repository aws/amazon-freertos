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

#include "hal_aes.h"

#ifdef HAL_AES_MODULE_ENABLED

#include <string.h>
#include "hal_define.h"
#include "mt7687.h"
#include "crypt_aes.h"
#include "hal_log.h"
#include "hal_crypt_internal.h"
#include "hal_nvic.h"
#include "mt7687_cm4_hw_memmap.h"

#ifdef HAL_DFS_MODULE_ENABLED
#include "hal_dfs_internal.h"
#endif

#define AES_POLLING         1
#define AES_INTERRUPT       0



/* global lock used to protect the crypto engine */
bool g_crypt_lock = CRYPT_UNLOCK;

/* internal function for getting lock, -100 means the crypto engine is not available */
int32_t hal_crypt_lock_take(void)
{
    uint32_t irq_status;
    irq_status = save_and_set_interrupt_mask();
    if (g_crypt_lock == CRYPT_LOCK) {
        restore_interrupt_mask(irq_status);
        return -100;
    }
    g_crypt_lock = CRYPT_LOCK;
    restore_interrupt_mask(irq_status);
    return 0;
}


/* internal function for releasing lock */
void hal_crypt_lock_give(void)
{
    g_crypt_lock = CRYPT_UNLOCK;
}

volatile static bool g_aes_op_done = false;

/* It's about 10s at 192MHz CPU clock */
#define HAL_AES_MAX_WAIT_COUNT 0x10000000

static void aes_operation_done(hal_nvic_irq_t irq_number)
{
    NVIC_DisableIRQ((IRQn_Type)CM4_MTK_CRYPTO_IRQ);
    NVIC_ClearPendingIRQ((IRQn_Type)CM4_MTK_CRYPTO_IRQ);

    g_aes_op_done = true;

    NVIC_EnableIRQ((IRQn_Type)CM4_MTK_CRYPTO_IRQ);
}

static hal_aes_status_t do_aes_encrypt(uint8_t *encrypt_buffer,
                                       uint32_t encrypt_buffer_length,
                                       uint8_t *plain_buffer,
                                       uint32_t plain_buffer_length,
                                       uint8_t init_vector[HAL_AES_CBC_IV_LENGTH], 
                                       uint8_t notify)
{
    P_IOT_CRYPTO_CTRL_TypeDef pCryptoCtrlTypeDef = (P_IOT_CRYPTO_CTRL_TypeDef)(CM4_MTK_CRYPTO_BASE + 0x0004);
    uint32_t wait_count = 0;
    g_aes_op_done = false;
    int32_t ret_val = aes_operate(encrypt_buffer,
                                  encrypt_buffer_length,
                                  plain_buffer,
                                  plain_buffer_length,
                                  init_vector,
                                  AES_MODE_ENCRYPT);
    if (ret_val < 0) {
        log_hal_error("aes_operate fail.");
        return HAL_AES_STATUS_ERROR;
    }

    if(notify == AES_INTERRUPT)
    {
        while (!g_aes_op_done) {
            //simple wait
            wait_count++;
            if (wait_count > HAL_AES_MAX_WAIT_COUNT) {
                log_hal_error("wait for encrypt timeout.");
                return HAL_AES_STATUS_ERROR;
            }
        }
    }
    else if(notify == AES_POLLING)
    {
        while((pCryptoCtrlTypeDef->ENGINE_CTRL) & (BIT(ENGINE_CTRL_START_OFFSET)))
        {
            wait_count++;
            if (wait_count > HAL_AES_MAX_WAIT_COUNT) {
                log_hal_error("wait for encrypt timeout.");
                return HAL_AES_STATUS_ERROR;
            }
        }
    }
    else
    {
        return HAL_AES_STATUS_ERROR;
    }
    return HAL_AES_STATUS_OK;
}

static hal_aes_status_t do_aes_decrypt(uint8_t *encrypt_buffer,
                                       uint32_t encrypt_buffer_length,
                                       uint8_t *plain_buffer,
                                       uint32_t plain_buffer_length,
                                       uint8_t init_vector[HAL_AES_CBC_IV_LENGTH], 
                                       uint8_t notify)
{
    P_IOT_CRYPTO_CTRL_TypeDef pCryptoCtrlTypeDef = (P_IOT_CRYPTO_CTRL_TypeDef)(CM4_MTK_CRYPTO_BASE + 0x0004);
    uint32_t wait_count = 0;
    g_aes_op_done = false;

    int32_t ret_val = aes_operate(encrypt_buffer,
                                  encrypt_buffer_length,
                                  plain_buffer,
                                  plain_buffer_length,
                                  init_vector,
                                  AES_MODE_DECRYPT);
    if (ret_val < 0) {
        log_hal_error("aes_operate fail.");
        return HAL_AES_STATUS_ERROR;
    }
    if(notify == AES_INTERRUPT)
    {
        while (!g_aes_op_done) {
            //simple wait
            wait_count++;
            if (wait_count > HAL_AES_MAX_WAIT_COUNT) {
                log_hal_error("wait for decrypt timeout.");
                return HAL_AES_STATUS_ERROR;
            }
        }
    }
    else if(notify == AES_POLLING)
    {
        while((pCryptoCtrlTypeDef->ENGINE_CTRL) & (BIT(ENGINE_CTRL_START_OFFSET)))
        {
            wait_count++;
            if (wait_count > HAL_AES_MAX_WAIT_COUNT) {
                log_hal_error("wait for encrypt timeout.");
                return HAL_AES_STATUS_ERROR;
            }
        }
    }
    else
    {
        return HAL_AES_STATUS_ERROR;
    }
    return HAL_AES_STATUS_OK;
}
static void xor_128(uint8_t *a, uint8_t *b, uint8_t *out)
{
    int i = 0;
    for (i = 0; i < 16; i++)
    {
        out[i] = a[i] ^ b[i];
    }
}
static void leftshift_onebit(uint8_t *input, uint8_t *output)
{
    int i = 0;  
    unsigned char overflow = 0;
    for (i = 15; i >= 0; i-- ) {
        output[i] = input[i] << 1;
        output[i] |= overflow;
        overflow = (input[i] & 0x80) ? 1 : 0;
    }
}
hal_aes_status_t aes_cmac_subkey(uint8_t* K1, uint8_t* K2)
{
    uint8_t Z[HAL_AES_BLOCK_SIZES] = {0};
    uint8_t const_Rb[HAL_AES_BLOCK_SIZES] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87};
    uint8_t tmp[HAL_AES_BLOCK_SIZES] = {0};
    uint8_t L[HAL_AES_BLOCK_SIZES] = {0};
    hal_aes_status_t ret = HAL_AES_STATUS_ERROR;


    ret = do_aes_encrypt(L, HAL_AES_BLOCK_SIZES, Z, HAL_AES_BLOCK_SIZES, NULL, AES_POLLING);
    if(ret != HAL_AES_STATUS_OK)
        return HAL_AES_STATUS_ERROR;

    //K1
    if((L[0] & 0x80) == 0 )
    {
        leftshift_onebit(L, K1);
    }
    else
    {
        leftshift_onebit(L, tmp);
        xor_128(tmp, const_Rb, K1);
    }
    //K2
    if((K1[0] & 0x80) == 0 )
    {
        leftshift_onebit(K1, K2);
    }
    else
    {
        leftshift_onebit(K1, tmp);
        xor_128(tmp, const_Rb, K2);
    }
    return HAL_AES_STATUS_OK;
}
/* internal common function */
hal_aes_status_t hal_aes_encrypt_with_padding(hal_aes_buffer_t *encrypted_text,
        hal_aes_buffer_t *plain_text,
        hal_aes_buffer_t *key,
        uint8_t init_vector[HAL_AES_CBC_IV_LENGTH],
        uint8_t mode,
        uint8_t use_hw_key,
        uint8_t key_slot)
{
    if ((NULL == encrypted_text)
            || (NULL == plain_text)
            || (NULL == init_vector)) {
        log_hal_error("NULL input.");
        return HAL_AES_STATUS_ERROR;
    }
    if (AES_KEY_SOURCE_SW == use_hw_key && NULL == key) {
        log_hal_error("NULL input.");
        return HAL_AES_STATUS_ERROR;
    }

    uint32_t last_block_size = plain_text->length % HAL_AES_BUFFER_SIZE;
    uint32_t block_num = plain_text->length / HAL_AES_BUFFER_SIZE;
    uint8_t padding_size = HAL_AES_BLOCK_SIZES - (plain_text->length % HAL_AES_BLOCK_SIZES);
    uint8_t *iv;
#ifdef HAL_DFS_MODULE_ENABLED
    bool freq_change = false;
#endif

    if (encrypted_text->length < (plain_text->length  + padding_size)) {
        log_hal_error("Inadequate encrypted buffer.");
        return HAL_AES_STATUS_ERROR;
    }

    hal_crypt_lock_take();

#ifdef HAL_DFS_MODULE_ENABLED
    if(hal_dfs_get_cpu_frequency() == 192) {
        /* dfs doesn't return error.*/
        hal_dfs_target_cpu_frequency(96);
        freq_change = true;
    }
#endif
    hal_nvic_register_isr_handler((IRQn_Type)CM4_MTK_CRYPTO_IRQ, aes_operation_done);
    NVIC_SetPriority((IRQn_Type)CM4_MTK_CRYPTO_IRQ, CM4_MTK_CRYPTO_PRI);
    NVIC_EnableIRQ((IRQn_Type)CM4_MTK_CRYPTO_IRQ);

    if (AES_TYPE_CBC == mode) { /* AES_TYPE_CBC */
        if (AES_KEY_SOURCE_EFUSE == use_hw_key) {
            aes_set_key((uint8_t)key->length, key->buffer); /* need length */
            aes_configure(0, 1); /* 1:software 0:ECB and 1:CBC */
            aes_set_key_slot(key_slot);
        } else {
            aes_set_key((uint8_t)key->length, key->buffer);
            aes_configure(1, 1); /* 1:software 0:ECB and 1:CBC */
        }
    } else { /* AES_TYPE_CBC */
        if (AES_KEY_SOURCE_EFUSE == use_hw_key) {
            aes_set_key((uint8_t)key->length, key->buffer); /* need length */
            aes_configure(0, 0); /* 1:software 0:ECB and 1:CBC */
            aes_set_key_slot(key_slot);
        } else {
            aes_set_key((uint8_t)key->length, key->buffer);
            aes_configure(1, 0); /* 1:software 0:ECB and 1:CBC */
        }
    }


    int i;
    iv = init_vector;
    for (i = 0; i < block_num; i++ ) {
        if (HAL_AES_STATUS_OK != do_aes_encrypt(encrypted_text->buffer + i * HAL_AES_BUFFER_SIZE,
                                                HAL_AES_BUFFER_SIZE,
                                                plain_text->buffer + i * HAL_AES_BUFFER_SIZE,
                                                HAL_AES_BUFFER_SIZE,
                                                iv,
                                                AES_INTERRUPT)) {
            log_hal_error("do_aes_encrypt fail.");
            hal_crypt_lock_give();
#ifdef HAL_DFS_MODULE_ENABLED
            if(freq_change == true) {
                hal_dfs_target_cpu_frequency(192);
            }
#endif
            return HAL_AES_STATUS_ERROR;
        }
        if (AES_TYPE_CBC == mode) { /* do 2nd aes cbc operation need to input newer iv */
            iv = encrypted_text->buffer + i * HAL_AES_BUFFER_SIZE + HAL_AES_BUFFER_SIZE - HAL_AES_BLOCK_SIZES;
        } else {
            iv = init_vector;
        }
    }

    uint32_t first_encypt_size = block_num * HAL_AES_BUFFER_SIZE;
    uint8_t data_block[HAL_AES_BUFFER_SIZE] = {0};
    memcpy(data_block, plain_text->buffer + first_encypt_size, last_block_size);
    memset(data_block + last_block_size, padding_size, padding_size);

    if (HAL_AES_STATUS_OK != do_aes_encrypt(encrypted_text->buffer + first_encypt_size,
                                            last_block_size + padding_size,
                                            data_block,
                                            last_block_size + padding_size,
                                            iv,
                                            AES_INTERRUPT)) {
        log_hal_error("do_aes_encrypt fail.");
        hal_crypt_lock_give();
#ifdef HAL_DFS_MODULE_ENABLED
        if(freq_change == true) {
            hal_dfs_target_cpu_frequency(192);
        }
#endif
        return HAL_AES_STATUS_ERROR;
    }

    encrypted_text->length = plain_text->length + padding_size;
//    log_hal_dump("Encrypted raw data(AES %s): ", encrypted_text->buffer, encrypted_text->length, (mode == AES_TYPE_CBC)?"CBC":"ECB");
    hal_crypt_lock_give();
#ifdef HAL_DFS_MODULE_ENABLED
    if(freq_change == true) {
        hal_dfs_target_cpu_frequency(192);
    }
#endif
    return HAL_AES_STATUS_OK;
}

hal_aes_status_t hal_aes_decrypt(hal_aes_buffer_t *plain_text,
                                 hal_aes_buffer_t *encrypted_text,
                                 hal_aes_buffer_t *key,
                                 uint8_t init_vector[HAL_AES_CBC_IV_LENGTH],
                                 uint8_t mode,
                                 uint8_t use_hw_key,
                                 uint8_t key_slot)
{
    if ((NULL == plain_text)
            || (NULL == encrypted_text)
            || (NULL == init_vector)) {
        log_hal_error("NULL input.");
        return HAL_AES_STATUS_ERROR;
    }
    if (AES_KEY_SOURCE_SW == use_hw_key && NULL == key) {
        log_hal_error("NULL input.");
        return HAL_AES_STATUS_ERROR;
    }

    if ((encrypted_text->length % HAL_AES_BLOCK_SIZES) != 0) {
        log_hal_error("Invalid encrypted text length: %lu.", encrypted_text->length);
        return HAL_AES_STATUS_ERROR;
    }
    if (plain_text->length < (encrypted_text->length - HAL_AES_BLOCK_SIZES)) {
        log_hal_error("Plain text buffer lengthL %lu is too small, encrypted length is: %lu",
                      encrypted_text->length, encrypted_text->length);
        return HAL_AES_STATUS_ERROR;
    }
    if (AES_KEY_SOURCE_SW == use_hw_key
            && (key->length != HAL_AES_KEY_LENGTH_128)
            && (key->length != HAL_AES_KEY_LENGTH_192)
            && (key->length != HAL_AES_KEY_LENGTH_256)) {
        log_hal_error("key length is %lu, invalid. It has to be 16, 24 or 32.", key->length);
        return HAL_AES_STATUS_ERROR;
    }
#ifdef HAL_DFS_MODULE_ENABLED
    bool freq_change = false;
#endif

    hal_crypt_lock_take();
#ifdef HAL_DFS_MODULE_ENABLED
    if(hal_dfs_get_cpu_frequency() == 192) {
        /* dfs doesn't return error.*/
        hal_dfs_target_cpu_frequency(96);
        freq_change = true;
    }
#endif

    hal_nvic_register_isr_handler((IRQn_Type)CM4_MTK_CRYPTO_IRQ, aes_operation_done);
    NVIC_SetPriority((IRQn_Type)CM4_MTK_CRYPTO_IRQ, CM4_MTK_CRYPTO_PRI);
    NVIC_EnableIRQ((IRQn_Type)CM4_MTK_CRYPTO_IRQ);

    if (AES_TYPE_CBC == mode) { /* AES_TYPE_CBC */
        if (AES_KEY_SOURCE_EFUSE == use_hw_key) {
            aes_set_key(key->length, key->buffer); /* need length */
            aes_configure(0, 1); /* 1:software 0:ECB and 1:CBC */
            aes_set_key_slot(key_slot);
        } else {
            aes_set_key(key->length, key->buffer);
            aes_configure(1, 1); /* 1:software 0:ECB and 1:CBC */
        }
    } else { /* AES_TYPE_CBC */
        if (AES_KEY_SOURCE_EFUSE == use_hw_key) {
            aes_set_key(key->length, key->buffer); /* need length */
            aes_configure(0, 0); /* 1:software 0:ECB and 1:CBC */
            aes_set_key_slot(key_slot);
        } else {
            aes_set_key(key->length, key->buffer);
            aes_configure(1, 0); /* 1:software 0:ECB and 1:CBC */
        }
    }

    uint32_t last_block_size = encrypted_text->length % HAL_AES_BUFFER_SIZE;
    uint32_t block_num = encrypted_text->length / HAL_AES_BUFFER_SIZE;
    uint8_t *iv;
    int i;

    iv = init_vector;
    for (i = 0; i < block_num; i++ ) {
        if (HAL_AES_STATUS_OK != do_aes_decrypt(encrypted_text->buffer + i * HAL_AES_BUFFER_SIZE,
                                                HAL_AES_BUFFER_SIZE,
                                                plain_text->buffer + i * HAL_AES_BUFFER_SIZE,
                                                HAL_AES_BUFFER_SIZE,
                                                iv,
                                                AES_INTERRUPT)) {
            log_hal_error("do_aes_decrypt fail");
            hal_crypt_lock_give();
#ifdef HAL_DFS_MODULE_ENABLiED
            if(freq_change == true) {
                hal_dfs_target_cpu_frequency(192);
            }
#endif
            return HAL_AES_STATUS_ERROR;
        }
        if (AES_TYPE_CBC == mode) { /* do 2nd aes cbc operation need to input newer iv */
            iv = encrypted_text->buffer + i * HAL_AES_BUFFER_SIZE + HAL_AES_BUFFER_SIZE - HAL_AES_BLOCK_SIZES;
        } else {
            iv = init_vector;
        }
    }

    if (last_block_size > 0) {
        if (HAL_AES_STATUS_OK != do_aes_decrypt(encrypted_text->buffer + block_num * HAL_AES_BUFFER_SIZE,
                                                last_block_size,
                                                plain_text->buffer + block_num * HAL_AES_BUFFER_SIZE,
                                                last_block_size,
                                                iv,
                                                AES_INTERRUPT)) {
            log_hal_error("do_aes_decrypt fail");
            hal_crypt_lock_give();
#ifdef HAL_DFS_MODULE_ENABLED
            if(freq_change == true) {
                hal_dfs_target_cpu_frequency(192);
            }
#endif
            return HAL_AES_STATUS_ERROR;
        }
    }

    uint8_t padding_size = plain_text->buffer[encrypted_text->length - 1];
    //log_hal_dump("Decrypted raw data(AES %s): ", plain_text->buffer, encrypted_text->length, (mode == AES_TYPE_CBC) ? "CBC" : "ECB");
    plain_text->length = encrypted_text->length - padding_size;

    hal_crypt_lock_give();
#ifdef HAL_DFS_MODULE_ENABLED
    if(freq_change == true) {
        hal_dfs_target_cpu_frequency(192);
    }
#endif
    return HAL_AES_STATUS_OK;
}

hal_aes_status_t hal_aes_cbc_encrypt(hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *key,
                                     uint8_t init_vector[HAL_AES_CBC_IV_LENGTH])
{
    return hal_aes_encrypt_with_padding(encrypted_text, plain_text, key, init_vector, AES_TYPE_CBC, AES_KEY_SOURCE_SW, 0);
}

hal_aes_status_t hal_aes_cbc_decrypt(hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *key,
                                     uint8_t init_vector[HAL_AES_CBC_IV_LENGTH])
{
    return hal_aes_decrypt(plain_text, encrypted_text, key, init_vector, AES_TYPE_CBC, AES_KEY_SOURCE_SW, 0);
}

hal_aes_status_t hal_aes_ecb_encrypt(hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *key)
{
    uint8_t init_vector[16] = {0};

    return hal_aes_encrypt_with_padding(encrypted_text, plain_text, key, init_vector, AES_TYPE_ECB, AES_KEY_SOURCE_SW, 0);
}

hal_aes_status_t hal_aes_ecb_decrypt(hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *key)
{
    uint8_t init_vector[HAL_AES_CBC_IV_LENGTH] = {0};

    return hal_aes_decrypt(plain_text, encrypted_text, key, init_vector, AES_TYPE_ECB, AES_KEY_SOURCE_SW, 0);
}

hal_aes_status_t hal_aes_cbc_encrypt_ex(hal_aes_buffer_t *encrypted_text,
                                        hal_aes_buffer_t *plain_text,
                                        hal_aes_buffer_t *key,
                                        uint8_t init_vector[HAL_AES_CBC_IV_LENGTH],
                                        hal_aes_efuse_key_t key_index)
{
    hal_aes_efuse_key_t efuse_key_index = (hal_aes_efuse_key_t)0;

    if (HAL_AES_EFUSE_KEY1 == key_index) { // key1
        efuse_key_index = (hal_aes_efuse_key_t)(AES_KEY_SOURCE_EFUSE_1 + 1); //IOT_CRYPTO_KEY_BANK_KEK;
    } else {  // key2
        efuse_key_index = (hal_aes_efuse_key_t)(AES_KEY_SOURCE_EFUSE_2 + 1); //IOT_CRYPTO_KEY_BANK_USECRET;
    }

    return hal_aes_encrypt_with_padding(encrypted_text, plain_text, key, init_vector, AES_TYPE_CBC, AES_KEY_SOURCE_EFUSE, efuse_key_index);
}

hal_aes_status_t hal_aes_cbc_decrypt_ex(hal_aes_buffer_t *plain_text,
                                        hal_aes_buffer_t *encrypted_text,
                                        hal_aes_buffer_t *key,
                                        uint8_t init_vector[HAL_AES_CBC_IV_LENGTH],
                                        hal_aes_efuse_key_t key_index)
{
    hal_aes_efuse_key_t efuse_key_index = (hal_aes_efuse_key_t)0;

    if (HAL_AES_EFUSE_KEY1 == key_index) { // key1
        efuse_key_index = (hal_aes_efuse_key_t)(AES_KEY_SOURCE_EFUSE_1 + 1); //IOT_CRYPTO_KEY_BANK_KEK;
    } else {  // key2
        efuse_key_index = (hal_aes_efuse_key_t)(AES_KEY_SOURCE_EFUSE_2 + 1); //IOT_CRYPTO_KEY_BANK_USECRET;
    }

    return hal_aes_decrypt(plain_text, encrypted_text, key, init_vector, AES_TYPE_CBC, AES_KEY_SOURCE_EFUSE, efuse_key_index);
}

hal_aes_status_t hal_aes_ecb_encrypt_ex(hal_aes_buffer_t *encrypted_text,
                                        hal_aes_buffer_t *plain_text,
                                        hal_aes_buffer_t *key,
                                        hal_aes_efuse_key_t key_index)
{
    uint8_t init_vector[16] = {0};
    hal_aes_efuse_key_t efuse_key_index = (hal_aes_efuse_key_t)0;

    if (HAL_AES_EFUSE_KEY1 == key_index) { // key1
        efuse_key_index = (hal_aes_efuse_key_t)(AES_KEY_SOURCE_EFUSE_1 + 1); //IOT_CRYPTO_KEY_BANK_KEK;
    } else {  // key2
        efuse_key_index = (hal_aes_efuse_key_t)(AES_KEY_SOURCE_EFUSE_2 + 1); //IOT_CRYPTO_KEY_BANK_USECRET;
    }

    return hal_aes_encrypt_with_padding(encrypted_text, plain_text, key, init_vector, AES_TYPE_ECB, AES_KEY_SOURCE_EFUSE, efuse_key_index);
}

hal_aes_status_t hal_aes_ecb_decrypt_ex(hal_aes_buffer_t *plain_text,
                                        hal_aes_buffer_t *encrypted_text,
                                        hal_aes_buffer_t *key,
                                        hal_aes_efuse_key_t key_index)
{
    uint8_t init_vector[HAL_AES_CBC_IV_LENGTH] = {0};
    hal_aes_efuse_key_t efuse_key_index = (hal_aes_efuse_key_t)0;

    if (HAL_AES_EFUSE_KEY1 == key_index) { // key1
        efuse_key_index = (hal_aes_efuse_key_t)(AES_KEY_SOURCE_EFUSE_1 + 1); //IOT_CRYPTO_KEY_BANK_KEK;
    } else {  // key2
        efuse_key_index = (hal_aes_efuse_key_t)(AES_KEY_SOURCE_EFUSE_2 + 1); //IOT_CRYPTO_KEY_BANK_USECRET;
    }

    return hal_aes_decrypt(plain_text, encrypted_text, key, init_vector, AES_TYPE_ECB, AES_KEY_SOURCE_EFUSE, efuse_key_index);
}

hal_aes_status_t hal_aes_cmac_subkey(hal_aes_buffer_t *key,
                                     hal_aes_efuse_key_t key_index,
                                     uint8_t *K1, 
                                     uint8_t *K2)
{
    hal_aes_status_t ret = HAL_AES_STATUS_ERROR; 
    hal_aes_efuse_key_t efuse_key_index = (hal_aes_efuse_key_t)0;
#ifdef HAL_DFS_MODULE_ENABLED
    bool freq_change = false;
#endif

    if (NULL == key) {
        log_hal_error("NULL software key.");
        return HAL_AES_STATUS_ERROR;
    }

    if (HAL_AES_EFUSE_KEY1 == key_index) { // key1
        efuse_key_index = (hal_aes_efuse_key_t)(AES_KEY_SOURCE_EFUSE_1 + 1); //IOT_CRYPTO_KEY_BANK_KEK;
    } else {  // key2
        efuse_key_index = (hal_aes_efuse_key_t)(AES_KEY_SOURCE_EFUSE_2 + 1); //IOT_CRYPTO_KEY_BANK_USECRET;
    }

    hal_crypt_lock_take();

#ifdef HAL_DFS_MODULE_ENABLED
    if(hal_dfs_get_cpu_frequency() == 192) {
        /* dfs doesn't return error.*/
        hal_dfs_target_cpu_frequency(96);
        freq_change = true;
    }
#endif


    //ECB mode
    aes_set_key((uint8_t)key->length, key->buffer); /* need length */
    aes_configure(0, 0);
    aes_set_key_slot(efuse_key_index);

    ret = aes_cmac_subkey(K1, K2);
    if(ret != HAL_AES_STATUS_OK) {
        hal_crypt_lock_give();
#ifdef HAL_DFS_MODULE_ENABLED
        if(freq_change == true) {
            hal_dfs_target_cpu_frequency(192);
        }
#endif
        return HAL_AES_STATUS_ERROR;
    }

    hal_crypt_lock_give();
#ifdef HAL_DFS_MODULE_ENABLED
    if(freq_change == true) {
        hal_dfs_target_cpu_frequency(192);
    }
#endif
    return HAL_AES_STATUS_OK;
}
hal_aes_status_t hal_aes_cmac_message(hal_aes_buffer_t *data,
                                      hal_aes_buffer_t *key,
                                      hal_aes_efuse_key_t key_index,
                                      uint8_t* cmac)
{
    uint8_t* iv = cmac;
    hal_aes_efuse_key_t efuse_key_index = (hal_aes_efuse_key_t)0;
#ifdef HAL_DFS_MODULE_ENABLED
    bool freq_change = false;
#endif

    if (NULL == data){
        log_hal_error("NULL input.");
        return HAL_AES_STATUS_ERROR;
    }

    if (NULL == key) {
        log_hal_error("NULL software key.");
        return HAL_AES_STATUS_ERROR;
    }

    if ((data->length == 0) ||(data->length % HAL_AES_BLOCK_SIZES)) {
        log_hal_error("Inadequate message size.");
        return HAL_AES_STATUS_ERROR;
    }

    if (HAL_AES_EFUSE_KEY1 == key_index) { // key1
        efuse_key_index = (hal_aes_efuse_key_t)(AES_KEY_SOURCE_EFUSE_1 + 1); //IOT_CRYPTO_KEY_BANK_KEK;
    } else {  // key2
        efuse_key_index = (hal_aes_efuse_key_t)(AES_KEY_SOURCE_EFUSE_2 + 1); //IOT_CRYPTO_KEY_BANK_USECRET;
    }

    hal_crypt_lock_take();

#ifdef HAL_DFS_MODULE_ENABLED
    if(hal_dfs_get_cpu_frequency() == 192) {
        /* dfs doesn't return error.*/
        hal_dfs_target_cpu_frequency(96);
        freq_change = true;
    }
#endif


    //CBC mode
    aes_set_key((uint8_t)key->length, key->buffer); /* need length */
    aes_configure(0, 1); 
    aes_set_key_slot(efuse_key_index);


    if(cmac_operate(data->buffer, data->length, iv))
    {
        log_hal_error("do_aes_encrypt fail.");
        hal_crypt_lock_give();
#ifdef HAL_DFS_MODULE_ENABLED
    if(freq_change == true) {
        hal_dfs_target_cpu_frequency(192);
    }
#endif
        return HAL_AES_STATUS_ERROR;
    }


    hal_crypt_lock_give();
#ifdef HAL_DFS_MODULE_ENABLED
    if(freq_change == true) {
        hal_dfs_target_cpu_frequency(192);
    }
#endif
    return HAL_AES_STATUS_OK;
}
#endif /* HAL_AES_MODULE_ENABLED */

