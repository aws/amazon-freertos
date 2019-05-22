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


/*! \file mdev_usb_host.h
 *  \brief Universal Serial Bus (USB) Host driver
 *
 *  The USB host driver is used to initiate and arbitrate communication over
 *  USB-OTG port of MCU. It is meant to use USB hardware to communicate with
 *  external devices connected on the USB-OTG bus.
 *
 *  @section mdev_usb_host_usage Usage
 *
 *  A typical USB host usage scenario is as follows:
 *
 *  -# Initialize USB host driver using class specific usb_host_xxx_drv_init().
 *  -# Open the USB host handle using usb_host_drv_open() call.
 *  -# Configure and start USB device using usb_host_drv_start() call
 *  -# Use usb_host_drv_read() or usb_host_drv_write() calls to read USB data
 *   or to write USB data respectively.
 *  -# Use usb_host_drv_block_read() or usb_host_drv_block_write() calls to
 *   read or write block/sector wise data from a USB device respectively.
 *  -# Use usb_host_drv_info() to get USB device specific information
 *  -# Stop USB device using usb_host_drv_stop() call
 *  -# Close USB host using usb_host_drv_close() call after its use.
 *
 *  Code snippet:\n
 *  Following code demonstrate how to use USB CDC host driver APIs for
 *  read/write operation. It sets line coding parameters as 9600,8,N,1 and
 *  reads 4 bytes of data and writes it back.
 *
 *  @code
 *  {
 *   int ret;
 *   usb_cdc_drv_data drv;
 *   mdev_t *usb_dev;
 *   char buf[4];
 *
 *   drv.Parity = 0;
 *   drv.Baud = 9600;
 *   drv.StopBits = 1;
 *   drv.WordLength = 8;
 *
 *   ret = usb_host_cdc_drv_init(&drv);
 *   if (ret != WM_SUCCESS)
 *	return;
 *
 *   usb_dev = usb_host_drv_open("MDEV_USB_HOST");
 *   if (usb_dev == NULL)
 *	return;
 *
 *   ret = usb_host_drv_start(usb_dev);
 *   if (ret != WM_SUCCESS)
 *	return;
 *
 *   while((usb_host_drv_read(usb_dev, buf, sizeof(buf))) <= 0);
 *   ret = usb_host_drv_write(usb_dev, buf, sizeof(buf));
 *   if (ret < 0)
 *	return;
 *   usb_host_drv_stop(usb_dev);
 *   usb_host_drv_close(usb_dev);
 *  }
 *  @endcode
 *
 */

#ifndef _MDEV_USB_HOST_H_
#define _MDEV_USB_HOST_H_

#include <mdev.h>
#include <wmerrno.h>
#include <usbvideo.h>
#include <usbaudio.h>
#include <usbcdc.h>
#include <usbdisk.h>
#include <usbclass.h>

#define USB_HOST_LOG(...)  wmlog("usb-host", __VA_ARGS__)

/** Enable/Disable debug logs for USB UVC Class Driver */
#define USB_UVC_DEBUG 1
/** Enable/Disable debug logs for USB CDC Class Driver */
#define USB_CDC_DEBUG 1

/** Initialize USB Host Driver for USB audio class (UAC) devices only
 *
 * This function registers the USB Host driver with the mdev interface and
 * initializes the USB device with the audio class.
 *
 * \param [in] data USB Host class specific data pointer
 * \return WM_SUCCESS on success
 * \return -WM_FAIL on failure
 */
int usb_host_uac_drv_init(void *data);

/** Initialize USB Host Driver for USB video class (UVC) devices only
 *
 * This function registers the USB Host driver with the mdev interface and
 * initializes the USB device with the video class.
 *
 * \param [in] data USB Host class specific data pointer
 * \return WM_SUCCESS on success
 * \return -WM_FAIL on failure
 * \note Video class driver currently supports  STREAM_MJPEG and
 * STREAM_UNCOMPRESSED video formats only
 */
int usb_host_uvc_drv_init(void *data);

/** Initialize USB Host Driver for USB communication class (CDC) devices only
 *
 * This function registers the USB Host driver with the mdev interface and
 * initializes the USB device with the communication class.
 *
 * \param [in] data USB Host class specific data pointer
 * \return WM_SUCCESS on success
 * \return -WM_FAIL on failure
 */
int usb_host_cdc_drv_init(void *data);

