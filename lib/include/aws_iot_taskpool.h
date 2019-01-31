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
* @file aws_iot_taskpool.h
* @brief User-facing functions of the Task Pool library.
*/

#ifndef _AWS_IOT_TASKPOOL_H_
#define _AWS_IOT_TASKPOOL_H_

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
#include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* Task Pool types. */
#include "aws_iot_taskpool_types.h"

/*------------------------- TASKPOOL defined constants --------------------------*/

/**
* @brief Retrives the handle to one and only system task pool.
* The system-level task pool is used by all libraries, and can also be used by the user application.
*
* @warning The task pool handle is not valid unless @ref taskpool_function_createsystemtaskpool is 
* called before the handle is used.
*/
#define AWS_IOT_TASKPOOL_SYSTEM_TASKPOOL    ( AwsIotTaskPool_GetSystemTaskPool( ) )

/**
* @functionspage{taskpool,Task Pool library}
* - @functionname{taskpool_function_createsystemtaskpool}
* - @functionname{taskpool_function_getsystemtaskpool}
* - @functionname{taskpool_function_create}
* - @functionname{taskpool_function_destroy}
* - @functionname{taskpool_function_setmaxthreads}
* - @functionname{taskpool_function_createjob}
* - @functionname{taskpool_function_destroyjob}
* - @functionname{taskpool_function_schedule}
* - @functionname{taskpool_function_wait}
* - @functionname{taskpool_function_timedwait}
* - @functionname{taskpool_function_getstatus}
* - @functionname{taskpool_function_trycancel}
*/

/**
* @functionpage{AwsIotTaskPool_CreateSystemTaskPool,taskpool,createsystemtaskpool}
* @functionpage{AwsIotTaskPool_GetSystemTaskPool,taskpool,getsystemtaskpool}
* @functionpage{AwsIotTaskPool_Create,taskpool,create}
* @functionpage{AwsIotTaskPool_Destroy,taskpool,destroy}
* @functionpage{AwsIotTaskPool_SetMaxThreads,taskpool,setmaxthreads}
* @functionpage{AwsIotTaskPool_CreateJob,taskpool,createjob}
* @functionpage{AwsIotTaskPool_DestroyJob,taskpool,destroyjob}
* @functionpage{AwsIotTaskPool_Schedule,taskpool,schedule}
* @functionpage{AwsIotTaskPool_Wait,taskpool,wait}
* @functionpage{AwsIotTaskPool_TimedWait,taskpool,timedwait}
* @functionpage{AwsIotTaskPool_GetStatus,taskpool,getstatus}
* @functionpage{AwsIotTaskPool_TryCancel,taskpool,trycancel}
*/

/**
* @brief Initialization function for one instance of the System Task Pool of the Task Pool library.
*
* This function should be called once by the application to initialize the only instance of the system task pool. 
* An application should initialize the system task pool early in the boot sequence, before any library 
* that uses the system task pool has a chance to use it, or before any application code posts to the system task pool.
* Early initialization it typically easy to accomplish by creating the system task pool before the scheduler is started.
*
* This function does not allocate memory to hold the Task Pool data structures and state, but it 
* may allocate memory to hold the dependent entities and data structures, e.g. the threads of the task
* pool. The system task pool handle is recoverable for later use by calling (@ref taskpool_function_getsystemtaskpool) or
* the shortcut @ref AWS_IOT_TASKPOOL_SYSTEM_TASKPOOL.
*
* @param[in] pInfo A pointer to the Task Pool initialization data.
* 
* @return One of the following:
* - #AWS_IOT_TASKPOOL_SUCCESS
* - #AWS_IOT_TASKPOOL_BAD_PARAMETER
* - #AWS_IOT_TASKPOOL_NO_MEMORY
*
* @warning This function should be called only once. Calling this function more that once will result in 
* undefined behavior.
*
*/
/* @[declare_taskpool_createsystemtaskpool] */
AwsIotTaskPoolError_t AwsIotTaskPool_CreateSystemTaskPool( const AwsIotTaskPoolInfo_t * const pInfo );
/* @[declare_taskpool_createsystemtaskpool] */

