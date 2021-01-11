/*
 * FreeRTOS HTTPS Client V1.2.0
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

/**
 * @file iot_tests_https_utils.c
 * @brief Tests for the user-facing API functions declared in iot_https_utils.h.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* C standard includes. */
#include <string.h>

/* HTTPS Client Utils include. */
#include "iot_https_utils.h"

/* Test framework includes. */
#include "unity_fixture.h"

/*-----------------------------------------------------------*/

/**
 * @brief Test URL with "https://"" prepending and query.
 */
#define HTTPS_TEST_URL_HTTPS                          "https://mybucket.s3.amazonaws.com/myobject.txt?AWSAccessKeyId=AAAAAAAAAAAAAAAAAAAA&Expires=0000000000&Signature=abcde%2fghijklmn%2pqrstuvwxyzAB%3D"

/**
 * @brief The path to the test URL above.
 */
#define HTTPS_TEST_URL_HTTPS_EXPECTED_PATH            "/myobject.txt"

/**
 * @brief The address to the test URL above.
 */
#define HTTPS_TSET_URL_HTTPS_EXPECTED_ADDRESS         "mybucket.s3.amazonaws.com"

/**
 * @brief Test URL with wwww.
 */
#define HTTPS_TEST_URL_WWW                            "https://www.amazon.com/gp/css/order-history?ref_=nav_orders_first"

/**
 * @brief The path to the test URL above.
 */
#define HTTPS_TEST_URL_WWW_EXPECTED_PATH              "/gp/css/order-history"

/**
 * @brief The address to the test URL above.
 */
#define HTTPS_TEST_URL_WWW_EXPECTED_ADDRESS           "www.amazon.com"

/**
 * @brief Test URL without a path.
 */
#define HTTPS_TEST_URL_NO_PATH                        "https://www.aws.amazon.com"

/**
 * @brief The path to the test URL above.
 */
#define HTTPS_TEST_URL_NO_PATH_EXPECTED_PATH          NULL

/**
 * @brief The address to the test URL above.
 */
#define HTTPS_TEST_URL_NO_PATH_EXPECTED_ADDRESS       "www.aws.amazon.com"

/**
 * @brief Test URL without a path.
 */
#define HTTPS_TEST_URL_NO_ADDRESS                     "/no/address/path?q=q"

/**
 * @brief The path to the test URL above.
 */
#define HTTPS_TEST_URL_NO_ADDRESS_EXPECTED_PATH       NULL

/**
 * @brief The address to the test URL above.
 */
#define HTTPS_TEST_URL_NO_ADDRESS_EXPECTED_ADDRESS    NULL

/**
 * @brief Invalid URL.
 */
#define HTTPS_TEST_INVALID_URL                        "invalid_url/invalid_path"

/*-----------------------------------------------------------*/

/**
 * @brief Test group for HTTPS Utils API tests.
 */
TEST_GROUP( HTTPS_Utils_Unit_API );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for HTTPS Utils API tests.
 */
TEST_SETUP( HTTPS_Utils_Unit_API )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for HTTPS Utils API tests.
 */
TEST_TEAR_DOWN( HTTPS_Utils_Unit_API )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for HTTPS Utils API tests.
 */
