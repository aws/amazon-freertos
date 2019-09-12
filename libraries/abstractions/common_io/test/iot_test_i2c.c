/*
 * Amazon FreeRTOS Common IO V1.0.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file iot_test_i2c.c
 * @brief Tests for the user-facing API functions declared in iot_i2c.h.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Main includes. */
#include "iot_i2c.h"
#include "iot_test_i2c_config.h"

/* Semaphore includes */
#include "platform/iot_threads.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "aws_test_runner.h"

extern uint16_t slave_address1;

extern uint8_t register_address1;
extern uint8_t register_value1;
extern uint8_t register_address2;

extern const int32_t instances[ NUM_OF_INSTANCES ];

/**
 * @brief Test group for Common IO I2C API
 */
TEST_GROUP( COMMON_I2C_API );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for Common IO I2C API
 */
TEST_SETUP( COMMON_I2C_API )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for Common IO I2C API
 */
TEST_TEAR_DOWN( COMMON_I2C_API )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Runs all test cases
 */
TEST_GROUP_RUNNER( COMMON_I2C_API )
{
    RUN_TEST_CASE( COMMON_I2C_API, CheckOpenInvalidParameters );
    RUN_TEST_CASE( COMMON_I2C_API, TestOpenClose );
    RUN_TEST_CASE( COMMON_I2C_API, CheckReadSyncInvalidParameters );
    RUN_TEST_CASE( COMMON_I2C_API, TestReadSyncSetSlaveAddr );
    RUN_TEST_CASE( COMMON_I2C_API, TestReadSyncFromRegister );
    RUN_TEST_CASE( COMMON_I2C_API, CheckWriteSyncInvalidParameters );
    RUN_TEST_CASE( COMMON_I2C_API, TestWriteSyncSetSlaveAddr );
    RUN_TEST_CASE( COMMON_I2C_API, TestWriteSyncFromRegister );
    RUN_TEST_CASE( COMMON_I2C_API, CheckReadAsyncInvalidParameters );
    RUN_TEST_CASE( COMMON_I2C_API, TestReadAsyncSetSlaveAddr );
    RUN_TEST_CASE( COMMON_I2C_API, TestReadAsyncFromRegister );
    RUN_TEST_CASE( COMMON_I2C_API, CheckWriteAsyncInvalidParameters );
    RUN_TEST_CASE( COMMON_I2C_API, TestWriteAsyncSetSlaveAddr );
    RUN_TEST_CASE( COMMON_I2C_API, TestWriteAsyncFromRegister );
    RUN_TEST_CASE( COMMON_I2C_API, CheckIoctlInvalidValue );
    RUN_TEST_CASE( COMMON_I2C_API, TestSetGetMasterConfig );
    RUN_TEST_CASE( COMMON_I2C_API, CheckCloseInvalidParameters );
}

/*-----------------------------------------------------------*/

/*--------------------Private functions----------------------*/

/**
 * @brief Callback function used for testing asynchronous calls
 */
static void prvCallback( IotI2COperationStatus_t xOpStatus,
                         void * pvUserContext )
{
    /* Check operation status */
    if( xOpStatus != eI2CCompleted )
    {
        return;
    }

    IotSemaphore_t * xSemaphore = ( IotSemaphore_t * ) pvUserContext;

    IotSemaphore_PostFromISR( xSemaphore );
}

/**
 * @brief Tests reading synchronously from a register and testing the expected value
 */
