/** @file qspi_reg.h
*
*  @brief Automatically generated register structure
*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
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

/****************************************************************************//**
 * @file     qspi_reg.h
 * @brief    Automatically generated register structure.
 * @version  V1.3.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 *
 *******************************************************************************/

#ifndef _QSPI_REG_H
#define _QSPI_REG_H

struct qspi_reg {
    /* 0x00: Serial Interface Control */
    union {
        struct {
            uint32_t SS_EN              :  1;  /* [0]     r/w */
            uint32_t XFER_RDY           :  1;  /* [1]     r/o */
            uint32_t RESERVED_3_2       :  2;  /* [3:2]   rsvd */
            uint32_t RFIFO_EMPTY        :  1;  /* [4]     r/o */
            uint32_t RFIFO_FULL         :  1;  /* [5]     r/o */
            uint32_t WFIFO_EMPTY        :  1;  /* [6]     r/o */
            uint32_t WFIFO_FULL         :  1;  /* [7]     r/o */
            uint32_t RFIFO_UNDRFLW      :  1;  /* [8]     r/o */
            uint32_t RFIFO_OVRFLW       :  1;  /* [9]     r/o */
            uint32_t WFIFO_UNDRFLW      :  1;  /* [10]    r/o */
            uint32_t WFIFO_OVRFLW       :  1;  /* [11]    r/o */
            uint32_t RESERVED_31_12     : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CNTL;

    /* 0x04: Serial Interface Configuration */
    union {
        struct {
            uint32_t CLK_PRESCALE       :  5;  /* [4:0]   r/w */
            uint32_t BYTE_LEN           :  1;  /* [5]     r/w */
            uint32_t RESERVED_6         :  1;  /* [6]     rsvd */
            uint32_t CLK_PHA            :  1;  /* [7]     r/w */
            uint32_t CLK_POL            :  1;  /* [8]     r/w */
            uint32_t FIFO_FLUSH         :  1;  /* [9]     r/w */
            uint32_t DATA_PIN           :  2;  /* [11:10] r/w */
            uint32_t ADDR_PIN           :  1;  /* [12]    r/w */
            uint32_t RW_EN              :  1;  /* [13]    r/w */
            uint32_t XFER_STOP          :  1;  /* [14]    r/w */
            uint32_t XFER_START         :  1;  /* [15]    r/w */
            uint32_t RESERVED_31_16     : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CONF;

    /* 0x08: Serial Interface Data Out */
    union {
        struct {
            uint32_t DATA_OUT           : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } DOUT;

    /* 0x0c: Serial Interface Data In */
    union {
        struct {
            uint32_t DATA_IN            : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DIN;

    /* 0x10: Serial Interface Instruction */
    union {
        struct {
            uint32_t INSTR              : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16     : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INSTR;

    /* 0x14: Serial Interface Address */
    union {
        struct {
            uint32_t ADDR               : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } ADDR;

    /* 0x18: Serial Interface Read Mode */
    union {
        struct {
            uint32_t RMODE              : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16     : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RDMODE;

    /* 0x1c: Serial Interface Header Count */
    union {
        struct {
            uint32_t INSTR_CNT          :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_3_2       :  2;  /* [3:2]   rsvd */
            uint32_t ADDR_CNT           :  3;  /* [6:4]   r/w */
            uint32_t RESERVED_7         :  1;  /* [7]     rsvd */
            uint32_t RM_CNT             :  2;  /* [9:8]   r/w */
            uint32_t RESERVED_11_10     :  2;  /* [11:10] rsvd */
            uint32_t DUMMY_CNT          :  2;  /* [13:12] r/w */
            uint32_t RESERVED_31_14     : 18;  /* [31:14] rsvd */
        } BF;
        uint32_t WORDVAL;
    } HDRCNT;

    /* 0x20: Serial Interface Data In Count */
    union {
        struct {
            uint32_t DATA_IN_CNT        : 20;  /* [19:0]  r/w */
            uint32_t RESERVED_31_20     : 12;  /* [31:20] rsvd */
        } BF;
        uint32_t WORDVAL;
    } DINCNT;

    /* 0x24: Serial Interface Timing */
    union {
        struct {
            uint32_t DATA_IN_DLY        :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_3_2       :  2;  /* [3:2]   rsvd */
            uint32_t CLK_IN_DLY         :  2;  /* [5:4]   r/w */
            uint32_t CLK_CAPT_EDGE      :  1;  /* [6]     r/w */
            uint32_t RESERVED_7         :  1;  /* [7]     rsvd */
            uint32_t CLK_OUT_DLY        :  2;  /* [9:8]   r/w */
            uint32_t RESERVED_31_10     : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } TIMING;

