/**
 * Copyright (c) 2012 - 2018, Nordic Semiconductor ASA
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
/* Attention!
 * To maintain compliance with Nordic Semiconductor ASA's Bluetooth profile
 * qualification listings, this section of source code must not be modified.
 */

#include "sdk_common.h"

#if NRF_MODULE_ENABLED(BLE_RSCS)

#include "ble_rscs.h"
#include <string.h>
#include "ble.h"
#include "ble_srv_common.h"

#define OPCODE_LENGTH 1                                                             /**< Length of opcode inside Running Speed and Cadence Measurement packet. */
#define HANDLE_LENGTH 2                                                             /**< Length of handle inside Running Speed and Cadence Measurement packet. */
#define MAX_RSCM_LEN  (BLE_GATT_ATT_MTU_DEFAULT - OPCODE_LENGTH - HANDLE_LENGTH)    /**< Maximum size of a transmitted Running Speed and Cadence Measurement. */

// Running Speed and Cadence Measurement flag bits
#define RSC_MEAS_FLAG_INSTANT_STRIDE_LEN_PRESENT (0x01 << 0)               /**< Instantaneous Stride Length Present flag bit. */
#define RSC_MEAS_FLAG_TOTAL_DISTANCE_PRESENT     (0x01 << 1)               /**< Total Distance Present flag bit. */
#define RSC_MEAS_FLAG_WALKING_OR_RUNNING_BIT     (0x01 << 2)               /**< Walking or Running Status flag bit. */


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_rscs      Running Speed and Cadence Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_rscs_t * p_rscs, ble_evt_t const * p_ble_evt)
{
    p_rscs->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_rscs      Running Speed and Cadence Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_rscs_t * p_rscs, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_rscs->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling the write events to the RSCS Measurement characteristic.
 *
 * @param[in]   p_rscs        Running Speed and Cadence Service structure.
 * @param[in]   p_evt_write   Write event received from the BLE stack.
 */
static void on_meas_cccd_write(ble_rscs_t * p_rscs, ble_gatts_evt_write_t const * p_evt_write)
{
    if (p_evt_write->len == 2)
    {
        // CCCD written, update notification state
        if (p_rscs->evt_handler != NULL)
        {
            ble_rscs_evt_t evt;

            if (ble_srv_is_notification_enabled(p_evt_write->data))
            {
                evt.evt_type = BLE_RSCS_EVT_NOTIFICATION_ENABLED;
            }
            else
            {
                evt.evt_type = BLE_RSCS_EVT_NOTIFICATION_DISABLED;
            }

            p_rscs->evt_handler(p_rscs, &evt);
        }
    }
}


/**@brief Function for handling the Write event.
 *
 * @param[in]   p_rscs      Running Speed and Cadence Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_rscs_t * p_rscs, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_evt_write->handle == p_rscs->meas_handles.cccd_handle)
    {
        on_meas_cccd_write(p_rscs, p_evt_write);
    }
}


void ble_rscs_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    if ((p_context == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

    ble_rscs_t * p_rscs = (ble_rscs_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_rscs, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_rscs, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_rscs, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for encoding a RSCS Measurement.
 *
 * @param[in]   p_rscs              Running Speed and Cadence Service structure.
 * @param[in]   p_rsc_measurement   Measurement to be encoded.
 * @param[out]  p_encoded_buffer    Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t rsc_measurement_encode(const ble_rscs_t      * p_rscs,
                                      const ble_rscs_meas_t * p_rsc_measurement,
                                      uint8_t               * p_encoded_buffer)
{
    uint8_t flags = 0;
    uint8_t len   = 1;

    // Instantaneous speed field
    len += uint16_encode(p_rsc_measurement->inst_speed, &p_encoded_buffer[len]);

    // Instantaneous cadence field
    p_encoded_buffer[len++] = p_rsc_measurement->inst_cadence;

    // Instantaneous stride length field
    if (p_rscs->feature & BLE_RSCS_FEATURE_INSTANT_STRIDE_LEN_BIT)
    {
        if (p_rsc_measurement->is_inst_stride_len_present)
        {
            flags |= RSC_MEAS_FLAG_INSTANT_STRIDE_LEN_PRESENT;
            len   += uint16_encode(p_rsc_measurement->inst_stride_length,
                                   &p_encoded_buffer[len]);
        }
    }

    // Total distance field
    if (p_rscs->feature & BLE_RSCS_FEATURE_TOTAL_DISTANCE_BIT)
    {
        if (p_rsc_measurement->is_total_distance_present)
        {
            flags |= RSC_MEAS_FLAG_TOTAL_DISTANCE_PRESENT;
            len   += uint32_encode(p_rsc_measurement->total_distance, &p_encoded_buffer[len]);
        }
    }

    // Flags field
    if (p_rscs->feature & BLE_RSCS_FEATURE_WALKING_OR_RUNNING_STATUS_BIT)
    {
        if (p_rsc_measurement->is_running)
        {
            flags |= RSC_MEAS_FLAG_WALKING_OR_RUNNING_BIT;
        }
    }
    p_encoded_buffer[0] = flags;

    return len;
}


uint32_t ble_rscs_init(ble_rscs_t * p_rscs, const ble_rscs_init_t * p_rscs_init)
{
    if (p_rscs == NULL || p_rscs_init == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t              err_code;
    uint8_t               init_value_encoded[MAX(MAX_RSCM_LEN, sizeof(uint16_t))];
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure
    p_rscs->evt_handler = p_rscs_init->evt_handler;
    p_rscs->conn_handle = BLE_CONN_HANDLE_INVALID;
    p_rscs->feature     = p_rscs_init->feature;

    // Add service
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_RUNNING_SPEED_AND_CADENCE);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_rscs->service_handle);

    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add measurement characteristic
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = BLE_UUID_RSC_MEASUREMENT_CHAR;
    add_char_params.max_len           = MAX_RSCM_LEN;
    add_char_params.is_var_len        = true;
    add_char_params.char_props.notify = 1;
    add_char_params.cccd_write_access = p_rscs_init->rsc_meas_cccd_wr_sec;
    add_char_params.p_init_value      = init_value_encoded;
    add_char_params.init_len          = rsc_measurement_encode(p_rscs,
                                                               &p_rscs_init->initial_rcm,
                                                               init_value_encoded);

    err_code = characteristic_add(p_rscs->service_handle, &add_char_params, &p_rscs->meas_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add feature characteristic
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid            = BLE_UUID_RSC_FEATURE_CHAR;
    add_char_params.max_len         = sizeof(uint16_t);
    add_char_params.init_len        = uint16_encode(p_rscs_init->feature, init_value_encoded);
    add_char_params.p_init_value    = init_value_encoded;
    add_char_params.char_props.read = 1;
    add_char_params.read_access     = p_rscs_init->rsc_feature_rd_sec;

    err_code = characteristic_add(p_rscs->service_handle,
                                  &add_char_params,
                                  &p_rscs->feature_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


uint32_t ble_rscs_measurement_send(ble_rscs_t * p_rscs, ble_rscs_meas_t * p_measurement)
{
    if (p_rscs == NULL || p_measurement == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t err_code;

    // Send value if connected and notifying
    if (p_rscs->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        uint8_t                encoded_rsc_meas[MAX_RSCM_LEN];
        uint16_t               len;
        uint16_t               hvx_len;
        ble_gatts_hvx_params_t hvx_params;

        len     = rsc_measurement_encode(p_rscs, p_measurement, encoded_rsc_meas);
        hvx_len = len;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_rscs->meas_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &hvx_len;
        hvx_params.p_data = encoded_rsc_meas;

        err_code = sd_ble_gatts_hvx(p_rscs->conn_handle, &hvx_params);
        if ((err_code == NRF_SUCCESS) && (hvx_len != len))
        {
            err_code = NRF_ERROR_DATA_SIZE;
        }
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }

    return err_code;
}
#endif // NRF_MODULE_ENABLED(BLE_RSCS)
