/*
 * Amazon FreeRTOS V1.1.4
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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */


/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Test framework includes. */
#include "unity_fixture.h"

#define ESP_TEST_DEFAULT_PRIORITY              ( tskIDLE_PRIORITY + 1 )
#define ESP_TEST_HIGH_PRIORITY                 ( tskIDLE_PRIORITY + 3 )
#define ESP_TASK_DEFAULT_STACK_SIZE            ( configMINIMAL_STACK_SIZE * 4 )

#define ESP_TEST_PINNED_TASK_NAME              "ESP_Pin"
#define ESP_TEST_WAITER_TASK_NAME              "ESP_Wait"
#define ESP_TEST_LOW_PRIORITY_TASK_NAME        "ESP_Low"
#define TASK_CREATION_TIMEOUT                  pdMS_TO_TICKS( 1000 )
#define TASK_SIMULTANEOUS_EXECUTION_TIMEOUT    pdMS_TO_TICKS( 500 )

/**
 * @brief Test group for ESP Task creation tests
 */
TEST_GROUP( ESP_SMP_Task_Creation );

/**
 * @brief Test group runner for ESP SMP task creation
 */
TEST_GROUP_RUNNER( ESP_SMP_Task_Creation )
{
    RUN_TEST_CASE( ESP_SMP_Task_Creation, IsMultiCore );
    RUN_TEST_CASE( ESP_SMP_Task_Creation, CreatePinnedTaskOnCore0 );

    if( portNUM_PROCESSORS > 1 )
    {
        RUN_TEST_CASE( ESP_SMP_Task_Creation, CreatePinnedTaskOnCore1 );
    }

    RUN_TEST_CASE( ESP_SMP_Task_Creation, LowPriorityTasksRunOnSecondCore );
}

TEST_SETUP( ESP_SMP_Task_Creation )
{
}
TEST_TEAR_DOWN( ESP_SMP_Task_Creation )
{
}

/**
 * @brief The response structure from report core
 */
typedef struct
{
    SemaphoreHandle_t done; /*< The reportCore task sets this semaphore on completion */
    BaseType_t xCoreID;     /*< The CPU ID on which report Core ran */
} coreReport_t;

/**
 * @brief Record the core on which this task runs, then suspend
 */
static void reportCore( void * pvReport )
{
    coreReport_t * pxReport = ( coreReport_t * ) pvReport;

    pxReport->xCoreID = xPortGetCoreID();

    if( pxReport->done )
    {
        if( !xSemaphoreGive( pxReport->done ) )
        {
            configPRINTF( ( "Error when returning semaphore.\n" ) );
        }
    }

    vTaskSuspend( NULL );
}

/**
 * @brief Test whether pinned tasks run on the core to which they were assigned
 */
static void TestCreatePinnedTask( BaseType_t xCoreID,
                                  char * taskName )
{
#define CORE_NOT_SET    INT_MAX
    TaskHandle_t xTaskHandle = NULL;
    BaseType_t taskCreated = pdFALSE, taskCompleted = pdFALSE;
    coreReport_t _coreReport =
    {
        .xCoreID = CORE_NOT_SET,
        .done    = xSemaphoreCreateBinary()
    };

    coreReport_t * coreReport = &_coreReport;

    if( TEST_PROTECT() )
    {
        TEST_ASSERT_NOT_NULL_MESSAGE( coreReport->done, "creating semaphore" );

        taskCreated = xTaskCreatePinnedToCore( reportCore,
                                               taskName,
                                               ESP_TASK_DEFAULT_STACK_SIZE,
                                               coreReport,
                                               ESP_TEST_DEFAULT_PRIORITY,
                                               &xTaskHandle,
                                               xCoreID );
        TEST_ASSERT_TRUE_MESSAGE( taskCreated, "creating task" );

        taskCompleted = xSemaphoreTake( coreReport->done, TASK_CREATION_TIMEOUT );

        TEST_ASSERT_TRUE_MESSAGE( taskCompleted, "Reporting task timed out" );
        TEST_ASSERT_EQUAL_MESSAGE( coreReport->xCoreID, xCoreID, "Core ID for pinned task" );
    }

    /* Cleanup */
    if( xTaskHandle != NULL )
    {
        vTaskDelete( xTaskHandle );
    }

    if( coreReport->done )
    {
        vSemaphoreDelete( coreReport->done );
    }
}

/**
 * @brief Test that the multicore configuration flag is enabled in this build
 */
TEST( ESP_SMP_Task_Creation, IsMultiCore )
{
    TEST_ASSERT_GREATER_THAN( 1, portNUM_PROCESSORS );
}

