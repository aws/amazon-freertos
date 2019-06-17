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

/** @file mdev_ssp.h
 *
 *  @brief This file contains Synchronous Serial Protocol (SSP) Driver
 *
 * The SSP port is a synchronous serial controller that can be connected to
 * a variety of external devices which use serial protocol for data transfer.
 * There are 3 SSP interfaces viz, SSP0 , SSP1, SSP2. The SSP ports can
 * be configured to operate in Master Mode (wherein attached device is slave)
 * or Slave Mode (wherein external device is master).
 * SSP driver exposes various SSP device features through mdev interface.
 *
 * @section mdev_ssp_usage Usage
 *
 * -# Verify that board file API board_ssp_pin_config() located at
 *  sdk/src/boards/{board_file}.c reflects the correct pinmux settings.
 *  If found missing, please add the appropriate pinmux configuration
 *  corresponding to the SSP_ID_Type.

 * -# Initialize the SSP driver using ssp_drv_init().
 * -# Open the SSP device handle using ssp_drv_open().
 *    ssp_drv_open() takes as input  frame format (SPI or I2S)
 *    and master/slave mode. Optionally it can also accept custom
 *    chip select gpio pin for slave device.
 * -# Use ssp_drv_read() or ssp_drv_write() to read SSP data or
 * -# ssp_drv_set_clk() can be used to set frequency of SSP port.
 * -# ssp_drv_rxbuf_size() can be called to set Receive buffer size.
 *    to write data to SSP respectively.
 * -# Close the SSP device using ssp_drv_close() after its use.
 *    ssp_drv_close() call informs the SSP driver to release the resources that
 *    it is holding. It is always good practice to free the resources after use.
 *
 *  Code snippet:\n
 *  Following code demonstrates SSP0 in master mode
 *  transmitting & reading data back from a slave device.
 *
 *  @code
 *  {
 *	int ret;
 *	uint8_t buf[512];
 *      uint8_t read_buf[512];
 *	mdev_t *ssp_dev;
 *      ssp_drv_init(SSP0_ID);
 *      ssp_drv_set_clk(SSP0_ID, 13000000);
 *      ssp_drv_rxbuf_size(SSP0_ID, 1024);
 *      ssp_dev = ssp_drv_open(SSP0_ID, SSP_FRAME_SPI,
 *                             SSP_MASTER, DMA_DISABLE,
 *                             -1, 0);
 *      unit32_t len = ssp_drv_write(ssp_dev, write_data,
 *                                    NULL,
 *                                    512,
 *                                    0);
 *                       .
 *                       .
 *                       .
 *      len = ssp_drv_read(ssp_dev, read_buf, 512);
 *
 *	ssp_drv_deinit(ssp_dev);
 * }
 * @endcode
 *
 *  @note SSP_ID_Type enumeration has values SSP0_ID, SSP1_ID, SSP2_ID
 *
 *  @section default_param Default Parameter Configuration
 *
 *     Default configuration parameter set for SPI are:<br>
 *     mode : SSP_NORMAL<br>
 *     frameformat : SSP_FRAME_SPI<br>
 *     master or slave : configured by user<br>
 *     tr/rx mode  : SSP_TR_MODE<br>
 *     frame data size : SSP_DATASIZE_8<br>
 *     serial frame polarity type : SSP_SAMEFRM_PSP<br>
 *     slave clock running type : SSP_SLAVECLK_TRANSFER<br>
 *     txd turns to three state type : SSP_TXD3STATE_ELSB (i.e. on clock edge
 *     ends LSB)<br>
 *     Enable or Disable SSP turns txd three state mode: ENABLE<br>
 *     Timeout : 0x3<br>
 *
 *     Default configuration parameters set for I2S are:<br>
 *     mode : SSP_NORMAL<br>
 *     frameformat : SSP_FRAME_PSP<br>
 *     master or slave : configured by user<br>
 *     tr/rx mode  : SSP_TR_MODE<br>
 *     frame data size : SSP_DATASIZE_8<br>
 *     serial frame polarity type : SSP_SAMEFRM_PSP<br>
 *     slave clock running type : SSP_SLAVECLK_TRANSFER<br>
 *     txd turns to three state type : SSP_TXD3STATE_12LSB (i.e. 1/2 clock
 *     cycle after start LSB)<br>
 *     Enable or Disable SSP turns txd three state mode: ENABLE<br>
 *     Timeout : 0x3<br>
 */

#ifndef _MDEV_SSP_H_
#define _MDEV_SSP_H_

#include <mdev.h>
#include <lowlevel_drivers.h>

#define SSP_LOG(...)  ll_log("[ssp] " __VA_ARGS__)

#define ACTIVE_HIGH 1
#define ACTIVE_LOW 0

#define SPI_DUMMY_WORD 0x00

