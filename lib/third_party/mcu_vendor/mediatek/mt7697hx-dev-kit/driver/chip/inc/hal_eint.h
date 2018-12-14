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

#ifndef __HAL_EINT_H__
#define __HAL_EINT_H__
#include "hal_platform.h"

#ifdef HAL_EINT_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup EINT
 * @{
 * This section introduces the External Interrupt Controller(EINT) APIs including terms and acronyms, supported features, software architecture, details on how to use this driver, EINT function groups, enums, structures and functions.
 *
 * @section HAL_EINT_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------|------------------------------------------------------------------------|
 * |\b EINT                 | External Interrupt Controller. Process the interrupt request from an external source or peripheral.|
 * |\b GPIO                 | General Purpose Inputs-Outputs. For more information, please refer to @ref GPIO module in HAL. |
 * |\b ISR                  | Interrupt service routine.|
 * |\b NVIC                 | Nested Vectored Interrupt Controller. NVIC is the interrupt controller of ARM Cortex-M4. For more details, please refer to <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100166_0001_00_en/ric1417175922867.html"> NVIC introduction in ARM Cortex-M4 Processor Technical Reference Manual </a>.|
 * @section HAL_EINT_Features_Chapter Supported features
 * The EINT controller has been designed to process interrupts from an external source or peripheral.
 *
 * - \b Support \b callback \b function \b registration. \n
 *   Call #hal_eint_clear_software_trigger() function to trigger an interrupt immediately after the EINT is unmasked.
 *   \n
 * @}
 * @}
 */

#ifdef HAL_EINT_FEATURE_MASK
/**
 * @addtogroup HAL
 * @{
 * @addtogroup EINT
 * @{
 *
 * @section HAL_EINT_Architecture_Chapter Software architecture of EINT
 *
 * For the architecture diagram please refer @ref HAL_Overview_3_Chapter.
 *
 *
 * @section HAL_EINT_Driver_Usage_Chapter How to use this driver
 *
 * - Use EINT. \n
 *  - 1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - 2: Call #hal_pinmux_set_function() to set the @ref GPIO pinmux. For more information about #hal_pinmux_set_function(), please refer to GPIO API reference.\n
 *  - 3: Call #hal_eint_mask() mask to select an EINT source. \n
 *  - 4: Initialize the selected EINT source. \n
 *      Call #hal_eint_init() to configue the EINT number's trigger mode and debounce time, or call #hal_eint_set_trigger_mode() and #hal_eint_set_debounce_time() to replace.
 *  - 5: Regist a callback function. \n
 *      Call #hal_eint_register_callback() to register a user callback function.
 *  - 6: Unmask the EINT. \n
 *      Call #hal_eint_unmask() to unmask the EINT.
 *  - Sample code:
 *    @code
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_4);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //function_index = HAL_GPIO_4_EINT3. Need not to configure pinmux if EPT tool is used.
 *       hal_pinmux_set_function(HAL_GPIO_4, function_index);
 *
 *       hal_eint_mask(eint_number);
 *       if (HAL_EINT_STATUS_OK !=  hal_eint_init(eint_number, &eint_config)) {
 *               // error handle
 *       }
 *       if (HAL_EINT_STATUS_OK !=  hal_eint_register_callback(eint_number, callback, user_data)) {
 *               // error handle
 *       }
 *
 *       // Change trigger mode or debounce time at runtime.
 *       hal_eint_mask(eint_number);
 *       if (HAL_EINT_STATUS_OK !=  hal_eint_set_trigger_mode(eint_number, trigger_mode)) {
 *               // error handle
 *       }
 *       if (HAL_EINT_STATUS_OK !=  hal_eint_set_debounce_time(eint_number, time_ms)) {
 *               // error handle
 *       }
 *       hal_eint_unmask(eint_number);
 *    @endcode
 *    @code
 *       // Callback function. This function should be registered with hal_eint_register_callback().
 *       void callback(void *user_data)
 *       {
 *           // Mask eint_number at first.
 *           hal_eint_mask(eint_number);
 *           // User's handler
 *           // Please call hal_eint_unmask() to unmask eint_number to receive an EINT interrupt.
 *           hal_eint_unmask(eint_number);
 *       }
 *    @endcode
 */
