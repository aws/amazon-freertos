/** @file pwrmgr.c
 *
 *  @brief This file provides  Power Manager generic functions
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


/* pwrmgr_generic.c: Power Manager generic functions
 */
#include <stdlib.h>

#include <wmstdio.h>
#include <cli.h>
#include <cli_utils.h>
#include <wmerrno.h>
#include <mdev_rtc.h>
#include <pwrmgr.h>
#include <rtc.h>
#include <wm_os.h>
#include <big-lock.h>
#include "wakelock_debug.h"

static char init_done;

static os_semaphore_t wakelock;

/* This value can be changed as per requirement*/
#define MAX_CLIENTS 10
/**
   This structure holds callback function
   and action for which callback is to be involved
*/
typedef struct {
	unsigned char action;
	power_save_state_change_cb_fn cb_fn;
	void *data;
} power_state_change_cb_t;

static power_state_change_cb_t ps_cb_list[MAX_CLIENTS];

int pm_register_cb(unsigned char action, power_save_state_change_cb_fn cb_fn,
		   void *data)
{
	if (NULL == cb_fn)
		return -WM_E_INVAL;
	int cnt = 0;
	/* A globally created mutex is being used for protection
	 *  to avoid inter-dependencies amongst module inits
	 */
	int ret = big_lock(OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		return ret;
	}
	for (cnt = 0; cnt < MAX_CLIENTS; cnt++) {
		if (NULL == ps_cb_list[cnt].cb_fn) {
			/* This is free location use it */
			ps_cb_list[cnt].cb_fn = cb_fn;
			ps_cb_list[cnt].action = action;
			ps_cb_list[cnt].data = data;
			big_unlock();
			return cnt;
		}
	}
	big_unlock();
	return -WM_FAIL;
}

int pm_change_event(int handle, unsigned char action)
{
	if (handle >= MAX_CLIENTS || handle < 0)
		return -WM_FAIL;
	int ret = big_lock(OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		return ret;
	}
	ps_cb_list[handle].action = action;
	return big_unlock();
}

int pm_deregister_cb(int handle)
{
	if (handle >= MAX_CLIENTS || handle < 0)
		return -WM_FAIL;
	int ret = big_lock(OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		return ret;
	}
	ps_cb_list[handle].cb_fn = NULL;
	return big_unlock();
}

void pm_invoke_ps_callback(power_save_event_t action)
{
	int cnt = 0;

	for (cnt = 0; cnt < MAX_CLIENTS; cnt++) {
		if (ps_cb_list[cnt].cb_fn) {
			if (ps_cb_list[cnt].action & action)
				ps_cb_list[cnt].cb_fn(action,
						      ps_cb_list[cnt].data);
		}
	}
}

inline int wakelock_get(const char *id_str)
{
	if (init_done) {
		wakelock_get_debug(id_str);
		return os_semaphore_put(&wakelock);
	} else {
		return 0;
	}
}

inline int wakelock_put(const char *id_str)
{
	if (init_done) {
		wakelock_put_debug(id_str);
		return os_semaphore_get(&wakelock, 0);
	} else {
		return 0;
	}
}

inline int wakelock_isheld(void)
{
	if (init_done) {
		return os_semaphore_getcount(&wakelock);
	} else {
		return 0;
	}
}


void pm_reboot_soc()
{
#ifdef CONFIG_HW_RTC
	hwrtc_time_update();
#endif
	arch_reboot();
}


int pm_init(void)
{
	int ret = WM_SUCCESS;

	if (init_done)
		return WM_SUCCESS;

	ret = os_semaphore_create_counting(&wakelock, "wake-lock", 10, 0);
	if (ret == -WM_FAIL) {
		pm_d("Failed to create counting semaphore");
		return ret;
	}
	/* RTC driver init is required by the power manager */
	rtc_drv_init();

	arch_pm_init();

	init_done = 1;

	return WM_SUCCESS;
}

bool is_pm_init_done()
{
	return init_done;
}
