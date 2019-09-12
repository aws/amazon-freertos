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

/*******************************************************************************
 * @file iot_test_i2c.c
 * @brief Functional Unit Test - I2C
 *******************************************************************************
 */

#include "unity.h"
#include "unity_fixture.h"

/* Driver includes */
#include "iot_i2c.h"

#include "FreeRTOS.h"
#include "semphr.h"

/*-----------------------------------------------------------*/
#define testIotI2C_BAUDRATE             IOT_I2C_FAST_MODE_BPS
#define testIotI2C_BAUDRATE_LOW_SPEED   IOT_I2C_STANDARD_MODE_BPS
#define testIotI2C_INSTANCE             uctestIotI2CInstanceIdx
#define testIotI2C_INSTANCE_NUM         uctestIotI2CInstanceNum
#define testIotI2C_DEFAULT_TIMEOUT      500 /**< 500 msec */
#define testIotI2C_FAST_TIMEOUT         100 /**< 100 msec */
#define testIotI2C_INVALID_IOCTL_INDEX  UINT32_MAX
#define testIotI2C_HANDLE_NUM           4
#define testIotI2C_MAX_TIMEOUT          pdMS_TO_TICKS(10000)
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/* Globals values which can be overwritten by the test
 * framework invoking these tests */
/*-----------------------------------------------------------*/

uint8_t uctestIotI2CSlaveAddr = 0;              /**< The slave address to be set for the I2C port. */
uint8_t uctestIotI2CInvalidSlaveAddr = 0xFF;    /**< The slave address to be set for the I2C port. */
uint8_t xtestIotI2CDeviceRegister = 0;          /**< The device register to be set for the I2C port. */
uint8_t uctestIotI2CWriteVal = 0;               /**< The write value to write to device. */
uint8_t uctestIotI2CInstanceIdx = 0;            /**< The current I2C test instance index */
uint8_t uctestIotI2CInstanceNum = 1;            /**< The total I2C test instance number */

extern IotI2CHandle_t gIotI2cHandle[testIotI2C_HANDLE_NUM];

/*-----------------------------------------------------------*/
/* Static Globals */
/*-----------------------------------------------------------*/
static SemaphoreHandle_t xtestIotI2CSemaphore = NULL;

/*-----------------------------------------------------------*/

/* Define Test Group. */
TEST_GROUP( TEST_IOT_I2C );

/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_I2C )
{
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
void prvI2CCallback(IotI2COperationStatus_t xOpStatus, void * pvParam)
{
    BaseType_t xHigherPriorityTaskWoken;

    if( xOpStatus == eI2CCompleted ){
        xSemaphoreGiveFromISR(xtestIotI2CSemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/*-----------------------------------------------------------*/
/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_I2C )
{
    xtestIotI2CSemaphore = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_EQUAL(NULL, xtestIotI2CSemaphore);

    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2COpenCloseSuccess );
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2COpenCloseFail );
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2COpenCloseFailUnsupportInst );
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CSetGetConfigurationSuccess );
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CSetGetConfigurationFail );
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CWriteAsyncSuccess);
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CWriteAsyncFailIoctl);
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CWriteAsyncFailWriteTwice);
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CReadAsyncSuccess);
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CReadAsyncFailIoctl);
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CReadAsyncFailReadTwice);
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CReadAsyncFailSetAddr);
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CCancelSuccess);
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CCancelFail);
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CWriteSyncSuccess);
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CWriteSyncFail);
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CReadSyncSuccess);
    RUN_TEST_CASE( TEST_IOT_I2C, AFQP_IotI2CReadSyncFail);
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
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    gIotI2cHandle[testIotI2C_INSTANCE] = NULL;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_i2c_open and iot_i2c_close fail case.
 */
TEST( TEST_IOT_I2C, AFQP_IotI2COpenCloseFail )
{
    IotI2CHandle_t xI2CHandle;
    IotI2CHandle_t xI2CHandle_1;
    int32_t lRetVal;

    /* Open i2c to initialize hardware */
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    /* Open i2c to initialize hardware again */
    xI2CHandle_1 = iot_i2c_open(testIotI2C_INSTANCE);
    TEST_ASSERT_EQUAL(NULL, xI2CHandle_1);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Close same i2c again which is NULL */
    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_INVALID_VALUE, lRetVal);

    /* Close i2c with NULL pointer */
    lRetVal = iot_i2c_close(xI2CHandle_1);
    TEST_ASSERT_EQUAL(IOT_I2C_INVALID_VALUE, lRetVal);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_i2c_open and iot_i2c_close with unsupport instance
 */
