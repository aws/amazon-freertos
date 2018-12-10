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
** $Log: low_hal_rtc.c $
**
**
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
#include "hal_rtc.h"
#ifdef HAL_RTC_MODULE_ENABLED

#include "low_hal_rtc.h"
#include "cos_api.h"
#include "hal_gpt.h"
#include <stdio.h>

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
#if 0
ENUM_HAL_RET_T halRTCLock()
{
    P_IOT_RTC_CTRL_TypeDef pRTCCtrlTypeDef = NULL;
    UINT32 u4Val = 0;
    UINT32 u4Count = 0;

    pRTCCtrlTypeDef = (P_IOT_RTC_CTRL_TypeDef)(CM4_RTC_BASE + 0x0004);

    // Wait RTC R/W ready
    // R    RTC_BASE + 0x100 [1] G_ENABLE    1'b1
    do {
        u4Val = HAL_REG_32(IOT_RTC_COREPDN_ADDR);
        if ((u4Count++) > 0xFFFFFF) {
            // Timeout
            return HAL_RET_FAIL;
        }
    } while (0 == (u4Val & BIT(RTC_COREPDN_G_ENABLE_OFFSET)));


    // Set RTC Power Check
    // W RTC_BASE + 0x04 [7:0]   RTC_PWRCHK1 8'hC6
    (pRTCCtrlTypeDef->RTC_PWRCHK1) = 0x0;

    // W RTC_BASE + 0x08 [7:0]   RTC_PWRCHK2 8'h9A
    (pRTCCtrlTypeDef->RTC_PWRCHK2) = 0x0;


    // Set RTC Key
    // W RTC_BASE + 0x0C [7:0]   RTC_KEY 8'h59
    (pRTCCtrlTypeDef->RTC_KEY) = 0x0;

    // Set RTC Protections
    // W    RTC_BASE + 0x10 [7:0]   RTC_PROT1   8'hA3
    (pRTCCtrlTypeDef->RTC_PROT1) = 0x0;
    // W    RTC_BASE + 0x14 [7:0]   RTC_PROT2   8'h57
    (pRTCCtrlTypeDef->RTC_PROT2) = 0x0;
    // W    RTC_BASE + 0x18 [7:0]   RTC_PROT3   8'h67
    (pRTCCtrlTypeDef->RTC_PROT3) = 0x0;
    // W    RTC_BASE + 0x1C [7:0]   RTC_PROT4   8'hD2
    (pRTCCtrlTypeDef->RTC_PROT4) = 0x0;

    return HAL_RET_SUCCESS;
}
#endif

