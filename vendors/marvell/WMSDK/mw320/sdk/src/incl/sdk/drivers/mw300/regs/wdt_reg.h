/** @file wdt_reg.h
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
 * @file     wdt_reg.h
 * @brief    Automatically generated register structure.
 * @version  V1.3.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 * 
 *******************************************************************************/

#ifndef _WDT_REG_H
#define _WDT_REG_H

struct wdt_reg {
    /* 0x00: Control Register */
    union {
        struct {
            uint32_t EN                :  1;  /* [0]     r/w */
            uint32_t RMOD              :  1;  /* [1]     r/w */
            uint32_t RPL               :  3;  /* [4:2]   r/w */
            uint32_t RESERVED          : 27;  /* [31:5]  r/o */
        } BF;
        uint32_t WORDVAL;
    } CR;

    /* 0x04: Timeout Range Register */
    union {
        struct {
            uint32_t TOP               :  4;  /* [3:0]   r/w */
            uint32_t TOP_INIT          :  4;  /* [7:4]   r/w */
            uint32_t RESERVED          : 24;  /* [31:8]  r/o */
        } BF;
        uint32_t WORDVAL;
    } TORR;

    /* 0x08: Current Counter Value Register */
    union {
        struct {
            uint32_t CCVR              : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } CCVR;

    /* 0x0c: Counter Restart Register */
    union {
        struct {
            uint32_t CRR               :  8;  /* [7:0]   w/o */
            uint32_t RESERVED_31_8     : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CRR;

    /* 0x10: Interrupt Status Register */
    union {
        struct {
            uint32_t STAT              :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1     : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } STAT;

    /* 0x14: Interrupt Clear Register */
    union {
        struct {
            uint32_t EOI               :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1     : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } EOI; 

};

typedef volatile struct wdt_reg wdt_reg_t;

#ifdef WDT_IMPL
BEGIN_REG_SECTION(wdt_registers)
wdt_reg_t WDTREG;
END_REG_SECTION(wdt_registers)
#else
extern wdt_reg_t WDTREG;
#endif

#endif /* _WDT_REG_H */

