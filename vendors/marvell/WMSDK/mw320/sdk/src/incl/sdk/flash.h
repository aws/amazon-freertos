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


/*! \file flash.h
 * \brief Flash Driver
 *
 *  This is a wrapper driver which abstracts the internal flash
 *  driver (iflash) and the external flash driver (xflash) (if
 *  configured) into one.
 *
 *  Steps to use flash with mdev
 *
 *  1. Flash driver initialization
 *
 *  Call \ref flash_drv_init from your application initialization function.
 *  This will initialize and register both internal and external
 *  flash drivers with mdev interface.
 *
 *  2. Open Flash device for use
 *
 *  This should be performed once before using Flash hardware.
 *  Here is example code to open mdev flash handler:
 *  \code
 *  {
 *
 *      mdev_t *fl_dev;
 *
 * 	fl_dev = flash_drv_open(FL_FTFS_DEV);
 *
 *  }
 * \endcode
 * Note: The parameter must be either FL_INT or FL_EXT depending
 * upon which flash you want to use.
 *
 * 3. Data read/write on Flash
 *
 * \ref flash_drv_read() and \ref flash_drv_write() can be used to
 * read and write data to/from the Flash device. Please note
 * that you should pass the handle returned by \ref flash_drv_open()
 * to the \ref flash_drv_read() and \ref flash_drv_write() functions.
 *
 * 4. Erase Flash
 *
 * \ref flash_drv_erase() amd \ref flash_drv_eraseall are used to
 * erase the flash chip.
 *
 * 5. Closing Flash device.
 *
 * \ref flash_drv_close() call is used to close the flash driver.
 *
 */

#ifndef _FLASH_H_
#define _FLASH_H_

#include <string.h>
#include <wmtypes.h>
#include <mdev.h>
#include <wmstdio.h>
#include <wmlog.h>

#define fl_e(...)				\
	wmlog_e("fl", ##__VA_ARGS__)
#define fl_w(...)				\
	wmlog_w("fl", ##__VA_ARGS__)

#ifdef CONFIG_FLASH_DEBUG
#define fl_d(...)				\
	wmlog("fl", ##__VA_ARGS__)
#define ifl_d(...)				\
	wmlog("ifl", ##__VA_ARGS__)
#define xfl_d(...)				\
	wmlog("xfl", ##__VA_ARGS__)
#define spi_fl_d(...)				\
	wmlog("spi_xfl", ##__VA_ARGS__)
#else
#define fl_d(...)
#define ifl_d(...)
#define xfl_d(...)
#define spi_fl_d(...)
#endif /* ! CONFIG_FLASH_DEBUG */



/* Hardware independent flash identifiers. Hardware can map this to any types
 * of flash devices. */
#define FL_INT		0
#define FL_EXT		1
#define FL_SPI_EXT	2

/** The flash descriptor
 *
 * All components that work with flash refer to the flash descriptor. The flash
 * descriptor captures the details of the component resident in flash.
 */
typedef struct flash_desc {
	/** The flash device */
	uint8_t   fl_dev;
	/** The start address on flash */
	uint32_t  fl_start;
	/** The size on flash  */
	uint32_t  fl_size;
} flash_desc_t;

typedef struct {
	/** The flash device */
	uint8_t fl_dev;
	/** The unique identifier for flash */
	uint64_t fl_id;
} flash_cfg;

/**  Initialize the Flash Driver
 *
 * This will initialize the internal as well as external (if defined)
 * flash driver and register it with the mdev interface.
 *
 * \return void
 */
extern void flash_drv_init(void);

/**  Open the Flash Driver
 *
 * This will open the flash driver for the flash
 * device specified using fl_dev.
 *
 * \param fl_dev Flash device to be opened
 * \return mdev handle if successful, NULL otherwise
 */
extern mdev_t *flash_drv_open(int fl_dev);

/**  Close the flash driver
 *
 * This will close the flash driver
 *
 * \param  dev mdev handle to the driver
 * \return void
 */
extern int flash_drv_close(mdev_t *dev);

/**  Write data to flash
 *
 * This will write specified number of bytes to flash.
 * Note that flash needs to be erased before writing.
 *
 * \param dev mdev handle to the driver
 * \param buf Pointer to the data to be written
 * \param len Length of data to be written
 * \param addr Write address in flash
 * \return 0 on success
 */
extern int flash_drv_write(mdev_t *dev, const uint8_t *buf,
			   uint32_t len, uint32_t addr);

/**  Read data from flash
 *
 * This will read specified number of bytes from flash.
 *
 * \param dev mdev handle to the driver
 * \param buf Pointer to store the read data
 * \param len Length of data to be read
 * \param addr Read address in flash
 * \return 0 on success
 */
extern int flash_drv_read(mdev_t *dev, uint8_t *buf,
			  uint32_t len, uint32_t addr);

/**  Erase flash
 *
 * This will erase the flash for the specified start address
 * and size
 *
 * \param dev mdev handle to the driver
 * \param start Start address in flash
 * \param size Number of bytes to be erased
 * \return 0 on success
 */
extern int flash_drv_erase(mdev_t *dev, unsigned long start,
					unsigned long size);

/**  Erase entire flash
 *
 * This will erase the entire flash chip
 *
 * \param dev mdev handle to the driver
 * \return 0 on success
 */
extern int flash_drv_eraseall(mdev_t *dev);

/** Get the unique id for flash
 * This will get the unique id for the internal flash
 * \param dev mdev handle to the driver
 * \param id a pointer to the 64 bit unique id
 * \return 0 on success
 * \return -WM_FAIL if handle is not for internal flash or id is NULL
 */
extern int flash_drv_get_id(mdev_t *dev, uint64_t *id);

#endif /* _FLASH_H_ */
