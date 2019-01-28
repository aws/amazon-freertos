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
 * @brief FreeRTOS atomic operations.
 */

#ifndef _AWS_IOT_ATOMIC_H_
#define _AWS_IOT_ATOMIC_H_

/* Standard includes. */
#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

#define AWS_IOT_ATOMIC_NATIVE_IMPLEMENTATION    ( 1 )

#if ( AWS_IOT_ATOMIC_NATIVE_IMPLEMENTATION == 1 )

/*-----------------------------------------------------------*/

/**
 * @brief Atomic Compare and Swap
 *
 * Wait for value in memory location to be equal to comparand, and write new value to the location.
 *
 * @param[in] pTarget   Pointer to memory location from where value is to be loaded and checked.
 * @param[in] value     If condition meets, write this value to memory.
 * @param[in] comparand Swap condition, checks and waits for *pTarget to be equal to comparand.
 *
 * @returns previous *pTarget value.
 *
 * @note This function guarantees to add value to *pTarget upon exit.
 */
static inline int32_t AwsIotAtomic_CompareAndSwap( int32_t volatile * pTarget, int32_t value, int32_t comparand )
{
    int32_t current, result;

    asm volatile(
            "1:     mov     %[result], #0 \n"                   /* Clear [result] */
            "       ldrex   %[current], [%[mem]] \n"            /* Load *pTarget value into [current]. Exclusive access starts. */
            "       teq     %[current], %[comparand] \n"        /* Compare and set zero flag upon equal. */
            "       strexeq %[result], %[value], [%[mem]]\n"    /* Succeed to write back into *pTarget with result=0, or fail with res=1. Exclusive access ends. */
            "       teq     %[result], #0\n"                    /* Check whether exclusive write back succeeded. */
            "       bne     1b\n"                               /* If exclusive write back failed, start over. */
            : [result] "=&r" (result), [current] "=&r" (current)
            : [mem] "r" (pTarget), [value] "Ir" (value), [comparand] "r" (comparand)
            : "memory", "cc");              /* "memory" prevent compiler optimization; condition flag is modified. */

    return current;
}

/*-----------------------------------------------------------*/

/**
 * @brief Atomic add
 *
 * @param[out] pTarget  Pointer to memory location from where value is to be loaded and written back to.
 * @param[in] value     Value to be added to *pTarget.
 *
 * @return previous *pTarget value.
 *
 * @note This function guarantees to add value to *pTarget upon exit.
 */
static inline int32_t AwsIotAtomic_Add( int32_t volatile * pTarget, int32_t value )
{
    int32_t current, result, sum;

    asm volatile(
            "1:     ldrex   %[current], [%[mem]]\n"
            "       add     %[sum], %[current], %[value]\n"
            "       strex   %[result], %[sum], [%[mem]]\n"
            "       teq     %[result], #0\n"
            "       bne     1b\n"
            : [result] "=&r" (result), [current] "=&r" (current), [sum] "=&r" (sum), "+Qo" (*pTarget)
            : [mem] "r" (pTarget), [value] "Ir" (value)
            : "memory", "cc");

    return current;
}

/*-----------------------------------------------------------*/

/**
 * @brief Atomic subtract
 *
 * @param[in] pTarget   Pointer to memory location from where value is to be loaded and checked.
 * @param[in] value     Value to be subtract from *pTarget.
 *
 * @return previous *pTarget value.
 *
 * @note This function guarantees to subtract from *pTarget upon exit.
 */
static inline int32_t AwsIotAtomic_Sub( int32_t volatile * pTarget, int32_t value )
{
    int32_t current, result, sum;

        asm volatile(
                "1:     ldrex   %[current], [%[mem]]\n"
                "       sub     %[sum], %[current], %[value]\n"
                "       strex   %[result], %[sum], [%[mem]]\n"
                "       teq     %[result], #0\n"
                "       bne     1b\n"
                : [result] "=&r" (result), [current] "=&r" (current), [sum] "=&r" (sum), "+Qo" (*pTarget)
                : [mem] "r" (pTarget), [value] "Ir" (value)
                : "memory", "cc");

        return current;
}

/*-----------------------------------------------------------*/

/**
 * @brief Atomic swap.
 *
 * @param[in] pTarget   Pointer to memory location from where value is to be loaded and checked.
 * @param[in] value     Value to be written to *pTarget.
 *
 * @return previous *pTarget value.
 *
 * @note This function guarantees to write value to *pTarget.
 */

static inline int32_t AwsIotAtomic_Swap( int32_t volatile * pTarget, int32_t value )
{
    int32_t current, result;

        asm volatile(
                "1:     mov     %[result], #0 \n"
                "       ldrex   %[current], [%[mem]] \n"
                "       strex %[result], %[value], [%[mem]]\n"
                "       teq     %[result], #0\n"
                "       bne     1b\n"
                : [result] "=&r" (result), [current] "=&r" (current)
                : [mem] "r" (pTarget), [value] "Ir" (value)
                : "memory", "cc");

        return current;
}

#else

/*-----------------------------------------------------------*/

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

#endif /* AWS_IOT_ATOMIC_NATIVE_IMPLEMENTATION */


#endif /* _AWS_IOT_ATOMIC_H_ */
