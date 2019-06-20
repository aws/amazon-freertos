/** @file mdev_dma.c
 *
 *  @brief This file provides  mdev driver for DMA
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

#include <board.h>
#include <mdev_dma.h>
#include <wmstdio.h>
#include <wm_os.h>
#include <lowlevel_drivers.h>
#include <wmlog.h>
#include <limits.h>
#include <wmassert.h>

#define str(x) #x
#define mdev_dma_sem_name(id) "DMA_SEM_"str(id)

/* #define CONFIG_DMA_FUNC_DEBUG */
#ifdef CONFIG_DMA_FUNC_DEBUG
#define dma_entry wmlog_entry
#define dma_exit wmlog_exit
#else
#define dma_entry(...)
#define dma_exit(...)
#endif

#define dma_e(...)				\
	wmlog_e("dma", ##__VA_ARGS__)

#ifdef CONFIG_DMA_DEBUG
#define dma_d(...)				\
	wmlog("dma", ##__VA_ARGS__)
#else
#define dma_d(...)
#endif				/*! CONFIG_DMA_DEBUG */

static bool dma_init_done;
static mdev_t mdev_dma;
static const char *mdev_dma_name = {MDEV_DMA};

typedef struct mdev_dma_priv_data {
	os_semaphore_t dma_ch_sem[NUM_DMA_CHANNELS];
	/* interrupt callback */
	dma_irq_cb dma_cb[NUM_DMA_CHANNELS];
	/* User's private pointer passed on to the callback \ref dma_irq_cb */
	void *data_cb[NUM_DMA_CHANNELS];
	/* channel error status */
	uint32_t dma_ch_error;
	/* channel busy status */
	uint32_t dma_ch_busy;
	os_mutex_t dma_mutex;
} mdev_dma_priv_data_t;

static mdev_dma_priv_data_t mdev_dma_p_data;

int dma_drv_set_cb(mdev_t *dev, dma_irq_cb dma_cb, void *data)
{
	int dma_ch = (int)dev - 1;
	if (((int) dma_ch < CHANNEL_0) || ((int) dev >= NUM_DMA_CHANNELS)) {
		dma_e("dma device handle pointer invalid\r\n");
		return -WM_E_INVAL;
	}

	mdev_dma_priv_data_t *p_data;
	p_data = (mdev_dma_priv_data_t *)(mdev_dma.private_data);

	os_mutex_get(&p_data->dma_mutex, OS_WAIT_FOREVER);
	p_data->dma_cb[dma_ch] = dma_cb;
	p_data->data_cb[dma_ch] = data;
	os_mutex_put(&p_data->dma_mutex);

	return WM_SUCCESS;
}

int dma_drv_init()
{
	dma_entry();
	if (dma_init_done)
		return WM_SUCCESS;
	mdev_t *mdev_p;

	mdev_p = &mdev_dma;

	mdev_p->name = mdev_dma_name;

	mdev_p->private_data = (uint32_t)&mdev_dma_p_data;

	int status = os_mutex_create(&mdev_dma_p_data.dma_mutex,
				"dma-mutex", OS_MUTEX_INHERIT);

	if (status != WM_SUCCESS)
		return status;

	dma_init_done = true;

	/* ENable DMA Interrupts at NVIC */
	NVIC_EnableIRQ(DMA_IRQn);
	NVIC_SetPriority(DMA_IRQn, 0xf);

	int ret = mdev_register(&mdev_dma);

	return (ret != 0) ? -WM_FAIL : WM_SUCCESS;
}

mdev_t *dma_drv_open()
{
	dma_entry();

	mdev_dma_priv_data_t *p_data;

	p_data = (mdev_dma_priv_data_t *)(mdev_dma.private_data);

	/* TODO: This should block the driver_close() function
	* of the respective driver when async operation will
	* be made */
	/* Take mutex before accessing any of the DMA regs */

	os_mutex_get(&p_data->dma_mutex, OS_WAIT_FOREVER);

	/* Select a channel; return NULL if no channel is available */
	int ch = CHANNEL_0;
	while (((p_data->dma_ch_busy >> ch) & 1) && ch < NUM_DMA_CHANNELS)
		ch++;
	if ((DMA_Channel_Type)ch == NUM_DMA_CHANNELS) {
		os_mutex_put(&p_data->dma_mutex);
		return NULL;
	}
	/* Update dma_ch_busy variable */
	p_data->dma_ch_busy |= (1 << ch);

	/* Handle interrupt masking */
	/* INT_BLK_TRANS_COMPLETE indicates BURST transfer complete
	 * MASK it to avoid interrupt at every burst transfer */
	DMA_IntMask(ch, INT_BLK_TRANS_COMPLETE, MASK);
	/* Unmask error interrupts and transfer complete interrupts */
	DMA_IntMask(ch, INT_DMA_TRANS_COMPLETE, UNMASK);
	DMA_IntMask(ch, INT_BUS_ERROR, UNMASK);
	DMA_IntMask(ch, INT_ADDRESS_ERROR, UNMASK);

	os_mutex_put(&p_data->dma_mutex);

	dma_d("DMA channel %d %x", ch, p_data->dma_ch_busy);

	/* Create channel based semaphore */
	int ret = os_semaphore_create(&p_data->dma_ch_sem[ch],
					mdev_dma_sem_name(ch));
	if (ret != WM_SUCCESS)
		return NULL;

	ret = os_semaphore_get(&p_data->dma_ch_sem[ch], OS_WAIT_FOREVER);
	if (ret != WM_SUCCESS)
		return NULL;

	/* return selected channel as mdev_t */
	return (mdev_t *)(ch + 1);
}

int dma_drv_transfer(mdev_t *dma, dma_config_t *dmac)
{
	dma_entry();

	ASSERT(dma != NULL && dmac != NULL);
	ASSERT(dmac->dma_cfg.transfLength <= DMA_MAX_CHUNK_SIZE);

	if (((dmac->dma_cfg.srcDmaAddr & 0x1f000000) == 0x1f000000) ||
		((dmac->dma_cfg.destDmaAddr & 0x1f000000) == 0x1f000000)) {
		dma_e("addr from memory mapped XIP region."
			" DMA not supported");
		return -WM_E_INVAL;
	}

	int dma_ch = (int)dma - 1;
	if ((dma_ch < CHANNEL_0) || (dma_ch >= NUM_DMA_CHANNELS))
		return -WM_E_INVAL;

	mdev_dma_priv_data_t *p_data;
	p_data = (mdev_dma_priv_data_t *)(mdev_dma.private_data);

	os_mutex_get(&p_data->dma_mutex, OS_WAIT_FOREVER);

	DMA_ChannelInit(dma_ch, &dmac->dma_cfg);
	/* Set peripheral type if involves peripheral */
	DMA_SetPeripheralType(dma_ch, dmac->perDmaInter);

	DMA_Enable(dma_ch);

	os_mutex_put(&p_data->dma_mutex);

	return WM_SUCCESS;
}

int dma_drv_wait_for_transfer_complete(mdev_t *dma, uint32_t timeout)
{
	ASSERT(dma != NULL);

	int dma_ch = (int)dma - 1;
	mdev_dma_priv_data_t *p_data;
	p_data = (mdev_dma_priv_data_t *)(mdev_dma.private_data);

	int ret = os_semaphore_get(&p_data->dma_ch_sem[dma_ch], timeout);

	/* Check if any DMA error has occurred */
	if (p_data->dma_ch_error && (1 << dma_ch)) {
		/* Reset error variable */
		p_data->dma_ch_error &= ~(1 << dma_ch);
		return -WM_FAIL;
	} else
		return (ret == WM_SUCCESS) ? ret : -WM_E_AGAIN;
}

int dma_drv_close(mdev_t *dma)
{
	dma_entry();

	int dma_ch = (int) dma - 1;

	if ((dma_ch < CHANNEL_0) || (dma_ch >= NUM_DMA_CHANNELS))
		return -WM_E_INVAL;

	mdev_dma_priv_data_t *p_data;
	p_data = (mdev_dma_priv_data_t *)(mdev_dma.private_data);

	/* De-register callback and private data pointer */
	p_data->dma_cb[dma_ch] = NULL;
	p_data->data_cb[dma_ch] = NULL;
	/* FIXME: mask all interrupts */
	os_semaphore_delete(&p_data->dma_ch_sem[dma_ch]);
	p_data->dma_ch_sem[dma_ch] = NULL;

	os_mutex_get(&p_data->dma_mutex, OS_WAIT_FOREVER);

	/* Ideally if DMA channel gets disabled
	 * on transfer complete. Software disables it
	 * just to make sure nothing goes wrong!! */
	DMA_Disable((DMA_Channel_Type)dma);

	/* Reset busy status & error status */
	p_data->dma_ch_busy &= ~(1 << dma_ch);
	p_data->dma_ch_error &= ~(1 << dma_ch);

	os_mutex_put(&p_data->dma_mutex);

	return WM_SUCCESS;
}

void dma_drv_deinit()
{
	/* TODO: Verify no channel is
	 * busy in dma_ch_en_status */
	/* NVIC Disable interrupts */
	NVIC_DisableIRQ(DMA_IRQn);
	os_mutex_delete(&mdev_dma_p_data.dma_mutex);
	mdev_deregister(mdev_dma.name);
}

void DMA_IRQHandler(void)
{
	int i = CHANNEL_0;

	uint32_t intStatusTfr, intStatusBusErr, intStatusAddrErr;

	/* Get current unmasked interrupt status */
	/* Block complete interrupt is not used for now */
	intStatusTfr = DMA->STATUS_TFRINT.WORDVAL;
	intStatusBusErr = DMA->STATUS_BUSERRINT.WORDVAL;
	intStatusAddrErr = DMA->STATUS_ADDRERRINT.WORDVAL;

	/* Clear the unmasked generated interrupts */
	DMA->STATUS_TFRINT.WORDVAL = 0xffffffff;
	DMA->STATUS_BLOCKINT.WORDVAL = 0xffffffff;
	DMA->STATUS_BUSERRINT.WORDVAL = 0xffffffff;
	DMA->STATUS_ADDRERRINT.WORDVAL = 0xffffffff;

	mdev_dma_priv_data_t *p_data;
	p_data = (mdev_dma_priv_data_t *)(mdev_dma.private_data);

	for (i = CHANNEL_0; i <= NUM_DMA_CHANNELS; i++) {
		if ((intStatusTfr >> i) & 1) {
			if (p_data->dma_cb[i]) {
				p_data->dma_cb[i](i, DMA_SUCCESS,
						p_data->data_cb[i]);
			} else if (p_data->dma_ch_sem[i])
				os_semaphore_put(&p_data->dma_ch_sem[i]);
		}
		if ((intStatusBusErr >> i) & 1 || (intStatusAddrErr >> i) & 1) {
			if (p_data->dma_cb[i])
				p_data->dma_cb[i](i, DMA_FAIL, p_data->data_cb);
			/* Release semaphore by reporting error */
			mdev_dma_p_data.dma_ch_error |= (1 << i);
			if (p_data->dma_ch_sem[i])
				os_semaphore_put(&p_data->dma_ch_sem[i]);
		}
	}
}

