/*
 * Amazon FreeRTOS HTTPS Client V1.0.0
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

/**
 * @file iot_tests_https_client.c
 * @brief Tests for the user-facing API functions declared in iot_https_client.h.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* C standard includes. */
#include <string.h>

/* HTTPS Client internal include. */
#include "private/iot_https_internal.h"

/* Credential includes. */
#include "aws_clientcredential.h"
#include "aws_clientcredential_keys.h"

/* Test framework includes. */
#include "unity_fixture.h"

/*-----------------------------------------------------------*/

/**
 * @brief Test TLS TCP port.
 */
#define HTTPS_TEST_PORT                         ( (uint16_t) 443 )

/**
 * @brief Test address for all of the tests to use.
 */
#define HTTPS_TEST_ADDRESS                      "www.amazon.com "

/**
 * @brief Test HTTP/1.1 protocol for all of the tests to use.
 */
#define HTTPS_ALPN_PROTOCOL                     "http/1.1"

/**
 * @brief Baltimore Cybertrust root CA to share among the tests.
 */
#define HTTPS_TEST_ROOT_CA      \
    "-----BEGIN CERTIFICATE-----\n"\
    "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n"\
    "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n"\
    "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n"\
    "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n"\
    "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n"\
    "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n"\
    "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n"\
    "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n"\
    "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n"\
    "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n"\
    "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n"\
    "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n"\
    "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n"\
    "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n"\
    "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n"\
    "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n"\
    "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n"\
    "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n"\
    "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"\
    "-----END CERTIFICATE-----\n"

/**
 * @brief The size of the connection user buffer.
 */
#define HTTPS_TEST_CONN_USER_BUFFER_SIZE        ( 512 )

/*-----------------------------------------------------------*/

/**
 * @brief HTTPS Client connection user buffer to share among the tests.
 */
static uint8_t _pConnUserBuffer[HTTPS_TEST_CONN_USER_BUFFER_SIZE] = { 0 };

/**
 * @brief An #IotNetworkInterface_t to share among the tests.
 */
static IotNetworkInterface_t _networkInterface = { 0 };

/**
 * @brief An #IotNetworkServerInfo_t to share among the tests.
 */
static IotNetworkServerInfo_t _networkServerInfo = { 0 };

/**
 * @brief An #IotNetworkCredentials_t to share among the tests.
 */
static IotNetworkCredentials_t _networkCredentials = { 0 };

/**
 * @brief An IotHttpsConnectionInfo_t to share among the tests.
 */
static IotHttpsConnectionInfo_t _connInfo = {
    .pAddress = HTTPS_TEST_ADDRESS,
    .addressLen = sizeof( HTTPS_TEST_ADDRESS ),
    .port = HTTPS_TEST_PORT,
    .flags = 0,
    .pCaCert = HTTPS_TEST_ROOT_CA,
    .caCertLen = sizeof( HTTPS_TEST_ROOT_CA ),
    .userBuffer.pBuffer = _pConnUserBuffer,
    .userBuffer.bufferLen = sizeof( _pConnUserBuffer ),
    .pClientCert = keyCLIENT_CERTIFICATE_PEM,
    .clientCertLen = sizeof( keyCLIENT_CERTIFICATE_PEM ),
    .pPrivateKey = keyCLIENT_PRIVATE_KEY_PEM,
    .privateKeyLen = sizeof( keyCLIENT_PRIVATE_KEY_PEM ),
    .pAlpnProtocols = HTTPS_ALPN_PROTOCOL,
    .alpnProtocolsLen = sizeof(HTTPS_ALPN_PROTOCOL),
    .pNetworkInterface = &_networkInterface,
    .pNetworkServerInfo = &_networkServerInfo,
    .pNetworkCredentialInfo = &_networkCredentials
};

/*-----------------------------------------------------------*/

/**
 * @brief Network Abstraction close function that fails. 
 */