/** Initialize USB Host Driver for USB mass storage class (MSC) devices only
 *
 * This function registers the USB Host driver with the mdev interface and
 * initializes the USB device with the mass storage class.
 *
 * \param [in] data USB Host class specific data pointer
 * \return WM_SUCCESS on success
 * \return -WM_FAIL on failure
 */
int usb_host_msc_drv_init(void *data);

/** Open USB device
 *
 * This function enables (power on) USB Host EHCI Controller.
 *
 * This returns a handle that should be used for other USB Host driver calls.
 *
 * \param [in] name Name of mdev usb driver. It should be "MDEV_USB_HOST"
 * string.
 * \return handle to driver on success
 * \return NULL otherwise
 */
mdev_t *usb_host_drv_open(const char *name);

/** Start USB device
 *
 * In addition specific USB Class driver can do its own implementation,
 * e.g. for video class this function will configure USB video device with
 * configuration provided during init phase and starts video streaming.
 *
 * \param [in] dev mdev handle returned from call to usb_drv_open()
 *
 * \return WM_SUCCESS on success
 * \return error code on failure
 */
int usb_host_drv_start(mdev_t *dev);

/** Stop USB device
 *
 * In addition specific USB Class driver can do its own implementation,
 * e.g. for video class this function will stop video streaming from end
 * device, putting interface into zero bandwidth mode.
 *
 * \param [in] dev mdev handle returned from call to usb_drv_open()
 *
 * \return WM_SUCCESS on success
 * \return error code on failure
 */
int usb_host_drv_stop(mdev_t *dev);

/** Get info from USB device
 *
 * This function reads the class specific information into the provided buffer
 * from the USB Host device.
 *
 * \param [in] dev mdev_t handle to the driver
 * \param [out] buf Buffer for storing retrieved device information
 * \return WM_SUCCESS or negative error number
 */
int usb_host_drv_info(mdev_t *dev, void *buf);

/** Read from USB device
 *
 * This function reads the specified number of bytes into the provided buffer
 * from the USB Host device. This call is NON-BLOCKING one.
 *
 * \param [in] dev mdev_t handle to the driver
 * \param [out] buf Buffer for storing the values read
 * \param [in] nbytes Number of bytes read out, 0 if data is not yet available,
 * -1 on buffer overflow error
 * \return number of bytes read
 * \note For USB Video Class, video data will start from frame boundary at
 * first read or after a buffer overflow exception has occurred.
 */
int usb_host_drv_read(mdev_t *dev, void *buf, uint32_t nbytes);

/** Write to USB device
 *
 * This function writes the specified number of bytes to the USB Host device
 * \param[in] dev mdev_t handle to the driver
 * \param[in] buf Pointer to buffer to be written
 * \param[in] nbytes Number for bytes to be written
 * \return number of bytes written on success
 * \return -1 on failure
 */
int usb_host_drv_write(mdev_t *dev, const void *buf, uint32_t nbytes);

/** Block Read from USB device
 *
 * This function reads the specified number of blocks/sectors into the
 * provided buffer from the USB Host device. This call is BLOCKING one.
 *
 * \param [in] dev mdev_t handle to the driver
 * \param [out] buf Buffer for storing the values read
 * \param [in] block_no initial block/sector number from which to read
 * \param [in] no_of_blocks number of blocks/sectors to read
 * \return number of bytes read or negative value on error
 */
int usb_host_drv_block_read(mdev_t *dev, void *buf, uint32_t block_no,
			uint32_t no_of_blocks);

/** Block Write to USB device
 *
 * This function writes the specified number of blocks/sectors from the
 * provided buffer to the USB Host device. This call is BLOCKING one.
 *
 * \param[in] dev mdev_t handle to the driver
 * \param[in] buf Pointer to buffer to be written
 * \param [in] block_no initial block/sector number to which to write
 * \param [in] no_of_blocks number of blocks/sectors to write
 * \return number of bytes written or negative value on error
 */
int usb_host_drv_block_write(mdev_t *dev, const void *buf, uint32_t block_no,
			uint32_t no_of_blocks);

/** Stop USB Host EHCI Controller
 *
 * This function disables (power off) USB Host EHCI Controller.
 *
 * \param [in] dev mdev handle to the driver to be closed
 * \return WM_SUCCESS on success
 */
int usb_host_drv_close(mdev_t *dev);

#endif
