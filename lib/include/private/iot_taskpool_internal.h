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
 * @file iot_taskpool_internal.h
 * @brief Internal header of task pool library. This header should not be included in
 * typical application code.
 */

#ifndef _IOT_TASKPOOL_INTERNAL_H_
#define _IOT_TASKPOOL_INTERNAL_H_

/* Task pool include. */
#include "private/iot_error.h"
#include "iot_taskpool.h"

/* Establish a few convenience macros to handle errors in a standard way. */

/**
 * @brief Every public API return an enumeration value with an undelying value of 0 in case of success.
 */
#define _TASKPOOL_SUCCEEDED( x )                       ( ( x ) == IOT_TASKPOOL_SUCCESS )

/**
 * @brief Every public API returns an enumeration value with an undelying value different than 0 in case of success.
 */
#define _TASKPOOL_FAILED( x )                          ( ( x ) != IOT_TASKPOOL_SUCCESS )

/**
 * @brief Jump to the cleanup area.
 */
#define _TASKPOOL_GOTO_CLEANUP()                       _IOT_GOTO_CLEANUP()

 /**
 * @brief Declare the storage for the error status variable.
 */
#define _TASKPOOL_FUNCTION_ENTRY( result )             _IOT_FUNCTION_ENTRY( IotTaskPoolError_t, result )

#define _TASKPOOL_NO_FUNCTION_CLEANUP_NOLABEL()        return status

/**
 * @brief Check error and leave in case of failure.
 */
#define _TASKPOOL_ON_ERROR_GOTO_CLEANUP( expr )        { if( _TASKPOOL_FAILED( status = ( expr ) ) ) { _IOT_GOTO_CLEANUP(); } }

/**
 * @brief Set error and leave.
 */
#define _TASKPOOL_SET_AND_GOTO_CLEANUP( expr )          _IOT_SET_AND_GOTO_CLEANUP( expr )

/**
 * @brief Initialize error and declare start of cleanup area.
 */
#define _TASKPOOL_FUNCTION_CLEANUP()                   _IOT_FUNCTION_CLEANUP_BEGIN( )

/**
 * @brief Initialize error and declare end of cleanup area.
 */
#define _TASKPOOL_FUNCTION_CLEANUP_END()               _IOT_FUNCTION_CLEANUP_END()

/**
 * @brief Create an empty cleanup area.
 */
#define _TASKPOOL_NO_FUNCTION_CLEANUP()                _IOT_FUNCTION_EXIT_NO_CLEANUP()

/**
 * @brief Exit if an argument is NULL.
 */
#define _TASKPOOL_ON_NULL_ARG_GOTO_CLEANUP( ptr )      _IOT_VALIDATE_PARAMETER( IOT_TASKPOOL, ( ptr != NULL ) )

/**
 * @brief Exit if an argument is NULL.
 */
#define _TASKPOOL_ON_ARG_ERROR_GOTO_CLEANUP( expr )    _IOT_VALIDATE_PARAMETER( IOT_TASKPOOL, ( ( expr ) == false ) )


/**
 * @def IotTaskPool_Assert( expression )
 * @brief Assertion macro for the Task pool library.
 *
 * Set @ref IOT_TASKPOOL_ENABLE_ASSERTS to `1` to enable assertions in the Task pool
 * library.
 *
 * @param[in] expression Expression to be evaluated.
 */
#if IOT_TASKPOOL_ENABLE_ASSERTS == 1
    #ifndef IotTaskPool_Assert
        #include <assert.h>
        #define IotTaskPool_Assert( expression )    assert( expression )
    #endif
#else
    #define IotTaskPool_Assert( expression )
#endif

/* Configure logs for TASKPOOL functions. */
#ifdef IOT_LOG_LEVEL_TASKPOOL
    #define _LIBRARY_LOG_LEVEL        IOT_LOG_LEVEL_TASKPOOL
#else
    #ifdef IOT_LOG_LEVEL_GLOBAL
        #define _LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
    #else
        #define _LIBRARY_LOG_LEVEL    IOT_LOG_NONE
    #endif
#endif

#define _LIBRARY_LOG_NAME    ( "TASKPOOL" )
#include "iot_logging_setup.h"

/**
 * @brief Overridable allocator and deallocator: provide default values for undefined memory
 * allocation functions based on the usage of dynamic memory allocation.
 */
#if IOT_STATIC_MEMORY_ONLY == 1
    #include "private/iot_static_memory.h"

/**
 * @brief Overridable allocator.
 */
    #ifndef IotTaskPool_MallocJob
        #define IotTaskPool_MallocJob    Iot_MallocTaskPoolJob
    #endif

    #ifndef IotTaskPool_MallocTimerEvent
        #define IotTaskPool_MallocTimerEvent    Iot_MallocTaskPoolTimerEvent
    #endif

/**
 * @brief Overridable deallocator.
 */
    #ifndef IotTaskPool_FreeJob
        #define IotTaskPool_FreeJob    Iot_FreeTaskPoolJob
    #endif

    #ifndef IotTaskPool_FreeTimerEvent
        #define IotTaskPool_FreeTimerEvent    Iot_FreeTaskPoolTimerEvent
    #endif

#else /* if IOT_STATIC_MEMORY_ONLY == 1 */
    #include <stdlib.h>

/**
 * @brief Overridable allocator.
 */
    #ifndef IotTaskPool_MallocJob
        #define IotTaskPool_MallocJob    malloc
    #endif

    #ifndef IotTaskPool_MallocTimerEvent
        #define IotTaskPool_MallocTimerEvent    malloc
    #endif

/**
 * @brief Overridable deallocator.
 */
    #ifndef IotTaskPool_FreeJob
        #define IotTaskPool_FreeJob    free
    #endif

    #ifndef IotTaskPool_FreeTimerEvent
        #define IotTaskPool_FreeTimerEvent    free
    #endif

#endif /* if IOT_STATIC_MEMORY_ONLY == 1 */

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default values for undefined configuration constants.
 */
#ifndef IOT_TASKPOOL_JOBS_RECYCLE_LIMIT
    #define IOT_TASKPOOL_JOBS_RECYCLE_LIMIT    ( 32 )
#endif
/** @endcond */

/* ---------------------------------------------------------------------------------------------- */

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * A few macros to manage task pool status.
 */
#define IOT_TASKPOOL_STATUS_MASK         0x0F                 /* Lower 4 bits reserved for status (IotTaskPoolJobStatus_t). */
#define IOT_TASKPOOL_FLAGS_MASK          0xF0                 /* Upper 4 bits reserved for flags. */
#define IOT_TASK_POOL_INTERNAL_STATIC    0x80                 /* Flag to mark a job as user-allocated. */
/** @endcond */

/**
 * @brief Represents an operation that is subject to a timer.
 *
 * These events are queued per MQTT connection. They are sorted by their
 * expiration time.
 */
typedef struct _taskPoolTimerEvent
{
    IotLink_t link;          /**< @brief List link member. */
    uint64_t expirationTime; /**< @brief When this event should be processed. */
    IotTaskPoolJob_t * pJob; /**< @brief The task pool job associated with this event. */
} _taskPoolTimerEvent_t;

#endif /* ifndef _IOT_TASKPOOL_INTERNAL_H_ */
