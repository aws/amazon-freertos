# CY8CKIT-062S2-43012 BSP Release Notes
The CY8CKIT-062S2-43012 PSoC 6S2 Wi-Fi BT Pioneer Kit is a low-cost hardware platform that enables design and debug of PSoC 6 MCUs. It comes with a Murata 1LV Module (CYW43012 Wi-Fi + Bluetooth Combo Chip), industry-leading CapSense for touch buttons and slider, on-board debugger/programmer with KitProg3, microSD card interface, 512-Mb Quad-SPI NOR flash, PDM-PCM microphone interface.

NOTE: BSPs are versioned by family. This means that version 1.2.0 of any BSP in a family (eg: PSoC 6) will have the same software maturity level. However, not all updates are necessarily applicable for each BSP in the family so not all version numbers will exist for each board. Additionally, new BSPs may not start at version 1.0.0. In the event of adding a common feature across all BSPs, the libraries are assigned the same version number. For example if BSP_A is at v1.3.0 and BSP_B is at v1.2.0, the event will trigger a version update to v1.4.0 for both BSP_A and BSP_B. This allows the common feature to be tracked in a consistent way.

### What's Included?
The CY8CKIT-062S2-43012 library includes the following:
* BSP specific makefile to configure the build process for the board
* cybsp.c/h files to initialize the board and any system peripherals
* cybsp_types.h file describing basic board setup
* CM4 Linker script & startup code for GCC, IAR, ARM toolchains
* CM0+ Linker script & startup code for GCC, IAR, ARM toolchains
* Configurator design files (and generated code) to setup board specific peripherals
* .lib file references for all dependent libraries
* API documentation

### What Changed?
#### v1.3.0
* Minor update for documentation & branding
* Updated design files to use latest personality files
* Initialize VDDA voltage if set in configurator
NOTE: This requires psoc6hal 1.3.0 or later
#### v1.2.1
* Added 43012/4343W/43438 component to appropriate BSPs
* Added multi-image policy for secure (064) BSPs
#### v1.2.0
* Standardize version numbering for all boards in a family
* Moved UDB SDIO implementation into its own library udb-sdio-whd library
* Added call to setup HAL SysPM driver (requires HAL 1.2.0 or later)
* Updated documentation
NOTE: This requires psoc6hal 1.2.0 or later
#### v1.1.0
* Updated linker scripts and startup code for the CM0+ and CM4 cores. The files are now in core specific directories.
* Minor updates to avoid potential warnings on some toolchains
#### v1.0.1
* Added pin references for the board's J2 Header (for appropriate boards)
#### v1.0.0
* Initial release

### Supported Software and Tools
This version of the CY8CKIT-062S2-43012 BSP was validated for compatibility with the following Software and Tools:

| Software and Tools                        | Version |
| :---                                      | :----:  |
| ModusToolbox Software Environment         | 2.0     |
| GCC Compiler                              | 7.4     |
| IAR Compiler                              | 8.32    |
| ARM Compiler                              | 6.11    |

### More information
* [CY8CKIT-062S2-43012 BSP API Reference Manual][api]
* [CY8CKIT-062S2-43012 Documentation](http://www.cypress.com/CY8CKIT-062S2-43012)
* [Cypress Semiconductor, an Infineon Technologies Company](http://www.cypress.com)
* [Cypress Semiconductor GitHub](https://github.com/cypresssemiconductorco)
* [ModusToolbox](https://www.cypress.com/products/modustoolbox-software-environment)

[api]: modules.html

---
© Cypress Semiconductor Corporation, 2019-2020.