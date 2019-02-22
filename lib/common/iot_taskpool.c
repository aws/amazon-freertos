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
 * @file iot_taskpool.c
 * @brief Implements the task pool functions in iot_taskpool.h
 */

/* Standard includes. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Platform layer includes. */
#include "platform/iot_threads.h"
#include "platform/iot_clock.h"

/* Task pool internal include. */
#include "private/iot_taskpool_internal.h"

/**
 * @brief Enter a critical section by locking a mutex.
 *
 */
#define _TASKPOOL_ENTER_CRITICAL_SECTION        IotMutex_Lock( &( ( ( IotTaskPool_t * ) pTaskPool )->lock ) )

/**
 * @brief Try entering a critical section by trying and lock a mutex.
 *
 */
#define _TASKPOOL_TRY_ENTER_CRITICAL_SECTION    IotMutex_TryLock( &( ( ( IotTaskPool_t * ) pTaskPool )->lock ) )

/**
 * @brief Exit a critical section by unlocking a mutex.
 *
 */
#define _TASKPOOL_EXIT_CRITICAL_SECTION         IotMutex_Unlock( &( ( ( IotTaskPool_t * ) pTaskPool )->lock ) )

/**
 * @brief Maximum semaphore value for wait operations.
 */
#define _TASKPOOL_MAX_SEM_VALUE                 0xFFFF

/**
 * @brief Reschedule delay in milliseconds for deferred jobs.
 */
#define _TASKPOOL_JOB_RESCHEDULE_DELAY_MS       10

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
IotTaskPool_t _IotSystemTaskPool = { 0 };

/** @endcond */

/* ---------------------------------------------------------------------------------------------- */

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 */

/* -------------- Convenience functions to create/recycle/destroy jobs -------------- */

/**
 * @brief Initializes one instance of a Task pool cache.
 *
 * @param[in] pCache The pre-allocated instance of the cache to initialize.
 */
static void _initJobsCache( IotTaskPoolCache_t * const pCache );

/**
 * @brief Extracts and initializes one instance of a job from the cache or, if there is none available, it allocates and initialized a new one.
 *
 * @param[in] pCache The instance of the cache to extract the job from.
 */
static IotTaskPoolJob_t * _fetchOrAllocateJob( IotTaskPoolCache_t * const pCache );

/**
 * Recycles one instance of a job into the cache or, if the cache is full, it destroys it.
 *
 * @param[in] pCache The instance of the cache to recycle the job into.
 * @param[in] pJob The job to recycle.
 *
 */
static void _recycleJob( IotTaskPoolCache_t * const pCache,
                         IotTaskPoolJob_t * const pJob );

/**
 * Destroys one instance of a job.
 *
 * @param[in] pJob The job to destroy.
 *
 */
static void _destroyJob( IotTaskPoolJob_t * const pJob );

/* -------------- The worker thread procedure for a task pool thread -------------- */

/**
 * The procedure for a task pool worker thread.
 *
 * @param[in] pUserContext The user context.
 *
 */
static void _taskPoolWorker( void * pUserContext );

/* -------------- Convenience functions to handle timer events  -------------- */

/**
 * Comparer for the time list.
 *
 * param[in] pTimerEventLink1 The link to the first timer event.
 * param[in] pTimerEventLink1 The link to the first timer event.
 */
static int32_t _timerEventCompare( const IotLink_t * const pTimerEventLink1,
                                   const IotLink_t * const pTimerEventLink2 );

static void _rescheduleDeferredJobsTimer( IotTimer_t * const pTimer,
                                          _taskPoolTimerEvent_t * const pFirstTimerEvent );

static void _timerThread( void * pArgument );

/* -------------- Convenience functions to create/initialize/destroy the task pool -------------- */

/**
 * Initializes a pre-allocated instance of a task pool.
 *
 * @param[in] pInfo The initialization information for the task pool.
 * @param[in] pTaskPool The pre-allocated instance of the task pool to initialize.
 *
 */
static IotTaskPoolError_t _initTaskPoolControlStructures( const IotTaskPoolInfo_t * const pInfo,
                                                          IotTaskPool_t * const pTaskPool );

/**
 * Initializes a pre-allocated instance of a task pool.
 *
 * @param[in] pInfo The initialization information for the task pool.
 * @param[in] pTaskPoolBuffer A storage to build the task pool when staic allocation is chosen.
 * @param[out] pTaskPool The handle to the created task pool.
 *
 */
static IotTaskPoolError_t _createTaskPool( const IotTaskPoolInfo_t * const pInfo,
                                           IotTaskPool_t * const ppTaskPool );

/**
 * Destroys one instance of a task pool.
 *
 * @param[in] pTaskPool The task pool to destroy.
 *
 */
static void _destroyTaskPool( IotTaskPool_t * const pTaskPool );

/**
 * Check for the exit condition.
 *
 * @param[in] pTaskPool The task pool to destroy.
 *
 */
static bool _IsShutdownStarted( const IotTaskPool_t * const pTaskPool );

/**
 * Set the exit condition.
 *
 * @param[in] pTaskPool The task pool to destroy.
 *
 */
static void _signalShutdown( IotTaskPool_t * const pTaskPool,
                             uint32_t threads );

/**
 * Places a job in the dispatch queue.
 *
 * @param[in] pTaskPool The task pool to scheduel the job with.
 * @param[in] pJob The job to schedule.
 *
 */
static IotTaskPoolError_t _scheduleInternal( IotTaskPool_t * const pTaskPool,
                                             IotTaskPoolJob_t * const pJob );

/**
 * Matches a deferred job in the timer queue with its timer event wrapper.
 *
 * @param[in] pLink A pointer to the timer event link in the timer queue.
 * @param[in] pMatch A pointer to the job to match.
 *
 */
static bool _matchJobByPointer( const IotLink_t * pLink,
                                void * pMatch );

/**
 * Tries to cancel a job.
 *
 * @param[in] pTaskPool The task pool to cancel an operation against.
 * @param[in] pJob The job to cancel.
 * @param[out] pStatus The status of the job at the time of cancellation.
 *
 */
static IotTaskPoolError_t _tryCancelInternal( IotTaskPool_t * const pTaskPool,
                                              IotTaskPoolJob_t * const pJob,
                                              IotTaskPoolJobStatus_t * const pStatus );

