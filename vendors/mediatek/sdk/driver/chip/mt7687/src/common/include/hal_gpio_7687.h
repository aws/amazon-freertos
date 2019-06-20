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

#include "hal_eint.h"
#include "hal_gpio.h"
#if defined(HAL_EINT_MODULE_ENABLED) || defined(HAL_GPIO_MODULE_ENABLED)

#ifndef __HAL_GPIO_7687_H__
#define __HAL_GPIO_7687_H__

#include "type_def.h"
#include "mt7687_cm4_hw_memmap.h"
#include "pinmux.h"

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/*
#define IOT_GPIO_PINMUX_AON_BASE    (0x81023000)
#define IOT_GPIO_PINMUX_OFF_BASE    (0x80025100)

#define MCU_CFG_NVIC_BASE           (0xE000E000)
#define MCU_CFG_DEBOUNCE_BASE       (0x8300c000)
#define MCU_CFG_NVIC_SENSE_BASE     (0x8300c000)

#define IOT_GPIO_OFF_BASE           (0x80025000)
#define IOT_GPIO_AON_BASE           (0x8300B000)
*/
#define IOT_GPIO_PU1                (0x00)
#define IOT_GPIO_PU1_SET            (IOT_GPIO_PU1 + 0x04)
#define IOT_GPIO_PU1_RESET          (IOT_GPIO_PU1 + 0x08)
#define IOT_GPIO_PU2                (0x10)
#define IOT_GPIO_PU2_SET            (IOT_GPIO_PU2 + 0x04)
#define IOT_GPIO_PU2_RESET          (IOT_GPIO_PU2 + 0x08)
#define IOT_GPIO_PU3                (0x20)
#define IOT_GPIO_PU3_SET            (IOT_GPIO_PU3 + 0x04)
#define IOT_GPIO_PU3_RESET          (IOT_GPIO_PU3 + 0x08)

#define IOT_GPIO_PD1                (0x30)
#define IOT_GPIO_PD1_SET            (IOT_GPIO_PD1 + 0x04)
#define IOT_GPIO_PD1_RESET          (IOT_GPIO_PD1 + 0x08)
#define IOT_GPIO_PD2                (0x40)
#define IOT_GPIO_PD2_SET            (IOT_GPIO_PD2 + 0x04)
#define IOT_GPIO_PD2_RESET          (IOT_GPIO_PD2 + 0x08)
#define IOT_GPIO_PD3                (0x50)
#define IOT_GPIO_PD3_SET            (IOT_GPIO_PD3 + 0x04)
#define IOT_GPIO_PD3_RESET          (IOT_GPIO_PD3 + 0x08)

#define IOT_GPIO_DOUT1              (0x60)
#define IOT_GPIO_DOUT1_SET          (IOT_GPIO_DOUT1 + 0x04)
#define IOT_GPIO_DOUT1_RESET        (IOT_GPIO_DOUT1 + 0x08)
#define IOT_GPIO_DOUT2              (0x70)
#define IOT_GPIO_DOUT2_SET          (IOT_GPIO_DOUT2 + 0x04)
#define IOT_GPIO_DOUT2_RESET        (IOT_GPIO_DOUT2 + 0x08)
#define IOT_GPIO_DOUT3              (0x80)
#define IOT_GPIO_DOUT3_SET          (IOT_GPIO_DOUT3 + 0x04)
#define IOT_GPIO_DOUT3_RESET        (IOT_GPIO_DOUT3 + 0x08)

#define IOT_GPIO_OE1                (0x90)
#define IOT_GPIO_OE1_SET            (IOT_GPIO_OE1 + 0x04)
#define IOT_GPIO_OE1_RESET          (IOT_GPIO_OE1 + 0x08)
#define IOT_GPIO_OE2                (0xA0)
#define IOT_GPIO_OE2_SET            (IOT_GPIO_OE2 + 0x04)
#define IOT_GPIO_OE2_RESET          (IOT_GPIO_OE2 + 0x08)
#define IOT_GPIO_OE3                (0xB0)
#define IOT_GPIO_OE3_SET            (IOT_GPIO_OE3 + 0x04)
#define IOT_GPIO_OE3_RESET          (IOT_GPIO_OE3 + 0x08)

#define IOT_GPIO_DIN1               (0xC0)
#define IOT_GPIO_DIN2               (IOT_GPIO_DIN1 + 0x04)
#define IOT_GPIO_DIN3               (IOT_GPIO_DIN1 + 0x08)

#define IOT_GPIO_PADDRV1            (0xD0)
#define IOT_GPIO_PADDRV2            (IOT_GPIO_PADDRV1 + 0x04)
#define IOT_GPIO_PADDRV3            (IOT_GPIO_PADDRV1 + 0x08)
#define IOT_GPIO_PADDRV4            (IOT_GPIO_PADDRV1 + 0x0C)
#define IOT_GPIO_PADDRV5            (IOT_GPIO_PADDRV1 + 0x10)

#define IOT_GPIO_PAD_CTRL0          (0x81021080)
#define IOT_GPIO_PAD_CTRL1          (IOT_GPIO_PAD_CTRL0 + 0x04)
#define IOT_GPIO_PAD_CTRL2          (IOT_GPIO_PAD_CTRL0 + 0x08)

#define IOT_GPIO_IES0               (0x100)
#define IOT_GPIO_IES1               (IOT_GPIO_IES0 + 0x04)
#define IOT_GPIO_IES2               (IOT_GPIO_IES0 + 0x08)


#define IOT_GPIO_SDIO_CLK               (0x48)
#define IOT_GPIO_SDIO_CMD               (IOT_GPIO_SDIO_CLK + 0x04)
#define IOT_GPIO_SDIO_DATA0             (IOT_GPIO_SDIO_CLK + 0x08)
#define IOT_GPIO_SDIO_DATA1             (IOT_GPIO_SDIO_CLK + 0x0C)
#define IOT_GPIO_SDIO_DATA2             (IOT_GPIO_SDIO_CLK + 0x10)
#define IOT_GPIO_SDIO_DATA3             (IOT_GPIO_SDIO_CLK + 0x14)


