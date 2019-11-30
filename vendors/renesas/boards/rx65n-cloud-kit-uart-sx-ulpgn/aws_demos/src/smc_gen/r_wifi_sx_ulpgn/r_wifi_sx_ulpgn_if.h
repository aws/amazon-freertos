#ifndef R_WIFI_SX_ULPGN_IF_H
#define R_WIFI_SX_ULPGN_IF_H

#include "r_sci_rx_if.h"
#include "r_byteq_if.h"
#include "r_wifi_sx_ulpgn_config.h"

#include "platform.h"

/* Driver Version Number. */
#define WIFI_SX_ULPGN_VERSION_MAJOR           (1)
#define WIFI_SX_ULPGN_VERSION_MINOR           (00)

#define WIFI_SOCKET_IP_PROTOCOL_TCP (6)
#define WIFI_SOCKET_IP_VERSION_4    (4)

#define WIFI_IPV4BYTE_TO_ULONG(in_addr1, in_addr2, in_addr3, in_addr4) \
	(((uint32_t)in_addr1 & 0x000000FF) << 24) | (((uint32_t)in_addr2 & 0x000000FF) << 16) |\
    (((uint32_t)in_addr3 & 0x000000FF) << 8)  | (((uint32_t)in_addr4 & 0x000000FF))

#define WIFI_ULONG_TO_IPV4BYTE_1(in_ul) (uint8_t)(((uint32_t)in_ul) >> 24)
#define WIFI_ULONG_TO_IPV4BYTE_2(in_ul) (uint8_t)(((uint32_t)in_ul) >> 16)
#define WIFI_ULONG_TO_IPV4BYTE_3(in_ul) (uint8_t)(((uint32_t)in_ul) >> 8)
#define WIFI_ULONG_TO_IPV4BYTE_4(in_ul) (uint8_t)((uint32_t)in_ul)

typedef enum
{
	WIFI_SECURITY_OPEN            = 0,
	WIFI_SECURITY_WEP,
	WIFI_SECURITY_WPA,
	WIFI_SECURITY_WPA2,
	WIFI_SECURITY_UNDEFINED,
}wifi_security_t;

typedef enum 			// Wi-Fi APIエラーコード
{
	WIFI_SUCCESS            = 0,	// 成功
	WIFI_ERR_PARAMETER	    = -1,	// 引数が無効です。
	WIFI_ERR_ALREADY_OPEN   = -2,	// すでに初期化済みです
	WIFI_ERR_NOT_OPEN       = -3,	// 初期化していません
	WIFI_ERR_SERIAL_OPEN    = -4,	// シリアルの初期化ができません。
	WIFI_ERR_MODULE_COM     = -5,	// WiFiモジュールとの通信に失敗しました。
	WIFI_ERR_NOT_CONNECT    = -6,	// アクセスポイントに接続していません。
	WIFI_ERR_SOCKET_NUM     = -7,	// 利用可能なソケットがありません。
	WIFI_ERR_SOCKET_CREATE  = -8,	// ソケットを作成できません。
	WIFI_ERR_CHANGE_SOCKET  = -9,	// ソケットを切り替えられません。
	WIFI_ERR_SOCKET_CONNECT = -10,	// ソケットに接続できません。
	WIFI_ERR_BYTEQ_OPEN     = -11,	// BYTEQの割り当てに失敗しました。
	WIFI_ERR_SOCKET_TIMEOUT = -12,	// ソケットの送信でタイムアウトしました。
	WIFI_ERR_TAKE_MUTEX     = -13,	// Mutexの取得に失敗しました。
} wifi_err_t;

typedef enum
{
	WIFI_EVENT_WIFI_REBOOT = 0,
	WIFI_EVENT_WIFI_DISCONNECT,
	WIFI_EVENT_SERIAL_OVF_ERR,
	WIFI_EVENT_SERIAL_FLM_ERR,
	WIFI_EVENT_SERIAL_RXQ_OVF_ERR,
	WIFI_EVENT_RCV_TASK_RXB_OVF_ERR,
	WIFI_EVENT_SOCKET_CLOSED,
	WIFI_EVENT_SOCKET_RXQ_OVF_ERR,
} wifi_err_event_enum_t;

typedef struct
{
		uint8_t ssid[ 33 ];			//SSID�̊i�[�̈�
		uint8_t bssid[ 6 ];			//BSSID�̊i�[�̈�
		wifi_security_t security;	//�Z�L�����e�B�^�C�v�̊i�[�̈�
		int8_t rssi;		     	//�M�����x�̊i�[�̈�
		int8_t channel;				//�`�����l���ԍ��̊i�[�̈�
		uint8_t hidden;				//Hidden�`�����l���̊i�[�̈�
} wifi_scan_result_t;

typedef struct
{
		uint32_t ipaddress;	//IP�A�h���X
		uint32_t subnetmask;	//�T�u�l�b�g�}�X�N
		uint32_t gateway;	//�Q�[�g�E�F�C
} wifi_ip_configuration_t;

