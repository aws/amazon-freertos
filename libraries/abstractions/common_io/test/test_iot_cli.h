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

/*******************************************************************************
 * IOT On-Target Unit Test CLI
 * @File: test_iot_cli.h
 ******************************************************************************/

#ifdef AMAZON_TESTS_ENABLE
#include <ace/aceCli.h>

extern const aceCli_moduleCmd_t iot_tests_cli_sub[];
#define IOT_TESTS_CLI {"iot_tests", "iot tests control", ACE_CLI_SET_FUNC, .command.subCommands=iot_tests_cli_sub},

#else
#define IOT_TESTS_CLI
#endif
