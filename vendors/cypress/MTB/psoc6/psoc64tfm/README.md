# TFM release package for PSoC64 board

### Overview

TFM release package for PSoC64 dual-core MCU, including pre-built TFM secure 
image for CM0 and TFM non-secure interface code for CM4. An optional test 
library is included for building TFM regression test. 

### Contents

* [COMPONENT_TFM_S_FW]

    TFM secure core CM0 image in hex format.

* [COMPONENT_TFM_NS_INTERFACE]

    TFM non-secure interface code for CM4
    CM4 memory map
        Type   Start Address  Size
        ------+--------------+-----------
        Flash  0x1005,0000    0x0012,0000
        RAM    0x0803,0000    0x000B,7000

* [COMPONENT_TFM_TEST]

    TFM test library to run TFM regression test.
        tfm_secure_client_run_tests()
        tfm_non_secure_client_run_tests()

  [COMPONENT_TFM_NS_FREERTOS_WRAPPER]

    TFM OS wrapper functions implemented for FreeRTOS

### More information
Use the following links for more information, as needed:
* [Cypress](http://www.cypress.com)
* [ModusToolbox](https://www.cypress.com/products/modustoolbox-software-environment)

---
Copyright (c) Cypress Semiconductor Corporation, 2019.
