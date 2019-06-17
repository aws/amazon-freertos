PLEASE REFER TO THE APPLICATION NOTE FOR THIS DRIVER FOR MORE INFORMATION

r_cmt_rx
=========

Overview
--------
This module creates a timer tick using a CMT channel based on a frequency input by the user.


Features
--------
* Create periodic or one-shot timer easily by passing in desired frequency/period
* User is alerted through callback function
* CMT channels are allocated dynamically.


File Structure
--------------
r_cmt_rx
|   readme.txt
|   r_cmt_rx_if.h
|
+---doc
|   |
|   +---en
|   |   r01an1856ej{VERSION_NUMBER}-rx.pdf
|   |
|   |
|   +---ja
|       r01an1856jj{VERSION_NUMBER}-rx.pdf
|
+---ref
|       r_cmt_rx_config_reference.h
|
\---src
        r_cmt_rx.c
        
r_config
    r_cmt_rx_config.h
            
