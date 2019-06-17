/** @file mdev.c
 *
 *  @brief This file provides  Functions for the Marvell Device Driver Interface
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


/** mdev.c: Functions for the Marvell Device Driver Interface
 */
#include <string.h>

#include <mdev.h>
#include <wm_os.h>

mdev_t *pMdevList = NULL;

/**
 * registering module
 *
 * @return
 */
uint32_t mdev_register(mdev_t *dev)
{
	mdev_t *pMdev = pMdevList;
	mdev_t *pTargetMdev = dev;
	uint32_t int_sta;

	if (pTargetMdev == NULL || pTargetMdev->name == NULL)
		return 1;

	pTargetMdev->pNextMdev = NULL;

	int_sta = os_enter_critical_section();

	while (pMdev != NULL) {
		if (!strcmp(pMdev->name, dev->name)) {
			os_exit_critical_section(int_sta);
			return 1;
		}

		if (pMdev->pNextMdev == NULL) {
			pMdev->pNextMdev = pTargetMdev;
			pTargetMdev->pNextMdev = NULL;
			os_exit_critical_section(int_sta);
			return 0;
		}
		pMdev = pMdev->pNextMdev;
	}

	pMdevList = pTargetMdev;

	os_exit_critical_section(int_sta);
	return 0;
}

/**
 * deregistering module
 *
 * @return
 */
uint32_t mdev_deregister(const char *name)
{
	mdev_t *pMdev = pMdevList;
	mdev_t *pPrevMdev = pMdevList;
	uint32_t int_sta;
	int_sta = os_enter_critical_section();

	while (pMdev != NULL) {
		if (!strcmp(pMdev->name, name)) {
			pPrevMdev->pNextMdev = pMdev->pNextMdev;
			os_exit_critical_section(int_sta);
			return 0;
		}
		pPrevMdev = pMdev;
		pMdev = pMdev->pNextMdev;
	}
	os_exit_critical_section(int_sta);
	return 1;
}

mdev_t *mdev_get_handle(const char *dev_name)
{
	mdev_t *pMdev = pMdevList;
	uint32_t int_sta;
	int_sta = os_enter_critical_section();

	while (pMdev != NULL) {
		if (!strcmp(pMdev->name, dev_name)) {
			os_exit_critical_section(int_sta);
			return (void *)pMdev;
		}
		pMdev = pMdev->pNextMdev;
	}
	os_exit_critical_section(int_sta);
	return NULL;
}
