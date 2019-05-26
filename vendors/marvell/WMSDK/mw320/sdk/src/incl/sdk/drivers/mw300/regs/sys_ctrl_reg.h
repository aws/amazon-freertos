/** @file sys_ctrl_reg.h
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
 * 
 * Date:     Fri Oct 25 01:32:58 AM PDT 2013
 * User:     chengwen
 * Hostname: sds-159s
 * OS:       Linux 2.6.18-238.12.1.el5
 * Midas:    V1.02 Thu Sep 13 12:12:00 PDT 2012
 * Usergen:  V1.02
 * Path:     /proj/klmcu0/wa/chengwen/klmcu0/top/sim/run
 * Cmdline:  midas -param internal=true -usergen /proj/cadsim/MIDAS/1.02/examples/usergen/cdef2/cdef2.tcl /proj/klmcu0/wa/chengwen/klmcu0/ip/sys_ctrl/doc/sys_ctrl.csv
 */

#ifndef _SYS_CTRL_REG_H
#define _SYS_CTRL_REG_H

struct sys_ctrl_reg {
    /* 0x00: Chip Revision Register */
    union {
        struct {
            uint32_t REV_ID             : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } REV_ID;

    /* 0x04: Memory Space Configuration Register */
    union {
        struct {
            uint32_t CFG                :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_31_2      : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } MEM;

    /* 0x08-0x14: RAM0 Control Register */
    union {
        struct {
            uint32_t RTC                :  2;  /* [1:0]   r/w */
            uint32_t WTC                :  2;  /* [3:2]   r/w */
            uint32_t RESERVED_31_4      : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RAM[4];

    /* 0x18-0x24: Reserved4 */
    union {
        struct {
            uint32_t RSVD_FIELD         : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } RESERVED[4];

    /* 0x28: ROM Control Register */
    union {
        struct {
            uint32_t RTC                :  3;  /* [2:0]   r/w */
            uint32_t RTC_REF            :  2;  /* [4:3]   r/w */
            uint32_t RESERVED_31_5      : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ROM;

    /* 0x2c: AON_MEM Control Register */
    union {
        struct {
            uint32_t RTC                :  2;  /* [1:0]   r/w */
            uint32_t WTC                :  2;  /* [3:2]   r/w */
            uint32_t RESERVED_31_4      : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } AON_MEM;

    uint8_t zReserved0x30[4];  /* pad 0x30 - 0x33 */

    /* 0x34: GPT Pin-in Selection */
    union {
        struct {
            uint32_t SEL                :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1      : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } GPT_IN;

    /* 0x38: Calibration channel selection */
    union {
        struct {
            uint32_t RTC_DUTY           :  1;  /* [0]     r/w */
            uint32_t RTC_TRIG           :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2      : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CAL;

