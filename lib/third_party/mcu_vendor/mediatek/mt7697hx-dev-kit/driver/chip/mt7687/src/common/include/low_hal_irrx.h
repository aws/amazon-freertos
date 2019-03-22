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
** $Log: low_hal_irrx.h $
**
**
**
**
*/

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

#ifndef __LOW_HAL_IRRX_H__
#define __LOW_HAL_IRRX_H__

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

#include "hal_platform.h"
#ifdef HAL_IRRX_MODULE_ENABLED
#include "mt7687_cm4_hw_memmap.h"
#include "top.h" // for gXtalFreq



/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

#define IRRX_OPER_FREQ  (2000) // 2000KHz
#define IRRX_BUS_FREQ   (top_xtal_freq_get()/1000) // KHz

#define IRRX_PWD_OK_PERIOD      (64)
#define IRRX_PWD_DATA_MAX_NUM   (68) //(17*4)
#define IRRX_PWD_DATA_MAX_VAL   (0xFF) // 8 bits

#define IRRX_RC5_DEFAULT_OK_PERIOD  (3)

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

typedef enum _low_hal_irrx_mode_e {
    LOW_HAL_IRRX_MODE_RC5 = 0,
    LOW_HAL_IRRX_MODE_PWD
} low_hal_irrx_mode_t;

typedef void (*low_hal_irrx_callback_t)(void  *user_data);

typedef struct {
    low_hal_irrx_callback_t func;
    void *argument;
} irrx_user_callback_t;



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
extern VOID halIrRxClearInterrupt(void);
extern VOID halIrRxRC5Read(PUINT8 pucBitNum, PUINT32 pu4DW0, PUINT32 pu4DW1);
extern VOID halIrRxPWDRead(PUINT8 pucPWNum, PUINT8 pucPWBuf, UINT8 ucBufLen);
extern VOID halIrRxReset(void);
extern ENUM_HAL_RET_T halIrRxRC5(UINT8 ucInverse, UINT8 ucBitReverse, UINT8 ucIgnoreEmpty, UINT16 u2SamplePeriod);
extern ENUM_HAL_RET_T halIrRxPulseWidthDetect(
    UINT32  u4DeGlitchCnt,
    UINT8   ucInverse,
    UINT32  u4TherminateThresholdUs,
    UINT32  *pu4DetectPrecesionUs
);
extern ENUM_HAL_RET_T halIrRxPulseWidthDetect_DVT(
    UINT32  u4DeGlitchCnt,
    UINT8   ucInverse,
    UINT32  u4OKPeriod,
    UINT32  u4SAPeriod,
    UINT32  u4ChkPeriod,
    UINT8   ucLowTerminate,
    UINT8   ucHighTerminate
);

extern ENUM_HAL_RET_T halIrRxRegisterCallback(low_hal_irrx_mode_t mode, low_hal_irrx_callback_t callback,void *parameter);

#endif

#endif //__LOW_HAL_IRRX_H__

