## Introduction
The connectivity utilities library is a collection of general purpose middleware utilities. Several connectivity middleware libraries shall depend on this utilities library. 
Currently, the following utilities are included:

### JSON parser
The JSON format is often used for serializing and transmitting structured data over a network connection. It is used primarily to transmit data between a server and web application, serving as an alternative to XML. JSON is JavaScript Object Notation. The JSON parser utility library provides helper functions to parse JSON objects and calls the function registered by the caller

Refer to the [cy_json_parser.h](./JSON_parser/cy_json_parser.h) for API documentation

### Linked list
This is a generic linked list library with helper functions to add, insert, delete and find nodes in a list.

Refer to the [cy_linked_list.h](./linked_list/cy_linked_list.h) for API documentation

### String utilities
The string utilities module is a collection of string conversion helpers to convert between integer and strings.

Refer to the [cy_string_utils.h](./cy_string/cy_string_utils.h) for API documentation

### Network helpers
This is a collection of network helper functions to fetch IPv4 address of the local device, notify IPv4 address change via callback and conversion utilities.

Refer to the [cy_nw_helper.h](./network/cy_nw_helper.h) for API documentation

### Logging functions
This module is a logging subsystem that allows run time control for the logging level. Log messages are passed back to the application for output. A time callback can be provided by the application for the timestamp for each output line. Log messages are mutex protected across threads so that log messages do not interrupt each other.

Refer to the [cy_log.h](./cy_log/cy_log.h) for API documenmtation

### Middleware Error codes
The utilities library includes a collection of middleware error codes for various middleware libraries such as AWS IoT, HTTP server, Enterprise security etc.

Refer to [cy_result_mw.h](./cy_result_mw.h) for details

## Supported platforms
This middleware library and its features are supported on following PSoC 6 platforms:
* [PSoC6 WiFi-BT Prototyping Kit (CY8CPROTO-062-4343W)](https://www.cypress.com/documentation/development-kitsboards/psoc-6-wi-fi-bt-prototyping-kit-cy8cproto-062-4343w)
* [PSoC6 WiFi-BT Pioneer Kit (CY8CKIT-062-WiFi-BT)](https://www.cypress.com/documentation/development-kitsboards/psoc-6-wifi-bt-pioneer-kit-cy8ckit-062-wifi-bt)
* [PSoC 62S2 Wi-Fi BT Pioneer Kit (CY8CKIT-062S2-43012)](https://www.cypress.com/documentation/development-kitsboards/psoc-62s2-wi-fi-bt-pioneer-kit-cy8ckit-062s2-43012)

## Integration Notes
* The connectivity utilities library has been designed to work with both the ARM mbed ecosystem and AnyCloud. 
* It is adequate to include this library in the desired ecosystem to use these utilities. Depending on the ecosystem, the respective source files will get picked up and linked. This is accomplished using the COMPONENT_ model.
* In order to ease integration of Wi-Fi connectivity components to code examples, this connectivity utilities library has been bundled into the [Wi-Fi middleware core library v2.0.0](https://github.com/cypresssemiconductorco/wifi-mw-core) on AnyCloud.
* For mbed ecosystem, this library has to be included by the respective code examples.
* NOTE: Refer to the COMPOMENT_ folders for implementation details pertinent to the ecosystem. For instance, certain network helper functions are not implemented on AnyCloud, and are leveraged from Wi-Fi Connection Manager

## Additional Information
* [Connectivity Utilities RELEASE.md](./RELEASE.md)
* [Connectivity Utilities API reference guide](https://cypresssemiconductorco.github.io/connectivity-utilities/api_reference_manual/html/index.html)
* [Connectivity Utilities version](./version.txt)
