## File Information

### reset.tcl
* This file is used to send commands to xsct to connect to the Microzed development board through JTAG and reset and run 
the current application that is in flash.  
* Run command:  
```xsct reset.tcl```

### aws_tests/bootimage/aws_tests.bif
* Used to create BOOT.bin for booting and running the aws_tests application on the Microzed development board from flash.  
* We create BOOT.bin with the current working directory as the AFR_ROOT with the commands:  
```bootgen -image tests/xilinx/microzed/xsdk/aws_tests.bif -o i tests/xilinx/microzed/xsdk/BOOT.bin```
