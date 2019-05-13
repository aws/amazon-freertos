/*
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

/** @file wm_wlan.h
*
*  @brief Architecture specific system include file
*/

#ifndef _WM_WLAN_H_
#define _WM_WLAN_H_

#include <stdint.h>


/** Initialize WLAN
 *
 * This function is responsible for initializing only wireless.
 *
 * \return Returns 0 on success, error otherwise.
 * */

int wm_wlan_init();

/** De Initialize WLAN
 *
 * This function is responsible for deinitializing only wireless.
 *
 */
void wm_wlan_deinit();

/** Initialize SDK core components plus WLAN
 *
 * This function is responsible for initializing core SDK components like
 * uart, console, time, power management, health monitor, flash partitions
 * and wireless. Internally it calls the following apis:
 *  - \ref wmstdio_init()
 *  - \ref cli_init()
 *  - \ref wmtime_init()
 *  - \ref pm_init()
 *  - \ref healthmon_init()
 *  - \ref part_init()
 *  - \ref wlan_init()
 *
 * \return Returns 0 on success, error otherwise.
 * */
int wm_core_and_wlan_init(void);

#endif /* ! _WM_WLAN_H_ */
