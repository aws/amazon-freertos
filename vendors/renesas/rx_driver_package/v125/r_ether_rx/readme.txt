PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

r_ether_rx
==========

Document Number
---------------
R01AN2009EJ0120 (English version)
R01AN2009JJ0120 (Japanese version)

Version
-------
v1.20

Overview
--------
The Ethernet FIT module uses an Ethernet controller (ETHERC) and an Ethernet controller DMA controller (EDMAC)
to implement Ethernet/IEEE 802.3 frame transmission and reception. This module supports the following functions.
  MII (Media Independent Interface) and RMII (Reduced Media Independent Interface)
  An automatic negotiating function is used for the Ethernet PHY-LSI link.
  The link state is detected using the link signals output by the Ethernet PHY-LSI.
  The result of the automatic negotiation is acquired from the Ethernet PHY-LSI and the connection mode (full or half
duplex, 10 or 100 Mbps transfer rate) is set in the ETHERC.
This driver is tested and works with the following toolchain:
  Renesas RX Toolchain v.3.01.00
  GCC for Renesas RX 4.8.4
  IAR C/C++ Compiler for Renesas RX 4.12.1
All API calls and their supporting interface definitions are located in r_ether_rx_if.h.

Features
--------
* Ethernet/IEEE 802.3 frame transmission and reception.

Supported MCUs
--------------
* RX65N Group
* RX64M Group
* RX71M Group
* RX72M Group
* RX72N Group
* RX66N Group

Boards Tested On
----------------
* RSKRX65N
* RSKRX65N_2MB
* RSKRX64M
* RSKRX71M
* RSKRX72M
* RSKRX72N

Limitations
-----------
* None

Peripherals Used Directly
-------------------------
* ETHERC
* EDMAC

Required Packages
-----------------
* r_bsp

How to add to your project
--------------------------
Please refer to the Adding Firmware Integration Technology Modules to Projects.
"r01an1723eu0121_rx.pdf/r01an1826ej0110_rx.pdf/r20an0451es0100-e2studio-sc.pdf"(Only English version)
"r01an1723ju0121_rx.pdf/r01an1826jj0110_rx.pdf/r20an0451js0100-e2studio-sc.pdf"(Only Japanese version)


Toolchain(s) Used
-----------------
* Renesas RX v3.01.00
* GCC for Renesas RX 4.8.4
* IAR C/C++ Compiler for Renesas RX 4.12.1

File Structure
--------------
r_ether_rx
|       readme.txt
|       r_ether_rx_if.h
|
+---doc
|   |
|   +---en
|   |   r01an2009ej0120-rx-ether.pdf
|   |
|   \---ja
|       r01an2009jj0120-rx-ether.pdf
|
+---ref
|       r_ether_config_reference.h
|
\---src
    |   r_ether_rx.c
    |   r_ether_rx_private.h
    |
    +---phy
    |        phy.c
    |        phy.h
    |
    \---targets
        |
        |
        +---rx64m
        |    r_ether_setting_rx64m.c
        |
        +---rx71m
        |    r_ether_setting_rx71m.c
        |
        +---rx65n
        |    r_ether_setting_rx65n.c
        |
        +---rx72m
        |    r_ether_setting_rx72m.c
        |
        +---rx72n
        |    r_ether_setting_rx72n.c
        |    
        \---rx66n
             r_ether_setting_rx66n.c
