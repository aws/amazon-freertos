#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "FreeRTOS.h"

#include "platform.h"
#include "r_sci_rx_if.h"
#include "r_byteq_if.h"
#include "r_wifi_sx_ulpgn_if.h"
#include "r_wifi_sx_ulpgn_private.h"
#include "r_sci_rx_if.h"

#if !defined(WIFI_CFG_SCI_CHANNEL)
#error "Error! Need to define WIFI_CFG_SCI_CHANNEL in r_wifi_sx_ulpgn_config.h"
#elif WIFI_CFG_SCI_CHANNEL == (0)
#define R_SCI_PinSet_wifi_serial_default()   R_SCI_PinSet_SCI0()
#define SCI_CH_wifi_serial_default           SCI_CH0
#define SCI_TX_BUSIZ_DEFAULT                 SCI_CFG_CH0_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                 SCI_CFG_CH0_RX_BUFSIZ
#elif WIFI_CFG_SCI_CHANNEL == (1)
#define R_SCI_PinSet_wifi_serial_default()   R_SCI_PinSet_SCI1()
#define SCI_CH_wifi_serial_default           SCI_CH1
#define SCI_TX_BUSIZ_DEFAULT                 SCI_CFG_CH1_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                 SCI_CFG_CH1_RX_BUFSIZ
#elif WIFI_CFG_SCI_CHANNEL == (2)
#define R_SCI_PinSet_wifi_serial_default()   R_SCI_PinSet_SCI2()
#define SCI_CH_wifi_serial_default           SCI_CH2
#define SCI_TX_BUSIZ_DEFAULT                 SCI_CFG_CH2_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                 SCI_CFG_CH2_RX_BUFSIZ
#elif WIFI_CFG_SCI_CHANNEL == (3)
#define R_SCI_PinSet_wifi_serial_default()   R_SCI_PinSet_SCI3()
#define SCI_CH_wifi_serial_default           SCI_CH3
#define SCI_TX_BUSIZ_DEFAULT                 SCI_CFG_CH3_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                 SCI_CFG_CH3_RX_BUFSIZ
#elif WIFI_CFG_SCI_CHANNEL == (4)
#define R_SCI_PinSet_wifi_serial_default()   R_SCI_PinSet_SCI4()
#define SCI_CH_wifi_serial_default           SCI_CH4
#define SCI_TX_BUSIZ_DEFAULT                 SCI_CFG_CH4_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                 SCI_CFG_CH4_RX_BUFSIZ
#elif WIFI_CFG_SCI_CHANNEL == (5)
#define R_SCI_PinSet_wifi_serial_default()   R_SCI_PinSet_SCI5()
#define SCI_CH_wifi_serial_default           SCI_CH5
#define SCI_TX_BUSIZ_DEFAULT                 SCI_CFG_CH5_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                 SCI_CFG_CH5_RX_BUFSIZ
#elif WIFI_CFG_SCI_CHANNEL == (6)
#define R_SCI_PinSet_wifi_serial_default()   R_SCI_PinSet_SCI6()
#define SCI_CH_wifi_serial_default           SCI_CH6
#define SCI_TX_BUSIZ_DEFAULT                 SCI_CFG_CH6_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                 SCI_CFG_CH6_RX_BUFSIZ
#elif WIFI_CFG_SCI_CHANNEL == (7)
#define R_SCI_PinSet_wifi_serial_default()   R_SCI_PinSet_SCI7()
#define SCI_CH_wifi_serial_default           SCI_CH7
#define SCI_TX_BUSIZ_DEFAULT                 SCI_CFG_CH7_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                 SCI_CFG_CH7_RX_BUFSIZ
#elif WIFI_CFG_SCI_CHANNEL == (8)
#define R_SCI_PinSet_wifi_serial_default()   R_SCI_PinSet_SCI8()
#define SCI_CH_wifi_serial_default           SCI_CH8
#define SCI_TX_BUSIZ_DEFAULT                 SCI_CFG_CH8_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                 SCI_CFG_CH8_RX_BUFSIZ
#elif WIFI_CFG_SCI_CHANNEL == (9)
#define R_SCI_PinSet_wifi_serial_default()   R_SCI_PinSet_SCI9()
#define SCI_CH_wifi_serial_default           SCI_CH9
#define SCI_TX_BUSIZ_DEFAULT                 SCI_CFG_CH9_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                 SCI_CFG_CH9_RX_BUFSIZ
#elif WIFI_CFG_SCI_CHANNEL == (10)
#define R_SCI_PinSet_wifi_serial_default()   R_SCI_PinSet_SCI10()
#define SCI_CH_wifi_serial_default           SCI_CH10
#define SCI_TX_BUSIZ_DEFAULT                 SCI_CFG_CH10_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                 SCI_CFG_CH10_RX_BUFSIZ
#elif WIFI_CFG_SCI_CHANNEL == (11)
#define R_SCI_PinSet_wifi_serial_default()   R_SCI_PinSet_SCI11()
#define SCI_CH_wifi_serial_default           SCI_CH11
#define SCI_TX_BUSIZ_DEFAULT                 SCI_CFG_CH11_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                 SCI_CFG_CH11_RX_BUFSIZ
#elif WIFI_CFG_SCI_CHANNEL == (12)
#define R_SCI_PinSet_wifi_serial_default()   R_SCI_PinSet_SCI12()
#define SCI_CH_wifi_serial_default           SCI_CH12
#define SCI_TX_BUSIZ_DEFAULT                 SCI_CFG_CH12_TX_BUFSIZ
#define SCI_RX_BUSIZ_DEFAULT                 SCI_CFG_CH12_RX_BUFSIZ
#else
#error "Error! Invalid setting for WIFI_CFG_SCI_CHANNEL in r_wifi_sx_ulpgn_config.h"
#endif

#if !defined(WIFI_CFG_SCI_SECOND_CHANNEL)
#error "Error! Need to define WIFI_CFG_SCI_SECOND_CHANNEL in r_bsp_config.h"
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (0)
#define R_SCI_PinSet_wifi_serial_second()      R_SCI_PinSet_SCI0()
#define SCI_CH_wifi_serial_second              SCI_CH0
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH0_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH0_RX_BUFSIZ
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (1)
#define R_SCI_PinSet_wifi_serial_second()      R_SCI_PinSet_SCI1()
#define SCI_CH_wifi_serial_second              SCI_CH1
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH1_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH1_RX_BUFSIZ
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (2)
#define R_SCI_PinSet_wifi_serial_second()      R_SCI_PinSet_SCI2()
#define SCI_CH_wifi_serial_second              SCI_CH2
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH2_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH2_RX_BUFSIZ
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (3)
#define R_SCI_PinSet_wifi_serial_second()      R_SCI_PinSet_SCI3()
#define SCI_CH_wifi_serial_second              SCI_CH3
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH3_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH3_RX_BUFSIZ
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (4)
#define R_SCI_PinSet_wifi_serial_second()      R_SCI_PinSet_SCI4()
#define SCI_CH_wifi_serial_second              SCI_CH4
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH4_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH4_RX_BUFSIZ
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (5)
#define R_SCI_PinSet_wifi_serial_second()      R_SCI_PinSet_SCI5()
#define SCI_CH_wifi_serial_second              SCI_CH5
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH5_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH5_RX_BUFSIZ
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (6)
#define R_SCI_PinSet_wifi_serial_second()      R_SCI_PinSet_SCI6()
#define SCI_CH_wifi_serial_second              SCI_CH6
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH6_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH6_RX_BUFSIZ
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (7)
#define R_SCI_PinSet_wifi_serial_second()      R_SCI_PinSet_SCI7()
#define SCI_CH_wifi_serial_second              SCI_CH7
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH7_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH7_RX_BUFSIZ
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (8)
#define R_SCI_PinSet_wifi_serial_second()      R_SCI_PinSet_SCI8()
#define SCI_CH_wifi_serial_second              SCI_CH8
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH8_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH8_RX_BUFSIZ
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (9)
#define R_SCI_PinSet_wifi_serial_second()      R_SCI_PinSet_SCI9()
#define SCI_CH_wifi_serial_second              SCI_CH9
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH9_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH9_RX_BUFSIZ
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (10)
#define R_SCI_PinSet_wifi_serial_second()      R_SCI_PinSet_SCI10()
#define SCI_CH_wifi_serial_second              SCI_CH10
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH10_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH10_RX_BUFSIZ
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (11)
#define R_SCI_PinSet_wifi_serial_second()      R_SCI_PinSet_SCI11()
#define SCI_CH_wifi_serial_second              SCI_CH11
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH11_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH11_RX_BUFSIZ
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (12)
#define R_SCI_PinSet_wifi_serial_second()      R_SCI_PinSet_SCI12()
#define SCI_CH_wifi_serial_second              SCI_CH12
#define SCI_TX_BUSIZ_SECOND                    SCI_CFG_CH12_TX_BUFSIZ
#define SCI_RX_BUSIZ_SECOND                    SCI_CFG_CH12_RX_BUFSIZ
#elif WIFI_CFG_SCI_SECOND_CHANNEL == (-1)
#define R_SCI_PinSet_wifi_serial_second()
#define SCI_CH_wifi_serial_second
#define SCI_TX_BUSIZ_SECOND
#define SCI_RX_BUSIZ_SECOND
#else
#error "Error! Invalid setting for WIFI_CFG_SCI_SECOND_CHANNEL in r_bsp_config.h"
#endif

#define ULPGN_UART_DEFAULT_PORT (0)
#define ULPGN_UART_SECOND_PORT (1)

#define MUTEX_TX (1 << 0)
#define MUTEX_RX (1 << 1)

uint8_t ULPGN_USE_UART_NUM = 2;

uint32_t g_wifi_tx_busiz_command;
uint32_t g_wifi_tx_busiz_data;
uint32_t g_wifi_rx_busiz_command;
uint32_t g_wifi_rx_busiz_data;

uint8_t g_wifi_cleateble_sockets;
uint8_t wifi_command_port;
uint8_t wifi_data_port;

uint8_t  g_wifi_transparent_mode = 0;

const uint8_t ulpgn_socket_status_closed[]       = ULPGN_SOCKET_STATUS_TEXT_CLOSED;
const uint8_t ulpgn_socket_status_socket[]       = ULPGN_SOCKET_STATUS_TEXT_SOCKET;
const uint8_t ulpgn_socket_status_bound[]        = ULPGN_SOCKET_STATUS_TEXT_BOUND;
const uint8_t ulpgn_socket_status_listen[]       = ULPGN_SOCKET_STATUS_TEXT_LISTEN;
const uint8_t ulpgn_socket_status_connected[]    = ULPGN_SOCKET_STATUS_TEXT_CONNECTED;

const uint8_t ulpgn_return_dummy[]   = "";

const uint32_t g_wifi_serial_buffsize_table[2][4] =
{
    /*SCI_TX_BUSIZ_DEFAULT*//*SCI_TX_BUSIZ_SIZE*/   /*SCI_RX_BUSIZ_DEFAULT*/ /*SCI_RX_BUSIZ_SIZE*/
    {SCI_TX_BUSIZ_DEFAULT,  SCI_TX_BUSIZ_DEFAULT,  SCI_RX_BUSIZ_DEFAULT,  SCI_RX_BUSIZ_DEFAULT,  }, /*ULPGN_USE_UART_NUM = 1 */
    {SCI_TX_BUSIZ_SECOND,   SCI_TX_BUSIZ_DEFAULT,  SCI_TX_BUSIZ_SECOND,   SCI_RX_BUSIZ_DEFAULT,  }, /*ULPGN_USE_UART_NUM = 2 */
};


const uint8_t * const wifi_socket_status_tbl[ULPGN_SOCKET_STATUS_MAX] =
{
    ulpgn_socket_status_closed,
    ulpgn_socket_status_socket,
    ulpgn_socket_status_bound,
    ulpgn_socket_status_listen,
    ulpgn_socket_status_connected,
};

volatile uint8_t current_socket_index;


static int32_t wifi_execute_at_command(uint8_t serial_ch_id, uint8_t *ptextstring, uint16_t timeout_ms, wifi_return_code_t expect_code,  wifi_command_list_t command, int32_t socket_no);

