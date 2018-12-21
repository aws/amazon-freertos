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
 * @file timer.h
 *
 *  Timer Control Function
 *
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include "hal_gpt.h"

#ifdef HAL_GPT_MODULE_ENABLED
#include "stdint.h"
#include "stdbool.h"


#ifdef __cplusplus
extern "C" {
#endif

#define TMR0            0
#define TMR1            1
#define GPT2            2
#define GPT4            4

#define OneShot_mode    0
#define AutoRepeat_mode 1



typedef struct {
    void (*TMR0_func)(void);
    void (*TMR1_func)(void);
} TMR_func;


typedef struct {
    TMR_func       TMR_FUNC;
} TMRStruct;


/**
 * @brief Get currenct system time
 *
 * @param None
 *
 * @return current GPT2 time in unit of 32k clock
 */
uint32_t get_current_count(void);
/**
 * Get currenct system time
 *
 * @param None
 *
 * @return current GPT4 time in unit of bus clock
 */
uint32_t get_current_gpt4_count(void);

/**
 * @brief System delay time 1ms function
 *
 * @param ms [IN] Delay time 1ms, unit is 32k clock unit.
 *
 * @return None
 */
void delay_ms(uint32_t ms);

/**
 * @brief System delay time 1us function
 *
 * @param ms [IN] Delay time 1us, clock source is bus clock.
 *
 * @return None
 */
void delay_us(uint32_t us);

/**
 * @brief TMR0/1 init function
 *
 * @param timerNum [IN]  TMR0 or TMR1
 * @param countValue [IN] Initial count value
 * @param autoRepeat  [IN] OneShot_mode or AutoRepeat_mode
 * @param TMR_Callback [IN] type void (*TMR_Callback)(void)
 *
 * @return None
 */
void drvTMR_init(uint32_t timerNum, uint32_t countValue, bool  autoRepeat, void (*TMR_Callback)(void));
/**
 * @brief Timer0/1 and GPT2/4start function
 *
 * @param timerNum [IN] TMR0 or TMR1
 *
 * @return None
 */
void TMR_Start(uint32_t timerNum);
/**
 * @brief Timer0/1 and GPT2/4 stop function
 *
 * @param timerNum [IN] TMR0 or TMR1
 *
 * @return None
 */
void TMR_Stop(uint32_t timerNum);
/**
 * @brief GPT2 init function
 *
 * @return None
 */
void drvGPT2Init(void);
/**
* @brief GPT4 init function
*
* @return None
*/
void drvGPT4Init(void);
/**
 * @brief Clean TMR0/1 Interrupt status bit
 *
 * @param timerNum [IN] TMR0 or TMR1
 *
 * @return None
 */
void clear_TMR_INT_status_bit(uint32_t timerNum);


#ifdef __cplusplus
}
#endif

#endif

#endif
