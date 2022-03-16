# Changelog
This repository contains the `FreeRTOS AWS Reference Integrations`, which are pre-integrated FreeRTOS projects ported to microcontroller-based evaluation boards that demonstrate end-to-end connectivity to AWS IoT Core. The repository contains projects for several microcontroller-based evaluation boards.

## 202203.00 March 2022

### Updates

#### FreeRTOS Kernel

- Updated to use V10.4.3-LTS-Patch-2

#### FreeRTOS-Plus-TCP

- Updated to use V2.3.2-LTS-Patch-2

#### corePKCS11

- Updated Mbed TLS version to 2.28.0

#### Vendors

- Updated the Wi-Fi abstraction implementations for the ports listed below. This is to minimize the time the Wi-Fi password is held in RAM by clearing it immediately after usage. In particular, within implementations of the WIFI_ConnectAP() and WIFI_ConfigureAP() functions on the following platforms:

    - CC3220SF-LAUNCHXL
    - STM32L4 Discovery kit IoT node 
    - LPC54018 IoT Module 
    - ESP32-DevKitC, ESP-WROVER-KIT, ESP32-WROOM-32SE, ESP32-S2-SAOLA-1 
    - XMC4800 Iot Connectivity Kit, XMC4800 and Optiga TrustX 
    - MediaTek MT7697Hx Development Kit 
    - Renesas Starter Kit+ for RX65N-2MB 
    - Cypress CYW954907AEVAL1F, CYW43907AEVAL1F Evaluation Kit, PSoC 64 Standard Secure AWS Wi-Fi Bluetooth Pioneer Kit 
    - MW320, MW322 AWS IoT Starter Kit 
    - Numaker-IoT-M487

## 202107.00 July 2021

### New Features

#### coreMQTT Agent v1.0.0

