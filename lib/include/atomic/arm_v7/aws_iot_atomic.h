/*
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @brief FreeRTOS atomic operations on ARM-V7 ports. 
 */

#ifndef _AWS_IOT_ATOMIC_H_
#define _AWS_IOT_ATOMIC_H_

/* Standard includes. */
#include <stdint.h>

/*----------------------------- Swap && CAS ------------------------------*/

/**
 * Atomic compare-and-swap
 *
 * @brief Performs an atomic compare-and-swap operation on the specified values.
 *
 * @param[in, out] pDestination  Pointer to memory location from where value is to be loaded and checked.
 * @param[in] ulExchange         If condition meets, write this value to memory.
 * @param[in] ulComparand        Swap condition, checks and waits for *pDestination to be equal to ulComparand.
 *
 * @return The initial value of *pDestination.
 *
 * @note This function guarantees to swap *pDestination with ulExchange upon exit.
 */
static inline __attribute__((always_inline)) uint32_t IotAtomic_CompareSwap_32( uint32_t volatile * pDestination, uint32_t ulExchange, uint32_t ulComparand )
{
    uint32_t current, result;

    __asm__ __volatile__(
            "1:     mov     %[result], #0\n"                    /* Clear [result] */
            "       clrex\n"                                    /* Clear exclusive access status. */
            "       ldrex   %[current], [%[mem]]\n"             /* Load *pDestination value into [current]. Exclusive access starts. */
            "       teq     %[current], %[comparand]\n"         /* Compare and set zero flag upon equal. */
            "       strexeq %[result], %[value], [%[mem]]\n"    /* Succeed to write back into *pDestination with result=0, or fail with result=1. Exclusive access ends. */
            "       clrex\n"                                    /* Clear exclusive access status. */
            "       teq     %[result], #0\n"                    /* Check whether exclusive write back succeeded. */
            "       bne     1b\n"                               /* If exclusive write back failed, start over. */
            : [result] "=&r" (result), [current] "=&r" (current)
            : [mem] "r" (pDestination), [value] "r" (ulExchange), [comparand] "r" (ulComparand)
            : "memory");                                        /* "memory" prevents compiler optimization. */

    return current;
}

/*-----------------------------------------------------------*/

/**
 * Atomic swap (pointers)
 *
 * @brief Atomically sets the address pointed to by *ppDestination to the value of *pExchange.
 *
 * @param[in, out] ppDestination  Pointer to memory location from where a pointer value is to be loaded and writen back to.
 * @param[in] pExchange           Pointer value to be written to *ppDestination.
 *
 * @return The initial value of *ppDestination.
 *
 * @note This function guarantees to swap *ppDestination with *pExchange upon exit.
 *
 * @todo To be implemented.
 */
static inline __attribute__((always_inline)) void * IotAtomic_SwapPointers_32( void * volatile * ppDestination, void * pExchange )
{
    // @todo
    return NULL;
}

/*-----------------------------------------------------------*/

/**
 * Atomic compare-and-swap (pointers)
 *
 * @brief Performs an atomic compare-and-swap operation on the specified pointer values.
 *
 * @param[in, out] ppDestination Pointer to memory location from where a pointer value is to be loaded and checked.
 * @param[in] pExchange          If condition meets, write this value to memory.
 * @param[in] pComparand         Swap condition, checks and waits for *ppDestination to be equal to *pComparand.
 *
 * @return The initial value of *ppDestination.
 *
 * @note This function guarantees to swap *ppDestination with *pExchange upon exit.
 *
 * @todo To be implemented.
 */
static inline __attribute__((always_inline)) void * IotAtomic_CompareSwapPointers_32( void * volatile * ppDestination, void * pExchange, void * pComparand )
{
    // @todo
    return NULL;
}

/*----------------------------- Arithmetic ------------------------------*/
/**
 * Atomic add
 *
 * @brief Adds count to the value of the specified 32-bit variable as an atomic operation.
 *
 * @param[in,out] pAddend  Pointer to memory location from where value is to be loaded and written back to.
 * @param[in] lCount       Value to be added to *pAddend.
 *
 * @return previous *pAddend value.
 *
 * @note This function guarantees to add value to *pAddend upon exit.
 */
