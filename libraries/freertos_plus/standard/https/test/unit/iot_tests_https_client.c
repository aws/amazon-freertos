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
#define HTTPS_TEST_PORT                             ( (uint16_t) 443 )

/**
 * @brief Test address to share among the tests.
 */
#define HTTPS_TEST_ADDRESS                          "www.amazon.com "

/**
 * @brief Test path to share among the tests.
 */
#define HTTPS_TEST_PATH                             "/path.txt"

/**
 * @brief Test HTTP method to share among the tests.
 */
#define HTTPS_TEST_METHOD                           IOT_HTTPS_METHOD_GET

/**
 * @brief Expected HTTP request line without the method.
 */
#define HTTPS_TEST_REQUEST_LINE_WITHOUT_METHOD      HTTPS_TEST_PATH " " HTTPS_PROTOCOL_VERSION HTTPS_END_OF_HEADER_LINES_INDICATOR

/**
 * @brief Expected HTTP request line without the path and without the method.
 */
#define HTTPS_TEST_REQUEST_LINE_WITHOUT_PATH_WITHOUT_METHOD     HTTPS_EMPTY_PATH " " HTTPS_PROTOCOL_VERSION HTTPS_END_OF_HEADER_LINES_INDICATOR

/**
 * @brief Expected HTTP User-Agent header line.
 */
#define HTTPS_TEST_USER_AGENT_HEADER_LINE           HTTPS_USER_AGENT_HEADER HTTPS_HEADER_FIELD_SEPARATOR IOT_HTTPS_USER_AGENT HTTPS_END_OF_HEADER_LINES_INDICATOR

/**
 * @brief Expected HTTP Host header line.
 */
#define HTTPS_TEST_HOST_HEADER_LINE                 HTTPS_HOST_HEADER HTTPS_HEADER_FIELD_SEPARATOR HTTPS_TEST_ADDRESS HTTPS_END_OF_HEADER_LINES_INDICATOR

/**
 * @brief Test HTTP/1.1 protocol to share among the tests.
 */
#define HTTPS_TEST_ALPN_PROTOCOL                    "http/1.1"

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
 * @brief The size of the connection user buffer to use among the tests.
 */
#define HTTPS_TEST_CONN_USER_BUFFER_SIZE            ( 512 )

/**
 * @brief The size of the request user buffer to use among the tests.
 */
#define HTTPS_TEST_REQ_USER_BUFFER_SIZE             ( 512 )

/**
 * @brief the size of the respons user buffer to use among the tests.
 */
#define HTTPS_TEST_RESP_USER_BUFFER_SIZE            ( 512 )

/*-----------------------------------------------------------*/

/**
 * @brief HTTPS Client connection user buffer to share among the tests.
 */
static uint8_t _pConnUserBuffer[HTTPS_TEST_CONN_USER_BUFFER_SIZE] = { 0 };

/**
 * @brief HTTPS Request user buffer to share among the tests.
 */
static uint8_t _pReqUserBuffer[HTTPS_TEST_REQ_USER_BUFFER_SIZE] = { 0 };

/**
 * @brief HTTPS Response user buffer to share among the tests.
 */
static uint8_t _pRespUserBuffer[HTTPS_TEST_RESP_USER_BUFFER_SIZE] = { 0 };

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
 * @brief A IotHttpsConnectionInfo_t to share among the tests.
 */
static IotHttpsConnectionInfo_t _connInfo = {
    .pAddress = HTTPS_TEST_ADDRESS,
    .addressLen = sizeof( HTTPS_TEST_ADDRESS ) - 1,
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
    .pAlpnProtocols = HTTPS_TEST_ALPN_PROTOCOL,
    .alpnProtocolsLen = sizeof(HTTPS_TEST_ALPN_PROTOCOL),
    .pNetworkInterface = &_networkInterface,
    .pNetworkServerInfo = &_networkServerInfo,
    .pNetworkCredentialInfo = &_networkCredentials
};