TEST( ESP_SMP_Task_Creation, CreatePinnedTaskOnCore0 )
{
    TestCreatePinnedTask( 0, ESP_TEST_PINNED_TASK_NAME "_0" );
}

TEST( ESP_SMP_Task_Creation, CreatePinnedTaskOnCore1 )
{
    TestCreatePinnedTask( 1, ESP_TEST_PINNED_TASK_NAME "_1" );
}

struct SimultaneousTasksTest_t
{
    /* An inner low priority task attempts to set the lowPriority semaphore*/
    SemaphoreHandle_t lowPriority;

    /* An outer high-priority task starves the low priority task and waits for the lowPriority semaphore
     * When it times out or succeeds, it sets Done to return control to the test harness */
    SemaphoreHandle_t done;

    /* The outer task indicates success by setting succeeded */
    BaseType_t succeeded;
};

static void lowPriorityTask( void * p );
static void highPriorityWaiter( void * p );

/* Sanity-check SMP by verifying that a lower priority task can
 * execute while a higher-priority task is consuming one core.
 * The high-priority waiter creates the low-priority task and then holds the core
 * in a busy-loop, waiting on a semaphore given by the low priority task
 */
TEST( ESP_SMP_Task_Creation, LowPriorityTasksRunOnSecondCore )
{
    BaseType_t taskCreated = pdFALSE, testComplete = pdFALSE;
    TaskHandle_t xHighPriorityTaskHandle = NULL;
    struct SimultaneousTasksTest_t test_status;

    test_status.lowPriority = xSemaphoreCreateBinary();
    test_status.done = xSemaphoreCreateBinary();
    test_status.succeeded = pdFALSE;

    if( ( test_status.done != NULL ) && ( test_status.lowPriority != NULL ) )
    {
        taskCreated = xTaskCreate( highPriorityWaiter,
                                   ESP_TEST_WAITER_TASK_NAME,
                                   ESP_TASK_DEFAULT_STACK_SIZE,
                                   &test_status,
                                   ESP_TEST_HIGH_PRIORITY,
                                   &xHighPriorityTaskHandle );

        testComplete = xSemaphoreTake( test_status.done, 2 * TASK_SIMULTANEOUS_EXECUTION_TIMEOUT );
    }

    if( taskCreated )
    {
        vTaskDelete( xHighPriorityTaskHandle );
    }

    if( test_status.done != NULL )
    {
        vSemaphoreDelete( test_status.done );
    }

    if( test_status.lowPriority != NULL )
    {
        vSemaphoreDelete( test_status.lowPriority );
    }

    TEST_ASSERT_TRUE( taskCreated )
    TEST_ASSERT_TRUE( testComplete )
    TEST_ASSERT_TRUE( test_status.succeeded );
}

/* The low-priority task attempts to give a semaphore while starved */
static void lowPriorityTask( void * p )
{
    SemaphoreHandle_t pxSemaphore = ( SemaphoreHandle_t ) p;

    xSemaphoreGive( pxSemaphore );
    vTaskSuspend( NULL );
}

/* The high-priority task waits on a low-priority semaphore while spinning to
 * consume a CPU core*/
static void highPriorityWaiter( void * p )
{
    struct SimultaneousTasksTest_t * results = ( struct SimultaneousTasksTest_t * ) p;

    results->succeeded = pdFALSE;
    TaskHandle_t xLowPriorityTaskHandle;
    /* Create the Low-priority task that will give the semaphore */
    BaseType_t taskCreated = xTaskCreate(
        lowPriorityTask,
        ESP_TEST_LOW_PRIORITY_TASK_NAME,
        ESP_TASK_DEFAULT_STACK_SIZE,
        results->lowPriority,
        ESP_TEST_DEFAULT_PRIORITY,
        &xLowPriorityTaskHandle
        );

    if( taskCreated == pdTRUE )
    {
        /* Use the FreeRTOS timeout api to busy-wait for the semaphore */
        TickType_t xTicksToWait = TASK_SIMULTANEOUS_EXECUTION_TIMEOUT;
        TimeOut_t xTimeOut;
        vTaskSetTimeOutState( &xTimeOut );

        while( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) == pdFALSE )
        {
            results->succeeded = xSemaphoreTake( results->lowPriority, 0 );

            if( results->succeeded )
            {
                break;
            }
        }

        vTaskDelete( xLowPriorityTaskHandle );
    }

    /* Indicate to the test harness that this task is complete */
    xSemaphoreGive( results->done );
    vTaskSuspend( NULL );
}
