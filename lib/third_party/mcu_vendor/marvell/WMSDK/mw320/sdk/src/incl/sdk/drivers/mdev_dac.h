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

/** @file mdev_dac.h
 *
 *  @brief This file contains Digital Analog Converter Driver
 *
 * DAC driver provides mdev interface to use Digital to Analog Converter
 * unit/s.
 * Follow the steps below to use DAC driver from applications
 *
 * 1. Include Files
 *
 * Include mdev.h and mdev_dac.h in your source file.
 *
 * 2. DAC driver initialization
 *
 * Call \ref dac_drv_init from your application initialization function.
 * This will initialize and register dac driver with the mdev interface.
 *
 * 3. Open DAC device to use
 *
 * \ref dac_drv_open should be called once before using DAC hardware.
 * Here you will specify DAC channel (A or B). The other parameters
 * that are set by default are -<br>
 * DAC_WAVE_NORMAL - No predefined waveform<br>
 * DAC_RANGE_LARGE - Output Range Large<br>
 * DAC_OUTPUT_PAD - Output is enable to pad<br>
 * DAC_NON_TIMING_CORRELATED - Non timing correlated mode<br>
 *
 * If these defaults need to be changed they should be changed in
 * \ref dac_drv_open
 *
 * \code
 * {
 *	mdev_t *dac_dev;
 *	dac_dev = dac_drv_open(MDEV_DAC, DAC_CH_A);
 * }
 * \endcode
 *
 * 4. Set output of DAC
 *
 * Call \ref dac_drv_output to get the output of the mentioned
 * dac. Max resolution is 10 bit.
 *
 * 5. Closing a device.
 *
 * A call to \ref dac_drv_close informs the DAC driver to release the
 * resources that it is holding.
 *
 * 6. De-register the DAC driver
 *
 * A call to \ref dac_drv_deinit informs DAC driver to de-register
 * the DAC driver.
 */

#ifndef _MDEV_DAC_H_
#define _MDEV_DAC_H_

#include <mdev.h>
#include <lowlevel_drivers.h>

#define DAC_LOG(...)  wmlog("dac", __VA_ARGS__)

#define MDEV_DAC "MDEV_DAC"
typedef enum {
	waveMode,
	outMode,
	timingMode
} dac_cfg_param;
/** Register DAC Device
 *
 * This function registers dac driver with mdev interface.
 *
 * \return WM_SUCCESS on success, error code otherwise.
 */
int dac_drv_init();

/** Modify the default DAC configuration
 *
 * This function allows user to over-ride the default DAC configuration.
 * A call to thif function is not mandatory and should be used only if the
 * user is aware of its impact.
 *
 * This function should be used after dac_drv_init() but before dac_drv_open().
 *
 * Only one parameter can be changed in a single call.
 * Use this function multiple times if multiple parameters need to be changed.
 * All un-modified parameters are set to their default values.
 * The default values are as below:
 * Config-Type	|	Default value
 * waveMode	=	DAC_WAVE_NORMAL
 * outMode	=	DAC_OUTPUT_PAD
 * timingMode	=	DAC_NON_TIMING_CORRELATED
 *
 * \param[in] config_type The config option to be changed. Can be waveMode,
 * outMode or timingMode.
 * \param[in] value The desired value for the parameter.
 */
void dac_modify_default_config(dac_cfg_param config_type, int value);

/** Open DAC Device
 *
 * This function opens the device for use and returns a handle. This handle
 * should be used for other calls to the DAC driver.
 *
 * \param[in] name Name of the driver(MDEV_DAC) to be opened.
 * \param[in] channel Channel to be used.
 * \return NULL if error, mdev_t handle otherwise.
 */
mdev_t *dac_drv_open(const char *name, DAC_ChannelID_Type channel);

/** Set DAC Output
 *
 * This function sets the output of dac device.
 *
 * \param[in] mdev_p mdev_t handle to the driver.
 * \param[in] val Max 10 bit value to be set.
 */
void dac_drv_output(mdev_t *mdev_p, int val);

/** Close DAC Device
 *
 * This function closes the device after use and frees up resources.
 *
 * \param[in] dev mdev handle to the driver to be closed.
 * \return WM_SUCCESS on success, -WM_FAIL on error.
 */
int dac_drv_close(mdev_t *dev);

/** De-register DAC device
 * This function de-registers the DAC driver.
 */
void dac_drv_deinit();
#endif /* !_MDEV_DAC_H_ */
