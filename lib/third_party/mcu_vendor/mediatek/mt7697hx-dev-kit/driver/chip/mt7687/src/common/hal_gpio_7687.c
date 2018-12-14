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

#include <stdio.h>
#include "type_def.h"
#include "hal_gpio_7687.h"
#include "nvic.h"
#include "hal_pinmux.h"
#include "hal_nvic.h"
#include "pinmux.h"
#include "hal_log.h"
#include "assert.h"
#include "hal_gpt.h"

static EINT_CON_REGISTER_T* eint_con_register = (EINT_CON_REGISTER_T*)(EINT_CON_BASE);
static EINT_DEBOUCE_REGISTER_T* eint_dbc_register = (EINT_DEBOUCE_REGISTER_T*)(EINT_DEBOUCE_BASE);

INT32 halGPIO_ConfDirection(UINT32 GPIO_pin, UINT8 outEnable)
{

    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;
    switch (no) {
        case 0:

            //config GPIO direction
            if (outEnable == 1) {
                if (remainder <= 26) {
                    DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_IES0) &= ~(1 << remainder);
                } else if (remainder == 27) {
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CLK) &= ~(0x2);
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CLK) |= 0x800;
                } else if (remainder == 28) {
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CMD) &= ~(0x2);
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CMD) |= 0x800;
                } else if (remainder == 29) {
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA3) &= ~(0x2);
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA3) |= 0x800;
                } else if (remainder == 30) {
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA2) &= ~(0x2);
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA2) |= 0x800;
                } else if (remainder == 31) {
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA1) &= ~(0x2);
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA1) |= 0x800;
                }

                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_OE1_SET, (1 << remainder));
            } else {

                if (remainder <= 26) {
                    DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_IES0) |= (1 << remainder);
                } else if (remainder == 27) {
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CLK) |= 0x802;
                } else if (remainder == 28) {
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CMD) |= 0x802;
                } else if (remainder == 29) {
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA3) |= 0x802;
                } else if (remainder == 30) {
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA2) |= 0x802;
                } else if (remainder == 31) {
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA1) |= 0x802;
                }


                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_OE1_RESET, (1 << remainder));
            }
            break;
        case 1:

            //config GPIO direction
            if (outEnable == 1) {
                if (remainder > 0) {
                    DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_IES1) &= ~(1 << remainder);
                } else {
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA0) &= ~(0x2);
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA0) |= 0x800;
                }

                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_OE2_SET, (1 << remainder));
            } else {

                if (remainder > 0) {
                    DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_IES1) |= (1 << remainder);
                } else {
                    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA0) |= 0x802;
                }

                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_OE2_RESET, (1 << remainder));
            }
            break;
        default:
            return -1;
    }
    return 0;
}


INT32 halGPO_Write(UINT32 GPIO_pin, UINT8 writeValue)
{
    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;
    switch (no) {
        case 0:

            if (writeValue) {
                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_DOUT1_SET, (1 << remainder));
            } else {
                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_DOUT1_RESET, (1 << remainder));
            }
            break;
        case 1:

            if (writeValue) {
                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_DOUT2_SET, (1 << remainder));
            } else {
                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_DOUT2_RESET, (1 << remainder));
            }
            break;
        default:
            return -1;
    }
    return 0;
}





UINT8 halgetGPIO_DOUT(UINT32 GPIO_pin)
{
    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;
    UINT8 dout = 0;

    switch (no) {
        case 0:
            dout = ((DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_DOUT1) >> remainder) & 1);
            break;
        case 1:
            dout = ((DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_DOUT2) >> remainder) & 1);
            break;
        default:
            return 0;
    }
    return dout ;
}
UINT8 halgetGPIO_DIN(UINT32 GPIO_pin)
{
    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;
    UINT8 din = 0;
    switch (no) {

        case 0:
            din = ((DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_DIN1) >> remainder) & 1);
            break;
        case 1:
            din = ((DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_DIN2) >> remainder) & 1);
            break;
        default:
            return 0;

    }
    return din;
}
UINT8 halgetGPIO_OutEnable(UINT32 GPIO_pin)
{
    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;
    UINT8 outEnable = 0;

    switch (no) {
        case 0:
            outEnable = ((DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_OE1) >> remainder) & 1);
            break;
        case 1:
            outEnable = ((DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_OE2) >> remainder) & 1);
            break;
        default:
            return 0;
    }
    return outEnable;
}


