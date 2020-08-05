# Connectivity Middleware Utilities

## What's Included?
Refer to the [README.md](./README.md) for a complete description of the utility libraries

## Changelog
### v2.1.0
* Introduced user argument support in JSON callback function
* Introduced default output, and time functions support in cy-log

### v2.0.0
* Removed cy_worker_thread APIs which are now part of abstraction RTOS library
* Renamed header files and APIs to include 'cy' prefix
* Added MBED and LWIP component-specific implementation for nw-helper

### v1.1.2
* Removed the usage of APIs that are deprecated in Mbed OS version 5.15.0

### v1.1.1
* Added TCPIP error code for socket options
* DHCP client ARP offload fixes to the network helper utilities

### v1.1.0
* Added support for string conversion helper, logging functions, worker thread and network helper functions
* Tested with ARMmbed OS 5.14.0

### v1.0.0
* Initial release for ARMmbed OS
* Supports JSON_parser and linked list libraries

### Supported Software and Tools
This version of the bluetooth gateway Middleware was validated for compatibility with the following Software and Tools:

| Software and Tools                                      | Version |
| :---                                                    | :----:  |
| ModusToolbox Software Environment                       | 2.1     |
| - ModusToolbox Device Configurator                      | 2.1     |
| - ModusToolbox CSD Personality in Device Configurator   | 2.0     |
| - ModusToolbox CapSense Configurator / Tuner tools      | 3.0     |
| GCC Compiler                                            | 9.2.1   |
| IAR Compiler                                            | 8.32    |
| ARM Compiler 6                                          | 6.13    |
| MBED OS                                                 | 5.15.3  |

