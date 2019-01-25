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
#define NUM_OF_PRODUCER         ( 10 )
#define NUM_OF_CONSUMER         ( 10 )

#define NUM_OF_EXE_ITERATION_PRODUCER ( 1000 )                                                               /* How many iterations EACH producer thread executes. */
#define NUM_OF_EXE_ITERATION_CONSUMER ( NUM_OF_EXE_ITERATION_PRODUCER * NUM_OF_PRODUCER / NUM_OF_CONSUMER )  /* How many iterations EACH consumer thread executes. Fraction is not taken care of. */

/*-----------------------------------------------------------*/

/* Tracing related definition. */
#define TRACING_THREAD_PRODUCER     ( 1 )
#define TRACING_THREAD_CONSUMER     ( 1 )

#define TRACING_DISABLE_MUTEX_AROUND_INDEXES    ( 1 )       /* This is trying to create bug on purpose! Set to 0 to run classic producer-consumer. */
/*-----------------------------------------------------------*/

static sem_t xSemaphore_empty;
static sem_t xSemaphore_available;

static int pBuffer[ SHARED_BUFFER_SIZE ];

static int iWriteIndex;
//static pthread_mutex_t xWriteMutex;           /* Commenting out purely for profiling interest. */

static int iReadIndex;
//static pthread_mutex_t xReadMutex;            /* Commenting out purely for profiling interest. */

#if ( TRACING_DISABLE_MUTEX_AROUND_INDEXES == 1 )
    static int iDummyCummulator;
#endif /* TRACING_DISABLE_MUTEX_AROUND_INDEXES */

/*-----------------------------------------------------------*/

static void * prvProducerThread( void * pvArgs )
{
    int iThreadId = *( int * ) pvArgs;
    int i = 0;  /* Write index. */

    #if ( TRACING_THREAD_PRODUCER == 1 )
        /* Thread specific counters. No need for synchronization. */
        uint64_t ullTotalCycleElapsed = 0;
        uint64_t ullSingleCycleElapsed = 0;
        int iNumOfEntry = 0;
    #endif /* TRACING_THREAD_PRODUCER */

    /* Grab mutex, load the buffer, and release the mutex. */
    while ( i < NUM_OF_EXE_ITERATION_PRODUCER )
    {
        sem_wait( &xSemaphore_empty );

        i++;

        /* For producer-consumer to work correctly, you need mutex around below two lines.
         * Here, not using mutex, for the interest of profiling. */
        pBuffer[ iWriteIndex % SHARED_BUFFER_SIZE ] = iWriteIndex;
        iWriteIndex++;

        /* Below is for debugging purpose, should not enable IO during profiling. */
        //configPRINTF( ( "%s -- Producer[%d] posted. Iteration [%d].\r\n", __FUNCTION__, iThreadId, i ) );

        #if ( TRACING_THREAD_PRODUCER == 1 )
            ullSingleCycleElapsed = aws_hal_perfcounter_get_value(); /* Temporary value. */
        #endif /* TRACING_THREAD_PRODUCER */

        sem_post( &xSemaphore_available );

        #if ( TRACING_THREAD_PRODUCER == 1 )
            ullSingleCycleElapsed = aws_hal_perfcounter_get_value() - ullSingleCycleElapsed;
            ullTotalCycleElapsed += ullSingleCycleElapsed;
            iNumOfEntry++;
        #endif /* TRACING_THREAD_PRODUCER */
    }

    configPRINTF( ( "%s -- Producer[%i] -- sem_post() total cycle elapsed [%ld], number of entry [%d].\r\n",
                    __FUNCTION__,
                    iThreadId,
                    ( long )ullTotalCycleElapsed,
                    iNumOfEntry ) );
    configPRINTF( ( "%s -- Stack bytes left [%u]. \r\n", __FUNCTION__, uxTaskGetStackHighWaterMark( NULL ) ) );

    return ( void * ) NULL;
}

/*-----------------------------------------------------------*/

