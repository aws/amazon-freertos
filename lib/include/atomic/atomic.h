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
 * @brief FreeRTOS atomic operations with disabling interrupts globally.
 */

#ifndef ATOMIC_H
#define ATOMIC_H

/* Standard includes. */
#include <stdint.h>

/* Check which atomic implementation to use. */
#include "FreeRTOSConfig.h"

#if defined ( configUSE_ATOMIC_INSTRUCTION ) && ( configUSE_ATOMIC_INSTRUCTION == 1 )

    #include <stdbool.h>

    #define portFORCE_INLINE  inline __attribute__((always_inline))

#else

    /* FreeRTOS includes. */
    #include "FreeRTOS.h"
    #include "portmacro.h"

    /* Port specific definitions --
     * how to enter/exit critical section from atomic.
     * Refer template -- ./lib/FreeRTOS/portable/Compiler/Arch/portmacro.h
     */
    #if defined( portSET_INTERRUPT_MASK_FROM_ISR )
        /* Nested interrupt scheme is supported in this port. */
        #define ATOMIC_ENTER_CRITICAL()     portSET_INTERRUPT_MASK_FROM_ISR()
        #define ATOMIC_EXIT_CRITICAL( x )   portCLEAR_INTERRUPT_MASK_FROM_ISR( x )
        typedef UBaseType_t CriticalSessionType_t;
    #else
        #define ATOMIC_ENTER_CRITICAL()     portENTER_CRITICAL()
        #define ATOMIC_EXIT_CRITICAL( x )   portEXIT_CRITICAL()
        typedef void CriticalSessionType_t;
    #endif /* portSET_INTERRUPT_MASK_FROM_ISR() */

    /* Port specific definitions --
     * how to ensure "always inline".
     * portFORCE_INLINE is used. If this is not defined, add the definition to the corresponding portmacro.h.
     */

#endif /* configUSE_ATOMIC_INSTRUCTION */

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
 * @warning This function does NOT guarantee to swap *pDestination with ulExchange upon exit.
 *          It's up to caller to check the return value, and decide whether to retry.
 */
static portFORCE_INLINE uint32_t Atomic_CompareAndSwap_u32( uint32_t volatile * pDestination, uint32_t ulExchange, uint32_t ulComparand )
{
    uint32_t ulPreValue;

#if defined ( configUSE_ATOMIC_INSTRUCTION ) && ( configUSE_ATOMIC_INSTRUCTION == 1 )

    do
    {
        ulPreValue = *pDestination;
    }
    while (false == __atomic_compare_exchange(pDestination, &ulComparand, &ulExchange, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));

#else

    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    ulPreValue = *pDestination;

    if ( *pDestination == ulComparand )
    {
        *pDestination = ulExchange;
    }

    ATOMIC_EXIT_CRITICAL( temp );

#endif

    return ulPreValue;
}

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
 */
static portFORCE_INLINE void * Atomic_SwapPointers_p32( void * volatile * ppDestination, void * pExchange )
{
    void * pReturnValue;

#if defined ( configUSE_ATOMIC_INSTRUCTION ) && ( configUSE_ATOMIC_INSTRUCTION == 1 )

    __atomic_exchange( ppDestination, &pExchange, &pReturnValue, __ATOMIC_SEQ_CST );

#else

    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    pReturnValue = *ppDestination;

    *ppDestination = pExchange;

    ATOMIC_EXIT_CRITICAL( temp );

#endif

    return pReturnValue;
}

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
 * @note This function does NOT guarantee to swap *ppDestination with *pExchange upon exit.
 *       It's up to caller to check the return value, and decide whether to retry.
 */
