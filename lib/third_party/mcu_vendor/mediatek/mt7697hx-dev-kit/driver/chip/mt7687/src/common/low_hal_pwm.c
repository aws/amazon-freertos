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
** $Log: hal_PWM.c $
**
** 04 23 2015 morris.yeh
** [PWM]
** refine code for wake up test command
**
** 04 20 2015 morris.yeh
** [PWM]
** Add test command for wake up test
**
** 04 02 2015 leo.hung
** [DVT][PWM]
** 1. Remove VOID halPWMPinmuxConfig().
** 2. Set XTAL related CR for PWM.
** 3. Update PWM test case.
**
** 02 06 2015 leo.hung
** [DVT][PWM]
** 1. Refine code.
**
** 02 06 2015 morris.yeh
** [PWM]
** modify hal PWM AON pinmux config
**
** 01 15 2015 morris.yeh
** [PWM]
** refine AON domain pinmux config
**
** 12 25 2014 leo.hung
** [DVT][PWM]
** 1. Relocate PWM base address to AON domain.
**
** 12 11 2014 leo.hung
** [DVT][PWM].
** 1. Fix global reset.
** 2. Add test case for global kick.
**
** 12 08 2014 leo.hung
** [DVT][PWM][IRDA][RTC][Crypto]
** 1. Update PWM, IRDA, RTC Crypto_AES/DES.
**
** 11 26 2014 morris.yeh
** [PWM]
** Add detail explanation of PWM formula
**
** 11 14 2014 leo.hung
** [DVT][PWM]
** 1. Update PWM.
**
** 11 11 2014 leo.hung
** [DVT][I2C][PWM]
** 1. Update I2C DMA mode.
** 2. Replace PWM field access with bit offset access.
**
**
*/

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/


#include "hal_platform.h"
#ifdef HAL_PWM_MODULE_ENABLED

#include <string.h>
#include "low_hal_pwm.h"
#include "top.h"


/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/


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


ENUM_HAL_RET_T halPWMResetAll()
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;

    // Write 1 and then write 0 to reset all PWM modules and its parameters (PWM_CTRL/PWM_PARAM_S0/PWM_PARAM_S1).
    HAL_REG_32(IOT_PWM_GLO_CTRL_ADDR) |= BIT(PWM_GLO_CTRL_PWM_GLOBAL_RESET_OFFSET);
    HAL_REG_32(IOT_PWM_GLO_CTRL_ADDR) &= (~BIT(PWM_GLO_CTRL_PWM_GLOBAL_RESET_OFFSET));

    return ret;

}


ENUM_HAL_RET_T halPWMClockSelect(ENUM_PWM_CLK_T ePwmClk)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    UINT32 u4Val = 0;

    if (ePwmClk >= PWM_CLK_NUM) {
        return HAL_RET_FAIL;
    }

    u4Val = HAL_REG_32(IOT_PWM_GLO_CTRL_ADDR);
    u4Val &= (~PWM_GLO_CTRL_PWM_TICK_CLOCK_SEL_MASK);
    u4Val |= (ePwmClk << PWM_GLO_CTRL_PWM_TICK_CLOCK_SEL_OFFSET);
    HAL_REG_32(IOT_PWM_GLO_CTRL_ADDR) = u4Val;

    return ret;
}

ENUM_HAL_RET_T halPWMEnable(UINT8 ucPwmIdx, UINT8 ucGlobalKick, UINT8 ucIOCtrl, UINT8 ucPolarity)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    IOT_PWM_TypeDef *pCRAddr = NULL;
    UINT32 u4Val = 0;

    // pinmux setting for PWM should be configured by system level.

    pCRAddr = (P_IOT_PWM_TypeDef)(((PUINT8)IOT_PWM0_ADDR) + (IOT_PWM_OFFSET * ucPwmIdx));

    u4Val = (pCRAddr->PWM_CTRL);
    // Enable clock
    u4Val |= (BIT(PWM_CTRL_PWM_CLOCK_EN_OFFSET));

    // Configure PWM
    if (ucGlobalKick) {
        u4Val |= (BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
    } else {
        u4Val &= (~BIT(PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET));
    }

    if (ucIOCtrl) {
        u4Val |= (BIT(PWM_CTRL_PWM_IO_CTRL_OFFSET));
    } else {
        u4Val &= (~BIT(PWM_CTRL_PWM_IO_CTRL_OFFSET));
    }

    if (ucPolarity) {
        u4Val |= (BIT(PWM_CTRL_POLARITY_OFFSET));
    } else {
        u4Val &= (~BIT(PWM_CTRL_POLARITY_OFFSET));
    }

    (pCRAddr->PWM_CTRL) = u4Val;


    return ret;
}

