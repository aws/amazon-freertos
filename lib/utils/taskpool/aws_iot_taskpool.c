/*
* Amazon FreeRTOS
* Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
* @file aws_iot_taskpool.c
* @brief Implements the user-facing functions of the MQTT library.
*/

/* Standard includes. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Platform layer includes. */
#include "platform/aws_iot_threads.h"
#include "platform/aws_iot_clock.h"

/* Task pool internal include. */
#include "private/aws_iot_taskpool_internal.h"

/* Validate task pool configuration settings. */
#if AWS_IOT_TASKPOOL_ENABLE_ASSERTS != 0 && AWS_IOT_TASKPOOL_ENABLE_ASSERTS != 1
#error "AWS_IOT_TASKPOOL_ENABLE_ASSERTS must be 0 or 1."
#endif
#if AWS_IOT_TASKPOOL_TEST != 0 && AWS_IOT_TASKPOOL_TEST != 1
#error "AWS_IOT_TASKPOOL_TEST must be 0 or 1."
#endif

/* Establish a few convenience macros */

/**
* @brief Checks a return result from internal and external APIs for failure.
*
*/
#define _TASKPOOL_FAILED(x)    ( ( x ) != 0)

/**
* @brief Checks a return result from internal and external APIs for success.
*
*/
#define _TASKPOOL_SUCCEEDED(x) ( ( x ) == 0)

/**
* @brief Enter a critical section by locking a mutex.
*
*/
#define _TASKPOOL_ENTER_CRITICAL_SECTION AwsIotMutex_Lock( &( ( ( AwsIotTaskPool_t *)pTaskPool )->lock ) )

/**
* @brief Exit a critical section by unlocking a mutex.
*
*/
#define _TASKPOOL_EXIT_CRITICAL_SECTION AwsIotMutex_Unlock( &( ( ( AwsIotTaskPool_t *)pTaskPool )->lock ) )

/**
* @brief Maximum semaphore value for wait operations.
*/
#define _TASKPOOL_MAX_SEM_VALUE 0xFFFF

/* ---------------------------------------------------------------------------------------------- */

/**
* @cond DOXYGEN_IGNORE
* Doxygen should ignore this section.
*
* @brief The system task pool handle for all libraries to use. 
* User application can use the system task pool as well knowing that the usage will be shared with 
* the system libraries as well. The system task pool needs to be initialized before any library is used or 
* before any code that posts jobs to the task pool runs. 
*/
AwsIotTaskPool_t _IotSystemTaskPool = { 0 };

/** @endcond */

/* ---------------------------------------------------------------------------------------------- */

/**
* @cond DOXYGEN_IGNORE
* Doxygen should ignore this section.
*/

/* -------------- Convenience functions to create/recycle/destroy jobs -------------- */

/**
* Destroys one instance of a job.
*
* @param[in] pJob The job to destroy.
*
*/
static void _destroyJob( AwsIotTaskPoolJob_t * const pJob );

/* -------------- The worker thread procedure for a task pool thread -------------- */

/**
* The procedure for a task pool worker thread.
*
* @param[in] pUserContext The user context.
*
*/
static void _taskPoolWorker( void * pUserContext );

/* -------------- Convenience functions to create/initialize/destroy the task pool -------------- */

/**
* Initializes a pre-allocated instance of a task pool.
*
* @param[in] pInfo The initialization information for the task pool.
* @param[in] pTaskPool The pre-allocated instance of the task pool to initialize.
*
*/
static AwsIotTaskPoolError_t _initTaskPool( const AwsIotTaskPoolInfo_t * const pInfo,
                                            AwsIotTaskPool_t * const pTaskPool );

/**
* Initializes a pre-allocated instance of a task pool.
*
* @param[in] pInfo The initialization information for the task pool.
* @param[in] pTaskPoolBuffer A storage to build the task pool when staic allocation is chosen.
* @param[out] pTaskPool The handle to the created task pool.
*
*/
static AwsIotTaskPoolError_t _createTaskPool( const AwsIotTaskPoolInfo_t * const pInfo,
                                            AwsIotTaskPool_t * const ppTaskPool );

/**
* Destroys one instance of a task pool.
*
* @param[in] pTaskPool The task pool to destroy.
*
*/
static void _destroyTaskPool( AwsIotTaskPool_t * const pTaskPool );

