PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

r_riic_rx
=========

Document Number
---------------
R01AN1692EJ0220 (English version)
R01AN1692JJ0220 (Japanese version)

Version
-------
v2.20

Overview
--------
The RIIC fit module provides a method to transmit and receive data between 
the master and slave devices using the I2C-bus interface (RIIC). 
The RIIC is in compliance with single master mode of the XP I2C-bus (Inter-IC-Bus) interface.

The driver supports one channels available on the mcu. 
The driver can be reducedin size by removing code used for parameter checking orfor unused channels. 
These configuration options can be found in "r_config\r_riic_rx_config.h". 
An original copy of the configuration file is stored in 
"r_riic_rx\ref\r_riic_rx_config_reference.h".

Features
--------
* Master transmission, master reception, slave transmission, and slave reception.
* Multi-master configuration which is that communication is arbitrated between multiple masters and a single slave.
* Communication mode can be normal or fast mode and the maximum communication rate is 400 kbps at a maximum. 
  However channel 0 of RX64M and RX65N, RX71M supports fast mode plus and the maximum communication rate is 1 Mbps.

Supported MCUs
--------------
* RX110 , RX111 Group
* RX113 Group
* RX130 Group
* RX230 , RX231 Group
* RX23T Group
* RX24T Group
* RX24U Group
* RX64M Group
* RX65N Group
* RX71M Group

Boards Tested On
----------------
* RSKRX111
* RSKRX113
* RSKRX130
* RSKRX130-512KB
* RSKRX231
* RSKRX23T
* RSKRX24T
* RSKRX24U
* RSKRX64M
* RSKRX65N
* RSKRX65N-2MB
* RSKRX71M

Technical Update of applying
-----------
* Document No        : TN-RX*-A012A

Limitations
-----------
None

Peripherals Used Directly
-------------------------
* RIIC

Required Packages
-----------------
* r_bsp     v2.30 or higher required for RX111
* r_bsp     v2.50 or higher required for RX110
* r_bsp     v2.60 or higher required for RX64M
* r_bsp     v2.70 or higher required for RX113
* r_bsp     v2.80 or higher required for RX71M
* r_bsp     v2.90 or higher required for RX231
* r_bsp     v3.00 or higher required for RX23T
* r_bsp     v3.10 or higher required for RX130
* r_bsp     v3.20 or higher required for RX24T
* r_bsp     v3.30 or higher required for RX230
* r_bsp     v3.40 or higher required for RX65N
* r_bsp     v3.50 or higher required for RX24U
* r_bsp     v3.60 or higher required for RX65N-2MB and RX130-512KB

How to add to your project
--------------------------
The module must be added to an existing e2Studio project.
It is best to use the e2Studio FIT plug-in to add the FIT module to your project 
as that will automatically update the include file paths for you.
To add the FIT module using the plug-in, refer to
"2. Adding FIT Modules to e2 studio Projects Using FIT Plug-In"
in the application note "Adding Firmware Integration Technology Modules to Projects (R01AN1723)".
Alternatively, the FIT module can be added manually.
To add the FIT module manually, refer to 
"3. Adding FIT Modules to e2 studio Projects Manually"
in the application note "Adding Firmware Integration Technology Modules to Projects (R01AN1723)".
When using the FIT module, the BSP FIT module also needs to be added to the project.
For details on the BSP FIT module, refer to the application note 
"Board Support Package Module Using Firmware Integration Technology (R01AN1685)".

Toolchain(s) Used
-----------------
* Renesas RX v2.07.00 

File Structure
--------------
r_riic_rx
|   readme.txt
|   r_riic_rx_if.h
|
+---doc
|   |
|   +---en
|   |   r01an1692ej0220-rx.pdf
|   |
|   +---ja
|       r01an1692jj0220-rx.pdf
|
+---ref
|       r_riic_rx_config_reference.h
|       r_riic_rx_pin_config_reference.h
|
+---src
    |   r_riic_rx.c
    |   r_riic_rx_private.h
    |
    +---targets
        |
        +---rx110
        |      r_riic_rx110.c
        |      r_riic_rx110_private.h
        |
        +---rx111
        |      r_riic_rx111.c
        |      r_riic_rx111_private.h
        |
        +---rx113
        |      r_riic_rx113.c
        |      r_riic_rx113_private.h
        |
        +---rx130
        |      r_riic_rx130.c
        |      r_riic_rx130_private.h
        |
        +---rx230
        |      r_riic_rx230.c
        |      r_riic_rx230_private.h
        |
        +---rx231
        |      r_riic_rx231.c
        |      r_riic_rx231_private.h
        |
        +---rx23t
        |      r_riic_rx23t.c
        |      r_riic_rx23t_private.h
        |
        +---rx24t
        |      r_riic_rx24t.c
        |      r_riic_rx24t_private.h
        |
        +---rx24u
        |      r_riic_rx24u.c
        |      r_riic_rx24u_private.h
        |
        +---rx64m
        |      r_riic_rx64m.c
        |      r_riic_rx64m_private.h
        |
        +---rx65n
        |      r_riic_rx65n.c
        |      r_riic_rx65n_private.h
        |
        +---rx71m
               r_riic_rx71m.c
               r_riic_rx71m_private.h

r_config
    r_riic_rx_config.h
    r_riic_rx_pin_config.h
