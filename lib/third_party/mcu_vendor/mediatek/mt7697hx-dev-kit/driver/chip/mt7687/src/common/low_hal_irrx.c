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
** $Log: low_hal_irrx.c $
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

#ifdef HAL_IRRX_MODULE_ENABLED
#include "hal_irrx.h"
#include <stdio.h>
#include <string.h>
#include "type_def.h"
#include "mt7687.h"
#include "low_hal_irrx.h"
#include "hal_nvic.h"

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
u2SamplePeriod
    12'h640	Set Sampling Period (800us for RC-5)
========================================================================
*/
static irrx_user_callback_t s_low_hal_irrx_rc5_callback;
static irrx_user_callback_t s_low_hal_irrx_pwd_callback;

VOID halIrRxClearInterrupt(void)
{
    P_IOT_IRRX_TypeDef pIRRXTypeDef = (P_IOT_IRRX_TypeDef)(CM4_IRDA_RX_BASE + 0x00);

    // Clear IRRX Interrupt
    // W   IRRX_BASE + 0x20    [0] IR_INTCLR   1'b1
    (pIRRXTypeDef->IR_INTCLR) |= BIT(IR_INTCLR_OFFSET);
}


static VOID halIrRxIsr(hal_nvic_irq_t irq_number)
{
    P_IOT_IRRX_TypeDef pIRRXTypeDef = (P_IOT_IRRX_TypeDef)(CM4_IRDA_RX_BASE + 0x00);

    halIrRxClearInterrupt();

    if (((pIRRXTypeDef->IRCFGH) & (BIT(IRCFGH_CHK_EN_OFFSET)))) {
        // Pulse width detection
        if (s_low_hal_irrx_pwd_callback.func) {
           s_low_hal_irrx_pwd_callback.func(s_low_hal_irrx_pwd_callback.argument);
        }
    } else {
        // RC5
        if (s_low_hal_irrx_rc5_callback.func) {
           s_low_hal_irrx_rc5_callback.func(s_low_hal_irrx_rc5_callback.argument);
        }
    }
}


ENUM_HAL_RET_T halIrRxRegisterCallback(low_hal_irrx_mode_t mode, low_hal_irrx_callback_t callback,void *parameter)
{
    ENUM_HAL_RET_T  ret = HAL_RET_SUCCESS;

    if (LOW_HAL_IRRX_MODE_RC5 == mode) {
        s_low_hal_irrx_rc5_callback.func =callback;
		  s_low_hal_irrx_rc5_callback.argument = parameter;	
    } else if (LOW_HAL_IRRX_MODE_PWD == mode) {
          s_low_hal_irrx_pwd_callback.func =callback;
		  s_low_hal_irrx_pwd_callback.argument = parameter;
    }

    hal_nvic_register_isr_handler(CM4_IRDA_RX_IRQ, halIrRxIsr);
    NVIC_SetPriority((IRQn_Type)CM4_IRDA_RX_IRQ, CM4_IRDA_RX_PRI);
    NVIC_EnableIRQ((IRQn_Type)CM4_IRDA_RX_IRQ);

    return ret;
}


VOID halIrRxRC5Read(PUINT8 pucBitNum, PUINT32 pu4DW0, PUINT32 pu4DW1)
{
    UINT8 ucBitNum = 0;
    UINT8 format = 0;
    P_IOT_IRRX_TypeDef pIRRXTypeDef = (P_IOT_IRRX_TypeDef)(CM4_IRDA_RX_BASE + 0x00);
    ucBitNum = (((pIRRXTypeDef->IRH) & (IRH_BIT_CNT_MASK)) >> (IRH_BIT_CNT_OFFSET));
    format = ((pIRRXTypeDef->IRCFGH) & (IRCFGH_ORDINV_MASK));
    if (ucBitNum > 0) {
        (*pucBitNum) = (ucBitNum - 1); // last terminated bit is non-valid
        (*pu4DW0) = (pIRRXTypeDef->IRM);
        (*pu4DW1) = (pIRRXTypeDef->IRL);
    } else {
        (*pucBitNum) = ucBitNum;
    }
    if (format == HAL_IRRX_RC5_RECEIVE_FORMAT_BIT_REVERSE){
       if (*pucBitNum < 32){
        (*pu4DW0) = (pIRRXTypeDef->IRM) & ~ (IRM_DATA_MASK << (*pucBitNum + 7));
        } else {
        (*pu4DW1) = (pIRRXTypeDef->IRL) & ~ (IRM_DATA_MASK << (*pucBitNum +7));
         }
      }
    halIrRxReset();
}


