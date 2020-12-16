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
#include "iot_test_common_io_internal.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "semphr.h"

#define SPI_BUFFER_SIZE                       ( 32 )
#define testIotSPI_DEFAULT_SEMAPHORE_DELAY    ( 3000U )
#define _MESSAGE_LENGTH                       ( 50 )

/*-----------------------------------------------------------*/

/* Globals values which can be overwritten by the test
 * framework invoking these tests */
/*-----------------------------------------------------------*/
uint32_t ultestIotSpiInstance = 0;                                /* Test SPI Instance */
uint32_t ulAssistedTestIotSpiInstance = 0;                        /* Assisted Test SPI Instance */
uint32_t ultestIotSPIFrequency = 500000U;                         /* Test SPI Frequency */
uint32_t ultestIotSPIDummyValue = 0;                              /* Test SPI Dummy Value */
IotSPIMode_t xtestIotSPIDefaultConfigMode = eSPIMode0;            /* Default SPI eSPIMode0 */
IotSPIBitOrder_t xtestIotSPIDefaultconfigBitOrder = eSPIMSBFirst; /* Default SPI bit order eSPIMSBFirst */
static SemaphoreHandle_t xtestIotSPISemaphore = NULL;
static StaticSemaphore_t xtestIotSPICompleted;
uint32_t ulAssistedTestIotSpiSlave = 0;
uint32_t ultestIotSpiSlave = 0;


/*-----------------------------------------------------------*/
/* The message string output by assisted test. */
static char _cMsg[ _MESSAGE_LENGTH ] = { 0 };

/* Append bytes in the buffer to _cMsg, starting at given offset. */
static void prvAppendToMessage( size_t * pOffset,
                                uint8_t * pBuffer,
                                size_t bufferLen );
/* Output message _cMsg. */
static void prvOutputMessage();

/**
 * @brief Application/POSIX defined callback for asynchronous operations
 * This callback function releases a semaphore every time it is called.
 */
