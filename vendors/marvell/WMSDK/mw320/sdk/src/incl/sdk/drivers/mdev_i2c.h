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

/** @file mdev_i2c.h
 *
 *  @brief This file contains Inter-Integrated Circuit (I2C) Driver (TWSI)

 * The 2-Wire Serial Interface (TSWI) unit is an interface between a
 * 2-wire serial bus and the CPU.
 * I2C Driver is meant to use TWSI hardware to communicate with
 * external peripheral connected on I2C bus through application.
 *
 *  a. The speed of operation:
 *	There are standard two speeds at which I2C peripherals operate.
 *	100KHz (Standard speed) and 400KHz (High speed).
 *	We have also provided 10KHz (Low speed) to handle any special case.
 *
 * b. Slave address definition:
 * 	Any valid transaction on I2C bus is associated with the device slave
 * 	address and each I2C peripheral has it's own address.
 * 	The I2C driver should be informed of the slave address of the peripheral
 *      that should be accessed.
 *
 * c. Interface type:
 * 	Internally TWSI can be connected to different hardware blocks.
 * 	This identifies the device connection and specifies the interface
 * 	type.
 *
 * Note: The FIFO size of the hardware FIFO is 16 bytes
 *
 * \section I2C_usage Usage
 *
 * Follow the steps below to use I2C driver from applications
 *
 * -# I2C driver initialization using \ref i2c_drv_init().
 * -# Open I2C device to handle using \ref i2c_drv_open() call.
 * -# Use i2c_drv_write or i2c_drv_read() calls to write or read data over
 *  I2C bus respectively.
 * -# Close the I2C port using i2c_drv_close() to release the resources that
 *  the driver holds.
 *
 * Typical example:
 *
 * Below peice of code demonstrates how to configure a I2C device whose slave
 * address is I2C_ADDR interfaced on I2C1 for read/write operation.
 * \code
 * {
 * 	mdev_t *i2c_dev;
 *
 *	i2c_dev = i2c_drv_open(I2C1_PORT,
 *			I2C_SLAVEADR(I2C_ADDR >> 1));
 *
 *  if (ret != WM_SUCCESS)
 *		return;
 *
 *   uint8_t read_buf[4];
 *   uint8_t write_buf[2];
 *   buf[0] = 0x01;
 *   buf[1] = 0x02;
 *	 int read_len = i2c_drv_read(i2c_dev, buf, 4);
 *   int write_len = i2c_drv_write(i2c_dev, buf, 4);
 *	 i2c_drv_close(i2c_dev);
 * }
 *\endcode
 *
 * \note I2C_LCD_ADDR being a 8 bit address, was right shifted by 1 to make it
 * valid 7 bit I2C address. This is slave dependant operation, please check
 * slave device datasheet for more details.
 *
 * I2C master start stop generation: The I2C IP automatically handles
 * START and STOP condition generation. When first byte is put in TxFIFO
 * (using the API i2c_drv_write()), a START condition is generated and
 * slave address is automatically sent on the bus. When all bytes from the
 * TxFIFO are out on the bus the IP generates STOP condition on the
 * bus. This point is important to note while designing the I2C
 * application. For instance, the application may want to call
 * i2c_drv_write() multiple times during a single transaction. If the time
 * gap between any successive call exceeds FIFO emptying time then a STOP
 * will be generated automatically and things will not work as
 * expected. A potential pitfall is adding debugging prints in between two
 * calls to i2c_drv_write() which may delay next write and the FIFO will
 * may get emptied generating the unexpected STOP.
 * After all the writes are complete you may call
 * i2c_drv_wait_till_inactivity() to ensure that all the writes are out on
 * the bus an a STOP condition is generated signifying end of current
 * transaction.
 *
 * Conversely, if the application wishes to have two back to back
 * calls to i2c_drv_write() in separate transactions it needs to ensure
 * that TxFIFO is empty before second call to i2c_drv_write(). The API
 * i2c_drv_wait_till_inactivity() is provided for this purpose. Using this
 * API will ensure the application can after this API returns with a
 * success the earlier write is out on the bus and a STOP is generated. The
 * following example illustrates the scenario:
 *
 * 1. Write a byte to I2C device at an address
 *    - Write slave address
 *    - Write destination address
 *    - Write data to be written
 * 2. Use i2c_drv_wait_till_inactivity() to ensure this transaction is
 * complete.
 * 3. Read a byte from some other address
 *   - Write slave address
 *   - Write read address
 *   - Read data
 *
 * Note that the API i2c_drv_wait_till_inactivity() is not technically
 * necessary for STOP to be generated. It only helps the application to
 * sync itself with that particular bus event.
 */