    /* 0x28: Serial Interface Configuration 2 */
    union {
        struct {
            uint32_t SRST               :  1;  /* [0]     r/w */
            uint32_t DMA_RD_EN          :  1;  /* [1]     r/w */
            uint32_t DMA_WR_EN          :  1;  /* [2]     r/w */
            uint32_t RESERVED_7_3       :  5;  /* [7:3]   rsvd */
            uint32_t DMA_RD_BURST       :  2;  /* [9:8]   r/w */
            uint32_t RESERVED_11_10     :  2;  /* [11:10] rsvd */
            uint32_t DMA_WR_BURST       :  2;  /* [13:12] r/w */
            uint32_t RESERVED_31_14     : 18;  /* [31:14] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CONF2;

    /* 0x2c: Serial Interface Interrupt Status */
    union {
        struct {
            uint32_t XFER_DONE_IS       :  1;  /* [0]     r/o */
            uint32_t XFER_RDY_IS        :  1;  /* [1]     r/o */
            uint32_t RFIFO_DMA_BURST_IS :  1;  /* [2]     r/o */
            uint32_t WFIFO_DMA_BURST_IS :  1;  /* [3]     r/o */
            uint32_t RFIFO_EMPTY_IS     :  1;  /* [4]     r/o */
            uint32_t RFIFO_FULL_IS      :  1;  /* [5]     r/o */
            uint32_t WFIFO_EMPTY_IS     :  1;  /* [6]     r/o */
            uint32_t WFIFO_FULL_IS      :  1;  /* [7]     r/o */
            uint32_t RFIFO_UNDRFLW_IS   :  1;  /* [8]     r/o */
            uint32_t RFIFO_OVRFLW_IS    :  1;  /* [9]     r/o */
            uint32_t WFIFO_UNDRFLW_IS   :  1;  /* [10]    r/o */
            uint32_t WFIFO_OVRFLW_IS    :  1;  /* [11]    r/o */
            uint32_t RESERVED_31_12     : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } ISR;

    /* 0x30: Serial Interface Interrupt Mask */
    union {
        struct {
            uint32_t XFER_DONE_IM       :  1;  /* [0]     r/o */
            uint32_t XFER_RDY_IM        :  1;  /* [1]     r/o */
            uint32_t RFIFO_DMA_BURST_IM :  1;  /* [2]     r/o */
            uint32_t WFIFO_DMA_BURST_IM :  1;  /* [3]     r/o */
            uint32_t RFIFO_EMPTY_IM     :  1;  /* [4]     r/o */
            uint32_t RFIFO_FULL_IM      :  1;  /* [5]     r/o */
            uint32_t WFIFO_EMPTY_IM     :  1;  /* [6]     r/o */
            uint32_t WFIFO_FULL_IM      :  1;  /* [7]     r/o */
            uint32_t RFIFO_UNDRFLW_IM   :  1;  /* [8]     r/o */
            uint32_t RFIFO_OVRFLW_IM    :  1;  /* [9]     r/o */
            uint32_t WFIFO_UNDRFLW_IM   :  1;  /* [10]    r/o */
            uint32_t WFIFO_OVRFLW_IM    :  1;  /* [11]    r/o */
            uint32_t RESERVED_31_12     : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } IMR;

    /* 0x34: Serial Interface Interrupt Raw Status */
    union {
        struct {
            uint32_t XFER_DONE_IR       :  1;  /* [0]     r/o */
            uint32_t XFER_RDY_IR        :  1;  /* [1]     r/o */
            uint32_t RFIFO_DMA_BURST_IR :  1;  /* [2]     r/o */
            uint32_t WFIFO_DMA_BURST_IR :  1;  /* [3]     r/o */
            uint32_t RFIFO_EMPTY_IR     :  1;  /* [4]     r/o */
            uint32_t RFIFO_FULL_IR      :  1;  /* [5]     r/o */
            uint32_t WFIFO_EMPTY_IR     :  1;  /* [6]     r/o */
            uint32_t WFIFO_FULL_IR      :  1;  /* [7]     r/o */
            uint32_t RFIFO_UNDRFLW_IR   :  1;  /* [8]     r/o */
            uint32_t RFIFO_OVRFLW_IR    :  1;  /* [9]     r/o */
            uint32_t WFIFO_UNDRFLW_IR   :  1;  /* [10]    r/o */
            uint32_t WFIFO_OVRFLW_IR    :  1;  /* [11]    r/o */
            uint32_t RESERVED_31_12     : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } IRSR;

    /* 0x38: Serial Interface Interrupt Clear */
    union {
        struct {
            uint32_t XFER_DONE_IC       :  1;  /* [0]     w/o */
            uint32_t RESERVED_31_1      : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ISC;

};

typedef volatile struct qspi_reg qspi_reg_t;

#ifdef QSPI_IMPL
BEGIN_REG_SECTION(qspi_registers)
qspi_reg_t QSPIREG;
END_REG_SECTION(qspi_registers)
#else
extern qspi_reg_t QSPIREG;
#endif

#endif /* _QSPI_REG_H */

