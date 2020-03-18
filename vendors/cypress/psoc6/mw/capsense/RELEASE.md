# Cypress CapSense Middleware Library 2.0

### What's Included?
Please refer to the [README.md](./README.md) and the [API Reference Guide](https://cypresssemiconductorco.github.io/capsense/capsense_api_reference_manual/html/index.html) for a complete description of the CapSense Middleware.
The revision history of the CapSense Middleware is also available on the [API Reference Guide Changelog](https://cypresssemiconductorco.github.io/capsense/capsense_api_reference_manual/html/index.html#section_capsense_changelog).
New in this release:
* Optimized flash memory consumption based on user's configuration	
* Added the memory usage section
* Added the errata section

### Known Issues
| Problem | Workaround |
| ------- | ---------- |
| GPIO simultaneous operation with unrestricted strength and frequency creates noise that can affect CapSense operation | Refer to the errata section of the device datasheet for details |

### Defect Fixes
* Renamed function Cy_CapSense_CheckCommandIntegrity() to Cy_CapSense_CheckTunerCmdIntegrity()
* Fixing a compilation error for non CapSense-capable devices: CapSense MW sources are enclosed with the conditional compilation

### Supported Software and Tools
This version of the CapSense Middleware was validated for compatibility with the following Software and Tools:

| Software and Tools                                      | Version |
| :---                                                    | :----:  |
| ModusToolbox Software Environment                       | 2.0     |
| - ModusToolbox Device Configurator                      | 2.0     |
| - ModusToolbox CSD Personality in Device Configurator   | 2.0     |
| - ModusToolbox CapSense Configurator / Tuner tools      | 2.0     |
| PSoC6 Peripheral Driver Library (PDL)                   | 1.2.0   |
| GCC Compiler                                            | 7.2.1   |
| IAR Compiler                                            | 8.32    |
| ARM Compiler 6                                          | 6.11    |
| MBED OS                                                 | 5.13.1  |
| FreeRTOS                                                | 10.0.1  |

### More information
The following resources contain more information:
* [CapSense Middleware README.md](./README.md)
* [CapSense Middleware API Reference Guide](https://cypresssemiconductorco.github.io/capsense/capsense_api_reference_manual/html/index.html)
* [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.cypress.com/products/modustoolbox-software-environment)
* [CapSense Middleware Code Example for MBED OS](https://github.com/cypresssemiconductorco/mbed-os-example-capsense)
* [CapSense Middleware Code Example for FreeRTOS](https://www.cypress.com/documentation/code-examples/ce218136-psoc-6-mcu-e-ink-display-capsense-rtos)
* [CapSense Middleware Code Examples at GITHUB](https://github.com/cypresssemiconductorco)
* [ModusToolbox CapSense Configurator Tool Guide](https://www.cypress.com/ModusToolboxCapSenseConfig)
* [ModusToolbox CapSense Tuner Tool Guide](https://www.cypress.com/ModusToolboxCapSenseTuner)
* [ModusToolbox Device Configurator Tool Guide](https://www.cypress.com/ModusToolboxDeviceConfig)
* [CapSense Design Guide](https://www.cypress.com/documentation/application-notes/an85951-psoc-4-and-psoc-6-mcu-capsense-design-guide)
* [CSDADC Middleware API Reference Guide](https://cypresssemiconductorco.github.io/csdadc/csdadc_api_reference_manual/html/index.html)
* [CSDIDAC Middleware API Reference Guide](https://cypresssemiconductorco.github.io/csdidac/csdidac_api_reference_manual/html/index.html)
* [AN210781 Getting Started with PSoC 6 MCU with Bluetooth Low Energy (BLE) Connectivity](http://www.cypress.com/an210781)
* [PSoC 6 Technical Reference Manual](https://www.cypress.com/documentation/technical-reference-manuals/psoc-6-mcu-psoc-63-ble-architecture-technical-reference)
* [PSoC 63 with BLE Datasheet Programmable System-on-Chip datasheet](http://www.cypress.com/ds218787)
* [Cypress Semiconductor](http://www.cypress.com)
  
---
© Cypress Semiconductor Corporation, 2019.