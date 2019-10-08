/** @file mdev_ssp.c
 *
 *  @brief This file provides  mdev driver for SSP
 *
 *  (C) Copyright 2015-2019 Marvell International Ltd. All Rights Reserved.
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
 * mdev_ssp.c: mdev driver for SSP
 */

#include <lowlevel_drivers.h>
#include <board.h>
#include <mdev_ssp.h>
#include <mdev_dma.h>
#include <wmstdio.h>
#include <wm_os.h>

#define SSP_MAX_PORT 3

/* Threshold - Programmed value in register should be 1 less than desired */
#define SSP_FIFO_RX_THRESHOLD 0 /* RXFIFO = 1 */
#define SSP_FIFO_TX_THRESHOLD 0 /* TXFIFO = 1 */

#define SSP_DMA_FIFO_RX_THRESHOLD 7 /* RXFIFO = 8 */
#define SSP_DMA_FIFO_TX_THRESHOLD 7 /* TXFIFO = 8 */

#define SSP_RX_BUF_SIZE 2048
#define SSP_IRQn_BASE SSP0_IRQn
#define SSP_INT_BASE INT_SSP0
#define DEFAULT_SSP_FREQ 10000000
#define MIN_SSP_FREQ 57347	/* 128MHz / 72  / 31 */
#define MAX_SSP_FREQ 25000000	/* As per datasheet */

/* Maximum dma transfer length */
#define MAX_SSP_DMA_SIZE (DMA_MAX_CHUNK_SIZE / DMA_TRANSF_WIDTH_8)

static mdev_t mdev_ssp[SSP_MAX_PORT];
static os_semaphore_t ssp_sem[SSP_MAX_PORT];
typedef struct sspdrv_ringbuf {
	int wr_idx;		/* write pointer */
	int rd_idx;		/* read pointer */
	int buf_size;	/* number of bytes in buffer *buf_p */
	uint8_t *buf_p;		/* pointer to buffer storage */
} sspdrv_ringbuf_t;

typedef struct sspdev_priv_data {
	sspdrv_ringbuf_t rx_ringbuf;
	SSP_FrameFormat_Type device_type;
	unsigned int cs;
	uint32_t freq;
	/* Transmit timeout */
	uint32_t tx_tout;
	/* Receive timeout */
	uint32_t rx_tout;
	bool slave:1; /* 0 = Master and 1 = Slave */
	bool cs_level:1; /* 0 = Active low and 1 = Active high */
	bool dma:1; /* 0 = Disable and 1 = Enable */
	bool audiomode:1; /* 0 = Disable and 1 = Enable */
	SSP_DataSize_Type datasize;
	SSP_FrameFormat_Type format;
} sspdev_data_t;

static sspdev_data_t sspdev_data[SSP_MAX_PORT];
static void sspdrv_read_irq_handler(int port_id);
static void sspdrv_overrun_irq_handler(int port_id);

static void (*ssp_read_irq_handler[SSP_MAX_PORT])();
static void (*ssp_overrun_irq_handler[SSP_MAX_PORT])();

#define SET_RX_BUF_SIZE(ssp_data_p, size) ssp_data_p->rx_ringbuf.buf_size = size
#define GET_RX_BUF_SIZE(ssp_data_p) (ssp_data_p->rx_ringbuf.buf_size)

#define DEFINE_SSP_RD_CB(id) \
	static void \
	sspdrv ## id ## _ ## readirq_handler() {\
		sspdrv_ ## read_irq_handler(id); }

#define GET_SSP_RD_CB(id) sspdrv ## id ## _ ## readirq_handler

DEFINE_SSP_RD_CB(0)
DEFINE_SSP_RD_CB(1)
DEFINE_SSP_RD_CB(2)

#define DEFINE_SSP_OR_CB(id) \
	static void \
	sspdrv ## id ## _ ## overrunirq_handler() {\
		sspdrv_ ## overrun_irq_handler(id); }

#define GET_SSP_OR_CB(id) sspdrv ## id ## _ ## overrunirq_handler

DEFINE_SSP_OR_CB(0)
DEFINE_SSP_OR_CB(1)
DEFINE_SSP_OR_CB(2)

static void fill_cb_array()
{
	ssp_read_irq_handler[0] = GET_SSP_RD_CB(0);
	ssp_read_irq_handler[1] = GET_SSP_RD_CB(1);
	ssp_read_irq_handler[2] = GET_SSP_RD_CB(2);

	ssp_overrun_irq_handler[0] = GET_SSP_OR_CB(0);
	ssp_overrun_irq_handler[1] = GET_SSP_OR_CB(1);
	ssp_overrun_irq_handler[2] = GET_SSP_OR_CB(2);
}

/* ring buffer apis */

static uint32_t read_rx_buf(sspdev_data_t *ssp_data_p)
{
	uint32_t ret = 0;
	int idx, incr = 0;
	idx = ssp_data_p->rx_ringbuf.rd_idx;

	switch (ssp_data_p->datasize) {
	case SSP_DATASIZE_8:
		ret = *(uint8_t *)(ssp_data_p->rx_ringbuf.buf_p + idx);
		incr = 1;
		break;
	case SSP_DATASIZE_16:
		ret = *(uint16_t *)(ssp_data_p->rx_ringbuf.buf_p + idx);
		incr = 2;
		break;
	case SSP_DATASIZE_18:
	case SSP_DATASIZE_32:
		ret = *(uint32_t *)(ssp_data_p->rx_ringbuf.buf_p + idx);
		incr = 4;
		break;
	}
	ssp_data_p->rx_ringbuf.rd_idx = (ssp_data_p->rx_ringbuf.rd_idx + incr)
		% GET_RX_BUF_SIZE(ssp_data_p);
	return ret;
}

static void write_rx_buf(sspdev_data_t *ssp_data_p, uint32_t wr_char)
{
	int idx, incr  = 0;
	idx = ssp_data_p->rx_ringbuf.wr_idx;

	switch (ssp_data_p->datasize) {
	case SSP_DATASIZE_8:
		*(uint8_t *)(ssp_data_p->rx_ringbuf.buf_p + idx) =
			(uint8_t)wr_char;
		incr = 1;
		break;
	case SSP_DATASIZE_16:
		*(uint16_t *)(ssp_data_p->rx_ringbuf.buf_p + idx) =
			(uint16_t)wr_char;
		incr = 2;
		break;
	case SSP_DATASIZE_18:
	case SSP_DATASIZE_32:
		*(uint32_t *)(ssp_data_p->rx_ringbuf.buf_p + idx) =
			(uint32_t)wr_char;
		incr = 4;
		break;
	}
	ssp_data_p->rx_ringbuf.wr_idx = (ssp_data_p->rx_ringbuf.wr_idx + incr)
		% GET_RX_BUF_SIZE(ssp_data_p);
}

static bool is_rx_buf_empty(sspdev_data_t *ssp_data_p)
{
	int rd_idx, wr_idx;
	rd_idx = ssp_data_p->rx_ringbuf.rd_idx;
	wr_idx = ssp_data_p->rx_ringbuf.wr_idx;
	if (rd_idx == wr_idx) {
		return true;
	}
	return false;
}

