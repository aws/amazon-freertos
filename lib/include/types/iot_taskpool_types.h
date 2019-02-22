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
 * @file iot_taskpool_types.h
 * @brief Types of the task pool.
 */

#ifndef _IOT_TASKPOOL_TYPES_H_
#define _IOT_TASKPOOL_TYPES_H_

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stdbool.h>
#include <stdint.h>

/* Platform types includes. */
#include "types/iot_platform_types.h"

/* Linear containers (lists and queues) include. */
#include "iot_linear_containers.h"

/*-------------------------- Task pool enumerated types --------------------------*/

/**
 * @enums{taskpool,Task pool library}
 */

/**
 * @ingroup taskpool_datatypes_enums
 * @brief Return codes of [task pool functions](@ref taskpool_functions).
 */
typedef enum IotTaskPoolError
{
    /**
     * @brief Task pool operation completed successfully.
     *
     * Functions that may return this value:
     * - @ref taskpool_function_createsystemtaskpool
     * - @ref taskpool_function_create
     * - @ref taskpool_function_destroy
     * - @ref taskpool_function_setmaxthreads
     * - @ref taskpool_function_createjob
     * - @ref taskpool_function_destroyjob
     * - @ref taskpool_function_recyclejob
     * - @ref taskpool_function_schedule
     * - @ref taskpool_function_scheduledeferred
     * - @ref taskpool_function_getstatus
     * - @ref taskpool_function_trycancel
     *
     */
    IOT_TASKPOOL_SUCCESS = 0,

    /**
     * @brief Task pool operation failed because at laest one parameter is invalid.
     *
     * Functions that may return this value:
     * - @ref taskpool_function_createsystemtaskpool
     * - @ref taskpool_function_create
     * - @ref taskpool_function_destroy
     * - @ref taskpool_function_setmaxthreads
     * - @ref taskpool_function_createjob
     * - @ref taskpool_function_destroyjob
     * - @ref taskpool_function_recyclejob
     * - @ref taskpool_function_schedule
     * - @ref taskpool_function_scheduledeferred
     * - @ref taskpool_function_getstatus
     * - @ref taskpool_function_trycancel
     *
     */
    IOT_TASKPOOL_BAD_PARAMETER,

    /**
     * @brief Task pool operation failed because it is illegal.
     *
     * Functions that may return this value:
     * - @ref taskpool_function_destroyjob
     * - @ref taskpool_function_recyclejob
     * - @ref taskpool_function_schedule
     * - @ref taskpool_function_scheduledeferred
     * - @ref taskpool_function_trycancel
     *
     */
    IOT_TASKPOOL_ILLEGAL_OPERATION,

    /**
     * @brief Task pool operation failed because allocating memory failed.
     *
     * Functions that may return this value:
     * - @ref taskpool_function_createsystemtaskpool
     * - @ref taskpool_function_create
     * - @ref taskpool_function_setmaxthreads
     * - @ref taskpool_function_createrecyclablejob
     * - @ref taskpool_function_scheduledeferred
     * - @ref taskpool_function_getstatus
     *
     */
    IOT_TASKPOOL_NO_MEMORY,

    /**
     * @brief Task pool operation failed because of an invalid parameter.
     *
     * Functions that may return this value:
     * - @ref taskpool_function_setmaxthreads
     * - @ref taskpool_function_createrecyclablejob
     * - @ref taskpool_function_destroyjob
     * - @ref taskpool_function_recyclejob
     * - @ref taskpool_function_schedule
     * - @ref taskpool_function_scheduledeferred
     * - @ref taskpool_function_getstatus
     * - @ref taskpool_function_trycancel
     *
     */
    IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS,

    /**
     * @brief Task pool cancellation failed.
     *
     * Functions that may return this value:
     * - @ref taskpool_function_trycancel
     *
     */
    IOT_TASKPOOL_CANCEL_FAILED,
} IotTaskPoolError_t;

/**
 * @enums{taskpool,Task pool library}
 */

/**
 * @ingroup taskpool_datatypes_enums
 * @brief Status codes of [task pool Job](@ref IotTaskPoolJob_t).
 *
 */