#ifndef _MDEV_I2C_H_
#define _MDEV_I2C_H_

#include <mdev.h>
#include <lowlevel_drivers.h>


/** Maximum number of I2C Hardware Interfaces available */
#define NUM_I2C_PORTS	2

/** I2C specific parameters required for initialization
  */
#define BIT10	(0x00000001 << 10)
#define BIT11	(0x00000001 << 11)
#define BIT12	(0x00000001 << 12)
#define BIT13	(0x00000001 << 13)

/** I2C Hardware Interface speed of operation -Standard (100Khz) */
#define I2C_CLK_100KHZ	(~(BIT12 | BIT13) & 0x3000)
/** I2C Hardware Interface speed of operation -Low (10Khz) */
#define I2C_CLK_10KHZ	BIT13
/** I2C Hardware Interface speed of operation -High (10Khz) */
#define I2C_CLK_400KHZ	BIT12
/** I2C flag to open driver as slave mode  */
#define I2C_DEVICE_SLAVE       BIT11
/** Macro to specify slave address of max 10 bits while opening a device
 * the address would be truncated to 7 bits if I2C_10_BIT_ARRD is not set
 * */
#define I2C_SLAVEADR(x)	(0x3ff & (x))
/** Macro to specify 10 bit address mode */
#define I2C_10_BIT_ADDR		BIT10

/**
 * Enum to enable and disable DMA mode for I2C transfer
 * */
typedef enum {
	/** Disable I2C DMA controller */
	I2C_DMA_DISABLE = 0,
	/** Enable I2C DMA controller */
	I2C_DMA_ENABLE,
} I2C_DMA_MODE;

/** Initialize  I2C Driver
 *
 *  This function initializes I2C driver and registers the device
 *  with the mdev interface.

 * @param[in] id I2C device to be used.
 * @return WM_SUCCESS on success
 * @return -WM_FAIL on error
 */
int i2c_drv_init(I2C_ID_Type id);

/** Set I2C driver clock(SCL) configuration
 *
 * This is optional call to change I2C CLK frequency.  By default the
 * clock settings are set for standard loading conditions.
 * I2C clock frequency may deviate from the standard 100K/ 400K due to bus
 * loading. User may use this API to tune the I2C clk frequency.
 * @note  This call should be made after i2c_drv_init() and
 *        before i2c_drv_open() to over-write default configuration.
 *
 * @param[in] i2c_id I2C ID of the driver
 * @param[in] hightime min high time for I2C SCL (clock line) in nanoseconds
 * @param[in] lowtime min low time for I2C SCL (clock line) in nanoseconds
 * Reducing the hightime or lowtime would increase the SCL frequency
 * Default values for 100K are
 * HCNT: 4000	LCNT:4700
 * Default values for 400K are
 * HCNT: 600	LCNT:1300
 */
void i2c_drv_set_clkcnt(I2C_ID_Type i2c_id, int hightime, int lowtime);

/** Set RX Ring-buffer size
 *
 * This is optional call to change rx ringbuffer size. By default
 * ringbuffer size is set to 128 bytes.
 * @note  This call should be made after i2c_drv_init() and
 *        before i2c_drv_open() to over-write default configuration.
 *
 * @param[in] i2c_id I2C ID of the driver
 * @param[in] size size of ring buffer in bytes
 * @return WM_SUCCESS on success
 * @return -WM_FAIL on error
 */
int i2c_drv_rxbuf_size(I2C_ID_Type i2c_id, uint32_t size);

