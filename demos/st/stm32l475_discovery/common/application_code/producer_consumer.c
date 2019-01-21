/*
* Amazon FreeRTOS V1.x.x
* Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* http://aws.amazon.com/freertos
* http://www.FreeRTOS.org
*/

/*
 * @brief Classic producer-consumer implementation.
 *
 */

#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/semaphore.h"
#include "FreeRTOS_POSIX/sched.h"
#include "FreeRTOS_POSIX/tracing.h"
#include "task.h"
#include "FreeRTOSConfig.h"

/* Header file for profiling. */
#include "kernel_profiling.h"

/* Tools used in profiling. */
#include "aws_hal_perfcounter.h"

/* Library dependencies. */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


/*-----------------------------------------------------------*/

#define SHARED_BUFFER_SIZE      ( 10 )
#define NUM_OF_EXE_ITERATION    ( 1000 )

/*-----------------------------------------------------------*/

static sem_t xSemaphore;
static int pBuffer[ SHARED_BUFFER_SIZE ];

/*-----------------------------------------------------------*/

static void * prvProducerThread( void * argv)
{
    sem_t xSemaphore = * ( sem_t * )argv;
    int iSemValue;
    int i = 0;  /* Write index. */

    /* Grab mutex, load the buffer, and release the mutex. */
    while ( i < NUM_OF_EXE_ITERATION )
    {
        sem_getvalue( &xSemaphore, &iSemValue );

        if ( iSemValue >= SHARED_BUFFER_SIZE )
        {
            /* Buffer is already full, do not overflow. */
            continue;
        }

        pBuffer[ i % SHARED_BUFFER_SIZE ] = i;
        i++;
        configPRINTF( ( "%s -- Producer posted [%d]\r\n", __FUNCTION__, i ) );
        sem_post( &xSemaphore );
    }

    configPRINTF( ( "%s -- Producer thread finished.\r\n", __FUNCTION__ ) );
    configPRINTF( ( "%s -- Stack bytes left [%u]. \r\n", __FUNCTION__, uxTaskGetStackHighWaterMark( NULL ) ) );

    return ( void * ) NULL;
}

/*-----------------------------------------------------------*/

static void * prvConsumerThread( void * argv)
{
    sem_t xSemaphore = * ( sem_t * )argv;
    int iTemp = 0;
    int i = 0; /* Read index. */

    while ( i <  NUM_OF_EXE_ITERATION)
    {
        sem_wait( &xSemaphore );
        iTemp = pBuffer[ i % SHARED_BUFFER_SIZE ];
        i++;
        configPRINTF( ( "%s -- Consumer got [%d]\r\n", __FUNCTION__, iTemp ) );
    }

    configPRINTF( ( "%s -- Consumer thread finished.\r\n", __FUNCTION__ ) );
    configPRINTF( ( "%s -- Stack bytes left [%u]. \r\n", __FUNCTION__, uxTaskGetStackHighWaterMark( NULL ) ) );

    return ( void * ) NULL;
}

/*-----------------------------------------------------------*/

void vKernelProfilingProducerConsumerMutex( int iPriority )
{

    pthread_t xProducer;
    pthread_t xConsumer;
    pthread_attr_t xThreadAttr;
    struct sched_param xSchedParam = { 0 };
    int iStatus = 0;

    #if ( POSIX_SEMAPHORE_TRACING == 1 )
        uint64_t ullTimeElapsed_timedwait = 0;
        int iNumOfEntry_timedwait = 0;

        uint64_t ullTimeElapsed_post = 0;
        int iNumOfEntry_post = 0;
    #endif /* POSIX_SEMAPHORE_TRACING */

    #if ( POSIX_SEMAPHORE_TRACING == 1 )
        /* Initializing performance timer. */
        aws_hal_perfcounter_open();

        /* Initializing tracing interfaces. */
        FreeRTOS_POSIX_semaphore_initPerfCounterCycleElapsed();
    #endif /* POSIX_SEMAPHORE_TRACING */

    /* Initialize semaphore with 0, since producer has not started. */
    sem_init( &xSemaphore, 0, 0 );

    /* Initialize thread attribute, to explicitly set task priority. */
    xSchedParam.sched_priority = iPriority;
    pthread_attr_init( &xThreadAttr );
    pthread_attr_setschedparam( &xThreadAttr, &xSchedParam );
    pthread_attr_setdetachstate( &xThreadAttr, PTHREAD_CREATE_JOINABLE );

    /* Create consumer. */
    iStatus = pthread_create( &xConsumer, &xThreadAttr, prvConsumerThread, &xSemaphore );
    if ( iStatus != 0 )
    {
        configPRINTF( ( "%s -- Failed to create consumer thread.\r\n", __FUNCTION__ ) );
    }

    /* Create producer. */
    iStatus = pthread_create( &xProducer, &xThreadAttr, prvProducerThread, &xSemaphore );
    if ( iStatus != 0 )
    {
        configPRINTF( ( "%s -- Failed to create producer thread.\r\n", __FUNCTION__ ) );
    }

    configPRINTF( ( "%s -- Producer-consumer threads are created.\r\n", __FUNCTION__ ) );

    /* Join producer. */
    iStatus = pthread_join( xProducer, NULL );
    if ( iStatus != 0 )
    {
        configPRINTF( ( "%s -- Failed to join producer thread.\r\n", __FUNCTION__ ) );
    }

    /* Join consumer. */
    pthread_join( xConsumer,  NULL );
    if ( iStatus != 0 )
    {
        configPRINTF( ( "%s -- Failed to join consumer thread.\r\n", __FUNCTION__ ) );
    }

    /* De-initialize thread attribute. */
    pthread_attr_destroy( &xThreadAttr );

    /* De-initialize semaphore. */
    sem_destroy( &xSemaphore );

    /* Get stat value, and print something on screen. */
    #if ( POSIX_SEMAPHORE_TRACING == 1 )
        /* Get tracing values. */
        ullTimeElapsed_timedwait = FreeRTOS_POSIX_semaphore_getPerfCounterCycleElapsed_timedwait();
        iNumOfEntry_timedwait = FreeRTOS_POSIX_semaphore_getPerfCounterNumOfEntry_timedwait();

        configPRINTF( ( "%s -- sem_timedwait: time elapsed [%ld], number of entry [%d] \r\n",
                        __FUNCTION__, (long)ullTimeElapsed_timedwait, iNumOfEntry_timedwait ) );

        ullTimeElapsed_post = FreeRTOS_POSIX_semaphore_getPerfCounterCycleElapsed_post();
        iNumOfEntry_post = FreeRTOS_POSIX_semaphore_getPerfCounterNumOfEntry_post();

        configPRINTF( ( "%s -- sem_post: time elapsed [%ld], number of entry [%d] \r\n",
                        __FUNCTION__, (long)ullTimeElapsed_post, iNumOfEntry_post ) );

        /* De-initializing tracing interfaces. */
        FreeRTOS_POSIX_semaphore_deInitPerfCounterCycleElapsed();

        /* De-initializing performance timer. */
        aws_hal_perfcounter_close();
    #endif /* POSIX_SEMAPHORE_TRACING */

    configPRINTF( ( "%s -- Stack bytes left [%u]. \r\n", __FUNCTION__, uxTaskGetStackHighWaterMark( NULL ) ) );
    configPRINTF( ( "%s -- Threads finished.\r\n", __FUNCTION__ ) );

}
