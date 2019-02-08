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

/**
 * @file ir_tx.h
 *
 * IR Tx supports NEC, RC5, RC6 protocol.
 * And software-pulse-width mode is provided to support user defined waveform which is indicated by high/low width in time unit.
 * Interrupt is raised to indicate Tx done.
 *
 */


#ifndef __IR_TX_H__
#define __IR_TX_H__



#include "hal_platform.h"
#ifdef HAL_IRTX_MODULE_ENABLED
#include "stdint.h"



#ifdef __cplusplus
extern "C" {
#endif

#define IR_TX_NEC               (0)
#define IR_TX_RC5               (1)
#define IR_TX_RC6               (2)
#define IR_TX_PROTOCOL_NUM      (3)

/**
 * @brief  Configure IR Tx as specific protocol.
 *
 * @param  mode [IN] 0:NEC, 1:RC5, 2:RC6 are supported.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note   Related pinmux setting should be configured before using IrTx.
 */
int32_t ir_tx_configure(uint8_t mode);


/**
 * @brief  Send data bits in configured IR protocol (RC5, RC6, NEC).
 *
 * @param  data0 [IN] Data bits[31:0].
 * @param  data1 [IN] Data bits[63:32].
 * @param  data2 [IN] Data bits[95: 64].
 * @param  bit_length [IN] Total length of valid Tx bits.
 * @param  repeat_code [IN] Indicate repeat code of NEC. Only valid for NEC.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note   Function ir_tx_send is used for RC5, RC6 and NEC. Function ir_tx_pulse_width_send is used for software-pulse-width mode.
 *
 * @note   data0 R07 R06 R05 R04 R03 R02 R01 R00[b7, b6, b5, b4, b3, b2, b1, b0]
 *          data1 R15 R14 R13 R12 R11 R10 R09 R08
 *          data2 R23 R22 R21 R20 R19 R18 R17 R16
 *          Tx bit order is R00b0, R00b1 ... R00b7, R01b0, R01b1 ... R23b7
 * @note   RC5 Start Bits (S1, S2) and Toggle Bit need to be included in data and bit_length.
 * @note   RC6 Leader and Start bit are not included in data and bit_length.
 * @note   NEC Leader are not included in data and bit_length
 */
int32_t ir_tx_send(uint32_t data0, uint32_t data1, uint32_t data2, uint8_t bit_length, uint8_t repeat_code);


/**
 * @brief  Configure IR Tx software pulse width mode with specific carrier frequency and duty cycle.
 *
 * @param  frequency [IN] Carrier frequency in unit of KHz, ex. NEC is 38KHz.
 * @param  duty_cycle [IN] Carrier duty cycle in unit of percentage, ex. NEC suppose to be 25% or 33%.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note Related pinmux setting should be configured before using IrTx.
 */
int32_t ir_tx_pulse_width_configure(uint16_t frequency, uint8_t duty_cycle);

/**
 * @brief  Send waveform indicated by high/low width in software-pulse-width mode.
 *
 * @param  data_length [IN] Total length of valid data, maximum is 68.
 * @param  data [IN] Interleaved high/low width in unit of base_period_us.
 * @param  base_period_us [IN] Unit of value indicated by parameter data, ex. 35, in unit of us.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note Related pinmux setting should be configured before using IrTx.
 */
int32_t ir_tx_pulse_width_send(uint8_t data_length, uint8_t *data, uint8_t base_period_us);

/**
 * @brief  Clear IR Tx interrupt status.
 *
 * @return >=0 means success, <0 means fail.
 */
int32_t ir_tx_interrupt_clear(void);


#ifdef __cplusplus
}
#endif

#endif

#endif /* __IR_TX_H__ */

