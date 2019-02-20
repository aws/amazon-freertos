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
* @file aws_iot_taskpool_types.h
* @brief User-facing data types of the Task Pool library.
*/


#ifndef _AWS_IOT_TASKPOOL_TYPES_H_
#define _AWS_IOT_TASKPOOL_TYPES_H_

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
#include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* Platform layer includes. */
#include "platform/aws_iot_threads.h"

/* List/Queue include. */
#include "iot_linear_containers.h"

/*-------------------------- Task Pool enumerated types --------------------------*/

/**
* @enums{taskpool,Task Pool library}
*/

/**
* @ingroup taskpool_datatypes_enums
* @brief Return codes of [Task Pool functions](@ref taskpool_functions).
*
*/
typedef enum AwsIotTaskPoolError
{
    /**
    * @brief Task Pool operation completed successfully.
    *
    * Functions that may return this value:
    * - @ref taskpool_function_createsystemtaskpool
    * - @ref taskpool_function_create
    * - @ref taskpool_function_destroy
    * - @ref taskpool_function_setmaxthreads
    * - @ref taskpool_function_createjob
    * - @ref taskpool_function_schedule
    * - @ref taskpool_function_wait
    * - @ref taskpool_function_timedwait
    * - @ref taskpool_function_getstatus
    * - @ref taskpool_function_trycancel
    *
    */
    AWS_IOT_TASKPOOL_SUCCESS = 0,

    /**
    * @brief Task Pool operation failed because at laest one parameter is invalid.
    *
    * Functions that may return this value:
    * - @ref taskpool_function_createsystemtaskpool
    * - @ref taskpool_function_create
    * - @ref taskpool_function_destroy
    * - @ref taskpool_function_setmaxthreads
    * - @ref taskpool_function_createjob
    * - @ref taskpool_function_schedule
    * - @ref taskpool_function_wait
    * - @ref taskpool_function_timedwait
    * - @ref taskpool_function_getstatus
    * - @ref taskpool_function_trycancel
    *
    */
    AWS_IOT_TASKPOOL_BAD_PARAMETER,

    /**
    * @brief Task Pool operation failed because it is illegal.
    *
    * Functions that may return this value:
    * - @ref taskpool_function_createsystemtaskpool
    * - @ref taskpool_function_destroyjob
    * - @ref taskpool_function_schedule
    * - @ref taskpool_function_wait
    * - @ref taskpool_function_timedwait
    * - @ref taskpool_function_trycancel
    *
    */
    AWS_IOT_TASKPOOL_ILLEGAL_OPERATION,

    /**
    * @brief Task Pool operation failed because allocating memory failed.
    *
    * Functions that may return this value:
    * - @ref taskpool_function_createsystemtaskpool
    * - @ref taskpool_function_create
    * - @ref taskpool_function_setmaxthreads
    * - @ref taskpool_function_createjob
    * - @ref taskpool_function_schedule
    * - @ref taskpool_function_wait
    * - @ref taskpool_function_timedwait
    * - @ref taskpool_function_getstatus
    *
    */
    AWS_IOT_TASKPOOL_NO_MEMORY,

    /**
    * @brief Task Pool operation failed because of out of memory condition.
    *
    * Functions that may return this value:
    * - @ref taskpool_function_timedwait
    *
    */
    AWS_IOT_TASKPOOL_TIMEDOUT,

    /**
    * @brief Task Pool operation failed because of an invalid parameter.
    *
    * Functions that may return this value:
    * - @ref taskpool_function_setmaxthreads
    * - @ref taskpool_function_schedule
    * - @ref taskpool_function_wait
    * - @ref taskpool_function_timedwait
    * - @ref taskpool_function_timedwait
    *
    */
    AWS_IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS,

    /**
    * @brief Task Pool cancellation failed.
    *
    * Functions that may return this value:
    * - @ref taskpool_function_trycancel
    *
    */
    AWS_IOT_TASKPOOL_CANCEL_FAILED,

} AwsIotTaskPoolError_t;

/**
* @enums{taskpool,Task Pool library}
*/

