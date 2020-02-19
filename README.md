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

## Supported Hardware

For additional boards that are supported for FreeRTOS, please visit the [AWS Device Catalog](https://devices.amazonaws.com/search?kw=freertos)

The following MCU boards are supported for FreeRTOS:
1. **Texas Instruments** - [CC3220SF-LAUNCHXL](http://www.ti.com/tool/cc3220sf-launchxl).
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_ti.html)
    * IDEs: [Code Composer Studio](http://www.ti.com/tools-software/ccs.html), [IAR Embedded Workbench](https://www.iar.com/iar-embedded-workbench/partners/texas-instruments)
2. **STMicroelectronics** - [STM32L4 Discovery kit IoT node](http://www.st.com/en/evaluation-tools/b-l475e-iot01a.html).
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_st.html)
    * IDE: [STM32 System Workbench](http://openstm32.org/HomePage)
3. **NXP** - [LPC54018 IoT Module](http://www.nxp.com/LPC-AWS-Module).
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_nxp.html)
    * IDEs: [IAR Embedded Workbench](https://www.iar.com/iar-embedded-workbench/partners/nxp), [MCUXpresso IDE](https://www.nxp.com/mcuxpresso/ide/download)
4. **Microchip** - [Curiosity PIC32MZEF](http://www.microchipdirect.com/product/search/all/dm320104-BNDL).
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_mch.html)
    * IDE: [MPLAB X IDE](http://www.microchip.com/mplab/mplab-x-ide)
5. **Espressif** - [ESP32-DevKitC](https://www.espressif.com/en/products/hardware/esp32-devkitc/overview), [ESP-WROVER-KIT](https://www.espressif.com/en/products/hardware/esp-wrover-kit/overview).
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_espressif.html)
6. **Infineon** - [Infineon XMC4800 IoT Connectivity Kit](https://www.infineon.com/connectivitykit)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_infineon.html)
    * IDE: [DAVE](https://infineoncommunity.com/dave-download_ID645)
7. **Xilinx** - [Xilinx Zynq-7000 based MicroZed Industrial IoT Bundle](http://www.zedboard.org/product/microzed-iiot-bundle-afreertos)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_xilinx.html)
    * IDE: [Xilinx SDK](https://www.xilinx.com/products/design-tools/embedded-software/sdk.html)
8. **MediaTek** - [MediaTek MT7697Hx Development Kit](https://www.mediatek.com/products/smartHome/mt7697h)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_mediatek.html)
    * IDE: [Keil uVision](http://www2.keil.com/mdk5/install/)
9. **Renesas** - [Renesas Starter Kit+ for RX65N-2MB](https://www.renesas.com/us/en/products/software-tools/boards-and-kits/renesas-starter-kits/renesas-starter-kitplus-for-rx65n-2mb.html)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_renesas.html)
    * IDE: [e2 studio](https://www.renesas.com/us/en/products/software-tools/tools/ide/e2studio.html)
10. **Cypress CYW54907** - [Cypress CYW954907AEVAL1F Evaluation Kit](https://www.cypress.com/documentation/development-kitsboards/cyw954907aeval1f-evaluation-kit)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_cypress_54.html)
    * IDE: [WICED Studio](https://community.cypress.com/community/wiced-wifi)
11. **Cypress CYW43907** - [Cypress CYW943907AEVAL1F Evaluation Kit](https://www.cypress.com/documentation/development-kitsboards/cyw943907aeval1f-evaluation-kit)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_cypress_43.html)
    * IDE: [WICED Studio](https://community.cypress.com/community/wiced-wifi)
12. **Marvell MW320** - [Marvell MW320 AWS IoT Starter Kit](https://www.marvell.com/microcontrollers/aws-iot-starter-kit/)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_marvell320.html)
13. **Marvell MW322** - [Marvell MW322 AWS IoT Starter Kit](https://www.marvell.com/microcontrollers/aws-iot-starter-kit/)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_marvell322.html)
14. **Nordic nRF52840 DK** - [nRF52840 DK Development kit](https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52840-DK/)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_nordic.html)  
15. **Nuvoton** - [NuMaker-IoT-M487](https://direct.nuvoton.com/en/numaker-iot-m487)
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting-started-nuvoton-m487.html)
16. **Windows Simulator** - To evaluate FreeRTOS without using MCU-based hardware, you can use the Windows Simulator.
    * Requirements: Microsoft Windows 7 or newer, with at least a dual core and a hard-wired Ethernet connection
    * [Getting Started Guide](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_windows.html)
    * IDE: [Visual Studio Community Edition](https://www.visualstudio.com/downloads/)


## amazon-freeRTOS/projects
The ```./projects``` folder contains the IDE test and demo projects for each vendor and their boards. The majority of boards can be built with both IDE and cmake (there are some exceptions!). Please refer to the Getting Started Guides above for board specific instructions.