/**
 * Try to safely cancel and/or remove a job from the cache when the user calls API out of order.
 *
 * @param[in] pTaskPool The task pool to safely extract a job from.
 * @param[in] pJob The job to extract.
 *
 */
static IotTaskPoolError_t _trySafeExtraction( IotTaskPool_t * const pTaskPool,
                                              IotTaskPoolJob_t * const pJob,
                                              bool checkExecutionInProgress );
/** @endcond */

/* ---------------------------------------------------------------------------------------------- */

IotTaskPool_t * IotTaskPool_GetSystemTaskPool()
{
    return &_IotSystemTaskPool;
}

/*-----------------------------------------------------------*/

IotTaskPoolError_t IotTaskPool_CreateSystemTaskPool( const IotTaskPoolInfo_t * const pInfo )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    _TASKPOOL_SET_AND_GOTO_CLEANUP( IotTaskPool_Create( pInfo, &_IotSystemTaskPool ) );

    _TASKPOOL_NO_FUNCTION_CLEANUP();
}

/*-----------------------------------------------------------*/

IotTaskPoolError_t IotTaskPool_Create( const IotTaskPoolInfo_t * const pInfo,
                                       IotTaskPool_t * const pTaskPool )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pTaskPool );

    _TASKPOOL_SET_AND_GOTO_CLEANUP( _createTaskPool( pInfo, pTaskPool ) );

    _TASKPOOL_NO_FUNCTION_CLEANUP();
}

/*-----------------------------------------------------------*/

IotTaskPoolError_t IotTaskPool_Destroy( IotTaskPool_t * pTaskPool )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    uint32_t count;

    /* Track how many threads the task pool owns. */
    uint32_t activeThreads;

    /* Parameter checking. */
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pTaskPool );

    /* Destroying the task pool should be safe, and therefore we will grab the task pool lock.
     * No worker thread or application thread should access any data structure
     * in the task pool while the task pool is being destroyed. */
    _TASKPOOL_ENTER_CRITICAL_SECTION;
    {
        IotLink_t * pItemLink;

        /* Record how many active threads in the task pool. */
        activeThreads = pTaskPool->activeThreads;

        /* Destroying a Task pool happens in six (6) stages: First, (1) we clear the job queue and (2) the timer queue.
         * Then (3) we clear the jobs cache. We will then (4) wait for all worker threads to signal exit,
         * before (5) setting the exit condition and wake up all active worker threads. Finally (6) destroying
         * all task pool data structures and release the associated memory.
         */

        /* (1) Clear the job queue. */
        do
        {
            pItemLink = NULL;

            pItemLink = IotQueue_Dequeue( &pTaskPool->dispatchQueue );

            if( pItemLink != NULL )
            {
                IotTaskPoolJob_t * pJob = IotLink_Container( IotTaskPoolJob_t, pItemLink, link );

                _destroyJob( pJob );
            }
        } while( pItemLink );

        /* (2) Clear the timer queue. */
        do
        {
            pItemLink = NULL;

            pItemLink = IotListDouble_RemoveHead( &pTaskPool->timerEventsList );

            if( pItemLink != NULL )
            {
                _taskPoolTimerEvent_t * pTimerEvent = IotLink_Container( _taskPoolTimerEvent_t, pItemLink, link );

                _destroyJob( pTimerEvent->pJob );

                IotTaskPool_FreeTimerEvent( pTimerEvent );
            }
        } while( pItemLink );

        /* (3) Clear the job cache. */
        do
        {
            pItemLink = NULL;

            pItemLink = IotListDouble_RemoveHead( &pTaskPool->jobsCache.freeList );

            if( pItemLink != NULL )
            {
                IotTaskPoolJob_t * pJob = IotLink_Container( IotTaskPoolJob_t, pItemLink, link );

                _destroyJob( pJob );
            }
        } while( pItemLink );

        /* (4) Set the exit condition. */
        _signalShutdown( pTaskPool, activeThreads );
    }
    _TASKPOOL_EXIT_CRITICAL_SECTION;

    /* (5) Wait for all active threads to reach the end of their life-span. */
    for( count = 0; count < activeThreads; ++count )
    {
        IotSemaphore_Wait( &pTaskPool->startStopSignal );
    }

    IotTaskPool_Assert( pTaskPool->activeThreads == 0 );
    IotTaskPool_Assert( IotSemaphore_GetCount( &pTaskPool->startStopSignal ) == 0 );

    /* (6) Destroy all signaling objects. */
    _destroyTaskPool( pTaskPool );

    _TASKPOOL_NO_FUNCTION_CLEANUP();
}

/*-----------------------------------------------------------*/

IotTaskPoolError_t IotTaskPool_SetMaxThreads( IotTaskPool_t * pTaskPool,
                                              uint32_t maxThreads )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    /* Parameter checking. */
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pTaskPool );
    _TASKPOOL_ON_ARG_ERROR_GOTO_CLEANUP( pTaskPool->minThreads > maxThreads );
    _TASKPOOL_ON_ARG_ERROR_GOTO_CLEANUP( maxThreads < 1 );

    _TASKPOOL_ENTER_CRITICAL_SECTION;
    {
        /* Bail out early if this task pool is shutting down. */
        if( _IsShutdownStarted( pTaskPool ) )
        {
            _TASKPOOL_EXIT_CRITICAL_SECTION;

            _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS );
        }

        uint32_t currentMaxThreads = pTaskPool->maxThreads;

        /* Reset the max threads counter. */
        pTaskPool->maxThreads = maxThreads;

        /* If the number of maximum threads in the pool is set to be smaller than the current value,
         * then we need to signal all redundant to exit.
         */
        if( maxThreads < currentMaxThreads )
        {
            uint32_t count = currentMaxThreads - maxThreads;

            while( count-- > 0 )
            {
                IotSemaphore_Post( &pTaskPool->dispatchSignal );
            }
        }
    }
    _TASKPOOL_EXIT_CRITICAL_SECTION;

    _TASKPOOL_NO_FUNCTION_CLEANUP();
}

/*-----------------------------------------------------------*/

