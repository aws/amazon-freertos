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
 * @file atomic.h
 * @brief FreeRTOS atomic operation support.
 *
 * Two implementations of atomic are given in this header file:
 * 1. Disabling interrupt globally.
 * 2. ISA native atomic support.
 * The former is available to all ports (compiler-architecture combination),
 * while the latter is only available to ports compiling with GCC compiler.
 *
 * User can select with implementation to use by:
 * including FreeRTOSConfig.h before atomic.h,
 * and then setting/clearing configUSE_ATOMIC_INSTRUCTION in FreeRTOSConfig.h.
 *
 * Define AND set configUSE_ATOMIC_INSTRUCTION to 1 for ISA native atomic support.
 * Undefine OR clear configUSE_ATOMIC_INSTRUCTION for disabling global interrupt implementation.
 */

#ifndef ATOMIC_H
#define ATOMIC_H

#ifndef INC_FREERTOS_H
    #error "include FreeRTOS.h must appear in source files before include atomic.h"
#endif

#ifndef PORTMACRO_H
    #error "include portmacro.h must appear in source files before include atomic.h"
#endif

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined ( configUSE_ATOMIC_INSTRUCTION ) && ( configUSE_ATOMIC_INSTRUCTION == 1 )

    /* This branch is for GCC compiler and GCC compiler only. */
    #ifndef portFORCE_INLINE
        #define portFORCE_INLINE  inline __attribute__((always_inline))
    #endif

#else

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
 * @return bool True for swapped, false for not swapped.
 *
 * @note This function only swaps *pDestination with ulExchange, if previous *pDestination value equals ulComparand.
 */
static portFORCE_INLINE bool Atomic_CompareAndSwap_u32( uint32_t volatile * pDestination, uint32_t ulExchange, uint32_t ulComparand )
{

#if defined ( configUSE_ATOMIC_INSTRUCTION ) && ( configUSE_ATOMIC_INSTRUCTION == 1 )
    return __atomic_compare_exchange( pDestination, &ulComparand, &ulExchange, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST );

#else

    bool bExecutionStatus = false;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    if ( *pDestination == ulComparand )
    {
        *pDestination = ulExchange;
        bExecutionStatus = true;
    }

    ATOMIC_EXIT_CRITICAL( temp );

    return bExecutionStatus;

#endif

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
 * @return bool True for swapped, false for not swapped.
 *
 * @note This function only swaps *ppDestination with pExchange, if previous *ppDestination value equals pComparand.
 */
static portFORCE_INLINE bool Atomic_CompareAndSwapPointers_p32( void * volatile * ppDestination, void * pExchange, void * pComparand )
{

#if defined ( configUSE_ATOMIC_INSTRUCTION ) && ( configUSE_ATOMIC_INSTRUCTION == 1 )
    return __atomic_compare_exchange( ppDestination, &pComparand, &pExchange, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST );

#else

    bool bExecutionStatus = false;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    if ( *ppDestination == pComparand )
    {
        *ppDestination = pExchange;
        bExecutionStatus = true;
    }

    ATOMIC_EXIT_CRITICAL( temp );
    return bExecutionStatus;

#endif

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

#ifdef __cplusplus
}
#endif

#endif /* ATOMIC_H */
