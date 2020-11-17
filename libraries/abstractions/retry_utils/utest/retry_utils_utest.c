/*
 * AWS IoT Device SDK for Embedded C V202011.00
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
 */

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

/* Unity include. */
#include "unity.h"

/* Retry utils library include */
#include "retry_utils.h"

/* Return value of mockRng. */
static int32_t randomValToReturn;

#define TEST_BACKOFF_BASE_VALUE    ( 1000 )
#define TEST_BACKOFF_MAX_VALUE     ( 10000 )
#define TEST_MAX_ATTEMPTS          ( 5 )
/* Parameters to track the next max jitter or number of attempts done. */
static RetryUtilsContext_t retryParams;
/* Return value of #RetryUtils_GetNextBackOff. */
static RetryUtilsStatus_t retryUtilsStatus;
static uint16_t nextBackOff;

/**
 * @brief A mock random number generator.
 */
static int32_t mockRng()
{
    return randomValToReturn;
}

/* ============================   UNITY FIXTURES ============================ */

/* Called before each test method. */
void setUp()
{
    /* Initialize context with random number generator that succeeds. */
    RetryUtils_InitializeParams( &retryParams,
                                 TEST_BACKOFF_BASE_VALUE,
                                 TEST_BACKOFF_MAX_VALUE,
                                 TEST_MAX_ATTEMPTS,
                                 mockRng );
}

/* Called after each test method. */
void tearDown()
{
    randomValToReturn = 0;
    retryUtilsStatus = RetryUtilsSuccess;
    nextBackOff = 0;
}

/* Called at the beginning of the whole suite. */
void suiteSetUp()
{
}

/* Called at the end of the whole suite. */
int suiteTearDown( int numFailures )
{
    return numFailures;
}

/* ========================================================================== */

/**
 * @brief Helper method to make assertions on data of the context object.
 */
static void verifyContextData( RetryUtilsContext_t * pContext,
                               uint32_t expectedAttemptsDone,
                               uint16_t expectedNextJitterMax,
                               uint16_t expectedMaxBackOff,
                               uint32_t expectedMaxAttempts,
                               RetryUtils_RNG_t pExpectedRng )
{
    TEST_ASSERT_EQUAL( expectedNextJitterMax, pContext->nextJitterMax );
    TEST_ASSERT_EQUAL( expectedAttemptsDone, pContext->attemptsDone );
    TEST_ASSERT_EQUAL( expectedMaxAttempts, pContext->maxRetryAttempts );
    TEST_ASSERT_EQUAL( expectedMaxBackOff, pContext->maxBackOffDelay );
    TEST_ASSERT_EQUAL_PTR( pExpectedRng, pContext->pRng );
}

/**
 * @brief Test that #RetryUtils_InitializeParams initializes the context
 * data as expected.
 */
void test_RetryUtils_InitializeParams_Sets_Jitter_Correctly( void )
{
    RetryUtils_InitializeParams( &retryParams,
                                 TEST_BACKOFF_BASE_VALUE,
                                 TEST_BACKOFF_MAX_VALUE,
                                 TEST_MAX_ATTEMPTS,
                                 mockRng );
    verifyContextData( &retryParams,
                       0,
                       TEST_BACKOFF_BASE_VALUE,
                       TEST_BACKOFF_MAX_VALUE,
                       TEST_MAX_ATTEMPTS,
                       mockRng );
}

/**
 * @brief Test that #RetryUtils_GetNextBackOff returns the expected failure
 * and does not update the context data when the random number generator fails.
 */
void test_RetryUtils_GetNextBackOff_Rng_Failure( void )
{
    int32_t testNegativeVal[] = { -1, -10 };
    uint iter = 0;

    /* Initialize context with random number generator that fails. */
    RetryUtils_InitializeParams( &retryParams,
                                 TEST_BACKOFF_BASE_VALUE,
                                 TEST_BACKOFF_MAX_VALUE,
                                 RETRY_UTILS_RETRY_FOREVER,
                                 mockRng );

    /* Test the #RetryUtils_GetNextBackOff API with the 2 negative values
     * from the mock random number generator. */

    for( ; iter < sizeof( testNegativeVal ) / sizeof( int32_t ); iter++ )
    {
        /* Set the random value generated to be negative. */
        randomValToReturn = testNegativeVal[ iter ];

        /* Make sure that the API function returns RNG failure.*/
        TEST_ASSERT_EQUAL( RetryUtilsRngFailure,
                           RetryUtils_GetNextBackOff( &retryParams, &nextBackOff ) );

        /* Make sure that the context data has not changed as the call to
         * RetryUtils_GetNextBackOff failed. */
        verifyContextData( &retryParams,
                           0,
                           TEST_BACKOFF_BASE_VALUE,
                           TEST_BACKOFF_MAX_VALUE,
                           RETRY_UTILS_RETRY_FOREVER,
                           mockRng );
    }
}

