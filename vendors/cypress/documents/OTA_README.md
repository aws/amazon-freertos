# AWS IoT and FreeRTOS: Over-the-Air Update

This release supports OTA firmware update using FreeRTOS for PSoC® 62 MCUs with 2M internal flash and CYW43xxx connectivity devices.

Current revision: OTA V2.1.1

## Supported Kits

- [PSoC 6 Wi-Fi BT Prototyping Kit](https://www.cypress.com/CY8CPROTO-062-4343W) (CY8CPROTO-062-4343W)
- [PSoC 62S2 Wi-Fi BT Pioneer Kit](https://www.cypress.com/CY8CKIT-062S2-43012) (CY8CKIT-062S2-43012)
- [PSoC® 64 Standard Secure - AWS Wi-Fi BT Pioneer Kit ](https://www.cypress.com/documentation/development-kitsboards/psoc-64-standard-secure-aws-wi-fi-bt-pioneer-kit-cy8ckit) (CY8CKIT-064S0S2-4343W)

Only PSoC® 62 MCU kits with 2 MB of Internal flash are supported at this time.

PSoC® 62 MCU kit-specific things that need to be changed:

- Flash locations and sizes for Bootloader, Primary Slot (slot 0), and Secondary Slot (slot 1).
  - See further information in the Configure MCUBootApp section below.

## Features and Functionality

This library utilizes MQTT or HTTP and TLS to securely connect to AWS and download an update for the user application.

Once the application starts the OTA agent, the OTA agent will contact the MQTT Broker to see if there is an update available. If an update is available, it will be downloaded.

## Requirements

- [Eclipse IDE for ModusToolbox](https://www.cypress.com/products/modustoolbox-software-environment) v2.1 or later
- [MCUBoot](https://juullabs-oss.github.io/mcuboot/)
- Programming Language: C
- [Python 3.7 or higher](https://www.python.org/downloads//) (for signing script)
  - Run pip to install the modules.

```
pip install -r <repo root>/vendors/cypress/MTB/port_support/ota/scripts/requirements.txt
```

## Supported Toolchains

- GCC ARM Compiler
- ARM C Compiler
  - NOTE: When using the ARM Compiler, add the GCC compiler directory to your system PATH.
- IAR Compiler
  - NOTE: When using the IAR Compiler, add the GCC compiler directory to your system PATH.

## Supported RTOS

- Amazon FreeRTOS

## Hardware Setup

This example uses the board's default configuration. See the kit user guide to ensure the board is configured correctly.

**Note**: Before using this code example, make sure that the board is upgraded to KitProg3. The tool and instructions are available in the [Firmware Loader](https://github.com/cypresssemiconductorco/Firmware-loader) GitHub repository. If you do not upgrade, you will see an error like "unable to find CMSIS-DAP device" or "KitProg firmware is out of date".

## Software Setup

- Install a terminal emulator if you don't have one. Instructions in this document use [Tera Term](https://ttssh2.osdn.jp/index.html.en).
- Python Interpreter. The supplied publisher.py is tested with [Python 3.8.1](https://www.python.org/downloads/release/python-381/).

## Clone MCUBoot (PSoC® 62 MCU kits)

We need to first build and program the bootloader app *MCUBootApp* that is available in the MCUBoot GitHub repo, before programming this OTA app. The bootloader app is run by CM0+ while this OTA app is run by CM4. Clone the MCUBoot repository onto your local machine, **outside of your application directory.**

1. git clone https://github.com/JuulLabs-OSS/mcuboot.git

2. Open a CLI terminal and navigate to the cloned mcuboot folder: `cd mcuboot`

3. Change the branch to get the Cypress version: `git checkout v1.6.1-cypress`

4. We need to pull in mcuboot sub-modules to build mcuboot: `git submodule update --init --recursive`

5. Install the required python packages mentioned in *mcuboot\scripts\requirements.txt*.

   i. cd `mcuboot/scripts` 

   ii. `pip install -r requirements.txt`

## Configure MCUBootApp (PSoC® 62 MCU kits)

#### Adjust MCUBootApp RAM start in linker script

The default RAM location needs to be modified for use with OTA.

1. Edit *mcuboot/boot/cypress/MCUBootApp/MCUBootApp.ld*

   `Change at line 66 from:`

   `    ram               (rwx)   : ORIGIN = 0x08020000, LENGTH = 0x20000`

   To:

   `    ram               (rwx)   : ORIGIN = 0x08000000, LENGTH = 0x20000`

#### Adjust MCUBootApp Flash Locations

It is important for both MCUBootApp and the OTA application to have the exact same understanding of the memory layout. Otherwise, the bootloader may consider an authentic image as invalid. To learn more about the bootloader refer to the [MCUBoot](https://github.com/JuulLabs-OSS/mcuboot/blob/cypress/docs/design.md) documentation. 

#### Internal Flash for Secondary Slot

These values are used when the Secondary Slot is located in **internal** flash. Since internal flash is 2 MB, we need to have the bootloader and the Primary and Secondary slots fit. The Primary and Secondary slots are always the same size.

1. Edit *mcuboot/boot/cypress/MCUBootApp/MCUBootApp.mk*

   Change at line 31:

   ```
USE_EXTERNAL_FLASH ?= 0
   ```

   Replace line 55 with:

   ```
DEFINES_APP +=-DMCUBOOT_MAX_IMG_SECTORS=2000
   ```
   
   Add at line 56:
   
   ```
   DEFINES_APP +=-DCY_BOOT_PRIMARY_1_SIZE=0x0EE000
   DEFINES_APP +=-DCY_BOOT_SECONDARY_1_SIZE=0x0EE000
   ```
   
   

#### External Flash for Secondary Slot

These values are used when the Secondary Slot is located in **external** flash. This allows for a larger Primary Slot and hence, a larger application size. The Primary and Secondary slots are always the same size.

1. Edit *mcuboot/boot/cypress/MCUBootApp/MCUBootApp.mk*

   Change at line 31:

   ```
USE_EXTERNAL_FLASH ?= 1
   ```

   Replace line 55 with:
   
   ```
   DEFINES_APP +=-DMCUBOOT_MAX_IMG_SECTORS=3584
   ```
   
   Add at line 56:
   
   ```
   DEFINES_APP +=-DCY_BOOT_PRIMARY_1_SIZE=0x01c0000
   DEFINES_APP +=-DCY_BOOT_SECONDARY_1_SIZE=0x01c00=00
   ```

### Configure MCUBoot to Ignore Primary Slot Verify

The Primary Slot is where the application is run from and the OTA download is saved to the Secondary Slot. MCUBoot verifies the signature in the Secondary Slot before copying to Primary slot. Signature verify takes some time. Removing the verify for the Primary slot allows for a faster bring up of your application. 

Make the following changes in *mcuboot/boot/cypress/MCUBootApp/config/mcuboot_config/mcuboot_config.h* to disable signature verification for primary slot. 

1. Comment out lines 38 & 39

   `/* Uncomment for ECDSA signatures using curve P-256. */`

   `//#define MCUBOOT_SIGN_EC256`

   `//#define NUM_ECC_BYTES (256 / 8) 	// P-256 curve size in bytes, rnok: to make compilable`

2. Comment out line 78

   `//#define MCUBOOT_VALIDATE_PRIMARY_SLOT`

## Build and Program MCUBootApp (PSoC® 62 MCU kits)

### Build MCUBootApp

1. Ensure that the toolchain path is set for the compiler. Check that the path is correct for your installed version of ModusToolbox.

​		`export TOOLCHAIN_PATH=<path>/ModusToolbox/tools_2.1/gcc-7.2.1`

2. Build the Application

​		`cd mcuboot/boot/cypress`

​		`make app APP_NAME=MCUBootApp TARGET=CY8CPROTO-062-4343W`

​		Use the following command if you are using Cygwin: 

​		`make app APP_NAME=MCUBootApp PLATFORM=PSOC_062_2M CURDIR=$(cygpath --mixed $(pwd))`

### Program MCUBootApp

1. Connect the board to your PC using the provided USB cable through the KitProg3 USB connector.

2. Use Cypress Programmer to program MCUBootApp. Remember to close Cypress Programmer before trying to program the application using ModusToolbox or from the CLI. The MCUBootApp.elf file is at:

   ​		`mcuboot/boot/cypress/MCUBootApp/out/PSOC_062_2M/Debug/MCUBootApp.elf`

## Prepare for Building your OTA Application (PSoC® 62 MCU kits)

The Makefile and cmake flows both look for this environment variable to determine how to build the application:

`OTA_USE_EXTERNAL_FLASH`

The default for the build is to use external flash. If the environment variable is undefined or is non-zero, the application will build for external flash. The build system will use the values described above for the slot sizes automatically.

To use internal FLASH only, set the environment variable to zero

`> export OTA_USE_EXTERNAL_FLASH=0`

## Flash Partitioning (PSoC® 62 MCU kits)

OTA provides a way to update the system software. The OTA mechanism stores the new software to a "staging" area called the Secondary Slot.  MCUboot will do the actual update from the Secondary Slot to the Primary Slot on the next reboot of the device. In order for the OTA software and MCUBoot to have the same information on where the two Slots are in FLASH, we need to tell MCUBoot where the Slots are located. 

Secondary slot can be placed on either Internal or external flash. The start address when internal flash is used is an offset from the starting address of the internal flash 0x10000000.

Internal flash:

Primary Slot      (Slot 0): start: 0x018000, size: 0xEE000

Secondary Slot (Slot 1): start: 0x106000, size: 0xEE000

External flash:

Primary Slot      (Slot 0): start: 0x018000, size: 0x01c0000

Secondary Slot (Slot 1): start: 0x18000000, size: 0x01c0000

## Limitations
1. Be sure to have a reliable network connection before starting an OTA update. If your Network connection is poor, OTA update may fail due to lost packets or lost connection.

4. Be sure to have a fully charged device before starting an OTA update. If you device's battery is low, OTA may fail.

## Additional Information

- [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.cypress.com/products/modustoolbox-software-environment)
-  [MCUBoot](https://github.com/JuulLabs-OSS/mcuboot/blob/cypress/docs/design.md) documentation

Cypress also provides a wealth of data at www.cypress.com to help you select the right device, and quickly and effectively integrate it into your design.

For PSoC 6 MCU devices, see [How to Design with PSoC 6 MCU - KBA223067](https://community.cypress.com/docs/DOC-14644) in the Cypress community.

## Document History

| Document Version | Description of Change                                      |
| ---------------- | ---------------------------------------------------------- |
| 1.1.0            | Added info for ARM and IAR builds                          |
|                  | Updated for external FLASH support and CY8CKIT_062S2_43012 |
| 1.0.0            | New Document                                               |

------

All other trademarks or registered trademarks referenced herein are the property of their respective owners.

------

![Banner](Banner.png)

-------------------------------------------------------------------------------

© Cypress Semiconductor Corporation, 2020. This document is the property of Cypress Semiconductor Corporation and its subsidiaries ("Cypress"). This document, including any software or firmware included or referenced in this document ("Software"), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide. Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights. If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress's patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products. Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.<br/>
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. No computing device can be absolutely secure. Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product. CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, "Security Breach"). Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach. In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications. To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document. Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes. It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product. "High-Risk Device" means any device or system whose failure could cause personal injury, death, or property damage. Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices. "Critical Component" means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness. Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device. You shall indemnify and hold Cypress, its directors, officers, employees, agents, affiliates, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device. Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress's published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.<br/>
Cypress, the Cypress logo, Spansion, the Spansion logo, and combinations thereof, WICED, PSoC, CapSense, EZ-USB, F-RAM, and Traveo are trademarks or registered trademarks of Cypress in the United States and other countries. For a more complete list of Cypress trademarks, visit cypress.com. Other names and brands may be claimed as property of their respective owners.