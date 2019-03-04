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
 * @brief FreeRTOS atomic operations with IAR built-in intrinsics.
 * 
 * @note Users are encouraged to use atomic implementation with "disabling interrupts" 
 *       on ports compiling with IAR. For atomic implementation with IAR intrinsics, 
 *       known limitations/questions at this point:
 * 
 *       - Missing documentation from IAR about intrinsics we are calling. 
 *         To be specific,
 *          atomic_fetch_{add, sub, and, or, xor}(), 
 *          atomic_compare_exchange_strong(),
 *          atomic_exchange().
 *         pending: 
 *          Confirm stdatomic.h is good to use, from other than compiler itself.  
 * 
 *       - Atomic compare-and-swap and swap are not implemented, in the  
 *         absence of documentation for atomic types. e.g. When calling 
 *         atomic_compare_exchange_strong() from IotAtomic_CompareAndSwap_u32()
 *         what to cast (uint32_t *) to? (atomic_uint *), (atomic_uint_least32_t *)
 *         (atomic_int_fast32_t *), or (atomic_uintptr_t *)?
 * 
 *       - Atomic NAND cannot be implemented with given intrinsics. See @warning
 *         in IotAtomic_NAND_u32() API doc.
 * 
 * @refernece [IAR C/C++ Development Guide Compiling and Linking]
 *            (http://ftp.iar.se/WWWfiles/arm/webic/doc/EWARM_DevelopmentGuide.ENU.pdf) 
 *            section "Atomic Operations". 
 */

#ifndef ATOMIC_H
#define ATOMIC_H

/* IAR compiler include. */
#include <stdatomic.h>

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>

/*----------------------------- Swap && CAS ------------------------------*/

/**
 * Atomic compare-and-swap
 *
 * @brief Performs an atomic compare-and-swap operation on the specified values.
 *
 * @param[in, out] pDestination  Pointer to memory location from where value is to be loaded and checked.
 * @param[in] ulExchange     	 If condition meets, write this value to memory.
 * @param[in] ulComparand 	 Swap condition, checks and waits for *pDestination to be equal to ulComparand.
 *
 * @return The initial value of *pDestination.
 * 
 * @note This function guarantees to swap *pDestination with ulExchange upon exit.
 */
static inline __attribute__((always_inline)) uint32_t IotAtomic_CompareAndSwap_u32( uint32_t volatile * pDestination, uint32_t ulExchange, uint32_t ulComparand );

/**
 * Atomic swap (pointers)
 * 
 * @brief Atomically sets the address pointed to by *ppDestination to the value of *pExchange. 
 *
 * @param[in, out] ppDestination  Pointer to memory location from where a pointer value is to be loaded and writen back to.
 * @param[in] pExchange     	  Pointer value to be written to *ppDestination.
 * 
 * @return The initial value of *ppDestination.
 * 
 * @note This function guarantees to swap *ppDestination with *pExchange upon exit.
 */
static inline __attribute__((always_inline)) void * IotAtomic_SwapPointers_p32( void * volatile * ppDestination, void * pExchange );

/**
 * Atomic compare-and-swap (pointers)
 *
 * @brief Performs an atomic compare-and-swap operation on the specified pointer values. 
 * 
 * @param[in, out] ppDestination Pointer to memory location from where a pointer value is to be loaded and checked.
 * @param[in] pExchange     	 If condition meets, write this value to memory.
 * @param[in] pComparand 		 Swap condition, checks and waits for *ppDestination to be equal to *pComparand.
 *
 * @return The initial value of *ppDestination.
 * 
 * @note This function guarantees to swap *ppDestination with *pExchange upon exit.
 */
static inline __attribute__((always_inline)) void * IotAtomic_CompareAndSwapPointers_p32( void * volatile * ppDestination, void * pExchange, void * pComparand );

/*----------------------------- Arithmetic ------------------------------*/

/**
 * Atomic add
 *
 * @brief Adds count to the value of the specified 32-bit variable as an atomic operation.
 * 
 * @param[in,out] pAddend  Pointer to memory location from where value is to be loaded and written back to.
 * @param[in] lCount	   Value to be added to *pAddend.
 * 
 * @return previous *pAddend value.
 * 
 * @note This function guarantees to add value to *pAddend upon exit.
 */
static inline __attribute__((always_inline)) int32_t IotAtomic_Add_i32( int32_t volatile * pAddend, int32_t lCount )
{
  return atomic_fetch_add(pAddend, lCount);
}

/**
 * Atomic sub
 *
 * @brief Subtracts count from the value of the specified 32-bit variable as an atomic operation.
 *
 * @param[in,out] pAddend  Pointer to memory location from where value is to be loaded and written back to.
 * @param[in] lCount	   Value to be subtract from *pAddend.
 * 
 * @return previous *pAddend value.
 * 
 * @note This function guarantees to subtract value from *pAddend upon exit.
 */
static inline __attribute__((always_inline)) int32_t IotAtomic_Subtract_i32( int32_t volatile * pAddend, int32_t lCount )
{
  return atomic_fetch_sub(pAddend, lCount);
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
static inline __attribute__((always_inline)) int32_t IotAtomic_Increment_i32( int32_t volatile * pAddend )
{
  return atomic_fetch_add(pAddend, 1);
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
static inline __attribute__((always_inline)) int32_t IotAtomic_Decrement_i32( int32_t volatile * pAddend )
{
  return atomic_fetch_sub(pAddend, 1);
}

/*----------------------------- Bitwise Logical ------------------------------*/

/**
 * Atomic OR
 *
 * @brief Performs an atomic OR operation on the specified values.
 *
 * @param [in, out] pDestination Pointer to memory location from where value is to be loaded and written back to.
 * @param [in] ulValue			 Value to be ORed with *pDestination.
 *
 * @return The original value of *pDestination.
 */
static inline __attribute__((always_inline)) uint32_t IotAtomic_OR_u32( uint32_t volatile * pDestination, uint32_t ulValue )
{
  return atomic_fetch_or(pDestination, ulValue);
}

/**
 * Atomic AND
 *
 * @brief Performs an atomic AND operation on the specified values.
 *
 * @param [in, out] pDestination Pointer to memory location from where value is to be loaded and written back to.
 * @param [in] ulValue			 Value to be ANDed with *pDestination. 
 * 
 * @return The original value of *pDestination.
 */
static inline __attribute__((always_inline)) uint32_t IotAtomic_AND_u32( uint32_t volatile * pDestination, uint32_t ulValue )
{
  return atomic_fetch_and(pDestination, ulValue);
}

/**
 * Atomic NAND
 *
 * @brief Performs an atomic NAND operation on the specified values.
 * 
 * @param [in, out] pDestination Pointer to memory location from where value is to be loaded and written back to.
 * @param [in] ulValue			 Value to be NANDed with *pDestination. 
 *
 * @return The original value of *pDestination.
 * 
 * @warning Atomic NAND cannot be guaranteed to be correctly compiled with IAR intrinsics. (There is no atomic_fetch_nand.)
 *          options 1: !(A & B) = !A || !B
 *          options 2: !(A & B) = (A & B ) ^ 0xFFFFFFFF (For 32-bit)
 *          Both above cannot be done with given interface definitions.
 */
static inline __attribute__((always_inline)) uint32_t IotAtomic_NAND_u32( uint32_t volatile * pDestination, uint32_t ulValue );

/**
 * Atomic XOR
 * @brief Performs an atomic XOR operation on the specified values.
 *
 * @param [in, out] pDestination Pointer to memory location from where value is to be loaded and written back to.
 * @param [in] ulValue			 Value to be XORed with *pDestination. 
 *
 * @return The original value of *pDestination.
 */
static inline __attribute__((always_inline)) uint32_t IotAtomic_XOR_u32( uint32_t volatile * pDestination, uint32_t ulValue )
{
  return atomic_fetch_xor(pDestination, ulValue);
}

#endif /* ATOMIC_H */


