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

/** @file mdev_uart.h
 *
 *  @brief This file contains UART driver
 *
 * UART is a piece of hardware that translates data between parallel and serial
 * forms. It provides serial interface for communication to external world.
 * UART driver provides interface to use all available UARTs.
 *
 * \section uart_usage Usage
 *
 * A typical UART device usage scenario is as follows:
 *
 * -# Verify that board file API board_uart_pin_config() located at
 *  sdk/src/boards/{board_file}.c reflects the correct pinmux settings.
 *  If found missing, please add the appropriate pinmux configuration
 *  corresponding to the UART_ID.
 *  * -# Initialize the UART driver using uart_drv_init().
 * -# Open the UART device handle using uart_drv_open() call.
 * -# Use uart_drv_read() or uart_drv_write() calls to read UART data or
 *  to write data to UART respectively.
 * -# Close the UART device using uart_drv_close() after its use.
 *  uart_drv_close() call informs the UART driver to release the resources that
 *  it is holding. It is always good practice to free the resources after use.
 * -# Deinitialize the UART driver using uart_drv_deinit().
 *
 *  Code snippet:\n
 *  Following code demonstrates basic read operation over UART0
 *  UART0 is configured as 8 bits per character & baud-rate of 115200.
 *  \code
 *  {
 *	int ret;
 *	uint8_t buf[512];
 *	mdev_t *uart_dev;
 *	ret = uart_drv_init(UART0_ID, UART_8BIT);
 *	if (ret != WM_SUCCESS)
 *		return;
 *	uart_dev = uart_drv_open(UART0_ID, 115200);
 *	if (uart_dev == NULL)
 *		return;
 *	len = uart_drv_read(uart_dev, buf, sizeof(buf));
 *	ret = uart_drv_close(uart_dev);
 *	if (ret != WM_SUCCESS)
 *		return;
 *	uart_drv_deinit(UART0_ID);
 * }
 * \endcode
 *
 * \note\n
 * 1. Specify standard baud rates such as 9600,115200..
 * 2. When baudrate is 0 in uart_drv_open() call, UART driver will take
 * default baudrate as 115200.
 * 3. UART_ID_Type enumeration has values UART0_ID, UART1_ID, UART2_ID and
 * UART3_ID, each of which represents individual UART device.
 * 4. External buffer protection is needed in uart_drv_write().
 * 5. The caller must provide the read buffer in uart_drv_read().
 *
 */

#ifndef _MDEV_UART_H_
#define _MDEV_UART_H_

#include <mdev.h>
#include <lowlevel_drivers.h>

/** Flag to enable UART in 8bit mode */
#define UART_8BIT 0
/** Flag to enable UART in 9bit mode */
#define UART_9BIT 1

#define UART_LOG(...)  ll_log("[uart] " __VA_ARGS__)

/** Size of UART RX and TX ring buffers */
#ifdef CONFIG_UART_LARGE_RCV_BUF
#define UARTDRV_SIZE_RX_BUF     1024   /* power of 2 */
#else
#define UARTDRV_SIZE_RX_BUF     256    /* power of 2 */
#endif
#define UARTDRV_SIZE_TX_BUF     256    /* power of 2 */

#define UARTDRV_DEFAULT_BAUDRATE 115200

/** Software flow control parameters */
#define UARTDRV_THRESHOLD_RX_BUF    64
#define XON    0x11
#define XOFF   0x13
#define ESC    0x1b

/** DEFAULT_DMA_BLK_SIZE is the default block size for DMA transfer.
 * If UARTDRV_SIZE_RX_BUF = DEFAULT_DMA_BLK_SIZE,
 * internal software buffer size is fixed to UARTDRV_SIZE_RX_BUF
 * If UARTDRV_SIZE_RX_BUF > DEFAULT_DMA_BLK_SIZE,
 * internal software buffer size is fixed to greatest possible multiple of
 * DEFAULT_DMA_BLK_SIZE.
 * It is not allowed to have UARTDRV_SIZE_RX_BUF smaller than
 * DEFAULT_DMA_BLK_SIZE
 */
#define DEFAULT_DMA_BLK_SIZE 256

/**
 * Enum to enable and disable DMA mode for uart transfer
 * */
typedef enum {
	/** Disable UART DMA controller */
	UART_DMA_DISABLE = 0,
	/** Enable UART DMA controller */
	UART_DMA_ENABLE,
} UART_DMA_MODE;

/** Enum to select flow control method to be used for
*   uart transfer
*/
typedef enum {
	/** Flow control disabled */
	FLOW_CONTROL_NONE = 0,
	/** Software flow control */
	FLOW_CONTROL_SW,
	/** Hardware flow control */
	FLOW_CONTROL_HW,
} flow_control_t;