INT32 halGPIO_PullUp_SET(UINT32 GPIO_pin)
{
    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;
    halGPIO_PullDown_RESET(GPIO_pin);
    switch (no) {
        case 0:
            if (remainder <= 26) {
                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_PU1_SET, (1 << remainder));
            } else if (remainder == 27) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CLK) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CLK) |= 0x804;
            } else if (remainder == 28) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CMD) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CMD) |= 0x804;
            } else if (remainder == 29) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA3) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA3) |= 0x804;
            } else if (remainder == 30) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA2) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA2) |= 0x804;
            } else if (remainder == 31) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA1) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA1) |= 0x804;
            }
            break;
        case 1:
            if (!remainder) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA0) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA0) |= 0x804;
            } else {
                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_PU2_SET, (1 << remainder));
            }
            break;
        default:
            return -1;
    }
    return 0;
}

INT32 halGPIO_PullUp_RESET(UINT32 GPIO_pin)
{
    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;

    switch (no) {
        case 0:
            if (remainder <= 26) {
                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_PU1_RESET, (1 << remainder));
            } else if (remainder == 27) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CLK) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CLK) |= (1 << 0xB);
            } else if (remainder == 28) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CMD) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CMD) |= (1 << 0xB);
            } else if (remainder == 29) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA3) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA3) |= (1 << 0xB);
            } else if (remainder == 30) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA2) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA2) |= (1 << 0xB);
            } else if (remainder == 31) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA1) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA1) |= (1 << 0xB);
            }
            break;
        case 1:
            if (!remainder) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA0) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA0) |= (1 << 0xB);
            } else {
                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_PU2_RESET, (1 << remainder));
            }
            break;
        default:
            return -1;
    }
    return 0;
}

INT32 halGPIO_PullDown_SET(UINT32 GPIO_pin)
{
    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;
    halGPIO_PullUp_RESET(GPIO_pin);
    switch (no) {
        case 0:
            if (remainder <= 26) {
                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_PD1_SET, (1 << remainder));
            } else if (remainder == 27) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CLK) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CLK) |= 0x814;
            } else if (remainder == 28) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CMD) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CMD) |= 0x814;
            } else if (remainder == 29) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA3) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA3) |= 0x814;
            } else if (remainder == 30) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA2) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA2) |= 0x814;
            } else if (remainder == 31) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA1) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA1) |= 0x814;
            }
            break;
        case 1:
            if (!remainder) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA0) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA0) |= 0x814;
            } else {
                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_PD2_SET, (1 << remainder));
            }
            break;
        default:
            return -1;

    }
    return 0;
}

INT32 halGPIO_PullDown_RESET(UINT32 GPIO_pin)
{

    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;

    switch (no) {
        case 0:
            if (remainder <= 26) {
                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_PD1_RESET, (1 << remainder));
            } else if (remainder == 27) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CLK) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CLK) |= (1 << 0xB);
            } else if (remainder == 28) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CMD) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CMD) |= (1 << 0xB);
            } else if (remainder == 29) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA3) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA3) |= (1 << 0xB);
            } else if (remainder == 30) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA2) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA2) |= (1 << 0xB);
            } else if (remainder == 31) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA1) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA1) |= (1 << 0xB);
            }
            break;
        case 1:
            if (!remainder) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA0) &= ~(0x7 << 2);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA0) |= (1 << 0xB);
            } else {
                DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_PD2_RESET, (1 << remainder));
            }
            break;
        default:
            return -1;

    }
    return 0;
}


INT32 halGPIO_SetDriving(UINT32 GPIO_pin, UINT8 GPIO_driving)
{
    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 16;
    remainder = GPIO_pin % 16;

    switch (no) {
        case 0:
            DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_PADDRV1) &= ~(0x3 << (remainder * 2));
            DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_PADDRV1) |= ((GPIO_driving / 4 - 1) << (remainder * 2));
            break;
        case 1:
            if (remainder < 11) {
                DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_PADDRV2) &= ~(0x3 << (remainder * 2));
                DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_PADDRV2) |= ((GPIO_driving / 4 - 1) << (remainder * 2));
            } else if (remainder == 11) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CLK) &= ~(0x7 << 6);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CLK) |= ((GPIO_driving / 2 - 1) << 6);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CLK) |= (1 << 0xB);
            } else if (remainder == 12) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CMD) &= ~(0x7 << 6);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CMD) |= ((GPIO_driving / 2 - 1) << 6);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CMD) |= (1 << 0xB);
            } else if (remainder == 13) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA3) &= ~(0x7 << 6);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA3) |= ((GPIO_driving / 2 - 1) << 6);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA3) |= (1 << 0xB);
            } else if (remainder == 14) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA2) &= ~(0x7 << 6);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA2) |= ((GPIO_driving / 2 - 1) << 6);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA2) |= (1 << 0xB);
            } else if (remainder == 15) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA1) &= ~(0x7 << 6);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA1) |= ((GPIO_driving / 2 - 1) << 6);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA1) |= (1 << 0xB);
            }
            break;
        case 2:
            if (!remainder) {
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA0) &= ~(0x7 << 6);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA0) |= ((GPIO_driving / 2 - 1) << 6);
                DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA0) |= (1 << 0xB);
            } else {
                DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_PADDRV3) &= ~(0x3 << (remainder * 2));
                DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_PADDRV3) |= ((GPIO_driving / 4 - 1) << (remainder * 2));
            }
            break;
        case 3:
            DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_PADDRV4) &= ~(0x3 << (remainder * 2));
            DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_PADDRV4) |= ((GPIO_driving / 4 - 1) << (remainder * 2));
            break;
        default:
            return -1;
    }
    return 0;
}


