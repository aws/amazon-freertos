/** @file  psm-v2.c
*
*  @brief Persistent storage manager version 2
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <wmlist.h>
#include <wmerrno.h>
#include <wm_utils.h>

#include <psm-v2.h>
#include "psm-internal.h"
#include "psm-v2-secure.h"
#include <partition.h>

#if defined(__linux__) || defined(__PSM_UTIL__)
#include <pthread.h>
#include <stdbool.h>
#else
#include <wm_os.h>
#endif /* __linux__ */

#if defined(BUILD_MINGW) && defined(__PSM_UTIL__)
size_t strnlen(const char *s, size_t maxlen);
#endif

#if defined(__linux__) || defined(__PSM_UTIL__)
#define mdev_t void

#define os_mem_alloc malloc
#define os_mem_realloc realloc
#define os_mem_free free
#define os_enter_critical_section(...) 1
#define os_exit_critical_section (void)

static void *os_mem_calloc(int size)
{
	void *ptr = malloc(size);
	if (!ptr)
		return NULL;

	memset(ptr, 0x00, size);
	return ptr;
}

#endif /* __linux__ */

#define OBJECT_TYPE_INVALID (object_type_t)0x0000
#define OBJECT_TYPE_SMALL_BINARY (object_type_t)0xAA55
#define OBJECT_TYPE_SMALL_BINARY_MAX_SIZE (uint16_t)(~0)

/*
 * Terminology:
 *
 * offset: Offset from the assigned flash block
 * address: Address of the flash device
 * bit clear: value is '1'
 * bit set: value is '0'
 * compaction: remove all inactive variables from flash.
 * indexing: Save the offset of active objects in RAM for direct access.
 */

/**
 * Defines the single read size during verification. Bigger size will speed
 * up verification but may cause stack overflows depending on application
 * stack usage pattern.
 */
#define READ_BUFFER_SIZE 64
#define PSM_INVALID_OBJ_ID 0xFFFF
#define PSM_DATA_SWAP_DEMARCATION 4
#define CHECKPOINT_MARKER 0x55AA
#define PSM_MAX_OBJNAME_LEN 256

/*
 * This is soft limit. Is defined to optimize memory usage considering
 * real world scenario. Can be increased upto what is supported by flash
 * object object-id data type.
 */
#define PSM_MAX_OBJECTS 512
#define PSM_DEFAULT_INDEX_ELEMENTS 10

/**
 * fixme: This value needs to be asked from flash driver. Fix this later
 * when appropriate API is added in flash driver.
 */
#define PSM_FLASH_SECTOR_SIZE 4096

enum {
	PSM_OBJ_INACTIVE = 0x01,
	PSM_OBJ_CACHE_REQ = 0x02,
	PSM_OBJ_INDEX_REQ = 0x04,
	PSM_OBJ_SECURE_MASK = 0x18,
};

/*
 * Encoding of secure status bits (PSM_OBJ_SECURE_MASK) is as follows:
 * (2b' stands for 2-bit binary bit-field)
 * 2b'00 : Encrypted
 * 2b'01 : Unencrypted
 * 2b'10 : Encrypted (legacy value, update to 2b'00)
 * 2b'11 : Unset, set to either 2b'00 or 2b'01
 * Values are shifted by secure status bit-position.
 */
enum {
	PSM_OBJ_SECURE = 0x00,
	PSM_OBJ_UNSECURE = 0x08,
	PSM_OBJ_SECURE_LEGACY = 0x10,
	PSM_OBJ_SECURE_UNSET = 0x18
};

enum {
	PSM_WRITE_NO_VERIFY = false,
	PSM_WRITE_VERIFY = true
};

typedef uint16_t objectid_t;
typedef uint16_t object_type_t;

PACK_START typedef struct {
	uint8_t name_len;
	uint16_t data_len;
	/* If secure PSM is enabled, the name variable will be encrypted. */
	/* void *name */
	/* If secure PSM is enabled, the data buffer will be in decrypted
	   state */
	/* void *data */
} PACK_END psm_cache_t;

typedef struct psm_index_s {
	uint32_t name_hash;
	uint16_t obj_id;
	uint32_t flash_offset;
	psm_cache_t *cache;
} psm_index_t;

/* Every PSM object has this structure on the flash */
PACK_START typedef struct {
	object_type_t object_type;
	/** Flags:
	 * [7-5]: Reserved
	 * [3:4]  : Secure status bits
	 * [2]  : Index request
	 * [1]  : Cache request
	 * [0]  : Active status
	 */
	uint8_t flags;
	/** CRC32 of entire psm object including metadata */
	uint32_t crc32;
	/** Length of the data field */
	uint16_t data_len;

	/** Object ID: A unique id for the object name ASCII string */
	objectid_t obj_id;
	/** Length of object name. If this is zero the object has been
	 * seen before and obj_id->name mapping is present. X-Ref: macro
	 * PSM_MAX_OBJNAME_LEN
	 */
	uint8_t name_len;
	/** Variable length object name string. */
	/* uint8_t name[<name_len>]; */
	/** Binary data here. Length == psm_object.data_len member above */
	/* uint8_t bin_data[<len>]; */
} PACK_END psm_object_t;

PACK_START typedef struct {
	uint32_t swap_start_offset;
	uint32_t swap_start_offset_inv;
	uint16_t checkpoint_marker;
	uint16_t checkpoint_marker_inv;
} PACK_END psm_swap_desc_t;

typedef struct {
#if defined(__linux__) || defined(__PSM_UTIL__)
	pthread_rwlock_t prwlock;
#else
	os_rw_lock_t prwlock;
#endif /* __linux__ */

	uint32_t total_active_object_size;
	uint32_t object_offset_limit;
	uint32_t data_free_offset;
	/*
	 * We do not re-use object ID's of inactivated objects. It is
	 * expected that number of used object ID will never actually reach
	 * the highest number we support. After erase object ID's are
	 * re-programmed.
	 */
	objectid_t highest_used_obj_id;
	uint32_t index_max_elements;
	uint32_t index_elements;
	psm_index_t *index;
	psm_cfg_t psm_cfg;

	/* Flash driver handle */
	uint8_t fdesc_fl_dev;
	mdev_t *fl_dev;
	uint32_t block_size;
	uint32_t block_cnt;
	uint32_t part_size;
	uint32_t start_address;
#ifdef CONFIG_SECURE_PSM
	psm_sec_hnd_t psm_sec_hnd;
	uint8_t psm_enc_buf[READ_BUFFER_SIZE];
	/* This will hold encrypted copy of the name for the whole time
	   the object is open in write mode. For read mode it is valid
	   only for some time during open */
	uint8_t psm_name_enc_buf[PSM_MAX_OBJNAME_LEN];
#endif

	struct list_head node;
} psm_t;

/* Ephemeral RAM structure corresponding to PSM object in flash */
typedef struct psm_objinfo_s {
	psm_t *psm;
	psm_object_t pobj;
	/* Start of object offset */
	uint32_t object_flash_offset;
	/* Will move according to user operations on the object */
	uint32_t curr_flash_offset;
	psm_mode_t mode;
	/* Communicated by caller in psm_object_open(). */
	uint32_t max_data_len;
	/** CRC32 calculated on the fly in read mode */
	uint32_t crc32;
	bool old_active_object_present;
	/* Pointer to old object structure, if any */
	uint32_t old_object_flash_offset;
	uint32_t old_object_size;

	psm_index_t *index;
} psm_objinfo_t;

#define block_size(__psm__) (__psm__->block_size)
#define block_cnt(__psm__) (__psm__->block_cnt)
#define part_size(__psm__) (__psm__->part_size)
#define start_address(__psm__) (__psm__->start_address)
#define fldev(__psm__)  (__psm__->fl_dev)

#define offset_to_address(__psm__, __offset__)	\
	(__psm__->start_address + __offset__)

#define align_to_block_offset(__psm__, __offset__)	\
	(__offset__ & ~(__psm__->block_size - 1))

#define is_offset_block_aligned(__psm__, __offset__)	\
	(!(__offset__ & (__psm__->block_size - 1)))

#define object_size(__pobj__)						\
	(sizeof(psm_object_t) + (__pobj__)->name_len + (__pobj__)->data_len)

#define psm_is_data_len_sane(__psm__, __len__)			\
	((__len__ < OBJECT_TYPE_SMALL_BINARY_MAX_SIZE) &&	\
	 (__len__ < (part_size(psm)/2)))

#define metadata_size(__pobj__)				\
	(sizeof(psm_object_t) + (__pobj__)->name_len)

#define is_object_active(__pobj__)		\
	((__pobj__)->flags & PSM_OBJ_INACTIVE)
#define is_object_indexable(__pobj__)			\
	(!((__pobj__)->flags & PSM_OBJ_INDEX_REQ))
#define is_object_cacheable(__pobj__)			\
	(!((__pobj__)->flags & PSM_OBJ_CACHE_REQ))

#ifdef CONFIG_SECURE_PSM
#define get_object_secure_bits(__pobj__)			\
	((__pobj__)->flags & PSM_OBJ_SECURE_MASK)

#define set_object_secure_bits(__pobj__, bitval)			\
	do {								\
		(__pobj__)->flags =					\
		(((__pobj__)->flags & ~PSM_OBJ_SECURE_MASK) | (bitval));\
	} while (0)

static inline bool is_object_secure(psm_object_t *__pobj__)
{
	uint8_t sec_status = get_object_secure_bits(__pobj__);

	switch (sec_status) {
	case PSM_OBJ_SECURE:
		return true;
	case PSM_OBJ_SECURE_LEGACY:
		return true;
	case PSM_OBJ_UNSECURE:
		return false;
	case PSM_OBJ_SECURE_UNSET:
		if (psm_is_key_present_in_keystore())
			return true;
		else
			return false;
	}

	/* Default is false */
	return false;
}

#define set_object_secure(__pobj__)			\
	set_object_secure_bits((__pobj__), PSM_OBJ_SECURE)

#define clear_object_secure(__pobj__)			\
	set_object_secure_bits((__pobj__), PSM_OBJ_UNSECURE)
#endif /* CONFIG_SECURE_PSM */

#define remaining_part_space(__psm__) (part_size(psm) - data_free_offset)

#define part_readonly(__psm__) (__psm__->psm_cfg.read_only)
#define part_encrypted(__psm__) (__psm__->psm_cfg.secure)

psm_event_callback g_psm_event_cb;

/* This is a list of all initialized PSM partitions */
static struct list_head part_list;

/* Only for test code. */
static bool force_psm_secure;

#ifdef CONFIG_PSM_EXTRA_DEBUG
void psm_print_object(psm_object_t *pobj)
{
	wmprintf("*******************\r\n");
	wmprintf("object_type: %x\r\n", pobj->object_type);
	wmprintf("flags       : %x\r\n", pobj->flags);
	wmprintf("crc32    : %x\r\n", pobj->crc32);
	wmprintf("data_len         : %d\r\n", pobj->data_len);
	wmprintf("obj_id      : %d\r\n", pobj->obj_id);
	wmprintf("name_len    : %d\r\n", pobj->name_len);
	wmprintf("*******************\r\n");
}

void psm_dump_hex(const char *buf, uint32_t size)
{
	int index = 0;
	wmprintf("-----------------\r\n");
	for (index = 0; index < size; index++) {
		if (isprint((unsigned char)buf[index]))
			wmprintf("%2x-(%c) ", (unsigned char)buf[index],
				 buf[index]);
		else
			wmprintf("%2x-( ) ", (unsigned char)buf[index]);
		if (!((index + 1) % 4))
			wmprintf("\r\n");
	}
	wmprintf("\r\n-----------------\r\n");
}
#endif /* CONFIG_PSM_EXTRA_DEBUG */

static int psm_init_locks(psm_t *psm)
{
#if defined(__linux__) || defined(__PSM_UTIL__)
	int rv = pthread_rwlock_init(&psm->prwlock, NULL);
#else
	int rv = os_rwlock_create(&psm->prwlock, "prwmutex", "prwlock");
#endif /* __linux__ */
	if (rv != WM_SUCCESS) {
		psm_d("%s: rwlock create failed", __func__);
		return rv;
	}

	return rv;
}

static int psm_deinit_locks(psm_t *psm)
{
#if defined(__linux__) || defined(__PSM_UTIL__)
	return pthread_rwlock_destroy(&psm->prwlock);
#else
	os_rwlock_delete(&psm->prwlock);
	return WM_SUCCESS;
#endif /* __linux__ */
}

int psm_rwlock_part(psm_t *psm, psm_mode_t mode)
{
#ifdef CONFIG_PSM_LOCKS_DEBUG
	psm_entry_i("mode: %d", mode);
#endif /* CONFIG_PSM_LOCKS_DEBUG */

#if defined(__linux__) || defined(__PSM_UTIL__)
	if (mode == PSM_MODE_WRITE)
		pthread_rwlock_wrlock(&psm->prwlock);
	else
		pthread_rwlock_rdlock(&psm->prwlock);
#else
	if (mode == PSM_MODE_WRITE)
		os_rwlock_write_lock(&psm->prwlock, OS_WAIT_FOREVER);
	else
		os_rwlock_read_lock(&psm->prwlock, OS_WAIT_FOREVER);
#endif /* __linux__ */

	return WM_SUCCESS;
}

