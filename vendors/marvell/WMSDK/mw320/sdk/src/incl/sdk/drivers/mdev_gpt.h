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

/** @file mdev_gpt.h
 *
 *  @brief This file contains General Purpose Timer (GPT) Driver
 *
 * General Purpose Timer (GPT) driver provides mdev interface
 * to use 4 general purpose hardware timers.
 *
 * @section mdev_gpt_usage Usage
 *
 * A typical GPT device usage scenario is as follows:
 *
 * -# Initialize the GPT driver using gpt_drv_init().
 * -# Open the GPT device handle using gpt_drv_open().
 *    Here caller  must specify GPT number (from 0 to 3)-
 * -# Use gpt_drv_set() to set desired interval in micro-seconds.
 * -# Close the GPT device using gpt_drv_close() after its use.
 *    gpt_drv_close() call informs the GPT driver to release the resources that
 *    it is holding. It is always good practice to free the resources after use.
 * -# gpt_drv_setcb() should be used to set GPT callback function.
 *    This callback will be called after interval specified in gpt_drv_set()
 *    until GPT is explicitly stopped using gpt_drv_stop().
 * -# Start GPT operation using gpt_drv_start().
 * -# Stop GPT operation using gpt_drv_stop().
 *    \note gpt_drv_stop must not be called from callback registered using
 *          gpt_drv_setcb()or from  interrupt context.
 *
 *  Code snippet:\n
 *  Following code demonstrates how to use GPT driver APIs
 *  to generate a timeout.
 *  @code
 *  static void gpt_cb(void)
 *  {
 *     process_gpt_timeout_cb();
 *  }
 *
 *  {
 *    gpt_drv_init(timer_id <0-3>);

 *         .
 *         .
 *         .
 *    mdev_t* gpt_dev = gpt_drv_open(timer_id<0-3>);
 *    if (gpt_dev == NULL)
 *        err_handling();
 *    gpt_drv_set(gpt_dev,time_in_microsecs);
 *    gpt_drv_setcb(gpt_dev, gpt_cb);
 *    gpt_drv_start(gpt_dev);
 *            .
 *            .
 *            .
 *    gpt_drv_stop(gpt_dev);
 *    gpt_drv_close(gpt_dev);
 *    return;
 *  }
 * @endcode
 *
 */

#ifndef _MDEV_GPT_H_
#define _MDEV_GPT_H_

#include <mdev.h>
#include <lowlevel_drivers.h>
#include <wmlog.h>

#define GPT_LOG(...)  wmlog("gpt", ##__VA_ARGS__)

/** Initialize the GPT driver
 *
 *  This function initializes GPT driver and registers it with mdev interface.
 *  @param[in] id GPT device to be used.
 *  @return WM_SUCCESS on success
 *  @return Error code otherwise
 */
int gpt_drv_init(GPT_ID_Type id);

/** Open GPT device driver
 *
 * This function opens the handle to GPT device and enables application to use
 * the device. This handle should be used in other calls related to GPT device.
 *
 *  @param [in] gpt_id GPT device to be opened.
 *  @return handle to driver on success
 *  @return NULL otherwise
 */
mdev_t *gpt_drv_open(GPT_ID_Type gpt_id);

/** Close the GPT device
 *
 *  This function closes the handle to GPT device.
 *
 *  @param [in] dev Handle to the GPT device returned by gpt_drv_open().
 *  @return WM_SUCCESS on Success
 *  @return -WM_FAIL on error
 *  @note Failing to call this function will make GPT<n> timer un-usable.
 */
int gpt_drv_close(mdev_t *dev);

/** Set GPT Configuration
 *
 * This function sets configuration for GPT device.
 *
 *  @param [in] dev Handle to the GPT device returned by gpt_drv_open().
 *  @param[in] interval_us Interval in micro-seconds.
  */
void gpt_drv_set(mdev_t *dev, uint32_t interval_us);


/** Set GPT callback handler
 *
 * This function sets callback handler with GPT driver.
 *
 *  @param [in] dev Handle to the GPT device returned by gpt_drv_open().
 *  @param[in]  func application registered callback handler.
 *              To deregister a callback pass NULL.
 *              This callback will be called after interval
 *              specified in gpt_drv_set() until GPT is explicitly
 *              stopped using gpt_drv_stop().
 */
void gpt_drv_setcb(mdev_t *dev, void (*func) (void));

/** Start GPT device
 *
 * This function starts GPT operation.
 *
 *  @param [in] dev Handle to the GPT device returned by gpt_drv_open().
 */
void gpt_drv_start(mdev_t *dev);

/** Stop GPT device
 *
 * This function stops GPT operation.
 *
 *  @param [in] dev Handle to the GPT device returned by gpt_drv_open().
 *  @ note gpt_drv_stop must not be called from callback registered using
 *          gpt_drv_setcb()or from  interrupt context.
 */
void gpt_drv_stop(mdev_t *dev);

/** Enable PWM using GPT
 *
 *  This function configures the PWM functionality
 *  using the General Purpose Timer.
 *
 *  @note The correct PinMux needs to be set using
 *        the @link mdev_pinmux.h PINMUX driver  @endlink
 *        before calling this function.
 *
 *  The following parameters are set by default<br>
 *  GPT_CH_FUNC_PWM_EDGE - Pulse starts at edge of cycle<br>
 *  GPT_CH_WAVE_POL_POS - Waveform goes from high to low<br>
 *
 *
 *  @param [in] dev Handle to the GPT device returned by gpt_drv_open().
 *  @param[in] ch The Channel to generate PWM waveform on.
 *  @param[in] ton Number of cycles of GPT the output will be On.
 *  @param[in] toff Number of cycles of GPT the output will be Off.
 */
void gpt_drv_pwm(mdev_t *dev, GPT_ChannelNumber_Type ch,
			uint32_t ton, uint32_t toff);

#endif /* !_MDEV_GPT_H_ */
