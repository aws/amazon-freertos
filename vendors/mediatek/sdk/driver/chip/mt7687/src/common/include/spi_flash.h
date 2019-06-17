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

#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__


#include "sfc.h"


typedef enum {
    SF_UNDEF = 0
    , SPI      = 1
    , SPIQ     = 2
    , QPI      = 3
} SF_MODE_Enum;

typedef enum {
    SF_DRV_NOT_READY = -1,
    SF_DRV_READY = 0,
    SF_DRV_READING = 1,
    SF_DRV_DIRECT_READING = 2,
    SF_DRV_PROGRAMMING = 3,
    SF_DRV_ERASE_WRITE = 4,
    SF_DRV_ERASE_WRITE_CRC = 5,
    SF_DRV_SECTOR_4K_ERASING = 6,
    SF_DRV_SECTOR_32K_ERASING = 7,
    SF_DRV_SECTOR_64K_ERASING = 8,
    SF_DRV_CHIP_ERASING = 9
} SF_DRV_STATE;

typedef enum {
    FLASH_NOT_READY = -1,
    FLASH_READY = 0,
    FLASH_PROGRAMMING = 1,
    FLASH_ERASING = 2,
} FLASH_DRV_STATE;

typedef enum {
    SF_TYPE_NONE       = 0,
    SF_TYPE_MXIC       = 0xC2,
    SF_TYPE_WINBOND    = 0xEF,
    SF_TYPE_GD         = 0xC8,
    SF_TYPE_MICRON     = 0x20,
    SF_TYPE_CUSTOMER   = 0xFF,
    SF_TYPE_COUNT      = 0xFFFF
} SF_TYPT;


#define FLASH_ERASE_4K		    (4096)
#define FLASH_ERASE_32K     (32768)
#define FLASH_ERASE_64K		(65536)
#define FLASH_STATUS_BUSY       (0x01)

/* Status Register bits. */
#define SR_WIP                  1       /* Write in progress */
#define SR_WEL                  2       /* Write enable latch */
#define SR_BP0                  4       /* Block protect 0 */
#define SR_BP1                  8       /* Block protect 1 */
#define SR_BP2                  0x10    /* Block protect 2 */
#define SR_EPE                  0x20    /* Erase/Program error */
#define SR_SRWD                 0x80    /* SR write protect */



/* Flash opcodes. */

#define OPCODE_WRSR			1       /* Write status register */
#define OPCODE_PP			2       /* Page program */
#define OPCODE_READ			3       /* Read data bytes */
#define OPCODE_WRDI			4       /* Write disable */
#define OPCODE_RDSR			5       /* Read status register */
#define OPCODE_WREN			6       /* Write enable */
#define OPCODE_FAST_READ		0xB     /* Read data bytes */
#define OPCODE_PE			    0x20    /* Page erase -- 4K */
#define OPCODE_RDSR_2			0x35    /* Read status register */		/* Winbond */
#define OPCODE_CE			    0x60	/* Chip Erase */
#define OPCODE_ENABLE_RESET		0x66    /* Enable Reset flash */		/* Winbond */
#define OPCODE_RD_MANU_ID		0x90    /* Read Manufacturer/Device ID */
#define OPCODE_RESET			0x99    /* Reset flash */ 			/* Winbond */
#define OPCODE_RD_JEDEC_ID		0x9F    /* Read JEDEC ID */
#define OPCODE_RES			    0xAB    /* Read Electronic Signature */
#define OPCODE_SE			    0xD8    /* Sector erase -- 64K */
#define OPCODE_SE_32K           0x52    /* Sector erase -- 32K */
#define OPCODE_READ_SPIQ		0xEB    /* Fast Read -- SPIQ */			/* Winbond */
//#define OPCODE_READ_QPI			0x00    /* Fast Read -- QPI */			/* Winbond */
#define OPCODE_RESET_QPI		0xFF	/* Reset flash */		/* SST */
#define OPCODE_RD_SECST   0x2B /*Read MXIC secure status */

#define SFC_MISC1_DEL_LATCH_LATENCY_ADDR              (0x83070008)
#define SFC_MISC1_DEL_LATCH_LATENCY_MASK              BITS(0,1)
#define SFC_MISC1_DEL_LATCH_LATENCY_SHFT              0

#define SFC_MISC1_SMPCK_INV_ADDR                       (0x83070008)
#define SFC_MISC1_SMPCK_INV_MASK                       BIT(4)
#define SFC_MISC1_SMPCK_INV_SHFT                       4

#define SFC_DELAY_CONTROL3_SFCK_SAM_DLY_ADDR          (0x83070028)
#define SFC_DELAY_CONTROL3_SFCK_SAM_DLY_MASK          BITS(0,5)
#define SFC_DELAY_CONTROL3_SFCK_SAM_DLY_SHFT          0


struct chip_info {
    char		*name;
    unsigned char	id;
    unsigned long	jedec_id;
    unsigned long	page_size;
    unsigned int	n_pages;

    unsigned char	quad_program_cmd;
    unsigned char	quad_read_cmd;
    unsigned char	qpi_read_cmd;

};

void flash_config(void);
void flash_init(int security_enable);

int flash_read_jedec_id(unsigned char *buf, int buf_size);
int flash_read_manu_id(unsigned char *buf, int buf_size);
int flash_read_manu_id_quad(unsigned char *buf, int buf_size);

int flash_read_sr(unsigned char *val);
int flash_read_sr2(unsigned char *val);

int flash_read(unsigned char *buf, unsigned int from, int len);
int flash_fast_read(unsigned char *buf, unsigned int from, int len, int dummy_cycle);

int flash_erase_sector(unsigned int offs);
int flash_erase_sector_32k(unsigned int offs);
int flash_erase_page(unsigned int offs);
int flash_erase(unsigned int offs, int len);

int flash_write(const unsigned char *buf, unsigned int to, int len);

int flash_erase_write(const unsigned char *buf, unsigned int offs, int count);
int flash_erase_chip(void);

int flash_protect(void);
int flash_unprotect(void);
int flash_write_enable(void);
int flash_write_disable(void);

int flash_wait_ready(int sleep_ms);
void flash_switch_mode(unsigned long mode);

//int flash_security_write(unsigned char *buf, unsigned int to, int len);
int flash_erase_write_security_crc(unsigned int to);


/* Winbond specific routines */
void flash_suspend_Winbond(void);
void flash_resume_Winbond(void);
FLASH_DRV_STATE query_spi_flash_status(void);
int flash_check_device(void);

#if defined(MTK_FLASH_POWER_SUPPLY_LOCK_DOWN)
int flash_software_lockdown_from_top(uint32_t size);
#endif
#endif
#endif