static int32_t wifi_take_mutex(uint8_t mutex_flag);
static void wifi_give_mutex(uint8_t mutex_flag);

static void wifi_uart_callback_second_port_for_command(void *pArgs);
static void wifi_uart_callback_default_port_for_inititial(void *pArgs);
static void wifi_uart_callback_default_port_for_data(void *pArgs);
static void timeout_init(int32_t serial_ch, uint16_t timeout_ms);
static void bytetimeout_init(int32_t serial_ch, uint16_t timeout_ms);
static int32_t check_timeout(int32_t serial_ch, int32_t rcvcount);
static int32_t check_bytetimeout(int32_t serial_ch, int32_t rcvcount);
static void socket_timeout_init(uint8_t socket_no, uint32_t timeout_ms, uint8_t flag);
static int32_t socket_check_timeout(uint8_t socket_no, uint8_t flag);

static int32_t wifi_get_ipaddress(void);
static int32_t wifi_serial_open_for_initial(void);
static int32_t wifi_serial_open_for_data(void);
static int32_t wifi_serial_default_port_close(void);
static int32_t wifi_change_command_mode(void);
static int32_t wifi_change_transparent_mode(void);
static int32_t wifi_serial_second_port_open(void);
static int32_t wifi_serial_second_port_close(void);
static int32_t wifi_check_uart_state(uint32_t *uart_receive_status, uint32_t *uart_send_status);

static int32_t wifi_serial_close(void);

static int32_t wifi_get_socket_status(uint8_t socket_no);
static int32_t wifi_change_socket_index(uint8_t socket_no);

/**
 * @brief The global mutex to ensure that only one operation is accessing the
 * g_wifi_tx_semaphore flag at one time.
 */
static SemaphoreHandle_t g_wifi_tx_semaphore = NULL;
static SemaphoreHandle_t g_wifi_rx_semaphore = NULL;

/**
 * @brief Maximum time in ticks to wait for obtaining a semaphore.
 */
static const TickType_t xMaxSemaphoreBlockTime = pdMS_TO_TICKS( 10000UL );

static uint8_t g_temporary_byteq_enable_flag = 0;
static uint8_t g_sockindex_command_flag = 0;

wifi_system_status_t g_wifi_system_state = WIFI_SYSTEM_CLOSE;
uint8_t g_wifi_at_command_buff[WIFI_AT_COMMAND_BUFF_SIZE];
uint8_t g_wifi_at_response_buff[WIFI_AT_RESPONSE_BUFF_SIZE];

wifi_at_communication_info_t g_wifi_uart[WIFI_NUMBER_OF_USE_UART];

uint32_t g_wifi_tx_busiz_command;
uint32_t g_wifi_tx_busiz_data;
uint32_t g_wifi_rx_busiz_command;
uint32_t g_wifi_rx_busiz_data;

uint32_t g_wifi_sci_err_flag;

