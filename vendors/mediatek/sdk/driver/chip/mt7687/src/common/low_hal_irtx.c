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
** $Log: hal_IrTx.c $
**
** 04 16 2015 leo.hung
** [DVT][IrTx]
** 1. Add rounding for duty cycle calculation.
** 2. Update IrTx DVT case.
**
** 12 19 2014 leo.hung
** [DVT][IRDA]
** 1. Update IR learning mode.
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


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/


#include "hal_platform.h"

#ifdef HAL_IRTX_MODULE_ENABLED
#include "low_hal_irtx.h"
#include <stdio.h>
#include <string.h>
#include "type_def.h"

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


UINT8 halIrTxRound(UINT32 x, UINT32 y, UINT32 z)
{
    /*
        z = round(x/y)

        if ((x/y) > *.5)
            return 1;
    */

    if (((x - (z * y)) * 2) > y) {
        return 1;
    }

    return 0;
}


/*
 *  ucOutputSel
 *      0: IR output is IRTX baseband signal  --> duty cycle as 100%
 *      1: IR output is IRTX modulated signal --> With RC5/RC6/NEC IRTXMT carrier duty cycle setting
 *
 *  Register transmission order
 *      0: IRTX_R0 first, IRTX_R11 last (R0, R1 ~ R11)
 *      1: IRTX_R11 first, IRTX_R0 last (R11, R10 ~R0)
 *
 *  Bit transmission order
 *      0: MSB first, LSB last (ex. R0[7], R0[6] ~ R0[0])
 *      1: LSB first, MSB last (ex. R0[0], R0[1] ~ R0[7])
 */
ENUM_HAL_RET_T halIrTxInit(UINT8 ucOutputSel, UINT8 ucRegOrder, UINT8 ucBitOrder)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    P_IOT_IRTX_TypeDef pIRTXTypeDef = (P_IOT_IRTX_TypeDef)(CM4_IRDA_TX_BASE + 0x00);
    UINT32  u4Val = 0;

    u4Val = pIRTXTypeDef->IRTXCFG;

    // IR output select
    // 0: IR output is IRTX baseband signal  --> duty cycle as 100%
    // 1: IR output is IRTX modulated signal --> With RC5/RC6/NEC default carrier duty cycle
    if (0 == ucOutputSel) {
        u4Val = u4Val & (~BIT(IRTX_IROS_OFFSET));
    } else {
        u4Val = u4Val | (BIT(IRTX_IROS_OFFSET));
    }

    // Register transmission order
    // 0: IRTX_R0 first, IRTX_R11 last (R0, R1 ~ R11)
    // 1: IRTX_R11 first, IRTX_R0 last (R11, R10 ~R0)
    if (0 == ucRegOrder) {
        u4Val = u4Val & (~BIT(IRTX_RODR_OFFSET));
    } else {
        u4Val = u4Val | (BIT(IRTX_RODR_OFFSET));
    }

    // Bit transmission order
    // 0: MSB first, LSB last (ex. R0[7], R0[6] ~ R0[0])
    // 1: LSB first, MSB last (ex. R0[0], R0[1] ~ R0[7])
    if (0 == ucBitOrder) {
        u4Val = u4Val & (~BIT(IRTX_BODR_OFFSET));
    } else {
        u4Val = u4Val | (BIT(IRTX_BODR_OFFSET));
    }

    pIRTXTypeDef->IRTXCFG = u4Val;

    return ret;
}

/*
 *  ucDataInvert
 *      value 0
 *          logic 0: active  --> idle
 *          logic 1: idle    --> active
 *
 *  ucOutputInvert
 *      value 0
 *          idle:    low
 *          active:  high
 *
 *  u4L0High, u4L0Low, u4L1High, u4L1Low, u2Freq, ucDutyCycle
 *      0 to apply protocol default value.
 *
 *  u2Freq
 *      unit is KHz
 *
 *  ucDutyCycle: 0 ~ 100
 */
