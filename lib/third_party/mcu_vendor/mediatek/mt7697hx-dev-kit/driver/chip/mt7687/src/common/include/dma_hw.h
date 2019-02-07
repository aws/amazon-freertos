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

#ifndef _DMA_HW_H
#define _DMA_HW_H

#include "hal_platform.h"
#ifdef HAL_GDMA_MODULE_ENABLED
#include "mt7687_cm4_hw_memmap.h"


#define DMA_base                CM4_DMA_BASE
#define VFIFO_base              (0x79000000)

/*
 * DMA registers.
 */
#define DMA_GLBSTA0             (DMA_base + 0x0000)
#define DMA_GLBSTA1             (DMA_base + 0x0004)
#define DMA_GLBLIMITER          (DMA_base + 0x0028)
#define DMA_PRIORITY_CON1       (DMA_base + 0x0040)
#define DMA_SRC(_n)             (DMA_base + 0x0000 + (0x0100 * (_n)))
#define DMA_DST(_n)             (DMA_base + 0x0004 + (0x0100 * (_n)))
#define DMA_WPPT(_n)            (DMA_base + 0x0008 + (0x0100 * (_n)))
#define DMA_WPTO(_n)            (DMA_base + 0x000c + (0x0100 * (_n)))
#define DMA_COUNT(_n)           (DMA_base + 0x0010 + (0x0100 * (_n)))
#define DMA_CON(_n)             (DMA_base + 0x0014 + (0x0100 * (_n)))
#define DMA_START(_n)           (DMA_base + 0x0018 + (0x0100 * (_n)))
#define DMA_INTSTA(_n)          (DMA_base + 0x001c + (0x0100 * (_n)))
#define DMA_ACKINT(_n)          (DMA_base + 0x0020 + (0x0100 * (_n)))
#define DMA_RLCT(_n)            (DMA_base + 0x0024 + (0x0100 * (_n)))
#define DMA_LIMITER(_n)         (DMA_base + 0x0028 + (0x0100 * (_n)))
#define DMA_PGMADDR(_n)         (DMA_base + 0x002c + (0x0100 * (_n)))
#define DMA_TO(_n)              (DMA_base + 0x0050 + (0x0100 * (_n)))
#define DMA_GLBSTA_RUN(_n)      (_n > 16 ? (0x00000001 << (2 * (_n - 16 - 1))) : (0x00000001 << (2 * (_n - 1))))
#define DMA_GLBSTA_IT(_n)       (_n > 16 ? (0x00000002 << (2 * (_n - 16 - 1))) : (0x00000002 << (2 * (_n - 1))))
#define DMA_COUNT_MASK          0xffff

/*
 * DMA_CON.
 */
#define DMA_CON_SIZE_MASK       0x00000003
#define DMA_CON_SIZE_BYTE       0x00000000
#define DMA_CON_SIZE_SHORT      0x00000001
#define DMA_CON_SIZE_LONG       0x00000002
#define DMA_CON_SINC            0x00000004
#define DMA_CON_DINC            0x00000008
#define DMA_CON_DRQ             0x00000010
#define DMA_CON_B2W             0x00000020
#define DMA_CON_BURST_SINGLE    0x00000000
#define DMA_CON_BURST_4BEAT     0x00000200
#define DMA_CON_BURST_8BEAT     0x00000400
#define DMA_CON_BURST_16BEAT    0x00000600
#define DMA_CON_TOEN            0x00004000
#define DMA_CON_ITEN            0x00008000
#define DMA_CON_WPSD            0x00010000
#define DMA_CON_WPEN            0x00020000
#define DMA_CON_DIR             0x00040000
#define DMA_CON_ACK_TYPE        0x00080000
#define DMA_CON_MASTER_MASK     0x03f00000
#define DMA_CON_RXCONTRL        (DMA_CON_DIR|DMA_CON_DINC)
#define DMA_CON_TXCONTRL        (DMA_CON_SINC)
#define DMA_CON_SWCOPY          (DMA_CON_SINC|DMA_CON_DINC)

