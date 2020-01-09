#include <stdbool.h>

#include "unity.h"

#include"portableDefs.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "mock_sockets.h"
#include "mock_portable.h"
#include "mock_task.h"
#include "mock_iot_tls.h"
#include "mock_iot_logging_task.h"
#include "mock_iot_wifi.h"

#include "iot_secure_sockets.h"

#define BUFFER_LEN 100

/* ==========================  FUNCTION PROTOTYPES  ========================= */
static void initCallbacks();


/* ============================  GLOBAL VARIABLES =========================== */

static uint16_t malloc_free_calls = 0;

/* ==========================  CALLBACK FUNCTIONS =========================== */
void* malloc_cb(size_t size, int numCalls)
{
    malloc_free_calls++;
    return malloc(size);
}

void free_cb(void* ptr, int numCalls)
{
    malloc_free_calls--;
    free(ptr);
}
void sleep_cb(int time, int numCalls)
{
    /* do nothing... we should not sleep */
}
/* ============================   UNITY FIXTURES ============================ */
void setUp(void)
{
    initCallbacks();
    malloc_free_calls = 0;
}

/* called before each testcase */
void tearDown(void)
{
    TEST_ASSERT_EQUAL_INT_MESSAGE(malloc_free_calls, 0,
           "free is not called the same number of times as malloc, \
            you might have a memory leak!!");
}

/* called at the beginning of the whole suite */
void suiteSetUp()
{
}

/* called at the end of the whole suite */
int suiteTearDown(int numFailures)
{
}
/* ==========================  Helper functions  ============================ */
static Socket_t initSocket()
{
    pvPortMalloc_Stub(malloc_cb);
    lwip_socket_ExpectAndReturn(SOCKETS_AF_INET,
                                SOCKETS_SOCK_STREAM ,
                                SOCKETS_IPPROTO_TCP,
                                5); /* socket descriptor */

    /* Socket_t is a pointer it's ok to return it from the stack */
    Socket_t so = SOCKETS_Socket(SOCKETS_AF_INET,
                                 SOCKETS_SOCK_STREAM ,
                                 SOCKETS_IPPROTO_TCP);

    TEST_ASSERT_NOT_EQUAL_MESSAGE(SOCKETS_INVALID_SOCKET, so,
                                  "Error calling SOCKETS_Socket");
    return so;
}

static void deinitSocket(Socket_t so)
{
    uint32_t ret;
    vPortFree_StubWithCallback(free_cb);
    vTaskDelay_Ignore();
    lwip_close_IgnoreAndReturn(0);
    ret = SOCKETS_Close(so);
    TEST_ASSERT_EQUAL_INT_MESSAGE(SOCKETS_ERROR_NONE, ret,
            "ERROR: Could not close socket");
}

static void initCallbacks(void)
{
    pvPortMalloc_Stub(malloc_cb);
    vPortFree_StubWithCallback(free_cb);
}

static void uninitCallbacks(void)
{
    pvPortMalloc_Stub(NULL);
    vPortFree_StubWithCallback(NULL);
}

/* enforce tls, this is a one way option */
static void setTLSMode(Socket_t s)
{
    int32_t ret = SOCKETS_SetSockOpt(s, 0, SOCKETS_SO_REQUIRE_TLS, NULL ,0);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ERROR_NONE, ret);
}

static void setALPN_protocols(Socket_t s, int count)
{
    TEST_ASSERT_TRUE(count <= 2);
    char * pcAlpns[] = { socketsAWS_IOT_ALPN_MQTT , socketsAWS_IOT_ALPN_MQTT};

    int32_t ret = SOCKETS_SetSockOpt(s,
                                     0, /* Level - Unused. */
                                     SOCKETS_SO_ALPN_PROTOCOLS,
                                     pcAlpns,
                                     count);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ERROR_NONE, ret);
}

static void setServerNameIndication(Socket_t s)
{
    int32_t ret = SOCKETS_SetSockOpt(s,
                                     0,
                                     SOCKETS_SO_SERVER_NAME_INDICATION,
                                     "destination",
                                     11);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ERROR_NONE, ret);
}

static void setServerCert(Socket_t s)
{
    int32_t ret = SOCKETS_SetSockOpt(s,
                                     0,
                                     SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE,
                                     "certificate",
                                     11);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ERROR_NONE, ret);
}

static void connectSocket(Socket_t s)
{
    int32_t ret;
    SocketsSockaddr_t pxAddress;
    lwip_connect_ExpectAnyArgsAndReturn(0);
    ret = SOCKETS_Connect(s, &pxAddress, sizeof(SocketsSockaddr_t));
    TEST_ASSERT_EQUAL_INT(SOCKETS_ERROR_NONE, ret);
}