static void prvReadSyncFromRegister( IotI2CHandle_t const xHandle,
                                     uint8_t * pucRegister,
                                     uint8_t ucExpectedVal )
{
    int32_t status;
    const char readBuffer[ 2 ];

    /* Go to specific register */
    status = iot_i2c_write_sync( xHandle, pucRegister, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Going to register failed." );

    /* Read from that register */
    status = iot_i2c_read_sync( xHandle, readBuffer, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( ucExpectedVal, readBuffer[ 0 ], "Read Synchronously failed." );
}

/**
 * @brief Tests reading asynchronously from a register and testing the expected value
 */
static void prvReadAsyncFromRegister( IotI2CHandle_t const xHandle,
                                      uint8_t * pucRegister,
                                      uint8_t ucExpectedVal,
                                      IotSemaphore_t * xTestSemaphore )
{
    int32_t status;
    int32_t semaphoreStatus;
    int32_t timeout = 25;
    const char readBuffer[ 2 ];

    /* Go to specific register */
    status = iot_i2c_write_async( xHandle, pucRegister, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Going to register failed." );

    /* Block the semaphore */
    semaphoreStatus = IotSemaphore_TimedWait( xTestSemaphore, timeout );
    TEST_ASSERT_EQUAL_MESSAGE( true, semaphoreStatus, "Semaphore wait timeout" );

    /* Read from that register */
    status = iot_i2c_read_async( xHandle, readBuffer, 1 );

    /* Block the semaphore */
    semaphoreStatus = IotSemaphore_TimedWait( xTestSemaphore, timeout );
    TEST_ASSERT_EQUAL_MESSAGE( true, semaphoreStatus, "Semaphore wait timeout" );

    /* Check that it is correct value */
    TEST_ASSERT_EQUAL_MESSAGE( ucExpectedVal, readBuffer[ 0 ], "Read Asynchronously failed." );
}

/*-----------------------------------------------------------*/

/*---------------Tests for iot_i2c_open()--------------------*/

/**
 * @brief Tests for invalid parameters
 */
TEST( COMMON_I2C_API, CheckOpenInvalidParameters )
{
    IotI2CHandle_t xHandle;
    int invalid_parameter = -1;
    int testAddr = 0x36;

    /* Check for null parameter */
    xHandle = iot_i2c_open( invalid_parameter );
    TEST_ASSERT_TRUE_MESSAGE( ( xHandle == NULL ), "Handle initialized with null parameter" );

    /* Check for incorrect address other than instances from the board */
    xHandle = iot_i2c_open( testAddr );
    TEST_ASSERT_TRUE_MESSAGE( ( xHandle == NULL ), "Handle initialized with invalid address" );
}

/**
 * @brief Tests for valid parameters and proper initialization of each I2C Instance
 */
TEST( COMMON_I2C_API, TestOpenClose )
{
    IotI2CHandle_t xHandle;
    int32_t status;
    int i = 0;

    for( ; i < NUM_OF_INSTANCES; i++ )
    {
        xHandle = iot_i2c_open( instances[ i ] );
        TEST_ASSERT_NOT_NULL_MESSAGE( &xHandle, "I2C handle initialization failed." );

        /* Deinitialize the driver once done */
        status = iot_i2c_close( xHandle );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Deinitializing I2C handle unsuccessful" );
    }
}

/*-----------------------------------------------------------*/

/*--------------Tests for iot_i2c_read_sync()---------------*/

/**
 * @brief Tests for invalid parameters
 */
TEST( COMMON_I2C_API, CheckReadSyncInvalidParameters )
{
    int32_t status;
    const char readBuffer[ 2 ];

    /* Opens new handle */
    IotI2CHandle_t xHandle = iot_i2c_open( IOT_I2C2 );

    TEST_ASSERT_NOT_NULL_MESSAGE( &xHandle, "I2C2 handle initialization failed." );

    /* Checks for every combinati0on of invalid parameters */
    status = iot_i2c_read_sync( NULL, NULL, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing all invalid parameters in read sync" );

    status = iot_i2c_read_sync( NULL, NULL, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid handle and buffer parameters in read sync" );

    status = iot_i2c_read_sync( xHandle, NULL, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid buffer and byte parameters in read sync" );

    status = iot_i2c_read_sync( NULL, readBuffer, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid handle and byte parameters in read sync" );

    status = iot_i2c_read_sync( NULL, readBuffer, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid handle parameter in read sync" );

    status = iot_i2c_read_sync( xHandle, readBuffer, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid byte parameter in read sync" );

    status = iot_i2c_read_sync( xHandle, NULL, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid buffer parameter in read sync" );

    /* Deinitialize the driver once done */
    status = iot_i2c_close( xHandle );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Deinitializing handle unsuccessful" );
}

/**
 * @brief Tests if read sync catches missing prerequisites
 */
TEST( COMMON_I2C_API, TestReadSyncSetSlaveAddr )
{
    int32_t status;
    const char readBuffer[ 2 ];
    IotI2CConfig_t xConfig =
    {
        25U,
        IOT_I2C_STANDARD_MODE_BPS
    };

    /* Opens new handle */
    IotI2CHandle_t xHandle = iot_i2c_open( IOT_I2C2 );

    TEST_ASSERT_NOT_NULL_MESSAGE( &xHandle, "I2C2 handle initialization failed." );

    /* Check to see read fails without slave address */
    status = iot_i2c_read_sync( xHandle, readBuffer, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_READ_FAILED, status, "Reading continues without set slave address." );

    /* Call the ioctl to set the slave address */
    status = iot_i2c_ioctl( xHandle, eI2CSetSlaveAddr, &slave_address1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Slave address not set properly" );

    /* Deinitialize the driver once done */
    status = iot_i2c_close( xHandle );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Deinitializing handle unsuccessful" );
}

/**
 * @brief Tests for correct reading from a register
 */
TEST( COMMON_I2C_API, TestReadSyncFromRegister )
{
    int32_t status;
    const char readBuffer[ 2 ];
    IotI2CConfig_t xConfig =
    {
        25U,
        IOT_I2C_STANDARD_MODE_BPS
    };

    /* Opens new handle */
    IotI2CHandle_t xHandle = iot_i2c_open( IOT_I2C2 );

    TEST_ASSERT_NOT_NULL_MESSAGE( &xHandle, "I2C2 handle initialization failed." );

    /* Call the ioctl to set the slave address */
    status = iot_i2c_ioctl( xHandle, eI2CSetSlaveAddr, &slave_address1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Slave address not set properly" );

    /* Call the ioctl to set configuration settings */
    status = iot_i2c_ioctl( xHandle, eI2CSetMasterConfig, &xConfig );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Setting configurations failed." );

    prvReadSyncFromRegister( xHandle, &register_address1, register_value1 );

    /* Deinitialize the driver once done */
    status = iot_i2c_close( xHandle );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Deinitializing handle unsuccessful" );
}

/*-----------------------------------------------------------*/

/*--------------Tests for iot_i2c_write_sync()---------------*/

/**
 * @brief Tests for invalid parameters
 */
TEST( COMMON_I2C_API, CheckWriteSyncInvalidParameters )
{
    int32_t status;
    const char * writeBuffer = "A";

    /* Opens new handle */
    IotI2CHandle_t xHandle = iot_i2c_open( IOT_I2C2 );

    TEST_ASSERT_NOT_NULL_MESSAGE( &xHandle, "I2C2 handle initialization failed." );

    /* Checks for every combinati0on of invalid parameters */
    status = iot_i2c_write_sync( NULL, NULL, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing all invalid parameters in write sync" );

    status = iot_i2c_write_sync( NULL, NULL, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid handle and buffer parameters in write sync" );

    status = iot_i2c_write_sync( xHandle, NULL, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid buffer and byte parameters in write sync" );

    status = iot_i2c_write_sync( NULL, writeBuffer, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid handle and byte parameters in write sync" );

    status = iot_i2c_write_sync( NULL, writeBuffer, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid handle parameter in write sync" );

    status = iot_i2c_write_sync( xHandle, writeBuffer, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid byte parameter in write sync" );

    status = iot_i2c_write_sync( xHandle, NULL, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid buffer parameter in write sync" );

    /* Deinitialize the driver once done */
    status = iot_i2c_close( xHandle );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Deinitializing handle unsuccessful" );
}

/**
 * @brief Tests if write sync catches missing prerequisites
 */
TEST( COMMON_I2C_API, TestWriteSyncSetSlaveAddr )
{
    int32_t status;
    const char * writeBuffer = "A";
    IotI2CConfig_t xConfig =
    {
        25U,
        IOT_I2C_STANDARD_MODE_BPS
    };

    /* Opens new handle */
    IotI2CHandle_t xHandle = iot_i2c_open( IOT_I2C2 );

    TEST_ASSERT_NOT_NULL_MESSAGE( &xHandle, "I2C2 handle initialization failed." );

    /* Check to see write fails without slave address */
    status = iot_i2c_write_sync( xHandle, writeBuffer, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_WRITE_FAILED, status, "Writing continues without set slave address." );

    /* Call the ioctl to set the slave address */
    status = iot_i2c_ioctl( xHandle, eI2CSetSlaveAddr, &slave_address1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Slave address not set properly" );

    /* Deinitialize the driver once done */
    status = iot_i2c_close( xHandle );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Deinitializing handle unsuccessful" );
}

/**
 * @brief Tests for writing to a register properly
 */
TEST( COMMON_I2C_API, TestWriteSyncFromRegister )
{
    int32_t status;
    const char readBuffer[ 2 ];
    uint8_t data[ 2 ];
    IotI2CConfig_t xConfig =
    {
        25U,
        IOT_I2C_STANDARD_MODE_BPS
    };
    uint16_t numBytes = 1;
    uint16_t value = 2;

    data[ 0 ] = register_address2;
    data[ 1 ] = value;

    /* Opens new handle */
    IotI2CHandle_t xHandle = iot_i2c_open( IOT_I2C2 );
    TEST_ASSERT_NOT_NULL_MESSAGE( &xHandle, "I2C2 handle initialization failed." );

    /* Call the ioctl to set the slave address */
    status = iot_i2c_ioctl( xHandle, eI2CSetSlaveAddr, &slave_address1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Slave address not set properly" );

    /* Call the ioctl to set configuration settings */
    status = iot_i2c_ioctl( xHandle, eI2CSetMasterConfig, &xConfig );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Setting configurations failed." );

    /* Write to specific register */
    status = iot_i2c_write_sync( xHandle, data, 2 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Writing to register failed." );

    /* Test if value was written correctly */
    prvReadSyncFromRegister( xHandle, &register_address2, value );

    /* Deinitialize the driver once done */
    status = iot_i2c_close( xHandle );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Deinitializing handle unsuccessful" );
}

/*-----------------------------------------------------------*/

/*--------------Tests for iot_i2c_read_async()---------------*/

/**
 * @brief Tests for invalid parameters
 */
TEST( COMMON_I2C_API, CheckReadAsyncInvalidParameters )
{
    int32_t status;
    const char readBuffer[ 2 ];

    /* Opens new handle */
    IotI2CHandle_t xHandle = iot_i2c_open( IOT_I2C2 );

    TEST_ASSERT_NOT_NULL_MESSAGE( &xHandle, "I2C2 handle initialization failed." );

    /* Checks for every combination of invalid parameters */
    status = iot_i2c_read_async( NULL, NULL, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing all invalid parameters in read async" );

    status = iot_i2c_read_async( NULL, NULL, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid handle and buffer parameters in read async" );

    status = iot_i2c_read_async( xHandle, NULL, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid buffer and byte parameters in read async" );

    status = iot_i2c_read_async( NULL, readBuffer, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid handle and byte parameters in read async" );

    status = iot_i2c_read_async( NULL, readBuffer, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid handle parameter in read async" );

    status = iot_i2c_read_async( xHandle, readBuffer, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid byte parameter in read async" );

    status = iot_i2c_read_async( xHandle, NULL, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid buffer parameter in read async" );

    /* Deinitialize the driver once done */
    status = iot_i2c_close( xHandle );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Deinitializing handle unsuccessful" );
}

/**
 * @brief Tests is read async catches missing prerequisites
 */
TEST( COMMON_I2C_API, TestReadAsyncSetSlaveAddr )
{
    int32_t status;
    int32_t semaphoreStatus;
    int32_t timeout = 25;
    const char readBuffer[ 2 ];
    IotI2CConfig_t xConfig =
    {
        25U,
        IOT_I2C_STANDARD_MODE_BPS
    };

    /* Opens new handle */
    IotI2CHandle_t xHandle = iot_i2c_open( IOT_I2C2 );

    TEST_ASSERT_NOT_NULL_MESSAGE( &xHandle, "I2C2 handle initialization failed." );

    /* Check to see read fails without slave address */
    status = iot_i2c_read_async( xHandle, readBuffer, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_READ_FAILED, status, "Reading continues without set slave address." );

    /* Call the ioctl to set the slave address */
    status = iot_i2c_ioctl( xHandle, eI2CSetSlaveAddr, &slave_address1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Slave address not set properly" );

    /* Deinitialize the driver once done */
    status = iot_i2c_close( xHandle );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Deinitializing handle unsuccessful" );
}

/**
 * @brief Tests for correct reading from a register
 */
TEST( COMMON_I2C_API, TestReadAsyncFromRegister )
{
    int32_t status;
    int32_t semaphoreStatus;
    int32_t timeout = 25;
    int numBytes = 1;
    const char readBuffer[ 2 ];
    const char readByteBuffer[ 2 ];
    IotI2CConfig_t xConfig =
    {
        25U,
        IOT_I2C_STANDARD_MODE_BPS
    };

    IotSemaphore_t xTestSemaphore;

    /* Opens new handle */
    IotI2CHandle_t xHandle = iot_i2c_open( IOT_I2C2 );

    TEST_ASSERT_NOT_NULL_MESSAGE( &xHandle, "I2C2 handle initialization failed." );

    /* Create semaphore */
    semaphoreStatus = IotSemaphore_Create( &xTestSemaphore, 0, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( true, semaphoreStatus, "Failed to create semaphore." );

    /* Call the ioctl to set the slave address */
    status = iot_i2c_ioctl( xHandle, eI2CSetSlaveAddr, &slave_address1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Slave address not set properly" );

    /* Call the ioctl to set configuration settings */
    status = iot_i2c_ioctl( xHandle, eI2CSetMasterConfig, &xConfig );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Setting configurations failed." );

    /* Set callback function */
    iot_i2c_set_callback( xHandle, prvCallback, &xTestSemaphore );

    prvReadAsyncFromRegister( xHandle, &register_address1, register_value1, &xTestSemaphore );

    status = iot_i2c_ioctl( xHandle, eI2CGetRxNoOfbytes, readByteBuffer );
    TEST_ASSERT_EQUAL_MESSAGE( numBytes, readByteBuffer[ 0 ], "Incorrect number of bytes received" );

    /* Deinitialize the driver once done */
    status = iot_i2c_close( xHandle );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Deinitializing handle unsuccessful" );

    /* Destroy semaphore */
    IotSemaphore_Destroy( &xTestSemaphore );
}

/*-----------------------------------------------------------*/

/*--------------Tests for iot_i2c_write_async()---------------*/

/**
 * @brief Tests for invalid parameters
 */
TEST( COMMON_I2C_API, CheckWriteAsyncInvalidParameters )
{
    int32_t status;
    const char * writeBuffer = "A";

    /* Opens new handle */
    IotI2CHandle_t xHandle = iot_i2c_open( IOT_I2C2 );

    TEST_ASSERT_NOT_NULL_MESSAGE( &xHandle, "I2C2 handle initialization failed." );

    /* Checks for every combination of invalid parameters */
    status = iot_i2c_write_async( NULL, NULL, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing all invalid parameters in write async" );

    status = iot_i2c_write_async( NULL, NULL, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid handle and buffer parameters in write async" );

    status = iot_i2c_write_async( xHandle, NULL, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid buffer and byte parameters in write async" );

    status = iot_i2c_write_async( NULL, writeBuffer, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid handle and byte parameters in write async" );

    status = iot_i2c_write_async( NULL, writeBuffer, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid handle parameter in write async" );

    status = iot_i2c_write_async( xHandle, writeBuffer, 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid byte parameter in write async" );

    status = iot_i2c_write_async( xHandle, NULL, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Utilizing invalid buffer parameter in write async" );

    /* Deinitialize the driver once done */
    status = iot_i2c_close( xHandle );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Deinitializing handle unsuccessful" );
}

/**
 * @brief Tests is write async catches missing prerequisites
 */
TEST( COMMON_I2C_API, TestWriteAsyncSetSlaveAddr )
{
    int32_t status;
    int32_t semaphoreStatus;
    int32_t timeout = 25;
    const char * writeBuffer = "A";
    IotI2CConfig_t xConfig =
    {
        25U,
        IOT_I2C_STANDARD_MODE_BPS
    };

    /* Opens new handle */
    IotI2CHandle_t xHandle = iot_i2c_open( IOT_I2C2 );

    TEST_ASSERT_NOT_NULL_MESSAGE( &xHandle, "I2C2 handle initialization failed." );

    /* Check to see write fails without slave address */
    status = iot_i2c_write_async( xHandle, writeBuffer, 1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_WRITE_FAILED, status, "Writing continues without set slave address." );

    /* Call the ioctl to set the slave address */
    status = iot_i2c_ioctl( xHandle, eI2CSetSlaveAddr, &slave_address1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Slave address not set properly" );

    /* Deinitialize the driver once done */
    status = iot_i2c_close( xHandle );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Deinitializing handle unsuccessful" );
}

/**
 * @brief Tests writing to a register
 */
TEST( COMMON_I2C_API, TestWriteAsyncFromRegister )
{
    int32_t status;
    int32_t semaphoreStatus;
    int32_t timeout = 25;
    int numBytes = 1;
    const char readBuffer[ 2 ];
    const char readByteBuffer[ 2 ];
    uint8_t data[ 2 ];
    uint16_t val = 2;
    IotI2CConfig_t xConfig =
    {
        25U,
        IOT_I2C_STANDARD_MODE_BPS
    };

    IotSemaphore_t xTestSemaphore;

    data[ 0 ] = register_address2;
    data[ 1 ] = val;

    /* Opens new handle */
    IotI2CHandle_t xHandle = iot_i2c_open( IOT_I2C2 );
    TEST_ASSERT_NOT_NULL_MESSAGE( &xHandle, "I2C2 handle initialization failed." );

    /* Create semaphore */
    semaphoreStatus = IotSemaphore_Create( &xTestSemaphore, 0, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( true, semaphoreStatus, "Failed to create semaphore." );

    /* Call the ioctl to set the slave address */
    status = iot_i2c_ioctl( xHandle, eI2CSetSlaveAddr, &slave_address1 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Slave address not set properly" );

    /* Call the ioctl to set configuration settings */
    status = iot_i2c_ioctl( xHandle, eI2CSetMasterConfig, &xConfig );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Setting configurations failed." );

    /* Set callback function */
    iot_i2c_set_callback( xHandle, prvCallback, &xTestSemaphore );

    /* Go to register and send data you want to write */
    status = iot_i2c_write_async( xHandle, data, 2 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Writing to register failed." );

    /* Block the semaphore */
    semaphoreStatus = IotSemaphore_TimedWait( &xTestSemaphore, timeout );
    TEST_ASSERT_EQUAL_MESSAGE( true, semaphoreStatus, "Semaphore wait timeout" );

    status = iot_i2c_ioctl( xHandle, eI2CGetTxNoOfbytes, readByteBuffer );
    TEST_ASSERT_EQUAL_MESSAGE( numBytes, readByteBuffer[ 0 ], "Incorrect number of bytes transmitted" );

    prvReadAsyncFromRegister( xHandle, &register_address2, val, &xTestSemaphore );

    status = iot_i2c_ioctl( xHandle, eI2CGetRxNoOfbytes, readByteBuffer );
    TEST_ASSERT_EQUAL_MESSAGE( numBytes, readByteBuffer[ 0 ], "Incorrect number of bytes received" );

    /* Deinitialize the driver once done */
    status = iot_i2c_close( xHandle );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Deinitializing handle unsuccessful" );

    /* Destroy semaphore */
    IotSemaphore_Destroy( &xTestSemaphore );
}

/*------------Tests for iot_i2c_ioctl() commands-------------*/

/**
 * @brief Tests for invalid ioctl commands
 */
TEST( COMMON_I2C_API, CheckIoctlInvalidValue )
{
    int32_t status;

    /* Opens new handle */
    IotI2CHandle_t xHandle = iot_i2c_open( IOT_I2C1 );

    TEST_ASSERT_NOT_NULL_MESSAGE( &xHandle, "I2C1 handle initialization failed." );

    status = iot_i2c_ioctl( xHandle, -1, NULL );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Invalid ioctl request in use." );

    /* Deinitialize the driver once done */
    status = iot_i2c_close( xHandle );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Deinitializing handle unsuccessful" );
}

/**
 * @brief Tests for eI2CSetMasterConfig and eI2CGetMasterConfig commands
 */
TEST( COMMON_I2C_API, TestSetGetMasterConfig )
{
    int32_t status;
    IotI2CConfig_t xConfig =
    {
        25U,
        IOT_I2C_STANDARD_MODE_BPS
    };
    IotI2CConfig_t receivedConfig;

    /* Opens new handle */
    IotI2CHandle_t xHandle = iot_i2c_open( IOT_I2C1 );

    TEST_ASSERT_NOT_NULL_MESSAGE( &xHandle, "I2C1 handle initialization failed." );

    status = iot_i2c_ioctl( xHandle, eI2CSetMasterConfig, &xConfig );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_SUCCESS, status, "Setting Master Config unsuccessful." );

    status = iot_i2c_ioctl( xHandle, eI2CGetMasterConfig, &receivedConfig );
    TEST_ASSERT_EQUAL_MESSAGE( xConfig.ulMasterTimeout, receivedConfig.ulMasterTimeout, "Receiving correct timeout failed." );
    TEST_ASSERT_EQUAL_MESSAGE( xConfig.ulBusFreq, receivedConfig.ulBusFreq, "Receiving correct bus frequemcy failed." );
}

/*----------------Tests for iot_i2c_close()------------------*/

/**
 * @brief Tests for invalid parameters
 */
TEST( COMMON_I2C_API, CheckCloseInvalidParameters )
{
    int32_t status;

    status = iot_i2c_close( NULL );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( IOT_I2C_INVALID_VALUE, status, "Deinitializing NULL handle" );
}

/*-----------------------------------------------------------*/
