/*
 * FreeRTOS Common V1.2.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file iot_tests_taskpool.c
 * @brief Tests for task pool.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* SDK initialization include. */
#include "iot_init.h"

/* Platform layer includes. */
#include "platform/iot_threads.h"
#include "platform/iot_clock.h"

/* Task pool internal include. */
#include "private/iot_taskpool_internal.h"

/* Task pool include. */
#include "iot_taskpool.h"

/* Test framework includes. */
#include "unity_fixture.h"

/*-----------------------------------------------------------*/

/**
 * @brief A simple user context to prove all callbacks are called.
 */
typedef struct JobUserContext
{
    IotMutex_t lock;  /**< @brief Protection from concurrent updates. */
    uint32_t counter; /**< @brief A counter to keep track of callback invocations. */
} JobUserContext_t;

/**
 * @brief A simple user context to prove the taskpool grows as expected.
 */
typedef struct JobBlockingUserContext
{
    IotSemaphore_t signal; /**< @brief A synch object to signal. */
    IotSemaphore_t block;  /**< @brief A synch object to wait on. */
} JobBlockingUserContext_t;

/*-----------------------------------------------------------*/

/**
 * @brief Test group for task pool tests.
 */
TEST_GROUP( Common_Unit_Task_Pool );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for task pool tests.
 */