static Socket_t create_TLS_connection()
{
    Socket_t s = initSocket();
    setTLSMode(s);
    TLS_Init_IgnoreAndReturn(pdFREERTOS_ERRNO_NONE);
    TLS_Connect_IgnoreAndReturn(pdFREERTOS_ERRNO_NONE);
    connectSocket(s);
    return s;
}

static Socket_t create_normal_connection()
{
    Socket_t s = initSocket();
    connectSocket(s);
    return s;
}
/* ======================  TESTING SOCKETS_Send  ============================ */
/*!
 * @brief A happy send case with normal sockets
 */
void test01_SecureSockets_send_successful(void)
{
    int32_t  ret;
    Socket_t so = create_normal_connection();
    const char buffer[BUFFER_LEN];
    lwip_send_ExpectAnyArgsAndReturn(BUFFER_LEN);
    ret = SOCKETS_Send(so,
                       buffer,
                       BUFFER_LEN,
                       0);
    TEST_ASSERT_EQUAL_INT(BUFFER_LEN, ret);
    deinitSocket(so);
}

/*!
 * @brief A happy send case with tls sockets
 */
void test02_SecureSockets_send_successful_tls(void)
{
    int32_t  ret;
    const char buffer[BUFFER_LEN];

    Socket_t so = create_TLS_connection();

    TLS_Send_ExpectAnyArgsAndReturn(BUFFER_LEN);
    ret = SOCKETS_Send(so,
                       buffer,
                       BUFFER_LEN,
                       0);
    TEST_ASSERT_EQUAL_INT(BUFFER_LEN, ret);
    TLS_Cleanup_ExpectAnyArgs();
    deinitSocket(so);
}

/*!
 * @brief Test various bad parameters
 */
void test03_SecureSockets_send_invalid_parameters(void)
{
    Socket_t s = SOCKETS_INVALID_SOCKET;
    const char buffer[BUFFER_LEN];
    int32_t  ret;
    /* test invalid socket */
    ret = SOCKETS_Send(s,
                       buffer,
                       BUFFER_LEN,
                       0);
    TEST_ASSERT_EQUAL_INT(SOCKETS_SOCKET_ERROR, ret);

    /* test null buffer */
    s = initSocket();
    ret = SOCKETS_Send(s,
                       NULL, /* null buffer */
                       BUFFER_LEN,
                       0);
    TEST_ASSERT_EQUAL_INT(SOCKETS_EINVAL, ret);

    /* test zero datalength */
    ret = SOCKETS_Send(s,
                       buffer,
                       0,
                       0);
    TEST_ASSERT_EQUAL_INT(SOCKETS_EINVAL, ret);
    deinitSocket(s);
}

void test04_SecureSockets_send_not_connected(void)
{
    int32_t ret;
    const char buffer[BUFFER_LEN];

    Socket_t so = initSocket();

    ret = SOCKETS_Send(so,
                       buffer,
                       BUFFER_LEN,
                       0);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ENOTCONN, ret);
    deinitSocket(so);
}

/* =====================  TESTING SOCKETS_Socket  =========================== */
/*!
 * @brief calling SOCKETS_Socket with various invalid arguments
 *        expects SOCKETS_INVALID_SOCKET
 */
void test05_SecureSockets_socket_invalid_arguments(void)
{
    Socket_t so = SOCKETS_Socket(SOCKETS_AF_INET ,
                                 SOCKETS_SOCK_DGRAM,
                                 SOCKETS_IPPROTO_TCP);
    TEST_ASSERT_EQUAL_INT(SOCKETS_INVALID_SOCKET, so);

    so = SOCKETS_Socket(SOCKETS_AF_INET ,
                                 SOCKETS_SOCK_STREAM,
                                 SOCKETS_IPPROTO_UDP);
    TEST_ASSERT_EQUAL_INT(SOCKETS_INVALID_SOCKET, so);

    so = SOCKETS_Socket(SOCKETS_AF_INET6,
                                 SOCKETS_SOCK_DGRAM,
                                 SOCKETS_IPPROTO_TCP);
    TEST_ASSERT_EQUAL_INT(SOCKETS_INVALID_SOCKET, so);
}
/*!
 * @brief out of memory test
 *        memory allocation returns NULL
 */
void test06_SecureSockets_socket_no_memory(void)
{
    uninitCallbacks();
    pvPortMalloc_ExpectAnyArgsAndReturn(NULL);

    Socket_t so = SOCKETS_Socket(SOCKETS_AF_INET ,
                                 SOCKETS_SOCK_STREAM,
                                 SOCKETS_IPPROTO_TCP);
    TEST_ASSERT_EQUAL_INT(SOCKETS_INVALID_SOCKET, so);
}

/*!
 * @brief  ip_sockets is >= 0
 */
void test07_SecureSockets_socket_ip_socket_is_ge_zero(void)
{
    lwip_socket_ExpectAnyArgsAndReturn(0);

    Socket_t so = SOCKETS_Socket(SOCKETS_AF_INET ,
                                 SOCKETS_SOCK_STREAM,
                                 SOCKETS_IPPROTO_TCP);

    TEST_ASSERT_NOT_EQUAL(SOCKETS_INVALID_SOCKET, so);
    deinitSocket(so);
}