TEST( TEST_IOT_I2C, AFQP_IotI2COpenCloseFailUnsupportInst )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;

    /* Open i2c to initialize hardware */
    xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE_NUM);
    TEST_ASSERT_EQUAL(NULL, xI2CHandle);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_INVALID_VALUE, lRetVal);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C set and get congirutation success
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
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    /* Set default i2c congifuration */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetMasterConfig, &xI2CConfig_write);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Get i2c congifuration */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CGetMasterConfig, &xI2CConfig_read);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Make sure get the correct config from i2c */
    TEST_ASSERT_EQUAL(testIotI2C_BAUDRATE, xI2CConfig_read.ulBusFreq);
    TEST_ASSERT_EQUAL(testIotI2C_DEFAULT_TIMEOUT, xI2CConfig_read.ulMasterTimeout);

    xI2CConfig_write.ulBusFreq = testIotI2C_BAUDRATE_LOW_SPEED;
    xI2CConfig_write.ulMasterTimeout = testIotI2C_FAST_TIMEOUT;

    /* Set new i2c congifuration */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetMasterConfig, &xI2CConfig_write);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Get new i2c congifuration */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CGetMasterConfig, &xI2CConfig_read);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Make sure get the correct config from i2c */
    TEST_ASSERT_EQUAL(testIotI2C_BAUDRATE_LOW_SPEED, xI2CConfig_read.ulBusFreq);
    TEST_ASSERT_EQUAL(testIotI2C_FAST_TIMEOUT, xI2CConfig_read.ulMasterTimeout);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C set and get congirutation fail
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CSetGetConfigurationFail)
{
    IotI2CHandle_t xI2CHandle;
    IotI2CConfig_t xI2CConfig;
    int32_t lRetVal;

    /* Open i2c to initialize hardware */
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    /* Set default i2c congifuration with NULL pointer */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetMasterConfig, NULL);
    TEST_ASSERT_EQUAL(IOT_I2C_INVALID_VALUE, lRetVal);

    /* Get i2c congifuration with NULL pointer */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CGetMasterConfig, NULL);
    TEST_ASSERT_EQUAL(IOT_I2C_INVALID_VALUE, lRetVal);

    /* i2c ioctl with unsupport request */
    lRetVal = iot_i2c_ioctl(xI2CHandle, testIotI2C_INVALID_IOCTL_INDEX, &xI2CConfig);
    TEST_ASSERT_EQUAL(IOT_I2C_INVALID_VALUE, lRetVal);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C async read success
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CReadAsyncSuccess )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t ret_val;

    IotI2CConfig_t xI2CConfig = {
        .ulBusFreq = testIotI2C_BAUDRATE,
        .ulMasterTimeout =  testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    /* Set completion callback */
    iot_i2c_set_callback(xI2CHandle, prvI2CCallback, NULL );

    /* Set i2c congifuration */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetMasterConfig, &xI2CConfig);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c slave address for writing the device register */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c not stop between transaction */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSendNoStopFlag, NULL);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* write the device register address. */
    lRetVal = iot_i2c_write_sync(xI2CHandle, &xtestIotI2CDeviceRegister, sizeof(xtestIotI2CDeviceRegister));
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c slave address to read from the device register. */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* read from i2c device */
    lRetVal = iot_i2c_read_async(xI2CHandle, &ret_val, sizeof(ret_val));
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    lRetVal = xSemaphoreTake(xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT);
    TEST_ASSERT_EQUAL(pdTRUE, lRetVal);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C async read fail to do ioctl
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CReadAsyncFailIoctl )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t ret_val[10];

    IotI2CConfig_t xI2CConfig = {
        .ulBusFreq = testIotI2C_BAUDRATE,
        .ulMasterTimeout =  testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    /* Set completion callback */
    iot_i2c_set_callback(xI2CHandle, prvI2CCallback, NULL );

    /* Set i2c congifuration */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetMasterConfig, &xI2CConfig);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c slave address */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c not stop between transaction */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSendNoStopFlag, NULL);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* write the device register address. */
    lRetVal = iot_i2c_write_sync(xI2CHandle, &xtestIotI2CDeviceRegister, sizeof(xtestIotI2CDeviceRegister));
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c slave address */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c not stop between transaction */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSendNoStopFlag, NULL);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* read from i2c device register */
    lRetVal = iot_i2c_read_async(xI2CHandle, ret_val, sizeof(ret_val));
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c congifuration during transaction */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetMasterConfig, &xI2CConfig);
    TEST_ASSERT_EQUAL(IOT_I2C_BUSY, lRetVal);

    lRetVal = xSemaphoreTake(xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT);
    TEST_ASSERT_EQUAL(pdTRUE, lRetVal);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C async read twice fail
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CReadAsyncFailReadTwice )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t ret_val[10];

    IotI2CConfig_t xI2CConfig = {
        .ulBusFreq = testIotI2C_BAUDRATE,
        .ulMasterTimeout =  testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    /* Set completion callback */
    iot_i2c_set_callback(xI2CHandle, prvI2CCallback, NULL );

    /* Set i2c congifuration */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetMasterConfig, &xI2CConfig);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c slave address */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c not stop between transaction */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSendNoStopFlag, NULL);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* write the device register address. */
    lRetVal = iot_i2c_write_sync(xI2CHandle, &xtestIotI2CDeviceRegister, sizeof(xtestIotI2CDeviceRegister));
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c slave address */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c not stop between transaction */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSendNoStopFlag, NULL);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);\

    /* read from i2c device */
    lRetVal = iot_i2c_read_async(xI2CHandle, ret_val, sizeof(ret_val));
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* read from i2c device again */
    lRetVal = iot_i2c_read_async(xI2CHandle, ret_val, sizeof(ret_val));
    TEST_ASSERT_EQUAL(IOT_I2C_BUSY, lRetVal);

    lRetVal = xSemaphoreTake(xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT);
    TEST_ASSERT_EQUAL(pdTRUE, lRetVal);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C read async fail. slave address and register not set.
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CReadAsyncFailSetAddr)
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t ret_val;

    IotI2CConfig_t xI2CConfig = {
        .ulBusFreq = testIotI2C_BAUDRATE,
        .ulMasterTimeout =  testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    /* Set completion callback */
    iot_i2c_set_callback(xI2CHandle, prvI2CCallback, NULL );

    /* read from i2c device */
    lRetVal = iot_i2c_read_async(xI2CHandle, &ret_val, sizeof(ret_val));
    TEST_ASSERT_EQUAL(IOT_I2C_SLAVE_ADDRESS_NOT_SET, lRetVal);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C cancel
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CCancelSuccess )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t ret_val;

    IotI2CConfig_t xI2CConfig = {
        .ulBusFreq = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    /* Set completion callback */
    iot_i2c_set_callback(xI2CHandle, prvI2CCallback, NULL );

    /* Set i2c congifuration */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetMasterConfig, &xI2CConfig);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c slave address */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c not stop between transaction */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSendNoStopFlag, NULL);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* write the device register address. */
    lRetVal = iot_i2c_write_sync(xI2CHandle, &xtestIotI2CDeviceRegister, sizeof(xtestIotI2CDeviceRegister));
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c slave address */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* read from i2c device */
    lRetVal = iot_i2c_read_async(xI2CHandle, &ret_val, sizeof(ret_val));
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    lRetVal = iot_i2c_cancel(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C cancel fail
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CCancelFail )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t ret_val;

    IotI2CConfig_t xI2CConfig = {
        .ulBusFreq = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    /* cancel transaction with NULL */
    lRetVal = iot_i2c_cancel(NULL);
    TEST_ASSERT_EQUAL(IOT_I2C_INVALID_VALUE, lRetVal);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C async write
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CWriteAsyncSuccess )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t writeVal[] = {xtestIotI2CDeviceRegister, uctestIotI2CWriteVal};

    IotI2CConfig_t xI2CConfig = {
        .ulBusFreq = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    /* Set completion callback */
    iot_i2c_set_callback(xI2CHandle, prvI2CCallback, NULL );

    /* Set i2c congifuration */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetMasterConfig, &xI2CConfig);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c slave address */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* write the value to the device */
    lRetVal = iot_i2c_write_async(xI2CHandle, writeVal, sizeof(writeVal));
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    lRetVal = xSemaphoreTake(xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT);
    TEST_ASSERT_EQUAL(pdTRUE, lRetVal);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C async write fail to do ioctl
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CWriteAsyncFailIoctl)
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t writeVal[] = {xtestIotI2CDeviceRegister, uctestIotI2CWriteVal, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    IotI2CConfig_t xI2CConfig = {
        .ulBusFreq = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    /* Set completion callback */
    iot_i2c_set_callback(xI2CHandle, prvI2CCallback, NULL );

    /* Set i2c congifuration */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetMasterConfig, &xI2CConfig);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c slave address */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c not stop between transaction */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSendNoStopFlag, NULL);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* wirte the value to the device */
    lRetVal = iot_i2c_write_async(xI2CHandle, writeVal, sizeof(writeVal));
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c congifuration during transaction */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetMasterConfig, &xI2CConfig);
    TEST_ASSERT_EQUAL(IOT_I2C_BUSY, lRetVal);

    lRetVal = xSemaphoreTake(xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT);
    TEST_ASSERT_EQUAL(pdTRUE, lRetVal);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C async write twice fail
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CWriteAsyncFailWriteTwice )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t writeVal[] = {xtestIotI2CDeviceRegister, uctestIotI2CWriteVal, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    IotI2CConfig_t xI2CConfig = {
        .ulBusFreq = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    /* Set completion callback */
    iot_i2c_set_callback(xI2CHandle, prvI2CCallback, NULL );

    /* Set i2c congifuration */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetMasterConfig, &xI2CConfig);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c slave address */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c not stop between transaction */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSendNoStopFlag, NULL);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* wirte the value to the device */
    lRetVal = iot_i2c_write_async(xI2CHandle, writeVal, sizeof(writeVal));
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* wirte the value to the device again */
    lRetVal = iot_i2c_write_async(xI2CHandle, writeVal, sizeof(writeVal));
    TEST_ASSERT_EQUAL(IOT_I2C_BUSY, lRetVal);

    lRetVal = xSemaphoreTake(xtestIotI2CSemaphore, testIotI2C_MAX_TIMEOUT);
    TEST_ASSERT_EQUAL(pdTRUE, lRetVal);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C sync read
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CReadSyncSuccess )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t ret_val;

    IotI2CConfig_t xI2CConfig = {
        .ulBusFreq = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT,
    };

    /* Open i2c to initialize hardware */
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    /* Set i2c congifuration */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetMasterConfig, &xI2CConfig);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c slave address */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c congifuration */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSendNoStopFlag, NULL);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* write the device register address. */
    lRetVal = iot_i2c_write_sync(xI2CHandle, &xtestIotI2CDeviceRegister, sizeof(xtestIotI2CDeviceRegister));
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* repeated start to read */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* read from i2c device */
    lRetVal = iot_i2c_read_sync(xI2CHandle, &ret_val, sizeof(ret_val));
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C sync read Fail
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CReadSyncFail )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t ret_val;

    IotI2CConfig_t xI2CConfig = {
        .ulBusFreq = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT,
    };

    /* Open i2c to initialize hardware */
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetMasterConfig, &xI2CConfig);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* read from i2c device */
    lRetVal = iot_i2c_read_sync(xI2CHandle, &ret_val, sizeof(ret_val));
    TEST_ASSERT_EQUAL(IOT_I2C_SLAVE_ADDRESS_NOT_SET, lRetVal);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C sync write
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CWriteSyncSuccess )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t writeVal[] = {xtestIotI2CDeviceRegister, uctestIotI2CWriteVal};

    IotI2CConfig_t xI2CConfig = {
        .ulBusFreq = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT
    };

    /* Open i2c to initialize hardware */
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    /* Set i2c configuration */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetMasterConfig, &xI2CConfig);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* Set i2c slave address */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* write the value to the device */
    lRetVal = iot_i2c_write_sync(xI2CHandle, writeVal, sizeof(writeVal));
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for I2C sync write
 */
