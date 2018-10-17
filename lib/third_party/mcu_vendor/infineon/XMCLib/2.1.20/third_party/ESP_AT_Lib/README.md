# ESP8266 AT commands parser for RTOS systems

ESP-AT Library commands parser is a generic, platform independent, library for communicating with ESP8266 Wi-Fi module using AT commands. Module is written in C99 and is independent from used platform. Its main targets are embedded system devices like ARM Cortex-M, AVR, PIC and others, but can easily work under `Windows`, `Linux` or `MAC` environments.

## Features

- Supports latest ESP8266 AT commands software
- Platform independent and very easy to port
- Development of library under Win32 platform
- Available examples for ARM Cortex-M or Win32 platforms
- Written in C language (C99)
- Allows different configurations to optimize user requirements
- Supports implementation with operating systems with advanced inter-thread communications
- Uses `2` tasks for data handling from user and device
- Includes several applications built on top of library
  - Netconn sequential API for client and server
  - HTTP server with dynamic files (file system) supported
  - MQTT client
- Embeds other AT features, such as `WPS`, `PING`, `Hostname` and others
- User friendly MIT license

## Examples and resources

For examples, please check second repository, available at https://github.com/MaJerle/ESP_AT_Lib_res

## Documentation

Full API documentation with description and examples is available and is regulary updated with the source changes

http://majerle.eu/documentation/esp_at/html/index.html

## Contribution

I invite you to give feature request or report a bug. Please use issues tracker.
