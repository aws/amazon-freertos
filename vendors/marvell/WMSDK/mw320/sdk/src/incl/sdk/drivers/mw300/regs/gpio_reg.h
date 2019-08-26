/** @file gpio_reg.h
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

#ifndef _GPIO_REG_H
#define _GPIO_REG_H

struct gpio_reg {
    /* 0x00-0x08: GPIO Pin Level Register */
    union {
        struct {
            uint32_t GPLR   : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } GPLR[3];

    /* 0x0c-0x14: GPIO Pin Direction Register */
    union {
        struct {
            uint32_t GPDR   : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } GPDR[3];

    /* 0x18-0x20: GPIO Pin Output Set Register */
    union {
        struct {
            uint32_t GPSR   : 32;  /* [31:0]  w/o */
        } BF;
        uint32_t WORDVAL;
    } GPSR[3];

    /* 0x24-0x2c: GPIO Pin Output Clear Register */
    union {
        struct {
            uint32_t GPCR   : 32;  /* [31:0]  w/o */
        } BF;
        uint32_t WORDVAL;
    } GPCR[3];

    /* 0x30-0x38: GPIO Rising Edge detect Enable Register */
    union {
        struct {
            uint32_t GRER   : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } GRER[3];

    /* 0x3c-0x44: GPIO Falling Edge detect Enable Register */
    union {
        struct {
            uint32_t GFER   : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } GFER[3];

    /* 0x48-0x50: GPIO Edge detect Status Register */
    union {
        struct {
            uint32_t GEDR   : 32;  /* [31:0]  r/w1clr */
        } BF;
        uint32_t WORDVAL;
    } GEDR[3];

    /* 0x54-0x5c: GPIO Pin Bitwise Set Direction Register */
    union {
        struct {
            uint32_t GSDR   : 32;  /* [31:0]  w/o */
        } BF;
        uint32_t WORDVAL;
    } GSDR[3];

    /* 0x60-0x68: GPIO Pin Bitwise Clear Direction Register */
    union {
        struct {
            uint32_t GCDR   : 32;  /* [31:0]  w/o */
        } BF;
        uint32_t WORDVAL;
    } GCDR[3];

    /* 0x6c-0x74: GPIO Bitwise Set Rising Edge detect Enable Register */
    union {
        struct {
            uint32_t GSRER  : 32;  /* [31:0]  w/o */
        } BF;
        uint32_t WORDVAL;
    } GSRER[3];

    /* 0x78-0x80: GPIO Bitwise Clear Rising Edge detect Enable Register */
    union {
        struct {
            uint32_t GCRER  : 32;  /* [31:0]  w/o */
        } BF;
        uint32_t WORDVAL;
    } GCRER[3];

    /* 0x84-0x8c: GPIO Bitwise Set Falling Edge detect Enable Register */
    union {
        struct {
            uint32_t GSFER  : 32;  /* [31:0]  w/o */
        } BF;
        uint32_t WORDVAL;
    } GSFER[3];

    /* 0x90-0x98: GPIO Bitwise Clear Falling Edge detect Enable Register */
    union {
        struct {
            uint32_t GCFER  : 32;  /* [31:0]  w/o */
        } BF;
        uint32_t WORDVAL;
    } GCFER[3];

    /* 0x9c-0xa4: GPIO Bitwise mask of Edge detect Status Register */
    union {
        struct {
            uint32_t APMASK : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } APMASK[3];

};

typedef volatile struct gpio_reg gpio_reg_t;

#ifdef GPIO_IMPL
BEGIN_REG_SECTION(gpio_registers)
gpio_reg_t GPIOREG;
END_REG_SECTION(gpio_registers)
#else
extern gpio_reg_t GPIOREG;
#endif

#endif /* _GPIO_REG_H */
