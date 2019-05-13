/** @file i2c_reg.h
*
*  @brief This file contains Automatically generated register structure
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
 * @version  V1.3.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 ******************************************************************************/

#ifndef _I2C_REG_H
#define _I2C_REG_H

struct i2c_reg {
    /* 0x00: I2C Control Register */
    union {
        struct {
            uint32_t MASTER_MODE              :  1;  /* [0]     r/w */
            uint32_t SPEED                    :  2;  /* [2:1]   r/w */
            uint32_t BITADDR10_SLAVE          :  1;  /* [3]     r/w */
            uint32_t BITADDR10_MASTER_RD_ONLY :  1;  /* [4]     r/o */
            uint32_t RESTART_EN               :  1;  /* [5]     r/w */
            uint32_t SLAVE_DISABLE            :  1;  /* [6]     r/w */
            uint32_t RESERVED_31_7            : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CON;

    /* 0x04: I2C Target Address Register */
    union {
        struct {
            uint32_t TAR                      : 10;  /* [9:0]   r/w */
            uint32_t GC_OR_START              :  1;  /* [10]    r/w */
            uint32_t SPECIAL                  :  1;  /* [11]    r/w */
            uint32_t BITADDR10_MASTER         :  1;  /* [12]    r/w */
            uint32_t RESERVED_31_13           : 19;  /* [31:13] rsvd */
        } BF;
        uint32_t WORDVAL;
    } TAR;

    /* 0x08: I2C Slave Address Register */
    union {
        struct {
            uint32_t SAR                      : 10;  /* [9:0]   r/w */
            uint32_t RESERVED_31_10           : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } SAR;

    /* 0x0c: I2C High Speed Master Mode Code Address Register */
    union {
        struct {
            uint32_t HS_MAR                   :  3;  /* [2:0]   r/w */
            uint32_t RESERVED_31_3            : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } HS_MADDR;

    /* 0x10: I2C Rx/Tx Data Buffer and Command Register */
    union {
        struct {
            uint32_t DAT                      :  8;  /* [7:0]   r/w */
            uint32_t CMD                      :  1;  /* [8]     r/w */
            uint32_t RESERVED_31_9            : 23;  /* [31:9]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DATA_CMD;

    /* 0x14: Standard Speed I2C Clock SCL High Count Register */
    union {
        struct {
            uint32_t SS_SCL_HCNT              : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16           : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } SS_SCL_HCNT;

    /* 0x18: Standard Speed I2C Clock SCL Low Count Register */
    union {
        struct {
            uint32_t SS_SCL_LCNT              : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16           : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } SS_SCL_LCNT;

    /* 0x1c: Fast Speed I2C Clock SCL High Count Register */
    union {
        struct {
            uint32_t FS_SCL_HCNT              : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16           : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } FS_SCL_HCNT;

    /* 0x20: Fast Speed I2C Clock SCL Low Count Register */
    union {
        struct {
            uint32_t FS_SCL_LCNT              : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16           : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } FS_SCL_LCNT;

    /* 0x24: High Speed I2C Clock SCL High Count Register */
    union {
        struct {
            uint32_t HS_SCL_HCNT              : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16           : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } HS_SCL_HCNT;

    /* 0x28: High Speed I2C Clock SCL Low Count Register */
    union {
        struct {
            uint32_t HS_SCL_LCNT              : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16           : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } HS_SCL_LCNT;

    /* 0x2c: I2C Interrupt Status Register */
    union {
        struct {
            uint32_t R_RX_UNDER               :  1;  /* [0]     r/o */
            uint32_t R_RX_OVER                :  1;  /* [1]     r/o */
            uint32_t R_RX_FULL                :  1;  /* [2]     r/o */
            uint32_t R_TX_OVER                :  1;  /* [3]     r/o */
            uint32_t R_TX_EMPTY               :  1;  /* [4]     r/o */
            uint32_t R_RD_REQ                 :  1;  /* [5]     r/o */
            uint32_t R_TX_ABRT                :  1;  /* [6]     r/o */
            uint32_t R_RX_DONE                :  1;  /* [7]     r/o */
            uint32_t R_ACTIVITY               :  1;  /* [8]     r/o */
            uint32_t R_STOP_DET               :  1;  /* [9]     r/o */
            uint32_t R_START_DET              :  1;  /* [10]    r/o */
            uint32_t R_GEN_CALL               :  1;  /* [11]    r/o */
            uint32_t RESERVED_31_12           : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INTR_STAT;

