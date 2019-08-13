/** @file mdev_pinmux.c
 *
 *  @brief This file provides  generic Pinmux helper functions
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
 * mdev_pinmux.c: generic Pinmux helper functions
 */

#include <wmtypes.h>
#include <wmstdio.h>
#include <mdev_pinmux.h>
#include <wm_os.h>

/* Please note this depends on mw300_pinmux.h */
int pinmux_drv_get_gpio_func(int pin)
{
	if (pin < GPIO_0 || pin > GPIO_49)
		return -WM_E_INVAL;

	if (((pin >= GPIO_6) && (pin <= GPIO_10))
	    || ((pin >= GPIO_22) && (pin <= GPIO_26))
	    || ((pin >= GPIO_28) && (pin <= GPIO_33))) {
		return PINMUX_FUNCTION_1;
	} else {
		return PINMUX_FUNCTION_0;
	}
}
