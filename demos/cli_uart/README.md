## FreeRTOS-Plus-CLI UART Demo 
The demo shows how to build a CLI application using FreeRTOS-Plus-CLI library and a UART driver implementation using Common-IO interface.

## Usage

Demo implements three command handlers which are reigstered with the FreeRTOS-Plus-CLI library:
1. `echo_parameters` : Echoes back the parameters one at a time to the console. Accepts a variable number of parameters.
2. `echo_3_parameters`: Echoes back the parameters one at a time to the console. Input should always contain three parameters.
3. `task_stats`: Displays the status of each runnning task that includes task name, current state of the task, task priority, stack high water mark and the task number.
Please note that inorder to run `task_stats` command, `configUSE_TRACE_FACILITY`, `configUSE_STATS_FORMATTING_FUNCTIONS` and `configSUPPORT_DYNAMIC_ALLOCATION` should be enabled in `FreeRTOSConfig.h` for the respective boards. 

* To see the list of all commands with the details type help and then press ENTER.
* To run the previous command executed, simply press ENTER.

### Enable the demo

1. Enable the demo by defining `CONFIG_CLI_UART_DEMO_ENABLED` flag in `aws_demo_config.h`
2. Set the network types `democonfigNETWORK_TYPES` to `AWSIOT_NETWORK_TYPE_NONE` in `aws_demo_config.h`
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

The boards should support UART interface by implementing `iot_uart.h` functions within common IO library. Currently the demo is supported on ESP32-DevKitC, ESP-WROVER-KIT and STM32L4 Discovery kit IoT Node.
