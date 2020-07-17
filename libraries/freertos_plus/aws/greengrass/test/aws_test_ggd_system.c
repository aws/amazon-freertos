/*
 * FreeRTOS Greengrass V2.0.2
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

#include <stdio.h>
#include <string.h>

#include "aws_greengrass_discovery.h"
#include "aws_helper_secure_connect.h"
#include "jsmn.h"
#include "unity_fixture.h"
#include "unity.h"
#include "aws_test_utils.h"
#include "aws_greengrass_discovery_test_access_declare.h"
#include "aws_test_runner.h"
#include "iot_mqtt.h"
#include "iot_init.h"

#define ggdJSON_FILE                     "{\"GGGroups\":[{\"GGGroupId\":\"myGroupID\",\"Cores\":[{\"thingArn\":\"myGreenGrassCoreArn\",\"Connectivity\":[{\"Id\":\"AUTOIP_10.60.212.138_0\",\"HostAddress\":\"44.44.44.44\",\"PortNumber\":1234,\"Metadata\":\"\"},{\"Id\":\"AUTOIP_127.0.0.1_1\",\"HostAddress\":\"127.0.0.1\",\"PortNumber\":8883,\"Metadata\":\"\"},{\"Id\":\"AUTOIP_192.168.2.2_2\",\"HostAddress\":\"01.23.456.789\",\"PortNumber\":4321,\"Metadata\":\"\"},{\"Id\":\"AUTOIP_::1_3\",\"HostAddress\":\"::1\",\"PortNumber\":8883,\"Metadata\":\"\"},{\"Id\":\"AUTOIP_fe80::bfda:8f62:7b4b:f358_4\",\"HostAddress\":\"fe80::bfda:8f62:7b4b:f358\",\"PortNumber\":8883,\"Metadata\":\"\"},{\"Id\":\"AUTOIP_fe80::e234:cff9:f53f:6216_5\",\"HostAddress\":\"fe80::e234:cff9:f53f:6216\",\"PortNumber\":8883,\"Metadata\":\"\"}]}],\"CAs\":[\"-----BEGIN CERTIFICATE-----\\nMIIEFTCCAv2gAwIBAgIVAPRru+NqCDr0r6oD6PnTG05rWuY+MA0GCSqGSIb3DQEB\\nCwUAMIGoMQswCQYDVQQGEwJVUzEYMBYGA1UECgwPQW1hem9uLmNvbSBJbmMuMRww\\nGgYDVQQLDBNBbWF6b24gV2ViIFNlcnZpY2VzMRMwEQYDVQQIDApXYXNoaW5ndG9u\\nMRAwDgYDVQQHDAdTZWF0dGxlMTowOAYDVQQDDDE5NDI5MjczNzY5NjU6ZDk3ZmZl\\nZmUtNTI4MS00ZWM5LTk4NDYtYjNlZTQxMDRjMjAxMCAXDTE3MDcwNjIwMDczOFoY\\nDzIwOTcwNzA2MjAwNzM3WjCBqDELMAkGA1UEBhMCVVMxGDAWBgNVBAoMD0FtYXpv\\nbi5jb20gSW5jLjEcMBoGA1UECwwTQW1hem9uIFdlYiBTZXJ2aWNlczETMBEGA1UE\\nCAwKV2FzaGluZ3RvbjEQMA4GA1UEBwwHU2VhdHRsZTE6MDgGA1UEAwwxOTQyOTI3\\nMzc2OTY1OmQ5N2ZmZWZlLTUyODEtNGVjOS05ODQ2LWIzZWU0MTA0YzIwMTCCASIw\\nDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKxzJpXU2DZDEglh/FT01epAWby6\\np4Ymw76icyMzBUJzafibABJ3cTyjDQE6ZqbSl1ryBxGwQBsveIgj8SVVtv927wk7\\nlncgD+EghfTZgSfscND653AJeVFQlCeHipZI32wzXyPmwglFrWp9vsrY/8BO1Kjk\\nSAs4o8fDVVMAaZCJDMuc5csc3CQ2OJYLOl+SZisGNM1h0xHpWieM38KDDrp99x8Q\\nTwDmgaMjtdIJR7Y9Nzm0N78gTf3gTazEO9iUKojVCNubxK/lQ6KjJ0JcvsljPpVp\\nuzjOmn91xmNoHEQCboa7YoYNNbdAbftGeUl16wFdTgbuUS9vakk5idVoC2ECAwEA\\nAaMyMDAwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUmcz4OlH9+mlpnTKG3taI\\nw+6FSk0wDQYJKoZIhvcNAQELBQADggEBACeiQ6MxiktsU0sLNmP1cNbiuBuutjoq\\nymk476Bhr4E2WSE0B9W1TFOSLIYx9oN63T3lXzsGHP/MznueIbqbwFf/o5aXI7th\\n+J+i9LgBrViNvzkze7G0GiPuEQ7ox4XnPBJAFtTZxa8gXL95QfcypERpQs28lg7W\\nQpdNhiBN+c4o1aSOzJ474sjXnjtI1G2jRTKucm0buYYeAeVT7kpBq9YL7gGfOcyj\\nsPxQEgyQV2Mk+b1q7lYDS4tnzoRkUfNLgAtDKSh8S8iVhAR6wRR2G3aMySKrOxbg\\nalghO3OqfeuTwIj9w17JTAyYAME22RJQ6oxEJ8rHp/9PaYnOmiSkP7M=\\n-----END CERTIFICATE-----\\n\"]}]}"
#define ggdTestMAX_REQUEST_LOOP_COUNT    3

/**
 * @brief The initial delay in milliseconds that is doubled each retry of request.
 */
