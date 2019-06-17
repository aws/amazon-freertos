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
#ifndef NRF_CLI_UART_H__
#define NRF_CLI_UART_H__

#include "nrf_cli.h"
#include "nrf_drv_uart.h"
#include "nrf_ringbuf.h"
#include "app_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@file
 *
 * @defgroup nrf_cli_uart UART command line interface transport layer
 * @ingroup nrf_cli
 *
 * @{
 *
 */

/**
 * @brief Command line interface transport.
 */
extern const nrf_cli_transport_api_t nrf_cli_uart_transport_api;

typedef struct nrf_cli_uart_internal_s nrf_cli_uart_internal_t;

typedef struct {
    nrf_cli_transport_handler_t   handler;
    void *                        p_context;
    nrf_drv_uart_config_t         uart_config;
    bool                          timer_created;
    bool                          blocking;
} nrf_cli_uart_internal_cb_t;

struct nrf_cli_uart_internal_s {
    nrf_cli_transport_t          transport;
    nrf_cli_uart_internal_cb_t * p_cb;
    app_timer_id_t const *       p_timer;
    nrf_ringbuf_t const *        p_rx_ringbuf;
    nrf_ringbuf_t const *        p_tx_ringbuf;
    nrf_drv_uart_t const *       p_uart;
};

typedef nrf_drv_uart_config_t nrf_cli_uart_config_t;

/**@brief CLI UART transport definition.
 *
 * @param _name      Name of the instance.
 * @param _uart_id   UART instance ID.
 * @param _tx_buf_sz Size of TX ring buffer.
 * @param _rx_buf_sz Size of RX ring buffer.
 */
#define NRF_CLI_UART_DEF(_name, _uart_id, _tx_buf_sz, _rx_buf_sz)   \
    APP_TIMER_DEF(CONCAT_2(_name, _timer));                         \
    NRF_RINGBUF_DEF(CONCAT_2(_name,_tx_ringbuf), _tx_buf_sz);       \
    NRF_RINGBUF_DEF(CONCAT_2(_name,_rx_ringbuf), _rx_buf_sz);       \
    static const nrf_drv_uart_t CONCAT_2(_name,_uart) =             \
                                 NRF_DRV_UART_INSTANCE(_uart_id);   \
    static nrf_cli_uart_internal_cb_t CONCAT_2(_name, _cb);         \
    static const nrf_cli_uart_internal_t _name = {                  \
        .transport = {.p_api = &nrf_cli_uart_transport_api},        \
        .p_cb = &CONCAT_2(_name, _cb),                              \
        .p_timer = &CONCAT_2(_name, _timer),                        \
        .p_rx_ringbuf = &CONCAT_2(_name,_rx_ringbuf),               \
        .p_tx_ringbuf = &CONCAT_2(_name,_tx_ringbuf),               \
        .p_uart       = &CONCAT_2(_name,_uart),                     \
    }
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRF_CLI_UART_H__ */
