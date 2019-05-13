/** @file mdev_adc.c
 *
 *  @brief This file provides  mdev driver for ADC
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


/*
 * mdev_adc.c: mdev driver for ADC
 */
#include <mdev_adc.h>
#include <lowlevel_drivers.h>
#include <wm_os.h>
#include <limits.h>
#include <mdev_dma.h>

#define MW300_ADC_NUM 1

/* adc mdev and synchronization handles */
static mdev_t mdev_adc[MW300_ADC_NUM];
static os_mutex_t adc_mutex[MW300_ADC_NUM];

static uint32_t adc_tout[MW300_ADC_NUM];

/* Maximum ADC samples DMA can transfer in one go */
#define MAX_ADC_DMA_SIZE (DMA_MAX_CHUNK_SIZE / DMA_TRANSF_WIDTH_16)

/* adc device specific names */
static const char *mdev_adc_name[MW300_ADC_NUM] = {
	"MDEV_ADC0",
};

typedef struct adc_priv_data {
	ADC_ChannelSource_Type channelx;
} adc_data_t;

static adc_data_t adcdrv_data[MW300_ADC_NUM];

ADC_CFG_Type adcConfig = {.adcResolution = ADC_RESOLUTION_16BIT,
			.adcVrefSource = ADC_VREF_12,
			.adcGainSel = ADC_GAIN_1,
			.adcClockDivider = ADC_CLOCK_DIVIDER_32,
};

void adc_get_config(ADC_CFG_Type *config)
{
	*config = adcConfig;
}

void adc_modify_default_config(adc_cfg_param config_type, int value)
{
	switch (config_type) {
	case adcResolution:
			if ((ADC_RESOLUTION_12BIT <= value) &&
					(value <= ADC_RESOLUTION_16BIT_AUDIO))
				adcConfig.adcResolution = value;
		break;

	case adcVrefSource:
		if ((ADC_VREF_18 <= value) &&
				(value <= ADC_VREF_FILTERED_12))
			adcConfig.adcVrefSource = value;
		break;

	case adcGainSel:
		if ((ADC_GAIN_0P5 <= value) &&
				(value <= ADC_GAIN_2))
			adcConfig.adcGainSel = value;
		break;

	case adcClockDivider:
		if ((ADC_CLOCK_DIVIDER_1 <= value) &&
				(value <= ADC_CLOCK_DIVIDER_32))
			adcConfig.adcClockDivider = value;
		break;

	default:
		break;
	}
}

mdev_t *adc_drv_open(ADC_ID_Type adc_id, ADC_ChannelSource_Type channel)
{
	if (channel < ADC_CH0) {
		ADC_LOG("Invalid channel \r\n");
		return NULL;
	}
	if (adc_id != ADC0_ID) {
		ADC_LOG("Invalid ADC id \r\n");
		return NULL;
	}
	adc_data_t *adc_data_p;
	int ret;
	mdev_t *mdev_p = mdev_get_handle(mdev_adc_name[adc_id]);

	if (mdev_p == NULL) {
		ADC_LOG("driver open called before register (%s)\r\n",
			mdev_adc_name[adc_id]);
		return NULL;
	}

	/* Make sure adc<n> is available for use */
	ret = os_mutex_get(&adc_mutex[mdev_p->port_id], OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		ADC_LOG("failed to get mutex\r\n");
		return NULL;
	}

	/* Configure pinmux for external pins */
	if (channel <= ADC_CH7) {
		int ret = board_adc_pin_config(mdev_p->port_id, channel);
		if (ret != WM_SUCCESS) {
			os_mutex_put(&adc_mutex[mdev_p->port_id]);
			return NULL;
		}
	}

	if (channel == ADC_TEMPP || channel == ADC_TEMPP_TEMPN) {
		adc_modify_default_config(adcResolution, ADC_RESOLUTION_14BIT);
	}
	ADC_Init(mdev_p->port_id, &adcConfig);
	adc_data_p = (adc_data_t *) mdev_p->private_data;
	adc_data_p->channelx = channel;
	ADC_Enable(mdev_p->port_id);
	if (channel == ADC_TEMPP || channel == ADC_TEMPP_TEMPN) {
		adcConfig.adcResolution = ADC_RESOLUTION_14BIT;
		ADC_ModeSelect(mdev_p->port_id, ADC_MODE_TSENSOR);
		ADC_TSensorConfig(ADC0_ID, ADC_SENSOR_INTERNAL);
	} else {
		ADC_ModeSelect(mdev_p->port_id, ADC_MODE_ADC);
	}
	/* Fixme single and differential mode */
	ADC_InputModeSelect(mdev_p->port_id, ADC_INPUT_SINGLE_ENDED);
	ADC_SetScanLength(mdev_p->port_id, ADC_SCAN_LENGTH_1);
	ADC_SetBufferMode(mdev_p->port_id, ENABLE);
	ADC_ConversionModeSelect(mdev_p->port_id, ADC_CONVERSION_CONTINUOUS);
	ADC_ResultWidthConfig(mdev_p->port_id, ADC_RESULT_WIDTH_16);
	/* FIXME: Verify usage for this API */
	ADC_SetScanChannel(mdev_p->port_id, ADC_SCAN_CHANNEL_0, channel);
	return mdev_p;
}

