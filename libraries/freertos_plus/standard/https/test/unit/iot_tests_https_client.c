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
#define HTTPS_TEST_ROOT_CA          \
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
 * @brief Header data to share among the tests.
 */
#define HTTPS_TEST_HEADER_LINES    \
    "HTTP/1.1 200 OK\r\n"\
    "Content-Type: image/gif\r\n"\
    "Content-Length: 43\r\n"\
    "Connection: keep-alive\r\n"\
    "Date: Sun, 14 Jul 2019 06:07:52 GMT\r\n"\
    "Server: Omniture DC/2.0.0\r\n"\
    "Access-Control-Allow-Origin: *\r\n"\
    "Set-Cookie: s_vi=[CS]v1|2E95635285034EA6-6000119E0002304C[CE]; Expires=Tue, 13 Jul 2021 06:07:52 GMT; Domain=amazonwebservices.d2.sc.omtrdc.net; Path=/; SameSite=None\r\n"\
    "X-C: ms-6.8.1\r\n"\
    "Expires: Sat, 13 Jul 2019 06:07:52 GMT\r\n"\
    "Last-Modified: Mon, 15 Jul 2019 06:07:52 GMT\r\n"\
    "Pragma: no-cache\r\n"\
    "ETag: \"3356698344146796544-5233166835360424028\"\r\n"\
    "Vary: *\r\n"\
    "P3P: CP=\"This is not a P3P policy\"\r\n"\
    "xserver: www1021\r\n"\
    "Cache-Control: no-cache, no-store, max-age=0, no-transform, private\r\n"\
    "X-XSS-Protection: 1; mode=block\r\n"\
    "X-Content-Type-Options: nosniff\r\n\r\n"

/**
 * @brief Header name and values to verify reading the header.
 */
#define HTTPS_DATE_HEADER           "Date"
#define HTTPS_ETAG_HEADER           "ETag"
#define HTTPS_NONEXISTENT_HEADER    "Non-Existent-Header"
#define HTTPS_DATE_HEADER_VALUE     "Sun, 14 Jul 2019 06:07:52 GMT"
#define HTTPS_ETAG_HEADER_VALUE     "\"3356698344146796544-5233166835360424028\""

/**
 * @brief The array lengths of to store the header values.
 */
#define HTTPS_TEST_VALUE_BUFFER_LENGTH_LARGE_ENOUGH     64
#define HTTPS_TEST_VALUE_BUFFER_LENGTH_TOO_SMALL        8

/**
 * @brief The size of the connection user buffer to use among the tests.
 */
#define HTTPS_TEST_CONN_USER_BUFFER_SIZE            ( 512 )

/**
 * @brief The size of the request user buffer to use among the tests.
 */
#define HTTPS_TEST_REQ_USER_BUFFER_SIZE             ( 1024 )

/**
 * @brief the size of the respons user buffer to use among the tests.
 */
#define HTTPS_TEST_RESP_USER_BUFFER_SIZE            ( 1024 )

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
    .pNetworkInterface = &_networkInterface
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

static IotNetworkError_t _networkDestroySuccess(void * pConnection)
{
    (void)pConnection;
    return IOT_NETWORK_SUCCESS;
}

/*-----------------------------------------------------------*/

/**
 * @brief Network Abstraction close function that fails. 
 */
