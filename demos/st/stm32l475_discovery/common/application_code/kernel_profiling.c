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
 * @brief Resource hierarchy solution to dining philosophers problem
 *
 * Dining philosophers problem illustrates the challenges of avoiding deadlock.
 * Resource hierarchy solution attempts to solve this problem by assigning a
 * partial order to resources (mutexes), and establishes the convention that
 * all resources will be requested in order. In this particular implementation,
 * it's also assumed that a list of resources required by a philosopher (thread)
 * is known prior.
 *
 * Refer to https://en.wikipedia.org/wiki/Dining_philosophers_problem
 * for detailed description.
 */

#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/sys/types.h"
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

#define NUM_OF_PHILOSOPHERS             ( 5UL )
#define NUM_OF_SPOONS                   ( NUM_OF_PHILOSOPHERS )

#define NUM_OF_SPOONS_PER_PHILOSOPHER   ( 2UL )

#define NUM_OF_EXE_ITERATION            ( 100UL )

/*-----------------------------------------------------------*/

typedef struct sharedSpoons
{
    pthread_mutex_t     xMutex;
    uint32_t            ulMutexOrder;
} sharedSpoons_t;

typedef struct philosopherInputs
{
    pthread_t           xPthreadHandle;
    pthread_barrier_t   xBarrier;
    uint32_t            ulIndex;
    uint32_t            ulNumOfSpoonsNeeded;
    sharedSpoons_t *    pListOfSpoons;
} philosopherInputs_t;

/*-----------------------------------------------------------*/

static void prvKernelProfilingSetup( void )
{
    aws_hal_perfcounter_open();
}

/*-----------------------------------------------------------*/

static void prvKernelProfilingCleanup( void )
{
    aws_hal_perfcounter_close();
}

/*-----------------------------------------------------------*/

/**
 * @brief Check demo configurations all in one place before initialization.
 *
 * @param[in] ulNumOfResources The number of resources (spoons) to be validated.
 */
static bool prvDemoConfigAssert( void )
{
    /* Needs at least 2 locks and 2 threads competing to create deadlock. */
    return ( NUM_OF_PHILOSOPHERS >= 2 &&
             NUM_OF_SPOONS >=2 &&
             NUM_OF_SPOONS >= NUM_OF_SPOONS_PER_PHILOSOPHER &&
             ( configUSE_16_BIT_TICKS == 1 ) ? ( NUM_OF_PHILOSOPHERS <= 8 ) : ( NUM_OF_PHILOSOPHERS <= 24 ) // pthread_barrier_*
            );
}

/*-----------------------------------------------------------*/

/**
 * @brief De-initialize resources allocated in prvInitResources().
 */
static bool prvDeInitSpoons( uint32_t ulNumOfSpoons,  sharedSpoons_t * pMutexList )
{
    if ( pMutexList == NULL )
    {
        return false;
    }

    for ( int i = 0; i < ulNumOfSpoons; i++ )
    {
        pthread_mutex_destroy( &pMutexList[i].xMutex );
    }

    return true;
}

/*-----------------------------------------------------------*/

/**
 * @brief Initialize resources.
 *
 * This function takes care of memory allocation on heap for resources,
 * and order assigning for resources.
 *
 * @param[in] ulNumOfResources The number of resources (spoons) to be initialized.
 * @param[out] pMutexList Pointer to
 *
 * @returns true for success; false for otherwise.
 */
