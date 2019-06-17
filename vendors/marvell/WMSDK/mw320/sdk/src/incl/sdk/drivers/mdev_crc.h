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

/** @file mdev_crc.h
 *
 *  @brief This file contains CRC Driver
 *
 * The CRC driver handles the CRC hardware engine and provides an API
 * to the user for calculation of CRC.
 *
 * \note This CRC driver does not support interrupt based CRC
 * calculations.
 *
 * Follow the steps below to use CRC driver from applications
 *
 * 1. CRC driver initialization
 *
 * Call \ref crc_drv_init  from your application initialization function.
 * This will initialize and register crc driver with the mdev
 * interface.
 *
 * 2. Open CRC device to use
 *
 * \ref crc_drv_open should be called once before using CRC hardware for
 * CRC calculations. Here you will specify the name of the CRC
 * module. There is only one hardware engine. Hence, you should pass
 * MDEV_CRC_0 here. Apart from name you also need to pass the CRC
 * mode. The modes are mentioned in the below enum \ref crc_mode_t
 *
 *
 * Typical example:
 *
 * Opening CRC device
 * \code
 * {
 *
 *     mdev_t *crc_dev;
 *
 *	crc_dev = crc_drv_open("MDEV_CRC_ENGINE_0", CRC32_IEEE);
 *
 * }
 * \endcode
 *
 * 3. Perform CRC calculations.
 *
 * CRC API can be used in two ways, viz block and stream. Which one to use
 * depends on the data availability. If the entire data (over which CRC is to
 * be calculated) is available, use the block operation API
 * \ref crc_drv_block_calc. This API will do the job in one go. However, if
 * you want to calculate the CRC on your data part by part (maybe because
 * only part of the data is available now) then use the stream based API.
 *
 * \note The stream API has a peculiar quirk. It can handle non word
 * (32-bit) aligned data. But, only the first call (among many) to the
 * function \ref crc_drv_stream_feed can have non word aligned length. In
 * all the remaining calls to \ref crc_drv_stream_feed the len should be
 * word aligned.
 *
 * Typical example:
 *
 * Block based operation:
 * \code
 * {
 *       uint32 crc32;
 *       char buf[100];
 *       crc_drv_block_calc(crc_dev, buf, 100, &crc32);
 *       wmprintf("The CRC32 result is: 0x%x\n\r", crc32);
 * }
 * \endcode
 *
 * Stream based operation (word aligned data):
 * \code
 * {
 *      const uint32_t total_data_size = 1024;
 *      uint32_t total_read_data = 0;
 *      uint32_t crc32;
 *      char buf[16];
 *      int size_read;
 *      crc_drv_stream_setlen(crc_dev, total_data_size);
 *      while(total_read_data < total_data_size) {
 *             xxx_fetch_data(&buf, sizeof(buf));
 *             crc_drv_stream_feed(crc_dev, buf, sizeof(buf));
 *             total_read_data += sizeof(buf);
 *      }
 *      crc_drv_stream_get_result(crc_dev, &crc32);
 *      wmprintf("CRC result: 0x%x\n\r", crc32);
 * }
 * \endcode
 *
 * Stream based operation (non word aligned data):
 * We know that the data size is non-word aligned. So, we will call
 * \ref crc_drv_stream_get_result first time with non-aligned word
 * data. All the later calls would be word aligned.
 *
 * \code
 * {
 *      const uint32_t total_data_size = 17;
 *      uint32_t remaining_size;
 *      uint32_t crc32;
 *      char buf[16];
 *      int size_read;
 *      crc_drv_stream_setlen(crc_dev, total_data_size);
 *
 *      int tail = total_data_size % 4;
 *      xxx_fetch_data(&buf, tail);
 *      crc_drv_stream_feed(crc_dev, buf, tail);
 *      remaining_size -= tail;
 *
 *      xxx_fetch_data(&buf, remaining_size);
 *      crc_drv_stream_feed(crc_dev, buf, remaining_size);
 *      crc_drv_stream_get_result(crc_dev, &crc32);
 *      wmprintf("CRC result: 0x%x\n\r", crc32);
 * }
 * \endcode
 *
 * 4. Closing a device.
 *
 * A call to \ref crc_drv_close informs the CRC driver to release the
 * resources that it is holding. It is always good practice to free the
 * resources after use.
 *
 * Typical usecase:
 * \code
 * crc_drv_close(crc_dev);
 * \endcode
 */