static bool is_rx_buf_full(sspdev_data_t *ssp_data_p)
{
	int wr_idx, rd_idx, incr = 0;
	rd_idx = ssp_data_p->rx_ringbuf.rd_idx;
	wr_idx = ssp_data_p->rx_ringbuf.wr_idx;

	switch (ssp_data_p->datasize) {
	case SSP_DATASIZE_8:
		incr = 1;
		break;
	case SSP_DATASIZE_16:
		incr = 2;
		break;
	case SSP_DATASIZE_18:
	case SSP_DATASIZE_32:
		incr = 4;
		break;
	}
	if (((wr_idx + incr) % GET_RX_BUF_SIZE(ssp_data_p)) == rd_idx) {
		return true;
	}
	return false;
}

static int rx_buf_init(sspdev_data_t *ssp_data_p)
{
	ssp_data_p->rx_ringbuf.rd_idx = 0;
	ssp_data_p->rx_ringbuf.wr_idx = 0;

	/* Make sure ringbuffer size is multiple of datawidth */
	switch (ssp_data_p->datasize) {
	case SSP_DATASIZE_8:
		break;
	case SSP_DATASIZE_16:
		SET_RX_BUF_SIZE(ssp_data_p,
				(GET_RX_BUF_SIZE(ssp_data_p) & ~0x1));
		break;
	case SSP_DATASIZE_18:
	case SSP_DATASIZE_32:
		SET_RX_BUF_SIZE(ssp_data_p,
				(GET_RX_BUF_SIZE(ssp_data_p) & ~0x3));
		break;
	default:
		break;
	}

	ssp_data_p->rx_ringbuf.buf_p = os_mem_alloc
		(GET_RX_BUF_SIZE(ssp_data_p));
	if (!ssp_data_p->rx_ringbuf.buf_p)
		return -WM_FAIL;
	return WM_SUCCESS;
}

static void ssp_dma_get_rx_config(dma_config_t *dmac, uint32_t addr,
		int port_id, bool dummy_data, SSP_DataSize_Type datasize)
{
	DMA_CFG_Type *dma = &dmac->dma_cfg;

	switch (port_id) {
	case SSP0_ID:
		dma->srcDmaAddr = (uint32_t) &SSP0->SSDR.WORDVAL;
		dmac->perDmaInter = DMA_PER10_SSP0_RX;
		break;
	case SSP1_ID:
		dma->srcDmaAddr = (uint32_t) &SSP1->SSDR.WORDVAL;
		dmac->perDmaInter = DMA_PER12_SSP1_RX;
		break;
	case SSP2_ID:
		dma->srcDmaAddr = (uint32_t) &SSP2->SSDR.WORDVAL;
		dmac->perDmaInter = DMA_PER40_SSP2_RX;
		break;
	}

	/* DMA controller peripheral to memory configuration */
	dma->destDmaAddr = addr;
	dma->transfType = DMA_PER_TO_MEM;
	dma->burstLength = DMA_ITEM_8;
	dma->srcAddrInc = DMA_ADDR_NOCHANGE;
	if (!dummy_data)
		dma->destAddrInc = DMA_ADDR_INC;
	else
		dma->destAddrInc = DMA_ADDR_NOCHANGE;
	switch (datasize) {
	case SSP_DATASIZE_8:
		dma->transfWidth = DMA_TRANSF_WIDTH_8;
		break;
	case SSP_DATASIZE_16:
		dma->transfWidth = DMA_TRANSF_WIDTH_16;
		break;
	case SSP_DATASIZE_18:
	case SSP_DATASIZE_32:
		dma->transfWidth = DMA_TRANSF_WIDTH_32;
		break;
	}

}

static void ssp_dma_get_tx_config(dma_config_t *dmac, uint32_t addr,
		int port_id, bool dummy_data, SSP_DataSize_Type datasize)
{
	DMA_CFG_Type *dma = &dmac->dma_cfg;

	switch (port_id) {
	case SSP0_ID:
		dma->destDmaAddr = (uint32_t) &SSP0->SSDR.WORDVAL;
		dmac->perDmaInter = DMA_PER11_SSP0_TX;
		break;
	case SSP1_ID:
		dma->destDmaAddr = (uint32_t) &SSP1->SSDR.WORDVAL;
		dmac->perDmaInter = DMA_PER13_SSP1_TX;
		break;
	case SSP2_ID:
		dma->destDmaAddr = (uint32_t) &SSP2->SSDR.WORDVAL;
		dmac->perDmaInter = DMA_PER41_SSP2_TX;
		break;
	}

	/* DMA controller memory to peripheral configuration */
	dma->srcDmaAddr = addr;
	dma->transfType = DMA_MEM_TO_PER;
	dma->burstLength = DMA_ITEM_8;
	if (!dummy_data)
		dma->srcAddrInc = DMA_ADDR_INC;
	else
		dma->srcAddrInc = DMA_ADDR_NOCHANGE;
	dma->destAddrInc = DMA_ADDR_NOCHANGE;
	switch (datasize) {
	case SSP_DATASIZE_8:
		dma->transfWidth = DMA_TRANSF_WIDTH_8;
		break;
	case SSP_DATASIZE_16:
		dma->transfWidth = DMA_TRANSF_WIDTH_16;
		break;
	case SSP_DATASIZE_18:
	case SSP_DATASIZE_32:
		dma->transfWidth = DMA_TRANSF_WIDTH_32;
		break;
	}
}

/* Device name */
static const char *mdev_ssp_name[SSP_MAX_PORT] = {
	"MDEV_SSP0",
	"MDEV_SSP1",
	"MDEV_SSP2"
};

/* FIXME: As of now, there is no software fifo (ringbuffer) in case of
 * slave dma rx. Problem is the transfer size is unknown for slave receive.
 * One solution is to provide the parameter at open time. Also need to take
 * care of case when we will expose both dma and pio read/write API's
 * separately.
 */
