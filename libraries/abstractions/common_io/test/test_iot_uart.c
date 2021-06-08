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

/* Standard includes */
#include <string.h>

/* Test includes. */
#include "unity_fixture.h"
#include "unity.h"

/* Driver model includes */
#include "iot_uart.h"
#include "iot_test_common_io_internal.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "semphr.h"

/*-----------------------------------------------------------*/
/* Test Defines */
#define testIotUART_DEFAULT_SEMPAHORE_DELAY            ( 60 )                                                       /** Amount of time a function waits for every callback to finish. */
#define testIotUART_TEST_BAUD_RATE                     ( 38400 )                                                    /** Alternative baud rate to test. */
#define testIotUART_TEST_BAUD_RATE_DEFAULT             ( IOT_UART_BAUD_RATE_DEFAULT )                               /** Default baud rate. */
#define testIotUART_BAUD_RATE_STRING                   ( "Baudrate: 38400\n" )                                      /** Signal external device to change to different baudrate. */
#define testIotUART_BAUD_RATE_DEFAULT_STRING           ( "Baudrate: 115200\n" )                                     /** Signal external device to revert back to default baudrate. */
#define testIotUART_BAUD_RATE_BUFFER_TEST_LENGTH       ( sizeof( testIotUART_BAUD_RATE_STRING ) - 1 )
#define testIotUART_BAUD_RATE_BUFFER_DEFAULT_LENGTH    ( sizeof( testIotUART_BAUD_RATE_DEFAULT_STRING ) - 1 )
#define testIotUART_BUFFER_STRING                      ( "Test for UART read-write\n" )
#define testIotUART_BUFFER_LENGTH                      ( sizeof( testIotUART_BUFFER_STRING ) - 1 )

#define testIotUART_BAUD_RATE_FAST                     ( 115200 )
#define testIotUART_BAUD_RATE_SLOW                     ( 9600 )
#define testIotUARTBUFFERSIZE                          ( 32 )
#define testIotUART_BUFFER_LENGTH_LARGE                ( 200 )
#define testIotUART_DELAY                              ( 1000 )
#define testIotUART_7BIT_WORD_LENGTH                   ( 7 )
#define testIotUART_8BIT_WORD_LENGTH                   ( 8 )
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/* Globals values which can be overwritten by the test
 * framework invoking these tests */
/*-----------------------------------------------------------*/
uint8_t uctestIotUartPort = 0; /** Default UART port for testing */
uint32_t ultestIotUartFlowControl = 0;
uint32_t ultestIotUartParity = 0;
uint32_t ultestIotUartWordLength = 0;
uint32_t ultestIotUartStopBits = 0;

/*-----------------------------------------------------------*/
/* Static Globals */
/*-----------------------------------------------------------*/
static SemaphoreHandle_t xReadCompleteSemaphore = NULL;
static SemaphoreHandle_t xWriteCompleteSemaphore = NULL;
static StaticSemaphore_t xReadSemaphoreBuffer;
static StaticSemaphore_t xWriteSemaphoreBuffer;

/* note:  config1 is different in each field from config2 */
static IotUARTConfig_t xSampleConfig1 =
{
    .ulBaudrate    = testIotUART_BAUD_RATE_FAST,
    .ucFlowControl = 0, /* no flow control */
    .xParity       = eUartParityNone,
    .xStopbits     = eUartStopBitsOne,
    .ucWordlength  = testIotUART_7BIT_WORD_LENGTH,
};
static IotUARTConfig_t xSampleConfig2 =
{
    .ulBaudrate    = testIotUART_BAUD_RATE_SLOW,
    .ucFlowControl = 1, /* yes flow control */
    .xParity       = eUartParityOdd,
    .xStopbits     = eUartStopBitsTwo,
    .ucWordlength  = testIotUART_8BIT_WORD_LENGTH,
};


/**
 * @brief Application/POSIX defined callback for asynchronous write operation on
 * UART for testing the loopback function.
 * This callback function releases a semaphore every time it is called.
 */
static void prvReadWriteCallback( IotUARTOperationStatus_t xOpStatus,
                                  void * pvParams )
{
    /* Disable unused parameter warning. */
    ( void ) pvParams;

    if( xOpStatus == eUartReadCompleted )
    {
        xSemaphoreGiveFromISR( xReadCompleteSemaphore, NULL );
    }
    else if( xOpStatus == eUartWriteCompleted )
    {
        xSemaphoreGiveFromISR( xWriteCompleteSemaphore, NULL );
    }
}