static IotNetworkError_t _networkDestroyFail(void * pConnection)
{
    (void)pConnection;
    return IOT_NETWORK_FAILURE;
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
 * @brief Mock the http parser execution failing.
 */
static size_t _httpParserExecuteFail( http_parser *parser,
                                      const http_parser_settings *settings,
                                      const char *data,
                                      size_t len)
{
    (void)settings;
    (void)data;
    (void)len;
    parser->http_errno = HPE_UNKNOWN;
    return 0;
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
 * @brief Get a valid response handle using _pRespUserBuffer and _reqInfo.
 */
static IotHttpsResponseHandle_t _getRespHandle( void )
{
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
    IotHttpsClient_InitializeRequest(&reqHandle, &_reqInfo);
    respHandle = reqHandle->pHttpsResponse;
    return respHandle;
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
    /* Initialize the library once. */
    IotHttpsClient_Init();

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
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ReadHeaderInvalidParameters);
    RUN_TEST_CASE( HTTPS_Client_Unit_API, ReadHeaderVaryingValues );
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
    returnCode = IotHttpsClient_Connect(NULL, pOriginalConnInfo);
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
    memcpy(&testConnInfo, pOriginalConnInfo, sizeof(IotHttpsConnectionInfo_t));
    testConnInfo.userBuffer.pBuffer = NULL;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    TEST_ASSERT_NULL(connHandle);
     /* Restore the testConnInfo for the next test. */
     testConnInfo.userBuffer.pBuffer = pOriginalConnInfo->userBuffer.pBuffer;

    /* IotHttpsConnectionInfo_t.userBuffer.bufferLen < connectionUserBufferMinimumSize */
    testConnInfo.userBuffer.bufferLen = connectionUserBufferMinimumSize - 1;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_NULL(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode);
    /* Restore the testConnInfo for the next test. */
    testConnInfo.userBuffer.bufferLen = pOriginalConnInfo->userBuffer.bufferLen;

    /* NULL IotHttpsConnectionInfo_t.pAddress in pConnConfig.  */
    testConnInfo.pAddress = NULL;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_NULL(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    /* Restore the testConnInfo for the next test. */
    testConnInfo.pAddress = pOriginalConnInfo->pAddress;
    
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
    testConnInfo.pAddress = pOriginalConnInfo->pAddress;
    testConnInfo.addressLen = pOriginalConnInfo->addressLen;

    /* IotHttpsConnectionInfo_t.alpnProtocolsLen is greater than IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH. */
    testConnInfo.alpnProtocolsLen = IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH + 1;
    returnCode = IotHttpsClient_Connect(&connHandle, &testConnInfo);
    TEST_ASSERT_NULL(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
    testConnInfo.alpnProtocolsLen = pOriginalConnInfo->alpnProtocolsLen;

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
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;

    /* Test that we receive an internal error when setReceiveCallback() returns failure. */
    _networkInterface.create = _networkCreateSuccess;
    _networkInterface.setReceiveCallback = _setReceiveCallbackFail;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    returnCode = IotHttpsClient_Connect(&connHandle, &_connInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_INTERNAL_ERROR, returnCode);
    TEST_ASSERT_NULL(connHandle);

    /* Test that we receive a connection error when create() returns failure. */
    _networkInterface.create = _networkCreateFail;
    _networkInterface.setReceiveCallback = _setReceiveCallbackSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    returnCode = IotHttpsClient_Connect(&connHandle, &_connInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_CONNECTION_ERROR, returnCode);
    TEST_ASSERT_NULL(connHandle);

    /* Test we receive an error if the connection handle is valid and connected but disconnect fails. */

    /* Get a valid connection handle and put a failure condition (a request in the process of sending). */
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL(connHandle);
    reqHandle = _getReqHandle();
    TEST_ASSERT_NOT_NULL(reqHandle);
    reqHandle->reqFinishedSending = false;
    IotDeQueue_EnqueueTail(&(connHandle->reqQ), &(reqHandle->link));

    _networkInterface.create = _networkCreateSuccess;
    _networkInterface.setReceiveCallback = _setReceiveCallbackSuccess;
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;
    returnCode = IotHttpsClient_Connect(&connHandle, &_connInfo);
    TEST_ASSERT_NOT_EQUAL(IOT_HTTPS_OK, returnCode);
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
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;

    /* Test a network close failure. */
    _networkInterface.close = _networkCloseFail;
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );

    returnCode = IotHttpsClient_Disconnect(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);
    /* The state is disconnected even if the network failed. */
    TEST_ASSERT_FALSE(connHandle->isConnected);

    /* Test a network destroy failure. */
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroyFail;
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    returnCode = IotHttpsClient_Disconnect(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);
    TEST_ASSERT_FALSE(connHandle->isConnected);

    /* Test that IOT_HTTPS_BUSY is returned when a request is in the process of sending. */
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.destroy = _networkDestroySuccess;

    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL(connHandle);
    reqHandle = _getReqHandle();
    TEST_ASSERT_NOT_NULL(reqHandle);
    reqHandle->reqFinishedSending = false;
    IotDeQueue_EnqueueTail(&(connHandle->reqQ), &(reqHandle->link));
    returnCode = IotHttpsClient_Disconnect(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_BUSY, returnCode);
    TEST_ASSERT_FALSE(connHandle->isConnected);
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that we set the connection in the correct state when a disconnect succeeds.
 */
TEST( HTTPS_Client_Unit_API, DisconnectSuccess )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
    IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
 
    /* Test a successful disconnect when there are no items in the request queue. */
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.close = _networkDestroySuccess;
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );

    returnCode = IotHttpsClient_Disconnect(connHandle);
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);
    TEST_ASSERT_FALSE(connHandle->isConnected);

    /* Test a successful disconnect when there is a request in the queue that just finished sending. 
       This case happens if the disconnect called when the network recieve callback task is in progress. */
    _networkInterface.close = _networkCloseSuccess;
    _networkInterface.close = _networkDestroySuccess;
    connHandle = _getConnHandle();
    TEST_ASSERT_NOT_NULL( connHandle );
    reqHandle = _getReqHandle();
    TEST_ASSERT_NOT_NULL( reqHandle );
    reqHandle->reqFinishedSending = true;
    IotDeQueue_EnqueueTail( &( connHandle->reqQ ), &( reqHandle->link ) );

    returnCode = IotHttpsClient_Disconnect( connHandle );
    TEST_ASSERT_EQUAL( IOT_HTTPS_OK, returnCode );
    TEST_ASSERT_FALSE( connHandle->isConnected );
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
    strncmpResult = strncmp((char*)(reqHandle->pHeaders), (char*)(_pHttpsMethodStrings[_reqInfo.method]), strlen(_pHttpsMethodStrings[_reqInfo.method]));
    TEST_ASSERT_EQUAL(0, strncmpResult);

    /* Check the request first line in the header buffer space. */
    pLocation = strstr((char*)(reqHandle->pHeaders), HTTPS_TEST_REQUEST_LINE_WITHOUT_METHOD );
    TEST_ASSERT_NOT_NULL(pLocation);

    /* Check the User-Agent header line. */
    pLocation = strstr((char*)(reqHandle->pHeaders), HTTPS_TEST_USER_AGENT_HEADER_LINE);
    TEST_ASSERT_NOT_NULL(pLocation);

    /* Check the Host header line. */
    pLocation = strstr((char*)(reqHandle->pHeaders), HTTPS_TEST_HOST_HEADER_LINE);
    TEST_ASSERT_NOT_NULL(pLocation);

    /* Check that for a NULL IotHttpsRequestInfo_t.pPath, we insert a "/" automatically. */
    pSavedPath = (char*)(_reqInfo.pPath);
    _reqInfo.pPath = NULL;
    returnCode = IotHttpsClient_InitializeRequest(&reqHandle, &_reqInfo);
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);
    TEST_ASSERT_NOT_NULL(reqHandle);
    pLocation = strstr((char*)(reqHandle->pHeaders), HTTPS_TEST_REQUEST_LINE_WITHOUT_PATH_WITHOUT_METHOD);
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
    uint8_t* headersCurBefore = NULL;

    /* Get a valid request handle with some header buffer space. */
    reqHandle = _getReqHandle();
    TEST_ASSERT_NOT_NULL( reqHandle );
    headersCurBefore = reqHandle->pHeadersCur;

    /* Write the test name and value and verify it was written correctly. */
    returnCode = IotHttpsClient_AddHeader(reqHandle, pTestName, pTestValue, testLen);
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);
    pLocation = strstr((char*)(reqHandle->pHeaders), pTestHeaderLine);
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
    pLocation = strstr((char*)(reqHandle->pHeaders), pExpectedHeaderLines);
    TEST_ASSERT_NOT_NULL(pLocation);
}