/**
* Check for the exit condition.
*
* @param[in] pTaskPool The task pool to destroy.
*
*/
static bool _shutdownInProgress( const AwsIotTaskPool_t * const pTaskPool );

/**
* Set the exit condition.
*
* @param[in] pTaskPool The task pool to destroy.
*
*/
static void _signalShutdown( AwsIotTaskPool_t * const pTaskPool, uint32_t threads );
/** @endcond */

/* ---------------------------------------------------------------------------------------------- */

AwsIotTaskPool_t * AwsIotTaskPool_GetSystemTaskPool( )
{
    return &_IotSystemTaskPool;
}

AwsIotTaskPoolError_t AwsIotTaskPool_CreateSystemTaskPool( const AwsIotTaskPoolInfo_t * const pInfo )
{
    AwsIotTaskPoolError_t error = AwsIotTaskPool_Create( pInfo, &_IotSystemTaskPool );

    return error;
}

AwsIotTaskPoolError_t AwsIotTaskPool_Create( const AwsIotTaskPoolInfo_t * const pInfo, 
                                             AwsIotTaskPool_t * const pTaskPool )
{
    AwsIotTaskPoolError_t error;

    if ( pTaskPool == NULL )
    {
        error = AWS_IOT_TASKPOOL_BAD_PARAMETER;
    }
    else
    {
        error = _createTaskPool( pInfo, pTaskPool );
    }

    return error;
}

AwsIotTaskPoolError_t AwsIotTaskPool_Destroy( AwsIotTaskPool_t * pTaskPool )
{
    AwsIotTaskPoolError_t error = AWS_IOT_TASKPOOL_SUCCESS;
    uint32_t count;

    /* Track how many threads the task pool owns. */
    uint32_t activeThreads;

    /* Parameter checking. */
    if ( pTaskPool == NULL )
    {
        error = AWS_IOT_TASKPOOL_BAD_PARAMETER;
    }
    else
    {
        /* Destroying the task pool should be safe, and therefore we will grab the task pool lock.
         * No worker thread or application thread should access any data structure
         * in the task pool while the task pool is being destroyed. */
        _TASKPOOL_ENTER_CRITICAL_SECTION;
        {
            IotLink_t * pItemLink;

            /* Record how many active threads in the task pool. */
            activeThreads = pTaskPool->activeThreads;

            /* Destroying a Task pool happens in four (4) stages: first we lock the task pool mutex for safety, and then (1) we clear the work queue. 
             * Then we (2)w e set the exit condition and wake up all active worker threads. Worker threads will observe the exit condition and bail out 
             * without trying to pick up any further work.  We will then release the mutex and (3) wait for all worker threads to signal exit, 
             * before (4) destroying all task pool data structures and release the associated memory.
             */

             /* (1) Clear the job queue. */
            do
            {
                pItemLink = NULL;

                pItemLink = IotQueue_Dequeue( &pTaskPool->dispatchQueue );

                if ( pItemLink != NULL )
                {
                    AwsIotTaskPoolJob_t * pJob = IotLink_Container( AwsIotTaskPoolJob_t, pItemLink, link );

                    _destroyJob( pJob );
                }

            } while ( pItemLink );

            /* (2) Set the exit condition. */
            _signalShutdown( pTaskPool, activeThreads );
        }
        _TASKPOOL_EXIT_CRITICAL_SECTION;

        /* (3) Wait for all active threads to reach the end of their life-span. */
        for ( count = 0; count < activeThreads; ++count )
        {
            AwsIotSemaphore_Wait( &pTaskPool->startStopSignal );
        }

        /* Make sure sure all threads have actually exited. */
        while ( pTaskPool->activeThreads != 0 )
        {
        }

        AwsIotTaskPool_Assert( pTaskPool->activeThreads == 0 );
        AwsIotTaskPool_Assert( AwsIotSemaphore_GetCount( &pTaskPool->startStopSignal ) == 0 );

        /* (4) Destroy all signaling objects. */
        _destroyTaskPool( pTaskPool );
    }

    return error;
}

