/** @file wakelock_debug.h
*
*  @brief Wlake Lock Debug Header
*
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

#ifndef _WAKELOCK_DEBUG_H_
#define _WAKELOCK_DEBUG_H_

#define MAX_LOGICAL_WAKELOCKS	16

struct wl_debug_info {
	const char *id_str;
	int count;
	unsigned underflow;
};

#ifndef CONFIG_WAKELOCK_DEBUG
inline void wakelock_put_debug(const char *id_str) { }
inline void wakelock_get_debug(const char *id_str) { }

/*
 * Enable wakelock CLI
 * This function registers the CLI for the wakelock
 * wakelock-dbg command is used to get the additional
 * debug information about it
 */

#define wakelock_cli_init(...)
#else
extern void wakelock_put_debug(const char *id_str);
extern void wakelock_get_debug(const char *id_str);
extern int wakelock_cli_init(void);
#endif

#endif
