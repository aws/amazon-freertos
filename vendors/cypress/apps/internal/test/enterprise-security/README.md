# Enterprise security commmand console app
### Overview
This application provides an interface for the user to interact and execute various enterprise security commands from the host PC via UART on the device.

### Steps to build and run this application

#### Generate makefiles using CMake
```
cd <dev>/amazon-freertos
cmake -DBLE_SUPPORTED=1 -DVENDOR=cypress -DCOMPILER=arm-gcc -DBOARD=CY8CPROTO_062_4343W -DAPP=vendors/cypress/apps/internal/test/enterprise-security -S . -B build/ -G Ninja
```
#### Build the application
```
cmake --build build
```
The ```enterprise-security.elf``` file will be created inside the build folder.
#### Program the ELF/HEX file
- Program the ```enterprise-security.elf``` file using Cypress Programmer
     ##### OR
- Convert ELF file to HEX file
    * ```arm-none-eabi-objcopy -O ihex ./build/enterprise-security.elf enterprise-security.hex```
- Flash HEX file to the board
    * ```pyocd flash -e chip enterprise-security.elf```

### Running the command
- Open a serial terminal emulator program like TeraTerm, Putty etc with ```baud rate = 115200```.
- Press the reset button on the device to start the application.
- Type ```help``` to see list of commands available.