AwsIotTaskPoolError_t AwsIotTaskPool_SetMaxThreads( AwsIotTaskPool_t * pTaskPool, uint32_t maxThreads )
{
    AwsIotTaskPoolError_t error = AWS_IOT_TASKPOOL_SUCCESS;

    /* Parameter checking. */
    if ( pTaskPool == NULL ||
         pTaskPool->minThreads > maxThreads ||
         maxThreads < 1 )
    {
        error = AWS_IOT_TASKPOOL_BAD_PARAMETER;
    }
    else
    {
        _TASKPOOL_ENTER_CRITICAL_SECTION;
        {
            /* Bail out early if this task pool is shutting down. */
            if ( _shutdownInProgress( pTaskPool ) )
            {
                error = AWS_IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS;
            }
            else
            {
                uint32_t currentMaxThreads = pTaskPool->maxThreads;

                /* Reset the max threads counter. */
                pTaskPool->maxThreads = maxThreads;

                /* If the number of maximum threads in the pool is set to be smaller than the current value,
                * then we need to signal all redundant to exit.
                */
                if ( maxThreads < currentMaxThreads )
                {
                    uint32_t count = currentMaxThreads - maxThreads;

                    while ( count-- > 0 )
                    {
                        AwsIotSemaphore_Post( &pTaskPool->dispatchSignal );
                    }
                }
            }
        }
        _TASKPOOL_EXIT_CRITICAL_SECTION;
    }

    return error;
}

AwsIotTaskPoolError_t AwsIotTaskPool_CreateJob( const IotTaskPoolRoutine_t userCallback,
                                                void * const pUserContext,
                                                AwsIotTaskPoolJob_t * const pJob )
{
    AwsIotTaskPoolError_t error = AWS_IOT_TASKPOOL_SUCCESS;

    /* Parameter checking. */
    if ( userCallback == NULL ||
         pJob == NULL )
    {
        error = AWS_IOT_TASKPOOL_BAD_PARAMETER;
    }
    else
    {
        /* Build a job around the user-provided storage. */
        pJob->link.pNext = NULL;
        pJob->link.pPrevious = NULL;
        pJob->userCallback = userCallback;
        pJob->pUserContext = pUserContext;
        pJob->status = AWS_IOT_TASKPOOL_STATUS_READY;

    }

    return error;
}

AwsIotTaskPoolError_t AwsIotTaskPool_DestroyJob( AwsIotTaskPoolJob_t * const pJob )
{
    AwsIotTaskPoolError_t error = AWS_IOT_TASKPOOL_SUCCESS;

    /* Parameter checking. */
    if ( pJob == NULL )
    {
        error = AWS_IOT_TASKPOOL_BAD_PARAMETER;
    }
    else
    {
        /* Do not destroy a job in the dispatch queue or the recycle queue. */
        if ( IotLink_IsLinked( &pJob->link ) )
        {
            AwsIotLogWarn( "Attempt to destroy a job that is part of a queue." );

            error = AWS_IOT_TASKPOOL_ILLEGAL_OPERATION;
        }
        else
        {
            _destroyJob( pJob );
        }
    }

    return error;
}

AwsIotTaskPoolError_t AwsIotTaskPool_Schedule( AwsIotTaskPool_t * const pTaskPool, 
                                               AwsIotTaskPoolJob_t * const pJob )
{
    AwsIotTaskPoolError_t error = AWS_IOT_TASKPOOL_SUCCESS;

    /* Parameter checking. */
    if ( pTaskPool == NULL ||
         pJob == NULL )
    {
        error = AWS_IOT_TASKPOOL_BAD_PARAMETER;
    }
    /* Bail out early if this task pool is shutting down. */
    else if ( _shutdownInProgress( pTaskPool ) )
    {
        AwsIotLogWarn( "Attempt to schedule a job while shutdown is in progress." );

        error = AWS_IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS;
    }
    else
    {
        _TASKPOOL_ENTER_CRITICAL_SECTION;
        {
            /* Bail out early if this task pool is shutting down. */
            if ( _shutdownInProgress( pTaskPool ) )
            {
                AwsIotLogInfo( "Attempt to schedule a job while shutdown is in progress." );

                error = AWS_IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS;
            }
            else if ( IotLink_IsLinked( &pJob->link ) )
            {
                AwsIotLogWarn( "Attempt to schedule a job in the dispatch queue." );

                error = AWS_IOT_TASKPOOL_ILLEGAL_OPERATION;
            }
            else
            {
                /* Append the job to the dispatch queue. */
                IotQueue_Enqueue( &pTaskPool->dispatchQueue, &pJob->link );

                /* Update the job status to 'scheduled'. */
                pJob->status = AWS_IOT_TASKPOOL_STATUS_SCHEDULED;

                /* Signal a worker to pick up the job. */
                AwsIotSemaphore_Post( &pTaskPool->dispatchSignal );

                /* If all threads are busy, try and create a new one. Failing to create a new thread
                 * only has performance implications on correctly exeuting th scheduled job.
                 */
                uint32_t activeThreads = pTaskPool->activeThreads;

                if ( activeThreads == pTaskPool->busyThreads )
                {
                    if ( activeThreads < pTaskPool->maxThreads )
                    {
                        AwsIotLogInfo( "Growing a Task pool with a new worker thread..." );

                        if ( AwsIot_CreateDetachedThread(_taskPoolWorker, pTaskPool ) )
                        {
                            AwsIotSemaphore_Wait( &pTaskPool->startStopSignal );

                            pTaskPool->activeThreads++;
                        }
                        else
                        {
                            /* Failure to create a worker thread does not hinder functional correctness, but rather just responsiveness. */
                            AwsIotLogWarn( "Task pool failed to create a worker thread." );
                        }
                    }
                }
            }
        }
        _TASKPOOL_EXIT_CRITICAL_SECTION;
    }

    return error;
}

