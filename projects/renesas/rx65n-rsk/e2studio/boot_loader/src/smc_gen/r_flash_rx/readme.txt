PLEASE REFER TO THE APPLICATION NOTE FOR THIS MODULE FOR MORE INFORMATION

r_flash_rx
=================

Document Number 
---------------
r01an2184ej0400
r01an2184jj0400

Version
-------
v4.00

Overview
--------
A simple Application Program Interface (API) has been created to allow users of
flash based RX700, RX600, RX200 and RX100 Series devices to easily integrate 
reprogramming abilities into their applications using User Mode programming. 
User Mode programming is the term used to describe a Renesas MCU's ability to 
reprogram its own internal flash memory while running in its normal operational 
mode. 

Features
--------
* Data Flash and ROM Erase
* Data Flash and ROM Blank Check
* Data Flash and ROM Write
* Supports background operations (BGO) on ROM and data flash.
* ROM Area Protection support
* ROM Start-Up Program Protection support
* ROM Cache

Supported MCUs
--------------
* RX110 Group
* RX111 Group
* RX113 Group
* RX130 Group
* RX230 Group
* RX231 Group
* RX23T Group
* RX24T Group
* RX24U Group
* RX64M Group
* RX651, RX65N Group
* RX66T Group
* RX71M Group
* RX72T Group


Boards Tested On
----------------
* RSKRX110
* RSKRX111
* RSKRX113
* RSKRX130
* RSKRX231
* RSKRX23T
* RSKRX24T
* RSKRX24U
* RSKRX64M
* RSKRX65N
* RSKRX65N-2
* RSKRX66T
* RSKRX71M
* RSKRX72T


Limitations
-----------
* This code is not re-entrant and protects against multiple concurrent
    function calls.
* During ROM operations neither ROM nor dataflash (DF) can be accessed. 
    If using ROM with BGO, then make sure the code runs from RAM.
* During DF operations, the DF cannot be accessed but ROM can be accessed 
    normally.

Peripherals Used Directly
-------------------------
* Flash Control Unit (FCU) - RX200, RX600, and RX700


Required Packages
-----------------
* r_bsp      V5.00

How to add to your project
--------------------------
* Add the r_flash_rx and r_config folders to your project.

* Add a project include path for the 'r_flash_rx' directory. 
* Add a project include path for the 'r_flash_rx\src' directory.
* Add a project include path for the 'r_flash_rx\src\targets' directory.
* Add a project include path for the 'r_flash_rx\src\flash_type_x' directory,
    where "x" is 1, 3, or 4 (see app note for MCU flash type assignments).
    Adding all 3 flash type paths will not cause a build issue.
* Add a project include path for the 'r_config' directory.
* Copy the reference configuration file from the '\r_flash_rx\ref' directory 
    into the 'r_config' directory and rename it to 'r_flash_rx_config.h'.
* Open "r_config\r_flash_rx_config.h" file and configure the driver for your 
  project.
* Add a #include for r_flash_rx_if.h to any source files that need to use the 
  API functions.

* (The following steps are only required if you are programming or erasing ROM.
    If you are only operating on data flash, then these steps can be ignored. 
    These steps are discussed with more detail in the app note.)
* Make a linker ROM section named 'PFRAM'.
* Make a linker RAM section named 'RPFRAM'.
* Configure your linker such that code in the 'FRAM' section will be executed 
    in RAM (see app note PFRAM=RPFRAM in linker output).


Toolchain(s) Used
-----------------
* Renesas RXC 3.01.00
* GNU GNURX 4.08.04.201801
* IAR ICCRX 4.10.01


File Structure
--------------
r_flash_rx
|   +-- readme.txt
|   +-- r_flash_rx_if.h
|
+---doc
|   +-- en
|   |   +-- r01an2184ej0400-rx-flash.pdf
|   +-- ja
|       +-- r01an2184jj0400-rx-flash.pdf
|
+---ref
|   +-- r_flash_rx_config_reference.h
|
+---src
    +-- r_flash_rx.c
    +-- r_flash_rx.h
    +-- r_flash_group.c
    +-- r_flash_group.h
    +-- r_flash_fcu.c
    +-- r_flash_fcu.h
    |
    +-- flash_type_1
        |
        +-- r_codeflash_extra.c
        +-- r_codeflash_extra.h
        +-- r_codeflash.c
        +-- r_codeflash.h
        +-- r_dataflash.c
        +-- r_dataflash.h
        +-- r_flash_common.h 
        +-- r_flash_type1.c
        +-- r_flash_type1_if.h
        +-- r_flash_utils.c 
    |
    +-- flash_type_3
        |
        +--r_flash_type3.c
        +--r_flash_type3_if.h
    |
    +-- flash_type_4
        |
        +--r_flash_type4.c
        +--r_flash_type4_if.h
    |
    +-- targets
        |
        +--rx110
             +-- r_flash_rx110.h
        |
        +--rx111
             +-- r_flash_rx111.h
        |
        +--rx113
             +-- r_flash_rx113.h
        |
        +--rx130
             +-- r_flash_rx130.h
        |
        +--rx231
             +-- r_flash_rx231.h
        |
        +--rx23t
             +-- r_flash_rx23t.h
        |
        +--rx24t
             +-- r_flash_rx24t.h
        |
        +--rx24u
             +-- r_flash_rx24u.h
        |
        +--rx64m
             +-- r_flash_rx64m.h
        |
        +--rx65n
             +-- r_flash_rx65n.h
        |
        +--rx66t
             +-- r_flash_rx66t.h
        |
        +--rx71m
             +-- r_flash_rx71m.h
        |    
        +--rx72t
             +-- r_flash_rx72t.h

r_config
    r_flash_rx_config.h