/**
* @brief Retrieves the one and only instance of a system Task Pool of the Task Pool library.
*
* This function retrieves the sytem task pool created with (@ref taskpool_function_createsystemtaskpool).
*
* @return The system task pool handle.
*
* @warning This function should be called after creating the system task pool with @ref AwsIotTaskPool_CreateSystemTaskPool.
* Calling this function before creating the system task pool may return a pointer to an uninitialized task pool, NULL, or otherwise
* fail with undefined behaviour.
*
*/
/* @[declare_taskpool_getsystemtaskpool] */
AwsIotTaskPool_t * AwsIotTaskPool_GetSystemTaskPool( );
/* @[declare_taskpool_getsystemtaskpool] */

/**
* @brief Initialization function for one instance of a Task Pool of the Task Pool library.
*
* This function should be called by the user to initialiaze one instance of a Task
* Pool. The Task Pool instance will be created around the storage pointed to by the `pTaskPool` 
* parameter. This function will create the minimum number of threads requested by the user 
* through an instance of the #AwsIotTaskPoolInfo_t type specified with
* the `pInfo` parameter.
* This function does not allocate memory to hold the Task Pool data structures and state, but it 
* may allocates memory to hold the dependent data structures, e.g. the threads of the task
* pool.
*
* @param[in] pInfo A pointer to the Task Pool initialization data.
* @param[in,out] pTaskPool A pointer to the task pool storage to be initialized. 
* The pointer `pTaskPool` will hold a valid handle only if (@ref taskpool_function_create) 
* completes succesfully.
*
* @return One of the following:
* - #AWS_IOT_TASKPOOL_SUCCESS
* - #AWS_IOT_TASKPOOL_BAD_PARAMETER
* - #AWS_IOT_TASKPOOL_NO_MEMORY
*
*/
/* @[declare_taskpool_create] */
AwsIotTaskPoolError_t AwsIotTaskPool_Create( const AwsIotTaskPoolInfo_t * const pInfo, AwsIotTaskPool_t * const pTaskPool );
/* @[declare_taskpool_create] */

/**
* @brief De-initialization function for a Task Pool of the Task Pool library.
*
* This function should be called to destroy one instance of a Task
* Pool previously created with a call to @ref AwsIotTaskPool_Create or @ref AwsIotTaskPool_GetSystemTaskPool 
* and release all underlying resources. After calling this function, any job scheduled but not yet executed 
* will be cancelled and uninitialized, as if the user called @ref AwsIotTaskPool_DestroyJob on every job 
* previously scheduled. 
* The `pTaskPool` instance will no longer be valid after this function returns.
*
* @param[in] pTaskPool A handle to the task pool, e.g. as returned by a call to @ref taskpool_function_create. 
* The `pTaskPool` instance will no longer be valid after this function returns.
*  
* @return One of the following:
* - #AWS_IOT_TASKPOOL_SUCCESS
* - #AWS_IOT_TASKPOOL_BAD_PARAMETER
*
* @warning Calling this function will destroy the wait handle for a job, and that will result in undefined 
* behaviour if any thread was waiting on a job when the task pool is destroyed. 
*
*/
/* @[declare_taskpool_destroy] */
AwsIotTaskPoolError_t AwsIotTaskPool_Destroy( AwsIotTaskPool_t * pTaskPool );
/* @[declare_taskpool_destroy] */

/**
* @brief This function sets the maximum number of threads at runtime.
*
* This function sets the maximum number of threads for the task pool
* pointed to by `pTaskPool`.
*
* If the number of currently active threads in the task pool is greater than `maxThreads`, this
* function causes the task pool to shrink the number of active threads.
*
* @param[in] pTaskPool A handle to the task pool that must have been previously initialized with 
* a call to @ref taskpool_function_create.
* @param[in] maxThreads The maximum number of threads for the task pool.
*
* @return One of the following:
* - #AWS_IOT_TASKPOOL_SUCCESS
* - #AWS_IOT_TASKPOOL_BAD_PARAMETER
*
*/
/* @[declare_taskpool_setmaxthreads] */
AwsIotTaskPoolError_t AwsIotTaskPool_SetMaxThreads( AwsIotTaskPool_t * pTaskPool, uint32_t maxThreads );
/* @[declare_taskpool_setmaxthreads] */