void adc_drv_tsensor_config(ADC_ID_Type adcID, mdev_t *mdev_p,
	ADC_TSensorMode_Type adcTSensorMode,
	ADC_InputMode_Type adcInputMode)
{
	if (!mdev_p) {
		ADC_LOG("Cannot configure, NULL mdev handle.\r\n");
		return;
	}
	ADC_TSensorConfig(adcID, adcTSensorMode);
	ADC_InputModeSelect(mdev_p->port_id, adcInputMode);
}

int adc_drv_result(mdev_t *mdev_p)
{
	int result = 0;
	ADC_IntClr((ADC_ID_Type)mdev_p->port_id, ADC_RDY);
	ADC_ConversionStart(mdev_p->port_id);
	while (ADC_GetStatus(mdev_p->port_id, ADC_STATUS_RDY) == RESET)
		;
	ADC_ConversionStop(mdev_p->port_id);
	result = ADC_GetConversionResult(mdev_p->port_id);
	return result;
}

void adc_drv_timeout(ADC_ID_Type id, uint32_t tout)
{
	mdev_t *mdev_p = mdev_get_handle(mdev_adc_name[id]);

	if (!mdev_p) {
		ADC_LOG("Unable to open device %s", mdev_adc_name[id]);
		return;
	}
	adc_tout[id] = tout;

}

static void adc_set_dma_config(mdev_t *mdev_p, dma_config_t *dmac,
				uint32_t addr, int num)
{
	DMA_CFG_Type *dmaConfigSet = &dmac->dma_cfg;

	dmaConfigSet->srcDmaAddr = (uint32_t) &ADC0->RESULT.WORDVAL;
	dmaConfigSet->destDmaAddr =  addr;
	dmaConfigSet->transfType = DMA_PER_TO_MEM;
	dmaConfigSet->burstLength = DMA_ITEM_16;
	dmaConfigSet->srcAddrInc = DMA_ADDR_NOCHANGE;
	dmaConfigSet->destAddrInc = DMA_ADDR_INC;
	dmaConfigSet->transfWidth = DMA_TRANSF_WIDTH_16;
	dmac->perDmaInter = DMA_PER24_ADC0;

	ADC_IntClr((ADC_ID_Type)mdev_p->port_id, ADC_RDY);
	ADC_DmaCmd((ADC_ID_Type)mdev_p->port_id, ENABLE);
	/* FIFO Threashold selection decides how many samples will be
	 * transferred on each DMA burst request. This value must equal
	 * DMA burst length, which in our case is 16 samples */
	ADC_DmaThresholdSel((ADC_ID_Type)mdev_p->port_id, ADC_DMA_THRESHOLD_16);
}

static void adc_reset_dma_config(mdev_t *mdev_p)
{
	/* Disable ADC - DMA */
	ADC_DmaCmd((ADC_ID_Type)mdev_p->port_id, DISABLE);
}

