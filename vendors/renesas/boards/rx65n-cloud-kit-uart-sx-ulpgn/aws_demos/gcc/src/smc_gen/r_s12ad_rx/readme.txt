PLEASE REFER TO THE APPLICATION NOTE FOR THIS DRIVER FOR MORE INFORMATION

r_s12ad_rx
==========

Overview
--------------------------------------------------------------------------------
The r_s12ad_rx module is an A/D driver for the S12AD peripherals. The API 
includes functions to initialize the peripheral, trigger conversions, check for
conversion completion, and read the results. The driver supports all channels
and sensors available on the mcu. The driver can be reduced in size by removing 
the code used for parameter checking. The configuration option for this can be 
found in "r_config\r_s12ad_rx_config.h". An original copy of the configuration 
file is stored in "r_s12ad_rx\ref\r_s12ad_rx_config_reference.h".


Features
--------
* Operating modes
  - Single scan mode
  - Continuous scan mode
  - Group scan mode
  - Group scan mode (when group priority control selected)
* Conditions for A/D conversion start
  - Software trigger
  - Synchronous trigger
  - Asynchronous trigger
* Functions
  - Channel-dedicated sample-and-hold function
  - Variable sampling state count
  - Self-diagnosis
  - Selectable A/D-converted value addition mode or average mode
  - Analog input disconnection detection assist function
  - Double trigger mode
  - 12-/10-/8-bit conversion switching
  - Automatic clear function of A/D data registers
  - Extended analog input
  - Comparison function
  - Order of channel conversion in each unit can be set
  - Input signal amplification function of the programmable gain amplifier

NOTE: Available functions are different for each MCU.


File Structure
--------------
r_s12ad_rx
|   readme.txt
|   r_s12ad_rx_if.h
|
+---doc
|    +---en
|    |    +---r01an1666ej{VERSION_NUMBER}-rx-adc.pdf
|    |
|    +---ja
|         +---r01an1666jj{VERSION_NUMBER}-rx-adc.pdf
|
+---ref
|    +---r_s12ad_rx_config_reference.h
|
+---src
     +---r_s12ad_rx.c
     +---r_s12ad_rx_private.h
     |
     +---targets
          +--- rx110
          |     +-- r_s12ad_rx110.c
          |     +-- r_s12ad_rx110_if.h
          |
          +--- rx111
          |     :
          :

r_config
    r_s12ad_rx_config.h

