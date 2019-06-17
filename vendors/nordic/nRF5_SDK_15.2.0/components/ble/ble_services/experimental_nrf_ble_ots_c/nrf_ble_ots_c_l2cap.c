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

#include "sdk_common.h"
#if NRF_MODULE_ENABLED(BLE_OTS_C_L2CAP)
#include <stdlib.h>
#include "nrf_ble_ots_c.h"
#include "nrf_ble_ots_c_oacp.h"
#include "nrf_ble_ots_c_l2cap.h"
#include "ble.h"

#define NRF_LOG_MODULE_NAME ble_ots_c_l2cap
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#include "sdk_common.h"

#define MODULE_INITIALIZED (p_ots_c->initialized)   /**< Macro designating whether the module has been initialized properly. */


/**@brief Function for handling the BLE_L2CAP_EVT_CH_SETUP event.
 *
 * @param[in] p_ots_c   Object Transfer Service Instance.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_l2cap_ch_setup_complete(nrf_ble_ots_c_t * const p_ots_c,
                                    ble_evt_t const * const p_ble_evt)
{
    p_ots_c->local_cid                   = p_ble_evt->evt.l2cap_evt.local_cid;
    p_ots_c->ch_setup.tx_params.peer_mps = p_ble_evt->evt.l2cap_evt.params.ch_setup.tx_params.peer_mps;
    p_ots_c->ch_setup.tx_params.tx_mtu   = p_ble_evt->evt.l2cap_evt.params.ch_setup.tx_params.tx_mtu; 
    p_ots_c->ch_setup.tx_params.credits  = p_ble_evt->evt.l2cap_evt.params.ch_setup.tx_params.credits;
}


/**@brief This function sends the next MTU of the object.
 *
 * @param[in] p_ots_c  Object Transfer Service Instance.
 */
static void send_resume(nrf_ble_ots_c_t * const p_ots_c)
{
    ret_code_t err_code;
    uint16_t   tx_size;

    tx_size = MIN((p_ots_c->current_obj->len - p_ots_c->transmitted_bytes),
                  p_ots_c->ch_setup.tx_params.tx_mtu);

    ble_data_t obj;
    obj.len    = tx_size;
    obj.p_data = &p_ots_c->current_obj->p_data[p_ots_c->transmitted_bytes];

    err_code = sd_ble_l2cap_ch_tx(p_ots_c->conn_handle, p_ots_c->local_cid, &obj);

    if (err_code == NRF_ERROR_RESOURCES)
    {
        return; // Too many SDUs queued for transmission, the transmission will be tried again on the next BLE_L2CAP_EVT_CH_TX event.
    }
    if(err_code != NRF_SUCCESS && p_ots_c->err_handler != NULL)
    {
        p_ots_c->err_handler(err_code);
    }
}


/**@brief This function receives the next of the object.
 *
 * @param[in] p_ots_c  Object Transfer Service Instance.
 */
static void receive_resume(nrf_ble_ots_c_t * const p_ots_c)
{
    ret_code_t err_code;
    ble_data_t sdu_buf;

    sdu_buf.p_data = &p_ots_c->current_obj->p_data[p_ots_c->received_bytes];
    sdu_buf.len    = p_ots_c->current_obj->len - p_ots_c->received_bytes;

    err_code = sd_ble_l2cap_ch_rx(p_ots_c->conn_handle,
                                  p_ots_c->local_cid,
                                  &sdu_buf);

    if (err_code == NRF_ERROR_RESOURCES)
    {
        return; // Too many SDUs queued for transmission, the transmission will be tried again on the next BLE_L2CAP_EVT_CH_RX event.
    }

    if(err_code != NRF_SUCCESS && p_ots_c->err_handler != NULL)
    {
        p_ots_c->err_handler(err_code);
    }
}