#else
/**
 * @addtogroup HAL
 * @{
 * @addtogroup EINT
 * @{
 *
 * @section HAL_EINT_Architecture_Chapter Software architecture of EINT
 *
 * For the architecture diagram please refer @ref HAL_Overview_3_Chapter.
 *
 *
 * @section HAL_EINT_Driver_Usage_Chapter How to use this driver
 *
 * - Use EINT. \n
 *  - 1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - 2: Call #hal_pinmux_set_function() to set the @ref GPIO pinmux. For more information about #hal_pinmux_set_function(), please refer to GPIO API reference.\n
 *  - 3: initialize the selected EINT source. \n
 *      Call #hal_eint_init() to configure the EINT number's trigger mode and debounce time, or call #hal_eint_set_trigger_mode() and #hal_eint_set_debounce_time() to replace.
 *  - 4: regist a callback function \n
 *      Call #hal_eint_register_callback() to register a user callback function.
 *  - Sample code:
 *    @code
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_4);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //function_index = HAL_GPIO_4_EINT3. Need not to configure pinmux if EPT tool is used.
 *       hal_pinmux_set_function(HAL_GPIO_4, function_index);
 *
 *       if (HAL_EINT_STATUS_OK !=  hal_eint_init(eint_number, &eint_config)) {
 *               // error handle
 *       }
 *       if (HAL_EINT_STATUS_OK !=  hal_eint_register_callback(eint_number, callback, user_data)) {
 *               // error handle
 *       }
 *
 *       //change trigger mode or debounce time at runtime.
 *       if (HAL_EINT_STATUS_OK !=  hal_eint_set_trigger_mode(eint_number, trigger_mode)) {
 *               // error handle
 *       }
 *       if (HAL_EINT_STATUS_OK !=  hal_eint_set_debounce_time(eint_number, time_ms)) {
 *               // error handle
 *       }
 *
 *    @endcode
.*
 *    @code
 *       // Callback function. This function should be registered with #hal_eint_register_callback().
 *       void callback(void *user_data)
 *       {
 *           // User's handler
 *       }
 *    @endcode
 */