/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/
//OE & DOUT & DIN Bit Mask
#define IOT_GPIO0_OE_DOUT_DIN_MASK (BIT(0))
#define IOT_GPIO1_OE_DOUT_DIN_MASK (BIT(1))
#define IOT_GPIO2_OE_DOUT_DIN_MASK (BIT(2))
#define IOT_GPIO3_OE_DOUT_DIN_MASK (BIT(3))
#define IOT_GPIO4_OE_DOUT_DIN_MASK (BIT(4))
#define IOT_GPIO5_OE_DOUT_DIN_MASK (BIT(5))
#define IOT_GPIO6_OE_DOUT_DIN_MASK (BIT(6))
#define IOT_GPIO7_OE_DOUT_DIN_MASK (BIT(7))

#define IOT_GPIO24_OE_DOUT_DIN_MASK (BIT(24))
#define IOT_GPIO25_OE_DOUT_DIN_MASK (BIT(25))
#define IOT_GPIO26_OE_DOUT_DIN_MASK (BIT(26))
#define IOT_GPIO27_OE_DOUT_DIN_MASK (BIT(27))
#define IOT_GPIO28_OE_DOUT_DIN_MASK (BIT(28))
#define IOT_GPIO29_OE_DOUT_DIN_MASK (BIT(29))
#define IOT_GPIO30_OE_DOUT_DIN_MASK (BIT(30))
#define IOT_GPIO31_OE_DOUT_DIN_MASK (BIT(31))
#define IOT_GPIO32_OE_DOUT_DIN_MASK (BIT(0))
#define IOT_GPIO33_OE_DOUT_DIN_MASK (BIT(1))
#define IOT_GPIO34_OE_DOUT_DIN_MASK (BIT(2))
#define IOT_GPIO35_OE_DOUT_DIN_MASK (BIT(3))
#define IOT_GPIO36_OE_DOUT_DIN_MASK (BIT(4))
#define IOT_GPIO37_OE_DOUT_DIN_MASK (BIT(5))
#define IOT_GPIO38_OE_DOUT_DIN_MASK (BIT(6))
#define IOT_GPIO39_OE_DOUT_DIN_MASK (BIT(7))

#define IOT_GPIO57_OE_DOUT_DIN_MASK (BIT(25))
#define IOT_GPIO58_OE_DOUT_DIN_MASK (BIT(26))
#define IOT_GPIO59_OE_DOUT_DIN_MASK (BIT(27))
#define IOT_GPIO60_OE_DOUT_DIN_MASK (BIT(28))

/********************************************/
/*********Config AON Function 3 WIC**********/
/********************************************/
/*config IOT_GPIO2 = PAD_ANTSEL2 = WIC_0 = IRQ_32 */
#define IOT_GPIO2_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO2_NVIC_EN_MASK                  BIT(0)
#define IOT_GPIO2_NVIC_EN_SHFT                  0

#define IOT_GPIO2_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO2_NVIC_CLR_EN_MASK              BIT(0)
#define IOT_GPIO2_NVIC_CLR_EN_SHFT              0

#define IOT_GPIO2_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO2_NVIC_PEND_MASK                BIT(0)
#define IOT_GPIO2_NVIC_PEND_SHFT                0

#define IOT_GPIO2_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO2_NVIC_CLR_PEND_MASK            BIT(0)
#define IOT_GPIO2_NVIC_CLR_PEND_SHFT            0

#define IOT_GPIO2_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO2_NVIC_ACTIVE_MASK              BIT(0)
#define IOT_GPIO2_NVIC_ACTIVE_SHFT              0

#define IOT_GPIO2_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO2_NVIC_SENSE_MASK               BIT(0)
#define IOT_GPIO2_NVIC_SENSE_SHFT               0

#define IOT_GPIO2_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x0008)
#define IOT_GPIO2_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO2_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO2_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x0008)
#define IOT_GPIO2_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO2_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO2_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x0008)
#define IOT_GPIO2_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO2_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO2_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x0008)
#define IOT_GPIO2_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO2_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO2_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x0008)
#define IOT_GPIO2_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO2_DEBOUNCE_CNT_SHFT             0
/*config IOT_GPIO25 = PAD_WAKE_N = WIC_1 = IRQ_33 */
#define IOT_GPIO25_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO25_NVIC_EN_MASK                  BIT(1)
#define IOT_GPIO25_NVIC_EN_SHFT                  1

#define IOT_GPIO25_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO25_NVIC_CLR_EN_MASK              BIT(1)
#define IOT_GPIO25_NVIC_CLR_EN_SHFT              1

#define IOT_GPIO25_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO25_NVIC_PEND_MASK                BIT(1)
#define IOT_GPIO25_NVIC_PEND_SHFT                1

#define IOT_GPIO25_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO25_NVIC_CLR_PEND_MASK            BIT(1)
#define IOT_GPIO25_NVIC_CLR_PEND_SHFT            1

#define IOT_GPIO25_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO25_NVIC_ACTIVE_MASK              BIT(1)
#define IOT_GPIO25_NVIC_ACTIVE_SHFT              1

#define IOT_GPIO25_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO25_NVIC_SENSE_MASK               BIT(1)
#define IOT_GPIO25_NVIC_SENSE_SHFT               1

#define IOT_GPIO25_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x000C)
#define IOT_GPIO25_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO25_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO25_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x000C)
#define IOT_GPIO25_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO25_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO25_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x000C)
#define IOT_GPIO25_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO25_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO25_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x000C)
#define IOT_GPIO25_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO25_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO25_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x000C)
#define IOT_GPIO25_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO25_DEBOUNCE_CNT_SHFT             0


/*config IOT_GPIO27 = PAD_SDIO_CLK = WIC_2 = IRQ_34 */
#define IOT_GPIO27_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO27_NVIC_EN_MASK                  BIT(2)
#define IOT_GPIO27_NVIC_EN_SHFT                  2

#define IOT_GPIO27_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO27_NVIC_CLR_EN_MASK              BIT(2)
#define IOT_GPIO27_NVIC_CLR_EN_SHFT              2

#define IOT_GPIO27_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO27_NVIC_PEND_MASK                BIT(2)
#define IOT_GPIO27_NVIC_PEND_SHFT                2

#define IOT_GPIO27_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO27_NVIC_CLR_PEND_MASK            BIT(2)
#define IOT_GPIO27_NVIC_CLR_PEND_SHFT            2

#define IOT_GPIO27_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO27_NVIC_ACTIVE_MASK              BIT(2)
#define IOT_GPIO27_NVIC_ACTIVE_SHFT              2