ENUM_HAL_RET_T halIrTxConfig(ENUM_IR_PROTOCOL eIrProtocol, UINT8 ucDataInvert, UINT8 ucOutputInvert,
                             UINT32 u4L0High, UINT32 u4L0Low, UINT32 u4L1High, UINT32 u4L1Low,
                             UINT16 u2Freq, UINT8 ucDutyCycle)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    P_IOT_IRTX_TypeDef pIRTXTypeDef = (P_IOT_IRTX_TypeDef)(CM4_IRDA_TX_BASE + 0x00);
    UINT32  u4Val = 0;
    UINT16  u2CarrierPeriod = 0;
    UINT16  u2DutyTime = 0;
    UINT32  u4Period = 0;
    UINT32  u4Logic0HighPeriod = 0;
    UINT32  u4Logic0LowPeriod = 0;
    UINT32  u4Logic1HighPeriod = 0;
    UINT32  u4Logic1LowPeriod = 0;

    if (eIrProtocol >= IR_PROTOCOL_NUM) {
        return HAL_RET_FAIL;
    }

    u4Val = pIRTXTypeDef->IRTXCFG;

    // IR N inverter
    // value 0
    // logic 0: active  --> idle
    // logic 1: idle    --> active
    if (0 == ucDataInvert) {
        u4Val = u4Val & (~BIT(IRTX_DATA_INV_OFFSET));
    } else {
        u4Val = u4Val | (BIT(IRTX_DATA_INV_OFFSET));
    }

    // IR inverter
    // value 0
    // idle:    low
    // active:  high
    if (0 == ucOutputInvert) {
        u4Val = u4Val & (~BIT(IRTX_IRINV_OFFSET));
    } else {
        u4Val = u4Val | (BIT(IRTX_IRINV_OFFSET));
    }

    // IR output protocol
    u4Val = u4Val & (~IRTX_MODE_MASK);
    u4Val = u4Val | (eIrProtocol << IRTX_MODE_OFFSET);

    pIRTXTypeDef->IRTXCFG = u4Val;


    // Logic 0/1 High/Low period
    // Carrier period and duty cycle
    if (IR_NEC == eIrProtocol) {
        // 38K
        if (0 == u2Freq) {
            u2Freq = 38;
        }
        u2CarrierPeriod = (IRTX_OPER_FREQ / u2Freq);

        // recommended carrier duty-cycle is 1/4 or 1/3.
        if (0 == ucDutyCycle) {
            // set 25% as default
            ucDutyCycle = 25;
        }

        // Logical '0' \A1V a 562.5us pulse burst followed by a 562.5us space, with a total transmit time of 1.125ms
        // Logical '1' \A1V a 562.5us pulse burst followed by a 1.6875ms space, with a total transmit time of 2.25ms
        u4Period = (((1125 * IRTX_OPER_FREQ) / 1000) >> 1);
        u4Logic0HighPeriod  = u4Period;
        u4Logic0LowPeriod   = u4Period;
        u4Logic1HighPeriod  = u4Period;
        u4Logic1LowPeriod   = (((3375 * IRTX_OPER_FREQ) / 1000) >> 1);

    } else if (IR_RC5 == eIrProtocol) {
        // 36K
        if (0 == u2Freq) {
            u2Freq = 36;
        }
        u2CarrierPeriod = (IRTX_OPER_FREQ / u2Freq);

        if (0 == ucDutyCycle) {
            // set 25% as default
            ucDutyCycle = 25;
        }

        // Logical '0' \A1V an 889us pulse burst followed by an 889us space, with a total transmit time of 1.778ms
        // Logical '1' \A1V an 889us space followed by an 889us pulse burst, with a total transmit time of 1.778ms
        u4Period = (889 * IRTX_OPER_FREQ) / 1000;
        u4Logic0HighPeriod = u4Period;
        u4Logic0LowPeriod = u4Period;
        u4Logic1HighPeriod = u4Period;
        u4Logic1LowPeriod = u4Period;
    } else if (IR_RC6 == eIrProtocol) {
        // 36K
        if (0 == u2Freq) {
            u2Freq = 36;
        }
        u2CarrierPeriod = (IRTX_OPER_FREQ / u2Freq);

        // duty cycle of this carrier has to be between 25% and 50%.
        if (0 == ucDutyCycle) {
            // set 25% as default
            ucDutyCycle = 25;
        }

        // Logical '0' \A1V an 444us pulse burst followed by an 444us space, with a total transmit time of 888ms
        // Logical '1' \A1V an 444us space followed by an 444us pulse burst, with a total transmit time of 888ms
        u4Period = (444 * IRTX_OPER_FREQ) / 1000;
        u4Logic0HighPeriod = u4Period;
        u4Logic0LowPeriod = u4Period;
        u4Logic1HighPeriod = u4Period;
        u4Logic1LowPeriod = u4Period;
    } else if (IR_SWM == eIrProtocol) {
        // 36K
        if (0 == u2Freq) {
            u2Freq = 36;
        }
        u2CarrierPeriod = (IRTX_OPER_FREQ / u2Freq);

        // duty cycle of this carrier has to be between 25% and 50%.
        if (0 == ucDutyCycle) {
            // set 25% as default
            ucDutyCycle = 25;
        }

        // Logical '0' \A1V an 444us pulse burst followed by an 444us space, with a total transmit time of 888ms
        // Logical '1' \A1V an 444us space followed by an 444us pulse burst, with a total transmit time of 888ms
        u4Period = (444 * IRTX_OPER_FREQ) / 1000;
        u4Logic0HighPeriod = u4Period;
        u4Logic0LowPeriod = u4Period;
        u4Logic1HighPeriod = u4Period;
        u4Logic1LowPeriod = u4Period;
    } else if (IR_SW_PULSE_WIDTH == eIrProtocol) {
        // Default use RC5 setting
        // 36K
        if (0 == u2Freq) {
            u2Freq = 36;
        }
        u2CarrierPeriod = (IRTX_OPER_FREQ / u2Freq);

        if (0 == ucDutyCycle) {
            // set 25% as default
            ucDutyCycle = 25;
        }

        // Logical '0' \A1V an 889us pulse burst followed by an 889us space, with a total transmit time of 1.778ms
        // Logical '1' \A1V an 889us space followed by an 889us pulse burst, with a total transmit time of 1.778ms
        u4Period = (889 * IRTX_OPER_FREQ) / 1000;
        u4Logic0HighPeriod = u4Period;
        u4Logic0LowPeriod = u4Period;
        u4Logic1HighPeriod = u4Period;
        u4Logic1LowPeriod = u4Period;

    }

    pIRTXTypeDef->IRTX_L0H = u4Logic0HighPeriod;
    pIRTXTypeDef->IRTX_L0L = u4Logic0LowPeriod;
    pIRTXTypeDef->IRTX_L1H = u4Logic1HighPeriod;
    pIRTXTypeDef->IRTX_L1L = u4Logic1LowPeriod;

    u2DutyTime = ((ucDutyCycle * IRTX_OPER_FREQ) / (u2Freq * 100));
    u2DutyTime = u2DutyTime + halIrTxRound((ucDutyCycle * IRTX_OPER_FREQ), (u2Freq * 100), u2DutyTime);
    pIRTXTypeDef->IRTXMT = (u2DutyTime << IRTX_CDT_OFFSET) | u2CarrierPeriod;

    return ret;
}

