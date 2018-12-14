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

#ifndef __UART_HW_H__
#define __UART_HW_H__

#include "type_def.h"
#include "mt7687_cm4_hw_memmap.h"


#if !defined(MTK_MAIN_CONSOLE_UART2_ENABLE)
#define CONSOLE     CM4_UART1_BASE      /* define in mt7687_hw_memmap.h */
#define BT_HCI      CM4_UART2_BASE
#else
#define CONSOLE     CM4_UART2_BASE      /* define in mt7687_hw_memmap.h */
#define BT_HCI      CM4_UART1_BASE
#endif

#define   UART_DATA_MASK                  0x0003
#define   UART_PARITY_MASK                0x0038
#define   UART_STOP_MASK                  0x0004

#define   UART_LCR_DLAB                   0x0080


#define UART_RBR                (0x0)           /* Read only */
#define UART_THR                (0x0)           /* Write only */
#define UART_IER                (0x4)
#define UART_IIR                (0x8)           /* Read only */
#define UART_FCR                (0x8)           /* Write only */
#define UART_LCR                (0xc)
#define UART_MCR                (0x10)
#define UART_LSR                (0x14)
#define UART_MSR                (0x18)
#define UART_SCR                (0x1c)
#define UART_DLL                (0x0)
#define UART_DLH                (0x4)
#define UART_EFR                (0x8)           /* Only when LCR = 0xbf */
#define UART_XON1               (0x10)          /* Only when LCR = 0xbf */
#define UART_XON2               (0x14)          /* Only when LCR = 0xbf */
#define UART_XOFF1              (0x18)          /* Only when LCR = 0xbf */
#define UART_XOFF2              (0x1c)          /* Only when LCR = 0xbf */

#define UART_AUTOBAUD_EN        (0x20)
#define UART_RATE_STEP          (0x24)
#define UART_STEP_COUNT         (0x28)
#define UART_SAMPLE_COUNT       (0x2c)
#define UART_AUTOBAUD_REG       (0x30)
#define UART_RATE_FIX_REG       (0x34)
#define UART_GUARD              (0x3c)
#define UART_ESCAPE_DATA        (0x40)
#define UART_ESCAPE_EN          (0x44)
#define UART_SLEEP_EN           (0x48)
#define UART_VFIFO_EN_REG       (0x4c)
#define UART_RX_TRIGGER_ADDR    (0x50)
#define UART_FRACDIV_L          (0x54)
#define UART_FRACDIV_M          (0x58)
#define UART_TX_ACTIVE_EN       (0x60)

#define UART_IER_CTSI           (0x80UL)
#define UART_IER_RTSI           (0x40UL)
#define UART_IER_XOFFI          (0x20UL)
#define UART_IER_EDSSI          (0x8UL)
#define UART_IER_ELSI           (0x4UL)
#define UART_IER_ETBEI          (0x2UL)
#define UART_IER_ERBFI          (0x1UL)

#endif
