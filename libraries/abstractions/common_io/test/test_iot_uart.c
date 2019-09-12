/*
 * Amazon FreeRTOS V1.2.3
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
#define testIotUART_WRITE_BUFFER_STRING                ( "Test for write async\n" )
#define testIotUART_READ_BUFFER_STRING                 ( "Test for read async\n" )
#define testIotUART_WRITE_BUFFER_LENGTH                ( sizeof( testIotUART_WRITE_BUFFER_STRING ) - 1 )
#define testIotUART_READ_BUFFER_LENGTH                 ( sizeof( testIotUART_READ_BUFFER_STRING ) - 1 )
#define testBufferSize                                 32
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/* Globals values which can be overwritten by the test
 * framework invoking these tests */
/*-----------------------------------------------------------*/
uint8_t ustestIotUartPort = 0;         /** Default UART port for testing */
uint32_t ultestIotUartFlowControl = 0; /** Default UART flow control for testing */
uint32_t ultestIotUartParity = 0;      /** Default UART parity for testing */
uint32_t ultestIotUartWordLength = 0;  /** Default UART port for testing */
uint32_t ultestIotUartStopBits = 0;    /** Default UART stop bits for testing */

/*-----------------------------------------------------------*/
/* Static Globals */
/*-----------------------------------------------------------*/
static SemaphoreHandle_t xtestIotUARTSemaphore = NULL;
static StaticSemaphore_t xtestIotUARTCompleted;
static uint8_t sCancelled = 0; /* Flag to check if an operation was properly cancelled. */

/**
 * @brief Application/POSIX defined callback for asynchronous read/write operation on
 * UART for testing the loopback function.
 * This callback function releases a semaphore every time it is called.
 */
static void prvReadWriteCallback( IotUARTOperationStatus_t xOpStatus,
                                  void * pvParams )
{
    BaseType_t xHigherPriorityTaskWoken;

    TEST_ASSERT_EQUAL( ( xOpStatus ), eUartCompleted );
    TEST_ASSERT_EQUAL( pvParams, NULL );
    xSemaphoreGiveFromISR( xtestIotUARTSemaphore, &xHigherPriorityTaskWoken );
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
/*-----------------------------------------------------------*/

/**
 * @brief Application/POSIX defined callback for canceling an asynchronous operation.
 * This callback function should never be called.
 */
static void prvCancelCallback( IotUARTOperationStatus_t xOpStatus,
                               void * pvParams )
{
    BaseType_t xHigherPriorityTaskWoken;

    sCancelled = 1;
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
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
    xtestIotUARTSemaphore = xSemaphoreCreateCountingStatic( 10, 0, &xtestIotUARTCompleted );
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotUARTSemaphore );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUART_IoctlFuzzing );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUART_ReadSyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUART_ReadAsyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUART_WriteSyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUART_WriteAsyncFuzzing );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUART_OpenCloseCancelFuzzing );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTWriteReadSync );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTWriteReadAsyncWithCallback );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTIoctlWhenBusy );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTBaudChange );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTIoctlGetSet );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTIoctlGetRxTxBytes );
    RUN_TEST_CASE( TEST_IOT_UART, AFQP_IotUARTCancel );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test function to test the asynchronous read with UART by doing a
 * hardware loopback. The Tx and Rx pin on the vendor board are shorted with a
 * connector and the test is run by transmitting some bytes on write async and checking
 * if same characters are read aynchronously.
 */