#define IOT_GPIO27_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO27_NVIC_SENSE_MASK               BIT(2)
#define IOT_GPIO27_NVIC_SENSE_SHFT               2

#define IOT_GPIO27_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x0010)
#define IOT_GPIO27_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO27_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO27_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x0010)
#define IOT_GPIO27_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO27_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO27_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x0010)
#define IOT_GPIO27_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO27_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO27_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x0010)
#define IOT_GPIO27_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO27_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO27_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x0010)
#define IOT_GPIO27_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO27_DEBOUNCE_CNT_SHFT             0

/*config IOT_GPIO29 = PAD_SDIO_DAT3 = WIC_3 = IRQ_35 */
#define IOT_GPIO29_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO29_NVIC_EN_MASK                  BIT(3)
#define IOT_GPIO29_NVIC_EN_SHFT                  3

#define IOT_GPIO29_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO29_NVIC_CLR_EN_MASK              BIT(3)
#define IOT_GPIO29_NVIC_CLR_EN_SHFT              3

#define IOT_GPIO29_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO29_NVIC_PEND_MASK                BIT(3)
#define IOT_GPIO29_NVIC_PEND_SHFT                3

#define IOT_GPIO29_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO29_NVIC_CLR_PEND_MASK            BIT(3)
#define IOT_GPIO29_NVIC_CLR_PEND_SHFT            3

#define IOT_GPIO29_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO29_NVIC_ACTIVE_MASK              BIT(3)
#define IOT_GPIO29_NVIC_ACTIVE_SHFT              3

#define IOT_GPIO29_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO29_NVIC_SENSE_MASK               BIT(3)
#define IOT_GPIO29_NVIC_SENSE_SHFT               3

#define IOT_GPIO29_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x0014)
#define IOT_GPIO29_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO29_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO29_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x0014)
#define IOT_GPIO29_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO29_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO29_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x0014)
#define IOT_GPIO29_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO29_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO29_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x0014)
#define IOT_GPIO29_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO29_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO29_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x0014)
#define IOT_GPIO29_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO29_DEBOUNCE_CNT_SHFT             0

/*config IOT_GPIO32 = PAD_SDIO_DAT0 = WIC_4 = IRQ_36 */
#define IOT_GPIO32_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO32_NVIC_EN_MASK                  BIT(4)
#define IOT_GPIO32_NVIC_EN_SHFT                  4

#define IOT_GPIO32_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO32_NVIC_CLR_EN_MASK              BIT(4)
#define IOT_GPIO32_NVIC_CLR_EN_SHFT              4

#define IOT_GPIO32_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO32_NVIC_PEND_MASK                BIT(4)
#define IOT_GPIO32_NVIC_PEND_SHFT                4

#define IOT_GPIO32_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO32_NVIC_CLR_PEND_MASK            BIT(4)
#define IOT_GPIO32_NVIC_CLR_PEND_SHFT            4

#define IOT_GPIO32_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO32_NVIC_ACTIVE_MASK              BIT(4)
#define IOT_GPIO32_NVIC_ACTIVE_SHFT              4

#define IOT_GPIO32_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO32_NVIC_SENSE_MASK               BIT(4)
#define IOT_GPIO32_NVIC_SENSE_SHFT               4

#define IOT_GPIO32_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x0018)
#define IOT_GPIO32_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO32_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO32_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x0018)
#define IOT_GPIO32_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO32_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO32_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x0018)
#define IOT_GPIO32_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO32_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO32_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x0018)
#define IOT_GPIO32_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO32_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO32_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x0018)
#define IOT_GPIO32_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO32_DEBOUNCE_CNT_SHFT             0


/*config IOT_GPIO33 = PAD_GPIO0 = WIC_5 = IRQ_37 */
#define IOT_GPIO33_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO33_NVIC_EN_MASK                  BIT(5)
#define IOT_GPIO33_NVIC_EN_SHFT                  5

#define IOT_GPIO33_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO33_NVIC_CLR_EN_MASK              BIT(5)
#define IOT_GPIO33_NVIC_CLR_EN_SHFT              5

#define IOT_GPIO33_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO33_NVIC_PEND_MASK                BIT(5)
#define IOT_GPIO33_NVIC_PEND_SHFT                5

#define IOT_GPIO33_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO33_NVIC_CLR_PEND_MASK            BIT(5)
#define IOT_GPIO33_NVIC_CLR_PEND_SHFT            5

#define IOT_GPIO33_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO33_NVIC_ACTIVE_MASK              BIT(5)
#define IOT_GPIO33_NVIC_ACTIVE_SHFT              5

#define IOT_GPIO33_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO33_NVIC_SENSE_MASK               BIT(5)
#define IOT_GPIO33_NVIC_SENSE_SHFT               5

#define IOT_GPIO33_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x001C)
#define IOT_GPIO33_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO33_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO33_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x001C)
#define IOT_GPIO33_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO33_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO33_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x001C)
#define IOT_GPIO33_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO33_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO33_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x001C)
#define IOT_GPIO33_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO33_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO33_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x001C)
#define IOT_GPIO33_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO33_DEBOUNCE_CNT_SHFT             0

/*config IOT_GPIO34 = PAD_GPIO1 = WIC_6 = IRQ_38 */
#define IOT_GPIO34_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO34_NVIC_EN_MASK                  BIT(6)
#define IOT_GPIO34_NVIC_EN_SHFT                  6

#define IOT_GPIO34_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO34_NVIC_CLR_EN_MASK              BIT(6)
#define IOT_GPIO34_NVIC_CLR_EN_SHFT              6

#define IOT_GPIO34_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO34_NVIC_PEND_MASK                BIT(6)
#define IOT_GPIO34_NVIC_PEND_SHFT                6

#define IOT_GPIO34_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO34_NVIC_CLR_PEND_MASK            BIT(6)
#define IOT_GPIO34_NVIC_CLR_PEND_SHFT            6

#define IOT_GPIO34_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO34_NVIC_ACTIVE_MASK              BIT(6)
#define IOT_GPIO34_NVIC_ACTIVE_SHFT              6

#define IOT_GPIO34_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO34_NVIC_SENSE_MASK               BIT(6)
#define IOT_GPIO34_NVIC_SENSE_SHFT               6

