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

#ifndef __HAL_IRRX_H__
#define __HAL_IRRX_H__

#include "hal_platform.h"

#ifdef HAL_IRRX_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup IRRX
 * @{
 * This section introduces the Infrared Receiver(IRRX) APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, IRRX function groups, enums, structures and functions.
 *
 * @section HAL_IRRX_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b IRRX                       | Infrared Receiver that detects infrared radiation. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Infrared_Data_Association"> introduction to IrDA in Wikipedia </a>. |
 *
 * @section HAL_IRRX_Features_Chapter Supported features
 *
 * - \b Support \b RC5 \b and \b PWD \b modes. \n
 *   MT7687 supports two hardware decoding methods:
 *  - \b PWD \b mode:
 *  PWD is pulse-width-detection in short. In this mode, the hardware detects the wavelengths, any type of code
 *  transmitted by infrared radiation can be received.The received content could be determined by using the wavelength.
 *  - \b RC5 \b mode:
 *  The Philips RC5 IR transmission protocol uses Manchester encoding of the message bits.
 *  In this mode, the RC5 code can be received and decoded directly.
 *
 * @section HAL_IRRX_Driver_Usage_Chapter How to use this driver
 *
 * - Use IRRX in the RC5 mode. \n
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Set GPIO34 to RX function to enable the IRRX mode.
 *  - Step3: Call hal_irrx_init() to initialize the IRRX.
 *  - Step4: Call hal_irrx_receive_rc5_start() to start the hardware.
 *  - Step5: Call hal_irrx_receive_rc5() to receive the RC5 code.
 *  - Step6: Call hal_irrx_deinit() to deinitialize the IRRX.
 *  - sample code:
 *    @code
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_34);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //function_index = HAL_GPIO_34_IR_RX.Need not to configure pinmux if EPT tool is used.
 *       hal_pinmux_set_function(HAL_GPIO_34, function_index); //Sets the GPIO34 to IRRX mode.
 *
 *       hal_irrx_init();
 *       hal_irrx_receive_rc5_start(HAL_IRRX_RC5_RECEIVE_FORMAT_BIT_REVERSE,
 *                                  user_callback, NULL);    //A callback function for the hardware will be triggered when the received code is decoded.
 *    @endcode
 *    @code
 *       // Callback function. This function should be registered with #hal_irrx_receive_rc5_start().
 *       void user_callback (void *user_data)
 *       {
 *           hal_irrx_deinit();
 *           // user's handler
 *       }
 *
 *    @endcode
 *
 * - Use IRRX in the PWD mode. \n
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Set GPIO34 to RX function to enable the IRRX mode.
 *  - Step3: Call hal_irrx_init() to initialize the IRRX.
 *  - Step4: Set the PWD parameters: inverse and terminate_threshold.
 *  - Step5: Call hal_irrx_receive_pwd_start() to start the hardware.
 *  - Step6: Call hal_irrx_receive_pwd() to receive the PWD code.
 *  - Step7: Call hal_irrx_receive_pwd_start() once again to restart the PWD mode.
 *  - Step8: Call hal_irrx_deinit() to deinitialize the IRRX.
 *  - sample code:
 *    @code
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_34);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //function_index = HAL_GPIO_34_IR_RX.Need not to configure pinmux if EPT tool is used.
 *       hal_pinmux_set_function(HAL_GPIO_34, function_index);//Set the GPIO34 to IRRX mode.
 *
 *       hal_irrx_init();
 *       uint32_t us = 0;
 *       hal_irrx_pwd_config_t format = {0};    //Initializes the PWD parameter.
 *       format.inverse = 1;
 *       format.terminate_threshold = 10200;
 *       if (HAL_IRRX_STATUS_OK != hal_irrx_receive_pwd_start(&format,
 *                                                            user_callback,
 *                                                            NULL,
 *                                                            &us)) {
 *          dbg_error("hal_irrx_receive_pwd_start fail!");
 *       }
 *       if (HAL_IRRX_STATUS_OK != hal_irrx_receive_pwd(&received_length,
 *                                                      data,
 *                                                      sizeof(data))) {
 *          dbg_error("hal_irrx_receive_pwd fail!");
 *       }
 *    @endcode
 *    @code
 *       // Callback function. This function should be registered with #hal_irrx_receive_pwd().
 *       void user_callback (void *user_data)
 *       {
 *           hal_irrx_deinit();
 *           // user's handler
 *       }
 *
 *    @endcode
 * - Use IRRX driver to trigger IRRX RC5/PWD manually. \n
 *
 *
 */



