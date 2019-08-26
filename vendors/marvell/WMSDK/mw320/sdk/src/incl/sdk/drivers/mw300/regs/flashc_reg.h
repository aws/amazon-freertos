/** @file flashc_reg.h
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


/*
 * Automatically generated register structure
 * 
 * Date:     Fri Oct 25 01:32:51 AM PDT 2013
 * User:     chengwen
 * Hostname: sds-159s
 * OS:       Linux 2.6.18-238.12.1.el5
 * Midas:    V1.02 Thu Sep 13 12:12:00 PDT 2012
 * Usergen:  V1.02
 * Path:     /proj/klmcu0/wa/chengwen/klmcu0/top/sim/run
 * Cmdline:  midas -param internal=true -usergen /proj/cadsim/MIDAS/1.02/examples/usergen/cdef2/cdef2.tcl /proj/klmcu0/wa/chengwen/klmcu0/ip/flashc/midas/flashc.csv
 */

#ifndef _FLASHC_REG_H
#define _FLASHC_REG_H

struct flashc_reg {
    /* 0x00: Flash Controller Configuration Registe */
    union {
        struct {
            uint32_t CMD_TYPE             :  4;  /* [3:0]   r/w */
            uint32_t RESERVED_7_4         :  4;  /* [7:4]   rsvd */
            uint32_t CLK_PRESCALE         :  5;  /* [12:8]  r/w */
            uint32_t RESERVED_13          :  1;  /* [13]    rsvd */
            uint32_t CLK_POL              :  1;  /* [14]    r/w */
            uint32_t CLK_PHA              :  1;  /* [15]    r/w */
            uint32_t RESERVED_27_16       : 12;  /* [27:16] rsvd */
            uint32_t SRAM_MODE_EN         :  1;  /* [28]    r/w */
            uint32_t CACHE_LINE_FLUSH     :  1;  /* [29]    r/w */
            uint32_t CACHE_EN             :  1;  /* [30]    r/w */
            uint32_t FLASHC_PAD_EN        :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } FCCR;

    /* 0x04: Flash Controller Timing Register */
    union {
        struct {
            uint32_t DIN_DLY              :  2;  /* [1:0]   r/w */
            uint32_t CLK_IN_DLY           :  2;  /* [3:2]   r/w */
            uint32_t CLK_CAPT_EDGE        :  1;  /* [4]     r/w */
            uint32_t CLK_OUT_DLY          :  2;  /* [6:5]   r/w */
            uint32_t RESERVED_31_7        : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } FCTR;

    /* 0x08: Flash Controller Status Register */
    union {
        struct {
            uint32_t CONT_RD_MD_EXIT_DONE :  1;  /* [0]     r/w1clr */
            uint32_t RESERVED_31_1        : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } FCSR;

    /* 0x0c: Flash Controller Auxiliary Configuration Register */
    union {
        struct {
            uint32_t HIT_CNT_EN           :  1;  /* [0]     r/w */
            uint32_t MISS_CNT_EN          :  1;  /* [1]     r/w */
            uint32_t ADDR_MATCH_EN        :  1;  /* [2]     r/w */
            uint32_t OFFSET_EN            :  1;  /* [3]     r/w */
            uint32_t RESERVED_31_4        : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } FCACR;

    /* 0x10: Flash Controller Hit Count Register */
    union {
        struct {
            uint32_t HIT_COUNT            : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } FCHCR;

    /* 0x14: Flash Controller Miss Count Register */
    union {
        struct {
            uint32_t MISS_COUNT           : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } FCMCR;

    /* 0x18: Flash Address Offset Register */
    union {
        struct {
            uint32_t OFFSET_VAL           : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } FAOFFR;

    /* 0x1c: Flash Address Match register */
    union {
        struct {
            uint32_t ADDR                 : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } FADDMAT;

    uint8_t zReserved0x20[4];  /* pad 0x20 - 0x23 */

    /* 0x24: Flash Controller Configurationb Register2 */
    union {
        struct {
            uint32_t INSTR_CNT            :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_3_2         :  2;  /* [3:2]   rsvd */
            uint32_t ADDR_CNT             :  3;  /* [6:4]   r/w */
            uint32_t RESERVED_7           :  1;  /* [7]     rsvd */
            uint32_t RM_CNT               :  2;  /* [9:8]   r/w */
            uint32_t RESERVED_11_10       :  2;  /* [11:10] rsvd */
            uint32_t DUMMY_CNT            :  2;  /* [13:12] r/w */
            uint32_t RESERVED_26_14       : 13;  /* [26:14] rsvd */
            uint32_t BYTE_LEN             :  1;  /* [27]    r/w */
            uint32_t ADDR_PIN             :  1;  /* [28]    r/w */
            uint32_t DATA_PIN             :  2;  /* [30:29] r/w */
            uint32_t USE_CFG_OVRD         :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } FCCR2;

    /* 0x28: Flash Instruction Register */
    union {
        struct {
            uint32_t INSTR                : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } FINSTR;

    /* 0x2c: Flash Read Mode Register */
    union {
        struct {
            uint32_t RDMODE               : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } FRMR;

};

typedef volatile struct flashc_reg flashc_reg_t;

#ifdef FLASHC_IMPL
BEGIN_REG_SECTION(flashc_registers)
flashc_reg_t FLASHCREG;
END_REG_SECTION(flashc_registers)
#else
extern flashc_reg_t FLASHCREG;
#endif

#endif /* _FLASHC_REG_H */
