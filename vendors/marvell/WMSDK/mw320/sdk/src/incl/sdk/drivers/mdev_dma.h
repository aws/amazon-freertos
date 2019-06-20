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

/** @file mdev_dma.h
 *
 *  @brief This file contains DMA Module
 *
 * mdev_dma is a module that handles DMA driver functionality a layer
 * above the low-level driver code. It establishes a mechanism to deploy
 * DMA capabilities to all peripherals, possibly simultaneously.
 *
 * The DMAC hardware supports 32 DMA channels. By default, four channels
 * can operate simultaneously. User may configure the value of NUM_DMA_CHANNELS
 * to modify the number of supported channels.
 *
 * This module provides a set of APIs to use DMA.
 * DMA transfer can be used for the following modes.
 * 1. MEMORY_TO_PERIPHERAL
 * 2. PERIPHERAL_TO_MEMORY
 * 3. MEMORY_TO_MEMORT
 * @note PERIPHERAL_TO_PERIPHERAL transfer is not supported.
 *
 * @note DMA driver limitations
 * 1. DMAC supports a maximum of 8191 byte data transfer with
 * the added restriction that SOURCE and DESTINATION
 * ADDRESSES should align with the transfer width.
 *
 * 2. DMA transfers will be asynchronous.
 *
 *
 *  A typical DMA usage scenario is as follows:
 *
 *  -# Initialize the DMA driver with a call dma_drv_init()
 *  -# Open a channel for DMA transfer using dma_drv_open()
 *  -# User needs to configure dma_config_type.
 *  -# [Optional] Call \ref dma_drv_set_cb to set callback for a specific
 *	dma channel.
 *  -# Call dma_drv_transfer \ref dma_drv_transfer with appropriate parameters.
 *  -# Close the DMM channel, on transfer complete.
 *  -# de-initialize DMA driver provided no code is using DMA.
 *
 * Code snippet:
 * Following code demonstrates how to use DMA driver API
 * to generate and handle an interrupt
 * @code
 * static void dma_cb(DMA_Channel_Type channel, void *data)
 * {
 *	process_dma_interrupt();
 * }
 *
 * {
 *	int err = dma_drv_init();
 *	if (err != WM_SUCCESS)
 *		err_handling();
 *
 *	mdev_t *dma_dev = dma_drv_open();
 *	if (dma_dev == NULL)
 *		err_handling();
 *
 * [optional] dma_drv_set_cb(dma_dev, dma_cb);
 *
 *	err = dma_drv_transfer(dma_dev, &dma_c);
 *	if (err != WM_SUCCESS)
 *		err_handling();
 *
 *	dma_drv_wait_for_transfer_complete();
 * }
 * @endcode
 */

#ifndef __MDEV_DMA_H_
#define _MDEV_DMA_H_

#include <mdev.h>
#include <lowlevel_drivers.h>

/* MW300 supports maximum 32 DMA_CHANNELS.
 * User may configure NUM_DMA_CHANNELS up-to 32 */
#define NUM_DMA_CHANNELS 4

#define MDEV_DMA "MDEV_DMA"

/* enum dma_transfer_status_t indicates
 * dma transfer status
 */
typedef enum {
	DMA_SUCCESS = 1,
	DMA_FAIL = 2,
} dma_transfer_status_t;

/* DMA callback function */
typedef void (*dma_irq_cb) (DMA_Channel_Type channel, dma_transfer_status_t
							status, void *data);

/**  \struct dma_config_type
 * DMA configuration structure
 */
typedef struct dma_config_type {
	/** dma_cfg is the configuration structure that stores
	 * all relevant parameters corresponding to the transfer
	 */
	DMA_CFG_Type dma_cfg;
	/** Dma Peripheral mapping or handshaking interface is required for
	 * memory to/from peripheral transfer. It would not be
	 * required in case the transfer is of type MEM-MEM
	 */
	DMA_PerMapping_Type perDmaInter;	/* Handshaking Interface */
} dma_config_t;


