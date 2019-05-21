/** @file cyccnt.h
*
*  @brief CPU cycles counter module
*
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


/*! \file cyccnt.h
 * \brief CPU cycles counter module
 */


#ifndef _ARCH_H_
#define _ARCH_H_

/** Start CPU cycles counter
 *
 * This function enables DWT unit in Cortex-M3 core and starts CPU cycles
 * counter, resetting its value to 0.
 */
void arch_start_cyccnt();

/** Get CPU cycles counter value
 * This function returns current cpu cycle counter (32 bit) value.
 *
 * \return uint32_t value
 * \note CPU cycle counter in Cortex-M3 core is 32 bit and hence it will
 * overflow based on configured frequency at which cpu is running, e.g. approx
 * 21 seconds for 200MHz frequency. This should be taken into account.
 */
uint32_t arch_get_cyccnt();

/** Stop CPU cycles counter
 *
 * This function disables DWT unit in Cortex-M3 core and stops CPU cycles
 * counter.
 */
void arch_stop_cyccnt();

#endif /* _ARCH_H_ */
