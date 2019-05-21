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

/** @file mdev_adc.h
 *
 *  @brief This file contains Analog to Digital Converter Driver
 *
 * ADC driver provides mdev interface to use the internal Analog to
 * Digital Converter unit/s.
 * Follow the steps below to use ADC driver from applications
 *
 * 1. Include Files
 *
 * Include mdev.h and mdev_adc.h in your source file.
 *
 * 2. ADC driver initialization
 *
 * Call \ref adc_drv_init from your application initialization function.
 * This will initialize and register adc driver with the mdev interface.
 *
 * 3. Open ADC device to use
 *
 * \ref adc_drv_open() should be called once before using ADC hardware.
 * Here you will specify ADC number (from 0 to 1). The other parameters
 * that are set by default are -<br>
 * ADC_RESOLUTION_16BIT - Resolution 16 bit<br>
 * ADC_VREF_INTERNAL - Internal 1.2V reference<br>
 * ADC_GAIN_1 - PGA gain set to 1<br>
 * ADC_CLOCK_DIVIDER_32 - ADC clock divided by 32<br>
 * ADC_BIAS_FULL - Full biasing mode<br>
 *
 * If these defaults need to be changed they should be changed in
 * \ref adc_drv_open()
 *
 * \code
 * {
 *	mdev_t *adc_dev;
 *	adc_dev = adc_drv_open(ADC0_ID);
 * }
 * \endcode
 *
 * 4. Get result/output of ADC
 *
 * Call \ref adc_drv_result() to get the output of the mentioned
 * adc.
 *
 * 5. Closing a device.
 *
 * A call to \ref adc_drv_close informs the ADC driver to release the
 * resources that it is holding.
 */

#ifndef _MDEV_ADC_H_
#define _MDEV_ADC_H_

#include <mdev.h>
#include <lowlevel_drivers.h>

#define ADC_LOG(...)  wmlog("adc", __VA_ARGS__)

typedef enum {
	adcResolution,
	adcVrefSource,
	adcGainSel,
	adcClockDivider,
	adcBiasMode
} adc_cfg_param;

#ifdef CONFIG_CPU_MW300
typedef enum {
	vref_internal,
	vref_external
} adc_calib_ref_type;
#endif
/** Register ADC Device
 *
 * This function registers adc driver with mdev interface.
 *
 * \param[in] adc_id The ADC to be initialized.
 * \return WM_SUCCESS on succes, error code otherwise.
 */
int adc_drv_init(ADC_ID_Type adc_id);

/** Get adc driver configuration
  *
  * This function returns a structure with ADC parameters as its
  * members. If the default parameters are changed by the user, this
  * function returns the structure with current parameters.
  *\param[in] config Pointer to ADC config structure
  */
void adc_get_config(ADC_CFG_Type *config);


#if defined(CONFIG_CPU_MW300)
/** Get adc driver configuration
  *
  * This function configures adc to measure internal temperature sensor.
  * \param[in] adc_id The ADC to be initialized.
  * \param[in] dev mdev handle to the driver to be closed.
  * \param[in] Temperature sensor mode.
  * \param[in] ADC input mode.
  */
void adc_drv_tsensor_config(ADC_ID_Type adcID, mdev_t *mdev_p,
		ADC_TSensorMode_Type adcTSensorMode,
		ADC_InputMode_Type adcInputMode);
#endif
/** Modify default ADC configuration
  *
  * This function allows the user to over-ride the default parameter/s
  * Call to this function is not mandatory and should be made if the user wants
  * to change any of the defaults for ADC.
  * It can change a single parameter in each call.
  * User may choose to modify one or more default parameters.
  * All the un-modified parameter are set to the default value.
  * Default values are as below:
  * Config-Type   |  Default value
  *	adcResolution = ADC_RESOLUTION_16BIT,
  *	adcVrefSource = ADC_VREF_INTERNAL,
  * adcGainSel = ADC_GAIN_1,
  * adcClockDivider =	ADC_CLOCK_DIVIDER_32,
  * adcBiasMode =	ADC_BIAS_FULL
  *
  *\param[in] config_type ADC Config-Type
  *\param[in] value Value for the chosen Config-Type
  */
