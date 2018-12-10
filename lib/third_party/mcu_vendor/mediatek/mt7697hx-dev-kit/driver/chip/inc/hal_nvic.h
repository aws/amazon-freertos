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

#ifndef __HAL_NVIC_H__
#define __HAL_NVIC_H__
#include "hal_platform.h"

#ifdef HAL_NVIC_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup NVIC
 * @{
 * This section introduces the NVIC HAL APIs including terms and acronyms, details on how to use this driver, supported features, enums, structures and functions.
 * NVIC usage follows the CMSIS except it doesn't support direct registration of ISR in the vector table, but it uses
 * #hal_nvic_register_isr_handler function to improve software efficiency and debugging.
 *
 * @section HAL_NVIC_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------|------------------------------------------------------------------------|
 * |\b  NVIC                 | Nested Vectored Interrupt Controller. NVIC is the interrupt controller of ARM Cortex-M4. For more details, please refer to <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100166_0001_00_en/ric1417175922867.html"> NVIC introduction in ARM Cortex-M4 Processor Technical Reference Manual </a>.|
 * |\b ISR                  | Interrupt service routine.|
 * |\b IRQ                  | Interrupt request.|
 *
 *
 *
 * @section HAL_NVIC_Driver_Usage_Chapter How to use this driver
 *
 * - Use NVIC. \n
 *
 *  - Step1: Call #hal_nvic_init() function to initialize the NVIC IRQ priority according to the exception number. By default, the lower the exception number the higher is the priority.
 *  - Step2: Call #hal_nvic_register_isr_handler() function to register an ISR handler before using the NVIC IRQ.
 *  - Step3: Use CMSIS interface to adjust IRQ priority if needed.
 *  - Step4: Use NVIC_EnableIRQ() to enable the IRQ.
 *  - Sample code:
 *    @code
 *       //Define a callback (an ISR) function.
 *       void isr_handler(hal_nvic_irq_t irq_number)
 *       {
 *           //Do something.
 *       }
 *    @endcode
 *    @code
 *       ret = hal_nvic_init();    //Should be called only once at initialization.
 *       if (HAL_NVIC_STATUS_OK != ret) {
 *               //error handle;
 *       }
 *       ret = hal_nvic_register_isr_handler(irq_number, isr_handler);   //Register an IRQ handler.
 *       if (HAL_NVIC_STATUS_OK !=  ret) {
 *               //error handle;
 *       }
 *       NVIC_EnableIRQ(irq_number);
 *
 *       //Change the IRQ priority. It is optional.
 *       NVIC_DisableIRQ(irq_number);   //Disable IRQ number.
 *       NVIC_SetPriority(irq_number, DEFAULT_IRQ_PRIORITY);  //Change the priority of the IRQ number if needed.
 *       NVIC_EnableIRQ(irq_number);    //Enable the IRQ number.
 *
 *    @endcode
 *    @code
 *       void function_task(void)
 *       {
 *           //function_task only can be running at thread mode.
 *       }
 *
 *       void function_isr(void)
 *       {
 *           //function_isr only can be running at handler mode.
 *       }
 *
 *    @endcode
 *    @code
 *
 *       //It needs to take different actions according to the Cortex-M4 current exception status.
 *       if (HAL_NVIC_QUERY_EXCEPTION_NUMBER == 0) {
 *           //Cortex-M4 is in thread mode.
 *           function_task();
 *       } else {
 *           //Cortex-M4 is handling the exception, and exception number is HAL_NVIC_QUERY_EXCEPTION_NUMBER.
 *           function_isr();
 *       }
 *
 *    @endcode
 *    @code
 *       //Preserve the original IRQ settings.
 *       void example(void)
 *       {
 *           uint32_t mask;
 *           hal_nvic_save_and_set_interrupt_mask(&mask); //Save the current IRQ settings.
 *           //Do something with the protected critical resource when the IRQ is disabled.
 *           hal_nvic_restore_interrupt_mask(mask);       //Restore the IRQ settings.
 *       }
 *
 *       void test(void)
 *       {
 *           uint32_t mask;
 *           hal_nvic_save_and_set_interrupt_mask(&mask);   //Save the current IRQ settings.
 *           example();
 *           //The IRQ is still disabled;
 *           //Do something with the protected critical resouce when the IRQ is disabled.
 *           hal_nvic_restore_interrupt_mask(mask);  //Restore the original IRQ settings in the mask.
 *       }
 *    @endcode
 *
 */


#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup hal_nvic_enum Enums
  * @{
  */

/** @brief  This enum defines the return status of the NVIC API.  */
typedef enum {
    HAL_NVIC_STATUS_ERROR_NO_ISR = -4,              /**< ISR routine is null. */
    HAL_NVIC_STATUS_ERROR_IRQ_NUMBER  = -3,         /**< Incorrect IRQ number. */
    HAL_NVIC_STATUS_INVALID_PARAMETER = -2,         /**< Invalid parameter. */
    HAL_NVIC_STATUS_ERROR             = -1,         /**< NVIC unspecified error. */
    HAL_NVIC_STATUS_OK                = 0           /**< No error occurred. */
} hal_nvic_status_t;


/**
  * @}
  */


/** @defgroup hal_nvic_typedef Typedef
  * @{
  */
/** @brief  This defines the callback function prototype.
 *          User should register a callback (an ISR) for each used NVIC IRQ.
 *          The callback function is called in the NVIC ISR after the NVIC IRQ is triggered.
 *          For more details about the callback, please refer to #hal_nvic_register_isr_handler().
 *  @param [in] irq_number is given by driver to notify the current NVIC IRQ.
 */
typedef void (*hal_nvic_isr_t)(hal_nvic_irq_t irq_number);
/**
  * @}
  */


/** @defgroup hal_nvic_macro Define
  * @{
  */
/** This macro is used to query the current exception status of Cortext-M4.
    If the value is 0, the Cortext-M4 is running successfully, otherwise an exception occured
    and the exception number is the value of the macro. */
#define HAL_NVIC_QUERY_EXCEPTION_NUMBER ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) >> SCB_ICSR_VECTACTIVE_Pos)
/**
  * @}
  */


/*****************************************************************************
* Functions
*****************************************************************************/

/**
 * @brief This function is used to register a callback (an ISR) for NVIC IRQ.
 * @param[in] irq_number is the NVIC IRQ number.
 * @param[in] isr_handler is the NVIC IRQ's ISR.
 * @return    To indicate whether this function call is successful.
 *            If the return value is #HAL_NVIC_STATUS_OK,  the operation completed successfully;
 *            If the return value is #HAL_NVIC_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 */
hal_nvic_status_t hal_nvic_register_isr_handler(hal_nvic_irq_t irq_number, hal_nvic_isr_t isr_handler);


/**
 * @brief This function initializes the NVIC IRQ with default priority. By default the exception number is lower and the priority is higher.
 */
hal_nvic_status_t hal_nvic_init(void);


/**
 * @brief This function saves the current IRQ settings in an variable, and then disables the IRQ by setting the IRQ mask.
 *  It should be used in conjunction with #hal_nvic_restore_interrupt_mask() to protect the critical resources.
 * @param[out] mask is used to store the current IRQ setting, upon the return of this function.
 * @return    To indicate whether this function call is successful.
 *            If the return value is #HAL_NVIC_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_NVIC_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 */
hal_nvic_status_t hal_nvic_save_and_set_interrupt_mask(uint32_t *mask);

/**
 * @brief This function restores the IRQ settings as specified in the mask. It should be used in conjunction with
 *  #hal_nvic_save_and_set_interrupt_mask() to protect critical resources.
 * @param[in] mask is an unsigned integer to specify the IRQ settings.
 * @return    Indicates whether this function call is successful.
 *            If the return value is #HAL_NVIC_STATUS_OK, the operation completed successfully;
 *            If the return value is #HAL_NVIC_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 */
hal_nvic_status_t hal_nvic_restore_interrupt_mask(uint32_t mask);


#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/

#endif /*HAL_NVIC_MODULE_ENABLED*/
#endif /* __HAL_NVIC_H__ */