static void prvSpiAsyncCallback( IotSPITransactionStatus_t xStatus,
                                 void * pvUserContext )
{
    /* Disable unused parameter warning. */
    ( void ) xStatus;
    ( void ) pvUserContext;

    xSemaphoreGiveFromISR( xtestIotSPISemaphore, NULL );
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

    memset( _cMsg, 0, _MESSAGE_LENGTH );
}
/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_SPI )
{
}

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

    #if ( IOT_TEST_COMMON_IO_SPI_LOOPBACK == 1 )
        RUN_TEST_CASE( TEST_IOT_SPI, IotSPI_LoopBack_ReadSync );
        RUN_TEST_CASE( TEST_IOT_SPI, IotSPI_LoopBack_ReadAsync );
        RUN_TEST_CASE( TEST_IOT_SPI, IotSPI_LoopBack_WriteSync );
        RUN_TEST_CASE( TEST_IOT_SPI, IotSPI_LoopBack_WriteAsync );
        RUN_TEST_CASE( TEST_IOT_SPI, IotSPI_LoopBack_TransferSync );
        RUN_TEST_CASE( TEST_IOT_SPI, IotSPI_LoopBack_TransferAsync );
    #endif


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

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

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

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        /* Save the original configuration for later restore. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

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

        /* Restore the original configuration saved in the beginning of this test,
         * in order to reset to the original state before this test. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

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
    uint8_t ucRxBuf[ 16 ] = { 0 };
    size_t xBytesRx;
    size_t msgOffset = 0;

    /* If unit test and assisted test have different spi slave, make sure select slave function in
     * ll hal is defined and select the slave for assisted slave. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef IOT_TEST_COMMON_IO_SPI_SLAVE_SELECT_SUPPORTED
            lRetVal = iot_spi_select_slave( ulAssistedTestIotSpiInstance, ulAssistedTestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #else
            TEST_ASSERT_MESSAGE( 0, "Assisted test has a different salve, but slave select is not supported." );
        #endif
    }

    xSPIHandle = iot_spi_open( ulAssistedTestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        /* Save the original configuration for later restore. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

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
        TEST_ASSERT_EQUAL( xBytesRx, sizeof( ucRxBuf ) );

        /* Restore the original configuration saved in the beginning of this test,
         * in order to reset to the original state before this test. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Restore the original selected slave device, in order to reset to the original state before this test. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef IOT_TEST_COMMON_IO_SPI_SLAVE_SELECT_SUPPORTED
            lRetVal = iot_spi_select_slave( ulAssistedTestIotSpiInstance, ultestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #endif
    }

    prvAppendToMessage( &msgOffset, ucRxBuf, sizeof( ucRxBuf ) );
    prvOutputMessage();
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

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        /* Save the original configuration for later restore. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

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

        /* Restore the original configuration saved in the beginning of this test,
         * in order to reset to the original state before this test. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

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
    size_t xBytesRx;
    size_t msgOffset = 0;

    /* If unit test and assisted test have different spi slave, make sure select slave function in
     * ll hal is defined and select the slave for assisted slave. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef IOT_TEST_COMMON_IO_SPI_SLAVE_SELECT_SUPPORTED
            lRetVal = iot_spi_select_slave( ulAssistedTestIotSpiInstance, ulAssistedTestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #else
            TEST_ASSERT_MESSAGE( 0, "Assisted test has a different salve, but slave select is not supported." );
        #endif
    }

    xSPIHandle = iot_spi_open( ulAssistedTestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        /* Save the original configuration for later restore. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

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
        TEST_ASSERT_EQUAL( xBytesRx, sizeof( ucRxBuf ) );

        /* Restore the original configuration saved in the beginning of this test,
         * in order to reset to the original state before this test. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Restore the original selected slave device, in order to reset to the original state before this test. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef IOT_TEST_COMMON_IO_SPI_SLAVE_SELECT_SUPPORTED
            lRetVal = iot_spi_select_slave( ulAssistedTestIotSpiInstance, ultestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #endif
    }

    prvAppendToMessage( &msgOffset, ucRxBuf, sizeof( ucRxBuf ) );
    prvOutputMessage();
}

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

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        /* Save the original configuration for later restore. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

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

        /* Restore the original configuration saved in the beginning of this test,
         * in order to reset to the original state before this test. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

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
    size_t xBytesTx;
    size_t i = 0;
    size_t msgOffset = 0;

    srand( xTaskGetTickCount() );

    /* Generate random tx bytes. */
    for( ; i < sizeof( ucTxBuf ); i++ )
    {
        ucTxBuf[ i ] = ( uint8_t ) rand();
    }

    prvAppendToMessage( &msgOffset, ucTxBuf, sizeof( ucTxBuf ) );

    /* If unit test and assisted test have different spi slave, make sure select slave function in
     * ll hal is defined and select the slave for assisted slave. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef IOT_TEST_COMMON_IO_SPI_SLAVE_SELECT_SUPPORTED
            lRetVal = iot_spi_select_slave( ulAssistedTestIotSpiInstance, ulAssistedTestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #else
            TEST_ASSERT_MESSAGE( 0, "Assisted test has a different salve, but slave select is not supported." );
        #endif
    }

    xSPIHandle = iot_spi_open( ulAssistedTestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        /* Save the original configuration for later restore. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

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
        TEST_ASSERT_EQUAL( xBytesTx, sizeof( ucTxBuf ) );

        /* Restore the original configuration saved in the beginning of this test,
         * in order to reset to the original state before this test. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Restore the original selected slave device, in order to reset to the original state before this test. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef IOT_TEST_COMMON_IO_SPI_SLAVE_SELECT_SUPPORTED
            lRetVal = iot_spi_select_slave( ulAssistedTestIotSpiInstance, ultestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #endif
    }

    prvOutputMessage();
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

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        /* Save the original configuration for later restore. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

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

        /* Restore the original configuration saved in the beginning of this test,
         * in order to reset to the original state before this test. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
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

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        /* Save the original configuration for later restore. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

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

        /* Restore the original configuration saved in the beginning of this test,
         * in order to reset to the original state before this test. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

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
    size_t xBytesTx, xBytesRx;
    size_t i = 0;
    size_t msgOffset = 0;

    srand( xTaskGetTickCount() );

    /* Generate random tx bytes. */
    for( ; i < sizeof( ucTxBuf ); i++ )
    {
        ucTxBuf[ i ] = ( uint8_t ) rand();
    }

    prvAppendToMessage( &msgOffset, ucTxBuf, sizeof( ucTxBuf ) );

    /* If unit test and assisted test have different spi slave, make sure select slave function in
     * ll hal is defined and select the slave for assisted slave. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef IOT_TEST_COMMON_IO_SPI_SLAVE_SELECT_SUPPORTED
            lRetVal = iot_spi_select_slave( ulAssistedTestIotSpiInstance, ulAssistedTestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #else
            TEST_ASSERT_MESSAGE( 0, "Assisted test has a different salve, but slave select is not supported." );
        #endif
    }

    xSPIHandle = iot_spi_open( ulAssistedTestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        /* Save the original configuration for later restore. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

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
        TEST_ASSERT_EQUAL( sizeof( ucRxBuf ), xBytesRx );

        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetTxNoOfbytes, &xBytesTx );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( sizeof( ucTxBuf ), xBytesTx );

        /* Restore the original configuration saved in the beginning of this test,
         * in order to reset to the original state before this test. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Restore the original selected slave device, in order to reset to the original state before this test. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef IOT_TEST_COMMON_IO_SPI_SLAVE_SELECT_SUPPORTED
            lRetVal = iot_spi_select_slave( ulAssistedTestIotSpiInstance, ultestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #endif
    }

    prvAppendToMessage( &msgOffset, ucRxBuf, sizeof( ucRxBuf ) );
    prvOutputMessage();
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

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        /* Save the original configuration for later restore. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

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

        /* Restore the original configuration saved in the beginning of this test,
         * in order to reset to the original state before this test. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

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
    size_t i = 0;
    size_t msgOffset = 0;

    srand( xTaskGetTickCount() );

    /* Generate random tx bytes. */
    for( ; i < sizeof( ucTxBuf ); i++ )
    {
        ucTxBuf[ i ] = ( uint8_t ) rand();
    }

    prvAppendToMessage( &msgOffset, ucTxBuf, sizeof( ucTxBuf ) );

    /* If unit test and assisted test have different spi slave, make sure select slave function in
     * ll hal is defined and select the slave for assisted slave. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef IOT_TEST_COMMON_IO_SPI_SLAVE_SELECT_SUPPORTED
            lRetVal = iot_spi_select_slave( ulAssistedTestIotSpiInstance, ulAssistedTestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #else
            TEST_ASSERT_MESSAGE( 0, "Assisted test has a different salve, but slave select is not supported." );
        #endif
    }

    xSPIHandle = iot_spi_open( ulAssistedTestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        /* Save the original configuration for later restore. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

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

        /* Restore the original configuration saved in the beginning of this test,
         * in order to reset to the original state before this test. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Restore the original selected slave device, in order to reset to the original state before this test. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef IOT_TEST_COMMON_IO_SPI_SLAVE_SELECT_SUPPORTED
            lRetVal = iot_spi_select_slave( ulAssistedTestIotSpiInstance, ultestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #endif
    }

    prvAppendToMessage( &msgOffset, ucRxBuf, sizeof( ucRxBuf ) );
    prvOutputMessage();
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
    size_t xBytesTx;
    size_t i = 0;
    size_t msgOffset = 0;

    srand( xTaskGetTickCount() );

    /* Generate random tx bytes. */
    for( ; i < sizeof( ucTxBuf ); i++ )
    {
        ucTxBuf[ i ] = ( uint8_t ) rand();
    }

    prvAppendToMessage( &msgOffset, ucTxBuf, sizeof( ucTxBuf ) );

    /* If unit test and assisted test have different spi slave, make sure select slave function in
     * ll hal is defined and select the slave for assisted slave. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef IOT_TEST_COMMON_IO_SPI_SLAVE_SELECT_SUPPORTED
            lRetVal = iot_spi_select_slave( ulAssistedTestIotSpiInstance, ulAssistedTestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #else
            TEST_ASSERT_MESSAGE( 0, "Assisted test has a different salve, but slave select is not supported." );
        #endif
    }

    xSPIHandle = iot_spi_open( ulAssistedTestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        /* Save the original configuration for later restore. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

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
        TEST_ASSERT_EQUAL( xBytesTx, sizeof( ucTxBuf ) );

        /* Restore the original configuration saved in the beginning of this test,
         * in order to reset to the original state before this test. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

    /* Restore the original selected slave device, in order to reset to the original state before this test. */
    if( ulAssistedTestIotSpiSlave != ultestIotSpiSlave )
    {
        #ifdef IOT_TEST_COMMON_IO_SPI_SLAVE_SELECT_SUPPORTED
            lRetVal = iot_spi_select_slave( ulAssistedTestIotSpiInstance, ultestIotSpiSlave );
            TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
        #endif
    }

    prvOutputMessage();
}