/** enum to enable or disable DMA in SSP data transfer */
typedef enum {
	/** Disable ssp DMA controller */
	DMA_DISABLE = 0,
	/** Enable ssp DMA controller */
	DMA_ENABLE,
} SSP_DMA;

/** enum for ssp channel data transfer */
typedef enum {
	/** Disable ssp data transfer on interface */
	CHXFR_DISABLE = 0,
	/** Select Left Channel for data transfer */
	CHXFR_LEFT,
	/** Select Left Channel for data transfer */
	CHXFR_RIGHT,
	/** Select Both Left and Right (Stereo) channels for data transfer */
	CHXFR_BOTH,
} SSP_CHXFR;

/** Initialize SSP Driver
 *
 * This function initializes SSP driver. And registers the device with the mdev
 * interface.
 *
 * @param[in] id  ssp port to be initialized.
 * @return WM_SUCCESS on success
 * @return error code otherwise
 */

int ssp_drv_init(SSP_ID_Type id);

/** De-initialize SSP Driver
 *
 * This function de-initializes SSP driver. Note that if a SSP device has
 * been opened using ssp_drv_open(), it should be first closed using
 * ssp_drv_close() before calling this function.
 *
 * \param[in] id SSP port id
 */
void ssp_drv_deinit(SSP_ID_Type id);

/** Open handle to SSP Device
 *
 * This function opens the handle to SSP device and enables application to use
 * the device. This handle should be used in other calls related to SSP device.
 *
 *  @param[in] ssp_id SSP ID of the driver to be opened
 *  @param[in] format configure mdev driver for SPI/I2S
 *  @param[in] mode it can be either master or slave
 *  @param[in] dma it can be either enabled or disabled
 *  @param[in] cs chip select gpio number (optional) or -1, for master mode
 *  only
 *  @param[in] level chip select gpio level, active low or active high, for
 *  master mode only
 *  @return handle to driver on success
 *  @return NULL otherwise
 *
 *  @note
 *       -# If DMA mode is enabled, it will used DMA channel 0 for SSP tx and
 *          DMA channel 1 for SSP rx operation.
 */
mdev_t *ssp_drv_open(SSP_ID_Type ssp_id, SSP_FrameFormat_Type format,
			SSP_MS_Type mode, SSP_DMA dma, int cs, bool level);

/** Close handle to SSP Device
 *
 * This function closes the handle to SSP device and resets the SSP.
 *
 * @param[in] dev Handle to the SSP device returned by ssp_drv_open().
 * @return WM_SUCCESS on success
 * @return -WM_FAIL on error.
 */
int ssp_drv_close(mdev_t *dev);

/** Write data to SSP port
 *
 * This function is used to write data to the SSP in order to transmit them
 * serially over serial out line.
 * The write call is BLOCKING. It will not return till all
 * the requested number of data elements (8 bit) are transmitted.
 *
 *  @param[in] dev Handle to the SSP device returned by ssp_drv_open().
 *  @param[in] data  data to be written
 *  @param[in] din buffer address for data to be read (if flag = 1) or null
 *  @param[in] num Number of 8 bit elements to be written
 *  @param[in] flag flag = 1, read data from rxfifo
 *  @return Number of 8 bit elements actually written (may be less than num
 *          if timeout occurs)
 *          -WM_FAIL otherwise.
 *  @note: read/write timeout is supported for dma io only
 *
 *  @note When SSP pushes out a byte from txfifo, it receives a byte in rxfifo.
 *        So, depending on slave device behavior one may want to read out these
 *        bytes from rxfifo and store them or discard them.
 *        If flag = 1 then rxfifo will be read out and data will be stored in
 *        din. If din is NULL data will be read out and discarded. If flag = 0
 *        data will not be read.
 *        e.g. Some flash chip interfaced over ssp might need to discard read
 *        data when writing some command to flash chip.
 */
int ssp_drv_write(mdev_t *dev, const uint8_t *data, uint8_t *din,
		uint32_t num, bool flag);

/** Read from SSP port
 *

 * This function is used to read data from the SSP
 * The Read call is NONBLOCKING for slave and BLOCKING for master.
 *
 *  @param[in] dev Handle to the SSP device returned by ssp_drv_open().
 *  @param[out] data Pointer to an allocated buffer of size equal to or more
 *             than the value of the third parameter num.
 *  @param[in] num The maximum number of bytes to be read from the SSP.
 *  @return Number of 8 bit elements actually read (may be less than num
 *          if timeout occurs)
 *          -WM_FAIL otherwise.
 *  @note: read/write timeout is supported for dma io only
 */
int ssp_drv_read(mdev_t *dev, uint8_t *data, uint32_t num);

/** Configure chip select gpio in master mode
 *
 *  This will configure chip select gpio (only applicable to master mode) and
 *  its assertion level. Use of this api is entirely optional and can be used to
 *  select one of multiple SSP slaves connected after ssp_drv_open().
 *
 *  @param[in] dev Handle to the SSP device returned by ssp_drv_open().
 *  @param[in] cs chip select gpio number (optional) or -1, for master mode
 *  only
 *  @param[in] level chip select gpio level, active low or active high, for
 *  master mode only
 */