#ifdef __cplusplus
extern "C" {
#endif



/** @defgroup hal_irrx_enum Enum
  * @{
  */


/** @brief RC5 and PWD modes are supported. */
typedef enum {
    HAL_IRRX_STATUS_INVALID_PARAM = -2,     /**< IRRX invalid parameter */
    HAL_IRRX_STATUS_NOT_SUPPORTED = -1,    /**< IRRX not support status */
    HAL_IRRX_STATUS_OK = 0                 /**< IRRX status ok */
} hal_irrx_status_t;


/** @brief IRRX transaction error. */
typedef enum {
    HAL_IRRX_EVENT_TRANSACTION_ERROR = -1,   /**< IRRX transaction error */
    HAL_IRRX_EVENT_TRANSACTION_SUCCESS = 0   /**< IRRX transaction success */
} hal_irrx_event_t;

/** @brief IRRX  running status */
typedef enum {
    HAL_IRRX_IDLE = 0,                     /**< IRRX idle */
    HAL_IRRX_BUSY = 1                      /**< IRRX busy  */
} hal_irrx_running_status_t;


/**
  * @}
  */

/** @defgroup hal_irrx_typedef Typedef
   * @{
   */

/** @brief  This defines the callback function prototype.
 *          Register a callback function when in an interrupt mode, this function is called in IRRX interrupt
 *          service routine after a transaction is complete.
 *  @param [in] event is the transaction event for the current transaction. Application obtains the transaction result from this parameter.
 *              For more details about the event type, please refer to #hal_irrx_event_t.
 *  @param [in] user_data is a parameter provided  by the application.
 */

typedef void (*hal_irrx_callback_t)(hal_irrx_event_t event, void  *user_data);

/**
  * @}
  */

/** @defgroup hal_irrx_struct Struct
  * @{
  */

/** @brief RC5 code. The structure member bits specifies the valid bits in code[2]. */
typedef struct {
    uint8_t     bits;  /**<  RC5 recevied bits number. */
    uint32_t    code[2];   /**<  RC5 received data code. */
} hal_irrx_rc5_code_t;


/** @brief Pulse-Width-Modulation receive config*/
typedef struct {
    uint8_t     inverse;  /**<  PWD signal format inverse. */
    uint32_t    terminate_threshold;  /**<  PWD terminate threshold count number,to calucate thresold time:uint is us. */
} hal_irrx_pwd_config_t;

/**
  * @}
  */




/** @defgroup hal_irrx_define Define
  * @{
 */

/** @brief When applied to RC5 receive format, this flag causes the IR pulse inversed  before being decoded. */
#define HAL_IRRX_RC5_RECEIVE_FORMAT_SIGNAL_INVERSE      (0x1)     /**< IRRX signal reverse format*/


/** @brief When applied to RC5 receive format, this flag causes a bit order reverse. The decoded IR pulse is bit-reversed.*/
#define HAL_IRRX_RC5_RECEIVE_FORMAT_BIT_REVERSE         (0x10)   /**< IRRX bit reverse format */

/**
  * @}
  */

/**
 * @brief    This function initializes the IRRX hardware clock.
 * @return   Indicates whether this function call is successful or not.
 *                If the return value is #HAL_IRRX_STATUS_OK, the operation completed successfully.
 *                If the return value is #HAL_IRRX_STATUS_INVALID_PARAM, it means a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * Sample code, please refer to @ref HAL_IRRX_Driver_Usage_Chapter.
 * @sa  hal_irrx_deinit()
 */

hal_irrx_status_t hal_irrx_init(void);


/**
 * @brief    This function deinitializes the IRRX hardware clock.
 * @return  Indicates whether this function call is successful or not.
 *                If the return value is #HAL_IRRX_STATUS_OK, the operation completed successfully.
 *                If the return value is #HAL_IRRX_STATUS_INVALID_PARAM, it means a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * Sample code, please refer to @ref HAL_IRRX_Driver_Usage_Chapter.
 * @sa  hal_irrx_init()
 */

hal_irrx_status_t hal_irrx_deinit(void);



/**
 * @brief     This function starts to receive the infrared radiation RC5 code.
 * @param[in] format  the combination of #HAL_IRRX_RC5_RECEIVE_FORMAT_SIGNAL_INVERSE,
 *                 and #HAL_IRRX_RC5_RECEIVE_FORMAT_BIT_REVERSE.
 * @param[in] callback the callback function to be called when the receive operation is complete.
 * @param[in] parameter the parameter to the caller when return.
 * @return    Indicates whether this function call is successful or not.
 *                If the return value is #HAL_IRRX_STATUS_OK, the operation completed successfully.
 *                If the return value is #HAL_IRRX_STATUS_INVALID_PARAM, it means a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * Sample code, please refer to @ref HAL_IRRX_Driver_Usage_Chapter.
 * @sa  hal_irrx_receive_rc5()
 */



hal_irrx_status_t hal_irrx_receive_rc5_start(uint8_t             format,
        hal_irrx_callback_t callback,
        void                *parameter);
/**
 * @brief     This function receives the infrared radiation RC5 code.
 * @param[in] code the infrared radiation RC5 code format definition. For more details about the parameter, please refer to #hal_irrx_rc5_code_t.
 * @return   Indicates whether this function call is successful or not.
 *                If the return value is #HAL_IRRX_STATUS_OK, the operation completed successfully.
 *                If the return value is #HAL_IRRX_STATUS_INVALID_PARAM, it means a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * Sample code, please refer to @ref HAL_IRRX_Driver_Usage_Chapter.
 * @sa  hal_irrx_receive_rc5_start()
 */

hal_irrx_status_t hal_irrx_receive_rc5(hal_irrx_rc5_code_t *code);


/**
 * @brief     This function starts to receive the infrared radiation PWD code.
 * @param[in] config it's used when the receive operation starts.
 * @param[in] callback the callback function to be called when the receive operation starts.
 * @param[out] precision_us the PWD dectection precise time.
 * @return    Indicates whether this function call is successful or not.
 *                If the return value is #HAL_IRRX_STATUS_OK, the operation completed successfully.
 *                If the return value is #HAL_IRRX_STATUS_INVALID_PARAM, it means a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * Sample code, please refer to @ref HAL_IRRX_Driver_Usage_Chapter.
 * @sa  hal_irrx_receive_pwd()
 */

hal_irrx_status_t hal_irrx_receive_pwd_start(
    const hal_irrx_pwd_config_t     *config,
    hal_irrx_callback_t             callback,
    uint32_t                        *precision_us);


/**
 * @brief     This function  receives the infrared radiation PWD code.
 * @param[in]  received_length the actual received length of code in the buffer.
 * @param[in]   buffer the buffer to be used to stored received code.
 * @param[in]  buffer_length the length of buffer that can be used to store  received code.
 *                    The maximum is 68 bytes and it is suggested to prepare 68 bytes of space for buffer and specify 68 as buffer_length.
 * @return    Indicates whether this function call is successful or not.
 *                If the return value is #HAL_IRRX_STATUS_OK, the operation completed successfully.
 *                If the return value is #HAL_IRRX_STATUS_INVALID_PARAM, it means a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * Sample code, please refer to @ref HAL_IRRX_Driver_Usage_Chapter.
 * @sa  hal_irrx_receive_pwd()
 */

hal_irrx_status_t hal_irrx_receive_pwd(uint8_t  *received_length,
                                       uint8_t  *buffer,
                                       uint8_t  buffer_length);


/**
 * @brief    This function gets the current state of the IRRX.
 * @param[out] running_status is the current running status.
 *             #HAL_IRRX_BUSY means the IRRX is in busy status.
 *             #HAL_IRRX_IDLE means the IRRX is in idle status, user can use it to transfer data now.
 * @return   Indicates whether this function call is successful or not.
 *                If the return value is #HAL_IRRX_STATUS_OK, the operation completed successfully.
 * @par       Example
 * Sample code, please refer to @ref HAL_IRRX_Driver_Usage_Chapter.
 * @sa   hal_irrx_receive_pwd()
 */

hal_irrx_status_t hal_irrx_get_running_status(hal_irrx_running_status_t *running_status);



#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
*/

#endif /*HAL_IRRX_MODULE_ENABLED*/
#endif /* __HAL_IRRX_H__ */



