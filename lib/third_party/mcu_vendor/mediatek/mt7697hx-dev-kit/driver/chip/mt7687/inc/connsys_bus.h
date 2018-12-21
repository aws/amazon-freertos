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

#ifndef __CONNSYS_BUS_H__
#define __CONNSYS_BUS_H__
#include "dma_hw.h"

/* ==============   CONNSYS CCCR/FBR CR ================ */
#define SDIO_CCCR_IOEx      0x02
#define SDIO_CCCR_IORx      0x03
#define SDIO_CCCR_IENx      0x04    /* Function/Master Interrupt Enable */
#define SDIO_CCCR_INTx      0x05    /* Function Interrupt Pending */
#define SDIO_CCCR_ABORT     0x06    /* function abort/card reset */
#define SDIO_CCCR_IF        0x07    /* bus interface controls */
// Function Basic Registers (FBR)
#define SDIO_FBR_BASE(f)    ((f) * 0x100) /* base of function f's FBRs */
#define SDIO_FBR_BLKSIZE    0x10    /* block size (2 bytes) */


/* ==============   CONNSYS FW side CR ================ */
#define SDIO_GEN3_BASE          (0x50200000)

#define SDIO_GEN3_CMD_SETUP     (SDIO_GEN3_BASE+0x0)
//#define SDIO_GEN3_DMA_SETUP     (SDIO_GEN3_BASE+0x100)

#define SDIO_GEN3_CMD3_DATA     (SDIO_GEN3_BASE+0x8)
#define SDIO_GEN3_CMD5_DATA     (SDIO_GEN3_BASE+0xC)
#define SDIO_GEN3_CMD7_DATA     (SDIO_GEN3_BASE+0x10)
#define SDIO_GEN3_CMD11_DATA    (SDIO_GEN3_BASE+0x14)

#define SDIO_GEN3_CMD52_DATA    (SDIO_GEN3_BASE+0x4)
#define SDIO_GEN3_CMD53_DATA    (SDIO_GEN3_BASE+0x1000)

/* ==============   CONNSYS HOST side CR ================ */
#define SDIO_base       (0x50030000)
#define HWFICR          (0x00000150 + SDIO_base)
#define D2H_SW_INT_SET  (0xFFFFFF << 8)
#define LEN_SDIO_TX_TERMINATOR  4 /*HW design spec*/
#define WCIR 0x0000
#define CHIP_ID_MASK (0xffff)
#define GET_CHIP_ID(p) (((p) & CHIP_ID_MASK))
#define REVISION_ID_MASK (0xf << 16)
#define GET_REVISION_ID(p) (((p) & REVISION_ID_MASK) >> 16)
#define POR_INDICATOR (1 << 20)
#define GET_POR_INDICATOR(p) (((p) & POR_INDICATOR) >> 20)
#define W_FUNC_RDY (1 << 21)
#define GET_W_FUNC_RDY(p) (((p) & W_FUNC_RDY) >> 21)
#define DEVICE_STATUS_MASK (0xff << 24)
#define GET_DEVICE_STATUS(p) (((p) & DEVICE_STATUS_MASK) >> 24)

#define WHLPCR 0x0004
#define W_INT_EN_SET (1 << 0)
#define W_INT_EN_CLR (1 << 1)
#define W_FW_OWN_REQ_SET (1 << 8)
#define GET_W_FW_OWN_REQ_SET(p) (((p) & W_FW_OWN_REQ_SET) >> 8)
#define W_FW_OWN_REQ_CLR (1 << 9)
#define WSDIOCSR 0x0008
#define WHCR 0x000C
#define W_INT_CLR_CTRL (1 << 1)
#define RECV_MAILBOX_RD_CLR_EN (1 << 2)
#define RPT_OWN_RX_PACKET_LEN (1 << 3)
#define MAX_HIF_RX_LEN_NUM_MASK (0x3f << 8)
#define MAX_HIF_RX_LEN_NUM_OFFSET (8)

#define MAX_HIF_RX_LEN_NUM(p) (((p) & 0x3f) << 8)
#define GET_MAX_HIF_RX_LEN_NUM(p) (((p) & MAX_HIF_RX_LEN_NUM_MASK) >> 8)
#define RX_ENHANCE_MODE (1 << 16)