    /* 0x30: I2C Interrupt Mask Register */
    union {
        struct {
            uint32_t M_RX_UNDER               :  1;  /* [0]     r/w */
            uint32_t M_RX_OVER                :  1;  /* [1]     r/w */
            uint32_t M_RX_FULL                :  1;  /* [2]     r/w */
            uint32_t M_TX_OVER                :  1;  /* [3]     r/w */
            uint32_t M_TX_EMPTY               :  1;  /* [4]     r/w */
            uint32_t M_RD_REQ                 :  1;  /* [5]     r/w */
            uint32_t M_TX_ABRT                :  1;  /* [6]     r/w */
            uint32_t M_RX_DONE                :  1;  /* [7]     r/w */
            uint32_t M_ACTIVITY               :  1;  /* [8]     r/w */
            uint32_t M_STOP_DET               :  1;  /* [9]     r/w */
            uint32_t M_START_DET              :  1;  /* [10]    r/w */
            uint32_t M_GEN_CALL               :  1;  /* [11]    r/w */
            uint32_t RESERVED_31_12           : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INTR_MASK;

    /* 0x34: I2C Raw Interrupt Status Register */
    union {
        struct {
            uint32_t RX_UNDER                 :  1;  /* [0]     r/o */
            uint32_t RX_OVER                  :  1;  /* [1]     r/o */
            uint32_t RX_FULL                  :  1;  /* [2]     r/o */
            uint32_t TX_OVER                  :  1;  /* [3]     r/o */
            uint32_t TX_EMPTY                 :  1;  /* [4]     r/o */
            uint32_t RD_REQ                   :  1;  /* [5]     r/o */
            uint32_t TX_ABRT                  :  1;  /* [6]     r/o */
            uint32_t RX_DONE                  :  1;  /* [7]     r/o */
            uint32_t ACTIVITY                 :  1;  /* [8]     r/o */
            uint32_t STOP_DET                 :  1;  /* [9]     r/o */
            uint32_t START_DET                :  1;  /* [10]    r/o */
            uint32_t GEN_CALL                 :  1;  /* [11]    r/o */
            uint32_t RESERVED_31_12           : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RAW_INTR_STAT;

    /* 0x38: I2C Receive FIFO Threshold Register */
    union {
        struct {
            uint32_t RX_TL                    :  5;  /* [4:0]   r/w */
            uint32_t RESERVED_31_5            : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RX_TL;

    /* 0x3c: I2C Transmit FIFO Threshold Register */
    union {
        struct {
            uint32_t TX_TL                    :  5;  /* [4:0]   r/w */
            uint32_t RESERVED_31_5            : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } TX_TL;

    /* 0x40: Clear Combined and Individual Interrupt Register */
    union {
        struct {
            uint32_t CLR_INTR                 :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1            : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLR_INTR;

    /* 0x44: Clear RX_UNDER Interrupt Register */
    union {
        struct {
            uint32_t CLR_RX_UNDER             :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1            : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLR_RX_UNDER;

    /* 0x48: Clear RX_OVER Interrupt Register */
    union {
        struct {
            uint32_t CLR_RX_OVER              :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1            : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLR_RX_OVER;

    /* 0x4c: Clear TX_OVER Interrupt Register */
    union {
        struct {
            uint32_t CLR_TX_OVER              :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1            : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLR_TX_OVER;

    /* 0x50: Clear RD_REQ Interrupt Register */
    union {
        struct {
            uint32_t CLR_RD_REQ               :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1            : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLR_RD_REQ;

    /* 0x54: Clear TX_ABRT Interrupt Register */
    union {
        struct {
            uint32_t CLR_TX_ABRT              :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1            : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLR_TX_ABRT;

    /* 0x58: Clear RX_DONE Interrupt Register */
    union {
        struct {
            uint32_t CLR_RX_DONE              :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1            : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLR_RX_DONE;

    /* 0x5c: Clear ACTIVITY Interrupt Register */
    union {
        struct {
            uint32_t CLR_ACTIVITY             :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1            : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLR_ACTIVITY;

    /* 0x60: Clear STOP_DET Interrupt Register */
    union {
        struct {
            uint32_t CLR_STOP_DET             :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1            : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLR_STOP_DET;

    /* 0x64: Clear START_DET Interrupt Register */
    union {
        struct {
            uint32_t CLR_START_DET            :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1            : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLR_START_DET;

    /* 0x68: Clear GEN_CALL Interrupt Register */
    union {
        struct {
            uint32_t CLR_GEN_CALL             :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1            : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLR_GEN_CALL;

    /* 0x6c: I2C Enable Register */
    union {
        struct {
            uint32_t ENABLE                   :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1            : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ENABLE;