void psm_rwunlock_part(psm_t *psm, psm_mode_t mode)
{
#ifdef CONFIG_PSM_LOCKS_DEBUG
	psm_entry_i("mode: %d", mode);
#endif /* CONFIG_PSM_LOCKS_DEBUG */

#if defined(__linux__) || defined(__PSM_UTIL__)
	pthread_rwlock_unlock(&psm->prwlock);
#else
	if (mode == PSM_MODE_WRITE)
		os_rwlock_write_unlock(&psm->prwlock);
	else
		os_rwlock_read_unlock(&psm->prwlock);
#endif /* __linux__ */
}

static bool psm_is_objid_available(psm_t *psm)
{
	psm_entry_i();
	if (psm->highest_used_obj_id == (PSM_INVALID_OBJ_ID - 1))
		return false;
	else
		return true;
}

bool psm_is_free_space(const psm_object_t *pobj)
{
	if (pobj->object_type == (object_type_t)~0)
		return true;

	return false;
}

/*
 * Block size can be a multiple of sector size. For now keep it same as
 * sector size.
 */
static int psm_get_block_size()
{
	return PSM_FLASH_SECTOR_SIZE;
}

static objectid_t psm_get_unused_obj_id(psm_t *psm)
{
	psm_entry_i();
	if (psm->highest_used_obj_id >= (objectid_t)PSM_MAX_OBJECTS) {
		/*
		 * It is expected that this case would never occur. Put
		 * other way the number of unique objects in PSM can never
		 * cross PSM_MAX_OBJECTS. During compaction we re-program
		 * the object IDs. So, holes are not an issue anyway.
		 */
		psm_d("Invalid use case");
		return PSM_INVALID_OBJ_ID;
	}

	psm->highest_used_obj_id++;
	psm_ed("New object id assigned: 0x%x", psm->highest_used_obj_id);
	return psm->highest_used_obj_id;
}

static uint32_t psm_calc_metadata_crc32(psm_object_t *pobj,
					uint32_t partial_crc32)
{
	/* Backup */
	uint32_t temp_crc32 = pobj->crc32;
	/* Init */
	pobj->crc32 = ~0;
	/* Calculate */
	uint32_t crc32 = soft_crc32(pobj, sizeof(psm_object_t), partial_crc32);
	/* Restore */
	pobj->crc32 = temp_crc32;
	return crc32;
}

/*
 * Ensure that offset and size is block aligned.
 */
static int psm_erase_flash(psm_t *psm, uint32_t offset, uint32_t size)
{
	psm_ll("%s: offset: 0x%x size: 0x%x", __func__, offset, size);

	if ((offset % block_size(psm)) || (size % block_size(psm))) {
		psm_d("Misaligned offset or size: %x: %d", offset, size);
		return -WM_E_INVAL;
	}

	if ((offset + size) > part_size(psm)) {
		psm_d("Invalid params given to erase: %u: %u", offset, size);
		return -WM_E_INVAL;
	}

	uint32_t address = offset_to_address(psm, offset);
	int rv = flash_drv_erase(fldev(psm), address, size);
	if (rv != WM_SUCCESS) {
		psm_d("Unable to erase flash data %x:%d", address, size);
		return rv;
	}

	if (g_psm_event_cb)
		g_psm_event_cb(PSM_EVENT_ERASE, (void *)((unsigned long)address)
				, (void *)((unsigned long)size));

	return rv;
}

int psm_read_flash(psm_t *psm, void *buffer, uint32_t size,
			  uint32_t offset)
{
	psm_ll("%s: %x, %d", __func__, offset, size);

	if (size == 0)
		psm_d("%s: Zero length read from 0x%x", __func__,
		      (uint32_t) __builtin_return_address(0));

	if ((offset + size) > part_size(psm)) {
		psm_d("Invalid params given to read: %u: %u", offset, size);
		return -WM_E_INVAL;
	}

	uint32_t address = offset_to_address(psm, offset);

	int rv = flash_drv_read(fldev(psm), buffer, size, address);
	if (rv != WM_SUCCESS) {
		psm_d("Unable to read from flash: %x: %d", address, size);
		return rv;
	}

	if (g_psm_event_cb)
		g_psm_event_cb(PSM_EVENT_READ, (void *)((unsigned long)address),
				(void *)((unsigned long)size));

	return rv;
}

static int psm_write_verify(psm_t *psm, const uint8_t *reference_buf,
			    uint32_t size, uint32_t offset)
{
	psm_entry_i();
	int rv;
	uint32_t read_size;
	char buf[READ_BUFFER_SIZE];
	uint32_t index;
	for (index = 0; index < size; index += read_size) {
		uint32_t remaining = (size - index);
		read_size = remaining > READ_BUFFER_SIZE ?
			READ_BUFFER_SIZE : remaining;
		rv = psm_read_flash(psm, buf, read_size, offset + index);
		if (rv != WM_SUCCESS) {
			psm_d("%s: psm_read_flash %x: %d failed", __func__,
			      offset + index, read_size);
			return rv;
		}

		if (memcmp(&reference_buf[index], buf, read_size)) {
			psm_d("%s: verification failed @ offset: %x: %d",
			      __func__, offset + index, read_size);
			return -WM_FAIL;
		}
	}

	return WM_SUCCESS;
}

/*
 * This function is necessary only when source and destination address is
 * from the same flash.
 */
static int psm_write_flash_same_device_rw(psm_t *psm, const void *buffer,
					  uint32_t size, uint32_t address)
{
	int rv = -WM_FAIL;
	const int tmpbuf_size = 16;
	void *tmpbuf = os_mem_alloc(tmpbuf_size);
	if (!tmpbuf)
		return -WM_E_NOMEM;

	uint32_t remaining_size = size;
	while (remaining_size) {
		int chunk_size = remaining_size > tmpbuf_size ?
			tmpbuf_size : remaining_size;
		memcpy(tmpbuf, buffer, chunk_size);
		rv = flash_drv_write(fldev(psm), tmpbuf, chunk_size, address);
		if (rv != WM_SUCCESS) {
			psm_d("Unable to write to flash: %x: %d",
			      address, size);
			break;
		}

		address += chunk_size;
		remaining_size -= chunk_size;
		buffer = (const uint8_t *) buffer + chunk_size;
	}

	os_mem_free(tmpbuf);
	return rv;
}

/**
 * If verify is 'true' the written data will be re-read and verified with
 * original given buffer. Note that this uses heap to allocate read
 * buffer.
 */
static int psm_write_flash(psm_t *psm, const void *buffer, uint32_t size,
			   uint32_t offset, bool verify)
{
	psm_ll("%s: %x, %d", __func__, offset, size);

	if ((offset + size) > part_size(psm)) {
		psm_d("Invalid params given to write: %u: %u", offset, size);
		return -WM_E_INVAL;
	}

	int rv;
	uint32_t address = offset_to_address(psm, offset);

	rv = flash_drv_write(fldev(psm), (void *) buffer, size, address);
	if (rv != WM_SUCCESS) {
		if (rv == -WM_E_FAULT)
			rv = psm_write_flash_same_device_rw(psm, buffer,
							    size, address);
		if (rv != WM_SUCCESS)
			psm_d("(rw-flash) Unable to write to flash: %x: %d",
			      address, size);
		return rv;
	}

	if (g_psm_event_cb)
		g_psm_event_cb(PSM_EVENT_WRITE, (void *)((unsigned long)address)
				, (void *)((unsigned long)size));

	if (verify)
		return psm_write_verify(psm, buffer, size, offset);

	return WM_SUCCESS;
}

static int write_flag_bit(psm_t *psm, uint32_t flag_offset, uint8_t flag)
{
	/*
	 * Note: As per flash properties '0' value of a bit cannot be
	 * changed to '1' unless the flash sector is erased. Hence, we do
	 * not need to do a read-modify-write cycle to set a particular bit
	 * to zero as long as other bits in the written byte are '1'
	 */
	uint8_t inv_flag = ~(flag);
	return psm_write_flash(psm, &inv_flag, sizeof(uint8_t),
			       flag_offset, PSM_WRITE_NO_VERIFY);
}

static int read_swap_descriptor(psm_t *psm, psm_swap_desc_t *swap_desc)
{
	psm_entry_i();
	uint32_t offset = part_size(psm) - sizeof(psm_swap_desc_t);
	int rv = psm_read_flash(psm, swap_desc, sizeof(psm_swap_desc_t),
				offset);
	if (rv != WM_SUCCESS) {
		psm_d("Unable to read swap descriptor");
		return rv;
	}

	return WM_SUCCESS;
}

static int write_swap_descriptor(psm_t *psm,
				 const psm_swap_desc_t *swap_desc,
				 bool write_swap_offset)
{
	uint32_t offset = part_size(psm) - sizeof(psm_swap_desc_t);
	if (write_swap_offset) {
		psm_entry_i("swap_offset: 0x%x", swap_desc->swap_start_offset);
		int rv =
			psm_write_flash(psm, &swap_desc->swap_start_offset,
					sizeof(swap_desc->swap_start_offset) *
					2, offset, PSM_WRITE_VERIFY);
		if (rv != WM_SUCCESS) {
			psm_d("Unable to write swap descriptor offset");
			return rv;
		}
	} else {
		psm_entry_i("marker: 0x%x", swap_desc->checkpoint_marker);
		offset += offsetof(psm_swap_desc_t, checkpoint_marker);
		int rv = psm_write_flash(psm, &swap_desc->checkpoint_marker,
					 sizeof(swap_desc->checkpoint_marker) *
					 2, offset, PSM_WRITE_VERIFY);
		if (rv != WM_SUCCESS) {
			psm_d("Unable to write swap descriptor marker");
			return rv;
		}
	}

	return WM_SUCCESS;
}

static int invalidate_swap_descriptor(psm_t *psm)
{
	psm_entry_i();
	psm_swap_desc_t swap_desc;
	memset(&swap_desc, 0x00, sizeof(psm_swap_desc_t));

	uint32_t offset = part_size(psm) - sizeof(psm_swap_desc_t);
	int rv = psm_write_flash(psm, &swap_desc, sizeof(psm_swap_desc_t),
				 offset, PSM_WRITE_VERIFY);
	if (rv != WM_SUCCESS) {
		psm_d("Unable to invalidate swap descriptor offset");
		return rv;
	}

	return WM_SUCCESS;
}

typedef enum {
	/*
	 * - Data area good.
	 * - No/Invalid swap descriptor present.
	 * - Compaction to be done.
	 */
	CHECKPOINT_0,
	/*
	 * - Data area good.
	 * - Compaction has/was started.
	 * - Swap offset is correct.
	 * - Checkpoint marker is not correct.
	 */
	CHECKPOINT_1,
	/*
	 * - Swap area is good.
	 * - Data incorrect assumed.
	 * - Valid swap descriptor present with checkpoint marker.
	 */
	CHECKPOINT_2,
	/*
	 * - Data area is good.
	 * - Swap area descriptor is invalid.
	 * - Compaction is not needed. Only the swap needs to be brought to
	 *   init state.
	 */
	CHECKPOINT_3,
	/*
	 * - Data area is good.
	 * - Swap area is clean.
	 * - No-op
	 */
	CHECKPOINT_4,
} checkpoint_t;

static uint32_t psm_get_swap_start_offset(psm_t *psm)
{
	return align_to_block_offset(psm, (part_size(psm) -
					   sizeof(psm_swap_desc_t) -
					   psm->total_active_object_size));
}

#ifdef CONFIG_SECURE_PSM
int resetkey(psm_t *psm, psm_resetkey_mode_t mode)
{
	return psm_key_reset(psm->psm_sec_hnd, mode);
}

int encrypt_buf(psm_t *psm, const void* plain, void *cipher,
			      int len)
{
	return psm_encrypt(psm->psm_sec_hnd, plain, cipher, len);
}

int decrypt_buf(psm_t *psm, const void *cipher, void *plain,
			      int len)
{
	return psm_decrypt(psm->psm_sec_hnd, cipher, plain, len);
}
#endif /* CONFIG_SECURE_PSM */

static void psm_reset_index(psm_t *psm)
{
	psm_entry_i();
	if (!psm->index) {
		psm->index_elements = 0;
		psm->index_max_elements = 0;
		return;
	}

	int cnt;
	for (cnt = 0; cnt < psm->index_elements; cnt++) {
		if (psm->index[cnt].cache) {
			os_mem_free(psm->index[cnt].cache);
			psm->index[cnt].cache = NULL;
		}
	}

	os_mem_free(psm->index);
	psm->index = NULL;
	psm->index_elements = 0;
	psm->index_max_elements = 0;
}

/*
 * Also copies the name to the name buffer is buffer address is supplied.
 */
static int psm_calc_name_hash(psm_t *psm, psm_object_t *pobj,
			      uint32_t flash_offset, uint32_t *name_hash,
			      uint8_t *name_buf)
{
	uint8_t buf[READ_BUFFER_SIZE];
	uint32_t crc32 = 0;
	uint8_t read_size;
	uint8_t index;
	uint32_t name_flash_offset = flash_offset + sizeof(psm_object_t);

	flash_offset += sizeof(psm_object_t);
	for (index = 0; index < pobj->name_len; index += read_size) {
		uint8_t remaining = pobj->name_len - index;
		read_size = remaining > READ_BUFFER_SIZE ?
			READ_BUFFER_SIZE : remaining;
		int rv = psm_read_flash(psm, buf, read_size, name_flash_offset);
		if (rv != WM_SUCCESS) {
			psm_d("%s: Unable to read name", __func__);
			return rv;
		}

		if (name_buf)
			memcpy(&name_buf[index], buf, read_size);

		name_flash_offset += read_size;
		crc32 = soft_crc32(buf, read_size, crc32);
	}

	*name_hash = crc32;
	return WM_SUCCESS;
}

