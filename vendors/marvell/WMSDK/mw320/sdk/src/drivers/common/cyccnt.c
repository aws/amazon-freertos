/** @file cyccnt.c
 *
 *  @brief This file provides  functions for cycle counter
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


#include <wmtypes.h>
#include <lowlevel_drivers.h>

void arch_start_cyccnt()
{
	/* Enable Cortex-M3 DWT unit */
	CoreDebug->DEMCR |= ((1 << 24));
	/* Enable cpu cycle counter */
	DWT->CTRL |= (0x1);
	/* Reset cpu cycle counter */
	DWT->CYCCNT = 0;
}

uint32_t arch_get_cyccnt()
{
	/* 32 bit counter will overflow based on frequency at which cpu is
	 * running, e.g. approx 21 seconds for 200MHz.
	 */
	return DWT->CYCCNT;
}

void arch_stop_cyccnt()
{
	/* Disable cpu cycle counter */
	DWT->CTRL &= ~(0x1);
	/* Disable Cortex-M3 DWT unit */
	CoreDebug->DEMCR &= (~(1 << 24));
}