TEST_SETUP( Common_Unit_Task_Pool )
{
    TEST_ASSERT_EQUAL_INT( true, IotSdk_Init() );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for task pool tests.
 */
TEST_TEAR_DOWN( Common_Unit_Task_Pool )
{
    IotSdk_Cleanup();
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for task pool.
 */
TEST_GROUP_RUNNER( Common_Unit_Task_Pool )
{
    RUN_TEST_CASE( Common_Unit_Task_Pool, Error );
    RUN_TEST_CASE( Common_Unit_Task_Pool, CreateDestroyMaxThreads );
    RUN_TEST_CASE( Common_Unit_Task_Pool, CreateDestroyJobError );
    RUN_TEST_CASE( Common_Unit_Task_Pool, CreateDestroyRecycleRecyclableJobError );
    RUN_TEST_CASE( Common_Unit_Task_Pool, CreateRecyclableJob );
    RUN_TEST_CASE( Common_Unit_Task_Pool, ScheduleTasksError );
    RUN_TEST_CASE( Common_Unit_Task_Pool, ScheduleTasks_LongRunningAndCachedJobsAndDestroy );
    RUN_TEST_CASE( Common_Unit_Task_Pool, ScheduleTasks_Grow );
    RUN_TEST_CASE( Common_Unit_Task_Pool, ScheduleTasks_GrowHighPri );
    RUN_TEST_CASE( Common_Unit_Task_Pool, ScheduleTasks_ScheduleOneThenWait );
    RUN_TEST_CASE( Common_Unit_Task_Pool, ScheduleTasks_ScheduleOneDeferredThenWait );
    RUN_TEST_CASE( Common_Unit_Task_Pool, ScheduleTasks_ScheduleAllThenWait );
    RUN_TEST_CASE( Common_Unit_Task_Pool, ScheduleTasks_ScheduleAllRecyclableThenWait );
    RUN_TEST_CASE( Common_Unit_Task_Pool, ScheduleTasks_ScheduleAllDeferredRecyclableThenWait );
    RUN_TEST_CASE( Common_Unit_Task_Pool, ScheduleTasks_ReSchedule );
    RUN_TEST_CASE( Common_Unit_Task_Pool, ScheduleTasks_ReScheduleDeferred );
    RUN_TEST_CASE( Common_Unit_Task_Pool, ScheduleTasks_CancelTasks );
}

/*-----------------------------------------------------------*/

/**
 * @brief Number of iterations for each test loop.
 */
#ifndef TEST_TASKPOOL_ITERATIONS
    #define TEST_TASKPOOL_ITERATIONS    ( 200 )
#endif

/**
 * @brief Define the stress job max duration time (emulated duration).
 */
#ifndef TEST_TASKPOOL_WORK_ITEM_DURATION_MAX
    #define TEST_TASKPOOL_WORK_ITEM_DURATION_MAX    ( 55 )
#endif

/**
 * @brief Define the number of long running jobs.
 */
#ifndef TEST_TASKPOOL_LONG_JOBS_NUMBER
    #define TEST_TASKPOOL_LONG_JOBS_NUMBER    3
#endif

/**
 * @brief Define the number of running jobs to grow the taskpool for.
 */
#ifndef TEST_TASKPOOL_NUMBER_OF_JOBS
    #define TEST_TASKPOOL_NUMBER_OF_JOBS    4
#endif

/**
 * @brief Define the number of threads to grow the taskpool to.
 */
#ifndef TEST_TASKPOOL_NUMBER_OF_THREADS
    #define TEST_TASKPOOL_NUMBER_OF_THREADS    ( TEST_TASKPOOL_NUMBER_OF_JOBS - 1 )
#endif

/**
 * @brief Define the number of threads to grow the taskpool to.
 */
#ifndef TEST_TASKPOOL_MAX_THREADS
    #define TEST_TASKPOOL_MAX_THREADS    7
#endif

/**
 * @brief Define the number of long running jobs.
 */
#ifndef TEST_TASKPOOL_LONG_JOBS_NUMBER
    #define TEST_TASKPOOL_LONG_JOBS_NUMBER    3
#endif

/**
 * @brief Define the number of running jobs to grow the taskpool for.
 */
#ifndef TEST_TASKPOOL_NUMBER_OF_JOBS
    #define TEST_TASKPOOL_NUMBER_OF_JOBS    4
#endif

/**
 * @brief Define the number of threads to grow the taskpool to.
 */
#ifndef TEST_TASKPOOL_NUMBER_OF_THREADS
    #define TEST_TASKPOOL_NUMBER_OF_THREADS    ( TEST_TASKPOOL_NUMBER_OF_JOBS - 1 )
#endif

/**
 * @brief Define the number of threads to grow the taskpool to.
 */
#ifndef TEST_TASKPOOL_MAX_THREADS
    #define TEST_TASKPOOL_MAX_THREADS    7
#endif

/**
 * @brief One hour in milliseconds.
 */
#define ONE_HOUR_FROM_NOW_MS    ( 3600 * 1000 )

/* ---------------------------------------------------------- */

/**
 * @brief A function that emulates some work in the task pool execution by sleeping.
 */
static void EmulateWork()
{
    IotClock_SleepMs( rand() % TEST_TASKPOOL_WORK_ITEM_DURATION_MAX );
}

/**
 * @brief A function that emulates some work in the task pool execution by sleeping.
 */
static void EmulateWorkLong()
{
    IotClock_SleepMs( 2000 + ( rand() % TEST_TASKPOOL_WORK_ITEM_DURATION_MAX ) );
}

/**
 * @brief A callback that does not recycle its job.
 */
static void ExecutionWithoutDestroyCb( IotTaskPool_t pTaskPool,
                                       IotTaskPoolJob_t pJob,
                                       void * pContext )
{
    JobUserContext_t * pUserContext;
    IotTaskPoolError_t error;
    IotTaskPoolJobStatus_t status;

    /*TEST_ASSERT( IotLink_IsLinked( &pJob->link ) == false ); */

    error = IotTaskPool_GetStatus( pTaskPool, pJob, &status );
    TEST_ASSERT( ( status == IOT_TASKPOOL_STATUS_COMPLETED ) || ( status == IOT_TASKPOOL_STATUS_UNDEFINED ) );
    TEST_ASSERT( ( error == IOT_TASKPOOL_SUCCESS ) || ( error == IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS ) );

    EmulateWork();

    pUserContext = ( JobUserContext_t * ) pContext;

    IotMutex_Lock( &pUserContext->lock );
    pUserContext->counter++;
    IotMutex_Unlock( &pUserContext->lock );
}

/**
 * @brief A callback that blocks.
 */
static void ExecutionBlockingWithoutDestroyCb( IotTaskPool_t pTaskPool,
                                               IotTaskPoolJob_t pJob,
                                               void * pContext )
{
    JobBlockingUserContext_t * pUserContext;
    IotTaskPoolError_t error;
    IotTaskPoolJobStatus_t status;

    /*TEST_ASSERT( IotLink_IsLinked( &pJob->link ) == false ); */

    error = IotTaskPool_GetStatus( pTaskPool, pJob, &status );
    TEST_ASSERT( ( status == IOT_TASKPOOL_STATUS_COMPLETED ) || ( status == IOT_TASKPOOL_STATUS_UNDEFINED ) );
    TEST_ASSERT( ( error == IOT_TASKPOOL_SUCCESS ) || ( error == IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS ) );

    pUserContext = ( JobBlockingUserContext_t * ) pContext;

    /* Signal that the callback has been called. */
    IotSemaphore_Post( &pUserContext->signal );

    /* This callback will emulate a blocking wait, for the sole purpose of stealing a task pool
     * thread and test that the taskpool can actually grow as expected. */
    IotSemaphore_Wait( &pUserContext->block );
}

/**
 * @brief A callback that recycles its job.
 */
static void ExecutionWithRecycleCb( IotTaskPool_t pTaskPool,
                                    IotTaskPoolJob_t pJob,
                                    void * pContext )
{
    JobUserContext_t * pUserContext;
    IotTaskPoolError_t error;
    IotTaskPoolJobStatus_t status;

    /*TEST_ASSERT( IotLink_IsLinked( &pJob->link ) == false ); */

    error = IotTaskPool_GetStatus( pTaskPool, pJob, &status );
    TEST_ASSERT( ( status == IOT_TASKPOOL_STATUS_COMPLETED ) || ( status == IOT_TASKPOOL_STATUS_UNDEFINED ) );
    TEST_ASSERT( ( error == IOT_TASKPOOL_SUCCESS ) || ( error == IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS ) );

    EmulateWork();

    pUserContext = ( JobUserContext_t * ) pContext;

    IotMutex_Lock( &pUserContext->lock );
    pUserContext->counter++;
    IotMutex_Unlock( &pUserContext->lock );

    IotTaskPool_RecycleJob( pTaskPool, pJob );
}

/**
 * @brief A callback that takes a long time and does not recycle its job.
 */
static void ExecutionLongWithoutDestroyCb( IotTaskPool_t pTaskPool,
                                           IotTaskPoolJob_t pJob,
                                           void * pContext )
{
    JobUserContext_t * pUserContext;
    IotTaskPoolError_t error;
    IotTaskPoolJobStatus_t status;

    /*TEST_ASSERT( IotLink_IsLinked( &pJob->link ) == false ); */

    error = IotTaskPool_GetStatus( pTaskPool, pJob, &status );
    TEST_ASSERT( ( status == IOT_TASKPOOL_STATUS_COMPLETED ) || ( status == IOT_TASKPOOL_STATUS_UNDEFINED ) );
    TEST_ASSERT( ( error == IOT_TASKPOOL_SUCCESS ) || ( error == IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS ) );

    EmulateWorkLong();

    pUserContext = ( JobUserContext_t * ) pContext;

    IotMutex_Lock( &pUserContext->lock );
    pUserContext->counter++;
    IotMutex_Unlock( &pUserContext->lock );
}

/**
 * @brief A callback that does not recycle its job.
 */
static void BlankExecution( IotTaskPool_t pTaskPool,
                            IotTaskPoolJob_t pJob,
                            void * pContext )
{
    IotTaskPoolError_t error;
    IotTaskPoolJobStatus_t status;

    ( void ) pTaskPool;
    ( void ) pJob;
    ( void ) pContext;

    /*TEST_ASSERT( IotLink_IsLinked( &pJob->link ) == false ); */

    error = IotTaskPool_GetStatus( pTaskPool, pJob, &status );
    TEST_ASSERT( ( status == IOT_TASKPOOL_STATUS_COMPLETED ) || ( status == IOT_TASKPOOL_STATUS_UNDEFINED ) );
    TEST_ASSERT( ( error == IOT_TASKPOOL_SUCCESS ) || ( error == IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS ) );
}

/* ---------------------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief Number of legal task pool initialization configurations.
 */
#define LEGAL_INFOS      3

/**
 * @brief Number of illegal task pool initialization configurations.
 */
#define ILLEGAL_INFOS    3

/**
 * @brief Legal initialization configurations.
 */
static IotTaskPoolInfo_t tpInfoLegal[ LEGAL_INFOS ] =
{
    { .minThreads = 1, .maxThreads = 1, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY },
    { .minThreads = 1, .maxThreads = 2, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY },
    { .minThreads = 2, .maxThreads = 3, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY }
};

/**
 * @brief Illegal initialization configurations.
 */
static IotTaskPoolInfo_t tpInfoIllegal[ ILLEGAL_INFOS ] =
{
    { .minThreads = 0, .maxThreads = 1, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY },
    { .minThreads = 1, .maxThreads = 0, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY },
    { .minThreads = 2, .maxThreads = 1, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY }
};

/*-----------------------------------------------------------*/

/**
 * @brief Test retrieving error string for each task pool status error.
 */
TEST( Common_Unit_Task_Pool, Error )
{
    IotTaskPoolError_t error;
    const char * errorString = NULL;

    /* Set error to all possible values, and test that the corresponding string is as expected. */
    /* NOTE: by convention, 'success' value must be equal to zero (0). */
    error = IOT_TASKPOOL_SUCCESS;
    errorString = IotTaskPool_strerror( error );
    TEST_ASSERT( 0 == strncmp( errorString, "SUCCESS", strlen( "SUCCESS" ) ) );

    error = IOT_TASKPOOL_BAD_PARAMETER;
    errorString = IotTaskPool_strerror( error );
    TEST_ASSERT( 0 == strncmp( errorString, "BAD PARAMETER", strlen( "BAD PARAMETER" ) ) );

    error = IOT_TASKPOOL_ILLEGAL_OPERATION;
    errorString = IotTaskPool_strerror( error );
    TEST_ASSERT( 0 == strncmp( errorString, "ILLEGAL OPERATION", strlen( "ILLEGAL OPERATION" ) ) );

    error = IOT_TASKPOOL_NO_MEMORY;
    errorString = IotTaskPool_strerror( error );
    TEST_ASSERT( 0 == strncmp( errorString, "NO MEMORY", strlen( "NO MEMORY" ) ) );

    error = IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS;
    errorString = IotTaskPool_strerror( error );
    TEST_ASSERT( 0 == strncmp( errorString, "SHUTDOWN IN PROGRESS", strlen( "SHUTDOWN IN PROGRESS" ) ) );

    error = IOT_TASKPOOL_CANCEL_FAILED;
    errorString = IotTaskPool_strerror( error );
    TEST_ASSERT( 0 == strncmp( errorString, "CANCEL FAILED", strlen( "CANCEL FAILED" ) ) );

    error = ( IotTaskPoolError_t ) -1;
    errorString = IotTaskPool_strerror( error );
    TEST_ASSERT( 0 == strncmp( errorString, "INVALID STATUS", strlen( "INVALID STATUS" ) ) );
}

/**
 * @brief Test task pool dynamic memory creation and destruction, with both legal and illegal information.
 */
TEST( Common_Unit_Task_Pool, CreateDestroyMaxThreads )
{
    uint32_t count;
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;

    /* Some legal and illegal create/destroy patterns. */
    for( count = 0; count < LEGAL_INFOS; ++count )
    {
        TEST_ASSERT( IotTaskPool_Create( &tpInfoLegal[ count ], &taskPool ) == IOT_TASKPOOL_SUCCESS );
        TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );
    }

    for( count = 0; count < ILLEGAL_INFOS; ++count )
    {
        TEST_ASSERT( IotTaskPool_Create( &tpInfoIllegal[ count ], &taskPool ) == IOT_TASKPOOL_BAD_PARAMETER );
    }

    TEST_ASSERT( IotTaskPool_Create( &tpInfoLegal[ 0 ], NULL ) == IOT_TASKPOOL_BAD_PARAMETER );
    TEST_ASSERT( IotTaskPool_Create( NULL, &taskPool ) == IOT_TASKPOOL_BAD_PARAMETER );

    /* Create a task pool a tweak max threads up & down. */
    {
        uint32_t count;
        IotTaskPoolJobStorage_t jobsStorage[ 2 * TEST_TASKPOOL_MAX_THREADS ];
        IotTaskPoolJob_t jobs[ 2 * TEST_TASKPOOL_MAX_THREADS ];
        IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = 6, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };

        TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

        if( TEST_PROTECT() )
        {
            TEST_ASSERT( IotTaskPool_SetMaxThreads( taskPool, 5 ) == IOT_TASKPOOL_SUCCESS );       /* lower. */
            TEST_ASSERT( IotTaskPool_SetMaxThreads( taskPool, 3 ) == IOT_TASKPOOL_SUCCESS );       /* lower. */
            TEST_ASSERT( IotTaskPool_SetMaxThreads( taskPool, 4 ) == IOT_TASKPOOL_SUCCESS );       /* higher. */
            TEST_ASSERT( IotTaskPool_SetMaxThreads( taskPool, 7 ) == IOT_TASKPOOL_SUCCESS );       /* higher. */
            TEST_ASSERT( IotTaskPool_SetMaxThreads( taskPool, 1 ) == IOT_TASKPOOL_BAD_PARAMETER ); /* less than min threads. */

            /* Initialize more jobs than max threads. */
            for( count = 0; count < 2 * TEST_TASKPOOL_MAX_THREADS; ++count )
            {
                /* Create legal recyclable job. */
                TEST_ASSERT( IotTaskPool_CreateJob( &BlankExecution, NULL, &jobsStorage[ count ], &jobs[ count ] ) == IOT_TASKPOOL_SUCCESS );
            }

            /* Schedule all jobs to make the task pool grow. */
            for( count = 0; count < 2 * TEST_TASKPOOL_MAX_THREADS; ++count )
            {
                /* Create legal recyclable job. */
                TEST_ASSERT( IotTaskPool_Schedule( taskPool, jobs[ count ], 0 ) == IOT_TASKPOOL_SUCCESS );
            }
        }

        TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Test task pool job static and dynamic memory creation with bogus parameters.
 */
TEST( Common_Unit_Task_Pool, CreateDestroyJobError )
{
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = 3, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        /* Trivial parameter validation. */
        {
            IotTaskPoolJobStorage_t jobStorage;
            IotTaskPoolJob_t job;

            /* NULL callback. */
            TEST_ASSERT( IotTaskPool_CreateJob( NULL, NULL, &jobStorage, &job ) == IOT_TASKPOOL_BAD_PARAMETER );
            /* NULL job handle. */
            TEST_ASSERT( IotTaskPool_CreateJob( &BlankExecution, NULL, &jobStorage, NULL ) == IOT_TASKPOOL_BAD_PARAMETER );
            /* NULL storage pointer. */
            TEST_ASSERT( IotTaskPool_CreateJob( &BlankExecution, NULL, NULL, &job ) == IOT_TASKPOOL_BAD_PARAMETER );
        }

        /* Create/Destroy. */
        {
            IotTaskPoolJobStorage_t jobStorage;
            IotTaskPoolJob_t job;

            /* Create legal static job. */
            TEST_ASSERT( IotTaskPool_CreateJob( &BlankExecution, NULL, &jobStorage, &job ) == IOT_TASKPOOL_SUCCESS );
            /* Illegally recycle legal static job. */
            TEST_ASSERT( IotTaskPool_DestroyRecyclableJob( taskPool, job ) == IOT_TASKPOOL_ILLEGAL_OPERATION );
        }

        /* Create/Destroy. */
        {
            IotTaskPoolJobStorage_t jobStorage;
            IotTaskPoolJob_t job;
            IotTaskPoolJobStatus_t jobStatusAtCancellation;

            /* Create legal static job. */
            TEST_ASSERT( IotTaskPool_CreateJob( &BlankExecution, NULL, &jobStorage, &job ) == IOT_TASKPOOL_SUCCESS );
            /* Schedule deferred, then try to illegally destroy, then cancel */
            TEST_ASSERT( IotTaskPool_ScheduleDeferred( taskPool, job, ONE_HOUR_FROM_NOW_MS ) == IOT_TASKPOOL_SUCCESS );
            TEST_ASSERT( IotTaskPool_DestroyRecyclableJob( taskPool, job ) == IOT_TASKPOOL_ILLEGAL_OPERATION );
            TEST_ASSERT( IotTaskPool_TryCancel( taskPool, job, &jobStatusAtCancellation ) == IOT_TASKPOOL_SUCCESS );
            TEST_ASSERT( jobStatusAtCancellation == IOT_TASKPOOL_STATUS_DEFERRED );
        }

        /* Create/Destroy. */
        {
            IotTaskPoolJobStorage_t jobStorage;
            IotTaskPoolJob_t job;

            /* Create legal static job. */
            TEST_ASSERT( IotTaskPool_CreateJob( &BlankExecution, NULL, &jobStorage, &job ) == IOT_TASKPOOL_SUCCESS );
            /* Schedule immediate, then try to illegally destroy it. */
            TEST_ASSERT( IotTaskPool_Schedule( taskPool, job, 0 ) == IOT_TASKPOOL_SUCCESS );
            TEST_ASSERT( IotTaskPool_DestroyRecyclableJob( taskPool, job ) == IOT_TASKPOOL_ILLEGAL_OPERATION );
        }
    }

    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test task pool job static and dynamic memory creation with bogus parameters.
 */
TEST( Common_Unit_Task_Pool, CreateDestroyRecycleRecyclableJobError )
{
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = 3, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        /* Trivial parameter validation jobs. */
        {
            IotTaskPoolJob_t pJob = IOT_TASKPOOL_JOB_INITIALIZER;
            /* NULL callback. */
            TEST_ASSERT( IotTaskPool_CreateRecyclableJob( taskPool, NULL, NULL, &pJob ) == IOT_TASKPOOL_BAD_PARAMETER );
            /* NULL engine handle. */
            TEST_ASSERT( IotTaskPool_CreateRecyclableJob( NULL, &ExecutionWithRecycleCb, NULL, &pJob ) == IOT_TASKPOOL_BAD_PARAMETER );
            /* NULL job handle. */
            TEST_ASSERT( IotTaskPool_CreateRecyclableJob( taskPool, &ExecutionWithRecycleCb, NULL, NULL ) == IOT_TASKPOOL_BAD_PARAMETER );
        }

        /* Create/Destroy. */
        {
            IotTaskPoolJob_t pJob = IOT_TASKPOOL_JOB_INITIALIZER;

            /* Create legal recyclable job. */
            TEST_ASSERT( IotTaskPool_CreateRecyclableJob( taskPool, &BlankExecution, NULL, &pJob ) == IOT_TASKPOOL_SUCCESS );
            /* Recycle the job. */
            TEST_ASSERT( IotTaskPool_DestroyRecyclableJob( taskPool, pJob ) == IOT_TASKPOOL_SUCCESS );
        }

        /* Create/Schedule/Destroy. */
        {
            IotTaskPoolJob_t pJob = IOT_TASKPOOL_JOB_INITIALIZER;

            /* Create legal recyclable job. */
            TEST_ASSERT( IotTaskPool_CreateRecyclableJob( taskPool, &BlankExecution, NULL, &pJob ) == IOT_TASKPOOL_SUCCESS );
            /* Schedule deferred, then try to destroy it. */
            TEST_ASSERT( IotTaskPool_ScheduleDeferred( taskPool, pJob, ONE_HOUR_FROM_NOW_MS ) == IOT_TASKPOOL_SUCCESS );
            TEST_ASSERT( IotTaskPool_DestroyRecyclableJob( taskPool, pJob ) == IOT_TASKPOOL_SUCCESS );
        }

        /* Create/Recycle. */
        {
            IotTaskPoolJob_t pJob = IOT_TASKPOOL_JOB_INITIALIZER;

            /* Create legal recyclable job. */
            TEST_ASSERT( IotTaskPool_CreateRecyclableJob( taskPool, &BlankExecution, NULL, &pJob ) == IOT_TASKPOOL_SUCCESS );
            /* Illegally recycle legal static job. */
            TEST_ASSERT( IotTaskPool_RecycleJob( taskPool, pJob ) == IOT_TASKPOOL_SUCCESS );
        }

        /* Create/Schedule/Cancel/Recycle. */
        {
            IotTaskPoolJob_t pJob = IOT_TASKPOOL_JOB_INITIALIZER;
            IotTaskPoolJobStatus_t jobStatusAtCancellation;

            /* Create legal recyclable job. */
            TEST_ASSERT( IotTaskPool_CreateRecyclableJob( taskPool, &BlankExecution, NULL, &pJob ) == IOT_TASKPOOL_SUCCESS );
            /* Schedule deferred, then try to cancel it and finally recycle it. */
            TEST_ASSERT( IotTaskPool_ScheduleDeferred( taskPool, pJob, ONE_HOUR_FROM_NOW_MS ) == IOT_TASKPOOL_SUCCESS );
            TEST_ASSERT( IotTaskPool_TryCancel( taskPool, pJob, &jobStatusAtCancellation ) == IOT_TASKPOOL_SUCCESS );
            TEST_ASSERT( jobStatusAtCancellation == IOT_TASKPOOL_STATUS_DEFERRED );
            TEST_ASSERT( IotTaskPool_RecycleJob( taskPool, pJob ) == IOT_TASKPOOL_SUCCESS );
        }
    }

    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test task pool job static and dynamic memory creation with bogus parameters.
 */
TEST( Common_Unit_Task_Pool, CreateRecyclableJob )
{
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = 3, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        /* Recyclable jobs. */
        {
            uint32_t count, jobLimit;

            /* In static memory mode, only the recyclable job limit may be allocated. */
            #if IOT_STATIC_MEMORY_ONLY == 1
                jobLimit = IOT_TASKPOOL_JOBS_RECYCLE_LIMIT;
                IotTaskPoolJob_t pJobs[ IOT_TASKPOOL_JOBS_RECYCLE_LIMIT ];
            #else
                jobLimit = 2 * IOT_TASKPOOL_JOBS_RECYCLE_LIMIT;
                IotTaskPoolJob_t pJobs[ 2 * TEST_TASKPOOL_ITERATIONS ];
            #endif

            for( count = 0; count < jobLimit; ++count )
            {
                TEST_ASSERT( IotTaskPool_CreateRecyclableJob( taskPool, &ExecutionWithRecycleCb, NULL, &pJobs[ count ] ) == IOT_TASKPOOL_SUCCESS );
                TEST_ASSERT( pJobs[ count ] != NULL );
            }

            for( count = 0; count < jobLimit; ++count )
            {
                TEST_ASSERT( IotTaskPool_RecycleJob( taskPool, pJobs[ count ] ) == IOT_TASKPOOL_SUCCESS );
            }
        }
    }

    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test scheduling a job with bad parameters.
 */
TEST( Common_Unit_Task_Pool, ScheduleTasksError )
{
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = 3, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        IotTaskPoolJobStorage_t jobStorage;
        IotTaskPoolJob_t job;

        TEST_ASSERT( IotTaskPool_CreateJob( &ExecutionWithoutDestroyCb, NULL, &jobStorage, &job ) == IOT_TASKPOOL_SUCCESS );

        /* NULL Task Pool Handle. */
        TEST_ASSERT( IotTaskPool_Schedule( NULL, job, 0 ) == IOT_TASKPOOL_BAD_PARAMETER );
        /* NULL Work item Handle. */
        TEST_ASSERT( IotTaskPool_Schedule( taskPool, NULL, 0 ) == IOT_TASKPOOL_BAD_PARAMETER );
    }

    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test scheduling a set of jobs: static allocation, bulk execution.
 */
TEST( Common_Unit_Task_Pool, ScheduleTasks_LongRunningAndCachedJobsAndDestroy )
{
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;
    IotTaskPoolJob_t pRecyclableJob = IOT_TASKPOOL_JOB_INITIALIZER;
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 1, .maxThreads = 2, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };
    IotTaskPoolJobStorage_t tpJobsStorage[ TEST_TASKPOOL_LONG_JOBS_NUMBER ];
    IotTaskPoolJob_t tpJobs[ TEST_TASKPOOL_LONG_JOBS_NUMBER ];
    IotTaskPoolJobStorage_t tpDeferredJobsStorage[ TEST_TASKPOOL_LONG_JOBS_NUMBER ];
    IotTaskPoolJob_t tpDeferredJobs[ TEST_TASKPOOL_LONG_JOBS_NUMBER ];

    JobUserContext_t userContext;

    memset( &userContext, 0, sizeof( JobUserContext_t ) );

    /* Initialize user context. */
    TEST_ASSERT( IotMutex_Create( &userContext.lock, false ) );

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    /* Create a recyclable job we will never schedule, just to have it in the cache for code coverage purposes. */
    TEST_ASSERT( IotTaskPool_CreateRecyclableJob( taskPool, &ExecutionWithRecycleCb, NULL, &pRecyclableJob ) == IOT_TASKPOOL_SUCCESS );
    TEST_ASSERT( IotTaskPool_DestroyRecyclableJob( taskPool, pRecyclableJob ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        /* Statically allocated jobs, schedule all, then wait all. */
        {
            uint32_t count;
            uint32_t scheduled = 0;
            IotTaskPoolError_t errorSchedule;

            for( count = 0; count < TEST_TASKPOOL_LONG_JOBS_NUMBER; ++count )
            {
                /* Schedule the job NOT to be recycle in the callback, since the buffer is statically allocated. */
                TEST_ASSERT( IotTaskPool_CreateJob( &ExecutionLongWithoutDestroyCb, &userContext, &tpJobsStorage[ count ], &tpJobs[ count ] ) == IOT_TASKPOOL_SUCCESS );

                errorSchedule = IotTaskPool_Schedule( taskPool, tpJobs[ count ], 0 );

                switch( errorSchedule )
                {
                    case IOT_TASKPOOL_SUCCESS:
                        ++scheduled;
                        break;

                    case IOT_TASKPOOL_BAD_PARAMETER:
                    case IOT_TASKPOOL_ILLEGAL_OPERATION:
                        TEST_ASSERT( false );
                        break;

                    default:
                        TEST_ASSERT( false );
                }
            }

            for( count = 0; count < TEST_TASKPOOL_LONG_JOBS_NUMBER; ++count )
            {
                /* Schedule the job NOT to be recycle in the callback, since the buffer is statically allocated. */
                TEST_ASSERT( IotTaskPool_CreateJob( &ExecutionLongWithoutDestroyCb, &userContext, &tpDeferredJobsStorage[ count ], &tpDeferredJobs[ count ] ) == IOT_TASKPOOL_SUCCESS );

                errorSchedule = IotTaskPool_ScheduleDeferred( taskPool, tpDeferredJobs[ count ], ONE_HOUR_FROM_NOW_MS );

                switch( errorSchedule )
                {
                    case IOT_TASKPOOL_SUCCESS:
                        ++scheduled;
                        break;

                    case IOT_TASKPOOL_BAD_PARAMETER:
                    case IOT_TASKPOOL_ILLEGAL_OPERATION:
                        TEST_ASSERT( false );
                        break;

                    default:
                        TEST_ASSERT( false );
                }
            }
        }
    }

    /* Destroy the taskpool. It will empty all queues. */
    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );

    /* Destroy user context. */
    IotMutex_Destroy( &userContext.lock );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test scheduling a job with bad parameters.
 */
TEST( Common_Unit_Task_Pool, TaskPool_ScheduleRecyclableTasksError )
{
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = 3, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        IotTaskPoolJob_t pJob = IOT_TASKPOOL_JOB_INITIALIZER;

        TEST_ASSERT( IotTaskPool_CreateRecyclableJob( taskPool, &ExecutionWithRecycleCb, NULL, &pJob ) == IOT_TASKPOOL_SUCCESS );

        /* NULL Task Pool Handle. */
        TEST_ASSERT( IotTaskPool_Schedule( NULL, pJob, 0 ) == IOT_TASKPOOL_BAD_PARAMETER );
        /* NULL Work item Handle. */
        TEST_ASSERT( IotTaskPool_Schedule( taskPool, NULL, 0 ) == IOT_TASKPOOL_BAD_PARAMETER );
        /* Recycle the job, so we do not leak it. */
        TEST_ASSERT( IotTaskPool_RecycleJob( taskPool, pJob ) == IOT_TASKPOOL_SUCCESS );
    }

    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests that the taskpool actually grows the number of tasks as expected.
 */
TEST( Common_Unit_Task_Pool, ScheduleTasks_Grow )
{
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = TEST_TASKPOOL_NUMBER_OF_JOBS, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };

    JobBlockingUserContext_t userContext;

    /* Initialize user context. */
    TEST_ASSERT( IotSemaphore_Create( &userContext.signal, 0, TEST_TASKPOOL_NUMBER_OF_JOBS ) );
    TEST_ASSERT( IotSemaphore_Create( &userContext.block, 0, TEST_TASKPOOL_NUMBER_OF_JOBS ) );

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        /* Statically allocated job, schedule one, then wait. */
        {
            uint32_t count;
            IotTaskPoolJobStorage_t jobsStorage[ TEST_TASKPOOL_NUMBER_OF_JOBS ];
            IotTaskPoolJob_t jobs[ TEST_TASKPOOL_NUMBER_OF_JOBS ];

            /* Create a number of jobs that is equal to the max number of threads in the taskpool. */
            for( count = 0; count < TEST_TASKPOOL_NUMBER_OF_JOBS; ++count )
            {
                /* Schedule the job NOT to be recycle in the callback, since the buffer is statically allocated. */
                /* The callback will block indefintely, stealing a task pool thread. The task pool will need to grow to pass this test. */
                TEST_ASSERT( IotTaskPool_CreateJob( &ExecutionBlockingWithoutDestroyCb, &userContext, &jobsStorage[ count ], &jobs[ count ] ) == IOT_TASKPOOL_SUCCESS );
            }

            for( count = 0; count < TEST_TASKPOOL_NUMBER_OF_JOBS; ++count )
            {
                TEST_ASSERT( IotTaskPool_Schedule( taskPool, jobs[ count ], 0 ) == IOT_TASKPOOL_SUCCESS );
            }

            count = 0;

            while( true )
            {
                /* Wait for the callback to signal the semaphore. It must happen exactly _NUMBER_OF_JOBS times. */
                IotSemaphore_Wait( &userContext.signal );

                ++count;

                if( count == TEST_TASKPOOL_NUMBER_OF_JOBS )
                {
                    break;
                }
            }

            /* Signal all taskpool threads to exit. */
            for( count = 0; count < TEST_TASKPOOL_NUMBER_OF_JOBS; ++count )
            {
                IotSemaphore_Post( &userContext.block );
            }
        }
    }

    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );

    /* Destroy user context. */
    IotSemaphore_Destroy( &userContext.signal );
    IotSemaphore_Destroy( &userContext.block );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test scheduling a set of non-recyclable jobs: static allocation, sequential execution.
 */
