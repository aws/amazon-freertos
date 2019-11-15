/**
 * \brief Component description for CHIPID
 *
 * Copyright (c) 2019 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip software and any derivatives
 * exclusively with Microchip products. It is your responsibility to comply with third party license
 * terms applicable to your use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY,
 * APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT
 * EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/* file generated from device description version 2019-07-24T15:04:36Z */
#ifndef _SAME70_CHIPID_COMPONENT_H_
#define _SAME70_CHIPID_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR CHIPID                                       */
/* ************************************************************************** */

/* -------- CHIPID_CIDR : (CHIPID Offset: 0x00) ( R/ 32) Chip ID Register -------- */
#define CHIPID_CIDR_VERSION_Pos               _U_(0)                                               /**< (CHIPID_CIDR) Version of the Device Position */
#define CHIPID_CIDR_VERSION_Msk               (_U_(0x1F) << CHIPID_CIDR_VERSION_Pos)               /**< (CHIPID_CIDR) Version of the Device Mask */
#define CHIPID_CIDR_VERSION(value)            (CHIPID_CIDR_VERSION_Msk & ((value) << CHIPID_CIDR_VERSION_Pos))
#define CHIPID_CIDR_EPROC_Pos                 _U_(5)                                               /**< (CHIPID_CIDR) Embedded Processor Position */
#define CHIPID_CIDR_EPROC_Msk                 (_U_(0x7) << CHIPID_CIDR_EPROC_Pos)                  /**< (CHIPID_CIDR) Embedded Processor Mask */
#define CHIPID_CIDR_EPROC(value)              (CHIPID_CIDR_EPROC_Msk & ((value) << CHIPID_CIDR_EPROC_Pos))
#define   CHIPID_CIDR_EPROC_SAMx7_Val         _U_(0x0)                                             /**< (CHIPID_CIDR) Cortex-M7  */
#define   CHIPID_CIDR_EPROC_ARM946ES_Val      _U_(0x1)                                             /**< (CHIPID_CIDR) ARM946ES  */
#define   CHIPID_CIDR_EPROC_ARM7TDMI_Val      _U_(0x2)                                             /**< (CHIPID_CIDR) ARM7TDMI  */
#define   CHIPID_CIDR_EPROC_CM3_Val           _U_(0x3)                                             /**< (CHIPID_CIDR) Cortex-M3  */
#define   CHIPID_CIDR_EPROC_ARM920T_Val       _U_(0x4)                                             /**< (CHIPID_CIDR) ARM920T  */
#define   CHIPID_CIDR_EPROC_ARM926EJS_Val     _U_(0x5)                                             /**< (CHIPID_CIDR) ARM926EJS  */
#define   CHIPID_CIDR_EPROC_CA5_Val           _U_(0x6)                                             /**< (CHIPID_CIDR) Cortex-A5  */
#define   CHIPID_CIDR_EPROC_CM4_Val           _U_(0x7)                                             /**< (CHIPID_CIDR) Cortex-M4  */
#define CHIPID_CIDR_EPROC_SAMx7               (CHIPID_CIDR_EPROC_SAMx7_Val << CHIPID_CIDR_EPROC_Pos) /**< (CHIPID_CIDR) Cortex-M7 Position  */
#define CHIPID_CIDR_EPROC_ARM946ES            (CHIPID_CIDR_EPROC_ARM946ES_Val << CHIPID_CIDR_EPROC_Pos) /**< (CHIPID_CIDR) ARM946ES Position  */
#define CHIPID_CIDR_EPROC_ARM7TDMI            (CHIPID_CIDR_EPROC_ARM7TDMI_Val << CHIPID_CIDR_EPROC_Pos) /**< (CHIPID_CIDR) ARM7TDMI Position  */
#define CHIPID_CIDR_EPROC_CM3                 (CHIPID_CIDR_EPROC_CM3_Val << CHIPID_CIDR_EPROC_Pos) /**< (CHIPID_CIDR) Cortex-M3 Position  */
#define CHIPID_CIDR_EPROC_ARM920T             (CHIPID_CIDR_EPROC_ARM920T_Val << CHIPID_CIDR_EPROC_Pos) /**< (CHIPID_CIDR) ARM920T Position  */
#define CHIPID_CIDR_EPROC_ARM926EJS           (CHIPID_CIDR_EPROC_ARM926EJS_Val << CHIPID_CIDR_EPROC_Pos) /**< (CHIPID_CIDR) ARM926EJS Position  */
#define CHIPID_CIDR_EPROC_CA5                 (CHIPID_CIDR_EPROC_CA5_Val << CHIPID_CIDR_EPROC_Pos) /**< (CHIPID_CIDR) Cortex-A5 Position  */
#define CHIPID_CIDR_EPROC_CM4                 (CHIPID_CIDR_EPROC_CM4_Val << CHIPID_CIDR_EPROC_Pos) /**< (CHIPID_CIDR) Cortex-M4 Position  */
#define CHIPID_CIDR_NVPSIZ_Pos                _U_(8)                                               /**< (CHIPID_CIDR) Nonvolatile Program Memory Size Position */
#define CHIPID_CIDR_NVPSIZ_Msk                (_U_(0xF) << CHIPID_CIDR_NVPSIZ_Pos)                 /**< (CHIPID_CIDR) Nonvolatile Program Memory Size Mask */
#define CHIPID_CIDR_NVPSIZ(value)             (CHIPID_CIDR_NVPSIZ_Msk & ((value) << CHIPID_CIDR_NVPSIZ_Pos))
#define   CHIPID_CIDR_NVPSIZ_NONE_Val         _U_(0x0)                                             /**< (CHIPID_CIDR) None  */
#define   CHIPID_CIDR_NVPSIZ_8K_Val           _U_(0x1)                                             /**< (CHIPID_CIDR) 8 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_16K_Val          _U_(0x2)                                             /**< (CHIPID_CIDR) 16 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_32K_Val          _U_(0x3)                                             /**< (CHIPID_CIDR) 32 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_64K_Val          _U_(0x5)                                             /**< (CHIPID_CIDR) 64 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_128K_Val         _U_(0x7)                                             /**< (CHIPID_CIDR) 128 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_160K_Val         _U_(0x8)                                             /**< (CHIPID_CIDR) 160 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_256K_Val         _U_(0x9)                                             /**< (CHIPID_CIDR) 256 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_512K_Val         _U_(0xA)                                             /**< (CHIPID_CIDR) 512 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_1024K_Val        _U_(0xC)                                             /**< (CHIPID_CIDR) 1024 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_2048K_Val        _U_(0xE)                                             /**< (CHIPID_CIDR) 2048 Kbytes  */
#define CHIPID_CIDR_NVPSIZ_NONE               (CHIPID_CIDR_NVPSIZ_NONE_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) None Position  */
#define CHIPID_CIDR_NVPSIZ_8K                 (CHIPID_CIDR_NVPSIZ_8K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 8 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_16K                (CHIPID_CIDR_NVPSIZ_16K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 16 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_32K                (CHIPID_CIDR_NVPSIZ_32K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 32 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_64K                (CHIPID_CIDR_NVPSIZ_64K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 64 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_128K               (CHIPID_CIDR_NVPSIZ_128K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 128 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_160K               (CHIPID_CIDR_NVPSIZ_160K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 160 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_256K               (CHIPID_CIDR_NVPSIZ_256K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 256 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_512K               (CHIPID_CIDR_NVPSIZ_512K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 512 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_1024K              (CHIPID_CIDR_NVPSIZ_1024K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 1024 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_2048K              (CHIPID_CIDR_NVPSIZ_2048K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 2048 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_Pos               _U_(12)                                              /**< (CHIPID_CIDR) Second Nonvolatile Program Memory Size Position */
#define CHIPID_CIDR_NVPSIZ2_Msk               (_U_(0xF) << CHIPID_CIDR_NVPSIZ2_Pos)                /**< (CHIPID_CIDR) Second Nonvolatile Program Memory Size Mask */
#define CHIPID_CIDR_NVPSIZ2(value)            (CHIPID_CIDR_NVPSIZ2_Msk & ((value) << CHIPID_CIDR_NVPSIZ2_Pos))
#define   CHIPID_CIDR_NVPSIZ2_NONE_Val        _U_(0x0)                                             /**< (CHIPID_CIDR) None  */
#define   CHIPID_CIDR_NVPSIZ2_8K_Val          _U_(0x1)                                             /**< (CHIPID_CIDR) 8 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ2_16K_Val         _U_(0x2)                                             /**< (CHIPID_CIDR) 16 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ2_32K_Val         _U_(0x3)                                             /**< (CHIPID_CIDR) 32 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ2_64K_Val         _U_(0x5)                                             /**< (CHIPID_CIDR) 64 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ2_128K_Val        _U_(0x7)                                             /**< (CHIPID_CIDR) 128 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ2_256K_Val        _U_(0x9)                                             /**< (CHIPID_CIDR) 256 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ2_512K_Val        _U_(0xA)                                             /**< (CHIPID_CIDR) 512 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ2_1024K_Val       _U_(0xC)                                             /**< (CHIPID_CIDR) 1024 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ2_2048K_Val       _U_(0xE)                                             /**< (CHIPID_CIDR) 2048 Kbytes  */
#define CHIPID_CIDR_NVPSIZ2_NONE              (CHIPID_CIDR_NVPSIZ2_NONE_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) None Position  */
#define CHIPID_CIDR_NVPSIZ2_8K                (CHIPID_CIDR_NVPSIZ2_8K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 8 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_16K               (CHIPID_CIDR_NVPSIZ2_16K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 16 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_32K               (CHIPID_CIDR_NVPSIZ2_32K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 32 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_64K               (CHIPID_CIDR_NVPSIZ2_64K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 64 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_128K              (CHIPID_CIDR_NVPSIZ2_128K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 128 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_256K              (CHIPID_CIDR_NVPSIZ2_256K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 256 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_512K              (CHIPID_CIDR_NVPSIZ2_512K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 512 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_1024K             (CHIPID_CIDR_NVPSIZ2_1024K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 1024 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_2048K             (CHIPID_CIDR_NVPSIZ2_2048K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 2048 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_Pos               _U_(16)                                              /**< (CHIPID_CIDR) Internal SRAM Size Position */
#define CHIPID_CIDR_SRAMSIZ_Msk               (_U_(0xF) << CHIPID_CIDR_SRAMSIZ_Pos)                /**< (CHIPID_CIDR) Internal SRAM Size Mask */
#define CHIPID_CIDR_SRAMSIZ(value)            (CHIPID_CIDR_SRAMSIZ_Msk & ((value) << CHIPID_CIDR_SRAMSIZ_Pos))
#define   CHIPID_CIDR_SRAMSIZ_48K_Val         _U_(0x0)                                             /**< (CHIPID_CIDR) 48 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_192K_Val        _U_(0x1)                                             /**< (CHIPID_CIDR) 192 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_384K_Val        _U_(0x2)                                             /**< (CHIPID_CIDR) 384 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_6K_Val          _U_(0x3)                                             /**< (CHIPID_CIDR) 6 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_24K_Val         _U_(0x4)                                             /**< (CHIPID_CIDR) 24 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_4K_Val          _U_(0x5)                                             /**< (CHIPID_CIDR) 4 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_80K_Val         _U_(0x6)                                             /**< (CHIPID_CIDR) 80 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_160K_Val        _U_(0x7)                                             /**< (CHIPID_CIDR) 160 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_8K_Val          _U_(0x8)                                             /**< (CHIPID_CIDR) 8 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_16K_Val         _U_(0x9)                                             /**< (CHIPID_CIDR) 16 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_32K_Val         _U_(0xA)                                             /**< (CHIPID_CIDR) 32 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_64K_Val         _U_(0xB)                                             /**< (CHIPID_CIDR) 64 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_128K_Val        _U_(0xC)                                             /**< (CHIPID_CIDR) 128 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_256K_Val        _U_(0xD)                                             /**< (CHIPID_CIDR) 256 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_96K_Val         _U_(0xE)                                             /**< (CHIPID_CIDR) 96 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_512K_Val        _U_(0xF)                                             /**< (CHIPID_CIDR) 512 Kbytes  */
#define CHIPID_CIDR_SRAMSIZ_48K               (CHIPID_CIDR_SRAMSIZ_48K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 48 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_192K              (CHIPID_CIDR_SRAMSIZ_192K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 192 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_384K              (CHIPID_CIDR_SRAMSIZ_384K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 384 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_6K                (CHIPID_CIDR_SRAMSIZ_6K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 6 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_24K               (CHIPID_CIDR_SRAMSIZ_24K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 24 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_4K                (CHIPID_CIDR_SRAMSIZ_4K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 4 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_80K               (CHIPID_CIDR_SRAMSIZ_80K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 80 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_160K              (CHIPID_CIDR_SRAMSIZ_160K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 160 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_8K                (CHIPID_CIDR_SRAMSIZ_8K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 8 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_16K               (CHIPID_CIDR_SRAMSIZ_16K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 16 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_32K               (CHIPID_CIDR_SRAMSIZ_32K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 32 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_64K               (CHIPID_CIDR_SRAMSIZ_64K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 64 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_128K              (CHIPID_CIDR_SRAMSIZ_128K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 128 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_256K              (CHIPID_CIDR_SRAMSIZ_256K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 256 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_96K               (CHIPID_CIDR_SRAMSIZ_96K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 96 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_512K              (CHIPID_CIDR_SRAMSIZ_512K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 512 Kbytes Position  */
#define CHIPID_CIDR_ARCH_Pos                  _U_(20)                                              /**< (CHIPID_CIDR) Architecture Identifier Position */
#define CHIPID_CIDR_ARCH_Msk                  (_U_(0xFF) << CHIPID_CIDR_ARCH_Pos)                  /**< (CHIPID_CIDR) Architecture Identifier Mask */
#define CHIPID_CIDR_ARCH(value)               (CHIPID_CIDR_ARCH_Msk & ((value) << CHIPID_CIDR_ARCH_Pos))
#define   CHIPID_CIDR_ARCH_SAME70_Val         _U_(0x10)                                            /**< (CHIPID_CIDR) SAM E70  */
#define   CHIPID_CIDR_ARCH_SAMS70_Val         _U_(0x11)                                            /**< (CHIPID_CIDR) SAM S70  */
#define   CHIPID_CIDR_ARCH_SAMV71_Val         _U_(0x12)                                            /**< (CHIPID_CIDR) SAM V71  */
#define   CHIPID_CIDR_ARCH_SAMV70_Val         _U_(0x13)                                            /**< (CHIPID_CIDR) SAM V70  */
#define CHIPID_CIDR_ARCH_SAME70               (CHIPID_CIDR_ARCH_SAME70_Val << CHIPID_CIDR_ARCH_Pos) /**< (CHIPID_CIDR) SAM E70 Position  */
#define CHIPID_CIDR_ARCH_SAMS70               (CHIPID_CIDR_ARCH_SAMS70_Val << CHIPID_CIDR_ARCH_Pos) /**< (CHIPID_CIDR) SAM S70 Position  */
#define CHIPID_CIDR_ARCH_SAMV71               (CHIPID_CIDR_ARCH_SAMV71_Val << CHIPID_CIDR_ARCH_Pos) /**< (CHIPID_CIDR) SAM V71 Position  */
#define CHIPID_CIDR_ARCH_SAMV70               (CHIPID_CIDR_ARCH_SAMV70_Val << CHIPID_CIDR_ARCH_Pos) /**< (CHIPID_CIDR) SAM V70 Position  */
#define CHIPID_CIDR_NVPTYP_Pos                _U_(28)                                              /**< (CHIPID_CIDR) Nonvolatile Program Memory Type Position */
#define CHIPID_CIDR_NVPTYP_Msk                (_U_(0x7) << CHIPID_CIDR_NVPTYP_Pos)                 /**< (CHIPID_CIDR) Nonvolatile Program Memory Type Mask */
#define CHIPID_CIDR_NVPTYP(value)             (CHIPID_CIDR_NVPTYP_Msk & ((value) << CHIPID_CIDR_NVPTYP_Pos))
#define   CHIPID_CIDR_NVPTYP_ROM_Val          _U_(0x0)                                             /**< (CHIPID_CIDR) ROM  */
#define   CHIPID_CIDR_NVPTYP_ROMLESS_Val      _U_(0x1)                                             /**< (CHIPID_CIDR) ROMless or on-chip Flash  */
#define   CHIPID_CIDR_NVPTYP_FLASH_Val        _U_(0x2)                                             /**< (CHIPID_CIDR) Embedded Flash Memory  */
#define   CHIPID_CIDR_NVPTYP_ROM_FLASH_Val    _U_(0x3)                                             /**< (CHIPID_CIDR) ROM and Embedded Flash Memory- NVPSIZ is ROM size- NVPSIZ2 is Flash size  */
#define   CHIPID_CIDR_NVPTYP_SRAM_Val         _U_(0x4)                                             /**< (CHIPID_CIDR) SRAM emulating ROM  */
#define CHIPID_CIDR_NVPTYP_ROM                (CHIPID_CIDR_NVPTYP_ROM_Val << CHIPID_CIDR_NVPTYP_Pos) /**< (CHIPID_CIDR) ROM Position  */
#define CHIPID_CIDR_NVPTYP_ROMLESS            (CHIPID_CIDR_NVPTYP_ROMLESS_Val << CHIPID_CIDR_NVPTYP_Pos) /**< (CHIPID_CIDR) ROMless or on-chip Flash Position  */
#define CHIPID_CIDR_NVPTYP_FLASH              (CHIPID_CIDR_NVPTYP_FLASH_Val << CHIPID_CIDR_NVPTYP_Pos) /**< (CHIPID_CIDR) Embedded Flash Memory Position  */
#define CHIPID_CIDR_NVPTYP_ROM_FLASH          (CHIPID_CIDR_NVPTYP_ROM_FLASH_Val << CHIPID_CIDR_NVPTYP_Pos) /**< (CHIPID_CIDR) ROM and Embedded Flash Memory- NVPSIZ is ROM size- NVPSIZ2 is Flash size Position  */
#define CHIPID_CIDR_NVPTYP_SRAM               (CHIPID_CIDR_NVPTYP_SRAM_Val << CHIPID_CIDR_NVPTYP_Pos) /**< (CHIPID_CIDR) SRAM emulating ROM Position  */
#define CHIPID_CIDR_EXT_Pos                   _U_(31)                                              /**< (CHIPID_CIDR) Extension Flag Position */
#define CHIPID_CIDR_EXT_Msk                   (_U_(0x1) << CHIPID_CIDR_EXT_Pos)                    /**< (CHIPID_CIDR) Extension Flag Mask */
#define CHIPID_CIDR_EXT(value)                (CHIPID_CIDR_EXT_Msk & ((value) << CHIPID_CIDR_EXT_Pos))
#define CHIPID_CIDR_Msk                       _U_(0xFFFFFFFF)                                      /**< (CHIPID_CIDR) Register Mask  */


