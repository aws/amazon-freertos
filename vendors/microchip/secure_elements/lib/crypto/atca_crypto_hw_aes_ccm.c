/**
 * \file
 * \brief CryptoAuthLib Basic API methods for AES CCM mode.
 *
 * The AES command supports 128-bit AES encryption or decryption of small
 * messages or data packets in ECB mode. CCM mode provides security and
 * authenticity to the message being processed.
 *
 *
 * \note List of devices that support this command - ATECC608A. Refer to device
 *       datasheet for full details.
 *
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
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


/** \brief Initialize context for AES CCM operation with an existing IV, which
 *         is common when starting a decrypt operation.
 *
 * \param[in] ctx          AES CCM context to be initialized
 * \param[in] key_id       Key location. Can either be a slot number or
 *                         ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in] key_block    Index of the 16-byte block to use within the key
 *                         location for the actual key.
 * \param[in] iv           Nonce to be fed into the AES CCM calculation.
 * \param[in] iv_size      Size of iv.
 * \param[in] aad_size     Size of Additional authtication data.
 * \param[in] text_size    Size of plaintext/ciphertext to be processed.
 * \param[in] tag_size     Prefered size of tag.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_ccm_init_ext(ATCADevice device, atca_aes_ccm_ctx_t* ctx, uint16_t key_id, uint8_t key_block, uint8_t* iv, size_t iv_size, size_t aad_size, size_t text_size, size_t tag_size)
{
    ATCA_STATUS status;
    uint8_t M, L;
    size_t i, size_left;
    // First block B of 16 bytes consisting of flags, nonce and l(m).
    uint8_t B[ATCA_AES128_BLOCK_SIZE];
    uint8_t counter[ATCA_AES128_BLOCK_SIZE];

    if (ctx == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    // Length/nonce field specifications according to rfc3610.
    if (iv == NULL || iv_size < 7 || iv_size > 13)
    {
        return ATCA_BAD_PARAM;
    }

    // Auth field specifications according to rfc3610.
    if (tag_size < 3 || tag_size > ATCA_AES128_BLOCK_SIZE || tag_size % 2 != 0)
    {
        return ATCA_BAD_PARAM;
    }

    memset(ctx, 0, sizeof(*ctx));
    ctx->iv_size = (uint8_t)iv_size;

    // --------------------- Init sequence for authentication .......................//
    // Encoding the number of bytes in auth field.
    M = (uint8_t)(tag_size - 2) / 2;
    // Encoding the number of bytes in length field.
    L = (uint8_t)(ATCA_AES128_BLOCK_SIZE - iv_size - 1 - 1);

    // Store M value in ctx for later use.
    ctx->M = M;

    /*----------------------
       Bit Number   Contents
       ----------   ----------------------
       7            Reserved (always zero)
       6            Adata
       5 ... 3      M'
       2 ... 0      L'
       -----------------------*/
    memset(B, 0, ATCA_AES128_BLOCK_SIZE);
    // Formatting flag field
    B[0] = L | (M << 3) | ((aad_size > 0) << 6);

    /*----------------------
       Octet Number   Contents
       ------------   ---------
       0              Flags
       1 ... 15-L     Nonce N
       16-L ... 15    l(m)
       -----------------------*/

    // Copying the IV into the nonce field.
    memcpy(&B[1], iv, iv_size);

    // Update length field in B0 block.
    for (i = 0, size_left = text_size; i < (size_t)(L + 1); i++, size_left >>= 8)
    {
        B[15 - i] = (unsigned char)( size_left & 0xFF );
    }

    // Init CBC-MAC context
    status = atcab_aes_cbcmac_init_ext(device, &ctx->cbc_mac_ctx, key_id, key_block);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    // Processing initial block B0 through CBC-MAC.
    status = atcab_aes_cbcmac_update(&ctx->cbc_mac_ctx, B, ATCA_AES128_BLOCK_SIZE);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    // Loading AAD size in ctx buffer.
    ctx->partial_aad[0] = (uint8_t)(aad_size >> 8) & 0xff;
    ctx->partial_aad[1] = (uint8_t)(aad_size & 0xff);
    ctx->partial_aad_size = 2;

    // --------------------- Init sequence for encryption/decryption .......................//
    memset(counter, 0, ATCA_AES128_BLOCK_SIZE);
    ctx->text_size = text_size;

    /*----------------------
       Bit Number   Contents
       ----------   ----------------------
       7            Reserved (always zero)
       6            Reserved (always zero)
       5 ... 3      Zero
       2 ... 0      L'
       -----------------------*/

    // Updating Flags field
    counter[0] = L;
    /*----------------------
       Octet Number   Contents
       ------------   ---------
       0              Flags
       1 ... 15-L     Nonce N
       16-L ... 15    Counter i
       -----------------------*/
    // Formatting to get the initial counter value
    memcpy(&counter[1], iv, iv_size);
    memcpy(ctx->counter, counter, ATCA_AES128_BLOCK_SIZE);

    // Init CTR mode context with the counter value obtained from previous step.
    status = atcab_aes_ctr_init_ext(device, &ctx->ctr_ctx, key_id, key_block, (uint8_t)(ATCA_AES128_BLOCK_SIZE - iv_size - 1), counter);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    // Increment the counter to skip the first block, first will be later reused to get tag.
    status = atcab_aes_ctr_increment(&ctx->ctr_ctx);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    return ATCA_SUCCESS;
}

