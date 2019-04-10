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

/* Defender include. */
    #include "private/aws_iot_defender_internal.h"

    #ifndef AWS_IOT_DEFENDER_REPORTS
        #define AWS_IOT_DEFENDER_REPORTS    ( 1 )
    #endif

    #ifndef AWS_IOT_DEFENDER_TOPICS
        #define AWS_IOT_DEFENDER_TOPICS    ( 3 )
    #endif

/* Validate static memory configuration settings. */
    #if AWS_IOT_DEFENDER_REPORTS <= 0
        #error "AWS_IOT_DEFENDER_REPORTS cannot be 0 or negative."
    #endif

    #if AWS_IOT_DEFENDER_TOPICS <= 0
        #error "AWS_IOT_DEFENDER_TOPICS cannot be 0 or negative."
    #endif

    #define _DEFENDER_REPORT_SIZE    200

/* Prefix(30) + esimated "thing name"(128) + sufix(30). */
    #define _DEFENDER_TOPIC_SIZE     200

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
    static bool _inUseDefenderReports[ AWS_IOT_DEFENDER_REPORTS ] = { 0 };
    static uint8_t _defenderReports[ AWS_IOT_DEFENDER_REPORTS ][ _DEFENDER_REPORT_SIZE ] = { { 0 } };

    static bool _inUseDefenderTopics[ AWS_IOT_DEFENDER_TOPICS ] = { 0 };
    static char _defenderTopics[ AWS_IOT_DEFENDER_TOPICS ][ _DEFENDER_TOPIC_SIZE ] = { { 0 } };

/*-----------------------------------------------------------*/

    void * AwsIot_MallocDefenderReport( size_t size )
    {
        int freeIndex = -1;
        void * pNewReport = NULL;

        if( size <= _DEFENDER_REPORT_SIZE )
        {
            freeIndex = IotStaticMemory_FindFree( _inUseDefenderReports,
                                                  AWS_IOT_DEFENDER_REPORTS );

            if( freeIndex != -1 )
            {
                pNewReport = &( _defenderReports[ freeIndex ][ 0 ] );
            }
        }

        return pNewReport;
    }

/*-----------------------------------------------------------*/

    void AwsIot_FreeDefenderReport( void * ptr )
    {
        IotStaticMemory_ReturnInUse( ptr,
                                     _defenderReports,
                                     _inUseDefenderReports,
                                     AWS_IOT_DEFENDER_REPORTS,
                                     _DEFENDER_REPORT_SIZE );
    }

/*-----------------------------------------------------------*/

    void * AwsIot_MallocDefenderTopic( size_t size )
    {
        int freeIndex = -1;
        void * pNewTopic = NULL;

        if( size <= _DEFENDER_TOPIC_SIZE )
        {
            freeIndex = IotStaticMemory_FindFree( _inUseDefenderTopics,
                                                  AWS_IOT_DEFENDER_TOPICS );

            if( freeIndex != -1 )
            {
                pNewTopic = &( _defenderTopics[ freeIndex ][ 0 ] );
            }
        }

        return pNewTopic;
    }

/*-----------------------------------------------------------*/

    void AwsIot_FreeDefenderTopic( void * ptr )
    {
        IotStaticMemory_ReturnInUse( ptr,
                                     _defenderTopics,
                                     _inUseDefenderTopics,
                                     AWS_IOT_DEFENDER_TOPICS,
                                     _DEFENDER_TOPIC_SIZE );
    }

#endif /* if IOT_STATIC_MEMORY_ONLY == 1 */
