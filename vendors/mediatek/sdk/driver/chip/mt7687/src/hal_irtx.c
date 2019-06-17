/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "hal_irtx.h"

#ifdef HAL_IRTX_MODULE_ENABLED
#include <stdlib.h>
#include "mt7687.h"
#include "ir_tx.h"
#include "hal_log.h"
#include "low_hal_irtx.h"
#include "hal_nvic.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_driver.h"


#ifdef HAL_SLEEP_MANAGER_ENABLED
static uint8_t irtx_sleep_handler = 0;
#endif


static hal_irtx_pulse_data_callback_t g_pulse_data_callback = NULL;
void *g_callback_user_data = NULL;
static bool irtx_status = false;
static uint8_t hal_irtx_revert_bit_order(uint8_t half_byte)
{
    uint8_t target = 0;
    target |= (half_byte & 0x01) << 3;
    target |= (half_byte & 0x02) << 1;
    target |= (half_byte & 0x04) >> 1;
    target |= (half_byte & 0x08) >> 3;
    return target;
}

static uint32_t hal_irtx_build_double_word(const uint8_t *data)
{
    uint32_t target_data = 0;
    target_data |= hal_irtx_revert_bit_order((data[0] >> 4) & 0x0F) << 0;
    target_data |= hal_irtx_revert_bit_order(data[0] & 0x0F) << 4;
    target_data |= hal_irtx_revert_bit_order((data[1] >> 4) & 0x0F) << 8;
    target_data |= hal_irtx_revert_bit_order(data[1] & 0x0F) << 12;
    target_data |= hal_irtx_revert_bit_order((data[2] >> 4) & 0x0F) << 16;
    target_data |= hal_irtx_revert_bit_order(data[2] & 0x0F) << 20;
    target_data |= hal_irtx_revert_bit_order((data[3] >> 4) & 0x0F) << 24;
    target_data |= hal_irtx_revert_bit_order(data[3] & 0x0F) << 28;
    return target_data;
}

static void hal_irtx_translate_user_data(uint32_t target_data[3], const uint8_t data[HAL_IRTX_MAX_DATA_LENGTH])
{
    target_data[0] = hal_irtx_build_double_word(data);
    target_data[1] = hal_irtx_build_double_word(data + 4);
    target_data[2] = hal_irtx_build_double_word(data + 8);
}

static uint8_t hal_irtx_translate_repeat_code(hal_irtx_repeat_code_t repeat_code)
{
    return HAL_IRTX_REPEAT_ENABLE == repeat_code ? 1 : 0;
}

static  hal_irtx_status_t hal_irtx_check_valid_data(const uint8_t *data, uint32_t bit_length)
{
    if (NULL == data) {
        log_hal_error("data is null");
        return HAL_IRTX_STATUS_ERROR;
    }
    if (bit_length > HAL_IRTX_MAX_DATA_LENGTH * 8) {
        log_hal_error("bit_length %lu, too many bits to send, limit is %u",
                      bit_length, HAL_IRTX_MAX_DATA_LENGTH * 8);
        return HAL_IRTX_STATUS_ERROR;
    }

    return HAL_IRTX_STATUS_OK;
}


hal_irtx_status_t hal_irtx_init(void)
{

    halIrTxInit(1, 0, 1);

    /*get sleep handler*/
#ifdef HAL_SLEEP_MANAGER_ENABLED
    irtx_sleep_handler = hal_sleep_manager_set_sleep_handle("irtx");
    if (irtx_sleep_handler == INVALID_SLEEP_HANDLE) {
        log_hal_error("there's no available handle when IRTX get sleep handle");
        return HAL_IRTX_STATUS_ERROR;
    }
#endif


    return HAL_IRTX_STATUS_OK;

}

hal_irtx_status_t hal_irtx_deinit(void)
{
    halIrTxInit(0, 0, 0);
    /*release sleep hander*/
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_release_sleep_handle(irtx_sleep_handler);
#endif
    return HAL_IRTX_STATUS_OK;

}