/**
 * @brief Test Function to test spi cancel with nothing to cancel
 *
 */
TEST( TEST_IOT_SPI, AFQP_IotSPI_CancelFail )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;

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

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        /** Save the original configuration for later restore. * / */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );

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

        /* Restore the original configuration saved in the beginning of this test,
         * in order to reset to the original state before this test. */
        lRetVal = iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xOrigConfig );
        TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
    }

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

    if( TEST_PROTECT() )
    {
        /* Opening the same SPI is not allowed */
        xSPIHandle2 = iot_spi_open( ultestIotSpiInstance );
        TEST_ASSERT_EQUAL( NULL, xSPIHandle2 );
    }

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

    lRetVal = iot_spi_close( NULL );
    TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

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
    }

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

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_spi_read_sync( NULL, ucBuffer, SPI_BUFFER_SIZE );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

        lRetVal = iot_spi_read_sync( xSPIHandle, NULL, SPI_BUFFER_SIZE );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

        lRetVal = iot_spi_read_sync( xSPIHandle, ucBuffer, 0 );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );
    }

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

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_spi_read_async( NULL, ucBuffer, SPI_BUFFER_SIZE );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

        lRetVal = iot_spi_read_async( xSPIHandle, NULL, SPI_BUFFER_SIZE );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

        lRetVal = iot_spi_read_async( xSPIHandle, ucBuffer, 0 );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );
    }

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

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_spi_write_sync( NULL, ucBuffer, SPI_BUFFER_SIZE );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

        lRetVal = iot_spi_write_sync( xSPIHandle, NULL, SPI_BUFFER_SIZE );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

        lRetVal = iot_spi_write_sync( xSPIHandle, ucBuffer, 0 );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );
    }

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

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_spi_write_async( NULL, ucBuffer, SPI_BUFFER_SIZE );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

        lRetVal = iot_spi_write_async( xSPIHandle, NULL, SPI_BUFFER_SIZE );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

        lRetVal = iot_spi_write_async( xSPIHandle, ucBuffer, 0 );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );
    }

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

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_spi_transfer_sync( NULL, ucRxBuffer, ucTxBuffer, SPI_BUFFER_SIZE );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

        lRetVal = iot_spi_transfer_sync( xSPIHandle, NULL, ucTxBuffer, SPI_BUFFER_SIZE );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

        lRetVal = iot_spi_transfer_sync( xSPIHandle, ucRxBuffer, NULL, SPI_BUFFER_SIZE );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

        lRetVal = iot_spi_transfer_sync( xSPIHandle, ucRxBuffer, ucTxBuffer, 0 );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );
    }

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

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_spi_transfer_async( NULL, ucRxBuffer, ucTxBuffer, SPI_BUFFER_SIZE );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

        lRetVal = iot_spi_transfer_async( xSPIHandle, NULL, ucTxBuffer, SPI_BUFFER_SIZE );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

        lRetVal = iot_spi_transfer_async( xSPIHandle, ucRxBuffer, NULL, SPI_BUFFER_SIZE );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );

        lRetVal = iot_spi_transfer_async( xSPIHandle, ucRxBuffer, ucTxBuffer, 0 );
        TEST_ASSERT_EQUAL( IOT_SPI_INVALID_VALUE, lRetVal );
    }

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

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
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
    }

    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}
