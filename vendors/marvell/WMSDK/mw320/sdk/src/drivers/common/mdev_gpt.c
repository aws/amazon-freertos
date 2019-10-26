/** @file mdev_gpt.c
 *
 *  @brief This file provides  mdev driver for GPT
 *
 *  (C) Copyright 2008-2019 Marvell International Ltd. All Rights Reserved.
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
 * mdev_gpt.c: mdev driver for GPT
 */
#include <lowlevel_drivers.h>
#include <mdev_gpt.h>
#include <wm_os.h>

#define MDEV_GPT_NUM 4

/* gpt mdev and synchronization handles */
static mdev_t mdev_gpt[MDEV_GPT_NUM];
static os_mutex_t gpt_mutex[MDEV_GPT_NUM];

/* gpt device specific names */
static const char *mdev_gpt_name[MDEV_GPT_NUM] = {
	"MDEV_GPT0",
	"MDEV_GPT1",
	"MDEV_GPT2",
	"MDEV_GPT3"
};

/* gpt device specific private data */
static GPT_Config_Type gptdev_data[MDEV_GPT_NUM];

mdev_t *gpt_drv_open(GPT_ID_Type gpt_id)
{
	int ret;
	GPT_Config_Type *gpt_cfg;
	mdev_t *mdev_p = mdev_get_handle(mdev_gpt_name[gpt_id]);

	if (mdev_p == NULL) {
		GPT_LOG("driver open called before register (%s)\r\n",
			mdev_gpt_name[gpt_id]);
		return NULL;
	}

	gpt_cfg = (GPT_Config_Type *) mdev_p->private_data;

	/* Make sure gpt<n> is available for use */
	ret = os_mutex_get(&gpt_mutex[mdev_p->port_id], OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		GPT_LOG("failed to get mutex\r\n");
		return NULL;
	}

	/* gpt running on internal clock */
	gpt_cfg->clockSrc = GPT_CLOCK_0;
	gpt_cfg->cntUpdate = GPT_CNT_VAL_UPDATE_NORMAL;
	gpt_cfg->clockPrescaler = 0;
	gpt_cfg->clockDivider = 0;

	return mdev_p;
}

void gpt_drv_set(mdev_t *dev, uint32_t interval_us)
{
	GPT_Config_Type *gpt_cfg;
	gpt_cfg = (GPT_Config_Type *) dev->private_data;

	/* Load gpt with user specified value in us, gpt @ 50MHz */
	gpt_cfg->uppVal = (50 * interval_us);
	/* Configure and load gpt */
	GPT_Init(dev->port_id, gpt_cfg);
	/* Clear old status */
	GPT_CounterReset(dev->port_id);
}

void gpt_drv_pwm(mdev_t *dev, GPT_ChannelNumber_Type ch,
			uint32_t ton, uint32_t toff)
{
	GPT_ChannelOutputConfig_Type type;

	GPT_ChannelFuncSelect(dev->port_id, ch, GPT_CH_FUNC_PWM_EDGE);
	type.polarity = GPT_CH_WAVE_POL_POS;
	type.cntMatchVal0 = ton;
	type.cntMatchVal1 = toff;
	GPT_ChannelOutputConfig(dev->port_id, ch, &type);
	GPT_ChannelReset(dev->port_id, (1 << ch));
}

void gpt_drv_setcb(mdev_t *dev, void (*func) (void))
{
	/* Install callback handler for counter overflow */
	install_int_callback(INT_GPT0 + (dev->port_id), GPT_INT_CNT_UPP,
				func);
}

void gpt_drv_start(mdev_t *dev)
{
	/* Enable interrupt and start gpt operation */
	NVIC_EnableIRQ(GPT0_IRQn + dev->port_id);
	NVIC_SetPriority((GPT0_IRQn + dev->port_id), 0xf);
	GPT_IntMask(dev->port_id, GPT_INT_CNT_UPP, UNMASK);
	GPT_Start(dev->port_id);
}

void gpt_drv_stop(mdev_t *dev)
{
	/* Disable interrupt, clear status and stop gpt operation */
	GPT_StatusClr(dev->port_id, GPT_STATUS_CNT_UPP);
	NVIC_DisableIRQ(GPT0_IRQn + dev->port_id);
	GPT_Stop(dev->port_id);
}

int gpt_drv_close(mdev_t *dev)
{
	/* Mark gpt<n> as free for use */
	return os_mutex_put(&gpt_mutex[dev->port_id]);
}

static void gpt_drv_mdev_init(uint8_t gpt_num)
{
	mdev_t *mdev_p;

	mdev_p = &mdev_gpt[gpt_num];

	mdev_p->name = mdev_gpt_name[gpt_num];
	mdev_p->port_id = gpt_num;
	mdev_p->private_data = (uint32_t) &gptdev_data[gpt_num];
}

int gpt_drv_init(GPT_ID_Type id)
{
	int ret, clk = 0, clk_id = 0;

	if (mdev_get_handle(mdev_gpt_name[id]) != NULL)
		return WM_SUCCESS;

	ret = os_mutex_create(&gpt_mutex[id], mdev_gpt_name[id],
						OS_MUTEX_INHERIT);
	if (ret == -WM_FAIL)
		return -WM_FAIL;

	/* Initialize the general purpose gpt mdev structure */
	gpt_drv_mdev_init(id);

	switch (id) {

	case GPT0_ID:
		clk = CLK_GPT0;
		clk_id = CLK_GPT_ID_0;
		break;
	case GPT1_ID:
		clk = CLK_GPT1;
		clk_id = CLK_GPT_ID_1;
		break;
	case GPT2_ID:
		clk = CLK_GPT2;
		clk_id = CLK_GPT_ID_2;
		break;
	case GPT3_ID:
		clk = CLK_GPT3;
		clk_id = CLK_GPT_ID_3;
		break;
	}

	/* Select CLock Source */
	CLK_GPTInternalClkSrc(clk_id, CLK_SYSTEM);

	/* Max GPT clock is 50MHz */
	if (CLK_GetSystemClk() > 50000000)
		CLK_ModuleClkDivider(clk, 4);

	/* Enable GPTx Clock */
	CLK_ModuleClkEnable(clk);

	/* Register the gpt device driver for this port */
	mdev_register(&mdev_gpt[id]);

	return WM_SUCCESS;
}