typedef struct
{
	wifi_err_event_enum_t event;
	uint32_t socket_no;
}wifi_err_event_t;

typedef enum
{
    ULPGN_SOCKET_STATUS_CLOSED            = 0,
    ULPGN_SOCKET_STATUS_SOCKET,
    ULPGN_SOCKET_STATUS_BOUND,
    ULPGN_SOCKET_STATUS_LISTEN,
    ULPGN_SOCKET_STATUS_CONNECTED,
    ULPGN_SOCKET_STATUS_MAX,
} sx_ulpgn_socket_status_t;

/* Configuration */
//#define ULPGN_USE_UART_NUM (1)
#define ULPGN_CFG_SOCKET_CHANGE_TIMEOUT_VALUE    100
#define ULPGN_CFG_SOCKET_CHANGE_TIMEOUT_PERIOD   BSP_DELAY_MILLISECS


#define ULPGN_RETURN_TEXT_OK          "OK\r\n"
#define ULPGN_RETURN_TEXT_CONNECT     "CONNECT\r\n"
#define ULPGN_RETURN_TEXT_RING        "RING\r\n"
#define ULPGN_RETURN_TEXT_NO_CARRIER  "NO_CARRIER\r\n"
#define ULPGN_RETURN_TEXT_ERROR       "ERROR\r\n"
#define ULPGN_RETURN_TEXT_NO_DIALTONE "NO_DIALTONE\r\n"
#define ULPGN_RETURN_TEXT_BUSY        "BUSY\r\n"
#define ULPGN_RETURN_TEXT_NO_ANSWER   "NO_ANSWER\r\n"
#define ULPGN_RETURN_TEXT_LENGTH      (13+1) /* strlen(ULPGN_RETURN_TEXT_NO_DIALTONE)+1 */

#define ULPGN_SOCKET_STATUS_TEXT_CLOSED          "CLOSED"
#define ULPGN_SOCKET_STATUS_TEXT_SOCKET          "SOCKET"
#define ULPGN_SOCKET_STATUS_TEXT_BOUND           "BOUND"
#define ULPGN_SOCKET_STATUS_TEXT_LISTEN          "LISTEN"
#define ULPGN_SOCKET_STATUS_TEXT_CONNECTED       "CONNECTED"

extern uint8_t ULPGN_USE_UART_NUM;

extern uint8_t g_wifi_cleateble_sockets;
extern uint8_t wifi_command_port;
extern uint8_t wifi_data_port;

int32_t sx_ulpgn_get_tcp_socket_status(uint8_t socket_no);

wifi_err_t R_WIFI_SX_ULPGN_Open(void);
wifi_err_t R_WIFI_SX_ULPGN_Close(void);
wifi_err_t R_WIFI_SX_ULPGN_Scan(wifi_scan_result_t *ap_results,uint32_t maxnetworks, uint32_t *exist_ap_count);
wifi_err_t R_WIFI_SX_ULPGN_SetDnsServerAddress(uint32_t dnsaddress1, uint32_t dnsaddress2);
wifi_err_t R_WIFI_SX_ULPGN_Connect (uint8_t *pssid, uint8_t *ppass, uint32_t security, uint8_t dhcp_enable, wifi_ip_configuration_t *pipconfig);
wifi_err_t R_WIFI_SX_ULPGN_Disconnect (void);
int32_t    R_WIFI_SX_ULPGN_IsConnected (void);
wifi_err_t R_WIFI_SX_ULPGN_GetMacAddress (uint8_t *pmacaddress);
wifi_err_t R_WIFI_SX_ULPGN_GetIpAddress (wifi_ip_configuration_t *pipconfig);
int32_t    R_WIFI_SX_ULPGN_SocketCreate(uint32_t type, uint32_t ipversion);
wifi_err_t R_WIFI_SX_ULPGN_SocketConnect(int32_t socket_no, uint32_t ipaddr, uint16_t port);
wifi_err_t R_WIFI_SX_ULPGN_SocketClose(int32_t socket_no);
int32_t    R_WIFI_SX_ULPGN_SocketSend (int32_t socket_no, uint8_t *pdata, int32_t length, uint32_t timeout_ms);
int32_t    R_WIFI_SX_ULPGN_SocketRecv (int32_t socket_no, uint8_t *pdata, int32_t length, uint32_t timeout_ms);
wifi_err_t R_WIFI_SX_ULPGN_SocketShutdown (int32_t socket_no);
wifi_err_t R_WIFI_SX_ULPGN_DnsQuery (uint8_t *pdomain_name, uint32_t *pipaddress);
wifi_err_t R_WIFI_SX_ULPGN_Ping (uint32_t ipaddr, uint16_t count, uint32_t intervalms);
uint32_t   R_WIFI_SX_ULPGN_GetVersion(void);

#endif /* #define SX_ULPGN_DRIVER_H */