IotTaskPoolError_t IotTaskPool_CreateJob( const IotTaskPoolRoutine_t userCallback,
                                          void * const pUserContext,
                                          IotTaskPoolJob_t * const pJob )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    /* Parameter checking. */
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( userCallback );
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pJob );

    /* Build a job around the user-provided storage. */
    pJob->link.pNext = NULL;
    pJob->link.pPrevious = NULL;
    pJob->userCallback = userCallback;
    pJob->pUserContext = pUserContext;
    pJob->status = IOT_TASKPOOL_STATUS_READY | IOT_TASK_POOL_INTERNAL_STATIC;

    _TASKPOOL_NO_FUNCTION_CLEANUP();
}

/*-----------------------------------------------------------*/

IotTaskPoolError_t IotTaskPool_CreateRecyclableJob( IotTaskPool_t * const pTaskPool,
                                                    const IotTaskPoolRoutine_t userCallback,
                                                    void * const pUserContext,
                                                    IotTaskPoolJob_t ** const ppJob )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    /* Parameter checking. */
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pTaskPool );
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( userCallback );
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( ppJob );

    {
        IotTaskPoolJob_t * pTempJob = NULL;

        _TASKPOOL_ENTER_CRITICAL_SECTION;
        {
            /* Bail out early if this task pool is shutting down. */
            if( _IsShutdownStarted( pTaskPool ) )
            {
                _TASKPOOL_EXIT_CRITICAL_SECTION;

                _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS );
            }

            pTempJob = _fetchOrAllocateJob( &pTaskPool->jobsCache );
        }
        _TASKPOOL_EXIT_CRITICAL_SECTION;

        /* Initialize all members. */
        if( pTempJob == NULL )
        {
            IotLogInfo( "Failed to allocate a job." );

            _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_NO_MEMORY );
        }

        pTempJob->link.pNext = NULL;
        pTempJob->link.pPrevious = NULL;
        pTempJob->userCallback = userCallback;
        pTempJob->pUserContext = pUserContext;
        pTempJob->status = IOT_TASKPOOL_STATUS_READY;

        *ppJob = pTempJob;
    }

    _TASKPOOL_NO_FUNCTION_CLEANUP();
}

/*-----------------------------------------------------------*/

IotTaskPoolError_t IotTaskPool_DestroyJob( IotTaskPool_t * const pTaskPool,
                                           IotTaskPoolJob_t * const pJob )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    /* Parameter checking. */
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pJob );

    _TASKPOOL_ENTER_CRITICAL_SECTION;
    {
        /* Bail out early if this task pool is shutting down. */
        if( _IsShutdownStarted( pTaskPool ) )
        {
            _TASKPOOL_EXIT_CRITICAL_SECTION;

            _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS );
        }
        else
        {
            status = _trySafeExtraction( pTaskPool, pJob, false );
        }
    }
    _TASKPOOL_EXIT_CRITICAL_SECTION;

    if( _TASKPOOL_SUCCEEDED( status ) )
    {
        /* At this point, the job must not be in any queue or list. */
        IotTaskPool_Assert( IotLink_IsLinked( &pJob->link ) == false );

        _destroyJob( pJob );
    }

    _TASKPOOL_NO_FUNCTION_CLEANUP();
}

/*-----------------------------------------------------------*/

IotTaskPoolError_t IotTaskPool_RecycleJob( IotTaskPool_t * const pTaskPool,
                                           IotTaskPoolJob_t * const pJob )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    /* Parameter checking. */
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pTaskPool );
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pJob );

    _TASKPOOL_ENTER_CRITICAL_SECTION;
    {
        /* Bail out early if this task pool is shutting down. */
        if( _IsShutdownStarted( pTaskPool ) )
        {
            _TASKPOOL_EXIT_CRITICAL_SECTION;

            _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS );
        }

        /* Do not recycle statically allocated jobs. */
        if( ( pJob->status & IOT_TASK_POOL_INTERNAL_STATIC ) == 0 )
        {
            status = _trySafeExtraction( pTaskPool, pJob, false );
        }
        else
        {
            IotLogWarn( "Attempt to recycle a statically allocated job." );

            status = IOT_TASKPOOL_ILLEGAL_OPERATION;
        }

        /* If all safety checks completed, proceed. */
        if( _TASKPOOL_SUCCEEDED( status ) )
        {
            _recycleJob( &pTaskPool->jobsCache, pJob );
        }
    }
    _TASKPOOL_EXIT_CRITICAL_SECTION;

    _TASKPOOL_NO_FUNCTION_CLEANUP();
}

/*-----------------------------------------------------------*/

IotTaskPoolError_t IotTaskPool_Schedule( IotTaskPool_t * const pTaskPool,
                                         IotTaskPoolJob_t * const pJob )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    /* Parameter checking. */
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pTaskPool );
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pJob );

    _TASKPOOL_ENTER_CRITICAL_SECTION;
    {
        /* Bail out early if this task pool is shutting down. */
        if( _IsShutdownStarted( pTaskPool ) )
        {
            _TASKPOOL_EXIT_CRITICAL_SECTION;

            _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS );
        }
        else
        {
            status = _trySafeExtraction( pTaskPool, pJob, true );
        }

        /* If all safety checks completed, proceed. */
        if( _TASKPOOL_SUCCEEDED( status ) )
        {
            status = _scheduleInternal( pTaskPool, pJob );
        }
    }
    _TASKPOOL_EXIT_CRITICAL_SECTION;

    _TASKPOOL_NO_FUNCTION_CLEANUP();
}

/*-----------------------------------------------------------*/