static bool prvInitSpoons( uint32_t ulNumOfSpoons,  sharedSpoons_t * pMutexList )
{
    int iStatus = 0;
    pthread_mutexattr_t xMutexAttr = { 0 };

    if ( pMutexList == NULL )
    {
        return false;
    }

    ( void ) pthread_mutexattr_init( &xMutexAttr );
    ( void ) pthread_mutexattr_settype( &xMutexAttr, PTHREAD_MUTEX_NORMAL );

    for ( int i = 0; i < ulNumOfSpoons; i++ )
    {
        /* Memory is allocated inside of this init call. */
        iStatus = pthread_mutex_init(  &pMutexList[i].xMutex, &xMutexAttr );

        if ( iStatus == 0 )
        {
            pMutexList[i].ulMutexOrder = i;
        }
        else
        {
            /* Not enough memory. Clear all previously allocated and abort. */
            prvDeInitSpoons( ( i - 1 ), pMutexList );

            /* This destroy call does nothing. Just for completeness. */
            ( void ) pthread_mutexattr_destroy( &xMutexAttr );

            return false;
        }
    }

    /* This destroy call does nothing. Just for completeness. */
    pthread_mutexattr_destroy( &xMutexAttr );

    return true;
}

/*-----------------------------------------------------------*/

/**
 * @brief Bubble sort, in rising order.
 */
