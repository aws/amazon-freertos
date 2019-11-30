#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "platform.h"
#include "r_sci_rx_if.h"
#include "r_byteq_if.h"
#include "r_wifi_sx_ulpgn_if.h"
#include "r_wifi_sx_ulpgn_private.h"

#if(1 == WIFI_CFG_USE_CALLBACK_FUNCTION)
void WIFI_CFG_CALLBACK_FUNCTION_NAME(void *p_args);
void (* const p_wifi_callback)(void *p_args) = WIFI_CFG_CALLBACK_FUNCTION_NAME;
#else
void (* const p_wifi_callback)(void *p_args) = NULL;
#endif


uint32_t g_wifi_response_recv_count;
uint32_t tmp_recvcnt;
uint32_t g_wifi_response_recv_status;

char g_wifi_response_last_string[15];
uint8_t g_wifi_response_last_string_recv_count;

uint8_t g_wifi_macaddress[6];
wifi_ip_configuration_t g_wifi_ipconfig;
uint32_t g_wifi_dnsaddress;
uint32_t g_wifi_dnsquery_subcount;

uint32_t g_wifi_atustat_recv;
uint32_t g_wifi_atustat_sent;

wifi_scan_result_t *gp_wifi_ap_results;
uint32_t g_wifi_aplistmax;
uint32_t g_wifi_aplist_stored_num;
uint32_t g_wifi_aplist_count;
uint32_t g_wifi_aplist_subcount;

uint8_t g_wifi_current_ssid[33];

uint8_t g_wifi_socket_status;

wifi_socket_t g_wifi_socket[WIFI_CFG_CREATABLE_SOCKETS];



wifi_at_execute_queue_t g_wifi_at_execute_queue[10];
uint8_t g_wifi_set_queue_index;
uint8_t g_wifi_get_queue_index;

static void wifi_recv_task( void * pvParameters );
TaskHandle_t g_wifi_recv_task_handle;

static void wifi_analyze_ipaddress_string(uint8_t *pstring);
static void wifi_analyze_get_macaddress_string(uint8_t *pstring);
static void wifi_analyze_get_aplist_string(uint8_t *pstring);
static void wifi_analyze_get_dnsquery_string(uint8_t *pstring);
static void wifi_analyze_get_sent_recv_size_string(uint8_t *pstring);
static void wifi_analyze_get_current_ssid_string(uint8_t *pstring);
void wifi_analyze_get_socket_status_string(uint8_t *pstring);
int32_t wifi_start_recv_task( void )
{
	int32_t ret = -1;
	BaseType_t xReturned;

    /* Create wifi driver at response tasks. */
	xReturned = xTaskCreate( wifi_recv_task,                               /* The function that implements the task. */
                 "sx_ulpgn_recv",                                     /* Just a text name for the task to aid debugging. */
                 1024,
                 ( void * ) 0,                                    /* The task parameter, not used in this case. */
				 tskIDLE_PRIORITY + 6,
				 &g_wifi_recv_task_handle );                                          /* The task handle is not used. */
	if(pdPASS == xReturned)
	{
		ret = 0;
	}
	return ret;
}

void wifi_delete_recv_task( void )
{
    /* Delete wifi driver at response tasks. */
    if( NULL != g_wifi_recv_task_handle )
    {
    	vTaskDelete(g_wifi_recv_task_handle);
    	g_wifi_recv_task_handle = NULL;
    }
}