TEST( Common_Unit_Task_Pool, ScheduleTasks_GrowHighPri )
{
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;

    /* Use a taskpool with not enough threads. */
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = TEST_TASKPOOL_NUMBER_OF_THREADS, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };

    JobBlockingUserContext_t userContext;

    /* Initialize user context. */
    TEST_ASSERT( IotSemaphore_Create( &userContext.signal, 0, TEST_TASKPOOL_NUMBER_OF_JOBS ) );
    TEST_ASSERT( IotSemaphore_Create( &userContext.block, 0, TEST_TASKPOOL_NUMBER_OF_JOBS ) );

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        /* Statically allocated job, schedule one, then wait. */
        {
            uint32_t count;
            IotTaskPoolJobStorage_t jobsStorage[ TEST_TASKPOOL_NUMBER_OF_JOBS ];
            IotTaskPoolJob_t jobs[ TEST_TASKPOOL_NUMBER_OF_JOBS ];

            /* Create a number of jobs that is equal to the max number of threads in the taskpool. */
            for( count = 0; count < TEST_TASKPOOL_NUMBER_OF_JOBS; ++count )
            {
                /* Schedule the job NOT to be recycle in the callback, since the buffer is statically allocated. */
                /* The callback will block indefintely, stealing a task pool thread. The task pool will need to grow to pass this test. */
                TEST_ASSERT( IotTaskPool_CreateJob( &ExecutionBlockingWithoutDestroyCb, &userContext, &jobsStorage[ count ], &jobs[ count ] ) == IOT_TASKPOOL_SUCCESS );
            }

            /* After scheduling _NUMBER_OF_JOBS - 1 jobs the task pool is maxed out, only a high pri task can make it grow more. */
            for( count = 0; count < TEST_TASKPOOL_NUMBER_OF_THREADS; ++count )
            {
                TEST_ASSERT( IotTaskPool_Schedule( taskPool, jobs[ count ], 0 ) == IOT_TASKPOOL_SUCCESS );
            }

            /*Schedule a high pri task can make it grow more. */
            TEST_ASSERT( IotTaskPool_Schedule( taskPool, jobs[ count ], IOT_TASKPOOL_JOB_HIGH_PRIORITY ) == IOT_TASKPOOL_SUCCESS );

            count = 0;

            while( true )
            {
                /* Wait for the callback to signal the semaphore. It must happen exactly _NUMBER_OF_JOBS times. */
                IotSemaphore_Wait( &userContext.signal );

                ++count;

                if( count == TEST_TASKPOOL_NUMBER_OF_JOBS )
                {
                    break;
                }
            }

            /* Signal all taskpool threads to exit. */
            for( count = 0; count < TEST_TASKPOOL_NUMBER_OF_JOBS; ++count )
            {
                IotSemaphore_Post( &userContext.block );
            }
        }
    }

    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );

    /* Destroy user context. */
    IotSemaphore_Destroy( &userContext.signal );
    IotSemaphore_Destroy( &userContext.block );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test scheduling a set of non-recyclable jobs: static allocation, sequential execution.
 */
