/**
 * Copyright (c) 2017 - 2018, Nordic Semiconductor ASA
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
#ifndef NRF_CLI_CDC_ACM_H__
#define NRF_CLI_CDC_ACM_H__

#include <stdbool.h>

#include "nrf_cli.h"
#include "app_usbd_cdc_acm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@file
 *
 * @defgroup nrf_cli_cdc_acm CDC ACM command line interface transport layer
 * @ingroup nrf_cli
 *
 * @{
 *
 */

/**
 * @brief Command line interface transport.
 */
extern const nrf_cli_transport_api_t nrf_cli_cdc_acm_transport_api;

/**
 * @brief Command line interface class instance.
 */
extern const app_usbd_cdc_acm_t nrf_cli_cdc_acm;

typedef struct {
    nrf_cli_transport_handler_t handler;
    void *                      p_context;
} nrf_cli_cdc_acm_internal_cb_t;

typedef struct {
    nrf_cli_transport_t             transport;
    nrf_cli_cdc_acm_internal_cb_t * p_cb;
} nrf_cli_cdc_acm_internal_t;

/**@brief CLI USB transport definition */
#define NRF_CLI_CDC_ACM_DEF(_name_)                             \
    static nrf_cli_cdc_acm_internal_cb_t CONCAT_2(_name_, _cb); \
    static const nrf_cli_cdc_acm_internal_t _name_ = {          \
        .transport = {.p_api = &nrf_cli_cdc_acm_transport_api}, \
        .p_cb = &CONCAT_2(_name_, _cb),                         \
    }

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRF_CLI_CDC_ACM_H__ */
