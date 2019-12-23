#include <stdio.h>
#include <string.h>

#include "platform.h"
#include "r_wifi_sx_ulpgn_if.h"
#include "r_wifi_sx_ulpgn_private.h"


uint32_t g_wifi_queue_ticket_no;

void wifi_init_at_execute_queue(void)
{
	memset(g_wifi_at_execute_queue, 0, sizeof(g_wifi_at_execute_queue));
	g_wifi_set_queue_index = 0;
	g_wifi_get_queue_index = 0;
	g_wifi_queue_ticket_no = 0;
}

uint32_t wifi_set_request_in_queue( wifi_command_list_t command, int32_t socket )
{
	uint32_t ticket_no;
	ticket_no = ++g_wifi_queue_ticket_no;
	g_wifi_at_execute_queue[g_wifi_set_queue_index].at_command_id = command;
	g_wifi_at_execute_queue[g_wifi_set_queue_index].socket_no = socket;
	g_wifi_at_execute_queue[g_wifi_set_queue_index].result = WIFI_RETURN_ENUM_PROCESSING;
	g_wifi_at_execute_queue[g_wifi_set_queue_index].ticket_no = ticket_no;
	g_wifi_set_queue_index++;
	if(g_wifi_set_queue_index >= 10)
	{
		g_wifi_set_queue_index = 0;
	}
	return 	ticket_no;
}

wifi_at_execute_queue_t * wifi_get_current_running_queue(void)
{
	return &g_wifi_at_execute_queue[g_wifi_get_queue_index];
}

void wifi_set_result_to_current_running_queue( wifi_return_code_t result )
{
	if(g_wifi_at_execute_queue[g_wifi_get_queue_index].at_command_id != WIFI_COMMAND_NONE)
	{
		g_wifi_at_execute_queue[g_wifi_get_queue_index].result = result;
		g_wifi_get_queue_index++;
		if(g_wifi_get_queue_index >= 10)
		{
			g_wifi_get_queue_index = 0;
		}
	}
}



int8_t wifi_get_result_from_queue( uint32_t ticket_no, wifi_return_code_t *result )
{
	int i;

	for(i = 0;i<10;i++)
	{
		if(g_wifi_at_execute_queue[i].ticket_no == ticket_no)
		{
			if((g_wifi_at_execute_queue[i].result != WIFI_RETURN_ENUM_PROCESSING))
			{
				*result = g_wifi_at_execute_queue[i].result;
				g_wifi_at_execute_queue[i].at_command_id = WIFI_COMMAND_NONE;
				break;
			}
		}
	}
	if(i>= 10)
	{
		return -1;
	}
	return 0;
}
