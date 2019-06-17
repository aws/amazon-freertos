/*
 * Copyright 2008-2015, Marvell International Ltd.
 * All Rights Reserved.
 */

#ifndef _FIRMWARE_STRUCTURE_H_
#define _FIRMWARE_STRUCTURE_H_

struct partition_header {
	uint32_t magic;
	uint32_t generation_level;
	uint32_t nr_segments;
	uint32_t crc;
};

struct firmware_segment {
	uint32_t offset;
	uint32_t length;
};

/*
 * Firmware magic signature
 *
 * First word is the string "MRVL" and is endian invariant.
 * Second word = magic value 0x2e9cf17.
 * Third word = time stamp (seconds since 00:00:00 UTC, January 1, 1970).
 */
struct img_hdr {
	uint32_t magic_str;
	uint32_t magic_sig;
	uint32_t time;
	uint32_t seg_cnt;
	uint32_t entry;
};

struct seg_hdr {
	uint32_t type;
	uint32_t offset;
	uint32_t len;
	uint32_t laddr;
	uint32_t crc;
};

struct wlan_fw_header {
	uint32_t        magic;
	uint32_t        length;
};

#define WLAN_FW_MAGIC          (('W' << 0)|('L' << 8)|('F' << 16)|('W' << 24))


#define PART_MAGIC	0x9e78d963

/* Make the "MRVL" string into an endian invariant 32-bit constant */
#if defined(__ARMEB__)
#define FW_MAGIC_STR		(('M' << 24)|('R' << 16)|('V' << 8)|('L' << 0))
#else /* if defined(__ARMEL__) */
#define FW_MAGIC_STR		(('M' << 0)|('R' << 8)|('V' << 16)|('L' << 24))
#endif

#define FW_MAGIC_SIG		0x2e9cf17b

#define FW_BLK_ANCILLARY	1	/* ignores data to follow */
#define FW_BLK_LOADABLE_SEGMENT	2	/* loads data to follow */
#define FW_BLK_FN_CALL		3	/* calls function at given addr */
#define FW_BLK_POKE_DATA	4	/* pokes 32-bit value to address */

#endif  /* ! _FIRMWARE_STRUCTURE_H_ */