ENUM_HAL_RET_T halPWMDisable(UINT8 ucPwmIdx)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    IOT_PWM_TypeDef *pCRAddr = NULL;

    pCRAddr = (P_IOT_PWM_TypeDef)(((PUINT8)IOT_PWM0_ADDR) + (IOT_PWM_OFFSET * ucPwmIdx));

    // Disable clock
    (pCRAddr->PWM_CTRL) &= (~BIT(PWM_CTRL_PWM_CLOCK_EN_OFFSET));

    return ret;
}

ENUM_HAL_RET_T halPWMKick(UINT8 ucPwmIdx)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    IOT_PWM_TypeDef *pCRAddr = NULL;


    pCRAddr = (P_IOT_PWM_TypeDef)(((PUINT8)IOT_PWM0_ADDR) + (IOT_PWM_OFFSET * ucPwmIdx));

    (pCRAddr->PWM_CTRL) |= (BIT(PWM_CTRL_KICK_OFFSET));

    return ret;
}


ENUM_HAL_RET_T halPWMConfig(UINT8 ucPwmIdx, ENUM_PWM_STATE_T eState, UINT16 u2DutyCycle, UINT32 u4PwmFreq)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    ENUM_PWM_CLK_T ePwmClk = PWM_CLK_32K;
    UINT32 u4ClkFreq = 0;
    UINT32 u4PwmStep = 0;
    UINT16 u2OnTime = 0;
    UINT16 u2OffTime = 0;
    IOT_PWM_TypeDef *pCRAddr = NULL;
    UINT32 u4Val = 0;

    pCRAddr = (P_IOT_PWM_TypeDef)(((PUINT8)IOT_PWM0_ADDR) + (IOT_PWM_OFFSET * ucPwmIdx));

    ePwmClk = (ENUM_PWM_CLK_T)(((*IOT_PWM_GLO_CTRL_ADDR) & (PWM_GLO_CTRL_PWM_TICK_CLOCK_SEL_MASK)) >> PWM_GLO_CTRL_PWM_TICK_CLOCK_SEL_OFFSET);

    switch (ePwmClk) {
        case PWM_CLK_32K:
            u4ClkFreq = (32700);
            break;

        case PWM_CLK_2M:
            u4ClkFreq = (2 * 1000 * 1000);
            break;

        case PWM_CLK_XTAL: {
            UINT8 ucXtal = 0;
            ucXtal = ((HAL_REG_32(TOP_AON_CM4_STRAP_STA) & CM4_STRAP_STA_XTAL_FREQ_MASK) >> CM4_STRAP_STA_XTAL_FREQ_OFFSET);
            //printf("ucXtal=%d\n", ucXtal);
            if (XTAL_FREQ_20M == ucXtal) {
                u4ClkFreq = (20 * 1000 * 1000);
            } else if (XTAL_FREQ_40M == ucXtal) {
                u4ClkFreq = (40 * 1000 * 1000);
            } else if (XTAL_FREQ_26M == ucXtal) {
                u4ClkFreq = (26 * 1000 * 1000);
            } else if (XTAL_FREQ_52M == ucXtal) {
                u4ClkFreq = (52 * 1000 * 1000);
            }
        }
        break;

        default:
            return HAL_RET_FAIL;
    }

    /*=====================================================
    T(second)	PWM period
    F (Hz)	    PWM frequency = 1/T
    t (second)  Tick clk period
    f (Hz)	    Tick clk frequency = 1/t
    D (%)	    Duty cycle
    X (unit t)	Value of configurable register pwm_on_time[15:0], in unit t
    Y (unit t)	Value of configurable register pwm_off_time[15:0], in unit t
    Res (step)	PWM resolution of duty cycle on certain F, f

    (X + Y) t = T
    (X + Y) = T/t = f/F =Res
    D = X/((X+Y))
    X = D(X+Y) =  Df/F
    Y = f/F - X = f/F - Df/F = ((1-D)f)/F
    =====================================================*/

    u4PwmStep = (u4ClkFreq / u4PwmFreq);
    u2OnTime =  u2DutyCycle;
    u2OffTime = u4PwmStep - u2OnTime;

    /* avoid hw limitation: when u2OffTime is 1, the next individual kick
            for PWM #N invalid, pwm_global_reset can recover it */
    if (u2OffTime == 1) {
        u2OffTime = 0;
        u2OnTime += 1;
    }

    if (PWM_S0 == eState) {
        u4Val = (pCRAddr->PWM_PARAM_S0);
        u4Val &= (~PWM_PARAM_S0_PWM_OFF_TIME_MASK);
        u4Val |= (u2OffTime << PWM_PARAM_S0_PWM_OFF_TIME_OFFSET);
        u4Val &= (~PWM_PARAM_S0_PWM_ON_TIME_MASK);
        u4Val |= (u2OnTime << PWM_PARAM_S0_PWM_ON_TIME_OFFSET);
        (pCRAddr->PWM_PARAM_S0) = u4Val;
    } else {
        u4Val = (pCRAddr->PWM_PARAM_S1);
        u4Val &= (~PWM_PARAM_S1_PWM_OFF_TIME_MASK);
        u4Val |= (u2OffTime << PWM_PARAM_S1_PWM_OFF_TIME_OFFSET);
        u4Val &= (~PWM_PARAM_S1_PWM_ON_TIME_MASK);
        u4Val |= (u2OnTime << PWM_PARAM_S1_PWM_ON_TIME_OFFSET);
        (pCRAddr->PWM_PARAM_S1) = u4Val;
    }

    //printf("[%s]u4ClkFreq=%u, u4PwmFreq=%u, ucDutyCycle=%u, u2OnTime=%u, u2OffTime=%u\n", __FUNCTION__, u4ClkFreq, u4PwmFreq, u2DutyCycle,u2OnTime, u2OffTime);

    return ret;
}

