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

    typedef struct tracingData
    {
        uint64_t        ullPerfCounter_cycleElapsed;
        int             iPerfCounter_numOfEntry;
        pthread_mutex_t xPerfData_mutex;
    } tracingData_t;

    /* Private tracing data.
     * for simplicity, just make two variables and 4 getters to get trace value.
     */
    tracingData_t xTracingData_timedwait = { 0 };
    tracingData_t xTracingData_post = { 0 };
    /**
     * @brief Initialize perf counter related
     *
     * This interface is called by top application, which intends to
     * profile interfaces implemented in this file.
     *
     * @warning no thread safety is guaranteed.
     */
    void FreeRTOS_POSIX_semaphore_initPerfCounterCycleElapsed( void )
    {
        /* Init tracing data structure for sem_timedwait(). */
        xTracingData_timedwait.ullPerfCounter_cycleElapsed = 0;
        xTracingData_timedwait.iPerfCounter_numOfEntry = 0;
        xTracingData_timedwait.xPerfData_mutex = PTHREAD_MUTEX_INITIALIZER;

        pthread_mutex_init( &xTracingData_timedwait.xPerfData_mutex, NULL );

        /* Init tracing data structure for sem_post(). */
        xTracingData_post.ullPerfCounter_cycleElapsed = 0;
        xTracingData_post.iPerfCounter_numOfEntry = 0;
        xTracingData_post.xPerfData_mutex = PTHREAD_MUTEX_INITIALIZER;

        pthread_mutex_init( &xTracingData_post.xPerfData_mutex, NULL );
    }

    /**
     * @brief De-initialize perf counter related.
     *
     * This interface must be called by top application, which intends to
     * profile interfaces implemented in this file.
     *
     * @warning no thread safety is guaranteed.
     */
    void FreeRTOS_POSIX_semaphore_deInitPerfCounterCycleElapsed( void )
    {
        /* De-init mutex used for sem_timedwait() tracing. */
        pthread_mutex_destroy( &xTracingData_timedwait.xPerfData_mutex );

        /* De-init mutex used for sem_post() tracing. */
        pthread_mutex_destroy( &xTracingData_post.xPerfData_mutex );
    }

    uint64_t FreeRTOS_POSIX_semaphore_getPerfCounterCycleElapsed_timedwait( void )
    {
        return xTracingData_timedwait.ullPerfCounter_cycleElapsed;
    }

    int FreeRTOS_POSIX_semaphore_getPerfCounterNumOfEntry_timedwait( void )
    {
        return xTracingData_timedwait.iPerfCounter_numOfEntry;
    }

    uint64_t FreeRTOS_POSIX_semaphore_getPerfCounterCycleElapsed_post( void )
    {
        return xTracingData_post.ullPerfCounter_cycleElapsed;
    }

    int FreeRTOS_POSIX_semaphore_getPerfCounterNumOfEntry_post( void )
    {
        return xTracingData_post.iPerfCounter_numOfEntry;
    }

    static void prvTraceFunctionIn( uint64_t * pullCounterValue )
    {
        *pullCounterValue = aws_hal_perfcounter_get_value();
    }


    static void prvTraceFunctionOut( uint64_t * pullCounterValue, tracingData_t * pxTracingDataHandle )
    {
        *pullCounterValue = aws_hal_perfcounter_get_value() - *pullCounterValue;

        /* Increase total time elapsed atomically. */
        pthread_mutex_lock( &pxTracingDataHandle->xPerfData_mutex );
        pxTracingDataHandle->ullPerfCounter_cycleElapsed += *pullCounterValue;
        pxTracingDataHandle->iPerfCounter_numOfEntry += 1;;
        pthread_mutex_unlock( &pxTracingDataHandle->xPerfData_mutex );

        # if ( POSIX_SEMAPHORE_TRACING_VERBOSE == 1 )
        configPRINTF( ( "%s -- time elapsed %ld, total elapsed %ld, total entry %d\r\n",
                        __FUNCTION__,
                        (long)*pullCounterValue,
                        (long)pxTracingDataHandle->ullPerfCounter_cycleElapsed,
                        pxTracingDataHandle->iPerfCounter_numOfEntry ) );
        #endif /* POSIX_SEMAPHORE_TRACING_VERBOSE*/
    }

#endif /* POSIX_SEMAPHORE_TRACING */

#if ( POSIX_SEMAPHORE_IMPLEMENTATION == 1 && POSIX_SEMAPHORE_IMPLEMENTATION_BRANCH_STAT == 1 )
    #include "FreeRTOS_POSIX/pthread.h"
    pthread_mutex_t xBranchMutex_post = PTHREAD_MUTEX_INITIALIZER;
    int iBranchTaken_post;

    pthread_mutex_t xBranchMutex_wait = PTHREAD_MUTEX_INITIALIZER;
    int iBranchTaken_wait;

    /**
     * @brief Initialize related variables.
     *
     * @warning Repeated initialization has undefined behavior.
     * @warning Called by app.
     */
    void FreeRTOS_POSIX_semaphore_initBranchTaken( void )
    {
        /* Branch stats init. */
        pthread_mutex_init( &xBranchMutex_post, NULL );
        iBranchTaken_post = 0;

        pthread_mutex_init( &xBranchMutex_wait, NULL );
        iBranchTaken_wait = 0;
    }

    void FreeRTOS_POSIX_semaphore_deInitBranchTaken( void )
    {
        pthread_mutex_destroy( &xBranchMutex_post );
        pthread_mutex_destroy( &xBranchMutex_wait );
    }

    int FreeRTOS_POSIX_semaphore_getBranchTaken_post( void )
    {
        return iBranchTaken_post;
    }

    int FreeRTOS_POSIX_semaphore_getBranchTaken_wait( void )
    {
        return iBranchTaken_wait;
    }
