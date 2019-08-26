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
** $Log: hal_pinmux.h $
**
**
**
*/

/*
    Porting from legacy/include/hal_GPIO.h
*/

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

#ifndef __HAL_PINMUX_H
#define __HAL_PINMUX_H

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "mt7687_cm4_hw_memmap.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

//IOT 73 PAD GPIOs AON domain pinmux mask
#define PAD_ANTSEL0_PINMUX_AON_MASK (BIT(3)|BIT(2)|BIT(1)|BIT(0))           //gen_pmux_top_aon function5 GPIO[0]     //Indium Ball PWM0
#define PAD_ANTSEL1_PINMUX_AON_MASK (BIT(7)|BIT(6)|BIT(5)|BIT(4))           //gen_pmux_top_aon function5 GPIO[1]     //Indium Ball PWM1
#define PAD_ANTSEL2_PINMUX_AON_MASK (BIT(11)|BIT(10)|BIT(9)|BIT(8))         //gen_pmux_top_aon function5 GPIO[2]
#define PAD_ANTSEL3_PINMUX_AON_MASK (BIT(15)|BIT(14)|BIT(13)|BIT(12))       //gen_pmux_top_aon function5 GPIO[3]
#define PAD_ANTSEL4_PINMUX_AON_MASK (BIT(19)|BIT(18)|BIT(17)|BIT(16))       //gen_pmux_top_aon function5 GPIO[4]     //Indium Ball GPIO0
#define PAD_ANTSEL5_PINMUX_AON_MASK (BIT(23)|BIT(22)|BIT(21)|BIT(20))       //gen_pmux_top_aon function5 GPIO[5]     //Indium Ball GPIO1
#define PAD_ANTSEL6_PINMUX_AON_MASK (BIT(27)|BIT(26)|BIT(25)|BIT(24))       //gen_pmux_top_aon function5 GPIO[6]     //Indium Ball GPIO2
#define PAD_ANTSEL7_PINMUX_AON_MASK (BIT(31)|BIT(30)|BIT(29)|BIT(28))       //gen_pmux_top_aon function5 GPIO[7]     //Indium Ball GPIO3
#if 0
#define PAD_IN_GPIO8_PINMUX_AON_MASK (BIT(3)|BIT(2)|BIT(1)|BIT(0))          //gen_pmux_top_aon function5 GPIO[8]     //Indium Ball GPIO4
#define PAD_IN_GPIO9_PINMUX_AON_MASK (BIT(7)|BIT(6)|BIT(5)|BIT(4))          //gen_pmux_top_aon function5 GPIO[9]     //Indium Ball GPIO5
#define PAD_IN_GPIO10_PINMUX_AON_MASK (BIT(11)|BIT(10)|BIT(9)|BIT(8))       //gen_pmux_top_aon function5 GPIO[10]     //Indium Ball GPIO6
#define PAD_IN_GPIO11_PINMUX_AON_MASK (BIT(15)|BIT(14)|BIT(13)|BIT(12))    //gen_pmux_top_aon function5 GPIO[11]     //Indium Ball GPIO7
#define PAD_IN_GPIO12_PINMUX_AON_MASK (BIT(19)|BIT(18)|BIT(17)|BIT(16))    //gen_pmux_top_aon function5 GPIO[12]     //Indium Ball GPIO8
#define PAD_IN_GPIO13_PINMUX_AON_MASK (BIT(23)|BIT(22)|BIT(21)|BIT(20))    //gen_pmux_top_aon function5 GPIO[13]     //Indium Ball GPIO9
#define PAD_IN_GPIO14_PINMUX_AON_MASK (BIT(27)|BIT(26)|BIT(25)|BIT(24))    //gen_pmux_top_aon function5 GPIO[14]     //Indium Ball GPIO10
#define PAD_IN_GPIO15_PINMUX_AON_MASK (BIT(31)|BIT(30)|BIT(29)|BIT(28))    //gen_pmux_top_aon function5 GPIO[15]     //Indium Ball GPIO11
#define PAD_IN_GPIO16_PINMUX_AON_MASK (BIT(3)|BIT(2)|BIT(1)|BIT(0))         //gen_pmux_top_aon function5 GPIO[16]     //Indium Ball GPIO12
#define PAD_IN_GPIO17_PINMUX_AON_MASK (BIT(7)|BIT(6)|BIT(5)|BIT(4))         //gen_pmux_top_aon function5 GPIO[17]     //Indium Ball GPIO13
#define PAD_IN_GPIO18_PINMUX_AON_MASK (BIT(11)|BIT(10)|BIT(9)|BIT(8))       //gen_pmux_top_aon function5 GPIO[18]     //Indium Ball GPIO14
#define PAD_IN_GPIO19_PINMUX_AON_MASK (BIT(15)|BIT(14)|BIT(13)|BIT(12))    //gen_pmux_top_aon function5 GPIO[19]     //Indium Ball GPIO15
#define PAD_IN_GPIO20_PINMUX_AON_MASK (BIT(19)|BIT(18)|BIT(17)|BIT(16))    //gen_pmux_top_aon function5 GPIO[20]
#define PAD_IN_GPIO21_PINMUX_AON_MASK (BIT(23)|BIT(22)|BIT(21)|BIT(20))    //gen_pmux_top_aon function5 GPIO[21]
#define PAD_IN_GPIO22_PINMUX_AON_MASK (BIT(27)|BIT(26)|BIT(25)|BIT(24))    //gen_pmux_top_aon function5 GPIO[22]
#define PAD_IN_UART0_TXD_PINMUX_AON_MASK (BIT(31)|BIT(30)|BIT(29)|BIT(28))  //gen_pmux_top_aon function5 GPIO[23]
#endif
#define PAD_PERST_N_PINMUX_AON_MASK (BIT(3)|BIT(2)|BIT(1)|BIT(0))           //gen_pmux_top_aon function5 GPIO[24]
#define PAD_WAKE_N_PINMUX_AON_MASK (BIT(7)|BIT(6)|BIT(5)|BIT(4))            //gen_pmux_top_aon function5 GPIO[25]
#define PAD_CLK_REQ_N_PINMUX_AON_MASK (BIT(11)|BIT(10)|BIT(9)|BIT(8))       //gen_pmux_top_aon function5 GPIO[26]
#define PAD_SDIO_CLK_PINMUX_AON_MASK (BIT(15)|BIT(14)|BIT(13)|BIT(12))      //gen_pmux_top_aon function5 GPIO[27]
#define PAD_SDIO_CMD_PINMUX_AON_MASK (BIT(19)|BIT(18)|BIT(17)|BIT(16))      //gen_pmux_top_aon function5 GPIO[28]
#define PAD_SDIO_DAT3_PINMUX_AON_MASK (BIT(23)|BIT(22)|BIT(21)|BIT(20))    //gen_pmux_top_aon function5 GPIO[29]
#define PAD_SDIO_DAT2_PINMUX_AON_MASK (BIT(27)|BIT(26)|BIT(25)|BIT(24))    //gen_pmux_top_aon function5 GPIO[30]
#define PAD_SDIO_DAT1_PINMUX_AON_MASK (BIT(31)|BIT(30)|BIT(29)|BIT(28))    //gen_pmux_top_aon function5 GPIO[31]
#define PAD_SDIO_DAT0_PINMUX_AON_MASK (BIT(3)|BIT(2)|BIT(1)|BIT(0))         //gen_pmux_top_aon function5 GPIO[32]
#define PAD_GPIO0_PINMUX_AON_MASK (BIT(7)|BIT(6)|BIT(5)|BIT(4))             //gen_pmux_top_aon function5 GPIO[33]
#define PAD_GPIO1_PINMUX_AON_MASK (BIT(11)|BIT(10)|BIT(9)|BIT(8))           //gen_pmux_top_aon function5 GPIO[34]
#define PAD_UART_DBG_PINMUX_AON_MASK (BIT(15)|BIT(14)|BIT(13)|BIT(12))      //gen_pmux_top_aon function5 GPIO[35]     //Indium Ball GPIO16
#define PAD_UART_RX_PINMUX_AON_MASK (BIT(19)|BIT(18)|BIT(17)|BIT(16))       //gen_pmux_top_aon function5 GPIO[36]     //Indium Ball GPIO17
#define PAD_UART_TX_PINMUX_AON_MASK (BIT(23)|BIT(22)|BIT(21)|BIT(20))       //gen_pmux_top_aon function5 GPIO[37]     //Indium Ball GPIO18
#define PAD_UART_RTS_PINMUX_AON_MASK (BIT(27)|BIT(26)|BIT(25)|BIT(24))      //gen_pmux_top_aon function5 GPIO[38]     //Indium Ball GPIO19
#define PAD_UART_CTS_PINMUX_AON_MASK (BIT(31)|BIT(30)|BIT(29)|BIT(28))      //gen_pmux_top_aon function5 GPIO[39]     //Indium Ball GPIO20
#if 0
#define PAD_IN_GPIO44_PINMUX_AON_MASK (BIT(3)|BIT(2)|BIT(1)|BIT(0))         //gen_pmux_top_aon function5 GPIO[40]     //Indium Ball GPIO21
#define PAD_IN_GPIO45_PINMUX_AON_MASK (BIT(7)|BIT(6)|BIT(5)|BIT(4))         //gen_pmux_top_aon function5 GPIO[41]     //Indium Ball GPIO22
#define PAD_IN_GPIO46_PINMUX_AON_MASK (BIT(11)|BIT(10)|BIT(9)|BIT(8))       //gen_pmux_top_aon function5 GPIO[42]     //Indium Ball GPIO23
#define PAD_IN_GPIO47_PINMUX_AON_MASK (BIT(15)|BIT(14)|BIT(13)|BIT(12))    //gen_pmux_top_aon function5 GPIO[43]     //Indium Ball GPIO24
#define PAD_IN_GPIO48_PINMUX_AON_MASK (BIT(19)|BIT(18)|BIT(17)|BIT(16))    //gen_pmux_top_aon function5 GPIO[44]     //Indium Ball GPIO25
#define PAD_IN_GPIO49_PINMUX_AON_MASK (BIT(23)|BIT(22)|BIT(21)|BIT(20))    //gen_pmux_top_aon function5 GPIO[45]     //Indium Ball GPIO26
#define PAD_IN_GPIO50_PINMUX_AON_MASK (BIT(27)|BIT(26)|BIT(25)|BIT(24))    //gen_pmux_top_aon function5 GPIO[46]     //Indium Ball GPIO27
#define PAD_IN_GPIO51_PINMUX_AON_MASK (BIT(31)|BIT(30)|BIT(29)|BIT(28))    //gen_pmux_top_aon function5 GPIO[47]     //Indium Ball GPIO28
#define PAD_IN_GPIO52_PINMUX_AON_MASK (BIT(3)|BIT(2)|BIT(1)|BIT(0))         //gen_pmux_top_aon function5 GPIO[48]     //Indium Ball GPIO29
#define PAD_IN_GPIO53_PINMUX_AON_MASK (BIT(7)|BIT(6)|BIT(5)|BIT(4))         //gen_pmux_top_aon function5 GPIO[49]     //Indium Ball GPIO30
#define PAD_IN_GPIO54_PINMUX_AON_MASK (BIT(11)|BIT(10)|BIT(9)|BIT(8))       //gen_pmux_top_aon function5 GPIO[50]     //Indium Ball GPIO31
#define PAD_IN_PWM2_PINMUX_AON_MASK (BIT(15)|BIT(14)|BIT(13)|BIT(12))       //gen_pmux_top_aon function5 GPIO[51]     //Indium Ball PWM2
#define PAD_IN_PWM3_PINMUX_AON_MASK (BIT(19)|BIT(18)|BIT(17)|BIT(16))       //gen_pmux_top_aon function5 GPIO[52]     //Indium Ball PWM3
#define PAD_IN_PWM4_PINMUX_AON_MASK (BIT(23)|BIT(22)|BIT(21)|BIT(20))       //gen_pmux_top_aon function5 GPIO[53]     //Indium Ball PWM4
#define PAD_IN_PWM5_PINMUX_AON_MASK (BIT(27)|BIT(26)|BIT(25)|BIT(24))       //gen_pmux_top_aon function5 GPIO[54]     //Indium Ball PWM5
#define PAD_IN_PWM6_PINMUX_AON_MASK (BIT(31)|BIT(30)|BIT(29)|BIT(28))       //gen_pmux_top_aon function5 GPIO[55]     //Indium Ball PWM6
#define PAD_IN_PWM7_PINMUX_AON_MASK (BIT(3)|BIT(2)|BIT(1)|BIT(0))           //gen_pmux_top_aon function5 GPIO[56]     //Indium Ball PWM7
#endif
#define PAD_WF_RF_DIS_B_PINMUX_AON_MASK (BIT(7)|BIT(6)|BIT(5)|BIT(4))       //gen_pmux_top_aon function5 GPIO[57]     //Indium Ball ADC0
#define PAD_BT_RF_DIS_B_PINMUX_AON_MASK (BIT(11)|BIT(10)|BIT(9)|BIT(8))     //gen_pmux_top_aon function5 GPIO[58]     //Indium Ball ADC1
#define PAD_WF_LED_B_PINMUX_AON_MASK (BIT(15)|BIT(14)|BIT(13)|BIT(12))      //gen_pmux_top_aon function5 GPIO[59]     //Indium Ball ADC2
#define PAD_BT_LED_B_PINMUX_AON_MASK (BIT(19)|BIT(18)|BIT(17)|BIT(16))      //gen_pmux_top_aon function5 GPIO[60]     //Indium Ball ADC3
#if 0
#define PAD_IN_ADC4_PINMUX_AON_MASK (BIT(23)|BIT(22)|BIT(21)|BIT(20))       //gen_pmux_top_aon function5 GPIO[61]     //Indium Ball ADC4
#define PAD_IN_ADC5_PINMUX_AON_MASK (BIT(27)|BIT(26)|BIT(25)|BIT(24))       //gen_pmux_top_aon function5 GPIO[62]     //Indium Ball ADC5
#define PAD_IN_ADC6_PINMUX_AON_MASK (BIT(31)|BIT(30)|BIT(29)|BIT(28))       //gen_pmux_top_aon function5 GPIO[63]     //Indium Ball ADC6
#define PAD_IN_ADC7_PINMUX_AON_MASK (BIT(3)|BIT(2)|BIT(1)|BIT(0))           //gen_pmux_top_aon function5 GPIO[64]     //Indium Ball ADC7
#define PAD_IN_ADC8_PINMUX_AON_MASK (BIT(7)|BIT(6)|BIT(5)|BIT(4))           //gen_pmux_top_aon function5 GPIO[65]     //Indium Ball ADC8
#define PAD_IN_ADC9_PINMUX_AON_MASK (BIT(11)|BIT(10)|BIT(9)|BIT(8))         //gen_pmux_top_aon function5 GPIO[66]     //Indium Ball ADC9
#define PAD_IN_ADC10_PINMUX_AON_MASK (BIT(15)|BIT(14)|BIT(13)|BIT(12))      //gen_pmux_top_aon function5 GPIO[67]     //Indium Ball ADC10
#define PAD_IN_ADC11_PINMUX_AON_MASK (BIT(19)|BIT(18)|BIT(17)|BIT(16))      //gen_pmux_top_aon function5 GPIO[68]     //Indium Ball ADC11
#define PAD_IN_ADC12_PINMUX_AON_MASK (BIT(23)|BIT(22)|BIT(21)|BIT(20))      //gen_pmux_top_aon function5 GPIO[69]     //Indium Ball ADC12
#define PAD_IN_ADC13_PINMUX_AON_MASK (BIT(27)|BIT(26)|BIT(25)|BIT(24))      //gen_pmux_top_aon function5 GPIO[70]     //Indium Ball ADC13
#define PAD_IN_ADC14_PINMUX_AON_MASK (BIT(31)|BIT(30)|BIT(29)|BIT(28))      //gen_pmux_top_aon function5 GPIO[71]     //Indium Ball ADC14
#define PAD_IN_ADC15_PINMUX_AON_MASK (BIT(3)|BIT(2)|BIT(1)|BIT(0))          //gen_pmux_top_aon function5 GPIO[72]     //Indium Ball ADC15
#endif

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef enum {
    ePAD_ANTSEL0,       //eBALL_PWM_IDX_0,
    ePAD_ANTSEL1,       //eBALL_PWM_IDX_1,
    ePAD_ANTSEL2,
    ePAD_ANTSEL3,
    ePAD_ANTSEL4,       //eBALL_GPIO_IDX_0
    ePAD_ANTSEL5,       //eBALL_GPIO_IDX_1
    ePAD_ANTSEL6,       //eBALL_GPIO_IDX_2
    ePAD_ANTSEL7,       //eBALL_GPIO_IDX_3
    ePAD_IN_GPIO8,      //eBALL_GPIO_IDX_4
    ePAD_IN_GPIO9,      //eBALL_GPIO_IDX_5
    ePAD_IN_GPIO10,     //eBALL_GPIO_IDX_6
    ePAD_IN_GPIO11,     //eBALL_GPIO_IDX_7
    ePAD_IN_GPIO12,     //eBALL_GPIO_IDX_8
    ePAD_IN_GPIO13,     //eBALL_GPIO_IDX_9
    ePAD_IN_GPIO14,     //eBALL_GPIO_IDX_10
    ePAD_IN_GPIO15,     //eBALL_GPIO_IDX_11
    ePAD_IN_GPIO16,     //eBALL_GPIO_IDX_12
    ePAD_IN_GPIO17,     //eBALL_GPIO_IDX_13
    ePAD_IN_GPIO18,     //eBALL_GPIO_IDX_14
    ePAD_IN_GPIO19,     //eBALL_GPIO_IDX_15
    ePAD_IN_GPIO20,
    ePAD_IN_GPIO21,
    ePAD_IN_GPIO22,
    ePAD_IN_UART0_TXD,
    ePAD_PERST_N,
    ePAD_WAKE_N,
    ePAD_CLK_REQ_N,
    ePAD_SDIO_CLK,
    ePAD_SDIO_CMD,
    ePAD_SDIO_DAT3,
    ePAD_SDIO_DAT2,
    ePAD_SDIO_DAT1,
    ePAD_SDIO_DAT0,
    ePAD_GPIO0,
    ePAD_GPIO1,
    ePAD_UART_DBG,      //eBALL_GPIO_IDX_16
    ePAD_UART_RX,       //eBALL_GPIO_IDX_17
    ePAD_UART_TX,       //eBALL_GPIO_IDX_18
    ePAD_UART_RTS,      //eBALL_GPIO_IDX_19
    ePAD_UART_CTS,      //eBALL_GPIO_IDX_20
    ePAD_IN_GPIO44,     //eBALL_GPIO_IDX_21
    ePAD_IN_GPIO45,     //eBALL_GPIO_IDX_22
    ePAD_IN_GPIO46,     //eBALL_GPIO_IDX_23
    ePAD_IN_GPIO47,     //eBALL_GPIO_IDX_24
    ePAD_IN_GPIO48,     //eBALL_GPIO_IDX_25
    ePAD_IN_GPIO49,     //eBALL_GPIO_IDX_26
    ePAD_IN_GPIO50,     //eBALL_GPIO_IDX_27
    ePAD_IN_GPIO51,     //eBALL_GPIO_IDX_28
    ePAD_IN_GPIO52,     //eBALL_GPIO_IDX_29
    ePAD_IN_GPIO53,     //eBALL_GPIO_IDX_30
    ePAD_IN_GPIO54,     //eBALL_GPIO_IDX_31
    ePAD_IN_PWM2,       //eBALL_PWM_IDX_2,
    ePAD_IN_PWM3,       //eBALL_PWM_IDX_3,
    ePAD_IN_PWM4,       //eBALL_PWM_IDX_4,
    ePAD_IN_PWM5,       //eBALL_PWM_IDX_5,
    ePAD_IN_PWM6,       //eBALL_PWM_IDX_6,
    ePAD_IN_PWM7,       //eBALL_PWM_IDX_7,
    ePAD_WF_RF_DIS_B,   //eBALL_ADC_IDX_0
    ePAD_BT_RF_DIS_B,   //eBALL_ADC_IDX_1
    ePAD_WF_LED_B,      //eBALL_ADC_IDX_2
    ePAD_BT_LED_B,      //eBALL_ADC_IDX_3
    ePAD_IN_ADC4,       //eBALL_ADC_IDX_4
    ePAD_IN_ADC5,       //eBALL_ADC_IDX_5
    ePAD_IN_ADC6,       //eBALL_ADC_IDX_6
    ePAD_IN_ADC7,       //eBALL_ADC_IDX_7
    ePAD_IN_ADC8,       //eBALL_ADC_IDX_8
    ePAD_IN_ADC9,       //eBALL_ADC_IDX_9
    ePAD_IN_ADC10,      //eBALL_ADC_IDX_10
    ePAD_IN_ADC11,      //eBALL_ADC_IDX_11
    ePAD_IN_ADC12,      //eBALL_ADC_IDX_12
    ePAD_IN_ADC13,      //eBALL_ADC_IDX_13
    ePAD_IN_ADC14,      //eBALL_ADC_IDX_14
    ePAD_IN_ADC15,      //eBALL_ADC_IDX_15
    ePAD_NUM,
} ENUM_PAD_NAME_T;


/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

extern VOID halPinmuxTopOnPadFunc(ENUM_PAD_NAME_T ePadIndex, UINT8 ucFunc);

#endif //__HAL_PINMUX_H