#define IOT_GPIO34_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x0020)
#define IOT_GPIO34_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO34_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO34_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x0020)
#define IOT_GPIO34_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO34_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO34_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x0020)
#define IOT_GPIO34_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO34_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO34_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x0020)
#define IOT_GPIO34_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO34_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO34_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x0020)
#define IOT_GPIO34_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO34_DEBOUNCE_CNT_SHFT             0

/*config IOT_GPIO36 = PAD_UART_RX = WIC_7 = IRQ_39 */
#define IOT_GPIO36_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO36_NVIC_EN_MASK                  BIT(7)
#define IOT_GPIO36_NVIC_EN_SHFT                  7

#define IOT_GPIO36_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO36_NVIC_CLR_EN_MASK              BIT(7)
#define IOT_GPIO36_NVIC_CLR_EN_SHFT              7

#define IOT_GPIO36_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO36_NVIC_PEND_MASK                BIT(7)
#define IOT_GPIO36_NVIC_PEND_SHFT                7

#define IOT_GPIO36_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO36_NVIC_CLR_PEND_MASK            BIT(7)
#define IOT_GPIO36_NVIC_CLR_PEND_SHFT            7

#define IOT_GPIO36_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO36_NVIC_ACTIVE_MASK              BIT(7)
#define IOT_GPIO36_NVIC_ACTIVE_SHFT              7

#define IOT_GPIO36_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO36_NVIC_SENSE_MASK               BIT(7)
#define IOT_GPIO36_NVIC_SENSE_SHFT               7

#define IOT_GPIO36_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x0024)
#define IOT_GPIO36_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO36_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO36_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x0024)
#define IOT_GPIO36_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO36_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO36_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x0024)
#define IOT_GPIO36_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO36_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO36_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x0024)
#define IOT_GPIO36_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO36_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO36_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x0024)
#define IOT_GPIO36_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO36_DEBOUNCE_CNT_SHFT             0

/*config IOT_GPIO57 = PAD_WF_RF_DIS_B = WIC_8 = IRQ_40 */
#define IOT_GPIO57_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO57_NVIC_EN_MASK                  BIT(8)
#define IOT_GPIO57_NVIC_EN_SHFT                  8

#define IOT_GPIO57_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO57_NVIC_CLR_EN_MASK              BIT(8)
#define IOT_GPIO57_NVIC_CLR_EN_SHFT              8

#define IOT_GPIO57_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO57_NVIC_PEND_MASK                BIT(8)
#define IOT_GPIO57_NVIC_PEND_SHFT                8

#define IOT_GPIO57_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO57_NVIC_CLR_PEND_MASK            BIT(8)
#define IOT_GPIO57_NVIC_CLR_PEND_SHFT            8

#define IOT_GPIO57_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO57_NVIC_ACTIVE_MASK              BIT(8)
#define IOT_GPIO57_NVIC_ACTIVE_SHFT              8

#define IOT_GPIO57_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO57_NVIC_SENSE_MASK               BIT(8)
#define IOT_GPIO57_NVIC_SENSE_SHFT               8

#define IOT_GPIO57_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x0028)
#define IOT_GPIO57_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO57_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO57_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x0028)
#define IOT_GPIO57_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO57_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO57_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x0028)
#define IOT_GPIO57_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO57_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO57_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x0028)
#define IOT_GPIO57_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO57_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO57_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x0028)
#define IOT_GPIO57_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO57_DEBOUNCE_CNT_SHFT             0

/*config IOT_GPIO58 = PAD_BT_RF_DIS_B = WIC_9 = IRQ_41 */
#define IOT_GPIO58_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO58_NVIC_EN_MASK                  BIT(9)
#define IOT_GPIO58_NVIC_EN_SHFT                  9

#define IOT_GPIO58_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO58_NVIC_CLR_EN_MASK              BIT(9)
#define IOT_GPIO58_NVIC_CLR_EN_SHFT              9

#define IOT_GPIO58_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO58_NVIC_PEND_MASK                BIT(9)
#define IOT_GPIO58_NVIC_PEND_SHFT                9

#define IOT_GPIO58_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO58_NVIC_CLR_PEND_MASK            BIT(9)
#define IOT_GPIO58_NVIC_CLR_PEND_SHFT            9

#define IOT_GPIO58_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO58_NVIC_ACTIVE_MASK              BIT(9)
#define IOT_GPIO58_NVIC_ACTIVE_SHFT              9

#define IOT_GPIO58_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO58_NVIC_SENSE_MASK               BIT(9)
#define IOT_GPIO58_NVIC_SENSE_SHFT               9

#define IOT_GPIO58_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x002C)
#define IOT_GPIO58_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO58_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO58_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x002C)
#define IOT_GPIO58_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO58_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO58_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x002C)
#define IOT_GPIO58_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO58_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO58_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x002C)
#define IOT_GPIO58_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO58_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO58_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x002C)
#define IOT_GPIO58_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO58_DEBOUNCE_CNT_SHFT             0


/*config IOT_GPIO59 = PAD_WF_LED_B = WIC_10 = IRQ_42 */
#define IOT_GPIO59_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO59_NVIC_EN_MASK                  BIT(10)
#define IOT_GPIO59_NVIC_EN_SHFT                  10

#define IOT_GPIO59_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO59_NVIC_CLR_EN_MASK              BIT(10)
#define IOT_GPIO59_NVIC_CLR_EN_SHFT              10

#define IOT_GPIO59_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO59_NVIC_PEND_MASK                BIT(10)
#define IOT_GPIO59_NVIC_PEND_SHFT                10

#define IOT_GPIO59_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO59_NVIC_CLR_PEND_MASK            BIT(10)
#define IOT_GPIO59_NVIC_CLR_PEND_SHFT            10

#define IOT_GPIO59_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO59_NVIC_ACTIVE_MASK              BIT(10)
#define IOT_GPIO59_NVIC_ACTIVE_SHFT              10

#define IOT_GPIO59_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO59_NVIC_SENSE_MASK               BIT(10)
#define IOT_GPIO59_NVIC_SENSE_SHFT               10

#define IOT_GPIO59_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x0030)
#define IOT_GPIO59_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO59_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO59_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x0030)
#define IOT_GPIO59_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO59_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO59_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x0030)
#define IOT_GPIO59_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO59_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO59_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x0030)
#define IOT_GPIO59_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO59_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO59_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x0030)
#define IOT_GPIO59_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO59_DEBOUNCE_CNT_SHFT             0


