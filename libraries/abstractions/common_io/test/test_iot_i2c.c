/*
 * FreeRTOS Common IO V0.1.3
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file iot_test_i2c.c
 * @brief Functional Unit Test - I2C
 *******************************************************************************
 */

#include "unity.h"
#include "unity_fixture.h"

#include "iot_i2c.h"
#include "iot_test_common_io_internal.h"

#include "FreeRTOS.h"
#include "semphr.h"

/* Max allowed length of data to write. */
#ifndef IOT_TEST_COMMON_IO_I2C_MAX_WRITE_LENGTH
    #define IOT_TEST_COMMON_IO_I2C_MAX_WRITE_LENGTH    ( 10 )
#endif

/*-----------------------------------------------------------*/
#define testIotI2C_BAUDRATE               IOT_I2C_FAST_MODE_BPS
#define testIotI2C_BAUDRATE_LOW_SPEED     IOT_I2C_STANDARD_MODE_BPS
#define testIotI2C_DEFAULT_TIMEOUT        500 /**< 500 msec */
#define testIotI2C_FAST_TIMEOUT           100 /**< 100 msec */
#define testIotI2C_INVALID_IOCTL_INDEX    UINT8_MAX
#define testIotI2C_HANDLE_NUM             4
#define testIotI2C_MAX_TIMEOUT            pdMS_TO_TICKS( 10000 )
#define testIotI2C_MESSAGE_LENGTH         50
/*-----------------------------------------------------------*/

typedef struct CallbackParam
{
    IotI2CHandle_t xHandle;
    uint8_t * pucBuffer;
    size_t xBufferSize;
    uint32_t ulRet;
} CallbackParam_t;

/*-----------------------------------------------------------*/

/* Globals values which can be overwritten by the test
 * framework invoking these tests */
/*-----------------------------------------------------------*/

uint16_t uctestIotI2CSlaveAddr = 0;             /**< The slave address to be set for the I2C port. */
uint16_t uctestIotI2CInvalidSlaveAddr = 0xFFFF; /**< The slave address to be set for the I2C port. */
uint8_t uctestIotI2CDeviceRegister = 0;         /**< The device register to be set for the I2C port. */
uint8_t uctestIotI2CWriteVal = 0;               /**< The write value to write to device. */
uint8_t uctestIotI2CInstanceIdx = 0;            /**< The current I2C test instance index */
uint8_t uctestIotI2CInstanceNum = 1;            /**< The total I2C test instance number */
uint16_t ucAssistedTestIotI2CSlaveAddr = 0;     /**< The slave address to be set for the assisted test. */

/*-----------------------------------------------------------*/
/* Static Globals */
/*-----------------------------------------------------------*/
static SemaphoreHandle_t xtestIotI2CSemaphore = NULL;

/*-----------------------------------------------------------*/
/* Private Functions */
/*-----------------------------------------------------------*/

static void prvAppendToMessage( size_t * pOffset,
                                uint8_t * pBuffer,
                                size_t bufferLen,
                                char * cMsg );

/*-----------------------------------------------------------*/

/* Define Test Group. */
TEST_GROUP( TEST_IOT_I2C );