typedef enum IotTaskPoolJobStatus
{
    /**
     * @brief Job is ready to be scheduled.
     *
     */
    IOT_TASKPOOL_STATUS_READY = 0,

    /**
     * @brief Job has been queued for execution.
     *
     */
    IOT_TASKPOOL_STATUS_SCHEDULED,

    /**
     * @brief Job has been scheduled for deferred execution.
     *
     */
    IOT_TASKPOOL_STATUS_DEFERRED,

    /**
     * @brief Job is executing.
     *
     */
    IOT_TASKPOOL_STATUS_EXECUTING,

    /**
     * @brief Job has been canceled before executing.
     *
     */
    IOT_TASKPOOL_STATUS_CANCELED,

    /**
     * @brief Job status is undefined.
     *
     */
    IOT_TASKPOOL_STATUS_UNDEFINED,
} IotTaskPoolJobStatus_t;

/**
 * @cond DOXYGEN_IGNORE
 * @brief Forward declarations for task pool types.
 *
 */
struct IotTaskPool;
struct IotTaskPoolJob;
/** @endcond */

/*---------------------------- Task pool function pointer types ----------------------------*/

/**
 * @functionpointers{taskpool,task pool library}
 */

/**
 * @ingroup taskpool_datatypes_functionpointers
 * @brief Callback type for a user callback.
 *
 * This type identifies the user callback signature to execute a task pool job. This callback will be invoked
 * by the task pool threads with the `pUserContext` parameter, as speficied by the user when
 * calling @ref IotTaskPool_Schedule.
 *
 */
typedef void ( * IotTaskPoolRoutine_t )( struct IotTaskPool * pTaskPool,
                                         struct IotTaskPoolJob * pJob,
                                         void * pUserContext );

/*------------------------- Task pool parameter structs --------------------------*/

/**
 * @paramstructs{taskpool,task pool}
 */

/**
 * @ingroup taskpool_datatypes_paramstructs
 * @brief Initialization information to create one task pool instance.
 *
 * @paramfor  @ref taskpool_function_createsystemtaskpool @ref taskpool_function_create.
 *
 * Passed as an argument to @ref taskpool_function_create.
 *
 * @initializer{IotTaskPoolInfo_t,IOT_TASKPOOL_INFO_INITIALIZER}
 */
typedef struct IotTaskPoolInfo
{
    /**
     * @brief Specifies the operating parameters for a task pool.
     *
     * @attention #IotTaskPoolInfo_t.minThreads <b>MUST</b> be at least 1.
     * #IotTaskPoolInfo_t.maxThreads <b>MUST</b> be greater or equal to #IotTaskPoolInfo_t.minThreads.
     * If the minimum number of threads is same as the maximum, then the task pool will not try and grow the
     * number of worker threads at run time.
     */

    uint32_t minThreads; /**< @brief Minimum number of threads in a task pool. These threads will be created when the task pool is first created with @ref taskpool_function_create. */
    uint32_t maxThreads; /**< @brief Maximum number of threads in a task pool. A task pool may try and grow the number of active threads up to #AwsIotTaskPoolInfo_t.maxThreads. */
    uint32_t stackSize;  /**< @brief Stack size for every task pool thread. The stack size for each thread is fixed after the task pool is created and cannot be changed. */
    uint32_t priority;   /**< @brief priority for every task pool thread. The priority for each thread is fixed after the task pool is created and cannot be changed. */
} IotTaskPoolInfo_t;

/*------------------------- Task pool handles structs --------------------------*/

/**
 * @ingroup taskpool_datatypes_types
 * @brief Task pool jobs cache.
 *
 * @warning This is a system-level data type that should not be modified.
 *
 */
typedef struct IotTaskPoolCache
{
    IotListDouble_t freeList; /**< @brief A list ot hold cached jobs. */
    uint32_t freeCount;       /**< @brief A counter to track the number of jobs in the cache. */
} IotTaskPoolCache_t;


/**
 * @ingroup taskpool_datatypes_types
 * @brief The task pool data structure keeps track of the internal state and the signals for the dispatcher threads.
 * The task pool is a thread safe data structure.
 *
 * @warning This is a system-level data type that should not be modified.
 *
 */