/**@brief Function for handling the BLE_L2CAP_EVT_CH_TX event.
 *
 * @param[in] p_ots_c   Object Transfer Service Instance.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_l2cap_ch_tx(nrf_ble_ots_c_t * const p_ots_c, ble_evt_t const * const p_ble_evt)
{
    if(p_ots_c->local_cid != p_ble_evt->evt.l2cap_evt.local_cid)
    {
        return;
    }

    NRF_LOG_DEBUG("Bytes sent in the previous PDU: %i",
                  p_ble_evt->evt.l2cap_evt.params.tx.sdu_buf.len);
    NRF_LOG_HEXDUMP_DEBUG(p_ble_evt->evt.l2cap_evt.params.tx.sdu_buf.p_data,
                          p_ble_evt->evt.l2cap_evt.params.tx.sdu_buf.len);

    p_ots_c->transmitted_bytes += p_ble_evt->evt.l2cap_evt.params.tx.sdu_buf.len;
    uint16_t remaining_tx_bytes = p_ots_c->current_obj->len - p_ots_c->transmitted_bytes;

    NRF_LOG_DEBUG("Total bytes transmitted: %i ",
                  (p_ots_c->transmitted_bytes));
    NRF_LOG_DEBUG("Total bytes remaining: %i",
                  (remaining_tx_bytes));

    if (remaining_tx_bytes == 0)
    {
        nrf_ble_ots_c_evt_t evt;

        evt.evt_type             = NRF_BLE_OTS_C_EVT_OBJ_WRITE;
        evt.params.object.len    = p_ots_c->current_obj->len;
        evt.params.object.p_data = p_ots_c->current_obj->p_data;
        p_ots_c->evt_handler(&evt);
    }
    else
    {
        send_resume(p_ots_c);
    }
}


/**@brief Function for handling the BLE_L2CAP_EVT_CH_TR event.
 *
 * @param[in] p_ots_c   Object Transfer Service Instance.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_l2cap_ch_rx(nrf_ble_ots_c_t * const p_ots_c,
                           ble_evt_t const * const p_ble_evt)
{
    if(p_ots_c->local_cid != p_ble_evt->evt.l2cap_evt.local_cid)
    {
        return;
    }

    NRF_LOG_DEBUG("Bytes received: %i", p_ble_evt->evt.l2cap_evt.params.rx.sdu_len);
    NRF_LOG_HEXDUMP_DEBUG(p_ble_evt->evt.l2cap_evt.params.rx.sdu_buf.p_data,
                          p_ble_evt->evt.l2cap_evt.params.rx.sdu_len);

    memcpy(&p_ots_c->current_obj->p_data[p_ots_c->received_bytes],  //Not needed`? buffer provied
           p_ble_evt->evt.l2cap_evt.params.rx.sdu_buf.p_data,
           p_ble_evt->evt.l2cap_evt.params.rx.sdu_len);

    p_ots_c->received_bytes += p_ble_evt->evt.l2cap_evt.params.rx.sdu_len;
    uint16_t remaining_bytes = (p_ots_c->current_obj->len - p_ots_c->received_bytes);

    if(remaining_bytes == 0)
    {
        nrf_ble_ots_c_evt_t evt;

        evt.evt_type             = NRF_BLE_OTS_C_EVT_OBJ_READ;
        evt.params.object.len    = p_ots_c->current_obj->len;
        evt.params.object.p_data = p_ots_c->current_obj->p_data;
        p_ots_c->evt_handler(&evt);
    }
    else
    {
        receive_resume(p_ots_c);
    }
}


/**@brief Function for handling the BLE_L2CAP_EVT_CH_RELEASED event.
 *
 * @param[in] p_ots_c   Object Transfer Service Instance.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_l2cap_ch_released(nrf_ble_ots_c_t * const p_ots_c,
                                 ble_evt_t const * const p_ble_evt)
{
    nrf_ble_ots_c_evt_t evt;

    evt.evt_type = NRF_BLE_OTS_C_EVT_CHANNEL_RELEASED;
    p_ots_c->evt_handler(&evt);
}


ret_code_t nrf_ble_ots_c_l2cap_obj_send(nrf_ble_ots_c_t * const p_ots_c, ble_data_t * p_obj)
{
    VERIFY_MODULE_INITIALIZED();
    VERIFY_PARAM_NOT_NULL(p_ots_c);
    VERIFY_PARAM_NOT_NULL(p_obj);

    ret_code_t err_code;
    uint16_t   tx_size;

    p_ots_c->transmitted_bytes = 0;
    tx_size                    = MIN(p_obj->len, p_ots_c->ch_setup.tx_params.tx_mtu);
    p_ots_c->current_obj       = p_obj;

    ble_data_t obj;
    obj.len    = tx_size;
    obj.p_data = p_obj->p_data;
    err_code   = sd_ble_l2cap_ch_tx(p_ots_c->conn_handle, p_ots_c->local_cid, &obj);
    return err_code;
}


ret_code_t nrf_ble_ots_c_l2cap_obj_receive(nrf_ble_ots_c_t * const p_ots_c, ble_data_t  * p_obj)
{
    uint32_t err_code;
    p_ots_c->received_bytes = 0;
    p_ots_c->current_obj    = p_obj;

    err_code = sd_ble_l2cap_ch_rx(p_ots_c->conn_handle,
                                  p_ots_c->local_cid,
                                  p_ots_c->current_obj);
    return err_code;
}


void ots_c_l2cap_on_ble_evt(nrf_ble_ots_c_t * const p_ots_c,
                            ble_evt_t const  * const p_ble_evt)
{
    VERIFY_MODULE_INITIALIZED_VOID();
    VERIFY_PARAM_NOT_NULL_VOID(p_ots_c);
    VERIFY_PARAM_NOT_NULL_VOID(p_ble_evt);

    if (p_ble_evt->evt.l2cap_evt.local_cid != p_ots_c->local_cid)
    {
        return;
    }

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_L2CAP_EVT_CH_SETUP:
            NRF_LOG_DEBUG("BLE_L2CAP_EVT_CH_SETUP");
            on_l2cap_ch_setup_complete(p_ots_c, p_ble_evt);
            break;

        case BLE_L2CAP_EVT_CH_TX:
            NRF_LOG_DEBUG("BLE_L2CAP_EVT_CH_TX");
            on_l2cap_ch_tx(p_ots_c, p_ble_evt);
            break;

        case BLE_L2CAP_EVT_CH_RX:
            NRF_LOG_DEBUG("BLE_L2CAP_EVT_CH_RX");
            on_l2cap_ch_rx(p_ots_c, p_ble_evt);
            break;

        case BLE_L2CAP_EVT_CH_CREDIT:
            NRF_LOG_DEBUG("BLE_L2CAP_EVT_CH_CREDIT");
            break;

        case BLE_L2CAP_EVT_CH_RELEASED:
            NRF_LOG_DEBUG("BLE_L2CAP_EVT_CH_RELEASED");
            on_l2cap_ch_released(p_ots_c, p_ble_evt);
            break;

        default:
            break;
    }
}
#endif // NRF_MODULE_ENABLED(BLE_OTS_C_L2CAP)