#ifndef IOT_TEST_GGD_INITIAL_CONNECTION_RETRY_DELAY
    #define IOT_TEST_GGD_INITIAL_CONNECTION_RETRY_DELAY    ( ( uint32_t ) 1100 )
#endif

/**
 * @brief The number of times to try a connection. Values greater than 1 will retry on failure with an exponential backoff.
 */
#ifndef IOT_TEST_GGD_CONNECT_RETRY_COUNT
    #define IOT_TEST_GGD_CONNECT_RETRY_COUNT    ( 1 )
#endif
/** @endcond */

#if IOT_TEST_GGD_CONNECT_RETRY_COUNT < 1
    #error "IOT_TEST_GGD_CONNECT_RETRY_COUNT must be at least 1."
#endif

static const char cJSON_FILE[] = ggdJSON_FILE;
static const char cCERTIFICATE[] = "-----BEGIN CERTIFICATE-----\nMIIEFTCCAv2gAwIBAgIVAPRru+NqCDr0r6oD6PnTG05rWuY+MA0GCSqGSIb3DQEB\nCwUAMIGoMQswCQYDVQQGEwJVUzEYMBYGA1UECgwPQW1hem9uLmNvbSBJbmMuMRww\nGgYDVQQLDBNBbWF6b24gV2ViIFNlcnZpY2VzMRMwEQYDVQQIDApXYXNoaW5ndG9u\nMRAwDgYDVQQHDAdTZWF0dGxlMTowOAYDVQQDDDE5NDI5MjczNzY5NjU6ZDk3ZmZl\nZmUtNTI4MS00ZWM5LTk4NDYtYjNlZTQxMDRjMjAxMCAXDTE3MDcwNjIwMDczOFoY\nDzIwOTcwNzA2MjAwNzM3WjCBqDELMAkGA1UEBhMCVVMxGDAWBgNVBAoMD0FtYXpv\nbi5jb20gSW5jLjEcMBoGA1UECwwTQW1hem9uIFdlYiBTZXJ2aWNlczETMBEGA1UE\nCAwKV2FzaGluZ3RvbjEQMA4GA1UEBwwHU2VhdHRsZTE6MDgGA1UEAwwxOTQyOTI3\nMzc2OTY1OmQ5N2ZmZWZlLTUyODEtNGVjOS05ODQ2LWIzZWU0MTA0YzIwMTCCASIw\nDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKxzJpXU2DZDEglh/FT01epAWby6\np4Ymw76icyMzBUJzafibABJ3cTyjDQE6ZqbSl1ryBxGwQBsveIgj8SVVtv927wk7\nlncgD+EghfTZgSfscND653AJeVFQlCeHipZI32wzXyPmwglFrWp9vsrY/8BO1Kjk\nSAs4o8fDVVMAaZCJDMuc5csc3CQ2OJYLOl+SZisGNM1h0xHpWieM38KDDrp99x8Q\nTwDmgaMjtdIJR7Y9Nzm0N78gTf3gTazEO9iUKojVCNubxK/lQ6KjJ0JcvsljPpVp\nuzjOmn91xmNoHEQCboa7YoYNNbdAbftGeUl16wFdTgbuUS9vakk5idVoC2ECAwEA\nAaMyMDAwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUmcz4OlH9+mlpnTKG3taI\nw+6FSk0wDQYJKoZIhvcNAQELBQADggEBACeiQ6MxiktsU0sLNmP1cNbiuBuutjoq\nymk476Bhr4E2WSE0B9W1TFOSLIYx9oN63T3lXzsGHP/MznueIbqbwFf/o5aXI7th\n+J+i9LgBrViNvzkze7G0GiPuEQ7ox4XnPBJAFtTZxa8gXL95QfcypERpQs28lg7W\nQpdNhiBN+c4o1aSOzJ474sjXnjtI1G2jRTKucm0buYYeAeVT7kpBq9YL7gGfOcyj\nsPxQEgyQV2Mk+b1q7lYDS4tnzoRkUfNLgAtDKSh8S8iVhAR6wRR2G3aMySKrOxbg\nalghO3OqfeuTwIj9w17JTAyYAME22RJQ6oxEJ8rHp/9PaYnOmiSkP7M=\n-----END CERTIFICATE-----\n";
static const char cMyGroupID[] = "myGroupID";
static const char cIP_ADDRESS_3[] = "01.23.456.789";
static const char cMY_CORE_ARN[] = "myGreenGrassCoreArn";

