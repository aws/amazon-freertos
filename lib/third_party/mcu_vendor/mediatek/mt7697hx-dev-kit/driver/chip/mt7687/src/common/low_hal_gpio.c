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

/****************************************************************************
    Module Name:
    GPIO

    Abstract:
    GPIO.

    Revision History:
    Who         When            What
    --------    ----------      ------------------------------------------
***************************************************************************/

#include "hal_eint.h"
#include "hal_gpio.h"
#if defined(HAL_EINT_MODULE_ENABLED) || defined(HAL_GPIO_MODULE_ENABLED)

#include "type_def.h"

#include "low_hal_gpio.h"
#include "hal_gpio_7687.h"
#include "hal_log.h"

typedef struct{
    uint32_t reg_data;
    uint32_t reg_addr;
} gpio_backup_restore_t;

volatile static gpio_backup_restore_t gpio_bakup_table[] = {
    /*reg_data, reg_addr*/
    {0, 0x8300B000},    //GPIO_PU1
    {0, 0x8300B010},    //GPIO_PU2
    {0, 0x8300B020},    //GPIO_PU3
    {0, 0x8300B030},    //GPIO_PD1
    {0, 0x8300B040},    //GPIO_PD2
    {0, 0x8300B050},    //GPIO_PD3
    {0, 0x8300B060},    //GPIO_DOUT1
    {0, 0x8300B070},    //GPIO_DOUT2
    {0, 0x8300B080},    //GPIO_DOUT3
    {0, 0x8300B090},    //GPIO_OE1
    {0, 0x8300B0A0},    //GPIO_OE2
    {0, 0x8300B0B0},    //GPIO_OE3
    /*{0, 0x8300B0C0},    //GPIO_DIN1
    {0, 0x8300B0C4},    //GPIO_DIN2
    {0, 0x8300B0C8},    //GPIO_DIN3*/
    {0, 0x8300B0D0},    //PADDRV1
    {0, 0x8300B0D4},    //PADDRV2
    {0, 0x8300B0D8},    //PADDRV3
    {0, 0x8300B0DC},    //PADDRV4
    {0, 0x8300B0E0},    //PADDRV5
    {0, 0x8300B0F0},    //PADCTRL1
    {0, 0x8300B0F4},    //PADCTRL2
    {0, 0x8300B100},    //PAD_GPIO_IES0
    {0, 0x8300B104},    //PAD_GPIO_IES1
    {0, 0x8300B108},    //PAD_GPIO_IES2
    /*{0, 0x81023020},    //PINMUX_AON_SEL0
    {0, 0x81023024},    //PINMUX_AON_SEL1
    {0, 0x81023028},    //PINMUX_AON_SEL2
    {0, 0x8102302c},    //PINMUX_AON_SEL3
    {0, 0x81023030},    //PINMUX_AON_SEL4
    {0, 0x81023034},    //PINMUX_AON_SEL5
    {0, 0x81023038},    //PINMUX_AON_SEL6
    {0, 0x8102303c},    //PINMUX_AON_SEL7
    {0, 0x81023040},    //PINMUX_AON_SEL8
    {0, 0x81023044},    //PINMUX_AON_SEL9
    {0, 0x81023048},    //PINMUX_SDIO_CLK_CTL
    {0, 0x8102304c},    //PINMUX_SDIO_CMD_CTL
    {0, 0x81023050},    //PINMUX_SDIO_DAT0_CTL
    {0, 0x81023054},    //PINMUX_SDIO_DAT1_CTL
    {0, 0x81023058},    //PINMUX_SDIO_DAT2_CTL
    {0, 0x8102305c}     //PINMUX_SDIO_DAT3_CTL*/
};

int32_t gpio_direction(ENUM_IOT_GPIO_NAME_T gpio_name, ENUM_DIR_T direction)
{
    return halGPIO_ConfDirection(gpio_name, direction);
}


int32_t gpio_write(ENUM_IOT_GPIO_NAME_T gpio_name, uint8_t data)
{
    return halGPO_Write(gpio_name, data);
}



int32_t gpio_int(ENUM_IOT_GPIO_NAME_T       gpio_name,
                 ENUM_NVIC_SENSE_T          edgeLevelTrig,
                 ENUM_DEBOUNCE_EN_T         debounceEnable,
                 ENUM_DEBOUNCE_POL_T        polarity,
                 ENUM_DEBOUNCE_DUAL_T       dual,
                 ENUM_DEBOUNCE_PRESCALER_T  prescaler,
                 uint8_t                    u1PrescalerCount,
                 void (*callback)(ENUM_IOT_GPIO_NAME_T gpio_name))
{
    return halRegGPInterrupt(gpio_name, edgeLevelTrig, debounceEnable , polarity , dual , prescaler, u1PrescalerCount , callback);
}