/*-----------------------------------------------------------*/

/**
 * @brief Assumes HW loopback where MOSI --> MISO.
 *        Verify the happy-path of a syncronous read
 *
 */
TEST( TEST_IOT_SPI, IotSPI_LoopBack_ReadSync )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;
    IotSPIMasterConfig_t xConfig;
    const uint32_t ulNBytes = 16;
    uint8_t pucRxBuf[ ulNBytes ];

    memset( pucRxBuf, 0, ulNBytes );

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        /* Adjust the dummy value */
        TEST_ASSERT( IOT_SPI_SUCCESS == iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xConfig ) );
        xConfig.ucDummyValue = 0xDC;
        TEST_ASSERT( IOT_SPI_SUCCESS == iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xConfig ) );

        /* Install callback which should not be exercised because it's sync */
        iot_spi_set_callback( xSPIHandle, prvSpiAsyncCallback, NULL );

        /* The callback should not be called to give sem back. Read, verify callback not called */
        TEST_ASSERT( IOT_SPI_SUCCESS == iot_spi_read_sync( xSPIHandle, pucRxBuf, ulNBytes ) );
        TEST_ASSERT( pdFAIL == xSemaphoreTake( xtestIotSPISemaphore, testIotSPI_DEFAULT_SEMAPHORE_DELAY / 2 ) );

        /* Verify expected bytes. Because it's loopback, the configured dummy value
         * should be transmitted, and thus read, for all N queried bytes.*/
        TEST_ASSERT_EACH_EQUAL_UINT8( xConfig.ucDummyValue, pucRxBuf, ulNBytes );
    }

    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Assumes HW loopback where MOSI --> MISO.
 *        Verify the happy-path of a async read
 *
 */