/*config IOT_GPIO60 = PAD_BT_LED_B = WIC_11 = IRQ_43 */
#define IOT_GPIO60_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO60_NVIC_EN_MASK                  BIT(11)
#define IOT_GPIO60_NVIC_EN_SHFT                  11

#define IOT_GPIO60_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO60_NVIC_CLR_EN_MASK              BIT(11)
#define IOT_GPIO60_NVIC_CLR_EN_SHFT              11

#define IOT_GPIO60_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO60_NVIC_PEND_MASK                BIT(11)
#define IOT_GPIO60_NVIC_PEND_SHFT                11

#define IOT_GPIO60_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO60_NVIC_CLR_PEND_MASK            BIT(11)
#define IOT_GPIO60_NVIC_CLR_PEND_SHFT            11

#define IOT_GPIO60_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO60_NVIC_ACTIVE_MASK              BIT(11)
#define IOT_GPIO60_NVIC_ACTIVE_SHFT              11

#define IOT_GPIO60_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO60_NVIC_SENSE_MASK               BIT(11)
#define IOT_GPIO60_NVIC_SENSE_SHFT               11

#define IOT_GPIO60_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x0034)
#define IOT_GPIO60_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO60_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO60_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x0034)
#define IOT_GPIO60_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO60_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO60_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x0034)
#define IOT_GPIO60_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO60_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO60_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x0034)
#define IOT_GPIO60_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO60_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO60_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x0034)
#define IOT_GPIO60_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO60_DEBOUNCE_CNT_SHFT             0
//===========================================================================
/********************************************/
/*********Config AON Function 3 EINT**********/
/********************************************/
/*config IOT_GPIO0 = PAD_ANTSEL0 = EINT_0 = IRQ_56 */
#define IOT_GPIO0_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO0_NVIC_EN_MASK                  BIT(24)
#define IOT_GPIO0_NVIC_EN_SHFT                  24

#define IOT_GPIO0_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO0_NVIC_CLR_EN_MASK              BIT(24)
#define IOT_GPIO0_NVIC_CLR_EN_SHFT              24

#define IOT_GPIO0_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO0_NVIC_PEND_MASK                BIT(24)
#define IOT_GPIO0_NVIC_PEND_SHFT                24

#define IOT_GPIO0_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO0_NVIC_CLR_PEND_MASK            BIT(24)
#define IOT_GPIO0_NVIC_CLR_PEND_SHFT            24

#define IOT_GPIO0_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO0_NVIC_ACTIVE_MASK              BIT(24)
#define IOT_GPIO0_NVIC_ACTIVE_SHFT              24

#define IOT_GPIO0_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO0_NVIC_SENSE_MASK               BIT(24)
#define IOT_GPIO0_NVIC_SENSE_SHFT               24

#define IOT_GPIO0_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x0068)
#define IOT_GPIO0_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO0_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO0_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x0068)
#define IOT_GPIO0_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO0_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO0_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x0068)
#define IOT_GPIO0_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO0_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO0_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x0068)
#define IOT_GPIO0_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO0_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO0_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x0068)
#define IOT_GPIO0_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO0_DEBOUNCE_CNT_SHFT             0

/*config IOT_GPIO1 = PAD_ANTSEL1 = EINT_1 = IRQ_57 */
#define IOT_GPIO1_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO1_NVIC_EN_MASK                  BIT(25)
#define IOT_GPIO1_NVIC_EN_SHFT                  25

#define IOT_GPIO1_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO1_NVIC_CLR_EN_MASK              BIT(25)
#define IOT_GPIO1_NVIC_CLR_EN_SHFT              25

#define IOT_GPIO1_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO1_NVIC_PEND_MASK                BIT(25)
#define IOT_GPIO1_NVIC_PEND_SHFT                25

#define IOT_GPIO1_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO1_NVIC_CLR_PEND_MASK            BIT(25)
#define IOT_GPIO1_NVIC_CLR_PEND_SHFT            25

#define IOT_GPIO1_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO1_NVIC_ACTIVE_MASK              BIT(25)
#define IOT_GPIO1_NVIC_ACTIVE_SHFT              25

#define IOT_GPIO1_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO1_NVIC_SENSE_MASK               BIT(25)
#define IOT_GPIO1_NVIC_SENSE_SHFT               25

#define IOT_GPIO1_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x006C)
#define IOT_GPIO1_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO1_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO1_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x006C)
#define IOT_GPIO1_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO1_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO1_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x006C)
#define IOT_GPIO1_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO1_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO1_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x006C)
#define IOT_GPIO1_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO1_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO1_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x006C)
#define IOT_GPIO1_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO1_DEBOUNCE_CNT_SHFT             0

/*config IOT_GPIO3 = PAD_ANTSEL3 = EINT_2 = IRQ_58 */
#define IOT_GPIO3_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO3_NVIC_EN_MASK                  BIT(26)
#define IOT_GPIO3_NVIC_EN_SHFT                  26

#define IOT_GPIO3_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO3_NVIC_CLR_EN_MASK              BIT(26)
#define IOT_GPIO3_NVIC_CLR_EN_SHFT              26

#define IOT_GPIO3_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO3_NVIC_PEND_MASK                BIT(26)
#define IOT_GPIO3_NVIC_PEND_SHFT                26

#define IOT_GPIO3_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO3_NVIC_CLR_PEND_MASK            BIT(26)
#define IOT_GPIO3_NVIC_CLR_PEND_SHFT            26

#define IOT_GPIO3_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO3_NVIC_ACTIVE_MASK              BIT(26)
#define IOT_GPIO3_NVIC_ACTIVE_SHFT              26

#define IOT_GPIO3_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO3_NVIC_SENSE_MASK               BIT(26)
#define IOT_GPIO3_NVIC_SENSE_SHFT               26

#define IOT_GPIO3_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x0070)
#define IOT_GPIO3_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO3_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO3_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x0070)
#define IOT_GPIO3_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO3_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO3_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x0070)
#define IOT_GPIO3_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO3_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO3_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x0070)
#define IOT_GPIO3_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO3_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO3_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x0070)
#define IOT_GPIO3_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO3_DEBOUNCE_CNT_SHFT             0

