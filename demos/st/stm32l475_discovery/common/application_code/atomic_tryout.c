/*
* Amazon FreeRTOS V1.x.x
* Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* http://aws.amazon.com/freertos
* http://www.FreeRTOS.org
*/

/*
 * @brief Call atomic interfaces defined in atomic.h and validate.
 *
 * @note Things to be validated in objdump asm:
 *       - inline with -O0
 *       - loop (make sure value of interested is refreshed in each iteration.)
 *       - instructions used between ports/compilers.
 *
 *       Things to be validated in C:
 *       - code does what it supposed to do. (e.g. if atomic add, we are adding at least.)
 *       - casting. (e.g. we are only supporting 32-bit. give some example. )
 */

#include "atomic.h"
#include "kernel_profiling.h"

/* For console print. */
#include "FreeRTOSConfig.h"

/* NULL */
#include <stdlib.h>

/* Magic numbers. */
#define MAGIC_NUMBER_32BIT_1    ( 0xA5A5A5A5 )
#define MAGIC_NUMBER_32BIT_2    ( 0xF0F0F0F0 )
#define MAGIC_NUMBER_32BIT_3    ( 0x0000000F )

#define MAGIC_NUMBER_8BIT_1     ( 0xA5 )
#define MAGIC_NUMBER_8BIT_2     ( 0xF0 )

#define MAGIC_NUMBER_INT8_1     ( 0xFF )    // -255
#define MAGIC_NUMBER_INT8_2     ( 0x7F )    // 256



void vKernelAtomicTryout_CAS_happy_path( void )
{

    uint32_t ulCasDestination_32;
    uint32_t ulCasComparator_32;
    uint32_t ulCasNewValue_32 ;
    uint32_t ulReturnValue;

    uint32_t *pSwapDestination_32;
    uint32_t *pSwapNewValue_32;
    uint32_t *pReturnValue_32;

    uint8_t uCasDestination_8 = MAGIC_NUMBER_8BIT_1;
    uint8_t uCasComparator_8 = MAGIC_NUMBER_8BIT_1;

    uint8_t *pSwapDestination_8 = &uCasDestination_8;
    uint8_t *pSwapNewValue_8 = &uCasComparator_8;
    uint8_t *pReturnValue_8 = NULL;

    /* asm (built-in function) implementation --
     * Check objdump -- in loop, value is reloaded each iteration, comparator is reloaded each iteration .
     * Check functionality -- swapped.
     *
     * disabling interrupt implementation --
     * no loop, make sure comparator and destination are using different registers.
     *
     * Below can be in test project, but unnecessary.
     * And objdump asm needs to be checked for each port, manually. */

    // #1 -- CAS
    ulCasDestination_32 = MAGIC_NUMBER_32BIT_1;
    ulCasComparator_32 = MAGIC_NUMBER_32BIT_1;
    ulCasNewValue_32 = MAGIC_NUMBER_32BIT_2;
    ulReturnValue = 0;

    __asm__ __volatile__("atomic_cas_1: nop");
    ulReturnValue = Atomic_CompareAndSwap_u32( &ulCasDestination_32, ulCasNewValue_32, ulCasComparator_32);
    __asm__ __volatile__("atomic_cas_1_end: nop");

    configPRINTF( ( "%s -- CAS (diff location). swapped? [%s], returned old value? [%s].\r\n",
                    __FUNCTION__,
                    ( ulCasDestination_32 == ulCasNewValue_32 ? "true" : "false" ),
                    ( ulReturnValue == MAGIC_NUMBER_32BIT_1 ? "true" : "false" )
                ) );

    // #2 -- CAS, comparator from the same mem location.
    ulCasDestination_32 = MAGIC_NUMBER_32BIT_1;
    ulReturnValue = 0;

    __asm__ __volatile__("atomic_cas_2: nop");
    ulReturnValue = Atomic_CompareAndSwap_u32( &ulCasDestination_32, MAGIC_NUMBER_32BIT_2, ulCasDestination_32);
    __asm__ __volatile__("atomic_cas_2_end: nop");

    configPRINTF( ( "%s -- CAS (same location). swapped? [%s], returned old value? [%s].\r\n",
                    __FUNCTION__,
                    ( ulCasDestination_32 == MAGIC_NUMBER_32BIT_2 ? "true" : "false" ),
                    ( ulReturnValue == MAGIC_NUMBER_32BIT_1 ? "true" : "false" )
                ) );

    // #3 -- swap
    pSwapDestination_32 = &ulCasDestination_32;
    pSwapNewValue_32 = &ulCasNewValue_32;
    pReturnValue_32 = NULL;

    __asm__ __volatile__("atomic_xchg_32bit: nop");
    pReturnValue_32 = Atomic_SwapPointers_p32( (void **)&pSwapDestination_32, pSwapNewValue_32);
    __asm__ __volatile__("atomic_xchg_32bit_end: nop");

    configPRINTF( ( "%s -- Swap (32-bit). swapped? [%s], returned old value? [%s].\r\n",
                        __FUNCTION__,
                        ( pSwapDestination_32 == &ulCasNewValue_32 ? "true" : "false" ),
                        ( pReturnValue_32 ==  &ulCasDestination_32? "true" : "false" )
                    ) );

    // #4 -- swap, pointer to variable of uint8_t type.
    pSwapDestination_8 = &uCasDestination_8;
    pSwapNewValue_8 = &uCasComparator_8;
    pReturnValue_8 = NULL;

    __asm__ __volatile__("atomic_xchg_8bit: nop");
    pReturnValue_8 = Atomic_SwapPointers_p32( (void **)&pSwapDestination_8, pSwapNewValue_8);
    __asm__ __volatile__("atomic_xchg_8bit_end: nop");

    configPRINTF( ( "%s -- Swap (8-bit). swapped? [%s], returned old value? [%s].\r\n",
                        __FUNCTION__,
                        ( pSwapDestination_8 == &uCasComparator_8 ? "true" : "false" ),
                        ( pReturnValue_8 ==  &uCasDestination_8? "true" : "false" )
                    ) );


    // #5 -- CAS pointers.
    pSwapDestination_32 = &ulCasDestination_32;
    pSwapNewValue_32 = &ulCasNewValue_32;
    pReturnValue_32 = NULL;

    __asm__ __volatile__("atomic_CAS_pointers: nop");
    pReturnValue_32 = Atomic_CompareAndSwapPointers_p32( (void **)&pSwapDestination_32, pSwapNewValue_32, &ulCasDestination_32);
    __asm__ __volatile__("atomic_CAS_pointers_end: nop");

    configPRINTF( ( "%s -- CAS pointers (same location). swapped? [%s], returned old value? [%s].\r\n",
                        __FUNCTION__,
                        ( (intptr_t)pSwapDestination_32 == (intptr_t)pSwapNewValue_32 ? "true" : "false" ),
                        ( (intptr_t)pReturnValue_32 == (intptr_t)&ulCasDestination_32 ? "true" : "false" )
                    ) );

    return;
}