static Socket_t xSocket;

/* Helper function to call GGD_JSONRequestGetFile() in a loop. */
static BaseType_t prvGGD_JSONRequestGetFileLoop( uint32_t ulBufferSize,
                                                 uint32_t * pulByteRead,
                                                 BaseType_t * pxJSONFileRetrieveCompleted,
                                                 uint32_t ulJSONFileSize );

/* Wrapper function to retry request in case of failure with exponential backoff. */
static BaseType_t prvGGD_JSONRequestStart( const char * pcHostAddress,
                                           uint16_t usGGDPort,
                                           const char * pcThingName,
                                           Socket_t * pxSocket );

TEST_GROUP( GGD_System );

TEST_SETUP( GGD_System )
{
    TEST_ASSERT_EQUAL( true, IotSdk_Init() );
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, IotMqtt_Init() );
}

TEST_TEAR_DOWN( GGD_System )
{
    if( xSocket != SOCKETS_INVALID_SOCKET )
    {
        /* Close the socket. */
        ( void ) SOCKETS_Close( xSocket );
    }

    IotMqtt_Cleanup();
    IotSdk_Cleanup();
}


TEST_GROUP_RUNNER( GGD_System )
{
    RUN_TEST_CASE( GGD_System, JSONRequestStart );
    RUN_TEST_CASE( GGD_System, JSONRequestAbort );
    RUN_TEST_CASE( GGD_System, GetIPandCertificateFromJSON );
    RUN_TEST_CASE( GGD_System, JSONRequestGetSize );
    RUN_TEST_CASE( GGD_System, JSONRequestGetFile );
    RUN_TEST_CASE( GGD_System, GetGGCIPandCertificate );
}