/*!
 * @brief  ip_sockets is < 0
 */
void test08_SecureSockets_socket_ip_socket_is_ge_zero(void)
{
    lwip_socket_ExpectAnyArgsAndReturn(-1);

    Socket_t so = SOCKETS_Socket(SOCKETS_AF_INET ,
                                 SOCKETS_SOCK_STREAM,
                                 SOCKETS_IPPROTO_TCP);

    TEST_ASSERT_EQUAL(SOCKETS_INVALID_SOCKET, so);
}

/*!
 * @brief allocate sockets grater than
 *        socketsconfigDEFAULT_MAX_NUM_SECURE_SOCKETS
 *        expect the last call to fail
*/
void test09_SecureSockets_socket_max_sockets(void)
{
    int i;
    int max_sockets = socketsconfigDEFAULT_MAX_NUM_SECURE_SOCKETS;
    Socket_t socket_array[socketsconfigDEFAULT_MAX_NUM_SECURE_SOCKETS];

    for (i = max_sockets -1 ; i >= 0; --i) {
        socket_array[i] = initSocket();
        TEST_ASSERT_NOT_EQUAL(SOCKETS_INVALID_SOCKET, socket_array[i]);
    }
    /* the following call should fail */
    Socket_t so = SOCKETS_Socket(SOCKETS_AF_INET ,
                                 SOCKETS_SOCK_STREAM ,
                                 SOCKETS_IPPROTO_TCP);
    TEST_ASSERT_EQUAL_INT(SOCKETS_INVALID_SOCKET, so);

    /* cleanup */
    for (i = max_sockets-1 ; i >= 0; --i) {
        deinitSocket(socket_array[i]);
    }
}

/* ======================  TESTING SOCKETS_Recv  ============================ */
/*!
 * @brief A happy path for normal socket receive
*/
void test10_SecureSockets_Recv_successful(void)
{
    int32_t ret;
    char buffer[BUFFER_LEN];
    Socket_t so = create_normal_connection();

    lwip_recv_IgnoreAndReturn(BUFFER_LEN);
    /* api call in test */
    ret = SOCKETS_Recv(so, buffer, BUFFER_LEN, 0);

    TEST_ASSERT_EQUAL_INT(BUFFER_LEN, ret);
    deinitSocket(so);
}

/*!
 * @brief A happy path for TLS socket receive
*/
void test11_SecureSockets_Recv_TLS_successful(void)
{
    int32_t ret;
    char buffer[BUFFER_LEN];
    Socket_t so = create_TLS_connection();

    TLS_Recv_IgnoreAndReturn(BUFFER_LEN);
    /* api call in test */
    ret = SOCKETS_Recv(so, buffer, BUFFER_LEN, 0);

    TEST_ASSERT_EQUAL_INT(BUFFER_LEN, ret);
    TLS_Cleanup_Ignore();
    deinitSocket(so);
}

/*!
 * @brief Receive while socket not connected
*/
void test12_SecureSockets_Recv_NotConnected(void)
{
    int32_t ret;
    char buffer[BUFFER_LEN];
    Socket_t so = initSocket();
    ret = SOCKETS_Recv(so, buffer, BUFFER_LEN, 0);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ENOTCONN, ret);
    deinitSocket(so);
}
/*!
 * @brief Receive invalid socket
*/
void test13_SecureSockets_Recv_InvalidParameters(void)
{
    int32_t ret;
    char buffer[BUFFER_LEN];
    Socket_t so = SOCKETS_INVALID_SOCKET;
    ret = SOCKETS_Recv(so, buffer, BUFFER_LEN, 0);
    TEST_ASSERT_EQUAL_INT(SOCKETS_SOCKET_ERROR, ret);

    so = initSocket();
    ret = SOCKETS_Recv(so, NULL, BUFFER_LEN, 0);
    TEST_ASSERT_EQUAL_INT(SOCKETS_EINVAL, ret);

    ret = SOCKETS_Recv(so, buffer, 0, 0);
    TEST_ASSERT_EQUAL_INT(SOCKETS_EINVAL, ret);

    deinitSocket(so);

}
/* =======================  TESTING  SOCKETS_Shutown  ======================= */
/*!
 * @brief Test a happy case
*/
void test14_SecureSockets_Shutdown_successful(void)
{
    int32_t ret;
    Socket_t so = create_normal_connection();

    lwip_shutdown_ExpectAnyArgsAndReturn(1);
    ret = SOCKETS_Shutdown(so, 0);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ERROR_NONE, ret);
    deinitSocket(so);
}