ENUM_HAL_RET_T halPWMStateConfig(UINT8 ucPwmIdx, UINT16 u2S0StayCycle, UINT16 u2S1StayCycle, UINT8 ucReplayMode)
{
    ENUM_HAL_RET_T  ret = HAL_RET_SUCCESS;
    IOT_PWM_TypeDef *pCRAddr = NULL;
    UINT32 u4Val = 0;


    // stay cycle register only has 12 bits
    if ((u2S0StayCycle & 0xF000) || ((u2S1StayCycle & 0xF000))) {
        return HAL_RET_FAIL;
    }

    pCRAddr = (P_IOT_PWM_TypeDef)(((PUINT8)IOT_PWM0_ADDR) + (IOT_PWM_OFFSET * ucPwmIdx));
    u4Val = (pCRAddr->PWM_CTRL);

    u4Val &= (~PWM_CTRL_S0_STAY_CYCLE_MASK);
    u4Val |= (u2S0StayCycle << PWM_CTRL_S0_STAY_CYCLE_OFFSET);

    u4Val &= (~PWM_CTRL_S1_STAY_CYCLE_MASK);
    u4Val |= (u2S1StayCycle << PWM_CTRL_S1_STAY_CYCLE_OFFSET);

    if (ucReplayMode) {
        u4Val |= (BIT(PWM_CTRL_REPLAY_MODE_OFFSET));
    } else {
        u4Val &= (~BIT(PWM_CTRL_REPLAY_MODE_OFFSET));
    }
    (pCRAddr->PWM_CTRL) = u4Val;

    return ret;
}



