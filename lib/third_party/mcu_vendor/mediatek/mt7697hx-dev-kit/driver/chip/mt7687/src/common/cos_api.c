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

#include "type_def.h"
#include "gpt.h"
#include "cos_api.h"
#include "hal_lp.h"
#include "mt7687_cm4_hw_memmap.h"


extern void delay_time(kal_uint32 count);

/**
 * Get currenct system time
 *
 * @param None
 *
 * @return current system time in unit of 32k clock tick
 */
kal_uint32 cos_get_systime(void)
{
    return GPT_return_current_count(2);
}


/**
 * System delay time function
 *
 * @param count     delay time unit is 32k clock unit
 *
 * @return None
 */
void cos_delay_time(kal_uint32 count)
{
    delay_time(count);
}

void cos_delay_ms(kal_uint32 ms)
{
    cos_delay_time(32 * ms);
}

void cos_delay_100us(kal_uint32 unit)
{
    //cos_delay_time(3*unit);
    cos_delay_time(4 * unit);
}


unsigned long randomseed;
unsigned long SYSrand_Get_Rand(void) /* reentrant */
{
    unsigned long result;
    unsigned long next_seed = randomseed;

    next_seed = (next_seed * 1103515245) + 12345; /* permutate seed */
    result = next_seed & 0xfffc0000; /* use only top 14 bits */

    next_seed = (next_seed * 1103515245) + 12345; /* permutate seed */
    result = result + ((next_seed & 0xffe00000) >> 14); /* top 11 bits */

    next_seed = (next_seed * 1103515245) + 12345; /* permutate seed */
    result = result + ((next_seed & 0xfe000000) >> (25)); /* use only top 7 bits */

    randomseed = next_seed;

    return (result & 0xffffffff);
}

void cos_control_force_pwm_mode(cos_module_on_off_state module)
{
    // Raise flag to claim using the PMU
    if (module == COS_MODULE_ON_OFF_STATE_CM4_ADC) {
        mSetHWEntry(TOP_CFG_AON_N9_CM4_MESSAGE_CM4_ADC, 1);
    } else if (module == COS_MODULE_ON_OFF_STATE_CM4_AUDIO) {
        mSetHWEntry(TOP_CFG_AON_N9_CM4_MESSAGE_CM4_AUDIO, 1);
    } else {
        return;
    }
    // delay to prevent from racing
    cos_delay_time(1);

    // get own bit from firmware
    //if (hal_lp_connsys_get_own_enable_int() != 0)
    //{
    //    return;
    //}

    // Force PWM mode
    // Set 0x8102140C bit[22] and bit[14] = 11
    mSetHWEntry(TOP_CFG_AON_PMU_RG_BUCK_FORCE_PWM, 0x101);

    // set firmware own if necessary
    //if (hal_lp_connsys_give_n9_own() != 0)
    //{
    //    return;
    //}
}

void cos_control_force_pwm_mode_exit(cos_module_on_off_state module)
{
    uint32_t volatile_reg1 = 0, volatile_reg2 = 0;

    // Clear flag to disclaim using the PMU
    if (module == COS_MODULE_ON_OFF_STATE_CM4_ADC) {
        mSetHWEntry(TOP_CFG_AON_N9_CM4_MESSAGE_CM4_ADC, 0);
    } else if (module == COS_MODULE_ON_OFF_STATE_CM4_AUDIO) {
        mSetHWEntry(TOP_CFG_AON_N9_CM4_MESSAGE_CM4_AUDIO, 0);
    } else {
        return;
    }
    // Check all modules
    volatile_reg1 = mGetHWEntry(TOP_CFG_AON_N9_MESSAGE);
    volatile_reg2 = mGetHWEntry(TOP_CFG_AON_N9_CM4_MESSAGE);
    if ((volatile_reg1 & volatile_reg2) != 0) {
        return;
    }
    // get own bit from firmware
    if (hal_lp_connsys_get_own_enable_int() != 0) {
        return;
    }

    // Exit force PWM mode
    // Set 0x8102140C bit[22] and bit[14] = 00
    mSetHWEntry(TOP_CFG_AON_PMU_RG_BUCK_FORCE_PWM, 0x000);

    // set firmware own if necessary
    //if (hal_lp_connsys_give_n9_own() != 0)
    //{
    //    return;
    //}
}