/*!
 * @brief Test bad arguments
*/
void test15_SecureSockets_Shutdown_bad_arguments(void)
{
    int32_t ret;
    Socket_t so = SOCKETS_INVALID_SOCKET;
    ret = SOCKETS_Shutdown(so, 1);
    TEST_ASSERT_EQUAL_INT(SOCKETS_EINVAL, ret);
}

/*!
 * @brief lwip_shutdown error
*/
void test16_SecureSockets_Shutdown_lwip_error(void)
{
    int32_t ret;
    Socket_t so = create_normal_connection();

    lwip_shutdown_ExpectAnyArgsAndReturn(-1);
    ret = SOCKETS_Shutdown(so, 0);
    TEST_ASSERT_EQUAL_INT(SOCKETS_SOCKET_ERROR, ret);
    deinitSocket(so);
}

/* ========================  TESTING  SOCKETS_Close  -======================= */
/*!
 * @brief Close happy case
*/
void test17_SecureSockets_Close_successful(void)
{
    int32_t ret;
    Socket_t so = initSocket();
    setALPN_protocols(so, 1);
    vTaskDelay_Ignore();
    lwip_close_IgnoreAndReturn(0);
    ret = SOCKETS_Close(so);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ERROR_NONE, ret);
}

/*!
 * @brief Close successful ALPN protocol
*/
void test18_SecureSockets_Close_successful2(void)
{
    int32_t ret;
    Socket_t so = initSocket();
    setALPN_protocols(so, 2);
    vTaskDelay_Ignore();
    lwip_close_IgnoreAndReturn(0);
    ret = SOCKETS_Close(so);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ERROR_NONE, ret);
}

/*!
 * @brief Close successful ALPN protocol with TLS
*/
void test19_SecureSockets_Close_successful3(void)
{
    int32_t ret;
    Socket_t so = initSocket();
    setALPN_protocols(so, 2);
    setTLSMode(so);
    vTaskDelay_Ignore();
    lwip_close_IgnoreAndReturn(0);
    TLS_Cleanup_Ignore();
    ret = SOCKETS_Close(so);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ERROR_NONE, ret);
}

/*!
 * @brief Close successful with Server Certificate
*/
void test20_SecureSockets_Close_successful4(void)
{
    int32_t ret;
    Socket_t so = initSocket();
    setServerCert(so);
    vTaskDelay_Ignore();
    lwip_close_IgnoreAndReturn(0);
    ret = SOCKETS_Close(so);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ERROR_NONE, ret);
}

/*!
 * @brief Close successful with Server Name indication
*/
void test21_SecureSockets_Close_successful5(void)
{
    int32_t ret;
    Socket_t so = initSocket();
    setServerNameIndication(so);
    vTaskDelay_Ignore();
    lwip_close_IgnoreAndReturn(0);
    ret = SOCKETS_Close(so);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ERROR_NONE, ret);
}

/*!
 * @brief Close invalid parameters
*/
void test22_SecureSockets_Close_InvalidSocket(void)
{
    int32_t ret;
    Socket_t so = SOCKETS_INVALID_SOCKET;
    ret = SOCKETS_Close(so);
    TEST_ASSERT_EQUAL_INT(SOCKETS_EINVAL, ret);
}

/* =======================  TESTING  SOCKETS_Connect  ======================= */
/*!
 * @brief Connect  with various invalid parameters
*/
void test23_SecureSockets_Connect_InvalidParameters(void)
{
    int32_t ret;
    Socket_t so = SOCKETS_INVALID_SOCKET;
    SocketsSockaddr_t pxAddress;

    pxAddress.ucLength = 32;
    pxAddress.ucSocketDomain = SOCKETS_AF_INET;
    pxAddress.usPort = 1000;
    pxAddress.ulAddress = 1234567;

    ret = SOCKETS_Connect(so, &pxAddress, sizeof(pxAddress));
    TEST_ASSERT_EQUAL(SOCKETS_EINVAL, ret);

    so = initSocket();
    ret = SOCKETS_Connect(so, NULL, sizeof(pxAddress));
    TEST_ASSERT_EQUAL_INT(SOCKETS_EINVAL, ret);
    deinitSocket(so);
}

/*!
 * @brief Connect  with lwip_connect error
*/
void test24_SecureSockets_Connect_lwip_connect_error(void)
{
    int32_t ret;
    Socket_t so = SOCKETS_INVALID_SOCKET;
    SocketsSockaddr_t pxAddress;

    pxAddress.ucLength = 32;
    pxAddress.ucSocketDomain = SOCKETS_AF_INET;
    pxAddress.usPort = 1000;
    pxAddress.ulAddress = 1234567;

    so = initSocket();
    lwip_connect_IgnoreAndReturn(-1);
    vLoggingPrintf_Ignore();
    ret = SOCKETS_Connect(so, &pxAddress, sizeof(pxAddress));
    TEST_ASSERT_EQUAL_INT(SOCKETS_SOCKET_ERROR, ret);
    deinitSocket(so);
}

