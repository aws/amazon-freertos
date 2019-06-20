/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HAL_ADC_H__
#define __HAL_ADC_H__
#include "hal_platform.h"

#ifdef HAL_ADC_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup ADC
 * @{
 * This section describes the programming interfaces of the ADC HAL driver.
 *
 * @section HAL_ADC_Terms_Chapter Terms and acronyms
 *
 * The following provides descriptions to the terms commonly used in the Analog-to-Digital Converter (ADC) driver and how to use its various functions.
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b ADC                        | ADC is an Analog-to-Digital Converter that converts a continuous physical quantity (usually voltage) to a digital number that represents the quantity's amplitude.|
 *
 * @section HAL_ADC_Features_Chapter Supported features
 * The ADC is a <a href="https://en.wikipedia.org/wiki/Successive_approximation_ADC">Successive Approximation ADC </a>. The supported channels vary based on the actual MCU devices used.
 * Resolution of MT2523 and MT7687 is 12 bits.
 * - \b Retrieve \b ADC \b data \b for \b a \b channel. \n
 *   Call hal_adc_get_data_polling() function to get data for a channel. Only one sample data is retrieved at a time.
 *
 * @}
 * @}
 */

#ifdef HAL_ADC_FEATURE_NO_NEED_PINMUX
/**
 * @addtogroup HAL
 * @{
 * @addtogroup ADC
 * @{
 * @section HAL_ADC_Driver_Usage_Chapter How to use this driver
 * - \b Use \b ADC \b driver \b to \b gets \b sample \b data \b for \b a \b channel.\n
 *   - Step1: Call hal_adc_init() to initialize the ADC module. No need to call hal_pinmux_set_function() when use pin as ADC mode.
 *   - Step2: Call hal_adc_get_data_polling() to retrieve sample data for a channel.
 *   - Step3: Call hal_adc_deinit() to return the ADC module back to its original state.
 *   - Sample code:
 *   @code
 *
 * 	 uint32_t adc_data;
 *
 * 	 hal_adc_init();//Initialize ADC module.
 * 	 hal_adc_get_data_polling(HAL_ADC_CHANNEL_0, &adc_data);//Retrieve ADC data for channel 0.
 * 	 hal_adc_deinit();//Deinit ADC module when job done.
 *
 *   @endcode
 *
 */
#elif defined HAL_ADC_FEATURE_SET_INPUT_AND_DISABLE_PULL

/**
 * @addtogroup HAL
 * @{
 * @addtogroup ADC
 * @{
 * @section HAL_ADC_Driver_Usage_Chapter How to use this driver
 * - \b Use \b ADC \b driver \b to \b gets \b sample \b data \b for \b a \b channel.\n
 *   - Step1: Call hal_gpio_init() to initialize the pin.
 *   - Step2: Call hal_pinmux_set_function() to set ADC pinmux if the EPT is not used.
 *   - Step3: Call hal_gpio_set_direction() to set the related pin's direction as input if the EPT is not used.
 *   - Step4: Call hal_gpio_disable_pull() to disable pull state of the related pin if the EPT is not used.
 *   - Step5: Call hal_adc_init() to initialize the ADC module.
 *   - Step6: Call hal_adc_get_data_polling() to retrieve sample data for a channel.
 *   - Step7: Call hal_adc_deinit() to return the ADC module back to its original state.
 *   - Sample code:
 *   @code
 *
 * 	 uint32_t adc_data;
 *       hal_gpio_init(HAL_GPIO_0);
 * 	 hal_pinmux_set_function(HAL_GPIO_0, 2);//Set GPIO 0 to an ADC work mode. For more details, please refer to hal_pinmux_set_function().
 * 	 hal_gpio_set_direction(HAL_GPIO_0, HAL_GPIO_DIRECTION_INPUT);//Set GPIO 0 direction as input.
 * 	 hal_gpio_disable_pull(HAL_GPIO_0);//Disable pull of GPIO 0.
 * 	 hal_adc_init();//Initialize ADC module.
 * 	 hal_adc_get_data_polling(HAL_ADC_CHANNEL_11, &adc_data);//Retrieve ADC data for channel 11.
 * 	 hal_adc_deinit();//Deinit ADC module when job done.
 *
 *   @endcode
 *
 */
