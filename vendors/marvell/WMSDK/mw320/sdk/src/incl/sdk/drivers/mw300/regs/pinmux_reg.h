/** @file pinmux_reg.h
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

#ifndef _PINMUX_REG_H                                         
#define _PINMUX_REG_H                                         
                                                               
struct pinmux_reg {                                           
    /* 0x000-0x13c: GPIO_PINMUX Configuration register */
    union {                                                    
        struct {                                               
            uint32_t FSEL_XR          :  3;  /* [2:0]   r/w */ 
            uint32_t DI_EN            :  1;  /* [3]     r/w */
            uint32_t RESERVED_12_4    :  9;  /* [12:4]  rsvd */
            uint32_t PIO_PULLDN_R     :  1;  /* [13]    r/w */ 
            uint32_t PIO_PULLUP_R     :  1;  /* [14]    r/w */ 
            uint32_t PIO_PULL_SEL_R   :  1;  /* [15]    r/w */ 
            uint32_t RESERVED_31_16   : 16;  /* [31:16] rsvd */ 
        } BF;                                                  
        uint32_t WORDVAL;                                      
    } GPIO_PINMUX[80];   
                                           
};                                                             
                                                               
typedef volatile struct pinmux_reg pinmux_reg_t;             

#ifdef PINMUX_IMPL
BEGIN_REG_SECTION(pinmux_registers)
pinmux_reg_t PINMUXREG;
END_REG_SECTION(pinmux_registers)
#else
extern pinmux_reg_t PINMUXREG;
#endif

#endif /* _PINMUX_REG_H */
