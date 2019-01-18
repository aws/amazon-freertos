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

/* Library dependencies. */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


/*-----------------------------------------------------------*/

#define SHARED_BUFFER_SIZE      ( 10 )
#define NUM_OF_PRODUCER         ( 8 )
#define NUM_OF_CONSUMER         ( 8 )

#define NUM_OF_EXE_ITERATION_PRODUCER ( 12500 )                                                             /* How many iterations EACH producer thread executes. */
#define NUM_OF_EXE_ITERATION_CONSUMER ( NUM_OF_EXE_ITERATION_PRODUCER * NUM_OF_PRODUCER / NUM_OF_CONSUMER )  /* How many iterations EACH consumer thread executes. Fraction is not taken care of. */

/*-----------------------------------------------------------*/

static sem_t xSemaphore_empty;
static sem_t xSemaphore_available;

static int pBuffer[ SHARED_BUFFER_SIZE ];

static int iWriteIndex;
//static pthread_mutex_t xWriteMutex;           /* Commenting out purely for profiling interest. */

static int iReadIndex;
//static pthread_mutex_t xReadMutex;            /* Commenting out purely for profiling interest. */

// This is a dummy variable to prevent compiler optimization, Do something with the value.
static int iDummyCummulator;

/*-----------------------------------------------------------*/

static void * prvProducerThread( void * pvArgs )
{
    int iThreadId = *( int * ) pvArgs;
    int i = 0;  /* Local counter. */

    /* Grab mutex, load the buffer, and release the mutex. */
    while ( i < NUM_OF_EXE_ITERATION_PRODUCER )
    {
        sem_wait( &xSemaphore_empty );

        i++;

        /* For producer-consumer to work correctly, you need mutex around below two lines.
         * Here, not using mutex, for the interest of profiling. */
        //pthread_mutex_lock( &xWriteMutex );
        pBuffer[ iWriteIndex % SHARED_BUFFER_SIZE ] = iWriteIndex;
        iWriteIndex++;
        //pthread_mutex_unlock( &xWriteMutex );

        /* Below is for debugging purpose, should not enable IO during profiling. */
        //configPRINTF( ( "%s -- Producer[%d] posted. Iteration [%d].\r\n", __FUNCTION__, iThreadId, i ) );

        sem_post( &xSemaphore_available );
    }

    configPRINTF( ( "%s -- Thread ID [%d], number of entry [%d], stack bytes left [%u]. \r\n",
            __FUNCTION__, iThreadId, i, uxTaskGetStackHighWaterMark( NULL ) ) );

    return ( void * ) NULL;
}

/*-----------------------------------------------------------*/

static void * prvConsumerThread( void * pvArgs )
{
    int iThreadId = *( int * ) pvArgs;
    int iTemp = 0;
    int i = 0; /* Local counter. */

    while ( i <  NUM_OF_EXE_ITERATION_CONSUMER )
    {
        sem_wait( &xSemaphore_available );

        i++;

        /* For producer-consumer to work correctly, you need mutex around below two lines.
         * Here, not using mutex, for the interest of profiling. */
        //pthread_mutex_lock( &xReadMutex );
        iTemp = pBuffer[ iReadIndex % SHARED_BUFFER_SIZE ];
        iReadIndex++;
        //pthread_mutex_unlock( &xReadMutex );

        /* Below is for debugging purpose, should not enable IO during profiling. */
        //configPRINTF( ( "%s -- Consumer[%d] received [%d]. Iteration [%d]\r\n", __FUNCTION__, iThreadId, iTemp, i ) );

        /* For the interest of profiling, do something with the value read from the buffer. */
        iDummyCummulator += iTemp;

        sem_post( &xSemaphore_empty );
    }

    configPRINTF( ( "%s -- Thread ID [%d], number of entry [%d], stack bytes left [%u]. \r\n",
                __FUNCTION__, iThreadId, i, uxTaskGetStackHighWaterMark( NULL ) ) );

    return ( void * ) NULL;
}

/*-----------------------------------------------------------*/

