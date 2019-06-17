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

/** @file generic_io.h
*
*  @brief This file contains Input/Output GPIO related data structures
*/

#ifndef __GENERIC_IO_H__
#define __GENERIC_IO_H__

#include <mdev_gpio.h>

/** GPIO needs to be pulled low to activate */
#define GPIO_ACTIVE_LOW		0
/** GPIO needs to be pulled high to activate */
#define GPIO_ACTIVE_HIGH	1

/** Input GPIO Configuration type.
 * Normally used for push buttons. */
typedef struct {
	/** The GPIO number associated with this input. */
	int8_t gpio;
	/** The type of the input. Can be either \ref GPIO_ACTIVE_LOW
	 * or \ref GPIO_ACTIVE_HIGH. This indicates what the state of
	 * the GPIO will be when the button is pressed.
	 */
	bool type;
} input_gpio_cfg_t;

/** Output GPIO Configuration type.
 * Normally used for LED indicators or for driving loads. */
typedef struct {
	/** The GPIO number associated with this output. */
	int8_t gpio;
	/** The type of the output. Can be either \ref GPIO_ACTIVE_LOW
	 * or \ref GPIO_ACTIVE_HIGH.
	 */
	bool type;
} output_gpio_cfg_t;

#endif /* __HAP_IO_H__ */
