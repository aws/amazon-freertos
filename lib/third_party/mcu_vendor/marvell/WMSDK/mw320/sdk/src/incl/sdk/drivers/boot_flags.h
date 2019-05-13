/** @file boot_flags.h
*
*  @brief Boot Flags
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

#ifndef __boot_flags_h
#define __boot_flags_h

#include <wmtypes.h>
#include <lowlevel_drivers.h>

/* BooROM version for A0 chip */
#define BOOTROM_A0_VERSION 3

/*
 * Location where the boot flags are stored by the bootloader.
 * Currently this is stored in NVRAM area after BootROM occupied range.
 */
#define BOOT_FLAGS	((volatile uint32_t *)0x480C0024)

/* final location for those boot flags */
extern uint32_t g_boot_flags;
extern uint32_t g_prev_version_fw;
extern uint32_t g_rst_cause;

/* boot flag definitions */
/* partition number used to boot */
#define BOOT_PARTITION_MASK		0x03
/* boot partition table */
#define BOOT_PARTITION_TABLE_MASK	0x100
  /* partition table1 state */
#define PARTITION_TABLE1_STATE_MASK	0x200
  /* partition table2 state */
#define PARTITION_TABLE2_STATE_MASK	0x400
/* main firmware had errors */
#define BOOT_MAIN_FIRMWARE_BAD_CRC     0x20

/* secure boot config flags for firmware */
#define BOOT_SECURE_FW_ENABLE		0x10000
#define BOOT_SECURE_SIGN_FW		0x20000
#define BOOT_SECURE_ENCRYPT_FW		0x40000

/*
 * Bootrom stores some information in initial few bytes of retention ram.
 * Following is the format used for storing this info.
 */

#if defined(CONFIG_CPU_MC200)
struct bootrom_info {
	uint8_t powermode;
	uint8_t pinstrap;
	uint8_t bootsource;
	uint8_t bootflag;
	uint32_t reserved;
	uint32_t pm3_entryaddr;
	uint32_t main_enteryaddr;
	uint32_t crystal_freq;
	uint32_t system_clk;
	uint32_t version;
	uint32_t reserved1;
};

#endif /* CONFIG_CPU_MC200 */

#if defined(CONFIG_CPU_MW300)
struct bootrom_info {
	uint32_t pm3_entryaddr;
	uint32_t bootMode;
	uint32_t powerMode;
};
#endif  /* CONFIG_CPU_MW300 */

extern unsigned long _nvram_start;

static inline int boot_get_partition_no()
{
	return g_boot_flags & BOOT_PARTITION_MASK;
}

static inline int boot_old_fw_version()
{
	return g_prev_version_fw;
}

static inline uint32_t boot_store_reset_cause()
{
	uint32_t rst_cause =  PMU_GetLastResetCause();
	PMU_ClrLastResetCause(PMU_RESETSRC_ALL);
	return rst_cause;
}

static inline uint32_t boot_reset_cause()
{
	return g_rst_cause;
}

void boot_init();
void boot_report_flags();

#endif
