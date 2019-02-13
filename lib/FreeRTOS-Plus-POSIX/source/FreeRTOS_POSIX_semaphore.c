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

/**
 * @file FreeRTOS_POSIX_semaphore.c
 * @brief Implementation of functions in semaphore.h
 */

/* C standard library includes. */
#include <stddef.h>

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/semaphore.h"
#include "FreeRTOS_POSIX/utils.h"
#include "FreeRTOS_POSIX/tracing.h"


/**
 * Enabling semaphore tracing
 *
 * @warning No thread guarantee is assured. This is not user interface,
 * whoever using tracing suppose to control how interfaces are called.
 *
 * @warning Performance counter shall be started prior. In this file,
 * we are only calling aws_hal_perfcounter_get_value().
 */

#if ( POSIX_SEMAPHORE_TRACING == 1)
    #include <stdint.h>
    #include "FreeRTOS_POSIX/pthread.h"
    #include "aws_hal_perfcounter.h"


    uint64_t ullPerfCounterCycleElapsed_timedwait = 0;
    int iPerfCounterNumOfEntry_timedwait = 0;
    //pthread_mutex_t xMutexCycleElapsed_timedwait;

    void FreeRTOS_POSIX_semaphore_initPerfCounterCycleElapsed( void )
    {
        ullPerfCounterCycleElapsed_timedwait = 0;
        iPerfCounterNumOfEntry_timedwait = 0;
        //pthread_mutex_init( &xMutexCycleElapsed_timedwait, NULL );
    }

    void FreeRTOS_POSIX_semaphore_deInitPerfCounterCycleElapsed( void )
    {
        //pthread_mutex_destroy( &xMutexCycleElapsed_timedwait );
    }

    uint64_t FreeRTOS_POSIX_semaphore_getPerfCounterCycleElapsed( void )
    {
        return ullPerfCounterCycleElapsed_timedwait;
    }

    int FreeRTOS_POSIX_semaphore_getPerfCounterNumOfEntry( void )
    {
        return iPerfCounterNumOfEntry_timedwait;
    }

    void prvTraceFunctionIn( uint64_t * pullCounterValue )
    {
        *pullCounterValue = aws_hal_perfcounter_get_value();
    }


    void prvTraceFunctionOut( uint64_t * pullCounterValue )
    {
        *pullCounterValue = aws_hal_perfcounter_get_value() - *pullCounterValue;

        /* Increase total time elapsed atomically. */
        /* pthread_mutex_lock( &xMutexCycleElapsed_timedwait );
        ullPerfCounterCycleElapsed_timedwait += *pullCounterValue;
        iPerfCounterNumOfEntry_timedwait++;
        pthread_mutex_unlock( &xMutexCycleElapsed_timedwait );*/
        configPRINTF( ( "%s -- time elapsed %u \r\n", __FUNCTION__, *pullCounterValue ) );
    }

#endif /* POSIX_SEMAPHORE_TRACING */


/*-----------------------------------------------------------*/

int sem_destroy( sem_t * sem )
{
    sem_internal_t * pxSem = ( sem_internal_t * ) ( sem );

    /* Free the resources in use by the semaphore. */
    vSemaphoreDelete( ( SemaphoreHandle_t ) &pxSem->xSemaphore );

    return 0;
}

/*-----------------------------------------------------------*/

int sem_getvalue( sem_t * sem,
                  int * sval )
{
    sem_internal_t * pxSem = ( sem_internal_t * ) ( sem );

    /* Get the semaphore count using the FreeRTOS API. */
    *sval = ( int ) uxSemaphoreGetCount( ( SemaphoreHandle_t ) &pxSem->xSemaphore );

    return 0;
}

/*-----------------------------------------------------------*/

int sem_init( sem_t * sem,
              int pshared,
              unsigned value )
{
    int iStatus = 0;
    sem_internal_t * pxSem =  ( sem_internal_t * ) ( sem );

    /* Silence warnings about unused parameters. */
    ( void ) pshared;

    /* Check value parameter. */
    if( value > SEM_VALUE_MAX )
    {
        errno = EINVAL;
        iStatus = -1;
    }


    /* Create the FreeRTOS semaphore. This call will not fail because the
     * memory for the semaphore has already been allocated. */
    if( iStatus == 0 )
    {
        ( void ) xSemaphoreCreateCountingStatic( SEM_VALUE_MAX, value, &pxSem->xSemaphore );
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

int sem_post( sem_t * sem )
{
    sem_internal_t * pxSem = ( sem_internal_t * ) ( sem );

    /* Give the semaphore using the FreeRTOS API. */
    ( void ) xSemaphoreGive( ( SemaphoreHandle_t ) &pxSem->xSemaphore );

    return 0;
}

/*-----------------------------------------------------------*/

int sem_timedwait( sem_t * sem,
                   const struct timespec * abstime )
{
    #if ( POSIX_SEMAPHORE_TRACING == 1 )
        uint64_t ullInvocationTimeElapsed = 0;
        prvTraceFunctionIn( &ullInvocationTimeElapsed );
    #endif /* POSIX_SEMAPHORE_TRACING */

    int iStatus = 0;
    sem_internal_t * pxSem = ( sem_internal_t * ) ( sem );
    TickType_t xDelay = portMAX_DELAY;

    if( abstime != NULL )
    {
        /* If the provided timespec is invalid, still attempt to take the
         * semaphore without blocking, per POSIX spec. */
        if( UTILS_ValidateTimespec( abstime ) == false )
        {
            xDelay = 0;
            iStatus = EINVAL;
        }
        else
        {
            struct timespec xCurrentTime = { 0 };

            /* Get current time */
            if( clock_gettime( CLOCK_REALTIME, &xCurrentTime ) != 0 )
            {
                iStatus = EINVAL;
            }
            else
            {
                iStatus = UTILS_AbsoluteTimespecToDeltaTicks( abstime, &xCurrentTime, &xDelay );
            }

            /* If abstime was in the past, still attempt to take the semaphore without
             * blocking, per POSIX spec. */
            if( iStatus == ETIMEDOUT )
            {
                xDelay = 0;
            }
        }
    }

    /* Take the semaphore using the FreeRTOS API. */
    if( xSemaphoreTake( ( SemaphoreHandle_t ) &pxSem->xSemaphore,
                        xDelay ) != pdTRUE )
    {
        if( iStatus == 0 )
        {
            errno = ETIMEDOUT;
        }
        else
        {
            errno = iStatus;
        }

        iStatus = -1;
    }
    else
    {
        iStatus = 0;
    }

    #if ( POSIX_SEMAPHORE_TRACING == 1 )
        prvTraceFunctionOut( &ullInvocationTimeElapsed );
    #endif /* POSIX_SEMAPHORE_TRACING */

    return iStatus;
}

/*-----------------------------------------------------------*/

int sem_trywait( sem_t * sem )
{
    int iStatus = 0;

    /* Setting an absolute timeout of 0 (i.e. in the past) will cause sem_timedwait
     * to not block. */
    struct timespec xTimeout = { 0 };

    iStatus = sem_timedwait( sem, &xTimeout );

    /* POSIX specifies that this function should set errno to EAGAIN and not
     * ETIMEDOUT. */
    if( ( iStatus == -1 ) && ( errno == ETIMEDOUT ) )
    {
        errno = EAGAIN;
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

int sem_wait( sem_t * sem )
{
    return sem_timedwait( sem, NULL );
}

/*-----------------------------------------------------------*/