    /* 0x3c: Peripheral SW reset */
    union {
        struct {
            uint32_t WDT_RSTN_EN        :  1;  /* [0]     r/w */
            uint32_t USB_RSTN_EN        :  1;  /* [1]     r/w */
            uint32_t SDIO1_RSTN_EN      :  1;  /* [2]     r/w */
            uint32_t SDIO0_RSTN_EN      :  1;  /* [3]     r/w */
            uint32_t GPT3_RSTN_EN       :  1;  /* [4]     r/w */
            uint32_t GPT2_RSTN_EN       :  1;  /* [5]     r/w */
            uint32_t GPT1_RSTN_EN       :  1;  /* [6]     r/w */
            uint32_t GPT0_RSTN_EN       :  1;  /* [7]     r/w */
            uint32_t SSP2_RSTN_EN       :  1;  /* [8]     r/w */
            uint32_t SSP1_RSTN_EN       :  1;  /* [9]     r/w */
            uint32_t SSP0_RSTN_EN       :  1;  /* [10]    r/w */
            uint32_t I2C2_RSTN_EN       :  1;  /* [11]    r/w */
            uint32_t I2C1_RSTN_EN       :  1;  /* [12]    r/w */
            uint32_t I2C0_RSTN_EN       :  1;  /* [13]    r/w */
            uint32_t RESERVED_3         :  1;  /* [14]    r/w */
            uint32_t UART2_RSTN_EN      :  1;  /* [15]    r/w */
            uint32_t RESERVED_16        :  1;  /* [16]    r/w */
            uint32_t UART0_RSTN_EN      :  1;  /* [17]    r/w */
            uint32_t FLASH_QSPI_RSTN_EN :  1;  /* [18]    r/w */
            uint32_t RESERVED_19        :  1;  /* [19]    r/w */
            uint32_t QSPI0_RSTN_EN      :  1;  /* [20]    r/w */
            uint32_t RESERVED_21        :  1;  /* [21]    r/w */
            uint32_t PDM_RSTN_EN        :  1;  /* [22]    r/w */
            uint32_t RESERVED_23        :  1;  /* [23]    r/w */
            uint32_t RESERVED_24        :  1;  /* [24]    r/w */
            uint32_t RESERVED_25        :  1;  /* [25]    rsvd */
            uint32_t GAU_RSTN_EN        :  1;  /* [26]    r/w */
            uint32_t RESERVED_31_27     :  5;  /* [31:27] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PERI_SW_RST;

    /* 0x40: USB Control Register */
    union {
        struct {
            uint32_t USB_PU_PLL         :  1;  /* [0]     r/w */
            uint32_t USB_PU_OTG         :  1;  /* [1]     r/w */
            uint32_t USB_PU             :  1;  /* [2]     r/w */
            uint32_t REG_RX_PDFVSSM     :  1;  /* [3]     r/w */
            uint32_t REG_RX_PDLVMC      :  1;  /* [4]     r/w */
            uint32_t REG_TX_PDFVSSM     :  1;  /* [5]     r/w */
            uint32_t REG_TX_PDLVMC      :  1;  /* [6]     r/w */
            uint32_t REG_RX_BUF_RTC     :  2;  /* [8:7]   r/w */
            uint32_t REG_RX_BUF_WTC     :  2;  /* [10:9]  r/w */
            uint32_t REG_TX_BUF_RTC     :  2;  /* [12:11] r/w */
            uint32_t REG_TX_BUF_WTC     :  2;  /* [14:13] r/w */
            uint32_t USB_RESUME         :  1;  /* [15]    r/o */
	    uint32_t IDDQ_TEST          :  1;  /* [16]    r/w */ 
	    uint32_t SOFT_PHY_RESET     :  1;  /* [17]    r/w */ 
	    uint32_t PHY_RESET_SEL      :  1;  /* [18]    r/w */ 
	    uint32_t SOFT_UTMI_SESSEND  :  1;  /* [19]    r/w */ 
	    uint32_t SOFT_UTMI_XVALID   :  1;  /* [20]    r/w */ 
	    uint32_t SOFT_UTMI_IDDIG    :  1;  /* [21]    r/w */ 
	    uint32_t MAC_CTRL_SEL       :  1;  /* [22]    r/w */ 
	    uint32_t RESERVED_31_23     :  9;  /* [31:23] rsvd */
        } BF;
        uint32_t WORDVAL;
    } USB_CTRL;

    uint8_t zReserved0x44[8];  /* pad 0x44 - 0x4b */

    /* 0x4c: SDIO Reg File Timing Control */
    union {
        struct {
            uint32_t MMC_RTC            :  2;  /* [1:0]   r/w */
            uint32_t MMC_WTC            :  2;  /* [3:2]   r/w */
            uint32_t RESERVED_31_4      : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } MMC_TC;

    /* 0x50: External Flash select register */
    union {
        struct {
            uint32_t EXTERNAL_FLASH_SEL :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1      : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } EXT_FLASH_SEL;

};

typedef volatile struct sys_ctrl_reg sys_ctrl_reg_t;

#ifdef SYS_CTRL_IMPL
BEGIN_REG_SECTION(sys_ctrl_registers)
sys_ctrl_reg_t SYS_CTRLREG;
END_REG_SECTION(sys_ctrl_registers)
#else
extern sys_ctrl_reg_t SYS_CTRLREG;
#endif

#endif /* _SYS_CTRL_REG_H */