/*
 *  ucRepeat: ONLY for NEC
 *      NEC: 1 to send Repeat Code
 *
 */

ENUM_HAL_RET_T halIrTxSend(UINT32 u4TxData0, UINT32 u4TxData1, UINT32 u4TxData2, UINT8 ucBitNum, UINT8 ucRepeat)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    P_IOT_IRTX_TypeDef pIRTXTypeDef = (P_IOT_IRTX_TypeDef)(CM4_IRDA_TX_BASE + 0x00);
    ENUM_IR_PROTOCOL eIrProtocol = IR_PROTOCOL_NUM;
    UINT32 u4Val = 0;

    // Sanity check
    if (ucBitNum & 0x80) {
        // 14:8        IRTX_BITNUM Bit Number
        // ONLY 7 bits
        return HAL_RET_FAIL;
    }

    // IR output protocol
    eIrProtocol = (ENUM_IR_PROTOCOL)(((pIRTXTypeDef->IRTXCFG) & (IRTX_MODE_MASK)) >> IRTX_MODE_OFFSET);

    if (IR_NEC == eIrProtocol) {
        if (1 == ucRepeat) {
            // Repeat Code
            ucBitNum = 1;
            u4TxData0 = 0x0;

            // Leading High
            (pIRTXTypeDef->IRTXSYNCH) = ((9000 * IRTX_OPER_FREQ) / 1000);
            // Leading Low
            (pIRTXTypeDef->IRTXSYNCL) = ((2250 * IRTX_OPER_FREQ) / 1000);
        } else {
            // Leading High
            (pIRTXTypeDef->IRTXSYNCH) = ((9000 * IRTX_OPER_FREQ) / 1000);
            // Leading Low
            (pIRTXTypeDef->IRTXSYNCL) = ((4500 * IRTX_OPER_FREQ) / 1000);
        }

    }

    //printf("u4TxData0=0x%08X\n", u4TxData0);
    //printf("u4TxData1=0x%08X\n", u4TxData1);
    //printf("u4TxData2=0x%08X\n", u4TxData2);
    //printf("ucBitNum=0x%02X\n", ucBitNum);

    // Tx Data
    pIRTXTypeDef->IRTXD0 = u4TxData0;
    pIRTXTypeDef->IRTXD1 = u4TxData1;
    pIRTXTypeDef->IRTXD2 = u4TxData2;

    // Tx Bit number
    u4Val = pIRTXTypeDef->IRTXCFG;
    u4Val = u4Val & (~IRTX_BITNUM_MASK);
    u4Val = u4Val | (ucBitNum << IRTX_BITNUM_OFFSET);
    pIRTXTypeDef->IRTXCFG = u4Val;

    // Trigger Tx
    (pIRTXTypeDef->IRTXCFG) = (pIRTXTypeDef->IRTXCFG) & (~BIT(IRTX_STRT_OFFSET));
    (pIRTXTypeDef->IRTXCFG) = (pIRTXTypeDef->IRTXCFG) | BIT(IRTX_STRT_OFFSET);

    return ret;
}


