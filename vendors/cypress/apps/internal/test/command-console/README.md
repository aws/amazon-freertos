# Command Console
### Overview
The command console framework provides an interface for the user to interact and execute various commands from the host PC via UART on the device.

### Features
- It enables a utility/module to register a set of commands, argument list, help and a handler for each command (as a command table).
- The command console forwards the response from the libraries' handlers back to the host running on the PC over UART.
- The command console framework currently supports the iperf , test_console utility and bt_utility.

### Dependencies
- [Cypress Middleware Error codes](https://github.com/cypresssemiconductorco/connectivity-utilities/blob/master/cy_result_mw.h)

### Steps to run command console application

#### Make root directory
```
mkdir <dev>
cd <dev>
```
#### Clone AFR
```
cd <dev>
git clone git@git-ore.aus.cypress.com:repo/amazon-freertos.git
```
#### Specify Wi-Fi/Soft-AP credentials for running the Iperf utility
- Specify the Wi-Fi SSID and Password in the file ```vendors/cypress/apps/internal/test/command-console/include/aws_clientcredential.h```
- Specify the Soft-AP SSID and Password in the file ```vendors/cypress/apps/internal/test/command-console/config_files/aws_wifi_config.h```
#### Build makefiles using CMake
```
cd <dev>/amazon-freertos
cmake -DBLE_SUPPORTED=1 -DVENDOR=cypress -DCOMPILER=arm-gcc -DBOARD=CY8CPROTO_062_4343W -DAPP=vendors/cypress/apps/internal/test/command-console -S . -B build -G Ninja
```
#### Build the application
```
cmake --build build
```
The ```command-console.elf``` file will be created inside the build folder.
#### Program the ELF/HEX file
- Program the ```command-console.elf``` file using Cypress Programmer
     ##### OR
- Convert ELF file to HEX file
    * ```arm-none-eabi-objcopy -O ihex ./build/command-console.elf command-console.hex```
- Flash HEX file to the board
    * ```pyocd flash -e chip command-console.hex```

### Test the command-console framework
- Open a serial terminal emulator program like TeraTerm, Putty etc with ```baud rate = 115200```.
- Press the reset button on the device to start the application.
- Type ```help``` to see list of modules/utilities available.

### Additional Information
- [Command Console Framework](https://confluence.cypress.com/display/MIDDLEWARE/Command+Console+framework)