/* -------- CHIPID_EXID : (CHIPID Offset: 0x04) ( R/ 32) Chip ID Extension Register -------- */
#define CHIPID_EXID_EXID_Pos                  _U_(0)                                               /**< (CHIPID_EXID) Chip ID Extension Position */
#define CHIPID_EXID_EXID_Msk                  (_U_(0xFFFFFFFF) << CHIPID_EXID_EXID_Pos)            /**< (CHIPID_EXID) Chip ID Extension Mask */
#define CHIPID_EXID_EXID(value)               (CHIPID_EXID_EXID_Msk & ((value) << CHIPID_EXID_EXID_Pos))
#define CHIPID_EXID_Msk                       _U_(0xFFFFFFFF)                                      /**< (CHIPID_EXID) Register Mask  */


/** \brief CHIPID register offsets definitions */
#define CHIPID_CIDR_REG_OFST           (0x00)              /**< (CHIPID_CIDR) Chip ID Register Offset */
#define CHIPID_EXID_REG_OFST           (0x04)              /**< (CHIPID_EXID) Chip ID Extension Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief CHIPID register API structure */
typedef struct
{
  __I   uint32_t                       CHIPID_CIDR;        /**< Offset: 0x00 (R/   32) Chip ID Register */
  __I   uint32_t                       CHIPID_EXID;        /**< Offset: 0x04 (R/   32) Chip ID Extension Register */
} chipid_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_CHIPID_COMPONENT_H_ */