/*
 *  u2Freq
 *      unit is KHz
 *
 *  ucDutyCycle: 0 ~ 100
 *
 *  ucBasePeriod
 *      Unit: 0.5 us (2 MHz operating clock)
 */
ENUM_HAL_RET_T halIrTxPWSend(UINT8 ucDataNum, PUINT8 pData, UINT8 ucBasePeriod)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    P_IOT_IRTX_TypeDef pIRTXTypeDef = (P_IOT_IRTX_TypeDef)(CM4_IRDA_TX_BASE + 0x00);
    UINT8   ucIdx = 0;
    UINT32  u4DataValue = 0;
    UINT8   ucResidual = 0;
    PUINT32 pu4PWAddr = 0;
    UINT8   ucIrTxPWTempIdx = 0;

    // Sanity check
    if (ucDataNum & 0x80) {
        // 14:8        IRTX_BITNUM Bit Number
        // ONLY 7 bits
        return HAL_RET_FAIL;
    }
    // Set IRTXCFG	W	IRTX_BASE + 0x00	[31:0]	IRTXCFG	USER_DEFINED	Set IRTX configuration
    (pIRTXTypeDef->IRTXCFG) &= (~IRTX_BITNUM_MASK);
    (pIRTXTypeDef->IRTXCFG) |= (ucDataNum << IRTX_BITNUM_OFFSET);

    // Set IRTXMT	W	IRTX_BASE + 0x28	[31:0]	IRTXMT	USER_DEFINED	Set the period and duty cycle of the carrier
    // Set in halIrTxConfig


    // Set IRTX_SWM_BP	W	IRTX_BASE + 0x30	[7:0]	IRTXD0	USER_DEFINED	Set the base period to be multiplied with PW
    (pIRTXTypeDef->IRTX_SWM_BP) = ucBasePeriod;

    // Set IRTX_SWM_PW0~N	W	IRTX_BASE + 0x34 + 4N	[31:0]	IRTX_SWM_PW0~N	USER_DEFINED	Set the data to be transferred
    for ((ucResidual = ucDataNum), (ucIdx = 0), (pu4PWAddr = (PUINT32)(&(pIRTXTypeDef->IRTX_SWM_PW0))); ucResidual > 0;) {
        u4DataValue = 0;

        if (ucResidual > 4) {
            u4DataValue = ((pData[ucIdx] << 0) | (pData[ucIdx + 1] << 8) | (pData[ucIdx + 2] << 16) | (pData[ucIdx + 3] << 24));
            ucIdx = ucIdx + 4;
            ucResidual = ucResidual - 4;
        } else {
            do {
                //u4DataValue = (u4DataValue | (pData[ucIdx] << ((4 - ucResidual) * 8)));
                u4DataValue = (u4DataValue | (pData[ucIdx] << (ucIrTxPWTempIdx * 8)));
                ucIrTxPWTempIdx++;
                ucIdx++;
                ucResidual--;
            } while (ucResidual);
        }
        HAL_REG_32(pu4PWAddr) = u4DataValue;
        pu4PWAddr = pu4PWAddr + 1;

    }

    // Start IRTX	W	IRTX_BASE + 0x00	[0]	IRTXCFG	1'b1	Enable IRTX module
    (pIRTXTypeDef->IRTXCFG) = (pIRTXTypeDef->IRTXCFG) & (~BIT(IRTX_STRT_OFFSET));
    (pIRTXTypeDef->IRTXCFG) = (pIRTXTypeDef->IRTXCFG) | BIT(IRTX_STRT_OFFSET);

    return ret;
}

