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

#ifndef __HAL_IRTX_H__
#define __HAL_IRTX_H__

#include "hal_platform.h"

#ifdef HAL_IRTX_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup IRTX
 * @{
 *   This section introduces the Infrared Transmitter(IRTX) APIs including terms and acronyms,
 *   supported features, software architecture, details on how to use this driver, IRTX function groups, enums, structures and functions.
 *
 * @section HAL_IRTX_Terms_Chapter Terms and acronyms
 *
 * The following provides descriptions to the terms commonly used in the IRTX driver and how to use its various functions.
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b IRTX                       | Infrared Transmitter is the transmitter element to send infrared radiation. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Infrared_Data_Association"> introduction to IrDA in Wikipedia </a>. |
 *
 * @section HAL_IRTX_Features_Chapter Supported features
 * - \b Support \b NEC, \b RC5, \b RC6 and \b PWD \b modes. \n
 *   MT7687 has four encoding modes:
 * - \b The \b NEC \b mode:
 *   The NEC encoding format is one of the widely used infrared protocols. In this mode, the NEC
 *   encoded code can be directly sent by the hardware.
 * - \b The \b PWD \b mode:
 *   PWD is Pulse width detection(PWD). In this mode, any type of code transmitted by infrared radiation
 *   can be sent. The transmitted content can be determined using the wavelength.
 * - \b The \b RC5 \b mode:
 *   The Philips RC5 IR transmission protocol uses Manchester encoding of the message bits.
 *   In this mode, the RC5 encoded code can be directly sent by the hardware.
 * - \b The \b RC6 \b mode:
 *   Similar to the RC-5 the new RC-6 protocol was also defined by Philips. In this mode,
 *   the RC6 encoded code can be directly sent by the hardware.
 *
 *
 * @section HAL_IRTX_Driver_Usage_Chapter How to use this driver
 * - Use IRTX in the NEC/RC5/RC6 mode, Maximum transfer size is 12 Bytes.  \n
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Set GPIO33 to TX function to enable the IRTX mode.
 *  - Step3: Call hal_irtx_init() to initialize the IRTX.
 *  - Step4: start the hardware in NEC/RC5/RC6 mode
 *           if use NEC mode:Call hal_irtx_send_nec_data() to start the hardware.
 *           if use RC5 mode:Call hal_irtx_send_rc5_data() to start the hardware.
 *           if use RC6 mode:Call hal_irtx_send_rc6_data() to start the hardware.
 *  - Step5: Call hal_irtx_deinit() to deinitialize the IRTX.
 *  - Sample code:
 *    @code
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_33);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //function_index = HAL_GPIO_33_IR_TX.Need not to configure pinmux if EPT tool is used.
 *       hal_pinmux_set_function(HAL_GPIO_33, function_index);//Set the GPIO33 to IRTX mode.
 *
 *       hal_irtx_init();
 *       uint8_t data[HAL_IRTX_MAX_DATA_LENGTH] = {
 *           0xA5, 0xF0, 0xAF, 0x00,
 *           0xff, 0x00, 0xAA, 0x55
 *       };
 *
 *       //If use NEC mode:  hal_irtx_send_nec_data(HAL_IRTX_REPEAT_DISABLE,data, 64)
 *       //If use RC5 mode:  hal_irtx_send_rc5_data(HAL_IRTX_REPEAT_DISABLE,data, 64)
 *       //If use RC6 mode:  hal_irtx_send_rc6_data(HAL_IRTX_REPEAT_DISABLE,data, 64)
 *       if (HAL_IRTX_STATUS_OK != hal_irtx_send_nec_data(HAL_IRTX_REPEAT_DISABLE,
 *                                                 data, 64)) {     //Use HAL_IRTX_REPEAT_ENABLE to send the data continuously.
 *           printf("hal_irtx_send_nec_data fail.\r\n");
 *
 *       }
 *     hal_irtx_deinit();
 *    @endcode
 *
 * - Use IRTX in the PWD mode, Maximum transfer size is 68 Bytes. \n
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Set GPIO33 to TX function to enable the IRTX mode.
 *  - Step3: Call hal_irtx_init() to initialize the IRTX.
 *  - Step4: Call hal_irtx_register_pulse_data_callback() to register callback function.
 *  - Step5: Call hal_irtx_configure_pulse_data_carrier() to set frequency and duty_cycle.
 *  - Step6: Call hal_irtx_send_pulse_data() to send data.
 *  - Step7: Call hal_irtx_deinit() to de-initialize the IRTX.
 *  - Sample code:
 *    @code
 *       static void isr_ir_tx(void *arg)
 *       {
 *           ir_tx_done = 1;
 *       }
 *
 *       uint8_t data_number = 41;
 *       uint8_t data[68] = {
 *           225, 112, 17, 15, 34, 15, 35, 15, 17, 15, 34, 15, 17, 15, 17, 15,
 *           17, 15, 34, 15, 17, 15, 34, 15, 34, 15, 17, 15, 34, 15, 17, 15,
 *           17, 15, 17, 15, 34, 15, 34, 15, 34
 *       };
 *
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_33);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //function_index = HAL_GPIO_33_IR_TX.Need not to configure pinmux if EPT tool is used.
 *       hal_pinmux_set_function(HAL_GPIO_33, function_index);//Set the GPIO33 to IRTX mode.
 *
 *       hal_irtx_init();
 *       printf("output Software Pluse Width IR \r\n");
 *       if (HAL_IRTX_STATUS_OK != hal_irtx_register_pulse_data_callback(isr_ir_tx, NULL)) {
 *           printf("hal_irtx_send_pulse_data failed.\r\n");
 *           return;
 *       }
 *
 *       uint16_t frequency = 40;//40KHz
 *       uint8_t duty_cycle = 25;// 25% duty cycle
 *       if (HAL_IRTX_STATUS_OK != hal_irtx_configure_pulse_data_carrier(frequency, duty_cycle)) {
 *           printf("hal_irtx_send_pulse_data failed.\r\n");
 *           return;
 *       }
 *
 *       uint8_t base_period = 80; // unit:0.5us, SWM_BP=80, PW_value saturates for one cycle of the NEC leading code with duration of 9ms (high) and 4.5ms(low).
 *       if (HAL_IRTX_STATUS_OK != hal_irtx_send_pulse_data(base_period, data, data_number)) {
 *           printf("hal_irtx_send_pulse_data failed.\r\n");
 *           return;    }
 *
 *       while (!ir_tx_done) {
 *       }//Wait for an interrupt
 *      hal_irtx_deinit();
 *    @endcode
 *
 */



