/*
 * Amazon FreeRTOS+POSIX V1.0.0
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

/**
 * @file FreeRTOS_POSIX_utils.c
 * @brief Implementation of utility functions in utils.h
 */

/* C standard library includes. */
#include <stddef.h>

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/utils.h"

/*-----------------------------------------------------------*/

size_t UTILS_strnlen( const char * const pcString,
                      size_t xMaxLength )
{
    const char * pcCharPointer = pcString;
    size_t xLength = 0;

    if( pcString != NULL )
    {
        while( ( *pcCharPointer != '\0' ) && ( xLength < xMaxLength ) )
        {
            xLength++;
            pcCharPointer++;
        }
    }

    return xLength;
}

/*-----------------------------------------------------------*/

int UTILS_AbsoluteTimespecToTicks( const struct timespec * const pxAbsoluteTime,
                                   TickType_t * const pxResult )
{
    int iStatus = 0;
    struct timespec xCurrentTime = { 0 }, xDifference = { 0 };

    /* Check parameters. */
    if( ( pxAbsoluteTime == NULL ) || ( pxResult == NULL ) )
    {
        iStatus = EINVAL;
    }

    /* Get the current time. */
    if( iStatus == 0 )
    {
        if( clock_gettime( CLOCK_REALTIME, &xCurrentTime ) != 0 )
        {
            iStatus = errno;
        }
    }

    /* Calculate the difference between the current time and pxAbsoluteTime. */
    if( iStatus == 0 )
    {
        if( UTILS_TimespecSubtract( &xDifference, pxAbsoluteTime, &xCurrentTime ) != 0 )
        {
            /* pxAbsoluteTime was in the past. */
            iStatus = ETIMEDOUT;
        }
    }

    /* Convert the time difference to ticks. */
    if( iStatus == 0 )
    {
        iStatus = UTILS_TimespecToTicks( &xDifference, pxResult );
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

int UTILS_TimespecToTicks( const struct timespec * const pxTimespec,
                           TickType_t * const pxResult )
{
    int iStatus = 0;
    uint64_t ullTotalTicks = 0;
    long lNanoseconds = 0;

    /* The FreeRTOS tick rate should never be 0 or negative. */
    configASSERT( configTICK_RATE_HZ >= 0 );

    /* Check parameters. */
    if( ( pxTimespec == NULL ) || ( pxResult == NULL ) )
    {
        iStatus = EINVAL;
    }
    else if( ( pxTimespec != NULL ) && ( UTILS_ValidateTimespec( pxTimespec ) == false ) )
    {
        iStatus = EINVAL;
    }

    if( iStatus == 0 )
    {
        /* Convert timespec.tv_sec to ticks. */
        ullTotalTicks = ( uint64_t ) configTICK_RATE_HZ * ( uint64_t ) ( pxTimespec->tv_sec );

        /* Check for overflow of uint64_t. */
        if( ullTotalTicks / ( ( uint64_t ) configTICK_RATE_HZ ) != ( uint64_t ) ( pxTimespec->tv_sec ) )
        {
            iStatus = EOVERFLOW;
        }
        /* Ensure that ullTotalTicks fits in TickType_t. */
        else if( ullTotalTicks > ( uint64_t ) portMAX_DELAY )
        {
            iStatus = EOVERFLOW;
        }
    }

    if( iStatus == 0 )
    {
        /* Convert timespec.tv_nsec to ticks. This value does not have to be checked
         * for overflow because a valid timespec has 0 <= tv_nsec < 1000000000 and
         * NANOSECONDS_PER_TICK > 1. */
        lNanoseconds = pxTimespec->tv_nsec / ( long ) NANOSECONDS_PER_TICK +                  /* Whole nanoseconds. */
                       ( long ) ( pxTimespec->tv_nsec % ( long ) NANOSECONDS_PER_TICK != 0 ); /* Add 1 to round up if needed. */

        /* Add the nanoseconds to the total ticks. */
        ullTotalTicks += ( uint64_t ) lNanoseconds;

        /* Check for overflow of uint64_t from the above addition. */
        if( ullTotalTicks - ( uint64_t ) lNanoseconds > ullTotalTicks )
        {
            iStatus = EOVERFLOW;
        }
        /* Ensure that ullTotalTicks fits in TickType_t. */
        else if( ullTotalTicks > ( uint64_t ) portMAX_DELAY )
        {
            iStatus = EOVERFLOW;
        }
        else
        {
            /* Write result. */
            *pxResult = ( TickType_t ) ullTotalTicks;
        }
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

void UTILS_NanosecondsToTimespec( int64_t llSrc,
                                  struct timespec * const pxDest )
{
    long lCarrySec = 0;

    /* Check the conversion constant. */
    configASSERT( NANOSECONDS_PER_SECOND == 1000000000LL );

    /* Convert to timespec. */
    pxDest->tv_sec = ( time_t ) ( llSrc / NANOSECONDS_PER_SECOND );
    pxDest->tv_nsec = ( long ) ( llSrc % NANOSECONDS_PER_SECOND );

    /* Subtract from tv_sec if tv_nsec < 0. */
    if( pxDest->tv_nsec < 0L )
    {
        /* Compute the number of seconds to carry. */
        lCarrySec = ( pxDest->tv_nsec / ( long ) NANOSECONDS_PER_SECOND ) + 1L;

        pxDest->tv_sec -= ( time_t ) ( lCarrySec );
        pxDest->tv_nsec += lCarrySec * ( long ) NANOSECONDS_PER_SECOND;
    }
}

/*-----------------------------------------------------------*/

int UTILS_TimespecAdd( struct timespec * const pxResult,
                       const struct timespec * const x,
                       const struct timespec * const y )
{
    int64_t llResult64 = 0;

    /* Check parameters. */
    if( ( pxResult == NULL ) || ( x == NULL ) || ( y == NULL ) )
    {
        return -1;
    }

    /* Perform addition. */
    llResult64 = ( ( ( int64_t ) ( x->tv_sec ) * NANOSECONDS_PER_SECOND ) + ( int64_t ) ( x->tv_nsec ) )
                 + ( ( ( int64_t ) ( y->tv_sec ) * NANOSECONDS_PER_SECOND ) + ( int64_t ) ( y->tv_nsec ) );

    /* Convert result to timespec. */
    UTILS_NanosecondsToTimespec( llResult64, pxResult );

    return ( int ) ( llResult64 < 0LL );
}

/*-----------------------------------------------------------*/

int UTILS_TimespecAddNanoseconds( struct timespec * const pxResult,
                                  const struct timespec * const x,
                                  int64_t llNanoseconds )
{
    struct timespec y = { .tv_sec = ( time_t ) 0, .tv_nsec = 0L };

    /* Check parameters. */
    if( ( pxResult == NULL ) || ( x == NULL ) )
    {
        return -1;
    }

    /* Convert llNanoseconds to a timespec. */
    UTILS_NanosecondsToTimespec( llNanoseconds, &y );

    /* Perform addition. */
    return UTILS_TimespecAdd( pxResult, x, &y );
}

/*-----------------------------------------------------------*/

int UTILS_TimespecSubtract( struct timespec * const pxResult,
                            const struct timespec * const x,
                            const struct timespec * const y )
{
    int64_t llResult64 = 0;

    /* Check parameters. */
    if( ( pxResult == NULL ) || ( x == NULL ) || ( y == NULL ) )
    {
        return -1;
    }

    /* Perform addition. */
    llResult64 = ( ( ( int64_t ) ( x->tv_sec ) * NANOSECONDS_PER_SECOND ) + ( int64_t ) ( x->tv_nsec ) )
                 - ( ( ( int64_t ) ( y->tv_sec ) * NANOSECONDS_PER_SECOND ) + ( int64_t ) ( y->tv_nsec ) );

    /* Convert result to timespec. */
    UTILS_NanosecondsToTimespec( llResult64, pxResult );

    return ( int ) ( llResult64 < 0LL );
}

/*-----------------------------------------------------------*/

bool UTILS_ValidateTimespec( const struct timespec * const pxTimespec )
{
    bool xReturn = false;

    if( pxTimespec != NULL )
    {
        /* Verify 0 <= tv_nsec < 1000000000. */
        if( ( pxTimespec->tv_nsec >= 0 ) &&
            ( pxTimespec->tv_nsec < NANOSECONDS_PER_SECOND ) )
        {
            xReturn = true;
        }
    }

    return xReturn;
}

/*-----------------------------------------------------------*/
