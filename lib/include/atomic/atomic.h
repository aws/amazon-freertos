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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "portmacro.h"

/* Standard includes. */
#include <stdint.h>

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
 * portFORCE_INLINE is used. If this is not defined, add the definition to corresponding portmacro.h. 
 */

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
    uint32_t current;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    current = *pDestination;

    if ( *pDestination == ulComparand )
    {
        *pDestination = ulExchange;
    }

    ATOMIC_EXIT_CRITICAL( temp );

    return current;
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
    void * current;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    current = *ppDestination;

    *ppDestination = pExchange;

    ATOMIC_EXIT_CRITICAL( temp );

    return current;
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
    void * current;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    current = *ppDestination;

    if ( *ppDestination == pComparand )
    {
        *ppDestination = pExchange;
    }

    ATOMIC_EXIT_CRITICAL( temp );

    return current;
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
    int32_t current;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    current = *pAddend;

    *pAddend += lCount;

    ATOMIC_EXIT_CRITICAL( temp );

    return current;
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
    int32_t current;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL();

    current = *pAddend;

    *pAddend -= lCount;

    ATOMIC_EXIT_CRITICAL( temp );

    return current;
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
    int32_t current;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    current = *pAddend;

    *pAddend += 1;

    ATOMIC_EXIT_CRITICAL( temp );

    return current;
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
    int32_t current;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    current = *pAddend;

    *pAddend -= 1;

    ATOMIC_EXIT_CRITICAL( temp );

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
static portFORCE_INLINE uint32_t Atomic_OR_u32( uint32_t volatile * pDestination, uint32_t ulValue )
{
    uint32_t current;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    current = *pDestination;

    *pDestination |= ulValue;

    ATOMIC_EXIT_CRITICAL( temp );

    return current;
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
    uint32_t current;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    current = *pDestination;

    *pDestination &= ulValue;

    ATOMIC_EXIT_CRITICAL( temp );

    return current;
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
    uint32_t current;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    current = *pDestination;

    *pDestination = ~(current & ulValue);

    ATOMIC_EXIT_CRITICAL( temp );

    return current;
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
    uint32_t current;
    CriticalSessionType_t temp;

    temp = ATOMIC_ENTER_CRITICAL( );

    current = *pDestination;

    *pDestination ^= ulValue;

    ATOMIC_EXIT_CRITICAL( temp );

    return current;
}

#endif /* ATOMIC_H */