/** Initialize UART Driver
 *
 * This function initializes UART driver. And registers the device with the mdev
 * interface.
 *
 * \param[in] id Port ID of UART.
 * \param[in] mode \ref UART_8BIT or \ref UART_9BIT.
 * \return WM_SUCCESS on success
 * \return -WM_FAIL on error.
 */
int uart_drv_init(UART_ID_Type id, int mode);

/** De-initialize UART Driver
 *
 * This function de-initializes UART driver. Note that if a UART device has
 * been opened using uart_drv_open(), it should be first closed using
 * uart_drv_close() before calling this function.
 *
 * \param[in] id Port ID of UART.
 */
void uart_drv_deinit(UART_ID_Type id);

/** Set UART Transfer Mode
 *
 * UART supports DMA signalling with handshaking signals.
 * This is an optional call to set uart transfer mode. By default
 * UART transfer mode is set to UART_DMA_DISABLE mode. This call
 * should be made after uart_drv_init() and before uart_drv_open() to
 * over-write default transfer mode and to enable DMA transfer mode.
 *
 * If DMA is enabled then set appropriate DMA block size using
 * uart_drv_dma_rd_blk_size(). uart_drv_read() can read data from internal
 * ringbuffer only if block size of data is received by uart driver.
 *
 * \param[in] port_id Port ID of UART.
 * \param[in] dma_mode \ref UART_DMA_DISABLE or \ref UART_DMA_ENABLE.
 * \return WM_SUCCESS on success.
 * \return -WM_FAIL on error.
 */
int uart_drv_xfer_mode(UART_ID_Type port_id, UART_DMA_MODE dma_mode);

/** Set timeout for UART operations
 *
 * This function sets the timeout for UART operations.
 * It is disabled by default.
 * @pre uart_drv_init()
 *
 * @param[in] port_id UART port number
 * @param[in] tx_tout timeout value in milli seconds, or zero to
 *            disable timeout(i.e.return immediately without
 *            waiting for transfer completion)
 * @param[in] rx_tout timeout value in milli seconds, or zero to
 *            disable timeout.(i.e.return immediately without waiting
 *            for transfer completion)
 */
void uart_drv_timeout(UART_ID_Type port_id, unsigned long tx_tout,
		unsigned long rx_tout);

/** Open handle to UART Device
 *
 * This function opens the handle to UART device and enables application to use
 * the device. This handle should be used in other calls related to UART device.
 *
 * In case UART port enables DMA using \ref uart_drv_xfer_mode, this API
 * configures and enables UART-DMA. If \ref uart_drv_dma_rd_blk_size() is
 * configured, UART port receives the data in chunks of size DMA_BLK_SIZE and
 * moves the data into the software FIFO. This data can be retrieved using the
 * API \ref uart_drv_read().
 *

 * \param[in] port_id Port ID of UART.
 * \param[in] baud Baud rate.
 * \return Handle to the UART device.
 * \return NULL on error.
 */
mdev_t *uart_drv_open(UART_ID_Type port_id, uint32_t baud);

/** Close handle to UART Device
 *
 * This function closes the handle to UART device and resets the UART.
 *
 * \param[in] dev Handle to the UART device returned by uart_drv_open().
 * \return WM_SUCCESS on success
 * \return -WM_FAIL on error.
 */
int uart_drv_close(mdev_t *dev);

/** Read data from UART (DMA or IO mode)
 *
 * This function is used to read data arriving serially over serial in line.
 *
 * \param[in] dev Handle to the UART device returned by uart_drv_open().
 * \param[out] buf Pointer to an allocated buffer of size equal to or more
 * than the value of the third parameter num.
 * \param[in] num The maximum number of bytes to be read from the UART.
 * Note that the actual read bytes can be less than this.
 * \return Number of bytes read.
 */
uint32_t uart_drv_read(mdev_t *dev, uint8_t *buf, uint32_t num);

/** Write data to UART (DMA or IO mode)
 *
 * This function is used to write data to the UART in order to transmit them
 * serially over serial out line.
 *
 * \param[in] dev Handle to the UART device returned by uart_drv_open().
 * \param[in] buf Pointer to a buffer which has the data to be written out.
 * \param[in] num Number of bytes to be written.
 * \return Number of bytes written.
 */
uint32_t uart_drv_write(mdev_t *dev, const uint8_t *buf, uint32_t num);