/*!
 * @brief Connect  with TLS_Init error
*/
void test25_SecureSockets_Connect_TLS_Init_error(void)
{
    int32_t ret;
    Socket_t so = SOCKETS_INVALID_SOCKET;
    SocketsSockaddr_t pxAddress;

    pxAddress.ucLength = 32;
    pxAddress.ucSocketDomain = SOCKETS_AF_INET;
    pxAddress.usPort = 1000;
    pxAddress.ulAddress = 1234567;

    so = initSocket();
    setTLSMode(so);
    lwip_connect_IgnoreAndReturn(0);
    vLoggingPrintf_Ignore();
    TLS_Init_IgnoreAndReturn(pdFREERTOS_ERRNO_ENOENT);
    ret = SOCKETS_Connect(so, &pxAddress, sizeof(pxAddress));
    TEST_ASSERT_EQUAL_INT(SOCKETS_SOCKET_ERROR, ret);
    TLS_Cleanup_ExpectAnyArgs();
    deinitSocket(so);
}

/*!
 * @brief Connect  with TLS_Connect error
*/
void test26_SecureSockets_Connect_TLS_connect_error(void)
{
    int32_t ret;
    Socket_t so = SOCKETS_INVALID_SOCKET;
    SocketsSockaddr_t pxAddress;

    pxAddress.ucLength = 32;
    pxAddress.ucSocketDomain = SOCKETS_AF_INET;
    pxAddress.usPort = 1000;
    pxAddress.ulAddress = 1234567;

    so = initSocket();
    setTLSMode(so);
    lwip_connect_IgnoreAndReturn(0);
    vLoggingPrintf_Ignore();
    TLS_Init_IgnoreAndReturn(pdFREERTOS_ERRNO_NONE);
    TLS_Connect_IgnoreAndReturn(pdFREERTOS_ERRNO_ENOENT);
    ret = SOCKETS_Connect(so, &pxAddress, sizeof(pxAddress));
    TEST_ASSERT_EQUAL_INT(SOCKETS_SOCKET_ERROR, ret);
    TLS_Cleanup_ExpectAnyArgs();
    deinitSocket(so);
}

/* ====================  TESTING  SOCKETS_GetHostByName  ==================== */
/*!
 * @brief GetHostByName  successful case
*/
void test27_SecureSockets_GetHostByName_suceessful(void)
{
    int32_t ret;
    uint8_t addr = 0;
    uint8_t ret_addr = 5;
    int32_t hostnameMaxLen = securesocketsMAX_DNS_NAME_LENGTH;
    char hostname[hostnameMaxLen];

    strncpy(hostname, "this is a hostname", hostnameMaxLen);

    WIFI_GetHostIP_ExpectAndReturn(hostname, &addr, 0);
    WIFI_GetHostIP_ReturnThruPtr_pucIPAddr(&ret_addr);
    ret = SOCKETS_GetHostByName(hostname);
    TEST_ASSERT_EQUAL_INT(ret_addr, ret);
}