TEST_GROUP_RUNNER( HTTPS_Utils_Unit_API )
{
    RUN_TEST_CASE( HTTPS_Utils_Unit_API, GetUrlPathInvalidParameters );
    RUN_TEST_CASE( HTTPS_Utils_Unit_API, GetUrlPathVerifications );
    RUN_TEST_CASE( HTTPS_Utils_Unit_API, GetUrlAddressInvalidParameters );
    RUN_TEST_CASE( HTTPS_Utils_Unit_API, GetUrlAddressVerifications );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test various invlaid parameters into IotHttpsClient_GetUrlPath().
 */
TEST( HTTPS_Utils_Unit_API, GetUrlPathInvalidParameters )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    const char * pReturnPath = NULL;
    size_t returnPathLen = 0;

    /* Test NULL pUrl parameter. */
    returnCode = IotHttpsClient_GetUrlPath( NULL, strlen( HTTPS_TEST_URL_HTTPS ), &pReturnPath, &returnPathLen );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test a URL length of zero. */
    returnCode = IotHttpsClient_GetUrlPath( HTTPS_TEST_URL_HTTPS, 0, &pReturnPath, &returnPathLen );
    TEST_ASSERT_EQUAL( IOT_HTTPS_PARSING_ERROR, returnCode );

    /* Test NULL pPath parameter. */
    returnCode = IotHttpsClient_GetUrlPath( HTTPS_TEST_URL_HTTPS, strlen( HTTPS_TEST_URL_HTTPS ), NULL, &returnPathLen );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test NULL pPathLen parameter. */
    returnCode = IotHttpsClient_GetUrlPath( HTTPS_TEST_URL_HTTPS, strlen( HTTPS_TEST_URL_HTTPS ), &pReturnPath, NULL );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test all parameters NULL. */
    returnCode = IotHttpsClient_GetUrlPath( NULL, strlen( HTTPS_TEST_URL_HTTPS ), NULL, NULL );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test an invalid URL. */
    returnCode = IotHttpsClient_GetUrlPath( HTTPS_TEST_INVALID_URL, strlen( HTTPS_TEST_INVALID_URL ), &pReturnPath, &returnPathLen );
    TEST_ASSERT_EQUAL( IOT_HTTPS_PARSING_ERROR, returnCode );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test parsing the path from various different forms of URLs.
 */
TEST( HTTPS_Utils_Unit_API, GetUrlPathVerifications )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    const char * pReturnPath = NULL;
    size_t returnPathLen = 0;

    /* Test getting the path from a URL with "https://" prepending. */
    returnCode = IotHttpsClient_GetUrlPath( HTTPS_TEST_URL_HTTPS, strlen( HTTPS_TEST_URL_HTTPS ), &pReturnPath, &returnPathLen );
    TEST_ASSERT_EQUAL( 0, strncmp( pReturnPath, HTTPS_TEST_URL_HTTPS_EXPECTED_PATH, strlen( HTTPS_TEST_URL_HTTPS_EXPECTED_PATH ) ) );
    TEST_ASSERT_EQUAL( strlen( HTTPS_TEST_URL_HTTPS_EXPECTED_PATH ), returnPathLen );

    /* Test getting the path from a www URL. */
    returnCode = IotHttpsClient_GetUrlPath( HTTPS_TEST_URL_WWW, strlen( HTTPS_TEST_URL_WWW ), &pReturnPath, &returnPathLen );
    TEST_ASSERT_EQUAL( 0, strncmp( pReturnPath, HTTPS_TEST_URL_WWW_EXPECTED_PATH, strlen( HTTPS_TEST_URL_WWW_EXPECTED_PATH ) ) );
    TEST_ASSERT_EQUAL( strlen( HTTPS_TEST_URL_WWW_EXPECTED_PATH ), returnPathLen );

    /* Test getting the path from a URL without a path. */
    returnCode = IotHttpsClient_GetUrlPath( HTTPS_TEST_URL_NO_PATH, strlen( HTTPS_TEST_URL_NO_PATH ), &pReturnPath, &returnPathLen );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NOT_FOUND, returnCode );
    TEST_ASSERT_EQUAL( HTTPS_TEST_URL_NO_PATH_EXPECTED_PATH, pReturnPath );
    TEST_ASSERT_EQUAL( 0, returnPathLen );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test various invlaid parameters into IotHttpsClient_GetUrlAddress().
 */
