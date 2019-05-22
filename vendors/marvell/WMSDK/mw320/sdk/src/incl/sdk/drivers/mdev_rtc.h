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

/** @file mdev_rtc.h
 *
 *  @brief This file contains RTC driver
 *
 * RTC (Real Time Clock) driver provides mdev interface to access the
 * internal RTC. Follow the steps below to use RTC driver from
 * applications
 *
 * 1. RTC driver initialization
 *
 * Call \ref rtc_drv_init from your application initialization function.
 * This will initialize and register rtc driver with the mdev interface.
 *
 * 2. Open RTC device to use
 *
 * \ref rtc_drv_open should be called once before using RTC hardware for
 * communication.
 *
 * \code
 * {
 *	mdev_t *rtc_dev;
 *	rtc_dev = rtc_drv_open("MDEV_RTC");
 * }
 * \endcode
 *
 * 3. Set RTC configuration
 *
 * Call \ref rtc_drv_set from your application to set desired upper count
 * The RTC runs at 1KHz by default (for A0 version only).
 *
 * 4. Set RTC callback function
 *
 * Call \ref rtc_drv_set_cb from your application to register callback handler
 * with RTC driver. This will be called after clock exceeds the upper value
 * specified in step 3, until RTC is explicitly stopped using rtc_drv_stop.
 * While RTC continues to run , this will be called everytime the counter value
 * wraps around. RTC can be stopped by calling rtc_drv_stop.
 *
 * 5. Reset the RTC counter
 *
 * Call \ref rtc_drv_reset to reset the rtc counter before starting .
 *
 * 5. Start RTC operation
 *
 * Call \ref rtc_drv_start to start rtc.
 *
 * 6. Get RTC counter value
 *
 * Call \ref rtc_drv_get to rtc counter value.
 *
 * 7. Stop RTC operation
 *
 * Call \ref rtc_drv_stop to stop rtc.
 *
 * 8. Closing a device.
 *
 * A call to \ref rtc_drv_close informs the RTC driver to release the
 * resources that it is holding.
 *
 * \note This must not be called from callback registered in step 4 or any
 * interrupt context.
 *
 * Typical usecase:
 * \code
 * rtc_drv_close(rtc_dev);
 * \endcode
 *
 */

#ifndef _MDEV_RTC_H_
#define _MDEV_RTC_H_

#include <mdev.h>
#include <wmlog.h>
#include <lowlevel_drivers.h>

#define RTC_LOG(...)  wmlog("rtc", ##__VA_ARGS__)

/** Register RTC Device
 *
 * This function registers rtc driver with mdev interface.
 *
 * \return WM_SUCCESS on success, error code otherwise.
 */
int rtc_drv_init(void);

/** Open RTC Device
 *
 * This function opens the device for use and returns a handle. This handle
 * should be used for other calls to the RTC driver.
 *
 * \param[in] name Name of the driver to be opened. Name should be "MDEV_RTC"
 *            string
 * \return NULL if error, mdev_t handle otherwise.
 */
mdev_t *rtc_drv_open(const char *name);

/** Set RTC Configuration
 *
 * This function sets configuration for rtc device.
 *
 * \param[in] dev mdev_t handle to the driver.
 * \param[in] cnt_upp Overflow value. Counting starts from 0
 * and does to to this value. Frequency is 1KHz by default for A0 version of
 * chip and 32KHz for Z1 or earlier chips.
 */
void rtc_drv_set(mdev_t *dev, uint32_t cnt_upp);

/** Set RTC callback handler for UPP Val
 *
 * This function sets callback handler with RTC driver.
 *
 * \param[in] user_cb application registered callback handler, if func is NULL
 * then the callback is unset.
 */
void rtc_drv_set_cb(void (*user_cb)());

/** Set RTC callback handler for Alarm
 *
 * This function sets callback handler with RTC driver.
 *
 * \param[in] user_cb application registered callback handler, if func is NULL
 * then the callback is unset.
 */
void rtc_drv_set_alarm_cb(void (*user_cb)());

/** Reset RTC counter
 *
 * This function resets the RTC counter.
 *
 * \param[in] dev mdev_t handle to the driver.
 */
void rtc_drv_reset(mdev_t *dev);

/** Start RTC device
 *
 * This function starts RTC operation.
 *
 * \param[in] dev mdev_t handle to the driver.
 */
void rtc_drv_start(mdev_t *dev);

/** Get RTC counter value.
 *
 * This function gets the value of RTC counter.
 *
 * \param[in] dev mdev_t handle to the driver.
 * \return the counter value.
 */
unsigned int rtc_drv_get(mdev_t *dev);

/** Get RTC upper count value.
 *
 * This function gets the value of RTC upper
 * count. That is the value till which the RTC
 * up-counts.
 *
 * \param[in] dev mdev_t handle to the driver.
 * \return the upper count value.
 */
unsigned int rtc_drv_get_uppval(mdev_t *dev);

/** Set RTC ALARM
 *
 * This function sets Alarm configuration for rtc device.
 *
 * \param[in] dev mdev_t handle to the driver.
 * \param[in] cnt_alarm  value at which Alarm interrupt will be generated.
 *  Frequency is 1KHz by default.
 */
void rtc_drv_set_alarm(mdev_t *dev, uint32_t cnt_alarm);

/** Stop RTC device
 *
 * This function stops RTC operation.
 *
 * \param[in] dev mdev_t handle to the driver.
 */
void rtc_drv_stop(mdev_t *dev);

/** Close RTC Device
 *
 * This function closes the device after use and frees up resources.
 *
 * \param[in] dev mdev handle to the driver to be closed.
 * \return WM_SUCCESS on success, -WM_FAIL on error.
 */
int rtc_drv_close(mdev_t *dev);

#endif /* _MDEV_RTC_H_ */
