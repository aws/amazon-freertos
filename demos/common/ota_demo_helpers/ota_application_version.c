/*
 * FreeRTOS V202203.00
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
 */

/**
 * @file ota_application_version.c
 *
 * @brief This file defines the appFirmwareVersion variable that is required to
 * build the OTA library. This variable has a dedicated file so that it can be
 * referenced by multiple OTA demos.
 */

/* Includes the OTA Application version number. */
#include "ota_appversion32.h"
#include "ota_config.h"

/* Definition of the appFirmwareVersion variable that is used by the OTA
 * library and is declared in the ota_appversion32.h file. The values for these
 * firmware versions are set in the ota_demo_config.h file. */
const AppVersion32_t appFirmwareVersion =
{
    .u.x.major = APP_VERSION_MAJOR,
    .u.x.minor = APP_VERSION_MINOR,
    .u.x.build = APP_VERSION_BUILD
};