AwsIotTaskPoolError_t AwsIotTaskPool_GetStatus( const AwsIotTaskPoolJob_t * pJob, AwsIotTaskPoolJobStatus_t * const pStatus )
{
    AwsIotTaskPoolError_t error = AWS_IOT_TASKPOOL_SUCCESS;

    /* Parameter checking. */
    if ( pJob == NULL || 
         pStatus == NULL )
    {
        error = AWS_IOT_TASKPOOL_BAD_PARAMETER;
    }
    else
    {
        *pStatus = pJob->status;
    }

    return error;
}

AwsIotTaskPoolError_t AwsIotTaskPool_TryCancel( const AwsIotTaskPool_t * pTaskPool, 
                                                AwsIotTaskPoolJob_t * const pJob, 
                                                AwsIotTaskPoolJobStatus_t * const pStatus )
{
    AwsIotTaskPoolError_t error = AWS_IOT_TASKPOOL_SUCCESS;

    /* Parameter checking. */
    if ( pTaskPool == NULL ||
         pJob == NULL )
    {
        error = AWS_IOT_TASKPOOL_BAD_PARAMETER;
    }
    /* Bail out early if this task pool is shutting down. */
    else if ( _shutdownInProgress( pTaskPool ) )
    {
        AwsIotLogWarn( "Attempt to cancel a job while shutdown is in progress." );

        error = AWS_IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS;
    }
    else
    {
        *pStatus = AWS_IOT_TASKPOOL_STATUS_UNDEFINED;

        _TASKPOOL_ENTER_CRITICAL_SECTION;
        {
            /* Check again if this task pool is shutting down. */
            if ( _shutdownInProgress( pTaskPool ) )
            {
                AwsIotLogWarn( "Attempt to cancel a job while shutdown is in progress." );

                error = AWS_IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS;
            }
            else 
            {
                /* We can only cancel jobs that are either 'ready' (waiting to be scheduled) or 'scheduled'. */
                {
                    bool cancelable = false;

                    /* Register the current status. */
                    AwsIotTaskPoolJobStatus_t currentStatus = pJob->status;

                    switch ( currentStatus )
                    {
                    case AWS_IOT_TASKPOOL_STATUS_READY:
                    case AWS_IOT_TASKPOOL_STATUS_SCHEDULED:
                        cancelable = true;
                        break;

                    case AWS_IOT_TASKPOOL_STATUS_EXECUTING:
                    case AWS_IOT_TASKPOOL_STATUS_COMPLETED:
                    case AWS_IOT_TASKPOOL_STATUS_CANCELED:
                        AwsIotLogWarn( "Attempt to cancel a job that is already, executing, completed, or canceled." );
                        break;

                    default:
                        AwsIotLogError( "Attempt to cancel a job with an undefined state." );
                        break;
                    }

                    /* Update the returned status to the current status of the job. */
                    if ( pStatus != NULL )
                    {
                        *pStatus = currentStatus;
                    }

                    if ( cancelable )
                    {
                        /* Update the status of the job. */
                        ( ( AwsIotTaskPoolJob_t * )pJob )->status = AWS_IOT_TASKPOOL_STATUS_CANCELED;

                        /* If the job is cancelable and still in the dispatch queue, then unlink it and signal any waiting threads. */
                        if ( IotLink_IsLinked( &pJob->link ) )
                        {
                            /* If the job is cancelable, it must be in the dispatch queue. */
                            AwsIotTaskPool_Assert( currentStatus == AWS_IOT_TASKPOOL_STATUS_SCHEDULED );

                            IotQueue_Remove( &pJob->link );
                        }
                    }
                    else
                    {
                        error = AWS_IOT_TASKPOOL_CANCEL_FAILED;
                    }
                }
            }
        }
        _TASKPOOL_EXIT_CRITICAL_SECTION;
    }

    return error;
}

