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

#ifndef __HAL_ISINK_H__
#define __HAL_ISINK_H__

#include "hal_platform.h"
#ifdef HAL_ISINK_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup ISINK
 * @{
 * This section introduces the ISINK APIs including terms and acronyms,
 * supported features, software architecture, how to use this module, ISINK function groups, enums, structures and functions.
 * Users can use this API to adjust differnt current output settings through ISINK registers. For example, to adjust the backlight brightness.
 *
 * @section ISINK_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b ISINK                |Current Sink that provides different current outputs depending on the PMIC setting. |
 * |\b PMIC                 |Power Management Integrated Chip. For an introduction to the Power Management Integrated Chip, please refer to <a href="https://en.wikipedia.org/wiki/Power_management_integrated_circuit">Power management integrated circuit Wikipedia</a> |

 * @section ISINK_Driver_Usage_Chapter How to use this module
 *
 * - Trigger ISINK different current outputs. \n
 *  - Step1: Call #hal_isink_init() to initialize the ISINK module.
 *  - Step2: Call #hal_isink_set_clock_source() to set the clock source of the ISINK.
 *  - Step3: Call #hal_isink_set_mode() to set the ISINK mode.
 *  - Step4: Call #hal_isink_set_step_current() to set the ISINK to 4mA output current.
 *  - Step5: Call #hal_isink_get_running_status() to get the current running status of the ISINK.
 *  - Step6: Call #hal_isink_deinit() to de-initialize ISINK module.
 *  - Sample code:
 *    @code
 *       if(HAL_ISINK_STATUS_OK != hal_isink_init(HAL_ISINK_CHANNEL_0)) {  //Initialize the ISINK clock source.
 *             //Error handler
 *       }
 *       if(HAL_ISINK_STATUS_OK != hal_isink_set_clock_source(HAL_ISINK_CHANNEL_0,HAL_ISINK_CLOCK_SOURCE_2MHZ)) { //Set the ISINK clock source.
 *             //Error handler
 *       }
 *       if(HAL_ISINK_STATUS_OK != hal_isink_set_mode(HAL_ISINK_CHANNEL_0,HAL_ISINK_MODE_REGISTER)) { //Set the ISINK register mode.
 *             //Error handler
 *       }
 *       if(HAL_ISINK_STATUS_OK != hal_isink_set_step_current(HAL_ISINK_CHANNEL_0,HAL_ISINK_OUTPUT_CURRENT_4_MA)) { //Set the ISINK to 4mA output current.
 *             //Error handler
 *       }
 *       if(HAL_ISINK_STATUS_OK != hal_isink_get_running_status(HAL_ISINK_CHANNEL_0,&status)) {  //Get the running status of the ISINK.
 *             //Error handler
 *       }
 *       if(HAL_ISINK_STATUS_OK != hal_isink_deinit(HAL_ISINK_CHANNEL_0)) {  //Deinitialize the ISINK module.
 *             //Error handler
 *       }
 *    @endcode
 */


#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_isink_enum Enum
  * @{
  */

/** @brief ISINK channel. */
typedef enum {
    HAL_ISINK_CHANNEL_0 = 0,                          /**< The ISINK channel 0. */
    HAL_ISINK_CHANNEL_1 = 1,                          /**< The ISINK channel 1. */
    HAL_ISINK_MAX_CHANNEL                             /**< The ISINK maximum channel (invalid).*/
} hal_isink_channel_t;


/** @brief ISINK clock source seletion. */
typedef enum {
    HAL_ISINK_CLOCK_SOURCE_32KHZ = 0,                 /**< The ISINK clock source 32kHz. */
    HAL_ISINK_CLOCK_SOURCE_2MHZ = 1                   /**< The ISINK clock source  2MHz. */
} hal_isink_clock_source_t ;

/** @brief ISINK operating mode seletion. */
typedef enum {
    HAL_ISINK_MODE_PWM = 0,                              /**<The PWM mode.  */
    HAL_ISINK_MODE_BREATH = 1,                           /**<The Breath mode.  */
    HAL_ISINK_MODE_REGISTER = 2                         /**<The Register mode.  */
} hal_isink_mode_t ;


/** @brief ISINK output current seletion. */
typedef enum {
    HAL_ISINK_OUTPUT_CURRENT_4_MA = 0,                       /**<The ISINK output current 4 mA.  */
    HAL_ISINK_OUTPUT_CURRENT_8_MA = 1,                       /**<The ISINK output current 8 mA. */
    HAL_ISINK_OUTPUT_CURRENT_12_MA = 2,                      /**<The ISINK output current 12 mA.  */
    HAL_ISINK_OUTPUT_CURRENT_16_MA = 3,                      /**<The ISINK output current 16 mA. */
    HAL_ISINK_OUTPUT_CURRENT_20_MA = 4,                      /**<The ISINK output current 20mA. */
    HAL_ISINK_OUTPUT_CURRENT_24_MA = 5                       /**<The ISINK output current 24mA. */
} hal_isink_current_t ;

/** @brief ISINK breath mode based ont the time adjustment and brightness levels. */
typedef enum {
    HAL_ISINK_TIME_0_123_SECOND = 0,                       /**<The ISINK breath adjustment time, 0.123s.  */
    HAL_ISINK_TIME_0_338_SECOND = 1,                       /**<The ISINK breath adjustment time, 0.338s.  */
    HAL_ISINK_TIME_0_523_SECOND = 2,                       /**<The ISINK breath adjustment time, 0.523s. */
    HAL_ISINK_TIME_0_707_SECOND = 3,                       /**<The ISINK breath adjustment time, 0.707s. */
    HAL_ISINK_TIME_0_926_SECOND = 4,                       /**<The ISINK breath adjustment time, 0.926s. */
    HAL_ISINK_TIME_1_107_SECOND = 5,                       /**<The ISINK breath adjustment time, 1.107s. */
    HAL_ISINK_TIME_1_291_SECOND = 6,                       /**<The ISINK breath adjustment time, 1.291s.  */
    HAL_ISINK_TIME_1_507_SECOND = 7,                       /**<The ISINK breath adjustment time, 1.507s.  */
    HAL_ISINK_TIME_1_691_SECOND = 8,                       /**<The ISINK breath adjustment time, 1.691s. */
    HAL_ISINK_TIME_1_876_SECOND = 9,                       /**<The ISINK breath adjustment time, 1.876s. */
    HAL_ISINK_TIME_2_091_SECOND = 10,                      /**<The ISINK breath adjustment time, 2.091s.*/
    HAL_ISINK_TIME_2_276_SECOND = 11,                      /**<The ISINK breath adjustment time, 2.276s. */
    HAL_ISINK_TIME_2_460_SECOND = 12,                      /**<The ISINK breath adjustment time, 2.460s. */
    HAL_ISINK_TIME_2_676_SECOND = 13,                      /**<The ISINK breath adjustment time, 2.676s. */
    HAL_ISINK_TIME_2_860_SECOND = 14,                      /**<The ISINK breath adjustment time, 2.860s. */
    HAL_ISINK_TIME_3_075_SECOND = 15                       /**<The ISINK breath adjustment time, 3.075s. */
} hal_isink_breath_adjust_on_t ;

/** @brief ISINK breath mode off based on each time adjustment. */

typedef enum {
    HAL_ISINK_TIME_0_246_SECOND = 0,                       /**<The ISINK breath adjustment time, 0.246s.  */
    HAL_ISINK_TIME_0_677_SECOND = 1,                       /**<The ISINK breath adjustment time, 0.677s.  */
    HAL_ISINK_TIME_1_046_SECOND = 2,                       /**<The ISINK breath adjustment time, 1.046s. */
    HAL_ISINK_TIME_1_417_SECOND = 3,                       /**<The ISINK breath adjustment time, 1.417s. */
    HAL_ISINK_TIME_1_845_SECOND = 4,                       /**<The ISINK breath adjustment time, 1.845s. */
    HAL_ISINK_TIME_2_214_SECOND = 5,                       /**<The ISINK breath adjustment time, 2.214s.  */
    HAL_ISINK_TIME_2_583_SECOND = 6,                       /**<The ISINK breath adjustment time, 2.583s.  */
    HAL_ISINK_TIME_3_014_SECOND = 7,                       /**<The ISINK breath adjustment time, 3.014s.  */
    HAL_ISINK_TIME_3_383_SECOND = 8,                       /**<The ISINK breath adjustment time, 3.383s. */
    HAL_ISINK_TIME_3_752_SECOND = 9,                       /**<The ISINK breath adjustment time, 3.752s. */
    HAL_ISINK_TIME_4_183_SECOND = 10,                      /**<The ISINK breath adjustment time, 4.183s. */
    HAL_ISINK_TIME_4_552_SECOND = 11,                      /**<The ISINK breath adjustment time, 4.552s. */
    HAL_ISINK_TIME_4_921_SECOND = 12,                      /**<The ISINK breath adjustment time, 4.921s. */
    HAL_ISINK_TIME_5_351_SECOND = 13,                      /**<The ISINK breath adjustment time, 5.351s. */
    HAL_ISINK_TIME_5_720_SECOND = 14,                      /**<The ISINK breath adjustment time, 5.720s. */
    HAL_ISINK_TIME_6_151_SECOND = 15                       /**<The ISINK breath adjustment time, 6.151s. */
} hal_isink_breath_adjust_off_t ;


/** @brief ISINK status. */
typedef enum {
    HAL_ISINK_STATUS_ERROR               = -3,       /**< The ISINK function error occurred. */
    HAL_ISINK_STATUS_ERROR_CHANNEL       = -2,       /**< The ISINK error channel. */
    HAL_ISINK_STATUS_ERROR_INVALID_PARAMETER   = -1, /**< An error occurred, invalid parameter was given. */
    HAL_ISINK_STATUS_OK   = 0                       /**< The ISINK operation completed successfully.*/
} hal_isink_status_t;


/** @brief  This enum defines the ISINK running status. */
typedef enum {
    HAL_ISINK_IDLE = 0,                         /**< The ISINK is idle. */
    HAL_ISINK_BUSY = 1                          /**< The ISINK is busy. */
} hal_isink_running_status_t;

/**
  * @}
  */


/** @defgroup hal_isink_struct Struct
  * @{
  */

/** @brief isink config */
typedef struct {
    hal_isink_breath_adjust_on_t  darker_to_lighter_time1;         /**<The ISINK breath mode timming adjustment setting 1 corresponding to the brightness level from darker to lighter. */
    hal_isink_breath_adjust_on_t  darker_to_lighter_time2;         /**<The ISINK breath mode timming adjustment setting 2 corresponding to the brightness level from darker to lighter. */
    hal_isink_breath_adjust_on_t  lightest_time;                   /**< Duration of the brightest level.  */
    hal_isink_breath_adjust_on_t  lighter_to_darker_time1;         /**<The ISINK breath mode timming adjustment setting 1 corresponding to the brightness level from lighter  to darker. */
    hal_isink_breath_adjust_on_t  lighter_to_darker_time2;         /**<The ISINK breath mode timming adjustment setting 2 corresponding to the brightness level from lighter   to darker.*/
    hal_isink_breath_adjust_off_t darkest_time;                    /**< Duration of the darkest time.  */
} hal_isink_breath_mode_t;


/**
  * @}
  */


/**
 * @brief    This function initializes the ISINK hardware.
 * @param[in]  channel is the ISINK channel number. For more details about the parameter, please refer to #hal_isink_channel_t.
 * @return   Indicates whether this function call is successful or not.
 *            If the return value is #HAL_ISINK_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_ISINK_STATUS_ERROR_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @sa  hal_isink_deinit()
 */
hal_isink_status_t  hal_isink_init(hal_isink_channel_t channel);



/**
 * @brief    This function deinitializes the ISINK hardware.
 * @param[in]  channel is the ISINK channel number. For more details about the parameter, please refer to #hal_isink_channel_t.
 * @return     Indicates whether this function call is successful or not.
 *            If the return value is #HAL_ISINK_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_ISINK_STATUS_ERROR_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @sa  hal_isink_init()
 */
hal_isink_status_t  hal_isink_deinit(hal_isink_channel_t channel);



/**
 * @brief    This function sets the ISINK hardware clock source.
 * @param[in]  channel is the ISINK channel number. For more details about the parameter, please refer to #hal_isink_channel_t.
 * @param[in]  source_clock is the ISINK clock source setting. For more details about the parameter, please refer to #hal_isink_clock_source_t.
 * @return     Indicates whether this function call is successful or not.
 *            If the return value is #HAL_ISINK_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_ISINK_STATUS_ERROR_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @sa  hal_isink_set_mode()
 */
hal_isink_status_t hal_isink_set_clock_source(hal_isink_channel_t channel, hal_isink_clock_source_t source_clock);



/**
 * @brief    This function sets the ISINK hardware operation mode.
 * @param[in]  channel is the ISINK channel number. For more details about the parameter, please refer to #hal_isink_channel_t.
 * @param[in]  mode is the ISINK working mode setting. For more details about the parameter, please refer to #hal_isink_mode_t.
 * @return     Indicates whether this function call is successful or not.
 *            If the return value is #HAL_ISINK_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_ISINK_STATUS_ERROR_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @sa  hal_isink_set_clock_source()
 */
hal_isink_status_t hal_isink_set_mode(hal_isink_channel_t channel, hal_isink_mode_t mode);



/**
 * @brief    This function sets the ISINK hardware step current.
 * @param[in]  channel is the ISINK channel number. For more details about the parameter, please refer to #hal_isink_channel_t.
 * @param[in]  current is ISINK current setting. For more details about the parameter, please refer to #hal_isink_current_t.
 * @return     Indicates whether this function call is successful or not.
 *            If the return value is #HAL_ISINK_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_ISINK_STATUS_ERROR_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @sa  hal_isink_set_doule_current()
 */
hal_isink_status_t hal_isink_set_step_current(hal_isink_channel_t channel, hal_isink_current_t current);


/**
 * @brief    This function sets the ISINK hardware double current.
 * @param[in]  channel is the ISINK channel number. For more details about the parameter, please refer to #hal_isink_channel_t.
 * @param[in]  current is the current ISINK setting. For more details about the parameter, please refer to #hal_isink_current_t.
 * @return     Indicates whether this function call is successful or not.
 *            If the return value is #HAL_ISINK_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_ISINK_STATUS_ERROR_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @sa  hal_isink_set_step_current()
 */

hal_isink_status_t hal_isink_set_double_current(hal_isink_channel_t channel, hal_isink_current_t current);


/**
 * @brief    This function enables the ISINK hardware breath mode.
 * @param[in]  channel is the ISINK channel number. For more details about the parameter, please refer to #hal_isink_channel_t.
 * @param[in]  breath_mode is the ISINK breath mode setting. For more details about the parameter, please refer to #hal_isink_breath_mode_t.
 * @return     Indicates whether this function call is successful or not.
 *            If the return value is #HAL_ISINK_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_ISINK_STATUS_ERROR_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @sa  hal_isink_set_mode()
 */
hal_isink_status_t hal_isink_enable_breath_mode(hal_isink_channel_t channel, hal_isink_breath_mode_t breath_mode);



/**
 * @brief  This function gets the current status of the ISINK.
 * @param[in]  channel is the ISINK channel number. For more details about the parameter, please refer to #hal_isink_channel_t.
 * @param[out]  running_status is the ISINK busy or idle status. For more details about this parameter, please refer to #hal_isink_running_status_t .
 * @return     Indicates whether this function call is successful or not.
 *            If the return value is #HAL_ISINK_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_ISINK_STATUS_ERROR_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 */
hal_isink_status_t hal_isink_get_running_status(hal_isink_channel_t channel, hal_isink_running_status_t *running_status);



#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/

#endif /*HAL_ISINK_MODULE_ENABLED*/
#endif /* __HAL_ISINK_H__ */