TEST( Common_Unit_Task_Pool, ScheduleTasks_ScheduleOneThenWait )
{
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = 3, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };

    JobUserContext_t userContext;

    memset( &userContext, 0, sizeof( JobUserContext_t ) );
    /* Initialize user context. */
    TEST_ASSERT_TRUE( IotMutex_Create( &userContext.lock, false ) );

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        /* Statically allocated job, schedule one, then wait. */
        {
            uint32_t count;
            uint32_t scheduled = 0;
            IotTaskPoolJobStorage_t jobStorage;
            IotTaskPoolJob_t job = IOT_TASKPOOL_JOB_INITIALIZER;

            for( count = 0; count < TEST_TASKPOOL_ITERATIONS; ++count )
            {
                /* Schedule the job NOT to be recycle in the callback, since the buffer is statically allocated. */
                TEST_ASSERT( IotTaskPool_CreateJob( &ExecutionWithoutDestroyCb, &userContext, &jobStorage, &job ) == IOT_TASKPOOL_SUCCESS );

                IotTaskPoolError_t errorSchedule = IotTaskPool_Schedule( taskPool, job, 0 );

                switch( errorSchedule )
                {
                    case IOT_TASKPOOL_SUCCESS:
                        ++scheduled;
                        break;

                    case IOT_TASKPOOL_BAD_PARAMETER:
                        TEST_ASSERT( false );
                        break;

                    case IOT_TASKPOOL_ILLEGAL_OPERATION:
                        TEST_ASSERT( false );
                        break;

                    case IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS:
                        TEST_ASSERT( false );
                        break;

                    default:
                        TEST_ASSERT( false );
                }

                /* Ensure callback actually executed. */
                while( true )
                {
                    IotClock_SleepMs( 50 );

                    IotMutex_Lock( &userContext.lock );

                    if( userContext.counter == scheduled )
                    {
                        IotMutex_Unlock( &userContext.lock );

                        break;
                    }

                    IotMutex_Unlock( &userContext.lock );
                }

                TEST_ASSERT( userContext.counter == scheduled );
            }

            /* Since jobs were build from a static buffer and scheduled one-by-one, we
             * should have received all callbacks.
             */
            TEST_ASSERT( scheduled == TEST_TASKPOOL_ITERATIONS );
        }
    }

    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );

    /* Destroy user context. */
    IotMutex_Destroy( &userContext.lock );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test scheduling a set of non-recyclable jobs: static allocation, sequential execution.
 */
