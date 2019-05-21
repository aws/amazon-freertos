/** @file wm_wlan.c
 *
 *  @brief This file provides  WLAN init API
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


#include <wlan.h>
#include <wmstdio.h>
#include <cli.h>
#include <healthmon.h>
#include <wmtime.h>
#include <boot_flags.h>
#include <partition.h>

#define init_e(...)	\
	wmlog_e("init", ##__VA_ARGS__)

#define init_l(...)	\
	wmlog("init", ##__VA_ARGS__)

static char wlan_init_done;
static char core_init_done;

int wm_wlan_init()
{
	struct partition_entry *p;
	short history = 0;
	struct partition_entry *f1, *f2;

	if (wlan_init_done)
		return WM_SUCCESS;

	int ret = part_init();
	if (ret != WM_SUCCESS) {
		init_e("wm_wlan_init: could not read partition table");
		return ret;
	}
	f1 = part_get_layout_by_id(FC_COMP_WLAN_FW, &history);
	f2 = part_get_layout_by_id(FC_COMP_WLAN_FW, &history);

	if (f1 && f2)
		p = part_get_active_partition(f1, f2);
	else if (!f1 && f2)
		p = f2;
	else if (!f2 && f1)
		p = f1;
	else
		return -WLAN_ERROR_FW_NOT_DETECTED;

	flash_desc_t fl;
	part_to_flash_desc(p, &fl);

	/* Initialize wlan */
	ret = wlan_init(&fl);
	if (ret != WM_SUCCESS)
		return ret;

	wlan_init_done = 1;

	init_l("Initialized wireless stack");

	return WM_SUCCESS;
}

int wm_core_init(void)
{
	int ret = 0;

	if (core_init_done)
		return WM_SUCCESS;

	wmstdio_init(UART0_ID, 0);

	ret = cli_init();
	if (ret != WM_SUCCESS) {
		init_e("Cli init failed.");
		return ret;
	}

	ret = wmtime_init();
	if (ret != WM_SUCCESS) {
		init_e("Wmtime init failed.");
		return ret;
	}

	ret = pm_init();
	if (ret != WM_SUCCESS) {
		init_e("Power manager init failed.");
		return ret;
	}
#if 0 // not enabling healthmon
	ret = healthmon_init();
	if (ret != WM_SUCCESS) {
		init_e("Healthmon init failed.");
		return ret;
	}
#endif

	/* Read partition table layout from flash */
	ret = part_init();
	if (ret != WM_SUCCESS) {
		init_e("Partition init failed.");
		return ret;
	}

	core_init_done = 1;

	init_l("Initialized core modules");

	return WM_SUCCESS;
}

int wm_core_and_wlan_init()
{
	int ret;

	ret = wm_core_init();
	if (ret != WM_SUCCESS) {
		init_e("Core modules initialization failed");
		return ret;
	}

	ret = wm_wlan_init();
	if (ret != WM_SUCCESS) {
		init_e("Wireless stack initialization failed");
		return ret;
	}

	return WM_SUCCESS;
}

void wm_wlan_deinit()
{
	/* De-Initialize wlan */
	wlan_deinit(WLAN_ACTIVE);

	wlan_init_done = 0;
}
