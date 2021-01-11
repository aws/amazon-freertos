/*
 * FreeRTOS POSIX V1.2.1
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

/**
 * @file iot_test_posix_unistd.c
 * @brief Tests for implementation of functions in unistd.h.
 */

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/unistd.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"

TEST_GROUP( Full_POSIX_UNISTD );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_POSIX_UNISTD )
{
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_POSIX_UNISTD )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_POSIX_UNISTD )
{
    RUN_TEST_CASE( Full_POSIX_UNISTD, sleep );
    RUN_TEST_CASE( Full_POSIX_UNISTD, usleep );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_UNISTD, sleep )
{
    TEST_ASSERT_EQUAL_INT( 0, sleep( 1 ) );
}

/*-----------------------------------------------------------*/
TEST( Full_POSIX_UNISTD, usleep )
{
    TEST_ASSERT_EQUAL_INT( 0, usleep( 1 ) );
}
