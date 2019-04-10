## File Information

### reset.tcl
* This file is used to send commands to xsct to connect to the Microzed development board through JTAG and reset and run 
the current application that is in flash.  
* Run command:  
```xsct reset.tcl```

### aws_demos/bootimage/aws_demos.bif
* Used to create BOOT.bin for booting and running the aws_demos application on the Microzed development board from flash.  
* BOOT.bin is created with the current working directory as the AFR_ROOT with the commands:  
```bootgen -image demos/xilinx/microzed/xsdk/aws_demos.bif -o i demos/xilinx/microzed/xsdk/BOOT.bin```