/** set I2C transfer mode
  *
  * This is an optional call to set I2C trasfer mode. By default I2C transfer
  * mode is set to I2C_DMA_DISABLE. This function should be called after
  * i2c_drv_init() and before i2c_drv_open() call to enable DMA transfer mode.
  * Master write and slave read over I2C bus using DMA are supported.
  * Slave write and master read over DMA is not supported and hence use PIO
  * (non DMA) mode for data transmit/receive.
  *
  * \param[in] i2c_id Port ID of I2C
  * \param[in] dma_mode \ref I2C_DMA_DISABLE or \ref I2C_DMA_ENABLE
  * \return WM_SUCCESS on success.
  * \return -WM_FAIL on error.
  */
int i2c_drv_xfer_mode(I2C_ID_Type i2c_id, I2C_DMA_MODE dma_mode);

typedef void (*i2c_int_cb) (I2C_INT_Type type, void *data);

/** Open handle to I2C Device
 *
 * This function opens the handle to I2C device and enables application to use
 * the device. This handle should be used in other calls related to I2C device.
 *
 * @param[in] i2c_id I2C port of the driver to be opened.
 * @param[in] flags
 * @section Specification of flags passed to i2c_drv_open:
 | BIT No	  | Use     |
 |-------------|--------------------------------------------|
 |	BIT[0:6]     | I2C_SLAVE_ADDRESS (7 bit address mode)   |
 |	BIT[0:9]     | I2C_SLAVE_ADDRESS (10 bit address mode)  |
 |   BIT10        | set for 10 bit address mode			    |
 |   BIT11        | set for slave mode operation			|
 |	BIT[12 : 13]   | speed of operation	                    |
 *
 *  @return handle to driver on success
 *  @return NULL otherwise
 */
mdev_t *i2c_drv_open(I2C_ID_Type i2c_id, uint32_t flags);

/**
 * Set callback function to receive notifications
 *
 * These notifications are actually hardware interrupts and are delivered
 * in ISR context. The actual ISR handling is done by the driver. The
 * notification may help the driver to manage its state machine.
 *
 * \note Only a few interrupts out of the complete list are delivered to
 * the application. Following is the list of interrupts delivered to the
 * application:
 *
 * 1. I2C_INT_RD_REQ
 * 2. I2C_INT_RX_OVER
 * 3. I2C_INT_RX_DONE
 * 4. I2C_INT_STOP_DET
 *
 * The description of the above interrupts are given in file
 * {mc200/mw300}_i2c.h in the SDK.
 *
 * \note Since the callbacks are delivered in ISR context, appropriate
 * care needs to be taken. Especially, no other i2c_drv_* calls like
 * i2c_drv_write() or i2c_drv_read() should be done from this callback
 * as they may using blocking API's internally.
 *
 * @param[in] dev Handle to I2C device returned by i2c_drv_open()
 * @param[in] event_cb Callback function defined by caller.
 * @param[in] data user's private pointer that is passed as
 *	a parameter to the callback function
 *
 * @return WM_SUCCESS if callback registered successfully.
 * @return -WM_E_INVAL if invalid argument.
 */
int i2c_drv_set_callback(mdev_t *dev, i2c_int_cb event_cb, void *data);

/**
 * Set the slave address in i2c master mode.
 *
 * This sets the slave address with which i2c module will perform
 * subsequent IO operations in master mode. It can be optionally called
 * after i2c_drv_open() and before performing any read/write operation.
 * This function does not set the address of i2c module in slave mode.
 *
 * @param[in] dev Handle to I2C device returned by i2c_drv_open()
 * @param[in] flags
 * @section Specification of flags passed to i2c_drv_set_address:
 | BIT No      | Use					  |
 |-------------|------------------------------------------|
 |BIT[0:6]     | I2C_SLAVE_ADDRESS (7 bit address mode)   |
 |BIT[0:9]     | I2C_SLAVE_ADDRESS (10 bit address mode)  |
 |BIT10        | set for 10 bit address mode		  |
 *
 * @return WM_SUCCESS I2C address is set successfully.
 * @return -WM_FAIL in case of failure.
 */
