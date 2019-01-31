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
 * @file aws_iot_taskpool_internal.h
 * @brief Internal header of Task Pool library. This header should not be included in
 * typical application code.
 */

#ifndef _AWS_IOT_TASKPOOL_INTERNAL_H_
#define _AWS_IOT_TASKPOOL_INTERNAL_H_

 /* Task Pool include. */
#include "aws_iot_taskpool.h"

/* Logging. */
#include "aws_iot_logging.h"

/**
* @def AwsIotTaskPool_Assert( expression )
* @brief Assertion macro for the Task Pool library.
*
* Set @ref AWS_IOT_TASKPOOL_ENABLE_ASSERTS to `1` to enable assertions in the Task Pool
* library.
*
* @param[in] expression Expression to be evaluated.
*/
#if AWS_IOT_TASKPOOL_ENABLE_ASSERTS == 1
#ifndef AwsIotTaskPool_Assert
#include <assert.h>
#define AwsIotTaskPool_Assert( expression )    assert( expression )
#endif
#else
#define AwsIotTaskPool_Assert( expression )
#endif

/* Configure logs for TASKPOOL functions. */
#ifdef AWS_IOT_LOG_LEVEL_TASKPOOL
#define _LIBRARY_LOG_LEVEL        AWS_IOT_LOG_LEVEL_TASKPOOL
#else
#ifdef AWS_IOT_LOG_LEVEL_GLOBAL
#define _LIBRARY_LOG_LEVEL    AWS_IOT_LOG_LEVEL_GLOBAL
#else
#define _LIBRARY_LOG_LEVEL    AWS_IOT_LOG_NONE
#endif
#endif

#define _LIBRARY_LOG_NAME    ( "TASKPOOL" )
#include "aws_iot_logging_setup.h"

/**
* @brief Overridable allocator and deallocator: provide default values for undefined memory 
* allocation functions based on the usage of dynamic memory allocation.
*/
#if AWS_IOT_STATIC_MEMORY_ONLY == 1

#include "platform/aws_iot_static_memory.h"

/**
* @brief Overridable allocator.
*/
#ifndef AwsIotTaskPool_Malloc
#define AwsIotTaskPool_Malloc    !ERROR Not Implemented!
#endif

/**
* @brief Overridable deallocator.
*/
#ifndef AwsIotTaskPool_Free
#define AwsIotTaskPool_Free    !ERROR Not Implemented!
#endif

#else /* if AWS_IOT_STATIC_MEMORY_ONLY */

#include <stdlib.h>

/**
* @brief Overridable allocator.
*/
#ifndef AwsIotTaskPool_Malloc
#define AwsIotTaskPool_Malloc    malloc
#endif

/**
* @brief Overridable deallocator.
*/
#ifndef AwsIotTaskPool_Free
#define AwsIotTaskPool_Free    free
#endif

#endif /* if AWS_IOT_STATIC_MEMORY_ONLY */

/**
* @cond DOXYGEN_IGNORE
* Doxygen should ignore this section.
*
* Provide default values for undefined configuration constants.
*/
#ifndef AWS_IOT_TASKPOOL_WORKTEIM_RECYCLE_LIMIT
#define AWS_IOT_TASKPOOL_WORKTEIM_RECYCLE_LIMIT      ( 32 )
#endif
#ifndef AWS_IOT_TASKPOOL_THREADS_STACK_SIZE
#define AWS_IOT_TASKPOOL_THREADS_STACK_SIZE          ( 0 ) /* 0 will trigger the default stack size. */
#endif
#ifndef AWS_IOT_TASKPOOL_THREADS_PRIORITY
#define AWS_IOT_TASKPOOL_THREADS_PRIORITY            ( 0 )
#endif
#ifndef AWS_IOT_TASKPOOL_TEST
#define AWS_IOT_TASKPOOL_TEST                        (  0 )
#endif
/** @endcond */

/* ---------------------------------------------------------------------------------------------- */

/**
* @cond DOXYGEN_IGNORE
* Doxygen should ignore this section.
*
* A few macros to manage task pool status.
*/
#define AWS_IOT_TASKPOOL_STATUS_MASK                         0x0F /* Lower 4 bits reserved for status (AwsIotTaskPoolJobStatus_t). */
#define AWS_IOT_TASKPOOL_FLAGS_MASK                          0xF0 /* Upper 4 bits reserved for flags. */
/** @endcond */

#endif /* ifndef _AWS_IOT_TASKPOOL_INTERNAL_H_ */