VOID halIrRxPWDRead(PUINT8 pucPWNum, PUINT8 pucPWBuf, UINT8 ucBufLen)
{
    P_IOT_IRRX_TypeDef pIRRXTypeDef = (P_IOT_IRRX_TypeDef)(CM4_IRDA_RX_BASE + 0x00);
    UINT8   ucNum = 0;
    UINT32  u4RegVal = 0;
    UINT32  u4RegAddr = (UINT32)(&(pIRRXTypeDef->CHK_DATA0));

    do {
        u4RegVal = HAL_REG_32(u4RegAddr);
        if ((0xFF != (u4RegVal & 0xFF)) && (0x00 != (u4RegVal & 0xFF))) {
            pucPWBuf[ucNum] = (u4RegVal & 0xFF);
            //printf("0x%02X\n", pucPWBuf[ucNum]);
            ucNum++;
        } else {
            break;
        }

        if ((0xFF != ((u4RegVal >> 8) & 0xFF)) && (0x00 != ((u4RegVal >> 8) & 0xFF))) {
            pucPWBuf[ucNum] = ((u4RegVal >> 8) & 0xFF);
            //printf("0x%02X\n", pucPWBuf[ucNum]);
            ucNum++;
        } else {
            break;
        }

        if ((0xFF != ((u4RegVal >> 16) & 0xFF)) && (0x00 != ((u4RegVal >> 16) & 0xFF))) {
            pucPWBuf[ucNum] = ((u4RegVal >> 16) & 0xFF);
            //printf("0x%02X\n", pucPWBuf[ucNum]);
            ucNum++;
        } else {
            break;
        }

        if ((0xFF != ((u4RegVal >> 24) & 0xFF)) && (0x00 != ((u4RegVal >> 24) & 0xFF))) {
            pucPWBuf[ucNum] = ((u4RegVal >> 24) & 0xFF);
            //printf("0x%02X\n", pucPWBuf[ucNum]);
            ucNum++;
        } else {
            break;
        }

        u4RegAddr = u4RegAddr + 4;
    } while (1);

    (*pucPWNum) = ucNum;

    halIrRxReset();
}


VOID halIrRxReset(void)
{
    P_IOT_IRRX_TypeDef pIRRXTypeDef = (P_IOT_IRRX_TypeDef)(CM4_IRDA_RX_BASE + 0x00);

    // Reset IRRX (All the states are cleared)
    // W   IRRX_BASE + 0x18    [0] IRCLR   1'b1
    (pIRRXTypeDef->IRCLR) |= BIT(IRCLR_OFFSET);
}