TEST( HTTPS_Utils_Unit_API, GetUrlAddressInvalidParameters )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    const char * pReturnAddress = NULL;
    size_t returnAddressLen = 0;

    /* Test NULL pUrl parameter. */
    returnCode = IotHttpsClient_GetUrlAddress( NULL, strlen( HTTPS_TEST_URL_HTTPS ), &pReturnAddress, &returnAddressLen );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test a URL length of zero. */
    returnCode = IotHttpsClient_GetUrlAddress( HTTPS_TEST_URL_HTTPS, 0, &pReturnAddress, &returnAddressLen );
    TEST_ASSERT_EQUAL( IOT_HTTPS_PARSING_ERROR, returnCode );

    /* Test NULL pAddress parameter. */
    returnCode = IotHttpsClient_GetUrlAddress( HTTPS_TEST_URL_HTTPS, strlen( HTTPS_TEST_URL_HTTPS ), NULL, &returnAddressLen );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test NULL pAddressLen parameter. */
    returnCode = IotHttpsClient_GetUrlAddress( HTTPS_TEST_URL_HTTPS, strlen( HTTPS_TEST_URL_HTTPS ), &pReturnAddress, NULL );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test all parameters NULL. */
    returnCode = IotHttpsClient_GetUrlAddress( NULL, strlen( HTTPS_TEST_URL_HTTPS ), NULL, NULL );
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode );

    /* Test an invalid URL. */
    returnCode = IotHttpsClient_GetUrlAddress( HTTPS_TEST_INVALID_URL, strlen( HTTPS_TEST_INVALID_URL ), &pReturnAddress, &returnAddressLen );
    TEST_ASSERT_EQUAL( IOT_HTTPS_PARSING_ERROR, returnCode );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test parsing the address from various different forms of URLs.
 */
TEST( HTTPS_Utils_Unit_API, GetUrlAddressVerifications )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    const char * pReturnAddress = NULL;
    size_t returnAddressLen = 0;

    /* Test getting the address from a URL with "https://" prepending. */
    returnCode = IotHttpsClient_GetUrlAddress( HTTPS_TEST_URL_HTTPS, strlen( HTTPS_TEST_URL_HTTPS ), &pReturnAddress, &returnAddressLen );
    TEST_ASSERT_EQUAL( 0, strncmp( pReturnAddress, HTTPS_TSET_URL_HTTPS_EXPECTED_ADDRESS, strlen( HTTPS_TSET_URL_HTTPS_EXPECTED_ADDRESS ) ) );
    TEST_ASSERT_EQUAL( strlen( HTTPS_TSET_URL_HTTPS_EXPECTED_ADDRESS ), returnAddressLen );

    /* Test getting the address from a www url. */
    returnCode = IotHttpsClient_GetUrlAddress( HTTPS_TEST_URL_WWW, strlen( HTTPS_TEST_URL_WWW ), &pReturnAddress, &returnAddressLen );
    TEST_ASSERT_EQUAL( 0, strncmp( pReturnAddress, HTTPS_TEST_URL_WWW_EXPECTED_ADDRESS, strlen( HTTPS_TEST_URL_WWW_EXPECTED_ADDRESS ) ) );
    TEST_ASSERT_EQUAL( strlen( HTTPS_TEST_URL_WWW_EXPECTED_ADDRESS ), returnAddressLen );

    /* Test getting the address from a URL with no path. */
    returnCode = IotHttpsClient_GetUrlAddress( HTTPS_TEST_URL_NO_PATH, strlen( HTTPS_TEST_URL_NO_PATH ), &pReturnAddress, &returnAddressLen );
    TEST_ASSERT_EQUAL( 0, strncmp( pReturnAddress, HTTPS_TEST_URL_NO_PATH_EXPECTED_ADDRESS, strlen( HTTPS_TEST_URL_NO_PATH_EXPECTED_ADDRESS ) ) );
    TEST_ASSERT_EQUAL( strlen( HTTPS_TEST_URL_NO_PATH_EXPECTED_ADDRESS ), returnAddressLen );

    /* Test getting the address from a URL with no address. */
    returnCode = IotHttpsClient_GetUrlAddress( HTTPS_TEST_URL_NO_ADDRESS, strlen( HTTPS_TEST_URL_NO_ADDRESS ), &pReturnAddress, &returnAddressLen );
    TEST_ASSERT_EQUAL( IOT_HTTPS_NOT_FOUND, returnCode );
    TEST_ASSERT_EQUAL( HTTPS_TEST_URL_NO_ADDRESS_EXPECTED_ADDRESS, pReturnAddress );
    TEST_ASSERT_EQUAL( 0, returnAddressLen );
}
