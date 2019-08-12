/**
 * Copyright (c) 2014 - 2018, Nordic Semiconductor ASA
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
#ifndef BLE_DTM_APP_H__
#define BLE_DTM_APP_H__

/**
 * @addtogroup ser_codecs Serialization codecs
 * @ingroup ble_sdk_lib_serialization
 * @brief Application and Connectivity codecs.
 */

/**
 * @addtogroup ser_codecs_app Application codecs
 * @ingroup ser_codecs
 */

/**
 * @addtogroup ser_app_common_codecs Application common codecs
 * @ingroup ser_codecs_app
 */

/**@file
 *
 * @defgroup ble_dtm_app DTM Application command request encoders and command response decoders
 * @{
 * @ingroup  ser_app_common_codecs
 *
 * @brief   DTM Application command request encoders and command response decoders.
 */

#include "dtm_uart_params.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@brief Encodes the @ref ble_dtm_init command request.
 *
 * @sa @ref encoding_data for packet format,
 *     @ref ble_dtm_init_rsp_dec for command response decoder.
 *
 * @param[in] p_uart_comm_params    Pointer to the UART configuration parameters.
 * @param[in] p_buf                 Pointer to the buffer where encoded data command will be returned.
 * @param[in,out] p_buf_len         \c in: Size of \p p_buf buffer.
 *                                  \c out: Length of the encoded command packet.
 *
 * @retval NRF_SUCCESS                Encoding success.
 * @retval NRF_ERROR_NULL             Encoding failure. NULL pointer supplied.
 * @retval NRF_ERROR_INVALID_LENGTH   Encoding failure. Incorrect buffer length.
 */
uint32_t ble_dtm_init_req_enc(app_uart_stream_comm_params_t const * const p_uart_comm_params,
                              uint8_t                             * const p_buf,
                              uint32_t                            * const p_buf_len);


/**@brief Decodes the response @ref ble_dtm_init command.
 *
 * @sa @ref encoding_data for packet format,
 *     @ref ble_dtm_init_req_enc for command request encoder.
 *
 * @param[in] p_buf             Pointer to the beginning of a command response packet.
 * @param[in] packet_len        Length (in bytes) of the response packet.
 * @param[out] p_result_code    Command result code.
 *
 * @retval NRF_SUCCESS               Decoding success.
 * @retval NRF_ERROR_NULL            Decoding failure. NULL pointer supplied.
 * @retval NRF_ERROR_INVALID_LENGTH  Decoding failure. Incorrect buffer length.
 */
uint32_t ble_dtm_init_rsp_dec(uint8_t const * const p_buf,
                              uint32_t              packet_len,
                              uint32_t * const      p_result_code);


/**@brief Function to initializing the DTM mode.
 *
 * @param[in] p_uart_comm_params    Pointer to the DTM UART configuration parameters.
 *
 * @retval NRF_SUCCESS          Encoding success.
 * @retval NRF_ERROR_NULL       Encoding failure. NULL pointer supplied.
 */
uint32_t ble_dtm_init(app_uart_stream_comm_params_t * p_uart_comm_params);

/** @} */

#ifdef __cplusplus
}
#endif

#endif // BLE_DTM_APP_H__