/**
* @brief This function creates a job for the task pool around a user-provided storage.
*
* This function may allocate memory to hold the state for a job and a wait handle. 
*
* @param[in] userCallback A user-specified callback for the job.
* @param[in] pUserContext A user-specified context for the callback.
* @param[out] pJob A pointer to an instance of @ref AwsIotTaskPoolJob_t that will be initialized when this 
* function returns succesfully. This handle can be used to wait on the job with @ref AwsIotTaskPool_Wait, 
* inspect the job status with @ref AwsIotTaskPool_GetStatus or cancel the job with @ref AwsIotTaskPool_TryCancel.
*
* @return One of the following:
* - #AWS_IOT_TASKPOOL_SUCCESS
* - #AWS_IOT_TASKPOOL_BAD_PARAMETER
* - #AWS_IOT_TASKPOOL_NO_MEMORY
* - #AWS_IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS
*
*
*/
/* @[declare_taskpool_createjob] */
AwsIotTaskPoolError_t AwsIotTaskPool_CreateJob(
    const IotTaskPoolRoutine_t userCallback,
    void * const pUserContext,
    AwsIotTaskPoolJob_t * const pJob );
/* @[declare_taskpool_createjob] */

/**
* @brief This function uninitializes a job.
*
* This function will destroy a job created with @ref AwsIotTaskPool_CreateJob. A job should not be destroyed twice. A job 
* that was previously scheduled but has not completed yet or a job that was successfully canceled cannot be destroyed. 
* An attempt to do so will result in an @ref AWS_IOT_TASKPOOL_ILLEGAL_OPERATION error.
*
* @param[in] pJob A handle to a job that was create with a call to @ref AwsIotTaskPool_CreateJob.
*
* @return One of the following:
* - #AWS_IOT_TASKPOOL_SUCCESS
* - #AWS_IOT_TASKPOOL_BAD_PARAMETER
* - #AWS_IOT_TASKPOOL_ILLEGAL_OPERATION
*
* @warning The task pool will try and prevent destroying jobs that are currently queued for execution, but does 
* not enforce strict ordering of operations. It is up to the user to make sure @ref AwsIotTaskPool_DestroyJob is not called
* our of order.
*
*/
/* @[declare_taskpool_destroyjob] */
AwsIotTaskPoolError_t AwsIotTaskPool_DestroyJob( AwsIotTaskPoolJob_t * const pJob );
/* @[declare_taskpool_destroyjob] */