ENUM_HAL_RET_T halRTCUnlock()
{
    P_IOT_RTC_CTRL_TypeDef pRTCCtrlTypeDef = NULL;
    UINT32 u4Val = 0;
    UINT32 u4Count = 0;

    pRTCCtrlTypeDef = (P_IOT_RTC_CTRL_TypeDef)(CM4_RTC_BASE + 0x0004);

    // Wait RTC R/W ready
    // R    RTC_BASE + 0x100 [1] G_ENABLE    1'b1
    do {
        u4Val = HAL_REG_32(IOT_RTC_COREPDN_ADDR);
        if ((u4Count++) > 0xFFFFFF) {
            // Timeout
            printf("halRTCUnlock timeout\r\n");
            return HAL_RET_FAIL;
        }
    } while (0 == (u4Val & BIT(RTC_COREPDN_G_ENABLE_OFFSET)));
    printf("u4Count:%d\r\n", u4Count);

    // Set RTC Power Check
    // W RTC_BASE + 0x04 [7:0]   RTC_PWRCHK1 8'hC6
    (pRTCCtrlTypeDef->RTC_PWRCHK1) = IOT_RTC_RTC_PWRCHK1;

    // W RTC_BASE + 0x08 [7:0]   RTC_PWRCHK2 8'h9A
    (pRTCCtrlTypeDef->RTC_PWRCHK2) = IOT_RTC_RTC_PWRCHK2;


    // Set RTC Key
    // W RTC_BASE + 0x0C [7:0]   RTC_KEY 8'h59
    (pRTCCtrlTypeDef->RTC_KEY) = IOT_RTC_RTC_KEY;

    // Set RTC Protections
    // W    RTC_BASE + 0x10 [7:0]   RTC_PROT1   8'hA3
    (pRTCCtrlTypeDef->RTC_PROT1) = IOT_RTC_RTC_PROT1;
    // W    RTC_BASE + 0x14 [7:0]   RTC_PROT2   8'h57
    (pRTCCtrlTypeDef->RTC_PROT2) = IOT_RTC_RTC_PROT2;
    // W    RTC_BASE + 0x18 [7:0]   RTC_PROT3   8'h67
    (pRTCCtrlTypeDef->RTC_PROT3) = IOT_RTC_RTC_PROT3;
    // W    RTC_BASE + 0x1C [7:0]   RTC_PROT4   8'hD2
    (pRTCCtrlTypeDef->RTC_PROT4) = IOT_RTC_RTC_PROT4;

    return HAL_RET_SUCCESS;
}
#if 0
/*
    000: Wait 2^5-1 ~ 2^5 cycle of RTC clock
    001: Wait 2^6-4 ~ 2^6 cycle of RTC clock
    010: Wait 2^8-2^4~2^8 cycle of RTC clock
    011: Wait 2^10-2^6~2^10 cycle of RTC clock
    100: Wait 2^12-2^8~2^12 cycle of RTC clock
    101: Wait 2^13-2^9~2^13 cycle of RTC clock
    110: Wait 2^14-2^10~2^14 cycle of RTC clock
    111: Wait 2^15-2^11~2^15 cycle of RTC clock
*/
VOID halRTCSetDebounce(UINT8 ucDebounce)
{
    P_IOT_RTC_CTRL_TypeDef pRTCCtrlTypeDef = NULL;

    pRTCCtrlTypeDef = (P_IOT_RTC_CTRL_TypeDef)(CM4_RTC_BASE + 0x0004);

    (pRTCCtrlTypeDef->RTC_DEBNCE) = (((pRTCCtrlTypeDef->RTC_DEBNCE) & (~RTC_DEBNCE_DEBOUNCE_MASK)) | (ucDebounce << RTC_DEBNCE_DEBOUNCE_OFFSET));
}
#endif
/*
    Inhibit status indicator.
    Before reading the registers of YEAR, MONTH, WEEK, DAY, HOUR, MIN, and SEC, read this bit first.
    0: UP is OK to read/write RTC
    1: RTC is updating RTC clock, inhibit UP write timer related registers and read following command YEAR, MONTH, WEEK, DAY, HOUR, MIN, SEC, TIMERCTL, TIMERH, and TIMERL.
*/
ENUM_HAL_RET_T halRTCCheckInhibit()
{
    P_IOT_RTC_CTRL_TypeDef pRTCCtrlTypeDef = NULL;
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    UINT8 ucCnt = 0;
    UINT32 ucTimeout = 0xFFFFFF;

    pRTCCtrlTypeDef = (P_IOT_RTC_CTRL_TypeDef)(CM4_RTC_BASE + 0x0004);

    do {
        ucCnt++;
    } while (((pRTCCtrlTypeDef->RTC_CTL) & BIT(RTC_CTL_INHIBIT_OFFSET)) && (ucCnt < ucTimeout));

    if (ucCnt < ucTimeout) {
        ret = HAL_RET_SUCCESS;
    } else {
        ret = HAL_RET_FAIL;
    }

    return ret;
}

