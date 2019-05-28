PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

r_sci_iic_rx
=========

Document Number
---------------
R01AN1691EJ0220 (English version)
R01AN1691JJ0220 (Japanese version)

Version
-------
v2.20

Overview
--------
The SCI simple I2C mode fit module provides a method to transmit and receive data between the master and slave devices using the serial communications interface (SCI). The SCI simple I2C mode is in compliance with single master mode of the XP I2C-bus (Inter-IC-Bus) interface.  

The driver supports three channels available on the mcu. 
The driver can be reducedin size by removing code used for parameter checking orfor unused channels. 
These configuration options can be found in "r_config\r_sci_iic_rx_config.h". 
An original copy of the configuration file is stored in 
"r_sci_iic_rx\ref\r_sci_iic_rx_config_reference.h".

Features
--------
* Single master mode (slave transmission or slave reception is not supported).
* Circuit to generate waveforms for conditions.
* Communication mode can be normal or fast mode and the maximum communication rate is 384 kbps.

Supported MCUs
--------------
* RX110 , RX111 Group
* RX113 Group
* RX130 Group
* RX230 , RX231 Group
* RX23T Group
* RX24T Group
* RX24U Group
* RX63N Group
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
* RSKRX63N
* RSKRX64M
* RSKRX65N
* RSKRX65N-2MB
* RSKRX71M

Limitations
-----------
None

Peripherals Used Directly
-------------------------
* SCIc, d, e, f, g, h or i

Required Packages
-----------------
* r_bsp     v2.30 or higher required for RX111, RX63N
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
* r_bsp     v3.60 or higher required for RX130-512KB, RX65N-2MB

How to add to your project
--------------------------
The driver must be added to an existing e2 Studio project.
It is best to use the e2Studio FIT plugin to add the driver to your project
as that will automatically update the include file paths for you. 
Alternatively,the driver can be imported from the archive that
accompanies this application note and manually added by following these steps:

1. This application note is distributed with a zip file package
   that includes the FIT Simple IIC support module in its own folder r_sci_iic_rx.
2. Unzip the package into the location of your choice.
3. In a file browser window, browse to the directory where you unzipped the 
   distribution package and locate the r_sci_iic_rx folder.
4. Open your e2 Studio workspace.
5. In the e2 Studio project explorer window, select the project 
   that you want to add the simple IIC module to.
6. Drag and drop the r_sci_iic_rx folder from the browser window (orcopy/paste)
   into your e2 Studio project at the top level of the project.
7. Update the source search/include paths for your project by adding the paths 
   to the module files:
   a. Navigate to the "Add directory path" control:
      i. 'project name'->properties->C/C++ Build->Settings->Compiler->Source -Add (green + icon)
   b. Add the following paths:
      i. "${workspace_loc:/${ProjName}/r_sci_iic_rx}"
     ii. "${workspace_loc:/${ProjName}/r_sci_iic_rx/src}"

Whether you used the plug-in or manually added the package to your project,
it is necessary to configure the driver for yourapplication.

8. Locate the r_sci_iic_rx_config_reference.h file in the r_sci_iic_rx/ref/targets/sci_e_f source folder
   in your project and copy it to your project's r_config folder.
9. Change the name of the copy in the r_config folder to r_sci_iic_rx_config.h
10. Make the required configuration settings by editing the copied
    r_sci_iic_rx_config.h file. See Configuration Overview.

The simple IIC module uses the r_bsp and r_cgc packages for certain MCU information
and support functions. The r_bsp package is easily configured through the platform.h header file 
which is located in the r_bsp folder. To configure the r_bsppackage, open up platform.h 
and uncomment the #include for the board you are using.
For example, to run the demo on a RSKRX111 board, the user would uncomment the #include for 
Åe./board/rskrx111/r_bsp.hÅf macro and make sure all other board #includes are commented out.

Toolchain(s) Used
-----------------
* Renesas RX v2.07.00

File Structure
--------------
r_sci_iic_rx
|   readme.txt
|   r_sci_iic_rx_if.h
|
+---doc
|   |
|   +---en
|   |   r01an1691ej0220-rx.pdf
|   |
|   +---ja
|       r01an1691jj0220-rx.pdf
|
+---ref
|       r_sci_iic_rx_config_reference.h
|       r_sci_iic_rx_pin_config_reference.h
|
+---src
    |   r_sci_iic_rx.c
    |   r_sci_iic_rx_platform.h
    |   r_sci_iic_rx_private.h
    |
    +---targets
        |
        +---rx110
        |       r_sci_iic_rx110.c
        |       r_sci_iic_rx110_private.h
        |
        +---rx111
        |       r_sci_iic_rx111.c
        |       r_sci_iic_rx111_private.h
        |
        +---rx113
        |       r_sci_iic_rx113.c
        |       r_sci_iic_rx113_private.h
        |
        +---rx130
        |       r_sci_iic_rx130.c
        |       r_sci_iic_rx130_private.h
        |
        +---rx230
        |       r_sci_iic_rx230.c
        |       r_sci_iic_rx230_private.h
        |
        +---rx231
        |       r_sci_iic_rx231.c
        |       r_sci_iic_rx231_private.h
        |
        +---rx23t
        |       r_sci_iic_rx23t.c
        |       r_sci_iic_rx23t_private.h
        |
        +---rx24t
        |        r_sci_iic_rx24t.c
        |        r_sci_iic_rx24t_private.h
        |
        +---rx24u
        |        r_sci_iic_rx24u.c
        |        r_sci_iic_rx24u_private.h
        |
        +---rx63n
        |       r_sci_iic_rx63n.c
        |       r_sci_iic_rx63n_private.h
        |
        +---rx64m
        |       r_sci_iic_rx64m.c
        |       r_sci_iic_rx64m_private.h
        |
        +---rx65n
        |       r_sci_iic_rx65n.c
        |       r_sci_iic_rx651_private.h
        |
        +---rx71m
                r_sci_iic_rx71m.c
                r_sci_iic_rx71m_private.h

r_config
    r_sci_iic_rx_config.h
    r_sci_iic_rx_pin_config.h