#define WHISR 0x0010
#define TX_DONE_INT (1 << 0)
#define RX0_DONE_INT (1 << 1)
#define RX1_DONE_INT (1 << 2)
#define ABNORMAL_INT (1 << 6)
#define FW_OWN_BACK_INT (1 << 7)
#define DUPM_N9_QUERY_STATUS_INT (1<<8)
#define D2H_SW_INT (0xffffff << 9)
#define D2H_SW_INT_MASK (0xffffff << 9)
#define GET_D2H_SW_INT(p) (((p) & D2H_SW_INT_MASK) >> 9)

#define WHIER 0x0014
#define TX_DONE_INT_EN (1 << 0)
#define RX0_DONE_INT_EN (1 << 1)
#define RX1_DONE_INT_EN (1 << 2)
#define ABNORMAL_INT_EN (1 << 6)
#define FW_OWN_BACK_INT_EN (1 << 7)
#define DUMP_N9_INT_EN  (1 << 8)
#define D2H_SW_INT_EN_MASK (0xffffff << 9)
#define D2H_SW_INT_EN(p) (((p) & 0xffffff) << 9)
#define GET_D2H_SW_INT_EN(p) (((p) & D2H_SW_INT_EN_MASK) >> 9)
#define WHIER_DEFAULT (TX_DONE_INT_EN | RX0_DONE_INT_EN | RX1_DONE_INT_EN\
                        | ABNORMAL_INT_EN\
                        | D2H_SW_INT_EN_MASK)

#define WASR 0x0020
#define TX1_OVERFLOW (1 << 1)
#define RX0_UNDERFLOW (1 << 8)
#define RX1_UNDERFLOW (1 << 9)
#define WASR_WASR2 (1 << 16)
#define WSICR 0x0024
#define NUMP_N9_TRIGGER_INT_SET (1 << 16)
#define NUMP_N9_TRIGGER_INT_CLR (1 << 17)

#define WTDR1 0x0034
#define WRDR0 0x0050
#define WRDR1 0x0054
#define H2DSM0R 0x0070
#define H2DSM1R 0x0074
#define D2HRM0R 0x0078
#define D2HRM1R 0x007c
#define WRPLR 0x0090
#define RX0_PACKET_LENGTH_MASK (0xffff)
#define GET_RX0_PACKET_LENGTH(p) (((p) & RX0_PACKET_LENGTH_MASK))
#define RX1_PACKET_LENGTH_MASK (0xffffU << 16U)
#define GET_RX1_PACKET_LENGTH(p) (((p) & RX1_PACKET_LENGTH_MASK) >> 16U)
#define WTMDR 0x00b0
#define WTMCR 0x00b4
#define WTMDPCR0 0x00b8
#define WTMDPCR1 0x00bc
#define WPLRCR 0x00d4
#define RX0_RPT_PKT_LEN_MASK (0x3f)
#define RX0_RPT_PKT_LEN_OFFSET 0
#define RX0_RPT_PKT_LEN(p) (((p) & 0x3f))
#define GET_RPT_PKT_LEN(p) (((p) & RX0_RPT_PKT_LEN_MASK))
#define RX1_RPT_PKT_LEN_MASK (0x3f << 8)
#define RX1_RPT_PKT_LEN_OFFSET 8
#define RX1_RPT_PKT_LEN(p) (((p) & 0x3f) << 8)
#define GET_RX1_RPT_PKT_LEN(p) (((p) & RX1_RPT_PKT_LEN_MASK) >> 8)
#define WSR 0x00D8

#define WASR2 0x00E0
#define WASR2_CMD53_ERR_CNT_OFFSET              16
#define WASR2_CMD53_ERR_CNT_MASK                ((0x3FF) << WASR2_CMD53_ERR_CNT_OFFSET)
#define WASR2_CMD53_ERR_CNT                     (1<<16)
#define WASR2_CMD53_ERR_OFFSET                  8
#define WASR2_CMD53_ERR                  (1<<8)
#define WASR2_CMD53_RD_TIMEOUT_OFFSET           2
#define WASR2_CMD53_RD_TIMEOUT           (1<<2)
#define WASR2_CMD53_WR_TIMEOUT_OFFSET           1
#define WASR2_CMD53_WR_TIMEOUT           (1<<1)
#define WASR2_FW_OWN_INVALID_ACCESS_OFFSET      0
#define WASR2_FW_OWN_INVALID_ACCESS      (1<<0)




