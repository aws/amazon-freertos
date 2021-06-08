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
 * @file iot_test_i2s.c
 * @brief Functional Unit Test - I2S
 *******************************************************************************
 */

#include "unity.h"
#include "unity_fixture.h"
#include <string.h>
/* Driver includes */
#include "iot_i2s.h"

#include "FreeRTOS.h"
#include "semphr.h"

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
#define testIotI2S_BUF_SIZE                   ( 512 )
#define testIotI2S_DEFAULT_SEMAPHORE_DELAY    ( 100000U )

/* expected loss of data because of time it takes to transition from write operation to read, or vice versa */
#define testIotI2S_LOOPBACK_OVERHEAD          ( 32 )

/*-----------------------------------------------------------*/

/* Globals values which can be overwritten by the test
 * framework invoking these tests */
/*-----------------------------------------------------------*/
int32_t ltestIotI2sInputInstance = 4;  /* Use Mic instance 4 */
int32_t ltestIotI2sOutputInstance = 2; /* Use Spkr instance 2 */
int32_t ltestIotI2SReadSize = testIotI2S_BUF_SIZE;
/*-----------------------------------------------------------*/
/* Static Globals */
/*-----------------------------------------------------------*/
static SemaphoreHandle_t xtestIotI2SWriteSemaphore = NULL;
static SemaphoreHandle_t xtestIotI2SReadSemaphore = NULL;

/*-----------------------------------------------------------*/

/* Define Test Group. */
TEST_GROUP( TEST_IOT_I2S );

/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_I2S )
{
    xtestIotI2SWriteSemaphore = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotI2SWriteSemaphore );
    xtestIotI2SReadSemaphore = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotI2SReadSemaphore );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_I2S )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief user callback function for write operations
 */
void prvI2SWriteCallback( IotI2SOperationStatus_t xOpStatus,
                          void * pvUserContext )
{
    BaseType_t xHigherPriorityTaskWoken;

    xSemaphoreGiveFromISR( xtestIotI2SWriteSemaphore, &xHigherPriorityTaskWoken );
}

/**
 * @brief user callback function for read operations
 */
void prvI2SReadCallback( IotI2SOperationStatus_t xOpStatus,
                         void * pvUserContext )
{
    BaseType_t xHigherPriorityTaskWoken;

    xSemaphoreGiveFromISR( xtestIotI2SReadSemaphore, &xHigherPriorityTaskWoken );
}