static void wifi_recv_task( void * pvParameters )
{
	uint8_t data;
	uint8_t *presponse_buff;
	uint8_t *pwkstr1;
	sci_err_t sci_ercd;
	int sscanf_ret;
	int i;
	uint32_t socket_no;
	uint32_t len;
	byteq_err_t byteq_ercd;
	uint32_t tmp_recvcnt;
	uint32_t psw_value;
	wifi_at_execute_queue_t * pqueue;
	wifi_err_event_t event;
	uint8_t command_port;
	g_wifi_response_recv_count = 0;
	g_wifi_response_last_string_recv_count = 0;
	memset(g_wifi_response_last_string, 0,sizeof(g_wifi_response_last_string));


    for( ; ; )
    {
    	command_port = wifi_command_port;
    	presponse_buff = g_wifi_uart[command_port].response_buff;
    	sci_ercd = R_SCI_Receive(g_wifi_uart[command_port].wifi_uart_sci_handle, &data, 1);
    	if(SCI_ERR_INSUFFICIENT_DATA == sci_ercd)
    	{
    		/* Pause for a short while to ensure the network is not too
         * congested. */
    		vTaskDelay( 1 );

    	}
    	else if(SCI_SUCCESS == sci_ercd)
    	{
#if DEBUGLOG == 2
			R_BSP_CpuInterruptLevelWrite (WIFI_CFG_SCI_INTERRUPT_LEVEL-1);
    		putchar(data);
			R_BSP_CpuInterruptLevelWrite (0);
#endif
			if(1 == g_wifi_transparent_mode)
			{
    			psw_value = R_BSP_CpuInterruptLevelRead();
				R_BSP_CpuInterruptLevelWrite (WIFI_CFG_SCI_INTERRUPT_LEVEL-1);
    			byteq_ercd = R_BYTEQ_Put(g_wifi_socket[0].socket_byteq_hdl, data);
				R_BSP_CpuInterruptLevelWrite (psw_value);
    			if(byteq_ercd != BYTEQ_SUCCESS)
    			{
    				g_wifi_socket[0].put_error_count++;
    			}
			}
			else
			{
				if(g_wifi_response_recv_count >= g_wifi_uart[WIFI_UART_COMMAND_PORT].response_buff_size -1 )
				{
					event.event = WIFI_EVENT_RCV_TASK_RXB_OVF_ERR;
					if(NULL != p_wifi_callback)
					{
						p_wifi_callback(&event);
					}
				}
				else
				{
					presponse_buff[g_wifi_response_recv_count] = data;
				}
				g_wifi_response_recv_count++;
			}
			if(g_wifi_response_last_string_recv_count >= (sizeof(g_wifi_response_last_string) -1))
			{
				memmove(&g_wifi_response_last_string[0], &g_wifi_response_last_string[1], (sizeof(g_wifi_response_last_string) -1));
				g_wifi_response_last_string[sizeof(g_wifi_response_last_string) -2] = data;
				g_wifi_response_last_string[sizeof(g_wifi_response_last_string) -1] = '\0';
			}
			else
			{
				g_wifi_response_last_string[g_wifi_response_last_string_recv_count] = data;
				g_wifi_response_last_string[g_wifi_response_last_string_recv_count+1] = '\0';
				g_wifi_response_last_string_recv_count++;

			}
    		if('\n' == data)
    		{
				pqueue = wifi_get_current_running_queue();

				if(pqueue->at_command_id == WIFI_COMMAND_GET_IPADDRESS)
				{
					if(0 == strncmp(presponse_buff,"IP:",3))
					{
						wifi_analyze_ipaddress_string(presponse_buff);
					}
				}
				if(pqueue->at_command_id == WIFI_COMMAND_GET_MACADDRESS)
				{
					if(0 == strncmp((char *)presponse_buff,"Mac Addr",8))
					{
						wifi_analyze_get_macaddress_string(presponse_buff);
					}
				}
				if(pqueue->at_command_id == WIFI_COMMAND_SET_DNSQUERY)
				{
					wifi_analyze_get_dnsquery_string(presponse_buff);
				}
				if(pqueue->at_command_id == WIFI_COMMAND_GET_APLIST)
				{
					wifi_analyze_get_aplist_string(presponse_buff);
				}
				if(pqueue->at_command_id == WIFI_COMMAND_GET_SENT_RECV_SIZE)
				{
					wifi_analyze_get_sent_recv_size_string(presponse_buff);
				}
				if(pqueue->at_command_id == WIFI_COMMAND_GET_CURRENT_SSID)
				{
					wifi_analyze_get_current_ssid_string(presponse_buff);
				}
				if(pqueue->at_command_id == WIFI_COMMAND_GET_SOCKET_STATUS)
				{
					wifi_analyze_get_socket_status_string(presponse_buff);
				}
				else if(0 == strcmp((const char *)presponse_buff,WIFI_RETURN_TEXT_ERROR))
				{
					wifi_set_result_to_current_running_queue( WIFI_RETURN_ENUM_ERROR );
				}
				else if(0 == strcmp((const char *)presponse_buff,WIFI_RETURN_TEXT_BUSY))
				{
					wifi_set_result_to_current_running_queue( WIFI_RETURN_ENUM_BUSY );
				}
				else if(0 == strcmp((const char *)presponse_buff,WIFI_RETURN_TEXT_NOCARRIER))
				{
					wifi_set_result_to_current_running_queue( WIFI_RETURN_ENUM_NOCARRIER );
				}
				else if(0 == strcmp((const char *)presponse_buff,WIFI_RETURN_TEXT_NOANSWER))
				{
					wifi_set_result_to_current_running_queue( WIFI_RETURN_ENUM_ERROR );
				}
				else if(0 == strcmp((const char *)presponse_buff,WIFI_RETURN_TEXT_OK))
				{
					/* 通常のOK　*/
					wifi_set_result_to_current_running_queue( WIFI_RETURN_ENUM_OK );
				}
				else
				{
				}
				memset(presponse_buff, 0, g_wifi_uart[command_port].response_buff_size);
				g_wifi_response_recv_count = 0;
				g_wifi_response_last_string_recv_count = 0;
				g_wifi_response_recv_status = 0x0000;
    		}
    	}
        if(0 != g_wifi_sci_err_flag)
        {
        	switch(g_wifi_sci_err_flag)
        	{
    			case 1:
    				event.event = WIFI_EVENT_SERIAL_RXQ_OVF_ERR;
    				break;
    			case 2:
    				event.event = WIFI_EVENT_SERIAL_OVF_ERR;
    				break;
    			case 3:
    				event.event = WIFI_EVENT_SERIAL_FLM_ERR;
    				break;
        	}
        	g_wifi_sci_err_flag = 0;
    		if(NULL != p_wifi_callback)
    		{
    			p_wifi_callback(&event);
    		}
        }
        for(i = 0;i<g_wifi_cleateble_sockets;i++)
        {
        	if(g_wifi_socket[i].put_error_count > 0)
        	{
        		g_wifi_socket[i].put_error_count = 0;
    			event.event = WIFI_EVENT_SOCKET_RXQ_OVF_ERR;
    			event.socket_no = i;
    			if(NULL != p_wifi_callback)
    			{
    				p_wifi_callback(&event);
    			}
        	}

        }
    }
}

