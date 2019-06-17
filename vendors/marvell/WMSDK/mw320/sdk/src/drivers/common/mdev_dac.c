/** @file mdev_dac.c
 *
 *  @brief This file provides  mdev driver for DAC
 *
 *  (C) Copyright 2014-2018 Marvell International Ltd. All Rights Reserved.
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


/*
 * mdev_dac.c: mdev driver for DAC
 */
#include <lowlevel_drivers.h>
#include <mdev_dac.h>
#include <wm_os.h>

/* dac mdev and synchronization handles */
static mdev_t mdev_dac;
static os_mutex_t dac_mutex;

/* dac device specific names */
static const char *mdev_dac_name = {MDEV_DAC};
static DAC_ChannelID_Type dacdev_data;
static DAC_ChannelConfig_Type dacConfigSet = {.waveMode = DAC_WAVE_NORMAL,
			.outMode = DAC_OUTPUT_PAD,
			.timingMode = DAC_NON_TIMING_CORRELATED
};

void dac_modify_default_config(dac_cfg_param config_type, int value)
{
	switch (config_type) {
	case waveMode:
		if (DAC_WAVE_NORMAL <= value &&
				value <= DAC_WAVE_NOISE_DIFFERENTIAL)
			dacConfigSet.waveMode = value;
		break;
	case outMode:
		if (DAC_OUTPUT_INTERNAL <= value &&
				value <= DAC_OUTPUT_PAD)
			dacConfigSet.outMode = value;
		break;
	case timingMode:
		if (DAC_NON_TIMING_CORRELATED <= value &&
				value <= DAC_TIMING_CORRELATED)
			dacConfigSet.timingMode = value;
		break;

	}
}

mdev_t *dac_drv_open(const char *name, DAC_ChannelID_Type channel)
{
	int ret;
	mdev_t *mdev_p = mdev_get_handle(name);

	if (mdev_p == NULL) {
		DAC_LOG("driver open called before register (%s)\r\n", name);
		return NULL;
	}

	/* Make sure dac<n> is available for use */
	ret = os_mutex_get(&dac_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		DAC_LOG("failed to get mutex\r\n");
		return NULL;
	}

	board_dac_pin_config(channel);

	DAC_ChannelConfig(channel, &dacConfigSet);
	mdev_p->private_data = channel;
	DAC_ChannelEnable(channel);
	return mdev_p;
}

void dac_drv_output(mdev_t *mdev_p, int val)
{
	DAC_SetChannelData(mdev_p->private_data, (val & 0x3ff));
}

int dac_drv_close(mdev_t *mdev_p)
{
	DAC_ChannelDisable(mdev_p->private_data);
	DAC_ChannelReset(mdev_p->private_data);

	/* Mark dac<n> as free for use */
	return os_mutex_put(&dac_mutex);
}

static void dac_drv_mdev_init()
{
	mdev_t *mdev_p;

	mdev_p = &mdev_dac;
	mdev_p->name = mdev_dac_name;
	mdev_p->private_data = (uint32_t) &dacdev_data;
}

int dac_drv_init()
{
	int ret;

	if (mdev_get_handle(mdev_dac_name) != NULL)
		return WM_SUCCESS;

	ret = os_mutex_create(&dac_mutex, mdev_dac_name,
				      OS_MUTEX_INHERIT);
	if (ret == -WM_FAIL)
		return -WM_FAIL;

	/* Initialize the dac mdev structure */
	dac_drv_mdev_init();

	/* Register the dac device driver for this port */
	mdev_register(&mdev_dac);

	/* Initialise DAC with default config
	 * This configuration is common for both channels */
	/* FIXME: DAC_CFG_Type config should be user configurable */
	DAC_CFG_Type dacrefConfig;
	dacrefConfig.conversionRate =  DAC_CONVERSION_RATE_62P5KHZ;
	dacrefConfig.refSource = DAC_VREF_INTERNAL;
	dacrefConfig.rangeSelect = DAC_RANGE_LARGE;
	DAC_Init(&dacrefConfig);
	return WM_SUCCESS;
}

void dac_drv_deinit()
{
	os_mutex_delete(&dac_mutex);
	mdev_deregister(mdev_dac.name);
}
