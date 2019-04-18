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
 * @file iot_atomic.h
 * @brief Chooses the appropriate atomic operations header.
 *
 * This file first checks if an atomic port is provided.
 *
 * Otherwise, if the operating system is Amazon FreeRTOS, the atomic header
 * provided with Amazon FreeRTOS is used.
 *
 * Otherwise, this file checks the compiler and chooses an appropriate atomic
 * header depending on the compiler.
 *
 * If no supported compilers are available, then a "generic" atomic implementation
 * is used. The generic implementation is less efficient, but will work on all
 * platforms.
 */

#ifndef IOT_ATOMIC_H_
#define IOT_ATOMIC_H_

/* Use an atomic port if provided. */
#if IOT_ATOMIC_USE_PORT == 1
    #include "atomic/iot_atomic_port.h"
#elif defined( __free_rtos__ )
    /* Use the FreeRTOS atomic operation header on FreeRTOS. */
    #include "atomic.h"
#elif defined( __GNUC__ )
    /* Both clang and gcc define __GNUC__, but only clang defines __clang__ */
    #ifdef __clang__
        /* clang versions 3.1.0 and greater have built-in atomic support. */
        #define CLANG_VERSION    ( __clang_major__ * 100 + __clang_minor__ )
        #if CLANG_VERSION > 301
            /* clang is compatible with gcc atomic extensions. */
            #include "atomic/iot_atomic_gcc.h"
        #else
            #define IOT_ATOMIC_GENERIC    1
        #endif
    #else
        /* GCC versions 4.7.0 and greater have built-in atomic support. */
        #define GCC_VERSION    ( __GNUC__ * 100 + __GNUC_MINOR__ )
        #if GCC_VERSION >= 407
            #include "atomic/iot_atomic_gcc.h"
        #else
            #define IOT_ATOMIC_GENERIC    1
        #endif
    #endif /* ifdef __clang__ */
#else  /* if IOT_ATOMIC_USE_PORT == 1 */
    #define IOT_ATOMIC_GENERIC    1
#endif /* if IOT_ATOMIC_USE_PORT == 1 */

/* Include the generic atomic header if no supported compiler was found. */
#if ( IOT_ATOMIC_GENERIC == 1 )
    #include "atomic/iot_atomic_generic.h"
#endif

#endif /* ifndef IOT_ATOMIC_H_ */
