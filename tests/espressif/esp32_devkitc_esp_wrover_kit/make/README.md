# Setup toolchain and serial communication driver

* Install the **pyserial** package for Python.

* Download the msys toolchain for **Windows**: https://dl.espressif.com/dl/esp32_win32_msys2_environment_and_toolchain-20180110.zip  
  msys32 will come with the Xtensa toolchain under **[install-dir]\msys32\opt\xtensa-esp32-elf\bin**.  
  The code must be built in a 32 bit GNU envirnment. 
    
  Download the Xtensa toolchain for **MAC OS**: https://dl.espressif.com/dl/xtensa-esp32-elf-osx-1.22.0-80-g6c4433a-5.2.0.tar.gz

* if your machine doesn't detect the board, install the VPC COM port driver : https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers  
  After installation you should see **Silicon Labs CP210x USB to UART Bridge (COM*N*)** in the device manager under the *Ports (COM & LPT)* device list.

# Setup Serial Communication Port:

1. Open mingw32.exe from the msys installation on Windows, or just a regular terminal on a UNIX based OS.
2. Navigate to this this directory (.../espressif/esp32_devkitc_esp_wrover_kit/make).
3. Run `make menuconfig` to configure the serial port. A graphical interface will build then appear, the *Espressif IDF Configuration*. 
4. In that menu, navigate to **Serial flasher config** > **Default serial port**, then press enter to configure the serial port.
5. Change default port from dev/ttyusb0 to COM*N* (Check the *Device Manager* for the enumeration). 
6. Say "yes" to saving the config when prompted on exit, so you don't have to enter it again.
7. We can update the serial configuration anytime by running `make menuconfig`.

The **sdkconfig** file will be modified after the above steps.

# Basic Commands:

- Build and display the *Espressif IDF Configuration* only:   
  `make menuconfig`
    
- Build only:  
  `make`  
    
- Build and flash the project:  
  `make flash`  
    
- Build, flash, and display the output in the terminal:  
  `make flash monitor`  
  
# Debugging:

## Debugging Setup:

1. You will need a JTAG to USB cable. We use a *USB to MPSSE cable*, for example the *FTDI C232HM-DDHSL-0* http://www.ftdichip.com/Products/Cables/USBMPSSE.htm.  
  
   ### **Windows Setup:**
   The FTDI C232HM-DDHSL-0 device will show up in the *Device Manager* under the Universal Serial Bus Controllers list.  
   Click on the device (manufacturer is FTDI) to get the properties. If you can not find this device you can install the drivers here: http://www.ftdichip.com/Drivers/D2XX.htm.  
   In the ***Details*** tab view the ***Hardware Ids*** property.
   
   This ID is typically: **USB\VID_0403&PID_6014**

   You may need to update the Hardware ID of the *USB to MPSSE cable* detected by your system in **esp32_devkitj_v1.cfg**, which is this line:

   `ftdi_vid_pid 0x0403 0x6014`

   For the FTDI C232HM-DDHSL-0 cable the connection to the ESP32 DevkitC are as follow:

   | C232HM-DDHSL-0 Wire Color | ESP32 GPIO Pin | JTAG Signal Name |
   |:--------------------------|:---------------|:-----------------|
   | Brown (pin 5)             | IO14           | TMS              |
   | Yellow (pin 3)            | IO12           | TDI              |
   | Black (pin 10)            | GND            | GND              |
   | Orange (pin 2)            | IO13           | TCK              |
   | Green (pin 4)             | IO15           | TDO              |

   The table above was developed from the device datasheet: http://www.ftdichip.com/Support/Documents/DataSheets/Cables/DS_C232HM_MPSSE_CABLE.PDF, see section *C232HM MPSSE Cable c onnection and Mechanical Details*.

2. Download **openOCD** for windows: https://github.com/espressif/openocd-esp32/releases/download/v0.10.0-esp32-20180418/openocd-esp32-win32-0.10.0-esp32-20180418.zip
   
   From the .zip downloaded, copy the **openocd-esp32/** directory somewhere and and add **[copy-location]\openocd-esp32\bin** to your PATH.

3. OpenOCD uses libusb which is availible on UNIX operating systems, but not on Windows.  
   In order to use openOCD on Windows you will need to convert the USB to MPSSE cable driver to be libusb to be compatible.  
  
   1. Download zadig.exe: https://zadig.akeo.ie/

   2. Run zadig.exe, select to **Options** > **List All Devices**.

   3. Select the *C232HM-DDHSL-0* device in the drop-down menu.

   4. Select *WinUSB* from on the othersie of the green array.

   5. In the selection box under the above option, make sure *Replace Driver* is selected and press it.

## Debugging Instruction:

1. From any terminal, navigate to this current directory (espressif/esp32_devkitc_esp_wrover_kit/make) and run:  
  `openocd.exe -f esp32_devkitj_v1.cfg -f esp-wroom-32.cfg`  
  Leave this terminal open.  
  
2. Open mingw32.exe Windows GNU terminal and run:  
  `make flash monitor`  
  
3. Open another new terminal and run:  
  `xtensa-esp32-elf-gdb -x gdbinit build/aws_tests.elf`  
  The program should stop in main.

Note: ESP32 has a maximum of 2 hardware break points.