TEST( TEST_IOT_SPI, IotSPI_LoopBack_ReadAsync )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;
    IotSPIMasterConfig_t xConfig;
    const uint32_t ulNBytes = 16;
    uint8_t pucRxBuf[ ulNBytes ];

    memset( pucRxBuf, 0, ulNBytes );

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        /* Adjust the dummy value */
        TEST_ASSERT( IOT_SPI_SUCCESS == iot_spi_ioctl( xSPIHandle, eSPIGetMasterConfig, &xConfig ) );
        xConfig.ucDummyValue = 0xDC;
        TEST_ASSERT( IOT_SPI_SUCCESS == iot_spi_ioctl( xSPIHandle, eSPISetMasterConfig, &xConfig ) );

        iot_spi_set_callback( xSPIHandle, prvSpiAsyncCallback, NULL );

        /* Read, then verify callback is called */
        TEST_ASSERT( IOT_SPI_SUCCESS == iot_spi_read_async( xSPIHandle, pucRxBuf, ulNBytes ) );
        TEST_ASSERT( pdPASS == xSemaphoreTake( xtestIotSPISemaphore, testIotSPI_DEFAULT_SEMAPHORE_DELAY ) );

        /* Verify expected bytes. Because it's loopback, the configured dummy value
         * should be transmitted, and thus read, for all N queried bytes.*/
        TEST_ASSERT_EACH_EQUAL_UINT8( xConfig.ucDummyValue, pucRxBuf, ulNBytes );
    }

    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}


/*-----------------------------------------------------------*/

/**
 * @brief Assumes HW loopback where MOSI --> MISO.
 *        Verify the happy-path of a syncronous write
 *
 */
TEST( TEST_IOT_SPI, IotSPI_LoopBack_WriteSync )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;
    IotSPIMasterConfig_t xConfig;
    char pcTxBuf[] = "This is a test message";

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        iot_spi_set_callback( xSPIHandle, prvSpiAsyncCallback, NULL );

        /* Write, then verify callback was not called as it's sync */
        TEST_ASSERT( IOT_SPI_SUCCESS == iot_spi_write_sync( xSPIHandle, pcTxBuf, sizeof( pcTxBuf ) - 1 ) );
        TEST_ASSERT( pdFAIL == xSemaphoreTake( xtestIotSPISemaphore, testIotSPI_DEFAULT_SEMAPHORE_DELAY / 2 ) );
    }

    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Assumes HW loopback where MOSI --> MISO.
 *        Verify the happy-path of a asyncronous write
 *
 */
