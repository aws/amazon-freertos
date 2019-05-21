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

/** @file mdev_gpio.h
 *
 *  @brief This file contains General Purpose Input Output (GPIO) Driver
 *
 * The GPIO driver is used to handle I/O requests to read/write GPIO pins.
 * GPIO pins are  used for communication with external entities.
 * They are used for sending control signals like chip select
 * and clock.
 *
 *  @note Interface is kept similar to other mDev drivers, but there is no
 *  need to register this driver with mDev, keeping simplicity and usage
 *  from ISR context in mind.
 *
 * @section mdev_gpio_usage Usage
 *
 *  A typical GPIO device usage scenario is as follows:
 *
 *  -# Initialize the GPIO driver using gpio_drv_init().
 *  -# Open the GPIO device handle  using gpio_drv_open() call.\n
 *  -# Use gpio_drv_setdir() to set direction of GPIO pin (input or output).
 *  -# Use gpio_drv_read() or gpio_drv_write()  to read GPIO state or\n
 *     to write GPIO pin state respectively.
 *  -# GPIO pins can be used to generate interrupts and interrupt\n
 *     callback is registered or unregistered using gpio_drv_set_cb().
 *  -# Close the GPIO device using gpio_drv_close() after its use.\n
 *     gpio_drv_close() informs the GPIO driver to release the
 *     resources that it is holding. It is always good practice to
 *     free the resources after use.
 *
 *  Code snippet:\n
 *  Following code demonstrates how to use GPIO driver APIs
 *  to set a pin as output and set it high.
 *  @code
 *
 *  {
 *     gpio_drv_init();

 *         .
 *         .
 *         .
 *    mdev_t* gpio_dev = gpio_drv_open("MDEV_GPIO");
 *    if (gpio_dev == NULL)
 *       err_handling();
 *    gpio_drv_setdir(gpio_dev, pin, GPIO_IO_OUTPUT);
 *    gpio_drv_write(gpio_dev, pin, GPIO_IO_HIGH);
 *    gpio_drv_close(gpio_dev);
 *    return;
 *  }
 *
 * @endcode
 *  Following code demonstrates how to use GPIO driver APIs
 *  to read a pin state.
 *  @code
 *
 *  {
 *    int err = gpio_drv_init();
 *    if (err != WM_SUCCESS)
 *       err_handling();
 *         .
 *         .
 *         .
 *    mdev_t* gpio_dev = gpio_drv_open("MDEV_GPIO");
 *    if (gpio_dev == NULL)
 *       err_handling();
 *    int state;
 *    int err = gpio_drv_read(gpio_dev, pin, &state);
 *    if (err != WM_SUCCESS)
 *       err_handling();
 *    gpio_drv_close(gpio_dev);
 *    return;
 *  }
 * @endcode
 *  Following code demonstrates how to use GPIO driver APIs
 *  to generate and handle an interrupt.
 * @code
 *  static void gpio_cb()
 *  {
 *      process_interrupt_of_gpio();
 *  }
 *  {
 *    int err = gpio_drv_init();
 *    if (err != WM_SUCCESS)
 *       err_handling();
 *         .
 *         .
 *         .
 *    mdev_t* gpio_dev = gpio_drv_open("MDEV_GPIO");
 *    if (gpio_dev == NULL)
 *       err_handling();
 *
 *    gpio_drv_setdir(gpio_dev, pin, GPIO_IO_INPUT);
 *
 *    err = gpio_drv_set_cb(gpio_dev, pin, GPIO_INT_FALLING_EDGE,
 *                        gpio_cb);
 *    if (err != WM_SUCCESS)
 *       err_handling();
 *
 *    gpio_drv_close(gpio_dev);
 *    return;
 *  }
 * @endcode
 * @note
 *     -# Locking mechanism is implemented to provide atomic access.
 *     -# The APIs assume that caller passed correct GPIO pin number.
 *     -# GPIO_Int_Type enumeration has following possible values
 *        GPIO_INT_RISING_EDGE , GPIO_INT_FALLING_EDGE, GPIO_INT_DISABLE,
 *        GPIO_INT_DISABLE.
 *     -# A GPIO pin state can have only two possible values
 *        GPIO_IO_LOW and GPIO_IO_HIGH.
 *     -# A GPIO pin direction can be either GPIO_INPUT or GPIO_OUTPUT.
 *
 */

#ifndef _MDEV_GPIO_H_
#define _MDEV_GPIO_H_