static int ssp_drv_dma_read(mdev_t *dev, uint8_t *data, int num)
{
	dma_config_t dmac_tx, dmac_rx;
	mdev_t *dma_rx_dev = NULL, *dma_tx_dev = NULL;
	uint32_t dummy;
	uint16_t dma_trans = MAX_SSP_DMA_SIZE;
	int ret = num;
	sspdev_data_t *ssp_data_p;

	dma_drv_init();

	/* Open dma for rx channel */
	dma_rx_dev = dma_drv_open();
	if (dma_rx_dev == NULL) {
		SSP_LOG("dma_drv_open for rx failed");
		return -WM_FAIL;
	}

	ssp_data_p = (sspdev_data_t *) dev->private_data;
	ssp_dma_get_rx_config(&dmac_rx, (uint32_t) data, dev->port_id, 0,
				ssp_data_p->datasize);

	switch (ssp_data_p->datasize) {
	case SSP_DATASIZE_8:
		dma_trans = MAX_SSP_DMA_SIZE;
		break;
	case SSP_DATASIZE_16:
		dma_trans = MAX_SSP_DMA_SIZE & ~0x1;
		break;
	case SSP_DATASIZE_18:
	case SSP_DATASIZE_32:
		dma_trans = MAX_SSP_DMA_SIZE & ~0x3;
		break;
	}

	/* In case of master, reading data requires writing dummy data
	 * on the bus, which will generate the clock for slave to
	 * send data
	 */
	dummy = SPI_DUMMY_WORD;

	/* Open dma for tx channel if master */
	if (ssp_data_p->slave == SSP_MASTER) {
		dma_tx_dev = dma_drv_open();
		if (dma_tx_dev == NULL) {
			SSP_LOG("dma_drv_open for tx failed");
			dma_drv_close(dma_rx_dev);
			return -WM_FAIL;
		}
		ssp_dma_get_tx_config(&dmac_tx, (uint32_t) &dummy,
					dev->port_id, 1, ssp_data_p->datasize);
	}

	while (num > 0) {
		if (num < dma_trans)
			dma_trans = num;
		if (ssp_data_p->slave == SSP_SLAVE) {

			/* Start transfer on RX channel*/
			dmac_rx.dma_cfg.transfLength = dma_trans;
			if (dma_drv_transfer(dma_rx_dev, &dmac_rx)
					!= WM_SUCCESS) {
				ret = -WM_FAIL;
				break;
			}

			/* Wait for dma transfer to complete */
			dummy = dma_drv_wait_for_transfer_complete
					(dma_rx_dev, ssp_data_p->rx_tout);
			if (dummy == -WM_E_AGAIN) {
				ret = ret - num;
				break;
			} else if (dummy != WM_SUCCESS) {
				ret = -WM_FAIL;
				break;
			}
		} else {
			/* Start transfer on RX channel*/
			dmac_rx.dma_cfg.transfLength = dma_trans;
			if (dma_drv_transfer(dma_rx_dev, &dmac_rx)
					!= WM_SUCCESS) {
				ret = -WM_FAIL;
				break;
			}
			/* Start transfer on TX channel*/
			dmac_tx.dma_cfg.transfLength = dma_trans;
			if (dma_drv_transfer(dma_tx_dev, &dmac_tx)
					!= WM_SUCCESS) {
				ret = -WM_FAIL;
				break;
			}

			/* Wait for dma transfer to complete on RX channel */
			dummy = dma_drv_wait_for_transfer_complete
					(dma_rx_dev, ssp_data_p->rx_tout);
			if (dummy == -WM_E_AGAIN) {
				ret = ret - num;
				break;
			} else if (dummy != WM_SUCCESS) {
				ret = -WM_FAIL;
				break;
			}
			/* Wait for dma transfer to complete on TX channel */
			dummy = dma_drv_wait_for_transfer_complete
					(dma_tx_dev, ssp_data_p->tx_tout);
			if (dummy == -WM_E_AGAIN) {
				ret = ret - num;
				break;
			} else if (dummy != WM_SUCCESS) {
				ret = -WM_FAIL;
				break;
			}
		}
		/* Increment destination address for rx channel */
		dmac_rx.dma_cfg.destDmaAddr += dma_trans;
		num -= dma_trans;
	}

	dma_drv_close(dma_rx_dev);
	if (dma_tx_dev != NULL)
		dma_drv_close(dma_tx_dev);

	return ret;
}

static int ssp_drv_dma_write(mdev_t *dev, const uint8_t *data, uint8_t *din,
		uint32_t num, bool flag)
{
	dma_config_t dmac_tx, dmac_rx;
	mdev_t *dma_rx_dev = NULL, *dma_tx_dev = NULL;
	uint32_t dummy;
	uint16_t dma_trans = MAX_SSP_DMA_SIZE;
	int ret = num;
	sspdev_data_t *ssp_data_p;

	dma_drv_init();

	/* Open dma for tx channel */
	dma_tx_dev = dma_drv_open();
	if (dma_tx_dev == NULL) {
		SSP_LOG("dma_drv_open for tx failed");
		return -WM_FAIL;
	}

	ssp_data_p = (sspdev_data_t *) dev->private_data;
	ssp_dma_get_tx_config(&dmac_tx, (uint32_t) data, dev->port_id, 0,
				ssp_data_p->datasize);

	switch (ssp_data_p->datasize) {
	case SSP_DATASIZE_8:
		dma_trans = MAX_SSP_DMA_SIZE;
		break;
	case SSP_DATASIZE_16:
		dma_trans = MAX_SSP_DMA_SIZE & ~0x1;
		break;
	case SSP_DATASIZE_18:
	case SSP_DATASIZE_32:
		dma_trans = MAX_SSP_DMA_SIZE & ~0x3;
		break;
	}

	/* Open dma for rx channel if read requested */
	if (flag) {
		dma_rx_dev = dma_drv_open();
		if (dma_rx_dev == NULL) {
			SSP_LOG("dma_drv_open for rx failed");
			dma_drv_close(dma_tx_dev);
			return -WM_FAIL;
		}
		if (din)
			ssp_dma_get_rx_config(&dmac_rx, (uint32_t) din,
				dev->port_id, 0, ssp_data_p->datasize);
		else
			ssp_dma_get_rx_config(&dmac_rx, (uint32_t) &dummy,
				dev->port_id, 1, ssp_data_p->datasize);
	}

	while (num > 0) {
		if (num < dma_trans)
			dma_trans = num;

		if (flag) {
			/* Start transfer on RX channel first*/
			dmac_rx.dma_cfg.transfLength = dma_trans;
			if (dma_drv_transfer(dma_rx_dev, &dmac_rx)
					!= WM_SUCCESS) {
				ret = -WM_FAIL;
				break;
			}
		}

		/* Start transfer on TX channel*/
		dmac_tx.dma_cfg.transfLength = dma_trans;
		if (dma_drv_transfer(dma_tx_dev, &dmac_tx) != WM_SUCCESS) {
			ret = -WM_FAIL;
			break;
		}

		/* Wait for dma transfer to complete on TX channel */
		dummy = dma_drv_wait_for_transfer_complete(dma_tx_dev,
				ssp_data_p->tx_tout);
		if (dummy == -WM_E_AGAIN) {
			ret = ret - num;
			break;
		} else if (dummy != WM_SUCCESS) {
			ret = -WM_FAIL;
			break;
		}

		if (flag) {
			/* Wait for dma transfer to complete on RX channel */
			dummy = dma_drv_wait_for_transfer_complete
					(dma_rx_dev, ssp_data_p->rx_tout);
			if (dummy == -WM_E_AGAIN) {
				ret = ret - num;
				break;
			} else if (dummy != WM_SUCCESS) {
				ret = -WM_FAIL;
				break;
			}
		}

		/* Increment source address for tx channel */
		dmac_tx.dma_cfg.srcDmaAddr += dma_trans;
		if (din)
			/* Increment destination address for rx channel */
			dmac_rx.dma_cfg.destDmaAddr += dma_trans;
		num -= dma_trans;
	}

	dma_drv_close(dma_tx_dev);
	if (dma_rx_dev != NULL)
		dma_drv_close(dma_rx_dev);

	return ret;
}