ENUM_HAL_RET_T halIrRxRC5(UINT8 ucInverse, UINT8 ucBitReverse, UINT8 ucIgnoreEmpty, UINT16 u2SamplePeriod)
{
    // IR RX Sequence for RC-5
    P_IOT_IRRX_TypeDef pIRRXTypeDef = (P_IOT_IRRX_TypeDef)(CM4_IRDA_RX_BASE + 0x00);
    UINT32 u4Val = 0;
    UINT32 u4DeGlitchCnt = 0x1F;
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;

    // Set IRCFGH  W   IRRX_BASE + 0x0C
    u4Val = (pIRRXTypeDef->IRCFGH);

    // [0] IREN    1'b1    Enable IRRX module
    u4Val |= BIT(IRCFGH_IREN_OFFSET);

    // [1] IRINV   1'b0    Disable IR inverse
    if (0 == ucInverse) {
        u4Val &= (~BIT(IRCFGH_IRINV_OFFSET));
    } else {
        u4Val |= (BIT(IRCFGH_IRINV_OFFSET));
    }

    // [4] ORDINV  USER_DEFINED    Set the order within each byte
    // 0: Keep the order of decoded bit
    // 1: Inverse the order of decoded bit
    if (0 == ucBitReverse) {
        u4Val &= (~BIT(IRCFGH_ORDINV_OFFSET));
    } else {
        u4Val |= (BIT(IRCFGH_ORDINV_OFFSET));
    }

    // [7:6]   DISL/DISH   2'b00   Disable DISL/DISH
    u4Val &= (~BIT(IRCFGH_DISL_OFFSET));
    u4Val &= (~BIT(IRCFGH_DISH_OFFSET));

    // [13]    CHK_EN  1'b0    Disable Pulse-Width Detection
    u4Val &= (~BIT(IRCFGH_CHK_EN_OFFSET));

    // [14]    IGB0    1'b0    Disable ignore bit 0
    if (0 == ucIgnoreEmpty) {
        u4Val &= (~BIT(IRCFGH_IGB0_OFFSET));
    } else {
        u4Val |= (BIT(IRCFGH_IGB0_OFFSET));
    }

    // [23:16] OK_PERIOD   7'd3    Set END pattern length to 3 for RC-5
    u4Val &= (~IRCFGH_OK_PERIOD_MASK);
    u4Val |= (IRRX_RC5_DEFAULT_OK_PERIOD << IRCFGH_OK_PERIOD_OFFSET);

    (pIRRXTypeDef->IRCFGH) = u4Val;


    // Set IRCFGL	W	IRRX_BASE + 0x10
    // [11:0]	SA_PERIOD	12'h640	Set Sampling Period (800us for RC-5)
    (pIRRXTypeDef->IRCFGL) &= (~IRCFGL_SAPERIOD_MASK);
    (pIRRXTypeDef->IRCFGL) |= (u2SamplePeriod << IRCFGL_SAPERIOD_OFFSET);

    // Set IRTHD	W	IRRX_BASE + 0x14
    // [7]	INTCLR_IRCLR	USER_DEFINED	Whether to clear IRRX along with interrupt
    (pIRRXTypeDef->IRTHD) &= (~BIT(IRTHD_INTCLR_IRCLR_OFFSET));

    // [12:8]	DG_SEL	USER_DEFINED	Set the de-glitch counter
    (pIRRXTypeDef->IRTHD) &= (~IRTHD_DG_SEL_MASK);
    (pIRRXTypeDef->IRTHD) |= (u4DeGlitchCnt << IRTHD_DG_SEL_OFFSET);


    // Reset IRRX	W	IRRX_BASE + 0x18
    // [0]	IRCLR	1'b1	Reset IRRX
    (pIRRXTypeDef->IRCLR) |= BIT(IRCLR_OFFSET);

    return ret;
}


/*
    (u4TherminateThresholdUs / u4DetectPrecesionUs) <= 0xFF (8bit)
*/

