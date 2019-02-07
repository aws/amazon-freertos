PLEASE REFER TO THE APPLICATION NOTE FOR THIS DRIVER FOR MORE INFORMATION

r_s12ad_rx
==========

Overview
--------------------------------------------------------------------------------
The r_s12ad_rx module is an A/D driver for the S12ADa/b/C/E/Fa peripherals. The API 
includes functions to initialize the peripheral, trigger conversions, check for
conversion completion, and read the results. The driver supports all channels
and sensors available on the mcu. The driver can be reduced in size by removing 
the code used for parameter checking. The configuration option for this can be 
found in "r_config\r_s12ad_rx_config.h". An original copy of the configuration 
file is stored in "r_s12ad_rx\ref\r_s12ad_rx_config_reference.h".


Features
--------
* Single Scans and Continuous Scanning.
* Software, asynchronous, and synchronous triggers.
* Group and Double Trigger Modes (on applicable MCUs).
* Automatic addition of multiple samples.

RX64M/RX71M
* Dual unit support.
* Ability to scan channels and sensors simultaneously (unit 1).
* Built-in comparator with level and window threshold checking.
* 8, 10 and 12-bit resolution selectable.
* Dedicated 3-channel simultaneous Sample & Hold (unit 0).
* External amplifier support for up to 8 channels (unit 1).
* Disconnection Detection Assist and Self-Diagnosis.
* Ability for Group A to interrupt scanning of Group B


File Structure
--------------
r_s12ad_rx
|   readme.txt
|   r_s12ad_rx_if.h
|
+---doc
|    |
|    +---en
|    |    |
|    |    +---r01an1666ej0230-rx.pdf
|    |
|    +---ja
|         |
|         +---r01an1666jj0230-rx.pdf
|
+---ref
|       r_s12ad_rx_config_reference.h
|
+---src
    +-- r_s12ad_rx.c
    +-- r_s12ad_rx_private.h
    |
    +-- targets
        +-- rx110
        |   +-- r_s12ad_rx110.c
        |   +-- r_s12ad_rx110_if.h
        |
        +-- rx111
        |   +-- r_s12ad_rx111.c
        |   +-- r_s12ad_rx111_if.h
        |
        +-- rx113
        |   +-- r_s12ad_rx113.c
        |   +-- r_s12ad_rx113_if.h
        |
        +-- rx130
        |   +-- r_s12ad_rx130.c
        |   +-- r_s12ad_rx130_if.h
        |
        +-- rx210
        |   +-- r_s12ad_rx210.c
        |   +-- r_s12ad_rx210_if.h
        |
        +-- rx230
        |   +-- r_s12ad_rx230.c
        |   +-- r_s12ad_rx230_if.h
        |
        +-- rx231
        |   +-- r_s12ad_rx231.c
        |   +-- r_s12ad_rx231_if.h
        |
        +-- rx63x
        |   +-- r_s12ad_rx63x.c
        |   +-- r_s12ad_rx63x_if.h
        |
        +-- rx64m
        |   +-- r_s12ad_rx64m.c
        |   +-- r_s12ad_rx64m_if.h
        |        
        +-- rx65x
        |   +-- r_s12ad_rx65x.c
        |   +-- r_s12ad_rx65x_if.h
        |
        +-- rx71m
            +-- r_s12ad_rx71m.c
            +-- r_s12ad_rx71m_if.h
   
r_config
    r_s12ad_rx_config.h

