/** @file flash.c
 *
 *  @brief This file provides  API for Flash
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


#include <wmtypes.h>
#include <wm_os.h>
#include <wmstdio.h>
#include <flash.h>
#include <mdev_iflash.h>
#ifdef CONFIG_XFLASH_DRIVER
#include <peripherals/xflash.h>
#endif
#ifdef CONFIG_SPI_FLASH_DRIVER
#include <peripherals/spi_flash.h>
#endif

int flash_drv_get_id(mdev_t *dev, uint64_t *id)
{
	flash_cfg *fl_cfg = (flash_cfg *) dev->private_data;

	if (id == NULL)
		return -WM_FAIL;

	switch (fl_cfg->fl_dev) {
	case FL_INT:
		return iflash_drv_get_id(dev, id);
#ifdef CONFIG_XFLASH_DRIVER
	case FL_EXT:
		/* This function is only for internal flash.
		 * So, as of now, return error if mdev device of external
		 * flash is passed.
		 */
		return -WM_FAIL;
#endif
#ifdef CONFIG_SPI_FLASH_DRIVER
	case FL_SPI_EXT:
		return spi_flash_drv_read_id(dev, (uint8_t *)id);
#endif
	default:
		fl_d("invalid flash device type: %d", fl_cfg->fl_dev);
		return -WM_FAIL;
	}
}

int flash_drv_eraseall(mdev_t *dev)
{
	flash_cfg *fl_cfg = (flash_cfg *) dev->private_data;

	switch (fl_cfg->fl_dev) {
	case FL_INT:
		return iflash_drv_eraseall(dev);
#ifdef CONFIG_XFLASH_DRIVER
	case FL_EXT:
		return xflash_drv_eraseall(dev);
#endif
#ifdef CONFIG_SPI_FLASH_DRIVER
	case FL_SPI_EXT:
		return spi_flash_drv_eraseall(dev);
#endif
	default:
		fl_d("invalid flash device type: %d", fl_cfg->fl_dev);
		return -WM_FAIL;
	}
}

int flash_drv_erase(mdev_t *dev, unsigned long start, unsigned long size)
{
	flash_cfg *fl_cfg = (flash_cfg *) dev->private_data;

	switch (fl_cfg->fl_dev) {
	case FL_INT:
		return iflash_drv_erase(dev, start, size);
#ifdef CONFIG_XFLASH_DRIVER
	case FL_EXT:
		return xflash_drv_erase(dev, start, size);
#endif
#ifdef CONFIG_SPI_FLASH_DRIVER
	case FL_SPI_EXT:
		return spi_flash_drv_erase(dev, start, size);
#endif
	default:
		fl_d("invalid flash device type: %d", fl_cfg->fl_dev);
		return -WM_FAIL;
	}
}

int flash_drv_read(mdev_t *dev, uint8_t *buf,
		   uint32_t len, uint32_t addr)
{
	flash_cfg *fl_cfg = (flash_cfg *) dev->private_data;

	switch (fl_cfg->fl_dev) {
	case FL_INT:
		return iflash_drv_read(dev, buf, len, addr);
#ifdef CONFIG_XFLASH_DRIVER
	case FL_EXT:
		return xflash_drv_read(dev, buf, len, addr);
#endif
#ifdef CONFIG_SPI_FLASH_DRIVER
	case FL_SPI_EXT:
		return spi_flash_drv_read(dev, buf, len, addr);
#endif
	default:
		fl_d("invalid flash device type: %d", fl_cfg->fl_dev);
		return -WM_FAIL;
	}
}

int flash_drv_write(mdev_t *dev, const uint8_t *buf,
		    uint32_t len, uint32_t addr)
{
	flash_cfg *fl_cfg = (flash_cfg *) dev->private_data;

	switch (fl_cfg->fl_dev) {
	case FL_INT:
		return iflash_drv_write(dev, (uint8_t *)buf, len, addr);
#ifdef CONFIG_XFLASH_DRIVER
	case FL_EXT:
		return xflash_drv_write(dev, (uint8_t *)buf, len, addr);
#endif
#ifdef CONFIG_SPI_FLASH_DRIVER
	case FL_SPI_EXT:
		return spi_flash_drv_write(dev, (uint8_t *)buf, len, addr);
#endif
	default:
		fl_d("invalid flash device type: %d", fl_cfg->fl_dev);
		return -WM_FAIL;
	}
}

int flash_drv_close(mdev_t *dev)
{
	flash_cfg *fl_cfg = (flash_cfg *) dev->private_data;

	switch (fl_cfg->fl_dev) {
	case FL_INT:
		return iflash_drv_close(dev);
#ifdef CONFIG_XFLASH_DRIVER
	case FL_EXT:
		return xflash_drv_close(dev);
#endif
#ifdef CONFIG_SPI_FLASH_DRIVER
	case FL_SPI_EXT:
		return spi_flash_drv_close(dev);
#endif
	default:
		fl_d("invalid flash device type: %d", fl_cfg->fl_dev);
		return -WM_FAIL;
	}
}

mdev_t *flash_drv_open(int fl_dev)
{
	mdev_t *mdev_p;

	switch (fl_dev) {
	case FL_INT:
		mdev_p =  iflash_drv_open("iflash", 0);
		break;
#ifdef CONFIG_XFLASH_DRIVER
	case FL_EXT:
		mdev_p = xflash_drv_open("xflash", 0);
		break;
#endif
#ifdef CONFIG_SPI_FLASH_DRIVER
	case FL_SPI_EXT:
		mdev_p = spi_flash_drv_open("spi_flash", 0);
		break;
#endif
	default:
		fl_d("invalid flash device type: %d", fl_dev);
		mdev_p = NULL;
		break;
	}

	if (mdev_p == NULL) {
		fl_d("driver open called without registering device");
	}

	return mdev_p;
}

void flash_drv_init(void)
{
	iflash_drv_init();
#ifdef CONFIG_SPI_FLASH_DRIVER
	spi_flash_drv_init();
#endif
#ifdef CONFIG_XFLASH_DRIVER
	xflash_drv_init();
#endif
}