/**
 * DMA driver initialization.
 *
 * @par
 * This API initializes the DMA driver.
 *
 * @return -WM_FAIL in case of failure to register the DMA driver
 * @return WM_SUCCESS if DMA driver registration is successful.
 */
 int dma_drv_init();

 /**
 * Open a dma channel.
 *
 * @par
 * This API selects a channel for DMA transfer. It will select
 * a free channel and reserve the same till dma_drv_close() is
 * explicitly called.
 *
 * @pre dma_drv_init()
 *
 * @return handle to dma channel. It is channel number type-casted
 * 	to pointer of type mdev_t.
 * @return -NULL in case the API fails to select a DMA channel
 *
 * @note Any available channel can be used for data transfer
 * There is no mapping between DMA channel and peripheral.
 * However correct handshaking interface must be chosen
 * during memory to/from peripheral data transfer.
 */
mdev_t* dma_drv_open();

/**
 * Set callback for a specific DMA channel on DMA interrupt.
 *
 * @par
 * This API registers a callback on a specific DMA interrupt for a given
 * DMA channel.
 *
 * @param[in] mdev_dma pointer to mdev_dma
 * @param[in] dma_cb function to be called when DMA interrupt occurs
 * @param[in] data user's private pointer that is passed as a parameter to the
 * callback function
 *
 * @return WM_SUCCESS on success
 * @return -WM_E_INVAL on failure to register callback or in case of invalid
 * parameters.
 */
int dma_drv_set_cb(mdev_t *mdev_dma, dma_irq_cb dma_cb, void *data);
/**
 * Configure & enable a particular dma channel.
 *
 * @par
 * This API configures a DMA channel and enables it to begin with the data
 * transfer.
 *
 * @pre dma_drv_open()
 *
 * @param[in] mdev_dma handle pointer to dma channel
 * @param[in] dmac pointer to \ref dma_config_type
 *
 * @return -WM_E_INVAL in case of invalid parameters
 * @return WM_SUCCESS on success
 *
 * @note Since DMA Controller handles transmit & receive
 * in the exact same manner; no separate read/write calls are built.
 * A single API, dma_transfer_start() is maintained for all types of
 * data transfers.
 * The API configures a channel and starts data transfer. In order to
 * check the status of data transfer, user should use
 * the API dma_drv_wait_for_transfer_complete().
 */
int dma_drv_transfer(mdev_t *mdev_dma, dma_config_t *dmac);

/**
 * Wait for transfer complete on a particular dma channel.
 *
 * @par
 * This API waits for a specified time for DMA transfer to complete.
 * It also notifies the user if an error occurs during DMA transfer.
 * The error may be a DMA bus error or address alignment error.
 *
 * @param[in] mdev_dma handle pinter to dma channel
 * @param[in] timeout can be a specific time or OS_NO_WAIT \ref OS_NO_WAIT
 * or OS_WAIT_FOREVER \ref OS_WAIT_FOREVER
 *
 * @return -WM_E_INVAL in case of invalid parameters
 * @return -WM_E_AGAIN in case of timeout
 * @return WM_SUCCESS on success
 *
 * @note It is mandatory to call dma_drv_wait_for_transfer_complete()
 * after dma_drv_transfer() and to check the status of data transfer.
 */
int dma_drv_wait_for_transfer_complete(mdev_t *mdev_dma, uint32_t timeout);

/**
 * Close DMA channel.
 *
 * @par
 * This API closes the DMA channel specified by the given
 * handle. The resources associated with the channel are
 * freed up. This channel now can be selected by dma_drv_open
 * for another DMA transfer. This API also de-registers the registered callbacks
 * on the channel.
 *
 * @param[in] mdev_dma handle to DMA channel
 *
 * @return WM_SUCCESS if channel is closed successfully
 * @return -WM_E_INVAL if the parameters are invalid
 */
int dma_drv_close(mdev_t* mdev_dma);

/**
 * De-initialize DMA driver.
 *
 * This API de-registers the DMA driver. All resources associated
 * with memory and DMA driver are freed up.
 *
 */
void dma_drv_deinit();

#endif /* _MDEV_DMA_H_ */