/**
* @brief This function schedules a job created with @ref taskpool_function_createjob against the task pool
* pointed to by `pTaskPool`.
*
* See @ref taskpool_design for a description of the jobs lifetime and interaction with the threads used in the Task Pool
* library.
* 
* @param[in] pTaskPool A handle to the task pool that must have been previously initialized with 
* a call to @ref taskpool_function_create.
* @param[in] pJob A job to schedule for execution. This must be first initialized with a call to @ref taskpool_function_createjob.
*
* @return One of the following:
* - #AWS_IOT_TASKPOOL_SUCCESS
* - #AWS_IOT_TASKPOOL_BAD_PARAMETER
* - #AWS_IOT_TASKPOOL_ILLEGAL_OPERATION
* - #AWS_IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS
*
*
* @note This function will not allocate memory. 
*
* @warning The `pTaskPool` used in this function should be the same
* used to create the job pointed to by `pJob`, or the results will be undefined.
*
* <b>Example</b>
* @code{c}
* // An example of a user context to pass to a callback through a Task Pool thread.
* typedef struct JobUserContext
* {
*     uint32_t counter;
* } JobUserContext_t;
*
* // An example of a user callback to invoke through a Task Pool thread. 
* static void ExecutionCb( AwsIotTaskPool_t * pTaskPool, AwsIotTaskPoolJob_t * pJob, void * context )
* {
*     ( void )pTaskPool;
*     ( void )pJob;
*
*     JobUserContext_t * pUserContext = ( JobUserContext_t * )context;
*
*     pUserContext->counter++;
*
*     // Destroy the job.
*     AwsIotTaskPool_DestroyJob( pTaskPool, pJob );
* }
*
* void TaskPoolExample( )
* {
*     JobUserContext_t userContext = { 0 };
*     AwsIotTaskPoolJob_t job;
*     AwsIotTaskPool_t * pTaskPool;
*
*     // Configure the task pool to hold at least two threads and three at the maximum.
*     // Provide proper stack size and priority per the application needs.
*
*     AwsIotTaskPoolInfo_t tpInfo = { .minThreads = 2, .maxThreads = 3, .stackSize = 512, .priority = 0 };
*
*     // Create a task pool.
*     AwsIotTaskPool_Create( &tpInfo, &pTaskPool );
*
*     // Statically allocate one job, schedule it, then wait.
*     AwsIotTaskPool_CreateJob( &ExecutionCb, &userContext, &job );
*
*     AwsIotTaskPoolError_t errorSchedule = AwsIotTaskPool_Schedule( pTaskPool, &job );
*
*     switch ( errorSchedule )
*     {
*     case AWS_IOT_TASKPOOL_SUCCESS:
*         break;
*     case AWS_IOT_TASKPOOL_BAD_PARAMETER:          // Invalid parameters, such as a NULL handle, can trigger this condition.
*     case AWS_IOT_TASKPOOL_ILLEGAL_OPERATION:      // Scheduling a job that was previously scheduled or destroyed could trigger this condition.
*     case AWS_IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS:   // Scheduling a job after trying to destroy the task pool could trigger this operation.
*         // ASSERT
*         break;
*     default:
*         // ASSERT
*     }
*
*     //
*     // ... Perform other operations ...
*     //
*
*     // Wait for the operation to be completed.
*     AwsIotTaskPoolError_t errorWait =  AwsIotTaskPool_Wait( pTaskPool, &job );
*
*     switch ( errorWait )
*     {
*     case AWS_IOT_TASKPOOL_SUCCESS:
*         break;
*     case AWS_IOT_TASKPOOL_TIMEDOUT:
*         // ASSERT: Timeout cannot happen because AwsIotTaskPool_Wait waits forever.
*         break;
*     case AWS_IOT_TASKPOOL_BAD_PARAMETER:          // Invalid parameters, such as a NULL handle, can trigger this condition.
*     case AWS_IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS:   // Scheduling a job after destroying the task pool could trigger this operation.
*         // ASSERT: Make sure the application did not shutdown the task pool and tried to use it afterwards.
*         break;
*     default:
*         // ASSERT: AwsIotTaskPool_Wait should not return any other value.
*     }
*
*     //
*     // ... Perform other operations ...
*     //
*
*     AwsIotTaskPool_Destroy( pTaskPool );
* }
* @endcode
*/
/* @[declare_taskpool_schedule] */
AwsIotTaskPoolError_t AwsIotTaskPool_Schedule( AwsIotTaskPool_t * const pTaskPool, AwsIotTaskPoolJob_t * const pJob );
/* @[declare_taskpool_schedule] */

/**
* @brief This function blocks the calling thread until the job specified by `pJob` completes or is canceled.
*
* See @ref taskpool_function_schedule for a complete flow involving scheduling and waiting for jobs to complete. 
* 
* @param[in] pTaskPool A handle to the task pool that must have been previously initialized with 
* a call to @ref taskpool_function_create.
* @param[in] pJob The job to be wait on.
*
* @return One of the following:
* - #AWS_IOT_TASKPOOL_SUCCESS
* - #AWS_IOT_TASKPOOL_BAD_PARAMETER
* - #AWS_IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS
*
* @warning This function can be called only once for each scheduled job.
*
*/
/* @[declare_taskpool_wait] */
AwsIotTaskPoolError_t AwsIotTaskPool_Wait( const AwsIotTaskPool_t * pTaskPool, AwsIotTaskPoolJob_t * const pJob );
/* @[declare_taskpool_wait] */