ENUM_HAL_RET_T halRTCGetTime(
    PUINT8 pucYear,
    PUINT8 pucMonth,
    PUINT8 pucDayofMonth,
    PUINT8 pucDayofWeek,
    PUINT8 pucHour,
    PUINT8 pucMinute,
    PUINT8 pucSecond
)
{
    // TODO: read Time should be done A.S.A.P to prevent from 0h:59m:59s --> 1h:0m:0s
    // interrupt handling can introduce read time error
    // MAYBE read time function with interrupt MASKED is needed?
    P_IOT_RTC_CTRL_TypeDef pRTCCtrlTypeDef = NULL;
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;

    pRTCCtrlTypeDef = (P_IOT_RTC_CTRL_TypeDef)(CM4_RTC_BASE + 0x0004);

    if (HAL_RET_SUCCESS == halRTCCheckInhibit()) {
        (*pucYear) = (pRTCCtrlTypeDef->RTC_TC_YEA);
    } else {
        return HAL_RET_FAIL;
    }

    if (HAL_RET_SUCCESS == halRTCCheckInhibit()) {
        (*pucMonth) = (pRTCCtrlTypeDef->RTC_TC_MON);
    } else {
        return HAL_RET_FAIL;
    }

    if (HAL_RET_SUCCESS == halRTCCheckInhibit()) {
        (*pucDayofMonth) = (pRTCCtrlTypeDef->RTC_TC_DOM);
    } else {
        return HAL_RET_FAIL;
    }

    if (HAL_RET_SUCCESS == halRTCCheckInhibit()) {
        (*pucDayofWeek) = (pRTCCtrlTypeDef->RTC_TC_DOW);
    } else {
        return HAL_RET_FAIL;
    }

    if (HAL_RET_SUCCESS == halRTCCheckInhibit()) {
        (*pucHour) = (pRTCCtrlTypeDef->RTC_TC_HOU);
    } else {
        return HAL_RET_FAIL;
    }

    if (HAL_RET_SUCCESS == halRTCCheckInhibit()) {
        (*pucMinute) = (pRTCCtrlTypeDef->RTC_TC_MIN);
    } else {
        return HAL_RET_FAIL;
    }

    if (HAL_RET_SUCCESS == halRTCCheckInhibit()) {
        (*pucSecond) = (pRTCCtrlTypeDef->RTC_TC_SEC);
    } else {
        return HAL_RET_FAIL;
    }

    return ret;
}

ENUM_HAL_RET_T halRTCSetTime(
    UINT8 ucYear,
    UINT8 ucMonth,
    UINT8 ucDayofMonth,
    UINT8 ucDayofWeek,
    UINT8 ucHour,
    UINT8 ucMinute,
    UINT8 ucSecond
)
{
    P_IOT_RTC_CTRL_TypeDef pRTCCtrlTypeDef = NULL;
    ENUM_HAL_RET_T ret = HAL_RET_FAIL;

    pRTCCtrlTypeDef = (P_IOT_RTC_CTRL_TypeDef)(CM4_RTC_BASE + 0x0004);

    ret = halRTCUnlock();

    if (HAL_RET_SUCCESS == ret) {
        // Stop RTC
        // W   RTC_BASE + 0x20 [0] RC_STOP 1'b1
        (pRTCCtrlTypeDef->RTC_CTL) |= BIT(RTC_CTL_RC_STOP_OFFSET);

        // Set RTC time
        // W   RTC_BASE + 0x40 [7:0]   RTC_TC_YEA  USER_DEFINED    Year
        (pRTCCtrlTypeDef->RTC_TC_YEA) = ucYear;
        // W   RTC_BASE + 0x44 [7:0]   RTC_TC_MON  USER_DEFINED    Month
        (pRTCCtrlTypeDef->RTC_TC_MON) = ucMonth;
        // W   RTC_BASE + 0x48 [7:0]   RTC_TC_DOM  USER_DEFINED    Day of month
        (pRTCCtrlTypeDef->RTC_TC_DOM) = ucDayofMonth;
        // W   RTC_BASE + 0x4C [7:0]   RTC_TC_DOW  USER_DEFINED    Day of week
        (pRTCCtrlTypeDef->RTC_TC_DOW) = ucDayofWeek;
        // W   RTC_BASE + 0x50 [7:0]   RTC_TC_HOU  USER_DEFINED    Hour
        (pRTCCtrlTypeDef->RTC_TC_HOU) = ucHour;
        // W   RTC_BASE + 0x54 [7:0]   RTC_TC_MIN  USER_DEFINED    Minute
        (pRTCCtrlTypeDef->RTC_TC_MIN) = ucMinute;
        // W   RTC_BASE + 0x58 [7:0]   RTC_TC_SEC  USER_DEFINED    Second
        (pRTCCtrlTypeDef->RTC_TC_SEC) = ucSecond;

        // Enable RTC
        // W   RTC_BASE + 0x20 [0] RC_STOP 1'b0
        (pRTCCtrlTypeDef->RTC_CTL) &= (~BIT(RTC_CTL_RC_STOP_OFFSET));
    }

    return ret;

}