#else
/**
 * @addtogroup HAL
 * @{
 * @addtogroup ADC
 * @{
 * @section HAL_ADC_Driver_Usage_Chapter How to use this driver
 * - \b Use \b ADC \b driver \b to \b gets \b sample \b data \b for \b a \b channel.\n
 *   - Step1: Call hal_gpio_init() to initialize the pin.
 *   - Step2: Call hal_pinmux_set_function() to set ADC pinmux if the EPT is not used.
 *   - Step3: Call hal_adc_init() to initialize the ADC module.
 *   - Step4: Call hal_adc_get_data_polling() to retrieve sample data for a channel.
 *   - Step5: Call hal_adc_deinit() to return the ADC module back to its original state.
 *   - Sample code:
 *   @code
 *
 * 	 uint32_t adc_data;
 *
 *       hal_gpio_init(HAL_GPIO_0);
 * 	 hal_pinmux_set_function(HAL_GPIO_0, 2);//Set GPIO 0 to an ADC work mode. For more details, please refer to hal_pinmux_set_function().
 * 	 hal_adc_init();//Initialize ADC module.
 * 	 hal_adc_get_data_polling(HAL_ADC_CHANNEL_11, &adc_data);//Retrieve ADC data for channel 11.
 * 	 hal_adc_deinit();//Deinit ADC module when job done.
 *
 *   @endcode
 *
 */
#endif

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * Enums
 *****************************************************************************/

/** @defgroup hal_adc_enum Enum
 *  @{
 */

/** @brief This enum defines the ADC API return status*/
typedef enum {
    HAL_ADC_STATUS_INVALID_PARAMETER = -4,      /**< Invalid parameter */
    HAL_ADC_STATUS_ERROR_BUSY = -3,             /**< ADC is busy */
    HAL_ADC_STATUS_ERROR_CHANNEL = -2,          /**< ADC channel number error */
    HAL_ADC_STATUS_ERROR = -1,                  /**< ADC error */
    HAL_ADC_STATUS_OK = 0                       /**< ADC ok */
} hal_adc_status_t;

/**
 * @}
 */


/*****************************************************************************
 * Structures
 *****************************************************************************/



/*****************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief    ADC init function.
 * @return
 * #HAL_ADC_STATUS_OK, ADC init success. \n
 * #HAL_ADC_STATUS_ERROR_BUSY, ADC is busy. \n
 * #HAL_ADC_STATUS_ERROR, ADC clock enable failed.
 */
hal_adc_status_t hal_adc_init(void);


/**
* @brief 	ADC deinit function. This function resets the ADC peripheral registers to their default values.
* @return
* #HAL_ADC_STATUS_OK, ADC deinit success. \n
* #HAL_ADC_STATUS_ERROR, ADC clock disable failed.
*/
hal_adc_status_t hal_adc_deinit(void);


/**
 * @brief 	ADC receives a sample data for a channel.
 * @param[in] channel is the channel that will be sampled. This parameter can only be the value of type #hal_adc_channel_t.
 * @param[out] data is the raw data of the channel.
 * @return
 * #HAL_ADC_STATUS_OK, ADC get data success. \n
 * #HAL_ADC_STATUS_ERROR_CHANNEL, the channel number is invalid. \n
 * #HAL_ADC_STATUS_INVALID_PARAMETER, data is NULL.
 * @par Example
 * Sample code please refer to Use ADC driver to get sample data for a channel of @ref HAL_ADC_Driver_Usage_Chapter
 */
hal_adc_status_t hal_adc_get_data_polling(hal_adc_channel_t channel, uint32_t *data);


#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
*/
#endif /*HAL_ADC_MODULE_ENABLED*/
#endif /* __HAL_ADC_H__ */




