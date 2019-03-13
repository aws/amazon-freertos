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
 * @file iot_platform_types_posix.h
 * @brief Definitions of platform layer types on POSIX systems.
 */

#ifndef _IOT_PLATFORM_TYPES_POSIX_H_
#define _IOT_PLATFORM_TYPES_POSIX_H_

/* POSIX includes. Allow the default POSIX headers to be overridden. */
#ifdef POSIX_TYPES_HEADER
    #include POSIX_TYPES_HEADER
#else
    #include <sys/types.h>
#endif
#ifdef POSIX_SEMAPHORE_HEADER
    #include POSIX_SEMAPHORE_HEADER
#else
    #include <semaphore.h>
#endif

/**
 * @brief The native mutex type on POSIX systems.
 */
typedef pthread_mutex_t   _IotSystemMutex_t;

/**
 * @brief The native semaphore type on POSIX systems.
 */
typedef sem_t             _IotSystemSemaphore_t;

/**
 * @brief Represents an #IotTimer_t on POSIX systems.
 */
typedef struct _IotSystemTimer
{
    timer_t timer;                      /**< @brief Underlying POSIX timer. */
    void * pArgument;                   /**< @brief First argument to threadRoutine. */
    void ( * threadRoutine )( void * ); /**< @brief Thread function to run on timer expiration. */
} _IotSystemTimer_t;

#endif /* ifndef _IOT_PLATFORM_TYPES_POSIX_H_ */