static void * prvConsumerThread( void * pvArgs )
{
    int iThreadId = *( int * ) pvArgs;
    int iTemp = 0;
    int i = 0; /* Read index. */

    #if ( TRACING_THREAD_CONSUMER == 1 )
        /* Thread specific counters. No need for synchronization. */
        uint64_t ullTotalCycleElapsed = 0;
        uint64_t ullSingleCycleElapsed = 0;
        int iNumOfEntry = 0;
    #endif /* TRACING_THREAD_CONSUMER */

    while ( i <  NUM_OF_EXE_ITERATION_CONSUMER )
    {
        #if ( TRACING_THREAD_CONSUMER == 1 )
            ullSingleCycleElapsed = aws_hal_perfcounter_get_value(); /* Temporary value. */
        #endif /* TRACING_THREAD_PRODUCER */

        sem_wait( &xSemaphore_available );

        #if ( TRACING_THREAD_CONSUMER == 1 )
            ullSingleCycleElapsed = aws_hal_perfcounter_get_value() - ullSingleCycleElapsed;
            ullTotalCycleElapsed += ullSingleCycleElapsed;
            iNumOfEntry++;
        #endif /* TRACING_THREAD_CONSUMER */

        i++;

        /* For producer-consumer to work correctly, you need mutex around below two lines.
         * Here, not using mutex, for the interest of profiling. */
        iTemp = pBuffer[ iReadIndex % SHARED_BUFFER_SIZE ];
        iReadIndex++;

        /* Below is for debugging purpose, should not enable IO during profiling. */
        //configPRINTF( ( "%s -- Consumer[%d] received [%d]. Iteration [%d]\r\n", __FUNCTION__, iThreadId, iTemp, i ) );

        /* For the interest of profiling, do something with the value read from the buffer. */
        iDummyCummulator += iTemp;

        sem_post( &xSemaphore_empty );
    }

    configPRINTF( ( "%s -- Consumer[%i] -- sem_wait() total cycle elapsed [%ld], number of entry [%d].\r\n",
                        __FUNCTION__,
                        iThreadId,
                        ( long )ullTotalCycleElapsed,
                        iNumOfEntry ) );
    configPRINTF( ( "%s -- Stack bytes left [%u]. \r\n", __FUNCTION__, uxTaskGetStackHighWaterMark( NULL ) ) );

    return ( void * ) NULL;
}

/*-----------------------------------------------------------*/

void vKernelProfilingMultiProducerConsumerMutex( int iPriority )
{

    pthread_t pxProducer[NUM_OF_PRODUCER];
    int pProducerArgs[NUM_OF_PRODUCER]; /* for pthread_create(). */

    pthread_t pxConsumer[NUM_OF_CONSUMER];
    int pConsumerArgs[NUM_OF_CONSUMER];     /* for pthread_create(). */

    pthread_attr_t xThreadAttr;
    struct sched_param xSchedParam = { 0 };
    int iStatus = 0;
    int i = 0;

    uint64_t ullCycleElapsed = 0;

    /* Tracing within tracing -- branch stat. */
    #if ( POSIX_SEMAPHORE_IMPLEMENTATION == 1 && POSIX_SEMAPHORE_IMPLEMENTATION_BRANCH_STAT == 1 )
        FreeRTOS_POSIX_semaphore_initBranchTaken();
    #endif /* POSIX_SEMAPHORE_IMPLEMENTATION && POSIX_SEMAPHORE_IMPLEMENTATION_BRANCH_STAT */

    /* Initializing performance timer. */
    aws_hal_perfcounter_open();

    /* Time stamp -- entering application. */
    ullCycleElapsed = aws_hal_perfcounter_get_value();

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

    /* Time stamp -- entering application. */
    ullCycleElapsed = aws_hal_perfcounter_get_value() - ullCycleElapsed;

    /* De-initializing performance timer. */
    aws_hal_perfcounter_close();

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

    configPRINTF( ( "%s -- Stack bytes left [%u]. \r\n", __FUNCTION__, uxTaskGetStackHighWaterMark( NULL ) ) );
    configPRINTF( ( "%s -- Threads finished. Application -- total cycle elapsed [%ld]\r\n", __FUNCTION__, ( long ) ullCycleElapsed ) );

}