static BaseType_t prvGGD_JSONRequestGetFileLoop( uint32_t ulBufferSize,
                                                 uint32_t * pulByteRead,
                                                 BaseType_t * pxJSONFileRetrieveCompleted,
                                                 uint32_t ulJSONFileSize )
{
    BaseType_t xStatus = pdPASS;
    /* Limit loop count to avoid any chance of infinite loops. */
    uint32_t ulRequestLoopCounter = 0;

    do
    {
        xStatus = GGD_JSONRequestGetFile( &xSocket,
                                          &cBuffer[ *pulByteRead ],
                                          ulBufferSize - *pulByteRead,
                                          pulByteRead,
                                          pxJSONFileRetrieveCompleted,
                                          ulJSONFileSize );
    } while( ( xStatus == pdPASS ) && ( *pxJSONFileRetrieveCompleted != pdTRUE ) && ( ulBufferSize > *pulByteRead ) && ( ++ulRequestLoopCounter < ggdTestMAX_REQUEST_LOOP_COUNT ) );

    return xStatus;
}

static BaseType_t prvGGD_JSONRequestStart( const char * pcHostAddress,
                                           uint16_t usGGDPort,
                                           const char * pcThingName,
                                           Socket_t * pxSocket )
{
    BaseType_t xStatus;

    RETRY_EXPONENTIAL( xStatus = GGD_JSONRequestStart( pcHostAddress, usGGDPort, pcThingName, pxSocket ),
                       pdPASS,
                       IOT_TEST_GGD_INITIAL_CONNECTION_RETRY_DELAY,
                       IOT_TEST_GGD_CONNECT_RETRY_COUNT );
    return xStatus;
}

TEST( GGD_System, JSONRequestAbort )
{
    /** @brief check for stability for all meaningfull values of socket.
     * 1. Invalid
     * 2. Correct
     * 3. NULL
     *  @{
     */
    if( TEST_PROTECT() )
    {
        xSocket = SOCKETS_INVALID_SOCKET;
        GGD_JSONRequestAbort( &xSocket );

        prvGGD_JSONRequestStart( clientcredentialMQTT_BROKER_ENDPOINT,
                                 clientcredentialGREENGRASS_DISCOVERY_PORT,
                                 clientcredentialIOT_THING_NAME,
                                 &xSocket );
        GGD_JSONRequestAbort( &xSocket );
    }
    else
    {
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        GGD_JSONRequestAbort( NULL );
        TEST_FAIL();
    }

    /** @}*/
}


