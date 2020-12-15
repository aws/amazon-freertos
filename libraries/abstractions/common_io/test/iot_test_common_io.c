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

    #ifdef IOT_TEST_COMMON_IO_PERFCOUNTER_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_PERFCOUNTER_SUPPORTED; i++ )
        {
            SET_TEST_IOT_PERFCOUNTER_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_PERFCOUNTER );
        }
    #endif

    /* These already used loop back tests which require minimum of two pins */
    #ifdef IOT_TEST_COMMON_IO_GPIO_SUPPORTED
        for( i = 1; i < IOT_TEST_COMMON_IO_GPIO_SUPPORTED; i++ )
        {
            SET_TEST_IOT_GPIO_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_GPIO )
        }
    #endif

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

    #ifdef IOT_TEST_COMMON_IO_WATCHDOG_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_WATCHDOG_SUPPORTED; i++ )
        {
            SET_TEST_IOT_WATCHDOG_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_WATCHDOG );
        }
    #endif

    #ifdef IOT_TEST_COMMON_IO_RTC_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_RTC_SUPPORTED; i++ )
        {
            SET_TEST_IOT_RTC_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_RTC );
        }
    #endif

    #ifdef IOT_TEST_COMMON_IO_RESET_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_RESET_SUPPORTED; i++ )
        {
            SET_TEST_IOT_RESET_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_RESET );
        }
    #endif

    #ifdef IOT_TEST_COMMON_IO_TIMER_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_TIMER_SUPPORTED; i++ )
        {
            SET_TEST_IOT_TIMER_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_TIMER );
        }
    #endif

    #ifdef IOT_TEST_COMMON_IO_FLASH_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_FLASH_SUPPORTED; i++ )
        {
            SET_TEST_IOT_FLASH_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_FLASH );
        }
    #endif

    #ifdef IOT_TEST_COMMON_IO_ADC_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_ADC_SUPPORTED; i++ )
        {
            SET_TEST_IOT_ADC_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_ADC );
        }
    #endif

    #ifdef IOT_TEST_COMMON_IO_PWM_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_PWM_SUPPORTED; i++ )
        {
            SET_TEST_IOT_PWM_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_PWM );
        }
    #endif

    #ifdef IOT_TEST_COMMON_IO_I2S_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_I2S_SUPPORTED; i++ )
        {
            SET_TEST_IOT_I2S_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_I2S );
        }
    #endif

    #ifdef IOT_TEST_COMMON_IO_EFUSE_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_EFUSE_SUPPORTED; i++ )
        {
            SET_TEST_IOT_EFUSE_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_EFUSE );
        }
    #endif

    #ifdef IOT_TEST_COMMON_IO_SDIO_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_SDIO_SUPPORTED; i++ )
        {
            SET_TEST_IOT_SDIO_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_SDIO );
        }
    #endif

    #ifdef IOT_TEST_COMMON_IO_TEMP_SENSOR_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_TEMP_SENSOR_SUPPORTED; i++ )
        {
            SET_TEST_IOT_TEMP_SENSOR_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_TSENSOR );
        }
    #endif

    #ifdef IOT_TEST_COMMON_IO_POWER_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_POWER_SUPPORTED; i++ )
        {
            SET_TEST_IOT_POWER_CONFIG( i );
            RUN_TEST_GROUP( TEST_IOT_POWER );
        }
    #endif

    #ifdef IOT_TEST_COMMON_IO_USB_DEVICE_SUPPORTED
        for( i = 0; i < IOT_TEST_COMMON_IO_USB_DEVICE_SUPPORTED; i++ )
        {
            SET_TEST_IOT_USB_DEVICE_CONFIG( i );
            /* No tests currently. */
        }
    #endif
}