TEST( Common_Unit_Task_Pool, ScheduleTasks_ScheduleOneDeferredThenWait )
{
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = 3, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };

    JobUserContext_t userContext;

    memset( &userContext, 0, sizeof( JobUserContext_t ) );

    /* Initialize user context. */
    TEST_ASSERT( IotMutex_Create( &userContext.lock, false ) );

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        /* Statically allocated job, schedule one, then wait. */
        {
            uint32_t count;
            uint32_t scheduled = 0;
            IotTaskPoolJobStorage_t jobStorage;
            IotTaskPoolJob_t job;

            for( count = 0; count < TEST_TASKPOOL_ITERATIONS; ++count )
            {
                /* Schedule the job NOT to be recycle in the callback, since the buffer is statically allocated. */
                TEST_ASSERT( IotTaskPool_CreateJob( &ExecutionWithoutDestroyCb, &userContext, &jobStorage, &job ) == IOT_TASKPOOL_SUCCESS );

                IotTaskPoolError_t errorSchedule = IotTaskPool_ScheduleDeferred( taskPool, job, 10 + ( rand() % 50 ) );

                switch( errorSchedule )
                {
                    case IOT_TASKPOOL_SUCCESS:
                        ++scheduled;
                        break;

                    case IOT_TASKPOOL_BAD_PARAMETER:
                        TEST_ASSERT( false );
                        break;

                    case IOT_TASKPOOL_ILLEGAL_OPERATION:
                        TEST_ASSERT( false );
                        break;

                    case IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS:
                        TEST_ASSERT( false );
                        break;

                    default:
                        TEST_ASSERT( false );
                }

                /* Ensure callback actually executed. */
                while( true )
                {
                    IotClock_SleepMs( 50 );

                    IotMutex_Lock( &userContext.lock );

                    if( userContext.counter == scheduled )
                    {
                        IotMutex_Unlock( &userContext.lock );

                        break;
                    }

                    IotMutex_Unlock( &userContext.lock );
                }

                TEST_ASSERT( userContext.counter == scheduled );
            }

            /* Since jobs were build from a static buffer and scheduled one-by-one, we
             * should have received all callbacks.
             */
            TEST_ASSERT( scheduled == TEST_TASKPOOL_ITERATIONS );
        }
    }

    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );

    /* Destroy user context. */
    IotMutex_Destroy( &userContext.lock );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test scheduling a set of recyclable jobs: dynamic allocation, sequential execution.
 */