/* ---------------------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */

/**
* @cond DOXYGEN_IGNORE
* Doxygen should ignore this section.
*/

static AwsIotTaskPoolError_t _createTaskPool( const AwsIotTaskPoolInfo_t * const pInfo, AwsIotTaskPool_t * const pTaskPool )
{
    uint32_t count;
    AwsIotTaskPoolError_t error = AWS_IOT_TASKPOOL_SUCCESS;

    /* Check input values for consistency. */
    if ( pInfo == NULL ||
         pTaskPool == NULL ||
         pInfo->minThreads > pInfo->maxThreads    ||
         pInfo->minThreads < 1 ||
         pInfo->maxThreads < 1 )
    {
        error = AWS_IOT_TASKPOOL_BAD_PARAMETER;
    }

    /* Start creating the pTaskPool. */
    else
    {
        /* Initialize all internal data structure prior to creating all threads. */
        error = _initTaskPool( pInfo, pTaskPool );

        if ( _TASKPOOL_SUCCEEDED( error ) )
        {
            uint32_t threadsCreated;

            AwsIotTaskPool_Assert( pInfo->minThreads == pTaskPool->minThreads );
            AwsIotTaskPool_Assert( pInfo->maxThreads == pTaskPool->maxThreads );

            /* The task pool will initialize the minimum number of threads reqeusted by the user upon start. */
            /* When a thread is created, it will signal a semaphore to signify that it is about to wait on incoming */
            /* jobs. A thread can be woken up for exit or for new jobs only at that point in time.  */
            /* The exit condition is setting the maximum number of threads to 0. */

            /* Create the minimum number of threads specified by the user, and if one fails shutdown and return error. */
            for ( threadsCreated = 0; threadsCreated < pTaskPool->minThreads; )
            {
                /* Create one thread. */
                if ( AwsIot_CreateDetachedThread( _taskPoolWorker, pTaskPool ) == false )
                {
                    AwsIotLogError( "Could not create worker thread! Exiting..." );

                    /* If creating one thread fails, set error condition and exit the loop. */
                    error = AWS_IOT_TASKPOOL_NO_MEMORY;
						
                    break;
                }

                /* Upon succesfull thread creation, increase the number of active threads. */
                pTaskPool->activeThreads++;

                ++threadsCreated;
            }

            /* Wait for threads to be ready to wait on the condition, so that threads are actually able to receive messages. */
            for ( count = 0; count < threadsCreated; ++count )
            {
                AwsIotSemaphore_Wait( &pTaskPool->startStopSignal );
            }

            /* In case of failure, wait on the created threads to exit. */
            if ( _TASKPOOL_FAILED( error ) )
            {
                /* Set the exit condition for the newly created threads. */
                _signalShutdown( pTaskPool, threadsCreated );

                /* Signal all threads to exit. */
                for ( count = 0; count < threadsCreated; ++count )
                {
                    AwsIotSemaphore_Wait( &pTaskPool->startStopSignal );
                }

                _destroyTaskPool( pTaskPool );
            }
        }
    }

    return error;
}