#endif /* POSIX_SEMAPHORE_IMPLEMENTATION && POSIX_SEMAPHORE_IMPLEMENTATION_BRANCH_STAT */

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
    #if ( POSIX_SEMAPHORE_IMPLEMENTATION == 0 )
        *sval = ( int ) uxSemaphoreGetCount( ( SemaphoreHandle_t ) &pxSem->xSemaphore );
    #else
        *sval = pxSem->value;
    #endif /* POSIX_SEMAPHORE_IMPLEMENTATION */

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


    #if ( POSIX_SEMAPHORE_IMPLEMENTATION == 1 || POSIX_SEMAPHORE_IMPLEMENTATION == 2 )
        pxSem->value = value;
    #endif /* POSIX_SEMAPHORE_IMPLEMENTATION */

    /* Create the FreeRTOS semaphore. This call will not fail because the
     * memory for the semaphore has already been allocated. */
    if( iStatus == 0 )
    {
        #if ( POSIX_SEMAPHORE_IMPLEMENTATION == 0)
            ( void ) xSemaphoreCreateCountingStatic( SEM_VALUE_MAX, value, &pxSem->xSemaphore );
        #else
            /* If using disabling interrupt or atomic, sem value is handled by pxSem-> value. */
            ( void ) xSemaphoreCreateCountingStatic( SEM_VALUE_MAX, 0, &pxSem->xSemaphore );
        #endif /* POSIX_SEMAPHORE_IMPLEMENTATION */
        *sem = pxSem;
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

int sem_post( sem_t * sem )
{
    #if ( POSIX_SEMAPHORE_TRACING == 1 && POSIX_SEMAPHORE_TRACING_POST == 1)
        uint64_t ullInvocationTimeElapsed = 0;
        prvTraceFunctionIn( &ullInvocationTimeElapsed );
    #endif /* POSIX_SEMAPHORE_TRACING */

    sem_internal_t * pxSem = ( sem_internal_t * ) ( sem );

    #if ( POSIX_SEMAPHORE_IMPLEMENTATION == 1 || POSIX_SEMAPHORE_IMPLEMENTATION == 2 )
        int32_t previousValue = AwsIotAtomic_Add( &pxSem->value, 1 );

        if ( previousValue > 0)
        {
            /* There isn't any task blocked by this semaphore. */
#if ( POSIX_SEMAPHORE_IMPLEMENTATION_BRANCH_STAT == 1)
            pthread_mutex_lock( &xBranchMutex_post );
            iBranchTaken_post++;
            pthread_mutex_unlock( &xBranchMutex_post);
#endif /* POSIX_SEMAPHORE_IMPLEMENTATION_BRANCH_STAT */
        }
        else
    #endif /* POSIX_SEMAPHORE_IMPLEMENTATION */
        {
            /* Give the semaphore using the FreeRTOS API. */
            ( void ) xSemaphoreGive( ( SemaphoreHandle_t ) &pxSem->xSemaphore );
        }

    #if ( POSIX_SEMAPHORE_TRACING == 1 && POSIX_SEMAPHORE_TRACING_POST == 1)
        prvTraceFunctionOut( &ullInvocationTimeElapsed, &xTracingData_post );
    #endif /* POSIX_SEMAPHORE_TRACING */

    return 0;
}

/*-----------------------------------------------------------*/

int sem_timedwait( sem_t * sem,
                   const struct timespec * abstime )
{
    #if ( POSIX_SEMAPHORE_TRACING == 1 && POSIX_SEMAPHORE_TRACING_TIMEDWAIT == 1 )
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

    #if ( POSIX_SEMAPHORE_IMPLEMENTATION == 1 || POSIX_SEMAPHORE_IMPLEMENTATION == 2 )
        int32_t previousValue = AwsIotAtomic_Sub( &pxSem->value, 1 );

        if ( previousValue > 0 )
        {
            /* There is at least one semaphore, no need to block. */
#if ( POSIX_SEMAPHORE_IMPLEMENTATION_BRANCH_STAT == 1)
            pthread_mutex_lock( &xBranchMutex_wait );
            iBranchTaken_wait++;
            pthread_mutex_unlock( &xBranchMutex_wait);
#endif /*POSIX_SEMAPHORE_IMPLEMENTATION_BRANCH_STAT*/
        }
        else
    #endif /* POSIX_SEMAPHORE_IMPLEMENTATION */
        {
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
        }

    #if ( POSIX_SEMAPHORE_TRACING == 1 && POSIX_SEMAPHORE_TRACING_TIMEDWAIT == 1 )
        prvTraceFunctionOut( &ullInvocationTimeElapsed, &xTracingData_timedwait );
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
