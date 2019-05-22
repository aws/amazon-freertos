/** @file mdev_rfctrl.c
 *
 *  @brief This file provides  mdev driver for rf control
 *
 *  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved.
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


#include <lowlevel_drivers.h>

void rfctrl_set_config(int ant)
{
	switch (ant) {
	case 2:
		/* Write 0 1 on RF_CTRL pins 44 and 45*/
		GPIO_PinMuxFun(GPIO_44, PINMUX_FUNCTION_0);
		GPIO_PinMuxFun(GPIO_45, PINMUX_FUNCTION_0);
		GPIO_SetPinDir(GPIO_44, GPIO_OUTPUT);
		GPIO_SetPinDir(GPIO_45, GPIO_OUTPUT);
		GPIO_WritePinOutput(GPIO_44, GPIO_IO_LOW);
		GPIO_WritePinOutput(GPIO_45, GPIO_IO_HIGH);
		break;
	case 65535:
		/* Use PinMux 7 on RF_CTRL pins
		 *  for antenna diversity */

		GPIO_PinMuxFun(GPIO_44,	PINMUX_FUNCTION_7);
		GPIO_PinMuxFun(GPIO_45,	PINMUX_FUNCTION_7);
		break;
	case 1:
	default:
		/* Write 1 0 on RF_CTRL pins 44 and 45*/
		GPIO_PinMuxFun(GPIO_44, PINMUX_FUNCTION_0);
		GPIO_PinMuxFun(GPIO_45,	PINMUX_FUNCTION_0);
		GPIO_SetPinDir(GPIO_44, GPIO_OUTPUT);
		GPIO_SetPinDir(GPIO_45, GPIO_OUTPUT);
		GPIO_WritePinOutput(GPIO_44, GPIO_IO_HIGH);
		GPIO_WritePinOutput(GPIO_45, GPIO_IO_LOW);
		break;
	}
}
