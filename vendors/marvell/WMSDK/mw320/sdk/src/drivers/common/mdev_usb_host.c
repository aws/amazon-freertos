/** @file mdev_usb_host.c
 *
 *  @brief This file provides  mdev driver for USB host
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


#include <wmstdio.h>
#include <mdev_usb_host.h>
#include <wm_os.h>
#include <cli.h>

static mdev_t mdev_usb_host;
static const char *MDEV_NAME_usb_host = "MDEV_USB_HOST";
static os_mutex_t usb_host_mutex;

typedef struct class_drv_funcs {
	int (*drv_read) (uint8_t *buf, uint32_t count);
	int (*drv_write) (uint8_t *buf, uint32_t count);
	int (*drv_info) (uint8_t *buf);
	int (*drv_block_read) (uint8_t *buf, uint32_t block_no,
				uint32_t no_of_blocks);
	int (*drv_block_write) (uint8_t *buf, uint32_t block_no,
				uint32_t no_of_blocks);
	int (*drv_start) ();
	int (*drv_stop) ();
} class_drv_funcs_t;

static class_drv_funcs_t cl_funcs;

void usb_uvc_get_stats(int argc, char **argv)
{
	wmprintf("usb uvc statistics =========:\r\n");
	/* Provide USB-Stack specific implementation here  */
}

void usb_dump_descriptors(int argc, char **argv)
{
	usb_uvc_print_descriptors();
	usb_uac_print_descriptors();
	usb_msc_print_descriptors();
	usb_cdc_print_descriptors();
}

struct cli_command usb_command[] = {
	{"usb_dump_desc", NULL, usb_dump_descriptors},
};

struct cli_command uvc_command[] = {
	{"uvc_stats", NULL, usb_uvc_get_stats},
};

int usb_host_drv_start(mdev_t *dev)
{
	int ret;

	ret = os_mutex_get(&usb_host_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		USB_HOST_LOG("failed to get mutex\n\r");
		return ret;
	}

	if (((class_drv_funcs_t *)(dev->private_data))->drv_start)
		ret = ((class_drv_funcs_t *)(dev->private_data))->
							drv_start();
	os_mutex_put(&usb_host_mutex);
	return ret;
}

int usb_host_drv_stop(mdev_t *dev)
{
	int ret;

	ret = os_mutex_get(&usb_host_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		USB_HOST_LOG("failed to get mutex\n\r");
		return ret;
	}
	if (((class_drv_funcs_t *)(dev->private_data))->drv_stop)
		ret = ((class_drv_funcs_t *)(dev->private_data))->
							drv_stop();

	os_mutex_put(&usb_host_mutex);
	return ret;
}

int usb_host_drv_write(mdev_t *dev, const void *buf, uint32_t nbytes)
{
	int ret, bytes_written = -WM_FAIL;

	ret = os_mutex_get(&usb_host_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		USB_HOST_LOG("failed to get mutex\n\r");
		return ret;
	}

	if (((class_drv_funcs_t *)(dev->private_data))->drv_write)
		bytes_written = ((class_drv_funcs_t *)(dev->private_data))->
			drv_write((void *)buf, nbytes);
	os_mutex_put(&usb_host_mutex);
	return bytes_written;
}

int usb_host_drv_read(mdev_t *dev, void *buf, uint32_t nbytes)
{
	int ret, bytes_read = -WM_FAIL;

	ret = os_mutex_get(&usb_host_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		USB_HOST_LOG("failed to get mutex\n\r");
		return ret;
	}

	if (((class_drv_funcs_t *)(dev->private_data))->drv_read)
		bytes_read = ((class_drv_funcs_t *)(dev->private_data))->
						drv_read(buf, nbytes);
	os_mutex_put(&usb_host_mutex);
	return bytes_read;
}

int usb_host_drv_info(mdev_t *dev, void *buf)
{
	int ret;

	ret = os_mutex_get(&usb_host_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		USB_HOST_LOG("failed to get mutex\n\r");
		return ret;
	}

	if (((class_drv_funcs_t *)(dev->private_data))->drv_info)
		ret = ((class_drv_funcs_t *)(dev->private_data))->
			drv_info(buf);
	os_mutex_put(&usb_host_mutex);
	return ret;
}

int usb_host_drv_block_write(mdev_t *dev, const void *buf, uint32_t block_no,
				uint32_t no_of_blocks)
{
	int ret, bytes_written = -WM_FAIL;

	ret = os_mutex_get(&usb_host_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		USB_HOST_LOG("failed to get mutex\n\r");
		return ret;
	}

	if (((class_drv_funcs_t *)(dev->private_data))->drv_block_write)
		bytes_written = ((class_drv_funcs_t *)(dev->private_data))->
			drv_block_write((void *)buf, block_no, no_of_blocks);
	os_mutex_put(&usb_host_mutex);
	return bytes_written;
}

int usb_host_drv_block_read(mdev_t *dev, void *buf, uint32_t block_no,
				uint32_t no_of_blocks)
{
	int ret, bytes_read = -WM_FAIL;

	ret = os_mutex_get(&usb_host_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		USB_HOST_LOG("failed to get mutex\n\r");
		return ret;
	}

	if (((class_drv_funcs_t *)(dev->private_data))->drv_block_read)
		bytes_read = ((class_drv_funcs_t *)(dev->private_data))->
				drv_block_read(buf, block_no, no_of_blocks);
	os_mutex_put(&usb_host_mutex);
	return bytes_read;
}

