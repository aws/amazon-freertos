/*
 * FreeRTOS Common IO V0.1.2
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

/* Test framework includes. */
#include "unity_fixture.h"

#include "iot_test_common_io_internal.h"

/* Define Test Group. */
TEST_GROUP( Common_IO );

/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( Common_IO )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( Common_IO )
{
}

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( Common_IO )
{
    size_t i = 0;

    #ifdef IOT_TEST_COMMON_IO_UART_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_UART_SUPPORTED; i++ )
        {
            SET_TEST_IOT_UART_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_UART );
        }
    #endif

    #ifdef IOT_TEST_COMMON_IO_I2C_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_I2C_SUPPORTED; i++ )
        {
            SET_TEST_IOT_I2C_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_I2C );
        }
    #endif

    #ifdef IOT_TEST_COMMON_IO_SPI_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_SPI_SUPPORTED; i++ )
        {
            SET_TEST_IOT_SPI_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_SPI );
        }
    #endif
}
