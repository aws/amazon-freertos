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

/*******************************************************************************
 * IOT On-Target Unit Test
 *******************************************************************************
 */
#include <stdio.h>

/* Test runner interface includes. */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Unity framework includes. */
#include "unity_fixture.h"
#include "unity_internals.h"

/* Board config includes. */
#include "test_iot_internal.h"

static void RUN_TEST_IOT_UART(void)
{
    int i;
    for(i = 0; i < UART_TEST_SET; i++)
    {
        SET_TEST_IOT_UART_CONFIG(i);
        RUN_TEST_GROUP( TEST_IOT_UART );
    }
}

static void RUN_TEST_IOT_FLASH(void)
{
    int i;
    for(i = 0; i < FLASH_TEST_SET; i++)
    {
        SET_TEST_IOT_FLASH_CONFIG(i);
        RUN_TEST_GROUP( TEST_IOT_FLASH );
    }
}

static void RUN_TEST_IOT_WATCHDOG(int testCaseIndex)
{
    int i;
    for(i = 0; i < WATCHDOG_TEST_SET; i++)
    {
        SET_TEST_IOT_WATCHDOG_CONFIG(i);
            switch (testCaseIndex)
            {
                case 0:
                    RUN_TEST_CASE(TEST_IOT_WATCHDOG, AFQP_IotWatchdogOpenClose);
                    RUN_TEST_CASE(TEST_IOT_WATCHDOG, AFQP_IotWatchdogValidateBarkTimer);
                    RUN_TEST_CASE(TEST_IOT_WATCHDOG, AFQP_IotWatchdogValidateBiteInterrupt);
                    RUN_TEST_CASE(TEST_IOT_WATCHDOG, AFQP_IotWatchdogStartNoTimerSet);
                    RUN_TEST_CASE(TEST_IOT_WATCHDOG, AFQP_IotWatchdogRestartNoStart);
                    RUN_TEST_CASE(TEST_IOT_WATCHDOG, AFQP_IotWatchdogSetBarkGreaterThanBiteSetWatchdogBarkFirst);
                    RUN_TEST_CASE(TEST_IOT_WATCHDOG, AFQP_IotWatchdogSetBarkGreaterThanBiteSetWatchdogBiteFirst);
                    RUN_TEST_CASE(TEST_IOT_WATCHDOG, AFQP_IotWatchdogOpenOpenClose);
                    RUN_TEST_CASE(TEST_IOT_WATCHDOG, AFQP_Fuzz_IotWatchdogClose);
                    RUN_TEST_CASE(TEST_IOT_WATCHDOG, AFQP_Fuzz_IotWatchdogStartStop);
                    RUN_TEST_CASE(TEST_IOT_WATCHDOG, AFQP_Fuzz_IotWatchdog_ioctl);
                    RUN_TEST_CASE(TEST_IOT_WATCHDOG, AFQP_IotWatchdogOpenCloseClose);
                    break;
                case 1:
                    RUN_TEST_CASE( TEST_IOT_WATCHDOG, AFQP_IotWatchdogValidateWatchdogReset);
                    break;
                case 2:
                    RUN_TEST_CASE( TEST_IOT_WATCHDOG, AFQP_IotWatchdogValidateResetReason);
                    break;
                default:
                    RUN_TEST_GROUP( TEST_IOT_WATCHDOG );
                    return;
        }
    }
}


static void RUN_TEST_IOT_GPIO(int testCaseIndex)
{
    int i;
    for(i = 0; i < GPIO_TEST_SET; i++)
    {
        SET_TEST_IOT_GPIO_CONFIG(i);
            switch (testCaseIndex)
            {
                case 0:
                    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioOpenClose );
                    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioOpenOpenClose );
                    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioOpenCloseClose );
                    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioIoctlSetGet );
                    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioMode );
                    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioPull );
                    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioSpeed );
                    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioOperation );
                    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioOpenCloseFuzz );
                    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioReadWriteSyncFuzz );
                    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioIoctlFuzz );
                    RUN_TEST_CASE( TEST_IOT_GPIO, AFQP_IotGpioInterrupt );
                    break;
                case 1:
                    RUN_TEST_CASE(TEST_IOT_GPIO, AFQP_AssistedIotGpioModeWritePushPullTrue);
                    break;
                case 2:
                    RUN_TEST_CASE(TEST_IOT_GPIO, AFQP_AssistedIotGpioModeWritePushPullFalse);
                    break;
                case 3:
                    RUN_TEST_CASE(TEST_IOT_GPIO, AFQP_AssistedIotGpioModeWriteOpenDrainTrue);
                    break;
                case 4:
                    RUN_TEST_CASE(TEST_IOT_GPIO, AFQP_AssistedIotGpioModeWriteOpenDrainFalse);
                    break;
                case 5:
                    RUN_TEST_CASE(TEST_IOT_GPIO, AFQP_AssistedIotGpioModeReadTrue);
                    break;
                case 6:
                    RUN_TEST_CASE(TEST_IOT_GPIO, AFQP_AssistedIotGpioModeReadFalse);
                    break;
                default:
                    RUN_TEST_GROUP( TEST_IOT_GPIO );
                    return;
        }
    }
}