TEST( GGD_System, GetGGCIPandCertificate )
{
    BaseType_t xStatus;
    GGD_HostAddressData_t xHostAddressData;
    uint32_t ulBufferSize = testrunnerBUFFER_SIZE;

    if( TEST_PROTECT() )
    {
        /** @brief Check function works in ideal scenario.
         * Provided with known JSON file and checking output.
         * Check with auto-search flag set to false and true
         *  @{
         */
        RETRY_EXPONENTIAL( xStatus = GGD_GetGGCIPandCertificate( clientcredentialMQTT_BROKER_ENDPOINT,
                                                                 clientcredentialGREENGRASS_DISCOVERY_PORT,
                                                                 clientcredentialIOT_THING_NAME,
                                                                 cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                                                 ulBufferSize,
                                                                 &xHostAddressData ),
                           pdPASS,
                           IOT_TEST_GGD_INITIAL_CONNECTION_RETRY_DELAY,
                           IOT_TEST_GGD_CONNECT_RETRY_COUNT );

        TEST_ASSERT_EQUAL_INT32_MESSAGE( pdPASS, xStatus, cBuffer );

        /** @}*/

        /** @brief Check fail is returned if provided buffer is too small
         *  @{
         */
        xStatus = GGD_GetGGCIPandCertificate( clientcredentialMQTT_BROKER_ENDPOINT,
                                              clientcredentialGREENGRASS_DISCOVERY_PORT,
                                              clientcredentialIOT_THING_NAME,
                                              cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                              xHostAddressData.ulCertificateSize - 1,
                                              &xHostAddressData );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( pdFAIL, xStatus, "GGD_GetGGCIPandCertificate() passed when the input buffer was too small." );
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }

    /** @brief Check stability.
     *
     *  @{
     */
    if( TEST_PROTECT() )
    {
        xStatus = GGD_GetGGCIPandCertificate( clientcredentialMQTT_BROKER_ENDPOINT,
                                              clientcredentialGREENGRASS_DISCOVERY_PORT,
                                              clientcredentialIOT_THING_NAME,
                                              NULL,
                                              ulBufferSize,
                                              &xHostAddressData );
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        xStatus = GGD_GetGGCIPandCertificate( clientcredentialMQTT_BROKER_ENDPOINT,
                                              clientcredentialGREENGRASS_DISCOVERY_PORT,
                                              clientcredentialIOT_THING_NAME,
                                              cBuffer,
                                              ulBufferSize,
                                              NULL );
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        xStatus = GGD_GetGGCIPandCertificate( NULL,
                                              clientcredentialGREENGRASS_DISCOVERY_PORT,
                                              clientcredentialIOT_THING_NAME,
                                              cBuffer,
                                              ulBufferSize,
                                              &xHostAddressData );
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        xStatus = GGD_GetGGCIPandCertificate( clientcredentialMQTT_BROKER_ENDPOINT,
                                              clientcredentialGREENGRASS_DISCOVERY_PORT,
                                              NULL,
                                              cBuffer,
                                              ulBufferSize,
                                              &xHostAddressData );
        TEST_FAIL();
    }

    /** @}*/
}