/*config IOT_GPIO4 = PAD_ANTSEL4 = EINT_3 = IRQ_59 */
#define IOT_GPIO4_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO4_NVIC_EN_MASK                  BIT(27)
#define IOT_GPIO4_NVIC_EN_SHFT                  27

#define IOT_GPIO4_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO4_NVIC_CLR_EN_MASK              BIT(27)
#define IOT_GPIO4_NVIC_CLR_EN_SHFT              27

#define IOT_GPIO4_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO4_NVIC_PEND_MASK                BIT(27)
#define IOT_GPIO4_NVIC_PEND_SHFT                27

#define IOT_GPIO4_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO4_NVIC_CLR_PEND_MASK            BIT(27)
#define IOT_GPIO4_NVIC_CLR_PEND_SHFT            27

#define IOT_GPIO4_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO4_NVIC_ACTIVE_MASK              BIT(27)
#define IOT_GPIO4_NVIC_ACTIVE_SHFT              27

#define IOT_GPIO4_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO4_NVIC_SENSE_MASK               BIT(27)
#define IOT_GPIO4_NVIC_SENSE_SHFT               27

#define IOT_GPIO4_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x0074)
#define IOT_GPIO4_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO4_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO4_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x0074)
#define IOT_GPIO4_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO4_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO4_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x0074)
#define IOT_GPIO4_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO4_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO4_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x0074)
#define IOT_GPIO4_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO4_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO4_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x0074)
#define IOT_GPIO4_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO4_DEBOUNCE_CNT_SHFT             0
/*config IOT_GPIO5 = PAD_ANTSEL5 = EINT_4 = IRQ_60 */
#define IOT_GPIO5_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO5_NVIC_EN_MASK                  BIT(28)
#define IOT_GPIO5_NVIC_EN_SHFT                  28

#define IOT_GPIO5_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO5_NVIC_CLR_EN_MASK              BIT(28)
#define IOT_GPIO5_NVIC_CLR_EN_SHFT              28

#define IOT_GPIO5_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO5_NVIC_PEND_MASK                BIT(28)
#define IOT_GPIO5_NVIC_PEND_SHFT                28

#define IOT_GPIO5_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO5_NVIC_CLR_PEND_MASK            BIT(28)
#define IOT_GPIO5_NVIC_CLR_PEND_SHFT            28

#define IOT_GPIO5_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO5_NVIC_ACTIVE_MASK              BIT(28)
#define IOT_GPIO5_NVIC_ACTIVE_SHFT              28

#define IOT_GPIO5_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO5_NVIC_SENSE_MASK               BIT(28)
#define IOT_GPIO5_NVIC_SENSE_SHFT               28

#define IOT_GPIO5_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x0078)
#define IOT_GPIO5_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO5_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO5_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x0078)
#define IOT_GPIO5_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO5_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO5_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x0078)
#define IOT_GPIO5_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO5_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO5_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x0078)
#define IOT_GPIO5_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO5_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO5_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x0078)
#define IOT_GPIO5_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO5_DEBOUNCE_CNT_SHFT             0
/*config IOT_GPIO6 = PAD_ANTSEL6 = EINT_5 = IRQ_61 */
#define IOT_GPIO6_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO6_NVIC_EN_MASK                  BIT(29)
#define IOT_GPIO6_NVIC_EN_SHFT                  29

#define IOT_GPIO6_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO6_NVIC_CLR_EN_MASK              BIT(29)
#define IOT_GPIO6_NVIC_CLR_EN_SHFT              29

#define IOT_GPIO6_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO6_NVIC_PEND_MASK                BIT(29)
#define IOT_GPIO6_NVIC_PEND_SHFT                29

#define IOT_GPIO6_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO6_NVIC_CLR_PEND_MASK            BIT(29)
#define IOT_GPIO6_NVIC_CLR_PEND_SHFT            29

#define IOT_GPIO6_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO6_NVIC_ACTIVE_MASK              BIT(29)
#define IOT_GPIO6_NVIC_ACTIVE_SHFT              29

#define IOT_GPIO6_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO6_NVIC_SENSE_MASK               BIT(29)
#define IOT_GPIO6_NVIC_SENSE_SHFT               29

#define IOT_GPIO6_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x007C)
#define IOT_GPIO6_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO6_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO6_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x007C)
#define IOT_GPIO6_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO6_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO6_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x007C)
#define IOT_GPIO6_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO6_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO6_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x007C)
#define IOT_GPIO6_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO6_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO6_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x007C)
#define IOT_GPIO6_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO6_DEBOUNCE_CNT_SHFT             0
/*config IOT_GPIO7 = PAD_ANTSEL7 = EINT_6 = IRQ_62 */
#define IOT_GPIO7_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x104)
#define IOT_GPIO7_NVIC_EN_MASK                  BIT(30)
#define IOT_GPIO7_NVIC_EN_SHFT                  30

#define IOT_GPIO7_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x184)
#define IOT_GPIO7_NVIC_CLR_EN_MASK              BIT(30)
#define IOT_GPIO7_NVIC_CLR_EN_SHFT              30

#define IOT_GPIO7_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x204)
#define IOT_GPIO7_NVIC_PEND_MASK                BIT(30)
#define IOT_GPIO7_NVIC_PEND_SHFT                30

#define IOT_GPIO7_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x284)
#define IOT_GPIO7_NVIC_CLR_PEND_MASK            BIT(30)
#define IOT_GPIO7_NVIC_CLR_PEND_SHFT            30

#define IOT_GPIO7_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x304)
#define IOT_GPIO7_NVIC_ACTIVE_MASK              BIT(30)
#define IOT_GPIO7_NVIC_ACTIVE_SHFT              30

#define IOT_GPIO7_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0204)
#define IOT_GPIO7_NVIC_SENSE_MASK               BIT(30)
#define IOT_GPIO7_NVIC_SENSE_SHFT               30

#define IOT_GPIO7_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x0080)
#define IOT_GPIO7_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO7_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO7_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x0080)
#define IOT_GPIO7_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO7_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO7_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x0080)
#define IOT_GPIO7_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO7_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO7_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x0080)
#define IOT_GPIO7_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO7_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO7_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x0080)
#define IOT_GPIO7_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO7_DEBOUNCE_CNT_SHFT             0
/*config IOT_GPIO35 = PAD_UART_DBG = EINT_19 = IRQ_75 */
#define IOT_GPIO35_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x108)
#define IOT_GPIO35_NVIC_EN_MASK                  BIT(11)
#define IOT_GPIO35_NVIC_EN_SHFT                  11