int usb_host_drv_close(mdev_t *dev)
{
	int ret;

	ret = os_mutex_get(&usb_host_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		USB_HOST_LOG("failed to get mutex\n\r");
		return ret;
	}

	USB_HOST_ShutDown();
	os_mutex_put(&usb_host_mutex);

	return WM_SUCCESS;
}

mdev_t *usb_host_drv_open(const char *name)
{
	mdev_t *mdev_p = mdev_get_handle(name);
	int ret;

	if (mdev_p == NULL) {
		USB_HOST_LOG("driver open called without registering device"
							" (%s)\n\r", name);
		return NULL;
	}

	ret = os_mutex_get(&usb_host_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		USB_HOST_LOG("failed to get mutex\n\r");
		return NULL;
	}

	USB_HOST_RegisterEHCI((void *)USB_BASE, USB_IRQn);
	os_mutex_put(&usb_host_mutex);

	return mdev_p;
}

static int usb_host_drv_preinit()
{
	int ret;

	memset(&cl_funcs, 0x00, sizeof(cl_funcs));

	if (mdev_get_handle(MDEV_NAME_usb_host) != NULL)
		return WM_SUCCESS;

	mdev_usb_host.name = MDEV_NAME_usb_host;

	ret = os_mutex_create(&usb_host_mutex, "usb-host", OS_MUTEX_INHERIT);
	if (ret == -WM_FAIL) {
		USB_HOST_LOG("Err: Failed to create mutex\r\n");
		return -WM_FAIL;
	}

	USB_HOST_HWInit();
	os_thread_sleep(1000);

	return WM_SUCCESS;
}

static int usb_host_drv_postinit()
{
	int i;

	for (i = 0; i < sizeof(usb_command) /
			sizeof(struct cli_command); i++)
		cli_register_command(&usb_command[i]);

	mdev_register(&mdev_usb_host);

	return WM_SUCCESS;
}

int usb_host_uac_drv_init(void *data)
{
	int ret;

	ret = usb_host_drv_preinit();
	if (ret != WM_SUCCESS)
		return ret;

	cl_funcs.drv_read = usb_uac_drv_read;
	mdev_usb_host.private_data = (uint32_t) &cl_funcs;

	ret = usb_uac_drv_init(data);
	if (ret < 0) {
		USB_HOST_LOG
			("Err: Failed to initialize uac driver\r\n");
		os_mutex_delete(&usb_host_mutex);
		return -WM_FAIL;
	}

	USB_HOST_RegisterCallback(USBAudio);

	usb_host_drv_postinit();

	return WM_SUCCESS;
}

int usb_host_uvc_drv_init(void *data)
{
	int ret, i;

	ret = usb_host_drv_preinit();
	if (ret != WM_SUCCESS)
		return ret;

	cl_funcs.drv_read = usb_uvc_drv_read;
	cl_funcs.drv_start = usb_uvc_drv_start;
	cl_funcs.drv_stop = usb_uvc_drv_stop;
	mdev_usb_host.private_data = (uint32_t) &cl_funcs;

	ret = usb_uvc_drv_init(data);
	if (ret < 0) {
		USB_HOST_LOG
			("Err: Failed to initialize uvc driver\r\n");
		os_mutex_delete(&usb_host_mutex);
		return -WM_FAIL;
	}

	USB_HOST_RegisterCallback(USBVideo);

	for (i = 0; i < sizeof(uvc_command) /
			sizeof(struct cli_command); i++)
		cli_register_command(&uvc_command[i]);

	usb_host_drv_postinit();

	return WM_SUCCESS;
}

int usb_host_cdc_drv_init(void *data)
{
	int ret;

	ret = usb_host_drv_preinit();
	if (ret != WM_SUCCESS)
		return ret;

	cl_funcs.drv_read = usb_cdc_drv_read;
	cl_funcs.drv_write = usb_cdc_drv_write;
	cl_funcs.drv_start = usb_cdc_drv_start;
	cl_funcs.drv_stop = usb_cdc_drv_stop;
	mdev_usb_host.private_data = (uint32_t) &cl_funcs;

	ret = usb_cdc_drv_init(data);
	if (ret < 0) {
		USB_HOST_LOG
			("Err: Failed to initialize cdc driver\r\n");
		os_mutex_delete(&usb_host_mutex);
		return -WM_FAIL;
	}
	USB_HOST_RegisterCallback(USBComm);

	usb_host_drv_postinit();

	return WM_SUCCESS;
}

int usb_host_msc_drv_init(void *data)
{
	int ret;

	ret = usb_host_drv_preinit();
	if (ret != WM_SUCCESS)
		return ret;

	cl_funcs.drv_info = usb_msc_drv_info;
	cl_funcs.drv_block_read = usb_msc_drv_read;
	cl_funcs.drv_block_write = usb_msc_drv_write;
	mdev_usb_host.private_data = (uint32_t) &cl_funcs;

	ret = usb_msc_drv_init(data);
	if (ret < 0) {
		USB_HOST_LOG
			("Err: Failed to initialize usb msc driver\r\n");
		os_mutex_delete(&usb_host_mutex);
		return -WM_FAIL;
	}

	USB_HOST_RegisterCallback(USBDisk);

	usb_host_drv_postinit();

	return WM_SUCCESS;
}