/**
* @ingroup taskpool_datatypes_enums
* @brief Status codes of [Task Pool Job](@ref AwsIotTaskPoolJob_t).
*
*/
typedef enum AwsIotTaskPoolJobStatus
{
    /**
    * @brief Job is ready to be scheduled.
    *
    */
    AWS_IOT_TASKPOOL_STATUS_READY = 0,

    /**
    * @brief Job has been queued for execution.
    *
    */
    AWS_IOT_TASKPOOL_STATUS_SCHEDULED,

    /**
    * @brief Job is executing.
    *
    */
    AWS_IOT_TASKPOOL_STATUS_EXECUTING,

    /**
    * @brief Job execution has completed.
    *
    */
    AWS_IOT_TASKPOOL_STATUS_COMPLETED,

    /**
    * @brief Job has been canceled before it was executed.
    *
    */
    AWS_IOT_TASKPOOL_STATUS_CANCELED,

    /**
    * @brief Job status is undefined.
    *
    */
    AWS_IOT_TASKPOOL_STATUS_UNDEFINED,

} AwsIotTaskPoolJobStatus_t;

/**
* @cond DOXYGEN_IGNORE
* @brief Forward declarations for task pool types.
*
*/
struct AwsIotTaskPool;
struct AwsIotTaskPoolJob;
/** @endcond */

/*---------------------------- Task Pool function pointer types ----------------------------*/

/**
* @functionpointers{taskpool,Task Pool library}
*/

/**
* @ingroup taskpool_datatypes_functionpointers
* @brief Callback type for a user callback.
*
* This type identifies the user callback signature to execute a task pool job. This callback will be invoked
* by the task pool threads with the `pUserContext` parameter, as speficied by the user when
* calling @ref AwsIotTaskPool_Schedule.
*
*/
typedef void ( * IotTaskPoolRoutine_t )( struct AwsIotTaskPool * pTaskPool, struct AwsIotTaskPoolJob * pJob, void * pUserContext );

/*------------------------- Task Pool parameter structs --------------------------*/

/**
* @paramstructs{taskpool,Task Pool}
*/

/**
* @ingroup taskpool_datatypes_paramstructs
* @brief Initialization information to create one task pool instance.
*
* @paramfor  @ref taskpool_function_createsystemtaskpool @ref taskpool_function_create.
*
* Passed as an argument to @ref taskpool_function_create.
*
* @initializer{AwsIotTaskPoolInfo_t,AWS_IOT_TASKPOOL_INFO_INITIALIZER}
*/
typedef struct AwsIotTaskPoolInfo
{
    /**
    * @brief Specifies the operating parameters for a task pool.
    *
    * @attention #AwsIotTaskPoolInfo_t.minThreads <b>MUST</b> be at least 1.
    * #AwsIotTaskPoolInfo_t.maxThreads <b>MUST</b> be smaller or equal to #AwsIotTaskPoolInfo_t.minThreads.
    * If the minimum number of threads is same as the maximum, then the task pool will not try and grow the 
     * number of worker threads at run time.
    */

    uint32_t minThreads; /**< @brief Minimum number of threads in a task pool. These threads will be created when the task pool is first created with @ref taskpool_function_create. */
    uint32_t maxThreads; /**< @brief Maximum number of threads in a task pool. */
    uint32_t stackSize;  /**< @brief Stack size for every task pool thread. */
    uint32_t priority;   /**< @brief priority for every task pool thread. */

} AwsIotTaskPoolInfo_t;

/*------------------------- Task Pool handles structs --------------------------*/

/**
* @ingroup taskpool_datatypes_types
* @brief The task pool data structure keeps track of the internal state and the signals for the dispatcher threads.
* The task pool is a thread safe data structure.
*
* @warning This is a system-level data type that should not be modified.
*
*/
typedef struct AwsIotTaskPool
{
    IotQueue_t        dispatchQueue;    /**< @brief The queue for the jobs waiting to be executed. */
    uint32_t          minThreads;       /**< @brief The minimum number of threads for the task pool. */
    uint32_t          maxThreads;       /**< @brief The maximum number of threads for the task pool. */
    uint32_t          activeThreads;    /**< @brief The number of threads in the task pool at any given time. */
    uint32_t          busyThreads;      /**< @brief The number of busy threads in the task pool at any given time. */
    uint32_t          stackSize;        /**< @brief The stack size for all task pool threads. */
    uint32_t          priority;         /**< @brief The priority for all task pool threads. */
    AwsIotSemaphore_t dispatchSignal;   /**< @brief The synchronization object on which threads are waiting for incoming jobs. */
    AwsIotSemaphore_t startStopSignal;  /**< @brief The synchronization object for threads to signal start and stop condition. */
    AwsIotMutex_t     lock;             /**< @brief The lock to protect the task pool data structure access. */
} AwsIotTaskPool_t;