void vKernelAtomicTryout_Arithmetic_happy_path( void )
{
    int32_t iAddend_32;
    int32_t iDelta_32;
    int32_t iReturnValue_32;

    int8_t  iAddend_8;

    /* asm (built-in function) implementation --
     * for curiosity, see what instructions add-register and add-immediate are using. */

    // #1 -- add, overflow
    iAddend_32 = 1;
    iDelta_32 = INT32_MAX;
    iReturnValue_32 = 0;

    __asm__ __volatile__("atomic_add_reg: nop");
    iReturnValue_32 = Atomic_Add_i32( &iAddend_32, iDelta_32);
    __asm__ __volatile__("atomic_add_reg_end: nop");

    configPRINTF( ( "%s -- Atomic add (register). added? [%s], returned old value? [%s].\r\n",
                        __FUNCTION__,
                        ( iAddend_32 == INT32_MIN ? "true" : "false" ),
                        ( iReturnValue_32 == 1 ? "true" : "false" )
                    ) );

    // #2 -- add immediate
    iAddend_32 = 1;
    iDelta_32 = INT32_MAX;
    iReturnValue_32 = 0;

    __asm__ __volatile__("atomic_add_imme: nop");
    iReturnValue_32 = Atomic_Add_i32( &iAddend_32, INT32_MAX);
    __asm__ __volatile__("atomic_add_imme_end: nop");

    configPRINTF( ( "%s -- Atomic add (immediate). added? [%s], returned old value? [%s].\r\n",
                        __FUNCTION__,
                        ( iAddend_32 == INT32_MIN ? "true" : "false" ),
                        ( iReturnValue_32 == 1 ? "true" : "false" )
                    ) );

    // #3 -- add, 8-bit casting
    iAddend_8 = 1;
    iAddend_32 = (uint32_t)iAddend_8;

    __asm__ __volatile__("atomic_add_8bit: nop");
    iReturnValue_32 = Atomic_Add_i32( &iAddend_32, INT8_MAX);
    __asm__ __volatile__("atomic_add_8bit_end: nop");

    configPRINTF( ( "%s -- Atomic add (8-bit). Casted return value [%d], which is not INT8_MIN [%d]!!!\r\n",
                        __FUNCTION__,
                        (uint8_t)iReturnValue_32,
                        INT8_MIN
                    ) );

    // #4 -- sub, almost but not underflow
    iAddend_32 = -1;

    __asm__ __volatile__("atomic_sub_reg: nop");
    iReturnValue_32 = Atomic_Subtract_i32( &iAddend_32, INT32_MAX);
    __asm__ __volatile__("atomic_sub_reg_end: nop");

    configPRINTF( ( "%s -- Atomic sub (immediate). Subtracted? [%s], returned old value? [%s].\r\n",
                        __FUNCTION__,
                        ( iAddend_32 == INT32_MIN ? "true" : "false" ),
                        ( iReturnValue_32 == -1 ? "true" : "false" )
                    ) );

    // #5 -- inc, sanity check
    iAddend_32 = INT32_MAX;

    __asm__ __volatile__("atomic_inc: nop");
    iReturnValue_32 = Atomic_Increment_i32( &iAddend_32);
    __asm__ __volatile__("atomic_inc_end: nop");

    configPRINTF( ( "%s -- Atomic increment. Incremented? [%s], returned old value? [%s].\r\n",
                        __FUNCTION__,
                        ( iAddend_32 == INT32_MIN ? "true" : "false" ),
                        ( iReturnValue_32 == INT32_MAX ? "true" : "false" )
                    ) );

    // #6 -- dec, sanity check
    iAddend_32 = INT32_MIN;

    __asm__ __volatile__("atomic_dec: nop");
    iReturnValue_32 = Atomic_Decrement_i32( &iAddend_32);
    __asm__ __volatile__("atomic_dec_end: nop");

    configPRINTF( ( "%s -- Atomic decrement. Decremented? [%s], returned old value? [%s].\r\n",
                        __FUNCTION__,
                        ( iAddend_32 == INT32_MAX ? "true" : "false" ),
                        ( iReturnValue_32 == INT32_MIN ? "true" : "false" )
                    ) );
}

