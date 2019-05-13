/** @file rc32m_reg.h
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
 * @file     rc32m_reg.h
 * @brief    Automatically generated register structure.
 * @version  V1.3.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 *
 *******************************************************************************/

#ifndef _RC32M_REG_H
#define _RC32M_REG_H

struct rc32m_reg {
    /* 0x00: Control Register */
    union {
        struct {
            uint32_t EN             :  1;  /* [0]     r/w */
            uint32_t CAL_EN         :  1;  /* [1]     r/w */
            uint32_t EXT_CODE_EN    :  1;  /* [2]     r/w */
            uint32_t PD             :  1;  /* [3]     r/w */
            uint32_t CODE_FR_EXT    :  8;  /* [11:4]  r/w */
            uint32_t CAL_DIV        :  2;  /* [13:12] r/w */
            uint32_t RESERVED_31_14 : 18;  /* [31:14] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CTRL;

    /* 0x04: Status Register */
    union {
        struct {
            uint32_t CLK_RDY        :  1;  /* [0]     r/o */
            uint32_t CAL_DONE       :  1;  /* [1]     r/o */
            uint32_t CODE_FR_CAL    :  8;  /* [9:2]   r/o */
            uint32_t RESERVED_31_10 : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } STATUS;

    /* 0x08: Interrupt Status Register */
    union {
        struct {
            uint32_t CALDON_INT     :  1;  /* [0]     r/o */
            uint32_t CKRDY_INT      :  1;  /* [1]     r/o */
            uint32_t RESERVED_31_2  : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ISR;

    /* 0x0c: Interrupt Mask Register */
    union {
        struct {
            uint32_t CALDON_INT_MSK :  1;  /* [0]     r/w */
            uint32_t CKRDY_INT_MSK  :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2  : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IMR;

    /* 0x10: Interrupt Raw Status Register */
    union {
        struct {
            uint32_t CALDON_INT_RAW :  1;  /* [0]     r/o */
            uint32_t CKRDY_INT_RAW  :  1;  /* [1]     r/o */
            uint32_t RESERVED_31_2  : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IRSR;

    /* 0x14: Interrupt Clear Register */
    union {
        struct {
            uint32_t CALDON_INT_CLR :  1;  /* [0]     r/w */
            uint32_t CKRDY_INT_CLR  :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2  : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ICR;

    /* 0x18: Clock Register */
    union {
        struct {
            uint32_t CLK_INV_SEL    :  1;  /* [0]     r/w */
            uint32_t FORCE_CLK_ON   :  1;  /* [1]     r/w */
            uint32_t REF_SEL        :  1;  /* [2]     r/w */
            uint32_t SOFT_CLK_RST   :  1;  /* [3]     r/w */
            uint32_t RESERVED_31_4  : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK;

    /* 0x1c: Soft Reset Register */
    union {
        struct {
            uint32_t SOFT_RST       :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1  : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RST;

    /* 0x20: Test Register */
    union {
        struct {
            uint32_t TEST_EN        :  1;  /* [0]     r/w */
            uint32_t TEST_SEL       :  2;  /* [2:1]   r/w */
            uint32_t RESERVED_31_3  : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } TEST;

    /* 0x24: Reserved Register */
    union {
        struct {
            uint32_t UNUSED_15_0    : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16 : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RSVD;

};

typedef volatile struct rc32m_reg rc32m_reg_t;

#ifdef RC32M_IMPL
BEGIN_REG_SECTION(rc32m_registers)
rc32m_reg_t RC32MREG;
END_REG_SECTION(rc32m_registers)
#else
extern rc32m_reg_t RC32MREG;
#endif

#endif /* _RC32M_REG_H */

