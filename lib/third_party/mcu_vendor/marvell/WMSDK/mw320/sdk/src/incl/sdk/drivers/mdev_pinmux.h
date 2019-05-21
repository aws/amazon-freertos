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

/** @file mdev_pinmux.h
 *
 *  @brief This file contains GPIO Pin Multiplexer Driver
 *
 *  Various functionalities are multiplexed on the same GPIO pin
 *  as the number of Pins are limited.
 *  The functionality to be assigned to any particular pin can be
 *  configured using the PINMUX driver.
 *
 *  @note Interface is kept similar to other mDev drivers, but there is no
 *  need to register this driver with mDev, keeping simplicity and usage
 *  from ISR context in mind.
 *
 * @section mdev_pinmux_usage Usage
 *
 *  A typical PINMUX device usage scenario is as follows:
 *
 *  -# Initialize the PINMUX driver using pinmux_drv_init().
 *  -# Open the PINMUX device handle  using pinmux_drv_open() call.\n
 *  -# Set the alternate function using pinmux_drv_setfunc()
 *  -# Close the PINMUX device using pinmux_drv_close() after its use.\n
 *     pinmux_drv_close() informs the PINMUX driver to release the
 *     resources that it is holding. It is always good practice to
 *     free the resources after use.
 *
 *  Code snippet:\n
 *  Following code demonstrates how to use PINMUX driver APIs
 *  to set a pin's alternate function.
 *  @code
 *
 *  {
 *    pinmux_drv_init();
 *         .
 *         .
 *    mdev_t* pinmux_dev = pinmux_drv_open("MDEV_PINMUX");
 *    if (pinmux_dev == NULL)
 *        err_handling();
 *
 *    pinmux_drv_setfunc(pinmux_dev, pin, PINMUX_FUNCTION_0);
 *    pinmux_drv_close(pinmux_dev);
 *    return;
 *  }
 *
 * @endcode
 *
 */

#ifndef _MDEV_PINMUX_H_
#define _MDEV_PINMUX_H_

#include <mdev.h>
#include <wmlog.h>
#include <lowlevel_drivers.h>

/** Initialize the PINMUX driver
 *
 * This function initializes PINMUX driver
 */
static inline void pinmux_drv_init()
{
}

/** Open PINMUX device driver
 *
 * This function opens the handle to PINMUX device and enables
 * application to use the device. This handle should be used
 * in other calls related to PINMUX device.
 *
 *  @param [in] name Name of mdev pinmux driver.
 *              It should be "MDEV_PINMUX" string.
 *  @return handle to driver on success
 */
static inline mdev_t *pinmux_drv_open(const char *name)
{
	return (mdev_t *) 1;
}

/** Close the PINMUX device
 *
 *  This function closes the handle to PINMUX device.
 *  @param [in] dev Handle to the PINMUX device returned by pinmux_drv_open().
 */
static inline void pinmux_drv_close(mdev_t *dev)
{
}

/** Set alternate function of a GPIO pin
 *
 *  Sets the alternate function for the specified pin.
 *
 *  @param [in] dev Handle to the PINMUX device returned by pinmux_drv_open().
 *  @param [in] pin Pin to be modified
 *  @param [in] func Alternate function number(GPIO_PinMuxFunc_Type)
 */
static inline void pinmux_drv_setfunc(mdev_t *dev,
			GPIO_NO_Type pin, GPIO_PinMuxFunc_Type func)
{
	GPIO_PinMuxFun(pin, func);
}

/** Get the GPIO pinmux function for a given pin
 *
 * The GPIO pins are multiplexed such that they can be used for various
 * alternate functions. The required function has to be selected by setting
 * the mux appropriately using pinmux_drv_setfunc(). If the pin has to be used
 * as a GPIO, use this function to get the corresponding pinmux function to be
 * passed to pinmux_drv_setfunc()
 *
 * @param[in] pin Pin number
 *
 * @return The GPIO pinmux function for the given pin if successful.
 * @return -WM_E_INVAL on error.
 */
int pinmux_drv_get_gpio_func(int pin);
#endif /* _MDEV_PINMUX_H_ */