IotTaskPoolError_t IotTaskPool_ScheduleDeferred( IotTaskPool_t * const pTaskPool,
                                                 IotTaskPoolJob_t * const pJob,
                                                 uint64_t timeMs )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    /* Parameter checking. */
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pTaskPool );
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pJob );

    if( timeMs == 0 )
    {
        _TASKPOOL_SET_AND_GOTO_CLEANUP( IotTaskPool_Schedule( pTaskPool, pJob ) );
    }

    _TASKPOOL_ENTER_CRITICAL_SECTION;
    {
        /* Bail out early if this task pool is shutting down. */
        if( _IsShutdownStarted( pTaskPool ) )
        {
            _TASKPOOL_EXIT_CRITICAL_SECTION;

            _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS );
        }

        /* If all safety checks completed, proceed. */
        if( _TASKPOOL_SUCCEEDED( _trySafeExtraction( pTaskPool, pJob, true ) ) )
        {
            _taskPoolTimerEvent_t * pTimerEvent = IotTaskPool_MallocTimerEvent( sizeof( _taskPoolTimerEvent_t ) );

            if( pTimerEvent == NULL )
            {
                _TASKPOOL_EXIT_CRITICAL_SECTION;

                _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS );
            }

            IotLink_t * pTimerEventLink;

            uint64_t now = IotClock_GetTimeMs();

            pTimerEvent->link.pNext = NULL;
            pTimerEvent->link.pPrevious = NULL;
            pTimerEvent->expirationTime = now + timeMs;
            pTimerEvent->pJob = pJob;

            /* Append the timer event to the timer list. */
            IotListDouble_InsertSorted( &pTaskPool->timerEventsList, &pTimerEvent->link, _timerEventCompare );

            /* Update the job status to 'scheduled'. */
            pJob->status &= ~IOT_TASKPOOL_STATUS_MASK;
            pJob->status |= IOT_TASKPOOL_STATUS_DEFERRED;

            /* Peek the first event in the timer event list. There must be at least one,
             * since we just inserted it. */
            pTimerEventLink = IotListDouble_PeekHead( &pTaskPool->timerEventsList );
            IotTaskPool_Assert( pTimerEventLink != NULL );

            /* If the event we inserted is at the front of the queue, then
             * we need to reschedule the underlying timer. */
            if( pTimerEventLink == &pTimerEvent->link )
            {
                pTimerEvent = IotLink_Container( _taskPoolTimerEvent_t, pTimerEventLink, link );

                _rescheduleDeferredJobsTimer( &pTaskPool->timer, pTimerEvent );
            }
        }
    }
    _TASKPOOL_EXIT_CRITICAL_SECTION;

    _TASKPOOL_NO_FUNCTION_CLEANUP();
}

/*-----------------------------------------------------------*/

IotTaskPoolError_t IotTaskPool_GetStatus( IotTaskPool_t * const pTaskPool,
                                          const IotTaskPoolJob_t * const pJob,
                                          IotTaskPoolJobStatus_t * const pStatus )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    /* Parameter checking. */
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pTaskPool );
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pJob );
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pStatus );

    _TASKPOOL_ENTER_CRITICAL_SECTION;
    {
        /* Bail out early if this task pool is shutting down. */
        if( _IsShutdownStarted( pTaskPool ) )
        {
            _TASKPOOL_EXIT_CRITICAL_SECTION;

            _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS );
        }

        *pStatus = ( pJob->status & IOT_TASKPOOL_STATUS_MASK );
    }
    _TASKPOOL_EXIT_CRITICAL_SECTION;

    _TASKPOOL_NO_FUNCTION_CLEANUP();
}

/*-----------------------------------------------------------*/

IotTaskPoolError_t IotTaskPool_TryCancel( IotTaskPool_t * const pTaskPool,
                                          IotTaskPoolJob_t * const pJob,
                                          IotTaskPoolJobStatus_t * const pStatus )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    /* Parameter checking. */
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pTaskPool );
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pJob );

    if( pStatus != NULL )
    {
        *pStatus = IOT_TASKPOOL_STATUS_UNDEFINED;
    }

    _TASKPOOL_ENTER_CRITICAL_SECTION;
    {
        /* Check again if this task pool is shutting down. */
        if( _IsShutdownStarted( pTaskPool ) )
        {
            _TASKPOOL_EXIT_CRITICAL_SECTION;

            _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS );
        }

        status = _tryCancelInternal( pTaskPool, pJob, pStatus );
    }
    _TASKPOOL_EXIT_CRITICAL_SECTION;

    _TASKPOOL_NO_FUNCTION_CLEANUP();
}

const char * IotTaskPool_strerror( IotTaskPoolError_t status )
{
    const char * pMessage = NULL;

    switch( status )
    {
        case IOT_TASKPOOL_SUCCESS:
            pMessage = "SUCCESS";
            break;
        case IOT_TASKPOOL_BAD_PARAMETER:
            pMessage = "BAD PARAMETER";
            break;
        case IOT_TASKPOOL_ILLEGAL_OPERATION:
            pMessage = "ILLEGAL OPERATION";
            break;
        case IOT_TASKPOOL_NO_MEMORY:
            pMessage = "NO MEMORY";
            break;
        case IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS:
            pMessage = "SHUTDOWN IN PROGRESS";
            break;
        case IOT_TASKPOOL_CANCEL_FAILED:
            pMessage = "CANCEL FAILED";
            break;
        default:
            pMessage = "INVALID STATUS";
            break;
    }

    return pMessage;
}

/* ---------------------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 */
static IotTaskPoolError_t _createTaskPool( const IotTaskPoolInfo_t * const pInfo,
                                           IotTaskPool_t * const pTaskPool )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    uint32_t count;
    uint32_t threadsCreated = 0;

    /* Check input values for consistency. */
    /* Parameter checking. */
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pTaskPool );
    _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( pInfo );
    _TASKPOOL_ON_ARG_ERROR_GOTO_CLEANUP( pInfo->minThreads > pInfo->maxThreads );
    _TASKPOOL_ON_ARG_ERROR_GOTO_CLEANUP( pInfo->minThreads < 1 );
    _TASKPOOL_ON_ARG_ERROR_GOTO_CLEANUP( pInfo->maxThreads < 1 );

    /* Initialize all internal data structure prior to creating all threads. */
    _TASKPOOL_ON_ERROR_GOTO_CLEANUP( _initTaskPoolControlStructures( pInfo, pTaskPool ) );

    /* Create the timer mutex for a new connection. */
    if( IotClock_TimerCreate( &( pTaskPool->timer ), _timerThread, pTaskPool ) == false )
    {
        IotLogError( "Failed to create timer for task pool." );

        _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_NO_MEMORY );
    }

    IotTaskPool_Assert( pInfo->minThreads == pTaskPool->minThreads );
    IotTaskPool_Assert( pInfo->maxThreads == pTaskPool->maxThreads );

    /* The task pool will initialize the minimum number of threads reqeusted by the user upon start. */
    /* When a thread is created, it will signal a semaphore to signify that it is about to wait on incoming */
    /* jobs. A thread can be woken up for exit or for new jobs only at that point in time.  */
    /* The exit condition is setting the maximum number of threads to 0. */

    /* Create the minimum number of threads specified by the user, and if one fails shutdown and return error. */
    for( ; threadsCreated < pTaskPool->minThreads; )
    {
        /* Create one thread. */
        if( Iot_CreateDetachedThread( _taskPoolWorker,
                                      pTaskPool,
                                      pTaskPool->priority,
                                      pTaskPool->stackSize ) == false )
        {
            IotLogError( "Could not create worker thread! Exiting..." );

            /* If creating one thread fails, set error condition and exit the loop. */
            _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_NO_MEMORY );
        }

        /* Upon successful thread creation, increase the number of active threads. */
        pTaskPool->activeThreads++;

        ++threadsCreated;
    }

    _TASKPOOL_FUNCTION_CLEANUP();

    /* Wait for threads to be ready to wait on the condition, so that threads are actually able to receive messages. */
    for( count = 0; count < threadsCreated; ++count )
    {
        IotSemaphore_Wait( &pTaskPool->startStopSignal );
    }

    /* In case of failure, wait on the created threads to exit. */
    if( _TASKPOOL_FAILED( status ) )
    {
        /* Set the exit condition for the newly created threads. */
        _signalShutdown( pTaskPool, threadsCreated );

        /* Signal all threads to exit. */
        for( count = 0; count < threadsCreated; ++count )
        {
            IotSemaphore_Wait( &pTaskPool->startStopSignal );
        }

        _destroyTaskPool( pTaskPool );
    }

    _TASKPOOL_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