TEST( Common_Unit_Task_Pool, ScheduleTasks_ScheduleOneRecyclableThenWait )
{
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = 3, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };

    JobUserContext_t userContext;

    memset( &userContext, 0, sizeof( JobUserContext_t ) );

    /* Initialize user context. */
    TEST_ASSERT( IotMutex_Create( &userContext.lock, false ) );

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        /* Dynamically allocated job, schedule one, then wait. */
        {
            uint32_t count;
            uint32_t scheduled = 0;
            IotTaskPoolJob_t pJob = IOT_TASKPOOL_JOB_INITIALIZER;

            for( count = 0; count < TEST_TASKPOOL_ITERATIONS; ++count )
            {
                /* Schedule the job to be recycle in the callback. */
                TEST_ASSERT( IotTaskPool_CreateRecyclableJob( taskPool, &ExecutionWithRecycleCb, &userContext, &pJob ) == IOT_TASKPOOL_SUCCESS );

                IotTaskPoolError_t errorSchedule = IotTaskPool_Schedule( taskPool, pJob, 0 );

                switch( errorSchedule )
                {
                    case IOT_TASKPOOL_SUCCESS:
                        ++scheduled;
                        break;

                    case IOT_TASKPOOL_BAD_PARAMETER:
                    case IOT_TASKPOOL_ILLEGAL_OPERATION:
                    case IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS:
                        TEST_ASSERT( false );
                        break;

                    default:
                        TEST_ASSERT( false );
                }

                /* Ensure callback actually executed. */
                while( true )
                {
                    IotClock_SleepMs( 50 );

                    IotMutex_Lock( &userContext.lock );

                    if( userContext.counter == scheduled )
                    {
                        IotMutex_Unlock( &userContext.lock );

                        break;
                    }

                    IotMutex_Unlock( &userContext.lock );
                }

                TEST_ASSERT( userContext.counter == scheduled );
            }

            TEST_ASSERT( scheduled == TEST_TASKPOOL_ITERATIONS );

            /* Since jobs were build from a static buffer and scheduled one-by-one, we
             * should have received all callbacks.
             */
            TEST_ASSERT( scheduled == TEST_TASKPOOL_ITERATIONS );
        }
    }

    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );

    /* Destroy user context. */
    IotMutex_Destroy( &userContext.lock );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test scheduling a set of jobs: static allocation, bulk execution.
 */
TEST( Common_Unit_Task_Pool, ScheduleTasks_ScheduleAllThenWait )
{
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = 3, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };

    JobUserContext_t userContext;

    memset( &userContext, 0, sizeof( JobUserContext_t ) );

    /* Initialize user context. */
    TEST_ASSERT( IotMutex_Create( &userContext.lock, false ) );

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        /* Statically allocated jobs, schedule all, then wait all. */
        {
            uint32_t count;
            uint32_t scheduled = 0;
            IotTaskPoolJobStorage_t tpJobsStorage[ TEST_TASKPOOL_ITERATIONS ];
            IotTaskPoolJob_t tpJobs[ TEST_TASKPOOL_ITERATIONS ];

            for( count = 0; count < TEST_TASKPOOL_ITERATIONS; ++count )
            {
                /* Schedule the job NOT to be recycle in the callback, since the buffer is statically allocated. */
                TEST_ASSERT( IotTaskPool_CreateJob( &ExecutionWithoutDestroyCb, &userContext, &tpJobsStorage[ count ], &tpJobs[ count ] ) == IOT_TASKPOOL_SUCCESS );

                IotTaskPoolError_t errorSchedule = IotTaskPool_Schedule( taskPool, tpJobs[ count ], 0 );

                switch( errorSchedule )
                {
                    case IOT_TASKPOOL_SUCCESS:
                        ++scheduled;
                        break;

                    case IOT_TASKPOOL_BAD_PARAMETER:
                    case IOT_TASKPOOL_ILLEGAL_OPERATION:
                        TEST_ASSERT( false );
                        break;

                    default:
                        TEST_ASSERT( false );
                }
            }

            /* Wait until callback is executed. */
            while( true )
            {
                IotClock_SleepMs( 50 );

                IotMutex_Lock( &userContext.lock );

                if( userContext.counter == scheduled )
                {
                    IotMutex_Unlock( &userContext.lock );

                    break;
                }

                IotMutex_Unlock( &userContext.lock );
            }

            TEST_ASSERT_TRUE( userContext.counter == scheduled );
        }
    }

    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );

    /* Destroy user context. */
    IotMutex_Destroy( &userContext.lock );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test scheduling a set of jobs: static allocation, bulk execution.
 */
TEST( Common_Unit_Task_Pool, ScheduleTasks_ScheduleAllRecyclableThenWait )
{
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = 3, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };

    JobUserContext_t userContext;

    memset( &userContext, 0, sizeof( JobUserContext_t ) );

    /* Initialize user context. */
    TEST_ASSERT( IotMutex_Create( &userContext.lock, false ) );

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        /* Statically allocated jobs, schedule all, then wait all. */
        {
            uint32_t count, maxJobs;
            uint32_t scheduled = 0;

            /* In static memory mode, only the recyclable job limit may be allocated. */
            #if IOT_STATIC_MEMORY_ONLY == 1
                maxJobs = IOT_TASKPOOL_JOBS_RECYCLE_LIMIT;
                IotTaskPoolJob_t tpJobs[ IOT_TASKPOOL_JOBS_RECYCLE_LIMIT ];
            #else
                maxJobs = TEST_TASKPOOL_ITERATIONS;
                IotTaskPoolJob_t tpJobs[ TEST_TASKPOOL_ITERATIONS ];
            #endif

            for( count = 0; count < maxJobs; ++count )
            {
                /* Schedule the job to be recycle in the callback. */
                TEST_ASSERT( IotTaskPool_CreateRecyclableJob( taskPool, &ExecutionWithRecycleCb, &userContext, &tpJobs[ count ] ) == IOT_TASKPOOL_SUCCESS );

                IotTaskPoolError_t errorSchedule = IotTaskPool_Schedule( taskPool, tpJobs[ count ], 0 );

                switch( errorSchedule )
                {
                    case IOT_TASKPOOL_SUCCESS:
                        ++scheduled;
                        break;

                    case IOT_TASKPOOL_BAD_PARAMETER:
                    case IOT_TASKPOOL_ILLEGAL_OPERATION:
                        TEST_ASSERT( false );
                        break;

                    default:
                        TEST_ASSERT( false );
                }
            }

            /* Wait until callback is executed. */
            while( true )
            {
                IotClock_SleepMs( 50 );

                IotMutex_Lock( &userContext.lock );

                if( userContext.counter == scheduled )
                {
                    IotMutex_Unlock( &userContext.lock );

                    break;
                }

                IotMutex_Unlock( &userContext.lock );
            }

            TEST_ASSERT_TRUE( userContext.counter == scheduled );
        }
    }

    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );

    /* Destroy user context. */
    IotMutex_Destroy( &userContext.lock );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test scheduling a set of deferred jobs.
 */