typedef struct IotTaskPool
{
    IotQueue_t dispatchQueue;        /**< @brief The queue for the jobs waiting to be executed. */
    IotListDouble_t timerEventsList; /**< @brief The timeouts queue for all deferred jobs waiting to be executed. */
    IotTaskPoolCache_t jobsCache;    /**< @brief A cache to re-use jobs in order to limit memory allocations. */
    uint32_t minThreads;             /**< @brief The minimum number of threads for the task pool. */
    uint32_t maxThreads;             /**< @brief The maximum number of threads for the task pool. */
    uint32_t activeThreads;          /**< @brief The number of threads in the task pool at any given time. */
    uint32_t activeJobs;             /**< @brief The number of active jobs in the task pool at any given time. */
    size_t stackSize;                /**< @brief The stack size for all task pool threads. */
    int32_t priority;                /**< @brief The priority for all task pool threads. */
    IotSemaphore_t dispatchSignal;   /**< @brief The synchronization object on which threads are waiting for incoming jobs. */
    IotSemaphore_t startStopSignal;  /**< @brief The synchronization object for threads to signal start and stop condition. */
    IotTimer_t timer;                /**< @brief The timer for deferred jobs. */
    IotMutex_t lock;                 /**< @brief The lock to protect the task pool data structure access. */
} IotTaskPool_t;

/**
 * @ingroup taskpool_datatypes_types
 * @brief The job data structure keeps track of the user callback and context, as well as the status of the job.
 *
 * @warning This is a system-level data type that should not be modified.
 *
 */
typedef struct IotTaskPoolJob
{
    IotTaskPoolRoutine_t userCallback; /**< @brief The user provided callback. */
    void * pUserContext;               /**< @brief The user provided context. */
    IotLink_t link;                    /**< @brief The link to insert the job in the dispatch queue. */
    IotTaskPoolJobStatus_t status;     /**< @brief The status for the job. */
} IotTaskPoolJob_t;

/*------------------------- TASKPOOL defined constants --------------------------*/

/**
 * @constantspage{taskpool,task pool library}
 *
 * @section taskpool_constants_initializers task pool Initializers
 * @brief Provides default values for initializing the data types of the task pool library.
 *
 * @snippet this define_taskpool_initializers
 *
 * All user-facing data types of the task pool library can be initialized using
 * one of the following.
 *
 * @warning Failure to initialize a task pool data type with the appropriate initializer
 * may result in a runtime error!
 * @note The initializers may change at any time in future versions, but their
 * names will remain the same.
 *
 * <b>Example</b>
 * @code{c}
 *
 * IotTaskPool_t * pTaskPool;
 *
 * const IotTaskPoolInfo_t tpInfo = IOT_TASKPOOL_INFO_INITIALIZER_LARGE;
 *
 * IotTaskPoolError_t error = IotTaskPool_Create( &tpInfo, &pTaskPool );
 *
 * // Use the task pool
 * // ...
 *
 * @endcode
 *
 */
/* @[define_taskpool_initializers] */
#define IOT_TASKPOOL_INFO_INITIALIZER_SMALL     { .minThreads = 1, .maxThreads = 1, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY } /**< @brief Initializer for a small #IotTaskPoolInfo_t. */
#define IOT_TASKPOOL_INFO_INITIALIZER_MEDIUM    { .minThreads = 1, .maxThreads = 2, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY } /**< @brief Initializer for a medium #IotTaskPoolInfo_t. */
#define IOT_TASKPOOL_INFO_INITIALIZER_LARGE     { .minThreads = 2, .maxThreads = 3, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY } /**< @brief Initializer for a large #IotTaskPoolInfo_t. */
#define IOT_TASKPOOL_INFO_INITIALIZER_XLARGE    { .minThreads = 2, .maxThreads = 4, .stackSize = IOT_THREAD_DEFAULT_STACK_SIZE, .priority = IOT_THREAD_DEFAULT_PRIORITY } /**< @brief Initializer for a very large #IotTaskPoolInfo_t. */
#define IOT_TASKPOOL_INFO_INITIALIZER           IOT_TASKPOOL_INFO_INITIALIZER_MEDIUM                                                                                  /**< @brief Initializer for a typical #IotTaskPoolInfo_t. */
#define IOT_TASKPOOL_INITIALIZER                { 0 }                                                                                                                     /**< @brief Initializer for a #IotTaskPoolJob_t. */
/* @[define_taskpool_initializers] */

/**
 * @brief Allows the use of the handle to the system task pool.
 *
 * @warning The task pool handle is not valid unless @ref taskpool_function_createsystemtaskpool is
 * called before the handle is used.
 */
#define IOT_SYSTEM_TASKPOOL    ( IotTaskPool_GetSystemTaskPool() )

#endif /* ifndef _IOT_TASKPOOL_TYPES_H_ */
