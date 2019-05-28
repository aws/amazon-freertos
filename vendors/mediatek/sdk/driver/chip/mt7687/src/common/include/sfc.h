/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "hal_flash.h"
#ifdef HAL_FLASH_MODULE_ENABLED

#ifndef __SFC_HW_H__
#define __SFC_HW_H__


#include "mt7687_cm4_hw_memmap.h"


#define MAX_SFC_GPRAM_SIZE	160

#define SFC_SECURITY_DISABLE	0x0
#define SFC_SECURITY_ENABLE	0x1


// Register access macros
#define SFC_ReadReg8(addr)          *((volatile unsigned char *)(addr))
#define SFC_ReadReg32(addr)         *((volatile unsigned int *)(addr))
#define SFC_WriteReg32(addr, data)  *((volatile unsigned int *)(addr)) = (unsigned int)(data)


//
#define SFC_IsQPIMode()			(SFC_ReadReg32(RW_SFC_DIRECT_CTL) & SFC_QPI_EN)



// SFI registers
#define RW_SFC_MAC_CTL              (CM4_SFC_BASE + 0x0000)
#define RW_SFC_DIRECT_CTL           (CM4_SFC_BASE + 0x0004)
#define RW_SFC_MISC_CTL             (CM4_SFC_BASE + 0x0008)
#define RW_SFC_MISC_CTL2            (CM4_SFC_BASE + 0x000C)
#define RW_SFC_MAC_OUTL             (CM4_SFC_BASE + 0x0010)
#define RW_SFC_MAC_INL              (CM4_SFC_BASE + 0x0014)
#define RW_SFC_RESET_CTL            (CM4_SFC_BASE + 0x0018)
#define RW_SFC_STA2_CTL             (CM4_SFC_BASE + 0x001C)
#define RW_SFC_DLY_CTL1             (CM4_SFC_BASE + 0x0020)
#define RW_SFC_DLY_CTL2             (CM4_SFC_BASE + 0x0024)
#define RW_SFC_DLY_CTL3             (CM4_SFC_BASE + 0x0028)
#define RW_SFC_DLY_CTL4             (CM4_SFC_BASE + 0x0030)
#define RW_SFC_DLY_CTL5             (CM4_SFC_BASE + 0x0034)
#define RW_SFC_DLY_CTL6             (CM4_SFC_BASE + 0x0038)
#define RW_SFC_DIRECT_CTL2          (CM4_SFC_BASE + 0x0040)
#define RW_SFC_MISC_CTL3            (CM4_SFC_BASE + 0x0044)
#define RW_SFC_GPRAM_DATA           (CM4_SFC_BASE + 0x0800)

// RW_SF_MAC_CTL
#define SFC_WIP                     (0x00000001)   // b0
#define SFC_WIP_READY               (0x00000002)   // b1
#define SFC_TRIG                    (0x00000004)   // b2
#define SFC_MAC_EN                  (0x00000008)   // b3
#define SFC_MAC_XIO_SEL             (0x00000010)   // b4

#define SFC_SF_IRQ_ACK		    (0x00000100)   //
#define SFC_SF_IRQ_EN               (0x00000200)   //

#define SFC_SF_ABORT_REQ_SRC        (0x00003000)   // b16
#define SFC_SF_ABORT                (0x00004000)   // b16
#define SFC_SF_ABORT_CLR            (0x00008000)   // b16
#define SFC_RELEASE_MAC             (0x00010000)   // b16
#define SFC_MAC_SEL                 (0x10000000)   // b28

// RW_SF_DIRECT_CTL
#define SFC_QPI_EN                  (0x00000001)  // b0
#define SFC_CMD1_EXTADDR_EN         (0x00000002)  // b1
#define SFC_CMD2_EN                 (0x00000004)  // b2
#define SFC_CMD2_EXTADDR_EN         (0x00000008)  // b3
#define SFC_DR_MODE_MASK            (0x00000070)  // b4~b6
#define SFC_DR_CMD2_DUMMY_CYC_MASK  (0x00000F00)  // b8~b11
#define SFC_DR_CMD1_DUMMY_CYC_MASK  (0x0000F000)  // b12~b15
#define SFC_DR_MODE_OFFSET                   (4)
#define SFC_DR_CMD2_DUMMY_CYC_OFFSET         (8)
#define SFC_DR_CMD1_DUMMY_CYC_OFFSET        (12)

