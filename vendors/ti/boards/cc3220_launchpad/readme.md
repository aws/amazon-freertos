aws_tests
=========

## Introduction

The aws_tests project is used to dispatch various test cases on the LaunchPad
in order to verify the correctness of TI's port.

## Prerequisites


### Setting up FreeRTOS Repo

Make sure submodules are initialized before starting.
Refer to the top level readme in `amazon-freertos`.

Specifically run the following command if not already run:

`git submodule update --init --recursive`

### Setting up CMake

The project binaries are built via CMake.
In order to run CMake, it must be installed and part of the user's `PATH`
variable. Dependencies must also be installed and on `PATH` with the execption
of Uniflash which runs only in GUI mode.

The dependencies are listed below:

1. [CMake 3.17.4 - Binary Distribution](https://cmake.org/download/)
   - Put `<CMAKE_INSTALL>/bin` on `PATH`
1. (Windows users)[MinGW 8.1.4](https://chocolatey.org/packages/mingw)
   - Chocolatey should put this on Path at `C:\ProgramData\chocolatey\lib\mingw\tools\install\mingw64\bin`
   - This step is only required for Windows users.

   TODO, change to TI-ARM
1. [ARM GCC 9.2019.q4.major](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads/9-2019-q4-major)

   - Put `<ARM_GCC_INSTALL>/bin` on `PATH`
1. [Uniflash v6.1.0](https://www.ti.com/tool/UNIFLASH)


If a debug environment is required the compilers may also be found as
part of Code Composer Studio [CCS](https://www.ti.com/tool/CCSTUDIO). If the
compiler is installed via CCS, they must be put on the `PATH` variable.
Version 10.1.1 is recommended. In CCS, compilers are installed to
`CCS_INSTALL_LOC/ccs/tools/compiler`. Where `CCS_INSTALL_LOC` is the location
that CCS is installed.

### Installing SDK dependencies

These steps should be done when changing SimpleLink SDKs.
The tested version is 4.30.00.xx where xx is an incrementing build number.

1. Install CC32XX SDK and SysConfig from TI.com. Please see top level TI readme
   for links to each SDK package and SysConfig.
1. Symlink the SDK from its install location (e.g. `C:\ti` or `/home/ti`) to
   `<AFR_REPO>/vendors/ti/simplelink_cc32xx_sdk/<CC32XX_SDK_VER>`

   - `<AFR_REPO>` is the location of the amazon-freertos repo
   - `<CC32XX_SDK_VER>` is the version of the CC32xx SDK, underscore
     separated. For example `4.30.00.06` --> `4_30_00_06`
1. Symlink sysconfig from its install location (e.g. `C:\ti` or `/home/ti`) to
   `<AFR_REPO>/vendors/ti/sysconfig/<SYSCONFIG_VER>/`
   - `<SYSCONFIG_VER>` is the version of the Sysconfig tool.
     For example use  `sysconfig_1.6.0` --> `1_6_0`

Note: as an alternative to symlinks, the contents of the sysconfig and
cc32xx SDK installers may be copied into their associated location within
the `vendors/ti` folder.

Note 2: Windows users may use powershell to create a symbolic
link (symlink). Please refer to
[this article](https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.management/new-item?view=powershell-7)
for further instructions. The following commands can be used to create the
necessary symlinks for sysconfig and the SDK. Note this assumes powershell as
admin and that the TI content is installed to `C:\ti` which is default.

```
New-Item -ItemType SymbolicLink -Path "<AFR_REPO>\vendors\ti\sysconfig\1_6_0" -Target "C:\ti\sysconfig_1.6.0"
```
and for the SDK
```
New-Item -ItemType SymbolicLink -Path "<AFR_REPO>\vendors\ti\simplelink_cc32xx_sdk\4_30_00_06" -Target "C:\ti\simplelink_cc32xx_sdk_4_30_00_06"
```

## Building

CMake is a two step process. The first step is generating the build files
This can be done with the following command:

Note: if you are on a system that supports unix makefiles, you may switch from
the MinGW Makefiles generator (`-G`). See [CMake Generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)
for more information.

This script should be run at the root of the amazon-freertos repo.

```
cmake -DVENDOR=ti -DBOARD=CC3220SF_LAUNCHXL -DCOMPILER=arm-ti -DCMAKE_BUILD_TYPE=debug -S . -B build -G"MinGW Makefiles" -DAFR_ENABLE_DEMOS=0 -DAFR_ENABLE_TESTS=1 -DCMAKE_EXPORT_COMPILE_COMMANDS=1  -DCC32XX_SDK_VER=4_30_00_06 -DSYSCONFIG_VER=1_6_0
```

Note that the variable `CC32XX_SDK_VER=4_30_00_06` must be set to the same
as created by the symlink in the dependency setup steps. Similiarly,
`SYSCONFIG_VER=1_6_0` must be set to the same as created by the symlink in the
dependency setup steps.

After the configuration is complete, the binary can be built using

```
cmake --build build/
```

The output is located at `build/aws_tests.out`. To load the output using Uniflash,
a binary version first has to be generated. Assuming `TI_CCS_PATH` was defined
during the CMake step, a binary version `aws_tests.bin` should also be available
in the `build` directory.

## Hardware setup

The aws_test project when testing common-io is setup to run on a LaunchPad using
the following hardware setup:

### GPIO:

P53, P63 and P50 assigned to GPIO index 0, 1 and 2 respectively.

Mount jumper to connect P53 and P63.

### I2C:

SCL at P1 and SDA at P2.

The vendor/ti/simplelink_common/common_io/CC3220SF_LAUNCXHL/iot_test_common_io_internal.c
file specifies the device address and register to verify functionality against.

### SPI:

No device needed, tests are currently single sided only.

### UART:

Uses UCA0 module for testing,

RX at P4 and TX at P3.

Mount jumper to connect P4 and P3 to support loopback test.

The AFQP_IotUARTWriteAsyncReadAsyncLoopbackTest test case may fail as we are
not able to capture in flight byte count without making it device specific.
This means it will return 0 until the TX operation is done.

### PWM:

Uses PWM0. PWM IO set to P64 to support measuring of frequency via the GPIO
driver. Jumper mounted to connect P64 and P50.

## Running the tests

Once the binary is flashed the tests will run by default. To view the results,
open a serial terminal like PuTTY and open the COM port assigned to the
launchpad:

1. Open [PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html)
   terminal using the following settings:

     - Serial line: COM port of user/UART port of LaunchPad
     - Baudrate : 115200
     - Terminal -> check boxes for "Implicit CR in every LF" and
       "Implicit LF in every CR"

2. Test results from the DUT will be printed in PuTTY.