static AwsIotTaskPoolError_t _initTaskPool( const AwsIotTaskPoolInfo_t * const pInfo, AwsIotTaskPool_t * const pTaskPool )
{
    AwsIotTaskPoolError_t error = AWS_IOT_TASKPOOL_SUCCESS;

    bool semStartStopInit = false;
    bool lockInit = false;
    bool semDispatchInit = false;

    /* Zero out all data structures. */
    memset( ( void * )pTaskPool, 0x00, sizeof( AwsIotTaskPool_t ) );

    /* Initialize a job data structures that require no de-initialization.
    * All other data structures carry a value of 'NULL' before initailization.
    */
    IotQueue_Create( &pTaskPool->dispatchQueue );
    
    pTaskPool->minThreads = pInfo->minThreads;
    pTaskPool->maxThreads = pInfo->maxThreads;
    pTaskPool->stackSize = pInfo->stackSize;
    pTaskPool->priority = pInfo->priority;

    /* Initialize the semaphore to ensure all threads have started. */
    if ( AwsIotSemaphore_Create( &pTaskPool->startStopSignal, 0, _TASKPOOL_MAX_SEM_VALUE ) )
    {
        semStartStopInit = true;

        if ( AwsIotMutex_Create( &pTaskPool->lock ) )
        {
            lockInit = true;

            /* Initialize the semaphore for waiting for incoming work. */
            if ( AwsIotSemaphore_Create( &pTaskPool->dispatchSignal, 0, _TASKPOOL_MAX_SEM_VALUE ) )
            {
                semDispatchInit = true;
            }
            else
            {
                error = AWS_IOT_TASKPOOL_NO_MEMORY;
            }
        }
        else
        {
            error = AWS_IOT_TASKPOOL_NO_MEMORY;
        }
    }
    else
    {
        error = AWS_IOT_TASKPOOL_NO_MEMORY;
    }

    if ( _TASKPOOL_FAILED( error ) )
    {
        if ( semStartStopInit )
        {
            AwsIotSemaphore_Destroy( &pTaskPool->startStopSignal );
        }
        if ( lockInit )
        {
            AwsIotMutex_Destroy( &pTaskPool->lock );
        }
        if ( semDispatchInit )
        {
            AwsIotSemaphore_Destroy( &pTaskPool->dispatchSignal );
        }
    }

    return error;
}

static void _destroyTaskPool( AwsIotTaskPool_t * const pTaskPool )
{
    AwsIotSemaphore_Destroy( &pTaskPool->startStopSignal );

    AwsIotMutex_Destroy( &pTaskPool->lock );

    AwsIotSemaphore_Destroy( &pTaskPool->dispatchSignal );
}

/* ---------------------------------------------------------------------------------------------- */

