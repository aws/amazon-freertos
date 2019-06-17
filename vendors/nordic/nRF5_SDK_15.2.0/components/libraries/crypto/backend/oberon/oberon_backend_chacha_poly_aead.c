/**
 * Copyright (c) 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "sdk_common.h"
#if NRF_MODULE_ENABLED(NRF_CRYPTO)

#include <stdbool.h>
#include "oberon_backend_chacha_poly_aead.h"

#if NRF_MODULE_ENABLED(NRF_CRYPTO_OBERON_CHACHA_POLY_AEAD)

static ret_code_t backend_cc310_init(void * const p_context, uint8_t * p_key)
{
    nrf_crypto_backend_chacha_poly_context_t * p_ctx =
        (nrf_crypto_backend_chacha_poly_context_t *)p_context;


    if (p_ctx->header.p_info->key_size != NRF_CRYPTO_KEY_SIZE_256)
    {
        return NRF_ERROR_CRYPTO_KEY_SIZE;
    }

    memcpy(p_ctx->key, p_key, sizeof(p_ctx->key));

    return NRF_SUCCESS;
}

static inline ret_code_t backend_cc310_uninit(void * const p_context)
{
    return NRF_SUCCESS;
}

static ret_code_t backend_cc310_crypt(void * const           p_context,
                                      nrf_crypto_operation_t operation,
                                      uint8_t *              p_nonce,
                                      uint8_t                nonce_size,
                                      uint8_t *              p_adata,
                                      size_t                 adata_size,
                                      uint8_t *              p_data_in,
                                      size_t                 data_in_size,
                                      uint8_t *              p_data_out,
                                      uint8_t *              p_mac,
                                      uint8_t                mac_size)

{
    int result;

    nrf_crypto_backend_chacha_poly_context_t * p_ctx =
        (nrf_crypto_backend_chacha_poly_context_t *)p_context;

    if ((adata_size == 0) || (data_in_size == 0))
    {
        return NRF_ERROR_CRYPTO_INPUT_LENGTH;
    }

    if (mac_size   != NRF_CRYPTO_CHACHA_POLY_MAC_SIZE)
    {
        return NRF_ERROR_CRYPTO_AEAD_MAC_SIZE;
    }

    if (nonce_size != NRF_CRYPTO_CHACHA_POLY_NONCE_SIZE)
    {
        return NRF_ERROR_CRYPTO_AEAD_NONCE_SIZE;
    }

    if (operation == NRF_CRYPTO_ENCRYPT)
    {
        occ_chacha20_poly1305_encrypt_aad(p_mac,
                                          p_data_out,
                                          p_data_in,
                                          data_in_size,
                                          p_adata,
                                          adata_size,
                                          p_nonce,
                                          (size_t)nonce_size,
                                          p_ctx->key);
    }
    else if (operation == NRF_CRYPTO_DECRYPT)
    {
        result = occ_chacha20_poly1305_decrypt_aad(p_mac,
                                                   p_data_out,
                                                   p_data_in,
                                                   data_in_size,
                                                   p_adata,
                                                   adata_size,
                                                   p_nonce,
                                                   (size_t)nonce_size,
                                                   p_ctx->key);

        if (result != 0)
        {
            return NRF_ERROR_CRYPTO_AEAD_INVALID_MAC;
        }
    }
    else
    {
        return NRF_ERROR_CRYPTO_INVALID_PARAM;
    }

    return NRF_SUCCESS;
}

nrf_crypto_aead_info_t const g_nrf_crypto_chacha_poly_256_info =
{
    .key_size  = NRF_CRYPTO_KEY_SIZE_256,
    .mode      = NRF_CRYPTO_AEAD_MODE_CHACHA_POLY,

    .init_fn   = backend_cc310_init,
    .uninit_fn = backend_cc310_uninit,
    .crypt_fn  = backend_cc310_crypt
};


#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_CC310_CHACHA_POLY_AEAD)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO)

