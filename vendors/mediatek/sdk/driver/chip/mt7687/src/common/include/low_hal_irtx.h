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
** $Log: hal_IrTx.h $
**
** 12 15 2014 leo.hung
** [DVT][IrDA][WDT]
** 1. Add Ir Tx busy checking.
** 2. Update Ir Tx/Rx Pulse Width mode for learning mode.
** 3. Update WDT.
**
** 12 11 2014 leo.hung
** [DVT][PWM][IRDA][RTC][Crypto][WDT]
** 1. Update PWM, IRDA, RTC Crypto_AES/DES, WDT.
**
** 12 08 2014 leo.hung
** [DVT][PWM][IRDA][RTC][Crypto]
** 1. Update PWM, IRDA, RTC Crypto_AES/DES.
**
**
**
*/

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

#ifndef _HAL_LOW_IRTX_H
#define _HAL_LOW_IRTX_H

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

#include "hal_platform.h"
#ifdef HAL_IRTX_MODULE_ENABLED
#include "mt7687_cm4_hw_memmap.h"


/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

#define IRTX_OPER_FREQ  (2000) // 2000KHz (fixed, always 2M)

#define IR_RC6_TOGGLE_BIT   (3)
#define IR_RC5_TOGGLE_BIT   (2)

#define IRTX_PW_DEFAULT_FREQ    (36) // KHz
#define IRTX_PW_DEFAULT_DUTY    (25) // KHz
#define IRTX_PW_NUM_MAX         (68) // 17*4


/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

typedef enum _ENUM_IR_PROTOCOL {
    IR_NEC = 0,
    IR_RC5,
    IR_RC6,
    IR_SWM,
    IR_SW_PULSE_WIDTH,
    IR_PROTOCOL_NUM
} ENUM_IR_PROTOCOL;

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

extern ENUM_HAL_RET_T halIrTxInit(UINT8 ucOutputSel, UINT8 ucRegOrder, UINT8 ucBitOrder);
extern ENUM_HAL_RET_T halIrTxConfig(ENUM_IR_PROTOCOL eIrProtocol, UINT8 ucDataInvert, UINT8 ucOutputInvert,
                                    UINT32 u4L0High, UINT32 u4L0Low, UINT32 u4L1High, UINT32 u4L1Low,
                                    UINT16 u2Freq, UINT8 ucDutyCycle);
extern ENUM_HAL_RET_T halIrTxSend(UINT32 u4TxData0, UINT32 u4TxData1, UINT32 u4TxData2, UINT8 ucBitNum, UINT8 ucRepeat);
extern ENUM_HAL_RET_T halIrTxPWSend(UINT8 ucDataNum, PUINT8 pData, UINT8 ucBasePeriod);
/* extern VOID halIrTxSWMSet(UINT8 ucEnable, UINT8 ucVal); */
extern VOID halIrTxClearInterrupt(void);


#endif
#endif //_HAL_IRTX_H

