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
 * @file pwm.h
 *
 *  Pulse Width Modulation.
 *
 */


#ifndef __PWM_H__
#define __PWM_H__



#include "hal_platform.h"
#ifdef HAL_PWM_MODULE_ENABLED
#include "pinmux.h"


#ifdef __cplusplus
extern "C" {
#endif

#define PWM_CLK_SRC_32K     (0)
#define PWM_CLK_SRC_2M      (1)
#define PWM_CLK_SRC_XTAL    (2)
#define PWM_CLK_SRC_NUM     (3)


/**
 * @brief  Configure clock source of all PWM module.
 *
 * @param  clock_source [IN] 0:32KHz, 1:2MHz, 2:XTAL(depending on XTAL, ex. 40MHz) are supported.
 *
 * @return >=0 means success, <0 means fail.
 *
 * @note Clock source should be selected based on PWM frequency.
 */
int32_t pwm_init(uint8_t clock_source);


/**
 * @brief  Configure specific PWM with indicated frequency and duty cycle.
 *
 * @param  index [IN] Indicate specific PWM module.
 * @param  frequency [IN] In unit of Hz.
 * @param  duty_cycle [IN] In 256 steps.
 * @param  enable [IN]
 *          1: PWM is enabled and will be active after pwm_set done.
 *          0: PWM is disabled, frequency and duty cycle setting are cleared as 0.
 *
 * @return >=0 means success, <0 means fail.
 *
 */
int32_t pwm_set(uint8_t index, uint32_t frequency, uint16_t duty_cycle, uint8_t enable);


/**
 * @brief  Get frequency, duty cycle and on/off status of specific PWM.
 *
 * @param  index [IN] Indicate specific PWM module.
 * @param  frequency [IN] In unit of Hz.
 * @param  duty_cycle [IN] In 256 steps.
 * @param  enable [IN]
 *          1: PWM is enabled.
 *          0: PWM is disabled.
 *
 * @return >=0 means success, <0 means fail.
 *
 */
int32_t pwm_get(uint8_t index, uint32_t *frequency, uint16_t *duty_cycle, uint8_t *status);


/** @brief Get PWM module index from GPIO number.
 *
 *  @param  index [IN] Indicate specific GOIP number.
 *  @param  frequency [IN] In unit of Hz.
 *
 *  @return >=0 means success and PWM module index, <0 means fail.
 *
 */



#ifdef __cplusplus
}
#endif

#endif
#endif /* __PWM_H__ */