INT32 halGPIO_GetDriving(UINT32 GPIO_pin, UINT8 *GPIO_driving)
{
    UINT16 no;
    UINT16 remainder;
    UINT32 temp;
    no = GPIO_pin / 16;
    remainder = GPIO_pin % 16;

    switch (no) {
        case 0:
            temp = DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_PADDRV1);
            temp &= (0x3 << (remainder * 2));
            *GPIO_driving = (1 + (temp >> (remainder * 2))) * 4;
            break;
        case 1:
            if (remainder < 11) {
                temp = DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_PADDRV2);
                temp &= (0x3 << (remainder * 2));
                *GPIO_driving = (1 + (temp >> (remainder * 2))) * 4;
            } else if (remainder == 11) {
                temp = DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CLK);
                temp &= 0x1C0;
                *GPIO_driving = ((temp >> 0x6) + 1) * 2;
            } else if (remainder == 12) {
                temp = DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_CMD);
                temp &= 0x1C0;
                *GPIO_driving = ((temp >> 0x6) + 1) * 2;
            } else if (remainder == 13) {
                temp = DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA3);
                temp &= 0x1C0;
                *GPIO_driving = ((temp >> 0x6) + 1) * 2;
            } else if (remainder == 14) {
                temp = DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA2);
                temp &= 0x1C0;
                *GPIO_driving = ((temp >> 0x6) + 1) * 2;
            } else if (remainder == 15) {
                temp = DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA1);
                temp &= 0x1C0;
                *GPIO_driving = ((temp >> 0x6) + 1) * 2;
            }
            break;
        case 2:
            if (!remainder) {
                temp = DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + IOT_GPIO_SDIO_DATA0);
                temp &= 0x1C0;
                *GPIO_driving = ((temp >> 0x6) + 1) * 2;
            } else {
                temp = DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_PADDRV3);
                temp &= (0x3 << (remainder * 2));
                *GPIO_driving = (1 + (temp >> (remainder * 2))) * 4;
            }
            break;
        case 3:
            temp = DRV_Reg32(IOT_GPIO_AON_BASE + IOT_GPIO_PADDRV4);
            temp &= (0x3 << (remainder * 2));
            *GPIO_driving = (1 + (temp >> (remainder * 2))) * 4;
            break;
        default:
            return -1;
    }
    return 0;
}

void topPAD_Control_GPIO(UINT32 GPIO_pin, ENUM_TOP_PAD_CONTROL_T ePadControl)
{

    UINT16 no;
    UINT16 remainder;
    no = GPIO_pin / 32;
    remainder = GPIO_pin % 32;

    if (ePAD_CONTROL_BY_CM4 == ePadControl) {

        switch (no) {
            case 0:
                DRV_Reg32(IOT_GPIO_PAD_CTRL0) |= (1 << remainder);
                break;
            case 1:
                DRV_Reg32(IOT_GPIO_PAD_CTRL1) |= (1 << remainder);
                break;
            default:
                break;
        }

    } else if (ePAD_CONTROL_BY_N9 == ePadControl) {
        switch (no) {
            case 0:
                DRV_Reg32(IOT_GPIO_PAD_CTRL0) &= ~(1 << remainder);
                break;
            case 1:
                DRV_Reg32(IOT_GPIO_PAD_CTRL1) &= ~(1 << remainder);
                break;
            default:
                break;
        }

    }
}


#if 1

static GPI_LISR_Struct LISR;