static void prvBubbleSort( uint32_t * pArray, uint32_t ulSizeOfArray )
{
    int i = 0, j = 0;
    uint32_t ulSwapBuffer = 0;

    for ( i = 0; i < ulSizeOfArray - 1; i++ )
    {
        /* Last i elements are already in order. */
        for ( j = 0; j < ulSizeOfArray - i - 1; j++ )
        {
            if ( pArray[j] > pArray[j + 1] )
            {
                ulSwapBuffer = pArray[j + 1];
                pArray[j + 1] = pArray[j];
                pArray[j] = ulSwapBuffer;
            }
        }
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief The philosopher thread.
 *
 * A philosopher acquires resources he needs in the order of
 */
static void * prvPhilosopherGetResources( void * pvArgs )
{
    philosopherInputs_t xPhilosopher = * ( philosopherInputs_t * )pvArgs;
    uint32_t pSpoons[ NUM_OF_SPOONS_PER_PHILOSOPHER ] = { 0 };
    int i = 0, j = 0;

    configPRINTF( ( "%s -- Philosopher[%d] entered, spoons needed [%u]. arg address 0x%x.\r\n", __FUNCTION__, xPhilosopher.ulIndex, pvArgs ) );

    /* Collect spoons needed by this philosopher.
     * Assume a philosopher takes spoons consecutively. */
    pSpoons[0] = xPhilosopher.ulIndex;

    for ( i = 1; i < NUM_OF_SPOONS_PER_PHILOSOPHER; i++ )
    {
        pSpoons[i] = ( pSpoons[0] + i ) % NUM_OF_SPOONS_PER_PHILOSOPHER;
    }

    /* Sort spoons, so that we can just take it in order later. */
    prvBubbleSort( pSpoons, NUM_OF_SPOONS_PER_PHILOSOPHER );

    /* Wait until every philosopher is ready. */
    pthread_barrier_wait( &xPhilosopher.xBarrier );

    configPRINTF( ( "%s -- Philosopher[%d] initialized. Waiting to start.\r\n", __FUNCTION__, xPhilosopher.ulIndex ) );

    /* Grab spoons, do something, release spoons. */
    for ( i = 0; i < NUM_OF_EXE_ITERATION; i++ )
    {
        /* Acquire resources in ascending order. */
        for (  j = 0; j < NUM_OF_EXE_ITERATION; j++ )
        {
            pthread_mutex_lock( &( xPhilosopher.pListOfSpoons[pSpoons[j]].xMutex ) );
        }

        /* You could optionally do something here. */

        /* Release resources descending order. */
        for ( j = 1; j <= NUM_OF_EXE_ITERATION; j++ )
        {
            pthread_mutex_unlock( &( xPhilosopher.pListOfSpoons[pSpoons[ NUM_OF_EXE_ITERATION - j ]].xMutex ) );
        }
    }

    configPRINTF( ( "%s -- Philosopher[%d] finished. Exit.\r\n", __FUNCTION__, xPhilosopher.ulIndex ) );

    return NULL;
}

/*-----------------------------------------------------------*/

void vKernelProfilingDiningPhilosopher( void )
{
    uint32_t i = 0;
    int iStatus = 0;
    bool bStatus;
    sharedSpoons_t pxSharedSpoons[ NUM_OF_SPOONS ];
    philosopherInputs_t pxPhilosophers[ NUM_OF_PHILOSOPHERS ];
    pthread_barrier_t xBarrier;

    /* Make sure things input are correct to start with. */
    if ( prvDemoConfigAssert() == false )
    {
        configPRINTF( ( "%s -- Invalid profiling configuration.\r\n", __FUNCTION__ ) );
        return;
    }

    /* Initialize tools needed in profiling. */
    prvKernelProfilingSetup();

    /* Init barrier for NUM_OF_PHILOSOPHERS threads. */
    iStatus = pthread_barrier_init( &xBarrier, NULL, NUM_OF_PHILOSOPHERS );
    if ( iStatus != 0 )
    {
        /* Not going to handle error case here. Just having something on console to warn user. */
        configPRINTF( ( "%s -- Barrier creation failed. error code [%d]\r\n", __FUNCTION__, iStatus ) );
    }

    /* Initialize spoons. */
    bStatus = prvInitSpoons( NUM_OF_SPOONS, pxSharedSpoons );
    if ( bStatus == false )
    {
        /* Not going to handle error case here. Just having something on console to warn user. */
        configPRINTF( ( "%s -- Spoon initialization failed.\r\n", __FUNCTION__ ) );
    }

    /* Create philosopher threads. */
    for( i = 0; i < NUM_OF_PHILOSOPHERS; i++ )
    {
        pxPhilosophers[i].ulIndex = i;
        pxPhilosophers[i].ulNumOfSpoonsNeeded = NUM_OF_SPOONS_PER_PHILOSOPHER;
        pxPhilosophers[i].xBarrier = xBarrier;
        pxPhilosophers[i].pListOfSpoons = pxSharedSpoons;

        configPRINTF( ( "%s -- creating thread: philosopher [%d], arg index [0x%x] \r\n", __FUNCTION__, pxPhilosophers[i].ulIndex, &pxPhilosophers[i] ) );
        iStatus = pthread_create( &( pxPhilosophers[i].xPthreadHandle ), NULL, prvPhilosopherGetResources, &pxPhilosophers[i] );
        if ( iStatus  != 0 )
        {
            /* Not going to handle error case here. Just having something on console to warn user. */
            configPRINTF( ( "%s -- Thread creation failed. thread [%d], error code [%d]\r\n", __FUNCTION__, i, iStatus ) );
        }
    }

    /* Join philosopher threads. */
    for( i = 0; i < NUM_OF_PHILOSOPHERS; i++ )
    {
        iStatus = pthread_join( &pxPhilosophers[i], NULL );
        if ( iStatus != 0 )
        {
            /* Not going to handle error case here. Just having something on console to warn user. */
           configPRINTF( ( "%s -- Thread join failed. thread [%d], error code [%d]\r\n", __FUNCTION__, i, iStatus ) );
        }
    }

    /* De-initialize spoons. */
    bStatus = prvDeInitSpoons( NUM_OF_SPOONS, pxSharedSpoons );
    if ( bStatus == false )
    {
        /* Not going to handle error case here. Just having something on console to warn user. */
        configPRINTF( ( "%s -- Spoon de-initialization failed.\r\n", __FUNCTION__ ) );
    }

    /* De-init barrier for NUM_OF_PHILOSOPHERS threads. */
    iStatus = pthread_barrier_destroy( &xBarrier );
    if ( iStatus != 0 )
    {
        /* Not going to handle error case here. Just having something on console to warn user. */
        configPRINTF( ( "%s -- Barrier destruction failed. error code [%d]\r\n", __FUNCTION__, iStatus ) );
    }

    /* De-initializing tools needed for profiling. */
    prvKernelProfilingCleanup();

}
