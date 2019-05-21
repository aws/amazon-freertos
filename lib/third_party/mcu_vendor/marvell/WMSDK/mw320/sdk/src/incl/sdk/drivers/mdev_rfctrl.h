/** @file mdev_rfctrl.h
*
*  @brief RF Control Configuration
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

/*! \file mdev_rfctrl.h
 * \brief RF Control Configuration
*/

/**
 *  This function configures the GPIO pins
 *  used for RF Control.
 *  @param [in]  antenna for which RF Control
 * 		GPIO pins need to be configured
 *  @note  Even after doing this configuration appropriate command needs
 *              to be send to Wi-Fi firmware to set antenna.
 */
#ifdef CONFIG_CPU_MC200
static inline void rfctrl_set_config(int antenna) {}
#else

void  rfctrl_set_config(int antenna);

#endif

