/**
 * \file
 * \brief CryptoAuthLib Basic API methods for AES CBC_MAC mode.
 *
 * The AES command supports 128-bit AES encryption or decryption of small
 * messages or data packets in ECB mode. Also can perform GFM (Galois Field
 * Multiply) calculation in support of AES-GCM.
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

/** \brief Initialize context for AES CBC-MAC operation.
 *
 * \param[in] ctx        AES CBC-MAC context to be initialized
 * \param[in] key_id     Key location. Can either be a slot number or
 *                       ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in] key_block  Index of the 16-byte block to use within the key
 *                       location for the actual key.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_cbcmac_init_ext(ATCADevice device, atca_aes_cbcmac_ctx_t* ctx, uint16_t key_id, uint8_t key_block)
{
    if (ctx == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    // Initializing the atca_aes_cbcmac_ctx_t structure, this will set the IV value to 0.
    // IV value must be set to zero for CBC-MAC calculation
    memset(ctx, 0, sizeof(*ctx));
    ctx->cbc_ctx.device = device;
    ctx->cbc_ctx.key_id = key_id;
    ctx->cbc_ctx.key_block = key_block;

    return ATCA_SUCCESS;
}

/** \brief Initialize context for AES CBC-MAC operation.
 *
 * \param[in] ctx        AES CBC-MAC context to be initialized
 * \param[in] key_id     Key location. Can either be a slot number or
 *                       ATCA_TEMPKEY_KEYID for TempKey.
 * \param[in] key_block  Index of the 16-byte block to use within the key
 *                       location for the actual key.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_cbcmac_init(atca_aes_cbcmac_ctx_t* ctx, uint16_t key_id, uint8_t key_block)
{
    return atcab_aes_cbcmac_init_ext(atcab_get_device(), ctx, key_id, key_block);
}


/** \brief Calculate AES CBC-MAC with key stored within ECC608A device.
 *         calib_aes_cbcmac_init() should be called before the first use of
 *         this function.
 *
 * \param[in] ctx           AES CBC-MAC context structure.
 * \param[in] data          Data to be added for AES CBC-MAC calculation.
 * \param[in] data_size     Data length in bytes.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_cbcmac_update(atca_aes_cbcmac_ctx_t* ctx, const uint8_t* data, uint32_t data_size)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    size_t i;
    uint8_t ciphertext[ATCA_AES128_BLOCK_SIZE];

    if (data_size == 0)
    {
        // Nothing to do
        return ATCA_SUCCESS;
    }

    if (ctx == NULL || data == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    // Process full blocks of data with AES-CBC
    for (i = 0; i < data_size / ATCA_AES128_BLOCK_SIZE; i++)
    {
        status = atcab_aes_cbc_encrypt_block(&ctx->cbc_ctx, &data[i * ATCA_AES128_BLOCK_SIZE], ciphertext);
        if (status != ATCA_SUCCESS)
        {
            return status;
        }
    }

    // Store incomplete block to context structure
    if ((i * ATCA_AES128_BLOCK_SIZE) < data_size)
    {
        memcpy(ctx->block, &data[i * ATCA_AES128_BLOCK_SIZE], data_size - i * ATCA_AES128_BLOCK_SIZE);
        ctx->block_size = (uint8_t)(data_size - i * ATCA_AES128_BLOCK_SIZE);
    }
    else
    {
        ctx->block_size = 0;
    }

    return ATCA_SUCCESS;
}

/** \brief Finish a CBC-MAC operation returning the CBC-MAC value. If the data
 *         provided to the calib_aes_cbcmac_update() function has incomplete
 *         block this function will return an error code.
 *
 * \param[in]  ctx        AES-128 CBC-MAC context.
 * \param[out] mac        CBC-MAC is returned here.
 * \param[in]  mac_size   Size of CBC-MAC requested in bytes (max 16 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_aes_cbcmac_finish(atca_aes_cbcmac_ctx_t* ctx, uint8_t* mac, uint32_t mac_size)
{
    if (ctx == NULL || mac == NULL || mac_size > ATCA_AES128_BLOCK_SIZE)
    {
        return ATCA_BAD_PARAM;
    }

    // Check for incomplete data block
    if (ctx->block_size == 0)
    {
        // All processing is already done, copying the mac to result buffer
        memcpy(mac, ctx->cbc_ctx.ciphertext, mac_size);
    }
    else
    {
        // Returns INVALID_SIZE if incomplete blocks are present
        return ATCA_INVALID_SIZE;
    }

    return ATCA_SUCCESS;
}