TEST( TEST_IOT_SPI, IotSPI_LoopBack_WriteAsync )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;
    IotSPIMasterConfig_t xConfig;
    char pcTxBuf[] = "This is a test message";

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        iot_spi_set_callback( xSPIHandle, prvSpiAsyncCallback, NULL );

        /* Write, then verify callback was called as it's async*/
        TEST_ASSERT( IOT_SPI_SUCCESS == iot_spi_write_async( xSPIHandle, pcTxBuf, sizeof( pcTxBuf ) - 1 ) );
        TEST_ASSERT( pdPASS == xSemaphoreTake( xtestIotSPISemaphore, testIotSPI_DEFAULT_SEMAPHORE_DELAY ) );
    }

    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Assumes HW loopback where MOSI --> MISO.
 *        Verify the happy-path of a syncronous transfer
 *
 */
TEST( TEST_IOT_SPI, IotSPI_LoopBack_TransferSync )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;
    IotSPIMasterConfig_t xConfig;
    char pcTxBuf[] = "This is a test message";
    char pcRxBuf[ sizeof( pcTxBuf ) ];

    memset( pcRxBuf, 0u, sizeof( pcRxBuf ) );

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        iot_spi_set_callback( xSPIHandle, prvSpiAsyncCallback, NULL );

        /* Write, then verify callback was not called as it's sync */
        TEST_ASSERT( IOT_SPI_SUCCESS == iot_spi_transfer_sync( xSPIHandle, pcTxBuf, pcRxBuf, sizeof( pcTxBuf ) ) );
        TEST_ASSERT( pdFAIL == xSemaphoreTake( xtestIotSPISemaphore, testIotSPI_DEFAULT_SEMAPHORE_DELAY ) );

        /* Data should also be the same, per loop back */
        TEST_ASSERT_EQUAL_UINT8_ARRAY( pcTxBuf, pcRxBuf, sizeof( pcTxBuf ) );
    }

    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Assumes HW loopback where MOSI --> MISO.
 *        Verify the happy-path of a asyncronous transfer
 *
 */
TEST( TEST_IOT_SPI, IotSPI_LoopBack_TransferAsync )
{
    IotSPIHandle_t xSPIHandle;
    int32_t lRetVal;
    IotSPIMasterConfig_t xConfig;
    char pcTxBuf[] = "This is a test message";
    char pcRxBuf[ sizeof( pcTxBuf ) ];

    memset( pcRxBuf, 0u, sizeof( pcRxBuf ) );

    xSPIHandle = iot_spi_open( ultestIotSpiInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xSPIHandle );

    if( TEST_PROTECT() )
    {
        iot_spi_set_callback( xSPIHandle, prvSpiAsyncCallback, NULL );

        /* Write, then verify callback was not called as it's sync */
        TEST_ASSERT( IOT_SPI_SUCCESS == iot_spi_transfer_async( xSPIHandle, pcTxBuf, pcRxBuf, sizeof( pcTxBuf ) ) );
        TEST_ASSERT( pdPASS == xSemaphoreTake( xtestIotSPISemaphore, testIotSPI_DEFAULT_SEMAPHORE_DELAY ) );

        /* Data should also be the same, per loop back */
        TEST_ASSERT_EQUAL_UINT8_ARRAY( pcTxBuf, pcRxBuf, sizeof( pcTxBuf ) );
    }

    lRetVal = iot_spi_close( xSPIHandle );
    TEST_ASSERT_EQUAL( IOT_SPI_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

static void prvAppendToMessage( size_t * pOffset,
                                uint8_t * pBuffer,
                                size_t bufferLen )
{
    size_t i = 0;
    size_t offset = *pOffset;

    for( ; i < bufferLen && offset + 2 < _MESSAGE_LENGTH; i++ )
    {
        _cMsg[ offset++ ] = ',';
        uint8_t upp = pBuffer[ i ] >> 4, low = pBuffer[ i ] & 0xF;
        _cMsg[ offset++ ] = upp + ( upp > 9 ? 'A' - 10 : '0' );
        _cMsg[ offset++ ] = low + ( low > 9 ? 'A' - 10 : '0' );
    }

    *pOffset = offset;
}

/*-----------------------------------------------------------*/

static void prvOutputMessage()
{
    TEST_IGNORE_MESSAGE( _cMsg );
}
