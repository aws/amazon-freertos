/*
 * Copyright 2019 Amazon.com, Inc. or its affiliates. All rights reserved.
 *
 * AMAZON PROPRIETARY/CONFIDENTIAL
 *
 * You may not use this file except in compliance with the terms and
 * conditions set forth in the accompanying LICENSE.TXT file.
 *
 * THESE MATERIALS ARE PROVIDED ON AN "AS IS" BASIS. AMAZON SPECIFICALLY
 * DISCLAIMS, WITH RESPECT TO THESE MATERIALS, ALL WARRANTIES, EXPRESS,
 * IMPLIED, OR STATUTORY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
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
                            {"test", "<num>\n\t0: all\n\t1: uart\n\t2: flash\n\t"
                            "3: wdog <num> \n\t\t0: All Non-Reset tests\n\t\t1: Bite-Reset test\n\t\t2: ResetReason Test\n\t"
                            "4: rtc\n\t"
                            "5: gpio <num> \n\t\t0: All Independent tests\n\t\t1: AFQP_AssistedIotGpioModeWritePushPullTrue\n\t\t2: AFQP_AssistedIotGpioModeWritePushPullFalse\n\t\t3: AFQP_AssistedIotGpioModeWriteOpenDrainTrue\n\t\t4: AFQP_AssistedIotGpioModeWriteOpenDrainFalse\n\t\t5: AFQP_AssistedIotGpioModeReadTrue\n\t\t6: AFQP_AssistedIotGpioModeReadFalse\n\t"
                            "6: timer\n\t"
                            "7: adc\n\t"
                            "8: reset <num>\n\t\t0: Shutdown\n\t\t1: RebootZero\n\t\t2: RebootNonZero"
                            "\n\t9: perfcounter\n\t10: pwm\n\t11: i2c\n\t12: tsensor\n\t13: power\n\t14: battery"
                            "\n\t15: efuse\n\t"
                            "16: spi <num> \n\t\t0: All Independent tests\n\t\t1: Read Sync Master\n\t\t"
                            "2: Write Sync Master\n\t\t3: Transfer Sync Master\n\t\t4: Read ASync Master\n\t\t"
                            "5: Write ASync Master\n\t\t6: Transfer ASync Master\n\t\t"
                            "10: Read Sync Slave\n\t\t11: Write Sync Slave\n\t\t12: Transfer Sync Slave",
                            ACE_CLI_SET_LEAF, .command.func=&runIotTests},
                            ACE_CLI_NULL_MODULE
                            };
