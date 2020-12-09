/*
 * FreeRTOS Common IO V0.1.3
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

/*******************************************************************************
 * @file test_iot_efuse.c
 * @brief Functional Unit Test - EFUSE
 *******************************************************************************
 */

/* Test includes */
#include "unity.h"
#include "unity_fixture.h"

/* Driver includes */
#include "iot_efuse.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/* Globals values which can be overwritten by the test
 * framework invoking these tests */
/*-----------------------------------------------------------*/
uint32_t ultestIotEfuse16BitWordValidIdx = 151;        /**< A valid 16-bit word fuse index. */
uint32_t ultestIotEfuse16BitWordInvalidIdx = 159;      /**< An invalid 16-bit word fuse index. */
uint16_t ustestIotEfuse16BitWordWriteVal = 0x5a5a;     /**< test value to write into a 16-bit efuse word */
uint32_t ultestIotEfuse32BitWordValidIdx = 159;        /**< A valid 32-bit word fuse index. */
uint32_t ultestIotEfuse32BitWordInvalidIdx = 151;      /**< An invalid 32-bit word fuse index. */
uint32_t ultestIotEfuse32BitWordWriteVal = 0x5a5a5a5a; /**< test value to write into a 32-bit efuse word */


/*-----------------------------------------------------------*/
/** Static globals */
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

/* Define Test Group. */
TEST_GROUP( TEST_IOT_EFUSE );

/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_EFUSE )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_EFUSE )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_EFUSE )
{
    RUN_TEST_CASE( TEST_IOT_EFUSE, AFQP_IotEfuseOpenClose );
    RUN_TEST_CASE( TEST_IOT_EFUSE, AFQP_IotEfuseWriteRead32BitWord );
    RUN_TEST_CASE( TEST_IOT_EFUSE, AFQP_IotEfuseWriteRead16BitWord );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_efuse_open and iot_efuse_close.
 *
 */
TEST( TEST_IOT_EFUSE, AFQP_IotEfuseOpenClose )
{
    IotEfuseHandle_t xEfuseHandle = NULL;
    IotEfuseHandle_t xEfuseHandle2 = NULL;
    int32_t lRetVal;

    /* Open efuse to initialize hardware. */
    xEfuseHandle = iot_efuse_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xEfuseHandle );

    /* Open again should get NULL. */
    xEfuseHandle2 = iot_efuse_open();
    TEST_ASSERT_EQUAL( NULL, xEfuseHandle2 );

    /* Close efuse to deinit hardware. */
    lRetVal = iot_efuse_close( xEfuseHandle );
    TEST_ASSERT_EQUAL( IOT_EFUSE_SUCCESS, lRetVal );

    /* Close again should get IOT_EFUSE_INVALID_VALUE */
    lRetVal = iot_efuse_close( xEfuseHandle );
    TEST_ASSERT_EQUAL( IOT_EFUSE_INVALID_VALUE, lRetVal );
}

/**
 * @brief Test Function to test iot_efuse_write_32bit_word()
 * iot_efuse_read_32bit_word().
 *
 */
TEST( TEST_IOT_EFUSE, AFQP_IotEfuseWriteRead32BitWord )
{
    IotEfuseHandle_t xEfuseHandle = NULL;
    int32_t lRetVal;
    uint32_t ulInvalidIndex = ultestIotEfuse32BitWordInvalidIdx;
    uint32_t ulValidIndex = ultestIotEfuse32BitWordValidIdx;
    uint32_t ulWriteVal = ultestIotEfuse32BitWordWriteVal;
    uint32_t ulReadVal;

    /* Write to 32-bit wide efuse word using a NULL handle */
    lRetVal = iot_efuse_write_32bit_word( xEfuseHandle, ulValidIndex, ulWriteVal );

    if( lRetVal != IOT_EFUSE_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_EFUSE_INVALID_VALUE, lRetVal );
    }

    /* Read a 32-bit wide efuse word using a NULL handle */
    lRetVal = iot_efuse_read_32bit_word( xEfuseHandle, ulValidIndex, ( uint32_t * ) &ulReadVal );

    if( lRetVal != IOT_EFUSE_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_EFUSE_INVALID_VALUE, lRetVal );
    }

    /* Open efuse to initialize hardware. */
    xEfuseHandle = iot_efuse_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xEfuseHandle );

    /* Write to 32-bit wide efuse word using an invalid index */
    lRetVal = iot_efuse_write_32bit_word( xEfuseHandle, ulInvalidIndex, ulWriteVal );

    if( lRetVal != IOT_EFUSE_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_EFUSE_INVALID_VALUE, lRetVal );
    }

    /* Read a 32-bit wide efuse word using an invalid index */
    lRetVal = iot_efuse_read_32bit_word( xEfuseHandle, ulInvalidIndex, ( uint32_t * ) &ulReadVal );

    if( lRetVal != IOT_EFUSE_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_EFUSE_INVALID_VALUE, lRetVal );
    }

    /* Write a 32-bit wide efuse word using a valid index */
    lRetVal = iot_efuse_write_32bit_word( xEfuseHandle, ulValidIndex, ulWriteVal );

    if( lRetVal != IOT_EFUSE_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_EFUSE_SUCCESS, lRetVal );
    }

    /* Read back to check it vlaue is written correctly */
    lRetVal = iot_efuse_read_32bit_word( xEfuseHandle, ulValidIndex, ( uint32_t * ) &ulReadVal );

    if( lRetVal != IOT_EFUSE_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_EFUSE_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( ulWriteVal, ulReadVal );
    }

    /* Read a 32-bit wide efuse word using a NULL receive buffer */
    lRetVal = iot_efuse_read_32bit_word( xEfuseHandle, ulValidIndex, NULL );

    if( lRetVal != IOT_EFUSE_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_EFUSE_INVALID_VALUE, lRetVal );
    }

    lRetVal = iot_efuse_close( xEfuseHandle );
    TEST_ASSERT_EQUAL( IOT_EFUSE_SUCCESS, lRetVal );
}


