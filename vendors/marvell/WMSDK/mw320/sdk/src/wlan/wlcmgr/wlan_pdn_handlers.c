/** @file wlan_pdn_handlers.c
 *
 *  @brief  This file provides Callback management for WLAN Power-Down event
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


/* wlan_pdn.c : Callback management for WLAN Power-Down event
 */
#include <stdlib.h>
#include <wmstdio.h>
#include <wm_os.h>
#include <big-lock.h>
#include <wlan_pdn_handlers.h>

/* This value can be changed as per requirement*/
#define MAX_CLIENTS 4
/**
   This structure holds callback function
   and action for which callback is to be involved
*/
typedef struct {
	unsigned char action;
	wlan_pdn_cb_fn cb_fn;
	void *data;
} wlan_pdn_cb_t;

static wlan_pdn_cb_t wlan_pdn_cb_list[MAX_CLIENTS];

int wlan_pdn_register_cb(unsigned char action, wlan_pdn_cb_fn cb_fn, void *data)
{
	if (NULL == cb_fn)
		return -WM_E_INVAL;
	int cnt = 0;
	/*  Big Lock is being used for protection
	 *  to avoid inter-dependencies amongst module inits
	 */
	int ret = big_lock(OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		return ret;
	}
	for (cnt = 0; cnt < MAX_CLIENTS; cnt++) {
		if (NULL == wlan_pdn_cb_list[cnt].cb_fn) {
			/* This is free location use it */
			wlan_pdn_cb_list[cnt].cb_fn = cb_fn;
			wlan_pdn_cb_list[cnt].action = action;
			wlan_pdn_cb_list[cnt].data = data;
			big_unlock();
			return cnt;
		}
	}
	big_unlock();
	return -WM_E_NOMEM;
}

int wlan_pdn_deregister_cb(int handle)
{
	if (handle >= MAX_CLIENTS || handle < 0)
		return -WM_FAIL;

	int ret = big_lock(OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		return ret;
	}
	wlan_pdn_cb_list[handle].cb_fn = NULL;
	return big_unlock();
}

void wlan_pdn_invoke_callback(wlan_pdn_event_t action)
{
	int cnt = 0;

	for (cnt = 0; cnt < MAX_CLIENTS; cnt++) {
		if (wlan_pdn_cb_list[cnt].cb_fn) {
			if (wlan_pdn_cb_list[cnt].action & action)
				wlan_pdn_cb_list[cnt].cb_fn(action,
						 wlan_pdn_cb_list[cnt].data);
		}
	}
}