hal_irtx_status_t hal_irtx_do_send_data(uint8_t mode,
                                        hal_irtx_repeat_code_t repeat_code,
                                        const uint8_t data[HAL_IRTX_MAX_DATA_LENGTH],
                                        uint32_t bit_length)
{
    int32_t ret_val = ir_tx_configure(mode);
    if (ret_val < 0) {
        log_hal_error("ir_tx_configure fail");
        return HAL_IRTX_STATUS_ERROR;
    }
    uint32_t target_data[3] = {0};
    hal_irtx_translate_user_data(target_data, data);
    /*lock sleep mode*/
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_lock_sleep(irtx_sleep_handler);
#endif
    ret_val = ir_tx_send(target_data[0],
                         target_data[1],
                         target_data[2],
                         bit_length,
                         hal_irtx_translate_repeat_code(repeat_code));
    /*unlock sleep mode*/
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_unlock_sleep(irtx_sleep_handler);
#endif

    if (ret_val < 0) {
        log_hal_error("ir_tx_configure fail");
        return HAL_IRTX_STATUS_ERROR;
    }
    irtx_status = true;
    return HAL_IRTX_STATUS_OK;
}

hal_irtx_status_t hal_irtx_send_nec_data(hal_irtx_repeat_code_t repeat_code,
        const uint8_t data[HAL_IRTX_MAX_DATA_LENGTH],
        uint32_t bit_length)
{

    if (HAL_IRTX_STATUS_ERROR == hal_irtx_check_valid_data(data, bit_length)) {
        return HAL_IRTX_STATUS_ERROR;
    }
    return hal_irtx_do_send_data(IR_TX_NEC, repeat_code, data, bit_length);
}

hal_irtx_status_t hal_irtx_send_rc5_data(hal_irtx_repeat_code_t repeat_code,
        const uint8_t data[HAL_IRTX_MAX_DATA_LENGTH],
        uint32_t bit_length)
{
    if (HAL_IRTX_STATUS_ERROR == hal_irtx_check_valid_data(data, bit_length)) {
        return HAL_IRTX_STATUS_ERROR;
    }

    return hal_irtx_do_send_data(IR_TX_RC5, repeat_code, data, bit_length);
}


hal_irtx_status_t hal_irtx_send_rc6_data(hal_irtx_repeat_code_t repeat_code,
        const uint8_t data[HAL_IRTX_MAX_DATA_LENGTH],
        uint32_t bit_length)
{
    if (HAL_IRTX_STATUS_ERROR == hal_irtx_check_valid_data(data, bit_length)) {
        return HAL_IRTX_STATUS_ERROR;
    }

    return hal_irtx_do_send_data(IR_TX_RC6, repeat_code, data, bit_length);
}

hal_irtx_status_t hal_irtx_configure_pulse_data_carrier(uint32_t frequency,
        uint32_t duty_cycle)
{
    int32_t ret_val = ir_tx_pulse_width_configure((uint16_t)frequency, (uint8_t)duty_cycle);
    return ret_val >= 0 ? HAL_IRTX_STATUS_OK : HAL_IRTX_STATUS_ERROR;
}

static void hal_irtx_interrupt_handle(hal_nvic_irq_t irq_number)
{
    if (NULL != g_pulse_data_callback) {
        g_pulse_data_callback(HAL_IRTX_EVENT_TRANSACTION_SUCCESS, g_callback_user_data);
    }
    (void)ir_tx_interrupt_clear();
    irtx_status = false;
}

hal_irtx_status_t hal_irtx_register_pulse_data_callback(hal_irtx_pulse_data_callback_t callback,
        void *user_data)
{
    g_pulse_data_callback = callback;
    g_callback_user_data = user_data;
    hal_nvic_register_isr_handler(CM4_IRDA_TX_IRQ, hal_irtx_interrupt_handle);
    NVIC_EnableIRQ((IRQn_Type)CM4_IRDA_TX_IRQ);
    return HAL_IRTX_STATUS_OK;
}

hal_irtx_status_t hal_irtx_send_pulse_data(uint32_t base_period,
        uint8_t *data,
        uint32_t length)
{
    int32_t ret_val = 0;
    /*lock sleep mode*/
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_lock_sleep(irtx_sleep_handler);
#endif

    ret_val = ir_tx_pulse_width_send(length, data, base_period);
    /*unlock sleep mode*/
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_unlock_sleep(irtx_sleep_handler);
#endif
    return ret_val >= 0 ? HAL_IRTX_STATUS_OK : HAL_IRTX_STATUS_ERROR;
}


hal_irtx_status_t hal_irtx_get_running_status(hal_irtx_running_status_t *running_status)
{
    if (irtx_status) {
        *running_status = HAL_IRTX_BUSY;
    } else {
        *running_status = HAL_IRTX_IDLE;
    }

    return HAL_IRTX_STATUS_OK;
}

#endif
