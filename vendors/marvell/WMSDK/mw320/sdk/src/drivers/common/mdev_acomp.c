/** @file mdev_acomp.c
 *
 *  @brief This file provides  mdev driver for ACOMP
 *
 *  (C) Copyright 2014-2019 Marvell International Ltd. All Rights Reserved.
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
 * mdev_acomp.c: mdev driver for ACOMP
 */
#include <lowlevel_drivers.h>
#include <mdev_acomp.h>
#include <wm_os.h>

#define DEV_ACOMP_NUM 2

/* acomp mdev and synchronization handles */
static mdev_t mdev_acomp[DEV_ACOMP_NUM];
static os_mutex_t acomp_mutex[DEV_ACOMP_NUM];

/* acomp device specific names */
static const char *mdev_acomp_name[DEV_ACOMP_NUM] = {
	"MDEV_ACOMP0",
	"MDEV_ACOMP1",
};

mdev_t *acomp_drv_open(ACOMP_ID_Type acomp_id, ACOMP_PosChannel_Type pos,
					 ACOMP_NegChannel_Type neg)
{
	int ret;
	mdev_t *mdev_p = mdev_get_handle(mdev_acomp_name[acomp_id]);

	if (mdev_p == NULL) {
		ACOMP_LOG("driver open called before register (%s)\r\n",
			  mdev_acomp_name[acomp_id]);
		return NULL;
	}

	/* Make sure acomp<n> is available for use */
	ret = os_mutex_get(&acomp_mutex[mdev_p->port_id], OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		ACOMP_LOG("failed to get mutex\r\n");
		return NULL;
	}

#ifdef CONFIG_CPU_MW300
	/* Configure pinmux for external pins */
	if (pos <= ACOMP_POS_CH_GPIO49)
		GPIO_PinMuxFun((ACOMP_POS_CH_GPIO42 + pos), PINMUX_FUNCTION_1);
	if (neg <= ACOMP_NEG_CH_GPIO49)
		GPIO_PinMuxFun((ACOMP_NEG_CH_GPIO42 + neg), PINMUX_FUNCTION_1);

#else
	if (pos <= ACOMP_POS_CH_GPIO0)
		GPIO_PinMuxFun((ACOMP_POS_CH_GPIO0 - pos), PINMUX_FUNCTION_6);
	if (neg <= ACOMP_NEG_CH_GPIO0)
		GPIO_PinMuxFun((ACOMP_NEG_CH_GPIO0 - neg), PINMUX_FUNCTION_6);
#endif

	ACOMP_ChannelConfig(mdev_p->port_id, pos, neg);
	ACOMP_Enable(mdev_p->port_id);

	return mdev_p;
}

int acomp_drv_result(mdev_t *mdev_p)
{
	return ACOMP_GetResult(mdev_p->port_id);
}

int acomp_drv_close(mdev_t *dev)
{
	/* Mark acomp<n> as free for use */
	return os_mutex_put(&acomp_mutex[dev->port_id]);
}

static void acomp_drv_mdev_init(uint8_t acomp_num)
{
	mdev_t *mdev_p;

	mdev_p = &mdev_acomp[acomp_num];

	mdev_p->name = mdev_acomp_name[acomp_num];

	mdev_p->port_id = acomp_num;
}

int acomp_drv_init(ACOMP_ID_Type id)
{
	int ret;

	if (mdev_get_handle(mdev_acomp_name[id]) != NULL)
		return WM_SUCCESS;

	ACOMP_CFG_Type acompConfigSet = {ACOMP_HYSTER_0MV, ACOMP_HYSTER_0MV,
				LOGIC_LO, ACOMP_PWMODE_1,
				ACOMP_WARMTIME_16CLK, ACOMP_PIN_OUT_SYN};

	ret = os_mutex_create(&acomp_mutex[id], mdev_acomp_name[id],
				      OS_MUTEX_INHERIT);
	if (ret == -WM_FAIL)
		return -WM_FAIL;

	/* Initialize the acomp mdev structure */
	acomp_drv_mdev_init(id);

	/* Initialize the ACOMP */
	ACOMP_Init(id, &acompConfigSet);

	/* Register the acomp device driver for this port */
	mdev_register(&mdev_acomp[id]);

	return WM_SUCCESS;
}

int acomp_drv_deinit(ACOMP_ID_Type acomp_id)
{
	/* Return if port id is invalid */
	if (acomp_id < 0 || acomp_id > DEV_ACOMP_NUM) {
		ACOMP_LOG("Port %d not enabled\r\n, port_id");
		return -WM_FAIL;
	}
	/* Return if the device was not registered */
	mdev_t *dev = mdev_get_handle(mdev_acomp_name[acomp_id]);
	if (dev == NULL) {
		ACOMP_LOG("Device was not initialized\r\n");
		return -WM_FAIL;
	}
	/* Delete mutex and de-register the device */
	os_mutex_delete(&acomp_mutex[acomp_id]);
	mdev_deregister(dev->name);
	return WM_SUCCESS;
}