ENUM_HAL_RET_T halRTCInit(
)
{
    P_IOT_RTC_CTRL_TypeDef pRTCCtrlTypeDef = NULL;
    ENUM_HAL_RET_T ret = HAL_RET_FAIL;

    pRTCCtrlTypeDef = (P_IOT_RTC_CTRL_TypeDef)(CM4_RTC_BASE + 0x0004);

    ret = halRTCUnlock();

    if (HAL_RET_SUCCESS == ret) {
        // Reduce RTC power consumption
        (pRTCCtrlTypeDef->RTC_XOSC_CFG) &= (~(BIT(RTC_XOSC_CFG_OSCPDN_OFFSET)));
        (pRTCCtrlTypeDef->RTC_XOSC_CFG) &= (~(BIT(RTC_XOSC_CFG_AMPCTL_EN_OFFSET)));
        (pRTCCtrlTypeDef->RTC_XOSC_CFG) &= (~(BIT(RTC_XOSC_CFG_AMP_GSEL_OFFSET)));
        (pRTCCtrlTypeDef->RTC_XOSC_CFG) &= (~(RTC_XOSC_CFG_OSCCALI_MASK));
        (pRTCCtrlTypeDef->RTC_XOSC_CFG) |= (BIT(RTC_XOSC_CFG_AMP_GSEL_OFFSET));
        printf("reduce RTC power\r\n");
    }

    return ret;
}

ENUM_HAL_RET_T halRTCDeInit(void)
{
    P_IOT_RTC_CTRL_TypeDef pRTCCtrlTypeDef = NULL;
    ENUM_HAL_RET_T ret = HAL_RET_FAIL;

    pRTCCtrlTypeDef = (P_IOT_RTC_CTRL_TypeDef)(CM4_RTC_BASE + 0x0004);

    ret = halRTCUnlock();

    if (HAL_RET_SUCCESS == ret) {
        // Stop RTC
        // W   RTC_BASE + 0x20 [0] RC_STOP 1'b1
        (pRTCCtrlTypeDef->RTC_CTL) |= BIT(RTC_CTL_RC_STOP_OFFSET);
    }

    return ret;
}