#define SFC_DR_CMD2_MASK            (0x00FF0000)  // b16~b23
#define SFC_DR_CMD1_MASK            (0xFF000000)  // b24~b31
#define SFC_DR_CMD2_OFFSET                  (16)
#define SFC_DR_CMD1_OFFSET                  (24)

// RW_SF_DIRECT_CTL: Serial flash direct read mode
#define SFC_DR_SPI_NORMAL_READ_MODE (0x00000000)
#define SFC_DR_SPI_FAST_READ_MODE   (0x00000010)
#define SFC_DR_QPI_FAST_READ_MODE   (0x00000070)

// RW_SF_MISC_CTL
#define SFC_DEL_LATCH_MASK          (0x00000003)  // b0~01
#define SFC_DEL_LATCH_OFFSET        (0)
#define SFC_REQ_IDLE                (0x00000100)  // b8
#define SFC_BOOT_REMAP              (0x00000200)  // b9
#define SFC_SYNC_EN                 (0x00020000)  // b17
#define SFC_NO_RELOAD               (0x00800000)  // b23
#define SFC_CS_EXT_MASK             (0xF0000000)  // b28~b31

// RW_SF_MISC_CTL2
#define SFC_WRAP_ACC_EN             (0x00000001)  // b0
#define SFC_DDR                     (0x00000002)  // b1

// RW_SF_MISC_CTL3
#define SFC_DEL_LATCH_MASK2         (0x00030000)  // b16~017
#define SFC_DEL_LATCH_OFFSET2       (16)
#define SFC_SF2_EN                  (0x00000001)    // b0
#define SFC_SF1_SIZE_MASK           (0x00000070)    // b4~6
#define SFC_SF1_SIZE_OFFSET         (4)             // b4~6
#define SFC_CH1_TRANS_MASK          (0x00000100)    // b8
#define SFC_CH2_TRANS_MASK          (0x00000200)    // b9
#define SFC_CH3_TRANS_MASK          (0x00000400)    // b10
#define SFC_CH1_TRANS_IDLE          (0x00001000)    // b12
#define SFC_CH2_TRANS_IDLE          (0x00002000)    // b13
#define SFC_CH3_TRANS_IDLE          (0x00004000)    // b14

// RW_SF_STA2_CTL
#define SFC_KEEP_READ_SETTING       (0x80000000)  // b31

void sfc_Init(int security_enable);

int sfc_read(const unsigned char *cmd, const unsigned long cmd_len, unsigned char *data, const unsigned long data_len);
int sfc_write(const unsigned char *cmd, const unsigned long cmd_len, const unsigned char *data, const unsigned long data_len);

int sfc_security_write_crc(const unsigned char *cmd, const unsigned long cmd_len);

//int sfc_security_read(const unsigned char *cmd, const unsigned long cmd_len, unsigned char *data, const unsigned long data_len);
//int sfc_security_write(const unsigned char *cmd, const unsigned long cmd_len, unsigned char *data, const unsigned long data_len);

void sfc_Command_List(const unsigned char *cmdlist);
//void sfc_Switch_Controller_Mode(int mode, int dummycyc, unsigned char read_cmd);
void sfc_direct_read_setup(unsigned long reg, int mode);

/* internal use */
void sfc_clear_gpram(void);
int sfc_transfer(const unsigned char *cmd, const unsigned long cmd_len, unsigned char *data, const unsigned long data_len, unsigned int force_qpi);
int sfc_getMaxGPRamSize(void);
int sfc_GetDirectMode(void);
void sfc_set_release_mac(int release_mac);
unsigned long sfc_get_mac_control(void);
void sfc_delay_ms(unsigned int ms);

#endif
#endif