/** \brief Initialize context for AES CCM operation with an existing IV, which
 *         is common when starting a decrypt operation.
 *
 * \param[in] ctx          AES CCM context to be initialized
 * \param[in] key_id       Key location. Can either be a slot number or
 *                         ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in] key_block    Index of the 16-byte block to use within the key
 *                         location for the actual key.
 * \param[in] iv           Nonce to be fed into the AES CCM calculation.
 * \param[in] iv_size      Size of iv.
 * \param[in] aad_size     Size of Additional authtication data.
 * \param[in] text_size    Size of plaintext/ciphertext to be processed.
 * \param[in] tag_size     Prefered size of tag.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_ccm_init(atca_aes_ccm_ctx_t* ctx, uint16_t key_id, uint8_t key_block, uint8_t* iv, size_t iv_size, size_t aad_size, size_t text_size, size_t tag_size)
{
    return atcab_aes_ccm_init_ext(atcab_get_device(), ctx, key_id, key_block, iv, iv_size, aad_size, text_size, tag_size);
}

/** \brief Initialize context for AES CCM operation with a random nonce
 *
 * \param[in] ctx          AES CCM context to be initialized
 * \param[in] key_id       Key location. Can either be a slot number or
 *                         ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in] key_block    Index of the 16-byte block to use within the key
 *                         location for the actual key.
 * \param[out] iv          Nonce used for AES CCM calculation is returned here.
 * \param[in] iv_size      Size of iv.
 * \param[in] aad_size     Size of Additional authtication data.
 * \param[in] text_size    Size of plaintext/ciphertext to be processed.
 * \param[in] tag_size     Prefered size of tag.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_ccm_init_rand_ext(ATCADevice device, atca_aes_ccm_ctx_t* ctx, uint16_t key_id, uint8_t key_block,
                                        uint8_t* iv, size_t iv_size, size_t aad_size,
                                        size_t text_size, size_t tag_size)
{
    ATCA_STATUS status;
    uint8_t random_nonce[32];

    // Length/nonce field specifications according to rfc3610.
    if (iv_size < 7 || iv_size > 13)
    {
        // Generating random number to feed into calib_aes_ccm_init function.
        status = atcab_random_ext(device, random_nonce);
        if (status != ATCA_SUCCESS)
        {
            return status;
        }
        memcpy(iv, random_nonce, iv_size);
    }

    // Pass required data along with generated random number to calib_aes_ccm_init function
    status =  atcab_aes_ccm_init_ext(device, ctx, key_id, key_block, iv, iv_size, aad_size, text_size, tag_size);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    return ATCA_SUCCESS;
}

/** \brief Initialize context for AES CCM operation with a random nonce
 *
 * \param[in] ctx          AES CCM context to be initialized
 * \param[in] key_id       Key location. Can either be a slot number or
 *                         ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in] key_block    Index of the 16-byte block to use within the key
 *                         location for the actual key.
 * \param[out] iv          Nonce used for AES CCM calculation is returned here.
 * \param[in] iv_size      Size of iv.
 * \param[in] aad_size     Size of Additional authtication data.
 * \param[in] text_size    Size of plaintext/ciphertext to be processed.
 * \param[in] tag_size     Prefered size of tag.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_ccm_init_rand(atca_aes_ccm_ctx_t* ctx, uint16_t key_id, uint8_t key_block,
                                    uint8_t* iv, size_t iv_size, size_t aad_size,
                                    size_t text_size, size_t tag_size)
{
    return atcab_aes_ccm_init_rand_ext(atcab_get_device(), ctx, key_id, key_block, iv, iv_size, aad_size, text_size, tag_size);
}

/** \brief Process Additional Authenticated Data (AAD) using CCM mode and a
 *         key within the ATECC608A device.
 *
 * This can be called multiple times. calib_aes_ccm_init() or
 * calib_aes_ccm_init_rand() should be called before the first use of this
 * function. When there is AAD to include, this should be called before
 * calib_aes_ccm_encrypt_update() or calib_aes_ccm_decrypt_update().
 *
 * \param[in] ctx       AES CCM context
 * \param[in] aad       Additional authenticated data to be added
 * \param[in] aad_size  Size of aad in bytes
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_ccm_aad_update(atca_aes_ccm_ctx_t* ctx, const uint8_t* aad, size_t aad_size)
{
    ATCA_STATUS status;
    size_t block_count;
    size_t rem_size;
    size_t copy_size;

    if (aad_size == 0)
    {
        return ATCA_SUCCESS;
    }

    if (ctx == NULL || aad == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    rem_size = ATCA_AES128_BLOCK_SIZE - ctx->partial_aad_size;
    copy_size = aad_size > rem_size ? rem_size : aad_size;

    // Copy data into current block
    memcpy(&ctx->partial_aad[ctx->partial_aad_size], aad, copy_size);

    if (ctx->partial_aad_size + aad_size < ATCA_AES128_BLOCK_SIZE)
    {
        // Not enough data to finish off the current block
        ctx->partial_aad_size += aad_size;
        return ATCA_SUCCESS;
    }

    // Process the current block
    if (ATCA_SUCCESS != (status = atcab_aes_cbcmac_update(&ctx->cbc_mac_ctx, ctx->partial_aad, ATCA_AES128_BLOCK_SIZE)))
    {
        return status;
    }

    // Process any additional blocks
    aad_size -= copy_size; // Adjust to the remaining aad bytes
    block_count = aad_size / ATCA_AES128_BLOCK_SIZE;
    if (block_count > 0)
    {
        if (ATCA_SUCCESS != (status = atcab_aes_cbcmac_update(&ctx->cbc_mac_ctx,  &aad[copy_size],  block_count * ATCA_AES128_BLOCK_SIZE)))
        {
            return status;
        }
    }

    // Save any remaining data
    ctx->partial_aad_size = aad_size % ATCA_AES128_BLOCK_SIZE;
    memcpy(ctx->partial_aad, &aad[copy_size + block_count * ATCA_AES128_BLOCK_SIZE], ctx->partial_aad_size);

    return ATCA_SUCCESS;
}

/** \brief Finish processing Additional Authenticated Data (AAD) using CCM mode.
 *
 * This function is called once all additional authentication data has been
 * added into ccm calculation through calib_aes_ccm_aad_update() function.
 *
 * This is an internal function, this function is called by the calib_aes_ccm_update()
 *
 * \param[in] ctx       AES CCM context
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_ccm_aad_finish(atca_aes_ccm_ctx_t* ctx)
{
    ATCA_STATUS status;

    if (ctx == NULL)
    {
        status = ATCA_TRACE(ATCA_BAD_PARAM, "Null pointer");
    }

    // Pad and process any incomplete aad data blocks
    if (ctx->partial_aad_size > 0)
    {
        uint8_t buffer[ATCA_AES128_BLOCK_SIZE];
        memset(buffer, 0, ATCA_AES128_BLOCK_SIZE);
        memcpy(buffer, ctx->partial_aad, ctx->partial_aad_size);

        status = atcab_aes_cbcmac_update(&ctx->cbc_mac_ctx, buffer, ATCA_AES128_BLOCK_SIZE);
        if (status != ATCA_SUCCESS)
        {
            return status;
        }
        // Reset ctx partial aad size variable
        ctx->partial_aad_size = 0;
    }

    return ATCA_SUCCESS;
}

/** \brief Process data using CCM mode and a key within the ATECC608A device.
 *         calib_aes_ccm_init() or calib_aes_ccm_init_rand() should be called
 *         before the first use of this function.
 *
 * \param[in]  ctx         AES CCM context structure.
 * \param[in]  input       Data to be processed.
 * \param[out] output      Output data is returned here.
 * \param[in]  is_encrypt  Encrypt operation if true, otherwise decrypt.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS atcab_aes_ccm_update(atca_aes_ccm_ctx_t* ctx, const uint8_t* input, size_t input_size, uint8_t* output, bool is_encrypt)
{
    ATCA_STATUS status;
    uint32_t data_idx;
    uint32_t i;

    if (input_size == 0)
    {
        // Nothing to do
        return ATCA_SUCCESS;
    }

    if (ctx == NULL || input == NULL || output == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    status = atcab_aes_ccm_aad_finish(ctx);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    data_idx = 0;
    while (data_idx < input_size)
    {
        if (ctx->data_size % ATCA_AES128_BLOCK_SIZE == 0)
        {
            // Need to calculate next encrypted counter block
            if (ATCA_SUCCESS != (status = atcab_aes_encrypt_ext(ctx->ctr_ctx.device, ctx->ctr_ctx.key_id, ctx->ctr_ctx.key_block, ctx->ctr_ctx.cb, ctx->enc_cb)))
            {
                ATCA_TRACE(status, "AES CCM CB encrypt failed");
            }

            // Increment counter
            if (ATCA_SUCCESS != (status = atcab_aes_ctr_increment(&ctx->ctr_ctx)))
            {
                ATCA_TRACE(status, "AES CCM counter increment failed");
            }
        }

        // Process data with current encrypted counter block
        for (i = ctx->data_size % ATCA_AES128_BLOCK_SIZE; i < ATCA_AES128_BLOCK_SIZE && data_idx < input_size; i++, data_idx++)
        {
            output[data_idx] = input[data_idx] ^ ctx->enc_cb[i];
            // Save the current ciphertext block depending on whether this is an encrypt or decrypt operation
            ctx->ciphertext_block[i] = is_encrypt ? input[data_idx] : output[data_idx];
            ctx->data_size += 1;
        }

        if (ctx->data_size % ATCA_AES128_BLOCK_SIZE == 0)
        {
            // Adding data to CBC-MAC to calculate tag
            status = atcab_aes_cbcmac_update(&ctx->cbc_mac_ctx, ctx->ciphertext_block, ATCA_AES128_BLOCK_SIZE);
            if (status != ATCA_SUCCESS)
            {
                return status;
            }
        }
    }

    return ATCA_SUCCESS;
}

/** \brief Process data using CCM mode and a key within the ATECC608A device.
 *         calib_aes_ccm_init() or calib_aes_ccm_init_rand() should be called
 *         before the first use of this function.
 *
 *
 * \param[in]  ctx              AES CCM context structure.
 * \param[in]  plaintext        Data to be processed.
 * \param[out] ciphertext       Output data is returned here.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_ccm_encrypt_update(atca_aes_ccm_ctx_t* ctx, const uint8_t* plaintext, uint32_t plaintext_size, uint8_t* ciphertext)
{
    return atcab_aes_ccm_update(ctx, plaintext, plaintext_size, ciphertext, true);
}

/** \brief Process data using CCM mode and a key within the ATECC608A device.
 *         calib_aes_ccm_init() or calib_aes_ccm_init_rand() should be called
 *         before the first use of this function.
 *
 * \param[in]  ctx              AES CCM context structure.
 * \param[in]  ciphertext       Data to be processed.
 * \param[out] plaintext        Output data is returned here.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_ccm_decrypt_update(atca_aes_ccm_ctx_t* ctx, const uint8_t* ciphertext, uint32_t ciphertext_size, uint8_t* plaintext)
{
    return atcab_aes_ccm_update(ctx, ciphertext, ciphertext_size, plaintext, false);
}

/** \brief Complete a CCM operation returning the authentication tag.
 *
 * \param[in]  ctx       AES CCM context structure.
 * \param[out] tag       Authentication tag is returned here.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
static ATCA_STATUS atcab_aes_ccm_finish(atca_aes_ccm_ctx_t* ctx, uint8_t* tag)
{
    ATCA_STATUS status;

    uint8_t t[ATCA_AES128_BLOCK_SIZE];
    uint8_t u[ATCA_AES128_BLOCK_SIZE];
    uint8_t buffer[ATCA_AES128_BLOCK_SIZE];

    memset(t, 0, ATCA_AES128_BLOCK_SIZE);
    memset(u, 0, ATCA_AES128_BLOCK_SIZE);
    memset(buffer, 0, ATCA_AES128_BLOCK_SIZE);

    if (ctx->data_size % ATCA_AES128_BLOCK_SIZE != 0)
    {
        memcpy(buffer, ctx->ciphertext_block, (ctx->data_size % ATCA_AES128_BLOCK_SIZE));

        // Adding data to CBC-MAC to calculate tag
        status = atcab_aes_cbcmac_update(&ctx->cbc_mac_ctx, buffer, ATCA_AES128_BLOCK_SIZE);
        if (status != ATCA_SUCCESS)
        {
            return status;
        }
    }

    status = atcab_aes_cbcmac_finish(&ctx->cbc_mac_ctx, t, (ctx->M * 2) + 2);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    // Init CTR mode context
    status = atcab_aes_ctr_init_ext(ctx->cbc_mac_ctx.cbc_ctx.device, &ctx->ctr_ctx, ctx->ctr_ctx.key_id, ctx->ctr_ctx.key_block, ATCA_AES128_BLOCK_SIZE - ctx->iv_size, ctx->counter);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    status = atcab_aes_ctr_block(&ctx->ctr_ctx, t, u);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    memcpy(tag, (const void*)u, (ctx->M * 2) + 2);

    return ATCA_SUCCESS;
}

/** \brief Complete a CCM encrypt operation returning the authentication tag.
 *
 * \param[in]   ctx         AES CCM context structure.
 * \param[out]  tag         Authentication tag is returned here.
 * \param[out]  tag_size    Tag size in bytes.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_ccm_encrypt_finish(atca_aes_ccm_ctx_t* ctx, uint8_t* tag, uint8_t *tag_size)
{
    ATCA_STATUS status;

    // Finish and get the tag
    status = atcab_aes_ccm_finish(ctx, tag);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    // Update tag size
    *tag_size = (ctx->M * 2) + 2;
    return ATCA_SUCCESS;
}

/** \brief Complete a CCM decrypt operation authenticating provided tag.
 *
 * \param[in]   ctx             AES CCM context structure.
 * \param[in]   tag             Tag to be authenticated.
 * \param[out]  is_verified     Value is set to true if the tag is authenticated else
 *                              the value is set to false.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_ccm_decrypt_finish(atca_aes_ccm_ctx_t* ctx, const uint8_t* tag, bool *is_verified)
{
    ATCA_STATUS status;
    uint8_t t[ATCA_AES128_BLOCK_SIZE];

    *is_verified = 0;

    // Finish and get the tag
    status = atcab_aes_ccm_finish(ctx, t);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    // Compare with the tag calculated.
    if (memcmp(t, tag, (ctx->M * 2) + 2) == 0)
    {
        *is_verified = 1;
    }

    return ATCA_SUCCESS;
}
