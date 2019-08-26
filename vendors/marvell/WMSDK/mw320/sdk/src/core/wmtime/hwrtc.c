/** @file hwrtc.c
 *
 *  @brief This file provides  API for HW RTC
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


#include <wm_os.h>
#include <wmtime.h>
#include <rtc.h>
#include <mdev_rtc.h>
#include <compat_attribute.h>

/**
 * Global variable to avoid multiple initialization of rtc
 */
static int hwrtc_init_flag;

__SECTION__(.nvram_uninit)
static uint64_t rtc_ticks;
__SECTION__(.nvram_uninit)
static uint32_t rtc_sig;

static mdev_t *rtc_dev;
/*
 * It is observed that it takes ~5ms for update, reset, start
 * cycle of the RTC. So we need to add the lost time everytime.
 * NOTE: This is an approximate compensation, so some inaccuracy
 * will persist in the time kept by the RTC.
 */
uint8_t tcalib = 5;

void hwrtc_time_update(void)
{
	rtc_ticks += (uint64_t)rtc_drv_get(rtc_dev) + (uint64_t)tcalib;
}

static void hwrtc_cb(void)
{
	rtc_ticks += (uint64_t)rtc_drv_get_uppval(rtc_dev) + (uint64_t)1;
	rtc_drv_set(rtc_dev, 0xffffffff);
	rtc_drv_reset(rtc_dev);
}

void hwrtc_init(void)
{
	if (hwrtc_init_flag)
		return;
	hwrtc_init_flag = 1;

	/* Initialize if nvram is empty */
	if (rtc_sig != 0xdeadbeef) {
		rtc_ticks = 0;
		rtc_sig = 0xdeadbeef;
	}

	rtc_drv_set_cb(hwrtc_cb);
	rtc_drv_init();
	rtc_dev = rtc_drv_open("MDEV_RTC");
	/* Update the time before reseting RTC */
	hwrtc_time_update();
	rtc_drv_reset(rtc_dev);
	rtc_drv_start(rtc_dev);
}

int hwrtc_time_set(time_t time)
{
	rtc_drv_reset(rtc_dev);
	/* RTC runs at 1024 Hz */
	rtc_ticks = (uint64_t)time << 10;
	return 0;
}

time_t hwrtc_time_get(void)
{
	/* Convert to seconds before returning */
	return (rtc_ticks + (uint64_t)rtc_drv_get(rtc_dev)) >> 10;
}
