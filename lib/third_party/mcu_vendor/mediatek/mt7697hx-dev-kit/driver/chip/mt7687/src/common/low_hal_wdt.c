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
** $Log: low_hal_wdt.c $
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
#include "hal_wdt.h"
#if defined(HAL_WDT_MODULE_ENABLED)
#include "low_hal_wdt.h"
#include "cos_api.h"
#include "timer.h"
#include "mt7687.h"
#include "system_mt7687.h"
#include "hal_nvic.h"
#include "hal_efuse.h"

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

extern void delay_time(kal_uint32 count);


VOID halWDTCRWrite(volatile UINT32 *pu4CRAddr, UINT32 u4Value)
{
    HAL_REG_32(pu4CRAddr) = u4Value;
    //cos_delay_time(10);
    drvGPT2Init();
    delay_time(10);
}

UINT32 halWDTReadInterruptStatus(void)
{
    P_IOT_WDT_TypeDef pWDTTypeDef = (P_IOT_WDT_TypeDef)(CM4_WDT_BASE + IOT_WDT_MODE_OFFSET);

    return (pWDTTypeDef->WDT_STA);
}

VOID halWDTRestart(void)
{
    P_IOT_WDT_TypeDef pWDTTypeDef = NULL;

    pWDTTypeDef = (P_IOT_WDT_TypeDef)(CM4_WDT_BASE + IOT_WDT_MODE_OFFSET);

    //(pWDTTypeDef->WDT_RESTART) = CM4_WDT_RESTART_KEY;
    halWDTCRWrite(&(pWDTTypeDef->WDT_RESTART), CM4_WDT_RESTART_KEY);
}

//  Watchdog Timer time-out period is a multiple of 1024* T32k  =32ms*(TIMEOUT + 1) if T32K is ideal.
VOID halWDTSetTimeout(UINT16 u2Timeout)
{
    P_IOT_WDT_TypeDef pWDTTypeDef = NULL;
    UINT32 u4Val = 0;

    pWDTTypeDef = (P_IOT_WDT_TypeDef)(CM4_WDT_BASE + IOT_WDT_MODE_OFFSET);

    u4Val = (pWDTTypeDef->WDT_LENGTH);
    // Unlock
    u4Val &= (~WDT_LENGTH_KEY_MASK);
    u4Val |= (CM4_WDT_LENGTH_KEY << WDT_LENGTH_KEY_OFFSET);

    u4Val &= (~WDT_LENGTH_TIMEOUT_MASK);
    u4Val |= (u2Timeout << WDT_LENGTH_TIMEOUT_OFFSET);
    //(pWDTTypeDef->WDT_LENGTH) = u4Val;
    halWDTCRWrite(&(pWDTTypeDef->WDT_LENGTH), u4Val);
    halWDTRestart();
}

VOID WDT_LISR(hal_nvic_irq_t irq_number)
{
    NVIC_DisableIRQ((IRQn_Type)CM4_WDT_IRQ);

    /* set GPIO 1 = Low */
    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) &= ~(0xF << (1 * 4));
    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) |= (0x8 << (1 * 4));

    /* Output Enable */
    DRV_Reg32(IOT_GPIO_AON_BASE + 0x90) |= (1 << 1);

    /* Set output polarity */
    DRV_Reg32(IOT_GPIO_AON_BASE + 0x60) &= ~(1 << 1);

    while (1);
}
VOID halWDTConfig(UINT8 ucIRQ, UINT8 ucExtEn, UINT8 ucExtPolarity, UINT8 ucGlobalRest)
{

    P_IOT_WDT_TypeDef pWDTTypeDef = NULL;
    UINT32 u4Val = 0;
    uint8_t efuse_buffer;

    pWDTTypeDef = (P_IOT_WDT_TypeDef)(CM4_WDT_BASE + IOT_WDT_MODE_OFFSET);

    u4Val = (pWDTTypeDef->WDT_MODE);
    // Unlock
    u4Val &= (~WDT_MODE_KEY_MASK);
    u4Val |= (CM4_WDT_MODE_KEY << WDT_MODE_KEY_OFFSET);

    /*
        3   IRQ Issue an interrupt instead of a Watchdog Timer reset.  For debug purposes, RGU issues an interrupt to the MCU instead of resetting the system.
            0   Disable.
            1   Enable.
    */
    if (0 == ucIRQ) {
        u4Val &= (~BIT(WDT_MODE_IRQ_OFFSET));
    } else {
        u4Val |= (BIT(WDT_MODE_IRQ_OFFSET));
    }

    /* work-around for external flash pinmux issue (ROM) . Only WDT reset mode take this workaround*/
    if (0 == ucIRQ) {
        hal_efuse_read(0x107, &efuse_buffer, 1);
        if ((((DRV_Reg32(TOP_CFG_CM4_CM4_STRAP_STA) >> 9) & 0x1) == 0x00) || /* Non-SIP mode */
            (((efuse_buffer >> 7) & 0x1) == 0x1)) {   /* efuse 0x107[7]==1,means external flash */
            u4Val |= (BIT(WDT_MODE_IRQ_OFFSET));

            hal_nvic_register_isr_handler(CM4_WDT_IRQ, WDT_LISR);
            NVIC_SetPriority((IRQn_Type)CM4_WDT_IRQ, (uint32_t)CM4_DMA_PRI);
            NVIC_EnableIRQ((IRQn_Type)CM4_WDT_IRQ);
        }
    }

    /*
        2   EXTEN   Specifies whether or not to generate an external watchdog reset signal.
            0: The watchdog does not generate an external watchdog reset signal.
            1: If the watchdog counter reaches zero, an external watchdog signal is generated
    */
    if (0 == ucExtEn) {
        u4Val &= (~BIT(WDT_MODE_EXTEN_OFFSET));
    } else {
        u4Val |= (BIT(WDT_MODE_EXTEN_OFFSET));
    }
    /*
        1   EXTPOL  Defines the polarity of the external watchdog pin.
            0: Active low.
            1: Active high.
    */
    if (0 == ucExtPolarity) {
        u4Val &= (~BIT(WDT_MODE_EXTPOL_OFFSET));
    } else {
        u4Val |= (BIT(WDT_MODE_EXTPOL_OFFSET));
    }

    //(pWDTTypeDef->WDT_MODE) = u4Val;
    halWDTCRWrite(&(pWDTTypeDef->WDT_MODE), u4Val);


    if (1 == ucGlobalRest) {
        //CR 0x8300917C[16] = 1 : CM4 WDT whole chip mode, CM4 WDT reset whole chip included N9 domain
        HAL_REG_32(0x8300917C) = (HAL_REG_32(0x8300917C) | BIT(16));
    } else {
        HAL_REG_32(0x8300917C) = (HAL_REG_32(0x8300917C) & (~BIT(16)));
    }

}

