/** @file crc_reg.h
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
 * @version  V1.3.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 ******************************************************************************/

#ifndef _CRC_REG_H
#define _CRC_REG_H

struct crc_reg {
    /* 0x00: Interrupt Status Register */
    union {
        struct {
            uint32_t STATUS        :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1 : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ISR;

    /* 0x04: Interrupt Raw Status Register */
    union {
        struct {
            uint32_t STATUS_RAW    :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1 : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IRSR;

    /* 0x08: Interrupt Clear Register */
    union {
        struct {
            uint32_t CLEAR         :  1;  /* [0]     w/o */
            uint32_t RESERVED_31_1 : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ICR;

    /* 0x0c: Interrupt Mask Register */
    union {
        struct {
            uint32_t MASK          :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1 : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IMR;

    /* 0x10: CRC Module Control Register */
    union {
        struct {
            uint32_t ENABLE        :  1;  /* [0]     r/w */
            uint32_t MODE          :  3;  /* [3:1]   r/w */
            uint32_t RESERVED_31_4 : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CTRL;

    /* 0x14: Stream Length Minus 1 Register */
    union {
        struct {
            uint32_t LENGTH_M1     : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } STREAM_LEN_M1;

    /* 0x18: Stream Input Register */
    union {
        struct {
            uint32_t DATA          : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } STREAM_IN;

    /* 0x1c: CRC Calculation Result */
    union {
        struct {
            uint32_t DATA          : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } RESULT;

};

typedef volatile struct crc_reg crc_reg_t;

#ifdef CRC_IMPL
BEGIN_REG_SECTION(crc_registers)
crc_reg_t CRCREG;
END_REG_SECTION(crc_registers)
#else
extern crc_reg_t CRCREG;
#endif

#endif /* _CRC_REG_H */
