/*
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 */

/**
 * @file aws_iot_atomic.h
 * @brief FreeRTOS atomic operations with disabling interrupts globally.
 */

#ifndef _AWS_IOT_ATOMIC_H_
#define _AWS_IOT_ATOMIC_H_

/* Standard includes. */
#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/**
 * @brief Pseudo atomic add.
 *
 * @param[in] pTarget   Pointer to memory location from where value is to be loaded and checked.
 * @param[in] value     Value to be added to *pTarget.
 *
 * @return calculated result.
 *
 * @note The exact behavior of this function may be port dependent.
 * https://www.freertos.org/taskENTER_CRITICAL_taskEXIT_CRITICAL.html
 */

int32_t AwsIotAtomic_Add( int32_t volatile * pTarget, int32_t value )
{
    int32_t current;

    portENTER_CRITICAL( );

    current = *pTarget;

    *pTarget += value;

    portEXIT_CRITICAL( );

    return current;
}

/*-----------------------------------------------------------*/

/**
 * @brief Pseudo atomic subtraction.
 *
 * @param[in] pTarget   Pointer to memory location from where value is to be loaded and checked.
 * @param[in] value     Value to be subtract from *pTarget.
 *
 * @return calculated result.
 *
 * @note The exact behavior of this function may be port dependent.
 * https://www.freertos.org/taskENTER_CRITICAL_taskEXIT_CRITICAL.html
 */
int32_t AwsIotAtomic_Sub( int32_t volatile * pTarget, int32_t value )
{
    int32_t current;

    portENTER_CRITICAL( );

    current = *pTarget;

    *pTarget -= value;

    portEXIT_CRITICAL( );

    return current;
}

/*-----------------------------------------------------------*/

/**
 * @brief Pseudo atomic swap.
 *
 * @param[in] pTarget   Pointer to memory location from where value is to be loaded and checked.
 * @param[in] value     Value to be written to *pTarget.
 *
 * @return value
 *
 * @note The exact behavior of this function may be port dependent.
 * https://www.freertos.org/taskENTER_CRITICAL_taskEXIT_CRITICAL.html
 */

int32_t AwsIotAtomic_Swap( int32_t volatile * pTarget, int32_t value )
{
    int32_t current;

    portENTER_CRITICAL( );

    current = *pTarget;

    *pTarget = value;

    portEXIT_CRITICAL( );

    return current;
}

/*-----------------------------------------------------------*/
/**
 * @brief Pseudo atomic compare and swap.
 *
 * Disabling interrupt before writing new value to the designated memory location.
 *
 * @param[in] pTarget   Pointer to memory location from where value is to be loaded and checked.
 * @param[in] value     If condition meets, write this value to memory.
 * @param[in] comparand Swap condition, checks and waits for *pTarget to be equal to comparand.
 *
 * @returns value.
 *
 * @note The exact behavior of this function may be port dependent.
 * https://www.freertos.org/taskENTER_CRITICAL_taskEXIT_CRITICAL.html
 */

int32_t AwsIotAtomic_CompareAndSwap( int32_t volatile * pTarget, int32_t value, int32_t comparand )
{
    int32_t current;

    portENTER_CRITICAL( );

    current = *pTarget;

    if ( *pTarget == comparand )
    {
        *pTarget = value;
    }

    portEXIT_CRITICAL( );

    return current;
}

#endif /* _AWS_IOT_ATOMIC_H_ */