static IotNetworkError_t _networkCloseFail(void * pConnection)
{
    (void)pConnection;
    return IOT_NETWORK_FAILURE;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network Abstraction close function that succeeds.
 */
static IotNetworkError_t _networkCloseSuccess(void * pConnection)
{
    (void)pConnection;
    return IOT_NETWORK_SUCCESS;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network Abstraction create function that succeeds
 */

static IotNetworkError_t _networkCreateSuccess(void * pConnectionInfo, void * pCredentialInfo, void ** pConnection)
{
    (void)pConnectionInfo;
    (void)pCredentialInfo;
    (void)pConnection;
    return IOT_NETWORK_SUCCESS;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network Abstraction setReceiveCallback that succeeds.
 */
static IotNetworkError_t _setReceiveCallbackSuccess( void * pConnection, 
                                                     IotNetworkReceiveCallback_t receiveCallback,
                                                     void * pContext )
{
    (void)pConnection;
    (void)receiveCallback;
    (void)pContext;
    return IOT_NETWORK_SUCCESS;
}

/*-----------------------------------------------------------*/

/**
 * @brief Get a valid connected state intialized connection handle using _pConnUserBuffer and _connInfo.
 */
static IotHttpsConnectionHandle_t _getConnHandle( void )
{
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    _networkInterface.create = _networkCreateSuccess;
    _networkInterface.setReceiveCallback = _setReceiveCallbackSuccess;
    IotHttpsClient_Connect(&connHandle, &_connInfo);
    return connHandle;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group for HTTPS Client API tests.
 */
TEST_GROUP( HTTPS_Client_Unit_API );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for HTTPS Client API tests.
 */
TEST_SETUP( HTTPS_Client_Unit_API )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for HTTPS Client API tests.
 */
TEST_TEAR_DOWN( HTTPS_Client_Unit_API )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for HTTPS Client API tests.
 */
TEST_GROUP_RUNNER( HTTPS_Client_Unit_API )
{
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ConnectInvalidParameters);
    RUN_TEST_CASE( HTTPS_Client_Unit_API, DisconnectInvalidParameters );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, DisconnectFailure );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, DisconnectSuccess );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test various invalid parameters in the @ref https_client_function_connect API. 
 */
TEST( HTTPS_Client_Unit_API, ConnectInvalidParameters)
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionInfo_t testConnInfo = IOT_HTTPS_CONNECTION_INFO_INITIALIZER;
    IotHttpsConnectionInfo_t* pOriginalConnInfo = &_connInfo;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;

    /* NULL pConnHandle  */
    returnCode = IotHttpsClient_Connect(NULL, &_connInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);

    /* NULL pConnConfig */
    returnCode = IotHttpsClient_Connect(&connHandle, NULL);
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode);

    /* Both pConnHandle and pConnConfig are NULL. */
    returnCode = IotHttpsClient_Connect(NULL, NULL);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);

    /* NULL IotHttpsConnectionInfo_t.pAddress in pConnConfig.  */
    memcpy(&testConnInfo, &_connInfo, sizeof(IotHttpsConnectionInfo_t));
    testConnInfo.pAddress = NULL;
    testConnInfo.pNetworkServerInfo = NULL;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    /* Restore the testConnInfo for the next test. */
    testConnInfo.pAddress = _connInfo.pAddress;
    
    /* IotHttpsConnectionInfo_t.addressLen is zero. */
    testConnInfo.addressLen = 0;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);

    /* IotHttpsConnectionInfo_t.addressLen is greater than IOT_HTTPS_MAX_HOST_NAME_LENGTH */
    testConnInfo.addressLen = IOT_HTTPS_MAX_HOST_NAME_LENGTH + 1;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    
    /* Both IotHttpsConnectionInfo_t.addressLen and IotHttpsConnectionInfo_t.pAddress are NULL. */
    testConnInfo.pAddress = NULL;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    /* Restore the testConnInfo for the next test. */
    testConnInfo.pAddress = _connInfo.pAddress;
    testConnInfo.addressLen = _connInfo.addressLen;
    testConnInfo.pNetworkServerInfo = _connInfo.pNetworkServerInfo;

    /* IotHttpsConnectionInfo_t.alpnProtocolsLen is greater than IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH. */
    testConnInfo.alpnProtocolsLen = IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH + 1;
    testConnInfo.pNetworkCredentialInfo = NULL;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    testConnInfo.alpnProtocolsLen = _connInfo.alpnProtocolsLen;
    testConnInfo.pNetworkCredentialInfo = _connInfo.pNetworkCredentialInfo;

    /* Unknown IotHttpsConnectionInfo_t.pFlags are ignored, so that is not tested. */
    /* NULL certificates will result in a network error in the underlying network stack, but is allowed in the API so '
       that is not tested here. */

    /* IotHttpsConnectionInfo_t.pNetworkInterface is NULL. */
    testConnInfo.pNetworkInterface = NULL;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test various invalid parameters in the @ref https_client_function_disconnect API. 
 */
TEST( HTTPS_Client_Unit_API, DisconnectInvalidParameters)
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;

    /* NULL connHandle. */
    returnCode = IotHttpsClient_Disconnect(NULL);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that we set the connection in the correct state when a disconnect fails.
 */
TEST( HTTPS_Client_Unit_API, DisconnectFailure )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;

    /* Set the network interface close to mock a failure. */
    _networkInterface.close = _networkCloseFail;
    connHandle = _getConnHandle();

    returnCode = IotHttpsClient_Disconnect(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_NETWORK_ERROR, returnCode);
    /* The state is disconnected even if the network failed. */
    TEST_ASSERT_FALSE(connHandle->isConnected);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that we set the connection in  the correct state when a disconnect succeeds.
 */
TEST( HTTPS_Client_Unit_API, DisconnectSuccess )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
 
    /* Set the network interface close to mock a success. */
    _networkInterface.close = _networkCloseSuccess;
    connHandle = _getConnHandle();

    returnCode = IotHttpsClient_Disconnect(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);
    TEST_ASSERT_FALSE(connHandle->isConnected);
}
