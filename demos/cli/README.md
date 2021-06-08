## FreeRTOS+CLI UART Demo 
The demo shows how to build a CLI application using FreeRTOS+CLI library and a UART driver implementation for Common-IO interface.

## Usage

Demo implements three command handlers which are registered with the FreeRTOS-Plus-CLI library:
1. `echo_parameters` : Echoes back the parameters one at a time to the console. Accepts a variable number of parameters.
2. `echo_3_parameters`: Echoes back the parameters one at a time to the console. Input should always contain three parameters.
3. `task_stats`: Displays the status of each runnning task that includes task name, current state of the task, task priority, stack high water mark and the task number.
Please note that inorder to run `task_stats` command, `configUSE_TRACE_FACILITY`, `configUSE_STATS_FORMATTING_FUNCTIONS` and `configSUPPORT_DYNAMIC_ALLOCATION` should be enabled in `FreeRTOSConfig.h` for the respective boards. 

* To see the list of all commands with the details type help and then press ENTER.
* To run the previous command executed, simply press ENTER.

### Enable the demo

1. Enable the demo by defining `CONFIG_CLI_UART_DEMO_ENABLED` flag in `aws_demo_config.h`.
2. Set the network types `democonfigNETWORK_TYPES` to `AWSIOT_NETWORK_TYPE_NONE` in `aws_demo_config.h` for the board.
3. Set the enabled network `configENABLED_NETWORKS` to `AWSIOT_NETWORK_TYPE_NONE` in `aws_iot_demo_network_config.h` for the board.
3. Build and flash the application onto the board.

### Example

```
[Input a command or type help then press ENTER to get started]
>task-stats
Task          State  Priority  Stack    #
************************************************
iot_thread      X       5       482     4
IDLE            R       0       70      2
iot_thread      B       5       1984    6
iot_thread      B       5       1986    5
Logging         B       6       402     1
Tmr Svc         B       5       97      3

[Press Enter to run the previous command]
```

## Requirements

The demo uses a console IO implementation using common IO uart APIs. For the demo to work, the board should have an implementation for common IO APIs in `iot_uart.h`. 
Alternatively user can implement the `xConsoleIO_t` interface defined in `FreeRTOS_CLI_Console.h` with their IO of choice and pass it to `FreeRTOS_CLIEnterConsoleLoop` within the demo.