/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_I2C )
{
    if( xtestIotI2CSemaphore == NULL )
    {
        xtestIotI2CSemaphore = xSemaphoreCreateBinary();
        TEST_ASSERT_NOT_EQUAL( NULL, xtestIotI2CSemaphore );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_I2C )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief user callback function
 */
void prvI2CCallback( IotI2COperationStatus_t xOpStatus,
                     void * pvParam )
{
    /* Disable unused parameter warning. */
    ( void ) pvParam;

    if( xOpStatus == eI2CCompleted )
    {
        xSemaphoreGiveFromISR( xtestIotI2CSemaphore, NULL );
    }
}

static void prvChainToReadCallback( IotI2COperationStatus_t xOpStatus,
                                    void * pvParam )
{
    static uint8_t ucOps = 0;

    CallbackParam_t * pxCallbackParam = ( CallbackParam_t * ) pvParam;

    if( xOpStatus == eI2CCompleted )
    {
        /* Write. */
        if( ucOps == 0 )
        {
            pxCallbackParam->ulRet = iot_i2c_read_async( pxCallbackParam->xHandle, pxCallbackParam->pucBuffer, pxCallbackParam->xBufferSize );
            ucOps++;
        }
        else
        {
            xSemaphoreGiveFromISR( xtestIotI2CSemaphore, NULL );
        }
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_I2C )
{
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2COpenCloseSuccess );
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2COpenCloseFail );
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2COpenCloseFailUnsupportInst );
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CSetGetConfigurationSuccess );
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CSetGetConfigurationFail );

    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CWriteSyncSuccess );
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CWriteSyncFail );
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CReadSyncFail );

    /* If it supports not sending stop condition. */
    #if ( IOT_TEST_COMMON_IO_I2C_SUPPORTED_SEND_NO_STOP == 1 )
        RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CGetBusStateSuccess );
        RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CWriteThenReadSyncSuccess );
        RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CReadAsyncFailIoctl );
        RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CReadAsyncFailReadTwice );
    #endif

    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CWriteAsyncSuccess );
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CWriteAsyncFailIoctl );
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CWriteAsyncFailWriteTwice );
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CReadAsyncFailSetAddr );

    /* If it supports not sending stop condition. */
    #if ( IOT_TEST_COMMON_IO_I2C_SUPPORTED_SEND_NO_STOP == 1 )
        RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CWriteThenReadAsyncSuccess );
        RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CWriteThenReadChainSuccess );
    #endif

    /* If it supports cancel the current operation. */
    #if ( IOT_TEST_COMMON_IO_I2C_SUPPORTED_CANCEL == 1 )
        #if ( IOT_TEST_COMMON_IO_I2C_SUPPORTED_SEND_NO_STOP == 1 )
            RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CCancelReadSuccess );
        #endif
        RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CCancelFail );
    #endif

    /* Unsupported tests. */
    #if ( IOT_TEST_COMMON_IO_I2C_SUPPORTED_SEND_NO_STOP != 1 )
        RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CSendNoStopUnsupported );
    #endif

    #if ( IOT_TEST_COMMON_IO_I2C_SUPPORTED_CANCEL != 1 )
        RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CCancelUnsupported );
    #endif
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_i2c_open and iot_i2c_close.
 */
TEST( TEST_IOT_I2C, AFQP_IotI2COpenCloseSuccess )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_i2c_open and iot_i2c_close fail case.
 */
