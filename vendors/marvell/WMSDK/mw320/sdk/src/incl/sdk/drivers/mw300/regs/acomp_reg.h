/** @file acomp_reg.h
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

#ifndef _ACOMP_REG_H
#define _ACOMP_REG_H

struct acomp_reg {
    /* 0x00-0x04: ACOMP0 Control Register */
    union {
        struct {
            uint32_t EN            :  1;  /* [0]     r/w */
            uint32_t GPIOINV       :  1;  /* [1]     r/w */
            uint32_t WARMTIME      :  2;  /* [3:2]   r/w */
            uint32_t HYST_SELN     :  3;  /* [6:4]   r/w */
            uint32_t HYST_SELP     :  3;  /* [9:7]   r/w */
            uint32_t BIAS_PROG     :  2;  /* [11:10] r/w */
            uint32_t LEVEL_SEL     :  6;  /* [17:12] r/w */
            uint32_t NEG_SEL       :  4;  /* [21:18] r/w */
            uint32_t POS_SEL       :  4;  /* [25:22] r/w */
            uint32_t MUXEN         :  1;  /* [26]    r/w */
            uint32_t INACT_VAL     :  1;  /* [27]    r/w */
            uint32_t RIE           :  1;  /* [28]    r/w */
            uint32_t FIE           :  1;  /* [29]    r/w */
            uint32_t INT_ACT_HI    :  1;  /* [30]    r/w */
            uint32_t EDGE_LEVL_SEL :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } CTRL[2];

    /* 0x08-0x0c: ACOMP0 Status Register */
    union {
        struct {
            uint32_t ACT           :  1;  /* [0]     r/o */
            uint32_t OUT           :  1;  /* [1]     r/o */
            uint32_t RESERVED_31_2 : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } STATUS[2];

    /* 0x10-0x14: ACOMP0 Route Register */
    union {
        struct {
            uint32_t OUTSEL        :  1;  /* [0]     r/w */
            uint32_t PE            :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2 : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ROUTE[2];

    /* 0x18-0x1c: ACOMP0 Interrupt Status Register */
    union {
        struct {
            uint32_t OUT_INT       :  1;  /* [0]     r/o */
            uint32_t OUTA_INT      :  1;  /* [1]     r/o */
            uint32_t RESERVED_31_2 : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ISR[2];

    /* 0x20-0x24: ACOMP0 Interrupt Mask Register */
    union {
        struct {
            uint32_t OUT_INT_MASK  :  1;  /* [0]     r/w */
            uint32_t OUTA_INT_MASK :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2 : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IMR[2];

    /* 0x28-0x2c: ACOMP0 Interrupt Raw Status Register */
    union {
        struct {
            uint32_t OUT_INT_RAW   :  1;  /* [0]     r/o */
            uint32_t OUTA_INT_RAW  :  1;  /* [1]     r/o */
            uint32_t RESERVED_31_2 : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IRSR[2];

    /* 0x30-0x34: ACOMP0 Interrupt Clear Register */
    union {
        struct {
            uint32_t OUT_INT_CLR   :  1;  /* [0]     w/o */
            uint32_t OUTA_INT_CLR  :  1;  /* [1]     w/o */
            uint32_t RESERVED_31_2 : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ICR[2];

    /* 0x38-0x3c: ACOMP0 Soft Reset Register */
    union {
        struct {
            uint32_t SOFT_RST      :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1 : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RST[2];

    uint8_t zReserved0x40[8];  /* pad 0x40 - 0x47 */

    /* 0x48: Clock  Register */
    union {
        struct {
            uint32_t FORCE_CLK_ON  :  1;  /* [0]     r/w */
            uint32_t SOFT_CLK_RST  :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2 : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK;

};

typedef volatile struct acomp_reg acomp_reg_t;

#ifdef ACOMP_IMPL
BEGIN_REG_SECTION(acomp_registers)
acomp_reg_t ACOMPREG;
END_REG_SECTION(acomp_registers)
#else
extern acomp_reg_t ACOMPREG;
#endif

#endif /* _ACOMP_REG_H */
