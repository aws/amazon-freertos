/** @file
*
*  @brief
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
 * @file     aes_reg.h
 * @brief    Automatically generated register structure.
 * @version  V1.3.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 *
 *******************************************************************************/

#ifndef _AES_REG_H
#define _AES_REG_H

struct aes_reg {
    /* 0x00: AES Control Register 1 */
    union {
        struct {
            uint32_t START          :  1;  /* [0]     r/w */
            uint32_t RESERVED_1     :  1;  /* [1]     rsvd */
            uint32_t IF_CLR         :  1;  /* [2]     r/w */
            uint32_t OF_CLR         :  1;  /* [3]     r/w */
            uint32_t OUT_MSG        :  1;  /* [4]     r/w */
            uint32_t OUT_HDR        :  1;  /* [5]     r/w */
            uint32_t RESERVED_7_6   :  2;  /* [7:6]   rsvd */
            uint32_t IO_SRC         :  1;  /* [8]     r/w */
            uint32_t DMA_EN         :  1;  /* [9]     r/w */
            uint32_t KEY_SIZE       :  2;  /* [11:10] r/w */
            uint32_t MIC_LEN        :  2;  /* [13:12] r/w */
            uint32_t OUT_MIC        :  1;  /* [14]    r/w */
            uint32_t DECRYPT        :  1;  /* [15]    r/w */
            uint32_t MODE           :  3;  /* [18:16] r/w */
            uint32_t CTR_MOD        :  7;  /* [25:19] r/w */
            uint32_t RESERVED_31_26 :  6;  /* [31:26] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CTRL1;

    /* 0x04: AES Control Register 2 */
    union {
        struct {
            uint32_t RESET          :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1  : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CTRL2;

    /* 0x08: AES Status Register */
    union {
        struct {
            uint32_t DONE           :  1;  /* [0]     r/o */
            uint32_t RESERVED_3_1   :  3;  /* [3:1]   rsvd */
            uint32_t IF_FULL        :  1;  /* [4]     r/o */
            uint32_t RESERVED_5     :  1;  /* [5]     rsvd */
            uint32_t OF_RDY         :  1;  /* [6]     r/o */
            uint32_t OF_EMPTY       :  1;  /* [7]     r/o */
            uint32_t RESERVED_10_8  :  3;  /* [10:8]  rsvd */
            uint32_t STATUS         :  3;  /* [13:11] r/o */
            uint32_t IF_DEPTH       :  3;  /* [16:14] r/o */
            uint32_t OF_DEPTH       :  3;  /* [19:17] r/o */
            uint32_t RESERVED_31_20 : 12;  /* [31:20] rsvd */
        } BF;
        uint32_t WORDVAL;
    } STATUS;

    /* 0x0c: AES Astr Length Register */
    union {
        struct {
            uint32_t ASTR_LEN       : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } ASTR_LEN;

    /* 0x10: AES Mstr Length Register */
    union {
        struct {
            uint32_t MSTR_LEN       : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } MSTR_LEN;

    /* 0x14: AES Stream Input Register */
    union {
        struct {
            uint32_t STR_IN         : 32;  /* [31:0]  w/o */
        } BF;
        uint32_t WORDVAL;
    } STR_IN;

    /* 0x18-0x24: AES Input Vector Register 0 */
    union {
        struct {
            uint32_t IV0            : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } IV[4];

    /* 0x28-0x44: AES Key 0 */
    union {
        struct {
            uint32_t KEY0           : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } KEY[8];

    /* 0x48: AES Stream Output Port */
    union {
        struct {
            uint32_t STR_OUT        : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } STR_OUT;

    /* 0x4c-0x58: AES Output Vector 0 */
    union {
        struct {
            uint32_t OV0            : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } OV[4];

    /* 0x5c: AES Interrupt Status Register */
    union {
        struct {
            uint32_t STATUS_0       :  1;  /* [0]     r/o */
            uint32_t STATUS_1       :  1;  /* [1]     r/o */
            uint32_t STATUS_2       :  1;  /* [2]     r/o */
            uint32_t RESERVED_31_3  : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ISR;

    /* 0x60: AES Interrupt Mask Register */
    union {
        struct {
            uint32_t MASK_0         :  1;  /* [0]     r/w */
            uint32_t MASK_1         :  1;  /* [1]     r/w */
            uint32_t MASK_2         :  1;  /* [2]     r/w */
            uint32_t RESERVED_31_3  : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IMR;

    /* 0x64: AES Interrupt Raw Status Register */
    union {
        struct {
            uint32_t STATUS_RAW_0   :  1;  /* [0]     r/o */
            uint32_t STATUS_RAW_1   :  1;  /* [1]     r/o */
            uint32_t STATUS_RAW_2   :  1;  /* [2]     r/o */
            uint32_t RESERVED_31_3  : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IRSR;

    /* 0x68: AES Interrupt Clear Register */
    union {
        struct {
            uint32_t CLEAR_0        :  1;  /* [0]     r/w */
            uint32_t CLEAR_1        :  1;  /* [1]     r/w */
            uint32_t CLEAR_2        :  1;  /* [2]     r/w */
            uint32_t RESERVED_31_3  : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ICR;

};

typedef volatile struct aes_reg aes_reg_t;

#ifdef AES_IMPL
BEGIN_REG_SECTION(aes_registers)
aes_reg_t AESREG;
END_REG_SECTION(aes_registers)
#else
extern aes_reg_t AESREG;
#endif

#endif /* _AES_REG_H */