void ssp_drv_cs_configure(mdev_t *dev, int cs, bool level);

/** Assert chip select gpio
 *
 *  This will assert chip select gpio (only applicable to master mode) as per
 *  level configured during driver init step. Use of this api is entirely
 *  optional.
 *  @param[in] dev Handle to the SSP device returned by ssp_drv_open().
 */
void ssp_drv_cs_activate(mdev_t *dev);

/** De-assert chip select gpio
 *
 *  This will de-assert chip select gpio (only applicable to master mode) as
 *  per level configured during driver init step. Use of this api is entirely
 *  optional.
 *  @param[in] dev Handle to the SSP device returned by ssp_drv_open().
 */
void ssp_drv_cs_deactivate(mdev_t *dev);

/** Set clock frequency
 *
 *  This is optional call to set ssp clock frequency. By default
 *  clock frequency is set to 10MHz. This call should be made after
 * ssp_drv_init and before ssp_drv_open to over-write default clock
 * configuration.
 *
 * @param[in] ssp_id SSP ID of the driver
 * @param[in] freq frequency to be set
 * @return Actual frequency set by driver
 * @return -WM_FAIL on error
 */
uint32_t ssp_drv_set_clk(SSP_ID_Type ssp_id, uint32_t freq);

/** Set RX Ringbuffer size
 *
 * This is optional call to change rx ringbuffer size. By default
 * ringbuffer size is set to 2K. This call should be made after
 * ssp_drv_init and before ssp_drv_open to over-write default configuration.
 *
 * @param[in] ssp_id SSP ID of the driver
 * @param[in] size size of ring buffer in bytes
 * @return WM_SUCCESS on success
 * @return -WM_FAIL on error
 */
int ssp_drv_rxbuf_size(SSP_ID_Type ssp_id, uint32_t size);

/** Set timeout for SSP read/write operations for dma io only.
 *
 *  This function sets the timeout for SSP dma read/write operations.
 *  It is disabled by default. This function needs to be called
 *  after init and can be optinally called before any read/write operation
 *  in dma io. Once set, it will use the same timeout values for all
 *  operations, till they are changed again by another call.
 *
 * @param[in] ssp_id SSP ID of the driver
 * @param[in] txtout Tx timeout value in milli seconds, or OS_WAIT_FOREVER to
 *            disable timeout.
 * @param[in] rxtout Rx timeout value in milli seconds, or OS_WAIT_FOREVER to
 *            disable timeout.
 * @return WM_SUCCESS on success
 * @return -WM_FAIL on error
 */
int ssp_drv_timeout(SSP_ID_Type ssp_id, uint32_t txtout, uint32_t rxtout);

/** Set data-size for SSP read/write operations.
 *
 * @note : Currently SSP_DATASIZE_18 is not supported.
 *
 * This function sets the data-size for SSP read/write operations.
 * It is 8-bit by default. This call should be made after
 * ssp_drv_init and before ssp_drv_open to override default configuration.
 *
 * @param[in] ssp_id SSP ID of the driver
 * @param[in] dataSize One of SSP_DATASIZE_8, SSP_DATASIZE_16, SSP_DATASIZE_32.
 * @return WM_SUCCESS on success
 * @return -WM_FAIL on error
 */
int ssp_drv_datasize(SSP_ID_Type ssp_id, SSP_DataSize_Type dataSize);

/** Set audio-mode for SSP read/write operations.
 *
 * @note : Audio Mode is DISABLED by default.
 *
 * This function enables or disables audio data transfer mode for SSP.
 * It is boolean value. This call should be made after
 * ssp_drv_init and before ssp_drv_open to override default configuration.
 *
 * @param[in] ssp_id SSP ID of the driver
 * @param[in] audio mode One of ENABLE, DISABLE.
 * @return WM_SUCCESS on success
 * @return -WM_FAIL on error
 */
int ssp_drv_audiomode(SSP_ID_Type ssp_id, FunctionalState amode);

/** Configure network parameters for PSM Mode operations.
 *
 * There is a need for AudioCodec to configure SSP network configuration
 * This API will be further used by nau881x and wm8731 drivers to configure
 * audiocodec for faithful I2S data transfer
 *
 * @param[in] dev Handle to the SSP device returned by ssp_drv_open().
 * @param[in] txTimeSlots to configure Tx data transfer to Right, Left
 *				or Both Channels
 * @param[in] rxTimeSlots to configure Rx data transfer from Right, Left
				or Both Channles
 * @return WM_SUCCESS on success
 * @return -WM_FAIL on error
 */
int ssp_drv_network_config(mdev_t *dev, SSP_CHXFR txTimeSlots,
			   SSP_CHXFR rxTimeSlots);

#endif /* _MDEV_SSP_H_ */