/*
UINT8 ucEnableMask
    Bit 7   ALMYR   Alarm year enable
        0: Alarm does not compare year.
        1: Alarm compares year.
    Bit 6   ALMMON  Alarm month enable
        0: Alarm does not compare month.
        1: Alarm compares month.
    Bit 5   ALMDOM  Alarm day of month enable
        0: Alarm does not compare day of month.
        1: Alarm compares day of month.
    Bit 4   ALMDOW  Alarm day of week enable
        0: Alarm does not compare day of week.
        1: Alarm compares day of week.
    Bit 3   ALMHR   Alarm hour enable
        0: Alarm does not compare hour.
        1: Alarm compares hour.
    Bit 2   ALMMIN  Alarm minute enable
        0: Alarm does not compare minute.
        1: Alarm compares minute.
    Bit 1   ALMSEC  Alarm second enable
        0: Alarm does not compare second.
        1: Alarm compares second.
    Bit 0   ALMEN   Alarm enable
        0: Disable alarm.
        1: Enable alarm.
*/
ENUM_HAL_RET_T halRTCSetAlarm(
    UINT8 ucYear,
    UINT8 ucMonth,
    UINT8 ucDayofMonth,
    UINT8 ucDayofWeek,
    UINT8 ucHour,
    UINT8 ucMinute,
    UINT8 ucSecond,
    UINT8 ucEnableMask
)
{
    P_IOT_RTC_CTRL_TypeDef pRTCCtrlTypeDef = NULL;
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;


    pRTCCtrlTypeDef = (P_IOT_RTC_CTRL_TypeDef)(CM4_RTC_BASE + 0x0004);

    ret = halRTCUnlock();

    if (HAL_RET_SUCCESS == ret) {
        // Set RTC alarm
        // W    RTC_BASE + 0x60 [7:0]   RTC_AL_YEA  USER_DEFINED    Year
        (pRTCCtrlTypeDef->RTC_AL_YEAR) = ucYear;
        // W    RTC_BASE + 0x64 [7:0]   RTC_AL_MON  USER_DEFINED    Month
        (pRTCCtrlTypeDef->RTC_AL_MON) = ucMonth;
        // W    RTC_BASE + 0x68 [7:0]   RTC_AL_DOM  USER_DEFINED    Day of month
        (pRTCCtrlTypeDef->RTC_AL_DOM) = ucDayofMonth;
        // W    RTC_BASE + 0x6C [7:0]   RTC_AL_DOW  USER_DEFINED    Day of week
        (pRTCCtrlTypeDef->RTC_AL_DOW) = ucDayofWeek;
        // W    RTC_BASE + 0x70 [7:0]   RTC_AL_HOU  USER_DEFINED    Hour
        (pRTCCtrlTypeDef->RTC_AL_HOUR) = ucHour;
        // W    RTC_BASE + 0x74 [7:0]   RTC_AL_MIN  USER_DEFINED    Minute
        (pRTCCtrlTypeDef->RTC_AL_MIN) = ucMinute;
        // W    RTC_BASE + 0x78 [7:0]   RTC_AL_SEC  USER_DEFINED    Second
        (pRTCCtrlTypeDef->RTC_AL_SEC) = ucSecond;

        // W    RTC_BASE + 0x7C [7:1]   RTC_AL_CTL  USER_DEFINED    Set alarm control
        // W    RTC_BASE + 0x7C [0]     RTC_AL_CTL/ALMEN    1'b1    Enable alarm
        (pRTCCtrlTypeDef->RTC_AL_CTL) = ucEnableMask;
    }

    return ret;
}

ENUM_HAL_RET_T halRTCGetAlarm(
    PUINT8 pucYear,
    PUINT8 pucMonth,
    PUINT8 pucDayofMonth,
    PUINT8 pucDayofWeek,
    PUINT8 pucHour,
    PUINT8 pucMinute,
    PUINT8 pucSecond
)
{
    P_IOT_RTC_CTRL_TypeDef pRTCCtrlTypeDef = NULL;
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;

    pRTCCtrlTypeDef = (P_IOT_RTC_CTRL_TypeDef)(CM4_RTC_BASE + 0x0004);

    (*pucYear) = (pRTCCtrlTypeDef->RTC_AL_YEAR);
    (*pucMonth) = (pRTCCtrlTypeDef->RTC_AL_MON);
    (*pucDayofMonth) = (pRTCCtrlTypeDef->RTC_AL_DOM);
    (*pucDayofWeek) = (pRTCCtrlTypeDef->RTC_AL_DOW);
    (*pucHour) = (pRTCCtrlTypeDef->RTC_AL_HOUR);
    (*pucMinute) = (pRTCCtrlTypeDef->RTC_AL_MIN);
    (*pucSecond) = (pRTCCtrlTypeDef->RTC_AL_SEC);

    return ret;
}

