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
#include "nrf_soc.h"
#include "nrf_soc_conn.h"
#include "nrf_error.h"
#include "ble_serialization.h"
#include "cond_field_serialization.h"
#include "nrf_soc_struct_serialization.h"

uint32_t ecb_block_encrypt_req_dec(uint8_t const * const            p_buf,
                                   uint32_t                         packet_len,
                                   nrf_ecb_hal_data_t * * const     pp_ecb_data)
{
    SER_REQ_DEC_BEGIN(SD_ECB_BLOCK_ENCRYPT);
    SER_PULL_COND(pp_ecb_data, nrf_ecb_hal_data_t_in_dec);
    SER_REQ_DEC_END;
}

uint32_t ecb_block_encrypt_rsp_enc(uint32_t         return_code,
                                   uint8_t * const  p_buf,
                                   uint32_t * const p_buf_len,
                                   nrf_ecb_hal_data_t * const  p_ecb_data)
{
    SER_RSP_ENC_BEGIN(SD_ECB_BLOCK_ENCRYPT);
    SER_PUSH_COND(p_ecb_data, nrf_ecb_hal_data_t_out_enc);
    SER_RSP_ENC_END;
}
uint32_t power_system_off_req_dec(uint8_t const * const p_buf,
                                  uint16_t              packet_len)
{
    SER_REQ_DEC_BEGIN(SD_POWER_SYSTEM_OFF);
    SER_REQ_DEC_END;
}

uint32_t temp_get_req_dec(uint8_t const * const p_buf,
                          uint32_t              packet_len,
                          int32_t * * const     pp_temp)
{
    SER_REQ_DEC_BEGIN(SD_TEMP_GET);
    SER_PULL_COND(pp_temp, NULL);
    SER_REQ_DEC_END;
}

uint32_t temp_get_rsp_enc(uint32_t         return_code,
                          uint8_t * const  p_buf,
                          uint32_t * const p_buf_len,
                          int32_t * const  p_temp)
{
    SER_RSP_ENC_BEGIN(SD_TEMP_GET);
    SER_PUSH_COND(p_temp, uint32_t_enc);
    SER_RSP_ENC_END;
}
