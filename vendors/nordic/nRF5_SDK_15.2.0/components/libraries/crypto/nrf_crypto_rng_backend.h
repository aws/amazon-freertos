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

#ifndef NRF_CRYPTO_RNG_BACKEND_H__
#define NRF_CRYPTO_RNG_BACKEND_H__

/** @file
 *
 * @defgroup nrf_crypto_rng_backend Meta RNG backend.
 * @{
 * @ingroup nrf_crypto_rng
 *
 * @brief Includes all backends definitions.
 *
 * @details This file includes all backend definitions.
 */

#include "cc310_backend_rng.h"
#include "nrf_hw_backend_rng.h"
#include "nrf_hw_backend_rng_mbedtls.h"

#if !NRF_MODULE_ENABLED(NRF_CRYPTO_RNG)

#ifdef __cplusplus
extern "C" {
#endif

/** @internal @brief Fallback dummy context type in case no backend is selected */
typedef nrf_crypto_rng_internal_context_t nrf_crypto_backend_rng_context_t;

/** @internal @brief Fallback dummy temp buffer type in case no backend is selected . */
typedef struct
{
    uint32_t reserved;
} nrf_crypto_backend_rng_temp_buffer_t;

#ifdef __cplusplus
}
#endif

#endif // !NRF_MODULE_ENABLED(NRF_CRYPTO_RNG)

/**@} */

#endif // NRF_CRYPTO_RNG_BACKEND_H__
