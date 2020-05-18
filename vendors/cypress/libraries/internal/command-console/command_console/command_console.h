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


#pragma once

#include "cy_result_mw.h"
#include "command_utility.h"
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#define CMD_TABLE_END      { NULL, NULL, 0, NULL, NULL, NULL, NULL }
#define MAX_LINE_LENGTH    (128)
#define MAX_HISTORY_LENGTH (20)

/* Enable polling, comment this line to use serial interrupts mechanism */
#define ENABLE_UART_POLLING

/*
 * Results returned by Command console library
 */
#define CY_RSLT_MODULE_COMMAND_CONSOLE_ERR_CODE_START       (0)

#define CY_RSLT_COMMAND_CONSOLE_ERR_BASE                    CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_COMMAND_CONSOLE_BASE, CY_RSLT_MODULE_COMMAND_CONSOLE_ERR_CODE_START)

#define CY_RSLT_COMMAND_CONSOLE_FAILURE                     ((cy_rslt_t)(CY_RSLT_COMMAND_CONSOLE_ERR_BASE + 1))

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    ERR_CMD_OK           =  0,
    ERR_UNKNOWN          = -1,
    ERR_UNKNOWN_CMD      = -2,
    ERR_INSUFFICENT_ARGS = -3,
    ERR_TOO_MANY_ARGS    = -4,
    ERR_ADDRESS          = -5,
    ERR_NO_CMD           = -6,
    ERR_TOO_LARGE_ARG    = -7,
    ERR_OUT_OF_HEAP      = -8,
    ERR_BAD_ARG          = -9,
/* !!!when adding values here, also update command_console.c:console_default_error_strings */
    ERR_LAST_ERROR       = -10
} cy_command_console_err_t;

typedef struct
{
	uint8_t   type;
	uint8_t   length;
	uint8_t   value[];
} tlv_buffer_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef int       (*command_function_t)     ( int argc, char *argv[], tlv_buffer_t** data );
typedef cy_command_console_err_t (*help_example_function_t)( char* command_name, uint32_t eg_select );

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    char* name;                             /* The command name matched at the command line. */
    command_function_t command;             /* Function that runs the command. */
    int arg_count;                          /* Minimum number of arguments. */
    const char* delimit;                    /* Custom string of characters that may delimit the arguments for this command - NULL value will use the default for the console. */

    /*
     * These three elements are only used by the help, not the console dispatching code.
     * The default help function will not produce a help entry if both format and brief elements
     * are set to NULL (good for adding synonym or short form commands).
     */
    help_example_function_t help_example;   /* Command specific help function. Generally set to NULL. */
    char *format;                           /* String describing argument format used by the generic help generator function. */
    char *brief;                            /* Brief description of the command used by the generic help generator function. */
} cy_command_console_cmd_t;

/*
*
*/
typedef struct
{
    /* base parameters */
    void* serial;
    uint32_t line_len;                     /* Size of buffer to store command */
    char* buffer;                          /* Pointer pointing to buffer provided by application to store command given from console */
    uint32_t history_len;                  /* Size of buffer to store history */
    char* history_buffer_ptr;			   /* Pointer pointing to buffer provided by application to store the command history */
    const char* delimiter_string;          /* Delimiter string which seperates out the command line arguments */
    uint8_t thread_priority;               /* Command console thread priority */
    uint8_t params_num;                    /* Maximum parameters allowed */
} cy_command_console_cfg_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/** Initialize the Command console framework and spawns off a thread that listens on the console for commands. To be called by the application once
 *
 * @param[in]  buffer      : Pointer to the command console configuration.
 *
 * @return @ref cyrslt_t
 */
cy_rslt_t cy_command_console_init ( cy_command_console_cfg_t *cfg);

/** De-initializes the command console framework and terminates the thread
 *
 * @return @ref cyrslt_t
 */
cy_rslt_t cy_command_console_deinit( void );

/** Invoked to register a Table of cy_command_console_cmd_t entries
 *
 * @param[in]  commands    : Pointer to the commands.
 *
 * @return @ref cyrslt_t
 */
cy_rslt_t cy_command_console_add_table ( const cy_command_console_cmd_t *commands );

/** De-registers the cy_command_console_cmd_t table
 *
 * @param[in]  commands    : Pointer to the commands.
 *
 * @return @ref cyrslt_t
 */
cy_rslt_t cy_command_console_delete_table ( const cy_command_console_cmd_t *commands );

#ifdef __cplusplus
}
#endif