static IotTaskPoolError_t _initTaskPoolControlStructures( const IotTaskPoolInfo_t * const pInfo,
                                                          IotTaskPool_t * const pTaskPool )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    bool semStartStopInit = false;
    bool lockInit = false;
    bool semDispatchInit = false;

    /* Zero out all data structures. */
    memset( ( void * ) pTaskPool, 0x00, sizeof( IotTaskPool_t ) );

    /* Initialize a job data structures that require no de-initialization.
     * All other data structures carry a value of 'NULL' before initailization.
     */
    IotQueue_Create( &pTaskPool->dispatchQueue );
    IotListDouble_Create( &pTaskPool->timerEventsList );

    pTaskPool->minThreads = pInfo->minThreads;
    pTaskPool->maxThreads = pInfo->maxThreads;
    pTaskPool->stackSize = pInfo->stackSize;
    pTaskPool->priority = pInfo->priority;

    _initJobsCache( &pTaskPool->jobsCache );

    /* Initialize the semaphore to ensure all threads have started. */
    if( IotSemaphore_Create( &pTaskPool->startStopSignal, 0, _TASKPOOL_MAX_SEM_VALUE ) )
    {
        semStartStopInit = true;

        if( IotMutex_Create( &pTaskPool->lock, true ) )
        {
            lockInit = true;

            /* Initialize the semaphore for waiting for incoming work. */
            if( IotSemaphore_Create( &pTaskPool->dispatchSignal, 0, _TASKPOOL_MAX_SEM_VALUE ) )
            {
                semDispatchInit = true;
            }
            else
            {
                _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_NO_MEMORY );
            }
        }
        else
        {
            _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_NO_MEMORY );
        }
    }
    else
    {
        _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_NO_MEMORY );
    }

    _TASKPOOL_FUNCTION_CLEANUP();

    if( _TASKPOOL_FAILED( status ) )
    {
        if( semStartStopInit )
        {
            IotSemaphore_Destroy( &pTaskPool->startStopSignal );
        }

        if( lockInit )
        {
            IotMutex_Destroy( &pTaskPool->lock );
        }

        if( semDispatchInit )
        {
            IotSemaphore_Destroy( &pTaskPool->dispatchSignal );
        }
    }

    _TASKPOOL_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

static void _destroyTaskPool( IotTaskPool_t * const pTaskPool )
{
    IotClock_TimerDestroy( &pTaskPool->timer );
    IotSemaphore_Destroy( &pTaskPool->dispatchSignal );
    IotSemaphore_Destroy( &pTaskPool->startStopSignal );
    IotMutex_Destroy( &pTaskPool->lock );
}

/* ---------------------------------------------------------------------------------------------- */