static void psm_add_to_index(psm_t *psm, psm_object_t *pobj,
			     uint32_t flash_offset)
{
	psm_i("Indexing obj_id: %d", pobj->obj_id);
	if (!psm->index) {
		psm_i("Allocating index");
		uint32_t index_size = sizeof(psm_index_t) *
			PSM_DEFAULT_INDEX_ELEMENTS;
		psm->index = os_mem_calloc(index_size);
		if (!psm->index) {
			psm_d("Index allocation failed");
			return;
		}

		psm->index_elements = 0;
		psm->index_max_elements = PSM_DEFAULT_INDEX_ELEMENTS;
	}

	if (psm->index_elements == psm->index_max_elements) {
		/* Indexing space is over. Try to get more space */
		psm_i("Increasing index allocation");
		uint32_t current_index_size = sizeof(psm_index_t) *
			psm->index_max_elements;
		uint32_t new_index_size = current_index_size +
			(sizeof(psm_index_t) * PSM_DEFAULT_INDEX_ELEMENTS);
		psm_index_t *new_index = os_mem_realloc(psm->index,
							new_index_size);
		if (!new_index) {
			psm_d("New index allocation failed: %d",
			      new_index_size);
			return;
		}

		/* Zero out new allocated area */
		memset(&new_index[psm->index_max_elements], 0x00,
		       (sizeof(psm_index_t) * PSM_DEFAULT_INDEX_ELEMENTS));
		psm->index = new_index;
		psm->index_max_elements += PSM_DEFAULT_INDEX_ELEMENTS;
	}

	psm_cache_t *cache = NULL;
	uint8_t *name_buf = NULL;
	uint8_t *data_buf = NULL;
	if (is_object_cacheable(pobj)) {
		uint32_t cache_size = sizeof(psm_cache_t) +
			pobj->name_len + pobj->data_len;
		cache = os_mem_calloc(cache_size);
		if (cache) {
			cache->name_len = pobj->name_len;
			cache->data_len = pobj->data_len;
			/* Point name buf to name field of cache */
			name_buf = (uint8_t *) (cache + 1);
			data_buf = name_buf + cache->name_len;
		} else {
			psm_d("Cache allocation failed. Ignoring");
		}
	}

	psm_index_t *index = &psm->index[psm->index_elements];

	uint32_t name_hash = 0;

	int rv = psm_calc_name_hash(psm, pobj, flash_offset,
				    &name_hash, name_buf);
	if (rv != WM_SUCCESS) {
		psm_d("Name_Hash calculation failed");
		os_mem_free(cache);
		return;
	}

	index->name_hash = name_hash;
	index->obj_id = pobj->obj_id;
	index->flash_offset = flash_offset;
	index->cache = cache;

	if (!cache) {
		psm->index_elements++;
		return;
	}

	psm_i("Object %d is also cacheable. Caching name and value (size: %d)",
	      pobj->obj_id, cache->data_len);
	/*
	 * Object is also cacheable. Name field is already read and
	 * cached. Other fields of cache object are populated. Now cache
	 * the data field.
	 */
	if (cache->data_len != 0) {
		rv = psm_read_flash(psm, data_buf, cache->data_len,
				    flash_offset + metadata_size(pobj));
		if (rv != WM_SUCCESS) {
			psm_d("Failed to cache name field");
			os_mem_free(cache);
			memset(index, 0x00, sizeof(psm_index_t));
			return;
		}

#ifdef CONFIG_SECURE_PSM
		if (part_encrypted(psm)) {
			rv = resetkey(psm, SET_DECRYPTION_KEY);
			if (rv != WM_SUCCESS) {
				psm_d("reset enc key (data_i) fail");
				os_mem_free(cache);
				memset(index, 0x00, sizeof(psm_index_t));
				return;
			}

			/* Use 'psm_name_enc_buf' as temp. buffer */
			rv = decrypt_buf(psm, name_buf, psm->psm_name_enc_buf,
					 cache->name_len);
			if (rv != WM_SUCCESS) {
				os_mem_free(cache);
				memset(index, 0x00, sizeof(psm_index_t));
				return;
			}

			rv = decrypt_buf(psm, data_buf, data_buf,
					 cache->data_len);
			if (rv != WM_SUCCESS) {
				psm_d("dec-i fail");
				os_mem_free(cache);
				memset(index, 0x00, sizeof(psm_index_t));
				return;
			}
		}
#endif /* CONFIG_SECURE_PSM */
	}

	psm->index_elements++;
}

/*
 * Return -WM_FAIL if already set. Set bit and return WM_SUCCESS
 * otherwise.
 */
static int test_and_set_bitmap(uint8_t *bitmap, objectid_t obj_id)
{
	uint32_t bit_num = (uint32_t) obj_id;
	uint32_t array_offset = bit_num / (sizeof(uint8_t) * 8);
	uint32_t bit_offset = bit_num % (sizeof(uint8_t) * 8);

	if (bitmap[array_offset] & (uint8_t)(1 << bit_offset))
		return -WM_FAIL; /* Already set */

	bitmap[array_offset] |= (uint8_t)(1 << bit_offset);
	return WM_SUCCESS;
}

/*
 * Kill the object. A recovery would be needed invariably.
 */
static void handle_double_active_object(psm_t *psm,
					uint32_t object_offset)
{
	/*
	 * Kill the object and invalidating the object type field. All
	 * objects after this object will become effectively lost after
	 * this. This is not so bad because a double active object case
	 * would is expected to occur when an new object write did not
	 * complete successfully. Thus, there should be no objects after
	 * this.
	 */
	object_type_t invalid_obj = OBJECT_TYPE_INVALID;
	psm_write_flash(psm, &invalid_obj, sizeof(invalid_obj),
			object_offset, false);
}

static int psm_part_scan(psm_t *psm)
{
	psm_entry_i();

	uint32_t offset = 0;

	uint32_t limit = psm->object_offset_limit;
	psm_object_t pobj;
	objectid_t highest_found = 0;

	uint32_t bitmap_size = (PSM_MAX_OBJECTS / 8) + 1;

	/* A bitmap to map all active objects */
	uint8_t *bitmap = os_mem_calloc(bitmap_size);
	if (!bitmap) {
		psm_e("unable to alloc psm object bitmap");
		return -WM_E_NOMEM;
	}

	int rv = WM_SUCCESS, rv1;

	psm->total_active_object_size = 0;
	psm_reset_index(psm);

	while (offset < limit) {
		psm_isc("offset: %p (%p) limit: %p (%p) ",
			(void *)offset, (void *)offset_to_address(psm, offset),
			(void *)limit, (void *)offset_to_address(psm, limit));
		rv1 = psm_read_flash(psm, &pobj, sizeof(psm_object_t),
				    offset);
		if (rv1 != WM_SUCCESS) {
			psm_w("scan: could not read object @ off: %x", offset);
			rv = rv1;
			break;
		}

		if (psm_is_free_space(&pobj)) {
			psm_isc("Free space marker found");
			break;
		}

		if (pobj.object_type != OBJECT_TYPE_SMALL_BINARY) {
			psm_w("scan: Invalid obj (%x) at the end of"
			      " data area off: %p addr: %p", pobj.object_type,
			      (void *)((long)offset),
			      (void *)((long)offset_to_address(psm, offset)));
			rv = -WM_FAIL;
			break;
		}

		if (!psm_is_data_len_sane(psm, pobj.data_len)) {
			/* Invalid length of object. Need recovery */
			psm_w("PSM invalid object found during scan.");
			rv = -WM_FAIL;
			break;
		}

		if (pobj.obj_id > highest_found)
			highest_found = pobj.obj_id;

		if (!is_object_active(&pobj)) {
			offset += object_size(&pobj);
			continue;
		}

		rv1 = test_and_set_bitmap(bitmap, pobj.obj_id);
		if (rv1 == -WM_FAIL) {
			psm_w("Double active object %d detected", pobj.obj_id);
			handle_double_active_object(psm, offset);
			/* Report error to start recovery */
			rv = rv1;
			break;
		}

#ifdef CONFIG_SECURE_PSM
		/*
		 * Check for partition encryption status vs. object encryption
		 * status mismatch
		 */
		if ((part_encrypted(psm) && !is_object_secure(&pobj)) ||
			   (!part_encrypted(psm) && is_object_secure(&pobj))) {
			/*
			 * Report error to start recovery/upgrade.
			 * Continue scanning however.
			 */
			psm_d("PSM secure status mismatch found during"
			      " scan");
			rv = -WM_FAIL;
			offset += object_size(&pobj);
			psm->total_active_object_size +=
						object_size(&pobj);
			continue;
		}
#else
		/*
		 * Ignore this case, as no recovery/upgrade is possible because
		 * of absence of encryption/decryption functions
		 */
#endif /* CONFIG_SECURE_PSM */

		if (is_object_indexable(&pobj))	/* Ignore any error */
			psm_add_to_index(psm, &pobj, offset);

		psm_isc("Obj: Name Len: %d  Data Len: %d Active: %d",
			pobj.name_len, pobj.data_len, is_object_active(&pobj));

		/* Only active objects length is added */
		psm->total_active_object_size += object_size(&pobj);
		offset += object_size(&pobj);
	}

	psm_isc("Scan complete: Data Free Offset: %p Phy. Address: %p",
		offset, offset_to_address(psm, offset));

	psm->highest_used_obj_id = highest_found;
	psm->data_free_offset = offset;
	psm_d("data_free_offset set to 0x%x", psm->data_free_offset);

	os_mem_free(bitmap);
	return rv;
}

/*
 * - Copies an object from source offset (swap) to dest offset (data).
 * - Verification of CRC is also done on-the-go. If the CRC is wrong, the
 *   destination object is marked inactive.
 * - Assumes that the source psm_object_t is already read and pobj param is
 *   populated.
 */
static int psm_copy_object(psm_t *psm, psm_object_t *pobj,
			   uint32_t source_offset, uint32_t dest_offset,
			   objectid_t new_objid)
{
	psm_entry_i("obj-id: 0x%x len: %d source: 0x%x dest: 0x%x",
		    pobj->obj_id, pobj->data_len, source_offset, dest_offset);

	psm_ed("Copying object %d\r\n", pobj->obj_id);

	/*
	 * The destination object ID is different than that of source. CRC
	 * comparison needs to take care of this fact. Basically, CRC needs
	 * to be calculated separately for source object and destination
	 * object.
	 */
	uint32_t src_ref_crc32 = pobj->crc32;
	uint32_t cal_crc32 = 0;
	objectid_t saved_src_obj_id = pobj->obj_id;

	uint32_t dest_object_start_offset = dest_offset;

	/* Update Object iD for destination object */
	pobj->obj_id = new_objid;
	/* Keep CRC32 field uninitialized */
	pobj->crc32 = (uint32_t)~0;

	if (g_psm_event_cb)
		g_psm_event_cb(PSM_EVENT_COMPACTION,
			       (void *)PSM_COMPACTION_10,
			       (void *)PSM_COMPACTION_MAX);

	int rv = psm_write_flash(psm, pobj, sizeof(psm_object_t),
				 dest_offset, PSM_WRITE_NO_VERIFY);
	if (rv != WM_SUCCESS) {
		psm_d("%s: metadata write failed", __func__);
		return rv;
	}

	dest_offset += sizeof(psm_object_t);
	source_offset += sizeof(psm_object_t);

	uint32_t copy_size = pobj->name_len + pobj->data_len;

	uint8_t buf[READ_BUFFER_SIZE];
	uint32_t index;
	uint32_t read_size;
	for (index = 0; index < copy_size; index += read_size) {
		uint32_t remaining = copy_size - index;
		read_size = remaining > READ_BUFFER_SIZE ?
			READ_BUFFER_SIZE : remaining;

		int rv = psm_read_flash(psm, buf, read_size, source_offset);
		if (rv != WM_SUCCESS) {
			psm_d("%s: read failed", __func__);
			return rv;
		}

		rv = psm_write_flash(psm, buf, read_size, dest_offset,
				     PSM_WRITE_NO_VERIFY);
		if (rv != WM_SUCCESS) {
			psm_d("%s: write failed", __func__);
			return rv;
		}

		cal_crc32 = soft_crc32(buf, read_size, cal_crc32);

		source_offset += read_size;
		dest_offset += read_size;
	}

	uint32_t dest_crc32 = psm_calc_metadata_crc32(pobj, cal_crc32);

	/* Restore obj_id to calculate CRC32 */
	pobj->obj_id = saved_src_obj_id;
	uint32_t src_crc32 = psm_calc_metadata_crc32(pobj, cal_crc32);

	psm_ed("src_ref_crc32: 0x%x src_crc32: 0x%x dest_crc32: 0x%x",
	       src_ref_crc32, src_crc32, dest_crc32);
	/*
	 * Check is source object CRC32 was correct. We are doing this
	 * late to avoid two passes over source object data.
	 */
	if (src_crc32 == src_ref_crc32) {
		/*
		 * Write new CRC32. We have to update CRC32 because we have
		 * programmed new obj-id.
		 */
		uint32_t crc32_offset = dest_object_start_offset +
			offsetof(psm_object_t, crc32);
		rv = psm_write_flash(psm, &dest_crc32, sizeof(dest_crc32),
				     crc32_offset, PSM_WRITE_NO_VERIFY);
		if (rv != WM_SUCCESS) {
			psm_d("Failed to update CRC32 of copied object");
			return rv;
		}

		return WM_SUCCESS;
	}

	/*
	 * Mark inactive just copied object because CRC calculation
	 * failed. This effectively deletes the object from the PSM.
	 */
	psm_d("Object: 0x%x has failed CRC during copy. Marking inactive",
	      pobj->obj_id);
	uint32_t flag_offset = dest_object_start_offset +
		offsetof(psm_object_t, flags);
	rv = write_flag_bit(psm, flag_offset, PSM_OBJ_INACTIVE);
	if (rv != WM_SUCCESS) {
		psm_d("Failed to mark copied object inactive");
		return rv;
	}

	return WM_SUCCESS;
}