ENUM_HAL_RET_T halRTCEnableAlarm(UINT8 ucEnable)
{
    P_IOT_RTC_CTRL_TypeDef pRTCCtrlTypeDef = NULL;
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;


    pRTCCtrlTypeDef = (P_IOT_RTC_CTRL_TypeDef)(CM4_RTC_BASE + 0x0004);

    ret = halRTCUnlock();

    if (HAL_RET_SUCCESS == ret) {
        if (1 == ucEnable) {
            // W    RTC_BASE + 0x7C [0]     RTC_AL_CTL/ALMEN    1'b1    Enable alarm
            (pRTCCtrlTypeDef->RTC_AL_CTL) |= BIT(0);
        } else {
            (pRTCCtrlTypeDef->RTC_AL_CTL) &= (~BIT(0));
        }
    }
    return ret;
}

#if 0
ENUM_HAL_RET_T halRTCSetCountDownTimer(
    UINT16 u2TimerCnt, // unit: 1/32 sec
    UINT8 ucEnable
)
{
    P_IOT_RTC_CTRL_TypeDef pRTCCtrlTypeDef = NULL;
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;


    pRTCCtrlTypeDef = (P_IOT_RTC_CTRL_TypeDef)(CM4_RTC_BASE + 0x0004);

    ret = halRTCUnlock();

    if (HAL_RET_SUCCESS == ret) {
        // Set RTC timer
        // W   RTC_BASE + 0x90 [1] TR_INTEN    1'b0    Disalbe Timer interrupt
        (pRTCCtrlTypeDef->RTC_TIMER_CTL) &= (~BIT(RTC_TIMER_CTL_TR_INTEN_OFFSET));

        // W   RTC_BASE + 0x94 [7:0]   RTC_TIMER_CNTH  USER_DEFINED    Upper byte of timer value
        (pRTCCtrlTypeDef->RTC_TIMER_CNTH) = (u2TimerCnt >> 8);
        // W   RTC_BASE + 0x98 [7:0]   RTC_TIMER_CNTL  USER_DEFINED    Lower byte of timer value
        (pRTCCtrlTypeDef->RTC_TIMER_CNTL) = (u2TimerCnt & 0xFF);

        if (ucEnable) {
            // W   RTC_BASE + 0x90 [1] TR_INTEN    1'b1    Enable Timer
            (pRTCCtrlTypeDef->RTC_TIMER_CTL) |= (BIT(RTC_TIMER_CTL_TR_INTEN_OFFSET));
        }
    }

    return ret;
}
#endif