TEST( GGD_System, GetIPandCertificateFromJSON )
{
    uint32_t ulJSONFileSize = strlen( cJSON_FILE );
    BaseType_t xStatus;
    HostParameters_t xHostParameters;
    GGD_HostAddressData_t xHostAddressData;
    BaseType_t xAutoSearchFlag;
    char cBadGroupId[] = "myBadGroupID";
    char cBadCoreARN[] = "myBadCoreARN";
    uint32_t ulBufferSize = testrunnerBUFFER_SIZE;
    uint32_t ulByteRead;
    BaseType_t xJSONFileRetrieveCompleted;

    if( TEST_PROTECT() )
    {
        /** @brief Prepare test.
         *  @{
         */
        memcpy( cBuffer, cJSON_FILE, strlen( cJSON_FILE ) );
        /** @}*/

        /** @brief Check function works in ideal scenario.
         * Provided with known JSON file and checking output.
         * Check with auto-search flag set to false and true
         *  @{
         */
        xHostParameters.pcCoreAddress = ( char * ) cMY_CORE_ARN;
        xHostParameters.pcGroupName = ( char * ) cMyGroupID;
        xHostParameters.ucInterface = 3;
        xAutoSearchFlag = pdFALSE;
        xStatus = GGD_GetIPandCertificateFromJSON( cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                                   ulJSONFileSize,
                                                   &xHostParameters,
                                                   &xHostAddressData,
                                                   xAutoSearchFlag );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        TEST_ASSERT_EQUAL_STRING( cIP_ADDRESS_3, xHostAddressData.pcHostAddress );
        TEST_ASSERT_EQUAL_STRING( cCERTIFICATE, xHostAddressData.pcCertificate );
        TEST_ASSERT_EQUAL_INT32( strlen( cCERTIFICATE ) + 1, xHostAddressData.ulCertificateSize );

        xAutoSearchFlag = pdTRUE;
        xStatus = prvGGD_JSONRequestStart( clientcredentialMQTT_BROKER_ENDPOINT,
                                           clientcredentialGREENGRASS_DISCOVERY_PORT,
                                           clientcredentialIOT_THING_NAME,
                                           &xSocket );

        if( xStatus == pdPASS )
        {
            xStatus = GGD_JSONRequestGetSize( &xSocket, &ulJSONFileSize );
        }

        if( xStatus == pdPASS )
        {
            ulByteRead = 0;
            xStatus = prvGGD_JSONRequestGetFileLoop( ulBufferSize,
                                                     &ulByteRead,
                                                     &xJSONFileRetrieveCompleted,
                                                     ulJSONFileSize );
        }

        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );

        xStatus = GGD_GetIPandCertificateFromJSON( cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                                   ulJSONFileSize,
                                                   &xHostParameters,
                                                   &xHostAddressData,
                                                   xAutoSearchFlag );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        /** @}*/

        /** @brief Check manual connect fails if host parameters not found in JSON
         *  @{
         */
        xAutoSearchFlag = pdFALSE;
        memcpy( cBuffer, cJSON_FILE, strlen( cJSON_FILE ) );
        xHostParameters.pcCoreAddress = ( char * ) cMY_CORE_ARN;
        xHostParameters.pcGroupName = ( char * ) cBadGroupId;
        xHostParameters.ucInterface = 3;
        xStatus = GGD_GetIPandCertificateFromJSON( cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                                   ulJSONFileSize,
                                                   &xHostParameters,
                                                   &xHostAddressData,
                                                   xAutoSearchFlag );
        TEST_ASSERT_EQUAL_INT32( pdFAIL, xStatus );

        xAutoSearchFlag = pdFALSE;
        memcpy( cBuffer, cJSON_FILE, strlen( cJSON_FILE ) );
        xHostParameters.pcCoreAddress = ( char * ) cBadCoreARN;
        xHostParameters.pcGroupName = ( char * ) cMyGroupID;
        xHostParameters.ucInterface = 3;
        xStatus = GGD_GetIPandCertificateFromJSON( cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                                   ulJSONFileSize,
                                                   &xHostParameters,
                                                   &xHostAddressData,
                                                   xAutoSearchFlag );
        TEST_ASSERT_EQUAL_INT32( pdFAIL, xStatus );

        xAutoSearchFlag = pdFALSE;
        memcpy( cBuffer, cJSON_FILE, strlen( cJSON_FILE ) );
        xHostParameters.pcCoreAddress = ( char * ) cMY_CORE_ARN;
        xHostParameters.pcGroupName = ( char * ) cMyGroupID;
        xHostParameters.ucInterface = 100;
        xStatus = GGD_GetIPandCertificateFromJSON( cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                                   ulJSONFileSize,
                                                   &xHostParameters,
                                                   &xHostAddressData,
                                                   xAutoSearchFlag );
        TEST_ASSERT_EQUAL_INT32( pdFAIL, xStatus );
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }

    /** @brief Check stability if NULL pointers are passed.
     *
     *  @{
     */
    xAutoSearchFlag = pdFALSE;

    if( TEST_PROTECT() )
    {
        xHostParameters.pcCoreAddress = ( char * ) cMY_CORE_ARN;
        xHostParameters.pcGroupName = ( char * ) cMyGroupID;
        xHostParameters.ucInterface = 3;
        xAutoSearchFlag = pdFALSE;
        xStatus = GGD_GetIPandCertificateFromJSON( NULL, /*lint !e971 can use char without signed/unsigned. */
                                                   ulJSONFileSize,
                                                   &xHostParameters,
                                                   &xHostAddressData,
                                                   xAutoSearchFlag );
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        memcpy( cBuffer, cJSON_FILE, strlen( cJSON_FILE ) );
        xStatus = GGD_GetIPandCertificateFromJSON( cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                                   ulJSONFileSize,
                                                   NULL,
                                                   &xHostAddressData,
                                                   xAutoSearchFlag );
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        memcpy( cBuffer, cJSON_FILE, strlen( cJSON_FILE ) );
        xStatus = GGD_GetIPandCertificateFromJSON( cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                                   ulJSONFileSize,
                                                   &xHostParameters,
                                                   NULL,
                                                   xAutoSearchFlag );
        TEST_FAIL();
    }

    /** @}*/
}