static int psm_upgrade_object(psm_t *psm, psm_object_t *pobj,
			   uint32_t source_offset, uint32_t dest_offset,
			   objectid_t new_objid)
{
	psm_entry_i("obj-id: 0x%x len: %d source: 0x%x dest: 0x%x",
		    pobj->obj_id, pobj->data_len, source_offset, dest_offset);

	psm_ed("Upgrading object %d\r\n", pobj->obj_id);

	/*
	 * The destination object ID is different than that of source. CRC
	 * comparison needs to take care of this fact. Basically, CRC needs
	 * to be calculated separately for source object and destination
	 * object.
	 */
	uint32_t src_ref_crc32 = pobj->crc32;
	uint32_t src_cal_crc32 = 0, dest_cal_crc32 = 0;
	objectid_t saved_src_obj_id = pobj->obj_id;

	uint32_t dest_object_start_offset = dest_offset;

	/* Handle object metadata */
	/* Update Object iD for destination object */
	pobj->obj_id = new_objid;
	/* Keep CRC32 field uninitialized */
	pobj->crc32 = (uint32_t)~0;

#ifdef CONFIG_SECURE_PSM
	uint8_t saved_object_secure_bits = get_object_secure_bits(pobj);
	bool saved_is_object_secure = is_object_secure(pobj);
	if (part_encrypted(psm))
		set_object_secure(pobj);
	else
		clear_object_secure(pobj);
#endif /* CONFIG_SECURE_PSM */

	if (g_psm_event_cb)
		g_psm_event_cb(PSM_EVENT_COMPACTION,
			       (void *)PSM_COMPACTION_10,
			       (void *)PSM_COMPACTION_MAX);

	int rv = psm_write_flash(psm, pobj, sizeof(psm_object_t),
				 dest_offset, PSM_WRITE_NO_VERIFY);
	if (rv != WM_SUCCESS) {
		psm_d("%s: metadata write failed", __func__);
		return rv;
	}

	dest_offset += sizeof(psm_object_t);
	source_offset += sizeof(psm_object_t);

	/* Handle object name & data */
	uint32_t copy_size = pobj->name_len + pobj->data_len;

	uint8_t buf[READ_BUFFER_SIZE];
	uint32_t index;
	uint32_t read_size;

#ifdef CONFIG_SECURE_PSM
	/* Encryption/decryption sequence is:
	 * 1. Reset AES engine (resetkey())
	 * 2. Encrypt/decrypt name field in chunks
	 * 3. Encrypt/decrypt data field in chunks
	 */
	if (part_encrypted(psm) && !saved_is_object_secure) {
		/* Reset the AES engine */
		rv = resetkey(psm, SET_ENCRYPTION_KEY);
		if (rv != WM_SUCCESS) {
			psm_d("upgrade: reset enc key fail");
			return rv;
		}
	} else if (!part_encrypted(psm) && saved_is_object_secure) {
		/* Reset the AES engine */
		rv = resetkey(psm, SET_DECRYPTION_KEY);
		if (rv != WM_SUCCESS) {
			psm_d("upgrade: reset dec key fail");
			return rv;
		}
	}
#endif /* CONFIG_SECURE_PSM */

	for (index = 0; index < copy_size; index += read_size) {
		uint32_t remaining = copy_size - index;
		read_size = remaining > READ_BUFFER_SIZE ?
			READ_BUFFER_SIZE : remaining;

		int rv = psm_read_flash(psm, buf, read_size, source_offset);
		if (rv != WM_SUCCESS) {
			psm_d("%s: read failed", __func__);
			return rv;
		}

		src_cal_crc32 = soft_crc32(buf, read_size, src_cal_crc32);

#ifdef CONFIG_SECURE_PSM
		if (part_encrypted(psm) && !saved_is_object_secure) {
			/* Encrypt the object chunk */
			rv = encrypt_buf(psm, buf, buf, read_size);
			if (rv != WM_SUCCESS) {
				psm_d("upgrade: enc fail");
				return rv;
			}
		} else if (!part_encrypted(psm) && saved_is_object_secure) {
			/* Decrypt the object chunk */
			rv = decrypt_buf(psm, buf, buf, read_size);
			if (rv != WM_SUCCESS) {
				psm_d("upgrade: dec fail");
				return rv;
			}
		}
#endif /* CONFIG_SECURE_PSM */

		rv = psm_write_flash(psm, buf, read_size, dest_offset,
				     PSM_WRITE_NO_VERIFY);
		if (rv != WM_SUCCESS) {
			psm_d("%s: write failed", __func__);
			return rv;
		}

		dest_cal_crc32 = soft_crc32(buf, read_size, dest_cal_crc32);

		source_offset += read_size;
		dest_offset += read_size;
	}

	uint32_t dest_crc32 = psm_calc_metadata_crc32(pobj, dest_cal_crc32);

	/* Restore obj_id to calculate CRC32 */
	pobj->obj_id = saved_src_obj_id;
#ifdef CONFIG_SECURE_PSM
	/* Restore secure flag to calculate CRC32 */
	set_object_secure_bits(pobj, saved_object_secure_bits);
#endif /* CONFIG_SECURE_PSM */

	uint32_t src_crc32 = psm_calc_metadata_crc32(pobj, src_cal_crc32);

	psm_ed("src_ref_crc32: 0x%x src_crc32: 0x%x dest_crc32: 0x%x",
	       src_ref_crc32, src_crc32, dest_crc32);
	/*
	 * Check is source object CRC32 was correct. We are doing this
	 * late to avoid two passes over source object data.
	 */
	if (src_crc32 == src_ref_crc32) {
		/*
		 * Write new CRC32. We have to update CRC32 because we have
		 * programmed new obj-id.
		 */
		uint32_t crc32_offset = dest_object_start_offset +
			offsetof(psm_object_t, crc32);
		rv = psm_write_flash(psm, &dest_crc32, sizeof(dest_crc32),
				     crc32_offset, PSM_WRITE_NO_VERIFY);
		if (rv != WM_SUCCESS) {
			psm_d("Failed to update CRC32 of copied object");
			return rv;
		}

		return WM_SUCCESS;
	}

	/*
	 * Mark inactive just upgraded object because CRC calculation
	 * on source object failed. This effectively deletes the object from the PSM.
	 */
	psm_d("Object: 0x%x has failed CRC during upgrade. Marking inactive",
	      pobj->obj_id);
	uint32_t flag_offset = dest_object_start_offset +
		offsetof(psm_object_t, flags);
	rv = write_flag_bit(psm, flag_offset, PSM_OBJ_INACTIVE);
	if (rv != WM_SUCCESS) {
		psm_d("Failed to mark copied object inactive");
		return rv;
	}

	return WM_SUCCESS;
}

static int psm_copy_all_objects(psm_t *psm,
				checkpoint_t checkpoint,
				uint32_t source_offset,
				uint32_t source_limit,
				uint32_t dest_offset, uint32_t dest_limit)
{
	psm_entry_i();

	psm_object_t pobj;
	objectid_t objid_counter = 0;

	while (source_offset < source_limit) {
		if (objid_counter >= PSM_MAX_OBJECTS) {
			psm_d("Unexpected error during copy all objs");
			return -WM_FAIL;
		}

		int rv = psm_read_flash(psm, &pobj, sizeof(psm_object_t),
					source_offset);
		if (rv != WM_SUCCESS) {
			psm_d("%s: could not read object @ : %x",
			      __func__, source_offset);
			return -WM_FAIL;
		}

		if (psm_is_free_space(&pobj))
			break;

		if (!psm_is_data_len_sane(psm, pobj.data_len)) {
			psm_d("%s: Invalid data length: %d",
			      __func__, pobj.data_len);
			return -WM_FAIL;
		}

		if (pobj.object_type != OBJECT_TYPE_SMALL_BINARY) {
			psm_d("%s: Invalid obj at the end of data area. "
			      "Ignoring", __func__);
			break;
		}

		if (!is_object_active(&pobj)) {
			psm_ed("Inactive object with id: %x size: %d",
			       pobj.obj_id, pobj.data_len);
			source_offset += object_size(&pobj);
			continue;
		}

		if (checkpoint == CHECKPOINT_1) {
			/*
			 * This is operation from data area to swap area.
			 * We upgrade data here to desired psm format,
			 * instead of simple copy. If no upgrade is required,
			 * following will do a simple copy
			 */
			rv = psm_upgrade_object(psm, &pobj, source_offset,
					     dest_offset, ++objid_counter);
		} else if (checkpoint == CHECKPOINT_2) {
			/*
			 * This is operation from swap area to data area.
			 * We simply copy data here
			 */
			rv = psm_copy_object(psm, &pobj, source_offset,
					     dest_offset, ++objid_counter);
		} else {
			rv = -WM_FAIL;
			psm_d("Invalid checkpoint during compaction/upgrade");
		}

		if (rv != WM_SUCCESS) {
			psm_d("compaction/upgrade failed");
			return rv;
		}

		source_offset += object_size(&pobj);
		dest_offset += object_size(&pobj);

		if (dest_offset >= dest_limit) {
			/*
			 * This case should never occur because we always
			 * keep tab of total size of active objects.
			 */
			psm_d("%s: no space left in destination to copy",
			      __func__);
			return -WM_FAIL;
		}
	}

	return WM_SUCCESS;
}

static uint32_t get_erase_start_offset(psm_t *psm)
{
	uint32_t erase_start;
	/*
	 * We have no idea about start of swap address. So erase all non
	 * data area.
	 */
	if (!is_offset_block_aligned(psm, psm->data_free_offset)) {
		erase_start = align_to_block_offset(psm, psm->data_free_offset);
		erase_start += block_size(psm);
		/* 'erase_start' is now block aligned */
	} else {
		erase_start = psm->data_free_offset;
	}

	return erase_start;
}