static void RUN_TEST_IOT_RTC(void)
{
    int i;
    for(i = 0; i < RTC_TEST_SET; i++)
    {
        SET_TEST_IOT_RTC_CONFIG(i);
        RUN_TEST_GROUP( TEST_IOT_RTC );
    }
}

static void RUN_TEST_IOT_TIMER(void)
{
    int i;

    for(i = 0; i < TIMER_TEST_SET; i++)
    {
        SET_TEST_IOT_TIMER_CONFIG(i);
        RUN_TEST_GROUP( TEST_IOT_TIMER );
    }
}

static void RUN_TEST_IOT_ADC(void)
{
    int i;
    for(i = 0; i < ADC_TEST_SET; i++)
    {
        SET_TEST_IOT_ADC_CONFIG(i);
        RUN_TEST_GROUP( TEST_IOT_ADC );
    }
}

static void RUN_TEST_IOT_RESET(int testCaseIndex)
{
    int i;
    for(i = 0; i < RESET_TEST_SET; i++)
    {
        SET_TEST_IOT_RESET_CONFIG(i);
        switch (testCaseIndex)
        {
            case 0:
                RUN_TEST_CASE( TEST_IOT_RESET, AFQP_IotResetShutdown );
                break;
            case 1:
                RUN_TEST_CASE( TEST_IOT_RESET, AFQP_IotResetRebootZero );
                break;
            case 2:
                RUN_TEST_CASE( TEST_IOT_RESET, AFQP_IotResetRebootNonZero );
                break;
            default:
                printf("Invalid reset test case\n");
                return;
        }
    }
}

static void RUN_TEST_IOT_PERFCOUNTER(void)
{
    int i;
    for(i = 0; i < PERFCOUNTER_TEST_SET; i++)
    {
        SET_TEST_IOT_PERFCOUNTER_CONFIG(i);
        RUN_TEST_GROUP( TEST_IOT_PERFCOUNTER );
    }
}

static void RUN_TEST_IOT_PWM(int testCaseIndex)
{
    int i;
    for(i = 0; i < PWM_TEST_SET; i++)
    {
        SET_TEST_IOT_PWM_CONFIG(i);
        switch (testCaseIndex)
        {
            case 0:
                RUN_TEST_GROUP( TEST_IOT_PWM );
                break;
            case 1:
                RUN_TEST_CASE( TEST_IOT_PWM, AFQP_IotPwm_Start );
                break;
            case 2:
                RUN_TEST_CASE( TEST_IOT_PWM, AFQP_IotPwmAccuracy );
                break;
            default:
                printf("Invalid reset test case\n");
                return;
        }
    }
}

static void RUN_TEST_IOT_I2C(void)
{
    int i;
    for(i = 0; i < I2C_TEST_SET; i++)
    {
        SET_TEST_IOT_I2C_CONFIG(i);
        RUN_TEST_GROUP( TEST_IOT_I2C );
    }
}

static void RUN_IOT_TEST_TSENSOR(void)
{
    int i;
    for(i = 0; i < TEMP_SENSOR_TEST_SET; i++)
    {
        SET_TEST_IOT_TEMP_SENSOR_CONFIG(i);
        RUN_TEST_GROUP( TEST_IOT_TSENSOR );
    }
}

static void RUN_IOT_TEST_POWER(int testCaseIndex)
{
    int i;
    for(i = 0; i < POWER_TEST_SET; i++)
    {
        SET_TEST_IOT_POWER_CONFIG(i);
        switch (testCaseIndex)
        {
            case 0:
                RUN_TEST_GROUP( TEST_IOT_POWER );
                break;
            case 1:
                RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_VddOffWakeThresholdTest );
                break;
            default:
                RUN_TEST_GROUP( TEST_IOT_POWER );
                return;
        }

    }
}

static void RUN_IOT_TEST_BATTERY(void)
{
    int i;
    for(i = 0; i < BATTERY_TEST_SET; i++)
    {
        RUN_TEST_GROUP( TEST_IOT_BATTERY );
    }
}

static void RUN_IOT_TEST_EFUSE(void)
{
    int i;
    for(i = 0; i < EFUSE_TEST_SET; i++)
    {
        SET_TEST_IOT_EFUSE_CONFIG(i);
        RUN_TEST_GROUP( TEST_IOT_EFUSE );
    }
}

