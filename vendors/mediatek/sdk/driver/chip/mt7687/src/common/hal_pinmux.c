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
** $Log: hal_pinmux.c $
**
**
**
*/

/*
    Porting from legacy/driver/hal_GPIO.c
*/

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "hal_gpio.h"
#ifdef HAL_GPIO_MODULE_ENABLED

#include <stdio.h>
#include <string.h>
#include "type_def.h"
#include "top.h"
#include "hal_pinmux.h"
#include "hal_gpio_7687.h"


#define HAL_GPIO_DIRECTION_IN 0
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

VOID halPinmuxTopOnPadFunc(ENUM_PAD_NAME_T ePadIndex, UINT8 ucFunc) //pinmux setting by gen_pmux_top_aon
{
    switch (ePadIndex) {
        case ePAD_ANTSEL0:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) &= ~PAD_ANTSEL0_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) |= (ucFunc << 0);
            break;
        case ePAD_ANTSEL1:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) &= ~PAD_ANTSEL1_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) |= (ucFunc << 4);
            break;
        case ePAD_ANTSEL2:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) &= ~PAD_ANTSEL2_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) |= (ucFunc << 8);
            break;
        case ePAD_ANTSEL3:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) &= ~PAD_ANTSEL3_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) |= (ucFunc << 12);
            break;
        case ePAD_ANTSEL4:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) &= ~PAD_ANTSEL4_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) |= (ucFunc << 16);
            break;
        case ePAD_ANTSEL5:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) &= ~PAD_ANTSEL5_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) |= (ucFunc << 20);
            break;
        case ePAD_ANTSEL6:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) &= ~PAD_ANTSEL6_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) |= (ucFunc << 24);
            break;
        case ePAD_ANTSEL7:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) &= ~PAD_ANTSEL7_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) |= (ucFunc << 28);
            break;
        case ePAD_PERST_N:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x2C) &= ~PAD_PERST_N_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x2C) |= (ucFunc << 0);
            break;
        case ePAD_WAKE_N:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x2C) &= ~PAD_WAKE_N_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x2C) |= (ucFunc << 4);
            break;
        case ePAD_CLK_REQ_N:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x2C) &= ~PAD_CLK_REQ_N_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x2C) |= (ucFunc << 8);
            break;
        case ePAD_SDIO_CLK:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x2C) &= ~PAD_SDIO_CLK_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x2C) |= (ucFunc << 12);
            if (2 == ucFunc || 4 == ucFunc || 5 == ucFunc) {
                halGPIO_ConfDirection((UINT32)ePadIndex, HAL_GPIO_DIRECTION_IN);
            }
            break;
        case ePAD_SDIO_CMD:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x2C) &= ~PAD_SDIO_CMD_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x2C) |= (ucFunc << 16);
            if (4 == ucFunc || 5 == ucFunc) {
                halGPIO_ConfDirection((UINT32)ePadIndex, HAL_GPIO_DIRECTION_IN);
            }
            break;
        case ePAD_SDIO_DAT3:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x2C) &= ~PAD_SDIO_DAT3_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x2C) |= (ucFunc << 20);
            if (6 == ucFunc) {
                halGPIO_ConfDirection((UINT32)ePadIndex, HAL_GPIO_DIRECTION_IN);
            }
            break;
        case ePAD_SDIO_DAT2:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x2C) &= ~PAD_SDIO_DAT2_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x2C) |= (ucFunc << 24);
            if (4 == ucFunc || 7 == ucFunc) {
                halGPIO_ConfDirection((UINT32)ePadIndex, HAL_GPIO_DIRECTION_IN);
            }
            break;
        case ePAD_SDIO_DAT1:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x2C) &= ~PAD_SDIO_DAT1_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x2C) |= (ucFunc << 28);
            if (4 == ucFunc || 6 == ucFunc) {
                halGPIO_ConfDirection((UINT32)ePadIndex, HAL_GPIO_DIRECTION_IN);
            }
            break;
        case ePAD_SDIO_DAT0:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x30) &= ~PAD_SDIO_DAT0_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x30) |= (ucFunc << 0);
            if (4 == ucFunc || 6 == ucFunc) {
                halGPIO_ConfDirection((UINT32)ePadIndex, HAL_GPIO_DIRECTION_IN);
            }
            break;
        case ePAD_GPIO0:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x30) &= ~PAD_GPIO0_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x30) |= (ucFunc << 4);
            break;
        case ePAD_GPIO1:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x30) &= ~PAD_GPIO1_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x30) |= (ucFunc << 8);
            break;
        case ePAD_UART_DBG:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x30) &= ~PAD_UART_DBG_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x30) |= (ucFunc << 12);
            break;
        case ePAD_UART_RX:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x30) &= ~PAD_UART_RX_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x30) |= (ucFunc << 16);
            break;
        case ePAD_UART_TX:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x30) &= ~PAD_UART_TX_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x30) |= (ucFunc << 20);
            break;
        case ePAD_UART_RTS:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x30) &= ~PAD_UART_RTS_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x30) |= (ucFunc << 24);
            break;
        case ePAD_UART_CTS:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x30) &= ~PAD_UART_CTS_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x30) |= (ucFunc << 28);
            break;
        case ePAD_WF_RF_DIS_B:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x3C) &= ~PAD_WF_RF_DIS_B_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x3C) |= (ucFunc << 4);
            break;
        case ePAD_BT_RF_DIS_B:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x3C) &= ~PAD_BT_RF_DIS_B_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x3C) |= (ucFunc << 8);
            break;
        case ePAD_WF_LED_B:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x3C) &= ~PAD_WF_LED_B_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x3C) |= (ucFunc << 12);
            break;
        case ePAD_BT_LED_B:
            //config GPIO pinmux
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x3C) &= ~PAD_BT_LED_B_PINMUX_AON_MASK;
            DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x3C) |= (ucFunc << 16);
            break;
        default:
            break;
    }
}
#endif