    /* 0x70: I2C Status Register */
    union {
        struct {
            uint32_t ACTIVITY                 :  1;  /* [0]     r/o */
            uint32_t TFNF                     :  1;  /* [1]     r/o */
            uint32_t TFE                      :  1;  /* [2]     r/o */
            uint32_t RFNE                     :  1;  /* [3]     r/o */
            uint32_t RFF                      :  1;  /* [4]     r/o */
            uint32_t MST_ACTIVITY             :  1;  /* [5]     r/o */
            uint32_t SLV_ACTIVITY             :  1;  /* [6]     r/o */
            uint32_t RESERVED_31_7            : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } STATUS;

    /* 0x74: I2C Transmit FIFO Level Register */
    union {
        struct {
            uint32_t TXFLR                    :  5;  /* [4:0]   r/o */
            uint32_t RESERVED_31_5            : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } TXFLR;

    /* 0x78: I2C Receive FIFO Level Register */
    union {
        struct {
            uint32_t RXFLR                    :  5;  /* [4:0]   r/o */
            uint32_t RESERVED_31_5            : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RXFLR;

    uint8_t zReserved0x7c[4];  /* pad 0x7c - 0x7f */

    /* 0x80: I2C Transmit Abort Source Register */
    union {
        struct {
            uint32_t ABRT_7B_ADDR_NOACK       :  1;  /* [0]     r/o */
            uint32_t ABRT_10ADDR1_NOACK       :  1;  /* [1]     r/o */
            uint32_t ABRT_10ADDR2_NOACK       :  1;  /* [2]     r/o */
            uint32_t ABRT_TXDATA_NOACK        :  1;  /* [3]     r/o */
            uint32_t ABRT_GCALL_NOACK         :  1;  /* [4]     r/o */
            uint32_t ABRT_GCALL_READ          :  1;  /* [5]     r/o */
            uint32_t ABRT_HS_ACKDET           :  1;  /* [6]     r/o */
            uint32_t ABRT_SBYTE_ACKDET        :  1;  /* [7]     r/o */
            uint32_t ABRT_HS_NORSTRT          :  1;  /* [8]     r/o */
            uint32_t ABRT_SBYTE_NORSTRT       :  1;  /* [9]     r/o */
            uint32_t ABRT_10B_RD_NORSTRT      :  1;  /* [10]    r/o */
            uint32_t ABRT_MASTER_DIS          :  1;  /* [11]    r/o */
            uint32_t ARB_LOST                 :  1;  /* [12]    r/o */
            uint32_t ABRT_SLVFLUSH_TXFIFO     :  1;  /* [13]    r/o */
            uint32_t ABRT_SLV_ARBLOST         :  1;  /* [14]    r/o */
            uint32_t ABRT_SLVRD_INTX          :  1;  /* [15]    r/o */
            uint32_t RESERVED_31_16           : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } TX_ABRT_SOURCE;

    uint8_t zReserved0x84[4];  /* pad 0x84 - 0x87 */

    /* 0x88: DMA Control Register */
    union {
        struct {
            uint32_t RDMAE                    :  1;  /* [0]     r/w */
            uint32_t TDMAE                    :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2            : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DMA_CR;

    /* 0x8c: DMA Transmit Data Level Register */
    union {
        struct {
            uint32_t DMATDL                   :  4;  /* [3:0]   r/w */
            uint32_t RESERVED_31_4            : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DMA_TDLR;

    /* 0x90: I2C Receive Data Level Register */
    union {
        struct {
            uint32_t DMARDL                   :  4;  /* [3:0]   r/w */
            uint32_t RESERVED_31_4            : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DMA_RDLR;

    /* 0x94: I2C SDA Setup Register */
    union {
        struct {
            uint32_t SDA_SETUP                :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8            : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } SDA_SETUP;

    /* 0x98: I2C ACK General Call Register */
    union {
        struct {
            uint32_t ACK_GEN_CALL             :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1            : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ACK_GENERAL_CALL;

    /* 0x9c: I2C Enable Status Register */
    union {
        struct {
            uint32_t EN                       :  1;  /* [0]     r/o */
            uint32_t SLV_DISABLED_WHILE_BUSY  :  1;  /* [1]     r/o */
            uint32_t SLV_RX_DATA_LOST         :  1;  /* [2]     r/o */
            uint32_t RESERVED_31_3            : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ENABLE_STATUS;

};

typedef volatile struct i2c_reg i2c_reg_t;

#ifdef I2C_IMPL
BEGIN_REG_SECTION(i2c_registers)
i2c_reg_t I2CREG;
END_REG_SECTION(i2c_registers)
#else
extern i2c_reg_t I2CREG;
#endif

#endif /* _I2C_REG_H */