ENUM_HAL_RET_T halRTCEnterMode()
{
    P_IOT_RTC_CTRL_TypeDef pRTCCtrlTypeDef = NULL;
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    UINT32 u4Val = 0;
    UINT32 u4Count = 0;

    pRTCCtrlTypeDef = (P_IOT_RTC_CTRL_TypeDef)(CM4_RTC_BASE + 0x0004);

    ret = halRTCUnlock();
    if (HAL_RET_SUCCESS == ret) {
        //Set debounce time
        // W   RTC_BASE + 0x2C [2:0]   RTC_DEBNCE  3'b000  Less than 1 ms
        (pRTCCtrlTypeDef->RTC_DEBNCE) &= (~RTC_DEBNCE_DEBOUNCE_MASK);

        //(pRTCCtrlTypeDef->RTC_XOSC_CFG) |= (BIT(RTC_XOSC_CFG_AMP_GSEL_OFFSET));
        // Disable RTC_PMU_EN
        // W   RTC_BASE + 0x30 [0] PMU_EN  1'b0
        (pRTCCtrlTypeDef->RTC_PMU_EN) &= (~BIT(RTC_PMU_EN_OFFSET));
        printf("start 50ms delay\r\n");
        // delay 50ms for safe power down
        hal_gpt_delay_ms(50);
        printf("50ms delay finish\r\n");
        // If after 50ms chip still have power, set PMU_EN high and return fail for enter RTC mode process
        // Wait RTC R/W ready
        // R    RTC_BASE + 0x100 [1] G_ENABLE    1'b1
        do {
            u4Val = HAL_REG_32(IOT_RTC_COREPDN_ADDR);
            if ((u4Count++) > 0xFFFFFF) {
                printf("restore timeout\r\n");
                // Timeout
                return HAL_RET_FAIL;
            }
        } while (0 == (u4Val & BIT(RTC_COREPDN_G_ENABLE_OFFSET)));
        // Set RTC Key
        // W RTC_BASE + 0x0C [7:0]   RTC_KEY 8'h59
        (pRTCCtrlTypeDef->RTC_KEY) = IOT_RTC_RTC_KEY;
        //(pRTCCtrlTypeDef->RTC_XOSC_CFG) &= (~BIT(RTC_XOSC_CFG_AMP_GSEL_OFFSET));
        // Enable RTC_PMU_EN
        // W   RTC_BASE + 0x30 [0] PMU_EN  1'b1
        (pRTCCtrlTypeDef->RTC_PMU_EN) |= (BIT(RTC_PMU_EN_OFFSET));
        ret = HAL_RET_FAIL;
    }
    return ret;
}
#if 0
ENUM_HAL_RET_T halRTCSetPMUEN()
{
    P_IOT_RTC_CTRL_TypeDef pRTCCtrlTypeDef = NULL;
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;

    pRTCCtrlTypeDef = (P_IOT_RTC_CTRL_TypeDef)(CM4_RTC_BASE + 0x0004);

    ret = halRTCUnlock();
    if (HAL_RET_SUCCESS == ret) {
        (pRTCCtrlTypeDef->RTC_PMU_EN) |= (BIT(RTC_PMU_EN_OFFSET));
    }

    return ret;
}
#endif

static ENUM_HAL_RET_T halRTCWriteBackup(UINT16 addr, const INT8 *buf, UINT16 len)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    P_IOT_RTC_BACKUP_TypeDef pRTCBackupTypeDef = NULL;
    volatile UINT32 *pu4RegVal = NULL;
    UINT8 ucRegOffset = 0;
    UINT8 ucRegIndex = 0;
    UINT16 u2Residual = len;
    UINT32 u4Val = 0;
    UINT16 i = 0;

    pRTCBackupTypeDef = (P_IOT_RTC_BACKUP_TypeDef)(CM4_RTC_BASE + RTC_BACKUP0_OFFSET);

    // sanity check
    if ((addr + len) > RTC_BACKUP_BYTE_NUM_MAX) {
        return HAL_RET_FAIL;
    }

    ret = halRTCUnlock();
    if (HAL_RET_SUCCESS == ret) {
        ucRegOffset = (addr & 0x3);
        ucRegIndex = (addr >> 2);
        if (0 != ucRegOffset) {
            pu4RegVal = ((&(pRTCBackupTypeDef->RTC_BACKUP0)) + ucRegIndex);
            u4Val = (*pu4RegVal);
            for (i = 0; ((i < (4 - ucRegOffset)) && (u2Residual > 0)); i++) {
                u4Val = (u4Val & (~((0xFF) << ((ucRegOffset + i) * 8))));
                if (NULL != buf) {
                    u4Val = (u4Val | ((*buf) << ((ucRegOffset + i) * 8)));

                    buf = buf + 1;
                } else {
                    u4Val = (u4Val | ((0x00) << ((ucRegOffset + i) * 8)));
                }
                u2Residual = u2Residual - 1;
            }
            (*pu4RegVal) = u4Val;

            ucRegIndex += 1;
        }
        pu4RegVal = ((&(pRTCBackupTypeDef->RTC_BACKUP0)) + ucRegIndex);
        while (u2Residual >= 4) {
            if (NULL != buf) {
                u4Val = (((*buf) << 0) | ((*(buf + 1)) << 8) | ((*(buf + 2)) << 16) | ((*(buf + 3)) << 24));
                (*pu4RegVal) = u4Val;
                pu4RegVal = pu4RegVal + 1;
                buf = buf + 4;
            } else {
                u4Val = 0x0;
                (*pu4RegVal) = u4Val;
                pu4RegVal = pu4RegVal + 1;
            }
            u2Residual = u2Residual - 4;
        }

        if (u2Residual > 0) {
            u4Val = (*pu4RegVal);
            switch (u2Residual) {
                case 1:
                    u4Val = (u4Val & 0xFFFFFF00);
                    if (NULL != buf) {
                        u4Val = u4Val | ((*buf) << 0);
                    }
                    break;
                case 2:
                    u4Val = (u4Val & 0xFFFF0000);
                    if (NULL != buf) {
                        u4Val = (u4Val | (((*buf) << 0) | ((*(buf + 1)) << 8)));
                    }
                    break;
                case 3:
                    u4Val = (u4Val & 0xFF000000);
                    if (NULL != buf) {
                        u4Val = (u4Val | (((*buf) << 0) | ((*(buf + 1)) << 8) | ((*(buf + 2)) << 16)));
                    }
                    break;
            }
            (*pu4RegVal) = u4Val;
        }

    }

    return ret;
}
ENUM_HAL_RET_T halRTCSetBackup(UINT16 addr, const INT8 *buf, UINT16 len)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    ret = halRTCWriteBackup(addr, buf, len);

    return ret;
}