/*-----------------------------------------------------------*/

/* Define Test Group. */
TEST_GROUP( TEST_IOT_UART );


/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_UART )
{
    /* xSemaphoreCreateBinaryStatic succeeds as long as buffer parameter is not NULL. */
    xReadCompleteSemaphore = xSemaphoreCreateBinaryStatic( &xReadSemaphoreBuffer );
    xWriteCompleteSemaphore = xSemaphoreCreateBinaryStatic( &xWriteSemaphoreBuffer );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_UART )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_UART )
{
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTWriteReadAsyncWithCallback );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTIoctlFuzzing );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTReadSyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTReadAsyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTWriteSyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTWriteAsyncFuzzing );
    #if ( IOT_TEST_COMMON_IO_UART_SUPPORTED_CANCEL == 1 )
        RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTCancel );
    #endif
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTIoctlWhenBusy )
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTWriteAsyncReadAsyncLoopbackTest );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTIoctlGetSet );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTOpenCloseCancelFuzzing );
}
/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/

/**
 * hardware loopback. The Tx and Rx pin on the vendor board are shorted with a
 * connector and the test is run by transmitting some bytes on write sync and checking
 * if same characters are read synchronously.
 */
TEST( TEST_IOT_UART, AFQP_AssistedIotUARTWriteReadSync )
{
    uint8_t cpBuffer[ testIotUARTBUFFERSIZE ] = { 0 };
    uint8_t cpBufferRead[ testIotUARTBUFFERSIZE ] = { 0 };

    memcpy( cpBuffer, testIotUART_BUFFER_STRING, testIotUART_BUFFER_LENGTH );
    IotUARTHandle_t xUartHandle;
    int32_t lRead, lWrite, lClose;

    xUartHandle = iot_uart_open( uctestIotUartPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xUartHandle );

    if( TEST_PROTECT() )
    {
        lWrite = iot_uart_write_sync( xUartHandle, cpBuffer, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

        lRead = iot_uart_read_sync( xUartHandle, cpBufferRead, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lRead );

        TEST_ASSERT_EQUAL( 0, strncmp( ( char * ) cpBuffer, ( char * ) cpBufferRead, testIotUART_BUFFER_LENGTH ) );
    }

    lClose = iot_uart_close( xUartHandle );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to check UART Ioctl API specifications.
 */
TEST( TEST_IOT_UART, AFQP_IotUARTIoctlGetSet )
{
    IotUARTHandle_t xUartHandle;
    int32_t lIoctl, lClose, lTransferAmount;
    IotUARTConfig_t xgetConfigBuffer, xConfigBuffer, xOriginalConfig;

    xUartHandle = iot_uart_open( uctestIotUartPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xUartHandle );

    if( TEST_PROTECT() )
    {
        /* Save original UART config */
        lIoctl = iot_uart_ioctl( xUartHandle, eUartGetConfig, &xOriginalConfig );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

        /* Initialze to a known value, then one by one change, and make sure it changes */
        memcpy( &xConfigBuffer, &xSampleConfig1, sizeof( IotUARTConfig_t ) );
        lIoctl = iot_uart_ioctl( xUartHandle, eUartSetConfig, &xConfigBuffer );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

        /* Test changing baudrate config */
        xConfigBuffer.ulBaudrate = xSampleConfig2.ulBaudrate;
        lIoctl = iot_uart_ioctl( xUartHandle, eUartSetConfig, &xConfigBuffer );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

        lIoctl = iot_uart_ioctl( xUartHandle, eUartGetConfig, &xgetConfigBuffer );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
        TEST_ASSERT_EQUAL( xSampleConfig2.ulBaudrate, xgetConfigBuffer.ulBaudrate );

        /* Test changing control flow config if specified by the test framework */
        if( ultestIotUartFlowControl )
        {
            xConfigBuffer.ucFlowControl = xSampleConfig2.ucFlowControl;
            lIoctl = iot_uart_ioctl( xUartHandle, eUartSetConfig, &xConfigBuffer );
            TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

            lIoctl = iot_uart_ioctl( xUartHandle, eUartGetConfig, &xgetConfigBuffer );
            TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
            TEST_ASSERT_EQUAL( xSampleConfig2.ucFlowControl, xgetConfigBuffer.ucFlowControl );
        }

        /* Test changing stop bit config */
        xConfigBuffer.xStopbits = xSampleConfig2.xStopbits;
        lIoctl = iot_uart_ioctl( xUartHandle, eUartSetConfig, &xConfigBuffer );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

        lIoctl = iot_uart_ioctl( xUartHandle, eUartGetConfig, &xgetConfigBuffer );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
        TEST_ASSERT_EQUAL( xSampleConfig2.xStopbits, xgetConfigBuffer.xStopbits );

        /* Test changing parity config */
        xConfigBuffer.xParity = xSampleConfig2.xParity;
        lIoctl = iot_uart_ioctl( xUartHandle, eUartSetConfig, &xConfigBuffer );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

        lIoctl = iot_uart_ioctl( xUartHandle, eUartGetConfig, &xgetConfigBuffer );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
        TEST_ASSERT_EQUAL( xSampleConfig2.xParity, xgetConfigBuffer.xParity );

        /* Test changing Wordlength config */
        xConfigBuffer.ucWordlength = xSampleConfig2.ucWordlength;
        lIoctl = iot_uart_ioctl( xUartHandle, eUartSetConfig, &xConfigBuffer );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

        lIoctl = iot_uart_ioctl( xUartHandle, eUartGetConfig, &xgetConfigBuffer );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
        TEST_ASSERT_EQUAL( xSampleConfig2.ucWordlength, xgetConfigBuffer.ucWordlength );

        /* Set config back to original */
        lIoctl = iot_uart_ioctl( xUartHandle, eUartSetConfig, &xOriginalConfig );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

        /* Getting transfer byte amount only works on an operation in flight. */
        lIoctl = iot_uart_ioctl( xUartHandle, eGetTxNoOfbytes, &lTransferAmount );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

        lIoctl = iot_uart_ioctl( xUartHandle, eGetRxNoOfbytes, &lTransferAmount );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
    }

    lClose = iot_uart_close( xUartHandle );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to check if UART can be configured to a different baudrate.
 *  Works by signal external device to change to a new baudrate. Sending and recieving a message.
 *  Then signals to return back to default baudrate.
 *-----------------------------------------------------------*/
TEST( TEST_IOT_UART, AFQP_AssistedIotUARTBaudChange )
{
    IotUARTHandle_t xUartHandle;
    int32_t lIoctl, lWrite, lRead, lClose;
    uint8_t cpBuffer[ testIotUARTBUFFERSIZE ] = { 0 };
    uint8_t cpBufferRead[ testIotUARTBUFFERSIZE ] = { 0 };

    memcpy( cpBuffer, testIotUART_BAUD_RATE_STRING, testIotUART_BAUD_RATE_BUFFER_TEST_LENGTH );

    xUartHandle = iot_uart_open( uctestIotUartPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xUartHandle );

    if( TEST_PROTECT() )
    {
        /* Signal other device to change to new baudrate */
        lWrite = iot_uart_write_sync( xUartHandle, cpBuffer, testIotUART_BAUD_RATE_BUFFER_TEST_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

        IotUARTConfig_t pvConfigBuffer;
        lIoctl = iot_uart_ioctl( xUartHandle, eUartGetConfig, &pvConfigBuffer );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

        pvConfigBuffer.ulBaudrate = testIotUART_TEST_BAUD_RATE;

        /* Configure a different baudrate */
        lIoctl = iot_uart_ioctl( xUartHandle, eUartSetConfig, &pvConfigBuffer );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

        /* Wait for other UART to change baudrate. */
        vTaskDelay( pdMS_TO_TICKS( testIotUART_DELAY ) );

        memset( cpBuffer, 0, testIotUARTBUFFERSIZE );
        memcpy( cpBuffer, testIotUART_BUFFER_STRING, testIotUART_BUFFER_LENGTH );

        lWrite = iot_uart_write_sync( xUartHandle, cpBuffer, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

        lRead = iot_uart_read_sync( xUartHandle, cpBufferRead, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lRead );

        TEST_ASSERT_EQUAL( 0, strncmp( ( char * ) cpBuffer, ( char * ) cpBufferRead, testIotUART_BUFFER_LENGTH ) );

        memcpy( cpBuffer, testIotUART_BAUD_RATE_DEFAULT_STRING, testIotUART_BAUD_RATE_BUFFER_DEFAULT_LENGTH );

        /* Signal other device to revert to original baudrate */
        lWrite = iot_uart_write_sync( xUartHandle, cpBuffer, testIotUART_BAUD_RATE_BUFFER_DEFAULT_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

        pvConfigBuffer.ulBaudrate = testIotUART_TEST_BAUD_RATE_DEFAULT;

        /* Reset baudrate back to default */
        lIoctl = iot_uart_ioctl( xUartHandle, eUartSetConfig, &pvConfigBuffer );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

        /* Wait for other UART to change baudrate. */
        vTaskDelay( pdMS_TO_TICKS( testIotUART_DELAY ) );

        memcpy( cpBuffer, testIotUART_BUFFER_STRING, testIotUART_BUFFER_LENGTH );

        lWrite = iot_uart_write_sync( xUartHandle, cpBuffer, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

        lRead = iot_uart_read_sync( xUartHandle, cpBufferRead, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lRead );

        /* Baudrate reverted succesfully */
        TEST_ASSERT_EQUAL( 0, strncmp( ( char * ) cpBuffer, ( char * ) cpBufferRead, testIotUART_BUFFER_LENGTH ) );
    }

    lClose = iot_uart_close( xUartHandle );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/**
 * @brief Test function to test the asynchronous read/write with UART by doing a
 * hardware loopback. The Tx and Rx pin on the vendor board are shorted with a
 * connector and the test is run by transmitting some bytes on write async and checking
 * if same characters are read asynchronously.
 * This test also checks if we are getting Tx bytes in flight.
 */
TEST( TEST_IOT_UART, AFQP_IotUARTWriteAsyncReadAsyncLoopbackTest )
{
    IotUARTHandle_t xUartHandle;
    int32_t lIoctl, lWrite, lClose, lRead, lTransferAmount;

    uint8_t cpBuffer[ testIotUART_BUFFER_LENGTH + 1 ] = { 0 };
    uint8_t cpBufferRead[ testIotUART_BUFFER_LENGTH + 1 ] = { 0 };
    uint8_t uStringCompare = 1;
    BaseType_t xCallbackReturn;

    memcpy( cpBuffer, testIotUART_BUFFER_STRING, testIotUART_BUFFER_LENGTH );

    xUartHandle = iot_uart_open( uctestIotUartPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xUartHandle );

    if( TEST_PROTECT() )
    {
        iot_uart_set_callback( xUartHandle, prvReadWriteCallback, NULL );

        lRead = iot_uart_read_async( xUartHandle, cpBufferRead, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lRead );

        lWrite = iot_uart_write_async( xUartHandle, cpBuffer, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

        xCallbackReturn = xSemaphoreTake( xWriteCompleteSemaphore, testIotUART_DEFAULT_SEMPAHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, xCallbackReturn );

        xCallbackReturn = xSemaphoreTake( xReadCompleteSemaphore, testIotUART_DEFAULT_SEMPAHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, xCallbackReturn );

        lIoctl = iot_uart_ioctl( xUartHandle, eGetTxNoOfbytes, &lTransferAmount );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
        TEST_ASSERT_GREATER_THAN( 0, lTransferAmount );

        uStringCompare = memcmp( cpBuffer, cpBufferRead, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( 0, uStringCompare );
    }

    lClose = iot_uart_close( xUartHandle );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/

/**
 * This is Assisted Test function to test the asynchronous read/write with UART by doing a
 * loopback with RPI3. The Tx and Rx pin on the vendor board are connected to RX and TX of
 * RPI3. Test is run by transmitting some bytes on write async and checking
 * if same characters are read asynchronously on RPI3. Validation happens on RPI3
 * This test also checks if we are getting Tx bytes in flight.
 */
TEST( TEST_IOT_UART, AFQP_AssistedIotUARTWriteAsync )
{
    IotUARTHandle_t xUartHandle;
    int32_t lIoctl, lWrite, lClose, lTransferAmount_1, lTransferAmount_2;
    uint8_t i;
    uint8_t cpBufferLarge[ testIotUART_BUFFER_LENGTH_LARGE ] = { 0 };

    for( i = 0; i < testIotUART_BUFFER_LENGTH_LARGE; i++ )
    {
        cpBufferLarge[ i ] = 0xAA;
    }

    cpBufferLarge[ testIotUART_BUFFER_LENGTH_LARGE - 1 ] = '\n';

    xUartHandle = iot_uart_open( uctestIotUartPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xUartHandle );

    if( TEST_PROTECT() )
    {
        lWrite = iot_uart_write_async( xUartHandle, cpBufferLarge, testIotUART_BUFFER_LENGTH_LARGE );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

        lIoctl = iot_uart_ioctl( xUartHandle, eGetTxNoOfbytes, &lTransferAmount_1 );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
        TEST_ASSERT_GREATER_THAN( 0, lTransferAmount_1 );

        /* Delay for 1 sec. */
        vTaskDelay( testIotUART_DELAY );

        lIoctl = iot_uart_ioctl( xUartHandle, eGetTxNoOfbytes, &lTransferAmount_2 );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
        TEST_ASSERT_GREATER_THAN( lTransferAmount_1, lTransferAmount_2 );

        /* Delay for 1 sec. */
        vTaskDelay( testIotUART_DELAY );
    }

    lClose = iot_uart_close( xUartHandle );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/**
 * @brief Test function to test the asynchronous read with UART by doing a
 * hardware loopback. The Tx and Rx pin on the vendor board are shorted with a
 * connector and the test is run by transmitting some bytes on write async and checking
 * if same characters are read aynchronously.
 */
TEST( TEST_IOT_UART, AFQP_IotUARTWriteReadAsyncWithCallback )
{
    IotUARTHandle_t xUartHandle;
    uint8_t cpBuffer[ testIotUARTBUFFERSIZE ] = { 0 };
    uint8_t cpBufferRead[ testIotUARTBUFFERSIZE ] = { 0 };

    memcpy( cpBuffer, testIotUART_BUFFER_STRING, testIotUART_BUFFER_LENGTH );
    int32_t lRead, lWrite, lClose;
    BaseType_t xCallbackReturn;
    uint8_t ucPort = uctestIotUartPort;

    xUartHandle = iot_uart_open( ucPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xUartHandle );

    if( TEST_PROTECT() )
    {
        iot_uart_set_callback( xUartHandle, prvReadWriteCallback, NULL );

        lRead = iot_uart_read_async( xUartHandle, cpBufferRead, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lRead );

        lWrite = iot_uart_write_async( xUartHandle, cpBuffer, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

        xCallbackReturn = xSemaphoreTake( xWriteCompleteSemaphore, testIotUART_DEFAULT_SEMPAHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, xCallbackReturn );

        xCallbackReturn = xSemaphoreTake( xReadCompleteSemaphore, testIotUART_DEFAULT_SEMPAHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, xCallbackReturn );

        TEST_ASSERT_EQUAL( 0, strncmp( ( char * ) cpBuffer, ( char * ) cpBufferRead, testIotUART_BUFFER_LENGTH ) );
    }

    lClose = iot_uart_close( xUartHandle );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_uart_cancel
 *
 *-----------------------------------------------------------*/
TEST( TEST_IOT_UART, AFQP_IotUARTCancel )
{
    IotUARTHandle_t xUartHandle;
    int32_t lWrite, lClose, lCancel;
    BaseType_t xCallbackReturn;

    uint8_t cSmallBuf[ 2 ] = { 'H', 'I' };
    uint8_t cpBuffer[ testIotUARTBUFFERSIZE ] = { 0 };
    uint8_t uSmallBuflen = sizeof( cSmallBuf ) / sizeof( uint8_t );

    memcpy( cpBuffer, testIotUART_BUFFER_STRING, testIotUART_BUFFER_LENGTH );

    xUartHandle = iot_uart_open( uctestIotUartPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xUartHandle );

    if( TEST_PROTECT() )
    {
        iot_uart_set_callback( xUartHandle, prvReadWriteCallback, NULL );

        lWrite = iot_uart_write_async( xUartHandle, cpBuffer, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

        lCancel = iot_uart_cancel( xUartHandle );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lCancel );

        /* Wait to make sure operation was really canceled. */
        xCallbackReturn = xSemaphoreTake( xWriteCompleteSemaphore, testIotUART_DEFAULT_SEMPAHORE_DELAY );
        TEST_ASSERT_EQUAL( pdFALSE, xCallbackReturn );

        lWrite = iot_uart_write_async( xUartHandle, cSmallBuf, uSmallBuflen );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

        /* Wait to make sure operation has completed. */
        xCallbackReturn = xSemaphoreTake( xWriteCompleteSemaphore, testIotUART_DEFAULT_SEMPAHORE_DELAY );
        TEST_ASSERT_EQUAL( pdTRUE, xCallbackReturn );

        /* Since  cSmallBuf is small buffer, write is already complete. Hence nothing to cancel */
        lCancel = iot_uart_cancel( xUartHandle );
        TEST_ASSERT_EQUAL( IOT_UART_NOTHING_TO_CANCEL, lCancel );
    }

    lClose = iot_uart_close( xUartHandle );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to fuzz iot_uart_write_async
 *-----------------------------------------------------------*/
TEST( TEST_IOT_UART, AFQP_IotUARTWriteAsyncFuzzing )
{
    IotUARTHandle_t xUartHandle;
    int32_t lClose, lWrite;

    uint8_t cpBuffer[ testIotUARTBUFFERSIZE ] = { 0 };

    memcpy( cpBuffer, testIotUART_BUFFER_STRING, testIotUART_BUFFER_LENGTH );

    lWrite = iot_uart_write_async( NULL, cpBuffer, testIotUART_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lWrite );

    xUartHandle = iot_uart_open( uctestIotUartPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xUartHandle );

    if( TEST_PROTECT() )
    {
        lWrite = iot_uart_write_async( xUartHandle, NULL, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lWrite );

        lWrite = iot_uart_write_async( xUartHandle, cpBuffer, 0 );
        TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lWrite );
    }

    lClose = iot_uart_close( xUartHandle );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to fuzz iot_uart_write_sync
 *-----------------------------------------------------------*/
TEST( TEST_IOT_UART, AFQP_IotUARTWriteSyncFuzzing )
{
    IotUARTHandle_t xUartHandle;
    int32_t lClose, lWrite;

    uint8_t cpBuffer[ testIotUARTBUFFERSIZE ] = { 0 };

    memcpy( cpBuffer, testIotUART_BUFFER_STRING, testIotUART_BUFFER_LENGTH );

    lWrite = iot_uart_write_sync( NULL, cpBuffer, testIotUART_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lWrite );

    xUartHandle = iot_uart_open( uctestIotUartPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xUartHandle );

    if( TEST_PROTECT() )
    {
        lWrite = iot_uart_write_sync( xUartHandle, NULL, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lWrite );

        lWrite = iot_uart_write_sync( xUartHandle, cpBuffer, 0 );
        TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lWrite );
    }

    lClose = iot_uart_close( xUartHandle );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to fuzz iot_uart_read_async
 *-----------------------------------------------------------*/
TEST( TEST_IOT_UART, AFQP_IotUARTReadAsyncFuzzing )
{
    IotUARTHandle_t xUartHandle;
    int32_t lClose, lRead;

    uint8_t cpBufferRead[ testIotUARTBUFFERSIZE ] = { 0 };

    lRead = iot_uart_read_async( NULL, cpBufferRead, testIotUART_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lRead );

    xUartHandle = iot_uart_open( uctestIotUartPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xUartHandle );

    if( TEST_PROTECT() )
    {
        lRead = iot_uart_read_async( xUartHandle, NULL, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lRead );

        lRead = iot_uart_read_async( xUartHandle, cpBufferRead, 0 );
        TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lRead );
    }

    lClose = iot_uart_close( xUartHandle );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to fuzz iot_uart_read_sync
 *-----------------------------------------------------------*/
TEST( TEST_IOT_UART, AFQP_IotUARTReadSyncFuzzing )
{
    IotUARTHandle_t xUartHandle;
    int32_t lClose, lRead;

    uint8_t cpBufferRead[ testIotUARTBUFFERSIZE ] = { 0 };

    lRead = iot_uart_read_sync( NULL, cpBufferRead, testIotUART_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lRead );

    xUartHandle = iot_uart_open( uctestIotUartPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xUartHandle );

    if( TEST_PROTECT() )
    {
        lRead = iot_uart_read_sync( xUartHandle, NULL, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lRead );

        lRead = iot_uart_read_sync( xUartHandle, cpBufferRead, 0 );
        TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lRead );
    }

    lClose = iot_uart_close( xUartHandle );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/* @brief Test Function to fuzz iot_uart_ioctl
 *-----------------------------------------------------------*/
TEST( TEST_IOT_UART, AFQP_IotUARTIoctlFuzzing )
{
    IotUARTHandle_t xUartHandle;
    int32_t lIoctl, lClose;
    IotUARTConfig_t xUartConfigTest;

    /* Call iot_uart_ioctl with NULL handle. Expect IOT_UART_INVALID_VALUE */
    lIoctl = iot_uart_ioctl( NULL, eUartSetConfig, &xUartConfigTest );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lIoctl );

    xUartHandle = iot_uart_open( uctestIotUartPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xUartHandle );

    if( TEST_PROTECT() )
    {
        /* Call iot_uart_ioctl with valid handle, but with invalid enum (-1) request.Expect IOT_UART_INVALID_VALUE */
        lIoctl = iot_uart_ioctl( xUartHandle, -1, &xUartConfigTest );
        TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lIoctl );

        /* Call iot_uart_ioctl with enum eUartSetConfig, and NULL buffer.Expect IOT_UART_INVALID_VALUE */
        lIoctl = iot_uart_ioctl( xUartHandle, eUartSetConfig, NULL );
        TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lIoctl );

        /* Call iot_uart_ioctl with enum eUartGetConfig, and NULL buffer.Expect IOT_UART_INVALID_VALUE */
        lIoctl = iot_uart_ioctl( xUartHandle, eUartGetConfig, NULL );
        TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lIoctl );

        /* Call iot_uart_ioctl with enum eGetTxNoOfbytes, and NULL buffer.Expect IOT_UART_INVALID_VALUE */
        lIoctl = iot_uart_ioctl( xUartHandle, eGetTxNoOfbytes, NULL );
        TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lIoctl );

        /* Call iot_uart_ioctl with enum eGetRxNoOfbytes, and NULL buffer.Expect IOT_UART_INVALID_VALUE */
        lIoctl = iot_uart_ioctl( xUartHandle, eGetRxNoOfbytes, NULL );
        TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lIoctl );
    }

    lClose = iot_uart_close( xUartHandle );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to check if UART can be configured when an operation is ongoing.
 * If any asynchronous operation is in the process in UART, iot_uart_ioctl should fail.
 */
TEST( TEST_IOT_UART, AFQP_IotUARTIoctlWhenBusy )
{
    IotUARTHandle_t xUartHandle;
    int32_t lIoctl, lWrite, lClose;
    uint8_t cpBuffer[ testIotUARTBUFFERSIZE ] = { 0 };

    memcpy( cpBuffer, testIotUART_BUFFER_STRING, testIotUART_BUFFER_LENGTH );
    IotUARTConfig_t xUartConfig = { .ulBaudrate = testIotUART_TEST_BAUD_RATE_DEFAULT };

    xUartHandle = iot_uart_open( uctestIotUartPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xUartHandle );

    if( TEST_PROTECT() )
    {
        lWrite = iot_uart_write_async( xUartHandle, cpBuffer, testIotUART_BUFFER_LENGTH );
        TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

        lIoctl = iot_uart_ioctl( xUartHandle, eUartSetConfig, &xUartConfig );
        TEST_ASSERT_EQUAL( IOT_UART_BUSY, lIoctl );
    }

    lClose = iot_uart_close( xUartHandle );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/

/* @brief Test Function to fuzz iot_uart_open and iot_uart_close
 *-----------------------------------------------------------*/
TEST( TEST_IOT_UART, AFQP_IotUARTOpenCloseCancelFuzzing )
{
    IotUARTHandle_t xUartHandle_1, xUartHandle_2, xUartHandle_3;
    int32_t lClose, lCancel;

    xUartHandle_1 = iot_uart_open( -1 );
    TEST_ASSERT_EQUAL( NULL, xUartHandle_1 );

    lClose = iot_uart_close( NULL );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lClose );

    xUartHandle_2 = iot_uart_open( uctestIotUartPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xUartHandle_2 );

    if( TEST_PROTECT() )
    {
        xUartHandle_3 = iot_uart_open( uctestIotUartPort );
        TEST_ASSERT_EQUAL( NULL, xUartHandle_3 );

        lCancel = iot_uart_cancel( NULL );
        TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lCancel );
    }

    lClose = iot_uart_close( xUartHandle_2 );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );

    lClose = iot_uart_close( xUartHandle_2 );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lClose );
}
/*-----------------------------------------------------------*/