int i2c_drv_set_address(mdev_t *dev, uint32_t flags);

/**
 * Wait for I2C module inactivity.
 *
 * Checks for and wait till the I2C module is inactive. For e.g. when
 * i2c_drv_write() returns, some bytes are still pending in the TxFIFO. If
 * the application wishes to ensure that TxFIFO is empty before starting a
 * new transaction this API needs to be called. Please check current file
 * documentation for more details.
 *
 * @param[in] dev Handle to I2C device returned by i2c_drv_open()
 * @param[in] timeout_ms 0 if to return immediately else spins for
 * inactivity for max timeout_ms milliseconds.
 *
 * @return WM_SUCCESS I2C module is inactive.
 * @return -WM_FAIL I2C module is still active.
 */
int i2c_drv_wait_till_inactivity(mdev_t *dev, uint32_t timeout_ms);

/** Close handle to I2C Device
 *
 * This function closes the handle to I2C device and resets the I2C
 * @param[in] dev Handle to the  I2C device returned by i2c_drv_open().
 * @return WM_SUCCESS on success, -WM_FAIL on error.
 */
int i2c_drv_close(mdev_t *dev);

/** Read from I2C
 *
 * This function reads specified number of bytes into
 * the provided buffer from the I2C device.
 * I2C Reads two bytes from interface as
 * [Start][Slave address(read)]Rd Byte0, Rd Byte1[Stop] sequence on I2C
 *
 * @param[in] dev Handle to the I2C device returned by i2c_drv_open().
 * @param[out] buf Buffer Pointer to an allocated buffer of size equal
 *              to or more than the value of the third parameter nbytes.
 * @param[in] nbytes Number of bytes to be read.
 * @return number of bytes read on success
 * @return  -WM_E_AGAIN in case of a timeout
 *        (if timeout is enabled using the API i2c_drv_timeout)
 *        non-zero otherwise.
 * @note: read/write timeout is supported for master mode only
 */
int i2c_drv_read(mdev_t *dev, void *buf, uint32_t nbytes);

/** Read from I2C port in slave mode in DMA mode
 *
 * This function reads specified number of bytes into the user buffer
 *
 * @param[in] dev Handle to the I2C device returned by i2c_drv_open().
 * @param[out] buf Buffer Pointer to an allocated buffer of size equal
 *              to or more than the value of the third parameter nbytes.
 * @param[in] nbytes Number of bytes to be read.
 *
 * @return number of bytes read on success
 */
int i2c_drv_slv_dma_read(mdev_t *dev, void *buf, uint32_t nbytes);

/** Write to I2C
 * I2C Reads two bytes from interface as
 * [Start][Slave address(write)]Byte0,Byte1[Stop] sequence on I2C
 *
 * This function writes specified number of bytes to the I2C device Tx-FIFO.
 * @note In slave mode if the user wants to enable low power modes,
 * application should take wakelock in order to prevent slave from
 * entering low power mode while serving the write request.
 *
 * While operating in slave mdoe, if the master sends RD_REQ to the slave
 * before i2c_drv_write call is made, slave Tx-FIFO remains empty and slave
 * sends out dymmy data byte (i.e.0xff).

 * @param[in] dev mdev_t handle to the driver.
 * @param[in] buf Buffer containing data to be written.
 * @param[in] nbytes Number of bytes to be written.
 * @return number of bytes written on success
 *         -WM_E_AGAIN in case of a timeout(if timeout is enabled
 *         using the API i2c_drv_timeout),
 *         -WM_FAIL otherwise.
 * @note
 *       -# For a slave device waiting on RD_REQ,
 *          if time out occurs, no data is sent
 *          to master and i2c_drv_write returns 0 in this case.
 *       -# read/write timeout is supported for master mode only
 */
int i2c_drv_write(mdev_t *dev, const void *buf, uint32_t nbytes);


