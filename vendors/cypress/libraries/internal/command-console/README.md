# Command Console Library

### Overview
The command console framework provides an interface for the user to interact and execute various commands from the host PC via UART on the device.

### Features
- It enables a utility/module to register a set of commands, argument list, help and a handler for each command (as a command table).
- The command console forwards the response from the libraries' handlers back to the host running on the PC over UART.
- The command console framework currently supports the iperf, test_console utility and bt_utility.

### Dependencies
- [Cypress Middleware Error codes](https://github.com/cypresssemiconductorco/connectivity-utilities/blob/master/cy_result_mw.h)

### Library Requirements for AFR
- Following compile/build options should be included in the application make file (CMakeLists.txt) that uses this library on AFR 
  - -D**CY_RETARGET_IO_CONVERT_LF_TO_CRLF**
  - -D**CY_USE_LWIP**
  - -D**AFR**

### Known Limitations
- This library on AFR only supports UART polling method, interrupt based mechanism is not implemented yet. Hence, ENABLE_UART_POLLING should not be commented out in _command_console.h_