/**
 * @brief A IotHttpsSyncRequestInfo_t to share among the tests.
 * TODO: - Create a separate test file for testing synchronous API verification.
 *       - Create a separate test file for testing asynchronous API verification.
 *       - Move the shared static variables to a separate header to be extern'ed.
 */ 
static IotHttpsSyncRequestInfo_t _syncInfo = IOT_HTTPS_SYNC_REQUEST_INFO_INITIALIZER;

/**
 * @brief A IotHttpsRequestInfo_t to share among the tests. 
 */
static IotHttpsRequestInfo_t _reqInfo = {
    .pPath = HTTPS_TEST_PATH,
    .pathLen = sizeof( HTTPS_TEST_PATH ) - 1,
    .method = HTTPS_TEST_METHOD,
    .pHost = HTTPS_TEST_ADDRESS,
    .hostLen = sizeof( HTTPS_TEST_ADDRESS ) - 1,
    .isNonPersistent = false,
    .reqUserBuffer.pBuffer = _pReqUserBuffer,
    .reqUserBuffer.bufferLen = sizeof( _pReqUserBuffer ),
    .respUserBuffer.pBuffer = _pRespUserBuffer,
    .respUserBuffer.bufferLen = sizeof( _pRespUserBuffer ),
    .isAsync = false,
    .pSyncInfo = &_syncInfo,
    .pConnInfo = NULL
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
 * @brief Network Abstraction create function that succeeds.
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
 * @brief Network Abstraction create function that fails.
 */
static IotNetworkError_t _networkCreateFail(void * pConnectionInfo, void * pCredentialInfo, void ** pConnection)
{
    (void)pConnectionInfo;
    (void)pCredentialInfo;
    (void)pConnection;
    return IOT_NETWORK_FAILURE;
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
 * @brief Network Abstraction setReceiveCallback that fails.
 */
static IotNetworkError_t _setReceiveCallbackFail( void * pConnection, 
                                                     IotNetworkReceiveCallback_t receiveCallback,
                                                     void * pContext )
{
    (void)pConnection;
    (void)receiveCallback;
    (void)pContext;
    return IOT_NETWORK_FAILURE;
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
 * @brief Get a valid request handle using _pReqUserBuffer and _reqInfo.
 */
static IotHttpsRequestHandle_t _getReqHandle( void )
{
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsClient_InitializeRequest(&reqHandle, &_reqInfo);
    return reqHandle;
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
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ConnectFailure);
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ConnectSuccess);
    RUN_TEST_CASE( HTTPS_Client_Unit_API, DisconnectInvalidParameters );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, DisconnectFailure );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, DisconnectSuccess );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, InitializeRequestInvalidParameters);
    RUN_TEST_CASE( HTTPS_Client_Unit_API, InitializeRequestFormatCheck );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, AddHeaderInvalidParameters);
    RUN_TEST_CASE( HTTPS_Client_Unit_API, AddHeaderFormatCheck );
    RUN_TEST_CASE( HTTPS_Client_Unit_API, AddHeaderMultipleHeaders );
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
    TEST_ASSERT_NULL(connHandle);

    /* NULL pConnConfig */
    returnCode = IotHttpsClient_Connect(&connHandle, NULL);
    TEST_ASSERT_EQUAL( IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(connHandle);

    /* Both pConnHandle and pConnConfig are NULL. */
    returnCode = IotHttpsClient_Connect(NULL, NULL);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(connHandle);
    
    /* IoHttpsConnectInfo_t.userBuffer.pBuffer is NULL. */
    memcpy(&testConnInfo, &_connInfo, sizeof(IotHttpsConnectionInfo_t));
    testConnInfo.userBuffer.pBuffer = NULL;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(connHandle);
     /* Restore the testConnInfo for the next test. */
     testConnInfo.userBuffer.pBuffer = _connInfo.userBuffer.pBuffer;

    /* IotHttpsConnectionInfo_t.userBuffer.bufferLen < connectionUserBufferMinimumSize */
    testConnInfo.userBuffer.bufferLen = connectionUserBufferMinimumSize - 1;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_NULL(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode);
    /* Restore the testConnInfo for the next test. */
    testConnInfo.userBuffer.bufferLen = _connInfo.userBuffer.bufferLen;

    /* NULL IotHttpsConnectionInfo_t.pAddress in pConnConfig.  */
    testConnInfo.pAddress = NULL;
    testConnInfo.pNetworkServerInfo = NULL;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_NULL(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    /* Restore the testConnInfo for the next test. */
    testConnInfo.pAddress = _connInfo.pAddress;
    
    /* IotHttpsConnectionInfo_t.addressLen is zero. */
    testConnInfo.addressLen = 0;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_NULL(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);

    /* IotHttpsConnectionInfo_t.addressLen is greater than IOT_HTTPS_MAX_HOST_NAME_LENGTH */
    testConnInfo.addressLen = IOT_HTTPS_MAX_HOST_NAME_LENGTH + 1;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_NULL(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    
    /* Both IotHttpsConnectionInfo_t.addressLen and IotHttpsConnectionInfo_t.pAddress are NULL. */
    testConnInfo.pAddress = NULL;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_NULL(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    /* Restore the testConnInfo for the next test. */
    testConnInfo.pAddress = _connInfo.pAddress;
    testConnInfo.addressLen = _connInfo.addressLen;
    testConnInfo.pNetworkServerInfo = _connInfo.pNetworkServerInfo;

    /* IotHttpsConnectionInfo_t.alpnProtocolsLen is greater than IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH. */
    testConnInfo.alpnProtocolsLen = IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH + 1;
    testConnInfo.pNetworkCredentialInfo = NULL;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_NULL(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    testConnInfo.alpnProtocolsLen = _connInfo.alpnProtocolsLen;
    testConnInfo.pNetworkCredentialInfo = _connInfo.pNetworkCredentialInfo;

    /* Unknown IotHttpsConnectionInfo_t.pFlags are ignored, so that is not tested. */
    /* NULL certificates will result in a network error in the underlying network stack, but is allowed in the API so '
       that is not tested here. */

    /* IotHttpsConnectionInfo_t.pNetworkInterface is NULL. */
    testConnInfo.pNetworkInterface = NULL;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_NULL(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
}

/* --------------------------------------------------------- */

/**
 * @brief Test a connection failing in few network abstraction upset scenarios.
 */
TEST( HTTPS_Client_Unit_API, ConnectFailure)
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;

    /* Test that we receive an internal error when setReceiveCallback() returns failure. */
    _networkInterface.create = _networkCreateSuccess;
    _networkInterface.setReceiveCallback = _setReceiveCallbackFail;
    _networkInterface.close = _networkCloseSuccess;
    returnCode = IotHttpsClient_Connect(&connHandle, &_connInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INTERNAL_ERROR, returnCode);
    TEST_ASSERT_NULL(connHandle);

    /* Test that we receive a connection error when create() returns failure. */
    _networkInterface.create = _networkCreateFail;
    _networkInterface.setReceiveCallback = _setReceiveCallbackSuccess;
    _networkInterface.close = _networkCloseSuccess;
    returnCode = IotHttpsClient_Connect(&connHandle, &_connInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_CONNECTION_ERROR, returnCode);
    TEST_ASSERT_NULL(connHandle);

}

/* --------------------------------------------------------- */

/**
 * @brief Test a connection succeeding.
 */
TEST( HTTPS_Client_Unit_API, ConnectSuccess)
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;

    _networkInterface.create = _networkCreateSuccess;
    _networkInterface.setReceiveCallback = _setReceiveCallbackSuccess;
    returnCode = IotHttpsClient_Connect(&connHandle, &_connInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);
    TEST_ASSERT_NOT_NULL(connHandle);
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
    TEST_ASSERT_NOT_NULL( connHandle );

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
    TEST_ASSERT_NOT_NULL( connHandle );

    returnCode = IotHttpsClient_Disconnect(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);
    TEST_ASSERT_FALSE(connHandle->isConnected);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test intitializing an HTTP request with various invalid parameters.
 */
TEST( HTTPS_Client_Unit_API, InitializeRequestInvalidParameters)
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsRequestInfo_t testReqInfo = IOT_HTTPS_REQUEST_INFO_INITIALIZER;

    /* Test NULL pReqHandle. */
    returnCode = IotHttpsClient_InitializeRequest(NULL, &_reqInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(reqHandle);

    /* Test NULL reqInfo. */
    returnCode = IotHttpsClient_InitializeRequest(&reqHandle, NULL);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(reqHandle);

    /* Test NULL pReqHandle and NULL reqInfo. */
    returnCode = IotHttpsClient_InitializeRequest(NULL, NULL);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(reqHandle);

    /* Test the request context does not fit into the user buffer. */
    memcpy(&testReqInfo, &_reqInfo, sizeof(IotHttpsRequestInfo_t));
    testReqInfo.reqUserBuffer.bufferLen = requestUserBufferMinimumSize - 1;
    returnCode = IotHttpsClient_InitializeRequest(&reqHandle, &testReqInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode);
    TEST_ASSERT_NULL(reqHandle);
    /* Restore the local IotHttpsRequestInfo_t to use in the next tests. */
    testReqInfo.reqUserBuffer.bufferLen = _reqInfo.reqUserBuffer.bufferLen;

    /* Test the response context does not fit into the user buffer. */
    testReqInfo.respUserBuffer.bufferLen = responseUserBufferMinimumSize - 1;
    returnCode = IotHttpsClient_InitializeRequest(&reqHandle, &testReqInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode);
    TEST_ASSERT_NULL(reqHandle);
    /* Restore the local IotHttpsRequestInfo_t to use in the next tests. */
    testReqInfo.respUserBuffer.bufferLen = _reqInfo.respUserBuffer.bufferLen;

    /* Test that the first line in the HTTP request message does not fit into the header space of the user buffer. */
    testReqInfo.pathLen = sizeof(_pReqUserBuffer);
    returnCode = IotHttpsClient_InitializeRequest(&reqHandle, &testReqInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode);
    TEST_ASSERT_NULL(reqHandle);
    /* Restore the local IotHttpsRequestInfo_t to use in the next tests. */
    testReqInfo.pathLen = _reqInfo.pathLen;

    /* Test a NULL IotHttpsRequestInfo_t.pHost. */
    testReqInfo.pHost = NULL;
    returnCode = IotHttpsClient_InitializeRequest(&reqHandle, &testReqInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(reqHandle);
    /* Restore the local IotHttpsRequestInfo_t to use in the next tests. */
    testReqInfo.pHost = _reqInfo.pHost;

    /* Test that the HTTP Host header does not fit into the request. */
    testReqInfo.hostLen = sizeof(_pReqUserBuffer);
    returnCode = IotHttpsClient_InitializeRequest(&reqHandle, &testReqInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode);
    TEST_ASSERT_NULL(reqHandle);
    /* Restore the local IotHttpsRequestInfo_t to use in the next tests. */
    testReqInfo.hostLen = _reqInfo.hostLen;

    /* Test a NULL request user buffer. */
    testReqInfo.reqUserBuffer.pBuffer = NULL;
    returnCode = IotHttpsClient_InitializeRequest(&reqHandle, &testReqInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(reqHandle);
    /* Restore the local IotHttpsRequestInfo_t to use in the next tests. */
    testReqInfo.reqUserBuffer.pBuffer = _reqInfo.reqUserBuffer.pBuffer;

    /* Test a NULL response user buffer. */
    testReqInfo.respUserBuffer.pBuffer = NULL;
    returnCode = IotHttpsClient_InitializeRequest(&reqHandle, &testReqInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(reqHandle);
    /* Restore the local IotHttpsRequestInfo_t to use in the next tests. */
    testReqInfo.respUserBuffer.pBuffer = _reqInfo.respUserBuffer.pBuffer;

    /* If IotHttpsRequestInfo_t.isAsync is false, then pSyncInfo must not be NULL. */
    testReqInfo.isAsync = false;
    testReqInfo.pSyncInfo = NULL;
    returnCode = IotHttpsClient_InitializeRequest(&reqHandle, &testReqInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(reqHandle);
    /* Restore the local IotHttpsRequestInfo_t to use in the next tests. */
    testReqInfo.isAsync = _reqInfo.isAsync;
    testReqInfo.pSyncInfo = _reqInfo.pSyncInfo;

    /* If IotHttpsRequestInfo_t.isAsync is true, then pAsyncInfo must not be NULL. */
    testReqInfo.isAsync = true;
    testReqInfo.pAsyncInfo = NULL;
    returnCode = IotHttpsClient_InitializeRequest(&reqHandle, &testReqInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(reqHandle);
    /* Restore the local IotHttpsRequestInfo_t to use in the next tests. */
    testReqInfo.isAsync = _reqInfo.isAsync;
    testReqInfo.pSyncInfo = _reqInfo.pSyncInfo;
}

/*-----------------------------------------------------------*/

/**
 * @brief Verify that the intialized request is in the standard HTTP format expected in the user buffer header space.
 */
TEST(HTTPS_Client_Unit_API, InitializeRequestFormatCheck )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    char * pLocation = NULL;
    int strncmpResult = -1;
    char * pSavedPath = NULL;

    /* Initialize the request using the statically defined configurations. */
    returnCode = IotHttpsClient_InitializeRequest(&reqHandle, &_reqInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);
    TEST_ASSERT_NOT_NULL(reqHandle);

    /* Check that the HTTP method is correct at the start of the header buffer space. */
    strncmpResult = strncmp(reqHandle->pHeaders, _pHttpsMethodStrings[_reqInfo.method], strlen(_pHttpsMethodStrings[_reqInfo.method]));
    TEST_ASSERT_EQUAL(0, strncmpResult);

    /* Check the request first line in the header buffer space. */
    pLocation = strstr(reqHandle->pHeaders, HTTPS_TEST_REQUEST_LINE_WITHOUT_METHOD );
    TEST_ASSERT_NOT_NULL(pLocation);

    /* Check the User-Agent header line. */
    pLocation = strstr(reqHandle->pHeaders, HTTPS_TEST_USER_AGENT_HEADER_LINE);
    TEST_ASSERT_NOT_NULL(pLocation);

    /* Check the Host header line. */
    pLocation = strstr(reqHandle->pHeaders, HTTPS_TEST_HOST_HEADER_LINE);
    TEST_ASSERT_NOT_NULL(pLocation);

    /* Check that for a NULL IotHttpsRequestInfo_t.pPath, we insert a "/" automatically. */
    pSavedPath = _reqInfo.pPath;
    _reqInfo.pPath = NULL;
    returnCode = IotHttpsClient_InitializeRequest(&reqHandle, &_reqInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);
    TEST_ASSERT_NOT_NULL(reqHandle);
    pLocation = strstr(reqHandle->pHeaders, HTTPS_TEST_REQUEST_LINE_WITHOUT_PATH_WITHOUT_METHOD);
    TEST_ASSERT_NOT_NULL(pLocation);
    /* Restore the IotHttpsRequestInfo_t.pPath for other tests. */
    _reqInfo.pPath = pSavedPath;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test IotHttpsClient_AddHeader() with various invalid parameters.
 */
TEST( HTTPS_Client_Unit_API, AddHeaderInvalidParameters)
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    char * pTestName = "Accept";
    char * pTestValue = "text";
    char * pTestContentLengthValueStr = "0";
    uint32_t testLen = strlen(pTestValue);

    /* Get a valid request handle with some header buffer space. */
    reqHandle = _getReqHandle();
    TEST_ASSERT_NOT_NULL( reqHandle );

    /* Test parameter reqHandle is NULL. */
    returnCode = IotHttpsClient_AddHeader(NULL, pTestName, pTestValue, testLen);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);

    /* Test parameter pTestName is NULL. */
    returnCode = IotHttpsClient_AddHeader(reqHandle, NULL, pTestValue, testLen);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);

    /* Test parameter pTestValue is NULL. */
    returnCode = IotHttpsClient_AddHeader(reqHandle, pTestName, NULL, testLen);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);

    /* Test all pointer parameters are NULL. */
    returnCode = IotHttpsClient_AddHeader(NULL, NULL, NULL, testLen);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);

    /* Test adding auto-generated headers. */
    returnCode = IotHttpsClient_AddHeader(reqHandle, HTTPS_USER_AGENT_HEADER, IOT_HTTPS_USER_AGENT, strlen(IOT_HTTPS_USER_AGENT));
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    returnCode = IotHttpsClient_AddHeader(reqHandle, HTTPS_HOST_HEADER, HTTPS_TEST_ADDRESS, strlen(HTTPS_TEST_ADDRESS));
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    returnCode = IotHttpsClient_AddHeader(reqHandle, HTTPS_CONTENT_LENGTH_HEADER, pTestContentLengthValueStr, strlen(pTestContentLengthValueStr));
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    returnCode = IotHttpsClient_AddHeader(reqHandle, HTTPS_CONNECTION_HEADER, HTTPS_CONNECTION_KEEP_ALIVE_HEADER_VALUE, strlen(HTTPS_CONNECTION_KEEP_ALIVE_HEADER_VALUE));
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);

    /* Test The length of the resulting header line exceeding the header buffer space. */
    returnCode = IotHttpsClient_AddHeader(reqHandle, pTestName, pTestValue, sizeof(_pReqUserBuffer));
    TEST_ASSERT_EQUAL(IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode);
}

/*-----------------------------------------------------------*/

/**
 * @brief Add a header to the HTTP request and verify the format is correct.
 */
TEST( HTTPS_Client_Unit_API, AddHeaderFormatCheck )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    char * pTestName = "Accept";
    char * pTestValue = "text";
    char * pTestHeaderLine = "Accept: text\r\n";
    uint32_t testLen = strlen(pTestValue);
    char * pLocation = NULL;
    int strncmpResult = -1;
    int headersCurBefore = NULL;

    /* Get a valid request handle with some header buffer space. */
    reqHandle = _getReqHandle();
    TEST_ASSERT_NOT_NULL( reqHandle );
    headersCurBefore = (int)(reqHandle->pHeadersCur);

    /* Write the test name and value and verify it was written correctly. */
    returnCode = IotHttpsClient_AddHeader(reqHandle, pTestName, pTestValue, testLen);
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);
    pLocation = strstr(reqHandle->pHeaders, pTestHeaderLine);
    TEST_ASSERT_NOT_NULL(pLocation);
    /* Check that the internal headersCur got incremented. */
    TEST_ASSERT_GREATER_THAN(headersCurBefore, reqHandle->pHeadersCur);
}

/*-----------------------------------------------------------*/

/**
 * @brief Add multiple headers to the header buffer space in the request. 
 */
TEST( HTTPS_Client_Unit_API, AddHeaderMultipleHeaders )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    /* Intead of iterating in a loop, all the dummy headers are declared immediately because in the unit testing
       infrastructure and workflow the number of repetitions is typically not changed from the original.  */
    char * pHeader0 = "header0";
    char * pHeader1 = "header1";
    char * pHeader2 = "header2";
    char * pValue0 = "value0";
    char * pValue1 = "value1";
    char * pValue2 = "value2";
    char * pExpectedHeaderLines = 
        "header0: value0\r\n"
        "header1: value1\r\n"
        "header2: value2\r\n";
    char * pLocation = NULL;
    
    /* Get a valid request handle with some header buffer space. */
    reqHandle = _getReqHandle();
    TEST_ASSERT_NOT_NULL( reqHandle );
    
    returnCode = IotHttpsClient_AddHeader(reqHandle, pHeader0, pValue0, strlen(pValue0));
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);
    returnCode = IotHttpsClient_AddHeader(reqHandle, pHeader1, pValue1, strlen(pValue1));
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);    
    returnCode = IotHttpsClient_AddHeader(reqHandle, pHeader2, pValue2, strlen(pValue2));
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);
    pLocation = strstr(reqHandle->pHeaders, pExpectedHeaderLines);
    TEST_ASSERT_NOT_NULL(pLocation);
}