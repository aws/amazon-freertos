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

/** @file mdev.h
*
*  @brief Marvell Device Driver API
*
*  This provides the Marvell Device abstraction for communicating with various
*  devices.
*
*  \section mdev_usage Usage
*
*  Drivers typically register an mdev API using the mdev_register() call. The
*  applications can use this device as follows:
*  -# open the device using [driver name]_drv_open()
*  -# perform read and write on the device using [driver name]_drv_read()
*     and [driver name]_drv_write() respectively.
*  -# close the device [driver name]_drv_close()
*/

#ifndef _MDEV_
#define _MDEV_

#include <wmtypes.h>

/* Data structure associated with the marvell device drivers*/
typedef struct _mdev_t {
	uint32_t  dev_id;
	struct _mdev_t      *pNextMdev;
	const char                *name;
	/* Function Number for Multifunction IO */
	uint32_t  port_id;
	uint32_t private_data;
} mdev_t;

typedef uint32_t (*MDEV_READ_CALLBACK) (mdev_t *dev,
					uint8_t *buf,
					uint32_t nbytes,
					uint32_t flags);

extern void mdev_Init(void);

extern mdev_t *hid_dev;
extern mdev_t *wlan_dev;
extern mdev_t *tcpip_dev;

#if defined (FMRX)
extern mdev_t *fmrx_dev;
#endif

 /*******************************************************************/
/*!
 *  \name Marvell Device Driver Accessory Functions
 * @{
 */
/******************************************************************/
/*!
 *  \brief This functions registers a marvell device for use.
 *
 *  \param[in] dev device handler
 *  \return PASS/FAIL
 */
uint32_t mdev_register(mdev_t *dev);

/*!
 *  \brief This function deregisters a marvell devices from use.
 *
 *  \param[in] name name of the device to deregister
 *  \return PASS/FAIL
 */
uint32_t mdev_deregister(const char *name);

/*!
 *  \brief This function gets a handle for marvell devices.
 *
 *  \param[in] dev_name name of the device to get handle
 *  \return dev handle to the device
 */
mdev_t * mdev_get_handle(const char * dev_name);

/*!
 *  \brief This function opens a marvell devices for use.
 *
 *  \param[in] dev_name name of the device to open
 *  \param[in] flags any flags to be used while opening the driver
 *  \return dev handle to the opened device
 */

#ifdef CONFIG_MDEV_TESTS
int mdev_cli_init();
#endif
#endif /* _MDEV_ */