void adc_modify_default_config(adc_cfg_param config_type, int value);

/** Open ADC Device
 *
 * This function opens the device for use and returns a handle. This handle
 * should be used for other calls to the ADC driver.
 *
 * \param[in] adc_id adc port of the driver to be opened.
 * \param[in] channel Channel to be used.
 * \return NULL if error, mdev_t handle otherwise.
 */
mdev_t *adc_drv_open(ADC_ID_Type adc_id,
#ifdef CONFIG_CPU_MC200
		 ADC_Channel_Type channel);
#else
		ADC_ChannelSource_Type channel);
#endif

/** Get ADC Output
 *
 * This function gets the output of adc device.
 *
 * \param[in] mdev_p  mdev_t handle to the driver.
 * \return The output value (max 16 bit)
 */
int adc_drv_result(mdev_t *mdev_p);

#ifdef CONFIG_CPU_MC200
/** Calibrate ADC
  *
  * This function removes linear gain and offset errors and starts ADC
  * sampling.
  *
  * \param[in] dev mdev handle to the driver
  * \param[in] sysOffsetCalVal offset value for zero correction
  * \return WM_SUCCESS on success , -WM_FAIL on error in calibration
  */
int adc_drv_calib(mdev_t *dev, int16_t sysOffsetCalVal);

#elif CONFIG_CPU_MW300
/** ADC Self-calibration
  *
  * This function (self) calibrates the ADC with respect to the given Vref
  * User need not specify the zero correction value.
  * \param[in] dev mdev handle to the driver
  * \param[in] vref Reference voltage source for ADC
  * \return WM_SUCCESS on success , -WM_FAIL on error in calibration
  */
int adc_drv_selfcalib(mdev_t *dev, adc_calib_ref_type vref);
#endif
/** Set timeout for ADC driver
  * This function allows user to set timeout for ADC conversion.
  * This time is time required for ADC conversion for a single
  * chunk (i.e. maximum 1023 datapoints) and dma transfer time.
  * If user does not set the timeout, default value is used. By default
  * this function waits till the DMA_TRANS_COMPLETE interrupt occurs.
  * Note: To set timeout this function needs to be called before adc_drv_init()
  * call.
  *
  * @param[in] id ADC ID.
  * @param[in] tout The timeout value.
  **/
void adc_drv_timeout(ADC_ID_Type id, uint32_t tout);

/** Get ADC samples for user supplied analog input
  *
  * This function provides n samples for user supplied analog input
  * (n specified by the user).It internally uses DMA to transfer data
  * to the user buffer.The DMA transfer takes place in chunks of data-points.
  * There is no limit on number of samples that can be transferred.
  * Each ADC sample by default uses 16 bit resolution.
  *
  * \param[in] mdev_p mdev handle to the driver
  * \param[in,out] buf pointer to user allocated buffer to store sample points
  * \param[in] num number of samples required
  *
  * @return -WM_FAIL if operation failed.
  * @return WM_SUCCESS if operation was a success.
  */

int adc_drv_get_samples(mdev_t *mdev_p, uint16_t *buf, int num);

/** Close ADC Device
 *
 * This function closes the device after use and frees up resources.
 *
 * \param[in] dev mdev handle to the driver to be closed.
 * \return WM_SUCCESS on success, -WM_FAIL on error.
 */
int adc_drv_close(mdev_t *dev);

/** De-initialize ADC Driver
 *
 * This function de-initializes ADC driver. Note that if an ADC device has
 * been opened using adc_drv_open(), it should be first closed using
 * adc_drv_close() before calling this function.
 *
 * \param[in] adc_id Port ID of ADC.
 */
void adc_drv_deinit(ADC_ID_Type adc_id);

#endif /* !_MDEV_ADC_H_ */