/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_I2S )
{
    RUN_TEST_CASE( TEST_IOT_I2S, AFQP_IotI2SOpenClose );
    RUN_TEST_CASE( TEST_IOT_I2S, AFQP_IotI2SIoctl );
    RUN_TEST_CASE( TEST_IOT_I2S, AFQP_IotI2SCancelFail );
    RUN_TEST_CASE( TEST_IOT_I2S, AFQP_IotI2SCancelSuccess );
    RUN_TEST_CASE( TEST_IOT_I2S, AFQP_IotI2SOpenCloseFuzzing );
    RUN_TEST_CASE( TEST_IOT_I2S, AFQP_IotI2SIoctlFuzzing );
    RUN_TEST_CASE( TEST_IOT_I2S, AFQP_IotI2SReadSyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_I2S, AFQP_IotI2SWriteSyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_I2S, AFQP_IotI2SReadAsyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_I2S, AFQP_IotI2SWriteAsyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_I2S, AFQP_IotI2SCancelFuzzing );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_i2s_open and iot_i2s_close.
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SOpenClose )
{
    IotI2SHandle_t xI2SHandle;
    int32_t lRetVal;

    /* Open i2s to initialize hardware */
    xI2SHandle = iot_i2s_open( ltestIotI2sInputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SHandle );

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test Write Async Read Async in loopback configuration
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SWriteAsyncReadAsyncLoopback )
{
    IotI2SHandle_t xI2SMicHandle;
    IotI2SHandle_t xI2SSpkrHandle;
    int32_t lRetVal;
    uint8_t ucRxbuf[ testIotI2S_BUF_SIZE ] = { 0 };
    uint8_t ucTxbuf[ testIotI2S_BUF_SIZE ] = { 0 };
    int i;
    uint32_t ulRxTxBytes;

    for( i = 0; i < testIotI2S_BUF_SIZE; i++ )
    {
        ucTxbuf[ i ] = i & 0xff;
    }

    /* Open i2s to initialize hardware */
    xI2SMicHandle = iot_i2s_open( ltestIotI2sInputInstance );
    xI2SSpkrHandle = iot_i2s_open( ltestIotI2sOutputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SMicHandle );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SSpkrHandle );

    if( TEST_PROTECT() )
    {
        /* Set a callback for async call */
        iot_i2s_set_callback( xI2SMicHandle, prvI2SReadCallback, NULL );
        iot_i2s_set_callback( xI2SSpkrHandle, prvI2SWriteCallback, NULL );

        lRetVal = iot_i2s_write_async( xI2SSpkrHandle, ucTxbuf, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        lRetVal = iot_i2s_read_async( xI2SMicHandle, ucRxbuf, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        lRetVal = xSemaphoreTake( xtestIotI2SWriteSemaphore, portMAX_DELAY );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        lRetVal = xSemaphoreTake( xtestIotI2SReadSemaphore, portMAX_DELAY );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        /* Get Tx Num bytes */
        ulRxTxBytes = 0;
        lRetVal = iot_i2s_ioctl( xI2SSpkrHandle, eI2SGetTxNoOfbytes, &ulRxTxBytes );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( testIotI2S_BUF_SIZE, ulRxTxBytes );

        /* Get Rx Num bytes */
        ulRxTxBytes = 0;
        lRetVal = iot_i2s_ioctl( xI2SMicHandle, eI2SGetRxNoOfbytes, &ulRxTxBytes );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( testIotI2S_BUF_SIZE, ulRxTxBytes );

        /* Validate data was read correctly */
        for( i = testIotI2S_LOOPBACK_OVERHEAD; i < testIotI2S_BUF_SIZE - testIotI2S_LOOPBACK_OVERHEAD; i++ )
        {
            TEST_ASSERT_EQUAL( ( ucRxbuf[ i ] + 1 ) & 0xff, ucRxbuf[ i + 1 ] );
        }
    }

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SMicHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
    lRetVal = iot_i2s_close( xI2SSpkrHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test Write Sync Read Async in loopback configuration
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SWriteSyncReadAsyncLoopback )
{
    IotI2SHandle_t xI2SMicHandle;
    IotI2SHandle_t xI2SSpkrHandle;
    int32_t lRetVal;
    uint8_t ucRxbuf[ testIotI2S_BUF_SIZE ] = { 0 };
    uint8_t ucTxbuf[ testIotI2S_BUF_SIZE ] = { 0 };
    int i;
    uint32_t ulRxTxBytes;

    for( i = 0; i < testIotI2S_BUF_SIZE; i++ )
    {
        ucTxbuf[ i ] = i & 0xff;
    }

    /* Open i2s to initialize hardware */
    xI2SMicHandle = iot_i2s_open( ltestIotI2sInputInstance );
    xI2SSpkrHandle = iot_i2s_open( ltestIotI2sOutputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SMicHandle );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SSpkrHandle );

    if( TEST_PROTECT() )
    {
        /* Set a callback for async call */
        iot_i2s_set_callback( xI2SMicHandle, prvI2SReadCallback, NULL );

        lRetVal = iot_i2s_write_sync( xI2SSpkrHandle, ucTxbuf, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        lRetVal = iot_i2s_read_async( xI2SMicHandle, ucRxbuf, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        lRetVal = xSemaphoreTake( xtestIotI2SReadSemaphore, portMAX_DELAY );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        /* Get Rx Num bytes */
        ulRxTxBytes = 0;
        lRetVal = iot_i2s_ioctl( xI2SMicHandle, eI2SGetRxNoOfbytes, &ulRxTxBytes );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( testIotI2S_BUF_SIZE, ulRxTxBytes );

        /* Validate data was read correctly */
        for( i = testIotI2S_LOOPBACK_OVERHEAD; i < testIotI2S_BUF_SIZE - testIotI2S_LOOPBACK_OVERHEAD; i++ )
        {
            TEST_ASSERT_EQUAL( ( ucRxbuf[ i ] + 1 ) & 0xff, ucRxbuf[ i + 1 ] );
        }
    }

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SMicHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
    lRetVal = iot_i2s_close( xI2SSpkrHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test Write Async Read Sync in loopback configuration
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SWriteAsyncReadSyncLoopback )
{
    IotI2SHandle_t xI2SMicHandle;
    IotI2SHandle_t xI2SSpkrHandle;
    int32_t lRetVal;
    uint8_t ucRxbuf[ testIotI2S_BUF_SIZE ] = { 0 };
    uint8_t ucTxbuf[ testIotI2S_BUF_SIZE ] = { 0 };
    int i;
    uint32_t ulRxTxBytes;

    for( i = 0; i < testIotI2S_BUF_SIZE; i++ )
    {
        ucTxbuf[ i ] = i & 0xff;
    }

    /* Open i2s to initialize hardware */
    xI2SMicHandle = iot_i2s_open( ltestIotI2sInputInstance );
    xI2SSpkrHandle = iot_i2s_open( ltestIotI2sOutputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SMicHandle );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SSpkrHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_i2s_write_async( xI2SSpkrHandle, ucTxbuf, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        lRetVal = iot_i2s_read_sync( xI2SMicHandle, ucRxbuf, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        /* Get Tx Num bytes */
        ulRxTxBytes = 0;
        lRetVal = iot_i2s_ioctl( xI2SMicHandle, eI2SGetTxNoOfbytes, &ulRxTxBytes );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( testIotI2S_BUF_SIZE, ulRxTxBytes );

        /* Validate data was read correctly */
        for( i = testIotI2S_LOOPBACK_OVERHEAD; i < testIotI2S_BUF_SIZE - testIotI2S_LOOPBACK_OVERHEAD; i++ )
        {
            TEST_ASSERT_EQUAL( ( ucRxbuf[ i ] + 1 ) & 0xff, ucRxbuf[ i + 1 ] );
        }
    }

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SMicHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
    lRetVal = iot_i2s_close( xI2SSpkrHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test Write Sync Read Sync in loopback configuration
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SWriteSyncReadSyncLoopback )
{
    IotI2SHandle_t xI2SMicHandle;
    IotI2SHandle_t xI2SSpkrHandle;
    int32_t lRetVal;
    uint8_t ucRxbuf[ testIotI2S_BUF_SIZE ] = { 0 };
    uint8_t ucTxbuf[ testIotI2S_BUF_SIZE ] = { 0 };
    int i;

    for( i = 0; i < testIotI2S_BUF_SIZE; i++ )
    {
        ucTxbuf[ i ] = i & 0xff;
    }

    /* Open i2s to initialize hardware */
    xI2SMicHandle = iot_i2s_open( ltestIotI2sInputInstance );
    xI2SSpkrHandle = iot_i2s_open( ltestIotI2sOutputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SMicHandle );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SSpkrHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_i2s_write_sync( xI2SSpkrHandle, ucTxbuf, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        lRetVal = iot_i2s_read_sync( xI2SMicHandle, ucRxbuf, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        /* Validate data was read correctly */
        for( i = testIotI2S_LOOPBACK_OVERHEAD; i < testIotI2S_BUF_SIZE - testIotI2S_LOOPBACK_OVERHEAD; i++ )
        {
            TEST_ASSERT_EQUAL( ( ucRxbuf[ i ] + 1 ) & 0xff, ucRxbuf[ i + 1 ] );
        }
    }

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SMicHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
    lRetVal = iot_i2s_close( xI2SSpkrHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test Read Sync in assisted configuration
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SReadSync )
{
    IotI2SHandle_t xI2SMicHandle;
    int32_t lRetVal;
    uint8_t ucRxbuf[ testIotI2S_BUF_SIZE ] = { 0 };
    int i;

    /* Open i2s to initialize hardware */
    xI2SMicHandle = iot_i2s_open( ltestIotI2sInputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SMicHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_i2s_read_sync( xI2SMicHandle, ucRxbuf, ltestIotI2SReadSize );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        /* Validate data was read correctly */
        for( i = 0; i < ltestIotI2SReadSize; i++ )
        {
            TEST_ASSERT_EQUAL( ( i & 0xff ), ucRxbuf[ i ] );
        }
    }

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SMicHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test Write Sync in assisted configuration
 * @note: Read is verified through assisted test
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SWriteSync )
{
    IotI2SHandle_t xI2SSpkrHandle;
    int32_t lRetVal;
    uint8_t ucTxbuf[ testIotI2S_BUF_SIZE ] = { 0 };
    int i;

    for( i = 0; i < testIotI2S_BUF_SIZE; i++ )
    {
        ucTxbuf[ i ] = i & 0xff;
    }

    /* Open i2s to initialize hardware */
    xI2SSpkrHandle = iot_i2s_open( ltestIotI2sInputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SSpkrHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_i2s_write_sync( xI2SSpkrHandle, ucTxbuf, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
    }

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SSpkrHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test Read Async in assisted configuration
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SReadAsync )
{
    IotI2SHandle_t xI2SMicHandle;
    int32_t lRetVal;
    uint8_t ucRxbuf[ testIotI2S_BUF_SIZE ] = { 0 };
    int i;
    uint32_t ulRxTxBytes;

    /* Open i2s to initialize hardware */
    xI2SMicHandle = iot_i2s_open( ltestIotI2sInputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SMicHandle );

    iot_i2s_set_callback( xI2SMicHandle, prvI2SReadCallback, NULL );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_i2s_read_async( xI2SMicHandle, ucRxbuf, ltestIotI2SReadSize );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        lRetVal = xSemaphoreTake( xtestIotI2SReadSemaphore, portMAX_DELAY );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        /* Get Rx Num bytes */
        ulRxTxBytes = 0;
        lRetVal = iot_i2s_ioctl( xI2SMicHandle, eI2SGetTxNoOfbytes, &ulRxTxBytes );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( testIotI2S_BUF_SIZE, ulRxTxBytes );

        /* Validate data was read correctly */
        for( i = 0; i < ltestIotI2SReadSize; i++ )
        {
            TEST_ASSERT_EQUAL( ( i & 0xff ), ucRxbuf[ i ] );
        }
    }

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SMicHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test Write Async in assisted configuration
 * @note: Read is verified through assisted test
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SWriteAsync )
{
    IotI2SHandle_t xI2SSpkrHandle;
    int32_t lRetVal;
    uint8_t ucTxbuf[ testIotI2S_BUF_SIZE ] = { 0 };
    int i;
    uint32_t ulRxTxBytes;

    for( i = 0; i < testIotI2S_BUF_SIZE; i++ )
    {
        ucTxbuf[ i ] = i & 0xff;
    }

    /* Open i2s to initialize hardware */
    xI2SSpkrHandle = iot_i2s_open( ltestIotI2sInputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SSpkrHandle );

    /* Set a callback for async call */
    iot_i2s_set_callback( xI2SSpkrHandle, prvI2SWriteCallback, NULL );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_i2s_write_async( xI2SSpkrHandle, ucTxbuf, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        lRetVal = xSemaphoreTake( xtestIotI2SWriteSemaphore, portMAX_DELAY );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        /* Get Rx Num bytes */
        ulRxTxBytes = 0;
        lRetVal = iot_i2s_ioctl( xI2SSpkrHandle, eI2SGetTxNoOfbytes, &ulRxTxBytes );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( testIotI2S_BUF_SIZE, ulRxTxBytes );
    }

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SSpkrHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test Ioctl api
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SIoctl )
{
    IotI2SHandle_t xI2SHandle;
    int32_t lRetVal;
    IotI2sIoctlConfig_t xOrigConfig, xTestConfig, xConfirmConfig;
    IotI2SMode_t xMode;
    IotI2SChannel_t xChannel;
    IotI2SClkPolarity_t xPolarity;
    uint32_t ulBusState;
    uint32_t ulRxTxBytes;
    uint32_t ulFreqVals[ 2 ] = { 8000, 16000 };
    uint32_t ulFrameLenVals[ 2 ] = { 32, 64 };
    uint32_t ulDataLenVals[ 2 ] = { 8, 16 };

    /* Open i2s to initialize hardware */
    xI2SHandle = iot_i2s_open( ltestIotI2sInputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SHandle );

    if( TEST_PROTECT() )
    {
        /* Save off original config */
        lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SGetConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
        memcpy( &xTestConfig, &xOrigConfig, sizeof( IotI2sIoctlConfig_t ) );

        /* Validate different modes */
        for( xMode = eI2SNormalMode; xMode <= eI2SDspMode; xMode++ )
        {
            xTestConfig.xI2SMode = xMode;

            lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SSetConfig, &xTestConfig );

            if( lRetVal != IOT_I2S_FUNCTION_NOT_SUPPORTED )
            {
                TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

                lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SGetConfig, &xConfirmConfig );
                TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
                TEST_ASSERT_EQUAL( xConfirmConfig.xI2SMode, xMode );
            }
        }

        xTestConfig.xI2SMode = xMode = eI2SNormalMode;

        /* Validate different channels */
        for( xChannel = eI2SChannelStereo; xChannel <= eI2SChannelMono; xChannel++ )
        {
            xTestConfig.xI2SChannel = xChannel;

            lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SSetConfig, &xTestConfig );
            TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

            lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SGetConfig, &xConfirmConfig );
            TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
            TEST_ASSERT_EQUAL( xConfirmConfig.xI2SChannel, xChannel );
        }

        xTestConfig.xI2SChannel = eI2SChannelStereo;

        /* Validate different polarities */
        for( xPolarity = eI2SFallingEdge; xPolarity <= eI2SRisingEdge; xPolarity++ )
        {
            xTestConfig.xI2SSckPolarity = xPolarity;
            xTestConfig.xI2SWsPolarity = xPolarity;

            lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SSetConfig, &xTestConfig );
            TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

            lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SGetConfig, &xConfirmConfig );
            TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
            TEST_ASSERT_EQUAL( xConfirmConfig.xI2SSckPolarity, xPolarity );
            TEST_ASSERT_EQUAL( xConfirmConfig.xI2SWsPolarity, xPolarity );
        }

        xTestConfig.xI2SSckPolarity = eI2SFallingEdge;
        xTestConfig.xI2SWsPolarity = eI2SFallingEdge;

        /* Validate different frequencies */
        if( xTestConfig.ulI2SFrequency != ulFreqVals[ 0 ] )
        {
            xTestConfig.ulI2SFrequency = ulFreqVals[ 0 ];
        }
        else
        {
            xTestConfig.ulI2SFrequency = ulFreqVals[ 1 ];
        }

        lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SSetConfig, &xTestConfig );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SGetConfig, &xConfirmConfig );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( xConfirmConfig.ulI2SFrequency, xTestConfig.ulI2SFrequency );

        /* Validate different data lengths */
        if( xTestConfig.ulI2SDataLength != ulDataLenVals[ 0 ] )
        {
            xTestConfig.ulI2SDataLength = ulDataLenVals[ 0 ];
        }
        else
        {
            xTestConfig.ulI2SDataLength = ulDataLenVals[ 1 ];
        }

        lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SSetConfig, &xTestConfig );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SGetConfig, &xConfirmConfig );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( xConfirmConfig.ulI2SDataLength, xTestConfig.ulI2SDataLength );

        /* Validate different frame lengths */
        if( xTestConfig.ulI2SFrameLength != ulFrameLenVals[ 0 ] )
        {
            xTestConfig.ulI2SFrameLength = ulFrameLenVals[ 0 ];
        }
        else
        {
            xTestConfig.ulI2SFrameLength = ulFrameLenVals[ 1 ];
        }

        lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SSetConfig, &xTestConfig );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SGetConfig, &xConfirmConfig );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( xConfirmConfig.ulI2SFrameLength, xTestConfig.ulI2SFrameLength );

        /* Restore original config */
        lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SSetConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        /* Get bus state */
        lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SGetBusState, &ulBusState );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        /* Get Tx Num bytes */
        lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SGetTxNoOfbytes, &ulRxTxBytes );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        /* Get Rx Num bytes */
        lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SGetRxNoOfbytes, &ulRxTxBytes );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
    }
    else
    {
        TEST_FAIL();
    }

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test i2s cancel with nothing to cancel
 *
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SCancelFail )
{
    IotI2SHandle_t xI2SHandle;
    int32_t lRetVal;

    /* Open i2s to initialize hardware */
    xI2SHandle = iot_i2s_open( ltestIotI2sInputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SHandle );

    lRetVal = iot_i2s_cancel( xI2SHandle );

    if( lRetVal != IOT_I2S_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_I2S_NOTHING_TO_CANCEL, lRetVal );
    }

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test i2s cancel
 *
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SCancelSuccess )
{
    IotI2SHandle_t xI2SHandle;
    int32_t lRetVal;
    uint8_t ucBuf[ testIotI2S_BUF_SIZE ];

    /* Open i2s to initialize hardware */
    xI2SHandle = iot_i2s_open( ltestIotI2sInputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SHandle );

    /* Set a callback for async call */
    iot_i2s_set_callback( xI2SHandle, prvI2SReadCallback, NULL );

    /* Cancel Read */
    lRetVal = iot_i2s_read_async( xI2SHandle, ucBuf, testIotI2S_BUF_SIZE );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

    lRetVal = iot_i2s_cancel( xI2SHandle );

    if( lRetVal != IOT_I2S_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
    }

    /* Cancel write */
    lRetVal = iot_i2s_write_async( xI2SHandle, ucBuf, testIotI2S_BUF_SIZE );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

    lRetVal = iot_i2s_cancel( xI2SHandle );

    if( lRetVal != IOT_I2S_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
    }

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test Ioctl returns busy when reading
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SReadAsyncIoctlBusy )
{
    IotI2SHandle_t xI2SHandle;
    int32_t lRetVal;
    uint8_t uxRxbuf[ testIotI2S_BUF_SIZE ];
    IotI2sIoctlConfig_t xOrigConfig;

    /* Open i2s to initialize hardware */
    xI2SHandle = iot_i2s_open( ltestIotI2sInputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SHandle );

    if( TEST_PROTECT() )
    {
        /* Save off original config */
        lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SGetConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        /* Set a callback for async call */
        iot_i2s_set_callback( xI2SHandle, prvI2SReadCallback, NULL );

        lRetVal = iot_i2s_read_async( xI2SHandle, uxRxbuf, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        /* Try setting config while busy */
        lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SGetConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_I2S_BUSY, lRetVal );

        /*Wait for the callback. */
        lRetVal = xSemaphoreTake( xtestIotI2SReadSemaphore, testIotI2S_DEFAULT_SEMAPHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
    }

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test Ioctl returns busy when writing
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SWriteAsyncIoctlBusy )
{
    IotI2SHandle_t xI2SHandle;
    int32_t lRetVal;
    uint8_t uxTxbuf[ testIotI2S_BUF_SIZE ];
    IotI2sIoctlConfig_t xOrigConfig;

    /* Open i2s to initialize hardware */
    xI2SHandle = iot_i2s_open( ltestIotI2sOutputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SHandle );

    if( TEST_PROTECT() )
    {
        /* Save off original config */
        lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SGetConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        /* Set a callback for async call */
        iot_i2s_set_callback( xI2SHandle, prvI2SWriteCallback, NULL );

        lRetVal = iot_i2s_write_async( xI2SHandle, uxTxbuf, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

        /* Try setting config while busy */
        lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SGetConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_I2S_BUSY, lRetVal );

        /*Wait for the callback. */
        lRetVal = xSemaphoreTake( xtestIotI2SWriteSemaphore, testIotI2S_DEFAULT_SEMAPHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
    }

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test two consecutive Read Async
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SReadAsyncTwice )
{
    IotI2SHandle_t xI2SHandle;
    int32_t lRetVal;
    uint8_t ucRxbuf1[ testIotI2S_BUF_SIZE ];
    uint8_t ucRxbuf2[ testIotI2S_BUF_SIZE ];

    /* Open i2s to initialize hardware */
    xI2SHandle = iot_i2s_open( ltestIotI2sInputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SHandle );

    /* Set a callback for async call */
    iot_i2s_set_callback( xI2SHandle, prvI2SReadCallback, NULL );

    if( TEST_PROTECT() )
    {
        /* Test read_async() read_async */
        lRetVal = iot_i2s_read_async( xI2SHandle, ucRxbuf1, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
        lRetVal = iot_i2s_read_async( xI2SHandle, ucRxbuf2, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_BUSY, lRetVal );

        /*Wait for the callback. */
        lRetVal = xSemaphoreTake( xtestIotI2SReadSemaphore, testIotI2S_DEFAULT_SEMAPHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        /* Test read_async() read_sync */
        lRetVal = iot_i2s_read_async( xI2SHandle, ucRxbuf1, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
        lRetVal = iot_i2s_read_sync( xI2SHandle, ucRxbuf2, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_BUSY, lRetVal );

        /*Wait for the callback. */
        lRetVal = xSemaphoreTake( xtestIotI2SReadSemaphore, testIotI2S_DEFAULT_SEMAPHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
    }

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test two consecutive Write Async
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SWriteAsyncTwice )
{
    IotI2SHandle_t xI2SHandle;
    int32_t lRetVal;
    uint8_t uxTxbuf1[ testIotI2S_BUF_SIZE ];
    uint8_t uxTxbuf2[ testIotI2S_BUF_SIZE ];

    /* Open i2s to initialize hardware */
    xI2SHandle = iot_i2s_open( ltestIotI2sOutputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SHandle );

    /* Set a callback for async call */
    iot_i2s_set_callback( xI2SHandle, prvI2SWriteCallback, NULL );

    if( TEST_PROTECT() )
    {
        /* test write_async write_async */
        lRetVal = iot_i2s_write_async( xI2SHandle, uxTxbuf1, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
        lRetVal = iot_i2s_write_async( xI2SHandle, uxTxbuf2, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_BUSY, lRetVal );

        /*Wait for the callback. */
        lRetVal = xSemaphoreTake( xtestIotI2SWriteSemaphore, testIotI2S_DEFAULT_SEMAPHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        /* test write_async write_sync */
        lRetVal = iot_i2s_write_async( xI2SHandle, uxTxbuf1, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
        lRetVal = iot_i2s_write_sync( xI2SHandle, uxTxbuf2, testIotI2S_BUF_SIZE );
        TEST_ASSERT_EQUAL( IOT_I2S_BUSY, lRetVal );

        /*Wait for the callback. */
        lRetVal = xSemaphoreTake( xtestIotI2SWriteSemaphore, testIotI2S_DEFAULT_SEMAPHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
    }

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test fuzzing of Open Close apis
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SOpenCloseFuzzing )
{
    IotI2SHandle_t xI2SHandle, xI2SHandleFail;
    int32_t lRetVal;

    /* Open i2s to initialize hardware with invalid instance, expect NULL*/
    xI2SHandle = iot_i2s_open( -1 );
    TEST_ASSERT_EQUAL( NULL, xI2SHandle );

    /* Open i2s to initialize hardware */
    xI2SHandle = iot_i2s_open( ltestIotI2sInputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SHandle );

    /* Open second i2s to initialize hardware.  Expect failure */
    xI2SHandleFail = iot_i2s_open( ltestIotI2sInputInstance );
    TEST_ASSERT_EQUAL( NULL, xI2SHandleFail );

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );

    /* Close instance with stale handle*/
    lRetVal = iot_i2s_close( xI2SHandle );
    TEST_ASSERT_NOT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test fuzzing of ioctl api
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SIoctlFuzzing )
{
    IotI2SHandle_t xI2SHandle;
    int32_t lRetVal;
    IotI2SBusStatus_t xBusStatus;

    /* Open i2s to initialize hardware */
    xI2SHandle = iot_i2s_open( ltestIotI2sInputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SHandle );

    lRetVal = iot_i2s_ioctl( NULL, eI2SGetBusState, &xBusStatus );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    lRetVal = iot_i2s_ioctl( xI2SHandle, -1, &xBusStatus );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    lRetVal = iot_i2s_ioctl( xI2SHandle, eI2SGetBusState, NULL );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test fuzzing of read sync api
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SReadSyncFuzzing )
{
    IotI2SHandle_t xI2SHandle;
    int32_t lRetVal;
    uint8_t uxRxbuf[ testIotI2S_BUF_SIZE ];

    /* Open i2s to initialize hardware */
    xI2SHandle = iot_i2s_open( ltestIotI2sInputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SHandle );

    lRetVal = iot_i2s_read_sync( NULL, uxRxbuf, testIotI2S_BUF_SIZE );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    lRetVal = iot_i2s_read_sync( xI2SHandle, NULL, testIotI2S_BUF_SIZE );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    lRetVal = iot_i2s_read_sync( xI2SHandle, uxRxbuf, 0 );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    /* read/write must be in increments of 4. */
    lRetVal = iot_i2s_read_sync( xI2SHandle, uxRxbuf, 1 );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test fuzzing of read async api
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SReadAsyncFuzzing )
{
    IotI2SHandle_t xI2SHandle;
    int32_t lRetVal;
    uint8_t uxRxbuf[ testIotI2S_BUF_SIZE ];

    /* Open i2s to initialize hardware */
    xI2SHandle = iot_i2s_open( ltestIotI2sInputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SHandle );

    lRetVal = iot_i2s_read_async( NULL, uxRxbuf, testIotI2S_BUF_SIZE );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    lRetVal = iot_i2s_read_async( xI2SHandle, NULL, testIotI2S_BUF_SIZE );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    lRetVal = iot_i2s_read_async( xI2SHandle, uxRxbuf, 0 );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    /* read/write must be in increments of 4. */
    lRetVal = iot_i2s_read_async( xI2SHandle, uxRxbuf, 1 );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test fuzzing of write sync api
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SWriteSyncFuzzing )
{
    IotI2SHandle_t xI2SHandle;
    int32_t lRetVal;
    uint8_t uxTxbuf[ testIotI2S_BUF_SIZE ];

    /* Open i2s to initialize hardware */
    xI2SHandle = iot_i2s_open( ltestIotI2sOutputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SHandle );

    lRetVal = iot_i2s_write_sync( NULL, uxTxbuf, testIotI2S_BUF_SIZE );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    lRetVal = iot_i2s_write_sync( xI2SHandle, NULL, testIotI2S_BUF_SIZE );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    lRetVal = iot_i2s_write_sync( xI2SHandle, uxTxbuf, 0 );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    /* read/write must be in increments of 4. */
    lRetVal = iot_i2s_write_sync( xI2SHandle, uxTxbuf, 1 );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test fuzzing of write async api
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SWriteAsyncFuzzing )
{
    IotI2SHandle_t xI2SHandle;
    int32_t lRetVal;
    uint8_t uxTxbuf[ testIotI2S_BUF_SIZE ];

    /* Open i2s to initialize hardware */
    xI2SHandle = iot_i2s_open( ltestIotI2sOutputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SHandle );

    lRetVal = iot_i2s_write_async( NULL, uxTxbuf, testIotI2S_BUF_SIZE );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    lRetVal = iot_i2s_write_async( xI2SHandle, NULL, testIotI2S_BUF_SIZE );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    lRetVal = iot_i2s_write_async( xI2SHandle, uxTxbuf, 0 );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    /* read/write must be in increments of 4. */
    lRetVal = iot_i2s_write_async( xI2SHandle, uxTxbuf, 1 );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test fuzzing of cancel api
 */
TEST( TEST_IOT_I2S, AFQP_IotI2SCancelFuzzing )
{
    IotI2SHandle_t xI2SHandle;
    int32_t lRetVal;

    /* Open i2s to initialize hardware */
    xI2SHandle = iot_i2s_open( ltestIotI2sInputInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2SHandle );

    lRetVal = iot_i2s_cancel( NULL );
    TEST_ASSERT_EQUAL( IOT_I2S_INVALID_VALUE, lRetVal );

    /* Close instance */
    lRetVal = iot_i2s_close( xI2SHandle );
    TEST_ASSERT_EQUAL( IOT_I2S_SUCCESS, lRetVal );
}