/**
 * @brief Test Function to test iot_efuse_write_16bit_word()
 * and iot_efuse_read_16bit_word().
 *
 */
TEST( TEST_IOT_EFUSE, AFQP_IotEfuseWriteRead16BitWord )
{
    IotEfuseHandle_t xEfuseHandle = NULL;
    int32_t lRetVal;
    uint32_t ulInvalidIndex = ultestIotEfuse16BitWordInvalidIdx;
    uint32_t ulValidIndex = ultestIotEfuse16BitWordValidIdx;
    uint16_t usWriteVal = ustestIotEfuse16BitWordWriteVal;
    uint16_t ulReadVal;

    /* Write to 16-bit wide efuse word using a NULL handle*/
    lRetVal = iot_efuse_write_16bit_word( xEfuseHandle, ulValidIndex, usWriteVal );

    if( lRetVal != IOT_EFUSE_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_EFUSE_INVALID_VALUE, lRetVal );
    }

    /* Read a 16-bit wide efuse word using a NULL handle */
    lRetVal = iot_efuse_read_16bit_word( xEfuseHandle, ulValidIndex, ( uint16_t * ) &ulReadVal );

    if( lRetVal != IOT_EFUSE_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_EFUSE_INVALID_VALUE, lRetVal );
    }

    /* Open efuse to initialize hardware. */
    xEfuseHandle = iot_efuse_open();
    TEST_ASSERT_NOT_EQUAL( NULL, xEfuseHandle );

    /* Write to 16-bit wide efuse word using an invalid index */
    lRetVal = iot_efuse_write_16bit_word( xEfuseHandle, ulInvalidIndex, usWriteVal );

    if( lRetVal != IOT_EFUSE_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_EFUSE_INVALID_VALUE, lRetVal );
    }

    /* Read a 16-bit wide efuse word using an invalid index */
    lRetVal = iot_efuse_read_16bit_word( xEfuseHandle, ulInvalidIndex, ( uint16_t * ) &ulReadVal );

    if( lRetVal != IOT_EFUSE_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_EFUSE_INVALID_VALUE, lRetVal );
    }

    /* Write a 16-bit wide efuse word using a valid index */
    lRetVal = iot_efuse_write_16bit_word( xEfuseHandle, ulValidIndex, usWriteVal );

    if( lRetVal != IOT_EFUSE_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_EFUSE_SUCCESS, lRetVal );
    }

    /* Read back to check it vlaue is written correctly */
    lRetVal = iot_efuse_read_16bit_word( xEfuseHandle, ulValidIndex, ( uint16_t * ) &ulReadVal );

    if( lRetVal != IOT_EFUSE_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_EFUSE_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( usWriteVal, ulReadVal );
    }

    /* Read a 16-bit wide efuse word using a NULL receive buffer */
    lRetVal = iot_efuse_read_16bit_word( xEfuseHandle, ulValidIndex, NULL );

    if( lRetVal != IOT_EFUSE_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_EFUSE_INVALID_VALUE, lRetVal );
    }

    /* Close efuse interface */
    lRetVal = iot_efuse_close( xEfuseHandle );
    TEST_ASSERT_EQUAL( IOT_EFUSE_SUCCESS, lRetVal );
}
