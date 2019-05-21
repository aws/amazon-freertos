/** @file crc32.c
 *
 *  @brief This file provides  CRC 32 API
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


#include <crc32.h>
#include <wmstdio.h>
#include <wmerrno.h>
#include <wmlog.h>
#include <wm_utils.h>

#define crc_e(...)				\
	wmlog_e("crc", ##__VA_ARGS__)
#define crc_w(...)				\
	wmlog_w("crc", ##__VA_ARGS__)

#ifdef CONFIG_CRC_DEBUG
#define crc_d(...)				\
	wmlog("crc", ##__VA_ARGS__)
#else
#define crc_d(...)
#endif /* ! CONFIG_CRC_DEBUG */

#ifndef CONFIG_SOFTWARE_CRC32
#include <mdev_crc.h>
#endif

uint32_t crc32(const void *data__, int data_size, uint32_t prev_crc)
{
#ifdef CONFIG_SOFTWARE_CRC32
	return soft_crc32(data__, data_size, prev_crc);
#else
	uint32_t crc = 0;
	if (prev_crc) {
		crc_e("Hardware crc engine do not support"
			       "checksum accumulation\r\n");
		return -WM_FAIL;
	}
	mdev_t *crc_drv = crc_drv_open(MDEV_CRC_ENGINE_0, CRC32_IEEE);
	if (crc_drv == NULL) {
		crc_e("Unable to open driver");
		return -WM_FAIL;
	}

	crc_drv_block_calc(crc_drv, (uint8_t *)data__, data_size, &crc);
	crc_drv_close(crc_drv);
	return crc;
#endif
}

void crc32_init()
{
#ifndef CONFIG_SOFTWARE_CRC32
	crc_drv_init();
#endif
}