#define CLKIOCR 0x0100
#define CMDIOCR 0x0104
#define DAT0IOCR 0x0108
#define DAT1IOCR 0x010C
#define DAT2IOCR 0x0110
#define DAT3IOCR 0x0114
#define CLKDLYCR 0x0118
#define CMDDLYCR 0x011C
#define ODATDLYCR 0x0120
#define IDATDLYCR1 0x0124
#define IDATDLYCR2 0x0128
#define ILCHCR 0x012C
#define WTQCR0 0x0130
#define WTQCR1 0x0134
#define WTQCR2 0x0138
#define WTQCR3 0x013C
#define WTQCR4 0x0140
#define WTQCR5 0x0144
#define WTQCR6 0x0148
#define WTQCR7 0x014C

#define TXCCPU (0xFFFF << 8)
#define TXCFFA (0xFFFF)
#define RXD_CLS_EN  (1 << 10)  //DW3 bit10
#define CCCR_IO_ABORT 0x06
#define CCCR_RESET_ALL (1 << 3)
#define CCCR_DEEP_SLEEP 0xF1

/* ==============   CONNSYS DMA CR ================ */
#define CONNSYS_GDMA_CH            4

#define GDMA_CH4_BASE          (DMA_base+0x400)
#define GDMA_CH4_COUNT         (GDMA_CH4_BASE+0x10)
#define GDMA_CH4_CON           (GDMA_CH4_BASE+0x14)
#define GDMA_CH4_START         (GDMA_CH4_BASE+0x18)
#define GDMA_CH4_PGMADDR       (GDMA_CH4_BASE+0x2c)
#define GDMA_CH4_RLCT          (GDMA_CH4_BASE+0x24)
#define GDMA_CHANNEL_ENABLE     (0x83000018)

/* ==================== HW Definition ============ */
#define ALIGN_4BYTE(size)       (((size+3)/4) * 4)
#define LEN_SDIO_TX_TERMINATOR  4 /*HW design spec*/
#define LEN_SDIO_RX_TERMINATOR  4
#define SDIO_HOST_REGISTER_VALUE_MAX    0x014C
#define LEN_SDIO_TX_AGG_WRAPPER(len)    ALIGN_4BYTE((len) + LEN_SDIO_TX_TERMINATOR)

int32_t connsys_bus_read_port(struct connsys_func *func, void *dst, uint32_t addr,  int32_t count);
int32_t connsys_bus_write_port(struct connsys_func *func, uint32_t addr, void *src, int32_t count);
void    connsys_bus_write_port_start(struct connsys_func *func, uint32_t addr, int32_t total);

uint32_t connsys_bus_readl(struct connsys_func *func, uint32_t addr, int32_t *err_ret);
void    connsys_bus_writel(struct connsys_func *func, uint32_t b, uint32_t addr, int32_t *err_ret);
uint8_t connsys_bus_fn0_read_byte(struct connsys_func *func, uint32_t addr, int32_t *err_ret);
void    connsys_bus_fn0_write_byte(struct connsys_func *func, uint8_t b, uint32_t addr, int32_t *err_ret);
int32_t connsys_bus_enable_func(struct connsys_func *func);
int32_t connsys_bus_disable_func(struct connsys_func *func);
int32_t connsys_bus_set_block_size(struct connsys_func *func, uint32_t blksz);
int32_t connsys_bus_get_irq(struct connsys_func *func, connsys_irq_handler_t handler);
int32_t connsys_bus_release_irq(struct connsys_func *func);
void    connsys_bus_get_bus(struct connsys_func *func);
void    connsys_bus_release_bus(struct connsys_func *func);
int32_t connsys_bus_read_port_garbage(uint32_t addr, int32_t size);

#endif
