/*
 * FreeRTOS V202203.00
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

#include "task_control.h"
#include <stdlib.h>

struct task
{
    pthread_t tid;
};

struct task * task_create( task_callback tc,
                           void * arg )
{
    int ret;
    struct task * t = malloc( sizeof( struct task ) );

    if( t != NULL )
    {
        ret = pthread_create( &t->tid, NULL, ( void *( * )( void * ) )tc, arg );

        if( ret != 0 )
        {
            free( t );
            t = NULL;
        }
    }

    return t;
}

void task_join( struct task * t )
{
    pthread_join( t->tid, NULL );
    free( t );
}

void task_kill( struct task * t )
{
    pthread_exit( NULL );
}