static int handle_recovery(psm_t *psm, checkpoint_t checkpoint,
			   psm_swap_desc_t *swap_desc)
{
	psm_entry_i("Checkpoint: %d", checkpoint);
	int rv;
	bool swap_erase_done = false;
	bool data_erase_done = false;

	switch (checkpoint) {
	case CHECKPOINT_0:{
		if (g_psm_event_cb)
			g_psm_event_cb(PSM_EVENT_COMPACTION,
				       (void *)PSM_COMPACTION_1,
				       (void *)PSM_COMPACTION_MAX);
		/* Swap needs to be erased */
		uint32_t swap_start_offset =
			psm_get_swap_start_offset(psm);
		uint32_t swap_size = part_size(psm) - swap_start_offset;
		rv = psm_erase_flash(psm, swap_start_offset, swap_size);
		if (rv != WM_SUCCESS) {
			psm_d("CHK-0: Cannot erase swap area");
			return -WM_FAIL;
		}

		if (g_psm_event_cb)
			g_psm_event_cb(PSM_EVENT_COMPACTION,
				       (void *)PSM_COMPACTION_2,
				       (void *)PSM_COMPACTION_MAX);
		swap_erase_done = true;
		memset(swap_desc, 0xFF, sizeof(psm_swap_desc_t));
		swap_desc->swap_start_offset = swap_start_offset;
		swap_desc->swap_start_offset_inv = ~swap_start_offset;
		/* Leave checkpoint marker uninit'ed */
		rv = write_swap_descriptor(psm, swap_desc, true);
		if (rv != WM_SUCCESS) {
			psm_d("CHK-0: Cannot write swap descriptor");
			return -WM_FAIL;
		}
		if (g_psm_event_cb)
			g_psm_event_cb(PSM_EVENT_COMPACTION,
				       (void *)PSM_COMPACTION_3,
				       (void *)PSM_COMPACTION_MAX);
		checkpoint = CHECKPOINT_1;
		psm_d("Swap des successfully written. Crossing CHK-1");
	}
		/* Fall through */
	case CHECKPOINT_1:{
		if (!swap_erase_done) {
			psm_d("Direct jump to chkpt 1 not supported");
			return -WM_FAIL;
		}

		uint32_t data_offset = 0;
		uint32_t data_limit = psm->data_free_offset;

		uint32_t swap_data_offset =
			swap_desc->swap_start_offset +
			PSM_DATA_SWAP_DEMARCATION;
		uint32_t swap_limit =
			part_size(psm) - sizeof(psm_swap_desc_t);

		rv = psm_copy_all_objects(psm, checkpoint,
					  data_offset, data_limit,
					  swap_data_offset, swap_limit);
		if (rv != WM_SUCCESS) {
			psm_d("CHK-1: copy/upgrade to swap failed");
			return rv;
		}
		if (g_psm_event_cb)
			g_psm_event_cb(PSM_EVENT_COMPACTION,
				       (void *)PSM_COMPACTION_5,
				       (void *)PSM_COMPACTION_MAX);

		swap_desc->checkpoint_marker = CHECKPOINT_MARKER;
		swap_desc->checkpoint_marker_inv = ~CHECKPOINT_MARKER;
		rv = write_swap_descriptor(psm, swap_desc, false);
		if (rv != WM_SUCCESS) {
			psm_d("CHK-1: Can't write swap desc marker");
			return -WM_FAIL;
		}
		if (g_psm_event_cb)
			g_psm_event_cb(PSM_EVENT_COMPACTION,
				       (void *)PSM_COMPACTION_6,
				       (void *)PSM_COMPACTION_MAX);
		checkpoint = CHECKPOINT_2;
		psm_d("Swap now has all objects. Crossing CHK-2");
	}
		/* Fall through */
	case CHECKPOINT_2:{
		/*
		 * Swap descriptor is fully valid and data area
		 * state is unknown. In this state compaction
		 * process will have to  continue irrespective of
		 * caller request.
		 */
		rv = psm_erase_flash(psm, 0,
				     swap_desc->swap_start_offset);
		if (rv != WM_SUCCESS) {
			psm_d("Failed to erase data section");
			return rv;
		}
		if (g_psm_event_cb)
			g_psm_event_cb(PSM_EVENT_COMPACTION,
				       (void *)PSM_COMPACTION_7,
				       (void *)PSM_COMPACTION_MAX);
		data_erase_done = true;

		uint32_t swap_data_offset =
			swap_desc->swap_start_offset +
			PSM_DATA_SWAP_DEMARCATION;
		uint32_t swap_limit =
			part_size(psm) - sizeof(psm_swap_desc_t);

		uint32_t data_offset = 0;
		uint32_t data_limit = swap_desc->swap_start_offset;

		rv = psm_copy_all_objects(psm, checkpoint,
					  swap_data_offset, swap_limit,
					  data_offset, data_limit);
		if (rv != WM_SUCCESS) {
			psm_d("CHK-2: swap copy to data failed");
			return rv;
		}
		if (g_psm_event_cb)
			g_psm_event_cb(PSM_EVENT_COMPACTION,
				       (void *)PSM_COMPACTION_8,
				       (void *)PSM_COMPACTION_MAX);
		rv = invalidate_swap_descriptor(psm);
		if (rv != WM_SUCCESS) {
			psm_d("CHK-2: swap invalidation failed");
			return rv;
		}
		if (g_psm_event_cb)
			g_psm_event_cb(PSM_EVENT_COMPACTION,
				       (void *)PSM_COMPACTION_9,
				       (void *)PSM_COMPACTION_MAX);
		checkpoint = CHECKPOINT_3;
		psm_d("All objs copied to data area. Crossing CHK-3");
	}
		/* Fall through */
	case CHECKPOINT_3:{
		/*
		 * Swap descriptor is invalid. Cleanup the swap area.
		 */
		uint32_t erase_start;
		if (!data_erase_done) {
			erase_start = get_erase_start_offset(psm);
		} else {
			/* Fall through case */
			erase_start = swap_desc->swap_start_offset;
		}

		rv = psm_erase_flash(psm, erase_start,
				     part_size(psm) - erase_start);
		if (rv != WM_SUCCESS) {
			psm_d("CHK-3 erase failed");
			return -WM_FAIL;
		}
		if (g_psm_event_cb)
			g_psm_event_cb(PSM_EVENT_COMPACTION,
				       (void *)PSM_COMPACTION_10,
				       (void *)PSM_COMPACTION_MAX);
		checkpoint = CHECKPOINT_4;
		psm_d("Swap area is now erased. Crossing CHK-4");
	}
		break;
	case CHECKPOINT_4:
		/* No-op */
		break;
	default:
		psm_d("Unknown checkpoint: %d", checkpoint);
		return -WM_FAIL;
		break;
	}

	return WM_SUCCESS;
}

/*
 * 1: Match
 * 0: No match
 * < 0: Error
 *
 * 'pobj' is passed because this function should only read name length
 * equal to actually present in flash.
 */
static int read_and_match_name(psm_t *psm, const psm_object_t *pobj,
			       const char *name, uint32_t offset)
{
	/* We have ensured that user given and flash name is same in length */
	char buf[READ_BUFFER_SIZE];

	uint32_t read_size, index;
	for (index = 0; index < pobj->name_len; index += read_size) {
		uint32_t remaining = pobj->name_len - index;
		read_size = remaining > READ_BUFFER_SIZE ?
			READ_BUFFER_SIZE : remaining;
		int rv = psm_read_flash(psm, buf, read_size, offset + index);
		if (rv != WM_SUCCESS) {
			psm_d("Failed to read name data @ off: %x", offset);
			return rv;
		}

		if (memcmp(&name[index], buf, read_size))
			return 0;
	}

	return 1;
}

/*
 * Returns:
 * 2  : Object found and matched.
 * 1  : An object found but no match.
 * 0  : No PSM object found.
 * < 0: Error
 */
static int read_and_match_object(psm_t *psm, psm_object_t *pobj,
				 objectid_t *obj_id,
				 const char *name, uint32_t name_len,
				 uint32_t offset, uint32_t limit)
{
	int rv = psm_read_flash(psm, pobj, sizeof(psm_object_t), offset);
	if (rv != WM_SUCCESS) {
		psm_d("could not read object @ off: %x", offset);
		return rv;
	}

	if (psm_is_free_space(pobj))
		return 0;

	if (pobj->object_type != OBJECT_TYPE_SMALL_BINARY) {
		psm_d("Object type not valid @ off %x", offset);
		return 0;
	}

	/* An object is definately present */
	if (*obj_id == PSM_INVALID_OBJ_ID) {
		/* No mapping has been done yet */
		if (pobj->name_len == 0) {
			psm_d("Object with name length 0 is not supported");
			return -WM_FAIL;
		}

		/* Need to use name to match */
		if (name_len != pobj->name_len)
			return 1;

		rv = read_and_match_name(psm, pobj, name,
					 offset + sizeof(psm_object_t));
		if (rv < 0)
			return rv;

		if (rv == 0)
			return 1;

		/* Name has matched */
		*obj_id = pobj->obj_id;
	} else {
		/* A name to obj_id was already done before */
		if (*obj_id != pobj->obj_id)
			return 1;
	}

	if (is_object_active(pobj)) {
		psm_d("Object active and matched");
		return 2;
	}

	return 1;
}

/*
 * 'pobj' will be populated only if found and cache was not present.
 */
static psm_index_t *psm_search_index(psm_t *psm, const char *name,
				     uint32_t name_len, psm_object_t *pobj)
{
	psm_entry_i("name: %s", name);
	psm_index_t *index = NULL;
	uint32_t name_hash = soft_crc32(name, name_len, 0);
	int cnt;
	for (cnt = 0; cnt < psm->index_elements; cnt++) {
		index = &psm->index[cnt];
		psm_i("Check: obj_id: %x H: %x: %x", index->obj_id,
		      name_hash, index->name_hash);
		if (name_hash != index->name_hash)
			continue;

		psm_i("Hash match: obj_id: %x", index->obj_id);
		/*
		 * Name_Hash has matched but there could be a collision. We
		 * need to probe more. If a cache is present the task is
		 * simpler. If not we will have to read the name field from
		 * the object stored in flash.
		 */
		if (index->cache) {
			psm_i("Cache available: obj_id: %x", index->obj_id);
			/* Cache is present */
			psm_cache_t *cache = index->cache;
			if (name_len != cache->name_len)
				continue;

			psm_i("Cache: check name match: %s", name);
			uint8_t *name_buf = (uint8_t *) (cache + 1);
			if (memcmp(name, name_buf, name_len))
				continue;	/* No match */

			psm_i("Cache: name matched");
			/*
			 * Name has matched. Since cache is there flash
			 * is not supposed to be accessed and thus 'pobj'
			 * will not be populated. Caller to should care.
			 */
			return index;
		}

		psm_i("Index: Check name match by accessing flash object");
		/* No cache is present. Need to access flash object */
		int rv = psm_read_flash(psm, pobj, sizeof(psm_object_t),
					index->flash_offset);
		if (rv != WM_SUCCESS) {
			psm_d("%s: Unable to read flash", __func__);
			break;
		}

		if (name_len != pobj->name_len) {
			/* Mismatch */
			psm_i("Name hash matched but name len mismatch");
			continue;
		}

		rv = read_and_match_name(psm, pobj, name,
					 index->flash_offset +
					 sizeof(psm_object_t));
		if (rv < 0) {
			psm_i("Error during name match: %d", rv);
			break;
		}

		if (rv == 1) {
			psm_i("Object %d found in index", pobj->obj_id);
			/* A match is found */
			return index;
		}

		/* Mismatch */
		psm_i("Name hash matched but name mismatch");
	}

	return NULL;
}

static void psm_remove_index_entry(psm_t *psm, psm_index_t *index)
{
	psm_i("Removing object: %d from index", index->obj_id);

	if (index->cache) {
		psm_i("Freeing object %d cache", index->obj_id);
		os_mem_free(index->cache);
	}

	uint32_t cnt = index - psm->index;
	if (cnt == (psm->index_elements - 1)) {
		/* Last entry is to be deleted */
		psm->index_elements--;
	} else {
		uint32_t later_index_count = psm->index_elements - cnt - 1;
		memmove(index, index + 1,
			(sizeof(psm_index_t) * later_index_count));
		psm->index_elements--;
	}
}

static int search_active_object(psm_t *psm, psm_object_t *pobj,
				const char *name, uint32_t name_len,
				uint32_t *flash_offset, bool *found,
				objectid_t *obj_id, psm_index_t **index)
{
	psm_entry_i("%s", name);

	*index = NULL;
	if (psm->index) {
		*index = psm_search_index(psm, name, name_len, pobj);
		if (*index != NULL) {
			/* Object found in index */
			*flash_offset = (*index)->flash_offset;
			*found = true;

			/*
			 * Load pobj with some values cached. This is tied
			 * to implementation of function psm_object_open()
			 * and keep this always in sync with that
			 * implementation.
			 */
			pobj->obj_id = (*index)->obj_id;
			if ((*index)->cache) {
				pobj->data_len = (*index)->cache->data_len;
				pobj->name_len = (*index)->cache->name_len;
			}

			psm_ed("Index matched for %s", name);
			return WM_SUCCESS;
		}

		psm_i("Index miss");
	}

	uint32_t offset = 0;
	/*
	 * Don't try reading flash if remaining size is less than
	 * necessary.
	 */
	uint32_t limit = psm->object_offset_limit;

	/* Set default not found */
	*found = false;

	while (offset < limit) {
		int rv = read_and_match_object(psm, pobj,
					       obj_id, name, name_len,
					       offset, limit);
		if (rv < 0) {
			psm_d("Failure during object search");
			return rv;
		}

		if (rv == 0) {
			psm_d("No more PSM objects");
			return WM_SUCCESS;
		}

		if (rv == 2) {
			/* Object found */
			*flash_offset = offset;
			*found = true;
			return WM_SUCCESS;
		}

		offset += object_size(pobj);
	}

	return WM_SUCCESS;
}

static void psm_init_new_object(psm_t *psm, psm_objinfo_t *objinfo,
				objectid_t obj_id, uint32_t name_len,
				psm_objattrib_t *attribs, psm_mode_t mode,
				uint32_t max_data_len)
{
	psm_entry_i("New obj: id: 0x%x", obj_id);
	/* Re-use the object structure */
	memset(objinfo, 0x00, sizeof(psm_objinfo_t));
	/* Init all 0xFF as per flash properties */
	memset(&objinfo->pobj, 0xFF, sizeof(psm_object_t));

	objinfo->psm = psm;
	objinfo->mode = mode;
	objinfo->max_data_len = max_data_len;

	objinfo->pobj.object_type = OBJECT_TYPE_SMALL_BINARY;
	if (attribs) {
		if (attribs->flags & PSM_INDEXING_ENABLED)
			objinfo->pobj.flags &= ~PSM_OBJ_INDEX_REQ;
		if (attribs->flags & PSM_CACHING_ENABLED)
			objinfo->pobj.flags &= ~PSM_OBJ_CACHE_REQ;
	}

#ifdef CONFIG_SECURE_PSM
	if (part_encrypted(psm))
		set_object_secure(&(objinfo->pobj));
	else
		clear_object_secure(&(objinfo->pobj));
#endif /* CONFIG_SECURE_PSM */

	/* Keep data length uninitialized i.e. all bytes to 0xFF */
	objinfo->pobj.name_len = name_len;
	objinfo->pobj.obj_id = obj_id;
}

static int manage_space_constraints(psm_t *psm, uint32_t new_obj_size,
				bool *compaction_reqd)
{
	psm_entry_i("new_obj_size: %d", new_obj_size);

	if (compaction_reqd)
		*compaction_reqd = false;

	if (!psm_is_data_len_sane(psm, new_obj_size)) {
		psm_d("data length invalid for new object: %u", new_obj_size);
		return -WM_E_INVAL;
	}

	uint32_t proposed_active_objects_size =
		psm->total_active_object_size + new_obj_size;
	uint32_t proposed_swap_start_offset =
		align_to_block_offset(psm, (part_size(psm) -
					    sizeof(psm_swap_desc_t) -
					    PSM_DATA_SWAP_DEMARCATION -
					    proposed_active_objects_size));
	uint32_t proposed_data_free_offset =
		psm->data_free_offset + new_obj_size;

	if (proposed_data_free_offset < proposed_swap_start_offset)
		return WM_SUCCESS;

	/*
	 * If we allow write of this object _no_ space will be left
	 * for swap handling. Check if compaction will help
	 */
	if (proposed_active_objects_size > proposed_swap_start_offset) {
		/* Compaction will also _not_ help */
		psm_d("Compaction will not help");
		return -WM_E_NOSPC;
	}

	/* Compaction will help */
	if (compaction_reqd) {
		*compaction_reqd = true;
		return WM_SUCCESS;
	}

	/* Return error as we could not signal compaction reqd status */
	return -WM_FAIL;
}

