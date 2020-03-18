/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software"), is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */


/** @file
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "command_console.h"
#include "test_console_utility.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
int test_console(int argc, char *argv[], tlv_buffer_t** data);
/******************************************************
 *               Variables Definitions
 ******************************************************/

#define TEST_CONSOLE_COMMANDS \
    { (char*) "test_console",           test_console, 1, NULL, NULL, (char *)"<0|1>", (char *)"Test Console application. 0 - get data in ASCII format, 1 - get data in binary format" }, \


const cy_command_console_cmd_t test_command_table[] =
{
	TEST_CONSOLE_COMMANDS
    CMD_TABLE_END
};

#pragma pack(1)
struct cmd_result
{
	char status;
	char flag;
	int  count;
};
#pragma pack()

/******************************************************
 *               Function Definitions
 ******************************************************/
void test_console_utility_init(void)
{
	cy_command_console_add_table ( test_command_table );
}

int test_console(int argc, char *argv[], tlv_buffer_t** data)
{
	int result = 0;
	int len = 0;
	int type = atoi( argv[1] );
	tlv_buffer_t* tlv = NULL;

	if( type == 0)
	{

		/* Allocate buffer with the size */
		tlv = (tlv_buffer_t*) malloc( sizeof(tlv_buffer_t) + 18);
		if ( tlv == NULL )
		{
			return -1;
		}

		memcpy(tlv->value, "status", strlen("status"));
		len += strlen("status");

		memcpy(tlv->value + len, ",", strlen(","));
		len += strlen(",");

		memcpy(tlv->value + len, "COMPLETED", strlen("COMPLETED"));
		len += strlen("COMPLETED");

		tlv->type   = 0;
		tlv->length = len;

		*data = tlv;
	}
	else
	{
		cmd_result cmd;
		cmd.status = 1;
		cmd.flag   = 1;
		cmd.count  = 100;

		/* Allocate buffer with the size */
		tlv = (tlv_buffer_t*) malloc( sizeof(tlv_buffer_t) + sizeof(cmd_result));
		if (tlv == NULL)
		{
			return -1;
		}

		tlv->type = 1;
		tlv->length = sizeof(cmd_result);

		tlv->value[0] = cmd.status;
		tlv->value[1] = cmd.flag;

		tlv->value[2] = (cmd.count&0xFF); //extract first byte
		tlv->value[3] = ((cmd.count>>8)&0xFF); //extract second byte
		tlv->value[4] = ((cmd.count>>16)&0xFF); //extract third byte
		tlv->value[5] = ((cmd.count>>24)&0xFF); //extract fourth byte

		*data = tlv;

	}

	return result;
}

#ifdef __cplusplus
}
#endif
