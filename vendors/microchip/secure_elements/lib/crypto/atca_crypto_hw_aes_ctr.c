/**
 * \file
 * \brief CryptoAuthLib Basic API methods for AES CTR mode.
 *
 * The AES command supports 128-bit AES encryption or decryption of small
 * messages or data packets in ECB mode.
 *
 * \note List of devices that support this command - ATECC608A, ATECC608B,
 *       & TA100. Refer to device datasheet for full details.
 *
 *
 * \copyright (c) 2015-2020 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#include "cryptoauthlib.h"
#include "atca_crypto_hw_aes.h"

/** \brief Initialize context for AES CTR operation with an existing IV, which
 *         is common when start a decrypt operation.
 *
 * The IV is a combination of nonce (left-field) and big-endian counter
 * (right-field). The counter_size field sets the size of the counter and the
 * remaining bytes are assumed to be the nonce.
 *
 * \param[in] device        Device context pointer
 * \param[in] ctx           AES CTR context to be initialized.
 * \param[in] key_id        Key location. Can either be a slot/handles or
 *                          in TempKey.
 * \param[in] key_block     Index of the 16-byte block to use within the key
 *                          location for the actual key.
 * \param[in] counter_size  Size of counter in IV in bytes. 4 bytes is a
 *                          common size.
 * \param[in] iv            Initialization vector (concatenation of nonce and
 *                          counter) 16 bytes.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_ctr_init_ext(ATCADevice device, atca_aes_ctr_ctx_t* ctx, uint16_t key_id, uint8_t key_block, uint8_t counter_size, const uint8_t* iv)
{
    if (ctx == NULL || iv == NULL || counter_size > ATCA_AES128_BLOCK_SIZE)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "Either NULL pointer or invalid counter size received");
    }
    memset(ctx, 0, sizeof(*ctx));
    ctx->device = device;
    ctx->key_id = key_id;
    ctx->key_block = key_block;
    ctx->counter_size = counter_size;
    memcpy(ctx->cb, iv, ATCA_AES128_BLOCK_SIZE);

    return ATCA_SUCCESS;
}

/** \brief Initialize context for AES CTR operation with an existing IV, which
 *         is common when start a decrypt operation.
 *
 * The IV is a combination of nonce (left-field) and big-endian counter
 * (right-field). The counter_size field sets the size of the counter and the
 * remaining bytes are assumed to be the nonce.
 *
 * \param[in] ctx           AES CTR context to be initialized.
 * \param[in] key_id        Key location. Can either be a slot/handles or
 *                          in TempKey.
 * \param[in] key_block     Index of the 16-byte block to use within the key
 *                          location for the actual key.
 * \param[in] counter_size  Size of counter in IV in bytes. 4 bytes is a
 *                          common size.
 * \param[in] iv            Initialization vector (concatenation of nonce and
 *                          counter) 16 bytes.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_ctr_init(atca_aes_ctr_ctx_t* ctx, uint16_t key_id, uint8_t key_block, uint8_t counter_size, const uint8_t* iv)
{
    return atcab_aes_ctr_init_ext(atcab_get_device(), ctx, key_id, key_block, counter_size, iv);
}

/** \brief Initialize context for AES CTR operation with a random nonce and
 *         counter set to 0 as the IV, which is common when starting an
 *         encrypt operation.
 *
 * The IV is a combination of nonce (left-field) and big-endian counter
 * (right-field). The counter_size field sets the size of the counter and the
 * remaining bytes are assumed to be the nonce.
 *
 * \param[in]  device        Device context pointer
 * \param[in]  ctx           AES CTR context to be initialized.
 * \param[in]  key_id        Key location. Can either be a slot number or
 *                           ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in]  key_block     Index of the 16-byte block to use within the key
 *                           location for the actual key.
 * \param[in]  counter_size  Size of counter in IV in bytes. 4 bytes is a
 *                           common size.
 * \param[out] iv            Initialization vector (concatenation of nonce and
 *                           counter) is returned here (16 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_ctr_init_rand_ext(ATCADevice device, atca_aes_ctr_ctx_t* ctx, uint16_t key_id, uint8_t key_block, uint8_t counter_size, uint8_t* iv)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t nonce_size;

    if (ctx == NULL || iv == NULL || counter_size > ATCA_AES128_BLOCK_SIZE)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "Either NULL pointer or invalid counter size received");
    }
    memset(ctx, 0, sizeof(*ctx));
    ctx->device = device;
    ctx->key_id = key_id;
    ctx->key_block = key_block;
    ctx->counter_size = counter_size;

    // Generate random nonce
    nonce_size = ATCA_AES128_BLOCK_SIZE - ctx->counter_size;
    if (nonce_size != 0)
    {
        uint8_t random_nonce[32];
        if (ATCA_SUCCESS != (status = atcab_random_ext(device, random_nonce)))
        {
            return status;
        }
        memcpy(iv, random_nonce, nonce_size);
    }
    memcpy(ctx->cb, iv, ATCA_AES128_BLOCK_SIZE);

    return ATCA_SUCCESS;
}

/** \brief Initialize context for AES CTR operation with a random nonce and
 *         counter set to 0 as the IV, which is common when starting an
 *         encrypt operation.
 *
 * The IV is a combination of nonce (left-field) and big-endian counter
 * (right-field). The counter_size field sets the size of the counter and the
 * remaining bytes are assumed to be the nonce.
 *
 * \param[in]  ctx           AES CTR context to be initialized.
 * \param[in]  key_id        Key location. Can either be a slot number or
 *                           ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in]  key_block     Index of the 16-byte block to use within the key
 *                           location for the actual key.
 * \param[in]  counter_size  Size of counter in IV in bytes. 4 bytes is a
 *                           common size.
 * \param[out] iv            Initialization vector (concatenation of nonce and
 *                           counter) is returned here (16 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_ctr_init_rand(atca_aes_ctr_ctx_t* ctx, uint16_t key_id, uint8_t key_block, uint8_t counter_size, uint8_t* iv)
{
    return atcab_aes_ctr_init_rand_ext(atcab_get_device(), ctx, key_id, key_block, counter_size, iv);
}

/** \brief Increments AES CTR counter value.
 *
 * \param[in,out] ctx  AES CTR context
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_ctr_increment(atca_aes_ctr_ctx_t* ctx)
{
    size_t i;

    if (ctx == NULL || ctx->counter_size > ATCA_AES128_BLOCK_SIZE)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "Either NULL pointer or invalid counter size received");
    }

    // Increment the big-endian counter value
    for (i = 0; i < ctx->counter_size; i++)
    {
        // Counter is right-aligned in buffer
        if (++(ctx->cb[ATCA_AES128_BLOCK_SIZE - i - 1]) != 0)
        {
            break;
        }
    }
    if (i >= ctx->counter_size)
    {
        // Counter overflowed
        memset(&ctx->cb[ATCA_AES128_BLOCK_SIZE - ctx->counter_size], 0, ctx->counter_size);
    }

    return ATCA_SUCCESS;
}

/** \brief Process a block of data using CTR mode and a key within the
 *         device. atcab_aes_ctr_init() or atcab_aes_ctr_init_rand()
 *         should be called before the first use of this function.
 *
 * \param[in]  ctx     AES CTR context structure.
 * \param[in]  input   Input data to be processed (16 bytes).
 * \param[out] output  Output data is returned here (16 bytes).
 *
 * \return ATCA_SUCCESS on success, ATCA_INVALID_SIZE on counter overflow,
 *         otherwise an error code.
 */