static int ssp_drv_pio_write(mdev_t *dev, const uint8_t *data, uint8_t *din,
		uint32_t num, bool flag)
{
	uint32_t len = 0, incr = 0;
	uint32_t size = num;

	sspdev_data_t *ssp_data_p;
	ssp_data_p = (sspdev_data_t *) dev->private_data;

	while (num > 0) {
		/* Wait if fifo is full */
		while (SSP_GetStatus(dev->port_id, SSP_STATUS_TFNF) != SET)
			os_thread_relinquish();

		switch (ssp_data_p->datasize) {
		case SSP_DATASIZE_8:
			SSP_SendData(dev->port_id, *(uint8_t *)data);
			incr = 1;
			break;
		case SSP_DATASIZE_16:
			SSP_SendData(dev->port_id, *(uint16_t *)data);
			incr = 2;
			break;
		case SSP_DATASIZE_18:
		case SSP_DATASIZE_32:
			SSP_SendData(dev->port_id, *(uint32_t *)data);
			incr = 4;
			break;
		}
		data += incr;
		num -= incr;
		/* Enable read RXFIFO during write operation if flag is set */
		if (flag) {

			/* read data while write operation is on to enable
			 * full duplex support in SPI protocol*/
			/*
			 * Read rxfifo and store data, provided valid din
			 * buffer is provided by user else discard it.
			 */

			if (ssp_data_p->slave == SSP_MASTER) {

				/* SSP_STATUS_BIT == SET when SSPx port is
				 * currently transmitting or receiving framed
				 * data.
				 * */
				while (SSP_GetStatus(dev->port_id,
						     SSP_STATUS_BUSY) == SET)
					os_thread_relinquish();

				/*  SSP_STATUS_RFNE == SET when RXFIFO is
				 *  non-empty
				 */
				while (SSP_GetStatus(dev->port_id,
						     SSP_STATUS_RFNE) != SET)
					os_thread_relinquish();

				if (din) {
					switch (ssp_data_p->datasize) {
					case SSP_DATASIZE_8:
						*(uint8_t *)din = (uint8_t)
							SSP_RecvData(
								dev->port_id);
						break;
					case SSP_DATASIZE_16:
						*(uint16_t *)din = (uint16_t)
							SSP_RecvData(
								dev->port_id);
						break;
					case SSP_DATASIZE_18:
					case SSP_DATASIZE_32:
						*(uint32_t *)din = (uint32_t)
							SSP_RecvData(
								dev->port_id);
						break;
					}
					din += incr;
				}
				else
					SSP_RecvData(dev->port_id);
			} else {
				/* if slave mode is enabled read from software
				 * fifo. We do not block on data just yet, but
				 * continue writing. This ensures that when
				 * master starts the clock, there is enough data
				 * to send in slave tx fifo. We come back
				 * to pending data read after write is over */
				if (is_rx_buf_empty(ssp_data_p) == true)
					continue;
				if (din) {
					switch (ssp_data_p->datasize) {
					case SSP_DATASIZE_8:
						*(uint8_t *)din = (uint8_t)
							read_rx_buf(ssp_data_p);
						break;
					case SSP_DATASIZE_16:
						*(uint16_t *)din = (uint16_t)
							read_rx_buf(ssp_data_p);
						break;
					case SSP_DATASIZE_18:
					case SSP_DATASIZE_32:
						*(uint32_t *)din = (uint32_t)
							read_rx_buf(ssp_data_p);
						break;
					}
					din += incr;
				}
				else
					read_rx_buf(ssp_data_p);
			}
		}
		len += incr;
	}

	/* Return if master as master can receive data immediately.
	 * Also return if read is not requested by slave.
	 */
	if (ssp_data_p->slave == SSP_MASTER || !flag)
		return len;

	/* Read pending data in case of slave */
	while (len < size) {
		while (is_rx_buf_empty(ssp_data_p) == true)
			os_thread_relinquish();
		if (din) {
			switch (ssp_data_p->datasize) {
			case SSP_DATASIZE_8:
				*(uint8_t *)din = (uint8_t)
					read_rx_buf(ssp_data_p);
				break;
			case SSP_DATASIZE_16:
				*(uint16_t *)din = (uint16_t)
					read_rx_buf(ssp_data_p);
				break;
			case SSP_DATASIZE_18:
			case SSP_DATASIZE_32:
				*(uint32_t *)din = (uint32_t)
					read_rx_buf(ssp_data_p);
				break;
			}
			din += incr;
		}
		else
			read_rx_buf(ssp_data_p);
		len += incr;
	}

	return len;
}

static int ssp_drv_pio_read(mdev_t *dev, uint8_t *data, uint32_t num)
{
	int len = 0, incr = 0;
	sspdev_data_t *ssp_data_p;

	ssp_data_p = (sspdev_data_t *) dev->private_data;

	if (ssp_data_p->slave == SSP_SLAVE) {
		while (num > 0) {
			if (!is_rx_buf_empty(ssp_data_p))
				switch (ssp_data_p->datasize) {
				case SSP_DATASIZE_8:
					*(uint8_t *)data = (uint8_t)
						read_rx_buf(ssp_data_p);
					incr = 1;
					break;
				case SSP_DATASIZE_16:
					*(uint16_t *)data = (uint16_t)
						read_rx_buf(ssp_data_p);
					incr = 2;
					break;
				case SSP_DATASIZE_18:
				case SSP_DATASIZE_32:
					*(uint32_t *)data = (uint32_t)
						read_rx_buf(ssp_data_p);
					incr = 4;
					break;
				}
			else
				break;
			data += incr;
			len += incr;
			num -= incr;
		}
	} else {
		/* In master mode, first send dummy data in TX line and read
		 * simultaneously. If nothing is on RX line then it can read
		 * 0x0 or 0xff. Application should check whether data is
		 * vaild or not.
		*/
		while (num > 0) {
			while (SSP_GetStatus(dev->port_id, SSP_STATUS_TFNF)
				!= SET)
				os_thread_relinquish();

			SSP_SendData(dev->port_id, SPI_DUMMY_WORD);
			while (SSP_GetStatus(dev->port_id, SSP_STATUS_RFNE)
				!= SET)
				os_thread_relinquish();
			switch (ssp_data_p->datasize) {
			case SSP_DATASIZE_8:
				*(uint8_t *)data = (uint8_t)
					SSP_RecvData(dev->port_id);
				incr = 1;
				break;
			case SSP_DATASIZE_16:
				*(uint16_t *)data = (uint16_t)
					SSP_RecvData(dev->port_id);
				incr = 2;
				break;
			case SSP_DATASIZE_18:
			case SSP_DATASIZE_32:
				*(uint32_t *)data = (uint32_t)
					SSP_RecvData(dev->port_id);
				incr = 4;
				break;
			}
			data += incr;
			len += incr;
			num -= incr;
		}
	}
	return len;
}

int ssp_drv_write(mdev_t *dev, const uint8_t *data, uint8_t *din, uint32_t num,
		bool flag)
{
	sspdev_data_t *ssp_data_p;
	ssp_data_p = (sspdev_data_t *) dev->private_data;

	switch (ssp_data_p->datasize) {
	case SSP_DATASIZE_8:
		break;
	case SSP_DATASIZE_16:
		if (num % 2) {
			SSP_LOG("Buffer length is not a multiple of 2\r\n");
			return -WM_E_INVAL;
		}
		if ((uint32_t)data & 0x1U || (uint32_t)din & 0x1U) {
			SSP_LOG("Buffer is not half-word aligned\r\n");
			return -WM_E_INVAL;
		}
		break;
	case SSP_DATASIZE_18:
	case SSP_DATASIZE_32:
		if (num % 4) {
			SSP_LOG("Buffer length is not a multiple of 4\r\n");
			return -WM_E_INVAL;
		}
		if ((uint32_t)data & 0x3U || (uint32_t)din & 0x3U) {
			SSP_LOG("Buffer is not word aligned\r\n");
			return -WM_E_INVAL;
		}
		break;
	}

	if (ssp_data_p->dma)
		return ssp_drv_dma_write(dev, data, din, num, flag);
	else
		return ssp_drv_pio_write(dev, data, din, num, flag);
}