/*!
 * @brief GetHostByName   hostname too large
*/
void test28_SecureSockets_GetHostByName_longHostname(void)
{
    int32_t ret;
    uint8_t addr = 0;
    int32_t hostnameMaxLen = securesocketsMAX_DNS_NAME_LENGTH + 5;
    char hostname[hostnameMaxLen];

    memset(hostname, 'a', hostnameMaxLen);
    hostname[hostnameMaxLen - 1] = '\0';

    vLoggingPrintf_Ignore();
    ret = SOCKETS_GetHostByName(hostname);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

/* ========================  TESTING   SOCKETS_Init  ======================== */
/*!
 * @brief Init  successful case
*/
void test29_SecureSockets_Init(void)
{
    BaseType_t ret;
    ret = SOCKETS_Init();
    TEST_ASSERT_EQUAL(ret, pdPASS);
}

/* =====================  TESTING   SOCKETS_SetSockOpt  ===================== */
/*!
 * @brief SetSockOp bad arguments
*/
void test30_SecureSockets_SetSockOpt_bad_arguments(void)
{
    Socket_t so = SOCKETS_INVALID_SOCKET;
    int32_t ret;

    ret = SOCKETS_SetSockOpt(so,  0, SOCKETS_SO_RCVTIMEO, NULL, 0);
    TEST_ASSERT_EQUAL(SOCKETS_EINVAL, ret);
}

/*!
 * @brief SetSockOp successful case for
 *                   SOCKETS_SO_RCVTIMEO:
 *                   SOCKETS_SO_SNDTIMEO:
*/
void test31_SecureSockets_SetSockOpt_So_RCV_SND_Timeout(void)
{
    Socket_t so = SOCKETS_INVALID_SOCKET;
    int32_t ret;
    TickType_t ticks = 3;

    so = initSocket();

    lwip_setsockopt_ExpectAndReturn(1, SOL_SOCKET, SO_RCVTIMEO, 0,
                                    sizeof(struct timeval), 0);
    lwip_setsockopt_IgnoreArg_s();
    lwip_setsockopt_IgnoreArg_optval();
    ret = SOCKETS_SetSockOpt(so, 0, SOCKETS_SO_RCVTIMEO, &ticks, 0);
    TEST_ASSERT_EQUAL(SOCKETS_ERROR_NONE, ret);

    /*  test setting send timeout */
    lwip_setsockopt_ExpectAndReturn(1, SOL_SOCKET, SO_SNDTIMEO, 0,
                                    sizeof(struct timeval), 0);
    lwip_setsockopt_IgnoreArg_s();
    lwip_setsockopt_IgnoreArg_optval();
    ret = SOCKETS_SetSockOpt(so, 0, SOCKETS_SO_SNDTIMEO, &ticks, 0);
    TEST_ASSERT_EQUAL(SOCKETS_ERROR_NONE, ret);

    deinitSocket(so);
}

/*!
 * @brief SetSockOp lwip_setsockopt error
*/
void test32SecureSockets_SetSockOpt_So_RCV_SND_Timeout_error(void)
{
    Socket_t so = SOCKETS_INVALID_SOCKET;
    int32_t ret;
    TickType_t ticks = 3;

    so = initSocket();

    lwip_setsockopt_ExpectAndReturn(1, SOL_SOCKET, SO_RCVTIMEO, 0,
                                    sizeof(struct timeval), -1);
    lwip_setsockopt_IgnoreArg_s();
    lwip_setsockopt_IgnoreArg_optval();
    ret = SOCKETS_SetSockOpt(so, 0, SOCKETS_SO_RCVTIMEO, &ticks, 0);
    TEST_ASSERT_EQUAL(SOCKETS_EINVAL, ret);

    deinitSocket(so);
}
/*!
 * @brief SetSockOp invalid option
*/
void test33_SecureSockets_SetSockOpt_Invalid_Option(void)
{
    Socket_t so = SOCKETS_INVALID_SOCKET;
    int32_t ret;
    TickType_t ticks = 3;

    so = initSocket();

    ret = SOCKETS_SetSockOpt(so, 0, -1, &ticks, 0);
    TEST_ASSERT_EQUAL(SOCKETS_ENOPROTOOPT, ret);

    deinitSocket(so);
}

/*!
 * @brief SetSockOp so nonblock_success succesful case
 */
void test34_SecureSockets_SetsockOpt_nonblock_success(void)
{
    Socket_t so = SOCKETS_INVALID_SOCKET;
    int32_t ret;
    int opt = 1;

    so = create_normal_connection();

    lwip_ioctl_ExpectAndReturn(0, FIONBIO,  &opt, 0);
    lwip_ioctl_IgnoreArg_s();
    ret = SOCKETS_SetSockOpt(so, 0, SOCKETS_SO_NONBLOCK, NULL, 0);
    TEST_ASSERT_EQUAL(SOCKETS_ERROR_NONE, ret);

    deinitSocket(so);
}
/*!
 * @brief 
 */
void test35_SecureSockets_SetsockOpt_nonblock_Error(void)
{
    Socket_t so = SOCKETS_INVALID_SOCKET;
    int32_t ret;
    int opt = 1;

    so = initSocket();

    ret = SOCKETS_SetSockOpt(so, 0, SOCKETS_SO_NONBLOCK, NULL, 0);
    TEST_ASSERT_EQUAL(SOCKETS_ENOTCONN, ret);

    connectSocket(so);
    lwip_ioctl_ExpectAndReturn(0, FIONBIO,  &opt, -1);
    lwip_ioctl_IgnoreArg_s();
    ret = SOCKETS_SetSockOpt(so, 0, SOCKETS_SO_NONBLOCK, NULL, 0);
    TEST_ASSERT_EQUAL(SOCKETS_EINVAL, ret);

    deinitSocket(so);
}
static TaskHandle_t handle;
static Socket_t s_so;
static bool userCallback_called = 0;
static void taskComplete_cb(Socket_t ctx)
{
    userCallback_called = true;
}
static int lwip_select_cb(int s, fd_set *read_set,
                     fd_set *write_set, fd_set *err_set, struct timeval *tv,
                     int call_num)
{
    if (call_num == 0) {
        return 0;
    } else  {
        deinitSocket(s_so);
    }
}
static long int xTaskCreate_cb2(TaskFunction_t pxTaskCode,
                            const char * const pcName,
                            const configSTACK_DEPTH_TYPE usStackDepth,
                            void * const pvParameters,
                            UBaseType_t uxPriority,
                            TaskHandle_t * const pxCreatedTask,
                            int num_of_calls)
{
    handle = malloc(sizeof (TaskHandle_t));
    *pxCreatedTask = handle;

    lwip_select_Stub(lwip_select_cb);
    vTaskDelete_Ignore();
    pxTaskCode(pvParameters); /* pvParameters the socket_t/ss_ctx */
    return pdPASS;
}
/*!
 * @brief 
 */
void test36_SecureSockets_SetSockOpt_wakeup_callback_socket_close(void)
{
    s_so = SOCKETS_INVALID_SOCKET;
    int32_t ret;
    void* option = &taskComplete_cb; /* user callback for socket event */

    xTaskCreate_StubWithCallback(xTaskCreate_cb2);
    s_so = initSocket();

    ret = SOCKETS_SetSockOpt(s_so, 0, SOCKETS_SO_WAKEUP_CALLBACK,
                             option, sizeof(void*));
    TEST_ASSERT_EQUAL(SOCKETS_ERROR_NONE, ret);
    TEST_ASSERT_TRUE(userCallback_called);
    userCallback_called = false;
    free(handle);
}

/*!
 * @brief 
 */
static long int xTaskCreate_cb(TaskFunction_t pxTaskCode,
                            const char * const pcName,
                            const configSTACK_DEPTH_TYPE usStackDepth,
                            void * const pvParameters,
                            UBaseType_t uxPriority,
                            TaskHandle_t * const pxCreatedTask,
                            int num_of_calls)
{
    handle = malloc(sizeof (TaskHandle_t));
    *pxCreatedTask = handle;

    lwip_select_IgnoreAndReturn(0);
    lwip_select_IgnoreAndReturn(-1);
    lwip_select_IgnoreAndReturn(0);
    vTaskDelete_Ignore();
    pxTaskCode(pvParameters); /* pvParameters the socket_t/ss_ctx */
    return pdPASS;
}
void test37_SecureSockets_SetSockOpt_wakeup_callback(void)
{
    Socket_t so = SOCKETS_INVALID_SOCKET;
    int32_t ret;
    void* option = &taskComplete_cb; /* user callback for socket event */

    xTaskCreate_StubWithCallback(xTaskCreate_cb);
    so = initSocket();

    ret = SOCKETS_SetSockOpt(so, 0, SOCKETS_SO_WAKEUP_CALLBACK,
                             option, sizeof(void*));
    TEST_ASSERT_EQUAL(SOCKETS_ERROR_NONE, ret);

    deinitSocket(so);
    TEST_ASSERT_TRUE(userCallback_called);
    userCallback_called = false;
    free(handle);
}
/*!
 * @brief SetSockOpt SOCKETS_SO_WAKEUP_CALLBACK currently the clean
 * functionality is not implemented, but once implemented this test case will
 * probably fails until it is updated to test the new code
 */
void test38_SecureSockets_SetSockOpt_wakeup_callback_clear(void)
{
    Socket_t so = SOCKETS_INVALID_SOCKET;
    int32_t ret;
    void* option = &taskComplete_cb;

    so = initSocket();

    ret = SOCKETS_SetSockOpt(so, 0, SOCKETS_SO_WAKEUP_CALLBACK,
                             NULL, 0);
    TEST_ASSERT_EQUAL(SOCKETS_ERROR_NONE, ret);

    deinitSocket(so);
}

/* Helper function for alpn memory
 * since alpn is trying to allocate a 2d array
 * we simulate that the first memory allocation passes but the second one fails
 */
static void* malloc_cb2(size_t size, int numCalls)
{
    if (numCalls == 1) {
        malloc_free_calls++;
        return malloc(size);
    }
    return NULL;
}

/*!
 * @brief  SetSockOpt alpn no memory tests
 */
void test39_SecureSockets_SetSockOpt_alpn_no_mem(void)
{
    char * pcAlpns[] = { socketsAWS_IOT_ALPN_MQTT , socketsAWS_IOT_ALPN_MQTT};
    Socket_t so = SOCKETS_INVALID_SOCKET;
    int32_t ret;
    void* option = &taskComplete_cb;

    so = initSocket();

    pvPortMalloc_Stub(NULL);
    pvPortMalloc_ExpectAnyArgsAndReturn(NULL);
    ret = SOCKETS_SetSockOpt(so,
                             0, /* Level - Unused. */
                             SOCKETS_SO_ALPN_PROTOCOLS,
                             pcAlpns,
                             1);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ENOMEM, ret);

    pvPortMalloc_Stub(malloc_cb2);
    ret = SOCKETS_SetSockOpt(so,
                             0, /* Level - Unused. */
                             SOCKETS_SO_ALPN_PROTOCOLS,
                             pcAlpns,
                             2);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ENOMEM, ret);

    deinitSocket(so);
}

