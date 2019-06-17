/** @file uart_reg.h
*
*  @brief This file contains automatically generated register structure
*
*  (C) Copyright 2012-2018 Marvell International Ltd. All Rights Reserved
*
*  MARVELL CONFIDENTIAL
*  The source code contained or described herein and all documents related to
*  the source code ("Material") are owned by Marvell International Ltd or its
*  suppliers or licensors. Title to the Material remains with Marvell
*  International Ltd or its suppliers and licensors. The Material contains
*  trade secrets and proprietary and confidential information of Marvell or its
*  suppliers and licensors. The Material is protected by worldwide copyright
*  and trade secret laws and treaty provisions. No part of the Material may be
*  used, copied, reproduced, modified, published, uploaded, posted,
*  transmitted, distributed, or disclosed in any way without Marvell's prior
*  express written permission.
*
*  No license under any patent, copyright, trade secret or other intellectual
*  property right is granted to or conferred upon you by disclosure or delivery
*  of the Materials, either expressly, by implication, inducement, estoppel or
*  otherwise. Any license under such intellectual property rights must be
*  express and approved by Marvell in writing.
*
*/

/****************************************************************************//*
 * @version  V1.0.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 ******************************************************************************/


#ifndef _UART_REG_H
#define _UART_REG_H

struct uart_reg {
    /* 0x00: Receive Buffer Register */
    /* 0x00: Transmit Holding Register */
    /* 0x00: Divisor Latch Low Byte Registers */
    union {
        struct {
            uint32_t BYTE_0         :  8;  /* [7:0]   r/o */
            uint32_t BYTE_1         :  8;  /* [15:8]  r/o */
            uint32_t BYTE_2         :  8;  /* [23:16] r/o */
            uint32_t BYTE_3         :  8;  /* [31:24] r/o */
        } BF_RBR;
        struct {
            uint32_t BYTE_0         :  8;  /* [7:0]   w/o */
            uint32_t BYTE_1         :  8;  /* [15:8]  w/o */
            uint32_t BYTE_2         :  8;  /* [23:16] w/o */
            uint32_t BYTE_3         :  8;  /* [31:24] w/o */
        } BF_THR;
        struct {
            uint32_t DLL            :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8  : 24;  /* [31:8]  rsvd */
        } BF_DLL;
        uint32_t WORDVAL;
    } RBR_THR_DLL;

    /* 0x04: Divisor Latch High Byte Registers */
    /* 0x04: Interrupt Enable Register */
    union {
        struct {
            uint32_t DLH            :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8  : 24;  /* [31:8]  rsvd */
        } BF_DLH;
        struct {
            uint32_t RAVIE          :  1;  /* [0]     r/w */
            uint32_t TIE            :  1;  /* [1]     r/w */
            uint32_t RLSE           :  1;  /* [2]     r/w */
            uint32_t MIE            :  1;  /* [3]     r/w */
            uint32_t RTOIE          :  1;  /* [4]     r/w */
            uint32_t NRZE           :  1;  /* [5]     r/w */
            uint32_t UUE            :  1;  /* [6]     r/w */
            uint32_t DMAE           :  1;  /* [7]     r/w */
            uint32_t HSE            :  1;  /* [8]     r/w */
            uint32_t RESERVED_31_9  : 23;  /* [31:9]  rsvd */
        } BF_IER;
        uint32_t WORDVAL;
    } DLH_IER;

    /* 0x08: Interrupt Identification Register */
    /* 0x08: FIFO Control Register */
    union {
        struct {
            uint32_t NIP            :  1;  /* [0]     r/o */
            uint32_t IID10          :  2;  /* [2:1]   r/o */
            uint32_t TOD            :  1;  /* [3]     r/o */
            uint32_t ABL            :  1;  /* [4]     r/o */
            uint32_t EOC            :  1;  /* [5]     r/o */
            uint32_t FIFOES10       :  2;  /* [7:6]   r/o */
            uint32_t RESERVED_31_8  : 24;  /* [31:8]  rsvd */
        } BF_IIR;
        struct {
            uint32_t TRFIFOE        :  1;  /* [0]     w/o */
            uint32_t RESETRF        :  1;  /* [1]     w/o */
            uint32_t RESETTF        :  1;  /* [2]     w/o */
            uint32_t TIL            :  1;  /* [3]     w/o */
            uint32_t RESERVED_4     :  1;  /* [4]     rsvd */
            uint32_t BUS            :  1;  /* [5]     w/o */
            uint32_t ITL            :  2;  /* [7:6]   w/o */
            uint32_t RESERVED_31_8  : 24;  /* [31:8]  rsvd */
        } BF_FCR;
        uint32_t WORDVAL;
    } IIR_FCR;

