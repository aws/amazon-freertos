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
 * @file iot_platform_types.h
 * @brief Types of the platform layer.
 */

#ifndef _IOT_PLATFORM_TYPES_H_
#define _IOT_PLATFORM_TYPES_H_

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/*------------------------- Thread management types -------------------------*/

/**
 * @brief A value representing the system default for new thread priority.
 */
#define IOT_THREAD_DEFAULT_PRIORITY      0

/**
 * @brief A value representhing the system default for new thread stack size.
 */
#define IOT_THREAD_DEFAULT_STACK_SIZE    0

/**
 * @ingroup platform_datatypes_handles
 * @brief The type used to represent mutexes, configured with the type
 * `_IotSystemMutex_t`.
 *
 * <span style="color:red;font-weight:bold">
 * `_IotSystemMutex_t` will be automatically configured during build and generally
 * does not need to be defined.
 * </span>
 *
 * Mutexes should only be released by the threads that take them.
 *
 * <b>Example</b> <br>
 * To change the type of #IotMutex_t to `long`:
 * @code{c}
 * typedef long _IotSystemMutex_t;
 * #include "iot_threads.h"
 * @endcode
 */
typedef _IotSystemMutex_t       IotMutex_t;

/**
 * @ingroup platform_datatypes_handles
 * @brief The type used to represent semaphores, configured with the type
 * `_IotSystemSemaphore_t`.
 *
 * <span style="color:red;font-weight:bold">
 * `_IotSystemSemaphore_t` will be automatically configured during build and
 * generally does not need to be defined.
 * </span>
 *
 * Semaphores must be counting, and any thread may take (wait on) or release
 * (post to) a semaphore.
 *
 * <b>Example</b> <br>
 * To change the type of #IotSemaphore_t to `long`:
 * @code{c}
 * typedef long _IotSystemSemaphore_t;
 * #include "iot_threads.h"
 * @endcode
 */
typedef _IotSystemSemaphore_t   IotSemaphore_t;

/**
 * @brief Thread routine function.
 *
 * @param[in] void * The argument passed to the @ref
 * platform_threads_function_createdetachedthread. For application use.
 */
typedef void ( * IotThreadRoutine_t )( void * );

/*-------------------------- Clock and timer types --------------------------*/

/**
 * @ingroup platform_datatypes_handles
 * @brief The type used to represent timers, configured with the type
 * `_IotSystemTimer_t`.
 *
 * <span style="color:red;font-weight:bold">
 * `_IotSystemTimer_t` will be automatically configured during build and generally
 * does not need to be defined.
 * </span>
 *
 * <b>Example</b> <br>
 * To change the type of #IotTimer_t to `long`:
 * @code{c}
 * typedef long _IotSystemTimer_t;
 * #include "iot_clock.h"
 * @endcode
 */
typedef _IotSystemTimer_t IotTimer_t;

#endif /* ifndef _IOT_PLATFORM_TYPES_H_ */
