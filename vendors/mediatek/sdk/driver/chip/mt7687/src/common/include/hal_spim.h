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

#ifndef __hal_SPI_H__
#define __hal_SPI_H__

#ifdef HAL_SPI_MASTER_MODULE_ENABLED

#include "type_def.h"
#include "mt7687_cm4_hw_memmap.h"
#include "hal_pinmux_define.h"


typedef void (*HALSPIM_ISR)(void);

INT32  halSpim_init(ULONG setting, ULONG clock);

INT32  spim_more_buf_trans_gpio(const UINT32 op_addr, const size_t n_cmd_byte, UINT8 *buf, const size_t buf_cnt, const ULONG flag);
INT32 spim_more_buf_trans_gpio_full_duplex(UINT32 op_addr, const size_t op_byte, const UINT8 *tx_buf, const size_t tx_byte, UINT8 *rx_buf, const size_t rx_byte);

void spim_isr_Register(HALSPIM_ISR spim_isr);

#define spi_reg_inb(offset)     (*(volatile unsigned char *)(offset))
#define spi_reg_inw(offset)     (*(volatile unsigned short *)(offset))
#define spi_reg_inl(offset)     (*(volatile ULONG *)(offset))

#define spi_reg_outb(offset,val)    (*(volatile unsigned char *)(offset) = val)
#define spi_reg_outw(offset,val)    (*(volatile unsigned short *)(offset) = val)
#define spi_reg_outl(offset,val)    (*(volatile ULONG *)(offset) = val)

#define spi_reg_and(addr, value)    spi_reg_outl(addr, (spi_reg_inl(addr) & (value)))
#define spi_reg_or(addr, value)     spi_reg_outl(addr, (spi_reg_inl(addr) | (value)))


#define SPI_REG_CTL         (CM4_SPIMASTER_BASE + 0x00)
#define SPI_REG_OPCODE      (CM4_SPIMASTER_BASE + 0x04)
#define SPI_REG_DATA0       (CM4_SPIMASTER_BASE + 0x08)
#define SPI_REG_DATA(x)     (SPI_REG_DATA0 + ((x) * 4))
#define SPI_REG_MASTER      (CM4_SPIMASTER_BASE + 0x28)
#define SPI_REG_MOREBUF     (CM4_SPIMASTER_BASE + 0x2c)
#define SPI_REG_Q_CTL       (CM4_SPIMASTER_BASE + 0x30)
#define SPI_REG_STATUS      (CM4_SPIMASTER_BASE + 0x34)
#define SPI_REG_CS_POLAR    (CM4_SPIMASTER_BASE + 0x38)

#define SPI_REG_SPACE_CR    (CM4_SPIMASTER_BASE + 0x3c)

#define SPI_PINMUX_REG      (IOT_GPIO_PINMUX_AON_BASE + 0x2c)
#define SPI_PINMUX_REG_0_3_VALUE      ((*(volatile uint32_t *)SPI_PINMUX_REG) & 0x0000000f)
#define SPI_PINMUX_REG_20_23_VALUE    (((*(volatile uint32_t *)SPI_PINMUX_REG) & 0x00f00000) >> 20)
#define SPI_USE_GPIO7_AS_CS           (HAL_GPIO_24_SPI_MOSI_M_CM4 == (SPI_PINMUX_REG_0_3_VALUE & HAL_GPIO_24_SPI_MOSI_M_CM4))
#define SPI_USE_GPIO32_AS_CS          (HAL_GPIO_29_SPI_MOSI_M_CM4 == (SPI_PINMUX_REG_20_23_VALUE & HAL_GPIO_29_SPI_MOSI_M_CM4))
extern hal_gpio_pin_t g_spi_master_cs_n;


#define SPI_CTL_ADDR_SIZE_16BIT     (0x00080000UL)
#define SPI_CTL_ADDR_SIZE_24BIT     (0x00100000UL)
#define SPI_CTL_ADDR_SIZE_32BIT     (0x00200000UL)


#define SPI_CTL_START           0x00000100
#define SPI_CTL_BUSY            0x00010000
#define SPI_CTL_TXCNT_MASK      0x0000000f
#define SPI_CTL_RXCNT_MASK      0x000000f0
#define SPI_CTL_TX_RX_CNT_MASK  0x000000ff
#define SPI_CTL_SIZE_MASK       0x00180000
#define SPI_CTL_ADDREXT_MASK    0xff000000

//0x24000028
#define SPI_MASTER_SERIAL_MODE_STANDARD     (0x0UL << 0)
#define SPI_MASTER_SERIAL_MODE_DUAL         (0x1UL << 0)
#define SPI_MASTER_SERIAL_MODE_QUAD         (0x2UL << 0)

#define SPI_MASTER_MB_MODE_DISABLE          (0x0UL << 2)
#define SPI_MASTER_MB_MODE_ENABLE           (0x1UL << 2)
#define SPI_MASTER_CLOCK_DIV_SHIFT          (16)

#define SPI_MASTER_BIDIR_MODE_DISABLE       (0x0UL << 6)
#define SPI_MASTER_BIDIR_MODE_ENABLE        (0x1UL << 6)

#define HALF_DUPLEX              (0x0UL << 10)
#define FULL_DUPLEX              (0x1UL << 10)
#define DUPLEX_MASK              (0x1UL << 10)


#define SPI_MBCTL_TXCNT_MASK        0x000001ff
#define SPI_MBCTL_RXCNT_MASK        0x001ff000
#define SPI_MBCTL_TX_RX_CNT_MASK    (SPI_MBCTL_TXCNT_MASK | SPI_MBCTL_RXCNT_MASK)
#define SPI_MBCTL_CMD_MASK          0x3f000000

#define SPI_CTL_CLK_SEL_MASK    0x03000000
#define SPI_OPCODE_MASK         0x000000ff

/*
 * User api flag
 */
#define SPI_READ_FLAG       0x1
#define SPI_WRITE_FLAG      0x2

/*
 * SPI clock
 *Div_Value = 0, SPI clock/2
 *Div_Value = 1, SPI clock/3
 *....
 *Div_Value = 4095, SPI clock/4097
 */

#define SPI_4M_DIV_VALUE    (30 -2)
#define SPI_6M_DIV_VALUE    (20 -2)
#define SPI_8M_DIV_VALUE    (15 -2)
#define SPI_10M_DIV_VALUE    (12 -2)
#define SPI_12M_DIV_VALUE    (10 - 2)

#endif /* HAL_SPI_MASTER_MODULE_ENABLED */

#endif