wifi_err_t R_WIFI_SX_ULPGN_Open(void)
{
    int32_t ret;
    sci_baud_t change_baud;
	uint8_t *pcbuff;
	wifi_err_t api_ret = WIFI_SUCCESS;
	uint8_t open_phase;
	uint8_t byteq_open_count;
	int32_t k;

	if( WIFI_SYSTEM_CLOSE != g_wifi_system_state)
	{
		return WIFI_ERR_ALREADY_OPEN;
	}

	/* Memory initialize */

	wifi_command_port = WIFI_UART_COMMAND_PORT;

    memset(g_wifi_uart, 0, sizeof(g_wifi_uart));
    g_wifi_uart[WIFI_UART_COMMAND_PORT].command_buff = g_wifi_at_command_buff;
    g_wifi_uart[WIFI_UART_COMMAND_PORT].command_buff_size = sizeof(g_wifi_at_command_buff);
    g_wifi_uart[WIFI_UART_COMMAND_PORT].response_buff = g_wifi_at_response_buff;
    g_wifi_uart[WIFI_UART_COMMAND_PORT].response_buff_size = sizeof(g_wifi_at_response_buff);
    g_wifi_uart[WIFI_UART_DATA_PORT].command_buff = g_wifi_at_command_buff;
    g_wifi_uart[WIFI_UART_DATA_PORT].command_buff_size = sizeof(g_wifi_at_command_buff);
    g_wifi_uart[WIFI_UART_DATA_PORT].response_buff = g_wifi_at_response_buff;
    g_wifi_uart[WIFI_UART_DATA_PORT].response_buff_size = sizeof(g_wifi_at_response_buff);

    pcbuff = g_wifi_uart[wifi_command_port].command_buff;

	g_wifi_tx_busiz_command = g_wifi_serial_buffsize_table[0][0];
	g_wifi_tx_busiz_data    = g_wifi_serial_buffsize_table[0][0];
	g_wifi_rx_busiz_command = g_wifi_serial_buffsize_table[0][1];
	g_wifi_rx_busiz_data    = g_wifi_serial_buffsize_table[0][1];

	memset(g_wifi_socket, 0, sizeof(g_wifi_socket));

	wifi_init_at_execute_queue();

	open_phase = 0;
	/* Phase 1: ByteQ initialize */
	for(byteq_open_count = 0;byteq_open_count <WIFI_CFG_CREATABLE_SOCKETS; byteq_open_count++)
	{
		if(BYTEQ_SUCCESS != R_BYTEQ_Open(g_wifi_socket[byteq_open_count].socket_recv_buff, sizeof(g_wifi_socket[byteq_open_count].socket_recv_buff), &g_wifi_socket[byteq_open_count].socket_byteq_hdl))
		{
			break;
		}
	}

	if(byteq_open_count >= WIFI_CFG_CREATABLE_SOCKETS)
	{
		/* Success */
		open_phase |= 0x01;
	}
	else
	{
		api_ret = WIFI_ERR_BYTEQ_OPEN;
	}

	if(WIFI_SUCCESS == api_ret)
	{
	    /* Phase 3 Semaphore tx initialize */
	    if (g_wifi_tx_semaphore != NULL)
	    {
	        vSemaphoreDelete(g_wifi_tx_semaphore);
	    }
	    g_wifi_tx_semaphore = xSemaphoreCreateMutex();

	    if( g_wifi_tx_semaphore != NULL )
	    {
			/* Success */
			open_phase |= 0x02;
	    }
	    else
	    {
	    	api_ret = WIFI_ERR_TAKE_MUTEX;
	    }
	}

	if(WIFI_SUCCESS == api_ret)
	{
	    /* Phase 4 Semaphore rx initialize */
	    if (g_wifi_rx_semaphore != NULL)
	    {
	        vSemaphoreDelete(g_wifi_rx_semaphore);
	    }

	    g_wifi_rx_semaphore = xSemaphoreCreateMutex();

	    if( g_wifi_rx_semaphore != NULL )
	    {
			/* Success */
			open_phase |= 0x04;
	    }
	    else
	    {
	    	api_ret = WIFI_ERR_TAKE_MUTEX;
	    }
	}

	if(WIFI_SUCCESS == api_ret)
	{
	    /* Phase 4 Serial initialize */
        WIFI_RESET_DR(WIFI_CFG_RESET_PORT, WIFI_CFG_RESET_PIN) = 0;
        WIFI_RESET_DDR(WIFI_CFG_RESET_PORT, WIFI_CFG_RESET_PIN) = 1;
	    R_BSP_SoftwareDelay(26, BSP_DELAY_MILLISECS); /* 5us mergin 1us */
		WIFI_RESET_DR(WIFI_CFG_RESET_PORT, WIFI_CFG_RESET_PIN) = 1;
	    R_BSP_SoftwareDelay(200, BSP_DELAY_MILLISECS); /*  */

		ret = wifi_serial_open_for_initial();
		if(0 == ret)
		{
			/* Success */
			open_phase |= 0x08;
		}
		else
		{
	    	api_ret = WIFI_ERR_SERIAL_OPEN;
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
	    ret = wifi_serial_second_port_open();
	    ret = 0;
	    if(ret == 0)
	    {
			/* Success */
			open_phase |= 0x08;
	    }
	    else
	    {
	    	api_ret = WIFI_ERR_SERIAL_OPEN;
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
	    /* Phase 5 Task initialize */
		if(0 == wifi_start_recv_task())
		{
			vTaskDelay( 2 );
			/* Success */
			open_phase |= 0x10;
		}
		else
		{
			api_ret = WIFI_ERR_SERIAL_OPEN;
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
		/* no echo */
		ret = wifi_execute_at_command(wifi_command_port, "ATE0\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_ECHO_OFF, 0xff);
		if(ret != 0)
		{
			api_ret = WIFI_ERR_MODULE_COM;
		}
	}

	if(WIFI_SUCCESS == api_ret)
	{
		/* no echo */
		ret = wifi_execute_at_command(wifi_command_port, "ATWREV\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_GET_MODULE_VERSION, 0xff);
		if(ret != 0)
		{
			api_ret = WIFI_ERR_MODULE_COM;
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
	    /* Command Port = HSUART2 */
		ret = wifi_execute_at_command(wifi_command_port, "ATUART=2\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_UART_CHANGE_TO_2, 0xff);
	    if(ret != 0)
	    {
			api_ret = WIFI_ERR_MODULE_COM;
	    }
	    else
	    {
	    	R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS); /* 1 sec */
	    	wifi_command_port = ULPGN_UART_SECOND_PORT;
	        wifi_serial_default_port_close();
	    }
	}

	if(WIFI_SUCCESS == api_ret)
	{
		/* Command Port = HSUART2, Data Port = HSUART1 */
		ret = wifi_execute_at_command(wifi_command_port, "ATE0\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_ECHO_OFF, 0xff);
		if(ret == 0)
		{
			/* Multiple Connection */
	        /* Change HSUART1 baudrate and flow control. */
#if (WIFI_CFG_SCI_USE_FLOW_CONTROL == 1)
	        sprintf((char *)g_wifi_uart[wifi_command_port].command_buff, "ATBX1=%d,,,,h\r",WIFI_CFG_SCI_BAUDRATE);
#else
	        sprintf((char *)g_wifi_uart[wifi_command_port].command_buff, "ATBX1=%d,,,,\r",WIFI_CFG_SCI_BAUDRATE);
#endif
			ret = wifi_execute_at_command(wifi_command_port, g_wifi_uart[wifi_command_port].command_buff, 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_UART_HISPEED, 0xff);
	        if(ret != 0)
	        {
	            return ret;
	        }

	        ret = wifi_serial_open_for_data();
	        if(ret != 0)
	        {
	            return ret;
	        }
	        R_SCI_Control(g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle, SCI_CMD_EN_CTS_IN, NULL);
	        WIFI_RTS_DR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 0;
	        WIFI_RTS_DDR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 1;
	        R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS);

			ret = wifi_execute_at_command(wifi_command_port, "ATUART=2,1\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_UART_CHANGE_TO_21, 0xff);
	        if(ret != 0)
	        {
	            return ret;
	        }
	        R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS); /* 1 sec */

	        ULPGN_USE_UART_NUM = 2;
	        g_wifi_cleateble_sockets = WIFI_CFG_CREATABLE_SOCKETS;
	        wifi_command_port = ULPGN_UART_SECOND_PORT;
	        wifi_data_port = ULPGN_UART_DEFAULT_PORT;
	        g_wifi_tx_busiz_command = g_wifi_serial_buffsize_table[1][0];
	        g_wifi_tx_busiz_data    = g_wifi_serial_buffsize_table[1][1];
	        g_wifi_rx_busiz_command = g_wifi_serial_buffsize_table[1][2];
	        g_wifi_rx_busiz_data    = g_wifi_serial_buffsize_table[1][3];


		}
		else
		{
			/* Single Connection */
	        ret = wifi_serial_second_port_close();

	    	wifi_init_at_execute_queue();
	        ULPGN_USE_UART_NUM = 1;
	        g_wifi_cleateble_sockets = 1;
	        wifi_command_port = ULPGN_UART_DEFAULT_PORT;
	        wifi_data_port = ULPGN_UART_DEFAULT_PORT;
	        g_wifi_tx_busiz_command = g_wifi_serial_buffsize_table[0][0];
	        g_wifi_tx_busiz_data    = g_wifi_serial_buffsize_table[0][1];
	        g_wifi_rx_busiz_command = g_wifi_serial_buffsize_table[0][2];
	        g_wifi_rx_busiz_data    = g_wifi_serial_buffsize_table[0][3];

	        /* Wifi Module hardware reset   */
	        WIFI_RESET_DR(WIFI_CFG_RESET_PORT, WIFI_CFG_RESET_PIN) = 0;
	        WIFI_RESET_DDR(WIFI_CFG_RESET_PORT, WIFI_CFG_RESET_PIN) = 1;
	        R_BSP_SoftwareDelay(26, BSP_DELAY_MILLISECS); /* 5us mergin 1us */
	        WIFI_RESET_DR(WIFI_CFG_RESET_PORT, WIFI_CFG_RESET_PIN) = 1;
	        R_BSP_SoftwareDelay(200, BSP_DELAY_MILLISECS); /*  */

	        ret = wifi_serial_open_for_initial();
	        if(ret != 0)
	        {
	            return ret;
	        }


	        /* no echo */
			ret = wifi_execute_at_command(wifi_command_port, "ATE0\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_ECHO_OFF, 0xff);
	        if(ret != 0)
	        {
	            return ret;
	        }

	        /* Change HSUART1 baudrate and flow control. */
#if (WIFI_CFG_SCI_USE_FLOW_CONTROL == 1)
	        sprintf((char *)g_wifi_uart[wifi_command_port].command_buff, "ATBX1=%d,,,,h\r",WIFI_CFG_SCI_BAUDRATE);
#else
	        sprintf((char *)g_wifi_uart[wifi_command_port].command_buff, "ATBX1=%d,,,,\r",WIFI_CFG_SCI_BAUDRATE);
#endif
			ret = wifi_execute_at_command(wifi_command_port, g_wifi_uart[wifi_command_port].command_buff, 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_UART_HISPEED, 0xff);
	        if(ret != 0)
	        {
	            return ret;
	        }
	        change_baud.pclk = BSP_PCLKB_HZ;
	        change_baud.rate = WIFI_CFG_SCI_BAUDRATE;
	        R_SCI_Control(g_wifi_uart[wifi_command_port].wifi_uart_sci_handle, SCI_CMD_CHANGE_BAUD, &change_baud);
	        R_SCI_Control(g_wifi_uart[wifi_command_port].wifi_uart_sci_handle, SCI_CMD_EN_CTS_IN, NULL);
	        WIFI_RTS_DR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 0;
	        WIFI_RTS_DDR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 1;

	        R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS); /* 5us mergin 1us */

		}

	}
	if(WIFI_SUCCESS == api_ret)
	{
		/* UART transmission flow control busy retry timeout */
		ret = wifi_execute_at_command(wifi_command_port, "ATS108=1\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_UART_FLOW_TIMEOUT, 0xff);
		if(ret != 0)
		{
			api_ret = WIFI_ERR_MODULE_COM;
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
		/* Escape guard time = 200msec */
		ret = wifi_execute_at_command(wifi_command_port, "ATS12=1\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_ESCAPE_GUARD_TIME, 0xff);
		if(ret != 0)
		{
			api_ret = WIFI_ERR_MODULE_COM;
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
		/* Buffer size = 1420byte */
		ret = wifi_execute_at_command(wifi_command_port, "ATBSIZE=1420\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_BUFFER_THRESHOLD, 0xff);
		if(ret != 0)
		{
			api_ret = WIFI_ERR_MODULE_COM;
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
		/* Disconnect from currently connected Access Point, */
		ret = wifi_execute_at_command(wifi_command_port, "ATWD\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_WIFI_DISCONNECT, 0xff);
		if(ret != 0)
		{
			api_ret = WIFI_ERR_MODULE_COM;
		}
		else
		{
		    current_socket_index = 0;
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
		/* Receive timeout  */
		ret = wifi_execute_at_command(wifi_command_port, "ATTO=1\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_AT_RECV_TIMEOUT, 0xff);
		if(ret != 0)
		{
			api_ret = WIFI_ERR_MODULE_COM;
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
		/* Receive timeout  */
		ret = wifi_execute_at_command(wifi_command_port, "ATS110=0\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_AUTOCLOSE, 0xff);
		if(ret != 0)
		{
			api_ret = WIFI_ERR_MODULE_COM;
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
		/* Receive timeout  */
		ret = wifi_execute_at_command(wifi_command_port, "ATS105=0\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_AUTO_TRANSPARENT_MODE, 0xff);
		if(ret != 0)
		{
			api_ret = WIFI_ERR_MODULE_COM;
		}
	}
	g_wifi_system_state = WIFI_SYSTEM_OPEN;
    return api_ret;
}

wifi_err_t R_WIFI_SX_ULPGN_Close(void)
{
	int i;
	wifi_err_t api_ret = WIFI_SUCCESS;

	if(0 == R_WIFI_SX_ULPGN_IsConnected())
	{
		R_WIFI_SX_ULPGN_Disconnect();
	}
	wifi_serial_close();
	wifi_delete_recv_task();
	for(i=0; i<WIFI_CFG_CREATABLE_SOCKETS; i++)
	{
		R_BYTEQ_Close(g_wifi_socket[i].socket_byteq_hdl);
	}

	g_wifi_system_state = WIFI_SYSTEM_CLOSE;

	return api_ret;
}

wifi_err_t R_WIFI_SX_ULPGN_SetDnsServerAddress(uint32_t dnsaddress1, uint32_t dnsaddress2)
{
	int32_t ret;
	wifi_err_t api_ret = WIFI_SUCCESS;
    uint8_t mutex_flag;
	uint8_t *pcbuff = g_wifi_uart[wifi_command_port].command_buff;

	if( WIFI_SYSTEM_CLOSE == g_wifi_system_state)
	{
		return WIFI_ERR_NOT_OPEN;
	}

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != wifi_take_mutex(mutex_flag))
	{
		api_ret = WIFI_ERR_TAKE_MUTEX;
	}
	if(WIFI_SUCCESS == api_ret)
	{
		wifi_change_command_mode();
		if(dnsaddress1 != 0)
		{
			sprintf((char *)pcbuff,"ATNDNSSVR1=%d.%d.%d.%d\r",
					WIFI_ULONG_TO_IPV4BYTE_1(dnsaddress1), WIFI_ULONG_TO_IPV4BYTE_2(dnsaddress1),
					WIFI_ULONG_TO_IPV4BYTE_3(dnsaddress1), WIFI_ULONG_TO_IPV4BYTE_4(dnsaddress1));
			ret = wifi_execute_at_command(wifi_command_port, pcbuff, 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_DNS_SRV_ADDRESS, 0xff);
			if(ret != 0)
			{
				api_ret = WIFI_ERR_MODULE_COM;
			}
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
		if(dnsaddress2 != 0)
		{
			sprintf((char *)pcbuff,"ATNDNSSVR2=%d.%d.%d.%d\r",
					WIFI_ULONG_TO_IPV4BYTE_1(dnsaddress2), WIFI_ULONG_TO_IPV4BYTE_2(dnsaddress2),
					WIFI_ULONG_TO_IPV4BYTE_3(dnsaddress2), WIFI_ULONG_TO_IPV4BYTE_4(dnsaddress2));
			ret = wifi_execute_at_command(wifi_command_port, pcbuff, 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_DNS_SRV_ADDRESS, 0xff);
			if(ret != 0)
			{
				api_ret = WIFI_ERR_MODULE_COM;
			}
		}
	}
	if(WIFI_ERR_TAKE_MUTEX != api_ret)
	{
		wifi_give_mutex(mutex_flag);
	}
	return api_ret;

}



wifi_err_t R_WIFI_SX_ULPGN_Connect (uint8_t *pssid, uint8_t *ppass, uint32_t security, uint8_t dhcp_enable, wifi_ip_configuration_t *pipconfig)
{
    int32_t ret;
    char *pstr, pstr2;
    uint32_t security_encrypt_type = 1;
    uint8_t mutex_flag;
    uint8_t security_type;
    uint8_t retry_count;
    uint8_t getip_retry_count;
    wifi_err_t api_ret = WIFI_SUCCESS;
	uint8_t *pcbuff = g_wifi_uart[wifi_command_port].command_buff;

	if((pssid == NULL) || (ppass == NULL) || ((security != WIFI_SECURITY_WPA) && (security != WIFI_SECURITY_WPA2)) || (pipconfig == NULL))
	{
		return WIFI_ERR_PARAMETER;
	}
	if( WIFI_SYSTEM_CLOSE == g_wifi_system_state)
	{
		return WIFI_ERR_NOT_OPEN;
	}
	if( 0 == R_WIFI_SX_ULPGN_IsConnected())
	{
		/* Nothing to do. */
		return WIFI_SUCCESS;
	}

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != wifi_take_mutex(mutex_flag))
	{
		api_ret = WIFI_ERR_TAKE_MUTEX;
	}
	if(WIFI_SUCCESS == api_ret)
	{
		if(0 == dhcp_enable)
		{
			/* DHCP Not Use */
			ret = wifi_execute_at_command(wifi_command_port, "ATNDHCP=0\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_DHCP_MODE, 0xff);
			if(ret != 0)
			{
				api_ret = WIFI_ERR_MODULE_COM;
			}
			if(WIFI_SUCCESS == api_ret)
			{
				sprintf((char *)pcbuff,"ATNSET=%d.%d.%d.%d,%d.%d.%d.%d,%d.%d.%d.%d\r\n",
						WIFI_ULONG_TO_IPV4BYTE_1(pipconfig->ipaddress), WIFI_ULONG_TO_IPV4BYTE_2(pipconfig->ipaddress),
						WIFI_ULONG_TO_IPV4BYTE_3(pipconfig->ipaddress), WIFI_ULONG_TO_IPV4BYTE_4(pipconfig->ipaddress),
						WIFI_ULONG_TO_IPV4BYTE_1(pipconfig->subnetmask), WIFI_ULONG_TO_IPV4BYTE_2(pipconfig->subnetmask),
						WIFI_ULONG_TO_IPV4BYTE_3(pipconfig->subnetmask), WIFI_ULONG_TO_IPV4BYTE_4(pipconfig->subnetmask),
						WIFI_ULONG_TO_IPV4BYTE_1(pipconfig->gateway), WIFI_ULONG_TO_IPV4BYTE_2(pipconfig->gateway),
						WIFI_ULONG_TO_IPV4BYTE_3(pipconfig->gateway), WIFI_ULONG_TO_IPV4BYTE_4(pipconfig->gateway));

				/* Set Static IP address */
				ret = wifi_execute_at_command(wifi_command_port, pcbuff, 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_STATIC_IP, 0xff);
				if(ret != 0)
				{
					api_ret = WIFI_ERR_MODULE_COM;
				}
			}
		}
		else
		{
			/* DHCP Use */
			ret = wifi_execute_at_command(wifi_command_port, "ATNDHCP=1\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_DHCP_MODE, 0xff);
			if(ret != 0)
			{
				api_ret = WIFI_ERR_MODULE_COM;
			}
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
		if(0 ==  R_WIFI_SX_ULPGN_IsConnected())
		{
			/* If Wifi is already connected, do nothing and return fail. */
			wifi_give_mutex(mutex_flag);
			if(0 != R_WIFI_SX_ULPGN_Disconnect())
			{
				api_ret = WIFI_ERR_MODULE_COM;
			}
			if(WIFI_SUCCESS == api_ret)
			{
				if(0 != wifi_take_mutex(mutex_flag))
				{
					api_ret = WIFI_ERR_TAKE_MUTEX;
				}
			}
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
		R_BSP_SoftwareDelay(2000, BSP_DELAY_MILLISECS);

		if(security == WIFI_SECURITY_WPA)
		{
			security_type = 1;
		}
		else
		{
			security_type = 2;
		}

		for(retry_count = 0;retry_count < 3;retry_count++)
		{
			sprintf((char *)pcbuff,"ATWAWPA=%s,%d,1,1,%s\r",pssid,security_type,ppass);
			ret = wifi_execute_at_command(wifi_command_port, pcbuff, 20000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_WIFI_CONNECT, 0xff);
			if((0 == ret) || (-2 == ret))
			{
				if(-2 == ret)
				{
					memset(g_wifi_current_ssid, 0, sizeof(g_wifi_current_ssid));
					ret = wifi_execute_at_command(wifi_command_port, "ATW\r", 1000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_GET_CURRENT_SSID, 0xff);
					if(0 == ret)
					{
						ret = -1;
						if( 0 == strcmp((const char *)g_wifi_current_ssid, (const char *)pssid))
						{
							ret = 0;
						}
					}
				}
			}
			if(0 != ret)
			{
				wifi_execute_at_command(wifi_command_port, "ATWD\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_WIFI_DISCONNECT, 0xff);
			}
			else
			{
				break;
			}
		}
		if(retry_count >= 3)
		{
			api_ret = WIFI_ERR_MODULE_COM;
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
		if(0 != dhcp_enable)
		{
			for(getip_retry_count = 0;getip_retry_count<10;getip_retry_count++)
			{
				ret = wifi_get_ipaddress();
				if((0 == ret) && (g_wifi_ipconfig.ipaddress != 0))
				{
					break;
				}
				else
				{
					vTaskDelay(1000);
				}
			}
			if(getip_retry_count >=10)
			{
				wifi_execute_at_command(wifi_command_port, "ATWD\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_WIFI_DISCONNECT, 0xff);
				api_ret = WIFI_ERR_MODULE_COM;
			}
		}
	}

	if(WIFI_SUCCESS == api_ret)
	{
		g_wifi_system_state = WIFI_SYSTEM_CONNECT;
	}
	if(WIFI_ERR_TAKE_MUTEX != api_ret)
	{
		wifi_give_mutex(mutex_flag);
	}
    return api_ret;
}

wifi_err_t R_WIFI_SX_ULPGN_Disconnect(void)
{
	uint8_t mutex_flag;
	wifi_err_t api_ret = WIFI_SUCCESS;

	if( WIFI_SYSTEM_CLOSE == g_wifi_system_state)
	{
		return WIFI_ERR_NOT_OPEN;
	}
	if( WIFI_SYSTEM_OPEN == g_wifi_system_state)
	{
		/* Nothing to do. */
		return WIFI_SUCCESS;
	}

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != wifi_take_mutex(mutex_flag))
	{
		api_ret = WIFI_ERR_TAKE_MUTEX;
	}
	if(WIFI_SUCCESS == api_ret)
	{
		wifi_change_command_mode();
		wifi_execute_at_command(wifi_command_port, "ATWD\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_WIFI_DISCONNECT, 0xff);
		memset(&g_wifi_ipconfig, 0, sizeof(g_wifi_ipconfig));
		g_wifi_system_state = WIFI_SYSTEM_OPEN;
	}
	if(WIFI_ERR_TAKE_MUTEX != api_ret)
	{
		wifi_give_mutex(mutex_flag);
	}

    return api_ret;
}

int32_t R_WIFI_SX_ULPGN_IsConnected(void)
{
	int32_t ret = -1;

	if(WIFI_SYSTEM_CONNECT == g_wifi_system_state)
	{
		ret = 0;
	}
	return ret;
}



wifi_err_t R_WIFI_SX_ULPGN_GetMacAddress (uint8_t *pmacaddress)
{
	int32_t ret;
	uint8_t mutex_flag;
	wifi_err_t api_ret = WIFI_SUCCESS;

	if(NULL == pmacaddress)
	{
		return WIFI_ERR_PARAMETER;
	}
	if( WIFI_SYSTEM_CLOSE == g_wifi_system_state)
	{
		return WIFI_ERR_NOT_OPEN;
	}
    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != wifi_take_mutex(mutex_flag))
	{
		api_ret = WIFI_ERR_TAKE_MUTEX;
	}

    if(WIFI_SUCCESS == api_ret)
	{
		ret = wifi_execute_at_command(wifi_command_port, "ATW\r\n", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_GET_MACADDRESS, 0xff);
		if(ret != 0)
		{
			api_ret = WIFI_ERR_MODULE_COM;
		}
		memcpy(pmacaddress, g_wifi_macaddress, sizeof(g_wifi_macaddress));
	}
	if(WIFI_ERR_TAKE_MUTEX != api_ret)
	{
		wifi_give_mutex(mutex_flag);
	}
	return api_ret;
}

wifi_err_t R_WIFI_SX_ULPGN_GetIpAddress (wifi_ip_configuration_t *pipconfig)
{
	int32_t ret;
	uint8_t mutex_flag;
	wifi_err_t api_ret = WIFI_SUCCESS;

	if(NULL == pipconfig)
	{
		return WIFI_ERR_PARAMETER;
	}
	if( WIFI_SYSTEM_CLOSE == g_wifi_system_state)
	{
		return WIFI_ERR_NOT_OPEN;
	}

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != wifi_take_mutex(mutex_flag))
	{
		api_ret = WIFI_ERR_TAKE_MUTEX;
	}

    if(WIFI_SUCCESS == api_ret)
	{
        ret = wifi_get_ipaddress();
		if(ret != 0)
		{
			api_ret = WIFI_ERR_MODULE_COM;
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
		memcpy(pipconfig, &g_wifi_ipconfig, sizeof(wifi_ip_configuration_t));
	}
	if(WIFI_ERR_TAKE_MUTEX != api_ret)
	{
		wifi_give_mutex(mutex_flag);
	}
	return api_ret;
}

wifi_err_t R_WIFI_SX_ULPGN_Scan (wifi_scan_result_t *pap_results, uint32_t maxnetworks, uint32_t *exist_ap_list)
{
    int32_t i;
    int32_t ret;
    uint8_t retry_count;
    uint8_t mutex_flag;
	wifi_err_t api_ret = WIFI_SUCCESS;

    if ((NULL == pap_results) || (0 == maxnetworks) || (NULL == exist_ap_list))
    {
    	return WIFI_ERR_PARAMETER;
    }
	if( WIFI_SYSTEM_CLOSE == g_wifi_system_state)
	{
		return WIFI_ERR_NOT_OPEN;
	}

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != wifi_take_mutex(mutex_flag))
	{
		api_ret = WIFI_ERR_TAKE_MUTEX;
	}
	if(WIFI_SUCCESS == api_ret)
	{
	    gp_wifi_ap_results = pap_results;
	    g_wifi_aplistmax = maxnetworks;
		g_wifi_aplist_stored_num = 0;
		g_wifi_aplist_count = 0;
		g_wifi_aplist_subcount = 0;
		memset(gp_wifi_ap_results,0x00,sizeof(wifi_scan_result_t)*maxnetworks);
		for(i=0; i<maxnetworks; i++)
		{
			gp_wifi_ap_results[i].security = WIFI_SECURITY_UNDEFINED;
		}
	    // TODO investigate why this never returns the full response
		for (retry_count = 0; retry_count < 3; retry_count++)
		{
			ret = wifi_execute_at_command(wifi_command_port, "ATWS\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_GET_APLIST, 0xff);
			if(ret == 0)
			{
				*exist_ap_list = g_wifi_aplist_count;
				break;
	        }
	    }
		if(retry_count >= 3)
		{
			api_ret = WIFI_ERR_MODULE_COM;
		}
	}
	if(WIFI_ERR_TAKE_MUTEX != api_ret)
	{
		wifi_give_mutex(mutex_flag);
	}

	return api_ret;
}




int32_t sx_ulpgn_get_tcp_socket_status(uint8_t socket_no)
{
	if(socket_no >= g_wifi_cleateble_sockets)
	{
	    return -1;
	}
	return g_wifi_socket[socket_no].socket_status;
}

int32_t    R_WIFI_SX_ULPGN_SocketCreate(uint32_t type, uint32_t ipversion)
{
	int32_t i;
	int32_t ret = WIFI_ERR_SOCKET_CREATE;
	uint8_t mutex_flag;
	wifi_err_t api_ret = WIFI_SUCCESS;

	if(type != WIFI_SOCKET_IP_PROTOCOL_TCP || ipversion != WIFI_SOCKET_IP_VERSION_4)
	{
		return WIFI_ERR_PARAMETER;
	}
	if( 0 != R_WIFI_SX_ULPGN_IsConnected())
	{
		return WIFI_ERR_NOT_CONNECT;
	}

	mutex_flag = (MUTEX_TX | MUTEX_RX);
	if(0 != wifi_take_mutex(mutex_flag))
	{
		api_ret = WIFI_ERR_TAKE_MUTEX;
	}
	if(WIFI_SUCCESS == api_ret)
	{
		for(i=0;i<g_wifi_cleateble_sockets;i++)
		{
			if(g_wifi_socket[i].socket_create_flag == 0)
			{
				break;
			}
		}
		if(i >=g_wifi_cleateble_sockets)
		{
			api_ret = WIFI_ERR_SOCKET_NUM;
		}
		if(WIFI_SUCCESS == api_ret)
		{
			g_wifi_socket[i].socket_create_flag = 1;
			g_wifi_socket[i].ipversion = ipversion;
			g_wifi_socket[i].protocol = type;
			g_wifi_socket[i].socket_status = WIFI_SOCKET_STATUS_SOCKET;
			g_wifi_socket[i].ssl_flag = 0;
			g_wifi_socket[i].ssl_type = 0;
			R_BYTEQ_Flush(g_wifi_socket[i].socket_byteq_hdl);
			ret = i;
		}
		else
		{
			ret = WIFI_ERR_SOCKET_CREATE;
		}
	}
	if(api_ret != WIFI_ERR_TAKE_MUTEX)
	{
		/* Give back the socketInUse mutex. */
		wifi_give_mutex(mutex_flag);
	}
    return ret;
}

wifi_err_t R_WIFI_SX_ULPGN_SocketConnect(int32_t socket_no, uint32_t ipaddr, uint16_t port)
{
	int32_t ret;
	uint8_t mutex_flag;
	wifi_err_t api_ret = WIFI_SUCCESS;

	if( (0 == ipaddr) || (0 == port) )
	{
		return  WIFI_ERR_PARAMETER;
	}
	if( (socket_no >= WIFI_CFG_CREATABLE_SOCKETS) || (socket_no < 0) || (g_wifi_socket[socket_no].socket_status != WIFI_SOCKET_STATUS_SOCKET) )
	{
		return  WIFI_ERR_SOCKET_NUM;
	}
	if( 0 != R_WIFI_SX_ULPGN_IsConnected())
	{
		return WIFI_ERR_NOT_CONNECT;
	}

	mutex_flag = (MUTEX_TX | MUTEX_RX);
	if(0 != wifi_take_mutex(mutex_flag))
	{
		api_ret = WIFI_ERR_TAKE_MUTEX;
	}
	if(WIFI_SUCCESS == api_ret)
	{
		if(ULPGN_USE_UART_NUM == 2)
		{
			ret = wifi_change_socket_index(socket_no);
			if(ret != 0)
			{
				api_ret = WIFI_ERR_CHANGE_SOCKET;
			}
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
		sprintf((char *)g_wifi_uart[wifi_command_port].command_buff, "ATNSOCK=%d,%d\r", 0, 4);
		ret = wifi_execute_at_command(wifi_command_port, g_wifi_uart[wifi_command_port].command_buff, 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_SOCKET_CREATE, 0xff);
		if(ret != 0)
		{
			api_ret = WIFI_ERR_SOCKET_CREATE;
		}
	}

	if(WIFI_SUCCESS == api_ret)
	{
		sprintf((char *)g_wifi_uart[wifi_command_port].command_buff,"ATNCTCP=%d.%d.%d.%d,%d\r",
				WIFI_ULONG_TO_IPV4BYTE_1(ipaddr), WIFI_ULONG_TO_IPV4BYTE_2(ipaddr),
				WIFI_ULONG_TO_IPV4BYTE_3(ipaddr), WIFI_ULONG_TO_IPV4BYTE_4(ipaddr),
				port);
		if(ULPGN_USE_UART_NUM == 2)
		{
			ret = wifi_execute_at_command(wifi_command_port, g_wifi_uart[wifi_command_port].command_buff, 15000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_SOCKET_CONNECT, socket_no);
		}
		else
		{
			ret = wifi_execute_at_command(wifi_command_port, g_wifi_uart[wifi_command_port].command_buff, 15000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_SOCKET_CONNECT, socket_no);
		}
		if(ret != 0)
		{
			wifi_execute_at_command(wifi_command_port, "ATNCLOSE\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_SOCKET_CLOSE, socket_no);
			R_BSP_SoftwareDelay(500, BSP_DELAY_MILLISECS);
			api_ret = WIFI_ERR_MODULE_COM;
		}
	}
	if(WIFI_SUCCESS == api_ret)
	{
		g_wifi_socket[socket_no].socket_status = WIFI_SOCKET_STATUS_CONNECTED;
	}
	if(WIFI_ERR_TAKE_MUTEX != api_ret)
	{
		wifi_give_mutex(mutex_flag);
	}

    return api_ret;

}

int32_t R_WIFI_SX_ULPGN_SocketSend (int32_t socket_no, uint8_t *pdata, int32_t length, uint32_t timeout_ms)
{
	volatile int32_t timeout;
	volatile int32_t sended_length;
    int32_t current_send_length;
    int32_t ret;
    sci_err_t ercd;
	int32_t api_ret = 0;

	if( (pdata == NULL) || (length < 0 ))
	{
		return  WIFI_ERR_PARAMETER;
	}

	if( 0 != R_WIFI_SX_ULPGN_IsConnected())
	{
		return WIFI_ERR_NOT_CONNECT;
	}

	if( (socket_no >= WIFI_CFG_CREATABLE_SOCKETS) || (socket_no < 0) || (0 == g_wifi_socket[socket_no].socket_create_flag) || (g_wifi_socket[socket_no].socket_status != WIFI_SOCKET_STATUS_CONNECTED) )
	{
		return  WIFI_ERR_SOCKET_NUM;
	}

    if(0 == wifi_take_mutex(MUTEX_TX))
	{
		sended_length = 0;

		if(ULPGN_USE_UART_NUM == 2)
        {
        	if(socket_no != current_socket_index)
        	{
        	    if(0 != wifi_take_mutex(MUTEX_RX))
        		{
        	    	wifi_give_mutex(MUTEX_TX);
        	    	return WIFI_ERR_TAKE_MUTEX;
        		}
                ret = wifi_change_socket_index(socket_no);
    	    	wifi_give_mutex(MUTEX_RX);
                if(ret != 0)
                {
                    /* Give back the socketInUse mutex. */
        	    	wifi_give_mutex(MUTEX_TX);
        	    	return WIFI_ERR_CHANGE_SOCKET;
                }
        	}
        }
		if(ULPGN_USE_UART_NUM == 1)
        {
			if(0 == g_wifi_transparent_mode)
			{
        	    if(0 != wifi_take_mutex(MUTEX_RX))
        		{
        	    	wifi_give_mutex(MUTEX_TX);
        	    	return WIFI_ERR_TAKE_MUTEX;
        		}
        	    ret = wifi_change_transparent_mode();
    	    	wifi_give_mutex(MUTEX_RX);
                if(ret != 0)
                {
                    /* Give back the socketInUse mutex. */
        	    	wifi_give_mutex(MUTEX_TX);
        	    	return WIFI_ERR_MODULE_COM;
                }
			}
        }

		if(0 != timeout_ms)
		{
			socket_timeout_init(socket_no, timeout_ms, 0);
		}

        timeout = 0;

        while(sended_length < length)
        {
            if(length - sended_length > g_wifi_tx_busiz_data)
            {
                current_send_length = g_wifi_tx_busiz_data;
            }
            else
            {
                current_send_length = length - sended_length;
            }

            if(current_send_length > WIFI_SOCKET_SENDABLE_DATASIZE)
            {
            	current_send_length = WIFI_SOCKET_SENDABLE_DATASIZE;
            }

            g_wifi_uart[wifi_data_port].tx_end_flag = 0;
            ercd = R_SCI_Send(g_wifi_uart[wifi_data_port].wifi_uart_sci_handle,
            		 (uint8_t *)pdata + sended_length, current_send_length);
            if(SCI_SUCCESS != ercd)
            {
            	break;
            }

            while(1)
            {
                if(0 != g_wifi_uart[wifi_data_port].tx_end_flag)
                {
                    break;
                }
                vTaskDelay(1);
            }

            sended_length += current_send_length;
            if(-1 == socket_check_timeout(socket_no, 0))
            {
            	break;
            }
        }
		api_ret = sended_length;

        /* Give back the socketInUse mutex. */
    	wifi_give_mutex(MUTEX_TX);
    }
    else
    {
    	api_ret = WIFI_ERR_TAKE_MUTEX;
    }
    return api_ret;
}

int32_t R_WIFI_SX_ULPGN_SocketRecv (int32_t socket_no, uint8_t *pdata, int32_t length, uint32_t timeout_ms)
{
    sci_err_t ercd;
    uint32_t recvcnt = 0;
    int32_t ret;
    byteq_err_t byteq_ret;
	int32_t api_ret;
	volatile int32_t timeout;
	uint32_t ipl;

	if( (pdata == NULL) || (length <= 0 ))
	{
		return  WIFI_ERR_PARAMETER;
	}

	if( 0 != R_WIFI_SX_ULPGN_IsConnected())
	{
		return WIFI_ERR_NOT_CONNECT;
	}

	if( (socket_no >= WIFI_CFG_CREATABLE_SOCKETS) || (socket_no < 0) || (0 == g_wifi_socket[socket_no].socket_create_flag) ||  (g_wifi_socket[socket_no].socket_status != WIFI_SOCKET_STATUS_CONNECTED) )
	{
		return  WIFI_ERR_SOCKET_NUM;
	}

    if(0 == wifi_take_mutex(MUTEX_RX))
    {
		if(ULPGN_USE_UART_NUM == 2)
		{
			if(socket_no != current_socket_index)
			{
				wifi_give_mutex(MUTEX_RX);
				if(0 != wifi_take_mutex(MUTEX_TX | MUTEX_RX))
				{
					return -1;
				}
				ret = wifi_change_socket_index(socket_no);
				wifi_give_mutex(MUTEX_TX);
				if(ret != 0)
				{
					/* Give back the socketInUse mutex. */
					wifi_give_mutex(MUTEX_RX);
					return -1;
				}
			}
		}
		if(ULPGN_USE_UART_NUM == 1)
		{
			if(0 == g_wifi_transparent_mode)
			{
				wifi_give_mutex(MUTEX_RX);
				if(0 != wifi_take_mutex(MUTEX_TX | MUTEX_RX))
				{
					return -1;
				}
				ret = wifi_change_transparent_mode();
				wifi_give_mutex(MUTEX_TX);
				if(ret != 0)
				{
					/* Give back the socketInUse mutex. */
					wifi_give_mutex(MUTEX_RX);
					return -1;
				}
			}
		}
		if((timeout_ms != 0) && (timeout_ms != portMAX_DELAY))
		{
			socket_timeout_init(socket_no, timeout_ms, 1);
		}

		while(1)
		{
			ipl = R_BSP_CpuInterruptLevelRead();
			R_BSP_CpuInterruptLevelWrite (WIFI_CFG_SCI_INTERRUPT_LEVEL);
			byteq_ret = R_BYTEQ_Get(g_wifi_socket[current_socket_index].socket_byteq_hdl, (pdata + recvcnt));
			R_BSP_CpuInterruptLevelWrite (ipl);

			if(BYTEQ_SUCCESS == byteq_ret)
			{
				recvcnt++;
				if(recvcnt >= length)
				{
					break;
				}
				continue;
			}
			if(g_wifi_socket[socket_no].socket_status != WIFI_SOCKET_STATUS_CONNECTED)
			{
				break;
			}
			if(g_wifi_system_state != WIFI_SYSTEM_CONNECT)
			{
				break;
			}
			if((timeout_ms != 0) && (timeout_ms != portMAX_DELAY))
			{
				if(-1 == socket_check_timeout(socket_no, 1))
				{
	#if DEBUGLOG == 1
					R_BSP_CpuInterruptLevelWrite (13);
					printf("recv timeout.%d received. requestsize=%d,lastdata=%02x,data1=%02x\r\n",recvcnt,length,*(pdata + (recvcnt-1)),data1);
					R_BSP_CpuInterruptLevelWrite (0);
	#endif
					R_BSP_NOP();
					break;
				}
			}
			vTaskDelay(1);
		}
#if 0
		if(recvcnt == 0)
		{
			ret = wifi_get_socket_status(socket_no);
			if(ret != ULPGN_SOCKET_STATUS_CONNECTED)
			{
				/* Give back the socketInUse mutex. */
				( void ) xSemaphoreGive( g_wifi_rx_semaphore );
				return 0;
			}
			/* socket is not closed, and recieve data size is 0. */
			( void ) xSemaphoreGive( g_wifi_rx_semaphore );
			return 0;
		}
#endif
		/* socket is not closed, and recieve data size is 0. */
		/* Give back the socketInUse mutex. */
		api_ret = recvcnt;
		wifi_give_mutex(MUTEX_RX);
#if DEBUGLOG == 1
		tmptime2 = xTaskGetTickCount();
		R_BSP_CpuInterruptLevelWrite (13);
		printf("r:%06d:tcp %ld byte received.reqsize=%ld,%x\r\n", tmptime2, recvcnt, length, (uint32_t)pdata);
		R_BSP_CpuInterruptLevelWrite (0);
#endif

	}

    return api_ret;
}

wifi_err_t R_WIFI_SX_ULPGN_SocketShutdown (int32_t socket_no)
{
	wifi_err_t api_ret = WIFI_SUCCESS;
	int32_t subroutain_ret;
	uint8_t mutex_flag;

	if((socket_no >= WIFI_CFG_CREATABLE_SOCKETS) || (socket_no < 0) ||
			(0 == g_wifi_socket[socket_no].socket_create_flag) || (g_wifi_socket[socket_no].socket_status <= WIFI_SOCKET_STATUS_SOCKET))
	{
		return WIFI_ERR_SOCKET_NUM;
	}
	if( WIFI_SYSTEM_CLOSE == g_wifi_system_state)
	{
		return WIFI_ERR_NOT_OPEN;
	}
	mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != wifi_take_mutex(mutex_flag))
	{
		api_ret = WIFI_ERR_TAKE_MUTEX;
	}
    if(WIFI_SUCCESS == api_ret)
    {
		if(ULPGN_USE_UART_NUM == 2)
		{
			subroutain_ret = wifi_change_socket_index(socket_no);
			if(subroutain_ret != 0)
			{
				api_ret = WIFI_ERR_CHANGE_SOCKET;
			}
		}
		if(ULPGN_USE_UART_NUM == 1)
		{
			wifi_change_command_mode();
		}
    }
    if(WIFI_SUCCESS == api_ret)
    {
		subroutain_ret = wifi_execute_at_command(wifi_command_port, "ATNCLOSE\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_SOCKET_CLOSE, socket_no);
		if(0 != subroutain_ret)
		{
			api_ret = WIFI_ERR_MODULE_COM;
		}
		else
		{
			g_wifi_socket[socket_no].socket_status = WIFI_SOCKET_STATUS_SOCKET;
			R_BSP_SoftwareDelay(500, BSP_DELAY_MILLISECS);
		}
    }
    if(WIFI_ERR_TAKE_MUTEX != api_ret)
    {
		/* Give back the socketInUse mutex. */
		wifi_give_mutex(mutex_flag);
    }
	return api_ret;

}

wifi_err_t R_WIFI_SX_ULPGN_SocketClose(int32_t socket_no)
{
	wifi_err_t api_ret = WIFI_SUCCESS;

	if((socket_no >= WIFI_CFG_CREATABLE_SOCKETS) || (socket_no < 0))
	{
		return WIFI_ERR_SOCKET_NUM;
	}
	if( WIFI_SYSTEM_CLOSE == g_wifi_system_state)
	{
		return WIFI_ERR_NOT_OPEN;
	}

	if(g_wifi_socket[socket_no].socket_create_flag == 1)
	{
		R_WIFI_SX_ULPGN_SocketShutdown (socket_no);
		R_BYTEQ_Flush(g_wifi_socket[socket_no].socket_byteq_hdl);
		g_wifi_socket[socket_no].ipversion = 0;
		g_wifi_socket[socket_no].protocol = 0;
		g_wifi_socket[socket_no].ssl_flag = 0;
		g_wifi_socket[socket_no].ssl_type = 0;
		g_wifi_socket[socket_no].socket_status = WIFI_SOCKET_STATUS_CLOSED;
		g_wifi_socket[socket_no].socket_create_flag = 0;
    }
    return api_ret;

}

wifi_err_t R_WIFI_SX_ULPGN_DnsQuery (uint8_t *pdomain_name, uint32_t *pipaddress)
{
    int32_t func_ret;
    uint8_t mutex_flag;
	wifi_err_t api_ret = WIFI_SUCCESS;

    if ((NULL == pdomain_name) || (NULL == pipaddress))
    {
    	return WIFI_ERR_PARAMETER;
    }
	if( 0 != R_WIFI_SX_ULPGN_IsConnected())
	{
		return WIFI_ERR_NOT_CONNECT;
	}

    mutex_flag = (MUTEX_TX | MUTEX_RX);
    if(0 != wifi_take_mutex(mutex_flag))
	{
		api_ret = WIFI_ERR_TAKE_MUTEX;
	}
	if(WIFI_SUCCESS == api_ret)
	{
		g_wifi_dnsquery_subcount = 0;
		sprintf((char *)g_wifi_uart[wifi_command_port].command_buff, "ATNDNSQUERY=%s\r", pdomain_name);

		func_ret = wifi_execute_at_command(wifi_command_port, g_wifi_uart[wifi_command_port].command_buff, 3000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_DNSQUERY, 0xff);
		if(func_ret != 0)
		{
			api_ret = WIFI_ERR_MODULE_COM;
		}
		else
		{
			*pipaddress = g_wifi_dnsaddress;
		}
	}
	if(WIFI_ERR_TAKE_MUTEX != api_ret)
	{
		wifi_give_mutex(mutex_flag);
	}

    return api_ret;

}


wifi_err_t R_WIFI_SX_ULPGN_Ping (uint32_t ipaddr, uint16_t count, uint32_t intervalms)
{
	int32_t func_ret;
	uint8_t mutex_flag;
	uint32_t i;
	wifi_err_t api_ret = WIFI_SUCCESS;
	uint32_t success_count;

	if ((0 == ipaddr) || (0 == count))
	{
		return WIFI_ERR_PARAMETER;
	}
	if( 0 != R_WIFI_SX_ULPGN_IsConnected())
	{
		return WIFI_ERR_NOT_CONNECT;
	}

	if(WIFI_SUCCESS == api_ret)
	{
		success_count = 0;
		for(i=0;i<count;i++)
		{
		    mutex_flag = (MUTEX_TX | MUTEX_RX);
		    if(0 != wifi_take_mutex(mutex_flag))
			{
				api_ret = WIFI_ERR_TAKE_MUTEX;
			}
			if(WIFI_SUCCESS == api_ret)
			{
				sprintf((char *)g_wifi_uart[wifi_command_port].command_buff, "ATNPING=%d.%d.%d.%d\r", \
						WIFI_ULONG_TO_IPV4BYTE_1(ipaddr),
						WIFI_ULONG_TO_IPV4BYTE_2(ipaddr),
						WIFI_ULONG_TO_IPV4BYTE_3(ipaddr),
						WIFI_ULONG_TO_IPV4BYTE_4(ipaddr));
				func_ret = wifi_execute_at_command(wifi_command_port, g_wifi_uart[wifi_command_port].command_buff, 20000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_PING, 0xff);
				if(func_ret != 0)
				{
				}
				else
				{
					success_count++;
				}
			}
			if(WIFI_ERR_TAKE_MUTEX != api_ret)
			{
				wifi_give_mutex(mutex_flag);
			}
			if((i+1) < count)
			{
				vTaskDelay(intervalms);
			}
		}
	}
	if(success_count == 0)
	{
		api_ret = WIFI_ERR_MODULE_COM;
	}
	return api_ret;

}

uint32_t R_WIFI_SX_ULPGN_GetVersion(void)
{
    /* These version macros are defined in r_wifi_sx_ulpgn_if.h. */
    return ((((uint32_t)WIFI_SX_ULPGN_VERSION_MAJOR) << 16) | (uint32_t)WIFI_SX_ULPGN_VERSION_MINOR);
}


static int32_t wifi_get_ipaddress(void)
{

    int32_t func_ret;

    func_ret = wifi_execute_at_command(wifi_command_port, "ATNSET=\?\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_GET_IPADDRESS, 0xff);
    if(func_ret != 0)
    {
        return -1;
    }
    return 0;
}


/* return code: from 0 to 5 : socket status
 *              -1 : Error
 * */
static int32_t wifi_get_socket_status(uint8_t socket_no)
{
    int32_t i;
    int32_t ret;

	ret = wifi_execute_at_command(wifi_command_port, "ATNSTAT\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_GET_SOCKET_STATUS, socket_no);
    if(ret != 0)
    {
        return -1;
    }

    if(g_wifi_socket_status < ULPGN_SOCKET_STATUS_MAX)
    {
    	return g_wifi_socket_status;
    }
    else
    {
    	return -1;
    }
}

static int32_t wifi_change_transparent_mode(void)
{
    int32_t i;
    int32_t ret;

    if((ULPGN_USE_UART_NUM == 1) && (g_wifi_transparent_mode == 0) )
    {
		ret = wifi_execute_at_command(wifi_command_port, "ATO\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_TRANSPARENT_MODE, 0xff);
		if(ret != 0)
		{
			return -1;
		}
		else
		{
			g_wifi_transparent_mode = 1;
		}
    }
    return 0;
}

static int32_t wifi_change_command_mode(void)
{
	int32_t ret;
    if((ULPGN_USE_UART_NUM == 1) && (g_wifi_transparent_mode == 1) )
    {
        R_BSP_SoftwareDelay(202, BSP_DELAY_MILLISECS);
        g_wifi_transparent_mode = 0;
    	ret = wifi_execute_at_command(wifi_command_port, "+++", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_COMMAND_MODE, 0xff);
    	if(ret == 0)
    	{
            R_BSP_SoftwareDelay(202, BSP_DELAY_MILLISECS);
    	}
    	else
    	{
    		return -1;
    	}
    }
    return 0;
}


//#if ULPGN_USE_UART_NUM == 2
static int32_t wifi_change_socket_index(uint8_t socket_no)
{
    volatile int32_t timeout;
    int32_t ret = 0;
    uint16_t i;
    uint16_t stored_data_size = 0;
    uint32_t previous_socket_store_data_size = 0;
    uint16_t read_data_size;
    uint8_t before_socket_no;
    uint8_t temprary_buff[64];

    uint32_t previous_atustat_sent,now_atustat_sent,atustat_recv;
    uint8_t sequence;
    uint8_t zero_cnt;
    uint32_t ipl;

    if(ULPGN_USE_UART_NUM == 2)
    {
      	if(socket_no != current_socket_index)
        {
    		sequence = 0;
            before_socket_no = current_socket_index;
            stored_data_size = 0;
            zero_cnt = 0;

            sprintf((char *)g_wifi_uart[wifi_command_port].command_buff, "ATNSOCKINDEX=%d\r", socket_no);

            ret = wifi_check_uart_state(&atustat_recv, &previous_atustat_sent);
            if(ret != 0)
            {
            	return -1;
            }
			R_BSP_SoftwareDelay(ULPGN_CFG_SOCKET_CHANGE_TIMEOUT_VALUE, ULPGN_CFG_SOCKET_CHANGE_TIMEOUT_PERIOD);

            while(sequence < 0x80)
            {
            	switch(sequence)
            	{
            		case 0:
            			/* Read the remaining data three times and proceed to the next step without data. */
                        /* Enable temporary byte que(default byte que). */
                        g_temporary_byteq_enable_flag = 1;
                        ret = wifi_check_uart_state(&atustat_recv, &now_atustat_sent);
                        if(ret != 0)
                        {
							ret = -1;
							sequence = 0x80;
							break;
                        }

                        do{

        					if(SCI_SUCCESS != R_SCI_Control(g_wifi_uart[wifi_data_port].wifi_uart_sci_handle, SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ, &stored_data_size))
        					{
    							ret = -1;
    							sequence = 0x80;
    							break;
        					}
        					if(stored_data_size > 0)
        					{
        						zero_cnt = 0;
        						if(sizeof(temprary_buff) >= stored_data_size)
            					{
            						read_data_size = stored_data_size;
            					}
            					else
            					{
            						read_data_size = sizeof(temprary_buff);
            					}

            					R_SCI_Receive(g_wifi_uart[wifi_data_port].wifi_uart_sci_handle, temprary_buff, read_data_size);

            					for(i = 0; i < read_data_size; i++ )
            					{
            						if(BYTEQ_SUCCESS != R_BYTEQ_Put(g_wifi_socket[current_socket_index].socket_byteq_hdl, *(temprary_buff + i)))
									{
            			                g_wifi_socket[current_socket_index].put_error_count++;
									}
            					}

            					stored_data_size -= read_data_size;
        					}
        					else
        					{
        					}
        				}while(stored_data_size > 0);
                        if(now_atustat_sent == previous_atustat_sent)
                        {
    						zero_cnt++;
                        }
                        else
                        {
    						zero_cnt = 0;
                        }
                        previous_atustat_sent = now_atustat_sent;
            			if(zero_cnt < 2)
            			{
        					R_BSP_SoftwareDelay(ULPGN_CFG_SOCKET_CHANGE_TIMEOUT_VALUE, ULPGN_CFG_SOCKET_CHANGE_TIMEOUT_PERIOD);
            				sequence = 0;
            			}
            			else
            			{
        					R_BSP_SoftwareDelay(ULPGN_CFG_SOCKET_CHANGE_TIMEOUT_VALUE, ULPGN_CFG_SOCKET_CHANGE_TIMEOUT_PERIOD);
        					zero_cnt = 0;
            				sequence = 2;
            			}
            			break;
            		case 2:

                        timeout = 0;
                        g_wifi_uart[wifi_command_port].tx_end_flag = 0;
                        /* Send ATNSOCKINDEX command. */

                        g_sockindex_command_flag =1;
            			ret = wifi_execute_at_command(wifi_command_port, g_wifi_uart[wifi_command_port].command_buff, 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_SET_SOCKET_CHANGE, socket_no);
                    	g_sockindex_command_flag =0;
                        if(-1 == ret)
                        {
							ret = -1;
							sequence = 0x80;
							break;
                        }

                        /* Parse command return code. */
						switch(ret)
						{
							case 0:
								/* change socket index to next. */
								current_socket_index = socket_no;

								ret = wifi_check_uart_state(&atustat_recv, &now_atustat_sent);
								if(ret != 0)
								{
									ret = -1;
									sequence = 0x80;
									break;
								}

								if(stored_data_size > 0)
								{
										previous_socket_store_data_size = now_atustat_sent - previous_atustat_sent;
								        WIFI_RTS_DR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 0;
										/* Copy data(which stored during socket switching) from temporary buff to current socket buff. */
										while(previous_socket_store_data_size > 0)
										{
											if(SCI_SUCCESS != R_SCI_Control(g_wifi_uart[wifi_command_port].wifi_uart_sci_handle,
													SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ, &stored_data_size))
											{
												ret = -1;
												sequence = 0x80;
												break;
											}
											if(stored_data_size > 0)
											{
												if(sizeof(temprary_buff) >= stored_data_size)
												{
													read_data_size = stored_data_size;
												}
												else
												{
													read_data_size = sizeof(temprary_buff);
												}

												if(read_data_size > previous_socket_store_data_size)
												{
													read_data_size = previous_socket_store_data_size;
												}
												if(SCI_SUCCESS != R_SCI_Receive(g_wifi_uart[wifi_data_port].wifi_uart_sci_handle,
														 temprary_buff, read_data_size))
												{
													ret = -1;
													sequence = 0x80;
													break;
												}
												else
												{
													for(i = 0; i < read_data_size; i++ )
													{
														if(BYTEQ_SUCCESS != R_BYTEQ_Put(g_wifi_socket[before_socket_no].socket_byteq_hdl, *(temprary_buff + i)))
														{
															g_wifi_socket[before_socket_no].put_error_count++;
														}
													}
													previous_socket_store_data_size -= read_data_size;
												}
											}
										}
								}
								else
								{
							        WIFI_RTS_DR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 0;
								}
//			        				R_BSP_InterruptsDisable();
								/* Copy data(which stored during socket switching) from temporary buff to next socket buff. */
								do{

									if(SCI_SUCCESS != R_SCI_Control(g_wifi_uart[wifi_data_port].wifi_uart_sci_handle,  SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ, &stored_data_size))
									{
										ret = -1;
										break;
									}
									if(sizeof(temprary_buff) >= stored_data_size)
									{
										read_data_size = stored_data_size;
									}
									else
									{
										read_data_size = sizeof(temprary_buff);
									}

									R_SCI_Receive(g_wifi_uart[wifi_data_port].wifi_uart_sci_handle,
											temprary_buff, read_data_size);

									for(i = 0; i < read_data_size; i++ )
									{
										R_BYTEQ_Put(g_wifi_socket[current_socket_index].socket_byteq_hdl, *(temprary_buff + i));
									}

									stored_data_size -= read_data_size;

								}while(stored_data_size > 0);
//			        				R_BSP_InterruptsEnable();
								/* Disable temporary byte que(default byte que). */
//#endif

								g_temporary_byteq_enable_flag = 0;
								ret = 0;
								sequence = 0x80;
								break;
							case -2:  /* BUSY */
								/* If dont change socket because of wifi module busy, copy data from temporary buff to socket buff
								 * and retry change socket command. */

						        WIFI_RTS_DR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 0;

								/* Wait 100us for module to complete send data. */
								R_BSP_SoftwareDelay(100, BSP_DELAY_MICROSECS);
						        WIFI_RTS_DR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 1;

								/* Copy data(which stored during socket switching) from temporary buff to current socekt buff. */
								do{

									if(SCI_SUCCESS != R_SCI_Control(g_wifi_uart[wifi_data_port].wifi_uart_sci_handle,
											 SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ, &stored_data_size))
									{
										ret = -1;
										sequence = 0x80;
									}
									if(stored_data_size > 0)
									{
										if(sizeof(temprary_buff) >= stored_data_size)
										{
											read_data_size = stored_data_size;
										}
										else
										{
											read_data_size = sizeof(temprary_buff);
										}

										R_SCI_Receive(g_wifi_uart[wifi_data_port].wifi_uart_sci_handle,
												 temprary_buff, read_data_size);

										for(i = 0; i < read_data_size; i++ )
										{
											if(BYTEQ_SUCCESS != R_BYTEQ_Put(g_wifi_socket[current_socket_index].socket_byteq_hdl, *(temprary_buff + i)))
											{
												g_wifi_socket[current_socket_index].put_error_count++;
											}
										}

										stored_data_size -= read_data_size;
									}
								}while(stored_data_size > 0);
								sequence = 0;
								break;
							default:
								/* This is command error and recovery socket status. */
								ret = -1;
								sequence = 0x80;
								break;
                    	}
            			break;
            	}
            }
            
			if(-1 == ret)
			{
				ipl = R_BSP_CpuInterruptLevelRead();
				R_BSP_CpuInterruptLevelWrite (WIFI_CFG_SCI_INTERRUPT_LEVEL);

				/* Copy data(which stored during socket switching) from temporary buff to current socket buff. */
				do{						

					if(SCI_SUCCESS != R_SCI_Control(g_wifi_uart[wifi_data_port].wifi_uart_sci_handle,
							SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ, &stored_data_size))
					{
						break;
					}
					if(stored_data_size > 0)
					{
						if(sizeof(temprary_buff) >= stored_data_size)
						{
							read_data_size = stored_data_size;
						}
						else
						{
							read_data_size = sizeof(temprary_buff);
						}

						R_SCI_Receive(g_wifi_uart[wifi_data_port].wifi_uart_sci_handle,
								temprary_buff, read_data_size);

						for(i = 0; i < read_data_size; i++ )
						{
							if(BYTEQ_SUCCESS != R_BYTEQ_Put(g_wifi_socket[current_socket_index].socket_byteq_hdl, *(temprary_buff + i)))
							{
								g_wifi_socket[current_socket_index].put_error_count++;
							}
						}
						stored_data_size -= read_data_size;
					}
				}while(stored_data_size > 0);
				
				/* Disable temporary byte que(default byte que). */
				g_temporary_byteq_enable_flag = 0;
		        WIFI_RTS_DR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 0;

				R_BSP_CpuInterruptLevelWrite (ipl);
				
			}
            
        }
    
    }
    
    return ret;
}
//#endif

static int32_t wifi_check_uart_state(uint32_t *uart_receive_status, uint32_t *uart_send_status)
{
#if DEBUGLOG == 1
    TickType_t tmptime1, tmptime2;
#endif

    uint8_t retry_count;
    int32_t ret;

	for (retry_count = 0; retry_count < 10; retry_count++)
    {
		ret = wifi_execute_at_command(wifi_command_port, "ATUSTAT\r", 10000, WIFI_RETURN_ENUM_OK, WIFI_COMMAND_GET_SENT_RECV_SIZE, 0xff);
		if(ret == 0)
		{
			*uart_receive_status = g_wifi_atustat_recv;
			*uart_send_status = g_wifi_atustat_sent;
			return 0;
		}
		if(ret == -1)
		{
			return -1;
		}
		if(ret == -2)
		{
			continue;
		}
    }
	return -1;
}





static int32_t wifi_serial_open_for_initial(void)
{
    sci_err_t   my_sci_err;

    R_SCI_PinSet_wifi_serial_default();

    memset(&g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle, 0, sizeof(sci_hdl_t));
	g_wifi_uart[WIFI_UART_COMMAND_PORT].sci_config.async.baud_rate    = WIFI_UART_BAUDRATE_DEFAULT;
	g_wifi_uart[WIFI_UART_COMMAND_PORT].sci_config.async.clk_src      = SCI_CLK_INT;
	g_wifi_uart[WIFI_UART_COMMAND_PORT].sci_config.async.data_size    = SCI_DATA_8BIT;
	g_wifi_uart[WIFI_UART_COMMAND_PORT].sci_config.async.parity_en    = SCI_PARITY_OFF;
	g_wifi_uart[WIFI_UART_COMMAND_PORT].sci_config.async.parity_type  = SCI_EVEN_PARITY;
	g_wifi_uart[WIFI_UART_COMMAND_PORT].sci_config.async.stop_bits    = SCI_STOPBITS_1;
	g_wifi_uart[WIFI_UART_COMMAND_PORT].sci_config.async.int_priority = WIFI_CFG_SCI_INTERRUPT_LEVEL;    // 1=lowest, 15=highest

    my_sci_err = R_SCI_Open(SCI_CH_wifi_serial_default, SCI_MODE_ASYNC, &g_wifi_uart[WIFI_UART_COMMAND_PORT].sci_config, wifi_uart_callback_default_port_for_inititial, &g_wifi_uart[WIFI_UART_COMMAND_PORT].wifi_uart_sci_handle);
    if(SCI_SUCCESS != my_sci_err)
    {
        return -1;
    }

    return 0;

}

static int32_t wifi_serial_open_for_data(void)
{
    sci_err_t   my_sci_err;
    uint8_t level;

    R_SCI_PinSet_wifi_serial_default();

    memset(&g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle, 0, sizeof(sci_hdl_t));
    g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config.async.baud_rate    = WIFI_CFG_SCI_BAUDRATE;
    g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config.async.clk_src      = SCI_CLK_INT;
    g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config.async.data_size    = SCI_DATA_8BIT;
    g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config.async.parity_en    = SCI_PARITY_OFF;
    g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config.async.parity_type  = SCI_EVEN_PARITY;
    g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config.async.stop_bits    = SCI_STOPBITS_1;
    g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config.async.int_priority = WIFI_CFG_SCI_INTERRUPT_LEVEL;    // 1=lowest, 15=highest

    my_sci_err = R_SCI_Open(SCI_CH_wifi_serial_default, SCI_MODE_ASYNC, &g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config, wifi_uart_callback_default_port_for_data, &g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle);

    if(SCI_SUCCESS != my_sci_err)
    {
        return -1;
    }

    level = g_wifi_uart[ULPGN_UART_DEFAULT_PORT].sci_config.async.int_priority -1;
    my_sci_err = R_SCI_Control(g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle, SCI_CMD_SET_TXI_PRIORITY,&level);
    if(SCI_SUCCESS != my_sci_err)
    {
        return -1;
    }

    return 0;

}


static int32_t wifi_serial_second_port_open(void)
{
    sci_err_t   my_sci_err;

    R_SCI_PinSet_wifi_serial_second();

    memset(&g_wifi_uart[ULPGN_UART_SECOND_PORT].wifi_uart_sci_handle, 0, sizeof(sci_hdl_t));
    g_wifi_uart[ULPGN_UART_SECOND_PORT].sci_config.async.baud_rate    = 115200;
    g_wifi_uart[ULPGN_UART_SECOND_PORT].sci_config.async.clk_src      = SCI_CLK_INT;
    g_wifi_uart[ULPGN_UART_SECOND_PORT].sci_config.async.data_size    = SCI_DATA_8BIT;
    g_wifi_uart[ULPGN_UART_SECOND_PORT].sci_config.async.parity_en    = SCI_PARITY_OFF;
    g_wifi_uart[ULPGN_UART_SECOND_PORT].sci_config.async.parity_type  = SCI_EVEN_PARITY;
    g_wifi_uart[ULPGN_UART_SECOND_PORT].sci_config.async.stop_bits    = SCI_STOPBITS_1;
    g_wifi_uart[ULPGN_UART_SECOND_PORT].sci_config.async.int_priority = WIFI_CFG_SCI_INTERRUPT_LEVEL;    // 1=lowest, 15=highest

    my_sci_err = R_SCI_Open(SCI_CH_wifi_serial_second, SCI_MODE_ASYNC, &g_wifi_uart[ULPGN_UART_SECOND_PORT].sci_config, wifi_uart_callback_second_port_for_command, &g_wifi_uart[ULPGN_UART_SECOND_PORT].wifi_uart_sci_handle);

    if(SCI_SUCCESS != my_sci_err)
    {
        return -1;
    }

    return 0;
}

static int32_t wifi_serial_close(void)
{
	wifi_serial_default_port_close();
    if(ULPGN_USE_UART_NUM == 2)
    {
    	wifi_serial_second_port_close();
    }
    return 0;
}



static int32_t wifi_serial_default_port_close(void)
{
    R_SCI_Control(g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle,  SCI_CMD_RX_Q_FLUSH, NULL);
    R_SCI_Control(g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle, SCI_CMD_TX_Q_FLUSH, NULL);
    R_SCI_Close(g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle);
    return 0;
}

static int32_t wifi_serial_second_port_close(void)
{
    R_SCI_Control(g_wifi_uart[ULPGN_UART_SECOND_PORT].wifi_uart_sci_handle, SCI_CMD_RX_Q_FLUSH, NULL);
    R_SCI_Control(g_wifi_uart[ULPGN_UART_SECOND_PORT].wifi_uart_sci_handle, SCI_CMD_TX_Q_FLUSH, NULL);
    R_SCI_Close(g_wifi_uart[ULPGN_UART_SECOND_PORT].wifi_uart_sci_handle);
    return 0;
}

static void wifi_uart_callback_default_port_for_inititial(void *pArgs)
{
    sci_cb_args_t   *p_args;

    p_args = (sci_cb_args_t *)pArgs;
    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        /* From RXI interrupt; received character data is in p_args->byte */
        R_BSP_NOP();
    }
#if SCI_CFG_TEI_INCLUDED
    else if (SCI_EVT_TEI == p_args->event)
    {
    	g_wifi_uart[ULPGN_UART_DEFAULT_PORT].tx_end_flag = 1;
    }
#endif
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
           You will need to increase buffer size or reduce baud rate */
    	g_wifi_sci_err_flag = 1;
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	g_wifi_sci_err_flag = 2;
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	g_wifi_sci_err_flag = 3;
    }
    else if (SCI_EVT_PARITY_ERR == p_args->event)
    {
        /* From receiver parity error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    }
    else
    {
        /* Do nothing */
    }

} /* End of function my_sci_callback() */



static void wifi_uart_callback_second_port_for_command(void *pArgs)
{
    sci_cb_args_t   *p_args;

    p_args = (sci_cb_args_t *)pArgs;
    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        /* From RXI interrupt; received character data is in p_args->byte */
        R_BSP_NOP();
    }
#if SCI_CFG_TEI_INCLUDED
    else if (SCI_EVT_TEI == p_args->event)
    {
    	g_wifi_uart[ULPGN_UART_SECOND_PORT].tx_end_flag = 1;
        if(g_sockindex_command_flag == 1)
        {
	        WIFI_RTS_DR(WIFI_CFG_RTS_PORT, WIFI_CFG_RTS_PIN) = 1;
        }
    }
#endif
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
           You will need to increase buffer size or reduce baud rate */
    	g_wifi_sci_err_flag = 1;
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	g_wifi_sci_err_flag = 2;
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	g_wifi_sci_err_flag = 3;
    }
    else if (SCI_EVT_PARITY_ERR == p_args->event)
    {
        /* From receiver parity error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    }
    else
    {
        /* Do nothing */
    }

} /* End of function my_sci_callback() */
static void wifi_uart_callback_default_port_for_data(void *pArgs)
{
    sci_cb_args_t   *p_args;
    sci_err_t ret;
    byteq_err_t byteq_ret;
    uint8_t data;

    p_args = (sci_cb_args_t *)pArgs;

    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        if((0 == g_temporary_byteq_enable_flag)&&(g_wifi_socket[current_socket_index].socket_create_flag == 1))
        {
            /* From RXI interrupt; received character data is in p_args->byte */
            R_SCI_Control(g_wifi_uart[ULPGN_UART_DEFAULT_PORT].wifi_uart_sci_handle, SCI_CMD_RX_Q_FLUSH, NULL);
            byteq_ret = R_BYTEQ_Put(g_wifi_socket[current_socket_index].socket_byteq_hdl, p_args->byte);
            if (BYTEQ_SUCCESS != byteq_ret)
            {
            	g_wifi_socket[current_socket_index].put_error_count++;
                return;
            }
            else
            {
            }
        }
        else
        {
//        	g_wifi_socket[current_socket_index].socket_not_create_recv_count= p_args->byte;
        }
    }
#if SCI_CFG_TEI_INCLUDED
    else if (SCI_EVT_TEI == p_args->event)
    {
    	g_wifi_uart[ULPGN_UART_DEFAULT_PORT].tx_end_flag = 1;
    }
#endif
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
           You will need to increase buffer size or reduce baud rate */
    	g_wifi_sci_err_flag = 1;
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	g_wifi_sci_err_flag = 2;
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	g_wifi_sci_err_flag = 3;
    }
    else if (SCI_EVT_PARITY_ERR == p_args->event)
    {
        /* From receiver parity error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    }
    else
    {
        /* Do nothing */
    }

} /* End of function my_sci_callback() */


/*--------*/
static int32_t wifi_execute_at_command(uint8_t serial_ch_id, uint8_t *ptextstring, uint16_t timeout_ms, wifi_return_code_t expect_code,  wifi_command_list_t command, int32_t socket_no)
{
	volatile int32_t timeout;
	sci_err_t ercd;
	int8_t  get_queue;
	uint32_t recvcnt = 0;
	int32_t ret;
	wifi_return_code_t result;
	uint32_t ticket_no;

	timeout_init(serial_ch_id, timeout_ms);

	if(ptextstring != NULL)
	{
		timeout = 0;
		recvcnt = 0;
#if DEBUGLOG == 2
		R_BSP_CpuInterruptLevelWrite (WIFI_CFG_SCI_INTERRUPT_LEVEL-1);
		printf("%s",ptextstring);
		R_BSP_CpuInterruptLevelWrite (0);
#endif

		ticket_no = wifi_set_request_in_queue( command, socket_no );
		g_wifi_uart[serial_ch_id].tx_end_flag = 0;
		ercd = R_SCI_Send(g_wifi_uart[serial_ch_id].wifi_uart_sci_handle, ptextstring, strlen((const char *)ptextstring));
		if(SCI_SUCCESS != ercd)
		{
			return -1;
		}

		while(1)
		{
			if(0 != g_wifi_uart[serial_ch_id].tx_end_flag)
			{
				break;
			}
			if(-1 == check_timeout(serial_ch_id, recvcnt))
			{
				wifi_set_result_to_current_running_queue( WIFI_RETURN_ENUM_INTERNAL_TIMEOUT );
				timeout = 1;
				break;
			}
			vTaskDelay(1);
		}
		if(timeout == 1)
		{
			return -1;
		}
	}
	else
	{
		ticket_no = wifi_set_request_in_queue( command, socket_no );
	}
	while(1)
	{
		get_queue = wifi_get_result_from_queue( ticket_no, &result );
		if(0 == get_queue )
		{
			break;
		}

		if(-1 == check_timeout(serial_ch_id, recvcnt))
		{
			wifi_set_result_to_current_running_queue( WIFI_RETURN_ENUM_INTERNAL_TIMEOUT );
			timeout = 1;
			break;
		}
		vTaskDelay(1);
	}
	if(timeout == 1)
	{
		return -1;
	}

	ret = -1;
	if(result == expect_code)
	{
		ret = 0;
	}
	else if(result == WIFI_RETURN_ENUM_BUSY)
	{
		ret = -2;
	}
	return ret;
}


static void timeout_init(int32_t serial_ch, uint16_t timeout_ms)
{
	wifi_at_communication_info_t *puart;
	puart = &g_wifi_uart[serial_ch];

	puart->starttime = xTaskGetTickCount();
	puart->endtime = puart->starttime + timeout_ms;
	if(puart->endtime < puart->starttime)
	{
		/* endtime value is overflow */
		puart->timeout_overflow_flag = 1;
	}
	else
	{
		puart->timeout_overflow_flag = 0;
	}
}

static int32_t check_timeout(int32_t serial_ch, int32_t rcvcount)
{
	wifi_at_communication_info_t *puart;
	puart = &g_wifi_uart[serial_ch];

	if(0 == rcvcount)
	{
		puart->thistime = xTaskGetTickCount();
		if(puart->timeout_overflow_flag == 0)
		{
			if(puart->thistime >= puart->endtime || puart->thistime < puart->starttime)
			{
				/* Timeout  */
				return -1;
			}
		}
		else
		{
			if(puart->thistime < puart->starttime && puart->thistime >= puart->endtime)
			{
				/* Timeout  */
				return -1;
			}
		}
	}
	/* Not timeout  */
	return 0;
}

static void bytetimeout_init(int32_t serial_ch, uint16_t timeout_ms)
{
	wifi_at_communication_info_t *puart;
	puart = &g_wifi_uart[serial_ch];

	puart->startbytetime = xTaskGetTickCount();
	puart->endbytetime = puart->startbytetime + timeout_ms;
	if(puart->endbytetime < puart->startbytetime)
	{
		/* overflow */
		puart->byte_timeout_overflow_flag = 1;
	}
	else
	{
		puart->byte_timeout_overflow_flag = 0;
	}
}

static int32_t check_bytetimeout(int32_t serial_ch, int32_t rcvcount)
{
	wifi_at_communication_info_t *puart;
	puart = &g_wifi_uart[serial_ch];

	if(0 != rcvcount)
	{
		puart->thisbytetime = xTaskGetTickCount();
		if(puart->byte_timeout_overflow_flag == 0)
		{
			if(puart->thisbytetime >= puart->endbytetime || puart->thisbytetime < puart->startbytetime)
			{
				return -1;
			}
		}
		else
		{
			if(puart->thisbytetime < puart->startbytetime && puart->thisbytetime >= puart->endbytetime)
			{
				/* Timeout  */
				return -1;
			}
		}
	}
	/* Not timeout  */
	return 0;
}

static void socket_timeout_init(uint8_t socket_no, uint32_t timeout_ms, uint8_t flag)
{
	TickType_t *starttime;
	TickType_t *thistime;
	TickType_t *endtime;
	uint8_t    *timeout_overflow_flag;

	wifi_socket_t *psocket;
	psocket = &g_wifi_socket[socket_no];

	if(0 == flag)
	{
		starttime             = &psocket->send_starttime ;
		thistime              = &psocket->send_thistime ;
		endtime               = &psocket->send_endtime ;
		timeout_overflow_flag = &psocket->send_timeout_overflow_flag;
	}
	else
	{
		starttime             = &psocket->recv_starttime ;
		thistime              = &psocket->recv_thistime ;
		endtime               = &psocket->recv_endtime ;
		timeout_overflow_flag = &psocket->recv_timeout_overflow_flag;
	}
    *starttime = xTaskGetTickCount();
    *endtime = *starttime + timeout_ms;
    if(*endtime < *starttime)
    {
        /* overflow */
        *timeout_overflow_flag = 1;
    }
    else
    {
        *timeout_overflow_flag = 0;
    }

}

static int32_t socket_check_timeout(uint8_t socket_no, uint8_t flag)
{
	TickType_t *starttime;
	TickType_t *thistime;
	TickType_t *endtime;
	uint8_t    *timeout_overflow_flag;
	wifi_socket_t *psocket;

	psocket = &g_wifi_socket[socket_no];
	if(0 == flag)
	{
		starttime             = &psocket->send_starttime ;
		thistime              = &psocket->send_thistime ;
		endtime               = &psocket->send_endtime ;
		timeout_overflow_flag = &psocket->send_timeout_overflow_flag;
	}
	else
	{
		starttime             = &psocket->recv_starttime ;
		thistime              = &psocket->recv_thistime ;
		endtime               = &psocket->recv_endtime ;
		timeout_overflow_flag = &psocket->recv_timeout_overflow_flag;
	}

	*thistime = xTaskGetTickCount();
    if(*timeout_overflow_flag == 0)
    {
        if(*thistime >= *endtime || *thistime < *starttime)
        {
            return -1;
        }
    }
    else
    {
        if(*thistime < *starttime && *thistime <= *endtime)
        {
            /* Not timeout  */
            return -1;
        }
    }
    /* Not timeout  */
    return 0;
}

static int32_t wifi_take_mutex(uint8_t mutex_flag)
{
	if(0 != (mutex_flag & MUTEX_TX))
	{
		if( xSemaphoreTake( g_wifi_tx_semaphore, xMaxSemaphoreBlockTime ) != pdTRUE )
		{
        	return -1;
		}
	}

	if(0 != (mutex_flag & MUTEX_RX))
	{
		if( xSemaphoreTake( g_wifi_rx_semaphore, xMaxSemaphoreBlockTime ) != pdTRUE )
		{
			if(0 != (mutex_flag & MUTEX_TX))
			{
				xSemaphoreGive( g_wifi_tx_semaphore );
			}
			return -1;
		}
	}
	return 0;
}

static void wifi_give_mutex(uint8_t mutex_flag)
{
	if(0 != (mutex_flag & MUTEX_RX))
	{
		xSemaphoreGive( g_wifi_rx_semaphore);
	}
	if(0 != (mutex_flag & MUTEX_TX))
	{
		xSemaphoreGive( g_wifi_tx_semaphore);
	}
	return;
}