static int psm_part_compact(psm_t *psm)
{
	psm_d("Compaction needs to be done to accomodate");
	psm_swap_desc_t swap_desc;
	int rv = handle_recovery(psm, CHECKPOINT_0, &swap_desc);
	if (rv != WM_SUCCESS) {
		psm_d("%s: compaction failed", __func__);
		return rv;
	}

	/* Need to scan again as flash layout has changed */
	rv = psm_part_scan(psm);
	if (rv != WM_SUCCESS) {
		psm_d("%s: Could not scan", __func__);
		return rv;
	}

	return WM_SUCCESS;
}

static int _psm_object_open(psm_t *psm, psm_objinfo_t *objinfo,
			    const char *name, psm_mode_t mode,
			    uint32_t max_data_len, psm_objattrib_t *attribs)
{
	psm_entry_i("name: %s mode: %d max_data_len = %d",
		    name, mode, max_data_len);

	uint32_t name_len = strnlen(name, PSM_MAX_OBJNAME_LEN + 1);
	if (!name_len || name_len > PSM_MAX_OBJNAME_LEN) {
		psm_d("name length invalid: %d", name_len);
		return -WM_E_INVAL;
	}

	int rv;
	if (mode == PSM_MODE_WRITE) {
		if (part_readonly(psm))
			return -WM_E_PERM;

		bool compaction_reqd;
		rv = manage_space_constraints(psm, sizeof(psm_object_t) +
					      name_len + max_data_len,
					      &compaction_reqd);
		if (rv != WM_SUCCESS) {
			psm_d("Unable to alloc space for new obj size: %u",
			      max_data_len);
			return rv;
		}

		if (compaction_reqd) {
			rv = psm_part_compact(psm);
			if (rv != WM_SUCCESS) {
				psm_d("Compaction to alloc space for new "
				      "obj size: %u failed", max_data_len);
				return rv;
			}
		}
	}

	memset(objinfo, 0x00, sizeof(psm_objinfo_t));

	/* The object may be already be present. */
	bool found;
	uint32_t flash_offset;
	psm_index_t *index = NULL;
	objectid_t obj_id = PSM_INVALID_OBJ_ID;
	const void *name_to_search = name;

#ifdef CONFIG_SECURE_PSM
	if (part_encrypted(psm)) {
		rv = resetkey(psm, SET_ENCRYPTION_KEY);
		if (rv != WM_SUCCESS) {
			psm_d("reset enc key (name) fail");
			return rv;
		}

		rv = encrypt_buf(psm, name, psm->psm_name_enc_buf, name_len);
		if (rv != WM_SUCCESS) {
			psm_d("enc name fail");
			return rv;
		}

		/* Override 'name_to_search' */
		name_to_search = psm->psm_name_enc_buf;
	}
#endif /* CONFIG_SECURE_PSM */

	rv = search_active_object(psm, &objinfo->pobj, name_to_search, name_len,
				  &flash_offset, &found, &obj_id, &index);
	if (rv != WM_SUCCESS) {
		psm_d("%s: Failed: %d", __func__, rv);
		return rv;
	}

	objinfo->object_flash_offset = flash_offset;
	objinfo->curr_flash_offset = flash_offset;
	if (mode == PSM_MODE_READ) {
		if (!found) {
			psm_d("Object: %s not found for reading", name);
			memset(objinfo, 0x00, sizeof(psm_objinfo_t));
			return -WM_FAIL;
		}

		objinfo->psm = psm;
		/* Point to start of data */
		objinfo->curr_flash_offset += metadata_size(&objinfo->pobj);

		/* Since name has already matched calculate CRC32 using
		   user name buffer */
		objinfo->crc32 = soft_crc32(name_to_search, name_len, 0);
		objinfo->mode = mode;
		objinfo->index = index;

#ifdef CONFIG_SECURE_PSM
		if (part_encrypted(psm)) {
			/*
			 * A Dummy decryption is needed. Since "name +
			 * data" are  encrypted in that sequence the
			 * decryption also need to happen in that
			 * sequence. Since we have already encrypted the
			 * name field we can decrypt it here.
			 */
			rv = resetkey(psm, SET_DECRYPTION_KEY);
			if (rv)
				return rv;

			/* The 'psm_name_enc_buf' will not be used after
			   this. Re-use that buffer.
			*/
			rv = decrypt_buf(psm, psm->psm_name_enc_buf,
					 psm->psm_name_enc_buf, name_len);
			if (rv)
				return rv;
		}
#endif /* CONFIG_SECURE_PSM */

		return WM_SUCCESS;
	}

	objectid_t old_obj_id;
	uint32_t old_object_flash_offset;
	if (found) {
		psm_d("Existing object %s is found.", name);
		/* Cache the needed values before re-using object */
		old_obj_id = objinfo->pobj.obj_id;
		old_object_flash_offset = objinfo->object_flash_offset;
		uint32_t old_object_size = object_size(&objinfo->pobj);
		psm_init_new_object(psm, objinfo, old_obj_id, name_len,
				    attribs, mode, max_data_len);
		objinfo->old_active_object_present = true;
		objinfo->old_object_flash_offset = old_object_flash_offset;
		objinfo->old_object_size = old_object_size;
	} else {
		if (!psm_is_objid_available(psm)) {
			/* No space */
			psm_d("No space for new objects. No more object ID's");
			return -WM_E_NOSPC;
		}

		/*
		 * It is possible that active object is not found but an
		 * inactive object was present.
		 */
		objectid_t new_obj_id = (obj_id == PSM_INVALID_OBJ_ID) ?
			psm_get_unused_obj_id(psm) : obj_id;
		/* Brand new object */
		psm_init_new_object(psm, objinfo, new_obj_id, name_len,
				    attribs, mode, max_data_len);
	}

	objinfo->object_flash_offset = psm->data_free_offset;
	objinfo->curr_flash_offset = psm->data_free_offset;

	/* Write the metadata structure. Keep data length as undefined
	   for now */
	psm_d("Writing metadata");
	rv = psm_write_flash(psm, &objinfo->pobj, sizeof(psm_object_t),
			     objinfo->curr_flash_offset, PSM_WRITE_NO_VERIFY);
	if (rv != WM_SUCCESS) {
		psm_d("%s: metadata write failed.", __func__);
		return rv;
	}

	/* Initialize length to 0 after object has been written */
	objinfo->pobj.data_len = 0;

	objinfo->curr_flash_offset += sizeof(psm_object_t);

	/* Write name field */
	psm_d("Writing name field");

	rv = psm_write_flash(psm, name_to_search, name_len,
			     objinfo->curr_flash_offset, PSM_WRITE_NO_VERIFY);
	if (rv != WM_SUCCESS) {
		psm_d("%s: metadata write failed.", __func__);
		return rv;
	}

	/* Start CRC calculation with name field */
	objinfo->pobj.crc32 = soft_crc32(name_to_search, name_len, 0);

	objinfo->curr_flash_offset += name_len;

	if (index)
		psm_remove_index_entry(psm, index);
	return WM_SUCCESS;
}

static int _psm_object_write(psm_t *psm, psm_objinfo_t *objinfo,
			     const uint8_t *data, uint32_t len)
{
	psm_object_t *pobj = &objinfo->pobj;
	psm_entry_i("id: %x len: %d", pobj->obj_id, len);
	int rv;

	if (len == 0) {
		psm_d("Ignoring zero length write");
		return WM_SUCCESS;
	}

	uint32_t max_allowed_len = objinfo->max_data_len - pobj->data_len;

	if (max_allowed_len < len) {
		psm_d("Write failed. Write more than requested: %d: %d",
		      max_allowed_len, len);
		return -WM_E_NOSPC;
	}

#ifdef CONFIG_SECURE_PSM
	if (part_encrypted(psm)) {
		int data_remaining = len;
		int offset = 0;
		while (data_remaining) {
			int enc_len = data_remaining > READ_BUFFER_SIZE ?
				READ_BUFFER_SIZE : data_remaining;

			rv = encrypt_buf(psm, data + offset, psm->psm_enc_buf,
					 enc_len);
			if (rv) {
				psm_d("%s: encryption failed", __func__);
				return rv;
			}

			rv = psm_write_flash(psm, psm->psm_enc_buf, enc_len,
					     objinfo->curr_flash_offset,
					     PSM_WRITE_NO_VERIFY);
			pobj->crc32 = soft_crc32(psm->psm_enc_buf, enc_len,
						 pobj->crc32);
			objinfo->curr_flash_offset += enc_len;

			offset += enc_len;
			data_remaining -= enc_len;
		}

		pobj->data_len += len;
		psm_d("Wrote %d bytes of data", len);
		return WM_SUCCESS;
	}
#endif
	rv = psm_write_flash(psm, data, len, objinfo->curr_flash_offset,
			     PSM_WRITE_NO_VERIFY);
	if (rv != WM_SUCCESS) {
		psm_d("%s: Write failed", __func__);
		return rv;
	}

	/* Update CRC32 */
	pobj->crc32 = soft_crc32(data, len, pobj->crc32);

	objinfo->curr_flash_offset += len;
	pobj->data_len += len;

	psm_d("Wrote %d bytes of data", len);
	return WM_SUCCESS;
}

static int _psm_object_read(psm_t *psm, psm_objinfo_t *objinfo,
			    uint8_t *buf, uint32_t max_len)
{
	psm_object_t *pobj = &objinfo->pobj;
	psm_entry_i("obj-id: %x mode: %d max_len: %d",
		    pobj->obj_id, objinfo->mode, max_len);
	int rv;

	if (!max_len) {
		psm_d("Ignoring zero length read");
		return WM_SUCCESS;
	}

	const uint32_t data_start_offset = objinfo->object_flash_offset +
		metadata_size(pobj);
	uint32_t remaining = pobj->data_len - (objinfo->curr_flash_offset -
					       data_start_offset);
	if (remaining == 0) {
		psm_d("no data to read");
		return 0;
	}

	uint32_t read_size = remaining > max_len ? max_len : remaining;

	if (objinfo->index && objinfo->index->cache) {
		psm_cache_t *cache = objinfo->index->cache;
		uint8_t *name_buf = (uint8_t *) (cache + 1);
		uint8_t *data_buf = name_buf + cache->name_len;

		uint32_t read_offset = pobj->data_len - remaining;
		memcpy(buf, &data_buf[read_offset], read_size);
		goto skip_actual_flash_read;
	}

	rv = psm_read_flash(psm, buf, read_size, objinfo->curr_flash_offset);
	if (rv != WM_SUCCESS) {
		psm_d("%s: read failed size: %d", __func__, read_size);
		return rv;
	}

	objinfo->crc32 = soft_crc32(buf, read_size, objinfo->crc32);
	/* Check if this is the last read */
	if (remaining == read_size) {
		/*
		 * Since this is last read, CRC32 on-the-fly calculation is
		 * now done. Verification can be done now
		 */
		objinfo->crc32 = psm_calc_metadata_crc32(&objinfo->pobj,
							 objinfo->crc32);
		if (objinfo->crc32 != pobj->crc32) {
			psm_d("Invalid CRC32 during read detected "
			      "0x%x != 0x%x", objinfo->crc32, pobj->crc32);
			return -WM_E_CRC;
		}
	}

#ifdef CONFIG_SECURE_PSM
	/* If read data is encrypted, decrypt it before sending back. */
	if (part_encrypted(psm)) {
		rv = decrypt_buf(psm, buf, buf, read_size);
		if (rv)
			return rv;
	}
#endif /* CONFIG_SECURE_PSM */

 skip_actual_flash_read:
	objinfo->curr_flash_offset += read_size;
	return read_size;
}

static int _psm_object_close(psm_t *psm, psm_objinfo_t *objinfo)
{
	psm_entry_i("obj-id: %d mode: %d", objinfo->pobj.obj_id, objinfo->mode);
	if (objinfo->mode == PSM_MODE_READ) {
		psm_d("Object %d (opened in read mode) closed",
		      objinfo->pobj.obj_id);
		return WM_SUCCESS;
	}

	psm_object_t *pobj = &objinfo->pobj;
	/*
	 * CRC32 of name and data is already done. Calculate CRC32 of
	 * metadata.
	 */
	pobj->crc32 = psm_calc_metadata_crc32(pobj, pobj->crc32);

	psm_d("Closing object: length: %d Crc32: 0x%x", pobj->data_len,
	      pobj->crc32);
	/* Write the crc32 and data length fields to flash */
	uint32_t size = sizeof(pobj->crc32) + sizeof(pobj->data_len);
	uint32_t offset = objinfo->object_flash_offset +
		offsetof(psm_object_t, crc32);
	int rv = psm_write_flash(psm, &pobj->crc32, size, offset,
				 PSM_WRITE_NO_VERIFY);
	if (rv != WM_SUCCESS) {
		psm_d("Failed to update length and crc32 fields for "
		      "object: %d", pobj->obj_id);
		return rv;
	}

	psm->total_active_object_size += object_size(pobj);
	psm_d("Checking if need to make earlier object inactive");
	if (objinfo->old_active_object_present) {
		psm_d("Marking earlier object inactive");
		/* Need to make earlier object inactive */
		uint32_t flag_offset = objinfo->old_object_flash_offset +
			offsetof(psm_object_t, flags);
		rv = write_flag_bit(psm, flag_offset, PSM_OBJ_INACTIVE);
		if (rv != WM_SUCCESS) {
			psm_d("Failed to make earlier object inactive");
		}

		/* Earlier object size can be reduced */
		psm->total_active_object_size -= objinfo->old_object_size;
		objinfo->old_object_flash_offset = 0;
	}

	/* For benefit of next object */
	psm->data_free_offset += object_size(&objinfo->pobj);

	if (is_object_indexable(pobj))	/* Ignore any error */
		psm_add_to_index(psm, pobj, objinfo->object_flash_offset);
	return WM_SUCCESS;
}