static inline __attribute__((always_inline)) int32_t IotAtomic_Add_32( int32_t volatile * pAddend, int32_t lCount )
{
    int32_t current, result, sum;

    __asm__ __volatile__(
            "1:     clrex\n"
            "       ldrex   %[current], [%[mem]]\n"
            "       add     %[sum], %[current], %[value]\n"
            "       strex   %[result], %[sum], [%[mem]]\n"
            "       clrex\n"
            "       teq     %[result], #0\n"
            "       bne     1b\n"
            : [result] "=&r" (result), [current] "=&r" (current), [sum] "=&r" (sum)
            : [mem] "r" (pAddend), [value] "r" (lCount)
            : "memory");

    return current;
}

/*-----------------------------------------------------------*/

/**
 * Atomic sub
 *
 * @brief Subtracts count from the value of the specified 32-bit variable as an atomic operation.
 *
 * @param[in,out] pAddend  Pointer to memory location from where value is to be loaded and written back to.
 * @param[in] lCount       Value to be subtract from *pAddend.
 *
 * @return previous *pAddend value.
 *
 * @note This function guarantees to subtract value from *pAddend upon exit.
 */
static inline __attribute__((always_inline)) int32_t IotAtomic_Subtract_32( int32_t volatile * pAddend, int32_t lCount )
{
    int32_t current, result, sum;

    __asm__ __volatile__(
            "1:     clrex\n"
            "       ldrex   %[current], [%[mem]]\n"
            "       sub     %[sum], %[current], %[value]\n"
            "       strex   %[result], %[sum], [%[mem]]\n"
            "       clrex\n"
            "       teq     %[result], #0\n"
            "       bne     1b\n"
            : [result] "=&r" (result), [current] "=&r" (current), [sum] "=&r" (sum)
            : [mem] "r" (pAddend), [value] "r" (lCount)
            : "memory");

    return current;
}

/*-----------------------------------------------------------*/

/**
 * Atomic increment
 *
 * @brief Increments the value of the specified 32-bit variable as an atomic operation.
 *
 * @param[in,out] pAddend  Pointer to memory location from where value is to be loaded and written back to.
 *
 * @return *pAddend value before increment.
 *
 * @note This function guarantees to subtract value from *pAddend upon exit.
 */
static inline __attribute__((always_inline)) int32_t IotAtomic_Increment_32( int32_t volatile * pAddend )
{
    int32_t current, result, sum;

    __asm__ __volatile__(
            "1:     clrex\n"
            "       ldrex   %[current], [%[mem]]\n"
            "       add     %[sum], %[current], %[value]\n"
            "       strex   %[result], %[sum], [%[mem]]\n"
            "       clrex\n"
            "       teq     %[result], #0\n"
            "       bne     1b\n"
            : [result] "=&r" (result), [current] "=&r" (current), [sum] "=&r" (sum)
            : [mem] "r" (pAddend), [value] "I" (1)
            : "memory");

    return current;
}

/*-----------------------------------------------------------*/

/**
 * Atomic decrement
 *
 * @brief Decrements the value of the specified 32-bit variable as an atomic operation.
 *
 * @param[in,out] pAddend  Pointer to memory location from where value is to be loaded and written back to.
 *
 * @return *pAddend value before decrement.
 *
 * @note This function guarantees to subtract value from *pAddend upon exit.
 */
static inline __attribute__((always_inline)) int32_t IotAtomic_Decrement_32( int32_t volatile * pAddend )
{
    int32_t current, result, sum;

    __asm__ __volatile__(
            "1:     clrex\n"
            "       ldrex   %[current], [%[mem]]\n"
            "       sub     %[sum], %[current], %[value]\n"
            "       strex   %[result], %[sum], [%[mem]]\n"
            "       clrex\n"
            "       teq     %[result], #0\n"
            "       bne     1b\n"
            : [result] "=&r" (result), [current] "=&r" (current), [sum] "=&r" (sum)
            : [mem] "r" (pAddend), [value] "I" (1)
            : "memory");

    return current;
}

/*----------------------------- Bitwise Logical ------------------------------*/

/**
 * Atomic OR
 *
 * @brief Performs an atomic OR operation on the specified values.
 *
 * @param [in, out] pDestination Pointer to memory location from where value is to be loaded and written back to.
 * @param [in] ulValue           Value to be ORed with *pDestination.
 *
 * @return The original value of *pDestination.
 */