static void _taskPoolWorker( void * pUserContext )
{
    /* Extract pTaskPool pointer from context. */
    AwsIotTaskPool_Assert( pUserContext != NULL );
    AwsIotTaskPool_t * pTaskPool = ( AwsIotTaskPool_t * )pUserContext;

    /* Signal that this worker completed initialization and it is ready to receive notifications. */
    AwsIotSemaphore_Post( &pTaskPool->startStopSignal );

    /* OUTER LOOP: it controls the lifetiem of the worker thread: exit condition for a worker thread
    * is setting maxThreads to zero. A worker thread is running until the maximum number of allowed
    * threads is not zero and the active threads are less than the maximum number of allowed threads.
    */
    while ( pTaskPool->activeThreads <= pTaskPool->maxThreads )
    {
        IotLink_t * pFirst = NULL;
        IotLink_t * pNext = NULL;
        AwsIotTaskPoolJob_t * pJob = NULL;

        /* Acquire the lock to check the exit condition, and release the lock if the exit condition is verified,
        * or before waiting for incoming notifications.
        */
        _TASKPOOL_ENTER_CRITICAL_SECTION;
        {
            /* If the exit condition is verified, update the number of active threads and exit the loop. */
            if ( _shutdownInProgress( pTaskPool ) )
            {
                AwsIotLogDebug( "Worker thread exiting because exit condition was set." );

                /* Release the lock before checking the exit condition. */
                _TASKPOOL_EXIT_CRITICAL_SECTION;

                /* Abandon the OUTER LOOP. */
                break;
            }
        }
        _TASKPOOL_EXIT_CRITICAL_SECTION;

        /* Wait on incoming notifications... */
        AwsIotSemaphore_Wait( &pTaskPool->dispatchSignal );

        /* .. upon receiving a notification, grab the lock... */
        _TASKPOOL_ENTER_CRITICAL_SECTION;
        {
            /* If the exit condition is verified, update the number of active threads and exit the loop.
            * This can happen when the task pool is being destroyed.
            */
            if ( _shutdownInProgress( pTaskPool ) )
            {
                AwsIotLogDebug( "Worker thread exiting because exit condition was set." );

                _TASKPOOL_EXIT_CRITICAL_SECTION;

                /* Abandon the OUTER LOOP. */
                break;
            }

            /* Dequeue one job in FIFO order. */
            pFirst = IotQueue_Dequeue( &pTaskPool->dispatchQueue );

            /* If there is indeed a job, then update status under lock, and release the lock before processing the job. */
            if ( pFirst != NULL )
            {
                /* Update the number of busy threads, so new requests can be served by creating new threads, up to maxThreads. */
                pTaskPool->busyThreads++;

                /* Extract the job from its link. */
                pJob = IotLink_Container( AwsIotTaskPoolJob_t, pFirst, link );

                /* Update status to 'executing'. */
                pJob->status = AWS_IOT_TASKPOOL_STATUS_EXECUTING;
				
                /* Check if there is another job in the queue, and if there is one, signal the task pool. */
                pNext = IotQueue_Peek( &pTaskPool->dispatchQueue );

                if ( pNext != NULL )
                {
                    AwsIotSemaphore_Post( &pTaskPool->dispatchSignal );
                }
            }
        }
        _TASKPOOL_EXIT_CRITICAL_SECTION;

        /* INNER LOOP: it controls the execution of jobs: the exit condition is the lack of a job to execute. */
        while ( pJob != NULL )
        {
			/* Record callback, so job can be re-used in the callback. */
			IotTaskPoolRoutine_t userCallback = pJob->userCallback;
			
            /* Process the job by invoking the associated callback with the user context.
            * This task pool thread will not be available until the user callback returns.
            */
            {
                userCallback( pTaskPool, pJob, pJob->pUserContext );
            }

            /* Acquire the lock before updating the job status. */
            _TASKPOOL_ENTER_CRITICAL_SECTION;
            {
                /* Mark the job as 'completed', and signal completion on the associated semaphore. */
                pJob->status = AWS_IOT_TASKPOOL_STATUS_COMPLETED;

                /* Signal the completion wait handle. */

                /* Try and dequeue the next job in the dispatch queue. */
                {
                    IotLink_t * pItem = NULL;

                    /* Dequeue the next job from the dispatch queue. */
                    pItem = IotQueue_Dequeue( &pTaskPool->dispatchQueue );

                    /* If there is no job left in the dispatch queue, update the worker status and leave. */
                    if ( pItem == NULL )
                    {
                        /* Update the busy threads value. */
                        pTaskPool->busyThreads--;

                        _TASKPOOL_EXIT_CRITICAL_SECTION;

                        /* Abandon the INNER LOOP. Execution will tranfer back to the OUTER LOOP condition. */
                        break;
                    }
                    else
                    {
                        pJob = IotLink_Container( AwsIotTaskPoolJob_t, pItem, link );
                    }
                }

                pJob->status = AWS_IOT_TASKPOOL_STATUS_EXECUTING;
            }
            _TASKPOOL_EXIT_CRITICAL_SECTION;
        }
    }

    AwsIotLogDebug( "Worker thread exiting because maximum quota was exceeded." );

    _TASKPOOL_ENTER_CRITICAL_SECTION;
    {
        /* Signal that this worker is exiting. */
        AwsIotSemaphore_Post( &pTaskPool->startStopSignal );

        pTaskPool->activeThreads--;
    }
    _TASKPOOL_EXIT_CRITICAL_SECTION;
}

/* ---------------------------------------------------------------------------------------------- */

static void _destroyJob( AwsIotTaskPoolJob_t * const pJob )
{

}

/* ---------------------------------------------------------------------------------------------- */

static bool _shutdownInProgress( const AwsIotTaskPool_t * const pTaskPool )
{
    return ( pTaskPool->maxThreads == 0 );
}

static void _signalShutdown( AwsIotTaskPool_t * const pTaskPool, uint32_t threads )
{
    /* Set the exit condition. */
    pTaskPool->maxThreads = 0;

    /* Broadcast to all active threads to wake-up. Active threads do check the exit condition right after wakein up. */
    for ( uint32_t count = 0; count < threads; ++count )
    {
        AwsIotSemaphore_Post( &pTaskPool->dispatchSignal );
    }
}

/** @endcond */