- The coreMQTT Agent library is a high level API that adds thread safety to the coreMQTT (https://github.com/FreeRTOS/coreMQTT) library. The library provides thread safe equivalents to the coreMQTT's APIs, greatly simplifying its use in multi-threaded environments. The coreMQTT Agent library manages the MQTT connection by serializing the access to the coreMQTT library and reducing implementation overhead. This allows your multi-threaded applications to share the same MQTT connection, and enables you to design an embedded application without having to worry about coreMQTT thread safety.
- See memory requirements for the latest release here (https://freertos.org/Documentation/api-ref/coreMQTT-Agent/docs/doxygen/output/html/index.html#core_mqtt_agent_memory_requirements)

### Updates

#### AWS IoT Over-the-air Update V3.0.0

- The AWS IoT Over-the-air update (OTA) (https://github.com/aws/ota-for-aws-iot-embedded-sdk) library enables you to manage the notification of a newly available update, download the update, and perform cryptographic verification of the firmware update. Using the OTA library, you can logically separate firmware updates from the application running on your devices. You can also use the library to send other files (e.g. images, certificates) to one or more devices registered with AWS IoT. More details about OTA library can be found in AWS IoT Over-the-air Update documentation (https://docs.aws.amazon.com/freertos/latest/userguide/freertos-ota-dev.html).
- The AWS IoT Over-the-air update library has a dependency on coreJSON (https://github.com/FreeRTOS/coreJSON) for parsing of JSON job document and tinyCBOR (https://github.com/intel/tinycbor) for decoding encoded data streams, other than the standard C library. It can be used with any MQTT library, HTTP library, and operating system (e.g. Linux, FreeRTOS). See demos to download firmware image over MQTT (using coreMQTT Agent) and over HTTP (using coreMQTT Agent and coreHTTP) using FreeRTOS at https://docs.aws.amazon.com/freertos/latest/userguide/dev-guide-ota-workflow.html

#### AWS IoT Jobs v1.0.0

- This release brings minor updates (https://github.com/aws/Jobs-for-AWS-IoT-embedded-sdk/blob/v1.1.0/CHANGELOG.md#v110-march-2021) to the AWS IoT Jobs library that add API support for DescribeNextPendingJob API of the AWS IoT Jobs service, and macro utilities for compile-time generation of topic strings.

#### AWS IoT Device Defender v1.0.1

- This release adds macros to AWS IoT Device Defender Library API for the custom metrics (https://docs.aws.amazon.com/iot/latest/developerguide/dd-detect-custom-metrics.html) feature of AWS IoT Device Defender service. 
- The demo has been updated to use the custom metrics feature.

#### BLE v2.0.2

- Moved from `libraries/c_sdk/ble` to `libraries/ble`.

#### HTTPS Compatibility Layer  v1.2.0

- Removed under `libraries/c_sdk/standard/https`. Migrated to coreHTTP v2.0.0 under `libraries/coreHTTP`.

#### MQTT Compatibility layer v2.3.1 

- Removed under `libraries/c_sdk/standard/mqtt`. Migrated to coreMQTT v1.1.0 under `libraries/coreMQTT`.

#### IoT Serializer v1.1.2

- Removed under `libraries/c_sdk/standard/serializer` as there are no longer dependencies on this library.

#### MQTT Compatibility-based AWS IoT Shadow v2.2.2

- Removed under `libraries/c_sdk/aws/shadow`. Migrated to AWS IoT Device Shadow v1.0.2 under `libraries/device_shadow_for_aws`.

#### MQTT Compatibility-based AWS IoT Defender V3.0.3

- Removed under `libraries/c_sdk/aws/defender`. Migrated to AWS IoT Device Defender v1.1.0 under `libraries/device_defender_for_aws`.

#### Secure Sockets v1.3.1

- Updated the secure socket test suite to be simpler and more robust.

#### Demos

- Added coreMQTT-Agent Demo.
- Added OTA demo over BLE transport interface.
- Updated GreenGrass Discovery Demo using coreMQTT library.

#### Vendors

- Updated all supported platforms to use coreMQTT-Agent Demo.
- Updated OTA PAL for the following ports to work with AWS IoT Over-the-air Update V3.0.0 Library Interfaces:
    - Espressif ESP32-DevKitC
    - Espressif ESP-WROVER-KIT
    - Microchip ATECC608A with Windows Simulator
    - Renesas Starter Kit + RX65N-2MB
    - TI CC3220 Launch Pad
    - Nordic RF52480
    - Microsoft Windows Simulator
- New board added:
    - Espressif ESP32-S2-SAOLA-1, supporting coreMQTT-Agent Demo and AWS IoT Over-the-air V3.0.0
- Updated Espressif (Amazon) FreeRTOS SDK (ESP-IDF) for ESP platforms to V4.2.1 and add I2C patch to fix the incorrect FreeRTOS API usage from ISR context. 
- Deprecated:
    - Microchip Curiosity PIC32MZEF
    - NXP LPC54608

## 202012.00 December 2020

### New Features

#### coreHTTP v2.0.0

- The coreHTTP (https://github.com/FreeRTOS/coreHTTP) library provides the ability to establish an HTTP connection with a server over a customer-implemented transport layer, which can either be a secure channel like a TLS session (mutually authenticated or server-only authentication) or a non-secure channel like a plaintext TCP connection. The HTTP connection can be used to make "GET" (include range requests), "PUT", "POST" and "HEAD" requests. The library provides a mechanism to register a customer-defined callback for receiving parsed header fields in an HTTP response. The library has been refactored for memory optimization, and is a client implementation of a subset of the HTTP/1.1 (https://tools.ietf.org/html/rfc2616) standard.
- See memory requirements for the latest release here (https://docs.aws.amazon.com/embedded-csdk/202011.00/lib-ref/libraries/standard/coreHTTP/docs/doxygen/output/html/index.html#http_memory_requirements).

#### backoffAlgorithm v1.0.0

- The backoffAlgorithm (https://github.com/FreeRTOS/backoffAlgorithm) library is a utility library to calculate backoff period for network operation retries (like failed network connection with server) using an exponential backoff with jitter algorithm. This library uses the "Full Jitter" strategy for the exponential backoff with jitter algorithm. More information about the algorithm can be seen in the Exponential Backoff and Jitter (https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/) AWS blog.
- Exponential backoff with jitter is typically used when retrying a failed connection or network operation with the server. An exponential backoff with jitter helps to mitigate the request failures made to servers, that are caused due to network congestion or high load on the server, by spreading out retry requests across multiple devices. Besides, in an environment with poor connectivity, a client can get disconnected at any time. A backoff strategy helps the client to conserve battery by not repeatedly attempting reconnections when they are unlikely to succeed.
- The backoffAlgorithm library has no dependencies on libraries other than the standard C library.

#### AWS IoT Device Defender v1.0.1

- The AWS IoT Device Defender (https://github.com/aws/device-defender-for-aws-iot-embedded-sdk) library enables you to interact with the AWS IoT Device Defender service to continuously monitor security metrics from devices for deviations from what you have defined as appropriate behavior for each device. If something doesn’t look right, AWS IoT Device Defender sends out an alert so you can take action to remediate the issue. More details about Device Defender can be found in AWS IoT Device Defender documentation (https://docs.aws.amazon.com/iot/latest/developerguide/device-defender.html).
- The AWS IoT Device Defender library has no dependencies on additional libraries other than the standard C library. It also doesn’t have any platform dependencies, such as threading or synchronization. It can be used with any MQTT library and any JSON library (see demos (https://github.com/aws/amazon-freertos/tree/master/demos/device_defender_for_aws) with coreMQTT and coreJSON).
- See memory requirements for the latest release here (https://docs.aws.amazon.com/embedded-csdk/202011.00/lib-ref/libraries/aws/device-defender-for-aws-iot-embedded-sdk/docs/doxygen/output/html/index.html#defender_memory_requirements).

#### AWS IoT Jobs v1.0.0

- The AWS IoT Jobs (https://github.com/aws/jobs-for-aws-iot-embedded-sdk) library enables you to interact with the AWS IoT Jobs service which notifies one or more connected devices of a pending “Job”. A Job can be used to manage your fleet of devices, update firmware and security certificates on your devices, or perform administrative tasks such as restarting devices and performing diagnostics. For documentation of the service, please see the AWS IoT Developer Guide (https://docs.aws.amazon.com/iot/latest/developerguide/iot-jobs.html). Interactions with the Jobs service use the MQTT protocol. This library provides an API to compose and recognize the MQTT topic strings used by the Jobs service.
- The AWS IoT Jobs library has no dependencies on additional libraries other than the standard C library. It also doesn’t have any platform dependencies, such as threading or synchronization. It can be used with any MQTT library and any JSON library (see demos (https://github.com/aws/amazon-freertos/tree/master/demos/jobs_for_aws) with coreMQTT and coreJSON).
- See memory requirements for the latest release (https://docs.aws.amazon.com/embedded-csdk/202011.00/lib-ref/libraries/aws/jobs-for-aws-iot-embedded-sdk/docs/doxygen/output/html/index.html#jobs_memory_requirements)here (https://docs.aws.amazon.com/embedded-csdk/202011.00/lib-ref/libraries/aws/jobs-for-aws-iot-embedded-sdk/docs/doxygen/output/html/index.html#jobs_memory_requirements).

#### HTTP Compatibility Layer v1.2.0

- The HTTP Compatibility Layer provides backwards compatibility from coreHTTP library to HTTPS V1.x.x APIs. In addition, the HTTP Compatibility Layer maintains the dependency of the network abstraction and linear containers from the HTTPS V1.x.x library. The task pool dependency is removed in order to allow the user to allocate tasks on the stack rather than exclusively on the heap.
- Configuration settings using C preprocessor constants, for the HTTP Compatibility Layer, are available in addition to the original configurations of the HTTPS V1.x.x library (https://docs.aws.amazon.com/freertos/latest/lib-ref/html2/https/https_config.html). They can be set with a #define in the config file (iot_config.h) or by using a compiler option such as -D in gcc. If a configuration setting is not defined, the HTTP Compatibility Layer will use a "sensible" default value (unless otherwise noted). Because they are compile-time constants, this HTTP Compatibility Layer must be rebuilt if a configuration setting is changed.
    - IOT_HTTPS_DISPATCH_QUEUE_SIZE - The number of requests in the queue that are ready to be sent to the HTTP server.
    - IOT_HTTPS_DISPATCH_TASK_COUNT - The number of tasks that are responsible for sending requests from the dispatch queue.
    - IOT_HTTPS_DISPATCH_TASK_STACK_SIZE - The stack size of each dispatch task, sized appropriately for each board.
    - IOT_HTTPS_DISPATCH_USE_STATIC_MEMORY - If set to 1, the memory used by the dispatch task will be allocated statically by the library. Otherwise, memory will be allocated on the heap.
    - IOT_HTTPS_DISPATCH_TASK_PRIORITY - The priority of each dispatch task. This priority is deliberately chosen to match the original taskpool's priority. Doing so prevents starvation of the network-receive task and tasks potentially used by other libraries.
- See memory requirements for the latest release (https://docs.aws.amazon.com/embedded-csdk/202011.00/lib-ref/libraries/aws/jobs-for-aws-iot-embedded-sdk/docs/doxygen/output/html/index.html#jobs_memory_requirements) here (https://github.com/aws/amazon-freertos/blob/master/libraries/c_sdk/standard/https/CODESIZE.md).

### Updates

#### FreeRTOS kernel V10.4.3

- Includes FreeRTOS kernel V10.4.3
- Additional details can be found here: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/V10.4.3-kernel-only/History.txt

#### FreeRTOS-Plus-TCP v2.3.2

- When a protocol error occurs during the SYN-phase of a TCP connection, a child socket will now be closed (calling FreeRTOS_closesocket() ), instead of being given the eCLOSE_WAIT status.  A client socket, which calls connect() to establish a connection, will receive the eCLOSE_WAIT status, just like before.

#### coreMQTT v1.1.0

- Update logs and format specifiers to use standard C types.
- Add dependence on stdbool.h
- Introduce two new configuration macros for setting the timeout for transport reads and sends.

#### coreJSON v3.0.0

- Added an API function to iterate over a collection in a JSON document.

#### corePKCS11 v3.0.0

- Updated xFindObjectWithLabelAndClass to include a size parameter to allow passing in the size of the label, in order to avoid calling strlen in the library code.
- Added CBMC memory proofs for all functions.
- Removed threading_alt.h from corePKCS11.
- Restructured third party folder in order to align with other core repositories. Folders located in “corePKCS11/3rdparty” are now in “corePKCS11/source/dependency/3rdparty”.
- Updated logs and format specifiers to use standard C types.

#### AWS IoT Device Shadow v1.0.2

- Update logs and format specifiers to use standard C types.

#### MQTT Compatibility Layer v2.3.1

- Fixes for compiler warnings by removing unused functions and variables.

## 202011.01 December 2020

This release does not change any source files compared to 202011.00. It only fixes an issue with `checksums.json` file used by AWS IoT Device Tester to verify the integrity of FreeRTOS source files during the qualification process. This issue was present only on Windows while Linux and MacOS were unaffected.

## 202011.00 November 2020

### New Features

This release includes refactored MQTT, JSON Parser, and AWS IoT Device Shadow libraries for optimized memory usage and modularity, and includes dependent libraries via GitHub submoduling. These libraries have gone through code quality checks including verification that no function has a [GNU Complexity](https://www.gnu.org/software/complexity/manual/complexity.html) score over 8, and checks against the [MISRA coding standard](https://www.misra.org.uk/MISRAHome/MISRAC2012/tabid/196/Default.aspx). Deviations from the MISRA C:2012 guidelines are documented under [MISRA Deviations](https://github.com/aws/aws-iot-device-sdk-embedded-C/blob/202009.00/MISRA.md). These libraries have also undergone both static code analysis from [Coverity static analysis](https://github.com/aws/aws-iot-device-sdk-embedded-C/blob/202009.00/MISRA.md), and validation of memory safety and functional correction proofs through the CBMC automated reasoning tool.

The MQTT library in this release, coreMQTT, supports backward compatibility with the MQTT library in FreeRTOS version 201906.00 or higher via a [compatibility layer](https://github.com/aws/amazon-freertos/tree/202011.00/libraries/c_sdk/standard/mqtt). Therefore, MQTT-dependent libraries in FreeRTOS version 201906.00 or higher, including AWS IoT Device Shadow, AWS IoT Device Defender, and Greengrass Discovery, can use the coreMQTT library via the compatibility layer.

#### AWS IoT Device Shadow V1.0.1

- The [AWS IoT Device Shadow](https://github.com/aws/device-shadow-for-aws-iot-embedded-sdk) library enables you to store and retrieve the current state (the “shadow”) of every registered device. The device’s shadow is a persistent, virtual representation of your device that you can interact with from AWS IoT Core even if the device is offline. The device state captured as its “shadow” is itself a JSON document. The device can send commands over MQTT or HTTP to update its latest state. Each device’s shadow is uniquely identified by the name of the corresponding “thing”, a representation of a specific device or logical entity on AWS IoT. More details about AWS IoT Device Shadow can be found in [AWS IoT documentation](https://docs.aws.amazon.com/iot/latest/developerguide/iot-device-shadows.html).
- The AWS IoT Device Shadow library has no dependencies on additional libraries other than the standard C library. It also doesn’t have any platform dependencies, such as threading or synchronization. It can be used with any MQTT library and any JSON library (see [demos](https://github.com/aws/aws-iot-device-sdk-embedded-C/tree/master/demos/shadow) with coreMQTT and coreJSON).
- See memory requirements for the latest release [here](https://docs.aws.amazon.com/embedded-csdk/202011.00/lib-ref/libraries/aws/device-shadow-for-aws-iot-embedded-sdk/docs/doxygen/output/html/index.html#shadow_memory_requirements).

#### coreJSON V2.0.0

- The [coreJSON](https://github.com/FreeRTOS/coreJSON) library is a JSON parser that strictly enforces the [ECMA-404 JSON standard](https://www.json.org/json-en.html). It provides a function to validate a JSON document, and a function to search for a key and return its value. A search can descend into nested structures using a compound query key. A JSON document validation also checks for illegal UTF8 encodings and illegal Unicode escape sequences.
- See memory requirements for the latest release [here](https://docs.aws.amazon.com/embedded-csdk/202011.00/lib-ref/libraries/standard/coreJSON/docs/doxygen/output/html/index.html#json_memory_requirements).

#### coreMQTT V1.0.1

- The [coreMQTT](https://github.com/FreeRTOS/coreMQTT) library provides the ability to establish an MQTT connection with a broker over a customer-implemented transport layer, which can either be a secure channel like a TLS session (mutually authenticated or server-only authentication) or a non-secure channel like a plaintext TCP connection. This MQTT connection can be used for performing publish operations to MQTT topics and subscribing to MQTT topics. The library provides a mechanism to register customer-defined callbacks for receiving incoming PUBLISH, acknowledgement and keep-alive response events from the broker. The library has been refactored for memory optimization and is compliant with the [MQTT 3.1.1](http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/mqtt-v3.1.1.html) standard. It has no dependencies on any additional libraries other than the standard C library, a customer-implemented network transport interface, and optionally a customer-implemented platform time function. The refactored design embraces different use-cases, ranging from resource-constrained platforms using only QoS 0 MQTT PUBLISH messages to resource-rich platforms using QoS 2 MQTT PUBLISH over TLS connections.
- See memory requirements for the latest release [here](https://docs.aws.amazon.com/embedded-csdk/202011.00/lib-ref/libraries/standard/coreMQTT/docs/doxygen/output/html/index.html#mqtt_memory_requirements).

#### OTA PAL for Renesas Starter Kit + RX65N-2MB

- Added OTA PAL Port for Renesas RX65N-2MB board

### Updates

#### Bluetooth Low Energy (BLE) Hardware Abstraction Library (HAL) V5.1.0

- Added ACL connection state change callback for BLE HAL

#### Bluetooth Low Energy Management Library V2.2.0

- Added transport interface for BLE library to send data to AWS IoT using the coreMQTT and AWS IoT Device Shadow client libraries. The transport interface utilizes a companion device mobile application implemented using FreeRTOS BLE Android and iOS SDKs to send data to AWS IoT.
- Added coreMQTT and AWS IoT Device Shadow demos for BLE. The respective demos "MQTT BLE Transport Demo" and "Shadow BLE Transport Demo" can be found under `demos/ble` folder. The demo uses the new MQTT and Shadow client libraries and the BLE transport interface to send and receive data with AWS IoT.

#### Common I/O Library V0.1.2

- Added more peripherals to CMake.

#### FreeRTOS+CLI V1.0.5

- Added FreeRTOS+CLI V1.0.4 to Amazon FreeRTOS repository.
- Added FreeRTOS Console API to interact with CLI over common IO or UDP interface.
- Added sample which demonstrates executing commands using FreeRTOS+CLI and UART interface.

#### FreeRTOS+TCP V2.3.1

- Sub-moduled to the [FreeRTOS/FreeRTOS-Plus-TCP](https://github.com/FreeRTOS/FreeRTOS-Plus-TCP) repository. This is a breaking change for users using 202007.00 release of amazon-freertos due to change in folder structure.
- Bug Fix for UDP only (`ipconfigUSE_TCP == 0`) compilation of FreeRTOS+TCP. Conditional compilation on the value of `ipconfigUSE_TCP` updated to exclude TCP only components.
- Added descriptions for functions and variables in Doxygen compatible format.
- Updated `prvParseDNSReply` function signature.

#### FreeRTOS+POSIX Utils V1.2.0

- The `iot_pki_utils.h` and `iot_pki_utils.c` are renamed to `core_pki_utils.h` and `core_pki_utils.c` respectively. They are also now a part of the corePKCS11 repository.

#### Greengrass Discovery V2.0.2

- Added more logging in library.

#### MQTT Client Library V2.3.0

- Refactored as compatibility layer for V2.x.x MQTT APIs using coreMQTT library. This is the library that supports backward compatibility with MQTT APIs present in between 201906.00 and 202007.00.

#### Over the Air Update V1.2.1

- Added check to abort the update if updating job status as self-test in service fails, this helps in early detection of mismatch in device and jobs states before activating new image.

#### PKCS11 V2.1.1

- Sub-moduled to the [FreeRTOS/corePKCS11](https://github.com/FreeRTOS/corePKCS11) repository.
- Updated ECC608A and PSA library dependencies to corePKCS naming scheme. Refactored AFQP tests to support HSMs that have locked down credentials.

#### Secure Sockets LwIP V1.3.0

- Added new optional API `SOCKETS_Bind`.
- Extended `SOCKETS_SetSockOpt` to support TCP keepalive settings.

#### Shadow V2.2.3

- Updated unit tests to work with the MQTT compatibility layer. This is the library that supports backward compatibility with Shadow APIs present in between 201906.00 and 202007.00.

#### TLS Shim Layer V1.3.0

- Added missing Max Fragment Length (MFL) runtime configuration if MFL macro is enabled.

#### Wi-Fi V2.0.0

- Updated WiFi APIs to support more granular error codes, WEP encryption, SoftAP provisioning, optional asynchronous APIs, event handling and set country code. This is a breaking change to the WIFI API. See more details [here](https://docs.aws.amazon.com/freertos/latest/lib-ref/html2/wifi/index.html).

#### OTA PAL for Espressif ESP32-DevKitC

- Changed default configuration for number and size of blocks to be compatible with the size of the mbedTLS input buffer.

#### OTA PAL for Espressif ESP-WROVER-KIT

- Changed default configuration for number and size of blocks to be compatible with the size of the mbedTLS input buffer.

#### OTA PAL for Microsoft Windows Simulator

- Changed default configuration for number and size of blocks to be compatible with the size of the mbedTLS input buffer.

#### OTA PAL for Microchip ATECC608A with Windows Simulator

- Changed default configuration for number and size of blocks to be compatible with the size of the mbedTLS input buffer.

#### OTA PAL for Microchip Curiosity PIC32MZEF

- Changed default configuration for number and size of blocks to be compatible with the size of the mbedTLS input buffer.

## 202007.00 July 2020

### New Features

#### Over the Air Update V1.2.0

- Updated the over-the-air (OTA) agent with the ability to pause and resume an in-progress update.
- Updated the OTA demo to demonstrate how to suspend an in-progress OTA update should the MQTT connection disconnect, then resume the same update when the MQTT connection reconnects. In line with best practice, the reconnect logic uses an exponential backoff and jitter algorithm to prevent the MQTT server getting overwhelmed with connection requests if a fleet of devices get disconnected and then attempt to reconnect at the same time.
- For testing purposes only, it is now possible to use the OTA agent to downgrade a version number or revert to an older version.  This new functionality is disabled by default.

#### New Board: Cypress PSoC 64 Standard Secure AWS Wi-Fi Bluetooth Pioneer Kit
- New Board: The <b>Cypress PSoC 64</b> board is now qualified with FreeRTOS.

#### New Board: ESP32-WROOM-32SE

- New Board: <b>ESP32-WROOM-32SE</b> is now available in the FreeRTOS Console.

### Updates

#### FreeRTOS+POSIX Utils V1.2.0

- Update the UTILS_TimespecAdd utility function to support both signed and unsigned definitions of `struct timespec.tv_sec`. (Some implementations use unsigned definition of `struct timespec.tv_sec` to address the *2038* problem on 32-bit systems. ) This change is backwards compatible.

#### MQTT Client Library V2.2.0

- Improved the Keep-Alive mechanism: The MQTT library will not send PING requests when connection is not idle, which fixes a disconnect issue during OTA. In prior versions, MQTT would sometimes disconnect during OTA due to timeouts for server PING response.
- Bug fix for Keep-Alive interval: The MQTT library was incorrectly sending PING requests at intervals greater than the keep alive period sent in the CONNECT request. This change fixes the problem.

#### Secure Sockets LwIP V1.2.0

- Fix invalid memory access - ss_ctx_t context is shared and sent to a user callback. If the socket is closed and subsequently freed during callback execution, the callback can potentially access the invalid context.
- Fix two separate issues for potential invalid memory access; at one place by validating socket handle before de-referencing, and at another place by freeing memory only if it had been previously allocated.

#### PKCS#11 V2.1.0

- Added doxygen to various PKCS #11 files.
- Added improved logging for mbed TLS  return codes in iot_pkcs11_mbedtls.c.

#### Bluetooth Low Energy (BLE) Hardware Abstraction Library (HAL) V5.0.0

- Change status parameter type for GATT Client callbacks to match Bluetooth 5.0.
- Add pxMultiAdvSetInstRawData API to set raw advertisement data for multi-advertisement instances.
- Mark pxBondedCb callback and ucTimeout variables as deprecated.

#### Bluetooth Low Energy Management Library V2.1.0

- Added new API IotBle_SetDeviceName() to set the BLE device name at runtime.
- Fixed IotBle_On() and IotBle_Off() APIs.
- Accommodate larger-than-expected writes to RXLargeMesg Gatt characteristic.

#### FreeRTOS+TCP V2.3.0

- Added ability to cache multiple IP addresses per DNS entry.
- Defensive security improvements:
    - In compliance with the UDP protocol specification, prior versions of FreeRTOS+TCP accepted UDP packets that had their checksum set to 0. FreeRTOS+TCP V2.3.0 adds a new configuration parameter, `ipconfigUDP_PASS_ZERO_CHECKSUM_PACKETS`, that enables users to opt to drop UDP packets that have their checksum set to 0. **Note:** This new setting defaults to 0, so it defaults to dropping UDP packets that have their checksum set to 0.
    - Prior versions of FreeRTOS+TCP accept IP packets that contain IP options, although those options are not processed. FreeRTOS+TCP V2.3.0 adds a new configuration parameter, `ipconfigIP_PASS_PACKETS_WITH_IP_OPTIONS`, that enables users to opt to drop IP packets that contain IP options.
    - Setting configuration parameter, `ipconfigDRIVER_INCLUDED_RX_IP_CHECKSUM`, to 1 offloads IP checksum and length checking to the hardware. From FreeRTOS+TCP V2.3.0, the length is checked in software even when it has already been checked in hardware.

#### Mbed TLS v2.16.7

- Upgraded the version of Mbed TLS to v2.16.7.
- Replaced copy of Mbed TLS with a submodule reference to the official [Mbed TLS GitHub repository](https://github.com/ARMmbed/mbedtls/tree/mbedtls-2.16.7).

#### Over the Air Update V1.2.0

- Fixed an issue encountered when an OTA job is force cancelled while the related download is in progress. It was caused due to the self-start timer starting after the OTA job document is received. The fix starts the self-start timer when the OTA agent on the device starts.

#### Espressif

- Support OTA via HTTP over the BLE channel for ESP32 (when SPIRAM is enabled).
- Added ESP-IDF component for WiFi provisioning in SoftAP mode. This allows provisioning devices with Wi-Fi credentials via a web-server running on the device and a provisioning mobile application.  This mode requires the use of lwIP as the networking stack.
- Replaced ESP-IDF code to be a submodule pointer to the official ESP-IDF repository.
- Updated LwIP as the default networking stack.

## 202002.00 2/18/2020

### New Features

#### FreeRTOS kernel V10.3.0
- Includes FreeRTOS kernel V10.3.0
- Additional details can be found here: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/V10.3.0-kernel-only/History.txt

#### Product Name Change
- The name 'Amazon FreeRTOS' has been changed to 'FreeRTOS'
- The name 'Amazon FreeRTOS Qualification (AFQ)' has been changed to 'Device Qualification Program for FreeRTOS'
- Changes were applied only to comments and headers

### Updates
### Cmake
- Fix a dependency parsing bug in CMake

## 201912.00 12/17/2019

### New Features

#### Over the Air Update V1.1.0
- Refactor OTA agent and separate data transfer and control interface.
- Support OTA data transfer over HTTP.

#### Common I/O Library V0.1.0
- A preview of Common I/O library is included.
- Common I/O library provides a set of standard APIs, across supported reference boards, for accessing common devices.
- This preview supports UART, SPI and I2C. Support for other peripherals will follow.

### Updates
#### lwIP
- Enable lwIP full duplex feature
    - The same socket may be used by multiple tasks concurrently
- lwIP is now a submodule from https://github.com/lwip-tcpip/lwip.git

#### Ethernet Support on Nuvoton NuMaker-IoT-M487
- Ethernet is now available for Nuvoton NuMaker-IoT-M487.
- Either Wi-Fi or Ethernet can be enabled exclusively.

#### HTTPS Client Library V1.1.1
- Documentation updates and additional debug logging.

#### Greengrass Discovery V2.0.0
- Remove demo configuration dependency from library files.
- Update demonstration code to use MQTT V2 APIs.

#### Bluetooth Low Energy V4.0.0
- HAL modification to replace bonded callback with pairing state change callback.
- HAL modification to change to 16-bit value for advertisement duration.

## 201910.00 10/29/2019

### New Features
#### New Board: Infineon XMC4800 IoT Connectivity Kit with OPTIGA Trust X
- The Infineon XMC4800 IoT Connectivity Kit with OPTIGA Trust X secure element is now qualified for Amazon FreeRTOS.

#### New Board: Microchip ATECC608A with Windows Simulator
- The Microchip ATECC608A secure element with Windows Simulator is now qualified for Amazon FreeRTOS.

#### Defender Client Library V3.0.0
- Defender Library API change to shared MQTT connection.
- Demonstration code for Defender Library updated to show use of shared connection.

#### HTTPS Client Library V1.1.0
- Upload support with HTTP methods PUT and POST is now available in the HTTPS Client Library.
- Demonstration code with PUT and POST methods are now available in demos/https.

#### SoftHSMv2 port for PKCS #11
- A port for SoftHSMv2, a third-party open-source implementation of PKCS #11, has been provided for use with the Windows Simulator project for Amazon FreeRTOS.
- The purpose of the SoftHSMv2 port is to allow ad hoc interoperability testing with an independent implementation of the PKCS #11 standard.

#### CMake Builds
- CMake build is now supported for Nordic nRF52840 DK Development kit.

### Updates
#### FreeRTOS Kernel
- FreeRTOS kernel is now a submodule from https://github.com/FreeRTOS/FreeRTOS-Kernel.git

#### HTTP Parser
- Add nodejs/http_parser as a submodule in place of copied-over files.

#### Unity
- Add ThrowTheSwitch/Unity as a submodule in place of copied-over files.

#### PKCS #11
- Update the PKCS #11 third-party headers to be the latest from OASIS (version 2.40 Plus Errata 01).
- Add PKCS #11 as a submodule.

#### Bluetooth Low Energy
- ESP32 board supports NIMBLE as the default underlying stack for Bluetooth Low Energy.
- Contains bug fixes and enhancements in ESP NIMBLE stack.
- Contains bug fixes in ESP Bluedroid stack.

#### Marvell SDK Update
- Marvell SDK now updated to R8 P2 (v1.2.r8.p2)

#### Bluetooth Low Energy
- ESP32 board supports NIMBLE as the default underlying stack for Bluetooth Low Energy.
- Contains bug fixes and enhancements in ESP NIMBLE stack.
- Contains bug fixes in ESP Bluedroid stack.

## 201908.00 08/26/2019

### New Features
#### New Board:Nuvoton NuMaker-IoT-M487
- Nuvoton NuMaker-IoT-M487 is now qualified for Amazon FreeRTOS.
- Disclaimer on RNG: The random number generation solution in this port is for demonstration purposes only.

#### FreeRTOS Kernel V10.2.1
- Kernel version for Amazon FreeRTOS is updated to V10.2.1.
- Add ARM Cortex-M23 (ARMv8-M) GCC/ARMclang and IAR ports.
- Add support to automatically switch between 32-bit and 64-bit cores to RISC-V port.

#### HTTPS Client Library V1.0.0
- The HTTPS Client library for Amazon FreeRTOS supports the HTTP/1.1 protocol over TLS.
- The current request methods supported are GET and HEAD.
- Examples demonstrate downloading a file from S3 using GET with a pre-signed URL.

### Updates
#### PKCS #11
- Update the Amazon FreeRTOS mbedTLS-based PKCS #11 implementation, tests, demos, and PKCS #11 consuming libraries for compliance with standard.
- Add PKCS #11 wrapper functions for easy use of commonly grouped PKCS #11 calls.

#### Demo specific stack size and priority
- Make stack size and priority to be demo specific. In current release all demos have same stack size and priority. This change will make stack size and priority configurable for each demo. Demo can use default stack size/ priority or define its own.
#### Ethernet for Microchip Curiosity PIC32MZEF
- Update Microchip Curiosity PIC32MZEF project and configuration files to support Ethernet connectivty. Developers must define PIC32_USE_ETHERNET at the project level to use Ethernet instead of Wi-Fi.
#### lwIP 2.1.2
- Update lwIP to version 2.1.2, and change existing ports as necessary.

### Test Updates
- Remove elliptic curve tests from "quarantine" test group and add them back to the TLS test group.

#### AWS OTA Agent
- OTA Callback changes for custom and secondary processor jobs. Modifying the OTA Agent to use callback structure instead of directly calling PAL functions. This allows users to pass in custom callbacks for the PAL functions.

## 201906.00 Major 06/17/2019
### Release Versioning
- Move Amazon FreeRTOS to a new versioning scheme (YYYYMM.NN [optional "Major" tag]), while retaining semantic versioning (x.y.z) used for individual libraries within Amazon FreeRTOS. This release contains multiple major version updates for individual libraries. See below for details.

### Folder Structure
- Update folder structure to provide a cleaner separation between FreeRTOS kernel, standard libraries, AWS libraries, platform-specific ports and 3rd party libraries. Customers upgrading from earlier versions will need to update their project files.

### New Features
#### Bluetooth Low Energy Management Library V1.0.0
- Bluetooth Low Energy management API for GAP and GATT services, with support for
    - Bluetooth Low Energy v4.2 and above.
    - Device discovery, notifications and indications.
    - Creating, starting, stopping, and deleting GATT services.
    - “Just Works” and “Secure Connections - Numeric Comparison” connection methods.
- Companion device SDK 1.0.0 release for
    - Android https://github.com/aws/amazon-freertos-ble-android-sdk/
    - iOS https://github.com/aws/amazon-freertos-ble-ios-sdk/
- GATT services for
    - Device information.
    - Wi-Fi credentials provisioning.
    - MQTT-over-Bluetooth Low Energy through Android or iOS device proxy to support.
        - OTA and Device Shadow functionality.

#### MQTT Library V2.0.0, Device Shadow Library V2.0.0, and Device Defender Library V2.0.0
- Enable consistent re-use pattern of one single connection across all libraries.
- Add support for MQTT 3.1.1 standard features.
    - Last Will and Testament.
    - QoS1 with randomized retry logic.
    - Persistent sessions.
- Add programming model revisions to enable.
    - Fully non-blocking programming model.
    - Per-operation user callback.
    - Fully dynamic or fully static memory management.
    - Full support for Bluetooth Low Energy transport as well as TCP/IP.
    - Re-implementable abstraction layer to allow port on any network stacks.
    - Standard, configurable logging mechanism.
- Extend Device Defender support to additional development boards. Current set of metrics now available on all development boards that implement Secure Sockets abstraction.

#### Task Pool library V1.0.0
- Task (Thread) pool library for asynchronous processing.

#### Atomic Operations Library V1.0.0
- Add library for atomic operations support.

### Updates
#### Wi-Fi Management Library V1.0.3
- Add new API ```WIFI_RegisterNetworkStateChangeEventCallback``` to allow application notifications for Wi-Fi state transitions.

#### CMake Builds
- Extend the ability to build projects using CMake in addition to providing IDE project files. CMake files are now available for the following development boards:
    - Espressif ESP32-DevKitC
    - Espressif ESP-WROVER-KIT
    - Infineon XMC4800 IoT Connectivity Kit
    - Marvell MW320 AWS IoT Starter Kit
    - Marvell MW322 AWS IoT Starter Kit
    - Microchip Curiosity PIC32MZEF Bundle
    - STMicroelectronicsSTM32L4 Discovery Kit IoT Node
    - Texas Instruments CC3220SF-LAUNCHXL
    - Microsoft Windows Simulator

### Updates
- mbedTLS library is upgraded to version 2.16.0.
- ESP-IDF version is upgraded to 3.1.3.
- Update demo projects for cleaner separation of platform specific code.
- Documentation update.

## V1.4.8 05/21/2019
### New Features
#### New Boards: Marvell MW320 and MW322
- Marvell boards MW320 and MW322 are now qualified for Amazon FreeRTOS.
- Disclaimer on RNG: The random number generation solution in this port is for demonstration purposes only.

#### FreeRTOS Kernel V10.2.0
- Kernel version for Amazon FreeRTOS is updated to V10.2.0.
- Add Support for RISC-V.
- Include pre-existing ARM Cortex-M33 (ARMv8-M) GCC/ARMclang and IAR ports.

### Updates

#### Greengrass Discovery V1.0.4
- Include C runtime header for snprintf.
- Sanity check the number of bytes written.
- Thing name can be a non-string literal.

#### MQTT Agent V1.1.5
- Set the socket to block on sends with a timeout in prvSetupConnection.

#### Secure Sockets for FreeRTOS+TCP V1.1.6
- ulApplicationGetNextSequenceNumber is now thread safe without stopping the scheduler.
- Leave the scheduler running during PKCS #11 calls.

#### Wi-Fi for ESP32-DevKitC ESP-WROVER-KIT V1.0.2
- lib/wifi: fix issue with WiFi configuration for non-null strings, and fix scanning failure under certain disconnect scenarios.
- ib/FreeRTOS-Plus-TCP: do not send eNetworkDownEvent to stack if interface is already down.
- mbedtls: configurable options for controlling dynamic memory allocations.
- lib/third_party: update ESP-IDF to latest v3.1.3 release.
- NetworkInterface: check interface state before sending packets to WiFi driver.
- Fix WIFI_GetMac returning wrong mac address.

#### PKCS #11 PAL for Cypress CYW943907AEVAL1F development kit V1.0.1
- Fix Cypress build error with IDE.

#### PKCS #11 PAL for Cypress CYW954907AEVAL1F development kit V1.0.1
- Fix Cypress build error with  IDE.

#### FreeRTOS+TCP V2.0.11
- Make RST packet handling more robust.
- Make TCP window high and low watermark thresholds runtime configurable.
- Fix parsing of the last option in a DHCP response packet.
- Fix TCP window size calculation.
- Allow the DNS cache to be programmatically cleared.
- Free the memory allocated by the pcap_compile routine in the WinPCap network interface module.

#### Shadow V1.0.6
- Add a debug message in the event that JSMN runs out of memory trying to parse JSON.
- Print a debug message for any JSMN error, not just 'JSMN_ERROR_NOMEM'.

#### PKCS #11 PAL for Windows Simulator V1.0.4
- Update to permit multithreaded read from object storage.

#### OTA Agent V1.0.2
- Update documentation.

#### TLS V1.1.4
- TLS_Send now handles the error condition when space is not avaiable.
- Convert errors in TLS wrapper to negative error codes.

#### Curiosity PIC32MZEF Linker Update for XC32 Compiler
- The latest XC32 compiler (version 2.15) does not allow multiple definitions by default. Explicitly enabling multiple definitions in aws_tests and aws_demos projects for now.

## V1.4.7 02/18/2019
### New Features
### New Boards: Cypress CYW43907 and CYW54907
- Cypress boards CYW54907 and CYW43907 are now qualified for Amazon FreeRTOS.

#### FreeRTOS Kernel V10.1.1
- Kernel version for Amazon FreeRTOS is updated to 10.1.1.
- Update all object handles (TaskHandle_t, QueueHandle_t, etc.) to be unique types instead of void pointers, improving type safety.
- Add Xtensa port.
- Update to the latest trace recorder code.
- Update lint checked MISRA compliance to use the latest MISRA standard.
- Add configUSE_POSIX_ERRNO to enable per task POSIX style errno functionality.

### Updates

#### FreeRTOS+POSIX V1.0.3
- Use stack based alloaction for POSIX types. Stack based allocation will reduce heap fragmentation.
- Fixed potential overflow in Posix timespec utils.
- Stopped Posix timer spawnning thread every time it is invoked.
- Unlock and update owner atomically, while unlocking mutex.

#### MQTT Agent V1.1.4
- Bug fix: MQTT agent tries to setup a connection with the MQTT broker when the socket does not exist.

####  Upgrading ESP-IDF to 3.1.1
- ESP-IDF upgraded to 3.1.1.

#### OTA PAL for Curiosity PIC32MZEF V1.0.3
- Rename variables to comply with style guidelines.

#### OTA PAL for Windows Simulator V1.0.2
- Rename variables to comply with style guidelines.

#### OTA PAL for CC3220SF-LAUNCHXL V1.0.1
- Rename variables to comply with style guidelines.

#### OTA Agent V1.0.1
- Rename variables to comply with style guidelines.

#### PKCS #11 PAL for Cypress CYW943907AEVAL1F development kit V1.0.0
- Added as part of Cypress CYW43907 board port.
- Note that the random number generation solution in this port is provided for demonstration purposes only. See the comment in lib/pkcs11/portable/cypress/CYW943907AEVAL1F/hw_poll.c.

#### PKCS #11 PAL for Cypress CYW954907AEVAL1F development kit V1.0.0
- Added as part of Cypress CYW54907 board port.
- Note that the random number generation solution in this port is provided for demonstration purposes only. See the comment in lib/pkcs11/portable/cypress/CYW954907AEVAL1F/hw_poll.c

#### PKCS #11 PAL for ESP32-DevKitC ESP-WROVER-KIT V1.0.2
- Updated as part of Updrade to ESP-IDF to 3.1.1.

#### mbedTLS-based PKCS#11 V1.0.7
- Bug Fix: Multi-threaded use of PKCS #11 Sign/Verify could cause key corruption.

#### Wi-Fi for Cypress CYW943907AEVAL1F development kit V1.0.0
- Added as part of Cypress CYW43907 board port.

#### Wi-Fi for Cypress CYW954907AEVAL1F development kit V1.0.0
- Added as part of Cypress CYW54907 board port.

#### Wi-Fi for ESP32-DevKitC ESP-WROVER-KIT V1.0.1
- Update as part of Updrade to ESP-IDF to 3.1.1.

#### Wi-Fi for LPC54018 IoT Module V1.0.3
- Bug fix: Update WIFI init API to return success if WIFI module was already successfully initialized.
- Bug fix: Update WIFI AP connection API for NXP to check if DHCP was successful.

## V1.4.6 12/27/2018
### New Features

#### New Board: Renesas Starter Kit+ for RX65N-2MB
The Renesas Starter Kit+ for RX65N-2MB is now qualified for Amazon FreeRTOS. This port updates the PKCS #11 portable layer, demo projects, and tests.

### Updates

#### FreeRTOS+POSIX V1.0.2
- Improvement to reduce the size of a pthread object and make the object user allocatable from stack.

#### FreeRTOS+TCP V2.0.10
- Add FreeRTOS+TCP support for the Renesas Starter Kit+ for RX65N-2MB.

#### FreeRTOS Kernel V10.0.1
- Add FreeRTOS Kernel support for the Renesas Starter Kit+ for RX65N-2MB.

#### PKCS #11 PAL for MT7697Hx-Dev-Kit V1.0.1
- Update license information.

#### PKCS #11 PAL for Renesas Starter Kit+ for RX65N-2MB V1.0.0
- Add PKCS #11 support for the Renesas Starter Kit+ for RX65N-2MB.
- Note that the random number generation solution in this port is provided for demonstration purposes only. See the comment in lib/third_party/mcu_vendor/renesas/amazon_freertos_common/entropy_hardware_poll.c for more information.

#### Wi-Fi for MT7697Hx-Dev-Kit V1.0.1
- Update license information.

## V1.4.5 12/13/2018
### New Features

#### New Board: MediaTek MT7697Hx-Dev-Kit
The MediaTek MT7697 System on Chip (SoC) is now qualified for Amazon FreeRTOS. You can take advantage of Amazon FreeRTOS features and benefits using the MediaTek MT7697Hx Development Kit available from MediaTek Labs.  This development board contains the MT7697 SoC, which includes an Arm Cortex-M4 MCU, low power 1T1R 802.11 b/g/n Wi-Fi, Bluetooth 4.2 subsystem and power management unit.

#### lwIP Support
Amazon FreeRTOS support for the MediaTek MT7697Hx-Dev-Kit includes for the first time support for the Lightweight TCP / IP network stack (lwIP). This flexibility will support customer choice in identifying the best TCP stack solution for IoT devices.

### Updates

#### FreeRTOS+TCP V2.0.9
- Update to flush ARP cache when then network is down.

#### mbedTLS-based PKCS#11 V1.0.6
- Delete extra include headers.

#### PKCS #11 PAL for MT7697Hx-Dev-Kit V1.0.0
- Add PKCS #11 support for the MediaTek MT7697Hx-Dev-Kit.

#### Secure Sockets for FreeRTOS+TCP V1.1.5
- Update documentation.

#### Secure Sockets for lwIP V1.0.0
- Add Secure Sockets support for lwIP.

#### Wi-Fi for Infineon XMC4800 IoT Connectivity Kit V1.0.1
- Update documentation.

#### Wi-Fi for MT7697Hx-Dev-Kit V1.0.0
- Add Wi-Fi support for the MediaTek MT7697Hx-Dev-Kit.

## V1.4.4 11/19/2018
### Updates

#### Device Defender Demo V1.4.4
- Remove warnings in Device Defender Demo build.

#### Microchip "OTA Demo" Factory Image Script
- Fix post-build command and python script for generating OTA factory image for Mac users.

#### Device Defender Agent V1.0.2
- Update formatting and build warnings.

#### OTA PAL for ESP32-DevKitC ESP-WROVER-KIT V1.0.2
- Fix bug in retrieving code signature verification certificate.

#### OTA PAL for Curiosity PIC32MZEF V1.0.2
- Fix bug in retrieving code signature verification certificate.

## V1.4.3 11/07/2018
### New Features

#### New Board: Xilinx Zynq-7000 based MicroZed Industrial IoT Bundle
- Update Amazon FreeRTOS with port files, demo projects, and tests for the Xilinx Zynq-7000 based MicroZed Industrial IoT Bundle

### Updates

#### mbedTLS Library
- Upgrade to mbedTLS version 2.13.1.

#### FreeRTOS+POSIX V1.0.1
- Minor bug fixes.

#### FreeRTOS+TCP V2.0.8
- Update the Zynq-7000 portable layer for receive descriptor alignment.

#### PKCS #11 Updates
Update mbedTLS-based PKCS #11, and PKCS #11 PAL. These changes have been made to more closely align with the PKCS #11 standard, respond to feedback from users and partners, and make it easy for users to go to production from a prototype.
Applications calling into PKCS #11 functions directly (rather than indirectly via an Amazon provided secure sockets or TLS layer) may experience breaking changes.

##### mbedTLS-based PKCS #11
- C_Initialize handles initialization of randomness in an effort to minimize entropy generation (or seed access) every time sessions are created and destroyed.  To protect random values, thread safety has been enabled in mbedTLS.
- C_SignInit and C_VerifyInit utilize the key handle that is passed in, rather than the first key found in order to comply with the PKCS #11 standard
- C_FindObject APIs no longer instantiate keys from the aws_clientcredential_keys.h header file if keys are not found. This removes the dependency of PKCS #11 on values that will be unique per-device (a transition step for enabling production-scale provisioning). Note that calling vDevModeKeyProvisioning() is now necessary to provision the device.
- C_FindObject PKCS #11 objects can be looked up by CKA_LABEL, in order to provide a standard-compliant object lookup.  Note that pkcs11configFILE_NAME_* configurations have been removed from aws_pkcs11_config.h, see aws_pkcs11.h for pkcs11configLABEL_* defines to access labels, and aws_pkcs11_pal.c for pkcs11palFILE_NAME_* defines.
- C_FindObject and C_GetAttributeValue accept different attribute arguments.
- C_CreateObject requires DER encoded certificates and keys instead of PEM formatted and object attributes required for creating objects have changed.  Note that vDevModeKeyProvisioning() has been updated to supply required attributes and convert inputs from DER to PEM if necessary.
- C_GenerateKeyPair now stores keys in non-volatile memory.
- C_Finalize is no longer invoked by helper functions to prevent threads from interfering with each other's PKCS #11 instances.
- Some error codes have been changes to better match the PKCS #11 standard.
- aws_tls.c and PKCS #11 AFQP tests have updates to reflect these changes.
    - mbedTLS-based PKCS #11 V1.0.5
    - TLS V1.1.3

##### PKCS #11 PAL for mbedTLS-based PKCS #11
- Breaking changes were made to PAL PKCS #11 functions in order to transition from file-centric API to object handle and object label based API.
    - PKCS #11 PAL for ESP32-DevKitC ESP-WROVER-KIT V1.0.1
    - PKCS #11 PAL for XMC4800 IoT Kit V1.0.1
    - PKCS #11 PAL for Curiosity PIC32MZEF V1.0.4
    - PKCS #11 PAL for LPC54018 IoT Module V1.0.3
    - PKCS #11 PAL for Windows Simulator V1.0.3
    - PKCS #11 PAL for STM32L4 Discovery kit IoT node V1.0.3
    - PKCS #11 PAL for Xilinx Zynq MicroZed V1.0.0 (new)

##### PKCS #11 for CC3220SF-LAUNCHXL
- Updates to match behavior of mbedTLS-based PKCS #11.
- mbedTLS added to support conversion between DER and PEM objects.  Note that after provisioning the device, mbedTLS and provisiong PKCS #11 functions may be removed to reduce code size.
    - PKCS #11 PAL for CC3220SF-LAUNCHXL V1.0.3

##### OTA PAL Updates
- The OTA PALs for the Curiosity PIC32MZEF and ESP32-DevKitC ESP-WROVER-KIT boards have been modified to utilize PKCS #11 API to retrieve code signing keys, rather than calling into PKCS #11 PAL functions.
    - OTA PAL for Curiosity PIC32MZEF V1.0.1
    - OTA PAL for ESP32-DevKitC ESP-WROVER-KIT V1.0.1

#### Secure Socket for FreeRTOS+TCP V1.1.4
- Minor update to handle PKCS #11 error codes.
- Update formatting.

#### Secure Sockets for Infineon XMC4800 IoT Connectivity Kit V1.0.1
- Fix the license header from Secure Socket to Secure Sockets.

#### Secure Sockets for STM32L4 Discovery kit IoT node V1.0.0 Beta 4
- Bug fix to support Amazon Trust Services endpoints. For more information, please see https://aws.amazon.com/blogs/iot/aws-iot-core-ats-endpoints/.

#### Secure Sockets for CC3220SF-LAUNCHXL V1.0.5
- Remove duplicate file name definitions.  See iot_secure_sockets_config.h for file name defines.

#### Shadow V1.0.5
- Minor bug fixes.


## V1.4.2 10/17/2018
### New Features

#### New Board: Infineon XMC4800 IoT Connectivity Kit
Update Amazon FreeRTOS with port files, demo projects, and tests for the Infineon XMC4800 IoT Connectivity Kit.

### Updates

#### Update pthread Implementation in ESP-IDF
Incorporate an update to Espressif's ESP-IDF which improves the implementation of pthread.

#### Build Improvement: MPLAB PIC32 Projects
Resolve several warnings in the MPLAB project builds, and update the projects to no longer assume that the XC32 compiler is in the host computer's path.

#### Move Helper Utilities to the 'tools/' Directory
Move a few utilities to the root-level 'tools/' directory, from the 'tests/common/tools/' and 'demos/common/tools/' directories.

#### Styling Updates
Improve consistency of Hungarian Notation usage, update a number of methods to use 'void' instead of an empty argument list, and update the style guide.

#### New POSIX Functions
Add POSIX functions including `time`, `localtime_r`, and `strftime`.

#### Refactor Device Defender Direcory Structure
Update Device Defender code to use the same 'lib/' and 'include/' directory structures as the other libraries.

#### AFQP Documentation Updates
Update the Amazon FreeRTOS Qualification Program's documentation to reflect updated directory structures.

## V1.4.1 08/29/2018
### New Features
None

### Updates

#### OTA PAL for Windows Simulator V1.0.1
- Update Amazon FreeRTOS Windows Simulator to use ECDSA/SHA-256 for OTA image verification.


## V1.4.0 08/27/2018
### New Features

#### Demo Bootloader for Microchip Curiosity PIC32MZEF V1.0.0
The demo bootloader supports Amazon FreeRTOS over-the-air (OTA) update by implementing firmware version checking, cryptographic signature verification, and application self-test. The firmware verification includes verifying the authenticity and integrity of the new firmware received over the air. The bootloader verifies the cryptographic signature of the application before boot. The elliptic-curve digital signature algorithm (ECDSA) with SHA256 is used. The utilities provided can be used to generate a signed application that can be flashed on the device. This enables signature verification of the initial image.

### Updates

#### Over-the-air (OTA) updates
The over-the-air (OTA) updates feature of Amazon FreeRTOS is now generally available. The release includes enhancements to the OTA Agent and changes to the OTA Portable Abstraction Layer (PAL) interface.

#### OTA PAL for Curiosity PIC32MZEF V1.0.0
Update for API changes for OTA general availability release.

#### OTA PAL for Windows Simulator V1.0.0
Update for API changes for OTA general availability release.

#### OTA PAL for CC3220SF-LAUNCHXL V1.0.0
Update for API changes for OTA general availability release.

#### OTA PAL for Espressif ESP32 V1.0.0
Update for API changes for OTA general availability release.

#### OTA Agent V1.0.0
Enhancements and API changes for OTA general availability release.


## V1.3.2 08/21/2018
### New Features
None

### Updates

#### FreeRTOS+TCP
- Multiple security improvements and fixes in packet parsing routines, DNS caching, and TCP sequence number and ID generation.
- Disable NBNS and LLMNR by default.
- Add TCP hang protection by default.

#### Secure Sockets for FreeRTOS+TCP
- Improve security in Amazon FreeRTOS Secure Sockets usage of mbedTLS and ALPN.

We thank Ori Karliner of Zimperium zLabs Team for reporting these issues.


## V1.3.1 08/10/2018
### New Features
None

### Updates

#### OTA Beta Update
- Updates to OTA Beta to incorporate a new API for the OTA service. This API is not compatible with the  API used in the original OTA Beta released on Dec. 20th.
- Add a "reference bootloader" for use in the OTA process. This bootloader is for use with the Microchip MCU.

#### Amazon FreeRTOS Qualification Program (AFQP) Update
- Update AFQP documentation. For more info on the changes to AFQP, reference the Revision History of the "Amazon FreeRTOS Qualification Program Developer Guide" in the "tests" directory.

#### Device Defender Update
- Add a demo to illustrate the operation of Device Defender for the Windows Simulator and Microchip PIC32MZEF MCU.

#### TI "Hello World" Build Failure Resolution
- Address an issue where the "Hello World" demo did not build with TI CCS 7.3 in AFR 1.3.0 when downloaded from OCW.


## V1.3.0 07/31/2018

### New Features

#### AFQP 1.0 Support
This release of AFR has support for vendors who wish to have their ports qualified for Amazon FreeRTOS through the Amazon FreeRTOS Qualification Program (AFQP). This is the first public release of AFQP tests and documentation. A new top level "tests" directory is added to support this functionality. AFQP documents are available in "tests" directory. [Learn more.](https://docs.aws.amazon.com/freertos/latest/userguide/freertos-qualification-program.html)

#### Device Defender 1.0 Support
AWS IoT Device Defender is an AWS IoT security service that allows users to audit the configuration of their devices, monitor connected devices to detect abnormal behavior, and to mitigate security risks. It gives you the ability to enforce consistent security policies across your AWS IoT device fleet and respond quickly when devices are compromised. Device side support of this feature is part of this release. Devices supported are WinSim and Microchip PIC32MZEF. [Learn more.](https://docs.aws.amazon.com/freertos/latest/userguide/afr-device-defender-library.html)

#### FreeRTOS+POSIX 1.0 Interface
This release includes version 1.0.0 of FreeRTOS+POSIX.  FreeRTOS+POSIX is a POSIX compatibility layer that allows existing POSIX applications to run without modifications of FreeRTOS.  This release supports POSIX threads, mutexes, barriers, condition variables, semaphores, message queues, clocks, timers, and error numbers.  While most of the POSIX functions are implemented and up to specification, limitations in the FreeRTOS kernel precluded the standard implementations of certain functions.  The functions which differ from the POSIX specification are identified in their header files. Currently, FreeRTOS+POSIX is only used by drivers of the TI CC3220SF.

### Updates

#### FreeRTOS Kernel V10.0.1
- Add Idle tick counter interface
- Rename posix/ to FreeRTOS_POSIX/

#### FreeRTOS+TCP V2.0.4
- Fix issues raised by the Coverity scan
    - Fix a typo ulRxWindowLength -> ulTxWindowLength in FreeRTOS_Sockets.c
    - Fix strncmp length in FreeRTOS_DNS.c
    - Fix styling in FreeRTOS_ARP.c
- Fix a spelling typo ";east" -> "least" (response from a pull request)
- Add auto check of network interfaces for WinSim

#### MQTT Agent V1.1.2
- Move MQTT metrics to agent

#### mbedTLS-based PKCS #11 V1.0.3
- Reduce the number of warnings generated

#### PKCS #11 for LPC54018 IoT Module V1.0.1
- Change project baudrate setting to resolve AFQP test failures

#### Secure Sockets for NXP54018_IoT_Module V1.0.0 Beta 3
- Update to latest NXP driver to address AFQP 1.0 test failures

#### Secure Sockets for STM32L4 Discovery kit IoT node V1.0.0 Beta 2
- Update to new Inventek driver to resolve AFQP 1.0 test failures

#### Wi-Fi for Curiosity PIC32MZEF V1.0.3
- Change Microchip network param to use a direct address instead of a section to reduce the size of the binary image to allow OTA to continue working.
- Reduce number of warnings generated.

#### Wi-Fi for LPC54018 IoT Module V1.0.1
- Add fixes for Demo and DHCP.

#### Wi-Fi STM32L4 Discovery kit IoT node V1.0.2
- Update for release of AFQP 1.0

#### Wi-Fi for CC3220SF-LAUNCHXL V1.0.2
- Update for release of AFQP 1.0

---------------------------------------------------------------------------------------

## V1.2.7 05/31/2018

- Update the Texas Instruments SimpleLink CC3220SF SDK from version 1.40.01.00 to version 2.10.00.04.
- Fix the MQTT Echo Demo (Hello World Demo) to avoid truncating received strings.
- Modify the Getting Started scripts to check if the AWS CLI is configured.

#### Secure Sockets for CC3220SF-LAUNCHXL V1.0.4
- Update comments for SimpleLink CC3220SF SDK version 2.10.00.04.

--------------

## V1.2.6 05/18/2018

- Fix NXP MCUXpresso project build failure on Linux.

----------------

## V1.2.5 05/14/2018

- Add support for Espressif's ESP32-DevKitC and ESP-WROVER-KIT.

#### FreeRTOS+TCP V2.0.4
 - Add Espressif ESP32 network interface support.

#### mbedTLS-based PKCS #11 V1.0.3
 - Implement C_DigestInit, C_DigestUpdate, and C_DigestFinal for SHA-256.
 - Implement C_GenerateKeyPair for non-persistent ECDSA P256.

#### PKCS #11 for ESP32-DevKitC ESP-WROVER-KIT V1.0.0
 - Add support for Espressif's ESP32-DevKitC and ESP-WROVER-KIT.

#### Wi-Fi STM32L4 Discovery kit IoT node V1.0.2
 - Bug fix to ensure that WIFI_ConnectAP() switches to the network parameters input,
      even when already connected to a different set.

#### Wi-Fi for ESP32-DevKitC ESP-WROVER-KIT V1.0.0
 - Add support for Espressif's ESP32-DevKitC and ESP-WROVER-KIT.

---------------------------------------------------------------------------------------

## V1.2.4 05/01/2018

 - Upgrade to mbedTLS version 2.8.
 - Add MCUXpresso IDE demo project for the NXP LPC54018 IoT Module.

#### Crypto V1.0.2
 - Minor updates due to mbedTLS crypto interface changes.

#### FreeRTOS+TCP V2.0.3
 - Fix a bug where re-transmission and duplicated TCP packets would create a
      computation underflow as well as a memory leak.
 - Add new public function FreeRTOS_UpdateMACAddress() to allow changing the MAC address
      after FreeRTOS_IPInit. Sometimes the device MAC address is not available at the
      time FreeRTOS_IPInit() is called, so it needs to be changed afterward.
 - Remove non-cryptographic rand() implementation.
 - Remove a static variable in functions prvGetHostByName() and prvCreateDNSSocket() to
      make them threadsafe.

#### Greengrass Discovery V1.0.3
 - Provide a helpful error message if the Greengrass Discovery JSON does not fit in
      the supplied buffer.

#### MQTT Agent V1.1.2
 - Bug fix to avoid socket leak if MQTT Connect fails after a successful TCP
      connection.
 - Add support for disabling subscription management feature by defining the macro
      mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT as 0.

#### OTA PAL for Curiosity PIC32MZEF V0.9.1
 - Update for PKCS #11 PAL layer API changes.

#### OTA PAL for Windows Simulator V0.9.2
- Minor restructuring of file locations.

#### OTA PAL for CC3220SF-LAUNCHXL V0.9.3
 - Minor changes to enable test integration.

#### OTA Agent V0.9.4
 - Minor restructuring of file locations.

#### mbedTLS-based PKCS #11 V1.0.2
 - Combine the mbedTLS based PKCS #11 implementation from Curiosity PIC32MZEF,
      LPC54018 IoT Module, Windows Simulator, and STM32L4 Discovery kit IoT node into a
      single file.
 - Add support for public key verification of signatures.
 - Fix to free context structures on session failure.
 - Update C_OpenSession to use CKF_SERIAL_SESSION flag.

#### PKCS #11 for Curiosity PIC32MZEF V1.0.2
 - Create port specific functions for certificate and key access:
      PKCS11_PAL_SaveFile(), PKCS11_PAL_ReadFile(), PKCS11_PAL_ReleaseFileData().

#### PKCS #11 for LPC54018 IoT Module V1.0.1
 - Create port specific functions for certificate and key access:
      PKCS11_PAL_SaveFile(), PKCS11_PAL_ReadFile(), PKCS11_PAL_ReleaseFileData().

#### PKCS #11 PAL for Windows Simulator V1.0.2
 - Create port specific functions for certificate and key access:
      PKCS11_PAL_SaveFile(), PKCS11_PAL_ReadFile(), PKCS11_PAL_ReleaseFileData().

#### PKCS #11 for STM32L4 Discovery kit IoT node V1.0.1
 - Create port specific functions for certificate and key access:
      PKCS11_PAL_SaveFile(), PKCS11_PAL_ReadFile(), PKCS11_PAL_ReleaseFileData().

#### PKCS #11 for CC3220SF-LAUNCHXL V1.0.2
 - PKCS #11 implementation for TI based on mbedTLS moved into this file.

#### Secure Socket for FreeRTOS+TCP V1.1.2
 - Combine Secure Sockets implementation for Curiosity PIC32MZEF and Windows
      Simulator into a single file.
 - Fix return value of SOCKETS_Socket on error.
 - Attempting to create an unsupported UDP socket now triggers an assert.
 - Add cryptographic random number generator function for TCP sequence numbers.
 - Update the Socket structure to keep track of a connection attempt and added
      support of the ECONN error.

#### Secure Sockets for LPC54018 IoT Module V1.0.0 Beta 3
 - Fix minor bug in SOCKETS_Recv().

#### Secure Sockets for STM32L4 Discovery kit IoT node V1.0.0 Beta 2
 - Fix return value of SOCKETS_Close on error.

#### Secure Sockets for CC3220SF-LAUNCHXL V1.0.3
 - Secure sockets printing is now controlled independently using the SOCKETS_PRINT
      macro. SOCKETS_PRINT prints TI driver error codes.

#### Shadow V1.0.3
 - Change names of configuration constants to be consistent with FreeRTOS style.

#### TLS V1.1.1
 - Support AWS IoT Just-in-Time Registration (JITR) by sending optional
      client-issuer certificate.
 - Use CKF_SERIAL_SESSION flag with PKCS #11.

#### Wi-Fi for Curiosity PIC32MZEF V1.0.3
 - Update for setting the MAC Address in WIFI_On() by using new FreeRTOS+TCP
      function FreeRTOS_UpdateMACAddress().
 - Redefine printing and assert stubs used in the Wi-Fi driver code.
 - Add implementation of WIFI_GetMAC().
 - Add implementation of WIFI_IsConnected().
 - Minor bug fixes.

#### Wi-Fi for LPC54018 IoT Module V1.0.2
 - Add implementation of WIFI_IsConnected().
 - Fix max DNS name length redefinition.
 - Fix compiler errors in MCUXpresso IDE.
 - Minor bug fixes.

#### Wi-Fi STM32L4 Discovery kit IoT node V1.0.1
 - Add implementation of WIFI_IsConnected().
 - Add NULL pointer checks throughout.
 - Minor bug fixes.

#### Wi-Fi for CC3220SF-LAUNCHXL V1.0.2
 - Add implementation of WIFI_IsConnected().
 - Add NULL pointer checks throughout.
 - Minor bug fixes.

---------------------------------------------------------------------------------------

## V1.2.3 03/29/2018
 - Fix TI IAR project build failure.

---------------------------------------------------------------------------------------

## V1.2.2 02/27/2018

#### OTA Agent V0.9.3
 - Formatting update.

#### OTA PAL for Curiosity PIC32MZEF V0.9.0
 - Beta release of the OTA Update support for the Microchip Curiosity PIC32MZEF.

#### PKCS #11 for Curiosity_PIC32MZEF V1.0.1
 - Add support for the management of OTA update code signing keys.

#### Wi-Fi for Curiosity PIC32MZEF V1.0.1
 - Update to conditionally compile the entire file.

---------------------------------------------------------------------------------------

## V1.2.1 02/23/2018

 - Add an IAR IDE demo project for the Texas Instruments CC3220SF-LAUNCHXL.
 - Add Wi-Fi support for the Microchip Curiosity PIC32MZEF.

#### FreeRTOS+TCP V2.0.2
 - Improve NULL variable check in buffer management.

#### MQTT Agent V1.1.1
 - Minor bug fix checking for a NULL pointer.

#### OTA Agent V0.9.2
 - Update to support NULL OTA file handles.

#### Amazon FreeRTOS OTA PAL for CC3220SF-LAUNCHXL V0.9.2
 - Update to support NULL OTA file handles.

#### PKCS #11 for CC3220SF-LAUNCHXL V1.0.1
 - Add a dummy variable to a previously empty structure to fix IAR compiler errors.

#### Secure Socket for Windows Simulator V1.1.1
 - Formatting update.

#### Secure Sockets for CC3220SF-LAUNCHXL V1.0.2
 - Update to print SimpleLink driver-specific error codes when debugging.
 - Add error handling for non-blocking sockets.
 - Update socket option to return an error if security options are specified after
      a connection.

#### Wi-Fi for Curiosity PIC32MZEF V1.0.1
 - Update such that Wi-Fi disconnection waits until the link is down before
      returning.

#### Wi-Fi for CC3220SF-LAUNCHXL V1.0.1
 - Fix error in attempting to overwrite a const memory.

---------------------------------------------------------------------------------------

## V1.2.0 02/06/2018

#### Greengrass Discovery V1.0.2
 - Update to send all data until an error is received.

#### MQTT Agent V1.1.0
 - Add support for ALPN. ALPN allows MQTT traffic to be sent to
      the AWS IoT broker over port 443 instead of 8883.

#### OTA Agent V0.9.1
 - Send a FAILED status from agent when a file too large for the platform
      is received.
 - Rename some files.

#### PKCS #11 for Windows Simulator
 - Add developer mode key provisioning support.

#### Secure Socket for Curiosity PIC32MZEF V1.0.1
 - Add support for ALPN.

#### Secure Socket for Windows Simulator V1.1.0
 - Add support for ALPN.

#### Secure Sockets for CC3220SF-LAUNCHXL V1.0.1
- Remove unnecessary server certificate storage on the client side.
- Remove unnecessary global synchronization semaphore.
- Update for other small bugs.

#### Shadow V1.0.2
- Fix error handling bugs.
- Require client tokens.
- Update for other small bugs.

#### TLS V1.1.0
- Add support for ALPN.

---------------------------------------

## V1.1.0 12/20/2017

#### Crypto V1.0.1
- Fix compiler warning for the Microchip Curiosity PIC32MZEF.

#### FreeRTOS+TCP V2.0.1
- Add support for the Microchip Curiosity PIC32MZEF.

#### FreeRTOS Kernel V10.0.1
- Minor bug fixes to allow Linux and C++ builds.

#### Greengrass Discovery V1.0.1
- Reformat console display messages in order to better facilitate demos and debugging.

#### MQTT Agent V1.0.1
- The MQTT Agent now processes commands between successive socket reads to       enable faster command handling, especially when the connected socket is       receiving data continuously.

#### OTA Agent V0.9.0
- Beta release of OTA Update library for Amazon FreeRTOS. Includes support   for the Texas Instruments CC3220SF-LAUNCHXL and Windows Simulator.

#### PKCS #11 for Curiosity PIC32MZEF V1.0.0 Beta 1
- Add support for the Microchip Curiosity PIC32MZEF.

#### Secure Socket for Curiosity PIC32MZEF V1.0.0
- Add support for the Microchip Curiosity PIC32MZEF.

#### Secure Sockets for LPC54018 IoT Module V1.0.0 Beta 2
- Fix bugs in the Secure Sockets implementation for the NXP LPC54018 IoT       Module.

#### Shadow V1.0.1
- Fix compiler warning for the Microchip Curiosity PIC32MZEF.

#### Wi-Fi for LPC54018 IoT Module V1.0.1
- Change the Wi-Fi Connection timeout to 10 seconds.
