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

/****************************************************************************
    Module Name:
    PWM

    Abstract:
    Pulse Width Modulation.

    Revision History:
    Who         When            What
    --------    ----------      ------------------------------------------
***************************************************************************/



#include "hal_platform.h"

#ifdef HAL_PWM_MODULE_ENABLED
#include <string.h>
#include "type_def.h"
#include "mt7687.h"
#include "pwm.h"
#include "low_hal_pwm.h"
#include "low_hal_gpio.h"



int32_t pwm_init(uint8_t clock_source)
{
    INT32 ret = 0;

    if (PWM_CLK_SRC_NUM <= clock_source) {
        return -1;
    }

    halPWMResetAll();
    if (halPWMClockSelect((ENUM_PWM_CLK_T)clock_source)) {
        return -1;
    }

    return ret;
}

int32_t pwm_set(uint8_t index, uint32_t frequency, uint16_t duty_cycle, uint8_t enable)
{
    INT32 ret = 0;
    ENUM_PWM_STATE_T eState = PWM_S0;
    UINT16 u2DutyStep = 0;
    UINT16 u2S0StayCycle = 1;
    UINT16 u2S1StayCycle = 0;
    UINT8 ucReplayMode = 0;

    if (0 == enable) {
        // Set duty_cycle to make sure PWM to be idle.
        if (halPWMConfig(index, eState, 0, frequency)) {
            return -1;
        }

        if (halPWMKick(index)) {
            return -1;
        }

        // Disable PWM clock
        if (halPWMDisable(index)) {
            return -1;
        }
    } else {
        u2DutyStep = duty_cycle;

        if (halPWMEnable(index, 0, 0, 0)) {
            return -1;
        }

        if (halPWMConfig(index, eState, u2DutyStep, frequency)) {
            return -1;
        }

        if (halPWMStateConfig(index, u2S0StayCycle, u2S1StayCycle, ucReplayMode)) {
            return -1;
        }

        if (halPWMKick(index)) {
            return -1;
        }
    }

    return ret;
}

int32_t pwm_get(uint8_t index, uint32_t *frequency, uint16_t *duty_cycle, uint8_t *status)
{
    INT32               ret = 0;
    ENUM_PWM_STATE_T    eState = PWM_S0;

    halPWMQuery(index, eState, duty_cycle, (UINT32 *)frequency, (UINT8 *)status);

    return ret;
}


#endif