#ifndef _MDEV_CRC_H_
#define _MDEV_CRC_H_

#include <mdev.h>
#include <lowlevel_drivers.h>

#define MDEV_CRC_ENGINE_0          "MDEV_CRC_ENGINE_0"

/** Emum to maintain various modes of CRC operation.
 *
 * The below enum members also each have a comment above them mentioning
 * the exact polynomial used by the hardware for CRC computation.
 */
typedef enum {
	/* x^16+x^12+x^5+1 */
	CRC16_CCITT,
	/* x^16+x^15+x^2+1 */
	CRC16_IBM,
	/* x^16+x^15+x^11+x^9+x^8+x^7+x^5+x^4+x^2+x+1 */
	CRC16_T10_DIF,
	/* x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8 +x^7+x^5+x^4+x^2+x+1 */
	CRC32_IEEE,
} crc_mode_t;

/**
 * Init CRC Driver
 *
 * This function registers the CRC driver with the mdev interface.
 *
 * \return WM_SUCCESS on success
 * \return error code otherwise.
 */
int crc_drv_init(void);

/**
 *  Open CRC Device
 *
 * This function opens the device for use and returns a handle. This handle
 * should be used for other calls to the CRC driver.
 *
 * \param[in] name Name of the driver(MDEV_CRC_ENGINE_0) to be opened.
 * \param[in] mode The mode of operation of the CRC engine. One of the enum
 * \ref crc_mode_t
 *
 * \return NULL if error, mdev_t handle otherwise.
 */
mdev_t *crc_drv_open(const char *name, crc_mode_t mode);

/**
 * Calculate CRC in a block operation.
 *
 * \param[in] dev The pointer returned in earlier call to \ref crc_drv_open
 * \param[in] buf The buffer holding the data whose CRC is to be
 * calculated.
 * \param[in] len The length of the data in buffer.
 * \param[out] result The calculated CRC
 *
 * \return Standard SDK return codes.
 */
int crc_drv_block_calc(mdev_t *dev, uint8_t* buf, uint32_t len,
		       uint32_t *result);

/**
 * Set length of the stream data. API for CRC stream based operation only.
 *
 * \param[in] dev The pointer returned in earlier call to \ref crc_drv_open
 * \param[in] len The total length of the stream data
 *
 * \return Standard SDK return codes.
 */
int crc_drv_stream_setlen(mdev_t *dev, uint32_t len);

/**
 * Feed stream data to CRC driver. API for CRC stream based operation
 * only.
 *
 * \param[in] dev The pointer returned in earlier call to \ref crc_drv_open
 * \param[in] buf The buffer holding the data whose CRC is to be
 * calculated.
 * \param[in] len The length of the data in buffer.
 *
 * \return Standard SDK return codes.
 */
int crc_drv_stream_feed(mdev_t *dev, uint8_t *buf, uint32_t len);

/**
 * Get the result of the stream data CRC. API for CRC stream based
 * operation only.
 *
 * \param[in] dev The pointer returned in earlier call to \ref crc_drv_open
 * \param[out] result The calculated CRC
 *
 * \return Standard SDK return codes.
 */
int crc_drv_stream_get_result(mdev_t *dev, uint32_t *result);

/**
 * Close the device instance.
 *
 * \param[in] dev The pointer returned in earlier call to \ref crc_drv_open
 *
 * \note Calling this function (after your work with CRC engine is over) is
 * essential. Failing to do so will result in failure of the CRC driver.
 *
 * \return void
 */
void crc_drv_close(mdev_t *dev);

#endif /* !_MDEV_CRC_H_ */