/**
 * @brief Test that #RetryUtils_GetNextBackOff returns the expected next back-off
 * value and updates the contents of the context as expected when the random value
 * generated is lower than the max jitter value for the retry attempts.
 *
 * This test assumes that the max jitter value has not reached the configured
 * maximum back-off value.
 */
void test_RetryUtils_GetNextBackOff_Success_RandVal_Less_Than_Jitter_Max( void )
{
    int iter = 1;
    uint16_t expectedAttemptsDone = 0;
    uint16_t expectedNextJitterMax = TEST_BACKOFF_BASE_VALUE;
    uint16_t expectedNextBackOff = 0;

    for( ; iter < 2; iter++ )
    {
        /* Set the random value to be generated as a value lower than
         * the jitter max value for the next retry attempt. */
        randomValToReturn = retryParams.nextJitterMax / 2;

        /* As the random value is less than the max jitter value, the expected
         * next backoff value should remain the same as the random value. */
        expectedNextBackOff = randomValToReturn;

        /* The jitter max value should double with the above call for use in next call. */
        expectedNextJitterMax *= 2;

        /* The number of attempts should be updated by the above API call. */
        expectedAttemptsDone++;

        TEST_ASSERT_LESS_THAN( TEST_BACKOFF_MAX_VALUE, expectedNextJitterMax );

        /* Call the RetryUtils_GetNextBackOff API a couple times. */
        TEST_ASSERT_EQUAL( RetryUtilsSuccess,
                           RetryUtils_GetNextBackOff( &retryParams, &nextBackOff ) );
        TEST_ASSERT_EQUAL( expectedNextBackOff, nextBackOff );

        /* Verify that the context data for expected data after the API call. */
        verifyContextData( &retryParams,
                           expectedAttemptsDone,
                           expectedNextJitterMax,
                           TEST_BACKOFF_MAX_VALUE,
                           TEST_MAX_ATTEMPTS,
                           mockRng );
    }
}

/**
 * @brief Test that #RetryUtils_GetNextBackOff returns the expected next back-off
 * value and updates the contents of the context when the random value generated
 * is higher than the max jitter value for the retry attempts.
 *
 * This test assumes that the max jitter value has not reached the configured
 * maximum back-off value.
 */
void test_RetryUtils_GetNextBackOff_Success_RandVal_More_Than_Jitter_Max( void )
{
    int iter = 1;
    uint16_t expectedAttemptsDone = 0;
    uint16_t expectedNextJitterMax = TEST_BACKOFF_BASE_VALUE;

    for( ; iter < 2; iter++ )
    {
        /* Set the random value to be generated as a value greater than
         * the jitter max value for the next retry attempt. */
        randomValToReturn = retryParams.nextJitterMax + 1;

        /* The jitter max value should double with the above call for use in next call. */
        expectedNextJitterMax *= 2;
        TEST_ASSERT_LESS_THAN( TEST_BACKOFF_MAX_VALUE, expectedNextJitterMax );

        /* The number of attempts should be updated by the above API call. */
        expectedAttemptsDone++;

        /* As the random value is greater than the jitter max value, the expected
         * next backoff value should be truncated to a value within the jitter window
         * for the retry attempt. */
        uint16_t expectedNextBackOff = ( randomValToReturn % ( retryParams.nextJitterMax + 1U ) );

        /* Call the RetryUtils_GetNextBackOff API a couple times. */
        TEST_ASSERT_EQUAL( RetryUtilsSuccess,
                           RetryUtils_GetNextBackOff( &retryParams, &nextBackOff ) );
        TEST_ASSERT_EQUAL( expectedNextBackOff, nextBackOff );

        /* Verify that the context data for expected data after the API call. */
        verifyContextData( &retryParams,
                           expectedAttemptsDone,
                           expectedNextJitterMax,
                           TEST_BACKOFF_MAX_VALUE,
                           TEST_MAX_ATTEMPTS,
                           mockRng );
    }
}

/**
 * @brief Tests the #RetryUtils_GetNextBackOff API when the next back-off value
 * is requested for exhausted retry attempts.
 */
