HAL Directory - Purpose
===========================
This directory contains all the Hardware Abstraction Layer (HAL) files used to
adapt the upper levels of atca-ng and abstractions to physical hardware.

HAL contains physical implementations for I2C, SWI, SPI, UART and timers for
specific hardware platforms.

**Include just those HAL files you require based on platform type.**

Cryptoauthlib HAL Architecture
=============================================

Cryptoauthlib has several intermediate conceptual layers

1. The highest layer of cryptoauthlib (outside of integration APIS) that may be used with an application is the atcab_
   api functions. These are general purpose functions that present a simple and consistent crypto interface to the application
   regardless of the device being used.

2. calib_, talib_ APIs are the library functions behind atcab_ ones that generate the correct command packets and process the
   received responses. Device specific logic is handled by the library here
   
3. hal_ these functions perform the transmit/recieve of data for a given interface. These are split into sublayers
    * The HAL layer is the first hal layer that presents the interface expected by the higher level library. When using a native
    driver and no further interpretation is required this layer is all that is required.
    * The PHY layer if for hals that perform an interpretation or additional protocol logic. In this situation the HAL performs
    protocol interpretation while the phy performs the physical communication


### HAL and PHY Requirements

The hal and phy layers have the same construction. A hal or phy must have the following functions and their signatures

* ATCA_STATUS hal_<name>_init(ATCAIface iface, ATCAIfaceCfg *cfg);
* ATCA_STATUS hal_<name>_post_init(ATCAIface iface);
* ATCA_STATUS hal_<name>_send(ATCAIface iface, uint8_t address, uint8_t *txdata, int txlength);
* ATCA_STATUS hal_<name>_receive(ATCAIface iface, uint8_t address, uint8_t *rxdata, uint16_t *rxlength);
* ATCA_STATUS hal_<name>_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen);
* ATCA_STATUS hal_<name>_release(void *hal_data);

If the hal is a native driver no phy is required. See the tables below for which hal is required to be ported based on a configured
interface


CryptoAuthLib Supported HAL Layers
=============================================


| Device Interface  | Physical Interface   | HAL            | PHY       |
|-------------------|----------------------|----------------|-----------|
| i2c               | i2c                  | hal_i2c        |           |
|                   | gpio                 | hal_i2c_gpio   | hal_gpio  |
| spi               | spi                  | hal_spi        |           |
| swi               | uart                 | hal_swi        | hal_uart  |
|                   | gpio                 | hal_swi_gpio   | hal_gpio  |
| any               | uart                 | kit            | hal_uart  |
|                   | hid                  | kit            | hal_hid   |
|                   | any (user provided)  | kit_bridge     |           |


Microchip Harmony 3 for all PIC32 & ARM products - Use the Harmony 3 Configurator to generate and configure prjects
--------------------------------------------
Obtain library and configure using [Harmony 3](https://github.com/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/wiki)

| Interface  | Files                        | API         | Notes                                           |
|------------|------------------------------|-------------|-------------------------------------------------|
|   I2C      | hal_i2c_harmony.c            | plib.h      |  For all Harmony 3 based projects               |
|   SPI      | hal_spi_harmony.c            | plib.h      |                                                 |
|   UART     | hal_uart_harmony.c           | plib.h      |                                                 }

Microchip 8 & 16 bit products - AVR, PIC16/18, PIC24/DSPIC
--------------------------------------------
Obtain library and integration through [Microchip Code Configurator](https://www.microchip.com/mplab/mplab-code-configurator)


OS & RTOS integrations
--------------------------------------------
Use [CMake](https://cmake.org/download/) to configure the library in Linux, Windows, and MacOS environments

| OS             | Interface  | Files                            | API         | Notes                              |
|----------------|------------|----------------------------------|-------------|------------------------------------|
| Linux          |    I2C     | hal_linux_i2c_userspace.c/h      | i2c-dev     |                                    |
| Linux          |    SPI     | hal_linux_spi_userspace.c/h      | spidev      |                                    |
| Linux/Mac      |            | hal_linux.c                      |             | For all Linux/Mac projects         |
| Windows        |            | hal_windows.c                    |             | For all Windows projects
| All            |  kit-hid   | hal_all_platforms_kit_hidapi.c/h | hidapi      | Works for Windows, Linux, and Mac  |
| freeRTOS       |            | hal_freertos.c                   |             | freeRTOS common routines           |


Legacy Support - [Atmel START](https://www.microchip.com/start) for AVR, ARM based processesors (SAM)
---------------------------------------------

| Interface  | Files                        | API         | Notes                              |
|------------|------------------------------|-------------|------------------------------------|
|            | hal_timer_start.c            | START       | Timer implementation               |
|   I2C      | hal_i2c_start.c/h            | START       |                                    |
|   SWI      | swi_uart_start.c/h           | START       | SWI using UART                     |


Legacy Support - ASF3 for ARM Cortex-m0 & Cortex-m based processors (SAM)
---------------------------------------------

|SAM Micros      | Interface  | Files                        | API         | Notes                              |
|----------------|------------|------------------------------|-------------|------------------------------------|
| cortex-m0      |   I2C      | hal_sam0_i2c_asf.c/h         | ASF3        | SAMD21, SAMB11, etc                |
| cortex-m3/4/7  |   I2C      | hal_sam_i2c_asf.c/h          | ASF3        | SAM4S, SAMG55, SAMV71, etc         |
| all            |            | hal_sam_timer_asf.c          | ASF3        | Common timer hal for all platforms |


@ingroup hal_
