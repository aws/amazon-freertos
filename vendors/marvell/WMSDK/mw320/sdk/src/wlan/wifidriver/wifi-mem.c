/** @file wifi-mem.c
 *
 *  @brief This file provides WIFI dymanic memory allocation APIs.
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
#include <mlan_wmsdk.h>
#include <string.h>
#include <wm_os.h>

#include <wifi-debug.h>

/* Simple memory allocator for Operating Systems that do not support dynamic
 * allocation. The size of the allocation is hard-coded to the need of the cli
 * module.
 *
 * If required this can be blown into a better slab-kind of allocator.
 */
#define HUGE_BUF_SIZE 2000
static char buffhuge[HUGE_BUF_SIZE] PM3_BSS;

void *wifi_mem_malloc_cmdrespbuf(int size)
{
	/* NOTE: There is no corresponding free call for cmdrespbuf */
	/* CMD are all serialised and they dont need any locking */
	return buffhuge;
}

void *wifi_malloc_eventbuf(int size)
{
	void *ptr = os_mem_alloc(size);

	if (ptr) {
		w_mem_d("[evtbuf] Alloc: A: %p S: %d",
			ptr, size);
	} else {
		w_mem_e("[evtbuf] Alloc: S: %d FAILED", size);
	}

	return ptr;
}

void wifi_free_eventbuf(void *buffer)
{
	w_mem_d("[evtbuf] Free: A: %p\n\r", buffer);
	os_mem_free(buffer);
}

mlan_status wrapper_moal_malloc(IN t_void *pmoal_handle, IN t_u32 size,
				IN t_u32 flag, OUT t_u8 **ppbuf)
{
	*ppbuf = os_mem_alloc(size);

#ifdef DEBUG_11N_ALLOC

#endif /* DEBUG_11N_ALLOC */

	if (*ppbuf) {
		w_mem_d("[mlan]: Alloc: A: %p S: %d", *ppbuf, size);
		return MLAN_STATUS_SUCCESS;
	} else {
		w_mem_e("[mlan] Alloc: S: %d FAILED", size);
		/*
		 * fixme: check if MLAN_STATUS_SUCCESS is to be returned in
		 * spite of the status failure.
		 */
		return MLAN_STATUS_FAILURE;
	}
}

mlan_status wrapper_moal_mfree(IN t_void *pmoal_handle, IN t_u8 *pbuf)
{
	w_mem_d("[mlan] Free: A: %p", pbuf);
	os_mem_free(pbuf);
	return MLAN_STATUS_SUCCESS;
}