static void wifi_analyze_ipaddress_string(uint8_t *pstring)
{
	int scanf_ret;
	uint32_t ipaddr[4];
	uint32_t subnetmask[4];
	uint32_t gateway[4];
    scanf_ret = sscanf((const char *)pstring, "IP:%d.%d.%d.%d, Mask:%d.%d.%d.%d, Gateway:%d.%d.%d.%d\r\n", \
                       &ipaddr[0], &ipaddr[1], &ipaddr[2], &ipaddr[3], \
                       &subnetmask[0], &subnetmask[1], &subnetmask[2], &subnetmask[3], \
                       &gateway[0], &gateway[1], &gateway[2], &gateway[3]);
    if(scanf_ret == 12)
    {
        g_wifi_ipconfig.ipaddress = WIFI_IPV4BYTE_TO_ULONG(ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
        g_wifi_ipconfig.subnetmask = WIFI_IPV4BYTE_TO_ULONG(subnetmask[0], subnetmask[1], subnetmask[2], subnetmask[3]);
        g_wifi_ipconfig.gateway = WIFI_IPV4BYTE_TO_ULONG(gateway[0], gateway[1], gateway[2], gateway[3]);
    }
}

static void wifi_analyze_get_macaddress_string(uint8_t *pstring)
{
	int scanf_ret;
	uint32_t macaddr[6];

	scanf_ret = sscanf((const char *)pstring, "Mac Addr     =   %2x:%2x:%2x:%2x:%2x:%2x\r\n",\
			&macaddr[0], &macaddr[1], &macaddr[2], &macaddr[3], &macaddr[4], &macaddr[5]);

	if (scanf_ret == 6)
	{
		g_wifi_macaddress[0] = macaddr[0];
		g_wifi_macaddress[1] = macaddr[1];
		g_wifi_macaddress[2] = macaddr[2];
		g_wifi_macaddress[3] = macaddr[3];
		g_wifi_macaddress[4] = macaddr[4];
		g_wifi_macaddress[5] = macaddr[5];
	}
}

static void wifi_analyze_get_aplist_string(uint8_t *pstring)
{
	int scanf_ret;
	uint32_t bssid[6];
	uint32_t channel;
	int32_t indicator;
	char *pstr1;
	char *pstr2;

	if(0 == strncmp((char *)pstring, "ssid = ", 7))
	{
		pstr1 =pstring+7;
		pstr2 = strchr((const char *)pstr1,'\r');
		if(NULL != pstr2)
		{
			*pstr2 = '\0';
			if(strlen(pstr2) <= 32)
			{
				if(g_wifi_aplist_subcount >= 1)
				{
					g_wifi_aplist_stored_num++;
					gp_wifi_ap_results++;
				}
				if(g_wifi_aplist_stored_num < g_wifi_aplistmax)
				{
					strcpy(gp_wifi_ap_results->ssid, pstr1);
				}
				g_wifi_aplist_subcount = 1;
			}
		}
	}
	else if(0 == strncmp((char *)pstring, "bssid =", 7))
	{
		scanf_ret = sscanf(pstring, "bssid = %2x:%2x:%2x:%2x:%2x:%2x\r\n",\
				&bssid[0], &bssid[1], &bssid[2], &bssid[3], &bssid[4], &bssid[5]);
		if(scanf_ret == 6)
		{
			if(g_wifi_aplist_subcount >= 2)
			{
				g_wifi_aplist_stored_num++;
				gp_wifi_ap_results++;
			}
			if(g_wifi_aplist_stored_num < g_wifi_aplistmax)
			{
				gp_wifi_ap_results->bssid[0] = bssid[0];
				gp_wifi_ap_results->bssid[1] = bssid[1];
				gp_wifi_ap_results->bssid[2] = bssid[2];
				gp_wifi_ap_results->bssid[3] = bssid[3];
				gp_wifi_ap_results->bssid[4] = bssid[4];
				gp_wifi_ap_results->bssid[5] = bssid[5];
			}
			g_wifi_aplist_subcount = 2;

		}
	}
	else if(0 == strncmp((char *)pstring, "channel =", 9))
	{
		scanf_ret = sscanf((const char *)pstring, "channel = %d\r\n",\
				&channel);
		if(scanf_ret == 1)
		{
			if(g_wifi_aplist_subcount >= 3)
			{
				g_wifi_aplist_stored_num++;
				gp_wifi_ap_results++;
			}
			if(g_wifi_aplist_stored_num < g_wifi_aplistmax)
			{
				gp_wifi_ap_results->channel = channel;
			}
			g_wifi_aplist_subcount = 3;
		}
	}
	else if(0 == strncmp((char *)pstring, "indicator =", 11))
	{
		scanf_ret = sscanf((char *)pstring, "indicator = %d\r\n",\
				&indicator);
		if(scanf_ret == 1)
		{
			if(g_wifi_aplist_subcount >= 4)
			{
				g_wifi_aplist_stored_num++;
				gp_wifi_ap_results++;
			}
			if(g_wifi_aplist_stored_num < g_wifi_aplistmax)
			{
				gp_wifi_ap_results->rssi = indicator;
			}
			g_wifi_aplist_subcount = 4;
		}
	}
	else if (0 == strncmp((char *)pstring, "security = NONE!", 16))
	{
		if(g_wifi_aplist_subcount >= 5)
		{
			g_wifi_aplist_stored_num++;
			gp_wifi_ap_results++;
		}
		if(g_wifi_aplist_stored_num < g_wifi_aplistmax)
		{
			gp_wifi_ap_results->security = WIFI_SECURITY_OPEN;
		}
		g_wifi_aplist_subcount = 5;
		g_wifi_aplist_count++;
	}
	else if (0 == strncmp((char *)pstring, "WPA", 3))
	{
		if(g_wifi_aplist_subcount >= 5)
		{
			g_wifi_aplist_stored_num++;
			gp_wifi_ap_results++;
		}
		if(g_wifi_aplist_stored_num < g_wifi_aplistmax)
		{
			gp_wifi_ap_results->security = WIFI_SECURITY_WPA;
		}
		g_wifi_aplist_subcount = 5;
		g_wifi_aplist_count++;
	}
	else if (0 == strncmp((char *)pstring, "RSN/WPA2", 8))
	{
		if(g_wifi_aplist_subcount >= 5)
		{
			g_wifi_aplist_stored_num++;
			gp_wifi_ap_results++;
		}
		if(g_wifi_aplist_stored_num < g_wifi_aplistmax)
		{
			gp_wifi_ap_results->security = WIFI_SECURITY_WPA2;
		}
		g_wifi_aplist_subcount = 5;
		g_wifi_aplist_count++;
	}
}

static void wifi_analyze_get_dnsquery_string(uint8_t *pstring)
{
	uint32_t ipaddr[4];
	if(0 == strncmp(pstring,"1\r\n",3))
	{
		g_wifi_dnsquery_subcount = 1;
	}
	else if(4 == sscanf(pstring,"%d.%d.%d.%d\r\n",&ipaddr[0],&ipaddr[1],&ipaddr[2],&ipaddr[3]))
	{
		if(1 == g_wifi_dnsquery_subcount)
		{
			g_wifi_dnsaddress = WIFI_IPV4BYTE_TO_ULONG(ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
			g_wifi_dnsquery_subcount = 2;
		}
	}
}
static void wifi_analyze_get_sent_recv_size_string(uint8_t *pstring)
{
	uint32_t recv;
	uint32_t sent;
	if(2 == sscanf(pstring,"recv=%lu sent=%lu\r\n",&recv,&sent))
	{
		g_wifi_atustat_recv = recv;
		g_wifi_atustat_sent = sent;
	}
}

static void wifi_analyze_get_current_ssid_string(uint8_t *pstring)
{
	char tag[] = "ssid         =   ";
	char *ptr1;
	char *ptr2;
	if(0 == strncmp((char *)pstring, tag, strlen(tag)))
	{
		ptr1 = (char *)pstring + strlen(tag);
		ptr2 = strchr(ptr1,'\r');
		if(NULL == ptr2)
		{
			ptr2 = '\0';
			if(strlen(ptr2) < 32)
			{
				strcpy((char *)g_wifi_current_ssid, ptr2);
			}
		}
	}
}

void wifi_analyze_get_socket_status_string(uint8_t *pstring)
{
	char * str_ptr;
	int i;

	/* focus 1st line "CLOSED", "SOCKET", "BOUND", "LISTEN" or "CONNECTED".*/
	if( ('A' <= pstring[0]) && (pstring[0] <= 'Z'))
	{
		str_ptr = strchr((const char *)pstring, ',');
	    if(str_ptr != NULL)
	    {
	    	str_ptr = "\0";
	        for(i = 0; i < ULPGN_SOCKET_STATUS_MAX; i++)
	        {
	            if(0 == strcmp((const char *)str_ptr, (const char *)wifi_socket_status_tbl[i]))
	            {
	                break;
	            }
	        }
		    if(i < ULPGN_SOCKET_STATUS_MAX)
		    {
		    	g_wifi_socket_status = i;
		    }
	    }
	}
}