#define IOT_GPIO35_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x188)
#define IOT_GPIO35_NVIC_CLR_EN_MASK              BIT(11)
#define IOT_GPIO35_NVIC_CLR_EN_SHFT              11

#define IOT_GPIO35_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x208)
#define IOT_GPIO35_NVIC_PEND_MASK                BIT(11)
#define IOT_GPIO35_NVIC_PEND_SHFT                11

#define IOT_GPIO35_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x288)
#define IOT_GPIO35_NVIC_CLR_PEND_MASK            BIT(11)
#define IOT_GPIO35_NVIC_CLR_PEND_SHFT            11

#define IOT_GPIO35_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x308)
#define IOT_GPIO35_NVIC_ACTIVE_MASK              BIT(11)
#define IOT_GPIO35_NVIC_ACTIVE_SHFT              11

#define IOT_GPIO35_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0208)
#define IOT_GPIO35_NVIC_SENSE_MASK               BIT(11)
#define IOT_GPIO35_NVIC_SENSE_SHFT               11

#define IOT_GPIO35_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x00B4)
#define IOT_GPIO35_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO35_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO35_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x00B4)
#define IOT_GPIO35_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO35_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO35_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x00B4)
#define IOT_GPIO35_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO35_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO35_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x00B4)
#define IOT_GPIO35_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO35_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO35_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x00B4)
#define IOT_GPIO35_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO35_DEBOUNCE_CNT_SHFT             0
/*config IOT_GPIO37 = PAD_UART_TX = EINT_20 = IRQ_76 */
#define IOT_GPIO37_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x108)
#define IOT_GPIO37_NVIC_EN_MASK                  BIT(12)
#define IOT_GPIO37_NVIC_EN_SHFT                  12

#define IOT_GPIO37_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x188)
#define IOT_GPIO37_NVIC_CLR_EN_MASK              BIT(12)
#define IOT_GPIO37_NVIC_CLR_EN_SHFT              12

#define IOT_GPIO37_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x208)
#define IOT_GPIO37_NVIC_PEND_MASK                BIT(12)
#define IOT_GPIO37_NVIC_PEND_SHFT                12

#define IOT_GPIO37_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x288)
#define IOT_GPIO37_NVIC_CLR_PEND_MASK            BIT(12)
#define IOT_GPIO37_NVIC_CLR_PEND_SHFT            12

#define IOT_GPIO37_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x308)
#define IOT_GPIO37_NVIC_ACTIVE_MASK              BIT(12)
#define IOT_GPIO37_NVIC_ACTIVE_SHFT              12

#define IOT_GPIO37_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0208)
#define IOT_GPIO37_NVIC_SENSE_MASK               BIT(12)
#define IOT_GPIO37_NVIC_SENSE_SHFT               12

#define IOT_GPIO37_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x00B8)
#define IOT_GPIO37_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO37_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO37_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x00B8)
#define IOT_GPIO37_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO37_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO37_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x00B8)
#define IOT_GPIO37_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO37_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO37_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x00B8)
#define IOT_GPIO37_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO37_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO37_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x00B8)
#define IOT_GPIO37_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO37_DEBOUNCE_CNT_SHFT             0

/*config IOT_GPIO38 = PAD_UART_RTS = EINT_21 = IRQ_77 */
#define IOT_GPIO38_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x108)
#define IOT_GPIO38_NVIC_EN_MASK                  BIT(13)
#define IOT_GPIO38_NVIC_EN_SHFT                  13

#define IOT_GPIO38_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x188)
#define IOT_GPIO38_NVIC_CLR_EN_MASK              BIT(13)
#define IOT_GPIO38_NVIC_CLR_EN_SHFT              13

#define IOT_GPIO38_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x208)
#define IOT_GPIO38_NVIC_PEND_MASK                BIT(13)
#define IOT_GPIO38_NVIC_PEND_SHFT                13

#define IOT_GPIO38_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x288)
#define IOT_GPIO38_NVIC_CLR_PEND_MASK            BIT(13)
#define IOT_GPIO38_NVIC_CLR_PEND_SHFT            13

#define IOT_GPIO38_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x308)
#define IOT_GPIO38_NVIC_ACTIVE_MASK              BIT(13)
#define IOT_GPIO38_NVIC_ACTIVE_SHFT              13

#define IOT_GPIO38_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0208)
#define IOT_GPIO38_NVIC_SENSE_MASK               BIT(13)
#define IOT_GPIO38_NVIC_SENSE_SHFT               13

#define IOT_GPIO38_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x00BC)
#define IOT_GPIO38_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO38_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO38_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x00BC)
#define IOT_GPIO38_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO38_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO38_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x00BC)
#define IOT_GPIO38_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO38_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO38_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x00BC)
#define IOT_GPIO38_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO38_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO38_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x00BC)
#define IOT_GPIO38_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO38_DEBOUNCE_CNT_SHFT             0
/*config IOT_GPIO39 = PAD_UART_CTS = EINT_22 = IRQ_78 */
#define IOT_GPIO39_NVIC_EN_ADDR                  (MCU_CFG_NVIC_BASE + 0x108)
#define IOT_GPIO39_NVIC_EN_MASK                  BIT(14)
#define IOT_GPIO39_NVIC_EN_SHFT                  14

#define IOT_GPIO39_NVIC_CLR_EN_ADDR              (MCU_CFG_NVIC_BASE + 0x188)
#define IOT_GPIO39_NVIC_CLR_EN_MASK              BIT(14)
#define IOT_GPIO39_NVIC_CLR_EN_SHFT              14

#define IOT_GPIO39_NVIC_PEND_ADDR                (MCU_CFG_NVIC_BASE + 0x208)
#define IOT_GPIO39_NVIC_PEND_MASK                BIT(14)
#define IOT_GPIO39_NVIC_PEND_SHFT                14

#define IOT_GPIO39_NVIC_CLR_PEND_ADDR            (MCU_CFG_NVIC_BASE + 0x288)
#define IOT_GPIO39_NVIC_CLR_PEND_MASK            BIT(14)
#define IOT_GPIO39_NVIC_CLR_PEND_SHFT            14