static void _taskPoolWorker( void * pUserContext )
{
    /* Extract pTaskPool pointer from context. */
    IotTaskPool_Assert( pUserContext != NULL );
    IotTaskPool_t * pTaskPool = ( IotTaskPool_t * ) pUserContext;

    /* Signal that this worker completed initialization and it is ready to receive notifications. */
    IotSemaphore_Post( &pTaskPool->startStopSignal );

    /* OUTER LOOP: it controls the lifetiem of the worker thread: exit condition for a worker thread
     * is setting maxThreads to zero. A worker thread is running until the maximum number of allowed
     * threads is not zero and the active threads are less than the maximum number of allowed threads.
     */
    for( ; ; )
    {
        IotLink_t * pFirst = NULL;
        IotTaskPoolJob_t * pJob = NULL;
        bool shouldExit = false;

        /* Wait on incoming notifications... */
        IotSemaphore_Wait( &pTaskPool->dispatchSignal );

        /* Acquire the lock to check the exit condition, and release the lock if the exit condition is verified,
         * or before waiting for incoming notifications.
         */
        _TASKPOOL_ENTER_CRITICAL_SECTION;
        {
            /* If the exit condition is verified, update the number of active threads and exit the loop. */
            if( _IsShutdownStarted( pTaskPool ) )
            {
                shouldExit = true;

                IotLogDebug( "Worker thread exiting because exit condition was set." );
            }
            else if( pTaskPool->activeThreads > pTaskPool->maxThreads )
            {
                shouldExit = true;

                IotLogDebug( "Worker thread exiting because maximum quota was exceeded." );
            }

            /* Check if thread should exit. */
            if( shouldExit )
            {
                /* Decrease the number of active threads. */
                pTaskPool->activeThreads--;

                _TASKPOOL_EXIT_CRITICAL_SECTION;

                /* Signal that this worker is exiting. */
                IotSemaphore_Post( &pTaskPool->startStopSignal );

                /* Abandon the OUTER LOOP. */
                break;
            }

            /* Dequeue the first job in FIFO order. */
            pFirst = IotQueue_Dequeue( &pTaskPool->dispatchQueue );

            /* If there is indeed a job, then update status under lock, and release the lock before processing the job. */
            if( pFirst != NULL )
            {
                /* Extract the job from its link. */
                pJob = IotLink_Container( IotTaskPoolJob_t, pFirst, link );

                /* Update status to 'executing'. */
                pJob->status &= ~IOT_TASKPOOL_STATUS_MASK;
                pJob->status |= IOT_TASKPOOL_STATUS_EXECUTING;
            }
        }
        _TASKPOOL_EXIT_CRITICAL_SECTION;

        /* INNER LOOP: it controls the execution of jobs: the exit condition is the lack of a job to execute. */
        while( pJob != NULL )
        {
            /* Record callback, so job can be re-used in the callback. */
            IotTaskPoolRoutine_t userCallback = pJob->userCallback;

            /* Process the job by invoking the associated callback with the user context.
             * This task pool thread will not be available until the user callback returns.
             */
            {
                IotTaskPool_Assert( IotLink_IsLinked( &pJob->link ) == false );

                userCallback( pTaskPool, pJob, pJob->pUserContext );

                /* This job is finished, clear its pointer. */
                pJob = NULL;
            }

            /* Acquire the lock before updating the job status. */
            _TASKPOOL_ENTER_CRITICAL_SECTION;
            {
                /* Update the number of busy threads, so new requests can be served by creating new threads, up to maxThreads. */
                pTaskPool->activeJobs--;

                /* Try and dequeue the next job in the dispatch queue. */
                IotLink_t * pItem = NULL;

                /* Dequeue the next job from the dispatch queue. */
                pItem = IotQueue_Dequeue( &pTaskPool->dispatchQueue );

                /* If there is no job left in the dispatch queue, update the worker status and leave. */
                if( pItem == NULL )
                {
                    _TASKPOOL_EXIT_CRITICAL_SECTION;

                    /* Abandon the INNER LOOP. Execution will tranfer back to the OUTER LOOP condition. */
                    break;
                }
                else
                {
                    pJob = IotLink_Container( IotTaskPoolJob_t, pItem, link );
                }

                pJob->status &= ~IOT_TASKPOOL_STATUS_MASK;
                pJob->status |= IOT_TASKPOOL_STATUS_EXECUTING;
            }
            _TASKPOOL_EXIT_CRITICAL_SECTION;
        }
    }
}

/* ---------------------------------------------------------------------------------------------- */

static void _initJobsCache( IotTaskPoolCache_t * const pCache )
{
    IotQueue_Create( &pCache->freeList );

    pCache->freeCount = 0;
}

/*-----------------------------------------------------------*/

static IotTaskPoolJob_t * _fetchOrAllocateJob( IotTaskPoolCache_t * const pCache )
{
    IotTaskPoolJob_t * pJob = NULL;
    IotLink_t * pLink = IotListDouble_RemoveHead( &( pCache->freeList ) );

    if( pLink != NULL )
    {
        pJob = IotLink_Container( IotTaskPoolJob_t, pLink, link );
    }

    /* If there is no available job in the cache, then allocate one. */
    if( pJob == NULL )
    {
        pJob = ( IotTaskPoolJob_t * ) IotTaskPool_MallocJob( sizeof( IotTaskPoolJob_t ) );

        if( pJob != NULL )
        {
            memset( pJob, 0x00, sizeof( IotTaskPoolJob_t ) );
        }
        else
        {
            IotLogInfo( "Failed to allocate job." );
        }
    }
    /* If there was a job in the cache, then make sure we keep the counters up-to-date. */
    else
    {
        IotTaskPool_Assert( pCache->freeCount > 0 );

        pCache->freeCount--;
    }

    return pJob;
}

/*-----------------------------------------------------------*/

static void _recycleJob( IotTaskPoolCache_t * const pCache,
                         IotTaskPoolJob_t * const pJob )
{
    /* We should never try and recycling a job that is linked into some queue. */
    IotTaskPool_Assert( IotLink_IsLinked( &pJob->link ) == false );

    /* We will recycle the job if there is space in the cache. */
    if( pCache->freeCount < IOT_TASKPOOL_JOBS_RECYCLE_LIMIT )
    {
        /* Destroy user data, for added safety&security. */
        pJob->userCallback = NULL;
        pJob->pUserContext = NULL;

        /* Reset the status for added debuggability. */
        pJob->status &= ~IOT_TASKPOOL_STATUS_MASK;
        pJob->status |= IOT_TASKPOOL_STATUS_UNDEFINED;

        IotListDouble_InsertTail( &pCache->freeList, &pJob->link );

        pCache->freeCount++;

        IotTaskPool_Assert( pCache->freeCount >= 1 );
    }
    else
    {
        _destroyJob( pJob );
    }
}

/*-----------------------------------------------------------*/

static void _destroyJob( IotTaskPoolJob_t * const pJob )
{
    /* Destroy user data, for added safety&security. */
    pJob->userCallback = NULL;
    pJob->pUserContext = NULL;

    /* Reset the status for added debuggability. */
    pJob->status &= ~IOT_TASKPOOL_STATUS_MASK;
    pJob->status |= IOT_TASKPOOL_STATUS_UNDEFINED;

    /* Only dispose of dynamically allocated jobs. */
    if( ( pJob->status & IOT_TASK_POOL_INTERNAL_STATIC ) == 0 )
    {
        IotTaskPool_FreeJob( pJob );
    }
}

/* ---------------------------------------------------------------------------------------------- */

static bool _IsShutdownStarted( const IotTaskPool_t * const pTaskPool )
{
    return( pTaskPool->maxThreads == 0 );
}

/*-----------------------------------------------------------*/

static void _signalShutdown( IotTaskPool_t * const pTaskPool,
                             uint32_t threads )
{
    uint32_t count;

    /* Set the exit condition. */
    pTaskPool->maxThreads = 0;

    /* Broadcast to all active threads to wake-up. Active threads do check the exit condition right after wakein up. */
    for( count = 0; count < threads; ++count )
    {
        IotSemaphore_Post( &pTaskPool->dispatchSignal );
    }
}

/* ---------------------------------------------------------------------------------------------- */