/*!
 * @brief 
 */
void test40_SecureSockets_SetSockOpt_alpn_connected(void)
{

    char * pcAlpns[] = { socketsAWS_IOT_ALPN_MQTT , socketsAWS_IOT_ALPN_MQTT};
    Socket_t so = SOCKETS_INVALID_SOCKET;
    int32_t ret;

    so = create_normal_connection();
    ret = SOCKETS_SetSockOpt(so,
                             0, /* Level - Unused. */
                             SOCKETS_SO_ALPN_PROTOCOLS,
                             pcAlpns,
                             1);
    TEST_ASSERT_EQUAL_INT(SOCKETS_EISCONN, ret);

    deinitSocket(so);
}

/*!
 * @brief  SetSockOpt SOCKETS_SO_SERVER_NAME_INDICATION
 *         various error conditions
 */
void test41_SecureSockets_SetSockOpt_setname_indication_error(void)
{
    int32_t ret;
    Socket_t so = initSocket();

    ret = SOCKETS_SetSockOpt(so,
                             0,
                             SOCKETS_SO_SERVER_NAME_INDICATION,
                             NULL,
                             11);
    TEST_ASSERT_EQUAL_INT(SOCKETS_EINVAL, ret);
    ret = SOCKETS_SetSockOpt(so,
                             0,
                             SOCKETS_SO_SERVER_NAME_INDICATION,
                             "server name indication",
                             0);

    TEST_ASSERT_EQUAL_INT(SOCKETS_EINVAL, ret);

    connectSocket(so);

    ret = SOCKETS_SetSockOpt(so,
                            0,
                            SOCKETS_SO_SERVER_NAME_INDICATION,
                            "server name indication",
                            22);
    TEST_ASSERT_EQUAL_INT(SOCKETS_EISCONN, ret);

    deinitSocket(so);
}

