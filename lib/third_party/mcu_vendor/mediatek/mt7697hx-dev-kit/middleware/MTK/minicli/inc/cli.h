/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __CLI_H__
#define __CLI_H__

#include <stdint.h>

#include "putter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *@addtogroup MiniCLI
 *@{
 * @brief This section introduces the MiniCLI APIs including terms and acronyms, supported features, details on how to use the MiniCLI, function groups, enums, structures and functions.
 *
 * MiniCLI is a CLI engine designed with the footprint in mind for the memory
 * and resource constrained systems. The code size is less than 3kB
 * including command history and line editing.
 *
 *
 * @section MiniCLI_Terms_Chapter Terms and acronyms
 *
 * |        Terms         |           Details                              |
 * |----------------------|------------------------------------------------|
 * |\b CLI                | Command line interface.                        |
 *
 * @section MiniCLI_Features_Chapter Supported features
 * - \b Custom \b command
 *   Enables to add custom-defined commands in addition to default CLI commands. The custom command signature must be consistent with other CLI commands.
 *
 * - \b Multi-Level \b command \b and \b argument \b passing
 *   The CLI command supports multi-level command and argument passing to a CLI command handler.
 *
 * - \b Hidden \b command
 *   Add prefix 0x01 to the CLI command help message. This command will not show in "?" the available command list.
 *
 * - \b Flexiable \b I/O \b interface
 *   Replace getchar, putchar with custom-defined functions to set user-defined source and destination for getchar and putchar commands.
 *
 * - \b Security
 *   Support custom login function. Authentication can be added to.
 *
 * @section MiniCLI_HOWTO How to use MiniCLI
 *
 * To use MiniCLI, two functions and one control block with configuration
 * are needed: \link #cli_init \endlink and \link #cli_task \endlink , as shown in the example below.
 *
 *    @code
 * - Step 1. Apply the configuration settings with the function \link #cli_init \endlink.
 * - Step 2. Call \link #cli_task \endlink to read the user input characters and then call the corresponding user-defined handler function.
 * - Step 3. The function \link #cli_task \endlink repeats until the user logs
 *    out, which may or may not be implemented by the developer.
 * - Step 4. The control is returned to the caller if log out is requested by
 *    the user. This step may never be reached if log out is not implemented.
 *    @code
 *      cli_t cli_config = { ... <configuration omitted> ... };
 *
 *      cli_init(&cli_config);
 *
 *      while (1) {
 *          cli_task();
 *      }
 *    @endcode
 *
 *
 *
 * @section MiniCLI_Configuration Configuration
 *
 * This section provides a MiniCLI configuration example using the following handler functions:
 * - #cli_setup_ip()
 * - #cli_setup_mac()
 * - #cli_ver()
 *
 * The command handlers are listed and linked together using a C structure
 * \link #cmd_t \endlink. In this example, the array \b cli_cmds is the root
 * of the commands, that is, there are two commands at the top-level:
 * <b>set</b> and <b>ver</b>. The command <b>set</b> requires a sub-command.
 * ) In this example, the sub-command could be <b>ip</b>
 * or <b>mac</b>.
 *
 * If "ver" is supplied as an input source (by calling #__io_getchar()),
 * #cli_ver() will be called. The version information will be
 * printed to the output using #__io_putchar().
 *
 * Whatever is supplied to the CLI engine is parsed into tokens similar to
 * main function. The only difference is, in main()
 * function call, the first parameter is the name of the executable.
 *
 * In MiniCLI, the parameter <b>len</b> indicates the number of tokens supplied to the handler.
 * The parameters supplied by the user on the CLI are stored in the <b>param</b> array based on the order of input.
 * The array counter starts from 0.
 *    @code
 *
 *      uint8_t cli_setup_ip(uint8_t len, char *param[])
 *      {
 *          if (len != 1) {
 *              printf("need an IP address\n");
 *              return 0;
 *          }
 *
 *          // Call this function to process the IP address.
 *          sys_setup_ip(param[0]);
 *
 *          return 0;
 *      }
 *
 *      uint8_t cli_setup_mac(uint8_t len, char *param[])
 *      {
 *          if (len != 1) {
 *              printf("need a mac address\n");
 *              return 0;
 *          }
 *
 *          // Call this function to process the MAC address.
 *          sys_setup_mac(param[0]);
 *
 *          return 0;
 *      }
 *
 *      uint8_t cli_ver(uint8_t len, char *param[])
 *      {
 *          printf("version: 2016-05-20\n");
 *          return 0;
 *      }
 *
 *      cmd_t cli_setup[] = {
 *          { "ip",   "ip address",   cli_setup_ip,  NULL },
 *          { "mac",  "mac address",  cli_setup_mac, NULL },
 *          { NULL,   NULL,           NULL,          NULL }
 *      };
 *
 *      cmd_t  cli_cmds[] = {
 *          { "set", "setup",        NULL,    &cli_setup[0] },
 *          { "ver", "show version", cli_ver, NULL          },
 *          { NULL,  NULL,           NULL,    NULL          }
 *      };
 *
 *      static cli_t cli_config = {
 *          .state  = 1,
 *          .echo   = 0,
 *          .get    = __io_getchar,
 *          .put    = __io_putchar,
 *          .knock  = cli_hardcode_login,
 *          .cmd    = cli_cmds
 *      };
 *    @endcode
 *
 * In #cli_config(), a member <b>state</b> specifies the initial
 * authentication status. If <b>state</b> is set to 1, the CLI engine
 * skips the login function. If it is set to 0, the CLI engine calls the function
 * #cli_hardcode_login() with the <b>user ID</b> and <b>password</b>
 * for an authenticated access.
 *
 * In #cli_config(), the member <b>echo</b> controls what to display.
 * In this example <b>echo</b> is used when typing a password, to hide or show it.
 * Default value is 0.
 *
 * This section explains the key fields in the configuration structure.
 * An example source code is provided in the cli_def.c source file.
 *
 * @note    The return value of the handler function is 0. MiniCLI may be enhanced in the future to support the
 *          processing of different return values.
 */

/****************************************************************************
 *
 * Constants.
 *
 ****************************************************************************/

/**
 * @defgroup _define Define
 * @{
  */

/**
 * Controls the number of tokens to parse in a single command.
 *
 * @note    The current limit of 20 may not be enough for some CLI commands,
 *          however, it is a fixed value to reduce the code size
 *          and reduce the need for dynamic memory allocation.
 */
#define CLI_MAX_TOKENS      (20)

/**
  * @}
  */

/****************************************************************************
 *
 * Types.
 *
 ****************************************************************************/

/** @defgroup minicli_typedef Typedef
  * @{
  */

/**
 * A function pointer prototype to get a characeter from an input source.
 *
 * @brief Generally, standard getchar() can be used for this purpose. But other
 * implementation can also be used. For example, in semihosting mode, the
 * standard getchar() function is replaced by a debug adapter. But to run CLI over telnet,
 * a custom-defined getchar() function can be used.
 *
 * @note    The standard getchar() returns an integer value. To reduce the CPU consumption
 *          of MiniCLI, the input source must be configured or written carefully
 *          to avoid returning non-character values.
 */
typedef int    (*getchar_fptr_t)(void);


/**
 * @brief A function pointer prototype to write a character to the output.
 *
 * Generally, standard putchar() can be used for this purpose. But other
 * implementations can also be used. For example, in semihosting mode, the
 * standard putchar() is replaced by a debug adapter. But to run CLI over telnet,
 * a replacement putchar function can be used.
 *
 * @note
 */
typedef int    (*putchar_fptr_t)(int);


/**
 * @brief This is the callback function to authenticate the user, if login is enabled and the credentials are not hardcoded.
 *
 * @retval  0,  if validation of the combination of 'id' and 'pass' failed.
 *              other values, if authentication completed successfully.
 */
typedef uint8_t (*knock_fptr_t)(char *id, char *pass);


/**
 * @brief Function pointer type of CLI command handlers.
 *
 * All CLI handlers must follow this prototype. An example is #cli_logout().
 *
 * @note    Currently, the return value of CLI handlers are ignored. However,
 *          to keep backward compatiblity, CLI handlers must return 0.
 *
 */
typedef uint8_t (*cli_cmd_handler_t)(uint8_t len, char *param[]);

/**
  * @}
  */

/*****************************************************************************
* Structures
*****************************************************************************/

/** @defgroup minicli_struct Struct
  * @{
  */

/**
 * @brief Provide a forward declaration for the cmd_t type
 *        to avoid issues with older compiler versions,
 *        see \link #cmd_s \endlink for more information.
 */
typedef struct cmd_s cmd_t;


/**
 * @brief   The MiniCLI command declaration structure.
 *          The strcut cmd_s is defined to aggregate function pointer, help
 *          messsage and sub-commands for MiniCLI engine.
 *
 * @note    Prefixing an ASCII value 01 to help message will effectively hide the command from user.
 * @warning To reduce code size, duplicate commands are not detected by
 *          MiniCLI engine. It is developer's task to ensure there is no
 *          duplication in the command declaration.
 */
struct cmd_s {
    char                *cmd;       /**< Command string. */
    char                *help;      /**< Type '?' to retrieve help message. */
    cli_cmd_handler_t   fptr;       /**< The function pointer to call when an input string matches with the command in MiniCLI engine. */
    cmd_t               *sub;       /**< Sub-commands. */
};


#if !defined(CLI_DISABLE_LINE_EDIT) && !defined(CLI_DISABLE_HISTORY)
/**
 * @brief Provide a forward declaration for the cli_history_t type
 * to avoid issues with older compiler versions,
 * see \link #cli_history_s \endlink for more information.
 */
typedef struct cli_history_s cli_history_t;

/**
 * @brief The MiniCLI control block definition.
 *
 * The strcut cli_history_s and its aliased type cli_history_t is defined to aggregate the
 * parameters for MiniCLI to work correctly.
 */
struct cli_history_s {
    char            **history;     /**< The pointers to <i>history_max</i> lines of buffer for input. */
    char            *input;        /**< used to save the current input before pressing "up" the first time */
    char            *parse_token;  /**< In cli.c, _cli_do_cmd() will result in the history command to be parsed
                                     *  into tokens, an additional array is required as an input to the #_cli_do_cmd() function 
									 *  to preserve the command history. */
    uint16_t        history_max;   /**< The maximum number of the lines in <i>history</i>. */
    uint16_t        line_max;      /**< The size of one history <i>line</i>. */
    uint16_t        index;         /**< When the user types a command and presses enter, index
                                     *  is the index that will be saved in. */
    uint16_t        position;      /**< When the user presses up/down, position is the history index
                                     *  displayed on the terminal. */
    uint8_t         full;          /**< Represents whether the number of history commands exceeded history_max
                                     *  if it is 0, history commands are not full, otherwise the commands are full. */
};
#endif /* !CLI_DISABLE_LINE_EDIT && !CLI_DISABLE_HISTORY */


/**
 * @brief Provide a forward declaration for the cli_t type
 *        to avoid issues with older compiler versions,
 *        see \link #cli_t \endlink for more information.
 */
typedef struct cli_s cli_t;

/**
 * @brief The MiniCLI control block definition.
 *
 * The strcut cli_s and its aliased type cli_t is defined to aggregate the
 * parameters for MiniCLI to work correctly.
 */
struct cli_s {
    uint8_t             state;                 /**< Defines the login status. 0, if not logged in and 1, otherwise. */
    cmd_t               *cmd;                  /**< A pointer to the CLI commands. */
    char                echo;                  /**< Determines if user input should be printed on the screen or not. */
    getchar_fptr_t      get;                   /**< A function pointer to get a characeter from input source. */
    putchar_fptr_t      put;                   /**< A function pointer to write a characeter to output sink. */
    knock_fptr_t        knock;                 /**< An authentication function pointer. */
    char                *tok[CLI_MAX_TOKENS];  /**< The tokenized input command. Note that there is a limit of 20 tokens. */

#if !defined(CLI_DISABLE_LINE_EDIT) && !defined(CLI_DISABLE_HISTORY)
    cli_history_t       history;               /**< The command history record. */
#endif /* !CLI_DISABLE_LINE_EDIT && !CLI_DISABLE_HISTORY */
};

/**  * @}
  */

/****************************************************************************
 *
 * Function prototypes.
 *
 ****************************************************************************/


/**
 * MiniCLI initialization.
 *
 * @brief          This function initializes the MiniCLI, call this function before applying other MiniCLI functions.
 *                 To initialize, one parameter containing a declared table and
 *                 the chains of commands is required. See cli_t for more information.
 *
 * @param[out] cli configuration.
 * @return         None
 *
 * @note    the memory pointed by <b>cli</b> must be writable and allocated
 *          solely for the use MiniCLI.
 * @par       Example
 *    Sample code, please refer to @ref MiniCLI_HOWTO How to start using MiniCLI
 */
void cli_init(cli_t *cli);


/**
 * @brief     Process one line of command in plain text format.
 *
 * @return    None
 * @param[in] line a string with command in it.
 */
void cli_line(char *line);


/**
 * @brief     Process a tokenized command in array of string pointers format.
 *
 * @return    None
 * @param[in] argc  the number of elements in argv[].
 * @param[in] argv  the array of string pointers.
 */
void cli_tokens(uint8_t argc, char *argv[]);


/**
 * @brief     The top-level function of the actual CLI.
 *            This function will never exit unless the user logged out.
 * @return    None
 * @par       Example
 *    Sample code, please refer to @ref MiniCLI_HOWTO How to use MiniCLI
 */
void cli_task(void);


/**
 * @brief       This function implements the logout function.
 *
 * @param[in]   len is the number of elements in the array pointer <b>param</b>.
 * @param[in]   param is an array of pointers remove to the parameters.
 * @return      always zero (for now).
 */
uint8_t cli_logout(uint8_t len, char *param[]);


/**
 * @brief       A sample login function implementation.
 *              The login ID and password were both hardcoded in source code as '1'.
 * @param[in]   ID is not used.
 * @param[in]   password is not used.
 * @return      always 1 (for now).
 * @note        Apply this as a reference implementation for user-defined login API.
 */
uint8_t cli_hardcode_login(char *id, char *password);


/**
 *@}
 */
#ifdef __cplusplus
}
#endif

#endif /* __CLI_H__ */

