# FreeRTOS AWS Reference Integrations

> :warning: **Preview Branch**: This branch is a temporary branch to track a work in progress feature. It will be removed in the future and should not be used in production projects.

## Cloning
This repo uses [Git Submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules) to bring in dependent components.

Note: If you download the ZIP file provided by GitHub UI, you will not get the contents of the submodules. (The ZIP file is also not a valid git repository)

To clone using HTTPS:
```
git clone https://github.com/aws/amazon-freertos.git --recurse-submodules
```
Using SSH:
```
git clone git@github.com:aws/amazon-freertos.git --recurse-submodules
```

If you have downloaded the repo without using the `--recurse-submodules` argument, you need to run:
```
git submodule update --init --recursive
```

## Important branches to know
master            --> Development is done continuously on this branch  
release           --> Fully tested released source code  
release-candidate --> Preview of upcoming release  
feature/*         --> Alpha/beta of an upcoming feature  

## Getting Started

For more information on FreeRTOS, refer to the [Getting Started section of FreeRTOS webpage](https://aws.amazon.com/freertos).

To directly access the **Getting Started Guide** for supported hardware platforms, click the corresponding link in the Supported Hardware section below.

For detailed documentation on FreeRTOS, refer to the [FreeRTOS User Guide](https://aws.amazon.com/documentation/freertos).

### Running the OTA demo

To run the OTA demo over coreMQTT, set the demo to `CONFIG_OTA_MQTT_UPDATE_DEMO_ENABLED` in `aws_demo_config.h`. 

To run the OTA demo over coreMQTT for control and coreHTTP for download, set the demo to `CONFIG_OTA_HTTP_UPDATE_DEMO_ENABLED` in `aws_demo_config.h`.

To enable the [202011.00 OTA library](libraries/freertos_plus/aws/ota), enable the modules in the CMake files for the [libraries](libraries/CMakeLists.txt) and [vendor boards](vendors/espressif/boards/esp32/CMakeLists.txt). Please see the demo [here](https://github.com/aws/amazon-freertos/tree/master/demos/ota) for how to reference the APIs of the 202011.00bOTA library.
### AWS Collection of Metrics

The demos that connect to AWS IoT report metrics to AWS about the operating system, and the MQTT client library used by sending a specially formatted string in the username field of the MQTT CONNECT packet. These metrics help AWS IoT improve security and provide better technical support. Providing these metrics is optional for users, and these can be disabled by updating the following configuration macros in the `demos/include/aws_iot_metrics.h` file:

```
#define AWS_IOT_METRICS_STRING           NULL
#define AWS_IOT_METRICS_STRING_LENGTH    0U
```

#### Format

The format of the username string with metrics is:

```
<Actual_Username>?SDK=<OS_Name>&Version=<OS_Version>MQTTLib=<MQTT_Library_name>@<MQTT_Library_version>
```

where

* **Actual_Username** is the actual username used for authentication (if a username/password is used for authentication).
* **OS_Name** is the Operating System the application is running on.
* **OS_Version** is the version number of the Operating System.
* **MQTT_Library_name** is the MQTT Client library being used.
* **MQTT_Library_version** is the version of the MQTT Client library being used.

## FreeRTOS Qualified Boards

For a complete list of boards that have been qualified for FreeRTOS by AWS Partners, please visit the [AWS Partner Device Catalog](https://devices.amazonaws.com/search?page=1&sv=freertos)

In addition, AWS supports the following boards with FreeRTOS Build Integration and maintains them with each release:

1. **Texas Instruments** - [CC3220SF-LAUNCHXL](https://devices.amazonaws.com/detail/a3G0L00000AANtaUAH/SimpleLink-Wi-Fi%C2%AE-CC3220SF-Wireless-Microcontroller-LaunchPad-Development-Kit).
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_ti.html)
    * IDEs: [Code Composer Studio](http://www.ti.com/tools-software/ccs.html), [IAR Embedded Workbench](https://www.iar.com/iar-embedded-workbench/partners/texas-instruments)
    * IDE: [STM32 System Workbench](http://openstm32.org/HomePage)
2. **Microchip** - [Curiosity PIC32MZEF](https://devices.amazonaws.com/detail/a3G0L00000AANscUAH/Curiosity-PIC32MZ-EF-FreeRTOS-Bundle).
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_mch.html)
    * IDE: [MPLAB X IDE](http://www.microchip.com/mplab/mplab-x-ide)
3. **Espressif** - [ESP32-DevKitC](https://devices.amazonaws.com/detail/a3G0L00000AANtjUAH/ESP32-WROOM-32-DevKitC), [ESP-WROVER-KIT](https://devices.amazonaws.com/detail/a3G0L00000AANtlUAH/ESP-WROVER-KIT), [ESP32-WROOM-32SE](https://devices.amazonaws.com/detail/a3G0L00000AANtjUAH/ESP32-WROOM-32-DevKitC) 
    * [Getting Started Guide - ESP32-DevKitC, ESP-WROVER-KIT](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_espressif.html)
    * [Getting Started Guide - ESP32-WROOM-32SE](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_esp32wroom-32se.html)
4. **Renesas** - [Renesas Starter Kit+ for RX65N-2MB](https://devices.amazonaws.com/detail/a3G0L00000AAOkeUAH/Renesas-Starter-Kit+-for-RX65N-2MB)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_renesas.html)
    * IDE: [e2 studio](https://www.renesas.com/us/en/products/software-tools/tools/ide/e2studio.html)
5. **Nordic nRF52840 DK** - [nRF52840 DK Development kit](https://devices.amazonaws.com/detail/a3G0L00000AANtrUAH/nRF52840-Development-Kit)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_nordic.html)  
6. **Windows Simulator** - To evaluate FreeRTOS without using MCU-based hardware, you can use the Windows Simulator.
    * Requirements: Microsoft Windows 7 or newer, with at least a dual core and a hard-wired Ethernet connection
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_windows.html)
    * IDE: [Visual Studio Community Edition](https://www.visualstudio.com/downloads/)


## amazon-freeRTOS/projects
The ```./projects``` folder contains the IDE test and demo projects for each vendor and their boards. The majority of boards can be built with both IDE and cmake (there are some exceptions!). Please refer to the Getting Started Guides above for board specific instructions.

## Mbed TLS License
This repository uses Mbed TLS under Apache 2.0
