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
** $Id: //MT7687 $
*/

/*! \file   "system_mt7687.h"
    \brief  This file provide utility functions for the driver

*/



#ifndef __SYSTEM_MT7687_H
#define __SYSTEM_MT7687_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#if defined(__GNUC__)
#define DECLSPEC_SELECTANY static
#define INLINE inline
#define STATIC_INLINE  static inline
#define ALWAYS_INLINE  __attribute__((always_inline)) inline
#elif defined(__CC_ARM)
#define DECLSPEC_SELECTANY  __declspec(selectany)
#define INLINE __forceinline
#define STATIC_INLINE  static __forceinline
#define ALWAYS_INLINE  __attribute__((always_inline)) inline
#else
#define DECLSPEC_SELECTANY static
#define INLINE inline
#define STATIC_INLINE  static inline
#define ALWAYS_INLINE  inline
#endif

#define TOP_CFG_AON_CM4             (CM4_CONFIG_BASE+0x8000)

#define TOP_CFG_CM4_CKG_EN0         (TOP_CFG_AON_CM4+0x01B0)
#define CM4_WBTAC_MCU_CK_SEL_SHIFT          14
#define CM4_WBTAC_MCU_CK_SEL_MASK           0x3
#define CM4_WBTAC_MCU_CK_SEL_XTAL           0
#define CM4_WBTAC_MCU_CK_SEL_WIFI_PLL_960   1
#define CM4_WBTAC_MCU_CK_SEL_WIFI_PLL_320   2
#define CM4_WBTAC_MCU_CK_SEL_BBP_PLL        3

#define CM4_MCU_DIV_SEL_SHIFT           4
#define CM4_MCU_DIV_SEL_MASK            0x3F

#define CM4_HCLK_SEL_SHIFT          0
#define CM4_HCLK_SEL_MASK           0x7
#define CM4_HCLK_SEL_OSC            0
#define CM4_HCLK_SEL_32K            1
#define CM4_HCLK_SEL_SYS_64M        2
#define CM4_HCLK_SEL_NA             3
#define CM4_HCLK_SEL_PLL            4

#define TOP_CFG_HCLK_2M_CKGEN           (TOP_CFG_AON_CM4+0x01B4)
#define SF_TOP_CLK_SEL_SHIFT            13
#define SF_TOP_CLK_SEL_MASK             0x3
#define SF_TOP_CLK_SEL_XTAL             0
#define SF_TOP_CLK_SEL_SYS_64M          1
#define SF_TOP_CLK_SEL_CM4_HCLK         2
#define SF_TOP_CLK_SEL_CM4_AON_32K      3

#define TOP_CFG_CM4_PWR_CTL_CR          (TOP_CFG_AON_CM4+0x01B8)
#define CM4_MCU_960_EN_SHIFT            18
#define CM4_MCU_960_EN_MASK             0x1
#define CM4_MCU_960_EN_DISABLE          0
#define CM4_MCU_960_EN_ENABLE           1
#define CM4_MPLL_EN_SHIFT               28
#define CM4_MPLL_EN_MASK                0x3
#define CM4_MPLL_EN_PLL1_OFF_PLL2_OFF   0
#define CM4_MPLL_EN_PLL1_ON_PLL2_OFF    2
#define CM4_MPLL_EN_PLL1_ON_PLL2_ON     3
#define CM4_BT_PLL_RDY_SHIFT            27
#define CM4_BT_PLL_RDY_MASK             0x1
#define CM4_BT_PLL_RDY                  0x1
#define CM4_WF_PLL_RDY_SHIFT            26
#define CM4_WF_PLL_RDY_MASK             0x1
#define CM4_WF_PLL_RDY                  0x1
#define CM4_NEED_RESTORE_SHIFT          24
#define CM4_NEED_RESTORE_MASK           0x1
#define CM4_NEED_RESTORE                0x1

#define CM4_WDT_STATUS                          (CM4_WDT_BASE+0x003C)
#define CM4_WDT_STATUS_TIMEOUT_EXPIRED_SHIFT    15
#define CM4_WDT_STATUS_TIMEOUT_EXPIRED_MASK     0x1


#define TOP_CFG_CM4_CM4_STRAP_STA       (TOP_CFG_AON_CM4+0x01C0)

#define MCU_FREQUENCY_192MHZ 192000000
#define MCU_FREQUENCY_160MHZ 160000000
#define MCU_FREQUENCY_96MHZ  96000000
#define MCU_FREQUENCY_64MHZ  64000000
#define MCU_FREQUENCY_40MHZ  40000000

typedef union _TOP_CFG_AON_BondingAndStrap {
    uint32_t SPI2HAB_N9_EN : 1;
    uint32_t SPI2HAB_CM4_EN : 1;
    uint32_t BOND_PCIE_MODE : 1;
    uint32_t BOND_USB_MODE : 1;
    uint32_t BOND_CM4_MODE : 1;
    uint32_t B_HOST_SEL : 1;
    uint32_t REG_SPI2AHB_N9_EN : 1;
    uint32_t REG_SPI2AHB_CM4_EN : 1;

    uint32_t XTAL_DIV2_EN : 1;
    uint32_t BOND_SIP_MODE : 1;
    uint32_t BOND_BT_OFF : 1;
    uint32_t BOND_WF_AC_OFF : 1;
    uint32_t BOND_WF_5G_OFF : 1;
    uint32_t XTAL_FREQ : 3;

    uint32_t PWR_ON_DBG_MODE : 1;
    uint32_t FUNC_PAT_MODE_1 : 1;
    uint32_t CODEC_XPLL_MODE : 1;
    uint32_t RBIST_MODE : 1;
    uint32_t Rserved2 : 1;
    uint32_t BT_AFE_TEST_MODE_1 : 1;
    uint32_t BT_AFE_TEST_MODE_2 : 1;
    uint32_t BT_RF_TEST_MODE : 1;

    uint32_t USB_I2C_MODE : 1;
    uint32_t PCIE_I2C_MODE : 1;
    uint32_t Rserved1 : 1;
    uint32_t WF_RF_RXTEST_MODE : 1;
    uint32_t WF_RF_RSSI_TEST_MODE : 1;
    uint32_t RFDIG_SCAN_MODE : 1;
    uint32_t HSP_SCAN_MODE : 1;
    uint32_t RTC_MODE : 1;

    uint32_t AsUint32;
} TOP_CFG_AON_BondingAndStrap, *PTOP_CFG_AON_BondingAndStrap;

static INLINE void cmnWriteRegister32(volatile void *Register, uint32_t Value)
{
    *(volatile uint32_t *)Register = Value;
}

static INLINE uint32_t cmnReadRegister32(volatile void *Register)
{
    return *(volatile uint32_t *)Register;
}

static INLINE uint8_t cmnReadRegister8(volatile void *Register)
{
    return *(volatile uint8_t *)Register;
}

extern uint32_t SystemCoreClock;      /*!< System Clock Frequency (Core Clock)  	*/

extern uint32_t SysTick_Set(uint32_t ticks);

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit(void);

/**
 * Update SystemCoreClock variable
 *
 * @param  none
 * @return none
 *
 * @brief  Updates the SystemCoreClock with current core Clock
 *         retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate(void);

/*
//-------- <<< end of configuration section >>> ------------------------------
*/

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_MT7687_H */
