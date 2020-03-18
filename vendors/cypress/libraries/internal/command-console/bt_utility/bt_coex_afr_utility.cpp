/*
 * $ Copyright Cypress Semiconductor $
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

#include "iot_ble.h"
#include "aws_ble_coex_console_demo.h"

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
    int handle_ble_on(int argc, char* argv[], tlv_buffer_t** data);
    int handle_coex_throughput(int argc, char* argv[], tlv_buffer_t** data);

    /******************************************************
     *               Variables Definitions
     ******************************************************/

#define BT_COEX_COMMANDS \
    { "ble_on"  ,              handle_ble_on,           0, NULL, NULL, NULL, "Turn on BLE \n\t Downloads the fw "}, \
    { "ble_coex_get_throughput", handle_coex_throughput,   0, NULL, NULL, NULL, "Get BLE coex Throughput \n\t ex. handle_coex_throughput"}, \

    const cy_command_console_cmd_t bt_coex_command_table[] =
    {
        BT_COEX_COMMANDS
        CMD_TABLE_END
    };

    /******************************************************
     *               Function Definitions
     ******************************************************/

    int handle_ble_on(int argc, char* argv[], tlv_buffer_t** data)
    {
        printf(" IotBle_Init \n");
        IotBle_Init();
        return 0;
    }

    int handle_coex_throughput(int argc, char* argv[], tlv_buffer_t** data)
    {
        vCalculateServerThroughput(); // function from the aws_ble_coex_console_demo.c
    }

    // BT Coex Utility initialization
    void bt_coex_afr_utility_init(void)
    {
        cy_command_console_add_table(bt_coex_command_table);
    }
#ifdef __cplusplus
}
#endif
