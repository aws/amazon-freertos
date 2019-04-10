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

void bsp_ept_gpio_setting_init(void);
void bsp_ept_gpio_set_control_by_cm4(void);

#define    GPIO_PINMUX_BASE            (0x81023020)
#define    GPIO_PULL_UP_BASE           (0x8300B000)
#define    GPIO_PULL_DOWN_BASE         (0x8300B030)
#define    GPIO_OE_BASE                (0x8300B090)
#define    GPIO_IES_BASE               (0x8300B100)
#define    GPIO_OUTPUT_BASE            (0x8300B060)
#define    GPIO_SDIO_CLK               (0x81023048)
#define    GPIO_SDIO_CMD               (0x8102304C)
#define    GPIO_SDIO_DATA0             (0x81023050)
#define    GPIO_SDIO_DATA1             (0x81023054)
#define    GPIO_SDIO_DATA2             (0x81023058)
#define    GPIO_SDIO_DATA3             (0x8102305C)


#define    IOT_GPIO_PINMUX(_no)          (GPIO_PINMUX_BASE + ((_no) * GPIO_MODE_REG_OFFSET_TO_NEXT))
#define    IOT_GPIO_PULL_UP(_no)         (GPIO_PULL_UP_BASE + ((_no) * GPIO_PULL_UP_REG_OFFSET_TO_NEXT))
#define    IOT_GPIO_PULL_DOWN(_no)       (GPIO_PULL_DOWN_BASE + ((_no) * GPIO_PULL_DOWN_REG_OFFSET_TO_NEXT))
#define    IOT_GPIO_OE(_no)              (GPIO_OE_BASE + ((_no) * GPIO_OE_REG_OFFSET_TO_NEXT))
#define    IOT_GPIO_IES(_no)             (GPIO_IES_BASE + ((_no) * GPIO_IES_REG_OFFSET_TO_NEXT))
#define    IOT_GPIO_OUTPUT(_no)          (GPIO_OUTPUT_BASE + ((_no) * GPIO_OUTPUT_REG_OFFSET_TO_NEXT))


#define    GPIO_MODE_REG_OFFSET_TO_NEXT             4  /* offset between two continuous mode registers */
#define    GPIO_MODE_REG_MAX_NUM                    9  /* maximam index number of mode register */
#define    GPIO_MODE_ONE_CONTROL_BITS               4  /* number of bits to control the mode of one pin */
#define    GPIO_MODE_ONE_REG_CONTROL_NUM            8  /* number of pins can be controled in one register */
#define    GPIO_MODE0    GPIO_HWORD_REG_VAL(MODE,  0,  1,  2,  3,  4,  5,  6,  7, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE1    GPIO_HWORD_REG_VAL(MODE,  8,  9,  10,  11,  12, 13,  14, 15, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE2    GPIO_HWORD_REG_VAL(MODE,  16,  17, 18,  19, 20, 21,  22, 23, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE3    GPIO_HWORD_REG_VAL(MODE,  24,  25, 26,  27, 28, 29,  30, 31, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE4    GPIO_HWORD_REG_VAL(MODE,  32,  33, 34,  35, 36, 37,  38, 39, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE5    GPIO_HWORD_REG_VAL(MODE,  40,  41, 42,  43, 44, 45,  46, 47, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE6    GPIO_HWORD_REG_VAL(MODE,  48,  49, 50,  51, 52, 53,  54, 55, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE7    GPIO_HWORD_REG_VAL(MODE,  56,  57, 58,  59, 60, 61,  62, 63, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE8    GPIO_HWORD_REG_VAL(MODE,  64,  65, 66,  67, 68, 69,  70, 71, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE9    GPIO_HWORD_REG_VAL(MODE,  72,  NULL, NULL,  NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE_ALL_VALUE   GPIO_MODE0,GPIO_MODE1,GPIO_MODE2,GPIO_MODE3,GPIO_MODE4,GPIO_MODE5,GPIO_MODE6,GPIO_MODE7,GPIO_MODE8,GPIO_MODE9


#define    GPIO_OE_REG_OFFSET_TO_NEXT              0x10  /* offset between two continuous output enable registers */
#define    GPIO_OE_REG_MAX_NUM                     2     /* maximam index number of output enable register */
#define    GPIO_OE_ONE_CONTROL_BITS                1     /* number of bits to control the output enable of one pin */
#define    GPIO_OE_REG0    GPIO_HWORD_REG_VAL(OE, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12,  13,  14,  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31)
#define    GPIO_OE_REG1    GPIO_HWORD_REG_VAL(OE,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47, 48, 49,  50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63)
#define    GPIO_OE_REG2    GPIO_HWORD_REG_VAL(OE,  64,  65,  66,  67,  68,  69,  70,  71,  72,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)