/**
* @brief This function blocks the calling thread until the job completes, is canceled, or the timeout elapses.
*
* See @ref taskpool_function_schedule for a complete flow involving scheduling and waiting for jobs to complete. 
* 
* @param[in] pTaskPool A handle to the task pool that must have been previously initialized with 
* a call to @ref taskpool_function_create.
* @param[in] pJob The job to be wait on.
* @param[in] timeoutMs The time in milliseconds to wait for the job to be executed.
*
* @return One of the following:
* - #AWS_IOT_TASKPOOL_SUCCESS
* - #AWS_IOT_TASKPOOL_BAD_PARAMETER
* - #AWS_IOT_TASKPOOL_TIMEDOUT
* - #AWS_IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS
*
* @warning This function may allocate memory if the job to wait on does not have yet an associated synchronization object.
*
* @warning The `pTaskPool` used in this function should be the same
* used to create the job pointed to by `pJob`, or the results will be undefined.
*
*/
/* @[declare_taskpool_timedwait] */
AwsIotTaskPoolError_t AwsIotTaskPool_TimedWait( const AwsIotTaskPool_t * pTaskPool, AwsIotTaskPoolJob_t * const pJob, uint64_t timeoutMs );
/* @[declare_taskpool_timedwait] */

/**
* @brief This function retrieves the current status of a job.
*
* @param[in] pJob The job to cancel.
* @param[out] pStatus The status of the job at the time of cancellation.
*
* @return One of the following:
* - #AWS_IOT_TASKPOOL_SUCCESS
* - #AWS_IOT_TASKPOOL_BAD_PARAMETER
*
* @warning This function is not thread safe and the job status returned in `pStatus` may be invalid by the time
* the calling thread has a chance to inspect it.
*/
/* @[declare_taskpool_getstatus] */
AwsIotTaskPoolError_t AwsIotTaskPool_GetStatus( const AwsIotTaskPoolJob_t * pJob, AwsIotTaskPoolJobStatus_t * const pStatus );
/* @[declare_taskpool_getstatus] */

/**
* @brief This function tries to cancel a job that was previously scheduled with @ref AwsIotTaskPool_Schedule.
*
* A job can be canceled only if it is not yet executing, i.e. if its status is 
* @ref AWS_IOT_TASKPOOL_STATUS_READY or @ref AWS_IOT_TASKPOOL_STATUS_SCHEDULED. Calling 
* @ref AwsIotTaskPool_TryCancel on a job whose status is @ref AWS_IOT_TASKPOOL_STATUS_EXECUTING, 
* @ref AWS_IOT_TASKPOOL_STATUS_COMPLETED, or @AWS_IOT_TASKPOOL_STATUS_CANCELED will yield 
* a @AWS_IOT_TASKPOOL_CANCEL_FAILED return result.
*
* @param[in] pTaskPool A handle to the task pool that must have been previously initialized with 
* a call to @ref taskpool_function_create.
* @param[in] pJob The job to cancel.
* @param[out] pStatus The status of the job at the time of cancellation.
*
* @return One of the following:
* - #AWS_IOT_TASKPOOL_SUCCESS
* - #AWS_IOT_TASKPOOL_BAD_PARAMETER
* - #AWS_IOT_TASKPOOL_CANCEL_FAILED
* - #AWS_IOT_TASKPOOL_SHUTDOWN_IN_PROGRESS
*
* @warning The `pTaskPool` used in this function should be the same
* used to create the job pointed to by `pJob`, or the results will be undefined.
*
*/
/* @[declare_taskpool_trycancel] */
AwsIotTaskPoolError_t AwsIotTaskPool_TryCancel( const AwsIotTaskPool_t * pTaskPool, AwsIotTaskPoolJob_t * const pJob, AwsIotTaskPoolJobStatus_t * const pStatus );
/* @[declare_taskpool_trycancel] */

#endif /* ifndef _AWS_IOT_TASKPOOL_H_ */