void test_retryUtils_GetNextBackOff_Attempts_Exhausted()
{
    /* Update the context data to represent that maximum configured number of
     * retry attempts have been completed. */
    retryParams.attemptsDone = TEST_MAX_ATTEMPTS;

    /* Call the RetryUtils_GetNextBackOff API. */
    TEST_ASSERT_EQUAL( RetryUtilsRetriesExhausted,
                       RetryUtils_GetNextBackOff( &retryParams, &nextBackOff ) );
    /* Make sure that the value of the output parameter has not changed. */
    TEST_ASSERT_EQUAL( 0, nextBackOff );

    /* Make sure that the context data has not changed as the call to
     * RetryUtils_GetNextBackOff failed. */
    verifyContextData( &retryParams,
                       TEST_MAX_ATTEMPTS /* Number of attempts shouldn't change */,
                       TEST_BACKOFF_BASE_VALUE,
                       TEST_BACKOFF_MAX_VALUE,
                       TEST_MAX_ATTEMPTS,
                       mockRng );
}

/**
 * @brief Test that the value of the next max jitter has a lower bound that will
 * then be capped at some max threshold.
 */
void test_RetryUtils_GetNextBackOff_Returns_Cap_BackOff( void )
{
    /* Initialize to 0 attempts, so the max value of next jitter will increase. */
    retryParams.attemptsDone = 0U;

    /* Update the next jitter value to greater than half the maximum backoff so
     * that the RetryUtils_GetNextBackOff API updates the next jitter value to
     * the configured maximum backoff value. */
    retryParams.nextJitterMax = ( TEST_BACKOFF_MAX_VALUE / 2U ) + 1;

    /* Set the random value to be generated equal to the current jitter max value.
     * Thus, the RetryUtils_GetNextBackOff API should return the random value as
     * the next back-off value. */
    randomValToReturn = retryParams.nextJitterMax;
    uint16_t expectedBackOffVal = randomValToReturn;

    /* Call the RetryUtils_GetNextBackOff API. */
    TEST_ASSERT_EQUAL( RetryUtilsSuccess,
                       RetryUtils_GetNextBackOff( &retryParams, &nextBackOff ) );
    /* Make sure that the expected value is returned for the next backoff. */
    TEST_ASSERT_EQUAL( expectedBackOffVal, nextBackOff );

    /* Verify that the next jitter max value has been set to the cap back-off value
     * configured in the context. */
    verifyContextData( &retryParams,
                       1,
                       TEST_BACKOFF_MAX_VALUE /* New jitter max */,
                       TEST_BACKOFF_MAX_VALUE,
                       TEST_MAX_ATTEMPTS,
                       mockRng );


    /* Now, set the random value to be generated as the maximum back-off value to
     * expect that the next back-off value returned by the API is the maximum
     * back-off value.*/
    randomValToReturn = TEST_BACKOFF_MAX_VALUE;

    /* Call RetryUtils_GetNextBackOff API again to verify that it now returns the
     * cap value as the next back-off value. */
    TEST_ASSERT_EQUAL( RetryUtilsSuccess,
                       RetryUtils_GetNextBackOff( &retryParams, &nextBackOff ) );
    /* Make sure that the capped backoff value is returned as the next backoff value . */
    TEST_ASSERT_EQUAL( TEST_BACKOFF_MAX_VALUE, nextBackOff );

    /* Verify that the context data for expected data after the API call. */
    verifyContextData( &retryParams,
                       2,
                       TEST_BACKOFF_MAX_VALUE /* jitter max remains unchanged */,
                       TEST_BACKOFF_MAX_VALUE,
                       TEST_MAX_ATTEMPTS,
                       mockRng );
}

/**
 * @brief Test that the value of the next max jitter has a lower bound that will
 * then be capped at some max threshold.
 */
void test_RetryUtils_GetNextBackOff_Returns_Rand_Val( void )
{
    /* Initialize to 0 attempts, so the max value of next jitter will increase. */
    retryParams.attemptsDone = 0U;

    /* Set the returned random value to zero to test that the
     * RetryUtils_GetNextBackOff API will return zero as the
     * next back-off value.*/
    randomValToReturn = 0;

    /* Update the next jitter max value to one less than half of max backoff
     * to make sure that the RetryUtils_GetNextBackOff API does not update it
     * to the cap value in the call.*/
    retryParams.nextJitterMax = ( TEST_BACKOFF_MAX_VALUE / 2U ) - 1;

    /* Call the RetryUtils_GetNextBackOff API. */
    TEST_ASSERT_EQUAL( RetryUtilsSuccess,
                       RetryUtils_GetNextBackOff( &retryParams, &nextBackOff ) );
    /* Make sure that zero is returned as the next backoff value . */
    TEST_ASSERT_EQUAL( 0, nextBackOff );

    /* Verify that the context data for expected data after the API call. */
    verifyContextData( &retryParams,
                       1,
                       TEST_BACKOFF_MAX_VALUE - 2U /* next jitter max value */,
                       TEST_BACKOFF_MAX_VALUE,
                       TEST_MAX_ATTEMPTS,
                       mockRng );
}