ENUM_HAL_RET_T halRTCClearBackup(UINT16 addr, UINT16 len)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    ret = halRTCWriteBackup(addr, NULL, len);

    return ret;
}


ENUM_HAL_RET_T halRTCGetBackup(UINT16 addr, INT8 *buf, UINT16 len)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    P_IOT_RTC_BACKUP_TypeDef pRTCBackupTypeDef = NULL;
    volatile UINT32 *pu4RegVal = NULL;
    UINT8 ucRegOffset = 0;
    UINT8 ucRegIndex = 0;
    UINT16 u2Residual = len;
    UINT32 u4Val = 0;
    UINT16 i = 0;

    pRTCBackupTypeDef = (P_IOT_RTC_BACKUP_TypeDef)(CM4_RTC_BASE + RTC_BACKUP0_OFFSET);

    // sanity check
    if ((addr + len) > RTC_BACKUP_BYTE_NUM_MAX) {
        return HAL_RET_FAIL;
    }

    ret = halRTCUnlock();
    if (HAL_RET_SUCCESS == ret) {
        ucRegOffset = (addr & 0x3);
        ucRegIndex = (addr >> 2);
        if (0 != ucRegOffset) {
            pu4RegVal = ((&(pRTCBackupTypeDef->RTC_BACKUP0)) + ucRegIndex);
            u4Val = (*pu4RegVal);
            for (i = 0; ((i < (4 - ucRegOffset)) && (u2Residual > 0)); i++) {
                (*buf) = ((u4Val & (((0xFF) << ((ucRegOffset + i) * 8)))) >> ((ucRegOffset + i) * 8));

                buf = buf + 1;
                u2Residual = u2Residual - 1;
            }

            ucRegIndex += 1;
        }
        pu4RegVal = ((&(pRTCBackupTypeDef->RTC_BACKUP0)) + ucRegIndex);
        while (u2Residual >= 4) {
            u4Val = (*pu4RegVal);

            (*buf) = (u4Val & 0xFF);
            (*(buf + 1)) = ((u4Val & 0xFF00) >> 8);
            (*(buf + 2)) = ((u4Val & 0xFF0000) >> 16);
            (*(buf + 3)) = ((u4Val & 0xFF000000) >> 24);

            pu4RegVal = pu4RegVal + 1;
            buf = buf + 4;
            u2Residual = u2Residual - 4;
        }

        if (u2Residual > 0) {
            u4Val = (*pu4RegVal);

            for (i = 0; i < u2Residual; i++) {
                (*buf) = ((u4Val & (0xFF << (i * 8))) >> (i * 8));
                buf = buf + 1;
            }
        }

    }

    return ret;

}

#endif /* HAL_RTC_MODULE_ENABLED */