int ssp_drv_read(mdev_t *dev, uint8_t *data, uint32_t num)
{
	sspdev_data_t *ssp_data_p;
	ssp_data_p = (sspdev_data_t *) dev->private_data;

	switch (ssp_data_p->datasize) {
	case SSP_DATASIZE_8:
		break;
	case SSP_DATASIZE_16:
		if (num % 2) {
			SSP_LOG("Buffer length is not a multiple of 2\r\n");
			return -WM_E_INVAL;
		}
		if ((uint32_t)data & 0x1U) {
			SSP_LOG("Buffer is not half-word aligned\r\n");
			return -WM_E_INVAL;
		}
		break;
	case SSP_DATASIZE_18:
	case SSP_DATASIZE_32:
		if (num % 4) {
			SSP_LOG("Buffer length is not a multiple of 4\r\n");
			return -WM_E_INVAL;
		}
		if ((uint32_t)data & 0x3U) {
			SSP_LOG("Buffer is not word aligned\r\n");
			return -WM_E_INVAL;
		}
		break;
	}

	if (ssp_data_p->dma)
		return ssp_drv_dma_read(dev, data, num);
	else
		return ssp_drv_pio_read(dev, data, num);
}

static void sspdrv_read_irq_handler(int port_num)
{
	uint32_t data;
	mdev_t *dev = &mdev_ssp[port_num];
	sspdev_data_t *ssp_data_p;

	ssp_data_p = (sspdev_data_t *) dev->private_data;
	while (SSP_GetStatus(dev->port_id, SSP_STATUS_RFNE)
			== SET) {
		/* Read data from HW-FIFO to user allocated buffer
		 * if buffer is not full */
		if (!is_rx_buf_full(ssp_data_p)) {
			data = SSP_RecvData(dev->port_id);
			write_rx_buf(ssp_data_p, data);
		} else {
			break;
		}
	}

}

/* FIXME: Add mechanism to communicate error conditions to user */
static void sspdrv_overrun_irq_handler(int port_num)
{
	mdev_t *dev = &mdev_ssp[port_num];

	SSP_LOG("FIFO overrun\r\n");
	SSP_IntClr(dev->port_id , SSP_INT_RFORI);

	/* Empty entire FIFO when FIFO overrun is detected */
	while (SSP_GetStatus(dev->port_id, SSP_STATUS_RFNE)
			== SET)
		SSP_RecvData(dev->port_id);
}

void ssp_drv_cs_configure(mdev_t *dev, int cs, bool level)
{
	sspdev_data_t *ssp_data_p;
	ssp_data_p = (sspdev_data_t *) dev->private_data;
	/* Configure the pinmux for ssp pins */
	if (cs >= 0 && ssp_data_p->slave == SSP_MASTER) {
		board_ssp_pin_config(dev->port_id, 0);
		/* Use user specified chip select pin */
		ssp_data_p->cs = cs;
		ssp_data_p->cs_level = level;
		GPIO_PinMuxFun(cs, PINMUX_FUNCTION_0);
		GPIO_SetPinDir(cs, GPIO_OUTPUT);
		/* Initially keep slave de-selected */
		GPIO_WritePinOutput(cs, !level);
	} else {
		board_ssp_pin_config(dev->port_id, 1);
		ssp_data_p->cs = 0;
		ssp_data_p->cs_level = 0;
	}
}

void ssp_drv_cs_activate(mdev_t *dev)
{
	sspdev_data_t *ssp_data_p;
	ssp_data_p = (sspdev_data_t *) dev->private_data;
	GPIO_WritePinOutput(ssp_data_p->cs, ssp_data_p->cs_level);
}

void ssp_drv_cs_deactivate(mdev_t *dev)
{
	sspdev_data_t *ssp_data_p;
	ssp_data_p = (sspdev_data_t *) dev->private_data;
	GPIO_WritePinOutput(ssp_data_p->cs, !ssp_data_p->cs_level);
}

int ssp_drv_network_config(mdev_t *dev,
			   SSP_CHXFR txTimeSlots,
			   SSP_CHXFR rxTimeSlots)
{
	SSP_NWK_Type sspNetworkCfg;
	sspdev_data_t *ssp_data_p;
	ssp_data_p = (sspdev_data_t *) dev->private_data;

	/* This API call only applicable for PSP Mode */
	if ((!dev) || (ssp_data_p->format != SSP_FRAME_PSP))
		return -1;

	sspNetworkCfg.frameRateDiv = 1;
	if (!ssp_data_p->audiomode) {
		sspNetworkCfg.txTimeSlotActive = CHXFR_BOTH;
		sspNetworkCfg.rxTimeSlotActive = CHXFR_BOTH;
	} else {
		sspNetworkCfg.txTimeSlotActive = txTimeSlots;
		sspNetworkCfg.rxTimeSlotActive = rxTimeSlots;
	}
	SSP_NwkConfig(dev->port_id, &sspNetworkCfg);
	return 0;
}