/**
 * @brief Test IotHttpsClient_ReadHeader() with various invalid parameters.
 */
TEST( HTTPS_Client_Unit_API, ReadHeaderInvalidParameters)
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    char valueBuffer[HTTPS_TEST_VALUE_BUFFER_LENGTH_LARGE_ENOUGH] = { 0 };

    /* Test a NULL response handle. */
    returnCode = IotHttpsClient_ReadHeader(NULL, HTTPS_DATE_HEADER, valueBuffer, sizeof(valueBuffer));
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);

    /* Test a NULL header name parameter. */
    returnCode = IotHttpsClient_ReadHeader(respHandle, NULL, valueBuffer, sizeof(valueBuffer));
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);

    /* Test a NULL header value buffer. */
    returnCode = IotHttpsClient_ReadHeader(respHandle, HTTPS_DATE_HEADER, NULL, sizeof(valueBuffer));
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);

    /* Test all parameters are NULL. */
    returnCode = IotHttpsClient_ReadHeader(NULL, NULL, NULL, sizeof(valueBuffer));
    TEST_ASSERT_EQUAL(IOT_HTTPS_INVALID_PARAMETER, returnCode);
}

/**
 * @brief Test IotHttpsClient_ReadHeader() with various header search parameters.
 */
TEST( HTTPS_Client_Unit_API, ReadHeaderVaryingValues )
{
    IotHttpsReturnCode_t returnCode = IOT_HTTPS_OK;
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    size_t testHeadersLen = sizeof( HTTPS_TEST_HEADER_LINES ) - 1; /* Not including NULL terminator */
    size_t headersBufferLen = 0;
    size_t copyLen = 0;
    char valueBufferLargeEnough[HTTPS_TEST_VALUE_BUFFER_LENGTH_LARGE_ENOUGH] = { 0 };
    char valueBufferTooSmall[HTTPS_TEST_VALUE_BUFFER_LENGTH_TOO_SMALL] = { 0 };

    /* Create a response handle. */
    respHandle = _getRespHandle();
    headersBufferLen = respHandle->pHeadersEnd - respHandle->pHeadersCur;
    /* Fill in with some header data. */
    if(testHeadersLen < headersBufferLen)
    {
        copyLen = testHeadersLen;
    }
    else
    {
        copyLen = headersBufferLen;
    }
    memcpy(respHandle->pHeadersCur, HTTPS_TEST_HEADER_LINES, copyLen);
    respHandle->pHeadersCur += copyLen;

    /* Test reading some header values successfully. */
    returnCode = IotHttpsClient_ReadHeader(respHandle, HTTPS_DATE_HEADER, valueBufferLargeEnough, sizeof(valueBufferLargeEnough));
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);
    TEST_ASSERT_EQUAL(0, strncmp(valueBufferLargeEnough, HTTPS_DATE_HEADER_VALUE, strlen( HTTPS_DATE_HEADER_VALUE )));
    returnCode = IotHttpsClient_ReadHeader(respHandle, HTTPS_ETAG_HEADER, valueBufferLargeEnough, sizeof(valueBufferLargeEnough));
    TEST_ASSERT_EQUAL(IOT_HTTPS_OK, returnCode);
    TEST_ASSERT_EQUAL(0, strncmp(valueBufferLargeEnough, HTTPS_ETAG_HEADER_VALUE, strlen( HTTPS_ETAG_HEADER_VALUE )));

    /* Test reading the header but the value buffer is not large enough. */
    returnCode = IotHttpsClient_ReadHeader(respHandle, HTTPS_DATE_HEADER, valueBufferTooSmall, sizeof(valueBufferTooSmall));
    TEST_ASSERT_EQUAL(IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode);
    returnCode = IotHttpsClient_ReadHeader(respHandle, HTTPS_ETAG_HEADER, valueBufferTooSmall, sizeof(valueBufferTooSmall));
    TEST_ASSERT_EQUAL(IOT_HTTPS_INSUFFICIENT_MEMORY, returnCode);
    
    /* Test reading a header value that does not exist in the header buffer. */
    returnCode = IotHttpsClient_ReadHeader(respHandle, HTTPS_NONEXISTENT_HEADER, valueBufferLargeEnough, sizeof(valueBufferLargeEnough));
    TEST_ASSERT_EQUAL(IOT_HTTPS_NOT_FOUND, returnCode);

    /* Test reading a header value with a failing parseFunc. */
    respHandle->httpParserInfo.parseFunc = _httpParserExecuteFail;
    returnCode = IotHttpsClient_ReadHeader(respHandle, HTTPS_DATE_HEADER, valueBufferLargeEnough, sizeof(valueBufferLargeEnough));
    TEST_ASSERT_EQUAL(IOT_HTTPS_PARSING_ERROR, returnCode);

    /* Test looking for a header value when there are no headers available. */
    /* Get a fresh response handle. */
    respHandle = _getRespHandle();
    returnCode = IotHttpsClient_ReadHeader(respHandle, HTTPS_DATE_HEADER, valueBufferLargeEnough, sizeof(valueBufferLargeEnough));
    TEST_ASSERT_EQUAL(IOT_HTTPS_NOT_FOUND, returnCode);
}