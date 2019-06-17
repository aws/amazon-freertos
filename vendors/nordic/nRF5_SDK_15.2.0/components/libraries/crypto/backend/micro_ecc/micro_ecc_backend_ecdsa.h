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

#ifndef MICRO_ECC_BACKEND_ECDSA_H__
#define MICRO_ECC_BACKEND_ECDSA_H__

#include "sdk_config.h"
#include "nordic_common.h"

#if NRF_MODULE_ENABLED(NRF_CRYPTO) && NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MICRO_ECC)

#include "nrf_crypto_ecc_shared.h"
#include "nrf_crypto_ecdsa_shared.h"


#ifdef __cplusplus
extern "C" {
#endif


/** @internal See @ref nrf_crypto_backend_ecdsa_sign_fn_t.
 */
ret_code_t nrf_crypto_backend_micro_ecc_sign(
    void           * p_context,
    void     const * p_private_key,
    uint8_t  const * p_data,
    size_t           data_size,
    uint8_t        * p_signature);


/** @internal See @ref nrf_crypto_backend_ecdsa_verify_fn_t.
 */
ret_code_t nrf_crypto_backend_micro_ecc_verify(
    void           * p_context,
    void     const * p_public_key,
    uint8_t  const * p_data,
    size_t           data_size,
    uint8_t  const * p_signature);


#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MICRO_ECC_ECC_SECP192R1)
#define NRF_CRYPTO_BACKEND_SECP192R1_SIGN_CONTEXT_SIZE 0
#define NRF_CRYPTO_BACKEND_SECP192R1_VERIFY_CONTEXT_SIZE 0
typedef uint32_t nrf_crypto_backend_secp192r1_sign_context_t;
typedef uint32_t nrf_crypto_backend_secp192r1_verify_context_t;
#define nrf_crypto_backend_secp192r1_sign nrf_crypto_backend_micro_ecc_sign
#define nrf_crypto_backend_secp192r1_verify nrf_crypto_backend_micro_ecc_verify
#endif


#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MICRO_ECC_ECC_SECP224R1)
#define NRF_CRYPTO_BACKEND_SECP224R1_SIGN_CONTEXT_SIZE 0
#define NRF_CRYPTO_BACKEND_SECP224R1_VERIFY_CONTEXT_SIZE 0
typedef uint32_t nrf_crypto_backend_secp224r1_sign_context_t;
typedef uint32_t nrf_crypto_backend_secp224r1_verify_context_t;
#define nrf_crypto_backend_secp224r1_sign nrf_crypto_backend_micro_ecc_sign
#define nrf_crypto_backend_secp224r1_verify nrf_crypto_backend_micro_ecc_verify
#endif


#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MICRO_ECC_ECC_SECP256R1)
#define NRF_CRYPTO_BACKEND_SECP256R1_SIGN_CONTEXT_SIZE 0
#define NRF_CRYPTO_BACKEND_SECP256R1_VERIFY_CONTEXT_SIZE 0
typedef uint32_t nrf_crypto_backend_secp256r1_sign_context_t;
typedef uint32_t nrf_crypto_backend_secp256r1_verify_context_t;
#define nrf_crypto_backend_secp256r1_sign nrf_crypto_backend_micro_ecc_sign
#define nrf_crypto_backend_secp256r1_verify nrf_crypto_backend_micro_ecc_verify
#endif


#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MICRO_ECC_ECC_SECP256K1)
#define NRF_CRYPTO_BACKEND_SECP256K1_SIGN_CONTEXT_SIZE 0
#define NRF_CRYPTO_BACKEND_SECP256K1_VERIFY_CONTEXT_SIZE 0
typedef uint32_t nrf_crypto_backend_secp256k1_sign_context_t;
typedef uint32_t nrf_crypto_backend_secp256k1_verify_context_t;
#define nrf_crypto_backend_secp256k1_sign nrf_crypto_backend_micro_ecc_sign
#define nrf_crypto_backend_secp256k1_verify nrf_crypto_backend_micro_ecc_verify
#endif


#ifdef __cplusplus
}
#endif

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO) && NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MICRO_ECC)

#endif // MICRO_ECC_BACKEND_ECDSA_H__