mdev_t *ssp_drv_open(SSP_ID_Type ssp_id, SSP_FrameFormat_Type format,
		SSP_MS_Type mode, SSP_DMA dma, int cs, bool level)
{
	int ret = 0;
	SSP_CFG_Type sspCfgStruct;
	SSP_FIFO_Type sspFifoCfg;
	SPI_Param_Type spiParaStruct;
	PSP_Param_Type pspParaStruct;
	sspdev_data_t *ssp_data_p;
	mdev_t *mdev_p = mdev_get_handle(mdev_ssp_name[ssp_id]);
	ssp_data_p = (sspdev_data_t *) mdev_p->private_data;

	if (mdev_p == NULL) {
		SSP_LOG("Unable to open device %s\r\n",
			mdev_ssp_name[ssp_id]);
		return NULL;
	}
	ssp_data_p->slave = mode;
	ret = os_semaphore_get(&ssp_sem[mdev_p->port_id], OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		SSP_LOG("failed to get semaphore\r\n");
		return NULL;
	}
	/* If ringbuffer size is not set by user then set to default size */
	if (GET_RX_BUF_SIZE(ssp_data_p) == 0) {
		SET_RX_BUF_SIZE(ssp_data_p, SSP_RX_BUF_SIZE);
	}
	/* Allocate ring buffer only for slave when dma is disabled*/
	if (mode == SSP_SLAVE && dma == DMA_DISABLE)
		if (rx_buf_init(ssp_data_p)) {
			SSP_LOG("Unable to allocate ssp software"
					" ring buffer\r\n");
			return NULL;
		}

	/* If clk is not set by user then set it to default */
	if (ssp_data_p->freq == 0) {
		ret = ssp_drv_set_clk(mdev_p->port_id, DEFAULT_SSP_FREQ);
	}

	/* Configure the pinmux for ssp pins */
	if (cs >= 0 && mode == SSP_MASTER) {
		board_ssp_pin_config(mdev_p->port_id, 0);
		/* Use user specified chip select pin */
		ssp_data_p->cs = cs;
		ssp_data_p->cs_level = level;
		GPIO_PinMuxFun(cs, PINMUX_FUNCTION_0);
		GPIO_SetPinDir(cs, GPIO_OUTPUT);
		/* Initially keep slave de-selected */
		GPIO_WritePinOutput(cs, !level);
	} else {
		board_ssp_pin_config(mdev_p->port_id, 1);
		ssp_data_p->cs = 0;
		ssp_data_p->cs_level = 0;
	}

	/* Configure SSP interface */
	sspCfgStruct.mode = SSP_NORMAL;
	sspCfgStruct.masterOrSlave = mode;
	sspCfgStruct.trMode = SSP_TR_MODE;
	sspCfgStruct.dataSize = ssp_data_p->datasize;
	sspCfgStruct.sfrmPola = SSP_SAMEFRM_PSP;
	sspCfgStruct.slaveClkRunning = SSP_SLAVECLK_TRANSFER;
	sspCfgStruct.txd3StateEnable = ENABLE;
	/* RXFIFO inactivity timeout, [timeout = 100 / 26MHz] (Bus clock) */
	sspCfgStruct.timeOutVal = 100;

	ssp_data_p->format = format;
	switch (format) {
	case SSP_FRAME_SPI:
		sspCfgStruct.frameFormat = SSP_FRAME_SPI;
		sspCfgStruct.txd3StateType = SSP_TXD3STATE_ELSB;
		break;
	case SSP_FRAME_PSP:
		sspCfgStruct.mode = SSP_NETWORK;
		sspCfgStruct.frameFormat = SSP_FRAME_PSP;
		sspCfgStruct.txd3StateType = SSP_TXD3STATE_12SLSB;
		break;
	case SSP_FRAME_SSP:
		SSP_LOG("Frame Format not implemented.\r\n");
		return NULL;
	}

	/* Configure SSP Fifo */
	sspFifoCfg.fifoPackMode =  DISABLE;

	/* See if dma needs to be enabled */
	if (dma == DMA_ENABLE) {
		/* Enable DMA controller clock */
		CLK_ModuleClkEnable(CLK_DMA);
		sspFifoCfg.rxFifoFullLevel = SSP_DMA_FIFO_RX_THRESHOLD;
		sspFifoCfg.txFifoEmptyLevel = SSP_DMA_FIFO_TX_THRESHOLD;
		sspFifoCfg.rxDmaService = ENABLE;
		sspFifoCfg.txDmaService = ENABLE;
		ssp_data_p->dma = 1;
	} else {
		sspFifoCfg.rxFifoFullLevel = SSP_FIFO_RX_THRESHOLD;
		sspFifoCfg.txFifoEmptyLevel = SSP_FIFO_TX_THRESHOLD;
		sspFifoCfg.rxDmaService = DISABLE;
		sspFifoCfg.txDmaService = DISABLE;
		ssp_data_p->dma = 0;
	}

	/* Let the settings take effect */
	SSP_Disable(mdev_p->port_id);
	SSP_Init(mdev_p->port_id, &sspCfgStruct);
	SSP_FifoConfig(mdev_p->port_id, &sspFifoCfg);

	/* Do frame format config */
	switch (format) {
	case SSP_FRAME_SPI:
		spiParaStruct.spiClkPhase = SPI_SCPHA_1;
		spiParaStruct.spiClkPolarity = SPI_SCPOL_LOW;
		SPI_Config(mdev_p->port_id, &spiParaStruct);
		break;
	case SSP_FRAME_PSP:
		pspParaStruct.pspFsrtType = PSP_AFTER_DMYSTOP;
		pspParaStruct.pspClkMode = PSP_DRIVFALL_SAMPRISE_IDLELOW;
		pspParaStruct.pspFrmPola = PSP_SFRMP_LOW;
		pspParaStruct.pspEndTransState = PSP_ENDTRANS_LOW;
		pspParaStruct.startDelay = 0;
		pspParaStruct.dummyStart = 0;
		pspParaStruct.dummyStop = 0;
		pspParaStruct.frmDelay = 0;
		if (!ssp_data_p->audiomode) {
			pspParaStruct.frmLength = 8;
		} else {
			pspParaStruct.frmLength = 16;
			pspParaStruct.pspFsrtType = PSP_WITH_LSB;
		}
		PSP_Config(mdev_p->port_id, &pspParaStruct);
		ssp_drv_network_config(mdev_p, CHXFR_BOTH, CHXFR_BOTH);
		break;
	case SSP_FRAME_SSP:
		SSP_LOG("Frame Format not implemented.\r\n");
		return NULL;
	}

	/* Enable read interrupts only for slave when dma is disabled*/
	if (mode == SSP_SLAVE && dma == DMA_DISABLE) {
		install_int_callback(SSP_INT_BASE + mdev_p->port_id,
				     SSP_INT_RFFI,
				     ssp_read_irq_handler[mdev_p->port_id]);
		install_int_callback(SSP_INT_BASE + mdev_p->port_id,
				     SSP_INT_RFORI,
				     ssp_overrun_irq_handler[mdev_p->port_id]);
		NVIC_EnableIRQ(SSP_IRQn_BASE + mdev_p->port_id);
		NVIC_SetPriority(SSP_IRQn_BASE + mdev_p->port_id, 0xF);
		SSP_IntMask(mdev_p->port_id, SSP_INT_RFFI, UNMASK);
	}

	SSP_Enable(mdev_p->port_id);
	return mdev_p;
}

int ssp_drv_close(mdev_t *dev)
{
	sspdev_data_t *ssp_data_p;
	ssp_data_p = (sspdev_data_t *) dev->private_data;
	os_mem_free(ssp_data_p->rx_ringbuf.buf_p);
	SSP_Disable(dev->port_id);

	/* Reset to default values */
	ssp_data_p->dma = DMA_DISABLE;

	SET_RX_BUF_SIZE(ssp_data_p, 0);
	ssp_data_p->freq = 0;
	ssp_data_p->datasize = SSP_DATASIZE_8;
	ssp_data_p->tx_tout = OS_WAIT_FOREVER;
	ssp_data_p->rx_tout = OS_WAIT_FOREVER;

	return os_semaphore_put(&ssp_sem[dev->port_id]);
}

static void ssp_drv_mdev_init(uint8_t port_num)
{
	mdev_t *mdev_p;
	sspdev_data_t *ssp_data_p;

	mdev_p = &mdev_ssp[port_num];
	mdev_p->name = mdev_ssp_name[port_num];
	mdev_p->port_id = port_num;
	mdev_p->private_data = (uint32_t)&sspdev_data[port_num];
	ssp_data_p = (sspdev_data_t *) mdev_p->private_data;
	SET_RX_BUF_SIZE(ssp_data_p, 0);
	ssp_data_p->freq = 0;
	ssp_data_p->datasize = SSP_DATASIZE_8;

	/* Set default tx/rx timeout */
	ssp_data_p->tx_tout = OS_WAIT_FOREVER;
	ssp_data_p->rx_tout = OS_WAIT_FOREVER;

}