#define DMA_CON_MASTER_I2S_TX_SADDR     0x830C0000
#define DMA_CON_MASTER_I2S_RX_SADDR     0x830C0000
#define DMA_CON_MASTER_EP2O_RX_SADDR    0x50310004
#define DMA_CON_MASTER_EP3I_TX_SADDR    0x50310008
#define DMA_CON_MASTER_EP3O_RX_SADDR    0x5031000C
#define DMA_CON_MASTER_EP4I_TX_SADDR    0x50310010
#define DMA_CON_MASTER_EP4O_RX_SADDR    0x50310014

#define DMA_CON_MASTER_VFF_DATA_PORT_START_ADDR    0x79000000

#define DMA_START_BIT           0x00008000
#define DMA_STOP_BIT            0x00000000
#define DMA_ACKINT_BIT          0x00008000
#define DMA_ACKTOINT_BIT        0x00010000
#define DMA_INTSTA_BIT          0x00008000
#define DMA_TOINTSTA_BIT        0x00010000

#define DMA_RLCT_MASK           0x0000ffff

#define DMA_LIMITER_MASK        0x000000ff

#define DMA_VPORT_BASE          VFIFO_base
#define DMA_VPORT(_n)           (DMA_VPORT_BASE + 0x0100 * (_n - DMA_VFIFO_CH_S))
#define DMA_WRPTR(_n)           (DMA_base + 0x0030 + (0x0100 * (_n)))
#define DMA_RDPTR(_n)           (DMA_base + 0x0034 + (0x0100 * (_n)))
#define DMA_FFCNT(_n)           (DMA_base + 0x0038 + (0x0100 * (_n)))
#define DMA_FFSTA(_n)           (DMA_base + 0x003c + (0x0100 * (_n)))
#define DMA_ALTLEN(_n)          (DMA_base + 0x0040 + (0x0100 * (_n)))
#define DMA_FFSIZE(_n)          (DMA_base + 0x0044 + (0x0100 * (_n)))
#define DMA_CVFF(_n)            (DMA_base + 0x0048 + (0x0100 * (_n)))

#define DMA_FFSTA_FULL          0x0001
#define DMA_FFSTA_EMPTY	        0x0002
#define DMA_FFSTA_ALERT	        0x0004

#define DMA_CVFF_CVFF_EN                    0x00000001
#define DMA_VIFOF_ALERT_LENGTH              0x3f

#define DMA_POP(_n)             *(volatile kal_uint8*)DMA_VPORT(_n)
#define DMA_PUSH(_n,_data)      *(volatile kal_uint8*)DMA_VPORT(_n) = (kal_uint8)_data

#define DMA_GetVFIFO_Avail(ch)              DRV_Reg16(DMA_FFCNT(ch))
#define DMA_GetVFIFO_FFSIZE(ch)             (DRV_Reg16(DMA_FFSIZE(ch)))
#define IS_VFIFO_FULL(ch)                   (DRV_Reg32(DMA_FFSTA(ch)) & DMA_FFSTA_FULL)
#define DMA_EnableINT(ch)                   (DRV_Reg32(DMA_CON(ch)) |= DMA_CON_ITEN)
#define DMA_DisableINT(ch)                  (DRV_Reg32(DMA_CON(ch)) &= ~DMA_CON_ITEN)
#define DMA_TASK_PUSH(ch,data)              while(IS_VFIFO_FULL(ch)); \
                                            DMA_PUSH(ch,data)
#define DMA_Enable_CVFF(ch, port_addr)      DRV_Reg32(DMA_CVFF(ch)) = ((port_addr & 0xFFFFFFFE) | DMA_CVFF_CVFF_EN);

#define DMA_TX_TRIGGER_LEVEL(n)             ((n) / 8)
#define DMA_RX_TRIGGER_LEVEL(n)             ((n) * 3 / 4)


#endif  //#ifdef HAL_GDMA_MODULE_ENABLED

#endif  /* !_DMA_HW_H */