static IotTaskPoolError_t _scheduleInternal( IotTaskPool_t * const pTaskPool,
                                             IotTaskPoolJob_t * const pJob )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    /* Update the job status to 'scheduled'. */
    pJob->status &= ~IOT_TASKPOOL_STATUS_MASK;
    pJob->status |= IOT_TASKPOOL_STATUS_SCHEDULED;

    /* Append the job to the dispatch queue. */
    IotQueue_Enqueue( &pTaskPool->dispatchQueue, &pJob->link );

    /* Update the number of busy threads, so new requests can be served by creating new threads, up to maxThreads. */
    pTaskPool->activeJobs++;

    /* If all threads are busy, try and create a new one. Failing to create a new thread
     * only has performance implications on correctly exeuting th scheduled job.
     */
    uint32_t activeThreads = pTaskPool->activeThreads;

    if( activeThreads == pTaskPool->activeJobs )
    {
        /* Grow the task pool up to the maximum number of threads indicated by the user.
         * Growing the taskpool can safely fail, the existing threads will eventually pick up
         * the job sometimes later. */
        if( activeThreads < pTaskPool->maxThreads )
        {
            IotLogInfo( "Growing a Task pool with a new worker thread..." );

            if( Iot_CreateDetachedThread( _taskPoolWorker,
                                          pTaskPool,
                                          pTaskPool->priority,
                                          pTaskPool->stackSize ) )
            {
                IotSemaphore_Wait( &pTaskPool->startStopSignal );

                pTaskPool->activeThreads++;
            }
            else
            {
                /* Failure to create a worker thread does not hinder functional correctness, but rather just responsiveness. */
                IotLogWarn( "Task pool failed to create a worker thread." );
            }
        }
    }

    /* Signal a worker to pick up the job. */
    IotSemaphore_Post( &pTaskPool->dispatchSignal );

    _TASKPOOL_NO_FUNCTION_CLEANUP_NOLABEL();
}

/*-----------------------------------------------------------*/

static bool _matchJobByPointer( const IotLink_t * pLink,
                                void * pMatch )
{
    const IotTaskPoolJob_t * pJob = ( IotTaskPoolJob_t * ) pMatch;

    const _taskPoolTimerEvent_t * pTimerEvent = IotLink_Container( _taskPoolTimerEvent_t, pLink, link );

    if( pJob == pTimerEvent->pJob )
    {
        return true;
    }

    return false;
}

/*-----------------------------------------------------------*/

static IotTaskPoolError_t _tryCancelInternal( IotTaskPool_t * const pTaskPool,
                                              IotTaskPoolJob_t * const pJob,
                                              IotTaskPoolJobStatus_t * const pStatus )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    bool cancelable = false;

    /* We can only cancel jobs that are either 'ready' (waiting to be scheduled). 'deferred', or 'scheduled'. */

    IotTaskPoolJobStatus_t currentStatus = ( pJob->status & IOT_TASKPOOL_STATUS_MASK );

    switch( currentStatus )
    {
        case IOT_TASKPOOL_STATUS_READY:
        case IOT_TASKPOOL_STATUS_DEFERRED:
        case IOT_TASKPOOL_STATUS_SCHEDULED:
            cancelable = true;
            break;

        case IOT_TASKPOOL_STATUS_EXECUTING:
        case IOT_TASKPOOL_STATUS_CANCELED:
            IotLogWarn( "Attempt to cancel a job that is already executing, or canceled." );
            break;

        default:
            IotLogError( "Attempt to cancel a job with an undefined state." );
            break;
    }

    /* Update the returned status to the current status of the job. */
    if( pStatus != NULL )
    {
        *pStatus = currentStatus;
    }

    if( cancelable == false )
    {
        _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_CANCEL_FAILED );
    }
    else
    {
        /* Update the status of the job. */
        pJob->status &= ~IOT_TASKPOOL_STATUS_MASK;
        pJob->status |= IOT_TASKPOOL_STATUS_CANCELED;

        /* If the job is cancelable and its current status is 'scheduled' then unlink it from the dispatch
         * queue and signal any waiting threads. */
        if( currentStatus == IOT_TASKPOOL_STATUS_SCHEDULED )
        {
            /* A scheduled work items must be in the dispatch queue. */
            IotTaskPool_Assert( IotLink_IsLinked( &pJob->link ) );

            IotQueue_Remove( &pJob->link );
        }

        /* If the job current status is 'deferred' then the job has to be pending
         * in the timeouts queue. */
        else if( currentStatus == IOT_TASKPOOL_STATUS_DEFERRED )
        {
            /* Find the timer event associated with the current job. There MUST be one, hence assert if not. */
            IotLink_t * pTimerEventLink = IotListDouble_FindFirstMatch( &pTaskPool->timerEventsList, NULL, _matchJobByPointer, pJob );
            IotTaskPool_Assert( pTimerEventLink != NULL );

            if( pTimerEventLink != NULL )
            {
                bool shouldReschedule = false;

                /* If the job being cancelled was at the head of the timeouts queue, then we need to reschedule the timer
                 * with the next job timeout */
                IotLink_t * pHeadLink = IotListDouble_PeekHead( &pTaskPool->timerEventsList );

                if( pHeadLink == pTimerEventLink )
                {
                    shouldReschedule = true;
                }

                /* Remove the timer event associated with the canceled job and free the associated memory. */
                IotListDouble_Remove( pTimerEventLink );
                IotTaskPool_FreeTimerEvent( IotLink_Container( _taskPoolTimerEvent_t, pTimerEventLink, link ) );

                if( shouldReschedule )
                {
                    IotLink_t * pNextTimerEventLink = IotListDouble_PeekHead( &pTaskPool->timerEventsList );

                    if( pNextTimerEventLink != NULL )
                    {
                        _rescheduleDeferredJobsTimer( &pTaskPool->timer, IotLink_Container( _taskPoolTimerEvent_t, pNextTimerEventLink, link ) );
                    }
                }
            }
        }
    }

    _TASKPOOL_NO_FUNCTION_CLEANUP();
}

/*-----------------------------------------------------------*/