int ssp_drv_datasize(SSP_ID_Type ssp_id, SSP_DataSize_Type dataSize)
{
	mdev_t *mdev_p;
	sspdev_data_t *ssp_data_p;

	if (!IS_SSP_PERIPH(ssp_id) || dataSize > SSP_DATASIZE_32)
		return -WM_FAIL;
	/* Check ssp_id is initialized */
	mdev_p = mdev_get_handle(mdev_ssp_name[ssp_id]);
	if (!mdev_p) {
		SSP_LOG("Unable to open device %s\r\n",
			mdev_ssp_name[ssp_id]);
		return -WM_FAIL;
	}
	ssp_data_p = (sspdev_data_t *) mdev_p->private_data;
	ssp_data_p->datasize = dataSize;
	return WM_SUCCESS;
}

int ssp_drv_audiomode(SSP_ID_Type ssp_id, FunctionalState amode)
{
	mdev_t *mdev_p;
	sspdev_data_t *ssp_data_p;

	if (!IS_SSP_PERIPH(ssp_id))
		return -WM_FAIL;
	/* Check ssp_id is initialized */
	mdev_p = mdev_get_handle(mdev_ssp_name[ssp_id]);
	if (!mdev_p) {
		SSP_LOG("Unable to open device %s\r\n",
			mdev_ssp_name[ssp_id]);
		return -WM_FAIL;
	}
	ssp_data_p = (sspdev_data_t *) mdev_p->private_data;
	ssp_data_p->audiomode = amode;
	return WM_SUCCESS;
}


int ssp_drv_rxbuf_size(SSP_ID_Type ssp_id, uint32_t size)
{
	mdev_t *mdev_p;
	sspdev_data_t *ssp_data_p;

	if (!IS_SSP_PERIPH(ssp_id) || size < 0)
		return -WM_FAIL;
	/* Check ssp_id is initialized */
	mdev_p = mdev_get_handle(mdev_ssp_name[ssp_id]);
	if (!mdev_p) {
		SSP_LOG("Unable to open device %s\r\n",
			mdev_ssp_name[ssp_id]);
		return -WM_FAIL;
	}
	ssp_data_p = (sspdev_data_t *) mdev_p->private_data;
	SET_RX_BUF_SIZE(ssp_data_p, size);
	return WM_SUCCESS;
}

int ssp_drv_timeout(SSP_ID_Type ssp_id, uint32_t txtout, uint32_t rxtout)
{
	mdev_t *mdev_p;
	sspdev_data_t *ssp_data_p;

	if (!IS_SSP_PERIPH(ssp_id))
		return -WM_FAIL;
	/* Check ssp_id is initialized */
	mdev_p = mdev_get_handle(mdev_ssp_name[ssp_id]);
	if (!mdev_p) {
		SSP_LOG("Unable to open device %s\r\n",
			mdev_ssp_name[ssp_id]);
		return -WM_FAIL;
	}
	ssp_data_p = (sspdev_data_t *) mdev_p->private_data;

	ssp_data_p->tx_tout = txtout;
	ssp_data_p->rx_tout = rxtout;
	return WM_SUCCESS;
}

#define AUPLL_FVCO_1 135475200
#define AUPLL_FVCO_2 147456000
#define AUPLL_FVCO_3 134140000
#define AUPLL_FVCO_4 128000000

static void CLK_Config_AuPll(uint32_t fvco, uint8_t pdiv)
{
	CLK_AupllConfig_Type aupllConfigSet;

	/* Default Values as per datasheet */
	aupllConfigSet.vcoClk_X2 = 1;
	aupllConfigSet.enaDither = 1;
	aupllConfigSet.icp = 0;

	switch (fvco) {
	case AUPLL_FVCO_1:
		aupllConfigSet.refDiv = 0xA;
		aupllConfigSet.fbDiv = 0x23;
		aupllConfigSet.fract = 0x08208;
		break;
	case AUPLL_FVCO_2:
		aupllConfigSet.refDiv = 0xA;
		aupllConfigSet.fbDiv = 0x26;
		aupllConfigSet.fract = 0x0AAAA;
		break;
	case AUPLL_FVCO_3:
		aupllConfigSet.refDiv = 0x9;
		aupllConfigSet.fbDiv = 0x1F;
		aupllConfigSet.fract = 0xE54B;
		break;
	case AUPLL_FVCO_4:
		aupllConfigSet.refDiv = 0x6;
		aupllConfigSet.fbDiv = 0x14;
		aupllConfigSet.fract = 0x0;
		break;
	default:
		return;
	}

	switch (pdiv) {
	case 1:
		aupllConfigSet.postDivModulo = 0x3;
		aupllConfigSet.postDivPattern = 0x0;
		break;
	case 2:
		aupllConfigSet.postDivModulo = 0x5;
		aupllConfigSet.postDivPattern = 0x0;
		break;
	case 4:
		aupllConfigSet.postDivModulo = 0x0;
		aupllConfigSet.postDivPattern = 0x0;
		break;
	case 6:
		aupllConfigSet.postDivModulo = 0x1;
		aupllConfigSet.postDivPattern = 0x1;
		break;
	case 8:
		aupllConfigSet.postDivModulo = 0x1;
		aupllConfigSet.postDivPattern = 0x0;
		break;
	case 9:
		aupllConfigSet.postDivModulo = 0x1;
		aupllConfigSet.postDivPattern = 0x2;
		break;
	case 12:
		aupllConfigSet.postDivModulo = 0x2;
		aupllConfigSet.postDivPattern = 0x1;
		break;
	case 16:
		aupllConfigSet.postDivModulo = 0x2;
		aupllConfigSet.postDivPattern = 0x0;
		break;
	case 18:
		aupllConfigSet.postDivModulo = 0x2;
		aupllConfigSet.postDivPattern = 0x2;
		break;
	case 24:
		aupllConfigSet.postDivModulo = 0x4;
		aupllConfigSet.postDivPattern = 0x1;
		break;
	case 36:
		aupllConfigSet.postDivModulo = 0x4;
		aupllConfigSet.postDivPattern = 0x2;
		break;
	case 48:
		aupllConfigSet.postDivModulo = 0x6;
		aupllConfigSet.postDivPattern = 0x1;
		break;
	case 72:
		aupllConfigSet.postDivModulo = 0x6;
		aupllConfigSet.postDivPattern = 0x2;
		break;
	default:
		return;
	}

	CLK_RefClkEnable(CLK_REFCLK_AUD);
	while (CLK_GetClkStatus(CLK_REFCLK_AUD) == RESET)
		;
	CLK_AupllRefClkSrc(CLK_REFCLK_AUD);
	CLK_AupllEnable(&aupllConfigSet);
}