/**
* @ingroup taskpool_datatypes_types
* @brief The job data structure keeps track of the user callback and context, as well as the status of the job.
* A job can be appended to a queue and hosts a synchronization object for the user to wait on its completion.
*
* @warning This is a system-level data type that should not be modified.
*
*/
typedef struct AwsIotTaskPoolJob
{
    IotTaskPoolRoutine_t      userCallback;  /**< @brief The user provided callback. */
    void *                    pUserContext;  /**< @brief The user provided context. */
    AwsIotSemaphore_t         waitHandle;    /**< @brief The wait handle for the user to wait on the job completion. */
    IotLink_t                 link;          /**< @brief The link to insert the job in the dispatch queue. */
    AwsIotTaskPoolJobStatus_t status;        /**< @brief The status for the job. */
} AwsIotTaskPoolJob_t;

/*------------------------- TASKPOOL defined constants --------------------------*/

/**
* @constantspage{taskpool,Task Pool library}
*
* @section taskpool_constants_initializers Task Pool Initializers
* @brief Provides default values for initializing the data types of the Task Pool library.
*
* @snippet this define_taskpool_initializers
*
* All user-facing data types of the Task Pool library can be initialized using
* one of the following.
*
* @warning Failure to initialize a Task Pool data type with the appropriate initializer
* may result in a runtime error!
* @note The initializers may change at any time in future versions, but their
* names will remain the same.
*
* <b>Example</b>
* @code{c}
* 
* AwsIotTaskPool_t * pTaskPool;
* 
* AwsIotTaskPoolInfo_t tpInfo = AWS_IOT_TASKPOOL_INFO_INITIALIZER_LARGE;
* 
* AwsIotTaskPoolError_t error = AwsIotTaskPool_Create( &tpInfo, &pTaskPool );
* 
* // Use the task pool
* // ...
* 
* @endcode
*
*/
/* @[define_taskpool_initializers] */
#define AWS_IOT_TASKPOOL_INFO_INITIALIZER_SMALL      { .minThreads = 1, .maxThreads = 1, .stackSize = AWS_IOT_TASKPOOL_THREADS_STACK_SIZE, .priority = AWS_IOT_TASKPOOL_THREADS_PRIORITY }  /**< @brief Initializer for a small #AwsIotTaskPoolInfo_t. */
#define AWS_IOT_TASKPOOL_INFO_INITIALIZER_MEDIUM     { .minThreads = 1, .maxThreads = 2, .stackSize = AWS_IOT_TASKPOOL_THREADS_STACK_SIZE, .priority = AWS_IOT_TASKPOOL_THREADS_PRIORITY }  /**< @brief Initializer for a medium #AwsIotTaskPoolInfo_t. */
#define AWS_IOT_TASKPOOL_INFO_INITIALIZER_LARGE      { .minThreads = 2, .maxThreads = 3, .stackSize = AWS_IOT_TASKPOOL_THREADS_STACK_SIZE, .priority = AWS_IOT_TASKPOOL_THREADS_PRIORITY }  /**< @brief Initializer for a large #AwsIotTaskPoolInfo_t. */
#define AWS_IOT_TASKPOOL_INFO_INITIALIZER_XLARGE     { .minThreads = 2, .maxThreads = 4, .stackSize = AWS_IOT_TASKPOOL_THREADS_STACK_SIZE, .priority = AWS_IOT_TASKPOOL_THREADS_PRIORITY }  /**< @brief Initializer for a very large #AwsIotTaskPoolInfo_t. */
#define AWS_IOT_TASKPOOL_INFO_INITIALIZER            AWS_IOT_TASKPOOL_INFO_INITIALIZER_MEDIUM                                                                                               /**< @brief Initializer for a typical #AwsIotTaskPoolInfo_t. */
#define AWS_IOT_TASKPOOL_INITIALIZER                 { 0 }                                                                                                                                  /**< @brief Initializer for a #AwsIotTaskPoolJob_t. */
/* @[define_taskpool_initializers] */

/**
* @brief Allows the use of the handle to the system task pool.
*
* @warning The task pool handle is not valid unless @ref taskpool_function_createsystemtaskpool is 
* called before the handle is used.
*/
#define AWS_IOT_TASKPOOL_SYSTEM_TASKPOOL    ( AwsIotTaskPool_GetSystemTaskPool( ) )

#endif /* ifndef _AWS_IOT_TASKPOOL_TYPES_H_ */
