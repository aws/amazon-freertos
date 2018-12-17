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

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* This file should only be compiled if dynamic memory allocation is forbidden. */
#if AWS_IOT_STATIC_MEMORY_ONLY == 1

/* Static memory include. */
    #include "platform/aws_iot_static_memory.h"

/* Defender internal include. */
    #include "private/aws_iot_defender_internal.h"

/*-----------------------------------------------------------*/

    void * AwsIot_MallocDefenderReport( size_t size )
    {
        AwsIotDefender_Assert( 0 );
    }

/*-----------------------------------------------------------*/

    void AwsIot_FreeDefenderReport( void * ptr )
    {
        AwsIotDefender_Assert( 0 );
    }

/*-----------------------------------------------------------*/

    void * AwsIot_MallocDefenderTopic( size_t size )
    {
        AwsIotDefender_Assert( 0 );
    }

/*-----------------------------------------------------------*/

    void AwsIot_FreeDefenderTopic( void * ptr )
    {
        AwsIotDefender_Assert( 0 );
    }

#endif /* if AWS_IOT_STATIC_MEMORY_ONLY == 1 */