TEST( GGD_System, JSONRequestGetFile )
{
    BaseType_t xStatus;
    uint32_t ulBufferSize = testrunnerBUFFER_SIZE;
    uint32_t ulByteRead;
    BaseType_t xJSONFileRetrieveCompleted;
    uint32_t ulJSONFileSize;

    if( TEST_PROTECT() )
    {
        /** @brief Check return status and value in ideal case.
         *  @{
         */
        xStatus = prvGGD_JSONRequestStart( clientcredentialMQTT_BROKER_ENDPOINT,
                                           clientcredentialGREENGRASS_DISCOVERY_PORT,
                                           clientcredentialIOT_THING_NAME,
                                           &xSocket );

        if( xStatus == pdPASS )
        {
            xStatus = GGD_JSONRequestGetSize( &xSocket, &ulJSONFileSize );
        }

        if( xStatus == pdPASS )
        {
            ulByteRead = 0;
            xStatus = prvGGD_JSONRequestGetFileLoop( ulBufferSize,
                                                     &ulByteRead,
                                                     &xJSONFileRetrieveCompleted,
                                                     ulJSONFileSize );
        }

        TEST_ASSERT_EQUAL_INT32( SOCKETS_INVALID_SOCKET, xSocket );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        TEST_ASSERT_EQUAL_INT32( pdTRUE, xJSONFileRetrieveCompleted );
        TEST_ASSERT_EQUAL_INT32( ulJSONFileSize, ulByteRead + 1 );

        /** @}*/

        /** @brief Retrieve the JSON file in separate chunks.
         *  @{
         */
        xStatus = prvGGD_JSONRequestStart( clientcredentialMQTT_BROKER_ENDPOINT,
                                           clientcredentialGREENGRASS_DISCOVERY_PORT,
                                           clientcredentialIOT_THING_NAME,
                                           &xSocket );

        if( xStatus == pdPASS )
        {
            xStatus = GGD_JSONRequestGetSize( &xSocket, &ulJSONFileSize );
        }

        if( xStatus == pdPASS )
        {
            ulByteRead = 0;
            xStatus = GGD_JSONRequestGetFile( &xSocket,
                                              cBuffer,
                                              ulJSONFileSize / 2,
                                              &ulByteRead,
                                              &xJSONFileRetrieveCompleted,
                                              ulJSONFileSize );


            TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
            TEST_ASSERT_EQUAL_INT32( pdFALSE, xJSONFileRetrieveCompleted ); /* Not yet retrieved - only half of it. */

            xStatus = prvGGD_JSONRequestGetFileLoop( ulBufferSize,
                                                     &ulByteRead,
                                                     &xJSONFileRetrieveCompleted,
                                                     ulJSONFileSize );
        }

        TEST_ASSERT_EQUAL_INT32_MESSAGE( pdPASS, xStatus, "GGD_JSONRequestGetFile() failed to return pdPASS." );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( pdTRUE, xJSONFileRetrieveCompleted, "GGD_JSONRequestGetFile() return pdFALSE in xJSONFileRetrieveCompleted." );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( ulJSONFileSize, ulByteRead + 1, "GGD_JSONRequestGetFile() returned ulJSONFileSize that is not ulByteRead + 1." );
        /** @}*/

        /** @brief Check fail if we receive more bytes than expected.
         *  @{
         */
        xStatus = prvGGD_JSONRequestStart( clientcredentialMQTT_BROKER_ENDPOINT,
                                           clientcredentialGREENGRASS_DISCOVERY_PORT,
                                           clientcredentialIOT_THING_NAME,
                                           &xSocket );

        if( xStatus == pdPASS )
        {
            xStatus = GGD_JSONRequestGetSize( &xSocket, &ulJSONFileSize );
        }

        if( xStatus == pdPASS )
        {
            ulByteRead = 0;
            xStatus = prvGGD_JSONRequestGetFileLoop( ulBufferSize,
                                                     &ulByteRead,
                                                     &xJSONFileRetrieveCompleted,
                                                     ulJSONFileSize - 1 ); /* Remove one byte from the expected JSON file size. */
        }

        TEST_ASSERT_EQUAL_INT32( pdFAIL, xStatus );
        TEST_ASSERT_EQUAL_INT32( pdFAIL, xJSONFileRetrieveCompleted ); /* Failed. */
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }

    /** @brief Check Stability by passing in NULL pointers or buffers too small
     *  @{
     */
    if( TEST_PROTECT() )
    {
        ulByteRead = 0;
        xStatus = GGD_JSONRequestGetFile( &xSocket,
                                          cBuffer,
                                          ulBufferSize,
                                          NULL,
                                          &xJSONFileRetrieveCompleted,
                                          ulJSONFileSize );
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        ulByteRead = 0;
        xStatus = GGD_JSONRequestGetFile( &xSocket,
                                          NULL,
                                          ulBufferSize,
                                          &ulByteRead,
                                          &xJSONFileRetrieveCompleted,
                                          ulJSONFileSize );
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        ulByteRead = 0;
        xStatus = GGD_JSONRequestGetFile( &xSocket,
                                          cBuffer,
                                          ulBufferSize,
                                          &ulByteRead,
                                          NULL,
                                          ulJSONFileSize );
        TEST_FAIL();
    }

    /** @}*/
}