int32_t eint_set_trigger_mode(ENUM_IOT_GPIO_NAME_T gpio_name,
                 ENUM_NVIC_SENSE_T          edgeLevelTrig,
                 ENUM_DEBOUNCE_POL_T        polarity,
                 ENUM_DEBOUNCE_DUAL_T       dual)
{
    return halRegGPInterrupt_set_triggermode(gpio_name,edgeLevelTrig, polarity, dual);
}

int32_t eint_set_debounce(ENUM_IOT_GPIO_NAME_T gpio_name,
                 ENUM_DEBOUNCE_EN_T         debounceEnable,
                  ENUM_DEBOUNCE_PRESCALER_T prescaler,
                 uint8_t                    u1PrescalerCount)
{
    return halRegGPInterrupt_set_debounce(gpio_name,debounceEnable, prescaler, prescaler);
}

int32_t eint_set_mask(ENUM_IOT_GPIO_NAME_T gpio_name)
{
    return halRegGPInterrupt_mask(gpio_name);
}

int32_t eint_set_unmask(ENUM_IOT_GPIO_NAME_T gpio_name)
{
    return halRegGPInterrupt_unmask(gpio_name);
}

int32_t eint_set_software_trigger(ENUM_IOT_GPIO_NAME_T gpio_name)
{
    return halRegGP_set_software_trigger(gpio_name);
}

int32_t eint_clear_software_trigger(ENUM_IOT_GPIO_NAME_T gpio_name)
{
    return halRegGP_clear_software_trigger(gpio_name);
}


gpio_status gpio_get_status(ENUM_IOT_GPIO_NAME_T gpio_name)
{
    gpio_status p;
    p.GPIO_PIN = gpio_name;
    p.GPIO_OUTEN = halgetGPIO_OutEnable(gpio_name);
    p.GPI_DIN = halgetGPIO_DIN(gpio_name);
    p.GPO_DOUT = halgetGPIO_DOUT(gpio_name);
    return p;
}

int32_t gpio_PullUp(ENUM_IOT_GPIO_NAME_T gpio_name)
{
    INT32 ret = 0;
    topPAD_Control_GPIO(gpio_name, ePAD_CONTROL_BY_CM4); //let gpio control by cm4
    ret = halGPIO_PullUp_SET(gpio_name);
    return ret;
}

int32_t gpio_PullDown(ENUM_IOT_GPIO_NAME_T gpio_name)
{
    INT32 ret = 0;
    topPAD_Control_GPIO(gpio_name, ePAD_CONTROL_BY_CM4); //let gpio control by cm4
    ret = halGPIO_PullDown_SET(gpio_name);
    return ret;
}
int32_t gpio_PullDisable(ENUM_IOT_GPIO_NAME_T gpio_name)
{
    INT32 ret = 0;
    topPAD_Control_GPIO(gpio_name, ePAD_CONTROL_BY_CM4); //let gpio control by cm4
    ret = halGPIO_PullDown_RESET(gpio_name);
    ret = halGPIO_PullUp_RESET(gpio_name);
    return ret;
}
int32_t gpio_SetDriving(ENUM_IOT_GPIO_NAME_T gpio_name, uint8_t driving)
{
    INT32 ret = 0;
    ret = halGPIO_SetDriving(gpio_name, driving);
    return ret;
}

int32_t gpio_GetDriving(ENUM_IOT_GPIO_NAME_T gpio_name, uint8_t *driving)
{
    INT32 ret = 0;
    ret = halGPIO_GetDriving(gpio_name, driving);
    return ret;
}

void gpio_backup_all_registers(void)
{
    uint32_t i;

    for(i=0; i<(sizeof(gpio_bakup_table)/sizeof(gpio_backup_restore_t));i++) {
        gpio_bakup_table[i].reg_data = DRV_Reg32(gpio_bakup_table[i].reg_addr);
    }

}

void gpio_restore_all_registers(void)
{
    uint32_t i;

    for(i=0; i<(sizeof(gpio_bakup_table)/sizeof(gpio_backup_restore_t));i++) {
        DRV_Reg32(gpio_bakup_table[i].reg_addr) = gpio_bakup_table[i].reg_data;
        //printf("addr:%x bak_data:%x read:%x\r\n",gpio_bakup_table[i].reg_addr,gpio_bakup_table[i].reg_data,DRV_Reg32(gpio_bakup_table[i].reg_addr));
    }
}

#endif