uint32_t ssp_drv_set_clk(SSP_ID_Type ssp_id, uint32_t freq)
{
	mdev_t *mdev_p;
	sspdev_data_t *ssp_data_p;
	CLK_Src_Type clk_src;
	uint16_t i, j, divider;
	uint16_t clk_div = 0;
	uint32_t temp, aupll_fvco = 0;
	uint32_t fvco[] = {AUPLL_FVCO_1, AUPLL_FVCO_2, AUPLL_FVCO_3,
				AUPLL_FVCO_4};
	uint8_t pdiv[] = {1, 2, 4, 6, 8, 9, 12, 16, 18, 24, 36, 48, 72};
	uint8_t aupll_pdiv = 0;

	if (!IS_SSP_PERIPH(ssp_id) || freq <= 0)
		return -WM_FAIL;
	/* Check ssp_id is initialized */
	mdev_p = mdev_get_handle(mdev_ssp_name[ssp_id]);
	if (!mdev_p) {
		SSP_LOG("Unable to open device %s\r\n",
			mdev_ssp_name[ssp_id]);
		return -WM_FAIL;
	}
	ssp_data_p = (sspdev_data_t *) mdev_p->private_data;

	/* Check if freq is supported */
	if (freq < MIN_SSP_FREQ || freq > MAX_SSP_FREQ) {
		SSP_LOG("Invalid SSP freq %u\r\n", freq);
		return -WM_FAIL;
	}
	/* Initialize to max invalid value */
	ssp_data_p->freq = MAX_SSP_FREQ + 1;

	if (ssp_data_p->audiomode) {
		/* In case of audio use case Check for CLK_AUPLL earlier */
		for (i = 0; i < sizeof(fvco) / sizeof(uint32_t); i++)
			for (j = 0; j < sizeof(pdiv); j++) {
				temp = (uint32_t) ((fvco[i] / pdiv[j]));
				if (temp <= ssp_data_p->freq && temp >= freq) {
					clk_src = CLK_AUPLL;
					aupll_fvco = fvco[i];
					aupll_pdiv = pdiv[j];
					ssp_data_p->freq = temp;
				} else if (temp < freq)
					break;
				if (ssp_data_p->freq == freq)
					goto freq_found;
			}
		/* Check for CLK_SYSTEM first */
		divider = (uint32_t)(CHIP_SFLL_FREQ()/freq);
	} else {
		/* Check for CLK_SYSTEM first */
		divider = (uint32_t)(board_cpu_freq()/freq);
	}

	if (IS_SSP_DIVIDER(divider)) {
		clk_src = CLK_SYSTEM;
		if (!ssp_data_p->audiomode) {
			clk_div = divider;
			ssp_data_p->freq = (uint32_t)((board_cpu_freq()
								/ divider));
		} else {
			ssp_data_p->freq = (uint32_t)((CHIP_SFLL_FREQ()
								/ divider));
		}
		if (ssp_data_p->freq == freq)
			goto freq_found;
	}

	if (!ssp_data_p->audiomode) {
		/* In case of standard SSP usecase Check for CLK_AUPLL here */
		for (i = 0; i < sizeof(fvco) / sizeof(uint32_t); i++)
			for (j = 0; j < sizeof(pdiv); j++) {
				divider = (uint32_t)((fvco[i]/pdiv[j]) / freq);
				if (IS_SSP_DIVIDER(divider)) {
					temp = (uint32_t) ((fvco[i] /
							pdiv[j]) / divider);
					if (temp <= ssp_data_p->freq
							&& temp >= freq) {
						clk_src = CLK_AUPLL;
						clk_div = divider;
						aupll_fvco = fvco[i];
						aupll_pdiv = pdiv[j];
						ssp_data_p->freq = temp;
					} else if (temp < freq)
						break;
					if (ssp_data_p->freq == freq)
						goto freq_found;
				}
			}
	}

	if (ssp_data_p->freq > MAX_SSP_FREQ) {
		SSP_LOG("Unable to set %u frequency for %s\r\n",
				freq, mdev_ssp_name[ssp_id]);
		ssp_data_p->freq = 0;
		return -WM_FAIL;
	}

freq_found:
	if (clk_src == CLK_AUPLL)
		CLK_Config_AuPll(aupll_fvco, aupll_pdiv);

	switch (ssp_id) {
	case SSP0_ID:
		CLK_ModuleClkEnable(CLK_SSP0);
		CLK_SSPClkSrc(CLK_SSP_ID_0, clk_src);
		if (!ssp_data_p->audiomode) {
			CLK_ModuleClkDivider(CLK_SSP0, clk_div);
		} else {
		/* AUPLL clock generated for audio codec has to be
		 * (sampling_rate * 256), considering sample is 16 bit,
		 * following divides clock further by factor of 8 to
		 * compensate above 256 multiplier (out of 2 time slots
		 * only one is used for TX/RX).
		 */
			CLK_ModuleClkDivider(CLK_SSP0, 8);
		}
		break;
	case SSP1_ID:
		CLK_ModuleClkEnable(CLK_SSP1);
		CLK_SSPClkSrc(CLK_SSP_ID_1, clk_src);
		if (!ssp_data_p->audiomode) {
			CLK_ModuleClkDivider(CLK_SSP1, clk_div);
		} else {
		/* AUPLL clock generated for audio codec has to be
		 * (sampling_rate * 256), considering sample is 16 bit,
		 * following divides clock further by factor of 8 to
		 * compensate above 256 multiplier (out of 2 time slots
		 * only one is used for TX/RX).
		 */
			CLK_ModuleClkDivider(CLK_SSP1, 8);
		}
		break;
	case SSP2_ID:
		CLK_ModuleClkEnable(CLK_SSP2);
		CLK_SSPClkSrc(CLK_SSP_ID_2, clk_src);
		if (!ssp_data_p->audiomode) {
			CLK_ModuleClkDivider(CLK_SSP2, clk_div);
		} else {
		/* AUPLL clock generated for audio codec has to be
		 * (sampling_rate * 256), considering sample is 16 bit,
		 * following divides clock further by factor of 8 to
		 * compensate above 256 multiplier (out of 2 time slots
		 * only one is used for TX/RX).
		 */
			CLK_ModuleClkDivider(CLK_SSP2, 8);
		}
		break;
	}

	return ssp_data_p->freq;
}

int ssp_drv_init(SSP_ID_Type id)
{
	int ret;

	if (mdev_get_handle(mdev_ssp_name[id]) != NULL)
		return WM_SUCCESS;

	ret = os_semaphore_create(&ssp_sem[id], "ssp");
	if (ret == -WM_FAIL)
		return -WM_FAIL;

	/* Initialize the SSP port's mdev structure */
	ssp_drv_mdev_init(id);

	/* Register the SSP device driver for this port */
	mdev_register(&mdev_ssp[id]);

	fill_cb_array();

	return WM_SUCCESS;
}

void ssp_drv_deinit(SSP_ID_Type id)
{
	mdev_t *dev;

	/* Return if the port ID is invalid */
	if (id < 0 || id >= SSP_MAX_PORT) {
		SSP_LOG("Port %d not enabled\r\n", id);
		return;
	}

	/* Return if the device was not registered */
	if ((dev = mdev_get_handle(mdev_ssp_name[id])) == NULL)
		return;

	/* Delete semaphore and de-register the device */
	os_semaphore_delete(&ssp_sem[dev->port_id]);
	mdev_deregister(dev->name);

	/* Disable Audio PLL */
	CLK_AupllDisable();
}