static portFORCE_INLINE void * Atomic_CompareAndSwapPointers_p32( void * volatile * ppDestination, void * pExchange, void * pComparand )
{
    void * pPrevValue;

#if defined ( configUSE_ATOMIC_INSTRUCTION ) && ( configUSE_ATOMIC_INSTRUCTION == 1 )

    do
    {
        pPrevValue = *ppDestination;
    }
    while( false == __atomic_compare_exchange( ppDestination, &pComparand, &pExchange, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST ) );

#else

    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    pPrevValue = *ppDestination;

    if ( *ppDestination == pComparand )
    {
        *ppDestination = pExchange;
    }

    ATOMIC_EXIT_CRITICAL( temp );

#endif

    return pPrevValue;
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
static portFORCE_INLINE int32_t Atomic_Add_i32( int32_t volatile * pAddend, int32_t lCount )
{
#if defined ( configUSE_ATOMIC_INSTRUCTION ) && ( configUSE_ATOMIC_INSTRUCTION == 1 )

    return __atomic_fetch_add(pAddend, lCount, __ATOMIC_SEQ_CST);

#else

    int32_t lCurrent;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    lCurrent = *pAddend;

    *pAddend += lCount;

    ATOMIC_EXIT_CRITICAL( temp );

    return lCurrent;

#endif
}

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
static portFORCE_INLINE int32_t Atomic_Subtract_i32( int32_t volatile * pAddend, int32_t lCount )
{
#if defined ( configUSE_ATOMIC_INSTRUCTION ) && ( configUSE_ATOMIC_INSTRUCTION == 1 )

    return __atomic_fetch_sub(pAddend, lCount, __ATOMIC_SEQ_CST);

#else

    int32_t lCurrent;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL();

    lCurrent = *pAddend;

    *pAddend -= lCount;

    ATOMIC_EXIT_CRITICAL( temp );

    return lCurrent;

#endif
}

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
static portFORCE_INLINE int32_t Atomic_Increment_i32( int32_t volatile * pAddend )
{
#if defined ( configUSE_ATOMIC_INSTRUCTION ) && ( configUSE_ATOMIC_INSTRUCTION == 1 )

    return __atomic_fetch_add(pAddend, 1, __ATOMIC_SEQ_CST);

#else

    int32_t lCurrent;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    lCurrent = *pAddend;

    *pAddend += 1;

    ATOMIC_EXIT_CRITICAL( temp );

    return lCurrent;

#endif
}

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
static portFORCE_INLINE int32_t Atomic_Decrement_i32( int32_t volatile * pAddend )
{
#if defined ( configUSE_ATOMIC_INSTRUCTION ) && ( configUSE_ATOMIC_INSTRUCTION == 1 )

    return __atomic_fetch_sub(pAddend, 1, __ATOMIC_SEQ_CST);

#else

    int32_t lCurrent;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    lCurrent = *pAddend;

    *pAddend -= 1;

    ATOMIC_EXIT_CRITICAL( temp );

    return lCurrent;

#endif
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
static portFORCE_INLINE uint32_t Atomic_OR_u32( uint32_t volatile * pDestination, uint32_t ulValue )
{
#if defined ( configUSE_ATOMIC_INSTRUCTION ) && ( configUSE_ATOMIC_INSTRUCTION == 1 )

    return __atomic_fetch_or(pDestination, ulValue, __ATOMIC_SEQ_CST);

#else

    uint32_t ulCurrent;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    ulCurrent = *pDestination;

    *pDestination |= ulValue;

    ATOMIC_EXIT_CRITICAL( temp );

    return ulCurrent;

#endif
}

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
static portFORCE_INLINE uint32_t Atomic_AND_u32( uint32_t volatile * pDestination, uint32_t ulValue )
{
#if defined ( configUSE_ATOMIC_INSTRUCTION ) && ( configUSE_ATOMIC_INSTRUCTION == 1 )

    return __atomic_fetch_and(pDestination, ulValue, __ATOMIC_SEQ_CST);

#else

    uint32_t ulCurrent;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    ulCurrent = *pDestination;

    *pDestination &= ulValue;

    ATOMIC_EXIT_CRITICAL( temp );

    return ulCurrent;

#endif
}

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
static portFORCE_INLINE uint32_t Atomic_NAND_u32( uint32_t volatile * pDestination, uint32_t ulValue )
{
#if defined ( configUSE_ATOMIC_INSTRUCTION ) && ( configUSE_ATOMIC_INSTRUCTION == 1 )

    return __atomic_fetch_nand(pDestination, ulValue, __ATOMIC_SEQ_CST);

#else

    uint32_t ulCurrent;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    ulCurrent = *pDestination;

    *pDestination = ~(ulCurrent & ulValue);

    ATOMIC_EXIT_CRITICAL( temp );

    return ulCurrent;

#endif
}

/**
 * Atomic XOR
 * @brief Performs an atomic XOR operation on the specified values.
 *
 * @param [in, out] pDestination Pointer to memory location from where value is to be loaded and written back to.
 * @param [in] ulValue           Value to be XORed with *pDestination.
 *
 * @return The original value of *pDestination.
 */
static portFORCE_INLINE uint32_t Atomic_XOR_u32( uint32_t volatile * pDestination, uint32_t ulValue )
{
#if defined ( configUSE_ATOMIC_INSTRUCTION ) && ( configUSE_ATOMIC_INSTRUCTION == 1 )

    return __atomic_fetch_xor(pDestination, ulValue, __ATOMIC_SEQ_CST);

#else

    uint32_t ulCurrent;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    ulCurrent = *pDestination;

    *pDestination ^= ulValue;

    ATOMIC_EXIT_CRITICAL( temp );

    return ulCurrent;

#endif
}

#endif /* ATOMIC_H */