/*!
 * @brief 
 */
void test42_SecureSockets_SetSockOpt_setname_indication_memory_errors(void)
{
    int32_t ret;
    Socket_t so = initSocket();
    setServerNameIndication(so);

    pvPortMalloc_Stub(NULL);
    pvPortMalloc_ExpectAndReturn(22 + 1, NULL);
    ret = SOCKETS_SetSockOpt(so,
                            0,
                            SOCKETS_SO_SERVER_NAME_INDICATION,
                            "server name indication",
                            22);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ENOMEM, ret);

    pvPortMalloc_Stub(malloc_cb);
    ret = SOCKETS_SetSockOpt(so,
                            0,
                            SOCKETS_SO_SERVER_NAME_INDICATION,
                            "server name indication",
                            22);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ERROR_NONE, ret);
    deinitSocket(so);
    
}

/*!
 * @brief  SetSockOpt set TLS to an already connected socket
 */
void test43_SecureSockets_SetSockOpt_require_tls_already_conected_error(void)
{
    int32_t ret;
    Socket_t so = create_normal_connection();

    ret = SOCKETS_SetSockOpt(so, 0, SOCKETS_SO_REQUIRE_TLS, NULL ,0);
    TEST_ASSERT_EQUAL_INT(SOCKETS_EISCONN, ret);

    deinitSocket(so);
}

/*!
 * @brief SetSockOpt set trusted server certificate memory errors
 */
void test44_SecureSocketsSetSockOpt_set_server_cert_memory_errors(void)
{
    int32_t ret;
    Socket_t so = initSocket();
    setServerNameIndication(so);

    pvPortMalloc_Stub(NULL);
    pvPortMalloc_ExpectAndReturn(18 + 1, NULL);
    ret = SOCKETS_SetSockOpt(so,
                            0,
                            SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE,
                            "server certificate",
                            18);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ENOMEM, ret);

    pvPortMalloc_Stub(malloc_cb);
    ret = SOCKETS_SetSockOpt(so,
                            0,
                            SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE,
                            "server certificate",
                            18);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ERROR_NONE, ret);

    ret = SOCKETS_SetSockOpt(so,
                            0,
                            SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE,
                            "server certificate",
                            18);
    TEST_ASSERT_EQUAL_INT(SOCKETS_ERROR_NONE, ret);
    deinitSocket(so);
}

/*!
 * @brief  SetSockOpt set TLS to an already connected socket
 */
void test45_SecureSockets_SetSockOpt_trusted_server_already_conected_error(void)
{
    int32_t ret;
    Socket_t so = create_normal_connection();

    ret = SOCKETS_SetSockOpt(so, 0, SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE,
                             "cert", 4);
    TEST_ASSERT_EQUAL_INT(SOCKETS_EISCONN, ret);

    deinitSocket(so);
}

/*!
 * @brief SetSockOpt set trusted server certificate 
 */
void test46_SecureSockets_SetSockOpt_trusted_server_invalid_arguments()
{
    int32_t ret;
    Socket_t so = initSocket();

    ret = SOCKETS_SetSockOpt(so, 0,
                    SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE, NULL ,4);
    TEST_ASSERT_EQUAL_INT(SOCKETS_EINVAL, ret);

    ret = SOCKETS_SetSockOpt(so, 0,
                    SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE, "server name" ,0);
    TEST_ASSERT_EQUAL_INT(SOCKETS_EINVAL, ret);

    deinitSocket(so);
}