TEST( TEST_IOT_UART, AFQP_IotUARTWriteReadAsyncWithCallback )
{
    uint8_t cpBuffer[ testBufferSize ] = { 0 };
    uint8_t cpBufferRead[ testBufferSize ] = { 0 };

    strncpy( ( char * ) cpBuffer, ( char * ) testIotUART_READ_BUFFER_STRING, testIotUART_READ_BUFFER_LENGTH );
    IotUARTHandle_t xOpen;
    int32_t lRead, lWrite, lClose;
    BaseType_t xCallbackReturn;
    uint8_t ucPort = ustestIotUartPort;

    xOpen = iot_uart_open( ucPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xOpen );

    iot_uart_set_callback( xOpen, prvReadWriteCallback, NULL );

    lWrite = iot_uart_write_async( xOpen, cpBuffer, testIotUART_READ_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

    xCallbackReturn = xSemaphoreTake( ( SemaphoreHandle_t ) &xtestIotUARTCompleted, testIotUART_DEFAULT_SEMPAHORE_DELAY );
    TEST_ASSERT_EQUAL( pdTRUE, xCallbackReturn );

    lRead = iot_uart_read_async( xOpen, cpBufferRead, testIotUART_READ_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lRead );

    xCallbackReturn = xSemaphoreTake( ( SemaphoreHandle_t ) &xtestIotUARTCompleted, testIotUART_DEFAULT_SEMPAHORE_DELAY );
    TEST_ASSERT_EQUAL( pdTRUE, xCallbackReturn );

    TEST_ASSERT_EQUAL( 0, strncmp( ( char * ) cpBuffer, ( char * ) cpBufferRead, testIotUART_READ_BUFFER_LENGTH ) );

    lClose = iot_uart_close( xOpen );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/**
 * hardware loopback. The Tx and Rx pin on the vendor board are shorted with a
 * connector and the test is run by transmitting some bytes on write sync and checking
 * if same characters are read synchronously.
 */
TEST( TEST_IOT_UART, AFQP_IotUARTWriteReadSync )
{
    uint8_t cpBuffer[ testBufferSize ] = { 0 };
    uint8_t cpBufferRead[ testBufferSize ] = { 0 };

    strncpy( ( char * ) cpBuffer, ( char * ) testIotUART_WRITE_BUFFER_STRING, testIotUART_WRITE_BUFFER_LENGTH );
    IotUARTHandle_t xOpen;
    int32_t lRead, lWrite, lClose;
    uint8_t ucPort = ustestIotUartPort;

    xOpen = iot_uart_open( ucPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xOpen );

    lWrite = iot_uart_write_sync( xOpen, cpBuffer, testIotUART_WRITE_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

    lRead = iot_uart_read_sync( xOpen, cpBufferRead, testIotUART_WRITE_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lRead );

    TEST_ASSERT_EQUAL( 0, strncmp( ( char * ) cpBuffer, ( char * ) cpBufferRead, testIotUART_WRITE_BUFFER_LENGTH ) );

    lClose = iot_uart_close( xOpen );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to check if UART can be configured when an operation is ongoing.
 * If any asynchronous operation is in the process in UART, iot_uart_ioctl should fail.
 */
TEST( TEST_IOT_UART, AFQP_IotUARTIoctlWhenBusy )
{
    IotUARTHandle_t xOpen;
    int32_t lIoctl, lWrite, lClose;
    BaseType_t xCallbackReturn;
    uint8_t cpBuffer[ testBufferSize ] = { 0 };
    uint8_t ucPort = ustestIotUartPort;

    strncpy( ( char * ) cpBuffer, ( char * ) testIotUART_WRITE_BUFFER_STRING, testIotUART_WRITE_BUFFER_LENGTH );
    IotUARTConfig_t xUartConfig = { .ulBaudrate = testIotUART_TEST_BAUD_RATE_DEFAULT };

    xOpen = iot_uart_open( ucPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xOpen );

    lWrite = iot_uart_write_async( xOpen, cpBuffer, testIotUART_WRITE_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

    lIoctl = iot_uart_ioctl( xOpen, eUartSetConfig, &xUartConfig );
    TEST_ASSERT_EQUAL( IOT_UART_BUSY, lIoctl );

    lClose = iot_uart_close( xOpen );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to check if UART can be configured to a different baudrate.
 *  Works by signal external device to change to a new baudrate. Sending and recieving a message.
 *  Then signals to return back to default baudrate.
 */
TEST( TEST_IOT_UART, AFQP_IotUARTBaudChange )
{
    IotUARTHandle_t xOpen;
    int32_t lIoctl, lWrite, lRead, lClose;
    uint8_t cpBuffer[ testBufferSize ] = { 0 };
    uint8_t cpBufferRead[ testBufferSize ] = { 0 };

    uint8_t ucPort = ustestIotUartPort;

    strncpy( ( char * ) cpBuffer, ( char * ) testIotUART_BAUD_RATE_STRING, testIotUART_BAUD_RATE_BUFFER_TEST_LENGTH );

    xOpen = iot_uart_open( ucPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xOpen );

    /* Signal other device to change to new baudrate */
    lWrite = iot_uart_write_sync( xOpen, cpBuffer, testIotUART_BAUD_RATE_BUFFER_TEST_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

    IotUARTConfig_t pvConfigBuffer;
    lIoctl = iot_uart_ioctl( xOpen, eUartGetConfig, &pvConfigBuffer );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

    pvConfigBuffer.ulBaudrate = testIotUART_TEST_BAUD_RATE;

    /* Configure a different baudrate */
    lIoctl = iot_uart_ioctl( xOpen, eUartSetConfig, &pvConfigBuffer );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

    /* Wait for other UART to change baudrate. */
    vTaskDelay( pdMS_TO_TICKS( 1000 ) );

    memset( cpBuffer, 0, testBufferSize );
    strncpy( ( char * ) cpBuffer, ( char * ) testIotUART_WRITE_BUFFER_STRING, testIotUART_WRITE_BUFFER_LENGTH );

    lWrite = iot_uart_write_sync( xOpen, cpBuffer, testIotUART_WRITE_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

    lRead = iot_uart_read_sync( xOpen, cpBufferRead, testIotUART_WRITE_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lRead );

    TEST_ASSERT_EQUAL( 0, strncmp( ( char * ) cpBuffer, ( char * ) cpBufferRead, testIotUART_WRITE_BUFFER_LENGTH ) );

    strncpy( ( char * ) cpBuffer, ( char * ) testIotUART_BAUD_RATE_DEFAULT_STRING, testIotUART_BAUD_RATE_BUFFER_DEFAULT_LENGTH );

    /* Signal other device to revert to original baudrate */
    lWrite = iot_uart_write_sync( xOpen, cpBuffer, testIotUART_BAUD_RATE_BUFFER_DEFAULT_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

    pvConfigBuffer.ulBaudrate = testIotUART_TEST_BAUD_RATE_DEFAULT;

    /* Reset baudrate back to default */
    lIoctl = iot_uart_ioctl( xOpen, eUartSetConfig, &pvConfigBuffer );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

    /* Wait for other UART to change baudrate. */
    vTaskDelay( pdMS_TO_TICKS( 1000 ) );

    strncpy( ( char * ) cpBuffer, ( char * ) testIotUART_WRITE_BUFFER_STRING, testIotUART_WRITE_BUFFER_LENGTH );

    lWrite = iot_uart_write_sync( xOpen, cpBuffer, testIotUART_WRITE_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

    lRead = iot_uart_read_sync( xOpen, cpBufferRead, testIotUART_WRITE_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lRead );

    /* Baudrate reverted succesfully */
    TEST_ASSERT_EQUAL( 0, strncmp( ( char * ) cpBuffer, ( char * ) cpBufferRead, testIotUART_WRITE_BUFFER_LENGTH ) );

    lClose = iot_uart_close( xOpen );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to check UART Ioctl API specifications.
 */
TEST( TEST_IOT_UART, AFQP_IotUARTIoctlGetSet )
{
    IotUARTHandle_t xOpen;
    int32_t lIoctl, lClose, lTransferAmount;
    IotUARTConfig_t pvConfigBuffer, pvOriginalConfig;
    uint8_t ucPort = ustestIotUartPort;


    xOpen = iot_uart_open( ucPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xOpen );

    /* Save original UART config */
    lIoctl = iot_uart_ioctl( xOpen, eUartGetConfig, &pvOriginalConfig );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

    lIoctl = iot_uart_ioctl( xOpen, eUartGetConfig, &pvConfigBuffer );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

    /* Test changing baudrate config */
    pvConfigBuffer.ulBaudrate = testIotUART_TEST_BAUD_RATE;
    lIoctl = iot_uart_ioctl( xOpen, eUartSetConfig, &pvConfigBuffer );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
    lIoctl = iot_uart_ioctl( xOpen, eUartGetConfig, &pvConfigBuffer );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
    TEST_ASSERT_EQUAL( testIotUART_TEST_BAUD_RATE, pvConfigBuffer.ulBaudrate );

    /* Test changing control flow config */
    pvConfigBuffer.ulFlowControl = ultestIotUartFlowControl;
    lIoctl = iot_uart_ioctl( xOpen, eUartSetConfig, &pvConfigBuffer );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
    lIoctl = iot_uart_ioctl( xOpen, eUartGetConfig, &pvConfigBuffer );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
    TEST_ASSERT_EQUAL( ultestIotUartFlowControl, pvConfigBuffer.ulFlowControl );

    /* Test changing stop bit config */
    pvConfigBuffer.ulStopbits = ultestIotUartStopBits;
    lIoctl = iot_uart_ioctl( xOpen, eUartSetConfig, &pvConfigBuffer );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
    lIoctl = iot_uart_ioctl( xOpen, eUartGetConfig, &pvConfigBuffer );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
    TEST_ASSERT_EQUAL( ultestIotUartStopBits, pvConfigBuffer.ulStopbits );

    /* Test changing stop bit config */
    pvConfigBuffer.ulWordlength = ultestIotUartWordLength;
    lIoctl = iot_uart_ioctl( xOpen, eUartSetConfig, &pvConfigBuffer );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
    lIoctl = iot_uart_ioctl( xOpen, eUartGetConfig, &pvConfigBuffer );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );
    TEST_ASSERT_EQUAL( ultestIotUartWordLength, pvConfigBuffer.ulWordlength );

    /* Set config back to original */
    lIoctl = iot_uart_ioctl( xOpen, eUartSetConfig, &pvOriginalConfig );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

    lIoctl = iot_uart_ioctl( xOpen, eGetTxNoOfbytes, &lTransferAmount );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

    lIoctl = iot_uart_ioctl( xOpen, eGetRxNoOfbytes, &lTransferAmount );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

    lClose = iot_uart_close( xOpen );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to check if getting Tx and Rx bytes in flight.
 */
TEST( TEST_IOT_UART, AFQP_IotUARTIoctlGetRxTxBytes )
{
    uint8_t cpBuffer[ testBufferSize ] = { 0 };

    strncpy( ( char * ) cpBuffer, ( char * ) testIotUART_WRITE_BUFFER_STRING, testIotUART_WRITE_BUFFER_LENGTH );
    IotUARTHandle_t xOpen;
    int32_t lWrite, lClose, lTransferAmount, lIoctl;
    BaseType_t xCallbackReturn;
    uint8_t ucPort = ustestIotUartPort;

    xOpen = iot_uart_open( ucPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xOpen );

    lWrite = iot_uart_write_async( xOpen, cpBuffer, testIotUART_WRITE_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

    lIoctl = iot_uart_ioctl( xOpen, eGetTxNoOfbytes, &lTransferAmount );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

    /* Need to assure that write operation finishes before calling read. */
    xCallbackReturn = xSemaphoreTake( ( SemaphoreHandle_t ) &xtestIotUARTCompleted, testIotUART_DEFAULT_SEMPAHORE_DELAY );
    TEST_ASSERT_EQUAL( pdTRUE, xCallbackReturn );

    lWrite = iot_uart_read_async( xOpen, cpBuffer, testIotUART_WRITE_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

    lIoctl = iot_uart_ioctl( xOpen, eGetRxNoOfbytes, &lTransferAmount );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lIoctl );

    lClose = iot_uart_close( xOpen );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to cancel an asynchronous operation in flight.
 */
TEST( TEST_IOT_UART, AFQP_IotUARTCancel )
{
    uint8_t cpBuffer[ testBufferSize ] = { 0 };

    IotUARTHandle_t xOpen;
    int32_t lWrite, lCancel, lClose;
    uint8_t ucPort = ustestIotUartPort;

    xOpen = iot_uart_open( ucPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xOpen );

    iot_uart_set_callback( xOpen, prvCancelCallback, NULL );

    lWrite = iot_uart_write_async( xOpen, cpBuffer, testIotUART_WRITE_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lWrite );

    lCancel = iot_uart_cancel( xOpen );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lCancel );

    /* Wait to make sure operation was really canceled. */
    vTaskDelay( pdMS_TO_TICKS( 1000 ) );

    TEST_ASSERT_EQUAL( 0, sCancelled );

    lClose = iot_uart_close( xOpen );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to fuzz iot_uart_write_async
 * /*-----------------------------------------------------------*/
TEST( TEST_IOT_UART, AFQP_IotUART_WriteAsyncFuzzing )
{
    IotUARTHandle_t xOpen;
    uint8_t ucPort = ustestIotUartPort;
    int32_t lClose, lWrite;

    uint8_t cpBuffer[ testBufferSize ] = { 0 };

    strncpy( cpBuffer, testIotUART_READ_BUFFER_STRING, testIotUART_READ_BUFFER_LENGTH );

    lWrite = iot_uart_write_async( NULL, cpBuffer, testIotUART_READ_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lWrite );

    xOpen = iot_uart_open( ucPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xOpen );

    lWrite = iot_uart_write_async( xOpen, NULL, testIotUART_READ_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lWrite );

    lWrite = iot_uart_write_async( xOpen, cpBuffer, 0 );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lWrite );

    lClose = iot_uart_close( xOpen );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to fuzz iot_uart_write_sync
 * /*-----------------------------------------------------------*/
TEST( TEST_IOT_UART, AFQP_IotUART_WriteSyncFuzzing )
{
    IotUARTHandle_t xOpen;
    uint8_t ucPort = ustestIotUartPort;
    int32_t lClose, lWrite;

    uint8_t cpBuffer[ testBufferSize ] = { 0 };

    strncpy( cpBuffer, testIotUART_READ_BUFFER_STRING, testIotUART_READ_BUFFER_LENGTH );

    lWrite = iot_uart_write_sync( NULL, cpBuffer, testIotUART_READ_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lWrite );

    xOpen = iot_uart_open( ucPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xOpen );

    lWrite = iot_uart_write_sync( xOpen, NULL, testIotUART_READ_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lWrite );

    lWrite = iot_uart_write_sync( xOpen, cpBuffer, 0 );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lWrite );

    lClose = iot_uart_close( xOpen );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to fuzz iot_uart_read_async
 * /*-----------------------------------------------------------*/
TEST( TEST_IOT_UART, AFQP_IotUART_ReadAsyncFuzzing )
{
    IotUARTHandle_t xOpen;
    uint8_t ucPort = ustestIotUartPort;
    int32_t lClose, lRead;

    uint8_t cpBufferRead[ 32 ] = { 0 };

    lRead = iot_uart_read_async( NULL, cpBufferRead, testIotUART_READ_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lRead );

    xOpen = iot_uart_open( ucPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xOpen );

    lRead = iot_uart_read_async( xOpen, NULL, testIotUART_READ_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lRead );

    lRead = iot_uart_read_async( xOpen, cpBufferRead, 0 );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lRead );

    lClose = iot_uart_close( xOpen );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to fuzz iot_uart_read_sync
 * /*-----------------------------------------------------------*/
TEST( TEST_IOT_UART, AFQP_IotUART_ReadSyncFuzzing )
{
    IotUARTHandle_t xOpen;
    uint8_t ucPort = ustestIotUartPort;
    int32_t lClose, lRead;

    uint8_t cpBufferRead[ 32 ] = { 0 };

    lRead = iot_uart_read_sync( NULL, cpBufferRead, testIotUART_READ_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lRead );

    xOpen = iot_uart_open( ucPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xOpen );

    lRead = iot_uart_read_sync( xOpen, NULL, testIotUART_READ_BUFFER_LENGTH );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lRead );

    lRead = iot_uart_read_sync( xOpen, cpBufferRead, 0 );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lRead );

    lClose = iot_uart_close( xOpen );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to fuzz iot_uart_ioctl
 * /*-----------------------------------------------------------*/
TEST( TEST_IOT_UART, AFQP_IotUART_IoctlFuzzing )
{
    IotUARTHandle_t xOpen;
    int32_t lIoctl, lClose;
    uint8_t ucPort = ustestIotUartPort;
    IotUARTConfig_t localUARTconfig_test;

    /* Call iot_uart_ioctl with NULL handle. Expect IOT_UART_INVALID_VALUE */
    lIoctl = iot_uart_ioctl( NULL, eUartSetConfig, &localUARTconfig_test );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lIoctl );

    xOpen = iot_uart_open( ucPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xOpen );

    /* Call iot_uart_ioctl with valid handle, but with invalid enum (-1) request.Expect IOT_UART_INVALID_VALUE */
    lIoctl = iot_uart_ioctl( xOpen, -1, &localUARTconfig_test );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lIoctl );

    /* Call iot_uart_ioctl with enum eUartSetConfig, and NULL buffer.Expect IOT_UART_INVALID_VALUE */
    lIoctl = iot_uart_ioctl( xOpen, eUartSetConfig, NULL );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lIoctl );

    /* Call iot_uart_ioctl with enum eUartGetConfig, and NULL buffer.Expect IOT_UART_INVALID_VALUE */
    lIoctl = iot_uart_ioctl( xOpen, eUartGetConfig, NULL );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lIoctl );

    /* Call iot_uart_ioctl with enum eGetTxNoOfbytes, and NULL buffer.Expect IOT_UART_INVALID_VALUE */
    lIoctl = iot_uart_ioctl( xOpen, eGetTxNoOfbytes, NULL );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lIoctl );

    /* Call iot_uart_ioctl with enum eGetRxNoOfbytes, and NULL buffer.Expect IOT_UART_INVALID_VALUE */
    lIoctl = iot_uart_ioctl( xOpen, eGetRxNoOfbytes, NULL );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lIoctl );

    lClose = iot_uart_close( xOpen );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );
}
/*-----------------------------------------------------------*/

/**
 * @brief Test Function to fuzz iot_uart_open,
 * iot_uart_cancel and iot_uart_close
 * /*-----------------------------------------------------------*/
TEST( TEST_IOT_UART, AFQP_IotUART_OpenCloseCancelFuzzing )
{
    IotUARTHandle_t xOpen, xOpen2;
    int32_t lClose, lCancel;
    uint8_t ucPort = ustestIotUartPort;

    xOpen = iot_uart_open( -1 );
    TEST_ASSERT_EQUAL( NULL, xOpen );

    lClose = iot_uart_close( NULL );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lClose );

    xOpen = iot_uart_open( ucPort );
    TEST_ASSERT_NOT_EQUAL( NULL, xOpen );

    xOpen2 = iot_uart_open( ucPort );
    TEST_ASSERT_EQUAL( NULL, xOpen2 );

    lCancel = iot_uart_cancel( NULL );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lCancel );

    lClose = iot_uart_close( xOpen );
    TEST_ASSERT_EQUAL( IOT_UART_SUCCESS, lClose );

    lClose = iot_uart_close( xOpen );
    TEST_ASSERT_EQUAL( IOT_UART_INVALID_VALUE, lClose );
}
/*-----------------------------------------------------------*/
