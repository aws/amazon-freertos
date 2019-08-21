/*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
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

/*! \file flash_layout.h
 * \brief Flash layout details
 *
 * This file presents entire layout of flash including secondary boot-loader,
 * firmware(including redundant partition), file-system and persistent storage
 * manager.
 *
 * The flash layout defined in this file is used in a consistent fashion
 * across all of SDK, namely by the flashprog utility, secondary boot loader
 * (boot2) and Firmware Upgrade code.
 */

#ifndef _FLASH_LAYOUT_H_
#define  _FLASH_LAYOUT_H_

/* MC200 specific flash layout */
#if defined(CONFIG_CPU_MC200)

/** Flash base address */
#define MC200_FLASH_BASE	0x0
/** XFlash base address */
#define MC200_XFLASH_BASE 0x0
/** Flash page size */
#define MC200_FLASH_PAGE_SIZE 0x100	/*!< 256 bytes */
/** Flash sector size */
#define MC200_FLASH_SECTOR_SIZE 0x1000	/*!< 4KB */
/** Flash block size */
#define MC200_FLASH_BLOCK_SIZE 0x10000	/*!< 64KB */
/** Flash chip size */
#define MC200_FLASH_CHIP_SIZE 0x100000	/*!< 1MB */

/** Section: Secondary stage boot-loader with bootrom header
 *  Start:   0x0
 *  Length:  0x1000(4KiB)
 *  Device:  Internal Flash
 */
#define FL_BOOT2_START (MC200_FLASH_BASE + 0x0)
#define FL_BOOTROM_H_SIZE (0x400)
/* Note: Maximum size considering future requirements */
#define FL_BOOT2_BLOCK_SIZE (MC200_FLASH_SECTOR_SIZE)
#define FL_BOOT2_BLOCK_END (FL_BOOT2_START + FL_BOOT2_BLOCK_SIZE)
#define FL_BOOT2_DEV	FL_INT

/** Section: Partition table 1
 *  Start:   0x1000
 *  Length:  0x1000(4KiB)
 *  Device:  Internal Flash
 */
#define FL_PART_SIZE (MC200_FLASH_SECTOR_SIZE)
#define FL_PART1_START FL_BOOT2_BLOCK_END
#define FL_PART1_TABLE_END (FL_PART1_START + FL_PART_SIZE)
#define FL_PART_DEV	FL_INT

/** Section: Partition table 2
 *  Start:   0x2000
 *  Length:  0x1000(4KiB)
 *  Device:  Internal Flash
 */
#define FL_PART2_START FL_PART1_TABLE_END
#define FL_PART2_END (FL_PART2_START + FL_PART_SIZE)

/* MW300 specific flash layout */
#elif defined(CONFIG_CPU_MW300)

/** Flash base address */
#define MW300_FLASH_BASE	0x0
/** Flash sector size */
#define MW300_FLASH_SECTOR_SIZE 0x1000	/*!< 4KB */

/** Section: Secondary stage boot-loader with bootrom header
 *  Start:   0x0
 *  Length:  0x4000(16KiB)
 *  Device:  Internal Flash
 */
#define FL_BOOT2_START (MW300_FLASH_BASE + 0x0)
#define FL_BOOTROM_H_SIZE (0x400)
/* Note: Maximum size considering future requirements */
#define FL_BOOT2_BLOCK_SIZE (MW300_FLASH_SECTOR_SIZE * 4)
#define FL_BOOT2_BLOCK_END (FL_BOOT2_START + FL_BOOT2_BLOCK_SIZE)
#define FL_BOOT2_DEV	FL_INT

/** Section: Partition table 1
 *  Start:   0x4000
 *  Length:  0x1000(4KiB)
 *  Device:  Internal Flash
 */
#define FL_PART_SIZE (MW300_FLASH_SECTOR_SIZE)
#define FL_PART1_START FL_BOOT2_BLOCK_END
#define FL_PART1_TABLE_END (FL_PART1_START + FL_PART_SIZE)
#define FL_PART_DEV	FL_INT

/** Section: Partition table 2
 *  Start:   0x5000
 *  Length:  0x1000(4KiB)
 *  Device:  Internal Flash
 */
#define FL_PART2_START FL_PART1_TABLE_END
#define FL_PART2_END (FL_PART2_START + FL_PART_SIZE)

#endif /* CONFIG_CPU_{MC200/MW300} */
#endif /* !  _FLASH_LAYOUT_H_ */
