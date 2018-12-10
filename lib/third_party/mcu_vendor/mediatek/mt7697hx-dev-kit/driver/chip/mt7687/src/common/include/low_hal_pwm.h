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

/*
** $Log: hal_PWM.h $
**
** 04 23 2015 morris.yeh
** [PWM]
** refine code for wake up test command
**
** 04 20 2015 morris.yeh
** [PWM]
** Add test command for wake up test
**
** 11 14 2014 leo.hung
** [DVT][PWM]
** 1. Update PWM.
**
** 11 10 2014 leo.hung
** [DVT][I2C]
** 1. Use CPU_FREQUENCY instead of hard code.
**
** 10 15 2014 pierre.chang
** Add DMA, modify makefile to add PRODUCT_VERSION....
**
**
*/

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

#ifndef _HAL_PWM_H
#define _HAL_PWM_H

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

#include "hal_platform.h"
#ifdef HAL_PWM_MODULE_ENABLED
#include "type_def.h"
#include "mt7687_cm4_hw_memmap.h"



/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
#define PWM_STAY_CYCLE_MAX  (0xFFF) // 12 bits


/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/


/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/


/*
========================================================================
Routine Description:

Note:
========================================================================
*/
ENUM_HAL_RET_T halPWMResetAll(void);
ENUM_HAL_RET_T halPWMClockSelect(ENUM_PWM_CLK_T ePwmClk);
ENUM_HAL_RET_T halPWMEnable(UINT8 ucPwmIdx, UINT8 ucGlobalKick, UINT8 ucIOCtrl, UINT8 ucPolarity);
ENUM_HAL_RET_T halPWMDisable(UINT8 ucPwmIdx);
ENUM_HAL_RET_T halPWMKick(UINT8 ucPwmIdx);
ENUM_HAL_RET_T halPWMConfig(UINT8 ucPwmIdx, ENUM_PWM_STATE_T eState, UINT16 u2DutyCycle, UINT32 u4PwmFreq);
ENUM_HAL_RET_T halPWMStateConfig(UINT8 ucPwmIdx, UINT16 u2S0StayCycle, UINT16 u2S1StayCycle, UINT8 ucReplayMode);
ENUM_HAL_RET_T halPWMQuery(UINT8 ucPwmIdx, ENUM_PWM_STATE_T eState, UINT16 *pu2DutyCycle, UINT32 *pu4PwmFreq, UINT8 *pucEnable);
#endif  //#ifdef HAL_PWM_MODULE_ENABLED
#endif //_HAL_PWM_H

