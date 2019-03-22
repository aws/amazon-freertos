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

#ifndef __HAL_PINMUX_DEFINE_H__
#define __HAL_PINMUX_DEFINE_H__

#include "hal_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAL_GPIO_MODULE_ENABLED


#define HAL_GPIO_0_EINT0   3
#define HAL_GPIO_0_UART1_RTS_CM4   7
#define HAL_GPIO_0_GPIO0   8
#define HAL_GPIO_0_PWM0   9
#define HAL_GPIO_1_EINT1   3
#define HAL_GPIO_1_UART1_CTS_CM4   7
#define HAL_GPIO_1_GPIO1   8
#define HAL_GPIO_1_PWM1   9
#define HAL_GPIO_2_WIC0   3
#define HAL_GPIO_2_SWD_CLK   4
#define HAL_GPIO_2_UART1_RX_CM4   7
#define HAL_GPIO_2_GPIO2   8
#define HAL_GPIO_2_PWM23   9
#define HAL_GPIO_3_PULSE_CNT   2
#define HAL_GPIO_3_EINT2   3
#define HAL_GPIO_3_SWD_DIO   4
#define HAL_GPIO_3_UART1_TX_CM4   7
#define HAL_GPIO_3_GPIO3   8
#define HAL_GPIO_3_PWM24   9
#define HAL_GPIO_4_EINT3   3
#define HAL_GPIO_4_SPI_DATA0_EXT   7
#define HAL_GPIO_4_GPIO4   8
#define HAL_GPIO_4_PWM2   9
#define HAL_GPIO_5_EINT4   3
#define HAL_GPIO_5_SPI_DATA1_EXT   7
#define HAL_GPIO_5_GPIO5   8
#define HAL_GPIO_5_PWM3   9
#define HAL_GPIO_6_EINT5   3
#define HAL_GPIO_6_SPI_CS_1_M_CM4   7
#define HAL_GPIO_6_GPIO6   8
#define HAL_GPIO_6_PWM4   9
#define HAL_GPIO_7_EINT6   3
#define HAL_GPIO_7_SPI_MISO_S_CM4   5
#define HAL_GPIO_7_SPI_CS_0_M_CM4   6
#define HAL_GPIO_7_SPI_CS_EXT   7
#define HAL_GPIO_7_GPIO7   8
#define HAL_GPIO_7_PWM5   9
#define HAL_GPIO_24_I2C2_CLK   4
#define HAL_GPIO_24_SPI_MOSI_S_CM4   5
#define HAL_GPIO_24_SPI_MOSI_M_CM4   6
#define HAL_GPIO_24_SPI_DATA2_EXT   7
#define HAL_GPIO_24_GPIO24   8
#define HAL_GPIO_24_PWM25   9
#define HAL_GPIO_25_FRAME_SYNC   2
#define HAL_GPIO_25_WIC1   3
#define HAL_GPIO_25_I2C2_DATA   4
#define HAL_GPIO_25_SPI_SCK_S_CM4   5
#define HAL_GPIO_25_SPI_MISO_M_CM4   6
#define HAL_GPIO_25_SPI_DATA3_EXT   7
#define HAL_GPIO_25_GPIO25   8
#define HAL_GPIO_25_PWM26   9
#define HAL_GPIO_26_I2S_TX   4
#define HAL_GPIO_26_SPI_CS_0_S_CM4   5
#define HAL_GPIO_26_SPI_SCK_M_CM4   6
#define HAL_GPIO_26_SPI_CLK_EXT   7
#define HAL_GPIO_26_GPIO26   8
#define HAL_GPIO_26_PWM27   9
#define HAL_GPIO_27_PULSE_CNT   2
#define HAL_GPIO_27_WIC2   3
#define HAL_GPIO_27_I2C1_CLK   4
#define HAL_GPIO_27_SWD_DIO   5
#define HAL_GPIO_27_GPIO27   8
#define HAL_GPIO_27_PWM28   9
#define HAL_GPIO_28_I2C1_DATA   4
#define HAL_GPIO_28_SWD_CLK  5
#define HAL_GPIO_28_GPIO28   8
#define HAL_GPIO_28_PWM29   9
#define HAL_GPIO_29_WIC3   3
#define HAL_GPIO_29_I2S_MCLK   4
#define HAL_GPIO_29_SPI_MOSI_S_CM4   6
#define HAL_GPIO_29_SPI_MOSI_M_CM4   7
#define HAL_GPIO_29_GPIO29   8
#define HAL_GPIO_29_PWM30   9
#define HAL_GPIO_30_I2S_FS   4
#define HAL_GPIO_30_SPI_MISO_S_CM4   6
#define HAL_GPIO_30_SPI_MISO_M_CM4   7
#define HAL_GPIO_30_GPIO30   8
#define HAL_GPIO_30_PWM31   9
#define HAL_GPIO_31_I2S_RX    4
#define HAL_GPIO_31_I2S_TX    5
#define HAL_GPIO_31_SPI_SCK_S_CM4    6
#define HAL_GPIO_31_SPI_SCK_M_CM4    7
#define HAL_GPIO_31_GPIO31   8
#define HAL_GPIO_31_PWM32   9
#define HAL_GPIO_32_WIC4   3
#define HAL_GPIO_32_I2S_BCLK   4
#define HAL_GPIO_32_SPI_CS_0_S_CM4    6
#define HAL_GPIO_32_SPI_CS_0_M_CM4    7
#define HAL_GPIO_32_GPIO32   8
#define HAL_GPIO_32_PWM33   9
#define HAL_GPIO_33_PULSE_CNT    1
#define HAL_GPIO_33_WIC5   3
#define HAL_GPIO_33_SWD_DIO    6
#define HAL_GPIO_33_IR_TX    7
#define HAL_GPIO_33_GPIO33   8
#define HAL_GPIO_33_PWM34   9
#define HAL_GPIO_34_FRAME_SYNC   1
#define HAL_GPIO_34_WIC6   3
#define HAL_GPIO_34_SWD_CLK   6
#define HAL_GPIO_34_IR_RX    7
#define HAL_GPIO_34_GPIO34   8
#define HAL_GPIO_34_PWM35   9
#define HAL_GPIO_35_EINT19   3
#define HAL_GPIO_35_I2S_TX   5
#define HAL_GPIO_35_UART_DBG_CM4   7
#define HAL_GPIO_35_GPIO35  8
#define HAL_GPIO_35_PWM18   9
#define HAL_GPIO_36_WIC7   3
#define HAL_GPIO_36_UART2_RX_CM4    7
#define HAL_GPIO_36_GPIO36   8
#define HAL_GPIO_36_PWM19   9
#define HAL_GPIO_37_EINT20   3
#define HAL_GPIO_37_UART2_TX_CM4    7
#define HAL_GPIO_37_GPIO37   8
#define HAL_GPIO_37_PWM20   9
#define HAL_GPIO_38_EINT21   3
#define HAL_GPIO_38_SWD_DIO    6
#define HAL_GPIO_38_UART2_RTS_CM4    7
#define HAL_GPIO_38_GPIO38    8
#define HAL_GPIO_38_PWM21    9
#define HAL_GPIO_39_PULSE_CNT    1
#define HAL_GPIO_39_EINT22   3
#define HAL_GPIO_39_SWD_CLK    6
#define HAL_GPIO_39_UART2_CTS_CM4    7
#define HAL_GPIO_39_GPIO39    8
#define HAL_GPIO_39_PWM22     9
#define HAL_GPIO_57_WIC8    3
#define HAL_GPIO_57_GPIO57     8
#define HAL_GPIO_57_PWM36     9
#define HAL_GPIO_58_WIC9    3
#define HAL_GPIO_58_GPIO58     8
#define HAL_GPIO_58_PWM37     9
#define HAL_GPIO_59_WIC10    3
#define HAL_GPIO_59_SWD_DIO    6
#define HAL_GPIO_59_GPIO59     8
#define HAL_GPIO_59_PWM38     9
#define HAL_GPIO_60_PULSE_CNT    2
#define HAL_GPIO_60_WIC11    3
#define HAL_GPIO_60_SWD_CLK    6
#define HAL_GPIO_60_GPIO60     8
#define HAL_GPIO_60_PWM39     9
#define HAL_GPIO_61_GPIO61     8
#define HAL_GPIO_62_GPIO62     8
#define HAL_GPIO_63_GPIO63     8
#define HAL_GPIO_64_GPIO64     8
#define HAL_GPIO_65_GPIO65     8
#define HAL_GPIO_66_GPIO66     8
#define HAL_GPIO_67_GPIO67     8
#define HAL_GPIO_68_GPIO68     8
#define HAL_GPIO_69_GPIO69     8
#define HAL_GPIO_70_GPIO70     8
#define HAL_GPIO_71_GPIO71     8
#define HAL_GPIO_72_GPIO72     8


#ifdef __cplusplus
}
#endif

#endif /*HAL_GPIO_MODULE_ENABLED*/

#endif /*__HAL_PINMUX_DEFINE_H__*/

