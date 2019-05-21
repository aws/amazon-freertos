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

/** @file mdev_acomp.h
 *
 *  @brief This file contains Analog Comparator Driver
 *
 * ACOMP driver provides mdev interface to use 2 analog comparators.
 * Follow the steps below to use ACOMP driver from applications
 *
 * 1. Include Files
 *
 * Include mdev.h and mdev_acomp.h in your source file.
 *
 * 2. ACOMP driver initialization
 *
 * Call \ref acomp_drv_init from your application initialization function.
 * This will initialize and register acomp driver with the mdev interface.
 *
 * 3. Open ACOMP device to use
 *
 * \ref acomp_drv_open should be called once before using ACOMP hardware.
 * Here you will specify ACOMP number (from 0 to 1). The other parameters
 * that are set by default are -<br>
 * ACOMP_HYSTER_0MV - Hysteresis 0mV for positive input<br>
 * ACOMP_HYSTER_0MV - Hysteresis 0mV for negative input<br>
 * LOGIC_LO         - Output LOW during inactive state<br>
 * ACOMP_PWMODE_1   - BIAS Current (Response time)<br>
 * ACOMP_WARMTIME_16CLK - Warm-up time 16 clock cycles<br>
 * ACOMP_PIN_OUT_SYN - Synchronous pin output<br>
 *
 * If these defaults need to be changed they should be changed in
 * \ref acomp_drv_open
 *
 * \code
 * {
 *	mdev_t *acomp_dev;
 *	acomp_dev = acomp_drv_open(MDEV_ACOMP<0-1>);
 * }
 * \endcode
 *
 * 4. Get result/output of ACOMP
 *
 * Call \ref acomp_drv_result to get the output of the mentioned
 * comparator.
 *
 * 5. Closing a device.
 *
 * A call to \ref acomp_drv_close informs the ACOMP driver to release the
 * resources that it is holding.
 *
 *
 */

#ifndef _MDEV_ACOMP_H_
#define _MDEV_ACOMP_H_

#include <mdev.h>
#include <lowlevel_drivers.h>

#define ACOMP_LOG(...)  wmlog("acomp", __VA_ARGS__)

/** Register ACOMP Device
 *
 * This function registers acomp driver with mdev interface.
 *
 * \param[in] id ACOMP device to be used.
 * \return WM_SUCCESS on success, error code otherwise.
 */
int acomp_drv_init(ACOMP_ID_Type id);

/** Open ACOMP Device
 *
 * This function opens the device for use and returns a handle. This handle
 * should be used for other calls to the ACOMP driver.
 *
 * \param[in] acomp_id ACOMP device id (ACOMP_ID_Type) to be opened.
 * \param[in] pos Positive channel to be used.
 * \param[in] neg Negative channel to be used.
 * \return NULL if error, mdev_t handle otherwise.
 */
mdev_t *acomp_drv_open(ACOMP_ID_Type acomp_id, ACOMP_PosChannel_Type pos,
					 ACOMP_NegChannel_Type neg);

/** Get ACOMP Output
 *
 * This function gets the output of acomp device.
 *
 * \param[in] mdev_p mdev_t handle to the driver.
 * \return The output value 0 or 1
 */
int acomp_drv_result(mdev_t *mdev_p);

/** Close ACOMP Device
 *
 * This function closes the device after use and frees up resources.
 *
 * \param[in] dev mdev handle to the driver to be closed.
 * \return WM_SUCCESS on success, -WM_FAIL on error.
 */
int acomp_drv_close(mdev_t *dev);

/** De-initialize ACOMP driver port
 *
 * This function de-initializes ACOMP driver port. Note that if an ACOMP device
 * has been opened using acomp_drv_open(), it should be first closed using
 * acomp_drv_close() before calling this function.
 *
 * \param[in] acomp_id: Port ID of ACOMP.
 *
 * @return -WM_FAIL if operation failed.
 * @return WM_SUCCESS if operation was a success.
 */

int acomp_drv_deinit(ACOMP_ID_Type acomp_id);

#endif /* !_MDEV_ACOMP_H_ */
