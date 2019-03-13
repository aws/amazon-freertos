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
 * @file iot_static_memory_taskpool.c
 * @brief Implementation of task pool static memory functions in iot_static_memory.h
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* This file should only be compiled if dynamic memory allocation is forbidden. */
#if IOT_STATIC_MEMORY_ONLY == 1

/* Standard includes. */
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/* Static memory include. */
#include "private/iot_static_memory.h"

/* Task pool internal include. */
#include "private/iot_taskpool_internal.h"

/*-----------------------------------------------------------*/

/* Validate static memory configuration settings. */
#if IOT_TASKPOOL_JOBS_RECYCLE_LIMIT <= 0
    #error "IOT_TASKPOOL_JOBS_RECYCLE_LIMIT cannot be 0 or negative."
#endif

/*-----------------------------------------------------------*/

/* Extern declarations of common static memory functions in iot_static_memory_common.c
 * Because these functions are specific to this static memory implementation, they are
 * not placed in the common static memory header file. */
extern int IotStaticMemory_FindFree( bool * const pInUse,
                                     int limit );
extern void IotStaticMemory_ReturnInUse( void * ptr,
                                         void * const pPool,
                                         bool * const pInUse,
                                         int limit,
                                         size_t elementSize );

/*-----------------------------------------------------------*/

/*
 * Static memory buffers and flags, allocated and zeroed at compile-time.
 */
static bool _pInUseTaskPoolJobs[ IOT_TASKPOOL_JOBS_RECYCLE_LIMIT ] = { 0 };                       /**< @brief Task pool jobs in-use flags. */
static IotTaskPoolJob_t _pTaskPoolJobs[ IOT_TASKPOOL_JOBS_RECYCLE_LIMIT ] = { { 0 } };            /**< @brief Task pool jobs. */

static bool _pInUseTaskPoolTimerEvents[ IOT_TASKPOOL_JOBS_RECYCLE_LIMIT ] = { 0 };                /**< @brief Task pool timer event in-use flags. */
static _taskPoolTimerEvent_t _pTaskPoolTimerEvents[ IOT_TASKPOOL_JOBS_RECYCLE_LIMIT ] = { 0 };    /**< @brief Task pool timer events. */

/*-----------------------------------------------------------*/

void * Iot_MallocTaskPoolJob( size_t size )
{
    int freeIndex = -1;
    void * pNewJob = NULL;

    /* Check size argument. */
    if( size == sizeof( IotTaskPoolJob_t ) )
    {
        /* Find a free task pool job. */
        freeIndex = IotStaticMemory_FindFree( _pInUseTaskPoolJobs,
                                              IOT_TASKPOOL_JOBS_RECYCLE_LIMIT );

        if( freeIndex != -1 )
        {
            pNewJob = &( _pTaskPoolJobs[ freeIndex ] );
        }
    }

    return pNewJob;
}

/*-----------------------------------------------------------*/

void Iot_FreeTaskPoolJob( void * ptr )
{
    /* Return the in-use task pool job. */
    IotStaticMemory_ReturnInUse( ptr,
                                 _pTaskPoolJobs,
                                 _pInUseTaskPoolJobs,
                                 IOT_TASKPOOL_JOBS_RECYCLE_LIMIT,
                                 sizeof( IotTaskPoolJob_t ) );
}

/*-----------------------------------------------------------*/

void * Iot_MallocTaskPoolTimerEvent( size_t size )
{
    int freeIndex = -1;
    void * pNewTimerEvent = NULL;

    /* Check size argument. */
    if( size == sizeof( _taskPoolTimerEvent_t ) )
    {
        /* Find a free task pool timer event. */
        freeIndex = IotStaticMemory_FindFree( _pInUseTaskPoolTimerEvents,
                                              IOT_TASKPOOL_JOBS_RECYCLE_LIMIT );

        if( freeIndex != -1 )
        {
            pNewTimerEvent = &( _pTaskPoolTimerEvents[ freeIndex ] );
        }
    }

    return pNewTimerEvent;
}

/*-----------------------------------------------------------*/

void Iot_FreeTaskPoolTimerEvent( void * ptr )
{
    /* Return the in-use task pool timer event. */
    IotStaticMemory_ReturnInUse( ptr,
                                 _pTaskPoolTimerEvents,
                                 _pInUseTaskPoolTimerEvents,
                                 IOT_TASKPOOL_JOBS_RECYCLE_LIMIT,
                                 sizeof( _taskPoolTimerEvent_t ) );
}

/*-----------------------------------------------------------*/

#endif