VOID halWDTEnable(UINT8 ucEnable)
{
    P_IOT_WDT_TypeDef pWDTTypeDef = NULL;
    UINT32 u4Val = 0;

    pWDTTypeDef = (P_IOT_WDT_TypeDef)(CM4_WDT_BASE + IOT_WDT_MODE_OFFSET);

    u4Val = (pWDTTypeDef->WDT_MODE);
    // Unlock
    u4Val &= (~WDT_MODE_KEY_MASK);
    u4Val |= (CM4_WDT_MODE_KEY << WDT_MODE_KEY_OFFSET);


    /*
        0   ENABLE  Enables the Watchdog Timer. Default watchdog timer is disabled.
            0: Disables the Watchdog Timer.
            1: Enables the Watchdog Timer.
    */
    if (0 == ucEnable) {
        u4Val &= (~BIT(WDT_MODE_ENABLE_OFFSET));
    } else {
        u4Val |= (BIT(WDT_MODE_ENABLE_OFFSET));
    }
    //(pWDTTypeDef->WDT_MODE) = u4Val;
    halWDTCRWrite(&(pWDTTypeDef->WDT_MODE), u4Val);

}

VOID halWDTSoftwareReset(void)
{
    P_IOT_WDT_TypeDef pWDTTypeDef = NULL;
    uint8_t efuse_buffer;

    /* work-around for external flash pinmux issue (ROM) . Only WDT reset mode take this workaround*/
    hal_efuse_read(0x107, &efuse_buffer, 1);
    if ((((DRV_Reg32(TOP_CFG_CM4_CM4_STRAP_STA) >> 9) & 0x1) == 0x00) || /* Non-SIP mode */
        (((efuse_buffer >> 7) & 0x1) == 0x1)) {   /* efuse 0x107[7]==1,means external flash */
        WDT_LISR(CM4_WDT_IRQ);
    } else {
        pWDTTypeDef = (P_IOT_WDT_TypeDef)(CM4_WDT_BASE + IOT_WDT_MODE_OFFSET);
        halWDTCRWrite(&(pWDTTypeDef->WDT_SWRST), CM4_WDT_SWRST_KEY);
    }
}

/*
VOID halWDTDualCoreReset()
{
    HAL_REG_32(IOT_WDT_DUAL_RST_ADDR) |= BIT(WDT_DUAL_RST_RST_OFFSET);
}

VOID halWDTDualCoreInterrupt(UINT8 ucClear)
{
    if (0 == ucClear)
    {
        HAL_REG_32(IOT_WDT_DUAL_RST_ADDR) |= BIT(WDT_DUAL_RST_SW_INT_OFFSET);
    }
    else
    {
        HAL_REG_32(IOT_WDT_DUAL_RST_ADDR) |= BIT(WDT_DUAL_RST_SW_INT_CLR_OFFSET);
    }
}

*/

#endif