/** Get UART Port Id
 *
 * This is used to get the port ID of the registered UART. Port ID of UART can
 * have one of the values from UART_ID_Type enumeration mentioned in usage
 * section.
 *
 * \param[in] dev Handle to the UART device returned by uart_drv_open().
 * \return UART Port Id.
 */
uint32_t uart_drv_get_portid(mdev_t *dev);

/** Flush UART transmit buffer
 *
 * This is used to flush all the data from UART transmit FIFO buffer.
 *
 * \param[in] dev Handle to the UART device returned by uart_drv_open().
 */
void uart_drv_tx_flush(mdev_t *dev);

/** Set receive Ring-buffer size
 *
 * This is optional call to change receive ringbuffer size. By default
 * ringbuffer size is set to 1024/256 bytes; based on the value of the flag
 * \ref UARTDRV_SIZE_RX_BUF.
 * This call should be made after uart_drv_init() and before uart_drv_open()
 * to over-write default configuration.
 *
 * \param[in] uart_id Port ID of UART.
 * \param[in] size size of ring buffer in bytes.
 * \return WM_SUCCESS on success.
 * \return -WM_FAIL on error.
 */
int uart_drv_rxbuf_size(UART_ID_Type uart_id, uint32_t size);

/** Set DMA read block length
 *
 * This is optional call to change DMA block length size. By default
 * DMA read block size is set to 128 bytes. This should be called
 * only after enabling DMA mode. DMA mode can be enabled using
 * uart_drv_xfer_mode(). Block size should be set in between 1 to 1023.
 *
 * uart_drv_read() can read data from internal ringbuffer only if block size of
 * data is received by uart driver. Though uart driver internally truncates
 * software ringbuffer buffer length depending on DMA block size, it is
 * recommended to set block size multiple of ringbuffer. Size of internal
 * ringbuffer can be changed using uart_drv_rxbuf_size().
 *
 * \param[in] uart_id Port ID of UART.
 * \param[in] size DMA block size in between 1 to 1023.
 * \return WM_SUCCESS on success.
 * \return -WM_FAIL on error.
 */
int uart_drv_dma_rd_blk_size(UART_ID_Type uart_id, uint32_t size);

/** Set UART line options
 *
 * This is optional call to change UART line options. By default
 * parity is set to NONE and stopbits to ONE.
 * This call should be made after uart_drv_init() and before uart_drv_open()
 * to over-write default configuration.
 * Parameter flow_control is to enable and disable software flow control.
 * If enabled, when uart_rx_buffer is almost full, it sends XOFF to the
 * transmitter to pause the transmission and when enough free space is
 * available in uart_rx_buffer, it sends XON to transmitter to resume the
 * transmission. Default values of XON and XOFF are defined in mdev_uart.h,
 * which are standard values 0x11 and 0x13. In case of binary data, if data
 * contains binary strings same as 0x11 or 0x13, they are escaped using
 * escape character, which is also defined in mdev_uart.h
 *
 * \param[in] uart_id Port ID of UART.
 * \param[in] parity Valid options are UART_PARITY_NONE, UART_PARITY_ODD,
 * UART_PARITY_EVEN.
 * \param[in] stopbits Valid options are UART_STOPBITS_1, UART_STOPBITS_1P5_2.
 * \param[in] flow_control to set flow control mechanism from
 * /ref flow_control_t options.
 *
 * \return WM_SUCCESS on success
 * \return -WM_FAIL on error.
 */
int uart_drv_set_opts(UART_ID_Type uart_id, uint32_t parity, uint32_t stopbits,
		      flow_control_t flow_control);

/** Make uart_drv_read() call blocking
 *
 * This is an optional call to make uart_drv_read() call blocking. By default
 * uart_drv_read() call is non-blocking. This call should be made after
 * uart_drv_init() & before uart_drv_open() to overwrite the default
 * configuration. Usage of this call is only for non-DMA mode UART. If enabled,
 * uart_drv_read() will be blocking till it reads greater than 0 bytes from UART
 * port.
 *
 * \param[in] uart_id Port ID of UART.
 * \param[in] is_blocking true to make uart_drv_read() blocking else false.
 * \return WM_SUCCESS on success.
 * \return -WM_FAIL on error.
 */
int uart_drv_blocking_read(UART_ID_Type uart_id, bool is_blocking);

/**
  * API to reset the UART driver RX buffer
  * This call is optional and may be used if the user wants to discard the old
  * data stored in the RX buffer.
  * @param[in] dev Handle to the UART device returned by uart_drv_open().
  * @return WM_SUCCESS on success.
  * @return -WM_FAIL on error.
  */
int uart_drv_rx_buf_reset(mdev_t *dev);
#endif /* _MDEV_UART_H_ */
