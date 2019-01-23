/*
 * Amazon FreeRTOS+POSIX V1.0.2
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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef _TRACING_H_
#define _TRACING_H_

/******** Attempt to go atomic ********/
#define POSIX_SEMAPHORE_IMPLEMENTATION          ( 1 )   /**< Set to value {0, 1, 2, 3} to select implementation.
                                                             0 - original implementation,
                                                             1 - critical section (possibly by disabling interrupt) + xSemaphoreGive()/xSemaphoreTake(),
                                                             2 - atomic with assembly.
                                                             3 - */
#if ( POSIX_SEMAPHORE_IMPLEMENTATION == 1 )
    #define POSIX_SEMAPHORE_IMPLEMENTATION_BRANCH_STAT  ( 0 )   /**< Set to 1 to see how many times we ended up executing FreeRTOS interfaces,
                                                                     instead of disabling interrupt. */
    #if ( POSIX_SEMAPHORE_IMPLEMENTATION_BRANCH_STAT == 1 )
        void FreeRTOS_POSIX_semaphore_initBranchTaken( void );
        void FreeRTOS_POSIX_semaphore_deInitBranchTaken( void );
        int FreeRTOS_POSIX_semaphore_getBranchTaken_post( void );
        int FreeRTOS_POSIX_semaphore_getBranchTaken_wait( void );
    #endif /* POSIX_SEMAPHORE_IMPLEMENTATION_BRANCH_STAT */
#endif /* POSIX_SEMAPHORE_IMPLEMENTATION */

#endif /* _TRACING_H_ */