void vKernelProfilingMultiProducerConsumerSemaphore( int iPriority )
{

    pthread_t pxProducer[NUM_OF_PRODUCER];
    int pProducerArgs[NUM_OF_PRODUCER]; /* for pthread_create(). */

    pthread_t pxConsumer[NUM_OF_CONSUMER];
    int pConsumerArgs[NUM_OF_CONSUMER];     /* for pthread_create(). */

    pthread_attr_t xThreadAttr;
    struct sched_param xSchedParam = { 0 };
    int iStatus = 0;
    int i = 0;

    /* Tracing within tracing -- branch stat. */
    #if ( POSIX_SEMAPHORE_IMPLEMENTATION == 1 && POSIX_SEMAPHORE_IMPLEMENTATION_BRANCH_STAT == 1 )
        FreeRTOS_POSIX_semaphore_initBranchTaken();
    #endif /* POSIX_SEMAPHORE_IMPLEMENTATION && POSIX_SEMAPHORE_IMPLEMENTATION_BRANCH_STAT */

    /* Reset global variables. */
    iReadIndex = 0;
    iWriteIndex = 0;

    /* Commenting out mutex for profiling interest. */
    //xReadMutex = PTHREAD_MUTEX_INITIALIZER;
    //xWriteMutex = PTHREAD_MUTEX_INITIALIZER;

    //pthread_mutex_init( &xReadMutex, NULL );
    //pthread_mutex_init( &xWriteMutex, NULL );

    /* Initialize semaphore_empty with 10, since producer has not started. */
    sem_init( &xSemaphore_empty, 0, SHARED_BUFFER_SIZE );

    /* Initialize semaphore_taken with 0, since nothing is produced.. */
    sem_init( &xSemaphore_available, 0, 0 );

    /* Initialize thread attribute, to explicitly set task priority. */
    xSchedParam.sched_priority = iPriority;
    pthread_attr_init( &xThreadAttr );
    pthread_attr_setschedparam( &xThreadAttr, &xSchedParam );
    pthread_attr_setdetachstate( &xThreadAttr, PTHREAD_CREATE_JOINABLE );

    /* Create consumer. */
    for ( i = 0; i < NUM_OF_CONSUMER; i++ )
    {
        pConsumerArgs[i] = i;
        iStatus = pthread_create( &pxConsumer[i], &xThreadAttr, prvConsumerThread, ( void * ) &pConsumerArgs[i] );
        if ( iStatus != 0 )
        {
            configPRINTF( ( "%s -- Failed to create consumer thread [%d].\r\n", __FUNCTION__, i ) );
        }
    }

    /* Create producer. */
    for ( i = 0; i < NUM_OF_PRODUCER; i++ )
    {
        pProducerArgs[i] = i;
        iStatus = pthread_create( &pxProducer[i], &xThreadAttr, prvProducerThread, ( void * ) &pProducerArgs[i] );
        if ( iStatus != 0 )
        {
            configPRINTF( ( "%s -- Failed to create producer thread [%d].\r\n", __FUNCTION__, i) );
        }
    }

    configPRINTF( ( "%s -- Producer-consumer threads are created.\r\n", __FUNCTION__ ) );

    /* Join producer. */
    for ( i = 0; i < NUM_OF_PRODUCER; i++ )
    {
        iStatus = pthread_join( pxProducer[i], NULL );
        if ( iStatus != 0 )
        {
            configPRINTF( ( "%s -- Failed to join producer thread [%d].\r\n", __FUNCTION__, i ) );
        }
    }

    /* Join consumer. */
    for ( i = 0; i < NUM_OF_CONSUMER; i++)
    {
        pthread_join( pxConsumer[i],  NULL );
        if ( iStatus != 0 )
        {
            configPRINTF( ( "%s -- Failed to join consumer thread [%d].\r\n", __FUNCTION__, i ) );
        }
    }

    /* De-initialize thread attribute. */
    pthread_attr_destroy( &xThreadAttr );

    /* Tracing within tracing -- branch stat. */
    #if ( POSIX_SEMAPHORE_IMPLEMENTATION == 1 && POSIX_SEMAPHORE_IMPLEMENTATION_BRANCH_STAT == 1 )
        int iBranchTaken_post = FreeRTOS_POSIX_semaphore_getBranchTaken_post();
        int iBranchTaken_wait = FreeRTOS_POSIX_semaphore_getBranchTaken_wait();

        configPRINTF( ( "%s -- total critical section branch taken -- sem_post [%d], sem_wait [%d]\r\n",
                __FUNCTION__,
                iBranchTaken_post,
                iBranchTaken_wait ) );

        FreeRTOS_POSIX_semaphore_deInitBranchTaken();
    #endif /* POSIX_SEMAPHORE_IMPLEMENTATION && POSIX_SEMAPHORE_IMPLEMENTATION_BRANCH_STAT */

    configPRINTF( ( "%s -- Main thread stack bytes left [%u]. \r\n", __FUNCTION__, uxTaskGetStackHighWaterMark( NULL ) ) );
}