int psm_object_read(psm_object_handle_t ohandle, void *buf, uint32_t max_len)
{
	psm_entry();
	if (!ohandle || !buf || !max_len) {
		psm_d("%s: Invalid arguments", __func__);
		return -WM_E_INVAL;
	}

	psm_objinfo_t *objinfo = (psm_objinfo_t *) ohandle;
	psm_t *psm = objinfo->psm;
	return _psm_object_read(psm, objinfo, buf, max_len);
}

int psm_object_write(psm_object_handle_t ohandle, const void *buf, uint32_t len)
{
	psm_entry();
	if (!ohandle || !buf || !len) {
		psm_d("%s: Invalid arguments", __func__);
		return -WM_E_INVAL;
	}

	psm_objinfo_t *objinfo = (psm_objinfo_t *) ohandle;
	psm_t *psm = objinfo->psm;
	if (objinfo->mode != PSM_MODE_WRITE) {
		psm_d("Cannot write in read mode");
		return -WM_E_PERM;
	}

	return _psm_object_write(psm, objinfo, buf, len);
}

int psm_object_open(psm_hnd_t phandle, const char *name,
		    psm_mode_t mode, uint32_t max_data_len,
		    psm_objattrib_t *attribs, psm_object_handle_t *ohandle)
{
	psm_entry("name: %s mode: %d", name, mode);
	if (!phandle || !name || !ohandle) {
		psm_d("%s: Invalid arguments", __func__);
		return -WM_E_INVAL;
	}

	*ohandle = 0;
	psm_t *psm = (psm_t *) phandle;

	if (mode == PSM_MODE_WRITE) {
		if (!psm_is_data_len_sane(psm, max_data_len)) {
			psm_d("Unsupported data length for new object: %u",
			      max_data_len);
			return -WM_E_INVAL;
		}
	}

	if (attribs)
		if (attribs->flags & PSM_CACHING_ENABLED)
			attribs->flags |= PSM_INDEXING_ENABLED;

	psm_rwlock_part(psm, mode);

	psm_objinfo_t *objinfo = os_mem_calloc(sizeof(psm_objinfo_t));
	if (!objinfo) {
		psm_rwunlock_part(psm, mode);
		psm_d("unable to alloc psm object");
		return -WM_E_NOMEM;
	}

	int rv = _psm_object_open(psm, objinfo, name, mode,
				  max_data_len, attribs);
	if (rv != WM_SUCCESS) {
		os_mem_free(objinfo);
		psm_rwunlock_part(psm, mode);
		return rv;
	}

	*ohandle = (psm_object_handle_t) objinfo;

	if (mode == PSM_MODE_WRITE)
		return WM_SUCCESS;
	else /* READ Mode */
		return objinfo->pobj.data_len;
}

static int _psm_object_delete(psm_t *psm, const char *name, uint32_t name_len)
{
	psm_entry_i();
	bool found;
	psm_object_t pobj;
	psm_index_t *index = NULL;
	uint32_t flash_offset;
	objectid_t obj_id = PSM_INVALID_OBJ_ID;
	const void *name_to_search = name;

#ifdef CONFIG_SECURE_PSM
	if (part_encrypted(psm)) {
		int rv = resetkey(psm, SET_ENCRYPTION_KEY);
		if (rv != WM_SUCCESS) {
			psm_d("d: reset enc key (name) fail");
			return rv;
		}

		rv = encrypt_buf(psm, name, psm->psm_name_enc_buf, name_len);
		if (rv != WM_SUCCESS) {
			psm_d("d: enc name fail");
			return rv;
		}

		/* Override 'name_to_search' */
		name_to_search = psm->psm_name_enc_buf;
	}
#endif /* CONFIG_SECURE_PSM */

	int rv = search_active_object(psm, &pobj, name_to_search, name_len,
				      &flash_offset, &found, &obj_id, &index);
	if (rv != WM_SUCCESS) {
		psm_d("%s: Failure during search: %d", __func__, rv);
		return rv;
	}

	if (!found) {
		psm_d("Object \"%s\" not found to delete", name);
		return WM_SUCCESS;
	}

	psm_d("Deleting object \"%s\" by making it inactive", name);
	/* Need to make earlier object inactive */
	uint32_t flag_offset = flash_offset + offsetof(psm_object_t, flags);
	rv = write_flag_bit(psm, flag_offset, PSM_OBJ_INACTIVE);
	if (rv != WM_SUCCESS) {
		psm_d("%s: Failed to make object inactive", __func__);
		return rv;
	}

	/* Reduce the size counter */
	psm->total_active_object_size -= object_size(&pobj);

	if (index)
		psm_remove_index_entry(psm, index);
	return rv;
}

int psm_object_delete(psm_hnd_t phandle, const char *name)
{
	psm_entry("name: %s", name);
	if (!phandle || !name) {
		psm_d("Invalid arguments");
		return -WM_E_INVAL;
	}

	uint32_t name_len = strnlen(name, PSM_MAX_OBJNAME_LEN + 1);
	if (!name_len || name_len > PSM_MAX_OBJNAME_LEN) {
		psm_d("%s: name length invalid: %d", __func__, name_len);
		return -WM_E_INVAL;
	}

	psm_t *psm = (psm_t *) phandle;
	psm_rwlock_part(psm, PSM_MODE_WRITE);
	int rv = _psm_object_delete(psm, name, name_len);
	psm_rwunlock_part(psm, PSM_MODE_WRITE);

	return rv;
}

int psm_object_close(psm_object_handle_t *ohandle)
{
	psm_entry();
	if (!ohandle) {
		psm_d("%s: Invalid argument", __func__);
		return -WM_E_INVAL;
	}

	psm_objinfo_t *objinfo = (psm_objinfo_t *) *ohandle;

	/* Save necessary variables before objinfo is freed */
	psm_t *psm = objinfo->psm;
	psm_mode_t mode = objinfo->mode;

	int rv = _psm_object_close(psm, objinfo);

	psm_rwunlock_part(psm, mode);

	memset(objinfo, 0x00, sizeof(psm_objinfo_t));
	os_mem_free(objinfo);

	*ohandle = 0;
	return rv;
}

static int _psm_init_verify_params(const flash_desc_t *fdesc,
				   uint32_t block_size,
				   psm_cfg_t *psm_cfg)
{
	if (!fdesc->fl_size) {
		psm_d("Zero sized partition given");
		return -WM_E_INVAL;
	}

	uint32_t block_cnt = fdesc->fl_size / block_size;
	/* With read/write capability atleast two blocks are required */
	uint32_t min_block_cnt_req = 2;
	if (psm_cfg && psm_cfg->read_only)
		min_block_cnt_req = 1;

	if (block_cnt < min_block_cnt_req) {
		psm_d("Atleast %d block(s) are required for PSM operation",
		      min_block_cnt_req);
		return -WM_E_PERM;
	}

	/* Basic sanity check */
	if (block_size < 64) {
		psm_d("Block size %d is too less\r\n", block_size);
		return -WM_E_INVAL;
	}

	return WM_SUCCESS;
}

static int deduce_checkpoint_at_init(psm_t *psm, psm_swap_desc_t *swap_desc,
				     checkpoint_t *checkpoint)
{
	psm_entry_i();

	int rv = read_swap_descriptor(psm, swap_desc);
	if (rv != WM_SUCCESS) {
		psm_d("Parititon check cannot start. Swap read failed");
		return rv;
	}

	/* Check if all are 0xFF */
	int index = 0;
	uint8_t *ptr = (uint8_t *) swap_desc;
	for (index = 0; index < sizeof(psm_swap_desc_t); index++)
		if (ptr[index] != 0xFF)
			break;

	if (index == sizeof(psm_swap_desc_t)) {
		/*
		 * Swap area is clean. This implies that no compaction was
		 * in progress.
		 */
		*checkpoint = CHECKPOINT_4;
		return WM_SUCCESS;
	}

	/*
	 * Swap has some data. If checkpoint marker is not valid it
	 * implies that compaction process did not complete after the last
	 * time it started. Set checkpoint such that only swap cleanup will
	 * be performed.
	 */
	if (swap_desc->checkpoint_marker ==
	    (uint16_t) ~swap_desc->checkpoint_marker_inv) {
		if (swap_desc->swap_start_offset ==
		    ~swap_desc->swap_start_offset_inv) {
			/*
			 * The descriptor is fully valid. This implies that
			 * compaction was interrupted after all objects
			 * were successfully copied to swap area. Now, we
			 * need to continue and copy them back to data area.
			 */
			*checkpoint = CHECKPOINT_2;
			return WM_SUCCESS;
		}

		/*
		 * This case is not expected because swap_start_offset is
		 * always written before checkpoint_marker. Lets erase the
		 * swap and cross our fingers.
		 */
		*checkpoint = CHECKPOINT_3;
		return WM_SUCCESS;
	}

	/*
	 * Swap descriptor is invalid. This implies that compaction process
	 * was incomplete. Data area is good. So only perform a cleanup of
	 * swap area.
	 */
	*checkpoint = CHECKPOINT_3;
	return WM_SUCCESS;
}

static int psm_module_physical_init(psm_t *psm)
{
	checkpoint_t checkpoint;
	psm_swap_desc_t swap_desc;

	int rv = deduce_checkpoint_at_init(psm, &swap_desc, &checkpoint);
	if (rv != WM_SUCCESS) {
		psm_d("swap state check failed");
		return rv;
	}

	psm_isc("Checkpoint: %d", checkpoint);
	if (checkpoint != CHECKPOINT_2) {
		/* Data area valid as per checkpoint. So, do a scan */
		rv = psm_part_scan(psm);
		if (rv != WM_SUCCESS) {
			/*
			 * Since partition scan has failed it implies that
			 * something is amiss in the data area. Running
			 * compaction/upgrade may salvage intact objects.
			 */
			psm_d("Partition scan failed. Will try compaction"
				"/upgrade");

			/* Check if space is available for compaction/upgrade */
			bool compaction_reqd;
			rv = manage_space_constraints(psm, 0, &compaction_reqd);
			if (rv != WM_SUCCESS) {
				psm_d("Unable to alloc space for swap");
				return rv;
			}

			/* Force checkpoint and compaction */
			checkpoint = CHECKPOINT_0;
		}
	}

	rv = handle_recovery(psm, checkpoint, &swap_desc);
	if (rv != WM_SUCCESS) {
		psm_d("unexpected error while compaction");
		return rv;
	}

	/* Scan is pending */
	if (checkpoint != CHECKPOINT_4) {
		rv = psm_part_scan(psm);
		if (rv != WM_SUCCESS) {
			psm_d("psm scan failed after restoring swap to data");
			return rv;
		}
	}

	return rv;
}

static psm_t *_get_existing_psm_object(const flash_desc_t *fdesc)
{
	if (part_list.next == NULL) {
		/* This is the first partition to be init'ed */
		INIT_LIST_HEAD(&part_list);
		return NULL;
	}

	psm_t *psm;
	list_for_each_entry_type(psm, psm_t, &part_list, node, struct list_head) {
		if (psm->fdesc_fl_dev == fdesc->fl_dev &&
		    psm->start_address == fdesc->fl_start) {
			/*
			 * There is a PSM already registered for this start
			 * address.
			 */
			psm_w("Using existing psm object");
			return psm;
		}
	}

	return NULL;
}