ENUM_HAL_RET_T halIrRxPulseWidthDetect(
    UINT32  u4DeGlitchCnt,
    UINT8   ucInverse,
    UINT32  u4TherminateThresholdUs,
    UINT32  *pu4DetectPrecesionUs
)
{
    // IR RX Sequence for Pulse-Width Detection Mode
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    P_IOT_IRRX_TypeDef pIRRXTypeDef = (P_IOT_IRRX_TypeDef)(CM4_IRDA_RX_BASE + 0x00);
    UINT32  u4Val = 0;
    UINT32  u4OKPeriod = 0;
    UINT32  u4SAPeriod = 0;
    UINT32  u4ChkPeriod = 0;
//    UINT8   ucFixSAPeriod = 0;

    /*
        CR: OK_PERIOD, SA_PERIOD, CHK_PERIOD

        SA_us * OK_Period = threshold_us
        (SA_Period * OPER_time_uint_us) = threshold_us / OK_Period
        (SA_Period * (1,000,000 / OPER_Freq_Hz)) = threshold_us / OK_Period
        SA_Period = (threshold_us / OK_Period) * (OPER_Freq_KHz / 1,000)
        --> SA_Period = (threshold_us * OPER_Freq_KHz) / (OK_Period * 1,000)
        (OK_Period * 1,000) = (threshold_us * OPER_Freq_KHz) / SA_Period
        --> OK_Period = (threshold_us * OPER_Freq_KHz) / (SA_Period * 1000)
    */
    /*
        DectectPrecision_us = (CHK_Period * BUS_Period)
        DectectPrecision_us = (CHK_Period * (1,000 / BUS_FREQ_KHZ))
        CHK_Period = DectectPrecision_us * (BUS_FREQ_KHZ / 1000);

    */
    /*
        threshold_us = DectectPrecision_us * 255 // 8bits=255
        threshold_us = (CHK_Period * BUS_Period) * 255
        OK_Period = (threshold_us * OPER_Freq_KHz) / (SA_Period * 1000)
        --> OK_Period = (((CHK_Period * BUS_Period) * 255) * OPER_Freq_KHz) / (SA_Period * 1000)
        (((CHK_Period * BUS_Period) * 255) * OPER_Freq_KHz) = (OK_Period * (SA_Period * 1000))
        (CHK_Period * BUS_Period) = (OK_Period * (SA_Period * 1000)) / (255 * OPER_Freq_KHz)
        CHK_Period = (OK_Period * (SA_Period * 1000)) / ((255 * OPER_Freq_KHz) * BUS_Period_us)
        CHK_Period = (OK_Period * (SA_Period * 1000)) / ((255 * OPER_Freq_KHz) * (1,000 / BUS_FREQ_KHz))
        CHK_Period = (OK_Period * (SA_Period * 1000) * BUS_Period_KHz) / ((255 * OPER_Freq_KHz) * 1,000 )
        --> CHK_Period = (OK_Period * (SA_Period ) * BUS_FREQ_KHz) / ((255 * OPER_Freq_KHz))

    */

    u4OKPeriod = IRRX_PWD_OK_PERIOD;
    do {
        u4SAPeriod = ((u4TherminateThresholdUs * IRRX_OPER_FREQ) / (u4OKPeriod * 1000));
        if (0 != (u4SAPeriod & 0xFFFFF000)) {
            // Saturate
            // 11  0   SAPERIOD    RW  12'hfff
            printf("[Saturate]u4SAPeriod = 0x%08X\n", u4SAPeriod);
            u4OKPeriod = (u4OKPeriod << 1);
            continue;
        }

        u4ChkPeriod = (u4OKPeriod * u4SAPeriod * IRRX_BUS_FREQ) / (0xFF * IRRX_OPER_FREQ);
        if (0 != (u4ChkPeriod & 0xFFFF0000)) {
            // Saturate,
            //31  16  CHK_PERIOD  RW  16'hffff
            printf("[Saturate]u4ChkPeriod = 0x%08X\n", u4ChkPeriod);
            return HAL_RET_FAIL;

        } else {
            break;
        }
    } while (1);

#if 0 // debug 
    {
        UINT32  u4CalTherminateThresholdUs = 0;
        UINT32  u4CalPrecesionUs = 0;

        // threshold_us = (SA_Period * (1,000 / OPER_Freq_KHz)) * OK_Period
        u4CalTherminateThresholdUs = (((u4SAPeriod * u4OKPeriod) * 1000) / IRRX_OPER_FREQ);
        u4CalPrecesionUs = ((u4ChkPeriod * 1000) / IRRX_BUS_FREQ);

        printf("u4SAPeriod = 0x%08X, u4ChkPeriod = 0x%08X, u4OKPeriod = 0x%08X\n", u4SAPeriod, u4ChkPeriod, u4OKPeriod);
        printf("u4CalTherminateThresholdUs = %u, u4CalPrecesionUs = %u\n", u4CalTherminateThresholdUs, u4CalPrecesionUs);

    }
#endif //#if 1 // debug 

    // Set IRCFGH  W   IRRX_BASE + 0x0C
    u4Val = (pIRRXTypeDef->IRCFGH);

    // [0] IREN    1'b1    Enable IRRX module
    u4Val |= BIT(IRCFGH_IREN_OFFSET);

    // [1] IRINV   1'b0    Disable IR inverse
    if (0 == ucInverse) {
        u4Val &= (~BIT(IRCFGH_IRINV_OFFSET));
    } else {
        u4Val |= (BIT(IRCFGH_IRINV_OFFSET));
    }


    // [7:6]   DISL/DISH   2'b00   Disable DISL/DISH
    u4Val &= (~BIT(IRCFGH_DISL_OFFSET));
    u4Val &= (~BIT(IRCFGH_DISH_OFFSET));

    // [13]    CHK_EN  1'b1    Enable Pulse-Width Detection
    u4Val |= (BIT(IRCFGH_CHK_EN_OFFSET));

    // [14]    IGB0    1'b0    Disable ignore bit 0
    u4Val &= (~BIT(IRCFGH_IGB0_OFFSET));

    // [23:16] OK_PERIOD   USER_DEFINED    Set END pattern length
    u4Val &= (~IRCFGH_OK_PERIOD_MASK);
    u4Val |= (u4OKPeriod << IRCFGH_OK_PERIOD_OFFSET);

    (pIRRXTypeDef->IRCFGH) = u4Val;

    // Set IRCFGL	W	IRRX_BASE + 0x10
    // [11:0]  SA_PERIOD   USER_DEFINED    Set Sampling Period
    // [11:0]	SA_PERIOD	12'h640	Set Sampling Period (800us for RC-5)
    (pIRRXTypeDef->IRCFGL) &= (~IRCFGL_SAPERIOD_MASK);
    (pIRRXTypeDef->IRCFGL) |= (u4SAPeriod << IRCFGL_SAPERIOD_OFFSET);

    (pIRRXTypeDef->IRCFGL) &= (~IRCFGL_CHK_PERIOD_MASK);
    (pIRRXTypeDef->IRCFGL) |= (u4ChkPeriod << IRCFGL_CHK_PERIOD_OFFSET);


    // Set IRTHD	W	IRRX_BASE + 0x14
    // [7]	INTCLR_IRCLR	USER_DEFINED	Whether to clear IRRX along with interrupt
    (pIRRXTypeDef->IRTHD) &= (~BIT(IRTHD_INTCLR_IRCLR_OFFSET));

    // [12:8]	DG_SEL	USER_DEFINED	Set the de-glitch counter
    (pIRRXTypeDef->IRTHD) &= (~IRTHD_DG_SEL_MASK);
    (pIRRXTypeDef->IRTHD) |= (u4DeGlitchCnt << IRTHD_DG_SEL_OFFSET);


    // Reset IRRX	W	IRRX_BASE + 0x18
    // [0]	IRCLR	1'b1	Reset IRRX
    (pIRRXTypeDef->IRCLR) |= BIT(IRCLR_OFFSET);

    // DectectPrecision_us = (CHK_Period * BUS_Period)
    (*pu4DetectPrecesionUs) = ((u4ChkPeriod * 1000) / IRRX_BUS_FREQ);

    return ret;
}

