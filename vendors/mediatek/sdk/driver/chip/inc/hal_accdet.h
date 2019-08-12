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

#ifndef __HAL_ACCDET_H__
#define __HAL_ACCDET_H__
#include "hal_platform.h"

#ifdef HAL_ACCDET_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup ACCDET
 * @{
 * This section describes the programming interfaces of accessory detector (ACCDET) HAL driver.
 * The hardware ACCDET detects the status of an earphone (plug-in/plug-out/hook-key), based on the suggested
 * circuit. The de-bounce scheme is also supported to resist uncertain input noises. When the state is stable, the Pulse Width Modulation (PWM) unit of
 * ACCDET enables the voltage of the comparator periodically to detect the plugging state. Very low-power consumption can be achieved if the detection feature is enabled and the PWM settings are defined according to the needs of the hardware being controlled.
 * In order to compensate the delay between the detection login and comparator, the delay enabling scheme is adopted. Given the suitable delay
 * number compared to the rising edge of PWM high pulse, the stable plugging state can be prorogated to digital detection logic. Then the
 * correct plugging state can be detected and reported.
 *
 * @section HAL_ACCDET_Terms_Chapter Terms and acronyms
 *
 * The following provides descriptions to the terms commonly used in the ACCDET driver and how to use its various functions.
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b ACCDET                     | Accessory detector is designed to detect the status of earphone (plug-in/plug-out/hook-key).|
 * |\b PWM                        | Pulse Width Modulation (PWM) is a modulation technique that encodes a message into a pulsing signal. The main use is to allow the control of the power supplied to electrical devices, especially to inertial loads such as motors. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Pulse-width_modulation">introduction to PWM in Wikipedia</a>.|
 * For PWM, the term duty cycle describes the proportion of 'on' time to the regular interval or 'period' of time; a low duty cycle corresponds to low power, because the power is off for most of the time. Duty cycle is expressed in percent, 100% being fully on.
 *
 * @section HAL_ACCDET_Features_Chapter Supported features
 * As described above, to perform an accurate detection, the debounce time and proper PWM settings are supported. The supported features of ACCDET driver are listed below: \n
 * - \b Set \b debounce \b time \b separately \b for \b plug-in/plug-out/hook-key. \n
 *   The API dedicated for debounce time setting is hal_accdet_set_debounce_time(), and the parameter is of type #hal_accdet_debounce_time_t. The parameters included are accdet_plug_in_debounce, accdet_plug_out_debounce and accdet_hook_key_debounce. User can adjust the three fields according to the sensitivity of the specific hardware circuit.
 *
 * - \b Callback \b function \b registration. \n
 *   If the state of the plugged earphone is changed, an interrupt is triggered. Users could register their own callback function, by calling hal_accdet_register_callback(). The registered callback function is invoked in the ACCDET ISR routine once an interrupt is triggered.
 *
 * @section ACCDET_Driver_Usage_Chapter How to use this driver
 *
 *  - \b Configure \b ACCDET \b module \b for \b earphone \b detection. \n
 *  Before the ACCDET module is enabled for earphone detection, user should configure the debounce time for all three states, and the callback function should be registered as well. The function hal_accdet_set_debounce_time() could be called anytime, to adjust the debounce time to a proper value, as shown in the following steps.
 *   - Step1: Call hal_accdet_init() to initialize the ACCDET module.
 *   - Step2: Call hal_accdet_set_debounce_time() to set debounce time separately for plug-in/plug-out/hook-key.
 *   - Step3: Call hal_accdet_register_callback() to register a callback function.
 *   - Step4: Call hal_accdet_enable() to enable the ACCDET module.
 *   - Step5: Call hal_accdet_disable() to disable the ACCDET module.
 *   - Step6: Call hal_accdet_deinit() to deinitialize the ACCDET module.
 *   - sample code:
 *   @code
 *
 *   #define HOOK_KEY_DEBOUNCE_TIME  300//Hook key debounce is 300ms
 *   #define PLUG_IN_DEBOUNCE_TIME   500//Plug in debounce is 500ms
 *   #define PLUG_OUT_DEBOUNCE_TIME  50//plug out debounce is 50ms
 *
 *   hal_accdet_debounce_time_t accdet_debounce;
 *   uint32_t user_data;
 *
 *   accdet_debounce.accdet_hook_key_debounce = HOOK_KEY_DEBOUNCE_TIME;//Set debounce time for hook key
 *   accdet_debounce.accdet_plug_in_debounce = PLUG_IN_DEBOUNCE_TIME;//Set debounce time for plug in
 *   accdet_debounce.accdet_plug_out_debounce = PLUG_OUT_DEBOUNCE_TIME;//Set debounce time for plug out
 *
 *   hal_accdet_init();//Initialize the ACCDET module
 *   hal_accdet_set_debounce_time(&debounce_time);//Set debounce time for ACCDET
 *   hal_accdet_register_callback(accdet_user_callback, (void *)&user_data);//Register a callback function
 *   hal_accdet_enable();//Enable ACCDET
 *
 *   hal_accdet_disable();//Disable the ACCDET module
 *   hal_accdet_deinit();//Deinitialize the ACCDET module
 *
 *   @endcode
 *     .
 *   @code
 *     // Callback function. This function should be registered with #hal_accdet_register_callback.
 *     void accdet_user_callback(hal_accdet_callback_event_t event, void *user_data);
 *        {
 *           switch (event) {
 *               case HAL_ACCDET_EVENT_HOOK_KEY:
 *                   //user's handler
 *                   //break;
 *               case HAL_ACCDET_EVENT_PLUG_IN:
 *                  // user's handler
 *                  // break;
 *               case HAL_ACCDET_EVENT_PLUG_OUT:
 *                  // user's handler
 *                  // break;
 *               default:
 *                  // user's handler
 *                  // break;
 *            }
 *        }
 *
 *   @endcode
 *
 */


#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * Enums
 *****************************************************************************/

/** @defgroup hal_accdet_enum Enum
 *  @{
 */

/** @brief This enum defines the ACCDET EVENT type.  */
typedef enum {
    HAL_ACCDET_EVENT_HOOK_KEY = 0,             /**< ACCDET event hook key */
    HAL_ACCDET_EVENT_PLUG_IN = 1,              /**< ACCDET event plug in */
    HAL_ACCDET_EVENT_ERROR = 2,                /**< ACCDET event error */
    HAL_ACCDET_EVENT_PLUG_OUT = 3              /**< ACCDET event plug out */
} hal_accdet_callback_event_t;


/** @brief This enum defines the ACCDET status type.  */
typedef enum {
    HAL_ACCDET_STATUS_INVALID_PARAMETER = -3,  /**< Invalid parameter */
    HAL_ACCDET_STATUS_ERROR_BUSY = -2,         /**< ACCDET is busy */
    HAL_ACCDET_STATUS_ERROR = -1,              /**< ACCDET status error */
    HAL_ACCDET_STATUS_OK = 0                   /**< ACCDET status ok */
} hal_accdet_status_t;

/**
 * @}
 */

/*****************************************************************************
 * Structures
 *****************************************************************************/

/** @defgroup hal_accdet_struct Struct
 *  @{
 */

/** @brief ACCDET debounce time*/
typedef struct {
    uint16_t accdet_hook_key_debounce;          /**< ACCDET hook key debounce time */
    uint16_t accdet_plug_in_debounce;           /**< ACCDET plug in debounce time */
    uint16_t accdet_plug_out_debounce;          /**< ACCDET plug out debounce time */
} hal_accdet_debounce_time_t;

/**
 * @}
 */

/** @defgroup hal_accdet_typedef Typedef
 *  @{
 */

/** @brief ACCDET callback typedef, when the plug state changes, an interrupt is triggered and the registered callback is invoked.*/
typedef void (*hal_accdet_callback_t)(hal_accdet_callback_event_t event, void *user_data);

/**
 * @}
 */

/*****************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief 	ACCDET initialize function.
 * @return
 * #HAL_ACCDET_STATUS_ERROR_BUSY, ACCDET is busy. \n
 * #HAL_ACCDET_STATUS_OK, ACCDET module is successfully initialized.
 */
hal_accdet_status_t hal_accdet_init(void);


/**
 * @brief  ACCDET deinitialization function.
 * @return
 * #HAL_ACCDET_STATUS_OK, ACCDET module is successfully deinitialized.
 */
hal_accdet_status_t hal_accdet_deinit(void);


/**
 * @brief 	ACCDET enable function. Enable the ACCDET module.
 * @return
 * #HAL_ACCDET_STATUS_OK, ACCDET module is successfully enabled.
 */
hal_accdet_status_t hal_accdet_enable(void);


/**
 * @brief 	ACCDET disable function. Disable the ACCDET module.
 * @return
 * #HAL_ACCDET_STATUS_OK, ACCDET module is successfully disabled.
 */
hal_accdet_status_t hal_accdet_disable(void);


/**
 * @brief 	register callback function for ACCDET interrupt.
 * @param[in] accdet_callback is the pointer to the ACCDET callback function.
 * @param[in] user_data is the user data of callback function.
 * @return
 * #HAL_ACCDET_STATUS_OK, the callback function of the ACCDET is registered successfully. \n
 * #HAL_ACCDET_STATUS_INVALID_PARAMETER, accdet_callback is NULL.
 */
hal_accdet_status_t hal_accdet_register_callback(hal_accdet_callback_t accdet_callback, void *user_data);


/**
 * @brief   set debounce time for the ACCDET.
 * @param[in] debounce_time is pointer of the debounce value structure of ACCDET, the unit of debounce is millisecond.
 * @return
 * #HAL_ACCDET_STATUS_OK, the debounce time of ACCDET is set successfully. \n
 * #HAL_ACCDET_STATUS_INVALID_PARAMETER, debounce_time is NULL.
 */
hal_accdet_status_t hal_accdet_set_debounce_time(const hal_accdet_debounce_time_t *debounce_time);


#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
*/

#endif /* HAL_ACCDET_MODULE_ENABLED */
#endif /* __HAL_ACCDET_H__ */


