/**
 * Copyright (c) 2016 - 2018, Nordic Semiconductor ASA
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

#ifndef NRF_DRV_RNG_H__
#define NRF_DRV_RNG_H__

#include <nrfx_rng.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_drv_rng RNG driver - legacy layer
 * @{
 * @ingroup  nrf_rng
 *
 * @brief    Layer providing compatibility with the former API.
 */

/** @brief Type definition for forwarding the new implementation. */
typedef nrfx_rng_config_t   nrf_drv_rng_config_t;

/** @brief Macro for forwarding the new implementation. */
#define NRF_DRV_RNG_DEFAULT_CONFIG  NRFX_RNG_DEFAULT_CONFIG

/**
 * @brief Function for initializing the nrf_drv_rng module.
 *
 * @param[in]  p_config                 Initial configuration.
 *
 * @retval  NRF_SUCCESS                             Driver was successfully initialized.
 * @retval  NRF_ERROR_MODULE_ALREADY_INITIALIZED    Driver was already initialized.
 */
ret_code_t nrf_drv_rng_init(nrf_drv_rng_config_t const * p_config);

/**
 * @brief Function for uninitializing the nrf_drv_rng module.
 */
void nrf_drv_rng_uninit(void);

/**
 * @brief Function for getting the number of currently available random bytes.
 *
 * @param[out] p_bytes_available        The number of bytes currently available in the pool.
 */
void nrf_drv_rng_bytes_available(uint8_t * p_bytes_available);

/**
 * @brief Function for getting the vector of random numbers.
 *
 * @param[out] p_buff                   Pointer to uint8_t buffer for storing the bytes.
 * @param[in]  length                   Number of bytes to take from the pool and place in p_buff.
 *
 * @retval     NRF_SUCCESS              If the requested bytes were written to p_buff.
 * @retval     NRF_ERROR_NOT_FOUND      If no bytes were written to the buffer because there were
 *                                      not enough bytes available in the pool.
 */
ret_code_t nrf_drv_rng_rand(uint8_t * p_buff, uint8_t length);

/**
 * @brief Blocking function for getting an arbitrary array of random numbers.
 *
 * @note This function may execute for a substantial amount of time depending on the length
 *       of the buffer required and on the state of the current internal pool of random numbers.
 *
 * @param[out] p_buff                   Pointer to uint8_t buffer for storing the bytes.
 * @param[in]  length                   Number of bytes place in p_buff.
 */
void nrf_drv_rng_block_rand(uint8_t * p_buff, uint32_t length);

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_DRV_RNG_H__