TEST( Common_Unit_Task_Pool, ScheduleTasks_ScheduleAllDeferredRecyclableThenWait )
{
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = 3, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };

    JobUserContext_t userContext;

    memset( &userContext, 0, sizeof( JobUserContext_t ) );

    /* Initialize user context. */
    TEST_ASSERT( IotMutex_Create( &userContext.lock, false ) );

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        /* Statically allocated jobs, schedule all, then wait all. */
        {
            uint32_t count, maxJobs;
            uint32_t scheduled = 0;

            /* In static memory mode, only the recyclable job limit may be allocated. */
            #if IOT_STATIC_MEMORY_ONLY == 1
                maxJobs = IOT_TASKPOOL_JOBS_RECYCLE_LIMIT;
                IotTaskPoolJob_t tpJobs[ IOT_TASKPOOL_JOBS_RECYCLE_LIMIT ];
            #else
                maxJobs = TEST_TASKPOOL_ITERATIONS;
                IotTaskPoolJob_t tpJobs[ TEST_TASKPOOL_ITERATIONS ];
            #endif

            for( count = 0; count < maxJobs; ++count )
            {
                /* Schedule the job to be recycle in the callback. */
                TEST_ASSERT( IotTaskPool_CreateRecyclableJob( taskPool, &ExecutionWithRecycleCb, &userContext, &tpJobs[ count ] ) == IOT_TASKPOOL_SUCCESS );

                IotTaskPoolError_t errorSchedule = IotTaskPool_ScheduleDeferred( taskPool, tpJobs[ count ], 10 + ( rand() % 500 ) );

                switch( errorSchedule )
                {
                    case IOT_TASKPOOL_SUCCESS:
                        ++scheduled;
                        break;

                    case IOT_TASKPOOL_BAD_PARAMETER:
                    case IOT_TASKPOOL_ILLEGAL_OPERATION:
                        TEST_ASSERT( false );
                        break;

                    default:
                        TEST_ASSERT( false );
                }
            }

            /* Wait until callback is executed. */
            while( true )
            {
                IotClock_SleepMs( 50 );

                IotMutex_Lock( &userContext.lock );

                if( userContext.counter == scheduled )
                {
                    IotMutex_Unlock( &userContext.lock );

                    break;
                }

                IotMutex_Unlock( &userContext.lock );
            }

            TEST_ASSERT_TRUE( userContext.counter == scheduled );
        }
    }

    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );

    /* Destroy user context. */
    IotMutex_Destroy( &userContext.lock );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test scheduling and re-scheduling (without canceling first) a set of jobs.
 */
TEST( Common_Unit_Task_Pool, ScheduleTasks_ReSchedule )
{
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = 3, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };

    JobUserContext_t userContext;

    memset( &userContext, 0, sizeof( JobUserContext_t ) );

    /* Initialize user context. */
    TEST_ASSERT( IotMutex_Create( &userContext.lock, false ) );

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        /* Statically allocated jobs, schedule all, then wait all. */
        {
            uint32_t count, maxJobs;
            uint32_t scheduled = 0, rescheduled = 0, failedReschedule = 0;

            /* In static memory mode, only the recyclable job limit may be allocated. */
            #if IOT_STATIC_MEMORY_ONLY == 1
                maxJobs = IOT_TASKPOOL_JOBS_RECYCLE_LIMIT;
                IotTaskPoolJobStorage_t tpJobsStorage[ IOT_TASKPOOL_JOBS_RECYCLE_LIMIT ];
                IotTaskPoolJob_t tpJobs[ IOT_TASKPOOL_JOBS_RECYCLE_LIMIT ];
            #else
                maxJobs = 10;
                IotTaskPoolJobStorage_t tpJobsStorage[ 10 ];
                IotTaskPoolJob_t tpJobs[ 10 ];
            #endif

            /* Create all jobs. */
            for( count = 0; count < maxJobs; ++count )
            {
                /* Schedule the job to be recycled in the callback. */
                TEST_ASSERT( IotTaskPool_CreateJob( &ExecutionLongWithoutDestroyCb, &userContext, &tpJobsStorage[ count ], &tpJobs[ count ] ) == IOT_TASKPOOL_SUCCESS );
            }

            /* Schedule all jobs. */
            for( count = 0; count < maxJobs; ++count )
            {
                IotTaskPoolError_t errorSchedule;

                /* Schedule jobs for a really really long time from now, so we know that they will not execute. */
                errorSchedule = IotTaskPool_Schedule( taskPool, tpJobs[ count ], 0 );

                switch( errorSchedule )
                {
                    case IOT_TASKPOOL_SUCCESS:
                        ++scheduled;
                        break;

                    case IOT_TASKPOOL_BAD_PARAMETER:
                    case IOT_TASKPOOL_ILLEGAL_OPERATION:
                    case IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS:
                        TEST_ASSERT( false );
                        break;

                    default:
                        TEST_ASSERT( false );
                }
            }

            /* Give a chance to some jobs to start execution. */
            IotClock_SleepMs( 50 );

            /* Reschedule all. Some will fail to be rescheduled... */
            for( count = 0; count < maxJobs; ++count )
            {
                IotTaskPoolError_t errorReSchedule;

                errorReSchedule = IotTaskPool_Schedule( taskPool, tpJobs[ count ], 0 );

                switch( errorReSchedule )
                {
                    case IOT_TASKPOOL_SUCCESS:
                        rescheduled++;
                        break;

                    case IOT_TASKPOOL_ILLEGAL_OPERATION:
                        /* Job already executed. */
                        failedReschedule++;
                        break;

                    case IOT_TASKPOOL_BAD_PARAMETER:
                    case IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS:
                        TEST_ASSERT( false );
                        break;

                    default:
                        TEST_ASSERT( false );
                }
            }

            TEST_ASSERT_TRUE( ( rescheduled + failedReschedule ) == scheduled );

            /* Wait until callback is executed. */
            while( true )
            {
                IotClock_SleepMs( 50 );

                IotMutex_Lock( &userContext.lock );

                if( userContext.counter == scheduled )
                {
                    IotMutex_Unlock( &userContext.lock );

                    break;
                }

                IotMutex_Unlock( &userContext.lock );
            }

            TEST_ASSERT_TRUE( userContext.counter == scheduled );
        }
    }

    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );

    /* Destroy user context. */
    IotMutex_Destroy( &userContext.lock );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test scheduling and re-scheduling (without canceling first) a set of deferred jobs.
 */
