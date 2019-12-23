#ifndef R_WIFI_SX_ULPGN_PRIVATE_H
#define R_WIFI_SX_ULPGN_PRIVATE_H

#include "r_wifi_sx_ulpgn_if.h"
 
/* Configuration */
#define WIFI_NUMBER_OF_USE_UART			(2)

#define WIFI_UART_COMMAND_PORT			(0)
#define WIFI_UART_DATA_PORT			    (1)

/* Reset port pin macros.  */
#define WIFI_RESET_DDR(x, y)               (WIFI_RESET_DDR_PREPROC(x, y))
#define WIFI_RESET_DDR_PREPROC(x, y)       ((PORT ## x .PDR.BIT.B ## y))
#define WIFI_RESET_DR(x, y)               (WIFI_RESET_DR_PREPROC(x, y))
#define WIFI_RESET_DR_PREPROC(x, y)       ((PORT ## x .PODR.BIT.B ## y))

/* RTS port pin macros.  */
#define WIFI_RTS_DDR(x, y)               (WIFI_RTS_DDR_PREPROC(x, y))
#define WIFI_RTS_DDR_PREPROC(x, y)       ((PORT ## x .PDR.BIT.B ## y))
#define WIFI_RTS_DR(x, y)               (WIFI_RTS_DR_PREPROC(x, y))
#define WIFI_RTS_DR_PREPROC(x, y)       ((PORT ## x .PODR.BIT.B ## y))

#define WIFI_RETURN_TEXT_OK          "OK\r\n"
#define WIFI_RETURN_TEXT_ERROR       "ERROR\r\n"
#define WIFI_RETURN_TEXT_READY       "ready\r\n"
#define WIFI_RETURN_TEXT_OK_GO_SEND  "OK\r\n> "
#define WIFI_RETURN_TEXT_SEND_BYTE   " bytes\r\n"
#define WIFI_RETURN_TEXT_SEND_OK     "SEND OK\r\n"
#define WIFI_RETURN_TEXT_SEND_FAIL   "SEND FAIL\r\n"
#define WIFI_RETURN_TEXT_CONNECT     "CONNECT\r\n"
#define WIFI_RETURN_TEXT_BUSY        "BUSY\r\n"
#define WIFI_RETURN_TEXT_NOCARRIER   "NO CARRIER\r\n"
#define WIFI_RETURN_TEXT_NOANSWER    "NO ANSWER\r\n"



#define WIFI_AT_COMMAND_BUFF_SIZE		256
#define WIFI_AT_RESPONSE_BUFF_SIZE		256

#define WIFI_SOCKET_SENDABLE_DATASIZE   1420

#define WIFI_UART_BAUDRATE_DEFAULT      115200


//#define DEBUGLOG 0
//#define DEBUGLOG 1
#define DEBUGLOG 2


typedef enum
{
	WIFI_RETURN_ENUM_OK            = 0,
	WIFI_RETURN_ENUM_ERROR,
	WIFI_RETURN_ENUM_READY,
	WIFI_RETURN_ENUM_CONNECT,
	WIFI_RETURN_ENUM_BUSY,
	WIFI_RETURN_ENUM_NOCARRIER,
	WIFI_RETURN_ENUM_PROCESSING,
	WIFI_RETURN_ENUM_INTERNAL_TIMEOUT,
	WIFI_RETURN_ENUM_MAX,
}wifi_return_code_t;

typedef enum
{
    WIFI_SYSTEM_CLOSE=0,
    WIFI_SYSTEM_OPEN,
    WIFI_SYSTEM_CONNECT,
} wifi_system_status_t;

typedef enum
{
    WIFI_SOCKET_STATUS_CLOSED=0,	//CLOSED
    WIFI_SOCKET_STATUS_SOCKET,		//SOCKET
    WIFI_SOCKET_STATUS_BOUND,		//BOUND
    WIFI_SOCKET_STATUS_LISTEN,		//LISTEN
    WIFI_SOCKET_STATUS_CONNECTED,	//CONNECTED
    WIFI_SOCKET_STATUS_MAX,	        //MAX
} wifi_socket_status_t;

typedef enum
{
	WIFI_COMMAND_NONE = 0,
	WIFI_COMMAND_SET_REBOOT,
	WIFI_COMMAND_SET_ECHO_OFF,
	WIFI_COMMAND_SET_UART_CHANGE_TO_2,
	WIFI_COMMAND_SET_UART_CHANGE_TO_21,
	WIFI_COMMAND_SET_UART_HISPEED,
	WIFI_COMMAND_SET_UART_FLOW_TIMEOUT,
	WIFI_COMMAND_SET_ESCAPE_GUARD_TIME,
	WIFI_COMMAND_SET_BUFFER_THRESHOLD,
	WIFI_COMMAND_SET_WIFI_DISCONNECT,
	WIFI_COMMAND_SET_AT_RECV_TIMEOUT,
	WIFI_COMMAND_SET_AUTOCLOSE,
	WIFI_COMMAND_SET_AUTO_TRANSPARENT_MODE,
	WIFI_COMMAND_SET_DNS_SRV_ADDRESS,
	WIFI_COMMAND_SET_STATIC_IP,
	WIFI_COMMAND_SET_WIFI_AUTOCONNECT,
	WIFI_COMMAND_SET_WIFI_ACT_MODE,
	WIFI_COMMAND_SET_DHCP_MODE,
	WIFI_COMMAND_SET_MULTIPLE_SOCKET,
	WIFI_COMMAND_SET_WIFI_CONNECT,
	WIFI_COMMAND_SET_DNSQUERY,
	WIFI_COMMAND_SET_PING,
	WIFI_COMMAND_SET_SSLCONFIG,
	WIFI_COMMAND_SET_SOCKET_CREATE,
	WIFI_COMMAND_SET_SOCKET_CONNECT,
	WIFI_COMMAND_SET_SOCKET_SEND_START,
	WIFI_COMMAND_SET_SOCKET_SEND_DATA,
	WIFI_COMMAND_SET_SOCKET_CLOSE,
	WIFI_COMMAND_SET_SOCKET_CHANGE,
	WIFI_COMMAND_SET_TRANSPARENT_MODE,
	WIFI_COMMAND_SET_COMMAND_MODE,
	WIFI_COMMAND_GET_SOCKET_STATUS,
	WIFI_COMMAND_GET_MODULE_VERSION,
	WIFI_COMMAND_GET_UART_BAUDRATE,
	WIFI_COMMAND_GET_APLIST,
	WIFI_COMMAND_GET_MACADDRESS,
	WIFI_COMMAND_GET_IPADDRESS,
	WIFI_COMMAND_GET_SENT_RECV_SIZE,
	WIFI_COMMAND_GET_CURRENT_SSID,
	WIFI_COMMAND_LIST_MAX
}wifi_command_list_t;

