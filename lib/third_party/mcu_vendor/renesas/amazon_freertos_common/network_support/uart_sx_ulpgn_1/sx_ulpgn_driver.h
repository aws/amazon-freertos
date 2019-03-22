#ifndef SX_ULPGN_DRIVER_H
#define SX_ULPGN_DRIVER_H

#define DEBUGLOG  0

#define ULPGN_RETURN_TEXT_OK             "OK\r\n"
#define ULPGN_RETURN_TEXT_ERROR          "ERROR\r\n"

#define ULPGN_RETURN_TEXT_OK          "OK\r\n"
#define ULPGN_RETURN_TEXT_CONNECT     "CONNECT\r\n"
#define ULPGN_RETURN_TEXT_RING        "RING\r\n"
#define ULPGN_RETURN_TEXT_NO_CARRIER  "NO_CARRIER\r\n"
#define ULPGN_RETURN_TEXT_ERROR       "ERROR\r\n"
#define ULPGN_RETURN_TEXT_NO_DIALTONE "NO_DIALTONE\r\n"
#define ULPGN_RETURN_TEXT_BUSY        "BUSY\r\n"
#define ULPGN_RETURN_TEXT_NO_ANSWER   "NO_ANSWER\r\n"

#define ULPGN_RETURN_NUMERIC_OK          "0\r"
#define ULPGN_RETURN_NUMERIC_CONNECT     "1\r"
#define ULPGN_RETURN_NUMERIC_RING        "2\r"
#define ULPGN_RETURN_NUMERIC_NO_CARRIER  "3\r"
#define ULPGN_RETURN_NUMERIC_ERROR       "4\r"
#define ULPGN_RETURN_NUMERIC_NO_DIALTONE "6\r"
#define ULPGN_RETURN_NUMERIC_BUSY        "7\r"
#define ULPGN_RETURN_NUMERIC_NO_ANSWER   "8\r"

typedef enum
{
    ULPGN_RETURN_OK            = 0,
    ULPGN_RETURN_CONNECT,
    ULPGN_RETURN_RING,
    ULPGN_RETURN_NO_CARRIER,
    ULPGN_RETURN_ERROR,
    ULPGN_RETURN_DUMMY,
    ULPGN_RETURN_NO_DIALTONE,
    ULPGN_RETURN_BUSY,
    ULPGN_RETURN_NO_ANSWER,
    ULPGN_RETURN_ENUM_MAX,
} sx_ulpgn_return_code_t;

typedef enum
{
    ULPGN_RETURN_STRING_TEXT            = 0,
    ULPGN_RETURN_STRING_NUMERIC,
    ULPGN_RETURN_STRING_MAX,
} sx_ulpgn_return_string_t;

typedef enum
{
    ULPGN_SECURITY_OPEN            = 0,
    ULPGN_SECURITY_WPA,
    ULPGN_SECURITY_WPA2,
    ULPGN_SECURITY_WEP,
    ULPGN_SECURITY_UNDEFINED,
    ULPGN_SECURITY_MAX,
} sx_ulpgn_security_t;

int32_t sx_ulpgn_init(void);
int32_t sx_ulpgn_wifi_get_macaddr(uint8_t *ptextstring);
int32_t sx_ulpgn_wifi_connect(const char *pssid, uint32_t security, const char *ppass);
int32_t sx_ulpgn_socket_create(uint32_t type, uint32_t ipversion);
int32_t sx_ulpgn_tcp_connect(uint32_t ipaddr, uint16_t port);
int32_t sx_ulpgn_tcp_send(const uint8_t *pdata, int32_t length, uint32_t timeout);
int32_t sx_ulpgn_tcp_recv(uint8_t *pdata, int32_t length, uint32_t timeout);
int32_t sx_ulpgn_serial_tcp_timeout_set(TickType_t timeout_ms);
int32_t sx_ulpgn_tcp_disconnect(void);
int32_t sx_ulpgn_dns_query(const char *ptextstring, uint32_t *ulipaddr);

#endif /* #define SX_ULPGN_DRIVER_H */
