/*
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
 */

/**
 * @file aws_iot_atomic.h
 * @brief Declares atomic operations data structures and functions.
 */

#ifndef _AWS_IOT_ATOMIC_H_
#define _AWS_IOT_ATOMIC_H_

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stdbool.h>
#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

#define AWS_IOT_ATOMIC_NATIVE_IMPLEMENTATION 0

#if AWS_IOT_ATOMIC_NATIVE_IMPLEMENTATION == 1

int32_t AwsIotAtomic_Add( int32_t volatile * pTarget, int32_t value )
{
    int32_t current;

    /*
    __asm__ __volatile__( <ASm code here> );
    */

    return current;
}

int32_t AwsIotAtomic_Sub( int32_t volatile * pTarget, int32_t value )
{
    int32_t current;

    /*
    __asm__ __volatile__( <ASm code here> );
    */

    return current;
}

int32_t AwsIotAtomic_Swap( int32_t volatile * pTarget, int32_t value )
{
    int32_t current;

    /*
    __asm__ __volatile__( <ASm code here> );
    */

    return current;
}

int32_t AwsIotAtomic_CompareAndSwap( int32_t volatile * pTarget, int32_t value, int32_t comparand )
{
    int32_t current;

    /*
    __asm__ __volatile__( <ASm code here> );
    */

    return current;
}

#else

int32_t AwsIotAtomic_Add( int32_t volatile * pTarget, int32_t value )
{
    int32_t current;

    portENTER_CRITICAL( );

    current = *pTarget;

    *pTarget += value;

    portEXIT_CRITICAL( );

    return current;
}

int32_t AwsIotAtomic_Sub( int32_t volatile * pTarget, int32_t value )
{
    int32_t current;

    portENTER_CRITICAL( );

    current = *pTarget;

    *pTarget -= value;

    portEXIT_CRITICAL( );

    return current;
}

int32_t AwsIotAtomic_Swap( int32_t volatile * pTarget, int32_t value )
{
    int32_t current;

    portENTER_CRITICAL( );

    current = *pTarget;

    *pTarget = value;

    portEXIT_CRITICAL( );

    return current;
}

int32_t AwsIotAtomic_CompareAndSwap( int32_t volatile * pTarget, int32_t value, int32_t comparand )
{
    int32_t current;

    portENTER_CRITICAL( );

    current = *pTarget;

    if ( *pTarget == comparand )
    {
        *pTarget = value;
    }

    portEXIT_CRITICAL( );

    return current;
}

#endif

#endif /* ifndef _AWS_IOT_ATOMIC_H_ */
