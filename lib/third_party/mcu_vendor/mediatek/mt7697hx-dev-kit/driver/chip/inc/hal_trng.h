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

#ifndef __HAL_TRNG_H__
#define __HAL_TRNG_H__

#include "hal_platform.h"
#ifdef HAL_TRNG_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup TRNG
 * @{
 * This section introduces the True Randam Number Generator(TRNG) APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, TRNG function groups, enums, structures and functions.
 *
 * @section HAL_TRNG_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b TRNG                |True Randam Number Generator. TRNG is a hardware generator to generate random data for applications. |

 * @section HAL_TRNG_Driver_Usage_Chapter  How to use this driver
 *
 * - Trigger TRNG to generate a random number. \n
 *  - Step1: Call hal_trng_init() to initialize the TRNG clock.
 *  - Step2: Call hal_trng_get_generated_random_number() to generate random number.
 *  - Step3: Call hal_trng_deinit() to de-initialize the TRNG.
 *  - Sample code:
 *  @code
 *       uint32_t random_number;
 *
 *       //Initializes the TRNG source clock.
 *       if(HAL_TRNG_STATUS_OK != hal_trng_init()) {
 *             //error handle
 *       }
 *       //Gets the random number.
 *       if(HAL_TRNG_STATUS_OK != hal_trng_get_generated_random_number(&random_number)) {
 *             //error handle
 *       }
 *       // random_number is ready to use.
 *       hal_trng_deinit();   // Deinitializes the TRNG.
 *
 *  @endcode
 */



#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup hal_trng_enum Enum
  * @{
  */


/** @brief This enum defines the API return type.  */
typedef enum {
    HAL_TRNG_STATUS_ERROR                = -2,         /**< An error occurred during the function call. */
    HAL_TRNG_STATUS_INVALID_PARAMETER    = -1,         /**< A wrong parameter is given. */
    HAL_TRNG_STATUS_OK                   =  0          /**< No error occurred during the function call. */
} hal_trng_status_t;


/**
  * @}
  */



/**
 * @brief     This function initializes the TRNG hardware clock.
 * @return   Indicates whether this function call is successful or not.
 *               If the return value is #HAL_TRNG_STATUS_OK, the operation completed successfully.
 * @par       Example
 * Sample code, please refer to @ref HAL_TRNG_Driver_Usage_Chapter.
 * @sa  hal_trng_deinit()
 */

hal_trng_status_t hal_trng_init(void);



/**
 * @brief     This function de-initializes the TRNG hardware clock.
 * @return   Indicates whether this function call is successful or not.
 *               If the return value is #HAL_TRNG_STATUS_OK, the operation completed successfully.
 * @par       Example
 * Sample code, please refer to @ref HAL_TRNG_Driver_Usage_Chapter.
 * @sa  hal_trng_init()
 */

hal_trng_status_t hal_trng_deinit(void);



/**
 * @brief     This function gets the random number generated by the TRNG.
 * @param[out] random_number is the TRNG hardware generated random number.
 * @return   Indicates whether this function call is successful or not.
 *               If the return value is #HAL_TRNG_STATUS_OK, the operation completed successfully.
 * @par       Example
 * Sample code, please refer to @ref HAL_TRNG_Driver_Usage_Chapter.
 * @sa  hal_trng_init(),hal_trng_deinit().
 */


hal_trng_status_t hal_trng_get_generated_random_number(uint32_t *random_number);


#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/

#endif /*HAL_TRNG_MODULE_ENABLED*/
#endif /* __HAL_TRNG_H__ */