static inline __attribute__((always_inline)) uint32_t IotAtomic_OR_32( uint32_t volatile * pDestination, uint32_t ulValue )
{
    uint32_t current, repeat, result;

    __asm__ __volatile__(
            "1:     clrex\n"
            "       ldrex   %[current], [%[mem]]\n"
            "       orr     %[result], %[current], %[value]\n"
            "       strex   %[repeat], %[result], [%[mem]]\n"
            "       clrex\n"
            "       teq     %[repeat], #0\n"
            "       bne     1b\n"
            : [repeat] "=&r" (repeat), [current] "=&r" (current), [result] "=&r" (result)
            : [mem] "r" (pDestination), [value] "r" (ulValue)
            : "memory");

    return current;
}

/*-----------------------------------------------------------*/

/**
 * Atomic AND
 *
 * @brief Performs an atomic AND operation on the specified values.
 *
 * @param [in, out] pDestination Pointer to memory location from where value is to be loaded and written back to.
 * @param [in] ulValue           Value to be ANDed with *pDestination.
 *
 * @return The original value of *pDestination.
 */
static inline __attribute__((always_inline)) uint32_t IotAtomic_AND_32( uint32_t volatile * pDestination, uint32_t ulValue )
{
    uint32_t current, repeat, result;

    __asm__ __volatile__(
            "1:     clrex\n"
            "       ldrex   %[current], [%[mem]]\n"
            "       and     %[result], %[current], %[value]\n"
            "       strex   %[repeat], %[result], [%[mem]]\n"
            "       clrex\n"
            "       teq     %[repeat], #0\n"
            "       bne     1b\n"
            : [repeat] "=&r" (repeat), [current] "=&r" (current), [result] "=&r" (result)
            : [mem] "r" (pDestination), [value] "r" (ulValue)
            : "memory");

    return current;
}

/*-----------------------------------------------------------*/

/**
 * Atomic NAND
 *
 * @brief Performs an atomic NAND operation on the specified values.
 *
 * @param [in, out] pDestination Pointer to memory location from where value is to be loaded and written back to.
 * @param [in] ulValue           Value to be NANDed with *pDestination.
 *
 * @return The original value of *pDestination.
 */
static inline __attribute__((always_inline)) uint32_t IotAtomic_NAND_32( uint32_t volatile * pDestination, uint32_t ulValue )
{
    uint32_t current, repeat, result;

    __asm__ __volatile__(
            "1:     clrex\n"
            "       ldrex   %[current], [%[mem]]\n"
            "       and     %[result], %[current], %[value]\n"
            "       xor     %[result], %[result], #0xffffffff\n"
            "       strex   %[repeat], %[result], [%[mem]]\n"
            "       clrex\n"
            "       teq     %[repeat], #0\n"
            "       bne     1b\n"
            : [repeat] "=&r" (repeat), [current] "=&r" (current), [result] "=&r" (result)
            : [mem] "r" (pDestination), [value] "r" (ulValue)
            : "memory");

    return current;
}

/*-----------------------------------------------------------*/

/**
 * Atomic XOR
 * @brief Performs an atomic XOR operation on the specified values.
 *
 * @param [in, out] pDestination Pointer to memory location from where value is to be loaded and written back to.
 * @param [in] ulValue           Value to be XORed with *pDestination.
 *
 * @return The original value of *pDestination.
 */
static inline __attribute__((always_inline)) uint32_t IotAtomic_XOR_32( uint32_t volatile * pDestination, uint32_t ulValue )
{
    uint32_t current, repeat, result;

    __asm__ __volatile__(
            "1:     clrex\n"
            "       ldrex   %[current], [%[mem]]\n"
            "       eor     %[result], %[current], %[value]\n"
            "       strex   %[repeat], %[result], [%[mem]]\n"
            "       clrex\n"
            "       teq     %[repeat], #0\n"
            "       bne     1b\n"
            : [repeat] "=&r" (repeat), [current] "=&r" (current), [result] "=&r" (result)
            : [mem] "r" (pDestination), [value] "r" (ulValue)
            : "memory");

        return current;
}

#endif /* _AWS_IOT_ATOMIC_H_ */
