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

/********* Library Level Tracing ********/
// top application is in producer_consumer.c
#define POSIX_SEMAPHORE_TRACING                 ( 0 )   /**< Set to 1 to enable POSIX semaphore tracing initialization.
                                                             With this set to 1 any below set to 0, you could still call the interfaces,
                                                             but the data for corresponding entry will be zero. */

#define POSIX_SEMAPHORE_TRACING_VERBOSE         ( 0 )   /**< Set to 1 to print something to console per profiling iteration. */

#define POSIX_SEMAPHORE_TRACING_TIMEDWAIT       ( 0 )   /**< Set to 1 to optionally profile sem_timedwait(). */
#define POSIX_SEMAPHORE_TRACING_POST            ( 0 )   /**< Set to 1 to optionally profile sem_post(). */

/******** Attempt to go atomic ********/
#define POSIX_SEMAPHORE_IMPLEMENTATION          ( 0 )   /**< Set to value {0, 1, 2} to select implementation.
                                                             0 - original implementation,
                                                             1 - critical section (possibly by disabling interrupt),
                                                             2 - atomic with assembly. */
#if ( POSIX_SEMAPHORE_IMPLEMENTATION == 1 )
    #define POSIX_SEMAPHORE_IMPLEMENTATION_BRANCH_STAT  ( 0 )   /**< Set to 1 to see how many times we ended up executing FreeRTOS interfaces,
                                                                     instead of disabling interrupt. */
    void FreeRTOS_POSIX_semaphore_initBranchTaken( void );
    void FreeRTOS_POSIX_semaphore_deInitBranchTaken( void );
    int FreeRTOS_POSIX_semaphore_getBranchTaken_post( void );
    int FreeRTOS_POSIX_semaphore_getBranchTaken_wait( void );
#endif /* POSIX_SEMAPHORE_IMPLEMENTATION */

#endif /* _TRACING_H_ */
