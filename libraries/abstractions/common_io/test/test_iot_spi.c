/*
 * FreeRTOS Common IO V0.1.1
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
 * @file test_iot_spi.c
 * @brief Functional Unit Test - SPI
 *******************************************************************************
 */

/* Test includes */
#include <string.h>

/* Test includes */
#include "unity.h"
#include "unity_fixture.h"

/* Driver includes */
#include "iot_spi.h"
#include "test_iot_internal.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "semphr.h"

#define SPI_BUFFER_SIZE                       ( 32 )
#define testIotSPI_DEFAULT_SEMAPHORE_DELAY    ( 3000U )

/*-----------------------------------------------------------*/

/* Globals values which can be overwritten by the test
 * framework invoking these tests */
/*-----------------------------------------------------------*/
uint32_t ultestIotSpiInstance = 0;                                /* Test SPI Instance */
uint32_t ultestIotSPIFrequency = 500000U;                         /* Test SPI Frequency */
uint32_t ultestIotSPIDummyValue = 0;                              /* Test SPI Dummy Value */
IotSPIMode_t xtestIotSPIDefaultConfigMode = eSPIMode0;            /* Default SPI eSPIMode0 */
IotSPIBitOrder_t xtestIotSPIDefaultconfigBitOrder = eSPIMSBFirst; /* Default SPI bit order eSPIMSBFirst */
static SemaphoreHandle_t xtestIotSPISemaphore = NULL;
static StaticSemaphore_t xtestIotSPICompleted;
uint32_t ulAssistedTestIotSpiSlave = 0;
uint32_t ultestIotSpiSlave = 0;


/*-----------------------------------------------------------*/

/**
 * @brief Application/POSIX defined callback for asynchronous operations
 * This callback function releases a semaphore every time it is called.
 */
static void prvSpiAsyncCallback( IotSPITransactionStatus_t xStatus,
                                 void * pvUserContext )
{
    BaseType_t xHigherPriorityTaskWoken;

    xSemaphoreGiveFromISR( xtestIotSPISemaphore, &xHigherPriorityTaskWoken );
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/* Define Test Group. */
TEST_GROUP( TEST_IOT_SPI );
/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_SPI )
{
    xtestIotSPISemaphore = xSemaphoreCreateBinaryStatic( &xtestIotSPICompleted );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotSPISemaphore );
}
/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_SPI )
{
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_SPI )
{
    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_OpenClose );
    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_Init );
    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_CancelFail );
    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_CancelSuccess );

    #if ( IOT_TEST_COMMON_IO_SPI_ASSISTED == 1 )
        RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_ReadSync );
        RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_ReadAsync );
        RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_WriteSync );
        RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_WriteAsync );
        RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_TransferSync );
        RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_TransferAsync );
    #endif

    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_OpenFuzzing );
    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_CloseFuzzing );
    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_IoctlFuzzing );
    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_ReadSyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_ReadAsyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_WriteSyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_WriteAsyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_TransferSyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_TransferAsyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_CancelFuzzing );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi open and close
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_OpenClose )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi init
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_Init )
{
    IotSPIHandle_t xSPIHandle;
    IotSPIMode_t xMode;
    IotSPIMasterConfig_t xNewConfig, xOrigConfig, xConfirmConfig;
    int32_t lRetVal;

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        memcpy( &xNewConfig, &xOrigConfig, sizeof( IotSPIMasterConfig_t ) );

        for( xMode = eSPIMode0; xMode <= eSPIMode3; xMode++ )
        {
            xNewConfig.eMode = xMode;
            lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xNewConfig );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

            lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xConfirmConfig );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
            TEST_ASSERT_EQUAL( xConfirmConfig.eMode, xNewConfig.eMode );
        }

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi read sync
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_ReadSync )
{
    IotSPIHandle_t xSPIHandle;
    IotSPIMasterConfig_t xOrigConfig, xTestConfig;
    int32_t lRetVal;
    int32_t lLoop = 0;
    uint8_t ucRxBuf[ 4 ] = { 0xff, 0xff, 0xff, 0xff };
    size_t xBytesRx;

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    /* save original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    if( TEST_PROTECT() )
    {
        /* configure bus */
        xTestConfig.ulFreq = ultestIotSPIFrequency;
        xTestConfig.eMode = xtestIotSPIDefaultConfigMode;
        xTestConfig.eSetBitOrder = xtestIotSPIDefaultconfigBitOrder;
        xTestConfig.ucDummyValue = ultestIotSPIDummyValue;

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xTestConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        lRetVal = iot_spi_read_sync( xSPIHandle, ucRxBuf, 4 );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetRxNoOfbytes, &xBytesRx );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( xBytesRx, 4 );

        /* Expect slave to send odd numbers on succesful master read */
        for( lLoop = 0; lLoop < 4; lLoop++ )
        {
            TEST_ASSERT_EQUAL( ucRxBuf[ lLoop ], ( ( lLoop * 2 ) + 1 ) );
        }
    }

    /* restore original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Assisted Test Function to test spi read sync
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_ReadSyncAssisted )
{
    IotSPIHandle_t xSPIHandle;
    IotSPIMasterConfig_t xOrigConfig, xTestConfig;
    int32_t lRetVal;
    int32_t lLoop = 0;
    uint8_t ucRxBuf[ 16 ] = { 0 };
    char cMsg[ 50 ] = { 0 };
    size_t xBytesRx;

    /* If unit test and assisted test have different spi slave, make sure select slave function in
     * ll hal is defined and select the slave for assisted slave. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef ASSISTED_TEST_COMMON_IO_SPI_SLAVE_SELECET_SUPPORTED
            lRetVal = iot_spi_select_slave( ultestIotSpiInstance, ulAssistedTestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #else
            TEST_ASSERT_MESSAGE( 0, "Assisted test has a different salve, but slave select is not supported." );
        #endif
    }

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    /* save original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    if( TEST_PROTECT() )
    {
        /* configure bus */
        xTestConfig.ulFreq = ultestIotSPIFrequency;
        xTestConfig.eMode = xtestIotSPIDefaultConfigMode;
        xTestConfig.eSetBitOrder = xtestIotSPIDefaultconfigBitOrder;
        xTestConfig.ucDummyValue = ultestIotSPIDummyValue;

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xTestConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        lRetVal = iot_spi_read_sync( xSPIHandle, ucRxBuf, sizeof( ucRxBuf ) );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetRxNoOfbytes, &xBytesRx );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( xBytesRx, 4 );
    }

    /* restore original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* restore slave select */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        lRetVal = iot_spi_select_slave( ultestIotSpiInstance, ulAssistedTestIotSpiSlave );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

    /* Create a string with read bytes and print it to console. */
    for( int i = 0, j = 0, len = sizeof( ucRxBuf ); i < len; i++ )
    {
        cMsg[ j++ ] = ',';
        uint8_t upp = ucRxBuf[ i ] >> 4, low = ucRxBuf[ i ] & 0xF;
        cMsg[ j++ ] = upp + ( upp > 9 ? 'A' - 10 : '0' );
        cMsg[ j++ ] = low + ( low > 9 ? 'A' - 10 : '0' );
    }

    TEST_IGNORE_MESSAGE( cMsg );
}