void drvGPIO_IDX_EINT_WIC_LISR(hal_nvic_irq_t irq_number)
{
    uint32_t i;
    uint32_t pin_number = 0xff;

    for (i = 0; i < hal_eint_count_max; i++) {
        if (eint_gpio_table[i].irq_number == irq_number) {
            pin_number = eint_gpio_table[i].gpio_pin;
            break;
        }
        if ((hal_eint_count_max - 1) == i) {
            log_hal_info("[Eint]drvGPIO_IDX_EINT_WIC_LISR error, the irq_number=%d\r\n", irq_number);
            assert(0);
        }
    }
    
    LISR.gpiLisr_func.GPIO_IDX_EINT_WIC_LISR((ENUM_IOT_GPIO_NAME_T)pin_number);
}


INT32 halRegGPInterrupt(UINT32 pad_name,
                        UINT8 edgeLevelTrig,
                        UINT8 debounceEnable,
                        UINT8 polarity,
                        UINT8 dual,
                        UINT8 prescaler,
                        UINT8 u1PrescalerCount,
                        void (*USER_GPI_Handler)(ENUM_IOT_GPIO_NAME_T pin))
{
    uint32_t i;
    uint32_t irq_number = 0xff;
    uint32_t priority_number = 0xff;
    uint32_t int_index;
    uint32_t wdata = 0;
    uint32_t rdata = 0;
    uint32_t reg_index;
    uint32_t reg_shift;
    
    for (i = 0; i < hal_eint_count_max; i++) {
        if (eint_gpio_table[i].gpio_pin == pad_name) {
            irq_number      = eint_gpio_table[i].irq_number;
            priority_number = eint_gpio_table[i].priority_number;
            break;
        }
        if ((hal_eint_count_max - 1) == i) {
            log_hal_info("[Eint]halRegGPInterrupt error, the padname=%d\r\n", pad_name);
            return -1;
        }
    }

    NVIC_DisableIRQ((IRQn_Type)irq_number);
    halPinmuxTopOnPadFunc((ENUM_PAD_NAME_T)pad_name, 3);     //set value 3 is for EINT & WIC

    //edge or level trigger setting
    reg_index = irq_number/32;
    reg_shift = irq_number%32;
    rdata = eint_dbc_register->CM4_IRQ_SENS[reg_index];
    rdata &= ~(1<<reg_shift);
    rdata |= (edgeLevelTrig<<reg_shift);
    eint_dbc_register->CM4_IRQ_SENS[reg_index] = rdata;
    int_index = irq_number - 30;

    wdata = 0 + (prescaler<<4) + (dual<<7) + (polarity<<8) + (debounceEnable<<9);
    eint_con_register->CM4_EINT_CON[int_index] = wdata;
    hal_gpt_delay_us(125);
    NVIC_ClearPendingIRQ((IRQn_Type)irq_number);
    wdata = u1PrescalerCount + (prescaler<<4) + (dual<<7) + (polarity<<8) + (debounceEnable<<9);
    eint_con_register->CM4_EINT_CON[int_index] = wdata;
    //printf("irq_number = %d, edgeLevelTrig = %d\r\n",irq_number, edgeLevelTrig);
    //printf(" rdata=0x%x, reg_shift=%d\r\n", rdata, reg_shift);
    //printf("eint_cont[%d]=%x\r\n", int_index,eint_con_register->CM4_EINT_CON[int_index]);
    //printf("CM4_IRQ_SENS[%d]=%x\r\n\r\n", reg_index,eint_dbc_register->CM4_IRQ_SENS[reg_index]);

    LISR.gpiLisr_func.GPIO_IDX_EINT_WIC_LISR = USER_GPI_Handler;
    hal_nvic_register_isr_handler((IRQn_Type)irq_number, drvGPIO_IDX_EINT_WIC_LISR);
    NVIC_SetPriority((IRQn_Type)irq_number, priority_number);

    return 0;
}


INT32 halRegGPInterrupt_set_triggermode(UINT32 pad_name,UINT8 edgeLevelTrig,UINT8 polarity,UINT8 dual)
{

    uint32_t i;
    uint32_t irq_number = 0xff;
    uint32_t int_index;
    uint32_t wdata;
    uint32_t rdata;
    uint32_t reg_index;
    uint32_t reg_shift;

    for (i = 0; i < hal_eint_count_max; i++) {
        if (eint_gpio_table[i].gpio_pin == pad_name) {
            irq_number      = eint_gpio_table[i].irq_number;
            break;
        }
        if ((hal_eint_count_max - 1) == i) {
            log_hal_info("[Eint]halRegGPInterrupt_set_triggermode error, the padname=%d\r\n", pad_name);
            return -1;
        }
    }


    //edge or level trigger setting
    reg_index = irq_number/32;
    reg_shift = irq_number%32;
    rdata = eint_dbc_register->CM4_IRQ_SENS[reg_index];
    rdata &= ~(1<<reg_shift);
    rdata |= edgeLevelTrig<<reg_shift;
    eint_dbc_register->CM4_IRQ_SENS[reg_index] = rdata;

    int_index = irq_number - 30;
    wdata = eint_con_register->CM4_EINT_CON[int_index] ;
    wdata &= ~((1<<7) + (1<<8));
    wdata |= (dual<<7) + (polarity<<8);
    eint_con_register->CM4_EINT_CON[int_index] = wdata;


    hal_gpt_delay_us(125);

    return 0;
}