/*
    (u4TherminateThresholdUs / u4DetectPrecesionUs) <= 0xFF (8bit)
*/

ENUM_HAL_RET_T halIrRxPulseWidthDetect_DVT(
    UINT32  u4DeGlitchCnt,
    UINT8   ucInverse,
    UINT32  u4OKPeriod,
    UINT32  u4SAPeriod,
    UINT32  u4ChkPeriod,
    UINT8   ucLowTerminate,
    UINT8   ucHighTerminate
)
{
    // IR RX Sequence for Pulse-Width Detection Mode
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    P_IOT_IRRX_TypeDef pIRRXTypeDef = (P_IOT_IRRX_TypeDef)(CM4_IRDA_RX_BASE + 0x00);
    UINT32  u4Val = 0;

#if 1 // debug 
    {
        UINT32  u4CalTherminateThresholdUs = 0;
        UINT32  u4CalPrecesionUs = 0;

        // threshold_us = (SA_Period * (1,000 / OPER_Freq_KHz)) * OK_Period
        u4CalTherminateThresholdUs = (((u4SAPeriod * u4OKPeriod) * 1000) / IRRX_OPER_FREQ);
        u4CalPrecesionUs = ((u4ChkPeriod * 1000) / IRRX_BUS_FREQ);

        printf("u4SAPeriod = 0x%08X, u4ChkPeriod = 0x%08X, u4OKPeriod = 0x%08X\n", u4SAPeriod, u4ChkPeriod, u4OKPeriod);
        printf("u4CalTherminateThresholdUs = %u, u4CalPrecesionUs = %u\n", u4CalTherminateThresholdUs, u4CalPrecesionUs);

    }
#endif //#if 1 // debug 


    // Set IRCFGH  W   IRRX_BASE + 0x0C
    u4Val = (pIRRXTypeDef->IRCFGH);

    // [0] IREN    1'b1    Enable IRRX module
    u4Val |= BIT(IRCFGH_IREN_OFFSET);

    // [1] IRINV   1'b0    Disable IR inverse
    if (0 == ucInverse) {
        u4Val &= (~BIT(IRCFGH_IRINV_OFFSET));
    } else {
        u4Val |= (BIT(IRCFGH_IRINV_OFFSET));
    }

    // [7:6]   DISL/DISH   2'b00   Disable DISL/DISH
    // [7:6]   DISL/DISH   2'b00   Disable DISL/DISH
    if (0 == ucLowTerminate) {
        u4Val |= (BIT(IRCFGH_DISL_OFFSET));
    } else {
        u4Val &= (~BIT(IRCFGH_DISL_OFFSET));
    }

    if (0 == ucHighTerminate) {
        u4Val |= (BIT(IRCFGH_DISH_OFFSET));
    } else {
        u4Val &= (~BIT(IRCFGH_DISH_OFFSET));
    }

    // [13]    CHK_EN  1'b1    Enable Pulse-Width Detection
    u4Val |= (BIT(IRCFGH_CHK_EN_OFFSET));

    // [14]    IGB0    1'b0    Disable ignore bit 0
    u4Val &= (~BIT(IRCFGH_IGB0_OFFSET));

    // [23:16] OK_PERIOD   USER_DEFINED    Set END pattern length
    u4Val &= (~IRCFGH_OK_PERIOD_MASK);
    u4Val |= (u4OKPeriod << IRCFGH_OK_PERIOD_OFFSET);

    (pIRRXTypeDef->IRCFGH) = u4Val;

    // Set IRCFGL	W	IRRX_BASE + 0x10
    // [11:0]  SA_PERIOD   USER_DEFINED    Set Sampling Period
    // [11:0]	SA_PERIOD	12'h640	Set Sampling Period (800us for RC-5)
    (pIRRXTypeDef->IRCFGL) &= (~IRCFGL_SAPERIOD_MASK);
    (pIRRXTypeDef->IRCFGL) |= (u4SAPeriod << IRCFGL_SAPERIOD_OFFSET);

    (pIRRXTypeDef->IRCFGL) &= (~IRCFGL_CHK_PERIOD_MASK);
    (pIRRXTypeDef->IRCFGL) |= (u4ChkPeriod << IRCFGL_CHK_PERIOD_OFFSET);


    // Set IRTHD	W	IRRX_BASE + 0x14
    // [7]	INTCLR_IRCLR	USER_DEFINED	Whether to clear IRRX along with interrupt
    (pIRRXTypeDef->IRTHD) &= (~BIT(IRTHD_INTCLR_IRCLR_OFFSET));

    // [12:8]	DG_SEL	USER_DEFINED	Set the de-glitch counter
    (pIRRXTypeDef->IRTHD) &= (~IRTHD_DG_SEL_MASK);
    (pIRRXTypeDef->IRTHD) |= (u4DeGlitchCnt << IRTHD_DG_SEL_OFFSET);


    // Reset IRRX	W	IRRX_BASE + 0x18
    // [0]	IRCLR	1'b1	Reset IRRX
    (pIRRXTypeDef->IRCLR) |= BIT(IRCLR_OFFSET);

    return ret;
}

#endif