typedef enum
{
	WIFI_RETURN_STRING_TEXT            = 0,
	WIFI_RETURN_STRING_MAX,
}wifi_return_string_t;

typedef struct wifi_socket_tag
{
	uint8_t socket_create_flag;
	uint8_t socket_status;
	uint8_t ipversion;
	uint8_t protocol;
	uint32_t receive_num;
	uint32_t receive_count;
	uint32_t put_error_count;
	uint8_t socket_recv_buff[WIFI_CFG_SOCKETS_RECEIVE_BUFFER_SIZE];
	byteq_hdl_t socket_byteq_hdl;
    TickType_t send_starttime;
    TickType_t send_thistime;
    TickType_t send_endtime;
    TickType_t recv_starttime;
    TickType_t recv_thistime;
    TickType_t recv_endtime;
    uint8_t send_timeout_overflow_flag;
    uint8_t recv_timeout_overflow_flag;
    uint8_t ssl_flag;
    uint8_t ssl_type;
    uint8_t ssl_cert_key_id;
    uint8_t ssl_ca_id;
}wifi_socket_t;

typedef struct
{
	sci_hdl_t   wifi_uart_sci_handle;
	sci_cfg_t   sci_config;
	uint8_t     *command_buff;
	uint32_t    command_buff_size;
	uint8_t     *response_buff;
	uint32_t    response_buff_size;
	TickType_t  starttime;
	TickType_t  thistime;
	TickType_t  endtime;
	TickType_t  startbytetime;
	TickType_t  thisbytetime;
	TickType_t  endbytetime;
	volatile uint8_t     tx_end_flag;
	uint8_t     timeout_overflow_flag;
	uint8_t     byte_timeout_overflow_flag;
}wifi_at_communication_info_t;

typedef struct
{
	wifi_command_list_t 	at_command_id;
	int32_t				 	socket_no;
	wifi_return_code_t	    result;
	uint32_t	    		ticket_no;
}wifi_at_execute_queue_t;

extern wifi_at_communication_info_t g_wifi_uart[WIFI_NUMBER_OF_USE_UART];

extern wifi_system_status_t g_wifi_system_state;
extern uint8_t g_wifi_at_command_buff[WIFI_AT_COMMAND_BUFF_SIZE];
extern uint8_t g_wifi_at_response_buff[WIFI_AT_RESPONSE_BUFF_SIZE];

extern wifi_socket_t g_wifi_socket[WIFI_CFG_CREATABLE_SOCKETS];

extern uint8_t g_wifi_macaddress[6];
extern wifi_ip_configuration_t g_wifi_ipconfig;
extern uint32_t g_wifi_dnsaddress;
extern uint32_t g_wifi_dnsquery_subcount;


extern wifi_scan_result_t *gp_wifi_ap_results;
extern uint32_t g_wifi_aplistmax;
extern uint32_t g_wifi_aplist_stored_num;
extern uint32_t g_wifi_aplist_count;
extern uint32_t g_wifi_aplist_subcount;

extern uint8_t g_wifi_current_ssid[33];

extern uint32_t g_wifi_atustat_recv;
extern uint32_t g_wifi_atustat_sent;

extern wifi_at_execute_queue_t g_wifi_at_execute_queue[10];
extern uint8_t g_wifi_set_queue_index;
extern uint8_t g_wifi_get_queue_index;

extern uint32_t g_wifi_sci_err_flag;

extern uint8_t g_wifi_socket_status;
extern uint8_t  g_wifi_transparent_mode;

extern const uint8_t * const wifi_socket_status_tbl[];

void wifi_init_at_execute_queue(void);
uint32_t    wifi_set_request_in_queue( wifi_command_list_t command, int32_t socket );
wifi_at_execute_queue_t * wifi_get_current_running_queue(void);
void    wifi_set_result_to_current_running_queue( wifi_return_code_t result );
int8_t  wifi_get_result_from_queue( uint32_t ticket_no, wifi_return_code_t *result);
int32_t wifi_start_recv_task( void );
void wifi_delete_recv_task( void );


#endif /*#define R_WIFI_SX_ULPGN_PRIVATE_H */