    /* 0x0c: Line Control Register */
    union {
        struct {
            uint32_t WLS10          :  2;  /* [1:0]   r/w */
            uint32_t STB            :  1;  /* [2]     r/w */
            uint32_t PEN            :  1;  /* [3]     r/w */
            uint32_t EPS            :  1;  /* [4]     r/w */
            uint32_t STKYP          :  1;  /* [5]     r/w */
            uint32_t SB             :  1;  /* [6]     r/w */
            uint32_t DLAB           :  1;  /* [7]     r/w */
            uint32_t RESERVED_31_8  : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } LCR;

    /* 0x10: Modem Control Register */
    union {
        struct {
            uint32_t DTR            :  1;  /* [0]     r/w */
            uint32_t RTS            :  1;  /* [1]     r/w */
            uint32_t OUT1           :  1;  /* [2]     r/w */
            uint32_t OUT2           :  1;  /* [3]     r/w */
            uint32_t LOOP           :  1;  /* [4]     r/w */
            uint32_t AFE            :  1;  /* [5]     r/w */
            uint32_t RESERVED_31_6  : 26;  /* [31:6]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } MCR;

    /* 0x14: Line Status Register */
    union {
        struct {
            uint32_t DR             :  1;  /* [0]     r/o */
            uint32_t OE             :  1;  /* [1]     r/o */
            uint32_t PE             :  1;  /* [2]     r/o */
            uint32_t FE             :  1;  /* [3]     r/o */
            uint32_t BI             :  1;  /* [4]     r/o */
            uint32_t TDRQ           :  1;  /* [5]     r/o */
            uint32_t TEMT           :  1;  /* [6]     r/o */
            uint32_t FIFOE          :  1;  /* [7]     r/o */
            uint32_t RESERVED_31_8  : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } LSR;

    /* 0x18: Modem Status Register */
    union {
        struct {
            uint32_t DCTS           :  1;  /* [0]     r/o */
            uint32_t DDSR           :  1;  /* [1]     r/o */
            uint32_t TERI           :  1;  /* [2]     r/o */
            uint32_t DDCD           :  1;  /* [3]     r/o */
            uint32_t CTS            :  1;  /* [4]     r/o */
            uint32_t DSR            :  1;  /* [5]     r/o */
            uint32_t RI             :  1;  /* [6]     r/o */
            uint32_t DCD            :  1;  /* [7]     r/o */
            uint32_t RESERVED_31_8  : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } MSR;

    /* 0x1c: Scratchpad Register */
    union {
        struct {
            uint32_t SCRATCHPAD     :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8  : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } SCR;

    /* 0x20: Infrared Selection Register */
    union {
        struct {
            uint32_t XMITIR         :  1;  /* [0]     r/w */
            uint32_t RCVEIR         :  1;  /* [1]     r/w */
            uint32_t XMODE          :  1;  /* [2]     r/w */
            uint32_t TXPL           :  1;  /* [3]     r/w */
            uint32_t RXPL           :  1;  /* [4]     r/w */
            uint32_t RESERVED_31_5  : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ISR;

    /* 0x24: Receive FIFO Occupancy Register */
    union {
        struct {
            uint32_t BYTE_COUNT     :  6;  /* [5:0]   r/o */
            uint32_t RESERVED_31_6  : 26;  /* [31:6]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RFOR;

    /* 0x28: Auto-Baud Control Register */
    union {
        struct {
            uint32_t ABE            :  1;  /* [0]     r/w */
            uint32_t ABLIE          :  1;  /* [1]     r/w */
            uint32_t ABUP           :  1;  /* [2]     r/w */
            uint32_t ABT            :  1;  /* [3]     r/w */
            uint32_t RESERVED_31_4  : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ABR;

    /* 0x2c: Auto-Baud Count Register */
    union {
        struct {
            uint32_t COUNT_VALUE    : 16;  /* [15:0]  r/o */
            uint32_t RESERVED_31_16 : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } ACR;

};

typedef volatile struct uart_reg uart_reg_t;

#ifdef UART_IMPL
BEGIN_REG_SECTION(uart_registers)
uart_reg_t UARTREG;
END_REG_SECTION(uart_registers)
#else
extern uart_reg_t UARTREG;
#endif

#endif /* _UART_REG_H */