/**
 * @brief Test Function to test spi read async
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_ReadAsync )
{
    IotSPIHandle_t xSPIHandle;
    IotSPIMasterConfig_t xOrigConfig, xTestConfig;
    int32_t lRetVal;
    uint8_t ucRxBuf[ 4 ] = { 0xff, 0xff, 0xff, 0xff };
    size_t xBytesRx;
    int32_t lLoop = 0;

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    /* save original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    if( TEST_PROTECT() )
    {
        /* configure bus */
        xTestConfig.ulFreq = ultestIotSPIFrequency;
        xTestConfig.eMode = xtestIotSPIDefaultConfigMode;
        xTestConfig.eSetBitOrder = xtestIotSPIDefaultconfigBitOrder;
        xTestConfig.ucDummyValue = ultestIotSPIDummyValue;

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xTestConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        /* Set a callback for async call */
        iot_spi_set_callback( xSPIHandle, prvSpiAsyncCallback, NULL );

        /* Make async call */
        lRetVal = iot_spi_read_async( xSPIHandle, ucRxBuf, 4 );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        /*Wait for the callback. */
        lRetVal = xSemaphoreTake( xtestIotSPISemaphore, testIotSPI_DEFAULT_SEMAPHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetRxNoOfbytes, &xBytesRx );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( xBytesRx, 4 );

        /* Expect slave to send odd numbers on succesful master read */
        for( lLoop = 0; lLoop < 4; lLoop++ )
        {
            TEST_ASSERT_EQUAL( ucRxBuf[ lLoop ], ( ( lLoop * 2 ) + 1 ) );
        }
    }

    /* restore original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Assisted Test Function to test spi read async
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_ReadAsyncAssisted )
{
    IotSPIHandle_t xSPIHandle;
    IotSPIMasterConfig_t xOrigConfig, xTestConfig;
    int32_t lRetVal;
    uint8_t ucRxBuf[ 16 ] = { 0 };
    char cMsg[ 50 ] = { 0 };
    size_t xBytesRx;
    int32_t lLoop = 0;

    /* If unit test and assisted test have different spi slave, make sure select slave function in
     * ll hal is defined and select the slave for assisted slave. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef ASSISTED_TEST_COMMON_IO_SPI_SLAVE_SELECET_SUPPORTED
            lRetVal = iot_spi_select_slave( ultestIotSpiInstance, ulAssistedTestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #else
            TEST_ASSERT_MESSAGE( 0, "Assisted test has a different salve, but slave select is not supported." );
        #endif
    }

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    /* save original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    if( TEST_PROTECT() )
    {
        /* configure bus */
        xTestConfig.ulFreq = ultestIotSPIFrequency;
        xTestConfig.eMode = xtestIotSPIDefaultConfigMode;
        xTestConfig.eSetBitOrder = xtestIotSPIDefaultconfigBitOrder;
        xTestConfig.ucDummyValue = ultestIotSPIDummyValue;

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xTestConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        /* Set a callback for async call */
        iot_spi_set_callback( xSPIHandle, prvSpiAsyncCallback, NULL );

        /* Make async call */
        lRetVal = iot_spi_read_async( xSPIHandle, ucRxBuf, sizeof( ucRxBuf ) );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        /*Wait for the callback. */
        lRetVal = xSemaphoreTake( xtestIotSPISemaphore, testIotSPI_DEFAULT_SEMAPHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetRxNoOfbytes, &xBytesRx );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( xBytesRx, 4 );
    }

    /* restore original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* restore slave select */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        lRetVal = iot_spi_select_slave( ultestIotSpiInstance, ulAssistedTestIotSpiSlave );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

    /* Create a string with read bytes and print it to console. */
    for( int i = 0, j = 0, len = sizeof( ucRxBuf ); i < len; i++ )
    {
        cMsg[ j++ ] = ',';
        uint8_t upp = ucRxBuf[ i ] >> 4, low = ucRxBuf[ i ] & 0xF;
        cMsg[ j++ ] = upp + ( upp > 9 ? 'A' - 10 : '0' );
        cMsg[ j++ ] = low + ( low > 9 ? 'A' - 10 : '0' );
    }

    TEST_IGNORE_MESSAGE( cMsg );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi write sync
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_WriteSync )
{
    IotSPIHandle_t xSPIHandle;
    IotSPIMasterConfig_t xOrigConfig, xTestConfig;
    int32_t lRetVal;
    uint8_t ucTxBuf[ 4 ] = { 0, 2, 4, 6 };
    size_t xBytesTx;

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    /* save original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    if( TEST_PROTECT() )
    {
        /* configure bus */
        xTestConfig.ulFreq = ultestIotSPIFrequency;
        xTestConfig.eMode = xtestIotSPIDefaultConfigMode;
        xTestConfig.eSetBitOrder = xtestIotSPIDefaultconfigBitOrder;
        xTestConfig.ucDummyValue = ultestIotSPIDummyValue;

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xTestConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        lRetVal = iot_spi_write_sync( xSPIHandle, ucTxBuf, 4 );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetTxNoOfbytes, &xBytesTx );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( xBytesTx, 4 );
    }

    /* restore original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}

/**
 * @brief Assisted Test Function to test spi write sync
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_WriteSyncAssisted )
{
    IotSPIHandle_t xSPIHandle;
    IotSPIMasterConfig_t xOrigConfig, xTestConfig;
    int32_t lRetVal;
    uint8_t ucTxBuf[ 16 ] = { 0 };
    char cMsg[ 50 ] = { 0 };
    size_t xBytesTx;

    srand( xTaskGetTickCount() );

    /* Generate random tx bytes and load them to a string to print later. */
    for( int i = 0, j = 0, len = sizeof( ucTxBuf ); i < len; i++ )
    {
        ucTxBuf[ i ] = ( uint8_t ) rand();
        cMsg[ j++ ] = ',';
        uint8_t upp = ucTxBuf[ i ] >> 4, low = ucTxBuf[ i ] & 0xF;
        cMsg[ j++ ] = upp + ( upp > 9 ? 'A' - 10 : '0' );
        cMsg[ j++ ] = low + ( low > 9 ? 'A' - 10 : '0' );
    }

    /* If unit test and assisted test have different spi slave, make sure select slave function in
     * ll hal is defined and select the slave for assisted slave. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef ASSISTED_TEST_COMMON_IO_SPI_SLAVE_SELECET_SUPPORTED
            lRetVal = iot_spi_select_slave( ultestIotSpiInstance, ulAssistedTestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #else
            TEST_ASSERT_MESSAGE( 0, "Assisted test has a different salve, but slave select is not supported." );
        #endif
    }

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    /* save original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    if( TEST_PROTECT() )
    {
        /* configure bus */
        xTestConfig.ulFreq = ultestIotSPIFrequency;
        xTestConfig.eMode = xtestIotSPIDefaultConfigMode;
        xTestConfig.eSetBitOrder = xtestIotSPIDefaultconfigBitOrder;
        xTestConfig.ucDummyValue = ultestIotSPIDummyValue;
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xTestConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        lRetVal = iot_spi_write_sync( xSPIHandle, ucTxBuf, sizeof( ucTxBuf ) );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetTxNoOfbytes, &xBytesTx );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( xBytesTx, 4 );
    }

    /* restore original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* restore slave select */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        lRetVal = iot_spi_select_slave( ultestIotSpiInstance, ulAssistedTestIotSpiSlave );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

    TEST_IGNORE_MESSAGE( cMsg );
}

