/*
 * Amazon FreeRTOS OTA AFQP V1.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

#include "unity_fixture.h"
#include "unity.h"
#include "aws_ota_agent_test_access_declare.h"
#include "aws_ota_pal_test_access_declare.h"
#include "aws_ota_pal.h"
#include "aws_ota_agent.h"

/**
 * @brief Configuration for this test group.
 */
#define pcTEST_PAL_FILEPATH    "otatesttempfile.bin"

/**
 * @brief Test group definition.
 */
TEST_GROUP( Full_OTA_PAL );

TEST_SETUP( Full_OTA_PAL )
{
}

TEST_TEAR_DOWN( Full_OTA_PAL )
{
}

TEST_GROUP_RUNNER( Full_OTA_PAL )
{
    RUN_TEST_CASE( Full_OTA_PAL, prvClose );
    RUN_TEST_CASE( Full_OTA_PAL, prvCreateFileForRx );
    RUN_TEST_CASE( Full_OTA_PAL, prvAbort );
    RUN_TEST_CASE( Full_OTA_PAL, prvActivateNewImage );
}

TEST( Full_OTA_PAL, prvActivateNewImage )
{
    OTA_Err_t xOtaStatus = TEST_OTA_prvActivateNewImage();

    TEST_ASSERT_EQUAL_INT( kOTA_Err_None, xOtaStatus );
}

TEST( Full_OTA_PAL, prvClose )
{
    OTA_FileContext_t xOtaFile;
    OTA_Err_t xOtaStatus;

    memset( &xOtaFile, 0, sizeof( xOtaFile ) );

    /* Close without doing anything else. */
    xOtaStatus = TEST_OTA_prvCloseFile( &xOtaFile );
    TEST_ASSERT_EQUAL_INT( kOTA_Err_SignatureCheckFailed, xOtaStatus );
}

TEST( Full_OTA_PAL, prvAbort )
{
    OTA_FileContext_t xOtaFile;
    uint8_t xCreateStatus;
    OTA_Err_t xOtaStatus;

    memset( &xOtaFile, 0, sizeof( xOtaFile ) );
    xOtaFile.pacFilepath = pcTEST_PAL_FILEPATH;

    /* Create a local file using the PAL. */
    xCreateStatus = TEST_OTA_prvCreateFileForRx( &xOtaFile );
    TEST_ASSERT_TRUE( xCreateStatus );

    /* Signal that the download is being aborted. */
    xOtaStatus = TEST_OTA_prvAbort( &xOtaFile );
    TEST_ASSERT_EQUAL_INT( kOTA_Err_None, xOtaStatus );

    /* Close should be redundant, but call it anyway and expect a signature
     * error. */
    xOtaStatus = TEST_OTA_prvCloseFile( &xOtaFile );
    TEST_ASSERT_EQUAL_INT( kOTA_Err_SignatureCheckFailed, xOtaStatus );
}

TEST( Full_OTA_PAL, prvCreateFileForRx )
{
    OTA_FileContext_t xOtaFile;
    uint8_t xCreateStatus;
    OTA_Err_t xOtaStatus;

    memset( &xOtaFile, 0, sizeof( xOtaFile ) );
    xOtaFile.pacFilepath = pcTEST_PAL_FILEPATH;

    /* Create a local file using the PAL. */
    xCreateStatus = TEST_OTA_prvCreateFileForRx( &xOtaFile );
    TEST_ASSERT_TRUE( xCreateStatus );

    /* Close the file handle, but expect an error since the file doesn't
     * consist of signed data. */
    xOtaStatus = TEST_OTA_prvCloseFile( &xOtaFile );
    TEST_ASSERT_EQUAL_INT( kOTA_Err_SignatureCheckFailed, xOtaStatus );
}
