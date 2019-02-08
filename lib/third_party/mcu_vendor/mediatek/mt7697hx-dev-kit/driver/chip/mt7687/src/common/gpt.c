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

/*****************************************************************************
 *
 * Filename:
 * ---------
 *    gpt.c
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   This Module defines the GPT driver.
 *
 * Author:
 * -------
 *  James Liu
 *
 ****************************************************************************/
#include "hal_gpt.h"

#ifdef HAL_GPT_MODULE_ENABLED
#include "type_def.h"
#include "_mtk_hal_debug.h"

#include "gpt.h"
#include "nvic.h"
#include "system_mt7687.h"
#include "timer.h"
#include "hal_nvic.h"


static GPTStruct GPTTimer;


//workaround: ADC use sys tick to access FIFO instead of GPT
volatile UINT32 g_u4ClkCnt1ms = 0;

void GPT_ResetTimer(kal_uint32 timerNum, kal_uint32 countValue, bool autoRepeat)
{
    ASSERT(timerNum == 0 || timerNum == 1);

    if (timerNum == 0) {
        DRV_Reg32(GPT0_ICNT) = countValue;
        if (KAL_TRUE == autoRepeat) {
            DRV_Reg32(GPT0_CTRL) |= GPT_CTRL_AUTOMODE;
        } else {
            DRV_Reg32(GPT0_CTRL) &= ~GPT_CTRL_AUTOMODE;
        }
    } else if (timerNum == 1) {
        DRV_Reg32(GPT1_ICNT) = countValue;
        if (KAL_TRUE == autoRepeat) {
            DRV_Reg32(GPT1_CTRL) |= GPT_CTRL_AUTOMODE;
        } else {
            DRV_Reg32(GPT1_CTRL) &= ~GPT_CTRL_AUTOMODE;
        }
    }
}

void GPT_Start(kal_uint32 timerNum)
{
    ASSERT(timerNum == 0 || timerNum == 1 || timerNum == 4);

    if (timerNum == 0) {
        DRV_Reg32(GPT0_CTRL) |= GPT_CTRL_EN;
        NVIC_EnableIRQ((IRQn_Type)CM4_GPT_IRQ);
    } else if (timerNum == 1) {
        DRV_Reg32(GPT1_CTRL) |= GPT_CTRL_EN;
        NVIC_EnableIRQ((IRQn_Type)CM4_GPT_IRQ);
    } else if (timerNum == 4) {
        DRV_Reg32(GPT4_CTRL) |= GPT_CTRL_EN;
    }

}

void GPT_Stop(kal_uint32 timerNum)
{
    if (timerNum == 0) {
        DRV_Reg32(GPT_IER)   &= ~GPT0_INT_EN;
        DRV_Reg32(GPT0_CTRL) &= ~GPT_CTRL_EN;
        DRV_Reg32(GPT_ISR)    = GPT0_INT;

    } else if (timerNum == 1) {
        DRV_Reg32(GPT_IER)   &= ~GPT1_INT_EN;
        DRV_Reg32(GPT1_CTRL) &= ~GPT_CTRL_EN;
        DRV_Reg32(GPT_ISR)    = GPT1_INT;
    } else if (timerNum == 2) {
        DRV_Reg32(GPT2_CTRL) &= ~GPT_CTRL_EN;
    } else if (timerNum == 4) {
        DRV_Reg32(GPT4_CTRL) &= ~GPT_CTRL_EN;
    }


}

void GPT_INT_Handler(hal_nvic_irq_t irq_number)
{
    kal_uint16 GPT_Status;

    irq_number = irq_number;
    GPT_Status = DRV_Reg32(GPT_ISR);
    DRV_Reg32(GPT_ISR) = GPT_Status;

    if (GPT_Status & GPT0_INT) {
        clear_TMR_INT_status_bit(TMR0); //clear gpt Int status bit
        GPTTimer.GPT_FUNC.gpt0_func();
    } else if (GPT_Status & GPT1_INT) {
        clear_TMR_INT_status_bit(TMR1); //clear gpt Int status bit
        GPTTimer.GPT_FUNC.gpt1_func();
    }
}

void GPT_init(kal_uint32 timerNum, kal_uint32 speed_32us, void (*GPT_Callback)(void))
{
    ASSERT(timerNum == 0 || timerNum == 1 || timerNum == 2 || timerNum == 4);

    if (timerNum == 0 || timerNum == 1) {
        hal_nvic_register_isr_handler((IRQn_Type)CM4_GPT_IRQ, GPT_INT_Handler); /* GPT0 and GPT1 share the same IRQ line */
        NVIC_SetPriority((IRQn_Type)CM4_GPT_IRQ, CM4_GPT_PRI);
        NVIC_EnableIRQ((IRQn_Type)CM4_GPT_IRQ);
    }

    if (timerNum == 0) {
        GPTTimer.GPT_FUNC.gpt0_func = GPT_Callback;
        DRV_Reg32(GPT_IER)   |= GPT0_INT_EN;
        DRV_Reg32(GPT0_CTRL) |= (speed_32us << 2);
    } else if (timerNum == 1) {
        GPTTimer.GPT_FUNC.gpt1_func = GPT_Callback;
        DRV_Reg32(GPT_IER)   |= GPT1_INT_EN;
        DRV_Reg32(GPT1_CTRL) |= (speed_32us << 2);
    } else if (timerNum == 2) {
        DRV_Reg32(GPT2_CTRL) |= GPT_CTRL_EN | (speed_32us << 1);
    } else if (timerNum == 4) {
        DRV_Reg32(GPT4_CTRL) |= GPT_CTRL_EN | (speed_32us << 1);  //bus clock or half bus clock
        DRV_Reg32(GPT4_INIT) = 0;
    }
}

kal_uint32 GPT_return_current_count(kal_uint32 timerNum)
{
    kal_uint32 current_count;

    ASSERT(timerNum == 0 || timerNum == 1 || timerNum == 2 || timerNum == 4);

    if (timerNum == 0) {
        current_count = DRV_Reg32(GPT0_CNT);
    } else if (timerNum == 1) {
        current_count = DRV_Reg32(GPT1_CNT);
    } else if (timerNum == 2) {
        current_count = DRV_Reg32(GPT2_CNT);
    } else if (timerNum == 4) {
        current_count = DRV_Reg32(GPT4_CNT);
    }

    return current_count;
}


void CM4_GPT2Init(void)
{
    GPT_init(2, 1, NULL);   //speed: 1x32k hz
}

void CM4_GPT4Init(void)
{
    GPT_init(4, 1, NULL);  //speed: bus clock
}

#endif

