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
#include "iperf_utility.h"
#include "iperf_sockets.h"
#include "cyabs_rtos.h"
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
extern int iperf( int argc, char *argv[], tlv_buffer_t** data );
int iperf_test(int argc, char *argv[], tlv_buffer_t** data);
/******************************************************
 *               Variables Definitions
 ******************************************************/

 static uint32_t wait_bits = 0x1;

#define IPERF_COMMANDS \
    { "iperf", iperf_test, 0, NULL, NULL, (char *)"", (char *)"Run iperf --help for usage."}, \


const cy_command_console_cmd_t iperf_command_table[] =
{
	IPERF_COMMANDS
	CMD_TABLE_END
};

#pragma pack(1)
struct cmd_result
{
	char status;
	char flag;
	int  count;
};

struct iperf_args
{
    int argc;
    char **argv;
};

typedef struct
{
    bool    available;
    struct  iperf_args args;
    cy_thread_t  *thread;
} iperf_thread_t;

static cy_event_t event;
iperf_thread_t iperf_util_threads[MAX_SIMULTANEOUS_COMMANDS];

/******************************************************
 *               Function Definitions
 ******************************************************/
void iperf_utility_init( void *networkInterface)
{
    int i;
    iperf_network_init( networkInterface );
    cy_command_console_add_table( iperf_command_table );

    for( i = 0; i < MAX_SIMULTANEOUS_COMMANDS; i++)
    {
        iperf_util_threads[i].available = true;
    }

    cy_rtos_init_event(&event);
}
int iperf_util_find_free_thread()
{
    int i = -1;

    for(i = 0; i < MAX_SIMULTANEOUS_COMMANDS; i++)
    {
        if(iperf_util_threads[i].available == true)
        {
            if(iperf_util_threads[i].thread != NULL)
            {
                delete iperf_util_threads[i].thread;
                iperf_util_threads[i].thread = NULL;
            }
            iperf_util_threads[i].available = false;
            break;
        }
    }
    return i;
}

void iperf_util_thread(cy_thread_arg_t arg)
{
    int index;
#ifdef MBEDOS
    index = arg;
#elif defined(AFR)
    index = *( (int *)arg );
#endif
    cy_rtos_setbits_event(&event, wait_bits, false);
    iperf ( iperf_util_threads[index].args.argc, iperf_util_threads[index].args.argv, NULL);
    iperf_util_threads[index].available = true;
}

int iperf_test(int argc, char *argv[], tlv_buffer_t** data)
{
    int result = 0;
    int index;

    index = iperf_util_find_free_thread();
    if( index == -1 )
    {
        printf("Error: exceeded maximum number of simultaneous commands supported\n");
        return -1;
    }

    iperf_util_threads[index].args.argc = argc;
    iperf_util_threads[index].args.argv = argv;

    iperf_util_threads[index].thread = new cy_thread_t;

    cy_thread_arg_t args;

#ifdef MBEDOS
    args = index;
#elif defined(AFR)
    args = &index;
#endif

    cy_rslt_t res = cy_rtos_create_thread(iperf_util_threads[index].thread, iperf_util_thread, NULL, NULL, 4096, CY_RTOS_PRIORITY_NORMAL, args);

    if(res != CY_RSLT_SUCCESS)
    {
    	printf("Error creating thread \n");
    	return -1;
    }

    cy_rtos_waitbits_event(&event, &wait_bits, true, true, CY_RTOS_NEVER_TIMEOUT);

    return result;
}

#ifdef __cplusplus
}
#endif