#define    GPIO_OE_ALL_VALUE     GPIO_OE_REG0,GPIO_OE_REG1,GPIO_OE_REG2


#define    GPIO_IES_REG_OFFSET_TO_NEXT              4     /* offset between two continuous input enable registers */
#define    GPIO_IES_REG_MAX_NUM                     2     /* maximam index number of input enable register */
#define    GPIO_IES_ONE_CONTROL_BITS                1     /* number of bits to control the input enable of one pin */
#define    GPIO_IES_REG0    GPIO_HWORD_REG_VAL(IES,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12,  13,  14,  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31)
#define    GPIO_IES_REG1    GPIO_HWORD_REG_VAL(IES,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47, 48, 49,  50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63)
#define    GPIO_IES_REG2    GPIO_HWORD_REG_VAL(IES,  64,  65,  66,  67,  68,  69,  70,  71,  72,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)

#define    GPIO_IES_ALL_VALUE     GPIO_IES_REG0,GPIO_IES_REG1,GPIO_IES_REG2


#define    GPIO_PULL_UP_REG_OFFSET_TO_NEXT              0x10  /* offset between two continuous pull-up registers */
#define    GPIO_PULL_UP_REG_MAX_NUM                     2     /* maximam index number of pull-up register */
#define    GPIO_PULL_UP_ONE_CONTROL_BITS                1     /* number of bits to control the pull-up of one pin */
#define    GPIO_PULL_UP_REG0    GPIO_HWORD_REG_VAL(PULL_UP,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12,  13,  14,  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31)
#define    GPIO_PULL_UP_REG1    GPIO_HWORD_REG_VAL(PULL_UP,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47, 48, 49,  50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63)
#define    GPIO_PULL_UP_REG2    GPIO_HWORD_REG_VAL(PULL_UP,  64,  65,  66,  67,  68,  69,  70,  71,  72,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)

#define    GPIO_PULL_UP_ALL_VALUE     GPIO_PULL_UP_REG0,GPIO_PULL_UP_REG1,GPIO_PULL_UP_REG2

#define    GPIO_PULL_DOWN_REG_OFFSET_TO_NEXT              0x10  /* offset between two continuous pull-down registers */
#define    GPIO_PULL_DOWN_REG_MAX_NUM                     2     /* maximam index number of pull-down register */
#define    GPIO_PULL_DOWN_ONE_CONTROL_BITS                1     /* number of bits to control the pull-down of one pin */
#define    GPIO_PULL_DOWN_REG0    GPIO_HWORD_REG_VAL(PULL_DOWN,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12,  13,  14,  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31)
#define    GPIO_PULL_DOWN_REG1    GPIO_HWORD_REG_VAL(PULL_DOWN,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47, 48, 49,  50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63)
#define    GPIO_PULL_DOWN_REG2    GPIO_HWORD_REG_VAL(PULL_DOWN,  64,  65,  66,  67,  68,  69,  70,  71,  72,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)

#define    GPIO_PULL_DOWN_ALL_VALUE     GPIO_PULL_DOWN_REG0,GPIO_PULL_DOWN_REG1,GPIO_PULL_DOWN_REG2


#define    GPIO_OUTPUT_REG_OFFSET_TO_NEXT              0x10  /* offset between two continuous output data registers */
#define    GPIO_OUTPUT_REG_MAX_NUM                     2     /* maximam index number of output data register */
#define    GPIO_OUTPUT_ONE_CONTROL_BITS                1     /* number of bits to control the output data of one pin */
#define    GPIO_OUTPUT_REG0    GPIO_HWORD_REG_VAL(OUTPUT,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12,  13,  14,  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31)
#define    GPIO_OUTPUT_REG1    GPIO_HWORD_REG_VAL(OUTPUT,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47, 48, 49,  50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63)
#define    GPIO_OUTPUT_REG2    GPIO_HWORD_REG_VAL(OUTPUT,  64,  65,  66,  67,  68,  69,  70,  71,  72,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)

#define    GPIO_OUTPUT_ALL_VALUE     GPIO_OUTPUT_REG0,GPIO_OUTPUT_REG1,GPIO_OUTPUT_REG2