void vKernelAtomicTryout_Bitwise_happy_path(void)
{
    uint32_t ulOp1;
    uint32_t ulOp2;
    uint32_t ulReturnValue;

    // #1 -- and
    ulOp1 = MAGIC_NUMBER_32BIT_1;
    ulOp2 = MAGIC_NUMBER_32BIT_2;

    __asm__ __volatile__("atomic_and: nop");
    ulReturnValue = Atomic_AND_u32( &ulOp1, ulOp2);
    __asm__ __volatile__("atomic_and_end: nop");

    configPRINTF( ( "%s -- Atomic and. ANDed? [%s], returned old value? [%s].\r\n",
                        __FUNCTION__,
                        ( ulOp1 == 0xA0A0A0A0 ? "true" : "false" ),
                        ( ulReturnValue == MAGIC_NUMBER_32BIT_1 ? "true" : "false" )
                        ) );

    // #2 -- or
    ulOp1 = MAGIC_NUMBER_32BIT_2;
    ulOp2 = MAGIC_NUMBER_32BIT_3;

    __asm__ __volatile__("atomic_or: nop");
    ulReturnValue = Atomic_OR_u32( &ulOp1, ulOp2);
    __asm__ __volatile__("atomic_or_end: nop");

    configPRINTF( ( "%s -- Atomic or. ORed? [%s], returned old value? [%s].\r\n",
                        __FUNCTION__,
                        ( ulOp1 == 0xF0F0F0FF ? "true" : "false" ),
                        ( ulReturnValue == MAGIC_NUMBER_32BIT_2 ? "true" : "false" )
                        ) );

    // #3 -- nand
    ulOp1 = MAGIC_NUMBER_32BIT_1;
    ulOp2 = MAGIC_NUMBER_32BIT_2;

    __asm__ __volatile__("atomic_nand: nop");
    ulReturnValue = Atomic_NAND_u32( &ulOp1, ulOp2);
    __asm__ __volatile__("atomic_nand_end: nop");

    configPRINTF( ( "%s -- Atomic nand. NANDed? [%s], returned old value? [%s].\r\n",
                        __FUNCTION__,
                        ( ulOp1 == 0x5F5F5F5F ? "true" : "false" ),
                        ( ulReturnValue == MAGIC_NUMBER_32BIT_1 ? "true" : "false" )
                        ) );

    // #4 -- xor
    ulOp1 = MAGIC_NUMBER_32BIT_1;
    ulOp2 = MAGIC_NUMBER_32BIT_2;

    __asm__ __volatile__("atomic_xor: nop");
    ulReturnValue = Atomic_XOR_u32( &ulOp1, ulOp2);
    __asm__ __volatile__("atomic_XOR_end: nop");

    configPRINTF( ( "%s -- Atomic XOR. XORed? [%s], returned old value? [%s].\r\n",
                        __FUNCTION__,
                        ( ulOp1 == 0x55555555 ? "true" : "false" ),
                        ( ulReturnValue == MAGIC_NUMBER_32BIT_1 ? "true" : "false" )
                        ) );

}

