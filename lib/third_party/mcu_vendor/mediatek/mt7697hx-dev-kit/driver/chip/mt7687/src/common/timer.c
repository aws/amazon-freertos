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

#include "hal_gpt.h"
#include "hal_nvic.h"

#ifdef HAL_GPT_MODULE_ENABLED

#include "type_def.h"
#include "gpt.h"
#include "timer.h"

#define SPEED_1K        0
#define SPEED_32K       1

#define time_after(a,b)  ((long)(b) - (long)(a) < 0)
#define time_before(a,b) time_after(b,a)

#ifdef HAL_DFS_MODULE_ENABLED
extern uint32_t dvfs_irq_status;
extern bool us_delay_in_dvfs;
#endif


/**
 * Get currenct system time
 *
 * @param None
 *
 * @return current GPT2 time in unit of 32k clock
 */
uint32_t get_current_count(void)
{
    return GPT_return_current_count(GPT2);
}


void delay_time(kal_uint32 count)
{
    kal_uint32 end_count, current;

    end_count = get_current_count() + count + 1;
    current = get_current_count();
    while (time_before(current, end_count)) {
        current = get_current_count();
    }

}


void delay_time_for_gpt4(kal_uint32 count)
{
    kal_uint32 end_count, current;

    end_count = get_current_gpt4_count() + count + 1;
    current = get_current_gpt4_count();
    while (time_before(current, end_count)) {
        current = get_current_gpt4_count();
    }

}


/**
 * Get currenct system time
 *
 * @param None
 *
 * @return current GPT4 time in unit of bus clock
 */
uint32_t get_current_gpt4_count(void)
{
    return (GPT_return_current_count(GPT4));
}
/**
 * System delay time function
 *
 * @param count     delay time unit is 32k clock unit
 *
 * @return None
 */
void delay_ms(uint32_t ms)
{
    uint32_t count;
#ifdef HAL_DFS_MODULE_ENABLED
    uint32_t temp_count;
#endif

    count  = 32 * ms;
    count += (7 * ms) / 10;
    count += (6 * ms) / 100;
    count += (8 * ms) / 1000;

#ifdef HAL_DFS_MODULE_ENABLED
    if (us_delay_in_dvfs) {
        us_delay_in_dvfs = false;
        hal_nvic_restore_interrupt_mask(dvfs_irq_status);
        temp_count = (count + 500) / 1000;
        if (temp_count) {
            temp_count--;
        }
        count = temp_count;
    }
#endif
    delay_time(count);
}


void delay_us(uint32_t us)
{
    uint32_t count;
    uint32_t ticks_per_us;

    ticks_per_us = top_mcu_freq_get() / 1000000;
    count = ticks_per_us * us;

    delay_time_for_gpt4(count);
}



void drvTMR_init(uint32_t timerNum, uint32_t countValue, bool  autoRepeat, void (*TMR_Callback)(void))
{

    GPT_Stop(timerNum);

    if (timerNum == TMR0) {
        DRV_Reg32(GPT0_CTRL) = 0;
        GPT_init(timerNum, SPEED_32K, TMR_Callback);
    } else {
        DRV_Reg32(GPT1_CTRL) = 0;
        GPT_init(timerNum, SPEED_32K, TMR_Callback);
    }

    GPT_ResetTimer(timerNum, countValue, autoRepeat);
}

void TMR_Start(uint32_t timerNum)
{
    GPT_Start(timerNum);
}

void TMR_Stop(uint32_t timerNum)
{
    GPT_Stop(timerNum);
}


void drvGPT2Init(void)
{
    CM4_GPT2Init();
}

void drvGPT4Init(void)
{
    CM4_GPT4Init();
}

void clear_TMR_INT_status_bit(uint32_t timerNum)
{
    if (timerNum == TMR0) {
        DRV_Reg32(GPT_ISR) = GPT0_INT;
    }
    if (timerNum == TMR1) {
        DRV_Reg32(GPT_ISR) = GPT1_INT;
    }
}

#endif