ENUM_HAL_RET_T halPWMQuery(UINT8 ucPwmIdx, ENUM_PWM_STATE_T eState, UINT16 *pu2DutyCycle, UINT32 *pu4PwmFreq, UINT8 *pucEnable)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    ENUM_PWM_CLK_T ePwmClk = PWM_CLK_32K;
    UINT32 u4ClkFreq = 0;
    UINT32 u4PwmStep = 0;
    UINT16 u2OnTime = 0;
    UINT16 u2OffTime = 0;
    IOT_PWM_TypeDef *pCRAddr = NULL;
    UINT32 u4Val = 0;

    pCRAddr = (P_IOT_PWM_TypeDef)(((PUINT8)IOT_PWM0_ADDR) + (IOT_PWM_OFFSET * ucPwmIdx));

    u4Val = (pCRAddr->PWM_CTRL);// &= (~BIT(PWM_CTRL_PWM_CLOCK_EN_OFFSET));

    if (u4Val & BIT(PWM_CTRL_PWM_CLOCK_EN_OFFSET)) {
        (*pucEnable) = 1;
    } else {
        (*pucEnable) = 0;
        (*pu4PwmFreq) = 0;
        (*pu2DutyCycle) = 0;
        return HAL_RET_SUCCESS;
    }

    ePwmClk = (ENUM_PWM_CLK_T)(((*IOT_PWM_GLO_CTRL_ADDR) & (PWM_GLO_CTRL_PWM_TICK_CLOCK_SEL_MASK)) >> PWM_GLO_CTRL_PWM_TICK_CLOCK_SEL_OFFSET);

    switch (ePwmClk) {
        case PWM_CLK_32K:
            u4ClkFreq = (32700);
            break;

        case PWM_CLK_2M:
            u4ClkFreq = (2 * 1000 * 1000);
            break;

        case PWM_CLK_XTAL: {
            UINT8 ucXtal = 0;
            ucXtal = ((HAL_REG_32(TOP_AON_CM4_STRAP_STA) & CM4_STRAP_STA_XTAL_FREQ_MASK) >> CM4_STRAP_STA_XTAL_FREQ_OFFSET);
            if (XTAL_FREQ_20M == ucXtal) {
                u4ClkFreq = (20 * 1000 * 1000);
            } else if (XTAL_FREQ_40M == ucXtal) {
                u4ClkFreq = (40 * 1000 * 1000);
            } else if (XTAL_FREQ_26M == ucXtal) {
                u4ClkFreq = (26 * 1000 * 1000);
            } else if (XTAL_FREQ_52M == ucXtal) {
                u4ClkFreq = (52 * 1000 * 1000);
            }
        }
        break;

        default:
            return HAL_RET_FAIL;
    }

    /*=====================================================
    T(second)	PWM period
    F (Hz)	    PWM frequency = 1/T
    t (second)  Tick clk period
    f (Hz)	    Tick clk frequency = 1/t
    D (%)	    Duty cycle
    X (unit t)	Value of configurable register pwm_on_time[15:0], in unit t
    Y (unit t)	Value of configurable register pwm_off_time[15:0], in unit t
    Res (step)	PWM resolution of duty cycle on certain F, f

    (X + Y) t = T
    (X + Y) = T/t = f/F =Res
    D = X/((X+Y))
    X = D(X+Y) =  Df/F
    Y = f/F - X = f/F - Df/F = ((1-D)f)/F
    =====================================================*/

    if (PWM_S0 == eState) {
        u4Val = (pCRAddr->PWM_PARAM_S0);
        u2OnTime = ((u4Val & PWM_PARAM_S0_PWM_ON_TIME_MASK) >> PWM_PARAM_S0_PWM_ON_TIME_OFFSET);
        u2OffTime = ((u4Val & PWM_PARAM_S0_PWM_OFF_TIME_MASK) >> PWM_PARAM_S0_PWM_OFF_TIME_OFFSET);
    } else {
        u4Val = (pCRAddr->PWM_PARAM_S1);
        u2OnTime = ((u4Val & PWM_PARAM_S1_PWM_ON_TIME_MASK) >> PWM_PARAM_S1_PWM_ON_TIME_OFFSET);
        u2OffTime = ((u4Val & PWM_PARAM_S1_PWM_OFF_TIME_MASK) >> PWM_PARAM_S1_PWM_OFF_TIME_OFFSET);
    }

    u4PwmStep = u2OnTime + u2OffTime;
    (*pu4PwmFreq) = (u4ClkFreq / u4PwmStep);
    (*pu2DutyCycle) = u2OnTime;

    return ret;
}

#endif