#include <mdev.h>
#include <wmlog.h>
#include <wmerrno.h>
#include <lowlevel_drivers.h>

/** Function pointer for callback function called on GPIO interrupt */
typedef void (*gpio_irq_cb) (int pin, void *data);

/** Initialize the GPIO driver
 *
 *  This function initializes GPIO peripheral clock domain.
 *  @return WM_SUCCESS on success
 */
int gpio_drv_init(void);

/** Open GPIO device driver
 *
 * This function opens the handle to GPIO device and enables application to use
 * the device. This handle should be used in other calls related to GPIO device.
 *
 *  @param name Name of mdev gpio driver.
 *              It should be "MDEV_GPIO" string.
 *  @return handle to driver on success
 *  @return NULL otherwise
 */
mdev_t *gpio_drv_open(const char *name);

/** Close the GPIO device
 *
 *  @note Kept for unified mDEV layer interface, nothing to do for GPIO
 *  driver
 *  @param [in] dev Handle to the GPIO device returned by gpio_drv_open().
 */
static inline void gpio_drv_close(mdev_t *dev)
{
}

/** Set direction of GPIO pin
 *
 *  This function allows caller to set direction of a GPIO pin.
 *  @param [in] dev Handle to the GPIO device returned by gpio_drv_open().
 *  @param [in] pin Pin Number(GPIO_XX) of GPIO pin (example: GPIO_12)
 *  @param [in] dir Either GPIO_INPUT or GPIO_OUTPUT
 */
static inline void gpio_drv_setdir(mdev_t *dev,
			GPIO_NO_Type pin, GPIO_Dir_Type dir)
{
	GPIO_SetPinDir(pin, dir);
}

/** Set GPIO pin state
 *
 *  This function allows caller to set GPIO pin's state to high or low.
 *  @param [in] dev Handle to the GPIO device returned by gpio_drv_open().
 *  @param [in] pin Pin Number(GPIO_XX) of GPIO pin
 *  @param [in] val Value to be set.
 *         GPIO_IO_LOW(0) or  GPIO_IO_HIGH(1)
 */
static inline void gpio_drv_write(mdev_t *dev,
			GPIO_NO_Type pin, GPIO_IO_Type val)
{
	GPIO_WritePinOutput(pin, val);
}

/** Read GPIO pin status
 *
 *  This function allows caller to get GPIO pin's state.
 *
 *  @param [in] dev Handle to the GPIO device returned by gpio_drv_open().
 *  @param [in]  pin Pin Number(GPIO_XX) of GPIO pin (example: GPIO_12)
 *  @param [out] *val Value read
 *  @return WM_SUCCESS on success
 *  @return Error code otherwise
 */
static inline int gpio_drv_read(mdev_t *dev, GPIO_NO_Type pin, int *val)
{
	if (!val)
		return -WM_E_INVAL;
	*val = GPIO_ReadPinLevel(pin);
	return WM_SUCCESS;
}

/** Register CallBack for GPIO Pin Interrupt
 *
 *  @param [in] dev Handle to the GPIO device returned by gpio_drv_open().
 *  @param [in] pin Pin Number(GPIO_XX) of GPIO pin
 *  @param [in] type Possible values from GPIO_Int_Type
 *  @param [in] data Pointer to a caller's data structure
 *  @param [in] gpio_cb Function to be called when said interrupt occurs.
 *              If NULL the callback will be de-registered.
 *  @note intCallback_Type is defined as typedef void (intCallback_Type)(void);
 *        GPIO pin's direction should be set as input to generate an interrupt.
 *
 *  @return WM_SUCCESS on success
 *  @return -WM_FAIL  if invalid parameters
 */
int gpio_drv_set_cb(mdev_t *dev, int pin, GPIO_Int_Type type,
		    void *data,
		    gpio_irq_cb gpio_cb);

/** Get the voltage IO domain for a given pin
 *
 * The different GPIOs of the MCU are part of different voltage domains.
 * These domains have significance in power management wherein some domains
 * can be turned off and only the required ones can be kept on. This API can
 * be used to get the domain for a particular GPIO so that the application can
 * decide whether it wants to keep that domain On or Off.
 *
 * \param[in] pin the GPIO pin number
 *
 * \return The domain number for the GPIO (value >= 0)
 * \return -1 if the GPIO is not part of any of the domains.
 */
int gpio_drv_get_io_domain(int pin);
#endif /* _MDEV_GPIO_H_ */