/**
 * @brief Test Function to test spi write async
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_WriteAsync )
{
    IotSPIHandle_t xSPIHandle;
    IotSPIMasterConfig_t xOrigConfig, xTestConfig;
    int32_t lRetVal;
    uint8_t ucTxBuf[ 4 ] = { 0, 2, 4, 6 };
    size_t xBytesTx;

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    /* save original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    if( TEST_PROTECT() )
    {
        /* configure bus */
        xTestConfig.ulFreq = ultestIotSPIFrequency;
        xTestConfig.eMode = xtestIotSPIDefaultConfigMode;
        xTestConfig.eSetBitOrder = xtestIotSPIDefaultconfigBitOrder;
        xTestConfig.ucDummyValue = ultestIotSPIDummyValue;

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xTestConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        /* Set a callback for async call */
        iot_spi_set_callback( xSPIHandle, prvSpiAsyncCallback, NULL );

        /* Make async write call */
        lRetVal = iot_spi_write_async( xSPIHandle, ucTxBuf, 4 );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        /*Wait for the callback. */
        lRetVal = xSemaphoreTake( xtestIotSPISemaphore, testIotSPI_DEFAULT_SEMAPHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetTxNoOfbytes, &xBytesTx );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( xBytesTx, 4 );
    }

    /* restore original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}

/**
 * @brief Assisted Test Function to test spi write async
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_WriteAsyncAssisted )
{
    IotSPIHandle_t xSPIHandle;
    IotSPIMasterConfig_t xOrigConfig, xTestConfig;
    int32_t lRetVal;
    uint8_t ucTxBuf[ 16 ] = { 0 };
    char cMsg[ 50 ] = { 0 };
    size_t xBytesTx;

    srand( xTaskGetTickCount() );

    /* Generate random tx bytes and load them to a string to print later. */
    for( int i = 0, j = 0, len = sizeof( ucTxBuf ); i < len; i++ )
    {
        ucTxBuf[ i ] = ( uint8_t ) rand();
        cMsg[ j++ ] = ',';
        uint8_t upp = ucTxBuf[ i ] >> 4, low = ucTxBuf[ i ] & 0xF;
        cMsg[ j++ ] = upp + ( upp > 9 ? 'A' - 10 : '0' );
        cMsg[ j++ ] = low + ( low > 9 ? 'A' - 10 : '0' );
    }

    /* If unit test and assisted test have different spi slave, make sure select slave function in
     * ll hal is defined and select the slave for assisted slave. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef ASSISTED_TEST_COMMON_IO_SPI_SLAVE_SELECET_SUPPORTED
            lRetVal = iot_spi_select_slave( ultestIotSpiInstance, ulAssistedTestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #else
            TEST_ASSERT_MESSAGE( 0, "Assisted test has a different salve, but slave select is not supported." );
        #endif
    }

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    /* save original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    if( TEST_PROTECT() )
    {
        /* configure bus */
        xTestConfig.ulFreq = ultestIotSPIFrequency;
        xTestConfig.eMode = xtestIotSPIDefaultConfigMode;
        xTestConfig.eSetBitOrder = xtestIotSPIDefaultconfigBitOrder;
        xTestConfig.ucDummyValue = ultestIotSPIDummyValue;

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xTestConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        /* Set a callback for async call */
        iot_spi_set_callback( xSPIHandle, prvSpiAsyncCallback, NULL );

        /* Make async write call */
        lRetVal = iot_spi_write_async( xSPIHandle, ucTxBuf, sizeof( ucTxBuf ) );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        /*Wait for the callback. */
        lRetVal = xSemaphoreTake( xtestIotSPISemaphore, testIotSPI_DEFAULT_SEMAPHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetTxNoOfbytes, &xBytesTx );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( xBytesTx, 4 );
    }

    /* restore original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* restore slave select */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        lRetVal = iot_spi_select_slave( ultestIotSpiInstance, ulAssistedTestIotSpiSlave );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

    TEST_IGNORE_MESSAGE( cMsg );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi transfer sync
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_TransferSync )
{
    IotSPIHandle_t xSPIHandle;
    IotSPIMasterConfig_t xOrigConfig, xTestConfig;
    int32_t lRetVal;
    uint8_t ucRxBuf[ 4 ] = { 0xff, 0xff, 0xff, 0xff };
    uint8_t ucTxBuf[ 4 ] = { 0x00, 0x02, 0x04, 0x06 };
    int32_t lLoop = 0;
    size_t xBytesTx, xBytesRx;

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    /* save original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    if( TEST_PROTECT() )
    {
        /* configure bus */
        xTestConfig.ulFreq = ultestIotSPIFrequency;
        xTestConfig.eMode = xtestIotSPIDefaultConfigMode;
        xTestConfig.eSetBitOrder = xtestIotSPIDefaultconfigBitOrder;
        xTestConfig.ucDummyValue = ultestIotSPIDummyValue;
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xTestConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        /* use some data to transfer.  Send even, expect odd */
        lRetVal = iot_spi_transfer_sync( xSPIHandle, ucTxBuf, ucRxBuf, 4 );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetRxNoOfbytes, &xBytesRx );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( 4, xBytesRx );

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetTxNoOfbytes, &xBytesTx );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( 4, xBytesTx );

        /* Expect slave to send odd numbers on succesful master read */
        for( lLoop = 0; lLoop < 4; lLoop++ )
        {
            TEST_ASSERT_EQUAL( ucRxBuf[ lLoop ], ( ( lLoop * 2 ) + 1 ) );
        }
    }

    /* restore original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Assisted Test Function to test spi transfer sync
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_TransferSyncAssisted )
{
    IotSPIHandle_t xSPIHandle;
    IotSPIMasterConfig_t xOrigConfig, xTestConfig;
    int32_t lRetVal;
    uint8_t ucRxBuf[ 8 ] = { 0 };
    uint8_t ucTxBuf[ 8 ] = { 0 };
    char cMsg[ 50 ] = { 0 };
    int32_t lLoop = 0;
    size_t xBytesTx, xBytesRx;

    srand( xTaskGetTickCount() );

    /* Generate random tx bytes and load them to a string to print later. */
    for( int i = 0, len = sizeof( ucTxBuf ); i < len; i++ )
    {
        ucTxBuf[ i ] = ( uint8_t ) rand();
        cMsg[ lLoop++ ] = ',';
        uint8_t upp = ucTxBuf[ i ] >> 4, low = ucTxBuf[ i ] & 0xF;
        cMsg[ lLoop++ ] = upp + ( upp > 9 ? 'A' - 10 : '0' );
        cMsg[ lLoop++ ] = low + ( low > 9 ? 'A' - 10 : '0' );
    }

    /* If unit test and assisted test have different spi slave, make sure select slave function in
     * ll hal is defined and select the slave for assisted slave. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef ASSISTED_TEST_COMMON_IO_SPI_SLAVE_SELECET_SUPPORTED
            lRetVal = iot_spi_select_slave( ultestIotSpiInstance, ulAssistedTestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #else
            TEST_ASSERT_MESSAGE( 0, "Assisted test has a different salve, but slave select is not supported." );
        #endif
    }

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    /* save original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    if( TEST_PROTECT() )
    {
        /* configure bus */
        xTestConfig.ulFreq = ultestIotSPIFrequency;
        xTestConfig.eMode = xtestIotSPIDefaultConfigMode;
        xTestConfig.eSetBitOrder = xtestIotSPIDefaultconfigBitOrder;
        xTestConfig.ucDummyValue = ultestIotSPIDummyValue;
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xTestConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        /* use some data to transfer.  Send even, expect odd */
        lRetVal = iot_spi_transfer_sync( xSPIHandle, ucTxBuf, ucRxBuf, sizeof( ucRxBuf ) );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetRxNoOfbytes, &xBytesRx );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( 4, xBytesRx );

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetTxNoOfbytes, &xBytesTx );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( 4, xBytesTx );
    }

    /* restore original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* restore slave select */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        lRetVal = iot_spi_select_slave( ultestIotSpiInstance, ulAssistedTestIotSpiSlave );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

    /* Append read bytes to string. */
    for( int i = 0, len = sizeof( ucRxBuf ); i < len; i++ )
    {
        cMsg[ lLoop++ ] = ',';
        uint8_t upp = ucRxBuf[ i ] >> 4, low = ucRxBuf[ i ] & 0xF;
        cMsg[ lLoop++ ] = upp + ( upp > 9 ? 'A' - 10 : '0' );
        cMsg[ lLoop++ ] = low + ( low > 9 ? 'A' - 10 : '0' );
    }

    TEST_IGNORE_MESSAGE( cMsg );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi transfer async
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_TransferAsync )
{
    IotSPIHandle_t xSPIHandle;
    IotSPIMasterConfig_t xOrigConfig, xTestConfig;
    int32_t lRetVal;
    uint8_t ucRxBuf[ 4 ] = { 0xff, 0xff, 0xff, 0xff };
    uint8_t ucTxBuf[ 4 ] = { 0x00, 0x02, 0x04, 0x06 };
    int32_t lLoop = 0;

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    /* save original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    if( TEST_PROTECT() )
    {
        /* configure bus */
        xTestConfig.ulFreq = ultestIotSPIFrequency;
        xTestConfig.eMode = xtestIotSPIDefaultConfigMode;
        xTestConfig.eSetBitOrder = xtestIotSPIDefaultconfigBitOrder;
        xTestConfig.ucDummyValue = ultestIotSPIDummyValue;

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xTestConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        /* Set a callback for async call */
        iot_spi_set_callback( xSPIHandle, prvSpiAsyncCallback, NULL );

        /* use some data to transfer.  Send even, expect odd */
        lRetVal = iot_spi_transfer_async( xSPIHandle, ucTxBuf, ucRxBuf, 4 );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        /*Wait for the callback. */
        lRetVal = xSemaphoreTake( xtestIotSPISemaphore, testIotSPI_DEFAULT_SEMAPHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        /* Expect slave to send odd numbers on succesful master read */
        for( lLoop = 0; lLoop < 4; lLoop++ )
        {
            TEST_ASSERT_EQUAL( ucRxBuf[ lLoop ], ( ( lLoop * 2 ) + 1 ) );
        }
    }

    /* restore original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Assisted Test Function to test spi transfer async
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_TransferAsyncAssisted )
{
    IotSPIHandle_t xSPIHandle;
    IotSPIMasterConfig_t xOrigConfig, xTestConfig;
    int32_t lRetVal;
    uint8_t ucRxBuf[ 8 ] = { 0 };
    uint8_t ucTxBuf[ 8 ] = { 0 };
    char cMsg[ 50 ] = { 0 };
    int32_t lLoop = 0;

    srand( xTaskGetTickCount() );

    /* Generate random tx bytes and load them to a string to print later. */
    for( int i = 0, len = sizeof( ucTxBuf ); i < len; i++ )
    {
        ucTxBuf[ i ] = ( uint8_t ) rand();
        cMsg[ lLoop++ ] = ',';
        uint8_t upp = ucTxBuf[ i ] >> 4, low = ucTxBuf[ i ] & 0xF;
        cMsg[ lLoop++ ] = upp + ( upp > 9 ? 'A' - 10 : '0' );
        cMsg[ lLoop++ ] = low + ( low > 9 ? 'A' - 10 : '0' );
    }

    /* If unit test and assisted test have different spi slave, make sure select slave function in
     * ll hal is defined and select the slave for assisted slave. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef ASSISTED_TEST_COMMON_IO_SPI_SLAVE_SELECET_SUPPORTED
            lRetVal = iot_spi_select_slave( ultestIotSpiInstance, ulAssistedTestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #else
            TEST_ASSERT_MESSAGE( 0, "Assisted test has a different salve, but slave select is not supported." );
        #endif
    }

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    /* save original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    if( TEST_PROTECT() )
    {
        /* configure bus */
        xTestConfig.ulFreq = ultestIotSPIFrequency;
        xTestConfig.eMode = xtestIotSPIDefaultConfigMode;
        xTestConfig.eSetBitOrder = xtestIotSPIDefaultconfigBitOrder;
        xTestConfig.ucDummyValue = ultestIotSPIDummyValue;

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xTestConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        /* Set a callback for async call */
        iot_spi_set_callback( xSPIHandle, prvSpiAsyncCallback, NULL );

        /* use some data to transfer.  Send even, expect odd */
        lRetVal = iot_spi_transfer_async( xSPIHandle, ucTxBuf, ucRxBuf, sizeof( ucRxBuf ) );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        /*Wait for the callback. */
        lRetVal = xSemaphoreTake( xtestIotSPISemaphore, testIotSPI_DEFAULT_SEMAPHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
    }

    /* restore original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* restore slave select */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        lRetVal = iot_spi_select_slave( ultestIotSpiInstance, ulAssistedTestIotSpiSlave );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

    /* Append read bytes to string. */
    for( int i = 0, len = sizeof( ucRxBuf ); i < len; i++ )
    {
        cMsg[ lLoop++ ] = ',';
        uint8_t upp = ucRxBuf[ i ] >> 4, low = ucRxBuf[ i ] & 0xF;
        cMsg[ lLoop++ ] = upp + ( upp > 9 ? 'A' - 10 : '0' );
        cMsg[ lLoop++ ] = low + ( low > 9 ? 'A' - 10 : '0' );
    }

    TEST_IGNORE_MESSAGE( cMsg );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi cancel with nothing to cancel
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_CancelFail )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_spi_cancel( xSPIHandle );

        if( lRetVal != IOT_SPI_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_SPI_NOTHING_TO_CANCEL, lRetVal );
        }
    }

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi cancel
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_CancelSuccess )
{
    IotSPIHandle_t xSPIHandle;
    IotSPIMasterConfig_t xOrigConfig, xTestConfig;
    int32_t lRetVal;
    uint8_t ucRxBuf[ 4 ] = { 0xff, 0xff, 0xff, 0xff };
    uint8_t ucTxBuf[ 4 ] = { 0x00, 0x02, 0x04, 0x06 };
    BaseType_t xCallbackReturn;

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    /* save original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    if( TEST_PROTECT() )
    {
        /* configure bus */
        xTestConfig.ulFreq = ultestIotSPIFrequency;
        xTestConfig.eMode = xtestIotSPIDefaultConfigMode;
        xTestConfig.eSetBitOrder = xtestIotSPIDefaultconfigBitOrder;
        xTestConfig.ucDummyValue = ultestIotSPIDummyValue;

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xTestConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        /* use some data to transfer.  Send even, expect odd */
        lRetVal = iot_spi_transfer_async( xSPIHandle, ucTxBuf, ucRxBuf, 4 );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        lRetVal = iot_spi_cancel( xSPIHandle );

        if( lRetVal != IOT_SPI_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        }

        /* Wait to make sure operation was really canceled. */
        xCallbackReturn = xSemaphoreTake( xtestIotSPISemaphore, testIotSPI_DEFAULT_SEMAPHORE_DELAY );
        TEST_ASSERT_EQUAL( pdFALSE, xCallbackReturn );
    }

    /* restore original configuration */
    lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi open with invalid params
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_OpenFuzzing )
{
    IotSPIHandle_t xSPIHandle, xSPIHandle2;
    int32_t lRetVal;

    /* Open invalid SPI handle */
    xSPIHandle = iot_spi_open( -1 );
    TEST_ASSERT_EQUAL( NULL, xSPIHandle );

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    /* Opening the same SPI is not allowed */
    xSPIHandle2 = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_EQUAL( NULL, xSPIHandle2 );

    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi close with invalid params
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_CloseFuzzing )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;

    /* Close with NULL handle */
    lRetVal = iot_spi_close( NULL );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    /* Close with valid handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Close with stale handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi ioctl with invalid params
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_IoctlFuzzing )
{
    IotSPIHandle_t xSPIHandle;
    IotSPIMode_t xMode;
    IotSPIBitOrder_t xBitOrder;
    IotSPIMasterConfig_t xNewConfig, xOrigConfig, xConfirmConfig;
    IotSPIIoctlRequest_t xRequest;
    int32_t lRetVal;

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    memcpy( &xNewConfig, &xOrigConfig, sizeof( IotSPIMasterConfig_t ) );

    for( xMode = eSPIMode0; xMode <= eSPIMode3; xMode++ )
    {
        xNewConfig.eMode = xMode;
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xNewConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xConfirmConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( xConfirmConfig.eMode, xNewConfig.eMode );
    }

    xNewConfig.eMode = xOrigConfig.eMode;

    for( xBitOrder = eSPIMSBFirst; xBitOrder <= eSPILSBFirst; xBitOrder++ )
    {
        xNewConfig.eSetBitOrder = xBitOrder;
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xNewConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xConfirmConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( xConfirmConfig.eSetBitOrder, xNewConfig.eSetBitOrder );
    }

    lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Try with invalid params.  Expect failures. */
    lRetVal = iot_spi_ioctl( NULL, eSPISetMasterConfig, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    lRetVal = iot_spi_ioctl( xSPIHandle, -1, &xOrigConfig );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    for( xRequest = eSPISetMasterConfig; xRequest <= eSPIGetRxNoOfbytes; xRequest++ )
    {
        lRetVal = iot_spi_ioctl( xSPIHandle, xRequest, NULL );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );
    }

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi readSync with invalid params
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_ReadSyncFuzzing )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;
    uint8_t ucBuffer[ SPI_BUFFER_SIZE ];

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    lRetVal = iot_spi_read_sync( NULL, ucBuffer, SPI_BUFFER_SIZE );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    lRetVal = iot_spi_read_sync( xSPIHandle, NULL, SPI_BUFFER_SIZE );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    lRetVal = iot_spi_read_sync( xSPIHandle, ucBuffer, 0 );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi readAsync with invalid params
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_ReadAsyncFuzzing )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;
    uint8_t ucBuffer[ SPI_BUFFER_SIZE ];

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    lRetVal = iot_spi_read_async( NULL, ucBuffer, SPI_BUFFER_SIZE );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    lRetVal = iot_spi_read_async( xSPIHandle, NULL, SPI_BUFFER_SIZE );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    lRetVal = iot_spi_read_async( xSPIHandle, ucBuffer, 0 );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi writeSync with invalid params
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_WriteSyncFuzzing )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;
    uint8_t ucBuffer[ SPI_BUFFER_SIZE ];

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    lRetVal = iot_spi_write_sync( NULL, ucBuffer, SPI_BUFFER_SIZE );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    lRetVal = iot_spi_write_sync( xSPIHandle, NULL, SPI_BUFFER_SIZE );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    lRetVal = iot_spi_write_sync( xSPIHandle, ucBuffer, 0 );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi writeAsync with invalid params
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_WriteAsyncFuzzing )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;
    uint8_t ucBuffer[ SPI_BUFFER_SIZE ];

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    lRetVal = iot_spi_write_async( NULL, ucBuffer, SPI_BUFFER_SIZE );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    lRetVal = iot_spi_write_async( xSPIHandle, NULL, SPI_BUFFER_SIZE );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    lRetVal = iot_spi_write_async( xSPIHandle, ucBuffer, 0 );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi transferSync with invalid params
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_TransferSyncFuzzing )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;
    uint8_t ucRxBuffer[ SPI_BUFFER_SIZE ];
    uint8_t ucTxBuffer[ SPI_BUFFER_SIZE ];

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    lRetVal = iot_spi_transfer_sync( NULL, ucRxBuffer, ucTxBuffer, SPI_BUFFER_SIZE );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    lRetVal = iot_spi_transfer_sync( xSPIHandle, NULL, ucTxBuffer, SPI_BUFFER_SIZE );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    lRetVal = iot_spi_transfer_sync( xSPIHandle, ucRxBuffer, NULL, SPI_BUFFER_SIZE );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    lRetVal = iot_spi_transfer_sync( xSPIHandle, ucRxBuffer, ucTxBuffer, 0 );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi transferAsync with invalid params
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_TransferAsyncFuzzing )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;
    uint8_t ucRxBuffer[ SPI_BUFFER_SIZE ];
    uint8_t ucTxBuffer[ SPI_BUFFER_SIZE ];

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    lRetVal = iot_spi_transfer_async( NULL, ucRxBuffer, ucTxBuffer, SPI_BUFFER_SIZE );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    lRetVal = iot_spi_transfer_async( xSPIHandle, NULL, ucTxBuffer, SPI_BUFFER_SIZE );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    lRetVal = iot_spi_transfer_async( xSPIHandle, ucRxBuffer, NULL, SPI_BUFFER_SIZE );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    lRetVal = iot_spi_transfer_async( xSPIHandle, ucRxBuffer, ucTxBuffer, 0 );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test spi cancel with invalid params
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_CancelFuzzing )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;

    /* Open SPI handle */
    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    lRetVal = iot_spi_cancel( NULL );

    if( lRetVal != IOT_SPI_FUNCTION_NOT_SUPPORTED )
    {
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );
    }

    lRetVal = iot_spi_cancel( xSPIHandle );

    if( ( lRetVal != IOT_SPI_FUNCTION_NOT_SUPPORTED ) &&
        ( lRetVal != IOT_SPI_INVALID_VALUE ) )
    {
        TEST_ASSERT_EQUAL( IOT_SPI_NOTHING_TO_CANCEL, lRetVal );
    }

    /* Close SPI handle */
    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/
