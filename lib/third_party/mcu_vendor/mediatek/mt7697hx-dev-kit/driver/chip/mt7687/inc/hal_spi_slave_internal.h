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

#ifndef __HAL_SPI_SLAVE_INTERNAL_H__
#define __HAL_SPI_SLAVE_INTERNAL_H__

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED

/*define SPIS register address*/
#define  SPIS_AHB_REG05            ((uint32_t)0x21000714)
#define  SPIS_AHB_REG06            ((uint32_t)0x21000718)
#define  SPIS_AHB_REG08            ((uint32_t)0x21000740)

/*SPIS datasheet required setting*/
#define PAD_SDIO_DAT2_CTL          ((uint32_t)0x81023058)
#define PAD_SDIO_DAT3_CTL          ((uint32_t)0x8102305C)


#define SPIS_20M_CLOCK_ENABLE() (*((uint32_t *)SPIS_AHB_REG06) |= 0x40)

/* clear SW irq */
#define SPIS_CLEAR_IRQ() (*((uint32_t *)SPIS_AHB_REG05) = 1)

/*
Mode 1 (CPOL =0 / CPHA=0)
Write address [0x21000718], Bit[6] = 0x1
Write address [0x21000740], Bit[1:0] = 0x0
Write address [0x81023058], Bit[5] = 0x1
Write address [0x8102305c], Bit[5] = 0x0
*/
/*
Mode 2 (CPOL =0 / CPHA=1)
Write address [0x21000718], Bit[6] = 0x1
Write address [0x21000740], Bit[1:0] = 0x1
Write address [0x81023058], Bit[5] = 0x1
Write address [0x8102305c], Bit[5] = 0x1
*/
/*
Mode 3 (CPOL =1 / CPHA=0)
Write address [0x21000718], Bit[6] = 0x1
Write address [0x21000740], Bit[1:0] = 0x2
Write address [0x81023058], Bit[5] = 0x1
Write address [0x8102305c], Bit[5] = 0x1
*/
/*
Mode 4 (CPOL =1 / CPHA=1)
Write address [0x21000718], Bit[6] = 0x1
Write address [0x21000740], Bit[1:0] = 0x3
Write address [0x81023058], Bit[5] = 0x1
Write address [0x8102305c], Bit[5] = 0x0
*/

#define SPIS_SET_MODE(mode)                                   \
do{                                                           \
       (*((uint32_t *)SPIS_AHB_REG08) = mode);                \
       (*((uint32_t *)PAD_SDIO_DAT2_CTL) |= 0x20);            \
       if ((mode == 0) || (mode ==3)) {                       \
           (*((uint32_t *)PAD_SDIO_DAT3_CTL) &= 0xffffffdf);  \
       } else {                                               \
           (*((uint32_t *)PAD_SDIO_DAT3_CTL) |= 0x20);        \
       }                                                      \
}while(0)

#endif   /* HAL_SPI_SLAVE_MODULE_ENABLED */
#endif   /*__HAL_SPI_SLAVE_INTERNAL_H__*/


