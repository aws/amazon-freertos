/*
 * FreeRTOS Common IO V0.1.3
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file    iot_reset.h
 * @brief   This file contains all the Reset HAL API definitions
 */

#ifndef _IOT_RESET_H_
#define _IOT_RESET_H_

#include <stdint.h>

/**
 * @defgroup iot_reset Reset HAL APIs
 * @{
 */

/**
 * Return values used by reset driver
 */
#define IOT_RESET_SUCCESS                   ( 0 )    /**< Reset operation completed successfully. */
#define IOT_RESET_FUNCTION_NOT_SUPPORTED    ( 1 )    /**< Reset function not supported. */
#define IOT_RESET_INVALID_VALUE             ( 2 )    /**< At least one parameter is invalid. */

typedef enum
{
    eResetPowerOnBoot, /**< Normal power on Boot, when the power is applied to the device. */
    eResetWarmBoot,    /**< Last reset triggered due to warm Reset. for ex: iot_reset_reboot(0) was called  to reset the device */
    eResetColdBoot,    /**< Last reset triggered due to cold Reset. for ex: iot_reset_reboot(1) */
    eResetWatchdog,    /**< Last reset triggered due to watchdog expiration */
    eResetBusTimeout,  /**< Last reset triggered due to internal bus timeout on the SoC */
    eResetPmic,        /**< Last reset caused by power management IC */
    eResetBrownOut,    /**< Last reset caused by power brown out */
    eResetOther,       /**< Last reset caused by other reasons specific to the underlying hardware */
} IotResetReason_t;

typedef enum
{
    eResetWarmBootFlag, /**< Perform warm-reset */
    eResetColdBootFlag, /**< Perform cold-reset */
} IotResetBootFlag_t;

/**
 * @brief    iot_reset_reboot is used to reboot the device.
 *
 * @param[in]   xResetBootFlag  flag to determine either to do cold-reset or warm-reset.
 *                              cold-reset means the device is restarted and does not keep
 *                              any blocks of the SOC powered on i.e. device is shutdown and rebooted),
 *                              and warm-reset means the device is restarted while keeping some of the SoC blocks
 *                              powered on through the reboot process.
 *                              For example warm-boot may keep the RAM contents valid after reset by keeping the power
 *                              on for RAM banks, while cold-boot will wipe off the contents.
 *                              One of the IotResetBootFlag_t value.
 */
void iot_reset_reboot( IotResetBootFlag_t xResetBootFlag );

/**
 * @brief   iot_reset_shutdown is used to shutdown the device.
 *          If the target does not support shutdown of the device, IOT_RESET_FUNCTION_NOT_SUPPORTED
 *          is returned to the user.
 *
 * @return
 *   - does not return and device shutdown on success
 *   - IOT_RESET_FUNCTION_NOT_SUPPORTED if shutdown not supported.
 */
int32_t iot_reset_shutdown( void );

/**
 * @brief   iot_get_reset_reason is used to get the last reset reason.
 *          If the underlying HW does not support the feature of persisting the
 *          reset reason, then this API will return IOT_RESET_FUNCTION_NOT_SUPPORTED
 *          error with the value in out as don't care.
 *
 * @param[out]   xResetReason  One of the reset reasons specified in IotResetReason_t types
 *
 * @return
 *   - IOT_RESET_SUCCESS on success.
 *   - IOT_RESET_FUNCTION_NOT_SUPPORTED if not supported.
 *   - IOT_RESET_INVALID_VALUE if xREsetReason == NULL
 */
int32_t iot_get_reset_reason( IotResetReason_t * xResetReason );

/**
 * @}
 */

#endif /* ifndef _IOT_RESET_H_ */