#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup hal_irtx_define Define
  * @{
 */

/** @brief irtx max data length */
#define HAL_IRTX_MAX_DATA_LENGTH (12)     /**< IRTX maxium data length */

/**
  * @}
 */



/** @defgroup hal_irtx_enum Enum
  * @{
 */

/** @brief The IRTX repeat mode */
typedef enum {
    HAL_IRTX_REPEAT_ENABLE = 0,             /**< IRTX repeat mode enable */
    HAL_IRTX_REPEAT_DISABLE                /**< IRTX repeat mode disable  */
} hal_irtx_repeat_code_t;


/** @brief The IRRX status */
typedef enum {
    HAL_IRTX_STATUS_ERROR = -1,                  /**< IRTX status error */
    HAL_IRTX_STATUS_OK = 0                       /**< IRTX status ok */
} hal_irtx_status_t;


/** @brief The IRRX transaction error */
typedef enum {
    HAL_IRTX_EVENT_TRANSACTION_ERROR = -1,       /**< IRTX transaction error */
    HAL_IRTX_EVENT_TRANSACTION_SUCCESS = 0      /**< IRTX transaction success */
} hal_irtx_event_t;


/** @brief The IRRX running status */
typedef enum {
    HAL_IRTX_IDLE = 0,                         /**< IRTX idle */
    HAL_IRTX_BUSY = 1                         /**< IRTX busy */
} hal_irtx_running_status_t;


/**
  * @}
 */

/** @defgroup hal_irtx_typedef Typedef
   * @{
   */

/** @brief  This defines the callback function prototype.
 *          Register a callback function when in an interrupt mode, this function is called in the IRTX interrupt
 *          service routine after a transaction is complete.
 *  @param [in] event is the transaction event for the current transaction, application can get the transaction result from this parameter.
 *              For more details about the event type, please refer to #hal_irtx_event_t.
 *  @param [in] user_data is a parameter provided by the application.
 */



/** @brief The IRTX callback */
typedef void (*hal_irtx_pulse_data_callback_t)(hal_irtx_event_t event, void  *user_data);

/**
  * @}
 */

/**
 * @brief    This function initializes the IRTX hardware clock.
 * @return   Indicates whether this function call is successful or not.
 *                If the return value is #HAL_IRTX_STATUS_OK, the operation completed successfully.
 *                If the return value is #HAL_IRTX_STATUS_ERROR, the operation failed.
 * @par       Example
 * Sample code please refer to @ref HAL_IRTX_Driver_Usage_Chapter.
 * @sa  hal_irtx_deinit()
 */

hal_irtx_status_t hal_irtx_init(void);


/**
 * @brief    This function deinitializes the IRTX hardware clock.
 * @return   Indicates whether this function call is successful or not.
 *                If the return value is #HAL_IRTX_STATUS_OK, the operation completed successfully.
 *                If the return value is #HAL_IRTX_STATUS_ERROR, the operation failed.
 * @par       Example
 * Sample code, please refer to @ref HAL_IRTX_Driver_Usage_Chapter.
 * @sa  hal_irtx_init()
 */

hal_irtx_status_t hal_irtx_deinit(void);



/**
 * @brief    This function sends the data in the NEC encoding format.
 * @param[in] repeat_code  repeat code for the NEC IRTX.
 * @param[in] data data to be sent.
 * @param[in] bit_length  the number of bits in the message.
 * @return   Indicates whether this function call is successful or not.
 *                If the return value is #HAL_IRTX_STATUS_OK, the operation completed successfully.
 *                If the return value is #HAL_IRTX_STATUS_ERROR, the operation failed.
 * @par       Example
 * Sample code, please refer to @ref HAL_IRTX_Driver_Usage_Chapter.
 */

hal_irtx_status_t hal_irtx_send_nec_data(hal_irtx_repeat_code_t repeat_code,
        const uint8_t data[HAL_IRTX_MAX_DATA_LENGTH],
        uint32_t bit_length);



/**
 * @brief     This function sends the data in the RC5 encoding format.
 * @param[in] repeat_code  repeat code for the RC5 IRTX.
 * @param[in] data the data content to be sent.
 * @param[in] bit_length  the number of bits in the message.
 * @return   Indicates whether this function call is successful or not.
 *                If the return value is #HAL_IRTX_STATUS_OK, the operation completed successfully.
 *                If the return value is #HAL_IRTX_STATUS_ERROR, the operation failed.
 * @par       Example
 * Sample code, please refer to @ref HAL_IRTX_Driver_Usage_Chapter.
 */

hal_irtx_status_t hal_irtx_send_rc5_data(hal_irtx_repeat_code_t repeat_code,
        const uint8_t data[HAL_IRTX_MAX_DATA_LENGTH],
        uint32_t bit_length);


/**
 * @brief    This function sends the data in the RC6 encoding format.
 * @param[in] repeat_code  repeat code for the RC6 IRTX.
 * @param[in] data the data content to be sent.
 * @param[in] bit_length  the number of bits in the message.
 * @return   Indicates whether this function call is successful or not.
 *                If the return value is #HAL_IRTX_STATUS_OK, the operation completed successfully.
 *                If the return value is #HAL_IRTX_STATUS_ERROR, the operation failed.
 * @par       Example
 * Sample code, please refer to @ref HAL_IRTX_Driver_Usage_Chapter.
 */

hal_irtx_status_t hal_irtx_send_rc6_data(hal_irtx_repeat_code_t repeat_code,
        const uint8_t data[HAL_IRTX_MAX_DATA_LENGTH],
        uint32_t bit_length);



/**
 * @brief    This function configures the pulse data carrier.
 * @param[in] frequency  pulse data carrier frequency.
 * @param[in] duty_cycle pulse data carrier duty cycle.
 * @return   Indicates whether this function call is successful or not.
 *                If the return value is #HAL_IRTX_STATUS_OK, the operation completed successfully.
 *                If the return value is #HAL_IRTX_STATUS_ERROR, the operation failed.
 * @par       Example
 * Sample code, please refer to @ref HAL_IRTX_Driver_Usage_Chapter.
 */

hal_irtx_status_t hal_irtx_configure_pulse_data_carrier(uint32_t frequency,
        uint32_t duty_cycle);


/**
 * @brief    This function registers the PWD encoding format data callback. Call it after the transmission is complete.
 * @param[in] callback user defined callback function.
 * @param[in] user_data  a user defined extension data.
 * @return   Indicates whether this function call is successful or not.
 *                If the return value is #HAL_IRTX_STATUS_OK, the operation completed successfully.
 *                If the return value is #HAL_IRTX_STATUS_ERROR, the operation failed.
 * @par       Example
 * Sample code, please refer to @ref HAL_IRTX_Driver_Usage_Chapter.
 */

hal_irtx_status_t hal_irtx_register_pulse_data_callback(hal_irtx_pulse_data_callback_t callback,
        void *user_data);

/**
 * @brief    This function sends the data in the PWD encoded format.
 * @param[in] base_period  the base period for sending data.
 * @param[in] data  the data content to be sent.
 * @param[in] length data length in bytes.
 * @return   Indicates whether this function call is successful or not.
 *                If the return value is #HAL_IRTX_STATUS_OK, the operation completed successfully.
 *                If the return value is #HAL_IRTX_STATUS_ERROR, the operation failed.
 * @par       Example
 * Sample code, please refer to @ref HAL_IRTX_Driver_Usage_Chapter.
 */

hal_irtx_status_t hal_irtx_send_pulse_data(uint32_t base_period, uint8_t *data, uint32_t length);


/**
 * @brief    This function gets the current state of the IRTX.
 * @param[out] running_status is the current running status.
 *             #HAL_IRTX_BUSY means the IRTX is in busy state.
 *             #HAL_IRTX_IDLE means the IRTX is in idle state, which can be used to transfer data immediately.
 * @return   Indicates whether this function call is successful or not.
 *                If the return value is #HAL_IRTX_STATUS_OK, the operation completed successfully.
 *                If the return value is #HAL_IRTX_STATUS_ERROR, the operation failed.
 * @par       Example
 * Sample code, please refer to @ref HAL_IRTX_Driver_Usage_Chapter.
 */

hal_irtx_status_t hal_irtx_get_running_status(hal_irtx_running_status_t *running_status);



#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
*/

#endif /*HAL_IRTX_MODULE_ENABLED*/
#endif /* __HAL_IRTX_H__ */