ATCA_STATUS atcab_aes_ctr_block(atca_aes_ctr_ctx_t* ctx, const uint8_t* input, uint8_t* output)
{
    uint8_t i;
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t encrypted_counter[ATCA_AES128_BLOCK_SIZE];

    if (ctx == NULL || input == NULL || output == NULL)
    {
        return ATCA_TRACE(ATCA_BAD_PARAM, "NULL pointer received");
    }

    // Block encrypt of counter block (128 bits)
    if (ATCA_SUCCESS != (status = atcab_aes_encrypt_ext(ctx->device, ctx->key_id, ctx->key_block, ctx->cb, encrypted_counter)))
    {
        return status;
    }

    // XOR output of AES encrypt with input to get output
    for (i = 0; i < ATCA_AES128_BLOCK_SIZE; i++)
    {
        output[i] = encrypted_counter[i] ^ input[i];
    }

    if (ATCA_SUCCESS != (status = atcab_aes_ctr_increment(ctx)))
    {
        return status;
    }

    return status;
}

/** \brief Encrypt a block of data using CTR mode and a key within the
 *         device. atcab_aes_ctr_init() or atcab_aes_ctr_init_rand()
 *         should be called before the first use of this function.
 *
 * \param[in]  ctx         AES CTR context structure.
 * \param[in]  plaintext   Plaintext to be encrypted (16 bytes).
 * \param[out] ciphertext  Encrypted data is returned here (16 bytes).
 *
 * \return ATCA_SUCCESS on success, ATCA_INVALID_SIZE on counter overflow,
 *         otherwise an error code.
 */
ATCA_STATUS atcab_aes_ctr_encrypt_block(atca_aes_ctr_ctx_t* ctx, const uint8_t* plaintext, uint8_t* ciphertext)
{
    return atcab_aes_ctr_block(ctx, plaintext, ciphertext);
}

/** \brief Decrypt a block of data using CTR mode and a key within the
 *         device. atcab_aes_ctr_init() or atcab_aes_ctr_init_rand()
 *         should be called before the first use of this function.
 *
 * \param[in]  ctx         AES CTR context structure.
 * \param[in]  ciphertext  Ciphertext to be decrypted (16 bytes).
 * \param[out] plaintext   Decrypted data is returned here (16 bytes).
 *
 * \return ATCA_SUCCESS on success, ATCA_INVALID_SIZE on counter overflow,
 *         otherwise an error code.
 */
ATCA_STATUS atcab_aes_ctr_decrypt_block(atca_aes_ctr_ctx_t* ctx, const uint8_t* ciphertext, uint8_t* plaintext)
{
    return atcab_aes_ctr_block(ctx, ciphertext, plaintext);
}

