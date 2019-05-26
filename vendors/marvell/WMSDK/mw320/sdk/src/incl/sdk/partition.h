/*
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

/*! \file partition.h
 * \brief Partition Management
 *
 * This defines functions for managing the flash partitions.
 *
 * \section part_usage Usage
 *
 * Applications can define and use their own flash layout using the flashprog
 * tool. Each partition in the layout has a name, type, start address and a
 * size associated with it.
 * Applications can use the part_get_layout_by_name() or part_get_layout_by_id()
 * functions to retrieve pointers to the partition entry. This partition entry
 * can be converted to a flash descriptor using the part_to_flash_desc()
 * function. All components that use the flash accept the flash descriptor to
 * recognize what portion of the flash they use.
 *
 * Additionally, the partition entries store information about active-passive
 * upgrades. Applications can define their own components in the flash in an
 * active-passive manner. Applications can then retrieve the active component
 * using the part_get_active_partition() API. If after an upgrade happens the
 * partitions need to be toggled then the part_get_active_partition() API is
 * used.
 *
 *
 */

#ifndef _PARTITION_H_
#define _PARTITION_H_

#include <wmtypes.h>
#include <wmerrno.h>
#include <flash.h>
#include <flash_layout.h>
#include <wmlog.h>

#define prt_e(...)				\
	wmlog_e("partition", ##__VA_ARGS__)
#define prt_w(...)				\
	wmlog_w("partition", ##__VA_ARGS__)
#define prt_d(...)				\
	wmlog("partition", ##__VA_ARGS__)


#define MAX_FL_COMP CONFIG_FLASH_PARTITION_COUNT
#define MAX_NAME    8

/** Partition Table */
struct partition_table {
#define PARTITION_TABLE_MAGIC (('W' << 0)|('M' << 8)|('P' << 16)|('T' << 24))
	/** The magic identifying the start of the partition table */
	uint32_t magic;
#define PARTITION_TABLE_VERSION 1
	/** The version number of this partition table */
	uint16_t version;
	/** The number of partition entries that follow this */
	uint16_t partition_entries_no;
	/** Generation level */
	uint32_t gen_level;
	/** The CRC of all the above components */
	uint32_t crc;
};

/** Partition Entry */
struct partition_entry {
	/** The type of the flash component */
	uint8_t type;
	/** The device id, internal flash is always id 0 */
	uint8_t device;
	/** A descriptive component name */
	char name[MAX_NAME];
	/** Start address on the given device */
	uint32_t start;
	/** Size on the given device */
	uint32_t size;
	/** Generation level */
	uint32_t gen_level;
};

/** The various components in a flash layout */
enum flash_comp {
	/** The secondary stage boot loader to assist firmware bootup */
	FC_COMP_BOOT2 = 0,
	/** The firmware image. There can be a maximum of two firmware
	 * components available in a flash layout. These will be used in an
	 * active-passive mode if rfget module is enabled.
	 */
	FC_COMP_FW,
	/** The wlan firmware image. There can be one wlan firmware image in the
	 * system. The contents of this location would be downloaded to the WLAN
	 * chip.
	 */
	FC_COMP_WLAN_FW,
	/** The FTFS image. */
	FC_COMP_FTFS,
	/** The PSM data */
	FC_COMP_PSM,
	/** Application Specific component */
	FC_COMP_USER_APP,
	/** The BT firmware image if external BT/BLE chip is used */
	FC_COMP_BT_FW,
};

/** Initialize the partitioning module
 *
 * \return WM_SUCCESS on success or error code
 */
int part_init(void);

/** Convert from partition to flash_descriptor
 *
 * All the components that own a portion of flash in the SDK use the flash
 * descriptor function to identify this portion. This function converts a
 * partition_entry into a corresponding flash descriptor that can be passed to
 * these modules.
 *
 * \param[in] p a pointer to a partition entry
 * \param[out] f a pointer to a flash descriptor
 */
void part_to_flash_desc(struct partition_entry *p,
			flash_desc_t *f);

/** Get partition entry by ID
 *
 * Retrieve the pointer to a partition entry using an ID. This function can be
 * called to repetitively to retrieve more entries of the same type. The value
 * of start_index is modified by the function so that search starts from this
 * index in the next call.
 *
 * \param [in] comp the flash component to search for
 * \param [in,out] start_index the start index for the search. The first call to
 * this function should always have a start index of 0.
 *
 * \return a pointer to a partition entry on success, null otherwise.
 */
struct partition_entry *part_get_layout_by_id(enum flash_comp comp,
					      short *start_index);

/** Get partition entry by name
 *
 * Retrieve the pointer to a partition entry using a name. This function can be
 * called to repetitively to retrieve more entries of the same type. The value
 * of start_index is modified by the function so that search starts from this
 * index in the next call.
 *
 * \param [in] name the flash component's name as mentioned in the layout.
 * \param [in,out] start_index the start index for the search. The first call to
 * this function should always have a start index of 0.
 *
 * \return a pointer to a partition entry on success, null otherwise.
 */
struct partition_entry *part_get_layout_by_name(const char *name,
						short *start_index);

struct partition_entry *part_get_active_partition_by_name(const char *name);
struct partition_entry *part_get_passive_partition_by_name(const char *name);

/** Find the active partition
 *
 * Given two partition entries that are upgrade buddies (that is, have the same
 * friendly name), this function returns the partition that is currently marked
 * as active.
 * \param[in] p1 Pointer to a partition entry
 * \param[in] p2 Pointer to the upgrade buddy for the first partition entry
 *
 * \return a pointer to the active partition entry on success, null otherwise.
 */
struct partition_entry *part_get_active_partition(struct partition_entry *p1,
                                                  struct partition_entry *p2);

/** Set active partition
 *
 * Given a pointer to a currently passive partition entry, this function marks
 * this partition entry as active.
 *
 * \param[in] p Pointer to partition entry object
 *
 * \return -WM_E_INVAL if invalid parameter.
 * \return WM_SUCCESS if operation successful.
 */
int part_set_active_partition(struct partition_entry *p);

static inline int part_get_desc_from_id(enum flash_comp comp,
					flash_desc_t *fl)
{
	if (!fl)
		return -WM_E_INVAL;

	int rv = part_init();
	if (rv != 0)
		return rv;

	struct partition_entry *p = part_get_layout_by_id(comp, NULL);
	if (!p)
		return -WM_FAIL;

	part_to_flash_desc(p, fl);

	return WM_SUCCESS;
}

/* Check if the flash descriptor is within a partition in partition table.
 *
 * When return true, the input flash_desc_t 'f' can be either a subset of
 * a partition's flash space or exactly the same as this partition flash
 * space itself.
 *
 * \param[in] f Pointer to a flash descriptor for a flash space range.
 *
 * \return true if 'f' is part of an existing partition flash space.
 * \return false if not.
 */
bool part_is_flash_desc_within_one_partition(flash_desc_t *f);

#endif /* _PARTITION_H_ */
