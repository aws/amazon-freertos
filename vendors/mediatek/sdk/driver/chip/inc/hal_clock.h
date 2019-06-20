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

#ifndef __HAL_CLOCK_H__
#define __HAL_CLOCK_H__
#include "hal_platform.h"

#ifdef HAL_CLOCK_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup CLOCK
 * @{
 * This section introduces the Clock APIs including terms and acronyms, Clock function groups, enums, structures and functions. The Clock Manager provides APIs to manage the clocks and the Clock Gating (CG), such as enabling and disabling the clock through the CG and querying the clock status.
 *
 * @section HAL_CLOCK_Terms_Chapter Terms and acronyms
 *
 * The following provides descriptions of the terms commonly used in the Clock driver.
 *
 * |Terms               |Details                                                                 |
 * |--------------------|------------------------------------------------------------------------|
 * |\b CG               | Clock gating, applied to disable the clock.|
 *
 * @section HAL_CLOCK_Driver_Usage_Chapter How to use this driver
 *
 * Call #hal_clock_init() to initialize the clock only once before using any Clock API, otherwise the return status of the functions #hal_clock_enable() and #hal_clock_disable() will be #HAL_CLOCK_STATUS_UNINITIALIZED.
 *
 * The #hal_clock_enable() and #hal_clock_disable() APIs contain a reference counter that counts the number of times the API is called to support multiple users.
 *
 * The Clock API supports sharing a single clock with multiple users.
 *
 * Consider the following suggestions when calling #hal_clock_enable() and #hal_clock_disable():
 *
 * -# Each clock has its own counter for reference counting. The default value is zero.
 * -# For each call of the #hal_clock_enable() API, the corresponding reference counter increases by 1.
 *  - Only when the counter value changes from 0 to 1, the corresponding register value is modified and the clock is enabled.
 *  - In all other cases, calling #hal_clock_enable() only increases the counter value.
 * -# When #hal_clock_disable() is called, the corresponding reference counter decreases by 1.
 *  - Only when the counter value changes from 1 to 0, the corresponding register value is modified and the clock is disabled.
 *  - In all other cases, calling #hal_clock_disable() decreases the counter value.
 * -# The #hal_clock_enable() and #hal_clock_disable() APIs should be used in pair. Otherwise the clock might not turn off and will result in more power consumption.
 * -# Use #hal_clock_is_enabled() to check and verify whether the corresponding clock is enabled or disabled.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
* Enum
*****************************************************************************/
/** @defgroup hal_clock_enum Enum
  * @{
  */
/** @brief This enum defines the return status of Clock APIs. User should check the return value after calling the APIs. */
typedef enum {
    HAL_CLOCK_STATUS_UNINITIALIZED = -3,        /**< Uninitialized clock driver. */
    HAL_CLOCK_STATUS_INVALID_PARAMETER = -2,    /**< Invalid parameter. */
    HAL_CLOCK_STATUS_ERROR = -1,                /**< Unknown error. */
    HAL_CLOCK_STATUS_OK = 0,                    /**< Successful. */
} hal_clock_status_t;

/**
  * @}
  */

/*****************************************************************************
* Extern global functions
*****************************************************************************/

/**
 * @brief       This function initializes the clock driver and CG before using any Clock API.
 * @return      #HAL_CLOCK_STATUS_OK, if the operation completed successfully.\n
 *              #HAL_CLOCK_STATUS_UNINITIALIZED, if the clock driver is not initialized.\n
 *              #HAL_CLOCK_STATUS_INVALID_PARAMETER, if the input parameter is invalid.\n
 *              #HAL_CLOCK_STATUS_ERROR, if the clock function detected a common error.\n
 */
hal_clock_status_t hal_clock_init(void);

/**
 * @brief       This function enables a specific CG clock.
 * @param[in]   clock_id is a unique clock identifier.
 * @return      #HAL_CLOCK_STATUS_OK, if the operation completed successfully.\n
 *              #HAL_CLOCK_STATUS_UNINITIALIZED, if the clock driver is not initialized.\n
 *              #HAL_CLOCK_STATUS_INVALID_PARAMETER, if the input parameter is invalid.\n
 *              #HAL_CLOCK_STATUS_ERROR, if the clock function detected a common error.\n
 */
hal_clock_status_t hal_clock_enable(hal_clock_cg_id clock_id);

/**
 * @brief       This function disables a specific CG clock.
 * @param[in]   clock_id is a unique clock identifier.
 * @return      #HAL_CLOCK_STATUS_OK, if the operation completed successfully.\n
 *              #HAL_CLOCK_STATUS_UNINITIALIZED, if the clock driver is not initialized.\n
 *              #HAL_CLOCK_STATUS_INVALID_PARAMETER, if the input parameter is invalid.\n
 *              #HAL_CLOCK_STATUS_ERROR, if the clock function detected a common error.\n
 */
hal_clock_status_t hal_clock_disable(hal_clock_cg_id clock_id);

/**
 * @brief       This function queries the status of a specific CG clock.
 * @param[in]   clock_id is a unique clock identifier.
 * @return      true, if the specified clock is enabled.\n
 *              false, if the specified clock is disabled or the clock driver is not initialized.\n
 */
bool hal_clock_is_enabled(hal_clock_cg_id clock_id);

/**
 * @brief       This function gets the MCU clock frequency.
 * @return      the MCU clock frequency in Hz.\n
 */
uint32_t hal_clock_get_mcu_clock_frequency(void);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
 */

#endif /* HAL_CLOCK_MODULE_ENABLED */
#endif /* __HAL_CLOCK_H__ */