VOID halIrTxClearInterrupt()
{
    P_IOT_IRTX_TypeDef pIRTXTypeDef = (P_IOT_IRTX_TypeDef)(CM4_IRDA_TX_BASE + 0x00);
    (pIRTXTypeDef->IRTX_INT_CLR) |=  BIT(IRTX_INT_CLR_OFFSET);
}

/*
VOID halIrTxSWMSet(UINT8 ucEnable, UINT8 ucVal)
{
    P_IOT_IRTX_TypeDef pIRTXTypeDef = (P_IOT_IRTX_TypeDef)(CM4_IRDA_TX_BASE + 0x00);

    if (0 == ucVal) {
        (pIRTXTypeDef->IRTXCFG) = (pIRTXTypeDef->IRTXCFG) & (~BIT(IRTX_SWO_OFFSET));
    } else {
        (pIRTXTypeDef->IRTXCFG) = (pIRTXTypeDef->IRTXCFG) | (BIT(IRTX_SWO_OFFSET));
    }

    // Trigger Tx
    if (0 == ucEnable) {
        (pIRTXTypeDef->IRTXCFG) = (pIRTXTypeDef->IRTXCFG) & (~BIT(IRTX_STRT_OFFSET));
    } else {
        (pIRTXTypeDef->IRTXCFG) = (pIRTXTypeDef->IRTXCFG) | BIT(IRTX_STRT_OFFSET);
    }
}
*/
#endif