int slave_transfer_sync_test(int);
static void RUN_IOT_TEST_SPI(int testCaseIndex)
{
    int rc = 0;

    SET_TEST_IOT_SPI_CONFIG(0);

    switch (testCaseIndex) {
       /* Assisted tests assume a connection with 2 Devices.  One acting as Master
          the other acting as Slave. This means that MOSI, MISO, CLK, and CS lines
          are connecting the two devices together.  Once this is done the setup is
          to run the Slave code first, and then the corresponding Master.
          The slave operations are defined as 0 = Read, 1 = Write, and 2 = Transfer.
          ex:  Calling iot_tests test 16 11    to start the Slave to Write and then
               Calling iot_tests test 16 1     to have the Master perform the Read.
       */
       case 0:    RUN_TEST_GROUP( TEST_IOT_SPI );                           break;
       case 1:    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_ReadSync );      break;
       case 2:    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_WriteSync );     break;
       case 3:    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_TransferSync );  break;
       case 4:    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_ReadAsync );      break;
       case 5:    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_WriteAsync );     break;
       case 6:    RUN_TEST_CASE( TEST_IOT_SPI, AFQP_IotSPI_TransferAsync );  break;
#ifdef BOARD_STARLITE
       case 10:    rc = slave_transfer_sync_test(0);                         break;
       case 11:    rc = slave_transfer_sync_test(1);                         break;
       case 12:    rc = slave_transfer_sync_test(2);                         break;
#endif
       default:   rc = -1;
    }
    TEST_ASSERT_EQUAL( rc, 0 );
}

static void RUN_TEST_IOT_ALL_GROUP(void){
    /* FWPLATFORM-730 to re-enable uart test
      RUN_TEST_IOT_UART();
    */
    RUN_IOT_TEST_EFUSE();
    /* ACE-9604 */
    //RUN_TEST_IOT_FLASH();
    RUN_TEST_IOT_RTC();
    RUN_TEST_IOT_GPIO(0);
    RUN_TEST_IOT_TIMER();
    RUN_TEST_IOT_WATCHDOG(0);
    RUN_TEST_IOT_PERFCOUNTER();
    RUN_TEST_IOT_PWM(0);
    RUN_TEST_IOT_I2C();
    RUN_IOT_TEST_TSENSOR();
    RUN_IOT_TEST_BATTERY();
    /* FWPLATFORM-1096  Power tests fail.
       temporarilly call tests individually to skip over tests that
       are failing.  Replace with RUN_IOT_TEST_POWER() once fixed
    */
    TEST_TEST_IOT_POWER_SETUP( );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_OpenClose );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_SetModeHigh );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_SetModeNormal );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_SetModeLow );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_EnterIdle );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_EnterIdleCancel );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_IoctlEnum );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_SetModeFuzzing );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_ResetModeFuzzing );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_IoctlFuzzing );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_GetModeFuzzing );
    RUN_TEST_CASE( TEST_IOT_POWER, AFQP_IotPower_CloseFuzzing );
    RUN_IOT_TEST_SPI(0);
}

void RunIotTests(int testIndex, int testCaseIndex)
{
    /* Initialize unity. */
    UnityFixture.Verbose = 1;

    UNITY_BEGIN();

    /* Give the print buffer time to empty */
    // vTaskDelay( 500 );

    switch (testIndex)
    {
        case 0:
            RUN_TEST_IOT_ALL_GROUP();
            break;
        case 1:
            RUN_TEST_IOT_UART();
            break;
        case 2:
            RUN_TEST_IOT_FLASH();
            break;
        case 3:
            RUN_TEST_IOT_WATCHDOG(testCaseIndex);
            break;
        case 4:
            RUN_TEST_IOT_RTC();
            break;
        case 5:
            RUN_TEST_IOT_GPIO(testCaseIndex);
            break;
        case 6:
            RUN_TEST_IOT_TIMER();
            break;
        case 7:
            RUN_TEST_IOT_ADC();
            break;
        case 8:
            RUN_TEST_IOT_RESET(testCaseIndex);
            break;
        case 9:
            RUN_TEST_IOT_PERFCOUNTER();
            break;
        case 10:
            RUN_TEST_IOT_PWM(testCaseIndex);
            break;
        case 11:
            RUN_TEST_IOT_I2C();
            break;
        case 12:
            RUN_IOT_TEST_TSENSOR();
            break;
        case 13:
            RUN_IOT_TEST_POWER(testCaseIndex);
            break;

        case 14:
            RUN_IOT_TEST_BATTERY();
            break;

        case 15:
            RUN_IOT_TEST_EFUSE();
            break;

        case 16:
            RUN_IOT_TEST_SPI(testCaseIndex);
            break;
        default:
            printf("Test index is wrong\r\n");
            return;
    }

    /* Currently disabled. Will be enabled after cleanup. */
    UNITY_END();

    // #ifdef CODE_COVERAGE
    //     exit( 0 );
    // #endif

    /* This task has finished.  FreeRTOS does not allow a task to run off the
     * end of its implementing function, so the task must be deleted. */
    // vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/