int psm_module_init(flash_desc_t *fdesc, psm_hnd_t *phandle,
		    psm_cfg_t *psm_cfg)
{
	if (!fdesc) {
		psm_d("%s: Invalid arguments", __func__);
		return -WM_E_INVAL;
	}

	if (phandle)
		*phandle = 0;

	/*
	 * Check is PSM is already registered for received partition and
	 * return same handle.
	 */
	int save = os_enter_critical_section(); /* We are working on a list */
	psm_t *psm = _get_existing_psm_object(fdesc);
	os_exit_critical_section(save);
	if (psm) {
		if (phandle)
			*phandle = (psm_hnd_t) psm;
		return WM_SUCCESS;
	}

	psm_entry("start-address: 0x%x size: %d",
		  fdesc->fl_start, fdesc->fl_size);

	uint32_t block_size = psm_get_block_size();
	int rv = _psm_init_verify_params(fdesc, block_size,
					 psm_cfg);
	if (rv != WM_SUCCESS)
		return rv;

	/* Initialise flash memory to be used */
	flash_drv_init();

	/* Open the flash */
	mdev_t *fl_dev = flash_drv_open(fdesc->fl_dev);
	if (fl_dev == NULL) {
		psm_d("Flash driver open failed");
		return -WM_FAIL;
	}

	/* Allocate partition object. */
	psm = os_mem_calloc(sizeof(psm_t));
	if (!psm) {
		psm_d("cannot allocate PSM instance");
		flash_drv_close(fl_dev);
		return -WM_E_NOMEM;
	}

	rv = psm_init_locks(psm);
	if (rv != WM_SUCCESS) {
		flash_drv_close(fl_dev);
		os_mem_free(psm);
		return rv;
	}

	if (psm_cfg)
		memcpy(&psm->psm_cfg, psm_cfg, sizeof(psm_cfg_t));

#ifdef CONFIG_SECURE_PSM
	/* For test code only */
	if (force_psm_secure)
		psm->psm_cfg.secure = true;

	if (psm->psm_cfg.keystore_override == false) {
		/* First check if keystore has a key */
		rv = psm_security_init(&psm->psm_sec_hnd);
		if (rv == WM_SUCCESS)
			psm_d("Using key from keystore");
		else if (rv == -WM_E_NOENT) {
			psm_d("No key in keystore. Try using key from app");
			rv = psm_security_init_with_key(&psm->psm_sec_hnd,
						&psm->psm_cfg);
			if (rv == WM_SUCCESS)
				psm_d("Using key from app");
			else if (rv == -WM_E_INVAL)
				psm_e("No app key found");
		}
	} else {
		/* Directly use app key, ignoring keystore key if any. */
		rv = psm_security_init_with_key(&psm->psm_sec_hnd,
						&psm->psm_cfg);
		if (rv == WM_SUCCESS)
			psm_d("Using key from app");
		else if (rv == -WM_E_INVAL)
			psm_e("No app key found");
	}

	/* Return error only if partition is encrypted. */
	if (part_encrypted(psm) && (rv != WM_SUCCESS)) {
		psm_d("sec init failed");
		psm_deinit_locks(psm);
		flash_drv_close(fl_dev);
		memset(psm, 0x00, sizeof(psm_t));
		os_mem_free(psm);
		return rv;
	}
#endif /* CONFIG_SECURE_PSM */

	psm->fdesc_fl_dev = fdesc->fl_dev;
	psm->fl_dev = fl_dev;
	psm->block_size = block_size;
	psm->block_cnt = fdesc->fl_size / block_size;
	psm->part_size = psm->block_size * psm->block_cnt;

	psm->start_address = fdesc->fl_start;

	if (!part_readonly(psm)) {
		/* Leave atleast one block for swap */
		psm->object_offset_limit = part_size(psm) -
			block_size(psm) - sizeof(psm_object_t);
	} else {
		psm->object_offset_limit = part_size(psm) -
			sizeof(psm_object_t);
	}

	psm_rwlock_part(psm, PSM_MODE_WRITE);
	rv = psm_module_physical_init(psm);
	if (rv != WM_SUCCESS) {
		psm_rwunlock_part(psm, PSM_MODE_WRITE);
		psm_deinit_locks(psm);
#ifdef CONFIG_SECURE_PSM
		if (part_encrypted(psm))
			psm_security_deinit(&psm->psm_sec_hnd);
#endif
		flash_drv_close(fl_dev);
		memset(psm, 0x00, sizeof(psm_t));
		os_mem_free(psm);
		return rv;
	}
	psm_rwunlock_part(psm, PSM_MODE_WRITE);

	save = os_enter_critical_section();
	list_add_tail(&psm->node, &part_list);
	os_exit_critical_section(save);

	if (phandle)
		*phandle = (psm_hnd_t) psm;

	return WM_SUCCESS;
}

int psm_module_deinit(psm_hnd_t *phandle)
{
	psm_entry();
	if (!phandle) {
		psm_d("NULL psm handle");
		return -WM_FAIL;
	}

	psm_t *psm = (psm_t *) *phandle;

	/* Ensure that all others have exited */
	psm_rwlock_part(psm, PSM_MODE_WRITE);
	psm_rwunlock_part(psm, PSM_MODE_WRITE);

	psm_reset_index(psm);
	psm_deinit_locks(psm);

#ifdef CONFIG_SECURE_PSM
	if (part_encrypted(psm))
		psm_security_deinit(&psm->psm_sec_hnd);
#endif

	/* Close the flash */
	flash_drv_close(psm->fl_dev);

	/* Remove ourselves from the global list */
	int save = os_enter_critical_section(); /* We are working on a list */
	list_del(&psm->node);
	os_exit_critical_section(save);

	memset(psm, 0x00, sizeof(psm_t));
	os_mem_free(psm);

	return WM_SUCCESS;
}

int psm_module_init_ex(const psm_init_part_t *psm_init_part, psm_hnd_t *phandle)
{
	flash_desc_t fl;
	int ret;
	struct partition_entry *p;

	ret = part_init();
	if (ret != WM_SUCCESS) {
		return ret;
	}

	memset(&fl, 0, sizeof(flash_desc_t));
	if (psm_init_part->pitype == PART_INFO_NAME) {
		p = part_get_layout_by_name(psm_init_part->part_info.name,
			NULL);
		if (p == NULL)
			return -WM_E_INVAL;
		part_to_flash_desc(p, &fl);
	} else if (psm_init_part->pitype == PART_INFO_FL_DESC) {
		memcpy(&fl, psm_init_part->part_info.fl, sizeof(flash_desc_t));
		if (!part_is_flash_desc_within_one_partition(&fl))
			return -WM_E_INVAL;
	} else
		return -WM_E_INVAL;

	return psm_module_init(&fl, phandle, psm_init_part->psm_cfg);
}

int psm_format(psm_hnd_t phandle)
{
	psm_entry();
	if (!phandle) {
		psm_d("NULL psm handle");
		return -WM_E_INVAL;
	}

	psm_t *psm = (psm_t *) phandle;

	psm_rwlock_part(psm, PSM_MODE_WRITE);
	psm_reset_index(psm);

	psm_d("Erasing psm partition");
	int rv = psm_erase_flash(psm, 0, part_size(psm));
	if (rv != WM_SUCCESS) {
		psm_d("Could not format");
		psm_rwunlock_part(psm, PSM_MODE_WRITE);
		return rv;
	}

	rv = psm_part_scan(psm);
	if (rv != WM_SUCCESS) {
		psm_d("Could not scan after format");
		psm_rwunlock_part(psm, PSM_MODE_WRITE);
		return rv;
	}

	psm_rwunlock_part(psm, PSM_MODE_WRITE);

	return WM_SUCCESS;
}


int psm_set_variable(psm_hnd_t phandle, const char *variable,
		     const void *value, uint32_t len)
{
	psm_entry("name: %s data-len: %d", variable, len);
	if (!phandle || !variable || !value) {
		psm_d("%s: Invalid arguments", __func__);
		return -WM_E_INVAL;
	}

	psm_t *psm = (psm_t *) phandle;
	if (!psm_is_data_len_sane(psm, len)) {
		psm_d("Unsupported data length for new object: %u", len);
		return -WM_E_INVAL;
	}

	psm_mode_t mode = PSM_MODE_WRITE;
	psm_rwlock_part(psm, mode);

	psm_objinfo_t objinfo;
	int rv = _psm_object_open(psm, &objinfo, variable,
				  mode, len, NULL);
	if (rv != WM_SUCCESS) {
		psm_d("Failed to open during set var-value");
		psm_rwunlock_part(psm, mode);
		return rv;
	}

	rv = _psm_object_write(psm, &objinfo, value, len);
	if (rv != WM_SUCCESS) {
		psm_d("Failed to write during set var-value");
		psm_rwunlock_part(psm, mode);
		return rv;
	}

	rv = _psm_object_close(psm, &objinfo);
	if (rv != WM_SUCCESS) {
		psm_d("Failed to close during set var-value");
		psm_rwunlock_part(psm, mode);
		return rv;
	}

	psm_rwunlock_part(psm, mode);
	return WM_SUCCESS;
}

int psm_get_variable(psm_hnd_t phandle,
		     const char *variable, void *value, uint32_t max_len)
{
	psm_entry("name: %s", variable);
	if (!phandle || !variable || !value || !max_len) {
		psm_d("%s: Invalid arguments", __func__);
		return -WM_E_INVAL;
	}

	psm_mode_t mode = PSM_MODE_READ;
	psm_t *psm = (psm_t *) phandle;
	psm_rwlock_part(psm, mode);

	psm_objinfo_t objinfo;
	int rv = _psm_object_open(psm, &objinfo, variable,
				  mode, 0, NULL);
	if (rv != WM_SUCCESS) {
		psm_d("Failed to open during get var-value");
		psm_rwunlock_part(psm, mode);
		return rv;
	}

	if (objinfo.pobj.data_len > max_len) {
		psm_d("%s: Variable value %d will not fit in given buffer %d",
		      __func__, objinfo.pobj.data_len, max_len);

		rv = _psm_object_close(psm, &objinfo);
		if (rv != WM_SUCCESS) {
			psm_d("Failed to close during get var-value (NO_SPC)");
			psm_rwunlock_part(psm, mode);
			return rv;
		}

		rv = -WM_E_NOSPC;
		goto psm_get_skip_read;
	}

	uint32_t read_len = _psm_object_read(psm, &objinfo, value, max_len);
	if (read_len < WM_SUCCESS) {
		psm_d("Failed to read during get var-value");
		psm_rwunlock_part(psm, mode);
		return rv;
	}

	rv = _psm_object_close(psm, &objinfo);
	if (rv != WM_SUCCESS) {
		psm_d("Failed to close during get var-value");
		psm_rwunlock_part(psm, mode);
		return rv;
	}

	rv = read_len;
 psm_get_skip_read:
	psm_rwunlock_part(psm, mode);
	return rv;
}

int psm_get_variable_size(psm_hnd_t phandle, const char *variable)
{
	psm_entry("name: %s", variable);
	if (!phandle || !variable) {
		psm_d("%s: Invalid arguments", __func__);
		return -WM_E_INVAL;
	}

	psm_mode_t mode = PSM_MODE_READ;
	psm_t *psm = (psm_t *) phandle;
	psm_rwlock_part(psm, mode);

	psm_objinfo_t objinfo;
	int rv = _psm_object_open(psm, &objinfo, variable,
				  mode, 0, NULL);
	if (rv != WM_SUCCESS) {
		psm_d("Failed to open during get size");
		psm_rwunlock_part(psm, mode);
		return rv;
	}

	int obj_size = objinfo.pobj.data_len;
	rv = _psm_object_close(psm, &objinfo);
	if (rv != WM_SUCCESS) {
		psm_d("Failed to close during get size");
		psm_rwunlock_part(psm, mode);
		return rv;
	}

	psm_rwunlock_part(psm, mode);
	return obj_size;
}

/* Only for testing interface */
int psm_register_event_callback(psm_event_callback cb)
{
	g_psm_event_cb = cb;
	return WM_SUCCESS;
}

/* Internal function. Only for testing. */
void psm_secure_state(bool state)
{
	force_psm_secure = state;
}

bool is_psm_encrypted(psm_hnd_t phandle)
{
	psm_entry();
	if (!phandle)
		return -WM_E_INVAL;

#ifdef CONFIG_SECURE_PSM
	psm_t *psm = (psm_t *) phandle;
	return part_encrypted(psm);
#else
	return false;
#endif
}

/* return non-zero value to signify that user does not want more names */
static int list_object(psm_t *psm, psm_object_t *pobj,
			uint32_t object_offset, psm_list_cb list_cb)
{
	if (pobj->name_len > READ_BUFFER_SIZE) {
		psm_d("name size more than buffer: %d", pobj->name_len);
		return -WM_FAIL;
	}

	uint8_t buf[READ_BUFFER_SIZE + 1];
	int rv;
	uint32_t name_offset = object_offset + sizeof(psm_object_t);

#ifdef CONFIG_SECURE_PSM
	if (part_encrypted(psm)) {
		rv = resetkey(psm, SET_DECRYPTION_KEY);
		if (rv)
			return -WM_FAIL;
	}
#endif

	/* Read the name field */
	rv = psm_read_flash(psm, buf, pobj->name_len, name_offset);
	if (rv != WM_SUCCESS) {
		psm_d("Failed to read name field @ 0x%x", name_offset);
		return -WM_FAIL;
	}

	/* NULL terminate */
	buf[pobj->name_len] = 0;

#ifdef CONFIG_SECURE_PSM
	if (part_encrypted(psm)) {
		rv = decrypt_buf(psm, buf, buf, pobj->name_len);
		if (rv)
			return -WM_FAIL;
	}
#endif

	/* Pass on to the user through the callback */
	return list_cb(buf);
}

static void _psm_objects_list(psm_t *psm, psm_list_cb list_cb)
{
	psm_entry_i();

	uint32_t offset = 0;

	/*
	 * Don't try reading flash if remaining size is less than
	 * necessary.
	 */
	uint32_t limit = psm->object_offset_limit;
	psm_object_t pobj;

	while (offset < limit) {
		int rv = psm_read_flash(psm, &pobj, sizeof(psm_object_t),
					offset);
		if (rv != WM_SUCCESS) {
			psm_d("list: could not read object @ off: %x", offset);
			return;
		}

		if (psm_is_free_space(&pobj)) {
			psm_d("Object list done");
			break;
		}

		if (pobj.object_type != OBJECT_TYPE_SMALL_BINARY) {
			psm_d("Invalid object at the end of data area");
			break;
		}

		if (!is_object_active(&pobj)) {
			offset += object_size(&pobj);
			continue;
		}

		if (!pobj.name_len) {
			psm_d("Zero name length objects not supported");
			offset += object_size(&pobj);
			continue;
		}

		rv = list_object(psm, &pobj, offset, list_cb);
		if (rv != WM_SUCCESS)
			break;

		offset += object_size(&pobj);
	}


}

void psm_objects_list(psm_hnd_t phandle, psm_list_cb list_cb)
{
	psm_entry();
	if (!phandle)
		return;

	psm_t *psm = (psm_t *) phandle;

	psm_rwlock_part(psm, PSM_MODE_READ);
	_psm_objects_list(psm, list_cb);
	psm_rwunlock_part(psm, PSM_MODE_READ);
}
