/** @file mdev_gpio.c
 *
 *  @brief This file provides  mdev driver for GPIO
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


/*
 * mdev_gpio.c: mdev driver for GPIO
 */
#include <wmstdio.h>
#include <mdev_gpio.h>
#include <wm_os.h>
#include <lowlevel_drivers.h>

static bool gpio_inited;

#define PortTotal         ((GPIO_MaxNo >> 5) + 1)
#define PortNum(gpioNo)   (gpioNo >> 5)
#define GpioNum(gpioNo)   (gpioNo & 0x1F)

/**
   This structure holds callback function to be called
   when interrupt occurs on the pin
*/
typedef struct {
	void *data;
	gpio_irq_cb gpio_cb;
} gpio_irq_obj_t;

static gpio_irq_obj_t gpio_irq_obj_list[GPIO_MaxNo + 1];

int gpio_drv_get_io_domain(int pin)
{
#ifdef CONFIG_CPU_MW300
	if (pin >= GPIO_0 && pin <= GPIO_15)
		return 0;
	else if (pin >= GPIO_16 && pin <= GPIO_21)
		return 1;
	else if (pin >= GPIO_28 && pin <= GPIO_33)
		return 2;
	else if (pin == GPIO_27 || (pin >= GPIO_34 && pin <= GPIO_49))
		return 3;
	else
		return -1;
#else
	if (pin >= GPIO_0 && pin <= GPIO_17)
		return 0;
	else if (pin >= GPIO_28 && pin <= GPIO_50)
		return 1;
	else if (pin >= GPIO_59 && pin <= GPIO_79)
		return 2;
	else
		return -1;
#endif
}

void GPIO_IRQHandler(void)
{
	uint32_t i, intStatus[PortTotal];

	for (i = 0; i < PortTotal; i++) {
		/* Get current interrupt status */
		intStatus[i] = GPIO->GEDR[i].WORDVAL;
		/* Clear the generated interrupts */
		GPIO->GEDR[i].WORDVAL = intStatus[i];
	}
	/* Check which GPIO pin has interrupt */
	for (i = GPIO_MinNo; i <= GPIO_MaxNo; i++) {
		if (intStatus[PortNum(i)] & (0x01 << GpioNum(i))) {
			/* Call interrupt callback function */
			if (gpio_irq_obj_list[i].gpio_cb) {
				void *data = gpio_irq_obj_list[i].data;
				gpio_irq_obj_list[i].gpio_cb(i,
							     data);
			} else{
				/* Disable interrupt if interrupt
				   callback is not install
				*/
				GPIO->GCRER[PortNum(i)].WORDVAL =
					(0x01 << GpioNum(i));
				GPIO->GCFER[PortNum(i)].WORDVAL =
					(0x01 << GpioNum(i));
			}
		}
	}
}

static void gpio_register_cb(int pin,
			    gpio_irq_cb gpio_cb,
			    void *data)
{
	gpio_irq_obj_list[pin].gpio_cb = gpio_cb;
	gpio_irq_obj_list[pin].data = data;
}

int gpio_drv_set_cb(mdev_t *dev, int pin, GPIO_Int_Type type,
		    void *data,
		    gpio_irq_cb gpio_cb)
{
	if (pin < 0)
		return -WM_FAIL;

	if (pin > GPIO_MaxNo)
		return -WM_FAIL;

	if (type != GPIO_INT_DISABLE && type != GPIO_INT_BOTH_EDGES
	    && type != GPIO_INT_FALLING_EDGE && type != GPIO_INT_RISING_EDGE)
		return -WM_FAIL;

	os_disable_all_interrupts();

	/* Check if any interrupt is pending for this pin
	 * if so clear it before installing callback
	 */
	if (GPIO_GetIntStatus(pin))
		GPIO_IntClr(pin);

	/* install_int_callback(INT_GPIO, pin, gpio_cb); */
	if (gpio_cb)
		gpio_register_cb(pin, gpio_cb, data);

	GPIO_IntConfig(pin, type);
	GPIO_IntMask(pin, (gpio_cb == NULL) ? MASK : UNMASK);
	NVIC_SetPriority(GPIO_IRQn, 0xf);
	NVIC_EnableIRQ(GPIO_IRQn);

	os_enable_all_interrupts();

	return WM_SUCCESS;
}

mdev_t *gpio_drv_open(const char *name)
{
	if (!gpio_inited) {
		ll_log("[gpio] driver open called without registering device"
			 " (%s)\n\r", name);
		return NULL;
	}

	/* Return dummy pointer, this will not be used anywhere */
	return (mdev_t *) 1;
}

int gpio_drv_init(void)
{
	/*
	 * Note: We will avoid mDEV interface altogether to keep things
	 * simple with GPIO driver
	 */

	if (!gpio_inited) {
		/* Enable GPIO Clock */
		CLK_ModuleClkEnable(CLK_GPIO);

		/* GPIO driver is initialized */
		gpio_inited = true;
	}

	return WM_SUCCESS;
}
