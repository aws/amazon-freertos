/** @file verify_firmware.c
 *
 *  @brief  This file provides functions verify the firmware image
 *
 *  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved.
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


/** verify_firmware.c: Verify the firmware image
 */
#include <wmstdio.h>
#include <rfget.h>
#include <crc32.h>
#include <mdev_crc.h>
#include <flash.h>

#include "firmware.h"


static void read_from_seghdr(mdev_t *fl_drv, struct firmware_segment *segment,
						void *addr_void, uint32_t size)
{
	uint8_t *addr = (uint8_t *) addr_void;
	while (size) {
		uint32_t length = segment->length;
		if (!length)
			return;
		if (length > size)
			length = size;
		flash_drv_read(fl_drv, (uint8_t *)addr, length,
							segment->offset);
		segment->offset += length;
		segment->length -= length;
		addr += length;
		size -= length;
	}

	return ;
}

static uint32_t check_crc(mdev_t *fl_drv, uint32_t offset, uint32_t len,
								uint32_t crc)
{
	uint32_t dummy_buf[32];
	uint32_t result = 0, blk = 128;

#ifndef CONFIG_SOFTWARE_CRC32
	mdev_t *crc_drv = crc_drv_open(MDEV_CRC_ENGINE_0, CRC32_IEEE);
	if (crc_drv == NULL) {
		rf_e("CRC driver init is required before open");
		return 1;
	}
	crc_drv_stream_setlen(crc_drv, len);
#endif

	while (len) {
		if (blk > len)
			blk = len;
		flash_drv_read(fl_drv, (uint8_t *)dummy_buf, blk, offset);
#ifndef CONFIG_SOFTWARE_CRC32
		crc_drv_stream_feed(crc_drv, (uint8_t *)dummy_buf, blk);
#else
		result = soft_crc32(dummy_buf, blk, result);
#endif
		len -= blk;
		offset += blk;
	}
#ifndef CONFIG_SOFTWARE_CRC32
	crc_drv_stream_get_result(crc_drv, &result);
	crc_drv_close(crc_drv);
#endif

	return (result == crc) ? 0 : 1;
}

int verify_load_firmware(uint32_t start, int size)
{
	mdev_t *fl_drv;
	struct firmware_segment segment;
	struct img_hdr ih;
	struct seg_hdr sh;

	fl_drv = flash_drv_open(FL_PART_DEV);
	if (fl_drv == NULL) {
		rf_e("Flash driver init is required before open");
		goto error;
	}

	segment.offset = start;
	segment.length = size;

	/* load and validate image header */
	read_from_seghdr(fl_drv, &segment, &ih, sizeof(ih));
	if ((ih.magic_str != FW_MAGIC_STR) ||
			(ih.magic_sig != FW_MAGIC_SIG))
		goto error;

	while (ih.seg_cnt--) {
		read_from_seghdr(fl_drv, &segment, &sh, sizeof(sh));
		sh.offset += start;

		switch (sh.type) {
		case FW_BLK_LOADABLE_SEGMENT:
			if (check_crc(fl_drv, sh.offset, sh.len, sh.crc))
				goto error;
			break;

		case FW_BLK_POKE_DATA:
		case FW_BLK_FN_CALL:
		case FW_BLK_ANCILLARY:
			/* do nothing */
			break;

		default:
			goto error;
		}
	}


	return WM_SUCCESS;
error:
	return -WM_E_RFGET_FFW_LOAD;
}