TEST( TEST_IOT_I2C, AFQP_IotI2COpenCloseFail )
{
    IotI2CHandle_t xI2CHandle;
    IotI2CHandle_t xI2CHandle_1 = NULL;
    int32_t lRetVal;

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Open i2c to initialize hardware again */
        xI2CHandle_1 = iot_i2c_open( uctestIotI2CInstanceIdx );
        TEST_ASSERT_EQUAL( NULL, xI2CHandle_1 );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

    /* Close same i2c again which is not open */
    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_INVALID_VALUE, lRetVal );

    /* Close i2c with NULL pointer */
    lRetVal = iot_i2c_close( xI2CHandle_1 );
    TEST_ASSERT_EQUAL( IOT_I2C_INVALID_VALUE, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_i2c_open and iot_i2c_close with unsupported instance
 */
TEST( TEST_IOT_I2C, AFQP_IotI2COpenCloseFailUnsupportInst )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceNum );
    TEST_ASSERT_EQUAL( NULL, xI2CHandle );

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_INVALID_VALUE, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C set and get configuration success
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CSetGetConfigurationSuccess )
{
    IotI2CHandle_t xI2CHandle;
    IotI2CConfig_t xI2CConfig_write;
    IotI2CConfig_t xI2CConfig_read;
    int32_t lRetVal;

    xI2CConfig_write.ulBusFreq = testIotI2C_BAUDRATE;
    xI2CConfig_write.ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT;

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set default i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig_write );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Get i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetMasterConfig, &xI2CConfig_read );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Make sure get the correct config from i2c */
        TEST_ASSERT_EQUAL( testIotI2C_BAUDRATE, xI2CConfig_read.ulBusFreq );
        TEST_ASSERT_EQUAL( testIotI2C_DEFAULT_TIMEOUT, xI2CConfig_read.ulMasterTimeout );

        xI2CConfig_write.ulBusFreq = testIotI2C_BAUDRATE_LOW_SPEED;
        xI2CConfig_write.ulMasterTimeout = testIotI2C_FAST_TIMEOUT;

        /* Set new i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig_write );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Get new i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetMasterConfig, &xI2CConfig_read );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Make sure get the correct config from i2c */
        TEST_ASSERT_EQUAL( testIotI2C_BAUDRATE_LOW_SPEED, xI2CConfig_read.ulBusFreq );
        TEST_ASSERT_EQUAL( testIotI2C_FAST_TIMEOUT, xI2CConfig_read.ulMasterTimeout );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C set and get configuration fail
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CSetGetConfigurationFail )
{
    IotI2CHandle_t xI2CHandle;
    IotI2CConfig_t xI2CConfig;
    int32_t lRetVal;

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set default i2c configuration with NULL pointer */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, NULL );
        TEST_ASSERT_EQUAL( IOT_I2C_INVALID_VALUE, lRetVal );

        /* Get i2c configuration with NULL pointer */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetMasterConfig, NULL );
        TEST_ASSERT_EQUAL( IOT_I2C_INVALID_VALUE, lRetVal );

        /* i2c ioctl with unsupported request */
        lRetVal = iot_i2c_ioctl( xI2CHandle, testIotI2C_INVALID_IOCTL_INDEX, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_INVALID_VALUE, lRetVal );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C async read success
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CWriteThenReadAsyncSuccess )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint16_t readBytes;
    uint16_t writeBytes;

    uint8_t singleByte;

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set completion callback */
        iot_i2c_set_callback( xI2CHandle, prvI2CCallback, NULL );

        /* Set i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address for writing the device register */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c not stop between transaction */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSendNoStopFlag, NULL );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* write the device register address. */
        lRetVal = iot_i2c_write_sync( xI2CHandle, &uctestIotI2CDeviceRegister, sizeof( uctestIotI2CDeviceRegister ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address to read from the device register. */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Read from i2c device for single byte. */
        lRetVal = iot_i2c_read_async( xI2CHandle, &singleByte, sizeof( singleByte ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        lRetVal = xSemaphoreTake( xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetRxNoOfbytes, &readBytes );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
        /* Assert the number of bytes being read is 1. */
        TEST_ASSERT_EQUAL( sizeof( singleByte ), readBytes );

        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetTxNoOfbytes, &writeBytes );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
        /* Assert the number of bytes being written is 1. */
        TEST_ASSERT_EQUAL( sizeof( uctestIotI2CDeviceRegister ), writeBytes );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C async read success
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CReadAsyncAssisted )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t ucReadBuf[ 16 ];
    char cMsg[ testIotI2C_MESSAGE_LENGTH ] = { 0 };
    size_t msgOffset = 0;

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set completion callback */
        iot_i2c_set_callback( xI2CHandle, prvI2CCallback, NULL );

        /* Set i2c congifuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address for writing the device register */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &ucAssistedTestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c not stop between transaction */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSendNoStopFlag, NULL );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Write the device register address */
        lRetVal = iot_i2c_write_sync( xI2CHandle, &uctestIotI2CDeviceRegister, sizeof( uctestIotI2CDeviceRegister ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address to read from the device register */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &ucAssistedTestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Read from i2c device */
        lRetVal = iot_i2c_read_async( xI2CHandle, ucReadBuf, sizeof( ucReadBuf ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        lRetVal = xSemaphoreTake( xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

    /* Create a string with read bytes and print it to console */
    prvAppendToMessage( &msgOffset, ucReadBuf, sizeof( ucReadBuf ), cMsg );

    TEST_IGNORE_MESSAGE( cMsg );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C async read success
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CWriteThenReadChainSuccess )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint16_t readBytes;
    uint16_t writeBytes;

    uint8_t singleByte;

    CallbackParam_t xCallbackParam;

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        xCallbackParam.xHandle = xI2CHandle;
        xCallbackParam.pucBuffer = &singleByte;
        xCallbackParam.xBufferSize = 1;
        xCallbackParam.ulRet = 0;

        /* Set completion callback */
        iot_i2c_set_callback( xI2CHandle, prvChainToReadCallback, &xCallbackParam );

        /* Set i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address for writing the device register */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c not stop between transaction */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSendNoStopFlag, NULL );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* write the device register address. */
        lRetVal = iot_i2c_write_async( xI2CHandle, &uctestIotI2CDeviceRegister, sizeof( uctestIotI2CDeviceRegister ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        lRetVal = xSemaphoreTake( xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetRxNoOfbytes, &readBytes );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
        /* Assert the number of bytes being read is 1. */
        TEST_ASSERT_EQUAL( sizeof( singleByte ), readBytes );

        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetTxNoOfbytes, &writeBytes );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
        /* Assert the number of bytes being written is 1. */
        TEST_ASSERT_EQUAL( sizeof( uctestIotI2CDeviceRegister ), writeBytes );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C async read fail to do ioctl
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CReadAsyncFailIoctl )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t ucBuffer[ 10 ];

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set completion callback */
        iot_i2c_set_callback( xI2CHandle, prvI2CCallback, NULL );

        /* Set i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSendNoStopFlag, NULL );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* write the device register address. */
        lRetVal = iot_i2c_write_sync( xI2CHandle, &uctestIotI2CDeviceRegister, sizeof( uctestIotI2CDeviceRegister ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* read from i2c device register */
        lRetVal = iot_i2c_read_async( xI2CHandle, ucBuffer, sizeof( ucBuffer ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c configuration during transaction */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_BUSY, lRetVal );

        lRetVal = xSemaphoreTake( xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C async read twice fail
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CReadAsyncFailReadTwice )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t ucBuffer[ 10 ];

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set completion callback */
        iot_i2c_set_callback( xI2CHandle, prvI2CCallback, NULL );

        /* Set i2c congifuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSendNoStopFlag, NULL );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* write the device register address. */
        lRetVal = iot_i2c_write_sync( xI2CHandle, &uctestIotI2CDeviceRegister, sizeof( uctestIotI2CDeviceRegister ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* read from i2c device */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSendNoStopFlag, NULL );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* read from i2c device */
        lRetVal = iot_i2c_read_async( xI2CHandle, ucBuffer, sizeof( ucBuffer ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* read from i2c device again */
        lRetVal = iot_i2c_read_async( xI2CHandle, ucBuffer, sizeof( ucBuffer ) );
        TEST_ASSERT_EQUAL( IOT_I2C_BUSY, lRetVal );

        lRetVal = xSemaphoreTake( xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C read async fail. slave address and register not set.
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CReadAsyncFailSetAddr )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t ucReadValue;

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set completion callback */
        iot_i2c_set_callback( xI2CHandle, prvI2CCallback, NULL );

        /* read from i2c device */
        lRetVal = iot_i2c_read_async( xI2CHandle, &ucReadValue, sizeof( ucReadValue ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SLAVE_ADDRESS_NOT_SET, lRetVal );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C cancel
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CCancelReadSuccess )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t readBuffer[ 64 ];

    uint16_t readBytes;

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set i2c configuration. */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSendNoStopFlag, NULL );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* write the device register address. */
        lRetVal = iot_i2c_write_sync( xI2CHandle, &uctestIotI2CDeviceRegister, sizeof( uctestIotI2CDeviceRegister ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set completion callback */
        iot_i2c_set_callback( xI2CHandle, prvI2CCallback, NULL );

        /* Set i2c slave address */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSendNoStopFlag, NULL );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* read from i2c device */
        lRetVal = iot_i2c_read_async( xI2CHandle, readBuffer, sizeof( readBuffer ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Cancel the transaction while the reading operation is in flight. */
        lRetVal = iot_i2c_cancel( xI2CHandle );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Assert the completion callback is NOT invoked. */
        lRetVal = xSemaphoreTake( xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT );
        TEST_ASSERT_EQUAL( pdFALSE, lRetVal );

        uint8_t writeVal1[] = { uctestIotI2CDeviceRegister, uctestIotI2CWriteVal };

        /* Write after cancel should also succeed. */
        lRetVal = iot_i2c_write_sync( xI2CHandle, writeVal1, sizeof( writeVal1 ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetRxNoOfbytes, &readBytes );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Assert the number of bytes being read is less than requested. */
        TEST_ASSERT_LESS_THAN( sizeof( readBuffer ), readBytes );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C cancel fail
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CCancelFail )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* cancel transaction with NULL */
        lRetVal = iot_i2c_cancel( NULL );
        TEST_ASSERT_EQUAL( IOT_I2C_INVALID_VALUE, lRetVal );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

TEST( TEST_IOT_I2C, AFQP_IotI2CCancelUnsupported )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* cancel transaction with NULL */
        lRetVal = iot_i2c_cancel( xI2CHandle );
        TEST_ASSERT_EQUAL( IOT_I2C_FUNCTION_NOT_SUPPORTED, lRetVal );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C async write
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CWriteAsyncSuccess )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t writeVal1[] = { uctestIotI2CDeviceRegister, uctestIotI2CWriteVal };
    uint8_t writeVal2[ IOT_TEST_COMMON_IO_I2C_MAX_WRITE_LENGTH ] = { uctestIotI2CDeviceRegister, uctestIotI2CWriteVal };

    uint16_t writeBytes;

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set completion callback */
        iot_i2c_set_callback( xI2CHandle, prvI2CCallback, NULL );

        /* Set i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* write the value to the device */
        lRetVal = iot_i2c_write_async( xI2CHandle, writeVal1, sizeof( writeVal1 ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        lRetVal = xSemaphoreTake( xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetTxNoOfbytes, &writeBytes );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
        /* Assert the number of bytes being written is 2. */
        TEST_ASSERT_EQUAL( sizeof( writeVal1 ), writeBytes );

        /* write the value to the device */
        lRetVal = iot_i2c_write_async( xI2CHandle, writeVal2, sizeof( writeVal2 ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        lRetVal = xSemaphoreTake( xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetTxNoOfbytes, &writeBytes );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
        /* Assert the number of written bytes. */
        TEST_ASSERT_EQUAL( sizeof( writeVal2 ), writeBytes );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Assisted Test Function for I2C async write
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CWriteAsyncAssisted )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t writeVal[ 16 ] = { uctestIotI2CDeviceRegister };
    char cMsg[ testIotI2C_MESSAGE_LENGTH ] = { 0 };
    size_t msgOffset = 0;
    int8_t i;
    size_t len;

    /* Generate bytes to write randomly */
    srand( xTaskGetTickCount() );

    /* Create a string with write bytes and print it to console later */
    for( i = 1, len = sizeof( writeVal ); i < len; i++ )
    {
        writeVal[ i ] = ( uint8_t ) rand();
    }

    prvAppendToMessage( &msgOffset, writeVal, sizeof( writeVal ), cMsg );

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set completion callback */
        iot_i2c_set_callback( xI2CHandle, prvI2CCallback, NULL );

        /* Set i2c congifuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &ucAssistedTestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Write the value to the device */
        lRetVal = iot_i2c_write_async( xI2CHandle, writeVal, sizeof( writeVal ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        lRetVal = xSemaphoreTake( xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

    TEST_IGNORE_MESSAGE( cMsg );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C async write fail to do ioctl
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CWriteAsyncFailIoctl )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t writeVal[ IOT_TEST_COMMON_IO_I2C_MAX_WRITE_LENGTH ] = { uctestIotI2CDeviceRegister, uctestIotI2CWriteVal };

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set completion callback */
        iot_i2c_set_callback( xI2CHandle, prvI2CCallback, NULL );

        /* Set i2c congifuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* wirte the value to the device */
        lRetVal = iot_i2c_write_async( xI2CHandle, writeVal, sizeof( writeVal ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c congifuration during transaction */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_BUSY, lRetVal );

        lRetVal = xSemaphoreTake( xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C async write twice fail
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CWriteAsyncFailWriteTwice )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t writeVal[ IOT_TEST_COMMON_IO_I2C_MAX_WRITE_LENGTH ] = { uctestIotI2CDeviceRegister, uctestIotI2CWriteVal };

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set completion callback */
        iot_i2c_set_callback( xI2CHandle, prvI2CCallback, NULL );

        /* Set i2c configuration. */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* write the value to the device */
        lRetVal = iot_i2c_write_async( xI2CHandle, writeVal, sizeof( writeVal ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* write the value to the device again */
        lRetVal = iot_i2c_write_async( xI2CHandle, writeVal, sizeof( writeVal ) );
        TEST_ASSERT_EQUAL( IOT_I2C_BUSY, lRetVal );

        lRetVal = xSemaphoreTake( xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C sync write then read, in same transaction.
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CWriteThenReadSyncSuccess )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t singleByte;

    uint16_t readBytes;
    uint16_t writeBytes;

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT,
    };

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSendNoStopFlag, NULL );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* write the device register address. */
        lRetVal = iot_i2c_write_sync( xI2CHandle, &uctestIotI2CDeviceRegister, sizeof( uctestIotI2CDeviceRegister ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* repeated start to read */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* read from i2c device for single byte */
        lRetVal = iot_i2c_read_sync( xI2CHandle, &singleByte, sizeof( singleByte ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetTxNoOfbytes, &writeBytes );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
        /* Assert the number of bytes being written is 1. */
        TEST_ASSERT_EQUAL( sizeof( uctestIotI2CDeviceRegister ), writeBytes );

        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetRxNoOfbytes, &readBytes );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
        /* Assert the number of bytes being read is 1. */
        TEST_ASSERT_EQUAL( sizeof( singleByte ), readBytes );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C sync read
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CReadSyncAssisted )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t ucReadBuf[ 16 ];
    char cMsg[ testIotI2C_MESSAGE_LENGTH ] = { 0 };
    size_t msgOffset = 0;

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT,
    };

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set i2c congifuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &ucAssistedTestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c congifuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSendNoStopFlag, NULL );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Write the device register address */
        lRetVal = iot_i2c_write_sync( xI2CHandle, &uctestIotI2CDeviceRegister, sizeof( uctestIotI2CDeviceRegister ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Repeated start to read */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &ucAssistedTestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Read from i2c device */
        lRetVal = iot_i2c_read_sync( xI2CHandle, ucReadBuf, sizeof( ucReadBuf ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

    /* Create a string with read bytes and print it to console */
    prvAppendToMessage( &msgOffset, ucReadBuf, sizeof( ucReadBuf ), cMsg );

    TEST_IGNORE_MESSAGE( cMsg );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C sync read Fail
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CReadSyncFail )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t ucReadValue;

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT,
    };

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* read from i2c device */
        lRetVal = iot_i2c_read_sync( xI2CHandle, &ucReadValue, sizeof( ucReadValue ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SLAVE_ADDRESS_NOT_SET, lRetVal );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C sync write
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CWriteSyncSuccess )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t writeVal1[] = { uctestIotI2CDeviceRegister, uctestIotI2CWriteVal };
    uint8_t writeVal2[ IOT_TEST_COMMON_IO_I2C_MAX_WRITE_LENGTH ] = { uctestIotI2CDeviceRegister, uctestIotI2CWriteVal };

    uint16_t writeBytes;

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* write the value to the device */
        lRetVal = iot_i2c_write_sync( xI2CHandle, writeVal1, sizeof( writeVal1 ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetTxNoOfbytes, &writeBytes );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
        /* Assert the number of bytes being written is 1. */
        TEST_ASSERT_EQUAL( sizeof( writeVal1 ), writeBytes );

        /* write the value to the device */
        lRetVal = iot_i2c_write_sync( xI2CHandle, writeVal2, sizeof( writeVal2 ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetTxNoOfbytes, &writeBytes );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
        /* Assert the number of bytes being written is 4. */
        TEST_ASSERT_EQUAL( sizeof( writeVal2 ), writeBytes );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Assisted Test Function for I2C sync write
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CWriteSyncAssisted )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t writeVal[ 16 ] = { uctestIotI2CDeviceRegister };
    char cMsg[ testIotI2C_MESSAGE_LENGTH ] = { 0 };
    size_t msgOffset = 0;
    int8_t i;
    size_t len;

    /* Generate bytes to write randomly */
    srand( xTaskGetTickCount() );

    /* Create a string with write bytes and print it to console later */
    for( i = 1, len = sizeof( writeVal ); i < len; i++ )
    {
        writeVal[ i ] = ( uint8_t ) rand();
    }

    prvAppendToMessage( &msgOffset, writeVal, sizeof( writeVal ), cMsg );

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set i2c congifuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &ucAssistedTestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Wirte the value to the device */
        lRetVal = iot_i2c_write_sync( xI2CHandle, writeVal, sizeof( writeVal ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

    TEST_IGNORE_MESSAGE( cMsg );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C sync write
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CWriteSyncFail )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t writeVal[] = { uctestIotI2CDeviceRegister, uctestIotI2CWriteVal };

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT,
    };

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* write to i2c device */
        lRetVal = iot_i2c_write_sync( xI2CHandle, writeVal, sizeof( writeVal ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SLAVE_ADDRESS_NOT_SET, lRetVal );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C but getState
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CGetBusStateSuccess )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t readBuffer[ 64 ];

    IotI2CBusStatus_t busStatus;

    IotI2CConfig_t xI2CConfig =
    {
        .ulBusFreq       = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        /* Set completion callback */
        iot_i2c_set_callback( xI2CHandle, prvI2CCallback, NULL );

        /* Set i2c configuration. */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c slave address */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Set i2c configuration */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSendNoStopFlag, NULL );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* write the device register address. */
        lRetVal = iot_i2c_write_sync( xI2CHandle, &uctestIotI2CDeviceRegister, sizeof( uctestIotI2CDeviceRegister ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Assert bus is idle before calling async read. */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetBusState, &busStatus );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eI2CBusIdle, busStatus );

        /* read from i2c device */
        lRetVal = iot_i2c_read_async( xI2CHandle, readBuffer, sizeof( readBuffer ) );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );

        /* Assert bus is busy during async read. */
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetBusState, &busStatus );
        TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
        TEST_ASSERT_EQUAL( eI2cBusBusy, busStatus );

        lRetVal = xSemaphoreTake( xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT );
        TEST_ASSERT_EQUAL( pdTRUE, lRetVal );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

TEST( TEST_IOT_I2C, AFQP_IotI2CSendNoStopUnsupported )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open( uctestIotI2CInstanceIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xI2CHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSendNoStopFlag, NULL );
        TEST_ASSERT_EQUAL( IOT_I2C_FUNCTION_NOT_SUPPORTED, lRetVal );
    }

    lRetVal = iot_i2c_close( xI2CHandle );
    TEST_ASSERT_EQUAL( IOT_I2C_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Append bytes in the buffer to _cMsg, starting at given offset.
 */

static void prvAppendToMessage( size_t * pOffset,
                                uint8_t * pBuffer,
                                size_t bufferLen,
                                char * cMsg )
{
    size_t i = 0;
    size_t offset = *pOffset;

    if( ( pOffset == NULL ) || ( pBuffer == NULL ) || ( cMsg == NULL ) )
    {
        return;
    }

    for( ; i < bufferLen && offset + 2 < testIotI2C_MESSAGE_LENGTH; i++ )
    {
        cMsg[ offset++ ] = ',';
        uint8_t upp = pBuffer[ i ] >> 4, low = pBuffer[ i ] & 0xF;
        cMsg[ offset++ ] = upp + ( upp > 9 ? 'A' - 10 : '0' );
        cMsg[ offset++ ] = low + ( low > 9 ? 'A' - 10 : '0' );
    }

    *pOffset = offset;
}
