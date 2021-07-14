# FreeRTOS AWS Reference Integrations

## Cloning
This repo uses [Git Submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules) to bring in dependent components.

Note: If you download the ZIP file provided by GitHub UI, you will not get the contents of the submodules. (The ZIP file is also not a valid git repository)

If using Windows, set `core.symlinks` to true since copying a directory with symlinks may cause hangups:
```
git config --global core.symlinks true
```

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
2. **STMicroelectronics** - [STM32L4 Discovery kit IoT node](https://devices.amazonaws.com/detail/a3G0L00000AANsWUAX/STM32L4-Discovery-Kit-IoT-Node).
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_st.html)
    * IDE: [STM32 System Workbench](http://openstm32.org/HomePage)
3. **NXP** - [LPC54018 IoT Module](https://devices.amazonaws.com/detail/a3G0L00000AANtAUAX/LPC54018-IoT-Solution), 
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_nxp.html)
    * IDEs: [IAR Embedded Workbench](https://www.iar.com/iar-embedded-workbench/partners/nxp), [MCUXpresso IDE](https://www.nxp.com/mcuxpresso/ide/download)
4. **Espressif** - [ESP32-DevKitC](https://devices.amazonaws.com/detail/a3G0L00000AANtjUAH/ESP32-WROOM-32-DevKitC), [ESP-WROVER-KIT](https://devices.amazonaws.com/detail/a3G0L00000AANtlUAH/ESP-WROVER-KIT), [ESP32-WROOM-32SE](https://devices.amazonaws.com/detail/a3G0L00000AANtjUAH/ESP32-WROOM-32-DevKitC), [ESP32-S2-SAOLA-1](https://devices.amazonaws.com/detail/a3G0h00000AkFngEAF/ESP32-S2-Saola-1)
    * [Getting Started Guide - ESP32-DevKitC, ESP-WROVER-KIT](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_espressif.html)
    * [Getting Started Guide - ESP32-WROOM-32SE](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_esp32wroom-32se.html)
5. **Infineon** - [Infineon XMC4800 IoT Connectivity Kit](https://devices.amazonaws.com/detail/a3G0L00000AANsbUAH/XMC4800-IoT-FreeRTOS-Connectivity-Kit-WiFi), [Optiga TrustX](https://devices.amazonaws.com/detail/a3G0h000007712QEAQ/OPTIGA%E2%84%A2-Trust-X-Security-Solution)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_infineon.html)
    * IDE: [DAVE](https://infineoncommunity.com/dave-download_ID645)
6. **Xilinx** - [Xilinx Zynq-7000 based MicroZed Industrial IoT Bundle](https://devices.amazonaws.com/detail/a3G0L00000AANtqUAH/MicroZed-IIoT-Bundle-with-FreeRTOS)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_xilinx.html)
    * IDE: [Xilinx SDK](https://www.xilinx.com/products/design-tools/embedded-software/sdk.html)
7. **MediaTek** - [MediaTek MT7697Hx Development Kit](https://devices.amazonaws.com/detail/a3G0L00000AAOmPUAX/MT7697Hx-Development-Kit)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_mediatek.html)
    * IDE: [Keil uVision](http://www2.keil.com/mdk5/install/)
8. **Renesas** - [Renesas Starter Kit+ for RX65N-2MB](https://devices.amazonaws.com/detail/a3G0L00000AAOkeUAH/Renesas-Starter-Kit+-for-RX65N-2MB)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_renesas.html)
    * IDE: [e2 studio](https://www.renesas.com/us/en/products/software-tools/tools/ide/e2studio.html)
9. **Cypress CYW54907** - [Cypress CYW954907AEVAL1F Evaluation Kit](https://devices.amazonaws.com/detail/a3G0L00000AAPg5UAH/CYW954907AEVAL1F)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_cypress_54.html)
    * IDE: [WICED Studio](https://community.cypress.com/community/wiced-wifi)
10. **Cypress CYW43907** - [Cypress CYW943907AEVAL1F Evaluation Kit](https://devices.amazonaws.com/detail/a3G0L00000AAPg0UAH/CYW943907AEVAL1F)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_cypress_43.html)
    * IDE: [WICED Studio](https://community.cypress.com/community/wiced-wifi)
11. **Cypress PSoC 64** - [PSoC 64 Standard Secure AWS Wi-Fi Bluetooth Pioneer Kit](https://devices.amazonaws.com/detail/a3G0h0000088AgXEAU/PSoC%C2%AE-64-Standard-Secure-AWS-Wi-Fi-Bluetooth-Pioneer-Kit)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_cypress_psoc64.html)
    * IDE: [ModusToolbox](https://www.cypress.com/products/modustoolbox-software-environment)
12. **NXP MW320** - [MW320 AWS IoT Starter Kit](https://devices.amazonaws.com/detail/a3G0h000000OaRnEAK/MW320-AWS-IoT-Starter-Kit)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_mw32x.html)
13. **NXP MW322** - [MW322 AWS IoT Starter Kit](https://devices.amazonaws.com/detail/a3G0h000000OblKEAS/MW322-AWS-IoT-Starter-Kit)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_mw32x.html)
14. **Nordic nRF52840 DK** - [nRF52840 DK Development kit](https://devices.amazonaws.com/detail/a3G0L00000AANtrUAH/nRF52840-Development-Kit)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_nordic.html)  
15. **Nuvoton** - [NuMaker-IoT-M487](https://devices.amazonaws.com/detail/a3G0h000000Tg9cEAC/NuMaker-IoT-M487)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting-started-nuvoton-m487.html)
16. **Windows Simulator** - To evaluate FreeRTOS without using MCU-based hardware, you can use the Windows Simulator.
    * Requirements: Microsoft Windows 7 or newer, with at least a dual core and a hard-wired Ethernet connection
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_windows.html)
    * IDE: [Visual Studio Community Edition](https://www.visualstudio.com/downloads/)


## amazon-freeRTOS/projects
The ```./projects``` folder contains the IDE test and demo projects for each vendor and their boards. The majority of boards can be built with both IDE and cmake (there are some exceptions!). Please refer to the Getting Started Guides above for board specific instructions.

## Mbed TLS License
This repository uses Mbed TLS under Apache 2.0.

## amazon-freerTOS/vendors License
The `./vendors` directory contains content that may be subject to different license terms. For vendor licensing information, see the LICENSE files or source header documentation for each vendor directory.
