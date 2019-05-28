/** @file write_firmware.c
 *
 *  @brief  This file provides functinos to write firmware images to flash
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


/** write_firmware.c: Write firmware images to flash
 */

#include <string.h>

#include <wmstdio.h>
#include <rfget.h>
#include <crc32.h>
#include <wm_os.h>
#include <flash.h>
#include <partition.h>

#include "firmware.h"

#ifdef CONFIG_CPU_MW300
#include <secure_boot.h>
#endif
/*
 * For efficiently receiving data from network and writing it to internal
 * flash it is better to keep write buffer size equal to MTU in TCP.
 * (1500 - TCP/IP headers = 1460)
 */
#define FIRMWARE_UPDATE_BUFF_SIZE 256
extern void purge_stream_bytes(data_fetch data_fetch_cb, void *priv,
		size_t length, unsigned char *scratch_buf,
		const int scratch_buflen);
/*
 * Description	: Update the firmware
 * Input	: void
 * Output	: WM_SUCCESS  => ok
 * 		: -1 => Fail
 */
int update_and_validate_firmware(data_fetch data_fetch_cb, void *priv,
		    size_t firmware_length, struct partition_entry *f,
		    data_validate data_validate_cb)
{
	uint32_t filesize, read_length;
	int32_t size_read = 0;
	int error;
	update_desc_t ud;
	struct img_hdr ih;
	int hdr_size;
	unsigned char buf[FIRMWARE_UPDATE_BUFF_SIZE];
#ifdef CONFIG_CPU_MW300
	img_sec_hdr ish;
#endif

	rf_d(" Updating firmware at address 0x%0x"
	     , f->start);
	rf_d("@ address 0x%x len: %d", f->start, f->size);

	if (firmware_length == 0) {
		rf_e("Firmware binary zero length");
		return -WM_E_RFGET_INVLEN;
	}

	/* Verify firmware length first before proceeding further */
	if (firmware_length > f->size) {
		rf_e("Firmware binary too large %d > %d",
		     firmware_length, f->size);
		purge_stream_bytes(data_fetch_cb, priv, firmware_length,
				   buf, FIRMWARE_UPDATE_BUFF_SIZE);
		return -WM_E_RFGET_INVLEN;
	}

	/*
	 * Before erasing anything in flash, let's get the first
	 * firmware data buffer in order to validate it.
	 */
	size_read = 0;
#ifdef CONFIG_CPU_MW300
	hdr_size = (sizeof(ih) > sizeof(ish)) ? sizeof(ih) : sizeof(ish);
#else
	hdr_size = sizeof(ih);
#endif

	while ((hdr_size - size_read) != 0) {
		error = data_fetch_cb(priv, buf + size_read,
					   sizeof(ih) - size_read);
		if (error < 0) {
			rf_e("Failed to get firmware data for "
			     "initial validation");
			error = -WM_FAIL;
			goto out;
		}
		size_read += error;
	}

#ifdef CONFIG_CPU_MW300
	memcpy(&ish, buf, sizeof(ish));
	if (ish.magic == SEC_FW_MAGIC_SIG)
		goto begin;
#endif
	memcpy(&ih, buf, sizeof(ih));

	if (ih.magic_str != FW_MAGIC_STR || ih.magic_sig != FW_MAGIC_SIG) {
		rf_e("File data is not a firmware file");
		error = -WM_E_RFGET_INV_IMG;
		purge_stream_bytes(data_fetch_cb, priv,
				   firmware_length - size_read,
				   buf, FIRMWARE_UPDATE_BUFF_SIZE);
		goto out;
	}

#ifdef CONFIG_CPU_MW300
begin:
#endif
	rfget_init();
	error = rfget_update_begin(&ud, f);
	if (error) {
		rf_e("Failed to erase firmware area");
		purge_stream_bytes(data_fetch_cb, priv,
				   firmware_length - size_read,
				   buf, FIRMWARE_UPDATE_BUFF_SIZE);
		goto out;
	}

	filesize = 0;
	read_length = FIRMWARE_UPDATE_BUFF_SIZE;
	while (filesize < firmware_length) {
		if (filesize != 0) {
			/* we got the data the first time around */
			if ((firmware_length - filesize) < sizeof(buf))
				read_length = firmware_length - filesize;
			size_read = data_fetch_cb(priv, buf, read_length);
		}
		if (size_read <= 0)
			break;
		error = rfget_update_data(&ud, (char *)buf, size_read);
		if (error) {
			rf_e("Failed to write firmware data");
			goto cleanup;
		}
		filesize += size_read;
	}
	if (filesize != firmware_length) {
		rf_e("Invalid size of receievd file size.");
		error = -WM_FAIL;
		goto cleanup;
	}
	if (data_validate_cb) {
		rf_d("Validating received data");
		error = data_validate_cb(priv);
		if (error != WM_SUCCESS)
			goto cleanup;
	}
	rf_d("Firmware verification start ... filesize = %d", filesize);
	/* Then validate firmware data in flash */
#ifdef CONFIG_CPU_MW300
	if (ish.magic == SEC_FW_MAGIC_SIG)
		/* we do not verify the secure firmware, leave it to boot2 */
		error = 0;
	else
		error = verify_load_firmware(f->start, filesize);
#else
	error = verify_load_firmware(f->start, filesize);
#endif
	if (error) {
		rf_e("Firmware verification failed with code %d",
		     error);
		goto cleanup;
	}

	rf_d("Firmware verification done");
	rfget_update_complete(&ud);

	return WM_SUCCESS;
cleanup:
	rfget_update_abort(&ud);
out:
	return error;
}

int update_firmware(data_fetch data_fetch_cb, void *priv,
		    size_t firmware_length, struct partition_entry *f)
{
	return update_and_validate_firmware(data_fetch_cb, priv,
			firmware_length, f, NULL);
}
