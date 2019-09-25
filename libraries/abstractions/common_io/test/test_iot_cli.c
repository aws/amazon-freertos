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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_iot_main.h"

#include "ace_cli.h"
#include <ace/aceCli.h>

ace_status_t runIotTests(int32_t argc, const char** argv) {
    if (argc < 1)
    {
        printf("Expected at least 1 arg\n");
        return ACE_STATUS_CLI_FUNC_ERROR;
    }

    int testIndex = ((int)atoi(argv[0]));
    int testCaseIndex = ((int)atoi(argv[1]));
    RunIotTests(testIndex, testCaseIndex);
    return ACE_STATUS_OK;
}

const aceCli_moduleCmd_t iot_tests_cli_sub[] = {
                            {"test", "<num>\n\t0: all\n\t"
                            "1: uart <num> \n\t\t0: All Independent tests\n\t\t1: AFQP_AssistedIotUARTReadWriteSync\n\t\t2: AFQP_AssistedIotUARTBaudChange\n\t\t3: AFQP_AssistedIotUARTReadWriteAsync\n\t"
                            "2: flash\n\t"
                            "3: wdog <num> \n\t\t0: All Non-Reset tests\n\t\t1: Bite-Reset test\n\t\t2: ResetReason Test\n\t"
                            "4: rtc\n\t"
                            "5: gpio <num> \n\t\t0: All Independent tests\n\t\t1: AFQP_AssistedIotGpioModeWritePushPullTrue\n\t\t2: AFQP_AssistedIotGpioModeWritePushPullFalse\n\t\t3: AFQP_AssistedIotGpioModeWriteOpenDrainTrue\n\t\t4: AFQP_AssistedIotGpioModeWriteOpenDrainFalse\n\t\t5: AFQP_AssistedIotGpioModeReadTrue\n\t\t6: AFQP_AssistedIotGpioModeReadFalse\n\t"
                            "6: timer\n\t"
                            "7: adc <num> \n\t\t0: All Independent tests\n\t\t1: AFQP_IotAdcPrintReadSample\n\t"
                            "8: reset <num>\n\t\t0: Shutdown\n\t\t1: RebootZero\n\t\t2: RebootNonZero"
                            "\n\t9: perfcounter\n\t10: pwm\n\t11: i2c\n\t12: tsensor"
                            "\n\t13: power <num>\n\t\t0: All tests excludes VddOff test\n\t\t1: Run VddOff test"
                            "\n\t14: battery"
                            "\n\t15: efuse\n\t"
                            "16: spi <num> \n\t\t0: All Independent tests\n\t\t1: Read Sync Master\n\t\t"
                            "2: Write Sync Master\n\t\t3: Transfer Sync Master\n\t\t4: Read ASync Master\n\t\t"
                            "5: Write ASync Master\n\t\t6: Transfer ASync Master\n\t\t"
                            "10: Read Sync Slave\n\t\t11: Write Sync Slave\n\t\t12: Transfer Sync Slave"
                            "\n\t17: UsbDevice <num> \n\t\t0: All Independent tests\n\t\t1: AFQP_IotUsbDeviceHidAttach\n\t\t2: AFQP_IotUsbDeviceHidDetach\n\t\t3: AFQP_IotUsbDeviceEndpointStall\n\t\t4: AFQP_IotUsbDeviceGetSpeed\n\t\t5: AFQP_IotUsbDeviceWriteAsync\n\t\t6: AFQP_IotUsbDeviceReadAsync\n\t\t7: AFQP_IotUsbDeviceWriteSync\n\t\t8: AFQP_IotUsbDeviceReadSync\n\t\t9: AFQP_IotUsbDeviceEndpointCancelTransfer\n\t\t10: AFQP_IotUsbDeviceIoctl\n\t"
                            "18: UsbHost <num> \n\t\t0: All Independent tests\n\t\t1: AFQP_IotUsbHostHidGeneric\n\t\t2: AFQP_IotUsbHostWriteAsync\n\t\t3: AFQP_IotUsbHostReadAsync\n\t",
                            ACE_CLI_SET_LEAF, .command.func=&runIotTests},
                            ACE_CLI_NULL_MODULE
                            };