/** Write to I2C using DMA
 * I2C Reads two bytes from interface as
 * [Start][Slave address(write)]Byte0,Byte1[Stop] sequence on I2C
 *
 * This function writes specified number of bytes to the I2C device Tx-FIFO;
 * using DMA to copy data from RAM to I2C FIFO.
 * @param[in] dev mdev_t handle to the driver.
 * @param[in] buf Buffer containing data to be written.
 * @param[in] nbytes Number of bytes to be written.
 * @return number of bytes written on success
 *         -WM_E_AGAIN in case the data length exceeds the max allowed length
 *         -WM_FAIL otherwise.
 * @note
 *       -# This API is supported for master mode only
 */
int i2c_drv_dma_write(mdev_t *dev, const uint8_t *buf, uint32_t nbytes);


/** Read from I2C using DMA
 *
 * This function reads specified number of bytes from the I2C-RX-FIFO
 * using DMA to copy data from I2C FIFO to RAM.
 * @param[in] dev mdev_t handle to the driver.
 * @param[in] buf Buffer containing data to be written.
 * @param[in] nbytes Number of bytes to be written.
 * @return number of bytes written on success
 *         -WM_E_AGAIN in case of a timeout
 *         -WM_FAIL otherwise.
 * @note
 *       -# This API is supported for master mode only
 */
int i2c_drv_dma_read(mdev_t *dev, uint8_t *buf, uint32_t nbytes);

/** Set timeout for I2C read/write operations for master mode only.
 *
 *  This function sets the timeout for I2C read/write operations.
 *  It is disabled by default. This function needs to be called
 *  after init and before open for that particular port.
 *
 * @param[in] id I2C port number
 * @param[in] txtout Tx timeout value in milli seconds, or zero to
 *            disable timeout.
 * @param[in] rxtout Rx timeout value in milli seconds, or zero to
 *            disable timeout.
 */
void i2c_drv_timeout(I2C_ID_Type id, uint32_t txtout, uint32_t rxtout);

/* Get status of I2C controller
 * This function returns the status of I2C controller
 * The controller may be in one of following states
 * 1. I2C_INACTIVE
 * 2. I2C_ACTIVE
 * 3. I2C_ERROR
 *
 * @param[in] dev mdev_t handle of the driver
 * @param[out] status pointer
 * @return on error return -WM_E_INVAL
 * @return WM_SUCCESS on success
 */

int i2c_drv_get_status_bitmap(mdev_t *mdev_p, uint32_t *status);

/** Enumeration which indicates status of I2C */
typedef enum {
		/** I2C_INACTIVE : indicates that no ongoing
		  * I2C activity on the bus
		  */
		I2C_INACTIVE = 1 << 0,

		/** I2C_ACTIVE : indicates I2C bus is busy.
		  */
		I2C_ACTIVE = 1 << 1,

		/** I2C_ERROR : indicates that an error has occurred on I2C bus.
		   */
		I2C_ERROR = 1 << 2
} i2c_status;

/* Enable I2C port
 * This API can be used as when the user wants to enable
 * I2C bus that is disabled by the user using i2c_disable().
 * Note: At the start, i2c_drv_open internally calls this API to enable
 * one of the (i.e.user specified) I2C port.
 *
 * @param[in] dev mdev_t handle to the driver.
 * @return On error return -WM_FAI
 * @return WM_SUCCESS on success
 */
int i2c_drv_enable(mdev_t *dev);

/* Disable I2C port
 * This API can be used when:
 * 1. User wants to disable an I2C port
 * 2. Device , operating in slave mode is unable to send data on a master's
 * request. In this case the bus stalls. User may disable the port and
 * re-enable the port using i2c_enable() to restore the I2C bus.
 *
 * @param[in] dev mdev_t handle to the driver.
 * @return On error return -WM_FAI
 * @return WM_SUCCESS on success
 *
 */
int i2c_drv_disable(mdev_t *dev);


/** De-initialize I2C Port
 *
 * This function de-initializes I2C port. Note that if an I2C device has
 * been opened using i2c_drv_open(), it should be first closed using
 * i2c_drv_close() before calling this function.
 *
 * @param[in] id I2C device to be used.
 */
void i2c_drv_deinit(I2C_ID_Type id);

#endif /* _MDEV_I2C_H_ */
