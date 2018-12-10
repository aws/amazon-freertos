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
** $Log: low_hal_rtc.h $
**
**
**
*/

#ifndef __LOW_HAL_RTC_H__
#define __LOW_HAL_RTC_H__

#ifdef HAL_RTC_MODULE_ENABLED
/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "mt7687_cm4_hw_memmap.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
#define RTC_TIMER_IRQ   (IRQn_Type)(19)
#define RTC_ALARM_IRQ   (IRQn_Type)(21)

#define RTC_BACKUP_BYTE_NUM_MAX     (144)
#define RTC_SPARE_NUM_MAX      (16)

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
extern ENUM_HAL_RET_T halRTCUnlock(void);
extern ENUM_HAL_RET_T halRTCInit(void);
extern ENUM_HAL_RET_T halRTCDeInit(void);

extern ENUM_HAL_RET_T halRTCSetTime(
    UINT8 ucYear,
    UINT8 ucMonth,
    UINT8 ucDayofMonth,
    UINT8 ucDayofWeek,
    UINT8 ucHour,
    UINT8 ucMinute,
    UINT8 ucSecond
);

extern ENUM_HAL_RET_T halRTCGetTime(
    PUINT8 pucYear,
    PUINT8 pucMonth,
    PUINT8 pucDayofMonth,
    PUINT8 pucDayofWeek,
    PUINT8 pucHour,
    PUINT8 pucMinute,
    PUINT8 pucSecond
);

extern ENUM_HAL_RET_T halRTCSetAlarm(
    UINT8 ucYear,
    UINT8 ucMonth,
    UINT8 ucDayofMonth,
    UINT8 ucDayofWeek,
    UINT8 ucHour,
    UINT8 ucMinute,
    UINT8 ucSecond,
    UINT8 ucEnableMask
);

extern ENUM_HAL_RET_T halRTCGetAlarm(
    PUINT8 pucYear,
    PUINT8 pucMonth,
    PUINT8 pucDayofMonth,
    PUINT8 pucDayofWeek,
    PUINT8 pucHour,
    PUINT8 pucMinute,
    PUINT8 pucSecond
);

extern ENUM_HAL_RET_T halRTCEnableAlarm(UINT8 ucEnable);

extern ENUM_HAL_RET_T halRTCEnterMode(void);


extern ENUM_HAL_RET_T halRTCSetBackup(UINT16 addr, const INT8 *buf, UINT16 len);
extern ENUM_HAL_RET_T halRTCClearBackup(UINT16 addr, UINT16 len);
extern ENUM_HAL_RET_T halRTCGetBackup(UINT16 addr, INT8 *buf, UINT16 len);

#endif /* HAL_RTC_MODULE_ENABLED */

#endif //#ifndef __LOW_HAL_RTC_H__