TEST( Common_Unit_Task_Pool, ScheduleTasks_ReScheduleDeferred )
{
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = 3, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };

    JobUserContext_t userContext;

    memset( &userContext, 0, sizeof( JobUserContext_t ) );

    /* Initialize user context. */
    TEST_ASSERT( IotMutex_Create( &userContext.lock, false ) );

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        /* Statically allocated jobs, schedule all, then wait all. */
        {
            uint32_t count, maxJobs;
            uint32_t scheduled = 0, rescheduled = 0;

            /* In static memory mode, only the recyclable job limit may be allocated. */
            #if IOT_STATIC_MEMORY_ONLY == 1
                maxJobs = IOT_TASKPOOL_JOBS_RECYCLE_LIMIT;
                IotTaskPoolJobStorage_t tpJobsStorage[ IOT_TASKPOOL_JOBS_RECYCLE_LIMIT ];
                IotTaskPoolJob_t tpJobs[ IOT_TASKPOOL_JOBS_RECYCLE_LIMIT ];
            #else
                maxJobs = TEST_TASKPOOL_ITERATIONS;
                IotTaskPoolJobStorage_t tpJobsStorage[ TEST_TASKPOOL_ITERATIONS ];
                IotTaskPoolJob_t tpJobs[ TEST_TASKPOOL_ITERATIONS ];
            #endif

            /* Schedule all jobs. */
            for( count = 0; count < maxJobs; ++count )
            {
                IotTaskPoolError_t errorSchedule;

                /* Schedule the job to be recycle in the callback. */
                TEST_ASSERT( IotTaskPool_CreateJob( &ExecutionWithoutDestroyCb, &userContext, &tpJobsStorage[ count ], &tpJobs[ count ] ) == IOT_TASKPOOL_SUCCESS );

                /* Schedule jobs for a really really long time from now, so we know that they will not execute. */
                errorSchedule = IotTaskPool_ScheduleDeferred( taskPool, tpJobs[ count ], ONE_HOUR_FROM_NOW_MS );

                switch( errorSchedule )
                {
                    case IOT_TASKPOOL_SUCCESS:
                        ++scheduled;
                        break;

                    case IOT_TASKPOOL_BAD_PARAMETER:
                    case IOT_TASKPOOL_ILLEGAL_OPERATION:
                        TEST_ASSERT( false );
                        break;

                    default:
                        TEST_ASSERT( false );
                }
            }

            /* Reschedule all. */
            for( count = 0; count < maxJobs; ++count )
            {
                IotTaskPoolError_t errorReSchedule;

                errorReSchedule = IotTaskPool_ScheduleDeferred( taskPool, tpJobs[ count ], 10 + ( rand() % 500 ) );

                switch( errorReSchedule )
                {
                    case IOT_TASKPOOL_SUCCESS:
                        ++rescheduled;
                        break;

                    case IOT_TASKPOOL_BAD_PARAMETER:
                    case IOT_TASKPOOL_ILLEGAL_OPERATION:
                    case IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS:
                        TEST_ASSERT( false );
                        break;

                    default:
                        TEST_ASSERT( false );
                }
            }

            TEST_ASSERT_TRUE( rescheduled == scheduled );

            /* Wait until callback is executed. */
            while( true )
            {
                IotClock_SleepMs( 50 );

                IotMutex_Lock( &userContext.lock );

                if( userContext.counter == scheduled )
                {
                    IotMutex_Unlock( &userContext.lock );

                    break;
                }

                IotMutex_Unlock( &userContext.lock );
            }

            TEST_ASSERT_TRUE( userContext.counter == scheduled );
        }
    }

    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );

    /* Destroy user context. */
    IotMutex_Destroy( &userContext.lock );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test scheduling and canceling jobs.
 */
TEST( Common_Unit_Task_Pool, ScheduleTasks_CancelTasks )
{
    uint32_t count, maxJobs;
    IotTaskPool_t taskPool = IOT_TASKPOOL_INITIALIZER;
    const IotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = 3, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY };
    uint32_t canceled = 0;
    uint32_t scheduled = 0;

    JobUserContext_t userContext;

    memset( &userContext, 0, sizeof( JobUserContext_t ) );

    /* In static memory mode, only the recyclable job limit may be allocated. */
    #if IOT_STATIC_MEMORY_ONLY == 1
        maxJobs = IOT_TASKPOOL_JOBS_RECYCLE_LIMIT;
        IotTaskPoolJobStorage_t jobsStorage[ IOT_TASKPOOL_JOBS_RECYCLE_LIMIT ];
        IotTaskPoolJob_t jobs[ IOT_TASKPOOL_JOBS_RECYCLE_LIMIT ];
    #else
        maxJobs = TEST_TASKPOOL_ITERATIONS;
        IotTaskPoolJobStorage_t jobsStorage[ TEST_TASKPOOL_ITERATIONS ];
        IotTaskPoolJob_t jobs[ TEST_TASKPOOL_ITERATIONS ];
    #endif

    /* Initialize user context. */
    TEST_ASSERT( IotMutex_Create( &userContext.lock, false ) );

    TEST_ASSERT( IotTaskPool_Create( &tpInfo, &taskPool ) == IOT_TASKPOOL_SUCCESS );

    if( TEST_PROTECT() )
    {
        /* Create and schedule loop. */
        for( count = 0; count < maxJobs; ++count )
        {
            IotTaskPoolError_t errorSchedule;

            IotTaskPoolError_t errorCreate = IotTaskPool_CreateJob( &ExecutionWithoutDestroyCb, &userContext, &jobsStorage[ count ], &jobs[ count ] );

            switch( errorCreate )
            {
                case IOT_TASKPOOL_SUCCESS:
                    break;

                case IOT_TASKPOOL_NO_MEMORY: /* OK. */
                    continue;

                case IOT_TASKPOOL_BAD_PARAMETER:
                    TEST_ASSERT( false );
                    break;

                default:
                    TEST_ASSERT( false );
            }

            errorSchedule = IotTaskPool_ScheduleDeferred( taskPool, jobs[ count ], 10 + ( rand() % 20 ) );

            switch( errorSchedule )
            {
                case IOT_TASKPOOL_SUCCESS:
                    ++scheduled;
                    break;

                case IOT_TASKPOOL_BAD_PARAMETER:
                case IOT_TASKPOOL_ILLEGAL_OPERATION:
                case IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS:
                    TEST_ASSERT( false );
                    break;

                default:
                    TEST_ASSERT( false );
            }
        }

        /* Cancellation loop. */
        for( count = 0; count < maxJobs; ++count )
        {
            IotTaskPoolError_t error;
            IotTaskPoolJobStatus_t statusAtCancellation = IOT_TASKPOOL_STATUS_READY;
            IotTaskPoolJobStatus_t statusAfterCancellation = IOT_TASKPOOL_STATUS_READY;

            error = IotTaskPool_TryCancel( taskPool, jobs[ count ], &statusAtCancellation );

            switch( error )
            {
                case IOT_TASKPOOL_SUCCESS:
                    canceled++;

                    TEST_ASSERT(
                        ( statusAtCancellation == IOT_TASKPOOL_STATUS_READY ) ||
                        ( statusAtCancellation == IOT_TASKPOOL_STATUS_DEFERRED ) ||
                        ( statusAtCancellation == IOT_TASKPOOL_STATUS_SCHEDULED ) ||
                        ( statusAtCancellation == IOT_TASKPOOL_STATUS_CANCELED )
                        );

                    TEST_ASSERT( IotTaskPool_GetStatus( taskPool, jobs[ count ], &statusAfterCancellation ) == IOT_TASKPOOL_SUCCESS );
                    TEST_ASSERT( statusAfterCancellation == IOT_TASKPOOL_STATUS_CANCELED );
                    break;

                case IOT_TASKPOOL_CANCEL_FAILED:
                    TEST_ASSERT( ( statusAtCancellation == IOT_TASKPOOL_STATUS_COMPLETED ) );
                    TEST_ASSERT( IotTaskPool_GetStatus( taskPool, jobs[ count ], &statusAfterCancellation ) == IOT_TASKPOOL_SUCCESS );
                    TEST_ASSERT( ( statusAfterCancellation == IOT_TASKPOOL_STATUS_COMPLETED ) );
                    break;

                case IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS:
                    /* This must be a test issue. */
                    TEST_ASSERT( false );
                    break;

                default:
                    TEST_ASSERT( false );
                    break;
            }
        }

        /* Wait until callback is executed. */
        while( ( scheduled - canceled ) != userContext.counter )
        {
            IotClock_SleepMs( 50 );
        }

        TEST_ASSERT( ( scheduled - canceled ) == userContext.counter );
    }

    TEST_ASSERT( IotTaskPool_Destroy( taskPool ) == IOT_TASKPOOL_SUCCESS );

    /* Destroy user context. */
    IotMutex_Destroy( &userContext.lock );
}

/*-----------------------------------------------------------*/
