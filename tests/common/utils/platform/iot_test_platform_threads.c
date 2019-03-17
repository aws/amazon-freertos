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
 * @file iot_test_platform_threads.c
 * @brief Tests for the functions in iot_threads.h
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Test framework includes. */
#include <string.h>
#include "unity_fixture.h"

#include "platform/iot_threads.h"

/*-----------------------------------------------------------*/
/**
 * @brief Test group for Platform Threads tests.
 */
TEST_GROUP( UTIL_Platform_Threads );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for Platform Threads tests.
 */
TEST_SETUP( UTIL_Platform_Threads )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for Platform Threads tests.
 */
TEST_TEAR_DOWN( UTIL_Platform_Threads )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for Platform Threads tests.
 */
TEST_GROUP_RUNNER( UTIL_Platform_Threads )
{
    RUN_TEST_CASE( UTIL_Platform_Threads, IotThreads_CreateDetachedThread );
    RUN_TEST_CASE( UTIL_Platform_Threads, IotThreads_MutexTest );
    RUN_TEST_CASE( UTIL_Platform_Threads, IotThreads_SemaphoreTest );
}

/*-----------------------------------------------------------*/

/**
 * @brief helper function for testing thread creation
 */
void threadTestFunction(void * param)
{
    * ( uint32_t * ) param = 4321;
}

TEST( UTIL_Platform_Threads, IotThreads_CreateDetachedThread )
{
    static uint32_t attrData = 1234;
    Iot_CreateDetachedThread( threadTestFunction , &attrData , 5 , 3072);
  
    // wait for the thread to set this
    for ( int i=0; i<10; i++ )
    {
        /* If we see the value has changed we stop waiting */
        if ( attrData != 1234 )
           break;

        /* Else we wait for 100ms here */
        vTaskDelay( configTICK_RATE_HZ / 10 );
    }
    
    TEST_ASSERT_EQUAL( 4321,  attrData);
}


/*-----------------------------------------------------------*/

/**
 * @brief helper function for testing mutex
 */

struct mutexTestInfo  {
    IotMutex_t  mutex;
    int            testValue;
};

void mutexTestFunction(void * param)
{
    struct mutexTestInfo *  pTi = ( struct mutexTestInfo * ) param;

    if ( IotMutex_TryLock( &pTi->mutex ) )
    {
        pTi->testValue = 2;

        /* Don't leave it locked */
        IotMutex_Unlock( &pTi->mutex );
    } else {
        pTi->testValue = 1;
    }
}


TEST( UTIL_Platform_Threads, IotThreads_MutexTest )
{
    struct mutexTestInfo  ti;
    IotMutex_Create( &ti.mutex , false );

    ti.testValue = 0;
    IotMutex_Lock( &ti.mutex );

    Iot_CreateDetachedThread( mutexTestFunction , &ti , 5 , 3072 );

    /* Wait for signal to continue */
    while ( ti.testValue == 0 );

    if ( ti.testValue != 1 )
    {
        TEST_FAIL_MESSAGE("Acquired locked mutex");
    }
    IotMutex_Unlock( &ti.mutex );

    ti.testValue = 0;
    Iot_CreateDetachedThread( mutexTestFunction , &ti , 5 , 3072 );

    while ( ti.testValue == 0 );
    if ( ti.testValue != 2 )
    {
        TEST_FAIL_MESSAGE("Unable to Acquire unlocked mutex");
    }

    /* Clean up */
    IotMutex_Destroy( &ti.mutex );
}

/*-----------------------------------------------------------*/

/**
 * @brief helper function for testing mutex
 */

struct semTestInfo  {
    IotSemaphore_t  testSemaphore;
    int                testValue;
};

void semTestFunction(void * param)
{
    struct semTestInfo *  pTi = ( struct semTestInfo * ) param;
    int count = 0;

    /* Wait for 1s here for sem to wait*/
    vTaskDelay( configTICK_RATE_HZ );

    IotSemaphore_Post(&pTi->testSemaphore);

    /* Remember the count right after we post */
    pTi->testValue = IotSemaphore_GetCount( &pTi->testSemaphore );
}

TEST( UTIL_Platform_Threads, IotThreads_SemaphoreTest )
{
    struct semTestInfo  ti;
    int count = 0;
    bool result = 0;

    IotSemaphore_Create( &ti.testSemaphore,
                            3, /* initial value */
                            3  /* max value */ );

    /* Make sure we get 3 as we asked */
    count = IotSemaphore_GetCount( &ti.testSemaphore );
    TEST_ASSERT_EQUAL( 3,  count );

    /* Take all the resources */
    IotSemaphore_Wait( &ti.testSemaphore );
    IotSemaphore_Wait( &ti.testSemaphore );
    IotSemaphore_Wait( &ti.testSemaphore );

    /* We should have 0 left now */
    count = IotSemaphore_GetCount( &ti.testSemaphore );
    TEST_ASSERT_EQUAL( 0,  count );

    /* Spawn a thread to get us out of deadlock */
    ti.testValue = 0;
    Iot_CreateDetachedThread( semTestFunction , &ti , 5 , 3072 );

    /* Try to wait on semaphore when we have no resources left, must fail */
    result = IotSemaphore_TryWait( &ti.testSemaphore );
    TEST_ASSERT_FALSE( result );

    /* Count should still be 0 */
    count = IotSemaphore_GetCount( &ti.testSemaphore );
    TEST_ASSERT_EQUAL( 0,  count );

    /* Wait for the unavailable resource to be freed by the thread */
    IotSemaphore_Wait( &ti.testSemaphore );
    
    /* At this point we should have gotten the last one, leaving 0 */
    count = IotSemaphore_GetCount( &ti.testSemaphore );
    TEST_ASSERT_EQUAL( 0,  count );

    /* Post more than the max times, the last post should fail */
    IotSemaphore_Post( &ti.testSemaphore );
    IotSemaphore_Post( &ti.testSemaphore );
    IotSemaphore_Post( &ti.testSemaphore );
    IotSemaphore_Post( &ti.testSemaphore );

    /* Make sure we did not end up exceeding the max count */
    count = IotSemaphore_GetCount( &ti.testSemaphore );
    TEST_ASSERT_EQUAL( 3,  count );

    IotSemaphore_Destroy( &ti.testSemaphore );
}

/*-----------------------------------------------------------*/