TEST( GGD_System, JSONRequestGetSize )
{
    BaseType_t xStatus;
    uint32_t ulJSONFileSize;

    if( TEST_PROTECT() )
    {
        /** @brief Check return status and value in ideal case
         *  @{
         */
        xStatus = prvGGD_JSONRequestStart( clientcredentialMQTT_BROKER_ENDPOINT,
                                           clientcredentialGREENGRASS_DISCOVERY_PORT,
                                           clientcredentialIOT_THING_NAME,
                                           &xSocket );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );

        xStatus = GGD_JSONRequestGetSize( &xSocket, &ulJSONFileSize );
        TEST_ASSERT_GREATER_THAN( 1000, ulJSONFileSize );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        GGD_SecureConnect_Disconnect( &xSocket );
        /* Check no disconnect after a pass. */
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }

    /** @brief Check Statility by passing in NULL pointers
     *  @{
     */
    if( TEST_PROTECT() )
    {
        xStatus = GGD_JSONRequestGetSize( &xSocket, NULL );
        TEST_FAIL();
    }

    /** @}*/
}

TEST( GGD_System, JSONRequestStart )
{
    BaseType_t xStatus;

    if( TEST_PROTECT() )
    {
        /** @brief Check return status and value in ideal case
         *  @{
         */
        xStatus = prvGGD_JSONRequestStart( clientcredentialMQTT_BROKER_ENDPOINT,
                                           clientcredentialGREENGRASS_DISCOVERY_PORT,
                                           clientcredentialIOT_THING_NAME,
                                           &xSocket );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        GGD_SecureConnect_Disconnect( &xSocket );
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }

    /** @brief Check Statility by passing in NULL pointers
     *  @{
     */
    if( TEST_PROTECT() )
    {
        xStatus = GGD_JSONRequestStart( clientcredentialMQTT_BROKER_ENDPOINT,
                                        clientcredentialGREENGRASS_DISCOVERY_PORT,
                                        clientcredentialIOT_THING_NAME,
                                        NULL );
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        xStatus = GGD_JSONRequestStart( NULL,
                                        clientcredentialGREENGRASS_DISCOVERY_PORT,
                                        NULL,
                                        &xSocket );
        TEST_FAIL();
    }

    /** @}*/
}