INT32 halRegGPInterrupt_set_debounce(UINT32 pad_name,UINT8 debounceEnable,UINT8 prescaler,uint8_t u1PrescalerCount)
{

    uint32_t i;
    uint32_t irq_number = 0xff;
    uint32_t int_index;
    uint32_t wdata;

    for (i = 0; i < hal_eint_count_max; i++) {
        if (eint_gpio_table[i].gpio_pin == pad_name) {
            irq_number      = eint_gpio_table[i].irq_number;
            break;
        }
        if ((hal_eint_count_max - 1) == i) {
            log_hal_info("[Eint]halRegGPInterrupt_set_debounce error, the padname=%d\r\n", pad_name);
            return -1;
        }
    }


    int_index = irq_number - 30;
    wdata = eint_con_register->CM4_EINT_CON[int_index] ;
    wdata &= ~((1<<4) + (1<<9) + 0xff);
    wdata |= 0 + (prescaler<<4) +  + (debounceEnable<<9); 
    eint_con_register->CM4_EINT_CON[int_index] = wdata;
    hal_gpt_delay_us(125);
    NVIC_ClearPendingIRQ((IRQn_Type)irq_number);

    wdata |= u1PrescalerCount + (prescaler<<4) +  + (debounceEnable<<9); 
    eint_con_register->CM4_EINT_CON[int_index] = wdata;

        
    return 0;
}


INT32 halRegGPInterrupt_mask(UINT32 pad_name)
{
    uint32_t i;
    uint32_t irq_number = 0xff;

    for (i = 0; i < hal_eint_count_max; i++) {
        if (eint_gpio_table[i].gpio_pin == pad_name) {
            irq_number = eint_gpio_table[i].irq_number;
            break;
        }
        if ((hal_eint_count_max - 1) == i) {
            log_hal_info("[Eint]halRegGPInterrupt_mask error, the padname=%d\r\n", pad_name);
            return -1;
        }
    }
    
    NVIC_DisableIRQ((IRQn_Type)irq_number);

    return 0;
}

INT32 halRegGPInterrupt_unmask(UINT32 pad_name)
{
    uint32_t i;
    uint32_t irq_number = 0xff;

    for (i = 0; i < hal_eint_count_max; i++) {
        if (eint_gpio_table[i].gpio_pin == pad_name) {
            irq_number = eint_gpio_table[i].irq_number;
            break;
        }
        if ((hal_eint_count_max - 1) == i) {
            log_hal_info("[Eint]halRegGPInterrupt_unmask error, the padname=%d\r\n", pad_name);
            return -1;
        }
    }
    
    NVIC_EnableIRQ((IRQn_Type)irq_number);

    return 0;
}

static void software_trigger_irq(UINT32 irq_number)
{
    NVIC->STIR = (irq_number << NVIC_STIR_INTID_Pos) & NVIC_STIR_INTID_Msk;
}

INT32 halRegGP_set_software_trigger(UINT32 pad_name)
{

    uint32_t i;
    uint32_t irq_number = 0xff;

    for (i = 0; i < hal_eint_count_max; i++) {
        if (eint_gpio_table[i].gpio_pin == pad_name) {
            irq_number = eint_gpio_table[i].irq_number;
            break;
        }
        if ((hal_eint_count_max - 1) == i) {
            log_hal_info("[Eint]halRegGP_set_software_trigger error, the padname=%d\r\n", pad_name);
            return -1;
        }
    }

        
    software_trigger_irq((IRQn_Type)irq_number);

    return 0;
    
}

INT32 halRegGP_clear_software_trigger(UINT32 pad_name)
{
    uint32_t i;
    uint32_t irq_number = 0xff;

    for (i = 0; i < hal_eint_count_max; i++) {
        if (eint_gpio_table[i].gpio_pin == pad_name) {
            irq_number = eint_gpio_table[i].irq_number;
            break;
        }
        if ((hal_eint_count_max - 1) == i) {
            log_hal_info("[Eint]halRegGP_clear_software_trigger error, the padname=%d\r\n", pad_name);
            return -1;
        }
    }
        
    NVIC_ClearPendingIRQ((IRQn_Type)irq_number);

    return 0;
}

#endif
#endif

