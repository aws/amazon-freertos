/** @file rtc_reg.h
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

#ifndef _RTC_REG_H
#define _RTC_REG_H

struct rtc_reg {
    /* 0x00: Counter Enable Register */
    union {
        struct {
            uint32_t CNT_START      :  1;  /* [0]     w/o */
            uint32_t CNT_STOP       :  1;  /* [1]     w/o */
            uint32_t CNT_RESET      :  1;  /* [2]     w/o */
            uint32_t RESERVED_15_3  : 13;  /* [15:3]  rsvd */
            uint32_t CNT_RUN        :  1;  /* [16]    r/o */
            uint32_t CNT_RST_DONE   :  1;  /* [17]    r/o */
            uint32_t STS_RESETN     :  1;  /* [18]    r/o */
            uint32_t RESERVED_31_19 : 13;  /* [31:19] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CNT_EN_REG;

    uint8_t zReserved0x04[28];  /* pad 0x04 - 0x1f */

    /* 0x20: Interrupt Raw Register */
    union {
        struct {
            uint32_t RESERVED_14_0  : 15;  /* [14:0]  rsvd */
            uint32_t CNT_ALARM_INT  :  1;  /* [15]    r/w1clr */
            uint32_t CNT_UPP_INT    :  1;  /* [16]    r/w1clr */
            uint32_t RESERVED_31_17 : 15;  /* [31:17] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INT_RAW_REG;

    /* 0x24: Interrupt Register */
    union {
        struct {
            uint32_t RESERVED_14_0  : 15;  /* [14:0]  rsvd */
            uint32_t CNT_ALARM_INTR :  1;  /* [15]    r/o */
            uint32_t CNT_UPP_INTR   :  1;  /* [16]    r/o */
            uint32_t RESERVED_31_17 : 15;  /* [31:17] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INT_REG;

    /* 0x28: Interrupt Mask Register */
    union {
        struct {
            uint32_t RESERVED_14_0  : 15;  /* [14:0]  rsvd */
            uint32_t CNT_ALARM_MSK  :  1;  /* [15]    r/w */
            uint32_t CNT_UPP_MSK    :  1;  /* [16]    r/w */
            uint32_t RESERVED_31_17 : 15;  /* [31:17] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INT_MSK_REG;

    uint8_t zReserved0x2c[20];  /* pad 0x2c - 0x3f */

    /* 0x40: Counter Control Register */
    union {
        struct {
            uint32_t RESERVED_3_0   :  4;  /* [3:0]   rsvd */
            uint32_t CNT_DBG_ACT    :  1;  /* [4]     r/w */
            uint32_t RESERVED_7_5   :  3;  /* [7:5]   rsvd */
            uint32_t CNT_UPDT_MOD   :  2;  /* [9:8]   r/w */
            uint32_t RESERVED_31_10 : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CNT_CNTL_REG;

    uint8_t zReserved0x44[12];  /* pad 0x44 - 0x4f */

    /* 0x50: Counter Value Register */
    union {
        struct {
            uint32_t CNT_VAL        : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } CNT_VAL_REG;

    uint8_t zReserved0x54[12];  /* pad 0x54 - 0x5f */

    /* 0x60: Counter Upper Value Register */
    union {
        struct {
            uint32_t UPP_VAL        : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } CNT_UPP_VAL_REG;

    uint8_t zReserved0x64[12];  /* pad 0x64 - 0x6f */

    /* 0x70: Counter Alarm Value Register */
    union {
        struct {
            uint32_t ALARM_VAL      : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } CNT_ALARM_VAL_REG;

    uint8_t zReserved0x74[12];  /* pad 0x74 - 0x7f */

    /* 0x80: Clock control register */
    union {
        struct {
            uint32_t RESERVED_7_0   :  8;  /* [7:0]   rsvd */
            uint32_t CLK_DIV        :  4;  /* [11:8]  r/w */
            uint32_t RESERVED_31_12 : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK_CNTL_REG;

};

typedef volatile struct rtc_reg rtc_reg_t;

#ifdef RTC_IMPL
BEGIN_REG_SECTION(rtc_registers)
rtc_reg_t RTCREG;
END_REG_SECTION(rtc_registers)
#else
extern rtc_reg_t RTCREG;
#endif

#endif /* _RTC_REG_H */
