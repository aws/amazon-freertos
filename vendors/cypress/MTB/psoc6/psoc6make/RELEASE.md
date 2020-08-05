# PSoC 6 GNU make Build System Release Notes
This repo provides the build recipe make files and scripts for building and programming PSoC 6 applications. Builds can be run either through a command-line interface (CLI) or through the ModusToolbox Integrated Devlopment Environment (IDE).

### What's Included?
The this release of the PSoC 6 GNU make Build System includes complete support for building, programming, and debugging PSoC 6 application projects. It is expected that a code example contains a top level make file for itself and references a Board Support Package (BSP) that defines specifics items, like the PSoC 6 part, for the target board. Supported functionality includes the following:
* Supported operations:
    * Build
    * Program
    * Debug
    * IDE Integration (Eclipse, VS Code, IAR)
    * BSP Generation
* Supported toolchains:
    * GCC
    * IAR
    * ARMv6

This also includes the getlibs.bash script that can be used directly, or via the make target to download additional git repo based libraries for the application.

### What Changed?
#### v1.4.0
* TBD
#### v1.3.0
* Separated build process into two stages to improve robustness
* Added support for building dependent apps for CM0+ & MCU Boot
* Added support for exporting to uvision5
#### v1.2.1
* Added support for PSoC 64 S3 parts
* Improved description for some targets
#### v1.2.0
* Added support for generating new BSPs
* Added support for launching Library Manager
* Added support for integrating with Embedded Workbench (beta)
* Improved support for integrating with Eclipse
* Improved file discovery performance
#### v1.1.0
* Improved support for Multi-Core boards
* Minor Bug fixes
#### v1.0.1
* Minor improvement for vscode target
#### v1.0.0
* Initial release supporting build/program/debug on gcc/iar/armv6 toolchains

### Product/Asset Specific Instructions
Builds require that the ModusToolbox tools be installed on your machine. This comes with the ModusToolbox install. On Windows machines, it is recommended that CLI builds be executed using the Cygwin.bat located in ModusToolBox/tools_x.y/modus-shell install directory. This guarantees a consistent shell environment for your builds.

To list the build options, run the "help" target by typing "make help" in CLI. For a verbose documentation on a specific subject type "make help CY_HELP={variable/target}", where "variable" or "target" is one of the listed make variables or targets.

### Supported Software and Tools
This version of the Cypress PSoC 6 Hardware Abstraction Layer was validated for compatibility with the following Software and Tools:

| Software and Tools                        | Version |
| :---                                      | :----:  |
| ModusToolbox Software Environment         | 2.0     |
| GCC Compiler                              | 7.4     |
| IAR Compiler                              | 8.32    |
| ARM Compiler                              | 6.11    |

### More information
Use the following links for more information, as needed:
* [Cypress Semiconductor](http://www.cypress.com)
* [Cypress Semiconductor GitHub](https://github.com/cypresssemiconductorco)
* [ModusToolbox](https://www.cypress.com/products/modustoolbox-software-environment)

---
© Cypress Semiconductor Corporation, 2019-2020.