TEST( TEST_IOT_I2C, AFQP_IotI2CWriteSyncFail )
{
    IotI2CHandle_t xI2CHandle;
    int32_t lRetVal;
    uint8_t writeVal[] = {xtestIotI2CDeviceRegister, uctestIotI2CWriteVal};

    IotI2CConfig_t xI2CConfig = {
        .ulBusFreq = testIotI2C_BAUDRATE,
        .ulMasterTimeout = testIotI2C_DEFAULT_TIMEOUT,
    };

    /* Open i2c to initialize hardware */
    if (gIotI2cHandle[testIotI2C_INSTANCE] == NULL)
    {
        xI2CHandle = iot_i2c_open(testIotI2C_INSTANCE);
        TEST_ASSERT_NOT_EQUAL(NULL, xI2CHandle);
    }
    else
    {
        xI2CHandle = gIotI2cHandle[testIotI2C_INSTANCE];
    }

    /* Set i2c congifuration */
    lRetVal = iot_i2c_ioctl(xI2CHandle, eI2CSetMasterConfig, &xI2CConfig);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);

    /* write to i2c device */
    lRetVal = iot_i2c_write_sync(xI2CHandle, writeVal, sizeof(writeVal));
    TEST_ASSERT_EQUAL(IOT_I2C_SLAVE_ADDRESS_NOT_SET, lRetVal);

    lRetVal = iot_i2c_close(xI2CHandle);
    TEST_ASSERT_EQUAL(IOT_I2C_SUCCESS, lRetVal);
}
/*-----------------------------------------------------------*/