static IotTaskPoolError_t _trySafeExtraction( IotTaskPool_t * const pTaskPool,
                                              IotTaskPoolJob_t * const pJob,
                                              bool checkExecutionInProgress )
{
    _TASKPOOL_FUNCTION_ENTRY( IOT_TASKPOOL_SUCCESS );

    IotTaskPoolJobStatus_t jobStatus = pJob->status & IOT_TASKPOOL_STATUS_MASK;

    /* if the job is executing, we cannot touch it. */
    if( checkExecutionInProgress && ( jobStatus == IOT_TASKPOOL_STATUS_EXECUTING ) )
    {
        _TASKPOOL_SET_AND_GOTO_CLEANUP( IOT_TASKPOOL_ILLEGAL_OPERATION );
    }
    /* Do not destroy a job in the dispatch queue or the timer queue without cancelling first. */
    else if( ( jobStatus == IOT_TASKPOOL_STATUS_SCHEDULED ) || ( jobStatus == IOT_TASKPOOL_STATUS_DEFERRED ) )
    {
        IotTaskPoolJobStatus_t statusAtCancellation;

        /* Cancellation can fail, e.g. if a job is being executed when we are trying to cancel it. */
        jobStatus = _tryCancelInternal( pTaskPool, pJob, &statusAtCancellation );

        switch( jobStatus )
        {
            case IOT_TASKPOOL_SUCCESS:
                break;

            case IOT_TASKPOOL_CANCEL_FAILED:
                IotLogWarn( "Removing a scheduled job failed because the job could not be canceled." );
                jobStatus = IOT_TASKPOOL_ILLEGAL_OPERATION;
                break;

            default:
                break;
        }
    }
    else if( IotLink_IsLinked( &pJob->link ) )
    {
        /* If the job is not in the dispatch or timer queue, it must be in the cache. */
        IotTaskPool_Assert( ( pJob->jobStatus & IOT_TASK_POOL_INTERNAL_STATIC ) == 0 );

        IotListDouble_Remove( &pJob->link );
    }

    _TASKPOOL_NO_FUNCTION_CLEANUP();
}

/*-----------------------------------------------------------*/

static int32_t _timerEventCompare( const IotLink_t * const pTimerEventLink1,
                                   const IotLink_t * const pTimerEventLink2 )
{
    const _taskPoolTimerEvent_t * pTimerEvent1 = IotLink_Container( _taskPoolTimerEvent_t,
                                                                    pTimerEventLink1,
                                                                    link );
    const _taskPoolTimerEvent_t * pTimerEvent2 = IotLink_Container( _taskPoolTimerEvent_t,
                                                                    pTimerEventLink2,
                                                                    link );

    if( pTimerEvent1->expirationTime < pTimerEvent2->expirationTime )
    {
        return -1;
    }

    if( pTimerEvent1->expirationTime > pTimerEvent2->expirationTime )
    {
        return 1;
    }

    return 0;
}

/*-----------------------------------------------------------*/

static void _rescheduleDeferredJobsTimer( IotTimer_t * const pTimer,
                                          _taskPoolTimerEvent_t * const pFirstTimerEvent )
{
    uint64_t delta = 0;
    uint64_t now = IotClock_GetTimeMs();

    if( pFirstTimerEvent->expirationTime > now )
    {
        delta = pFirstTimerEvent->expirationTime - now;
    }

    if( delta < _TASKPOOL_JOB_RESCHEDULE_DELAY_MS )
    {
        delta = _TASKPOOL_JOB_RESCHEDULE_DELAY_MS; /* The job will be late... */
    }

    IotTaskPool_Assert( delta > 0 );

    if( IotClock_TimerArm( pTimer, delta, 0 ) == false )
    {
        IotLogWarn( "Failed to re-arm timer for task pool" );
    }
}

/*-----------------------------------------------------------*/

static void _timerThread( void * pArgument )
{
    IotTaskPool_t * pTaskPool = ( IotTaskPool_t * ) pArgument;
    _taskPoolTimerEvent_t * pTimerEvent = NULL;

    IotLogDebug( "Timer thread started for task pool %p.", pTaskPool );

    /* Attempt to lock the timer mutex. Return immediately if the mutex cannot be locked.
     * If this mutex cannot be locked it means that another thread is manipulating the
     * timeouts list, and will reset the timer to fire again, although it will be late.
     */
    if( _TASKPOOL_TRY_ENTER_CRITICAL_SECTION == false )
    {
        IotLogWarn( "Failed to lock timer mutex in timer thread. Rescheduling deferred job." );

        IotClock_TimerArm( &pTaskPool->timer, _TASKPOOL_JOB_RESCHEDULE_DELAY_MS, 0 );

        return;
    }
    else
    {
        /* Check again for shutdown and bail out early in case. */
        if( _IsShutdownStarted( pTaskPool ) )
        {
            _TASKPOOL_EXIT_CRITICAL_SECTION;

            return;
        }

        /* Dispatch all deferred job whose timer expired, then reset the timer for the next
         * job down the line. */
        while( true )
        {
            /* Peek the first event in the timer event list. */
            IotLink_t * pLink = IotListDouble_PeekHead( &pTaskPool->timerEventsList );

            /* Check if the timer misfired for any reason.  */
            if( pLink != NULL )
            {
                /* Record the current time. */
                uint64_t now = IotClock_GetTimeMs();

                /* Extract the job from its envelope. */
                pTimerEvent = IotLink_Container( _taskPoolTimerEvent_t, pLink, link );

                /* Check if the first event should be processed now. */
                if( pTimerEvent->expirationTime <= now )
                {
                    /*  Remove the timer event for immediate processing. */
                    IotListDouble_Remove( &( pTimerEvent->link ) );
                }
                else
                {
                    /* The first element in the timer queue shouldn't be processed yet.
                     * Arm the timer for when it should be processed and leave altogether. */
                    _rescheduleDeferredJobsTimer( &pTaskPool->timer, pTimerEvent );

                    break;
                }
            }
            /* If there are no timer events to process, terminate this thread. */
            else
            {
                IotLogDebug( "No further timer events to process. Exiting timer thread." );

                break;
            }

            IotLogDebug( "Scheduling job from timer event." );

            /* Queue the job associated with the received timer event. */
            _scheduleInternal( pTaskPool, pTimerEvent->pJob );

            /* Free the timer event. */
            IotTaskPool_FreeTimerEvent( pTimerEvent );
        }
    }

    _TASKPOOL_EXIT_CRITICAL_SECTION;
}

/** @endcond */
