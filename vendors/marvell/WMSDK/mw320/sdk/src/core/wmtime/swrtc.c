/** @file swrtc.c
 *
 *  @brief This file provides  Software rtc implementation
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


/** swrtc.c: Software rtc implementation
 */

#include <wm_os.h>
#include <wmtime.h>
#include <rtc.h>

static unsigned long n_ticks_per_sec;

/**
 * Global variable to avoid multiple initialization of rtc
 */
static int swrtc_init_flag = 0;

time_t cur_posix_time = 0;
unsigned int abs_tick_count = 0;
static unsigned int tick_count_ = 0;

/**
 * Update RTC counters on a system clock tick
 */
void swrtc_tick(void)
{
	abs_tick_count++;

	/* No protection is required as tick_count_ is not used anywhere else */
	if (++tick_count_ >= n_ticks_per_sec) {
		tick_count_ = 0;
		cur_posix_time++;
	}
}

/**
 * Initialize Software Real Time Clock
 */
void swrtc_init(void)
{
	if (swrtc_init_flag)
		return;
	swrtc_init_flag = 1;

	/* Set initial date to 1/1/1970 00:00 */
	cur_posix_time = 0;
	abs_tick_count = 0;

	n_ticks_per_sec = os_msec_to_ticks(1000);
	os_setup_tick_function(swrtc_tick);
}

int swrtc_time_set(time_t time)
{
    cur_posix_time = time;
    return 0;
}

time_t swrtc_time_get(void)
{
    return cur_posix_time;
}