#endif

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup hal_eint_enum Enum
  * @{
  */

/** @brief This emun defines the EINT trigger mode.  */
typedef enum {
    HAL_EINT_LEVEL_LOW     = 0,                 /**< Level and low trigger. */
    HAL_EINT_LEVEL_HIGH    = 1,                 /**< Level and high trigger. */
    HAL_EINT_EDGE_FALLING  = 2,                 /**< Edge and falling trigger. */
    HAL_EINT_EDGE_RISING   = 3,                 /**< Edge and rising trigger. */
    HAL_EINT_EDGE_FALLING_AND_RISING = 4        /**< Edge and falling or rising trigger. */
} hal_eint_trigger_mode_t;


/** @brief  This enum define the API return type.  */
typedef enum {
    HAL_EINT_STATUS_ERROR_EINT_NUMBER  = -3,     /**< EINT error number. */
    HAL_EINT_STATUS_INVALID_PARAMETER  = -2,     /**< EINT error invalid parameter. */
    HAL_EINT_STATUS_ERROR              = -1,     /**< EINT undefined error. */
    HAL_EINT_STATUS_OK                 = 0       /**< EINT operation completed successfully. */
} hal_eint_status_t;


/**
  * @}
  */


/** @defgroup hal_eint_struct Struct
  * @{
  */

/** @brief This structure defines the initial configuration structure. For more information please refer to #hal_eint_init(). */
typedef struct {
    hal_eint_trigger_mode_t trigger_mode;      /**< EINT trigger mode. */
    uint32_t debounce_time;                    /**< EINT hardware debounce time in milliseconds. EINT debounce is disabled when the debounce time is set to zero. */
} hal_eint_config_t;

/**
  * @}
  */


/** @defgroup hal_eint_typedef Typedef
  * @{
  */
/** @brief  This defines the callback function prototype.
 *          A callback function should be registered for every EINT in use.
 *          This function will be called after an EINT interrupt is triggered in the EINT ISR.
 *          For more details about the callback function, please refer to hal_eint_register_callback().
 *  @param [out] user_data is the parameter which is set manually using hal_eint_register_callback() function.
 */
typedef void (*hal_eint_callback_t)(void *user_data);

/**
  * @}
  */


/*****************************************************************************
* Functions
*****************************************************************************/

/**
 * @brief This function initializes the EINT number, it sets the EINT trigger mode and debounce time.
 * @param[in] eint_number is the EINT number, the value is HAL_EINT_0~HAL_EINT_MAX-1.
 * @param[in] eint_config is the initial configuration parameter. EINT debounce is disabled when debounce time is set to zero. For more details, please refer to #hal_eint_config_t.
 * @return    To indicate whether this function call is successful.
 *            If the return value is #HAL_EINT_STATUS_OK, the operation completed successfully;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 * @sa  hal_eint_deinit()
 * @par       Example
 * Sample code please refer to @ref HAL_EINT_Driver_Usage_Chapter
*/
hal_eint_status_t hal_eint_init(hal_eint_number_t eint_number, const hal_eint_config_t *eint_config);


/**
 * @brief This function deinitializes the EINT number, it resets the EINT trigger mode and debounce time.
 * @param[in] eint_number is the EINT number, the value is HAL_EINT_0~HAL_EINT_MAX-1.
 * @return    To indicate whether this function call is successful.
 *            If the return value is #HAL_EINT_STATUS_OK, the operation completed successfully;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 * @sa  hal_eint_deinit()
 * @par       Example
 * Sample code please refer to @ref HAL_EINT_Driver_Usage_Chapter
*/
hal_eint_status_t hal_eint_deinit(hal_eint_number_t eint_number);


/**
 * @brief This function registers a callback function for a specified EINT number.
 * @param[in] eint_number is the EINT number, the value is HAL_EINT_0~HAL_EINT_MAX-1, please refer to #hal_eint_number_t.
 * @param[in] callback is the function given by the user, which will be called at EINT ISR routine.
 * @param[in] user_data is a reserved parameter for user.
 * @return    To indicate whether this function call is successful.
 *            If the return value is #HAL_EINT_STATUS_OK, the operation completed successfully;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 * @sa  hal_eint_init()
 * @par       Example
 * Sample code please refer to @ref HAL_EINT_Driver_Usage_Chapter
 */
hal_eint_status_t hal_eint_register_callback(hal_eint_number_t eint_number,
        hal_eint_callback_t callback,
        void *user_data);


/**
 * @brief This function sets the EINT number to a trigger mode.
 * @param[in] eint_number is the EINT number, the value is from HAL_EINT_0 to HAL_EINT_MAX-1, please refer to #hal_eint_number_t.
 * @param[in] trigger_mode is EINT number trigger mode. For more details, please refer to #hal_eint_trigger_mode_t.
 * @return    To indicate whether this function call is successful.
 *            If the return value is #HAL_EINT_STATUS_OK, the operation completed successfully;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 @par         Example
 * Sample code please refer to @ref HAL_EINT_Driver_Usage_Chapter
 */
hal_eint_status_t hal_eint_set_trigger_mode(hal_eint_number_t eint_number,
        hal_eint_trigger_mode_t trigger_mode);


/**
 * @brief This function sets the EINT debounce time.
 * @param[in] eint_number is the EINT number, the value is from HAL_EINT_0 to HAL_EINT_MAX-1, please refer to #hal_eint_number_t.
 * @param[in] time_ms is the EINT number's hardware debounce time in milliseconds. EINT debounce is disabled when time_ms is set to zero.
 * @return    To indicate whether this function call  is successful.
 *            If the return value is #HAL_EINT_STATUS_OK, the operation completed successfully;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 @par         Example
 * Sample code please refer to @ref HAL_EINT_Driver_Usage_Chapter
 */
hal_eint_status_t hal_eint_set_debounce_time(hal_eint_number_t eint_number,
        uint32_t time_ms);

#ifdef HAL_EINT_FEATURE_SW_TRIGGER_EINT
/**
 * @brief This function triggers EINT software interrupt.
 * @param[in] eint_number is the EINT number, the value is from HAL_EINT_0 to HAL_EINT_MAX-1, please refer to #hal_eint_number_t.
 * @return    To indicate whether this function call is successful.
 *            If the return value is #HAL_EINT_STATUS_OK, the operation completed successfully;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 * @sa  hal_eint_clear_software_trigger()
 @par         Example
 * Sample code please refer to @ref HAL_EINT_Driver_Usage_Chapter
 */
hal_eint_status_t hal_eint_set_software_trigger(hal_eint_number_t eint_number);


/**
 * @brief This function clears the EINT software interrupt.
 * @param[in] eint_number is the EINT number, the value is from HAL_EINT_0 to HAL_EINT_MAX-1, please refer to #hal_eint_number_t.
 * @return    To indicate whether this function call is successful.
 *            If the return value is #HAL_EINT_STATUS_OK, the operation completed successfully;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 * @sa  hal_eint_set_software_trigger()
 @par         Example
 * Sample code please refer to @ref HAL_EINT_Driver_Usage_Chapter
 */
hal_eint_status_t hal_eint_clear_software_trigger(hal_eint_number_t eint_number);
#endif

#ifdef HAL_EINT_FEATURE_MASK
/**
 * @brief This function masks the dedicated EINT source.
 * @param[in] eint_number is the EINT number, the value is from HAL_EINT_0 to HAL_EINT_MAX-1, please refer to #hal_eint_number_t.
 * @return    To indicate whether this function call is successful.
 *            If the return value is #HAL_EINT_STATUS_OK,  the operation completed successfully;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 * @sa  #hal_eint_unmask
 */
hal_eint_status_t hal_eint_mask(hal_eint_number_t eint_number);


/**
 * @brief This function unmasks the dedicated EINT source.
 * @param[in] eint_number is the EINT number, the value is HAL_EINT_0~HAL_EINT_MAX-1, please refer to #hal_eint_number_t.
 * @return    To indicate whether this function call is successful.
 *            If the return value is #HAL_EINT_STATUS_OK,  the operation completed successfully;
 *            If the return value is #HAL_EINT_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 * @sa  hal_eint_mask()
 */
hal_eint_status_t hal_eint_unmask(hal_eint_number_t eint_number);
#endif


#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/

#endif /*HAL_EINT_MODULE_ENABLED*/
#endif /* __HAL_EINT_H__ */