int adc_drv_get_samples(mdev_t *mdev_p, uint16_t *buf, int num)
{
	int ret = WM_SUCCESS;

	if (num < 0)
		return -WM_FAIL;

	ADC_LOG("samples to be transferred %d", num);

	dma_config_t dmac;
	mdev_t *dma_dev = dma_drv_open();
	if (dma_dev == NULL) {
		ADC_LOG("dma_drv_open failed");
		return -WM_FAIL;
	}
	/* Configure dma trasfer */
	adc_set_dma_config(mdev_p, &dmac, (uint32_t) buf, num);
	ADC_ConversionStart((ADC_ID_Type)mdev_p->port_id);

	while (num > 0) {
		/* transfer length in samples */
		dmac.dma_cfg.transfLength = (num > MAX_ADC_DMA_SIZE) ?
				MAX_ADC_DMA_SIZE : num;
		num -= dmac.dma_cfg.transfLength;
		/* transfer length in bytes */
		dmac.dma_cfg.transfLength *= DMA_TRANSF_WIDTH_16;

		if ((dma_drv_transfer(dma_dev, &dmac) != WM_SUCCESS) ||
				(dma_drv_wait_for_transfer_complete(dma_dev,
				OS_WAIT_FOREVER) != WM_SUCCESS)) {
			ret = -WM_FAIL;
			break;
		}
		dmac.dma_cfg.destDmaAddr += dmac.dma_cfg.transfLength;
	}

	ADC_ConversionStop((ADC_ID_Type)mdev_p->port_id);
	adc_reset_dma_config(mdev_p);
	dma_drv_close(dma_dev);

	return ret;
}

int adc_drv_close(mdev_t *mdev_p)
{
	ADC_ConversionStop(mdev_p->port_id);

	ADC_Disable(mdev_p->port_id);

	/* Mark adc<n> as free for use */
	return os_mutex_put(&adc_mutex[mdev_p->port_id]);
}

static void adc_drv_mdev_init(uint8_t adc_id)
{
	mdev_t *mdev_p;

	mdev_p = &mdev_adc[adc_id];

	mdev_p->name = mdev_adc_name[adc_id];

	mdev_p->port_id = adc_id;

	mdev_p->private_data = (uint32_t)&adcdrv_data[adc_id];
}

int adc_drv_init(ADC_ID_Type adc_id)
{
	/* Return if adc_id is invalid */
	if (adc_id < 0 || adc_id > MW300_ADC_NUM) {
		ADC_LOG("Port %d not enabled\r\n, port_id");
		return -WM_FAIL;
	}


	if (mdev_get_handle(mdev_adc_name[adc_id]) != NULL)
		return WM_SUCCESS;

	int ret = os_mutex_create(&adc_mutex[adc_id], mdev_adc_name[adc_id],
				      OS_MUTEX_INHERIT);
	if (ret == -WM_FAIL)
		return -WM_FAIL;

	/* Initialize the adc mdev structure */
	adc_drv_mdev_init(adc_id);

	/* set timeout for ADC conversion + dma transfer
	 * if no timeout is specified, default value is set such that
	 * no timeout will occur.
	 */
	if (adc_tout[adc_id] == 0)
		adc_tout[adc_id] = OS_WAIT_FOREVER;

	/* Register the adc device driver for this port */
	mdev_register(&mdev_adc[adc_id]);
	ADC_Init(ADC0_ID, &adcConfig);
	dma_drv_init();
	return WM_SUCCESS;
}

int adc_drv_selfcalib(mdev_t *dev, adc_calib_ref_type vref)
{
	if (dev == NULL) {
		ADC_LOG("Unable to open device\r\n");
		return -WM_FAIL;
	}
	/* Fixme: verify selfcalibration */
	int status = ADC_SelfCalibration(dev->port_id, vref);
	if (!status)
		return -WM_FAIL;

	return WM_SUCCESS;

}

void adc_drv_deinit(ADC_ID_Type adc_id)
{
	/* Return if adc_id is invalid */
	if (adc_id < 0 || adc_id > MW300_ADC_NUM) {
		ADC_LOG("Port %d not enabled\r\n, port_id");
		return;
	}
	/* Return if the device was not registered */
	mdev_t *dev = mdev_get_handle(mdev_adc_name[adc_id]);
	if (dev == NULL) {
		ADC_LOG("Device was not initialized\r\n");
		return;
	}
	/* Delete mutex and de-register the device */
	os_mutex_delete(&adc_mutex[adc_id]);
	/* Deregister mdev handle */
	mdev_deregister(dev->name);
}