#define IOT_GPIO39_NVIC_ACTIVE_ADDR              (MCU_CFG_NVIC_BASE + 0x308)
#define IOT_GPIO39_NVIC_ACTIVE_MASK              BIT(14)
#define IOT_GPIO39_NVIC_ACTIVE_SHFT              14

#define IOT_GPIO39_NVIC_SENSE_ADDR               (MCU_CFG_NVIC_SENSE_BASE + 0x0208)
#define IOT_GPIO39_NVIC_SENSE_MASK               BIT(14)
#define IOT_GPIO39_NVIC_SENSE_SHFT               14

#define IOT_GPIO39_DEBOUNCE_EN_ADDR              (MCU_CFG_DEBOUNCE_BASE + 0x00C0)
#define IOT_GPIO39_DEBOUNCE_EN_MASK              BIT(9)
#define IOT_GPIO39_DEBOUNCE_EN_SHFT              9

#define IOT_GPIO39_DEBOUNCE_POLARITY_ADDR        (MCU_CFG_DEBOUNCE_BASE + 0x00C0)
#define IOT_GPIO39_DEBOUNCE_POLARITY_MASK        BIT(8)
#define IOT_GPIO39_DEBOUNCE_POLARITY_SHFT        8

#define IOT_GPIO39_DEBOUNCE_DUAL_ADDR            (MCU_CFG_DEBOUNCE_BASE + 0x00C0)
#define IOT_GPIO39_DEBOUNCE_DUAL_MASK            BIT(7)
#define IOT_GPIO39_DEBOUNCE_DUAL_SHFT            7

#define IOT_GPIO39_DEBOUNCE_PRESCALER_ADDR       (MCU_CFG_DEBOUNCE_BASE + 0x00C0)
#define IOT_GPIO39_DEBOUNCE_PRESCALER_MASK       BITS(4,6)
#define IOT_GPIO39_DEBOUNCE_PRESCALER_SHFT       4

#define IOT_GPIO39_DEBOUNCE_CNT_ADDR             (MCU_CFG_DEBOUNCE_BASE + 0x00C0)
#define IOT_GPIO39_DEBOUNCE_CNT_MASK             BITS(0,3)
#define IOT_GPIO39_DEBOUNCE_CNT_SHFT             0




typedef enum {
    eAon_GPIO = 8,
    eAon_EINT = 3,
    eAon_WIC = 3,
} ENUM_AON_DOMAIN_T;

typedef enum {
    ePAD_CONTROL_BY_N9 = 0,
    ePAD_CONTROL_BY_CM4 = 1,
} ENUM_TOP_PAD_CONTROL_T;

typedef struct {
    void (*GPIO_IDX_EINT_WIC_LISR)(ENUM_IOT_GPIO_NAME_T pin); //WIC5
} GPI_LISR_func;

typedef struct {
    __IO uint32_t CM4_EINT_CON[66];
         uint32_t reserved[251];              
    __IO uint32_t CM4_IRQ_SENS[4];
} EINT_CON_REGISTER_T;

#define EINT_CON_BASE       (0x8300c000)
#define EINT_CON            ((EINT_CON_REGISTER_T*)(EINT_CON_BASE))

typedef struct {
    __IO uint32_t CM4_IRQ_SENS[4];
} EINT_DEBOUCE_REGISTER_T;

#define EINT_DEBOUCE_BASE   (0x8300c200)
#define EINT_DEBOUCE        ((EINT_DEBOUCE_REGISTER_T*)(EINT_DEBOUCE_BASE))

typedef struct {
    GPI_LISR_func    gpiLisr_func;
} GPI_LISR_Struct;


typedef struct {
    hal_eint_number_t    eint_number;       //datasheet eint number
    ENUM_IOT_GPIO_NAME_T gpio_pin;          //pin number
    uint32_t             irq_number;        //nvic irq nubmer
    uint32_t             priority_number;   //nvic priority number
} hal_eint_gpio_map_t;
extern const hal_eint_gpio_map_t eint_gpio_table[];
extern const uint32_t hal_eint_count_max;


INT32 halGPIO_ConfDirection(UINT32 GPIO_pin, UINT8 outEnable);
INT32 halGPO_Write(UINT32 GPIO_pin, UINT8 writeValue);

INT32 halGPIO_PullUp_SET(UINT32 GPIO_pin);
INT32 halGPIO_PullUp_RESET(UINT32 GPIO_pin);
INT32 halGPIO_PullDown_SET(UINT32 GPIO_pin);
INT32 halGPIO_PullDown_RESET(UINT32 GPIO_pin);
INT32 halGPIO_SetDriving(UINT32 GPIO_pin, UINT8 GPIO_driving);
INT32 halGPIO_GetDriving(UINT32 GPIO_pin, UINT8 *GPIO_driving);

UINT8 halgetGPIO_DOUT(UINT32 GPIO_pin);
UINT8 halgetGPIO_DIN(UINT32 GPIO_pin);
UINT8 halgetGPIO_OutEnable(UINT32 GPIO_pin);

INT32 halRegGPInterrupt(UINT32 pad_name,
                        UINT8 edgeLevelTrig,
                        UINT8 debounceEnable,
                        UINT8 polarity,
                        UINT8 dual,
                        UINT8 prescaler,
                        UINT8 u1PrescalerCount,
                        void (*USER_GPI_Handler)(ENUM_IOT_GPIO_NAME_T pin));
void topPAD_Control_GPIO(UINT32 GPIO_pin, ENUM_TOP_PAD_CONTROL_T ePadControl);

INT32 halRegGPInterrupt_set_triggermode(UINT32 pad_name,UINT8 edgeLevelTrig,UINT8 polarity,UINT8 dual);
INT32 halRegGPInterrupt_set_debounce(UINT32 pad_name,UINT8 debounceEnable,UINT8 prescaler,uint8_t u1PrescalerCount);
INT32 halRegGPInterrupt_mask(UINT32 pad_name);
INT32 halRegGPInterrupt_unmask(UINT32 pad_name);
INT32 halRegGP_set_software_trigger(UINT32 pad_name);
INT32 halRegGP_clear_software_trigger(UINT32 pad_name);


#endif /*__HAL_GPIO_7687_H__*/
#endif

